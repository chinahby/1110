/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                         D S 7 0 7 _ S 0 3 3 

GENERAL DESCRIPTION
  This file contains functions specific to service option 33.  It does NOT
  contain RLP 3 services.  

EXTERNALIZED FUNCTIONS
  DS707_SO33_PKT_INITIALIZER
    Called when an SO 33 call connects.  Initializes RLP 3 and initializes
    the RRM state (which includes SCRM & Control/Hold).
    
  DS707_SO33_REG_INITIALIZERS
    Called at startup.  Registers initializer with SNM for when SO 33 
    call comes up.  Also registers RLP 3 BLOB generator asw well as QOS
    functionality.
    
  DS707_SO33_ORIG_QOS
    Asks RLP to create a QOS blob for CM origination parameters.
    
  DS707_SO33_PROCESS_QOS
    Called when QOS info received from BS.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Must call ds707_so33_reg_initializers at startup.

   Copyright (c) 2002-2008 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_so33.c_v   1.5   18 Dec 2002 17:30:26   ajithp  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_so33.c#2 $ $DateTime: 2008/06/24 01:30:38 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/18/08   sn      Fixed compiler warnings.
04/27/05   gr      Featurized handling of SO_OMIT under feature IS2000_SDB
04/05/05   gr      Added interface to handle SO_OMIT specified in SCM/UHDM
12/18/02   atp     Minor print message changes.
11/17/02   ak      Updated file header comments.
09/24/02   atp     Added support for QOS. (1x Release-A feature).

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "cai.h"
#include "caii.h"
#include "cm.h"
#include "ds707_rrm.h"
#ifdef FEATURE_DATA_IS2000_SDB
#include "ds707_sdb.h"
#endif /* FEATURE_DATA_IS2000_SDB */
#include "dsrlp.h"
#include "err.h"
#include "msg.h"
#include "rxc.h"
#include "snm.h"


#ifdef FEATURE_IS2000_REL_A_MUXPDU5  
  #include "dsrlp3.h"
  extern dsrlpi_ver_const_type dsrlp3_constants;
#endif /*FEATURE_IS2000_REL_A_MUXPDU5*/

/*===========================================================================
                         EXTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_SO33_PKT_INITIALIZER

DESCRIPTION   Called when a so 33 packet data call is connected.  Goes ahead
              and sets up the associated RLP service.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so33_pkt_initializer
(
  const caii_srv_cfg_type*  cfg_info,
  word                      so_index
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (cfg_info->con[so_index].so == CAI_SO_PPP_PKT_DATA_3G)
  {
    ds707_rrm_call_init();
    ds707_rrm_reg_funcs();

    dsrlp_init((byte)DSRLP_FIXED_SR_ID_INDEX,
               (byte)cfg_info->con[so_index].sr_id, 
                DSRLP_REV_3, 
                FALSE
              );
    
#ifdef FEATURE_IS2000_REL_A_MUXPDU5               
	/*-------------------------------------------------------------------------
	 In case of Mux PDU5 turbo since we have multiple dsm items chained 
	 together in one SCH PDU. We have to recalculate the size of the 
	 transmit queue (For convolutional we have 8 SCH PDU's smaller in size)
    -------------------------------------------------------------------------*/
	if((cfg_info->rev_sch->sch_chn_cfg.coding ==1)&&
	   (cfg_info->rev_sch->rev_sch_mux == CAI_MUX_VARIABLE) )
	{
		dsrlp3_constants.txq_size = DSRLP3_F20_TURBO_TXQ_SIZE;
	}
	else
	{
		dsrlp3_constants.txq_size = DSRLP3_TXQ_SIZE;
	}
#endif /*FEATURE_IS2000_REL_A_MUXPDU5*/
    /*-----------------------------------------------------------------------
      Now that we have initialized the rlp, go ahead and process the BLOB
    -----------------------------------------------------------------------*/
    if (cfg_info->con[so_index].rlp_info_incl) 
    {
        /*-------------------------------------------------------------------
          this assumes that the session control block pointed  by the 
          rscb_ptr that we gonna get by using this sr_id has been 
          initialized and pointing to valid info. dsrlp_init() does that.
        -------------------------------------------------------------------*/
        if (dsrlp_process_rlp_blob( 
                                  (byte)cfg_info->con[so_index].sr_id,
                                  (byte *)cfg_info->con[so_index].rlp_blob,
                                  (byte)cfg_info->con[so_index].rlp_blob_len
                                  ) == FALSE)
        {
          ERR("RLP BLOB DID NOT PARSE CORRECTLY!!!",0,0,0);
        }
    }
    
    rxc_init_sch_mux();
  }
  else
  {
    ERR_FATAL("Only SO 33 uses callback right now",0,0,0);
  }
}

/*===========================================================================
FUNCTION      DS707_SO33_REG_INITIALIZERS

DESCRIPTION   Register initializer functions used by snm when a Service
              Option 33 is connected.

DEPENDENCIES  This function should only be called after all SO's have been
              added to the SNM.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so33_reg_initializers(void)
{
  snm_status_type     status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  status = snm_reg_connection_initializer(CAI_SO_PPP_PKT_DATA_3G, 
                                          ds707_so33_pkt_initializer);
  if (status == SNM_FAILURE)
  {
    ERR_FATAL("Could not register initializer",0,0,0);
  }

  status = snm_reg_rlp_blob_generator(CAI_SO_PPP_PKT_DATA_3G,
                                      dsrlp_build_rlp_blob);
  if (status == SNM_FAILURE)
  {
    ERR_FATAL("Could not register initializer",0,0,0);
  }

#ifdef FEATURE_IS2000_REL_A
  status =   snm_reg_qos_functions( CAI_SO_PPP_PKT_DATA_3G,
                                    dsrlp_build_qos_blob,
                                    dsrlp_eval_qos_blob);

  if (status == SNM_FAILURE)
  {
    ERR_FATAL("Could not register initializer",0,0,0);
  }

  status =  snm_reg_sdb_so_omit_status_handler( CAI_SO_PPP_PKT_DATA_3G,
                                            ds707_sdb_so_omit_handler);

  if (status == SNM_FAILURE)
  {
    ERR_FATAL("Could not register initializer",0,0,0);
  }
#endif /* FEATURE_IS2000_REL_A */
}


#ifdef FEATURE_IS2000_REL_A
/*===========================================================================
FUNCTION      DS707_SO33_ORIG_QOS

DESCRIPTION   Asks RLP to create QOS blob and  puts the blob in CM
              origination parameter.

DEPENDENCIES  NONE

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so33_orig_qos
(
  cm_cdma_orig_params_s_type    *orig_params
)
{
  /*-------------------------------------------------------------------------
    Build QOS blob.  Note that we donot build blob always
  -------------------------------------------------------------------------*/
  orig_params->qos_parms_len = dsrlp_build_qos_blob (orig_params->qos_parms);

  /*-------------------------------------------------------------------------
    dsrlp_build_qos_blob returns length of QOS blob to be sent after
    building the blob. If length == 0, it means default values of QOS
    are enough, and no blob needs to be sent to BS.
  -------------------------------------------------------------------------*/
  if ( orig_params->qos_parms_len > 0 )
  {
    orig_params->qos_parms_incl = TRUE;
  }

  MSG_MED ("DesirQOS:incl=%d,mode=%d,priadj=%x",
           dsrlp_qos_values.desired.qos_incl,
           dsrlp_qos_values.desired.assur_mode,
           dsrlp_qos_values.desired.params.non_assur_params.pri_adj
          );
} /* ds707_so33_orig_qos() */


/*===========================================================================
FUNCTION      DS707_SO33_PROCESS_QOS

DESCRIPTION   Asks RLP to process QOS blob received from BS.

DEPENDENCIES  NONE

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so33_process_qos
(
  cm_call_mode_info_s_type  *mode_info_ptr
)
{
  /*-------------------------------------------------------------------------
    Check if qos blob (from BS) present in connect event. If so, process
    blob, otherwise note qos values not received from BS.
    The check for qos_parms_len > 0 is just for safety. If 1st condition
    is true, the second should be true normally.
  -------------------------------------------------------------------------*/
  if (
          (  mode_info_ptr->info.cdma_call.qos_parms_incl     )
       && (( mode_info_ptr->info.cdma_call.qos_parms_len ) >0 )
     )
  {
    dsrlp_qos_values.rx_from_bs.qos_incl = TRUE;

    dsrlp_process_qos_blob (
                             mode_info_ptr->info.cdma_call.qos_parms,
                             mode_info_ptr->info.cdma_call.qos_parms_len
                           );
  }
  else
  {
    /*-----------------------------------------------------------------------
      Didn't receive QOS blob from BS
    -----------------------------------------------------------------------*/
    dsrlp_qos_values.rx_from_bs.qos_incl = FALSE;
  }

  /*-------------------------------------------------------------------------
    Determine current QOS values (default or rx from BS)
  -------------------------------------------------------------------------*/
  dsrlp_calc_curr_qos_values();
  MSG_MED (
           "CurrQOS:incl=%d,mode=%d,priadj=%x",
           (dsrlp_qos_values.curr_p)->qos_incl,
           (dsrlp_qos_values.curr_p)->assur_mode,
           (dsrlp_qos_values.curr_p)->params.non_assur_params.pri_adj
          );

} /* ds707_so33_process_qos() */
#endif /* FEATURE_IS2000_REL_A */

#endif /* FEATURE_DATA_IS707 */

