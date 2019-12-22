/* perpboot.c
** perp: persistent process supervision
** perp 2.0: single process scanner/supervisor/controller
** perpboot: perpd startup utility
**   - exec and supervise perpd + logger
**   - a lightweight process supervisor for perpd itself
** (see also rundeux(8) in runtools)
** wcm, 2009.10.04 - 2011.03.22
** ===
*/

/* TODO
**   * built-in envfile processing
*/

#include <stdlib.h>
/* unix: */
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/wait.h>

/* lasagna: */
#include "cstr.h"
#include "fd.h"
#include "newenv.h"
#include "nextopt.h"
#include "sig.h"
#include "sigset.h"
#include "tain.h"

#include "perp_common.h"
#include "perp_stderr.h"

/* other macros: */
#define warn(...) \
    eputs(progname, ": warning: ", __VA_ARGS__)

#define do_kill(p,k) \
    if((p) > 0) kill((p),(k))

/* environ: */
extern char  **environ;

static const char *progname = NULL;
static const char prog_usage[] = "[-hV] [-d] [-x] [dir]";

struct child {
  char     *argv[2];
  pid_t     pid;
  tain_t    when;
};

/* variables in scope: */
/* deux[0] = logger, deux[1] = program: */
static struct child  deux[2];
static int           my_sigpipe[2];
static int           my_logpipe[2];
static int           flag_term = 0;
static sigset_t      my_sigset;
static char         *perp_base = NULL;
/* running as supervisor (by default): */
static int           opt_super = 1;

static void sigtrap(int sig);
static void sigpipe_ping(void);
static void setup(void);
static void deux_init(void);
static void child_exec(int which);
static void child_wait(void);
static void main_loop(void);


static
void
sigtrap(int sig)
{
  int  want_cont = 0;

  switch(sig){
  case SIGCHLD:
      break;
  case SIGINT:
      /* want clean term with SIGINT! */
      /* fall through: */
  case SIGTERM:
      flag_term = 1;
      want_cont = 1;
      /* fallthrough: */
  default:
      /* by default, pass signals to perpd: */
      do_kill(deux[1].pid, sig);
      if(want_cont){
          do_kill(deux[1].pid, SIGCONT);
      }
      break;
  }

  sigpipe_ping();
  return;
}


static
void
sigpipe_ping(void)
{
  int  terrno = errno;
  int  w;

  do{
      w = write(my_sigpipe[1], "!", 1);
  }while((w == -1) && (errno == EINTR));

  errno = terrno;
  return;
}


static
void
setup(void)
{
  int  i = 0;

  if(pipe(my_sigpipe) == -1){
      fatal_syserr("failure setting up selfpipe");
  }

  for(i = 0; i < 2; ++i){
      fd_cloexec(my_sigpipe[i]);
      fd_nonblock(my_sigpipe[i]);
  }

  if(pipe(my_logpipe) == -1){
      fatal_syserr("failure creating logpipe");
  }

  sigset_fill(&my_sigset);
  sigset_block(&my_sigset);

  sig_catch(SIGTERM, &sigtrap);
  sig_catch(SIGINT, &sigtrap);
  sig_catch(SIGCHLD, &sigtrap);

  /* catching these signals to pass to perpd: */
  sig_catch(SIGALRM, &sigtrap);
  sig_catch(SIGCONT, &sigtrap);
  sig_catch(SIGHUP, &sigtrap);
  sig_catch(SIGQUIT, &sigtrap);
  sig_catch(SIGTSTP, &sigtrap);
  sig_catch(SIGUSR1, &sigtrap);
  sig_catch(SIGUSR2, &sigtrap);

  sig_ignore(SIGPIPE);
  return;
}    


static
void
deux_init(void)
{
  /* deux[0]: logger: */
  deux[0].pid = 0;
  tain_load(&deux[0].when, 0, 0);
  deux[0].argv[0] = "./rc.log";
  deux[0].argv[1] = NULL;

  /* deux[1]: program: */
  deux[1].pid = 0;
  tain_load(&deux[1].when, 0, 0);
  deux[1].argv[0] = "./rc.perp";
  deux[1].argv[1] = NULL;

  return;
}


static
void
child_exec(int which)
{
  pid_t      pid = 0;
  tain_t     now;
  tain_t     when_ok;
  char     **argv = deux[which].argv;

  tain_now(&now);
  tain_load(&when_ok, 1, 0);
  tain_plus(&when_ok, &deux[which].when, &when_ok);
  if(tain_less(&now, &when_ok)){
      warn("pausing for restart of ", argv[0], " ...");
      sleep(1);
  }

  /*  if option -x:
  **    not supervising, don't fork here, just exec into deux[1]
  */
  if(opt_super ? 1 : (which == 0)){
      while((pid = fork()) == -1){
          warn("failure on fork() while starting ", argv[0]);
          sleep(2);
      }
  }
      
  /* child (or, if not opt_super, execing into deux[1]): */
  if(pid == 0){
      int            fd, fd_max;
      struct rlimit  rlim;
      int            i;

      /* setup PERP_BASE in environment: */
      if(newenv_set("PERP_BASE", perp_base) == -1){
          fatal_syserr("failure setting environment in child for ", argv[0]);
      }
      /* prepare for closing unused file descriptors: */
      if(getrlimit(RLIMIT_NOFILE, &rlim) == -1){
          fatal_syserr("failure getting file rlimit in child for ", argv[0]);
      }
      fd_max = (rlim.rlim_max == RLIM_INFINITY) ? 1024 : rlim.rlim_max;
      /* start fd 0,1,2 on /dev/null: */
      if((fd = open("/dev/null", O_RDWR)) == -1){
          fatal_syserr("failure opening /dev/null in child for ", argv[0]);
      }
      fd_dupe(0, fd); fd_dupe(1, fd); fd_dupe(2, fd);
      close(fd);
      /* setup logpipe: */
      fd_dupe(which, my_logpipe[which]);
      if(which == 1){
          /* perpd gets stderr redirected to stdout for logger: */
          fd_dupe(2, 1);
      }
      /* close all other descriptors: */
      for(i = 3; i < fd_max; ++i) close(i);
      /* set default umask: */
      umask(0);
      /* reset default signal handlers, unblock: */
      sig_default(SIGTERM);
      sig_default(SIGCHLD);
      sig_default(SIGALRM);
      sig_default(SIGCONT);
      sig_default(SIGHUP);
      sig_default(SIGINT);
      sig_default(SIGQUIT);
      sig_default(SIGTSTP);
      sig_default(SIGUSR1);
      sig_default(SIGUSR2);
      sig_default(SIGPIPE);
      sigset_unblock(&my_sigset);
      /* do it: */
      newenv_run(argv, environ);
      /* uh oh: */
      fatal_syserr("failure on exec of ", argv[0]);
  }

  /* parent: */
  deux[which].pid = pid;
  tain_now(&deux[which].when);

  return;
}


static
void
child_wait(void)
{
  pid_t  pid;

  while((pid = waitpid(-1, NULL, WNOHANG)) > 0){
      if(pid == deux[0].pid){
          deux[0].pid = 0;
      }else if(pid == deux[1].pid){
          deux[1].pid = 0;
      }
  }

  return;
}


static
void
main_loop(void)
{
  struct pollfd  pollv[1];
  int            i;
  int            e = 0;
  char          *c;

  pollv[0].fd = my_sigpipe[0];
  pollv[0].events = POLLIN;

  for(;;){
      if(flag_term){
          break;
      }

      /* start/restart children: */
      /* deux[0] is logger, check it first: */
      for(i = 0; i < 2; ++i){
          if(deux[i].pid == 0){
              child_exec(i);
          }
      }

      /* poll on sigpipe: */
      sigset_unblock(&my_sigset);
      do{
          e = poll(pollv, 1, -1);
      }while((e == -1) && (errno == EINTR));
      sigset_block(&my_sigset);

      /* consume sigpipe: */
      while(read(my_sigpipe[0], &c, 1) == 1){/*empty*/ ;}

      /* consume dead children: */
      child_wait();
  }

  /* here on SIGTERM: */
  if(deux[1].pid){
      /* harvest deux[1]: */
      waitpid(deux[1].pid, NULL, 0);
  } 
  if(deux[0].pid){
      /* logger should exit on eof: */
      close(my_logpipe[1]);
      do_kill(deux[0].pid, SIGCONT);
      waitpid(deux[0].pid, NULL, 0);
  }

  return;
}


int
main(int argc, char *argv[])
{
  nextopt_t  nopt =  nextopt_INIT(argc, argv, ":hVdx");
  char       opt;
  int        opt_detach = 0;

  progname = nextopt_progname(&nopt);
  while((opt = nextopt(&nopt))){
      char optc[2] = {nopt.opt_got, '\0'};
      switch(opt){
      case 'V': version(); die(0); break;
      case 'h': usage(); die(0); break;
      case 'd': opt_detach = 1; break;
      case 'x': opt_super = 0; break;
      case ':':
          fatal_usage("missing argument for option -", optc);
          break;
      case '?':
          if(nopt.opt_got != '?'){
              fatal_usage("invalid option: -", optc);
          }
          /* else fallthrough: */
      default :
          die_usage(); break; 
      }
  }

  argc -= nopt.arg_ndx;
  argv += nopt.arg_ndx;

  perp_base = argv[0];

  /* perp_base: default is "/etc/perp": */
  if((perp_base == NULL) || (perp_base[0] == '\0')){
      perp_base = getenv("PERP_BASE");
  }
  if((perp_base == NULL) || (perp_base[0] == '\0')){
      perp_base = PERP_BASE_DEFAULT;
  }

  /* initialize sigpipe[], etc: */
  setup();

  if(opt_detach){
      pid_t  pid = fork();
      switch(pid){
      case -1: fatal_syserr("failure fork(): unable to detach"); break;
      case 0:
          /* child continues daemonized in new session: */
          setsid(); break;
      default:
          /* parent exits: */
          exit(0);
      }
  }

  if(chdir(perp_base) == -1){
      fatal_syserr("failure chdir() to ", perp_base);
  }
  if(chdir(PERP_BOOT) == -1){
      fatal_syserr("failure chdir() to ", perp_base, "/", PERP_BOOT);
  }

  deux_init();
  main_loop();

  return 0;
}


/* eof: perpboot.c */
