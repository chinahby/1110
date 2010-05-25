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
#ifndef __DRV_H
#define __DRV_H

#ifdef __cplusplus
extern "C" {
#endif

#include "drv_sd.h"

unsigned char _f_checkstatus(void);
unsigned char _f_readsector(void *, unsigned long); 

#if F_WRITING
unsigned char _f_writesector(void *, unsigned long);
#endif

#if F_MULTIREAD
unsigned char _f_readmultisector(void *_tdata, unsigned long sector,unsigned long count);
#endif
#if (F_WRITING && F_MULTIWRITE)
unsigned char _f_writemultisector(void *_tdata, unsigned long sector,unsigned long count);
#endif


#ifdef __cplusplus
}
#endif

#endif
