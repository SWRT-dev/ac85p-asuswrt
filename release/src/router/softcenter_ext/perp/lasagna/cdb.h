/* cdb.h
** constant (hash) database on disk
** wcm, 2010.05.27 - 2012.07.25
** ===
*/

#ifndef CDB_H
#define CDB_H 1

/* libc: */
#include <stddef.h>  /* size_t */
#include <stdint.h>  /* uint32_t */

/* unix: */
#include <unistd.h>  /* off_t */

/* lasagna: */
#include "ioq.h"
#include "dynbuf.h"
#include "uchar.h"

/*
** cdb32 file structure:
** 
**   P  R  H
** 
** where:
** 
**   P: hash table pointer section (8 bytes * 256)
**       P[0..255] hash subtable pointers
**       pointers to 256 hash subtables
**       each pointer comprised of:
**           4 bytes tbase: offset to base of subtable in section H
**           4 bytes slots: number of slots in subtable
** 
**   R: record data section
**       R[0..(nrecs-1)] records
**       nrecs records, each record comprised of:
**           4 bytes klen: length of key in this record
**           4 bytes dlen: length of data in this record
**           klen bytes: key
**           dlen bytes: data
** 
**   H: hash table section
**       H[0..255][0..(slots[t]-1)] hash subtables
**       256 subtables of slots[t] slots per subtable
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
** --- cdb ---
*/

/*
** struct cdb:  cdb32 reader object
*/
struct cdb {
  int       fd;
  /* mmap() on fd: */
  uchar_t  *map;
  uint32_t  map_size;
  /* internal offsets: */
  uint32_t  hbase;  /* base offset to hash table (also eod) */
  /* find parameters: */
  uchar_t  *key;    /* pointer to key (as supplied by cdb_find()) */
  uint32_t  klen;   /* length of key (as supplied by cdb_find()) */
  /* find internals: */
  uint32_t  h;      /* hash: cdb_hash(key, klen) */
  uint32_t  tbase;  /* base offset of hash subtable */
  uint32_t  tslots; /* slots in hash subtable */
  uint32_t  s0;     /* slot probe begins (initial target slot) */
  uint32_t  sN;     /* slot probed for last record found */
  /* retrieval cursors (to found record data): */
  uint32_t  rpos;   /* offset of found record data */
  uint32_t  dlen;   /* length of found record data */
};
typedef struct cdb  cdb_t;


/* cdb_init()
**   initialize an cdb_t object from open file descriptor fd
**   required before any operation
*)*
**   return:
**     0 : success
**    -1 : error, errno set
**
**   note:
**     pair with cdb_clear() on completion
*/
extern int cdb_init(cdb_t *C, int fd);

/* cdb_clear()
**   clear/reset cdb object when finished and/or for reuse
**
**   return:
**     0 : success
**    -1 : error, errno set
**
**   note:
**     application responsible to close/handle any open fd
**     usually paired with cdb_init()
*/
extern int cdb_clear(cdb_t *C);

/* cdb_open()
**   open fd for path then initialize cdb_t object with cdb_init()
**   return:
**     0 : success
**    -1 : error, errno set
**
**   convenience for:
**       fd = open(path, O_RDONLY | O_NONBLOCK);
**       cdb_init(C, fd); 
**     
**   notes:
**     fd is opened: fd = open(path, O_RDONLY | O_NONBLOCK);
**     usually paired with cdb_close() on completion
*/
extern int cdb_open(cdb_t *C, const char *path);

/* cdb_close()
**   close fd for cdb and reset with cdb_clear()
**   return:
**     0 : success
**    -1 : error, errno set
**
**   note:
**     usually paired with cdb_open()
*/
extern int cdb_close(cdb_t *C);


/* cdb_pbase()
**   after cdb_init(), find computed offset to start of hash table pointers
*/
#define cdb_pbase(C)  (0)

/* cdb_rbase()
**   after cdb_init(), find computed offset to start of record block
*/
#define cdb_rbase(C)  (256 * 8)

/* cdb_fd()
**   after cdb_open() (or as set in cdb_init()), get file descriptor
*/
#define cdb_fd(C)  (C)->fd

/* cdb_map()
**   after cdb_init(), access mmap()'d file
**   (valid map if non-NULL)
*/
#define cdb_map(C)  (C)->map

/* cdb_map()
**   after cdb_init(), size of mmap()'d file
**   (valid if map non-NULL)
*/
#define cdb_mapsize(C)  (C)->map_size

/* cdb_find()
**   initialize and begin first search of cdb for key of length klen
**   return:
**     1 : found
**     0 : not found
**    -1 : error, errno set
*/
extern int cdb_find(cdb_t *C, const uchar_t *key, uint32_t klen);

/* cdb_findnext()
**   after successful cdb_find(), continue search for duplicate keys
**   return:
**     1 : found
**     0 : not found
**    -1 : error, errno set
*/
extern int cdb_findnext(cdb_t *C);

/* cdb_dlen()
**   after succesful cdb_find(), return size of associated data
**   note: used to prepare/size buffer for subsequent cdb_get()
**   return: C->dlen
*/
#define  cdb_dlen(C)  ((C)->dlen)

/* cdb_dpos()
**   after succesful cdb_find(), return position of associated data
**   note: used to prepare/size buffer for subsequent cdb_get()
**   return: computed from C->rpos
*/
#define  cdb_dpos(C)  ((C)->rpos + 8 + (C)->klen)

/* cdb_klen()
**   after successful cdb_find(), return size of associated key
**   return: C->klen
*/
#define  cdb_klen(C)  ((C)->klen)

/* cdb_kpos()
**   after successful cdb_find(), return position of associated key
**   return: computed from C->rpos
*/
#define  cdb_kpos(C)  ((C)->rpos + 8)

/* cdb_rpos()
**   after successful cdb_find(), return position of associated record
**   return:  C->rpos
*/
#define  cdb_rpos(C)  ((C)->rpos)

/* cdb_get()
**   after successful cdb_find():
**     copy len bytes of record data at cdb_dpos(C) into buffer
**     buffer supplied by caller is at least len bytes
**     if (len > C->dlen), only dlen bytes copied into buffer 
**
**   return:
**     0 : success
**    -1 : error, errno set 
*/
extern int cdb_get(cdb_t *C, uchar_t *buffer, size_t len);

/* cdb_dynget()
**   after successful cdb_find():
**     copy C->dlen bytes record data at C->dpos cursor into dynbuf
**     copy into dynbuf starts/appends from D->buf[D->p]
**     dynbuf grown as necessary
**
**   return:
**     0 : success
**    -1 : error, errno set
**
**   notes:
**     for copy (not append) into D->buf[0], use dynbuf_CLEAR(D)
**     before each cdb_dynget()
**
*/
extern int cdb_dynget(cdb_t *C, dynbuf_t *D);

/* cdb_distance()
**   after successful cdb_find(), return "distance" from target hash slot
**   may be used as an indicator for hash efficiency
**   return:
**     0 : matching record found in target hash slot (first probe)
**     1 : matching record found 1 slot from target (second probe)
**     2 : matching record found 2 slots from target (third probe)
**     ...
*/
extern uint32_t cdb_distance(cdb_t *C);

/* cdb_seqinit()
**   initiate sequential access to records starting from record 0
**   return:
**     1 : success, positioned to record 0
**     0 : eod (no records in database!)
**    -1 : error, errno set
*/
extern int cdb_seqinit(cdb_t *C);

/* cdb_seqnext()
**   sequential access to next record
**   return:
**     1 : success, positioned to next record
**     0 : eod
**    -1 : error, errno set
*/
extern int cdb_seqnext(cdb_t *C);

/* cdb_read()
**   from offset in C, read/copy len bytes into buf
**   a low-level operation to provide arbitrary access into an cdb
**
**   return:
**     0 : success
**    -1 : error, errno set
**
**   notes:
**     * caller responsible for providing buf of at least len
**     * caller responsible for providing "meaningful" offset
*/
extern int cdb_read(cdb_t *C, uchar_t *buf, size_t len, uint32_t offset);

/* cdb_dynread()
**   from offset in C, read/copy len bytes into dynbuf
**   a low-level operation to provide arbitrary access into an cdb
**   copy into dynbuf appends from D->buf[D->p]
**   (dynbuf grown as necessary)
**    
**   return:
**     0 : success
**    -1 : error, errno set
**
**   notes:
**     for copy (not append) into D->buf[0], use dynbuf_CLEAR(D)
**     before each cdb_dynread()
*/
extern int cdb_dynread(cdb_t *C, dynbuf_t *B, size_t len, uint32_t offset);

/* cdb_cc()
**   duplicate an initialized cdb_t object from src to dest
**   return pointer to cdb_t object dest
**
**   notes:
**     provided to save state and/or multiplex cdb operations
**     on the same underlying data file
*/
extern cdb_t * cdb_cc(cdb_t *dest, const cdb_t *src);


/*
** --- hash function ---
*/

#define CDB_NTABS       256
#define CDB_HASHINIT    5381

/* incremental hashing support provided for cdbmk:
**
**   h = CDB_HASHINIT;
**   while(klen){
**       // hash next single byte in key:
**       h = cdb_hashnext(h, *key);
**       ++key; --kley;
**   }
**
**   //or
**
**   h = CDB_HASHINIT;
**   for(;;){
**       // hash next part of key:
**       h = cdb_hashpart(h, *keypart, partlen);
**       ...
**   }
*/

extern
uint32_t
cdb_hashpart(uint32_t h, const uchar_t *keypart, size_t partlen);

#define cdb_hash(key, klen)  \
    cdb_hashpart(CDB_HASHINIT, (key), (klen))


/* table/slot algorithms: */
/* basis for partitioning hash value among 256 subtables: */
#define  cdb_NTAB(hash)  ((hash) & 255)

/* basis for selection of slot within tslots of subtable: */
#define  cdb_SLOT(hash, tslots)  ((hash>>8) % (tslots))


#endif /* CDB_H */
/* eof (cdb.h) */
