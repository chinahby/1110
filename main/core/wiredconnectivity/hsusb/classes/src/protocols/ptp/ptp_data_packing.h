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
#ifndef __PTP_DATA_PACKING_H__
#define __PTP_DATA_PACKING_H__

#include "ptp_types.h"

/* PTP data packing/unpacking services */
#ifdef PTP_TRANSPORT_BIG_ENDIAN         /* Transport is big endian */

#define PTP_H2T16       ua_htobe16
#define PTP_H2T32       ua_htobe32
#define PTP_T2H16       ua_be16toh
#define PTP_T2H32       ua_be32toh

#elif PTP_TRANSPORT_LITTLE_ENDIAN       /* Transport is little endian */

#define PTP_H2T16       ua_htole16
#define PTP_H2T32       ua_htole32
#define PTP_T2H16       ua_le16toh
#define PTP_T2H32       ua_le32toh

#else
#error PTP Transport Endianess is not defined
#endif

#define PTP_STR_PACKED_SIZE(nc) (nc*sizeof(wchar_t)+sizeof(juint8_t))
#define PTP_UINT64_PACKED_SIZE  (sizeof(juint32_t)*2)
#define PTP_UINT128_PACKED_SIZE (PTP_UINT64_PACKED_SIZE*2)

/*  P A C K I N G  */

void pack_u8(void **buf, juint8_t u8);
void pack_u16(void **buf, juint16_t u16);
void pack_u32(void **buf, juint32_t u32);
void pack_u64(void **buf, ptp_uint64_t *u64);
void pack_u128(void **buf, ptp_uint128_t *u128);
juint32_t pack_str(void **buf, juint32_t max_size /* In bytes */,
    wchar_t *in_str);

juint32_t pack_prop_value(ptp_type_t type, void **buf, juint32_t buf_size,
    ptp_prop_value_t *value);
juint32_t unpack_prop_value(ptp_type_t type, void *buf, juint32_t buf_size,
    ptp_prop_value_t *value, void *out_buf, void *res_buf, juint32_t *res);
juint32_t pack_dev_prop_desc(void **buf, ptp_dev_prop_desc_t *dev_prop);

/*  U N P A C K I N G  */

juint8_t unpack_u8(void **buf);
juint16_t unpack_u16(void **buf);
juint32_t unpack_u32(void **buf);
ptp_uint64_t unpack_u64(void **buf);
ptp_uint128_t unpack_u128(void **buf);
juint32_t unpack_str(void **buf, wchar_t **out_str);

void unpack_objinfo(void *buf, ptp_obj_info_t *objinfo);


#endif /*__PTP_DATA_PACKING_H__*/
