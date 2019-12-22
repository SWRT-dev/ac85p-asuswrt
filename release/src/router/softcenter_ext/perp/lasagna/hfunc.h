/* hfunc.h
** hash functions
** wcm, 2010.06.04 - 2010.10.25
** ===
*/

#ifndef HFUNC_H
#define HFUNC_H 1

/* libc: */
#include <stddef.h>  /* size_t */
#include <stdint.h>  /* uint32_t */

/* lasagna: */
#include "uchar.h"

/*
** possibly useful #define constants:
*/

/* fnv32 init/mult parameters: */
/* init = 2166136261: */
#define HFUNC_FNV32_BASIS ((uint32_t)0x0811c9dc5)
/* mult = 16777619: */
#define HFUNC_FNV32_PRIME ((uint32_t)0x01000193)

/* prime nearest (phi * 2^32) is 2654435761: */
#define HFUNC_PHI32_PRIME ((uint32_t)0x09e3779b1)


/*
** possibly useful macros:
*/

/* hfunc_WXOR()
**   xor high word into low word on hash function result
**   per Peter Kankowski[*6]
**
**   <quote>
**     For table size less than 2^16, we can improve the quality of
**     hash function by XORing high and low words, so that more letters
**     will be taken into account.
**   </quote>
**
**   usage:
**
**     hash = hfunc_WX(hfunc_xxxx(key, klen));
**
*/
#define hfunc_WXOR(h) ((h) ^ ((h) >> 16))


/*
** possibly useful helper functions:
*/

/* hfunc_postmix32()
**   apply post-process mixing to hashed value h
**
**   example:
**
**     hash = hfunc_postmix32(hfunc_ghfx(key, klen, 5381, 33));
**
**   notes:
**     * not a hash function in itself
**
**     * used for post-process mixing of a hash result that
**       doesn't otherwise provide it's own post-processing
**
**     * based on mods to fnv-1a suggested by bret mulvey[*8]
**
**     * implemented internally in fnvm(), ghfm(), djbm(), rotm()
**
**   XXX, wanted: in-place macro version!
*/
extern uint32_t  hfunc_postmix32(uint32_t h);


/*
** basic generalized hash functions:
*/

/* hfunc_ghfa()
**   general hash function "add":
**   (each successive byte of key added to result)
**
**   mix preceeds combine:
**       combine operation is add
**       mix operation is mult
**
**   no post-processing
*/
extern uint32_t hfunc_ghfa(const uchar_t *key, size_t klen, uint32_t init, uint32_t mult);

/* hfunc_ghfx()
**   general hash function "xor":
**   (each successive byte of key xor'ed with result)
**
**   mix preceeds combine:
**       combine operation is xor
**       mix operation is mult
**
**   no post-processing
*/
extern uint32_t hfunc_ghfx(const uchar_t *key, size_t klen, uint32_t init, uint32_t mult);

/* hfunc_ghfm()
**   general hash function "mix":
**
**   combine preceeds mix:
**       combine operation is xor
**       mix operation is mult
**
**   post-processing:
**       mix w/ shifts/adds/xors (as in postmix32())
**
**   note:
**       a generalized template of fnvm() described below
*/
extern uint32_t hfunc_ghfm(const uchar_t *key, size_t klen, uint32_t init, uint32_t mult);


/*
** functions derived from generalized versions of ghfa() and ghfx() above,
** but defined with specific parameters
**
** note:
**   although the following hash functions may be parameterized to use
**   the general hash functions declared above, these are implemented
**   with hard-coded parameters and bit operations for efficiency
*/

/* hfunc_djba()
**   djb "add": ghfa with i=5381, m=33
**   credited to Daniel Bernstein
*/
extern uint32_t hfunc_djba(const uchar_t *key, size_t klen);

/* hfunc_djbx()
**   djb "xor": ghfx with i=5381, m=33
**   hash function used in cdb database, Daniel Bernstein
*/
extern uint32_t hfunc_djbx(const uchar_t *key, size_t klen);

/* hfunc_fnv1()
**   "fowler/noll/vo" hash
**   fnv "xor": ghfx with i=2166136261, m=16777619
**   (params setup for 32-bit hash value)
**
**   notes:
**     see hfunc_fnva() in next section below for FNV-1a
**     see hfunc_fnvm() in next section below for modified FNV-1a
**     see fnv website [*2]
*/
#define hfunc_fnv1(k,l) \
  hfunc_ghfx((k), (l), HFUNC_FNV32_BASIS, HFUNC_FNV32_PRIME);

/* hfunc_kp37()
**   kernighan/pike: ghfa with i=0, m=37
**   TPOP/kp, p57
*/
extern uint32_t hfunc_kp37(const uchar_t *key, size_t klen);

/* hfunc_kr31()
**   kernighan/ritchie: ghfa with i=0, m=31
**   k&rc/2e, p144
**
**   note: also as in nawk
*/
extern uint32_t hfunc_kr31(const uchar_t *key, size_t klen);

/* hfunc_nawk()
**   nawk: ghfa with i=0, m=31
**   hash function found in nawk
**   defined as an alias for kr31()
*/
#define hfunc_nawk(k,l)  hfunc_kr31((k),(l))

/* hfunc_p50a()
**   p50a: ghfa with i=0, m=33
**   hash function found in perl 5 (versions < 5.8)
**
**   note:
**   starting from (roughly) 5.8, perl uses a version of hfunc_oata()
*/
extern uint32_t hfunc_p50a(const uchar_t *key, size_t klen);

/* hfunc_sdbm()
**   sdbm: ghfa with i=0, m=65599
**   original attribution?
**     sdbm hash function, ozan yigit
**     also in gawk
**     also "red dragon book"
**
**   note:
**     (h * 65599) == ((h<<6) + (h<<16) - h)
**     2^6 + 2^65536 - 1 = 65599
*/
extern uint32_t hfunc_sdbm(const uchar_t *key, size_t klen);


/*
**  other hash functions
*/

/* hfunc_djbm()
**   djb hash, "modified": ghfm() with init=5831, mult=33
**
**   ie, djbx() with modifications as for fnvm():
**     mix/combine reordered
**     post-process mixing applied as with postmix32()
**     
*/
extern uint32_t hfunc_djbm(const uchar_t *key, size_t klen);

/* hfunc_elf1()
**   unix ELF object file hash
**   slight modification of PJW (pjw1 below)
*/
extern uint32_t hfunc_elf1(const uchar_t *key, size_t klen);

/* hfunc_fnva()
**   "fowler/noll/vo" alternate hash, FNV-1a
**   (params setup for 32-bit hash value)
**
**   notes:
**     FNV-1a
**       params identical to fnv1
**       but: internal operations ordered slightly differently
**
**     <quote>
**       Some people use FNV-1a instead of FNV-1 because they see slightly
**       better dispersion for tiny (<4 octets) chunks of memory.
**     </quote>
**
**   see fnv website [*2]
*/
extern uint32_t hfunc_fnva(const uchar_t *key, size_t klen);

/* hfunc_fnvm()
**   FNV-1a "modified", by Bret Mulvey[*8]
**   adds post-process mixing, eliminates avalanching
**
**   note:
**     see ghfm() above for generalized/parameterized version
**     internal post-process mixing as with postmix32()
*/
extern uint32_t hfunc_fnvm(const uchar_t *key, size_t klen);

/* hfunc_jsw1()
**   JSW, devised/enumerated by Julienne Walker[*1]
**   uses internal random[0..255] table of bytes
**   each successive byte key[i] mapped to random[key[i]]
**
**   note/XXX:
**   this function should be parameterized for the random key map
*/
extern uint32_t hfunc_jsw1(const uchar_t *key, size_t klen);

/* hfunc_kx17()
**   Peter Kankowski[*6] "x17"
**   much like ghfa with init=0, mult=17
**   except: a constant of 32 is subtracted from each key byte before mixing
**
**   theory:
**   <quote>
**     x17 hash function subtracts a space from each letter to cut off
**     the control characters in the range 0x00..0x1F. If the hash keys
**     are long and contain only Latin letters and numbers, the letters
**     will be less frequently shifted out, and the overall number of
**     collisions will be lower. You can even subtract 'A' when you know
**     that the keys will be only English words.
**   </quote>
**   see Peter Kanowski's website at [*6]   
**
**   note/XXX:
**   this function could be parameterized for the subtraction constant,
**   ...but isn't
*/
extern uint32_t hfunc_kx17(const uchar_t *key, size_t klen);

/* hfunc_murm()
**   "MurmurHash 2"
**   algorithm Austin Appleby[*7]
**
**   processes 4-bytes of key per loop
**   internal design somewhat like "sfh1" below
**   compares favorably in published benchmarks[*6]
*/
extern uint32_t hfunc_murm(const uchar_t *key, size_t klen);

/* hfunc_oat1()
**   "one-at-a-time", Bob Jenkins[*3]
**   enumerated also by Julienne Walker[*1]
**   also adapted in perl > 5.8
*/
extern uint32_t hfunc_oat1(const uchar_t *key, size_t klen);

/* hfunc_pjw1()
**   attributed to P.J. Weinberger
**   numerous implementations
**   see Compilers: ..., Aho/Sethi/Ullman
*/
extern uint32_t hfunc_pjw1(const uchar_t *key, size_t klen);

/* hfunc_rot1()
**   rotating hash
**   attributed to Knuth, The Art of Programming
*/
extern uint32_t hfunc_rot1(const uchar_t *key, size_t klen);

/* hfunc_rotm()
**   rotating hash, modified
**   modifications similar to fnvm():
**     reordered mix/combine operations
**     internal post-process mixing as with postmix32()
*/
extern uint32_t hfunc_rotm(const uchar_t *key, size_t klen);

/* hfunc_rsuh()
**   "universal hash function", Robert Sedgewick
**   derived from:
**     Algorithms in C, 3rd Edition
**     p. 579 (Program 14.2)
**
**   simple pseudo-random sequence for mix multiplier
*/
extern uint32_t hfunc_rsuh(const uchar_t *key, size_t klen);

/* hfunc_sax1()
**   "shift/add/xor"
**   enumerated by Julienne Walker[*1]
**   (original credit?)
*/ 
extern uint32_t hfunc_sax1(const uchar_t *key, size_t klen);

/* hfunc_sfh1()
**   "super fast hash"
**   algorithm Paul Hsieh[*5]
**   (BSD license)
*/
extern uint32_t hfunc_sfh1(const uchar_t *key, size_t klen);


/* misc references cited:
**
**   [1]: Julienne Walker, "Eternally Confuzzled - The Art of Hashing"
**   http://www.eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx
**
**   [2]: FNV hash
**   http://www.isthe.com/chongo/tech/comp/fnv/
**
**   [3]: Bob Jenkins on hash functions
**   http://burtleburtle.net/bob/hash/doobs.html
**   http://burtleburtle.net/bob/hash/examhash.html
**
**   [4]: Chris Savoie, comparison table of hash functions
**   http://chrissavoie.com/index.php?option=com_content&task=view&id=65&Itemid=1
**
**   [5]: Paul Hsieh, Super Fast Hash
**   http://www.azillionmonkeys.com/qed/hash.html
**
**   [6]: Peter Kankowski, An empirical comparison
**   http://www.strchr.com/hash_functions
**
**   [7]: Austin Appleby, Murmur Hash
**   http://sites.google.com/site/murmurhash/
**
**   [8]: Bret Mulvey, Hash Functions
**   http://bretm.home.comcast.net/~bretm/hash/
**
** (links current at 2010.05.06)
**   
*/


/* comment:
**
** This module contains a collection of a few hash functions I was
** exploring during the development of the hdb/hash database.  The
** collection may contain errors and is not exhaustive.
** 
** Most of the functions follow a similar "skeleton" pattern.  Here
** is the usual conventional hash function as found in "classic"
** texts:
** 
**   uint32_t hf(key, klen)
**   {
**     uint32_t h = INIT;
**     while(klen){
**       h = MIX(h);  // mix before add
**       h = ADD(h, *key);
**       ++key; --klen;
**     }
**     return h;
**   }
** 
** As shown in the snippet above, the hash is first initialized to
** some value INIT.  Then, for each byte of key, the hash is first
** mixed by a MIX operation, and then the new byte is combined with
** the hash by an ADD operation.
** 
** When the loop is finished, the last computed value of the hash
** is returned.
** 
** The variances among "classic" hash functions involve small
** differences in the assignment of INIT, the MIX operation, and
** the ADD operation.  Generally INIT is usually zero, sometimes
** the klen argument, or otherwise set to some prime number:
** 
**     h = 0;
**     h = klen;
**     h = 5381;
** 
** The MIX operation is usually a multiplication by some constant
** factor, usually prime:
** 
**     h = h * 37;
**     h = h * 33;
** 
** In many cases this MIX multiplication operation is recast to
** bit shift operations, providing the same results as above:
** 
**     h = (h << 5) + (h << 2) + h;  // h *= 37
**     h = (h << 5) + h;             // h *= 33
** 
** The ADD operation is usually one of add or xor:
** 
**     h = h + *key;
**     h = h ^ *key;
** 
** Although the "classic" hash function formula is still widely
** practiced, many have observed a weakness in the model.  Namely,
** that the value of the last key byte is strongly "imprinted" in
** the hash result.  Instead of producing a reliably random
** distribution across all bits of the hash, the final ADD of the
** last key byte leaves a bias from that last byte within the lower
** bits of the hash.
** 
** A couple of strategies to deal with this weakness are found in
** current practice.  The first strategy is to simply reorder the
** MIX and ADD operations, so that the key byte is added into the
** hash _before_ the mix operation.
** 
**   uint32_t hf(key, klen)
**   {
**     uint32_t h = INIT;
**     while(klen){
**       h = ADD(h, *key); // add before mix
**       h = MIX(h);
**       ++key; --klen;
**     }
**     return h;
**   }
** 
** The second strategy is to add a post-processing function to the
** hash.  Its purpose is to give all the bits of the hash a good
** stir after the final key byte has been added:
** 
**   uint32_t hf(key, klen)
**   {
**     uint32_t h = INIT;
**     while(klen){
**       h = ADD(h, *key); // add before mix
**       h = MIX(h);
**       ++key; --klen;
**     }
**     h = POSTMIX(h);  // post-processing
**     return h;
**   }
** 
** An example of a post-processing mix is suggested by Bret Mulvey[*8]:
** 
**   h += h << 13;
**   h ^= h >> 7;
**   h += h << 3;
**   h ^= h >> 17;
**   h += h << 5;
** 
** Most hash function attempting to achieve a "best practice" will
** now employ both strategies. That is:
**
**   * add before mix
**   * post-process mix
**
** The objective of any hash function is to generate a single value
** that maps some key expression into some fixed integral range.
** And further, that such mapping will be spread uniformly and
** randomly among the range for any given set of keys.
**
** Many hash functions implicitly suppose that the individual bytes
** within keys will themselves be uniformly distributed among the
** range of possible byte values, 0..255.  In actual practice,
** however, most key systems are expressed in alpha and/or numeric
** characters that span just a small subset of the possible byte
** values.  Individual key bytes -- and especially the individual
** bits within them --  are generally found to be "clustered" around
** a limited range of values.  A poor hash function will tend to
** preserve this clustering by generating a clustered distribution
** of hash values.  Clustered hash values, in turn, will cause
** insertion collisions and performance degradation, particularly
** in direct-addressing hash tables.
** 
** This problem of key byte clustering may be further amplified
** when key strings are also of fixed length.  The mix-first/add-last
** algorithms are the worst to use in these scenarios.
** 
** A few of the algorithms here are interesting in that they explore
** alternative variations to the "classic" functions.  The "jsw1"
** function suggested by Julien Walker, for example, first maps
** individual key bytes into a table of random integers, prior to
** the ADD operation.  Although input key bytes may still be poorly
** distributed, the random mapping will at least tend to spread
** their values over a greater range of bits.
** 
** Another of the more interesting functions in this library is
** "Murmur2" ("murm") by Austin Appleby.  Although it is somewhat
** more complex than the "classic" functions, it processes keys
** 4-bytes at a time, so running-time is quite good.  It performs
** a throrough mix on each 4-byte key chunk before the combine
** operation.  Although it does perform the hash mix before the
** key add operation, the preliminary key mixing coupled with a
** post-processing hash mix tends to result in very good distributions.
**
** The "Super Fast Hash" by Paul Hsieh ("sfh1") is similar to
** "Murmur2" in that it processes 4-byte key chunks and provides
** a good post-processing mix.  It is the only algorithm here that
** is *not* public domain, however.
**
** The hdb/hash database has a special requirement that favors hash
** functions that can be performed incrementally.  That is, hash
** functions that may be setup to sequentially process an arbitrary
** number of key bytes at a time.  All of the "classic" functions
** may be easily factored into incremental versions.  Functions like
** "Murmur2" are more difficult to factor into incremental processing,
** unless one can impose certain restrictions:  all increments must
** be in 4-byte chunks except the last; must call the post-processing
** mix explicitly.
**
*/ 

#endif /* HFUNC_H */
/* eof (hfunc.h) */
