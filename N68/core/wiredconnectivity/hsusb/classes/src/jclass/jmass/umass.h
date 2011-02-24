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

#ifndef _UMASS_H_
#define _UMASS_H_

/* JMS Transport side
 * ================== */

/* Called by Umass when command is complete */
void jms_transport_complete(void *context, juint32_t residue, juint16_t status);
  
/* Called by Umass on device attach */
jresult_t jms_transport_attach(void *sc, void *usbd_dev,juint8_t maxlun, 
    juint8_t subclass, void *user_ctx);

/* Called by Umass on device detach */
void jms_transport_detach(void *user_ctx);

/* Called by Umass to handle power management */
jresult_t jms_transport_suspend(void *user_ctx);
void jms_transport_resume(void *user_ctx);

/* Umass side
 * ========== */
#define STATUS_CMD_OK           0       /* Success */
#define STATUS_CMD_UNKNOWN      1       /* Device did not recognize command */
#define STATUS_CMD_FAILED       2       /* Transfer was ok, command failed */
#define STATUS_WIRE_FAILED      3       /* Could not send command */

/* Called by Transport to transfer data */
jint_t umass_transfer(void *dev_h, jint_t lun, unsigned char *cmd, jint_t cmdlen,
    jint_t is_read, juint32_t timeout, void *data, juint32_t data_len, 
    juint8_t flags, juint32_t *actual_data_len, void *context);

jresult_t umass_suspend_self(void *dev_h);
jresult_t umass_resume_self(void *dev_h);

#endif

