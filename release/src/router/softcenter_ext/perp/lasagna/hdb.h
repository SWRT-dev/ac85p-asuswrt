/* hdb.h
** constant (hash) database on disk
** wcm, 2010.05.27 - 2012.07.25
** ===
*/

#ifndef HDB_H
#define HDB_H 1

/* libc: */
#include <stddef.h>  /* size_t */
#include <stdint.h>  /* uint32_t */

/* unix: */
#include <unistd.h>     /* off_t */
#include <sys/types.h>  /* off_t */

/* lasagna: */
#include "ioq.h"
#include "dynbuf.h"
#include "uchar.h"

/*
** hdb32 file structure:
** 
**   I  M  P  C  R  H
** 
** where:
** 
**   I: identifier
**       16 byte id string: "hdb32/1.0\0\0\0\0\0\0\0"
**
**   M: metadata
**        4 bytes: nrecs, number of records
**        4 bytes: rbase, offset to start of record data
**
**   P: hash table pointer section (8 bytes * 8)
**       P[0..7] hash subtable pointers
**       pointers to 8 hash subtables
**       each pointer comprised of:
**           4 bytes slots: number of slots in subtable
**           4 bytes tbase: offset to base of subtable in section H
**
**   C: descriptive comment
**       arbitrary byte string of any length 
**       length computed from: rbase - cbase
**       (cbase is a constant 16 + 8 + 64 = 88 bytes offset in file)
**
**   R: record data section
**       R[0..(nrecs-1)] records
**       nrecs records, each record comprised of:
**           3 bytes klen: length of key in this record
**           3 bytes dlen: length of data in this record
**           klen bytes: key
**           dlen bytes: data
**       note: klen/dlen each limited to (2^24 - 1) = 16M
** 
**   H: hash table section
**       H[0..7][0..(slots[t])] hash subtables
**       8 subtables of slots[t] slots per subtable
**       each slot comprised of:
**           4 bytes: hash of key for record inserted in this slot [*1]
**           4 bytes: rpos, offset to record entry or 0 (empty)
**
**   Notes:
**       1.  The key hash value included in each hash table slot is not
**           strictly required to resolve record searches, but improves
**           search performance in cases of key collisions (enables successive
**           entries in the hash table to be scanned directly, rather than
**           performing a separate record lookup for each collison test).
*/


/*
** --- hdb ---
** hdb file reader
*/

/* ident string: */
#define HDB_IDLEN  16
#define HDB32_IDENT  "hdb32/1.0\0\0\0\0\0\0\0"

/* number of hash subtables: */
#define HDB_NTABS  8

/* offset to comment area: */
#define HDB_CBASE  (HDB_IDLEN + 8 + (HDB_NTABS * 8))

/* constraint on klen, dlen for 3-byte packing: */
#define HDB_U24MAX  ((1 << 24) - 1)

/*
** struct hdb_valoff: generic container for value/offset pair
*/
struct hdb_valoff{
  uint32_t  value;
  uint32_t  offset;
};
typedef struct hdb_valoff  hdb_valoff_t;

/*
** struct hdb:  hdb32 reader object
*/
struct hdb {
  int       fd;
  /* mmap() on fd: */
  uchar_t  *map;
  uint32_t  map_size;
/*
** loaded from header by hdb_init()
*/
  /* metadata: */
  uint32_t  nrecs;  /* total number of records */
  /* hash subtable index: */
  struct hdb_valoff  tndx[HDB_NTABS];
  /* internal offsets: */
  uint32_t  cbase;  /* base offset to comment area */
  uint32_t  rbase;  /* base offset to record data */
  uint32_t  hbase;  /* base offset to hash table (also eod) */
/*
** internal cursors
*/
  /* find parameters: */
  uchar_t  *key;    /* pointer to key (as supplied by hdb_find()) */
  uint32_t  klen;   /* length of key (as supplied by hdb_find()) */
  /* find internals: */
  uint32_t  h;      /* hash value: hdb_hash(key, klen) */
  uint32_t  tbase;  /* base offset of hash subtable */
  uint32_t  tslots; /* slots in hash subtable */
  uint32_t  s0;     /* slot probe begins (initial target slot) */
  uint32_t  sN;     /* slot probed for last record found */
  /* retrieval cursors (to found record data): */
  uint32_t  rpos;   /* offset of found record data */
  uint32_t  dlen;   /* length of found record data */
};
typedef struct hdb  hdb_t;


/* hdb_init()
**   initialize an hdb_t object from open file descriptor fd
**   required before any operation
*)*
**   return:
**     0 : success
**    -1 : error, errno set
**
**   note:
**     pair with hdb_clear() on completion
*/
extern int hdb_init(hdb_t *H, int fd);

/* hdb_clear()
**   clear/reset hdb object when finished and/or for reuse
**
**   return:
**     0 : success
**    -1 : error, errno set
**
**   note:
**     application responsible to close/handle any open fd
**     usually paired with hdb_init()
*/
extern int hdb_clear(hdb_t *H);

/* hdb_open()
**   open fd for path then initialize hdb_t object with hdb_init()
**   return:
**     0 : success
**    -1 : error, errno set
**
**   convenience for:
**       fd = open(path, O_RDONLY | O_NONBLOCK);
**       hdb_init(H, fd); 
**     
**   notes:
**     fd is opened: fd = open(path, O_RDONLY | O_NONBLOCK);
**     usually paired with hdb_close() on completion
*/
extern int hdb_open(hdb_t *H, const char *path);

/* hdb_close()
**   close fd for hdb and reset with hdb_clear()
**   return:
**     0 : success
**    -1 : error, errno set
**
**   note:
**     usually paired with hdb_open()
*/
extern int hdb_close(hdb_t *H);

/* hdb_cbase()
**   after hdb_init(), offset to start of comment area
*/
#define hdb_cbase(H)  (H)->cbase

/* hdb_clen()
**   after hdb_init(), length of comment area (assume no terminal nul):
*/
#define hdb_clen(H)  (hdb_rbase((H)) - hdb_cbase((H)))

/* hdb_hase()
**   after hdb_init(), offset to hash tables (also eod)
*/
#define hdb_hbase(H)  (H)->hbase

/* hdb_rbase()
**   after hdb_init(), find computed offset to start of record block
*/
#define hdb_rbase(H)  (H)->rbase

/* hdb_fd()
**   after hdb_open() (or as set in hdb_init()), get file descriptor
*/
#define hdb_fd(H)  (H)->fd

/* hdb_map()
**   after hdb_init(), access mmap()'d file
**   (valid map if non-NULL)
*/
#define hdb_map(H)  (H)->map

/* hdb_map()
**   after hdb_init(), size of mmap()'d file
**   (valid if map non-NULL)
*/
#define hdb_mapsize(H)  (H)->map_size

/* hdb_find()
**   initialize and begin first search of hdb for key of length klen
**   return:
**     1 : found
**     0 : not found
**    -1 : error, errno set
*/
extern int hdb_find(hdb_t *H, const uchar_t *key, uint32_t klen);

/* hdb_findnext()
**   after successful hdb_find(), continue search for duplicate keys
**   return:
**     1 : found
**     0 : not found
**    -1 : error, errno set
*/
extern int hdb_findnext(hdb_t *H);

/* hdb_dlen()
**   after succesful hdb_find(), return size of associated data
**   note: used to prepare/size buffer for subsequent hdb_get()
**   return: H->dlen
*/
#define  hdb_dlen(H)  ((H)->dlen)

/* hdb_dpos()
**   after succesful hdb_find(), return position of associated data
**   note: used to prepare/size buffer for subsequent hdb_get()
**   return: computed from H->rpos
*/
#define  hdb_dpos(H)  ((H)->rpos + 6 + (H)->klen)

/* hdb_klen()
**   after successful hdb_find(), return size of associated key
**   return: H->klen
*/
#define  hdb_klen(H)  ((H)->klen)

/* hdb_kpos()
**   after successful hdb_find(), return position of associated key
**   return: computed from H->rpos
*/
#define  hdb_kpos(H)  ((H)->rpos + 6)

/* hdb_rpos()
**   after successful hdb_find(), return position of associated record
**   return:  H->rpos
*/
#define  hdb_rpos(H)  ((H)->rpos)

/* hdb_get()
**   after successful hdb_find():
**     copy len bytes of record data at hdb_dpos(H) into buffer
**     buffer supplied by caller is at least len bytes
**     if (len > H->dlen), only dlen bytes copied into buffer 
**
**   return:
**     0 : success
**    -1 : error, errno set 
*/
extern int hdb_get(hdb_t *H, uchar_t *buffer, size_t len);

/* hdb_dynget()
**   after successful hdb_find():
**     copy H->dlen bytes record data at H->dpos cursor into dynbuf
**     copy into dynbuf starts/appends from D->buf[D->p]
**     dynbuf grown as necessary
**
**   return:
**     0 : success
**    -1 : error, errno set
**
**   notes:
**     for copy (not append) into D->buf[0], use dynbuf_CLEAR(D)
**     before each hdb_dynget()
**
*/
extern int hdb_dynget(hdb_t *H, dynbuf_t *D);

/* hdb_distance()
**   after successful hdb_find(), return "distance" from target hash slot
**   may be used as an indicator for hash efficiency
**   return:
**     0 : matching record found in target hash slot (first probe)
**     1 : matching record found 1 slot from target (second probe)
**     2 : matching record found 2 slots from target (third probe)
**     ...
*/
extern uint32_t hdb_distance(hdb_t *H);

/* hdb_seqinit()
**   initiate sequential access to records starting from record 0
**   return:
**     1 : success, positioned to record 0
**     0 : eod (no records in database!)
**    -1 : error, errno set
*/
extern int hdb_seqinit(hdb_t *H);

/* hdb_seqnext()
**   sequential access to next record
**   return:
**     1 : success, positioned to next record
**     0 : eod
**    -1 : error, errno set
*/
extern int hdb_seqnext(hdb_t *H);

/* hdb_read()
**   from offset in H, read/copy len bytes into buf
**   a low-level operation to provide arbitrary access into an hdb
**
**   return:
**     0 : success
**    -1 : error, errno set
**
**   notes:
**     * caller responsible for providing buf of at least len
**     * caller responsible for providing "meaningful" offset
*/
extern int hdb_read(hdb_t *H, uchar_t *buf, size_t len, uint32_t offset);

/* hdb_dynread()
**   from offset in H, read/copy len bytes into dynbuf
**   a low-level operation to provide arbitrary access into an hdb
**   copy into dynbuf appends from D->buf[D->p]
**   (dynbuf grown as necessary)
**    
**   return:
**     0 : success
**    -1 : error, errno set
**
**   notes:
**     for copy (not append) into D->buf[0], use dynbuf_CLEAR(D)
**     before each hdb_dynread()
*/
extern int hdb_dynread(hdb_t *H, dynbuf_t *B, size_t len, uint32_t offset);

/* hdb_cc()
**   duplicate an initialized hdb_t object from src to dest
**   return pointer to hdb_t object dest
**
**   notes:
**     provided to save state and/or multiplex hdb operations
**     on the same underlying data file
*/
extern hdb_t * hdb_cc(hdb_t *dest, const hdb_t *src);


/*
** --- hash function ---
*/

#define HDB_HASHINIT    0x0
#define HDB_HWORD       16
#define HDB_SHIFT       3

/* table/slot algorithms: */
/* basis for partitioning hash value among subtables: */
#define  hdb_NTAB(hash)  ((hash) & (HDB_NTABS - 1))

/* basis for selection of slot within tslots of subtable: */
#define  hdb_XMIX(hash)  (((hash) >> (HDB_HWORD - HDB_SHIFT)) ^ (hash))
#define  hdb_SLOT(hash, tslots)  ((hdb_XMIX((hash)) >> HDB_SHIFT) % (tslots))


/* incremental hashing support provided for hdbmk:
**
**   h = HDB_HASHINIT;
**   for(;;){
**       // hash next part of key:
**       h = hdb_hashpart(h, *keypart, partlen);
**       ...
**   }
*/

extern
uint32_t
hdb_hashpart(uint32_t h, const uchar_t *keypart, size_t partlen);

#define hdb_hash(key, klen) \
  hdb_hashpart(HDB_HASHINIT, (key), (klen))


#endif /* HDB_H */
/* eof (hdb.h) */
