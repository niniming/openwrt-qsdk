# 
# Copyright (C) 2006-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/host.mk
include $(INCLUDE_DIR)/version.mk
include $(INCLUDE_DIR)/feeds.mk

override MAKEFLAGS=

IB_NAME:=OpenWrt-ImageBuilder-$(if $(CONFIG_VERSION_FILENAMES),$(VERSION_NUMBER)-)$(BOARD)$(if $(SUBTARGET),-$(SUBTARGET)).$(HOST_OS)-$(HOST_ARCH)
PKG_BUILD_DIR:=$(BUILD_DIR)/$(IB_NAME)
IB_KDIR:=$(patsubst $(TOPDIR)/%,$(PKG_BUILD_DIR)/%,$(KERNEL_BUILD_DIR))
IB_LDIR:=$(patsubst $(TOPDIR)/%,$(PKG_BUILD_DIR)/%,$(LINUX_DIR))
IB_DTSDIR:=$(patsubst $(TOPDIR)/%,$(PKG_BUILD_DIR)/%,$(LINUX_DIR))/arch/$(LINUX_KARCH)/boot/dts/
IB_EXT_KDIR:=$(patsubst $(TOPDIR)/%,$(PKG_BUILD_DIR)/%,$(CONFIG_EXTERNAL_KERNEL_TREE))

all: compile

$(BIN_DIR)/$(IB_NAME).tar.bz2: clean
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(IB_KDIR) $(IB_LDIR) $(PKG_BUILD_DIR)/staging_dir/host/lib \
		$(PKG_BUILD_DIR)/target $(PKG_BUILD_DIR)/scripts $(IB_DTSDIR)
	-cp $(TOPDIR)/.config $(PKG_BUILD_DIR)/.config
	$(CP) \
		$(INCLUDE_DIR) $(SCRIPT_DIR) \
		$(TOPDIR)/rules.mk \
		./files/Makefile \
		./files/repositories.conf \
		$(TMP_DIR)/.targetinfo \
		$(TMP_DIR)/.packageinfo \
		$(PKG_BUILD_DIR)/

ifeq ($(CONFIG_IB_STANDALONE),)
	echo '## Remote package repositories' >> $(PKG_BUILD_DIR)/repositories.conf
	$(call FeedSourcesAppend,$(PKG_BUILD_DIR)/repositories.conf)
endif

	echo ''                                                        >> $(PKG_BUILD_DIR)/repositories.conf
	echo '## This is the local package repository, do not remove!' >> $(PKG_BUILD_DIR)/repositories.conf
	echo 'src imagebuilder file:packages'                          >> $(PKG_BUILD_DIR)/repositories.conf

	$(VERSION_SED) $(PKG_BUILD_DIR)/repositories.conf

ifeq ($(CONFIG_IB_STANDALONE),)
	(cd $(PACKAGE_DIR); $(FIND) -type f -name 'libc_*.ipk' -or -name 'kernel_*.ipk' -or -name 'kmod-*.ipk') | \
		while read path; do \
			mkdir -p "$(PKG_BUILD_DIR)/packages/$${path%/*}"; \
			cp "$(PACKAGE_DIR)/$$path" "$(PKG_BUILD_DIR)/packages/$$path"; \
		done
else
	$(CP) $(PACKAGE_DIR) $(PKG_BUILD_DIR)/packages
endif

	$(CP) $(TOPDIR)/target/linux $(PKG_BUILD_DIR)/target/
	if [ -d $(TOPDIR)/staging_dir/host/lib/grub ]; then \
		$(CP) $(TOPDIR)/staging_dir/host/lib/grub/ $(PKG_BUILD_DIR)/staging_dir/host/lib; \
	fi
	rm -rf \
		$(PKG_BUILD_DIR)/target/linux/*/files{,-*} \
		$(PKG_BUILD_DIR)/target/linux/*/patches{,-*}
	-cp $(KERNEL_BUILD_DIR)/* $(IB_KDIR)/ # don't copy subdirectories here
	-cp $(LINUX_DIR)/.config $(IB_LDIR)/
	-$(SCRIPT_DIR)/bundle-libraries.sh $(IB_LDIR)/scripts/dtc \
	  $(LINUX_DIR)/scripts/dtc/dtc
	if [ -d $(LINUX_DIR)/arch/$(LINUX_KARCH)/boot/dts ]; then \
		$(CP) $(LINUX_DIR)/arch/$(LINUX_KARCH)/boot/dts/* $(IB_DTSDIR); \
	fi
ifdef CONFIG_EXTERNAL_KERNEL_TREE
	-cp $(CONFIG_EXTERNAL_KERNEL_TREE)/Makefile $(IB_LDIR)/
	mkdir -p $(IB_EXT_KDIR)
	cp -rf $(IB_LDIR)/* $(IB_EXT_KDIR)/
endif
	$(SED) 's,^# REVISION:=.*,REVISION:=$(REVISION),g' $(PKG_BUILD_DIR)/include/version.mk
	find $(PKG_BUILD_DIR) -name CVS -o -name .git -o -name .svn \
	  | $(XARGS) rm -rf
	find $(STAGING_DIR_HOST)/bin -maxdepth 1 -type f -perm -u=x \
	  | $(XARGS) $(SCRIPT_DIR)/bundle-libraries.sh $(PKG_BUILD_DIR)/staging_dir/host/bin/
	STRIP=sstrip $(SCRIPT_DIR)/rstrip.sh $(PKG_BUILD_DIR)/staging_dir/host/bin/
	$(TAR) -cf - -C $(BUILD_DIR) $(IB_NAME) | bzip2 -c > $@

download:
prepare:
compile: $(BIN_DIR)/$(IB_NAME).tar.bz2
install: compile

clean: FORCE
	rm -rf $(PKG_BUILD_DIR) $(BIN_DIR)/$(IB_NAME).tar.bz2
