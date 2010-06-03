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

#ifndef _JHID_UTIL_H_
#define _JHID_UTIL_H_

typedef __PACKED_PRE__ struct usb_hid_descriptor 
{
    juint8_t             bLength;
    juint8_t             bDescriptorType;
    juint8_t             bcdHID[2];
    juint8_t             bCountryCode;
    juint8_t             bNumDescriptors;
    __PACKED_PRE__ struct 
    {
        juint8_t         bDescriptorType;
        juint8_t         wDescriptorLength[2];
    } __PACKED_POST__ descrs[1];
} __PACKED_POST__ usb_hid_descriptor_t;

usbd_status     usbd_set_protocol(usbd_interface_handle dev, juint16_t report);
usbd_status     usbd_get_protocol(usbd_interface_handle dev, juint8_t *data);
usbd_status     usbd_get_report_descriptor(usbd_device_handle dev, 
    juint16_t ifcno, juint16_t size, void *d, juint8_t report_id);
struct usb_hid_descriptor *usbd_get_hid_descriptor(usbd_interface_handle ifc);
usbd_status     usbd_set_report(usbd_interface_handle iface, juint8_t type, 
    juint8_t id, void *data, juint16_t len);
usbd_status     usbd_set_report_pipe(usbd_interface_handle iface, 
    usbd_pipe_handle pipe, void *data, juint16_t len);
usbd_status     usbd_set_report_async(usbd_interface_handle iface, juint8_t type,
    juint8_t id, void *data, juint16_t len);
usbd_status     usbd_get_report(usbd_interface_handle iface, juint8_t type, 
    juint8_t id, void *data, juint16_t len);
usbd_status     usbd_set_idle(usbd_interface_handle iface, juint8_t duration,
    juint8_t id);
usbd_status     usbd_get_idle(usbd_interface_handle iface, juint8_t id,
    juint8_t *duration);
usbd_status     usbd_read_report_desc(usbd_interface_handle ifc, void **descp,
    juint16_t *sizep, juint8_t report_id);
#endif

