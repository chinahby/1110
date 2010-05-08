#ifndef DSMIP_PARSE_H
#define DSMIP_PARSE_H
/*===========================================================================

                   SNOOP HEADER FILE

DESCRIPTION

 The Data Services Snoop Header File. Contains shared variables and enums, as
 well as declarations for functions.

Copyright (c) 2000,2001 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_parse.h_v   1.4   12 Jul 2002 15:11:54   jeffd  $
  $Header: //source/qcom/qct/modem/data/1x/mip/main/lite/src/dsmip_parse.h#1 $ $DateTime: 2007/11/06 01:07:58 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/07/02   jd      Moved FA info block to dsmip.c
08/14/01   jd      added from_ip argument to mip_parse_msg()
07/17/01   jd      changed mip_parse_msg argument types for memory efficiency
06/11/01   jd      dsmip_parse_msg accepts arbitrary pointer to data to parse
10/20/00   na/mvl  Created module.
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                      PUBLIC FUNCTION DECLARATIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/


/*===========================================================================

FUNCTION MIP_PARSE_MSG

DESCRIPTION
  Processes a received message passed from the mipio layer

PARAMETERS
  prot - rx'd message protocol (IPPROTO_ICMP or IPPROTO_UDP)
  data - pointer to rx'd message
  len  - length of rx'd message

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mip_parse_msg
(
  uint8   idx,                            /* connection on which msg rx'd  */
  void *  data,                           /* data received                 */
  uint32  from_ip,                        /* sender's IP address length    */
  uint16  len                             /* data length                   */
);

#endif /* DSMIP_PARSE_H */
