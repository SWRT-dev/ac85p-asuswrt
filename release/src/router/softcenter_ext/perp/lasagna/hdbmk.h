/* hdbmk.h
** hdb: hdb32 hash database
** hdbmk: hdb generator (writer)
** wcm, 2010.05.27 - 2010.12.14
** ===
*/

#ifndef HDBMK_H
#define HDBMK_H 1

/* libc: */
#include <stddef.h>  /* size_t */
#include <stdint.h>  /* uint32_t */

/* unix: */
#include <sys/types.h>  /* off_t */

/* lasagna: */
#include "ioq.h"
#include "uchar.h"

/* hdb: */
#include "hdb.h"

/*
** --- hdbmk ---
** hdb file generator
*/

/*
** struct hdbmk: hdb generator object
** (container for stuff used to fabricate a hdb)
*/
struct hdbmk {
  int                  fd;    /* open file descriptor for writer */
  off_t                fp;    /* current offset for record in fd */
  uint32_t             rbase; /* beginning of record block */
  /* input partitioned into HDB_NTABS lists of hdbmk_blocks: */
  struct hdbmk_block  *block_list[HDB_NTABS];
  /* counter for entries in each subtable: */
  uint32_t             subtab_count[HDB_NTABS];
  /* ioq buffered output: */
  struct ioq           ioq;
  /* buffer for ioq: */
#define HDBMK_BUFSIZE  8192
  uchar_t              buf[HDBMK_BUFSIZE];
};    
typedef struct hdbmk  hdbmk_t;


/*
** hdbmk public api
*/

/* hdbmk_start()
**   initialize hdbmk_t object M
**     fd:      open file descriptor for hdb output
**     comment: arbitrary string of bytes
**     clen:    size of comment
**
**   return:
**     0 : success, ioq/fd positioned to begin writing record data
**    -1 : failure, errno set (lseek())
**
*/
extern int hdbmk_start(hdbmk_t *M, int fd, const uchar_t *comment, uint32_t clen);


/* hdbmk_add()
**   add a record to hdb
**     key:  key
**     klen: length of key
**     data: data
**     dlen: length of data
**
**    return:
**      0 : success, no error
**     -1 : failure, errno set
*/
extern int hdbmk_add(
  hdbmk_t *M,
  const uchar_t *key, uint32_t klen,
  const uchar_t *data, uint32_t dlen
);


/* hdbmk_addioq()
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
**     fast, "wirespeed" interface for hdbmk
**     no double-copying of data
**     (but: requires pre-parsed knowledge of klen, dlen)
*/
extern int hdbmk_addioq(
  hdbmk_t   *M,
  ioq_t     *ioq_in,
  size_t     klen,
  size_t     dlen,
  uchar_t   *sep,
  size_t     seplen);


/* hdbmk_addrec()
**   add record to M from current record cursor in hdb H
**   (used in set operations)
**
**   return:
**     0 : success, no error
**    -1 : failure, errno set
*/
extern int hdbmk_addrec(hdbmk_t *M, hdb_t *H);


/* hdbmk_finish()
**   complete generation of hdb after all records added
**
**   return:
**     0 : success
**    -1 : failure, errno set
**
**   notes:
**     * on success, caller should fsync()/close() fd
**     * normal convention is then to atomically rename() fd
**     * on return, caller may/(should) also call hdbmk_clear()
*/
extern int hdbmk_finish(hdbmk_t *M);


/* hdbmk_clear()
**   deallocate/reset an hdbmk_t object
**   notes:
**     * used to free any memory allocated by hdbmk*() functions
**     * should be called after hdbmk_finish() or any error
**     * should be called before reusing an hdbmk_t object with hdbmk_init()
*/
extern void hdbmk_clear(hdbmk_t *M);


/*
** hdbmk internals
*/

/* input_block:
**   block/node used in linked lists for single-pass store of input records
*/
struct hdbmk_block {
/* number of hdb_valoff records per input_block: */
#define HDBMK_NRECS    256
  struct hdb_valoff    record[HDBMK_NRECS];
  int                  n;    /* entries used in hashoffs[] */
  struct hdbmk_block  *next; /* linked list */
};
typedef struct hdbmk_block hdbmk_block_t;


/* hdbmk__update()
**   internal subroutine common to hdbmk_add*() functions
*/
extern int hdbmk__update(struct hdbmk *M, uint32_t hash, uint32_t rpos, uint32_t fp_up);


#endif /* HDBMK_H */
/* eof (hdbmk.h) */
