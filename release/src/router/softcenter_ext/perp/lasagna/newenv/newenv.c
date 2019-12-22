/* newenv.c
** wcm, 2009.09.09 - 2009.09.28
** ===
*/
/* stdlib: */
#include <errno.h>
#include <stdlib.h>

/* unix: */
#include <unistd.h>

#include "cstr.h"
#include "dynstr.h"
#include "dynstuf.h"
#include "execvx.h"

#include "newenv.h"


static dynstuf_t newenv = dynstuf_INIT();
static char ** newenv_merge(char * const envp[]);

int
newenv_set(const char *name, const char *value)
{
    dynstr_t  newvar = dynstr_INIT();

    if((name == NULL) || (name[0] == '\0'))
        return 0;

    if(dynstr_puts(&newvar, name) == -1)
        goto memfail;
    if(value != NULL){
        if(dynstr_vputs(&newvar, "=", value) == -1)
            goto memfail;
    }

    if(dynstuf_push(&newenv, dynstr_STR(&newvar)) == -1)
        goto memfail;

    return 0;

memfail:
    if(dynstr_STR(&newvar) != NULL)
        free(dynstr_STR(&newvar));
    errno = ENOMEM;
    return -1;    
}


static
char **
newenv_merge(char * const envp[])
{
    char     **ee;
    size_t     ee_len;
    size_t     n = 0;

    if(envp != NULL){
        while(envp[n] != NULL) ++n;
    }
    n += dynstuf_ITEMS(&newenv);
    ee = (char **)malloc((n+1) * sizeof(char *));
    if(ee == NULL){
        return NULL;
    }

    n = 0;
    if(envp != NULL){
        /* init ee with envp[]: */
        while(envp[n] != NULL){
            ee[n] = envp[n];
            ++n;
        }
    }
    ee_len = n;
    ee[ee_len] = NULL;

    /* merge newenv: */
    for(n = 0; n < dynstuf_ITEMS(&newenv); ++n){
        char     *envstr = dynstuf_get(&newenv, n);
        size_t    split = cstr_pos(envstr, '=');
        size_t    i;

        for(i = 0; i < ee_len; ++i){
            /* search existing environ for env: */
            if(cstr_pos(ee[i], '=') == split){
                if((envstr[split] == '=') && (cstr_ncmp(envstr, ee[i], split) == 0)){
                    /* replace existing env: */
                    ee[i] = envstr;
                    break;
                } else if(cstr_ncmp(envstr, ee[i], split - 1) == 0){
                    /* remove existing env: */
                    --ee_len;
                    ee[i] = ee[ee_len];
                    ee[ee_len] = NULL;
                    break;
                }
             }
         }
         if((i >= ee_len) && (envstr[split] == '=')){
             /* add new env: */
             ee[ee_len] = envstr;
             ++ee_len;
             ee[ee_len] = NULL;
         }
    }

    return ee;
}


int
newenv_run(char * const argv[], char * const envp[])
{
    return newenv_exec(argv[0], argv, NULL, envp);
}

 
int
newenv_exec(
  const char        *prog,
  char * const       argv[],
  const char        *search,
  char * const       envp[])
{
    char  **ee;
    int     err = 0;
    int     terrno;

    ee = newenv_merge(envp);
    if(ee == NULL){
        errno = ENOMEM;
        return -1;
    }
  
    err = execvx(prog, argv, (char * const *)ee, search);
    /* uh oh, exec() failure: */
    terrno = errno;
    /*
    ** free only ee
    ** (existing environ and allocated newenv will persist unchanged):
    */
    free(ee); 

    errno = terrno;
    return err; 
}


/* eof: newenv.c */
