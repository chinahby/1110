#ifndef WMSMSGI_H
#define WMSMSGI_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


           W I R E L E S S   M E S S A G I N G   S E R V I C E S
           -- Message Handling Group  (private header)

GENERAL DESCRIPTION
  The WMS module which implements the User API for SMS. This source file
  defines the internal translation functions.

Copyright (c) 2007, 2008 by QUALCOMM INCORPORATED.
  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/24/08   PMD     Removed Tabs from code
06/04/08   PMD     Removed support for Auto / Transparent SMS 
02/27/08   PMD     Added Comment
02/26/08   PMD     Updated wmscm_gw changes for Enhanced Domain Selection
02/14/08   PMD     Added support for common type wms_cm_conn_id_type
01/31/08   PMD     Updated Copyright Information to include 2008
10/17/07   PMD     Guard wms_msg_mt_CDMA_tl_ack_proc with FEATURE_CDSMS
10/09/07   PY      Refactor the TL ack handler.
08/10/07   PY      Created from wmsmsg.h
===========================================================================*/



/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/
#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)
#include "wmsmsg.h"

#ifdef FEATURE_IWMSC
#error code not present
#endif /* FEATURE_IWMSC */


#ifdef FEATURE_LOW_MEMORY_USAGE
#define WMS_MAX_MO_MESSAGES     5
#define WMS_MAX_MT_MESSAGES     5
#else
#define WMS_MAX_MO_MESSAGES     20
#define WMS_MAX_MT_MESSAGES     20
#endif /* FEATURE_LOW_MEMORY_USAGE  */

#ifdef FEATURE_CDSMS
/* Message record
*/
typedef struct
{
  wms_client_type_e_type         client_id;
  wms_msg_state_e_type           state;

  uint8                           seq_num;

  boolean                        send_status_to_clients;

  void                           * user_data;
  void                           * mt_rec_user_data;

  wms_address_s_type             address;
  wms_subaddress_s_type          subaddress;

  wms_send_mode_e_type           send_mode;
  wms_client_msg_hdr_s_type      msg_hdr;

  /* Retry Time Left is the Time left for the Current SMS Delivery */
  sint31                         retry_time_left;

  /* Retry Wait Time is the Time to Wait before SMS retransmission */
  sint31                         retry_wait_time;

  /* Use prev_report if the retry period expires when processing the
  ** the submit report event.
  */
  wms_submit_report_info_s_type  prev_report;

  /* Fields for Pending MO SMS Message */
  wms_OTA_message_type          *ota;

  /* Fields for Pending MO SMS Ack*/
  wms_error_class_e_type         error_class;
  wms_cdma_tl_status_e_type      tl_status;

  /* each message has its own timer to wake up the SMS task
  */
  rex_timer_type                 timer;
 //rex_timer_type                 timer;
#ifdef CUST_EDITION	 
  clk_cb_type                    clk_timer;
#endif /*CUST_EDITION*/
  boolean                        large_msg;

#ifdef FEATURE_IWMSC
#error code not present
#endif /* FEATURE_IWMSC */

} wms_msg_cdma_message_record_type;

#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */


typedef struct
{

#ifdef FEATURE_CDSMS
  /* Information of incoming and outgoing CDMA mode SMS messages that need
     transport layer ack.
  */
  wms_msg_cdma_message_record_type     cdma_mt_rec[ WMS_MAX_MT_MESSAGES ];
  wms_msg_cdma_message_record_type     cdma_mo_rec[ WMS_MAX_MO_MESSAGES ];

  uint8                            tl_seq_num;
  uint8                            non_tl_seq_num;
  wms_transaction_id_type          tid;

  struct
  {
    boolean                        is_valid;
    boolean                        is_ack;    /* whether the msg is a TL Ack */
    wms_msg_cdma_message_record_type   * rec_ptr; /* the msg record ptr */
    wms_transaction_id_type        tid;
    wms_error_class_e_type         error_class;
    wms_cdma_tl_status_e_type      tl_status;
    wms_bearer_e_type              bearer;
  } mc_msg;

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

  uint32                                mo_retry_period;
  uint32                                mo_retry_interval;

} wms_msg_s_type;

wms_msg_s_type* wms_msg_s_ptr
(
  void
);

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
void wms_msg_mt_CDMA_tl_ack_proc(
   wms_msg_cdma_message_record_type * rec_ptr,
   wms_cause_code_type              * cause_code_ptr
);
#endif /* FEATURE_CDSMS */

#endif /* FEATURE_CDSMS || FEATURE_GWSMS */
#endif /* WMSMSGI_H */


