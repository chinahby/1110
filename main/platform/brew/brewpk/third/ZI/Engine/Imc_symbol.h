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
 *    Imc_symbol.h
 *
 * Project:
 * --------
 *   Maui_Software
 *
 * Description:
 * ------------
 *   header file for IME
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
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#include "OEMFeatures.h"

#ifndef __IMC_SYMBOL_H__
#define __IMC_SYMBOL_H__

/*****************************************************************************
 * Typedef
 *****************************************************************************/
typedef enum {
    MMI_IMC_KEY_BASE    = 0x0,
    MMI_IMC_KEY_SYM_0       = MMI_IMC_KEY_BASE + 0,
    MMI_IMC_KEY_SYM_1,
    MMI_IMC_KEY_SYM_2,
    MMI_IMC_KEY_SYM_3,
    MMI_IMC_KEY_SYM_4,
    MMI_IMC_KEY_SYM_5,
    MMI_IMC_KEY_SYM_6,
    MMI_IMC_KEY_SYM_7,
    MMI_IMC_KEY_SYM_8,
    MMI_IMC_KEY_SYM_9,
    MMI_IMC_KEY_SYM_STAR, /* Star */
    MMI_IMC_KEY_SYM_POUND, /* Pound */
    MMI_IMC_KEY_UP,
    MMI_IMC_KEY_DOWN,
    MMI_IMC_KEY_LEFT,
    MMI_IMC_KEY_RIGHT,
    MMI_IMC_KEY_LSK,
    MMI_IMC_KEY_RSK,
    MMI_IMC_KEY_CONFIRM, /* Confirm */
    MMI_IMC_KEY_CANCEL, /* Cancel */
    MMI_IMC_KEY_CLEAR, /* Clear */
    MMI_IMC_KEY_CHANGE_MODE,
    MMI_IMC_KEY_CHANGE_LANGUAGE,
    MMI_IMC_KEY_SYMBOL_PICKER,
    MMI_IMC_KEY_CAPTITAL_SWITCH,
    MMI_IMC_KEY_CLEAN,
    MMI_IMC_KEY_CHAR_0,
    MMI_IMC_KEY_CHAR_1,
    MMI_IMC_KEY_CHAR_2,
    MMI_IMC_KEY_CHAR_3,
    MMI_IMC_KEY_CHAR_4,
    MMI_IMC_KEY_CHAR_5,
    MMI_IMC_KEY_CHAR_6,
    MMI_IMC_KEY_CHAR_7,
    MMI_IMC_KEY_CHAR_8,
    MMI_IMC_KEY_CHAR_9,
    MMI_IMC_KEY_SPACE,
    MMI_IMC_KEY_COMPOSITION_SELECT, /* for cstar pinyin style, it is used to change composition index */
    MMI_IMC_KEY_TONE_CHANGE,
    MMI_IMC_KEY_MAX,
    MMI_IMC_KEY_INVALID
}   mmi_imc_key_custom_value;


typedef enum {
    MMI_IMC_SYMB_TONE_BASE = 1,
    MMI_IMC_SYMB_TONE_NONE = MMI_IMC_SYMB_TONE_BASE + 0,
    MMI_IMC_SYMB_TONE_1,
    MMI_IMC_SYMB_TONE_2,
    MMI_IMC_SYMB_TONE_3,
    MMI_IMC_SYMB_TONE_4,
    MMI_IMC_SYMB_TONE_5,
    MMI_IMC_SYMB_TONE_MAX
}   mmi_imc_symb_tone_enum;

#if defined(__MMI_LANG_SM_CHINESE__) || defined(__MMI_LANG_TR_CHINESE__)

typedef enum {
    MMI_IMC_SYMB_STROKE_BASE        = 20,
    MMI_IMC_SYMB_STROKE_WILDCARD    = MMI_IMC_SYMB_STROKE_BASE + 0,
    MMI_IMC_SYMB_STROKE_DOWN,
    MMI_IMC_SYMB_STROKE_DOT,
    MMI_IMC_SYMB_STROKE_CURVED_HOOK,
    MMI_IMC_SYMB_STROKE_OVER,
    MMI_IMC_SYMB_STROKE_OVER_DOWN,
    MMI_IMC_SYMB_STROKE_DOWN_OVER,
    MMI_IMC_SYMB_STROKE_LEFT,
    MMI_IMC_SYMB_STROKE_OVER_DOWN_OVER,
    MMI_IMC_SYMB_STROKE_MORE,
    MMI_IMC_SYMB_STROKE_TURN,
    MMI_IMC_SYMB_STROKE_DOT_OR_LEFT,
    MMI_IMC_SYMB_STROKE_MAX
}   mmi_imc_symb_stroke_enum;

#endif /* defined(__MMI_LANG_SM_CHINESE__) || defined(__MMI_LANG_TR_CHINESE__) */

typedef enum {
    MMI_IMC_SYMB_KEY_BASE   = 40,
    MMI_IMC_SYMB_KEY_0      = MMI_IMC_SYMB_KEY_BASE + 0,
    MMI_IMC_SYMB_KEY_1,
    MMI_IMC_SYMB_KEY_2,
    MMI_IMC_SYMB_KEY_3,
    MMI_IMC_SYMB_KEY_4,
    MMI_IMC_SYMB_KEY_5,
    MMI_IMC_SYMB_KEY_6,
    MMI_IMC_SYMB_KEY_7,
    MMI_IMC_SYMB_KEY_8,
    MMI_IMC_SYMB_KEY_9,
    MMI_IMC_SYMB_KEY_STAR,
    MMI_IMC_SYMB_KEY_POUND,
    MMI_IMC_SYMB_KEY_MAX,
    MMI_IMC_SYMB_KEY_INVALID
}   mmi_imc_symbol_key_enum;

#if defined(__MMI_LANG_SM_CHINESE__)
typedef enum {
    MMI_IMC_SYMB_PY_BASE    = 60,
    MMI_IMC_SYMB_PY_A       = MMI_IMC_SYMB_PY_BASE + 0,
    MMI_IMC_SYMB_PY_B,
    MMI_IMC_SYMB_PY_C,

    MMI_IMC_SYMB_PY_D,
    MMI_IMC_SYMB_PY_E,
    MMI_IMC_SYMB_PY_F,

    MMI_IMC_SYMB_PY_G,
    MMI_IMC_SYMB_PY_H,
    MMI_IMC_SYMB_PY_I,

    MMI_IMC_SYMB_PY_J,
    MMI_IMC_SYMB_PY_K,
    MMI_IMC_SYMB_PY_L,

    MMI_IMC_SYMB_PY_M,
    MMI_IMC_SYMB_PY_N,
    MMI_IMC_SYMB_PY_O,

    MMI_IMC_SYMB_PY_P,
    MMI_IMC_SYMB_PY_Q,
    MMI_IMC_SYMB_PY_R,
    MMI_IMC_SYMB_PY_S,

    MMI_IMC_SYMB_PY_T,
    MMI_IMC_SYMB_PY_U,
    MMI_IMC_SYMB_PY_V,

    MMI_IMC_SYMB_PY_W,
    MMI_IMC_SYMB_PY_X,
    MMI_IMC_SYMB_PY_Y,
    MMI_IMC_SYMB_PY_Z,
    MMI_IMC_SYMB_PY_MAX
}   mmi_imc_symbol_pinyin_enum;
#endif /* #if defined(__MMI_LANG_SM_CHINESE__) */

#if defined(__MMI_LANG_TR_CHINESE__)
typedef enum {
    MMI_IMC_SYMB_ZY_BASE    = 90,
    MMI_IMC_SYMB_ZY_B       = MMI_IMC_SYMB_ZY_BASE + 0,
    MMI_IMC_SYMB_ZY_P,
    MMI_IMC_SYMB_ZY_M,
    MMI_IMC_SYMB_ZY_F,

    MMI_IMC_SYMB_ZY_D,
    MMI_IMC_SYMB_ZY_T,
    MMI_IMC_SYMB_ZY_N,
    MMI_IMC_SYMB_ZY_L,

    MMI_IMC_SYMB_ZY_G,
    MMI_IMC_SYMB_ZY_K,
    MMI_IMC_SYMB_ZY_H,

    MMI_IMC_SYMB_ZY_J,
    MMI_IMC_SYMB_ZY_Q,
    MMI_IMC_SYMB_ZY_X,

    MMI_IMC_SYMB_ZY_ZH,
    MMI_IMC_SYMB_ZY_CH,
    MMI_IMC_SYMB_ZY_SH,
    MMI_IMC_SYMB_ZY_R,

    MMI_IMC_SYMB_ZY_Z,
    MMI_IMC_SYMB_ZY_C,
    MMI_IMC_SYMB_ZY_S,

    MMI_IMC_SYMB_ZY_A,
    MMI_IMC_SYMB_ZY_O,
    MMI_IMC_SYMB_ZY_E,
    MMI_IMC_SYMB_ZY_EH,

    MMI_IMC_SYMB_ZY_AI,
    MMI_IMC_SYMB_ZY_EI,
    MMI_IMC_SYMB_ZY_AU,
    MMI_IMC_SYMB_ZY_OU,

    MMI_IMC_SYMB_ZY_AN,
    MMI_IMC_SYMB_ZY_EN,
    MMI_IMC_SYMB_ZY_ANG,
    MMI_IMC_SYMB_ZY_ENG,
    MMI_IMC_SYMB_ZY_ER,

    MMI_IMC_SYMB_ZY_I,
    MMI_IMC_SYMB_ZY_U,
    MMI_IMC_SYMB_ZY_IU,
    MMI_IMC_SYMB_ZY_MAX
}   mmi_imc_symbol_bopomofo_enum;
#endif /* #if defined(__MMI_LANG_TR_CHINESE__) */


#define MMI_IMC_SYMB_SEPARATER    255

/*****************************************************************************
 * Define
 *****************************************************************************/

#define MMI_IMC_IS_SYMB_TONE( symb ) \
        ( (symb) >= MMI_IMC_SYMB_TONE_BASE && (symb) < MMI_IMC_SYMB_TONE_MAX ? MMI_TRUE : MMI_FALSE )
        
#if defined(__MMI_LANG_SM_CHINESE__) || defined(__MMI_LANG_TR_CHINESE__)
#define MMI_IMC_IS_SYMB_STROKE( symb ) \
        ( (symb) >= MMI_IMC_SYMB_STROKE_BASE && (symb) < MMI_IMC_SYMB_STROKE_MAX ? MMI_TRUE : MMI_FALSE )
#endif /* defined(__MMI_LANG_SM_CHINESE__) || defined(__MMI_LANG_SM_CHINESE__) */

#define MMI_IMC_IS_SYMB_KEY( symb ) \
        ( (symb) >= MMI_IMC_SYMB_KEY_BASE && (symb) < MMI_IMC_SYMB_KEY_MAX ? MMI_TRUE : MMI_FALSE )

#if defined(__MMI_LANG_SM_CHINESE__)
#define MMI_IMC_IS_SYMB_PY( symb ) \
        ( (symb) >= MMI_IMC_SYMB_PY_BASE && (symb) < MMI_IMC_SYMB_PY_MAX ? MMI_TRUE : MMI_FALSE )
#endif /* #if defined(__MMI_LANG_SM_CHINESE__) */

#if defined(__MMI_LANG_TR_CHINESE__)
#define MMI_IMC_IS_SYMB_ZY( symb ) \
        ( (symb) >= MMI_IMC_SYMB_ZY_BASE && (symb) < MMI_IMC_SYMB_ZY_MAX ? MMI_TRUE : MMI_FALSE )
#endif /* #if defined(__MMI_LANG_TR_CHINESE__) */

#if 0 /* These changes are different during different vendors */
#if defined(__MMI_LANG_SM_CHINESE__)
/* under construction !*/
/* under construction !*/
/* under construction !*/
#endif /* #if defined(__MMI_LANG_SM_CHINESE__) */
/* under construction !*/
#if defined(__MMI_LANG_TR_CHINESE__)
/* under construction !*/
/* under construction !*/
#endif /* #if defined(__MMI_LANG_TR_CHINESE__) */
#endif /* 0 */

#endif /* !defined(__IMC_SYMBOL_H__) */ 
