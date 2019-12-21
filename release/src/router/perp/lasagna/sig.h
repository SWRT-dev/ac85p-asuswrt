/* sig.h
** signal operations
** wcm, 2008.01.04 - 2009.10.06
** ===
*/
#ifndef SIG_H
#define SIG_H 1

#include <signal.h>

/* sigprocmask() to block/unblock signals: */
extern void sig_block(int sig);
extern void sig_unblock(int sig);

/* sigaction() to set signal handlers: */
typedef void (*sig_handler_t)(int);

/* sig_catch()
**   install handler for signal sig
**   explicitly does *not* set the SA_RESTART flag in sigaction
**
**   return:
**     success: pointer to signal handler previously installed
**     error:   SIG_ERR
**
**   note:
**     use this function when you *do* specifically want/expect
**     slow/blocking system calls like read() to be interrupted
**     errno = EINTR by the signal, rather than automatically/quietly
**     restarted
**
**   see APUE/2e, sec 10.5, p. 303 for discussion
**
**   compatible with "djb" sig_catch()
*/
extern sig_handler_t sig_catch(int sig, sig_handler_t handler);

/* sig_uncatch()
** sig_default()
**   revert to the default handler for signal sig
**
** note: sig_uncatch() provided for "djb" compatibility
*/
#define sig_uncatch(s) sig_catch((s), (SIG_DFL))
#define sig_default(s) sig_catch((s), (SIG_DFL))

/* sig_ignore()
**   catch and ignore the signal sig
*/
#define sig_ignore(s)  sig_catch((s), (SIG_IGN))


/* sig_catchr()
**   install handler for signal sig
**   sets SA_RESTART flag in sigaction
**
**   note:
**     explicitly sets the SA_RESTART flag in sigaction, so that
**     system calls like read() are automatically/quietly restarted
**     and not interrupted with an error
*/
extern sig_handler_t sig_catchr(int sig, sig_handler_t handler);

#endif /* SIG_H */
/* EOF (sig.h) */
