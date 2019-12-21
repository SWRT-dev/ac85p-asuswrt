/* sysstr.h
** correlate errno and signal numbers with stringified names
** wcm, 2008.01.21 - 2011.02.01
** ===
*/
#ifndef SYSSTR_H
#define SYSSTR_H 1

/* faux errno 0: */
#ifndef EOK
#define EOK 0
#endif


/*
** return pointer to errno name corresponding to errno,
** or NULL if not found:
*/
extern const char *sysstr_errno(int errno_num);

/*
** return pointer to errno message corresponding to errno,
** or NULL if not found:
*/
extern const char *sysstr_errno_mesg(int errno_num);

/*
** return pointer to signal name corresponding to signal,
** or NULL if not found:
*/
extern const char *sysstr_signal(int signum);

/*
** return pointer to signal message corresponding to signal,
** or NULL if not found:
*/
extern const char *sysstr_signal_mesg(int signum);



#endif /* SYSSTR_H */
/* EOF (sysstr.h) */
