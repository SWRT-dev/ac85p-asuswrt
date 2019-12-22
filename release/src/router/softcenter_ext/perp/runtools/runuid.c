/* runuid.c
** exec prog with permissions of user uid
** (djb setuidgid facility, with enhancements)
** wcm, 2009.09.08 - 2012.01.04
** ===
*/

#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>

#include "execvx.h"
#include "nextopt.h"

#include "runtools_common.h"

static const char *progname = NULL;
static const char prog_usage[] =
  "[-hV] [-g grp] [-s | -S grplist] account program [args ...]";

/* functions in local scope: */
static void do_grplist(gid_t gid, const char *grplist);


/* XXX, no relation to sysconf values: */
#define MY_NGROUPS  32
#define MY_GRPBUF   255

static
void
do_grplist(gid_t gid, const char *grplist)
{
  struct group *gr = NULL;
  gid_t         gidset[MY_NGROUPS];
  char          grpbuf[MY_GRPBUF];
  int           n = 0, n_max = (int)MY_NGROUPS;
  int           k = 0, k_max = (int)MY_GRPBUF;
  const char   *s = grplist;
  char          c;

  /* base gid entered as first group in gidset: */
  gidset[n++] = gid;

  /* parse colon-delimited grplist: */
  for(;;){
      c = *s;
      if((c == ':') || (c == '\0')){
          grpbuf[k] = '\0';
          if(grpbuf[0] == '\0'){
              fatal(100, "empty group name found in group list ", grplist);
          }
          if((gr = getgrnam(grpbuf)) == NULL){
              fatal(111, "no group found for \"", grpbuf, "\" in grplist ", grplist);
          }
          if(!(n < n_max)){
              fatal(111, "too many group names in group list ", grplist);
          }
          gidset[n++] = gr->gr_gid;
          k = 0;
          /* loop terminal: */
          if(c == '\0') break;
      }else{
          grpbuf[k++] = c;
          if(!(k < k_max)){
              fatal(111, "excessive length group name found in ", grplist);
          }
      }
      ++s;
  }

  /* set: */
  if(setgroups(n, gidset) == -1){
      fatal_syserr(111, "failure setgroups() for group list ", grplist);
  }

  return; 
}



int
main(int argc, char *argv[], char *envp[])
{
  char            opt;
  nextopt_t       nopt = nextopt_INIT(argc, argv, "hVg:S:s");
  struct passwd  *pw = NULL;
  struct group   *gr = NULL;
  const char     *user_acct = NULL;
  const char     *prog = NULL;
  /* alternative base group: */
  const char     *arg_grp = NULL;
  /* supplemental groups from command-line: */
  int             opt_grplist = 0;
  const char     *arg_grplist = NULL;
  /* supplemental groups from /etc/group (ie, initgroups(3): */
  int             opt_grpsupp = 0;
  uid_t           uid;
  gid_t           gid;

  progname = nextopt_progname(&nopt);
  while((opt = nextopt(&nopt))){
      char optc[2] = {nopt.opt_got, '\0'};
      switch(opt){
      case 'V': version(); die(0); break;
      case 'h': usage(); die(0); break;
      case 'g': arg_grp = nopt.opt_arg; break;
      case 'S':
          opt_grplist = 1;
          arg_grplist = nopt.opt_arg;
          break;
      case 's': opt_grpsupp = 1; break;
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

  if(getuid() != 0){
      fatal_usage("not running as super user");
  }

  if(argc < 2){
      fatal_usage("missing required argument(s)");
  }

  if(opt_grpsupp && opt_grplist){
      fatal_usage("options -s and -S <grplist> are mutually exclusive");
  }

  user_acct = argv[0];
  ++argv;
  prog = argv[0];

  pw = getpwnam(user_acct);
  if(pw == NULL){
      fatal(111, "no account found for user ", user_acct);
  }
  
  gid = pw->pw_gid;
  uid = pw->pw_uid;

  /* alternative base group: */
  if(arg_grp != NULL){
      gr = getgrnam(arg_grp);
      if(gr == NULL){
          fatal(111, "no group found for group ", arg_grp);
      }
      gid = gr->gr_gid;
  }

  if(opt_grplist){
      /* supplemental groups given on command-line: */
      do_grplist(gid, arg_grplist);
  }else if(opt_grpsupp){
      /* supplemental groups from /etc/group with initgroups(3): */
      /* XXX, note: initgroups(3) may malloc() */
      if(initgroups(user_acct, gid) == -1){
          fatal_syserr("failure initgroups()");
      }
  }else{
      if(setgroups(1, &gid) == -1){
          fatal_syserr("failure setgroups()");
      }
  }

  if(setgid(gid) == -1){
      fatal_syserr("failure setgid()");
  }

  if(setuid(uid) == -1){
      fatal_syserr("failure setuid()");
  } 

  /* execvx() provides path search for prog */
  execvx(prog, argv, envp, NULL);

  /* uh oh: */
  fatal_syserr("unable to run ", prog);

  /* not reached: */
  return 0;
}


/* eof: runuid.c */
