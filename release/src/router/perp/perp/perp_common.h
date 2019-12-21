/* perp_common.h
** common defines for perp apps
** wcm, 2008.01.23 - 2013.01.11
** ===
*/
#ifndef PERP_COMMON_H
#define PERP_COMMON_H  1

/* release version string: */
#ifndef PERP_VERSION
#define PERP_VERSION "2.07"
#endif

/* default PERP_BASE directory: */
#ifndef PERP_BASE_DEFAULT
#define PERP_BASE_DEFAULT  "/etc/perp"
#endif

/* base service control directory
**   (relative to PERP_BASE; usually configured as a symlink):
*/
#define PERP_CONTROL  ".control"

/* perpboot directory
**   (relative to PERP_BASE):
*/
#define PERP_BOOT ".boot"

/* perpd pid/lockfile
**   (relative to PERP_CONTROL):
*/
#define PERPD_PIDLOCK  "perpd.pid"

/* perpd socket
**   (relative to PERP_CONTROL):
*/
#define PERPD_SOCKET   "perpd.sock"


/* perp svdef (service definition) flags: */
#define SVDEF_FLAG_ACTIVE  0x01
#define SVDEF_FLAG_HASLOG  0x02
#define SVDEF_FLAG_CULL    0x04
#define SVDEF_FLAG_CYCLE   0x08
#define SVDEF_FLAG_DOWN    0x10
#define SVDEF_FLAG_ONCE    0x20

/* perp subsv (subservice) flags: */
#define SUBSV_FLAG_ISLOG     0x01
#define SUBSV_FLAG_ISRESET   0x02
#define SUBSV_FLAG_ISONCE    0x04
#define SUBSV_FLAG_ISPAUSED  0x08
#define SUBSV_FLAG_WANTDOWN  0x10
#define SUBSV_FLAG_FAILING   0x20

/* perp command flags (second byte of command packet): */
#define SVCMD_FLAG_LOG     0x01
#define SVCMD_FLAG_KILLPG  0x02

/* default attributes for colorized perpls: */
#ifndef PERPLS_COLORS_DEFAULT
#define PERPLS_COLORS_DEFAULT  \
  "df=00:na=00:an=01:ar=01;33:ap=01;33:ad=01;34:wu=01;33:wd=01;33:er=01;31:ex=01;31"
#endif

/*
** other common macros:
*/
#include <unistd.h>
#define  die(e) \
    _exit((e))


#endif /* PERP_COMMON_H */
/* eof (perp_common.h) */
