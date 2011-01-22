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
/*
 * Copyright (c) 2004 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Lennart Augustsson (lennart@augustsson.net) and by Charles M. Hannum.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __JHID_PARSER_H
#define __JHID_PARSER_H

enum hid_type 
{
    hid_input = 0, 
    hid_output, 
    hid_feature, 
    hid_collection, 
    hid_endcollection
};

enum hid_ret_value
{
    hid_end_report = 0,
    hid_end_item,
    hid_end_collection
};

struct hid_location 
{
    juint32_t size;
    juint32_t count;
    juint32_t pos;
};

struct hid_limits
{
    juint32_t usage;
    jint32_t logical_min;
    jint32_t logical_max;
    jint32_t physical_min;
    jint32_t physical_max;
    jint32_t usage_min;
    jint32_t usage_max;
    jint32_t unit;
};

jint_t hid_report_id_by_index(void *buf, juint16_t len, juint8_t index);
jint_t hid_report_count(void *buf, juint16_t len);
jint_t hid_report_size(void *buf, juint16_t len, enum hid_type k, 
    juint8_t report_id);
jresult_t hid_get_field_location(void *desc, juint16_t size, juint32_t usage,
    enum hid_type type,  juint8_t report_id, struct hid_location *loc, 
    juint32_t *flags, struct hid_limits *lim);
juint32_t hid_get_data(juint8_t *buf, struct hid_location *loc, struct hid_limits *lim);
jbool_t hid_is_collection(void *desc, juint16_t size, juint32_t usage);
jresult_t hid_set_data(juint8_t *buf, struct hid_location *loc,
    struct hid_limits *lim, juint32_t data);
typedef void* jhid_report_desc_t;

/* Report Descriptor bType values*/
#define HID_TYPE_MAIN                   0
#define HID_TYPE_GLOBAL                 1
#define HID_TYPE_LOCAL                  2
#define HID_TYPE_RESERVED               3
/* Report Descriptor MAIN bTag values */
#define HID_TAG_MAIN_INPUT              8
#define HID_TAG_MAIN_OUTPUT             9
#define HID_TAG_MAIN_COLLECTION         10
#define HID_TAG_MAIN_FEATURE            11
#define HID_TAG_MAIN_END_COLLECT        12
/* Report Descriptor GLOBAL bTag values */
#define HID_TAG_GLOBAL_USAGE_PAGE       0
#define HID_TAG_GLOBAL_LOG_MIN          1
#define HID_TAG_GLOBAL_LOG_MAX          2
#define HID_TAG_GLOBAL_PHYS_MIN         3
#define HID_TAG_GLOBAL_PHYS_MAX         4
#define HID_TAG_GLOBAL_UNIT_EXP         5
#define HID_TAG_GLOBAL_UNIT             6
#define HID_TAG_GLOBAL_REP_SIZE         7
#define HID_TAG_GLOBAL_REP_ID           8
#define HID_TAG_GLOBAL_REP_COUNT        9
#define HID_TAG_GLOBAL_PUSH             10
#define HID_TAG_GLOBAL_POP              11
/* Report Descriptor LOCAL bTag values */
#define HID_TAG_LOCAL_USAGE             0
#define HID_TAG_LOCAL_USAGE_MIN         1
#define HID_TAG_LOCAL_USAGE_MAX         2
#define HID_TAG_LOCAL_DESIG_INDEX       3
#define HID_TAG_LOCAL_DESIG_MIN         4
#define HID_TAG_LOCAL_DESIG_MAX         5
#define HID_TAG_LOCAL_STR_INDEX         7
#define HID_TAG_LOCAL_STR_MIN           8
#define HID_TAG_LOCAL_STR_MAX           9
#define HID_TAG_LOCAL_DELIMITER         10
#define HID_TAG_LOCAL_RESERVED          11

#endif
