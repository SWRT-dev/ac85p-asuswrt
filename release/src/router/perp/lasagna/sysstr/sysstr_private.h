/* sysstr_private.h
** correlate errno and signal numbers with stringified names
** non-public, internal header
** wcm, 2008.01.21 - 2008.10.23
** ===
*/
#ifndef SYSSTR_PRIVATE_H
#define SYSSTR_PRIVATE_H 1


/* structure of data tables produced by mksysstr.c: */
struct sysno_def {
  int          n;
  const char  *name;
  const char  *mesg;
};


#endif /* SYSSTR_PRIVATE_H */
/* EOF (sysstr_private.h) */
