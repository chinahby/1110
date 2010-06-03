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

#include <jos.h>
#include "ptp_utils.h"

juint32_t ptp_type_info(enum ptp_type type)
{
    juint32_t rc = type & 0x000F;

    if (type == PTP_TYPE_STR)
        return sizeof(juint16_t);

    rc = (1 << ((rc+1) >>1)) >> 1;

    if ((type & 0X4000) && (type != PTP_TYPE_STR))
        rc |= 0x10000;

    return rc;
}

/*
 *   PTP string manipulation
 */

wchar_t* ptp_string_alloc(juint32_t size)
{
    if (size > PTP_MAX_STRING_LEN)
        return NULL;
    return jmalloc(size*sizeof(wchar_t), M_ZERO);
}

juint32_t ptp_string_len(wchar_t *str)
{
    juint32_t i = 0;
    if (!str)
        return 0;
    while (i < PTP_MAX_STRING_LEN-1 && str[i])
        i++;
    return i;
}

wchar_t* ptp_string_duplicate(wchar_t *in_str)
{
    wchar_t *new_str;
    juint32_t str_len;

    if (!in_str)
        return NULL;
    
    str_len = ptp_string_len(in_str);
    new_str = ptp_string_alloc(str_len+1);
    
    if (!new_str)
        return NULL;

    j_memcpy(new_str, in_str, str_len*sizeof(wchar_t));
    new_str[str_len] = 0;
    return new_str;
}

/* ASCII to WIDE conversion */
wchar_t* ptp_string_atow(char *ascii_str)
{
    wchar_t *wstr = ptp_string_alloc(j_strlen(ascii_str) + 1);

    if (wstr)
    {
        juint32_t i = 0;
        while (ascii_str[i])
        {
            wstr[i] = (wchar_t)ascii_str[i];
            i++;
        }
        wstr[i] = 0;
    }
    
    return wstr;
}

char* ptp_wide_to_ascii(wchar_t *wide_str)
{
    return NULL;
}

jint_t ptp_string_compare(wchar_t *str1, wchar_t *str2)
{
    while (*str1 && *str2)
    {
        if (*str1 < *str2)
            return -1;
        else if (*str1 > *str2)
            return 1;
        
        str1++;
        str2++;
    }

    return 0;
}

void ptp_string_free(wchar_t *str)
{
    jfree(str);
}

/*
 *   PTP dataset manipulation
 */

ptp_obj_info_t* ptp_objectinfo_alloc(void)
{
    return jmalloc(sizeof(ptp_obj_info_t), M_ZERO);    
}

void ptp_objectinfo_free(ptp_obj_info_t *objinfo)
{
    if (objinfo)
    {
        if (objinfo->filename)
            jfree(objinfo->filename);
        if (objinfo->capture_date)
            jfree(objinfo->capture_date);
        if (objinfo->modification_date)
            jfree(objinfo->modification_date);
        if (objinfo->keywords)
            jfree(objinfo->keywords);

        jfree(objinfo);
    }
}

void ptp_prop_value_copy(ptp_prop_value_t *dst, ptp_prop_value_t *src)
{
    juint32_t elem_size;

    j_memcpy(dst, src, sizeof(ptp_prop_value_t));

    /* Special handling for strings (need duplication) */
    if (src->data_type == PTP_TYPE_STR)
    {
        if (src->data.string)
            dst->data.string = ptp_string_duplicate(src->data.string);
        return;
    }
    
    /* If prop is a single value - our job is done */
    if (!PTP_IS_TYPE_ARRAY(src->data_type))
        return;

    /* Prop is an array - we need to duplicate it */
    if (src->num_elems && src->data.a_uint8)
    {
        elem_size = PTP_GET_TYPE_SIZE(src->data_type);
        dst->data.a_uint8 = (juint8_t*)jmalloc(src->num_elems*elem_size, 0);
        if (dst->data.a_uint8)
            j_memcpy(dst->data.a_uint8, src->data.a_uint8, src->num_elems*elem_size);
    }
}

void ptp_prop_value_clear(ptp_prop_value_t *val)
{
    if ((val->data_type == PTP_TYPE_STR) && val->data.string)
        ptp_string_free(val->data.string);
    else if (PTP_IS_TYPE_ARRAY(val->data_type) && val->data.a_uint8)
        jfree(val->data.a_uint8);

    j_memset(val, 0, sizeof(ptp_prop_value_t));
}

