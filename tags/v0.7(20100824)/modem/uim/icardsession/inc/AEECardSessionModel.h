#ifndef AEECARDSESSIONMODEL_H
#define AEECARDSESSIONMODEL_H

/*============================================================================
FILE:  AEECardSessionModel.h

SERVICES:  AEE ICardSession Model Interface

GENERAL DESCRIPTION:
        Base level definitions, typedefs, etc.

        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/icardsession/rel/07H1_2/inc/AEECardSessionModel.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/03/07   sk     Fixed compilation warning
11/17/07   sk     Initial release.
=============================================================================*/


#include "AEE.h"
#include "AEECardSession.h"

/**********************************************************************

 ICardSessionModel Interface

*********************************************************************/

/*===========================================================================

                    TYPE DEFINITIONS

===========================================================================*/
/* Event Indicators */
/* EVENT AEECARDSESSION_MODEL_CARD_INSERTED: 
   Event payload -- App Info 
*/
#define AEECARDSESSION_MODEL_CARD_INSERTED   1

/* EVENT AEECARDSESSION_MODEL_APP_SELECTED: 
   Event payload -- Selected App Info
*/
#define AEECARDSESSION_MODEL_APP_SELECTED    2

/* EVENT AEECARDSESSION_MODEL_CARD_ERROR: 
   Event payload -- None 
*/
#define AEECARDSESSION_MODEL_CARD_ERROR      3

/* EVENT AEECARDSESSION_MODEL_CARD_READY: 
   Event payload -- App Info, Pin1 status, Pin2 status 
*/
#define AEECARDSESSION_MODEL_CARD_READY      4

/* EVENT AEECARDSESSION_MODEL_CARD_REFRESH: 
   Event payload -- Mode, Stage, File List 
*/
#define AEECARDSESSION_MODEL_CARD_REFRESH    5

/* EVENT AEECARDSESSION_MODEL_SESSION_CLOSED: 
   Event payload -- None 
*/
#define AEECARDSESSION_MODEL_SESSION_CLOSED  6


/* Refresh Mode */
#define AEECARDSESSION_MODEL_REFRESH_RESET              1
#define AEECARDSESSION_MODEL_REFRESH_NAA_INIT           2
#define AEECARDSESSION_MODEL_REFRESH_NAA_INIT_FCN       3
#define AEECARDSESSION_MODEL_REFRESH_FCN                4
#define AEECARDSESSION_MODEL_REFRESH_NAA_INIT_FULL_FCN  5
#define AEECARDSESSION_MODEL_REFRESH_APP_RESET          6
#define AEECARDSESSION_MODEL_REFRESH_3G_SESSION_RESET   7


/* Refresh Stage  */
#define AEECARDSESSION_MODEL_REFRESH_STAGE_START                       1
#define AEECARDSESSION_MODEL_REFRESH_STAGE_IN_PROGRESS_APP_DEACTIVATED 2
#define AEECARDSESSION_MODEL_REFRESH_STAGE_IN_PROGRESS_APP_ACTIVATED   3
#define AEECARDSESSION_MODEL_REFRESH_STAGE_END_SUCCESS                 4
#define AEECARDSESSION_MODEL_REFRESH_STAGE_END_FAILED                  5
#define AEECARDSESSION_MODEL_REFRESH_STAGE_OK_TO_INIT                  6
#define AEECARDSESSION_MODEL_REFRESH_STAGE_OK_TO_FCN                   7


/* NMASK_CARDSESSION_PIN_EVENT's event indication */
#define AEECARDSESSION_MODEL_PIN_ENABLED_NOT_VERIFIED  1
#define AEECARDSESSION_MODEL_PIN_DISABLED              2
#define AEECARDSESSION_MODEL_PIN_ENABLED_VERIFIED      3
#define AEECARDSESSION_MODEL_PIN_UNBLOCKED             4
#define AEECARDSESSION_MODEL_PIN_CHANGED               5
#define AEECARDSESSION_MODEL_PIN_BLOCKED               6
#define AEECARDSESSION_MODEL_PIN_PERM_BLOCKED          7


/* PIN Replacement for PIN EVENT */
#define AEECARDSESSION_MODEL_PIN_REPLACE_NOT_APPLICABLE     1
#define AEECARDSESSION_MODEL_PIN_REPLACED_BY_UNIVERSAL      2
#define AEECARDSESSION_MODEL_PIN_NOT_REPLACED_BY_UNIVERSAL  3

typedef struct ICardSessionModel ICardSessionModel;

AEEINTERFACE(ICardSessionModel)
{
   INHERIT_IQueryInterface(ICardSessionModel);
};

/*=============================================================================
FUNCTION: ICARDSESSIONMODEL_QueryInterface

DESCRIPTION:  

PARAMETERS:
   *p  :  [Input] ICardSessionModel interface pointer
    id :  [Input] Class ID that application wants to obtain a query interface on
  **ppo:  [Input/Output] Return interface pointer for the Class ID provided in
                        input parameter "id"

PROTOTYPE:

  int   ICARDSESSIONMODEL_QueryInterface(ICardSessionModel     *p, 
                                         AEECLSID               id, 
                                         void                  **ppo);

RETURN VALUE:
  SUCCESS    - Status information is valid
  EBADPARM   - Bad parameter
  EUNSUPPORT - Not supported

COMMENTS:
  The client is **should not**  this function for creating Model Interface
  to ICardSession. Instead the client should use ICARDSESSION_QueryInterface()
  and provide the model class id AEECLSID_CARDSESSION_MODEL. This API has been 
  added to fix *warnings* that appear when making a build.
  
SIDE EFFECTS:
  None

SEE ALSO:
  None

=============================================================================*/
static __inline int ICARDSESSIONMODEL_QueryInterface
  (ICardSessionModel     *p, 
   AEECLSID               id, 
   void                  **ppo)
{
   return AEEGETPVTBL((p),ICardSessionModel)->QueryInterface(p,
                                                             id,
                                                             ppo);
}
#endif    /* #define AEECARDSESSIONMODEL_H */

