#ifndef _QCPLAYER_MISC_H_
#define _QCPLAYER_MISC_H_
/* =======================================================================
                               qcplayer_misc.h
DESCRIPTION
//    This is the include file for some miscellanous
//    routines that are used by the CoreLibs but are
//    not directly available when building for zrex/psos.
  
Portions copyrighted by PacketVideo Corporation; 
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/qc/main/latest/inc/qcplayer_misc.h#3 $
$DateTime: 2009/12/03 02:35:48 $
$Change: 1101863 $


========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
//#include "rex.h"
/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define QTV_MAX_USER_AGENT_BYTES         128
#define QTV_MAX_USER_AGENT_MAN_BYTES     3
#define QTV_MAX_USER_AGENT_CTN_BYTES     14 
#define QTV_MAX_USER_AGENT_VCT_BYTES     3
#define QTV_MAX_USER_AGENT_ACT_BYTES     3
#define QTV_MAX_USER_AGENT_SCT_BYTES     3
#define QTV_MAX_USER_AGENT_CLR_BYTES     3
#define QTV_MAX_USER_AGENT_LCD_BYTES     2
#define QTV_MAX_USER_AGENT_VER_BYTES     5

#define QTV_MAX_USER_AGENT_MIN_BYTES     12
#define QTV_MAX_USER_AGENT_STR_BYTES     200


#include "zrex_os.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
//////////////////////////////////////////////////////////////////////////////////
//
// Windows Critical Section calls
//
//////////////////////////////////////////////////////////////////////////////////
*/

  /* Use REX critical sections */

typedef rex_crit_sect_type CRITICAL_SECTION;

#define InitializeCriticalSection(x) rex_init_crit_sect(x)
#define EnterCriticalSection(x) rex_enter_crit_sect(x)
#define LeaveCriticalSection(x) rex_leave_crit_sect(x)
/* ------
Commented out, since this code goes to all targets as common and 
below REX API is not currently ported to all active targets and baselines.
--------- */
#define DeleteCriticalSection(x) //rex_del_crit_sect(x)



/*
//////////////////////////////////////////////////////////////////////////////////
//
// Windows Sleep call
//
//////////////////////////////////////////////////////////////////////////////////
*/



  /* Use zrex sleep */

  #define Sleep(x) zrex_sleep(x)



#ifdef __cplusplus
}
#endif


/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */

/* =======================================================================
MACRO MYOBJ

ARGS 
  xx_obj - this is the xx argument

DESCRIPTION:
  Complete description of what this macro does
========================================================================== */

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/* ======================================================================
FUNCTION 
  SAMPLE_FUNC

DESCRIPTION
  Thorough, meaningful description of what this function does. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */

#endif /* _QCPLAYER_MISC_H_ */
