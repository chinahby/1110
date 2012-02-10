#ifndef PRL_H
#define PRL_H

/*===========================================================================

        P R E F F E R E D   R O A M I N G   L I S T   H E A D E R

DESCRIPTION
  This file contains Preferred Roaming List declarations for targets that
  support SD 2.0.

Copyright (c) 1996,1997 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/sd/rel/08H1/inc/prl.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/02/08   ks      Porting SD code to support QDSP 6 based compiler
09/02/08   sv      Add band channel denifitions
08/12/08   fj      Added new API prl_commit2()
04/04/08   ak      Fixed valid channel range for BC0 as per 
                   3GPP2 C.P0057-C v0.1
12/19/07   ka      Changes to support 16k PRL
11/29/07   ak      Standardized channel validation and updated channel
                   lowest and highest values as per standard.
08/21/07   jqi/cl  Removed the cell/pcs band mapping features.
04/16/07   rp      Adding RPC support for prl_validate
02/12/06   jqi     Externalized prl_get_ptr() API;
01/04/07   jqi     Provide the utility APIs for user to change the acquisition
                   table of PRL
10/10/06   jqi     Added CDMA450 BLOCK L support.
04/14/06   jqi     Doubled the GSM/UMTS PRL table size.
03/05/06   jqi     Merged WCDMA BC3,4,8 and 9 support.
01/12/05   pk      Modified prli_acq_rec_type, added is_valid flag
                   Added PRL_INVALID_UNIQUE_SYS to prl_validation_enum_type
12/02/05   jqi     Lint fixes for lint ver8.
11/18/05   pk      Added support for sub-blocks I,J,K for BC 5 and BC 11
05/04/05   jqi     Added support for new Japan BC0 subclass2 and 3.
11/30/04   jqi     Externalize the API prl_hdr_extract().
09/03/04   RC      Added prl_get_total_size() to return total size of PRL.
06/04/04   RC      Added support for FFEATURE_2ND_PRL
05/06/04   jqi     Renamed T32, KPCS, BC5,6 with band mapping features.
05/03/04   jqi     Added UMTS 1700.850,800 band support.
04/19/04   jqi     Lint fix update.
03/08/04   jqi     Lint fix.
10/31/03   JQ      Added support for latest IS-683D format changes.
10/09/03   jqi     Adjusted band preference enum.
09/22/03   SJ      Added max enum to sel_mode_e_type.
08/18/03   RC      Fixed issues with IS-683D implementation.
07/15/03   JQ      Added Extended PRL support
                   - D PRL
                   - Removed FEATURE_IS683C_PRL
05/06/03   JQ      Added IS683B PRL support.
01/21/03   RC      Relaxed channel checking to include invalid channels.
01/16/03   SJ      Removed use of DB for SD20.
07/18/02   RC      Added prl_map_acq_rec_to_sd_mode and prl_sys_rec_get_sid.
05/28/02   az      Added support for new korean channels
05/02/02   RC      Added FEATURE_NO_CELL_GUARD_BANDS to eliminate cellular
                   guard bands checking.
03/14/02   RC      More PRL format changes to support IS-683C/SSPR_P_REV=3.
03/05/02   RC      PRL format changes to support IS-683C/SSPR_P_REV=3.
01/10/02   jqi     For band class 5 support in SD 1.0
                   1.added definitions of PRL_BLOCK_G and PRL_BLOCK_H.
                   2.redefined PRL_LOWEST_PCS_CHANNEL and PRL_HIGHEST_PCS_CHANNEL
                     if either FEATURE_BAND_CLASS_5 or FEATURE_KPCS was defined.
12/11/01   sj      Made mcsyspr_map_roam_ind_to_tsb58() extern.
12/07/01   jqi     For band class 5 support in SD 2.0
                   1. added definitions of PRL_BLOCK_G and PRL_BLOCK_H.
                   2. redefined PRL_LOWEST_PCS_CHANNEL and PRL_HIGHEST_PCS_CHANNEL
                      if either FEATURE_BAND_CLASS_5 or FEATURE_KPCS was defined.
12/03/01   RC      Changed 311 to 333 in Korean PCS frequency range check.
07/20/01   RC      Added support for IS-683C.
07/02/01   ht      Added defines for Cellular Sys chan upper and lower
                   limits and macros to check for in-range channel.
02/09/01   RC      Created new prl.h from old prl.h and prli.h for SD 2.0.
06/29/00   vn      Added function prototype mcsyspr_multiple_geo() - CR13283
03/16/00   vn      Merged fix for CR7346.
03/09/00   vn      Merged Wildcard SID feature.
02/08/99   ht      Fix Lint warnings.
01/26/99   dna     Change GET_GEO_FOR() to not depend on signed value.
                   Use bpackw/bunpackw for 9 bit acq_index.
01/26/99   ht      Add #defines for Fast PRL support.
01/25/99   br      Packed Unions for ARM Support.
12/18/98   pms     Moved prl_reformat() prototype from mcsyspr.h to here.
09/01/98   lcc     Merged in ARM support.
08/28/98   ht      Add macros and definitions for Fast PRL Searching
08/06/98   ych     Fixed Lint warnings
08/04/98   ych     Merged E911, SSPR-800, and Acq. Improvements. Also added
                   SSPR CDMA available GAM processing support.
07/22/98   abh     Merged E911, SSPR-800, Acq. Improvements.
02/24/98   dna     New type for pri_sec and new enum for cell CDMA w/chans
11/05/97   abh     Deleted definitions for MAX/MIN Cellular Band CDMA
                   channels
01/20/97   jrs     Added PRL_ prefix to identifiers. Moved internal typedefs
                   to prli.h.
11/13/96   jrs     Updates for cellular analog.
06/24/96   jrs     Created module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h" /* Customer configuration file */
#include "comdef.h"   /* Definition for basic types and macros */
#include "nv.h"       /* Interface to NV services */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to check whether a CDMA band-class 0 channel is in the A side.
*/
//#if defined( FEATURE_NO_CELL_GUARD_BANDS )

  #define PRL_IS_CHAN_IN_CELLULAR_SYS_A( chan ) \
      ( ((chan) >= 1      &&  (chan) <= 333)  || \
        ((chan) >= 667    &&  (chan) <= 716)  || \
        ((chan) >= 991    &&  (chan) <= 1023) )



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to check whether a CDMA band-class 0 channel is in the B side.
*/
//#if defined( FEATURE_NO_CELL_GUARD_BANDS )

  #define PRL_IS_CHAN_IN_CELLULAR_SYS_B( chan ) \
      ( ((chan) >= 334    && (chan) <= 666) || \
        ((chan) >= 717    && (chan) <= 799) )



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#include "sd.h"       /* External interface to sd.c */


/* The first half of this file is for targets with SD 2.0 support. The second
** half is for targets with no SD 2.0 support.
*/


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/*---------------------------------------------------------------------------
                                COMMON
---------------------------------------------------------------------------*/

/* Default PRL version number to be used for default PRL's. Note the a PRL
** that is read from NV might use this value as well.
*/
#define PRL_DEFAULT_VER 0

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Max. number of System Records that can be stored in the System Record
** Table. For non 1x targets use a small table size to save on RAM.
*/
#if( !defined( FEATURE_CDMA_800 ) && \
     !defined( FEATURE_CDMA_1900 ) && \
     !defined( FEATURE_ACP ) && \
     !defined( FEATURE_HDR ) )

  #define   PRL_TBL_MAX         20
  #define   PRL_DEFAULT_ENTRIES  0

#else

  #define   PRL_TBL_MAX         200
  #define   PRL_DEFAULT_ENTRIES  14


#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Enumeration of PRL SSPR_P_REV
*/
typedef enum
{
  PRL_SSPR_P_REV_INVALID = 0,
  PRL_SSPR_P_REV_1 = 1,              /* PRL */
  PRL_SSPR_P_REV_3 = 3,              /* Extended PRL */
  PRL_SSPR_P_REV_RESERVED = 4
} prl_sspr_p_rev_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Enumeration of PRL validation results.
*/
typedef enum
{
  PRL_INVALID_CRC = 0,         /* Invalid CRC  */
  PRL_INVALID_NUM_OF_RECORDS,  /* Invalid number of records */
  PRL_INVALID_RECORD,          /* Invalid record */
  PRL_INVALID_SIZE,            /* Invalid size of the roaming list */
  PRL_VALID,                   /* Valid PRL */
  PRL_INVALID_SSPR_P_REV,      /* Invalid format revision (SSPR_P_REV) */
  PRL_INVALID_UNIQUE_SYS,      /* Invalid unique Mode, band and channel combinations */
  PRL_VALIDATE_MAX_TYPE
} prl_validation_enum_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Enumeration of PRL commit results.
*/
typedef enum
{
  PRL_COMMIT_STATUS_NONE = -1,
  PRL_COMMIT_STATUS_DONE = 0,
  PRL_COMMIT_STATUS_NV_FAIL,
  PRL_COMMIT_STATUS_PRL_INVALID
} prl_commit_status_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* If the phone is currently using a default roaming list, and which one */
typedef enum
{
  PRL_SOURCE_NV,
  PRL_SOURCE_A_ONLY,
  PRL_SOURCE_B_ONLY,
  PRL_SOURCE_STANDARD,
  PRL_SOURCE_OTASP
} prl_source_enum;


/*---------------------------------------------------------------------------
                             PRL SYSTEM RECORD
---------------------------------------------------------------------------*/

/* Enumeration of system record types.
*/
typedef enum
{
  PRL_SYS_REC_IS2000,    /* IS-2000S system record for CDMA and AMPS */
  PRL_SYS_REC_IS856,     /* IS-856 system record for HDR */
  PRL_SYS_REC_PLMN,      /* PLMN system record for GSM/WCDMA */
  PRL_SYS_REC_MAX        /* For range checking */
} prl_sys_rec_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of wildcard SID / NID types
*/
typedef enum
{
  PRL_WCARD_NOT,         /* NOT wildcard */
  PRL_WCARD_SID,         /* SID/PLMN wildard */
  PRL_WCARD_NID,         /* NID/LAC wildcard */
  PRL_WCARD_UNKNOWN,      /* Unknow wildcard */
  PRL_WCARD_MAX          /* For range checking */
} prl_wcard_e_type;

/* Roaming list value for a wildcard SID.
*/
#define PRL_WILDCARD_SID 0

/* Roaming list value for a wildcard NID.
*/
#define PRL_WILDCARD_NID 65535

/* Roaming list value for SIDs that inicate that an MCC/IMIS_11_12 are
** encoded into the SID/NID fields.
*/
#define PRL_MCC_SID_0  15904
#define PRL_MCC_SID_1  15905

#define PRL_IS_MCC_SID( sid ) ( (sid == PRL_MCC_SID_0) || \
                                (sid == PRL_MCC_SID_1) )

/* Roaming List Value for a Public Network NID.
*/
#define PRL_PUBLIC_NETWORK_NID 0

/* Roaming List value for a wildcard UARFCN.
*/
#define PRL_WILDCARD_UARFCN 0

/* Romaing List value for a wildcard LAC
*/
#define PRL_WILDCARD_LAC    0xfffe

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* This enumerated type defines the values the NID_INCL field in the System
** Record can assume
*/
typedef enum
{
  PRL_NID_NOT_INCL_WILDCARD_NID=0,    /* NID Not included in Sys Record */
  PRL_NID_INCL=1,                     /* NID Included in Sys Record */
  PRL_NID_NOT_INCL_PUBLIC_NID=2,      /* NID Not included in Sys Record */
  PRL_NID_INCL_RESERVED=3             /* Reserved */
} prl_nid_incl_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* This enumerated type defines the values that the neg_pref field can
* contain
*/
typedef enum
{
  PRL_NEGATIVE,               /* A Negative system record */
  PRL_PREFERRED               /* A Preferred system record */
} prl_neg_pref_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* This enumerated type defines the values that the geo region field can
* contain
*/
typedef enum
{
  PRL_NEW_GEO_REGION = 0,     /* Delineates a new geo region */
  PRL_SAME_GEO_REGION         /* This entry is in the same geo region as */
                              /* the previous entry */
} prl_geo_region_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* This enumerated type defines the values that the pri field can contain
*/
typedef enum
{
  PRL_PRI_SAME = 0,           /* This entry is the same priority as the */
                              /* next entry */
  PRL_PRI_HIGHER              /* This entry is  higher priority than the */
                              /* next entry */
} prl_pri_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* This enumerated type defines the values that the PLMN/LAC include field
** can contain.
*/
typedef enum
{
  PRL_PLMN_LAC_NOT_INCL=0,            /* PLMN and LAC Not included */
  PRL_PLMN_INCL=1,                    /* PLMN Included */
  PRL_PLMN_LAC_INCL=2,                /* PLMN and LAC included */
  PRL_PLMN_MULTI_LAC_INCL=3,          /* PRLMN and multiple LAC included */
  PRL_PLMN_LAC_INCL_RESERVED=4        /* Reserved 4 - 7*/

//  PRL_PLMN_LAC_USER_SPECIFIED = 8     /* For SD internal use only.*/
                                      /* To indicate record in user list */
} prl_plmn_lac_incl_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* This enumerated type defines the values that the PLMN select mode field
** can contain.
*/
typedef enum
{
  PRL_PLMN_SEL_MODE_ANY_PLMN=0,       /* Any PLMNs in EF */
  PRL_PLMN_SEL_MODE_ALLOWED_PLMN=1,   /* Any PLMNs exceptfor PLMNs in EF */
  PRL_PLMN_SEL_MODE_HOME_PLMN=2,      /* Home PLMN */
  PRL_PLMN_SEL_MODE_RESERVED=3,       /* Reserved 3 */
  PRL_PLMN_SEL_MODE_MAX               /* For SD internal use only */
} prl_plmn_sel_mode_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* This enumerated typedefines the values that network selection mode field
** can contains.
*/
typedef enum
{
  PRL_PLMN_NET_SEL_MODE_MANUAL=0,     /* manual network selection mode */
  PRL_PLMN_NET_SEL_MODE_AUTO=1,       /* automatic network selection mode */
  PRL_PLMN_NET_SEL_MODE_MAX           /* For SD internal use only */
} prl_plmn_net_sel_mode_e_type;

/*---------------------------------------------------------------------------
                          PRL ACQUISITION RECORD
---------------------------------------------------------------------------*/

/* This enumerated type lists the types of acquisition records
*/
typedef enum
{
  PRL_ACQ_REC_RESERVED,                 /* Reserved */
  PRL_ACQ_REC_CELLULAR_ANALOG,          /* Cellular analog */
  PRL_ACQ_REC_CELLULAR_CDMA_STD,
  PRL_ACQ_REC_CELLULAR_CDMA_CUST,
  PRL_ACQ_REC_CELLULAR_GENERIC,         /* Cellular generic */
  PRL_ACQ_REC_PCS_CDMA_BLOCKS,          /* PCS CDMA using blocks */
  PRL_ACQ_REC_PCS_CDMA_CHANNELS,        /* PCS CDMA using channels */

  /* The following three ACQ REC types apply for IS683B PRL or above */
  PRL_ACQ_REC_JTACS_CDMA_STD,           /* JTACS CDMA standard channels */
  PRL_ACQ_REC_JTACS_CDMA_CUST,          /* JTACS CDMA custom channels */
  PRL_ACQ_REC_CDMA_2GHZ_CHANS,          /* 2G BAND CDMA using channels */

  /* The following three ACQ REC types apply for IS683C PRL or above */
  PRL_ACQ_REC_CDMA_GENERIC,             /* CDMA Generic */
  PRL_ACQ_REC_HDR_GENERIC,              /* HDR Generic */

  /* The following three ACQ REC types apply for IS683D PRL or above */
  PRL_ACQ_REC_GSM_GENERIC,              /* GSM Generic*/
  PRL_ACQ_REC_WCDMA_GENERIC,            /* WCDMA Generic*/
  PRL_ACQ_REC_GW_PREFERRED,             /* GSM/WCDMA perferred */

  PRL_ACQ_REC_NONE
} prl_acq_rec_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* This enumerated type defines the values that the a_b field can contain
*/
typedef enum
{
  PRL_AB_SYSTEM_A,            /* System A */
  PRL_AB_SYSTEM_B,            /* System B */
  PRL_AB_RESERVED,            /* Reserved */
  PRL_AB_SYSTEM_A_OR_B        /* System A or system B */
} prl_ab_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* This enumerated type defines the values that the pri_sec field can contain
*/
typedef enum
{
  PRL_PRISEC_RESERVED=0,           /* Reserved */
  PRL_PRISEC_PRIMARY=1,            /* Primary Channel Only */
  PRL_PRISEC_SECONDARY=2,          /* Secondary Channel Only */
  PRL_PRISEC_PRI_OR_SEC=3          /* Primary or Secondary Channel */
} prl_pri_sec_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The valid blocks that can be defined in a roaming list
*/
#define PRL_BLOCK_A         0
#define PRL_BLOCK_B         1
#define PRL_BLOCK_C         2
#define PRL_BLOCK_D         3
#define PRL_BLOCK_E         4
#define PRL_BLOCK_F         5
#define PRL_BLOCK_RESERVED  6
#define PRL_BLOCK_WILDCARD  7

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Table associating PCS frequency blocks with their set of
  ** preferred CDMA channels.
  ** Please refer standard TIA/EIA-97-D
  ** Table 3.1.2-4. CDMA Preferred Set of Frequency Assignments for Band Class 1
  ** Table 3.1.5-4. CDMA Preferred Set of Frequency Assignments for Band Class 4
  ** Table 3.1.6-4. CDMA Preferred Set of Frequency Assignments for Band Class 5

  */
  static const struct {

      byte      block;
          /* PCS frequency block */

      word      chan;
          /* Associated CDMA channel number */

  } prl_pcs_block_chan_tbl[] = {

    /* US PCS block A preferred channels
    */
    { PRL_BLOCK_A,    25 },
    { PRL_BLOCK_A,    50 },
    { PRL_BLOCK_A,    75 },
    { PRL_BLOCK_A,    100 },
    { PRL_BLOCK_A,    125 },
    { PRL_BLOCK_A,    150 },
    { PRL_BLOCK_A,    175 },
    { PRL_BLOCK_A,    200 },
    { PRL_BLOCK_A,    225 },
    { PRL_BLOCK_A,    250 },
    { PRL_BLOCK_A,    275 },

    /* US PCS block B preferred channels.
    */
    { PRL_BLOCK_B,    425 },
    { PRL_BLOCK_B,    450 },
    { PRL_BLOCK_B,    475 },
    { PRL_BLOCK_B,    500 },
    { PRL_BLOCK_B,    525 },
    { PRL_BLOCK_B,    550 },
    { PRL_BLOCK_B,    575 },
    { PRL_BLOCK_B,    600 },
    { PRL_BLOCK_B,    625 },
    { PRL_BLOCK_B,    650 },
    { PRL_BLOCK_B,    675 },

    /* US PCS block C preferred channels.
    */
    { PRL_BLOCK_C,    925 },
    { PRL_BLOCK_C,    950 },
    { PRL_BLOCK_C,    975 },
    { PRL_BLOCK_C,    1000 },
    { PRL_BLOCK_C,    1025 },
    { PRL_BLOCK_C,    1050 },
    { PRL_BLOCK_C,    1075 },
    { PRL_BLOCK_C,    1100 },
    { PRL_BLOCK_C,    1125 },
    { PRL_BLOCK_C,    1150 },
    { PRL_BLOCK_C,    1175 },

    /* US PCS block D preferred channels.
    */
    { PRL_BLOCK_D,    325 },
    { PRL_BLOCK_D,    350 },
    { PRL_BLOCK_D,    375 },

    /* US PCS block E preferred channels.
    */
    { PRL_BLOCK_E,    725 },
    { PRL_BLOCK_E,    750 },
    { PRL_BLOCK_E,    775 },

    /* US PCS block F preferred channels.
    */
    { PRL_BLOCK_F,    825 },
    { PRL_BLOCK_F,    850 },
    { PRL_BLOCK_F,    875 }

  };

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Define Block G and Block L for Band Class 5 support.
** They are used for internal purpose to support Band Class 5.
** Note that Block G and Block H has values that are not supported by
** IS683-A.
*/

#define PRL_BLOCK_G         8
#define PRL_BLOCK_H         9
#define PRL_BLOCK_I        10
#define PRL_BLOCK_J        11
#define PRL_BLOCK_K        12
#define PRL_BLOCK_L        13

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The lowest and highest CDMA PCS channel numbers
*/
#define PRL_LOWEST_PCS_CHANNEL  0
#define PRL_HIGHEST_PCS_CHANNEL 1199

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* This enumerated type defines the values the UARFCN_INCL field in the ACQ
** Record
*/
typedef enum
{
  PRL_UARFCN_NOT_INCL=0,              /* UARFCN Not included in ACQ Record */
  PRL_UARFCN_INCL=1,                  /* UARFCN Included in ACQ Record */
  PRL_UARFCN_INCL_RESERVED=2
} prl_uarfcn_incl_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* This enumerated type defines the values the GSM/WCDMA preferred system
** indicator in the ACQ Record
*/
typedef enum
{
  PRL_GW_PREF_SYS_IND_GSM=0,          /* GSM system preferred */
  PRL_GW_PREF_SYS_IND_WCDMA=1,        /* WCDMA system preferred */
  PRL_GW_PREF_SYS_IND_RESERVED=2
} prl_gw_pref_sys_ind_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of GSM bands that can be used in GSM acquisition record.
*/
typedef enum
{
  PRL_GSM_450 = 0,
  PRL_GSM_480 = 1,
  PRL_GSM_750 = 2,
  PRL_GSM_850 = 3,
  PRL_GSM_PGSM_900 = 4,
  PRL_GSM_EGSM_900 = 5,
  PRL_GSM_RGSM_900 = 6,
  PRL_GSM_DCS_1800 = 7,
  PRL_GSM_PCS_1900 = 8,
  PRL_GSM_BAND_MAX     /* All other GSM BAND values are reserved.*/
} prl_gsm_band_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of WCDMA bands that can be used in WCDMA acquisition record.
*/
typedef enum
{
  PRL_WCDMA_I_IMT_2000 = 0,
  PRL_WCDMA_II_PCS_1900 = 1,
  PRL_WCDMA_III_1700 = 2,
  PRL_WCDMA_IV_1700 = 3,
  PRL_WCDMA_V_850 = 4,
  PRL_WCDMA_VI_800 = 5,
  PRL_WCDMA_VII_2600 = 6,
  PRL_WCDMA_VIII_900 = 7,
  PRL_WCDMA_IX_1700 = 8,
  PRL_WCDMA_BAND_MAX   /* All other WCDMA BAND values are reserved.*/
} prl_wcdma_band_e_type;

/*---------------------------------------------------------------------------
                           EXTERNAL PRL TYPES
---------------------------------------------------------------------------*/

/* Length in bits of PRL header fields.
*/
#define PR_LIST_SIZE_LENGTH    16
#define PR_LIST_ID_LENGTH      16
#define PREF_ONLY_LENGTH       1
#define ROAM_IND_LENGTH        8
#define ACQ_REC_INDEX_LENGTH   9
#define SYS_REC_INDEX_LENGTH   14
#define CRC_LENGTH             16

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Length in bits of common acquisition record fields.
*/
/* sspr_p_rev only can be either 3 or 1*/
#define ACQ_REC_TYPE_LENGTH(sspr_p_rev)     \
                                    ((sspr_p_rev == PRL_SSPR_P_REV_3)? 8:  \
                                    ((sspr_p_rev == PRL_SSPR_P_REV_1)? 4:0 ))

#define ACQ_REC_LENGTH_LENGTH(sspr_p_rev) \
                                    ((sspr_p_rev == PRL_SSPR_P_REV_3)? 8:0)
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Length in bits of common system record fields.
*/
#define SYS_REC_SID_LENGTH      15
#define SYS_REC_NID_INCL_LENGTH 2
#define SYS_REC_NID_LENGTH      16
#define SYS_REC_PREF_NEG_LENGTH 1
#define SYS_REC_GEO_LENGTH      1
#define SYS_REC_PRI_LENGTH      1

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Length in bits of additional header fields.
*/
#define SSPR_P_REV_LENGTH               8
#define NUM_SUBNET_RECS_LENGTH          9
#define SSPR_P_REV_3_RESERVED_LENGTH    7

/* Length in bits of leading common system record fields.
*/
#define SYS_REC_LENGTH_LENGTH           5
#define SYS_REC_TYPE_LENGTH             4
#define SYS_REC_IS2000_RESERVED_LENGTH  1
#define SYS_REC_IS856_RESERVED_LENGTH   3
#define SYS_REC_PLMN_RESERVED_LENGTH    3

/* never been used */
#define SYS_REC_COMMON_LENGTH   ( SYS_REC_LENGTH_LENGTH + \
                                  SYS_REC_TYPE_LENGTH + \
                                  SYS_REC_PREF_NEG_LENGTH + \
                                  SYS_REC_GEO_LENGTH + \
                                  SYS_REC_PRI_LENGTH + \
                                  ACQ_REC_INDEX_LENGTH + \
                                  SYS_REC_RESERVED_LENGTH )

/* Length in bits of trailing common system record fields.
*/
#define SYS_REC_ASSOCIATION_INC_LENGTH    1
#define SYS_REC_ASSOCIATION_TAG_LENGTH    8
#define SYS_REC_PN_ASSOCIATION_LENGTH     1
#define SYS_REC_DATA_ASSOCIATION_LENGTH   1


/* Length in bits of subnet specific system record fields.
*/
#define SYS_REC_SUBNET_COM_INC_LENGTH     1
#define SYS_REC_SUBNET_LSB_LENGTH         7
#define SYS_REC_SUBNET_COM_OFFSET_LENGTH  12


/* Length in bits of subnet record fields.
*/
#define COM_SUBNET_REC_RESERVED_LENGTH    4
#define COM_SUBNET_REC_LENGTH_LENGTH      4

#define NV_ROAMING_LIST_ITEM    NV_ROAMING_LIST_683_I

#define SSPR_P_REV_1_PRL_FIXED_LENGTH  ( PR_LIST_SIZE_LENGTH + \
                                         PR_LIST_ID_LENGTH + \
                                         PREF_ONLY_LENGTH + \
                                         ROAM_IND_LENGTH + \
                                         ACQ_REC_INDEX_LENGTH + \
                                         SYS_REC_INDEX_LENGTH )


#define SSPR_P_REV_3_PRL_FIXED_LENGTH  ( PR_LIST_SIZE_LENGTH + \
                                         PR_LIST_ID_LENGTH + \
                                         SSPR_P_REV_LENGTH + \
                                         PREF_ONLY_LENGTH + \
                                         ROAM_IND_LENGTH + \
                                         ACQ_REC_INDEX_LENGTH + \
                                         NUM_SUBNET_RECS_LENGTH + \
                                         SYS_REC_INDEX_LENGTH + \
                                         SSPR_P_REV_3_RESERVED_LENGTH )

#define PRL_FIXED_ROAMING_LIST_LENGTH(sspr_p_rev) \
      ((sspr_p_rev == PRL_SSPR_P_REV_3) ? SSPR_P_REV_3_PRL_FIXED_LENGTH: \
      ((sspr_p_rev == PRL_SSPR_P_REV_1) ? SSPR_P_REV_1_PRL_FIXED_LENGTH:0) \
                   )

#define MINIMUM_PRL_SIZE(sspr_p_rev)  \
                 ((PRL_FIXED_ROAMING_LIST_LENGTH(sspr_p_rev)+ CRC_LENGTH)/8)


#define SYS_REC_PLMN_LAC_INCL_LENGTH    3
#define SYS_REC_PLMN_SELECT_MODE_LENGTH 2
#define SYS_REC_PLMN_LENGTH             24
#define SYS_REC_LAC_LENGTH              16
#define SYS_REC_NET_SELECT_MODE_LENGTH 1
#define SYS_REC_NUM_LAC_LENGTH          4
#define SYS_REC_MULTI_LAC_RESERVED_LENGTH 3

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define PRL_ACQ_REC_HEADER_LENGTH(sspr_p_rev)     \
        (ACQ_REC_TYPE_LENGTH(sspr_p_rev)+ ACQ_REC_LENGTH_LENGTH(sspr_p_rev))

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The cellular analog system acquisition record
*/
typedef PACKED struct PACKED_POST
{
  byte a_b[2];                          /* System A/B selection */
} prl_cellular_analog_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The cellular CDMA system acquisition record
*/
typedef PACKED struct PACKED_POST
{
  byte a_b[2];                          /* System A/B selection */
  byte pri_sec[2];                      /* Primary/secondary CDMA channel */
} prl_cellular_cdma_type;               /* selection                      */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef PACKED struct PACKED_POST
{
  byte num_chans[5];
  byte chan[11];
} prl_cellular_cdma_custom_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The cellular generic system acquisition record
*/
typedef PACKED struct PACKED_POST
{
  byte a_b[2];                          /* System A/B selection */
} prl_cellular_generic_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The PCS CDMA system acquisition (using blocks) record
*/
typedef PACKED struct PACKED_POST
{
  byte num_blocks[3];                   /* Number of frequency blocks */
  byte block[3];                        /* PCS frequency block */
} prl_pcs_cdma_using_blocks_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The PCS CDMA system acquisition (using channels) record
*/
typedef PACKED struct PACKED_POST
{
  byte num_chans[5];                    /* Number of channels */
  byte chan[11];                        /* PCS CDMA channel */
} prl_pcs_cdma_using_chans_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* The following three ACQ REC types apply for IS683B PRL or above */
/* The JTACS CDMA system acquisition (standard channels) record
 */
typedef PACKED struct PACKED_POST
{
  byte a_b[2];                          /* System A/B selection */
  byte pri_sec[2];                      /* Primary/secondary CDMA channel */
} prl_jtacs_cdma_std_type;                  /* selection                      */

/* The JTACS CDMA system acquisition (custom channels) record
 */
typedef PACKED struct PACKED_POST
{
 byte num_chans[5];
 byte chan[11];
} prl_jtacs_cdma_cust_type;

/* The 2G BAND CDMA system acquisition (using channels) record
 */
typedef PACKED struct PACKED_POST
{
 byte num_chans[5];
 byte chan[11];
} prl_cdma_2ghz_chans_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The CDMA generic acquisition record
*/
typedef PACKED struct PACKED_POST
{
  byte rec[16];                         /* 16 bit record - 5bit band-class +
                                        ** 11bit channel number */
} prl_cdma_generic_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The HDR generic acquisition record
*/
typedef PACKED struct PACKED_POST
{
  byte rec[16];                         /* 16 bit record - 5bit band-class +
                                        ** 11bit channel number */
} prl_hdr_generic_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The GSM generic acquisition record
*/
typedef PACKED struct PACKED_POST
{
  byte num_bands[5];
  byte band[5];

} prl_gsm_generic_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The WCDMA generic acquisition record
*/
typedef PACKED struct PACKED_POST
{
  byte num_bands[5];
  byte uarfcn_incl[1];
  byte band[5];
  byte uarfcn[14];

} prl_wcdma_generic_type;

/* The GSM/UMTS preferred acquisition record
*/
typedef PACKED struct PACKED_POST
{
  byte pref_sys_ind[1];
  byte num_gsm_bands[5];
  byte num_wcdma_bands[5];
  byte gsm_band[5];
  byte wcdma_band[5];
  byte uarfcn_incl[1];
  byte uarfcn[14];
} prl_gw_pref_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The system record format (IS-683A)
*/
typedef PACKED struct PACKED_POST
{
  byte sid[SYS_REC_SID_LENGTH];           /* System identification */
  byte nid_incl[SYS_REC_NID_INCL_LENGTH]; /* NID included */
  byte nid[SYS_REC_NID_LENGTH];           /* Network identification */
  byte neg_pref[SYS_REC_PREF_NEG_LENGTH]; /* Negative/preferred system */
  byte geo[SYS_REC_GEO_LENGTH];           /* Geographical region indicator */
  byte pri[SYS_REC_PRI_LENGTH];           /* Relative priority indicator */
  byte acq_index[ACQ_REC_INDEX_LENGTH];   /* Acquisition record index */
  byte roam_ind[ROAM_IND_LENGTH];         /* Roaming indicator */
} prl_sys_record_type;



/*---------------------------------------------------------------------------
                           INTERNAL PRL TYPES
---------------------------------------------------------------------------*/

/* The cellular analog system acquisition record
*/
typedef struct
{
  prl_ab_type a_b;                      /* System A/B selection type */
} prli_cellular_analog_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The cellular CDMA system acquisition record
*/
typedef struct
{
  prl_ab_type a_b;                      /* System A/B selection type */
  prl_pri_sec_type pri_sec;             /* Primary/secondary CDMA channel */
} prli_cellular_cdma_type;              /* selection type */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef struct
{
  int num_chans;                        /* Number of channels */
  word chan[32];                        /* Channels */
} prli_cellular_cdma_custom_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The cellular generic system acquisition record
*/
typedef struct
{
  prl_ab_type a_b;                      /* System A/B selection type */
} prli_cellular_generic_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The PCS CDMA system acquisition (using blocks) record
*/
typedef struct
{
  int num_blocks;                       /* Number of blocks */
  word block[7];                        /* Blocks */
} prli_pcs_cdma_block_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The PCS CDMA system acquisition (using channels) record
*/
typedef struct
{
  int num_chans;                        /* Number of channels */
  word chan[32];                        /* Channels */
} prli_pcs_cdma_chan_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
** The following three acq record types apply for IS683B PRL or above.
**- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The JTACS CDMA system acquisition record (standard channel)
*/
typedef struct
{
  prl_ab_type a_b;                      /* System A/B selection type */
  prl_pri_sec_type pri_sec;             /* Primary/secondary CDMA channel */
} prli_jtacs_cdma_std_type;              /* selection type */

/* The JTACS CDMA system acquisition record (customer channels)
*/
typedef struct
{
  int num_chans;                        /* Number of channels */
  word chan[32];                        /* Channels */
} prli_jtacs_cdma_cust_type;

/* The 2G BAND CDMA system acquisition record (using channels)
*/
typedef struct
{
  int num_chans;                        /* Number of channels */
  word chan[32];                        /* Channels */
} prli_cdma_2ghz_chans_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The CDMA generic acquisition record
*/
typedef struct
{
  int   num_recs;                         /* Number of records */
  word  rec[128];                         /* Packed records - 5bit band-class
                                          ** plus 11bit Channel number */
} prli_cdma_generic_type;

/* The HDR generic acquisition record
*/
typedef struct
{
  int   num_recs;                         /* Number of records */
  word  rec[128];                         /* Packed records - 5bit band-class
                                          ** plus 11bit Channel number */
} prli_hdr_generic_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The GSM generic acquisition record
*/
typedef struct
{
  int num_bands;                          /* Number of bands */
  byte band[32];                          /* Bands - 5bit band-class*/

} prli_gsm_generic_type;

/* The WCDMA generic acquisition record
*/
typedef struct
{
  prl_uarfcn_incl_e_type uarfcn_incl;     /* UARFCN include */
  byte band;                              /* Bands -  5bit band-class */
  word uarfcn;                            /* Chans - 14bit UARFCN */
} prli_wcdma_generic_rec_type;

typedef struct
{
  int num_bands;                          /* Number of bands */
  prli_wcdma_generic_rec_type rec[32];   /* WCDMA generic record */
} prli_wcdma_generic_type;

/* The GSM/WCDMA perferred acquisition record
*/
typedef struct
{
  prl_gw_pref_sys_ind_e_type pref_sys_ind;   /* Preferred system indicator */
  int num_gsm_bands;                         /* Number of GSM bands */
  int num_wcdma_bands;                       /* Number of WCDMA bands */
  byte gsm_band[32];                         /* GSM bands - 5bit band-class*/
  prli_wcdma_generic_rec_type wcdma_rec[32]; /* WCDMA generic record */
}prli_gw_pref_type;

/* PLMN based system specific fields.
*/
typedef struct
{
  sys_plmn_id_s_type        plmn_id;             /* PLMN ID use */

  word                      lac;                 /* LAC ID array*/

  /* Combined plmn_lac_incl field, plmn select mode (plmn lac not incl), and
  ** network selection mode (plmn included)
  */
  sd_net_select_mode_e_type net_select_mode;     /* network selection mode */

  byte                      num_lac;             /* Number of lac */

  word                      lac_a[16];           /* lac array */
} prli_plmn_sys_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Macros to extract the band-class and channel number out of a packed
** generic CDMA channel record.
*/
#define PRLI_CDMA_GENERIC_BAND( rec ) \
                                     ((sd_band_e_type)(((rec)>>11) & 0x001F))
#define PRLI_CDMA_GENERIC_CHAN( rec ) \
                                     ((sd_chan_type)( (rec) & 0x07FF ))

#define PRLI_CDMA_GENERIC_REC( band, chan ) \
                                     (word)( (( band & 0x001f ) << 11) +  \
                                             (chan & 0x07FF) )


/* Macros to extract the PLMN bytes from dword.
*/
#define PRLI_PLMN_HIGH( plmn ) (byte)( ((plmn) >> 16 ) & 0x000000FF )
#define PRLI_PLMN_MID( plmn )  (byte)( ((plmn) >> 8  ) & 0x000000FF )
#define PRLI_PLMN_LOW( plmn )  (byte)(  (plmn)         & 0x000000FF )
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* This structure defines the acquisition record type
*/
typedef struct
{
  prl_acq_rec_type acq_type;
  boolean          is_valid;
  union
  {
    prli_cellular_analog_type       cellular_analog;
    prli_cellular_cdma_type         cellular_cdma;
    prli_cellular_cdma_custom_type  cellular_cdma_cust;
    prli_cellular_generic_type      cellular_generic;
    prli_pcs_cdma_block_type        pcs_cdma_block;
    prli_pcs_cdma_chan_type         pcs_cdma_chan;

    /* The following three acq record types apply for IS683B PRL or above.*/
    prli_jtacs_cdma_std_type        jtacs_cdma_std;
    prli_jtacs_cdma_cust_type       jtacs_cdma_cust;
    prli_cdma_2ghz_chans_type       cdma_2ghz_chans;

    /* The following two acq record types apply for IS683C PRL or above. */
    prli_cdma_generic_type          cdma_generic;
    prli_hdr_generic_type           hdr_generic;

    /* The following two acq record types apply for IS683D PRL or above. */
    prli_gsm_generic_type           gsm_generic;
    prli_wcdma_generic_type         wcdma_generic;
    prli_gw_pref_type               gw_pref;

  } type;
  prli_plmn_sys_type plmn;          /* PLMN information from system record
                                    ** only valide for gsm/wcdma related acq
                                    ** record
                                    */

} prli_acq_rec_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD prli_acq_rec_type.type    DISC prli_acq_rec_type.acq_type */
/*~ CASE  PRL_ACQ_REC_CELLULAR_ANALOG    prli_acq_rec_type.type.cellular_analog */
/*~ CASE  PRL_ACQ_REC_CELLULAR_CDMA_STD  prli_acq_rec_type.type.cellular_cdma */
/*~ CASE  PRL_ACQ_REC_CELLULAR_CDMA_CUST prli_acq_rec_type.type.cellular_cdma_cust */
/*~ CASE  PRL_ACQ_REC_CELLULAR_GENERIC   prli_acq_rec_type.type.cellular_generic */
/*~ CASE  PRL_ACQ_REC_PCS_CDMA_BLOCKS    prli_acq_rec_type.type.pcs_cdma_block */
/*~ CASE  PRL_ACQ_REC_PCS_CDMA_CHANNELS  prli_acq_rec_type.type.pcs_cdma_chan */
/*~ CASE  PRL_ACQ_REC_JTACS_CDMA_STD     prli_acq_rec_type.type.jtacs_cdma_std */
/*~ CASE  PRL_ACQ_REC_JTACS_CDMA_CUST    prli_acq_rec_type.type.jtacs_cdma_cust */
/*~ CASE  PRL_ACQ_REC_CDMA_2GHZ_CHANS    prli_acq_rec_type.type.cdma_2ghz_chans */
/*~ CASE  PRL_ACQ_REC_CDMA_GENERIC       prli_acq_rec_type.type.cdma_generic */
/*~ CASE  PRL_ACQ_REC_HDR_GENERIC        prli_acq_rec_type.type.hdr_generic */
/*~ CASE  PRL_ACQ_REC_GSM_GENERIC        prli_acq_rec_type.type.gsm_generic */
/*~ CASE  PRL_ACQ_REC_WCDMA_GENERIC      prli_acq_rec_type.type.wcdma_generic */
/*~ CASE  PRL_ACQ_REC_GW_PREFERRED       prli_acq_rec_type.type.gw_pref */
/*~ CASE  PRL_ACQ_REC_NONE               prli_acq_rec_type.type.void */
/*~ CASE  PRL_ACQ_REC_RESERVED           prli_acq_rec_type.type.void */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* IS-2000 system specific fields.
*/
typedef struct
{
  word                sid;              /* System identification */
  prl_nid_incl_type   nid_incl;         /* NID Included */
  word                nid;              /* Network identification */
} prli_is2000_sys_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* IS-856 system specific fields.
*/
typedef struct
{
  byte                subnet_length;    /* Subnet length */
  byte                subnet[16];       /* Subnet */
} prli_is856_sys_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* This structure defines the system record type
*/
typedef struct
{
  prl_sys_rec_type      rec_type;
  prl_neg_pref_type     neg_pref;       /* Negative/preferred system */
  prl_geo_region_type   geo;            /* Geographical region indicator */
  prl_pri_type          pri;            /* Relative priority indicator */
  byte                  acq_index;      /* Acquisition record index */

  union {
    prli_is2000_sys_type  is2000;       /* IS-2000 system specific fields */
    prli_is856_sys_type   is856;        /* IS-856 system specific fields */
    prli_plmn_sys_type    plmn;         /* PLMN based system specific fields */
  } rec;

  sys_roam_status_e_type  roam_ind;       /* Roaming indicator */
  boolean               association_inc;/* Association fields included */
  byte                  association_tag;/* Association tag */
  boolean               pn_association; /* PN association */
  boolean               data_association;/* Data association */

} prli_sys_rec_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* This structure defines a roaming list entry that contains an acquisition
** record and a system record
*/
typedef struct
{
  prli_sys_rec_type sys_rec;
  prli_acq_rec_type acq_rec;
} prli_roaming_list_entry_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type to hold the information that is associated with a PRL's header.
*/
typedef struct {

  /* PRL valid indicator.
  */
  boolean       is_valid;

  /* PRL disabled indicator.
  */
  boolean       is_disabled;

  /* Indicate whether the system table contains any MCC/IMIS_11_12 records.
  ** An MCC/IMIS_11_12 record is a record in which the SID filed is set to
  ** PRL_MCC_SID_0 or PRL_MCC_SID_1
  **
  */
  boolean       is_mcc_sys_rec;

  /* PR_LIST_SIZE field.
  */
  word          pr_list_size;

  /* PR_LIST_ID field - set to PRL_DEFAULT_VER if PRL is not loaded.
  */
  word          pr_list_id;
  /* SSPR_P_REV field.
  */
  prl_sspr_p_rev_e_type     sspr_p_rev;

  /* PREF_ONLY field.
  */
  byte          pref_only;

  /* DEF_ROAM_IND field.
  */
  sys_roam_status_e_type def_roam_ind;

  /* NUM_ACQ_RECS field - the number of records in the acquisition table.
  */
  word          num_acq_recs;

  /* NUM_SUBNET_RECS field - the number of records in the common subnet-ID
  ** prefix table.
  */
  word          num_subnet_recs;

  /* Pointer to the beginning the common subnet-ID table.
  */
  byte          *subnet_tbl_ptr;

  /* NUM_SYS_RECS field - the number of records in the system table.
  */
  word          num_sys_recs;

  /* Offset (in bits) from the beginning of the PRL to the acquisition table.
  */
  word          acq_tbl_bit_offset;

  /* Offset (in bits) from the beginning of the PRL to the system table.
  */
  word          sys_tbl_bit_offset;

} prl_hdr_s_type;




/* <EJECT> */
/*===========================================================================

FUNCTION prl_buf_get_ptr

DESCRIPTION
  Return a pointer to the PRL buffer.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the to the PRL buffer.

SIDE EFFECTS
  None.

===========================================================================*/
byte          *prl_buf_get_ptr( void );




/* <EJECT> */
/*===========================================================================

FUNCTION prl_hdr_get_ptr

DESCRIPTION
  Return pointer to a structure containing the header information of the
  currently loaded PRL.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to a structure containing the header information of the currently
  loaded PRL.

SIDE EFFECTS
  None.

===========================================================================*/
extern  const prl_hdr_s_type      *prl_hdr_get_ptr( void );




/* <EJECT> */
/*===========================================================================

FUNCTION prl_hdr_nv_get_ptr

DESCRIPTION
  Return pointer to a structure containing the header information of the
  PRL that is stored in NV.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to a structure containing the header information of the PRL that is
  stored in NV.

SIDE EFFECTS
  None.

===========================================================================*/
extern  const prl_hdr_s_type      *prl_hdr_nv_get_ptr( void );




/* <EJECT> */
/*===========================================================================

FUNCTION prl_extract_acq_rec

DESCRIPTION
  Extract a specified acquisition record from the currently loaded PRL.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the acquisition record is successfully extracted. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   prl_extract_acq_rec(

        prli_acq_rec_type         *acq_rec,
            /* Pointer to where to build the acq record.
            */

        int                       index
            /* Index of the acquisition entry to extract.
            */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION prl_extract_acq_rec */
/*~ PARAM OUT acq_rec POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */




/* <EJECT> */
/*===========================================================================

FUNCTION prl_extract_sys_rec

DESCRIPTION
  Extract PRL system record from a specified memory location.

DEPENDENCIES
  None.

RETURN VALUE
  Offset (in bits) to next system record.

SIDE EFFECTS
  None.

===========================================================================*/
extern  dword                     prl_extract_sys_rec(

        byte                      *buf_ptr,
            /* Pointer to buffer containing PRL from which to extract the
            ** system record.
            */

        dword                      offset,
            /* Offset (in bits) to the beginning of system record which to
            ** extract.
            */

        prli_sys_rec_type         *sys_rec,
            /* Pointer to buffer where to deposit the extracted system
            ** record.
            */

        prl_sspr_p_rev_e_type    sspr_p_rev
            /* PRL SSPR_P_REV to indicate PRL or Extended PRL.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION prl_sys_rec_get_sid

DESCRIPTION
  Extract the SID from a PRL system record.

DEPENDENCIES
  None.

RETURN VALUE
  Void.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      prl_sys_rec_get_sid(

        const prli_sys_rec_type   *sys_rec_ptr,
            /* Pointer to a system record from which to get the SID.
            */

        sd_sid2_u_type            *sid_ptr
            /* Pointer to buffer where to extract the SID.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION prl_sys_wildcard

DESCRIPTION
  Get PRL wildcard enum type of a system record.

DEPENDENCIES
  None.

RETURN VALUE
  PRL wcard enum type.

SIDE EFFECTS
  None.

===========================================================================*/
extern  prl_wcard_e_type          prl_sys_wildcard(

        const prli_sys_rec_type   *sys_rec_ptr
            /* Pointer to a system record which to check for wildcard ID.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION prl_map_tsb58_roam_to_sd_roam

DESCRIPTION
  Map TSB-58 roaming indicator representation to the internal DB roaming
  indicator representation.

DEPENDENCIES
  None.

RETURN VALUE
  DB roaming indicator value corresponding to input TSB-58 roaming indicator
  value.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sys_roam_status_e_type    prl_map_tsb58_roam_to_sd_roam(

        sys_roam_status_e_type    roam_ind
            /* TSB roaming indicator which to convert.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION prl_map_gsm_band_to_band

DESCRIPTION
  Map gsm band to sd band.

DEPENDENCIES
  None.

RETURN VALUE
  sd band.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_band_e_type             prl_map_gsm_band_to_band(

        prl_gsm_band_e_type        gsm_band
           /* GSM band enum type.
           */
);




/* <EJECT> */
/*===========================================================================

FUNCTION prl_map_wcdma_band_to_band

DESCRIPTION
  Map wcdma band to sd band.

DEPENDENCIES
  None.

RETURN VALUE
  sd band.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_band_e_type             prl_map_wcdma_band_to_band(

        prl_wcdma_band_e_type      wcdma_band
           /* WCDMA band enum type.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION prl_map_acq_rec_to_sd_mode

DESCRIPTION
  Map acquisition record type to SD mode.

DEPENDENCIES
  None.

RETURN VALUE
  SD mode value corresponding to input PRL system record type.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_mode_e_type            prl_map_acq_rec_to_sd_mode(

        prl_acq_rec_type          acq_rec
            /* PRL acquisition record type which to convert.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION prl_validate

DESCRIPTION
  Validate a specified PRL.

  This function is expected to be called by clients such as OTASP or DIAG
  upon writing a new PRL into NV. The returned validation indication should
  be saved along with the PRL.

  System determination consults the validation indication when reading the
  PRL from NV at power-up or NAM change.

  The function return the PRL version number through the prl_ver_ptr
  parameter.

  The function update the PRL protocol revision through sspr_p_rev_ptr
  if is_full_validation is set to true and validation is succeeded with
  protocol revision other than the specified one.

    - If is_full_validation is set to true, validate with the specific PRL
      protocol revision first. If fail, try other protocl revisions and
      update protocol revision parameter.
    - If is_full_validation is set to false, only validate with the specific
      PRL protocol revision.

DEPENDENCIES
  None.

RETURN VALUE
  PRL_VALID if the roaming list is valid. Specific PRL_INVALID_XXX code
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  prl_validation_enum_type    prl_validate(

        byte                        *prl_ptr,
            /* Pointer to the PRL to validate */

        word                        *prl_ver_ptr,
            /* Pointer where to save the PRL version.*/

        prl_sspr_p_rev_e_type       *sspr_p_rev_ptr,
            /* Pointer to PRL protocol revision information. */

        boolean                     is_full_validation
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION prl_validate */
/*~ PARAM prl_ptr POINTER */
/*~ PARAM prl_ver_ptr POINTER */
/*~ PARAM sspr_p_rev_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */



/* <EJECT> */
/*===========================================================================

FUNCTION prl_commit2

DESCRIPTION
  Validate a specified PRL and commits it to NV if validation is successful.

  Wrapper function around prl_validate() that transfers the PRL locally then 
  calls the validation function.

DEPENDENCIES
  None.

RETURN VALUE
  SD error code corresponding to the error state:
  TRUE        - success
  FALSE       - PRL invalid

SIDE EFFECTS
  None.

===========================================================================*/
extern  prl_commit_status_e_type    prl_commit2(

        byte                       *prl,
            /* PRL to validate */

        dword                       size,
           /*  Size of PRL array in BYTES */

        sd_nam_e_type               curr_nam,
           /* The current NAM to write for */
        word                        *prl_ver_ptr,
            /* Where to save the PRL version.*/

        prl_sspr_p_rev_e_type       *sspr_p_rev_ptr,
            /* PRL protocol revision information. */
        
        boolean                     is_full_validation
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION prl_commit2 */
/*~ PARAM prl VARRAY LENGTH size */
/*~ PARAM curr_nam */
/*~ PARAM INOUT prl_ver_ptr POINTER */
/*~ PARAM INOUT sspr_p_rev_ptr POINTER */
/*~ PARAM is_full_validation */
#endif /* FEATURE_HTORPC_METACOMMENTS */



/* <EJECT> */
/*===========================================================================

FUNCTION prl_get_total_size

DESCRIPTION
  Get the total size of the PRL in octets.

  If a 2nd PRL is detected at the end of the 1st PRL, the total size is size
  of 1st PRL + size 2nd PRL + 2 for the 3rd CRC.

  If no PRL is detected at the end of the 1st PRL, the total size is the size
  of 1st PRL.

DEPENDENCIES
  None.

RETURN VALUE
  Total size of PRL.

SIDE EFFECTS
  None.

===========================================================================*/
extern  unsigned int              prl_get_total_size(

        byte                      *prl_ptr
            /* Pointer to PRL for which to get the total size.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION prl_init

DESCRIPTION
  Initialize the PRL component with a specified NAM.

DEPENDENCIES
  The PRL must have already been loaded into the "prl" buffer.

RETURN VALUE
  TRUE if PRL initialization is successful. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   prl_init(

        byte                      nam,
            /* NAM with which to initialize the PRL component.
            */

        word                      cdma_home_sid,
            /* CDMA home SID.
            */

        word                      cdma_home_nid,
            /* CDMA home NID.
            */

        word                      amps_home_sid
            /* AMPS home SID.
            */
);




/*===========================================================================

FUNCTION PRL_IS_EXTENDED_PRL

DESCRIPTION
  Check if current used PRL is extended PRL.

DEPENDENCIES
  None.

RETURN VALUE
  True if PRL is extended PRL.
  False otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                     prl_is_extended_prl( void );




/* <EJECT> */
/*===========================================================================

FUNCTION prl_hdr_extract

DESCRIPTION
  Extract the header of a PRL.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      prl_hdr_extract(

        prl_hdr_s_type            *prl_hdr_ptr,
            /* Pointer to PRL header where to copy extracted header
            ** information.
            */

        byte                      *prl_ptr,
            /* Pointer to PRL from which to extract the header information.
            */

        prl_sspr_p_rev_e_type     sspr_p_rev
            /* SSPR P REV information
  */
);




/*===========================================================================

FUNCTION prl_update_acq_rec

DESCRIPTION
  Update the PRL acquisition table entry with the specified acqusition record.

DEPENDENCIES
  None.

RETURN VALUE
  True if acquisition record is updated successful.
  False otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean                    prl_update_acq_rec(

       const prli_acq_rec_type    *acq_rec_ptr,
            /* The input of the acq record pointer to be updated.
            */
       int                        index
            /* The index of the acq table to be updated.
            */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION prl_update_acq_rec */
/*~ PARAM acq_rec_ptr POINTER */
#endif /* FEATURE_HTORPC_METACOMMENTS */




/*===========================================================================

FUNCTION prl_commit

DESCRIPTION
  Commit PRL in NV

DEPENDENCIES
  None.

RETURN VALUE
  True if PRL is valide.
  False if PRL is invalid

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean                    prl_commit(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION prl_commit */
#endif /* FEATURE_HTORPC_METACOMMENTS */



/* <EJECT> */
/*===========================================================================

FUNCTION prl_get_ptr

DESCRIPTION
  Return a pointer to the PRL.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the to the PRL.

SIDE EFFECTS
  None.

===========================================================================*/
extern nv_ram_roaming_list_type   *prl_get_ptr( void );




#endif /* PRL_H */

