# SPDX-License-Identifier: GPL-2.0

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

obj-y := irqdesc.o handle.o manage.o spurious.o resend.o chip.o dummychip.o devres.o
obj-$(CONFIG_IRQ_TIMINGS) += timings.o
obj-$(CONFIG_GENERIC_IRQ_CHIP) += generic-chip.o
obj-$(CONFIG_GENERIC_IRQ_PROBE) += autoprobe.o
obj-$(CONFIG_IRQ_DOMAIN) += irqdomain.o
obj-$(CONFIG_IRQ_SIM) += irq_sim.o
obj-$(CONFIG_PROC_FS) += proc.o
obj-$(CONFIG_GENERIC_PENDING_IRQ) += migration.o
obj-$(CONFIG_GENERIC_IRQ_MIGRATION) += cpuhotplug.o
obj-$(CONFIG_PM_SLEEP) += pm.o
obj-$(CONFIG_GENERIC_MSI_IRQ) += msi.o
obj-$(CONFIG_GENERIC_IRQ_IPI) += ipi.o
obj-$(CONFIG_SMP) += affinity.o
obj-$(CONFIG_GENERIC_IRQ_DEBUGFS) += debugfs.o
obj-$(CONFIG_GENERIC_IRQ_MATRIX_ALLOCATOR) += matrix.o
obj-$(CONFIG_IRQ_SBALANCE) += sbalance.o
