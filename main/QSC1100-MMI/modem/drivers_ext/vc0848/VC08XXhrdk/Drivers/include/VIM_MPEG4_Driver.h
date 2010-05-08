/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]          : VIM_MPEG4_Driver.h
 * [Description]        : 适用于578b.和MPEG4相关的头文件
 *				
 * [Author]                 : angela	
 * [Date Of Creation]  : 2007-05-20 16:01
 * [Platform]               : 平台相关
 * [Note]                 : 
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        			Author     		Modifications
 * ------------------------------------------------------------------------------
 * 2007-05-20		angela      		 Created
 *******************************************************************************/
/*******************************************************************************
 * This source code has been made available to you by VIMICRO on an
 * AS-IS basis. Anyone receiving this source code is licensed under VIMICRO
 * copyrights to use it in any way he or she deems fit, including copying it,
 * modifying it, compiling it, and redistributing it either with or without
 * modifications. Any person who transfers this source code or any derivative
 * work must include the VIMICRO copyright notice and this paragraph in
 * the transferred software.
 *******************************************************************************/


#ifndef _VIM_5XRDK_MPEG4_H_
#define _VIM_5XRDK_MPEG4_H_

#ifdef __cplusplus
extern "C" {
#endif
#if (BOARD_TYPE==VC0820_FPGA)
#define VIM_MPEG4_CAP_MAX_WIDTH 720
#define VIM_MPEG4_CAP_MAX_HEIGHT 576
#endif

typedef enum _VIM_MPEG4_VOPMODE
{
	VIM_MPEG4_FIRSTVOP=0x01,
	VIM_MPEG4_NEXTVOP=0X2,
	VIM_MPEG4_LASTVOP=0X6,
       VIM_MPEG4_VOPMODE_RESERVED=0x7fffffff	
}VIM_MPEG4_VOPMODE;

#if(BOARD_TYPE==VC0820_FPGA)
typedef enum _VIM_MP4_DECMODE
{
	VIM_MPEG4_MP4_START=0X1,
	VIM_MPEG4_VOP_START =0X2,
	VIM_MPEG4_BLOCK_START=0X4,	
	VIM_MPEG4_LAST_BLOCK =0X8	
}VIM_MP4_DECMODE;
typedef enum _VIM_MP4_DECINTFLAG
{
	VIM_MPEG4_MP4_FINISH = 0X01,
	VIM_MPEG4_VOP_END = 0X02,
	VIM_MPEG4_BLOCK_END  = 0X04,
	VIM_MPEG4_STUCK_ERROR   = 0X08,
	VIM_MPEG4_PINGPANG_BLOCKING  = 0X10,
	VIM_MPEG4_TV_BUFFER_AVAILABLE   = 0X20
}VIM_MP4_DECINTFLAG;

#endif

typedef enum _VIM_MPEG4_DECMODE
{
	VIM_MPEG4_DECNEXTVOP=0x02,
	VIM_MPEG4_NEXTBLOCK=0X4,
	VIM_MPEG4_NEXVOPBLOCK=0X6,
	VIM_MPEG4_LASTBLOCK=0XC,
	VIM_MPEG4_FIRSTBLOCK=0X7,
	VIM_MPEG4_ONLYONEBLOCK=0Xf

}VIM_MPEG4_DECMODE;
typedef enum _VIM_MPEG4_BITLENGTH
{
	VIM_MPEG4_32BIT=0x0100,
	VIM_MPEG4_8BIT=0X0
}VIM_MPEG4_BITLENGTH;
typedef enum _VIM_MPEG4_MODE
{
	VIM_MPEG4_ENCODE=0x01,
	VIM_MPEG4_DECODE=0X2,
	VIM_MPEG4_VIDEOCON=0X3,
	VIM_MPEG4_NOUSE=0X4,
#if(BOARD_TYPE==VC0820_FPGA)
	VIM_MPEG4_DECODE_LBUF=0X5,
#endif
	VIM_MPEG4_MODE_RESERVED=0x7fffffff
}VIM_MPEG4_MODE;
typedef enum _VIM_MPEG4_VOPTYPE
{
	VIM_MPEG4_VOP_I=0x0,
	VIM_MPEG4_VOP_P=0X1,
       VIM_MPEG4_VOPTYPE_RESERVED=0x7fffffff
}VIM_MPEG4_VOPTYPE;
typedef enum _VIM_MPEG4_ENCINTFLAG
{
	//VIM_MPEG4_ENC_FINISH=0x3,
	VIM_MPEG4_ENC_FINISH=0x2,
	VIM_MPEG4_ENCVOP_FINISH=0X1,
	VIM_MPEG4_ENCSTUFF_ERROR=0X4,
	VIM_MPEG4_LINEBUF_ERROR=0XC,
    VIM_MPEG4_INTFLAG_RESERVED=0x7fffffff	
}VIM_MPEG4_ENCINTFLAG;

typedef enum _VIM_MPEG4_DECINTFLAG
{
	VIM_MPEG4_DEC_FINISH=0x1,
	VIM_MPEG4_DECVOP_FINISH=0X22,
	VIM_MPEG4_DECVOP1_FINISH=0X2,
	VIM_MPEG4_DECBLOCK_FINISH=0X4,
	VIM_MPEG4_DECBLOCK_FINISH1=0X24,
	VIM_MPEG4_PINGPANGDECBLOCK_FINISH=0X14,
	VIM_MPEG4_DECBLOCKVOP_FINISH=0X6,
	VIM_MPEG4_DECSTUFF_ERROR=0X8,
	VIM_MPEG4_VOPDECSTUFF_ERROR=0xa,
	VIM_MPEG4_PINGPANG_BLOCK=0X10,
	VIM_MPEG4_TVVDEC=0X20
}VIM_MPEG4_DECINTFLAG;

typedef struct _tag_VIM_MPEG4_RCParam
{
	UINT32 vop_quant_ini;
	UINT32 vop_quant_rc;
	UINT32 no_frame_skipping ;
	UINT32 vop_coded_rc;
	UINT32 buffer_fullness;
	UINT32 rc_fixed_Qp;
	
	/*!< Input */
	UINT32 Rs;         		/*!< bitrate for the sequence */
	UINT32 Rp;         		/*!< bits to be removed from the buffer per picture, Rp=Rs/Fs */
	UINT32 Bs;        		/*!< buffer size */
	UINT32 Bs_thr_ini;		/*!< buffer upper threshold, Bs*0.8 */
	UINT32 Bs_thr_hi;  		/*!< high level threshold */
	UINT32 Bs_thr_lo;  		/*!< low level threshold */
	
	UINT32 Rc;               /*!< bits used for the current frame */
	UINT32 last_Rc;          /*!< bits used for the last frame except skipping frame */
	UINT32 last_coded_Rc;    /*!< bits used for the last coded frame */
	UINT32 cur_vop_typ;      /*!< current vop type, I(0), P(1) */
	UINT32 cur_B;            /*!< actual current buffer fullness */
	UINT32 cur_vop_coded;
	UINT32 cur_vop_quant;
	
	/*!< Output */
	UINT32 nxt_B;
	UINT32 nxt_vop_typ;
	UINT32 nxt_vop_coded;
	UINT32 nxt_vop_quant;
	
}VIM_MPEG4_RCParam, *PVIM_MPEG4_RCParam;
typedef enum _VIM_MPEG4_INT_TYPE
{
	VIM_INT_MPEG4_ENC_DONE		= 0,
	VIM_INT_MPEG4_DEC_DONE		= 1,
	VIM_INT_MPEG4_HW_VSYN		=2,
	VIM_INT_MPEG4_HW_TIMER		=3,
	VIM_MPEG4_INT_ALL		= (VIM_MPEG4ENC_INT_NUM+VIM_MPEG4DEC_INT_NUM+VIM_TIMER_INT_NUM),
	VIM_MPEG4_INT_TYPE_RESERVED=0x7fffffff
}VIM_MPEG4_INT_TYPE;
#if (BOARD_TYPE==VC0820_FPGA)
UINT32 VIM_MPEG4_Estimate_I_Size(UINT32 width, UINT32 height,UINT32 bitrate);
void VIM_MPEG4_GetDecSize(PTSize psize);
void VIM_MPEG4_AdjustClkBlank(void);
#endif
void VIM_MPEG4_GetVolSize(PTSize psize);
void _ISR_Mpeg4EncIntHandle(UINT8 wIntFlag);
 void _ISR_Mpeg4DecIntHandle(UINT8 wIntFlag);
 void _ISR_Mpeg4TimerIntHandle(UINT8 wIntFlag);
void VIM_MPEG4_SetIntEnable(VIM_MPEG4_INT_TYPE bySecondLevelInt,BOOL Eanble);
void VIM_MPEG4_Reset(void);
void VIM_MPEG4_StartEnc(VIM_MPEG4_VOPMODE VopMode,UINT8 FrameNum);
void VIM_MPEG4_SetVopQt(UINT8 Qt);
void VIM_MPEG4_SetVolSize(UINT16 wWidth, UINT16 wHeight);
void VIM_MPEG4_SetFrameRate(UINT16 FrameRate);
void VIM_MPEG4_SetVopType(VIM_MPEG4_VOPTYPE VopType);
void VIM_MPEG4_SetVopCoded(BOOL Enalbe);
void VIM_MPEG4_SetMode(VIM_MPEG4_MODE bMode);
UINT32 VIM_MPEG4_GetLength(UINT8 Num, UINT8 MaxFrame);
 void VIM_MPEG4_SetExternTime(UINT16 Frame);
 void VIM_MPEG4_SetVsyCapEn(BOOL Eenable);
 void VIM_MPEG4_SetHwTimerInt(UINT16 TimeS);
void VIM_MPEG4_initrc(
		 PVIM_MPEG4_RCParam	prcParam,
		 UINT32			m4RC_TargetBitRate_Channel_p, 
		 UINT32			rc_no_frame_skip_p, 
		 UINT32			rc_fixed_Qp_p, 
		 UINT32			vop_time_increment_p, 
		 UINT32   		vop_time_increment_resolution_p, 
		 UINT32			iniQp_p,
		 UINT32			*m4Qp_g,
		 UINT32  		*m4Coded_g,
		 UINT32   		multiFrame);

void VIM_MPEG4_updatetargetbitratevars(	
					PVIM_MPEG4_RCParam	prcParam,
					UINT32			m4RC_TargetBitRate_Channel_p, 
		 			UINT32			vop_time_increment_p, 
		 			UINT32  		vop_time_increment_resolution_p,
					UINT32    		multiFrame);
 VIM_MPEG4_ENCINTFLAG VIM_MPEG4_GetEncIntFlag(void);
void VIM_MPEG4_RegisterISR(VIM_MPEG4_INT_TYPE bySecondLevelInt, PVIM_Isr  Isr);
void VIM_MPEG4_takeactiveonce(	
				PVIM_MPEG4_RCParam  prcParam,
				UINT32  		encSize_g,
				UINT32  		m4Typ_g,
				UINT32  		*m4Coded_g,
				UINT32  		*m4Qp_g);
UINT32 VIM_MPEG4_SetTimeAdd(UINT32 TimeTicks);
void VIM_MPEG4_SetLength(UINT32 Length);
void VIM_MPEG4_StartDec(VIM_MPEG4_DECMODE VopMode);
 VIM_MPEG4_DECINTFLAG VIM_MPEG4_GetDecIntFlag(void);
void VIM_MPEG4_SetDecVolSize(UINT16 wWidth, UINT16 wHeight);
void VIM_MPEG4_GetDecVolSize(PTSize psize);
#ifdef __cplusplus
}
#endif

#endif /* _RDK_HIF_H_ */
