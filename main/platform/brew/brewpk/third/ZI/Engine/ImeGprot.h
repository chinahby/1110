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
 *    ImeGprot.h
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

#ifndef __IMEGROT_H__
#define __IMEGROT_H__


typedef unsigned int U32;
typedef unsigned short U16;
typedef unsigned char U8;

typedef signed int S32;
typedef signed short S16;
typedef char S8;
typedef boolean MMI_BOOL;
typedef byte*				UI_image_type;
typedef byte*				UI_audio_type;//101205 audio resource Calvin add
typedef byte				UI_transparent_color_type;

#ifdef __UCS2_ENCODING
typedef	U16					UI_character_type;
#else
typedef U8					UI_character_type;
#endif

typedef UI_character_type*	UI_string_type;
typedef U8*					UI_buffer_type;
typedef unsigned short * PU16;
#define MMI_FALSE FALSE
#define MMI_TRUE TRUE


/*****************************************************************************
 * Enums
 *****************************************************************************/

typedef enum {
    MMI_IMC_MESSAGE_NONE             = 0x0,
    MMI_IMC_MESSAGE_INSERT_STRING,   
    MMI_IMC_MESSAGE_INSERT_CHAR,
    MMI_IMC_MESSAGE_INSERT_NEW_LINE,
    MMI_IMC_MESSAGE_INSERT_MULTITAP_CHAR,
    MMI_IMC_MESSAGE_DELETE_CHAR,
    MMI_IMC_MESSAGE_DELETE_STRING,
    MMI_IMC_MESSAGE_CLEAR_ALL,
    MMI_IMC_MESSAGE_CLEAR_HILIGHT,
    MMI_IMC_MESSAGE_MOVE_CURSOR,
    MMI_IMC_MESSAGE_HILIGHT_BFCURSOR,
    MMI_IMC_MESSAGE_GET_INPUT_BOX_RECT,
    MMI_IMC_MESSAGE_GET_HANDWRITING_RECT,
    MMI_IMC_MESSAGE_GET_HANDWRITING_STYLE,
    MMI_IMC_MESSAGE_RESET_MULTITAP_STATE,
    MMI_IMC_MESSAGE_RESIZE,
    MMI_IMC_MESSAGE_MOVE,
    MMI_IMC_MESSAGE_REDRAW,
    MMI_IMC_MESSAGE_UPDATE_SOFTKEY,
    MMI_IMC_MESSAGE_KEY_EVENT,
    MMI_IMC_MESSAGE_PEN_EVENT,
    MMI_IMC_MESSAGE_MODE_CHANGED,
    MMI_IMC_MESSAGE_CHANGE_TO_INITIAL,
    MMI_IMC_MESSAGE_GET_TEXT,
    MMI_IMC_MESSAGE_IS_EDITOR_EMPTY,
    MMI_IMC_MESSAGE_INSERT_SPELLING_DIRECTLY,
    MMI_IMC_MESSAGE_GET_PREVIOUS_CHAR,
    MMI_IMC_MESSAGE_GET_STRING_BEFORE_CANDIDATE,
    MMI_IMC_MESSAGE_HIDE_MULTITAP_CANDIDATE_BOX,
    MMI_IMC_MESSAGE_INSERT_CHAR_AT_DESIRED_POSITION,
    MMI_IMC_MESSAGE_DELETE_CHAR_AT_DESIRED_POSITION,
    MMI_IMC_MESSAGE_GET_CHAR_AT_DESIRED_POSITION,
    MMI_IMC_MESSAGE_CHECK_FLOAT,
    MMI_IMC_MESSAGE_SET_KEY,
    MMI_IMC_MESSAGE_SET_MULTITAP_STATE,
    MMI_IMC_MESSAGE_SET_MASK,
    MMI_IMC_MESSAGE_DECIAML_CHECK,
    MMI_IMC_MESSAGE_DECIMAL_INPUT,
    MMI_IMC_MESSAGE_INSERT_DECIMAL_SYMBOL,
    MMI_IMC_MESSAGE_INSERT_SPACE,
    MMI_IMC_MESSAGE_DRAW_INFORMATION_BAR,
    MMI_IMC_MESSAGE_IS_SOFTKEY_HIDE,
    MMI_IMC_MESSAGE_CHECK_CSK_ENABLE,
    MMI_IMC_MESSAGE_CHECK_INFO_BAR_ENABL,
    MMI_IMC_MESSAGE_CHECK_MULTITAP_STATE,
    MMI_IMC_MESSAGE_START_SMART_ALPHABETIC,
    MMI_IMC_MESSAGE_STOP_SMART_ALPHABETIC,
    MMI_IMC_MESSAGE_WCSS_CHECK,
	MMI_IMC_MESSAGE_IS_LAST_POSITION,
    MMI_IMC_MESSAGE_IS_OVERWRITE_MODE,
    MMI_IMC_MESSAGE_GET_TEXT_PTR_BEFORE_CANDIDATE,
	MMI_IMC_MESSGAE_IS_WRITING_LANG_SELECTION_SCREEN_DISABLED,
    MMI_IMC_MESSGAE_GET_EXT_INPUT_BOX_RECT,
    MMI_IMC_MESSAGE_IS_THERE_CURSOR,
    MMI_IMC_MESSAGE_INSERT_STRING_NO_DRAW,
	MMI_IMC_MESSAGE_REPLACE_HIGHLIGHT_STRING
}   mmi_imc_editor_message_id_enum;

/*
notification:   MMI_IMC_MESSAGE_INSERT_STRING
remark:         insert a character into editor
param_0:        [IN] (UI_string_type)    Ucs2 string pointer
param_1:        reserved
return:         reserved

notification:   MMI_IMC_MESSAGE_DELETE_CHAR
remark:         delete characters before cursor
param_0:        [IN] (U32)             string length
param_1:        reserved
return:         reserved

notification:   IMC_MESSAGE_MOVE_CURSOR
remark:         move cursor
param_0:        [IN] (U32)             start position
param_1:        [IN] (U32)             move step count,
                                        negative value indicates backward direction,
                                        positive value indicates forward direction
return:         reserved

notification:   IMC_MESSAGE_HILIGHT_BFCURSOR
remark:         highlight characters before cursor
param_0:        [IN] (U32)              highlight character count
param_1:        [IN] (MMI_BOOL)         whether move cursor at the same time
return:         reserved

notification:   IMC_MESSAGE_RESIZE
remark:         resize editor's height
param_0:        [IN] (U32)             width
param_1:        [IN] (U32)             height
return:         reserved

notification:   IMC_MESSAGE_REDRAW
remark:         redraw editor
param_0:        reserved
param_1:        reserved
return:         reserved

notification:   IMC_MESSAGE_UPDATE_SOFTKEY
remark:         change softkey caption and icon
param_0:        [IN] (U16)             High Part - string id
                [IN] (U16)             Low  Part - image id
param_1:        [IN] (WGUI_SOFTKEY_ENUM)    softkey type    (refference to wgui_softkeys.h)
return:         reserved

notification:   IMC_MESSAGE_KEY_EVENT
remark:         forwarding key event
param_0:        [IN] (U32)             key value
param_1:        [IN] (U32)             key type
return:         (MMI_BOOL)  indicates whether disposed?

notification:   MMI_IMC_MESSAGE_MODE_CHANGED
remark:         input mode changed
param_0:        input mode value
param_1:        input type value
return:         reserved

notification:   MMI_IMC_MESSAGE_INSERT_MULTITAP_CHAR
remark:         insert multitap character
param_0:       the candidate character
param_1:       NULL
return:         (MMI_BOOL)  indicates insertion success or fail

notification:  MMI_IMC_MESSAGE_IS_EDITOR_EMPTY
remark:         to check if editor is empty or not
return:         (MMI_BOOL)  MMI_TRUE: editor is empty else empty is not empty.

notification:   MMI_IMC_MESSAGE_GET_STRING_BEFORE_CANDIDATE
remake:         to get string with its length being n before the candidate.
param_0:       the desired character count.
param_1:       the buffer to put the character. the size of the buffer should be large then the desired count.
return:          

*/

typedef enum {
    MMI_IMC_EDITOR_PEN_EVENT_NONE,
    MMI_IMC_EDITOR_PEN_EVENT_TEXT_DOWN,
    MMI_IMC_EDITOR_PEN_EVENT_TEXT_UP,
    MMI_IMC_EDITOR_PEN_EVENT_SCROLL_BAR,
    MMI_IMC_EDITOR_PEN_EVENT_MAX
} mmi_imc_editor_pen_event_enum;

typedef enum
{
    MMI_IMC_EDITOR_CURSOR_UP,
    MMI_IMC_EDITOR_CURSOR_DOWN,
    MMI_IMC_EDITOR_CURSOR_LEFT,
    MMI_IMC_EDITOR_CURSOR_RIGHT
}mmi_imc_editor_cusor_move_direction_enum;

typedef enum
{
    IMM_INPUT_TYPE_NONE = 0,
    IMM_INPUT_TYPE_NUMERIC = 0x1,
    IMM_INPUT_TYPE_PHONE_NUMBER = (0x1 << 1),
    IMM_INPUT_TYPE_DECIMAL_NUMERIC = (0x1 << 2),
    IMM_INPUT_TYPE_SIGNED_NUMERIC = (0x1 << 3),
    IMM_INPUT_TYPE_SIGNED_DECIMAL_NUMERIC = (0x1 << 4),
    IMM_INPUT_TYPE_URL = (0x1 << 5), /* for url input */
    IMM_INPUT_TYPE_QUICK_SEARCH = (0x1 << 6), /* for quick search */
    IMM_INPUT_TYPE_SENTENCE = (0x1 << 7), /* all kind of character are allowed to input. */
    IMM_INPUT_TYPE_ENGLISH_SENTENCE = (0x1 << 8), /* only English character are allowed. */
    IMM_INPUT_TYPE_USER_SPECIFIC = (0x1 << 9), /* APP define the input method he want */
    IMM_INPUT_TYPE_KEYPAD_NUMERIC = (0x1 << 10),
    IMM_INPUT_TYPE_SIM_NUMERIC = (0x1 << 11),
    IMM_INPUT_TYPE_ENGLISH_SENTENCE_UPPERCASE = (0x1 << 12) ,
    IMM_INPUT_TYPE_ENGLISH_SENTENCE_LOWERCASE = (0x1 << 13) ,
    IMM_INPUT_TYPE_EMAIL = (0x01 << 14),
    IMM_INPUT_TYPE_PHONE_NUMBER_WILDCHAR = (0x01 << 15),
    IMM_INPUT_TYPE_NUMERIC_SYMBOL = (0x01 << 16),
    IMM_INPUT_TYPE_ASCII_CHAR = (0x01 << 17),
    IMM_INPUT_TYPE_MAX
} mmi_imm_input_type_enum;

#define    IMM_INPUT_TYPE_MASK                                   0xf00000 
#define    IMM_INPUT_TYPE_EXT_PASSWORD                    0x800000
#define    IMM_INPUT_TYPE_OVERRIDE_DEFAULT              0x400000
#define    IMM_INPUT_TYPE_EXT_MULTITAP                      0x200000
#define    IMM_INPUT_TYPE_ADD_NEW_WORD                      0x100000
#define    IMM_INPUT_TYPE_SAT_NUMERIC_PASSWORD      (IMM_INPUT_TYPE_KEYPAD_NUMERIC |IMM_INPUT_TYPE_EXT_PASSWORD)
#define    IMM_INPUT_TYPE_DECIMAL_NUMERIC_PASSWORD  (IMM_INPUT_TYPE_DECIMAL_NUMERIC | IMM_INPUT_TYPE_EXT_PASSWORD)
#define    IMM_INPUT_TYPE_SIGNED_NUMERIC_PASSWORD   (IMM_INPUT_TYPE_SIGNED_NUMERIC | IMM_INPUT_TYPE_EXT_PASSWORD)
#define    IMM_INPUT_TYPE_SIGNED_DECIMAL_NUMERIC_PASSWORD (IMM_INPUT_TYPE_SIGNED_DECIMAL_NUMERIC | IMM_INPUT_TYPE_EXT_PASSWORD)
#define    IMM_INPUT_TYPE_NUMERIC_PASSWORD                (IMM_INPUT_TYPE_NUMERIC | IMM_INPUT_TYPE_EXT_PASSWORD)
#define    IMM_INPUT_TYPE_ALPHANUMERIC_PASSWORD       (IMM_INPUT_TYPE_ENGLISH_SENTENCE | IMM_INPUT_TYPE_EXT_PASSWORD)
#define    IMM_INPUT_TYPE_MULTITAP_SENTENCE             (IMM_INPUT_TYPE_EXT_MULTITAP | IMM_INPUT_TYPE_SENTENCE)

#define MMI_IMM_MAX_INPUT_METHOD_NUM  100

typedef enum
{
    IMM_INPUT_MODE_NONE = 0,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_ABC,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_ABC,
    IMM_INPUT_MODE_123,
    IMM_INPUT_MODE_SIGNED_123,
    IMM_INPUT_MODE_FLOAT_123,
    IMM_INPUT_MODE_PHONE_NUMBER,
    IMM_INPUT_MODE_PHONE_NUMBER_WILDCHAR,
    IMM_INPUT_MODE_SIM_NUMERIC,
    IMM_INPUT_MODE_SIGNED_FLOAT_123,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_ABC_NO_NUMERIC,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_ABC_NO_NUMERIC,
    IMM_INPUT_MODE_123_SYMBOLS,
    IMM_INPUT_MODE_TR_MULTITAP_BOPOMO,
    IMM_INPUT_MODE_SM_MULTITAP_PINYIN,
    IMM_INPUT_MODE_TR_BOPOMO,
    IMM_INPUT_MODE_SM_PINYIN,
    IMM_INPUT_MODE_TR_STROKE,
    IMM_INPUT_MODE_SM_STROKE,
    IMM_INPUT_MODE_SMART_UPPERCASE_ABC,
    IMM_INPUT_MODE_SMART_LOWERCASE_ABC,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_ABC,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_ABC,
    IMM_INPUT_MODE_MULTITAP_THAI,
    IMM_INPUT_MODE_SMART_THAI,
    IMM_INPUT_MODE_MULTITAP_ARABIC,
    IMM_INPUT_MODE_SMART_ARABIC,
    IMM_INPUT_MODE_ARABIC_NUMERIC,
    IMM_INPUT_MODE_MULTITAP_PERSIAN,
    IMM_INPUT_MODE_SMART_PERSIAN,
    IMM_INPUT_MODE_MULTITAP_URDU,
    IMM_INPUT_MODE_SMART_URDU,
    IMM_INPUT_MODE_MULTITAP_HEBREW,
    IMM_INPUT_MODE_SMART_HEBREW,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_SPANISH,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_SPANISH,
    IMM_INPUT_MODE_SMART_UPPERCASE_SPANISH,
    IMM_INPUT_MODE_SMART_LOWERCASE_SPANISH,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_SPANISH,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_SPANISH,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_DANISH,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_DANISH,
    IMM_INPUT_MODE_SMART_UPPERCASE_DANISH,
    IMM_INPUT_MODE_SMART_LOWERCASE_DANISH,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_DANISH,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_DANISH,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_POLISH,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_POLISH,
    IMM_INPUT_MODE_SMART_UPPERCASE_POLISH,
    IMM_INPUT_MODE_SMART_LOWERCASE_POLISH,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_POLISH,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_POLISH,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_FRENCH,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_FRENCH,
    IMM_INPUT_MODE_SMART_UPPERCASE_FRENCH,
    IMM_INPUT_MODE_SMART_LOWERCASE_FRENCH,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_FRENCH,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_FRENCH,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_GERMAN,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_GERMAN,
    IMM_INPUT_MODE_SMART_UPPERCASE_GERMAN,
    IMM_INPUT_MODE_SMART_LOWERCASE_GERMAN,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_GERMAN,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_GERMAN,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_ITALIAN,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_ITALIAN,
    IMM_INPUT_MODE_SMART_UPPERCASE_ITALIAN,
    IMM_INPUT_MODE_SMART_LOWERCASE_ITALIAN,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_ITALIAN,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_ITALIAN,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_RUSSIAN,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_RUSSIAN,
    IMM_INPUT_MODE_SMART_UPPERCASE_RUSSIAN,
    IMM_INPUT_MODE_SMART_LOWERCASE_RUSSIAN,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_RUSSIAN,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_RUSSIAN,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_BULGARIAN,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_BULGARIAN,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_UKRAINIAN,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_UKRAINIAN,
    IMM_INPUT_MODE_SMART_UPPERCASE_UKRAINIAN,
    IMM_INPUT_MODE_SMART_LOWERCASE_UKRAINIAN,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_UKRAINIAN,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_UKRAINIAN,
    IMM_INPUT_MODE_SMART_UPPERCASE_BULGARIAN,
    IMM_INPUT_MODE_SMART_LOWERCASE_BULGARIAN,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_BULGARIAN,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_BULGARIAN,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_TURKISH,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_TURKISH,
    IMM_INPUT_MODE_SMART_UPPERCASE_TURKISH,
    IMM_INPUT_MODE_SMART_LOWERCASE_TURKISH,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_TURKISH,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_TURKISH,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_PORTUGUESE,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_PORTUGUESE,
    IMM_INPUT_MODE_SMART_UPPERCASE_PORTUGUESE,
    IMM_INPUT_MODE_SMART_LOWERCASE_PORTUGUESE,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_PORTUGUESE,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_PORTUGUESE,
    IMM_INPUT_MODE_MULTITAP_HINDI,    
    IMM_INPUT_MODE_SMART_HINDI,
    IMM_INPUT_MODE_SMART_UPPERCASE_HINGLISH,
    IMM_INPUT_MODE_SMART_LOWERCASE_HINGLISH,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_INDONESIAN,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_INDONESIAN,
    IMM_INPUT_MODE_SMART_UPPERCASE_INDONESIAN,
    IMM_INPUT_MODE_SMART_LOWERCASE_INDONESIAN,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_INDONESIAN,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_INDONESIAN,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_CZECH,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_CZECH,
    IMM_INPUT_MODE_SMART_UPPERCASE_CZECH,
    IMM_INPUT_MODE_SMART_LOWERCASE_CZECH,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_CZECH,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_CZECH,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_MALAY,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_MALAY,
    IMM_INPUT_MODE_SMART_UPPERCASE_MALAY,
    IMM_INPUT_MODE_SMART_LOWERCASE_MALAY,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_MALAY,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_MALAY,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_VIETNAMESE,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_VIETNAMESE,
    IMM_INPUT_MODE_SMART_UPPERCASE_VIETNAMESE,
    IMM_INPUT_MODE_SMART_LOWERCASE_VIETNAMESE,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_VIETNAMESE,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_VIETNAMESE,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_FINNISH,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_FINNISH,
    IMM_INPUT_MODE_SMART_UPPERCASE_FINNISH,
    IMM_INPUT_MODE_SMART_LOWERCASE_FINNISH,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_FINNISH,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_FINNISH,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_HUNGARIAN,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_HUNGARIAN,
    IMM_INPUT_MODE_SMART_UPPERCASE_HUNGARIAN,
    IMM_INPUT_MODE_SMART_LOWERCASE_HUNGARIAN,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_HUNGARIAN,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_HUNGARIAN,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_SLOVAK,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_SLOVAK,
    IMM_INPUT_MODE_SMART_UPPERCASE_SLOVAK,
    IMM_INPUT_MODE_SMART_LOWERCASE_SLOVAK,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_SLOVAK,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_SLOVAK,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_DUTCH,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_DUTCH,
    IMM_INPUT_MODE_SMART_UPPERCASE_DUTCH,
    IMM_INPUT_MODE_SMART_LOWERCASE_DUTCH,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_DUTCH,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_DUTCH,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_NORWEGIAN,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_NORWEGIAN,
    IMM_INPUT_MODE_SMART_UPPERCASE_NORWEGIAN,
    IMM_INPUT_MODE_SMART_LOWERCASE_NORWEGIAN,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_NORWEGIAN,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_NORWEGIAN,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_SWEDISH,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_SWEDISH,
    IMM_INPUT_MODE_SMART_UPPERCASE_SWEDISH,
    IMM_INPUT_MODE_SMART_LOWERCASE_SWEDISH,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_SWEDISH,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_SWEDISH,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_CROATIAN,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_CROATIAN,
    IMM_INPUT_MODE_SMART_UPPERCASE_CROATIAN,
    IMM_INPUT_MODE_SMART_LOWERCASE_CROATIAN,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_CROATIAN,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_CROATIAN,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_ROMANIAN,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_ROMANIAN,
    IMM_INPUT_MODE_SMART_UPPERCASE_ROMANIAN,
    IMM_INPUT_MODE_SMART_LOWERCASE_ROMANIAN,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_ROMANIAN,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_ROMANIAN,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_SLOVENIAN,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_SLOVENIAN,
    IMM_INPUT_MODE_SMART_UPPERCASE_SLOVENIAN,
    IMM_INPUT_MODE_SMART_LOWERCASE_SLOVENIAN,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_SLOVENIAN,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_SLOVENIAN,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_GREEK,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_GREEK,
    IMM_INPUT_MODE_SMART_UPPERCASE_GREEK,
    IMM_INPUT_MODE_SMART_LOWERCASE_GREEK,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_GREEK,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_GREEK,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_XHOSA,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_XHOSA,
    IMM_INPUT_MODE_SMART_UPPERCASE_XHOSA,
    IMM_INPUT_MODE_SMART_LOWERCASE_XHOSA,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_XHOSA,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_XHOSA,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_SWAHILI,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_SWAHILI,
    IMM_INPUT_MODE_SMART_UPPERCASE_SWAHILI,
    IMM_INPUT_MODE_SMART_LOWERCASE_SWAHILI,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_SWAHILI,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_SWAHILI,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_AFRIKAANS,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_AFRIKAANS,
    IMM_INPUT_MODE_SMART_UPPERCASE_AFRIKAANS,
    IMM_INPUT_MODE_SMART_LOWERCASE_AFRIKAANS,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_AFRIKAANS,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_AFRIKAANS,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_ZULU,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_ZULU,
    IMM_INPUT_MODE_SMART_UPPERCASE_ZULU,
    IMM_INPUT_MODE_SMART_LOWERCASE_ZULU,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_ZULU,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_ZULU,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_LITHUANIAN,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_LITHUANIAN,
    IMM_INPUT_MODE_SMART_UPPERCASE_LITHUANIAN,
    IMM_INPUT_MODE_SMART_LOWERCASE_LITHUANIAN,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_LITHUANIAN,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_LITHUANIAN,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_LATVIAN,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_LATVIAN,
    IMM_INPUT_MODE_SMART_UPPERCASE_LATVIAN,
    IMM_INPUT_MODE_SMART_LOWERCASE_LATVIAN,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_LATVIAN,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_LATVIAN,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_ESTONIAN,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_ESTONIAN,
    IMM_INPUT_MODE_SMART_UPPERCASE_ESTONIAN,
    IMM_INPUT_MODE_SMART_LOWERCASE_ESTONIAN,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_ESTONIAN,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_ESTONIAN,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_ARMENIAN,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_ARMENIAN,
    IMM_INPUT_MODE_SMART_UPPERCASE_ARMENIAN,
    IMM_INPUT_MODE_SMART_LOWERCASE_ARMENIAN,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_ARMENIAN,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_ARMENIAN,
    IMM_INPUT_MODE_MULTITAP_GEORGIAN,
    IMM_INPUT_MODE_SMART_GEORGIAN,
    IMM_INPUT_MODE_MULTITAP_UPPERCASE_MOLDOVAN,
    IMM_INPUT_MODE_MULTITAP_LOWERCASE_MOLDOVAN,
    IMM_INPUT_MODE_SMART_UPPERCASE_MOLDOVAN,
    IMM_INPUT_MODE_SMART_LOWERCASE_MOLDOVAN,
    IMM_INPUT_MODE_SMART_FIRST_UPPERCASE_MOLDOVAN,
    IMM_INPUT_MODE_MULTITAP_FIRST_UPPERCASE_MOLDOVAN,
    IMM_INPUT_MODE_MULTITAP_MARATHI,
    IMM_INPUT_MODE_SMART_MARATHI,
    IMM_INPUT_MODE_MULTITAP_TAMIL,
    IMM_INPUT_MODE_SMART_TAMIL, 
    IMM_INPUT_MODE_MULTITAP_BENGALI,
    IMM_INPUT_MODE_SMART_BENGALI,  
    IMM_INPUT_MODE_MULTITAP_PUNJABI,
    IMM_INPUT_MODE_MMI_MULTITAP_PUNJABI,
    IMM_INPUT_MODE_SMART_PUNJABI, 
    IMM_INPUT_MODE_MULTITAP_TELUGU,
    IMM_INPUT_MODE_SMART_TELUGU,
    IMM_INPUT_MODE_MULTITAP_KANNADA,
    IMM_INPUT_MODE_MMI_MULTITAP_KANNADA,
    IMM_INPUT_MODE_SMART_KANNADA,
    IMM_INPUT_MODE_MULTITAP_GUJARATI,
    IMM_INPUT_MODE_MMI_MULTITAP_GUJARATI,
    IMM_INPUT_MODE_SMART_GUJARATI, 
    IMM_INPUT_MODE_MULTITAP_MALAYALAM,
    IMM_INPUT_MODE_MMI_MULTITAP_MALAYALAM,
    IMM_INPUT_MODE_SMART_MALAYALAM,         
    IMM_INPUT_MODE_MULTITAP_ORIYA,
    IMM_INPUT_MODE_MMI_MULTITAP_ORIYA,
    IMM_INPUT_MODE_SMART_ORIYA, 
    IMM_INPUT_MODE_QUICK_SEARCH_BOPOMO,
    IMM_INPUT_MODE_QUICK_SEARCH_PINYIN,
    IMM_INPUT_MODE_KEYPAD_NUMERIC,
    IMM_INPUT_MODE_QUICK_SEARCH_SM_STROKE,
    IMM_INPUT_MODE_QUICK_SEARCH_TR_STROKE,
    IMM_INPUT_MODE_MAGIC_NUMBER,
    IMM_INPUT_MODE_MAX_NUM
} mmi_imm_input_mode_enum;

typedef enum {
    IME_WRITING_LANG_NONE,
    IME_WRITING_LANG_ABC, /* English */
    IME_WRITING_LANG_FR, /* French */
    IME_WRITING_LANG_ES, /* SPANISH */
    IME_WRITING_LANG_DA, /* DANISH */
    IME_WRITING_LANG_PL, /* POLISH */
    IME_WRITING_LANG_PT, /* PORTUGUESE */
    IME_WRITING_LANG_IT, /* ITALIAN */
    IME_WRITING_LANG_DE, /* GERMAN */
    IME_WRITING_LANG_MS, /* MALAY */
    IME_WRITING_LANG_ID, /* INDONESIAN */
    IME_WRITING_LANG_CS, /* CZECH */
    IME_WRITING_LANG_NO, /* NORWEGIAN */
    IME_WRITING_LANG_SK, /*SLOVAK  */
    IME_WRITING_LANG_NL, /*DUTCH  */
    IME_WRITING_LANG_FI, /* FINNISH */
    IME_WRITING_LANG_HU, /* HUNGARIAN */
    IME_WRITING_LANG_VI, /* VIETNAMESE */
    IME_WRITING_LANG_TR, /* TURKISH */
    IME_WRITING_LANG_RU, /* RUSSIAN */
    IME_WRITING_LANG_BG, /* BULGARIAN */
    IME_WRITING_LANG_UK, /* UKRAINIAN */
    IME_WRITING_LANG_AR, /* ARABIC */
    IME_WRITING_LANG_PS, /* PERSIAN */
    IME_WRITING_LANG_HE, /* HEBREW */
    IME_WRITING_LANG_TH, /* Thai */
    IME_WRITING_LANG_EL, /* GREEK */
    IME_WRITING_LANG_ZHTW, /* Traditional Chinese */
    IME_WRITING_LANG_ZHCN, /* Simpled Chinese */
    IME_WRITING_LANG_SV, /* SWEDISH */
    IME_WRITING_LANG_SH, /* CROATIAN */
    IME_WRITING_LANG_RO, /* ROMANIAN */
    IME_WRITING_LANG_SL, /* SLOVENIAN */
    IME_WRITING_LANG_HI, /* HINDI */
    IME_WRITING_LANG_MR, /* MARATHI */
    IME_WRITING_LANG_UR, /* URDU */
    IME_WRITING_LANG_TA, /* Tamil */
    IME_WRITING_LANG_BE, /* Bengali */
    IME_WRITING_LANG_PA, /* Punjabi */
    IME_WRITING_LANG_TE, /* Telugu */
    IME_WRITING_LANG_KN, /* Kannada */
    IME_WRITING_LANG_GU, /* Gujarati */
    IME_WRITING_LANG_ML, /* malayalam */
		IME_WRITING_LANG_OR, /* oriya */
    IME_WRITING_LANG_XH, /* Xhosa */
    IME_WRITING_LANG_SW, /* Swahili */
    IME_WRITING_LANG_AF, /* Afrilaans */
    IME_WRITING_LANG_ZU, /* Zulu */
    IME_WRITING_LANG_LT, /* Lithuanian */
    IME_WRITING_LANG_LV, /* Latvian */
    IME_WRITING_LANG_ET, /* Estonian */
    IME_WRITING_LANG_HY, /* Armenian */
    IME_WRITING_LANG_KA, /* Georgian */
    IME_WRITING_LANG_MO, /* Moldovan */
    IME_WRITING_LANG_123,
    IME_WRITING_LANG_TOTAL
}   mmi_imm_support_writing_lang_enum;

typedef enum
{
    IMM_HANDWRITING_TYPE_NONE = 0,
    IMM_HANDWRITING_TYPE_TRADITIONAL_CHINESE,
    IMM_HANDWRITING_TYPE_SIMPLIFIED_CHINESE,
    IMM_HANDWRITING_TYPE_UPPER_CASE_ENGLISH,
    IMM_HANDWRITING_TYPE_LOWER_CASE_ENGLISH,
    IMM_HANDWRITING_TYPE_UPPER_CASE_RUSSIAN,
    IMM_HANDWRITING_TYPE_LOWER_CASE_RUSSIAN,
    IMM_HANDWRITING_TYPE_THAI,
    IMM_HANDWRITING_TYPE_NUMBER,
    IMM_HANDWRITING_TYPE_PUNCTUATION
} mmi_imc_pen_handwriting_type_enum;


typedef enum
{
    IMM_MESSAGE_ID_NONE,
    IMM_MESSAGE_ID_ERROR,
    IMM_MESSAGE_ID_MAX
}mmi_imm_message_id_enum;

typedef enum
{
    MMI_IMC_PEN_HANDWRITING_STYLE_FULL_SCREEN,
    MMI_IMC_PEN_HANDWRITING_STYLE_MULTI_BLOCK
} mmi_imc_pen_handwriting_style_enum;

/*****************************************************************************
 * Typedef
 *****************************************************************************/
typedef void (*FuncPtr) (void);

typedef struct 
{
    U32 param_0;
    U32 param_1;
    U8  message_id;
}   mmi_imc_message_struct, *mmi_imc_message_struct_p;

#ifdef __MMI_TOUCH_SCREEN__
typedef struct
{
    mmi_pen_event_type_enum   mmi_pen_event;
    mmi_pen_point_struct      pos;
    mmi_imc_editor_pen_event_enum    editor_pen_event;
}mmi_imc_editor_pen_event_struct, *mmi_imc_editor_pen_event_struct_p;
#endif /* __MMI_TOUCH_SCREEN__ */

typedef U32 (*mmi_imc_message_funcptr)( void *, mmi_imc_message_struct_p );
typedef void (*mmi_imui_draw_back_ground_funcptr)(S32 x1, S32 y1, S32 x2, S32 y2);

typedef struct
{
    S32 x;
    S32 y;
    S32 width;
    S32 height;
} mmi_imc_rect, *mmi_imc_rect_p;

typedef struct
{
    U32 state;
    MMI_BOOL is_multitap_state;
#ifdef __MMI_TOUCH_SCREEN__
    gui_virtual_keyboard_language_enum vk_type;
#endif

#if defined(__MMI_IMC_CAPITAL_SWITCH__)
    mmi_imm_support_writing_lang_enum writing_lang;
#endif
} mmi_imc_history, *mmi_imc_history_p;

typedef MMI_BOOL (*mmi_imc_pre_key_handler)(U16, U16);

/*****************************************************************************
*  Global Functions
*****************************************************************************/

/* imc functions */
extern MMI_BOOL mmi_imc_init_all( void );
extern void     mmi_imc_deinit_all( void );

extern MMI_BOOL mmi_imc_connect( void *handle, mmi_imc_message_funcptr msg_callback );
extern void     mmi_imc_disconnect( void );
extern MMI_BOOL mmi_imc_is_connected( void );
extern U32      mmi_imc_send_message( mmi_imc_message_struct_p msg_ptr ); /* Send message to editor */
extern void     mmi_imc_draw_input_mode_infor(S32 x1, S32 y1, S32 x2, S32 y2);
extern void     mmi_imc_redraw_screen_by_state(void);
extern void     mmi_imc_disable_key_input(void);
extern void     mmi_imc_disable_symbol_picker(void);
extern void     mmi_imc_enable_symbol_picker(void);
extern MMI_BOOL mmi_imc_get_char_info(UI_character_type code, UI_string_type info_buffer, S32 buffer_size, mmi_imm_input_mode_enum mode);
extern MMI_BOOL mmi_imc_is_tone_character(UI_character_type code);
extern MMI_BOOL mmi_imc_is_ime_state(void);
extern void     mmi_imc_show_disabled_vk(void);
extern void     mmi_imc_set_fixed_input_box(void);
extern void     mmi_imc_key_setup_lsk_text(void);
extern void     mmi_imc_key_setup_rsk_text(void);
extern void     mmi_imc_key_set_pre_key_handler(mmi_imc_pre_key_handler key_handler);

extern MMI_BOOL mmi_imc_insert_char_by_symbol_picker(UI_character_type symbol);

/* This function can only be called by editor's get history function */
extern void     mmi_imc_get_input_method_history(mmi_imc_history_p history);

extern void     mmi_imc_key_setup_lsk_function(FuncPtr f);
extern void     mmi_imc_key_setup_rsk_function(FuncPtr f);
extern void     mmi_imc_key_register_ime_keys(void);
extern void     mmi_imc_exit_screen(void);

extern void     mmi_imc_scan_and_add_new_words(UI_string_type str);

extern U32      mmi_imc_get_max_candidate_width(void);

#ifdef __MMI_TOUCH_SCREEN__
extern void     mmi_imc_disable_handwriting(void);
extern S32      mmi_imc_get_vk_area_height(void);
extern gui_virtual_keyboard_language_enum mmi_imc_get_current_vk_type(void);
extern void mmi_imc_set_desired_vk_type(gui_virtual_keyboard_language_enum vk_type);
extern void  mmi_imc_set_inline_editor_full_height(S32 height);
extern S32  mmi_imc_get_inline_editor_full_height(void);
#endif /* __MMI_TOUCH_SCREEN__ */

/* imm functions */
extern mmi_imm_message_id_enum mmi_imm_set_required_mode_list(U32 input_type,
                                                              mmi_imm_input_mode_enum input_mode_array[],
                                                              mmi_imm_input_mode_enum history_input_mode);

extern mmi_imm_message_id_enum mmi_imm_create_input_method_list(U32 input_type, mmi_imm_input_mode_enum input_mode_array[]);

extern mmi_imm_input_mode_enum mmi_imm_get_next_mode(void);

extern mmi_imm_input_mode_enum mmi_imm_get_curr_input_mode(void);

extern mmi_imm_input_type_enum mmi_imm_get_curr_input_type(void);

extern MMI_BOOL mmi_imm_is_curr_allow_input_mode(mmi_imm_input_mode_enum target);

extern S32  mmi_imm_is_system_supported_input_mode(mmi_imm_input_mode_enum target);

extern S32 mmi_imm_get_system_support_input_mode_list(mmi_imm_input_mode_enum *mode_array, S32 max);

extern S32 mmi_imm_get_curr_allow_input_mode_list(mmi_imm_input_mode_enum *mode_array, S32 max);

extern S32 mmi_imm_get_system_support_input_mode_string_list(U8 **strarray, S32 max);

extern S32 mmi_imm_get_curr_allow_input_mode_string_list(U16 *strarray, S32 max);

extern mmi_imm_input_type_enum mmi_imm_get_user_desired_input_mode(void);

extern MMI_BOOL mmi_imm_check_mixed_lang_support(mmi_imm_input_mode_enum target);

extern MMI_BOOL mmi_imm_is_smart_input_method(mmi_imm_input_mode_enum target);

extern MMI_BOOL mmi_imm_is_chinese_input_method(mmi_imm_input_mode_enum target);

extern mmi_imm_message_id_enum mmi_imm_set_prefer_input_mode(mmi_imm_input_mode_enum desired_input_mode);

extern mmi_imm_input_mode_enum mmi_imm_get_prefer_input_mode(void);

extern mmi_imm_message_id_enum mmi_imm_set_user_desired_input_mode(mmi_imm_input_mode_enum desired_input_mode);

extern mmi_imm_message_id_enum mmi_imm_set_app_desired_input_mode(mmi_imm_input_mode_enum desired_input_mode);

extern mmi_imm_message_id_enum mmi_imm_set_characters(
        MMI_BOOL is_allowed,
        const UI_character_type *characters_lsit);

extern mmi_imm_message_id_enum mmi_imm_set_default_input_mode(mmi_imm_input_mode_enum default_input_mode);

extern S32 mmi_imm_get_current_allowed_input_mode_num(void);

extern MMI_BOOL mmi_imm_change_input_mode_for_wcss(mmi_imm_input_mode_enum mode);

extern void mmi_imm_config_change_input_mode(MMI_BOOL is_enabled);

#if defined(__MMI_IMC_CAPITAL_SWITCH__)

extern S32 mmi_imm_get_curr_allow_writing_language_string_list(UI_string_type *strarray, S32 max);

extern U16 mmi_imm_get_curr_writing_language_index(void);

extern void mmi_imm_set_user_desired_writing_language(S32 index);

extern void mmi_imm_set_writing_language_from_history(mmi_imm_support_writing_lang_enum writing_lang);

#endif

#ifdef __MMI_TOUCH_SCREEN__
extern mmi_imm_message_id_enum mmi_imm_set_events(
        MMI_BOOL is_allowed,
        const gui_virtual_keyboard_pen_enum *disable_event_keys_list);

#endif /* __MMI_TOUCH_SCREEN__ */

extern MMI_BOOL mmi_imm_test_input(UI_string_type cand, S32 cand_len);

#if defined(__MMI_PREFER_WRITING_LANG__) || defined(__MMI_MIXED_LANG_SUPPORT__)
extern void mmi_imm_set_main_lang(mmi_imm_support_writing_lang_enum main_lang);
extern mmi_imm_support_writing_lang_enum mmi_imm_get_main_lang(void);
#endif

#if defined(__MMI_MIXED_LANG_SUPPORT__)
extern void mmi_imm_set_sub_lang(mmi_imm_support_writing_lang_enum sub_lang);
extern mmi_imm_support_writing_lang_enum mmi_imm_get_sub_lang(void);

#endif

extern MMI_BOOL mmi_imc_check_multitap_state(void);
extern void mmi_imc_restore_multitap_state(MMI_BOOL value);
extern void mmi_imc_disable_csk(void);

/* Multi-tap engine functions */
extern U16 mmi_ime_multitap_get_corresponding_key_code(mmi_imm_input_mode_enum input_mode, UI_character_type char_code);

extern U32 mmi_ime_multitap_get_alphabetic_keymap_table_size(void);

extern MMI_BOOL mmi_ime_multitap_create_alphabetic_keymap_table(U16 * buffer_ptr, U32 buffer_size);

extern U16 * mmi_ime_multitap_get_keymap(U16 * keymap_table, U16 key_code, U32 buffer_size);
extern void mmi_imc_set_backgroud_filler(mmi_imui_draw_back_ground_funcptr funcptr);

#endif /* __IMEGROT_H__ */
