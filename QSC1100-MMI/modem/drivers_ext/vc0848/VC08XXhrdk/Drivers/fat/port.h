#ifndef _PORT_H_
#define _PORT_H_

/****************************************************************************
 *
 *            Copyright (c) 2003 by HCC Embedded 
 *
 * This software is copyrighted by and is the sole property of 
 * HCC.  All rights, title, ownership, or other interests
 * in the software remain the property of HCC.  This
 * software may only be used in accordance with the corresponding
 * license agreement.  Any unauthorized use, duplication, transmission,  
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice may not be removed or modified without prior
 * written consent of HCC.
 *
 * HCC reserves the right to modify this software without notice.
 *
 * HCC Embedded
 * Budapest 1132
 * Victor Hugo Utca 11-15
 * Hungary
 *
 * Tel:  +36 (1) 450 1302
 * Fax:  +36 (1) 450 1303
 * http: www.hcc-embedded.com
 * email: info@hcc-embedded.com
 *
 ***************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if F_LONGFILENAME
/* need to be ported depending on system charset setting */
#define F_DBCS 		1 /*0 - SBCS, 1 - DBCS */
#endif

#if (F_FORMATTING && F_WRITING) 
unsigned long f_getrand(unsigned long rand);
#endif
#if F_WRITING
unsigned short f_getdate(void);
unsigned short f_gettime(void);
#endif
#if F_LONGFILENAME
unsigned short f_unichar2oem(unsigned short unichar);
void *f_malloc(unsigned int size);
void f_free(void * ptr);
#endif



/****************************************************************************
 *
 * end of port.h
 *
 ***************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /*_PORT_H_*/

