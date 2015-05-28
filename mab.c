/*******************************************************************

  OS Exercises - Project 2 - HOST dispatcher

   mab - memory management functions for HOST dispatcher

   MabPtr memChk (MabPtr arena, int size);
      - check for memory available
 
    returns address of "First Fit" block or NULL

   int memChkMax (int size);
      - check for over max memory
 
    returns TRUE/FALSE OK/OVERSIZE

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

   void memPrint(MabPtr arena);
      - print contents of memory arena
   no return

 ********************************************************************
 
 version: 1.0 bis (exercises 10 & 11 code template)
 date:    December 2003
 author:  Dr Ian G Graham, ian.graham@griffith.edu.au
 history:
     v1.0:     Original for exercise 10
     v1.0 bis: Code template for exercises 10 & 11
 
 Note: This code template is based on the original version. It is
 edited and distributed by the COMP3520 teaching staff for use
 in COMP3520 labs.
 
 *******************************************************************/

#include "mab.h"

static MabPtr next_mab = NULL;    // Is this value always reasonable?

/*******************************************************
 * MabPtr memChk (MabPtr arena, int size);
 *    - check for memory available
 *
 * returns address of "First Fit" block or NULL
 *******************************************************/
MabPtr memChk(MabPtr arena, int size)
{
    while(arena){
      if(arena->allocated == 0){
        //If memory isn't allocated check for a fit
        if(arena->size >= size){
          return arena;
        }
      }
      arena = arena->next;
    }
    return NULL;
}
      
/*******************************************************
 * int memChkMax (int size);
 *    - check for memory available
 *
 * returns TRUE/FALSE
 *******************************************************/
int memChkMax(int size)
{
    return TRUE;
}      

/*******************************************************
 * MabPtr memAlloc (MabPtr arena, int size);
 *    - allocate a memory block
 *
 * returns address of block or NULL if failure
 *******************************************************/
MabPtr memAlloc(MabPtr arena, int size)
{
  if(arena == NULL){
    return NULL;
  }

  if(arena->size > size){
    //Split the memory and allocate:

    arena = memSplit(arena, size); //Split the memory;
    arena->allocated = 1;
    arena->next->allocated = 0;
    return arena;
  }
  else if(arena->size == size){
    //Just allocate, don't have to split memory
    arena->allocated = 1;
    return arena;
  }
  return NULL; //Failure to allocate, return null

}

/*******************************************************
 * MabPtr memFree (MabPtr mab);
 *    - de-allocate a memory block
 *
 * returns address of block or merged block
 *******************************************************/
MabPtr memFree(MabPtr m)
{
    if(m){
	  m->allocated = 0;
	  m = memMerge(m);
	  if(m->prev != NULL && m->prev->allocated == 0){
		m->prev->allocated = 0;
		m->prev = memMerge(m->prev); //IM A GODDD YEAHHH BRAHHHHHHHHHHHHHHHHHHHHHHHHHHHH
	  }
    }
    return m;
}
      
/*******************************************************
 * MabPtr memMerge(Mabptr m);
 *    - merge m with m->next
 *
 * returns m
 *******************************************************/
MabPtr memMerge(MabPtr m)
{
  if(m->next == NULL){
    return m;  
  }
  else if(m->next->allocated == 1){
    return m;
  }
  else{
    // Set the size of block B to mem_size_B + mem_size_C.
    MabPtr mabC = m->next;
    m->size = m->size + mabC->size;
    // Copy the "next" pointer for block C to the "next" pointer for block B
    m->next = mabC->next;
    // If there exists an adjacent memory block D to the right of block C:
    // Set the "prev" pointer for block D to the address of block B.
    MabPtr mabD = mabC->next;
    if(mabD != NULL){
      mabD->prev = m;
    }
    //Free memC
    free(mabC);
  }
  MabPtr mabZ = m;
//   If there exists an adjacent memory block A to the left of block B:
  if(m->prev != NULL && m->prev->allocated == 0){
    mabZ = memMerge(m->prev);
  }
  else{
    mabZ = m;
  }
  if(mabZ->next != NULL && mabZ->next->allocated == 0){
    mabZ = memMerge(mabZ);
  }
  return mabZ;

}

/*******************************************************
 * MabPtr memSplit(MabPtr m, int size);
 *    - split m into two with first mab having size
 *
 * returns m or NULL if unable to supply size bytes
 *******************************************************/
MabPtr memSplit(MabPtr m, int size)
{
  int alloc = m->allocated;
  if(m && m->size >= size){
    MabPtr mabC;
    if((mabC = (MabPtr) malloc(sizeof(Mab)))){

      int newSize = m->size - size;
      mabC->offset = m->offset + size;
      mabC->prev = m;
      mabC->next = m->next;
      mabC->size = newSize;
      mabC->allocated = alloc;

      m->size = size;
      m->allocated = alloc;
      if(m->next != NULL){
        m->next->prev = mabC;
      }
      m->next = mabC;

      return m;
    }
    perror("error allocating memory for new Mab");
    return NULL;
  }
  else{
    return NULL;
  }
}

/*******************************************************
 * void memPrint(MabPtr arena);
 *    - print contents of memory arena
 * no return
 *******************************************************/
void memPrint(MabPtr arena) // Do you really need this?
{
    while(arena) {
        printf("offset%7d: size%7d - ",arena->offset, arena->size);
        if (arena->allocated) printf("allocated\n");
        else printf("free\n");
        arena = arena->next;
    }
}
