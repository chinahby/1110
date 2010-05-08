#ifndef MDP_LAYER_PROC_H
#define MDP_LAYER_PROC_H
/*============================================================================

                   MDP Device Driver Header File

  DESCRIPTION
  This file contains the definitions needed for the MDP device driver.

  Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/display/mdp1/mdp_api/main/latest/inc/mdp_layer_proc.h#1 $ $DateTime: 2008/05/16 15:58:49 $ $Author: aansari $

when      who    what, where, why
--------  -----  ----------------------------------------------------------
11/07/07  dw      Add memory reduction support
03/08/05  cjr     Adjusted MDP system bitmap with BREW offset
02/10/05  cjr     Completed source file re-architect
02/08/05  cjr	    Initial code
============================================================================*/

//============================================================================
//                        INCLUDE FILES
//============================================================================
#include "mdp_api.h"

#define MDP_MAX_SUBWIN 5

typedef struct mdpSubWin_
{
	uint8 win;	
	uint16 lcdX[MDP_MAX_SUBWIN];
	uint16 lcdY[MDP_MAX_SUBWIN];
	
	MDP_CWIN cwin[MDP_MAX_SUBWIN];	
} MDP_SUBWIN;

#define MDPOP_DISABLE_ALPHAB  0x80000000

extern disp_info_type _primary_lcd;

#ifdef FEATURE_MDP2
#error code not present
#else
#define MDP_SYS_BMP_OFFSET (MDP_SYS_OFFSET_Y*_primary_lcd.disp_width*2)
#endif

/*===========================================================================
    LOCAL FUNCTIONS PROTOTYPES
============================================================================*/
void mdp_adjust_XY(MDPIMG *mdpImg,
                   int32 *sx1, 
                   int32 *sx2, 
                   int32 *sy1, 
                   int32 *sy2, 
                   MDPSCALE_INFO *scale);
void mdpCalSubWin(MDP_CWIN *thisXY, MDP_CWIN *ovXY, MDP_SUBWIN *sub);
void mdp_vdo_scale_cal(int32 *x1, int32 *x2, int32 *y1, int32 *y2, 
                       uint16 x_num, uint16 x_den,                       
                       uint16 y_num, uint16 y_den,
                       int32 lcd_x, int32 lcd_y);
int mdpCalOverXYInt(int32 fg_x1, 
					int32 fg_x2, 
					int32 fg_y1, 
					int32 fg_y2, 
					int32 bg_x1, 
					int32 bg_x2, 
					int32 bg_y1, 
					int32 bg_y2, 
					MDP_CWIN *overXY);
MDPIMG_LIST * mdp_sort_imglist(MDPIMG_LIST * node);
uint32 mdpPeekOverXYInt(int32 fg_x1, 
					int32 fg_x2, 
					int32 fg_y1, 
					int32 fg_y2, 
					int32 bg_x1, 
					int32 bg_x2, 
					int32 bg_y1, 
					int32 bg_y2);
void mdp_get_subwin_gfx(LCD_TYPE destDev,
						MDPIMG_LIST *fgImg,
						MDPIMG_LIST *bgImg,
						MDP_SUBWIN *fgSub,
						MDP_SUBWIN *ovSub);					
uint32 mdp_cal_ovimg_rec( MDPIMG *mdpImg,
                          uint32 sameLayer,
                          uint32 * fgSubFlag,
                          MDPIMG * orig_fgImg,
                          MDPSCALE_INFO * orig_scale,
                          uint32 * orig_fgMbcOp,
                          uint32 * alphaMask,
                          uint32 * vsync_flag,
                          MDP_CWIN * this_cwin,
                          uint32 sysBmpMode);
                          
/*===========================================================================
============================================================================*/
	
	
#endif /* MDP_LAYER_PROC_H */

