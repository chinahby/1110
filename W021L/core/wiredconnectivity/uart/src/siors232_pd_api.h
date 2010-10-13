#ifndef SIORS232_PD_API_H
#define SIORS232_PD_API_H
/*===========================================================================

              S I O   D E V I C E   L A Y E R   H E A D E R 

DESCRIPTION
   This is the header file for the interface layer of SIO.  Therefore, this 
   header file provides the complete interface to SIO services.

   Copyright (c) 1998 by QUALCOMM Incorporated.  All Rights Reserved.
   Copyright (c) 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: O:/src/asw/COMMON/vcs/siors232.h_v   1.1   19 Oct 2001 16:04:58   chenc  $
  $Header: //depot/asic/msmshared/drivers/uart/02.00.XX/siors232_pd_api.h#2 $ $DateTime: 2009/02/11 17:20:36 $ $Author: cmihalik $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/02/07   rh      Created file.
===========================================================================*/

/*===========================================================================

                        FEATURE FLAGS RESOLUTION 

===========================================================================*/

/*===========================================================================
 
 						Include Files

===========================================================================*/

#include "sio.h"                             /* SIO Server header file     */
#include "clk.h"                             /* Clock services header file */
#include "customer.h"                        /* Customer feature files     */
#include "tramp.h"                 /* Trampoline typedefs and prototypes   */
#include "uart_compat.h"                 

/*===========================================================================

                        ENUMERATION TYPE

===========================================================================*/


/*===========================================================================

                        DATA STRUCTURES 

===========================================================================*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*===========================================================================
 FUNCTION/PROCEDURE prototypes.
===========================================================================*/              

/*===========================================================================

FUNCTION SIORS_INITIALIZE							EXTERNALIZED FUNCTION

DESCRIPTION
  This is the only externalized function which the MDD layer can call.
  The function initialize the exported API table so the MDD layer can 
  access the UART driver

DEPENDENCIES
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void siors_pd_initialize
(
   uart_device_id_type  device_id,             
	sio_pdd_api_type    *pFuncTab,
	void 				    **pHead,
	void				     *pMddHead,
	void               (*mdd_callback) (void *, siors_mdd_callback_type)
);


void siors_dml_initialize
(
   uart_device_id_type  device_id,             
	sio_pdd_api_type    *pFuncTab,
	void 				    **pHead,
	void				     *pMddHead,
	void               (*mdd_callback) (void *, siors_mdd_callback_type)

);


void siors_dm_initialize
(
   uart_device_id_type  device_id,
   sio_pdd_api_type    *pFuncTab,
   void               **pHead,
   void                *pMddHead,
   void               (*mdd_callback) (void *, siors_mdd_callback_type)
);


#endif /* SIORS232_PD_API_H */
