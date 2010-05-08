/*========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


/*===========================================================================

                      CameraDevice DECLARATIONS

===========================================================================*/

camera_ret_code_type camera_start_preview(camera_cb_f_type callback,void * client_data);
camera_ret_code_type camera_take_picture(  uint16 quality,  boolean multi,  camera_cb_f_type callback,  void * client_data);
void frame_record(char *movname,uint32 quality);
void frame_record_begin(char *movname);
camera_ret_code_type camera_stop_preview(void);
int camera_set_dimensions(OEMCamera * Ppme);
int camera_disable_frame_callback( void *dst);
int camera_release_picture(void);
int camera_stop_record(void);
int camera_enable_frame_callback(void);


