/*******************************************************************

  OS Exercises - Project 2 - HOST dispatcher

    rsrc - resource management routines for the HOST dispatcher

    int rsrcChk (RsrcPtr available, Rsrc claim)
       - check that resources are available now 
      returns:
        TRUE or FALSE - no allocation is actally done

    int rsrcChkMax (Rsrc claim)
       - check that resources will ever be available
      returns:
        TRUE or FALSE

    int rsrcAlloc (RsrcPtr available, Rsrc claim)
       - allocate resources
      returns:
        TRUE or FALSE if not enough resources available

    void rsrcFree (RsrcPtr available, Rsrc claim);
       - free resources
      returns NONE

********************************************************************

  version: 1.0 bis (exercise 11 code template)
  date:    December 2003
  author:  Dr Ian G Graham, ian.graham@griffith.edu.au
  history:
     v1.0:     Original for exercises 11 and full project dispatcher
     v1.0 bis: Code template for exercise 11

*******************************************************************/

#include "rsrc.h"
                                          
/*******************************************************
 * int rsrcChk (RsrcPtr available, Rsrc claim)
 *    - check that resources are available
 *
 * returns:
 *    TRUE or FALSE - no allocation is actally done
 *******************************************************/
int rsrcChk(RsrcPtr available, Rsrc claim)
{
	if(available->printers >= claim.printers && available->scanners >= claim.scanners && available->modems >= claim.modems && available->cds >= claim.cds){
		return TRUE;
	}
	return FALSE;
}
    
/*******************************************************
 * int rsrcChkMax (Rsrc claim)
 *    - check that resources could be available
 *
 * returns:
 *    TRUE or FALSE - no allocation is actally done
 *******************************************************/
int rsrcChkMax(Rsrc claim)
{
    return FALSE;
}
    
/*******************************************************
 * int rsrcAlloc (RsrcPtr available, Rsrc claim)
 *    - allocate resources
 *
 * returns:
 *    TRUE or FALSE if not enough resources available
 *******************************************************/
int rsrcAlloc(RsrcPtr available, Rsrc claim)
{
  if(rsrcChk(available, claim) == TRUE){
		available->printers = available->printers - claim.printers;
		available->scanners = available->scanners - claim.scanners;
		available->modems = available->modems - claim.modems;
		available->cds = available->cds - claim.cds;
		return TRUE;
	}
	return FALSE;
}

/*******************************************************
 * void rsrcFree (RsrcPtr available, Rsrc claim);
 *    - free resources
 *
 * returns NONE
 *******************************************************/
void rsrcFree(RsrcPtr available, Rsrc claim)
{
  available->printers = available->printers - claim.printers;
	available->scanners = available->scanners - claim.scanners;
	available->modems = available->modems - claim.modems;
	available->cds = available->cds - claim.cds;
	return;
}
