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
#ifndef _MTP_H_
#define _MTP_H_

typedef struct
{
    /* for current array property values */
    jbool_t              is_array;
    juint8_t            *res_buf;
    juint32_t            residue_count;

    /* corresponding with ptp_array_value_t elems offset fields */
    juint32_t            prop_total_elems;
    juint32_t            prop_num_elems;
    juint32_t            prop_elem_offset; 

    void               *unpack_buf;
    juint32_t            unpack_buf_size; /* same size as PTP_MAX_BLOCK_SIZE + 
                                            PTP_MAX_RESIDUE */
} mtp_ctx_t;

enum ptp_type get_obj_prop_type(mtp_obj_prop_t prop_code);
juint32_t get_prop_desc_size(mtp_obj_prop_desc_t *obj_prop);
juint32_t pack_obj_prop_desc(void **buf, mtp_obj_prop_desc_t *obj_prop);
juint32_t calc_max_type(mtp_obj_prop_t prop_code, juint32_t buf_size/*in bytes*/);
#endif
