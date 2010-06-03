#ifndef AEEIOCTL_H_
#define AEEIOCTL_H_

/*=====================================================================
FILE:  AEEIOCtl.h

SERVICES:  AEE IOCtl macros and ids

DESCRIPTION: Declarations and definition of IOCTL macros

PUBLIC CLASSES: Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS: Not Applicable
        
        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================================*/

/*=========================================================================

  The option id in an IOCTL compliant interface uses the following format

  2  bits  - permission/direction (BREWIOCTL_NONE, BREWIOCTL_R. BREWIOCTL_RW)
  14 bits  - size in bytes
  8  bits  - application or interface type (uniquely identifies the app/if)
  8  bits  - ID of the IOCTL
===========================================================================*/

#define BREWIOCTLD_NONE  0x00000000
#define BREWIOCTLD_R     0x40000000
#define BREWIOCTLD_RW    0x80000000



#define DIR_MASK          0xC0000000 //Top 2bits
#define SIZE_SHIFT        16
#define SIZE_MASK         0x00003FFF //Bottom 14 bits

#define BREWIOCTL_TYPE_SHIFT         8
#define TYPE_MASK         0x0000000F

#define ID_MASK           0x000000FF

#define BREWIOCTL_SET(dir, type, id, size)      \
   ((dir  & DIR_MASK)                      |    \
    (size & SIZE_MASK) << SIZE_SHIFT       |    \
    (type & TYPE_MASK) << BREWIOCTL_TYPE_SHIFT       |    \
    (id   & ID_MASK) )

/*=============================================================================
  Use the following macros to create IOCTL option IDs for your interface.
  type: This is the type of your interface. (Ex: BREWIOCTL_SIO_TYPE)
  id:   ID of the option. Start at 1 and increment
  size: Here specify the structure. The sizeof() is applied on the supplied param.
 ================================================================================*/
#define BREWIOCTL(type, id)            BREWIOCTL_SET(BREWIOCTLD_NONE, (type), (id), 0)
#define BREWIOCTL_R(type, id, size)    BREWIOCTL_SET(BREWIOCTLD_R, (type), (id), sizeof(size))
#define BREWIOCTL_RW(type, id, size)   BREWIOCTL_SET(BREWIOCTLD_RW, (type), (id), sizeof(size))

#define BREWIOCTL_SIZE(n)  ( ((n) >> SIZE_SHIFT) & SIZE_MASK)
#define BREWIOCTL_TYPE(n)  ( ((n) >> BREWIOCTL_TYPE_SHIFT) & TYPE_MASK)
#define BREWIOCTL_ID(n)    ( (n) & ID_MASK)
#define BREWIOCTL_DIR(n)   ( (n) & DIR_MASK)


//Definition of application/interface IDs
//This list starts at one. Please do not changes already existing IDs
//If you need a new type, take the next one in the ID space

#define BREWIOCTL_SIO_TYPE   (1)
#define BREWIOCTL_FIFO_TYPE  (2)
#define BREWIOCTL_USBDRIVER_TYPE (3)
#define BREWIOCTL_ANYPORT (4)

#endif //End of File
