#ifndef CMIDBG_H
#define CMIDBG_H
/*===========================================================================

 C A L L   M A N A G E R   I N T E R N A L D E B U G   H E A D E R   F I L E

DESCRIPTION
  This header file contains debug macros and definitions necessary to
  interface with CMDBG.C

  It is different from cmdbg.h in the way that has no dependency on other cm 
  header files.



Copyright (c) 2005 - 2006 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmidbg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/20/08   ak      Moved cmtest_test_fail_inc and BAD_CHOICE from cmidbg.h
01/24/08   ak      Transferred CM_VERIFY and CM_HELPER_VERIFY to cmtest.h
01/15/07   ak      Asserts removed from verifications
08/07/07   ic      Lint clean up
04/10/06   pk      Lint fixes
03/15/06   sk      Deleted CMDBG_CMD_MSG. Already defined in cmdbg.h.
01/06/06   ic      Lint cleanup  
12/11/05   ic      Lint cleanup
11/18/05   jqi     Initial release.


===========================================================================*/


#include "comdef.h"    /* Definition for basic types and macros */
#include "target.h"    /* Target configuration file */
#include "err.h"       /* Interface to error log services */
#include "msg.h"       /* Interface to F3 messages services */

/*lint -save -e656 -e641 
** Turn off enum to int / operationuses compatible enums
*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* For use in cmdbg.c only! */

#ifdef CMDEBUG_STRING_MAPS_ACTIVE
  #define CMDBG_RPT_MSG(rpttype, cmd, offset, strmap, num1)\
          cmdbg_print_message( "CM<< " rpttype " %d (%s): %d", cmd, strmap [ cmd - (offset - 1)], num1)
#else
  #define CMDBG_RPT_MSG(rpttype, cmd, offset, strmap, num1)\
          CM_MSG_HIGH( "CM<< " rpttype " %d: %d", cmd, num1, 0)
#endif


/* For use in cmdbg.c only! */
#ifdef CMDEBUG_STRING_MAPS_ACTIVE
  #define CMDBG_CMD_MSG(cmdtype, cmd, strmap, client_id)\
          cmdbg_print_message( ">>CM " cmdtype " %d (%s): %d", cmd,\
                               strmap [ cmd ],\
                               cmclient_get_client_type(client_id))
#else
  #define CMDBG_CMD_MSG(cmdtype, cmd, strmap, client_id)\
          CM_MSG_HIGH( ">>CM " cmdtype " %d: %d", cmd,\
                       cmclient_get_client_type(client_id), 0)
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Debug/PC independent macros.
*/

#define CM_MSG_LOW( str, a, b, c )       MSG_LOW( "=CM= " str, a, b, c )
#define CM_MSG_MED( str, a, b, c )       MSG_MED( "=CM= " str, a, b, c )
#define CM_MSG_HIGH( str, a, b, c )      MSG_HIGH( "=CM= " str, a, b, c )
#define CM_ERR_FATAL( str, a, b, c ) /*lint -e571*/ ERR_FATAL( "=CM= " str, a, b, c ) /*lint +e571*/

#define CM_FUNC_NOT_IMP( func ) \
    CM_MSG_HIGH( #func " NOT Implemented!",0,0,0 )

#define CM_CASE_NOT_IMP( _case ) \
    CM_ERR( #_case " case NOT Implemented",0,0,0 )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Debug dependent macros.
*/

#ifdef CM_DEBUG
#error code not present
#else

  #define CM_ERR( str,a,b,c )            ERR( "=CM= " str,a,b,c )
  #define CM_MSG_ERROR( str,a,b,c )      MSG_ERROR( "=CM= " str,a,b,c )
  #define CM_DBG_ERR( str,a,b,c )
  #define CM_CASE_ERR( str,a,b,c )       ERR( "=CM= " str,a,b,c )
  #define CM_SWITCH_ERR( str,a,b,c ) /*lint -e571*/ ERR_FATAL( "=CM= " str,a,b,c ) /*lint +e571*/
  #define CM_ELSE_ERR( str ) \
               else { MSG_MED( "=CM= " str " else make no sense",0,0,0 ); }

#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* PC dependent macros.
*/

#ifdef T_PC

  #define CM_FUNC_START( str,a,b,c ) CM_MSG_LOW( "START " str,a,b,c )
  #define CM_FUNC_END( str,a,b,c )   CM_MSG_LOW( "END " str,a,b,c )

#else

  #define CM_FUNC_START( str,a,b,c )
  #define CM_FUNC_END( str,a,b,c )

#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Debug and PC dependent macros.
*/

#ifdef CM_DEBUG
#error code not present
#else

  #define CM_DBG_ERR_FATAL( str,a,b,c )

#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Asserts and verifications macros.
*/

/* 
** 506: Constant value Boolean
** 774: Boolean within 'if' always evaluates to True
** This takes care of things like CM_ASSERT(FALSE)
*/ 
#ifdef CM_DEBUG
#error code not present
#else
  #define CM_ASSERT( exp )
#endif


#ifdef CM_DEBUG
#error code not present
#else
  #define CM_ASSERT_ERR( exp )                         \
    if( !(exp) )                                       \
    {                                                  \
      ERR( "=CM= ASSERTION FAILED! " #exp ,0,0,0);      \
    }
#endif


#define CM_NONE_DBG_ASSERT( exp )                      \
    if( !(exp) )                                       \
    {                                                  \
      ERR_FATAL( "ASSERTION FAILED! " #exp ,0,0,0 );    \
    }


#ifdef CM_DEBUG
#error code not present
#else
  #define CM_ASSERT_ENUM_IS_INRANGE( val, max )
#endif


#ifdef CM_DEBUG
#error code not present
#else
  #define CM_ASSERT_ENUM_IS_INRANGE_INC( val, max )
#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#ifdef CM_DEBUG
#error code not present
#else
  #define CM_ASSERT_ENUM_IS_INRANGE_OR( val, max, equal )
#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enum print macros.
*/
#ifdef CM_DEBUG
#error code not present
#else
  #define CMDBG_PRT_FUNC( prt_func, enum_val )
#endif

/*lint -restore */          
#endif /* CMIDBG_H */

