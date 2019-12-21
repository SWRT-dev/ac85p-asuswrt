# Makefile
# makfile for runtools:
# utilities for perpetrate runscripts
# wcm, 2008.01.04 - 2013.01.08
# ===

# build configuration:
include ../conf.mk
CFLAGS += -I../lasagna
LDFLAGS += -L../lasagna -lasagna

RUNTOOLS = \
  runargs \
  runargv0 \
  runchoom \
  rundetach \
  rundeux \
  runenv \
  runfile \
  runlimit \
  runlock \
  runpause \
  runsession \
  runtrap \
  runtool \
  runuid \

CATTOOLS = \
  catargs \
  catenv \
  catlimits \
  catuid \

## default target:
all: $(RUNTOOLS) $(CATTOOLS)

## runtools:
runargs: runargs.c runtools_common.h
	$(CC) $(CFLAGS) -o $@ runargs.c $(LDFLAGS)

runargv0: runargv0.c runtools_common.h
	$(CC) $(CFLAGS) -o $@ runargv0.c $(LDFLAGS)

runchoom: runchoom.c runtools_common.h
	$(CC) $(CFLAGS) -o $@ runchoom.c $(LDFLAGS)

rundetach: rundetach.c runtools_common.h
	$(CC) $(CFLAGS) -o $@ rundetach.c $(LDFLAGS)

rundeux: rundeux.c runtools_common.h
	$(CC) $(CFLAGS) -o $@ rundeux.c $(LDFLAGS)

runenv: runenv.c runtools_common.h
	$(CC) $(CFLAGS) -o $@ runenv.c $(LDFLAGS)

runfile: runfile.c runtools_common.h
	$(CC) $(CFLAGS) -o $@ runfile.c $(LDFLAGS)

runlimit: runlimit.c runtools_common.h
	$(CC) $(CFLAGS) -o $@ runlimit.c $(LDFLAGS)

runlock: runlock.c runtools_common.h
	$(CC) $(CFLAGS) -o $@ runlock.c $(LDFLAGS)

runpause: runpause.c runtools_common.h
	$(CC) $(CFLAGS) -o $@ runpause.c $(LDFLAGS)

runsession: runsession.c runtools_common.h
	$(CC) $(CFLAGS) -o $@ runsession.c $(LDFLAGS)

runtool: runtool.c runtools_common.h
	$(CC) $(CFLAGS) -o $@ runtool.c $(LDFLAGS)

runtrap: runtrap.c runtools_common.h
	$(CC) $(CFLAGS) -o $@ runtrap.c $(LDFLAGS)

runuid: runuid.c runtools_common.h
	$(CC) $(CFLAGS) -o $@ runuid.c $(LDFLAGS)


## cattools:
catargs: catargs.c runtools_common.h
	$(CC) $(CFLAGS) -o $@ catargs.c $(LDFLAGS)

catenv: catenv.c runtools_common.h
	$(CC) $(CFLAGS) -o $@ catenv.c $(LDFLAGS)

catlimits: catlimits.c runtools_common.h
	$(CC) $(CFLAGS) -o $@ catlimits.c $(LDFLAGS)

catuid: catuid.c runtools_common.h
	$(CC) $(CFLAGS) -o $@ catuid.c $(LDFLAGS)


## misc targets:
TARGETS: Makefile
	grep '^[a-z].*:' Makefile | cut -d':' -f1 >TARGETS

clean: TARGETS
	rm -f `cat TARGETS` TARGETS

install-man:
	install -d -m 0755 $(DESTDIR)$(MANDIR)/man8
	@for f in ./man/*.8 ; do\
	  echo "install -m 0644 $${f} $(DESTDIR)$(MANDIR)/man8" ;\
	  install -m 0644 $${f} $(DESTDIR)$(MANDIR)/man8 ;\
	done

install-sbin: $(RUNTOOLS)
	install -d -m 0755 $(DESTDIR)$(SBINDIR)
	@for f in $(RUNTOOLS) ; do\
	  echo "install -m 0755 $${f} $(DESTDIR)$(SBINDIR)";\
	  install -m 0755 $${f} $(DESTDIR)$(SBINDIR);\
	done

install: install-sbin install-man

## "cattools" not installed by default:
install-cattools: $(CATTOOLS)
	install -d -m 0755 $(DESTDIR)$(BINDIR)
	@for f in $(CATTOOLS) ; do\
	  echo "install -m 0755 $${f} $(DESTDIR)$(BINDIR)";\
	  install -m 0755 $${f} $(DESTDIR)$(BINDIR);\
	done

strip: $(RUNTOOLS) $(CATTOOLS)
	$(STRIP) $(RUNTOOLS) $(CATTOOLS)


.PHONY: all clean install install-man install-sbin install-cattools strip

### EOF (Makefile)
