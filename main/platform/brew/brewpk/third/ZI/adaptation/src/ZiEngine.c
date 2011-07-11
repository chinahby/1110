/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2005
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

/*****************************************************************************
 *
 * Filename:
 * ---------
 *    ZiEngine.c
 *
 * Project:
 * --------
 *   Maui_Software
 *
 * Description:
 * ------------
 *
 * Author:
 * -------
 * -------
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

#include "OEMFeatures.h"

#if 1//def  __MMI_ZI__

//#include "GlobalConstants.h"
//#include "gui_data_types.h"
//#include "Fat_fs.h"
#include "IMERes.h"     /* include for sIMEModeDetails */

//#include "DebugInitDef.h"

//#include "ImeGprot.h"
#include "AEE.h"
#include "AEEShell.h"
#include "AEEStdlib.h"
#include "assert.h"
#include "Word_engine.h"  
#include "Imc_symbol.h"
#include "Imc_config.h"
//#include "Imc.h"
//#include "ImmProt.h"
#include "AEEText.h"
#include "zi8api.h"
#include "err.h"

#if defined(__MMI_ZI_MULTITAP__)
//#include "Multitap_engine.h"
#endif

/*****************************************************************************
 * Global variables
 *****************************************************************************/
extern const sIMELDBDetails *gIMELDBArray;

/*****************************************************************************
 * Typedefs
 *****************************************************************************/

/*****************************************************************************
 * Define
 *****************************************************************************/

#if defined(__MMI_IME_USER_DATABASE__)

#define MMI_IME_EZI_PD_SIZE      1024  /* 1k bytes */
#define MMI_IME_EZI_UWD_SIZE     2048  /* 2k bytes */
#define MMI_IME_EZI_UWD_ZH_SIZE  5120  /* 5k bytes */

#define MMI_IME_USER_DATABASE_FILE_PD        L"ime_pd.dat"
#define MMI_IME_USER_DATABASE_FILE_UWD       L"ime_uwd.dat"
#define MMI_IME_USER_DATABASE_FILE_UWD_ZH    L"ime_uwd_zh.dat"

#endif

#define MMI_IME_EZI_MAX_TEMP_BUFFER_SIZE    (400)

#define MMI_IME_EZI_TEMP_BUFFER_FLAG        (0xFFFF)
/*****************************************************************************
 * Typedefs
 *****************************************************************************/

typedef struct
{
    AEETextInputMode         input_mode;
    ZI8UCHAR                        language;
    ZI8UCHAR                        sub_language;
    U32                             required_keys;
    U32                             symbol_keys;
    mmi_ime_get_spellings_funcptr   get_spell_func;
    mmi_ime_get_associates_funcptr  get_assoc_func;
    mmi_ime_get_candidates_funcptr  get_cand_func;
}   mmi_ime_ezi_mode_info_struct, *mmi_ime_ezi_mode_info_struct_p;

typedef struct 
{
    ZI8WCHAR    elem_buff[MMI_IMC_MAX_INPUT_SYMBOL_BUFFSIZE];
#if defined(__MMI_ZI_MULTITAP__)
    ZI8WCHAR    mt_page_buff[MULTITAP_MAX_KEY_CHAR];
#endif /* #if defined(__MMI_ZI_MULTITAP__) */
}   mmi_ime_ezi_data_struct, *mmi_ime_ezi_data_struct_p;

typedef struct
{
    mmi_ime_ezi_data_struct             data;
    ZI8GETPARAM                         get_param;
    MMI_BOOL                            is_inited;
    mmi_ime_ezi_mode_info_struct_p      curr_mode_info;
    U8                                  get_context;
    mmi_ime_word_capital_state_enum     case_type;
#if defined(__MMI_ZI_MULTITAP__)
    S32                              multitap_string_length;
#endif /* #if defined(__MMI_ZI_MULTITAP__) */
}   mmi_ime_ezi_word_engine_struct, *mmi_ime_ezi_word_engine_struct_p;

/*****************************************************************************
 * Local functions
 *****************************************************************************/

/* Utility functions */

static void  mmi_ime_ezi_fill_elem_buff( mmi_ime_query_param_struct_p param_ptr,
                                            U8 symb_range_begin, U8 symb_range_end, const ZI8WCHAR *symb_conv_table );

static void  mmi_ime_ezi_get_candidates(  mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr,
                                            ZI8UCHAR get_mode, ZI8UCHAR get_options );
                                            
#if defined(__MMI_ZI_SM_CHINESE__) || defined(__MMI_ZI_TR_CHINESE__)
static void  mmi_ime_ezi_get_associates( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr );
#endif

static void  mmi_ime_ezi_get_associates_alphabetic( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr );

static const mmi_ime_ezi_mode_info_struct * mmi_ime_ezi_query_mode_info( mmi_imm_input_mode_enum input_mode );

static void  mmi_ime_ezi_change_candidates_case(mmi_ime_query_result_struct_p result_str);

static UI_character_type mmi_ime_ezi_char_to_upper(UI_character_type char_code);

static UI_character_type mmi_ime_ezi_char_to_lower(UI_character_type char_code);

static UI_character_type mmi_ime_ezi_zicode_to_code(ZI8WCHAR zicode);

static void mmi_ime_ezi_check_page_info_and_resume_result(mmi_ime_query_result_struct_p result_ptr);

/* Query functions */

static void     mmi_ime_ezi_get_candidates_alphabetic( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr );

#if defined(__MMI_ZI_SM_CHINESE__) || defined(__MMI_ZI_TR_CHINESE__)
static void     mmi_ime_ezi_get_spellings_stroke( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr );
static void     mmi_ime_ezi_get_candidates_stroke( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr );
#endif /* #if defined(__MMI_ZI_SM_CHINESE__) || defined(__MMI_ZI_TR_CHINESE__) */

/* Simplified Chinese functions */
#if defined(__MMI_ZI_SM_CHINESE__)

#if defined(__MMI_ZI_MULTITAP_PHONETIC_INPUT__)
static void mmi_ime_ezi_get_spellings_pinyin_multitap( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr );
#endif /* #if defined(__MMI_ZI_MULTITAP_PHONETIC_INPUT__) */

#if defined(__MMI_ZI_SMART_PHONETIC_INPUT__)
static void mmi_ime_ezi_get_spellings_pinyin_smart( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr );
#endif /* #if defined(__MMI_ZI_SMART_PHONETIC_INPUT__) */

static void mmi_ime_ezi_get_candidates_pinyin( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr );

#endif /* #if defined(__MMI_ZI_SM_CHINESE__) */

/* Traditional Chinese functions */
#if defined(__MMI_ZI_TR_CHINESE__)

#if defined(__MMI_ZI_MULTITAP_PHONETIC_INPUT__)
static void mmi_ime_ezi_get_spellings_zhuyin_multitap( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr );
#endif /* #if defined(__MMI_ZI_MULTITAP_PHONETIC_INPUT__) */

#if defined(__MMI_ZI_SMART_PHONETIC_INPUT__)
static void mmi_ime_ezi_get_spellings_zhuyin_smart( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr );
#endif /* #if defined(__MMI_ZI_SMART_PHONETIC_INPUT__) */

static void mmi_ime_ezi_get_candidates_zhuyin( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr );

#endif /* #if defined(__MMI_ZI_TR_CHINESE__) */

#if defined(__MMI_MIXED_LANG_SUPPORT__)
static ZI8UCHAR mmi_ime_ezi_map_ime_lang_to_zi_lang(mmi_imm_support_writing_lang_enum lang_id);
#endif /* defined(__MMI_MIXED_LANG_SUPPORT__) */

#if defined(__MMI_IME_USER_DATABASE__)

static MMI_BOOL mmi_ime_ezi_init_user_database(void);

static void mmi_ime_ezi_add_word_to_all_langs(UI_string_type str);

static FS_HANDLE mmi_ime_ezi_open_file(PU16 path_name_ptr, PU16 file_name_ptr, MMI_BOOL is_create_always);

static void mmi_ime_ezi_close_file(FS_HANDLE file_handle);

static U32 mmi_ime_ezi_write_file(FS_HANDLE file_handle, PU16 data_ptr, U32 size_in_byte);

static U32 mmi_ime_ezi_read_file(FS_HANDLE file_handle, PU16 data_ptr, U32 size_in_byte);

#endif

#if defined(__MMI_ZI_MULTITAP__)
static MMI_BOOL mmi_ime_multitap_vendor_valid_key_check( mmi_imc_symbol_key_enum sym);
static UI_string_type mmi_ime_multitap_vendor_get_string(S32 index);
#endif

/*****************************************************************************
 * Local variables
 *****************************************************************************/
   
static const ZI8WCHAR   g_ezi_onekeys[MMI_IMC_SYMB_KEY_MAX - MMI_IMC_SYMB_KEY_BASE] =
{
#if defined(__MMI_ZI_CHINESE_PHRASE_LEVEL_INPUTS__)
    ZI8_CODE_SEPARATOR, // MMI_IMC_SYMB_KEY_0
#else
    ZI8_ONEKEY_KEY0, // MMI_IMC_SYMB_KEY_0
#endif
    ZI8_ONEKEY_KEY1, // MMI_IMC_SYMB_KEY_1
    ZI8_ONEKEY_KEY2, // MMI_IMC_SYMB_KEY_2
    ZI8_ONEKEY_KEY3, // MMI_IMC_SYMB_KEY_3
    ZI8_ONEKEY_KEY4, // MMI_IMC_SYMB_KEY_4
    ZI8_ONEKEY_KEY5, // MMI_IMC_SYMB_KEY_5
    ZI8_ONEKEY_KEY6, // MMI_IMC_SYMB_KEY_6
    ZI8_ONEKEY_KEY7, // MMI_IMC_SYMB_KEY_7
    ZI8_ONEKEY_KEY8, // MMI_IMC_SYMB_KEY_8
    ZI8_ONEKEY_KEY9, // MMI_IMC_SYMB_KEY_9
    0, //MMI_IMC_SYMB_KEY_STAR,
    0, //MMI_IMC_SYMB_KEY_POUND,
};

static const ZI8WCHAR   g_ezi_keys_latin[MMI_IMC_SYMB_KEY_MAX - MMI_IMC_SYMB_KEY_BASE] =
{
    ZI8_CODE_LATIN_KEY0, // MMI_IMC_SYMB_KEY_0
    ZI8_CODE_LATIN_PUNCT, // MMI_IMC_SYMB_KEY_1
    ZI8_CODE_LATIN_KEY2, // MMI_IMC_SYMB_KEY_2
    ZI8_CODE_LATIN_KEY3, // MMI_IMC_SYMB_KEY_3
    ZI8_CODE_LATIN_KEY4, // MMI_IMC_SYMB_KEY_4
    ZI8_CODE_LATIN_KEY5, // MMI_IMC_SYMB_KEY_5
    ZI8_CODE_LATIN_KEY6, // MMI_IMC_SYMB_KEY_6
    ZI8_CODE_LATIN_KEY7, // MMI_IMC_SYMB_KEY_7
    ZI8_CODE_LATIN_KEY8, // MMI_IMC_SYMB_KEY_8
    ZI8_CODE_LATIN_KEY9, // MMI_IMC_SYMB_KEY_9
    0, //MMI_IMC_SYMB_KEY_STAR,
    0, //MMI_IMC_SYMB_KEY_POUND,
};

#if defined(__MMI_ZI_MULTITAP__)
static const ZI8WCHAR   g_ezi_keys_multitap[MMI_IMC_SYMB_KEY_MAX - MMI_IMC_SYMB_KEY_BASE] =
{
    0, // MMI_IMC_SYMB_KEY_0
    ZI8_CODE_LATIN_PUNCT, // MMI_IMC_SYMB_KEY_1
    ZI8_CODE_LATIN_ABC, // MMI_IMC_SYMB_KEY_2
    ZI8_CODE_LATIN_DEF, // MMI_IMC_SYMB_KEY_3
    ZI8_CODE_LATIN_GHI, // MMI_IMC_SYMB_KEY_4
    ZI8_CODE_LATIN_JKL, // MMI_IMC_SYMB_KEY_5
    ZI8_CODE_LATIN_MNO, // MMI_IMC_SYMB_KEY_6
    ZI8_CODE_LATIN_PQRS, // MMI_IMC_SYMB_KEY_7
    ZI8_CODE_LATIN_TUV, // MMI_IMC_SYMB_KEY_8
    ZI8_CODE_LATIN_WXYZ, // MMI_IMC_SYMB_KEY_9
    0, //MMI_IMC_SYMB_KEY_STAR,
    0, //MMI_IMC_SYMB_KEY_POUND,
};
#endif /* #if defined(__MMI_ZI_MULTITAP__) */

static const ZI8WCHAR   g_ezi_codes_tone[MMI_IMC_SYMB_TONE_MAX - MMI_IMC_SYMB_TONE_BASE] =
{
    0, /* MMI_IMC_SYMB_TONE_NONE */
    ZI8_CODE_TONES_1, /* MMI_IMC_SYMB_TONE_1 */
    ZI8_CODE_TONES_2, /* MMI_IMC_SYMB_TONE_2 */
    ZI8_CODE_TONES_3, /* MMI_IMC_SYMB_TONE_3 */
    ZI8_CODE_TONES_4, /* MMI_IMC_SYMB_TONE_4 */
    ZI8_CODE_TONES_5, /* MMI_IMC_SYMB_TONE_5 */
};

#if defined(__MMI_ZI_SM_CHINESE__) || defined(__MMI_ZI_TR_CHINESE__)

static const mmi_imc_symb_stroke_enum g_ezi_keys_stroke[MMI_IMC_SYMB_KEY_MAX - MMI_IMC_SYMB_KEY_BASE] =
{
#if defined(__MMI_ZI_CHINESE_PHRASE_LEVEL_INPUTS__)
    MMI_IMC_SYMB_SEPARATER,  /* MMI_IMC_SYMB_KEY_0 */
#else
    0,                         /* MMI_IMC_SYMB_KEY_0 */
#endif
    MMI_IMC_SYMB_STROKE_OVER,  /* MMI_IMC_SYMB_KEY_1 */
    MMI_IMC_SYMB_STROKE_DOWN,  /* MMI_IMC_SYMB_KEY_2 */
    MMI_IMC_SYMB_STROKE_LEFT,  /* MMI_IMC_SYMB_KEY_3 */
    MMI_IMC_SYMB_STROKE_DOT,   /* MMI_IMC_SYMB_KEY_4 */
    MMI_IMC_SYMB_STROKE_OVER_DOWN, /* MMI_IMC_SYMB_KEY_5 */
    MMI_IMC_SYMB_STROKE_CURVED_HOOK, /* MMI_IMC_SYMB_KEY_6 */
    MMI_IMC_SYMB_STROKE_DOWN_OVER, /* MMI_IMC_SYMB_KEY_7 */
    MMI_IMC_SYMB_STROKE_WILDCARD,  /* MMI_IMC_SYMB_KEY_8 */
    MMI_IMC_SYMB_STROKE_OVER_DOWN_OVER, /* MMI_IMC_SYMB_KEY_9 */
    0,   /* MMI_IMC_SYMB_KEY_STAR */
    0    /* MMI_IMC_SYMB_KEY_POUND */
};

static const ZI8WCHAR   g_ezi_codes_stroke[MMI_IMC_SYMB_STROKE_MAX - MMI_IMC_SYMB_STROKE_BASE] =
{
    ZI8_CODE_WILDCARD,  /* MMI_IMC_SYMB_STROKE_WILDCARD, */
    ZI8_CODE_DOWN,      /* MMI_IMC_SYMB_STROKE_DOWN, */
    ZI8_CODE_DOT,       /* MMI_IMC_SYMB_STROKE_DOT, */
    ZI8_CODE_CURVED_HOOK, /* MMI_IMC_SYMB_STROKE_CURVED_HOOK, */
    ZI8_CODE_OVER,      /* MMI_IMC_SYMB_STROKE_OVER, */
    ZI8_CODE_OVER_DOWN, /* MMI_IMC_SYMB_STROKE_OVER_DOWN, */
    ZI8_CODE_DOWN_OVER, /* MMI_IMC_SYMB_STROKE_DOWN_OVER, */
    ZI8_CODE_LEFT,      /* MMI_IMC_SYMB_STROKE_LEFT, */
    ZI8_CODE_OVER_DOWN_OVER,    /* MMI_IMC_SYMB_STROKE_OVER_DOWN_OVER, */
    ZI8_CODE_MORE,      /* MMI_IMC_SYMB_STROKE_MORE, */
    ZI8_CODE_TURN,      /* MMI_IMC_SYMB_STROKE_TURN, */
    0                   /* MMI_IMC_SYMB_STROKE_DOT_OR_LEFT, */
};
#endif /* #if defined(__MMI_ZI_SM_CHINESE__) || defined(__MMI_ZI_TR_CHINESE__) */

#if defined(__MMI_ZI_SM_CHINESE__)
static const ZI8WCHAR   g_ezi_codes_pinyin[MMI_IMC_SYMB_PY_MAX - MMI_IMC_SYMB_PY_BASE] =
{
    ZI8_CODE_PINYIN_A, /* MMI_IMC_SYMB_PY_A */
    ZI8_CODE_PINYIN_B, /* MMI_IMC_SYMB_PY_B */
    ZI8_CODE_PINYIN_C, /* MMI_IMC_SYMB_PY_C */
    ZI8_CODE_PINYIN_D, /* MMI_IMC_SYMB_PY_D */
    ZI8_CODE_PINYIN_E, /* MMI_IMC_SYMB_PY_E */
    ZI8_CODE_PINYIN_F, /* MMI_IMC_SYMB_PY_F */
    ZI8_CODE_PINYIN_G, /* MMI_IMC_SYMB_PY_G */
    ZI8_CODE_PINYIN_H, /* MMI_IMC_SYMB_PY_H */
    ZI8_CODE_PINYIN_I, /* MMI_IMC_SYMB_PY_I */
    ZI8_CODE_PINYIN_J, /* MMI_IMC_SYMB_PY_J */
    ZI8_CODE_PINYIN_K, /* MMI_IMC_SYMB_PY_K */
    ZI8_CODE_PINYIN_L, /* MMI_IMC_SYMB_PY_L */
    ZI8_CODE_PINYIN_M, /* MMI_IMC_SYMB_PY_M */
    ZI8_CODE_PINYIN_N, /* MMI_IMC_SYMB_PY_N */
    ZI8_CODE_PINYIN_O, /* MMI_IMC_SYMB_PY_O */
    ZI8_CODE_PINYIN_P, /* MMI_IMC_SYMB_PY_P */
    ZI8_CODE_PINYIN_Q, /* MMI_IMC_SYMB_PY_Q */
    ZI8_CODE_PINYIN_R, /* MMI_IMC_SYMB_PY_R */
    ZI8_CODE_PINYIN_S, /* MMI_IMC_SYMB_PY_S */
    ZI8_CODE_PINYIN_T, /* MMI_IMC_SYMB_PY_T */
    ZI8_CODE_PINYIN_U, /* MMI_IMC_SYMB_PY_U */
    ZI8_CODE_PINYIN_V, /* MMI_IMC_SYMB_PY_V */
    ZI8_CODE_PINYIN_W, /* MMI_IMC_SYMB_PY_W */
    ZI8_CODE_PINYIN_X, /* MMI_IMC_SYMB_PY_X */
    ZI8_CODE_PINYIN_Y, /* MMI_IMC_SYMB_PY_Y */
    ZI8_CODE_PINYIN_Z, /* MMI_IMC_SYMB_PY_Z */
};
#endif /* #if defined(__MMI_ZI_SM_CHINESE__) */

#if defined(__MMI_ZI_TR_CHINESE__)
static const ZI8WCHAR   g_ezi_codes_bopomofo[MMI_IMC_SYMB_ZY_MAX - MMI_IMC_SYMB_ZY_BASE] =
{
    ZI8_CODE_BPMF_B, /*MMI_IMC_SYMB_ZY_B,*/
    ZI8_CODE_BPMF_P, /*MMI_IMC_SYMB_ZY_P,*/
    ZI8_CODE_BPMF_M, /*MMI_IMC_SYMB_ZY_M,*/
    ZI8_CODE_BPMF_F, /*MMI_IMC_SYMB_ZY_F,*/
    
    ZI8_CODE_BPMF_D, /*MMI_IMC_SYMB_ZY_D,*/
    ZI8_CODE_BPMF_T, /*MMI_IMC_SYMB_ZY_T,*/
    ZI8_CODE_BPMF_N, /*MMI_IMC_SYMB_ZY_N,*/
    ZI8_CODE_BPMF_L, /*MMI_IMC_SYMB_ZY_L,*/
    
    ZI8_CODE_BPMF_G, /*MMI_IMC_SYMB_ZY_G,*/
    ZI8_CODE_BPMF_K, /*MMI_IMC_SYMB_ZY_K,*/
    ZI8_CODE_BPMF_H, /*MMI_IMC_SYMB_ZY_H,*/
    
    ZI8_CODE_BPMF_J, /*MMI_IMC_SYMB_ZY_J,*/
    ZI8_CODE_BPMF_Q, /*MMI_IMC_SYMB_ZY_Q,*/
    ZI8_CODE_BPMF_X, /*MMI_IMC_SYMB_ZY_X,*/
    
    ZI8_CODE_BPMF_ZH, /*MMI_IMC_SYMB_ZY_ZH,*/
    ZI8_CODE_BPMF_CH, /*MMI_IMC_SYMB_ZY_CH,*/
    ZI8_CODE_BPMF_SH, /*MMI_IMC_SYMB_ZY_SH,*/
    ZI8_CODE_BPMF_R,  /*MMI_IMC_SYMB_ZY_R,*/
    
    ZI8_CODE_BPMF_Z,  /*MMI_IMC_SYMB_ZY_Z,*/
    ZI8_CODE_BPMF_C,  /*MMI_IMC_SYMB_ZY_C,*/
    ZI8_CODE_BPMF_S,  /*MMI_IMC_SYMB_ZY_S,*/
    
    ZI8_CODE_BPMF_A,  /*MMI_IMC_SYMB_ZY_A,*/
    ZI8_CODE_BPMF_O,  /*MMI_IMC_SYMB_ZY_O,*/
    ZI8_CODE_BPMF_E,  /*MMI_IMC_SYMB_ZY_E,*/
    ZI8_CODE_BPMF_EH, /*MMI_IMC_SYMB_ZY_EH,*/
    
    ZI8_CODE_BPMF_AI, /*MMI_IMC_SYMB_ZY_AI,*/
    ZI8_CODE_BPMF_EI, /*MMI_IMC_SYMB_ZY_EI,*/
    ZI8_CODE_BPMF_AU, /*MMI_IMC_SYMB_ZY_AU,*/
    ZI8_CODE_BPMF_OU, /*MMI_IMC_SYMB_ZY_OU,*/
    
    ZI8_CODE_BPMF_AN,  /*MMI_IMC_SYMB_ZY_AN,*/
    ZI8_CODE_BPMF_EN,  /*MMI_IMC_SYMB_ZY_EN,*/
    ZI8_CODE_BPMF_ANG, /*MMI_IMC_SYMB_ZY_ANG,*/
    ZI8_CODE_BPMF_ENG, /*MMI_IMC_SYMB_ZY_ENG,*/
    ZI8_CODE_BPMF_ER,  /*MMI_IMC_SYMB_ZY_ER,*/
    
    ZI8_CODE_BPMF_I,  /*MMI_IMC_SYMB_ZY_I,*/
    ZI8_CODE_BPMF_U,  /*MMI_IMC_SYMB_ZY_U,*/
    ZI8_CODE_BPMF_IU, /*MMI_IMC_SYMB_ZY_IU*/
};

static const ZI8WCHAR g_bopomofo_table[MMI_IMC_SYMB_ZY_MAX - MMI_IMC_SYMB_ZY_BASE] = 
{
    0x3105, /*MMI_IMC_SYMB_ZY_B*/
    0x3106, /*MMI_IMC_SYMB_ZY_P*/
    0x3107, /*MMI_IMC_SYMB_ZY_M*/
    0x3108, /*MMI_IMC_SYMB_ZY_F*/
    0x3109, /*MMI_IMC_SYMB_ZY_D*/
    0x310A, /*MMI_IMC_SYMB_ZY_T*/
    0x310B, /*MMI_IMC_SYMB_ZY_N*/
    0x310C, /*MMI_IMC_SYMB_ZY_L*/
    0x310D, /*MMI_IMC_SYMB_ZY_G*/
    0x310E, /*MMI_IMC_SYMB_ZY_K*/
    0x310F, /*MMI_IMC_SYMB_ZY_H*/
    0x3110, /*MMI_IMC_SYMB_ZY_J*/
    0x3111, /*MMI_IMC_SYMB_ZY_Q*/
    0x3112, /*MMI_IMC_SYMB_ZY_X*/
    0x3113, /*MMI_IMC_SYMB_ZY_ZH*/
    0x3114, /*MMI_IMC_SYMB_ZY_CH*/
    0x3115, /*MMI_IMC_SYMB_ZY_SH*/
    0x3116, /*MMI_IMC_SYMB_ZY_R*/
    0x3117, /*MMI_IMC_SYMB_ZY_Z*/
    0x3118, /*MMI_IMC_SYMB_ZY_C*/
    0x3119, /*MMI_IMC_SYMB_ZY_S*/
    0x311A, /*MMI_IMC_SYMB_ZY_A*/
    0x311B, /*MMI_IMC_SYMB_ZY_O*/
    0x311C, /*MMI_IMC_SYMB_ZY_E*/
    0x311D, /*MMI_IMC_SYMB_ZY_EH*/
    0x311E, /*MMI_IMC_SYMB_ZY_AI*/
    0x311F, /*MMI_IMC_SYMB_ZY_EI*/
    0x3120, /*MMI_IMC_SYMB_ZY_AU*/
    0x3121, /*MMI_IMC_SYMB_ZY_OU*/
    0x3122, /*MMI_IMC_SYMB_ZY_AN*/
    0x3123, /*MMI_IMC_SYMB_ZY_EN*/
    0x3124, /*MMI_IMC_SYMB_ZY_ANG*/
    0x3125, /*MMI_IMC_SYMB_ZY_ENG*/
    0x3126, /*MMI_IMC_SYMB_ZY_ER*/
    0x3127, /*MMI_IMC_SYMB_ZY_I*/
    0x3128, /*MMI_IMC_SYMB_ZY_U*/
    0x3129  /*MMI_IMC_SYMB_ZY_IU*/
};
#endif /* #if defined(__MMI_ZI_TR_CHINESE__) */


/* All input mode informations */
static const mmi_ime_ezi_mode_info_struct   g_mode_infos[] =
{
#if defined(__MMI_ZI_TR_CHINESE__) && defined(__MMI_ZI_MULTITAP_PHONETIC_INPUT__)
    {   
        IMM_INPUT_MODE_TR_MULTITAP_BOPOMO,
        ZI8_LANG_ZH,
        ZI8_SUBLANG_ZH_TW,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        mmi_ime_ezi_get_spellings_zhuyin_multitap,
        mmi_ime_ezi_get_associates,
        mmi_ime_ezi_get_candidates_zhuyin
    },
    {   
        IMM_INPUT_MODE_QUICK_SEARCH_BOPOMO,
        ZI8_LANG_ZH,
        ZI8_SUBLANG_ZH_TW,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        mmi_ime_ezi_get_spellings_zhuyin_multitap,
        mmi_ime_ezi_get_associates,
        mmi_ime_ezi_get_candidates_zhuyin
    },
#endif /* #if defined(__MMI_ZI_TR_CHINESE__) && defined(__MMI_ZI_MULTITAP_PHONETIC_INPUT__) */

#if defined(__MMI_ZI_SM_CHINESE__) && defined(__MMI_ZI_MULTITAP_PHONETIC_INPUT__)
    {
        IMM_INPUT_MODE_SM_MULTITAP_PINYIN,
        ZI8_LANG_ZH,
        ZI8_SUBLANG_ZH,
        MMI_IME_REQUIRED_ALL_KEYS & (~MMI_IME_REQUIRED_KEY_1),
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_1) & (~MMI_IME_REQUIRED_KEY_0),
        mmi_ime_ezi_get_spellings_pinyin_multitap,
        mmi_ime_ezi_get_associates,
        mmi_ime_ezi_get_candidates_pinyin
    },
    {
        IMM_INPUT_MODE_QUICK_SEARCH_PINYIN,
        ZI8_LANG_ZH,
        ZI8_SUBLANG_ZH,
        MMI_IME_REQUIRED_ALL_KEYS & (~MMI_IME_REQUIRED_KEY_1),
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_1) & (~MMI_IME_REQUIRED_KEY_0),
        mmi_ime_ezi_get_spellings_pinyin_multitap,
        mmi_ime_ezi_get_associates,
        mmi_ime_ezi_get_candidates_pinyin
    },
#endif /* #if defined(__MMI_ZI_SM_CHINESE__) && defined(__MMI_ZI_MULTITAP_PHONETIC_INPUT__) */

#if defined(__MMI_ZI_TR_CHINESE__) && defined(__MMI_ZI_SMART_PHONETIC_INPUT__)
    {   
        IMM_INPUT_MODE_TR_BOPOMO,
        ZI8_LANG_ZH,
        ZI8_SUBLANG_ZH_TW,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        mmi_ime_ezi_get_spellings_zhuyin_smart,
        mmi_ime_ezi_get_associates,
        mmi_ime_ezi_get_candidates_zhuyin
    },
#endif /* #if defined(__MMI_ZI_TR_CHINESE__) && defined(__MMI_ZI_SMART_PHONETIC_INPUT__) */    

#if defined(__MMI_ZI_SM_CHINESE__) && defined(__MMI_ZI_SMART_PHONETIC_INPUT__)
    {
        IMM_INPUT_MODE_SM_PINYIN,
        ZI8_LANG_ZH,
        ZI8_SUBLANG_ZH,
        MMI_IME_REQUIRED_ALL_KEYS & (~MMI_IME_REQUIRED_KEY_1),
#if defined(__MMI_ZI_CHINESE_PHRASE_LEVEL_INPUTS__)
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_1),
#else
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_1) & (~MMI_IME_REQUIRED_KEY_0),
#endif
        mmi_ime_ezi_get_spellings_pinyin_smart,
        mmi_ime_ezi_get_associates,
        mmi_ime_ezi_get_candidates_pinyin
    },
#endif /* #if defined(__MMI_ZI_SM_CHINESE__) && defined(__MMI_ZI_SMART_PHONETIC_INPUT__) */

#if defined(__MMI_ZI_TR_CHINESE__)
    {
        IMM_INPUT_MODE_TR_STROKE, 
        ZI8_LANG_ZH,
        ZI8_SUBLANG_ZH_TW,
#if defined(__MMI_ZI_CHINESE_PHRASE_LEVEL_INPUTS__)
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
#else
        MMI_IME_REQUIRED_ALL_KEYS  & (~MMI_IME_REQUIRED_KEY_0),
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
#endif
        mmi_ime_ezi_get_spellings_stroke,
        mmi_ime_ezi_get_associates,
        mmi_ime_ezi_get_candidates_stroke
    },
    {
        IMM_INPUT_MODE_QUICK_SEARCH_TR_STROKE,
        ZI8_LANG_ZH,
        ZI8_SUBLANG_ZH_TW,
        MMI_IME_REQUIRED_ALL_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        mmi_ime_ezi_get_spellings_stroke,
        mmi_ime_ezi_get_associates,
        mmi_ime_ezi_get_candidates_stroke
    },
#endif /* #if defined(__MMI_ZI_TR_CHINESE__) */

#if defined(__MMI_ZI_SM_CHINESE__)
    {
        IMM_INPUT_MODE_SM_STROKE,
        ZI8_LANG_ZH,
        ZI8_SUBLANG_ZH,
#if defined(__MMI_ZI_CHINESE_PHRASE_LEVEL_INPUTS__)
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
#else
        MMI_IME_REQUIRED_ALL_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
#endif
        mmi_ime_ezi_get_spellings_stroke,
        mmi_ime_ezi_get_associates,
        mmi_ime_ezi_get_candidates_stroke
    },
    {
        IMM_INPUT_MODE_QUICK_SEARCH_SM_STROKE,
        ZI8_LANG_ZH,
        ZI8_SUBLANG_ZH,
        MMI_IME_REQUIRED_ALL_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        mmi_ime_ezi_get_spellings_stroke,
        mmi_ime_ezi_get_associates,
        mmi_ime_ezi_get_candidates_stroke
    },
#endif /* #if defined(__MMI_ZI_TR_CHINESE__) */

#if defined(__MMI_ZI_MULTITAP__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_ABC,
        ZI8_LANG_EN,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_ABC,
        ZI8_LANG_EN,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
#endif /* defined(__MMI_ZI_MULTITAP__) */

#if defined(__MMI_ZI_PRC_ENGLISH__)

    {
        IMM_INPUT_MODE_SMART_UPPERCASE_ABC,
        ZI8_LANG_EN,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_ABC,
        ZI8_LANG_EN,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_PRC_ENGLISH__) */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_ARABIC__)
    {
        IMM_INPUT_MODE_MULTITAP_ARABIC,
        ZI8_LANG_AR,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
#endif /* defined(__MMI_ZI_MULTITAP__) */

#if defined(__MMI_ZI_ARABIC__)
    {
        IMM_INPUT_MODE_SMART_ARABIC,
        ZI8_LANG_AR,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_ARABIC__) */

#if defined(__MMI_ZI_BENGALI__)
    {
        IMM_INPUT_MODE_SMART_BENGALI,
        ZI8_LANG_BN,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_BENGALI__) */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_BULGARIAN__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_BULGARIAN,
        ZI8_LANG_BG,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_BULGARIAN,
        ZI8_LANG_BG,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
#endif /*defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_BULGARIAN__)*/

#if defined(__MMI_ZI_BULGARIAN__)
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_BULGARIAN,
        ZI8_LANG_BG,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },

    {
        IMM_INPUT_MODE_SMART_LOWERCASE_BULGARIAN,
        ZI8_LANG_BG,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_BULGARIAN__) */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_PORTUGUESE__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_PORTUGUESE,
        ZI8_LANG_PT,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_PORTUGUESE,
        ZI8_LANG_PT,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
#endif /* defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_PORTUGUESE__) */

#if defined(__MMI_ZI_BZ_PORTUGUESE__) || defined(__MMI_ZI_SA_PORTUGUESE__)
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_PORTUGUESE,
        ZI8_LANG_PT,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_PORTUGUESE,
        ZI8_LANG_PT,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_BZ_PORTUGUESE__) || defined(__MMI_ZI_SA_PORTUGUESE__) */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_CROATIAN__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_CROATIAN,
        ZI8_LANG_HR,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_CROATIAN,
        ZI8_LANG_HR,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
#endif /*defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_CROATIAN__)*/

#if defined(__MMI_ZI_CROATIAN__) 
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_CROATIAN,
        ZI8_LANG_HR,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_CROATIAN,
        ZI8_LANG_HR,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_CROATIAN__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_CZECH__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_CZECH,
        ZI8_LANG_CS,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_CZECH,
        ZI8_LANG_CS,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
#endif /*defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_CZECH__)*/

#if defined(__MMI_ZI_CZECH__) 
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_CZECH,
        ZI8_LANG_CS,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_CZECH,
        ZI8_LANG_CS,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_CZECH__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_DANISH__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_DANISH,
        ZI8_LANG_DA,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_DANISH,
        ZI8_LANG_DA,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
#endif /*defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_DANISH__)*/

#if defined(__MMI_ZI_DANISH__) 
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_DANISH,
        ZI8_LANG_DA,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_DANISH,
        ZI8_LANG_DA,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_DANISH__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_DUTCH__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_DUTCH,
        ZI8_LANG_NL,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_DUTCH,
        ZI8_LANG_NL,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
#endif /*defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_DUTCH__)*/

#if defined(__MMI_ZI_DUTCH__) 
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_DUTCH,
        ZI8_LANG_NL,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_DUTCH,
        ZI8_LANG_NL,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_DUTCH__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_FRENCH__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_FRENCH,
        ZI8_LANG_FR,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_FRENCH,
        ZI8_LANG_FR,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
#endif /*defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_FRENCH__)*/

#if defined(__MMI_ZI_EU_FRENCH__) 
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_FRENCH,
        ZI8_LANG_FR,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_FRENCH,
        ZI8_LANG_FR,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_EU_FRENCH__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_FINNISH__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_FINNISH,
        ZI8_LANG_FI,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_FINNISH,
        ZI8_LANG_FI,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
#endif /*defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_FINNISH__)*/

#if defined(__MMI_ZI_FINNISH__) 
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_FINNISH,
        ZI8_LANG_FI,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_FINNISH,
        ZI8_LANG_FI,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_FINNISH__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_GERMAN__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_GERMAN,
        ZI8_LANG_DE,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_GERMAN,
        ZI8_LANG_DE,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
#endif /*defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_GERMAN__)*/

#if defined(__MMI_ZI_GERMAN__) 
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_GERMAN,
        ZI8_LANG_DE,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_GERMAN,
        ZI8_LANG_DE,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_GERMAN__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_GREEK__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_GREEK,
        ZI8_LANG_EL,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_GREEK,
        ZI8_LANG_EL,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
#endif /*defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_GREEK__)*/

#if defined(__MMI_ZI_GREEK__) 
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_GREEK,
        ZI8_LANG_EL,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_GREEK,
        ZI8_LANG_EL,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_GREEK__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_HEBREW__)
    {
        IMM_INPUT_MODE_MULTITAP_HEBREW,
        ZI8_LANG_IW,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
#endif /*defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_HEBREW__)*/

#if defined(__MMI_ZI_HEBREW__) 
    {
        IMM_INPUT_MODE_SMART_HEBREW,
        ZI8_LANG_IW,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_HEBREW__)  */

#if defined(__MMI_ZI_HINDI__) 
    {
        IMM_INPUT_MODE_SMART_HINDI,
        ZI8_LANG_HI,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_HINDI__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_HUNGARIAN__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_HUNGARIAN,
        ZI8_LANG_HU,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_HUNGARIAN,
        ZI8_LANG_HU,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
#endif /* defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_HUNGARIAN__) */

#if defined(__MMI_ZI_HUNGARIAN__) 
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_HUNGARIAN,
        ZI8_LANG_HU,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_HUNGARIAN,
        ZI8_LANG_HU,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_HUNGARIAN__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_INDONESIAN__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_INDONESIAN,
        ZI8_LANG_IN,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_INDONESIAN,
        ZI8_LANG_IN,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
#endif /* defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_INDONESIAN__) */

#if defined(__MMI_ZI_INDONESIAN__) 
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_INDONESIAN,
        ZI8_LANG_IN,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_INDONESIAN,
        ZI8_LANG_IN,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_INDONESIAN__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_ITALIAN__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_ITALIAN,
        ZI8_LANG_IT,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_ITALIAN,
        ZI8_LANG_IT,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
#endif /* defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_ITALIAN__) */

#if defined(__MMI_ZI_ITALIAN__) 

    {
        IMM_INPUT_MODE_SMART_UPPERCASE_ITALIAN,
        ZI8_LANG_IT,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_ITALIAN,
        ZI8_LANG_IT,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_ITALIAN__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_MALAY__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_MALAY,
        ZI8_LANG_MS,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_MALAY,
        ZI8_LANG_MS,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
#endif /* defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_MALAY__) */

#if defined(__MMI_ZI_MALAY__) 
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_MALAY,
        ZI8_LANG_MS,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_MALAY,
        ZI8_LANG_MS,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_MALAY__)  */

#if defined(__MMI_ZI_MARATHI__) 
    {
        IMM_INPUT_MODE_SMART_MARATHI,
        ZI8_LANG_MR,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_MARATHI__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_NORWEGIAN__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_NORWEGIAN,
        ZI8_LANG_NO,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_NORWEGIAN,
        ZI8_LANG_NO,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
#endif /* defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_NORWEGIAN__) */

#if defined(__MMI_ZI_NORWEGIAN__) 
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_NORWEGIAN,
        ZI8_LANG_NO,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_NORWEGIAN,
        ZI8_LANG_NO,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_NORWEGIAN__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_PERSIAN__)
    {
        IMM_INPUT_MODE_MULTITAP_PERSIAN,
        ZI8_LANG_FA,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
#endif /* defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_PERSIAN__) */

#if defined(__MMI_ZI_PERSIAN__) 
    {
        IMM_INPUT_MODE_SMART_PERSIAN,
        ZI8_LANG_FA,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_PERSIAN__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_POLISH__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_POLISH,
        ZI8_LANG_PL,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_POLISH,
        ZI8_LANG_PL,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
#endif /* defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_POLISH__) */

#if defined(__MMI_ZI_POLISH__) 
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_POLISH,
        ZI8_LANG_PL,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_POLISH,
        ZI8_LANG_PL,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_POLISH__)  */

#if defined(__MMI_ZI_PUNJABI__) 
    {
        IMM_INPUT_MODE_SMART_PUNJABI,
        ZI8_LANG_PA,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_PUNJABI__)  */

#if defined(__MMI_ZI_KANNADA__) 
    {
        IMM_INPUT_MODE_SMART_KANNADA,
        ZI8_LANG_KN,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_KANNADA__)  */

#if defined(__MMI_ZI_MALAYALAM__) 
    {
        IMM_INPUT_MODE_SMART_MALAYALAM,
        ZI8_LANG_ML,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        NULL,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_MALAYALAM__)  */


#if defined(__MMI_ZI_GUJARATI__) 
    {
        IMM_INPUT_MODE_SMART_GUJARATI,
        ZI8_LANG_GU,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_GUJARATI__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_ROMANIAN__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_ROMANIAN,
        ZI8_LANG_RO,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_ROMANIAN,
        ZI8_LANG_RO,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
#endif /* defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_ROMANIAN__) */

#if defined(__MMI_ZI_ROMANIAN__) 
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_ROMANIAN,
        ZI8_LANG_RO,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_ROMANIAN,
        ZI8_LANG_RO,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_ROMANIAN__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_RUSSIAN__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_RUSSIAN,
        ZI8_LANG_RU,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_RUSSIAN,
        ZI8_LANG_RU,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
#endif /* defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_RUSSIAN__) */

#if defined(__MMI_ZI_RUSSIAN__) 
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_RUSSIAN,
        ZI8_LANG_RU,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_RUSSIAN,
        ZI8_LANG_RU,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_RUSSIAN__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_SPANISH__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_SPANISH,
        ZI8_LANG_ES,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_SPANISH,
        ZI8_LANG_ES,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
#endif /* defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_SPANISH__) */

#if defined(__MMI_ZI_SA_SPANISH__) 
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_SPANISH,
        ZI8_LANG_ES,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_SPANISH,
        ZI8_LANG_ES,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_SA_SPANISH__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_SLOVAK__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_SLOVAK,
        ZI8_LANG_SK,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_SLOVAK,
        ZI8_LANG_SK,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
#endif /* defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_SLOVAK__) */

#if defined(__MMI_ZI_SLOVAK__) 
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_SLOVAK,
        ZI8_LANG_SK,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_SLOVAK,
        ZI8_LANG_SK,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_SLOVAK__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_SLOVENIAN__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_SLOVENIAN,
        ZI8_LANG_SL,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_SLOVENIAN,
        ZI8_LANG_SL,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
#endif /* defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_SLOVENIAN__) */

#if defined(__MMI_ZI_SLOVENIAN__) 
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_SLOVENIAN,
        ZI8_LANG_SL,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_SLOVENIAN,
        ZI8_LANG_SL,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_SLOVENIAN__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_SWEDISH__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_SWEDISH,
        ZI8_LANG_SV,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_SWEDISH,
        ZI8_LANG_SV,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
#endif /* defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_SWEDISH__) */

#if defined(__MMI_ZI_SWEDISH__) 
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_SWEDISH,
        ZI8_LANG_SV,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_SWEDISH,
        ZI8_LANG_SV,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_SWEDISH__)  */

#if defined(__MMI_ZI_TAMIL__) 
    {
        IMM_INPUT_MODE_SMART_TAMIL,
        ZI8_LANG_TA,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_TAMIL__)  */

#if defined(__MMI_ZI_TELUGU__) 
    {
        IMM_INPUT_MODE_SMART_TELUGU,
        ZI8_LANG_TE,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_TELUGU__)  */

#if defined(__MMI_ZI_THAI__) 
    {
        IMM_INPUT_MODE_SMART_THAI,
        ZI8_LANG_TH,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_THAI__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_TURKISH__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_TURKISH,
        ZI8_LANG_TR,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
    
    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_TURKISH,
        ZI8_LANG_TR,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

#endif /* defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_TURKISH__) */

#if defined(__MMI_ZI_TURKISH__) 
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_TURKISH,
        ZI8_LANG_TR,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_TURKISH,
        ZI8_LANG_TR,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_TURKISH__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_UKRAINIAN__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_UKRAINIAN,
        ZI8_LANG_UK,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
    
    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_UKRAINIAN,
        ZI8_LANG_UK,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

#endif /* defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_UKRAINIAN__) */

#if defined(__MMI_ZI_UKRAINIAN__) 
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_UKRAINIAN,
        ZI8_LANG_UK,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_UKRAINIAN,
        ZI8_LANG_UK,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_UKRAINIAN__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_URDU__)
	{
		IMM_INPUT_MODE_MULTITAP_URDU,
		ZI8_LANG_UR,
		ZI8_SUBLANG_DEFAULT,
		MMI_IME_REQUIRED_ALL_KEYS,
		MMI_IME_REQUIRED_ALL_NUM_KEYS,
		NULL,
		NULL,
		NULL
    },
#endif

#if defined(__MMI_ZI_URDU__) 
    {
        IMM_INPUT_MODE_SMART_URDU,
        ZI8_LANG_UR,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_URDU__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_VIETNAMESE__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_VIETNAMESE,
        ZI8_LANG_VI,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
    
    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_VIETNAMESE,
        ZI8_LANG_VI,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

#endif /* defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_VIETNAMESE__) */

#if defined(__MMI_ZI_VIETNAMESE__) 
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_VIETNAMESE,
        ZI8_LANG_VI,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_VIETNAMESE,
        ZI8_LANG_VI,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_VIETNAMESE__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_SWAHILI__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_SWAHILI,
        ZI8_LANG_SW,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
    
    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_SWAHILI,
        ZI8_LANG_SW,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

#endif /* defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_SWAHILI__) */

#if defined(__MMI_ZI_SWAHILI__) 
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_SWAHILI,
        ZI8_LANG_SW,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_SWAHILI,
        ZI8_LANG_SW,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_SWAHILI__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_AFRIKAANS__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_AFRIKAANS,
        ZI8_LANG_AF,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
    
    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_AFRIKAANS,
        ZI8_LANG_AF,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

#endif /* defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_AFRIKAANS__) */

#if defined(__MMI_ZI_AFRIKAANS__) 
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_AFRIKAANS,
        ZI8_LANG_AF,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_AFRIKAANS,
        ZI8_LANG_AF,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_AFRIKAANS__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_LITHUANIAN__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_LITHUANIAN,
        ZI8_LANG_LT,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
    
    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_LITHUANIAN,
        ZI8_LANG_LT,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

#endif /* defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_LITHUANIAN__) */

#if defined(__MMI_ZI_LITHUANIAN__) 
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_LITHUANIAN,
        ZI8_LANG_LT,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_LITHUANIAN,
        ZI8_LANG_LT,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_LITHUANIAN__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_LATVIAN__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_LATVIAN,
        ZI8_LANG_LV,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
    
    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_LATVIAN,
        ZI8_LANG_LV,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

#endif /* defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_LATVIAN__) */

#if defined(__MMI_ZI_LATVIAN__) 
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_LATVIAN,
        ZI8_LANG_LV,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_LATVIAN,
        ZI8_LANG_LV,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_LATVIAN__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_ESTONIAN__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_ESTONIAN,
        ZI8_LANG_ET,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
    
    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_ESTONIAN,
        ZI8_LANG_ET,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

#endif /* defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_ESTONIAN__) */

#if defined(__MMI_ZI_ESTONIAN__) 
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_ESTONIAN,
        ZI8_LANG_ET,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_ESTONIAN,
        ZI8_LANG_ET,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_MOLDOVAN__)  */

#if defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_MOLDOVAN__)
    {
        IMM_INPUT_MODE_MULTITAP_UPPERCASE_MOLDOVAN,
        ZI8_LANG_RO,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },
    
    {
        IMM_INPUT_MODE_MULTITAP_LOWERCASE_MOLDOVAN,
        ZI8_LANG_RO,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS,
        NULL,
        NULL,
        NULL
    },

#endif /* defined(__MMI_ZI_MULTITAP__) && defined(__MMI_MULTITAP_MOLDOVAN__) */

#if defined(__MMI_ZI_MOLDOVAN__) 
    {
        IMM_INPUT_MODE_SMART_UPPERCASE_MOLDOVAN,
        ZI8_LANG_RO,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
    
    {
        IMM_INPUT_MODE_SMART_LOWERCASE_MOLDOVAN,
        ZI8_LANG_RO,
        ZI8_SUBLANG_DEFAULT,
        MMI_IME_REQUIRED_ALL_KEYS,
        MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
        NULL,
        mmi_ime_ezi_get_associates_alphabetic,
        mmi_ime_ezi_get_candidates_alphabetic
    },
#endif /* #if defined(__MMI_ZI_MOLDOVAN__)  */

    {
        IMM_INPUT_MODE_MAX_NUM,
        0,
        0,
        0,
        0,
        NULL,
        NULL,
        NULL
    }
};


#if defined(__MMI_MIXED_LANG_SUPPORT__)
static const ZI8UCHAR g_ezi_mixed_lang_mapping[][2] =
{
    { IME_WRITING_LANG_ABC, ZI8_LANG_EN }, /* English */
#ifdef __MMI_ZI_EU_FRENCH__
    { IME_WRITING_LANG_FR, ZI8_LANG_FR },  /* French */
#endif
#ifdef __MMI_ZI_SA_SPANISH__
    { IME_WRITING_LANG_ES, ZI8_LANG_ES },  /* SPANISH */
#endif
#ifdef __MMI_ZI_DANISH__
    { IME_WRITING_LANG_DA, ZI8_LANG_DA },  /* DANISH */
#endif
#ifdef __MMI_ZI_POLISH__
    { IME_WRITING_LANG_PL, ZI8_LANG_PL},  /* POLISH */
#endif
#if defined(__MMI_ZI_BZ_PORTUGUESE__) || defined(__MMI_ZI_SA_PORTUGUESE__) || defined(__MMI_ZI_EU_PORTUGUESE__) 
    { IME_WRITING_LANG_PT, ZI8_LANG_PT},  /* PORTUGUESE */
#endif
#ifdef __MMI_ZI_ITALIAN__
    { IME_WRITING_LANG_IT, ZI8_LANG_IT},  /* ITALIAN */
#endif
#ifdef __MMI_ZI_GERMAN__
    { IME_WRITING_LANG_DE, ZI8_LANG_DE},  /* GERMAN */
#endif
#ifdef __MMI_ZI_MALAY__
    { IME_WRITING_LANG_MS, ZI8_LANG_MS},  /* MALAY */
#endif
#ifdef __MMI_ZI_INDONESIAN__
    { IME_WRITING_LANG_ID, ZI8_LANG_IN},  /* INDONESIAN */
#endif
#ifdef __MMI_ZI_CZECH__
    { IME_WRITING_LANG_CS, ZI8_LANG_CS},  /* CZECH */
#endif
#ifdef __MMI_ZI_NORWEGIAN__
    { IME_WRITING_LANG_NO, ZI8_LANG_NO},  /* NORWEGIAN */
#endif
#ifdef __MMI_ZI_SLOVAK__
    { IME_WRITING_LANG_SK, ZI8_LANG_SK},  /*SLOVAK  */
#endif
#ifdef __MMI_ZI_DUTCH__
    { IME_WRITING_LANG_NL, ZI8_LANG_NL},  /*DUTCH  */
#endif
#ifdef __MMI_ZI_FINNISH__
    { IME_WRITING_LANG_FI, ZI8_LANG_FI},  /* FINNISH */
#endif
#ifdef __MMI_ZI_HUNGARIAN__
    { IME_WRITING_LANG_HU, ZI8_LANG_HU},  /* HUNGARIAN */
#endif
#ifdef __MMI_ZI_VIETNAMESE__
    { IME_WRITING_LANG_VI, ZI8_LANG_VI},  /* VIETNAMESE */
#endif
#ifdef __MMI_ZI_TURKISH__
    { IME_WRITING_LANG_TR, ZI8_LANG_TR},  /* TURKISH */
#endif
#ifdef __MMI_ZI_RUSSIAN__
    { IME_WRITING_LANG_RU, ZI8_LANG_RU},  /* RUSSIAN */
#endif
#ifdef __MMI_ZI_BULGARIAN__
    { IME_WRITING_LANG_BG, ZI8_LANG_BG},  /* BULGARIAN */
#endif
#ifdef __MMI_ZI_UKRAINIAN__
    { IME_WRITING_LANG_UK, ZI8_LANG_UK},  /* BULGARIAN */
#endif
#ifdef __MMI_ZI_ARABIC__
    { IME_WRITING_LANG_AR, ZI8_LANG_AR},  /* ARABIC */
#endif
#ifdef __MMI_ZI_PERSIAN__
    { IME_WRITING_LANG_PS, ZI8_LANG_FA},  /* PERSIAN */
#endif
#ifdef __MMI_ZI_HEBREW__
    { IME_WRITING_LANG_HE, ZI8_LANG_IW},  /* HEBREW */
#endif
#ifdef __MMI_ZI_THAI__
    { IME_WRITING_LANG_TH, ZI8_LANG_TH},  /* Thai */
#endif
#ifdef __MMI_ZI_GREEK__
    { IME_WRITING_LANG_EL, ZI8_LANG_EL},  /* GREEK */
#endif
#ifdef __MMI_ZI_TR_CHINESE__
    { IME_WRITING_LANG_ZHTW, ZI8_LANG_ZH},  /* Traditional Chinese */
#endif
#ifdef __MMI_ZI_SM_CHINESE__
    { IME_WRITING_LANG_ZHCN, ZI8_LANG_ZH},  /* Simpled Chinese */
#endif
#ifdef __MMI_ZI_SWEDISH__
    { IME_WRITING_LANG_SV, ZI8_LANG_SV},  /* SWEDISH */
#endif
#ifdef __MMI_ZI_CROATIAN__
    { IME_WRITING_LANG_SH, ZI8_LANG_HR },  /* CROATIAN */
#endif
#ifdef __MMI_ZI_ROMANIAN__
    { IME_WRITING_LANG_RO, ZI8_LANG_RO},  /* ROMANIAN */
#endif
#ifdef __MMI_ZI_SLOVENIAN__
    { IME_WRITING_LANG_SL, ZI8_LANG_SL},  /* SLOVENIAN */
#endif
#ifdef __MMI_ZI_HINDI__
    { IME_WRITING_LANG_HI, ZI8_LANG_HI},  /* HINDI */
#endif
#ifdef __MMI_ZI_MARATHI__
    { IME_WRITING_LANG_MR, ZI8_LANG_MR},  /* MARATHI */
#endif
#ifdef __MMI_ZI_TAMIL__
    {IME_WRITING_LANG_TA, ZI8_LANG_TA},     /* TAMIL */
#endif 

#ifdef __MMI_ZI_BENGALI__
    {IME_WRITING_LANG_BE, ZI8_LANG_BN}, /* BENGALI */
#endif 

#ifdef __MMI_ZI_TELUGU__
    {IME_WRITING_LANG_TE, ZI8_LANG_TE}, /* TELUGU */
#endif 

#ifdef __MMI_ZI_PUNJABI__
    {IME_WRITING_LANG_PA, ZI8_LANG_PA}, /* PUNJABI */
#endif 

#ifdef __MMI_ZI_KANNADA__
    {IME_WRITING_LANG_KN, ZI8_LANG_KN}, /* KANNADA */
#endif 

#ifdef __MMI_ZI_MALAYALAM__
    {IME_WRITING_LANG_ML, ZI8_LANG_ML}, /* MALAYALAM */
#endif 

#ifdef __MMI_ZI_GUJARATI__
    {IME_WRITING_LANG_GU, ZI8_LANG_GU}, /* GUJARATI */
#endif 

#ifdef __MMI_ZI_URDU__
    { IME_WRITING_LANG_UR, ZI8_LANG_UR},  /* URDU */
#endif
    {IME_WRITING_LANG_NONE, ZI8_LANG_GSMDEFAULT}
};
#endif /* defined(__MMI_MIXED_LANG_SUPPORT__) */



#if defined(__MMI_IME_USER_DATABASE__)

static U16 g_ezi_pud_buffer[MMI_IME_EZI_PD_SIZE/2];   
static ZI8UCHAR g_ezi_pud_id_active = 0;

static U16 g_ezi_uwd_buffer[MMI_IME_EZI_UWD_SIZE/2];   
static ZI8UCHAR g_ezi_uwd_id_active = 0;

#if defined(__MMI_ZI_SM_CHINESE__) || defined(__MMI_ZI_TR_CHINESE__)
static U16 g_ezi_uwd_zh_buffer[MMI_IME_EZI_UWD_ZH_SIZE/2];   
static ZI8UCHAR g_ezi_uwd_zh_id_active = 0;
#endif

#endif /* defined(__MMI_IME_USER_DATABASE__) */

/*****************************************************************************
 * Local variables
 *****************************************************************************/

// The Zi8 engine singleton
static mmi_ime_ezi_word_engine_struct    g_engine;

/*****************************************************************************
 *          Macros
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_set_context
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
void    mmi_ime_word_set_context( U8 context )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    switch ( context )
    {
    case MMI_IME_WORD_CONTEXT_DEFAULT:
    case MMI_IME_WORD_CONTEXT_SMS:
    case MMI_IME_WORD_CONTEXT_SURNAME:
    case MMI_IME_WORD_CONTEXT_GIVENNAME:
        g_engine.get_context = context;
        break;

    default:
        break;
    }
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_init
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
MMI_BOOL mmi_ime_word_init( void )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    if ( MMI_FALSE != g_engine.is_inited )
        return MMI_TRUE;

    memset( &g_engine, 0, sizeof(mmi_ime_ezi_word_engine_struct) );

    g_engine.get_param.pElements = &g_engine.data.elem_buff[0];
    g_engine.get_param.pCandidates = NULL;

    if (gIMELDBArray == NULL)
    {
    	MSG_FATAL("g_engine.is_inited111111111111111111111111111111111111",0,0,0);
        g_engine.is_inited = MMI_TRUE;
    }
    else if (gIMELDBArray[0].Language == 0 && gIMELDBArray[0].pTable == 0) /* We can't go to fail when there is not Zi resource */
    {
    	MSG_FATAL("g_engine.is_inited222222222222222222222222222",0,0,0);
        g_engine.is_inited = MMI_TRUE;
    }
    else
    {
        g_engine.is_inited = Zi8InitializeDynamic((PZI8VOID)gIMELDBArray); /* Initialize LDB table */
		MSG_FATAL("g_engine.is_inited========%d",g_engine.is_inited,0,0);
    }
    
#if defined(__MMI_IME_USER_DATABASE__)

    g_engine.is_inited = mmi_ime_ezi_init_user_database(); /* Initialize UDB */

#endif /* defined(__MMI_IME_USER_DATABASE__) */
   
    return g_engine.is_inited;
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_deinit
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
void mmi_ime_word_deinit( void )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    if ( MMI_FALSE != g_engine.is_inited )
    {
        memset( &g_engine, 0, sizeof(mmi_ime_ezi_word_engine_struct) );
        g_engine.is_inited = MMI_FALSE;

    #if defined(__MMI_IME_USER_DATABASE__)
        if (g_ezi_pud_id_active > 0) 
        {
            Zi8DetachPUD(g_ezi_pud_id_active, (PZI8UCHAR)&g_ezi_pud_buffer[0]);
        }

        if (g_ezi_uwd_id_active > 0) 
        {
            Zi8DetachUWD(g_ezi_uwd_id_active, (PZI8UCHAR)&g_ezi_uwd_buffer[0]);
        }

    #if defined(__MMI_ZI_SM_CHINESE__) || defined(__MMI_ZI_TR_CHINESE__)
        if (g_ezi_uwd_zh_id_active > 0) 
        {
            Zi8DetachUWD_ZH(g_ezi_uwd_zh_id_active, (PZI8UCHAR)&g_ezi_uwd_zh_buffer[0]);
        }
    #endif

    #endif /* defined(__MMI_IME_USER_DATABASE__) */

    }
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_leave_current_mode
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
void  mmi_ime_word_leave_current_mode(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    g_engine.curr_mode_info = NULL;
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_change_mode
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
MMI_BOOL mmi_ime_word_change_mode(const sIMEModeDetails * mode_details_ptr, U32 reserved )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    ZI8BOOL                             ret_val;
    mmi_ime_ezi_mode_info_struct_p      mode_info_ptr;

#if defined(__MMI_MIXED_LANG_SUPPORT__)
    mmi_imm_input_mode_enum             sub_lang = IME_WRITING_LANG_NONE;
#endif /* defined(__MMI_MIXED_LANG_SUPPORT__) */
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    if ( MMI_FALSE == g_engine.is_inited )
        return MMI_FALSE;

    mode_info_ptr = (mmi_ime_ezi_mode_info_struct_p)mmi_ime_ezi_query_mode_info( (mmi_imm_input_mode_enum)mode_details_ptr->imm_mode_id );

    g_engine.curr_mode_info = NULL;
    
    if ( NULL == mode_info_ptr )
        return MMI_FALSE;

    g_engine.get_param.Language     = mode_info_ptr->language;
    g_engine.get_param.SubLanguage  = mode_info_ptr->sub_language;

#if defined(__MMI_MIXED_LANG_SUPPORT__)
	if (g_engine.get_param.SubLanguage == ZI8_SUBLANG_DEFAULT)
	{
		sub_lang = mmi_imm_get_sub_lang();

		if (sub_lang != IME_WRITING_LANG_NONE)
		{
			g_engine.get_param.SubLanguage = mmi_ime_ezi_map_ime_lang_to_zi_lang(sub_lang);
		}
	}
#endif /* defined(__MMI_MIXED_LANG_SUPPORT__) */


    if ( (mode_details_ptr->Mode_Special_Attrib & IME_MODE_ATTR_SMART) &&
         (mode_details_ptr->Lang_Special_Attrib & IME_LANG_ATTR_ALPHABETIC) )
    {
        ret_val = Zi8SetPredictionStartAt( MMI_IME_PREDICTION_START_AT );
        if ( ret_val != ZI8TRUE )
        {
            /* add error */
            return MMI_FALSE;
        }
    }
	else
	{
        ret_val = Zi8SetPredictionStartAt(255); /* Turn off prediction */
        if ( ret_val != ZI8TRUE )
        {
            /* add error */
            return MMI_FALSE;
        }
	}

    if ( (mode_details_ptr->Mode_Special_Attrib & IME_MODE_ATTR_UPPERCASE) )
    {
        g_engine.case_type = MMI_IME_WORD_CAPITAL_STATE_UPPER;
    }
    else if ( mode_details_ptr->Mode_Special_Attrib & IME_MODE_ATTR_LOWERCASE )
    {
        g_engine.case_type = MMI_IME_WORD_CAPITAL_STATE_LOWER;
    }
    else if ( mode_details_ptr->Mode_Special_Attrib & IME_MODE_ATTR_TITLECASE )
    {
        g_engine.case_type = MMI_IME_WORD_CAPITAL_STATE_FIRST_UPPER;
    }
    else
    {
        g_engine.case_type = MMI_IME_WORD_CAPITAL_STATE_NONE;
    }

    g_engine.get_param.FirstCandidate       = 0;
    g_engine.get_param.MaxCandidates        = 0;
    g_engine.get_param.WordCharacterCount   = 0;
    g_engine.get_param.ElementCount         = 0;
    
    //g_engine.get_param.pElements     = NULL;
    g_engine.get_param.pCandidates   = NULL;
    g_engine.get_param.pCurrentWord  = NULL;

    g_engine.curr_mode_info = mode_info_ptr; /* Append new input mode */
    
    return MMI_TRUE;
}   
 

/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_is_valid_key
 * DESCRIPTION
 * PARAMETERS
 *  input_mode    [IN]    Input mode
 * RETURNS
 *  Is valid key for this input mode
 *****************************************************************************/
MMI_BOOL mmi_ime_word_is_valid_key( mmi_imm_input_mode_enum input_mode, U16 key_code )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mmi_ime_ezi_mode_info_struct_p  mode_info_p;
    U32 required_key;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    mode_info_p = (mmi_ime_ezi_mode_info_struct_p)mmi_ime_ezi_query_mode_info(input_mode);

    if (mode_info_p == NULL)
        return MMI_FALSE;

    required_key = mode_info_p->required_keys;
    
    switch(key_code)
    {
    case AVK_SELECT:
    case AVK_CLR:
    case AVK_INFO:
    case AVK_UP:
    case AVK_DOWN:
    case AVK_LEFT:
    case AVK_RIGHT:
        return MMI_TRUE;
    case AVK_0:
    case AVK_1:
    case AVK_2:
    case AVK_3:
    case AVK_4:
    case AVK_5:
    case AVK_6:
    case AVK_7:
    case AVK_8:
    case AVK_9:
        if (required_key & (MMI_IME_REQUIRED_KEY_0 << (key_code - AVK_0)))
        {
            return MMI_TRUE;
        }
        else
        {
            return MMI_FALSE;
        }
    case AVK_STAR:
        if (required_key & MMI_IME_REQUIRED_KEY_STAR)
        {
            return MMI_TRUE;
        }
        else
        {
            return MMI_FALSE;
        }
    case AVK_POUND:
        if (required_key & MMI_IME_REQUIRED_KEY_POUND)
        {
            return MMI_TRUE;
        }
        else
        {
            return MMI_FALSE;
        }
    default:
        return MMI_FALSE;
    }
    return MMI_FALSE;
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_get_candidates
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
void mmi_ime_word_get_candidates( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    result_ptr->result_cnt = 0;
    if (g_engine.curr_mode_info)
    {
        if ( NULL != g_engine.curr_mode_info->get_cand_func )
        {
            g_engine.curr_mode_info->get_cand_func( param_ptr, result_ptr);
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_get_associates
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
void mmi_ime_word_get_associates( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    result_ptr->result_cnt = 0;
    
    if (g_engine.curr_mode_info)
    {
        if ( NULL != g_engine.curr_mode_info->get_assoc_func )
        {
            g_engine.curr_mode_info->get_assoc_func( param_ptr, result_ptr );
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_get_spellings
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
void mmi_ime_word_get_spellings( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    result_ptr->result_cnt = 0;

    if (g_engine.curr_mode_info)
    {
        if ( NULL != g_engine.curr_mode_info->get_spell_func )
        {
            g_engine.curr_mode_info->get_spell_func( param_ptr, result_ptr );
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_symbol_to_code
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
UI_character_type mmi_ime_word_symbol_to_code(U16 symbol)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    if (MMI_IMC_IS_SYMB_TONE(symbol))
    {
        return mmi_ime_ezi_zicode_to_code(g_ezi_codes_tone[symbol - MMI_IMC_SYMB_TONE_BASE]);
    }
#if defined(__MMI_ZI_SM_CHINESE__) || defined(__MMI_ZI_TR_CHINESE__)
    else if (MMI_IMC_IS_SYMB_STROKE(symbol))
    {
        return mmi_ime_ezi_zicode_to_code(g_ezi_codes_stroke[symbol - MMI_IMC_SYMB_STROKE_BASE]);
    }
#endif /* defined(__MMI_ZI_SM_CHINESE__) || defined(__MMI_ZI_TR_CHINESE__) */
#if defined(__MMI_ZI_SM_CHINESE__)
    else if (MMI_IMC_IS_SYMB_PY(symbol))
    {
        return mmi_ime_ezi_zicode_to_code(g_ezi_codes_pinyin[symbol - MMI_IMC_SYMB_PY_BASE]);
    }
#endif /* #if defined(__MMI_ZI_SM_CHINESE__) */
#if defined(__MMI_ZI_TR_CHINESE__)
    else if (MMI_IMC_IS_SYMB_ZY(symbol))
    {
        return mmi_ime_ezi_zicode_to_code(g_ezi_codes_bopomofo[symbol - MMI_IMC_SYMB_ZY_BASE]);
    }
#endif /* #if defined(__MMI_ZI_TR_CHINESE__) */
    else if (Zi8IsComponent(symbol))
    {
        return symbol;
    }
    else if (symbol == MMI_IMC_SYMB_SEPARATER)
    {
        return 0x0027;
    }
    else
    {
        return 0;
    }
}

/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_code_to_symbol
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
U16  mmi_ime_word_code_to_symbol(UI_character_type code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    switch (code)
    {
    case 0x002D:
        return MMI_IMC_SYMB_TONE_1;
        
    case 0x02CA:
        return MMI_IMC_SYMB_TONE_2;
        
    case 0x02C7:
        return MMI_IMC_SYMB_TONE_3;
        
    case 0x02CB:
        return MMI_IMC_SYMB_TONE_4;
        
    case 0x02D9:
        return MMI_IMC_SYMB_TONE_5;
    case 0x0027:
        return MMI_IMC_SYMB_SEPARATER;
#if defined(__MMI_ZI_SM_CHINESE__) || defined(__MMI_ZI_TR_CHINESE__)        
    case ZI8_CODE_WILDCARD:      
        return MMI_IMC_SYMB_STROKE_WILDCARD;
        
    case 0x2026:     
        return MMI_IMC_SYMB_STROKE_MORE;
        
    case ZI8_CODE_LEFT:          
        return MMI_IMC_SYMB_STROKE_LEFT;
        
    case ZI8_CODE_OVER:
        return MMI_IMC_SYMB_STROKE_OVER;
        
    case ZI8_CODE_DOT:           
        return MMI_IMC_SYMB_STROKE_DOT;
        
    case ZI8_CODE_DOWN:  
        return MMI_IMC_SYMB_STROKE_DOWN;
        
    case ZI8_CODE_TURN:           
        return MMI_IMC_SYMB_STROKE_TURN;
        
    case ZI8_CODE_DOWN_OVER:      
        return MMI_IMC_SYMB_STROKE_DOWN_OVER;
        
    case ZI8_CODE_OVER_DOWN:      
        return MMI_IMC_SYMB_STROKE_OVER_DOWN;
        
    case ZI8_CODE_CURVED_HOOK:    
        return MMI_IMC_SYMB_STROKE_CURVED_HOOK;
        
    case ZI8_CODE_OVER_DOWN_OVER: 
        return MMI_IMC_SYMB_STROKE_OVER_DOWN_OVER;
#endif /* defined(__MMI_ZI_SM_CHINESE__) || defined(__MMI_ZI_TR_CHINESE__) */        
    default:
#ifdef __MMI_ZI_SM_CHINESE__
        if (code >= 0x0061 && code <= 0x007A) 
        {
            return code - 0x0061 + MMI_IMC_SYMB_PY_BASE; /* mmi_imc_symbol_pinyin_enum */
        }
        else 
#endif /* __MMI_ZI_SM_CHINESE__ */
#ifdef __MMI_ZI_TR_CHINESE__
        if (code >= 0x3105 && code <= 0x3129)
        {
            return code - 0x3105 + MMI_IMC_SYMB_ZY_BASE; /* mmi_imc_symbol_bopomofo_enum */
        }
        else
#endif /* __MMI_ZI_TR_CHINESE__ */
        {
            return 0xFF;
        }
        break;
    }
    
    return 0xFF; 
}

/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_is_symbol_key
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
MMI_BOOL  mmi_ime_word_is_symbol_key( mmi_imm_input_mode_enum input_mode, mmi_imc_key_custom_value key_code )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mmi_ime_ezi_mode_info_struct_p mode_info = NULL;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (input_mode != g_engine.curr_mode_info->input_mode)
    {
        mode_info = (mmi_ime_ezi_mode_info_struct_p)mmi_ime_ezi_query_mode_info( input_mode );
    }
    else
    {
        mode_info = g_engine.curr_mode_info;
    }
    
    switch(key_code)
    {
    case MMI_IMC_KEY_UP:
    case MMI_IMC_KEY_DOWN:
    case MMI_IMC_KEY_LEFT:
    case MMI_IMC_KEY_RIGHT:
    case MMI_IMC_KEY_CONFIRM: 
    case MMI_IMC_KEY_CANCEL: 
    case MMI_IMC_KEY_CLEAR: 
        return MMI_FALSE;
    case MMI_IMC_KEY_SYM_0:
    case MMI_IMC_KEY_SYM_1:
    case MMI_IMC_KEY_SYM_2:
    case MMI_IMC_KEY_SYM_3:
    case MMI_IMC_KEY_SYM_4:
    case MMI_IMC_KEY_SYM_5:
    case MMI_IMC_KEY_SYM_6:
    case MMI_IMC_KEY_SYM_7:
    case MMI_IMC_KEY_SYM_8:
    case MMI_IMC_KEY_SYM_9:
        if (mode_info->symbol_keys & (MMI_IME_REQUIRED_KEY_0 << (key_code - MMI_IMC_KEY_SYM_0)))
        {
            return MMI_TRUE;
        }
        else
        {
            return MMI_FALSE;
        }
    case MMI_IMC_KEY_SYM_STAR:
        if (mode_info->symbol_keys & MMI_IME_REQUIRED_KEY_STAR)
        {
            return MMI_TRUE;
        }
        else
        {
            return MMI_FALSE;
        }
    case MMI_IMC_KEY_SYM_POUND:
        if (mode_info->symbol_keys & MMI_IME_REQUIRED_KEY_POUND)
        {
            return MMI_TRUE;
        }
        else
        {
            return MMI_FALSE;
        }
    default:
        return MMI_FALSE;
    }
    return MMI_FALSE;
}

/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_add_separator_to_candidate_buffer
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
void mmi_ime_word_add_separator_to_candidate_buffer(UI_character_type * destination_buffer, UI_character_type * source_buffer, S32 num)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 i = 0, j = 0;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    while (i < num && source_buffer[i] != 0)
    {
        destination_buffer[j] = source_buffer[i];
        destination_buffer[j + 1] = 0; /* Default separator is '\0' */
        i++;
        j += 2;
    }

    destination_buffer[j] = 0;
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_engine_get_char_info
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
MMI_BOOL mmi_ime_word_engine_get_char_info(UI_character_type code, UI_string_type info_buffer, S32 buffer_size, mmi_imm_input_mode_enum mode)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ZI8UCHAR get_mode;
    S32 i, j;
    ZI8UCHAR ret;
	ZI8GETPARAM zi8GetParam;
    ZI8WCHAR tmpCandBuffer[2];
	UI_character_type char_code;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

	memset(&zi8GetParam, 0, sizeof(zi8GetParam));

    switch(mode)
    {
    case IMM_INPUT_MODE_QUICK_SEARCH_BOPOMO:
    case IMM_INPUT_MODE_TR_BOPOMO:
        get_mode = ZI8_GETMODE_BOPOMOFO_NAME;
		zi8GetParam.GetMode = ZI8_GETMODE_BOPOMOFO;
		zi8GetParam.Language = ZI8_LANG_ZH;
		zi8GetParam.SubLanguage = ZI8_SUBLANG_ZH_TW;
        break;
        
    case IMM_INPUT_MODE_QUICK_SEARCH_PINYIN:
    case IMM_INPUT_MODE_SM_PINYIN:
        get_mode = ZI8_GETMODE_PINYIN_NAME;
		zi8GetParam.GetMode = ZI8_GETMODE_PINYIN;
		zi8GetParam.Language = ZI8_LANG_ZH;
		zi8GetParam.SubLanguage = ZI8_SUBLANG_ZH;
        break;
        
    case IMM_INPUT_MODE_QUICK_SEARCH_SM_STROKE:
    case IMM_INPUT_MODE_SM_STROKE:
		get_mode = ZI8_GETMODE_STROKES;
		zi8GetParam.GetMode = ZI8_GETMODE_STROKES;
		zi8GetParam.Language = ZI8_LANG_ZH;
		zi8GetParam.SubLanguage = ZI8_SUBLANG_ZH;
        break;

    case IMM_INPUT_MODE_QUICK_SEARCH_TR_STROKE:
    case IMM_INPUT_MODE_TR_STROKE:
        get_mode = ZI8_GETMODE_STROKES;
		zi8GetParam.GetMode = ZI8_GETMODE_STROKES;
		zi8GetParam.Language = ZI8_LANG_ZH;
		zi8GetParam.SubLanguage = ZI8_SUBLANG_ZH_TW;
        break;
        
    default:
        return MMI_FALSE;
        break;
    }

    zi8GetParam.Context = ZI8_GETCONTEXT_SMS;
    zi8GetParam.GetOptions = ZI8_GETOPTION_DEFAULT;
    zi8GetParam.pCandidates = tmpCandBuffer;
    zi8GetParam.MaxCandidates = 1;

	Zi8GetCandidates(&zi8GetParam); /* Set Chinese Language to ZI, otherwise Zi8GetCharInfo maybe can not work */

    ret = Zi8GetCharInfo((ZI8WCHAR)code, (PZI8WCHAR) info_buffer, (ZI8UCHAR)buffer_size, (ZI8UCHAR)get_mode);

    for (i = 0, j = 0; i < buffer_size && info_buffer[i]; i++)
    {
        char_code = mmi_ime_ezi_zicode_to_code(info_buffer[i]);

		if (!MMI_IMC_IS_SYMB_TONE(mmi_ime_word_code_to_symbol(char_code)))
        {
            info_buffer[j++] = char_code;
        }
    }

    info_buffer[j] = 0;

    if (ret)
    {
        return MMI_TRUE;
    }
    else
    {
        return MMI_FALSE;
    }
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_is_redical
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
MMI_BOOL  mmi_ime_word_is_redicals(UI_character_type key_code )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (Zi8IsComponent(key_code))
        return MMI_TRUE;
    return MMI_FALSE;
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_is_wildcard
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
MMI_BOOL mmi_ime_word_is_wildcard(UI_character_type code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (code == ZI8_CODE_WILDCARD)
        return MMI_TRUE;
    return MMI_FALSE;
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_change_word_case
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
void   mmi_ime_word_change_word_case(mmi_ime_word_capital_state_enum capital_state)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    g_engine.case_type = capital_state;
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_get_candidates_alphabetic
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
static void  mmi_ime_ezi_get_candidates_alphabetic( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    ASSERT( NULL != param_ptr );
    
    if ( 0 == param_ptr->elembuf_cnt )
        return;
    
    mmi_ime_ezi_fill_elem_buff( param_ptr, MMI_IMC_SYMB_KEY_BASE, MMI_IMC_SYMB_KEY_MAX, g_ezi_keys_latin );
    if ( g_engine.get_param.ElementCount == 0 )
    {
        /* add error */
        return;
    }
    
	switch (g_engine.case_type)
    {
    case MMI_IME_WORD_CAPITAL_STATE_FIRST_UPPER:
		 Zi8SetCandidateCase(ZI8_CASE_TITLE);
	     break;

    case MMI_IME_WORD_CAPITAL_STATE_UPPER:
		Zi8SetCandidateCase(ZI8_CASE_UPPER);
		 break;

    case MMI_IME_WORD_CAPITAL_STATE_LOWER:
		Zi8SetCandidateCase(ZI8_CASE_LOWER);
		 break;

	default:
		Zi8SetCandidateCase(ZI8_CASE_DEFAULT);
		 break;
    }

    mmi_ime_ezi_get_candidates( param_ptr, result_ptr, ZI8_GETMODE_DEFAULT, ZI8_GETOPTION_WSTRINGS );

    if ( result_ptr->result_cnt > 0 )
    {
        /* adjust result case */
        mmi_ime_ezi_change_candidates_case(result_ptr);
    }
#if defined(__MMI_IME_USER_DATABASE__)
    else if (param_ptr->first_index == 0)
    {
        result_ptr->result_cnt = 1;
        result_ptr->next_page = MMI_FALSE;
        result_ptr->result_buffer[0] = MMI_IME_USER_DATABASE_NEW_WORD;
        result_ptr->result_buffer[1] = 0;
    }
#endif

}

#if defined(__MMI_ZI_SM_CHINESE__) || defined(__MMI_ZI_TR_CHINESE__)
/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_get_spellings_stroke
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
static void   mmi_ime_ezi_get_spellings_stroke( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8  i;
    U8  symb;
    UI_string_type temp_str;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    ASSERT( NULL != param_ptr );
    if ( 0 == param_ptr->elembuf_cnt )
        return;
    
    temp_str = result_ptr->result_buffer;
    
    for ( i = 0; i < param_ptr->elembuf_cnt; ++i )
    {
        symb = param_ptr->elembuf_ptr[i];
        if ( MMI_FALSE != MMI_IMC_IS_SYMB_KEY(symb) )
        {
            if (g_ezi_keys_stroke[symb - MMI_IMC_SYMB_KEY_BASE])
            {
                *temp_str = mmi_ime_word_symbol_to_code((U8)g_ezi_keys_stroke[symb - MMI_IMC_SYMB_KEY_BASE]);
                temp_str++;
            }
            else /* Invalid key */
            {      
                i = 0;
                temp_str = result_ptr->result_buffer;
                break;
            }
        }
        else
        {
            i = 0;
            temp_str = result_ptr->result_buffer;
            break;
        }       
    }

    *temp_str = 0;

    result_ptr->result_cnt = i;
    
}

/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_get_candidates_stroke
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
static void   mmi_ime_ezi_get_candidates_stroke( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    ASSERT( NULL != param_ptr );
    if ( 0 == param_ptr->elembuf_cnt )
    {
        return;
    }
    
    mmi_ime_ezi_fill_elem_buff( param_ptr, MMI_IMC_SYMB_STROKE_BASE, MMI_IMC_SYMB_STROKE_MAX, g_ezi_codes_stroke );
    if ( g_engine.get_param.ElementCount == 0 )
    {
        /* add error */
        return;
    }

#if defined(__MMI_ZI_CHINESE_PHRASE_LEVEL_INPUTS__)
    mmi_ime_ezi_get_candidates( param_ptr, result_ptr, ZI8_GETMODE_PHRASE_STROKES, ZI8_GETOPTION_DEFAULT );
#else
    mmi_ime_ezi_get_candidates( param_ptr, result_ptr, ZI8_GETMODE_STROKES, ZI8_GETOPTION_DEFAULT );
#endif
}
#endif /* #if defined(__MMI_ZI_SM_CHINESE__) || defined(__MMI_ZI_TR_CHINESE__) */

#if defined(__MMI_ZI_SM_CHINESE__)

#if defined(__MMI_ZI_MULTITAP_PHONETIC_INPUT__)
/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_get_spellings_pinyin_multitap
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
static void   mmi_ime_ezi_get_spellings_pinyin_multitap( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    U8          i;
    UI_string_type temp_str;
    U8          symb;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    ASSERT( NULL != param_ptr );
    if ( 0 == param_ptr->elembuf_cnt )
        return;

    temp_str = result_ptr->result_buffer;
    
    for ( i = 0; i < param_ptr->elembuf_cnt; i++ )
    {
        symb = param_ptr->elembuf_ptr[i];
        if ( MMI_FALSE != MMI_IMC_IS_SYMB_PY( symb ) )
        {
            *temp_str = mmi_ime_word_symbol_to_code(symb);
            temp_str++;
        }
        else
        {
            i = 0;
            temp_str = result_ptr->result_buffer;
            break;
        }
    }

    *temp_str = 0;
    result_ptr->result_cnt = i;
}



#endif /* __MMI_ZI_MULTITAP_PHONETIC_INPUT__ */

#if defined(__MMI_ZI_SMART_PHONETIC_INPUT__)
/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_get_spellings_pinyin_smart
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
static void mmi_ime_ezi_get_spellings_pinyin_smart( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8  cnt;
    UI_string_type  temp_result_ptr;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    //ASSERT( NULL != param_ptr );
    
    if ( 0 == param_ptr->elembuf_cnt )
        return;
    
    mmi_ime_ezi_fill_elem_buff( param_ptr, MMI_IMC_SYMB_KEY_BASE, MMI_IMC_SYMB_KEY_MAX, g_ezi_onekeys );
    if ( g_engine.get_param.ElementCount == 0 )
    {
        /* add error */
        return;
    }

    mmi_ime_ezi_get_candidates( param_ptr, result_ptr, ZI8_GETMODE_1KEYPRESS_PINYIN, ZI8_GETOPTION_GET_SPELLING );
    if ( result_ptr->result_cnt > 0 )
    {
        temp_result_ptr = result_ptr->result_buffer;

        cnt = 0;
        while ( cnt < result_ptr->result_cnt)
        {
            if ((*temp_result_ptr) != 0)
            {
                *temp_result_ptr = mmi_ime_ezi_zicode_to_code(*temp_result_ptr);
            }
            else
            {
                cnt++;
            }
            
            temp_result_ptr++;
        }
    }
}

#endif /* __MMI_ZI_SMART_PHONETIC_INPUT__ */

/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_get_candidates_pinyin
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
static void mmi_ime_ezi_get_candidates_pinyin( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    ASSERT( NULL != param_ptr );
    if ( 0 == param_ptr->elembuf_cnt )
        return;
    
    mmi_ime_ezi_fill_elem_buff( param_ptr, MMI_IMC_SYMB_PY_BASE, MMI_IMC_SYMB_PY_MAX, g_ezi_codes_pinyin );
    if ( g_engine.get_param.ElementCount == 0 )
    {
        /* add error */
        return;
    }

    mmi_ime_ezi_get_candidates( param_ptr, result_ptr, ZI8_GETMODE_PINYIN, ZI8_GETOPTION_DEFAULT );
}

#endif /* __MMI_ZI_SM_CHINESE__ */



#if defined(__MMI_ZI_TR_CHINESE__)

#if defined(__MMI_ZI_MULTITAP_PHONETIC_INPUT__)
/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_get_candidates_pinyin_multitap
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
static void   mmi_ime_ezi_get_spellings_zhuyin_multitap( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    U8          i;
    ZI8WCHAR    symb;
    UI_string_type temp_str;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    ASSERT( NULL != param_ptr );
    if ( 0 == param_ptr->elembuf_cnt )
        return;

    temp_str = result_ptr->result_buffer;
        
    for ( i = 0; i < param_ptr->elembuf_cnt; i++ )
    {
        symb = param_ptr->elembuf_ptr[i];
        if ( MMI_FALSE != MMI_IMC_IS_SYMB_ZY(symb) )
        {
            *temp_str = mmi_ime_word_symbol_to_code((U8)symb);
            temp_str++;
        }
        else
        {
            i = 0;
            temp_str = result_ptr->result_buffer;
            break;
        }
    }
    
    *temp_str = 0;
    result_ptr->result_cnt = i;
}

#endif /* __MMI_ZI_MULTITAP_PHONETIC_INPUT__) */


#if defined(__MMI_ZI_SMART_PHONETIC_INPUT__)
/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_get_spellings_zhuyin_smart
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
static void   mmi_ime_ezi_get_spellings_zhuyin_smart( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8  cnt;
    UI_string_type  temp_result_ptr;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    //ASSERT( NULL != param_ptr );
    if ( 0 == param_ptr->elembuf_cnt )
        return;
    
    mmi_ime_ezi_fill_elem_buff( param_ptr, MMI_IMC_SYMB_KEY_BASE, MMI_IMC_SYMB_KEY_MAX, g_ezi_onekeys );
    if ( g_engine.get_param.ElementCount == 0 )
    {
        /* add error */
        return;
    }

    mmi_ime_ezi_get_candidates( param_ptr, result_ptr, ZI8_GETMODE_1KEYPRESS_BOPOMOFO, ZI8_GETOPTION_GET_SPELLING );

    if ( result_ptr->result_cnt > 0 )
    {
        temp_result_ptr = result_ptr->result_buffer;
        
        cnt = 0;
        while ( cnt < result_ptr->result_cnt)
        {
            if (*temp_result_ptr != 0)
            {
                *temp_result_ptr = mmi_ime_ezi_zicode_to_code(*temp_result_ptr);
            }
            else
            {
                cnt++;
            }
            temp_result_ptr++;
        }
    }
}

#endif /* __MMI_ZI_SMART_PHONETIC_INPUT__ */

/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_get_candidates_zhuyin
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
static void   mmi_ime_ezi_get_candidates_zhuyin( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    ASSERT( NULL != param_ptr );
    if ( 0 == param_ptr->elembuf_cnt )
        return;
    
    mmi_ime_ezi_fill_elem_buff( param_ptr, MMI_IMC_SYMB_ZY_BASE, MMI_IMC_SYMB_ZY_MAX, g_ezi_codes_bopomofo );
    if ( g_engine.get_param.ElementCount == 0 )
    {
        /* add error */
        return;
    }

    mmi_ime_ezi_get_candidates( param_ptr, result_ptr, ZI8_GETMODE_BOPOMOFO, ZI8_GETOPTION_DEFAULT );
}

#endif /* #if defined(__MMI_ZI_TR_CHINESE__) */


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_fill_elem_buff
 * DESCRIPTION
 *  Fill element buffer by eZi symbol according to IMC symbol
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
static void mmi_ime_ezi_fill_elem_buff( mmi_ime_query_param_struct_p param_ptr,
                                        U8 symb_range_begin, U8 symb_range_end, const ZI8WCHAR *symb_conv_table )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    U8          i;
    U16         symb_code;
    ZI8WCHAR    zi_code;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    ASSERT( symb_range_begin <= symb_range_end );

    g_engine.get_param.ElementCount = 0;
    
    for ( i = 0; i < param_ptr->elembuf_cnt; ++i )
    {
        symb_code = param_ptr->elembuf_ptr[i];
        if ( symb_code >= symb_range_begin && symb_code < symb_range_end )
        {
            zi_code = symb_conv_table[symb_code - symb_range_begin];
            if ( zi_code == 0 )
                break;
            g_engine.get_param.pElements[i] = zi_code;
            g_engine.get_param.ElementCount++;
        }
        else if (Zi8IsComponent(symb_code))
        {
            g_engine.get_param.pElements[i] = symb_code;
            g_engine.get_param.ElementCount++;
        }
        else if (symb_code == MMI_IMC_SYMB_SEPARATER)
        {
            g_engine.get_param.pElements[i] = ZI8_CODE_SEPARATOR;
            g_engine.get_param.ElementCount++;
        }
        else
        {
            break;
        }
    }

    if ( i < param_ptr->elembuf_cnt )
    {
        g_engine.get_param.pElements[0] = 0;
        g_engine.get_param.ElementCount = 0;
    }
    else
    {
        symb_code = param_ptr->tone_value;
        if ( (symb_code >= MMI_IMC_SYMB_TONE_BASE && symb_code < MMI_IMC_SYMB_TONE_MAX) )
        {
            zi_code = g_ezi_codes_tone[symb_code - MMI_IMC_SYMB_TONE_BASE];
            if ( zi_code != 0 )
            {
                g_engine.get_param.pElements[g_engine.get_param.ElementCount] = zi_code;
                g_engine.get_param.ElementCount++;
            }
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_get_candidates
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
static void mmi_ime_ezi_get_candidates( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr,
                                        ZI8UCHAR get_mode, ZI8UCHAR get_options )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    UI_character_type candidate_buffer[MMI_IME_EZI_MAX_TEMP_BUFFER_SIZE];
    S32 i, j, k; 
	ZI8GETPARAM      temp_get_param;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    if ( param_ptr->first_index >= MMI_IMC_MAX_CANDIDATE_BUFFSIZE )
    {
        result_ptr->result_cnt       = 0;
        result_ptr->result_buffer[0]    = '\0';
        return;
    }

	candidate_buffer[MMI_IME_EZI_MAX_TEMP_BUFFER_SIZE - 1] = MMI_IME_EZI_TEMP_BUFFER_FLAG;

    switch ( g_engine.get_context )
    {
    case MMI_IME_WORD_CONTEXT_SMS:
        g_engine.get_param.Context = ZI8_GETCONTEXT_SMS;
        break;

    case MMI_IME_WORD_CONTEXT_SURNAME:
        g_engine.get_param.Context = ZI8_GETCONTEXT_SURNAME;
        break;

    case MMI_IME_WORD_CONTEXT_GIVENNAME:
        g_engine.get_param.Context = ZI8_GETCONTEXT_GIVENNAME;
        break;
        
    case MMI_IME_WORD_CONTEXT_DEFAULT:
    default:
        g_engine.get_param.Context = ZI8_GETCONTEXT_DEFAULT;
        break;
    }

    g_engine.get_param.Context |= ZI8_GETCONTEXT_NO_RADICALS;

#if defined(__MMI_ZI_CHINESE_PHRASE_LEVEL_INPUTS__)
    g_engine.get_param.Context |= ZI8_GETCONTEXT_WORD_CAND;
#else
    g_engine.get_param.Context |= ZI8_GETCONTEXT_SINGLE_CHAR;
#endif
    
    g_engine.get_param.GetMode      = get_mode;
    g_engine.get_param.GetOptions   = get_options;

    g_engine.get_param.MaxCandidates = param_ptr->desired_cnt + 1; /* Plus 1 for getting next page info */
    g_engine.get_param.WordCharacterCount   = 0;
    g_engine.get_param.FirstCandidate       = (ZI8WCHAR)param_ptr->first_index;
    g_engine.get_param.pCandidates          = (PZI8WCHAR)candidate_buffer;
    
    /* This is a tricky way to resolve a issue of ZI */
    /* This segment must be removed after Zi has fixed the bug  */
	memset(&temp_get_param, 0, sizeof(ZI8GETPARAM));

	memcpy(&temp_get_param, &g_engine.get_param, sizeof(ZI8GETPARAM));

    temp_get_param.ElementCount = 0;

    Zi8GetCandidates(&temp_get_param);

    result_ptr->result_cnt = Zi8GetCandidates( &g_engine.get_param );
	
	//dele by yangdecai 2011-07-05
	//MMI_ASSERT(candidate_buffer[MMI_IME_EZI_MAX_TEMP_BUFFER_SIZE - 1] == MMI_IME_EZI_TEMP_BUFFER_FLAG);

    if (result_ptr->result_cnt) /* Get next page info and resume candidate list */
    {
        
        if (get_options == ZI8_GETOPTION_WSTRINGS 
            || get_options ==  ZI8_GETOPTION_GET_SPELLING
            || (g_engine.get_param.Context & ZI8_GETCONTEXT_WORD_CAND))
        {
            i = 0;
            j = 0;
            k = 0;

            while( i < result_ptr->result_cnt)
            {
            #if defined(__MMI_ZI_CHINESE_PHRASE_LEVEL_INPUTS__)
                if (candidate_buffer[j] >= ZI8_CODE_PY_SEPARATOR_A && candidate_buffer[j] <= ZI8_CODE_PY_SEPARATOR_Z)
                {
                    result_ptr->result_buffer[k++] = ZI8_CODE_SEPARATOR;

                    result_ptr->result_buffer[k] = candidate_buffer[j] - ZI8_CODE_PY_SEPARATOR_A + ZI8_CODE_PINYIN_A;
                }
                else
            #endif /* #if defined(__MMI_ZI_CHINESE_PHRASE_LEVEL_INPUTS__) */
                {
                    result_ptr->result_buffer[k] = candidate_buffer[j];
                }
            
                if (candidate_buffer[j] == 0 || candidate_buffer[j] == 0x20)
                {
                    result_ptr->result_buffer[k] = 0;
                    i++;
                }
                j++;
                k++;
            }
            
            result_ptr->result_buffer[k] = 0;
        }
        else 
        {
            mmi_ime_word_add_separator_to_candidate_buffer(result_ptr->result_buffer, candidate_buffer, result_ptr->result_cnt);
        }
        mmi_ime_ezi_check_page_info_and_resume_result(result_ptr);
    }
}

#if defined(__MMI_ZI_SM_CHINESE__) || defined(__MMI_ZI_TR_CHINESE__)
/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_get_associates
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
static void  mmi_ime_ezi_get_associates( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    UI_character_type candidate_buffer[MMI_IME_EZI_MAX_TEMP_BUFFER_SIZE];
    UI_character_type current_word[MMI_IMC_MAX_PRE_STRING_LEN + 2];
    S32 len, i, j;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	candidate_buffer[MMI_IME_EZI_MAX_TEMP_BUFFER_SIZE - 1] = MMI_IME_EZI_TEMP_BUFFER_FLAG;
    
    g_engine.get_param.Context      = ZI8_GETCONTEXT_DEFAULT;

#if defined(__MMI_ZI_CHINESE_PHRASE_LEVEL_INPUTS__)
    g_engine.get_param.Context      |= ZI8_GETCONTEXT_WORD_CAND;
#endif

    g_engine.get_param.GetMode      = ZI8_GETMODE_DEFAULT;
    g_engine.get_param.GetOptions   = ZI8_GETOPTION_DEFAULT;
    
    g_engine.get_param.ElementCount     = 0;

    g_engine.get_param.MaxCandidates    = (ZI8UCHAR)param_ptr->desired_cnt + 1; /* Plus 1 for getting next page info */
    g_engine.get_param.FirstCandidate   = (ZI8WCHAR)param_ptr->first_index;
    g_engine.get_param.pCandidates      = (PZI8WCHAR)candidate_buffer;

    len = STRLEN((const S8 *) param_ptr->pre_string);

    g_engine.get_param.WordCharacterCount   = (ZI8UCHAR)len;

    memcpy(&current_word[0], param_ptr->pre_string, sizeof(UI_character_type) * len);

#if defined(__MMI_ZI_CHINESE_PHRASE_LEVEL_INPUTS__)
    current_word[len] = ' ';
    current_word[len+1] = 0;
#else
    current_word[len] = 0;
#endif

    g_engine.get_param.pCurrentWord  = (PZI8WCHAR)current_word;
    
    result_ptr->result_cnt = Zi8GetCandidates( &g_engine.get_param );

	//dele by yangdecai 2011-07-05
	//MMI_ASSERT(candidate_buffer[MMI_IME_EZI_MAX_TEMP_BUFFER_SIZE - 1] == MMI_IME_EZI_TEMP_BUFFER_FLAG);

    if (result_ptr->result_cnt) /* Get next page info and resume candidate list */
    {
		if (g_engine.get_param.GetOptions == ZI8_GETOPTION_WSTRINGS 
            || g_engine.get_param.GetOptions ==  ZI8_GETOPTION_GET_SPELLING
            || (g_engine.get_param.Context & ZI8_GETCONTEXT_WORD_CAND))
        {
            i = 0;
            j = 0;
			
            while( i < result_ptr->result_cnt)
            {
                result_ptr->result_buffer[j] = candidate_buffer[j];
                if (candidate_buffer[j] == 0 || candidate_buffer[j] == 0x20)
                {
                    result_ptr->result_buffer[j] = 0;
                    i++;
                }
                j++;
            }
            
            result_ptr->result_buffer[j] = 0;
        }
        else 
		{	
            mmi_ime_word_add_separator_to_candidate_buffer(result_ptr->result_buffer, candidate_buffer, result_ptr->result_cnt);
        }

        mmi_ime_ezi_check_page_info_and_resume_result(result_ptr);
    }
}
#endif /* #if defined(__MMI_ZI_SM_CHINESE__) || defined(__MMI_ZI_TR_CHINESE__) */

#if defined(__MMI_ALPHABETIC_WORD_PREDICTION__)
/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_get_associates_alphabetic
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
static void  mmi_ime_ezi_get_associates_alphabetic( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    UI_character_type candidate_buffer[MMI_IME_EZI_MAX_TEMP_BUFFER_SIZE];
    UI_character_type current_word[MMI_IMC_MAX_PRE_STRING_LEN + 2];
    S32 len, i, j;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	candidate_buffer[MMI_IME_EZI_MAX_TEMP_BUFFER_SIZE - 1] = MMI_IME_EZI_TEMP_BUFFER_FLAG;

    g_engine.get_param.Context = ZI8_GETCONTEXT_DEFAULT;

    g_engine.get_param.GetMode      = ZI8_GETMODE_DEFAULT;
    g_engine.get_param.GetOptions   = ZI8_GETOPTION_WSTRINGS;
    
    g_engine.get_param.ElementCount     = 0;

    g_engine.get_param.MaxCandidates    = (ZI8UCHAR)param_ptr->desired_cnt + 1; /* Plus 1 for getting next page info */
    g_engine.get_param.FirstCandidate   = (ZI8WCHAR)param_ptr->first_index;
    g_engine.get_param.pCandidates      = (PZI8WCHAR)candidate_buffer;

    len = STRLEN((const S8 *) param_ptr->pre_string);

    for (i = 0; i < len; i++)
    {
        current_word[i] = mmi_ime_ezi_char_to_lower(param_ptr->pre_string[i]);
    }

    current_word[len++] = 0x20;
    current_word[len] = 0;

	g_engine.get_param.WordCharacterCount   = (ZI8UCHAR)len;

    g_engine.get_param.pCurrentWord  = (PZI8WCHAR)current_word;

    g_engine.get_param.pElements[0] = 0;

    g_engine.get_param.count = 0;
    
    result_ptr->result_cnt = Zi8GetCandidates( &g_engine.get_param );

	//dele by yangdecai 2011-07-05
	//MMI_ASSERT(candidate_buffer[MMI_IME_EZI_MAX_TEMP_BUFFER_SIZE - 1] == MMI_IME_EZI_TEMP_BUFFER_FLAG);

    if (result_ptr->result_cnt) /* Get next page info and resume candidate list */
    {
		if (g_engine.get_param.GetOptions == ZI8_GETOPTION_WSTRINGS 
            || g_engine.get_param.GetOptions ==  ZI8_GETOPTION_GET_SPELLING
            || (g_engine.get_param.Context & ZI8_GETCONTEXT_WORD_CAND))
        {
            i = 0;
            j = 0;
			
            while( i < result_ptr->result_cnt)
            {
                result_ptr->result_buffer[j] = candidate_buffer[j];
                if (candidate_buffer[j] == 0 || candidate_buffer[j] == 0x20)
                {
                    result_ptr->result_buffer[j] = 0;
                    i++;
                }
                j++;
            }
            
            result_ptr->result_buffer[j] = 0;
        }
        else 
		{	
            mmi_ime_word_add_separator_to_candidate_buffer(result_ptr->result_buffer, candidate_buffer, result_ptr->result_cnt);
        }

        mmi_ime_ezi_check_page_info_and_resume_result(result_ptr);

        mmi_ime_ezi_change_candidates_case(result_ptr);
    }
}
#else
/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_get_associates_alphabetic
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
static void  mmi_ime_ezi_get_associates_alphabetic( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    return;
}
#endif  /* #if defined(__MMI_IME_USER_DATABASE__) */

/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_query_mode_info
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
static const mmi_ime_ezi_mode_info_struct * mmi_ime_ezi_query_mode_info( mmi_imm_input_mode_enum input_mode )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    U8  i;
#if defined(__MMI_SMART_FIRST_UPPERCASE__) || defined(__MMI_MULTITAP_FIRST_UPPERCASE__)
    const sIMEModeDetails * mode_details_ptr;
#endif
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    if ( input_mode >= IMM_INPUT_MODE_MAX_NUM )
        return NULL;
    
#if defined(__MMI_SMART_FIRST_UPPERCASE__) || defined(__MMI_MULTITAP_FIRST_UPPERCASE__)

    mode_details_ptr = mmi_imm_get_input_mode_array_item(input_mode);
#endif

#if defined(__MMI_SMART_FIRST_UPPERCASE__)
    if ((mode_details_ptr->Mode_Special_Attrib & IME_MODE_ATTR_TITLECASE)
        && (mode_details_ptr->Mode_Special_Attrib & IME_MODE_ATTR_SMART))
    {
        mode_details_ptr = mmi_imm_get_corresponding_smart_lower_case_mode(input_mode);

        if (mode_details_ptr)
        {
            input_mode = mode_details_ptr->imm_mode_id;
        }
        else
        {
            return NULL;
        }
    }
#endif /* defined(__MMI_SMART_FIRST_UPPERCASE__) */
    
#if defined(__MMI_MULTITAP_FIRST_UPPERCASE__)
	if ((mode_details_ptr->Mode_Special_Attrib & IME_MODE_ATTR_TITLECASE)
              && (mode_details_ptr->Mode_Special_Attrib & IME_MODE_ATTR_MULTITAP))
	{
        mode_details_ptr = mmi_imm_get_corresponding_multitap_mode(input_mode);
		
        if (mode_details_ptr)
        {
            input_mode = mode_details_ptr->imm_mode_id;
        }
        else
        {
            return NULL;
        }
	}	

#endif /* defined(__MMI_MULTITAP_FIRST_UPPERCASE__) */

    
    for ( i = 0; g_mode_infos[i].input_mode != IMM_INPUT_MODE_MAX_NUM; ++i )
    {
        if ( g_mode_infos[i].input_mode == input_mode )
        {
            return &g_mode_infos[i];
        }
    }
    
    return NULL;
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_change_candidates_case
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
static void  mmi_ime_ezi_change_candidates_case(mmi_ime_query_result_struct_p result_str)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 i = 0, j = 0;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    if (g_engine.case_type == MMI_IME_WORD_CAPITAL_STATE_FIRST_UPPER)
    {
        while (j < result_str->result_cnt)
        {
            result_str->result_buffer[i] = mmi_ime_ezi_char_to_upper(result_str->result_buffer[i]);

            i++;

            while (result_str->result_buffer[i])
            {
                result_str->result_buffer[i] = mmi_ime_ezi_char_to_lower(result_str->result_buffer[i]);
                i++;
            }
            i++;
            j++;
        }
    }
    else if (g_engine.case_type == MMI_IME_WORD_CAPITAL_STATE_UPPER)
    {
        while (j < result_str->result_cnt)
        {
            result_str->result_buffer[i] = mmi_ime_ezi_char_to_upper(result_str->result_buffer[i]);
            i++;
            
            while (result_str->result_buffer[i])
            {
                result_str->result_buffer[i] = mmi_ime_ezi_char_to_upper(result_str->result_buffer[i]);
                i++;
            }
            i++;
            j++;
        }
    }
    else if (g_engine.case_type == MMI_IME_WORD_CAPITAL_STATE_LOWER)
    {
        while (j < result_str->result_cnt)
        {
            result_str->result_buffer[i] = mmi_ime_ezi_char_to_lower(result_str->result_buffer[i]);
            i++;
            
            while (result_str->result_buffer[i])
            {
                result_str->result_buffer[i] = mmi_ime_ezi_char_to_lower(result_str->result_buffer[i]);
                i++;
            }
            i++;
            j++;
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_char_to_upper
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
static UI_character_type mmi_ime_ezi_char_to_upper(UI_character_type char_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
#if defined(__MMI_ZI_V7__)
    UI_character_type inChar = char_code;
#endif
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#if defined(__MMI_ZI_V7__)

    Zi8ChangeCharCase(ZI8TRUE, (PZI8WCHAR)&inChar, g_engine.curr_mode_info->language);

    return inChar;
    
#else
#if 0  //add by yangdecai
    return (UI_character_type)mmi_towupper((kal_wchar)char_code);
#else
{
	UI_character_type inChar = char_code;
	return inChar;
}
#endif
    
#endif
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_char_to_lower
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
static UI_character_type mmi_ime_ezi_char_to_lower(UI_character_type char_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
#if defined(__MMI_ZI_V7__)
    ZI8WCHAR inChar = char_code;
#endif

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#if defined(__MMI_ZI_V7__)
    Zi8ChangeCharCase(ZI8FALSE, (PZI8WCHAR)&inChar, g_engine.curr_mode_info->language);
    return inChar;
#else
    
#if 0  //add by yangdecai
    return (UI_character_type)mmi_towupper((kal_wchar)char_code);
#else
{
	UI_character_type inChar = char_code;
	return inChar;
}
#endif
    
#endif
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_zicode_to_code
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
static UI_character_type mmi_ime_ezi_zicode_to_code(ZI8WCHAR zicode)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    switch (zicode)
    {
    case ZI8_CODE_TONES_1:
        return 0x002D;
        
    case ZI8_CODE_TONES_2:
        return 0x02CA;
        
    case ZI8_CODE_TONES_3:
        return 0x02C7;
        
    case ZI8_CODE_TONES_4:
        return 0x02CB;
        
    case ZI8_CODE_TONES_5:
        return 0x02D9;
        
    case ZI8_CODE_WILDCARD:      
    case ZI8_CODE_MORE:          
    case ZI8_CODE_LEFT:          
    case ZI8_CODE_OVER:          
    case ZI8_CODE_DOT:           
    case ZI8_CODE_DOWN:          
    case ZI8_CODE_TURN:          
    case ZI8_CODE_DOWN_OVER:     
    case ZI8_CODE_OVER_DOWN:     
    case ZI8_CODE_CURVED_HOOK:    
    case ZI8_CODE_OVER_DOWN_OVER:
        return zicode;

    case ZI8_CODE_SEPARATOR:
        return 0x0027;

    default:
        if ( zicode >= ZI8_CODE_PINYIN_A && zicode<= ZI8_CODE_PINYIN_Z)
        {
            return zicode - ZI8_BASE_PINYIN + 0x0061;
        }
        else if (zicode >= ZI8_CODE_BPMF_B && zicode<= ZI8_CODE_BPMF_IU)
        {
            return zicode - ZI8_BASE_BPMF + 0x3105; /* BPMF code range: 0x3105-0x3129(g_bopomofo_table) */
        }
        else
        {
            break;
        }
    }
    return 0;
}

/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_check_page_info_and_resume_result
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
static void mmi_ime_ezi_check_page_info_and_resume_result(mmi_ime_query_result_struct_p result_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8          i;
    UI_string_type temp_str;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    if (result_ptr->result_cnt == g_engine.get_param.MaxCandidates)
    {
        result_ptr->result_cnt--;
        result_ptr->next_page = MMI_TRUE;
        temp_str = result_ptr->result_buffer;
        i = 0;

        /* Delete the redundant one candidate */
        while (i < result_ptr->result_cnt)/* Go to the last candidate */
        {
            if ((*temp_str) == 0)
            {
                i++;
            }
            temp_str++;
        }

        while((*temp_str) != 0)/* Delete the last candidate */
        {
            *temp_str = 0;
        }
    }
    else
    {
        result_ptr->next_page = MMI_FALSE;
    }
}


#if defined(__MMI_MIXED_LANG_SUPPORT__)
/*****************************************************************************
 * FUNCTION
 *  zi_map_ime_lang_to_zi_lang
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
static ZI8UCHAR mmi_ime_ezi_map_ime_lang_to_zi_lang(mmi_imm_support_writing_lang_enum lang_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 i = 0;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    while (g_ezi_mixed_lang_mapping[i][0] != IME_WRITING_LANG_NONE)
    {
        if (g_ezi_mixed_lang_mapping[i][0] == lang_id)
        {
            return g_ezi_mixed_lang_mapping[i][1];
        }
        i++;
    }

    return ZI8_LANG_GSMDEFAULT;
}
#endif /* defined(__MMI_MIXED_LANG_SUPPORT__) */


#if defined(__MMI_IME_USER_DATABASE__)
/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_add_new_word
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
mmi_ime_word_add_new_word_ret_value_enum mmi_ime_word_add_new_word(mmi_imm_input_mode_enum input_mode, UI_string_type str_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mmi_ime_ezi_mode_info_struct_p  mode_info_p;
    const sIMEModeDetails * mode_details_ptr;
    MMI_BOOL ret = MMI_FALSE;
    mmi_ime_word_add_new_word_ret_value_enum ret_val = MMI_IME_WORD_ADD_NEW_WORD_FAILED; 
    ZI8WCHAR str_buf[MMI_IMC_MAX_PRE_STRING_LEN + 1];
    S32 i, str_len;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    mode_info_p = (mmi_ime_ezi_mode_info_struct_p)mmi_ime_ezi_query_mode_info(input_mode);

    if (mode_info_p == NULL)
        return MMI_IME_WORD_ADD_NEW_WORD_FAILED;

	if (g_engine.curr_mode_info == NULL)
	{
        mode_details_ptr = mmi_imm_get_input_mode_array_item(input_mode);

        if (mode_details_ptr == NULL)
        {
        	return MMI_IME_WORD_ADD_NEW_WORD_FAILED;
        }
        
        mmi_ime_word_change_mode(mode_details_ptr, 0);

		if (g_engine.curr_mode_info == NULL)
		{
			return MMI_IME_WORD_ADD_NEW_WORD_FAILED;
		}
	}	

    if (g_ezi_pud_id_active > 0)
    {
        str_len = STRLEN((const S8 *)str_ptr);

        for (i = 0; i < str_len; i++)
        {
            str_buf[i] = mmi_ime_ezi_char_to_lower(str_ptr[i]);
        }
        
        str_buf[i] = 0;
        
        ret = (MMI_BOOL)Zi8AddUDWordW(g_ezi_pud_id_active, &str_buf[0], mode_info_p->language);

        if (!ret)
        {
            if (Zi8IsUDWordW(g_ezi_pud_id_active, &str_buf[0], mode_info_p->language))
            {
                ret_val = MMI_IME_WORD_ADD_NEW_WORD_DUPLICAT;
            }
            else
            {
                ret_val = MMI_IME_WORD_ADD_NEW_WORD_FAILED;
            }
        }
        else
        {
            ret_val = MMI_IME_WORD_ADD_NEW_WORD_SUCCESS;
        }
    }

    return ret_val;

}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_scan_and_add_new_words
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
void mmi_ime_word_scan_and_add_new_words(UI_string_type str_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    UI_character_type str_buf[MMI_IMC_MAX_INPUT_SYMBOL_BUFFSIZE + 1];
    S32 i = 0, j = 0;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    while (str_ptr[i] != 0)
    {
        if (str_ptr[i] == 0x20 || str_ptr[i] == 0x0D || str_ptr[i] == 0x0A)
        {
            if (j > 0 && j <= MMI_IMC_MAX_INPUT_SYMBOL_BUFFSIZE)
            {
                mmi_ime_ezi_add_word_to_all_langs(str_buf);
            }

            j = 0;
        }
        else 
        {
            if (j <= MMI_IMC_MAX_INPUT_SYMBOL_BUFFSIZE)
            {
                str_buf[j++] = str_ptr[i];
                str_buf[j] = 0;
            }
            else
            {
                j = MMI_IMC_MAX_INPUT_SYMBOL_BUFFSIZE + 1;
            }
        }

        i++;
    }

	if (j > 0 && j <= MMI_IMC_MAX_INPUT_SYMBOL_BUFFSIZE)
	{
		mmi_ime_ezi_add_word_to_all_langs(str_buf);
    }
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_update_word_frequency
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
MMI_BOOL mmi_ime_word_update_word_frequency(mmi_imm_input_mode_enum input_mode, UI_string_type str_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mmi_ime_ezi_mode_info_struct_p  mode_info_p;
    MMI_BOOL ret_val = MMI_FALSE; 
    ZI8WCHAR str_buf[MMI_IMC_MAX_PRE_STRING_LEN + 1];
    S32 i, str_len;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    mode_info_p = (mmi_ime_ezi_mode_info_struct_p)mmi_ime_ezi_query_mode_info(input_mode);

    if (mode_info_p == NULL)
        return MMI_FALSE;

#if defined(__MMI_ZI_SM_CHINESE__) || defined(__MMI_ZI_TR_CHINESE__)
    if (mode_info_p->language == ZI8_LANG_ZH)
    {
        if (g_ezi_uwd_zh_id_active > 0)
        {
            str_len = STRLEN((const S8 *)str_ptr);

			if (str_len > 0)
            {
                ret_val = (MMI_BOOL)Zi8AddUsedWordW_ZH(g_ezi_uwd_zh_id_active, str_ptr, (ZI8USHORT)str_len);
			}
        }
    } 
    else
#endif
    {
        if (g_ezi_uwd_id_active > 0)
        {
            str_len = STRLEN((const S8 *)str_ptr);

            for (i = 0; i < str_len; i++)
            {
                str_buf[i] = mmi_ime_ezi_char_to_lower(str_ptr[i]);
            }
        
            str_buf[i] = 0;
            
            ret_val = (MMI_BOOL)Zi8AddUsedWordW(g_ezi_uwd_id_active, &str_buf[0], mode_info_p->language);
        }
    }

    return ret_val;

}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_load_user_database
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
MMI_BOOL mmi_ime_word_load_user_database(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    FS_HANDLE file_handle;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    memset(&g_ezi_pud_buffer[0], 0, sizeof(U16) * MMI_IME_EZI_PD_SIZE / 2);   
    
    memset(&g_ezi_uwd_buffer[0], 0, sizeof(U16) * MMI_IME_EZI_UWD_SIZE / 2);   

#if defined(__MMI_ZI_SM_CHINESE__) || defined(__MMI_ZI_TR_CHINESE__)
    memset(&g_ezi_uwd_zh_buffer[0], 0, sizeof(U16) * MMI_IME_EZI_UWD_ZH_SIZE / 2); 
#endif

    file_handle = mmi_ime_ezi_open_file(MMI_IME_USER_DATABASE_PATH, MMI_IME_USER_DATABASE_FILE_PD, MMI_TRUE);
    
    if (file_handle == -1)
        return MMI_FALSE;

    mmi_ime_ezi_read_file(file_handle, &g_ezi_pud_buffer[0], sizeof(U16) * MMI_IME_EZI_PD_SIZE / 2);
    
    mmi_ime_ezi_close_file(file_handle);

    file_handle = mmi_ime_ezi_open_file(MMI_IME_USER_DATABASE_PATH, MMI_IME_USER_DATABASE_FILE_UWD, MMI_TRUE);
    
    if (file_handle == -1)
        return MMI_FALSE;

    mmi_ime_ezi_read_file(file_handle, &g_ezi_uwd_buffer[0], sizeof(U16) * MMI_IME_EZI_UWD_SIZE / 2);
    
    mmi_ime_ezi_close_file(file_handle);

#if defined(__MMI_ZI_SM_CHINESE__) || defined(__MMI_ZI_TR_CHINESE__)

    file_handle = mmi_ime_ezi_open_file(MMI_IME_USER_DATABASE_PATH, MMI_IME_USER_DATABASE_FILE_UWD_ZH, MMI_TRUE);
    
    if (file_handle == -1)
        return MMI_FALSE;

    mmi_ime_ezi_read_file(file_handle, &g_ezi_uwd_zh_buffer[0], sizeof(U16) * MMI_IME_EZI_UWD_ZH_SIZE / 2);
    
    mmi_ime_ezi_close_file(file_handle);
    
#endif /* defined(__MMI_ZI_SM_CHINESE__) || defined(__MMI_ZI_TR_CHINESE__) */

    return MMI_TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_save_user_database
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
MMI_BOOL mmi_ime_word_save_user_database(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    FS_HANDLE file_handle;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    file_handle = mmi_ime_ezi_open_file(MMI_IME_USER_DATABASE_PATH, MMI_IME_USER_DATABASE_FILE_PD, MMI_TRUE);
    
    if (file_handle == -1)
        return MMI_FALSE;

    mmi_ime_ezi_write_file(file_handle, &g_ezi_pud_buffer[0], sizeof(U16) * MMI_IME_EZI_PD_SIZE / 2);
    mmi_ime_ezi_close_file(file_handle);

    file_handle = mmi_ime_ezi_open_file(MMI_IME_USER_DATABASE_PATH, MMI_IME_USER_DATABASE_FILE_UWD, MMI_TRUE);
    
    if (file_handle == -1)
        return MMI_FALSE;

    mmi_ime_ezi_write_file(file_handle, &g_ezi_uwd_buffer[0], sizeof(U16) * MMI_IME_EZI_UWD_SIZE / 2);
    mmi_ime_ezi_close_file(file_handle);

#if defined(__MMI_ZI_SM_CHINESE__) || defined(__MMI_ZI_TR_CHINESE__)

    file_handle = mmi_ime_ezi_open_file(MMI_IME_USER_DATABASE_PATH, MMI_IME_USER_DATABASE_FILE_UWD_ZH, MMI_TRUE);

    if (file_handle == -1)
        return MMI_FALSE;

    mmi_ime_ezi_write_file(file_handle, &g_ezi_uwd_zh_buffer[0], sizeof(U16) * MMI_IME_EZI_UWD_ZH_SIZE / 2);
    mmi_ime_ezi_close_file(file_handle);
    
#endif /* defined(__MMI_ZI_SM_CHINESE__) || defined(__MMI_ZI_TR_CHINESE__) */

    return MMI_TRUE;

}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_init_user_database
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 ****************************************************************************/
static MMI_BOOL mmi_ime_ezi_init_user_database(void)
    {
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    if (!mmi_ime_word_load_user_database())
    {
        return MMI_FALSE;
    }

    if (!(g_ezi_pud_id_active = Zi8AttachPUD((PZI8UCHAR)&g_ezi_pud_buffer[0], MMI_IME_EZI_PD_SIZE)))
    {
        return MMI_FALSE;
    }

    if (!(g_ezi_uwd_id_active = Zi8AttachUWD((PZI8UCHAR)&g_ezi_uwd_buffer[0], MMI_IME_EZI_UWD_SIZE)))
    {
        return MMI_FALSE;
    }

#if defined(__MMI_ZI_SM_CHINESE__) || defined(__MMI_ZI_TR_CHINESE__)
    if (!(g_ezi_uwd_zh_id_active = Zi8AttachUWD_ZH((PZI8UCHAR)&g_ezi_uwd_zh_buffer[0], MMI_IME_EZI_UWD_ZH_SIZE)))
    {
        return MMI_FALSE;
    }
#endif

    return MMI_TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_add_word_to_all_langs
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 ****************************************************************************/
static void mmi_ime_ezi_add_word_to_all_langs(UI_string_type str)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 i, j;
    S32 str_len;
    MMI_BOOL ret;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    for ( i = 0; g_mode_infos[i].input_mode != IMM_INPUT_MODE_MAX_NUM; ++i )
    {
        if ( g_mode_infos[i].get_cand_func == mmi_ime_ezi_get_candidates_alphabetic )
        {
            if (g_ezi_pud_id_active > 0)
            {
                str_len = STRLEN((const S8 *)str);

                if (str_len > 0)
                {
                    for (j = 0; j < str_len; j++)
                    {
                        str[j] = mmi_ime_ezi_char_to_lower(str[j]);
                    }
                    
                    str[j] = 0;
                    
                    ret = Zi8AddUDWordW(g_ezi_pud_id_active, &str[0], g_mode_infos[i].language);
                }
            }
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_open_file
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 ****************************************************************************/
static FS_HANDLE mmi_ime_ezi_open_file(PU16 path_name_ptr, PU16 file_name_ptr, MMI_BOOL is_create_always)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    int ret_val;
    FS_HANDLE file_handle;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //dele by yangdecai 2011-07-05
	//MMI_ASSERT(NULL != path_name_ptr);
    //dele by yangdecai 2011-07-05
	//MMI_ASSERT(NULL != file_name_ptr);

    ret_val = FS_SetCurrentDir(path_name_ptr);
    if (ret_val < 0)
    {
        ret_val = FS_CreateDir(path_name_ptr);
        if (ret_val < 0 )
        {
            return -1;
        }
        ret_val = FS_SetCurrentDir(path_name_ptr);
        if (ret_val < 0)
        {
            return -1;
        }
    }

    FS_SetAttributes(file_name_ptr, FS_ATTR_ARCHIVE);

    /* try to open file */
    file_handle = FS_Open(file_name_ptr, FS_READ_WRITE | FS_ATTR_ARCHIVE);
    if (file_handle < 0)    /* failed to open existing file, or file not exists */
    {
        if (MMI_FALSE == is_create_always)
        {
            return -1;
        }

        /* try to create file */
        file_handle = FS_Open(file_name_ptr, FS_READ_WRITE | FS_CREATE_ALWAYS | FS_ATTR_ARCHIVE);
        if (file_handle < 0)
        {
            return -1;
        }
    }

    return file_handle;
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_close_file
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 ****************************************************************************/
static void mmi_ime_ezi_close_file(FS_HANDLE file_handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (file_handle < 0)
        return;

    FS_Close(file_handle);
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_write_file
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
static U32 mmi_ime_ezi_write_file(FS_HANDLE file_handle, PU16 data_ptr, U32 size_in_byte)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int ret_val;
    U32 writen;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (file_handle < 0)
        return 0;

    writen = 0;
    ret_val = FS_Write(file_handle, (void*)data_ptr, size_in_byte, &writen);
    if (ret_val < 0)
        return 0;

    return writen;
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_read_file
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
static U32 mmi_ime_ezi_read_file(FS_HANDLE file_handle, PU16 data_ptr, U32 size_in_byte)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int ret_val;
    U32 readed;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (file_handle < 0)
        return 0;

    readed = 0;
    ret_val = FS_Read(file_handle, (void*)data_ptr, size_in_byte, &readed);
    if (ret_val < 0)
        return 0;

    return readed;
}


#endif /* defined(__MMI_IME_USER_DATABASE__) */

#if defined(__MMI_ZI_MULTITAP__)
/*****************************************************************************
 * FUNCTION
 *  mmi_ime_multitap_vendor_init
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
MMI_BOOL mmi_ime_multitap_vendor_init( void )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    return mmi_ime_word_init();
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_multitap_vendor_deinit
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
void mmi_ime_multitap_vendor_deinit( void )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_multitap_vendor_get_string_func
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
get_multitap_string mmi_ime_multitap_vendor_get_string_func( mmi_imm_input_mode_enum input_mode )
        {
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    mmi_ime_ezi_mode_info_struct_p      mode_info_ptr = NULL;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    if ( MMI_FALSE == g_engine.is_inited )
        return NULL;
  
    mode_info_ptr = (mmi_ime_ezi_mode_info_struct_p)mmi_ime_ezi_query_mode_info(input_mode);

    if (mode_info_ptr)
    {
        return mmi_ime_multitap_vendor_get_string;
    }
    else
        {
            return NULL;
        }
    }


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_multitap_vendor_query_key_check_function
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
valid_key_check_func mmi_ime_multitap_vendor_query_key_check_function( mmi_imm_input_mode_enum input_mode )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mmi_ime_ezi_mode_info_struct_p      mode_info_ptr = NULL;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    if ( MMI_FALSE == g_engine.is_inited )
        return NULL;

    mode_info_ptr = (mmi_ime_ezi_mode_info_struct_p)mmi_ime_ezi_query_mode_info(input_mode);

    if (mode_info_ptr)
    {
        return mmi_ime_multitap_vendor_valid_key_check;
    }
    else
    {
       return NULL;
    }
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_multitap_vendor_change_mode
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
MMI_BOOL mmi_ime_multitap_vendor_change_mode( const sIMEModeDetails *mode_details_ptr, U32 reserved )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    return mmi_ime_word_change_mode( mode_details_ptr, reserved );
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_multitap_vendor_valid_key_check
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
static MMI_BOOL mmi_ime_multitap_vendor_valid_key_check( mmi_imc_symbol_key_enum sym)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    return mmi_ime_word_is_symbol_key(g_engine.curr_mode_info->input_mode, sym);
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_multitap_vendor_get_string
 * DESCRIPTION
 * PARAMETERS
 * RETURNS
 *****************************************************************************/
static UI_string_type mmi_ime_multitap_vendor_get_string(S32 index)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    UI_character_type  candidate_buf[MULTITAP_MAX_KEY_CHAR * 2];
    ZI8GETPARAM        temp_get_param;
    ZI8UCHAR           cnt;
    S32                cand_index = 0, multitap_len = 0, i = 0;
    UI_string_type     mt_string = NULL;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    g_engine.multitap_string_length = 0;
    if ( (index >=0  && index <= MMI_IMC_SYMB_KEY_POUND - MMI_IMC_SYMB_KEY_0) )
    {
        g_engine.get_param.Context      = ZI8_GETCONTEXT_DEFAULT | ZI8_GETCONTEXT_SINGLE_CHAR;

        g_engine.get_param.GetMode      = ZI8_GETMODE_DEFAULT;
        g_engine.get_param.GetOptions   = ZI8_GETOPTION_WSTRINGS;

        g_engine.get_param.pElements[0] = g_ezi_keys_latin[index];

        g_engine.get_param.MaxCandidates    = MULTITAP_MAX_KEY_CHAR + 1; /* Plus 1 for getting next page info */
        g_engine.get_param.FirstCandidate   = 0;
        g_engine.get_param.pCandidates      = (PZI8WCHAR)candidate_buf;

        g_engine.get_param.WordCharacterCount   = 0;

        g_engine.get_param.pCurrentWord  = NULL;

        /* This is a tricky way to resolve a issue of ZI */
        /* This segment must be removed after Zi has fixed the bug  */
    	memset(&temp_get_param, 0, sizeof(ZI8GETPARAM));

    	memcpy(&temp_get_param, &g_engine.get_param, sizeof(ZI8GETPARAM));

        temp_get_param.ElementCount = 0;

        Zi8GetCandidates(&temp_get_param);

        g_engine.get_param.ElementCount = 1;

        cnt = Zi8GetCandidates( &g_engine.get_param );

        if (cnt == 1 && g_engine.get_param.pCandidates[0] == MMI_IMC_HINT_CANDIDATE)
        {
            g_engine.get_param.FirstCandidate = 1;
            cnt = Zi8GetCandidates( &g_engine.get_param );
        }
        
        if ( cnt > 0 )
        {
            while (cand_index < cnt)
            {
                while (candidate_buf[i] == 0)
                {
                    i++;
                }

                if (mmi_imc_candidate_check(&candidate_buf[i], 1))
                {
                    if (g_engine.case_type == MMI_IME_WORD_CAPITAL_STATE_UPPER)
                    {
                        g_engine.data.mt_page_buff[multitap_len ++] = mmi_ime_ezi_char_to_upper(candidate_buf[i]);
                    }
                    else
                    {
                        g_engine.data.mt_page_buff[multitap_len ++] = mmi_ime_ezi_char_to_lower(candidate_buf[i]);
                    }
                }
                    
                i++;

                cand_index ++;
            }
            g_engine.multitap_string_length = multitap_len;
            g_engine.data.mt_page_buff[multitap_len] = 0;
        }

        if (g_engine.multitap_string_length > 0)
        {
            mt_string = g_engine.data.mt_page_buff;
        }

	    /* Walk around for Zi vendor bug */
	    if (g_engine.get_param.Language == ZI8_LANG_EN && mt_string != NULL)
		{
			if (mt_string[0] == 'i' || mt_string[0] == 'I')
			{
				i = 0;

                while (mt_string[i] != 'h' && mt_string[i] != 'H' && mt_string[i] != 0)
				{
                    mt_string[i] = mt_string[i + 1];
					i++;
				}

				if (mt_string[i] == 'h')
				{
                    mt_string[i] = 'i';
				}
				else if (mt_string[i] == 'H')
				{
                    mt_string[i] = 'I';
				}
			}
		}
    }
   
    return mt_string;
}


#endif /* defined(__MMI_ZI_MULTITAP__) */

#endif /* #ifdef  __MMI_ZI__ */
