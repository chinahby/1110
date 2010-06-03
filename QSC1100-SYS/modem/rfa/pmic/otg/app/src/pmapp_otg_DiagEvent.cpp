/*! \file 
*  \n
*  \brief  pmapp_otg_DiagEvent.cpp ----- Diag Events related class implementation
*  \details This header file contains implementation for diag event used by the USB-OTG application 
*     files developed for the Qualcomm Power Management Chip Set.
*  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/otg2/main/latest/app/src/pmapp_otg_DiagEvent.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/08   aab      Created

========================================================================== */

/* ==========================================================================

INCLUDE FILES FOR MODULE

========================================================================== */
#include "pmapp_otg_DiagEvent.h"       /*!< For diag event logging */


#ifdef FEATURE_PMIC_USB_OTG

/* ==========================================================================
DEFINITIONS AND DECLARATIONS FOR MODULE
This section contains definitions for constants, macros, types, variables
and other items needed by this module.
========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
OtgDiagEvent* OtgDiagEvent::mpOtgDiagEvent_Instance=NULL;

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */


/* =======================================================================
**                            Function Definitions
** ======================================================================= */
/**************************************************************************
     class OtgDiagEvent											  
**************************************************************************/
OtgDiagEvent::OtgDiagEvent()
{
}

OtgDiagEvent::~OtgDiagEvent()
{
}



OtgDiagEvent* OtgDiagEvent::GetInstance()
{
	if(NULL == mpOtgDiagEvent_Instance )
	{
		mpOtgDiagEvent_Instance = new OtgDiagEvent;
	}
	return mpOtgDiagEvent_Instance;
}



void OtgDiagEvent::DestroyInstance()
{
	delete mpOtgDiagEvent_Instance;
	mpOtgDiagEvent_Instance = NULL;
}




void OtgDiagEvent::OtgEventHandle(const event_id_enum_type event_id)
{
	event_report(event_id);
}


void OtgDiagEvent::OtgEventHandlePayload(const event_id_enum_type event_id, const uint8 payload_size, void* payload)
{
	event_report_payload(event_id, payload_size,payload);
}


void OtgDiagEvent::OtgEventHandleRemote_a_DevInfo(const event_id_enum_type event_id, const uint16 a_dev_id, const uint16 max_i_ma)
{
	OtgEventRemote_a_DevInfoType remote_a_dev_event_info;

	remote_a_dev_event_info.remote_a_dev_id = a_dev_id;
	remote_a_dev_event_info.max_i_ma = max_i_ma;
	event_report_payload(event_id,sizeof(OtgEventRemote_a_DevInfoType),(void*) &remote_a_dev_event_info);
}


void OtgDiagEvent::OtgEventHandleStateTransition(const event_id_enum_type event_id, const StateEventIdType curr_state_id, const StateEventIdType next_state_id)
{
	OtgStateTransitionEventType stateTransition_payload;
	stateTransition_payload.current_state = curr_state_id;
	stateTransition_payload.next_state    = next_state_id;
	event_report_payload(event_id,sizeof(OtgStateTransitionEventType),(void*) &stateTransition_payload);
}

#endif //FEATURE_PMIC_USB_OTG

