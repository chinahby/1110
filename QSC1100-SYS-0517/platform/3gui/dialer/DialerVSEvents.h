#ifndef DIALERVSEVENTS_H
#define DIALERVSEVENTS_H

/*=============================================================================

FILE: DialerVSEvents.h

GENERAL DESCRIPTION: VideoShare Events to Dialer app

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:


(c) COPYRIGHT 2002 - 2006 QUALCOMM Incorporated.
All Rights Reserved.

QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Dialer/DialerVSEvents.h#2 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
08/29/06   RI/JAS  New AVAILABLE/UNAVAILABLE events added
05/31/06   RI      Initial revision.

=============================================================================*/

// Events to Dialer for IMS registration and deregistration.
#define EVT_QP_IMS_REGISTRATION             EVT_USER + 2000
#define EVT_QP_IMS_DEREGISTRATION           EVT_USER + 2001

// Events to DialerApp about VideoShare session.
#define EVT_QP_IMS_VIDEOSHARE_INVITING      EVT_USER + 2002
#define EVT_QP_IMS_VIDEOSHARE_RINGING       EVT_USER + 2003
#define EVT_QP_IMS_VIDEOSHARE_ACTIVE        EVT_USER + 2004
#define EVT_QP_IMS_VIDEOSHARE_CLOSING       EVT_USER + 2005
#define EVT_QP_IMS_VIDEOSHARE_END           EVT_USER + 2006
#define EVT_QP_IMS_VIDEOSHARE_AVAILABLE     EVT_USER + 2007
#define EVT_QP_IMS_VIDEOSHARE_UNAVAILABLE   EVT_USER + 2008

#endif // DIALERVSEVENTS_H

