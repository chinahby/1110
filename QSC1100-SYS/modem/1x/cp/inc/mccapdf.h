#ifndef MCCAPDF_H
#define MCCAPDF_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    C D M A   M A I N   C O N T R O L
               C A P A B I L I T Y   D Y N A M I C   F E A T U R E

GENERAL DESCRIPTION
  This file contains the MC Dynamic Feature functions. The Dynamic Feature
  is to control the feature capability via the NV item at run-time, rather
  than the feature flags during compile time. The Dynamic Feature includes
  optional features in Release 0 and A. When performing the capability
  reporting and service negotiation, it should consult the Dynamic Feature
  capability in this module, rather than rely on the feature flags.
  
  The Dynamic Feature item can be programmed using service programming
  tools. A verification routine is provided to ensure the Dynamic Feature
  item is programmed correctly.
  
  Upon power up, MC reads and verifies the Dynamic Feature item. If
  verifications fails, the mobile goes offline/FTM mode, prompting for
  service programming.
  
Copyright (c) 2003-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/08/07   pg      Featurization and general code cleanup for memory reduction
                   effort.
                   Re-featurized FEATURE_IS2000_DYNAMIC_FEATURE.
08/09/06   fh      AKA/AES: renamed MCCAP_DF_AES_UI to MCCAP_DF_AES_UI_VOICE
08/04/06   fh      AKA/AES: changed AES related DF items
07/13/06   fh      Added AES DF support under AES feature
05/12/05   an      Changed function attributes to PACKED
03/25/05   an      Fixed lint errors.

===========================================================================*/
#ifdef FEATURE_IS2000_DYNAMIC_FEATURE
#include "nv.h"
#endif /* FEATURE_IS2000_DYNAMIC_FEATURE */

/* DF item type */

typedef enum
{
  MCCAP_DF_P_REV = 0,
  MCCAP_DF_QPCH, 
  MCCAP_DF_SLOTTED_TIMER,
  MCCAP_DF_CHS,
  MCCAP_DF_SDB,
  MCCAP_DF_CS,
  MCCAP_DF_OTD,
  MCCAP_DF_STS,
  MCCAP_DF_DCCH,
  MCCAP_DF_FOR_SCH,
  MCCAP_DF_FOR_MAX_RATE,
  MCCAP_DF_REV_SCH,
  MCCAP_DF_REV_MAX_RATE,
  MCCAP_DF_MUXPDU5,
  MCCAP_DF_REV_FCH_GATING,
  MCCAP_DF_SYNC_ID,
  MCCAP_DF_RECONNECT_MSG,
  MCCAP_DF_NEG_SCI,
  MCCAP_DF_AEHO,
  MCCAP_DF_APHO,
  MCCAP_DF_AES_SIG,
  MCCAP_DF_AES_UI_VOICE,
  MCCAP_DF_MAX
} mccap_df_type;

/* DF item range */

typedef struct
{
  uint8                    df_min;                     /* Minimum Range of DF item */
  uint8                    df_max;                     /* Maximum Range of DF item */
} mccap_df_range;

#define MAX_DF_STR_LEN                           15    /* MAX Length of DF String */

#ifdef FEATURE_IS2000_DYNAMIC_FEATURE
/* DF item structure */

typedef struct {
  char                     df_str_name[MAX_DF_STR_LEN];/* DF String */
  mccap_df_type            df_type;                    /* DF type */  
  boolean                  df_status;                  /* Flag indicating whether the DF item is supported */
  uint8                    df_bits;                    /* Number of bits in NV, used by the DF item */ 
  uint8                    df_min_p_rev;               /* Minimum P_REV supported by the DF item */  
  mccap_df_range           df_range;                   /* Min and Max value of the DF item */
} mccap_df_item_type;


/* This should defined in nv.h, for reserved bits */
#define MC_DYNAMIC_FEATURE_ITEM_BYTE    20


/*===========================================================================

FUNCTION MCCAP_DF_SET_DEFAULT

  This function set the default value for the Dynamic Feature related item
  based on the feature flag.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  mccap_df_init() has to be called before calling this function.
  
===========================================================================*/
void mccap_df_set_default(void);
/*===========================================================================

FUNCTION MCCAP_DF_PACK

  This function xlates the Dynamic Feature item from internal to external
  format.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
boolean mccap_df_pack
(
  uint16                length,      /* The length of the buffer */
  uint8                 *buf_ptr     /* The pointer to the buffer */
);

/* <EJECT> */
/*===========================================================================

FUNCTION MCCAP_DF_UNPACK

  This function xlates the Dynamic Feature item from external to internal
  format.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if successful.
  FALSE if failed due to invalid length.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
boolean mccap_df_unpack
(
  uint16                length,     /* The length of buffer */
  uint8                 *buf_ptr    /* The pointer to buffer */
);



/* <EJECT> */
/*===========================================================================

FUNCTION MCCAP_DF_NV_READ
  This function reads the Dynamic Feature item from NV.

DEPENDENCIES
  NV task has to be started.
  This is a blocking function, waiting for NV response before return.

RETURN VALUE
  Status returned by nv_read procedure.

SIDE EFFECTS
  This is a blocking function, waiting for NV response before return.

===========================================================================*/
nv_stat_enum_type mccap_df_nv_read
(
  nv_cmd_type *cmd_ptr
    /* Pointer to command to send to NV */
);



/* <EJECT> */
/*===========================================================================

FUNCTION MCCAP_DF_NV_WRITE
  This function writes the Dynamic Feature item to NV.

DEPENDENCIES
  NV task has to be started.
  This is a blocking function, waiting for NV response before return.

RETURN VALUE
  Status returned by nv_write procedure.

SIDE EFFECTS
  This is a blocking function, waiting for NV response before return.

===========================================================================*/
nv_stat_enum_type mccap_df_nv_write
(
  nv_cmd_type *cmd_ptr
    /* Pointer to command to send to NV */
);


/*===========================================================================

FUNCTION MCCAP_DF_DISCOVERY

  This function discovers the DF items which are supported by the MS.

DEPENDENCIES
  None.

RETURN VALUE
  Status of DF items and their values in the NV.

SIDE EFFECTS
  None
  
===========================================================================*/
boolean mccap_df_discovery(void);


/*===========================================================================

FUNCTION MCCAP_DF_QUERY

  This function is called by the DIAG task to query to DF items. 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the DF items were successfully read.

SIDE EFFECTS
  This function should be called only from the DIAG task context.
  
===========================================================================*/

boolean mccap_df_query (PACKED char *df_str);

/*===========================================================================

FUNCTION MCCAP_DF_PRINT

  This function prints out the DF fields.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void mccap_df_print(void);


/*===========================================================================

FUNCTION MCCAP_DF_DISCOVERY_VERIFY

  The function checks for dependencies between the DF items which the user may
  have violated and sets the correct value accordingly.
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  mccap_df_init() has to be called before calling this function.
  
===========================================================================*/

boolean mccap_df_discovery_verify(void);

/*===========================================================================

FUNCTION MCCAP_DF_CREATE_DISCOVERY_STRING

  This function updates the dynamic feature character string with the information
  contained in the df_ptr. The first byte in the character string is the number of
  DF items. The first byte in the DF string, is the number of DF items.
  The format of the string to be sent to the diag is as follows.
  
  Bytes       Content           Description                      Example
  ----------------------------------------------------------------------------------
  1            Item No.          Item No.                          1
  Len          DF String         The actual DF String              "QPCH"
  1            Status            Flag, whether DF is active        1 
  1            Min               Min Value                         0
  1            Max               Max Value                         1 
  1            Value             Value either a boolean or a byte   TRUE

DEPENDENCIES
  None.

RETURN VALUE
  
  
SIDE EFFECTS
  None.
  
===========================================================================*/
void mccap_df_create_discovery_string(PACKED char *df_str);

/*===========================================================================

FUNCTION MCCAP_DF_INIT

  This function reads the dynamic feature item from NV upon power-up and
  verifies it. If the NV item is inactive, it sets the default value and 
  writes back to NV. If verification fails but use default value is
  preferred, it will do so and writes back to NV. It then updates the 
  system capability with the dynamic feature fields.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This is a blocking function, waiting for NV response before return.
  system_capability variable will be updated.
  
===========================================================================*/
void mccap_df_init ( void );

/* <EJECT> */

/*===========================================================================

FUNCTION MCCAP_DF_LOAD

  This function loads the dynamic feature item from NV and verifies it. 
  If verification fails, it will return FALSE to prompt user for service
  programming.
  
DEPENDENCIES
  None.

RETURN VALUE
  If the NV item is active and the verification passes, it returns TRUE.
  Otherwise it returns FALSE to prompt user for service programming. 

SIDE EFFECTS
  mccap_df_init() has to be called before calling this function.
  This is a blocking function, waiting for NV response before return.
  
===========================================================================*/
boolean mccap_df_load ( void );

/*===========================================================================

FUNCTION MCCAP_DF_VERIFY
  This function verifies a DF item for dependency problems.
DEPENDENCIES
  None.

RETURN VALUE
  A string of DF items, which need to be changed 
SIDE EFFECTS
  None
  
===========================================================================*/
boolean mccap_df_verify (PACKED char *req_str, PACKED char *rsp_str);

/*===========================================================================

FUNCTION MCCAP_DF_WRITE
  This function writes the DF items to the NV
DEPENDENCIES
  None.

RETURN VALUE
  Status whether the write was a SUCCESS or FAILURE
SIDE EFFECTS
  None
  
===========================================================================*/

boolean mccap_df_write (PACKED char *req_str);

#endif /* FEATURE_IS2000_DYNAMIC_FEATURE */
#endif /* MCCAPDF_H */
