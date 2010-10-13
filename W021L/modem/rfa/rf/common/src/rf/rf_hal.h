#ifndef RF_HAL_H
#define RF_HAL_H

/*=============================================================================

                R F _ H A L   H E A D E R   F I L E 
       
DESCRIPTION
  This file contains declarations and definitions to support a Hardware
  Abstraction Layer.

Copyright (c) 2006, 2007   by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/rf/rf_hal.h#1 $ $DateTime: 2008/04/11 05:11:57 $ $Author: ppottier $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/15/07   ems     Support for QSC1100
08/13/07   vm      More updates for Sc2x
02/27/07   dyc     Added QSC60X5 support
11/16/06   jfc     Added RFH_SHDW() macro
11/15/06   jfc     Renamed RF_TODO_7600 with RF_MSM7600_HWIO
11/14/06   adk     Added macros HWIO_CDMA_SHADOW_* for shadow registers.
10/31/06   ycl     Added HWIO_SAMPSRV_TEST_RAM_DATA_ADDR for ftm files.
10/31/06   ycl     Delete macros in msm_help.h.
10/26/06   jfc     Initial revision

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "msmhwio.h"

/*===========================================================================

                         GLOBAL DEFINIATIONS and VARIABLES

===========================================================================*/

/* Ideally RFH_SHDWI() should be mapped to HWIO_SHDWI() but the msmhwioreg.h 
   file does not contain the shadow register names for RXF registers
*/

#define RFH_SHDW(hwiosym)                    __rfh_shdw(hwiosym)
#define RFH_SHDWI(hwiosym, index)            __rfh_shdwi(hwiosym, index)
#define RFH_SHDWI2(hwiosym, index1, index2)  __rfh_shdwi2(hwiosym, index1, index2)

#define __rfh_shdw(hwiosym)                    HWIO_##hwiosym##_shadow
#define __rfh_shdwi(hwiosym, index)            HWIO_##hwiosym##_shadow[index]
#define __rfh_shdwi2(hwiosym, index1, index2)  HWIO_##hwiosym##_shadow[index1][index2]

#if defined( RF_MSM7600_HWIO )
#error code not present
#else
#define RXF_UMTS(hwiosym)  hwiosym
#define RXF_CDMA(hwiosym)  hwiosym
#define RXF_MODEM(hwiosym) hwiosym
#define RFH(hwiosym)       hwiosym
#endif /* RF_MSM7600_HWIO */

//RF_TODO_7600_ADK: Macro not used
//#define RF_HWIO_SHDW_IN(hwiosym_shadow, index, field) ( HWIO_SHDWI(hwiosym_shadow, index) & HWIO_FMSK(field)) >> HWIO_SHFT(field));

#if defined( T_QSC60X5 ) && !defined(T_QSC1100)

/* T_QSC60X5 RXF HAL */
#define RXFc_FG_IOFFSET         RXF_SRCc_FG_IOFFSET
#define RXFc_FG_QOFFSET         RXF_SRCc_FG_QOFFSET
#define DACCc_EST_IOFFSET       RXF_SRCc_DACC_EST_IOFFSET
#define DACCc_EST_QOFFSET       RXF_SRCc_DACC_EST_QOFFSET
#define RXFc_DACC_CTL           RXF_SRCc_DACC_CTL
#define RXFc_OFFSET_CTL         RXF_SRCc_OFFSET_CTL
#define RXFc_CTL                RXF_SRCc_CTL
#define RXF_BYPASS              BYPASS_RXF
#define RXFc_RESET              RXF_SRCc_RESET
#define RXFc_DC_IOFFSET_ADJ     RXF_SRCc_DC_IOFFSET_ADJ
#define RXFc_DC_QOFFSET_ADJ     RXF_SRCc_DC_QOFFSET_ADJ
#define RXFc_MIS_COMP_A_COEFF   RXF_SRCc_MIS_COMP_A_COEFF
#define RXFc_MIS_COMP_B_COEFF   RXF_SRCc_MIS_COMP_B_COEFF
#define RXFc_FG_IOFFSET         RXF_SRCc_FG_IOFFSET
#define RXFc_FG_QOFFSET         RXF_SRCc_FG_QOFFSET
#define DACCc_EST_THRESH        RXF_SRCc_DACC_EST_THRESH
#define DACCc_ACQ_TIMER         RXF_SRCc_DACC_ACQ_TIMER
#define DACCc_CLR_TIMER         RXF_SRCc_DACC_CLR_TIMER
#define RXFc_DVGA_PARAMS        RXF_SRCc_DVGA_PARAMS
#define MICROc_DACC_UPDATE      RXF_SRCc_MICRO_DACC_UPDATE
#define RXFc_PHASE_DEL          RXF_SRCc_PHASE_DEL

#define SAMPSRV_TEST_RAM_ADDR   CDMA_SAMPSRV_TEST_RAM_ADDR
#define SAMPSRV_TEST_RAM_DATA   CDMA_SAMPSRV_TEST_RAM_DATA 
#define SAMPSRV_n_MODE          CDMA_SAMPSRV_n_MODE
#define SAMPSRV_n_CTL           CDMA_SAMPSRV_n_CTL
#define SAMPSRV_n_LATEST_RTC    CDMA_SAMPSRV_n_LATEST_RTC

#define HWIO_RXF_SRCc_CTL_DIGITAL_VGA_BYPASS_BMSK    HWIO_RXF_SRCc_CTL_BYPASS_DVGA_BMSK
#define HWIO_RXF_SRCc_CTL_DIGITAL_VGA_BYPASS_SHFT    HWIO_RXF_SRCc_CTL_BYPASS_DVGA_SHFT

#define HWIO_RXF_SRCc_DACC_EST_IOFFSET_DATA_BMSK     HWIO_RXF_SRCc_DACC_EST_IOFFSET_EST_I_DC_OFFSET_BMSK
#define HWIO_RXF_SRCc_DACC_EST_IOFFSET_DATA_SHFT     HWIO_RXF_SRCc_DACC_EST_IOFFSET_EST_I_DC_OFFSET_SHFT

#define HWIO_RXF_SRCc_DACC_EST_QOFFSET_DATA_BMSK     HWIO_RXF_SRCc_DACC_EST_QOFFSET_EST_Q_DC_OFFSET_BMSK
#define HWIO_RXF_SRCc_DACC_EST_QOFFSET_DATA_SHFT     HWIO_RXF_SRCc_DACC_EST_IOFFSET_EST_Q_DC_OFFSET_SHFT

#define HWIO_RXF_SRCc_FG_IOFFSET_DATA_BMSK           HWIO_RXF_SRCc_FG_IOFFSET_FG_I_OFFSET_BMSK
#define HWIO_RXF_SRCc_FG_IOFFSET_DATA_SHFT           HWIO_RXF_SRCc_FG_IOFFSET_FG_I_OFFSET_SHFT

#define HWIO_RXF_SRCc_FG_QOFFSET_DATA_BMSK           HWIO_RXF_SRCc_FG_QOFFSET_FG_Q_OFFSET_BMSK
#define HWIO_RXF_SRCc_FG_QOFFSET_DATA_SHFT           HWIO_RXF_SRCc_FG_IOFFSET_FG_Q_OFFSET_SHFT

#define HWIO_RXF_SRCc_MICRO_DACC_UPDATE_DATA_BMSK    HWIO_RXF_SRCc_MICRO_DACC_UPDATE_MICRO_DACC_UPDATE_BMSK
#define HWIO_RXF_SRCc_MICRO_DACC_UPDATE_DATA_SHFT    HWIO_RXF_SRCc_MICRO_DACC_UPDATE_MICRO_DACC_UPDATE_SHFT

   
#define HWIO_RXF_SRCc_PHASE_DEL_DATA_BMSK    HWIO_RXF_SRCc_PHASE_DEL_RXF_PHASE_SEL_DELAY_BMSK
#define HWIO_RXF_SRCc_PHASE_DEL_DATA_SHFT    HWIO_RXF_SRCc_PHASE_DEL_RXF_PHASE_SEL_DELAY_SHFT

#endif /* T_QSC60X5 */

#if defined( T_QSC1100 )

/* T_QSC1100 RXF HAL */
//#define RXFc_FG_IOFFSET         RXF_SRCc_FG_IOFFSET
//#define RXFc_FG_QOFFSET         RXF_SRCc_FG_QOFFSET
//#define DACCc_EST_IOFFSET       RXF_SRCc_DACC_EST_IOFFSET
//#define DACCc_EST_QOFFSET       RXF_SRCc_DACC_EST_QOFFSET
#define RXFc_DACC_CTL           RXF_SRCc_DACC_CTL
#define RXFc_OFFSET_CTL         RXF_SRCc_OFFSET_CTL
#define RXFc_CTL                RXF_SRCc_CTL
//#define RXF_BYPASS              BYPASS_RXF
#define RXFc_RESET              RXF_SRCc_RESET
//#define RXFc_DC_IOFFSET_ADJ     RXF_SRCc_DC_IOFFSET_ADJ
//#define RXFc_DC_QOFFSET_ADJ     RXF_SRCc_DC_QOFFSET_ADJ
//#define RXFc_MIS_COMP_A_COEFF   RXF_SRCc_MIS_COMP_A_COEFF
//#define RXFc_MIS_COMP_B_COEFF   RXF_SRCc_MIS_COMP_B_COEFF
//#define RXFc_FG_IOFFSET         RXF_SRCc_FG_IOFFSET
//#define RXFc_FG_QOFFSET         RXF_SRCc_FG_QOFFSET
//#define DACCc_EST_THRESH        RXF_SRCc_DACC_EST_THRESH
//#define DACCc_ACQ_TIMER         RXF_SRCc_DACC_ACQ_TIMER
//#define DACCc_CLR_TIMER         RXF_SRCc_DACC_CLR_TIMER
#define RXFc_DVGA_PARAMS        RXF_SRCc_DVGA_PARAMS
#define MICROc_DACC_UPDATE      RXF_SRCc_MICRO_DACC_UPDATE
#define RXFc_PHASE_DEL          RXF_SRCc_PHASE_DEL

#define SAMPSRV_TEST_RAM_ADDR   CDMA_SAMPSRV_TEST_RAM_ADDR
#define SAMPSRV_TEST_RAM_DATA   CDMA_SAMPSRV_TEST_RAM_DATA 
#define SAMPSRV_n_MODE          CDMA_SAMPSRV_n_MODE
#define SAMPSRV_n_CTL           CDMA_SAMPSRV_n_CTL
#define SAMPSRV_n_LATEST_RTC    CDMA_SAMPSRV_n_LATEST_RTC

#define HWIO_RXF_SRCc_CTL_DIGITAL_VGA_BYPASS_BMSK    HWIO_RXF_SRCc_CTL_BYPASS_DVGA_BMSK
#define HWIO_RXF_SRCc_CTL_DIGITAL_VGA_BYPASS_SHFT    HWIO_RXF_SRCc_CTL_BYPASS_DVGA_SHFT

//#define HWIO_RXF_SRCc_DACC_EST_IOFFSET_DATA_BMSK     HWIO_RXF_SRCc_DACC_EST_IOFFSET_EST_I_DC_OFFSET_BMSK
//#define HWIO_RXF_SRCc_DACC_EST_IOFFSET_DATA_SHFT     HWIO_RXF_SRCc_DACC_EST_IOFFSET_EST_I_DC_OFFSET_SHFT

//#define HWIO_RXF_SRCc_DACC_EST_QOFFSET_DATA_BMSK     HWIO_RXF_SRCc_DACC_EST_QOFFSET_EST_Q_DC_OFFSET_BMSK
//#define HWIO_RXF_SRCc_DACC_EST_QOFFSET_DATA_SHFT     HWIO_RXF_SRCc_DACC_EST_IOFFSET_EST_Q_DC_OFFSET_SHFT

//#define HWIO_RXF_SRCc_FG_IOFFSET_DATA_BMSK           HWIO_RXF_SRCc_FG_IOFFSET_FG_I_OFFSET_BMSK
//#define HWIO_RXF_SRCc_FG_IOFFSET_DATA_SHFT           HWIO_RXF_SRCc_FG_IOFFSET_FG_I_OFFSET_SHFT

//#define HWIO_RXF_SRCc_FG_QOFFSET_DATA_BMSK           HWIO_RXF_SRCc_FG_QOFFSET_FG_Q_OFFSET_BMSK
//#define HWIO_RXF_SRCc_FG_QOFFSET_DATA_SHFT           HWIO_RXF_SRCc_FG_IOFFSET_FG_Q_OFFSET_SHFT

#define HWIO_RXF_SRCc_MICRO_DACC_UPDATE_DATA_BMSK    HWIO_RXF_SRCc_MICRO_DACC_UPDATE_MICRO_DACC_UPDATE_BMSK
#define HWIO_RXF_SRCc_MICRO_DACC_UPDATE_DATA_SHFT    HWIO_RXF_SRCc_MICRO_DACC_UPDATE_MICRO_DACC_UPDATE_SHFT

#define HWIO_RXF_SRCc_PHASE_DEL_DATA_BMSK    HWIO_RXF_SRCc_PHASE_DEL_RXF_PHASE_SEL_DELAY_BMSK
#define HWIO_RXF_SRCc_PHASE_DEL_DATA_SHFT    HWIO_RXF_SRCc_PHASE_DEL_RXF_PHASE_SEL_DELAY_SHFT
   
#endif /* T_QSC1100 */


#endif /* RF_HAL_H */

