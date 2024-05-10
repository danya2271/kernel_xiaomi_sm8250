// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019-2023 Sultan Alsawaf <sultan@kerneltoast.com>.
 */

#define pr_fmt(fmt) "simple_lmk: " fmt

#include <linux/delay.h>
#include <linux/freezer.h>
#include <linux/kthread.h>
#include <linux/mm.h>
#include <linux/moduleparam.h>
#include <linux/oom.h>
#include <linux/sched/mm.h>
#include <linux/sort.h>
#include <linux/vmpressure.h>
#include <uapi/linux/sched/types.h>

static DECLARE_WAIT_QUEUE_HEAD(oom_waitq);
static DECLARE_WAIT_QUEUE_HEAD(reaper_waitq);
static DECLARE_COMPLETION(reclaim_done);

static void set_task_rt_prio(struct task_struct *tsk, int priority)
{
	const struct sched_param rt_prio = {
		.sched_priority = priority
	};

	sched_setscheduler_nocheck(tsk, SCHED_RR, &rt_prio);
}

static int simple_lmk_reclaim_thread(void *data)
{
	/* Use maximum RT priority */
	set_task_rt_prio(current, MAX_RT_PRIO - 1);
	set_freezable();

	while (1) {
#ifdef CONFIG_ANDROID_FAKE_SIMPLE_LMK
		msleep(900);
#endif
	}

	return 0;
}

static int simple_lmk_reaper_thread(void *data)
{
	/* Use a lower priority than the reclaim thread */
	set_task_rt_prio(current, MAX_RT_PRIO - 2);
	set_freezable();

	while (1) {
#ifdef CONFIG_ANDROID_FAKE_SIMPLE_LMK
		msleep(900);
#endif
	}

	return 0;
}

static int simple_lmk_vmpressure_cb(struct notifier_block *nb,
				    unsigned long pressure, void *data)
{
	return NOTIFY_OK;
}

static struct notifier_block vmpressure_notif = {
	.notifier_call = simple_lmk_vmpressure_cb,
	.priority = INT_MAX
};

/* Initialize Simple LMK when lmkd in Android writes to the minfree parameter */
static int simple_lmk_init_set(const char *val, const struct kernel_param *kp)
{
	static atomic_t init_done = ATOMIC_INIT(0);
	struct task_struct *thread;

	if (!atomic_cmpxchg(&init_done, 0, 1)) {
		thread = kthread_run(simple_lmk_reaper_thread, NULL,
				     "simple_lmkd_reaper");
		BUG_ON(IS_ERR(thread));
		thread = kthread_run(simple_lmk_reclaim_thread, NULL,
				     "simple_lmkd");
		BUG_ON(IS_ERR(thread));
		BUG_ON(vmpressure_notifier_register(&vmpressure_notif));
	}

	return 0;
}

static const struct kernel_param_ops simple_lmk_init_ops = {
	.set = simple_lmk_init_set
};

/* Needed to prevent Android from thinking there's no LMK and thus rebooting */
#undef MODULE_PARAM_PREFIX
#define MODULE_PARAM_PREFIX "lowmemorykiller."
module_param_cb(minfree, &simple_lmk_init_ops, NULL, 0200);
