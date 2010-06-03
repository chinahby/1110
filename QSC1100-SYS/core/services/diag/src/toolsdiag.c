/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                Diagnostics Packet Processing Routines
                  WCDMA Targets Common Routines

General Description
  Core diagnostic packet processing routines that are common to all targets.

Copyright (c) 2001 - 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

$PVCSPath:  L:/src/asw/MSM5200/common/vcs/diagpkt_tools.c_v   1.6   02 Apr 2002 11:36:40   amcintos  $
$Header: //depot/asic/msmshared/services/diag/toolsdiag.c#25 $ $DateTime: 2008/10/06 15:46:52 $ $Author: vgandhi $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/03/08   vg      Updated code to use PACK() vs. PACKED
03/27/08   as      Added support to 32 bit HW model numbers.
07/12/06   as      Featurized for AppsProc,to prevent access to modem code.
10/31/05   as      Fixed lint errors.
04/25/05   as      Added extended build id response to include MSM 
                   hardware revision register under HW_REV_TYPE_V2.
12/17/04   as      Added check for FEATURE_HW_FULL_VERSION to get hw version
12/02/03   yll     Changed the mob_cai_rev to reflect the NV setting.
01/08/03   ma      featurization for GSM PLT
04/01/02   ATM     Added check for NULL after call to diagpkt_alloc
01/10/02   ATM     Corrected the ESN response length
09/21/01   abp     Using correct mobile model number for WCDMA
09/18/01   jal     Removed DIAG_STAT_F support
08/23/01   jal     Removed redundant DIAG_TS_F packet processing
05/08/01   cpe     renamed this file. Previously diagpkt_wcdma.c
05/07/01   lad     Cleaned up file.
02/23/01   lad     Created file.

===========================================================================*/
#ifdef __cplusplus
  extern "C" {
#endif

#include "comdef.h"
#include "customer.h"
#include "AEEstd.h"
#include "diag.h"
#include "diagbuf.h"
#include "diagcmd.h"
#include "diagi.h"
#include "diagnv.h"
#include "toolsdiag.h"

#include "dload.h"
#include "err.h"
#include "hw.h"
#ifdef FEATURE_IS2000
#include "mccdma.h"
#endif
#include "mobile.h"
#include "ts.h"

#if defined(FEATURE_DIAG_NV)
#include "nv.h"
#endif

#include <string.h>

/* Tell lint that we are OK with the fact that pkt_len and req_ptr are not 
   used in some of the packets, as this is dictated by the awesome design */
/* lint -esym(715,pkt_len,req_ptr) */



#ifndef IMAGE_APPS_PROC
/*===========================================================================

FUNCTION TOOLSDIAG_VERNO

DESCRIPTION
  This procedure processes a version number request.  It returns the version
  number, and the date and time of compilation.

===========================================================================*/
PACK(void *) toolsdiag_verno (PACK(void *) req_pkt, uint16 pkt_len)
{
  int i;
  word ver;
  DIAG_VERNO_F_rsp_type *rsp_ptr;
  const int rsp_len = sizeof(DIAG_VERNO_F_rsp_type);
#if defined(T_MSM6500) || defined(T_MSM6250)
  hw_rev_type new_ver; /* Buffer for the new MSM version information */
#endif /* T_MSM6500 or T_MSM6250 */

  (void) req_pkt; /* suppress compiler warning */
  (void) pkt_len; /* suppress compiler warning */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  rsp_ptr = (DIAG_VERNO_F_rsp_type *)diagpkt_alloc(DIAG_VERNO_F, rsp_len);

  if (rsp_ptr != NULL)
  {   /* Format response packet with version, date and time. */
      for ( i = 0; i < VERNO_DATE_STRLEN; i++ ) {
          rsp_ptr->comp_date[i] = ver_date[i];
          rsp_ptr->rel_date[i]  = rel_date[i];
      }

      for ( i=0; i<VERNO_TIME_STRLEN; i++ ) {
          rsp_ptr->comp_time[i] = ver_time[i];
          rsp_ptr->rel_time[i]  = rel_time[i];
      }

      for ( i=0; i<VERNO_DIR_STRLEN; i++ ) {
          rsp_ptr->ver_dir[i] = ver_dir[i];
      }

#if defined (FEATURE_IS2000) 
      rsp_ptr->scm = station_class_mark;
      rsp_ptr->mob_cai_rev = cdma.mob_cai_rev;

#if !defined(CUST_MOB_MODEL_EXT) 
      rsp_ptr->mob_model = mob_model;
#else
      rsp_ptr->mob_model = 0xff;   /* Invalid */
#endif

      rsp_ptr->mob_firm_rev = mob_firm_rev;

#if defined (FEATURE_HWTC) || defined (PLATFORM_LTK) || defined(FEATURE_GSM_PLT)
#error code not present
#else
      rsp_ptr->slot_cycle_index = cdma.slot_cycle_index_p;
#endif

#else

      /* This information doesn't make sense for non-IS2000 targets.  Stub out
         the fields. */
      rsp_ptr->scm = 0;
      rsp_ptr->mob_cai_rev = 0;

#if !defined(CUST_MOB_MODEL_EXT) 
      rsp_ptr->mob_model = mob_model;
#else
      rsp_ptr->mob_model = 0xff;   /* Invalid */
#endif

      rsp_ptr->mob_firm_rev = 0;
      rsp_ptr->slot_cycle_index = 0;

#endif /* #if defined (FEATURE_IS2000) && !defined (DIAG_APPS_PROC) */

      /* Supply the hardware version */
      /* NOTE: This should be in 0xF0nn format where F0 is hw_maj_ver      */
#if defined FEATURE_HW_FULL_VERSION
      ver = hw_version_full();
#else
      ver = hw_version();
#endif

#if defined(T_MSM6500) || defined(T_MSM6250)
      if (ver == 0xFF00)
      {
        /* 0xFF00 is reserved for Jaguar/Saber and later */
        hw_partnum_version(&new_ver);
        ver = new_ver.partnum;
      }
#endif /* T_MSM6500 or T_MSM6250 */
      rsp_ptr->hw_maj_ver = ver >> 8;     /* Upper nibble */
      rsp_ptr->hw_min_ver = (byte)(ver & 0x00FF); /* Lower nibble */
  }

  return (rsp_ptr);

} /* toolsdiag_verno */
#endif /* IMAGE_APPS_PROC */


/*===========================================================================

FUNCTION TOOLSDIAG_ESN

DESCRIPTION
  This procedure processes a received Mobile Station ESN request.

===========================================================================*/
PACK(void *) toolsdiag_esn (PACK(void *)req_pkt, uint16 pkt_len)
{
  DIAG_ESN_F_rsp_type *rsp_ptr;
  const int rsp_len = sizeof(DIAG_ESN_F_rsp_type);

  if (pkt_len != sizeof (DIAG_ESN_F_req_type))
  {
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  rsp_ptr = (DIAG_ESN_F_rsp_type *)diagpkt_alloc(DIAG_ESN_F, rsp_len);
  
  if (rsp_ptr != NULL)
  {
#ifdef FEATURE_IS2000

    /* Format response packet ESN. */
    rsp_ptr->esn = diagnv_get_esn ();

#else

    /* Format response packet with dummy value. */
    rsp_ptr->esn = 0xDEADD00DU;

#endif /* FEATURE_IS2000 */
  }
 
  return(rsp_ptr);

} /* toolsdiag_esn */

#ifndef IMAGE_APPS_PROC
/*===========================================================================

FUNCTION TOOLSDIAG_EXT_BUILD_ID

DESCRIPTION
  This procedure processes a version number request.  It returns the version
  number, and the date and time of compilation.

===========================================================================*/
PACK(void *) toolsdiag_ext_build_id (PACK(void *) req_pkt, uint16 pkt_len)
{
  toolsdiag_ext_build_id_rsp_type *build_id = NULL;
  char *str = NULL;
  uint32 rsp_len = FPOS (toolsdiag_ext_build_id_rsp_type, ver_strings);
  uint32 build_id_strlen = 0, model_strlen = 0; /* Includes NULL term char */
#if defined(T_MSM6500) || defined(T_MSM6250)
  hw_rev_type new_ver; /* Buffer for the new MSM version information */
#endif /* T_MSM6500 or T_MSM6250 */

  (void) req_pkt; /* suppress compiler warning */
  (void) pkt_len; /* suppress compiler warning */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* calc length for the "build_id" field */
  build_id_strlen = strlen (mob_sw_rev) + 1; /* NULL char */
  rsp_len += build_id_strlen;

  //XXX not supported
  model_strlen = 1; /* NULL char */
  rsp_len += model_strlen;

  
  build_id = (toolsdiag_ext_build_id_rsp_type *) 
    diagpkt_alloc (DIAG_EXT_BUILD_ID_F, rsp_len);

  if (build_id != NULL)
  {   
    build_id->msm_hw_version_format = 0;
    build_id->reserved[0] = 0;
    build_id->reserved[1] = 0;

#if defined FEATURE_HW_FULL_VERSION
    build_id->msm_hw_version = hw_version_full();
#else
    build_id->msm_hw_version = hw_version ();
#endif

#if !defined(CUST_MOB_MODEL_EXT)
     build_id->mobile_model_id = mob_model;
#else
     build_id->mobile_model_id = mob_model_ext;
#endif

#if defined(T_MSM6500) || defined(T_MSM6250)
    if (build_id->msm_hw_version == 0xFF00)
    {
      /* 0xFF00 is reserved for Jaguar/Saber and later */
      hw_partnum_version(&new_ver);
#if defined HW_REV_TYPE_V2
      build_id->msm_hw_version = new_ver.hw_register;
      build_id->msm_hw_version_format = 2;
#else
      build_id->msm_hw_version =  new_ver.partnum << 4;
      build_id->msm_hw_version |= new_ver.version;
      build_id->msm_hw_version_format = 1;
#endif
    }
#endif /* T_MSM6500 or T_MSM6250 */

    str = build_id->ver_strings;
    
    memset (str, 0, build_id_strlen + model_strlen);

    (void) std_strlcpy (str, mob_sw_rev, build_id_strlen);

    str += build_id_strlen;

    //XXX stubbed out
    (void) std_strlcpy (str, "", 1);
  }

  return (build_id);

} /* toolsdiag_ext_build_id */
#endif /* IMAGE_APPS_PROC */



/*===========================================================================
FUNCTION TOOLSDIAG_DLOAD_JUMP

DESCRIPTION
  Processes a request for the DMSS to jump to the download protocol.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  A download will occur as soon as this packet has been acknowledged. Normal
  processing will cease.

===========================================================================*/
PACK(void *) toolsdiag_dload_jump (PACK(void *) req_pkt, uint16 pkt_len)
{
  toolsdiag_dload_rsp_type *rsp;
  const int rsp_len = sizeof (toolsdiag_dload_rsp_type);
#ifdef IMAGE_MODEM_PROC
  int j;
#endif

  (void) req_pkt; /* suppress compiler warning */
  (void) pkt_len; /* suppress compiler warning */

  rsp = (toolsdiag_dload_rsp_type *) diagpkt_alloc (DIAG_DLOAD_F, rsp_len);

  if (rsp)
  {
    diagpkt_commit (rsp);
    rsp = NULL;
  }

  /* This call disables all streamed data services and flushes the buffers. */
  diagbuf_flush ();

#ifdef IMAGE_MODEM_PROC
  /* pause briefly to allow the response to reach QPST */
  for(j=0; j<1000; j++)
  {
    clk_pause(1000);
  }
#endif

  dload_entry ();
  /*lint -unreachable*/

  return (rsp);

} /* toolsdiag_dload_jump */

static const diagpkt_user_table_entry_type toolsdiag_tbl[] =
{
#ifndef IMAGE_APPS_PROC
  {DIAG_VERNO_F, DIAG_VERNO_F, toolsdiag_verno},
  {DIAG_EXT_BUILD_ID_F, DIAG_EXT_BUILD_ID_F, toolsdiag_ext_build_id},
#endif 
  {DIAG_ESN_F, DIAG_ESN_F, toolsdiag_esn},
  {DIAG_DLOAD_F, DIAG_DLOAD_F, toolsdiag_dload_jump}
};

#ifdef __cplusplus

  DIAGPKT_DISPATCH_AUTOREGISTER (DIAGPKT_NO_SUBSYS_ID, toolsdiag_tbl);

#else
  
  void toolsdiag_init (void)
  {
    DIAGPKT_DISPATCH_TABLE_REGISTER (DIAGPKT_NO_SUBSYS_ID, toolsdiag_tbl);
  }

#endif

/*lint +esym(715,pkt_len,req_ptr) */

#ifdef __cplusplus
  }
#endif
