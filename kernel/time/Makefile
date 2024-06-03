# SPDX-License-Identifier: GPL-2.0
obj-y += time.o timer.o hrtimer.o
obj-y += timekeeping.o ntp.o clocksource.o jiffies.o timer_list.o
obj-y += timeconv.o timecounter.o alarmtimer.o

ifeq ($(cc-name),clang)
KBUILD_CFLAGS	+= -mllvm -inline-threshold=7000
KBUILD_CFLAGS	+= -mllvm -inlinehint-threshold=6000
KBUILD_CFLAGS   += -mllvm -inlinehint-threshold=4200
else ifeq ($(cc-name),gcc)
KBUILD_CFLAGS	+= --param max-inline-insns-auto=500

# We limit inlining to 5KB on the stack.
KBUILD_CFLAGS	+= --param large-stack-frame=1288

KBUILD_CFLAGS	+= --param inline-min-speedup=5
KBUILD_CFLAGS	+= --param inline-unit-growth=60
endif

ifeq ($(CONFIG_POSIX_TIMERS),y)
 obj-y += posix-timers.o posix-cpu-timers.o posix-clock.o itimer.o
else
 obj-y += posix-stubs.o
endif

obj-$(CONFIG_GENERIC_CLOCKEVENTS)		+= clockevents.o tick-common.o
ifeq ($(CONFIG_GENERIC_CLOCKEVENTS_BROADCAST),y)
 obj-y						+= tick-broadcast.o
 obj-$(CONFIG_TICK_ONESHOT)			+= tick-broadcast-hrtimer.o
endif
obj-$(CONFIG_GENERIC_SCHED_CLOCK)		+= sched_clock.o
obj-$(CONFIG_TICK_ONESHOT)			+= tick-oneshot.o tick-sched.o
obj-$(CONFIG_HAVE_GENERIC_VDSO)			+= vsyscall.o
obj-$(CONFIG_DEBUG_FS)				+= timekeeping_debug.o
obj-$(CONFIG_TEST_UDELAY)			+= test_udelay.o
