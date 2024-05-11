# SPDX-License-Identifier: GPL-2.0-only
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
obj-$(CONFIG_EROFS_FS) += erofs.o
erofs-objs := super.o inode.o data.o namei.o dir.o utils.o pcpubuf.o
erofs-$(CONFIG_EROFS_FS_XATTR) += xattr.o
erofs-$(CONFIG_EROFS_FS_ZIP) += decompressor.o zmap.o zdata.o
