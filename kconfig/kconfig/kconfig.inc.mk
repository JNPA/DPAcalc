.PHONY: menuconfig xconfig gconfig nconfig config

ifndef KCONFIG_PATH
error 'Please set KCONFIG_PATH before including kconfig.inc.mk'
endif

ifndef KCONFIG_FILENAME
KCONFIG_FILENAME:= Kconfig
endif
export KCONFIG_CONFIG
menuconfig:
	cd $(KCONFIG_PATH) && $(MAKE) menuconfig
	$(KCONFIG_PATH)/scripts/kconfig/mconf $(KCONFIG_FILENAME)
ifdef KCONFIG_OUTPUT_CMAKE
	cat $(KCONFIG_CONFIG)|sed 's/^\([^=]*\)=\(.*\)/set ( \1 \2 )/' > $(KCONFIG_OUTPUT_CMAKE)
endif
xconfig:
	cd $(KCONFIG_PATH)/ && $(MAKE) xconfig
	$(KCONFIG_PATH)/scripts/kconfig/qconf $(KCONFIG_FILENAME)
gconfig:
	cd $(KCONFIG_PATH)/ && $(MAKE) gconfig
	$(KCONFIG_PATH)/scripts/kconfig/gconf $(KCONFIG_FILENAME)
nconfig:
	cd $(KCONFIG_PATH)/ && $(MAKE) gconfig
	$(KCONFIG_PATH)/scripts/kconfig/nconf $(KCONFIG_FILENAME)
config:
	cd $(KCONFIG_PATH)/ && $(MAKE) gconfig
	$(KCONFIG_PATH)/scripts/kconfig/conf $(KCONFIG_FILENAME)

ifdef KCONFIG_CONFIG
-include $(KCONFIG_CONFIG)
else
-include .config
endif
