/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M A I N   M O D U L E

GENERAL DESCRIPTION
  This module contains the AMSS exception handler

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright(c) 2007             by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/services/err/err_exception_handler.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/25/07   tbg     Initial verion (pulled code from mobile.c)
===========================================================================*/


#include <ctype.h>      
#include <locale.h>
#include "AEEstd.h"
#include "target.h"     
#include "customer.h"   
#include "comdef.h"
#include "armasm.h"
#include "err.h"
#include "msg.h"
#include "errlog.h"

#if defined(FEATURE_L4)
#error code not present
#endif /* FEATURE_L4 */
