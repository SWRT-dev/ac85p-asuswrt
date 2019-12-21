/* rlimit_private.h
** rlimit table element
** non-public, internal header
** wcm, 2009.09.18 - 2009.09.18
** ===
*/
#ifndef RLIMIT_PRIVATE_H
#define RLIMIT_PRIVATE_H 1


/* structure of data tables produced by mk_rlimitc: */
struct rlimit_def {
  int          r;
  const char  *name;
  const char  *mesg;
};


#endif /* RLIMIT_PRIVATE_H */
/* EOF (rlimit_private.h) */
