/****************************************************************************
 * hfat_intf.h
 *
 * This file exposes all the glue functions needed by HFAT
 *
 * Copyright (C) 2006,2008 QUALCOMM, Inc.
 *
 * Verbose Description
 *
 ***************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/hfat/main/latest/inc/hfat_interface.h#3 $ $DateTime: 2008/06/05 13:57:19 $ $Author: richardp $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2008-06-04   ebb   Switched #include order to prevent compiler warning.
2008-05-23   rp    Introducing /inc folder as per new SCM.
2006-07-13   sch   create

===========================================================================*/
#ifndef _HFAT_INTF_H
#define _HFAT_INTF_H

#include "hfat_udefs_f.h"
#include "hfat_api_f.h"

F_DRIVER* hfat_device_init (unsigned long);
extern int fn_getsize_from_fp (F_FILE *fp, uint32 *numbytes);

#endif
