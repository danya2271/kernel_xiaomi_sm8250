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
obj-$(CONFIG_TOUCHSCREEN_FOCALTECH_3658U) += focaltech_3658u.o
focaltech_3658u-y := focaltech_core.o \
                     focaltech_ex_fun.o \
                     focaltech_ex_mode.o \
                     focaltech_gesture.o \
                     focaltech_esdcheck.o \
                     focaltech_spi.o \
                     focaltech_point_report_check.o \
                     focaltech_flash.o \
                     focaltech_flash/focaltech_upgrade_ft3658u.o \
                     focaltech_test/focaltech_test.o \
                     focaltech_test/focaltech_test_ini.o \
                     focaltech_test/supported_ic/focaltech_test_ft3658u.o
