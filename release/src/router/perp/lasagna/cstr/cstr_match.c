/* cstr_match.c
** wcm, 2010.11.09 - 2010.11.12
** ===
*/
#include "cstr.h"

#define CHAR_CMP(c1,c2) ((c1) == (c2))

/* cstr_match()
**   synopsis:
**     match string against pattern
**     pattern may include:
**       - any literal character, matched case-sensitive
**       - '?' matches any single character
**       - '*' matches any zero or more characters
**     pattern is nul-terminated
**
**   return:
**     1: match
**     0: no match
**
**   notes:
**     fast, non-recursive implementation
**     no supplemental storage allocated (eg, as compared to regex)
**     less "powerful" than fnmatch(), regex()
**         eg: no [..] set/range match, no escaping wildcards, etc.
**     pattern is implicitly anchored from beginning to end of string
*/

/* note:
** this implementation derived from analysis of wildcmp(),
** by Jack Handy, as posted on codeproject.com:
**
**   http://www.codeproject.com/KB/string/wildcmp.aspx
**
** (noted 2010.11.09)
*/

int
cstr_match(const char *pattern, const char *string)
{

  const char  *p = NULL;  /* mark start of current pattern block */
  const char  *s = NULL;  /* mark next substring */

  /* spin off any initial matching chunk of string and pattern: */
  while((*string != '\0') && (*pattern != '*')){
      if(!(CHAR_CMP(*pattern, *string) || (*pattern == '?'))){
          /* short circuit fail on mismatch: */
          return 0;
      }
      ++pattern;
      ++string;
  }

  /* scan through string */
  /* note:
  **   this block will be entered only if an asterisk encountered in pattern
  **   on the initial scan above
  */
  while(*string != '\0'){
      if(*pattern == '*'){
          /* pattern at asterisk */
          /* increment pattern (merging consecutive asterisks): */
          do{
              ++pattern;
          }while(*pattern == '*');
          /* a final asterisk in pattern matches any remaining string: */
          if(*pattern == '\0'){
              /* short circuit return on terminal asterisk: */
              return 1;
          }
          /* mark start of current pattern block: */
          p = pattern;
          /* mark next substring: */
          s = &string[1];
      }else if(CHAR_CMP(*pattern, *string) || (*pattern == '?')){
          /* pattern at an exact or '?' match of string */
          /* increment pattern and string: */
          ++pattern;
          ++string;
#if 1
      /* dubious "optimization" */
      }else if(string[1] == '\0'){
          /* pattern match failed on last character of string */
          /* short circuit return: */
          return 0;
#endif
      }else{
          /* pattern match failed within current substring */
          /* reset to start of current pattern block: */
          pattern = p;
          /* seek to next substring: */
          string = s;
          /* update next substring: */
          ++s;
      }
  }

  /* spin off any remaining asterisks in pattern: */
  while(*pattern == '*'){
      ++pattern;
  }

  /* return value logic:
  ** reached to end (nul) of pattern?
  **   yes: match
  **   no:  no match
  */ 
  return (*pattern == '\0') ? 1 : 0;
}


/* eof (cstr_match.c) */
