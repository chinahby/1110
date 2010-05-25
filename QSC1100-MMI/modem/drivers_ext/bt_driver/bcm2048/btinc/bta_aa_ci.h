/*****************************************************************************
**
**  Name:           bta_aa_ci.h
**
**  Description:    This is the interface file for advanced audio call-in
**                  functions.
**
**  Copyright (c) 2003, Widcomm Inc., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_AA_CI_H
#define BTA_AA_CI_H

#include "bta_aa_api.h"

/*****************************************************************************
**  Function Declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
**
** Function         bta_aa_ci_src_data_ready
**
** Description      This function sends an event to the AA indicating that
**                  the phone has audio stream data ready to send and AA
**                  should call bta_aa_co_src_data_path().                 
**
** Returns          void 
**
*******************************************************************************/
BTA_API extern void bta_aa_ci_src_data_ready(void);

/*******************************************************************************
**
** Function         bta_aa_ci_setconfig
**
** Description      This function must be called in response to function
**                  bta_aa_co_setconfig().  Parameter err_code is set to an
**                  AVDTP status value; AVDT_SUCCESS if the codec configuration
**                  is ok, otherwise error.                 
**
** Returns          void 
**
*******************************************************************************/
BTA_API extern void bta_aa_ci_setconfig(UINT8 err_code, UINT8 category,
                                        UINT8 num_seid, UINT8 *p_seid);

#ifdef __cplusplus
}
#endif

#endif /* BTA_AA_CI_H */

