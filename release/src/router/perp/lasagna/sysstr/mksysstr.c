/* mksysstr.c
** generate sorted tables for stringified errno and signal numbers
** wcm, 2008.01.21 - 2010.12.01
** ===
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h> 
#include <signal.h>

struct sysno_def {
  int          n;
  const char  *name;
  const char  *mesg;
};

const char progname[] = "mksysstr";

/*
** all SUS/XSI signals plus:
** + SIGIO  (note: SIGPOLL may be an alias on some systems)
** + SIGPWR
** + SIGWINCH
** (ref: APUE/2e, fig 10.1):
*/
static struct sysno_def signo_defs[] = {
#ifdef SIGABRT
  {SIGABRT, "SIGABRT", "abnormal termination"},
#endif
#ifdef SIGALRM
  {SIGALRM, "SIGALRM", "timer expired"},
#endif
#ifdef SIGBUS
  {SIGBUS, "SIGBUS", "hardware fault"},
#endif
#ifdef SIGCHLD
  {SIGCHLD, "SIGCHLD", "child status changed"},
#endif
#ifdef SIGCONT
  {SIGCONT, "SIGCONT", "continue stopped process"},
#endif
#ifdef SIGFPE
  {SIGFPE, "SIGFPE", "arithmetic exception"},
#endif
#ifdef SIGHUP
  {SIGHUP, "SIGHUP", "hangup"},
#endif
#ifdef SIGILL
  {SIGILL, "SIGILL", "illegal instruction"},
#endif
#ifdef SIGINT
  {SIGINT, "SIGINT", "terminal interrupt"},
#endif
#ifdef SIGIO
  {SIGIO, "SIGIO", "asynchronous i/o event"},
#endif
#ifdef SIGKILL
  {SIGKILL, "SIGKILL", "kill"},
#endif
#ifdef SIGPIPE
  {SIGPIPE, "SIGPIPE", "write to broken pipe"},
#endif
#ifdef SIGPOLL
  {SIGPOLL, "SIGPOLL", "pollable i/o event"},
#endif
#ifdef SIGPROF
  {SIGPROF, "SIGPROF", "profiling timer alarm"},
#endif
#ifdef SIGPWR
  {SIGPWR, "SIGPWR", "power failure alert"},
#endif
#ifdef SIGQUIT
  {SIGQUIT, "SIGQUIT", "terminal quit"},
#endif
#ifdef SIGSEGV
  {SIGSEGV, "SIGSEGV", "invalid memory access"},
#endif
#ifdef SIGSTOP
  {SIGSTOP, "SIGSTOP", "stop"},
#endif
#ifdef SIGSYS
  {SIGSYS, "SIGSYS", "invalid system call"},
#endif
#ifdef SIGTERM
  {SIGTERM, "SIGTERM", "terminate"},
#endif
#ifdef SIGTRAP
  {SIGTRAP, "SIGTRAP", "hardware fault"},
#endif
#ifdef SIGTSTP
  {SIGTSTP, "SIGTSTP", "terminal stop"},
#endif
#ifdef SIGTTIN
  {SIGTTIN, "SIGTTIN", "background read from tty"},
#endif
#ifdef SIGTTOU
  {SIGTTOU, "SIGTTOU", "background write to tty"},
#endif
#ifdef SIGURG
  {SIGURG, "SIGURG", "socket urgent condition"},
#endif
#ifdef SIGUSR1
  {SIGUSR1, "SIGUSR1", "user-defined signal 1"},
#endif
#ifdef SIGUSR2
  {SIGUSR2, "SIGUSR2", "user-defined signal 2"},
#endif
#ifdef SIGVTALRM
  {SIGVTALRM, "SIGVTALRM", "virtual timer alarm"},
#endif
#ifdef SIGWINCH
  {SIGWINCH, "SIGWINCH", "terminal window change"},
#endif
#ifdef SIGXCPU
  {SIGXCPU, "SIGXCPU", "cpu limit exceeded"},
#endif
#ifdef SIGXFSZ
  {SIGXFSZ, "SIGXFSZ", "file size limit exceeded"},
#endif
};


/*
** this errno list is not exhaustive ...
*/
static struct sysno_def errno_defs[] = {
  {0, "EOK", "no error"},
#ifdef EPERM
  {EPERM, "EPERM", "permission denied"},
#endif
#ifdef ENOENT
  {ENOENT, "ENOENT", "file does not exist"},
#endif
#ifdef ESRCH
  {ESRCH, "ESRCH", "no such process"},
#endif
#ifdef EINTR
  {EINTR, "EINTR", "interrupted system call"},
#endif
#ifdef EIO
  {EIO, "EIO", "input/output error"},
#endif
#ifdef ENXIO
  {ENXIO, "ENXIO", "device not configured"},
#endif
#ifdef E2BIG
  {E2BIG, "E2BIG", "argument list too long"},
#endif
#ifdef ENOEXEC
  {ENOEXEC, "ENOEXEC", "exec format error"},
#endif
#ifdef EBADF
  {EBADF, "EBADF", "file descriptor not open"},
#endif
#ifdef ECHILD
  {ECHILD, "ECHILD", "no child processes"},
#endif
#ifdef EAGAIN
  {EAGAIN, "EAGAIN", "temporary i/o failure"},
#endif
#ifdef EWOULDBLOCK
/* EWOULDBLOCK may be an alias for EAGAIN: */
#if EWOULDBLOCK != EAGAIN
  {EWOULDBLOCK, "EWOULDBLOCK", "input/output would block"},
#endif
#endif
#ifdef ENOMEM
  {ENOMEM, "ENOMEM", "out of memory"},
#endif
#ifdef EACCES
  {EACCES, "EACCES", "access denied"},
#endif
#ifdef EFAULT
  {EFAULT, "EFAULT", "bad address"},
#endif
#ifdef ENOTBLK
  {ENOTBLK, "ENOTBLK", "not a block device"},
#endif
#ifdef EBUSY
  {EBUSY, "EBUSY", "device busy"},
#endif
#ifdef EEXIST
  {EEXIST, "EEXIST", "file already exists"},
#endif
#ifdef EXDEV
  {EXDEV, "EXDEV", "cross-device link"},
#endif
#ifdef ENODEV
  {ENODEV, "ENODEV", "device does not support operation"},
#endif
#ifdef ENOTDIR
  {ENOTDIR, "ENOTDIR", "not a directory"},
#endif
#ifdef EISDIR
  {EISDIR, "EISDIR", "is a directory"},
#endif
#ifdef EINVAL
  {EINVAL, "EINVAL", "invalid argument"},
#endif
#ifdef ENFILE
  {ENFILE, "ENFILE", "system cannot open more files"},
#endif
#ifdef EMFILE
  {EMFILE, "EMFILE", "process cannot open more files"},
#endif
#ifdef ENOTTY
  {ENOTTY, "ENOTTY", "not a tty"},
#endif
#ifdef ETXTBSY
  {ETXTBSY, "ETXTBSY", "text busy"},
#endif
#ifdef EFBIG
  {EFBIG, "EFBIG", "file too big"},
#endif
#ifdef ENOSPC
  {ENOSPC, "ENOSPC", "out of disk space"},
#endif
#ifdef ESPIPE
  {ESPIPE, "ESPIPE", "unseekable descriptor"},
#endif
#ifdef EROFS
  {EROFS, "EROFS", "read-only file system"},
#endif
#ifdef EMLINK
  {EMLINK, "EMLINK", "too many links"},
#endif
#ifdef EPIPE
  {EPIPE, "EPIPE", "broken pipe"},
#endif
#ifdef EDOM
  {EDOM, "EDOM", "input out of range"},
#endif
#ifdef ERANGE
  {ERANGE, "ERANGE", "output out of range"},
#endif
#ifdef EDEADLK
  {EDEADLK, "EDEADLK", "operation would cause deadlock"},
#endif
#ifdef ENAMETOOLONG
  {ENAMETOOLONG, "ENAMETOOLONG", "file name too long"},
#endif
#ifdef ENOLCK
  {ENOLCK, "ENOLCK", "no locks available"},
#endif
#ifdef ENOSYS
  {ENOSYS, "ENOSYS", "system call not available"},
#endif
#ifdef ENOTEMPTY
  {ENOTEMPTY, "ENOTEMPTY", "directory not empty"},
#endif
#ifdef ELOOP
  {ELOOP, "ELOOP", "symbolic link loop"},
#endif
#ifdef ENOMSG
  {ENOMSG, "ENOMSG", "no message of desired type"},
#endif
#ifdef EIDRM
  {EIDRM, "EIDRM", "identifier removed"},
#endif
#ifdef ECHRNG
  {ECHRNG, "ECHRNG", "channel number out of range"},
#endif
#ifdef ELNRNG
  {ELNRNG, "ELNRNG", "link number out of range"},
#endif
#ifdef EUNATCH
  {EUNATCH, "EUNATCH", "protocol driver not attached"},
#endif
#ifdef EBADE
  {EBADE, "EBADE", "invalid exchange"},
#endif
#ifdef EBADR
  {EBADR, "EBADR", "invalid request descriptor"},
#endif
#ifdef EXFULL
  {EXFULL, "EXFULL", "exchange full"},
#endif
#ifdef ENOANO
  {ENOANO, "ENOANO", "no anode"},
#endif
#ifdef EBADRQC
  {EBADRQC, "EBADRQC", "invalid request code"},
#endif
#ifdef EBADSLT
  {EBADSLT, "EBADSLT", "invalid slot"},
#endif
#ifdef ENOSTR
  {ENOSTR, "ENOSTR", "not a stream device"},
#endif
#ifdef ENODATA
  {ENODATA, "ENODATA", "no data available"},
#endif
#ifdef ETIME
  {ETIME, "ETIME", "timer expired"},
#endif
#ifdef ENOSR
  {ENOSR, "ENOSR", "out of stream resources"},
#endif
#ifdef ENONET
  {ENONET, "ENONET", "machine not on network"},
#endif
#ifdef ENOPKG
  {ENOPKG, "ENOPKG", "package not installed"},
#endif
#ifdef EREMOTE
  {EREMOTE, "EREMOTE", "too many levels of remote in path"},
#endif
#ifdef ENOLINK
  {ENOLINK, "ENOLINK", "link severed"},
#endif
#ifdef EADV
  {EADV, "EADV", "advertise error"},
#endif
#ifdef ESRMNT
  {ESRMNT, "ESRMNT", "srmount error"},
#endif
#ifdef ECOMM
  {ECOMM, "ECOMM", "communication error"},
#endif
#ifdef EPROTO
  {EPROTO, "EPROTO", "protocol error"},
#endif
#ifdef EMULTIHOP
  {EMULTIHOP, "EMULTIHOP", "multihop attempted"},
#endif
#ifdef EBADMSG
  {EBADMSG, "EBADMSG", "bad message type"},
#endif
#ifdef EOVERFLOW
  {EOVERFLOW, "EOVERFLOW", "value exceeds range of data type"},
#endif
#ifdef ENOTUNIQ
  {ENOTUNIQ, "ENOTUNIQ", "name not unique on network"},
#endif
#ifdef EBADFD
  {EBADFD, "EBADFD", "file descriptor in bad state"},
#endif
#ifdef EREMCHG
  {EREMCHG, "EREMCHG", "remote address changed"},
#endif
#ifdef ELIBACC
  {ELIBACC, "ELIBACC", "cannot access shared library"},
#endif
#ifdef ELIBBAD
  {ELIBBAD, "ELIBBAD", "shared library corrupted"},
#endif
#ifdef ELIBMAX
  {ELIBMAX, "ELIBMAX", "linking too many shared libraries"},
#endif
#ifdef ELIBEXEC
  {ELIBEXEC, "ELIBEXEC", "cannot exec a shared library"},
#endif
#ifdef EILSEQ
  {EILSEQ, "EILSEQ", "illegal byte sequence"},
#endif
#ifdef ERESTART
  {ERESTART, "ERESTART", "interrupted system call not restarted"},
#endif
#ifdef ESTRPIPE
  {ESTRPIPE, "ESTRPIPE", "streams pipe error"},
#endif
#ifdef EUSERS
  {EUSERS, "EUSERS", "too many users"},
#endif
#ifdef ENOTSOCK
  {ENOTSOCK, "ENOTSOCK", "not a socket"},
#endif
#ifdef EDESTADDRREQ
  {EDESTADDRREQ, "EDESTADDRREQ", "destination address required"},
#endif
#ifdef EMSGSIZE
  {EMSGSIZE, "EMSGSIZE", "message too long"},
#endif
#ifdef EPROTOTYPE
  {EPROTOTYPE, "EPROTOTYPE", "incorrect protocol type"},
#endif
#ifdef ENOPROTOOPT
  {ENOPROTOOPT, "ENOPROTOOPT", "protocol not available"},
#endif
#ifdef EPROTONOSUPPORT
  {EPROTONOSUPPORT, "EPROTONOSUPPORT", "protocol not supported"},
#endif
#ifdef ESOCKTNOSUPPORT
  {ESOCKTNOSUPPORT, "ESOCKTNOSUPPORT", "socket type not supported"},
#endif
#ifdef EOPNOTSUPP
  {EOPNOTSUPP, "EOPNOTSUPP", "operation not supported"},
#endif
#ifdef EPFNOSUPPORT
  {EPFNOSUPPORT, "EPFNOSUPPORT", "protocol family not supported"},
#endif
#ifdef EAFNOSUPPORT
  {EAFNOSUPPORT, "EAFNOSUPPORT", "address family not supported"},
#endif
#ifdef EADDRINUSE
  {EADDRINUSE, "EADDRINUSE", "address already used"},
#endif
#ifdef EADDRNOTAVAIL
  {EADDRNOTAVAIL, "EADDRNOTAVAIL", "address not available"},
#endif
#ifdef ENETDOWN
  {ENETDOWN, "ENETDOWN", "network down"},
#endif
#ifdef ENETUNREACH
  {ENETUNREACH, "ENETUNREACH", "network unreachable"},
#endif
#ifdef ENETRESET
  {ENETRESET, "ENETRESET", "network reset"},
#endif
#ifdef ECONNABORTED
  {ECONNABORTED, "ECONNABORTED", "connection aborted"},
#endif
#ifdef ECONNRESET
  {ECONNRESET, "ECONNRESET", "connection reset"},
#endif
#ifdef ENOBUFS
  {ENOBUFS, "ENOBUFS", "out of buffer space"},
#endif
#ifdef EISCONN
  {EISCONN, "EISCONN", "already connected"},
#endif
#ifdef ENOTCONN
  {ENOTCONN, "ENOTCONN", "not connected"},
#endif
#ifdef ESHUTDOWN
  {ESHUTDOWN, "ESHUTDOWN", "socket shut down"},
#endif
#ifdef ETOOMANYREFS
  {ETOOMANYREFS, "ETOOMANYREFS", "too many references"},
#endif
#ifdef ETIMEDOUT
  {ETIMEDOUT, "ETIMEDOUT", "connection timed out"},
#endif
#ifdef ECONNREFUSED
  {ECONNREFUSED, "ECONNREFUSED", "connection refused"},
#endif
#ifdef EHOSTDOWN
  {EHOSTDOWN, "EHOSTDOWN", "host down"},
#endif
#ifdef EHOSTUNREACH
  {EHOSTUNREACH, "EHOSTUNREACH", "host unreachable"},
#endif
#ifdef EALREADY
  {EALREADY, "EALREADY", "operation already in progress"},
#endif
#ifdef EINPROGRESS
  {EINPROGRESS, "EINPROGRESS", "operation in progress"},
#endif
#ifdef ESTALE
  {ESTALE, "ESTALE", "stale NFS file handle"},
#endif
#ifdef EUCLEAN
  {EUCLEAN, "EUCLEAN", "structure needs cleaning"},
#endif
#ifdef EREMOTEIO
  {EREMOTEIO, "EREMOTEIO", "remote i/o error"},
#endif
#ifdef EDQUOT
  {EDQUOT, "EDQUOT", "disk quota exceeded"},
#endif
#ifdef ENOMEDIUM
  {ENOMEDIUM, "ENOMEDIUM", "no medium found"},
#endif
#ifdef EMEDIUMTYPE
  {EMEDIUMTYPE, "EMEDIUMTYPE", "wrong medium type"},
#endif
#ifdef ECANCELED
  {ECANCELED, "ECANCELED", "operation cancelled"},
#endif
#ifdef ENOKEY
  {ENOKEY, "ENOKEY", "required key not available"},
#endif
#ifdef EKEYEXPIRED
  {EKEYEXPIRED, "EKEYEXPIRED", "key has expired"},
#endif
#ifdef EKEYREVOKED
  {EKEYREVOKED, "EKEYREVOKED", "key has been revoked"},
#endif
#ifdef EKEYREJECTED
  {EKEYREJECTED, "EKEYREJECTED", "key rejected by service"},
#endif
#ifdef EPROCLIM
  {EPROCLIM, "EPROCLIM", "too many processes"},
#endif
#ifdef EBADRPC
  {EBADRPC, "EBADRPC", "RPC structure is bad"},
#endif
#ifdef ERPCMISMATCH
  {ERPCMISMATCH, "ERPCMISMATCH", "RPC version mismatch"},
#endif
#ifdef EPROGUNAVAIL
  {EPROGUNAVAIL, "EPROGUNAVAIL", "RPC program unavailable"},
#endif
#ifdef EPROGMISMATCH
  {EPROGMISMATCH, "EPROGMISMATCH", "program version mismatch"},
#endif
#ifdef EPROCUNAVAIL
  {EPROCUNAVAIL, "EPROCUNAVAIL", "bad procedure for program"},
#endif
#ifdef EFTYPE
  {EFTYPE, "EFTYPE", "bad file type"},
#endif
#ifdef EAUTH
  {EAUTH, "EAUTH", "authentication error"},
#endif
#ifdef ENEEDAUTH
  {ENEEDAUTH, "ENEEDAUTH", "not authenticated"},
#endif
#ifdef ERREMOTE
  {ERREMOTE, "ERREMOTE", "object not local"},
#endif
};



static
int sysno_compare(const void *p1, const void *p2)
{
  struct sysno_def  *sd1 = (struct sysno_def *)p1;
  int                s1  = sd1->n;
  struct sysno_def  *sd2 = (struct sysno_def *)p2;
  int                s2  = sd2->n;

  if(s1 < s2)  return -1;
  if(s1 == s2) return 0;

  /* else: */
  return 1;
}


static
size_t signo_init(void)
{
  size_t  items = (sizeof signo_defs / sizeof(struct sysno_def));

  /* qsort errno_defs[]: */
  qsort(signo_defs, items, sizeof(struct sysno_def), &sysno_compare); 

  return items;
}


static
size_t errno_init(void)
{
  size_t  items = (sizeof errno_defs / sizeof(struct sysno_def));

  /* qsort errno_defs[]: */
  qsort(errno_defs, items, sizeof(struct sysno_def), &sysno_compare); 

  return items;
}


static
void
do_signo(void)
{
  size_t  signo_items = 0;
  size_t  i;

  /* sort signo table: */
  signo_items = signo_init();
  fprintf(stderr, "%s: sorted %zu items in signo table\n", progname, signo_items);

  printf(
      "/* sysstr_signo.c.in\n"
      "** automatically generated by %s (%s) \n"
      "*/\n\n", progname, __FILE__ );

  printf(
      "/*\n"
      "** %zu sorted sysno_def entries in signo_defs[] table:\n"
      "*/\n", signo_items);

  printf(
    "static const struct sysno_def signo_defs[] = {\n");

  for(i = 0; i < signo_items; ++i){
      printf("  {%d, \"%s\", \"%s\"},\n", signo_defs[i].n,
                                          signo_defs[i].name,
                                          signo_defs[i].mesg);
  }

  printf("};\n\n");

  printf(
    "/* eof (sysstr_signo.c.in) */\n");

  return;
}


static
void
do_errno(void)
{
  size_t  errno_items = 0;
  size_t  i;

  /* sort errno table: */
  errno_items = errno_init(); 
  fprintf(stderr, "%s: sorted %zu items in errno table\n", progname, errno_items);

  printf(
      "/* sysstr_errno.c.in\n"
      "** automatically generated by %s (%s) \n"
      "*/\n\n", progname, __FILE__ );

  printf(
      "/*\n"
      "** %zu sorted sysno_def entries in errno_defs[] table:\n"
      "*/\n", errno_items);

  printf(
    "static const struct sysno_def errno_defs[] = {\n");

  for(i = 0; i < errno_items; ++i){
      printf("  {%d, \"%s\", \"%s\"},\n", errno_defs[i].n,
                                          errno_defs[i].name,
                                          errno_defs[i].mesg);
  }

  printf("};\n\n");

  printf(
    "/* eof (sysstr_errno.c.in) */\n");

  return;

}


int
main(int argc, char *argv[])
{
  if(argc < 2){
      fprintf(stderr, "%s: error: requires argument \"errno\" or \"signo\"\n",
              progname);
      exit(1);
  }

  if(strcmp(argv[1], "errno") == 0){
      do_errno();
  } else if(strcmp(argv[1], "signo") == 0){
      do_signo();
  } else {
      fprintf(stderr, "%s: error: requires argument \"errno\" or \"signo\"\n",
              progname);
      exit(1);
  }

  return 0;
}


/* eof (mksysstr.c) */
