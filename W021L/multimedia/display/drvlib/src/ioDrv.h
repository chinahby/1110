#ifndef IO_DRV_H
#define IO_DRV_H

/*============================================================================

  Driver Library Header File

  DESCRIPTION
  This file contains the definitions needed for the driver library.

  Copyright (c) 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================

  EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/Display/drvLib/main/latest/src/ioDrv.h#1 $ $DateTime: 2008/08/05 10:35:09 $ $Author: yuanz $

when      who    what, where, why
--------  -----  ----------------------------------------------------------
05/21/04  cjr    Initial code
============================================================================*/

//============================================================================
//                        INCLUDE FILES
//============================================================================

#define MAXDRV_INDEX 32
#define MAX_MDRV_STR_LEN 32

typedef struct _ioDrv_Struct_
{
    char str[MAX_MDRV_STR_LEN];
    int (*write) (char *, int);
    int (*read) (char *, int);
    int (*ioctl) (int, void *);
    int (*init) (void);
    int (*shutdown) (void);
} IODRV_STRUCT;

static IODRV_STRUCT masterDrv[MAXDRV_INDEX];
static int drvIndex = 0;

#endif /* IO_DRV_H */
