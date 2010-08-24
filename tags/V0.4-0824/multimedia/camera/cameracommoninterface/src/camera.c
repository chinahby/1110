/* =========================================================================
                 Camera

   DESCRIPTION
      Software layer which provides a common interface to camera services
   functions.  Camera services functions are functions which can apply to
   one or more of the following applications: camera, camcorder, or qvp.  To
   cleanly separate unique camera service functions for a given application,
   a layer is added to the camera services interface which indirectly calls
   the appropriate application specific support function for the common
   camera service inteface function.

   Design Note:

   Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/6k/cameracommoninterface/main/latest/src/camera.c#3 $ $DateTime: 2009/04/13 23:57:33 $ $Author: c_caluru $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 04/07/09  cg      Re-Enabling Feature FEATURE_CAMERA_BURST_MODE for LCU 1.x for supporting LGEs Best Picture and Multishot AEC requirement
 12/18/08  sm     Added support for multi raw preview capture
 03/07/07  zl     Updated FEATURE_CAMERA_MULTI_SENSOR support.
 11/08/06  dhaval Removed the camera_start_record prototype change.
 10/30/06  dhaval Changed prototype of camera_start_record routine to pass
                  space limit value
 09/01/06  kadhir Removed unwanted Cross Compilation Dependency
                  When qcamcorder Feature Disabled
 06/20/06  ven    Cleaned up the unused offline snapshot routines meant to do
                  offline snapshot from raw image in EFS.
 06/19/06  jv     Removed camera_enable_bestshot_mode and
                  camera_disable_bestshot_mode. Now use camera_set_parm to
                  enable or disable bestshot mode.
 06/09/06  jch    Added camera_svcs_take_multiple_pictures and related functions.
 05/02/06  dle    Added FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE and
                  FEATURE_CAMERA_BESTSHOT_MODE, default to not supported.
                  Also put FEATURE_CAMERA back.
 04/20/06  sch    Display preview frame at user specified x,y coordinates,
                  CR91980.
 01/29/06  ali    Added a new parameter to camera_start_record
 01/16/06  dle    Fixed all lint problems.
                  Removed FEATURE_CAMERA_ENCODE_PROPERTIES.
                  Reject _start_stand_alone(), _stop_stand_alone(), and
                  _encode_picture_stand_alone(), _color_convert(). Not
                  applicable for 6550.
 12/01/05  jj     FEATURE_CAMERA_SVCS_DIAG around #include in case they
                  not only preprocess but also remove the _diag.{c,h} files.
 11/30/05  anb    Modified camera_cm_call_event_cb to support multiple calls.
 09/02/05  vlc    Added new CM interface to support in call voice hand off.
 08/15/05  cl     Added support for setting multiple sets of uuid.
 05/09/05  mvd    Removed FEATURE_CAMERA_BRIDLE
 04/05/05  drl    Revised camera_set_dimensions for consistant use between
                  camera and camcorder apps.
 03/24/05  mvd    Added camera_set_parm_2
 03/22/05  sai    Added support for offline snapshot.
 03/16/05  drl    Added V7 parameters to camera_set_dimensions for encode
                  width and height
 02/25/05  sch    Fixed the missing returns in *_focus_window () routines.
 02/25/05  sch    Added *_focus_window () routines.
 02/24/05  drl    Changed interface to camera_start_record to add user specified
                  bitrate.
 02/23/05  wh     Go back to #99
 02/22/05  wh/dw  User bitrate support.
 01/20/05  sch    -Zooming support
                  -Single window focus API
                  -Fixed compilation failure when
                   FEATURE_CAMERA_ENCODE_PROPERTIES is defined.
 01/17/05  drl    Moved previous contents of camera.c to camera_svcs.c.
                  Implemented a function table indirect calling scheme to
                  separate camera, camcorder, and qvp s/w.
 ---- see camera_svcs.c for previous revision history of camera.c
============================================================================*/



/*============================================================================
                        INCLUDE FILES
============================================================================*/
#include "camera.h"
#ifdef FEATURE_CAMERA
#include "camera_svcs.h"
#ifdef    FEATURE_CAMERA_SVCS_DIAG
#include "camera_svcs_diag.h"
#endif  //FEATURE_CAMERA_SVCS_DIAG
#include "cm.h"

/*============================================================================
                     GLOBAL DATA DEFINITIONS
============================================================================*/
camerai_func_type   camerai_client [CAMERAI_CLIENT_TYPE_LAST];
camerai_client_type camerai_current_client;
camera_phone_state_type camera_phone_state = NOT_IN_VOICE_CALL;
static boolean camera_in_voice_call_flg[CM_CALL_ID_MAX];

/*============================================================================
                     LOCAL DATA DEFINITIONS
============================================================================*/
static cm_client_id_type camera_cm_client = 0;

static void camera_cm_call_event_cb
(
  cm_call_event_e_type          ph_event,
  const cm_mm_call_info_s_type *ph_info_ptr
);

/*============================================================================
                     LOCAL FUNCTIONS DECLARATIONS
============================================================================*/



/*===========================================================================

FUNCTION      CAMERA_INIT

DESCRIPTION
   Initialize function tables which point to the various application support
   functions.  Call overall powerup initialization routine.

DEPENDENCIES
   None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camera_init (void)
{
  uint16 i;
  camerai_init ();

  camerai_client [camerai_current_client].camera_init ();
#ifndef CAMERA_USES_SOFTDSP
  #ifdef  FEATURE_CAMERA_SVCS_DIAG
  camera_svcs_diag_init();
  #endif//FEATURE_CAMERA_SVCS_DIAG
#endif
  /* Initialize the camera_in_voice_call_flg array */
  for (i = 0; i < CM_CALL_ID_MAX; i++)
  {
    camera_in_voice_call_flg[i] = FALSE;
  }

  /* Register with CM, required to monitor phone state.
  */
  camera_phone_state = NOT_IN_VOICE_CALL;
  if (cm_client_init( CM_CLIENT_TYPE_CAMERA, &camera_cm_client ) == CM_CLIENT_OK)
  {
    if (cm_mm_client_call_reg(camera_cm_client,
                              camera_cm_call_event_cb,
                              CM_CLIENT_EVENT_REG,
                              CM_CALL_EVENT_END,
                              CM_CALL_EVENT_CONNECT,
                              NULL /* not applicable to Camera */
       ) != CM_CLIENT_OK)
    {
      MSG_ERROR("Failure in cm_client_ph_reg",0,0,0);
    }
    else
    {
      if (cm_client_act(camera_cm_client) != CM_CLIENT_OK)
      {
        MSG_ERROR("Failure in cm_client_act",0,0,0);
      }
    }
  }
  else
  {
    MSG_ERROR("Failure in cm_client_init",0,0,0);
    camera_cm_client = 0;
  }
}

#ifdef FEATURE_CAMERA_MULTI_SENSOR
/*===========================================================================

FUNCTION      CAMERA_GET_SENSORS

DESCRIPTION
              Retrieve number of camera sensors and a pointer to the info
              table.

              Synchronous function executed in the callers context.

DEPENDENCIES
  None

RETURN VALUE
  Number of camera sensors

SIDE EFFECTS
  None

===========================================================================*/
uint16 camera_get_sensors(camera_info_type **sensor_info)
{
  MSG_MED("camera_get_sensors", 0, 0, 0);

  if (sensor_info == NULL)
  {
    MSG_FATAL("Null pointer: sensor_info", 0, 0, 0);
    return 0;
  }

  *sensor_info = camera_sensors;
  return camera_number_of_sensors;
} /* camera_get_sensors */
#endif /* FEATURE_CAMERA_MULTI_SENSOR */

#ifdef FEATURE_CAMERA_MULTI_SENSOR
/*===========================================================================

FUNCTION      CAMERA_SELECT_SENSOR

DESCRIPTION
              Set camera_sensor_id to sensor_id.

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_select_sensor(uint16 sensor_id)
{
  extern camera_ret_code_type camera_svcs_select_sensor(uint16 sensor_id);
  return camera_svcs_select_sensor(sensor_id);
} /* camera_select_sensor */
#endif /* FEATURE_CAMERA_MULTI_SENSOR */

/*===========================================================================

FUNCTION      CAMERA_START

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_start (camera_cb_f_type callback,
                                   void            *client_data)
{
  return camerai_client [camerai_current_client].camera_start
                                     (callback, client_data);
}

/*===========================================================================

FUNCTION      CAMERA_STOP

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_stop (camera_cb_f_type callback,
                                  void            *client_data)
{
  return camerai_client [camerai_current_client].camera_stop
                                     (callback, client_data);
}

/*===========================================================================

FUNCTION      CAMERA_START_STAND_ALONE

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_start_stand_alone (camera_cb_f_type callback,
                                               void            *client_data)
{
  /*lint -save -e715 -e818 parameters not referenced in dummy function */
  return CAMERA_NOT_SUPPORTED;
  /*lint -restore */
}

/*===========================================================================

FUNCTION      CAMERA_STOP_STAND_ALONE

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_stop_stand_alone (camera_cb_f_type callback,
                                              void            *client_data)
{
  /*lint -save -e715 parameters not referenced in dummy function */
  /*lint -e818 pointer could be const */
  return CAMERA_NOT_SUPPORTED;
  /*lint -restore */
}

/*===========================================================================

FUNCTION      CAMERA_SET_DIMENSIONS

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_set_dimensions (uint16           picture_width,
                                            uint16           picture_height,
                                            uint16           display_width,
#ifdef FEATURE_CAMERA_V7
                                            uint16           display_height,
#endif
                                            camera_cb_f_type callback,
                                            void            *client_data)
{
  return camerai_client [camerai_current_client].camera_set_dimensions
                                     (picture_width,
                                      picture_height,
                                      display_width,
#ifdef FEATURE_CAMERA_V7
                                      display_height,
#endif
                                      callback,
                                      client_data);
}

/*===========================================================================

FUNCTION      CAMERA_START_PREVIEW

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_start_preview (
    camera_cb_f_type    callback,
    void               *client_data)
{
  return camerai_client [camerai_current_client].camera_start_preview
                                     (
                                      callback,
                                      client_data
                                     );
}

/*===========================================================================

FUNCTION      CAMERA_STOP_PREVIEW

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_stop_preview (void)
{
  return camerai_client [camerai_current_client].camera_stop_preview ();
}

/*===========================================================================

FUNCTION      CAMERA_TAKE_PICTURE

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_take_picture
   (
     camera_cb_f_type    callback,
     void               *client_data
   )
{
  return camerai_client [camerai_current_client].camera_take_picture
                      (
                       callback,
                       client_data
                      );
}

/*===========================================================================

FUNCTION      CAMERA_ENCODE_PICTURE

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_encode_picture (camera_frame_type  *frame,
                                            camera_handle_type *handle,
                                            camera_cb_f_type    callback,
                                            void               *client_data)
{
  return camerai_client [camerai_current_client].camera_encode_picture
                                              (frame,
                                               handle,
                                               callback,
                                               client_data);
}

/*===========================================================================

FUNCTION      CAMERA_ENCODE_PICTURE_STAND_ALONE

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_encode_picture_stand_alone
   (int32               file_size,
    camera_frame_type  *frame,
    camera_handle_type *handle,
    camera_cb_f_type    callback,
    void               *client_data)
{
  /*lint -save -e715 -e818 parameters not referenced in dummy function */
  return CAMERA_NOT_SUPPORTED;
  /*lint -restore */
}

/*===========================================================================

FUNCTION      CAMERA_COLOR_CONVERT

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_color_convert (camera_frame_type *frame,
                                           camera_format_type format,
                                           uint32             length,
                                           byte              *buffer,
                                           camera_cb_f_type   callback,
                                           void              *client_data)
{
  /*lint -save -e715 -e818 parameters not referenced in dummy function */
  return CAMERA_NOT_SUPPORTED;
  /*lint -restore */
}

/*===========================================================================

FUNCTION      CAMERA_GET_THUMBNAIL_PROPERTIES

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_get_thumbnail_properties (uint32 *width,
                                                      uint32 *height,
                                                      uint32 *quality)
{
  return camerai_client [camerai_current_client].camera_get_thumbnail_properties
                     (width,
                      height,
                      quality);
}

/*===========================================================================

FUNCTION      CAMERA_SET_THUMBNAIL_PROPERTIES

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_set_thumbnail_properties (uint32 width,
                                                      uint32 height,
                                                      uint32 quality)
{
  return camerai_client [camerai_current_client].camera_set_thumbnail_properties
                     (width,
                      height,
                      quality);
}

/*===========================================================================

FUNCTION      CAMERA_START_FOCUS

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_start_focus (camera_focus_e_type focus,
                                         camera_cb_f_type    callback,
                                         void               *client_data)
{
  return camerai_client [camerai_current_client].camera_start_focus
                     (focus,
                      callback,
                      client_data);
}

/*===========================================================================

FUNCTION      CAMERA_STOP_FOCUS

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_stop_focus (void)
{
  return camerai_client [camerai_current_client].camera_stop_focus ();
}

/*===========================================================================

FUNCTION      CAMERA_ENABLE_FRAME_CALLBACK

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_enable_frame_callback (void)
{
  return camerai_client [camerai_current_client].camera_enable_frame_callback ();
}

/*===========================================================================

FUNCTION      CAMERA_DISABLE_FRAME_CALLBACK

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_disable_frame_callback (void)
{
  return camerai_client [camerai_current_client].camera_disable_frame_callback ();
}

/*===========================================================================

FUNCTION      CAMERA_RELEASE_FRAME

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_release_frame (void)
{
  return camerai_client [camerai_current_client].camera_release_frame ();
}

/*===========================================================================

FUNCTION      CAMERA_SET_OVERLAY

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_set_overlay (uint16            dx,
                                         uint16            dy,
                                         uint32            transparent_color,
                                         void             *overlay_bmp,
                                         camera_cb_f_type  callback,
                                         void             *client_data)
{
  return camerai_client [camerai_current_client].camera_set_overlay
            (dx,
             dy,
             transparent_color,
             overlay_bmp,
             callback,
             client_data);
}

/*===========================================================================

FUNCTION      CAMERA_CLR_OVERLAY

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_clr_overlay (camera_cb_f_type callback,
                                         void            *client_data)
{
  return camerai_client [camerai_current_client].camera_clr_overlay
                     (callback,
                      client_data);
}

/*===========================================================================

FUNCTION      CAMERA_SET_ICON_ARRAY

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_set_icon_array (uint32          dx,
                                            uint32          dy,
                                            uint32          transparent_color,
                                            ipl_icon_type **icon_array)
{
  return camerai_client [camerai_current_client].camera_set_icon_array
                     (dx,
                      dy,
                      transparent_color,
                      icon_array);
}

/*===========================================================================

FUNCTION      CAMERA_CLR_ICON_ARRAY

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_clr_icon_array (void)
{
  return camerai_client [camerai_current_client].camera_clr_icon_array ();
}

/*===========================================================================

FUNCTION      CAMERA_SET_POSITION

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_set_position (camera_position_type *position,
                                          camera_cb_f_type      callback,
                                          void                 *client_data)
{
  return camerai_client [camerai_current_client].camera_set_position
                     (position,
                      callback,
                      client_data);
}

/*===========================================================================

FUNCTION      CAMERA_SET_EXIF_TAG

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_set_exif_tag (camera_exif_tag_type *tag,
                                          camera_cb_f_type      callback,
                                          void                 *client_data)
{
  return camerai_client [camerai_current_client].camera_set_exif_tag
                     (tag,
                      callback,
                      client_data);
}

/*===========================================================================

FUNCTION      CAMERA_BLT

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_blt (camera_frame_type *frame,
                                 uint16             dst_dx,
                                 uint16             dst_dy,
                                 void              *dst_ptr,
                                 uint16             x,
                                 uint16             y,
                                 uint16             dx,
                                 uint16             dy)
{
  return camerai_client [camerai_current_client].camera_blt
                     (frame,
                      dst_dx,
                      dst_dy,
                      dst_ptr,
                      x,
                      y,
                      dx,
                      dy);
}

/*===========================================================================

FUNCTION      CAMERA_BLT_EXT

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_blt_ext (camera_frame_type *frame,
                                     uint16             dst_dx,
                                     uint16             dst_dy,
                                     void              *dst_ptr,
                                     uint16             x,
                                     uint16             y,
                                     uint16             dx,
                                     uint16             dy,
                                     camera_format_type format)
{
  /*lint -save -e715 function param not referenced */
  /*lint -e818 pointer could be const */
  return camerai_client [camerai_current_client].camera_blt_ext
                     (frame,
                      dst_dx,
                      dst_dy,
                      dst_ptr,
                      x,
                      y,
                      dx,
                      dy,
                      format);
  /*lint -restore */
}

/*===========================================================================

FUNCTION      CAMERA_SET_PARM

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_set_parm (camera_parm_type id,
                                      int32            parm,
                                      camera_cb_f_type callback,
                                      void            *client_data)
{
  return camerai_client [camerai_current_client].camera_set_parm
                     (id,
                      parm,
                      callback,
                      client_data);
}

/*===========================================================================

FUNCTION      CAMERA_SET_PARM_2

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_set_parm_2 (camera_parm_type id,
                                        int32            parm1,
                                        int32            parm2,
                                        camera_cb_f_type callback,
                                        void            *client_data)
{
  return camerai_client [camerai_current_client].camera_set_parm_2
                     (id,
                      parm1,
                      parm2,
                      callback,
                      client_data);
}


/*===========================================================================

FUNCTION      CAMERA_GET_INFO

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
void camera_get_info (camera_info_type *info)
{
  camerai_client [camerai_current_client].camera_get_info (info);
}

/*===========================================================================

FUNCTION      CAMERA_GET_PARM

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_get_parm (camera_parm_type       id,
                                      camera_parm_info_type *parm)
{
  return camerai_client [camerai_current_client].camera_get_parm (id, parm);
}

/*===========================================================================

FUNCTION      CAMERA_GET_PARM_2

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_get_parm_2 (camera_parm_type      id,
                                        int32                *parm1_ptr,
                                        int32                *parm2_ptr)
{
  return camerai_client [camerai_current_client].camera_get_parm_2 (id,
                                                                    parm1_ptr,
                                                                    parm2_ptr);
}

/*===========================================================================

FUNCTION      CAMERA_PROCESS_FUNC

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
void camera_process_func (uint32 func,
                          void  *callback,
                          void  *client_data,
                          uint32 info1,
                          uint32 info2,
                          uint32 info3,
                          uint32 info4)
{
  camerai_client [camerai_current_client].camera_process_func
                     ((camera_func_type)func,
                      callback,
                      client_data,
                      info1,
                      info2,
                      info3,
                      info4);
}

/*===========================================================================

FUNCTION      CAMERA_PROCESS_QDSP_MSG

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
void camera_process_qdsp_msg (uint32 msg,
                              void  *buffer,
                              uint32 size)
{
/*lint -save -e715 function param not referenced */
  camerai_client [camerai_current_client].camera_process_qdsp_msg
                     (msg,
                      buffer);
/*lint -restore */
}


/*===========================================================================

FUNCTION      CAMERA_RELEASE_PICTURE

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_release_picture (void)
{
  return camerai_client [camerai_current_client].camera_release_picture ();
}




#if defined FEATURE_VIDEO_ENCODE || defined FEATURE_QVPHONE

/*===========================================================================

FUNCTION      CAMERA_VIDEO_HANDSHAKE

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
void camera_video_handshake (uint32 code)
{
  camerai_client [camerai_current_client].camera_video_handshake (code);
}
#endif /* FEATURE_VIDEO_ENCODE || FEATURE_QVPHONE */
#ifdef FEATURE_VIDEO_ENCODE
/*===========================================================================

FUNCTION      CAMERA_START_RECORD

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_start_record
  (
    video_fmt_type               file_format,
    video_fmt_stream_video_type  video_format,
    video_fmt_brand_type         video_brand,
    boolean                      short_header,
    video_fmt_stream_audio_type  audio_format,
#ifdef FEATURE_CAMERA_V770
    boolean                      bAudioPostCard,
#endif
#ifdef FEATURE_VIDEO_MULTI_UUID
    video_uuid_info_type        *uuid_info,
#else /* FEATURE_VIDEO_MULTI_UUID */
    void                        *uuid_data,
    uint32                       uuid_data_size,
    uint8                       *uuid_atom,
#endif /* FEATURE_VIDEO_MULTI_UUID */
    video_profile_type           profile,
#ifdef FEATURE_CAMERA_V7
    int32                        user_bitrate,     /* bitrate specified by usr */
#endif
    video_handle_type           *destination,
    camera_cb_f_type             callback,
    void                        *client_data
   )
{
  return camerai_client [camerai_current_client].camera_start_record
                     (
                      file_format,
                      video_format,
                      video_brand,
                      short_header,
                      audio_format,
#ifdef FEATURE_CAMERA_V770
                      bAudioPostCard,
#endif
#ifdef FEATURE_VIDEO_MULTI_UUID
                      uuid_info,
#else /* FEATURE_VIDEO_MULTI_UUID */
                      uuid_data,
                      uuid_data_size,
                      uuid_atom,
#endif /* FEATURE_VIDEO_MULTI_UUID */
                      profile,
#ifdef FEATURE_CAMERA_V7
                      user_bitrate,
#endif
                      destination,
                      callback,
                      client_data
                     );
}

/*===========================================================================

FUNCTION      CAMERA_PAUSE_RECORD

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_pause_record (void)
{
  return camerai_client [camerai_current_client].camera_pause_record ();
}

/*===========================================================================

FUNCTION      CAMERA_RESUME_RECORD

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_resume_record (void)
{
  return camerai_client [camerai_current_client].camera_resume_record ();
}

/*===========================================================================

FUNCTION      CAMERA_STOP_RECORD

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_stop_record (void)
{
  return camerai_client [camerai_current_client].camera_stop_record ();
}

#endif /* FEATURE_VIDEO_ENCODE */



#ifdef FEATURE_QVPHONE

/*===========================================================================

FUNCTION      CAMERA_ENABLE_QVP

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_enable_qvp
   (void              **cb,
    camera_sensor_type *sensor_type,
    boolean             use_ycbcr1,
    boolean             arm_kick_backend)
{
  return camerai_client [camerai_current_client].camera_enable_qvp
                     (cb,
                      sensor_type,
                      use_ycbcr1,
                      arm_kick_backend);
}

/*===========================================================================

FUNCTION      CAMERA_DISABLE_QVP

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_disable_qvp (void)
{
  return camerai_client [camerai_current_client].camera_disable_qvp ();
}

/*===========================================================================

FUNCTION      CAMERA_START_QVP_ENCODE

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_start_qvp_encode
   (
    camera_cb_f_type    callback,
    void               *client_data)
{
  return camerai_client [camerai_current_client].camera_start_qvp_encode
                     (
                      callback,
                      client_data);
}

/*===========================================================================

FUNCTION      CAMERA_STOP_QVP_ENCODE

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_stop_qvp_encode (void)
{
  return camerai_client [camerai_current_client].camera_stop_qvp_encode ();
}

/*===========================================================================

FUNCTION      CAMERA_QVP_RESET

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
void camera_qvp_reset (void)
{
  camerai_client [camerai_current_client].camera_qvp_reset ();
}

#endif /* FEATURE_QVPHONE */

#ifdef FEATURE_MP4_TRANSCODER
/*===========================================================================

FUNCTION      CAMERA_ENABLE_OFFLINE

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
camera_ret_code_type camera_enable_offline (void)
{
  return camerai_client [camerai_current_client].camera_enable_offline ();
}
#endif /* FEATURE_MP4_TRANSCODER */

#ifdef FEATURE_PNG_ENCODER
/*===========================================================================

FUNCTION      CAMERA_PROCESS_PNGE_MSG

DESCRIPTION
   Call the appropriate application specific support function.  Use the
   client identifier to find the correct corresponding routine.

DEPENDENCIES
   Assumes camerai_init has been called.

RETURN VALUE
   Passes back the return value returned from the underlying client routine.

SIDE EFFECTS
   None.
===========================================================================*/
void camera_process_pnge_msg (uint32 msg)
{
  camerai_client [camerai_current_client].camera_process_pnge_msg (msg);
}
#endif /* FEATURE_PNG_ENCODER */

#ifdef FEATURE_CAMERA_V7
/*===========================================================================

FUNCTION      CAMERA_GET_FOCUS_WINDOW

DESCRIPTION
              Gets the focus window. Executes under caller's context

DEPENDENCIES

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_get_focus_window (camera_focus_window_type *camera_display_focus_window)
{
  return camerai_client [camerai_current_client].camera_get_focus_window (camera_display_focus_window);
} /* camera_get_focus_window */

/*===========================================================================

FUNCTION      CAMERA_SET_FOCUS_WINDOW

DESCRIPTION
              Sets the focus window. Executes under caller's context

DEPENDENCIES

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_set_focus_window (camera_focus_window_type *camera_display_focus_window)
{
  return camerai_client [camerai_current_client].camera_set_focus_window (camera_display_focus_window);
} /* camera_set_focus_window */


#endif /* FEATURE_CAMERA_V7 */


/*===========================================================================

FUNCTION      CAMERAI_INIT

DESCRIPTION
   Initialize function pointer table.  Set the default client.

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None.
===========================================================================*/
void camerai_init (void)
{
  camera_svcs_init_self (&camerai_client [CAMERAI_CAMERA]);

  /* For now, setup camera service client to be camera (the old way) */
  camerai_set_client (CAMERAI_CAMERA);

}

/*===========================================================================

FUNCTION      CAMERAI_SET_CLIENT

DESCRIPTION
   Indentify the client to be called when a camera services routine is called.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.
===========================================================================*/
void camerai_set_client (camerai_client_type client)
{
  camerai_current_client = client;
}

/*===========================================================================

FUNCTION camera_cm_call_event_cb

DESCRIPTION
   This is a call back function registered to CM.  This function is used to
   monitor if the phone is idle state or call state.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.
===========================================================================*/

static void camera_cm_call_event_cb
(
  cm_call_event_e_type          ph_event,
  const cm_mm_call_info_s_type *ph_info_ptr
  /*lint -esym(715,ph_info_ptr)*/
)
{
  uint16 i = 0;
  boolean still_in_voice_call = FALSE;

  if (ph_info_ptr == NULL)
  {
    MSG_FATAL("Null pointer: ph_info_ptr",0,0,0);
    return;
  }

  MSG_MED ("CM call event %d,call type %d", ph_event, ph_info_ptr->call_type,0);

  /* Only care about voice and emergency type, and ignore all others. The
   * same client may have multiple calls such as data call and voice call
   * at the same time. When data call is dropped, the voice call is still
   * going on. */
  if (ph_info_ptr->call_type != CM_CALL_TYPE_VOICE &&
      ph_info_ptr->call_type != CM_CALL_TYPE_EMERGENCY)
  {
    return;
  }

  switch( ph_event )
  {
    case CM_CALL_EVENT_END:
      camera_in_voice_call_flg[ph_info_ptr->call_id] = FALSE;

      /* Check to make sure that there are no other calls that would need
       * the vocoder before returning */
      for(i = 0; i < CM_CALL_ID_MAX; i++)
      {
        if (camera_in_voice_call_flg[i] == TRUE)
        {
          still_in_voice_call = TRUE;
          break;
        }
      }

      if (still_in_voice_call == FALSE)
      {
        camera_phone_state = NOT_IN_VOICE_CALL;
        MSG_MED ("Phone is not in call",0,0,0);
      }

      break;

    case CM_CALL_EVENT_CONNECT:
      camera_in_voice_call_flg[ph_info_ptr->call_id] = TRUE;
      camera_phone_state = IN_VOICE_CALL;
      MSG_MED ("Phone is in voice call",0,0,0);
      break;

    default:
      break;
  }
  return;
} /* camera_cm_call_event_cb() */


#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */

#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */

#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */

#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */

#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif /*FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/

#endif /* FEATURE_CAMERA */
