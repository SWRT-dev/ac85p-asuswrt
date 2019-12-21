/* tain.h
** tain: TAI timestamp with nanosecond precision
** wcm, 2008.01.08 - 2011.01.26
** ===
*/
#ifndef TAIN_H
#define TAIN_H 1

#include <stdint.h>
#include <time.h>

#include "uchar.h"

/*
** nanosecond precision timestamp module
*/

/* tain object: */
struct tain {
  /* seconds (if absolute, from TAI epoch 1958.01.01): */
  uint64_t  sec;
  /* nanoseconds (billionths of second): */
  uint32_t  nsec;
};

/* object typedef: */
typedef struct tain tain_t;

/* offset from tai epoch to utc (in seconds): */
#define TAIN_UTC_OFFSET  4611686018427387914ULL

/* buffer sizes required for packed TAI64N and TAI64 labels: */
#define  TAIN_PACK_SIZE    12
#define  TAIN_TAIPACK_SIZE  8

/* buffer size required for tain_t packed as ascii string hexadecimal
** including terminating nul:
*/
#define  TAIN_HEXSTR_SIZE  (24 + 1)


/* tain methods: */

/* tain_load()
** load t->sec and t->nsec from sec, nsec
** return t
*/
extern tain_t * tain_load(tain_t *t, uint64_t sec, uint32_t nsec);

/* tain_LOAD()
** macro version of tain_load()
**   note: no return
*/
#define tain_LOAD(t,s,n) ((t)->sec = (uint64_t)(s), (t)->nsec = (uint32_t)(n))

/* tain_INIT()
** macro for initializing a static tain_t value
** initialize t.sec and t.nsec with sec, nsec
** idiom:
**   tain_t  t = tain_INIT(sec, nsec);
*/
#define tain_INIT(s,n) {(uint64_t)(s), (uint32_t)(n)}

/* tain_now()
** load tai-adjusted gettimeofday() into t
** return t
*/
extern tain_t * tain_now(tain_t *t);

/* tain_assign()
** t1 = t2
** return t1
*/
extern tain_t * tain_assign(tain_t *t1, const tain_t *t2);

/* tain_load_utc()
** tai-adjust unix/utc time() representation and load into t->sec
** return t
*/
extern tain_t * tain_load_utc(tain_t *t, time_t u);

/* tain_to_utc()
** return the absolute tai timestamp in t->sec expressed in utc-adjusted time
*/
extern time_t tain_to_utc(tain_t *t);

/* tain_load_msecs()
** load t with initial value expressed in milliseconds
** return t
*/
extern tain_t * tain_load_msecs(tain_t *t, uint64_t msecs);

/* tain_to_msecs()
** return the value in t expressed as milliseconds
** note: t may overflow return value
** (caller responsible for determining overflow possibility before call)
*/
extern uint64_t tain_to_msecs(tain_t *t);

/* tain_to_float()
** return the value in t as seconds expressed in floating point
*/
extern double tain_to_float(tain_t *t);

/* tain_plus()
** t = t1 + t2
** return t
** note: args and result may overlap
*/
extern tain_t * tain_plus(tain_t *t, const tain_t *t1, const tain_t *t2);

/* tain_minus()
** t = t1 - t2
** return t
** note: args and result may overlap
*/
extern tain_t * tain_minus(tain_t *t, const tain_t *t1, const tain_t *t2);

/* tain_less()
** t1 < t2 ? 1 : 0
*/
extern int tain_less(const tain_t *t1, const tain_t *t2);

/* tain_iszero()
** t == 0 ?
*/
extern int tain_iszero(const tain_t *t);

/* tain_uptime()
**   difference between now and when
**   return seconds
**
**   notes:
**   - think: "when" happened before "now" (now > when)
**   - but this function does accomodate when greater than now
**     eg: a reset of system timeclock moves now before when
**   - if result cast to uint32_t, uptime in seconds to 136 years
*/
extern uint64_t tain_uptime(const tain_t *now, const tain_t *when);

/* tain_pack(), tain_unpack()
** portable 12 byte representation of tain
** (TAI64N external format compatible)
*/
extern uchar_t * tain_pack(uchar_t *buf, const tain_t *t);
extern tain_t * tain_unpack(tain_t *t, const uchar_t *buf);

/* tain_packhex()
**   convert t into a nul-terminated hexadecimal string in s
**   return s
**   note: caller required to supply s of at least TAIN_HEXSTR_SIZE
**   
*/
extern char * tain_packhex(char *s, const tain_t *t);

/* tain_unpackhex()
**   convert hex string representation of tain into t
**   return t
*/
extern tain_t * tain_unpackhex(tain_t *t, const char *s);


/* tain_tai_pack(), tain_tai_unpack()
** 8 byte representation of tain->sec (TAI64 format compatible)
** tain_tai_pack() returns buf
** tain_tai_unpack() returns t
*/
extern uchar_t * tain_tai_pack(uchar_t *buf, const tain_t *t);
extern tain_t * tain_tai_unpack(tain_t *t, const uchar_t *buf);

/* tain_pause()
** wrapper for nanosleep()
*/
extern int tain_pause(const tain_t *t, tain_t *rem);


#endif /* TAIN_H */
/* eof: tain.h */
