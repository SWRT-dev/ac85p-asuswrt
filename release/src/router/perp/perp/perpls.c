/* perpls.c
** perp: persistent process supervision
** perp 2.0: single process scanner/supervisor/controller
** perpls: query and list perp services
** (ipc client query to perpd server)
** wcm, 2008.01.23 - 2013.01.07
** ===
*/

/* libc: */
#include <stdlib.h>

/* unix: */
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/stat.h>

/* lasanga: */
#include "buf.h"
#include "cstr.h"
#include "domsock.h"
#include "dynstuf.h"
#include "fd.h"
#include "nextopt.h"
#include "nfmt.h"
#include "nuscan.h"
#include "pkt.h"
#include "sysstr.h"
#include "tain.h"
#include "upak.h"
#include "uchar.h"

#include "perp_common.h"
#include "perp_stderr.h"

/* redefine eputs() for ioq: */
#ifdef eputs
#undef eputs
#endif

/* perpls using ioq-based i/o: */
#include "ioq.h"
#include "ioq_std.h"

/* stderr: */
#define eputs(...) \
  {ioq_flush(ioq1); ioq_vputs(ioq2, __VA_ARGS__, "\n"); ioq_flush(ioq2); }

#define barf(...) \
    { eputs(progname, " barf: ", __VA_ARGS__); die(120); }

/* stdout: */
#define vputs(...) \
  ioq_vputs(ioq1, __VA_ARGS__)

#define vputs_flush() \
  ioq_flush(ioq1)


/* logging variables in scope: */
static const char  *progname = NULL;
static const char   prog_usage[] = "[-hV] [-b basedir] [-cGgrt] [sv ...]";

/* status display panel: */
#define  PERP_PANEL  "[- --- ---]"

/* "colorized" perpls capbilities: */
enum cap_id {
  CAP_DF = 0,
  CAP_NA,
  CAP_AN, CAP_AR, CAP_AP, CAP_AD,
  CAP_WU, CAP_WD,
  CAP_ER,
  CAP_EX,
  CAP_NU
};

/* storage for a capability attribute string: */
#define CAP_ATTR_MAX  15

/* colorized perpls capability record: */
struct lscap {
    enum cap_id   cap_id;
    const char   *cap_key;
    char          attr[CAP_ATTR_MAX + 1];
};

/* colorized perpls capability table: */
static struct lscap captab[] = {
  /* note:
  ** C99 designated initializers could/should be used here, eg:
  **
  **     [CAP_DF] = { CAP_DF, "df", "0\0" },
  **     [CAP_NA] = { CAP_NA, "na", "0\0" },
  **     ...
  */

  /* INITIALIZE IN cap_id ORDER: */

  /* default capability:     */ { CAP_DF, "df", "0\0" },
  /* not active:             */ { CAP_NA, "na", "0\0" },
  /* active/normal:          */ { CAP_AN, "an", "0\0" },
  /* active/resetting:       */ { CAP_AR, "ar", "0\0" },
  /* active/paused:          */ { CAP_AP, "ap", "0\0" },
  /* active/down:            */ { CAP_AD, "ad", "0\0" },
  /* want up:                */ { CAP_WU, "wu", "0\0" },
  /* want down:              */ { CAP_WD, "wd", "0\0" },
  /* error:                  */ { CAP_ER, "er", "0\0" },
  /* exception:              */ { CAP_EX, "ex", "0\0" },
  /* NULL SENTINAL:          */ { CAP_NU, NULL, "\0"  }

  /* note:
  ** the CAP_DF/"df" provides a user-definable default capability
  ** that is, what to use when a capability is not otherwise defined
  */
};

/* LSCAPS: number of items in captab[], _not_ including null sentinal: */
#define LSCAPS ((sizeof captab / sizeof(struct lscap)) - 1)

/* PADMAX: maximum field size for svstat->name: */
#define PADMAX  80

/* svstat object: */
struct svstat {
    int          is_active;
    enum cap_id  cap_id;
    char        *name;
    char        *panel;
/* 66 bytes in status query reply: */
#define BINSTAT_SIZE  66
    uchar_t      binstat[BINSTAT_SIZE];
    pid_t        pid_main;
    uint64_t     uptime_main;
    int          has_log;
    pid_t        pid_log;
    uint64_t     uptime_log;
    int          sys_errno;
    const char  *errmsg;
}; 



/* prototypes in scope: */
static struct svstat * svstat_init(struct svstat *svstat, const char *dir);
static void svstat_query(int fd_conn, struct svstat *svstat);
static void svstat_unpack(struct svstat *svstat, tain_t *now);
static void svstat_setcap(struct svstat *svstat);
static void svstat_free(struct svstat *svstat);

static void captab_init(const char *db);
static const char * captab_lookup(const char *db, const char *key, size_t *attr_len);

static const char *name_pad(const char *name, size_t width);

/* qsort comparison functions: */
typedef int(*cmp_func_t)(const void *, const void *);
static int cmp_byname(const void *a, const void *b);
static int cmp_byuptime(const void *a, const void *b);


/*
**  definitions
*/

/* svstat_init()
** svstat constructor/initialization:
** initialize existing svstat, or allocate a new one if NULL
**   strdup(dir) into svstat->name if dir non-NULL
** return:
**   pointer to initialized (possibly new) svstat
**   NULL on allocation failure
*/
static
struct svstat *
svstat_init(struct svstat *svstat, const char *svdir)
{
  struct svstat *S = svstat;
  int            new = 0;
  char          *panel = NULL;
  char          *name = NULL;    

  if(S == NULL){
      S = (struct svstat *)malloc(sizeof(struct svstat));
      if(S == NULL)
          return NULL;
      /* else: */
      new = 1;
  }

  panel = cstr_dup(PERP_PANEL);
  if(panel == NULL){
      goto fail;
  }

  if(svdir != NULL){
      name = cstr_dup(svdir);
      if(name == NULL)
          goto fail;
  }

  S->is_active   = 0;
  S->cap_id      = CAP_NA;
  S->name        = name;
  S->panel       = panel;
  S->pid_main    = -1;
  S->uptime_main = 0;
  S->has_log     = 0;
  S->pid_log     = -1;
  S->uptime_log  = 0;
  S->sys_errno   = EOK;
  return S;

fail:
  if(panel) free(panel);
  if(name) free(name);
  if(new) free(S);
  return NULL;
}


/* svstat_query()
**   query fd_conn socket for status of svstat object sv
*/
static
void
svstat_query(int fd_conn, struct svstat *S)
{
  const char  *svdir = S->name;
  struct stat  st;
  pkt_t        pkt = pkt_INIT(2, 'Q', 16);


  S->sys_errno = 0;

  if(stat(svdir, &st) == -1){
      S->panel[1] = 'E';
      S->sys_errno = errno;
      S->errmsg = "failure stat() on service directory";
      return;
  }

  if(! S_ISDIR(st.st_mode)){
      S->panel[1] = 'E';
      S->sys_errno = ENOTDIR;
      S->errmsg = "not a directory";
      return;
  }

  if(! (st.st_mode & S_ISVTX)){
      /* sticky bit not set; service not activated: */
      S->panel[1] = '-';
      return;
  }

  /* service considered "active" if sticky bit set: */
  S->panel[1] = '+';

  upak_pack(pkt_data(pkt), "LL", (uint64_t)st.st_dev, (uint64_t)st.st_ino);

  if(pkt_write(fd_conn, pkt, 0) == -1){
      S->panel[1] = 'E';
      S->sys_errno = errno;
      S->errmsg = "failure pkt_write() during status query";
      return;
  }

  if(pkt_read(fd_conn, pkt, 0) == -1){
      S->panel[1] = 'E';
      S->sys_errno = errno;
      S->errmsg = "failure pkt_read() during status query";
      return;
  }

  if(pkt[0] != 2){
      S->panel[1] = 'E';
      S->sys_errno = EPROTO;
      S->errmsg = "unknown packet protocol in status reply";
      return;
  }
  if(pkt[1] != 'S'){
      S->panel[1] = 'E';
      if(pkt[1] == 'E'){
          S->sys_errno = (int)upak32_unpack(&pkt[3]);
          S->errmsg = "error reported in status reply";
      } else {
          S->sys_errno = EPROTO;
          S->errmsg = "unknown packet type in status reply";
      }
      return;
  }

  /* save binary-encoded status for later interpretation: */
  buf_copy(S->binstat, pkt_data(pkt), BINSTAT_SIZE);

  return;
}


/* svstat_unpack()
**   unpack S->binstat and interpret service status into panel
**   compute uptimes compared to now
*/
static
void
svstat_unpack(struct svstat *S, tain_t *now)
{
  uchar_t  *buf = S->binstat;
  pid_t     pid;
  tain_t    when;
  uint64_t  uptime;
  int       flags;

  /* already flagged error/inactive in svstat_query(): */
  if((S->panel[1] == 'E') || (S->panel[1] == '-')) return;

  /* svdef: */
  flags = buf[28];
  /* marked for deactivation (and possibly reactivating)? */
  if(flags & SVDEF_FLAG_CULL){
      /* mark with attention and continue to fill panel: */
      S->panel[1] = (flags & SVDEF_FLAG_CYCLE) ? 'R' : '!';
  }
  /* log? */
  S->has_log = (flags & SVDEF_FLAG_HASLOG) ? 1 : 0;

  /* main: */
  S->pid_main = pid = upak32_unpack(&buf[30]);
  tain_unpack(&when, &buf[34]);
  S->uptime_main = uptime = tain_uptime(now, &when);
  flags = buf[46];
  S->panel[3] = pid != 0 ? '+' : '.';
  S->panel[4] = S->panel[3];
  S->panel[5] = S->panel[3];
  if( ((pid == 0) && !(flags & SUBSV_FLAG_WANTDOWN)) ||
      ((pid != 0) && (flags & SUBSV_FLAG_WANTDOWN)) ){
      S->panel[3] = '!';
  }
  if(pid > 0){
      if(flags & SUBSV_FLAG_ISONCE) S->panel[4] = 'o';
      if(flags & SUBSV_FLAG_ISPAUSED) S->panel[5] = 'p';
      if(flags & SUBSV_FLAG_ISRESET) S->panel[5] = 'r';
      /* munge uptime < 1 sec to wantup: */
      if((uptime < 1) && !(flags & SUBSV_FLAG_ISRESET)){
          S->pid_main = 0;
          S->panel[3] = '!';
          S->panel[4] = '.';
          S->panel[5] = '.';
      }
  }

  if(!S->has_log)
      return;

  /* log: */  
  S->pid_log = pid = upak32_unpack(&buf[48]);
  tain_unpack(&when, &buf[52]);
  S->uptime_log = uptime = tain_uptime(now, &when);
  flags = buf[64];
  S->panel[7] = pid != 0 ? '+' : '.';
  S->panel[8] = S->panel[7];
  S->panel[9] = S->panel[7];
  if( ((pid == 0) && !(flags & SUBSV_FLAG_WANTDOWN)) ||
      ((pid != 0) && (flags & SUBSV_FLAG_WANTDOWN)) ){
      S->panel[7] = '!';
  }
  if(pid > 0){
      if(flags & SUBSV_FLAG_ISONCE) S->panel[8] = 'o';
      if(flags & SUBSV_FLAG_ISPAUSED) S->panel[9] = 'p';
      if(flags & SUBSV_FLAG_ISRESET) S->panel[9] = 'r';
      /* munge uptime < 1 sec to wantup: */
      if((uptime < 1) && !(flags & SUBSV_FLAG_ISRESET)){
          S->pid_log = 0;
          S->panel[7] = '!';
          S->panel[8] = '.';
          S->panel[9] = '.';
      }
  }

  return;
}


/* svstat_setcap()
**   setup svstat->cap_id based on svstat
*/
static
void
svstat_setcap(struct svstat *S)
{
  char *panel = S->panel;

  switch(panel[1]){
  case 'E': S->cap_id = CAP_ER; return;
  case '!':
  case 'R':
      S->cap_id = CAP_EX; return;
  case '-': S->cap_id = CAP_NA; return;
  case '+': S->cap_id = CAP_AN; break;
  default : S->cap_id = CAP_ER; return;
  }

  if(panel[3] == '!'){
      if(panel[4] == '.')
          S->cap_id = CAP_WU;
      else
          S->cap_id = CAP_WD;
      return;
  }

  if(panel[7] == '!'){
      if(panel[8] == '.')
          S->cap_id = CAP_WU;
      else
          S->cap_id = CAP_WD;
      return;
  }

  if(panel[4] == '.'){
      S->cap_id = CAP_AD;
      return;
  }

  switch(panel[5]){
  case 'p': S->cap_id = CAP_AP; break;
  case 'r': S->cap_id = CAP_AR; break;
  case '+': /* fallthrough: */
  default : S->cap_id = CAP_AN;
  }

  return;
}


/*
** svstat destructor:
*/
static
void
svstat_free(struct svstat *S)
{
  if(S){
      if(S->name)
          free(S->name);
      if(S->panel)
          free(S->panel);
      free(S);
  }

  return;
}


/* captab_init()
**   initialize global captab with attributes extracted from db
*/
static
void
captab_init(const char *db)
{
    const char  *attr;
    size_t       len = 0;
    size_t       i;

    /* initialize defined capabilities: */
    for(i = 0; i < LSCAPS; ++i){
        if(captab[i].cap_id != i){
            /* internal consistency check: */
            barf("barfing on programmer error in captab_init()");
        }
        attr = captab_lookup(db, captab[i].cap_key, &len);
        if(attr && (len < CAP_ATTR_MAX)){
            ((char *)buf_copy(captab[i].attr, attr, len))[len] = '\0';
        } else {
            /* fill with default attribute: */
            cstr_copy(captab[i].attr, captab[CAP_DF].attr);
        }  
    }

    return;
}


/* captab_lookup()
** search captab db for capability matching key
** if found:
**     if attr_len is supplied (non-NULL), set to length of attribute
**     return start position of relevant attribute within db
** not found:
**     return NULL
**     no change to attr_len
*/
static
const char *
captab_lookup(const char *db, const char *key, size_t *attr_len)
{
    const char  *d0;
    const char  *attr = NULL;
    size_t       key_len;

    if((db == NULL) || (key == NULL) || (*key == '\0'))
        return NULL;

    key_len = cstr_len(key);

    d0 = db;
    while(*d0 != '\0'){
        size_t       k = 0;
        const char  *d = d0;  

        while((*d != '\0') && (*d != '='))
            ++d;

        if(*d == '\0')
            /* reached end of db: */
            break;

        /* else: */
        /* save key length of current cap: */
        k = d - d0;
        /* increment beyond '=' : */
        ++d;

        if((k == key_len) && (cstr_ncmp(key, d0, k) == 0)){
            /* cap found: */
            /* attribute for cap begins at d: */
            attr = d;
            if(attr_len != NULL){
                /* get length of attribute: */
                while((*d != '\0') && (*d != ':'))
                    ++d;
                /* setup for return: */
                *attr_len = d - attr;
            }
            /* all done: */
            break;
        }

        /* else: */
        /* cap not found in this loop: */
        /* increment to next key in db: */
        while((*d != '\0') && (*d != ':'))
            ++d;

        /* increment beyond ':' : */
        if(*d == ':')
            ++d;

        /* if *d=='\0', reached end of db ...*/
        d0 = d;
    }

    return attr;
}



static
int
cmp_byname(const void *a, const void *b)
{
    /* fun with pointers: */
    struct svstat *sv1 = *(struct svstat **)a;
    struct svstat *sv2 = *(struct svstat **)b;

    return cstr_cmp(sv1->name, sv2->name);
}


static
int
cmp_byuptime(const void *a, const void *b)
{
    /* fun with pointers: */
    struct svstat *sv1 = *(struct svstat **)a;
    struct svstat *sv2 = *(struct svstat **)b;

#if 0
    /* note:
    ** we want default sort to be "youngest" first
    **
    ** if tain_less(sv1, sv2) true:
    **    the start time of sv1 was actually farther in the past
    **    ie: sv1 "older/greater" than sv2
    **    ie: return 1
    **
    ** we want:
    **   "older" case to return 1
    **   "younger" case to return -1
    */

    if(tain_less(&sv1->tain_main, &sv2->tain_main))
        return 1;

    if(tain_less(&sv2->tain_main, &sv1->tain_main))
        return -1;
#endif

    if(sv1->uptime_main < sv2->uptime_main)
        return -1;

    if(sv1->uptime_main > sv2->uptime_main)
        return 1;

    /* else: */
    return cstr_cmp(sv1->name, sv2->name);
}


/* name_pad()
**   right pad "name" with whitespace upto width
**   note: return is pointer to internal static buffer
*/
static
const char *
name_pad(const char *name, size_t width)
{
    static char   padbuf[PADMAX + 1];
    const char   *s = name;
    char         *p = &padbuf[0];

    if(width > PADMAX){
        return NULL;
    }

    while(width && *s){
        *p = *s; ++p; ++s; --width;
    }

    while(width){
        *p = ' '; ++p; --width;
    }

    *p = '\0';

    return (const char *)padbuf;
}


int
main(int argc, char *argv[])
{
  nextopt_t        nopt = nextopt_INIT(argc, argv, ":hVb:cGgKrt");
  char             opt;
  int              opt_G = 0; /* explicit want colorized */
  int              opt_K = 0; /* undocumented show color */
  int              opt_r = 0; /* reverse display order */
  int              use_color = 1;
  const char      *basedir = NULL;
  struct dynstuf  *svstuf = NULL;
  struct svstat   *svstat = NULL;
  dynstuf_cmp_t    sort_by = NULL;
  const char      *cap_db = NULL;
  char             pathbuf[256];
  size_t           n;
  tain_t           now;
  int              fd_conn;
  size_t           i, width = 0;

  progname = nextopt_progname(&nopt);
  while((opt = nextopt(&nopt))){
      char  optc[2] = {nopt.opt_got, '\0'};
      switch(opt){
      case 'h': usage(); die(0); break;
      case 'V': version(); die(0); break;
      case 'b': basedir = nopt.opt_arg; break;
      case 'c': basedir = "."; break;
      case 'G': opt_G = 1; use_color = 1; break;
      case 'g': opt_G = 0; use_color = 0; break;
      case 'K': opt_K = 1; break;
      case 'r': opt_r = 1; break;
      case 't': sort_by = &cmp_byuptime; break;
      case ':':
          fatal_usage("missing argument for option -", optc);
          break;
      case '?':
          if(nopt.opt_got != '?'){
              fatal_usage("invalid option -", optc);
          }
          /* else fallthrough: */
      default : die_usage(); break;
      }
  }

  argc -= nopt.arg_ndx;
  argv += nopt.arg_ndx;

  /* using color? */
  if(use_color){
      cap_db = getenv("PERPLS_COLORS");
      switch(opt_G){
      case 1:
          /* colorized output explicitly requested
          ** use_color even if:
          **   PERPLS_COLORS not defined in environment
          **   isatty(stdout) == 0
          */
          if((cap_db == NULL) || (cap_db[0] == '\0')){
              cap_db = PERPLS_COLORS_DEFAULT;
              if(cap_db[0] == '\0'){
                  /* not compiled with color support: */
                  cap_db = NULL;
                  use_color = 0;
              }
          }
          break;
      default:
          /* colorize by default
          ** use_color only if:
          **     PERPLS_COLORS defined in environment
          **     isatty(stdout) == 1
          */
          if((cap_db == NULL) || (cap_db[0] == '\0') || (isatty(1) == 0)){
              cap_db = NULL;
              use_color = 0;
          }
      }
  }

  /* initialize captab: */
  if(use_color){
      captab_init(cap_db);
  } 

  /* display color capabilities and exit (undocumented): */
  if(use_color && opt_K){
      eputs(progname, ": color capabilities: ", cap_db);
      die(0);
  }

  /* initialize svstuf: */
  svstuf = dynstuf_new();
  if(svstuf == NULL){
      fatal(111, "allocation failure");
  }

  if(basedir == NULL)
      basedir = getenv("PERP_BASE");
  if((basedir == NULL) || (basedir[0] == '\0'))
      basedir = ".";

  /* all work from basedir: */
  if(chdir(basedir) != 0){
      fatal_syserr("unable to chdir() to ", basedir);
  }

  /* build list of sv args in svstuf: */
  if(*argv != NULL){
  /* initialize svstat list from command line: */
      while(*argv != NULL){
          svstat = svstat_init(NULL, *argv);
          if(svstat == NULL){
              fatal(111, "allocation failure");
          }
          if(dynstuf_push(svstuf, svstat) != 0){
              fatal(111, "allocation failure");
          }
          ++argv;
      }
  } else {
  /* initialize svstat list from current directory: */
      DIR            *dir;
      struct dirent  *d;

      if((dir = opendir(".")) == NULL){
          fatal_syserr("failure opendir() on base directory ", basedir);
      }

      while((d = readdir(dir)) != NULL){
          struct stat   st;

          /* skip entries beginning with '.': */
          if(d->d_name[0] == '.'){
              continue;
          }

          /* skip non-directory entries: */
          if ((stat(d->d_name, &st) != 0) || (! S_ISDIR(st.st_mode))){
              continue;
          }

          svstat = svstat_init(NULL, d->d_name);
          if(svstat == NULL){
              fatal(111, "allocation failure");
          }
          if(dynstuf_push(svstuf, svstat) != 0){
              fatal(111, "allocation failure");
          }
      }
      closedir(dir);

      /* sort directory list by name: */
      if(sort_by == NULL)
          sort_by = &cmp_byname;
  }

  /* connect to control socket: */
  n = cstr_vlen(basedir, "/", PERP_CONTROL, "/", PERPD_SOCKET);
  if(!(n < sizeof pathbuf)){
      errno = ENAMETOOLONG;
      fatal_syserr("failure locating perpd control socket ",
                   basedir, "/", PERP_CONTROL, "/", PERPD_SOCKET);
  }
  cstr_vcopy(pathbuf, basedir, "/", PERP_CONTROL, "/", PERPD_SOCKET);
  fd_conn = domsock_connect(pathbuf);
  if(fd_conn == -1){
      if(errno == ECONNREFUSED){
          fatal_syserr("perpd not running on control socket ", pathbuf);
      } else {
          fatal_syserr("failure connecting to perpd control socket ", pathbuf);
      }
  }

  /* uptimes compared to now: */
  tain_now(&now);

  /* svstat_fill()
  **   query status from perpd socket for each svstat object
  **   scan/update for longest name
  */
  for(i = 0; i < dynstuf_ITEMS(svstuf); ++i){
      size_t  w;
      svstat = (struct svstat *)dynstuf_get(svstuf, i);
      svstat_query(fd_conn, svstat);
      w = cstr_len(svstat->name);
      if(w > width) width = w; 
  }  

  /* close fd_conn (ignore error): */
  close(fd_conn);

  /* apply sort: */
  if(sort_by != NULL){
      dynstuf_sort(svstuf, sort_by);
  }

  /* reverse order: */
  if(opt_r == 1){
      dynstuf_reverse(svstuf);
  }

  /* print results: */
  for(i = 0; i < dynstuf_ITEMS(svstuf); ++i){
      const char  *panel;
      const char  *name;
      char        *attr = NULL;
      char         up_main[NFMT_SIZE], pid_main[NFMT_SIZE];
      char         up_log[NFMT_SIZE], pid_log[NFMT_SIZE];

      svstat = dynstuf_get(svstuf, i);
      svstat_unpack(svstat, &now);
      if(use_color){
          svstat_setcap(svstat);
          attr = captab[svstat->cap_id].attr;
      }
      panel = svstat->panel;
      name = name_pad(svstat->name, width);

      if(use_color && (attr != NULL)){
          /* colorizing name: */
          vputs(panel, "  ", "\033[00m\033[", attr, "m", name, "\033[00m");
      } else {
          vputs(panel, "  ", name);
      }

      switch(panel[1]){
      case 'E':
          vputs("  error: ", svstat->errmsg, " [", sysstr_errno(svstat->sys_errno), "]");
          break;
      case '+':
          vputs("  ",
               /* uptime: */
               "uptime: ",
               ((panel[4] == '+') || (panel[4] == 'o'))
                   ? nfmt_uint32(up_main, svstat->uptime_main)
                   : "-",
               "s/",
               ((panel[8] == '+') || (panel[8] == 'o'))
                   ? nfmt_uint32(up_log, svstat->uptime_log)
                   : "-",
               "s",
               /* pids: */
               "  pids: ",
               ((panel[4] == '+') || (panel[4] == 'o'))
                   ? nfmt_uint32(pid_main, svstat->pid_main)
                   : "-",
               "/",
               ((panel[8] == '+') || (panel[8] == 'o'))
                   ? nfmt_uint32(pid_log, svstat->pid_log)
                   : "-");
          break;
      case '-':
          vputs("  (service not activated)");
          break;
      case '!':
          vputs("  (service deactivation in progress)");
          break;
      case 'R':
          vputs("  (service reactivation in progress)");
          break;
      default:
          vputs("  ???");
          break;
      }
      vputs("\n");

  }

  vputs_flush();
  dynstuf_free(svstuf, (void (*)(void *))&svstat_free);

  die(0);
}


/* eof: perpls.c */
