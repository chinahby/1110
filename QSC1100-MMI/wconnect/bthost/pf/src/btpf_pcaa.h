/*****************************************************************************
***
***  TITLE
***  MT OBEX API Header
***
***
***  DESCRIPTION
***  This contains the Interface to a Multi Transport AMSS OBEX Implementation.
***
***
***  Copyright (c) 2004 QUALCOMM UK Limited.
***  All Rights Reserved. QUALCOMM Proprietary.
***
***  Export of this technology or software is regulated by the U.S.
***  Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
***  $Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/btpf_pcaa.h#1 $
***
***  when       who     what, where, why
***  --------   ---     --------------------------------------------------------
***  22/10/05   JH      Replaced FEATURE_PCAA with FEATURE_PCAA_SIRIUS
***  19/09/05   JH      Added changes to BT mainline
***  14/02/05   ABu     Added FEATURE_PCAA_SIRIUS for Sirius project, PC Application
***                     Agent is a server to support Sirius Mobile Phone Manager
***                     or MPM running on PC
*****************************************************************************/

#ifndef _BTPF_PCAA_H
#define _BTPF_PCAA_H

#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS) */

#endif /* _BTPF_PCAA_H */
