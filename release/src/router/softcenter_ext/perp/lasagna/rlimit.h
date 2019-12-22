/* rlimit.h
** rlimit utility module
** wcm, 2009.09.18 - 2009.09.18
** ===
*/
#ifndef RLIMIT_H
#define RLIMIT_H 1

/* rlimit_lookup()
** check for platform support of stringified RLIMIT_ name
**
** return
**   -1  : rlimit resource not supported on this platform
**   >=0 : rlimit resource handle
*/
extern int rlimit_lookup(const char *name);

/* rlimit_name()
** stringified rlimit name for rlimit resource handle r
**
** return:
**   NULL     : rlimit resource handle not found
**   non-null : stringified name of RLIMIT_ definition
*/
extern const char *rlimit_name(int r);

/* rlimit_mesg()
** description of resource
**
** return:
**   NULL     : rlimit resource handle not defined
**   non-null : wordy description of resource
*/
extern const char *rlimit_mesg(int r);


#endif /* RLIMIT_H */
/* EOF (rlimit.h) */
