#
# Copyright (C) 2010-2016 Jo-Philipp Wich <jo@mein.io>
#
# This is free software, licensed under the GPL 2 license.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libiwinfo
PKG_RELEASE:=5

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=http://git.openwrt.org/project/iwinfo.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_DATE:=2021-06-28
PKG_SOURCE_VERSION:=c9b1672f
PKG_SOURCE:=$(PKG_NAME)-$(PKG_SOURCE_DATE)-$(PKG_SOURCE_VERSION).tar.xz
PKG_MAINTAINER:=Jo-Philipp Wich <jo@mein.io>
PKG_LICENSE:=GPL-2.0
PKG_BUILD_DIR=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

PKG_CONFIG_DEPENDS := \
	CONFIG_PACKAGE_kmod-brcm-wl \
	CONFIG_PACKAGE_kmod-brcm-wl-mini \
	CONFIG_PACKAGE_kmod-brcm-wl-mimo \
	CONFIG_PACKAGE_kmod-madwifi \
	CONFIG_PACKAGE_kmod-cfg80211 \
	CONFIG_PACKAGE_kmod-qca-wifi-unified-profile \
	CONFIG_PACKAGE_kmod-qca-wifi-custc-profile \
	CONFIG_PACKAGE_kmod-qca-wifi-unified-perf \
	CONFIG_PACKAGE_kmod-qca-wifi-lowmem-profile

IWINFO_ABI_VERSION:=20210430

PKG_BUILD_DEPENDS := \
	PACKAGE_kmod-qca-wifi:kmod-qca-wifi \
	PACKAGE_kmod-qca-wifi-unified-profile:kmod-qca-wifi-unified-profile \
	PACKAGE_kmod-qca-wifi-custc-profile:kmod-qca-wifi-custc-profile \
	PACKAGE_kmod-qca-wifi-unified-perf:kmod-qca-wifi-unified-perf \
	PACKAGE_kmod-qca-wifi-lowmem-profile:kmod-qca-wifi-lowmem-profile
include $(INCLUDE_DIR)/package.mk


define Package/libiwinfo
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Generalized Wireless Information Library (iwinfo)
  DEPENDS:=+PACKAGE_kmod-cfg80211:libnl-tiny +libuci +libubus
  ABI_VERSION:=$(IWINFO_ABI_VERSION)
endef

define Package/libiwinfo/description
  Wireless information library with consistent interface for proprietary Broadcom,
  madwifi, nl80211 and wext driver interfaces.
endef


define Package/libiwinfo-lua
  SUBMENU:=Lua
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=libiwinfo Lua binding
  DEPENDS:=+libiwinfo +liblua
endef

define Package/libiwinfo-lua/description
  This is the Lua binding for the iwinfo library. It provides access to all enabled
  backends.
endef


define Package/iwinfo
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Generalized Wireless Information utility
  DEPENDS:=+libiwinfo
endef

define Package/iwinfo/description
  Command line frontend for the wireless information library.
endef


define Build/Configure
endef

IWINFO_BACKENDS := \
	$(if $(CONFIG_PACKAGE_kmod-brcm-wl),wl) \
	$(if $(CONFIG_PACKAGE_kmod-brcm-wl-mini),wl) \
	$(if $(CONFIG_PACKAGE_kmod-brcm-wl-mimo),wl) \
	$(if $(CONFIG_PACKAGE_kmod-madwifi),madwifi) \
	$(if $(CONFIG_PACKAGE_kmod-cfg80211),nl80211) \
	$(if $(CONFIG_PACKAGE_kmod-qca-wifi-unified-profile),qcawifi) \
	$(if $(CONFIG_PACKAGE_kmod-qca-wifi-custc-profile),qcawifi) \
	$(if $(CONFIG_PACKAGE_kmod-qca-wifi-unified-perf),qcawifi) \
	$(if $(CONFIG_PACKAGE_kmod-qca-wifi-lowmem-profile),qcawifi)

TARGET_CFLAGS += \
	-I$(STAGING_DIR)/usr/include/libnl-tiny \
	-I$(STAGING_DIR)/usr/include \
	-D_GNU_SOURCE

MAKE_FLAGS += \
	FPIC="$(FPIC)" \
	CFLAGS="$(TARGET_CFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS)" \
	BACKENDS="$(IWINFO_BACKENDS)" \
	SOVERSION="$(IWINFO_ABI_VERSION)"

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/iwinfo
	$(CP) $(PKG_BUILD_DIR)/include/iwinfo.h $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/include/iwinfo/* $(1)/usr/include/iwinfo/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libiwinfo.so* $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/lua
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/iwinfo.so $(1)/usr/lib/lua/iwinfo.so
endef

define Package/libiwinfo/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/libiwinfo.so.$(IWINFO_ABI_VERSION) $(1)/usr/lib/libiwinfo.so.$(IWINFO_ABI_VERSION)
	$(INSTALL_DIR) $(1)/usr/share/libiwinfo
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/hardware.txt $(1)/usr/share/libiwinfo/hardware.txt
endef

define Package/libiwinfo-lua/install
	$(INSTALL_DIR) $(1)/usr/lib/lua
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/iwinfo.so $(1)/usr/lib/lua/iwinfo.so
endef

define Package/iwinfo/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/iwinfo $(1)/usr/bin/iwinfo
endef

$(eval $(call BuildPackage,libiwinfo))
$(eval $(call BuildPackage,libiwinfo-lua))
$(eval $(call BuildPackage,iwinfo))
