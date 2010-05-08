/* =========================================================================

FILE: OEMMediaAudio.c

SERVICES: Audio effects functions

DESCRIPTION
  This file contains audio functions that apps can use
  to create audio effects such as 3d positioning and reverberation.

PUBLIC CLASSES:
   N/A

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A


           Copyright © 2005-2009 QUALCOMM Incorporated.
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/oem/staticextensions/qaudiofx/main/latest/src/OEMMediaAudio.c#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/16/09    sk     Modified to included Math.h library to resolve compilation
                   warnings.
05/08/08   vsud    fixed compilation errors after adding Qaudio FX headers.
01/15/08    sk     Fixed Misspelled reverb preset names. CR 135068. 
08/28/07    pl     Modified synchronous command to time out if CMX
                   does not return status within time allowed.
09/23/06   ajt     Rewrote for new CMX MM APIs

06/23/06   ajt     Added support for Reverb and Doppler

12/12/05    jg     Removed the limit on the number of media that can be added
                   into an environment.                    

12/09/05   jas     Changed signal name to be more generic

12/02/05    jg     Removed cache of 3D object's channel Id; this can change
                   if playback stops and is restarted.
                   

========================================================================= */


/*=========================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
========================================================================= */

#include "OEMFeatures.h"

#if defined(FEATURE_BREW_MULTIMEDIA) && defined(FEATURE_QAUDIOFX)

#include <math.h>          /* For sin & cos compiler warnings */
#include "cmx.h"
#if defined (FEATURE_CMX_MM_API)
#include "cmx_mm.h"
#endif // defined (FEATURE_CMX_MM_API)
#include "rex.h"
#include "ui.h"

#include "OEMFeatures.h"
#include "OEMHeap.h"
#include "OEMObjectMgr.h"
#include "OEMCriticalSection.h"
#include "AEE_OEM.h"
#include "AEE_OEMDispatch.h"
#include "AEEStdLib.h"
#include "AEEInterface.h"
#include "AEESound.h"
#include "AEEMedia.h"
#include "AEEIMediaAudioEnv.h"
#include "AEEIMediaAudio3D.h"
#include "AEEIMediaAudioFX.h"
#include "OEMMediaAudio.h"
#include "OEMMediaCMX.h"

#include "AEEMediaAudio3D.bid"
#include "AEEMediaAudioEnv.bid"
#include "AEEMEdiaAudioFX.bid"


/*=========================================================================

                      DEFINES

========================================================================= */
#define OEMMM_ENTER_CRITICAL_SECTION()    OEMCriticalSection_Enter( & gMediaAudioCriticalSection ); {
#define OEMMM_LEAVE_CRITICAL_SECTION()  ;} OEMCriticalSection_Leave( & gMediaAudioCriticalSection );

#define MAX_ENVIRONMENTS 1
#define MEDIA_LIST_INCREMENT 4
#define REVERB_UNINITIALIZED -1

#define PI (double) 3.141592653589793
#define RADANGLE FDIV(( double ) 180.0, PI )
#define AEE_CMX_CMD_TIMEOUT 40 /* Time out in 40 ms */
/*=========================================================================

                      IMediaAudioEnv DECLARATIONS

=========================================================================*/
struct IMediaAudioEnv
{
   const AEEVTBL( IMediaAudioEnv ) * cpvtAudioEnv;
   int32                    nRefs;
   IShell *                 piShell;
   AEEObjectHandle          hObject;
   IMedia **                ppiMediaList;
   uint16                   nMediaListCount; // How many items are in the list
   uint16                   nMediaListSize;  // How many items is there space for

#if defined (FEATURE_CMX_MM_API)
   cmx_mm_env_handle_type   hEnv;            // cmx handle for the environment
#endif // !defined (FEATURE_CMX_MM_API)
   uint32                   dwEnable;        // current setting for audio effects
   AEEVector                LsnrPosition;
   AEEMediaOrientation      LsnrOrientation;
   AEEVector                LsnrVelocity;

   AEECallback              cbCommit;
   int32 *                  pnCommitRet;
   AEESoundDevice           eDevice;
   ISound *                 piSound;
   int32                    nReverbPreset;
   int32                    nReverbGain;
   uint32                   dwReverbDecayTime;
   uint32                   dwReverbDampingFactor;
   flg                      bDeferred:1;     // Whether deferred commit is enabled
   flg                      bMIDIOpen:1;     // Wheter MIDI_OUT is enabled
   flg                      bCMXCb:1;        /* This is flag for race condition which time 
                                                out occurs right after CMX Callback */
#if defined (FEATURE_CMX_MM_API)
   cmx_mm_env_set_param_type  CmxParam;
   cmx_mm_env_get_param_type  CmxGetParam;
#else
   cmx_3d_pos_param_type      cmxParam;
   cmx_3d_pos_get_param_type  cmxGetParam;
#endif // defined (FEATURE_CMX_MM_API)

   // For synchronous operation
   void *                     p1;
   void *                     p2;
   void *                     p3;
   int                        nReturn;
   rex_timer_type             cmxCmdTimer;
   uint8                      ncVoidCount;

};

extern int     IMediaAudioEnv_New ( IShell * ps, AEECLSID cls, void ** ppif );
static uint32  OEMMediaAudioEnv_AddRef ( IMediaAudioEnv * pme );
static uint32  OEMMediaAudioEnv_Release ( IMediaAudioEnv * pme );
static int     OEMMediaAudioEnv_QueryInterface ( IMediaAudioEnv * pme, AEECLSID clsIdReq, void ** ppo );
static int     OEMMediaAudioEnv_AddMedia ( IMediaAudioEnv * pme, IMedia * piMedia );
static int     OEMMediaAudioEnv_RemoveMedia ( IMediaAudioEnv * pme, IMedia * piMedia );
static int     OEMMediaAudioEnv_GetMediaList ( IMediaAudioEnv * pme, IMedia ** ppiMediaList, uint16 * pnCount );
static int     OEMMediaAudioEnv_EnableDeferCommit( IMediaAudioEnv * pme, boolean * pbNew, boolean * pbOld );
static int     OEMMediaAudioEnv_Commit( IMediaAudioEnv * pme, AEECallback * pcb, int32  *pnRet );
static int     OEMMediaAudioEnv_SetParm( IMediaAudioEnv * pme, int32 nParmID, void * p1, void * p2 );
static int     OEMMediaAudioEnv_GetParm( IMediaAudioEnv * pme, int32 nParmID, void * p1, void * p2 );

int OEMMediaAudioEnvFX_New ( IShell * ps, AEECLSID cls, void ** ppif, IMediaAudioEnv * pEnv );

#if !defined (FEATURE_CMX_MM_API)
static int OEMMediaAudioEnv_CallAndWait( IMediaAudioEnv * pme, cmx_3d_pos_cmd_enum_type nCommand );
static void OEMMediaAudioEnv_CMXCmdcb ( cmx_status_type status, const void * cpClientData );
static int OEMMediaAudioEnv_GetAndWait( IMediaAudioEnv * pme );
static void OEMMediaAudioEnv_CMXParamcb ( cmx_status_type status, cmx_3d_pos_param_enum_type param, cmx_3d_pos_param_setting_type * pSetting, const void * cpClientData );
#endif //!defined (FEATURE_CMX_MM_API)

static const AEEVTBL( IMediaAudioEnv ) gvtIMediaAudioEnv =
{
   OEMMediaAudioEnv_AddRef,
   OEMMediaAudioEnv_Release,
   OEMMediaAudioEnv_QueryInterface,
   OEMMediaAudioEnv_GetParm,
   OEMMediaAudioEnv_SetParm,
   OEMMediaAudioEnv_AddMedia,
   OEMMediaAudioEnv_RemoveMedia,
   OEMMediaAudioEnv_GetMediaList,
   OEMMediaAudioEnv_EnableDeferCommit,
   OEMMediaAudioEnv_Commit
};

static OEMCriticalSection gMediaAudioCriticalSection;
static int gEnvironmentCount = 0;

extern rex_tcb_type ui_tcb;

/*=========================================================================

                      IMediaAudio3D DECLARATIONS

=========================================================================*/
struct IMediaAudio3D
{
   const AEEVTBL( IMediaAudio3D ) * cpvtAudio3D;
   int32                nRefs;
   IShell *             piShell;
   AEEObjectHandle      hObject;
   IMedia *             piMedia;  // The IMedia object that did QueryInterface to create IMediaAudio3D

   cmx_3d_pos_param_type          cmxParam;
#if defined (FEATURE_CMX_MM_API)
   cmx_mm_play_get_param_type     CmxGetParam;
#else
   cmx_3d_pos_get_param_type      cmxGetParam;
   cmx_3d_pos_param_setting_type  cmxSetting;
#endif // defined (FEATURE_CMX_MM_API)
   cmx_3d_pos_cmd_enum_type       cmd;

   // For synchronous operation
   void *               p1;
   void *               p2;
   void *               p3;
   int                  nReturn;
   rex_timer_type       cmxCmdTimer;
   uint8                ncVoidCount; /* This is to keep track number of 3D commands
                                       issued to CMX being voided */
   flg                  bCMXCb:1; /* This is flag for race condition which time out occurs right
                                   after CMX Callback */
};

static uint32  OEMMediaAudio3D_AddRef ( IMediaAudio3D * pme );
static uint32  OEMMediaAudio3D_Release ( IMediaAudio3D * pme );
static int     OEMMediaAudio3D_QueryInterface ( IMediaAudio3D * pme, AEECLSID clsIdReq, void ** ppo );
static int     OEMMediaAudio3D_SetParm ( IMediaAudio3D * pme, int32 nParmID, void * p1, void * p2 );
static int     OEMMediaAudio3D_GetParm ( IMediaAudio3D * pme, int32 nParmID, void * p1, void * p2 );

#if !defined (FEATURE_CMX_MM_API)
static int OEMMediaAudio3D_CallAndWait ( IMediaAudio3D * pme );
static void OEMMediaAudio3D_CMXCmdcb ( cmx_status_type status, const void * cpClientData );
static int OEMMediaAudio3D_GetAndWait( IMediaAudio3D * pme );
static void OEMMediaAudio3D_CMXParamcb ( cmx_status_type status, cmx_3d_pos_param_enum_type param, cmx_3d_pos_param_setting_type * pSetting, const void * cpClientData );
static int OEMMediaAudio3D_SetChannel ( IMediaAudio3D * pme, void ** ppChannel );
#endif // !defined (FEATURE_CMX_MM_API)

static const AEEVTBL( IMediaAudio3D ) gvtIMediaAudio3D =
{
   OEMMediaAudio3D_AddRef,
   OEMMediaAudio3D_Release,
   OEMMediaAudio3D_QueryInterface,
   OEMMediaAudio3D_GetParm,
   OEMMediaAudio3D_SetParm,
};


/*=========================================================================

                      IMediaAudioFX DECLARATIONS

=========================================================================*/
struct IMediaAudioFX
{
   const AEEVTBL( IMediaAudioFX ) * cpvtAudioFX;
   int32                nRefs;
   IShell *             piShell;
   AEEObjectHandle      hObject;
   IMedia *             piMedia;     // The IMedia object that did QueryInterface to create IMediaAudioFX

   cmx_3d_pos_param_type          cmxParam;
   cmx_3d_pos_get_param_type      cmxGetParam;
   cmx_3d_pos_param_setting_type  cmxSetting;
   cmx_3d_pos_cmd_enum_type       cmd;

   // For synchronous operation
   void *               p1;
   void *               p2;
   void *               p3;
   int                  nReturn;
   rex_timer_type       cmxCmdTimer;
   uint8                ncVoidCount;  /* This is to keep track number of FX commands
                                       issued to CMX being voided */
   flg                  bCMXCb:1;        /* This is flag for race condition which time 
                                            out occurs right after CMX Callback */
   // Until here, this class must look like IMediaAudio3D.
   // add private members after this comment

   IMediaAudioEnv *     pEnv;        // The IMediaAudioEnv object that did QueryInterface to create IMediaAudioFX
};

static uint32  OEMMediaAudioFX_AddRef( IMediaAudioFX * pme );
static uint32  OEMMediaAudioFX_Release( IMediaAudioFX * pme );
static int     OEMMediaAudioFX_QueryInterface( IMediaAudioFX * pme, AEECLSID clsIdReq, void ** ppo );
static int     OEMMediaAudioFX_SetParm( IMediaAudioFX * pme, int32 nParmID, void * p1, void * p2 );
static int     OEMMediaAudioFX_GetParm( IMediaAudioFX * pme, int32 nParmID, void * p1, void * p2 );

#if !defined (FEATURE_CMX_MM_API)
static int OEMMediaAudioFX_CallAndWait( IMediaAudioFX * pme);
static void OEMMediaAudioFX_CMXCmdcb ( cmx_status_type status, const void * cpClientData );
static int OEMMediaAudioFX_GetAndWait( IMediaAudioFX * pme);
static void OEMMediaAudioFX_CMXParamcb ( cmx_status_type status, cmx_3d_pos_param_enum_type param, cmx_3d_pos_param_setting_type * pSetting, const void * cpClientData );
#endif // !defined (FEATURE_CMX_MM_API)

static const AEEVTBL( IMediaAudioFX ) gvtIMediaAudioFX =
{
   OEMMediaAudioFX_AddRef,
   OEMMediaAudioFX_Release,
   OEMMediaAudioFX_QueryInterface,
   OEMMediaAudioFX_GetParm,
   OEMMediaAudioFX_SetParm,
};


/*=========================================================================

                     IMediaAudioEnv FUNCTION DEFINITIONS

=========================================================================*/

#if defined (FEATURE_CMX_MM_API)
/*==================================================================

FUNCTION       OEMMediaAudioEnv_CMXCmdStatusCB()

DESCRIPTION    Callback method for CMX MM API environment operations. 
               Called by CMX to inform OEM layer of whether an operation was accepted for processing.

DEPENDENCIES   None

PARAMETERS     cmx_mm_cmd_status_type:  Status of the started operation
               const void *:            Pointer to data identifying the client of the operation

RETURN VALUE   None

SIDE EFFECTS   Saves returned status in the client object for delivery to application at a later time

==================================================================*/
static void OEMMediaAudioEnv_CMXCmdStatusCB ( cmx_mm_cmd_status_type   status, 
                                            const void             * cpClientData )
{
   IMediaAudioEnv * pme;

   OEMMM_ENTER_CRITICAL_SECTION()

   pme = ( IMediaAudioEnv * ) AEEObjectMgr_GetObject (( AEEObjectHandle ) cpClientData );
   if ( NULL == pme )
   {
      goto cleanup;
   }

   pme->nReturn = EFAILED;
   switch ( status )
   {
   case CMX_MM_STATUS_ACCEPTED:
      break;

   case CMX_MM_STATUS_SUCCESS:
   case CMX_MM_STATUS_FAILURE:
   default:
      goto cleanup;
   }
   pme->nReturn = SUCCESS;

cleanup:
   if ( status != CMX_MM_STATUS_ACCEPTED )
   {
      // Release UI
      rex_set_sigs ( & ui_tcb, AEE_MEDIA_SIG );
   }

   OEMMM_LEAVE_CRITICAL_SECTION()
}

/*==================================================================

FUNCTION       OEMMediaAudioEnv_CMXCmdMainCB()

DESCRIPTION    Callback method for CMX MM API environment operations. 
               Called from CMX to inform OEM layer of completition of an operation.

DEPENDENCIES   None

PARAMETERS     cmx_mm_env_status_enum_type:     Status of the completed operation
               const void *:                    Pointer to data identifying the client of the operation
               cmx_mm_env_callback_data_type *: Resulting data of the operation
 
RETURN VALUE   None

SIDE EFFECTS   Saves returned data in the client object for delivery to application at a later time

==================================================================*/
static void OEMMediaAudioEnv_CMXCmdMainCB ( cmx_mm_env_status_enum_type     env_cmd, 
                                          const void                    * cpClientData,  
                                          cmx_mm_env_callback_data_type * pCbData )
{
   IMediaAudioEnv * pme;

   OEMMM_ENTER_CRITICAL_SECTION()

   pme = ( IMediaAudioEnv * ) AEEObjectMgr_GetObject (( AEEObjectHandle ) cpClientData );
   if ( NULL == pme )
   {
      goto cleanup;
   }

   pme->nReturn = EFAILED;
   switch ( env_cmd )
   {
   case CMX_MM_ENV_HANDLE:
      pme->hEnv = pCbData->env_handle;
      break;

   case CMX_MM_ENV_CONTROL_CB:
      if ( pCbData->control_cb.num_control_cmds != 1 )
      {
         goto cleanup;
      }
      switch ( pCbData->control_cb.control_results->status )
      {
      case CMX_MM_ENV_ACTIVATED:
      case CMX_MM_ENV_DEACTIVATED:
         if ( pCbData->control_cb.control_results->control_cmd != CMX_MM_ENV_CONTROL_ACTIVATE )
         {
            goto cleanup;
         }
         break;

      case CMX_MM_ENV_ATTACHED:    
         if ( pCbData->control_cb.control_results->control_cmd != CMX_MM_ENV_CONTROL_ATTACH )
         {
            goto cleanup;
         }
         break;

      case CMX_MM_ENV_DETACHED:
         if ( pCbData->control_cb.control_results->control_cmd != CMX_MM_ENV_CONTROL_DETACH )
         {
            goto cleanup;
         }
         break;

      default:
         goto cleanup;
      }
      break;

   case CMX_MM_ENV_SET_CB:
      if (( pCbData->set_cb.num_set_cmds != 1 ) || 
          ( pCbData->set_cb.set_status->status != CMX_MM_ENV_SUCCESS ))
      {
         goto cleanup;
      }
      switch ( pCbData->set_cb.set_status->set_cmd )
      {
      case CMX_MM_ENV_SET_POS_ENABLE:
      case CMX_MM_ENV_SET_POS_LISTENER_POS:
      case CMX_MM_ENV_SET_POS_LISTENER_ORI:
         break;

      case CMX_MM_ENV_SET_EVENT_CALLBACK:
      case CMX_MM_ENV_SET_INVALID:
      default:
         pme->nReturn = EUNSUPPORTED;
         goto cleanup;
      }
      break;

   case CMX_MM_ENV_GET_CB:
      if ( pCbData->get_cb.num_get_cmds != 1 ) 
      {
         goto cleanup;
      }
      switch ( pCbData->get_cb.get_results->get_cmd )
      {
      case CMX_MM_ENV_GET_POS_LISTENER_POS:
         pme->nReturn = ( pCbData->get_cb.get_results->pos_lsnr_pos.status == CMX_MM_ENV_SUCCESS ) ? SUCCESS : EFAILED;
         
         if ( SUCCESS == pme->nReturn )
         {
            AEEVector * pVector = ( AEEVector * ) pme->p1;

            pVector->x = pCbData->get_cb.get_results->pos_lsnr_pos.lsnr_pos.x_pos;
            pVector->y = pCbData->get_cb.get_results->pos_lsnr_pos.lsnr_pos.y_pos;
            pVector->z = pCbData->get_cb.get_results->pos_lsnr_pos.lsnr_pos.z_pos;
         }
         goto cleanup;

      case CMX_MM_ENV_GET_POS_LISTENER_ORI:
         pme->nReturn = ( pCbData->get_cb.get_results->pos_lsnr_ori.status == CMX_MM_ENV_SUCCESS ) ? SUCCESS : EFAILED;
         
         if ( SUCCESS == pme->nReturn )
         {
            AEEMediaOrientation * pOrientation = ( AEEMediaOrientation * ) pme->p1;

            pOrientation->ForwardVector.x = pCbData->get_cb.get_results->pos_lsnr_ori.lsnr_orient.x_forward;
            pOrientation->ForwardVector.y = pCbData->get_cb.get_results->pos_lsnr_ori.lsnr_orient.y_forward;
            pOrientation->ForwardVector.z = pCbData->get_cb.get_results->pos_lsnr_ori.lsnr_orient.z_forward;
            pOrientation->UpVector.x = pCbData->get_cb.get_results->pos_lsnr_ori.lsnr_orient.x_up;
            pOrientation->UpVector.y = pCbData->get_cb.get_results->pos_lsnr_ori.lsnr_orient.y_up;
            pOrientation->UpVector.z = pCbData->get_cb.get_results->pos_lsnr_ori.lsnr_orient.z_up;
         }
         goto cleanup;

      case CMX_MM_ENV_GET_POS_ENABLED:
      case CMX_MM_ENV_GET_NUM_SESSIONS:
      case CMX_MM_ENV_GET_SESSIONS:
      case CMX_MM_ENV_GET_MAX_SESSIONS:
      case CMX_MM_ENV_GET_HYBRID_MODE:
      case CMX_MM_ENV_GET_MAX:
      default:
         pme->nReturn = EUNSUPPORTED;
         goto cleanup;
      }
      break;

   case CMX_MM_ENV_SUCCESS:
      break;

   case CMX_MM_ENV_ACTIVATED:
   case CMX_MM_ENV_DEACTIVATED:
   case CMX_MM_ENV_ATTACHED:
   case CMX_MM_ENV_DETACHED:
   case CMX_MM_ENV_FAILURE:
   case CMX_MM_ENV_SES_HANDLE:
   case CMX_MM_ENV_EVENT_ENV_DEACTIVATED:
   case CMX_MM_ENV_DLS_PARTIAL_LOAD:
   case CMX_MM_ENV_INVALID:
   default:
      goto cleanup;
   }
   pme->nReturn = SUCCESS;

cleanup:
   // Release UI
   rex_set_sigs ( & ui_tcb, AEE_MEDIA_SIG );

   OEMMM_LEAVE_CRITICAL_SECTION()
}

/*==================================================================
  
FUNCTION       OEMMediaAudioEnv_InitEnvParam()

DESCRIPTION    Initializes environment identification parameter for environment method calls

DEPENDENCIES   None

PARAMETERS     IMediaAudioEnv *:        Object reference pointer
               cmx_mm_env_param_type *: Pointer to environment identification parameter 
 
RETURN VALUE   None

SIDE EFFECTS   None

==================================================================*/
static __inline void OEMMediaAudioEnv_InitEnvParam (  IMediaAudioEnv *pme, cmx_mm_env_param_type * param )
{
   param->env_handle = pme->hEnv;
   param->cb_func = ( cmx_mm_cb_func_ptr_type ) OEMMediaAudioEnv_CMXCmdStatusCB;
   param->client_data = ( const void * ) pme->hObject;
}

/*==================================================================
  
FUNCTION       OEMMediaAudioEnv_CtrlCmd()

DESCRIPTION    Sends a control command to the CMX environment.

DEPENDENCIES   None

PARAMETERS     IMediaAudioEnv *:                   Client requesting a CMX operation
               const cmx_mm_env_control_enum_type: Control command 
               uint32:                             Associated command data, if any
 
RETURN VALUE   None

SIDE EFFECTS   Synchronous operation that suspends the UI task until CMX has finished processing

==================================================================*/
static void OEMMediaAudioEnv_CtrlCmd ( IMediaAudioEnv * pme, cmx_mm_env_control_enum_type Cmd, uint32 dwValue )
{
   cmx_mm_env_param_type         EnvParam;
   cmx_mm_env_control_param_type EnvCtrl;

   switch ( Cmd )
   {
   case CMX_MM_ENV_CONTROL_ACTIVATE:
      EnvCtrl.activate.control_cmd = CMX_MM_ENV_CONTROL_ACTIVATE;
      EnvCtrl.activate.activate = ( boolean ) dwValue;
      break;

   case CMX_MM_ENV_CONTROL_ATTACH:
      EnvCtrl.attach.control_cmd = CMX_MM_ENV_CONTROL_ATTACH;
      EnvCtrl.attach.session_handle = ( cmx_mm_session_handle_type ) dwValue;
      break;

   case CMX_MM_ENV_CONTROL_DETACH:
      EnvCtrl.attach.control_cmd = CMX_MM_ENV_CONTROL_DETACH;
      EnvCtrl.attach.session_handle = ( cmx_mm_session_handle_type ) dwValue;
      break;

   case CMX_MM_ENV_CONTROL_HANDOFF:
   case CMX_MM_ENV_CONTROL_INVALID:
   default:
      pme->nReturn = EUNSUPPORTED;
      return;
   }

   OEMMediaAudioEnv_InitEnvParam ( pme, & EnvParam );

   ( void ) rex_clr_sigs ( & ui_tcb, AEE_MEDIA_SIG );
   cmx_mm_environment_control ( & EnvParam, 1, & EnvCtrl );
   ( void ) rex_wait( AEE_MEDIA_SIG );
}

/*==================================================================
    
FUNCTION       OEMMediaAudioEnv_SetEnvParam()

DESCRIPTION    Sets a parameter value in the CMX environment.

DEPENDENCIES   CmxParam member in the client must be initialized before calling this function

PARAMETERS     IMediaAudioEnv *: Client requesting a CMX operation
 
RETURN VALUE   None

SIDE EFFECTS   Synchronous operation that suspends the UI task until CMX has finished processing

==================================================================*/
static void OEMMediaAudioEnv_SetEnvParam ( IMediaAudioEnv * pme )
{
   cmx_mm_env_param_type     EnvParam;

   switch ( pme->CmxParam.set_cmd )
   {
   case CMX_MM_ENV_SET_POS_ENABLE:
   case CMX_MM_ENV_SET_POS_LISTENER_POS:
   case CMX_MM_ENV_SET_POS_LISTENER_ORI:
      break;

   case CMX_MM_ENV_SET_EVENT_CALLBACK:
   case CMX_MM_ENV_SET_INVALID:
      pme->nReturn = EUNSUPPORTED;
      return;
   }

   OEMMediaAudioEnv_InitEnvParam ( pme, & EnvParam );

   ( void ) rex_clr_sigs ( & ui_tcb, AEE_MEDIA_SIG );
   cmx_mm_environment_set_param ( & EnvParam, 1, & pme->CmxParam );
   ( void ) rex_wait( AEE_MEDIA_SIG );
}

/*==================================================================

FUNCTION       OEMMediaAudioEnv_GetEnvParam()

DESCRIPTION    Gets a parameter value from the CMX environment.

DEPENDENCIES   CmxGetParam member in the client must be initialized before calling this function

PARAMETERS     IMediaAudioEnv *: Client requesting a CMX operation
 
RETURN VALUE   None

SIDE EFFECTS   Synchronous operation that suspends the UI task until CMX has finished processing
               CmxGetParam member of the client gets initialized here

==================================================================*/
static void OEMMediaAudioEnv_GetEnvParam ( IMediaAudioEnv * pme )
{
   cmx_mm_env_param_type EnvParam;

   switch ( pme->CmxGetParam.get_cmd )
   {
   case CMX_MM_ENV_GET_POS_ENABLED:
   case CMX_MM_ENV_GET_POS_LISTENER_POS:
   case CMX_MM_ENV_GET_POS_LISTENER_ORI:
      break;

   case CMX_MM_ENV_GET_NUM_SESSIONS:
   case CMX_MM_ENV_GET_SESSIONS:
   case CMX_MM_ENV_GET_MAX_SESSIONS:
   case CMX_MM_ENV_GET_HYBRID_MODE:
   case CMX_MM_ENV_GET_MAX:
   default:
      pme->nReturn = EUNSUPPORTED;
      return;
   }

   OEMMediaAudioEnv_InitEnvParam ( pme, & EnvParam );

   ( void ) rex_clr_sigs ( & ui_tcb, AEE_MEDIA_SIG );
   cmx_mm_environment_get_param ( & EnvParam, 1, & pme->CmxGetParam );
   ( void ) rex_wait ( AEE_MEDIA_SIG );
}
#endif//  defined (FEATURE_CMX_MM_API)

#if !defined (FEATURE_CMX_MM_API)
static void OEMMediaAudioEnv_MIDIOutNotify( void * pObj, boolean bSuccess )
{
   IMediaAudioEnv * pme = ( IMediaAudioEnv * ) pObj;

   pme->bMIDIOpen = bSuccess;
}
#endif // !defined (FEATURE_CMX_MM_API)

/*==================================================================

FUNCTION       IMediaAudioEnv_New()

DESCRIPTION    Constructor of IMediaAudioEnv object

DEPENDENCIES   None

PARAMETERS     IShell *: Pointer to Brew environment
               AEECLSID: Class ID of the created object
               void **:  Pointer to location where the created object reference pointer will be stored
 
RETURN VALUE   int:      Status code

SIDE EFFECTS   None

==================================================================*/
int IMediaAudioEnv_New( IShell * ps, AEECLSID cls, void ** ppif )
{
#if defined (FEATURE_CMX_MM_API)  
   cmx_mm_env_info_param_type EnvInfo;
   cmx_mm_env_param_type      EnvParam;
#else
   IMediaAudioFX  * pFX = NULL;
#endif // defined (FEATURE_CMX_MM_API)
   IMediaAudioEnv * pme;

   if ( ppif != NULL )
   {
      *ppif = NULL;
   }

   if ( gEnvironmentCount >= MAX_ENVIRONMENTS )
   {
      return ENOTALLOWED;
   }

   pme = MALLOC( sizeof( IMediaAudioEnv ));

   if ( NULL == pme )
   {
      return ENOMEMORY;
   }

   pme->cpvtAudioEnv = & gvtIMediaAudioEnv;
   pme->nRefs = 1;
   pme->piShell = ps;
   pme->ncVoidCount = 0;
   rex_def_timer(&pme->cmxCmdTimer, &ui_tcb, AEE_MEDIA_SIG);

   // Register the object with ObjectMgr...
   if ( AEEObjectMgr_Register( pme, & pme->hObject ))
   {
      goto cleanup;
   }

#if defined (FEATURE_CMX_MM_API)
   EnvInfo.env_type = CMX_MM_ENV_TYPE_POSITIONAL;
   EnvInfo.cb_func = ( cmx_mm_env_cb_func_ptr_type ) OEMMediaAudioEnv_CMXCmdMainCB;
   EnvInfo.client_data = ( void * ) pme->hObject;

   ( void ) rex_clr_sigs ( & ui_tcb, AEE_MEDIA_SIG );
   cmx_mm_environment_create ( NULL, & EnvInfo );
   ( void ) rex_wait ( AEE_MEDIA_SIG );

   if ( SUCCESS != pme->nReturn )
   {
      goto cleanup;
   }

   OEMMediaAudioEnv_CtrlCmd ( pme, CMX_MM_ENV_CONTROL_ACTIVATE, ( uint32 ) TRUE );

   if ( SUCCESS != pme->nReturn )
   {
      goto cleanup;
   }
#else
   if ( SUCCESS != CMIDIOut_Open (( byte ) CMX_ADEC_MODE_ADPCM, OEMMediaAudioEnv_MIDIOutNotify, ( void * ) pme, pme->hObject, TRUE ))
   { 
      goto cleanup;
   }

   // initialize reverb preset in cmx if it hasn't one already
   if ( SUCCESS != IMediaAudioEnv_QueryInterface ( pme, AEECLSID_MediaAudioFX, ( void ** ) & pFX ))
   {
      goto cleanup;
   }

   pFX->cmxGetParam.param = CMX_3D_POS_PARAM_REVERB;
   if ( SUCCESS != OEMMediaAudioFX_GetAndWait ( pFX ))
   {
      uint32 dwEnable = MM_AENV_ENABLE_NONE;

      // 3D needs to be enabled before we can set reverb preset
      pme->cmxParam.enable_cmd.mode = CMX_3D_POS_MODE_ENABLE;
      if ( SUCCESS != OEMMediaAudioEnv_CallAndWait ( pme, CMX_3D_POS_CMD_ENABLE ))
      {  
         goto cleanup;
      }

      // set reverb preset
      pFX->cmxParam.reverb_cmd.env_type = CMX_3D_REVERB_GENERIC;  
      pFX->cmd = CMX_3D_POS_CMD_SET_REVERB_PRESET;
      if ( SUCCESS != OEMMediaAudioFX_CallAndWait ( pFX ))
      {  
         goto cleanup;
      }

      // reverb needs to be enabled before it can be disabled
      pFX->cmd = CMX_3D_POS_CMD_ENABLE_REVERB;
      if ( SUCCESS !=  OEMMediaAudioFX_CallAndWait ( pFX ))
      {
         goto cleanup;
      }

      // disable all processing
      if ( SUCCESS != IMediaAudioEnv_Enable ( pme, & dwEnable, NULL ))
      {
         goto cleanup;
      }
   }

   // Set defaults
   pme->nReverbPreset = REVERB_UNINITIALIZED; // illegal non-value
   pme->nReverbGain = 0;              // full volume
   pme->dwReverbDecayTime = 0;        // no echos
   pme->dwReverbDampingFactor = 1000; // normal damping
   pme->LsnrOrientation.ForwardVector.x = 0;
   pme->LsnrOrientation.ForwardVector.y = 0;
   pme->LsnrOrientation.ForwardVector.z = -1;
   pme->LsnrOrientation.UpVector.x = 0;
   pme->LsnrOrientation.UpVector.y = 1;
   pme->LsnrOrientation.UpVector.z = 0;
   
   OEMMediaAudioFX_Release ( pFX );
#endif // defined (FEATURE_CMX_MM_API)

   if ( ppif != NULL )
   {
      *ppif = pme;
   }

   ++gEnvironmentCount;

   return SUCCESS;

cleanup:
#if defined (FEATURE_CMX_MM_API)
   if ( pme->hEnv )
   {
      OEMMediaAudioEnv_InitEnvParam ( pme, & EnvParam );

      ( void ) rex_clr_sigs ( & ui_tcb, AEE_MEDIA_SIG );
      cmx_mm_terminate_environment ( & EnvParam ); 
      ( void ) rex_wait ( AEE_MEDIA_SIG );
   }
#else
   if ( pFX )
   {
      OEMMediaAudioFX_Release( pFX );
   }
#endif // defined (FEATURE_CMX_MM_API)
   OEMMediaAudioEnv_Release( pme );
   return EFAILED;
}

/*==================================================================

FUNCTION       OEMMediaAudioEnv_AddRef()

DESCRIPTION    Increments reference counf of IMediaAudioEnv object

DEPENDENCIES   None

PARAMETERS     IMediaAudioEnv *: Environment object reference pointer

RETURN VALUE   uint32: New reference count

SIDE EFFECTS   None

==================================================================*/
static uint32 OEMMediaAudioEnv_AddRef ( IMediaAudioEnv * pme )
{
   return ++pme->nRefs;
}

/*==================================================================

FUNCTION       OEMMediaAudioEnv_Release()

DESCRIPTION    Destructor of IMediaAudioEnv object

DEPENDENCIES   None

PARAMETERS     IMediaAudioEnv *: Environment object reference pointer

RETURN VALUE   uint32: New reference count

SIDE EFFECTS   Only the object reference count is decremented, if the count is greater than 1.
               The object is destroyed once the reference count becomes 0
               
==================================================================*/
static uint32 OEMMediaAudioEnv_Release ( IMediaAudioEnv * pme )
{
   uint32                dwEnable = MM_AENV_ENABLE_NONE;
#if defined (FEATURE_CMX_MM_API)
   cmx_mm_env_param_type EnvParam;
#endif // defined (FEATURE_CMX_MM_API)

   if ( pme->nRefs > 0 )
   {
      if ( --pme->nRefs )
      {
         return( pme->nRefs );
      }

      pme->nRefs = 1; // protect against circular calls during release

      // Disable all processing if no other environments left
      if ( 0 == --gEnvironmentCount )
      {
         IMediaAudioEnv_Enable ( pme, &dwEnable, NULL );
      }

      while ( pme->nMediaListCount )
      {
         if ( SUCCESS != OEMMediaAudioEnv_RemoveMedia ( pme, pme->ppiMediaList[0] ))
         {
            break;
         }
      }

#if defined (FEATURE_CMX_MM_API)
      if ( pme->hEnv )
      {
         OEMMediaAudioEnv_CtrlCmd ( pme, CMX_MM_ENV_CONTROL_ACTIVATE, ( uint32 ) FALSE );

         OEMMediaAudioEnv_InitEnvParam ( pme, & EnvParam );

         ( void ) rex_clr_sigs ( & ui_tcb, AEE_MEDIA_SIG );
         cmx_mm_terminate_environment ( & EnvParam ); 
         ( void ) rex_wait( AEE_MEDIA_SIG );
      }
#else
      if ( NULL != pme->piSound )
      {
         ISOUND_Release ( pme->piSound );
      }
      if ( pme->bMIDIOpen )
      {
         CMIDIOut_Release ();
      }
#endif // defined (FEATURE_CMX_MM_API)

      AEEObjectMgr_Unregister ( pme->hObject );

      pme->nRefs = 0;
      (void) rex_clr_timer(&pme->cmxCmdTimer);
      FREE ( pme );
   }

   return 0;
}

/*==================================================================

FUNCTION       OEMMediaAudioEnv_QueryInterface()

DESCRIPTION    Interface extractor of IMediaAudioEnv object
               IMediaAudioFX interface can be retrieved from IMediaAudioEnv

DEPENDENCIES   None

PARAMETERS     IMediaAudioEnv *: Environment object reference pointer
               AEECLSID:         Class ID of the desired interface
               void **:          Pointer to location where the new interface reference pointer will be stored

RETURN VALUE   int:              Status code

SIDE EFFECTS   None
               
==================================================================*/
static int OEMMediaAudioEnv_QueryInterface ( IMediaAudioEnv * pme, AEECLSID clsIdReq, void ** ppo )
{
   if ( NULL == ppo )
   {
      return EBADPARM;
   }

   if (( NULL != IQI_SELF( pme, clsIdReq, ppo, AEECLSID_MediaAudioEnv )) ||
       ( NULL != IQI_SELF( pme, clsIdReq, ppo, AEEIID_MEDIAAUDIOENV )))
   {
      IQI_AddRef ( *ppo );
      return SUCCESS;
   }
   else if (( AEECLSID_MediaAudioFX == clsIdReq ) ||
            ( AEEIID_MEDIAAUDIOFX == clsIdReq ))
   {
      return OEMMediaAudioEnvFX_New ( pme->piShell, clsIdReq, ppo, pme );
   }

   return ECLASSNOTSUPPORT;
}

/*=========================================================================

FUNCTION       OEMMediaAudioEnv_AddMedia()

DESCRIPTION    Associates a media object to the audio environment

DEPENDENCIES   None

PARAMETERS     IMediaAudioEnv *: Environment object reference pointer
               IMedia *:         Pointer to media object to associate

RETURN VALUE   int:              Status code

SIDE EFFECTS   None
 
========================================================================= */
static int OEMMediaAudioEnv_AddMedia ( IMediaAudioEnv * pme, IMedia * piMedia )
{
   int     nRet = SUCCESS;
#if !defined (FEATURE_CMX_MM_API)
   boolean bEnable;
#endif // !defined (FEATURE_CMX_MM_API)

   // See if there is enough room to add to the list
   if ( pme->nMediaListSize == pme->nMediaListCount )
   {
      IMedia ** ppiNewList = REALLOC( pme->ppiMediaList, ( pme->nMediaListSize + MEDIA_LIST_INCREMENT ) * sizeof( IMedia * ));
      if ( ppiNewList )
      {
         pme->ppiMediaList = ppiNewList;
         pme->nMediaListSize += MEDIA_LIST_INCREMENT;
      }
      else
      {
         return ENOMEMORY;
      }
   }

#if !defined (FEATURE_CMX_MM_API)
   nRet = IMEDIA_IsChannelShare ( piMedia, & bEnable );
   if ( SUCCESS != nRet )
   {
      return nRet;
   }

   if ( !bEnable )
   {
      nRet = IMEDIA_EnableChannelShare ( piMedia, TRUE );
      if ( SUCCESS != nRet )
      {
         return nRet;
      }
   }
#endif // !defined (FEATURE_CMX_MM_API)

   pme->ppiMediaList[ pme->nMediaListCount++ ] = piMedia;
   IMEDIA_AddRef ( piMedia );

#if defined (FEATURE_CMX_MM_API)
   OEMMediaAudioEnv_CtrlCmd ( pme, CMX_MM_ENV_CONTROL_ATTACH, ( uint32 ) CMediaCMX_GetSessionHandle ( piMedia ));
   nRet = pme->nReturn;
#else
   CMediaMIDIOutQCP_SetMediaEnv ( piMedia, pme->dwEnable, TRUE );
#endif // defined (FEATURE_CMX_MM_API)

   return nRet;
}

/*=========================================================================

FUNCTION       OEMMediaAudioEnv_RemoveMedia()

DESCRIPTION    Disassociates a media object from the audio environment

DEPENDENCIES   None

PARAMETERS     IMediaAudioEnv *: Environment object reference pointer
               IMedia *:         Pointer to media object to dissassociate

RETURN VALUE   int:              Status code

SIDE EFFECTS   None
 
========================================================================= */
static int OEMMediaAudioEnv_RemoveMedia ( IMediaAudioEnv * pme, IMedia * piMedia )
{
   int i, j;
   int nRet = SUCCESS;

   // Find item in list
   for ( i = 0; i < pme->nMediaListCount; ++i )
   {
      if ( pme->ppiMediaList[ i ] == piMedia )
      {
         break;
      }
   }

   // Shift higher items down
   if ( i < pme->nMediaListCount )
   {
      for ( j = i; j < pme->nMediaListCount - 1; ++j )
      {
         pme->ppiMediaList[ j ] = pme->ppiMediaList[ j + 1 ];
      }
      --pme->nMediaListCount;

#if defined (FEATURE_CMX_MM_API)
      OEMMediaAudioEnv_CtrlCmd ( pme, CMX_MM_ENV_CONTROL_DETACH, ( uint32 ) CMediaCMX_GetSessionHandle ( piMedia ));
      nRet = pme->nReturn;
#else
      CMediaMIDIOutQCP_SetMediaEnv ( piMedia, MM_AENV_ENABLE_NONE, FALSE );
#endif // defined (FEATURE_CMX_MM_API)

      IMEDIA_Release ( piMedia );
   }
   else
   {
      return EBADPARM;
   }

   return nRet;
}

/*=========================================================================

FUNCTION       OEMMediaAudioEnv_GetMediaList()

DESCRIPTION    Retrieves a list of media object currently associated with the audio environment

DEPENDENCIES   None

PARAMETERS     IMediaAudioEnv *: Environment object reference pointer
               IMedia **:         Pointer to a list of media objects references
                                 Pass NULL to find out only how many items are in the list
               uint16 *:         Pointer to the size of the list

RETURN VALUE   int:              Status code

SIDE EFFECTS   The given list is filled with media object references upto the size of the list.
               Size is updated with the number of references copied into the list

========================================================================= */
static int OEMMediaAudioEnv_GetMediaList ( IMediaAudioEnv * pme, IMedia ** ppiMediaList, uint16 * pnCount )
{
   if ( NULL == pnCount )
   {
      return EBADPARM;
   }

   if ( ppiMediaList != NULL )
   {
      uint16 i;

      for ( i = 0; i < *pnCount && i < pme->nMediaListCount; ++i )
      {
         ppiMediaList[ i ] = pme->ppiMediaList[ i ];
      }
      *pnCount = i;
   }
   else
   {
      *pnCount = pme->nMediaListCount;
   }

   return SUCCESS;
}

#if !defined (FEATURE_CMX_MM_API)
/*=========================================================================

========================================================================= */
static void OEMMediaAudioEnv_UpdateMediaEnv ( IMediaAudioEnv * pme )
{
   uint16 i;

   for ( i = 0; i < pme->nMediaListCount; ++i )
   {
      CMediaMIDIOutQCP_SetMediaEnv ( pme->ppiMediaList[ i ], pme->dwEnable, TRUE );
   }
}
#endif // !defined (FEATURE_CMX_MM_API)

/*=========================================================================
FUNCTION       OEMMediaAudioEnv_EnableDeferCommit()

DESCRIPTION    Enables or disables deferred commit of changes to settings.

DEPENDENCIES   None

PARAMETERS     IMediaAudioEnv *: Environment object reference pointer
               boolean *:        Pointer to new setting
               boolean *:        Pointer to old setting

RETURN VALUE   int:              Status code

SIDE EFFECTS   None

========================================================================= */
static int OEMMediaAudioEnv_EnableDeferCommit ( IMediaAudioEnv * pme, boolean * pbNew, boolean * pbOld )
{
   return EUNSUPPORTED;
}

/*=========================================================================

FUNCTION       OEMMediaAudioEnv_Commit()

DESCRIPTION    Commits associated IMediaAudio3D, IMediaAudioFX and environment settings
               if deferred commit is enabled.

DEPENDENCIES   None

PARAMETERS     IMediaAudioEnv *: Environment object reference pointer
               AEECallback *:    Pointer to callback function to call when commit is completed
               int32 *:          Status code after commit is completed

RETURN VALUE   int:              Status code

SIDE EFFECTS   All pending parameter value changes will take effect

========================================================================= */
static int OEMMediaAudioEnv_Commit ( IMediaAudioEnv * pme, AEECallback * pcb, int32 * pnRet )
{
   return EUNSUPPORTED;
}

#if !defined (FEATURE_CMX_MM_API)
/*=========================================================================

FUNCTION       map_fx2cmx()

DESCRIPTION    Converts Brew enumeration type to CMX enumeration type for reverb preset values

DEPENDENCIES   None

PARAMETERS     uint32:                  Brew reverb preset

RETURN VALUE   cmx_3d_reverb_enum_type: CMX reverb preset

SIDE EFFECTS   None

========================================================================= */
static cmx_3d_reverb_enum_type map_fx2cmx ( uint32 nFx )
{
   cmx_3d_reverb_enum_type result = CMX_3D_REVERB_INVALID;

   switch ( nFx )
   {
   case MM_REVERB_ENV_PRESET_NONE:
   default:
      result = CMX_3D_REVERB_INVALID;
      break;
      
   case MM_REVERB_ENV_PRESET_ROOM:
      result = CMX_3D_REVERB_ROOM;
      break;

   case MM_REVERB_ENV_PRESET_BATHROOM:
      result = CMX_3D_REVERB_BATHROOM;
      break;

   case MM_REVERB_ENV_PRESET_CONCERTHALL:
      result = CMX_3D_REVERB_CONCERTHALL;
      break;

   case MM_REVERB_ENV_PRESET_CAVE:
      result = CMX_3D_REVERB_CAVE;
      break;

   case MM_REVERB_ENV_PRESET_ARENA:
      result = CMX_3D_REVERB_ARENA;
      break;

   case MM_REVERB_ENV_PRESET_FOREST:
      result = CMX_3D_REVERB_FOREST;
      break;

   case MM_REVERB_ENV_PRESET_CITY:
      result = CMX_3D_REVERB_CITY;
      break;

   case MM_REVERB_ENV_PRESET_MOUNTAINS:
      result = CMX_3D_REVERB_MOUNTAIN;
      break;

   case MM_REVERB_ENV_PRESET_UNDERWATER:
      result = CMX_3D_REVERB_UNDERWATER;
      break;

   case MM_REVERB_ENV_PRESET_AUDITORIUM:
      result = CMX_3D_REVERB_AUDITORIUM;
      break;

   case MM_REVERB_ENV_PRESET_ALLEY:
      result = CMX_3D_REVERB_ALLEY;
      break;

   case MM_REVERB_ENV_PRESET_HALLWAY:
      result = CMX_3D_REVERB_HALLWAY;
      break;

   case MM_REVERB_ENV_PRESET_HANGAR:
      result = CMX_3D_REVERB_HANGAR;
      break;

   case MM_REVERB_ENV_PRESET_LIVINGROOM:
      result = CMX_3D_REVERB_LIVINGROOM;
      break;

   case MM_REVERB_ENV_PRESET_SMALLROOM:
      result = CMX_3D_REVERB_SMALLROOM;
      break;

   case MM_REVERB_ENV_PRESET_MEDIUMROOM:
      result = CMX_3D_REVERB_MEDIUMROOM;
      break;

   case MM_REVERB_ENV_PRESET_LARGEROOM:
      result = CMX_3D_REVERB_LARGEROOM;
      break;

   case MM_REVERB_ENV_PRESET_MEDIUMHALL:
      result = CMX_3D_REVERB_MEDIUMHALL;
      break;

   case MM_REVERB_ENV_PRESET_LARGEHALL:
      result = CMX_3D_REVERB_LARGEHALL;
      break;

   case MM_REVERB_ENV_PRESET_PLATE:
      result = CMX_3D_REVERB_PLATE;
      break;
   }

   return result;
}

/*=========================================================================

FUNCTION       map_fx2cmx()

DESCRIPTION    Converts CMX enumeration type to Brew enumeration type for reverb preset values

DEPENDENCIES   None

PARAMETERS     uint32:                  CMX reverb preset

RETURN VALUE   cmx_3d_reverb_enum_type: Brew reverb preset

SIDE EFFECTS   None

========================================================================= */
static uint32 map_cmx2fx ( cmx_3d_reverb_enum_type nFx )
{
   uint32 result = MM_REVERB_ENV_PRESET_NONE;

   switch ( nFx )
   {     
   case CMX_3D_REVERB_ROOM:
      result = MM_REVERB_ENV_PRESET_ROOM;
      break;

   case CMX_3D_REVERB_BATHROOM:
      result = MM_REVERB_ENV_PRESET_BATHROOM;
      break;

   case CMX_3D_REVERB_CONCERTHALL:
      result = MM_REVERB_ENV_PRESET_CONCERTHALL;
      break;

   case CMX_3D_REVERB_CAVE:
      result = MM_REVERB_ENV_PRESET_CAVE;
      break;

   case CMX_3D_REVERB_ARENA:
      result = MM_REVERB_ENV_PRESET_ARENA;
      break;

   case CMX_3D_REVERB_FOREST:
      result = MM_REVERB_ENV_PRESET_FOREST;
      break;

   case CMX_3D_REVERB_CITY:
      result = MM_REVERB_ENV_PRESET_CITY;
      break;

   case CMX_3D_REVERB_MOUNTAIN:
      result = MM_REVERB_ENV_PRESET_MOUNTAINS;
      break;

   case CMX_3D_REVERB_UNDERWATER:
      result = MM_REVERB_ENV_PRESET_UNDERWATER;
      break;

   case CMX_3D_REVERB_AUDITORIUM:
      result = MM_REVERB_ENV_PRESET_AUDITORIUM;
      break;

   case CMX_3D_REVERB_ALLEY:
      result = MM_REVERB_ENV_PRESET_ALLEY;
      break;

   case CMX_3D_REVERB_HALLWAY:
      result = MM_REVERB_ENV_PRESET_HALLWAY;
      break;

   case CMX_3D_REVERB_HANGAR:
      result = MM_REVERB_ENV_PRESET_HANGAR;
      break;

   case CMX_3D_REVERB_LIVINGROOM:
      result = MM_REVERB_ENV_PRESET_LIVINGROOM;
      break;

   case CMX_3D_REVERB_SMALLROOM:
      result = MM_REVERB_ENV_PRESET_SMALLROOM;
      break;

   case CMX_3D_REVERB_MEDIUMROOM:
      result = MM_REVERB_ENV_PRESET_MEDIUMROOM;
      break;

   case CMX_3D_REVERB_LARGEROOM:
      result = MM_REVERB_ENV_PRESET_LARGEROOM;
      break;

   case CMX_3D_REVERB_MEDIUMHALL:
      result = MM_REVERB_ENV_PRESET_MEDIUMHALL;
      break;

   case CMX_3D_REVERB_LARGEHALL:
      result = MM_REVERB_ENV_PRESET_LARGEHALL;
      break;

   case CMX_3D_REVERB_PLATE:
      result = MM_REVERB_ENV_PRESET_PLATE;
      break;

   default:
      result = MM_REVERB_ENV_PRESET_NONE;
      break;
   }

   return result;
}
#endif // !defined (FEATURE_CMX_MM_API)

/*=========================================================================

FUNCTION       OEMMediaAudioEnv_SoundCB()

DESCRIPTION    Callback method for ISound operations

DEPENDENCIES   None

PARAMETERS     void *:         Pointer to data identifying the client of the operation
               AEESoundCmd:    Sound command that was issued by the client
               AEESoundStatus: Status code of the operation

RETURN VALUE   None

SIDE EFFECTS   The status code of the operation will be stored in nReturn member of the client object

========================================================================= */
static void OEMMediaAudioEnv_SoundCB ( void * pUser, AEESoundCmd CbType, AEESoundStatus Status, uint32 dwParam )
{
   IMediaAudioEnv * pme = ( IMediaAudioEnv * ) AEEObjectMgr_GetObject (( AEEObjectHandle ) pUser );

   if ( NULL == pme )
   {
      goto Done;
   }
   
   pme->nReturn = EUNSUPPORTED;
   if (( AEE_SOUND_STATUS_CB == CbType ) && ( AEE_SOUND_SUCCESS == Status ))
   {
      pme->nReturn = SUCCESS;
   }

Done:
   rex_set_sigs ( & ui_tcb, AEE_MEDIA_SIG );
}

/*=========================================================================

FUNCTION       OEMMediaAudioEnv_SetParam()

DESCRIPTION    Sets a parameter value in the audio environment.

DEPENDENCIES   None

PARAMETERS     IMediaAudioEnv *: Environment object reference pointer
               int32:            ID of the parameter to be set
               void *:           Associated data for the request
               void *:           Associated data for the request
               void *:           Associated data for the request
 
RETURN VALUE   int:              Status code

SIDE EFFECTS   None

========================================================================= */
static int OEMMediaAudioEnv_SetParm ( IMediaAudioEnv * pme, int32 nParmID, void * p1, void * p2 )
{
   int nRet = SUCCESS;

   switch ( nParmID )
   {
      case MM_AENV_PARM_ENABLE:
         // Return old value
         // Note: since this is a global setting in CMX, and since
         // we cannot query CMX for the current setting, this will
         // not work very well in a multiple environment situation
         // where other environments can change the global setting...
         if ( p2 != NULL )
         {
            *( uint32 * ) p2 = pme->dwEnable;
         }

         // Set new value
         if ( p1 != NULL )
         {
            uint32          dwEnable = *( uint32 * ) p1;

#if defined (FEATURE_CMX_MM_API)
            if (( pme->dwEnable & MM_AENV_ENABLE_3D ) != ( dwEnable & MM_AENV_ENABLE_3D ))
            {
               pme->CmxParam.env_enable.set_cmd = CMX_MM_ENV_SET_POS_ENABLE;
               pme->CmxParam.env_enable.enable = ( dwEnable & MM_AENV_ENABLE_3D ) != 0;
               OEMMediaAudioEnv_SetEnvParam ( pme );
               if ( SUCCESS == ( nRet = pme->nReturn ))
               {
                  pme->dwEnable = dwEnable;
               }
            }
#else
            IMediaAudioFX * pFX = NULL;


            nRet = IMediaAudioEnv_QueryInterface ( pme, AEECLSID_MediaAudioFX, ( void ** ) & pFX );
            if ( nRet != SUCCESS )
            {
               break;
            }

            // disable reverb before 3D
            if ( !( dwEnable & MM_AENV_ENABLE_REVERB ))
            {
               pFX->cmd = CMX_3D_POS_CMD_DISABLE_REVERB;
               nRet = OEMMediaAudioFX_CallAndWait ( pFX );
               if ( nRet != SUCCESS )
               {
                  OEMMediaAudioFX_Release ( pFX );
                  break;
               }

               pme->dwEnable &= ~MM_AENV_ENABLE_REVERB;
            }

            // enable 3D even if only reverb is needed
            if ( dwEnable & ( MM_AENV_ENABLE_3D | MM_AENV_ENABLE_REVERB ))
            {
               pme->cmxParam.enable_cmd.mode = CMX_3D_POS_MODE_ENABLE;
            }
            else
            {
               pme->cmxParam.enable_cmd.mode = CMX_3D_POS_MODE_DISABLE;
            }
            nRet = OEMMediaAudioEnv_CallAndWait ( pme, CMX_3D_POS_CMD_ENABLE );
            if ( nRet != SUCCESS )
            {
               OEMMediaAudioFX_Release ( pFX );
               break;
            }

            pme->dwEnable = ( pme->dwEnable & ~MM_AENV_ENABLE_3D ) | ( dwEnable & MM_AENV_ENABLE_3D );

            if (( MM_REVERB_ENV_PRESET_NONE != pme->nReverbPreset ) && // no reverb preset
                ( REVERB_UNINITIALIZED != pme->nReverbPreset )) // reverb preset uninitialized
            {
               // enable reverb after 3D
               if ( dwEnable & MM_AENV_ENABLE_REVERB )
               {
                  pFX->cmd = CMX_3D_POS_CMD_ENABLE_REVERB;
                  nRet = OEMMediaAudioFX_CallAndWait ( pFX );
                  if ( nRet != SUCCESS )
                  {
                     OEMMediaAudioFX_Release ( pFX );
                     break;
                  }
               }
            }
            pme->dwEnable = dwEnable;
            OEMMediaAudioFX_Release ( pFX );
            OEMMediaAudioEnv_UpdateMediaEnv ( pme );
#endif // defined (FEATURE_CMX_MM_API)
         }
         break;

      case MM_AENV_PARM_LISTENER_POSITION:
         if ( p1 != NULL )
         {
            AEEVector * pVector = ( AEEVector * ) p1;
            uint32      nDuration = ( uint32 ) p2;

#if defined (FEATURE_CMX_MM_API)
            pme->CmxParam.lsnr_pos.set_cmd = CMX_MM_ENV_SET_POS_LISTENER_POS;        
            pme->CmxParam.lsnr_pos.lsnr_pos.x_pos = pVector->x;
            pme->CmxParam.lsnr_pos.lsnr_pos.y_pos = pVector->y;
            pme->CmxParam.lsnr_pos.lsnr_pos.z_pos = pVector->z;
            pme->CmxParam.lsnr_pos.lsnr_pos.duration_ms = nDuration;

            OEMMediaAudioEnv_SetEnvParam ( pme );
            nRet = pme->nReturn;
#else
            
            pme->cmxParam.lsnr_pos_cmd.x_pos = pVector->x;
            pme->cmxParam.lsnr_pos_cmd.y_pos = pVector->y;
            pme->cmxParam.lsnr_pos_cmd.z_pos = pVector->z;
            pme->cmxParam.lsnr_pos_cmd.duration_ms = nDuration;

            nRet = OEMMediaAudioEnv_CallAndWait ( pme, CMX_3D_POS_CMD_SET_LSNR_POS );

            if ( nRet == SUCCESS ) {
              pme->LsnrPosition = *pVector;
            }

#endif // defined (FEATURE_CMX_MM_API)
         }
         else
         {
            nRet = EBADPARM;
         }

         break;

      case MM_AENV_PARM_LISTENER_ORIENTATION:
         if ( p1 != NULL )
         {
            AEEMediaOrientation * pOrientation = ( AEEMediaOrientation * ) p1;
            uint32 nDuration = (uint32) p2;

#if defined (FEATURE_CMX_MM_API)
            pme->CmxParam.lsnr_ori.set_cmd = CMX_MM_ENV_SET_POS_LISTENER_ORI;
            pme->CmxParam.lsnr_ori.lsnr_orient.x_forward = pOrientation->ForwardVector.x;
            pme->CmxParam.lsnr_ori.lsnr_orient.y_forward = pOrientation->ForwardVector.y;
            pme->CmxParam.lsnr_ori.lsnr_orient.z_forward = pOrientation->ForwardVector.z;
            pme->CmxParam.lsnr_ori.lsnr_orient.x_up = pOrientation->UpVector.x;
            pme->CmxParam.lsnr_ori.lsnr_orient.y_up = pOrientation->UpVector.y;
            pme->CmxParam.lsnr_ori.lsnr_orient.z_up = pOrientation->UpVector.z;
            pme->CmxParam.lsnr_ori.lsnr_orient.duration_ms = nDuration;

            OEMMediaAudioEnv_SetEnvParam ( pme );
            nRet = pme->nReturn;
#else
            pme->cmxParam.lsnr_orient_cmd.x_forward = pOrientation->ForwardVector.x;
            pme->cmxParam.lsnr_orient_cmd.y_forward = pOrientation->ForwardVector.y;
            pme->cmxParam.lsnr_orient_cmd.z_forward = pOrientation->ForwardVector.z;
            pme->cmxParam.lsnr_orient_cmd.x_up = pOrientation->UpVector.x;
            pme->cmxParam.lsnr_orient_cmd.y_up = pOrientation->UpVector.y;
            pme->cmxParam.lsnr_orient_cmd.z_up = pOrientation->UpVector.z;
            pme->cmxParam.lsnr_orient_cmd.duration_ms = nDuration;

            nRet = OEMMediaAudioEnv_CallAndWait ( pme, CMX_3D_POS_CMD_SET_LSNR_ORIENT );
#endif // defined (FEATURE_CMX_MM_API)
         }
         else
         {
            nRet = EBADPARM;
         }

         break;

      case MM_AENV_PARM_LISTENER_VELOCITY:
         {
#if defined (FEATURE_CMX_MM_API)
            nRet = EUNSUPPORTED;
#else
            AEEVector * pVelocity = ( AEEVector * ) p1;
            uint32      nDuration = ( uint32 ) p2;

            if ( p1 != NULL )
            {
               pme->cmxParam.lsnr_vel_cmd.x_vel = pVelocity->x;
               pme->cmxParam.lsnr_vel_cmd.y_vel = pVelocity->y;
               pme->cmxParam.lsnr_vel_cmd.z_vel = pVelocity->z;
               pme->cmxParam.lsnr_vel_cmd.duration_ms = nDuration;
               
               nRet = OEMMediaAudioEnv_CallAndWait ( pme, CMX_3D_POS_CMD_SET_LSNR_VEL );

               if ( nRet == SUCCESS ) {
                 pme->LsnrVelocity = *pVelocity;
               }
            }
            else
            {
               nRet = EBADPARM;
            }
#endif // defined (FEATURE_CMX_MM_API)
         }
         break;

      case MM_AENV_PARM_OUTPUT_DEVICE:
         {
            AEESoundDevice eDevice = ( uint32 ) p1;
            AEESoundInfo   si;

            if ( eDevice >= AEE_SOUND_DEVICE_LAST )
            {
               return EBADPARM;
            }

            if ( eDevice != pme->eDevice )
            {
               if ( NULL == pme->piSound )
               {
                  // Create ISound and register callback notify
                  if ( SUCCESS != ISHELL_CreateInstance ( pme->piShell, AEECLSID_SOUND, ( void ** ) & pme->piSound ))
                  {
                     return EUNSUPPORTED;
                  }

                  ISOUND_RegisterNotify ( pme->piSound, OEMMediaAudioEnv_SoundCB, ( void * ) pme->hObject );
               }

               ISOUND_Get ( pme->piSound, & si );
               si.eDevice = eDevice;
               ISOUND_Set ( pme->piSound, & si );
   
               ( void ) rex_clr_sigs ( & ui_tcb, AEE_MEDIA_SIG );
               ISOUND_SetDevice( pme->piSound );
               ( void ) rex_wait( AEE_MEDIA_SIG );

               if ( SUCCESS == pme->nReturn )
               {
                  pme->eDevice = eDevice;
               }

               nRet = pme->nReturn;
            }
         }
         break;

      default:
         nRet = EUNSUPPORTED;
         break;
   }

   return nRet;
}

/*=========================================================================

FUNCTION       OEMMediaAudioEnv_SetParam()

DESCRIPTION    Gets a parameter value from the audio environment.

DEPENDENCIES   None

PARAMETERS     IMediaAudioEnv *: Environment object reference pointer
               int32:            ID of the parameter to be set
               void *:           Pointer to location where the returned data will be stored
               void *:           Pointer to location where the returned data will be stored
               void *:           Pointer to location where the returned data will be stored
 
RETURN VALUE   int:              Status code

SIDE EFFECTS   None

========================================================================= */
static int OEMMediaAudioEnv_GetParm ( IMediaAudioEnv * pme, int32 nParmID, void * p1, void * p2 )
{
   int nRet = EBADPARM;

   switch ( nParmID )
   {
      case MM_AENV_PARM_LISTENER_POSITION:
         if ( p1 != NULL )
         {
            pme->p1 = p1;
#if defined (FEATURE_CMX_MM_API)
            pme->CmxGetParam.get_cmd = CMX_MM_ENV_GET_POS_LISTENER_POS;
            nRet = OEMMediaAudioEnv_GetEnvParam ( pme );
#else
            pme->cmxGetParam.param = CMX_3D_POS_PARAM_LSNR_POS;
            nRet = OEMMediaAudioEnv_GetAndWait ( pme );
#endif // defined (FEATURE_CMX_MM_API)
         }
         break;

      case MM_AENV_PARM_LISTENER_ORIENTATION:
         if ( p1 != NULL )
         {
            pme->p1 = p1;
#if defined (FEATURE_CMX_MM_API)
            pme->CmxGetParam.get_cmd = CMX_MM_ENV_GET_POS_LISTENER_ORI;
            nRet = OEMMediaAudioEnv_GetEnvParam( pme );
#else
            pme->cmxGetParam.param = CMX_3D_POS_PARAM_LSNR_ORIENT;
            nRet = OEMMediaAudioEnv_GetAndWait ( pme );
#endif // defined (FEATURE_CMX_MM_API)
         }
         break;

      case MM_AENV_PARM_LISTENER_VELOCITY:
         if ( p1 != NULL )
         {
            pme->p1 = p1;
#if defined (FEATURE_CMX_MM_API)
            nRet = EUNSUPPORTED;
#else
            pme->cmxGetParam.param = CMX_3D_POS_PARAM_LSNR_VEL;
            nRet = OEMMediaAudioEnv_GetAndWait ( pme );
#endif // defined (FEATURE_CMX_MM_API)
         }

         // Fill in range
         if ( p2 != NULL )
         {
            AEEAudioParmRange * pRange = ( AEEAudioParmRange * ) p2;

            pRange->nMin = 0x0;
            pRange->nMax = 0x7FFFFFFF;
            pRange->nDefault = 0;
            pRange->nStep = 1;
         }
         break;

      case MM_AENV_PARM_OUTPUT_DEVICE:
         if ( p1 != NULL )
         {
            AEESoundDevice *eDevice = ( AEESoundDevice * ) p1;
            AEESoundInfo   si;

            if ( NULL == pme->piSound )
            {
               // Create ISound and register callback notify
               if ( SUCCESS != ISHELL_CreateInstance ( pme->piShell, AEECLSID_SOUND, ( void ** ) & pme->piSound ))
               {
                  return EUNSUPPORTED;
               }

               ISOUND_RegisterNotify ( pme->piSound, OEMMediaAudioEnv_SoundCB, ( void * ) pme->hObject );
            }

            ISOUND_Get ( pme->piSound, & si );
            *eDevice = si.eDevice;
            nRet = SUCCESS;
         }
         break;

      default:
         nRet = EUNSUPPORTED;
         break;
   }

   return nRet;
}

#if !defined (FEATURE_CMX_MM_API)
/*=========================================================================

========================================================================= */
static void IMediaAudioEnv_RestoreParm(IMediaAudioEnv * pme, int nParmID) 
{
  cmx_3d_pos_cmd_enum_type cmx_cmd = (cmx_3d_pos_cmd_enum_type) nParmID;
  cmx_3d_pos_param_type    cmxParam;

  if ( NULL != pme ) {
    switch ( cmx_cmd )
    {
      case CMX_3D_POS_CMD_SET_LSNR_POS:
        cmxParam.lsnr_pos_cmd.x_pos = pme->LsnrPosition.x;
        cmxParam.lsnr_pos_cmd.y_pos = pme->LsnrPosition.y;
        cmxParam.lsnr_pos_cmd.z_pos = pme->LsnrPosition.z;
        cmxParam.lsnr_pos_cmd.duration_ms = 0;
        break;
      case CMX_3D_POS_CMD_SET_LSNR_ORIENT:
        cmxParam.lsnr_orient_cmd.x_forward = pme->LsnrOrientation.ForwardVector.x;
        cmxParam.lsnr_orient_cmd.y_forward = pme->LsnrOrientation.ForwardVector.y;
        cmxParam.lsnr_orient_cmd.z_forward = pme->LsnrOrientation.ForwardVector.z;
        cmxParam.lsnr_orient_cmd.x_up = pme->LsnrOrientation.UpVector.x;
        cmxParam.lsnr_orient_cmd.y_up = pme->LsnrOrientation.UpVector.y;
        cmxParam.lsnr_orient_cmd.z_up = pme->LsnrOrientation.UpVector.z;
        cmxParam.lsnr_orient_cmd.duration_ms = 0;
        break;

      case CMX_3D_POS_CMD_SET_LSNR_VEL:
        cmxParam.lsnr_vel_cmd.x_vel = pme->LsnrVelocity.x;
        cmxParam.lsnr_vel_cmd.y_vel = pme->LsnrVelocity.y;
        cmxParam.lsnr_vel_cmd.z_vel = pme->LsnrVelocity.z;
        cmxParam.lsnr_vel_cmd.duration_ms = 0;
        break;

      case CMX_3D_POS_CMD_ENABLE:
        if (pme->dwEnable & MM_AENV_ENABLE_3D ) {
          cmxParam.enable_cmd.mode = CMX_3D_POS_MODE_ENABLE;
        } else {
          cmxParam.enable_cmd.mode = CMX_3D_POS_MODE_DISABLE;
        }
        break;

      default:
         MSG_ERROR("IMediaAudioEnv_RestoreParm: Invalid CMD", 0, 0, 0);
         return;
    }

    cmx_3d_pos_cmd(cmx_cmd, &cmxParam, NULL, NULL);

  } else {
    MSG_ERROR("IMediaAudioEnv_RestoreParm: NULL pme", 0, 0, 0);
  }
}
/*=========================================================================

========================================================================= */
static int OEMMediaAudioEnv_CallAndWait ( IMediaAudioEnv * pme, cmx_3d_pos_cmd_enum_type nCommand )
{

   switch ( nCommand )
   {
      case CMX_3D_POS_CMD_SET_LSNR_POS:
      case CMX_3D_POS_CMD_SET_LSNR_ORIENT:
      case CMX_3D_POS_CMD_SET_LSNR_VEL:
      case CMX_3D_POS_CMD_ENABLE:
         break;

      default:
         return EUNSUPPORTED;
   }

   ( void ) rex_clr_sigs ( & ui_tcb, AEE_MEDIA_SIG );
   pme->bCMXCb = 0;

   cmx_3d_pos_cmd ( nCommand,
                    & pme->cmxParam,
                    OEMMediaAudioEnv_CMXCmdcb,
                    ( void * ) pme->hObject );

   // UI will wait until released in callback.
  
   ( void ) rex_timed_wait(AEE_MEDIA_SIG,
                           &pme->cmxCmdTimer,
                           AEE_CMX_CMD_TIMEOUT);
   
   if ((rex_get_timer(&pme->cmxCmdTimer) == 0) && 
       (pme->bCMXCb == 0)) {
OEMMM_ENTER_CRITICAL_SECTION()
     IMediaAudioEnv_RestoreParm(pme, nCommand);
     pme->ncVoidCount++;
     pme->nReturn = EFAILED;
OEMMM_LEAVE_CRITICAL_SECTION()
     MSG_ERROR("OEMMediaAudioEnv_CallAndWait command timeout", 0, 0, 0);
   }

   ( void ) rex_clr_timer( &pme->cmxCmdTimer );

   return pme->nReturn;
}


static void OEMMediaAudioEnv_CMXCmdcb ( cmx_status_type status, const void * cpClientData )
{
   IMediaAudioEnv * pme;

   OEMMM_ENTER_CRITICAL_SECTION()

   pme = ( IMediaAudioEnv * ) AEEObjectMgr_GetObject (( AEEObjectHandle ) cpClientData );
   if ( NULL == pme )
   {
      goto Done;
   } else if ( 0 != pme->ncVoidCount ) 
   {
     pme->ncVoidCount--;
     MSG_HIGH("Env cmd voided", 0, 0, 0);
     goto Done;
   }

   pme->nReturn = ( status == CMX_SUCCESS ) ? SUCCESS : EFAILED;
   pme->bCMXCb = 1;

Done:
   // Release UI
   rex_set_sigs ( & ui_tcb, AEE_MEDIA_SIG );

   OEMMM_LEAVE_CRITICAL_SECTION()
}

/*=========================================================================

========================================================================= */
static int OEMMediaAudioEnv_GetAndWait ( IMediaAudioEnv * pme )
{
   ( void ) rex_clr_sigs ( & ui_tcb, AEE_MEDIA_SIG);
   pme->bCMXCb = 0;
   cmx_3d_pos_get_param ( & pme->cmxGetParam,
                          OEMMediaAudioEnv_CMXParamcb,
                          ( void * ) pme->hObject );

   // UI will wait until released in callback.
   ( void ) rex_timed_wait(AEE_MEDIA_SIG,
                           &pme->cmxCmdTimer,
                           AEE_CMX_CMD_TIMEOUT);
   
   if ((rex_get_timer(&pme->cmxCmdTimer) == 0) &&
       (pme->bCMXCb == 0)) {
OEMMM_ENTER_CRITICAL_SECTION()
     pme->nReturn = EFAILED;
     pme->ncVoidCount++;
OEMMM_LEAVE_CRITICAL_SECTION()
     MSG_ERROR("OEMMediaAudioEnv_GetAndWait command timeout", 0, 0, 0);
   }

   return pme->nReturn;
}

static void OEMMediaAudioEnv_CMXParamcb ( cmx_status_type status, cmx_3d_pos_param_enum_type param, cmx_3d_pos_param_setting_type * pSetting, const void * cpClientData )
{
   IMediaAudioEnv * pme;

   OEMMM_ENTER_CRITICAL_SECTION()

   pme = ( IMediaAudioEnv * ) AEEObjectMgr_GetObject (( AEEObjectHandle ) cpClientData );
   if ( NULL == pme )
   {
      goto Done;
   } else if ( 0 != pme->ncVoidCount) {
      pme->ncVoidCount--;
      MSG_HIGH("Environment query voided", 0, 0, 0);
      goto Done;
   }

   if ( CMX_SUCCESS == status )
   {
      pme->nReturn = SUCCESS;
      switch ( param )
      {
         case CMX_3D_POS_PARAM_LSNR_POS:
         {
            AEEVector * pVector = ( AEEVector * ) pme->p1;

            pVector->x = pSetting->position.x_pos;
            pVector->y = pSetting->position.y_pos;
            pVector->z = pSetting->position.z_pos;
            break;
         }
         case CMX_3D_POS_PARAM_LSNR_ORIENT:
         {
            AEEMediaOrientation * pOrientation = ( AEEMediaOrientation * ) pme->p1;

            pOrientation->ForwardVector.x = pSetting->lsnr_orient.x_forward;
            pOrientation->ForwardVector.y = pSetting->lsnr_orient.y_forward;
            pOrientation->ForwardVector.z = pSetting->lsnr_orient.z_forward;
            pOrientation->UpVector.x = pSetting->lsnr_orient.x_up;
            pOrientation->UpVector.y = pSetting->lsnr_orient.y_up;
            pOrientation->UpVector.z = pSetting->lsnr_orient.z_up;
            break;
         }
         case CMX_3D_POS_PARAM_LSNR_VEL:
         {
            AEEVector * pVelocity = ( AEEVector * ) pme->p1;

            pVelocity->x = pSetting->lsnr_vel.x_vel;
            pVelocity->y = pSetting->lsnr_vel.y_vel;
            pVelocity->z = pSetting->lsnr_vel.z_vel;
            break;
         }
         default:
            pme->nReturn = EFAILED;
            break;
      }
   }
   else
   {
      pme->nReturn = EFAILED;
   }
   pme->bCMXCb = 1;

Done:
   // Release UI
   rex_set_sigs ( & ui_tcb, AEE_MEDIA_SIG );

   OEMMM_LEAVE_CRITICAL_SECTION()

}
#endif // !defined (FEATURE_CMX_MM_API)

/*=========================================================================

                     IMediaAudio3D FUNCTION DEFINITIONS

=========================================================================*/

/*==================================================================
FUNCTION       IMediaAudio3D_New()

DESCRIPTION    Constructor of IMediaAudio3D object

DEPENDENCIES   None

PARAMETERS     IShell *: Pointer to Brew environment
               AEECLSID: Class ID of the created object
               void **:  Pointer to location where the created object reference pointer will be stored
               IMedia *: Associated IMedia object reference

RETURN VALUE   int:      Status code

SIDE EFFECTS   None

==================================================================*/
int IMediaAudio3D_New ( IShell * ps, AEECLSID cls, void ** ppif, IMedia * piMedia )
{
   IMediaAudio3D * pme = MALLOC( sizeof( IMediaAudio3D ));

   if ( ppif != NULL )
   {
      *ppif = NULL;
   }
   if ( NULL == pme )
   {
      return ENOMEMORY;
   }

   pme->cpvtAudio3D = & gvtIMediaAudio3D;
   pme->nRefs = 1;
   pme->piShell = ps;
   pme->piMedia = piMedia;
   pme->ncVoidCount = 0;
   rex_def_timer(&pme->cmxCmdTimer, &ui_tcb, AEE_MEDIA_SIG);
   // Set non-0 defaults

   // Register the object with ObjectMgr...
   if ( AEEObjectMgr_Register ( pme, & pme->hObject ))
   {
      OEMMediaAudio3D_Release ( pme );
      return EFAILED;
   }

   IMEDIA_AddRef ( piMedia );
   if ( ppif != NULL )
   {
      *ppif = pme;
   }

   return SUCCESS;
}

/*==================================================================

FUNCTION       OEMMediaAudio3D_AddRef()

DESCRIPTION    Increments reference counf of IMediaAudio3D object

DEPENDENCIES   None

PARAMETERS     IMediaAudio3D *: Object reference pointer

RETURN VALUE   uint32: New reference count

SIDE EFFECTS   None

==================================================================*/
static uint32 OEMMediaAudio3D_AddRef ( IMediaAudio3D * pme )
{
   return ++pme->nRefs;
}

/*==================================================================

FUNCTION       OEMMediaAudio3D_Release()

DESCRIPTION    Destructor of IMediaAudio3D object

DEPENDENCIES   None

PARAMETERS     IMediaAudio3D *: Object reference pointer

RETURN VALUE   uint32: New reference count

SIDE EFFECTS   Only the object reference count is decremented, if the count is greater than 1.
               The object is destroyed once the reference count becomes 0
   
==================================================================*/
static uint32 OEMMediaAudio3D_Release ( IMediaAudio3D * pme )
{
   if ( pme->nRefs > 0 )
   {
      if ( --pme->nRefs )
         return pme->nRefs;

      IMEDIA_Release ( pme->piMedia );
      AEEObjectMgr_Unregister ( pme->hObject );
      (void) rex_clr_timer(&pme->cmxCmdTimer);
      FREE( pme );
   }

   return 0;
}

/*==================================================================

FUNCTION       OEMMediaAudio3D_QueryInterface()

DESCRIPTION    Interface extractor of IMediaAudio3D object

DEPENDENCIES   None

PARAMETERS     IMediaAudio3D *: Object reference pointer
               AEECLSID:        Class ID of the desired interface
               void **:         Pointer to location where the new interface reference pointer will be stored

RETURN VALUE   int:             Status code

SIDE EFFECTS   None
   
==================================================================*/
static int OEMMediaAudio3D_QueryInterface ( IMediaAudio3D * pme, AEECLSID clsIdReq, void ** ppo )
{
   if ( NULL == ppo )
   {
      return EBADPARM;
   }

   if (( NULL != IQI_SELF( pme, clsIdReq, ppo, AEECLSID_MediaAudio3D )) ||
       ( NULL != IQI_SELF( pme, clsIdReq, ppo, AEEIID_MEDIAAUDIO3D )))
   {
      IQI_AddRef ( *ppo );
      return SUCCESS;
   }

   return ECLASSNOTSUPPORT;
}


#if defined (FEATURE_CMX_MM_API)
/*=========================================================================
   
FUNCTION       OEMMediaAudio3D_SetParmCb()

DESCRIPTION    Callback method for set parameter operations on IMediaAudio3D 
               Called from CMX to inform OEM layer of completition of a set operation.

DEPENDENCIES   None

PARAMETERS     cmx_mm_cmd_status_type: Status of the completed operation
               const void *:           Pointer to data identifying the client of the operation
 
RETURN VALUE   None

SIDE EFFECTS   The status code of the operation will be stored in nReturn member of the client object

========================================================================= */
static void OEMMediaAudio3D_SetParmCb ( cmx_mm_cmd_status_type Status, const void * cpClientData )
{
   IMediaAudio3D * pme;

   OEMMM_ENTER_CRITICAL_SECTION()

   pme = ( IMediaAudio3D * ) AEEObjectMgr_GetObject (( AEEObjectHandle ) cpClientData );
   if ( NULL == pme )
   {
      goto Done;
   }

   pme->nReturn = ( CMX_MM_STATUS_SUCCESS == Status ) ? SUCCESS : EFAILED;

Done:
   rex_set_sigs ( & ui_tcb, AEE_MEDIA_SIG );

   OEMMM_LEAVE_CRITICAL_SECTION()
}

/*=========================================================================

FUNCTION       OEMMediaAudio3D_SetParamAndWait()

DESCRIPTION    Sends a set parameter command to CMX.

DEPENDENCIES   CmxParam member of the client must be initialized before calling this function

PARAMETERS     IMediaAudio3D *:                       Client requesting a CMX operation
               const cmx_mm_play_set_param_enum_type: Set command 
 
RETURN VALUE   int: Status code

SIDE EFFECTS   Synchronous operation that suspends the UI task until CMX has finished processing

========================================================================= */
static int OEMMediaAudio3D_SetParamAndWait ( IMediaAudio3D * pme, cmx_mm_play_set_param_enum_type param )
{
   int nRet;

   ( void ) rex_clr_sigs ( & ui_tcb, AEE_MEDIA_SIG );
   nRet = CMediaCMX_SetMedia3DParam ( pme->piMedia, ( int ) param, 
                                     ( void * ) & pme->cmxParam, ( void * ) OEMMediaAudio3D_SetParmCb, ( void * ) pme->hObject );
   if ( nRet == MM_PENDING )
   {
      ( void ) rex_wait ( AEE_MEDIA_SIG );
      nRet = pme->nReturn;
   }
   return nRet;
}
#endif // defined (FEATURE_CMX_MM_API)

/*=========================================================================

FUNCTION       OEMMediaAudio3D_SetParm()

DESCRIPTION    Sets a parameter value in the audio object.

DEPENDENCIES   None

PARAMETERS     IMediaAudio3D *: Object reference pointer
               int32:           ID of the parameter to be set
               void *:          Associated data for the request
               void *:          Associated data for the request
               void *:          Associated data for the request
 
RETURN VALUE   int:             Status code

SIDE EFFECTS   None

========================================================================= */
static int OEMMediaAudio3D_SetParm ( IMediaAudio3D * pme, int32 nParmID, void * p1, void * p2 )
{
   int nRet = SUCCESS;

   switch ( nParmID )
   {
      case MM_A3D_PARM_POSITION:
         if ( p1 != NULL )
         {
            AEEVector * pVector = ( AEEVector * ) p1;
            uint32 nDuration = ( uint32 ) p2;

            pme->cmxParam.src_pos_cmd.x_pos = pVector->x;
            pme->cmxParam.src_pos_cmd.y_pos = pVector->y;
            pme->cmxParam.src_pos_cmd.z_pos = pVector->z;
            pme->cmxParam.src_pos_cmd.duration_ms = nDuration;

#if defined (FEATURE_CMX_MM_API)
            nRet = OEMMediaAudio3D_SetParamAndWait( pme, CMX_MM_PLAY_SET_PARAM_POS_SRC_POS );
#else
            if ( SUCCESS != CMediaMIDIOutQCP_AddCachedParam ( pme->piMedia, CMX_3D_POS_CMD_SET_SRC_POS, ( void * ) & pme->cmxParam ))
            {
               nRet = OEMMediaAudio3D_SetChannel ( pme, & pme->cmxParam.src_pos_cmd.id );
               if ( nRet != SUCCESS )
               {
                  return nRet;

               }
               pme->cmd = CMX_3D_POS_CMD_SET_SRC_POS;
               nRet = OEMMediaAudio3D_CallAndWait ( pme );
            }
#endif // defined (FEATURE_CMX_MM_API)
         }
         else
         {
            nRet = EBADPARM;
         }
         break;

      case MM_A3D_PARM_POLAR_POSITION:
         {
            AEEVector Vector;
            AEEPolarPosition * pPos = ( AEEPolarPosition * ) p1;

            if (( pPos->nElevation < -90 ) || ( pPos->nElevation > 90 ) ||
                ( pPos->nAzimuth < -360 )  || ( pPos->nAzimuth > 360 ))
            {
               nRet = EBADPARM;
            }
            else
            {
               // convert Brew notion of the world to spherical coordinates
               int32 nEl = 90 - pPos->nElevation;
               int32 nAz = pPos->nAzimuth;

               if ( nAz < 0 )
               {
                  nAz += 360;
               }
               nAz = 90 - nAz;
               if ( nAz < 0 )
               {
                  nAz += 360;
               }
    
               Vector.x = ( int32 ) FLTTOINT( FMUL( FMUL(( double ) pPos->nRange, FSIN( FDIV(( double ) nEl, RADANGLE ))), FCOS( FDIV(( double ) nAz, RADANGLE ))));
               Vector.z = -( int32 ) FLTTOINT( FMUL( FMUL(( double ) pPos->nRange, FSIN( FDIV(( double ) nEl, RADANGLE ))), FSIN( FDIV(( double ) nAz, RADANGLE ))));
               Vector.y = ( int32 ) FLTTOINT( FMUL(( double ) pPos->nRange, FCOS( FDIV(( double ) nEl, RADANGLE ))));

               nRet = OEMMediaAudio3D_SetParm ( pme, MM_A3D_PARM_POSITION, & Vector, p2 );
            }
            break;
         }

      case MM_A3D_PARM_MODE:
      {
         uint32 mode = ( uint32 ) p1;

         if ( MM_A3D_MODE_LISTENER_RELATIVE == mode )
         {
            nRet = EUNSUPPORTED;
         }
         else 
         {
#if defined (FEATURE_CMX_MM_API)
            nRet = EUNSUPPORTED;
#else
            pme->cmxParam.src_enable_cmd.enable_mode = ( cmx_3d_pos_mode_enum_type ) mode;
            if ( SUCCESS != CMediaMIDIOutQCP_AddCachedParam ( pme->piMedia, CMX_3D_POS_CMD_SRC_ENABLE, ( void * ) & pme->cmxParam ))
            {
               nRet = OEMMediaAudio3D_SetChannel ( pme, & pme->cmxParam.src_enable_cmd.id );
               if ( nRet != SUCCESS )
               {
                  return nRet;
               }

               pme->cmd = CMX_3D_POS_CMD_SRC_ENABLE;
               nRet = OEMMediaAudio3D_CallAndWait ( pme );
            }
#endif // defined (FEATURE_CMX_MM_API)
         }
         break;
      }

      case MM_A3D_PARM_VOLUME:
      {
         sint31 nVolume = ( sint31 ) p1;
         uint32 nDuration = ( uint32 ) p2;

         if ( 0 < nVolume || -9600 > nVolume )
         {
            nRet = EBADPARM;
         }
         else
         {
            pme->cmxParam.src_volume_cmd.volume = nVolume;
            pme->cmxParam.src_volume_cmd.duration_ms = nDuration;

#if defined (FEATURE_CMX_MM_API)
            nRet = OEMMediaAudio3D_SetParamAndWait( pme, CMX_MM_PLAY_SET_PARAM_POS_SRC_VOL );
#else
            if ( SUCCESS != CMediaMIDIOutQCP_AddCachedParam ( pme->piMedia, CMX_3D_POS_CMD_SET_SRC_VOL, ( void * ) & pme->cmxParam) )
            {
               nRet = OEMMediaAudio3D_SetChannel ( pme, & pme->cmxParam.src_volume_cmd.id );
               if ( nRet != SUCCESS )
               {
                  return nRet;

               }
               pme->cmd = CMX_3D_POS_CMD_SET_SRC_VOL;
               nRet = OEMMediaAudio3D_CallAndWait ( pme );
            }
#endif // defined (FEATURE_CMX_MM_API)
         }
         break;
      }

      case MM_A3D_PARM_VELOCITY:
         if ( p1 != NULL )
         {
#if defined (FEATURE_CMX_MM_API)
            nRet = EUNSUPPORTED;
#else
            AEEVector * pVelocity = ( AEEVector * ) p1;
            uint32 nDuration = (uint32) p2;

            pme->cmxParam.src_vel_cmd.x_vel = pVelocity->x;
            pme->cmxParam.src_vel_cmd.y_vel = pVelocity->y;
            pme->cmxParam.src_vel_cmd.z_vel = pVelocity->z;
            pme->cmxParam.src_vel_cmd.duration_ms = nDuration;

            if ( SUCCESS != CMediaMIDIOutQCP_AddCachedParam ( pme->piMedia, CMX_3D_POS_CMD_SET_SRC_VEL, ( void * ) & pme->cmxParam ))
            {
               nRet = OEMMediaAudio3D_SetChannel ( pme, &pme->cmxParam.src_pos_cmd.id );
               if ( nRet != SUCCESS )
               {
                  return nRet;
               }

               pme->cmd = CMX_3D_POS_CMD_SET_SRC_VEL;
               nRet = OEMMediaAudio3D_CallAndWait ( pme );
            }
#endif // defined (FEATURE_CMX_MM_API)
         }
         else
         {
            nRet = EBADPARM;
         }
         break;

      case MM_A3D_PARM_ROLL_OFF:
         if ( p1 != NULL )
         {
            AEEMediaRollOff * pRolloff = ( AEEMediaRollOff * ) p1;

            pme->cmxParam.src_vol_atten_cmd.min_distance = ( uint32 ) pRolloff->nMinDistance;
            pme->cmxParam.src_vol_atten_cmd.max_distance = ( uint32 ) pRolloff->nMaxDistance;
            pme->cmxParam.src_vol_atten_cmd.scale = ( uint32 ) pRolloff->nRollOffFactor;
            pme->cmxParam.src_vol_atten_cmd.mute_after_max = pRolloff->bMuteAfterMax;

#if defined (FEATURE_CMX_MM_API)
            nRet = OEMMediaAudio3D_SetParamAndWait( pme, CMX_MM_PLAY_SET_PARAM_POS_SRC_VOL_ATTEN );
#else
            if ( SUCCESS != CMediaMIDIOutQCP_AddCachedParam ( pme->piMedia, CMX_3D_POS_CMD_SET_SRC_VOL_ATTEN, ( void * ) & pme->cmxParam ))
            {
               nRet = OEMMediaAudio3D_SetChannel( pme, & pme->cmxParam.src_vol_atten_cmd.id );
               if ( nRet != SUCCESS )
               {
                  return nRet;
               }

               pme->cmd = CMX_3D_POS_CMD_SET_SRC_VOL_ATTEN;
               nRet = OEMMediaAudio3D_CallAndWait ( pme );
            }
#endif // defined (FEATURE_CMX_MM_API)
         }
         else
         {
            nRet = EBADPARM;
         }
         break;

      default:
         nRet = EUNSUPPORTED;
         break;
   }

   return nRet;
}

/*=========================================================================
   
FUNCTION       FATAN()

DESCRIPTION    computes arctan using Maclaurin series to 5 decimal precision

DEPENDENCIES   None

PARAMETERS     double: operand
 
RETURN VALUE   double: arctan( operand )

SIDE EFFECTS   None

========================================================================= */
static double FATAN( double dbX )
{
   double dbReturn = dbX;

   if ( FCMP_LE(( double ) -1.0, dbX ) && FCMP_GE(( double ) 1.0, dbX ))
   {
      // arctan ( x ) = x - x ^ 3 / 3 + x ^ 5 / 5 - x ^ 7 / 7 + x ^ 9 / 9 - ... 
      double dbTerm = dbX;
      int nPow = 3;
      int nSign = -1;

      while ( FCMP_G(( double ) -0.00001, dbTerm ) || FCMP_L(( double ) 0.00001, dbTerm ))
      {
         dbTerm = FDIV( FPOW( dbX, ( double ) nPow ), ( double ) nPow );
         if ( 1 == nSign )
         {
            dbReturn = FADD( dbReturn, dbTerm );
         }
         else
         {
            dbReturn = FSUB( dbReturn, dbTerm );
         }

         nSign = -nSign;
         nPow += 2;
      }
   }
   else
   {
      // | x | > 1, then | 1 / x | < 1, use  arctan ( x ) = Pi / 2 - arctan ( 1 / x )
      if ( FCMP_G( dbX, ( double ) 1.0 ))
      {
         dbReturn = FSUB( FDIV( PI, ( double ) 2.0 ), FATAN( FDIV((double) 1.0, dbX )));
      }
      else
      {
         dbReturn = FSUB( FDIV( PI, ( double ) -2.0 ), FATAN( FDIV((double) 1.0, dbX )));
      }
   }
   return dbReturn;
}

#if defined (FEATURE_CMX_MM_API)
/*=========================================================================

FUNCTION       OEMMediaAudio3D_GetParmCb()

DESCRIPTION    Callback method for get parameter operations on IMediaAudio3D 
               Called from CMX to inform OEM layer of completition of a get operation.

DEPENDENCIES   None

PARAMETERS     cmx_mm_cmd_status_type: Status of the completed operation
               const void *:           Pointer to data identifying the client of the operation
 
RETURN VALUE   None

SIDE EFFECTS   The status code of the operation will be stored in nReturn member of the client object
               The return value(s) of the operation will be copied to requested locations

========================================================================= */
static void OEMMediaAudio3D_GetParmCb ( cmx_mm_cmd_status_type Status, const void * cpClientData )
{
   IMediaAudio3D * pme;

   OEMMM_ENTER_CRITICAL_SECTION()

   pme = ( IMediaAudio3D * ) AEEObjectMgr_GetObject (( AEEObjectHandle ) cpClientData );
   if ( NULL == pme )
   {
      goto Done;
   }

   pme->nReturn = ( CMX_MM_STATUS_SUCCESS == Status ) ? SUCCESS : EFAILED;

   if ( SUCCESS == pme->nReturn )
   {
      switch ( pme->CmxGetParam.audio_spec.get_cmd )
      {
      case CMX_MM_PLAY_GET_PARAM_POS_SRC_POS:
      {
         AEEVector * pVector = ( AEEVector * ) pme->p1;

         pVector->x = pme->CmxGetParam.src_pos.src_pos.x_pos;
         pVector->y = pme->CmxGetParam.src_pos.src_pos.y_pos;
         pVector->z = pme->CmxGetParam.src_pos.src_pos.z_pos;
         break;
      }

      case CMX_MM_PLAY_GET_PARAM_POS_SRC_VOL:
      {
         int32 * pnVolume = ( int32 * ) pme->p1;

         *pnVolume = pme->CmxGetParam.src_vol.src_volume.volume;
         break;
      }

      case CMX_MM_PLAY_GET_PARAM_POS_SRC_VOL_ATTEN:
      {
         AEEMediaRollOff * pRolloff = ( AEEMediaRollOff * ) pme->p1;

         pRolloff->nMinDistance = pme->CmxGetParam.src_vol_atten.src_vol_atten.min_distance;
         pRolloff->nMaxDistance = pme->CmxGetParam.src_vol_atten.src_vol_atten.max_distance;
         pRolloff->nRollOffFactor = pme->CmxGetParam.src_vol_atten.src_vol_atten.scale;
         pRolloff->bMuteAfterMax = pme->CmxGetParam.src_vol_atten.src_vol_atten.mute_after_max;
         break;
      }

      default:
         pme->nReturn = EUNSUPPORTED;
         break;
      }
   }

Done:
   rex_set_sigs ( & ui_tcb, AEE_MEDIA_SIG );

   OEMMM_LEAVE_CRITICAL_SECTION()
}

/*=========================================================================

FUNCTION       OEMMediaAudio3D_GetParamAndWait()

DESCRIPTION    Sends a get parameter command to CMX.

DEPENDENCIES   CmxGetParam member of the client must be initialized before calling this function

PARAMETERS     IMediaAudio3D *:                       Client requesting a CMX operation
               int:                                   Brew get command
               const cmx_mm_play_get_param_enum_type: CMX get command 
 
RETURN VALUE   int: Status code

SIDE EFFECTS   Synchronous operation that suspends the UI task until CMX has finished processing

========================================================================= */
static int OEMMediaAudio3D_GetParamAndWait ( IMediaAudio3D * pme, int nParamID, cmx_mm_play_get_enum_type param )
{
   int nRet;

   pme->CmxGetParam.audio_spec.get_cmd = param;

   ( void ) rex_clr_sigs ( & ui_tcb, AEE_MEDIA_SIG );
   nRet = CMediaCMX_GetMedia3DParam ( pme->piMedia, nParamID,
                                      ( void * ) & pme->CmxGetParam, ( void * ) OEMMediaAudio3D_GetParmCb, ( void * ) pme->hObject );
   if ( nRet == MM_PENDING )
   {
      ( void ) rex_wait ( AEE_MEDIA_SIG );
      nRet = pme->nReturn;
   }
   return nRet;
}
#endif // defined (FEATURE_CMX_MM_API)

/*=========================================================================

FUNCTION       OEMMediaAudio3D_GetParam()

DESCRIPTION    Gets a parameter value from the audio object.

DEPENDENCIES   None

PARAMETERS     IMediaAudio3D *: Object reference pointer
               int32:           ID of the parameter to get
               void *:          Pointer to location where the returned data will be stored
               void *:          Pointer to location where the returned data will be stored
               void *:          Pointer to location where the returned data will be stored
 
RETURN VALUE   int:              Status code

SIDE EFFECTS   None

========================================================================= */
static int OEMMediaAudio3D_GetParm ( IMediaAudio3D * pme, int32 nParmID, void * p1, void * p2 )
{
   int nRet = EBADPARM;

   switch ( nParmID )
   {
      case MM_A3D_PARM_POSITION:
         if ( p1 != NULL )
         {
            pme->p1 = p1;

#if defined (FEATURE_CMX_MM_API)
            nRet = OEMMediaAudio3D_GetParamAndWait ( pme, nParmID, CMX_MM_PLAY_GET_PARAM_POS_SRC_POS );
#else
            if ( SUCCESS == ( nRet = CMediaMIDIOutQCP_CheckCachedParam ( pme->piMedia, CMX_3D_POS_PARAM_SRC_POS, ( void * ) & pme->cmxSetting )))
            {
               AEEVector * pVector = ( AEEVector * ) p1;

               pVector->x = pme->cmxSetting.position.x_pos;
               pVector->y = pme->cmxSetting.position.y_pos;
               pVector->z = pme->cmxSetting.position.z_pos;
            }
            else
            {
               nRet = OEMMediaAudio3D_SetChannel ( pme, & pme->cmxGetParam.src_param.id );
               if ( SUCCESS == nRet )
               {
                  pme->cmxGetParam.src_param.param = CMX_3D_POS_PARAM_SRC_POS;
                  nRet = OEMMediaAudio3D_GetAndWait ( pme );
               }
            }
#endif // defined (FEATURE_CMX_MM_API)
         }
         break;

      case MM_A3D_PARM_POLAR_POSITION:
         if ( p1 != NULL )
         {
            AEEVector Vector;
            AEEPolarPosition * pPos = ( AEEPolarPosition * ) p1;

            if ( SUCCESS == ( nRet = OEMMediaAudio3D_GetParm ( pme, MM_A3D_PARM_POSITION, & Vector, p2 )))
            {
               // Azimuth
               if ( 0 == Vector.x )
               {
                  pPos->nAzimuth = ( 0 > Vector.z ) ? 0 : 180;
               }
               else
               {
                  int32 nAz = -( int32 ) FLTTOINT( FMUL( RADANGLE, FATAN( FDIV(( double ) Vector.z, ( double ) Vector.x ))));
              
                  if ( Vector.x < 0 )
                  {
                     nAz += 180;
                  }
                  if ( nAz < 0 )
                  {
                     nAz += 360;
                  }

                  // convert to Brew notion of the world
                  nAz = 90 - nAz;
                  if ( nAz < 0 )
                  {
                     nAz += 360;
                  }
                  pPos->nAzimuth = nAz;
               }

               // Range
               pPos->nRange = ( int32 ) FLTTOINT( FSQRT( FADD ( FADD( FMUL(( double ) Vector.x, ( double ) Vector.x ), 
                                                                      FMUL(( double ) Vector.y, ( double ) Vector.y )),
                                                                FMUL(( double ) Vector.z, ( double ) Vector.z ))));
               // Elevation
               if ( 0 == Vector.y )
               {
                  pPos->nElevation = 0;
               }
               else
               {
                  int32 nEl = ( int32 ) FLTTOINT( FMUL( RADANGLE, FATAN( FDIV( FSQRT( FADD( FMUL(( double ) Vector.x, ( double ) Vector.x ),
                                                                                            FMUL(( double ) Vector.z, ( double ) Vector.z ))), ( double ) Vector.y ))));
               
                  if ( nEl < 0 )
                  {
                     pPos->nElevation = -90 - nEl;
                  }
                  else
                  {
                     pPos->nElevation = 90 - nEl;
                  }
               }
            }
         }
         break;

      case MM_A3D_PARM_MODE:
         if ( p1 != NULL )
         {
#if defined (FEATURE_CMX_MM_API)
            nRet = EUNSUPPORTED;
#else
            pme->p1 = p1;
            if ( SUCCESS == ( nRet = CMediaMIDIOutQCP_CheckCachedParam ( pme->piMedia, CMX_3D_POS_PARAM_SRC_ENABLED, ( void * ) & pme->cmxSetting )))
            {
               int32* mode = (int32*)p1;

               *mode = pme->cmxSetting.src_enable ? MM_A3D_MODE_NORMAL : MM_A3D_MODE_DISABLE;
            }
            else
            {
               nRet = OEMMediaAudio3D_SetChannel ( pme, & pme->cmxGetParam.src_param.id  ); /* here */
               if ( SUCCESS == nRet )
               {
                  pme->cmxGetParam.src_param.param = CMX_3D_POS_PARAM_SRC_ENABLED;
                  nRet = OEMMediaAudio3D_GetAndWait ( pme );
               }
            }
#endif // defined (FEATURE_CMX_MM_API)
         }
         break;

      case MM_A3D_PARM_VOLUME:
         if ( p1 != NULL )
         {
            pme->p1 = p1;

#if defined (FEATURE_CMX_MM_API)
            nRet = OEMMediaAudio3D_GetParamAndWait ( pme, nParmID, CMX_MM_PLAY_GET_PARAM_POS_SRC_VOL );
#else
            if ( SUCCESS == ( nRet = CMediaMIDIOutQCP_CheckCachedParam ( pme->piMedia, CMX_3D_POS_PARAM_SRC_VOLUME, ( void * ) & pme->cmxSetting )))
            {
               int32 * pnVolume = ( int32 * ) p1;

               *pnVolume = pme->cmxSetting.src_volume.volume;
            }
            else
            {
               nRet = OEMMediaAudio3D_SetChannel ( pme, & pme->cmxGetParam.src_param.id );
               if ( SUCCESS == nRet )
               {
                  pme->cmxGetParam.src_param.param = CMX_3D_POS_PARAM_SRC_VOLUME;
                  nRet = OEMMediaAudio3D_GetAndWait ( pme );
               }
            }
#endif // defined (FEATURE_CMX_MM_API)
         }
         
         // Fill in range
         if ( p2 != NULL )
         {
            AEEAudioParmRange * pRange = ( AEEAudioParmRange * ) p2;

            pRange->nMin = -9600;
            pRange->nMax = 0;
            pRange->nDefault = 0;
            pRange->nStep = 1;
         }
         break;

      case MM_A3D_PARM_VELOCITY:
         if ( p1 != NULL )
         {
#if defined (FEATURE_CMX_MM_API)
            nRet = EUNSUPPORTED;
#else
            pme->p1 = p1;
            if ( SUCCESS == ( nRet = CMediaMIDIOutQCP_CheckCachedParam ( pme->piMedia, CMX_3D_POS_PARAM_SRC_VEL, ( void * ) & pme->cmxSetting )))
            {
               AEEVector * pVelocity = ( AEEVector * ) p1;

               pVelocity->x = pme->cmxSetting.src_vel.x_vel;
               pVelocity->y = pme->cmxSetting.src_vel.y_vel;
               pVelocity->z = pme->cmxSetting.src_vel.z_vel;
            }
            else
            {
               nRet = OEMMediaAudio3D_SetChannel ( pme, & pme->cmxGetParam.src_param.id );
               if ( SUCCESS == nRet )
               {
                  pme->cmxGetParam.src_param.param = CMX_3D_POS_PARAM_SRC_VEL;
                  nRet = OEMMediaAudio3D_GetAndWait ( pme );
               }
            }
#endif // defined (FEATURE_CMX_MM_API)
         }

         // Fill in range
         if ( p2 != NULL )
         {
            AEEAudioParmRange * pRange = ( AEEAudioParmRange * ) p2;

            pRange->nMin = 0x0;
            pRange->nMax = 0x7FFFFFFF;
            pRange->nDefault = 0;
            pRange->nStep = 1;
         }
         break;

      case MM_A3D_PARM_ROLL_OFF:
         if ( p1 != NULL )
         {
            pme->p1 = p1;

#if defined (FEATURE_CMX_MM_API)
            nRet = OEMMediaAudio3D_GetParamAndWait ( pme, nParmID, CMX_MM_PLAY_GET_PARAM_POS_SRC_VOL_ATTEN );
#else
            if ( SUCCESS == ( nRet = CMediaMIDIOutQCP_CheckCachedParam ( pme->piMedia, CMX_3D_POS_PARAM_SRC_VOL_ATTEN, ( void * ) & pme->cmxSetting )))
            {
               AEEMediaRollOff* pRolloff = (AEEMediaRollOff*)p1;

               pRolloff->nMinDistance = pme->cmxSetting.src_vol_atten.min_distance;
               pRolloff->nMaxDistance = pme->cmxSetting.src_vol_atten.max_distance;
               pRolloff->nRollOffFactor = pme->cmxSetting.src_vol_atten.scale;
               pRolloff->bMuteAfterMax = pme->cmxSetting.src_vol_atten.mute_after_max;
            }
            else
            {
               nRet = OEMMediaAudio3D_SetChannel ( pme, & pme->cmxGetParam.src_param.id );
               if ( SUCCESS == nRet )
               {
                  pme->cmxGetParam.src_param.param = CMX_3D_POS_PARAM_SRC_VOL_ATTEN;
                  nRet = OEMMediaAudio3D_GetAndWait ( pme );
               }
            }
#endif // defined (FEATURE_CMX_MM_API)
         }

         // Fill in range
         if ( p2 != NULL )
         {
            AEEAudioParmRange * pRange = ( AEEAudioParmRange * ) p2;

            pRange->nMin = 0;
            pRange->nMax = 0x7FFFFFFF;
            pRange->nDefault = 1000;
            pRange->nStep = 1;
         }
         break;

      default:
         nRet = EUNSUPPORTED;
         break;
   }

   return nRet;
}

#if !defined (FEATURE_CMX_MM_API)
/*=========================================================================

========================================================================= */
static int OEMMediaAudio3D_CallAndWait ( IMediaAudio3D * pme )
{
   switch ( pme->cmd )
   {
      case CMX_3D_POS_CMD_SET_SRC_POS:
      case CMX_3D_POS_CMD_SET_SRC_VOL:
      case CMX_3D_POS_CMD_SET_SRC_VOL_ATTEN:
      case CMX_3D_POS_CMD_SET_SRC_VEL:
      case CMX_3D_POS_CMD_SET_SRC_REVERB_VOL:
      case CMX_3D_POS_CMD_SRC_ENABLE:
         break;

      default:
         return EUNSUPPORTED;
   }

   ( void ) rex_clr_sigs ( & ui_tcb, AEE_MEDIA_SIG );
   pme->bCMXCb = 0;
   cmx_3d_pos_cmd ( pme->cmd,
                    & pme->cmxParam,
                    OEMMediaAudio3D_CMXCmdcb,
                    ( void * ) pme->hObject );

   // UI will wait until released in callback.
   
   ( void ) rex_timed_wait(AEE_MEDIA_SIG,
                           &pme->cmxCmdTimer,
                           AEE_CMX_CMD_TIMEOUT);
   
   if ((rex_get_timer(&pme->cmxCmdTimer) == 0) &&
       (pme->bCMXCb == 0)) {
OEMMM_ENTER_CRITICAL_SECTION()
     CMediaMIDIOutQCP_RestoreParm(pme->piMedia, pme->cmd);
     pme->nReturn = EFAILED;
     pme->ncVoidCount++;
OEMMM_LEAVE_CRITICAL_SECTION()
     MSG_ERROR("IMediaAudio3D_CallAndWait command timeout", 0, 0, 0);
   }

   ( void ) rex_clr_timer( &pme->cmxCmdTimer );

   return pme->nReturn;
}


static void OEMMediaAudio3D_CMXCmdcb ( cmx_status_type status, const void * cpClientData )
{
   IMediaAudio3D * pme;

   OEMMM_ENTER_CRITICAL_SECTION()

   pme = ( IMediaAudio3D * ) AEEObjectMgr_GetObject(( AEEObjectHandle ) cpClientData );
   if ( NULL == pme )
   {
      goto Done;
   } else if ( 0 != pme->ncVoidCount ) {
     pme->ncVoidCount--;
     MSG_HIGH("3D cmd voided", 0, 0, 0);
     goto Done;
   }

   pme->nReturn = ( CMX_SUCCESS == status ) ? SUCCESS : EFAILED;

   // On CMX_FAILURE
   //    Cache the request if it failed because media stopped playing while processing 
   //    this request. We sent the request to cmx because the media was playing,
   //    but if the media is no longer playing when we get this failed response callback
   //    we'll cache the parameter and treat this as success.
   //
   // On CMX_SUCCESS
   //    Cache the request if media is no longer playing in order to
   //    make sure that Get Param gets the correct value. 
   //    just in case DSP dropped the parameter at the end of a media play
   //    If the media is still playing save the last successfull command

   if ( SUCCESS == CMediaMIDIOutQCP_AddCachedParam ( pme->piMedia, pme->cmd, ( void * ) & pme->cmxParam ))
   {
      // media is no longer playing
      pme->nReturn = SUCCESS;
   }
   else if ( CMX_SUCCESS == status )
   {
      // set param succeeded and media is still playing
      CMediaMIDIOutQCP_SaveParam ( pme->piMedia, pme->cmd, ( void * ) & pme->cmxParam );
   }
   pme->bCMXCb = 1;

Done:
   // Release UI
   rex_set_sigs ( & ui_tcb, AEE_MEDIA_SIG );

   OEMMM_LEAVE_CRITICAL_SECTION()
}


/*=========================================================================

========================================================================= */
static int OEMMediaAudio3D_GetAndWait ( IMediaAudio3D * pme )
{
   ( void ) rex_clr_sigs ( & ui_tcb, AEE_MEDIA_SIG );
   pme->bCMXCb = 0;
   cmx_3d_pos_get_param  ( & pme->cmxGetParam,
                           OEMMediaAudio3D_CMXParamcb,
                           ( void * ) pme->hObject );

   // UI will wait until released in callback.
   ( void ) rex_timed_wait(AEE_MEDIA_SIG,
                           &pme->cmxCmdTimer,
                           AEE_CMX_CMD_TIMEOUT);
   
   if ((rex_get_timer(&pme->cmxCmdTimer) == 0) &&
       (pme->bCMXCb == 0)) {
OEMMM_ENTER_CRITICAL_SECTION()
     pme->nReturn = EFAILED;
     pme->ncVoidCount++;
OEMMM_LEAVE_CRITICAL_SECTION()
     MSG_ERROR("IMediaAudio3D_GetAndWait command timeout", 0, 0, 0);
   }

   return pme->nReturn;
}


static void OEMMediaAudio3D_CMXParamcb ( cmx_status_type status, cmx_3d_pos_param_enum_type param, cmx_3d_pos_param_setting_type * pSetting, const void * cpClientData )
{
   IMediaAudio3D * pme;

   OEMMM_ENTER_CRITICAL_SECTION()

   pme = ( IMediaAudio3D * ) AEEObjectMgr_GetObject(( AEEObjectHandle ) cpClientData );
   if ( NULL == pme )
   {
      goto Done;
   } else if ( 0 != pme->ncVoidCount) {
      pme->ncVoidCount--;
      MSG_HIGH("3D query voided", 0, 0, 0);
      goto Done;
   }

   if ( status == CMX_SUCCESS )
   {
      pme->nReturn = SUCCESS;
      switch ( param )
      {
         case CMX_3D_POS_PARAM_SRC_POS:
         {
            AEEVector * pVector = ( AEEVector * ) pme->p1;

            pVector->x = pSetting->position.x_pos;
            pVector->y = pSetting->position.y_pos;
            pVector->z = pSetting->position.z_pos;
            break;
         }
         case CMX_3D_POS_PARAM_SRC_VOLUME:
         {
            int32 * pnVolume = ( int32 * )pme->p1;

            *pnVolume = pSetting->src_volume.volume;
            break;
         }
         case CMX_3D_POS_PARAM_SRC_VOL_ATTEN:
         {
            AEEMediaRollOff * pRolloff = ( AEEMediaRollOff * ) pme->p1;

            pRolloff->nMinDistance = pSetting->src_vol_atten.min_distance;
            pRolloff->nMaxDistance = pSetting->src_vol_atten.max_distance;
            pRolloff->nRollOffFactor = pSetting->src_vol_atten.scale;
            pRolloff->bMuteAfterMax = pSetting->src_vol_atten.mute_after_max;
            break;
         }
         case CMX_3D_POS_PARAM_SRC_VEL:
         {
            AEEVector * pVelocity = ( AEEVector * ) pme->p1;

            pVelocity->x = pSetting->src_vel.x_vel;
            pVelocity->y = pSetting->src_vel.y_vel;
            pVelocity->z = pSetting->src_vel.z_vel;
            break;
         }
         case CMX_3D_POS_PARAM_SRC_REVERB_VOL:
         {
            int32 * pnVolume = ( int32 * ) pme->p1;

            *pnVolume = pSetting->src_reverb_vol.volume;
            break;
         }
         case CMX_3D_POS_PARAM_SRC_ENABLED:
         {
            int32 * pnMode = ( int32 * ) pme->p1;

            *pnMode = pSetting->src_enable ? MM_A3D_MODE_NORMAL : MM_A3D_MODE_DISABLE;

            break;
         }
         default:
            pme->nReturn = EFAILED;
            break;
      }
   }
   else
   {
      pme->nReturn = EFAILED;
   }
   pme->bCMXCb = 1;

Done:
   // Release UI
   rex_set_sigs ( & ui_tcb, AEE_MEDIA_SIG );

   OEMMM_LEAVE_CRITICAL_SECTION()
}

static int OEMMediaAudio3D_SetChannel ( IMediaAudio3D * pme, void ** ppChannel )
{
   return CMediaMIDIOutQCP_GetMediaParmOEM ( pme->piMedia, MM_PARM_OEM_CHANNEL, ( int32 * ) ppChannel, 0 );
}
#endif // !defined (FEATURE_CMX_MM_API)


/*=========================================================================

                     IMediaAudioFX FUNCTION DEFINITIONS

=========================================================================*/

/*==================================================================

FUNCTION       OEMMediaAudioEnvFX_New()

DESCRIPTION    Constructor of IMediaAudioFX object via QueryInterface() on an IMediaAudioEnv object

DEPENDENCIES   None

PARAMETERS     IShell *:         Pointer to Brew environment
               AEECLSID:         Class ID of the created object
               void **:          Pointer to location where the created object reference pointer will be stored
               IMediaAudioEnv *: Pointer to the audio environment of the created objects

RETURN VALUE   int:              Status code

SIDE EFFECTS   None

==================================================================*/
int OEMMediaAudioEnvFX_New ( IShell * ps, AEECLSID cls, void ** ppif, IMediaAudioEnv * pEnv )
{
   IMediaAudioFX * pme = MALLOC( sizeof( IMediaAudioFX ));

   if ( NULL == pme )
   {
      return ENOMEMORY;
   }

   if ( ppif != NULL )
   {
      *ppif = NULL;
   }

   pme->cpvtAudioFX = & gvtIMediaAudioFX;
   pme->nRefs = 1;
   pme->piShell = ps;
   pme->pEnv = pEnv;

   if ( pEnv != NULL )
   {
      IMediaAudioEnv_AddRef ( pEnv );
   }

   // Register the object with ObjectMgr...
   if ( AEEObjectMgr_Register ( pme, &pme->hObject ))
   {
      OEMMediaAudioFX_Release ( pme );
      return EFAILED;
   }

   if ( ppif != NULL )
   {
      *ppif = pme;
   }

   return SUCCESS;
}


/*==================================================================

FUNCTION       IMediaAudioMediaFX_New()

DESCRIPTION    Constructor of IMediaAudioFX object via QueryInterface() on an IMedia object

DEPENDENCIES   None

PARAMETERS     IShell *: Pointer to Brew environment
               AEECLSID: Class ID of the created object
               void **:  Pointer to location where the created object reference pointer will be stored
               IMedia *: Pointer to the associated media of the created objects

RETURN VALUE   int:      Status code

SIDE EFFECTS   None

==================================================================*/
int IMediaAudioMediaFX_New ( IShell * ps, AEECLSID cls, void ** ppif, IMedia * piMedia )
{
   IMediaAudioFX * pme = NULL;
    
   if ( ppif != NULL )
   {
      *ppif = NULL;
   }

   ( void ) OEMMediaAudioEnvFX_New ( ps, cls, ( void ** ) & pme, NULL );
   if ( NULL == pme )
   {
      return ENOMEMORY;
   }
   
   pme->piMedia = piMedia;
   rex_def_timer(&pme->cmxCmdTimer, &ui_tcb, AEE_MEDIA_SIG);

   IMEDIA_AddRef ( piMedia );

   if ( ppif != NULL )
   {
      *ppif = pme;
   }

   return SUCCESS;
}

/*==================================================================
FUNCTION       OEMMediaAudioFX_AddRef()

DESCRIPTION    Increments reference counf of IMediaAudioFX object

DEPENDENCIES   None

PARAMETERS     IMediaAudioFX *: Object reference pointer

RETURN VALUE   uint32: New reference count

SIDE EFFECTS   None

==================================================================*/
static uint32 OEMMediaAudioFX_AddRef ( IMediaAudioFX * pme )
{
   return ++pme->nRefs;
}

/*==================================================================

FUNCTION       OEMMediaAudioFX_Release()

DESCRIPTION    Destructor of IMediaAudioFX object

DEPENDENCIES   None

PARAMETERS     IMediaAudioFX *: Object reference pointer

RETURN VALUE   uint32: New reference count

SIDE EFFECTS   Only the object reference count is decremented, if the count is greater than 1.
               The object is destroyed once the reference count becomes 0
  
==================================================================*/
static uint32 OEMMediaAudioFX_Release ( IMediaAudioFX * pme )
{
   if ( pme->nRefs > 0 )
   {
      if ( --pme->nRefs )
      {
         return pme->nRefs;
      }

      if ( NULL != pme->piMedia )
      {
         IMEDIA_Release ( pme->piMedia );
      }
      if ( NULL != pme->pEnv )
      {
         IMediaAudioEnv_Release ( pme->pEnv );
      }

      AEEObjectMgr_Unregister ( pme->hObject );
      (void) rex_clr_timer(&pme->cmxCmdTimer);
      FREE ( pme );
   }

   return 0;
}


/*==================================================================

FUNCTION       OEMMediaAudioFX_QueryInterface()

DESCRIPTION    Interface extractor of IMediaAudioFX object

DEPENDENCIES   None

PARAMETERS     IMediaAudioFX *: Object reference pointer
               AEECLSID:        Class ID of the desired interface
               void **:         Pointer to location where the new interface reference pointer will be stored

RETURN VALUE   int:              Status code

SIDE EFFECTS   None
      
==================================================================*/
static int OEMMediaAudioFX_QueryInterface ( IMediaAudioFX * pme, AEECLSID clsIdReq, void ** ppo )
{
   if ( NULL == ppo )
   {
      return EBADPARM;
   }

if (( NULL != IQI_SELF ( pme, clsIdReq, ppo, AEECLSID_MediaAudioFX )) ||
       ( NULL != IQI_SELF ( pme, clsIdReq, ppo, AEEIID_MEDIAAUDIOFX )))
   {
      IQI_AddRef ( *ppo );
      return SUCCESS;
   }

   return ECLASSNOTSUPPORT;
}

/*=========================================================================

FUNCTION       OEMMediaAudioFX_SetParm()

DESCRIPTION    Sets a parameter value in the audio object.

DEPENDENCIES   None

PARAMETERS     IMediaAudioFX *: Object reference pointer
               int32:           ID of the parameter to be set
               void *:          Associated data for the request
               void *:          Associated data for the request
               void *:          Associated data for the request
 
RETURN VALUE   int:             Status code

SIDE EFFECTS   None

========================================================================= */
static int OEMMediaAudioFX_SetParm ( IMediaAudioFX * pme, int32 nParmID, void * p1, void * p2 )
{
   int nRet = SUCCESS;

   switch ( nParmID )
   {
      case MM_AFX_PARM_REVERB_DAMPING_FACTOR:
         if ((( uint32 ) p1 < 1 ) || (( uint32 ) p1 > 1000000 ))
         {
            nRet = EBADPARM;
         }
         else if ( pme->piMedia != NULL )
         {
            nRet = EUNSUPPORTED;
         }
         else if ( pme->pEnv->dwReverbDampingFactor != ( uint32 ) p1 )
         {
#if defined (FEATURE_CMX_MM_API)
            nRet = EUNSUPPORTED;
#else
            pme->cmxParam.reverb_ratio_cmd.decay_ratio = ( uint32 ) p1; 
            pme->cmd = CMX_3D_POS_CMD_SET_REVERB_RATIO;
            nRet = OEMMediaAudioFX_CallAndWait ( pme );  
            if ( SUCCESS == nRet )
            {
               pme->pEnv->dwReverbDampingFactor = ( uint32 ) p1;
            }
#endif // defined (FEATURE_CMX_MM_API)
         }
         break;

      case MM_AFX_PARM_REVERB_DECAY_TIME:
         if ( pme->piMedia != NULL )
         {
            nRet = EUNSUPPORTED;
         }
         else if ( pme->pEnv->dwReverbDecayTime != ( uint32 ) p1 )
         {
#if defined (FEATURE_CMX_MM_API)
            nRet = EUNSUPPORTED;
#else
            pme->cmxParam.reverb_decay_cmd.decay_ms = ( uint32 ) p1; 
            pme->cmd = CMX_3D_POS_CMD_SET_REVERB_TIME;
            nRet = OEMMediaAudioFX_CallAndWait ( pme );
            if ( SUCCESS == nRet )
            {
               pme->pEnv->dwReverbDecayTime = ( uint32 ) p1;
            }
#endif // defined (FEATURE_CMX_MM_API)
         }
         break;

      case MM_AFX_PARM_REVERB_PRESET:
         if ((( int32 ) p1 < MM_REVERB_ENV_PRESET_NONE ) || (( int32 ) p1 > MM_REVERB_ENV_PRESET_PLATE ))
         {
            nRet = EBADPARM;
         }
         else if ( pme->piMedia != NULL )
         {
            nRet = EUNSUPPORTED;
         }
         // skip repetitive setting of reverb to same value
         else  if ( pme->pEnv->nReverbPreset != ( int32 ) p1 )
         {
#if defined (FEATURE_CMX_MM_API)
            nRet = EUNSUPPORTED;
#else
            if ( MM_REVERB_ENV_PRESET_NONE == ( int32 ) p1 )
            {
               // disable reverb
               pme->cmd = CMX_3D_POS_CMD_DISABLE_REVERB;
               nRet = OEMMediaAudioFX_CallAndWait ( pme );
            }
            else
            {
               uint32 dwEnable;

               pme->cmxParam.reverb_cmd.env_type = map_fx2cmx (( int32 ) p1 ); 
               pme->cmd = CMX_3D_POS_CMD_SET_REVERB_PRESET;
               nRet = OEMMediaAudioFX_CallAndWait ( pme );
               if ( SUCCESS == nRet )
               {
                  // preset was previously set to MM_REVERB_ENV_PRESET_NONE ?
                 if (( MM_REVERB_ENV_PRESET_NONE == pme->pEnv->nReverbPreset ) || // no reverb preset
                     ( REVERB_UNINITIALIZED == pme->pEnv->nReverbPreset )) // reverb preset uninitialized
                  {
                     nRet = IMediaAudioEnv_Enable ( pme->pEnv, NULL, & dwEnable );
                     if (( SUCCESS == nRet ) && ( MM_AENV_ENABLE_REVERB & dwEnable ))
                     {
                        pme->cmd = CMX_3D_POS_CMD_ENABLE_REVERB;
                        nRet = OEMMediaAudioFX_CallAndWait ( pme );
                     }
                  }
               }
            }
            if ( SUCCESS == nRet )
            {
               pme->pEnv->nReverbPreset = ( int32 ) p1;
            }
#endif // defined (FEATURE_CMX_MM_API)
         }
         break;

      case MM_AFX_PARM_REVERB_GAIN:
         if ((( int32 ) p1 > 0 ) || (( int32 ) p1  < -9600 ))
         {
            nRet = EBADPARM;
         }
         else if ( pme->piMedia != NULL )
         {
#if defined (FEATURE_CMX_MM_API)
            nRet = EUNSUPPORTED;
#else
            int32 nVolume = ( int32 ) p1;

            pme->cmxParam.src_reverb_vol_cmd.volume = nVolume;

            if ( SUCCESS != CMediaMIDIOutQCP_AddCachedParam ( pme->piMedia, CMX_3D_POS_CMD_SET_SRC_REVERB_VOL, ( void * ) & pme->cmxParam ))
            {
               nRet = OEMMediaAudio3D_SetChannel (( IMediaAudio3D * ) pme, & pme->cmxParam.src_reverb_vol_cmd.id );
               if ( nRet != SUCCESS )
               {
                  return nRet;
               }

               pme->cmd = CMX_3D_POS_CMD_SET_SRC_REVERB_VOL;
               nRet = OEMMediaAudio3D_CallAndWait (( IMediaAudio3D * ) pme );
            }
#endif // defined (FEATURE_CMX_MM_API)
         }
         else if ( pme->pEnv->nReverbGain != ( int32 ) p1 )
         {
#if defined (FEATURE_CMX_MM_API)
            nRet = EUNSUPPORTED;
#else 
            pme->cmxParam.reverb_level_cmd.reverb_gain = ( sint31 ) p1;
            pme->cmd = CMX_3D_POS_CMD_SET_REVERB_LEVEL;
            nRet = OEMMediaAudioFX_CallAndWait ( pme );
        
            if ( SUCCESS == nRet )
            {
               pme->pEnv->nReverbGain = ( uint32 ) p1;
            }
#endif // defined (FEATURE_CMX_MM_API)
         }
         break;

      default:
         return EUNSUPPORTED;
   }

   return nRet;
}

/*=========================================================================

FUNCTION       OEMMediaAudioFX_GetParam()

DESCRIPTION    Gets a parameter value from the audio object.

DEPENDENCIES   None

PARAMETERS     IMediaAudioFX *: Object reference pointer
               int32:           ID of the parameter to get
               void *:          Pointer to location where the returned data will be stored
               void *:          Pointer to location where the returned data will be stored
               void *:          Pointer to location where the returned data will be stored
 
RETURN VALUE   int:              Status code

SIDE EFFECTS   None

========================================================================= */
static int OEMMediaAudioFX_GetParm ( IMediaAudioFX * pme, int32 nParmID, void * p1, void * p2 )
{
   int nRet = EBADPARM;

   switch ( nParmID )
   {
      case MM_AFX_PARM_REVERB_DAMPING_FACTOR:
         if ( p1 != NULL )
         {
            if ( pme->piMedia != NULL )
            {
               nRet = EUNSUPPORTED;
            }
            else
            {
#if defined (FEATURE_CMX_MM_API)
               nRet = EUNSUPPORTED;
#else
               pme->cmxGetParam.param = CMX_3D_POS_PARAM_REVERB;
               nRet = OEMMediaAudioFX_GetAndWait ( pme );
               *( uint32 * ) p1 = pme->pEnv->dwReverbDampingFactor;
#endif // defined (FEATURE_CMX_MM_API)
            }
         }

         // Fill in range
         if ( p2 != NULL )
         {
            AEEAudioParmRange* pRange = ( AEEAudioParmRange * ) p2;

            pRange->nMin = 1;
            pRange->nMax = 1000000;
            pRange->nDefault = 1000;
            pRange->nStep = 1;
         }
         break;

      case MM_AFX_PARM_REVERB_DECAY_TIME:
         if ( p1 != NULL )
         {
            if ( pme->piMedia != NULL )
            {
               nRet = EUNSUPPORTED;
            }
            else
            {
#if defined (FEATURE_CMX_MM_API)
               nRet = EUNSUPPORTED;
#else
               pme->cmxGetParam.param = CMX_3D_POS_PARAM_REVERB;
               nRet = OEMMediaAudioFX_GetAndWait ( pme );
               *( int32 * ) p1 = pme->pEnv->dwReverbDecayTime;
#endif // defined (FEATURE_CMX_MM_API)
            }
         }

         // Fill in range
         if ( p2 != NULL )
         {
            AEEAudioParmRange* pRange = ( AEEAudioParmRange * ) p2;

            pRange->nMin = 0;
            pRange->nMax = 0x7FFFFFFF;
            pRange->nDefault = 1490;
            pRange->nStep = 1;
         }
         break;

      case MM_AFX_PARM_REVERB_PRESET:
         if ( p1 != NULL )
         {
            if ( pme->piMedia != NULL )
            {
               nRet = EUNSUPPORTED;
            }
            else
            {
#if defined (FEATURE_CMX_MM_API)
               nRet = EUNSUPPORTED;
#else
               nRet = SUCCESS;
               if ( MM_REVERB_ENV_PRESET_NONE != pme->pEnv->nReverbPreset )
               {
                  pme->cmxGetParam.param = CMX_3D_POS_PARAM_REVERB;
                  nRet = OEMMediaAudioFX_GetAndWait ( pme );
               }
               *( uint32 * ) p1 = pme->pEnv->nReverbPreset;
#endif // defined (FEATURE_CMX_MM_API)
            }
         }
         break;

      case MM_AFX_PARM_REVERB_GAIN:
         if ( p1 != NULL )
         {
            if ( pme->piMedia != NULL )
            {
#if defined (FEATURE_CMX_MM_API)
               nRet = EUNSUPPORTED;
#else
               if ( SUCCESS == ( nRet = CMediaMIDIOutQCP_CheckCachedParam ( pme->piMedia, CMX_3D_POS_PARAM_SRC_REVERB_VOL, ( void * ) & pme->cmxSetting )))
               {
                  *( int32 * ) p1 = pme->cmxSetting.src_reverb_vol.volume;
               }
               else
               {
                  pme->p1 = p1;

                  nRet = OEMMediaAudio3D_SetChannel (( IMediaAudio3D * ) pme, & pme->cmxGetParam.src_param.id );
                  if ( SUCCESS == nRet )
                  {
                     pme->cmxGetParam.src_param.param = CMX_3D_POS_PARAM_SRC_REVERB_VOL;
                     nRet = OEMMediaAudio3D_GetAndWait (( IMediaAudio3D * ) pme );
                  }
               }
#endif // defined (FEATURE_CMX_MM_API)
            }
            else
            {
#if defined (FEATURE_CMX_MM_API)
               nRet = EUNSUPPORTED;
#else
               pme->cmxGetParam.param = CMX_3D_POS_PARAM_REVERB;
               nRet = OEMMediaAudioFX_GetAndWait ( pme );
               *( int32 * ) p1 = pme->pEnv->nReverbGain;
#endif // defined (FEATURE_CMX_MM_API)
            }
         }

         // Fill in range
         if ( p2 != NULL )
         {
            AEEAudioParmRange* pRange = ( AEEAudioParmRange * ) p2;

            pRange->nMin = -9600;
            pRange->nMax = 0;
            pRange->nDefault = ( pme->piMedia != NULL ) ? -9600 : 0;
            pRange->nStep = 1;
         }
         break;

      default:
         nRet = EUNSUPPORTED;
   }

   return nRet;
}

#if !defined (FEATURE_CMX_MM_API)
/*=========================================================================

========================================================================= */
static void OEMMediaAudioFX_RestoreParm(IMediaAudioFX * pme) {
  cmx_3d_pos_param_type cmxParam;

  if (pme != NULL) 
  {
    switch (pme->cmd) {
      case CMX_3D_POS_CMD_ENABLE_REVERB:
      case CMX_3D_POS_CMD_DISABLE_REVERB: 
        if (pme->pEnv->dwEnable & MM_AENV_ENABLE_REVERB) {
          if ((pme->pEnv->nReverbPreset != MM_REVERB_ENV_PRESET_NONE) &&
              (pme->pEnv->nReverbPreset != REVERB_UNINITIALIZED)) {
            cmx_3d_pos_cmd(CMX_3D_POS_CMD_ENABLE_REVERB, NULL, NULL, NULL);
          }
        } else {
          cmx_3d_pos_cmd(CMX_3D_POS_CMD_DISABLE_REVERB, NULL, NULL, NULL);
        }
        break;
      case CMX_3D_POS_CMD_SET_REVERB_PRESET:
          if ((pme->pEnv->nReverbPreset == MM_REVERB_ENV_PRESET_NONE) ||
              (pme->pEnv->nReverbPreset == REVERB_UNINITIALIZED)) {
            cmx_3d_pos_cmd(CMX_3D_POS_CMD_DISABLE_REVERB, NULL, NULL, NULL);
          } else {
            cmxParam.reverb_cmd.env_type = map_fx2cmx( pme->pEnv->nReverbPreset );
            cmx_3d_pos_cmd(CMX_3D_POS_CMD_SET_REVERB_PRESET, NULL, NULL, NULL);
          }
          break;
      case CMX_3D_POS_CMD_SET_REVERB_LEVEL:
        cmxParam.reverb_level_cmd.reverb_gain = pme->pEnv->nReverbGain;
        cmx_3d_pos_cmd ( pme->cmd, & cmxParam, NULL, NULL );
        break;
      case CMX_3D_POS_CMD_SET_REVERB_TIME:
        cmxParam.reverb_decay_cmd.decay_ms = pme->pEnv->dwReverbDecayTime;
        cmx_3d_pos_cmd( pme->cmd, & cmxParam, NULL, NULL );
        break;
      case CMX_3D_POS_CMD_SET_REVERB_RATIO:
        cmxParam.reverb_ratio_cmd.decay_ratio = pme->pEnv->dwReverbDampingFactor;
        cmx_3d_pos_cmd( pme->cmd, & cmxParam, NULL, NULL );
        break;
      default:
        MSG_ERROR("OEMMediaAudioFX_RestoreParm: Illegal FX cmd", 0, 0, 0);
    }
  }
  else 
  {
    MSG_ERROR("OEMMediaAudioFX_RestoreParm: NULL pme", 0, 0, 0);
  }
}

/*=========================================================================

========================================================================= */
static int OEMMediaAudioFX_CallAndWait(IMediaAudioFX * pme)
{
   if (pme->piMedia)
   {
      return EUNSUPPORTED;
   }

   switch (pme->cmd)
   {
      case CMX_3D_POS_CMD_ENABLE_REVERB:
      case CMX_3D_POS_CMD_DISABLE_REVERB:
      case CMX_3D_POS_CMD_SET_REVERB_PRESET:
      case CMX_3D_POS_CMD_SET_REVERB_LEVEL:
      case CMX_3D_POS_CMD_SET_REVERB_TIME:
      case CMX_3D_POS_CMD_SET_REVERB_RATIO:
         break;

      default:
         return EUNSUPPORTED;
   }

   ( void ) rex_clr_sigs ( & ui_tcb, AEE_MEDIA_SIG );
   pme->bCMXCb = 0;
   cmx_3d_pos_cmd ( pme->cmd,
                    & pme->cmxParam,
                    OEMMediaAudioFX_CMXCmdcb,
                    ( void * ) pme->hObject );

   // UI will wait until released in callback.
   
   ( void ) rex_timed_wait(AEE_MEDIA_SIG,
                          &pme->cmxCmdTimer,
                          AEE_CMX_CMD_TIMEOUT);
   
   if ((rex_get_timer(&pme->cmxCmdTimer) == 0) &&
       (pme->bCMXCb == 0)) {
OEMMM_ENTER_CRITICAL_SECTION()
     OEMMediaAudioFX_RestoreParm(pme);
     pme->nReturn = EFAILED;
     pme->ncVoidCount++;
OEMMM_LEAVE_CRITICAL_SECTION()
     MSG_ERROR("OEMMediaAudioFX_CallAndWait command timeout", 0, 0, 0);
   }

   ( void ) rex_clr_timer( &pme->cmxCmdTimer );

   return pme->nReturn;
}


static void OEMMediaAudioFX_CMXCmdcb (cmx_status_type status, const void * cpClientData)
{
   IMediaAudioFX * pme;

   OEMMM_ENTER_CRITICAL_SECTION()

   pme = (IMediaAudioFX*)AEEObjectMgr_GetObject((AEEObjectHandle) cpClientData);
   if (!pme) 
   {
     goto Done;
   }
   else if ( 0 != pme->ncVoidCount) 
   {
     pme->ncVoidCount--;
     MSG_HIGH("FX cmd voided", 0, 0, 0);
     goto Done;
   }
   
   pme->nReturn = (status == CMX_SUCCESS) ? SUCCESS : EFAILED;
   pme->bCMXCb = 1;

Done:
   // Release UI
   rex_set_sigs ( & ui_tcb, AEE_MEDIA_SIG );

   OEMMM_LEAVE_CRITICAL_SECTION()
}


/*=========================================================================

========================================================================= */
static int OEMMediaAudioFX_GetAndWait(IMediaAudioFX * pme)
{
   if (pme->piMedia)
   {
      return EUNSUPPORTED;
   }

   ( void ) rex_clr_sigs ( & ui_tcb, AEE_MEDIA_SIG );
   pme->bCMXCb = 0;
   cmx_3d_pos_get_param ( & pme->cmxGetParam,
                          OEMMediaAudioFX_CMXParamcb,
                          ( void *  )pme->hObject );

   // UI will wait until released in callback.
   ( void ) rex_timed_wait(AEE_MEDIA_SIG,
                           &pme->cmxCmdTimer,
                           AEE_CMX_CMD_TIMEOUT);
  
   if ((rex_get_timer(&pme->cmxCmdTimer) == 0) &&
       (pme->bCMXCb == 0)) {
OEMMM_ENTER_CRITICAL_SECTION()
     pme->nReturn = EFAILED;
     pme->ncVoidCount++;
OEMMM_LEAVE_CRITICAL_SECTION()
     MSG_ERROR("IMediaAudioFX_GetAndWait command timeout", 0, 0, 0);
   }

   return pme->nReturn;
}


static void OEMMediaAudioFX_CMXParamcb (cmx_status_type status, cmx_3d_pos_param_enum_type param, cmx_3d_pos_param_setting_type * pSetting, const void * cpClientData)
{
   IMediaAudioFX * pme;

   OEMMM_ENTER_CRITICAL_SECTION()

   pme = (IMediaAudioFX*)AEEObjectMgr_GetObject((AEEObjectHandle) cpClientData);
   if (!pme)
   {
     goto Done;
   }
   else if ( 0 != pme->ncVoidCount) 
   {
      pme->ncVoidCount--;
      MSG_HIGH("FX query voided", 0, 0, 0);
      goto Done;
   }

   if (status == CMX_SUCCESS)
   {
      pme->nReturn = SUCCESS;
      switch (param)
      {
      case CMX_3D_POS_PARAM_REVERB:
         pme->pEnv->nReverbPreset = map_cmx2fx( pSetting->reverb_param.env_type );
         pme->pEnv->nReverbGain = ( int32 ) pSetting->reverb_param.reverb_gain;
         pme->pEnv->dwReverbDecayTime = pSetting->reverb_param.decay_ms;
         pme->pEnv->dwReverbDampingFactor = pSetting->reverb_param.decay_ratio;
         break;

      default:
         pme->nReturn = EFAILED;
         break;
      }
   }
   else
   {
      pme->nReturn = EFAILED;
   }
   pme->bCMXCb = 1;
Done:
   // Release UI
   rex_set_sigs ( & ui_tcb, AEE_MEDIA_SIG );

   OEMMM_LEAVE_CRITICAL_SECTION()
}
#endif // !defined (FEATURE_CMX_MM_API)
#endif //defined(FEATURE_BREW_MULTIMEDIA) && defined(FEATURE_QAUDIOFX)
