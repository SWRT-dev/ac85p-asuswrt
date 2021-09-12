
OBJS = rtmpiapp.o mt_iapp_util.o

# If you want to debug daemon, add following line
CFLAGS += -I./ -DDBG 
#-DSYSLOG

all: mtkiappd

mtkiappd: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

install:
	install -d $(INSTALLDIR)/usr/sbin
	install -d $(INSTALLDIR)/usr/bin
	install -m 0755 mtkiappd $(INSTALLDIR)/usr/sbin
	install -m 0755 iappd.sh $(INSTALLDIR)/usr/bin

clean:
	rm -f core *.o mtkiappd
