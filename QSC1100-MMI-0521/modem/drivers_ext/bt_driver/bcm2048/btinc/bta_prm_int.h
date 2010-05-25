/*****************************************************************************
**
**  Name:           bta_prm_int.h
**
**  Description:    This is the private file for the bta patch ram
**
**  Copyright (c) 2003-2005, Widcomm Inc., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_PRM_INT_H
#define BTA_PRM_INT_H

#include "bt_target.h"
#include "bta_sys.h"
#include "bta_prm_api.h"
#include "bta_fs_co.h"
#include "bta_fs_ci.h"
#include "bcm2045_prm_api.h"

/*****************************************************************************
**  Constants and data types
*****************************************************************************/

/* state machine events */
enum
{
    /* these events are handled by the state machine */
    BTA_PRM_API_START_PRM_EVT = BTA_SYS_EVT_START(BTA_ID_PRM),
    BTA_PRM_CI_OPEN_EVT,            /* Response to File Open request */
    BTA_PRM_CI_READ_EVT,            /* Response to Read request */
    BTA_PRM_LL_CONTINUE_EVT,        /* Lower lever continue event */
    BTA_PRM_LL_SUCCESS_EVT,         /* Lower lever patch ram complete event */
    BTA_PRM_LL_FAIL_EVT             /* Lower lever patch ram fail event */
};


/* data type for BTA_PRM_API_START_PRM_EVT */
typedef struct
{
    BT_HDR              hdr;
    tBTA_PRM_CBACK      *p_cback;
    char                *p_name; 
    UINT8               fs_app_id;
} tBTA_PRM_API_START_PRM;


/* union of all event data types */
typedef union
{
    BT_HDR                  hdr;
    tBTA_FS_CI_OPEN_EVT     open_evt; /* represent BTA_PRM_CI_OPEN_EVT */
    tBTA_FS_CI_READ_EVT     read_evt; /* represent BTA_PRM_CI_READ_EVT */
    tBTA_PRM_API_START_PRM  start_evt;/* represent tBTA_PRM_API_START_PRM */
} tBTA_PRM_DATA;


/* PRM control block */
typedef struct
{
    tBTA_PRM_CBACK  *p_cback;       /* pointer to application callback function */
    UINT32           file_length;   /* length of file being patched */
    UINT32           read_length;   /* length of file already been read */
    int              fd;            /* File Descriptor of opened file */
    UINT8            patch_data[MAX_PRM_LEN]; /* space for fs read co */
    UINT8            state;         /* state machine state */
    UINT8            app_id;
    BOOLEAN          internal_patch;/* internal code compile patch */     
} tBTA_PRM_CB;

/*****************************************************************************
**  Global data
*****************************************************************************/

/* PRM control block */
#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_PRM_CB  bta_prm_cb;
#else
extern tBTA_PRM_CB *bta_prm_cb_ptr;
#define bta_prm_cb (*bta_prm_cb_ptr)
#endif

extern BOOLEAN  bta_prm_hdl_event(BT_HDR *p_msg);

#endif


