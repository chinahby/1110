#ifndef __MI_QCSBL_H__
#define __MI_QCSBL_H__
/*===========================================================================

                MIBIB QCSBL Page Definitions

DESCRIPTION
  This header file gives the external definition of the structures used
  to find the QC-SBL partitionn
  
Copyright(c) 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

    This section contains comments describing changes made to this file.
    Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/drivers/mibib/MSM_MIBIB.02.00/miqcsbl.h#7 $ $DateTime: 2006/03/08 14:23:33 $ $Author: rtilak $

   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/06/04   drh     Created.

===========================================================================*/

/* Definitions of magic numbers used in the first page of each
 * of the QC-SBL and OEM-SBL blocks.  We must mark these blocks
 * with some identifier because the PBL does not have enough
 * knowledge to know how to find bad blocks.
 */

#define MI_QCSBL_MAGIC1           0x5FE85DDF
#define MI_QCSBL_MAGIC2           0x5264CEBC

#endif /* __MI_QCSBL_H__ */


