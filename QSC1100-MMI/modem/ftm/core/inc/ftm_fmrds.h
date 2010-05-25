#ifndef FTM_FMRDS_H
#define FTM_FMRDS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                F T M   F M R D S   A P I   E X T E N S I O N 

GENERAL DESCRIPTION
  This is the FTM file which contains fmrds api specific commands

Copyright (c) 1994, 1995, 1996 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1997, 1998, 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2000, 2001, 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/fmradio/fmrds/dev/silab_ftm_test/ftm_fmrds.h#4 $$DateTime: 2008/10/01 10:53:09 $$Author: abhis $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
2008-07-25   as      Added support for seekstation()
2008-07-07   ap      Added support for Raw RDS, example service available
                     & AF support APIs
2008-07-07   as      Updated with build fixes
2008-06-27   ap      Initial Revision
===========================================================================*/

#include "comdef.h"
#include "target.h"


#ifdef FEATURE_FACTORY_TESTMODE
#include "ftmicap.h"

#ifdef FEATURE_FTM_FMRDS

#include "ftm.h"
#include "ftm_task.h"

/*===========================================================================
* Global Variables
===========================================================================*/

/*===========================================================================
* Packet Definitions
===========================================================================*/

/* ftm customer extensions request type */
typedef PACKED struct
{
  ftm_diagpkt_subsys_header_type  diag_hdr;
  ftm_cmd_header_type             ftm_hdr;
  void                            *data;
} ftm_fmrds_req_pkt_type;



/*===========================================================================
* Command Request Parameter Structures
===========================================================================*/

/*===========================================================================
* Command Result Parameter Structures
===========================================================================*/

/* Generic result, used for any command that only returns an error code */

typedef PACKED struct
{
  uint16  error_code;
}  ftm_fmrds_api_result_type;



/*===========================================================================
* Request/Response Unions
===========================================================================*/


/*===========================================================================
* Enums
===========================================================================*/

typedef enum 
{
  FTM_FMRDS_REGISTER_CLIENT   = 1, 
  FTM_FMRDS_DEREGISTER_CLIENT = 2,
  FTM_FMRDS_ENABLE_FM		  = 3,
  FTM_FMRDS_DISABLE_FM        = 4,
  FTM_FMRDS_SET_LOW_PWR_MODE  = 5,
  FTM_FMRDS_CONFIGURE_RADIO   = 6,
  FTM_FMRDS_SET_MUTE_MODE     = 7,
  FTM_FMRDS_SET_STEREO_MODE   = 8,
  FTM_FMRDS_SET_STATION       = 9,
  FTM_FMRDS_GET_TUNE_STATUS   = 10,
  FTM_FMRDS_SET_RADIO_MODE    = 11,
  FTM_FMRDS_SILAB_READ_REGS   = 12,
  FTM_FMRDS_SET_VOLUME_LEVEL  = 13,
  FTM_FMRDS_GET_RDS_PS_INFO   = 14,
  FTM_FMRDS_GET_RDS_RT_INFO   = 15,
  FTM_FMRDS_GET_RDS_LOCK_STATUS = 16,
  FTM_FMRDS_GET_RSSI_INFO     = 17,
  FTM_FMRDS_GET_STATION_PARAMETERS = 18,
  FTM_FMRDS_GET_RDS_AF_INFO = 19,
  FTM_FMRDS_SERVICE_AVAILABLE = 20,
  FTM_FMRDS_SET_RDS_RX_PROCESSING_OPTIONS = 21,
  FTM_FMRDS_SEEK_STATION       = 22
} ftm_frds_cmd_type;

typedef enum
{
 NORDS,
 RDS,
 RBDS
}rdsStd_type;

/*===========================================================================
* Error codes
===========================================================================*/
#define FTM_SUCCESS      			          0
#define FTM_FAIL                 			 1
#define FTM_FILE_DOES_NOT_EXIST   			 2
#define FTM_MMC_ERROR             			 3
#define FTM_FMRDS_API_UNRECOGNIZED_CMD     4
#define FTM_FMRDS_API_QFULL					 5
#define FTM_FMRDS_API_CMD_PENDING      	 6
#define FTM_TEST_NOT_IMPLEMENTED 			0xFF
#define FTM_CUST_HW_ID_UNKNOWN    			0xFF


/*===========================================================================
* Misc. definitions
===========================================================================*/

#define US_GENERIC_NORBDS            {US_EUROPE_BAND,    DE_EMP75, CHSPACE_200_KHZ, RDS_STD_NONE,	 {0, 0}}
#define US_GENERIC_RBDS              {US_EUROPE_BAND,    DE_EMP75, CHSPACE_200_KHZ, RDS_STD_RBDS,	 {0, 0}}
#define EUROPE_GENERIC_NORDS         {US_EUROPE_BAND,    DE_EMP50, CHSPACE_100_KHZ, RDS_STD_NONE,	 {0, 0}}
#define EUROPE_GENERIC_RDS           {US_EUROPE_BAND,    DE_EMP50, CHSPACE_100_KHZ, RDS_STD_RDS,	 {0, 0}}
#define JAPAN_GENERIC_NORDS          {JAPAN_STANDARD_BAND,         DE_EMP50, CHSPACE_100_KHZ, RDS_STD_NONE,	 {0, 0}}
#define JAPAN_WIDE_GENERIC_NORDS     {JAPAN_WIDE_BAND,     DE_EMP50, CHSPACE_100_KHZ, RDS_STD_NONE,	 {0, 0}}
#define JAPAN_GENERIC_RDS            {JAPAN_STANDARD_BAND,		 DE_EMP50, CHSPACE_100_KHZ, RDS_STD_RDS,	 {0, 0}}
#define JAPAN_WIDE_GENERIC_RDS       {JAPAN_WIDE_BAND,     DE_EMP50, CHSPACE_100_KHZ, RDS_STD_RDS,	 {0, 0}}


/*===========================================================================

FUNCTION FTM_FMRDS_API_DISPATCH

DESCRIPTION
   This function handles FMRDS API FTM requests to run tests and
   other primitives by dispatching the appropriate functions.

DEPENDENCIES
   None.

RETURN VALUE
   FTM response packet.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_fmrds_api_dispatch(ftm_fmrds_req_pkt_type *req_pkt);

#endif /* FEATURE_FTM_FMRDS */
#endif /* FEATURE_FACTORY_TESTMODE*/
#endif
