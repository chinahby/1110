/*****************************************************************************
**
**  Name:           bta_aa_co.h
**
**  Description:    This is the interface file for advanced audio call-out
**                  functions.
**
**  Copyright (c) 2003, Widcomm Inc., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_AA_CO_H
#define BTA_AA_CO_H

#include "bta_aa_api.h"

/*****************************************************************************
**  Constants and data types
*****************************************************************************/

/* TRUE to use SCMS-T content protection  */
#ifndef BTA_AA_CO_CP_SCMS_T
#define BTA_AA_CO_CP_SCMS_T     FALSE
#endif

/* the content protection IDs assigned by BT SIG */
#define BTA_AA_CP_SCMS_T_ID     0x0002
#define BTA_AA_CP_DTCP_ID       0x0001

#define BTA_AA_CP_LOSC                  2
#define BTA_AA_CP_INFO_LEN              3

#define BTA_AA_CP_SCMS_COPY_MASK        3
#define BTA_AA_CP_SCMS_COPY_FREE        2
#define BTA_AA_CP_SCMS_COPY_ONCE        1
#define BTA_AA_CP_SCMS_COPY_NEVER       0

/*******************************************************************************
**
** Function         bta_aa_co_init
**
** Description      This callout function is executed by AA when it is
**                  started by calling BTA_AaEnable().  This function can be
**                  used by the phone to initialize audio paths or for other
**                  initialization purposes.  
**                  
**
** Returns          void.
**
*******************************************************************************/
BTA_API extern BOOLEAN bta_aa_co_init(UINT8 *p_codec_type, UINT8 *p_codec_info,
                                   UINT8 *p_num_protect, UINT8 *p_protect_info, UINT8 index);

/*******************************************************************************
**
** Function         bta_aa_co_disc_res
**
** Description      This callout function is executed by AA to report the
**                  number of stream end points (SEP) were found during the
**                  AVDT stream discovery process.
**                  
**
** Returns          void.
**
*******************************************************************************/
BTA_API extern void bta_aa_co_disc_res(UINT8 num_seps, UINT8 num_snk, BD_ADDR addr);

/*******************************************************************************
**
** Function         bta_aa_co_getconfig
**
** Description      This callout function is executed by AA to retrieve the
**                  desired codec and content protection configuration for the
**                  stream.
**                  
**
** Returns          Stream codec and content protection configuration info.
**
*******************************************************************************/
BTA_API extern UINT8 bta_aa_co_getconfig(tBTA_AA_CODEC codec_type, UINT8 *p_codec_info,
                                         UINT8 *p_sep_info_idx, UINT8 seid,
                                         UINT8 *p_num_protect, UINT8 *p_protect_info);

/*******************************************************************************
**
** Function         bta_aa_co_setconfig
**
** Description      This callout function is executed by AA to set the
**                  codec and content protection configuration of the stream.  
**                  
**
** Returns          void  
**
*******************************************************************************/
BTA_API extern void bta_aa_co_setconfig(tBTA_AA_CODEC codec_type, UINT8 *p_codec_info,
                                        UINT8 seid, BD_ADDR addr,
                                        UINT8 num_protect, UINT8 *p_protect_info);

/*******************************************************************************
**
** Function         bta_aa_co_open
**
** Description      This function is called by AA when the stream connection
**                  is opened.
**                  
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void bta_aa_co_open(tBTA_AA_CODEC codec_type, UINT8 *p_codec_info, 
                                   UINT16 mtu);

/*******************************************************************************
**
** Function         bta_aa_co_close
**
** Description      This function is called by AA when the stream connection
**                  is closed.
**                  
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void bta_aa_co_close(tBTA_AA_CODEC codec_type);

/*******************************************************************************
**
** Function         bta_aa_co_start
**
** Description      This function is called by AA when the streaming data
**                  transfer is started.
**                  
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void bta_aa_co_start(tBTA_AA_CODEC codec_type);

/*******************************************************************************
**
** Function         bta_aa_co_stop
**
** Description      This function is called by AA when the streaming data
**                  transfer is stopped.
**                  
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void bta_aa_co_stop(tBTA_AA_CODEC codec_type);

/*******************************************************************************
**
** Function         bta_aa_co_src_data_path
**
** Description      This function is called to manage data transfer from
**                  the codec to AVDTP.                 
**
** Returns          TRUE if AVDT_WriteReq called, false otherwise.
**
*******************************************************************************/
BTA_API extern BOOLEAN bta_aa_co_src_data_path(tBTA_AA_CODEC codec_type, UINT8 avdt_handle, UINT16 mtu);

#endif /* BTA_AA_CO_H */

