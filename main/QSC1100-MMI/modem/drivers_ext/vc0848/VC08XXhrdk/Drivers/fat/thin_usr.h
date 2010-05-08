#ifndef _THIN_USR_H
#define _THIN_USR_H

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

/**************************************************************************
**
**  FAT thin user settings
**
**************************************************************************/
#define F_GETVERSION	1	/*defines f_getversion*/

#define F_FORMATTING	1	/*defines f_format, f_hardformat*/
#define F_SETLABEL		1	/*defines f_setlabel*/
#define F_GETLABEL		1	/*defines f_getlabel*/
#define F_GETFREESPACE	1	/*defines f_getfreespace*/

#define F_CHDIR			1	/*defines f_chdir*/
#define F_MKDIR			1	/*defines f_mkdir*/
#define F_RMDIR			1	/*defines f_rmdir*/

#define F_RENAME		1	/*defines f_rename*/

#define F_DELETE		1	/*defines f_delete*/
#define F_GETTIMEDATE	1	/*defines f_gettimedate*/
#define F_SETTIMEDATE	1	/*defines f_settimedate*/
#define F_GETATTR		1	/*defines f_getattr*/
#define F_SETATTR		1	/*defines f_setattr*/
#define F_FILELENGTH	1	/*defines f_filelength*/
#define F_FINDING		1	/*defines f_findfirst, f_findnext*/
#define F_TELL			1	/*defines f_tell*/
#define F_GETC			1	/*defines f_getc*/
#define F_PUTC			1	/*defines f_putc*/
#define F_REWIND		1	/*defines f_rewind*/
#define F_EOF			1	/*defines f_eof*/
#define F_SEEK			1	/*defines f_seek*/
#define F_WRITE			1	/*defines f_write*/
#define F_TRUNCATE		1	/*defines f_truncate*/
#define F_SETEOF		1	/*defines f_seteof*/
#define F_DUPHANDLE		1	/*defines f_duphandle*/
#define F_COMBINE 		1 	/*defines f_combine*/

#define F_DIRECTORIES	1	/*enables subdirectories*/
#define F_CHECKNAME		1	/*enables name checking*/

#define F_WRITING		1	/*enables/removes everything related to write*/

#define F_MULTIWRITE 	1 	/*enables multi-sector writing if device supported*/ 	
#define F_MULTIREAD 	1 	/*enables multi-sector reading if device supported*/

#define F_SEEKCACHE 	1 	/*enables seek cache*/

#define F_FAT12			1	/*enables FAT12 usage*/
#define F_FAT16			1	/*enables FAT16 usage*/
#define F_FAT32			1	/*enables FAT32 usage*/

#define F_MAXFILES		8	/*maximum number of files */

#define F_LONGFILENAME	1	/* 0 - 8+3 names   1 - long file names  */

#if F_LONGFILENAME
#define F_MAXLNAME 		(256*2) /*maximum length of the file name in long file name mode */
								//bytes, 256 Unicode char stream, including 1 double-null terminator at the end
#define F_MAXPATH 		(260*2) /*considered unsigned char in thin version*/
								//bytes, 260 Unicdoe char stream, including 1 double-null terminator at the end
#else
#define F_MAXPATH		64		/*considered unsigned char in thin version*/	
#endif


#include "fat_thin.h"


#ifdef __cplusplus
}
#endif

#endif

