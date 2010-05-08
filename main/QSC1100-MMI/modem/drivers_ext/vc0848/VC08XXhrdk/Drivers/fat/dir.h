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


#ifndef __DIR_H
#define __DIR_H


#ifdef __cplusplus
extern "C" {
#endif

#if !(F_LONGFILENAME)

unsigned char _f_getdirsector(unsigned long);
#if F_FINDING
unsigned char _f_findfilewc(char *,char *,F_POS *, F_DIRENTRY **, unsigned char); 
#else
unsigned char _f_findfilewc(char *,char *,F_POS *, F_DIRENTRY **); 
#endif
unsigned char _f_findpath(F_NAME *,F_POS *); 
unsigned long _f_getdecluster(F_DIRENTRY *);

#if F_WRITING
unsigned char _f_writedirsector(void);
void _f_setdecluster(F_DIRENTRY *,unsigned long);
unsigned char _f_addentry(F_NAME *,F_POS *,F_DIRENTRY **);
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif
