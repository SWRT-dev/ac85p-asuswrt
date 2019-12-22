/* cdbmk.h
** cdb: constant (hash) database on disk
** cdbmk: cdb generator (writer)
** wcm, 2010.05.27 - 2010.12.06
** ===
*/

#ifndef CDBMK_H
#define CDBMK_H 1

/* libc: */
#include <stddef.h>  /* size_t */
#include <stdint.h>  /* uint32_t */

/* unix: */
#include <sys/types.h>  /* off_t */

/* lasagna: */
#include "ioq.h"
#include "uchar.h"

/* cdb: */
#include "cdb.h"

/*
** --- cdbmk ---
** cdb file generator
*/

/*
** struct cdbmk: cdb generator object
** (container for stuff used to fabricate a cdb)
*/
struct cdbmk {
  int                    fd; /* open file descriptor for writer */
  off_t                  fp; /* current offset for record in fd */
  /* input partitioned into 4 lists of input blocks: */
#define  CDBMK_NLISTS  4
  struct input_block    *input_list[CDBMK_NLISTS];
  /* counter for entries in each subtable: */
  uint32_t               subtab_count[CDB_NTABS];
  /* ioq buffered output: */
  struct ioq             ioq;
  /* buffer for ioq: */
#define  CDBMK_BUFSIZE  8192
  uchar_t                buf[CDBMK_BUFSIZE];
};    
typedef struct cdbmk  cdbmk_t;


/*
** cdbmk public api
*/

/* cdbmk_init()
**   initialize cdbmk_t object M
**     fd: open file descriptor for cdb output
**
**   return:
**     0 : success, ioq/fd positioned to begin writing record data
**    -1 : failure, errno set (lseek())
**
*/
extern int cdbmk_init(cdbmk_t *M, int fd);


/* cdbmk_add()
**   add a record to cdb
**     key:  key
**     klen: length of key
**     data: data
**     dlen: length of data
**
**    return:
**      0 : success, no error
**     -1 : failure, errno set
*/
extern int cdbmk_add(
  cdbmk_t *M,
  const uchar_t *key, uint32_t klen,
  const uchar_t *data, uint32_t dlen
);


/* cdbmk_addioq()
**   add next record from an ioq reader
**     ioq_in: ioq of input reader positioned for next key/data item
**     klen:   size of key
**     dlen:   size of data
**     sep:    explicit byte/character sequence between key/data
**     seplen: size of sep
**
**   return:
**     0 : success, no error
**    -1 : failure, errno set
**
**   notes:
**     fast, "wirespeed" interface for cdbmk
**     no double-copying of data
**     (but: requires pre-parsed knowledge of klen, dlen)
*/
extern int cdbmk_addioq(
  cdbmk_t   *M,
  ioq_t     *ioq_in,
  size_t     klen,
  size_t     dlen,
  uchar_t   *sep,
  size_t     seplen);


/* cdbmk_addrec()
**   add record to M from current record cursor in C
**
**   return:
**     0 : success, no error
**    -1 : failure, errno set
*/
extern int cdbmk_addrec(cdbmk_t *M, cdb_t *C);


/* cdbmk_finish()
**   complete generation of cdb after all records added
**
**   return:
**     0 : success
**    -1 : failure, errno set
**
**   notes:
**     * on success, caller should fsync()/close() fd
**     * normal convention is then to atomically rename() fd
**     * on return, caller may/(should) also call cdbmk_clear()
*/
extern int cdbmk_finish(cdbmk_t *M);


/* cdbmk_clear()
**   deallocate/reset an cdbmk_t object
**   notes:
**     * used to free any memory allocated by cdbmk*() functions
**     * should be called after cdbmk_finish() or any error
**     * should be called before reusing an cdbmk_t object with cdbmk_init()
*/
extern void cdbmk_clear(cdbmk_t *M);


/*
** cdbmk internals
*/

/* input list partioned according to hash subtable: */
#define  cdbmk_NLIST(ntab) \
  ((ntab) < 128) \
      ? (((ntab) < 64)  ? 0 : 1) \
      : (((ntab) < 192) ? 2 : 3)


/* hash/offset input record: */
struct hashoff {
  uint32_t  hash;  /* hash value of key for this record */
  uint32_t  rpos;  /* record offset */
};


/* input_block:
**   block/node used in linked lists for single-pass store of input records
*/
struct input_block {
/* number of hashoff records per input_block: */
#define CDBMK_NRECS  500
  struct hashoff       record[CDBMK_NRECS];
  int                  n;    /* entries used in hashoffs[] */
  struct input_block  *next; /* linked list */
};


/* cdbmk__update()
**   internal subroutine common to cdbmk_add*() functions
*/
extern int cdbmk__update(struct cdbmk *M, uint32_t hash, uint32_t rpos, uint32_t fp_up);


#endif /* CDBMK_H */
/* eof (cdbmk.h) */
