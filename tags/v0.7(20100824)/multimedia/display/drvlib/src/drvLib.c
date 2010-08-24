/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  Driver IO Library

  GENERAL DESCRIPTION
  This module contains functions and data blocks pertaining to the
  Driver IO Library

  EXTERNALIZED FUNCTIONS

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*============================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/Display/drvLib/main/latest/src/drvLib.c#2 $ $DateTime: 2008/09/09 04:34:20 $ $Author: gauravs $

when      who     what, where, why
--------  -----   ----------------------------------------------------------
04/04/07  gmy     Lint cleanup & replacement of banned lib api interfaces
04/02/07  gmy     Replaced ret code magic numbers with constant labels
04/11/07  cjr     Replaced strncpy() with std_strlcpy() for a secure code 
05/28/04  cjr     Initial code
============================================================================ */

/*============================================================================
INCLUDE FILES FOR MODULE
============================================================================*/
#include "string.h"
#include "AEEStd.h"    /* Definition for basic types and macros */
#include "ioDrv.h"
#include "drvLib.h"

int drv_install(const char * str,
        int (* write)(char *, int),
          int (*read) (char *, int),
          int (*ioctl) (int, void *),
          int (*init) (void),
          int (*shutdown) (void))
{
    int i;

    if ((drvIndex >= MAXDRV_INDEX) || (NULL == str))
       return DRV_RET_FAILURE;

    for (i=0;i<drvIndex;i++)
    {
       if (strcmp(masterDrv[i].str, str) == 0)
          return DRV_RET_OK;
    }

    (void)std_strlcpy((char *)masterDrv[drvIndex].str, 
                      str, 
                      MAX_MDRV_STR_LEN);
    masterDrv[drvIndex].write = write;
    masterDrv[drvIndex].read = read;
    masterDrv[drvIndex].ioctl = ioctl;
    masterDrv[drvIndex].init = init;
    masterDrv[drvIndex].shutdown = shutdown;

    drvIndex++;

    return DRV_RET_OK;
}

void drv_show(void)
{
    int i;

    for (i=0;i<drvIndex;i++)
    {
    /* printf("%s\n", masterDrv[i].str);*/
    }
}

int drv_open(const char * str)
{
    int i;

    for (i=0;i<drvIndex;i++)
    {
    if (strncmp(masterDrv[i].str, str,MAX_MDRV_STR_LEN) == 0)
        return i;
    }

    return DRV_RET_FAILURE;
}

int drv_init(int fd)
{
    if ((fd < 0) || (fd > drvIndex))
        return DRV_RET_FAILURE;

    if (masterDrv[fd].init == NULL)
       return DRV_RET_OK;

    return masterDrv[fd].init();
}

int drv_write(int fd, char * buf, int size)
{
    if ((fd < 0) || (fd > drvIndex))
        return DRV_RET_FAILURE;

    if (masterDrv[fd].write == NULL)
      return DRV_RET_OK;

    return masterDrv[fd].write(buf, size);
}

int drv_read(int fd, char * buf, int size)
{
    if ((fd < 0) || (fd > drvIndex))
        return DRV_RET_FAILURE;

    if (masterDrv[fd].read == NULL)
      return DRV_RET_OK;

    return masterDrv[fd].read(buf, size);
}

int drv_ioctl(int fd, int cmd, void *arg)
{
    if ((fd < 0) || (fd > drvIndex))
        return DRV_RET_FAILURE;

    if (masterDrv[fd].ioctl == NULL)
      return DRV_RET_OK;

    return masterDrv[fd].ioctl(cmd, arg);
}

int drv_shutdown(int fd)
{
    if ((fd < 0) || (fd > drvIndex))
        return DRV_RET_FAILURE;

    if (masterDrv[fd].shutdown == NULL)
        return DRV_RET_OK;

    return masterDrv[fd].shutdown();
}

