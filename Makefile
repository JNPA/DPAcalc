.PHONY:
all: dpacalc

KCONFIG_PATH=./kconfig/kconfig
KCONFIG_CONFIG=dpacalc-config
KCONFIG_FILENAME=Kconfig
KCONFIG_OUTPUT_CMAKE=dpacalc-config.cmake
NUMCPUS=$(shell grep -c '^processor' /proc/cpuinfo)

$(KCONFIG_PATH)/kconfig.inc.mk:
	git submodule init
	git submodule update

include $(KCONFIG_PATH)/kconfig.inc.mk

clean:
	rm -r out

dpaconfig: 
ifndef CONFIG_DONE
	@echo 'You should configure DPAcalc with "make menuconfig" or "make xconfig" at your choose.' && exit 1
endif

out/Makefile:
	mkdir -p out
	cd out && cmake ../src

dpacalc: dpaconfig out/Makefile
	cd out && $(MAKE) -j$(NUMCPUS)
	
