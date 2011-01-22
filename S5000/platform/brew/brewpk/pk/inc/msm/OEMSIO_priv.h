
/*============================================================================
FILE:	OEMSIO_Priv.h

SERVICES:  OEMSIO private data

DESCRIPTION:
   This file provides private defintions for the IPort interface for Serial 
   Ports made available other services.

   
INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
#ifndef _OEMSIO_PRIV_H_
#define _OEMSIO_PRIV_H_

#include "AEEIOCtl.h"

#ifdef __cplusplus
extern "C" {
#endif

//Register a callback for when the cable state changes
#define AEESIO_IOCTL_DTR_NOTIFIER  BREWIOCTL_RW(BREWIOCTL_SIO_TYPE, 4, AEECallback)

//Get the current state of the cable
#define AEESIO_IOCTL_GET_DTRSTATUS BREWIOCTL_RW(BREWIOCTL_SIO_TYPE, 5, boolean)

//Register a callback for when data is completely flushed.  Data should
//not be sent subsequently until that data is flushed.
#define AEESIO_IOCTL_FLUSH_NOTIFIER BREWIOCTL_RW(BREWIOCTL_SIO_TYPE, 6, AEECallback)

#ifdef __cplusplus
}
#endif


/*=====================================================================
  
   SIO specific option IDs valid in IPORT_IOCtl()

   =========================================================
   AEESIO_IOCTL_DTR_NOTIFIER
      Register a callback for when the cable state changes.

   Option Value: AEECallback pointer. 
   
   Example:

   =========================================================
   AEESIO_IOCTL_GET_DTRSTATUS
      Get the current state of the cable.
   
   Option Value: boolean pointer. 
   
   Example:
   
   =========================================================
   AEESIO_IOCTL_FLUSH_NOTIFIER
      Register a callback for when data is completely flushed.  Data should
      not be sent subsequently until that data is flushed.

   Option Value: AEECallback pointer. 

   Example:

=======================================================================*/

#endif  //_OEMSIO_PRIV_H_

