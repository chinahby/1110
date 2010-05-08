/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.











************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

#ifndef _OBJ_PROPS_H_
#define _OBJ_PROPS_H_

#ifdef CONFIG_FD_MTP_APP

#include "sicd_sample.h"

#define MTP_PROPERTY_COUNT 171
#define DESC_GET_SET_DEVICE_DEFINED 1
#define DESC_FORM_FIXED_LEN_BYTE_ARRAY 1000
#define DESC_FORM_MAX_BYTE_ARR 1000
#define DESC_FORM_MAX_LONG_STR 1000
#define DESC_GROUP_CODE_DEVICE_DEFINED GROUP_CODE_NONE
#define DESC_SET_GET 1
#define DESC_GET 0

typedef struct
{
    ptp_obj_format_t            obj_format;
    mtp_obj_prop_desc_t        *obj_prop_desc;
    juint32_t                    obj_prop_desc_count;
} obj_format_prop_desc_t;

extern obj_format_prop_desc_t obj_format_prop_desc_map[];

void objprops_init_descs(void);
void objprops_free_descs(void);

jresult_t objprops_setup_obj(sicd_object_t *obj);
void objprops_free(sicd_object_t *obj);

#endif /* CONFIG_FD_MTP_APP */

#endif /*_OBJ_PROPS_H_*/
