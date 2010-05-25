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


#ifndef __FAT_H
#define __FAT_H


#ifdef __cplusplus
extern "C" {
#endif

unsigned char _f_getfatsector(unsigned long);
unsigned char _f_getclustervalue(unsigned long,unsigned long *);
void _f_clustertopos(unsigned long, F_POS *);
unsigned char _f_getcurrsector(F_FILE *,void *);
unsigned char _f_getmultisector(F_FILE *f, void *ptr, unsigned int max, unsigned long *size);


#if F_WRITING
unsigned char _f_writefatsector(void);
unsigned char _f_setclustervalue(unsigned long,unsigned long);
unsigned char _f_alloccluster(unsigned long *);
unsigned char _f_dobadblock(F_FILE *);
unsigned char _f_removechain(unsigned long);
#endif

#ifdef __cplusplus
}
#endif

#endif
