ifeq ($(CONFIG_BOARD_XIAOMI_SM8250),y)
ifeq ($(CONFIG_BUILD_ARM64_DT_OVERLAY),y)

ifeq ($(CONFIG_MACH_XIAOMI_MUNCH),y)
dtbo-$(CONFIG_MACH_XIAOMI_MUNCH) += \
		munch-sm8250-overlay.dtbo
munch-sm8250-overlay.dtbo-base := kona-v2.1.dtb
endif

endif
endif

always		:= $(dtb-y)
subdir-y	:= $(dts-dirs)
clean-files    := *.dtb *.dtbo
