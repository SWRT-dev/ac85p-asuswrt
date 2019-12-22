/* perpd_svdef.c
** perp: persistent process supervision
** perpd 2.0: single process scanner/supervisor/controller
** perpd_svdef: perpd subroutines on service definitions
** wcm, 2010.12.28 - 2013.01.07
** ===
*/

#include <stddef.h>
#include <stdint.h>

/* unix: */
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

/* lasanga: */
#include "buf.h"
#include "cstr.h"
#include "fd.h"
#include "newenv.h"
#include "nfmt.h"
#include "sig.h"
#include "sigset.h"
#include "sysstr.h"
#include "tain.h"
#include "uchar.h"

/* perp: */
#include "perp_common.h"
#include "perpd.h"


/* perpd_svdef_clear()
**   prepare a clean perpd_svdef object
*/
void
perpd_svdef_clear(struct svdef *svdef)
{
  buf_zero(svdef, sizeof (struct svdef));
  return;
}


/* perpd_svdef_close()
**   close descriptors in a perpd_svdef object
*/
void
perpd_svdef_close(struct svdef *svdef)
{
  close(svdef->fd_dir);
  return;
}


/* perpd_svdef_activate()
**   activate service definition
**   called by perpd_scan()
**
**   return:
**     0: success
**    -1: failure
**
**   notes:
**     failures include:
**       - service definition directory name too long
**         (must me less than, say, 240 characters)
**       - open() on service definition directory
**       - pipe() for logpipe
**     service is activated only on success
*/
int
perpd_svdef_activate(struct svdef *svdef, const char *svdir, const struct stat *st_dir)
{
  struct stat  st;
  char         path_buf[256];
  int          fd;

  perpd_svdef_clear(svdef);

  if(cstr_len(svdir) > 240){
      errno = ENAMETOOLONG;
      warn_syserr("service definition directory name error: ", svdir);
      return -1;
  }

  svdef->dev = st_dir->st_dev;
  svdef->ino = st_dir->st_ino;
  cstr_lcpy(svdef->name, svdir, sizeof svdef->name);
  tain_now(&svdef->when);
  svdef->bitflags |= SVDEF_FLAG_ACTIVE;

  /* open an fd to use for fchdir() in perpd_svrun(): */
  cstr_vcopy(path_buf, "./", svdir);
  if((fd = open(path_buf, O_RDONLY)) == -1){
      warn_syserr("failure open() on service definition directory ", svdir);
      return -1;
  }
  fd_cloexec(fd);
  svdef->fd_dir = fd;

  /* inspect service definition directory: */
  cstr_vcopy(path_buf, "./", svdir, "/flag.down");
  if(stat(path_buf, &st) != -1){
      svdef->bitflags |= SVDEF_FLAG_DOWN;
  }
  cstr_vcopy(path_buf, "./", svdir, "/flag.once");
  if(stat(path_buf, &st) != -1){
      svdef->bitflags |= SVDEF_FLAG_ONCE;
  }

  /* logging? */
  cstr_vcopy(path_buf, "./", svdir, "/rc.log");
  if(stat(path_buf, &st) != -1){
      if(st.st_mode & S_IXUSR){
          svdef->bitflags |= SVDEF_FLAG_HASLOG;
          log_debug("rc.log exists and is executable for ", svdir);
      }else{
          log_warning("rc.log exists but is not set executable for ", svdir);
      }
  }

  /* setup logpipe: */
  if(svdef->bitflags & SVDEF_FLAG_HASLOG){
      if(pipe(svdef->logpipe) == -1){
          warn_syserr("failure pipe() on logpipe for ", svdir);
          close(fd);
          return -1;
      }
      fd_cloexec(svdef->logpipe[0]);
      fd_cloexec(svdef->logpipe[1]);
  } 

  /*
  ** from here on, the service is considered activated
  */

  /* first time startup: */
  /* log: if FLAG_HASLOG, start irrespective of any other svdef->bitflags: */
  if(svdef->bitflags & SVDEF_FLAG_HASLOG){
      svdef->svpair[SUBSV_LOG].bitflags |= SUBSV_FLAG_ISLOG;
      perpd_svdef_run(svdef, SUBSV_LOG, SVRUN_START);
  }

  /* XXX, bail here if log startup fails on fork() ?  */

  /* main: */
  if(!(svdef->bitflags & SVDEF_FLAG_DOWN)){
      /* setup for running once? */
      if(svdef->bitflags & SVDEF_FLAG_ONCE){
          svdef->svpair[SUBSV_MAIN].bitflags |= SUBSV_FLAG_ISONCE;
      }
      perpd_svdef_run(svdef, SUBSV_MAIN, SVRUN_START);
  } else {
      svdef->svpair[SUBSV_MAIN].bitflags |= SUBSV_FLAG_WANTDOWN;
  }
 
  return 0;
}


/* perpd_svdef_keep()
**   set this svdef as active
**   called by perpd_scan()
*/
void
perpd_svdef_keep(struct svdef *svdef, const char *svdir)
{
  /* flag active: */
  svdef->bitflags |= SVDEF_FLAG_ACTIVE;

  /* possible name update: */
  cstr_lcpy(svdef->name, svdir, sizeof svdef->name);

  return;
}


/* perd_svdef_checkfail()
**   in perpd_mainloop():
**     if global flag_failing is set, called for each active svdef
*/
void
perpd_svdef_checkfail(struct svdef *svdef)
{
  struct subsv  *subsv;
  int            target, r = 0;

  /* insanity? */
  if(!(svdef->bitflags & SVDEF_FLAG_ACTIVE))
      return;

  /* logic:
  **   perpd_svdef_run() sets last attempted target for fork(), eg FLAG_ISRESET
  **   if failing, retry that target
  */

  /* check log first: */
  subsv = (svdef->bitflags & SVDEF_FLAG_HASLOG) ? &svdef->svpair[SUBSV_LOG] : NULL;
  if((subsv != NULL) && (subsv->bitflags & SUBSV_FLAG_FAILING)){
      target = (subsv->bitflags & SUBSV_FLAG_ISRESET) ? SVRUN_RESET : SVRUN_START;
      r = perpd_svdef_run(svdef, SUBSV_LOG, target);
  }

  /* XXX, bail here if log is failing? */

  /* main: */
  subsv = &svdef->svpair[SUBSV_MAIN];  
  if(subsv->bitflags & SUBSV_FLAG_FAILING){
      target = (subsv->bitflags & SUBSV_FLAG_ISRESET) ? SVRUN_RESET : SVRUN_START;
      perpd_svdef_run(svdef, SUBSV_MAIN, target);
  }

  return;
}


/* perpd_svdef_wantcull()
**   initiate deactivation of service definition
**     - called by perpd_scan() on deactivation of single service
**     - called by perpd_mainloop() on shutdown of all services
**
**   return:
**     0: cull initiated on svdef (but not complete)
**     1: svdef already in cull state
*/
int
perpd_svdef_wantcull(struct svdef *svdef)
{
  struct subsv  *subsv;

  svdef->bitflags |= SVDEF_FLAG_CULL;

  /* initiate termination: */
  svdef->svpair[SUBSV_MAIN].bitflags |= SUBSV_FLAG_WANTDOWN;
  if(svdef->bitflags & SVDEF_FLAG_HASLOG){
      svdef->svpair[SUBSV_LOG].bitflags |= SUBSV_FLAG_WANTDOWN;
  }

  /* subsv main: */
  subsv = &svdef->svpair[SUBSV_MAIN];

  /* sigterm main (if not running reset) and make sure not paused: */
  if(subsv->pid > 0){
      if(!(subsv->bitflags & SUBSV_FLAG_ISRESET)){
          kill(subsv->pid, SIGTERM);
      }
      subsv->bitflags &= ~SUBSV_FLAG_ISPAUSED;
      kill(subsv->pid, SIGCONT);
      return 0;
  }

  /* subsv log?: */
  subsv = (svdef->bitflags & SVDEF_FLAG_HASLOG) ? &svdef->svpair[SUBSV_LOG] : NULL;

  /* shutdown log (if not already running reset) and make sure not paused: */
  if((subsv != NULL) && (subsv->pid > 0)){
      if(!(subsv->bitflags & SUBSV_FLAG_ISRESET)){
          close(svdef->logpipe[1]);
          close(svdef->logpipe[0]);
          kill(subsv->pid, SIGTERM);
      }
      subsv->bitflags &= ~SUBSV_FLAG_ISPAUSED;
      kill(subsv->pid, SIGCONT);
      return 0;
  }

  /* if here, nothing was running to kill()
  **   - for example: no log, and main service was already down
  **   - but up to now the service was still considered active
  **
  ** in any case, no kill() means:
  **   - no triggering of sigchld
  **   - and perpd_waitup() will not harvest the cull
  **
  ** indicate already in cull ready state:
  */

  return 1;
}


/* perpd_svdef_cullok()
**   inspect svdef if cull complete and ok for deactivation
**     - called by perpd_cull()
**     - called by perd_waitup()
**     - called by perd_scan()
**
**   return:
**     0: not flagged for cull or cull incomplete
**     1: cull complete and ok for deactivation
*/
int
perpd_svdef_cullok(struct svdef *svdef)
{
  struct subsv  *subsv;

  /* XXX, if fork() failing:
  **   possibility of (pid == -1) state on attempt of running reset
  **   in which case, svdef will be culled *without* running reset
  **   do we care?
  */

  if(!(svdef->bitflags & SVDEF_FLAG_CULL)){
      return 0;
  }

  /* main: */
  subsv = &svdef->svpair[SUBSV_MAIN];
  if(subsv->pid > 0){
      return 0;
  }

  /* log: */
  subsv = (svdef->bitflags & SVDEF_FLAG_HASLOG) ? &svdef->svpair[SUBSV_LOG] : NULL;
  if((subsv != NULL) && (subsv->pid > 0)){
      return 0;
  }

  return 1;
}


/* perpd_svdef_run()
**   exec() a service:
**     "which" is SUBSV_MAIN or SUBSV_LOG
**     "target" is SVRUN_START or SVRUN_RESET
**
**   side effects:
**     global flag_failing set on fail of fork()
**     child uses (but does not alter) global poll_sigset
*/
int
perpd_svdef_run(struct svdef *svdef, int which, int target)
{
  struct subsv  *subsv = &svdef->svpair[which];
  char          *prog[7];
  tain_t         now, when_ok;
  tain_t         towait = tain_INIT(0,0);
  pid_t          pid;
  int            wstat;
  char           nbuf_reset[NFMT_SIZE];
  int            i;

  /* insanity checks: */
  if((which == SUBSV_LOG) && !(svdef->bitflags & SVDEF_FLAG_HASLOG)){
      log_debug("logging service requested but not enabled");
      return 0;
  }

  if(subsv->pid > 0){
      log_debug("perpd_svrun() requested for service still running");
      return 0;
  }

  /* initialize (attempted) target, etc: */
  switch(target){
  case SVRUN_RESET: subsv->bitflags |= SUBSV_FLAG_ISRESET; break;
  default: subsv->bitflags &= ~SUBSV_FLAG_ISRESET; break;
  }
  wstat = subsv->wstat;
  subsv->pid = 0;
  subsv->wstat = 0;
  subsv->bitflags &= ~SUBSV_FLAG_FAILING;

  /* setup argv: */
  prog[0] = (which == SUBSV_LOG) ? "./rc.log" : "./rc.main";
  prog[1] = (target == SVRUN_START) ? "start" : "reset";
  prog[2] = svdef->name;
  prog[3] = NULL;

  /* additional args if running "reset": */
  if(target == SVRUN_RESET){
      if(WIFEXITED(wstat)){
          prog[3] = "exit";
          prog[4] = nfmt_uint32(nbuf_reset, (uint32_t)WEXITSTATUS(wstat));
          prog[5] = NULL;
      } else {
          int    n = (WIFSIGNALED(wstat) ? WTERMSIG(wstat) : WSTOPSIG(wstat));
          char  *s = (char *)sysstr_signal(n);
          prog[3] = (WIFSIGNALED(wstat) ? "signal" : "stopped");
          prog[4] = nfmt_uint32(nbuf_reset, (uint32_t)n);
          prog[5] = ((s != NULL) ? s : "SIGUNKNOWN");
          prog[6] = NULL;
      }
  }

  /* timestamps and respawn governor: */
  tain_now(&now);
  tain_assign(&when_ok, &subsv->when_ok);
  if((target == SVRUN_START) && tain_less(&now, &when_ok)){
          log_warning("setting respawn governor on 'start' target of service ", svdef->name,
                      " for ", prog[0]);
          tain_minus(&towait, &when_ok, &now);
  }

  /* fork/exec: */
  if((pid = fork()) == -1){
      subsv->pid = 0;
      subsv->bitflags |= SUBSV_FLAG_FAILING;
      perpd_trigger_fail();
      warn_syserr("failure fork() for service ", svdef->name);
      return -1;
  }

  /* XXX, TODO:
  **   if child error before exec(), die() with a distinctive error code
  */

  /* child: */
  if(pid == 0){
      /* nfmt buffer for environmental variables (reusable with newenv_set()): */
      char  nbuf_env[NFMT_SIZE];

      /* run child in new process group: */
      setsid();
      /* cwd for runscripts is svdir: */
      if(fchdir(svdef->fd_dir) == -1){
          fatal_syserr("(in child for service ", svdef->name,
                       "): failure fchdir() to service directory");
      }
      /* setup logpipe: */
      if(svdef->bitflags & SVDEF_FLAG_HASLOG){
          if(which == SUBSV_MAIN){
              /* set stdout to logpipe: */
              close(1);
              if(dup2(svdef->logpipe[1], 1) != 1){
                  fatal_syserr("(in child for service ", svdef->name,
                               "): failure dup2() on logpipe[1] to logging service");
              }
          }
          if((which == SUBSV_LOG) && (target == SVRUN_START)){
              /* set stdin to logpipe:
              **   (but not if this is a resetting log service)
              */
              close(0);
              if(dup2(svdef->logpipe[0], 0) != 0){
                  fatal_syserr("(in child for service ", svdef->name,
                               "): failure dup2() on logpipe[0] for logging service");
              }
          }
          close(svdef->logpipe[0]);
          close(svdef->logpipe[1]);
      }
      /* close extraneous descriptors (shouldn't be any!): */
      for(i = 3; i < 1024; ++i) close(i);
      /* set PERP_BASE in the environment: */
      if(newenv_set("PERP_BASE", basedir) == -1){
          fatal_syserr("(in child for service ", svdef->name,
                       "): failure setting PERP_BASE environment for ",
                       prog[0], " ", prog[1]);
      }
      /* set PERP_SVPID in the environment: */
      if(target == SVRUN_RESET){
          nfmt_uint64(nbuf_env, (uint64_t)subsv->pid_prev);
      }else{
          nfmt_uint64(nbuf_env, (uint64_t)getpid());
      }
      if(newenv_set("PERP_SVPID", nbuf_env) == -1){
          fatal_syserr("(in child for service ", svdef->name,
                       "): failure setting PERP_SVPID environment for ",
                       prog[0], " ", prog[1]);
      }
      /* set PERP_SVSECS in the environment (reset target only): */
      if(target == SVRUN_RESET){
          nfmt_uint64(nbuf_env, tain_uptime(&now, &subsv->when));
          if(newenv_set("PERP_SVSECS", nbuf_env) == -1){
              fatal_syserr("(in child for service ", svdef->name,
                           "): failure setting PERP_SVSECS environment for ",
                           prog[0], " ", prog[1]);
          }
      }
      /* respawn governor: */
      if((target == SVRUN_START) && !(tain_iszero(&towait))){
          tain_pause(&towait, NULL);
      }
      /* clear signal handlers from child process: */
      sig_uncatch(SIGCHLD);
      sig_uncatch(SIGHUP);
      sig_uncatch(SIGINT);
      sig_uncatch(SIGTERM);
      sig_uncatch(SIGPIPE);
      sigset_unblock(&poll_sigset);
      /* go forth my child: */
      newenv_run(prog, environ);
      /* nuts, exec failed: */
      fatal_syserr("(in child for service ", svdef->name,
                   "):  failure execve()");
  }

  /* parent: */
  subsv->pid = pid;
  /* set timestamps and respawn governor: */
  tain_assign(&subsv->when, &now);
  if(target == SVRUN_START){
      /* when_ok = now + 1sec + wait: */ 
      tain_LOAD(&when_ok, 1, 0);
      tain_plus(&when_ok, &now, &when_ok);
      tain_plus(&when_ok, &when_ok, &towait);
      tain_assign(&subsv->when_ok, &when_ok);
  }

  return 0;
}


/* eof: perpd_svdef.c */
