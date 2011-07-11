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
 *    Imm.h
 *
 * Project:
 * --------
 *   Maui_Software
 *
 * Description:
 * ------------
 *   header file for IMCtrl.c
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
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#include "MMI_features.h"


#ifndef __IMM_H__
#define __IMM_H__

#include "gui_data_types.h"
#include "ImeGprot.h"
#include "IMERes.h"
/*****************************************************************************
*  Enum
*****************************************************************************/

/*****************************************************************************
*  Define
*****************************************************************************/
#define   IMM_MAX_DISABLE_CHAR_NUM_BUF_LEN  32

typedef enum
{
    MMI_IME_HINT_FLAG_NONE = 0,
    MMI_IME_HINT_FLAG_INFO_BAR = 1,
    MMI_IME_HINT_FLAG_CSK = 2,
    MMI_IME_HINT_FLAG_VK_ICON = 4,
    MMI_IME_HINT_FLAG_MAX
}mmi_ime_hint_flag_enum;

typedef enum
{
    MMI_IME_HINT_TYPE_STRING = 0,
    MMI_IME_HINT_TYPE_IMAGE,
    MMI_IME_HINT_TYPE_MAX
}mmi_ime_hint_type_enum;


typedef struct
{
    U8 display_flag;
    U8 hint_type;
    U16 hint_id;
}mmi_imm_input_mode_hint_item_struct, *mmi_imm_input_mode_hint_item_struct_p;

typedef struct
{
    mmi_imm_input_mode_enum mode_id;
    mmi_imm_input_mode_hint_item_struct_p hint_item_ptr;
}mmi_imm_input_mode_info_struct, *mmi_imm_input_mode_info_struct_p;

typedef struct
{
    mmi_imm_support_writing_lang_enum lang_id;
    UI_character_type new_word[20];
}mmi_imm_new_word_string_mapping_struct, *mmi_imm_new_word_string_mapping_struct_p;


/*****************************************************************************
*  Global Functions
*****************************************************************************/

extern void mmi_imm_reset_allowed_and_disabled_list(void);
extern void mmi_imm_connect(void);

extern MMI_BOOL mmi_imm_get_allowed_or_disabled_characters(MMI_BOOL * is_allowed, UI_string_type characters_list);

extern MMI_BOOL mmi_imm_is_change_input_mode_enabled(void);

extern MMI_BOOL mmi_imm_test_r2l_input(void);

extern MMI_BOOL mmi_imm_set_current_input_mode(mmi_imm_input_mode_enum input_mode);

extern MMI_BOOL mmi_imm_test_reset_allowed_and_disabled_list(void);

extern void mmi_imm_set_allowed_and_disabled_list(MMI_BOOL value);

extern mmi_imm_input_mode_info_struct_p mmi_imm_get_curr_input_mode_hint_info(void);

#ifdef __MMI_TOUCH_SCREEN__
extern MMI_BOOL mmi_imm_get_allowed_or_disabled_events(MMI_BOOL * is_allowed, gui_virtual_keyboard_pen_enum * events_list);
#endif /* __MMI_TOUCH_SCREEN__ */

#if defined(__MMI_IME_USER_DATABASE__)
extern UI_string_type mmi_imm_get_new_word_string_by_language(mmi_imm_support_writing_lang_enum lang_id);
#endif


#if defined(__MMI_IMC_CAPITAL_SWITCH__)
extern S32 mmi_imm_get_current_allowed_writing_language_num(void);
extern mmi_imm_support_writing_lang_enum mmi_imm_get_current_writing_language(void);
extern void mmi_imm_set_current_writing_lang(mmi_imm_support_writing_lang_enum lang);
extern mmi_imm_support_writing_lang_enum mmi_imm_get_not_used_desired_writing_language_from_history(void);
extern S32 mmi_imm_get_input_mode_num_in_current_writing_language(void);
#endif

extern const sIMEModeDetails * mmi_imm_get_input_mode_array_item(mmi_imm_input_mode_enum input_mode);

extern const sIMEModeDetails * mmi_imm_get_corresponding_multitap_mode(mmi_imm_input_mode_enum input_mode);

extern const sIMEModeDetails * mmi_imm_get_corresponding_smart_lower_case_mode(mmi_imm_input_mode_enum input_mode);

#endif /* !defined(__IMM_H__) */ 
