#ifndef OEMQUERTKEY_H
#define OEMQUERTKEY_H
/*===========================================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:      OEMQuertkey.h
                 
 
SERVICES:  OEM Functions for Text Control support

 
DESCRIPTION: This file provides the data structures and prototypes of functions
that OEMs must provide in order to support the Text Control. This header file
shall be included by OEMTextCtl.c which provides sample implementation for
the functions mentioned here. The AEETextCtl API implementation makes down calls into
the functions mentioned here.


PUBLIC CLASSES:  Not Applicable


INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable


        Copyright ?1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //tsg/UI/main/TSGBREWOEM/inc/OEMText.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/01/08   bw      Build for T9

===========================================================================*/

/*===============================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===============================================================================*/

/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/

#define MAX_SHEFTKEYPAD_NUMBER 40
#define MAX_ARKEYPAD_NUMBER    8
#define MAX_HIKEYPAD_NUMBER    10
#define MAX_RUKEYPAD_NUMBER    10
#define MAX_MULTITAPS          10
typedef struct _Mya_character_type Mya_character_type;

struct _Mya_character_type
{
	uint16          wParam;
	uint16          wp[32];
	uint16          wsize;
};

#ifdef FEATURE_MT_MYANMRA

typedef enum
{   
	  /* DEVANAGARI SIGNS */
	MYNAMMAR_DEVANAGARI_S_DANDA		= 0x0964,
	MYNAMMAR_DEVANAGARI_S_DDANDA	= 0x0965,
			
    MYNAMMAR_C_KA   = 0x1000,
    MYNAMMAR_C_KHA  = 0x1001,
    MYNAMMAR_C_GA   = 0x1002,
    MYNAMMAR_C_GHA  = 0x1003,
    MYNAMMAR_C_NGA  = 0x1004,
    MYNAMMAR_C_CA   = 0x1005,
    MYNAMMAR_C_CHA  = 0x1006,
    MYNAMMAR_C_JA   = 0x1007,
    MYNAMMAR_C_JHA  = 0x1008,
    MYNAMMAR_C_NYA  = 0x1009,
    MYNAMMAR_C_NNYA = 0x100A,
    MYNAMMAR_C_TTA  = 0x100B,
    MYNAMMAR_C_TTHA = 0x100C,
    MYNAMMAR_C_DDA  = 0x100D,
    MYNAMMAR_C_DDHA = 0x100E,
    MYNAMMAR_C_NNA  = 0x100F,
    MYNAMMAR_C_TA   = 0x1010,
    MYNAMMAR_C_THA  = 0x1011,
    MYNAMMAR_C_DA   = 0x1012,
    MYNAMMAR_C_DHA  = 0x1013,
    MYNAMMAR_C_NA   = 0x1014,
    MYNAMMAR_C_PA   = 0x1015,
    MYNAMMAR_C_PHA  = 0x1016,
    MYNAMMAR_C_BA   = 0x1017,
    MYNAMMAR_C_BHA  = 0x1018,
    MYNAMMAR_C_MA   = 0x1019,
    MYNAMMAR_C_YA   = 0x101A,
    MYNAMMAR_C_RA   = 0x101B,
    MYNAMMAR_C_LA   = 0x101C,
    MYNAMMAR_C_WA   = 0x101D,
    MYNAMMAR_C_SA   = 0x101E,
    MYNAMMAR_C_HA   = 0x101F,	
    MYNAMMAR_C_LLA  = 0x1020,	
	
    MYNAMMAR_IV_A   = 0x1021,
    MYNAMMAR_IV_SA  = 0x1022,
    MYNAMMAR_IV_I   = 0x1023,
    MYNAMMAR_IV_II  = 0x1024,
    MYNAMMAR_IV_U   = 0x1025,
    MYNAMMAR_IV_UU  = 0x1026,
    MYNAMMAR_IV_E   = 0x1027,
    MYNAMMAR_IV_ME  = 0x1028,
    MYNAMMAR_IV_O   = 0x1029,
    MYNAMMAR_IV_AU  = 0x102A,	

	MYNAMMAR_DV_TAA = 0x102B,
    MYNAMMAR_DV_AA  = 0x102C,
    MYNAMMAR_DV_I   = 0x102D,
    MYNAMMAR_DV_II  = 0x102E,
    MYNAMMAR_DV_U   = 0x102F,
    MYNAMMAR_DV_UU  = 0x1030,
    MYNAMMAR_DV_E   = 0x1031,
    MYNAMMAR_DV_AI  = 0x1032,
    MYNAMMAR_DV_MON_II  = 0x1033,
    MYNAMMAR_DV_MON_O   = 0x1034,
    MYNAMMAR_DV_E_ABOVE = 0x1035,

	
    MYNAMMAR_S_ANUSVARA = 0x1036,
    MYNAMMAR_S_DOT      = 0x1037,
    MYNAMMAR_S_VISARGA  = 0x1038,
    MYNAMMAR_S_VIRAMA   = 0x1039,
    MYNAMMAR_S_ASAT     = 0x103A,
    
    MYNAMMAR_CS_YA      = 0x103B,
    MYNAMMAR_CS_RA      = 0x103C,
    MYNAMMAR_CS_WA      = 0x103D,
    MYNAMMAR_CS_HA      = 0x103E,
    
    MYNAMMAR_C_GREAT_SA = 0x103F,

    MYNAMMAR_D_ZERO  = 0x1040, /* MYNAMMAR digital 0 */
    MYNAMMAR_D_ONE   = 0x1041, /* MYNAMMAR digital 1 */
    MYNAMMAR_D_TWO   = 0x1042, /* MYNAMMAR digital 2 */
    MYNAMMAR_D_THREE = 0x1043, /* MYNAMMAR digital 3 */
    MYNAMMAR_D_FOUR  = 0x1044, /* MYNAMMAR digital 4 */
    MYNAMMAR_D_FIVE  = 0x1045, /* MYNAMMAR digital 5 */
    MYNAMMAR_D_SIX   = 0x1046, /* MYNAMMAR digital 6 */
    MYNAMMAR_D_SEVEN = 0x1047, /* MYNAMMAR digital 7 */
    MYNAMMAR_D_EIGHT = 0x1048, /* MYNAMMAR digital 8 */
    MYNAMMAR_D_NINE  = 0x1049, /* MYNAMMAR digital 9 */

    MYNAMMAR_S_LITTLE_SECTION = 0x104A,
    MYNAMMAR_S_SECTION        = 0x104B,
    
    MYNAMMAR_S_LOCATIVE  = 0x104C,
    MYNAMMAR_S_COMPLETED = 0x104D,
    MYNAMMAR_S_AFOREMENTONED = 0x104E,
    MYNAMMAR_S_GENITIVE      = 0x104F,
    
    MYNAMMAR_L_SHA = 0x1050,
    MYNAMMAR_L_SSA = 0x1051,
    MYNAMMAR_L_VR  = 0x1052,
    MYNAMMAR_L_VRR = 0x1053,
    MYNAMMAR_L_VL  = 0x1054,
    MYNAMMAR_L_VLL = 0x1055,
    MYNAMMAR_VS_VR = 0x1056,
    MYNAMMAR_VS_VRR = 0x1057,
    MYNAMMAR_VS_VL  = 0x1058,
    MYNAMMAR_VS_VLL = 0x1059,
    
    MYNAMMAR_L_M_NGA = 0x105A,
    MYNAMMAR_L_M_JHA = 0x105B,
    MYNAMMAR_L_M_BBA = 0x105C,
    MYNAMMAR_L_M_BBE = 0x105D,

    MYNAMMAR_CS_MON_MEDIALNA = 0x105E,
    MYNAMMAR_CS_MON_MEDIALMA = 0x105F,
    MYNAMMAR_CS_MON_MEDIALLA = 0x1060,
    
    MYNAMMAR_LS_KAREN_SHA         = 0x1061,
    MYNAMMAR_VS_SGAW_KAREN_EU     = 0x1062,
    MYNAMMAR_TM_SGAW_KAREN_HATHI  = 0x1063,
    MYNAMMAR_TM_SGAW_KAREN_KE_PHO = 0x1064,

    MYNAMMAR_L_WPK_THA = 0x1065,
    MYNAMMAR_L_WPK_PWA = 0x1066,
    
    MYNAMMAR_VS_WPK_EU = 0x1067,
    MYNAMMAR_VS_WPK_UE = 0x1068, 
    
    MYNAMMAR_S_WPK_TONE1 = 0x1069, 
    MYNAMMAR_S_WPK_TONE2 = 0x106A, 
    MYNAMMAR_S_WPK_TONE3 = 0x106B, 
    MYNAMMAR_S_WPK_TONE4 = 0x106C, 
    MYNAMMAR_S_WPK_TONE5 = 0x106D, 
    MYNAMMAR_L_EPK_NNA = 0x106E, 
    MYNAMMAR_L_EPK_YWA = 0x106F, 
    
    MYNAMMAR_L_EPK_GHWA     = 0x1070,
    
    MYNAMMAR_VS_GEBA_KAREN1 = 0x1071, 
    
    MYNAMMAR_VSK_OE = 0x1072, 
    MYNAMMAR_VSK_U  = 0x1073, 
    MYNAMMAR_VSK_EE = 0x1074, 
    
    MYNAMMAR_LS_KA  = 0x1075,
    MYNAMMAR_LS_KHA = 0x1076,
    MYNAMMAR_LS_GA  = 0x1077,
    MYNAMMAR_LS_CA  = 0x1078,
    MYNAMMAR_LS_ZA  = 0x1079,
    MYNAMMAR_LS_NYA = 0x107A,
    MYNAMMAR_LS_DA  = 0x107B,
    MYNAMMAR_LS_NA  = 0x107C,
    MYNAMMAR_LS_PHA = 0x107D,
    MYNAMMAR_LS_FA  = 0x107E,
    MYNAMMAR_LS_BA  = 0x107F,
    MYNAMMAR_LS_THA = 0x1080,
    MYNAMMAR_LS_HA  = 0x1081,
  
    MYNAMMAR_CSSM_WA = 0x1082, 
    MYNAMMAR_VSS_AA  = 0x1083, 
    MYNAMMAR_VSS_E   = 0x1084, 
    MYNAMMAR_VSS_EA  = 0x1085, 
    MYNAMMAR_VSS_FY  = 0x1086, 
    MYNAMMAR_SS_TONE2 = 0x1087, 
    MYNAMMAR_SS_TONE3 = 0x1088, 
    MYNAMMAR_SS_TONE5 = 0x1089, 
    MYNAMMAR_SS_TONE6 = 0x108A, 
    MYNAMMAR_SSC_TONE2 = 0x108B, 
    MYNAMMAR_SSC_TONE3 = 0x108C, 
    
    MYNAMMAR_SSC_EMPHATICTONE = 0x108D, 
    
    MYNAMMAR_LRP_FA    = 0x108E, 
    MYNAMMAR_SRP_TONE5 = 0x108F, 
    
    MYNAMMAR_SD_ZERO  = 0x1090, /* MYNAMMAR digital 0 */
    MYNAMMAR_SD_ONE   = 0x1091, /* MYNAMMAR digital 1 */
    MYNAMMAR_SD_TWO   = 0x1092, /* MYNAMMAR digital 2 */
    MYNAMMAR_SD_THREE = 0x1093, /* MYNAMMAR digital 3 */
    MYNAMMAR_SD_FOUR  = 0x1094, /* MYNAMMAR digital 4 */
    MYNAMMAR_SD_FIVE  = 0x1095, /* MYNAMMAR digital 5 */
    MYNAMMAR_SD_SIX   = 0x1096, /* MYNAMMAR digital 6 */
    MYNAMMAR_SD_SEVEN = 0x1097, /* MYNAMMAR digital 7 */
    MYNAMMAR_SD_EIGHT = 0x1098, /* MYNAMMAR digital 8 */
    MYNAMMAR_SD_NINE  = 0x1099, /* MYNAMMAR digital 9 */
    
    MYNAMMAR_SK_TONE1 = 0x109A, /* sign */
    MYNAMMAR_SK_TONE3 = 0x109B,
    
    MYNAMMAR_VSA_A  = 0x109C,
    MYNAMMAR_VSA_AI = 0x109D,
    MYNAMMAR_SS_ONE = 0x109E,
    MYNAMMAR_SS_EXCLAMATION = 0x109F
    
}Mynammar_characters_enum;

const Mya_character_type default_mynamar_multitap_strings[10] = 
{
    {AVK_0,{' ', MYNAMMAR_S_VIRAMA, MYNAMMAR_S_ANUSVARA, MYNAMMAR_S_DOT, MYNAMMAR_S_VISARGA, MYNAMMAR_S_ASAT, MYNAMMAR_CS_YA, MYNAMMAR_CS_RA, MYNAMMAR_CS_WA, MYNAMMAR_CS_HA, '.', MYNAMMAR_S_LITTLE_SECTION, MYNAMMAR_S_SECTION, MYNAMMAR_S_LOCATIVE, MYNAMMAR_S_COMPLETED, MYNAMMAR_S_AFOREMENTONED, MYNAMMAR_S_GENITIVE, MYNAMMAR_D_ZERO, '0'},19},  /* key 0 */
    {AVK_1,{MYNAMMAR_C_KA, MYNAMMAR_C_KHA, MYNAMMAR_C_GA, MYNAMMAR_C_GHA, MYNAMMAR_C_NGA, MYNAMMAR_D_ONE, '1'},7},        /* key 1 */
    {AVK_2,{MYNAMMAR_C_CA, MYNAMMAR_C_CHA, MYNAMMAR_C_JA, MYNAMMAR_C_JHA, MYNAMMAR_C_NYA, MYNAMMAR_C_NNYA, MYNAMMAR_D_TWO, '2'},8},        /* key 2 */
    {AVK_3,{MYNAMMAR_C_TTA, MYNAMMAR_C_TTHA, MYNAMMAR_C_DDA, MYNAMMAR_C_DDHA, MYNAMMAR_C_NNA, MYNAMMAR_D_THREE, '3'},7},  /* key 3 */
    {AVK_4,{MYNAMMAR_C_TA, MYNAMMAR_C_THA, MYNAMMAR_C_DA, MYNAMMAR_C_DHA, MYNAMMAR_C_NA, MYNAMMAR_D_FOUR, '4'}, 7},        /* key 4 */
    {AVK_5,{MYNAMMAR_C_PA, MYNAMMAR_C_PHA, MYNAMMAR_C_BA, MYNAMMAR_C_BHA, MYNAMMAR_C_MA, MYNAMMAR_D_FIVE, '5'}, 7},        /* key 5 */
    {AVK_6,{MYNAMMAR_C_YA, MYNAMMAR_C_RA, MYNAMMAR_C_LA, MYNAMMAR_C_WA, MYNAMMAR_C_SA, MYNAMMAR_D_SIX,  '6'}, 7},       /* key 6 */
    {AVK_7,{MYNAMMAR_C_HA, MYNAMMAR_C_LLA, MYNAMMAR_C_GREAT_SA, MYNAMMAR_D_SEVEN, '7'}, 5}, /* key 7 */
    {AVK_8,{MYNAMMAR_IV_A, MYNAMMAR_IV_SA, MYNAMMAR_IV_I, MYNAMMAR_IV_II, MYNAMMAR_IV_U, MYNAMMAR_IV_UU, MYNAMMAR_IV_E, MYNAMMAR_IV_ME, MYNAMMAR_IV_O, MYNAMMAR_IV_AU, MYNAMMAR_D_EIGHT, '8'},  12},        /* key 8 */
    {AVK_9,{MYNAMMAR_DV_TAA, MYNAMMAR_DV_AA, MYNAMMAR_DV_I, MYNAMMAR_DV_II, MYNAMMAR_DV_U, MYNAMMAR_DV_UU, MYNAMMAR_DV_E, MYNAMMAR_DV_AI, MYNAMMAR_DV_MON_II, MYNAMMAR_DV_MON_O, MYNAMMAR_DV_E_ABOVE, MYNAMMAR_D_NINE, '9'},13},                        /* key 9 */
};

#endif


typedef struct _SheftKeyItem_Own  SheftKeyItem_Own;
//typedef struct _UI_character_type UI_character_type ;
typedef struct _AR_character_type AR_character_type;
typedef struct _HI_character_type HI_character_type;
typedef struct _RU_character_type RU_character_type;
typedef struct _SE_character_type SE_character_type;

struct _SheftKeyItem_Own
{
	uint16          wParam;
	uint16          wp;
	AECHAR          name[1];
};
struct _AR_character_type   //add by yangdecai
{
	uint16                 wParam;
	uint16                 wp[10];
	uint16                 wsize;
};
struct _HI_character_type   
{
	uint16                 wParam;
	uint16                 wp[15];
	uint16                 wsize;
};
struct _RU_character_type   
{
	uint16                 wParam;
	uint16                 wp[10];
	uint16                 wsize;
};

struct _SE_character_type
{
	uint16          wParam;
	uint16          wp[5];
	uint16          wsize;
};

struct _UI_character_type
{
	uint16          wParam;
	uint16          wp[8];
	AECHAR          name[1];
};
#ifdef FEATURE_LANG_ARABIC
static AR_character_type  VLARCharKeyItem[MAX_ARKEYPAD_NUMBER] =
{
	#if defined(FEATURE_VERSION_N021)
	{AVK_2,	 {0x062B,0x0629,0x062A,0x0628,'2'},5},
	{AVK_3,  {0x0621,0x0626,0x0624,0x0649,0x0622,0x0625,0x0623,0x0627,'3'},9},
	{AVK_4,  {0x0634,0x0636,0x0635,0x0633,'4'},5},
	{AVK_5,  {0x0632,0x0631,0x0630,0x062F,'5'},5},
	{AVK_6,  {0x062E,0x062D,0x062C,'6'},4},
	{AVK_7,  {0x064A,0x0648,0x0647,0x0646,'7'},5},
	{AVK_8,  {0x0645,0x0644,0x0643,0x0642,0x0641,'8'},6},
	{AVK_9,  {0x063A,0x0639,0x0638,0x0637,'9'},5}
	#elif defined(FEATURE_VERSION_W0216A)
	{AVK_2,	 {0x0628,0x062A,0x0629,0x062B,'2'},5},
	{AVK_3,  {0x0627,0x0623,0x0622,0x0649,0x0624,0x0621,'3'},7},
	{AVK_4,  {0x0633,0x0634,0x0635,0x0636,'4'},5},
	{AVK_5,  {0x062F,0x0630,0x0631,0x0632,'5'},5},
	{AVK_6,  {0x062C,0x062D,0x062E,'6'},4},
	{AVK_7,  {0x0646,0x0647,0x0648,0x064A,'7'},5},
	{AVK_8,  {0x0641,0x0642,0x0643,0x0644,0x0645,'8'},6},
	{AVK_9,  {0x0637,0x0638,0x0639,0x063A,'9'},5}
    #elif defined(FEATURE_VERSION_C180)
    {AVK_2,	 {0x0628,0x062A,0x0629,0x062B,'2'},5},
	{AVK_3,  {0x0627,0x0623,0x0625,0x0622,0x0649,0x0624,0x0626,0x0621,'3'},9},
    {AVK_4,  {0x0633,0x0634,0x0635,0x0636,'4'},5},    
	{AVK_5,  {0x062F,0x0630,0x0631,0x0632,'5'},5},
	{AVK_6,  {0x062C,0x062D,0x062E,'6'},4},
	{AVK_7,  {0x0646,0x0647,0x0648,0x064A,'7'},5},
	{AVK_8,  {0x0641,0x0642,0x0643,0x0644,0x0645,'8'},6},
	{AVK_9,  {0x0637,0x0638,0x0639,0x063A,'9'},5}
    #elif defined(FEATURE_VERSION_C11)
    {AVK_2,	 {0x0628,0x062A,0x0629,0x062B,'2'},5},
	{AVK_3,  {0x0627,0x0623,0x0625,0x0622,0x0649,0x0624,0x0626,0x0621,'3'},9},
	{AVK_4,  {0x0633,0x0634,0x0635,0x0636,'4'},5},
	{AVK_5,  {0x062F,0x0630,0x0631,0x0632,'5'},5},
	{AVK_6,  {0x062C,0x062D,0x062E,'6'},4},
	{AVK_7,  {0x0646,0x0647,0x0648,0x064A,'7'},5},
	{AVK_8,  {0x0641,0x0642,0x0643,0x0644,0x0645,'8'},6},
	{AVK_9,  {0x0637,0x0638,0x0639,0x063A,'9'},5}
	#else
	{AVK_2,	 {0x0628,0x062A,0x0629,0x062B,'2'},5},
	{AVK_3,  {0x0627,0x0623,0x0625,0x0622,0x0649,0x0624,0x0626,0x0621,'3'},9},
	{AVK_4,  {0x0633,0x0635,0x0636,0x0634,'4'},5},
	{AVK_5,  {0x062F,0x0630,0x0631,0x0632,'5'},5},
	{AVK_6,  {0x062C,0x062D,0x062E,'6'},4},
	{AVK_7,  {0x0646,0x0647,0x0648,0x064A,'7'},5},
	{AVK_8,  {0x0641,0x0642,0x0643,0x0644,0x0645,'8'},6},
	{AVK_9,  {0x0637,0x0638,0x0639,0x063A,'9'},5}
	#endif
};

#endif
#if defined(FEATURE_LANG_HINDI)
static HI_character_type VLHICharKeyItem[MAX_HIKEYPAD_NUMBER] = 
{
    {AVK_1,  {0x0901, 0x0902, 0x0903, 0x094d, '1'}, 5}, /* key 1 */
    {AVK_2,  {0x093e, 0x093f, 0x0940, 0x0941, 0x0942, 0x0943, 0x0905, 0x0906, 0x0907, 0x0908, 0x0909, 0x090a, 0x090b, '2'}, 14},      /* key 2 */
    {AVK_3,  {0x0947, 0x0948, 0x0945, 0x094b, 0x094c, 0x0949, 0x0947, 0x0910, 0x090d, 0x0913, 0x0914, 0x0911, '3'}, 13},     /* key 3 */
    {AVK_4,  {0x0915, 0x0916, 0x0917, 0x0918, 0x0919, 0x0958, 0x0959, 0x095a, '4'}, 9},      /* key 4 */
    {AVK_5,  {0x091a, 0x091b, 0x091c, 0x091d, 0x091e, 0x095b, '5'}, 7},  /* key 5 */
    {AVK_6,  {0x091f, 0x0920, 0x0921, 0x0922, 0x0923, 0x095c, 0x095d, '6'}, 8},       /* key 6 */
    {AVK_7,  {0x0924, 0x0925, 0x0926, 0x0927, 0x0928, 0x0929, '7'}, 7},    /* key 7 */
    {AVK_8,  {0x092a, 0x092b, 0x092c, 0x092d, 0x092e, 0x095e, '8'}, 7},     /* key 8 */
    {AVK_9,  {0x092f, 0x0930, 0x0932, 0x0935, 0x0936, 0x0937, 0x0938, 0x0939, 0x0933, 0x0931, 0x0934, 0x095f, '9'}, 13},        /* key 9 */
    {AVK_0,  {' ', '0'}, 2}   /* key 0 */    
};
#endif

#if defined(FEATURE_LANG_RUSSIAN)
static RU_character_type VLRUCharKeyItem[MAX_RUKEYPAD_NUMBER] = 
{
    {AVK_1,{'.', ',', '?', '!', '1'},5},   /* key 1 */
    {AVK_2,{0x0410, 0x0411, 0x0412, 0x0413, '2', 'A', 'B', 'C'},8},   /* key 2 */
    {AVK_3,{0x0414, 0x0415, 0x0401, 0x0416, 0x0417, '3', 'D', 'E', 'F'},9},   /* key 3 */
    {AVK_4,{0x0418, 0x0419, 0x041a, 0x041b, '4', 'G', 'H', 'I'},8},   /* key 4 */
    {AVK_5,{0x041c, 0x041d, 0x041e, 0x041f, '5', 'J', 'K', 'L'},8},   /* key 5 */
    {AVK_6,{0x0420, 0x0421, 0x0422, 0x0423, '6', 'M', 'N', 'O'},8},   /* key 6 */
    {AVK_7,{0x0424, 0x0425, 0x0426, 0x0427, '7', 'P', 'Q', 'R', 'S'},9},  /* key 7 */
    {AVK_8,{0x0428, 0x0429, 0x042a, 0x042b, '8', 'T', 'U', 'V'},8},  /* key 8 */
    {AVK_9,{0x042c, 0x042d, 0x042e, 0x042f, '9', 'W', 'X', 'Y', 'Z'},9},   /* key 9 */
    {AVK_0,{' ', '0'},2},    /* key 0 */   
};
static RU_character_type VLRAPIDRUCharKeyItem[MAX_RUKEYPAD_NUMBER] = 
{
    {AVK_1,{'.', ',', '?', '!', '1'},5},   /* key 1 */
    {AVK_2,{0x0430, 0x0431, 0x0432, 0x0433, '2', 'a', 'b', 'c'},8},   /* key 2 */
    {AVK_3,{0x0434, 0x0435, 0x0451, 0x0436, 0x0437, '3', 'd', 'e', 'f'},9},   /* key 3 */
    {AVK_4,{0x0438, 0x0439, 0x043a, 0x043b, '4', 'g', 'h', 'i'},8},   /* key 4 */
    {AVK_5,{0x043c, 0x043d, 0x043e, 0x043f, '5', 'j', 'k', 'l'},8},   /* key 5 */
    {AVK_6,{0x0440, 0x0441, 0x0442, 0x0443, '6', 'm', 'n', 'o'},8},   /* key 6 */
    {AVK_7,{0x0444, 0x0445, 0x0446, 0x0447, '7', 'p', 'q', 'r', 's'},9},  /* key 7 */
    {AVK_8,{0x0448, 0x0449, 0x044a, 0x044b, '8', 't', 'u', 'v'},8},  /* key 8 */
    {AVK_9,{0x044c, 0x044d, 0x044e, 0x044f, '9', 'w', 'x', 'y', 'z'},9},   /* key 9 */
    {AVK_0,{' ', '0'},2},    /* key 0 */   
};

#endif

#if defined (FEATURE_VERSION_VG68)&&defined(FEATURE_LANG_THAI)
struct _TH_character_type
{
	uint16                 wParam;
	uint16                 wp[12];
	uint16                 wsize;
};

typedef struct _TH_character_type TH_character_type;
#define MAX_THKEYPAD_NUMBER 10

static TH_character_type VLTHCharKeyItem[MAX_THKEYPAD_NUMBER] = 
{
	{AVK_2,	 {0x0E01,0x0E02,0x0E03,0x0E04,0x0E05,0x0E06,0x0E07,0x0E08,0x0E09,'2'},10},
	{AVK_3,  {0x0E0A,0x0E0B,0x0E0C,0x0E0D,0x0E0E,0x0E0F,0x0E10,'3'},8},
	{AVK_4,  {0x0E11,0x0E12,0x0E13,0x0E14,0x0E15,0x0E16,0x0E17,'4'},8},
	{AVK_5,  {0x0E18,0x0E19,0x0E1A,0x0E1B,0x0E1C,0x0E1D,0x0E1E,'5'},8},
	{AVK_6,  {0x0E1F,0x0E20,0x0E21,0x0E22,0x0E23,0x0E25,0x0E27,'6'},8},
	{AVK_7,  {0x0E28,0x0E29,0x0E2A,0x0E2B,0x0E2C,0x0E2D,0x0E2E,'7'},8},
	{AVK_8,  {0x0E31,0x0E34,0x0E35,0x0E36,0x0E37,0x0E38,0x0E39,0x0E30,'8'},9},
	{AVK_9,  {0x0E32,0x0E33,0x0E40,0x0E41,0x0E42,0x0E43,0x0E44,0x0E46,0x0E24,'9'},10},
	{AVK_0,  {0x0E48,0x0E49,0x0E4A,0x0E4B,0x0E47,0x0E4C,0x0E39,0x0E30,'0'},9},
};
#endif

#ifdef FEATURE_LANG_THAI
static SheftKeyItem_Own  VLCharKeyItem[MAX_SHEFTKEYPAD_NUMBER] =
{
	{AVK_POUND, '#', {'#'}},
	{AVK_1, '1', {'1'}},
	{AVK_2, '2', {'2'}},
	{AVK_3, '3', {'3'}},
	{AVK_T, '(', {'('}},
	{AVK_Y, ')', {')'}},
	{AVK_U, '_', {'_'}},

	{AVK_I, '-', {'-'}},
	{AVK_O, '+', {'+'}},
	{AVK_P, '?', {'?'}},
	{AVK_STAR, '*', {'*'}},
	{AVK_4, '4', {'4'}},
	{AVK_5, '5', {'5'}},
	{AVK_6, '6', {'6'}},

	{AVK_G, '/', {'/'}},
	{AVK_H, ':', {':'}},
	{AVK_J, ';', {';'}},
	{AVK_K, 0x0027, {0x0027}},
	{AVK_L, 0x0022, {0x0022}},
	{AVK_Z,NULL,{NULL}},
	{AVK_7, '7', {'7'}},
	{AVK_8, '8', {'8'}},
	
	{AVK_9, '9', {'9'}},
	{AVK_B, '!', {'!'}},
	{AVK_N, ',', {','}},
	{AVK_M, '.', {'.'}},
	{AVK_RWD, '$', {'$'}},
	{AVK_MUTE,'@', {'@'}},
    {AVK_ENTER,0x000A,{0x000A}},
    
    {AVK_SPACE,' ',{' '}},
	{AVK_0,'0',{'0'}}
	
	
};
#else
#ifdef FEATURE_NUM_KEY_MID
static SheftKeyItem_Own  VLCharKeyItem[MAX_SHEFTKEYPAD_NUMBER] =
{
	{AVK_Q, '?', {'?'}},
	{AVK_W, '(', {'('}},
	{AVK_E, ')', {')'}},
	{AVK_1, '1', {'1'}},
	{AVK_2, '2', {'2'}},
	{AVK_3, '3', {'3'}},
	{AVK_STAR, '*', {'*'}},

	{AVK_I, '-', {'-'}},
	{AVK_O, '+', {'+'}},
	{AVK_P, '@', {'@'}},
	{AVK_A, '/', {'/'}},
	{AVK_S, ':', {':'}},
	{AVK_D, ';', {';'}},
	{AVK_4, '4', {'4'}},

	{AVK_5, '5', {'5'}},
	{AVK_6, '6', {'6'}},
	{AVK_POUND, '#', {'#'}},
	{AVK_K, 0x0027, {0x0027}},
	{AVK_L, 0x0022, {0x0022}},
	{AVK_Z, '!', {'!'}},
	{AVK_X, ',', {','}},
	
	{AVK_7, '7', {'7'}},
	{AVK_8, '8', {'8'}},
	{AVK_9, '9', {'9'}},
	{AVK_0, '0', {'0'}},
	{AVK_M, '$', {'$'}},
	{AVK_RWD, '.', {'.'}},
    {AVK_ENTER,0x000A,{0x000A}},
    
    {AVK_SPACE,' ',{' '}}
	
	
};
#else
static SheftKeyItem_Own  VLCharKeyItem[MAX_SHEFTKEYPAD_NUMBER] =
{
	{AVK_POUND, '#', {'#'}},
	{AVK_1, '1', {'1'}},
	{AVK_2, '2', {'2'}},
	{AVK_3, '3', {'3'}},
	{AVK_T, '(', {'('}},
	{AVK_Y, ')', {')'}},
	{AVK_U, '_', {'_'}},

	{AVK_I, '-', {'-'}},
	{AVK_O, '+', {'+'}},
	{AVK_P, '@', {'@'}},
	{AVK_STAR, '*', {'*'}},
	{AVK_4, '4', {'4'}},
	{AVK_5, '5', {'5'}},
	{AVK_6, '6', {'6'}},

	{AVK_G, '/', {'/'}},
	{AVK_H, ':', {':'}},
	{AVK_J, ';', {';'}},
	{AVK_K, 0x0027, {0x0027}},
	{AVK_L, 0x0022, {0x0022}},
	{AVK_7, '7', {'7'}},
	{AVK_8, '8', {'8'}},
	
	{AVK_9, '9', {'9'}},
	{AVK_V, '?', {'?'}},
	{AVK_B, '!', {'!'}},
	{AVK_N, ',', {','}},
	{AVK_M, '.', {'.'}},
	{AVK_RWD, '$', {'$'}},
    {AVK_ENTER,0x000A,{0x000A}},
    
    {AVK_SPACE,' ',{' '}},
	{AVK_0,'0',{'0'}}
	
	
};
#endif
#endif
#ifdef FEATURE_LANG_THAI

static SheftKeyItem_Own  VLCharLowKeyItem[MAX_SHEFTKEYPAD_NUMBER] =
{
    {AVK_POUND, 'q', {'q'}},
	{AVK_1, 'w', {'w'}},
	{AVK_2, 'e', {'e'}},
	{AVK_3, 'r', {'r'}},
	{AVK_T, 't', {'t'}},
	{AVK_Y, 'y', {'y'}},
	{AVK_U, 'u', {'u'}},

	{AVK_I, 'i', {'i'}},
	{AVK_O, 'o', {'o'}},
	{AVK_P, 'p', {'p'}},
	{AVK_STAR, 'a', {'a'}},
	{AVK_4, 's', {'s'}},
	{AVK_5, 'd', {'d'}},
	{AVK_6, 'f', {'f'}},

	{AVK_G, 'g', {'g'}},
	{AVK_H, 'h', {'h'}},
	{AVK_J, 'j', {'j'}},
	{AVK_K, 'k', {'k'}},
	{AVK_L, 'l', {'l'}},

	{AVK_Z, 'z', {'z'}},
	{AVK_7, 'x', {'x'}},
	{AVK_8, 'c', {'c'}},
	{AVK_9, 'v', {'v'}},

	{AVK_B, 'b', {'b'}},
	{AVK_N, 'n', {'n'}},
	{AVK_M, 'm', {'m'}},
	{AVK_RWD, '$', {'$'}},
	{AVK_MUTE,'@', {'@'}},
    {AVK_ENTER,0x000A,{0x000A}},
    
    {AVK_SPACE,' ',{' '}},
    {AVK_0,'0',{'0'}}
};
#else
#ifdef FEATURE_NUM_KEY_MID
static SheftKeyItem_Own  VLCharLowKeyItem[MAX_SHEFTKEYPAD_NUMBER] =
{
    {AVK_Q, 'q', {'q'}},
	{AVK_W, 'w', {'w'}},
	{AVK_E, 'e', {'e'}},
	{AVK_1, 'r', {'r'}},
	{AVK_2, 't', {'t'}},
	{AVK_3, 'y', {'y'}},
	{AVK_STAR, 'u', {'u'}},

	{AVK_I, 'i', {'i'}},
	{AVK_O, 'o', {'o'}},
	{AVK_P, 'p', {'p'}},
	{AVK_A, 'a', {'a'}},
	{AVK_S, 's', {'s'}},
	{AVK_D, 'd', {'d'}},
	{AVK_4, 'f', {'f'}},

	{AVK_5, 'g', {'g'}},
	{AVK_6, 'h', {'h'}},
	{AVK_POUND, 'j', {'j'}},
	{AVK_K, 'k', {'k'}},
	{AVK_L, 'l', {'l'}},
	{AVK_Z, 'z', {'z'}},
	{AVK_X, 'x', {'x'}},
	
	{AVK_7, 'c', {'c'}},
	
	{AVK_8, 'v', {'v'}},
	{AVK_9, 'b', {'b'}},
	{AVK_0, 'n', {'n'}},
	{AVK_M, 'm', {'m'}},
	{AVK_RWD, '.', {'.'}},
    {AVK_ENTER,0x000A,{0x000A}},
    
    {AVK_SPACE,' ',{' '}}   
};
#else
static SheftKeyItem_Own  VLCharLowKeyItem[MAX_SHEFTKEYPAD_NUMBER] =
{
    {AVK_POUND, 'q', {'q'}},
	{AVK_1, 'w', {'w'}},
	{AVK_2, 'e', {'e'}},
	{AVK_3, 'r', {'r'}},
	{AVK_T, 't', {'t'}},
	{AVK_Y, 'y', {'y'}},
	{AVK_U, 'u', {'u'}},

	{AVK_I, 'i', {'i'}},
	{AVK_O, 'o', {'o'}},
	{AVK_P, 'p', {'p'}},
	{AVK_STAR, 'a', {'a'}},
	{AVK_4, 's', {'s'}},
	{AVK_5, 'd', {'d'}},
	{AVK_6, 'f', {'f'}},

	{AVK_G, 'g', {'g'}},
	{AVK_H, 'h', {'h'}},
	{AVK_J, 'j', {'j'}},
	{AVK_K, 'k', {'k'}},
	{AVK_L, 'l', {'l'}},
	{AVK_7, 'z', {'z'}},
	{AVK_8, 'x', {'x'}},
	
	{AVK_9, 'c', {'c'}},
	
	{AVK_V, 'v', {'v'}},
	{AVK_B, 'b', {'b'}},
	{AVK_N, 'n', {'n'}},
	{AVK_M, 'm', {'m'}},
	{AVK_RWD, '$', {'$'}},
    {AVK_ENTER,0x000A,{0x000A}},
    
    {AVK_SPACE,' ',{' '}},
    {AVK_SPACE,'0',{'0'}}
};
#endif
#endif
#ifdef FEATURE_LANG_THAI
static SheftKeyItem_Own  VLCharCapKeyItem[MAX_SHEFTKEYPAD_NUMBER] =
{
    {AVK_POUND, 'Q', {'Q'}},
	{AVK_1, 'W', {'W'}},
	{AVK_2, 'E', {'E'}},
	{AVK_3, 'R', {'R'}},
	{AVK_T, 'T', {'T'}},
	{AVK_Y, 'Y', {'Y'}},
	{AVK_U, 'U', {'U'}},

	{AVK_I, 'I', {'I'}},
	{AVK_O, 'O', {'O'}},
	{AVK_P, 'P', {'P'}},
	{AVK_STAR, 'A', {'A'}},
	{AVK_4, 'S', {'S'}},
	{AVK_5, 'D', {'D'}},
	{AVK_6, 'F', {'F'}},

	{AVK_G, 'G', {'G'}},
	{AVK_H, 'H', {'H'}},
	{AVK_J, 'J', {'J'}},
	{AVK_K, 'K', {'K'}},
	{AVK_L, 'L', {'L'}},
	
	{AVK_Z, 'Z', {'Z'}},
	{AVK_7, 'X', {'X'}},
	{AVK_8, 'C', {'C'}},
	{AVK_9, 'V', {'V'}},

	{AVK_B, 'B', {'B'}},
	{AVK_N, 'N', {'N'}},
	{AVK_M, 'M', {'M'}},
	{AVK_RWD, '$', {'$'}},
	{AVK_MUTE,'@', {'@'}},
    {AVK_ENTER,0x000A,{0x000A}},
    
    {AVK_SPACE,' ',{' '}},
    {AVK_0,'0',{'0'}}
};
#else
#ifdef FEATURE_NUM_KEY_MID
static SheftKeyItem_Own  VLCharCapKeyItem[MAX_SHEFTKEYPAD_NUMBER] =
{
    {AVK_Q, 'Q', {'Q'}},
	{AVK_W, 'W', {'W'}},
	{AVK_E, 'E', {'E'}},
	{AVK_1, 'R', {'R'}},
	{AVK_2, 'T', {'T'}},
	{AVK_3, 'Y', {'Y'}},
	{AVK_STAR, 'U', {'U'}},

	{AVK_I, 'I', {'I'}},
	{AVK_O, 'O', {'O'}},
	{AVK_P, 'P', {'P'}},
	{AVK_A, 'A', {'A'}},
	{AVK_S, 'S', {'S'}},
	{AVK_D, 'D', {'D'}},
	{AVK_4, 'F', {'F'}},

	{AVK_5, 'G', {'G'}},
	{AVK_6, 'H', {'H'}},
	{AVK_POUND, 'J', {'J'}},
	{AVK_K, 'K', {'K'}},
	{AVK_L, 'L', {'L'}},
	{AVK_Z, 'Z', {'Z'}},
	{AVK_X, 'X', {'X'}},
	
	{AVK_7, 'C', {'C'}},
	
	{AVK_8, 'V', {'V'}},
	{AVK_9, 'B', {'B'}},
	{AVK_0, 'N', {'N'}},
	{AVK_M, 'M', {'M'}},
	{AVK_RWD, '.', {'.'}},
    {AVK_ENTER,0x000A,{0x000A}},
    
    {AVK_SPACE,' ',{' '}}  
};

#else
static SheftKeyItem_Own  VLCharCapKeyItem[MAX_SHEFTKEYPAD_NUMBER] =
{
    {AVK_POUND, 'Q', {'Q'}},
	{AVK_1, 'W', {'W'}},
	{AVK_2, 'E', {'E'}},
	{AVK_3, 'R', {'R'}},
	{AVK_T, 'T', {'T'}},
	{AVK_Y, 'Y', {'Y'}},
	{AVK_U, 'U', {'U'}},

	{AVK_I, 'I', {'I'}},
	{AVK_O, 'O', {'O'}},
	{AVK_P, 'P', {'P'}},
	{AVK_STAR, 'A', {'A'}},
	{AVK_4, 'S', {'S'}},
	{AVK_5, 'D', {'D'}},
	{AVK_6, 'F', {'F'}},

	{AVK_G, 'G', {'G'}},
	{AVK_H, 'H', {'H'}},
	{AVK_J, 'J', {'J'}},
	{AVK_K, 'K', {'K'}},
	{AVK_L, 'L', {'L'}},
	{AVK_7, 'Z', {'Z'}},
	{AVK_8, 'X', {'X'}},
	
	{AVK_9, 'C', {'C'}},
	
	{AVK_V, 'V', {'V'}},
	{AVK_B, 'B', {'B'}},
	{AVK_N, 'N', {'N'}},
	{AVK_M, 'M', {'M'}},
	{AVK_RWD, '$', {'$'}},
    {AVK_ENTER,0x000A,{0x000A}},
    
    {AVK_SPACE,' ',{' '}},  
    {AVK_SPACE,'0',{'0'}} 
};
#endif
#endif
#if 0 //change by xuhui
static SheftKeyItem_Own  VLCharArabicKeyItem[MAX_SHEFTKEYPAD_NUMBER] =
{
	{AVK_POUND, 0x0636, {0x0636}},
  	{AVK_1, 0x0635, {0x0635}},
  	{AVK_2, 0x062B, {0x062B}},
  	{AVK_3, 0x0642, {0x0642}},
  	{AVK_T, 0x0641, {0x0641}},
  	{AVK_Y, 0x063A, {0x063A}},
  	{AVK_U, 0x0639, {0x0639}},
  
  	{AVK_I, 0x062E, {0x062E}},
  	{AVK_O, 0x062D, {0x062D}},
  	{AVK_P, 0x062C, {0x062C}},
  	{AVK_STAR, 0x0634, {0x0634}},
  	{AVK_4, 0x0624, {0x0624}},
  	{AVK_5, 0x064A, {0x064A}},
  	{AVK_6, 0x0628, {0x0628}},
  
  	{AVK_G, 0x0644, {0x0644}},
  	{AVK_H, 0x0627, {0x0627}},
  	{AVK_J, 0x0629, {0x0629}},
  	{AVK_K, 0x0646, {0x0646}},
  	{AVK_L, 0x0645, {0x0645}},
  	{AVK_7, 0x0626, {0x0626}},
  	{AVK_8, 0x0630, {0x0630}},
  	
  	{AVK_9, 0x0647, {0x0647}},
  	{AVK_V, 0x0631, {0x0631}},
  	{AVK_B, 0x0621, {0x0621}},
  	{AVK_N, 0x0649, {0x0649}},
  	{AVK_M, 0x0643, {0x0643}},
  	{AVK_RWD, 0x061F, {0x061F}},
    {AVK_ENTER,0x000A,{0x000A}},
      
    {AVK_SPACE,' ',{' '}},
	{AVK_0,0x0637,{0x0637}}
};
#else
//效准阿拉伯语键盘对应值
static SheftKeyItem_Own  VLCharArabicKeyItem[MAX_SHEFTKEYPAD_NUMBER] =
{
	{AVK_POUND, 0x0642, {0x0642}},
  	{AVK_1, 0x0648, {0x0648}},
  	{AVK_2, 0x064E, {0x064E}},
  	{AVK_3, 0x0631, {0x0631}},
  	{AVK_T, 0x062A, {0x062A}},
  	{AVK_Y, 0x064A, {0x064A}},
  	{AVK_U, 0x064F, {0x064F}},
  
  	{AVK_I, 0x0650, {0x0650}},
  	{AVK_O, 0x0652, {0x0652}},
  	{AVK_P, 0x0637, {0x0637}},
  	{AVK_STAR, 0x0627, {0x0627}},
  	{AVK_4, 0x0633, {0x0633}},
  	{AVK_5, 0x062F, {0x062F}},
  	{AVK_6, 0x0641, {0x0641}},
  
  	{AVK_G, 0x063A, {0x063A}},
  	{AVK_H, 0x062D, {0x062D}},
  	{AVK_J, 0x062C, {0x062C}},
  	{AVK_K, 0x0643, {0x0643}},
  	{AVK_L, 0x0644, {0x0644}},
  	{AVK_7, 0x0632, {0x0632}},
  	{AVK_8, 0x0621, {0x0621}},
  	
  	{AVK_9, 0x0635, {0x0635}},
  	{AVK_V, 0x0651, {0x0651}},
  	{AVK_B, 0x0628, {0x0628}},
  	{AVK_N, 0x0646, {0x0646}},
  	{AVK_M, 0x0645, {0x0645}},
  	{AVK_RWD, 0x061F, {0x061F}},
    {AVK_ENTER,0x000A,{0x000A}},
      
    {AVK_SPACE,' ',{' '}},
	{AVK_0,0x0660,{0x0660}}
};
#endif


#ifdef FEATURE_LANG_THAI


static SheftKeyItem_Own VLCharThaiKeyItem[MAX_SHEFTKEYPAD_NUMBER] = 
{
	{AVK_POUND, 0xE16, {'Q'}},
	{AVK_1, 0xE15, {'W'}},
	{AVK_2, 0xE0E, {'E'}},
	{AVK_3, 0xE1E, {'R'}},
	{AVK_T, 0xE02, {'T'}},
	{AVK_Y, 0xE0A, {'Y'}},
	{AVK_U, 0xE27, {'U'}},

	{AVK_I, 0xE23, {'I'}},
	{AVK_O, 0xE07, {'O'}},
	{AVK_P, 0xE22, {'P'}},
	{AVK_STAR, 0xE1F, {'A'}},
	{AVK_4, 0xE2B, {'S'}},
	{AVK_5, 0xE01, {'D'}},
	{AVK_6, 0xE14, {'F'}},

	{AVK_G, 0xE0C, {'G'}},
	{AVK_H, 0xE08, {'H'}},
	{AVK_J, 0xE1A, {'J'}},
	{AVK_K, 0xE25, {'K'}},
	{AVK_L, 0xE2A, {'L'}},
	{AVK_Z, 0xE1C, {'Z'}},
	{AVK_7, 0xE1B, {'X'}},
	
	{AVK_8, 0xE09, {'C'}},
	{AVK_9, 0xE2D, {'V'}},
	{AVK_B, 0xE2E, {'B'}},
	{AVK_N, 0xE0B, {'N'}},
	{AVK_M, 0xE17, {'M'}},
	{AVK_RWD, 0xE21, {'$'}},
	{AVK_MUTE,0xE1d, {'@'}},
    {AVK_ENTER,0x000A,{0x000A}},
    
    {AVK_SPACE,' ',{' '}}  
};


static SheftKeyItem_Own VLCharShiftThaiKeyItem[MAX_SHEFTKEYPAD_NUMBER] = 
{
	{AVK_POUND, 0xE20, {'Q'}},
	{AVK_1, 0xE04, {'W'}},
	{AVK_2, 0xE26, {'E'}},
	{AVK_3, NULL, {'R'}},
	{AVK_T, NULL, {'T'}},
	{AVK_Y, NULL, {'Y'}},
	{AVK_U, NULL, {'U'}},

	{AVK_I, 0xE13, {'I'}},
	{AVK_O, 0xE19, {'O'}},
	{AVK_P, 0xE0D, {'P'}},
	{AVK_STAR, 0xE24, {'A'}},
	{AVK_4, 0xE06, {'S'}},
	{AVK_5, 0xE0F, {'D'}},
	{AVK_6, NULL, {'F'}},

	{AVK_G, NULL, {'G'}},
	{AVK_H, NULL, {'H'}},
	{AVK_J, NULL, {'J'}},
	{AVK_K, 0xE29, {'K'}},
	{AVK_L, 0xE28, {'L'}},
	{AVK_Z, 0xE11, {'Z'}},
	{AVK_7, 0xE18, {'X'}},
	
	{AVK_8, 0xE10, {'C'}},
	{AVK_9, NULL, {'V'}},
	{AVK_B, NULL, {'B'}},
	{AVK_N, NULL, {'N'}},
	{AVK_M, NULL, {'M'}},
	{AVK_RWD, 0xE12, {'$'}},
	{AVK_MUTE,0xE2C, {'@'}},
    {AVK_ENTER,0x000A,{0x000A}},
    
    {AVK_SPACE,' ',{' '}}  
};
#endif

#ifdef FEATURE_LANG_SPANISH
static SE_character_type VLCharCapSEKeyItem[MAX_SHEFTKEYPAD_NUMBER] = 
{
	{AVK_POUND, {'Q', '#'}, 2},
        
	{AVK_1, {'W','1'}, 2},
	{AVK_2, {'E','2', 0x00C9, 0x00C8}, 4},
	{AVK_3, {'R','3'}, 2},
	{AVK_T, {'T','('}, 2},
	{AVK_Y, {'Y',')'}, 2},
	{AVK_U, {'U', '_', 0x00DA, 0x00DC}, 4},

	{AVK_I, {'I', '-', 0x00CD, 0x00CF}, 4},
	{AVK_O, {'O', '+', 0x00D3, 0x00D2}, 4},
	{AVK_P, {'P', '@'}, 2},
	{AVK_STAR, {'A', '*', 0x00C1, 0x00C0}, 4},
	{AVK_4, {'S', '4'}, 2},
	{AVK_5, {'D', '5'}, 2},
	{AVK_6, {'F', '6'}, 2},

	{AVK_G, {'G', '/'}, 2},
	{AVK_H, {'H', ':'}, 2},
	{AVK_J, {'J', ';'}, 2},
	{AVK_K, {'K', '\''}, 2},
	{AVK_L, {'L', '\"'}, 2},
	{AVK_7, {'Z', '7'}, 2},
	
	{AVK_8, {'X', '8'}, 2},
	{AVK_9, {'C', '9', 0x00C7}, 3},
    {AVK_V, {'V', '?'}, 2},
	{AVK_B, {'B', '!'}, 2},
	{AVK_N, {'N', ',', 0x00D1}, 3},
	{AVK_M, {'M', '.'}, 2},
	{AVK_RWD, {'$', '\0'}, 2},
    {AVK_ENTER,{0x000A},1},
    
    {AVK_SPACE,{' '},1}, 
    {AVK_0,{'0'},1}
};


static SE_character_type VLCharSEKeyItem[MAX_SHEFTKEYPAD_NUMBER] = 
{
	{AVK_POUND, {'q', '#'}, 2},
        
	{AVK_1, {'w','1'}, 2},
	{AVK_2, {'e','2', 0x00E9, 0x00E8}, 4},
	{AVK_3, {'r','3'}, 2},
	{AVK_T, {'t','('}, 2},
	{AVK_Y, {'y',')'}, 2},
	{AVK_U, {'u', '_', 0x00FA, 0x00FC}, 4},

	{AVK_I, {'i', '-', 0x00ED, 0x00EF}, 4},
	{AVK_O, {'o', '+', 0x00F3, 0x00F2}, 4},
	{AVK_P, {'p', '@'}, 2},
	{AVK_STAR, {'a', '*', 0x00E1, 0x00E0}, 4},
	{AVK_4, {'s', '4'}, 2},
	{AVK_5, {'d', '5'}, 2},
	{AVK_6, {'f', '6'}, 2},

	{AVK_G, {'g', '/'}, 2},
	{AVK_H, {'h', ':'}, 2},
	{AVK_J, {'j', ';'}, 2},
	{AVK_K, {'k', '\''}, 2},
	{AVK_L, {'l', '\"'}, 2},
	{AVK_7, {'z', '7'}, 2},
	
	{AVK_8, {'x', '8'}, 2},
	{AVK_9, {'c', '9', 0x00E7},3},
    {AVK_V, {'v', '?'}, 2},
	{AVK_B, {'b', '!'}, 2},
	{AVK_N, {'n', ',', 0x00F1}, 3},
	{AVK_M, {'m', '.'}, 2},
	{AVK_RWD, {'$', '\0'}, 2},
    {AVK_ENTER,{0x000A},1},
    
    {AVK_SPACE,{' '},1},  
    {AVK_0,{'0'},1}

};
#endif


/*-------------------------------------------------------------------
      Type Declarations
      FEATURE DEFINED
-------------------------------------------------------------------*/




//#endif

/*========================================================================
  INTERFACES DOCUMENTATION
==========================================================================

OEM_Text Interface

Description:
  This module provides a input text interface for the AEE. The reference
implementation can be modified to add more text input modes for foreign
languges or other input methods.

=======================================================================

Function: OEM_TextCreate()

Description:
   This function creates a dynamic text control object. It uses the given
   rectangle pRect to create the text control. The IShell and IDisplay
   pointers are saved in the newly created context to be used by the
   text control for notification, drawing, etc.

Prototype:
   OEMCONTEXT OEM_TextCreate(const IShell* pIShell,
                                                                  const IDisplay* pIDisplay,
                                                                  const AEERect *pRect,
                                                                  AEECLSID cls)

Parameters:
   pIShell:    const IShell *   Pointer to the IShell interface object

   pIDisplay:  const IDIsplay*  Pointer to the IDisplay interface object

   pRect:      const AEERect*   Pointer to the rectangle specifying the bounds
                                and location of the text control to be created.

   cls:        AEECLSID         Class ID to create. Should be stored to maintain
                                compatibility with older applets.


Return Value:
   If successful, a OEMCONTEXT which can be used as the handle to the
   newly created text control. If failed, returns NULL. If pIShell or
   pIDisplay or pRect is NULL, the function fails.

Comments:
   None

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextDelete()

Description:
   This function deletes a dynamic text control object. The text control
   must have been created successfully using OEM_TextCreate(). This function,
   also frees up memory and other resources (if any) associated with this
   text control.

Prototype:
   void OEM_TextDelete(OEMCONTEXT hTextField)

Parameters:
   htextField:    OEMCONTEXT   Handle for the text control object that needs
                               to be deleted.

Return Value:
   None

Comments:
   If hTextField is NULL, the function simply returns.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextSet()

Description:
   This function sets the text of a given text control object.
   The text control must have been created successfully using
   OEM_TextCreate(). Once the text has been set, OEM_Textdraw()
   must be called to update the screen with the new text.

Prototype:
   boolean OEM_TextSet(OEMCONTEXT hTextField, const AECHAR *pszText, int nChars)

Parameters:
   htextField:    OEMCONTEXT   Handle for the text control object
   pszText:       AECHAR *     Text string that is to be set into the text control
   nChars:        int          Number of characters to set

Return Value:
   None

Comments:
   If htextField is NULL, the function simply returns.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextGet()

Description:
   This function gets the text associated with a given text control object.
   It returns a pointer to the text present in the given text control object.

Prototype:
   AECHAR* OEM_TextGet(OEMCONTEXT hTextField)

Parameters:
   htextField:    OEMCONTEXT   Handle for the text control object

Return Value:
   On Success, returns pointer to the Text string in the text control.
   On failure, returns NULL.

Comments:
   If hTextCtl is NULL, the function returns NULL.

Side Effects:
   None

See Also:
   None
==========================================================================

Function: OEM_TextDraw()

Description:
   This function draws the text associated with a given text control object
   on the screen. This function also draws the associated items such as
   Scroll Bar, Border, Cursor etc. if  necessary and if supported.

Prototype:
   void OEM_TextDraw(OEMCONTEXT hTextField)

Parameters:
   htextField:    OEMCONTEXT   Handle for the text control object

Return Value:
   None.

Comments:
When the TP_PASSWORD property is set please display a text buffer of **** in place
of actual characters. You must maintain your original buffer of actual text.
Also when in multitap mode, please allow selection(ie. last character to be shown) while typing
Only * out the last character when it is committed to the text.  If hTextCtl is NULL, the
function simply returns.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextUpdate()

Description:
   This function draws the text associated with a given text control object
   on the screen if the text control is modified.

Prototype:
   void OEM_TextUpdate(OEMCONTEXT hTextField)

Parameters:
   htextField:    OEMCONTEXT   Handle for the text control object

Return Value:
   None.

Comments:
   If htextField is NULL, the function simply returns.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextGetModeString()

Description:
   This function returns the wide string corresponding to the current mode of
   the text control specified by hTextField. The possible mode strings are:
   "Multitap", "Numbers" and "Symbols".

Prototype:
   void OEM_TextGetModeString(OEMCONTEXT hTextField, AECHAR* szBuf, uint16 len)

Parameters:
   htextField:    OEMCONTEXT   Handle for the text control object [IN]
   szBuf:         AECHAR *     String corresponding to the mode of the text control [OUT]
   len:           uint16       Length of the mode string buffer [IN]

Return Value:
   None.

Comments:
   If hTextCtl or szBuf is NULL or if len is one or less, the function simply returns.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextSetEdit()

Description:
   This function informs the text control whenever it goes into or out of focus.
   Typically, when the text control is in focus, the border and cursor are
   activated and when the text control goes out of focus, these items are
   de-activated. This function also informs the text control of the current
   text mode.

Prototype:
   void OEM_TextSetEdit(OEMCONTEXT hTextField,
                        boolean bIsEditable,
                        AEETextInputMode wmode
                       )

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object
   bIsEditable:   boolean           Flag to indicate if the text control
                                    object is in focus (is editable)
   wmode:         AEETextInputMode  Text input mode

Return Value:
   None.

Comments:
   If htextField is NULL, the function simply returns.
   If wmode is out of the valid range, please change the input mode to your
   default mode (reference code: AEE_TM_LETTERS)

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextGetCurrentMode()

Description:
   This function returns the current text mode of the text control specified
   by hTextField.

Prototype:
   AEETextInputMode OEM_TextGetCurrentMode(OEMCONTEXT hTextField)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object

Return Value:
  The current text mode for the text control specified.

Comments:
   None.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextAddChar()

Description:
   This function adds/overwrites a charecter at the current cursor location
   in the specified text control.

Prototype:
   void OEM_TextAddChar(OEMCONTEXT hTextField,AECHAR ch, boolean bOverStrike)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object
   ch:            AECHAR            Wide character to be added to the text control
   bOverStrike:   boolean           Overwrite the text at the cursor location.


Return Value:
   None.

Comments:
   Overstrike is only meaningful if there is an insertion point rather than a
   selection AND the insertion point is not at the very end of the text.  If
   hTextField is NULL, the function simply returns.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextQueryModes()

Description:
   This function is invoked by the AEE to query the different text modes
   (T9, MULTITAP, etc) supported by the OEM. This information is used by
   the AEE for two purposes:
   1) To populate the pop-up menu containing selections for the
      different modes supported.
   2) To notify the text control (using OEM_TextSetEdit()) of the mode
      selected by the user.

   The OEM must populate the given data structure and return from this function.


Prototype:
   int OEM_TextQueryModes(AEETextMode** ppTextMode)

Parameters:
   ppTextMode:    AEETextMode**    On return, this contains a valid pointer
                                   to an array of AEE_TextMode containing
                                   information about the different modes
                                   supported by the OEM. The OEM must use the
                                   standard identifier OEM_TEXT_MODE_ SYMBOLS
                                   for symbols mode. Memory for this pointer
                                   must be allocated by the OEM.


Return Value:
   Returns the number of text modes supported by the OEM.

Comments:
   Text  Modes Support:
   Here is a brief description of how text modes are supported.

   ?The AEE platform invokes the OEM function OEM_TextQueryModes() to get
     information on the different text modes supported by the OEM.
   ?The information obtained above is used to populate the menu containing
     selection strings for the different modes.
   ?When the user selects a particular mode, the function OEM_TextSetEdit()
     is invoked and is passed the ID of the mode that has been selected.
     If the user has not changed the mode, the ID is set to OEM_TEXT_MODE_DEFAULT,
     informing the OEM to use the currently selected mode.
   ?The OEM must use the standard ID OEM_TEXT_MODE_SYMBOLS for supporting the
     symbol mode. All other IDs must be based out of OEM_TEXT_MODE_USER.


Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextKeyPress()

Description:
   This function handles the key press events  in a text control.
   Whenever any key is pressed while a text control is active,
   this function is invoked, passing information relating to the
   key that has been pressed. The OEM must handle the key event and
   process it appropriately.

Prototype:
   boolean OEM_TextKeyPress(OEMCONTEXT hTextField,
                         AEEEvent eCode,
                                                                 uint32 dwKeyCode,
                                                                 uint32 dwKeySyms)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object
   eCode:         AEEEvent          Event code for the key event
   dwKeyCode:     uint32            Key code of the key that has been pressed
   dwKeySyms      uint32


Return Value:
  The current text mode for the text control specified.

Comments:
   Ensure to handle key navigation presses on a grapheme boundary.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextDialogEvt()

Description:
   This function handles dialog events of any corresponding dialogs that are
   shown to obtain screen space when inline text editing is not possible due
   to UI constraints.

Prototype:
  boolean OEM_TextDialogEvt(OEMCONTEXT hTextField,
                          AEEEvent eCode,
                                                                  uint16 wDlgID,
                                                                  uint32 dwDlgPtr)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object
   eCode:         AEEEvent          Event code for the dialog event
   wDlgID:        uint32            The dialog ID that sent the event
   dwDlgPtr       uint32            IDialog * for the dialog


Return Value:
  TRUE   Dialog event handled
  FALSE  Dialog event not handled

Comments:
   You must handle only dialog events based on the dialogs created by this layer.

Side Effects:
   Your dialog will have the input focus, reference code sets all events to this layer
   until the dialog is dismissed. This is highly recommended behavior, so events are
   not mixed between dialog and application.

See Also:
   None

==========================================================================

Function: OEM_TextHandlePenEvt()

Description:
   This function will be called ot handle pen events. Please follow the guidelines
   for handling pen events in a cooperative manner to ensure compatibility when
   used in dialogs and applications.

Prototype:
  boolean OEM_TextHandlePenEvt(OEMCONTEXT hTextField,
                           AEEEvent eCode,
                           uint16 wParam,
                           uint32 dwParam);

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object
   eCode:         AEEEvent          Pen Event code
   wParam:        uint16            wParam for EVT_POINTER_* events
   dwParam:       uint32            dwParam for EVT_POINTER_* events


Return Value:
  TRUE   Pen event handled, see documentation for expectations of IControl with pen events
  FALSE  Pen event not handled, see documentation for expectations of IControl with pen events

Comments:
   You must handle the pen events as described in the expectations of Pens with IControl.

Side Effects:
   None.

See Also:
   None

==========================================================================

Function: OEM_TextGetProperties()

Description:
   This function returns the properties of the text control, such as frame type,
   multiline, or rapid entry (such as T9).

Prototype:
   uint32 OEM_TextGetProperties(OEMCONTEXT hTextField)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object

Return Value:
   Property of the text control, which could be combination of 1 or more
   of the following:


Comments:
Important properties
    TP_MULTILINE :  If set, text control object is multiple line control
    TP_FRAME :      If set, text control object has a frame
    TP_RAPID_MODE : If set, text control object is in rapid mode
    TP_PASSWORD :   If set, text control will display * characters in place of real characters
Safe to ignore:
    TP_NODRAW :     If set, text control object does not draw itself
    TP_NOUPDATE :   If set, text control object does not call IDIPLAY_Update when not needed

When using TP_PASSWORD in multitap mode, please allow selection(ie. last character to be shown) while typing
Only * out the last character when it is committed to the text

If hTextCtl is NULL, the function returns 0 ( zero ).

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextSetProperties()

Description:
   This function sets the properties of the text control, such as frame type,
   multiline, rapid entry (such as T9), or a combination thereof.

Prototype:
   void OEM_TextSetProperties(OEMCONTEXT hTextField, uint32 dwProperties)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object
   dwProperties   uint32            Properties (TP_FRAME, TP_MULTILINE,
                                    TP_RAPID_MODE, or a combination of these)

Return Value:
   None.

Comments:
Important properties
    TP_MULTILINE :  If set, text control object is multiple line control
    TP_FRAME :      If set, text control object has a frame
    TP_RAPID_MODE : If set, text control object is in rapid mode
    TP_PASSWORD :   If set, text control will display * characters in place of real characters
Safe to ignore:
    TP_NODRAW :     If set, text control object does not draw itself
    TP_NOUPDATE :   If set, text control object does not call IDIPLAY_Update when not needed

When using TP_PASSWORD in multitap mode, please allow selection(ie. last character to be shown) while typing
Only * out the last character when it is committed to the text

If htextField is NULL, the function simply returns.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextGetRect()

Description:
   This function returns the rectangle correpsonding to the bounds of this
   text control.

Prototype:
   void OEM_TextGetRect(OEMCONTEXT hTextField, AEERect *pOutRect)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object[IN]
   pOuntRect:     AEERect *         Rectangle corresponding to the
                                    bounds of the text control[OUT]

Return Value:
   None.

Comments:
   If hTextCtl or pOutRect is NULL, the function simply returns.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextSetRect()

Description:
   This function returns the rectangle correpsonding to the bounds of this
   text control.

Prototype:
   void OEM_TextSetRect(OEMCONTEXT hTextField, const AEERect *pInRect)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object
   pInRect:     AEERect *         New bounds for the text control

Return Value:
   None.

Comments:
   If htextField or pInRect is NULL, the function simply returns

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextGetMaxChars()

Description:
   This function gets the maximum number of characters that can be added to
   the specified text control.

Prototype:
   uint16 OEM_TextGetMaxChars(OEMCONTEXT hTextField)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object

Return Value:
   Max. number of characters for the text control specified by hTextField.

Comments:
   if hTextCtl is NULL, the function returns 0 (zero).

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextSetMaxChars()

Description:
   This function sets the maximum number of characters that can be added to
   the specified text control.

Prototype:
   void OEM_TextSetMaxChars(OEMCONTEXT hTextField, uint16 wMaxChars)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object
   wMaxChars:     uint16            New maximum number of chars in this
                                    text control

Return Value:
   None.

Comments:
   If htextField is NULL, the function simply returns.

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextGetSel()

Description:
   This function gets the start and end locations for the selected text.

Prototype:
   void OEM_TextGetSel(OEMCONTEXT hTextField, int *piSelStart, int *piSelEnd)

Parameters:
   htextField:    OEMCONTEXT        Handle for the text control object [IN]
   piSelStart:    int *             Start location of the text selection [OUT]
   piSelEnd:      int *             Ending location of the text selection [OUT]

Return Value:
   None.

Comments:
   If htextField is NULL, then
     if piSelStart is non-NULL, the first entry is set to zero.
     if piSelEnd is non-NULL, the first entry is set to zero.

   The selection start position must be preserved to what the applet had set to the
   nearest grapheme.
   This means, if you internally represent the selection start as a value less than
   selection end you must return the actual data set in OEM_TextSetSel().
   Examples:
   OEM_TextSetSel(p, 3, 6) -> OEM_TextGetSel() will return *piSelStart = 3, *piSelEnd = 6
   OEM_TextSetSel(p, 6, 3) -> OEM_TextGetSel() will return *piSelStart = 6, *piSelEnd = 3


Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextSetSel()

Description:
   This function sets the start and end locations for text selection.

Prototype:
   void OEM_TextSetSel(OEMCONTEXT hTextField, int iSelStart, int iSelEnd)

Parameters:
   htextField:   OEMCONTEXT       Handle for the text control object
   iSelStart:    int              Start location of the text selection
   iSelEnd:      int              Ending location of the text selection

Return Value:
   None.

Comments:
   If htextField is NULL, the function simply returns.

   This must update the cursor position to the place where iSelEnd is set.
   The selection start position must be preserved to what the applet had set to the
   nearest grapheme.
   This means, if you internally represent the selection start as a value less than
   selection end you must return the actual data set in OEM_TextSetSel().
   Examples:
   OEM_TextSetSel(p, 3, 6) -> OEM_TextGetSel() will return *piSelStart = 3, *piSelEnd = 6
   OEM_TextSetSel(p, 6, 3) -> OEM_TextGetSel() will return *piSelStart = 6, *piSelEnd = 3

Side Effects:
   None

See Also:
   None

==========================================================================

Function: OEM_TextEnumModesInit()

Description:
   This function initializes the enumeration mode. This function does not
   have an associated handle for the text control object.


Prototype:
   void OEM_TextEnumModesInit(void);

Parameters:
   None.

Return Value:
   None.

Comments:
   None.

Side Effects:
   None

See Also:
   None


==========================================================================

Function: OEM_TextEnumMode()

Description:
   This function gets the next text enumeration mode.


Prototype:
   boolean OEM_TextEnumMode(AEETextMode* pMode)

Parameters:
   pMode:    AEETextMode *     Pointer to the next text mode [IN/OUT]


Return Value:
   TRUE if the next mode is valid.
   FALSE if already at the end of the list.

Comments:
   If pMode is NULL, the function returns FALSE.
   Use the wID member of the AEETextMode structure to enumerate the next mode

Side Effects:
   None

See Also:
   None



==========================================================================

Function: OEM_TextQuerySymbols()

Description:
   This function gets the buffer pszOut with the symbols. The length of
   the buffer is specified by size.


Prototype:
   uint16 OEM_TextQuerySymbols(AECHAR *pszOut, uint16 size)

Parameters:
   pszOut:  AECHAR *     Symbols buffer [OUT]
   size:    uint16       Size of the buffer [IN]

Return Value:
   The number of symbols put into the query buffer. If pszOut is NULL,
   or if size is less than the number of OEM symbols, this function returns
   0.

Comments:
   If pszOut is NULL, the function returns zero. If you do not want the Symbols
   mode to be supported, simply fill the pszOut buffer with an empty string and
   return 0.

Side Effects:
   None

See Also:
   None


==========================================================================

Function: OEM_TextGetCursorPos()

Description:
   This function gets the absolute position of the cursor


Prototype:
   int32 OEM_TextGetCursorPos(OEMCONTEXT hTextField)

Parameters:
   hTextField:    OEMCONTEXT     Handle for the text control object

Return Value:
   The 0 based position of the cursor.
   For example:
   If you have the Text Hi and the cursor is given as |~
~
   |Hi would return 0~
   H|i would return 1~
   Hi| would return 2 ~
*

Comments:
  If you set the character position between an element of a grapheme, the position
  will be updated such that it is in the next available character position after the
  complete grapheme. This means that a call to this will not necessarily match the value
  passed to a previous call to OEM_TextGetCursorPos(). A grapheme is all the characters
  that compose a complete symbol in a language such as Thai.

Side Effects:
   None

See Also:
   None


==========================================================================

Function: OEM_TextSetCursorPos()

Description:
   This function gets the absolute position of the cursor


Prototype:
   int32 OEM_TextSetCursorPos(OEMCONTEXT hTextField, int32 nOffset)

Parameters:
   hTextField:    OEMCONTEXT     Handle for the text control object
   nOffset:       int32          Absolute Offset to move the cursor to

Return Value:
   None.

Comments:
   This function should move the cursor to the 0 based position of the cursor.
   If nOffset is > the length of the text, the cursor should be placed after the text.
   If nOffset is <= 0, the cursor should be placed at the begging of the text
   For example:
   If you have the Text Hi and | represents the cursor:~
~
   nOffset = 0    |Hi~
   nOffset = -1   |Hi~
   nOffset = 1    H|i ~
   nOffset = 2    Hi| ~
   nOffset = 100  Hi| ~
*

  If you set the character position between an element of a grapheme, the position
  must be updated such that it is in the next available character position after the
  complete grapheme. A grapheme is all the characters that compose a complete symbol
  in a language such as Thai.

Side Effects:
   Calling this does not guarentee a matching result in OEM_TextGetCursorPos().

See Also:
   None

==========================================================================

Function: OEM_TextDrawIMEDlg()

Description:
   This function redraws the IME dialog as needed.


Prototype:
   void  OEM_TextDrawIMEDlg(OEMCONTEXT hTextField);

Parameters:
   hTextField:    OEMCONTEXT     Handle for the text control object

Return Value:
   None.

Comments:
   This function should redraw the IME dialog when it is displayed. If one
   is not display this function should return without any operation.

Side Effects:
   None

See Also:
   None

==============================================================
AEEText

See the ITextCTL Interface in the BREW API Reference.

========================================================================*/

#endif    // OEMQUERTKEY_H
