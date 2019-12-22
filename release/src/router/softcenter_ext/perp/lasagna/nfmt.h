/* nfmt.h
** format numeric types
** wcm, 2004.04.19 - 2011.02.01
** ===
*/
#ifndef NFMT_H
#define NFMT_H 1

#include <stddef.h>    /* sizet_t */
#include <stdint.h>

/*
** size of char buffer to hold maximum uint128_t value + nul
** (planning for 16 byte integers ...):
*/
#define NFMT_SIZE  40

/*
** stringify unsigned integer n into s, decimal representation
** s is nul-terminated
** return pointer to s:
*/
extern char *nfmt_uint32(char *s, uint32_t n);
extern char *nfmt_uint64(char *s, uint64_t n);

/*
** stringify unsigned integer n into s for field width w
** pad with ' ' (whitespace) on left if/as necessary to fill width
** s is nul-terminated
** return pointer to s
*/
extern char *nfmt_uint32_pad(char *s, uint32_t n, size_t w);
extern char *nfmt_uint64_pad(char *s, uint64_t n, size_t w); 

/*
** stringify unsigned integer n into s, octal representation
** s is nul-terminated
** return pointer to s:
*/
extern char *nfmt_uint32o(char *s, uint32_t n);

/*
** stringify unsigned integer n into s, hexadecimal representation
** s is nul-terminated
** return pointer to s:
*/
extern char *nfmt_uint32x(char *s, uint32_t n);


/*
** stringify unsigned decimal integer n into s for field width w
** pad with '0' on left if/as necessary to fill w
** s is nul-terminated
** return pointer to s
*/
extern char *nfmt_uint32_pad0(char *s, uint32_t n, size_t w);
extern char *nfmt_uint64_pad0(char *s, uint64_t n, size_t w); 


/*
** stringify unsigned decimal integer n into s for field width w
** octal representation
** pad with '0' on left if/as necessary to fill w
** s is nul-terminated
** return pointer to s
*/
extern char *nfmt_uint32o_pad0(char *s, uint32_t n, size_t w);

/*
** stringify unsigned integer n into s for field width w
** hexadecimal representation
** pad with ' ' (whitespace) on left if/as necessary to fill width
** s is nul-terminated
** return pointer to s
*/
extern char *nfmt_uint32x_pad(char *s, uint32_t n, size_t w);

/*
** stringify unsigned decimal integer n into s for field width w
** hexadecimal representation
** pad with '0' on left if/as necessary to fill w
** s is nul-terminated
** return pointer to s
*/
extern char *nfmt_uint32x_pad0(char *s, uint32_t n, size_t w);


/*
** "djb classic" versions of the above
*/

/*
** stringify decimal integer n into s
** s is *not* nul-terminated
** return number of characters in s
**
** djb classic usage is:
**   char  s[NFMT_UINT32];
**   s[nfmt_uint32_(s,n)]='\0';
**
** if s is NULL, return number of characters to convert n
*/
extern size_t nfmt_uint32_(char *s, uint32_t n);
extern size_t nfmt_uint64_(char *s, uint64_t n);

/* octal form: */
extern size_t nfmt_uint32o_(char *s, uint32_t n);
/* hexadecimal form: */
extern size_t nfmt_uint32x_(char *s, uint32_t n);

/*
** stringify unsigned decimal integer n into s for field width w
** pad with ' ' (whitespace) on left if/as necessary to fill w
** s is *not* nul-terminated
** return number of characters in s
**
** if s is NULL, return number of characters to convert n
*/
extern size_t nfmt_uint32_pad_(char *s, uint32_t n, size_t w);
extern size_t nfmt_uint64_pad_(char *s, uint64_t n, size_t w);

/* hexadecimal: */
extern size_t nfmt_uint32x_pad_(char *s, uint32_t n, size_t w);

/*
** stringify unsigned decimal integer n into s for field width w
** pad with '0' on left if/as necessary to fill w
** s is *not* nul-terminated
** return number of characters in s
**
** if s is NULL, return number of characters to convert n
*/
extern size_t nfmt_uint32_pad0_(char *s, uint32_t n, size_t w);
extern size_t nfmt_uint64_pad0_(char *s, uint64_t n, size_t w);

/* octal: */
extern size_t nfmt_uint32o_pad0_(char *s, uint32_t n, size_t w);
/* hexadecimal: */
extern size_t nfmt_uint32x_pad0_(char *s, uint32_t n, size_t w);

#endif /* NFMT_H */
/* EOF (nfmt.h) */
