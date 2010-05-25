/****************************************************************************
** Name:         bcm2045_prm_api.h
** 
** Description:  Broadcom 2045 Patch RAM API
**
** Copyright (c) 1999-2007, BROADCOM Inc., All Rights Reserved.
** Broadcom Bluetooth Core. Proprietary and confidential.
**
******************************************************************************/
#ifndef BCM2045_PRM_API_H
#define BCM2045_PRM_API_H

/* max patch data length (Can be overridden by platform for ACL HCI command size) */
#ifndef MAX_PRM_LEN
#define MAX_PRM_LEN 250
#endif

#include "bt_target.h"
#include "bt_types.h"
/*****************************************************************************
**  BCM2045 Patch RAM MANAGEMENT
*****************************************************************************/
/****************************
**  Patch RAM Constants
*****************************/
/* BCM2045 Patch RAM status codes */
enum
{
    BCM2045_PRM_STS_CONTINUE = 0,
    BCM2045_PRM_STS_COMPLETE,
    BCM2045_PRM_STS_ABORT
};
typedef UINT8 tBCM2045_PRM_STATUS;

#ifndef BCM2045_USE_DELAY
#define BCM2045_USE_DELAY TRUE
#endif

#if (BCM2045_USE_DELAY == FALSE)
extern void bcm2045_timer_cback (void *p_tle);
#endif

/*************************************
**  BCM2045 Patch RAM Callback Functions
**************************************/
typedef void (tBCM2045_PRM_CBACK) (tBCM2045_PRM_STATUS status);

enum
{
    BCM2045_PRM_ST_IDLE  = 0,
    BCM2045_PRM_ST_LOADING_MINIDRV,
    BCM2045_PRM_ST_LOADING_MINIDRV_DONE,
    BCM2045_PRM_ST_LOADING_DATA,
    BCM2045_PRM_ST_LOADING_DATA_DONE,
    BCM2045_PRM_ST_LAUNCHING_RAM
};
typedef UINT8 tBCM2045_PRM_STATE;

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
**  BCM2045 Patch RAM MANAGEMENT FUNCTIONS
*****************************************************************************/
/*******************************************************************************
**
** Function         BCM2045_PrmInit
**
** Description      Register patch ram callback, start to send Download_Mini_Driver
**                  to controller to start the download process, if use internal patch, 
**                  it will start the download data process from the internal patch, else
**                  application need to provide the patch data 
** Input Param      p_cb - callback for download status
**                  use_internal_patch_data - if true patch data is built into code, 
**                                          - else application need to provide patch data
**                                          - by calling BCM2045_PRM_Load_Data                 
**
** Returns          TRUE if successful, otherwise FALSE
**                  
**
*******************************************************************************/
BT_API extern BOOLEAN BCM2045_PrmInit (tBCM2045_PRM_CBACK *p_cb,
                                       BOOLEAN use_internal_patch_data);

/*******************************************************************************
**
** Function         BCM2045_PrmLoadData
**
** Description      Download data to controller if application need to provide patch data.
**                  
** Input Param      p_patch_data            - pointer to patch data
**                  patch_data_len          - patch data len
**                  
** Returns          TRUE if successful, otherwise FALSE
**
*******************************************************************************/
BT_API extern BOOLEAN BCM2045_PrmLoadData (UINT8 *p_patch_data,
                               UINT16 patch_data_len);

/*******************************************************************************
**
** Function         BCM2045_PrmLaunchRam
**
** Description      After all patches are download, lauch ram
** Input Param      
**
**
** Returns          TRUE if successful, otherwise FALSE
**
*******************************************************************************/
BT_API extern BOOLEAN BCM2045_PrmLaunchRam (void);

/*******************************************************************************
**
** Function         BCM2045_PrmGetState
**
** Description      get patch ram state info
** Input Param      
**
**
** Returns          patch ram state
**
*******************************************************************************/
BT_API extern UINT8 BCM2045_PrmGetState (void);




#ifdef __cplusplus
}
#endif

#endif
