# Makefile
# makfile for libasagna
# wcm, 2008.01.04 - 2013.01.08
# ===

include ../conf.mk
CFLAGS += -I.

all: libasagna.a

## libasagna modules:
LIB_OBJS=\
 $(BUF_OBJS) \
 $(CDB_OBJS) \
 $(CSTR_OBJS) \
 $(DEVOUT_OBJS) \
 $(DOMSOCK_OBJS) \
 $(DYNBUF_OBJS) \
 $(DYNSTR_OBJS) \
 $(DYNSTUF_OBJS) \
 $(EXECVX_OBJS) \
 $(FD_OBJS) \
 $(HDB_OBJS) \
 $(IOQ_OBJS) \
 $(NEWENV_OBJS) \
 $(NEXTOPT_OBJS) \
 $(NFMT_OBJS) \
 $(NUSCAN_OBJS) \
 $(OUTVEC_OBJS) \
 $(PACKET_OBJS) \
 $(PADLOCK_OBJS) \
 $(PIDLOCK_OBJS) \
 $(PKT_OBJS) \
 $(POLLIO_OBJS) \
 $(RLIMIT_OBJS) \
 $(SIG_OBJS) \
 $(SYSSTR_OBJS) \
 $(TAIN_OBJS) \
 $(TX64_OBJS) \
 $(UFUNC_OBJS) \
 $(UPAK_OBJS) \


##
## buf:
##
BUF_OBJS=\
 buf_cmp.o \
 buf_copy.o \
 buf_fill.o \
 buf_ndx.o \
 buf_rcopy.o \
 buf_rndx.o \

buf_cmp.o : buf/buf_cmp.c buf.h
	$(CC) $(CFLAGS) -c buf/buf_cmp.c

buf_copy.o : buf/buf_copy.c buf.h
	$(CC) $(CFLAGS) -c buf/buf_copy.c

buf_fill.o : buf/buf_fill.c buf.h
	$(CC) $(CFLAGS) -c buf/buf_fill.c

buf_ndx.o : buf/buf_ndx.c buf.h
	$(CC) $(CFLAGS) -c buf/buf_ndx.c

buf_rcopy.o : buf/buf_rcopy.c buf.h
	$(CC) $(CFLAGS) -c buf/buf_rcopy.c

buf_rndx.o : buf/buf_rndx.c buf.h
	$(CC) $(CFLAGS) -c buf/buf_rndx.c


##
## cstr:
##
CSTR_OBJS=\
 cstr_cat.o \
 cstr_chop.o \
 cstr_cmp.o \
 cstr_cmpi.o \
 cstr_contains.o \
 cstr_copy.o \
 cstr_dup.o \
 cstr_lcat.o \
 cstr_lcpy.o \
 cstr_len.o \
 cstr_ltrim.o \
 cstr_match.o \
 cstr_matchi.o \
 cstr_ncmp.o \
 cstr_pos.o \
 cstr_rpos.o \
 cstr_rtrim.o \
 cstr_trim.o \
 cstr_vcat_.o \
 cstr_vcopy_.o \
 cstr_vlen_.o \

cstr_cat.o : cstr/cstr_cat.c cstr.h
	$(CC) $(CFLAGS) -c cstr/cstr_cat.c

cstr_chop.o : cstr/cstr_chop.c cstr.h
	$(CC) $(CFLAGS) -c cstr/cstr_chop.c

cstr_cmp.o : cstr/cstr_cmp.c cstr.h
	$(CC) $(CFLAGS) -c cstr/cstr_cmp.c

cstr_cmpi.o : cstr/cstr_cmpi.c cstr.h
	$(CC) $(CFLAGS) -c cstr/cstr_cmpi.c

cstr_contains.o : cstr/cstr_contains.c cstr.h
	$(CC) $(CFLAGS) -c cstr/cstr_contains.c

cstr_copy.o : cstr/cstr_copy.c cstr.h
	$(CC) $(CFLAGS) -c cstr/cstr_copy.c

cstr_dup.o : cstr/cstr_dup.c cstr.h
	$(CC) $(CFLAGS) -c cstr/cstr_dup.c

cstr_lcat.o : cstr/cstr_lcat.c cstr.h
	$(CC) $(CFLAGS) -c cstr/cstr_lcat.c

cstr_lcpy.o : cstr/cstr_lcpy.c cstr.h
	$(CC) $(CFLAGS) -c cstr/cstr_lcpy.c

cstr_len.o : cstr/cstr_len.c cstr.h
	$(CC) $(CFLAGS) -c cstr/cstr_len.c

cstr_ltrim.o : cstr/cstr_ltrim.c cstr.h
	$(CC) $(CFLAGS) -c cstr/cstr_ltrim.c

cstr_match.o : cstr/cstr_match.c cstr.h
	$(CC) $(CFLAGS) -c cstr/cstr_match.c

cstr_matchi.o : cstr/cstr_matchi.c cstr.h
	$(CC) $(CFLAGS) -c cstr/cstr_matchi.c

cstr_ncmp.o : cstr/cstr_ncmp.c cstr.h
	$(CC) $(CFLAGS) -c cstr/cstr_ncmp.c

cstr_pos.o : cstr/cstr_pos.c cstr.h
	$(CC) $(CFLAGS) -c cstr/cstr_pos.c

cstr_rpos.o : cstr/cstr_rpos.c cstr.h
	$(CC) $(CFLAGS) -c cstr/cstr_rpos.c

cstr_rtrim.o : cstr/cstr_rtrim.c cstr.h
	$(CC) $(CFLAGS) -c cstr/cstr_rtrim.c

cstr_trim.o : cstr/cstr_trim.c cstr.h
	$(CC) $(CFLAGS) -c cstr/cstr_trim.c

cstr_vcat_.o : cstr/cstr_vcat_.c cstr.h
	$(CC) $(CFLAGS) -c cstr/cstr_vcat_.c

cstr_vcopy_.o : cstr/cstr_vcopy_.c cstr.h
	$(CC) $(CFLAGS) -c cstr/cstr_vcopy_.c

cstr_vlen_.o : cstr/cstr_vlen_.c cstr.h
	$(CC) $(CFLAGS) -c cstr/cstr_vlen_.c

##
## cdb:
##
CDB_OBJS=\
  cdb_find.o \
  cdb_cc.o \
  cdb_clear.o \
  cdb_close.o \
  cdb_distance.o \
  cdb_dynget.o \
  cdb_dynread.o \
  cdb_get.o \
  cdb_hashpart.o \
  cdb_init.o \
  cdb_open.o \
  cdb_read.o \
  cdb_seq.o \
  cdbmk_add.o \
  cdbmk_addioq.o \
  cdbmk_addrec.o \
  cdbmk_clear.o \
  cdbmk_finish.o \
  cdbmk_init.o \
  cdbmk__update.o \

cdb_find.o : cdb/cdb_find.c cdb.h
	$(CC) $(CFLAGS) -c cdb/cdb_find.c

cdb_cc.o : cdb/cdb_cc.c cdb.h
	$(CC) $(CFLAGS) -c cdb/cdb_cc.c

cdb_clear.o : cdb/cdb_clear.c cdb.h
	$(CC) $(CFLAGS) -c cdb/cdb_clear.c

cdb_close.o : cdb/cdb_close.c cdb.h
	$(CC) $(CFLAGS) -c cdb/cdb_close.c

cdb_distance.o : cdb/cdb_distance.c cdb.h
	$(CC) $(CFLAGS) -c cdb/cdb_distance.c

cdb_dynget.o : cdb/cdb_dynget.c cdb.h
	$(CC) $(CFLAGS) -c cdb/cdb_dynget.c

cdb_dynread.o : cdb/cdb_dynread.c cdb.h
	$(CC) $(CFLAGS) -c cdb/cdb_dynread.c

cdb_get.o : cdb/cdb_get.c cdb.h
	$(CC) $(CFLAGS) -c cdb/cdb_get.c

cdb_hashpart.o : cdb/cdb_hashpart.c cdb.h
	$(CC) $(CFLAGS) -c cdb/cdb_hashpart.c
 
cdb_init.o : cdb/cdb_init.c cdb.h
	$(CC) $(CFLAGS) -c cdb/cdb_init.c

cdb_open.o : cdb/cdb_open.c cdb.h
	$(CC) $(CFLAGS) -c cdb/cdb_open.c

cdb_read.o : cdb/cdb_read.c cdb.h
	$(CC) $(CFLAGS) -c cdb/cdb_read.c

cdb_seq.o : cdb/cdb_seq.c cdb.h
	$(CC) $(CFLAGS) -c cdb/cdb_seq.c

cdbmk_add.o : cdb/cdbmk_add.c cdb.h cdbmk.h
	$(CC) $(CFLAGS) -c cdb/cdbmk_add.c

cdbmk_addioq.o : cdb/cdbmk_addioq.c cdb.h cdbmk.h
	$(CC) $(CFLAGS) -c cdb/cdbmk_addioq.c

cdbmk_addrec.o : cdb/cdbmk_addrec.c cdb.h cdbmk.h
	$(CC) $(CFLAGS) -c cdb/cdbmk_addrec.c

cdbmk_clear.o : cdb/cdbmk_clear.c cdb.h cdbmk.h
	$(CC) $(CFLAGS) -c cdb/cdbmk_clear.c

cdbmk_finish.o : cdb/cdbmk_finish.c cdb.h cdbmk.h
	$(CC) $(CFLAGS) -c cdb/cdbmk_finish.c

cdbmk_init.o : cdb/cdbmk_init.c cdb.h cdbmk.h
	$(CC) $(CFLAGS) -c cdb/cdbmk_init.c

cdbmk__update.o : cdb/cdbmk__update.c cdb.h cdbmk.h
	$(CC) $(CFLAGS) -c cdb/cdbmk__update.c


##
## devout:
##
DEVOUT_OBJS=\
  devout.o \

devout.o : devout/devout.c devout.h
	$(CC) $(CFLAGS) -c devout/devout.c


##
## domsock:
##
DOMSOCK_OBJS=\
  domsock_accept.o \
  domsock_close.o \
  domsock_connect.o \
  domsock_create.o \

domsock_accept.o : domsock/domsock_accept.c domsock.h
	$(CC) $(CFLAGS) -c domsock/domsock_accept.c

domsock_close.o : domsock/domsock_close.c domsock.h
	$(CC) $(CFLAGS) -c domsock/domsock_close.c

domsock_connect.o : domsock/domsock_connect.c domsock.h
	$(CC) $(CFLAGS) -c domsock/domsock_connect.c

domsock_create.o : domsock/domsock_create.c domsock.h
	$(CC) $(CFLAGS) -c domsock/domsock_create.c


##
## dynbuf:
##
DYNBUF_OBJS=\
 dynbuf.o \
 dynbuf_clear.o \
 dynbuf_copy.o \
 dynbuf_free.o \
 dynbuf_freebuf.o \
 dynbuf_grow.o \
 dynbuf_need.o \
 dynbuf_new.o \
 dynbuf_pack.o \
 dynbuf_put.o \
 dynbuf_putb.o \
 
dynbuf.o : dynbuf/dynbuf.c dynbuf.h
	$(CC) $(CFLAGS) -c dynbuf/dynbuf.c

dynbuf_clear.o : dynbuf/dynbuf_clear.c dynbuf.h
	$(CC) $(CFLAGS) -c dynbuf/dynbuf_clear.c

dynbuf_copy.o : dynbuf/dynbuf_copy.c dynbuf.h
	$(CC) $(CFLAGS) -c dynbuf/dynbuf_copy.c

dynbuf_free.o : dynbuf/dynbuf_free.c dynbuf.h
	$(CC) $(CFLAGS) -c dynbuf/dynbuf_free.c

dynbuf_freebuf.o : dynbuf/dynbuf_freebuf.c dynbuf.h
	$(CC) $(CFLAGS) -c dynbuf/dynbuf_freebuf.c

dynbuf_grow.o : dynbuf/dynbuf_grow.c dynbuf.h
	$(CC) $(CFLAGS) -c dynbuf/dynbuf_grow.c

dynbuf_need.o : dynbuf/dynbuf_need.c dynbuf.h
	$(CC) $(CFLAGS) -c dynbuf/dynbuf_need.c

dynbuf_new.o : dynbuf/dynbuf_new.c dynbuf.h
	$(CC) $(CFLAGS) -c dynbuf/dynbuf_new.c

dynbuf_pack.o : dynbuf/dynbuf_pack.c dynbuf.h
	$(CC) $(CFLAGS) -c dynbuf/dynbuf_pack.c

dynbuf_put.o : dynbuf/dynbuf_put.c dynbuf.h
	$(CC) $(CFLAGS) -c dynbuf/dynbuf_put.c

dynbuf_putb.o : dynbuf/dynbuf_putb.c dynbuf.h
	$(CC) $(CFLAGS) -c dynbuf/dynbuf_putb.c


##
## dynstr:
##
DYNSTR_OBJS=\
 dynstr.o \
 dynstr_chop.o \
 dynstr_clear.o \
 dynstr_copy.o \
 dynstr_copys.o \
 dynstr_free.o \
 dynstr_freestr.o \
 dynstr_grow.o \
 dynstr_need.o \
 dynstr_new.o \
 dynstr_put.o \
 dynstr_putc.o \
 dynstr_putn.o \
 dynstr_puts.o \
 dynstr_set.o \
 dynstr_vputs_.o \

dynstr.o : dynstr/dynstr.c dynstr.h
	$(CC) $(CFLAGS) -c dynstr/dynstr.c

dynstr_chop.o : dynstr/dynstr_chop.c dynstr.h
	$(CC) $(CFLAGS) -c dynstr/dynstr_chop.c

dynstr_clear.o : dynstr/dynstr_clear.c dynstr.h
	$(CC) $(CFLAGS) -c dynstr/dynstr_clear.c

dynstr_copy.o : dynstr/dynstr_copy.c dynstr.h
	$(CC) $(CFLAGS) -c dynstr/dynstr_copy.c

dynstr_copys.o : dynstr/dynstr_copys.c dynstr.h
	$(CC) $(CFLAGS) -c dynstr/dynstr_copys.c

dynstr_free.o : dynstr/dynstr_free.c dynstr.h
	$(CC) $(CFLAGS) -c dynstr/dynstr_free.c

dynstr_freestr.o : dynstr/dynstr_freestr.c dynstr.h
	$(CC) $(CFLAGS) -c dynstr/dynstr_freestr.c

dynstr_grow.o : dynstr/dynstr_grow.c dynstr.h
	$(CC) $(CFLAGS) -c dynstr/dynstr_grow.c

dynstr_need.o : dynstr/dynstr_need.c dynstr.h
	$(CC) $(CFLAGS) -c dynstr/dynstr_need.c

dynstr_new.o : dynstr/dynstr_new.c dynstr.h
	$(CC) $(CFLAGS) -c dynstr/dynstr_new.c

dynstr_put.o : dynstr/dynstr_put.c dynstr.h
	$(CC) $(CFLAGS) -c dynstr/dynstr_put.c

dynstr_putc.o : dynstr/dynstr_putc.c dynstr.h
	$(CC) $(CFLAGS) -c dynstr/dynstr_putc.c

dynstr_putn.o : dynstr/dynstr_putn.c dynstr.h
	$(CC) $(CFLAGS) -c dynstr/dynstr_putn.c

dynstr_puts.o : dynstr/dynstr_puts.c dynstr.h
	$(CC) $(CFLAGS) -c dynstr/dynstr_puts.c

dynstr_set.o : dynstr/dynstr_set.c dynstr.h
	$(CC) $(CFLAGS) -c dynstr/dynstr_set.c

dynstr_vputs_.o : dynstr/dynstr_vputs_.c dynstr.h
	$(CC) $(CFLAGS) -c dynstr/dynstr_vputs_.c


##
## dynstuf:
##
DYNSTUF_OBJS=\
 dynstuf_find.o \
 dynstuf_free.o \
 dynstuf_get.o \
 dynstuf_grow.o \
 dynstuf_init.o \
 dynstuf_isempty.o \
 dynstuf_items.o \
 dynstuf_new.o \
 dynstuf_peek.o \
 dynstuf_pop.o \
 dynstuf_push.o \
 dynstuf_replace.o \
 dynstuf_reverse.o \
 dynstuf_set.o \
 dynstuf_slots.o \
 dynstuf_sort.o \
 dynstuf_stuf.o \
 dynstuf_visit.o \
 
dynstuf_find.o : dynstuf/dynstuf_find.c dynstuf.h
	$(CC) $(CFLAGS) -c dynstuf/dynstuf_find.c

dynstuf_free.o : dynstuf/dynstuf_free.c dynstuf.h
	$(CC) $(CFLAGS) -c dynstuf/dynstuf_free.c

dynstuf_get.o : dynstuf/dynstuf_get.c dynstuf.h
	$(CC) $(CFLAGS) -c dynstuf/dynstuf_get.c

dynstuf_grow.o : dynstuf/dynstuf_grow.c dynstuf.h
	$(CC) $(CFLAGS) -c dynstuf/dynstuf_grow.c

dynstuf_init.o : dynstuf/dynstuf_init.c dynstuf.h
	$(CC) $(CFLAGS) -c dynstuf/dynstuf_init.c

dynstuf_isempty.o : dynstuf/dynstuf_isempty.c dynstuf.h
	$(CC) $(CFLAGS) -c dynstuf/dynstuf_isempty.c

dynstuf_items.o : dynstuf/dynstuf_items.c dynstuf.h
	$(CC) $(CFLAGS) -c dynstuf/dynstuf_items.c

dynstuf_new.o : dynstuf/dynstuf_new.c dynstuf.h
	$(CC) $(CFLAGS) -c dynstuf/dynstuf_new.c

dynstuf_peek.o : dynstuf/dynstuf_peek.c dynstuf.h
	$(CC) $(CFLAGS) -c dynstuf/dynstuf_peek.c

dynstuf_pop.o : dynstuf/dynstuf_pop.c dynstuf.h
	$(CC) $(CFLAGS) -c dynstuf/dynstuf_pop.c

dynstuf_push.o : dynstuf/dynstuf_push.c dynstuf.h
	$(CC) $(CFLAGS) -c dynstuf/dynstuf_push.c

dynstuf_replace.o : dynstuf/dynstuf_replace.c dynstuf.h
	$(CC) $(CFLAGS) -c dynstuf/dynstuf_replace.c
 
dynstuf_reverse.o : dynstuf/dynstuf_reverse.c dynstuf.h
	$(CC) $(CFLAGS) -c dynstuf/dynstuf_reverse.c

dynstuf_set.o : dynstuf/dynstuf_set.c dynstuf.h
	$(CC) $(CFLAGS) -c dynstuf/dynstuf_set.c

dynstuf_slots.o : dynstuf/dynstuf_slots.c dynstuf.h
	$(CC) $(CFLAGS) -c dynstuf/dynstuf_slots.c

dynstuf_sort.o : dynstuf/dynstuf_sort.c dynstuf.h
	$(CC) $(CFLAGS) -c dynstuf/dynstuf_sort.c

dynstuf_stuf.o : dynstuf/dynstuf_stuf.c dynstuf.h
	$(CC) $(CFLAGS) -c dynstuf/dynstuf_stuf.c

dynstuf_visit.o : dynstuf/dynstuf_visit.c dynstuf.h
	$(CC) $(CFLAGS) -c dynstuf/dynstuf_visit.c


##
## execvx:
##
EXECVX_OBJS=\
 execvx.o \

execvx.o : execvx/execvx.c execvx.h
	$(CC) $(CFLAGS) -c execvx/execvx.c


##
## fd:
##
FD_OBJS=\
 fd_blocking.o \
 fd_cloexec.o \
 fd_dupe.o \
 fd_move.o \
 fd_nonblock.o \

fd_blocking.o : fd/fd_blocking.c fd.h
	$(CC) $(CFLAGS) -c fd/fd_blocking.c

fd_cloexec.o : fd/fd_cloexec.c fd.h
	$(CC) $(CFLAGS) -c fd/fd_cloexec.c

fd_dupe.o : fd/fd_dupe.c fd.h
	$(CC) $(CFLAGS) -c fd/fd_dupe.c

fd_move.o : fd/fd_move.c fd.h
	$(CC) $(CFLAGS) -c fd/fd_move.c

fd_nonblock.o : fd/fd_nonblock.c fd.h
	$(CC) $(CFLAGS) -c fd/fd_nonblock.c


##
## hdb:
##
HDB_OBJS=\
  hdb_find.o \
  hdb_cc.o \
  hdb_clear.o \
  hdb_close.o \
  hdb_distance.o \
  hdb_dynget.o \
  hdb_dynread.o \
  hdb_get.o \
  hdb_hashpart.o \
  hdb_init.o \
  hdb_open.o \
  hdb_read.o \
  hdb_seq.o \
  hdbmk_add.o \
  hdbmk_addioq.o \
  hdbmk_addrec.o \
  hdbmk_clear.o \
  hdbmk_finish.o \
  hdbmk_start.o \
  hdbmk__update.o \

hdb_find.o : hdb/hdb_find.c hdb.h
	$(CC) $(CFLAGS) -c hdb/hdb_find.c

hdb_cc.o : hdb/hdb_cc.c hdb.h
	$(CC) $(CFLAGS) -c hdb/hdb_cc.c

hdb_clear.o : hdb/hdb_clear.c hdb.h
	$(CC) $(CFLAGS) -c hdb/hdb_clear.c

hdb_close.o : hdb/hdb_close.c hdb.h
	$(CC) $(CFLAGS) -c hdb/hdb_close.c

hdb_distance.o : hdb/hdb_distance.c hdb.h
	$(CC) $(CFLAGS) -c hdb/hdb_distance.c

hdb_dynget.o : hdb/hdb_dynget.c hdb.h
	$(CC) $(CFLAGS) -c hdb/hdb_dynget.c

hdb_dynread.o : hdb/hdb_dynread.c hdb.h
	$(CC) $(CFLAGS) -c hdb/hdb_dynread.c

hdb_get.o : hdb/hdb_get.c hdb.h
	$(CC) $(CFLAGS) -c hdb/hdb_get.c

hdb_hashpart.o : hdb/hdb_hashpart.c hdb.h
	$(CC) $(CFLAGS) -c hdb/hdb_hashpart.c
 
hdb_init.o : hdb/hdb_init.c hdb.h
	$(CC) $(CFLAGS) -c hdb/hdb_init.c

hdb_open.o : hdb/hdb_open.c hdb.h
	$(CC) $(CFLAGS) -c hdb/hdb_open.c

hdb_read.o : hdb/hdb_read.c hdb.h
	$(CC) $(CFLAGS) -c hdb/hdb_read.c

hdb_seq.o : hdb/hdb_seq.c hdb.h
	$(CC) $(CFLAGS) -c hdb/hdb_seq.c

hdbmk_add.o : hdb/hdbmk_add.c hdb.h hdbmk.h
	$(CC) $(CFLAGS) -c hdb/hdbmk_add.c

hdbmk_addioq.o : hdb/hdbmk_addioq.c hdb.h hdbmk.h
	$(CC) $(CFLAGS) -c hdb/hdbmk_addioq.c

hdbmk_addrec.o : hdb/hdbmk_addrec.c hdb.h hdbmk.h
	$(CC) $(CFLAGS) -c hdb/hdbmk_addrec.c

hdbmk_clear.o : hdb/hdbmk_clear.c hdb.h hdbmk.h
	$(CC) $(CFLAGS) -c hdb/hdbmk_clear.c

hdbmk_finish.o : hdb/hdbmk_finish.c hdb.h hdbmk.h
	$(CC) $(CFLAGS) -c hdb/hdbmk_finish.c

hdbmk_start.o : hdb/hdbmk_start.c hdb.h hdbmk.h
	$(CC) $(CFLAGS) -c hdb/hdbmk_start.c

hdbmk__update.o : hdb/hdbmk__update.c hdb.h hdbmk.h
	$(CC) $(CFLAGS) -c hdb/hdbmk__update.c


##
## hfunc:
##
HFUNC_OBJS=\
  hfunc_djba.o \
  hfunc_djbm.o \
  hfunc_djbx.o \
  hfunc_elf1.o \
  hfunc_fnva.o \
  hfunc_fnvm.o \
  hfunc_ghfa.o \
  hfunc_ghfm.o \
  hfunc_ghfx.o \
  hfunc_jsw1.o \
  hfunc_kp37.o \
  hfunc_kr31.o \
  hfunc_kx17.o \
  hfunc_murm.o \
  hfunc_oat1.o \
  hfunc_p50a.o \
  hfunc_pjw1.o \
  hfunc_rsuh.o \
  hfunc_rot1.o \
  hfunc_rotm.o \
  hfunc_sax1.o \
  hfunc_sdbm.o \
  hfunc_sfh1.o \
  hfunc_postmix32.o \

hfunc_djba.o : hfunc/hfunc_djba.c hfunc.h
	$(CC) $(CFLAGS) -c hfunc/hfunc_djba.c

hfunc_djbm.o : hfunc/hfunc_djbm.c hfunc.h
	$(CC) $(CFLAGS) -c hfunc/hfunc_djbm.c

hfunc_djbx.o : hfunc/hfunc_djbx.c hfunc.h
	$(CC) $(CFLAGS) -c hfunc/hfunc_djbx.c

hfunc_elf1.o : hfunc/hfunc_elf1.c hfunc.h
	$(CC) $(CFLAGS) -c hfunc/hfunc_elf1.c

hfunc_fnva.o : hfunc/hfunc_fnva.c hfunc.h
	$(CC) $(CFLAGS) -c hfunc/hfunc_fnva.c

hfunc_fnvm.o : hfunc/hfunc_fnvm.c hfunc.h
	$(CC) $(CFLAGS) -c hfunc/hfunc_fnvm.c

hfunc_ghfa.o : hfunc/hfunc_ghfa.c hfunc.h
	$(CC) $(CFLAGS) -c hfunc/hfunc_ghfa.c

hfunc_ghfm.o : hfunc/hfunc_ghfm.c hfunc.h
	$(CC) $(CFLAGS) -c hfunc/hfunc_ghfm.c

hfunc_ghfx.o : hfunc/hfunc_ghfx.c hfunc.h
	$(CC) $(CFLAGS) -c hfunc/hfunc_ghfx.c

hfunc_kp37.o : hfunc/hfunc_kp37.c hfunc.h
	$(CC) $(CFLAGS) -c hfunc/hfunc_kp37.c

hfunc_kr31.o : hfunc/hfunc_kr31.c hfunc.h
	$(CC) $(CFLAGS) -c hfunc/hfunc_kr31.c

hfunc_kx17.o : hfunc/hfunc_kx17.c hfunc.h
	$(CC) $(CFLAGS) -c hfunc/hfunc_kx17.c

hfunc_murm.o : hfunc/hfunc_murm.c hfunc.h
	$(CC) $(CFLAGS) -c hfunc/hfunc_murm.c

hfunc_jsw1.o : hfunc/hfunc_jsw1.c hfunc.h
	$(CC) $(CFLAGS) -c hfunc/hfunc_jsw1.c

hfunc_oat1.o : hfunc/hfunc_oat1.c hfunc.h
	$(CC) $(CFLAGS) -c hfunc/hfunc_oat1.c

hfunc_p50a.o : hfunc/hfunc_p50a.c hfunc.h
	$(CC) $(CFLAGS) -c hfunc/hfunc_p50a.c

hfunc_pjw1.o : hfunc/hfunc_pjw1.c hfunc.h
	$(CC) $(CFLAGS) -c hfunc/hfunc_pjw1.c

hfunc_rot1.o : hfunc/hfunc_rot1.c hfunc.h
	$(CC) $(CFLAGS) -c hfunc/hfunc_rot1.c

hfunc_rotm.o : hfunc/hfunc_rotm.c hfunc.h
	$(CC) $(CFLAGS) -c hfunc/hfunc_rotm.c

hfunc_rsuh.o : hfunc/hfunc_rsuh.c hfunc.h
	$(CC) $(CFLAGS) -c hfunc/hfunc_rsuh.c

hfunc_sax1.o : hfunc/hfunc_sax1.c hfunc.h
	$(CC) $(CFLAGS) -c hfunc/hfunc_sax1.c

hfunc_sdbm.o : hfunc/hfunc_sdbm.c hfunc.h
	$(CC) $(CFLAGS) -c hfunc/hfunc_sdbm.c

hfunc_sfh1.o : hfunc/hfunc_sfh1.c hfunc.h
	$(CC) $(CFLAGS) -c hfunc/hfunc_sfh1.c

hfunc_postmix32.o : hfunc/hfunc_postmix32.c hfunc.h
	$(CC) $(CFLAGS) -c hfunc/hfunc_postmix32.c

##
## ioq:
##
IOQ_OBJS=\
  ioq_get.o \
  ioq_getln.o \
  ioq_init.o \
  ioq_put.o \
  ioq_putfd.o \
  ioq_putfile.o \
  ioq_stderr.o \
  ioq_stdin.o \
  ioq_stdout.o \
  ioq_vputs_.o

ioq_get.o : ioq/ioq_get.c ioq.h
	$(CC) $(CFLAGS) -c ioq/ioq_get.c

ioq_getln.o : ioq/ioq_getln.c ioq.h
	$(CC) $(CFLAGS) -c ioq/ioq_getln.c

ioq_init.o : ioq/ioq_init.c ioq.h
	$(CC) $(CFLAGS) -c ioq/ioq_init.c

ioq_put.o : ioq/ioq_put.c ioq.h
	$(CC) $(CFLAGS) -c ioq/ioq_put.c

ioq_putfd.o : ioq/ioq_putfd.c ioq.h
	$(CC) $(CFLAGS) -c ioq/ioq_putfd.c

ioq_putfile.o : ioq/ioq_putfile.c ioq.h
	$(CC) $(CFLAGS) -c ioq/ioq_putfile.c

ioq_stderr.o : ioq/ioq_stderr.c ioq.h ioq_std.h
	$(CC) $(CFLAGS) -c ioq/ioq_stderr.c

ioq_stdin.o : ioq/ioq_stdin.c ioq.h ioq_std.h
	$(CC) $(CFLAGS) -c ioq/ioq_stdin.c

ioq_stdout.o : ioq/ioq_stdout.c ioq.h ioq_std.h
	$(CC) $(CFLAGS) -c ioq/ioq_stdout.c

ioq_vputs_.o : ioq/ioq_vputs_.c ioq.h
	$(CC) $(CFLAGS) -c ioq/ioq_vputs_.c

##
## newenv:
##
NEWENV_OBJS=\
  newenv.o \

newenv.o : newenv/newenv.c newenv.h
	$(CC) $(CFLAGS) -c newenv/newenv.c

##
## nextopt:
##
NEXTOPT_OBJS=\
  nextopt.o \
  nextopt_progname.o \

nextopt.o : nextopt/nextopt.c nextopt.h
	$(CC) $(CFLAGS) -c nextopt/nextopt.c

nextopt_progname.o : nextopt/nextopt_progname.c nextopt.h
	$(CC) $(CFLAGS) -c nextopt/nextopt_progname.c


##
## nfmt:
##
NFMT_OBJS=\
 nfmt_uint32.o \
 nfmt_uint32_pad.o \
 nfmt_uint32_pad_.o \
 nfmt_uint32_pad0.o \
 nfmt_uint32_pad0_.o \
 nfmt_uint32o.o \
 nfmt_uint32o_pad0.o \
 nfmt_uint32o_pad0_.o \
 nfmt_uint32x.o \
 nfmt_uint32x_pad.o \
 nfmt_uint32x_pad_.o \
 nfmt_uint32x_pad0.o \
 nfmt_uint32x_pad0_.o \
 nfmt_uint64.o \
 nfmt_uint64_pad.o \
 nfmt_uint64_pad_.o \
 nfmt_uint64_pad0.o \
 nfmt_uint64_pad0_.o \

nfmt_uint32.o : nfmt/nfmt_uint32.c nfmt.h
	$(CC) $(CFLAGS) -c nfmt/nfmt_uint32.c

nfmt_uint32_pad.o : nfmt/nfmt_uint32_pad.c nfmt.h
	$(CC) $(CFLAGS) -c nfmt/nfmt_uint32_pad.c

nfmt_uint32_pad_.o : nfmt/nfmt_uint32_pad_.c nfmt.h
	$(CC) $(CFLAGS) -c nfmt/nfmt_uint32_pad_.c

nfmt_uint32_pad0.o : nfmt/nfmt_uint32_pad0.c nfmt.h
	$(CC) $(CFLAGS) -c nfmt/nfmt_uint32_pad0.c

nfmt_uint32_pad0_.o : nfmt/nfmt_uint32_pad0_.c nfmt.h
	$(CC) $(CFLAGS) -c nfmt/nfmt_uint32_pad0_.c

nfmt_uint32o.o : nfmt/nfmt_uint32o.c nfmt.h
	$(CC) $(CFLAGS) -c nfmt/nfmt_uint32o.c

nfmt_uint32x.o : nfmt/nfmt_uint32x.c nfmt.h
	$(CC) $(CFLAGS) -c nfmt/nfmt_uint32x.c

nfmt_uint32o_pad0.o : nfmt/nfmt_uint32o_pad0.c nfmt.h
	$(CC) $(CFLAGS) -c nfmt/nfmt_uint32o_pad0.c

nfmt_uint32o_pad0_.o : nfmt/nfmt_uint32o_pad0_.c nfmt.h
	$(CC) $(CFLAGS) -c nfmt/nfmt_uint32o_pad0_.c

nfmt_uint32x_pad.o : nfmt/nfmt_uint32x_pad.c nfmt.h
	$(CC) $(CFLAGS) -c nfmt/nfmt_uint32x_pad.c

nfmt_uint32x_pad_.o : nfmt/nfmt_uint32x_pad_.c nfmt.h
	$(CC) $(CFLAGS) -c nfmt/nfmt_uint32x_pad_.c

nfmt_uint32x_pad0.o : nfmt/nfmt_uint32x_pad0.c nfmt.h
	$(CC) $(CFLAGS) -c nfmt/nfmt_uint32x_pad0.c

nfmt_uint32x_pad0_.o : nfmt/nfmt_uint32x_pad0_.c nfmt.h
	$(CC) $(CFLAGS) -c nfmt/nfmt_uint32x_pad0_.c

nfmt_uint64.o : nfmt/nfmt_uint64.c nfmt.h
	$(CC) $(CFLAGS) -c nfmt/nfmt_uint64.c

nfmt_uint64_pad.o : nfmt/nfmt_uint64_pad.c nfmt.h
	$(CC) $(CFLAGS) -c nfmt/nfmt_uint64_pad.c

nfmt_uint64_pad_.o : nfmt/nfmt_uint64_pad_.c nfmt.h
	$(CC) $(CFLAGS) -c nfmt/nfmt_uint64_pad_.c

nfmt_uint64_pad0.o : nfmt/nfmt_uint64_pad0.c nfmt.h
	$(CC) $(CFLAGS) -c nfmt/nfmt_uint64_pad0.c

nfmt_uint64_pad0_.o : nfmt/nfmt_uint64_pad0_.c nfmt.h
	$(CC) $(CFLAGS) -c nfmt/nfmt_uint64_pad0_.c


##
## nuscan:
##
NUSCAN_OBJS=\
 nuscan_uint32.o \
 nuscan_uint32o.o \
 
nuscan_uint32.o : nuscan/nuscan_uint32.c nuscan.h
	$(CC) $(CFLAGS) -c nuscan/nuscan_uint32.c

nuscan_uint32o.o : nuscan/nuscan_uint32o.c nuscan.h
	$(CC) $(CFLAGS) -c nuscan/nuscan_uint32o.c


##
## outvec:
##
OUTVEC_OBJS=\
 outvec_STDERR.o \
 outvec_STDOUT.o \
 outvec_flush.o \
 outvec_put.o \
 outvec_puts.o \
 outvec_vputs_.o \

## special builds for outvec:
## build generator for outvec_STDOUT.h:
outvec/mk_outvec : outvec/mk_outvec.c
	gcc $(CFLAGS) -o $@ outvec/mk_outvec.c

## generate outvec_STDOUT.h:
outvec/outvec_STDOUT.h : outvec/mk_outvec
	./outvec/mk_outvec > outvec/outvec_STDOUT.h

## outvec:
outvec_STDERR.o : outvec/outvec_STDERR.c outvec.h
	$(CC) $(CFLAGS) -c outvec/outvec_STDERR.c

outvec_STDOUT.o : outvec/outvec_STDOUT.c outvec/outvec_STDOUT.h outvec.h
	$(CC) $(CFLAGS) -c outvec/outvec_STDOUT.c

outvec_flush.o : outvec/outvec_flush.c outvec.h
	$(CC) $(CFLAGS) -c outvec/outvec_flush.c

outvec_put.o : outvec/outvec_put.c outvec.h
	$(CC) $(CFLAGS) -c outvec/outvec_put.c

outvec_puts.o : outvec/outvec_puts.c outvec.h
	$(CC) $(CFLAGS) -c outvec/outvec_puts.c

outvec_vputs_.o : outvec/outvec_vputs_.c outvec.h
	$(CC) $(CFLAGS) -c outvec/outvec_vputs_.c


##
## packet:
##
PACKET_OBJS=\
 packet_read.o \
 packet_write.o \

packet_read.o : packet/packet_read.c packet.h
	$(CC) $(CFLAGS) -c packet/packet_read.c

packet_write.o : packet/packet_write.c packet.h
	$(CC) $(CFLAGS) -c packet/packet_write.c


##
## padlock:
##
PADLOCK_OBJS=\
 padlock_exbyte.o \
 padlock_exlock.o \
 padlock_extest.o \
 padlock_fcntl.o \
 padlock_shbyte.o \
 padlock_shlock.o \
 padlock_shtest.o \
 padlock_unbyte.o \
 padlock_unlock.o \


padlock_exbyte.o : padlock/padlock_exbyte.c padlock.h
	$(CC) $(CFLAGS) -c padlock/padlock_exbyte.c

padlock_exlock.o : padlock/padlock_exlock.c padlock.h
	$(CC) $(CFLAGS) -c padlock/padlock_exlock.c

padlock_extest.o : padlock/padlock_extest.c padlock.h
	$(CC) $(CFLAGS) -c padlock/padlock_extest.c

padlock_fcntl.o : padlock/padlock_fcntl.c padlock.h
	$(CC) $(CFLAGS) -c padlock/padlock_fcntl.c

padlock_shbyte.o : padlock/padlock_shbyte.c padlock.h
	$(CC) $(CFLAGS) -c padlock/padlock_shbyte.c

padlock_shlock.o : padlock/padlock_shlock.c padlock.h
	$(CC) $(CFLAGS) -c padlock/padlock_shlock.c

padlock_shtest.o : padlock/padlock_shtest.c padlock.h
	$(CC) $(CFLAGS) -c padlock/padlock_shtest.c

padlock_unbyte.o : padlock/padlock_unbyte.c padlock.h
	$(CC) $(CFLAGS) -c padlock/padlock_unbyte.c

padlock_unlock.o : padlock/padlock_unlock.c padlock.h
	$(CC) $(CFLAGS) -c padlock/padlock_unlock.c


##
## pidlock:
##
PIDLOCK_OBJS=\
  pidlock_check.o \
  pidlock_set.o \

pidlock_check.o : pidlock/pidlock_check.c pidlock.h
	$(CC) $(CFLAGS) -c pidlock/pidlock_check.c

pidlock_set.o : pidlock/pidlock_set.c pidlock.h
	$(CC) $(CFLAGS) -c pidlock/pidlock_set.c


##
## pkt:
##
PKT_OBJS=\
 pkt_load.o \
 pkt_read.o \
 pkt_write.o\

pkt_load.o : pkt/pkt_load.c pkt.h
	$(CC) $(CFLAGS) -c pkt/pkt_load.c

pkt_read.o : pkt/pkt_read.c pkt.h
	$(CC) $(CFLAGS) -c pkt/pkt_read.c

pkt_write.o : pkt/pkt_write.c pkt.h
	$(CC) $(CFLAGS) -c pkt/pkt_write.c


##
## pollio:
##
POLLIO_OBJS=\
  pollio.o \

pollio.o : pollio/pollio.c pollio.h
	$(CC) $(CFLAGS) -c pollio/pollio.c


##
## rlimit:
##
RLIMIT_OBJS=\
 rlimit.o \

## special builds for rlimit:
## mk_rlimit (rlimit_defs generator):
rlimit/mk_rlimit : rlimit/mk_rlimit.c
	gcc $(CFLAGS) -o $@ rlimit/mk_rlimit.c

## generate rlimit_defs table:
rlimit/rlimit_defs.c.in : rlimit/mk_rlimit
	./rlimit/mk_rlimit > rlimit/rlimit_defs.c.in

rlimit.o : rlimit/rlimit.c \
  rlimit/rlimit_defs.c.in \
  rlimit/rlimit_private.h \
  rlimit.h
	$(CC) $(CFLAGS) -c rlimit/rlimit.c


##
## sig:
##
SIG_OBJS=\
 sig_block.o \
 sig_catch.o \
 sig_catchr.o \
 sig_unblock.o \

sig_block.o : sig/sig_block.c sig.h
	$(CC) $(CFLAGS) -c sig/sig_block.c

sig_catch.o : sig/sig_catch.c sig.h
	$(CC) $(CFLAGS) -c sig/sig_catch.c

sig_catchr.o : sig/sig_catchr.c sig.h
	$(CC) $(CFLAGS) -c sig/sig_catchr.c

sig_unblock.o : sig/sig_unblock.c sig.h
	$(CC) $(CFLAGS) -c sig/sig_unblock.c


##
## sigset:
##

# note:
#   no object files for sigset
#   sigset consists only of the header file "sigset.h"


##
## sysstr:
##
SYSSTR_OBJS=\
 sysstr_errno.o \
 sysstr_signal.o \

## special builds for sysstr:
## mysysstr sysno table generator:
sysstr/mksysstr : sysstr/mksysstr.c
	gcc $(CFLAGS) -o $@ sysstr/mksysstr.c

## generate errno table:
sysstr/sysstr_errno.c.in : sysstr/mksysstr sysstr.h
	./sysstr/mksysstr errno > $@

## generate signal table:
sysstr/sysstr_signo.c.in : sysstr/mksysstr sysstr.h
	./sysstr/mksysstr signo > $@

sysstr_errno.o : sysstr/sysstr_errno.c \
  sysstr/sysno_lookup.c.in \
  sysstr/sysstr_errno.c.in \
  sysstr/sysstr_private.h \
  sysstr.h
	$(CC) $(CFLAGS) -c sysstr/sysstr_errno.c

sysstr_signal.o : sysstr/sysstr_signal.c \
  sysstr/sysno_lookup.c.in \
  sysstr/sysstr_signo.c.in \
  sysstr/sysstr_private.h \
  sysstr.h
	$(CC) $(CFLAGS) -c sysstr/sysstr_signal.c


##
## tain:
##
TAIN_OBJS=\
 tain_assign.o \
 tain_iszero.o \
 tain_less.o \
 tain_load.o \
 tain_load_msecs.o \
 tain_load_utc.o \
 tain_minus.o \
 tain_now.o \
 tain_pack.o \
 tain_packhex.o \
 tain_pause.o \
 tain_plus.o \
 tain_tai_pack.o \
 tain_tai_unpack.o \
 tain_uptime.o \
 tain_to_float.o \
 tain_to_msecs.o \
 tain_to_utc.o \
 tain_unpack.o \
 tain_unpackhex.o \

tain_assign.o : tain/tain_assign.c tain.h
	$(CC) $(CFLAGS) -c tain/tain_assign.c

tain_iszero.o : tain/tain_iszero.c tain.h
	$(CC) $(CFLAGS) -c tain/tain_iszero.c

tain_less.o : tain/tain_less.c tain.h
	$(CC) $(CFLAGS) -c tain/tain_less.c

tain_load.o : tain/tain_load.c tain.h
	$(CC) $(CFLAGS) -c tain/tain_load.c

tain_load_msecs.o : tain/tain_load_msecs.c tain.h
	$(CC) $(CFLAGS) -c tain/tain_load_msecs.c

tain_load_utc.o : tain/tain_load_utc.c tain.h
	$(CC) $(CFLAGS) -c tain/tain_load_utc.c

tain_minus.o : tain/tain_minus.c tain.h
	$(CC) $(CFLAGS) -c tain/tain_minus.c

tain_now.o : tain/tain_now.c tain.h
	$(CC) $(CFLAGS) -c tain/tain_now.c

tain_pack.o : tain/tain_pack.c tain.h
	$(CC) $(CFLAGS) -c tain/tain_pack.c

tain_packhex.o : tain/tain_packhex.c tain.h
	$(CC) $(CFLAGS) -c tain/tain_packhex.c

tain_pause.o : tain/tain_pause.c tain.h
	$(CC) $(CFLAGS) -c tain/tain_pause.c

tain_plus.o : tain/tain_plus.c tain.h
	$(CC) $(CFLAGS) -c tain/tain_plus.c

tain_tai_pack.o : tain/tain_tai_pack.c tain.h
	$(CC) $(CFLAGS) -c tain/tain_tai_pack.c

tain_tai_unpack.o : tain/tain_tai_unpack.c tain.h
	$(CC) $(CFLAGS) -c tain/tain_tai_unpack.c

tain_to_float.o : tain/tain_to_float.c tain.h
	$(CC) $(CFLAGS) -c tain/tain_to_float.c

tain_to_msecs.o : tain/tain_to_msecs.c tain.h
	$(CC) $(CFLAGS) -c tain/tain_to_msecs.c

tain_to_utc.o : tain/tain_to_utc.c tain.h
	$(CC) $(CFLAGS) -c tain/tain_to_utc.c

tain_uptime.o : tain/tain_uptime.c tain.h
	$(CC) $(CFLAGS) -c tain/tain_uptime.c

tain_unpack.o : tain/tain_unpack.c tain.h
	$(CC) $(CFLAGS) -c tain/tain_unpack.c

tain_unpackhex.o : tain/tain_unpackhex.c tain.h
	$(CC) $(CFLAGS) -c tain/tain_unpackhex.c

##
## tx64:
##
TX64_OBJS=\
 base64_vec.o \
 tx64_encode.o \

base64_vec.o : tx64/base64_vec.c tx64.h
	$(CC) $(CFLAGS) -c tx64/base64_vec.c

tx64_encode.o : tx64/tx64_encode.c tx64.h
	$(CC) $(CFLAGS) -c tx64/tx64_encode.c


##
## ufunc:
##
UFUNC_OBJS=\
 ufunc_u32add.o \
 ufunc_u48add.o \

ufunc_u32add.o : ufunc/ufunc_u32add.c ufunc.h
	$(CC) $(CFLAGS) -c ufunc/ufunc_u32add.c

ufunc_u48add.o : ufunc/ufunc_u48add.c ufunc.h
	$(CC) $(CFLAGS) -c ufunc/ufunc_u48add.c

 
##
## upak:
##
UPAK_OBJS=\
 upak_pack.o \
 upak_unpack.o \
 upak_vpack.o \
 upak16_pack.o \
 upak16_unpack.o \
 upak24_pack.o \
 upak24_unpack.o \
 upak32_pack.o \
 upak32_unpack.o \
 upak48_pack.o \
 upak48_unpack.o \
 upak64_pack.o \
 upak64_unpack.o \

upak_pack.o : upak/upak_pack.c upak.h
	$(CC) $(CFLAGS) -c upak/upak_pack.c

upak_unpack.o : upak/upak_unpack.c upak.h
	$(CC) $(CFLAGS) -c upak/upak_unpack.c

upak_vpack.o : upak/upak_vpack.c upak.h
	$(CC) $(CFLAGS) -c upak/upak_vpack.c

upak16_pack.o : upak/upak16_pack.c upak.h
	$(CC) $(CFLAGS) -c upak/upak16_pack.c

upak16_unpack.o : upak/upak16_unpack.c upak.h
	$(CC) $(CFLAGS) -c upak/upak16_unpack.c

upak24_pack.o : upak/upak24_pack.c upak.h
	$(CC) $(CFLAGS) -c upak/upak24_pack.c

upak24_unpack.o : upak/upak24_unpack.c upak.h
	$(CC) $(CFLAGS) -c upak/upak24_unpack.c

upak32_pack.o : upak/upak32_pack.c upak.h
	$(CC) $(CFLAGS) -c upak/upak32_pack.c

upak32_unpack.o : upak/upak32_unpack.c upak.h
	$(CC) $(CFLAGS) -c upak/upak32_unpack.c

upak48_pack.o : upak/upak48_pack.c upak.h
	$(CC) $(CFLAGS) -c upak/upak48_pack.c

upak48_unpack.o : upak/upak48_unpack.c upak.h
	$(CC) $(CFLAGS) -c upak/upak48_unpack.c

upak64_pack.o : upak/upak64_pack.c upak.h
	$(CC) $(CFLAGS) -c upak/upak64_pack.c

upak64_unpack.o : upak/upak64_unpack.c upak.h
	$(CC) $(CFLAGS) -c upak/upak64_unpack.c


## libasagna:
libasagna.a: $(LIB_OBJS)
	ar cr $@ $(LIB_OBJS)
	ranlib $@

## misc targets:
TARGETS: Makefile
	grep '^[a-z].*:' Makefile | cut -d':' -f1 >TARGETS

clean: TARGETS
	rm -f `cat TARGETS` TARGETS

### EOF (Makefile)
