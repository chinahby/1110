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
 *    Imc_config.h
 *
 * Project:
 * --------
 *   Maui_Software
 *
 * Description:
 * ------------
 *   header file for IMC
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
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#include "OEMFeatures.h"

#ifndef __IMC_CONFIG_H__
#define __IMC_CONFIG_H__

/*****************************************************************************
 * Define
 *****************************************************************************/

#if defined(__MMI_ZI__)
#define MULTITAP_MAX_KEY_CHAR   (44)
#elif defined(__MMI_CSTAR__)
#define MULTITAP_MAX_KEY_CHAR   (32)
#else
#define MULTITAP_MAX_KEY_CHAR   (24)
#endif
#define MULTITAP_KEY_TOTAL      (MMI_IMC_SYMB_KEY_MAX - MMI_IMC_SYMB_KEY_BASE)

/* Config buffer size */
#define MMI_IMC_MAX_INPUT_SYMBOL_BUFFSIZE           (32)
#define MMI_IMC_MAX_CANDIDATE_BUFFSIZE              (400)
#define MMI_IMC_MAX_INPUT_STRING_SIZE               (150)
#define MMI_IMC_MAX_INPUT_BUFFSIZE                  (MMI_IMC_MAX_INPUT_STRING_SIZE + 1)
#define MMI_IMC_MAX_COMPOSITION_BUFFSIZE            (64)
#define MMI_IMC_KEY_MAX_TEXT_REFER_BY_ASSOCIATIVE   (40)
#define MMI_IMC_PEN_STROKE_BUFFER_SIZE              (500)
#define MMI_IMC_MAX_KEY_NUM                         (MMI_IMC_MAX_INPUT_SYMBOL_BUFFSIZE)
#define MMI_IMC_MAX_KEY_BUFFER_SIZE                 (MMI_IMC_MAX_KEY_NUM + 1)

#if defined(__MMI_ALPHABETIC_WORD_PREDICTION__) && defined(__MMI_T9__)
#define MMI_IMC_MAX_PRE_STRING_LEN                  (MMI_IMC_MAX_INPUT_SYMBOL_BUFFSIZE * 2 + 1)
#else
#define MMI_IMC_MAX_PRE_STRING_LEN                  (MMI_IMC_MAX_INPUT_SYMBOL_BUFFSIZE)
#endif

/* The max candidate count in one page */
/* Shoud be specified by resolutions */
#define MMI_IMC_MAX_CANDIDATE_IN_ONE_PAGE_LATIN     (10)

#ifdef __MMI_CSTAR__
    #define MMI_IMC_MAX_CANDIDATE_IN_ONE_PAGE_CHINESE   (9) /* In Cstar, this value must less than or equal to 9  */
#else
    #define MMI_IMC_MAX_CANDIDATE_IN_ONE_PAGE_CHINESE   (10) /* Can be defined by screen size, can be every value */
#endif

/* set the cand number in one page as 1. It means that only one candidate return in one query. */
#define MMI_IMC_MAX_CANDIDATE_IN_ONE_PAGE_MULTITP (10)

#ifdef __MMI_MAINLCD_128X160__
    #define MMI_IMC_PEN_MAX_CANDIDATE_NUM (8)
#else
    #define MMI_IMC_PEN_MAX_CANDIDATE_NUM (9)   
#endif /* __MMI_MAINLCD_128X160__ */

/* Page size */
#define MMI_IMC_MAX_CANDIDATE_PAGE                  (40)
#define MMI_IMC_MAX_COMPOSITION_PAGE                (40)

#define MMI_IMC_HAND_WRITING_MAX_CANDIDATE_SIZE 20

#if defined(__MMI_ZI_CHINESE_PHRASE_LEVEL_INPUTS__)
#define MMI_IMC_COMPOSITION_BOX_STYLE     (MMI_IMUI_SELECTION_AREA_STYLE_ARROW | MMI_IMUI_SELECTION_AREA_STYLE_BORDER | MMI_IMUI_SELECTION_AREA_STYLE_SPACE_AFTER_APOSTROPHE)
#else
#define MMI_IMC_COMPOSITION_BOX_STYLE     (MMI_IMUI_SELECTION_AREA_STYLE_ARROW | MMI_IMUI_SELECTION_AREA_STYLE_BORDER | MMI_IMUI_INPUT_AREA_STYLE_CENTER_ALIGN)
#endif
                                          
#define MMI_IMC_CANDIDATE_BOX_STYLE    (MMI_IMUI_SELECTION_AREA_STYLE_ARROW | MMI_IMUI_SELECTION_AREA_STYLE_BORDER)

#define MMI_IMC_HINT_CANDIDATE 0x3e

/* Config UDB function */
#if defined(__MMI_CSTAR_USER_DICT__) && !defined(__MMI_TOUCH_SCREEN__) && !defined(__MMI_IME_USER_DATABASE__)
    #define __MMI_IME_USER_DATABASE__
#endif

#if defined(__MMI_IME_USER_DATABASE__)

    #define MMI_IME_USER_DATABASE_PATH           L"Z:\\@ime"
    #define MMI_IME_USER_DATABASE_NEW_WORD       (0xffff)

    #if defined(__MMI_CSTAR__)
    #define MMI_IMC_TITLE_BOX_STYLE              (MMI_IMUI_INPUT_AREA_STYLE_BORDER)
    #define MMI_IMC_INPUT_BOX_STYLE              (MMI_IMUI_INPUT_AREA_STYLE_CURSOR | MMI_IMUI_INPUT_AREA_STYLE_BORDER)
    #endif

#endif

/* Configuration by engine */

#if defined(__MMI_T9__) /* T9 engine configeration */

#if !defined(__MMI_T9_DEFAULT_KEY_DEFINITION__)
#define __MMI_IME_TONE_IN_STAR_KEY__
#endif

#elif defined(__MMI_ZI__) /* Zi engine configeration */

    #define MMI_IME_PREDICTION_START_AT             (5)

    #undef MMI_IMC_CANDIDATE_BOX_STYLE
    #define MMI_IMC_CANDIDATE_BOX_STYLE (MMI_IMUI_SELECTION_AREA_STYLE_ARROW | MMI_IMUI_SELECTION_AREA_STYLE_BORDER)

#if !defined(__MMI_ZI_CHINESE_PHRASE_LEVEL_INPUTS__)
    #define __MMI_IME_CANDIDATE_BOX_STYLE_FOCUS_ON_CENTER__
#endif

#elif defined(__MMI_CSTAR__) /* Cstar engine configeration */

    #undef MMI_IMC_INPUT_BOX_STYLE
    #define MMI_IMC_INPUT_BOX_STYLE       (MMI_IMUI_INPUT_AREA_STYLE_WAVE | MMI_IMUI_INPUT_AREA_STYLE_CURSOR | MMI_IMUI_INPUT_AREA_STYLE_BORDER)

    #if defined(__MMI_IME_SMART_WITH_INPUT_AREA__)
        #define MMI_IMC_MAX_KEY_SEQUENCE_RECORD 10

        #undef MMI_IMC_MAX_INPUT_STRING_SIZE
        #undef MMI_IMC_MAX_INPUT_BUFFSIZE

        #define MMI_IMC_MAX_INPUT_STRING_SIZE               (MMI_IMC_MAX_KEY_SEQUENCE_RECORD * (MMI_IMC_MAX_INPUT_SYMBOL_BUFFSIZE + 1))
        #define MMI_IMC_MAX_INPUT_BUFFSIZE                  (MMI_IMC_MAX_INPUT_STRING_SIZE + 1)
    #endif

    #undef MMI_IMC_CANDIDATE_BOX_STYLE
    #define MMI_IMC_CANDIDATE_BOX_STYLE (MMI_IMUI_SELECTION_AREA_STYLE_ARROW | MMI_IMUI_SELECTION_AREA_STYLE_BORDER | MMI_IMUI_SELECTION_AREA_STYLE_LABEL)

#endif 

#ifdef __MMI_TOUCH_SCREEN__

    #undef MMI_IMC_CANDIDATE_BOX_STYLE
    #define MMI_IMC_CANDIDATE_BOX_STYLE (MMI_IMUI_SELECTION_AREA_STYLE_ARROW |  MMI_IMUI_SELECTION_AREA_STYLE_BORDER | MMI_IMUI_SELECTION_AREA_STYLE_SEPARATOR)

#endif /* __MMI_TOUCH_SCREEN__ */


#endif /* !defined(__IMC_CONFIG_H__) */ 
