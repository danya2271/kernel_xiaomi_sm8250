# SPDX-License-Identifier: GPL-2.0
# Any varying coverage in these files is non-deterministic
# and is generally not a function of system call inputs.
KCOV_INSTRUMENT := n
ifeq ($(cc-name),clang)
KBUILD_CFLAGS	+= -mllvm -inline-threshold=7250
KBUILD_CFLAGS	+= -mllvm -inlinehint-threshold=9750
KBUILD_CFLAGS   += -mllvm -inlinehint-threshold=5750
else ifeq ($(cc-name),gcc)
KBUILD_CFLAGS	+= --param max-inline-insns-auto=500

# We limit inlining to 5KB on the stack.
KBUILD_CFLAGS	+= --param large-stack-frame=1288

KBUILD_CFLAGS	+= --param inline-min-speedup=5
KBUILD_CFLAGS	+= --param inline-unit-growth=60
endif
obj-y += update.o sync.o
obj-$(CONFIG_TREE_SRCU) += srcutree.o
obj-$(CONFIG_TINY_SRCU) += srcutiny.o
obj-$(CONFIG_RCU_TORTURE_TEST) += rcutorture.o
obj-$(CONFIG_RCU_PERF_TEST) += rcuperf.o
obj-$(CONFIG_TREE_RCU) += tree.o
obj-$(CONFIG_PREEMPT_RCU) += tree.o
obj-$(CONFIG_TINY_RCU) += tiny.o
obj-$(CONFIG_RCU_NEED_SEGCBLIST) += rcu_segcblist.o
