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
 *    Imc_pen.h
 *
 * Project:
 * --------
 *   Maui_Software
 *
 * Description:
 * ------------
 *   header file for Imc_pen.c
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
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#include "MMI_features.h"

#ifdef __MMI_TOUCH_SCREEN__

#ifndef __IMC_PEN_H__
#define __IMC_PEN_H__

#include "IMERes.h"
#include "FrameworkGprot.h"

/*****************************************************************************
 *  Define
 *****************************************************************************/


typedef enum
{
    MMI_IMC_PEN_SORT_CAPITAL_FIRST,
    MMI_IMC_PEN_SORT_NON_CAPITAL_FIRST
} mmi_imc_pen_resort_type_enum;

/*****************************************************************************
 *  Global Functions
 *****************************************************************************/

extern MMI_BOOL mmi_imc_pen_init( void );
extern void     mmi_imc_pen_deinit( void );
extern MMI_BOOL mmi_imc_pen_change_mode(const sIMEModeDetails * mode_details_ptr, U32 reserved );

extern void     mmi_imc_pen_activate( void );
extern void     mmi_imc_pen_deactivate( void );
extern void     mmi_imc_pen_disable_handwriting( void );
extern void     mmi_imc_pen_enable_handwriting( void );
extern void     mmi_imc_pen_change_handwriting_area( void );
extern void     mmi_imc_pen_set_desired_vk_type(gui_virtual_keyboard_language_enum vk_type);
extern void     mmi_imc_pen_enable_multiblock_handwriting( void );
extern void     mmi_imc_pen_disable_multiblock_handwriting( void );
extern MMI_BOOL mmi_imc_pen_is_multiblock_handwriting_type(gui_virtual_keyboard_language_enum vk_type);
extern mmi_imc_pen_handwriting_style_enum mmi_imc_pen_get_handwriting_style(void);
extern MMI_BOOL mmi_imc_pen_vk_insert_char(UI_character_type char_code);
extern MMI_BOOL mmi_imc_pen_set_vk_candidate(void);
extern void mmi_imc_pen_update_display_area_by_sym_buffer(void);
extern void mmi_imc_pen_softkey_up_post_hdlr(FuncPtr f);

#endif /* !defined(__IMC_PEN_H__) */ 
#endif /* #ifdef __MMI_TOUCH_SCREEN__ */
