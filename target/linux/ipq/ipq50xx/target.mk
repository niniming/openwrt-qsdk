
SUBTARGET:=ipq50xx
BOARDNAME:=QCA IPQ50XX(32bit) based boards
CPU_TYPE:=cortex-a7

DEFAULT_PACKAGES += \
	fwupgrade-tools kmod-usb-phy-ipq5018 kmod-usb-dwc3-qcom

define Target/Description
	Build firmware image for IPQ50xx SoC devices.
endef
