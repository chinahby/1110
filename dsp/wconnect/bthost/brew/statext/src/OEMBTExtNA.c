/*===========================================================================

FILE:      OEMBTExtNA.c

SERVICES:  BlueTooth Service Discovery QCT extension

GENERAL DESCRIPTION: BREW interface to the Bluetooth Network Access

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2003-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtNA.c#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

#include "OEMFeatures.h"

#if defined(FEATURE_IBT)
#include "bt.h"
#include "msg.h"
#include "rdevmap.h"

#include "AEEBTExt_priv.h"
#include "OEMBTExtNA.h"

//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Type definitions
//==========================================================================
typedef struct OEMBTExtNAobj_struct
{
  bt_app_id_type    m_appId;
  ACONTEXT*         m_pac;
  IBTExtNotifier*   m_pNotifier;
} OEMBTExtNAobj_t;

//==========================================================================
//   Function prototypes
//==========================================================================

// helper functions
static void OEMBTExtNA_EventCallback(struct bt_ev_msg_struct* bt_ev_msg_ptr);

//==========================================================================
//   Static data
//==========================================================================

static OEMBTExtNAobj_t* gpOEMBTExtNAobj = NULL;

//==========================================================================
//   Public Funtions
//==========================================================================

/*===========================================================================

Function:  OEMBTExtNA_New()

Description:
   This is called from AEE when an app tries to create an instance of a
   class ID associated with this module.
   There can only be one of these interfaces at a time.

Parameters:
   IBTExtNA *pParent: Pointer to IBTExtNA allocated by AEE

Return Value:  SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object

Componts:      None

Side Effects:  Allocates app-associated memory.

===========================================================================*/
int OEMBTExtNA_Init(IBTExtNA* pParent)
{
  OEMBTExtNAobj_t* pNew;

  if ( gpOEMBTExtNAobj != NULL )
  {
    // all ready created the singleton object
    return EFAILED;
  }

  // allocate the object
  pNew = MALLOCREC( OEMBTExtNAobj_t );

  if ( pNew == NULL )
  {
    return ENOMEMORY;
  }

  if ( AEEHandle_To( &gOEMBTExtHandleList, (OEMINSTANCE*)pNew,
                     &pParent->m_hBT ) == FALSE )
  {
    FREE( pNew );
    return EFAILED;
  }

  // init BT layer
  pNew->m_appId     = BT_APP_ID_NULL;
  pNew->m_pNotifier = NULL;
  pNew->m_pac       = NULL;

  pNew->m_appId = bt_cmd_ec_get_app_id_and_register( OEMBTExtNA_EventCallback );
  if ( pNew->m_appId == BT_APP_ID_NULL )
  {
    OEMBTExtNA_Release( pParent );
    return EFAILED;
  }

  if ( ISHELL_CreateInstance( pParent->m_pIShell, AEECLSID_BLUETOOTH_NOTIFIER, 
                              (void**) &pNew->m_pNotifier ) != SUCCESS )
  {
    OEMBTExtNA_Release( pParent );
    return EFAILED;
  }

  pNew->m_pac     = AEE_GetAppContext();
  gpOEMBTExtNAobj = pNew;

  return SUCCESS;
}


// IBase (see AEE.h)
int OEMBTExtNA_Release( IBTExtNA* pParent )
{
  OEMBTExtNAobj_t* pMe;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }

  if ( pMe != gpOEMBTExtNAobj )
  {
    return EBADPARM;
  }

  gpOEMBTExtNAobj = NULL;
  if ( pMe->m_appId != BT_APP_ID_NULL )
  {
    (void)bt_cmd_ec_free_application_id( pMe->m_appId );
    pMe->m_appId = BT_APP_ID_NULL;
  }

  if ( pMe->m_pNotifier != NULL )
  {
    IBTEXTNOTIFIER_Release( pMe->m_pNotifier );
    pMe->m_pNotifier = NULL;
  }
  pMe->m_pac = NULL;

  FREE( pMe );
  (void) AEEHandle_Clear( &gOEMBTExtHandleList, pParent->m_hBT );
  
  return SUCCESS;
}

// IBTEXTNA_Enable (see AEEBTExtNA.h)
int OEMBTExtNA_Enable( IBTExtNA* pParent )
{
  OEMBTExtNAobj_t*   pMe;
  bt_cmd_status_type stat;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }

  if ( pMe != gpOEMBTExtNAobj )
  {
    return EBADPARM;
  }

  if ( rdm_is_srv_holding_dev( RDM_DATA_SRVC, RDM_BT_NA_DEV ) == FALSE )
  {
    return AEEBT_EPORT;
  }
#ifdef FEATURE_BT_NA
  stat = bt_cmd_na_enable( pMe->m_appId );
#endif
  switch (stat)
  {
    case BT_CS_GN_SUCCESS:
    case BT_CS_GN_PENDING:
      return SUCCESS;
    case BT_CS_GN_CMD_Q_FULL:
      return ENOMEMORY;
    default:
      return EFAILED;
  }
}

// IBTEXTNA_Disable (see AEEBTExtNA.h)
int OEMBTExtNA_Disable( IBTExtNA* pParent )
{
  OEMBTExtNAobj_t*   pMe;
  bt_cmd_status_type stat;

  if ( AEEHandle_From( &gOEMBTExtHandleList, pParent->m_hBT, 
                       (OEMINSTANCE*)&pMe ) != TRUE )
  {
    return EBADPARM;
  }

  if ( pMe != gpOEMBTExtNAobj )
  {
    return EBADPARM;
  }
	#ifdef FEATURE_BT_NA
  stat = bt_cmd_na_disable( pMe->m_appId );
	#endif
  switch (stat)
  {
    case BT_CS_GN_SUCCESS:
    case BT_CS_GN_PENDING:
      return SUCCESS;
    case BT_CS_GN_CMD_Q_FULL:
      return ENOMEMORY;
    default:
      return EFAILED;
  }
}


// called from BT task context; so just queue the event
static void OEMBTExtNA_EventCallback( bt_ev_msg_type* bt_ev_msg_ptr )
{
  OEMBTExtNAobj_t*   pMe = gpOEMBTExtNAobj;
  AEEBTNotification* pN  = NULL;

  if ( pMe == NULL )
  {
    return; // the object no longer exists
  }

  pN = IBTEXTNOTIFIER_GetFreeNotification( pMe->m_pNotifier );
  if( pN == NULL )
  {
    return;
  }

  pN->data.uError = AEEBT_NA_ERR_NONE;

  switch (bt_ev_msg_ptr->ev_hdr.ev_type)
  {
    case BT_EV_GN_CMD_DONE:
    {
      bt_ev_gn_cmd_done_type* pm = 
        (bt_ev_gn_cmd_done_type*)&bt_ev_msg_ptr->ev_msg;

      switch ( pm->cmd_type )
      {
        case BT_CMD_NA_ENABLE:
          if ( pm->cmd_status == BT_CS_GN_SUCCESS )
          {
            IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
            return;
          }

          pN->uID  = AEEBT_NA_EVT_ENABLED;
          if ( pm->cmd_status == BT_CS_NA_NO_SIO_PORT )
          {
            pN->data.uError = AEEBT_NA_ERR_NO_SIO_PORT;
          }
          else
          {
            pN->data.uError = AEEBT_NA_ERR_ALREADY_ENABLED;
          }
          break;

        case BT_CMD_NA_DISABLE:
          if ( pm->cmd_status == BT_CS_GN_SUCCESS )
          {
            IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
            return;
          }
          pN->uID  = AEEBT_NA_EVT_DISABLED;
          pN->data.uError = AEEBT_NA_ERR_NOT_ALLOWED;
          break;

        case BT_CMD_SD_SET_SERVICE_DISCOVERABLE_MODE:
          // always success, no BT event expected
          pN->uID    = AEEBT_SD_EVT_DISCOVERABLE_MODE_SET;
          pN->uID = (pN->uID << 16) | NMASK_BT_SD;
          IBTEXTNOTIFIER_DoNotify( pMe->m_pNotifier, pN, pMe->m_pac );
          return;

        default:
          MSG_ERROR( "NAEvCb - unexpect Cmd Done %x", pm->cmd_type, 0, 0 );
          IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
          return;
      }
      break;
    } // end of case BT_EV_GN_CMD_DONE

    case BT_EV_NA_ENABLED:
    {
      pN->uID  = AEEBT_NA_EVT_ENABLED;
      break;
    }

    case BT_EV_NA_DISABLED:
    {
      pN->uID  = AEEBT_NA_EVT_DISABLED;
      break;
    }

    case BT_EV_NA_CONNECTED:
    {
      pN->uID         = AEEBT_NA_EVT_CONNECTED;
      pN->data.bdAddr = 
        *((AEEBTBDAddr*)&bt_ev_msg_ptr->ev_msg.ev_na_conn.bd_addr);
      break;
    }

    case BT_EV_NA_DISCONNECTED:
    {
      pN->uID         = AEEBT_NA_EVT_DISCONNECTED;
      pN->data.bdAddr = 
        *((AEEBTBDAddr*)&bt_ev_msg_ptr->ev_msg.ev_na_disc.bd_addr);
      break;
    }

    default:
    {
      MSG_ERROR( "NAEvCb - unexpect event %x", 
                 bt_ev_msg_ptr->ev_hdr.ev_type, 0, 0 );
      IBTEXTNOTIFIER_ReleaseNotification( pMe->m_pNotifier, pN );
      return;
    }
  }

  pN->uID = (pN->uID << 16) | NMASK_BT_NA;
  IBTEXTNOTIFIER_DoNotify( pMe->m_pNotifier, pN, pMe->m_pac );

}

#endif // defined(FEATURE_IBT)
