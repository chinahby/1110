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

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#ifndef _PTP_UTILS_H_
#define _PTP_UTILS_H_

#include "ptp_types.h"

/*
 *   PTP type helper functions
 */

#define PTP_GET_TYPE_SIZE(type)       \
    (ptp_type_info(type) & 0x0000FFFF)

#define PTP_IS_TYPE_ARRAY(type)       \
    (ptp_type_info(type) & 0x000F0000)

juint32_t ptp_type_info(ptp_type_t data_type);

/*
 *   PTP string manipulation
 */

wchar_t* ptp_string_alloc(juint32_t size);
void ptp_string_free(wchar_t *str);
juint32_t ptp_string_len(wchar_t *str);
wchar_t* ptp_string_duplicate(wchar_t *in_str);
jint_t ptp_string_compare(wchar_t *str1, wchar_t *str2);

/* ASCII - WIDE conversion */
wchar_t* ptp_string_atow(char *ascii_str);
char* ptp_wide_to_ascii(wchar_t *wide_str);


/*
 *   PTP dataset manipulation
 */

ptp_obj_info_t* ptp_objectinfo_alloc(void);
void ptp_objectinfo_free(ptp_obj_info_t *objinfo);

/*
 *   PTP propery manipulation
 */

void ptp_prop_value_copy(ptp_prop_value_t *dst, ptp_prop_value_t *src);
void ptp_prop_value_clear(ptp_prop_value_t *val);
    
#endif/*_PTP_UTILS_H_*/

