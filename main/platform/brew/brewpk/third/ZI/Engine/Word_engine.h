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
 *    Word_engine.h
 *
 * Project:
 * --------
 *   Maui_Software
 *
 * Description:
 * ------------
 *   common header file for ZiEngine.c, T9Engine.c and CstarEngine.c
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
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#include "OEMFeatures.h"

#ifndef __WORD_ENGINE_H__
#define __WORD_ENGINE_H__

#include "IMERes.h"
#include "ImeGProt.h"
#include "Imc_config.h"
#include "Imc_symbol.h"

/*****************************************************************************
 * Define
 *****************************************************************************/

#define MMI_IME_WORD_CONTEXT_DEFAULT    (0)
#define MMI_IME_WORD_CONTEXT_SMS        (1)
#define MMI_IME_WORD_CONTEXT_SURNAME    (2)
#define MMI_IME_WORD_CONTEXT_GIVENNAME  (3)

#define MMI_IME_REQUIRED_KEY_0          (0x1)
#define MMI_IME_REQUIRED_KEY_1          (0x2)
#define MMI_IME_REQUIRED_KEY_2          (0x4)
#define MMI_IME_REQUIRED_KEY_3          (0x8)
#define MMI_IME_REQUIRED_KEY_4          (0x10)
#define MMI_IME_REQUIRED_KEY_5          (0x20)
#define MMI_IME_REQUIRED_KEY_6          (0x40)
#define MMI_IME_REQUIRED_KEY_7          (0x80)
#define MMI_IME_REQUIRED_KEY_8          (0x100)
#define MMI_IME_REQUIRED_KEY_9          (0x200)
#define MMI_IME_REQUIRED_KEY_POUND      (0x400)
#define MMI_IME_REQUIRED_KEY_STAR       (0x800)    

#define MMI_IME_REQUIRED_ALL_NUM_KEYS   ((MMI_IME_REQUIRED_KEY_1 | MMI_IME_REQUIRED_KEY_2) \
                                         | (MMI_IME_REQUIRED_KEY_3 | MMI_IME_REQUIRED_KEY_4) \
                                         | (MMI_IME_REQUIRED_KEY_5 | MMI_IME_REQUIRED_KEY_6) \
                                         | (MMI_IME_REQUIRED_KEY_7 | MMI_IME_REQUIRED_KEY_8) \
                                         | (MMI_IME_REQUIRED_KEY_9 | MMI_IME_REQUIRED_KEY_0))

#define MMI_IME_REQUIRED_ALL_KEYS       (MMI_IME_REQUIRED_ALL_NUM_KEYS | MMI_IME_REQUIRED_KEY_POUND | MMI_IME_REQUIRED_KEY_STAR)

/****************************************MMI_IME_REQUIRED_KEY_STAR *************************************
 * Typedef
 *****************************************************************************/

typedef struct
{
    PU16                 elembuf_ptr;    /* IN */
    U8                  elembuf_cnt;    /* IN */
    U16                 first_index;    /* IN */
    U8                  desired_cnt;    /* IN */
    
#if defined(__MMI_IME_SMART_WITH_INPUT_AREA__)
    U8                  composition_desired_cnt;  /* IN */
    U8                  composition_first_index;
    U8                  composition_index;        /* IN */
#endif /* defined(__MMI_CSTAR_SM_CHINESE__) && defined(__MMI_CSTAR_SMART_PHONETIC_INPUT__) */

    U8                  tone_value;     /* IN */
    UI_string_type      pre_string;     /* IN */
}mmi_ime_query_param_struct, *mmi_ime_query_param_struct_p;

typedef struct
{
    UI_string_type      result_buffer;  /* OUT */
    U8                  result_cnt;     /* OUT */
    MMI_BOOL            next_page;      /* OUT */
    
#if defined(__MMI_IME_SMART_WITH_INPUT_AREA__)
    UI_string_type      composition_buffer;         /* OUT */
    U8                  composition_result_cnt;     /* OUT */
    MMI_BOOL            composition_next_page;      /* OUT */
    UI_string_type      symbol_string;              /* OUT */
    U8 *                match_length_table;         /* OUT */
#endif /* defined(__MMI_CSTAR_SM_CHINESE__) && defined(__MMI_CSTAR_SMART_PHONETIC_INPUT__) */

}mmi_ime_query_result_struct, *mmi_ime_query_result_struct_p;

typedef void  (*mmi_ime_get_spellings_funcptr)( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr );
typedef void  (*mmi_ime_get_associates_funcptr)( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr );
typedef void  (*mmi_ime_get_candidates_funcptr)( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr );


typedef enum{
    MMI_IME_WORD_CAPITAL_STATE_NONE, /* don't care, follow the input mode case */
    MMI_IME_WORD_CAPITAL_STATE_LOWER,
    MMI_IME_WORD_CAPITAL_STATE_FIRST_UPPER,
    MMI_IME_WORD_CAPITAL_STATE_UPPER,
    MMI_IME_WORD_CAPITAL_STATE_MAX
} mmi_ime_word_capital_state_enum;

#if defined(__MMI_IME_USER_DATABASE__)
typedef enum{
    MMI_IME_WORD_ADD_NEW_WORD_FAILED = 0,
    MMI_IME_WORD_ADD_NEW_WORD_DUPLICAT,
    MMI_IME_WORD_ADD_NEW_WORD_SUCCESS
}mmi_ime_word_add_new_word_ret_value_enum;
#endif

/*****************************************************************************
 * Global functions
 *****************************************************************************/


/*
 * when adding new AIP, please remenber to add dummy function to Imc.c 
 * to avoid compiling error under non-vendor case
 */

/* Query APIs */
extern void     mmi_ime_word_get_spellings( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr );
extern void     mmi_ime_word_get_candidates( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr );
extern void     mmi_ime_word_get_associates( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr );

/* Utility APIs */
extern void              mmi_ime_word_set_context( U8 context );
extern MMI_BOOL          mmi_ime_word_is_valid_key( mmi_imm_input_mode_enum input_mode, U16 key_code);
extern UI_character_type mmi_ime_word_symbol_to_code(U16 symbol);
extern U16                mmi_ime_word_code_to_symbol(UI_character_type code); 
extern MMI_BOOL  mmi_ime_word_is_symbol_key( mmi_imm_input_mode_enum input_mode, mmi_imc_key_custom_value key_code );

extern MMI_BOOL  mmi_ime_word_is_redicals(UI_character_type key_code );

extern MMI_BOOL mmi_ime_word_is_wildcard(UI_character_type code);
    
extern void  mmi_ime_word_add_separator_to_candidate_buffer(UI_character_type * destination_buffer, UI_character_type * source_buffer, S32 num);

extern MMI_BOOL mmi_ime_word_engine_get_char_info(UI_character_type code, UI_string_type info_buffer, S32 buffer_size, mmi_imm_input_mode_enum mode);

extern MMI_BOOL mmi_ime_word_init( void );
extern void     mmi_ime_word_deinit( void );
extern MMI_BOOL mmi_ime_word_change_mode(const sIMEModeDetails * mode_details_ptr, U32 reserved );
extern void     mmi_ime_word_leave_current_mode(void);
extern void     mmi_ime_word_change_word_case(mmi_ime_word_capital_state_enum capital_state);

#if defined(__MMI_IME_MAGIC_ONE__)
extern MMI_BOOL mmi_ime_word_is_magic_key( mmi_imm_input_mode_enum input_mode, mmi_imc_key_custom_value key_code );
#endif

#if defined(__MMI_IME_USER_DATABASE__)

extern mmi_ime_word_add_new_word_ret_value_enum mmi_ime_word_add_new_word(mmi_imm_input_mode_enum input_mode, UI_string_type str_ptr);

extern MMI_BOOL mmi_ime_word_update_word_frequency(mmi_imm_input_mode_enum input_mode, UI_string_type str_ptr);

extern MMI_BOOL mmi_ime_word_load_user_database(void);

extern MMI_BOOL mmi_ime_word_save_user_database(void);

extern void mmi_ime_word_scan_and_add_new_words(UI_string_type str_ptr);

#endif /* defined(__MMI_IME_USER_DATABASE__) */


#if defined(__MMI_CSTAR__)

extern mmi_imc_symbol_key_enum mmi_ime_word_map_char_to_symbol(mmi_imm_input_mode_enum input_mode, UI_character_type char_code);

#endif

#endif /* !defined(__WORD_ENGINE_H__) */ 
