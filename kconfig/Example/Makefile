.PHONY: default_target mytarget
default_target: mytarget

#MANDATORY
KCONFIG_PATH := ../kconfig
#OPTIONAL
KCONFIG_CONFIG := myconfig

include $(KCONFIG_PATH)/kconfig.inc.mk

mytarget:
	@echo Var1 vale $(CONFIG_VAR1)
	@echo Var2 vale $(CONFIG_VAR2)
	@echo Var3_A vale $(CONFIG_VAR3_A)
	@echo Var3_B vale $(CONFIG_VAR3_B)
	@echo Var3_C vale $(CONFIG_VAR3_C)
