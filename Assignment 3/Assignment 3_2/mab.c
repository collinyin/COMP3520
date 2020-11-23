/*******************************************************************

   mab - memory management functions for HOST dispatcher

   MabPtr memChk (MabPtr arena, int size);
      - check for memory available (any algorithm)
 
    returns address of "First Fit" block or NULL

   MabPtr memAlloc (MabPtr arena, int size);
      - allocate a memory block
 
    returns address of block or NULL if failure

   MabPtr memFree (MabPtr mab);
      - de-allocate a memory block
 
    returns address of block or merged block

   MabPtr memMerge(Mabptr m);
      - merge m with m->next
 
    returns m

   MabPtr memSplit(Mabptr m, int size);
      - split m into two with first mab having size
  
    returns m or NULL if unable to supply size bytes

*******************************************************************/

#include "mab.h"

/*******************************************************
 * MabPtr memChk (MabPtr arena, int size);
 *    - check for memory available 
 *
 * returns address of "First Fit" block or NULL
 *******************************************************/
MabPtr memChk(MabPtr arena, int size)
{

// you need to implement it.

}
      
/*******************************************************
 * MabPtr memAlloc (MabPtr arena, int size);
 *    - allocate a memory block
 *
 * returns address of block or NULL if failure
 *******************************************************/
MabPtr memAlloc(MabPtr arena, int size)
{

// You need to implement it.

}

/*******************************************************
 * MabPtr memFree (MabPtr mab);
 *    - de-allocate a memory block
 *
 * returns address of block or merged block
 *******************************************************/
MabPtr memFree(MabPtr m)
{

// You need to implement it.

}
      
/*******************************************************
 * MabPtr memMerge(Mabptr m);
 *    - merge m with m->next
 *
 * returns m
 *******************************************************/
MabPtr memMerge(MabPtr m)
{
    MabPtr n;

    if (m && (n = m->next)) {
        m->next = n->next;
        m->size += n->size;
        
        free (n);
        if (m->next) (m->next)->prev = m;
    }
    return m;
}

/*******************************************************
 * MabPtr memSplit(MabPtr m, int size);
 *    - split m into two with first mab having size
 *
 * returns m or NULL if unable to supply size bytes
 *******************************************************/
MabPtr memSplit(MabPtr m, int size)
{
    MabPtr n;
    
    if (m) {
        if (m->size > size) {
            n = (MabPtr) malloc( sizeof(Mab) );
            if (!n) {
                fprintf(stderr,"memory allocation error\n");
                exit(127);
            }
            n->offset = m->offset + size;
            n->size = m->size - size;
            m->size = size;
            n->allocated = m->allocated;
            n->next = m->next;
            m->next = n;
            n->prev = m;
            if (n->next) n->next->prev = n;
        }
        if (m->size == size) return m;
    }
    return NULL;    
}
