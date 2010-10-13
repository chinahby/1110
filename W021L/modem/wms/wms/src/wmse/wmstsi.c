/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            W I R E L E S S   M E S S A G I N G   S E R V I C E S
            -- Translation Services

GENERAL DESCRIPTION
  The WMS module which implements the User API for SMS. This source file
  implements the translation functions for the client applications to call
  and for internal use.

Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008
              by QUALCOMM INCORPORATED.
  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/src/WMSE/wmstsi.c#2 $ 
$DateTime: 2008/12/02 13:02:59 $ $Author: pyaramad $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/31/08   PMD     Added Buffer Oveflow check in wms_ts_convert_to_bcd_address
01/31/08   PMD     Updated Copyright Information to include 2008
06/20/07   PMD     New Private TS File
===========================================================================*/


/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/


#include "comdef.h"

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

#include "wmstsi.h"
#include "wmsts.h"
#include "msg.h"


/*===========================================================================
============================ FUNCTIONS ======================================
===========================================================================*/
#ifdef FEATURE_GWSMS
#error code not present
#endif/*FEATURE_GWSMS*/

#ifdef FEATURE_CDSMS

#ifdef FEATURE_CDSMS_RUIM

/*=========================================================================
FUNCTION
  wms_ts_cdma_pack_ruim_sms

DESCRIPTION
  This function packs the RUIM SMS Message

DEPENDENCIES
  None

RETURN VALUE
  TRUE / FALSE

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_ts_cdma_pack_ruim_sms
(
  const wms_msg_ruim_sms_s_type     * ruim_sms, /* IN */
  uint8                             * result /* OUT */
)
{
  /*---- checking ---- */
  if ( ruim_sms == NULL || result == NULL)
  {
    MSG_ERROR("wms_ts_cdma_pack_ruim_sms null ptr!",0,0,0);
    return FALSE;
  }

  result[0] = ruim_sms->status;
  result[1] = ruim_sms->ota.data_len;

  if( ruim_sms->ota.data_len > sizeof(ruim_sms->ota.data) )
  {
    return FALSE;
  }
  else
  {
    memcpy( result+2, (uint8*)ruim_sms->ota.data, ruim_sms->ota.data_len );
    return TRUE;
  }

} /* wms_ts_cdma_pack_ruim_sms() */

/*=========================================================================
FUNCTION
  wms_ts_cdma_unpack_ruim_sms

DESCRIPTION
  This function unpacks the RUIM SMS Message

DEPENDENCIES
  None

RETURN VALUE
  TRUE / FALSE

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_ts_cdma_unpack_ruim_sms
(
  const uint8              * data,  /* IN */
  wms_msg_ruim_sms_s_type  * result /* OUT */
)
{

  /*---- checking ---- */
  if ( data == NULL || result == NULL)
  {
    MSG_ERROR("wms_ts_cdma_unpack_ruim_sms null ptr!",0,0,0);
    return FALSE;
  }

  result->status = data[0];
  result->ota.data_len = data[1];
  result->ota.format   = WMS_FORMAT_CDMA;

  if( result->ota.data_len > sizeof(result->ota.data) )
  {
    return FALSE;
  }
  else
  {
    memcpy( (uint8*)result->ota.data, data+2, result->ota.data_len );
    return TRUE;
  }

} /* wms_ts_cdma_unpack_ruim_sms() */

#endif /* FEATURE_CDSMS_RUIM */

#endif /* FEATURE_CDSMS */
  
#endif /* FEATURE_CDSMS || FEATURE_GWSMS */
