/* tinylog.c
** log from data read on stdin
** wcm, 2006.08.04 - 2011.02.01
** ===
*/

/* TODO (maybe ...):
**   USR1/USR2 toggle fsync() on each ouput line
**   provide short-circuit logic for keep if keep_max == 0
**   provide unlimited keep files
**   provide unlimited growth of current if size == 0
*/  

/* standard libs: */
#include <stdlib.h>
/* rename() from stdio.h: */
extern int rename(const char *oldpath, const char *newpath);

/* unix libs: */
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

/* lasagna: */
#include "uchar.h"
#include "buf.h"
#include "cstr.h"
#include "fd.h"
#include "ioq.h"
#include "nextopt.h"
#include "nfmt.h"
#include "nuscan.h"
#include "pidlock.h"
#include "sig.h"
#include "sigset.h"
#include "sysstr.h"
#include "tain.h"

#include "tinylog.h"

/* environ: */
extern char **environ;

/* logging variables in scope: */
static const char *progname = NULL;
static const char prog_usage[] = "[-hV] [-k numkeep] [-r] [-s logsize] [-t] [-z] dir";
static const char *my_pidstr = NULL;

/*
** tinylog object:
*/
struct tinylog {
    const char  *fn_logdir;
    int          fd_orig;
    int          fd_logdir;
    int          fd_pidlock;
    int          fd_current;
    size_t       current_size;
    size_t       current_max;
    size_t       keep_max;
    int          wantstamp; 
    int          wantzip; 
};

/* ioq for stdin: */
#define INBUF_SIZE  1024
uchar_t  inbuf[INBUF_SIZE];
ssize_t read_op(int fd, void *buf, size_t len);
ioq_t  in = ioq_INIT(0, inbuf, sizeof inbuf , &read_op);

/*
** variables in scope:
*/
static pid_t  mypid = 0;
static int    flagexit = 0;
static int    flagrotate = 0;

/* sigset for blocking/unblocking signal handler: */
static sigset_t my_sigset;

/* global timestamp string: */
char  stamp8601[] = "yyyymmddThhmmss.uuuuuu" ;
/* global filename string for log archive: */
char  fn8601[] = "_yyyymmddThhmmss.uuuuuu.?" ZIP_EXT ;

/* where to find gzip: */
const char *gzip_path = NULL;

/* default maximum size for log file (bytes): */
#define CURRENT_MAX  100000
/* maximum size for line in log file (bytes): */
#define LOGLINE_MAX    1000
/* pause on exceptional error (billionths of second): */
#define EPAUSE  555444321UL

/* RETRY():
**  if test evaluates true: issue warning, pause, and repeat
**  test is a system call that sets errno
**  exits failure on sigterm
**  example usage:
**      RETRY(chdir(".") == -1),
**          "fail chdir() on current directory");
*/
#define RETRY(test, ...) \
  {\
      while((test)){ \
          tain_t  epause = tain_INIT(0,EPAUSE); \
          if(flagexit){ \
              fatal_syserr("exiting on SIGTERM during retry error: ", __VA_ARGS__); \
          } \
          warn_syserr("pausing: ", __VA_ARGS__); \
          tain_pause(&epause, NULL); \
      } \
  }


/*
** declarations in scope:
*/
static void  write_all(int fd, void *buf, size_t len);
static void stamp8601_make(char *stamp_buf);
static void init_logdir(struct tinylog *tinylog);
static void init_current(struct tinylog *tinylog, int resume);
static void tinylog_rotate(struct tinylog *tinylog);
static void tinylog_keep(struct tinylog *tinylog, const char *filename, const char *ext);
static int  tinylog_prune(struct tinylog *tinylog);
static int  tinylog_gzip(struct tinylog *tinylog, const char *file);
static void tinylog_post(struct tinylog *tinylog, char *logline, size_t len);
static int  do_log(struct tinylog *tinylog);


/*
** definitions:
*/

static
void
sig_handler(int sig)
{
  switch(sig){
  case SIGTERM: ++flagexit; break;
  case SIGHUP:  ++flagrotate; break;
  default: break;
  }

  return;
}


/* read_op()
**   read() operation installed for ioq on stdin
*/
ssize_t read_op(int fd, void *buf, size_t len)
{
  ssize_t  r = 0;

  /* note:
  **
  **   SIGTERM handler setup *without* SA_RESTART sigaction flag
  **   so SIGTERM will interrupt the read() here
  */
  do{
      /* if SIGTERM, simulate eof before read(): */
      if(flagexit) return 0;
      r = read(fd, buf, len);
  }while((r == -1) && (errno == EINTR));

  return r;
}


/* write_all()
**   write() len bytes from buf to fd
**   retry until len bytes complete
*/
static
void
write_all(int fd, void *buf, size_t len)
{
  ssize_t w = 0;
  tain_t  epause = tain_INIT(0, EPAUSE);

  while(len){
      do{
          w = write(fd, buf, len);
          if((w == -1) && (errno != EINTR)){
              int terrno = errno;
              warn_syserr("pausing on write() error");
              tain_pause(&epause, NULL);
              errno = terrno;
          }
      }while(w == -1);

      buf += w;
      len -= w;
  }

  return;
} 


/* stamp8601_make()
** generate an rfc8601-type ascii timestamp into buffer stamp_buf
**
** notes:
**   caller must supply stamp_buf of sufficient size: (sizeof stamp8601)
**   stamp_buf is not nul-terminated
*/
static
void
stamp8601_make(char *stamp_buf)
{
    char            *s = stamp_buf;
    tain_t           now;
    time_t           now_utc;
    struct tm       *tm_now = NULL;

    tain_now(&now);
    now_utc = tain_to_utc(&now);
    tm_now = gmtime(&now_utc);

    nfmt_uint32_pad0_(s, 1900 + tm_now->tm_year, 4); s += 4;
    /* use mday format (day of month): */
    nfmt_uint32_pad0_(s, 1 + tm_now->tm_mon, 2); s += 2;
    nfmt_uint32_pad0_(s, tm_now->tm_mday,    2); s += 2;
    *s = 'T'; ++s;
    nfmt_uint32_pad0_(s, tm_now->tm_hour, 2); s += 2;
    nfmt_uint32_pad0_(s, tm_now->tm_min,  2); s += 2;
    nfmt_uint32_pad0_(s, tm_now->tm_sec,  2); s += 2;
    *s = '.'; ++s;
    nfmt_uint32_pad0_(s, (uint32_t)(now.nsec/1000), 6);

    return;
}


static
void
init_logdir(struct tinylog *tinylog)
{
    int    fd;

    /* open log directory: */
    mkdir(tinylog->fn_logdir, 0700);
    if(chdir(tinylog->fn_logdir) == -1){
        fatal_syserr("failure chdir() to log directory ", tinylog->fn_logdir);
    }

    if((fd = open(".", O_RDONLY)) == -1){
        fatal_syserr("failure open() on log directory ", tinylog->fn_logdir);
    }
    fd_cloexec(fd);
    tinylog->fd_logdir = fd;

    if((fd = pidlock_set(TINYLOG_PIDLOCK, mypid, PIDLOCK_NOW)) == -1){
        fatal_syserr("failure acquiring lock file ", TINYLOG_PIDLOCK,
                " in log directory ", tinylog->fn_logdir);
    }
    fd_cloexec(fd);
    tinylog->fd_pidlock = fd;

    return;
}


/* init_current()
**   setup or resume "current" logfile
**   if resume == 0: open new current
**   on entry and exit, cwd is logging directory fd_logdir
*/
static
void
init_current(struct tinylog *tinylog, int resume)
{
    struct stat  sb;
    int          e;
    int          fd;
    int          new = 0;
    
    /* stat current: */
    e = stat("current", &sb);
    if(e == -1){
        if(errno != ENOENT){
            /* current exists but stat() failed: */
            fatal_syserr("failure stat() on current in log directory");
        } else {
            /* current does not exist, will be new: */
            ++new;
        }
    } else {
        if(!(sb.st_mode & 0100)){
            /* current exists, found in mode 0644: last exit not clean: */
            tinylog_keep(tinylog, "current", "u");
            /* current will now be new: */
            ++new;
        } else if(resume == 0){
            /* current exists with clean exit status, but new current requested: */
            tinylog_keep(tinylog, "current", "s");
            /* current will now be new: */
            ++new;
        }
        /* otherwise: current exists with clean exit status and will be resumed */
    }

    /* whether current exists or will be new, open() it: */
    fd = open("current", O_WRONLY | O_CREAT | O_APPEND | O_NONBLOCK, 0600);
    if(fd == -1){
        fatal_syserr("failure open() on current in log directory");
    }

    /* mode for active logging: */
    if(fchmod(fd, 0644) == -1){
        fatal_syserr("failure fchmod() on current in log directory");
    }
    fd_cloexec(fd);

    /* all set: */
    tinylog->fd_current = fd;
    tinylog->current_size = new ? 0 : sb.st_size;

    return;
}


/* tinylog_rotate()
**   flush current to disk;
**   rename as previous; archive previous;
**   open new current
*/
static
void
tinylog_rotate(struct tinylog *tinylog)
{
    int  fd = tinylog->fd_current;

    if(!(tinylog->current_size > 0)){
        flagrotate = 0;
        return;
    }

    fsync(fd);
    close(fd);

    RETRY((rename("current", "previous") == -1),
        "failure rename() on current");

    tinylog_keep(tinylog, "previous", "s");

    RETRY(((fd = open("current", O_WRONLY | O_CREAT | O_APPEND | O_NONBLOCK, 0600)) == -1),
        "failure open() for new current");

    /* mode for active logging: */
    RETRY((fchmod(fd, 0644) == -1),
        "failure fchmod() for new current");
    fd_cloexec(fd);

    /* all set: */
    tinylog->fd_current = fd;
    tinylog->current_size = 0;
    flagrotate = 0;

    return;
}


/* tinylog_keep()
**   rotate log to timestamped archive with extension
**   on entry and exit, cwd is logdir
*/
static
void
tinylog_keep(struct tinylog *tinylog, const char *log, const char *ext)
{
  struct stat  sb;
  tain_t       ewait;
  int          e;

  for(;;){
      e = stat(log, &sb);
      if(e == 0)
          break;
      if((e == -1) && (errno == ENOENT))
          return;
      /* else: stat() error: */
      warn_syserr("pausing: failure stat() for log ", log);
      tain_LOAD(&ewait, 1, 0);
      tain_pause(&ewait, NULL);
      continue;
  }

  if(sb.st_nlink == 1){
      fn8601[0]  = '_';
      fn8601[23] = '.';
      fn8601[24] = ext[0];
      fn8601[25] = '\0';

      for(;;){
          stamp8601_make(&fn8601[1]);
          e = link(log, fn8601);
          if(e == 0)
              break;
          if((e == -1) && (errno == EEXIST)){
              /* collision on microsecond timestamp: */
              tain_LOAD(&ewait, 0, 2000);
              tain_pause(&ewait, NULL);
              continue;
          }
          /* else link() error: */
          warn_syserr("pausing: failure link() for ", log, " to ", fn8601);
          tain_LOAD(&ewait, 1, 0);
          tain_pause(&ewait, NULL);
          continue;
      }
  }

  RETRY((unlink(log) == -1),
        "failure unlink() on file ", log);

  if(tinylog->wantzip){
      e = tinylog_gzip(tinylog, fn8601);
      if(e != 0){
          log_warning("failure gzip on log archive ", fn8601);
      }
  }

  while(tinylog_prune(tinylog) == -1){
      log_warning("pausing: failure while pruning log directory");
      tain_LOAD(&ewait, 1, 0);
      tain_pause(&ewait, NULL);
      continue;
  }

  /* success: */
  return;
}


/* tinylog_prune()
**   scan logdir and prune oldest log archive
**   return
**     1 : an archive was pruned, yet more remain to be pruned
**     0 : logdir pruned (either nothing to prune or prune complete)
**    -1 : system error from opendir(), readdir(), or unlink(), errno set
**
**   on entry and exit, cwd is logdir
*/
static
int
tinylog_prune(struct tinylog *tinylog)
{
  DIR            *dir;
  struct dirent  *d;
  char            target[80] = "_z";
  size_t          n;
  int             terrno;
  int             keep = tinylog->keep_max;
  int             count = 0;

  if((dir = opendir(".")) == NULL){
      warn_syserr("failure opendir(\".\") in log directory");
      return -1;
  }

  /* repeat prune operation while count > keep: */
  do{
      /* scan: find "oldest" archive file (from its filename) */
      for(;;){
          errno = 0;
          if((d = readdir(dir)) == NULL){
              /* done or failure: */
              break;
          }
          if((d->d_name[0] == '_')
              && (n = cstr_len(d->d_name) >= 23)
              && (d->d_name[9] == 'T')
              && (n < sizeof target)){
              /* smells like a log archive: */
              ++count;
              /* target: oldest filename found in scan: */
              if(cstr_cmp(d->d_name, target) < 0){
                  cstr_copy(target, d->d_name);
              }
          }
      } 
      terrno = errno;
      closedir(dir);

      if(terrno){
          /* oops, readdir() error in loop: */
          errno = terrno;
          warn_syserr("failure readdir() while scanning log directory");
          return -1;
      }

      if(count <= keep){
          /* nothing to prune: */
          return 0;
      }

      /* remove the oldest log archive in scan: */
      if(unlink(target) == -1){
          warn_syserr("failure unlink() to prune target log ", target);
          return -1;
      }

      /* target archive pruned: */
      --count;

  }while(count > keep);

  return 0;
}


/* tinylog_gzip()
**   execve() a gzip child process: file -> "zipped"
**   on successful completion of child:
**     rename() "zipped" -> file + ZIP_EXT
**     unlink() original file
**
**   return
**     0:  success
**    -1:  something failed
*/
static
int
tinylog_gzip(struct tinylog *tinylog, const char *file)
{
    char    gzipfile[80] = "";
    pid_t   pid;
    int     wstat;

    (void)tinylog; /* unused parameter */
    RETRY(((pid = fork()) == -1),
          "fail fork() for gzip process");

    if(pid == 0){ /* child */
        int          fd;
        const char  *args[2];

        sig_uncatch(SIGTERM);
        sig_uncatch(SIGHUP);
        sigset_unblock(&my_sigset);

        if((fd = open(file, O_RDONLY | O_NONBLOCK)) == -1){
            return -1;
        }
        fd_move(0, fd);

        if((fd = open("zipped", O_WRONLY | O_CREAT | O_TRUNC | O_NONBLOCK , 0600)) == -1){
            return -1;
        }
        fd_move(1, fd);

        args[0] = gzip_path;
        args[1] = NULL;
        execve(gzip_path, (char * const *)args, environ);
        fatal_syserr("fail execve() for gzip process");
    }

    /* parent */
    RETRY((waitpid(pid, &wstat, 0) == -1),
          "fail waitpid() for gzip process");

    if(WIFEXITED(wstat) && (WEXITSTATUS(wstat) != 0)){
        char nbuf[NFMT_SIZE];
        nfmt_uint32(nbuf, WEXITSTATUS(wstat));
        log_warning("gzip process exited non-zero error code: ", nbuf);
        unlink("zipped");
        return -1;
    }
    if(WIFSIGNALED(wstat)){
        log_warning("gzip process terminated on signal: ",
                     sysstr_signal(WTERMSIG(wstat)));
        unlink("zipped");
        return -1;
    }

    cstr_vcat(gzipfile, file, ZIP_EXT);
    if(rename("zipped", gzipfile) == -1){
        warn_syserr("fail rename() after gzip");
        unlink("zipped");
        return -1;
    }
   
    if(chmod(gzipfile, 0644) == -1){
        warn_syserr("fail chmod() after gzip");
        unlink(gzipfile);
        return -1;
    }
 
    /* archive zipped successfully, delete original file: */
    RETRY((unlink(file) == -1),
        "fail unlink() after gzip on ", file);

    return 0;
}


static
void
tinylog_post(struct tinylog *tinylog, char *logline, size_t len)
{
    /* rotate? */
    if(flagrotate || (tinylog->current_size >= (tinylog->current_max - len))){
        tinylog_rotate(tinylog);
    }

    /* post logline: */
    write_all(tinylog->fd_current, logline, len);

#if 0
    /* XXX, make this an option? */
    fsync(tinylog->fd_current);
#endif

    tinylog->current_size += len;

    return;
}


static
int
do_log(struct tinylog *tinylog)
{
    char    logline[LOGLINE_MAX + 1];
    size_t  len = 0;
    char    c;
    int     r;
    int     wantstamp = tinylog->wantstamp;
    size_t  startpos = (wantstamp ? 24 : 0);
    int     eof = 0;

    /* terminal condition: eof */
    /* outer loop: one line per loop: */
    while(!eof){
        /* initialize line: */
        len = startpos;
        /* inner loop: one char per loop: */
        while((r = ioq_GETC(&in, &c)) == 1){
            if(c == '\n'){
                /* newline appended after break (if line non-empty): */
                break;
            }
            if(len < LOGLINE_MAX){
                /* filter unprintable control chars: */
                if((c < 32) && (c != '\t')) c = '?';
                logline[len++] = c;
            }
            /* implicit else: draining input buffer from logline overflow */
        }
        if(r == -1) /* io error on stdin: */ return -1;
        if(r == 0) ++eof;
        /* post any non-empty line collected from inner loop: */
        if(len > startpos){
            /* prepend timestamp: */
            if(wantstamp){
                stamp8601_make(logline);
                logline[22] = ':';
                logline[23] = ' ';
            }
            /* append newline: */
            logline[len++] = '\n';
            /* post it: */
            tinylog_post(tinylog, logline, len);
        }
    }

    /* here on eof */
    /* XXX, ignoring errors now: */
    fchmod(tinylog->fd_current, 0744);
    close(tinylog->fd_current);

    return 0;
}


int
main(int argc, char *argv[])
{
    nextopt_t         nopt = nextopt_INIT(argc, argv, ":hVk:rs:tz");
    char              opt;
    static char       pidbuf[NFMT_SIZE];
    struct tinylog    tinylog;
    int               opt_resume = 1;
    uint32_t          n = 0;
    const char       *z;
    int               err = 0;

    /* initialize defaults: */
    tinylog.wantstamp = 0;
    tinylog.wantzip = 0;
    tinylog.current_max = CURRENT_MAX;
    tinylog.keep_max = 5;

    mypid = getpid();
    my_pidstr = nfmt_uint32(pidbuf, (uint32_t)mypid);
    progname = nextopt_progname(&nopt);    
    while((opt = nextopt(&nopt))){
        char optc[2] = {nopt.opt_got, '\0'};
        switch(opt){
        case 'h': usage(); die(0); break;
        case 'V': version(); die(0); break;
        case 'k':
            z = nuscan_uint32(&n, nopt.opt_arg);
            if(*z != '\0'){
                fatal_usage("numeric argument required for option -", optc);
            }
            tinylog.keep_max = (size_t) n;
            break;
        case 'r': opt_resume = 0; break;
        case 's':
            z = nuscan_uint32(&n, nopt.opt_arg);
            if(*z != '\0'){
                fatal_usage("numeric argument required for option -", optc);
            }
            if(n < (LOGLINE_MAX * 2))
                n = (LOGLINE_MAX * 2);
            tinylog.current_max = (size_t) n;
            break;
        case 't': tinylog.wantstamp = 1; break;
        case 'z': tinylog.wantzip = 1; break;
        case ':':
            fatal_usage("missing argument for option -", optc);
            break;
        case '?':
            if(nopt.opt_got != '?'){
                fatal_usage("invalid option -", optc);
            }
            /* else fallthrough: */
        default:
            die_usage(); break;
        }
    }

    argc -= nopt.arg_ndx;
    argv += nopt.arg_ndx;

    if(argc < 1){
        fatal_usage("missing log directory argument");
    }

    /* log output directory: */
    tinylog.fn_logdir = argv[0];
    log_info("starting for logging in ", tinylog.fn_logdir, " ...");

    /* where to find gzip: */
    if(tinylog.wantzip){
        gzip_path = getenv("TINYLOG_ZIP");
        if((gzip_path == NULL) || (gzip_path[0] == '\0')){
            /* tinylog.h: */
            gzip_path = TINYLOG_ZIP;
        }
    }

    if(chdir(".") == -1){
        fatal_syserr("fail chdir() on startup directory");
    }
    if((tinylog.fd_orig = open(".", O_RDONLY)) == -1){
        fatal_syserr("fail open() on startup directory");
    }
    fd_cloexec(tinylog.fd_orig);

    /* initialize signal set: */
    sigset_empty(&my_sigset);
    sigset_add(&my_sigset, SIGTERM);
    sigset_add(&my_sigset, SIGHUP);
    /* block signals: */
    sigset_block(&my_sigset);
    /* install signal handlers
    ** note:
    **   sig_catch() does _not_ set SA_RESTART sigaction flag
    **   so calls like read() will be interrupted errno = EINTR on signal
    **   which is what we want, esp SIGTERM during blocking read() on stdin
    */
    sig_catch(SIGTERM, &sig_handler);
    sig_catch(SIGHUP,  &sig_handler);

    /* open and cd to logdir: */
    init_logdir(&tinylog);

    /* open/reopen current: */
    init_current(&tinylog, opt_resume);

    /* start logging: */
    sigset_unblock(&my_sigset);
    err = do_log(&tinylog);

    return (err ? 111 : 0);
}

/* eof: tinylog.c */
