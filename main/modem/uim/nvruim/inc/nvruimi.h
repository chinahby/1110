#ifndef NVRUIMI_H
#define NVRUIMI_H
/*==========================================================================

         N V    R - U I M    I N T E R N A L    H E A D E R    F I L E

DESCRIPTION
  This header file contains all the internal interface definitions for
  the R-UIM subsystem of the NV task.

REFERENCES
  IS-820, Removable User Identity Module (RUIM) for Spread Spectrum Systems

Copyright (c) 2000, 2001, 2002-2008, 2009, 2010 by QUALCOMM Incorporated.  
All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/nvruimi.h_v   1.15   06 Sep 2002 10:04:06   tsummers  $
$Header: //source/qcom/qct/modem/uim/nvruim/rel/07H1_2/inc/nvruimi.h#8 $ $DateTime: 2010/07/15 00:05:49 $ $Author: sratnu $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/29/10   ssr     Fixed FEATURE_UIM_MISCONFIG_RUIM_N5_WORKAROUND_OPTIMIZED
05/21/10   ssr     Add non 3gpd cdma card check
04/30/10   ssr     Fixed nvruim clear cache 
04/23/10   ssr     Fixed UIM 3GPD NV support
03/25/10   ssr     Added nvruim_data_3gpd_ext_support
07/27/09   yb      Fixed bad RUIM card issue with HRPD Service n5 in CDMA 
                   Service Table improperly set
07/02/09   ssr     Operator MCC Check for 3GPD
06/20/08   ssr     Support of OMH LBS Items
04/25/08   ssr     Support of OMH Data Items
03/26/08   jk      OMH Phase 1B Support - Multiple SIP Profiles
01/17/08   nk      Added CDMA Refresh Support
10/10/07   jk      Changes to support Open Market Handset
10/08/07   vs/nk   Removed inclusion of header file "uiuint.h"
08/16/07   jk      Rearranged featuization to support DATA Auth on 7K
08/03/07   nk      Added usage indicator for EUIMID support
07/10/07   nk      Added EUIMID support
05/11/06   wli	   Extern nvruim_decode_min() for perso
01/25/06   jk      Added uiuint.h include file for old targets which require
                   this include in nvimr.c
11/22/05   jk      Added support for multiprocessor RPC
11/22/04   jk      Finished adding support for 3GPD.
09/01/04   jk      Added support for HRPD
12/19/03   jk      Added interface for R-UIM 3GPD Services.
09/25/03   tml     Featurized uiuint.h
11/25/02   ts      Expose the directories present variable for the UIM power
                   up/wake up procedures to set accordingly.
06/19/02   ts      Added function prototype to process RTRE configuration.
03/28/02   wli     Added NAM_LOCK mask for IS820 Addendum 1 changes.
02/13/02   wli     Added a function to indicate if some DFs exist.
12/13/01   wli     Added funciton to flush SMS cache.
07/23/01   ck      Added zeroing of ESN as an option in the esn usage enum.
05/18/01   ck      Modified the NVRUIM module to interface with the UIM server.
03/07/01   wli     Defined constants.
03/06/01   wli     Support more NV items for registration indicators.
02/22/01   ck      Added definitions to handle accesses to the items OTAPA 
                   enable and SPC enable.
02/07/01   wli     Deleted invalidate and rehabilitate interface functions.
1/28/01    wli     Added phone book support which includes speed dial(NV_DIAL_I) 
                   and call history (NV_STACK_I).
                   Added a new interface function that enable user to invalidate
                   or rehabilitate an EF.
12/11/01   ts      Added nvruim_flush_cache to be accessed when the RUIM is 
                   reset.
11/15/00   ck      Added the enum definitions for esn usage indicator.
04/24/00   jah     Initial Revision

===========================================================================*/


/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "nv.h"
#include "uim.h"  


/*===========================================================================
 
                           DATA DECLARATIONS

===========================================================================*/

/*=========================================================================*/
/* Status as to whether or not a particular NV item is supported by the    */
/* R-UIM subsystem.                                                        */

typedef enum {
  NV_RUIM_ITEM_NOT_SUPPORTED=0,         /* item is supported by R-UIM      */
  NV_RUIM_SUPPORTS_ITEM=1               /* item is not supported by R-UIM  */
} nv_ruim_support_status;

typedef enum {
  NV_RUIM_USE_ESN = 0,                  /* Use ESN as ESN */
  NV_RUIM_USE_RUIM_ID = 1,              /* Use R-UIM ID as ESN */
  NV_RUIM_ZERO_ESN = 2,                 /* Zero the ESN */
  NV_RUIM_USE_MEID = 3,                 /* Use MEID as MEID */
  NV_RUIM_USE_SF_EUIMID = 4,            /* Use SF RUIM ID as MEID */  
  NV_RUIM_USE_SF_EUIMID_ESN = 5,        /* Use SF EUIMID for identification and 
                                           Use ESN for authentication*/ 
  NV_RUIM_USE_MEID_UIMID = 6            /* Use MEID for identification and use
                                           UIMID for authentication */  
} nvruim_esn_usage_type;

/* IMSI EF structure for IMSI_M and IMSI_T accesses                        */
typedef PACKED struct {
  byte imsi_class;
  byte imsi_s2[2];
  byte imsi_s1[3];
  byte imsi_11_12;
  byte imsi_addr_num;
  byte imsi_mcc[2];
} nvruim_imsi_type;

/* Analog Channel Preferences EF structure (EF 6F2F) */
typedef PACKED struct {
  byte analog_firstchp[2];
  byte analog_first_dcc_a[2];
  byte analog_first_dcc_b[2];
  byte analog_num_of_dcc;
} nvruim_analog_ch_pref;

/* TMSI EF structure */
typedef PACKED struct {
  byte length;
  byte zone[8];
  byte code[4];
  byte time[3];
}nvruim_tmsi_type;

/* Analog Location and Registration Indicators EF structure */
typedef PACKED struct {
  byte nxtreg[3];
  byte sid[2];
  byte locaid_pureg[2];
}nvruim_analog_reg_ind_type;

/* CDMA Zone-Based Registration Indicators */
typedef PACKED struct{
  byte reg_zone[2];
  byte sid[2];
  byte nid[2];
  byte freq_block;
  byte band_class;
}nvruim_zone_reg_ind_type;

/* CDMA System/Network Registration Indicators */
typedef PACKED struct{
  byte sid[2];
  byte nid[2];
  byte freq_block;
  byte band_class;
}nvruim_sys_reg_ind_type;

/* CDMA Distance-Based Registration Indicators */
typedef PACKED struct{
  byte base_lat_reg[3];
  byte base_long_reg[3];
  byte reg_dist_reg[2];
}nvruim_dist_reg_ind_type;

#ifdef FEATURE_UIM_SUPPORT_3GPD

/* Typedefs to represent the number of bits for each field in a 3GPD EF */
typedef PACKED struct {
  byte length_of_block_field[8];
  byte num_nai_field[4];
  byte profile_num_field[4];
  byte nai_length_field[8];
  byte auth_algo_field[4];
}nvruim_sipupp_ef_fields;

typedef PACKED struct {
  byte length_of_block_field[8];
  byte retry_info_incl_field;
  byte max_num_retry_field[2];
  byte retry_timeout_field[3];
  byte rereg_threshold_field[6];
  byte num_nai_field[4];
  byte nai_entry_index_field[4];
  byte nai_length_field[8];
  byte tunnel_bit_field;
  byte home_addr_field[32];
  byte primary_ha_field[32];
  byte secondary_ha_field[32];
  byte mn_aaa_auth_algo_field[4];
  byte mn_aaa_spi_indicator_field;
  byte mn_aaa_spi_field[32];
  byte mn_ha_auth_algo_field[4];
  byte mn_ha_spi_indicator_field;
  byte mn_ha_spi_field[32];
}nvruim_mipupp_ef_fields;

typedef PACKED struct {
  byte length_of_block_field[8];
  byte num_nai_field[4];
  byte profile_num_field[4];
  byte ss_length_field[5];
}nvruim_sippapss_ef_fields;

#ifdef FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES
typedef PACKED struct {
  byte num_nai_field[4];
  byte profile_num_field[4];
  byte applications_field[32];
  byte priority_field[8];
  byte data_rate_mode_field[4];
  byte data_bearer_field[4];
}nvruim_sipuppext_ef_fields;
#endif /* FEATURE_UIM_3GPD_MULTIPLE_SIP_PROFILES */
#endif /* FEATURE_UIM_SUPPORT_3GPD */

#ifdef FEATURE_RUIM_CDMA_REFRESH
#define NVRUIM_MAX_NV_ITEM 4
#define NVRUIM_MAX_REFRESH_FILES 28
/* This value must be greater than NVRUIM_MAX_REFRESH_FILES */
#define NVRUIM_CLEAR_ALL_CACHE   NVRUIM_MAX_REFRESH_FILES + 1 
                                         

typedef struct {
  nv_items_enum_type nv_items[NVRUIM_MAX_NV_ITEM];
  uint8              num_nv_items;
}nvruim_nv_conversion;
#endif /* FEATURE_RUIM_CDMA_REFRESH */

/* Types of return values for nv 3gpd check from Data Services */
typedef enum {
  NVRUIM_NO_CARD=0,                 /* NO Card  */
  NVRUIM_NON_3GPD_CARD=1,           /* 3GPD disabled card  */
  NVRUIM_3GPD_CARD=2                /* 3GPD enabled card   */
} nvruim_3gpd_nv_card_status;

/* Mask of bit indicating the IMSI was programmed */
#define NVRUIM_IMSI_PROGRAMMED  0x80

/* Mask of bits for the IMSI Addr Num */
#define NVRUIM_IMSI_ADDR_NUM    0x07

/* Mask of the OTAPA_enable but in OTAPA/SPC_enable EF */
#define NVRUIM_OTAPA_ENABLE_MASK 0xFE

/* OTAPA disabled by setting last bit to 1 */
#define NVRUIM_OTAPA_DISABLE    0x01

/* Mask of the SPC_enable but in OTAPA/SPC_enable EF */
#define NVRUIM_SPC_ENABLE_MASK  0xEF

/* SPC is disabled by setting the bit to 1 */
#define NVRUIM_SPC_DISABLE      0x10

/* Mask of the NAM_LOCK bit in NAM_LOCK EF */
#define NVRUIM_NAM_LOCK_MASK  0xFD

/* SPC is disabled by setting the bit to 1 */
#define NVRUIM_NAM_LOCK       0x02

/* Mask for Analog/Digital Selection */
#define NVRUIM_PREF_MODE_MASK   0x70

/* Mask for A/B Selection */
#define NVRUIM_SYSTEM_PREF_MASK 0x07

/* Mask for band class */
#define NVRUIM_BAND_CLASS_MASK 0xF8

/* Service Preferences bits */
#define NVRUIM_ANALOG_PREF   0x10
#define NVRUIM_CDMA_PREF     0x20
#define NVRUIM_ANALOG_ONLY   0x50
#define NVRUIM_CDMA_ONLY     0x60
#define NVRUIM_A_PREF        0x01
#define NVRUIM_B_PREF        0x02
#define NVRUIM_A_ONLY        0x05
#define NVRUIM_B_ONLY        0x06
#define NVRUIM_NO_PREF       0x00

/* The following present the Files found in the card */
#define NVRUIM_NO_DIR_PRESENT          0x00
#define NVRUIM_MF_PRESENT              0x80
#define NVRUIM_CDMA_DF_PRESENT         0x01
#define NVRUIM_GSM_DF_PRESENT          0x02
#define NVRUIM_DCS1800_DF_PRESENT      0x04
extern byte nvruim_dir_present;

/*=========================================================================*/
/* R-UIM data sizes which do not match NV data types                       */

#define NVRUIM_MIN1_SIZE        3       /* MIN1, True IMSI S1              */
#define NVRUIM_MIN2_SIZE        2       /* MIN2, True IMSI S2              */

#define NVRUIM_MSISDN_DN_SIZE   8       /* MSISDN MDN size                 */

#define NVRUIM_SMSS_SIZE        5       /* SMS Status                      */

#define NVRUIM_MSISDN_SIZE      11      /* MSISDN total size               */

#define NVRUIM_MSISDN_ND_INDEX   0      /* Number of digits byte offset from 
                                           beginning  of MSISDN    */

#define NVRUIM_MSISDN_MDN_INDEX  1      /* MDN byte offset from beginning of 
                                           MSISDN    */

/* RUIM access is limited to 255 byte maximum */
#define MAX_BYTES_PER_ACCESS 255

/* HOME SID NID PAIR record size */
#define SID_NID_REC_SIZE 5

/* CDMA Servie Table (EF '6F32') size */
#define  NVRUIM_CDMA_SVC_TABLE_SIZE 5

/* Max buffer size for phonebook and sms EFs */ 
#define NVRUIM_EF_BUF_SIZE 255    

/* Number of bytes after Alpha Identifier in phonebook EFs */
#define PHONEBOOK_TAIL_LENGTH 14

/* Dialing numbers */
#define PHONEBOOK_DN 20

/* Bits for ADN avalability from CDMA service table */
#define CDMA_SVC_TABLE_ADN_BITS  0x0C

/* Bits for ADN avalability from CDMA service table */
#define CDMA_SVC_TABLE_LND_BITS  0x03

/* Buffer for phonebook and sms */
extern byte nvruim_ef_buffer[NVRUIM_EF_BUF_SIZE];

/* Number of bytes in the array to hold digits from converted MIN */
#define NVRUIM_NUM_MIN_DIGITS 15

/* Non 3GPD cdma card   */
#define NVRUIM_NON_3GPD_CDMA_CARD_MCC  0x167


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/



/*===========================================================================

FUNCTION NVRUIM_WRITE

DESCRIPTION
  Check to see if the item is supported by the R-UIM.
  If it is supported, write it.

DEPENDENCIES
  None

RETURN VALUE
  NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
  NV_RUIM_SUPPORTS_ITEM         Item is supported, see op_status

  op_status indicates the success/failure of the write.  For items
  which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
  None

===========================================================================*/
extern nv_ruim_support_status nvruim_write
(
  nv_cmd_type  *nv_cmd_ptr,       /* command block               */
  nv_stat_enum_type *op_status    /* status of the I/O operation */
);


/*===========================================================================

FUNCTION NVRUIM_READ

DESCRIPTION
  Check to see if the item is supported by the R-UIM.
  If it is supported, read it.

DEPENDENCIES
  None

RETURN VALUE
  NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
  NV_RUIM_SUPPORTS_ITEM         Item is supported, see op_status

  op_status indicates the success/failure of the read.  For items
  which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
  None

===========================================================================*/
extern nv_ruim_support_status nvruim_read
(
  nv_cmd_type  *nv_cmd_ptr,       /* command block               */
  nv_stat_enum_type *op_status    /* status of the I/O operation */
);


/*===========================================================================

FUNCTION NVRUIM_FREE

DESCRIPTION
  Check to see if the item is supported by the R-UIM.
  If it is supported, free it.

DEPENDENCIES
  Non-reentrant

RETURN VALUE
  NV_RUIM_ITEM_NOT_SUPPORTED    Item is not supported
  NV_RUIM_SUPPORTS_ITEM         Item is supported, see op_status

  op_status indicates the success/failure of the free.  For items
  which are "not supported", the value of op_status is undefined.

SIDE EFFECTS
  None

===========================================================================*/
extern nv_ruim_support_status nvruim_free
(
  nv_cmd_type  *nv_cmd_ptr,       /* command block               */
  nv_stat_enum_type *op_status    /* status of the I/O operation */
);


/*===========================================================================

FUNCTION NVRUIM_ACCESS                                  INTERNAL FUNCTION

DESCRIPTION
  Access the R-UIM

DEPENDENCIES
  Non-reentrant, only works for NAM=0

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern uim_rpt_status nvruim_access
(
  uim_cmd_type *cmd_ptr       /* R-UIM command                               */
);


/*===========================================================================

FUNCTION NVRUIM_SET_ESN_USAGE

DESCRIPTION
   This function sets the usage indicator that is used by the NV-RUIM module
   to decide whether ESN or R-UIM ID is to be returned for an ESN read .

DEPENDENCIES
  Non-reentrant

RETURN VALUE
  None.
                                               
SIDE EFFECTS
  Updates the usage indicator variable nvruim_esn_usage.

===========================================================================*/
extern void nvruim_set_esn_usage
(
   nvruim_esn_usage_type esn_usage
);
#ifdef FEATURE_HTORPC_METACOMMENTS 
/*~ FUNCTION nvruim_set_esn_usage */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/*===========================================================================

FUNCTION NVRUIM_GET_ESN_USAGE

DESCRIPTION
   This function gets the usage indicator that is used by the NV-RUIM module
   to decide whether ESN or R-UIM ID is to be returned for an ESN read .

DEPENDENCIES
  Non-reentrant

RETURN VALUE
  None.
                                               
SIDE EFFECTS

===========================================================================*/
extern nvruim_esn_usage_type nvruim_get_esn_usage
(
   void
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION nvruim_get_esn_usage */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/*===========================================================================

FUNCTION NVRUIM_FLUSH_CACHE                                 EXTERNAL FUNCTION

DESCRIPTION
  This function clears the cache control so that all items are read from the 
  RUIM the next time they are requested.

DEPENDENCIES
  This function affects the cache control variable and therefore affects the 
  operation of the read of cached NV items from the RUIM.

RETURN VALUE
  None

SIDE EFFECTS
  This function affects the operation of the cache algorithm.

===========================================================================*/
extern void nvruim_flush_cache
(
  void 
);


/*===========================================================================

FUNCTION NVRUIM_FLUSH_SMS_CACHE                             EXTERNAL FUNCTION

DESCRIPTION
  This function clears the SMS cache control so that the SMS record is read 
  from the RUIM the next time it is requested.

DEPENDENCIES
  This function affects the cache control variable and therefore affects the 
  operation of the read of cached SMS record from the RUIM.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void nvruim_flush_sms_cache
(
  void 
);

/*===========================================================================

FUNCTION NVRUIM_SET_UIM_DIR_PRESENT

DESCRIPTION
  The function set the flag to indicate which of the MF, CDMA DF, GSM DF, or DCS 
  1800 DF exist.
  
DEPENDENCIES
  All above files have to be selected first.

RETURN VALUE
  None
                                               
SIDE EFFECTS
  None.
===========================================================================*/
extern void nvruim_set_uim_dir_present(byte dir);


#ifdef FEATURE_RUIM_PHONEBOOK

/*===========================================================================

FUNCTION NVRUIM_SERVICE_INIT

DESCRIPTION
    This function does the following:
      1.  Read the CDMA Service Table (EF '6F32') and determine if a particular
        service is allocated and activated.
      2.  Select EFs and collect parameters that will be used in future.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE:   The service is available.
  FALSE:  The service is unailable or the EF not accessable.
                                               
SIDE EFFECTS
  None.

===========================================================================*/
extern boolean nvruim_service_init( nv_items_enum_type nvitem ); 

/*===========================================================================

FUNCTION NVRUIM_NUM_ADN_REC

DESCRIPTION
    This function returns the ADN number of records.

DEPENDENCIES
  None.

RETURN VALUE
  ADN number of records
                                               
SIDE EFFECTS
  None.

===========================================================================*/
extern word nvruim_num_adn_rec (void);

/*===========================================================================

FUNCTION NVRUIM_NUM_LND_REC

DESCRIPTION
  This function returns the LND number of records.

DEPENDENCIES
  None.

RETURN VALUE
  LND number of records
                                               
SIDE EFFECTS
  None.

===========================================================================*/
extern word nvruim_num_lnd_rec (void);

/*===========================================================================

FUNCTION NVRUIM_PHBOOK_BCD_TO_ASCII
                                        
DESCRIPTION
  This function converts phone book data from BCD format to ASCII format. 
  'F' indicates the end of data string.

DEPENDENCIES
  None.

RETURN VALUE
  Actual number of digits.
                                                 
SIDE EFFECTS
  None.

===========================================================================*/
extern byte nvruim_phbook_bcd_to_ascii( byte num_digi, /* Number of dialing digits */
                                 byte *from_ptr,/* Convert from */ 
                                 byte *to_ptr   /* Convert to */
);
/*===========================================================================

FUNCTION NVRUIM_PHBOOK_ASCII_TO_BCD
                                        
DESCRIPTION
  This function converts phone book data from ASCII format to BCD format. 
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
                                                 
SIDE EFFECTS
  None.

===========================================================================*/
extern void nvruim_phbook_ascii_to_bcd( byte num_digi, /* Number of dialing digits */
                                 byte *from_ptr,/* Convert from */ 
                                 byte *to_ptr   /* Convert to */
);
   
#endif /* FEATURE_RUIM_PHONEBOOK */

/*===========================================================================

FUNCTION UIM_INIT_3GPD_PREFERENCE
                                        
DESCRIPTION
  This function is called from the DS task during initialization for 3GPD.
  It passes in a boolean which indicates whether or not the RUIM provides
  for 3GPD services
  
DEPENDENCIES
  None.

RETURN VALUE
                                                 
SIDE EFFECTS
  Sets the uim_3gpd_support_available global variable for RUIM support of 3GPD
  Sets the uim_3gpd_cdma_svc global variable to indicate CDMA service support


===========================================================================*/ 
extern void uim_init_3gpd_preference
(
  boolean service_3gpd_sip,
  boolean service_3gpd_mip
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION uim_init_3gpd_preference */
#endif

/*===========================================================================

FUNCTION UIM_3GPD_SUPPORT
                                        
DESCRIPTION
  This function checks whether 3GPD data and processing should come from the
  RUIM or from NV 
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
                                                 
SIDE EFFECTS
  None.

===========================================================================*/
extern uim_3gpd_support_status uim_3gpd_support
(
  void
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION uim_3gpd_support */
#endif


/*===========================================================================

FUNCTION NVRUIM_DATA_3GPD_INIT_EXTENSIONS_SUPPORT

DESCRIPTION
  This function is called from the Data task during initialization for 3GPD 
  Extentions Support. It passes in a boolean value which indicates whether 
  or not the RUIM card have support for service 15 (Messagind and 3GPD 
  Extentions) in CDMA service table. The requirement on the RUIM card for 
  this serviec is defined in Open Market Handset RUIM Specification
  (CDG Document 166).

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  Sets the nvruim_3gpd_ext_support global variable for extended 3GPD RUIM support
  Sets the nvruim_3gpd_ext_use_card variable for current status of 3GPD ext usage
===========================================================================*/ 
void nvruim_data_3gpd_init_extensions_support(
  boolean service_3gpd_extensions
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION nvruim_data_3gpd_init_extensions_support */
#endif


/*===========================================================================

FUNCTION NVRUIM_LBS_INIT_SUPPORT

DESCRIPTION
  This function is called from the Data task during initialization for LBS  
  Support. It passes in a boolean value which indicates whether or not the 
  RUIM card have support for service 24 (LBS)in its CDMA Service Table.
  The requirement on the RUIM card for this serviec is defined in Open Market 
  Handset RUIM Specification(CDG Document 166).

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  Sets the nvruim_lbs_support global variable for LBS support
===========================================================================*/ 
extern void nvruim_lbs_init_support(
  boolean service_lbs
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION nvruim_data_3gpd_init_extensions_support */
#endif


/*===========================================================================

FUNCTION UIM_INIT_HRPD_PREFERENCE
                                        
DESCRIPTION
  This function is called from the PS task during initialization for HRPD.
  It passes in a boolean which indicates whether or not the RUIM provides
  for HRPD services, and a boolean which indicates whether cave is to be used
  
DEPENDENCIES
  None.

RETURN VALUE
                                                 
SIDE EFFECTS
  Sets the nvruim_hrpd_control global variable for RUIM support of HRPD
  Sets the nvruim_hrpd_cdma_svc global variable to indicate CDMA service support
===========================================================================*/ 
void uim_init_an_hrpd_preference
(
boolean service_hrpd
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION uim_init_an_hrpd_preference */
#endif 


/*===========================================================================

FUNCTION UIM_AN_HRPD_SUPPORT
                                        
DESCRIPTION
  This function checks whether HRPD data and processing should come from the
  RUIM or from NV 
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
                                                 
SIDE EFFECTS
  None.

===========================================================================*/
extern uim_an_hrpd_support_status uim_an_hrpd_support
(
  void
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION uim_an_hrpd_support */
#endif


#ifdef FEATURE_UIM_RUN_TIME_ENABLE
/*===========================================================================

FUNCTION NVRUIM_PROCESS_RTRE_CONFIGURATION

DESCRIPTION
  This function queries the R-UIM card to determine if it supports
  R-UIM functionality (existance of CDMA DF) and if it supports
  phonebook (existance of Telcom DF).

DEPENDENCIES
  This function should only be called by the NV_CMD function to process
  an NV_RTRE_OP_CONFIG_F command.

RETURN VALUE
  NV_DONE_S  Operation is complete.

SIDE EFFECTS
  This function updates the RTRE control passed by reference based on 
  the selects made to the UIM card.

===========================================================================*/
extern nv_stat_enum_type nvruim_process_rtre_configuration
(
  nv_cmd_type     *cmd_ptr,   /* Pointer to command received on nv_cmd_q */
  nv_rtre_control_type *nv_rtre_control,
  nv_rtre_polling_control_type *nv_rtre_polling_control
);

#endif /* FEATURE_UIM_RUN_TIME_ENABLE */

#if defined (FEATURE_UIM_CAVE_AN_AUTH) || defined (FEATURE_PERSO_RUIM)
/*===========================================================================

FUNCTION NVRUIM_DECODE_MIN                                 INTERNAL FUNCTION

DESCRIPTION
     Decode MIN 1 & 2 to a vector of digits

PARAMETERS
   min1        MIN1 being decoded
   min2        MIN2 being decoded
   digits      where the decoded MIN is placed

RETURN VALUE
   TRUE - MIN was decoded
   FALSE - Decoding failed

SIDE EFFECTS
  None

===========================================================================*/
extern boolean nvruim_decode_min
(
  byte *digits,                   /* pointer to converted digits */
  word *mcc,                      /* mcc */
  byte *mnc,                      /* mnc */
  dword *min1,                    /* min1 */
  word *min2                      /* min2 */
);
#endif /* FEATURE_UIM_CAVE_AN_AUTH) || defined (FEATURE_PERSO_RUIM) */

/*===========================================================================

FUNCTION nvruim_init_wms_svc_items

DESCRIPTION
  This function is called from the WMS task during initialization for SMS.
  It passes in boolean values to indicate whether or not the RUIM provides
  for BCSMS and SMS Capabilities provisioning.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  Sets the nvruim_bcsms_svc and nvruim_smscap_svc 
  global variables to indicate service support.

===========================================================================*/ 
void nvruim_init_wms_svc_items
(
  boolean service_bcsms, 
  boolean service_smscap
);

#ifdef FEATURE_RUIM_CDMA_REFRESH
/*===========================================================================

FUNCTION nvruim_clear_cache

DESCRIPTION
  This function is called from the mmgsdi task as part of the refresh process.
  This function will clear the nvruim cache if a refresh on one of the files
  is requested.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE:  If the nvruim process was not locked and the cache could be cleared
  FALSE: If the nvruim process was locked and the cache could not be
         cleared.

SIDE EFFECTS
  nvruim cache will be cleared

===========================================================================*/ 
boolean nvruim_clear_cache
(
  uint8 num_files,
  const uim_items_enum_type *file_list_p
);

/*===========================================================================

FUNCTION nvruim_nv_item_to_uim_file

DESCRIPTION
  This function will return the corresponding uim_items_enum_type for a
  nv item.  This function can be used by clients that are registering for
  refresh notifications but use NV apis to read items.

DEPENDENCIES
  None.

RETURN VALUE
  uim_items_enum_type

SIDE EFFECTS
  None

===========================================================================*/ 
uim_items_enum_type nvruim_nv_item_to_uim_file
(
  nv_items_enum_type nvitem
);

/*===========================================================================

FUNCTION nvruim_uim_file_to_nv_item

DESCRIPTION
  This function will populate the pointer of type nvruim_nv_conversion passed 
  in with the NV Items corresponding with the file that is passed in.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/ 
void nvruim_uim_file_to_nv_item
(
  uim_items_enum_type  uim_file,
  nvruim_nv_conversion *nv_items_ptr
);
#endif /* FEATURE_RUIM_CDMA_REFRESH */

/*===========================================================================

FUNCTION NVRUIM_INIT_NON_3GPD_CDMA_CARD

DESCRIPTION
  This function is called from mmgsdi task during post pin initialization.
  It passes in a boolean, where
    TRUE : it is 3gpd disabled card
    FALSE: it is 3gpd enabled  card 

DEPENDENCIES
None.

RETURN VALUE

SIDE EFFECTS
none
===========================================================================*/ 
void nvruim_init_non_3gpd_cdma_card
( 
  boolean non_3gpd_cdma_card
);

/*===================================================================
FUNCTION NVRUIM_GENERATE_AN_NAI_WITH_IMSI

DESCRIPTION
  Get IMSI of R-UIM.

DEPENDENCIES
  Non-reentrant

RETURN VALUE

SIDE EFFECTS
None

==================================================================*/
extern boolean nvruim_generate_an_nai_with_imsi(
  uint8 *user_id_len_ptr, 
  char *user_id_info_ptr
);

/*===========================================================================

FUNCTION NVRUIM_DATA_3GPD_EXT_SUPPORT

DESCRIPTION
This function returns 3GPD_EXT service availability

DEPENDENCIES
This function should only be called after SUBSCRIPTION_READY evt

RETURN VALUE
 TRUE : When RUIM card has 3GPD_EXT service available
 FALSE: When RUIM card does not have 3GPD_EXT service 

SIDE EFFECTS
None.

===========================================================================*/
boolean nvruim_data_3gpd_ext_support( void
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION nvruim_data_3gpd_ext_support */
#endif 

/*===========================================================================

FUNCTION NVRUIM_CHECK_NON_3GPD_CDMA_CARD

DESCRIPTION
 This function returns the non 3gpd cdma card presence.

DEPENDENCIES
This function should only be called after SUBSCRIPTION_READY evt

RETURN VALUE
  nvruim_3gpd_nv_card_status where
  NVRUIM_NO_CARD        : NO Card 
  NVRUIM_NON_3GPD_CARD  : 3GPD disabled card 
  NVRUIM_3GPD_CARD      : 3GPD enabled card

SIDE EFFECTS
none
===========================================================================*/ 
nvruim_3gpd_nv_card_status nvruim_check_non_3gpd_cdma_card (
  void
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION nvruim_check_non_3gpd_cdma_card */
#endif

/*===================================================================
FUNCTION NVRUIM_PPP_CAVE_FALLBACK

DESCRIPTION
  Sets the hrpd control variable to Cave

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==================================================================*/
void nvruim_ppp_cave_fallback( void
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION nvruim_ppp_cave_fallback */
#endif

#endif /* NVRUIMI_H */
