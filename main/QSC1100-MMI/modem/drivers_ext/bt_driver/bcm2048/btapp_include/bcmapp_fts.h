/*****************************************************************************
**                                                                            
**  Name:             bcmapp_fts.h                                              
**                                                                            
**  Description:     This file contains bcmapp interface               
**				     definition                                         
**                                                                            
**  Copyright (c) 2000-2004, Widcomm Inc., All Rights Reserved.               
**  Widcomm Bluetooth Core. Proprietary and confidential.                     
******************************************************************************/


#ifndef BCMAPP_FTS_H
#define BCMAPP_FTS_H

typedef struct
{
    UINT32  bytes_transferred;
    boolean b_fts_enable;
#if( defined BTA_FS_INCLUDED ) && (BTA_FS_INCLUDED == TRUE)
    tBTA_FT_ACCESS  access_flag;    /* BTA_FT_ACCESS_ALLOW or BTA_FT_ACCESS_FORBID */
#endif
} tBTUI_FTS_CB;

//extern tBTUI_FTS_CB bcmui_fts_cb;

void bcmapp_fts_init(void);
void bcmapp_fts_disable(void);
tBTUI_FTS_CB * bcmapp_get_fts_cb(void);
#endif
