/* cstr.h
** cstr: nul-terminated character buffer
** wcm, 2004.04.19 - 2012.06.02
** ===
*/
#ifndef CSTR_H
#define CSTR_H

#include <stddef.h> /* size_t */

/*
** operations on C strings (nul-terminated characters buffers)
** concept: typedef char * cstr;
*/


/* cstr_cat()
**   synopsis:
**     append nul-terminated "from" onto end of nul-terminated "to"
**
**   return:
**     number of chars preceeding nul in "to" after append
**
**   notes:
**     * caller required to provide "to" of sufficient size
**     * return value differs from strcat(to, from)
*/
extern
size_t
cstr_cat(char *to, const char *from);

/* cstr_chop()
**  synopsis:
**    remove single trailing '\n' if present from end of string
**
**  return:
**    s
*/
extern
char *
cstr_chop(char *s);

/* cstr_cmp()
**   synopsis:
**     compare two strings from beginning
**
**   return:
**      0:  s1 matches s2
**     <0:  s1 "less than" s2
**     >0:  s1 "greater than" s2
**
**   notes:
**      djb analogue:  str_diff(s1, s2)
*/
extern
int
cstr_cmp(const char *s1, const char *s2);


/* cstr_cmpi()
**   synopsis:
**     compare two strings from beginning, case insensitive
**
**   return:
**      0:  s1 matches s2
**     <0:  s1 "less than" s2
**     >0:  s1 "greater than" s2
*/
extern
int
cstr_cmpi(const char *s1, const char *s2);


/* cstr_contains()
**   synopsis:
**     test s1 begins with s2
**
**   return:
**     1: true, s1 begins with s2
**     0: false, test fails
**      
**   notes:
**     djb analogue:  str_start(s1, s2)
*/
extern
int
cstr_contains(const char *s1, const char *s2);


/* cstr_copy()
**   synopsis:
**     copy nul-terminated "from" into "to"
**
**   return:
**     number of chars preceeding nul in "to" after copy
**
**   notes:
**     * caller required to provide "to" of sufficient size
**     * return value differs from strcpy(to, from)
**     * djb analogue:  str_copy(to, from)
*/
extern
size_t
cstr_copy(char *to, const char *from);

/* cstr_dup()
**   synopsis:
**     allocate memory and duplicate string s
**
**   return
**     success: pointer to newly allocated copy
**     failure: NULL (allocation failure)
*/
extern
char *
cstr_dup(const char *s);

/* cstr_eq()
**   synopsis:
**     compare two strings for equivalence
**
**   return
**     boolean true, two strings equivalent
**     boolean false, two strings not equivalent
*/
#define cstr_eq(s1, s2)  (cstr_cmp((s1),(s2))==0)

/* cstr_eqi()
**   synopsis:
**     compare two strings for equivalence, case insensitive
**
**   return
**     boolean true, two strings equivalent
**     boolean false, two strings not equivalent
*/
#define cstr_eqi(s1, s2)  (cstr_cmpi((s1),(s2))==0)

/* cstr_lcat()
**   synopsis:
**      see strlcat(3)
*/
extern
size_t
cstr_lcat(char *to, const char *from, size_t to_size);


/* cstr_lcpy()
**   synopsis:
**     see strlcpy(3)
*/
extern
size_t
cstr_lcpy(char *to, const char *from, size_t to_size);


/* cstr_len()
**   synopsis:
**     string length
**
**   return:
**     number of chars preceeding nul
*/ 
extern
size_t
cstr_len(const char *s);

/* cstr_ltrim()
**   trim leading whitespace
**   return s
*/
extern
char *
cstr_ltrim(char *s);

/* cstr_match()
**   synopsis:
**     match string s against pattern
**     pattern may include:
**       - any literal character, matched case-sensitive
**       - '?' matches any single character
**       - '*' matches any zero or more characters
**     pattern is nul-terminated
**
**   return:
**     1: match
**     0: no match
**
**   notes:
**     fast, non-recursive implementation
**     no supplemental storage allocated (eg, as compared to regex)
**     less "powerful" than fnmatch(), regex()
**         eg: no [..] set/range match, no escaping wildcards, etc.
**     pattern is implicitly anchored from beginning to end of string
*/
extern
int
cstr_match(const char *pattern, const char *s);

/* cstr_matchi()
**   synopsis:
**     case-insensitive version of cstr_match()
**
**   return:
**     1: match
**     0: no match
*/
extern
int
cstr_matchi(const char *pattern, const char *s);

/* cstr_ncmp()
**   synopsis:
**     like cstr_cmp(), but comparing only first len characters
**
**   return:
**      0:  s1 matches s2
**     <0:  s1 "less than" s2
**     >0:  s1 "greater than" s2
**
*/
extern
int
cstr_ncmp(const char *s1, const char *s2, size_t len);


/* cstr_pos()
**   synopsis:
**     find first occurence of character in string
**
**   return:
**     found: index position in s of first occurence of c
**     not found: index position of nul in s
**
**   usage example:
**       if(s[cstr_pos(s, c)]) { //found }...
**       if(s[cstr_pos(s, c)] == '\0') { // not found }...
**
**   notes:
**     * return value differs from strchr(3)
**     * djb analogue:  str_chr()
*/
extern
size_t
cstr_pos(const char *s, int c);


/* cstr_rpos()
**   synopsis:
**     find last occurence of character in string
**
**   return:
**     found: index position in s of last occurence of c
**     not found: index position of nul in s
**
**   usage example:
**       if(s[cstr_rpos(s, c)]) { //found }...
**       if(s[cstr_rpos(s, c)] == '\0') { // not found }...
**
**   notes:
**     * return value differs from strrchr(3)
**     * djb analogue:  str_rchr()
*/
extern
size_t
cstr_rpos(const char *s, int c);


/* cstr_rtrim()
**   trim trailing whitespace
**   return s
*/
extern
char *
cstr_rtrim(char *s);

/* cstr_trim()
**   trim leading and trailing whitespace
**   return s
*/
extern
char *
cstr_trim(char *s);

/* cstr_vcat_()
**   synopsis:
**     append variable number of argument strings onto end of "to"
**
**   return:
**     number of chars preceeding nul in "to" after append
**
**   notes:
**     * caller _must_ supply NULL as final argument
**     * instead: use the macro version
**     * caller required to provide "to" of sufficient size
*/
extern
size_t
cstr_vcat_(char *to, ...);

/* cstr_vcat()
**   macro convenience function for cstr_vcat_():
*/
#define  cstr_vcat(to, ...)  cstr_vcat_(to, __VA_ARGS__, NULL)


/* cstr_vcopy_()
**   synopsis:
**     copy variable number of strings into "to"
**
**   return:
**     number of chars preceeding nul in "to" after copy
**
**   notes:
**     * caller _must_ supply NULL as final argument
**     * instead: use the macro version
**     * caller required to provide "to" of sufficient size
**
**   example:
**
**     cstr_vcopy_(mybuf, "hello", " ", "world!", NULL);
*/
extern
size_t
cstr_vcopy_(char *to, ...);

/* cstr_vcopy()
**   macro convenience function for cstr_vcopy_():
*/
#define  cstr_vcopy(to, ...)  cstr_vcopy_(to, __VA_ARGS__, NULL)


/* cstr_vlen_()
**   synopsis:
**     compute combined length of one or more argument strings
**
**   return:
**     number of chars of all strings (not including nul)
**
**   notes:
**     * caller _must_ supply NULL as final argument
**     * instead: use the macro version
*/
extern
size_t
cstr_vlen_(const char *s1, ...);

/* cstr_vlen()
**   macro convenience function for cstr_vlen_():
*/
#define  cstr_vlen(...)  cstr_vlen_(__VA_ARGS__, NULL)


#endif /* CSTR_H */
/* eof: cstr.h */
