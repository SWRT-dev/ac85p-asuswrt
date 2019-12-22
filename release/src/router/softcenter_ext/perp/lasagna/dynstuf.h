/* dynstuf.h
** dynstuf: dynamic storage of arbitrary data
** wcm, 2006.06.19 - 2012.07.25
** ===
*/
#ifndef DYNSTUF_H
#define DYNSTUF_H 1

#include <stddef.h>  /* size_t */


/* dynstuf object datatype */
struct dynstuf {
    void    **stuf;   /* think:  void *stuf[], a vector of anonymous pointers */
    size_t    slots;  /* stuf allocated */
    size_t    items;  /* stuf used (note: stuf[0] .. stuf[items - 1]) */
};
typedef struct dynstuf dynstuf_t;


/* interface */

/* dynstuf_INIT()
** dynstuf static initialization
** usage:
**   dynstuf dyn = dynstuf_INIT()
*/
#define  dynstuf_INIT()  {NULL, 0, 0}

/* dynstuf object constructor: */
/* allocate and initialize dynstuf
** return:
**   initialized dynstuf on success
**   NULL on error (allocation failure)
*/
extern struct dynstuf * dynstuf_new(void);

/* dynstuf object initialization: */
/* initialize S
** if S is NULL, allocate new dynstuf object and initialize it
** return:
**   initialized dynstuf on success
**   NULL on error (allocation failure)
*/
extern struct dynstuf * dynstuf_init(struct dynstuf *S);

/* preallocate more slots in dynstuf: */
/* only needed if you want to avoid repetitive error checking on dynstuf_push()
** return:
**    0 (no error) on success
**   -1 on error
*/
extern int dynstuf_grow(struct dynstuf *, size_t add);

/* destructor: */
/* optional second argument may be used to free data items within dynstuf
** before the dynstuf itself is freed
** (ignored if NULL)
*/
extern void dynstuf_free(struct dynstuf *, void (*free_data)(void *data));


/* dynstuf metadata: */
extern size_t dynstuf_items(struct dynstuf *);
#define dynstuf_ITEMS(S) ((S)->items)

extern size_t dynstuf_isempty(struct dynstuf *);
#define dynstuf_ISEMPTY(S) ((S)->items == 0)

extern size_t dynstuf_slots(struct dynstuf *);
#define dynstuf_SLOTS(S) ((S)->slots)

extern void ** dynstuf_stuf(struct dynstuf *);
#define dynstuf_STUF(S)  ((S)->stuf)


/* dynstuf insertion: */
/* return:
**   0 (no error) on success
**  -1 on error
*/
extern int dynstuf_push(struct dynstuf *S, void *item);

/* dynstuf stack operations: */
extern void * dynstuf_peek(struct dynstuf *S);
extern void * dynstuf_pop(struct dynstuf *S);

/* get item in stuf[slot]: */
/* returns NULL if slot >= S->items, else item */
extern void * dynstuf_get(struct dynstuf *S, size_t slot);

/* set item in stuf[slot]: */
/* returns NULL if slot >= S->items, else item */
extern void * dynstuf_set(struct dynstuf *S, size_t slot, void *item);

/* replace item at stuf[slot] with newitem: */
/* returns NULL if slot >= S->items, else original item */
extern void * dynstuf_replace(struct dynstuf *S, size_t slot, void *newitem);


/* type signature for user-supplied comparison function in:
**   dynstuf_find()
**   dynstuf_sort()
*/
typedef int(*dynstuf_cmp_t)(const void *a, const void *b);

/* search stuf from stuf[slot] for item matching key:
**   a sequential search; S->stuf may be unorderd
**   returns matching slot if found, else S->items
*/
extern size_t dynstuf_find(
    struct dynstuf *S,
    size_t          slot,
    void           *key,
    int            (*cmp)(const void *key1, const void *key2)
); 


/* sort items in dynstuf using supplied cmp() function: */
extern void dynstuf_sort(struct dynstuf *S, int(*cmp)(const void *a, const void *b));
/* that is:
**   void dynstuf_sort(struct dynstuf *S, dynstuf_cmp_t cmp);
*/

/* put items in dynstuf in reverse order: */
extern void dynstuf_reverse(struct dynstuf *S);

/* apply a "visitor" function to each item in dynstuf: */
extern void dynstuf_visit(struct dynstuf *S, void (*visitor)(void *item, void *xtra), void *xtra);


#endif /* DYNSTUF_H */
