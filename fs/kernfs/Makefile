#
# Makefile for the kernfs pseudo filesystem
#
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
obj-y		:= mount.o inode.o dir.o file.o symlink.o
