#ifndef PARMDIAG_H
#define PARMDIAG_H

#include "customer.h"
#include "comdef.h"

#include "cai.h"
#include "cdma2kdiag.h" /* Note - this file may not exist forever */

#if defined (T_WINNT) || defined (FEATURE_WINCE)
#error code not present
#endif

/*===========================================================================

PACKET   diag_parm_set_req_type

ID       DIAG_PARM_SET_F

PURPOSE  Sent by DM to set one of the retreivable parameters.
         Parameters referred to here are the CAI's settable and
         retreival parameters listed in section 6.4.5 and Appendix E.

RESPONSE DMSS replaces current value of the given parameter with the one
         supplied, and sends a diag_parm_set_rsp_type packet back.
         

============================================================================*/
typedef PACKED struct {
  byte                cmd_code; 
  word                parm_id;                /* Parameter value, from
                                                 cai.h.  A parm_id of 0 will
                                                 cause ALL parmameters to
                                                 be cleared.                */
  dword               parm_value;             /* New value for parameter    */
} diag_parm_set_req_type;

typedef PACKED struct
{
  byte      cmd_code;           
  uint64    time;               /* Time stamp for operation */
} diag_parm_set_rsp_type;

/*===========================================================================

PACKET   diag_parm_get2_req_type

ID       DIAG_PARM_GET2_F

PURPOSE  Sent by DM to request the set of retreivable parameters.
         Parameters referred to here are the CAI's settable and
         retreival parameters listed in section 6.4.5 and Appendix E.
         This packet includes the MUX2 parameters listed in 6.4.5.2

RESPONSE DMSS places current value of all parameters in a diag_parm_
         get2_rsp_type packet and sends it back.
         

============================================================================*/
typedef PACKED struct
{
  byte                cmd_code;
} diag_parm_get2_req_type;

typedef PACKED struct {
  byte      cmd_code;                  /* Command code */
  uint64    time;                      /* Time stamp */
  dword     mux_rev[ 14 ];             /* Reverse Traffic Channel params */
  dword     mux_for[ 14 ];             /* Forward Traffic Channel params */
  dword     pag[ 7 ];                  /* Paging Channel params */
  dword     acc[ 8 ];                  /* Access Channel params */
  dword     layer2[ 5 ];               /* Layer2 params */
  uint64    sys_time;                  /* OTH_SYS_TIME parameter */
  dword     mux2_rev[ 26 ];            /* MUX2 Reverse Traffic Channel params */
  dword     mux2_for[ 26 ];            /* MUX2 Forward Traffic Channel params */
} diag_parm_get2_rsp_type;

#if defined(FEATURE_IS95B_MDR)
/*===========================================================================

PACKET   diag_parm_get_is95b_req_type

ID       DIAG_PARM_GET_IS95B_F

PURPOSE  Sent by DM to request the set of retreivable parameters.
         Parameters referred to here are the CAI's settable and
         retreival parameters listed in section 6.4.5 and Appendix E.
         This packet includes the MUX2 parameters listed in 6.4.5.2 as
         well as the SUPP (supplemental channel) parameters in 6.4.5.2 of
         the TIA/EIA IS-95-B standard.        

RESPONSE DMSS places current value of all parameters in a 
         diag_parm_get_is95b_rsp_type packet and sends it back.
         

============================================================================*/
typedef PACKED struct {
  byte                cmd_code;               /* Command code */
} diag_parm_get_is95b_req_type;

typedef PACKED struct {
   dword secondary;       /* 9600 bps or 14400 bps frame, blank-and-burst 
                             with secondary traffic only. */
   dword primary;         /* 9600 bps or 14400 bps frame,
                             primary traffic only. */
} diag_parm_sup_ch_type;

typedef PACKED struct {
  byte      cmd_code;                  /* Command code */
  qword     time;                      /* Time stamp */
  dword     mux_rev[ 14 ];             /* Reverse Traffic Channel params */
  dword     mux_for[ 14 ];             /* Forward Traffic Channel params */
  dword     pag[ 7 ];                  /* Paging Channel params */
  dword     acc[ 8 ];                  /* Access Channel params */
  dword     layer2[ 5 ];               /* Layer2 params */
  uint64    sys_time;                  /* OTH_SYS_TIME parameter */
  dword     mux2_rev[ 26 ];            /* MUX2 Reverse Traffic Channel parms */
  dword     mux2_for[ 26 ];            /* MUX2 Forward Traffic Channel parms */
  diag_parm_sup_ch_type sup_rev[DIAG_SUP_CHANNEL_MAX]; /* Reverse supplemental
                                                          channels */ 
  diag_parm_sup_ch_type sup_for[DIAG_SUP_CHANNEL_MAX]; /* Forward supplemental
                                                          channels */

} diag_parm_get_is95b_rsp_type;

#endif /* defined(FEATURE_IS95B_MDR) */

/*===========================================================================

PACKET   diag_parm_retrieve_req_type

ID       DIAG_PARM_RETRIEVE_F

PURPOSE  Sent by DM to set or retriecce a all or a subset set of retreivable
         parameters.  Parameters referred to here are the CAI's settable and
         retreival parameters listed in section 6.4.5 and Appendix E.

RESPONSE DMSS places the current value of all parameters in the range 
         specified.
         A range of 0--0 will return the full list of parameters.
         If an invalid range is speciied, DIAG_BAD_PARM_F is returned.

============================================================================*/
typedef enum {
  DIAG_PARM_STANDARD_RANGE_C = 0, /* Return range of valid standard PARMs */
  DIAG_PARM_STANDARD_C       = 1, /* PARM defined in TIA/EIA standard     */
  DIAG_PARM_EXTRA_RANGE_C    = 2, /* Return range of valid extra PARMs    */
  DIAG_PARM_EXTRA_C          = 3  /* PARM defined in TIA/EIA standard         */

} diag_parm_subcmd_enum_type;

typedef PACKED struct {
  uint8  cmd_code;      /* Command code */

  uint8  subcmd;        /* Use diag_parm_subcmd_enum_type */

  uint16 start_parm_id; /* refer to CAI */
  uint16 end_parm_id;   /* refer to CAI */

} diag_parm_retrieve_req_type;

typedef PACKED struct {
  uint8  cmd_code;      /* Command code */

  uint8  subcmd;        /* Use diag_parm_subcmd_enum_type */

  uint16 start_parm_id; /* refer to CAI */
  uint16 end_parm_id;   /* refer to CAI */

  uint64 time;          /* Time stamp of parm retrieval */

  /* PARM value */
#if defined(FEATURE_IS2000) || \
    defined(FEATURE_IS95B_MDR) || defined(FEATURE_IS95B_SPECIAL_MDR)
  uint32 parm[MAX(CAI_LAST_PARM_ID, CAI_XTRA_LAST_PARM_ID)];
#else
  uint32 parm[CAI_LAST_PARM_ID];
#endif

} diag_parm_retrieve_rsp_type;

#if defined (T_WINNT) || defined (FEATURE_WINCE)
#error code not present
#endif

#endif /* PARMDIAG_H */

