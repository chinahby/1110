/*===========================================================================

                     D S 7 0 7 _ J C D M A _ A P P R A T E
GENERAL DESCRIPTION

 This file contains the API's and functionality to implement the JCDMA
 functionality for an application to set it's data rate.

 Copyright (c) 2002 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_jcdma_apprate.c_v   1.8   12 Dec 2002 17:06:56   akhare  $
  $Header: //source/qcom/qct/modem/data/1x/jcdma/main/lite/src/ds707_jcdma_apprate.c#1 $ $DateTime: 2007/11/05 03:38:38 $ $Author: nsivakum $
  
when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/29/03    ak     Updated to modify snm capabilities record.
11/06/02    ak     Update initialization to LOW speed.
11/04/02    ak     Updated to new SNM interface.
10/22/02    rsl    Updated FEATURE_JCDMA_DS_1X to FEATURE_JCDMA_1X
10/15/02    ak     Updated FEATURE_JCDMA_DS to FEATURE_JCDMA_DS_1X
10/10/02    ak     First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_JCDMA_1X
#include "cai.h"
#include "ds707_jcdma_apprate.h"
#include "ds707_so_pkt.h"
#include "err.h"
#include "mccap.h"
#include "msg.h"
#include "snm.h"


/*===========================================================================
                          EXTERNAL VARIABLES
===========================================================================*/
ds707_jcdma_apprate_type     ds707_jcdma_apprate_val;

/*===========================================================================
                          INTERNAL STRUCTS
===========================================================================*/
snm_sch_mo_list_type  ds707_jcdma_sch_null =
{
  0,
  {
    (word)0,(word)0,(word)0,(word)0,
    (word)0,(word)0,(word)0,(word)0,
    (word)0,(word)0,(word)0,(word)0,
    (word)0,(word)0,(word)0,(word)0
#ifdef FEATURE_IS2000_REL_A
   ,(word)0,(word)0
#endif
  }
};

snm_sch_mo_list_type  ds707_jcdma_sch_1X =
{
#ifdef FEATURE_IS2000_RS2
  2,
#else
  1,
#endif /* RS 2 */
  {
#ifdef FEATURE_IS2000_RS2
    (word)CAI_MUX_1X_RS1,                 /* MuxPDU 1,  1x, single sch     */
    (word)CAI_MUX_1X_RS2,                 /* MuxPDU 2,  1x, single sch     */
    (word)0,(word)0,(word)0,(word)0,
    (word)0,(word)0,(word)0,(word)0,
    (word)0,(word)0,(word)0,(word)0,
    (word)0,(word)0
#else
    (word)CAI_MUX_1X_RS1,                 /* MuxPDU 1,  1x, single sch     */
    (word)0,(word)0,(word)0,(word)0,
    (word)0,(word)0,(word)0,(word)0,
    (word)0,(word)0,(word)0,(word)0,
    (word)0,(word)0,(word)0
#endif /* RS 2 */
#ifdef FEATURE_IS2000_REL_A
   ,(word)0,(word)0
#endif
  }
};

snm_sch_mo_list_type  ds707_jcdma_sch_8X =
{
#ifdef FEATURE_IS2000_RS2
  14,
#else
  7,
#endif /* RS 2 */
  {
#ifdef FEATURE_IS2000_RS2
     (word)CAI_MUX_8X_RS1_DOUBLE          /* MuxPDU 3,  8x, double sch     */
    ,(word)CAI_MUX_8X_RS1_SINGLE          /* MuxPDU 3,  8x, single sch     */
    ,(word)CAI_MUX_4X_RS1_DOUBLE          /* MuxPDU 3,  4x, double sch     */
    ,(word)CAI_MUX_4X_RS1_SINGLE          /* MuxPDU 3,  4x, single sch     */

    ,(word)CAI_MUX_2X_RS1_DOUBLE          /* MuxPDU 3,  2x, double sch     */
    ,(word)CAI_MUX_2X_RS1_SINGLE          /* MuxPDU 3,  2x, single sch     */
    ,(word)CAI_MUX_1X_RS1                 /* MuxPDU 1,  1x, single sch     */

    ,(word)CAI_MUX_8X_RS2_DOUBLE          /* MuxPDU 3, RS2, 8x, double sch */
    ,(word)CAI_MUX_8X_RS2_SINGLE          /* MuxPDU 3, RS2, 8x, sincle sch */
    ,(word)CAI_MUX_4X_RS2_DOUBLE          /* MuxPDU 3, RS2, 4x, double sch */ 
    ,(word)CAI_MUX_4X_RS2_SINGLE          /* MuxPDU 3, RS2, 4x, single sch */

    ,(word)CAI_MUX_2X_RS2_DOUBLE          /* MuxPDU 3, RS2, 2x, double sch */ 
    ,(word)CAI_MUX_2X_RS2_SINGLE          /* MuxPDU 3, RS2, 2x, single sch */
    ,(word)CAI_MUX_1X_RS2                 /* MuxPDU 2, 1x, single sch      */   
                                 
    ,(word)0,(word)0
#else
     (word)CAI_MUX_8X_RS1_DOUBLE          /* MuxPDU 3,  8x, double sch     */
    ,(word)CAI_MUX_8X_RS1_SINGLE          /* MuxPDU 3,  8x, single sch     */
    ,(word)CAI_MUX_4X_RS1_DOUBLE          /* MuxPDU 3,  4x, double sch     */
    ,(word)CAI_MUX_4X_RS1_SINGLE          /* MuxPDU 3,  4x, single sch     */

    ,(word)CAI_MUX_2X_RS1_DOUBLE          /* MuxPDU 3,  2x, double sch     */
    ,(word)CAI_MUX_2X_RS1_SINGLE          /* MuxPDU 3,  2x, single sch     */
    ,(word)CAI_MUX_1X_RS1                 /* MuxPDU 1,  1x, single sch     */

    ,(word)0,(word)0,(word)0,(word)0
    ,(word)0,(word)0,(word)0,(word)0
    ,(word)0
#endif /* RS 2 */
#ifdef FEATURE_IS2000_REL_A
    ,(word)0,(word)0
#endif
  }
};

snm_sch_mo_list_type  ds707_jcdma_sch_16X =
{
#ifdef FEATURE_IS2000_RS2
  #ifdef FEATURE_DS_IS2000_RS2_16X_F_SCH
  16,
  #endif
  15,
#else
  8,
#endif /* RS 2 */
  {
#ifdef FEATURE_IS2000_RS2
    (word)CAI_MUX_16X_RS1_DOUBLE          /* MuxPDU 3, 16x, double sch     */
   ,(word)CAI_MUX_8X_RS1_DOUBLE          /* MuxPDU 3,  8x, double sch      */
   ,(word)CAI_MUX_8X_RS1_SINGLE          /* MuxPDU 3,  8x, single sch      */
   ,(word)CAI_MUX_4X_RS1_DOUBLE          /* MuxPDU 3,  4x, double sch      */

   ,(word)CAI_MUX_4X_RS1_SINGLE          /* MuxPDU 3,  4x, single sch      */
   ,(word)CAI_MUX_2X_RS1_DOUBLE          /* MuxPDU 3,  2x, double sch      */
   ,(word)CAI_MUX_2X_RS1_SINGLE          /* MuxPDU 3,  2x, single sch      */
   ,(word)CAI_MUX_1X_RS1                 /* MuxPDU 1,  1x, single sch      */

#ifdef FEATURE_DS_IS2000_RS2_16X_F_SCH
   ,(word)CAI_MUX_16X_RS2_DOUBLE         /* MuxPDU 3, RS2, 16x,double sch  */
#endif
   ,(word)CAI_MUX_8X_RS2_DOUBLE          /* MuxPDU 3, RS2, 8x, double sch  */
   ,(word)CAI_MUX_8X_RS2_SINGLE          /* MuxPDU 3, RS2, 8x, sincle sch  */
   ,(word)CAI_MUX_4X_RS2_DOUBLE          /* MuxPDU 3, RS2, 4x, double sch  */ 

   ,(word)CAI_MUX_4X_RS2_SINGLE          /* MuxPDU 3, RS2, 4x, single sch  */
   ,(word)CAI_MUX_2X_RS2_DOUBLE          /* MuxPDU 3, RS2, 2x, double sch  */ 
   ,(word)CAI_MUX_2X_RS2_SINGLE          /* MuxPDU 3, RS2, 2x, single sch  */
   ,(word)CAI_MUX_1X_RS2                 /* MuxPDU 2, 1x, single sch       */   
#ifndef FEATURE_DS_IS2000_RS2_16X_F_SCH
   ,(word)0
#endif /* !RS 2, 16x */
#else  /* RS 2 */
    (word)CAI_MUX_16X_RS1_DOUBLE          /* MuxPDU 3, 16x, double sch     */
   ,(word)CAI_MUX_8X_RS1_DOUBLE          /* MuxPDU 3,  8x, double sch      */
   ,(word)CAI_MUX_8X_RS1_SINGLE          /* MuxPDU 3,  8x, single sch      */
   ,(word)CAI_MUX_4X_RS1_DOUBLE          /* MuxPDU 3,  4x, double sch      */

   ,(word)CAI_MUX_4X_RS1_SINGLE          /* MuxPDU 3,  4x, single sch      */
   ,(word)CAI_MUX_2X_RS1_DOUBLE          /* MuxPDU 3,  2x, double sch      */
   ,(word)CAI_MUX_2X_RS1_SINGLE          /* MuxPDU 3,  2x, single sch      */
   ,(word)CAI_MUX_1X_RS1                 /* MuxPDU 1,  1x, single sch      */

   ,(word)0,(word)0,(word)0,(word)0
   ,(word)0,(word)0,(word)0,(word)0
#endif /* RS 2 */
#ifdef FEATURE_IS2000_REL_A
    ,(word)0,(word)0
#endif
  }
};

/*===========================================================================
                      EXTERNAL FUNCTION DEFINITIONS
==========================================================================*/
/*===========================================================================
FUNCTION      DS707_JCDMA_APPRATE_INIT

DESCRIPTION   Called once, at phone startup.  Initializes the app rate to
              the default (HIGH).

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_jcdma_apprate_init(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds707_jcdma_apprate_val = DS707_JCDMA_APPRATE_HIGH;
  ds707_jcdma_apprate_set_rate(DS707_JCDMA_APPRATE_HIGH);
}

/*===========================================================================
FUNCTION      DS707_JCDMA_APPRATE_SET_RATE

DESCRIPTION   Called by application to set its data rate.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_jcdma_apprate_set_rate
(
  ds707_jcdma_apprate_type     rate
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ((rate <= DS707_JCDMA_APPRATE_MIN) ||
      (rate >= DS707_JCDMA_APPRATE_MAX)
     )
  {
    ERR("invalid application rate",0,0,0);
    return;
  }

  switch(rate)
  {
    case DS707_JCDMA_APPRATE_LOW:
      ds707_jcdma_apprate_val = rate;
      mccap_set_sch_sys_cap(
                             FALSE,
                             FALSE,
                             CAI_SCH_RATE_NULL,
                             CAI_SCH_RATE_NULL
                           );
      snm_set_sch_mux_list(CAI_SO_PPP_PKT_DATA_3G,
                           SNM_FORWARD,
                           &ds707_jcdma_sch_null);
      snm_set_sch_mux_list(CAI_SO_PPP_PKT_DATA_3G,
                           SNM_REVERSE,
                           &ds707_jcdma_sch_null);
      break;

    case DS707_JCDMA_APPRATE_MED:
      ds707_jcdma_apprate_val = rate;
      mccap_set_sch_sys_cap(
                             TRUE,
                             TRUE,
                             CAI_SCH_RATE_RS1_16X_RS2_16X,
                             CAI_SCH_RATE_RS1_1X_RS2_1X
                           );
      snm_set_sch_mux_list(CAI_SO_PPP_PKT_DATA_3G,
                           SNM_FORWARD,
                           &ds707_jcdma_sch_16X);
      snm_set_sch_mux_list(CAI_SO_PPP_PKT_DATA_3G,
                           SNM_REVERSE,
                           &ds707_jcdma_sch_1X);
      break;

    case DS707_JCDMA_APPRATE_HIGH:
      ds707_jcdma_apprate_val = rate;
      mccap_set_sch_sys_cap(
                             TRUE,
                             TRUE,
                             CAI_SCH_RATE_RS1_16X_RS2_16X,
                             CAI_SCH_RATE_RS1_8X_RS2_8X
                           );
      snm_set_sch_mux_list(CAI_SO_PPP_PKT_DATA_3G,
                           SNM_FORWARD,
                           &ds707_jcdma_sch_16X);
      snm_set_sch_mux_list(CAI_SO_PPP_PKT_DATA_3G,
                           SNM_REVERSE,
                           &ds707_jcdma_sch_8X);
      break;

    default:
      ERR("Unknown app rate %d", rate, 0, 0);
      break;
  }

  /*-------------------------------------------------------------------------
    Need to enable/disable various SO's based on new setting
  -------------------------------------------------------------------------*/
  ds707_so_pkt_recal();

  MSG_HIGH("New app rate %d", rate, 0, 0);
}

#endif /* FEATURE_JCDMA_1X */

