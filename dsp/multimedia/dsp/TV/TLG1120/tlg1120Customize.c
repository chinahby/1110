#include "tlgInclude.h"
#include "tlgi2c.h"
#include "tlg1120api.h"
extern int tv_mode;
extern uint32           tlg_i2c_addr;
extern uint16           tlg_cur_gain;
extern uint16           tlg_cur_aud_mode;
// #define STROBE_CHECK
#define HOST_I2C	(1)
TLGDLL_API int TLG_StrobeCheck(uint32 reg_addr);

TLGDLL_API void TLGAPP_InitVideoAndAudio(void)
{
	/*
	 * BEGIN CUSTOM SECTION: Channel Mode
	 * This is the area where a customer sets up their default operating
	 * mode, video standard, bandwidth, etc.
	 */
	TLG_SetChnStandard(tlg_i2c_addr, TLG_VSTD_PAL_D);
	/*
	Also for all channel changes (in any country) set the filter bandwidth
	to 12MHz.
	(This masks a bug that will be fixed properly in Jupiter 1.2)
	*/


	TLG_SetChnBandwidth(tlg_i2c_addr, TLG_BW_8);

	/*
	 * BEGIN CUSTOM SECTION: Audio Out Mode
	 * Set up default audio settings here.
	     tlg_cur_gain     = 3;       
	   The new gain has new range 0 - 14 instead of 0 - 9
	 */
	tlg_cur_gain     = 3;       
	tlg_cur_aud_mode = TLG_DAC;

	if (tlg_cur_aud_mode != TLG_DAC)
		TLG_SetAudioSampleRate(tlg_i2c_addr, TLG_AUD_RATE_26_367KHZ);
	TLG_SetAudioMode(tlg_i2c_addr, tlg_cur_aud_mode); 
	TLG_SetAudioGain(tlg_i2c_addr, tlg_cur_aud_mode, tlg_cur_gain);
	TLG_Set_FM_AudioStereoMode(tlg_i2c_addr, TLG_FM_AUTO_DETECT);

	/* 
	* BEGIN CUSTOM SECTION: Video Out Mode
	* Set up default video settings here
	* This example sets up 320x240 NTSC 30fps
	* Other options include:
	* TLG_SetVGA_601_PAL()
	* TLG_SetVGA_601_NTSC()
	* TLG_Set240x240_LCD_NTSC()
	* TLG_Set240x240_LCD_PAL()
	*/
//	TLG_SetExtHSync(tlg_i2c_addr, 139, 858, 1, 19, 264, 525);
//    TLG_SetExtVSync(tlg_i2c_addr, 1, 19, 0, 0, 0, 0);

    TLG_SetExtHSync(tlg_i2c_addr, 145, 864, 1, 22, 313, 625);
	TLG_SetExtVSync(tlg_i2c_addr, 1, 22, 0, 0, 0, 0);


	TLG_Set601VideoOutClock (tlg_i2c_addr, TLG_NORMAL);//TLG_INVERT); add pan
	TLG_SetHDecimate        (tlg_i2c_addr, TLG_ON);


	
	/*
	Only use TLG_VID_LCD type
	*/
	TLG_SetVidOutType       (tlg_i2c_addr, TLG_VID_LCD);
	TLG_SetChromaLumaOrder  (tlg_i2c_addr, TLG_NORMAL);
	TLG_SetVSyncPolarity    (tlg_i2c_addr, TLG_ACTIVE_HIGH);
	TLG_SetHSyncPolarity    (tlg_i2c_addr, TLG_ACTIVE_HIGH);
	TLG_SetHsyncLowPower    (tlg_i2c_addr, TLG_OFF);
	/*
	* BEGIN CUSTOM SECTION: Antenna Initialization. This is where you set up
	* the antenna behaviour and perform any GPIO (or other) manipulations to
	* to intialize any LNA/switching devices.
	*/
	TLG_SetAntennaType(tlg_i2c_addr, TLG_ANTENNA_DEF);
	TLG_SetMobileVidEnhancement(tlg_i2c_addr,TLG_MVE_MANUAL,0x2c);//added pan 2010-01-05
	/* GPIO setup done here                                                 */

}

/*****************************************************************************
* TLGAPP_SetLnaGpio
* BEGIN CUSTOM SECTION: This procedure is called to perform LNA switching.
* It may be customized for a given build.
*****************************************************************************/

TLGDLL_API void TLGAPP_SetLnaGpio(uint32 hz)
{
    if (hz > TLG_UHF_CUTOFF_HZ)
    {
        if (tlg_cur_lna != TLG_LNA_UHF)
        {
            tlg_cur_lna = TLG_LNA_UHF;
            /** BEGIN CUSTOM SECTION: DO GPIO FOR UHF SWITCH HERE          **/
#ifdef ENABLE_FM_SWITCH
            TLG_REG_LOG((b, "\n\n\nTLGAPP_SetLnaGpio UHF\n\n\n\n"));
            TLG_SetGPIOOutput(tlg_i2c_addr, 0x20, 0x00);
            TLG_SetGPIOEnable(tlg_i2c_addr, 0x20, 0x20);
#endif /* ENABLE_FM_SWITCH */
        }

    } 
    else if (hz > TLG_VHF2_CUTOFF_HZ) 
    {
        if (tv_mode == TLG_MODE_DVB_T)
        {
            if (tlg_cur_lna != TLG_LNA_VHF3)
            {
                tlg_cur_lna = TLG_LNA_VHF3;
                /** BEGIN CUSTOM SECTION: DO GPIO FOR VHF3 SWITCH HERE         **/
                /*
                    Note that 1120 chip does not have DVB_T mode
                */
            }
        }
        else
        {
            if (tlg_cur_lna != TLG_LNA_VHF2)
            {
                tlg_cur_lna = TLG_LNA_VHF2;
                /** BEGIN CUSTOM SECTION: DO GPIO FOR VHF2 SWITCH HERE         **/
#ifdef ENABLE_FM_SWITCH
                TLG_REG_LOG((b, "\n\n\nTLGAPP_SetLnaGpio VHF3\n\n\n\n"));
                TLG_SetGPIOOutput(tlg_i2c_addr, 0x20, 0x00);
                TLG_SetGPIOEnable(tlg_i2c_addr, 0x20, 0x20);
#endif /* ENABLE_FM_SWITCH */
            }
        }
    } 
    else 
    {
        if (tlg_cur_lna != TLG_LNA_VHF1)
        {
            tlg_cur_lna = TLG_LNA_VHF1;
            /** BEGIN CUSTOM SECTION: DO GPIO FOR VHF1 SWITCH HERE         **/
#ifdef ENABLE_FM_SWITCH
            TLG_REG_LOG((b, "\n\n\nTLGAPP_SetLnaGpio VHF1\n\n\n\n"));
            TLG_SetGPIOOutput(tlg_i2c_addr, 0x20, 0x20);
            TLG_SetGPIOEnable(tlg_i2c_addr, 0x20, 0x20);
#endif /* ENABLE_FM_SWITCH */
        }
    }
}

#ifdef TLG_NEW_CALLBACK
extern p_tlg_channel_t(p_tlg_cur_map)     ; /* Current Default Channel Map    */
fp_call_back            pre_scan_call_back_func     = NULL;
void                   *pre_scan_call_back_param    = NULL;

fp_call_back            scan_call_back_func     = NULL;
void                   *scan_call_back_param    = NULL;

extern fp_call_back     test_call_back_func     ;
extern void            *test_call_back_param    ;

/*
    Call back functions:
    The TLGAPP_SetCallbackFunc function is used to register all call back
    functions.
    To add new call back function, use the following template and procedure:
        1) Create 2 new variables (replace XXX with the new name):
                fp_call_back    XXX_call_back_func      = NULL;
                void            *XXX_call_back_param    = NULL;
        2) Add new call back ID:
                #define CB_ID_XXX   3
        3) Add new entry to the tlg_call_back_arr[]:
            {
                CB_ID_NEW      ,
                "XXX call back"        ,
                &XXX_call_back_func    ,
                &XXX_call_back_param   
            },
        4) The call back function is ready to use:
            XXX_call_back_func(0, XXX_call_back_param);
        5) From the application:
            1) Create the call back function and the call back param:
                int __stdcall GUI_XXX_call_back_func(int num, void *param);
                tlg_cb_param_s  GUI_XXX_call_back_param;
            2) Register the call back function:
                 TLGAPP_SetCallbackFunc(
                    CB_ID_XXX,
                    GUI_XXX_call_back_func,
                    &GUI_XXX_call_back_param);

    HINT:
        In the call back function, insert the code to release the 
        control to the apps to make the apps more responsive in the
        multitasking environment.
        For example, in Windows, pump all pending messages inside the
        call back function with the following code:
        void PumpMessage()
        {
            MSG msg;
            while( (::PeekMessage( &msg, NULL, 0, 0, PM_REMOVE )) != 0)
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }
        }


*/
typedef struct tlg_call_back_arr_t
{
    uint32          call_back_id        ;   /* To identify the call back function */
    char            *call_back_id_str   ;   /* for debugging purpose, use NULL to reduce code size */
    fp_call_back    *p_call_back_func   ;   /* pointer to call back function */
    void            **p_param           ;   /* pointer to call back param */
} tlg_call_back_arr_s;

tlg_call_back_arr_s  tlg_call_back_arr[] =
{
    {
        CB_ID_TEST      ,
        "Test call back"        ,
        &test_call_back_func    ,
        &test_call_back_param   
    },
    {
        CB_ID_SCAN      ,
        "Scan call back"        ,
        &scan_call_back_func    ,
        &scan_call_back_param   
    },
    {
        CB_ID_PRE_SCAN      ,
        "Pre Scan call back"        ,
        &pre_scan_call_back_func    ,
        &pre_scan_call_back_param   
    },
    {
        CB_ID_NONE      ,
        NULL,
        NULL,
        NULL
    }
};

TLGDLL_API int TLGAPP_SetCallbackFunc(
    uint16              cb_id    ,
    fp_call_back        cb_func  ,
    void                *cb_param     )
{
    tlg_call_back_arr_s *p_cb_arr = tlg_call_back_arr;

    while (p_cb_arr && p_cb_arr->call_back_id_str != NULL)
    {
        if (p_cb_arr->call_back_id == cb_id)
        {
            *p_cb_arr->p_call_back_func = cb_func   ;
            *p_cb_arr->p_param          = cb_param  ;
            return(TLG_ERR_SUCCESS);
        }
        p_cb_arr++;
    }
    return(TLG_ERR_PARAM);
}
#endif /* TLG_NEW_CALLBACK */
/*****************************************************************************
* TLGAPP_ScanChanFound
* BEGIN CUSTOM SECTION: This procedure is called when TLGAPP_ScanChannels()
*                       has found a new channel.
* It may be customized for a given build.
*****************************************************************************/

TLGDLL_API void TLGAPP_ScanChanFound(int chn)
{
    /** BEGIN CUSTOM SECTION: Provide actions to be performed when a channel
     **                       has been found.                              **/

#ifdef TLG_NEW_CALLBACK
    if (p_tlg_cur_map[chn] CHN_S == 0)
        return;
    if (scan_call_back_func && scan_call_back_param)
    {
        tlg_cb_param_chn_s *p_prm = (tlg_cb_param_chn_s *) scan_call_back_param;
        p_prm->chn   = p_tlg_cur_map[chn] CHN_S;
        p_prm->hz    = p_tlg_cur_map[chn] HZ_S;
        p_prm->bw    = p_tlg_cur_map[chn] BW_S;
        p_prm->std   = p_tlg_cur_map[chn] STD_S;

        scan_call_back_func(0, scan_call_back_param);
    }
#endif /* TLG_NEW_CALLBACK */
}

/*****************************************************************************
* TLGAPP_PreScanChan
* BEGIN CUSTOM SECTION: This procedure is called before TLGAPP_ScanChannels()
*                       starts to scan a new channel.
* It may be customized for a given build.
*****************************************************************************/

TLGDLL_API void TLGAPP_PreScanChan(int chn)
{
    /** BEGIN CUSTOM SECTION: Provide actions to be performed before scannig a channel
    **/

#ifdef TLG_NEW_CALLBACK
    if (p_tlg_cur_map[chn] CHN_S == 0)
        return;
    if (pre_scan_call_back_func && pre_scan_call_back_param)
    {
        tlg_cb_param_chn_s *p_prm = (tlg_cb_param_chn_s *) pre_scan_call_back_param;
        p_prm->chn   = p_tlg_cur_map[chn] CHN_S;
        p_prm->hz    = p_tlg_cur_map[chn] HZ_S;
        p_prm->bw    = p_tlg_cur_map[chn] BW_S;
        p_prm->std   = p_tlg_cur_map[chn] STD_S;

        pre_scan_call_back_func(0, pre_scan_call_back_param);
    }
#endif /* TLG_NEW_CALLBACK */
}

/*****************************************************************************
* TLG_ClearRegShadow
*
* 
*
*
*****************************************************************************/
TLGDLL_API int TLG_ClearRegShadow(uint32 base_addr)
{
#ifdef TLG_USE_SHADOW_REGISTERS
    int i;
    for (i=0; i < TLG_MAX_REG_ADDR; i++)
        _TLG_shadow_reg_use[i] = TLG_OFF;
#endif /* TLG_USE_SHADOW_REGISTERS */
    return TLG_ERR_SUCCESS;
}
/*****************************************************************************
 * TLG_SetBits                                                        *
 ****************************************************************************/
/** Read, modify and write operation.
 *
 * Provide a simple way to modify some bits in the register 
 * without too much overhead.
 * Note that all parameters now are 32 bits to be able to support
 * 32 bit address from amber space
 *
 * @param base_addr i2c address for the device
 * @param reg_addr  register address to modify
 * @param new_val   new value to set
 * @param shift     starting bit to modify
 * @param mask      mask of the bits
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 * 
 ****************************************************************************/

TLGDLL_API int TLG_SetBits(
    uint32 base_addr    , 
    uint32 reg_addr     , 
    uint32 new_val      ,
    uint32 shift        ,
    uint32 mask
    )
{
    uint16 val;
	int ret;

    TLG_BOUNDS_CHECK(reg_addr, TLG_MAX_REG_ADDR);
#ifdef CHECK_STROBE
    ASSERT(TLG_StrobeCheck(reg_addr));
#endif /* CHECK_STROBE */

    /*
        READ 
    */
#ifdef TLG_USE_SHADOW_REGISTERS
x
    if (_TLG_shadow_reg_use[reg_addr] == TLG_ON)
    {
        val = _TLG_shadow_reg[reg_addr];
        TLG_REG_LOG((b, "READ(Cache) 0x%04x = 0x%04x\n", reg_addr, val));
    } 
    else
#endif /* TLG_USE_SHADOW_REGISTERS */
    {
        ret = TLG_ReadReg(
            base_addr, 
            reg_addr, 
            &val);
#ifdef TLG_USE_SHADOW_REGISTERS
        _TLG_shadow_reg[reg_addr]     = val;
#endif /* TLG_USE_SHADOW_REGISTERS */
        TLG_REG_LOG((b, "READ  0x%04x = 0x%04x\n", reg_addr, val));
    }
    /*
        MODIFY
    */
#define OLD_SET_BIT
#ifdef OLD_SET_BIT
    TLGHAL_SET(val, 
        new_val, 
        shift, 
        mask);
    /*
        WRITE
    */
    TLG_WriteReg(base_addr, reg_addr, val);
#ifdef TLG_USE_SHADOW_REGISTERS
    _TLG_shadow_reg[reg_addr]     = val;
    _TLG_shadow_reg_use[reg_addr] = TLG_ON;
#endif /* TLG_USE_SHADOW_REGISTERS */
    TLG_REG_LOG((b, "WRITE 0x%04x = 0x%04x\n", reg_addr, val));
#else /* ! OLD_SET_BIT */
    {
        uint16 read_val;
        read_val = val;
        TLGHAL_SET(val, new_val, shift, mask);
        /*
            WRITE
        */
        if (read_val != val)
        {
            ret = TLG_WriteReg(base_addr, reg_addr, val);
            TLG_REG_LOG((b, "WRITE 0x%04x = 0x%04x\n", reg_addr, val));
        }
#ifdef TLG_USE_SHADOW_REGISTERS
        _TLG_shadow_reg[reg_addr]     = val;
        _TLG_shadow_reg_use[reg_addr] = TLG_ON;
#endif /* TLG_USE_SHADOW_REGISTERS */
    }
#endif /* OLD_SET_BIT */

    return ret;//(TLG_ERR_SUCCESS);
}

/*****************************************************************************
 * TLG_ModifyBits                                                        *
 ****************************************************************************/
/** Read, modify and write operation.
 *
 * Provide a simple way to modify some bits in the register 
 * without too much overhead.
 * Note that all parameters now are 32 bits to be able to support
 * 32 bit address from amber space
 * Only write when the register value changed
 *
 * @param base_addr i2c address for the device
 * @param reg_addr  register address to modify
 * @param new_val   new value to set
 * @param shift     starting bit to modify
 * @param mask      mask of the bits
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 * 
 ****************************************************************************/

TLGDLL_API int TLG_ModifyBits(
    uint32 base_addr    , 
    uint32 reg_addr     , 
    uint32 new_val      ,
    uint32 shift        ,
    uint32 mask
    )
{
    uint16 val;

    TLG_BOUNDS_CHECK(reg_addr, TLG_MAX_REG_ADDR);

    /*
        READ 
    */
#ifdef TLG_USE_SHADOW_REGISTERS

    if (_TLG_shadow_reg_use[reg_addr] == TLG_ON)
    {
        val = _TLG_shadow_reg[reg_addr];
        //TLG_REG_LOG((b, "READ(Cache) 0x%04x = 0x%04x\n", reg_addr, val));
    } 
    else
#endif /* TLG_USE_SHADOW_REGISTERS */
    {
        TLG_ReadReg(
            base_addr, 
            reg_addr, 
            &val);
#ifdef TLG_USE_SHADOW_REGISTERS
        _TLG_shadow_reg[reg_addr]     = val;
#endif /* TLG_USE_SHADOW_REGISTERS */
        //TLG_REG_LOG((b, "READ  0x%04x = 0x%04x\n", reg_addr, val));
    }
    /*
        MODIFY
    */
    {
        uint16 read_val;
        read_val = val;
        TLGHAL_SET(val, new_val, shift, mask);
        /*
            WRITE
        */
        if (read_val != val)
        {
            TLG_WriteReg(base_addr, reg_addr, val);
           // TLG_REG_LOG((b, "WRITE 0x%04x = 0x%04x\n", reg_addr, val));
        }
#ifdef TLG_USE_SHADOW_REGISTERS
        _TLG_shadow_reg[reg_addr]     = val;
        _TLG_shadow_reg_use[reg_addr] = TLG_ON;
#endif /* TLG_USE_SHADOW_REGISTERS */
    }
    return(TLG_ERR_SUCCESS);
}

/*****************************************************************************
* TLG_ReadReg
*
* 
*
*
*****************************************************************************/
TLGDLL_API int TLG_ReadReg(uint32 base_addr, uint32 reg_addr, uint16 *val)
{
	int ret= TLG_ERR_SUCCESS;
    TLG_NULLPTR_CHECK(val);
    TLG_BOUNDS_CHECK(reg_addr, TLG_MAX_REG_ADDR);

#ifdef CHECK_STROBE
x
    ASSERT(TLG_StrobeCheck(reg_addr));
#endif /* CHECK_STROBE */

#ifdef TLG_USE_SHADOW_REGISTERS
x
    if (_TLG_shadow_reg_use[reg_addr] == TLG_ON)
    {
        *val = _TLG_shadow_reg[reg_addr];
        //TLG_REG_LOG((b, "READ(Cache) 0x%04x = 0x%04x\n", reg_addr, *val));
    } 
    else
#endif /* TLG_USE_SHADOW_REGISTERS */
    {
        ret = TLG_ReadRegNS(base_addr, reg_addr, val);
    }
    return ret;//(TLG_ERR_SUCCESS);
}


/*****************************************************************************
* TLG_ReadRegNS
*
* 
*
*
*****************************************************************************/

TLGDLL_API int TLG_ReadRegNS(uint32 base_addr, uint32 reg_addr, uint16 *val)
{
    TLG_ReadRegNS_NoLog(base_addr, reg_addr, val);
    //TLG_REG_LOG((b, "READ  0x%04x = 0x%04x\n", reg_addr, *val));

    return(TLG_ERR_SUCCESS);
}

/*****************************************************************************
* TLG_ReadReg
*
* 
*
*
*****************************************************************************/
TLGDLL_API int TLG_ReadRegNoLog(uint32 base_addr, uint32 reg_addr, uint16 *val)
{
    TLG_NULLPTR_CHECK(val);
    TLG_BOUNDS_CHECK(reg_addr, TLG_MAX_REG_ADDR);

#ifdef CHECK_STROBE
    ASSERT(TLG_StrobeCheck(reg_addr));
#endif /* CHECK_STROBE */

#ifdef TLG_USE_SHADOW_REGISTERS
    if (_TLG_shadow_reg_use[reg_addr] == TLG_ON)
    {
        *val = _TLG_shadow_reg[reg_addr];
    } else
#endif /* TLG_USE_SHADOW_REGISTERS */
    TLG_ReadRegNS_NoLog(base_addr, reg_addr, val);

    return(TLG_ERR_SUCCESS);
}

/*****************************************************************************
* TLG_ReadRegNS_NoLog
*
* 
*
*
*****************************************************************************/
TLGDLL_API int TLG_ReadRegNS_NoLog(uint32 base_addr, uint32 reg_addr, uint16 *val)
{
    TLG_NULLPTR_CHECK(val);
    TLG_BOUNDS_CHECK(reg_addr, TLG_MAX_REG_ADDR);

#ifdef CHECK_STROBE
    ASSERT(TLG_StrobeCheck(reg_addr));
#endif /* CHECK_STROBE */

    TLGI2C_ReadReg(base_addr, reg_addr, val);

#ifdef TLG_USE_SHADOW_REGISTERS
    _TLG_shadow_reg[reg_addr]     = *val;
    _TLG_shadow_reg_use[reg_addr] = TLG_ON;
#endif /* TLG_USE_SHADOW_REGISTERS */
    return(TLG_ERR_SUCCESS);
}

#ifdef CHECK_STROBE

TLGDLL_API int TLG_StrobeCheck(uint32 reg_addr)
{
    if (reg_addr == 0x0003)
        return(1);
    if ((reg_addr >= 0x0005) && (reg_addr <= 0x000f))
        return(1);
    if ((reg_addr >= 0x00c4) && (reg_addr <= 0x00cf))
        return(1);
    if ((reg_addr >= 0x0100) && (reg_addr <= 0x010b))
        return(1);
    return(0);
}
#endif /* CHECK_STROBE */

/*****************************************************************************
* TLG_WriteReg
*
* 
*
*
*****************************************************************************/
TLGDLL_API int TLG_WriteReg(uint32 base_addr, uint32 reg_addr, uint16 val)
{
	int ret=TLG_ERR_SUCCESS;

    ret = TLG_WriteRegNoLog(base_addr, reg_addr, val);
    //TLG_REG_LOG((b, "WRITE 0x%04x = 0x%04x\n", reg_addr, val));

    return ret;//(TLG_ERR_SUCCESS);
}
/*****************************************************************************
* TLG_ModifyReg
*
* 
*
*
*****************************************************************************/
TLGDLL_API int TLG_ModifyReg(uint32 base_addr, uint32 reg_addr, uint16 val)
{

    return TLG_WriteReg(base_addr, reg_addr, val);
}

/*****************************************************************************
* TLG_WriteRegNoLog
*
* 
*
*
*****************************************************************************/
TLGDLL_API int TLG_WriteRegNoLog(uint32 base_addr, uint32 reg_addr, uint16 val)
{
	int ret=TLG_ERR_SUCCESS;
    TLG_BOUNDS_CHECK(reg_addr, TLG_MAX_REG_ADDR);

#ifdef CHECK_STROBE
    ASSERT(TLG_StrobeCheck(reg_addr));
#endif /* CHECK_STROBE */
    ret = TLGI2C_WriteReg(base_addr, reg_addr, val);

#ifdef TLG_USE_SHADOW_REGISTERS
    _TLG_shadow_reg[reg_addr]     = val;
    _TLG_shadow_reg_use[reg_addr] = TLG_ON;
#endif /* TLG_USE_SHADOW_REGISTERS */

    return ret;//(TLG_ERR_SUCCESS);
}

/*****************************************************************************
* TLG_BurstWrite
*
* 
*
*
*****************************************************************************/
TLGDLL_API int TLG_BurstWrite(uint32 base_addr, uint32 start_addr, int len, uint16 *val)
{
		int ret=TLG_ERR_SUCCESS;

    TLG_NULLPTR_CHECK(val);
    TLG_BOUNDS_CHECK(start_addr, TLG_MAX_REG_ADDR);
    TLG_BOUNDS_CHECK(start_addr+len-1, TLG_MAX_REG_ADDR);

    ret = TLGI2C_WriteBurst(base_addr, start_addr, len, val);

#ifdef TLG_USE_SHADOW_REGISTERS
    {   
        int i;
        for (i = 0; i < len; i++)
        {
            _TLG_shadow_reg[start_addr+i]     = val[i];
            _TLG_shadow_reg_use[start_addr+i] = TLG_ON;
        }
    }
#endif /* TLG_USE_SHADOW_REGISTERS */
#ifdef TLG_DO_REG_LOGGING
    {   
        int i;
        for (i = 0; i < len; i++)
        {
            TLG_REG_LOG((b, "WRITE 0x%04x = 0x%04x\n", start_addr+i, val[i]));
        }
    }
#endif /* TLG_DO_REG_LOGGING */
            
    return ret;//(TLG_ERR_SUCCESS);
}

/*****************************************************************************
* TLG_BurstRead
*
* 
*
*
*****************************************************************************/
TLGDLL_API int TLG_BurstRead(uint32 base_addr, uint32 start_addr, int len, uint16 *val)
{
    TLG_NULLPTR_CHECK(val);
    TLG_BOUNDS_CHECK(start_addr, TLG_MAX_REG_ADDR);
    TLG_BOUNDS_CHECK(start_addr+len-1, TLG_MAX_REG_ADDR);

    TLGI2C_ReadBurst(base_addr, start_addr, len, val);

#ifdef TLG_USE_SHADOW_REGISTERS
    {   
        int i;
        for (i = 0; i < len; i++)
        {
            _TLG_shadow_reg[start_addr+i]     = val[i];
            _TLG_shadow_reg_use[start_addr+i] = TLG_ON;
        }
    }
#endif /* TLG_USE_SHADOW_REGISTERS */
#ifdef TLG_DO_REG_LOGGING
    {   
        int i;
        for (i = 0; i < len; i++)
        {
            TLG_REG_LOG((b, "READ  0x%04x = 0x%04x\n", start_addr+i, val[i]));
        }
    }
#endif /* TLG_DO_REG_LOGGING */

    return(TLG_ERR_SUCCESS);
}

#if 0
void TLG_Delay(uint16 milliseconds)
{
    int i, j;
    volatile int k;

    for (i=0; i < milliseconds; i++)
        for (j=0; j < 9000; j++)
            k = i+j;
#if 0
    /** BEGIN CUSTOM SECTION: Perform millisecond delay                    **/
    delayMS((uint16) milliseconds);
#endif
}
#endif

#if 0
void TLG_WriteLog(char *str)
{
    /** BEGIN CUSTOM SECTION: Write logging string                         **/
}
#endif
// sean@2008_12_22
#if AITCHIP_701U
extern void sys_IF_ait_delay1ms(u_int time);
#endif
#define AITCHIP_701U //added by jiajin 2009-12-29
#if AITCHIP_716
extern void AITC_SYS_BusyWaitMS(unsigned int ms);
#endif
void TLG_Delay(uint16 milliseconds)
{
#ifdef AITCHIP_701U
	#if 0
    sys_IF_ait_delay1ms(milliseconds);
	#else
	volatile unsigned int delay;
	for (delay = milliseconds * 5000; delay > 0; delay--);
	#endif
#endif
#if AITCHIP_716
    AITC_SYS_BusyWaitMS(milliseconds);
#endif        
}

void TLG_WriteLog(char *str)
{
    /** BEGIN CUSTOM SECTION: Write logging string                         **/
#if 0
	AIT_Message_P0(str);
#endif
}

// sean@2008_12_22 , Sensor/I2C function
extern unsigned short AITS_INIT_TLG1120(void); 
extern unsigned short AITS_SetSensorRegister(unsigned short reg, unsigned short val);
extern unsigned short AITS_GetSensorRegister(unsigned short reg) ;
/** Initialize I2C
 *
 * @fn int TLGI2C_Init(const char *DlpDllPath);
 *
 * This is a customer-written procedure that represents initializing their I2c
 * driver subsystem. It is not called by the TLG API and it is assumed the
 * customer code will call this direclty as part of their startup procedures.
 * It may have whatever calling conventions or format that is needed.
 */
DECLSPEC TLGI2C_Init(const char *DlpDllPath)
{
	//AITS_INIT_TLG1120();
}


/** Write a block of registers to TLG
 *
 * @fn int TLGI2C_WriteBurst(unsigned i2c_addr, unsigned start_addr, int length, unsigned short * data);
 *
 * This is a customer-supplied function that writes a block of registers starting 
 * at the specified register address on the TLG. IT IS CURRENTLY NOT CALLED
 * BY THE TLG API.
 *
 * @param i2c_addr      I2c address for TLG chip.
 * @param start_addr    Starting address of registers to write.
 * @param length        Number of registers to write.
 * @param data          Address of block of register values to write.
 *
 * @returns 
 * @todo returns

DECLSPEC TLGI2C_WriteBurst(unsigned  i2c_addr, unsigned  start_addr, int length, unsigned short * data)
{
	int i;
	unsigned short  tmp ;
	for(i=0;i<length;i++) {
#if HOST_I2C==1	
		tmp = data[i] ;
		Tlg_i2c_write_data(	i2c_addr,start_addr+i,&tmp);
#else
		AITS_SetSensorRegister( (unsigned short)(start_addr+i),data[i]);
#endif
	}
}
 */
/** Read a contiguous block of registers from the TLG.
 *
 * @fn int TLGI2C_ReadBurst(unsigned i2c_addr, unsigned start_addr, int length, unsigned short * data);
 *
 * This is a customer-supplied procedure that reads a block of registers starting 
 * at the specified register address on the TLG. IT IS CURRENTLY NOT CALLED
 * BY THE TLG API.
 *
 * @param i2c_addr      I2c address for TLG chip.
 * @param start_addr    Starting address of registers to write.
 * @param length        Number of registers to read.
 * @param data          Address of block of register values to write.
 * 

DECLSPEC TLGI2C_ReadBurst(unsigned i2c_addr, unsigned start_addr, int length, unsigned short * data)
{
	int i;
	unsigned short tmp ;
	for(i=0;i<length;i++) {
#if HOST_I2C==1	
		Tlg_i2c_read_data(i2c_addr,(start_addr+i),&tmp ) ;
		data[i] = tmp ;
#else
		data[i] = AITS_GetSensorRegister( (unsigned short)( (start_addr+i) | 0x8000) );		
#endif	
	}
}
  */
/** Write a register value to the TLG.
 *
 * @fn int TLGI2C_WriteReg(unsigned i2c_addr, unsigned reg_addr, unsigned short data);
 *
 * This customer-supplied procedure writes the specified register value at the 
 * specified register address on the TLG. 
 *
 * @param i2c_addr      I2c address for TLG chip.
 * @param reg_addr      Address of register to write.
 * @param data          Register value to write.
 * 

DECLSPEC TLGI2C_WriteReg(unsigned i2c_addr, unsigned reg_addr, unsigned short data)
{
#if HOST_I2C==1
	unsigned char ret ;
	unsigned short  tmp ;
	ret = Tlg_i2c_write_data(	i2c_addr,reg_addr,&data);
	//AIT_Message_P3("[%d]I2c.wr : 0x%04x -> 0x%04x\n",ret,reg_addr,data);
	//TLGI2C_ReadReg(i2c_addr,reg_addr,&tmp);
#else
	AITS_SetSensorRegister( (unsigned short)reg_addr,data);
#endif
}
 */

/** Read a register value from the TLG.
 *
 * @fn int TLGI2C_ReadReg(unsigned i2c_addr, unsigned reg_addr, unsigned short * data);
 *
 * This customer-supplied procedure will retrieve a register value direclty 
 * from the TLG. 
 *
 * @param i2c_addr      I2c address for TLG chip.
 * @param reg_addr      Register Address to retrieve.
 * @param data          Returned register value.
 * 
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.

DECLSPEC TLGI2C_ReadReg(unsigned i2c_addr, unsigned reg_addr, unsigned short * data)
{
#if HOST_I2C==1
	unsigned char ret ;
	ret = Tlg_i2c_read_data(i2c_addr,reg_addr,data);
	//AIT_Message_P3("[%d]I2c.rd :0x%04x -> 0x%04x\n",ret,reg_addr,*data);
#else
	*data = AITS_GetSensorRegister( (unsigned short)(reg_addr|0x8000) );
#endif	
}
 */

