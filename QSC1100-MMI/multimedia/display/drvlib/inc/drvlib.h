#ifndef DRV_LIB_H
#define DRV_LIB_H

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

  $Header: //source/qcom/qct/multimedia/Display/drvLib/main/latest/inc/drvlib.h#2 $ $DateTime: 2008/09/09 04:34:20 $ $Author: gauravs $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 04/04/07  gmy    Lint cleanup & replacement of banned lib api interfaces
 04/02/07  gmy    Replaced ret code magic numbers with constant labels
 05/28/04  cjr    Initial code
============================================================================*/

//============================================================================
//                        INCLUDE FILES
//============================================================================

//============================================================================
//                           MACROS
//============================================================================
/* Error handling definitions. Since the return code of these
 * functions is int, do not use an enum here. */
#define DRV_RET_FAILURE   (-1)  // drvLib operation failed
#define DRV_RET_OK        (0)   // drvLib operation succeeded or no installed
                                //        driver handler available. 
                                // 
                                // NOTE: This return code does not cannote
                                //       successful servicing of requested
                                //       operation by installed driver.

/* File Descriptor definitions */
#define DRV_FD_INVALID    (-1)


//============================================================================
//                     FUNCTION PROTOTYPES
//============================================================================
int drv_install(const char * str,
        int (* write)(char *, int),
          int (*read) (char *, int),
          int (*ioctl) (int, void *),
          int (*init) (void),
          int (*shutdown) (void));
void drv_show(void);
int drv_open(const char * str);
int drv_init(int fd);
int drv_write(int fd, char * buf, int size);
int drv_read(int fd, char * buf, int size);
int drv_ioctl(int fd, int cmd, void *arg);
int drv_shutdown(int fd);

#endif /* DRV_LIB_H */

