#ifndef GSDIDIAG_UIM_H
#define GSDIDIAG_UIM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   SIM/USIM  Diagnostic Interface to
                    PIPE commands to the UIM task


Supports automated testing of SIM/USIM/RUIM cards via DIAG commands.
Implements packet processing.

Copyright (c) 2003-2004,2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/gsdidiag_uim.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/22/08   js      Fixed UIM_MAX_PATH_ELEMENTS issue. Updated path length to 4 
                   for all UIM commands. MAX path length in MMGSDI is set to 5.
06/21/08   kk      Added PKCS15 Support
04/30/08   kk      Added support for MMS EFs
03/20/08   nk      Added OMH EF Support
11/19/07   js      Radio Access Technology Balancing support
11/08/07   vs      Added OMH feature support
02/09/07   sk      Added support for 1000 phonebook entries.
08/23/06   sun     Added support for CDMA_ECC
08/16/06   jar     Added Support for ANR Files
04/05/06   tml     lint
03/21/06   tml     added cfis and missing service table support and lint
02/16/06    sp     Support for EF_EHPLMN
11/17/05   tml     PNN and OPL support
11/09/05   sun/tml lint fixes
05/14/05    pv     Added new commands and fixed some data types
04/13/05    pv     Initial version with UIM_RESET_SWITCH_UIM_PASSIVE
                   UIM_RESET_WITH_TP_F & UIM_RESET_F commands
===========================================================================*/

#include "uim.h" /* for uim enum defns*/
#include "gsdi_exp.h" /* for return types etc */
#include "mmgsdilib.h"

#define MAX_INT               0x7FFFFFFF
#define MAX_UIM_DATA_LEN      800
/*============================================================================
    Enumerated type:  gsdidiag_uim_cmd_type
    List of available UIM Commands using the gsdidiag interface.
============================================================================*/
typedef enum
{
  GSDIDIAG_UIM_CMD_NO_SUCH_UIM_CMD,
  GSDIDIAG_UIM_CMD_UIM_RESET_SWITCH_UIM_PASSIVE_F,
  GSDIDIAG_UIM_CMD_UIM_POWER_DOWN_F,
  GSDIDIAG_UIM_CMD_UIM_POWER_UP_F,
  GSDIDIAG_UIM_CMD_UIM_MANAGE_CHANNEL_F,
  GSDIDIAG_UIM_CMD_UIM_ACCESS_F,
  GSDIDIAG_UIM_CMD_UIM_ISIM_AUTHENTICATE_F,
  GSDIDIAG_UIM_CMD_UIM_READ_BIN_CMD_F,
  GSDIDIAG_UIM_CMD_UIM_READ_REC_CMD_F,
  GSDIDIAG_UIM_CMD_UIM_WRITE_BIN_CMD_F,
  GSDIDIAG_UIM_CMD_UIM_WRITE_REC_CMD_F,
  GSDIDIAG_UIM_CMD_UIM_INC_CMD_F,

  GSDIDIAG_UIM_MAX_CMDS,

  /* Dummy to extend this enum to an int always */
  GSDIDIAG_DUMMY_UIM_CMD_TO_INT = MAX_INT
}gsdidiag_uim_cmd_type;

/*============================================================================
    Type definition for gsdidiag_uim_cmd_uim_cmd_pair_type
    PURPOSE   Translation between UIM commands exported from
    GSDIDIAG to UIM command
============================================================================*/
typedef struct
{
  gsdidiag_uim_cmd_type gsdidiag_uim_cmd;
  uim_cmd_name_type     uim_cmd_name;
}gsdidiag_uim_cmd_uim_cmd_pair_type;

/*============================================================================
    Enumerated type:  gsdidiag_uim_cmd_type
    List of available UIM Commands using the gsdidiag interface.
============================================================================*/
typedef enum
{
  GSDIDIAG_UIM_RPT_UIM_NO_SUCH_REPORT_R,
  GSDIDIAG_UIM_RPT_UIM_RESET_SWITCH_UIM_PASSIVE_R,
  GSDIDIAG_UIM_RPT_UIM_POWER_DOWN_R,
  GSDIDIAG_UIM_RPT_UIM_POWER_UP_R,
  GSDIDIAG_UIM_RPT_UIM_MANAGE_CHANNEL_R,
  GSDIDIAG_UIM_RPT_UIM_ACCESS_R,
  GSDIDIAG_UIM_RPT_UIM_ISIM_AUTHENTICATE_R,
  GSDIDIAG_UIM_RPT_UIM_READ_BIN_CMD_R,
  GSDIDIAG_UIM_RPT_UIM_READ_REC_CMD_R,
  GSDIDIAG_UIM_RPT_UIM_WRITE_BIN_CMD_R,
  GSDIDIAG_UIM_RPT_UIM_WRITE_REC_CMD_R,
  GSDIDIAG_UIM_RPT_UIM_INC_CMD_R,

  GSDIDIAG_UIM_MAX_RPTS,

  /* Dummy to extend this enum to an int always */
  GSDIDIAG_DUMMY_UIM_RPT_TO_INT = MAX_INT
}gsdidiag_uim_rpt_code_type;

/*============================================================================
    Type definition for gsdidiag_uim_cmd_uim_cmd_pair_type
    PURPOSE   Translation between UIM commands exported from
    GSDIDIAG to UIM command
============================================================================*/
typedef struct
{
  gsdidiag_uim_rpt_code_type gsdidiag_uim_rpt_code;
  uim_report_code_type          uim_rpt_code;
}gsdidiag_uim_rpt_uim_rpt_pair_type;


/*============================================================================
    Enumerated type:  gsdidiag_uim_cmd_type
    List of available protocol types when using the gsdidiag interface
============================================================================*/
typedef enum {
  GSDIDIAG_UIM_NO_SUCH_MODE = 0,  /* No such mode */
  GSDIDIAG_UIM_CDMA         = 1,  /* CDMA mode of operation */
  GSDIDIAG_UIM_GSM          = 2,  /* GSM mode of operation */
  GSDIDIAG_UIM_WCDMA        = 3,  /* USIM protocol which is based on UICC */
  GSDIDIAG_UIM_UICC         = 4,  /* UICC protocol */
  GSDIDIAG_UIM_MAXIMUM_MODE = 5,

  /* Dummy to extend this enum to an int always */
  GSDIDIAG_DUMMY_UIM_PROTOCOL_TO_INT = MAX_INT
}gsdidiag_uim_protocol_type;


/*============================================================================
    Type definition for gsdidiag_gsdi_protocol_uim_protocol_pair_type
    PURPOSE   Translation between protocol exported from the DIAG to
    the protocol type in UIM
============================================================================*/
typedef struct
{
  gsdidiag_uim_protocol_type gsdidiag_uim_protocol;
  uim_protocol_type          uim_protocol;
}gsdidiag_gsdi_protocol_uim_protocol_pair_type;


typedef enum
{
  GSDIDIAG_EF_ENUM_ACCESS = 0x00000000,
  GSDIDIAG_DF_ENUM_ACCESS,
  GSDIDIAG_BY_PATH_ACCESS,
  GSDIDIAG_MAX_ACCESS_ENUM           = 0x7FFFFFFF
} mmgsdidiag_access_enum_type;

/*===========================================================================
    Enumerated type:  gsdidiag_elementary_file_enum_type
      Define Filenames Available through GSDI - DIAG Interface
      CDMA RUIM Related Files Range:  0x00000001 - 0x000000FF
      GSM  SIM  Related Files Range:  0x00001000 - 0x0000FF00
      UMTS USIM Related Files Range:  0x00010000 - 0x00FF0000
      TELECOM   Related Files Range:  0x01000000 - 0xFF000000
===========================================================================*/

typedef enum
{
    //GSDIDIAG_MIN_CMD =-1

    GSDIDIAG_MF_ICCID                             = 0x00000001,
    GSDIDIAG_MF_ELP                               = 0x00000002,
    GSDIDIAG_MF_DIR                               = 0x00000003,
    GSDIDIAG_MF_ARR                               = 0x00000004,
    GSDIDIAG_MF_PL                                = 0x00000005,

    GSDIDIAG_CDMA_CC                              = 0x00000100,
    GSDIDIAG_CDMA_IMSI_M                          = 0x00000101,
    GSDIDIAG_CDMA_IMSI_T                          = 0x00000102,
    GSDIDIAG_CDMA_ANALOG_HOME_SID                 = 0x00000103,
    GSDIDIAG_CDMA_ANALOG_OP_PARAMS                = 0x00000104,
    GSDIDIAG_CDMA_ANALOG_LOCN_AND_REGN_IND        = 0x00000105,
    GSDIDIAG_CDMA_HOME_SID_NID                    = 0x00000106,
    GSDIDIAG_CDMA_ZONE_BASED_REGN_IND             = 0x00000107,
    GSDIDIAG_CDMA_SYS_REGN_IND                    = 0x00000108,
    GSDIDIAG_CDMA_DIST_BASED_REGN_IND             = 0x00000109,
    GSDIDIAG_CDMA_ACCOLC                          = 0x0000010A,
    GSDIDIAG_CDMA_CALL_TERM_MOD_PREF              = 0x0000010B,
    GSDIDIAG_CDMA_SCI                             = 0x0000010C,
    GSDIDIAG_CDMA_ANALOG_CHAN_PREF                = 0x0000010D,
    GSDIDIAG_CDMA_PRL                             = 0x0000010E,
    GSDIDIAG_CDMA_RUIM_ID                         = 0x0000010F,
    GSDIDIAG_CDMA_SVC_TABLE                       = 0x00000110,
    GSDIDIAG_CDMA_SPC                             = 0x00000111,
    GSDIDIAG_CDMA_OTAPA_SPC_ENABLE                = 0x00000112,
    GSDIDIAG_CDMA_NAM_LOCK                        = 0x00000113,
    GSDIDIAG_CDMA_OTASP_OTAPA_FEATURES            = 0x00000114,
    GSDIDIAG_CDMA_SERVICE_PREF                    = 0x00000115,
    GSDIDIAG_CDMA_ESN_ME                          = 0x00000116,
    GSDIDIAG_CDMA_RUIM_PHASE                      = 0x00000117,
    GSDIDIAG_CDMA_PREF_LANG                       = 0x00000118,
    GSDIDIAG_CDMA_UNASSIGNED_1                    = 0x00000119,
    GSDIDIAG_CDMA_SMS                             = 0x0000011A,
    GSDIDIAG_CDMA_SMS_PARAMS                      = 0x0000011B,
    GSDIDIAG_CDMA_SMS_STATUS                      = 0x0000011C,
    GSDIDIAG_CDMA_SUP_SVCS_FEATURE_CODE_TABLE     = 0x0000011D,
    GSDIDIAG_CDMA_UNASSIGNED_2                    = 0x0000011E,
    GSDIDIAG_CDMA_HOME_SVC_PVDR_NAME              = 0x0000011F,
    GSDIDIAG_CDMA_UIM_ID_USAGE_IND                = 0x00000120,
    GSDIDIAG_CDMA_ADM_DATA                        = 0x00000121,
    GSDIDIAG_CDMA_MSISDN                          = 0x00000122,
    GSDIDIAG_CDMA_MAXIMUM_PRL                     = 0x00000123,
    GSDIDIAG_CDMA_SPC_STATUS                      = 0x00000124,
    GSDIDIAG_CDMA_ECC                             = 0x00000125,
    GSDIDIAG_CDMA_TMSI                            = 0x00000126,
    GSDIDIAG_CDMA_3GPD_ME3GPDOPC                  = 0x00000127,
    GSDIDIAG_CDMA_3GPD_3GPDOPM                    = 0x00000128,
    GSDIDIAG_CDMA_3GPD_SIPCAP                     = 0x00000129,
    GSDIDIAG_CDMA_3GPD_MIPCAP                     = 0x0000012A,
    GSDIDIAG_CDMA_3GPD_SIPUPP                     = 0x0000012B,
    GSDIDIAG_CDMA_3GPD_MIPUPP                     = 0x0000012C,
    GSDIDIAG_CDMA_3GPD_SIPSP                      = 0x0000012D,
    GSDIDIAG_CDMA_3GPD_MIPSP                      = 0x0000012E,
    GSDIDIAG_CDMA_3GPD_SIPPAPSS                   = 0x0000012F,
    GSDIDIAG_CDMA_UNASSIGNED_3                    = 0x00000130,
    GSDIDIAG_CDMA_UNASSIGNED_4                    = 0x00000131,
    GSDIDIAG_CDMA_PUZL                            = 0x00000132,
    GSDIDIAG_CDMA_MAXPUZL                         = 0x00000133,
    GSDIDIAG_CDMA_MECRP                           = 0x00000134,
    GSDIDIAG_CDMA_HRPDCAP                         = 0x00000135,
    GSDIDIAG_CDMA_HRPDUPP                         = 0x00000136,
    GSDIDIAG_CDMA_CSSPR                           = 0x00000137,
    GSDIDIAG_CDMA_ATC                             = 0x00000138,
    GSDIDIAG_CDMA_EPRL                            = 0x00000139,
    GSDIDIAG_CDMA_BCSMS_CONFIG                    = 0x0000013A,
    GSDIDIAG_CDMA_BCSMS_PREF                      = 0x0000013B,
    GSDIDIAG_CDMA_BCSMS_TABLE                     = 0x0000013C,
    GSDIDIAG_CDMA_BCSMS_PARAMS                    = 0x0000013D,
    GSDIDIAG_CDMA_MMS_NOTIF                       = 0x0000013E,
    GSDIDIAG_CDMA_MMS_EXT8                        = 0x0000013F,
    GSDIDIAG_CDMA_MMS_ICP                         = 0x00000140,
    GSDIDIAG_CDMA_MMS_UP                          = 0x00000141,
    GSDIDIAG_CDMA_SMS_CAP                         = 0x00000142,
    GSDIDIAG_CDMA_3GPD_IPV6CAP                    = 0x00000143,
    GSDIDIAG_CDMA_3GPD_MIPFLAGS                   = 0x00000144,
    GSDIDIAG_CDMA_3GPD_TCPCONFIG                  = 0x00000145,
    GSDIDIAG_CDMA_3GPD_DGC                        = 0x00000146,
    GSDIDIAG_CDMA_BROWSER_CP                      = 0x00000147,
    GSDIDIAG_CDMA_BROWSER_BM                      = 0x00000148,
    GSDIDIAG_CDMA_3GPD_SIPUPPEXT                  = 0x00000149,
    GSDIDIAG_CDMA_MMS_CONFIG                      = 0x0000014A,
    GSDIDIAG_CDMA_JAVA_DURL                       = 0x0000014B,
    GSDIDIAG_CDMA_3GPD_MIPUPPEXT                  = 0x0000014C,
    GSDIDIAG_CDMA_BREW_DLOAD                      = 0x0000014D,
    GSDIDIAG_CDMA_BREW_TSID                       = 0x0000014E,
    GSDIDIAG_CDMA_BREW_SID                        = 0x0000014F,
    GSDIDIAG_CDMA_LBS_XCONFIG                     = 0x00000150,
    GSDIDIAG_CDMA_LBS_XSURL                       = 0x00000151,
    GSDIDIAG_CDMA_LBS_V2CONFIG                    = 0x00000152,
    GSDIDIAG_CDMA_LBS_V2PDEADDR                   = 0x00000153,
    GSDIDIAG_CDMA_LBS_V2MPCADDR                   = 0x00000154,
    GSDIDIAG_CDMA_BREW_AEP                        = 0x00000155,
    GSDIDIAG_CDMA_MODEL                           = 0x00000156,
    GSDIDIAG_CDMA_RC                              = 0x00000157,
    GSDIDIAG_CDMA_APP_LABELS                      = 0x00000158,
    GSDIDIAG_CDMA_USER_AGENT_STRING               = 0x00000159,
    GSDIDIAG_CDMA_GID2                            = 0x0000015A,
    GSDIDIAG_CDMA_GID1                            = 0x0000015B,
    GSDIDIAG_CDMA_GID_REFERENCES                  = 0x0000015C,
    GSDIDIAG_CDMA_IMSI_STATUS                     = 0x0000015D,
    GSDIDIAG_CDMA_ME_DOWNLOADABLE_DATA            = 0x0000015E,
    GSDIDIAG_CDMA_ME_SETTING_DATA                 = 0x0000015F,
    GSDIDIAG_CDMA_ME_USER_DATA                    = 0x00000160,
    GSDIDIAG_CDMA_RESERVED2                       = 0x00000161,
    GSDIDIAG_CDMA_RESERVED1                       = 0x00000162,
    GSDIDIAG_CDMA_UIM_SVC_TABLE                   = 0x00000163,
    GSDIDIAG_CDMA_UIM_FEATURES                    = 0x00000164,
    GSDIDIAG_CDMA_UIM_VERSION                     = 0x00000165,
    GSDIDIAG_CDMA_HRPD_HRPDUPP                    = 0x00000166,
    GSDIDIAG_CDMA_SF_EUIM_ID                      = 0x00000167,

    GSDIDIAG_GSM_LP                               = 0x00000200,
    GSDIDIAG_GSM_IMSI                             = 0x00000201,
    GSDIDIAG_GSM_KC                               = 0x00000202,
    GSDIDIAG_GSM_PLMN                             = 0x00000203,
    GSDIDIAG_GSM_HPLMN                            = 0x00000204,
    GSDIDIAG_GSM_ACM_MAX                          = 0x00000205,
    GSDIDIAG_GSM_SST                              = 0x00000206,
    GSDIDIAG_GSM_ACM                              = 0x00000207,
    GSDIDIAG_GSM_GID1                             = 0x00000208,
    GSDIDIAG_GSM_GDI2                             = 0x00000209,
    GSDIDIAG_GSM_SPN                              = 0x0000020A,
    GSDIDIAG_GSM_PUCT                             = 0x0000020B,
    GSDIDIAG_GSM_CBMI                             = 0x0000020C,
    GSDIDIAG_GSM_BCCH                             = 0x0000020D,
    GSDIDIAG_GSM_ACC                              = 0x0000020E,
    GSDIDIAG_GSM_FPLMN                            = 0x0000020F,
    GSDIDIAG_GSM_LOCI                             = 0x00000210,
    GSDIDIAG_GSM_AD                               = 0x00000211,
    GSDIDIAG_GSM_PHASE                            = 0x00000222,
    GSDIDIAG_GSM_VGCS                             = 0x00000223,
    GSDIDIAG_GSM_VGCSS                            = 0x00000224,
    GSDIDIAG_GSM_VBS                              = 0x00000225,
    GSDIDIAG_GSM_VBSS                             = 0x00000226,
    GSDIDIAG_GSM_EMLPP                            = 0x00000227,
    GSDIDIAG_GSM_AAEM                             = 0x00000228,
    GSDIDIAG_GSM_CBMID                            = 0x00000229,
    GSDIDIAG_GSM_ECC                              = 0x0000022A,
    GSDIDIAG_GSM_CBMIR                            = 0x0000022B,
    GSDIDIAG_GSM_DCK                              = 0x0000022C,
    GSDIDIAG_GSM_CNL                              = 0x0000022D,
    GSDIDIAG_GSM_NIA                              = 0x0000022E,
    GSDIDIAG_GSM_KCGPRS                           = 0x0000022F,
    GSDIDIAG_GSM_LOCIGPRS                         = 0x00000230,
    GSDIDIAG_GSM_SUME                             = 0x00000231,
    GSDIDIAG_GSM_PLMNWAT                          = 0x00000232,
    GSDIDIAG_GSM_OPLMNWACT                        = 0x00000233,
    GSDIDIAG_GSM_CPBCCH                           = 0x00000234,
    GSDIDIAG_GSM_INVSCAN                          = 0x00000235,
    GSDIDIAG_GSM_RPLMNACT                         = 0x00000236,
    GSDIDIAG_GSM_SAI                              = 0x00000237,
    GSDIDIAG_GSM_SLL                              = 0x00000238,
    GSDIDIAG_GSM_MEXE_ST                          = 0x00000239,
    GSDIDIAG_GSM_ORPK                             = 0x0000023A,
    GSDIDIAG_GSM_ARPK                             = 0x0000023B,
    GSDIDIAG_GSM_TPRPK                            = 0x0000023C,
    GSDIDIAG_GSM_IMG                              = 0x0000023D,
    GSDIDIAG_GSM_IMG1INST1                        = 0x0000023E,
    GSDIDIAG_GSM_IMG1INST2                        = 0x0000023F,
    GSDIDIAG_GSM_IMG1INST3                        = 0x00000240,
    GSDIDIAG_GSM_IMG2INST1                        = 0x00000241,
    GSDIDIAG_GSM_IMG2INST2                        = 0x00000242,
    GSDIDIAG_GSM_IMG2INST3                        = 0x00000243,
    GSDIDIAG_GSM_IMG3INST1                        = 0x00000244,
    GSDIDIAG_GSM_IMG3INST2                        = 0x00000245,
    GSDIDIAG_GSM_IMG3INST3                        = 0x00000246,
    GSDIDIAG_GSM_HPLMNWACT                        = 0x00000247,
    GSDIDIAG_GSM_PNN                              = 0x00000248,
    GSDIDIAG_GSM_OPL                              = 0x00000249,
    GSDIDIAG_GSM_HPLMNACT                         = 0x0000024A,
    GSDIDIAG_GSM_MBDN                             = 0x0000024B,
    GSDIDIAG_GSM_EXT6                             = 0x0000024C,
    GSDIDIAG_GSM_MBI                              = 0x0000024D,
    GSDIDIAG_GSM_MWIS                             = 0x0000024E,
    GSDIDIAG_GSM_EXT1                             = 0x0000024F,
    GSDIDIAG_GSM_SPDI                             = 0x00000250,
    GSDIDIAG_GSM_CFIS                             = 0x00000251,

    GSDIDIAG_TELECOM_ADN                          = 0x00000300,
    GSDIDIAG_TELECOM_FDN                          = 0x00000301,
    GSDIDIAG_TELECOM_SMS                          = 0x00000302,
    GSDIDIAG_TELECOM_CCP                          = 0x00000303,
    GSDIDIAG_TELECOM_ECCP                         = 0x00000304,
    GSDIDIAG_TELECOM_MSISDN                       = 0x00000305,
    GSDIDIAG_TELECOM_SMSP                         = 0x00000306,
    GSDIDIAG_TELECOM_SMSS                         = 0x00000307,
    GSDIDIAG_TELECOM_LND                          = 0x00000308,
    GSDIDIAG_TELECOM_SDN                          = 0x00000309,
    GSDIDIAG_TELECOM_EXT1                         = 0x0000030A,
    GSDIDIAG_TELECOM_EXT2                         = 0x0000030B,
    GSDIDIAG_TELECOM_EXT3                         = 0x0000030C,
    GSDIDIAG_TELECOM_BDN                          = 0x0000030D,
    GSDIDIAG_TELECOM_EXT4                         = 0x0000030E,
    GSDIDIAG_TELECOM_SMSR                         = 0x0000030F,
    GSDIDIAG_TELECOM_CMI                          = 0x00000310,
    GSDIDIAG_TELECOM_SUME                         = 0x00000311,
    GSDIDIAG_TELECOM_ARR                          = 0x00000312,
    GSDIDIAG_TELECOM_PBR                          = 0x00000313,
    GSDIDIAG_TELECOM_CCP1                         = 0x00000314,
    GSDIDIAG_TELECOM_UID                          = 0x00000315,
    GSDIDIAG_TELECOM_PSC                          = 0x00000316,
    GSDIDIAG_TELECOM_CC                           = 0x00000317,
    GSDIDIAG_TELECOM_PUID                         = 0x00000318,
    GSDIDIAG_TELECOM_ADN1                         = 0x00000319,
    GSDIDIAG_TELECOM_GRP                          = 0x0000031A,
    GSDIDIAG_TELECOM_GRP1                         = 0x0000031B,
    GSDIDIAG_TELECOM_GAS                          = 0x0000031C,
    GSDIDIAG_TELECOM_SNE                          = 0x0000031D,
    GSDIDIAG_TELECOM_SNE1                         = 0x0000031E,
    GSDIDIAG_TELECOM_EMAIL                        = 0x0000031F,
    GSDIDIAG_TELECOM_EMAIL1                       = 0x00000320,
    GSDIDIAG_TELECOM_IAP                          = 0x00000321,
    GSDIDIAG_TELECOM_IAP1                         = 0x00000322,
    GSDIDIAG_TELECOM_PBC                          = 0x00000323,
    GSDIDIAG_TELECOM_ANR                          = 0x00000324,
    GSDIDIAG_TELECOM_ANRA                         = 0x00000325,
    GSDIDIAG_TELECOM_ANRB                         = 0x00000326,
    GSDIDIAG_TELECOM_ANRC                         = 0x00000327,
    GSDIDIAG_TELECOM_ANR1                         = 0x00000328,
    GSDIDIAG_TELECOM_ANRA1                        = 0x00000329,
    GSDIDIAG_TELECOM_ANRB1                        = 0x00000330,
    GSDIDIAG_TELECOM_ANRC1                        = 0x00000331,

    GSDIDIAG_TELECOM_ADN2                         = 0x00000332,
    GSDIDIAG_TELECOM_ANR2                         = 0x00000333,
    GSDIDIAG_TELECOM_ANRA2                        = 0x00000334,
    GSDIDIAG_TELECOM_ANRB2                        = 0x00000335,
    GSDIDIAG_TELECOM_ANRC2                        = 0x00000336,
    GSDIDIAG_TELECOM_EMAIL2                       = 0x00000337,
    GSDIDIAG_TELECOM_GRP2                         = 0x00000338,
    GSDIDIAG_TELECOM_SNE2                         = 0x00000339,
    GSDIDIAG_TELECOM_IAP2                         = 0x00000340,
    GSDIDIAG_TELECOM_UID2                         = 0x00000341,
    GSDIDIAG_TELECOM_PBC2                         = 0x00000342,

    GSDIDIAG_TELECOM_ADN3                         = 0x00000343,
    GSDIDIAG_TELECOM_ANR3                         = 0x00000344,
    GSDIDIAG_TELECOM_ANRA3                        = 0x00000345,
    GSDIDIAG_TELECOM_ANRB3                        = 0x00000346,
    GSDIDIAG_TELECOM_ANRC3                        = 0x00000347,
    GSDIDIAG_TELECOM_EMAIL3                       = 0x00000348,
    GSDIDIAG_TELECOM_GRP3                         = 0x00000349,
    GSDIDIAG_TELECOM_SNE3                         = 0x00000350,
    GSDIDIAG_TELECOM_IAP3                         = 0x00000351,
    GSDIDIAG_TELECOM_UID3                         = 0x00000352,
    GSDIDIAG_TELECOM_PBC3                         = 0x00000353,

    GSDIDIAG_TELECOM_GRAPHICS_IMG                 = 0x00000400,

    GSDIDIAG_USIM_LI                              = 0x00000500,
    GSDIDIAG_USIM_IMSI                            = 0x00000501,
    GSDIDIAG_USIM_KEYS                            = 0x00000502,
    GSDIDIAG_USIM_KEYSPS                          = 0x00000503,
    GSDIDIAG_USIM_PLMNWACT                        = 0x00000504,
    GSDIDIAG_USIM_UPLMNSEL                        = 0x00000505,
    GSDIDIAG_USIM_HPLMN                           = 0x00000506,
    GSDIDIAG_USIM_ACM_MAX                         = 0x00000507,
    GSDIDIAG_USIM_UST                             = 0x00000508,
    GSDIDIAG_USIM_ACM                             = 0x00000509,
    GSDIDIAG_USIM_GID1                            = 0x0000050A,
    GSDIDIAG_USIM_GID2                            = 0x0000050B,
    GSDIDIAG_USIM_SPN                             = 0x0000050C,
    GSDIDIAG_USIM_PUCT                            = 0x0000050D,
    GSDIDIAG_USIM_CMBI                            = 0x0000050E,
    GSDIDIAG_USIM_ACC                             = 0x0000050F,
    GSDIDIAG_USIM_FPLMN                           = 0x00000510,
    GSDIDIAG_USIM_LOCI                            = 0x00000511,
    GSDIDIAG_USIM_AD                              = 0x00000512,
    GSDIDIAG_USIM_CBMID                           = 0x00000513,
    GSDIDIAG_USIM_ECC                             = 0x00000514,
    GSDIDIAG_USIM_CBMIR                           = 0x00000515,
    GSDIDIAG_USIM_PSLOCI                          = 0x00000516,
    GSDIDIAG_USIM_FDN                             = 0x00000517,
    GSDIDIAG_USIM_SMS                             = 0x00000518,
    GSDIDIAG_USIM_MSISDN                          = 0x00000519,
    GSDIDIAG_USIM_SMSP                            = 0x0000051A,
    GSDIDIAG_USIM_SMSS                            = 0x0000051B,
    GSDIDIAG_USIM_SDN                             = 0x0000051C,
    GSDIDIAG_USIM_EXT2                            = 0x0000051D,
    GSDIDIAG_USIM_EXT3                            = 0x0000051E,
    GSDIDIAG_USIM_SMSR                            = 0x0000051F,
    GSDIDIAG_USIM_ICI                             = 0x00000520,
    GSDIDIAG_USIM_OCI                             = 0x00000521,
    GSDIDIAG_USIM_ICT                             = 0x00000522,
    GSDIDIAG_USIM_OCT                             = 0x00000523,
    GSDIDIAG_USIM_EXT5                            = 0x00000524,
    GSDIDIAG_USIM_CCP2                            = 0x00000525,
    GSDIDIAG_USIM_EMLPP                           = 0x00000526,
    GSDIDIAG_USIM_AAEM                            = 0x00000527,
    GSDIDIAG_USIM_GMSI                            = 0x00000528,
    GSDIDIAG_USIM_HIDDENKEY                       = 0x00000529,
    GSDIDIAG_USIM_BDN                             = 0x0000052A,
    GSDIDIAG_USIM_EXT4                            = 0x0000052B,
    GSDIDIAG_USIM_CMI                             = 0x0000052C,
    GSDIDIAG_USIM_EST                             = 0x0000052D,
    GSDIDIAG_USIM_ACL                             = 0x0000052E,
    GSDIDIAG_USIM_DCK                             = 0x0000052F,
    GSDIDIAG_USIM_CNL                             = 0x00000530,
    GSDIDIAG_USIM_START_HFN                       = 0x00000531,
    GSDIDIAG_USIM_THRESHOLD                       = 0x00000532,
    GSDIDIAG_USIM_OPLMNWACT                       = 0x00000533,
    GSDIDIAG_USIM_OPLMNSEL                        = 0x00000534,
    GSDIDIAG_USIM_HPLMNWACT                       = 0x00000535,
    GSDIDIAG_USIM_ARR                             = 0x00000536,
    GSDIDIAG_USIM_RPLMNACT                        = 0x00000537,
    GSDIDIAG_USIM_NETPAR                          = 0x00000538,
    GSDIDIAG_USIM_ADN                             = 0x00000539,
    GSDIDIAG_USIM_ADN1                            = 0x0000053A,
    GSDIDIAG_USIM_GAS                             = 0x0000053B,
    GSDIDIAG_USIM_GAS1                            = 0x0000053C,
    GSDIDIAG_USIM_GRP1                            = 0x0000053D,
    GSDIDIAG_USIM_SNE                             = 0x0000053E,
    GSDIDIAG_USIM_SNE1                            = 0x0000053F,
    GSDIDIAG_USIM_EMAIL                           = 0x00000540,
    GSDIDIAG_USIM_EMAIL1                          = 0x00000541,
    GSDIDIAG_USIM_IAP                             = 0x00000542,
    GSDIDIAG_USIM_IAP1                            = 0x00000543,
    GSDIDIAG_USIM_PNN                             = 0x00000544,
    GSDIDIAG_USIM_OPL                             = 0x00000545,
    GSDIDIAG_USIM_EHPLMN                          = 0x00000546,
    GSDIDIAG_USIM_MBDN                            = 0x00000547,
    GSDIDIAG_USIM_EXT6                            = 0x00000548,
    GSDIDIAG_USIM_MBI                             = 0x00000549,
    GSDIDIAG_USIM_MWIS                            = 0x0000054A,
    GSDIDIAG_USIM_SPDI                            = 0x0000054B,
    GSDIDIAG_USIM_SPT_TABLE                       = 0x0000054C,
    GSDIDIAG_USIM_CFIS                            = 0x0000054D,
    GSDIDIAG_USIM_ANR                             = 0x0000054E,
    GSDIDIAG_USIM_ANRA                            = 0x0000054F,
    GSDIDIAG_USIM_ANRB                            = 0x00000550,
    GSDIDIAG_USIM_ANRC                            = 0x00000551,
    GSDIDIAG_USIM_ANR1                            = 0x00000552,
    GSDIDIAG_USIM_ANRA1                           = 0x00000553,
    GSDIDIAG_USIM_ANRB1                           = 0x00000554,
    GSDIDIAG_USIM_ANRC1                           = 0x00000555,

    GSDIDIAG_USIM_ADN2                            = 0x00000556,
    GSDIDIAG_USIM_ANR2                            = 0x00000557,
    GSDIDIAG_USIM_ANRA2                           = 0x00000558,
    GSDIDIAG_USIM_ANRB2                           = 0x00000559,
    GSDIDIAG_USIM_ANRC2                           = 0x00000560,
    GSDIDIAG_USIM_EMAIL2                          = 0x00000561,
    GSDIDIAG_USIM_GRP2                            = 0x00000562,
    GSDIDIAG_USIM_SNE2                            = 0x00000563,
    GSDIDIAG_USIM_IAP2                            = 0x00000564,
    GSDIDIAG_USIM_UID2                            = 0x00000565,
    GSDIDIAG_USIM_PBC2                            = 0x00000567,

    GSDIDIAG_USIM_ADN3                            = 0x00000568,
    GSDIDIAG_USIM_ANR3                            = 0x00000569,
    GSDIDIAG_USIM_ANRA3                           = 0x00000570,
    GSDIDIAG_USIM_ANRB3                           = 0x00000571,
    GSDIDIAG_USIM_ANRC3                           = 0x00000572,
    GSDIDIAG_USIM_EMAIL3                          = 0x00000573,
    GSDIDIAG_USIM_GRP3                            = 0x00000574,
    GSDIDIAG_USIM_SNE3                            = 0x00000575,
    GSDIDIAG_USIM_IAP3                            = 0x00000576,
    GSDIDIAG_USIM_UID3                            = 0x00000577,
    GSDIDIAG_USIM_PBC3                            = 0x00000578,
    GSDIDIAG_USIM_RAT                             = 0X00000579,

    /* EFs for MMS */
    GSDIDIAG_USIM_MMSN                            = 0x0000057A,
    GSDIDIAG_USIM_MMSICP                          = 0x0000057B,
    GSDIDIAG_USIM_MMSUP                           = 0x0000057C,
    GSDIDIAG_USIM_MMSUCP                          = 0x0000057D,
    GSDIDIAG_USIM_EXT8                            = 0x0000057E,

    GSDIDIAG_USIM_PHONEBOOK_PBR                   = 0x00000600,
    GSDIDIAG_USIM_PHONEBOOK_CCP1                  = 0x00000601,
    GSDIDIAG_USIM_PHONEBOOK_UID                   = 0x00000602,
    GSDIDIAG_USIM_PHONEBOOK_PSC                   = 0x00000603,
    GSDIDIAG_USIM_PHONEBOOK_CC                    = 0x00000604,
    GSDIDIAG_USIM_PHONEBOOK_PUID                  = 0x00000605,
    GSDIDIAG_USIM_PHONEBOOK_PBC                   = 0x00000606,

    GSDIDIAG_USIM_GSMACCESS_KC                    = 0x00000700,
    GSDIDIAG_USIM_GSMACCESS_KCGPRS                = 0x00000701,
    GSDIDIAG_USIM_GSMACCESS_CPBCCH                = 0x00000702,
    GSDIDIAG_USIM_GSMACCESS_INVSCAN               = 0x00000703,

    GSDIDIAG_USIM_MEXE_MEXE_ST                    = 0x00000800,
    GSDIDIAG_USIM_MEXE_ORPK                       = 0x00000801,
    GSDIDIAG_USIM_MEXE_ARPK                       = 0x00000802,
    GSDIDIAG_USIM_MEXE_TPRPK                      = 0x00000803,

    GSDIDIAG_UIM_ISIM_IMPI                        = 0X00000900,
    GSDIDIAG_UIM_ISIM_DOMAIN                      = 0X00000901,
    GSDIDIAG_UIM_ISIM_IMPU                        = 0X00000902,
    GSDIDIAG_UIM_ISIM_ARR                         = 0X00000903,
    GSDIDIAG_UIM_ISIM_KEYS                        = 0X00000904,
    GSDIDIAG_UIM_ISIM_AD                          = 0X00000905,

    /* EFs for PKCS */
    GSDIDIAG_PKCS15_ODF                           = 0x00000A00,
    GSDIDIAG_PKCS15_PRKDF                         = 0x00000A01,
    GSDIDIAG_PKCS15_PUKDF                         = 0x00000A02,
    GSDIDIAG_PKCS15_SKDF                          = 0x00000A03,
    GSDIDIAG_PKCS15_CDF                           = 0x00000A04,
    GSDIDIAG_PKCS15_DODF                          = 0x00000A05,
    GSDIDIAG_PKCS15_AODF                          = 0x00000A06,
    GSDIDIAG_PKCS15_TI                            = 0x00000A07,
    GSDIDIAG_PKCS15_US                            = 0x00000A09,

  GSDIDIAG_MAX_FILE_ENUM                        = 0x7FFFFFFF
  //reserved
} gsdidiag_elementary_file_enum_type;


typedef struct
{
  gsdidiag_elementary_file_enum_type gsdidiag_file;
  uim_items_enum_type                uim_file;
  mmgsdi_file_enum_type              mmgsdi_file;
} gsdidiag_file_to_uim_map;

/*============================================================================
    Enumerated type:  gsdidiag_uim_mc_action_type
    List of available manage channel command options
============================================================================*/

typedef enum {
  GSDIDIAG_UIM_MC_OPEN  =0,    /* Manage Channel Open  */
  GSDIDIAG_UIM_MC_CLOSE =1,   /* Manage Channel Close */
  GSDIDIAG_MAX_MC       =2,         /* Maximum              */

  /* Dummy to extend this enum to an int always */
  GSDIDIAG_DUMMY_UIM_MC_TO_INT = MAX_INT
}gsdidiag_uim_mc_action_type;


/*============================================================================
    Type definition for gsdidiag_gsdi_protocol_uim_protocol_pair_type
    PURPOSE   Translation between protocol exported from the DIAG to
    the protocol type in UIM
============================================================================*/
typedef struct
{
  gsdidiag_uim_mc_action_type gsdidiag_uim_mc;
  uim_mc_action_type          uim_mc;
}gsdidiag_gsdi_mc_uim_mc_pair_type;


/*============================================================================
    Enumerated type:  gsdidiag_uim_access_type
    List of available tyypes of access for the access command
============================================================================*/
typedef enum {
  GSDIDIAG_UIM_READ_F  =0,   /* Read item from UIM */
  GSDIDIAG_UIM_WRITE_F =1,  /* Write item to UIM */
  GSDIDIAG_UIM_INC_F   =2,    /* Increment item in UIM */
  GSDIDIAG_MAX_ACCESS,   /* Maximum */

  /* Dummy to extend this enum to an int always */
  GSDIDIAG_DUMMY_UIM_ACCESS_TO_INT = MAX_INT
}gsdidiag_uim_access_type;

/*============================================================================
    Type definition for gsdidiag_gsdi_protocol_uim_protocol_pair_type
    PURPOSE   Translation between protocol exported from the DIAG to
    the protocol type in UIM
============================================================================*/
typedef struct
{
  gsdidiag_uim_access_type    gsdidiag_access;
  uim_access_type             uim_access;
}gsdidiag_gsdi_access_uim_access_pair_type;


/*============================================================================
    Enumerated type:  gsdidiag_uim_record_mode_type
    List of available types of record access modes
============================================================================*/
typedef enum {
  GSDIDIAG_UIM_CURRENT    = 0, /* Current mode */
  GSDIDIAG_UIM_NEXT       = 1, /* access the next record in a record structure */
  GSDIDIAG_UIM_PREVIOUS   = 2, /* access the previous record in a record structure */
  GSDIDIAG_UIM_ABSOLUTE   = 3,  /* Absolute mode of record access */
  GSDIDIAG_MAX_RECORD_MODE,

  /* Dummy to extend this enum to an int always */
  GSDIDIAG_DUMMY_UIM_REC_MODE_TO_INT = MAX_INT
} gsdidiag_uim_record_mode_type;

/*============================================================================
    Type definition for gsdidiag_gsdi_protocol_uim_protocol_pair_type
    PURPOSE   Translation between protocol exported from the DIAG to
    the protocol type in UIM
============================================================================*/
typedef struct
{
  gsdidiag_uim_record_mode_type    gsdidiag_rec_mode;
  uim_record_mode_type             uim_record_mode;
}gsdidiag_gsdi_rec_mode_uim_rec_mode_pair_type;


/*============================================================================
PURPOSE  To supply parameters for a manage channel command
============================================================================*/

typedef PACKED struct
{
  int32                       op_close_channel;/* Channel to open or close */
  gsdidiag_uim_mc_action_type mc_action;   /* Manage Channel Open or Close */
}gsdidiag_manage_ch_req_type;



/*============================================================================
PURPOSE  To supply parameters for a read request
============================================================================*/
typedef PACKED struct
{
  gsdidiag_elementary_file_enum_type item;  /* Item to be accessed */
  gsdidiag_uim_access_type           access;/* Type of access, Read or Write */
  gsdidiag_uim_record_mode_type      rec_mode; /* Record Mode for record access */
  int32   num_bytes;      /* Number of bytes to be read or written or */
  int32   offset;         /* Offset into the EF */
  word    path[UIM_MAX_PATH_ELEMENTS];
}gsdidiag_uim_access_req_type;


/*============================================================================
PURPOSE  To supply parameters to an ISIM authenticate
============================================================================*/
typedef PACKED struct
{
  int32  auth_context; /* Authentication context place holder if need be*/
  int32  num_bytes;               /* Number of data bytes */
  byte   *data;     /* Application specific Input */
}gsdidiag_uim_isim_autn_req_type;

/*============================================================================
PURPOSE  To supply parameters to Read command
============================================================================*/
typedef PACKED struct
{
  gsdidiag_elementary_file_enum_type item;  /* Item to be accessed */
  word    path[UIM_MAX_PATH_ELEMENTS];

  int32   offset;         /* Offset into the EF */
  int32   num_bytes;      /* Number of bytes to be read*/
}gsdidiag_uim_read_bin_req_type;

/*============================================================================
PURPOSE  To supply parameters to Read command
============================================================================*/
typedef PACKED struct
{
  gsdidiag_elementary_file_enum_type item;  /* Item to be accessed */
  word    path[UIM_MAX_PATH_ELEMENTS];

  gsdidiag_uim_record_mode_type      rec_mode; /* Record Mode for record access */
  int32                              rec_num;  /* Record number to be read*/

}gsdidiag_uim_read_rec_req_type;


/*============================================================================
PURPOSE  To supply parameters to Write Binary
============================================================================*/
typedef PACKED struct
{
  gsdidiag_elementary_file_enum_type item;  /* Item to be accessed */
  word    path[UIM_MAX_PATH_ELEMENTS];

  int32   offset;         /* Offset into the EF */
  int32   len;      /* Number of bytes to be written*/
  byte   *data;           /* Data to be written */
}gsdidiag_uim_write_bin_req_type;


/*============================================================================
PURPOSE  To supply parameters to INC
============================================================================*/
typedef PACKED struct
{
  gsdidiag_elementary_file_enum_type item;  /* Item to be accessed */
  word    path[UIM_MAX_PATH_ELEMENTS];

  int32   len;      /* Number of bytes to be written*/
  byte   *data;           /* Data to be written */
}gsdidiag_uim_inc_req_type;



/*============================================================================
PURPOSE  To supply parameters to write record
============================================================================*/
typedef PACKED struct
{
  gsdidiag_elementary_file_enum_type item;  /* Item to be accessed */
  word    path[UIM_MAX_PATH_ELEMENTS];

  gsdidiag_uim_record_mode_type      rec_mode; /* Record Mode for record access */
  int32                              rec_num;  /* Record number to be read*/

  int32   len; /* number of bytes to be written */
  byte   *data; /* data to be written */
}gsdidiag_uim_write_rec_req_type;


/*============================================================================
typedef gsdidiag_uim_cmd_req_type
         int32 gsdidiag_uim_protocol : Protocol for the UIM command
         int32 gsdidiag_uim_cmd      : gsdidiag_uim_cmd_type command equalent
                                       for the UIM command to be executed.
                                       We use this type instead of exposing
                                       the uim_cmd_name_type
PURPOSE  Main type-def for all the UIM commands.
============================================================================*/

typedef PACKED struct
{
  gsdidiag_uim_protocol_type gsdidiag_uim_protocol;
  gsdidiag_uim_cmd_type      gsdidiag_uim_cmd;

  uint32 user_data;
  int32  logical_channel;

  PACKED union
  {
    gsdidiag_manage_ch_req_type     mng_ch_req;
    gsdidiag_uim_access_req_type    access_req;
    gsdidiag_uim_isim_autn_req_type isim_autn_req;

    gsdidiag_uim_read_bin_req_type  read_bin;
    gsdidiag_uim_read_rec_req_type  read_rec;

    gsdidiag_uim_write_bin_req_type write_bin;
    gsdidiag_uim_write_rec_req_type write_rec;

    gsdidiag_uim_inc_req_type       inc;
  }params;
} gsdidiag_uim_cmd_req_type;

/*============================================================================
typedef uim_atr_rsp_type
  byte num_atr_bytes                : Length of the ATR bytes received
  byte atr_bytes[UIM_MAX_ATR_CHARS] : The ATR bytes

PURPOSE  Response when the uim CMD is UIM_RESET_SWITCH_UIM_PASSIVE_F

REQUEST   gsdidiag_uim_cmd_req_type.
============================================================================*/
typedef PACKED struct
{
  byte num_atr_bytes;
  byte atr_bytes[UIM_MAX_ATR_CHARS];
}gsdidiag_uim_atr_rsp_type;

/*============================================================================
PURPOSE  Response when the uim CMD is UIM_MANAGE_CHANNEL_F

REQUEST   gsdidiag_uim_cmd_req_type.
============================================================================*/
typedef PACKED struct
{
  int32 channel;
}gsdidiag_uim_manage_channel_rsp;

/*============================================================================
PURPOSE  Response when the uim CMD is an ACCESS

REQUEST   gsdidiag_uim_cmd_req_type.
============================================================================*/
typedef PACKED struct
{
  int32 num_response_bytes;
  byte  data[GSDI_MAX_DATA_BLOCK_LEN * 2];
  int32 num_records_rsp;
}gsdidiag_uim_access_rsp_type;

/*============================================================================
PURPOSE  Response when the uim CMD is an ISIM authenticate

REQUEST   gsdidiag_uim_cmd_req_type.
============================================================================*/
typedef PACKED struct
{
  int32 data_length;            /* Length of data returned */
  byte  data[UIM_MAX_CHARS];    /* Data returned */
}gsdidiag_uim_isim_autn_rsp_type;

/*============================================================================
PURPOSE  Response when the uim CMD is an read binary or read record

REQUEST   gsdidiag_uim_cmd_req_type.
============================================================================*/
typedef PACKED struct
{
  int32 data_length;            /* Length of data returned */
  byte  data[MAX_UIM_DATA_LEN]; /* Data returned (max of 800 bytes )*/
}gsdidiag_uim_read_rsp_type;

/*============================================================================
PURPOSE  Response when the uim CMD is an INCREASE
REQUEST   gsdidiag_uim_cmd_req_type.
============================================================================*/
typedef PACKED struct
{
  int32 data_length;            /* Length of data returned */
  byte  data[MAX_UIM_DATA_LEN]; /* Data returned (max of 800 bytes )*/
}gsdidiag_uim_inc_rsp_type;


/*============================================================================
PURPOSE  Response when the uim CMD is an write binary or write record

REQUEST   gsdidiag_uim_cmd_req_type.
============================================================================*/
typedef PACKED struct
{
  int32 bytes_written;            /* Length of data returned */
}gsdidiag_uim_write_rsp_type;

/*============================================================================
PURPOSE  Main type-def for response to UIM commands.
    int32  rpt_status  :  PASS or FAIL information
    union uim_response : Union of all possible responses

REQUEST   gsdidiag_uim_cmd_req_type.
============================================================================*/
typedef PACKED struct
{
    int32                      rpt_status;
    gsdidiag_uim_rpt_code_type rpt_type;
    int32                      sw1;
    int32                      sw2;
    int32                      cmd_transacted;

    PACKED union
    {
      gsdidiag_uim_atr_rsp_type           atr_response;
      gsdidiag_uim_manage_channel_rsp     manage_channel_response;
      gsdidiag_uim_access_rsp_type        uim_access_rsp;
      gsdidiag_uim_isim_autn_rsp_type     uim_isim_autn_rsp;

      gsdidiag_uim_read_rsp_type          uim_read_rsp;
      gsdidiag_uim_write_rsp_type         uim_write_rsp;

      gsdidiag_uim_inc_rsp_type           inc_rsp;
      /* Keep adding datatypes for each new UIM command */
    }uim_response;
}gsdidiag_rsp_uim_cmd_type;


/*===========================================================================
FUNCTION gsdidiag_get_uim_cmd_payload

DESCRIPTION
  Populates the request structures with the data from the request packet.

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
extern void gsdidiag_get_uim_cmd_payload(
  gsdidiag_uim_cmd_req_type *req_ptr,
  PACKED void * req,
  uint32 offset
);

/*===========================================================================
FUNCTION gsdidiag_free_uim_req_mem

DESCRIPTION
  Frees up any memory that might be allocated memory based on the command type

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
extern void gsdidiag_free_uim_req_mem(
  gsdidiag_uim_cmd_req_type *uim_req_ptr
);

/*===========================================================================
FUNCTION gsdidiag_populate_uim_cmd

DESCRIPTION
  Populates the UIM command pointer with the information from the request
  based on the command.  Memory is allocated to the supplied pointer
  for specific commands that need memory allocation

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/

gsdi_returns_T gsdidiag_populate_uim_cmd(
  uim_cmd_type *uim_cmd_ptr,
  const gsdidiag_uim_cmd_req_type  *req_ptr,
  void **data_ptr,
  uint8 slot_id,
  uint8 app_id_len,
  const uint8 *app_id
);

/*===========================================================================
FUNCTION gsdidiag_populate_uim_cmd_rsp

DESCRIPTION
  Populates the uim_response pointer based on the information received
  in the call back to the UIM command.  The information is filled in
  based on the type of response received.

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
extern void gsdidiag_populate_uim_cmd_rsp(
  gsdidiag_rsp_uim_cmd_type  *uim_rsp_ptr,
  const uim_rpt_type* uim_data,
  const uim_cmd_type* uim_cmd_ptr
);



/*===========================================================================
FUNCTION gsdidiag_convert_file_to_uim_enum()

DESCRIPTION
    Search through the mapping table and return the corresponding
    uim_items_enum_type based on the passed in gsdidiag_elementary_file_enum_type

DEPENDENCIES
    None

RETURN VALUE
    uim_items_enum_type

SIDE EFFECTS
    None
===========================================================================*/

extern uim_items_enum_type gsdidiag_convert_file_to_uim_enum(
  gsdidiag_elementary_file_enum_type gsdidiag_file
);

extern gsdidiag_elementary_file_enum_type gsdidiag_convert_uim_file_to_gsdidiag_file(
  uint16 uim_file);

extern gsdidiag_elementary_file_enum_type gsdidiag_convert_file_from_mmgsdi_enum(
  mmgsdi_file_enum_type mmgsdi_file);

extern mmgsdi_file_enum_type gsdidiag_convert_file_to_mmgsdi_enum(
  gsdidiag_elementary_file_enum_type gsdidiag_file);

#endif
