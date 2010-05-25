#ifndef _CUSTOSBL_H
#define _CUSTOSBL_H
/*===========================================================================

              Operating System  B o o t   L o a d e r
                        C U S T O S B L

DESCRIPTION
  This header file provides OSBL customer specific information.
  
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:$
$Header: //depot/asic/qsc1100/romboot/osbl/custosbl.h#1 $
when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/07/07   ly      Create
===========================================================================*/


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/* Make sure that CUST_H is defined and then include whatever file it
** specifies.
*/
#ifdef CUST_H
#include CUST_H
#else
#error Must Specify /DCUST_H=CUSTxxxx.H on the compile line
#endif

#ifndef _ARM_ASM_
#include "osbl_error_handler.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

#ifdef MSG_HIGH
  #undef MSG_HIGH
#endif
#define MSG_HIGH(x,a,b,c)

#ifdef  MSG_ERROR
  #undef  MSG_ERROR
#endif
#define MSG_ERROR(x,a,b,c) OSBL_ERR_FATAL( BL_ERR_OSBL )

#undef  ERR
#define ERR(x,a,b,c)

#undef ERR_FATAL
#define ERR_FATAL(x,a,b,c) OSBL_ERR_FATAL( BL_ERR_OSBL )

#ifdef ASSERT
  #undef ASSERT
#endif

#define ASSERT	2

#ifndef ERR_H
  #define ERR_H
#endif

/* Perform a busy wait by programming the MSM's pause timer for tools */
#undef clk_busy_wait
extern void osbl_clk_busy_wait (uint32);
#define clk_busy_wait(us)                                             \
        osbl_clk_busy_wait( (us) )


/* Default flash error behavior is *BAD*, no dload entry :( 
 * Fix it by routing to the BL error handler */
#if defined(FLASH_ERR_FATAL)
  #undef FLASH_ERR_FATAL
  #define FLASH_ERR_FATAL(x,a,b,c) OSBL_ERR_FATAL( BL_ERR_FLASH_DRIVER )
#endif /* defined(FLASH_ERR_FATAL) */


#endif /* not _ARM_ASM_ */
#endif /* _CUSTOSBL_H */
