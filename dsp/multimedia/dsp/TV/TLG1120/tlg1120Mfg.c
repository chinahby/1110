/*****************************************************************************
* FILENAME
* tlg1120Mfg.c
*
*
* ABSTRACT
* This file contains Application Programmer's Interface library support for
* manufacturing floor tests of the TLG1120 Ultra Low Power NTSC/PAL
* Tuner/Decoder Front End.
*
* $Revision: 1.17 $
*
* (c) 2008 Telegent Systems
*****************************************************************************/
#include "tlgInclude.h"

#if TLG_CHIP_TYPE == TLG_CHIP_TYPE_TLG1120_1

#define USE_OLD_TFCALCODE

extern int tv_mode;
TLGDLL_API void TLG_FixAGC_1_1(uint32 base_addr, uint32 lna_mode);

extern TLGDLL_API int TLG_GetGains(uint32 base_addr, 
    int *p_lna1, 
    int *p_lna2, 
    int *p_agc1, 
    int *p_agc2, 
    int *p_agcd, 
    int *p_levl, 
    int *p_leva,
    uint16 *p_totalGain 
    );


TLGDLL_API int TLG_FreezeAgcLoops(uint32 base_addr, int minmax, tlg_statep state)
{
    uint16 regData;
    if (minmax == TLG_FREEZE_MAX)
    {
        
        
        TLG_SetBits(base_addr, REG_002E, 0x3b,
            REG_002E_BIT14_8_SHIFT,
            REG_002E_BIT14_8_MASK);
        
        TLG_SetBits(base_addr, REG_002F, 0x7F,
            REG_002F_BIT14_8_SHIFT,
            REG_002F_BIT14_8_MASK);
        
        TLG_SetBits(base_addr, REG_001D, 0x2F,
            REG_001D_BIT15_10_SHIFT,
            REG_001D_BIT15_10_MASK);
        
        TLG_SetBits(base_addr, REG_001C, 0x7F,
            REG_001C_BIT15_9_SHIFT,
            REG_001C_BIT15_9_MASK);
    } 
    else if (minmax == TLG_FREEZE_MIN)
    {
        
        uint16 gain_table;
        uint16 mingain_hi_lna;
        
        TLG_ReadReg(base_addr, REG_0031, &gain_table);
        switch (gain_table)
        {
            case 0:
                
                mingain_hi_lna = 0x4E;
                break;
            case 1:
                
                mingain_hi_lna = 0x36;
                break;
            case 2:
                
                mingain_hi_lna = 0x48;
                break;
            case 3:
            default:
                
                mingain_hi_lna = 0x5E;
                break;
        }
        
        TLG_SetBits(base_addr, REG_002E, 0x0,
            REG_002E_BIT14_8_SHIFT,
            REG_002E_BIT14_8_MASK);
        
        TLG_SetBits(base_addr, REG_002F, mingain_hi_lna,
            REG_002F_BIT14_8_SHIFT,
            REG_002F_BIT14_8_MASK);
        
        TLG_WriteReg(base_addr, REG_001D, 0);
        
        TLG_WriteReg(base_addr,REG_001C, 0);
    }
    else
    {
        uint16 gain_lo_lna;
        uint16 gain_hi_lna;
        uint16 gain_agc1;
        uint16 gain_agc2;
        

        
        TLG_ReadReg(base_addr, REG_0113, &regData);
        TLGHAL_GET(gain_lo_lna, regData,
            REG_0113_BIT14_8_SHIFT,
            REG_0113_BIT14_8_MASK);
        
        TLG_ReadReg(base_addr, REG_0114, &regData);
        TLGHAL_GET(gain_hi_lna, regData,
            REG_0114_BIT14_8_SHIFT,
            REG_0114_BIT14_8_MASK);
        
        TLG_ReadReg(base_addr, REG_010C, &regData);
        TLGHAL_GET(gain_agc1, regData,
            REG_010C_BIT13_8_SHIFT,
            REG_010C_BIT13_8_MASK);
        
        TLGHAL_GET(gain_agc2, regData,
            REG_010C_BIT6_0_SHIFT,
            REG_010C_BIT6_0_MASK);
        
        TLG_SetBits(base_addr, REG_002E, gain_lo_lna,
            REG_002E_BIT14_8_SHIFT,
            REG_002E_BIT14_8_MASK);
        
        TLG_SetBits(base_addr, REG_002F, gain_hi_lna,
            REG_002F_BIT14_8_SHIFT,
            REG_002F_BIT14_8_MASK);
        
        TLG_SetBits(base_addr, REG_001D, gain_agc1,
            REG_001D_BIT15_10_SHIFT,
            REG_001D_BIT15_10_MASK);
        
        TLG_SetBits(base_addr, REG_001C, gain_agc2,
            REG_001C_BIT15_9_SHIFT,
            REG_001C_BIT15_9_MASK);

    }
    
    
    TLG_SetBits(base_addr, REG_0030, 0x3,
        REG_0031_BIT15_14_SHIFT,
        REG_0031_BIT15_14_MASK);
    
    TLG_SetBits(base_addr, REG_001B, 1,
        REG_001B_BIT0_SHIFT,
        REG_001B_BIT0_MASK);
    
    TLG_SetBits(base_addr, REG_001B, 1,
        REG_001B_BIT1_SHIFT,
        REG_001B_BIT1_MASK);

	state[0] = TLG_STATE1;
    return(TLG_ERR_SUCCESS);
}


TLGDLL_API int TLG_RestoreAgcLoops(uint32 base_addr, tlg_statep state)
{
    if (state[0] != TLG_STATE1)
        return (TLG_ERR_PARAM);

    
    
    TLG_SetBits(base_addr, REG_0030, 0x0,
        REG_0031_BIT15_14_SHIFT,
        REG_0031_BIT15_14_MASK);
    
    TLG_SetBits(base_addr, REG_001B, 0,
        REG_001B_BIT0_SHIFT,
        REG_001B_BIT0_MASK);
    
    TLG_SetBits(base_addr, REG_001B, 0,
        REG_001B_BIT1_SHIFT,
        REG_001B_BIT1_MASK);
    return(TLG_ERR_SUCCESS);
}


TLGDLL_API int TLG_EnterAdcMode(uint32 base_addr, tlg_statep state)
{
    uint16 reg;

#ifdef TLG_SUPPORT_RUNTIME_CHECK
    {uint32 version; TLG_GetChipVersion(base_addr, &version);}
#endif 

    TLG_ReadReg(base_addr, REG_0098, &state[2]);
    reg = state[2];
    TLGHAL_SET(reg, REG_0098_VALUE_1, REG_0098_BIT3_0_SHIFT,
                                           REG_0098_BIT3_0_MASK);
    TLG_WriteReg(base_addr, REG_0098, reg);

    state[0] = TLG_STATE1;
    return(TLG_ERR_SUCCESS);
}


TLGDLL_API int TLG_ExitAdcMode(uint32 base_addr, tlg_statep state)
{
    if (state[0] != TLG_STATE1)
        return(TLG_ERR_PARAM);
    TLG_WriteReg(base_addr, REG_0000,        state[1]);
    TLG_WriteReg(base_addr, REG_0098,      state[2]);

    return(TLG_ERR_SUCCESS);
}



TLGDLL_API int TLG_GetTFCalCode(uint32 base_addr, uint16 *tfCalCode)
{
    uint32 cur_freq_hz ;

    TLG_GetCurrentFreq(base_addr, &cur_freq_hz);
    MSG_FATAL("TLG_GetTFCalCode %01d\n", cur_freq_hz,0,0);
    return TLG_GetCalCode(base_addr, tfCalCode, cur_freq_hz);

    return TLG_ERR_SUCCESS;
}

TLGDLL_API int TLG_GetCalCode_VHF1_2(uint32 base_addr, uint16 *tfCalCode)
{
    uint16          data;
    uint16          tf_dac;
    uint16          value = 0;
    
    
    TLG_ReadRegNS(base_addr,    REG_011C,   &data);
    TLGHAL_GET(tf_dac, data,
        REG_011C_BIT9_0_SHIFT,
        REG_011C_BIT9_0_MASK);
    {
        TLGHAL_SET(value, tf_dac,
            REG_01B8_BIT9_0_SHIFT,
            REG_01B8_BIT9_0_MASK );
        *tfCalCode = value;
    }
    return(TLG_ERR_SUCCESS);
}
TLGDLL_API int TLG_GetCalCode_UHF(uint32 base_addr, uint16 *tfCalCode)
{
    uint16          data;
    uint16          cgstune, citune, cal_override;
    uint16          value = 0;
    
    TLG_ReadRegNS(base_addr,    REG_0183,   &data);
    TLGHAL_GET(cgstune, data,
        REG_0183_BIT15_11_SHIFT,
        REG_0183_BIT15_11_MASK);
    TLGHAL_GET(citune, data,
        REG_0183_BIT4_0_SHIFT,
        REG_0183_BIT4_0_MASK);
    TLGHAL_GET(cal_override, data,
        REG_0183_BIT5_SHIFT,
        REG_0183_BIT5_MASK);
    {
        TLGHAL_SET(value, cgstune,
            REG_01B8_BIT15_11_SHIFT,
            REG_01B8_BIT15_11_MASK );
        TLGHAL_SET(value, citune,
            REG_01B8_BIT4_0_SHIFT,
            REG_01B8_BIT4_0_MASK );
        TLGHAL_SET(value, cal_override,
            REG_01B8_BIT5_SHIFT,
            REG_01B8_BIT5_MASK );
        *tfCalCode = value;
    }
    return(TLG_ERR_SUCCESS);
}

TLGDLL_API int TLG_GetCalCode_VHF3(uint32 base_addr, uint16 *tfCalCode)
{
    uint16          data;
    uint16          cin_code, cnotch;
    uint16          value = 0;

    TLG_ReadRegNS(base_addr,    REG_011C,   &data);
    TLGHAL_GET(cin_code, data,
        REG_011C_BIT9_1_SHIFT,
        REG_011C_BIT9_1_MASK);
    TLG_ReadRegNS(base_addr,    REG_016E_VALUE_1,   &data);
    MSG_FATAL("TLG_GetTFCalCode_VHF3 0x%04x\n",data,0,0);
    MSG_FATAL("REg170 = 0x%04x\n", data,0,0);
    TLGHAL_GET(cnotch, data,
        REG_016E_BIT9_1_SHIFT,
        REG_016E_BIT9_1_MASK);
    {
        TLGHAL_SET(value, cin_code,
            REG_01B8_BIT5_0_SHIFT,
            REG_01B8_BIT5_0_MASK );
        TLGHAL_SET(value, cnotch,
            REG_01B8_BIT15_6_SHIFT,
            REG_01B8_BIT15_6_MASK );
        *tfCalCode = value;
    }
    return(TLG_ERR_SUCCESS);
}


TLGDLL_API int TLG_GetCalCode(uint32 base_addr, uint16 *tfCalCode, uint32 cur_freq_hz)
{

    TLG_NULLPTR_CHECK(tfCalCode);
    if (cur_freq_hz < TLG_VHF3_CUTOFF_HZ)
    {
        
        TLG_GetCalCode_VHF1_2(base_addr, tfCalCode);
    }
    else if (cur_freq_hz > TLG_UHF_CUTOFF_HZ)
    {
        
        TLG_GetCalCode_UHF(base_addr, tfCalCode);
    }
    else
    {
        
        if (tv_mode == TLG_MODE_DVB_T)
        {
            TLG_GetCalCode_UHF(base_addr, tfCalCode);
        }
        else
        {
            TLG_GetCalCode_VHF3(base_addr, tfCalCode);
        }
    }
    return TLG_ERR_SUCCESS;
}



TLGDLL_API int TLG_GetPeakAgcGain(uint32 base_addr, uint16 *peakAgcGain)
{
    TLG_NULLPTR_CHECK(peakAgcGain);
    TLG_ReadRegNS(base_addr, REG_0123, peakAgcGain);
    return TLG_ERR_SUCCESS;
}



TLGDLL_API int TLG_GetTotalGain(uint32 base_addr, uint16 *ttlGain)
{
    int res = TLG_ERR_SUCCESS;
    
    int lna1, lna2, agc1, agc2, agcd, levl, leva;


    TLG_NULLPTR_CHECK(ttlGain);

#ifdef TLG_SUPPORT_RUNTIME_CHECK
    {
        uint32 version; 
        TLG_GetChipVersion(base_addr, &version);
    }
#endif 
    res = TLG_GetGains(base_addr, 
                &lna1, 
                &lna2, 
                &agc1, 
                &agc2, 
                &agcd, 
                &levl, 
                &leva,
                ttlGain );
    return(res);
}


int TLG_GetChromaLockStatus(void)
{ 
	uint16 locks;
	TLG_ReadRegNS(tlg_i2c_addr, 0x13B, &locks);
	
if (locks & (uint16)0x0004)
	return 1;
else
	return 0;
}
#endif 
