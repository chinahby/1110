/*===========================================================================

                          D S 7 0 7 _ J C D M A _ M 5 1
GENERAL DESCRIPTION

 This file contains the API's and functionality to implement the JCDMA
 M51 data functionality.

 Copyright (c) 2002-2007 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_jcdma_m51.c_v   1.6   02 Dec 2002 17:30:06   akhare  $
  $Header: //source/qcom/qct/modem/data/1x/jcdma/main/lite/src/ds707_jcdma_m51.c#2 $ $DateTime: 2007/12/10 21:46:00 $ $Author: nsivakum $
  
when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/15/07 sq/mga/yz Changed ERROR_FATAL to MSG_ERROR.
11/01/06    an     Featurize ATCOP, RMSM, Async Fax
10/28/03    ak     Add function to see if pkt or async/fax allowed.
12/02/02    ak     Changed make_pkt_call func name to include m51.
10/22/02    rsl    Updated FEATURE_JCDMA_DS_1X to FEATURE_JCDMA_1X
10/15/02    ak     Updated FEATURE_JCDMA_DS to FEATURE_JCDMA_DS_1X
10/02/02    ak     First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_JCDMA_1X
#include "cm.h"
#include "ds707_jcdma_m51.h"
#include "ds707_pkt_mgr.h"
#include "ds707_so_pkt.h"
#include "dstaski.h"
#include "err.h"
#include "msg.h"
#include "nv.h"
#include "ps_iface.h"
#include "sio.h"
#include "sys.h"

/*===========================================================================
                         PUBLIC VARIABLES
===========================================================================*/
/*---------------------------------------------------------------------------
  Variables used to track state of the M51* values.
---------------------------------------------------------------------------*/
sys_jcdma_m511_e_type     ds707_jcdma_m511_val;
sys_jcdma_m512_e_type     ds707_jcdma_m512_val;
sys_jcdma_m513_e_type     ds707_jcdma_m513_val;

/*===========================================================================
                          LOCAL VARIABLES
===========================================================================*/
/*---------------------------------------------------------------------------
  Variable used for writing to NV
---------------------------------------------------------------------------*/
LOCAL nv_item_type  ds707_jcdmai_nv_item;


/*===========================================================================
                      INTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_JCDMAI_SET_SIO_BAUDRATE

DESCRIPTION   Sets the SIO baudrate based on M51* settings.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_jcdmai_set_sio_baudrate(void)
{
  sio_bitrate_type   bitrate = SIO_BITRATE_AUTO;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch(ds707_jcdma_m513_val)
  {
    case SYS_JCDMA_M513_AUTO:
      if ((ds707_jcdma_m511_val == SYS_JCDMA_M511_PKT) &&
          (ds707_jcdma_m512_val == SYS_JCDMA_M512_HIGH)
         )
      {
        bitrate = SIO_BITRATE_230400;
      }
      else
      {
        bitrate = SIO_BITRATE_115200;
      }
      break;

    case SYS_JCDMA_M513_19200:
      bitrate = SIO_BITRATE_AUTO;
      break;

    case SYS_JCDMA_M513_115200:
      bitrate = SIO_BITRATE_115200;
      break;

    case SYS_JCDMA_M513_230400:
      bitrate = SIO_BITRATE_230400;
      break;

    default:
      ERR("Unknown M513 val %d", ds707_jcdma_m513_val, 0, 0);
      break;
  }

  /*-------------------------------------------------------------------------
    The following function updates IPR in NV as well as updates the SIO
    baudrate.  This function is special in that it can be called from any
    task (i.e, it does not require being in the DS task context).
  -------------------------------------------------------------------------*/
  dsat_change_baud(bitrate, TRUE);
} /* ds707_jcdmai_set_sio_baudrate() */

/*===========================================================================
                      EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_JCDMA_M51_INIT

DESCRIPTION   Initializes the JCDMA M51 module.  Called only once, at 
              startup.  Will read NV and if the values are bogus, then will
              write defaults.

DEPENDENCIES  Executes only in DS task.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_jcdma_m51_init(void)
{
  nv_stat_enum_type status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    M51   1
  -------------------------------------------------------------------------*/
  status = dsi_get_nv_item( NV_JCDMA_M511_MODE_I, &ds707_jcdmai_nv_item);
  if( status == NV_NOTACTIVE_S )
  {
    /*-----------------------------------------------------------------------
       At time of first boot, the NV item may not have been initialized yet.
       Initialize it from statically-initialized default.
    -----------------------------------------------------------------------*/
    ds707_jcdmai_nv_item.jcdma_m511_mode = DS707_JCDMA_M511_DEF;
    (void) dsi_put_nv_item( NV_JCDMA_M511_MODE_I, &ds707_jcdmai_nv_item );
  }
  else if( status != NV_DONE_S)
  {
    MSG_ERROR( "Bad NV read status %d for M51 1", status, 0, 0 );
  }
  /*-------------------------------------------------------------------------
    Store number retrieved from NV.
  -------------------------------------------------------------------------*/
  ds707_jcdma_m511_val = ds707_jcdmai_nv_item.jcdma_m511_mode;

  /*-------------------------------------------------------------------------
    M51   2
  -------------------------------------------------------------------------*/
  status = dsi_get_nv_item( NV_JCDMA_M512_MODE_I, &ds707_jcdmai_nv_item);
  if( status == NV_NOTACTIVE_S )
  {
    /*-----------------------------------------------------------------------
       At time of first boot, the NV item may not have been initialized yet.
       Initialize it from statically-initialized default.
    -----------------------------------------------------------------------*/
    ds707_jcdmai_nv_item.jcdma_m512_mode = DS707_JCDMA_M512_DEF;
    (void) dsi_put_nv_item( NV_JCDMA_M512_MODE_I, &ds707_jcdmai_nv_item );
  }
  else if( status != NV_DONE_S)
  {
    MSG_ERROR( "Bad NV read status %d for M51 2", status, 0, 0 );
  }
  /*-------------------------------------------------------------------------
    Store number retrieved from NV.
  -------------------------------------------------------------------------*/
  ds707_jcdma_m512_val = ds707_jcdmai_nv_item.jcdma_m512_mode;

  /*-------------------------------------------------------------------------
    M51   3
  -------------------------------------------------------------------------*/
  status = dsi_get_nv_item( NV_JCDMA_M513_MODE_I, &ds707_jcdmai_nv_item);
  if( status == NV_NOTACTIVE_S )
  {
    /*-----------------------------------------------------------------------
       At time of first boot, the NV item may not have been initialized yet.
       Initialize it from statically-initialized default.
    -----------------------------------------------------------------------*/
    ds707_jcdmai_nv_item.jcdma_m513_mode = DS707_JCDMA_M513_DEF;
    (void) dsi_put_nv_item( NV_JCDMA_M513_MODE_I, &ds707_jcdmai_nv_item );
  }
  else if( status != NV_DONE_S)
  {
    MSG_ERROR( "Bad NV read status %d for M51 3", status, 0, 0 );
  }
  /*-------------------------------------------------------------------------
    Store number retrieved from NV.
  -------------------------------------------------------------------------*/
  ds707_jcdma_m513_val = ds707_jcdmai_nv_item.jcdma_m513_mode;

  /*-------------------------------------------------------------------------
    Based on these settings, set up mobile to follow user settings.
  -------------------------------------------------------------------------*/
  ds707_so_pkt_recal();

  /*-------------------------------------------------------------------------
    Update the SIO baud rate, based on NV setting.
  -------------------------------------------------------------------------*/
  ds707_jcdmai_set_sio_baudrate();

  /* Register handlers for M51 mode changes */
  cm_m51_mode_reg(ds707_jcdma_chng_m511_mode,
				  ds707_jcdma_chng_m512_mode,
				  ds707_jcdma_chng_m513_mode);

} /* ds707_jcdma_m51_init() */


/*===========================================================================
FUNCTION      DS707_JCDMA_CHNG_M511_MODE

DESCRIPTION   Called by UI when the user selects a particular M511 setting.
              If it is okay to update the setting, will send a msg to DS
              so that the value can be changed.  This is because the new
              value is written into NV, and that should happen in DS task.

DEPENDENCIES  None

RETURN VALUE  TRUE - it is okay to change the setting.
              FALSE - invalid input.

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_jcdma_chng_m511_mode
(
  sys_jcdma_m511_e_type  new_mode
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ((new_mode <= SYS_JCDMA_M511_MIN) ||
      (new_mode >= SYS_JCDMA_M511_MAX)
     )
  {
    ERR("Invalid mode for M511 %d",new_mode,0,0);
  }
  else
  {
    ds707_jcdma_m511_val = new_mode;
    ds707_so_pkt_recal();
    ds707_jcdmai_set_sio_baudrate();
  }
  return(TRUE);
}

/*===========================================================================
FUNCTION      DS707_JCDMA_CHNG_M512_MODE

DESCRIPTION   Called by UI when the user selects a particular M512 setting.
              If it is okay to change the settings, then this will send a
              msg to DS, so that it can write the value into NV.  This 
              function is called in UI context.

DEPENDENCIES  None

RETURN VALUE  TRUE - it is okay to change the setting.
              FALSE - we are dormant or in an active call.  User not allowed
                      to change the setting.

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_jcdma_chng_m512_mode
(
  sys_jcdma_m512_e_type  new_mode
)
{ 
  ps_iface_type   *iface_ptr;
  boolean          ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  iface_ptr = ds707_pkt_get_iface_ptr(DS707_DEF_PKT_INSTANCE);

  if ((new_mode <= SYS_JCDMA_M512_MIN) ||
      (new_mode >= SYS_JCDMA_M512_MAX)
     )
  {
    ERR("Invalid mode for M512 %d",new_mode,0,0);
  }
  else if ((ps_iface_phys_link_state(iface_ptr) == PHYS_LINK_DOWN) &&
           ((ps_iface_state(iface_ptr) == IFACE_DISABLED) || 
            (ps_iface_state(iface_ptr) == IFACE_DOWN)
           )
          )
  {
    ds707_jcdma_m512_val = new_mode;
    ds707_so_pkt_recal();
    ds707_jcdmai_set_sio_baudrate();
    ret_val = TRUE;
  }
  else
  {
    MSG_HIGH("Cant do M512 - data call not null",0,0,0);
  }

  return(ret_val);
}/* ds707_jcdma_chng_m512_mode() */

/*===========================================================================
FUNCTION      DS707_JCDMA_CHNG_M513_MODE

DESCRIPTION   Called by UI when the user selects a particular M513 setting.

DEPENDENCIES  None

RETURN VALUE  TRUE if input value is okay.  else returns FALSE.

SIDE EFFECTS  None
===========================================================================*/
boolean  ds707_jcdma_chng_m513_mode
(
  sys_jcdma_m513_e_type  new_mode
)
{
  boolean          ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ((new_mode <= SYS_JCDMA_M513_MIN) ||
      (new_mode >= SYS_JCDMA_M513_MAX)
     )
  {
    ERR("Invalid mode for M513 %d",new_mode,0,0);
  }
  else
  {
    ds707_jcdma_m513_val = new_mode;
    ds707_so_pkt_recal();
    ds707_jcdmai_set_sio_baudrate();
    ret_val = TRUE;
  }

  return(ret_val);
} /* ds707_jcdma_chng_m513_mode() */

/*===========================================================================
FUNCTION      DS707_JCDMA_M51_MAKE_PKT_CALL

DESCRIPTION   Returns TRUE or FALSE, indicating if the next call should be
              pkt or async/fax.  This is decided by the M511 setting.

DEPENDENCIES  None

RETURN VALUE  TRUE  - M511 indicates make a packet call
              FALSE - M511 indicates a fax/async call.

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_jcdma_m51_make_pkt_call(void)
{ 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ds707_jcdma_m511_val == SYS_JCDMA_M511_PKT)
  {
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }
} /* ds707_jcdma_make_pkt_call() */


/*===========================================================================
FUNCTION      DS707_JCDMA_CHNG_M513_VIA_AT

DESCRIPTION   Called by AT when the user enters an AT+IPR or AT$QCTER
              command.
              
              Note that the input is not the enum used in other functions.
              It must be translated correctly.

DEPENDENCIES  None

RETURN VALUE  TRUE if input value is okay.  else returns FALSE.

SIDE EFFECTS  None
===========================================================================*/
boolean  ds707_jcdma_chng_m513_via_at
(
  sio_bitrate_type   new_ipr_val
)
{
  sys_jcdma_m513_e_type   new_mode;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (new_ipr_val)
  {
    case SIO_BITRATE_AUTO:
      new_mode = SYS_JCDMA_M513_19200;
      break;
    case SIO_BITRATE_115200:
      new_mode = SYS_JCDMA_M513_115200;
      break;
    case SIO_BITRATE_230400:
      new_mode = SYS_JCDMA_M513_230400;
      break;
    default:
      ASSERT(0);
      ERR("Unknown SIO bitrate %d",new_ipr_val,0,0);
      return(FALSE);
  }

  /*-------------------------------------------------------------------------
     Write value to NV
  -------------------------------------------------------------------------*/
  ASSERT(IS_IN_DS_TASK());
  ds707_jcdma_m513_val = new_mode;
  ds707_jcdmai_nv_item.jcdma_m513_mode = ds707_jcdma_m513_val;
  (void) dsi_put_nv_item( NV_JCDMA_M511_MODE_I, &ds707_jcdmai_nv_item );

  /*-------------------------------------------------------------------------
    Re-calibrate SO's and then set the baudrate.  This will do the write
    to the SIO driver.  Note that this will also store the value in NV.
  -------------------------------------------------------------------------*/
  ds707_so_pkt_recal();
  ds707_jcdmai_set_sio_baudrate();

  return(TRUE);
} /* ds707_jcdma_chng_m513_via_at() */

/*===========================================================================
FUNCTION      DS707_JCDMA_M51_IS_ASYNC_FAX_ALLOWED

DESCRIPTION   Returns TRUE/FALSE, indicating if the M511 value allows
              async/fax.

DEPENDENCIES  None

RETURN VALUE  TRUE if async/fax allowed.  else returns FALSE.

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_jcdma_m51_is_async_fax_allowed(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ds707_jcdma_m511_val == SYS_JCDMA_M511_ASYNC_FAX)
  {
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }
} /* ds707_jcdma_m51_is_async_fax_allowed() */

/*===========================================================================
FUNCTION      DS707_JCDMA_M51_IS_PKT_ALLOWED

DESCRIPTION   Returns TRUE/FALSE, indicating if the M511 value allows
              packet.

DEPENDENCIES  None

RETURN VALUE  TRUE if packet allowed.  else returns FALSE.

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_jcdma_m51_is_pkt_allowed(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ds707_jcdma_m511_val == SYS_JCDMA_M511_PKT)
  {
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }
} /* ds707_jcdma_m51_is_pkt_allowed() */

#endif /* FEATURE_JCDMA_1X */

