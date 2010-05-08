/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]          : VIM_MPEG4_Driver.c
 * [Description]        : 适用于578b.定义了所有跟mpeg4有关的
 *				     函数。 
 *				
 * [Author]                 : angela	
 * [Date Of Creation]  : 2007-05-20 16:01
 * [Platform]               : 平台无关
 * [Note]                 : 
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        			Author     		Modifications
 * ------------------------------------------------------------------------------
 * 2007-05-20  		angela      		 Created
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

#include "VIM_COMMON.h"



/////////////////////////tools  start//////////////////////////////////////////
/**************************** RC ****************************/
void VIM_MPEG4_rc_framelevel(
		PVIM_MPEG4_RCParam prcParam,
        /*!< input */
		UINT32 Rs,
		UINT32 Rp,
		UINT32 Bs,
		UINT32 Bs_thr_hi_i,
		UINT32 Bs_thr_lo_i,
		UINT32 last_Rc,
		UINT32 last_coded_Rc,
		UINT32 cur_vop_typ,
		UINT32 cur_B,
		UINT32 cur_vop_coded,
		UINT32 cur_vop_quant,
		UINT32 no_frame_skipping,
		UINT32 rc_fixed_Qp,
        /*!< output */
		UINT32 *nxt_B,
		UINT32 *nxt_vop_coded,
		UINT32 *nxt_vop_quant)

{
	UINT32 T;                /*!<target bits for next frame */
	UINT32 S;                /*!<previous frame usage bits */
	UINT32 d_B;              /*!<delta B */
	UINT32 d_S;              /*!<delta S */
	UINT32 d_T;              /*!<delta T */

	UINT32 tmp_nxt_B;
	UINT32 pred_nxt_B;
	UINT32 do_est_loop;
	UINT32 est_S;
	UINT32 delta_B;

	UINT32 th1;
	UINT32 th2;
	UINT32 th3;
	UINT32 th4;
	UINT32 th5;
	UINT32 th6;
	UINT32 th7;

	UINT32 rc_qp_incr;
	UINT32 rc_qp_decr;
	UINT32 rc_qp_maxincstep;  /*!<user defined maximum QP increase step */
	UINT32 rc_qp_maxdecstep;  /*!<user defined maximum QP decrease step */
	UINT32 rc_qp_ubound;      /*!<user defined QP upper bound */
    
	UINT32 limit_Qp;
    
	limit_Qp = (prcParam->vop_quant_ini != 31);
	
	/*!<Taget bits for next frame T */
    /*!<Update buffer fullness nxt_B = cur_B + Rc - Rp */
    *nxt_B = cur_B + last_coded_Rc;

    S = last_Rc;

    /*!<Update actual buffer fullness */
    /*!<B = B - Rp */
    if(*nxt_B > Rp)
        d_B = Rp;
    else
        d_B = *nxt_B;
    *nxt_B = *nxt_B - d_B; 	/*!<buffer fullness after transmitting bits to channel */


    if(*nxt_B > Bs_thr_hi_i)
    {
        if(no_frame_skipping)
			*nxt_vop_coded = 1;
        else
			*nxt_vop_coded = 0;
    }
    else
        *nxt_vop_coded = 1;

    if(cur_vop_typ == 0) 	/*!<previous frame is IVOP */
        T = S;
    else
    {
        /*!<T = max(Rp, 0.95Rp + 0.05S) <approximated form> */
        if(S > Rp)
        {
                d_S = S - Rp;
                d_T = ((d_S<<1) + d_S)>>6;
        }
        else
                d_T = 0;

        T = Rp + d_T;
    }


    /*!< Adjust T with repect to buffer high and low threshlod */
    /*!<prediction next buffer fullness */
    tmp_nxt_B = *nxt_B + T;
    if(tmp_nxt_B > Rp)
        pred_nxt_B =  tmp_nxt_B - Rp;
    else
        pred_nxt_B =  0;


    if(pred_nxt_B > Bs_thr_hi_i)
    {
        /*!< T = max{Rp, Bs_thr_hi_i + Rp - pred_nxt_B} */
        T = Rp;
    }
    else if(pred_nxt_B < Bs_thr_lo_i)
    {
        /*!<T = Bs_thr_lo_i + Rp - *nxt_B */
        /*!<T = Rp + Bs_thr_lo_i - *nxt_B */
        T = Rp + Bs_thr_lo_i - pred_nxt_B;
    }

    /*!<adjust T with repect to 1/2 Buffer size. */
    /*!<T = T - (nxt_B - Bs/2)/4 */
    th1 = (7*Bs_thr_hi_i)>>3;
    th2 = (6*Bs_thr_hi_i)>>3;
    th3 = (5*Bs_thr_hi_i)>>3;
    th4 = (4*Bs_thr_hi_i)>>3;
    th5 = (3*Bs_thr_hi_i)>>3;
    th6 = (2*Bs_thr_hi_i)>>3;
    th7 = (  Bs_thr_hi_i)>>3;

    if(*nxt_B > (Bs/2))
        delta_B = *nxt_B - (Bs/2);
    else
        delta_B = (Bs/2) - *nxt_B;


    if(*nxt_B > th1)
        if(T > delta_B/3)
                T = T - delta_B/3;
        else
                T = 0;
    else if(*nxt_B > th2)
        if(T > delta_B/4)
                T = T - delta_B/4;
        else
                T = 0;
    else if(*nxt_B > th3)
        if(T > delta_B/5)
                T = T - delta_B/5;
        else
                T = 0;
    else if(*nxt_B > th4)
        if(T > delta_B/6)
                T = T - delta_B/6;
        else
                T = 0;
    else if(*nxt_B > th5)
        T = T + delta_B/6;
    else if(*nxt_B > th6)
        T = T + delta_B/5;
    else if(*nxt_B > th7)
        T = T + delta_B/4;
    else
        T = T + delta_B/3;

    /*!<I:p = 4:1 */
    /*!<Weight = 1*30/(4+29) */
    /*!<if(cur_vop_typ == 1) !<for P-VOP */
    /*!<    T = T*30/33; */


    /*!<Estimate the new VopQuant */
    *nxt_vop_quant = cur_vop_quant;
    do_est_loop = 1;


    if(limit_Qp){
        rc_qp_maxincstep = 2;					/*!<user defined maximum QP increase step */
        rc_qp_maxdecstep = 5;					/*!<user defined maximum QP decrease step */
        rc_qp_ubound = prcParam->vop_quant_ini;	/*!<user defined QP upper bound */
    }else{
        rc_qp_maxincstep = 31;  				/*!<user defined maximum QP increase step */
        rc_qp_maxdecstep = 31;  				/*!<user defined maximum QP decrease step */
        rc_qp_ubound = 31;     					/*!<user defined QP upper bound */
    }

    if(T > S)
    {
        rc_qp_decr = 0;
        while(do_est_loop && *nxt_vop_coded  && (rc_qp_decr < rc_qp_maxdecstep))
        {
                /*!< est_S = alpha * S; */
                if(*nxt_vop_quant == 2)
                        /*!<est_S = S << 3; */
                        est_S = S*4;
                else if(*nxt_vop_quant == 3)
                        /*!<est_S = S << 2; */
                        est_S = S*3;
                else if((*nxt_vop_quant==4)||(*nxt_vop_quant==5)||(*nxt_vop_quant==6))
                        est_S = S << 1;
                else if((*nxt_vop_quant==7)||(*nxt_vop_quant==8)||(*nxt_vop_quant==9)||(*nxt_vop_quant==10))
                        est_S = S + (S>>1);
                else
                        est_S = S + (S>>3);

                /*!< Adjust vop_quant */
                if((T > est_S) && (*nxt_vop_quant!=1))
                {
                        *nxt_vop_quant = *nxt_vop_quant - 1;
                        rc_qp_decr = rc_qp_decr + 1;
                }
                else
                {
                        /*!<Avoid dead zone(B=0, but QP doesn't decrease) */
                        if(pred_nxt_B==0 && rc_qp_decr ==0 && (*nxt_vop_quant!=1))
                                *nxt_vop_quant = *nxt_vop_quant - 1;

                        do_est_loop = 0;
                }
                S = est_S;
        }
    }
    else
    {
        rc_qp_incr = 0;
        while(do_est_loop && *nxt_vop_coded  && (rc_qp_incr < rc_qp_maxincstep))
        {

                /*!< est_S = alpha * S; */
                if(*nxt_vop_quant == 1)
                        est_S = (S >> 1);
                else if(*nxt_vop_quant == 2)
                        est_S = S >> 1;
                else if((*nxt_vop_quant==3)||(*nxt_vop_quant==4)||(*nxt_vop_quant==5))
                        est_S = ((S<<3) + (S<<1) + S) >> 4;
                else
                        est_S = ((S<<3) + (S<<2) + (S<<1)) >> 4;


                /*!< Adjust vop_quant */
                /*!<if((T < est_S) && (*nxt_vop_quant!=31)) */
                if((T < est_S) && (*nxt_vop_quant!= rc_qp_ubound) && (*nxt_vop_quant!=31))
                {
                        *nxt_vop_quant = *nxt_vop_quant + 1;
                        rc_qp_incr = rc_qp_incr + 1;
                }
                else
                        do_est_loop = 0;
                        
                S = est_S;
        }
    }

    /*!< Add by HS,2004/06/28 */
    if (rc_fixed_Qp)
    {
        *nxt_vop_quant = prcParam->vop_quant_ini;
    }
    /*
    else
    {
        if(*nxt_vop_coded==0 && *nxt_vop_quant!=31)
                *nxt_vop_quant = *nxt_vop_quant + 1;
    } */

    /*!< Dynamic Buffer Threshold */
    if( *nxt_vop_coded == 0 )
        prcParam->Bs_thr_hi = prcParam->Bs_thr_hi + (Rp>>1);    /*!<katagogogo */
    else
        prcParam->Bs_thr_hi = prcParam->Bs_thr_ini;
}


/***********************************************************************************
Description:
		Initial the rc variables
		
Parameters:
		prcParam 						: 	the rc parameters structure.	
		m4RC_TargetBitRate_Channel_p	: 	Target bitrate, This is a user-defined parameter 
											in unit of kbit/sec. For example, if 64kbps, set 
											this variable as 64. If 256kbps, set this variable 
											as 256.
		rc_no_frame_skip_p				:	Avoid frame skipping flag, This is a user-defined 
											parameter to inform RC to avoid frame skipping.
											For example, if set this variable to 1, the frame 
											will always not be skipped when MP4 encoding. 
											Otherwise, if set this variable to 0, the frame 
											maybe skipped depends on RC mechanism.
		rc_fixed_Qp_p					: 	Fix QP flag, This is a user-defined parameter to 
											fix QP. For example, if set this variable to 1, 
											the QP during entire MP4 encoding will be fixed to 
											the initial QP (iniQp_p). Otherwise, if set this 
											variable to 0, the QP will be varied automatically 
											according to RC mechanism.
		vop_time_increment_p			: 	VOP time increment resolution, This is a user-defined 
											parameter of VOP time scale during one second for 
											specification the frame rate per second (fps). Here, 
											we suggest users to set this variable to 30.
		vop_time_increment_resolution_p	:	VOP time increment, This is a user-defined parameter 
											of time duration between two adjacent VOPs in unit of 
											VOP time scale (vop_time_increment_resolution_p).For 
											example, if fps 30, set vop_time_increment_p and 
											vop_time_increment_resolution_p to 1 and 30, respectively. 
											If fps 15, set set vop_time_increment_p and 
											vop_time_increment_resolution_p to 2 and 30, respectively.
		iniQp_p							:	Initial QP, This is a user-defined parameter to specify 
											the initial quantization parameter (QP) of RC. In addition, 
											it also used to specify the upper bound of QP when 
											MP4ENCencoding. For example, if iniQP_p is 20, the RC 
											mechanism set the QP 20 to encode the first frame (or VOP), 
											and then limit the maximum value of QP to 20.
		m4Qp_g							:	vop_quant, Quantization parameter (QP) for MP4 encoding VOPs.	
		m4Coded_g						:	vop_coded, A flag to inform MP4ENC the VOP is coded or not coded.										
		multiFrame						:	indicated the frame number for every rc 
																						
Returns:
		NULL
		
Remarks:
		
************************************************************************************/
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
		 UINT32   		multiFrame)
{
    prcParam->Rs = 1024 * m4RC_TargetBitRate_Channel_p;		 /*!< bitrate for the sequence */
    prcParam->no_frame_skipping = rc_no_frame_skip_p;
    prcParam->rc_fixed_Qp       = rc_fixed_Qp_p;
    
    /*!<bits to be removed from the buffer per picture, Rp=Rs/Fs */
    prcParam->Rp = (prcParam->Rs)*vop_time_increment_p/vop_time_increment_resolution_p * multiFrame;
    prcParam->Bs = (prcParam->Rs)*0.5;                		/*!< buffer size */
    prcParam->Bs_thr_ini = (prcParam->Bs)*0.8;        		/*!< buffer upper threshold, Bs*0.8 */
    prcParam->Bs_thr_hi = prcParam->Bs_thr_ini;     		/*!< high level threshold */
    prcParam->Bs_thr_lo = (prcParam->Bs)*0.1;        		/*!< low level threshold */
   
    prcParam->buffer_fullness = (prcParam->Bs)>>1; 			/*!< initial buffer fullness = 1/2 * buffer size */
    
    prcParam->nxt_vop_typ = 0;  /*!<IVOP */   

    prcParam->vop_coded_rc = 1;
    prcParam->vop_quant_ini= iniQp_p;
    prcParam->vop_quant_rc = prcParam->vop_quant_ini;
    
    prcParam->Rc = 0;
    
	*m4Qp_g = prcParam->vop_quant_rc;
	*m4Coded_g = prcParam->vop_coded_rc;
}

/***********************************************************************************
Description:
		updated the target bitrate 
		
Parameters:
		prcParam 						: 	the rc parameters structure.	
		m4RC_TargetBitRate_Channel_p	: 	Target bitrate, This is a user-defined parameter 
											in unit of kbit/sec. For example, if 64kbps, set 
											this variable as 64. If 256kbps, set this variable 
											as 256.
		vop_time_increment_p			: 	VOP time increment resolution, This is a user-defined 
											parameter of VOP time scale during one second for 
											specification the frame rate per second (fps). Here, 
											we suggest users to set this variable to 30.
		vop_time_increment_resolution_p	:	VOP time increment, This is a user-defined parameter 
											of time duration between two adjacent VOPs in unit of 
											VOP time scale (vop_time_increment_resolution_p).For 
											example, if fps 30, set vop_time_increment_p and 
											vop_time_increment_resolution_p to 1 and 30, respectively. 
											If fps 15, set set vop_time_increment_p and 
											vop_time_increment_resolution_p to 2 and 30, respectively.
		multiFrame						:	indicated the frame number for every rc 	

Returns:
		NULL
		
Remarks:
		
************************************************************************************/
void VIM_MPEG4_updatetargetbitratevars(	
					PVIM_MPEG4_RCParam	prcParam,
					UINT32			m4RC_TargetBitRate_Channel_p, 
		 			UINT32			vop_time_increment_p, 
		 			UINT32  		vop_time_increment_resolution_p,
					UINT32    		multiFrame)
{	
    prcParam->Rs = m4RC_TargetBitRate_Channel_p<<10; 	/*!< bitrate for the sequence */
    
    /*!<bits to be removed from the buffer per picture, Rp=Rs/Fs */
    prcParam->Rp = (prcParam->Rs)*vop_time_increment_p/vop_time_increment_resolution_p * multiFrame;
    prcParam->Bs = (prcParam->Rs)*0.5;                	/*!< buffer size */
    prcParam->Bs_thr_ini = (prcParam->Bs)*0.8;        	/*!< buffer upper threshold, Bs*0.8 */
    prcParam->Bs_thr_hi = prcParam->Bs_thr_ini;     	/*!< high level threshold */
    prcParam->Bs_thr_lo = (prcParam->Bs)*0.1;         	/*!< low level threshold */
    
    prcParam->buffer_fullness = (prcParam->Bs)>>1; 		/*!< initial buffer fullness = 1/2 * buffer size */
}

/***********************************************************************************
Description:
		updated the QP value 
		
Parameters:
		encSize_g		:	Encoding bit size, This variable is used to specify the total usage bits 
							when coding current frame (or VOP) in unit of bit.
		m4Typ_g			:	Encoding type, This variable is used to specify the coding type of current 
							frame (or VOP). For example, if I-VOP, set this variable to 0, if P- or
							B-VOP, set this variable to 1.
		m4Coded_g		:	vop_coded, A flag to inform MP4ENC the VOP is coded or not coded
		m4Qp_g			:	vop_quant, Quantization parameter (QP) for MP4 encoding VOPs.

Returns:
		NULL
		
Remarks:
		Called before every encoding frame
************************************************************************************/
void VIM_MPEG4_takeactiveonce(	
				PVIM_MPEG4_RCParam  prcParam,
				UINT32  		encSize_g,
				UINT32  		m4Typ_g,
				UINT32  		*m4Coded_g,
				UINT32  		*m4Qp_g)
{
	prcParam->Rc    = encSize_g; 			/*!< encSize_g should be sum of bits from the multi-frame encoding; */
	prcParam->nxt_vop_typ = m4Typ_g;
    prcParam->vop_coded_rc = *m4Coded_g;		
	prcParam->vop_quant_rc = *m4Qp_g;

	/*!<Added by Kata, 2004-06-18 (Kata_RC) */
    prcParam->cur_vop_typ = prcParam->nxt_vop_typ;
    
    prcParam->cur_B = prcParam->buffer_fullness;
    if(prcParam->cur_vop_typ == 0)
    	prcParam->cur_vop_coded = 1;
    else
    	prcParam->cur_vop_coded = prcParam->vop_coded_rc;
    prcParam->cur_vop_quant = prcParam->vop_quant_rc;

    prcParam->last_coded_Rc = prcParam->Rc;
    if(prcParam->cur_vop_coded) prcParam->last_Rc = prcParam->Rc;
    
    VIM_MPEG4_rc_framelevel(
    		prcParam,
            /*!< input */
            prcParam->Rs,
            prcParam->Rp,
            prcParam->Bs,
            prcParam->Bs_thr_hi,
            prcParam->Bs_thr_lo,
            prcParam->last_Rc,
            prcParam->last_coded_Rc,
            prcParam->cur_vop_typ,
            prcParam->cur_B,
            prcParam->cur_vop_coded,
            prcParam->cur_vop_quant,
            prcParam->no_frame_skipping,
           	prcParam->rc_fixed_Qp,
        	/*!< output */
        	&prcParam->nxt_B,
        	&prcParam->nxt_vop_coded,
        	&prcParam->nxt_vop_quant);

	prcParam->buffer_fullness = prcParam->nxt_B;
	prcParam->vop_coded_rc = prcParam->nxt_vop_coded;
	prcParam->vop_quant_rc = prcParam->nxt_vop_quant;

	*m4Coded_g = prcParam->vop_coded_rc;
	*m4Qp_g = prcParam->vop_quant_rc;
}
/************************* End of RC *************************/





//////////////////////////////tools end////////////////////////////////////////////////











/********************************************************************************
	Description:
		encode reset
	Parameters:
		void
	Note:
	Remarks:
*********************************************************************************/
void VIM_MPEG4_Reset(void)
{
	VIM_HIF_SetReg32(V5_REG_MP4ENC_RESET, 0);
	VIM_HIF_SetReg32(V5_REG_MP4DEC_RESET, 0);	
}
/*
	Description:
		Estimate
	Parameters:
		void
	Note:
	Remarks:
*/
#if (BOARD_TYPE==VC0820_FPGA)
UINT32 VIM_MPEG4_Estimate_I_Size(UINT32 width, UINT32 height,UINT32 bitrate)
{
return width*height/4;
}
#endif
/********************************************************************************
	Description:
		Start encode 
	Parameters:
		VIM_MPEG4_FIRSTVOP=0x01,
		VIM_MPEG4_NEXTVOP=0X2,
		VIM_MPEG4_LASTVOP=0X4
	FrameNum:
		一次压缩的桢率
	 	0~31
	Note:
	Remarks:
*********************************************************************************/
void VIM_MPEG4_StartEnc(VIM_MPEG4_VOPMODE VopMode,UINT8 FrameNum)
{
#if (BOARD_TYPE==VC0820_FPGA)
	VIM_HIF_SetReg32(V5_REG_MP4ENC_START, VopMode);
#else	
UINT16 Temp;
	VIM_HIF_SetReg32(V5_REG_MP4ENC_START, VopMode);
	Temp=VIM_HIF_GetReg16(V5_REG_MP4ENC_IM_FRAMECTL);
	if(FrameNum>1)
	{
		FrameNum--;
		FrameNum&=0x1f;
		Temp&=(0xc0);
		Temp|=FrameNum;
		if(VIM_MPEG4_LASTVOP==VopMode)
			Temp|=BIT5;
		VIM_HIF_SetReg16(V5_REG_MP4ENC_IM_FRAMECTL,Temp|0x100);
	}
#endif	
}
/********************************************************************************
	Description:
		Start encode 
	Parameters:
		VIM_MPEG4_DECNEXTVOP=0x02,
		VIM_MPEG4_NEXTBLOCK=0X4,
		VIM_MPEG4_LASTBLOCK=0XC,
		VIM_MPEG4_FIRSTBLOCK=0X7
		VIM_MPEG4_ONLYONEBLOCK=0Xf

	FrameNum:
		一次压缩的桢率
	 	0~31
	Note:
	Remarks:
*********************************************************************************/
void VIM_MPEG4_StartDec(VIM_MPEG4_DECMODE VopMode)
{
	VIM_HIF_SetReg32(V5_REG_MP4DEC_START, VopMode);
}
/********************************************************************************
	Description:
		设置一帧长度是8bit的还是32bit的。如果是32bit的，
		则其余的位置用什么数据填充Suff表示
	Parameters:
		BitLen
		VIM_MPEG4_32BIT=0x0100,
		VIM_MPEG4_8BIT=0X0,
		Suff:
			填充的8位数据
	Note:
	Remarks:
*********************************************************************************/
void VIM_MPEG4_SetBitLength(VIM_MPEG4_BITLENGTH BitLen,UINT8 Suff)
{
	VIM_HIF_SetReg32(V5_REG_MP4ENC_STUFFCODE, (UINT32)(BitLen|Suff));
}

/********************************************************************************
	Description:
		得到一帧的长度。
	Parameters:
		
	Note:
	Remarks:
*********************************************************************************/
UINT32 VIM_MPEG4_GetLength(UINT8 Num, UINT8 MaxFrame)
{
UINT32 temp;
	  MaxFrame=MaxFrame;
		temp=VIM_HIF_GetReg16(V5_REG_MP4ENC_FRAME_WORD_CNT)*4;
		if(gVc0578b_Info.Mpeg4Status.m_Enc_Stuff&0x100)
			return temp;
		temp=temp-(VIM_HIF_GetReg8(V5_REG_MP4ENC_FRAME_WORD_CNT+2)&0x3);
	return temp;
}
/********************************************************************************
	Description:
		设置解一个block数据的长度
	Parameters:
		
	Note:
	Remarks:
*********************************************************************************/
void VIM_MPEG4_SetLength(UINT32 Length)
{
	//VIM_HIF_SetReg32(V5_REG_MP4DEC_BITSTREAM_INI_ADD,gVc0578b_Info.MarbStatus.vdec_bitstream_buf);

	VIM_HIF_SetReg32(V5_REG_MP4DEC_BITSTREAM_SIZE,Length);
}
/********************************************************************************
	Description:
		Config vol size
	Parameters:
		wWidth:		vol image width to mp4 module
		wHeight:		vol image height to mp4 module
	Note:
	 	必须是16的倍数
	Remarks:
*********************************************************************************/
void VIM_MPEG4_SetVolSize(UINT16 wWidth, UINT16 wHeight)
{

	VIM_HIF_SetReg16(V5_REG_MP4ENC_WIDTH, wWidth);
	VIM_HIF_SetReg16(V5_REG_MP4ENC_HEIGHT, wHeight);

}
/********************************************************************************
	Description:
		Config vol size
	Parameters:
		wWidth:		vol image width to mp4 module
		wHeight:		vol image height to mp4 module
	Note:
	 	必须是16的倍数
	Remarks:
*********************************************************************************/
void VIM_MPEG4_SetDecVolSize(UINT16 wWidth, UINT16 wHeight)
{
UINT32	regburst0=0, regburst12=0;
//UINT8 framemode;

       wWidth=(wWidth+15)&0xfff0;
       wHeight=(wHeight+15)&0xfff0;
	VIM_HIF_SetReg8(V5_REG_MP4DEC_PINGPANG_MBL, (wHeight>>4));
	VIM_HIF_SetReg8(V5_REG_MP4DEC_PINGPANG_MBL+1, (wWidth>>4));
	
	VIM_HIF_SetReg16(V5_REG_LBUF_MP4DEC_IMAGESIZE, wWidth);
	VIM_HIF_SetReg16(V5_REG_LBUF_MP4DEC_IMAGESIZE+2, wHeight);
#if VIM_USER_MPEG4_DEC_FRAMEMODE
	regburst0	= (wWidth*wHeight)>>6;
	VIM_HIF_SetReg32(V5_REG_LBUF_BURSTNUMBER0,	regburst0);
#else
	regburst0	= wWidth>>2;
	regburst12	= ((wWidth*3)>>3)<<16;
	regburst12	|= wWidth>>3;
	VIM_HIF_SetReg32(V5_REG_LBUF_BURSTNUMBER0,	regburst0);
	VIM_HIF_SetReg32(V5_REG_LBUF_BURSTNUMBER12,regburst12);
#endif
	
/*
	temp=wWidth>>2;
 	VIM_HIF_SetReg16(V5_REG_LBUF_BURSTNUMBER0, temp);
 	temp=wWidth>>3;
 	VIM_HIF_SetReg16(V5_REG_LBUF_BURSTNUMBER12, temp);	
  	temp=(wWidth*3)>>3;	
   	VIM_HIF_SetReg16(V5_REG_LBUF_BURSTNUMBER12+2, temp);	*/
}
/***********************************************************************************
Description:
		 get  image size.
		
Parameters:
		psize	: address to  store image size.
		
Returns:
		void.

Remarks:
		image width must be the multiple of 16; image height must be even.
************************************************************************************/
void VIM_MPEG4_GetVolSize(PTSize psize)
{
	psize->cx	= (UINT16)VIM_HIF_GetReg16(V5_REG_MP4ENC_WIDTH);
	psize->cy	= (UINT16)VIM_HIF_GetReg16(V5_REG_MP4ENC_HEIGHT);
}
#if (BOARD_TYPE==VC0820_FPGA)
/********************************************************************************
* Description:
*	adjust pixel clk and hblank in display mode
* Parameters:

* Return:
.* Note:
* 
**********************************************************************************/
void VIM_MPEG4_AdjustClkBlank(void)
{
	UINT8  pixelrate;
	UINT32 temp;
	UINT16 blank;
	UINT16 vblank=0,D2Ddata=0;
#if 0	
  temp=(gVc0578b_Info.Mpeg4Status.DisInfo.SorWidth*gVc0578b_Info.Mpeg4Status.DisInfo.SorHeigth)>>15;
	if(temp>9)
		temp=9;
	pixelrate=0x0b-temp;
#else
 	temp=gVc0578b_Info.Mpeg4Status.DisInfo.SorWidth*gVc0578b_Info.Mpeg4Status.DisInfo.SorHeigth;
//	if(temp>=720*576)
//		temp=720*576;
//	else if(temp<=128*96)
//		temp=128*96;
//	temp/=128*96;
//	for(i=1;i<6;i++)
//      if(i*i>=temp)
//		 break;
	//pixelrate=17-2*i;
	
	if(temp>128*96){
		pixelrate=20;//13;// 2;//6;//10;//  
		blank= 60;//1000;//400;//60;//
	} else {
		pixelrate=20;
		blank=260;
	}
	vblank = 2;
	//D2Ddata=pixelrate-2;
	D2Ddata=VIM_HIF_GetReg8(V5_REG_LCDC_DEDLY+1);
	pixelrate=(pixelrate>=(D2Ddata+2)?pixelrate:(D2Ddata+2));
	//VIM_HIF_SetReg8(V5_REG_LCDC_DEDLY+1,D2Ddata);
#endif
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)	
	MMDPrintf("*******pixelrate=%d\n",pixelrate);
#endif
	VIM_JPEG_SetPixelRate(pixelrate);
#if(VIM_MID_API_DEBUG)&&(VIM_5XRDK_DEBUG)	
	MMDPrintf("*******blank=%d\n",blank);
#endif
	VIM_JPEG_SetLineBlank(blank);
	VIM_HIF_SetReg16(V5_REG_LBUF_VBLANK,(UINT32)vblank);
}
#endif
/***********************************************************************************
Description:
		 get  image size.
		
Parameters:
		psize	: address to  store image size.
		
Returns:
		void.

Remarks:
		image width must be the multiple of 16; image height must be even.
************************************************************************************/
void VIM_MPEG4_GetDecVolSize(PTSize psize)
{
	psize->cx	= (UINT16)VIM_HIF_GetReg16(V5_REG_LBUF_MP4DEC_IMAGESIZE);
	psize->cy	= (UINT16)VIM_HIF_GetReg16(V5_REG_LBUF_MP4DEC_IMAGESIZE+2);
}
#if (BOARD_TYPE==VC0820_FPGA)
/***********************************************************************************
Description:
		 get  image size.
		
Parameters:
		psize	: address to  store image size.
		
Returns:
		void.

Remarks:
		image width must be the multiple of 16; image height must be even.
************************************************************************************/
void VIM_MPEG4_GetDecSize(PTSize psize)
{
	psize->cx	= (UINT8)VIM_HIF_GetReg8(V5_REG_MP4DEC_PINGPANG_MBL+1);
	psize->cy	= (UINT8)VIM_HIF_GetReg8(V5_REG_MP4DEC_PINGPANG_MBL);
	psize->cx<<=4;
	psize->cy<<=4;
}
#endif

/********************************************************************************
	Description:
		设置一秒几帧
	Parameters:
		FrameRate:帧率 12bit valid
	1ms=99个clock
	Remarks:
*********************************************************************************/
#if (BOARD_TYPE==VC0820_FPGA)
#else
void VIM_MPEG4_SetFrameRate(UINT16 FrameRate)
{
	VIM_HIF_SetReg16(V5_REG_MP4ENC_TIMERESO, FrameRate);
#if(VIM_MPEG4_DEBUG)&&(VIM_5XRDK_ERROR)
		VIM_USER_PrintDec("[VC0578B][MPEG4] Fraemrate is",FrameRate);
#endif		
	FrameRate=1000/FrameRate;
	FrameRate*=99;
#if(VIM_MPEG4_DEBUG)&&(VIM_5XRDK_ERROR)
		VIM_USER_PrintDec("[VC0578B][MPEG4] after caculate FrameRate",FrameRate);
#endif	
	VIM_HIF_SetReg16(V5_REG_MP4ENC_IM_TIMECTRL+2, FrameRate);
}
#endif
/********************************************************************************
	Description:
		设置桢类型
	Parameters:
		VopType:i 帧， P帧
	Note:
	 	
	Remarks:
*********************************************************************************/
void VIM_MPEG4_SetVopType(VIM_MPEG4_VOPTYPE VopType)
{
UINT32 temp;
	temp=VIM_HIF_GetReg32(V5_REG_MP4ENC_VOP_CTRL2);
	temp&=(~BIT0);
	VIM_HIF_SetReg32(V5_REG_MP4ENC_VOP_CTRL2,temp|VopType);
}
/********************************************************************************
	Description:
		设置vop是否压缩
	Parameters:
		Enalbe:1 开，0关
	Note:
	 	
	Remarks:
*********************************************************************************/
void VIM_MPEG4_SetVopCoded(BOOL Enalbe)
{
UINT32 temp;
	temp=VIM_HIF_GetReg32(V5_REG_MP4ENC_VOP_CTRL2);
	temp&=(~BIT2);
	VIM_HIF_SetReg32(V5_REG_MP4ENC_VOP_CTRL2,(temp|(Enalbe<<2)));
}
/********************************************************************************
	Description:
		设置vop是否压缩的quantisation
	Parameters:
		Qt:0-31
	 	
	Remarks:
*********************************************************************************/
void VIM_MPEG4_SetVopQt(UINT8 Qt)
{
UINT32 temp;
	if(Qt>31)
		Qt=31;
	temp=VIM_HIF_GetReg32(V5_REG_MP4ENC_VOP_CTRL2);
	temp&=(~(BIT4|BIT5|BIT6|BIT7|BIT8));
	temp|=((Qt&0x1f)<<4);
	VIM_HIF_SetReg32(V5_REG_MP4ENC_VOP_CTRL2,(temp));
}
/********************************************************************************
	Description:
		设置是否使用每桢都拍的模式，还是硬件计算时间
	Parameters:
		void
	Remarks:
*********************************************************************************/
 void VIM_MPEG4_ClockInit(void)
{
#if (BOARD_TYPE==VC0820_FPGA)
#else
UINT16 Temp;
UINT32 Temp1;
	Temp1=VIM_HIF_GetHclock();
	Temp1=Temp1/100-1;
	Temp1&=0x7ff;
	Temp=VIM_HIF_GetReg16(V5_REG_MP4ENC_IM_TIMECTRL);
	Temp&=0xf800;
#if(VIM_MPEG4_DEBUG)&&(VIM_5XRDK_ERROR)
		VIM_USER_PrintDec("[VC0578B][MPEG4] VIM_MPEG4_ClockInit",Temp1);
#endif	
	VIM_HIF_SetReg16(V5_REG_MP4ENC_IM_TIMECTRL,Temp|Temp1);
#endif	
}
/********************************************************************************
  Description:
	set Mpeg4 mode
  Parameters:

  Returns:
  	void
  Remarks:
*********************************************************************************/
void VIM_MPEG4_SetMode(VIM_MPEG4_MODE bMode)
{
#if (BOARD_TYPE==VC0820_FPGA)	
UINT32 sysctrl=0,vol=0,header_ext=0;
UINT8 bLineMode;
#endif
UINT32 Temp,Temp1,Vop1=0;
UINT8 Wrapper=0;
	VIM_MPEG4_Reset();

	switch(bMode)
	{
#if (BOARD_TYPE==VC0820_FPGA)
		case VIM_MPEG4_ENCODE:
			sysctrl=0x42; 
			//Enable mv interrupt
			//fixedly reconstruct /reference buf (line buf 0 is used for reconstruct buf   line buf1 is reference buffer)
			// automatically reconstruct/refrence buffer select
			 // output big endian
			// disable external stall signal 
			//source image is not single field
			//enable encode finish interrrupt
			//enable vop finish interrrupt
			// source big endian
			if(!gVc0578b_Info.Mpeg4Status.m_Enc_OutBitStreamOrder)
				sysctrl&=0xffffffbf;//little endian
            if(!gVc0578b_Info.Mpeg4Status.m_Enc_SrcBitStreamOrder)	
				sysctrl&=0xfffffffd;//little endian	 

		    if(gVc0578b_Info.Mpeg4Status.m_Enc_Half_Pixsel_Estimation_Enable)
			   	Vop1|=0x80;
			   
			if(gVc0578b_Info.Mpeg4Status.m_Enc_Short_HeaderSurpport)
			{
				Vop1|=0x100;//must disable 4mv
				vol=0x6;//short enable,disable resync, h.263
				Vop1&=0xffffffb8;//must disable rounding, use dc vlc			
			}
			else
			{
		       	if(gVc0578b_Info.Mpeg4Status.m_Enc_4mv_Disable)
					Vop1|=0x100;
				if(gVc0578b_Info.Mpeg4Status.m_Enc_Rounding_Enable)
			   		Vop1|=0x40;
				if(gVc0578b_Info.Mpeg4Status.m_Enc_intra_dc_vlc)
					Vop1|=(gVc0578b_Info.Mpeg4Status.m_Enc_intra_dc_vlc&7);
				if(gVc0578b_Info.Mpeg4Status.m_Enc_ResyncMarker_Disable)
					vol|=2;
				else
				{
					if(gVc0578b_Info.Mpeg4Status.m_Enc_HeaderExt)
					{
						header_ext|=0x10000;
				      	header_ext|=gVc0578b_Info.Mpeg4Status.m_Enc_HeaderExtMaxPacketWordCnt;
					}					
				}
				
				if(gVc0578b_Info.Mpeg4Status.m_Enc_QuantType)
					vol|=1;
		
			}
			
			VIM_MPEG4_SetVopType(VIM_MPEG4_VOP_I);
			VIM_MPEG4_SetVopCoded(ENABLE);
			VIM_HIF_SetReg32(V5_REG_MP4ENC_RESYNC_CTRL,header_ext);
	        bLineMode=VIM_HIF_GetReg8(V5_REG_LBUF_MODE+3);
	        VIM_HIF_SetReg8(V5_REG_LBUF_MODE+3,bLineMode|BIT4);// 设成420 
            VIM_HIF_SetReg32(V5_REG_MP4ENC_STUFFCODE,gVc0578b_Info.Mpeg4Status.m_Enc_Stuff);
#if (VIM_USER_MPEG4_FRAMEMODE==0)
			Wrapper|=BIT7;// use line buf mode
#endif
			Wrapper|=BIT6;// use pingpang mode
			Wrapper&=(~BIT4);// use normal mode
	
			VIM_HIF_SetReg8(V5_REG_MP4ENC_WRAPPER_CTRL+3,Wrapper);// 设成420 
#if VIM_USER_MPEG4_FRAMEMODE
			VIM_IPP_SetMpeg4BufMode(VIM_IPP_MPG4_ONEFRAME);
#else
			VIM_IPP_SetMpeg4BufMode(VIM_IPP_MPG4_LINEBUF);
#endif
			//VIM_HIF_SetReg8(V5_REG_MP4ENC_IM_FRAMECTL+1,1);// one tick is one frame  cancel it for next im capture (get register 0)2007-5-30

			//set bit length
	              //VIM_MPEG4_SetBitLength(VIM_MPEG4_32BIT,0xff);
			
			VIM_HIF_SetReg32(V5_REG_MP4ENC_SYSCTRL,sysctrl);
			VIM_HIF_SetReg32(V5_REG_MP4ENC_VOL_CTRL,vol);
			VIM_HIF_SetReg32(V5_REG_MP4ENC_VOP_CTRL1,Vop1);
			break;
#else		
		case VIM_MPEG4_ENCODE:
			Temp=VIM_HIF_GetReg32(V5_REG_MP4ENC_SYSCTRL);
			Temp1=VIM_HIF_GetReg32(V5_REG_MP4ENC_VOL_CTRL);
			Temp&=(~BIT8);  //Enable mv interrupt
			//Temp|=(BIT7);  //fixedly reconstruct /reference buf (line buf 0 is used for reconstruct buf   line buf1 is reference buffer)
			Temp&=(~BIT7);  // automatically reconstruct/refrence buffer select
			Temp|=(BIT6);  // output big endian
			Temp&=(~BIT5);// disable external stall signal 
			Temp&=(~BIT4);//source image is not single field
			Temp&=(~BIT3);//enable encode finish interrrupt
			Temp&=(~BIT2);//enable vop finish interrrupt
			Temp|=(BIT1);// source big endian

			Temp1&=(~BIT2);//disable short header
			Temp1|=(BIT1);// disable resync marker
			Temp1|=(BIT0);// mpeg2 method

			Vop1|=BIT7;//enable half pixel me 
			Vop1|=BIT6;//enable rounding control
			Vop1&=(~BIT8);//enable 4mvmode
			VIM_MPEG4_SetVopType(VIM_MPEG4_VOP_I);
			VIM_MPEG4_SetVopCoded(ENABLE);
			VIM_HIF_SetReg32(V5_REG_MP4ENC_RESYNC_CTRL,0XFFFF);
			VIM_HIF_SetReg8(V5_REG_LBUF_MODE,(UINT8)1);// 设成420 
#if (VIM_USER_MPEG4_FRAMEMODE==0)
			Wrapper|=BIT7;// use line buf mode
#endif
			Wrapper|=BIT6;// use pingpang mode
			Wrapper|=BIT4;// use fifo mode
	
			VIM_HIF_SetReg8(V5_REG_MP4ENC_WRAPPER_CTRL+3,Wrapper);// 设成420 
#if VIM_USER_MPEG4_FRAMEMODE
			VIM_IPP_SetMpeg4BufMode(VIM_IPP_MPG4_ONEFRAME);
#else
			VIM_IPP_SetMpeg4BufMode(VIM_IPP_MPG4_LINEBUF);
#endif
			//VIM_HIF_SetReg8(V5_REG_MP4ENC_IM_FRAMECTL+1,1);// one tick is one frame  cancel it for next im capture (get register 0)2007-5-30
#if (BOARD_TYPE==VC0820_FPGA)
#else
			VIM_HIF_SetReg32(V5_REG_MP4ENC_IM_FRAMETYPE,0xfffffffc);//一桢i 剩下都是p
			VIM_HIF_SetReg32(V5_REG_MP4ENC_IM_FRAMECODE,0xfffffffe);//每桢都压

#endif
			//set bit length
			VIM_MPEG4_SetBitLength(VIM_MPEG4_32BIT,0xff);
			
			VIM_HIF_SetReg32(V5_REG_MP4ENC_SYSCTRL,Temp);
			VIM_HIF_SetReg32(V5_REG_MP4ENC_VOL_CTRL,Temp1);
			VIM_HIF_SetReg32(V5_REG_MP4ENC_VOP_CTRL1,Vop1);
			break;
#endif			
		case VIM_MPEG4_DECODE:
#if VIM_USER_MPEG4_VOPMODE 
			VIM_HIF_SetReg32(V5_REG_MP4DEC_FUNC,0x3A1);
#else
			VIM_HIF_SetReg32(V5_REG_MP4DEC_FUNC,0X1A1);
#endif
			VIM_HIF_SetReg32(V5_REG_MP4DEC_INT_MASK,0);
			VIM_HIF_SetReg32(V5_REG_MP4DEC_CHAN_NUM,0);	
#if(BOARD_TYPE==VC0820_FPGA)
#if VIM_USER_MPEG4_DEC_FRAMEMODE
			// vc820 vdec not support virtual fifo
			VIM_HIF_SetReg32(V5_REG_MP4DEC_PINGPANG_MBL,0x40000000);	
#else
			VIM_HIF_SetReg32(V5_REG_MP4DEC_PINGPANG_MBL,0xc0000000);	
#endif
#else
			VIM_HIF_SetReg32(V5_REG_MP4DEC_PINGPANG_MBL,0xd0000000);	
#endif		
			VIM_HIF_SetReg8(V5_REG_LBUF_MODE,(UINT8)0);// 设成422
#if VIM_USER_MPEG4_DEC_FRAMEMODE
			VIM_IPP_SetMpeg4BufMode(VIM_IPP_MPG4_ONEFRAME);
#else
			VIM_IPP_SetMpeg4BufMode(VIM_IPP_MPG4_LINEBUF);
#endif			
			
			break;
#if(BOARD_TYPE==VC0820_FPGA)
		case VIM_MPEG4_DECODE_LBUF:
			VIM_HIF_SetReg32(V5_REG_MP4DEC_FUNC,0x1a1);
			VIM_HIF_SetReg32(V5_REG_MP4DEC_INT_MASK,0);
			VIM_HIF_SetReg32(V5_REG_MP4DEC_CHAN_NUM,0);	
			VIM_HIF_SetReg32(V5_REG_MP4DEC_PINGPANG_MBL,0x60000000);
			VIM_HIF_SetReg8(V5_REG_LBUF_MODE,(UINT8)0);// 设成422
			VIM_HIF_SetReg32(V5_REG_MP4DEC_ERROR_LIMIT,0x7fffffff);
			VIM_IPP_SetMpeg4BufMode(VIM_IPP_MPG4_ONEFRAME);
			break;
#endif
		case VIM_MPEG4_VIDEOCON:
			Temp=VIM_HIF_GetReg32(V5_REG_MP4ENC_SYSCTRL);
			Temp1=VIM_HIF_GetReg32(V5_REG_MP4ENC_VOL_CTRL);
			Temp&=(~BIT8);  //Enable mv interrupt
			//Temp|=(BIT7);  //fixedly reconstruct /reference buf (line buf 0 is used for reconstruct buf   line buf1 is reference buffer)
			Temp&=(~BIT7);  // automatically reconstruct/refrence buffer select
			Temp|=(BIT6);  // output big endian
			Temp&=(~BIT5);// disable external stall signal 
			Temp&=(~BIT4);//source image is not single field
			Temp&=(~BIT3);//enable encode finish interrrupt
			Temp&=(~BIT2);//enable vop finish interrrupt
			Temp|=(BIT1);// source big endian

			Temp1&=(~BIT2);//disable short header
			Temp1|=(BIT1);// disable resync marker
			Temp1|=(BIT0);// mpeg2 method

			Vop1|=BIT7;//enable half pixel me 
			Vop1|=BIT6;//enable rounding control
			Vop1&=(~BIT8);//enable 4mvmode
			VIM_MPEG4_SetVopType(VIM_MPEG4_VOP_I);
			VIM_MPEG4_SetVopCoded(ENABLE);
			VIM_HIF_SetReg32(V5_REG_MP4ENC_RESYNC_CTRL,0XFFFF);
			VIM_HIF_SetReg8(V5_REG_LBUF_MODE,(UINT8)1);// 设成420 
			Wrapper|=BIT7;// use line buf mode
			Wrapper|=BIT6;// use pingpang mode
			Wrapper|=BIT4;// use fifo mode
	
			VIM_HIF_SetReg8(V5_REG_MP4ENC_WRAPPER_CTRL+3,Wrapper);// 设成420 
			VIM_IPP_SetMpeg4BufMode(VIM_IPP_MPG4_LINEBUF);
#if (BOARD_TYPE==VC0820_FPGA)
#else
			//VIM_HIF_SetReg8(V5_REG_MP4ENC_IM_FRAMECTL+1,1);// one tick is one frame  cancel it for next im capture (get register 0)2007-5-30
			VIM_HIF_SetReg32(V5_REG_MP4ENC_IM_FRAMETYPE,0xfffffffc);//一桢i 剩下都是p
			VIM_HIF_SetReg32(V5_REG_MP4ENC_IM_FRAMECODE,0xfffffffe);//每桢都压

#endif

			//set bit length
			VIM_MPEG4_SetBitLength(VIM_MPEG4_32BIT,0xff);
			
			VIM_HIF_SetReg32(V5_REG_MP4ENC_SYSCTRL,Temp);
			VIM_HIF_SetReg32(V5_REG_MP4ENC_VOL_CTRL,Temp1);
			VIM_HIF_SetReg32(V5_REG_MP4ENC_VOP_CTRL1,Vop1);

			//////dec////////////////////
			VIM_HIF_SetReg32(V5_REG_MP4DEC_FUNC,0X1A1);
			VIM_HIF_SetReg32(V5_REG_MP4DEC_INT_MASK,0);
			VIM_HIF_SetReg32(V5_REG_MP4DEC_CHAN_NUM,0);	
			VIM_HIF_SetReg32(V5_REG_MP4DEC_PINGPANG_MBL,0xd0000000);	
			
			VIM_HIF_SetReg8(V5_REG_LBUF_MODE,(UINT8)1);// 设成420 .decode fixed to 422
			VIM_IPP_SetMpeg4BufMode(VIM_IPP_MPG4_LINEBUF);
			break;
		case VIM_MPEG4_NOUSE:
			break;
	}
#if (BOARD_TYPE==VC0820_FPGA)
#else	
	VIM_MPEG4_ClockInit();
#endif

}

/********************************************************************************
	Description:
		取得中断标志
	Parameters:
		Qt:0-31
	 	
	Remarks:
*********************************************************************************/
 VIM_MPEG4_ENCINTFLAG VIM_MPEG4_GetEncIntFlag(void)
{
	UINT32 temp;

	temp=VIM_HIF_GetReg32(V5_REG_MP4ENC_INT);
	return (VIM_MPEG4_ENCINTFLAG)temp;
}
/********************************************************************************
	Description:
		取得中断标志
	Parameters:
		Qt:0-31
	 	
	Remarks:
*********************************************************************************/
 VIM_MPEG4_DECINTFLAG VIM_MPEG4_GetDecIntFlag(void)
{
UINT32 temp;

	temp=VIM_HIF_GetReg32(V5_REG_MP4DEC_INT);
	return (VIM_MPEG4_DECINTFLAG)temp;
}
/********************************************************************************
	Description:
		设置超时时间
	Parameters:
		frame:桢数
	 	
	Remarks:
*********************************************************************************/
 void VIM_MPEG4_SetExternTime(UINT16 Frame)
{
#if (BOARD_TYPE==VC0820_FPGA)
	VIM_HIF_SetReg32(V5_REG_MP4ENC_STUCK_LIMIT,0xffffffff);//FrameRate);
#else

UINT32 FrameRate;
	FrameRate=VIM_HIF_GetReg16(V5_REG_MP4ENC_TIMERESO);
	FrameRate=VIM_HIF_GetHclock()*1000/FrameRate;
#if(VIM_MPEG4_DEBUG)&&(VIM_5XRDK_ERROR)
		VIM_USER_PrintDec("[VC0578B][MPEG4] ONE FRAME HCLOCK IS",FrameRate);
#endif	
	FrameRate*=Frame;
	VIM_HIF_SetReg32(V5_REG_MP4ENC_STUCK_LIMIT,FrameRate);
#endif	
}
/********************************************************************************
	Description:
		设置是否使用每桢都拍的模式，还是硬件计算时间
	Parameters:
		Eenable:开
	 Note: 这个函数必须在最开始设好，拍摄过程中不能修改
	Remarks:
*********************************************************************************/
 void VIM_MPEG4_SetVsyCapEn(BOOL Eenable)
{
#if (BOARD_TYPE==VC0820_FPGA)
#else
	VIM_HIF_SetReg8(V5_REG_MP4ENC_IM_FRAMECTL,(Eenable<<6));
#endif
}
/********************************************************************************
	Description:
		设置定时器的时间
	Parameters:
		秒为单位，最大是0X1C1F 2个小时
	Remarks:
*********************************************************************************/
 void VIM_MPEG4_SetHwTimerInt(UINT16 TimeS)
{
#if (BOARD_TYPE==VC0820_FPGA)
#else
	VIM_HIF_SetReg16(V5_REG_MP4ENC_IM_TIMECNT,TimeS);
#endif	
}

/********************************************************************************
	Description:
		设置时间戳的信息
	Parameters:
		当前的ticks.
		返回ticks的初值
	Remarks:
*********************************************************************************/

UINT32 VIM_MPEG4_SetTimeAdd(UINT32 TimeTicks)
{
	UINT32 Time_Base,timeReso,Time_inc;
	timeReso=VIM_HIF_GetReg16(V5_REG_MP4ENC_TIMERESO);
	if(TimeTicks >= timeReso)
	{
		Time_Base = (TimeTicks/timeReso);
		Time_inc = TimeTicks%timeReso; //g_mp4e_timeInTick - g_mp4e_timeReso;
		TimeTicks = TimeTicks%timeReso;
		//if(VIM_USER_MPEG4_FRAMS>1)
			//Time_Base=0;
	}
	else
	{
		Time_Base = 0;
		Time_inc = TimeTicks;
	}		
#if(VIM_MPEG4_DEBUG)&&(VIM_5XRDK_ERROR)
		VIM_USER_PrintDec("[VC0578B][MPEG4] TimeTicks",TimeTicks);
		VIM_USER_PrintDec("[VC0578B][MPEG4] Time_Base",Time_Base);
#endif	
	VIM_HIF_SetReg16(V5_REG_MP4ENC_TIME_INC,Time_inc);
	
	Time_inc=VIM_HIF_GetReg32(V5_REG_MP4ENC_VOP_CTRL2);

	Time_inc &= 0xfffff1ff;
	Time_inc|=(Time_Base<<9); //0x8;

	VIM_HIF_SetReg16(V5_REG_MP4ENC_VOP_CTRL2,Time_inc);
	return TimeTicks;
}



/********************************************************************************
  Description:
  	interrupt of MPEG4
  Parameters:
  Returns:
  	void
  Remarks:
*********************************************************************************/


static 	const UINT8 gMPEG_EncIntPriority[VIM_MPEG4ENC_INT_NUM] = 
{ 
	VIM_INT_MPEG4_ENC_DONE
};
static 	const UINT8 gMPEG_DecIntPriority[VIM_MPEG4DEC_INT_NUM] = 
{ 
	VIM_INT_MPEG4_DEC_DONE
};
static 	const UINT8 gMPEG_TimerIntPriority[VIM_TIMER_INT_NUM] = 
{ 
	VIM_INT_MPEG4_HW_VSYN,VIM_INT_MPEG4_HW_TIMER
};
void _ISR_Mpeg4EncIntHandle(UINT8 wIntFlag)
{
	UINT8 i = 0;
	UINT8 bySecondLevelInt = 0;

	for(i=0; i<VIM_MPEG4ENC_INT_NUM; i++)
	{
		bySecondLevelInt = gMPEG_EncIntPriority[i];
		if(TSTINT(wIntFlag, bySecondLevelInt))
		{
			if(gVc0578b_Isr.Mpeg4isr[bySecondLevelInt])
			{
				gVc0578b_Isr.Mpeg4isr[bySecondLevelInt]();
			}
		}
	}
}
/********************************************************************************
  Description:
  	interrupt of mpeg4 deocd
  Parameters:
  Returns:
  	void
  Remarks:
*********************************************************************************/


 void _ISR_Mpeg4DecIntHandle(UINT8 wIntFlag)
{
	UINT8 i = 0;
	UINT8 bySecondLevelInt = 0;

	for(i=0; i<VIM_MPEG4DEC_INT_NUM; i++)
	{
		bySecondLevelInt = gMPEG_DecIntPriority[i];
		if(TSTINT(wIntFlag, bySecondLevelInt-VIM_MPEG4ENC_INT_NUM))
		{
			if(gVc0578b_Isr.Mpeg4isr[bySecondLevelInt])
			{
				gVc0578b_Isr.Mpeg4isr[bySecondLevelInt]();
			}
		}
	}	
}

/********************************************************************************
  Description:
  	interrupt of mpeg4 timer
  Parameters:
  Returns:
  	void
  Remarks:
*********************************************************************************/


 void _ISR_Mpeg4TimerIntHandle(UINT8 wIntFlag)
{
	UINT8 i = 0;
	UINT8 bySecondLevelInt = 0;

	for(i=0; i<VIM_TIMER_INT_NUM; i++)
	{
		bySecondLevelInt = gMPEG_TimerIntPriority[i];
		if(TSTINT(wIntFlag, bySecondLevelInt-VIM_MPEG4ENC_INT_NUM-VIM_MPEG4DEC_INT_NUM))
		{
			if(gVc0578b_Isr.Mpeg4isr[bySecondLevelInt])
			{
				gVc0578b_Isr.Mpeg4isr[bySecondLevelInt]();
			}
		}
	}	
}
/********************************************************************************
  Description:
  	register isr 
  Parameters:
  	bySecondLevelInt:
		VIM_INT_MPEG4_ENC_DONE		= 0,
		VIM_INT_MPEG4_DEC_DONE		= 1,
		VIM_INT_MPEG4_HW_VSYN		=2,
		VIM_INT_MPEG4_HW_TIMER		=3,
		VIM_MPEG4_INT_ALL		= (VIM_MPEG4ENC_INT_NUM+VIM_MPEG4DEC_INT_NUM+VIM_TIMER_INT_NUM)
  Returns:
  	void
  Remarks:
*********************************************************************************/

void VIM_MPEG4_RegisterISR(VIM_MPEG4_INT_TYPE bySecondLevelInt, PVIM_Isr  Isr)
{
UINT8 i;
	if(bySecondLevelInt>=VIM_MPEG4_INT_ALL)
	{
		for(i=0;i<VIM_MPEG4_INT_ALL;i++)
			gVc0578b_Isr.Mpeg4isr[i] = Isr;
		return;
	}
	gVc0578b_Isr.Mpeg4isr[bySecondLevelInt] = Isr;
}
/********************************************************************************
  Description:
  	enable jpeg interrupt
  Parameters:
  	bySecondLevelInt:
		VIM_INT_MPEG4_ENC_DONE		= 0,
		VIM_INT_MPEG4_DEC_DONE		= 1,
		VIM_INT_MPEG4_HW_VSYN		=2,
		VIM_INT_MPEG4_HW_TIMER		=3,
		VIM_MPEG4_INT_ALL		= (VIM_MPEG4ENC_INT_NUM+VIM_MPEG4DEC_INT_NUM+VIM_TIMER_INT_NUM)
  Returns:
  	void
  Remarks:
*********************************************************************************/
void VIM_MPEG4_SetIntEnable(VIM_MPEG4_INT_TYPE bySecondLevelInt,BOOL Eanble)
{
#if (BOARD_TYPE==VC0820_FPGA)
#else
	UINT8 temp;
#endif	
	UINT8 bInten;
	if(bySecondLevelInt<VIM_INT_MPEG4_DEC_DONE)
	{
		bInten=VIM_HIF_GetIntEnableSec(VM_HIF_INT_VENC);
		if(Eanble==ENABLE)
			bInten |= (0x1<<bySecondLevelInt);
		else
			bInten &= (~(0x1<<bySecondLevelInt));	
		VIM_HIF_SetIntEnableSec(VM_HIF_INT_VENC,bInten);
		if(bInten)
			VIM_HIF_SetIntModuleEn(VM_HIF_INT_VENC,ENABLE);
		else
			VIM_HIF_SetIntModuleEn(VM_HIF_INT_VENC,DISABLE);
	}
	else if(bySecondLevelInt<VIM_INT_MPEG4_HW_VSYN)
	{
		bInten=VIM_HIF_GetIntEnableSec(VM_HIF_INT_VDEC);
		if(Eanble==ENABLE)
			bInten |= (0x1<<(bySecondLevelInt-VIM_MPEG4ENC_INT_NUM));
		else
			bInten &= (~(0x1<<(bySecondLevelInt-VIM_MPEG4ENC_INT_NUM)));	
		VIM_HIF_SetIntEnableSec(VM_HIF_INT_VDEC,bInten);
		if(bInten)
			VIM_HIF_SetIntModuleEn(VM_HIF_INT_VDEC,ENABLE);
		else
			VIM_HIF_SetIntModuleEn(VM_HIF_INT_VDEC,DISABLE);
	}
	else if(bySecondLevelInt<VIM_MPEG4_INT_ALL)
	{
#if (BOARD_TYPE==VC0820_FPGA)
#else
		temp=VIM_HIF_GetReg8(V5_REG_MP4ENC_IM_TIMECTRL+1);
		bInten=VIM_HIF_GetIntEnableSec(VM_HIF_INT_HWT);
		if(Eanble==ENABLE)
		{
			bInten |= (0x1<<(bySecondLevelInt-VIM_MPEG4ENC_INT_NUM-VIM_MPEG4DEC_INT_NUM));
			temp |=( (0x1<<(bySecondLevelInt-VIM_MPEG4ENC_INT_NUM-VIM_MPEG4DEC_INT_NUM))<<4);
		}
		else
		{
			bInten &= (~(0x1<<(bySecondLevelInt-VIM_MPEG4ENC_INT_NUM-VIM_MPEG4DEC_INT_NUM)));	
			temp &= (~((0x1<<(bySecondLevelInt-VIM_MPEG4ENC_INT_NUM-VIM_MPEG4DEC_INT_NUM))<<4));	
		}
		temp|=BIT3;
		VIM_HIF_SetReg8(V5_REG_MP4ENC_IM_TIMECTRL+1,temp);
		VIM_HIF_SetIntEnableSec(VM_HIF_INT_HWT,bInten);
		if(bInten)
			VIM_HIF_SetIntModuleEn(VM_HIF_INT_HWT,ENABLE);
		else
			VIM_HIF_SetIntModuleEn(VM_HIF_INT_HWT,DISABLE);
#endif	
	}
	else
	{
#if (BOARD_TYPE==VC0820_FPGA)
		VIM_HIF_SetIntEnableSec(VM_HIF_INT_VENC,0xff);
		VIM_HIF_SetIntEnableSec(VM_HIF_INT_VDEC,0xff);
		VIM_HIF_SetIntModuleEn(VM_HIF_INT_VENC,ENABLE);
		VIM_HIF_SetIntModuleEn(VM_HIF_INT_VDEC,ENABLE);
#else	
		VIM_HIF_SetIntEnableSec(VM_HIF_INT_VENC,0xff);
		VIM_HIF_SetIntEnableSec(VM_HIF_INT_VDEC,0xff);
		VIM_HIF_SetIntEnableSec(VM_HIF_INT_HWT,0xff);
		VIM_HIF_SetIntModuleEn(VM_HIF_INT_VENC,ENABLE);
		VIM_HIF_SetIntModuleEn(VM_HIF_INT_VDEC,ENABLE);
		VIM_HIF_SetIntModuleEn(VM_HIF_INT_HWT,ENABLE);
#endif		
	}
}



