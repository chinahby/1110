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
 *    Imc.h
 *
 * Project:
 * --------
 *   Maui_Software
 *
 * Description:
 * ------------
 *   header file for Imc.c
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
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#include "MMI_features.h"

#ifndef __IMC_H__
#define __IMC_H__

#include "IMERes.h"

#include "FrameworkGprot.h"
#include "ImeGprot.h"
#include "ImmProt.h"
//#include "Imui.h"
#include "Imc_config.h"
#include "Word_engine.h"

/*****************************************************************************
 * Enums
 *****************************************************************************/

typedef enum {
    MMI_IMC_STATE_NONE = 0,
    MMI_IMC_STATE_INITIAL,
    MMI_IMC_STATE_SMART_CANDIDATE,
    MMI_IMC_STATE_PRE_SMART_CANDIDATE,
    MMI_IMC_STATE_ASSOC_SELECTING_ALPHABETIC,
    MMI_IMC_STATE_MULTITAP,
    MMI_IMC_STATE_SPELL_INPUTING,
    MMI_IMC_STATE_SPELL_MULTITAP,
    MMI_IMC_STATE_CAND_SELECTING,
    MMI_IMC_STATE_ASSOC_SELECTING,
    MMI_IMC_STATE_PRE_ASSOC_SELECTING,
    MMI_IMC_STATE_HAND_WRITING,
    MMI_IMC_STATE_HAND_WRITING_CAND,
    MMI_IMC_STATE_MULTI_BLOCK_HAND_WRITING,
    MMI_IMC_STATE_SPELL_WITH_INPUT, 
    MMI_IMC_STATE_CAND_SELECT_WITH_INPUT,
    MMI_IMC_STATE_VK_CAND,
    MMI_IMC_STATE_VK_CAND_MULTITAP,
    MMI_IMC_STATE_PREDICTING,
    MMI_IMC_STATE_INPUT,  /* state for cstar input */
    MMI_IMC_STATE_NEW_WORD_INITIAL,
    MMI_IMC_STATE_NEW_WORD_MULTITAP,
    MMI_IMC_STATE_NUMERIC_PASSWORD_MULTITAP
} mmi_imc_state_enum;

/*****************************************************************************
 * Typedef
 *****************************************************************************/

typedef U32 (*mmi_imc_message_funcptr)( void *, mmi_imc_message_struct_p );

typedef struct
{
    U16                          symb_buff[MMI_IMC_MAX_INPUT_SYMBOL_BUFFSIZE];
    U16                          symb_buff_backup[MMI_IMC_MAX_INPUT_SYMBOL_BUFFSIZE];
    UI_character_type           cand_buff[MMI_IMC_MAX_CANDIDATE_BUFFSIZE];
    UI_character_type           comp_buff[MMI_IMC_MAX_COMPOSITION_BUFFSIZE];
    U8                            cand_valid_flags[(MMI_IMC_MAX_CANDIDATE_PAGE*MMI_IMC_MAX_CANDIDATE_IN_ONE_PAGE_LATIN)/8];
    S32                          cand_check_cnt;
    U16                          key_buffer[MMI_IMC_MAX_KEY_BUFFER_SIZE]; /* mmi_imc_symbol_key_enum */
    S32                         key_element_length;
    U8                          symb_count;
    U8                          symb_count_backup;
    U8                          cand_count;
    U8                          cand_select;
    U16                         cand_first_index[MMI_IMC_MAX_CANDIDATE_PAGE + 1];
    U8                          cand_page;
    U8                          comp_count;
    U8                          comp_select;
    U8                          comp_first_index[MMI_IMC_MAX_COMPOSITION_PAGE + 1];
    U8                          comp_page;
    U8                          old_comp_select;
    U8                          old_comp_page;
    U16                        tone;
    UI_character_type           pre_string[MMI_IMC_MAX_PRE_STRING_LEN + 1];
    UI_string_type              permit_string_ptr;
    U8                          permit_string_len;
    UI_string_type              forbid_string_ptr;
    U8                          forbid_string_len;
    S32                        cand_length;
    mmi_ime_word_capital_state_enum  capital_state;
#if defined(__MMI_IME_SMART_WITH_INPUT_AREA__)    
    S32                         key_buffer_cusor; /* used in smart phonetic input to indicate the key element index */
    UI_character_type           input_buff[MMI_IMC_MAX_INPUT_BUFFSIZE];
    S32                          input_length;
    S32                          input_confirm_length;
    U8                          cand_match_len[MMI_IMC_MAX_CANDIDATE_IN_ONE_PAGE_CHINESE];
    U16                          key_buffer_backup[MMI_IMC_MAX_KEY_SEQUENCE_RECORD + 1][MMI_IMC_MAX_KEY_BUFFER_SIZE]; /* mmi_imc_symbol_key_enum */
    U16                          key_len_backup[MMI_IMC_MAX_KEY_SEQUENCE_RECORD + 1];
    UI_character_type    select_cand[MMI_IMC_MAX_KEY_SEQUENCE_RECORD + 1][MMI_IMC_MAX_KEY_BUFFER_SIZE + 1]; /* mmi_imc_symbol_key_enum */
    S32                        key_seq_index; /* the active key sequence */
    S32                        key_seq_num; /* total key symbol number */
    MMI_BOOL              cand_filter; /* to store candidate filter status. After recover input, it should be true and then if there is any modification to the input, it should be false; */
    UI_character_type    cand_check[MMI_IMC_MAX_KEY_BUFFER_SIZE + 1];
    U8 filter_page;
#endif

#if defined(__MMI_IME_USER_DATABASE__)
    UI_character_type          first_candidate[MMI_IMC_MAX_KEY_BUFFER_SIZE + 1];
    S32                        active_multitap;
    S32                        multitap_buffer_len;
    UI_character_type          multitap_buffer[MULTITAP_MAX_KEY_CHAR * 4];
    UI_character_type          new_word[MMI_IMC_MAX_KEY_BUFFER_SIZE + 1];
    mmi_imc_key_custom_value   multitap_key_code;
    S32                        char_index;
    UI_string_type             new_word_prompt;
#endif
}   mmi_imc_global_data_struct, *mmi_imc_global_data_struct_p;

/*****************************************************************************
*  Global Functions
*****************************************************************************/

extern MMI_BOOL mmi_imc_change_input_mode( mmi_imm_input_mode_enum input_mode, U32 reserved );

extern void     mmi_imc_start_timer(S32 count, void (*callback) (void), void (*cancel_callback) (void));
extern void     mmi_imc_restart_timer(S32 count, void (*callback) (void), void (*cancel_callback) (void));
extern void     mmi_imc_cancel_timer(void); /* Called by interrup event*/
extern FuncPtr mmi_imc_delete_timer(void);
#if defined(__MMI_TOUCH_SCREEN__)
extern MMI_BOOL     mmi_imc_is_handwriting_disabled(void);
extern void mmi_imc_set_hw_type_validity(MMI_BOOL v);
extern MMI_BOOL mmi_imc_get_hw_type_validity(void);
#endif
extern MMI_BOOL     mmi_imc_is_key_disabled(void);
extern MMI_BOOL mmi_imc_is_symbol_picker_disable(void);
extern MMI_BOOL mmi_imc_is_fixed_input_box(void);

extern mmi_imc_global_data_struct_p     mmi_imc_get_global_data( void );
extern void mmi_imc_reset_global_data(void);

extern UI_string_type mmi_imc_get_candidate_by_index(UI_character_type * cand_buff, U8 index);

extern void mmi_imc_clear_symbol_buffer(void);
extern void mmi_imc_select_candidate_from_candidate_box(void);
extern MMI_BOOL mmi_imc_change_candidate_box_page(void);

extern void mmi_imc_change_highlighted_candidate_from_candidate_box(U8 candidate_index);

extern S32 mmi_imc_change_highlighted_composition_from_composition_box(U8 candidate_index);

extern S32 mmi_imc_update_candidate_box_by_query_result(
        UI_string_type      result_buffer,
        U8                  result_cnt,
        MMI_BOOL            next_page);
extern S32 mmi_imc_update_composition_box_by_query_result(
        UI_string_type      result_buffer,
        U8                  result_cnt,
        MMI_BOOL            next_page);
extern void mmi_imc_string_to_title_case(UI_string_type string);
extern void mmi_imc_string_to_lower_case(UI_string_type string);
extern void mmi_imc_string_to_upper_case(UI_string_type string);
extern MMI_BOOL mmi_imc_candidate_check(UI_string_type cand, S32 len);
extern MMI_BOOL mmi_imc_select_composition_from_composition_box(void);

extern void mmi_imc_redraw_imui(U32 imui_set);

extern void mmi_imc_common_arrow_key_handler(mmi_imc_key_custom_value key_code, U16 event_type);
extern mmi_imc_state_enum  mmi_imc_get_state( void );
extern MMI_BOOL mmi_imc_change_composition_box_page(void);
extern void  mmi_imc_set_state( mmi_imc_state_enum state);
extern void  mmi_imc_get_candidates_from_word_engine( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr );
extern void mmi_imc_candidate_index_mapping(S32 *index);
extern void mmi_imc_set_candidate_valid_flag(S32 index, S32 flags, S32 len);
extern void mmi_imc_reset_candidate_valid_flag(void);
extern void mmi_imc_change_alphabetic_candidate_case(UI_string_type cand, mmi_ime_word_capital_state_enum state);
extern void mmi_imc_change_candidate_list_caps(mmi_ime_word_capital_state_enum state);
extern mmi_ime_word_capital_state_enum mmi_imc_get_input_mode_caps(const sIMEModeDetails * mode_details_ptr);
    
    
#if defined(__MMI_IME_MAGIC_ONE__)
extern MMI_BOOL mmi_imc_get_magic_key_down(void);
extern void mmi_imc_set_magic_key_down(MMI_BOOL val);
extern MMI_BOOL mmi_imc_change_to_magic_state(void);
extern void mmi_imc_back_from_magic_state(void);
extern MMI_BOOL mmi_imc_check_magic_state(void);
#endif

#if defined(__MMI_IME_SMART_WITH_INPUT_AREA__)
extern void mmi_imc_update_input_area(S32 highlight_start, S32 highlight_num, S32 wave_start, S32 wave_num, S32 cursor_index);
extern void mmi_imc_update_candidate_as_null(void);
extern S32 mmi_imc_input_area_add_string(UI_string_type in);
extern MMI_BOOL mmi_imc_input_area_delete_string(S32 len);
extern MMI_BOOL mmi_imc_input_area_add_space(void);
extern void mmi_imc_reset_input_area(void);
extern void mmi_imc_reset_key_seq_data(void);
#endif

MMI_BOOL mmi_imc_get_csk_config(void);
extern void mmi_imc_config_draw(MMI_BOOL draw);
extern MMI_BOOL mmi_imc_get_draw(void);

#if defined(__MMI_IME_USER_DATABASE__)
extern void mmi_imc_update_word_frequency_for_string(UI_string_type str_ptr);
#endif

#if defined(__MMI_ALPHABETIC_WORD_PREDICTION__)
extern void  mmi_imc_get_associates_from_word_engine( mmi_ime_query_param_struct_p param_ptr, mmi_ime_query_result_struct_p result_ptr );
#endif

#endif /* !defined(__IMC_H__) */ 
