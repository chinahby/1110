#ifndef SDDBG_H
#define SDDBG_H
/*===========================================================================

                 S Y S T E M   D E T E R M I N A T I O N
 
                    D E B U G   H E A D E R   F I L E

DESCRIPTION
  This header file contains SD debug macros.
   

Copyright (c) 2000 by QUALCOMM, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/sd/rel/08H1/src/sddbg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/04/07   jqi     Lint fixes for barred API -sprintf.
12/02/05   jqi     Lint fixes for Lint ver8.
08/22/01   RC      Change SD_CASE_NOT_IMP() to ERR() when SD_DEBUG is off.
03/27/01   RC      Added debug macros.
11/17/00   RC      Initial release.

===========================================================================*/

#include "comdef.h"    /* Definition for basic types and macros */
#include "target.h"    /* Target configuration file */
#include "err.h"       /* Interface to error log services */
#include "msg.h"       /* Interface to F3 messages services */


/* <EJECT> */
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*---------------------------------------------------------------------------
      Compile SD with debug information, asserts and messages.
---------------------------------------------------------------------------*/

//#define SD_DEBUG




/* <EJECT> */
/*---------------------------------------------------------------------------
                  SD_DEBUG/T_PC independent macros.
---------------------------------------------------------------------------*/

#define SD_MSG_LOW( str, a, b, c )       MSG_LOW( "=SD= " str, a, b, c )
#define SD_MSG_MED( str, a, b, c )       MSG_MED( "=SD= " str, a, b, c )
#define SD_MSG_HIGH( str, a, b, c )      MSG_HIGH( "=SD= " str, a, b, c )
#define SD_MSG_ERROR( str, a, b, c )     MSG_ERROR( "=SD= " str, a, b, c )


/* <EJECT> */
/*---------------------------------------------------------------------------
                   SD_DEBUG/T_PC dependent macros.
---------------------------------------------------------------------------*/

#ifdef SD_DEBUG
#error code not present
#elif( TG==T_PC )
#error code not present
#else
  
  #define SD_ERR_FATAL(str,a,b,c )      ERR_FATAL( "",a,b,c )
  
  #define SD_ERR( str,a,b,c )           ERR( "",a,b,c )
  #define SD_CASE_ERR( str,a,b,c )      ERR( "",a,b,c )
  #define SD_SWITCH_ERR( str,a,b,c )    ERR( "",a,b,c )
  #define SD_ELSE_ERR( str,a,b,c )      ERR( "",a,b,c )
                        
  #define SD_FUNC_START( str,a,b,c )
  #define SD_FUNC_END( str,a,b,c )

  #define SD_FUNC_NOT_IMP( func )       ERR( "",0,0,0 )
  #define SD_CASE_NOT_IMP( _case )      ERR( "",0,0,0 )

#endif




/*---------------------------------------------------------------------------
                  Asserts and verifications macros
---------------------------------------------------------------------------*/

#ifdef SD_DEBUG
#error code not present
#elif( TG==T_PC )
#error code not present
#else
  
  #define SD_ASSERT( exp )

#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#ifdef SD_DEBUG
#error code not present
#else
  #define SD_ASSERT_ENUM_IS_INRANGE( val, max )
#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#ifdef SD_DEBUG
#error code not present
#else
  #define SD_ASSERT_ENUM_IS_INRANGE_INC( val, max )
#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#ifdef SD_DEBUG
#error code not present
#else
  #define SD_ASSERT_ENUM_IS_INRANGE_OR( val, max, equal )
#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* vsnprintf is not part of ANSI C standard, so visual studio defines it 
** as _vsnprintf
*/
#if( TG != T_PC )
  
#define vsnprintf _vsnprintf

#endif

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/



#endif /* SDDBG_H */





