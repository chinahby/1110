/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.











************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#ifndef _JERRNO_H_
#define _JERRNO_H_

#define JEPERM           -1              /* Operation not permitted */
#define JENOENT          -2              /* No such file or directory */
#define JEINTR           -4              /* Interrupted system call */
#define JEIO             -5              /* Input/output error */
#define JENXIO           -6              /* Device not configured */
#define JE2BIG           -7              /* Argument list too long */
#define JEAGAIN          -11             /* Try Again */
#define JENOMEM          -12             /* Cannot allocate memory */
#define JEACCES          -13             /* Permission denied */
#define JEBUSY           -16             /* Device busy */
#define JENODEV          -19             /* Operation not supported by device */
#define JEINVAL          -22             /* Invalid argument */
#define JENOTTY          -25             /* Inappropriate ioctl for device */
#define JEFBIG           -27             /* File too large */
#define JENOSPC          -28             /* No space left on device */
#define ECANCEL         -29             /* Transfer cancelled */
#define JEWOULDBLOCK     JEAGAIN          /* Operation would block */
#define JERESTART        -85             /* restart syscall */
#define JEOPNOTSUPP      -95             /* Operation not supported */
#define JENOTSUP         JEOPNOTSUPP      /* Operation not supported */
#define JETIMEDOUT       -110            /* Operation timed out */
#define JENOMEDIUM       -123            /* No medium found */
#define ESUSPENDED      -200            /* Device is suspended */
#define EUNKNOWN        -1000           /* General error */
#define JETEST_FAILED    -1001           /* Test failed */

#endif
