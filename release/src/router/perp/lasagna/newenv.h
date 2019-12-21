/* newenv.h
** newenv: exec() with new environment
** specifically: provides the means to exec() a program with a new environment,
** _without_ altering the existing environment of the caller
** (djb pathexec)
** wcm, 2009.09.09 - 2009.10.07
** ===
*/
#ifndef NEWENV_H
#define NEWENV_H

/* newenv_set()
**   add/replace "name=value" variable for the newenv_exec() environment
**   if 'value' is NULL, will unset 'name' for the newenv_exec() environment
**
**   return
**     0 : success
**    -1 : failure, errno set
*/
extern
int newenv_set(const char *name, const char *value);

/* newenv_unset()
**   remove environmental variable 'name' from the newenv_exec() environment
**   (a convenience macro)
*/
#define newenv_unset(name)  newenv_set((name), NULL)


/* newenv_run()
**   calls execvx() with argument list argv[]
**   if envp non-NULL:
**     merges envp with any set by newenv_set()
**   if envp == NULL:
**     uses only environment set by newenv_set()
**   calls execvx()
**     * see execvx.h
**
**   return
**     on success, no return
**     on failure, -1 and errno set
*/ 
extern
int newenv_run(char * const argv[], char * const envp[]);

/* newenv_RUN()
** macro version of above
*/
#define newenv_RUN(av,ev) \
  newenv_exec(*(av)[0], (av), NULL, (ev))

/* newenv_exec()
**   if envp non-NULL:
**     merges envp with any environment set by newenv_set()
**   if envp == NULL:
**     uses only environment set my newenv_set()
**
**   ultimately calls:
**     execvx(prog, argv, new_envp, search);
**   where new_envp is the result of merging any envp with newenv_set()
**
**   if search is non-NULL, it is taken as a PATH-like string for finding prog
**   (if search is non-NULL, the environment PATH is not searched) 
**
**  returns
**    on success, no return
**    on failure, -1 and errno set
*/
extern
int newenv_exec(
  const char        *prog,
  char * const       argv[],
  const char        *search,
  char * const       envp[]
);

#endif /* NEWENV_H */
/* eof: newenv.h */
