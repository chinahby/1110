#ifndef ADIE_REGDEFS_H
#define ADIE_REGDEFS_H
/*===========================================================================


   A N A L O G   D I E   R E G I S T E R   D E F S   H E A D E R    F I L E

DESCRIPTION
  This file contains all the register and bit definitions for the analog die.
  these definitions are used by the analog die c file and header file.  The
  definitions are the same for both targets that use memory mapped 
  communication and SBI communication.

Copyright (c) 1998 - 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/
 
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/rf/adie_regdefs.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/22/06   hkk     Common Adie driver for 1x and UMTS 
04/07/06   gfr     Added missing include file (msmshift.h)
02/05/04    et     defined TEST_ADIE_DATA_SLEEP_BBR_M and _V instead of the 
                   CODEC_M and _V for TEST_ADIE_DATA reg.
01/20/04    et     added new MASK and VALUE values for TEST_ADIE_DATA
                   and TEST_ADIE_DATA_SEL MSM registers for MSM6500
01/12/04    rv     Added more defns for BBRX1 power down/wake up states. 
12/02/02   dlb     Fix error in HKADC_SETUP bit 2 definitions.
11/26/02   dyc     Add GP_RESERVED_M definition.
11/01/02   aks     Added RF_HAS_MULTIMODE_BBRX support.
09/26/02   aks     Define a more proper mask for HKADC config.
09/06/02    sg     Updated CODEC and HKADC definitions.
12/05/01   rmd     moved ADC defs 
11/12/01    et     moved ADIE_POWER_UP/DOWN defs from msm6050reg.h
10/25/01    et     cleaned up file and added composite definitions
10/19/01   aks     Removed bbrx register definitions.
10/12/01    et     added a few masks and values for switching modes
09/05/01    et     created

===========================================================================*/
#include "rficap.h"
#include "adie.h"
#include "msmshift.h"

#ifdef T_MSM6280
#error code not present
#endif
/* ADC read conversion time( us) =  12 * 1/(TCXO/4/n) where
   TCXO = 19.2 MHz and n= 2/4/8/16 
  Padding : 10us  */
  
#define HKADC_CONVERSION_TIME_TCXO_BY4_BY_2     5+10
#define HKADC_CONVERSION_TIME_TCXO_BY4_BY_4    10+10
#define HKADC_CONVERSION_TIME_TCXO_BY4_BY_8    20+10
#define HKADC_CONVERSION_TIME_TCXO_BY4_BY_16   40+10

#define ADIE_POWER_UP_V                                              0x0001
#define ADIE_POWER_DOWN_V                                            0x0000   

//lesp#include "adie_cdcdefs.h"

#ifdef T_MSM6500

/* allows software to control the test interface to Pluto sleepb_bbr signal.*/
#define TEST_ADIE_DATA_SEL_SLEEPB_BBR_SEL_M  0x02
#define TEST_ADIE_DATA_SEL_SLEEPB_BBR_SEL_V  0x02

/* TEST_ADIE_DATA */
/* This is an MSM reg, not an ADIE sbi reg */

/* This bit is the bonding test data input to the analog die */
#define TEST_ADIE_DATA_SLEEP_BBR_M     0x10
#define TEST_ADIE_DATA_SLEEP_BBR_V     0x10

#endif /* T_MSM6500 */


#ifdef FEATURE_RF_TX_PWR_LIM_REV
/********************************************************************/
/* Value used by MDSP to access HKADC, more details can be found in */
/* in 80-V7864-5 MDSP_SBI_HWREQ register                            */
/********************************************************************/
#define ADIE_HKADC_SBIDATA_VALUE    (0x08653040|((ADC_HDET_HKAIN)<<1))
#endif

#endif /* ADIE_REGDEFS_H */



