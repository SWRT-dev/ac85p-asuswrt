# Makefile
# makefile for project perp
# wcm, 2011.01.12 - 2011.01.31
# ===

include ../conf.mk
CFLAGS += -I. -I../lasagna
LDFLAGS += -L../lasagna -lasagna

PERPAPPS = \
  perpd \
  perpboot \
  perpctl \
  perphup \
  perpls \
  perpok \
  perpstat \
  sissylog \
  tinylog \

all: $(PERPAPPS) perp-setup

##
## perpd:
##
PERPD_OBJS = \
  perpd.o \
  perpd_conn.o \
  perpd_svdef.o \

perpd: $(PERPD_OBJS)
	$(CC) $(CFLAGS) -o $@ $(PERPD_OBJS) $(LDFLAGS)

perpd.o: perpd.c perpd.h perp_common.h
	$(CC) $(CFLAGS) -c perpd.c

perpd_conn.o: perpd_conn.c perpd.h perp_common.h
	$(CC) $(CFLAGS) -c perpd_conn.c

perpd_svdef.o: perpd_svdef.c perpd.h perp_common.h
	$(CC) $(CFLAGS) -c perpd_svdef.c


##
## perp clients:
##
perpboot: perpboot.c perp_common.h perp_stderr.h
	$(CC) $(CFLAGS) -o $@ perpboot.c $(LDFLAGS)

perpctl: perpctl.c perp_common.h perp_stderr.h
	$(CC) $(CFLAGS) -o $@ perpctl.c $(LDFLAGS)

perphup: perphup.c perp_common.h perp_stderr.h
	$(CC) $(CFLAGS) -o $@ perphup.c $(LDFLAGS)

perpls: perpls.c perp_common.h perp_stderr.h
	$(CC) $(CFLAGS) -o $@ perpls.c $(LDFLAGS)

perpok: perpok.c perp_common.h perp_stderr.h
	$(CC) $(CFLAGS) -o $@ perpok.c $(LDFLAGS)

perpstat: perpstat.c perp_common.h perp_stderr.h
	$(CC) $(CFLAGS) -o $@ perpstat.c $(LDFLAGS)


##
## sissylog:
##
sissylog: sissylog.c sissylog.h
	$(CC) $(CFLAGS) -o $@ sissylog.c $(LDFLAGS)

##
## tinylog:
##
tinylog: tinylog.c tinylog.h
	$(CC) $(CFLAGS) -o $@ tinylog.c $(LDFLAGS)


##
## perp-setup (not in PERPAPPS)
##
perp-setup: perp-setup.in
	cat perp-setup.in | sed -e 's}@@SBIN@@}$(SBINDIR)}g' > $@
	chmod 0755 $@


##
## install targets:
##
install-man:
	install -d -m 0755 $(DESTDIR)$(MANDIR)/man5
	@for f in ./man/*.5 ; do\
	    echo "install -m 0644 $${f} $(DESTDIR)$(MANDIR)/man5";\
	    install -m 0644 $${f} $(DESTDIR)$(MANDIR)/man5 ;\
	done
	install -d -m 0755 $(DESTDIR)$(MANDIR)/man8
	@for f in ./man/*.8 ; do\
	    echo "install -m 0644 $${f} $(DESTDIR)$(MANDIR)/man8";\
	install -m 0644 $${f} $(DESTDIR)$(MANDIR)/man8 ;\
	done

install-sbin: $(PERPAPPS) perp-setup tinylog_run
	install -d -m 0755  $(DESTDIR)$(SBINDIR)
	@for f in $(PERPAPPS) perp-setup tinylog_run ; do\
	    echo "install -m 0755 $${f} $(DESTDIR)$(SBINDIR)";\
	    install -m 0755 $${f} $(DESTDIR)$(SBINDIR);\
	done

install: install-sbin install-man


##
## misc targets:
##

TARGETS: Makefile
	grep '^[a-z].*:' Makefile | cut -d':' -f1 >TARGETS

clean: TARGETS
	rm -f `cat TARGETS` TARGETS

strip: $(PERPAPPS)
	$(STRIP) $(PERPAPPS)


.PHONY: all clean install install-man install-sbin strip


### EOF (Makefile)
