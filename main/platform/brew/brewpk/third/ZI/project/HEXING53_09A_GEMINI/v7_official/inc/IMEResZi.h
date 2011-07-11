/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2001
*
*****************************************************************************/

/*****************************************************************************
 *
 * Filename:
 * ---------
 *   IMEResZi.h
 *
 * Project:
 * --------
 *   Maui_Software
 *
 * Description:
 * ------------
 *   This is header file for Zi IME Resource for Multiple Bin.
 *
 * Author:
 * -------
 * Steven Wu (mtk01123)
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Log$
 *
 * Aug 9 2007 mtk01578
 * [MAUI_00530839] [MMI][Lang] New langauge and Input method for Ukraine
 * 
 *
 * Jul 16 2007 mtk01578
 * [MAUI_00417397] [MMI][InputMethod] solve compile error in enabling European langauge in Zi V7
 * 
 *
 * Apr 12 2007 mtk01578
 * [MAUI_00381457] [PC SIM] Remove compile option for PC Simulator
 * 
 *
 * Nov 3 2006 mtk01578
 * [MAUI_00340410] [MMI][InputMethod] CHeck in new language - Persian
 * 
 *
 * Aug 15 2006 mtk01123
 * [MAUI_00321616] [MMI][Inputmethod][checkin] Zi V7 inputmethod check-in
 * 
 *
 * Aug 14 2006 mtk01123
 * [MAUI_00321616] [MMI][Inputmethod][checkin] Zi V7 inputmethod check-in
 * 
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#ifndef _IME_Res_Zi_H_
#define _IME_Res_Zi_H_
// __CUSTPACK_MULTIBIN     Terry   BEGIN
#if defined __MMI_ZI_V7__
//PMT ZIV7 PATCH 20060616 START
#if defined __LARGE_CHINESE_DB_V7__
#if defined(__MMI_ZI_TR_CHINESE__) && defined(__MMI_ZI_SM_CHINESE__)
#include "Zi8DatZH_Pans_Large.h"
#elif defined(__MMI_ZI_TR_CHINESE__)
#include "Zi8DatZH_TW_Large.h"
#elif defined(__MMI_ZI_SM_CHINESE__)
#include "Zi8DatZH_Prc_Large.h"
#endif
#else
#if defined(__MMI_ZI_TR_CHINESE__) && defined(__MMI_ZI_SM_CHINESE__)
#include "Zi8DatZH_Pan.h"
#elif defined(__MMI_ZI_TR_CHINESE__)
#include "Zi8DatZH_TW.h"
#elif defined(__MMI_ZI_SM_CHINESE__)
#include "Zi8DatZH_Prc.h"
#endif
#endif
//PMT ZIV7 PATCH 20060616 END
#if defined(__MMI_ZI_PRC_ENGLISH__)
#include "Zi8DatENPRC.h"
#elif defined(__MMI_ZI_UK_ENGLISH__)
#include "Zi8DatENUK.h"
#elif defined(__MMI_ZI_AM_ENGLISH__)
#include "Zi8DatENAM.h"
#endif

#if defined(__MMI_ZI_RUSSIAN__)
#include "Zi8DatRU.h"
#endif

#if defined(__MMI_ZI_BULGARIAN__)
#include "Zi8DatBG.h"
#endif

#if defined(__MMI_ZI_UKRAINIAN__)
#include "Zi8DatUK.h"
#endif

#if defined(__MMI_ZI_CA_FRENCH__)
#include "Zi8DatFRCA.h"
#elif defined(__MMI_ZI_EU_FRENCH__)
#include "Zi8DatFREU.h"
#endif

#if defined(__MMI_ZI_GERMAN__)
#include "Zi8DatDE.h"
#endif

#if defined(__MMI_ZI_ITALIAN__)
#include "Zi8DatIT.h"
#endif

#if defined(__MMI_ZI_EU_SPANISH__)
#include "Zi8DatESEU.h"
#elif defined(__MMI_ZI_SA_SPANISH__)
#include "Zi8DatESSA.h"
#endif

#if defined(__MMI_ZI_THAI__)
#include "Zi8DatTH.h"
#endif

#if defined(__MMI_ZI_ARABIC__)
#include "Zi8DatAR.h"
#endif

//PMT START PERSIAN
#if defined(__MMI_ZI_PERSIAN__)
#include "Zi8DatFA.h"
#endif
//PMT END PERSIAN
#if defined(__MMI_ZI_HINDI__)
#include "Zi8DatHI.h"
#endif

#if defined(__MMI_ZI_HEBREW__)
#include "Zi8DatIW.h"
#endif

#if defined(__MMI_ZI_EU_PORTUGUESE__)
#include "Zi8DatPTEU.h"
#elif defined(__MMI_ZI_BZ_PORTUGUESE__)
#include "Zi8DatPTBZ.h"
#endif

#if defined(__MMI_ZI_TURKISH__)
#include "Zi8DatTR.h"
#endif

#if defined(__MMI_ZI_NORWEGIAN__)
#include "Zi8DatNO.h"
#endif

#if defined(__MMI_ZI_POLISH__)
#include "Zi8DatPL.h"
#endif

#if defined(__MMI_ZI_INDONESIAN__)
#include "Zi8DatIN.h"
#endif

#if defined(__MMI_ZI_MALAY__)
#include "Zi8DatMS.h"
#endif

#if defined(__MMI_ZI_VIETNAMESE__)
#include "Zi8DatVI.h"
#endif

#if defined(__MMI_ZI_SLOVAK__)
#include "Zi8DatSK.h"
#endif

#if defined(__MMI_ZI_DUTCH__)
#include "Zi8DatNL.h"
#endif

#if defined(__MMI_ZI_SWEDISH__)
#include "Zi8DatSV.h"
#endif

#if defined(__MMI_ZI_CROATIAN__)
#include "Zi8DatHR.h"
#endif

#if defined(__MMI_ZI_ROMANIAN__)
#include "Zi8DatRO.h"
#endif

#if defined(__MMI_ZI_SLOVENIAN__)
#include "Zi8DatSL.h"
#endif

#if defined(__MMI_ZI_GREEK__)
#include "Zi8DatEL.h"
#endif

#if defined(__MMI_ZI_CZECH__)
#include "Zi8DatCS.h"
#endif

#if defined(__MMI_ZI_FINNISH__)
#include "Zi8DatFI.h"
#endif

#if defined(__MMI_ZI_DANISH__)
#include "Zi8DatDA.h"
#endif

#if defined(__MMI_ZI_HUNGARIAN__)
#include "Zi8DatHU.h"
#endif

#else
#if defined(__MMI_ZI_TR_CHINESE__) && defined(__MMI_ZI_SM_CHINESE__)
#include "Zi8DatZH_Hk.h"
//#include "Zi8DatZH_Pans.h"//moci by yangdecai 
#elif defined(__MMI_ZI_TR_CHINESE__)
#include "Zi8DatZH_Hk.h"
#elif defined(__MMI_ZI_SM_CHINESE__)
#include "Zi8DatZH_Prc.h"
#endif

#if defined(__MMI_ZI_PRC_ENGLISH__)
#include "Zi8DatENPRC.h"
#elif defined(__MMI_ZI_UK_ENGLISH__)
#include "Zi8DatENUK.h"
#endif

#if defined(__MMI_ZI_RECOMMENDED_SIZE_RUSSIAN__)
#include "Zi8DatRU.h"
#elif defined(__MMI_ZI_RUSSIAN__)
#include "Zi8DatRUM.h"
#endif

#if defined(__MMI_ZI_BULGARIAN__)
#include "Zi8DatBG.h"
#endif

#if defined(__MMI_ZI_CA_FRENCH__)
#include "Zi8DatFRCA.h"
#elif defined(__MMI_ZI_EU_FRENCH__)
#include "Zi8DatFREU.h"
#endif

#if defined(__MMI_ZI_GERMAN__)
#include "Zi8DatDE.h"
#endif

#if defined(__MMI_ZI_ITALIAN__)
#include "Zi8DatIT.h"
#endif

#if defined(__MMI_ZI_EU_SPANISH__)
#include "Zi8DatESEU.h"
#elif defined(__MMI_ZI_SA_SPANISH__)
#include "Zi8DatESSA.h"
#endif

#if defined(__MMI_ZI_THAI__)
#include "Zi8DatTH.h"
#endif

#if defined(__MMI_ZI_ARABIC__)
#include "Zi8DatAR.h"
#endif

//PMT START PERSIAN
#if defined(__MMI_ZI_PERSIAN__)
#include "Zi8DatFA.h"
#endif
//PMT END PERSIAN
#if defined(__MMI_ZI_HINDI__)
#include "Zi8DatHI.h"
#endif

#if defined(__MMI_ZI_HEBREW__)
#include "Zi8DatIW.h"
#endif

#if defined(__MMI_ZI_EU_PORTUGUESE__)
#include "Zi8DatPTEU.h"
#elif defined(__MMI_ZI_SA_PORTUGUESE__)
#include "Zi8DatPTSA.h"
#endif

#if defined(__MMI_ZI_TURKISH__)
#include "Zi8DatTR.h"
#endif

#if defined(__MMI_ZI_NORWEGIAN__)
#include "Zi8DatNO.h"
#endif

#if defined(__MMI_ZI_POLISH__)
#include "Zi8DatPL.h"
#endif

#if defined(__MMI_ZI_INDONESIAN__)
#include "Zi8DatIN.h"
#endif

#if defined(__MMI_ZI_MALAY__)
#include "Zi8DatMS.h"
#endif

#if defined(__MMI_ZI_VIETNAMESE__)
#include "Zi8DatVI.h"
#endif

#if defined(__MMI_ZI_SLOVAK__)
#include "Zi8DatSKM.h"
#endif

#if defined(__MMI_ZI_DUTCH__)
#include "Zi8DatNLM.h"
#endif

#if defined(__MMI_ZI_SWEDISH__)
#include "Zi8DatSVM.h"
#endif

#if defined(__MMI_ZI_CROATIAN__)
#include "Zi8DatHRM.h"
#endif

#if defined(__MMI_ZI_ROMANIAN__)
#include "Zi8DatROM.h"
#endif

#if defined(__MMI_ZI_SLOVENIAN__)
#include "Zi8DatSLM.h"
#endif

#if defined(__MMI_ZI_GREEK__)
#include "Zi8DatELM.h"
#endif

#if defined(__MMI_ZI_CZECH__)
#include "Zi8DatCSM.h"
#endif

#if defined(__MMI_ZI_FINNISH__)
#include "Zi8DatFIM.h"
#endif

#if defined(__MMI_ZI_DANISH__)
#include "Zi8DatDAM.h"
#endif

#if defined(__MMI_ZI_HUNGARIAN__)
#include "Zi8DatHUM.h"
#endif
#endif


ZI8_LANGUAGE_ENTRY ZI8ROMDATA mtk_gIMELDBArray[] =
{
//Chinese
#if defined(__MMI_ZI_TR_CHINESE__) || defined(__MMI_ZI_SM_CHINESE__)
    {ZI8_LANG_ZH,     (void ZI8ROMPOINTER)&Zi8DatZH[0]},
#endif 
//English
#ifdef __MMI_ZI_V7__
#if defined(__MMI_ZI_PRC_ENGLISH__)
    {ZI8_LANG_EN,     (void ZI8ROMPOINTER)&Zi8DatENPRC[0]},
#elif defined(__MMI_ZI_UK_ENGLISH__)
    {ZI8_LANG_EN,     (void ZI8ROMPOINTER)&Zi8DatENUK[0]},
#endif
#else
#if defined(__MMI_ZI_PRC_ENGLISH__)
    {ZI8_LANG_EN,     (void ZI8ROMPOINTER)&Zi8DatENPRC30k[0]},
#elif defined(__MMI_ZI_UK_ENGLISH__)
    {ZI8_LANG_EN,     (void ZI8ROMPOINTER)&Zi8DatENUK30k[0]},
#endif
#endif

//Russian
#if defined(__MMI_ZI_V7__)
	#if defined(__MMI_ZI_RUSSIAN__)
	        {ZI8_LANG_RU,     (void ZI8ROMPOINTER)&Zi8DatRU[0]},
	#endif
#else
	#if defined(__MMI_ZI_RECOMMENDED_SIZE_RUSSIAN__)
	     {ZI8_LANG_RU,     (void ZI8ROMPOINTER)&Zi8DatRU[0]},
	#elif defined(__MMI_ZI_RUSSIAN__)
	        {ZI8_LANG_RU,     (void ZI8ROMPOINTER)&Zi8DatRUM[0]},
	#endif
#endif
   
#if defined(__MMI_ZI_BULGARIAN__)
     {ZI8_LANG_BG,     (void ZI8ROMPOINTER)&Zi8DatBG[0]},
#endif
#if defined(__MMI_ZI_UKRAINIAN__)
   {ZI8_LANG_UK,      (void ZI8ROMPOINTER)&Zi8DatUK[0]},
#endif
//French
#if defined(__MMI_ZI_CA_FRENCH__)
     {ZI8_LANG_FR,     (void ZI8ROMPOINTER)&Zi8DatFRCA[0]},
#elif defined(__MMI_ZI_EU_FRENCH__)
        {ZI8_LANG_FR,     (void ZI8ROMPOINTER)&Zi8DatFREU[0]},
#endif
   
//German
#if defined(__MMI_ZI_GERMAN__)
     {ZI8_LANG_DE,     (void ZI8ROMPOINTER)&Zi8DatDE[0]},
#endif
   
//Italian
#if defined(__MMI_ZI_ITALIAN__)
     {ZI8_LANG_IT,     (void ZI8ROMPOINTER)&Zi8DatIT[0]},
#endif
   
//Spanish
#if defined(__MMI_ZI_EU_SPANISH__)
     {ZI8_LANG_ES,     (void ZI8ROMPOINTER)&Zi8DatESEU[0]},
#elif defined(__MMI_ZI_SA_SPANISH__)
        {ZI8_LANG_ES,     (void ZI8ROMPOINTER)&Zi8DatESSA[0]},
#endif
   
//Thai
#if defined(__MMI_ZI_THAI__)
     {ZI8_LANG_TH,     (void ZI8ROMPOINTER)&Zi8DatTH[0]},
#endif

//Arabic
#if defined(__MMI_ZI_ARABIC__)
     {ZI8_LANG_AR,     (void ZI8ROMPOINTER)&Zi8DatAR[0]},
#endif

//PMT START PERSIAN
#if defined(__MMI_ZI_PERSIAN__)
     {ZI8_LANG_FA,     (void ZI8ROMPOINTER)&Zi8DatFA[0]},
#endif
//PMT END PERSIAN
#if defined(__MMI_ZI_HINDI__)
     {ZI8_LANG_HI,     (void ZI8ROMPOINTER)&Zi8DatHI[0]},
#endif

#if defined(__MMI_ZI_HEBREW__)
     {ZI8_LANG_IW,     (void ZI8ROMPOINTER)&Zi8DatIW[0]},
#endif

#if defined(__MMI_ZI_EU_PORTUGUESE__)
    {ZI8_LANG_PT,	(void ZI8ROMPOINTER)&Zi8DatPT[0]},
#elif defined(__MMI_ZI_SA_PORTUGUESE__)
    {ZI8_LANG_PT,	(void ZI8ROMPOINTER)&Zi8DatPTSA[0]},
#elif defined(__MMI_ZI_BZ_PORTUGUESE__)
    {ZI8_LANG_PT,	(void ZI8ROMPOINTER)&Zi8DatPTBZ[0]},
#endif

#if defined(__MMI_ZI_TURKISH__)
    {ZI8_LANG_TR,	(void ZI8ROMPOINTER)&Zi8DatTR[0]},
#endif
   
#if defined(__MMI_ZI_INDONESIAN__)
    {ZI8_LANG_IN,     (void ZI8ROMPOINTER)&Zi8DatIN[0]},
#endif
   
#if defined(__MMI_ZI_MALAY__)
    {ZI8_LANG_MS,     (void ZI8ROMPOINTER)&Zi8DatMS[0]},
#endif
   
#if defined(__MMI_ZI_VIETNAMESE__)
    {ZI8_LANG_VI,     (void ZI8ROMPOINTER)&Zi8DatVI[0]},
#endif
   
#if defined(__MMI_ZI_NORWEGIAN__)
    {ZI8_LANG_NO,     (void ZI8ROMPOINTER)&Zi8DatNO[0]},
#endif

#if defined(__MMI_ZI_POLISH__)
    {ZI8_LANG_PL,     (void ZI8ROMPOINTER)&Zi8DatPL[0]},
#endif

#if defined(__MMI_ZI_SLOVAK__)
    {ZI8_LANG_SK,     (void ZI8ROMPOINTER)&Zi8DatSK[0]},
#endif
 
#if defined(__MMI_ZI_DUTCH__)
    {ZI8_LANG_NL,     (void ZI8ROMPOINTER)&Zi8DatNL[0]},
#endif  
 
#if defined(__MMI_ZI_SWEDISH__)
    {ZI8_LANG_SV,     (void ZI8ROMPOINTER)&Zi8DatSV[0]},
#endif

#if defined(__MMI_ZI_CROATIAN__)
    {ZI8_LANG_HR,     (void ZI8ROMPOINTER)&Zi8DatHR[0]},
#endif

#if defined(__MMI_ZI_ROMANIAN__)
    {ZI8_LANG_RO,     (void ZI8ROMPOINTER)&Zi8DatRO[0]},
#endif 

 #if defined(__MMI_ZI_SLOVENIAN__)
    {ZI8_LANG_SL,     (void ZI8ROMPOINTER)&Zi8DatSL[0]}, 
#endif

#if defined(__MMI_ZI_GREEK__)
    {ZI8_LANG_EL,     (void ZI8ROMPOINTER)&Zi8DatEL[0]},
#endif

#if defined(__MMI_ZI_CZECH__)
    {ZI8_LANG_CS,     (void ZI8ROMPOINTER)&Zi8DatCS[0]},
#endif

#if defined(__MMI_ZI_FINNISH__)
    {ZI8_LANG_FI,     (void ZI8ROMPOINTER)&Zi8DatFI[0]},
#endif 


#if defined(__MMI_ZI_DANISH__)
#if defined(__MMI_ZI_V7__)
	{ZI8_LANG_DA,     (void ZI8ROMPOINTER)&Zi8DatDA[0]},
#else
    {ZI8_LANG_DA,     (void ZI8ROMPOINTER)&Zi8DatDAM[0]},
#endif
#endif

#if defined(__MMI_ZI_HUNGARIAN__)
    {ZI8_LANG_HU,     (void ZI8ROMPOINTER)&Zi8DatHU[0]},
#endif

    {0,0}
};
// __CUSTPACK_MULTIBIN     Terry   END

#endif /*IME_RES_ZI*/