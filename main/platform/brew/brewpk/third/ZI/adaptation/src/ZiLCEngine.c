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
 * Adaptation layer for eZi Smart IME
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
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

#include "OEMFeatures.h"

#ifdef __MMI_ZI__

#include "GlobalConstants.h"
#include "gui_data_types.h"
#include "IMERes.h"
#include "ImeGprot.h"

#include "Word_engine.h"
#include "Imc_symbol.h"
#include "Imc_config.h"
#include "ImmProt.h"

#include "zi8api.h"

/*****************************************************************************
 * Global variables
 *****************************************************************************/
extern const sIMELDBDetails *gIMELDBArray;

/*****************************************************************************
 * Typedefs
 *****************************************************************************/

typedef struct
{
    S32 input_mode;
    ZI8UCHAR language;
    ZI8UCHAR sub_language;
    U32 required_keys;
    U32 symbol_keys;
    mmi_ime_get_spellings_funcptr get_spell_func;
    mmi_ime_get_associates_funcptr get_assoc_func;
    mmi_ime_get_candidates_funcptr get_cand_func;
} mmi_ime_ezi_mode_info_struct, *mmi_ime_ezi_mode_info_struct_p;

typedef struct
{
    ZI8WCHAR elem_buff[MMI_IMC_MAX_INPUT_SYMBOL_BUFFSIZE];
} mmi_ime_ezi_data_struct, *mmi_ime_ezi_data_struct_p;

typedef struct
{
    mmi_ime_ezi_data_struct data;
    ZI8GETPARAM get_param;
    MMI_BOOL is_inited;
    mmi_ime_ezi_mode_info_struct_p curr_mode_info;
    S32 input_mode;
    U8 get_context;
    U8 case_type;
} mmi_ime_ezi_word_engine_struct, *mmi_ime_ezi_word_engine_struct_p;

/*****************************************************************************
 * Local functions
 *****************************************************************************/

/* Utility functions */

static void mmi_ime_ezi_fill_elem_buff(mmi_ime_query_param_struct_p param_ptr, U8 symb_range_begin, U8 symb_range_end, const ZI8WCHAR *symb_conv_table);

static void mmi_ime_ezi_get_candidates(mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr, ZI8UCHAR get_mode, ZI8UCHAR get_options);
static void mmi_ime_ezi_get_associates(mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr);

static const mmi_ime_ezi_mode_info_struct *mmi_ime_ezi_query_mode_info(S32 input_mode);

static void mmi_ime_ezi_change_candidates_case(mmi_ime_query_result_struct_p result_str);

static UI_character_type mmi_ime_ezi_char_to_upper(UI_character_type char_code);

static UI_character_type mmi_ime_ezi_char_to_lower(UI_character_type char_code);

static UI_character_type mmi_ime_ezi_zicode_to_code(ZI8WCHAR zicode);

static void mmi_ime_ezi_check_page_info_and_resume_result(mmi_ime_query_result_struct_p result_ptr);

/* Query functions */

static void mmi_ime_ezi_get_candidates_alphabetic(mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr);

#if defined(__MMI_ZI_SM_CHINESE__) || defined(__MMI_ZI_TR_CHINESE__)
static void mmi_ime_ezi_get_spellings_stroke(mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr);
static void mmi_ime_ezi_get_candidates_stroke(mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr);
#endif /* defined(__MMI_ZI_SM_CHINESE__) || defined(__MMI_ZI_TR_CHINESE__) */

/* Simplified Chinese functions */
#if defined(__MMI_ZI_SM_CHINESE__)

#if defined(__MMI_ZI_MULTITAP_PHONETIC_INPUT__)
static void mmi_ime_ezi_get_spellings_pinyin_multitap(mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr);
#endif /* defined(__MMI_ZI_MULTITAP_PHONETIC_INPUT__) */

#if defined(__MMI_ZI_SMART_PHONETIC_INPUT__)
static void mmi_ime_ezi_get_spellings_pinyin_smart(mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr);
#endif /* defined(__MMI_ZI_SMART_PHONETIC_INPUT__) */ 

static void mmi_ime_ezi_get_candidates_pinyin(mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr);

#endif /* defined(__MMI_ZI_SM_CHINESE__) */ 

/* Traditional Chinese functions */
#if defined(__MMI_ZI_TR_CHINESE__)

#if defined(__MMI_ZI_MULTITAP_PHONETIC_INPUT__)
static void mmi_ime_ezi_get_spellings_zhuyin_multitap(mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr);
#endif /* defined(__MMI_ZI_MULTITAP_PHONETIC_INPUT__) */

#if defined(__MMI_ZI_SMART_PHONETIC_INPUT__)
static void mmi_ime_ezi_get_spellings_zhuyin_smart(mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr);
#endif /* defined(__MMI_ZI_SMART_PHONETIC_INPUT__) */

static void mmi_ime_ezi_get_candidates_zhuyin(mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr);

#endif /* defined(__MMI_ZI_TR_CHINESE__) */

#if defined(__MMI_MIXED_LANG_SUPPORT__)
static ZI8UCHAR mmi_ime_ezi_map_ime_lang_to_zi_lang(mmi_imm_support_writing_lang_enum lang_id);
#endif /* defined(__MMI_MIXED_LANG_SUPPORT__) */ 

/*****************************************************************************
 * Local variables
 *****************************************************************************/

static const ZI8WCHAR g_ezi_onekeys[MMI_IMC_SYMB_KEY_MAX - MMI_IMC_SYMB_KEY_BASE] = 
{
    ZI8_ONEKEY_KEY0,    /* MMI_IMC_SYMB_KEY_0 */
    ZI8_ONEKEY_KEY1,    /* MMI_IMC_SYMB_KEY_1 */
    ZI8_ONEKEY_KEY2,    /* MMI_IMC_SYMB_KEY_2 */
    ZI8_ONEKEY_KEY3,    /* MMI_IMC_SYMB_KEY_3 */
    ZI8_ONEKEY_KEY4,    /* MMI_IMC_SYMB_KEY_4 */
    ZI8_ONEKEY_KEY5,    /* MMI_IMC_SYMB_KEY_5 */
    ZI8_ONEKEY_KEY6,    /* MMI_IMC_SYMB_KEY_6 */
    ZI8_ONEKEY_KEY7,    /* MMI_IMC_SYMB_KEY_7 */
    ZI8_ONEKEY_KEY8,    /* MMI_IMC_SYMB_KEY_8 */
    ZI8_ONEKEY_KEY9,    /* MMI_IMC_SYMB_KEY_9 */
    0,                  /* MMI_IMC_SYMB_KEY_STAR, */
    0,                  /* MMI_IMC_SYMB_KEY_POUND, */
};

static const ZI8WCHAR g_ezi_keys_latin[MMI_IMC_SYMB_KEY_MAX - MMI_IMC_SYMB_KEY_BASE] = 
{
    ZI8_CODE_LATIN_KEY0,    /* MMI_IMC_SYMB_KEY_0 */
    ZI8_CODE_LATIN_PUNCT,   /* MMI_IMC_SYMB_KEY_1 */
    ZI8_CODE_LATIN_KEY2,    /* MMI_IMC_SYMB_KEY_2 */
    ZI8_CODE_LATIN_KEY3,    /* MMI_IMC_SYMB_KEY_3 */
    ZI8_CODE_LATIN_KEY4,    /* MMI_IMC_SYMB_KEY_4 */
    ZI8_CODE_LATIN_KEY5,    /* MMI_IMC_SYMB_KEY_5 */
    ZI8_CODE_LATIN_KEY6,    /* MMI_IMC_SYMB_KEY_6 */
    ZI8_CODE_LATIN_KEY7,    /* MMI_IMC_SYMB_KEY_7 */
    ZI8_CODE_LATIN_KEY8,    /* MMI_IMC_SYMB_KEY_8 */
    ZI8_CODE_LATIN_KEY9,    /* MMI_IMC_SYMB_KEY_9 */
    0,                      /* MMI_IMC_SYMB_KEY_STAR, */
    0,                      /* MMI_IMC_SYMB_KEY_POUND, */
};

static const ZI8WCHAR g_ezi_codes_tone[MMI_IMC_SYMB_TONE_MAX - MMI_IMC_SYMB_TONE_BASE] = 
{
    0,                  /* MMI_IMC_SYMB_TONE_NONE */
    ZI8_CODE_TONES_1,   /* MMI_IMC_SYMB_TONE_1 */
    ZI8_CODE_TONES_2,   /* MMI_IMC_SYMB_TONE_2 */
    ZI8_CODE_TONES_3,   /* MMI_IMC_SYMB_TONE_3 */
    ZI8_CODE_TONES_4,   /* MMI_IMC_SYMB_TONE_4 */
    ZI8_CODE_TONES_5,   /* MMI_IMC_SYMB_TONE_5 */
};

#if defined(__MMI_ZI_SM_CHINESE__) || defined(__MMI_ZI_TR_CHINESE__)

static const mmi_imc_symb_stroke_enum g_ezi_keys_stroke[MMI_IMC_SYMB_KEY_MAX - MMI_IMC_SYMB_KEY_BASE] = 
{
    0,                                  /* MMI_IMC_SYMB_KEY_0 */
    MMI_IMC_SYMB_STROKE_OVER,           /* MMI_IMC_SYMB_KEY_1 */
    MMI_IMC_SYMB_STROKE_DOWN,           /* MMI_IMC_SYMB_KEY_2 */
    MMI_IMC_SYMB_STROKE_LEFT,           /* MMI_IMC_SYMB_KEY_3 */
    MMI_IMC_SYMB_STROKE_DOT,            /* MMI_IMC_SYMB_KEY_4 */
    MMI_IMC_SYMB_STROKE_OVER_DOWN,      /* MMI_IMC_SYMB_KEY_5 */
    MMI_IMC_SYMB_STROKE_CURVED_HOOK,    /* MMI_IMC_SYMB_KEY_6 */
    MMI_IMC_SYMB_STROKE_DOWN_OVER,      /* MMI_IMC_SYMB_KEY_7 */
    MMI_IMC_SYMB_STROKE_WILDCARD,       /* MMI_IMC_SYMB_KEY_8 */
    MMI_IMC_SYMB_STROKE_OVER_DOWN_OVER, /* MMI_IMC_SYMB_KEY_9 */
    0,                                  /* MMI_IMC_SYMB_KEY_STAR */
    0                                   /* MMI_IMC_SYMB_KEY_POUND */
};

static const ZI8WCHAR g_ezi_codes_stroke[MMI_IMC_SYMB_STROKE_MAX - MMI_IMC_SYMB_STROKE_BASE] = 
{
    ZI8_CODE_WILDCARD,          /* MMI_IMC_SYMB_STROKE_WILDCARD, */
    ZI8_CODE_DOWN,              /* MMI_IMC_SYMB_STROKE_DOWN, */
    ZI8_CODE_DOT,               /* MMI_IMC_SYMB_STROKE_DOT, */
    ZI8_CODE_CURVED_HOOK,       /* MMI_IMC_SYMB_STROKE_CURVED_HOOK, */
    ZI8_CODE_OVER,              /* MMI_IMC_SYMB_STROKE_OVER, */
    ZI8_CODE_OVER_DOWN,         /* MMI_IMC_SYMB_STROKE_OVER_DOWN, */
    ZI8_CODE_DOWN_OVER,         /* MMI_IMC_SYMB_STROKE_DOWN_OVER, */
    ZI8_CODE_LEFT,              /* MMI_IMC_SYMB_STROKE_LEFT, */
    ZI8_CODE_OVER_DOWN_OVER,    /* MMI_IMC_SYMB_STROKE_OVER_DOWN_OVER, */
    ZI8_CODE_MORE,              /* MMI_IMC_SYMB_STROKE_MORE, */
    ZI8_CODE_TURN,              /* MMI_IMC_SYMB_STROKE_TURN, */
    0,                          /* MMI_IMC_SYMB_STROKE_DOT_OR_LEFT, */
};
#endif /* defined(__MMI_ZI_SM_CHINESE__) || defined(__MMI_ZI_TR_CHINESE__) */ /* #if defined(__MMI_ZI_SM_CHINESE__) || defined(__MMI_ZI_TR_CHINESE__) */

#if defined(__MMI_ZI_SM_CHINESE__)
#if defined(__MMI_ZI_MULTITAP_PHONETIC_INPUT__) || defined(__MMI_ZI_SMART_PHONETIC_INPUT__)
static const ZI8WCHAR g_ezi_codes_pinyin[MMI_IMC_SYMB_PY_MAX - MMI_IMC_SYMB_PY_BASE] = 
{
    ZI8_CODE_PINYIN_A,  /* MMI_IMC_SYMB_PY_A */
    ZI8_CODE_PINYIN_B,  /* MMI_IMC_SYMB_PY_B */
    ZI8_CODE_PINYIN_C,  /* MMI_IMC_SYMB_PY_C */
    ZI8_CODE_PINYIN_D,  /* MMI_IMC_SYMB_PY_D */
    ZI8_CODE_PINYIN_E,  /* MMI_IMC_SYMB_PY_E */
    ZI8_CODE_PINYIN_F,  /* MMI_IMC_SYMB_PY_F */
    ZI8_CODE_PINYIN_G,  /* MMI_IMC_SYMB_PY_G */
    ZI8_CODE_PINYIN_H,  /* MMI_IMC_SYMB_PY_H */
    ZI8_CODE_PINYIN_I,  /* MMI_IMC_SYMB_PY_I */
    ZI8_CODE_PINYIN_J,  /* MMI_IMC_SYMB_PY_J */
    ZI8_CODE_PINYIN_K,  /* MMI_IMC_SYMB_PY_K */
    ZI8_CODE_PINYIN_L,  /* MMI_IMC_SYMB_PY_L */
    ZI8_CODE_PINYIN_M,  /* MMI_IMC_SYMB_PY_M */
    ZI8_CODE_PINYIN_N,  /* MMI_IMC_SYMB_PY_N */
    ZI8_CODE_PINYIN_O,  /* MMI_IMC_SYMB_PY_O */
    ZI8_CODE_PINYIN_P,  /* MMI_IMC_SYMB_PY_P */
    ZI8_CODE_PINYIN_Q,  /* MMI_IMC_SYMB_PY_Q */
    ZI8_CODE_PINYIN_R,  /* MMI_IMC_SYMB_PY_R */
    ZI8_CODE_PINYIN_S,  /* MMI_IMC_SYMB_PY_S */
    ZI8_CODE_PINYIN_T,  /* MMI_IMC_SYMB_PY_T */
    ZI8_CODE_PINYIN_U,  /* MMI_IMC_SYMB_PY_U */
    ZI8_CODE_PINYIN_V,  /* MMI_IMC_SYMB_PY_V */
    ZI8_CODE_PINYIN_W,  /* MMI_IMC_SYMB_PY_W */
    ZI8_CODE_PINYIN_X,  /* MMI_IMC_SYMB_PY_X */
    ZI8_CODE_PINYIN_Y,  /* MMI_IMC_SYMB_PY_Y */
    ZI8_CODE_PINYIN_Z,  /* MMI_IMC_SYMB_PY_Z */
};
#endif /* defined(__MMI_ZI_MULTITAP_PHONETIC_INPUT__) || defined(__MMI_ZI_SMART_PHONETIC_INPUT__) */ /* #if defined(__MMI_ZI_MULTITAP_PHONETIC_INPUT__) || defined(__MMI_ZI_SMART_PHONETIC_INPUT__) */
#endif /* defined(__MMI_ZI_SM_CHINESE__) */

#if defined(__MMI_ZI_TR_CHINESE__)
#if defined(__MMI_ZI_MULTITAP_PHONETIC_INPUT__) || defined(__MMI_ZI_SMART_PHONETIC_INPUT__)
static const ZI8WCHAR g_ezi_codes_bopomofo[MMI_IMC_SYMB_ZY_MAX - MMI_IMC_SYMB_ZY_BASE] = 
{
    ZI8_CODE_BPMF_B,    /* MMI_IMC_SYMB_ZY_B, */
    ZI8_CODE_BPMF_P,    /* MMI_IMC_SYMB_ZY_P, */
    ZI8_CODE_BPMF_M,    /* MMI_IMC_SYMB_ZY_M, */
    ZI8_CODE_BPMF_F,    /* MMI_IMC_SYMB_ZY_F, */

    ZI8_CODE_BPMF_D,    /* MMI_IMC_SYMB_ZY_D, */
    ZI8_CODE_BPMF_T,    /* MMI_IMC_SYMB_ZY_T, */
    ZI8_CODE_BPMF_N,    /* MMI_IMC_SYMB_ZY_N, */
    ZI8_CODE_BPMF_L,    /* MMI_IMC_SYMB_ZY_L, */

    ZI8_CODE_BPMF_G,    /* MMI_IMC_SYMB_ZY_G, */
    ZI8_CODE_BPMF_K,    /* MMI_IMC_SYMB_ZY_K, */
    ZI8_CODE_BPMF_H,    /* MMI_IMC_SYMB_ZY_H, */

    ZI8_CODE_BPMF_J,    /* MMI_IMC_SYMB_ZY_J, */
    ZI8_CODE_BPMF_Q,    /* MMI_IMC_SYMB_ZY_Q, */
    ZI8_CODE_BPMF_X,    /* MMI_IMC_SYMB_ZY_X, */

    ZI8_CODE_BPMF_ZH,   /* MMI_IMC_SYMB_ZY_ZH, */
    ZI8_CODE_BPMF_CH,   /* MMI_IMC_SYMB_ZY_CH, */
    ZI8_CODE_BPMF_SH,   /* MMI_IMC_SYMB_ZY_SH, */
    ZI8_CODE_BPMF_R,    /* MMI_IMC_SYMB_ZY_R, */

    ZI8_CODE_BPMF_Z,    /* MMI_IMC_SYMB_ZY_Z, */
    ZI8_CODE_BPMF_C,    /* MMI_IMC_SYMB_ZY_C, */
    ZI8_CODE_BPMF_S,    /* MMI_IMC_SYMB_ZY_S, */

    ZI8_CODE_BPMF_A,    /* MMI_IMC_SYMB_ZY_A, */
    ZI8_CODE_BPMF_O,    /* MMI_IMC_SYMB_ZY_O, */
    ZI8_CODE_BPMF_E,    /* MMI_IMC_SYMB_ZY_E, */
    ZI8_CODE_BPMF_EH,   /* MMI_IMC_SYMB_ZY_EH, */

    ZI8_CODE_BPMF_AI,   /* MMI_IMC_SYMB_ZY_AI, */
    ZI8_CODE_BPMF_EI,   /* MMI_IMC_SYMB_ZY_EI, */
    ZI8_CODE_BPMF_AU,   /* MMI_IMC_SYMB_ZY_AU, */
    ZI8_CODE_BPMF_OU,   /* MMI_IMC_SYMB_ZY_OU, */

    ZI8_CODE_BPMF_AN,   /* MMI_IMC_SYMB_ZY_AN, */
    ZI8_CODE_BPMF_EN,   /* MMI_IMC_SYMB_ZY_EN, */
    ZI8_CODE_BPMF_ANG,  /* MMI_IMC_SYMB_ZY_ANG, */
    ZI8_CODE_BPMF_ENG,  /* MMI_IMC_SYMB_ZY_ENG, */
    ZI8_CODE_BPMF_ER,   /* MMI_IMC_SYMB_ZY_ER, */

    ZI8_CODE_BPMF_I,    /* MMI_IMC_SYMB_ZY_I, */
    ZI8_CODE_BPMF_U,    /* MMI_IMC_SYMB_ZY_U, */
    ZI8_CODE_BPMF_IU,   /* MMI_IMC_SYMB_ZY_IU */
};

static const ZI8WCHAR g_bopomofo_table[MMI_IMC_SYMB_ZY_MAX - MMI_IMC_SYMB_ZY_BASE] = 
{
    0x3105, /* MMI_IMC_SYMB_ZY_B */
    0x3106, /* MMI_IMC_SYMB_ZY_P */
    0x3107, /* MMI_IMC_SYMB_ZY_M */
    0x3108, /* MMI_IMC_SYMB_ZY_F */
    0x3109, /* MMI_IMC_SYMB_ZY_D */
    0x310A, /* MMI_IMC_SYMB_ZY_T */
    0x310B, /* MMI_IMC_SYMB_ZY_N */
    0x310C, /* MMI_IMC_SYMB_ZY_L */
    0x310D, /* MMI_IMC_SYMB_ZY_G */
    0x310E, /* MMI_IMC_SYMB_ZY_K */
    0x310F, /* MMI_IMC_SYMB_ZY_H */
    0x3110, /* MMI_IMC_SYMB_ZY_J */
    0x3111, /* MMI_IMC_SYMB_ZY_Q */
    0x3112, /* MMI_IMC_SYMB_ZY_X */
    0x3113, /* MMI_IMC_SYMB_ZY_ZH */
    0x3114, /* MMI_IMC_SYMB_ZY_CH */
    0x3115, /* MMI_IMC_SYMB_ZY_SH */
    0x3116, /* MMI_IMC_SYMB_ZY_R */
    0x3117, /* MMI_IMC_SYMB_ZY_Z */
    0x3118, /* MMI_IMC_SYMB_ZY_C */
    0x3119, /* MMI_IMC_SYMB_ZY_S */
    0x311A, /* MMI_IMC_SYMB_ZY_A */
    0x311B, /* MMI_IMC_SYMB_ZY_O */
    0x311C, /* MMI_IMC_SYMB_ZY_E */
    0x311D, /* MMI_IMC_SYMB_ZY_EH */
    0x311E, /* MMI_IMC_SYMB_ZY_AI */
    0x311F, /* MMI_IMC_SYMB_ZY_EI */
    0x3120, /* MMI_IMC_SYMB_ZY_AU */
    0x3121, /* MMI_IMC_SYMB_ZY_OU */
    0x3122, /* MMI_IMC_SYMB_ZY_AN */
    0x3123, /* MMI_IMC_SYMB_ZY_EN */
    0x3124, /* MMI_IMC_SYMB_ZY_ANG */
    0x3125, /* MMI_IMC_SYMB_ZY_ENG */
    0x3126, /* MMI_IMC_SYMB_ZY_ER */
    0x3127, /* MMI_IMC_SYMB_ZY_I */
    0x3128, /* MMI_IMC_SYMB_ZY_U */
    0x3129  /* MMI_IMC_SYMB_ZY_IU */
};
#endif /* defined(__MMI_ZI_MULTITAP_PHONETIC_INPUT__) || defined(__MMI_ZI_SMART_PHONETIC_INPUT__) */ /* #if defined(__MMI_ZI_MULTITAP_PHONETIC_INPUT__) || defined(__MMI_ZI_SMART_PHONETIC_INPUT__) */
#endif /* defined(__MMI_ZI_TR_CHINESE__) */

/* All input mode informations */
static const mmi_ime_ezi_mode_info_struct g_mode_infos[] = 
{
#if defined(__MMI_ZI_TR_CHINESE__) && defined(__MMI_ZI_MULTITAP_PHONETIC_INPUT__)
    {
     INPUT_MODE_TR_MULTITAP_BOPOMO,
     ZI8_LANG_ZH,
     ZI8_SUBLANG_ZH_TW,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS,
     mmi_ime_ezi_get_spellings_zhuyin_multitap,
     mmi_ime_ezi_get_associates,
     mmi_ime_ezi_get_candidates_zhuyin},
#endif /* defined(__MMI_ZI_TR_CHINESE__) && defined(__MMI_ZI_MULTITAP_PHONETIC_INPUT__) */

 #if defined(__MMI_ZI_SM_CHINESE__) && defined(__MMI_ZI_MULTITAP_PHONETIC_INPUT__)
    {
     INPUT_MODE_MULTITAP_PINYIN,
     ZI8_LANG_ZH,
     ZI8_SUBLANG_ZH,
     MMI_IME_REQUIRED_ALL_KEYS & (~MMI_IME_REQUIRED_KEY_1),
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_1) & (~MMI_IME_REQUIRED_KEY_0),
     mmi_ime_ezi_get_spellings_pinyin_multitap,
     mmi_ime_ezi_get_associates,
     mmi_ime_ezi_get_candidates_pinyin},
 #endif /* defined(__MMI_ZI_SM_CHINESE__) && defined(__MMI_ZI_MULTITAP_PHONETIC_INPUT__) */

 #if defined(__MMI_ZI_TR_CHINESE__) && defined(__MMI_ZI_SMART_PHONETIC_INPUT__)
    {
     INPUT_MODE_TR_BOPOMO,
     ZI8_LANG_ZH,
     ZI8_SUBLANG_ZH_TW,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS,
     mmi_ime_ezi_get_spellings_zhuyin_smart,
     mmi_ime_ezi_get_associates,
     mmi_ime_ezi_get_candidates_zhuyin},
 #endif /* defined(__MMI_ZI_TR_CHINESE__) && defined(__MMI_ZI_SMART_PHONETIC_INPUT__) */

 #if defined(__MMI_ZI_SM_CHINESE__) && defined(__MMI_ZI_SMART_PHONETIC_INPUT__)
    {
     INPUT_MODE_SM_PINYIN,
     ZI8_LANG_ZH,
     ZI8_SUBLANG_ZH,
     MMI_IME_REQUIRED_ALL_KEYS & (~MMI_IME_REQUIRED_KEY_1),
     MMI_IME_REQUIRED_ALL_NUM_KEYS &(~MMI_IME_REQUIRED_KEY_0),
     mmi_ime_ezi_get_spellings_pinyin_smart,
     mmi_ime_ezi_get_associates,
     mmi_ime_ezi_get_candidates_pinyin},
 #endif /* defined(__MMI_ZI_SM_CHINESE__) && defined(__MMI_ZI_SMART_PHONETIC_INPUT__) */

 #if defined(__MMI_ZI_TR_CHINESE__)
    {
     INPUT_MODE_TR_STROKE,
     ZI8_LANG_ZH,
     ZI8_SUBLANG_ZH_TW,
     MMI_IME_REQUIRED_ALL_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     mmi_ime_ezi_get_spellings_stroke,
     mmi_ime_ezi_get_associates,
     mmi_ime_ezi_get_candidates_stroke},
 #endif /* defined(__MMI_ZI_TR_CHINESE__) */

 #if defined(__MMI_ZI_SM_CHINESE__)
    {
     INPUT_MODE_SM_STROKE,
     ZI8_LANG_ZH,
     ZI8_SUBLANG_ZH,
     MMI_IME_REQUIRED_ALL_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     mmi_ime_ezi_get_spellings_stroke,
     mmi_ime_ezi_get_associates,
     mmi_ime_ezi_get_candidates_stroke},
 #endif /* defined(__MMI_ZI_SM_CHINESE__) */

 #if defined(__MMI_ZI_PRC_ENGLISH__)
    {
     INPUT_MODE_MULTITAP_UPPERCASE_ABC,
     ZI8_LANG_EN,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS,
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
    {
     INPUT_MODE_MULTITAP_LOWERCASE_ABC,
     ZI8_LANG_EN,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS,
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
    {
     INPUT_MODE_SMART_UPPERCASE_ABC,
     ZI8_LANG_EN,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
    {
     INPUT_MODE_SMART_LOWERCASE_ABC,
     ZI8_LANG_EN,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_PRC_ENGLISH__) */ 

 #if defined(__MMI_ZI_ARABIC__)
    {
     INPUT_MODE_SMART_ARABIC,
     ZI8_LANG_AR,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_ARABIC__) */

 #if defined(__MMI_ZI_BENGALI__)
    {
     INPUT_MODE_SMART_BENGALI,
     ZI8_LANG_BN,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_BENGALI__) */

 #if defined(__MMI_ZI_BZ_PORTUGUESE__) || defined(__MMI_ZI_SA_PORTUGUESE__)
    {
     INPUT_MODE_SMART_UPPERCASE_PORTUGUESE,
     ZI8_LANG_PT,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},

    {
     INPUT_MODE_SMART_LOWERCASE_PORTUGUESE,
     ZI8_LANG_PT,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_BZ_PORTUGUESE__) || defined(__MMI_ZI_SA_PORTUGUESE__) */

 #if defined(__MMI_ZI_CROATIAN__)
    {
     INPUT_MODE_SMART_UPPERCASE_CROATIAN,
     ZI8_LANG_HR,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},

    {
     INPUT_MODE_SMART_LOWERCASE_CROATIAN,
     ZI8_LANG_HR,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_CROATIAN__) */

 #if defined(__MMI_ZI_CZECH__)
    {
     INPUT_MODE_SMART_UPPERCASE_CZECH,
     ZI8_LANG_CS,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},

    {
     INPUT_MODE_SMART_LOWERCASE_CZECH,
     ZI8_LANG_CS,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_CZECH__) */

 #if defined(__MMI_ZI_DANISH__)
    {
     INPUT_MODE_SMART_UPPERCASE_DANISH,
     ZI8_LANG_DA,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},

    {
     INPUT_MODE_SMART_LOWERCASE_DANISH,
     ZI8_LANG_DA,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_DANISH__) */

 #if defined(__MMI_ZI_DUTCH__)
    {
     INPUT_MODE_SMART_UPPERCASE_DUTCH,
     ZI8_LANG_NL,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},

    {
     INPUT_MODE_SMART_LOWERCASE_DUTCH,
     ZI8_LANG_NL,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_DUTCH__) */

 #if defined(__MMI_ZI_EU_FRENCH__)
    {
     INPUT_MODE_SMART_UPPERCASE_FRENCH,
     ZI8_LANG_FR,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},

    {
     INPUT_MODE_SMART_LOWERCASE_FRENCH,
     ZI8_LANG_FR,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_EU_FRENCH__) */ 

 #if defined(__MMI_ZI_FINNISH__)
    {
     INPUT_MODE_SMART_UPPERCASE_FINNISH,
     ZI8_LANG_FI,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},

    {
     INPUT_MODE_SMART_LOWERCASE_FINNISH,
     ZI8_LANG_FI,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_FINNISH__) */ 

 #if defined(__MMI_ZI_GERMAN__)
    {
     INPUT_MODE_SMART_UPPERCASE_GERMAN,
     ZI8_LANG_DE,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},

    {
     INPUT_MODE_SMART_LOWERCASE_GERMAN,
     ZI8_LANG_DE,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_GERMAN__) */ 

 #if defined(__MMI_ZI_GREEK__)
    {
     INPUT_MODE_SMART_UPPERCASE_GREEK,
     ZI8_LANG_EL,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},

    {
     INPUT_MODE_SMART_LOWERCASE_GREEK,
     ZI8_LANG_EL,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_GREEK__) */ 

 #if defined(__MMI_ZI_HINDI__)
    {
     INPUT_MODE_SMART_HINDI,
     ZI8_LANG_HI,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS,
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_HINDI__) */ /* #if defined(__MMI_ZI_HINDI__)  */

 #if defined(__MMI_ZI_INDONESIAN__)
    {
     INPUT_MODE_SMART_UPPERCASE_INDONESIAN,
     ZI8_LANG_IN,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},

    {
     INPUT_MODE_SMART_LOWERCASE_INDONESIAN,
     ZI8_LANG_IN,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_INDONESIAN__) */ 

 #if defined(__MMI_ZI_ITALIAN__)
    {
     INPUT_MODE_SMART_UPPERCASE_ITALIAN,
     ZI8_LANG_IT,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},

    {
     INPUT_MODE_SMART_LOWERCASE_ITALIAN,
     ZI8_LANG_IT,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_ITALIAN__) */ 

 #if defined(__MMI_ZI_MALAY__)
    {
     INPUT_MODE_SMART_UPPERCASE_MALAY,
     ZI8_LANG_MS,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},

    {
     INPUT_MODE_SMART_LOWERCASE_MALAY,
     ZI8_LANG_MS,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_MALAY__) */ 

 #if defined(__MMI_ZI_MARATHI__)
    {
     INPUT_MODE_SMART_MARATHI,
     ZI8_LANG_MR,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_MARATHI__) */ 

 #if defined(__MMI_ZI_PERSIAN__)
    {
     INPUT_MODE_SMART_PERSIAN,
     ZI8_LANG_FA,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_PERSIAN__) */ 

 #if defined(__MMI_ZI_POLISH__)
    {
     INPUT_MODE_SMART_UPPERCASE_POLISH,
     ZI8_LANG_PL,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},

    {
     INPUT_MODE_SMART_LOWERCASE_POLISH,
     ZI8_LANG_PL,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_POLISH__) */ 

 #if defined(__MMI_ZI_PUNJABI__)
    {
     INPUT_MODE_SMART_PUNJABI,
     ZI8_LANG_PA,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_PUNJABI__) */ 

 #if defined(__MMI_ZI_KANNADA__)
    {
     INPUT_MODE_SMART_KANNADA,
     ZI8_LANG_KN,

     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS,
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_KANNADA__) */ 

 #if defined(__MMI_ZI_MALAYALAM__)
    {
     INPUT_MODE_SMART_MALAYALAM,
     ZI8_LANG_ML,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_MALAYALAM__) */ 


 #if defined(__MMI_ZI_GUJARATI__)
    {
     INPUT_MODE_SMART_GUJARATI,
     ZI8_LANG_GU,

     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS,
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_GUJARATI__) */ 

 #if defined(__MMI_ZI_ROMANIAN__)
    {
     INPUT_MODE_SMART_UPPERCASE_ROMANIAN,
     ZI8_LANG_RO,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},

    {
     INPUT_MODE_SMART_LOWERCASE_ROMANIAN,
     ZI8_LANG_RO,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_ROMANIAN__) */

 #if defined(__MMI_ZI_RUSSIAN__)
    {
     INPUT_MODE_SMART_UPPERCASE_RUSSIAN,
     ZI8_LANG_RU,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},

    {
     INPUT_MODE_SMART_LOWERCASE_RUSSIAN,
     ZI8_LANG_RU,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_RUSSIAN__) */ 

 #if defined(__MMI_ZI_SA_SPANISH__)
    {
     INPUT_MODE_SMART_UPPERCASE_SPANISH,
     ZI8_LANG_ES,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},

    {
     INPUT_MODE_SMART_LOWERCASE_SPANISH,
     ZI8_LANG_ES,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_SA_SPANISH__) */ 

 #if defined(__MMI_ZI_TAMIL__)
    {
     INPUT_MODE_SMART_TAMIL,
     ZI8_LANG_TA,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_TAMIL__) */ 

 #if defined(__MMI_ZI_TELUGU__)
    {
     INPUT_MODE_SMART_TELUGU,
     ZI8_LANG_TE,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_TELUGU__) */ /* #if defined(__MMI_ZI_TELUGU__)  */

 #if defined(__MMI_ZI_THAI__)
    {
     INPUT_MODE_SMART_THAI,
     ZI8_LANG_TH,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_THAI__) */ 

 #if defined(__MMI_ZI_TURKISH__)
    {
     INPUT_MODE_SMART_UPPERCASE_TURKISH,
     ZI8_LANG_TR,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},

    {
     INPUT_MODE_SMART_LOWERCASE_TURKISH,
     ZI8_LANG_TR,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_TURKISH__) */ 

 #if defined(__MMI_ZI_UKRAINIAN__)
    {
     INPUT_MODE_SMART_UPPERCASE_UKRAINIAN,
     ZI8_LANG_UK,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},

    {
     INPUT_MODE_SMART_LOWERCASE_UKRAINIAN,
     ZI8_LANG_UK,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_UKRAINIAN__) */ 

 #if defined(__MMI_ZI_URDU__)
    {
     INPUT_MODE_SMART_URDU,
     ZI8_LANG_UR,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_URDU__) */ /* #if defined(__MMI_ZI_URDU__)  */

 #if defined(__MMI_ZI_VIETNAMESE__)
    {
     INPUT_MODE_SMART_UPPERCASE_VIETNAMESE,
     ZI8_LANG_VI,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},

    {
     INPUT_MODE_SMART_LOWERCASE_VIETNAMESE,
     ZI8_LANG_VI,
     ZI8_SUBLANG_DEFAULT,
     MMI_IME_REQUIRED_ALL_KEYS,
     MMI_IME_REQUIRED_ALL_NUM_KEYS & (~MMI_IME_REQUIRED_KEY_0),
     NULL,
     NULL,
     mmi_ime_ezi_get_candidates_alphabetic},
 #endif /* defined(__MMI_ZI_VIETNAMESE__) */ 

    {
     INPUT_MODE_MAX_NUM,
     0,
     0,
     0,
     0,
     NULL,
     NULL,
     NULL}
};

/*****************************************************************************
 * Local variables
 *****************************************************************************/

/* The Zi8 engine singleton */
static mmi_ime_ezi_word_engine_struct g_engine;

/*****************************************************************************
 *          Macros
 *****************************************************************************/


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_toupper
 * DESCRIPTION
 *  Convert character to uppercase.
 * PARAMETERS
 *  wc      [IN]        Character to convert.
 * RETURNS
 *  Each of these routines converts a copy of wc, if possible, and returns the
 *  result.
 *****************************************************************************/
U16 mmi_ime_word_toupper(U16 wc)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* Latin Basic        0x0000 ~ 0x007F */
    /* Latin-1            0x0080 ~ 0x00FF */
    /* Latin extend-A     0x0100 ~ 0x017F */
    /* Latin extend-B     0x0180 ~ 0x024F */
    /* Greek              0x0370 ~ 0x03FF */
    /* Cyrillic           0x0400 ~ 0x04FF */

    if ((wc >= 0x0061 && wc <= 0x007A) ||   /* Latin basic */
        (wc >= 0x00E0 && wc <= 0x00F6) || (wc >= 0x00F8 && wc <= 0x00FE) ||     /* Latin-1 */
        (wc >= 0x0430 && wc <= 0x044F) ||   /* Cyrillic */
        (wc >= 0x03B1 && wc <= 0x03C1) || (wc >= 0x03C3 && wc <= 0x03CA))       /* Greek */
    {
        return (wc - 0x0020);
    }
    else if (wc == 0x0103 ||    /* Latin extend-A */
             wc == 0x01A1 || wc == 0x01B0 || wc == 0x0188 || wc == 0x018C ||    /* Latin extend-B */
             wc == 0x0192 || wc == 0x01AC || wc == 0x01B9 || wc == 0x01F3)      /* Latin extend-B */
    {
        return (wc - 0x0001);
    }
    else if (wc >= 0x0450 && wc <= 0x045f)  /* Cyrillic */
    {
        return (wc - 0x50);
    }
    else if (wc == 0x0491)  /* Cyrillic */
    {
        return 0x0490;
    }
    else if (wc >= 0x0100 && wc <= 0x0137 || wc >= 0x014A && wc <= 0x0177 ||    /* Latin extend-A */
             wc >= 0x0182 && wc <= 0x0185 || wc >= 0x0198 && wc <= 0x01A5 ||    /* Latin extend-B */
             wc >= 0x01DE && wc <= 0x01EF || wc >= 0x01F8 && wc <= 0x01FF ||    /* Latin extend-B */
             wc >= 0x0222 && wc <= 0x0233)  /* Latin extend-B */
    {
        if ((wc & 1) == 1)  /* odd is small case */
            return wc - 1;
        else
            return wc;
    }
    else if (wc >= 0x0139 && wc <= 0x0148 ||    /* Latin extend-A */
             wc >= 0x0179 && wc <= 0x017E || wc >= 0x01B3 && wc <= 0x01B6 ||    /* Latin extend-B */
             wc >= 0x01CD && wc <= 0x01DC)      /* Latin extend-B */
    {
        if ((wc & 1) == 0)  /* even is small case */
            return wc - 1;
        else
            return wc;
    }
    else
    {
        return wc;
    }

}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_tolower
 * DESCRIPTION
 *  Convert character to lowercase.
 * PARAMETERS
 *  wc      [IN]        Character to convert.
 * RETURNS
 *  Each of these routines converts a copy of wc, if possible, and returns the
 *  result.
 *****************************************************************************/
U16 mmi_ime_word_tolower(U16 wc)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* Latin Basic        0x0000 ~ 0x007F */
    /* Latin-1            0x0080 ~ 0x00FF */
    /* Latin extend-A     0x0100 ~ 0x017F */
    /* Latin extend-B     0x0180 ~ 0x024F */
    /* Greek              0x0370 ~ 0x03FF */
    /* Cyrillic           0x0400 ~ 0x04FF */

    if ((wc >= 0x0041 && wc <= 0x005A) ||   /* Latin basic */
        (wc >= 0x00C0 && wc <= 0x00D6) || (wc >= 0x00D8 && wc <= 0x00DE) ||     /* Latin-1 */
        (wc >= 0x0410 && wc <= 0x042F) ||   /* Cyrillic */
        (wc >= 0x0391 && wc <= 0x03A1) || (wc >= 0x03A3 && wc <= 0x03AA))       /* Greek */
    {
        return (wc + 0x0020);
    }
    else if (wc == 0x0102 ||    /* Latin extend-A */
             wc == 0x01A0 || wc == 0x01AF || wc == 0x0187 || wc == 0x018B ||    /* Latin extend-B */
             wc == 0x0191 || wc == 0x01AB || wc == 0x01B8 || wc == 0x01F2)      /* Latin extend-B */
    {
        return (wc + 0x0001);
    }
    else if (wc >= 0x0400 && wc <= 0x040F)  /* Cyrillic */
    {
        return (wc + 0x50);
    }
    else if (wc == 0x0490)  /* Cyrillic */
    {
        return 0x0491;
    }
    else if (wc >= 0x0100 && wc <= 0x0137 || wc >= 0x014A && wc <= 0x0177 ||    /* Latin extend-A */
             wc >= 0x0182 && wc <= 0x0185 || wc >= 0x0198 && wc <= 0x01A5 ||    /* Latin extend-B */
             wc >= 0x01DE && wc <= 0x01EF || wc >= 0x01F8 && wc <= 0x01FF ||    /* Latin extend-B */
             wc >= 0x0222 && wc <= 0x0233)  /* Latin extend-B */
    {
        if ((wc & 0x0001) == 0) /* odd is small case */
            return wc + 1;
        else
            return wc;
    }
    else if (wc >= 0x0139 && wc <= 0x0148 ||    /* Latin extend-A */
             wc >= 0x0179 && wc <= 0x017E || wc >= 0x01B3 && wc <= 0x01B6 ||    /* Latin extend-B */
             wc >= 0x01CD && wc <= 0x01DC)      /* Latin extend-B */
    {
        if ((wc & 0x0001) == 1) /* even is small case */
            return wc + 1;
        else
            return wc;
    }
    else
    {
        return wc;
    }

}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_init
 * DESCRIPTION
 * PARAMETERS
 *  void
 * RETURNS
 *****************************************************************************/
MMI_BOOL mmi_ime_word_init(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (MMI_FALSE != g_engine.is_inited)
        return MMI_TRUE;

    memset(&g_engine, 0, sizeof(mmi_ime_ezi_word_engine_struct));

    g_engine.get_param.pElements = &g_engine.data.elem_buff[0];
    g_engine.get_param.pCandidates = NULL;

    g_engine.is_inited = Zi8InitializeDynamic((PZI8VOID) gIMELDBArray); /* Initialize LDB table */

    return g_engine.is_inited;
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_deinit
 * DESCRIPTION
 *  Deinitialize Word object
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void mmi_ime_word_deinit(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (MMI_FALSE != g_engine.is_inited)
    {
        memset(&g_engine, 0, sizeof(mmi_ime_ezi_word_engine_struct));
        g_engine.is_inited = MMI_FALSE;
    }
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_leave_current_mode
 * DESCRIPTION
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void mmi_ime_word_leave_current_mode(void)
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
 *  mode_details_ptr        [IN]        
 * RETURNS
 *****************************************************************************/
MMI_BOOL mmi_ime_word_change_mode(const mmi_ime_mode_details_struct *mode_details_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    ZI8BOOL ret_val;
    mmi_ime_ezi_mode_info_struct_p mode_info_ptr;
    S32 curr_mode;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    curr_mode = mmi_imm_get_current_input_mode();

    if (MMI_FALSE == g_engine.is_inited)
        return MMI_FALSE;

    mode_info_ptr = (mmi_ime_ezi_mode_info_struct_p) mmi_ime_ezi_query_mode_info((curr_mode));

    g_engine.curr_mode_info = NULL;

    if (NULL == mode_info_ptr)
        return MMI_FALSE;

    g_engine.get_param.Language = mode_info_ptr->language;
    g_engine.get_param.SubLanguage = mode_info_ptr->sub_language;

    if ((mode_details_ptr->flags & MMI_SMART_MODE) && (mode_details_ptr->flags & MMI_IME_ALPHABETIC))
    {
        ret_val = Zi8SetPredictionStartAt(MMI_IME_PREDICTION_START_AT);
        if (ret_val != ZI8TRUE)
        {
            /* add error */
            return MMI_FALSE;
        }
    }
    else
    {
        ret_val = Zi8SetPredictionStartAt(255); /* Turn off prediction */
        if (ret_val != ZI8TRUE)
        {
            /* add error */
            return MMI_FALSE;
        }
    }

    if ((mode_details_ptr->flags & MMI_IME_UPPERCASE))
    {
        g_engine.case_type = MMI_IME_WORD_CASE_UPPER;
    }
    else if (mode_details_ptr->flags & MMI_IME_LOWERCASE)
    {
        g_engine.case_type = MMI_IME_WORD_CASE_LOWER;
    }
    else
    {
        g_engine.case_type = MMI_IME_WORD_CASE_DEFAULT;
    }

    g_engine.get_param.FirstCandidate = 0;
    g_engine.get_param.MaxCandidates = 0;
    g_engine.get_param.WordCharacterCount = 0;
    g_engine.get_param.ElementCount = 0;

    /* g_engine.get_param.pElements     = NULL; */
    g_engine.get_param.pCandidates = NULL;
    g_engine.get_param.pCurrentWord = NULL;

    g_engine.curr_mode_info = mode_info_ptr;    /* Append new input mode */

    return MMI_TRUE;
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_is_valid_key
 * DESCRIPTION
 * PARAMETERS
 *  input_mode      [IN]        Input mode
 *  key_code        [IN]        
 * RETURNS
 *  Is valid key for this input mode
 *****************************************************************************/
MMI_BOOL mmi_ime_word_is_valid_key(S32 input_mode, U16 key_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mmi_ime_ezi_mode_info_struct_p mode_info_p;
    U32 required_key;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    mode_info_p = (mmi_ime_ezi_mode_info_struct_p) mmi_ime_ezi_query_mode_info(input_mode);

    if (mode_info_p == NULL)
        return MMI_FALSE;

    required_key = mode_info_p->required_keys;

    switch (key_code)
    {
        case KEY_LSK:
        case KEY_RSK:
    #if defined (__MMI_WITH_C_KEY__)
        case KEY_CSK:
    #endif /* defined (__MMI_WITH_C_KEY__) */ 
        case KEY_UP_ARROW:
        case KEY_DOWN_ARROW:
        case KEY_LEFT_ARROW:
        case KEY_RIGHT_ARROW:
            return MMI_TRUE;
        case KEY_0:
        case KEY_1:
        case KEY_2:
        case KEY_3:
        case KEY_4:
        case KEY_5:
        case KEY_6:
        case KEY_7:
        case KEY_8:
        case KEY_9:
            if (required_key & (MMI_IME_REQUIRED_KEY_0 << (key_code - KEY_0)))
            {
                return MMI_TRUE;
            }
            else
            {
                return MMI_FALSE;
            }
        case KEY_STAR:
            if (required_key & MMI_IME_REQUIRED_KEY_STAR)
            {
                return MMI_TRUE;
            }
            else
            {
                return MMI_FALSE;
            }
        case KEY_POUND:
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
 *  param_ptr       [IN]        
 *  result_ptr      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void mmi_ime_word_get_candidates(mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr)
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
        if (NULL != g_engine.curr_mode_info->get_cand_func)
        {
            g_engine.curr_mode_info->get_cand_func(param_ptr, result_ptr);
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_get_associates
 * DESCRIPTION
 * PARAMETERS
 *  param_ptr       [IN]        
 *  result_ptr      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void mmi_ime_word_get_associates(mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr)
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
        if (NULL != g_engine.curr_mode_info->get_assoc_func)
        {
            g_engine.curr_mode_info->get_assoc_func(param_ptr, result_ptr);
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_get_spellings
 * DESCRIPTION
 * PARAMETERS
 *  param_ptr       [IN]        
 *  result_ptr      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void mmi_ime_word_get_spellings(mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr)
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
        if (NULL != g_engine.curr_mode_info->get_spell_func)
        {
            g_engine.curr_mode_info->get_spell_func(param_ptr, result_ptr);
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_word_symbol_to_code
 * DESCRIPTION
 * PARAMETERS
 *  symbol      [IN]        
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
#if defined(__MMI_LANG_SM_CHINESE__) || defined(__MMI_LANG_TR_CHINESE__)
    else if (MMI_IMC_IS_SYMB_STROKE(symbol))
    {
        return mmi_ime_ezi_zicode_to_code(g_ezi_codes_stroke[symbol - MMI_IMC_SYMB_STROKE_BASE]);
    }
#endif /* defined(__MMI_LANG_SM_CHINESE__) || defined(__MMI_LANG_TR_CHINESE__) */ 
#if defined(__MMI_LANG_SM_CHINESE__)
    else if (MMI_IMC_IS_SYMB_PY(symbol))
    {
        return mmi_ime_ezi_zicode_to_code(g_ezi_codes_pinyin[symbol - MMI_IMC_SYMB_PY_BASE]);
    }
#endif /* defined(__MMI_LANG_SM_CHINESE__) */ 
#if defined(__MMI_LANG_TR_CHINESE__)
    else if (MMI_IMC_IS_SYMB_ZY(symbol))
    {
        return mmi_ime_ezi_zicode_to_code(g_ezi_codes_bopomofo[symbol - MMI_IMC_SYMB_ZY_BASE]);
    }
#endif /* defined(__MMI_LANG_TR_CHINESE__) */ 
    else if (Zi8IsComponent(symbol))
    {
        return symbol;
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
 *  code        [IN]        
 * RETURNS
 *****************************************************************************/
U16 mmi_ime_word_code_to_symbol(UI_character_type code)
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
                return code - 0x0061 + MMI_IMC_SYMB_PY_BASE;    /* mmi_imc_symbol_pinyin_enum */
            }
            else
    #endif /* __MMI_ZI_SM_CHINESE__ */ 
        #ifdef __MMI_ZI_TR_CHINESE__
            if (code >= 0x3105 && code <= 0x3129)
            {
                return code - 0x3105 + MMI_IMC_SYMB_ZY_BASE;    /* mmi_imc_symbol_bopomofo_enum */
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
 *  input_mode      [IN]        
 *  key_code        [IN]        
 * RETURNS
 *****************************************************************************/
MMI_BOOL mmi_ime_word_is_symbol_key(S32 input_mode, mmi_imc_key_custom_value key_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (key_code)
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
            if (g_engine.curr_mode_info->symbol_keys & (MMI_IME_REQUIRED_KEY_0 << (key_code - MMI_IMC_KEY_SYM_0)))
            {
                return MMI_TRUE;
            }
            else
            {
                return MMI_FALSE;
            }
        case MMI_IMC_KEY_SYM_STAR:
            if (g_engine.curr_mode_info->symbol_keys & MMI_IME_REQUIRED_KEY_STAR)
            {
                return MMI_TRUE;
            }
            else
            {
                return MMI_FALSE;
            }
        case MMI_IMC_KEY_SYM_POUND:
            if (g_engine.curr_mode_info->symbol_keys & MMI_IME_REQUIRED_KEY_POUND)
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
 *  destination_buffer      [OUT]       
 *  source_buffer           [IN]        
 *  num                     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void mmi_ime_word_add_separator_to_candidate_buffer(UI_character_type *destination_buffer, UI_character_type *source_buffer, S32 num)
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
        destination_buffer[j + 1] = 0;  /* Default separator is '\0' */
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
 *  code            [IN]        
 *  info_buffer     [IN]        
 *  buffer_size     [IN]        
 *  mode            [IN]        
 * RETURNS
 *****************************************************************************/
MMI_BOOL mmi_ime_word_engine_get_char_info(UI_character_type code, UI_string_type info_buffer, S32 buffer_size, S32 mode)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ZI8UCHAR get_mode;
    S32 i, j;
    ZI8UCHAR ret;
    UI_character_type char_code;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    switch (mode)
    {
        case INPUT_MODE_TR_BOPOMO:
        case INPUT_MODE_TR_MULTITAP_BOPOMO:
            get_mode = ZI8_GETMODE_BOPOMOFO_NAME;
            break;
        case INPUT_MODE_TR_STROKE:
            get_mode = ZI8_GETMODE_STROKES;
            break;
        case INPUT_MODE_SM_PINYIN:
        case INPUT_MODE_MULTITAP_PINYIN:
            get_mode = ZI8_GETMODE_PINYIN_NAME;
            break;
        case INPUT_MODE_SM_STROKE:
            get_mode = ZI8_GETMODE_STROKES;
            break;
        default:
            return MMI_FALSE;
            break;
    }

    ret = Zi8GetCharInfo((ZI8WCHAR) code, (PZI8WCHAR) info_buffer, (ZI8UCHAR) buffer_size, (ZI8UCHAR) get_mode);
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
 *  mmi_ime_word_is_redicals
 * DESCRIPTION
 * PARAMETERS
 *  key_code        [IN]        
 * RETURNS
 *****************************************************************************/
MMI_BOOL mmi_ime_word_is_redicals(UI_character_type key_code)
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
 *  code        [IN]        
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
 *  mmi_ime_ezi_get_candidates_alphabetic
 * DESCRIPTION
 * PARAMETERS
 *  param_ptr       [IN]        
 *  result_ptr      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void mmi_ime_ezi_get_candidates_alphabetic(mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ASSERT(NULL != param_ptr);

    if (0 == param_ptr->elembuf_cnt)
        return;

    mmi_ime_ezi_fill_elem_buff(param_ptr, MMI_IMC_SYMB_KEY_BASE, MMI_IMC_SYMB_KEY_MAX, g_ezi_keys_latin);
    if (g_engine.get_param.ElementCount == 0)
    {
        /* add error */
        return;
    }

    mmi_ime_ezi_get_candidates(param_ptr, result_ptr, ZI8_GETMODE_DEFAULT, ZI8_GETOPTION_WSTRINGS);

    if (result_ptr->result_cnt > 0)
    {
        /* adjust result case */
        mmi_ime_ezi_change_candidates_case(result_ptr);
    }
}

#if defined(__MMI_ZI_SM_CHINESE__) || defined(__MMI_ZI_TR_CHINESE__)


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_get_spellings_stroke
 * DESCRIPTION
 * PARAMETERS
 *  param_ptr       [IN]        
 *  result_ptr      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void mmi_ime_ezi_get_spellings_stroke(mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 i;
    U8 symb;
    UI_string_type temp_str;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ASSERT(NULL != param_ptr);
    if (0 == param_ptr->elembuf_cnt)
        return;

    temp_str = result_ptr->result_buffer;

    for (i = 0; i < param_ptr->elembuf_cnt; ++i)
    {
        symb = param_ptr->elembuf_ptr[i];
        if (MMI_FALSE != MMI_IMC_IS_SYMB_KEY(symb))
        {
            if (g_ezi_keys_stroke[symb - MMI_IMC_SYMB_KEY_BASE])
            {
                *temp_str = mmi_ime_word_symbol_to_code((U8) g_ezi_keys_stroke[symb - MMI_IMC_SYMB_KEY_BASE]);
                temp_str++;
            }
            else    /* Invalid key */
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
 *  param_ptr       [IN]        
 *  result_ptr      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void mmi_ime_ezi_get_candidates_stroke(mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ASSERT(NULL != param_ptr);
    if (0 == param_ptr->elembuf_cnt)
    {
        return;
    }

    mmi_ime_ezi_fill_elem_buff(param_ptr, MMI_IMC_SYMB_STROKE_BASE, MMI_IMC_SYMB_STROKE_MAX, g_ezi_codes_stroke);
    if (g_engine.get_param.ElementCount == 0)
    {
        /* add error */
        return;
    }

    mmi_ime_ezi_get_candidates(param_ptr, result_ptr, ZI8_GETMODE_STROKES, ZI8_GETOPTION_DEFAULT);
}
#endif /* defined(__MMI_ZI_SM_CHINESE__) || defined(__MMI_ZI_TR_CHINESE__) */ 

#if defined(__MMI_ZI_SM_CHINESE__)

#if defined(__MMI_ZI_MULTITAP_PHONETIC_INPUT__)


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_get_spellings_pinyin_multitap
 * DESCRIPTION
 * PARAMETERS
 *  param_ptr       [IN]        
 *  result_ptr      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void mmi_ime_ezi_get_spellings_pinyin_multitap(mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    U8 i;
    UI_string_type temp_str;
    U8 symb;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ASSERT(NULL != param_ptr);
    if (0 == param_ptr->elembuf_cnt)
        return;

    temp_str = result_ptr->result_buffer;

    for (i = 0; i < param_ptr->elembuf_cnt; i++)
    {
        symb = param_ptr->elembuf_ptr[i];
        if (MMI_FALSE != MMI_IMC_IS_SYMB_PY(symb))
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

#endif /* defined(__MMI_ZI_MULTITAP_PHONETIC_INPUT__) */ 

#if defined(__MMI_ZI_SMART_PHONETIC_INPUT__)


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_get_spellings_pinyin_smart
 * DESCRIPTION
 * PARAMETERS
 *  param_ptr       [IN]        
 *  result_ptr      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void mmi_ime_ezi_get_spellings_pinyin_smart(mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 cnt;
    UI_string_type temp_result_ptr;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ASSERT(NULL != param_ptr);

    if (0 == param_ptr->elembuf_cnt)
        return;

    mmi_ime_ezi_fill_elem_buff(param_ptr, MMI_IMC_SYMB_KEY_BASE, MMI_IMC_SYMB_KEY_MAX, g_ezi_onekeys);
    if (g_engine.get_param.ElementCount == 0)
    {
        /* add error */
        return;
    }

    mmi_ime_ezi_get_candidates(param_ptr, result_ptr, ZI8_GETMODE_1KEYPRESS_PINYIN, ZI8_GETOPTION_GET_SPELLING);
    if (result_ptr->result_cnt > 0)
    {
        temp_result_ptr = result_ptr->result_buffer;

        cnt = 0;
        while (cnt < result_ptr->result_cnt)
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

#endif /* defined(__MMI_ZI_SMART_PHONETIC_INPUT__) */ 


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_get_candidates_pinyin
 * DESCRIPTION
 * PARAMETERS
 *  param_ptr       [IN]        
 *  result_ptr      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void mmi_ime_ezi_get_candidates_pinyin(mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ASSERT(NULL != param_ptr);
    if (0 == param_ptr->elembuf_cnt)
        return;

    mmi_ime_ezi_fill_elem_buff(param_ptr, MMI_IMC_SYMB_PY_BASE, MMI_IMC_SYMB_PY_MAX, g_ezi_codes_pinyin);
    if (g_engine.get_param.ElementCount == 0)
    {
        /* add error */
        return;
    }

    mmi_ime_ezi_get_candidates(param_ptr, result_ptr, ZI8_GETMODE_PINYIN, ZI8_GETOPTION_DEFAULT);
}

#endif /* defined(__MMI_ZI_SM_CHINESE__) */ 

#if defined(__MMI_ZI_TR_CHINESE__)

#if defined(__MMI_ZI_MULTITAP_PHONETIC_INPUT__)


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_get_spellings_zhuyin_multitap
 * DESCRIPTION
 * PARAMETERS
 *  param_ptr       [IN]        
 *  result_ptr      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void mmi_ime_ezi_get_spellings_zhuyin_multitap(mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    U8 i;
    ZI8WCHAR symb;
    UI_string_type temp_str;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ASSERT(NULL != param_ptr);
    if (0 == param_ptr->elembuf_cnt)
        return;

    temp_str = result_ptr->result_buffer;

    for (i = 0; i < param_ptr->elembuf_cnt; i++)
    {
        symb = param_ptr->elembuf_ptr[i];
        if (MMI_FALSE != MMI_IMC_IS_SYMB_ZY(symb))
        {
            *temp_str = mmi_ime_word_symbol_to_code((U8) symb);
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

#endif /* defined(__MMI_ZI_MULTITAP_PHONETIC_INPUT__) */ 

#if defined(__MMI_ZI_SMART_PHONETIC_INPUT__)


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_get_spellings_zhuyin_smart
 * DESCRIPTION
 * PARAMETERS
 *  param_ptr       [IN]        
 *  result_ptr      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void mmi_ime_ezi_get_spellings_zhuyin_smart(mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 cnt;
    UI_string_type temp_result_ptr;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ASSERT(NULL != param_ptr);
    if (0 == param_ptr->elembuf_cnt)
        return;

    mmi_ime_ezi_fill_elem_buff(param_ptr, MMI_IMC_SYMB_KEY_BASE, MMI_IMC_SYMB_KEY_MAX, g_ezi_onekeys);
    if (g_engine.get_param.ElementCount == 0)
    {
        /* add error */
        return;
    }

    mmi_ime_ezi_get_candidates(param_ptr, result_ptr, ZI8_GETMODE_1KEYPRESS_BOPOMOFO, ZI8_GETOPTION_GET_SPELLING);

    if (result_ptr->result_cnt > 0)
    {
        temp_result_ptr = result_ptr->result_buffer;

        cnt = 0;
        while (cnt < result_ptr->result_cnt)
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

#endif /* defined(__MMI_ZI_SMART_PHONETIC_INPUT__) */ 


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_get_candidates_zhuyin
 * DESCRIPTION
 * PARAMETERS
 *  param_ptr       [IN]        
 *  result_ptr      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void mmi_ime_ezi_get_candidates_zhuyin(mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ASSERT(NULL != param_ptr);
    if (0 == param_ptr->elembuf_cnt)
        return;

    mmi_ime_ezi_fill_elem_buff(param_ptr, MMI_IMC_SYMB_ZY_BASE, MMI_IMC_SYMB_ZY_MAX, g_ezi_codes_bopomofo);
    if (g_engine.get_param.ElementCount == 0)
    {
        /* add error */
        return;
    }

    mmi_ime_ezi_get_candidates(param_ptr, result_ptr, ZI8_GETMODE_BOPOMOFO, ZI8_GETOPTION_DEFAULT);
}

#endif /* defined(__MMI_ZI_TR_CHINESE__) */ 


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_fill_elem_buff
 * DESCRIPTION
 *  Fill element buffer by eZi symbol according to IMC symbol
 * PARAMETERS
 *  param_ptr               [IN]        
 *  symb_range_begin        [IN]        
 *  symb_range_end          [IN]        
 *  symb_conv_table         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void mmi_ime_ezi_fill_elem_buff(mmi_ime_query_param_struct_p param_ptr, U8 symb_range_begin, U8 symb_range_end, const ZI8WCHAR *symb_conv_table)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    U8 i;
    U16 symb_code;
    ZI8WCHAR zi_code;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ASSERT(symb_range_begin <= symb_range_end);

    g_engine.get_param.ElementCount = 0;

    for (i = 0; i < param_ptr->elembuf_cnt; ++i)
    {
        symb_code = param_ptr->elembuf_ptr[i];
        if (symb_code >= symb_range_begin && symb_code < symb_range_end)
        {
            zi_code = symb_conv_table[symb_code - symb_range_begin];
            if (zi_code == 0)
                break;
            g_engine.get_param.pElements[i] = zi_code;
            g_engine.get_param.ElementCount++;
        }
        else if (Zi8IsComponent(symb_code))
        {
            g_engine.get_param.pElements[i] = symb_code;
            g_engine.get_param.ElementCount++;
        }
        else
        {
            break;
        }
    }

    if (i < param_ptr->elembuf_cnt)
    {
        g_engine.get_param.pElements[0] = 0;
        g_engine.get_param.ElementCount = 0;
    }
    else
    {
        symb_code = param_ptr->tone_value;
        if ((symb_code >= MMI_IMC_SYMB_TONE_BASE && symb_code < MMI_IMC_SYMB_TONE_MAX))
        {
            zi_code = g_ezi_codes_tone[symb_code - MMI_IMC_SYMB_TONE_BASE];
            if (zi_code != 0)
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
 *  param_ptr       [IN]        
 *  result_ptr      [IN]        
 *  get_mode        [IN]        
 *  get_options     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void mmi_ime_ezi_get_candidates(mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr, ZI8UCHAR get_mode, ZI8UCHAR get_options)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    UI_character_type candidate_buffer[256];
    UI_string_type temp_ptr;
    S32 i, j;
    ZI8GETPARAM temp_get_param;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (param_ptr->first_index >= MMI_IMC_MAX_CANDIDATE_BUFFSIZE)
    {
        result_ptr->result_cnt = 0;
        result_ptr->result_buffer[0] = '\0';
        return;
    }

    switch (g_engine.get_context)
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
    g_engine.get_param.Context |= ZI8_GETCONTEXT_SINGLE_CHAR;

    g_engine.get_param.GetMode = get_mode;
    g_engine.get_param.GetOptions = get_options;

    g_engine.get_param.MaxCandidates = param_ptr->desired_cnt + 1;      /* Plus 1 for getting next page info */
    g_engine.get_param.WordCharacterCount = 0;
    g_engine.get_param.FirstCandidate = (ZI8WCHAR) param_ptr->first_index;
    g_engine.get_param.pCandidates = (PZI8WCHAR) candidate_buffer;

    /* This is a tricky way to resolve a issue of ZI */
    /* This segment must be removed after Zi has fixed the bug  */
    memset(&temp_get_param, 0, sizeof(ZI8GETPARAM));

    memcpy(&temp_get_param, &g_engine.get_param, sizeof(ZI8GETPARAM));

    temp_get_param.ElementCount = 0;

    Zi8GetCandidates(&temp_get_param);

    result_ptr->result_cnt = Zi8GetCandidates(&g_engine.get_param);

    if (result_ptr->result_cnt) /* Get next page info and resume candidate list */
    {

        if (get_options == ZI8_GETOPTION_WSTRINGS || get_options == ZI8_GETOPTION_GET_SPELLING)
        {
            temp_ptr = result_ptr->result_buffer;

            i = 0;
            j = 0;

            while (i < result_ptr->result_cnt)
            {
                result_ptr->result_buffer[j] = candidate_buffer[j];
                if (candidate_buffer[j] == 0 || candidate_buffer[j] == 0x20)
                {
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


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_get_associates
 * DESCRIPTION
 * PARAMETERS
 *  param_ptr       [IN]        
 *  result_ptr      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void mmi_ime_ezi_get_associates(mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    UI_character_type candidate_buffer[MMI_IMC_MAX_CANDIDATE_IN_ONE_PAGE_CHINESE + 2];

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    g_engine.get_param.Context = ZI8_GETCONTEXT_DEFAULT;
    g_engine.get_param.GetMode = ZI8_GETMODE_DEFAULT;
    g_engine.get_param.GetOptions = ZI8_GETOPTION_DEFAULT;

    g_engine.get_param.ElementCount = 0;

    g_engine.get_param.MaxCandidates = (ZI8UCHAR) param_ptr->desired_cnt + 1;   /* Plus 1 for getting next page info */
    g_engine.get_param.FirstCandidate = (ZI8WCHAR) param_ptr->first_index;
    g_engine.get_param.pCandidates = (PZI8WCHAR) candidate_buffer;

    g_engine.get_param.WordCharacterCount = (ZI8UCHAR) pfnUnicodeStrlen((const S8*)param_ptr->pre_string);
    g_engine.get_param.pCurrentWord = (PZI8WCHAR) param_ptr->pre_string;

    result_ptr->result_cnt = Zi8GetCandidates(&g_engine.get_param);

    if (result_ptr->result_cnt) /* Get next page info and resume candidate list */
    {
        mmi_ime_word_add_separator_to_candidate_buffer(result_ptr->result_buffer, candidate_buffer, result_ptr->result_cnt);
        mmi_ime_ezi_check_page_info_and_resume_result(result_ptr);
    }
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_query_mode_info
 * DESCRIPTION
 * PARAMETERS
 *  input_mode      [IN]        
 * RETURNS
 *****************************************************************************/
static const mmi_ime_ezi_mode_info_struct *mmi_ime_ezi_query_mode_info(S32 input_mode)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    U8 i;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (input_mode >= INPUT_MODE_MAX_NUM)
        return NULL;

    for (i = 0; g_mode_infos[i].input_mode != INPUT_MODE_MAX_NUM; ++i)
    {
        if (g_mode_infos[i].input_mode == input_mode)
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
 *  result_str      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void mmi_ime_ezi_change_candidates_case(mmi_ime_query_result_struct_p result_str)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 i = 0, j = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_engine.case_type == MMI_IME_WORD_CASE_UPPER)
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
    else if (g_engine.case_type == MMI_IME_WORD_CASE_LOWER)
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
 *  char_code       [IN]        
 * RETURNS
 *****************************************************************************/
static UI_character_type mmi_ime_ezi_char_to_upper(UI_character_type char_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
#if defined(__MMI_ZI_V7__)
    UI_character_type inChar = char_code;
#endif /* defined(__MMI_ZI_V7__) */ 

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#if defined(__MMI_ZI_V7__)

    Zi8ChangeCharCase(ZI8TRUE, (PZI8WCHAR) & inChar, g_engine.curr_mode_info->language);

    return inChar;

#else /* defined(__MMI_ZI_V7__) */ 

    return (UI_character_type) mmi_ime_word_toupper((U16) char_code);

#endif /* defined(__MMI_ZI_V7__) */ 
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_char_to_lower
 * DESCRIPTION
 * PARAMETERS
 *  char_code       [IN]        
 * RETURNS
 *****************************************************************************/
static UI_character_type mmi_ime_ezi_char_to_lower(UI_character_type char_code)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
#if defined(__MMI_ZI_V7__)
    ZI8WCHAR inChar = char_code;
#endif /* defined(__MMI_ZI_V7__) */ 

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#if defined(__MMI_ZI_V7__)
    Zi8ChangeCharCase(ZI8FALSE, (PZI8WCHAR) & inChar, g_engine.curr_mode_info->language);
    return inChar;
#else /* defined(__MMI_ZI_V7__) */ 
    return (UI_character_type) mmi_ime_word_tolower((U16) char_code);
#endif /* defined(__MMI_ZI_V7__) */ 
}


/*****************************************************************************
 * FUNCTION
 *  mmi_ime_ezi_zicode_to_code
 * DESCRIPTION
 * PARAMETERS
 *  zicode      [IN]        
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

        default:
            if (zicode >= ZI8_CODE_PINYIN_A && zicode <= ZI8_CODE_PINYIN_Z)
            {
                return zicode - ZI8_BASE_PINYIN + 0x0061;
            }
            else if (zicode >= ZI8_CODE_BPMF_B && zicode <= ZI8_CODE_BPMF_IU)
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
 *  result_ptr      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void mmi_ime_ezi_check_page_info_and_resume_result(mmi_ime_query_result_struct_p result_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    U8 i;
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
        while (i < result_ptr->result_cnt)  /* Go to the last candidate */
        {
            if ((*temp_str) == 0)
            {
                i++;
            }
            temp_str++;
        }

        while ((*temp_str) != 0)    /* Delete the last candidate */
        {
            *temp_str = 0;
        }
    }
    else
    {
        result_ptr->next_page = MMI_FALSE;
    }
}

#endif /* __MMI_ZI__ */ /* #ifdef  __MMI_ZI__ */

