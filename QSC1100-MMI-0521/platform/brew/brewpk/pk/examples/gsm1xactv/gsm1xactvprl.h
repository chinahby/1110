#ifndef GSM1XACTVPRL_H
#define GSM1XACTVPRL_H 1

/*======================================================================
FILE: gsm1xactvprl.h

DESCRIPTION:

     Defines interfaces between core API and module that supports
     core PRL functionality.
     
     In addition, defines PRL-related data types to support packaging
     of PRL into IS683A format.  The definitions are most taken
     from DMSS.
     
     Call gsm1xactvprl_provisionGSM1xPRLToNV to provision GSM1x PRL
     to GSM1x NV NAM based on thec currently selected PRL generation
     algorithm. 
     
              (c) COPYRIGHT 2002,2003 QUALCOMM Incorporated.
                         All Rights Reserved.
                      QUALCOMM Proprietary/GTDR
======================================================================*/

/*----------------------------------------------------------------------
 Include files 
----------------------------------------------------------------------*/
#include "AEEComdef.h"



/*----------------------------------------------------------------------
  Typedefs and constants.
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  PRL type definitions (copied from db.h).
----------------------------------------------------------------------*/
/*
** Type for DB_ROAM item
*/
typedef enum {
  DB_ROAM_OFF,
  DB_ROAM_ON,
  DB_ROAM_BLINK
} db_roam_type;
                                       
                                       
/*----------------------------------------------------------------------
  PRL type definitions (copied from prl.h).
----------------------------------------------------------------------*/          




/* Default PRL version number to be used for default PRL's. Note the a PRL
** that is read from NV might use this value as well.
*/
#define PRL_DEFAULT_VER 0

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
  #ifdef FEATURE_IS683C_PRL
  PRL_INVALID_SSPR_P_REV,      /* Invalid format revision (SSPR_P_REV) */
  #endif /* FEATURE_IS683C_PRL */
  PRL_MAX_TYPE
} prl_validation_enum_type;

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


/* TBD - for now support only 683A */
#ifndef FEATURE_IS683A_PRL
#define FEATURE_IS683A_PRL 1
#endif

#undef  FEATURE_IS683C_PRL

/*---------------------------------------------------------------------------
                             PRL SYSTEM RECORD
---------------------------------------------------------------------------*/



#ifdef FEATURE_IS683C_PRL
/* Enumeration of system record types.
*/
typedef enum
{
  PRL_SYS_REC_IS2000,    /* IS-2000S system record for CDMA and AMPS */
  PRL_SYS_REC_IS856,     /* IS-856 system record for HDR */

  PRL_SYS_REC_MAX        /* For range checking */
} prl_sys_rec_type;

#endif /* FEATURE_IS683C_PRL */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Roaming list value for a wildcard SID.
*/
#define PRL_WILDCARD_SID 0

/* Roaming list value for a wildcard NID.
*/
#define PRL_WILDCARD_NID 65535

/* Roaming List Value for a Public Network NID.
*/
#define PRL_PUBLIC_NETWORK_NID 0

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

  #ifdef FEATURE_IS683C_PRL
  PRL_ACQ_REC_CDMA_GENERIC=10,           /* CDMA Generic */
  PRL_ACQ_REC_HDR_GENERIC,               /* HDR Generic */
  #endif

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

/* The lowest and highest CDMA PCS channel numbers
*/
#define PRL_LOWEST_PCS_CHANNEL  25
#define PRL_HIGHEST_PCS_CHANNEL 1175


/*---------------------------------------------------------------------------
                           EXTERNAL PRL TYPES
---------------------------------------------------------------------------*/

#define PR_LIST_SIZE_LENGTH    16
#define PR_LIST_ID_LENGTH      16
#define PREF_ONLY_LENGTH       1
#define ROAM_IND_LENGTH        8
#define ACQ_REC_INDEX_LENGTH   9
#define SYS_REC_INDEX_LENGTH   14
#define CRC_LENGTH             16

#define SYS_REC_SID_LENGTH      15
#define SYS_REC_NID_INCL_LENGTH 2
#define SYS_REC_NID_LENGTH      16
#define SYS_REC_PREF_NEG_LENGTH 1
#define SYS_REC_GEO_LENGTH      1
#define SYS_REC_PRI_LENGTH      1


#ifdef FEATURE_IS683C_PRL

/* Length in bits of additional header fields.
*/
#define SSPR_P_REV_LENGTH       8
#define NUM_SUBNET_RECS_LENGTH  9


/* Length in bits of leading common system record fields.
*/
#define SYS_REC_LENGTH_LENGTH   5
#define SYS_REC_TYPE_LENGTH     4
#define SYS_REC_RESERVED_LENGTH 3

#define SYS_REC_COMMON_LENGTH   ( SYS_REC_LENGTH_LENGTH + \
                                  SYS_REC_TYPE_LENGTH + \
                                  SYS_REC_PREF_NEG_LENGTH + \
                                  SYS_REC_GEO_LENGTH + \
                                  SYS_REC_PRI_LENGTH + \
                                  ACQ_REC_INDEX_LENGTH + \
                                  SYS_REC_RESERVED_LENGTH )

/* Length in bits of trailing common system record fields.
*/
#define SYS_REC_COLLOCATED_SYS_LENGTH   1
#define SYS_REC_COLLOCATED_TAG_LENGTH   2


/* Length in bits of subnet specific system record fields.
*/
#define SYS_REC_SUBNET_COM_INC_LENGTH     1
#define SYS_REC_SUBNET_LSB_LENGTH         7
#define SYS_REC_SUBNET_COM_OFFSET_LENGTH  12


/* Length in bits of subnet record fields.
*/
#define COM_SUBNET_REC_RESERVED_LENGTH    4
#define COM_SUBNET_REC_LENGTH_LENGTH      4

#endif /* FEATURE_IS683C_PRL */


#define NV_ROAMING_LIST_ITEM    NV_ROAMING_LIST_683_I

#define MINIMUM_PRL_SIZE  ((sizeof(prl_fixed_roaming_list_type)+CRC_LENGTH)/8)


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Acquisition type header
*/
typedef PACKED struct
{
#ifdef FEATURE_IS683C_PRL
  byte acq_rec[8];                      /* Acquisition type */
#else
  byte acq_rec[4];                      /* Acquisition type */
#endif
} prl_acq_rec_header_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The cellular analog system acquisition record
*/
typedef PACKED struct
{
  byte a_b[2];                          /* System A/B selection */
} prl_cellular_analog_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The cellular CDMA system acquisition record
*/
typedef PACKED struct
{
  byte a_b[2];                          /* System A/B selection */
  byte pri_sec[2];                      /* Primary/secondary CDMA channel */
} prl_cellular_cdma_type;               /* selection                      */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef PACKED struct
{
  byte num_chans[5];
  byte chan[11];
} prl_cellular_cdma_custom_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The cellular generic system acquisition record
*/
typedef PACKED struct
{
  byte a_b[2];                          /* System A/B selection */
} prl_cellular_generic_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The PCS CDMA system acquisition (using blocks) record
*/
typedef PACKED struct
{
  byte num_blocks[3];                   /* Number of frequency blocks */
  byte block[3];                        /* PCS frequency block */
} prl_pcs_cdma_using_blocks_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The PCS CDMA system acquisition (using channels) record
*/
typedef PACKED struct
{
  byte num_chans[5];                    /* Number of channels */
  byte chan[11];                        /* PCS CDMA channel */
} prl_pcs_cdma_using_chans_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_IS683C_PRL

/* The CDMA generic acquisition record
*/
typedef PACKED struct
{
  byte length[8];                       /* Total length of record */
  byte rec[16];                         /* 16 bit record - 5bit band-class +
                                        ** 11bit channel number */
} prl_cdma_generic_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The HDR generic acquisition record
*/
typedef PACKED struct
{
  byte length[8];                       /* Total length of record */
  byte rec[16];                         /* 16 bit record - 5bit band-class +
                                        ** 11bit channel number */
} prl_hdr_generic_type;

#endif /* FEATURE_IS683C_PRL */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The acquisition record format
*/
typedef PACKED struct
{
  prl_acq_rec_header_type             acq_rec_header;
  PACKED union
  {
    prl_cellular_analog_type          cellular_analog;
    prl_cellular_cdma_type            cellular_cdma;
    prl_cellular_generic_type         cellular_generic;
    prl_pcs_cdma_using_blocks_type    pcs_cdma_using_blocks;
    prl_pcs_cdma_using_chans_type     pcs_cdma_using_channels;

    #ifdef FEATURE_IS683C_PRL
    prl_cdma_generic_type             cdma_generic;
    prl_hdr_generic_type              hdr_generic;
    #endif

  } acq_rec;
} prl_acq_record_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The system record format (IS-683A)
*/
typedef PACKED struct
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

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The fixed portion of the preferred roaming list
*/
typedef PACKED struct
{
  byte pr_list_size[PR_LIST_SIZE_LENGTH];       /* PR_LIST_SIZE */
  byte pr_list_id[PR_LIST_ID_LENGTH];           /* PR_LIST_ID */

  #ifdef FEATURE_IS683C_PRL
  byte sspr_p_rev[SSPR_P_REV_LENGTH];
  #endif

  byte pref_only[PREF_ONLY_LENGTH];             /* PREF_ONLY */
  byte def_roam_ind[ROAM_IND_LENGTH];           /* DEF_ROAM_IND */
  byte num_acq_recs[ACQ_REC_INDEX_LENGTH];      /* NUM_ACQ_RECS */

  #ifdef FEATURE_IS683C_PRL
  byte num_subnet_recs[NUM_SUBNET_RECS_LENGTH]; /* NUM_SUBNET_RECS */
  #endif

  byte num_sys_recs[SYS_REC_INDEX_LENGTH];      /* NUM_SYS_RECS */

  #ifdef FEATURE_IS683C_PRL
  byte reserved[7];                             /* Reserved bits */
  #endif

} prl_fixed_roaming_list_type;


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
  uint16 chan[32];                        /* Channels */
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
  uint16 block[7];                        /* Blocks */
} prli_pcs_cdma_block_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The PCS CDMA system acquisition (using channels) record
*/
typedef struct
{
  int num_chans;                        /* Number of channels */
  uint16 chan[32];                        /* Channels */
} prli_pcs_cdma_chan_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_IS683C_PRL
/* The CDMA generic acquisition record
*/
typedef struct
{
  int   num_recs;                         /* Number of records */
  uint16  rec[128];                         /* Packed records - 5bit band-class
                                          ** plus 11bit Channel number */
} prli_cdma_generic_type;

/* The HDR generic acquisition record
*/
typedef struct
{
  int   num_recs;                         /* Number of records */
  uint16  rec[128];                         /* Packed records - 5bit band-class
                                          ** plus 11bit Channel number */
} prli_hdr_generic_type;


/* Macros to extract the band-class and channel number out of a packed
** generic CDMA channel record.
*/
#define PRLI_CDMA_GENERIC_BAND( rec )    ( ((rec) >> 11) & 0x001F )
#define PRLI_CDMA_GENERIC_CHAN( rec )    ( (rec) & 0x07FF )

#endif /* FEATURE_IS683C_PRL */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* This structure defines the acquisition record type
*/
typedef struct
{
  prl_acq_rec_type acq_type;
  union
  {
    prli_cellular_analog_type       cellular_analog;
    prli_cellular_cdma_type         cellular_cdma;
    prli_cellular_cdma_custom_type  cellular_cdma_cust;
    prli_cellular_generic_type      cellular_generic;
    prli_pcs_cdma_block_type        pcs_cdma_block;
    prli_pcs_cdma_chan_type         pcs_cdma_chan;

    #ifdef FEATURE_IS683C_PRL
    prli_cdma_generic_type          cdma_generic;
    prli_hdr_generic_type           hdr_generic;
    #endif

  } type;
} prli_acq_rec_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_IS683C_PRL

/* IS-2000 system specific fields.
*/
typedef struct
{
  uint16                sid;              /* System identification */
  prl_nid_incl_type   nid_incl;         /* NID Included */
  uint16                nid;              /* Network identification */
} prli_is2000_sys_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* IS-856 system specific fields.
*/
typedef struct
{
  byte             common_incl;      /* Subnet common included */
  uint16                common_offset;    /* Subnet common offset */
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
  } rec;

  db_roam_type          roam_ind;       /* Roaming indicator */
  byte               colloc_sys;     /* Collocated tag included */
  byte                  colloc_tag;     /* Collocated tag */
} prli_sys_rec_type;

#else /* FEATURE_IS683C_PRL */

/* This structure defines the system record type
*/
typedef struct
{
  uint16 sid;                             /* System identification */
  prl_nid_incl_type nid_incl;           /* NID Included */
  uint16 nid;                             /* Network identification */
  prl_neg_pref_type neg_pref;           /* Negative/preferred system */
  prl_geo_region_type geo;              /* Geographical region indicator */
  prl_pri_type pri;                     /* Relative priority indicator */
  byte acq_index;                       /* Acquisition record index */
  db_roam_type roam_ind;                /* Roaming indicator */
} prli_sys_rec_type;

#endif /* FEATURE_IS683C_PRL */

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
  byte       is_valid;

  /* PRL disabled indicator.
  */
  byte       is_disabled;

  /* PR_LIST_SIZE field.
  */
  uint16          pr_list_size;

  /* PR_LIST_ID field - set to PRL_DEFAULT_VER if PRL is not loaded.
  */
  uint16          pr_list_id;

  #ifdef FEATURE_IS683C_PRL
  /* SSPR_P_REV field.
  */
  byte          sspr_p_rev;
  #endif /* FEATURE_IS683C_PRL */

  /* PREF_ONLY field.
  */
  byte          pref_only;

  /* DEF_ROAM_IND field.
  */
  db_roam_type  def_roam_ind;

  /* NUM_ACQ_RECS field.
  */
  uint16          num_acq_recs;

  #ifdef FEATURE_IS683C_PRL
  /* NUM_SUBNET_RECS field.
  */
  uint16          num_subnet_recs;

  /* Offset (in bits) from the beginning of the PRL to the common subnet
  ** table.
  */
  uint16          subnet_tbl_bit_offset;
  #endif /* FEATURE_IS683C_PRL */

  /* NUM_SYS_RECS field.
  */
  uint16          num_sys_recs;

  /* Offset (in bits) from the beginning of the PRL to the acquisition table.
  */
  uint16          acq_tbl_bit_offset;

  /* Offset (in bits) from the beginning of the PRL to the system table.
  */
  uint16          sys_tbl_bit_offset;

} prl_hdr_s_type;

/*----------------------------------------------------------------------
  PRL type definitions (copied from nv.h).
----------------------------------------------------------------------*/

#define NV_ROAMING_LIST_MAX_SIZE  (3072)
#define NV_RAM_PRL_BUF_BYTES      (6)     
                                    /* buffer bytes to account for uneven
                                     * bit in PRL system table, ni_pr area,
                                     * ni2bit area, and word alignment in
                                     * geo_sid area.
                                     */


typedef PACKED struct {
  byte      nam;                    /* NAM id 0-N */
  uint16      prl_version;
  uint16      size;
  byte   valid;
  byte      roaming_list[NV_ROAMING_LIST_MAX_SIZE + NV_RAM_PRL_BUF_BYTES];
} nv_ram_roaming_list_type;





#define GSM1XACTVPRL_SEL_PLMN_MAX_LEN_BYTES          (96)
#define GSM1XACTVPRL_FORBIDDEN_PLMN_MAX_LEN_BYTES    (12)
#define GSM1XACTVPRL_HPLMN_LEN_BYTES                 (3)


/* TBD Adjust this based on the number of supported
   systems, see standard 683.*/
#define GSM1XACTVPRL_ACQ_REC_MAX_CNT                 (10)

#define GSM1XACTVPRL_SYS_REC_MAX_CNT                 (150)


typedef struct
{
   uint16       SID;
   uint16       NID;
   boolean      isPreferred;
} gsm1xactvprl_prlSIDNIDType;

  


/* This variables are used to access externally allocated
   variables. */
typedef struct 
{

    nv_ram_roaming_list_type         NewPRL;
    gsm1xactvprl_prlSIDNIDType      SIDNIDPair[GSM1XACTVPRL_SYS_REC_MAX_CNT];
    prli_acq_rec_type                AcqRecords[GSM1XACTVPRL_ACQ_REC_MAX_CNT];
    prl_hdr_s_type                   Header;
    AEEGSM1xControl_PLMNTripletType  PLMNEntries[
                                               ((GSM1XACTVPRL_SEL_PLMN_MAX_LEN_BYTES)/3)
                                             + ((GSM1XACTVPRL_FORBIDDEN_PLMN_MAX_LEN_BYTES)/3)
                                             + ((GSM1XACTVPRL_HPLMN_LEN_BYTES)/3)
                                               ];
    prli_sys_rec_type                SysRecord;
    uint16                           HomeSID;
    uint16                           HomeNID;
    /*uint16                           CRC16Table[ CRC_TAB_SIZE ] */

} gsm1xactvprl_LargeDataBuffers;

/*----------------------------------------------------------------------
 Global Constant Declarations
----------------------------------------------------------------------*/
/* None.*/

/*----------------------------------------------------------------------
 Global Data Declarations
----------------------------------------------------------------------*/
/* None.*/

/*----------------------------------------------------------------------
 Function Prototypes
----------------------------------------------------------------------*/
/* Generate (based on the current PRL Generation Mode)
   and Provision GSM1x PRL to NV.*/
boolean gsm1xactvprl_provisionGSM1xPRLToNV
(
   IGSM1xControl                    *instancePtr,
   gsm1xactvprl_LargeDataBuffers    *largeDataBuf
);

#endif /* GSM1XACTVPRL_H */
