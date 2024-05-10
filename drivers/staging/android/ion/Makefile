# SPDX-License-Identifier: GPL-2.0
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
obj-$(CONFIG_ION) +=	ion.o ion-ioctl.o ion_heap.o \
			ion_page_pool.o ion_system_heap.o \
			ion_carveout_heap.o ion_chunk_heap.o \
			ion_system_secure_heap.o ion_cma_heap.o \
			ion_camera_heap.o \
			ion_secure_util.o ion_cma_secure_heap.o msm/

