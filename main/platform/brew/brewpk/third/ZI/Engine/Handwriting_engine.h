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
 *    Handwriting_engine.h
 *
 * Project:
 * --------
 *   Maui_Software
 *
 * Description:
 * ------------
 *   common header file for HanwangEngine.c, PenpowerEngine.c and DecumaEngine.c
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
#include "MMI_features.h"

#ifndef __HANDWRITING_ENGINE_H__
#define __HANDWRITING_ENGINE_H__

#include "IMERes.h"
#include "ImeGprot.h"
#include "ImmProt.h"
#include "Imc_config.h"
#include "FrameworkGprot.h"

/*****************************************************************************
 * Define
 *****************************************************************************/


/*****************************************************************************
 * Typedef
 *****************************************************************************/


typedef struct
{
    mmi_pen_point_struct *stroke_buffer;    
    mmi_imc_pen_handwriting_type_enum h_w_type;
    S32 desired_cnt;
} mmi_ime_hand_writing_query_param, *mmi_ime_hand_writing_query_param_p;


typedef struct
{
    UI_character_type *candidate_array;
    S32 result_cnt;
} mmi_ime_hand_writing_result_param, *mmi_ime_hand_writing_result_param_p;

typedef struct
{
    UI_string_type      pre_string;     /* IN */
    S32 first_index;
    S32 desired_cnt;
}mmi_ime_hand_writing_phrase_query_param, *mmi_ime_hand_writing_phrase_query_param_p;

typedef struct
{
    UI_string_type      result_buffer;  /* OUT */
    U8                  result_cnt;     /* OUT */
    MMI_BOOL            next_page;      /* OUT */
}mmi_ime_hand_writing_phrase_result_param, *mmi_ime_hand_writing_phrase_result_param_p;

/*****************************************************************************
*  Global Functions
*****************************************************************************/
extern S32 mmi_ime_hand_writing_get_candidates(
                    mmi_ime_hand_writing_query_param_p query_param,
                    mmi_ime_hand_writing_result_param_p result_param);

extern void mmi_ime_hand_writing_initialize(mmi_imc_pen_handwriting_type_enum hw_type);

extern void mmi_ime_hand_writing_deinitialize(void);

extern MMI_BOOL mmi_ime_hand_writing_hw_type_query(mmi_imc_pen_handwriting_type_enum h_w_type);
extern void  mmi_ime_hand_writing_add_separator_to_candidate_buffer(UI_character_type * destination_buffer, UI_character_type * source_buffer, S32 num);

#if defined(__MMI_HANDWRITING_PHRASE__)            
extern S32 mmi_ime_hand_writing_get_candidates_by_phrase(mmi_ime_hand_writing_phrase_query_param_p query, mmi_ime_hand_writing_phrase_result_param_p result);
#endif /*  __MMI_HANDWRITING_PHRASE__ */


#endif /* __HANDWRITING_ENGINE_H__ */
