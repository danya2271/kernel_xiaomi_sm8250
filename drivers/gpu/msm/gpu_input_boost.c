// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2018-2019 Sultan Alsawaf <sultan@kerneltoast.com>.
 * Copyright (C) 2024 danya2271 <danya2271@yandex.ru>.
 */

#define pr_fmt(fmt) "gpu_input_boost: " fmt

#include <linux/fb.h>
#include <linux/input.h>
#include <linux/kthread.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/version.h>
#include "gpu_input.h"

/* The sched_param struct is located elsewhere in newer kernels */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 10, 0)
#include <uapi/linux/sched/types.h>
#endif


enum {
    SCREEN_OFF,
    INPUT_BOOST,
    MAX_BOOST
};

struct boost_drv {
    struct delayed_work input_unboost;
    struct delayed_work max_unboost;
    struct notifier_block gpu_notif;
    struct notifier_block fb_notif;
    wait_queue_head_t boost_waitq;
    atomic_long_t max_boost_expires;
    unsigned long state;
};

void input_unboost_worker(struct work_struct *work);
void max_unboost_worker(struct work_struct *work);

struct boost_drv boost_drv_g __read_mostly = {
    .input_unboost = __DELAYED_WORK_INITIALIZER(boost_drv_g.input_unboost,
                                                input_unboost_worker, 0),
                                                .max_unboost = __DELAYED_WORK_INITIALIZER(boost_drv_g.max_unboost,
                                                                                          max_unboost_worker, 0),
                                                                                          .boost_waitq = __WAIT_QUEUE_HEAD_INITIALIZER(boost_drv_g.boost_waitq)
};

module_param(input_boost_level, uint, 0644);
module_param(input_boost_duration, short, 0644);
module_param(wake_boost_duration, short, 0644);

int boost_adjust_notify(void)
{
    struct boost_drv *b = &boost_drv_g;

    /* Boost CPU to max frequency for max boost */
    if (test_bit(MAX_BOOST, &b->state)) {
        return 2;
    }

    /*
     * Boost to policy->max if the boost frequency is higher. When
     * unboosting, set policy->min to the absolute min freq for the CPU.
     */
    if (test_bit(INPUT_BOOST, &b->state))
        return 1;

    return 0;
}

static void __gpu_input_boost_kick(struct boost_drv *b)
{
    if (test_bit(SCREEN_OFF, &b->state))
        return;

    if (!input_boost_duration)
        return;

    set_bit(INPUT_BOOST, &b->state);
    if (!mod_delayed_work(system_unbound_wq, &b->input_unboost,
        msecs_to_jiffies(input_boost_duration)))
        wake_up(&b->boost_waitq);
}

void gpu_input_boost_kick(void)
{
    struct boost_drv *b = &boost_drv_g;

    __gpu_input_boost_kick(b);
}

static void __gpu_input_boost_kick_max(struct boost_drv *b,
                                       unsigned int duration_ms)
{
    unsigned long boost_jiffies = msecs_to_jiffies(duration_ms);
    unsigned long curr_expires, new_expires;

    if (test_bit(SCREEN_OFF, &b->state))
        return;

    do {
        curr_expires = atomic_long_read(&b->max_boost_expires);
        new_expires = jiffies + boost_jiffies;

        /* Skip this boost if there's a longer boost in effect */
        if (time_after(curr_expires, new_expires))
            return;
    } while (atomic_long_cmpxchg(&b->max_boost_expires, curr_expires,
                                 new_expires) != curr_expires);

    set_bit(MAX_BOOST, &b->state);
    if (!mod_delayed_work(system_unbound_wq, &b->max_unboost,
        boost_jiffies))
        wake_up(&b->boost_waitq);
}

void gpu_input_boost_kick_max(unsigned int duration_ms)
{
    struct boost_drv *b = &boost_drv_g;

    __gpu_input_boost_kick_max(b, duration_ms);
}

void input_unboost_worker(struct work_struct *work)
{
    struct boost_drv *b = container_of(to_delayed_work(work),
                                       typeof(*b), input_unboost);

    clear_bit(INPUT_BOOST, &b->state);
    wake_up(&b->boost_waitq);
}

void max_unboost_worker(struct work_struct *work)
{
    struct boost_drv *b = container_of(to_delayed_work(work),
                                       typeof(*b), max_unboost);

    clear_bit(MAX_BOOST, &b->state);
    wake_up(&b->boost_waitq);
}

static int gpu_boost_thread(void *data)
{
    static const struct sched_param sched_max_rt_prio = {
        .sched_priority = MAX_RT_PRIO - 1
    };
    struct boost_drv *b = data;
    unsigned long old_state = 0;

    sched_setscheduler_nocheck(current, SCHED_FIFO, &sched_max_rt_prio);

    while (1) {
        bool should_stop = false;
        unsigned long curr_state;

        wait_event(b->boost_waitq,
                   (curr_state = READ_ONCE(b->state)) != old_state ||
                   (should_stop = kthread_should_stop()));

        if (should_stop)
            break;

        old_state = curr_state;
    }

    return 0;
}

static int fb_notifier_cb(struct notifier_block *nb,
                          unsigned long action, void *data)
{
    struct boost_drv *b = container_of(nb, typeof(*b), fb_notif);
    struct fb_event *evdata = data;
    int *blank = evdata->data;

    if (action != FB_EVENT_BLANK)
        return NOTIFY_OK;

    /* Boost when the screen turns on and unboost when it turns off */
    if (*blank == FB_BLANK_UNBLANK) {
        clear_bit(SCREEN_OFF, &b->state);
        __gpu_input_boost_kick_max(b, wake_boost_duration);
    }

    return NOTIFY_OK;
}

static void gpu_input_boost_input_event(struct input_handle *handle,
                                        unsigned int type, unsigned int code,
                                        int value)
{
    struct boost_drv *b = handle->handler->private;

    __gpu_input_boost_kick(b);
}

static int gpu_input_boost_input_connect(struct input_handler *handler,
                                         struct input_dev *dev,
                                         const struct input_device_id *id)
{
    struct input_handle *handle;
    int ret;

    handle = kzalloc(sizeof(*handle), GFP_KERNEL);
    if (!handle)
        return -ENOMEM;

    handle->dev = dev;
    handle->handler = handler;
    handle->name = "gpu_input_boost_handle";

    ret = input_register_handle(handle);
    if (ret)
        goto free_handle;

    ret = input_open_device(handle);
    if (ret)
        goto unregister_handle;

    return 0;

    unregister_handle:
    input_unregister_handle(handle);
    free_handle:
    kfree(handle);
    return ret;
}

static void gpu_input_boost_input_disconnect(struct input_handle *handle)
{
    input_close_device(handle);
    input_unregister_handle(handle);
    kfree(handle);
}

static const struct input_device_id gpu_input_boost_ids[] = {
    /* Multi-touch touchscreen */
    {
        .flags = INPUT_DEVICE_ID_MATCH_EVBIT |
        INPUT_DEVICE_ID_MATCH_ABSBIT,
        .evbit = { BIT_MASK(EV_ABS) },
        .absbit = { [BIT_WORD(ABS_MT_POSITION_X)] =
            BIT_MASK(ABS_MT_POSITION_X) |
            BIT_MASK(ABS_MT_POSITION_Y) }
    },
    /* Touchpad */
    {
        .flags = INPUT_DEVICE_ID_MATCH_KEYBIT |
        INPUT_DEVICE_ID_MATCH_ABSBIT,
        .keybit = { [BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH) },
        .absbit = { [BIT_WORD(ABS_X)] =
            BIT_MASK(ABS_X) | BIT_MASK(ABS_Y) }
    },
    /* Keypad */
    {
        .flags = INPUT_DEVICE_ID_MATCH_EVBIT,
        .evbit = { BIT_MASK(EV_KEY) }
    },
    { }
};

static struct input_handler gpu_input_boost_input_handler = {
    .event		= gpu_input_boost_input_event,
    .connect	= gpu_input_boost_input_connect,
    .disconnect	= gpu_input_boost_input_disconnect,
    .name		= "gpu_input_boost_handler",
    .id_table	= gpu_input_boost_ids
};

static int __init gpu_input_boost_init(void)
{
    struct boost_drv *b = &boost_drv_g;
    struct task_struct *thread;
    int ret;

    gpu_input_boost_input_handler.private = b;
    ret = input_register_handler(&gpu_input_boost_input_handler);
    if (ret) {
        pr_err("Failed to register input handler, err: %d\n", ret);
    }

    b->fb_notif.notifier_call = fb_notifier_cb;
    b->fb_notif.priority = INT_MAX;
    ret = fb_register_client(&b->fb_notif);
    if (ret) {
        pr_err("Failed to register fb notifier, err: %d\n", ret);
        goto unregister_handler;
    }

    thread = kthread_run(gpu_boost_thread, b, "gpu_boostd");
    if (IS_ERR(thread)) {
        ret = PTR_ERR(thread);
        pr_err("Failed to start gpu boost thread, err: %d\n", ret);
        goto unregister_fb_notif;
    }

    return 0;

    unregister_fb_notif:
    fb_unregister_client(&b->fb_notif);
    unregister_handler:
    input_unregister_handler(&gpu_input_boost_input_handler);
    return ret;
}
late_initcall(gpu_input_boost_init);
