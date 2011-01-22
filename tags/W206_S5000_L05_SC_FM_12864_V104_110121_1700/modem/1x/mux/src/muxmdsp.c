/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              MUX mDSP interface drivers

GENERAL DESCRIPTION
  Contains implementation of the MUX mDSP drivers


EXTERNALIZED FUNCTIONS
  muxmdsp_wr_pdchChanEnable  - Enable DV operation

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2003 through 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                              Edit History

$PVCSPath:   $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/muxmdsp.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/08   jtm     Removed FEATURE_IS2000_REL_C_DV.
01/09/07   vlc     Merged the following change to main line from SC2X branch.
                   Mainly, adding support for FEATURE_MDSP_SUPPORTS_TX_AGC.
           vm      Added the support for Firmware TX 
09/05/06   vlc     Implemented TX gain enhancements to increase digital gain
                   resolution.
06/09/06   trc     Remove T_MSM6700 featurization
03/14/05   vlc     Fixed compiler warnings for MSM6700.
03/07/05   bn      Cleaned up debug F3 messages
12/09/04   jyw     Refeatureized to seperate the DV code and 1x code
                   under T_MSM6700
11/08/04   sr      Added muxmdsp_wr_pdcchStartDelay()
09/14/04   jrp     Update calls to DV_INFO header.
09/10/04   ljl     Added the function muxmdsp_wr_rlGainPdchMode().
09/01/04   jrp     Fixed Nak counter offsets.  Added counters for 0.5x rates.
                   Fixed compilation error for 6700 builds.
09/01/04   sr      Added handshaking between ARM and mDSP in
                   muxmdsp_dv_chan_disable()
09/01/04   jrp     Conditionally compile entire file.
08/30/04   jrp     Corrected offsets for statistics counters.
08/20/04   jrp     Added support for hidra dme buffer.
08/16/04   sr      Replaced mdsp_read() and mdsp_write() macros with
                   mdsp_read_offset() and mdsp_write_offset(), respectively,
                   for indexed reads and writes to mDSP registers
08/13/04   ljl     Added to enable R-PICH for CQI/ACK override PLT tests.
08/02/04   jrp     Changed dme base memory address for new target.
07/30/04   vlc     Added support for rel C TDSO.
                   Removed obsolete muxmdsp_rd_ackHistoryValid() and
                   muxmdsp_rd_ackHistoryState().
07/28/04   jyw     Fixed the bug in the muxmdsp_wr_pdchInitialDescrambleState
07/28/04   jrp     Renamed Rev D gain enums.
07/27/04   jrp     Moved non-DV functions outside ifdef.
07/26/04   bn      Added new function for tx_rate_adj latched frame offset
07/23/04   jyw     Modifed the interface of function muxmdsp_wr_pdchInitialDescrambleState
07/23/04   jyw     moved the function call of muxmdsp_wr_pdchInitialDescrambleState from
                   muxmdsp_dv_ctrl_init() to pdch_init();
07/21/04   bn      Added function for MDSP frame offset loading command
07/20/04   bn      Indicate to the MDSP to load the rev frame offset immediately
07/15/04   jrp     Added calls to muxmdsp_wr_passThruEnable() on DV channel
                   enable and disable.
06/29/04   jrp     Added pass-through and init functions.
06/24/04   sr      Updated muxmdsp_wr_walshinfo(), dme base address & fixed
                   some typos in decob interface functions
06/18/04   bn      Added support for Rel D channel gains override
05/19/04   sr      Updated to latest mDSP firmware and cleaned up warnings
05/04/04   bn      Added the MDSP to ARM universal INT handler and registering utilizes
03/17/04   bn      Modified the register callback function to allow deregistering
03/09/04   sr/bkm  Featurized DV code
09/09/03   bn      Created MUX mDSP interface file.

===========================================================================*/

#include "customer.h"
#include "target.h"
#include "comdef.h"
#include "mdspext.h"
#include "muxmdsp.h"
#include "msg.h"
#include "tx.h"
#include "txccommon.h"


#ifdef FEATURE_MDSP_SUPPORTS_TX_AGC
#include "rf_mdsp_tx_agc.h"  
#define MUXMDSP_10_TO_16_BIT( val )      ( val << 6 )
#endif

/*===========================================================================

FUNCTION MUXMDSP_WR_PASSTHRUENABLE

DESCRIPTION
  This funtion sets (TRUE) or clears (FALSE) the mDSP digital gain pass
  through functionality.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void muxmdsp_wr_passThruEnable(boolean enable)  /* TRUE turns on pass-through */
{
  if (enable)
  {
    mdsp_write(CDMA1X_cdma1xPassThruEnable, 0xFFFF); /* -1 equivalent */
  }
  else
  {
    mdsp_write(CDMA1X_cdma1xPassThruEnable, 0);
  }
}


/*===========================================================================

FUNCTION muxmdsp_wr_mod_pch_gain

DESCRIPTION
  Write the MOD_PCH_GAIN to be latched at last PCG of fundicated frame by mDSP
  and latched to HW on the fundicated frame boundary.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void muxmdsp_wr_mod_pch_gain (
  word pch_gain)       /* 15 bit resolution rev. pilot ch gain */
{
  mdsp_write(CDMA1X_MOD_PCH_GAIN, pch_gain);
  //MSG_ERROR("R-PICH = %d",pch_gain,0,0);
}

/*===========================================================================

FUNCTION muxmdsp_wr_mod_fch_gain

DESCRIPTION
  Write the MOD_FCH_GAIN to be latched at last PCG of fundicated frame by mDSP
  and latched to HW on the fundicated frame boundary.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void muxmdsp_wr_mod_fch_gain (
  word fch_gain)       /* 15 bit resolution rev. fundamental ch gain */
{
  mdsp_write(CDMA1X_MOD_FCH_GAIN, fch_gain);
  //MSG_ERROR("R-FCH = %d",fch_gain,0,0);
}


/*===========================================================================

FUNCTION muxmdsp_wr_mod_dcch_gain

DESCRIPTION
  Write the MOD_DCCH_GAIN to be latched at last PCG of fundicated frame by mDSP
  and latched to HW on the fundicated frame boundary.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void muxmdsp_wr_mod_dcch_gain (
  word dcch_gain)       /* 15 bit resolution rev. dedicated control ch gain */
{
  mdsp_write(CDMA1X_MOD_DCCH_GAIN, dcch_gain);
  //MSG_ERROR("R-DCCH = %d",dcch_gain,0,0);
}



/*===========================================================================

FUNCTION muxmdsp_wr_mod_sch_gain

DESCRIPTION
  Write the MOD_SCH_GAIN to be latched at last PCG of fundicated frame by mDSP
  and latched to HW on the fundicated frame boundary.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void muxmdsp_wr_mod_sch_gain (
  word sch_gain)       /* 15 bit resolution rev. supplemental ch gain */
{
  mdsp_write(CDMA1X_MOD_SCH_GAIN, sch_gain);
  //MSG_ERROR("R-SCH = %d",sch_gain,0,0);
}

/*===========================================================================

FUNCTION muxmdsp_wr_mod_prmbl_gain

DESCRIPTION
  Write the MOD_PRMBL_GAIN to be latched at last PCG of fundicated frame by mDSP
  and latched to HW on the fundicated frame boundary.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void muxmdsp_wr_mod_prmbl_gain (
  word prmbl_gain)       /* 8:0 of MOD_PRMBL_GAIN register is rev. supplemental
                          ch gain */
{
  mdsp_write(CDMA1X_MOD_PRMBL_GAIN, prmbl_gain<<1);
  MSG_ERROR("PRMBL = %d",prmbl_gain,0,0);
}


/*===========================================================================

FUNCTION muxmdsp_wr_tx_rate_adj

DESCRIPTION
  Write the TX_RATE_ADJ to be latched at last PCG of fundicated frame by mDSP
  and latched to HW on the fundicated frame boundary.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void muxmdsp_wr_tx_rate_adj (
  word tx_rate_adj)
{
  #ifdef FEATURE_MDSP_SUPPORTS_TX_AGC  
  word tx_rate_adj_mdsp = (word)(MUXMDSP_10_TO_16_BIT(tx_rate_adj));

  CDMA_MDSP_WR_TX_RATE_ADJ(tx_rate_adj_mdsp);
  #else
  mdsp_write(CDMA1X_TX_RATE_ADJ, tx_rate_adj); 
  #endif
  
 // MSG_ERROR("tx_rate_adj = %d",tx_rate_adj,0,0);
}

/*===========================================================================

FUNCTION muxmdsp_wr_tx_rate_adj_latch_offset

DESCRIPTION
  The function updates offset parameter which spcecifies the number of chips before
  the fundicated frame boundary that the modulator HW latches TX_RATE_ADJ

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void muxmdsp_wr_tx_rate_adj_latch_offset (
  word tx_rate_adj_latch_offset)
{
  mdsp_write(CDMA1X_txRateAdjLatchOffset, tx_rate_adj_latch_offset);
}

/*===========================================================================

FUNCTION MUXMDSP_WR_FUNDFRAMEOFFSET

DESCRIPTION
  This funtion sets the fundicated frame offset to the mDSP

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void muxmdsp_wr_fundFrameOffset (uint16  fundoffset_value) /* value range from 0 to 15 PCG*/

{
   /* call the function to write the fundicated frame offset to the mDSP*/
   mdsp_write(CDMA1X_fundFrameOffset, fundoffset_value);
}

/*===========================================================================

FUNCTION MUXMDSP_WR_FUNDFRAMEOFFSETIMMLOADCMD

DESCRIPTION
  This funtion indicates to the mDSP if the frame offset is load immediately

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void muxmdsp_wr_fundFrameOffsetImmLoadCmd (boolean load_imm)

{
   if (load_imm)
   {
     /* load right away */
     mdsp_write(CDMA1X_fundFrameOffsetImmLoadCmd, 0x1);
   }
   else
   {
     /* load right away */
     mdsp_write(CDMA1X_fundFrameOffsetImmLoadCmd, 0x0);
   }
}

/*===========================================================================

FUNCTION muxmdsp_cdma_init

DESCRIPTION
  This function is called by MUX to initialize DV control parameters to mDSP

DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS
  None.

===========================================================================*/
void muxmdsp_cdma_init(void)
{
  mdsp_write(CDMA1X_MOD_PRMBL_CTL_0, 0);
  mdsp_write(CDMA1X_MOD_PRMBL_CTL_1, 0);
  muxmdsp_wr_mod_pch_gain(0);
  muxmdsp_wr_mod_fch_gain(0);
  muxmdsp_wr_mod_dcch_gain(0);
  muxmdsp_wr_mod_sch_gain(0);
  muxmdsp_wr_mod_prmbl_gain(0);
}
