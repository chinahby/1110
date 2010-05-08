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

#include <jtypes.h>
#include <os_port.h>
#include <jos.h>
#include <jusb.h>
#include <uw_host_hid_common.h>

#include "jhid_parser.h"

struct hid_item {
    /* Global */
    jint32_t _usage_page;
    jint32_t logical_minimum;
    jint32_t logical_maximum;
    jint32_t physical_minimum;
    jint32_t physical_maximum;
    jint32_t unit_exponent;
    jint32_t unit;
    jint32_t report_ID;
    jint32_t report_size;
    /* Local */
    jint32_t usage;
    jint32_t usage_minimum;
    jint32_t usage_maximum;
    jint32_t designator_index;
    jint32_t designator_minimum;
    jint32_t designator_maximum;
    jint32_t string_index;
    jint32_t string_minimum;
    jint32_t string_maximum;
    jint32_t set_delimiter;
    /* Misc */
    jint32_t collection;
    jint_t collevel;
    enum hid_type type;
    juint32_t flags;
    /* Location */
    struct hid_location loc;
    /* */
    struct hid_item *next;
};

struct hid_data *hid_start_parse(void *d, juint16_t len, jint_t typeset);
void hid_end_parse(struct hid_data *s);
enum hid_ret_value hid_get_item(struct hid_data *s, struct hid_item *h);


static void hid_clear_local(struct hid_item *);

#define MAXUSAGE 100
struct hid_data {
    juint8_t *start;
    juint8_t *end;
    juint8_t *p;
    struct hid_item cur;
    jint32_t usages[MAXUSAGE];
    jint_t nu;
    jint_t minset;
    jint_t multi;
    jint_t multimax;
    jint_t typeset;
    jint_t report_count;
};

static void hid_clear_local(struct hid_item *c)
{
    c->usage = 0;
    c->usage_minimum = 0;
    c->usage_maximum = 0;
    c->designator_index = 0;
    c->designator_minimum = 0;
    c->designator_maximum = 0;
    c->string_index = 0;
    c->string_minimum = 0;
    c->string_maximum = 0;
    c->set_delimiter = 0;
}

struct hid_data * hid_start_parse(void *d, juint16_t len, jint_t typeset)
{
    struct hid_data *s;

    s = jmalloc(sizeof(struct hid_data), M_ZERO);
    s->start = s->p = d;
    s->end = (juint8_t *)d + len;
    s->typeset = typeset;
    s->report_count = 0;
    return (s);
}

void hid_end_parse(struct hid_data *s)
{
    while (s->cur.next != NULL) 
    {
        struct hid_item *hi = s->cur.next->next;
        jfree(s->cur.next);
        s->cur.next = hi;
    }
    jfree(s);
}

static void parse_item_header(juint8_t **p, juint_t *bSize, juint_t *bTag, 
    juint_t *bType, jint32_t *dval)
{

    juint8_t tmp_bTag, tmp_bType; 
    juint16_t tmp_bSize;
    juint8_t *ptr = *p;
    jint32_t tmp_dval = 0;
    
    tmp_bSize = *ptr++;
    if (tmp_bSize == 0xfe) 
    {
        /* long item */
        tmp_bSize = *ptr++;
        tmp_bSize |= *ptr++ << 8;
        tmp_bTag = *ptr++;
        tmp_bType = 0xff; /* XXX what should it be */
    } 
    else 
    {
        /* short item */
        tmp_bTag = tmp_bSize >> 4;
        tmp_bType = (tmp_bSize >> 2) & 3;
        tmp_bSize &= 3;
        if (tmp_bSize == 3) 
            tmp_bSize = 4;
    }

    switch (tmp_bSize) 
    {
    case 0:
        tmp_dval = 0;
        break;
    case 1:
        tmp_dval = (jint8_t)*ptr++;
        break;
    case 2:
        tmp_dval = *ptr++;
        tmp_dval |= *ptr++ << 8;
        tmp_dval = (jint16_t)tmp_dval;
        break;
    case 4:
        tmp_dval = *ptr++;
        tmp_dval |= *ptr++ << 8;
        tmp_dval |= *ptr++ << 16;
        tmp_dval |= *ptr++ << 24;
        break;
    default:
        DBG_E(DHOST_HID_LIB,("parse_item_header: BAD LENGTH %d\n", bSize));
    }

    *p = ptr;
    *bSize = tmp_bSize;
    *bTag = tmp_bTag;
    *bType = tmp_bType;
    *dval = tmp_dval;
}

static void parse_global_item(juint_t bTag, jint32_t dval, struct hid_data *s,
    struct hid_item *c)
{
    juint32_t oldpos;
    struct hid_item *hi;

    switch (bTag) 
    {
    case HID_TAG_GLOBAL_USAGE_PAGE:
        c->_usage_page = dval << 16;
        break;
    case HID_TAG_GLOBAL_LOG_MIN:
        c->logical_minimum = dval;
        break;
    case HID_TAG_GLOBAL_LOG_MAX:
        c->logical_maximum = dval;
        break;
    case HID_TAG_GLOBAL_PHYS_MIN:
        c->physical_maximum = dval;
        break;
    case HID_TAG_GLOBAL_PHYS_MAX:
        c->physical_maximum = dval;
        break;
    case HID_TAG_GLOBAL_UNIT_EXP:
        c->unit_exponent = dval;
        break;
    case HID_TAG_GLOBAL_UNIT:
        c->unit = dval;
        break;
    case HID_TAG_GLOBAL_REP_SIZE:
        c->loc.size = dval;
        break;
    case HID_TAG_GLOBAL_REP_ID:
        if ((c->loc.pos != 8) || !c->report_ID)
            c->report_size = c->loc.pos;

        c->report_ID = dval;
        c->loc.pos = 8;
        s->report_count++;
        break;
    case HID_TAG_GLOBAL_REP_COUNT:
        c->loc.count = dval;
        break;
    case HID_TAG_GLOBAL_PUSH:
        hi = jmalloc(sizeof(struct hid_item), M_ZERO);
        *hi = s->cur;
        c->next = hi;
        break;
    case HID_TAG_GLOBAL_POP:
        hi = c->next;
        oldpos = c->loc.pos;
        s->cur = *hi;
        c->loc.pos = oldpos;
        jfree(hi);
        break;
    default:
        DBG_E(DHOST_HID_LIB,("parse_global_item: Unknown Global bTag=%d\n", 
            bTag));
        break;
    }
}

static void parse_local_item(juint_t bSize, juint_t bTag, struct hid_data *s,
    struct hid_item *c, jint32_t dval)
{
    switch (bTag) 
    {
    case HID_TAG_LOCAL_USAGE:
        if (bSize == 1)
            dval = c->_usage_page | (dval & 0xff);
        else
        {
            if (bSize == 2)
                dval = c->_usage_page | (dval & 0xffff);
        }
        c->usage = dval;
        if (s->nu < MAXUSAGE)
            s->usages[s->nu++] = dval;
        break;
    case HID_TAG_LOCAL_USAGE_MIN:
        s->minset = 1;
        if (bSize == 1)
            dval = c->_usage_page | (dval & 0xff);
        else 
        {
            if (bSize == 2)
                dval = c->_usage_page | (dval & 0xffff);
        }
        c->usage_minimum = dval;
        break;
    case HID_TAG_LOCAL_USAGE_MAX:
        if (bSize == 1)
            dval = c->_usage_page | (dval & 0xff);
        else
        {
            if (bSize == 2)
                dval = c->_usage_page | (dval & 0xffff);
        }
        c->usage_maximum = dval;
        break;
    case HID_TAG_LOCAL_DESIG_INDEX:
        c->designator_index = dval;
        break;
    case HID_TAG_LOCAL_DESIG_MIN:
        c->designator_minimum = dval;
        break;
    case HID_TAG_LOCAL_DESIG_MAX:
        c->designator_maximum = dval;
        break;
    case HID_TAG_LOCAL_STR_INDEX:
        c->string_index = dval;
        break;
    case HID_TAG_LOCAL_STR_MIN:
        c->string_minimum = dval;
        break;
    case HID_TAG_LOCAL_STR_MAX:
        c->string_maximum = dval;
        break;
    case HID_TAG_LOCAL_DELIMITER:
        c->set_delimiter = dval;
        break;
    default:
        DBG_E(DHOST_HID_LIB,("parse_local_item: Unknown Local bTag=%d\n", 
            bTag));
        break;
    }
}

static __INLINE__ jbool_t is_multimax(struct hid_data *s, struct hid_item *c, 
    struct hid_item *h)
{
    jbool_t rc = FALSE;
    
    if (s->multimax != 0) 
    {
        if (s->multi < s->multimax) 
        {
            c->usage = s->usages[MIN(s->multi, s->nu-1)];
            s->multi++;
            *h = *c;
            c->loc.pos += c->loc.size;
            h->next = 0;
            rc = TRUE;
        } 
        else 
        {
            c->loc.count = s->multimax;
            s->multimax = 0;
            s->nu = 0;
            hid_clear_local(c);
        }
    }
    return rc;
}

static __INLINE__ jbool_t is_done(struct hid_data *s, struct hid_item *c,
    struct hid_item *h)
{
    jint32_t i;
    jbool_t found = FALSE;
    
    if (c->flags & HIO_VARIABLE) 
    {
        s->multimax = c->loc.count;
        s->multi = 0;
        c->loc.count = 1;
        if (s->minset) 
        {
            for (i = c->usage_minimum; i <= c->usage_maximum; i++) 
            {
                s->usages[s->nu] = i;
                if (s->nu < MAXUSAGE-1)
                    s->nu++;
            }
            s->minset = 0;
        }
        if (is_multimax(s, c, h))
            found = TRUE;
    } 
    else 
    {
        *h = *c;
        h->next = 0;
        c->loc.pos += c->loc.size * c->loc.count;
        hid_clear_local(c);
        s->minset = 0;
        found = TRUE;
    }
    
    return found;
}
#define HANDLE_REPORT_TAG(_s, _type, _c, _h, _dval) \
do { \
    if (!((_s)->typeset & (1 << (_type)))) \
    {\
        (_s)->nu = 0; \
        continue; \
    } \
    (_c)->type = (_type); \
    (_c)->flags = (_dval); \
    if (is_done((_s), (_c), (_h))) \
        goto next; \
} while (0)

enum hid_ret_value hid_get_item(struct hid_data *s, struct hid_item *h)
{
    struct hid_item *c = &s->cur;
    juint_t bTag, bType, bSize;
    jint32_t dval;
    juint8_t *p;

    if (is_multimax(s, c, h))
        goto next;
    while (1)
    {
        p = s->p;
        if (p >= s->end)
            goto end_desc;

        parse_item_header(&p, &bSize, &bTag, &bType, &dval);
        
        s->p = p;

        switch (bType) 
        {
        case HID_TYPE_MAIN :
            switch (bTag) 
            {
            case HID_TAG_MAIN_INPUT:
                HANDLE_REPORT_TAG(s, hid_input, c, h, dval);
                break;
            case HID_TAG_MAIN_OUTPUT:
                HANDLE_REPORT_TAG(s, hid_output, c, h, dval);
                break;
            case HID_TAG_MAIN_COLLECTION:
                c->type = hid_collection;
                c->collection = dval;
                c->collevel++;
                *h = *c;
                hid_clear_local(c);
                s->nu = 0;
                goto next;
            case HID_TAG_MAIN_FEATURE:
                HANDLE_REPORT_TAG(s, hid_feature, c, h, dval);
                break;
            case HID_TAG_MAIN_END_COLLECT:
                c->type = hid_endcollection;
                c->collevel--;

                if ((c->loc.pos != 8) || !c->report_ID)
                    c->report_size = c->loc.pos;
                    
                *h = *c;
                hid_clear_local(c);
                s->nu = 0;
                goto end_collection;
            default:
                DBG_E(DHOST_HID_LIB,("hid_get_item: Unknown Main bTag=%d\n", 
                    bTag));
                break;
            }
            break;
        case HID_TYPE_GLOBAL:
            parse_global_item(bTag, dval, s, c);
            break;
        case HID_TYPE_LOCAL:
            parse_local_item(bSize, bTag, s, c, dval);
            break;
        default:
            DBG_E(DHOST_HID_LIB,("hid_get_item: Unknown bType=%d\n", bType));
            break;
        }
    }

end_desc:

    if ((c->loc.pos != 8) || !c->report_ID)
        c->report_size = c->loc.pos;
    return hid_end_report;
    
end_collection:
    return hid_end_collection;

next:
    return hid_end_item;
}

jint_t hid_report_count(void *buf, juint16_t len)
{
    struct hid_data *d;
    struct hid_item h;
    jint_t report_count = 1;

    d = hid_start_parse(buf, len, 1<<hid_endcollection); 
    while (hid_get_item(d, &h))
        ; /* Parse all report descriptor */
    
    if (d->report_count)
        report_count = d->report_count;
    hid_end_parse(d);

    return report_count;
}

jint_t hid_report_id_by_index(void *buf, juint16_t len, juint8_t index)
{
    struct hid_data *d;
    struct hid_item h;
    jint_t report_id = 0;

    DBG_V(DHOST_HID_LIB, ("Looking for report id of index %d\n", index));

    d = hid_start_parse(buf, len, 1<<hid_input | 1<<hid_output | 
        1<<hid_feature); 
    while (hid_get_item(d, &h) && d->report_count < (index + 1))
        ; /* Parse all report descriptor */

    hid_end_parse(d);
    report_id = h.report_ID;
    DBG_V(DHOST_HID_LIB, ("report id for index %d is %d\n", index, report_id));

    return report_id;
}

jint_t hid_report_size(void *buf, juint16_t len, enum hid_type k, juint8_t rep_id)
{
    struct hid_data *d;
    struct hid_item h;
    jint_t size = 0, done = 0;
    jbool_t is_report = FALSE;
    
    DBG_V(DHOST_HID_LIB, ("Looking for size of report %s id %d\n", 
        k == hid_input ? "INPUT" : k == hid_output ? "OUTPUT" : "FEATURE", 
        rep_id));
    
    d = hid_start_parse(buf, len, 1<<k); 
    while (!done)
    {
        switch (hid_get_item(d, &h))
        {
        case hid_end_report: /* End report descriptor */
            done = 1;
            break;
        case hid_end_item:
            if (h.report_ID == rep_id)
                is_report = TRUE;
            else if (is_report)
                done = 1;
            break;
        case hid_end_collection: /* End Collection */
        default:
            break;
        }
    }

    hid_end_parse(d);
    if (is_report)
        size = (h.report_size + 7) / 8;  

    DBG_V(DHOST_HID_LIB, ("The size of report %s id %d is %d\n", 
        k == hid_input ? "INPUT" : k == hid_output ? "OUTPUT" : "FEATURE", 
        rep_id, size));

    return size;
}

jresult_t hid_get_field_location(void *desc, juint16_t size, juint32_t u,
    enum hid_type k, juint8_t report_id, struct hid_location *loc,
    juint32_t *flags, struct hid_limits *lim)
{
    struct hid_data *d;
    struct hid_item h;
    jint_t done = 0, found = 0;

    j_memset(&h, 0, sizeof(struct hid_item));
    d = hid_start_parse(desc, size, 1<<k); 
    while (!done)
    {
        switch (hid_get_item(d, &h))
        {
        case hid_end_report: /* End of descriptor (not found) */
            done = 1;
            break;
        case hid_end_item:
            if ((h.report_ID == report_id) && (h.type == k) &&
                !(h.flags & HIO_CONST) && ((h.usage == u) || 
                /* In case the report field is array */
                (!(h.flags & HIO_MASK) && (u >= h.usage_minimum) && 
                (u <= h.usage_maximum)))) 
            {
                if (loc)
                    *loc = h.loc;
                if (flags)
                    *flags = h.flags;
                if (lim)
                {
                    lim->usage = h.usage;
                    lim->logical_min = h.logical_minimum;
                    lim->logical_max = h.logical_maximum;
                    lim->physical_min = h.physical_minimum;
                    lim->physical_max = h.physical_maximum;
                    lim->usage_min = h.usage_minimum;
                    lim->usage_max = h.usage_maximum;
                    lim->unit = h.unit;
                }

                found = 1;
                done = 1;
            }
            DBG_X(DHOST_HID_LIB, ("Report id: %d report type: %d flags 0x%x "
                "usage 0x%x log_min 0x%x log_max 0x%x\n", h.report_ID, h.type, 
                h.flags, h.usage, h.logical_minimum, h.logical_maximum));
            break;
        case hid_end_collection:
        default:
            break;
        }
    }
    hid_end_parse(d);

    if (!found)
        goto NotFound;
    
    return 0;

NotFound: 
    if (loc)
        loc->size = 0;
    DBG_I(DHOST_HID_LIB, ("usage 0x%x + type 0x%x not found in report id %d\n", 
        u, k, report_id));
    return JENOENT;
}

static void copybit(juint8_t *dest, juint8_t dbit, juint8_t source, juint8_t sbit)
{
    if (source & (1<<sbit))
        *dest |= 1<<dbit;
    else
        *dest &= ~(1<<dbit);
    DBG_X(DHOST_HID_LIB, ("dest = %x source = %x\n", *dest, source));
}

juint32_t hid_get_data(juint8_t *buf, struct hid_location *loc, 
    struct hid_limits *lim)
{
    juint32_t hpos = loc->pos;
    juint32_t hsize = loc->size;
    juint32_t data;
    juint8_t *tmp_data = (juint8_t *)&data;
    juint32_t s, d;

    DBG_X(DHOST_HID_LIB,("hid_get_data: loc %d/%d\n", hpos, hsize));

    if (hsize == 0)
        return 0;

    data = 0;
    for (s = hpos, d = 0; s < hpos + hsize; s++, d++)
        copybit(&tmp_data[d/8], d%8, buf[s/8], s%8);

    /* Insure to return data in correct format 
     * By USB spec the data passed on the bus is little endian */
    data = le32toh(data);
    
    if (lim->logical_min < 0)
    {
        juint_t shift_off = (sizeof(juint32_t) * 8) - hsize;
        /* Sign extend */
        data = ((jint32_t)data << shift_off) >> shift_off;
    }
   
    DBG_V(DHOST_HID_LIB,("hid_get_data: loc %d/%d = 0x%x(%u/%d)\n",
        loc->pos, loc->size, data, data, data));
    return data;
}

jresult_t hid_set_data(juint8_t *buf, struct hid_location *loc, 
    struct hid_limits *lim, juint32_t data)
{
    juint32_t hpos = loc->pos;
    juint32_t hsize = loc->size;
    juint32_t s,d;
    juint8_t *tmp_data;
    jint_t i;

    DBG_X(DHOST_HID_LIB, ("hid_set_data: loc %d/%d data:%x\n", hpos, hsize, 
        data));

    if (!hsize)
        return JEINVAL;

    i = (lim->logical_min < 0) ?
        (((jint32_t)data < lim->logical_min) || 
            ((jint32_t)data > lim->logical_max)) :
        ((data < (juint32_t)lim->logical_min) ||
            (data > (juint32_t)lim->logical_max));
    
    if (i)
        return JEINVAL;

    data = htole32(data);
    tmp_data = (juint8_t *)&data;
   
    for (d = hpos, s = 0; d < hpos + hsize; d++, s++)
        copybit(&buf[d/8], d%8, tmp_data[s/8], s%8);
    
    return 0;
}

jbool_t hid_is_collection(void *desc, juint16_t size, juint32_t usage)
{
    struct hid_data *hd;
    struct hid_item hi;
    jbool_t err;

    hd = hid_start_parse(desc, size, hid_input);
    if (hd == NULL)
        return 0;

    err = ((hid_get_item(hd, &hi) == hid_end_item) && 
        hi.type == hid_collection &&
        hi.usage == usage);
        
    hid_end_parse(hd);
    return (err);
}
