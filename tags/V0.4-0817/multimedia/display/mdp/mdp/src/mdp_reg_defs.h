#ifndef MDP_REG_DEFS_H 
#define MDP_REG_DEFS_H
/*============================================================================

                   MDP Device Driver Header File

 DESCRIPTION
   This file contains the definitions needed for the MDP device driver.

 Copyright (c) 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/display/mdp1/mdp/main/latest/src/mdp_reg_defs.h#1 $ $DateTime: 2008/05/06 14:29:11 $ $Author: karenp $
 
 
 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 05/28/04  cjr	  Initial code
============================================================================*/

//============================================================================
//                        INCLUDE FILES
//============================================================================

// MDP_COMMAND (W) - Register Definitions

#define BEGIN_SCRIPT 		0x01
//#define HALT_SCRIPT		0x02
#define LOAD_RD_PTR			0x04


// MDP_SYNC_CONFIG  (R/W) - Register Definitions
// SST - 'Synchronization, Synthesis and Track' block of MDP 

//#define GAMMA_COR_DIS		0x80000			// Disable Gamma Correction for R,G,B values
//#define SST_CGC_EN		0x20000			// Enable the clock gating of SST block
#define	PRIM_VSYNC			0x00000
#define SEC_VSYNC			0x10000
#define	SW_VSYNC			0x20000


// MDP_SYNC_STATUS (R) - Register Definitions 

//#define SYNC_STATE		while(!(inpdw(MDP_SYNC_STATUS) & 0x80000000))
//#define SYNC_STATE		inpdw(MDP_SYNC_STATUS) & 0x80000000

#define FRAME_COUNT		((inpdw(MDP_SYNC_STATUS) & 0x00FF0000) >> 16)
#define LINE_COUNT		(inpdw(MDP_SYNC_STATUS) & 0x00000FFF)


// MDP_SST_REFRESH_COUNT (R) - Register Definitions

#define REFRESH_COUNT	(inpdw(MDP_SST_REFRESH_COUNT) & 0x00FFFFFF)


// MDP_SCRIPT_STATUS (R) - Register Definitions

#define SCRIPT_ACTIVE	while(!(inpdw(MDP_SCRIPT_STATUS) & 0x00000001))
#define SCRIPT_DONE		while((inpdw(MDP_SCRIPT_STATUS) & 0x00000001))


// MDP_INTR_ENABLE (R/W) - Register Definitions

#define VSYNC_INTR_EN 				outpdw(MDP_INTR_ENABLE, ((inpdw(MDP_INTR_ENABLE)) | 0x001))
#define CIP_INTR_EN 				outpdw(MDP_INTR_ENABLE, ((inpdw(MDP_INTR_ENABLE)) | 0x002))
#define JOB_DONE_INTR_EN 			outpdw(MDP_INTR_ENABLE, ((inpdw(MDP_INTR_ENABLE)) | 0x004))
#define END_SCRIPT_INTR_EN 			outpdw(MDP_INTR_ENABLE, ((inpdw(MDP_INTR_ENABLE)) | 0x008))
#define SLAVE_ERROR_INTR_EN 		outpdw(MDP_INTR_ENABLE, ((inpdw(MDP_INTR_ENABLE)) | 0x010))
#define MASTER_ERROR_INTR_EN 		outpdw(MDP_INTR_ENABLE, ((inpdw(MDP_INTR_ENABLE)) | 0x020))


#define VSYNC_INTR_DIS 				outpdw(MDP_INTR_ENABLE, ((inpdw(MDP_INTR_ENABLE)) & 0xFFE))
#define CIP_INTR_DIS 				outpdw(MDP_INTR_ENABLE, ((inpdw(MDP_INTR_ENABLE)) & 0xFFD))
#define JOB_DONE_INTR_DIS 			outpdw(MDP_INTR_ENABLE, ((inpdw(MDP_INTR_ENABLE)) & 0xFFB))
#define END_SCRIPT_INTR_DIS 		outpdw(MDP_INTR_ENABLE, ((inpdw(MDP_INTR_ENABLE)) & 0xFF7))
#define SLAVE_ERROR_INTR_DIS 		outpdw(MDP_INTR_ENABLE, ((inpdw(MDP_INTR_ENABLE)) & 0xFEF))
#define MASTER_ERROR_INTR_DIS 		outpdw(MDP_INTR_ENABLE, ((inpdw(MDP_INTR_ENABLE)) & 0xFDF))

// MDP_INTR_STATUS (RD only) - Register Definitions

/*
#define VSYNC_INTR 				while(!(inpdw(MDP_INTR_STATUS) & 0x001))
#define CIP_INTR 				while(!(inpdw(MDP_INTR_STATUS) & 0x002))
#define JOB_DONE_INTR 			while(!(inpdw(MDP_INTR_STATUS) & 0x004))
#define END_SCRIPT_INTR 		while(!(inpdw(MDP_INTR_STATUS) & 0x008))
#define SLAVE_ERROR_INTR 		while(!(inpdw(MDP_INTR_STATUS) & 0x010))
#define MASTER_ERROR_INTR 		while(!(inpdw(MDP_INTR_STATUS) & 0x020))
*/

#define VSYNC_INTR 				(inpdw(MDP_INTR_STATUS) & 0x001)
#define CIP_INTR 				(inpdw(MDP_INTR_STATUS) & 0x002)
#define JOB_DONE_INTR 			(inpdw(MDP_INTR_STATUS) & 0x004)
#define END_SCRIPT_INTR 		(inpdw(MDP_INTR_STATUS) & 0x008)
#define SLAVE_ERROR_INTR 		(inpdw(MDP_INTR_STATUS) & 0x010)
#define MASTER_ERROR_INTR 		(inpdw(MDP_INTR_STATUS) & 0x020)


// MDP_INTR_CLEAR (WR only) - Register Definitions

#define VSYNC_INTR_CLR 				outpdw(MDP_INTR_CLEAR, 0x001)
#define CIP_INTR_CLR 				outpdw(MDP_INTR_CLEAR, 0x002)
#define JOB_DONE_INTR_CLR 			outpdw(MDP_INTR_CLEAR, 0x004)
#define END_SCRIPT_INTR_CLR 		outpdw(MDP_INTR_CLEAR, 0x008)
#define SLAVE_ERROR_INTR_CLR 		outpdw(MDP_INTR_CLEAR, 0x010)
#define MASTER_ERROR_INTR_CLR 		outpdw(MDP_INTR_CLEAR, 0x020)


#endif /* MDP_REG_DEFS_H */
