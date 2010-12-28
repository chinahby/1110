#ifndef RFIFREQ_H
#define RFIFREQ_H

/* EJECT  */
/*===========================================================================

   R F   F r e q u e n c y   D e f i n i t i o n s   H e a d e r   F i l e

DESCRIPTION
  This header file defines relationship between channels and frequency for
  FM, CDMA, PCS, Korean PCS.  This header file is internal to the RF unit.

Copyright (c) 1997, 1998, 1999 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2000             by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rfifreq.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/25/07   jfc     Include high level rficap.h instead of rficap_mm_cdma1x.h
12/15/06   jfc     Moved rf_card_band_type to rfcom.h
10/31/06   ycl     Include the correct rficap file.
10/18/06   ycl     Added BC15 for initial MSM7600 build.
11/19/05   rmd     Removed "," from RF_MAX_BAND definition.
10/18/05   rmd     Updated code according to code review recommendations.
09/19/05   rmd     Added support for band class 14 (US PCS + 5MHz).
07/01/05   sar     Mainlined feature RF_HAS_ENHANCED_BAND_CLASS.
05/11/05   sar     Eliminated all the references to CAI_BAND_CLASS13, and 
                   redefinitions of CAI_BAND_CLASSxx.
08/23/04   sar     Replacing references to FEATURE_ENHANCED_BAND_CLASS with
                   RF_HAS_ENHANCED_BAND_CLASS.
04/14/04   sar     Added support for Enhanced Bands.
04/09/04   jac     Added RF_KPCS_CHAN_SPN definition
03/26/04   dbc     Added RF_450_CDMA to rf_band_type
02/06/04   dbc     Changes for ZIF library architecture update
09/05/03   dbc     Added RF_IMT to rf_band_type enumeration
10/25/02   dbc     Defined RF_800_BAND enum value to be the same as  
                   CAI_BAND_CELL.  Defined RF_PCS_BAND enum to be the same as
                   CAI_BAND_PCS. Added include of "cai.h".
10/18/02   dbc     Added prototype for rf_set_pcs_band().  Removed channel 
                   min/max hard coded defines for all bands.  Added
                   rft_min_max_chan_type. 
09/10/02   et      changed PCS min channel from 1200 to 1199
                   added min and max channel values for JCDMA and KPCS
09/04/02   tn      removed unnecessary and unused definitions
08/26/02   dbc     Added enum rf_band_type and rft_band_sel_chan_config_type
                   structure.  Added rf_set_band_sel_config() and 
                   rf_set_pcs_band() prototypes. 
04/12/00   dd      Added GPS_BAND enumeration.
12/15/99   rv      Merge with latest PP (TD1017) code.
12/09/99   ymc     Renamed rf_cdma_band_type as rf_card_band_type;
                   Renamed RF_CDMA_BAND_800MHZ as RF_800_BAND;
                   Renamed RF_CDMA_BAND_1900MHZ as RF_PCS_BAND;
10/26/99   ymc     Cleaned up and modified to support BSP.
01/06/99   ychan   Modified for targets NGP, SURF, Q-1900.
01/06/99   ychan   Updated Copyright date.
02/10/98   thh     Added channel scheme for Korean PCS.
                   Removed frequency definitions dependency on supported RF
                   mode.
11/17/97   jjn     Partitioned ISS2 RF to conform to the new RF architecture
09/04/97   thh     Initial version.

===========================================================================*/
#include "rficap.h"
#include "comdef.h" 
#include "cai.h"
#include "rfcom.h"

/* EJECT  */
/*===========================================================================

                          PUBLIC DEFINITIONS

===========================================================================*/

/* ==================== US Cellular Frequency Spectrum ==============
** This table shows how FM/CDMA channel #, Tx frequency, Rx frequency,
** UHF synthesizer frequency are related in U.S. spectrum:
**
**  BAND   CHANNEL   CHANNEL     Tx FREQ    Rx FREQ    UHF FREQ
**  NAME   NUMBER    OFFSET       (kHz)      (kHz)      (kHz)
**  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
**   ?       990     unused      824,010    869,010    954,390
**  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
**   A"      991         0       824,040    869,040    954,420
**   A"     1023 (0)    32       825,000    870,000    955,380
**  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
**   A         1        33       825,030    870,030    955,410
**   A       333       365       834,990    879,990    965,370
**  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
**   B       334       366       835,020    880,020    965,400
**   B       666       698       844,980    889,980    975,360
**  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
**   A'      667       699       845,010    890,010    975,390
**   A'      716       748       846,480    891,480    976,860
**  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
**   B'      717       749       846,510    891,510    976,890
**   B'      799       831       848,970    893,970    979,350
**  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#define RF_800MHZ_CHAN_SPN        ((word)832)  /* channel span (total channels)   */


/* ==================== US PCS Frequency Spectrum =================
** This table shows how PCS channel #, Tx frequency, Rx frequency,
** UHF synthesizer frequency are related in U.S. spectrum:
**
**  BAND   CHANNEL   Tx FREQ     Rx FREQ    UHF FREQ
**  NAME   NUMBER     (kHz)       (kHz)       (kHz)
**  - - - - - - - - - - - - - - - - - - - - - - - - -
**   A         0    1,930,000   1,850,000   1,719,620
**   A       299    1,944,950   1,864,950   1,734,570
**  - - - - - - - - - - - - - - - - - - - - - - - - -
**   D       300    1,945,000   1,865,000   1,734,620
**   D       399    1,949,950   1,869,950   1,739,570
**  - - - - - - - - - - - - - - - - - - - - - - - - -
**   B       400    1,950,000   1,870,000   1,739,620
**   B       699    1,964,950   1,884,950   1,754,570
**  - - - - - - - - - - - - - - - - - - - - - - - - -
**  HIGH/LOW BAND BOUNDARY
**  - - - - - - - - - - - - - - - - - - - - - - - - -
**   E       700    1,965,000   1,885,000   1,754,620
**   E       799    1,969,950   1,889,950   1,759,570
**  - - - - - - - - - - - - - - - - - - - - - - - - -
**   F       800    1,970,000   1,890,000   1,759,620
**   F       899    1,974,950   1,894,950   1,764,570
**  - - - - - - - - - - - - - - - - - - - - - - - - -
**   C       900    1,975,000   1,895,000   1,764,620
**   C      1199    1,989,950   1,909,950   1,779,570
**  - - - - - - - - - - - - - - - - - - - - - - - - -
**   ?      1200    1,990,000   1,910,000   1,779,620
**  - - - - - - - - - - - - - - - - - - - - - - - - - */
#define RF_PCS_CHAN_SPN           ((word)1201)    /* channel span (total channels)   */


#define RF_KPCS_CHAN_SPN          ((word)600)    /* channel span (total channels)   */

/* ================ BAND TYPE DEFINITIONS ================== */

/* ------------------
** RF band (CDMA/PCS)
** ------------------ */
#define CAI_BAND_CELL RF_BC0_BAND
#define CAI_BAND_PCS RF_BC1_BAND
#define RF_800_BAND RF_BC0_BAND
#define RF_PCS_BAND RF_BC1_BAND


/* -------------------------------------
** Channel configuration for A/B switch
** ------------------------------------- */
typedef struct
{
  word a_band_min_chan;
  word a_band_max_chan;
  word b_band_min_chan;
  word b_band_max_chan;
}
rflib_band_sel_chan_config_type;


/* -------------------------------------
** Structure to hold channel range
** ------------------------------------- */
typedef struct
{
  word min_chan;
  word max_chan;
}
rflib_min_max_chan_type;


/*=========================================================================

FUNCTION rf_set_band_sel_config

DESCRIPTION
  This function populates the A/B channel configuration that determines 
  the switch points for the specified band. Currently supported
  bands are RF_US_PCS and RF_KPCS

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rf_set_band_sel_config( rf_card_band_type band, word a_chan_min, word a_chan_max, 
                              word b_chan_min, word b_chan_max);


/*=========================================================================

FUNCTION rf_set_pcs_band

DESCRIPTION
  This function sets the current PCS band configuration.  Currently supported
  bands are RF_US_PCS and RF_KPCS

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rf_set_pcs_band( rf_card_band_type );

/*=========================================================================

FUNCTION rf_set_cdma_band

DESCRIPTION
  This function sets the current CDMA band configuration.  Currently supported
  bands are RF_US_CDMA and RF_JCDMA

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rf_set_cdma_band( rf_card_band_type );

#endif  /* RFIFREQ_H */
