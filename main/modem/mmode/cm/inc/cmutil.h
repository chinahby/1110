#ifndef CMUTIL_H
#define CMUTIL_H
/*===========================================================================

         C A L L   M A N A G E R   U T I L I T Y   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with CMUTIL.C


Copyright (c) 2001 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1_QSC1110_3.X/inc/cmutil.h#1 $

when       who     what, where, why
--------   ---    ----------------------------------------------------------
06/19/09   rm      Adding new feature defintions to have consistent value in 
                   the RPC files. Added cmutil_get_emergency_num_count
03/31/08   rk      Moving MAX_DISPLAY_TEXT_LEN macro definition to cm.h
01/28/09   np      Added HPCD Rev C. Japan IDD change
01/20/08   fj      Move APIs to cm.h
09/15/08   np      Added HPCD Rev B.  
03/17/08   ka      Fixing plus to digit conversion when converted num
                   results in dtmf for SMS.
01/04/08   ka      Adding handset based plus code dial feature
01/09/07   ka      Changing 0xFFFFFFFF to 0x7FFFFFFF to satisfy C++ compilers
11/23/05   ka      Giving SIP error access to UI clients.
07/25/05   sk      Added constant CHAR_CR.
07/08/05   ic      Moved several function declarations to cm.h
08/18/04   ic      Constants used by cm_util_set_default_voice_bearer_cap()
                   now defined under FEATURE_GSM || FEATURE_WCDMA
08/11/04   ka      Utility function for reading bearer capability structure
07/27/04   ws      Add functions to default Orig parameters.
02/06/04   ws      Initial jaguar/mainline merge.
01/21/03   ws      Updated copyright information for 2003
09/11/02   RI      Added prototype for cm_util_subaddr_to_ascii().
03/25/02   HD      Added macro for ISDIGIT.
02/08/02   PRK     Changed the cm_util_sys_id_match function prototype.
01/23/02   PRK     Replaced cmutil prefix with cm_util prefix.
                   Added sys_id utility functions.
11/21/01   RI      Modified function declaration of
                   cmutil_phone_ascii_to_phone_bcd().
09/13/01   RI      Initial Release
===========================================================================*/
#include "comdef.h"
#include "cm.h"       /* Needed for cm_sys_id_s_type */


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#define LCD_COLUMNS       12
#define CHAR_CR           0x0D

#define BCD_LEN    0
#define BCD_NT_NPI 1
#define BCD_NUM    2
#define BCD_STAR                   0x0A
#define BCD_HASH                   0x0B

#define ISDIGIT(X) ((X >= '0') && (X <= '9'))

#if defined(FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#endif /* #if defined(FEATURE_WCDMA) || defined (FEATURE_GSM) */

#define LTM_INVALID 127
  /* Invalid LTM. Giving it a high enough value */

#define  NETWORK_SOLUTION_ABSENT 0
  /* Network solution for sid range not present */

#define  NETWORK_SOLUTION_PRESENT 1
  /* Network solution for sid range is present */

#define  DRR_STRING             "*77"
  /* Starting prefix for Direct routing
  ** roamer to roamer number
  */

#define  CMUTIL_PLUS_REPL_DIGIT_LEN 7
  /* Len of digits that can replace plus */

#define CMUTIL_COUNTRY_CODE_LEN  7
  /* Should be maximum size country_code can potentially have */

/* Constants for origination message translation, From 1x CP*/
#define CMUTIL_DTMF_FOR_DIAL_0    0xA
#define CMUTIL_DTMF_FOR_DIAL_STAR 0xB
#define CMUTIL_DTMF_FOR_DIAL_PND  0xC
#define CMUTIL_DTMF_FOR_DIAL_NULL 0x0

 
#define CMUTIL_MCC_LOOP_LEN 30
  /*  The length of MCC loop */

/*===========================================================================

FUNCTION: cmutil_get_emergency_num_count

DESCRIPTION
  Returns the maximum number of emergency calls
  Based on the feature GSM or WCDMA, this value will differ

DEPENDENCIES
  None.

RETURN VALUE
  Initialized parameters

SIDE EFFECTS
  None
===========================================================================*/
extern int cmutil_get_emergency_num_count( void );



#endif

