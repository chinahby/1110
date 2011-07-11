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
 *    Multitap_engine.h
 *
 * Project:
 * --------
 *   Maui_Software
 *
 * Description:
 * ------------
 *   header file for multi-tap input method
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
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#include "MMI_features.h"

#ifndef __MULTITAP_ENGINE_H__
#define __MULTITAP_ENGINE_H__

#include "MMIDataType.h"
#include "IMERes.h"
#include "ImeGProt.h"
#include "ImmProt.h"
#include "Imc_symbol.h"
#include "Imc_config.h"

/****************************************MMI_IME_REQUIRED_KEY_STAR *************************************
 * Typedef
 *****************************************************************************/


typedef struct
{
    PU16 elembuf_ptr;    /* IN */
    S32 elembuf_cnt;
    S32 first_index;
    S32 desired_cnt;
}mmi_ime_multitap_query_param_struct, *mmi_ime_multitap_query_param_struct_p;

typedef struct
{
    UI_string_type result_buffer;
    S32 result_count;
    MMI_BOOL next_page;
}mmi_ime_multitap_query_result_struct, *mmi_ime_multitap_query_result_struct_p;

typedef MMI_BOOL (*valid_key_check_func)(mmi_imc_symbol_key_enum sym);
typedef UI_string_type (*get_multitap_string)(S32 i);

/*****************************************************************************
 * Global Functions
 *****************************************************************************/

extern MMI_BOOL         mmi_ime_multitap_init( void );
extern void             mmi_ime_multitap_deinit( void );
extern MMI_BOOL         mmi_ime_multitap_change_mode( const sIMEModeDetails *mode_details_ptr, U32 reserved );
extern MMI_BOOL mmi_ime_multitap_get_candidate( mmi_ime_multitap_query_param_struct_p param_ptr, mmi_ime_multitap_query_result_struct_p result_ptr );
extern MMI_BOOL mmi_ime_multitap_is_symbol_key( mmi_imm_input_mode_enum input_mode, mmi_imc_key_custom_value key_code );
extern void mmi_ime_multitap_confirm(void);
extern mmi_imm_input_mode_enum mmi_ime_get_current_multitap_input_mode(void);

#if defined(__MMI_ZI_MULTITAP__) || defined(__MMI_T9_MULTITAP__) || defined(__MMI_CSTAR_MULTITAP__)
extern MMI_BOOL         mmi_ime_multitap_vendor_init( void );
extern void             mmi_ime_multitap_vendor_deinit( void );
extern get_multitap_string mmi_ime_multitap_vendor_get_string_func( mmi_imm_input_mode_enum input_mode );
extern valid_key_check_func mmi_ime_multitap_vendor_query_key_check_function( mmi_imm_input_mode_enum input_mode );
extern MMI_BOOL mmi_ime_multitap_vendor_change_mode( const sIMEModeDetails *mode_details_ptr, U32 reserved );
#endif /* defined(__MMI_ZI_MULTITAP__) || defined(__MMI_T9_MULTITAP__) || defined(__MMI_CSTAR_MULTITAP__) */


#endif /* __MULTITAP_ENGINE_H__ */ 





















