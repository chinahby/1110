#ifndef GSDI_PERSO_SEC_H
#define GSDI_PERSO_SEC_H
/*===========================================================================


        G S D I   P E R S O N A L I Z A T I O N   E N G I N E

     S T R U C T U R E S  A N D  F U N C T I O N  P R O T O T Y P E S


DESCRIPTION
  Structure and Function prototype definitions for the personalization
  engine.  The gsdi_perso.c functions are the entry point into the
  engine.  The engine is going to retrieve all of the information required
  to allow the security library to perform the security procedures.


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2001-2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/gsdi_perso_sec.h#3 $$ $DateTime: 2009/02/05 22:10:16 $ $Author: jruvalca $

when       who     what, where, why
--------   ---     --------------------------------------------------------
02/02/09   jar     Removed timer to gsdi.h for global scope
01/22/09   sun     Added new timer 
04/16/08   kk      Added support of get command on sim lock data
02/08/07   sun     Made indicators more meaningful
12/06/06   wli	   Corrected RUIM offset index.
09/13/06   wli     Added #defines for RUIM lock
04/13/06   sun     Removed perm_feat_ind
04/06/06   sun     Added support for RUIM Lock
03/16/06   sun     Added #defines for SP/CP code length
03/08/06   jar     Merged in mainline Reference Security Library
01/25/06   sun     Write to FS only if the data was modified.
12/20/05   sun     Removed unnecessary booleans from gsdi_perso_sec_data_mod_inds_type
11/09/05   sun     Fixed Lint Errors
10/03/05   sun     Moved #define to gsdi_exp.h
05/05/05   sun     Added support for Perso Refresh
04/28/05   sun     Initial release of the GSDI SIM Lock Architecutre to support
                   Plug In Proprietary Security Libraries driven by a Perso Engine.


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "gsdi_perso_engine.h"

#if defined  (FEATURE_MMGSDI_PERSONALIZATION) && \
    defined  (FEATURE_MMGSDI_PERSONALIZATION_ENGINE ) && \
    !defined (FEATURE_MMGSDI_PERSONALIZATION_NON_QC)

#include "gsdi_perso_engine.h"
/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* -------------------------------------------------------------------------
   MACROS:       GSDI_PERSO_SEC_MASK_FEAT_NOT_INIT  0x0000
                 GSDI_PERSO_SEC_MASK_FEAT_NW        0x0001
                 GSDI_PERSO_SEC_MASK_FEAT_NS        0x0002
                 GSDI_PERSO_SEC_MASK_FEAT_SP        0x0004
                 GSDI_PERSO_SEC_MASK_FEAT_CP        0x0008
                 GSDI_PERSO_SEC_MASK_FEAT_SIM       0x0010
                 GSDI_PERSO_SEC_MASK_FEAT_1_TO_1    0x0020

   DESCRIPTION:  Used to notify the security Library as to which actions
                 need to be performed on which SIM LOCK FEATURES
   -----------------------------------------------------------------------*/
#define GSDI_PERSO_SEC_MASK_FEAT_NOT_INIT  0x0000
#ifdef FEATURE_PERSO_SIM
#define GSDI_PERSO_SEC_MASK_FEAT_NW        0x0001
#define GSDI_PERSO_SEC_MASK_FEAT_NS        0x0002
#define GSDI_PERSO_SEC_MASK_FEAT_SP        0x0004
#define GSDI_PERSO_SEC_MASK_FEAT_CP        0x0008
#define GSDI_PERSO_SEC_MASK_FEAT_SIM       0x0010
#endif /* FEATURE_PERSO_SIM */
#ifdef FEATURE_PERSO_RUIM
#define GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW1  0x0020
#define GSDI_PERSO_SEC_MASK_FEAT_RUIM_NW2  0x0040
#define GSDI_PERSO_SEC_MASK_FEAT_RUIM_HRPD 0x0080
#define GSDI_PERSO_SEC_MASK_FEAT_RUIM_SP   0x0100
#define GSDI_PERSO_SEC_MASK_FEAT_RUIM_CP   0x0200
#define GSDI_PERSO_SEC_MASK_FEAT_RUIM_RUIM 0x0400
#endif /* FEATURE_PERSO_RUIM */
#define GSDI_PERSO_SEC_MASK_FEAT_PROP1     0x0800


/* -------------------------------------------------------------------------
   MACROS:       GSDI_PERSO_SEC_IMSI_MAX_LEN       0x0009
                 GSDI_PERSO_SEC_CNL_MAX_LEN        0x00FF
                 GSDI_PERSO_SEC_GID1_MAX_LEN       0x00FF
                 GSDI_PERSO_SEC_MAX_DATA_LEN       0x00FF

   DESCRIPTION:  Used to indicate the maximum length allowed for the
                 files identified.
   -----------------------------------------------------------------------*/
#define GSDI_PERSO_SEC_IMSI_MAX_LEN        0x0009
#define GSDI_PERSO_SEC_CNL_MAX_LEN         0x00FF
#define GSDI_PERSO_SEC_GID_MAX_LEN         0x00FF
#define GSDI_PERSO_SEC_MAX_DATA_LEN        0x00FF

/* -------------------------------------------------------------------------
   MACROS:       GSDI_PERSO_SEC_MAX_DCK_LEN

   DESCRIPTION:  Used to control the max number of bytes allowed
                 for a Depersonalization Control Key.
   -----------------------------------------------------------------------*/
#define GSDI_PERSO_SEC_MAX_DCK_LEN         0x08

/* -------------------------------------------------------------------------
   MACROS:       GSDI_PERSO_SEC_DECRYPT_MAX_LEN
                 GSDI_PERSO_SEC_ENCRYPT_MAX_LEN
                 GSDI_PERSO_SEC_ENCRYPT_CK_LEN

   DESCRIPTION:  Used to limit the amount of encrypted and decrytped
                 data used in the encryption and decryption algorithms.

                 Safeguards to avoid memory overwrites.
   -----------------------------------------------------------------------*/
#define GSDI_PERSO_SEC_ENCRYPT_MAX_LEN   0x0400
#define GSDI_PERSO_SEC_DECRYPT_MAX_LEN   (GSDI_PERSO_SEC_ENCRYPT_MAX_LEN / 2)
#define GSDI_PERSO_SEC_ENCRYPT_CK_LEN    (GSDI_PERSO_SEC_MAX_DCK_LEN * 2)

/* -------------------------------------------------------------------------
   MACRO:        GSDI_PERSO_SEC_MAX_NUM_INDS

   DESCRIPTION:  Will indicate the maximum number of personalization
                 indicators.  The proprietary solutions may contain more
                 or less.  This Macro will need to be updated accordingly.
   -----------------------------------------------------------------------*/
#if defined(FEATURE_PERSO_SIM) && defined(FEATURE_PERSO_RUIM)
/* Need to work this out when both SIM and RUIM are defined */
#define GSDI_PERSO_SEC_MAX_NUM_INDS      0x0B
#elif defined(FEATURE_PERSO_SIM)
#define GSDI_PERSO_SEC_MAX_NUM_INDS      0x05
#elif defined(FEATURE_PERSO_RUIM)
#define GSDI_PERSO_SEC_MAX_NUM_INDS      0x06
#endif


/* -------------------------------------------------------------------------
   MACRO:        GSDI_PERSO_SEC_IND_OFFSET_NW_OFFSET 0x01
                 GSDI_PERSO_SEC_IND_OFFSET_NS_OFFSET 0x02
                 GSDI_PERSO_SEC_IND_OFFSET_SP_OFFSET 0x03
                 GSDI_PERSO_SEC_IND_OFFSET_CP_OFFSET 0x04
                 GSDI_PERSO_SEC_IND_OFFSET_SIM_OFFSET 0x05

   DESCRIPTION:  Offsets used in the arrays used to store the different
                 Feature Indications when bundled into arrays.
   -----------------------------------------------------------------------*/
#ifdef FEATURE_PERSO_SIM
#define GSDI_PERSO_SEC_IND_OFFSET_NW_OFFSET  0x00
#define GSDI_PERSO_SEC_IND_OFFSET_NS_OFFSET  0x01
#define GSDI_PERSO_SEC_IND_OFFSET_SP_OFFSET  0x02
#define GSDI_PERSO_SEC_IND_OFFSET_CP_OFFSET  0x03
#define GSDI_PERSO_SEC_IND_OFFSET_SIM_OFFSET 0x04
#define GSDI_PERSO_SEC_IND_OFFSET_PROP1_OFFSET 0x05
#endif /* FEATURE_PERSO_SIM */

#ifdef FEATURE_PERSO_RUIM
#define GSDI_PERSO_SEC_IND_OFFSET_RUIM_NW1_OFFSET   0x00
#define GSDI_PERSO_SEC_IND_OFFSET_RUIM_NW2_OFFSET   0x01
#define GSDI_PERSO_SEC_IND_OFFSET_RUIM_HRPD_OFFSET  0x02
#define GSDI_PERSO_SEC_IND_OFFSET_RUIM_SP_OFFSET    0x03
#define GSDI_PERSO_SEC_IND_OFFSET_RUIM_CP_OFFSET    0x04
#define GSDI_PERSO_SEC_IND_OFFSET_RUIM_RUIM_OFFSET  0x05
#define GSDI_PERSO_SEC_IND_OFFSET_PROP1_OFFSET      0x06
#endif /* FEATURE_PERSO_RUIM */
                                               

/* -------------------------------------------------------------------------
   MACRO:        GSDI_PERSO_SEC_CK_CNT_OFFSET_NW_OFFSET  0x01
                 GSDI_PERSO_SEC_CK_CNT_OFFSET_NS_OFFSET  0x02
                 GSDI_PERSO_SEC_CK_CNT_OFFSET_SP_OFFSET  0x03
                 GSDI_PERSO_SEC_CK_CNT_OFFSET_CP_OFFSET  0x04
                 GSDI_PERSO_SEC_CK_CNT_OFFSET_SIM_OFFSET 0x05

   DESCRIPTION:  Offsets used in the arrays used to store the different
                 Controk Key Counters used to track incorrect
                 Control Key Verification attepts.
   -----------------------------------------------------------------------*/
#ifdef FEATURE_PERSO_SIM
#define GSDI_PERSO_SEC_CK_CNT_OFFSET_NW_OFFSET  0x00
#define GSDI_PERSO_SEC_CK_CNT_OFFSET_NS_OFFSET  0x01
#define GSDI_PERSO_SEC_CK_CNT_OFFSET_SP_OFFSET  0x02
#define GSDI_PERSO_SEC_CK_CNT_OFFSET_CP_OFFSET  0x03
#define GSDI_PERSO_SEC_CK_CNT_OFFSET_SIM_OFFSET 0x04
#endif /*FEATURE_PERSO_SIM*/

#ifdef FEATURE_PERSO_RUIM
#define GSDI_PERSO_SEC_CK_CNT_OFFSET_RUIM_NW1_OFFSET  NUM_OF_PERSO_SIM_FEATURES
#define GSDI_PERSO_SEC_CK_CNT_OFFSET_RUIM_NW2_OFFSET  NUM_OF_PERSO_SIM_FEATURES+0x01
#define GSDI_PERSO_SEC_CK_CNT_OFFSET_RUIM_HRPD_OFFSET NUM_OF_PERSO_SIM_FEATURES+0x02
#define GSDI_PERSO_SEC_CK_CNT_OFFSET_RUIM_SP_OFFSET   NUM_OF_PERSO_SIM_FEATURES+0x03
#define GSDI_PERSO_SEC_CK_CNT_OFFSET_RUIM_CP_OFFSET   NUM_OF_PERSO_SIM_FEATURES+0x04
#define GSDI_PERSO_SEC_CK_CNT_OFFSET_RUIM_RUIM_OFFSET NUM_OF_PERSO_SIM_FEATURES+0x05
#endif /* FEATURE_PERSO_RUIM */


/* -------------------------------------------------------------------------
   MACRO:        GSDI_PERSO_SEC_CK_OFFSET_NW_OFFSET  0x01
                 GSDI_PERSO_SEC_CK_OFFSET_NS_OFFSET  0x02
                 GSDI_PERSO_SEC_CK_OFFSET_SP_OFFSET  0x03
                 GSDI_PERSO_SEC_CK_OFFSET_CP_OFFSET  0x04
                 GSDI_PERSO_SEC_CK_OFFSET_SIM_OFFSET 0x05

   DESCRIPTION:  Offsets used in the data provided to move to different
                 Personalization Control Keys.
   -----------------------------------------------------------------------*/
#ifdef FEATURE_PERSO_SIM
#define GSDI_PERSO_SEC_CK_OFFSET_NW_OFFSET  0x0000
#define GSDI_PERSO_SEC_CK_OFFSET_NS_OFFSET  ( GSDI_PERSO_SEC_CK_OFFSET_NW_OFFSET \
                                            + GSDI_PERSO_SEC_MAX_DCK_LEN * 2)
#define GSDI_PERSO_SEC_CK_OFFSET_SP_OFFSET  ( GSDI_PERSO_SEC_CK_OFFSET_NS_OFFSET \
                                            + GSDI_PERSO_SEC_MAX_DCK_LEN * 2)
#define GSDI_PERSO_SEC_CK_OFFSET_CP_OFFSET  ( GSDI_PERSO_SEC_CK_OFFSET_SP_OFFSET \
                                            + GSDI_PERSO_SEC_MAX_DCK_LEN * 2)
#define GSDI_PERSO_SEC_CK_OFFSET_SIM_OFFSET ( GSDI_PERSO_SEC_CK_OFFSET_CP_OFFSET \
                                            + GSDI_PERSO_SEC_MAX_DCK_LEN * 2)
#endif /* FEATURE_PERSO_SIM */

#ifdef FEATURE_PERSO_RUIM
#define GSDI_PERSO_SEC_CK_OFFSET_RUIM_NW1_OFFSET  0x0000
#define GSDI_PERSO_SEC_CK_OFFSET_RUIM_NW2_OFFSET  ( GSDI_PERSO_SEC_CK_OFFSET_RUIM_NW1_OFFSET \
                                                  + GSDI_PERSO_SEC_MAX_DCK_LEN * 2)
#define GSDI_PERSO_SEC_CK_OFFSET_RUIM_HRPD_OFFSET ( GSDI_PERSO_SEC_CK_OFFSET_RUIM_NW2_OFFSET \
                                                  + GSDI_PERSO_SEC_MAX_DCK_LEN * 2)
#define GSDI_PERSO_SEC_CK_OFFSET_RUIM_SP_OFFSET  ( GSDI_PERSO_SEC_CK_OFFSET_RUIM_HRPD_OFFSET \
                                                 + GSDI_PERSO_SEC_MAX_DCK_LEN * 2)
#define GSDI_PERSO_SEC_CK_OFFSET_RUIM_CP_OFFSET  ( GSDI_PERSO_SEC_CK_OFFSET_RUIM_SP_OFFSET \
                                                 + GSDI_PERSO_SEC_MAX_DCK_LEN * 2)
#define GSDI_PERSO_SEC_CK_OFFSET_RUIM_RUIM_OFFSET ( GSDI_PERSO_SEC_CK_OFFSET_RUIM_CP_OFFSET \
                                                 + GSDI_PERSO_SEC_MAX_DCK_LEN * 2)
#endif /* FEATURE_PERSO_RUIM */

/* ----------------------------------------------------------------------------
   GSDI_PERSO_DCK_NW_CK_OFFSET
   GSDI_PERSO_DCK_NS_CK_OFFSET
   GSDI_PERSO_DCK_SP_CK_OFFSET
   GSDI_PERSO_DCK_CP_CK_OFFSET
   File Offsets for EF-DCK to retrieve the Control Keys From
   --------------------------------------------------------------------------*/

#ifdef FEATURE_PERSO_SIM
#define GSDI_PERSO_SEC_DCK_NW_CK_OFFSET        0x00
#define GSDI_PERSO_SEC_DCK_NS_CK_OFFSET        0x04
#define GSDI_PERSO_SEC_DCK_SP_CK_OFFSET        0x08
#define GSDI_PERSO_SEC_DCK_CP_CK_OFFSET        0x0C
#endif /* FEATURE_PERSO_SIM */

#ifdef FEATURE_PERSO_RUIM
#define GSDI_PERSO_SEC_DCK_RUIM_NW1_CK_OFFSET    0x00
#define GSDI_PERSO_SEC_DCK_RUIM_NW2_CK_OFFSET    0x04
#define GSDI_PERSO_SEC_DCK_RUIM_HRPD_CK_OFFSET   0x08
#define GSDI_PERSO_SEC_DCK_RUIM_SP_CK_OFFSET     0x0C
#define GSDI_PERSO_SEC_DCK_RUIM_CP_CK_OFFSET     0x10
#endif /* FEATURE_PERSO_RUIM */                    


/* Offsets used when taking data_p information and populating the cnf rsp */
#ifdef FEATURE_PERSO_SIM
#define GSDI_PERSO_SEC_NW_STATUS_IND_OFFSET                   0x00
#define GSDI_PERSO_SEC_NS_STATUS_IND_OFFSET                   0x01
#define GSDI_PERSO_SEC_SP_STATUS_IND_OFFSET                   0x02
#define GSDI_PERSO_SEC_CP_STATUS_IND_OFFSET                   0x03
#define GSDI_PERSO_SEC_SIM_STATUS_IND_OFFSET                  0x04
#endif /* FEATURE_PERSO_SIM */

#ifdef FEATURE_PERSO_RUIM
#define GSDI_PERSO_SEC_RUIM_NW1_STATUS_IND_OFFSET  0x00
#define GSDI_PERSO_SEC_RUIM_NW2_STATUS_IND_OFFSET  0x01
#define GSDI_PERSO_SEC_RUIM_HRPD_STATUS_IND_OFFSET 0x02
#define GSDI_PERSO_SEC_RUIM_SP_STATUS_IND_OFFSET   0x03
#define GSDI_PERSO_SEC_RUIM_CP_STATUS_IND_OFFSET   0x04
#define GSDI_PERSO_SEC_RUIM_RUIM_STATUS_IND_OFFSET 0x05
#endif /* FEATURE_PERSO_RUIM */


/* OTA Deperso Message Offsets
 1 -  40  Operator specific text padded with spaces to character 40.
41 -  48  Network control key
49 -  56  Network subset control key
57 -  64  SP control key
65 -  72  Corporate control key
73 -  88  IMEI
*/
#ifdef FEATURE_PERSO_SIM
#define GSDI_PERSO_SEC_OTA_DEPERSO_OP_STOP_OFFSET  39
#define GSDI_PERSO_SEC_OTA_DEPERSO_IMEI_OFFSET     72
#define GSDI_PERSO_SEC_OTA_DEPERSO_NW_CK_OFFSET    40
#define GSDI_PERSO_SEC_OTA_DEPERSO_NS_CK_OFFSET    48
#define GSDI_PERSO_SEC_OTA_DEPERSO_SP_CK_OFFSET    56
#define GSDI_PERSO_SEC_OTA_DEPERSO_CP_CK_OFFSET    64

#define GSDI_PERSO_SEC_OTA_DEPERSO_RSP_NW_OFFSET   16
#define GSDI_PERSO_SEC_OTA_DEPERSO_RSP_NS_OFFSET   17
#define GSDI_PERSO_SEC_OTA_DEPERSO_RSP_SP_OFFSET   18
#define GSDI_PERSO_SEC_OTA_DEPERSO_RSP_CP_OFFSET   19
#define GSDI_PERSO_SEC_OTA_DEPERSO_RSP_LEN         20

/* OTA Deperso Depersonalization CK Len */
#define GSDI_PERSO_SEC_OTA_DEPERSO_CK_LEN          0x08
#endif /*FEATURE_PERSO_SIM*/
/* Indicator that there are no more retries remaining */
#define GSDI_PERSO_NO_MORE_CK_RETRIES                     0x00

/* Max number of retries allowed */
#define GSDI_PERSO_MAX_NUMBER_OF_RETRIES                  0x0A
#define GSDI_PERSO_MAX_UNBLOCK_NUMBER_OF_RETRIES          0x20


/* Default Power Up Indicators */
#define GSDI_PERSO_INDICATOR_NONE                         0x00

#ifdef FEATURE_PERSO_SIM
#define GSDI_PERSO_NW_INDICATOR                           0x10
#define GSDI_PERSO_NS_INDICATOR                           0x20
#define GSDI_PERSO_SP_INDICATOR                           0x30
#define GSDI_PERSO_CP_INDICATOR                           0x40
#define GSDI_PERSO_SIM_INDICATOR                          0x50
#endif /* FEATURE_PERSO_SIM */
#ifdef FEATURE_PERSO_RUIM
#define GSDI_PERSO_RUIM_NW1_INDICATOR                     0x60
#define GSDI_PERSO_RUIM_NW2_INDICATOR                     0x70
#define GSDI_PERSO_RUIM_HRPD_INDICATOR                    0x80
#define GSDI_PERSO_RUIM_SP_INDICATOR                      0x90
#define GSDI_PERSO_RUIM_CP_INDICATOR                      0xA0
#define GSDI_PERSO_RUIM_RUIM_INDICATOR                    0xB0

#define GSDI_PERSO_SEC_RUIM_NW1_CODE_LEN                  0x03
#define GSDI_PERSO_SEC_RUIM_NW2_CODE_LEN                  0x02
#define GSDI_PERSO_SEC_RUIM_RUIM_CODE_LEN                 0x08

#endif /* FEATURE_PERSO_RUIM */

#ifdef FEATURE_PERSO_SIM
/*Code Lengths for SP/CP features*/
#define GSDI_PERSO_SEC_SP_CODE_LEN                    0x04
#define GSDI_PERSO_SEC_CP_CODE_LEN                    0x05

/* Exhausted the number of Phone Code Retries */
#define GSDI_PERSO_PHONE_CODE_EXHAUSTED               0x00
#endif


/* --------------------------------------------------------------------------
   GSDI_PERSO_MAX_SMS_STATUS_LEN
   Max number of bytes allowed in the SMS Status returned for the OTA
   Deperso SMS-PP
   Character  Description
    1- 16 IMEI of ME
       17 Network personalisation status
       18 Network subset personalisation status
       19 SP personalisation status
       20 Corporate personalisation status
   ------------------------------------------------------------------------*/
#define GSDI_PERSO_MAX_SMS_STATUS_LEN      0x0014

/*Bytes that are used from the EF-CNL to generate 
  Codes for the various Features*/
#define GSDI_PERSO_SEC_CNL_NW_BYTES        0x03
#define GSDI_PERSO_SEC_CNL_NS_BYTES        0x04

#ifdef FEATURE_PERSO_SIM
#define GSDI_PERSO_SEC_CNL_SP_BYTES        0x03
#define GSDI_PERSO_SEC_CNL_CP_BYTES        0x03
#endif 

 /*Total Size of the a CNL Block*/
#define GSDI_PERSO_SEC_CNL_BLOCK_SIZE      0x06

typedef enum {
  GSDI_PERSO_CURRENTLY_NOT_PERSO  = 0,
  GSDI_PERSO_PERMANENTLY_NOT_PERS = 1,
  GSDI_PERSO_PERSONALIZED         = 2,
  GSDI_PERSO_IMEI_MISMATCH        = 3
}gsdi_perso_sec_ota_deperso_status_enum_type;

/* -------------------------------------------------------------------------
   STRUCTURE:    GSDI_PERSO_SEC_DATA_TYPE

   DESCRIPTION:  Used to hold the data send to the security library.
                 Data held is typically encryped Data Stored on
                 the File System.
   -----------------------------------------------------------------------*/
typedef struct {
  uint8 * buffer_p;
  uint32   len;
}gsdi_perso_sec_data_type;

/* -------------------------------------------------------------------------
   STRUCTURE:    GSDI_PERSO_SEC_CLIENT_DATA_TYPE

   DESCRIPTION:  Will contain the data provided by the client.
                 This data will be encoded as required by the caller's
                 desired function.

                 Feature Mask:
                 0x00 - GSDI_PERSO_SEC_LIB_MASK_NOT_INIT
   -----------------------------------------------------------------------*/
typedef struct {
  uint32  feature_mask;
  gsdi_perso_sec_data_type dck;
  gsdi_perso_sec_data_type feature_data;
  gsdi_perso_sec_data_type phone_code;
  gsdi_perso_sec_data_type new_phone_code;
  gsdi_perso_sec_data_type master_key;
  boolean                  get_unblock_key;
}gsdi_perso_sec_client_req_type;

/* --------------------------------------------------------------------------
   STRUCTURE:    GSDI_PERSO_SEC_SIM_ELE_DATA_TYPE

   DESCRIPTION:  This is the basic SIM Data Element provided to the security
                 library.

                 This structure can be used for the data elements required
                 in the SIM Personalization Procedures.

                 EF-IMSI
                 EF-CNL
                 EF-GID1
                 EF-GID2
   ------------------------------------------------------------------------*/
typedef struct {
  uint32   len;
  uint8 * buffer_p;
}gsdi_perso_sec_sim_ele_data_type;

/* --------------------------------------------------------------------------
   STRUCTURE:    GSDI_PERSO_SEC_ME_DATA_UPDATED

   DESCRIPTION:  This is the indicator used to let the personalization
                 engine know wheter or no the data was updated in the
                 Security Library for writing.
   --------------------------------------------------------------------------*/
typedef struct{
  boolean  update_fs;
  boolean  refresh;
}gsdi_perso_sec_data_mod_inds_type;

/* --------------------------------------------------------------------------
   STRUCTURE:    GSDI_PERSO_SEC_ME_DATA_TYPE

   DESCRIPTION:  This is the GSDI Personalization Mobile Equipment
                 structure used to hold all data that is encrypted.

                 The secured / encrypted information is the
                 Feature Indications
                 Depersonalization key
                 Mobile Equipment IMEI
                 Mobile Equipment Serial Number
   --------------------------------------------------------------------------*/
typedef struct{
  gsdi_perso_sec_data_type          feat_inds;
  gsdi_perso_sec_data_type          perm_feat_inds;
  gsdi_perso_sec_data_type          sim_lock_codes;
  gsdi_perso_sec_data_type          dcks;
  gsdi_perso_sec_data_type          dck_counts;
  gsdi_perso_sec_data_type          unblock_cks;
  gsdi_perso_sec_data_type          unblock_ck_counts;
  gsdi_perso_sec_data_type          me_imei;
  gsdi_perso_sec_data_type          me_sn;
  gsdi_perso_sec_data_mod_inds_type data_mod;
}gsdi_perso_sec_me_data_type;


/* --------------------------------------------------------------------------
   STRUCTURE:    GSDI_PERSO_SEC_SIM_DATA_TYP

   DESCRIPTION:  This is the SIM Data Structure provided to the Security
                 Library.  This structure will contain the following data:
                 IMSI, Cooperative Network List, Group ID 1, and Group ID 2
   --------------------------------------------------------------------------*/
typedef struct {
  gsdi_perso_sec_sim_ele_data_type imsi;
  gsdi_perso_sec_sim_ele_data_type cnl;
  gsdi_perso_sec_sim_ele_data_type gid1;
  gsdi_perso_sec_sim_ele_data_type gid2;
  gsdi_perso_sec_sim_ele_data_type dck;
}gsdi_perso_sec_sim_data_type;


/* ----------------------------------------------------------------------------
   STRUCTURE:    GSDI_PERSO_SEC_INDS_STATE_ENUM_TYPE

   DESCRIPTION:  Will be used to label the state of each
                 feature indication once decrypted.
   --------------------------------------------------------------------------*/
typedef enum {
  GSDI_PERSO_SEC_DEACTIVATE_FEATURE   = GSDI_PERSO_ENG_NOT_ACTIVE_STATE,
  GSDI_PERSO_SEC_ACTIVATE_FEATURE     = GSDI_PERSO_ENG_ACTIVE_STATE,
  GSDI_PERSO_SEC_DISABLE_FEATURE      = GSDI_PERSO_ENG_DISABLED_STATE,
  GSDI_PERSO_SEC_AUTOLOCK_FEATURE     = GSDI_PERSO_ENG_AUTOLOCK_STATE,
  GSDI_PERSO_SEC_MAX_FEATURE          = 0xFF
}gsdi_perso_sec_inds_state_enum_type;



/* ----------------------------------------------------------------------------
   STRUCTURE:    GSDI_PERSO_SEC_PROCEDURES_TYPE

   DESCRIPTION:  Will desribe the different procedures performed by the
                 Security Libraries
   --------------------------------------------------------------------------*/
typedef enum {
  GSDI_PERSO_SEC_PROC_DEACTIVATE_FEATURE   = 0x0000,
  GSDI_PERSO_SEC_PROC_ACTIVATE_FEATURE,
  GSDI_PERSO_SEC_PROC_DISABLE_FEATURE,
  GSDI_PERSO_SEC_PROC_SET_FEATURE_DATA,
  GSDI_PERSO_SEC_PROC_SELF_PERSONALIZE,
  GSDI_PERSO_SEC_PROC_VERIFY_DCK,
  GSDI_PERSO_SEC_PROC_CHANGE_DCK,
  GSDI_PERSO_SEC_PROC_MAX_                 = 0xFFFF
}gsdi_perso_sec_procedures_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:    GSDI_PERSO_SEC_DCRYPTED_INDS_TYPE

   DESCRIPTION:  Will contain the decrypted indicators
   --------------------------------------------------------------------------*/
typedef struct {
  gsdi_perso_sec_inds_state_enum_type inds[GSDI_PERSO_SEC_MAX_NUM_INDS];
  boolean  is_allowed[GSDI_PERSO_SEC_MAX_NUM_INDS];
}gsdi_perso_sec_inds_state_type;


/* ----------------------------------------------------------------------------
   STRUCTURE:    GSDI_PERSO_SEC_CALC_ENUM_TYPE

   DESCRIPTION:  Used primarily in the calcualtion of the required
                 memory.
   --------------------------------------------------------------------------*/
typedef enum {
  GSDI_PERSO_SEC_CALC_INDS         = 0x0000,
  GSDI_PERSO_SEC_CALC_CKS          = 0x0001,
  GSDI_PERSO_SEC_CALC_DATA         = 0x0002,
  GSDI_PERSO_SEC_CALC_FOR_GET_DATA = 0x0003
}gsdi_perso_sec_calc_enum_type;


gsdi_returns_T gsdi_perso_security_handle_request
(
  gsdi_perso_engine_proc_enum_type feature_type,
  gsdi_perso_sec_client_req_type  * req_data_p,
  gsdi_perso_sec_me_data_type    * encrypted_data_p,
  gsdi_perso_sec_sim_data_type   * sim_data_p,
  uint8 *                          ret_data_p,
  int32*                           ret_data_len_p,
  gsdi_perso_event_enum_type      * perso_event_p
);

/*===========================================================================
FUNCTION GSDI_SET_DATA_IN_FILES

DESCRIPTION
  This function is used to autogenerate the control keys, indicators,
  and number of retries.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
gsdi_returns_T gsdi_perso_security_create_data(
  gsdi_perso_sec_me_data_type    * encrypted_data_p,
  const uint8                    * data_p,
  int32                          * returned_data_len_p
);

#ifdef FEATURE_PERSO_RUIM
/*===========================================================================
FUNCTION GSDI_PERSO_RUIM_SECURITY_CREATE_DATA

DESCRIPTION
  This function is used to autogenerate the control keys, indicators,
  and number of retries.

DEPENDENCIES
  None

RETURN VALUE
  void
===========================================================================*/
gsdi_returns_T gsdi_perso_ruim_security_create_data(
  gsdi_perso_sec_me_data_type    * encrypted_data_p,
  const uint8                    * data_p,
  int32                          * returned_data_len_p

);
#endif /*FEATURE_PERSO_RUIM*/

#endif /* FEATURE_MMGSDI_PERSONALIZATION_NON_QCT */
#endif /* GSDI_PERSO_ENGINE_H */
