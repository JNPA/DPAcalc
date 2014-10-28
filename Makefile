.PHONY:
all: dpacalc

NUMCPUS=$(shell grep -c '^processor' /proc/cpuinfo)

clean:
	rm -r out

out/Makefile:
	mkdir -p out
	cd out && cmake ../src

dpacalc: out/Makefile
	cd out && $(MAKE) -j$(NUMCPUS)
	
