#include "rex.h"                /* definition of REX data types           */
#include "task.h"               /* Task definitions and prototypes        */
#include "dog.h"                /* Watchdog signal and time definitions   */
#include "mc.h"                 /* Need for MC signals and prototypes     */
#include "fs.h"                  /*Required for file system related functions*/
#include "AEEstd.h"           /* Definitions of standard functions*/ 

#include "acmtask.h"
//#include "acmtaski.h"
#include "acm.h"
#include "acmapi.h"
/* <EJECT> */
#ifdef FEATURE_AVS_CALDB_FILE_LOADER

#define ACMTASK_MAX_FILE_SIZE 50
#define ACMTASK_MAX_MESSAGE_SIZE 80

/* Watchdog Timers */
rex_timer_type acmtask_rpt_timer;    /* Watch Dog Timer*/
fs_handle_type acmtask_log_file_handle;
fs_handle_type acmtask_caldb_file_handle;

fs_open_xparms_type open_parms;
fs_rsp_msg_type acm_fs_rsp;
const char sfs_caldb_file_name[ACMTASK_MAX_FILE_SIZE] = {"/audio/audiocal/audiocaldb.cdb"};
const char caldb_file_name[ACMTASK_MAX_FILE_SIZE] = {"audio/audiocal/audiocaldb.cdb"};
const char log_file_name[ACMTASK_MAX_FILE_SIZE] = {"audio/audiocal/audiocaldb.log"};
const char caldb_file_dir[ACMTASK_MAX_FILE_SIZE] = {"audio/audiocal"};
const char log_message[ACMTSK_MSG_MAX][ACMTASK_MAX_MESSAGE_SIZE] =
      {
        {"\nNo calibration file exists."},
        {"\nIncorrect command ID found in calibration file."},
        {"\nCalibration file load complete."},
        {"\nCould not set Calunit instance."},
        {"\nCalunit instance set successfully."},
        {"\nIncorrect calibration file format."},
        {"\nTrying to Set Calunit instance..."},
        {"\nIndecies :Index1: 0x%X  Index2: 0x%X  Index3: 0x%X."},
        {"\nIndecies :Index1: 0x%X  Index2: 0x%X."},
        {"\nReturn code : 0x%X."},
        {"\nCalibration file doesn't matches the build."}
      };
/*===========================================================================

FUNCTION acmtask_init

DESCRIPTION
  This function performs ACM task initialization process.

DEPENDENCIES
  acmtask_rpt_timer

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void acmtask_init( void ) 
{
  /* Initialize timers */
  rex_def_timer( &acmtask_rpt_timer, &acm_tcb, ACM_RPT_TIMER_SIG );
  
  /* Set the load calibration data from file signal so that we'll
  ** load the calibration values once task gets started.
  */
  (void) rex_set_sigs( &acm_tcb, ACM_LOAD_CALFILE_SIG );
} /* acmtask_init */


/*===========================================================================

FUNCTION acm_task

DESCRIPTION
  This procedure is the entrance procedure for the audio protocol over diag Task.  It
  contains the main processing loop for the ATP RX Task which controls
  queue processing.

DEPENDENCIES
  None

RETURN VALUE
  Does not return

SIDE EFFECTS
  atprx_tcb

===========================================================================*/

void acm_task( dword dummy ) 
{
  int fd = NULL ;
  int bytes_written = 0 ;
  rex_sigs_type rex_signals_mask; /* Mask of signals returned by rex */
  acm_cmd_struct_header acm_req_cmd_header;
  acm_cmd_result_struct acm_cmd_result ;
  acm_calunit_enum calunit_name;
  acm_guid calunit_guid;
  acm_guid calfile_guid;
  acm_error_code error_code = ACM_ERR_UNKNOWN ;
  voc_filter_indices_struct voc_indices;
  audio_filter_indices_struct audio_indices;
  byte *acm_cmd_buf_ptr = NULL ;
  byte *resp_buf_ptr = NULL ;
  byte *tmp_resp_buf_ptr = NULL ;
  uint32 resp_buf_length = 0 ;
  uint32 cmd_buf_length = 0 ;
  char indicies_log_msg[ACMTASK_MAX_MESSAGE_SIZE];

  acmtask_init(); /* Initialize timers */

  /* Wait for start signal from Main Control task   */
#ifdef FEATURE_MULTIMODE_ARCH
  task_start
  (
    ACM_RPT_TIMER_SIG, /* report timer signal for task */
    DOG_ACM_RPT, /* watchdog report */
    &acmtask_rpt_timer /* pointer to report timer */
  );
#else
  mctask_start
  (
    ACM_RPT_TIMER_SIG, /* report timer signal for task */
    DOG_ACM_RPT, /* watchdog report */
    &acmtask_rpt_timer /* pointer to report timer */
  );
#endif /*FEATURE_MULTIMODE_ARCH*/

  /* Clear start signal
  */
  (void) rex_clr_sigs( &acm_tcb, TASK_START_SIG );

  /* Set the watchdog report timer signal so that we'll
  ** reset the watchdog as soon as we start checking for events
  */
  (void) rex_set_sigs( &acm_tcb, ACM_RPT_TIMER_SIG );

  /* wait in a infinite loop for commands / signals from other tasks */
  for (;;)
  {
    MSG_MED("ACM TASK WAITING ON SIGS",0,0,0);
    rex_signals_mask =
                rex_wait( ACM_RPT_TIMER_SIG | TASK_STOP_SIG | 
                                TASK_OFFLINE_SIG | ACM_LOAD_CALFILE_SIG | 
                                ACM_CAN_READ_CALFILE_SIG
                                );

    if ((rex_signals_mask & ACM_RPT_TIMER_SIG) != 0)
    {
      dog_report(DOG_ACM_RPT); /* handle timer expiry */
      (void) rex_set_timer( &acmtask_rpt_timer, DOG_ACM_RPT_TIME );
    }
    /* Handle STOP signal. All tasks must handle this signal as shown below*/
    if ((rex_signals_mask & TASK_STOP_SIG) != 0)
    {
      (void) rex_clr_sigs( &acm_tcb, TASK_STOP_SIG );
      (void) rex_set_sigs( &mc_tcb, MC_ACK_SIG );
    }
    /* Handle OFFLINE signal. All tasks must handle this signal shown below */
    if ((rex_signals_mask & TASK_OFFLINE_SIG) != 0)
    {
      (void) rex_clr_sigs( &acm_tcb, TASK_OFFLINE_SIG );
      (void) rex_set_sigs( &mc_tcb, MC_ACK_SIG );
    }
    /*Handle Load Calibration File signal*/
    if ( (rex_signals_mask & ACM_LOAD_CALFILE_SIG) != 0) 
    {
      (void) rex_clr_sigs( &acm_tcb, ACM_LOAD_CALFILE_SIG );
      /*Check if we have the required directory structure*/
      fs_nametest(caldb_file_dir,FS_TEST_DIR,NULL,&acm_fs_rsp);
      if (acm_fs_rsp.any.status == FS_OKAY_S &&
            acm_fs_rsp.nametest.name_found == TRUE )
      {
        MSG_HIGH("acm_task:dir- %s exists", caldb_file_dir, 0, 0);
        
        /*check if a log file already exists*/
        fs_nametest(log_file_name,FS_TEST_FILE,NULL,&acm_fs_rsp);
        if ( acm_fs_rsp.any.status == FS_OKAY_S )
        {
          if( acm_fs_rsp.nametest.name_found == TRUE )
          {
            /*If exists just open it*/
            fs_open(log_file_name, FS_OA_TRUNCATE, &open_parms, NULL, &acm_fs_rsp);
          }
          else
          {
            /*If doesn't exists create one*/
            MSG_HIGH("acm_task:log file - %s ~Not~ exists", log_file_name, 0, 0);
            open_parms.create.cleanup_option   = FS_OC_CLOSE;
            open_parms.create.buffering_option = FS_OB_ALLOW;
            open_parms.create.attribute_mask   = FS_FA_UNRESTRICTED;
            fs_open(log_file_name, FS_OA_CREATE, &open_parms, NULL, &acm_fs_rsp);
          }
          //save log file handle
          if ( acm_fs_rsp.open.status == FS_OKAY_S && acm_fs_rsp.open.handle != NULL )
          {
            //fs_open(log_file_name, FS_OA_TRUNCATE, &open_parms, NULL, &acm_fs_rsp);
            if ( acm_fs_rsp.open.status == FS_OKAY_S )
            {
              acmtask_log_file_handle = acm_fs_rsp.open.handle;
            }
          }
        }
        //check if we have calibration file
        fs_nametest(caldb_file_name,FS_TEST_FILE,NULL,&acm_fs_rsp);
        if (acm_fs_rsp.any.status == FS_OKAY_S && 
            acm_fs_rsp.nametest.name_found == TRUE )
        {
          /*calibration file is available. So open it*/
          MSG_HIGH("acm_task: %s exists", caldb_file_name, 0, 0);
          fs_open(caldb_file_name, FS_OA_READONLY, NULL, NULL, &acm_fs_rsp);
          if (acm_fs_rsp.open.status == FS_OKAY_S && acm_fs_rsp.open.handle != NULL)
          {
            /*Get handle*/
            acmtask_caldb_file_handle = acm_fs_rsp.open.handle ;
            /*Set signal that we are ready to read calibration file and load the values*/
            rex_set_sigs(&acm_tcb, ACM_CAN_READ_CALFILE_SIG);
          }
          else
          {
            /*No calibration file exists. So just write that to log file*/
            if ( acmtask_log_file_handle != NULL )
            {
              fs_write(acmtask_log_file_handle, log_message[ACMTSK_MSG_NO_CALFILE], 
                            ACMTASK_MAX_MESSAGE_SIZE, 
                            NULL, &acm_fs_rsp);
            }
            fs_close(acmtask_log_file_handle, NULL, &acm_fs_rsp);
          }
        }
        else
        {
          MSG_HIGH("acm_task: fs_nametest operation failed", 0, 0, 0);
        }
      }
      else
      {
        MSG_HIGH("acm_task: DIR ~NOT~ exists", 0, 0, 0);
      }
    }
    /* ******* Handle read calibration file signal ******** */
    if ( ( rex_signals_mask & ACM_CAN_READ_CALFILE_SIG ) != 0 ) 
    {
      /*clear signal*/
      rex_clr_sigs(&acm_tcb, ACM_CAN_READ_CALFILE_SIG );
//      sfs_rm(sfs_caldb_file_name);
      fd = sfs_open(sfs_caldb_file_name,ACM_SFS_CREAT | ACM_SFS_WRONLY);
      if( acmtask_caldb_file_handle != NULL && acmtask_log_file_handle != NULL)
      {
      
        fs_read(acmtask_caldb_file_handle,(void *)&calfile_guid,
                      sizeof(calfile_guid), NULL, &acm_fs_rsp
                    );
        error_code = acmapi_check_build_guid(&calfile_guid);
        if ( error_code == ACM_ERR_CALFILE_MISMATCH )
        {
          fs_write(acmtask_log_file_handle,
                                log_message[ACMTSK_MSG_CALFILE_MISMATCH], 
                                ACMTASK_MAX_MESSAGE_SIZE, NULL, &acm_fs_rsp
                       ) ;
        }
        else
        {
          bytes_written = sfs_write(fd, (const char *)&calfile_guid,sizeof(calfile_guid));
          MSG_MED("acm_task: No. of bytes written to SFS file: %d", bytes_written,0,0);

          do
          {
            (void) rex_set_timer( &acmtask_rpt_timer, DOG_ACM_RPT_TIME );
            dog_report(DOG_ACM_RPT); /* handle timer expiry */
            /*read command buffer header (6 ytes)*/
            fs_read( acmtask_caldb_file_handle, &acm_req_cmd_header,
                            sizeof(acm_req_cmd_header), NULL, &acm_fs_rsp
                      );
            if ( acm_fs_rsp.read.status == FS_EOF_S || acm_fs_rsp.read.count == 0)
            {
              fs_write(acmtask_log_file_handle,
                                  log_message[ACMTSK_MSG_CALFILE_COMPLETE], 
                                  ACMTASK_MAX_MESSAGE_SIZE, NULL, &acm_fs_rsp
                             ) ;
              break ;
            }
            else if ( acm_fs_rsp.read.count > 0 )
            {
              if ( acm_req_cmd_header.cmd_id == CMD_SET_STRUCT )
              {
                cmd_buf_length = 
                            acm_req_cmd_header.cmd_buf_length + sizeof(acm_req_cmd_header) ;
                acm_cmd_buf_ptr = malloc(cmd_buf_length) ;
                tmp_resp_buf_ptr = acm_cmd_buf_ptr;
                memcpy ( acm_cmd_buf_ptr, (void *) &acm_req_cmd_header,
                                  sizeof(acm_req_cmd_header)
                               );
                tmp_resp_buf_ptr += sizeof(acm_req_cmd_header) ;
                fs_read( acmtask_caldb_file_handle, tmp_resp_buf_ptr ,
                            acm_req_cmd_header.cmd_buf_length, NULL, &acm_fs_rsp
                      );
                if ( acm_fs_rsp.read.count == acm_req_cmd_header.cmd_buf_length)
                {
                  fs_write(acmtask_log_file_handle,
                                  log_message[ACMTSK_MSG_STARTED_SETOPR_STR], 
                                  ACMTASK_MAX_MESSAGE_SIZE, NULL,&acm_fs_rsp
                                ) ;
                  /*Get Calunit GUID*/
                  memcpy((void *)&calunit_guid, tmp_resp_buf_ptr,sizeof(acm_guid));
                  tmp_resp_buf_ptr += sizeof(acm_guid) ;
                  calunit_name = acmapi_get_calunit_from_guid(&calunit_guid);
                  fs_write(acmtask_log_file_handle, acm_calunit_name_list[calunit_name], 
                                  CALUNIT_NAME_MAX_SIZE, 
                                  NULL,&acm_fs_rsp
                              ) ;
                  if ( calunit_name == CALUNIT_VOLUME_LEVELS )
                  {
                    memcpy( (void *)&audio_indices, tmp_resp_buf_ptr, sizeof(audio_indices));
                    tmp_resp_buf_ptr += sizeof(audio_indices);
                    std_strlprintf(indicies_log_msg,ACMTASK_MAX_MESSAGE_SIZE,
                                            log_message[ACMTSK_MSG_AUDIO_INDICES_STR],
                                            audio_indices.snd_device, audio_indices.snd_method
                                          );
                  }
                  else
                  {
                    memcpy( (void *)&voc_indices, tmp_resp_buf_ptr, sizeof(voc_indices));
                    tmp_resp_buf_ptr += sizeof(voc_indices);
                    std_strlprintf(indicies_log_msg, ACMTASK_MAX_MESSAGE_SIZE,
                                log_message[ACMTSK_MSG_VOC_INDICES_STR],
                                voc_indices.voc_cal_pcm_path_cfg,voc_indices.snd_device,
                                voc_indices.snd_method
                                );
                  }
                  fs_write(acmtask_log_file_handle, indicies_log_msg,
                                  ACMTASK_MAX_MESSAGE_SIZE, NULL,&acm_fs_rsp
                                ) ;
                  bytes_written = sfs_write(fd, (const char *)acm_cmd_buf_ptr,cmd_buf_length);
                  MSG_MED("acm_task: No. of bytes written to SFS file: %d", bytes_written,0,0);
                  acm_receive_req_buffer ( acm_cmd_buf_ptr ,
                                          cmd_buf_length, &resp_buf_ptr ,
                                          &resp_buf_length
                                          );
                  if ( resp_buf_ptr != NULL )
                  {
                    memcpy((void *)&acm_cmd_result,resp_buf_ptr,sizeof(acm_cmd_result));
                    if ( acm_cmd_result.result_flag == TRUE )
                    {
                      fs_write(acmtask_log_file_handle, 
                                      log_message[ACMTSK_MSG_CALUNIT_SET_SUCCESS], 
                                      ACMTASK_MAX_MESSAGE_SIZE, NULL, &acm_fs_rsp
                                    ) ;
                    }
                    else
                    {
                      fs_write(acmtask_log_file_handle, 
                                      log_message[ACMTSK_MSG_CALUNIT_SET_FAILED], 
                                      ACMTASK_MAX_MESSAGE_SIZE, NULL,&acm_fs_rsp
                                    ) ;
                      std_strlprintf(indicies_log_msg, ACMTASK_MAX_MESSAGE_SIZE,
                                  log_message[ACMTSK_MSG_RETURN_CODE_STR],
                                  acm_cmd_result.error_code
                                  );
                      fs_write(acmtask_log_file_handle, indicies_log_msg, 
                                      ACMTASK_MAX_MESSAGE_SIZE, NULL, &acm_fs_rsp
                                    ) ;
                    }
                  }
  
                  if ( acm_cmd_buf_ptr != NULL )
                  {
                    free(acm_cmd_buf_ptr);
                    acm_cmd_buf_ptr = NULL;
                  }
  
                  if ( resp_buf_ptr != NULL )
                  {
                    free(resp_buf_ptr);
                    resp_buf_ptr = NULL ;
                  }
                }
                else
                {
                  fs_write(acmtask_log_file_handle,
                             log_message[ACMTSK_MSG_INVALID_FILE_FORMAT], 
                             ACMTASK_MAX_MESSAGE_SIZE, NULL, &acm_fs_rsp
                          ) ;
                  break;
                }
              }
              else
              {
                /*If found incorrect command id, log it*/
                fs_write(acmtask_log_file_handle,
                                log_message[ACMTSK_MSG_INCORRECT_CMD], 
                                ACMTASK_MAX_MESSAGE_SIZE, NULL, &acm_fs_rsp
                              ) ;
                /*Then skip the command*/
                fs_seek( acmtask_caldb_file_handle, FS_SEEK_CURRENT, 
                                acm_req_cmd_header.cmd_buf_length, NULL, &acm_fs_rsp) ;
              }
            }
          }
          while(TRUE);
        }
        fs_close(acmtask_log_file_handle, NULL, &acm_fs_rsp);
        fs_close(acmtask_caldb_file_handle, NULL, &acm_fs_rsp);
      }
      sfs_close(fd);
    }
  } /* end for (;;) */
  MSG_MED("ACM TASK ENDED",0,0,0);
} /* end of atprx_task */
#endif /*FEATURE_AVS_CALDB_FILE_LOADER*/
