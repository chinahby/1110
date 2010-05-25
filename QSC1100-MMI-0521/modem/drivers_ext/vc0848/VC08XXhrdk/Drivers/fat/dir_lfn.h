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


#ifndef __DIR_LFN_H
#define __DIR_LFN_H


#ifdef __cplusplus
extern "C" {
#endif

#if F_LONGFILENAME

unsigned char _f_getdirsector(unsigned long);
#if F_FINDING
unsigned char _f_findlfilewc(char *,F_POS *, F_DIRENTRY **, F_LFNINT *, unsigned char); 
#else
unsigned char _f_findlfilewc(char *,F_POS *, F_DIRENTRY **, F_LFNINT *); 
#endif
unsigned char _f_findpath(F_NAME *,F_POS *); 
unsigned long _f_getdecluster(F_DIRENTRY *);
#if (F_DELETE || F_RMDIR || F_WRITING || F_DIRECTORIES || F_GETLABEL || F_SETLABEL)
F_FILE *_f_opendir(unsigned long); 
unsigned char _f_readdir(F_DIRENTRY *,F_FILE *); 
unsigned char _f_writedir(F_DIRENTRY *,F_FILE *); 
unsigned char _f_seekdir(F_FILE *,long,unsigned char); 
unsigned char _f_closedir(F_FILE *, unsigned char);
#endif
#if F_WRITING
unsigned char _f_writedirsector(void);
void _f_setdecluster(F_DIRENTRY *,unsigned long);
unsigned char _f_addentry(F_NAME *, F_POS *, F_DIRENTRY **, F_LFNINT *);
void _f_initentry(F_DIRENTRY *,char *, char *);
#endif
#if ((F_DELETE || F_RMDIR) && F_WRITING)
unsigned char _f_removelfn(unsigned long, F_LFNINT *); 
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif
