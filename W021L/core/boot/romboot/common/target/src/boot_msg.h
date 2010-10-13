#ifndef BOOT_MSG_H
#define BOOT_MSG_H

/*===========================================================================

                         B o o t   L o a d e r
                     M o d u l e  I n t e r f a c e
                          H e a d e r  F i l e

DESCRIPTION
  This header file contains the definition of the public interface to
  the boot DLOAD module.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/romboot/shared/boot_msg.h#1 $ 
$DateTime: 2008/04/30 12:43:19 $ 
$Author: psanghvi $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/31/07   anb     Initial revision.

============================================================================*/

/* Flash tests do not use the MSG_* macros, or INTLOCK and INTFREE 
 * so undefine them */
#undef  DMOV_MSG_HIGH
#undef  DMOV_MSG_MED
#undef  DMOV_MSG_LOW
#undef  DMOV_MSG_ERROR
#undef  DMOV_MSG_FATAL
#define DMOV_MSG_HIGH(x,a,b,c) /**/
#define DMOV_MSG_MED(x,a,b,c) /**/
#define DMOV_MSG_LOW(x,a,b,c) /**/
#define DMOV_MSG_ERROR(x,a,b,c) /**/
#define DMOV_MSG_FATAL(x,a,b,c) /**/

/* Include appropriate header depending on the client we are building with */
#if (defined(BOOT_LOADER) || defined(FEATURE_WINCE_BOOTLOADER) || defined(BUILD_BOOT_CHAIN))
  #define TPRINTF(level, x) /**/
  #define DPRINTF(x)        /**/

  /* There are supposed to be defined in boot code. Should eventually be
   * be pulled from one of the headers in boot code
   */
  #undef MSG_HIGH
  #undef MSG_MED
  #undef MSG_LOW
  #undef MSG_ERROR
  #undef MSG_FATAL
  #undef ERR
  #undef ERR_FATAL  
  #undef ASSERT
  #undef ERR_H    

  #define MSG_HIGH(x,a,b,c)   /**/
  #define MSG_MED(x,a,b,c)    /**/
  #define MSG_LOW(x,a,b,c)    /**/
  #define MSG_ERROR(x,a,b,c)  /**/
  #define MSG_FATAL(x,a,b,c)  /**/
  #define ERR(x,a,b,c)        /**/
  #define ERR_H
  
  /* Following should be redefined in indiviual boot loaders which want to 
   * enable following defines to be used.
   */  
  #define ERR_FATAL(x,a,b,c)  
  #define ASSERT   2

#elif defined(FLASH_MSG_H)
  #include FLASH_MSG_H
#else
  #include "msg.h"
  #define TPRINTF(level, x) /**/
  //#define DPRINTF(x) MSG_ERROR(x,0,0,0)
  #define DPRINTF(x)    /**/ 
#endif
             
//#undef  INTLOCK
//#undef  INTFREE
//#define INTLOCK() /**/
//#define INTFREE() /**/

#endif /*BOOT_MSG_H*/

