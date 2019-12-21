# Makefile
# master makfile for project perp
# perp: a persistent process supervisor (and utility suite)
# includes: perp, runtools, and libasagna
# wcm, 2008.01.04 - 2011.01.31
# ===

# build configuration (not used in this makefile):
#include ./conf.mk

PROJLIBS = \
 ./_done.lasagna \

PROJAPPS = \
 ./_done.perpapps \
 ./_done.runtools \

## default target:
all: ./_done.all
./_done.all: $(PROJLIBS) $(PROJAPPS)
	touch $@

## lasagna:
lasagna: ./_done.lasagna
./_done.lasagna: conf.mk
	cd lasagna && $(MAKE)
	touch $@

## perp:
perpapps: ./_done.perpapps
./_done.perpapps: conf.mk $(PROJLIBS)
	cd perp && $(MAKE)
	touch $@

## runtools:
runtools: ./_done.runtools
./_done.runtools: conf.mk $(PROJLIBS)
	cd runtools && $(MAKE)
	touch $@

## misc targets:
clean:
	rm -f ./_done.perpapps; cd perp && $(MAKE) clean
	rm -f ./_done.runtools; cd runtools && $(MAKE) clean	

cleanlib:
	rm -f ./_done.lasagna; cd lasagna && $(MAKE) clean

cleanall distclean: cleanlib clean
	rm -f ./_done.all


install: $(PROJAPPS)
	cd perp && $(MAKE) install
	cd runtools && $(MAKE) install

strip:
	cd perp && $(MAKE) strip
	cd runtools && $(MAKE) strip

.PHONY: all lasagna perpapps runtools clean cleanall cleanlib distclean install strip


### EOF (Makefile)
