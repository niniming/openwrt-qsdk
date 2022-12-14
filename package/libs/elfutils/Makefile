#
# Copyright (C) 2010-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=elfutils
PKG_VERSION:=0.161
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://fedorahosted.org/releases/e/l/$(PKG_NAME)/$(PKG_VERSION)
PKG_MD5SUM:=e1b9847c9a6a1ad340de8d47a863ec52
PKG_MAINTAINER:=Luiz Angelo Daros de Luca <luizluca@gmail.com>
PKG_LICENSE:=GPL-3.0+
PKG_LICENSE_FILES:=COPYING COPYING-GPLV2 COPYING-LGPLV3

PKG_INSTALL:=1
PKG_USE_MIPS16:=0

PKG_BUILD_DEPENDS:=argp-standalone

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/elfutils/Default
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=$(INTL_DEPENDS)
  TITLE:=ELF manipulation libraries
  URL:=https://fedorahosted.org/elfutils/
endef

define Package/libasm
  $(call Package/elfutils/Default)
  TITLE+= (libasm)
  DEPENDS:=libelf1
endef

define Package/libdw
  $(call Package/elfutils/Default)
  DEPENDS:=libelf1 +zlib +libbz2
  TITLE+= (libdw)
endef

define Package/libelf1
  $(call Package/elfutils/Default)
  TITLE+= (libelf)
endef

ifeq ($(CONFIG_BUILD_NLS),y)
TARGET_LDFLAGS += "-lintl"
endif

CONFIGURE_VARS += \
	LIBS="-largp"

CONFIGURE_ARGS += \
	--disable-werror \
	--without-lzma

TARGET_CFLAGS += -Wno-misleading-indentation \
	-Wno-nonnull-compare \
	-Wno-implicit-fallthrough

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libasm/libasm.{a,so*} $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/libdw/libdw.{a,so*} $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/libelf/libelf.{a,so*} $(1)/usr/lib/
endef

define Package/libasm/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libasm/libasm.so* $(1)/usr/lib/
endef

define Package/libdw/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libdw/libdw.so* $(1)/usr/lib/
endef

define Package/libelf1/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libelf/libelf.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libasm))
$(eval $(call BuildPackage,libdw))
$(eval $(call BuildPackage,libelf1))
