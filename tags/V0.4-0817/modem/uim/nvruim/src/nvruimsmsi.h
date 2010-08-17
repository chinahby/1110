#ifndef NVRUIMSMS_H
#define NVRUIMSMS_H

/*===========================================================================

                      I N T E R N A L  N V R U I M S M S
                           D E F I N I T I O N S

DESCRIPTION
  This contains all the internal declarations for the NV RUIM SMS module.

Copyright (c) 2000, 2001 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/13/03   wli     Corrected lint errors.
05/18/01   ck      Added support for SMS module to interface with UIM server

===========================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "nv.h"
#include "uim.h"

/*===========================================================================
 
                           DATA DECLARATIONS

===========================================================================*/

/* Size of CDMA Service Table to read off of card */
#define NVRUIMSMS_SERVICETABLE_SIZE 3

/* Service Table Flags */

#define NVRUIMSMS_SERVICETABLE_SMS_ALLOC    0x40

#define NVRUIMSMS_SERVICETABLE_SMS_ACTIVE   0x80

#define NVRUIMSMS_SERVICETABLE_SMSP_ALLOC   0x40

#define NVRUIMSMS_SERVICETABLE_SMSP_ACTIVE  0x80

/* SMSP Flags for parsing SMSP messages */

#define NVRUIMSMS_SMSP_ORIGADDR     0x0001

#define NVRUIMSMS_SMSP_DESTADDR     0x0002

#define NVRUIMSMS_SMSP_DCS          0x0008

#define NVRUIMSMS_SMSP_VALIDPRD     0x0010

#define NVRUIMSMS_SMSP_SVCCATEGORY  0x0020

#define NVRUIMSMS_SMSP_ORIGSUBADDR  0x0040

#define NVRUIMSMS_SMSP_DESTSUBADDR  0x0080

#define NVRUIMSMS_SMSP_BRO          0x0100

#define NVRUIMSMS_SMSP_BDATA        0x0200


/* SMS message status flags */

/* Allocated Entry */
#define NVRUIMSMS_STATUS_USED       0x01  
/* Mobile-Originated Message */
#define NVRUIMSMS_STATUS_MO         0x04
/* Read or Sent Message */
#define NVRUIMSMS_STATUS_NOT_READ   0x02

/* Blank char in SMS data */
#define NVRUIMSMS_SMS_EMPTY_CHAR    0x00
/* Blank char in SMSP data */
#define NVRUIMSMS_SMSP_EMPTY_CHAR   0xFF
#define NVRUIMSMS_SMSP_EMPTY_CHAR2  0x00

/* Values used to identify parameters in an SMSP item: */
#define NVRUIMSMS_SMSP_ID_ORIGADDR     0x02    /* Orig. Address */
#define NVRUIMSMS_SMSP_ID_DESTADDR     0x04    /* Dest. Address */


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION NVRUIMSMS_READ                    

DESCRIPTION
  Reads an SMS message or SMSP item from the RUIM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern uim_rpt_status nvruimsms_read ( nv_cmd_type *nv_cmd_ptr);


/*===========================================================================

FUNCTION NVRUIMSMS_WRITE

DESCRIPTION
  Writes an SMS message or SMSP item to the RUIM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern uim_rpt_status nvruimsms_write ( nv_cmd_type *nv_cmd_ptr);


/*===========================================================================

FUNCTION NVRUIMSMS_BUILDLIST

DESCRIPTION
  Builds an SMS control list

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern uim_rpt_status nvruimsms_buildlist ( nv_cmd_type *nv_cmd_ptr);


/*===========================================================================

FUNCTION NVRUIMSMS_FREE

DESCRIPTION
  Erases an SMS message or SMSP item

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern uim_rpt_status nvruimsms_free ( nv_cmd_type *nv_cmd_ptr);

#endif /* NVRUIMSMS_H */
