/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   SIM/USIM  Diagnostic Interface to
                    PIPE commands to the UIM task


Supports automated testing of SIM/USIM/RUIM cards via DIAG commands.
Implements packet processing.

The following are some simple guide-lines to add new commands to
this diag module

*) Add the new command to gsdidiag_uim_cmd_type
*) Have an entry in gsdidiag_uim_cmd_uim_cmd_pair_type
*) Have the data types in gsdidiag_uim_cmd_req_type
*) Have the data types in gsdidiag_rsp_uim_cmd_type
*) Implement gsdidiag_get_uim_cmd_payload to get the information out of the
   packed structure to local command structures
*) gsdidiag_populate_uim_cmd has to handle this new command to populate
   the uim_cmd from the local command request
*) gsdidiag_populate_uim_cmd_rsp has to pupulate the response
*) Clear any memory allocated to the req_ptr in gsdidiag_free_uim_req_mem



Copyright (c) 2003-2009 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //source/qcom/qct/modem/uim/mmgsdi/rel/07H1_2/src/gsdidiag_uim.c#2 $ $DateTime: 2009/04/29 16:02:05 $ $Author: jsharma $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/29/09   js      Added prototype for static functions, fixed compiler warnings
08/08/08   vs      Fixed compile errors related to featurization
06/21/08   kk      Added PKCS15 Support
04/30/08   kk      Added support for MMS EFs
03/20/08   nk      Added OMH EF Support
03/17/08   nk      Fixed Klocwork issues.
11/19/07   js      Radio Access Technology Balancing support
11/08/07   vs      Added OMH feature support
05/15/07   sp      Featurized phonebook  
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
04/14/07   sp      Fixed buffer overflow
02/09/07   sk      Added support for 1000 phonebook entries.
02/05/07   pv      Avoid possibility of using NULL uim_cmd_ptr->write_rec.data.
                   Rectify the check for allocating memory to
                   uim_cmd_ptr->write_rec.data
08/23/06   sun     Added support for CDMA_ECC
08/16/06   jar     Added Support for ANR Files
06/29/06   tml     Compilation warning fix
06/15/06   tml     Fixed compilation warning
04/05/06   tml     lint + NO FILE ENUM change
03/21/06   tml     added cfis and missing service table support and lint
02/16/06   sp      Support for EF_EHPLMN
11/17/05   tml     PNN and OPL support
11/09/05 sun/tml   lint fixes
10/25/05   tml     Added ISIM File supports
05/13/05    pv     Added additional commands, fixed some data types
04/13/05    pv     Initial version with UIM_RESET_SWITCH_UIM_PASSIVE
                   UIM_RESET_WITH_TP_F & UIM_RESET_F, P_UP & P_DOWN commands
===========================================================================*/
#include "intconv.h"
#include "comdef.h"
#ifdef FEATURE_MMGSDI_TEST_AUTOMATION
#include "customer.h"
#include "diagpkt.h"
#include "string.h"
#include "msg.h"
#include "gsdidiag_uim.h"
#include "gsdi_exp.h"
#include "gsdi.h"
#include "event.h" /* event_report function */
#include "fs.h"   /* fs_open and fs_close */
#include "tmc.h"  /* tmc(task main controller) heap used for memory allcation*/
#include "gsdi_efs.h"
#include "mmgsdi.h"
#include "mmgsdiutil.h"

#if defined (FEATURE_MMGSDI_GSM ) || defined (FEATURE_MMGSDI_UMTS)
#error code not present
#endif /*FEATURE_GSM || UMTS */

/*=============================================================================
    Static table containing mapping between user specified elementary file
    to gsdi task internal values.
=============================================================================*/
static gsdidiag_file_to_uim_map gsdidiag_file_to_uim_table[] =
{
    /* EFs at MF Level */
    {GSDIDIAG_MF_ICCID, UIM_ICCID, MMGSDI_ICCID                    },
    {GSDIDIAG_MF_ELP, UIM_ELP, MMGSDI_ELP                          },
    {GSDIDIAG_MF_DIR, UIM_DIR, MMGSDI_DIR                          },
    {GSDIDIAG_MF_ARR, UIM_ARR, MMGSDI_ARR                          },

    /* EFs at CDMA DF Level */
    {GSDIDIAG_CDMA_CC, UIM_CDMA_CC, MMGSDI_CDMA_CC                 },
    {GSDIDIAG_CDMA_IMSI_M, UIM_CDMA_IMSI_M, MMGSDI_CDMA_IMSI_M     },
    {GSDIDIAG_CDMA_IMSI_T, UIM_CDMA_IMSI_T, MMGSDI_CDMA_IMSI_T     },
    {GSDIDIAG_CDMA_ANALOG_HOME_SID, UIM_CDMA_ANALOG_HOME_SID,
        MMGSDI_CDMA_ANALOG_HOME_SID                                },
    {GSDIDIAG_CDMA_ANALOG_OP_PARAMS, UIM_CDMA_ANALOG_OP_PARAMS,
        MMGSDI_CDMA_ANALOG_OP_PARAMS                               },
    {GSDIDIAG_CDMA_ANALOG_LOCN_AND_REGN_IND, UIM_CDMA_ANALOG_LOCN_AND_REGN_IND,
        MMGSDI_CDMA_ANALOG_LOCN_AND_REGN_IND                       },
    {GSDIDIAG_CDMA_HOME_SID_NID, UIM_CDMA_HOME_SID_NID,
        MMGSDI_CDMA_HOME_SID_NID                                   },
    {GSDIDIAG_CDMA_ZONE_BASED_REGN_IND, UIM_CDMA_ZONE_BASED_REGN_IND,
        MMGSDI_CDMA_ZONE_BASED_REGN_IND                            },
    {GSDIDIAG_CDMA_SYS_REGN_IND, UIM_CDMA_SYS_REGN_IND,
        MMGSDI_CDMA_SYS_REGN_IND                                   },
    {GSDIDIAG_CDMA_DIST_BASED_REGN_IND, UIM_CDMA_DIST_BASED_REGN_IND,
        MMGSDI_CDMA_DIST_BASED_REGN_IND                            },
    {GSDIDIAG_CDMA_ACCOLC, UIM_CDMA_ACCOLC,
        MMGSDI_CDMA_ACCOLC                                         },
    {GSDIDIAG_CDMA_CALL_TERM_MOD_PREF, UIM_CDMA_CALL_TERM_MODE_PREF,
        MMGSDI_CDMA_CALL_TERM_MODE_PREF                             },
    {GSDIDIAG_CDMA_SCI, UIM_CDMA_SCI, MMGSDI_CDMA_SCI              },
    {GSDIDIAG_CDMA_ANALOG_CHAN_PREF, UIM_CDMA_ANALOG_CHAN_PREF,
        MMGSDI_CDMA_ANALOG_CHAN_PREF                               },
    {GSDIDIAG_CDMA_PRL, UIM_CDMA_PRL, MMGSDI_CDMA_PRL              },
    {GSDIDIAG_CDMA_RUIM_ID, UIM_CDMA_RUIM_ID, MMGSDI_CDMA_RUIM_ID  },
    {GSDIDIAG_CDMA_SVC_TABLE, UIM_CDMA_CDMA_SVC_TABLE,
        MMGSDI_CDMA_SVC_TABLE                                      },
    {GSDIDIAG_CDMA_SPC, UIM_CDMA_SPC, MMGSDI_CDMA_SPC              },
    {GSDIDIAG_CDMA_OTAPA_SPC_ENABLE, UIM_CDMA_OTAPA_SPC_ENABLE,
        MMGSDI_CDMA_OTAPA_SPC_ENABLE                               },
    {GSDIDIAG_CDMA_NAM_LOCK, UIM_CDMA_NAM_LOCK,
        MMGSDI_CDMA_NAM_LOCK                                       },
    {GSDIDIAG_CDMA_OTASP_OTAPA_FEATURES, UIM_CDMA_OTASP_OTAPA_FEATURES,
        MMGSDI_CDMA_OTASP_OTAPA_FEATURES                           },
    {GSDIDIAG_CDMA_SERVICE_PREF, UIM_CDMA_SERVICE_PREF,
        MMGSDI_CDMA_SERVICE_PREF                                   },
    {GSDIDIAG_CDMA_ESN_ME, UIM_CDMA_ESN_ME, MMGSDI_CDMA_ESN_ME     },
    {GSDIDIAG_CDMA_RUIM_PHASE, UIM_CDMA_RUIM_PHASE,
        MMGSDI_CDMA_RUIM_PHASE                                     },
    {GSDIDIAG_CDMA_PREF_LANG, UIM_CDMA_PREF_LANG,
        MMGSDI_CDMA_PREF_LANG                                      },
    {GSDIDIAG_CDMA_UNASSIGNED_1, UIM_CDMA_UNASSIGNED_1,
        MMGSDI_CDMA_UNASSIGNED_1                                   },
    {GSDIDIAG_CDMA_SMS, UIM_CDMA_SMS, MMGSDI_CDMA_SMS              },
    {GSDIDIAG_CDMA_SMS_PARAMS, UIM_CDMA_SMS_PARAMS,
        MMGSDI_CDMA_SMS_PARAMS                                     },
    {GSDIDIAG_CDMA_SMS_STATUS, UIM_CDMA_SMS_STATUS,
        MMGSDI_CDMA_SMS_STATUS                                     },
    {GSDIDIAG_CDMA_SUP_SVCS_FEATURE_CODE_TABLE,
        UIM_CDMA_SUP_SVCS_FEATURE_CODE_TABLE,
        MMGSDI_CDMA_SUP_SVCS_FEATURE_CODE_TABLE                    },
    {GSDIDIAG_CDMA_UNASSIGNED_2, UIM_CDMA_UNASSIGNED_2,
        MMGSDI_CDMA_UNASSIGNED_2                                   },
    {GSDIDIAG_CDMA_HOME_SVC_PVDR_NAME, UIM_CDMA_HOME_SVC_PVDR_NAME,
        MMGSDI_CDMA_HOME_SVC_PVDR_NAME                             },
    {GSDIDIAG_CDMA_UIM_ID_USAGE_IND, UIM_CDMA_UIM_ID_USAGE_IND,
        MMGSDI_CDMA_UIM_ID_USAGE_IND                               },
    {GSDIDIAG_CDMA_ADM_DATA, UIM_CDMA_ADM_DATA,
        MMGSDI_CDMA_ADM_DATA                                       },
    {GSDIDIAG_CDMA_MSISDN, UIM_CDMA_MSISDN,
        MMGSDI_CDMA_MSISDN                                         },
    {GSDIDIAG_CDMA_MAXIMUM_PRL, UIM_CDMA_MAXIMUM_PRL,
        MMGSDI_CDMA_MAXIMUM_PRL                                    },
    {GSDIDIAG_CDMA_SPC_STATUS, UIM_CDMA_SPC_STATUS,
        MMGSDI_CDMA_SPC_STATUS                                     },
    {GSDIDIAG_CDMA_ECC, UIM_CDMA_ECC,
        MMGSDI_CDMA_ECC                                            },
    {GSDIDIAG_CDMA_TMSI, UIM_CDMA_TMSI,
        MMGSDI_CDMA_TMSI                                           },
    {GSDIDIAG_CDMA_3GPD_ME3GPDOPC, UIM_CDMA_3GPD_ME3GPDOPC,
        MMGSDI_CDMA_3GPD_ME3GPDOPC                                 },
    {GSDIDIAG_CDMA_3GPD_3GPDOPM, UIM_CDMA_3GPD_3GPDOPM,
        MMGSDI_CDMA_3GPD_3GPDOPM                                   },
    {GSDIDIAG_CDMA_3GPD_SIPCAP, UIM_CDMA_3GPD_SIPCAP,
        MMGSDI_CDMA_3GPD_SIPCAP                                    },
    {GSDIDIAG_CDMA_3GPD_MIPCAP, UIM_CDMA_3GPD_MIPCAP,
        MMGSDI_CDMA_3GPD_MIPCAP                                    },
    {GSDIDIAG_CDMA_3GPD_SIPUPP, UIM_CDMA_3GPD_SIPUPP,
        MMGSDI_CDMA_3GPD_SIPUPP                                    },
    {GSDIDIAG_CDMA_3GPD_MIPUPP, UIM_CDMA_3GPD_MIPUPP,
        MMGSDI_CDMA_3GPD_MIPUPP                                    },
    {GSDIDIAG_CDMA_3GPD_SIPSP , UIM_CDMA_3GPD_SIPSP,
        MMGSDI_CDMA_3GPD_SIPSP                                     },
    {GSDIDIAG_CDMA_3GPD_MIPSP , UIM_CDMA_3GPD_MIPSP,
        MMGSDI_CDMA_3GPD_MIPSP                                     },
    {GSDIDIAG_CDMA_3GPD_SIPPAPSS, UIM_CDMA_3GPD_SIPPAPSS,
        MMGSDI_CDMA_3GPD_SIPPAPSS                                  },
    {GSDIDIAG_CDMA_UNASSIGNED_3 , UIM_CDMA_UNASSIGNED_3,
        MMGSDI_CDMA_UNASSIGNED_3                                   },
    {GSDIDIAG_CDMA_UNASSIGNED_4, UIM_CDMA_UNASSIGNED_4,
        MMGSDI_CDMA_UNASSIGNED_4                                   },
    {GSDIDIAG_CDMA_PUZL, UIM_CDMA_PUZL,
        MMGSDI_CDMA_PUZL                                           },
    {GSDIDIAG_CDMA_MAXPUZL, UIM_CDMA_MAXPUZL,
        MMGSDI_CDMA_MAXPUZL                                        },
    {GSDIDIAG_CDMA_MECRP, UIM_CDMA_MECRP,
        MMGSDI_CDMA_MECRP                                          },
    {GSDIDIAG_CDMA_HRPDCAP, UIM_CDMA_HRPDCAP,
        MMGSDI_CDMA_HRPDCAP                                        },
    {GSDIDIAG_CDMA_HRPDUPP, UIM_CDMA_HRPDUPP,
        MMGSDI_CDMA_HRPDUPP                                        },
    {GSDIDIAG_CDMA_CSSPR, UIM_CDMA_CSSPR,
        MMGSDI_CDMA_CSSPR                                          },
    {GSDIDIAG_CDMA_ATC, UIM_CDMA_ATC,
        MMGSDI_CDMA_ATC                                            },
    {GSDIDIAG_CDMA_EPRL, UIM_CDMA_EPRL,
        MMGSDI_CDMA_EPRL                                           },
    {GSDIDIAG_CDMA_BCSMS_CONFIG, UIM_CDMA_BCSMS_CONFIG,
        MMGSDI_CDMA_BCSMS_CONFIG                                   },
    {GSDIDIAG_CDMA_BCSMS_PREF, UIM_CDMA_BCSMS_PREF,
        MMGSDI_CDMA_BCSMS_PREF                                     },
    {GSDIDIAG_CDMA_BCSMS_TABLE, UIM_CDMA_BCSMS_TABLE,
        MMGSDI_CDMA_BCSMS_TABLE                                    },
    {GSDIDIAG_CDMA_BCSMS_PARAMS, UIM_CDMA_BCSMS_PARAMS,
        MMGSDI_CDMA_BCSMS_PARAMS                                   },
    {GSDIDIAG_CDMA_MMS_NOTIF, UIM_CDMA_MMS_NOTIF,
        MMGSDI_CDMA_MMS_NOTIF                                      },
    {GSDIDIAG_CDMA_MMS_EXT8, UIM_CDMA_MMS_EXT8,
        MMGSDI_CDMA_MMS_EXT8                                       },
    {GSDIDIAG_CDMA_MMS_ICP, UIM_CDMA_MMS_ICP,
        MMGSDI_CDMA_MMS_ICP                                        },
    {GSDIDIAG_CDMA_MMS_UP, UIM_CDMA_MMS_UP,
        MMGSDI_CDMA_MMS_UP                                         },
    {GSDIDIAG_CDMA_SMS_CAP, UIM_CDMA_SMS_CAP,
        MMGSDI_CDMA_SMS_CAP                                        },
    {GSDIDIAG_CDMA_3GPD_IPV6CAP, UIM_CDMA_3GPD_IPV6CAP,
        MMGSDI_CDMA_3GPD_IPV6CAP                                   },
    {GSDIDIAG_CDMA_3GPD_MIPFLAGS, UIM_CDMA_3GPD_MIPFLAGS,
        MMGSDI_CDMA_3GPD_MIPFLAGS                                  },
    {GSDIDIAG_CDMA_3GPD_TCPCONFIG, UIM_CDMA_3GPD_TCPCONFIG,
        MMGSDI_CDMA_3GPD_TCPCONFIG                                 },
    {GSDIDIAG_CDMA_3GPD_DGC, UIM_CDMA_3GPD_DGC,
        MMGSDI_CDMA_3GPD_DGC                                       },
    {GSDIDIAG_CDMA_BROWSER_CP, UIM_CDMA_BROWSER_CP,
        MMGSDI_CDMA_BROWSER_CP                                     },
    {GSDIDIAG_CDMA_BROWSER_BM, UIM_CDMA_BROWSER_BM,
        MMGSDI_CDMA_BROWSER_BM                                     },
    {GSDIDIAG_CDMA_3GPD_SIPUPPEXT, UIM_CDMA_3GPD_SIPUPPEXT,
        MMGSDI_CDMA_3GPD_SIPUPPEXT                                 },
    {GSDIDIAG_CDMA_MMS_CONFIG, UIM_CDMA_MMS_CONFIG,
        MMGSDI_CDMA_MMS_CONFIG                                     },
    {GSDIDIAG_CDMA_JAVA_DURL, UIM_CDMA_JAVA_DURL,
        MMGSDI_CDMA_JAVA_DURL                                      },
    {GSDIDIAG_CDMA_3GPD_MIPUPPEXT, UIM_CDMA_3GPD_MIPUPPEXT,
        MMGSDI_CDMA_3GPD_MIPUPPEXT                                 },
    {GSDIDIAG_CDMA_BREW_DLOAD, UIM_CDMA_BREW_DLOAD,
        MMGSDI_CDMA_BREW_DLOAD                                     },
    {GSDIDIAG_CDMA_BREW_TSID, UIM_CDMA_BREW_TSID,
        MMGSDI_CDMA_BREW_TSID                                      },
    {GSDIDIAG_CDMA_BREW_SID, UIM_CDMA_BREW_SID,
        MMGSDI_CDMA_BREW_SID                                       },
    {GSDIDIAG_CDMA_LBS_XCONFIG, UIM_CDMA_LBS_XCONFIG,
        MMGSDI_CDMA_LBS_XCONFIG                                    },
    {GSDIDIAG_CDMA_LBS_XSURL, UIM_CDMA_LBS_XSURL,
        MMGSDI_CDMA_LBS_XSURL                                      },
    {GSDIDIAG_CDMA_LBS_V2CONFIG, UIM_CDMA_LBS_V2CONFIG,
        MMGSDI_CDMA_LBS_V2CONFIG                                   },
    {GSDIDIAG_CDMA_LBS_V2PDEADDR, UIM_CDMA_LBS_V2PDEADDR,
        MMGSDI_CDMA_LBS_V2PDEADDR                                  },
    {GSDIDIAG_CDMA_LBS_V2MPCADDR, UIM_CDMA_LBS_V2MPCADDR,
        MMGSDI_CDMA_LBS_V2MPCADDR                                  },
    {GSDIDIAG_CDMA_BREW_AEP, UIM_CDMA_BREW_AEP,
        MMGSDI_CDMA_BREW_AEP                                       },
    {GSDIDIAG_CDMA_MODEL, UIM_CDMA_MODEL,
        MMGSDI_CDMA_MODEL                                          },
    {GSDIDIAG_CDMA_RC, UIM_CDMA_RC,
        MMGSDI_CDMA_RC                                             },
    {GSDIDIAG_CDMA_APP_LABELS, UIM_CDMA_APP_LABELS,
        MMGSDI_CDMA_APP_LABELS                                     },
    {GSDIDIAG_CDMA_USER_AGENT_STRING, UIM_CDMA_USER_AGENT_STRING,
        MMGSDI_CDMA_USER_AGENT_STRING                              },
    {GSDIDIAG_CDMA_GID2, UIM_CDMA_GID2,
        MMGSDI_CDMA_GID2                                           },
    {GSDIDIAG_CDMA_GID1, UIM_CDMA_GID1,
        MMGSDI_CDMA_GID1                                           },
    {GSDIDIAG_CDMA_GID_REFERENCES, UIM_CDMA_GID_REFERENCES,
        MMGSDI_CDMA_GID_REFERENCES                                 },
    {GSDIDIAG_CDMA_IMSI_STATUS, UIM_CDMA_IMSI_STATUS,
        MMGSDI_CDMA_IMSI_STATUS                                    },
    {GSDIDIAG_CDMA_ME_DOWNLOADABLE_DATA,
        UIM_CDMA_ME_DOWNLOADABLE_DATA,
        MMGSDI_CDMA_ME_DOWNLOADABLE_DATA                           },
    {GSDIDIAG_CDMA_ME_SETTING_DATA, UIM_CDMA_ME_SETTING_DATA,
        MMGSDI_CDMA_ME_SETTING_DATA                                },
    {GSDIDIAG_CDMA_ME_USER_DATA, UIM_CDMA_ME_USER_DATA,
        MMGSDI_CDMA_ME_USER_DATA                                   },
    {GSDIDIAG_CDMA_RESERVED2, UIM_CDMA_RESERVED2,
        MMGSDI_CDMA_RESERVED2                                      },
    {GSDIDIAG_CDMA_RESERVED1, UIM_CDMA_RESERVED1,
        MMGSDI_CDMA_RESERVED1                                      },
    {GSDIDIAG_CDMA_UIM_SVC_TABLE, UIM_CDMA_UIM_SVC_TABLE,
        MMGSDI_CDMA_UIM_SVC_TABLE                                  },
    {GSDIDIAG_CDMA_UIM_FEATURES, UIM_CDMA_UIM_FEATURES,
        MMGSDI_CDMA_UIM_FEATURES                                   },
    {GSDIDIAG_CDMA_UIM_VERSION, UIM_CDMA_UIM_VERSION,
        MMGSDI_CDMA_UIM_VERSION                                    },
    {GSDIDIAG_CDMA_HRPD_HRPDUPP, UIM_CDMA_HRPD_HRPDUPP,
        MMGSDI_CDMA_HRPD_HRPDUPP                                   },
    {GSDIDIAG_CDMA_SF_EUIM_ID, UIM_CDMA_SF_EUIM_ID,
        MMGSDI_CDMA_SF_EUIM_ID                                     },


    {GSDIDIAG_GSM_LP, UIM_GSM_LP, MMGSDI_GSM_LP                    },
    {GSDIDIAG_GSM_IMSI, UIM_GSM_IMSI, MMGSDI_GSM_IMSI               },
    {GSDIDIAG_GSM_KC, UIM_GSM_KC, MMGSDI_GSM_KC                     },
    {GSDIDIAG_GSM_PLMN, UIM_GSM_PLMN, MMGSDI_GSM_PLMN               },
    {GSDIDIAG_GSM_HPLMN, UIM_GSM_HPLMN, MMGSDI_GSM_HPLMN            },
    {GSDIDIAG_GSM_ACM_MAX, UIM_GSM_ACM_MAX, MMGSDI_GSM_ACM_MAX      },
    {GSDIDIAG_GSM_SST, UIM_GSM_SST, MMGSDI_GSM_SST                                 },
    {GSDIDIAG_GSM_ACM, UIM_GSM_ACM, MMGSDI_GSM_ACM                                 },
    {GSDIDIAG_GSM_GID1, UIM_GSM_GID1, MMGSDI_GSM_GID1                               },
    {GSDIDIAG_GSM_GDI2, UIM_GSM_GID2, MMGSDI_GSM_GID2                               },
    {GSDIDIAG_GSM_SPN, UIM_GSM_SPN, MMGSDI_GSM_SPN                                 },
    {GSDIDIAG_GSM_PUCT, UIM_GSM_PUCT, MMGSDI_GSM_PUCT                               },
    {GSDIDIAG_GSM_CBMI, UIM_GSM_CBMI, MMGSDI_GSM_CBMI                               },
    {GSDIDIAG_GSM_BCCH, UIM_GSM_BCCH, MMGSDI_GSM_BCCH                               },
    {GSDIDIAG_GSM_ACC, UIM_GSM_ACC, MMGSDI_GSM_ACC                                 },
    {GSDIDIAG_GSM_FPLMN, UIM_GSM_FPLMN, MMGSDI_GSM_FPLMN                             },
    {GSDIDIAG_GSM_LOCI, UIM_GSM_LOCI, MMGSDI_GSM_LOCI                               },
    {GSDIDIAG_GSM_AD, UIM_GSM_AD, MMGSDI_GSM_AD                                   },
    {GSDIDIAG_GSM_PHASE, UIM_GSM_PHASE, MMGSDI_GSM_PHASE                             },
    {GSDIDIAG_GSM_VGCS, UIM_GSM_VGCS, MMGSDI_GSM_VGCS                               },
    {GSDIDIAG_GSM_VGCSS, UIM_GSM_VGCSS, MMGSDI_GSM_VGCSS                             },
    {GSDIDIAG_GSM_VBS, UIM_GSM_VBS, MMGSDI_GSM_VBS                                 },
    {GSDIDIAG_GSM_VBSS, UIM_GSM_VBSS, MMGSDI_GSM_VBSS                               },
    {GSDIDIAG_GSM_EMLPP, UIM_GSM_EMLPP, MMGSDI_GSM_EMLPP                             },
    {GSDIDIAG_GSM_AAEM, UIM_GSM_AAEM, MMGSDI_GSM_AAEM                               },
    {GSDIDIAG_GSM_CBMID, UIM_GSM_CBMID, MMGSDI_GSM_CBMID                             },
    {GSDIDIAG_GSM_ECC, UIM_GSM_ECC, MMGSDI_GSM_ECC                                 },
    {GSDIDIAG_GSM_CBMIR, UIM_GSM_CBMIR, MMGSDI_GSM_CBMIR                             },
    {GSDIDIAG_GSM_DCK, UIM_GSM_DCK, MMGSDI_GSM_DCK                                 },
    {GSDIDIAG_GSM_CNL, UIM_GSM_CNL, MMGSDI_GSM_CNL                                 },
    {GSDIDIAG_GSM_NIA, UIM_GSM_NIA, MMGSDI_GSM_NIA                                 },
    {GSDIDIAG_GSM_KCGPRS, UIM_GSM_KCGPRS, MMGSDI_GSM_KCGPRS                           },
    {GSDIDIAG_GSM_LOCIGPRS, UIM_GSM_LOCIGPRS, MMGSDI_GSM_LOCIGPRS                       },
    {GSDIDIAG_GSM_SUME, UIM_GSM_SUME, MMGSDI_GSM_SUME                               },
    {GSDIDIAG_GSM_PLMNWAT, UIM_GSM_PLMNWACT, MMGSDI_GSM_PLMNWACT                        },
    {GSDIDIAG_GSM_OPLMNWACT, UIM_GSM_OPLMNWACT, MMGSDI_GSM_OPLMNWACT                     },
    {GSDIDIAG_GSM_CPBCCH, UIM_GSM_CPBCCH, MMGSDI_GSM_CPBCCH                           },
    {GSDIDIAG_GSM_INVSCAN, UIM_GSM_INVSCAN, MMGSDI_GSM_INVSCAN                         },
    {GSDIDIAG_GSM_RPLMNACT, UIM_GSM_RPLMNAT, MMGSDI_GSM_RPLMNAT                        },
    {GSDIDIAG_GSM_SAI, UIM_GSM_SAI, MMGSDI_GSM_SAI                                 },
    {GSDIDIAG_GSM_SLL, UIM_GSM_SLL, MMGSDI_GSM_SLL                                 },
    {GSDIDIAG_GSM_MEXE_ST, UIM_GSM_MEXE_ST, MMGSDI_GSM_MEXE_ST                         },
    {GSDIDIAG_GSM_ORPK, UIM_GSM_ORPK, MMGSDI_GSM_ORPK                               },
    {GSDIDIAG_GSM_ARPK, UIM_GSM_ARPK, MMGSDI_GSM_ARPK                               },
    {GSDIDIAG_GSM_TPRPK, UIM_GSM_TPRPK, MMGSDI_GSM_TPRPK                             },
    {GSDIDIAG_GSM_IMG, UIM_GSM_IMG, MMGSDI_GSM_IMG                                 },
    {GSDIDIAG_GSM_IMG1INST1, UIM_GSM_IMG1INST1, MMGSDI_GSM_IMG1INST1                     },
    {GSDIDIAG_GSM_IMG1INST2, UIM_GSM_IMG1INST2, MMGSDI_GSM_IMG1INST2                     },
    {GSDIDIAG_GSM_IMG1INST3, UIM_GSM_IMG1INST3, MMGSDI_GSM_IMG1INST3                     },
    {GSDIDIAG_GSM_IMG2INST1, UIM_GSM_IMG2INST1, MMGSDI_GSM_IMG2INST1                     },
    {GSDIDIAG_GSM_IMG2INST2, UIM_GSM_IMG2INST2, MMGSDI_GSM_IMG2INST2                     },
    {GSDIDIAG_GSM_IMG2INST3, UIM_GSM_IMG2INST3, MMGSDI_GSM_IMG2INST3                     },
    {GSDIDIAG_GSM_IMG3INST1, UIM_GSM_IMG3INST1, MMGSDI_GSM_IMG3INST1                     },
    {GSDIDIAG_GSM_IMG3INST2, UIM_GSM_IMG3INST2, MMGSDI_GSM_IMG3INST2                    },
    {GSDIDIAG_GSM_IMG3INST3, UIM_GSM_IMG3INST3, MMGSDI_GSM_IMG3INST3                     },
    {GSDIDIAG_GSM_HPLMNWACT, UIM_GSM_HPLMNACT,  MMGSDI_GSM_HPLMNACT                      },
    {GSDIDIAG_GSM_PNN,       UIM_GSM_PNN,       MMGSDI_GSM_PNN },
    {GSDIDIAG_GSM_OPL,       UIM_GSM_OPL,       MMGSDI_GSM_OPL },
    {GSDIDIAG_GSM_HPLMNACT,  UIM_GSM_HPLMNACT,  MMGSDI_GSM_HPLMNACT },
    {GSDIDIAG_GSM_MBDN,      UIM_GSM_MBDN,      MMGSDI_GSM_MBDN },
    {GSDIDIAG_GSM_EXT6,      UIM_GSM_EXT6,      MMGSDI_GSM_EXT6 },
    {GSDIDIAG_GSM_MBI,       UIM_GSM_MBI,       MMGSDI_GSM_MBI },
    {GSDIDIAG_GSM_MWIS,      UIM_GSM_MWIS,      MMGSDI_GSM_MWIS },
    {GSDIDIAG_GSM_EXT1,      UIM_GSM_EXT1,      MMGSDI_GSM_EXT1 },
    {GSDIDIAG_GSM_SPDI,      UIM_GSM_SPDI,      MMGSDI_GSM_SPDI },
    {GSDIDIAG_GSM_CFIS,      UIM_GSM_CFIS,      MMGSDI_GSM_CFIS },

    {GSDIDIAG_TELECOM_ADN, UIM_TELECOM_ADN, MMGSDI_TELECOM_ADN                         },
    {GSDIDIAG_TELECOM_FDN, UIM_TELECOM_FDN, MMGSDI_TELECOM_FDN                         },
    {GSDIDIAG_TELECOM_SMS, UIM_TELECOM_SMS, MMGSDI_TELECOM_SMS                         },
    {GSDIDIAG_TELECOM_CCP, UIM_TELECOM_CCP, MMGSDI_TELECOM_CCP                         },
    {GSDIDIAG_TELECOM_ECCP, UIM_TELECOM_ECCP, MMGSDI_TELECOM_ECCP                       },
    {GSDIDIAG_TELECOM_MSISDN, UIM_TELECOM_MSISDN, MMGSDI_TELECOM_MSISDN                   },
    {GSDIDIAG_TELECOM_SMSP, UIM_TELECOM_SMSP, MMGSDI_TELECOM_SMSP                       },
    {GSDIDIAG_TELECOM_SMSS, UIM_TELECOM_SMSS, MMGSDI_TELECOM_SMSS                       },
    {GSDIDIAG_TELECOM_LND, UIM_TELECOM_LND, MMGSDI_TELECOM_LND                         },
    {GSDIDIAG_TELECOM_SDN, UIM_TELECOM_SDN, MMGSDI_TELECOM_SDN                         },
    {GSDIDIAG_TELECOM_EXT1, UIM_TELECOM_EXT1, MMGSDI_TELECOM_EXT1                       },

#ifdef FEATURE_USIM_1000_PBM  
    {GSDIDIAG_TELECOM_ADN2,    UIM_TELECOM_ADN2,  MMGSDI_TELECOM_ADN2                       },
    {GSDIDIAG_TELECOM_ANR2,    UIM_TELECOM_ANR2,  MMGSDI_TELECOM_ANR2                       },
    {GSDIDIAG_TELECOM_ANRA2,   UIM_TELECOM_ANRA2, MMGSDI_TELECOM_ANRA2                      },
    {GSDIDIAG_TELECOM_ANRB2,   UIM_TELECOM_ANRB2, MMGSDI_TELECOM_ANRB2                      },
    {GSDIDIAG_TELECOM_ANRC2,   UIM_TELECOM_ANRC2, MMGSDI_TELECOM_ANRC2                      },
    {GSDIDIAG_TELECOM_EMAIL2,  UIM_TELECOM_EMAIL2,MMGSDI_TELECOM_EMAIL2                     },
    {GSDIDIAG_TELECOM_GRP2,    UIM_TELECOM_GRP2,  MMGSDI_TELECOM_GRP2                       },
    {GSDIDIAG_TELECOM_SNE2,    UIM_TELECOM_SNE2,  MMGSDI_TELECOM_SNE2                       },
    {GSDIDIAG_TELECOM_IAP2,    UIM_TELECOM_IAP2,  MMGSDI_TELECOM_IAP2                       },
    {GSDIDIAG_TELECOM_UID2,    UIM_TELECOM_UID2,  MMGSDI_TELECOM_UID2                       },
    {GSDIDIAG_TELECOM_PBC2,    UIM_TELECOM_PBC2,  MMGSDI_TELECOM_PBC2                       },

    {GSDIDIAG_TELECOM_ADN3,    UIM_TELECOM_ADN3,  MMGSDI_TELECOM_ADN3                       },
    {GSDIDIAG_TELECOM_ANR3,    UIM_TELECOM_ANR3,  MMGSDI_TELECOM_ANR3                       },
    {GSDIDIAG_TELECOM_ANRA3,   UIM_TELECOM_ANRA3, MMGSDI_TELECOM_ANRA3                      },
    {GSDIDIAG_TELECOM_ANRB3,   UIM_TELECOM_ANRB3, MMGSDI_TELECOM_ANRB3                      },
    {GSDIDIAG_TELECOM_ANRC3,   UIM_TELECOM_ANRC3, MMGSDI_TELECOM_ANRC3                      },
    {GSDIDIAG_TELECOM_EMAIL3,  UIM_TELECOM_EMAIL3,MMGSDI_TELECOM_EMAIL3                     },
    {GSDIDIAG_TELECOM_GRP3,    UIM_TELECOM_GRP3,  MMGSDI_TELECOM_GRP3                       },
    {GSDIDIAG_TELECOM_SNE3,    UIM_TELECOM_SNE3,  MMGSDI_TELECOM_SNE3                       },
    {GSDIDIAG_TELECOM_IAP3,    UIM_TELECOM_IAP3,  MMGSDI_TELECOM_IAP3                       },
    {GSDIDIAG_TELECOM_UID3,    UIM_TELECOM_UID3,  MMGSDI_TELECOM_UID3                       },
    {GSDIDIAG_TELECOM_PBC3,    UIM_TELECOM_PBC3,  MMGSDI_TELECOM_PBC3                       },
#endif /* FEATURE_USIM_1000_PBM  */

    {GSDIDIAG_TELECOM_EXT2, UIM_TELECOM_EXT2, MMGSDI_TELECOM_EXT2                       },
    {GSDIDIAG_TELECOM_EXT3, UIM_TELECOM_EXT3, MMGSDI_TELECOM_EXT3                       },
    {GSDIDIAG_TELECOM_BDN, UIM_TELECOM_BDN, MMGSDI_TELECOM_BDN                         },
    {GSDIDIAG_TELECOM_EXT4, UIM_TELECOM_EXT4, MMGSDI_TELECOM_EXT4                       },
    {GSDIDIAG_TELECOM_SMSR, UIM_TELECOM_SMSR, MMGSDI_TELECOM_SMSR                       },
    {GSDIDIAG_TELECOM_CMI, UIM_TELECOM_CMI, MMGSDI_TELECOM_CMI                         },
    {GSDIDIAG_TELECOM_SUME, UIM_TELECOM_SUME,    MMGSDI_TELECOM_SUME                   },
    {GSDIDIAG_TELECOM_ARR, UIM_TELECOM_ARR,MMGSDI_TELECOM_ARR                         },
    {GSDIDIAG_TELECOM_PBR, UIM_TELECOM_PBR, MMGSDI_TELECOM_PBR                        },
    {GSDIDIAG_TELECOM_CCP1, UIM_TELECOM_CCP1, MMGSDI_TELECOM_CCP1                      },
    {GSDIDIAG_TELECOM_UID, UIM_TELECOM_UID,  MMGSDI_TELECOM_UID                       },
    {GSDIDIAG_TELECOM_PSC, UIM_TELECOM_PSC, MMGSDI_TELECOM_PSC                        },
    {GSDIDIAG_TELECOM_CC, UIM_TELECOM_CC,  MMGSDI_TELECOM_CC                         },
    {GSDIDIAG_TELECOM_PUID, UIM_TELECOM_PUID, MMGSDI_TELECOM_PUID                      },
    {GSDIDIAG_TELECOM_ADN1, UIM_TELECOM_ADN1, MMGSDI_TELECOM_ADN1                      },
    {GSDIDIAG_TELECOM_GRP, UIM_TELECOM_GRP,  MMGSDI_TELECOM_GRP                       },
    {GSDIDIAG_TELECOM_GRP1, UIM_TELECOM_GRP1, MMGSDI_TELECOM_GRP1                      },
    {GSDIDIAG_TELECOM_GAS, UIM_TELECOM_GAS, MMGSDI_TELECOM_GAS                        },
    {GSDIDIAG_TELECOM_SNE, UIM_TELECOM_SNE, MMGSDI_TELECOM_SNE                        },
    {GSDIDIAG_TELECOM_SNE1, UIM_TELECOM_SNE1,MMGSDI_TELECOM_SNE1                       },
    {GSDIDIAG_TELECOM_EMAIL, UIM_TELECOM_EMAIL, MMGSDI_TELECOM_EMAIL                    },
    {GSDIDIAG_TELECOM_EMAIL1, UIM_TELECOM_EMAIL1, MMGSDI_TELECOM_EMAIL1                  },
    {GSDIDIAG_TELECOM_IAP, UIM_TELECOM_IAP,  MMGSDI_TELECOM_IAP                       },
    {GSDIDIAG_TELECOM_IAP1, UIM_TELECOM_IAP1, MMGSDI_TELECOM_IAP1                      },
    {GSDIDIAG_TELECOM_PBC, UIM_TELECOM_PBC, MMGSDI_TELECOM_PBC            },
    {GSDIDIAG_TELECOM_ANR,    UIM_TELECOM_ANR,   MMGSDI_TELECOM_ANR    },
    {GSDIDIAG_TELECOM_ANRA,   UIM_TELECOM_ANRA,  MMGSDI_TELECOM_ANRA   },
    {GSDIDIAG_TELECOM_ANRB,   UIM_TELECOM_ANRB,  MMGSDI_TELECOM_ANRB   },
    {GSDIDIAG_TELECOM_ANRC,   UIM_TELECOM_ANRC,  MMGSDI_TELECOM_ANRC   },
    {GSDIDIAG_TELECOM_ANR1,   UIM_TELECOM_ANR1,  MMGSDI_TELECOM_ANR1   },
    {GSDIDIAG_TELECOM_ANRA1,  UIM_TELECOM_ANRA1, MMGSDI_TELECOM_ANRA1  },
    {GSDIDIAG_TELECOM_ANRB1,  UIM_TELECOM_ANRB1, MMGSDI_TELECOM_ANRB1  },
    {GSDIDIAG_TELECOM_ANRC1,  UIM_TELECOM_ANRC1, MMGSDI_TELECOM_ANRC1  },

    {GSDIDIAG_TELECOM_GRAPHICS_IMG, UIM_IMAGE,  MMGSDI_IMAGE                    },

    {GSDIDIAG_USIM_LI, UIM_USIM_LI, MMGSDI_USIM_LI                                },
    {GSDIDIAG_USIM_IMSI, UIM_USIM_IMSI,  MMGSDI_USIM_IMSI                           },
    {GSDIDIAG_USIM_KEYS, UIM_USIM_KEYS,MMGSDI_USIM_KEYS                             },
    {GSDIDIAG_USIM_KEYSPS, UIM_USIM_KEYSPS,  MMGSDI_USIM_KEYSPS                       },
    {GSDIDIAG_USIM_PLMNWACT, UIM_USIM_PLMNWACT, MMGSDI_USIM_PLMNWACT                    },
    {GSDIDIAG_USIM_UPLMNSEL, UIM_USIM_UPLMNSEL, MMGSDI_USIM_UPLMNSEL                    },
    {GSDIDIAG_USIM_HPLMN, UIM_USIM_HPLMN,MMGSDI_USIM_HPLMN                           },
    {GSDIDIAG_USIM_ACM_MAX, UIM_USIM_ACM_MAX, MMGSDI_USIM_ACM_MAX                      },
    {GSDIDIAG_USIM_UST, UIM_USIM_UST, MMGSDI_USIM_UST                              },
    {GSDIDIAG_USIM_ACM, UIM_USIM_ACM,  MMGSDI_USIM_ACM                             },
    {GSDIDIAG_USIM_GID1, UIM_USIM_GID1, MMGSDI_USIM_GID1                            },
    {GSDIDIAG_USIM_GID2, UIM_USIM_GID2, MMGSDI_USIM_GID2                            },
    {GSDIDIAG_USIM_SPN, UIM_USIM_SPN,   MMGSDI_USIM_SPN                            },
    {GSDIDIAG_USIM_PUCT, UIM_USIM_PUCT,  MMGSDI_USIM_PUCT                           },
    {GSDIDIAG_USIM_CMBI, UIM_USIM_CBMI,  MMGSDI_USIM_CBMI                           },
    {GSDIDIAG_USIM_ACC, UIM_USIM_ACC, MMGSDI_USIM_ACC                              },
    {GSDIDIAG_USIM_FPLMN, UIM_USIM_FPLMN, MMGSDI_USIM_FPLMN                          },
    {GSDIDIAG_USIM_LOCI, UIM_USIM_LOCI, MMGSDI_USIM_LOCI                            },
    {GSDIDIAG_USIM_AD, UIM_USIM_AD,  MMGSDI_USIM_AD                               },
    {GSDIDIAG_USIM_CBMID, UIM_USIM_CBMID,   MMGSDI_USIM_CBMID                        },
    {GSDIDIAG_USIM_ECC, UIM_USIM_ECC, MMGSDI_USIM_ECC                              },
    {GSDIDIAG_USIM_CBMIR, UIM_USIM_CBMIR, MMGSDI_USIM_CBMIR                          },
    {GSDIDIAG_USIM_PSLOCI, UIM_USIM_PSLOCI, MMGSDI_USIM_PSLOCI                        },
    {GSDIDIAG_USIM_FDN, UIM_USIM_FDN, MMGSDI_USIM_FDN                              },
    {GSDIDIAG_USIM_SMS, UIM_USIM_SMS,  MMGSDI_USIM_SMS                             },
    {GSDIDIAG_USIM_MSISDN, UIM_USIM_MSISDN,MMGSDI_USIM_MSISDN                         },
    {GSDIDIAG_USIM_SMSP, UIM_USIM_SMSP,  MMGSDI_USIM_SMSP                           },
    {GSDIDIAG_USIM_SMSS, UIM_USIM_SMSS,MMGSDI_USIM_SMSS                             },
    {GSDIDIAG_USIM_SDN, UIM_USIM_SDN, MMGSDI_USIM_SDN                              },
    {GSDIDIAG_USIM_EXT2, UIM_USIM_EXT2,  MMGSDI_USIM_EXT2                           },
    {GSDIDIAG_USIM_EXT3, UIM_USIM_EXT3,  MMGSDI_USIM_EXT3                           },
    {GSDIDIAG_USIM_SMSR, UIM_USIM_SMSR, MMGSDI_USIM_SMSR                            },
    {GSDIDIAG_USIM_ICI, UIM_USIM_ICI,  MMGSDI_USIM_ICI                             },
    {GSDIDIAG_USIM_OCI, UIM_USIM_OCI, MMGSDI_USIM_OCI                              },
    {GSDIDIAG_USIM_ICT, UIM_USIM_ICT,  MMGSDI_USIM_ICT                             },
    {GSDIDIAG_USIM_OCT, UIM_USIM_OCT,  MMGSDI_USIM_OCT                             },
    {GSDIDIAG_USIM_EXT5, UIM_USIM_EXT5, MMGSDI_USIM_EXT5                            },
    {GSDIDIAG_USIM_CCP2, UIM_USIM_CCP2,  MMGSDI_USIM_CCP2                           },
    {GSDIDIAG_USIM_EMLPP, UIM_USIM_EMLPP,   MMGSDI_USIM_EMLPP                        },
    {GSDIDIAG_USIM_AAEM, UIM_USIM_AAEM,   MMGSDI_USIM_AAEM                          },
    {GSDIDIAG_USIM_GMSI, UIM_USIM_GMSI,  MMGSDI_USIM_GMSI                           },
    {GSDIDIAG_USIM_HIDDENKEY, UIM_USIM_HIDDENKEY,  MMGSDI_USIM_HIDDENKEY                 },
    {GSDIDIAG_USIM_BDN, UIM_USIM_BDN,  MMGSDI_USIM_BDN                             },
    {GSDIDIAG_USIM_EXT4, UIM_USIM_EXT4, MMGSDI_USIM_EXT4                            },
    {GSDIDIAG_USIM_CMI, UIM_USIM_CMI, MMGSDI_USIM_CMI                              },
    {GSDIDIAG_USIM_EST, UIM_USIM_EST, MMGSDI_USIM_EST                              },
    {GSDIDIAG_USIM_ACL, UIM_USIM_ACL, MMGSDI_USIM_ACL                              },
    {GSDIDIAG_USIM_DCK, UIM_USIM_DCK,MMGSDI_USIM_DCK },
    {GSDIDIAG_USIM_CNL, UIM_USIM_CNL,  MMGSDI_USIM_CNL                             },
    {GSDIDIAG_USIM_START_HFN, UIM_USIM_START_HFN, MMGSDI_USIM_START_HFN                  },
    {GSDIDIAG_USIM_THRESHOLD, UIM_USIM_THRESHOLD,  MMGSDI_USIM_THRESHOLD                 },
    {GSDIDIAG_USIM_OPLMNWACT, UIM_USIM_OPLMNWACT, MMGSDI_USIM_OPLMNWACT                  },
    {GSDIDIAG_USIM_OPLMNSEL, UIM_USIM_OPLMNSEL,MMGSDI_USIM_OPLMNSEL                     },
    {GSDIDIAG_USIM_HPLMNWACT, UIM_USIM_HPLMNWACT,  MMGSDI_USIM_HPLMNWACT                 },
    {GSDIDIAG_USIM_ARR, UIM_USIM_ARR, MMGSDI_USIM_ARR                              },
    {GSDIDIAG_USIM_RPLMNACT, UIM_USIM_RPLMNACT, MMGSDI_USIM_RPLMNACT                    },
    {GSDIDIAG_USIM_NETPAR, UIM_USIM_NETPAR, MMGSDI_USIM_NETPAR                        },
    {GSDIDIAG_USIM_ADN, UIM_USIM_ADN, MMGSDI_USIM_ADN                              },
    {GSDIDIAG_USIM_ADN1, UIM_USIM_ADN1, MMGSDI_USIM_ADN1                            },
    {GSDIDIAG_USIM_GAS, UIM_USIM_GAS, MMGSDI_USIM_GAS                              },
    {GSDIDIAG_USIM_GAS1, UIM_USIM_GAS1, MMGSDI_USIM_GAS1                            },
    {GSDIDIAG_USIM_SNE, UIM_USIM_SNE, MMGSDI_USIM_SNE                              },
    {GSDIDIAG_USIM_SNE1, UIM_USIM_SNE1,  MMGSDI_USIM_SNE1                          },
    {GSDIDIAG_USIM_EMAIL, UIM_USIM_EMAIL,  MMGSDI_USIM_EMAIL                         },
    {GSDIDIAG_USIM_EMAIL1, UIM_USIM_EMAIL1,MMGSDI_USIM_EMAIL1                         },
    {GSDIDIAG_USIM_IAP, UIM_USIM_IAP, MMGSDI_USIM_IAP                              },
    {GSDIDIAG_USIM_IAP1, UIM_USIM_IAP1, MMGSDI_USIM_IAP1                            },

#ifdef FEATURE_USIM_1000_PBM  
    {GSDIDIAG_USIM_ADN2,    UIM_USIM_ADN2,  MMGSDI_USIM_ADN2                       },
    {GSDIDIAG_USIM_ANR2,    UIM_USIM_ANR2,  MMGSDI_USIM_ANR2                       },
    {GSDIDIAG_USIM_ANRA2,   UIM_USIM_ANRA2, MMGSDI_USIM_ANRA2                      },
    {GSDIDIAG_USIM_ANRB2,   UIM_USIM_ANRB2, MMGSDI_USIM_ANRB2                      },
    {GSDIDIAG_USIM_ANRC2,   UIM_USIM_ANRC2, MMGSDI_USIM_ANRC2                      },
    {GSDIDIAG_USIM_EMAIL2,  UIM_USIM_EMAIL2,MMGSDI_USIM_EMAIL2                     },
    {GSDIDIAG_USIM_GRP2,    UIM_USIM_GRP2,  MMGSDI_USIM_GRP2                       },
    {GSDIDIAG_USIM_SNE2,    UIM_USIM_SNE2,  MMGSDI_USIM_SNE2                       },
    {GSDIDIAG_USIM_IAP2,    UIM_USIM_IAP2,  MMGSDI_USIM_IAP2                       },
    {GSDIDIAG_USIM_UID2,    UIM_USIM_UID2,  MMGSDI_USIM_UID2                       },
    {GSDIDIAG_USIM_PBC2,    UIM_USIM_PBC2,  MMGSDI_USIM_PBC2                       },

    {GSDIDIAG_USIM_ADN3,    UIM_USIM_ADN3,  MMGSDI_USIM_ADN3                       },
    {GSDIDIAG_USIM_ANR3,    UIM_USIM_ANR3,  MMGSDI_USIM_ANR3                       },
    {GSDIDIAG_USIM_ANRA3,   UIM_USIM_ANRA3, MMGSDI_USIM_ANRA3                      },
    {GSDIDIAG_USIM_ANRB3,   UIM_USIM_ANRB3, MMGSDI_USIM_ANRB3                      },
    {GSDIDIAG_USIM_ANRC3,   UIM_USIM_ANRC3, MMGSDI_USIM_ANRC3                      },
    {GSDIDIAG_USIM_EMAIL3,  UIM_USIM_EMAIL3,MMGSDI_USIM_EMAIL3                     },
    {GSDIDIAG_USIM_GRP3,    UIM_USIM_GRP3,  MMGSDI_USIM_GRP3                       },
    {GSDIDIAG_USIM_SNE3,    UIM_USIM_SNE3,  MMGSDI_USIM_SNE3                       },
    {GSDIDIAG_USIM_IAP3,    UIM_USIM_IAP3,  MMGSDI_USIM_IAP3                       },
    {GSDIDIAG_USIM_UID3,    UIM_USIM_UID3,  MMGSDI_USIM_UID3                       },
    {GSDIDIAG_USIM_PBC3,    UIM_USIM_PBC3,  MMGSDI_USIM_PBC3                       },
#endif /* FEATURE_USIM_1000_PBM  */

    {GSDIDIAG_USIM_PNN, UIM_USIM_PNN, MMGSDI_USIM_PNN },
    {GSDIDIAG_USIM_OPL, UIM_USIM_OPL, MMGSDI_USIM_OPL },
    {GSDIDIAG_USIM_EHPLMN, UIM_USIM_EHPLMN, MMGSDI_USIM_EHPLMN },
    {GSDIDIAG_USIM_MBDN, UIM_USIM_MBDN, MMGSDI_USIM_MBDN },
    {GSDIDIAG_USIM_EXT6, UIM_USIM_EXT6, MMGSDI_USIM_EXT6 },
    {GSDIDIAG_USIM_MBI, UIM_USIM_MBI, MMGSDI_USIM_MBI },
    {GSDIDIAG_USIM_MWIS, UIM_USIM_MWIS, MMGSDI_USIM_MWIS },
    {GSDIDIAG_USIM_SPDI, UIM_USIM_SPDI, MMGSDI_USIM_SPDI },
    {GSDIDIAG_USIM_SPT_TABLE, UIM_USIM_SPT_TABLE, MMGSDI_USIM_7F66_PROP1_SPT_TABLE },
    {GSDIDIAG_USIM_RAT, UIM_USIM_RAT, MMGSDI_USIM_7F66_PROP1_RAT },
    {GSDIDIAG_USIM_CFIS, UIM_USIM_CFIS, MMGSDI_USIM_CFIS },
    {GSDIDIAG_USIM_ANR,    UIM_USIM_ANR,   MMGSDI_USIM_ANR    },
    {GSDIDIAG_USIM_ANRA,   UIM_USIM_ANRA,  MMGSDI_USIM_ANRA   },
    {GSDIDIAG_USIM_ANRB,   UIM_USIM_ANRB,  MMGSDI_USIM_ANRB   },
    {GSDIDIAG_USIM_ANRC,   UIM_USIM_ANRC,  MMGSDI_USIM_ANRC   },
    {GSDIDIAG_USIM_ANR1,   UIM_USIM_ANR1,  MMGSDI_USIM_ANR1   },
    {GSDIDIAG_USIM_ANRA1,  UIM_USIM_ANRA1, MMGSDI_USIM_ANRA1  },
    {GSDIDIAG_USIM_ANRB1,  UIM_USIM_ANRB1, MMGSDI_USIM_ANRB1  },
    {GSDIDIAG_USIM_ANRC1,  UIM_USIM_ANRC1, MMGSDI_USIM_ANRC1  },

    {GSDIDIAG_USIM_PHONEBOOK_PBR, UIM_USIM_PBR,  MMGSDI_USIM_PBR                   },
    {GSDIDIAG_USIM_PHONEBOOK_CCP1, UIM_USIM_CCP1, MMGSDI_USIM_CCP1                  },
    {GSDIDIAG_USIM_PHONEBOOK_UID, UIM_USIM_UID,   MMGSDI_USIM_UID                  },
    {GSDIDIAG_USIM_PHONEBOOK_PSC, UIM_USIM_PSC,  MMGSDI_USIM_PSC                   },
    {GSDIDIAG_USIM_PHONEBOOK_CC, UIM_USIM_CC, MMGSDI_USIM_CC                      },
    {GSDIDIAG_USIM_PHONEBOOK_PUID, UIM_USIM_PUID,  MMGSDI_USIM_PUID                 },

    {GSDIDIAG_USIM_GSMACCESS_KC, UIM_USIM_KC,  MMGSDI_USIM_KC                     },
    {GSDIDIAG_USIM_GSMACCESS_KCGPRS, UIM_USIM_KCGPRS,   MMGSDI_USIM_KCGPRS            },
    {GSDIDIAG_USIM_GSMACCESS_CPBCCH, UIM_USIM_CPBCCH, MMGSDI_USIM_CPBCCH              },
    {GSDIDIAG_USIM_GSMACCESS_INVSCAN, UIM_USIM_INVSCAN, MMGSDI_USIM_INVSCAN            },

    {GSDIDIAG_USIM_MEXE_MEXE_ST, UIM_USIM_MEXE_ST, MMGSDI_USIM_MEXE_ST                 },
    {GSDIDIAG_USIM_MEXE_ORPK, UIM_USIM_ORPK, MMGSDI_USIM_ORPK                       },
    {GSDIDIAG_USIM_MEXE_ARPK, UIM_USIM_ARPK, MMGSDI_USIM_ARPK                       },
    {GSDIDIAG_USIM_MEXE_TPRPK, UIM_USIM_TPRPK, MMGSDI_USIM_TPRPK                     },

    {GSDIDIAG_UIM_ISIM_IMPI, UIM_ISIM_IMPI, MMGSDI_ISIM_IMPI },
    {GSDIDIAG_UIM_ISIM_DOMAIN, UIM_ISIM_DOMAIN, MMGSDI_ISIM_DOMAIN },
    {GSDIDIAG_UIM_ISIM_IMPU, UIM_ISIM_IMPU, MMGSDI_ISIM_IMPU },
    {GSDIDIAG_UIM_ISIM_ARR, UIM_ISIM_ARR, MMGSDI_ISIM_ARR },
    {GSDIDIAG_UIM_ISIM_KEYS, UIM_ISIM_KEYS, MMGSDI_ISIM_KEYS },
    {GSDIDIAG_UIM_ISIM_AD, UIM_ISIM_AD, MMGSDI_ISIM_AD },
    /* MMS */
    {GSDIDIAG_USIM_MMSN, UIM_USIM_MMSN, MMGSDI_USIM_MMSN },
    {GSDIDIAG_USIM_MMSICP, UIM_USIM_MMSICP, MMGSDI_USIM_MMSICP },
    {GSDIDIAG_USIM_MMSUP, UIM_USIM_MMSUP, MMGSDI_USIM_MMSUP },
    {GSDIDIAG_USIM_MMSUCP, UIM_USIM_MMSUCP, MMGSDI_USIM_MMSUCP },
    {GSDIDIAG_USIM_EXT8, UIM_USIM_EXT8, MMGSDI_USIM_EXT8 },
    /* PKCS */
    {GSDIDIAG_PKCS15_ODF, UIM_PKCS15_ODF, MMGSDI_PKCS15_ODF },
    {GSDIDIAG_PKCS15_PRKDF, UIM_PKCS15_PRKDF, MMGSDI_PKCS15_PRKDF },
    {GSDIDIAG_PKCS15_PUKDF, UIM_PKCS15_PUKDF, MMGSDI_PKCS15_PUKDF },
    {GSDIDIAG_PKCS15_SKDF, UIM_PKCS15_SKDF, MMGSDI_PKCS15_SKDF },
    {GSDIDIAG_PKCS15_CDF, UIM_PKCS15_CDF, MMGSDI_PKCS15_CDF },
    {GSDIDIAG_PKCS15_DODF, UIM_PKCS15_DODF, MMGSDI_PKCS15_DODF },
    {GSDIDIAG_PKCS15_AODF, UIM_PKCS15_AODF, MMGSDI_PKCS15_AODF },
    {GSDIDIAG_PKCS15_TI, UIM_PKCS15_TI, MMGSDI_PKCS15_TI },
    {GSDIDIAG_PKCS15_US, UIM_PKCS15_US, MMGSDI_PKCS15_US }
};

/*=============================================================================
    Static table containing mapping between user specified elementary UIM
    command to UIM task internal values.
=============================================================================*/

static const gsdidiag_uim_cmd_uim_cmd_pair_type gsdidiag_gsdi_to_uim_cmd[] =
{
  {GSDIDIAG_UIM_CMD_NO_SUCH_UIM_CMD, UIM_NO_SUCH_COMMAND_F},
  {GSDIDIAG_UIM_CMD_UIM_RESET_SWITCH_UIM_PASSIVE_F, UIM_RESET_SWITCH_UIM_PASSIVE_F},
  {GSDIDIAG_UIM_CMD_UIM_POWER_DOWN_F, UIM_POWER_DOWN_F},
  {GSDIDIAG_UIM_CMD_UIM_POWER_UP_F, UIM_POWER_UP_UIM_PASSIVE_F},
  {GSDIDIAG_UIM_CMD_UIM_MANAGE_CHANNEL_F,UIM_MANAGE_CHANNEL_F},
  {GSDIDIAG_UIM_CMD_UIM_ACCESS_F, UIM_ACCESS_F},
  {GSDIDIAG_UIM_CMD_UIM_ISIM_AUTHENTICATE_F, UIM_ISIM_AUTHENTICATE_F},
  {GSDIDIAG_UIM_CMD_UIM_READ_BIN_CMD_F, UIM_READ_BIN_CMD_F},
  {GSDIDIAG_UIM_CMD_UIM_READ_REC_CMD_F, UIM_READ_REC_CMD_F},
  {GSDIDIAG_UIM_CMD_UIM_WRITE_BIN_CMD_F, UIM_WRITE_BIN_CMD_F},
  {GSDIDIAG_UIM_CMD_UIM_WRITE_REC_CMD_F, UIM_WRITE_REC_CMD_F},
  {GSDIDIAG_UIM_CMD_UIM_INC_CMD_F, UIM_INC_CMD_F},
  {GSDIDIAG_UIM_MAX_CMDS,UIM_MAX_F}
};


/*=============================================================================
    Static table containing mapping between user specified elementary UIM
    command to UIM task internal values.
=============================================================================*/

static const gsdidiag_uim_rpt_uim_rpt_pair_type gsdidiag_gsdi_to_uim_rpt[] =
{
  {GSDIDIAG_UIM_RPT_UIM_NO_SUCH_REPORT_R, UIM_NO_SUCH_REPORT_R},
  {GSDIDIAG_UIM_RPT_UIM_RESET_SWITCH_UIM_PASSIVE_R, UIM_RESET_SWITCH_UIM_PASSIVE_R},
  {GSDIDIAG_UIM_RPT_UIM_POWER_DOWN_R, UIM_POWER_DOWN_R},
  {GSDIDIAG_UIM_RPT_UIM_POWER_UP_R, UIM_POWER_UP_UIM_PASSIVE_R},
  {GSDIDIAG_UIM_RPT_UIM_MANAGE_CHANNEL_R,UIM_MANAGE_CHANNEL_R},
  {GSDIDIAG_UIM_RPT_UIM_ACCESS_R, UIM_ACCESS_R},
  {GSDIDIAG_UIM_RPT_UIM_ISIM_AUTHENTICATE_R, UIM_ISIM_AUTHENTICATE_R},
  {GSDIDIAG_UIM_RPT_UIM_READ_BIN_CMD_R, UIM_READ_BIN_CMD_R},
  {GSDIDIAG_UIM_RPT_UIM_READ_REC_CMD_R, UIM_READ_REC_CMD_R},
  {GSDIDIAG_UIM_RPT_UIM_WRITE_BIN_CMD_R, UIM_WRITE_BIN_CMD_R},
  {GSDIDIAG_UIM_RPT_UIM_WRITE_REC_CMD_R, UIM_WRITE_REC_CMD_R},
  {GSDIDIAG_UIM_RPT_UIM_INC_CMD_R, UIM_INC_CMD_R},
  {GSDIDIAG_UIM_MAX_RPTS,UIM_MAX_R}
};

/*=============================================================================
    Static table containing mapping between user specified protocol
    to UIM task internal values.
=============================================================================*/
static const gsdidiag_gsdi_protocol_uim_protocol_pair_type
  gsdidiag_gsdi_protocol_to_uim_protocol[] =
{
  {GSDIDIAG_UIM_NO_SUCH_MODE ,UIM_NO_SUCH_MODE},
  {GSDIDIAG_UIM_CDMA         ,UIM_CDMA},
  {GSDIDIAG_UIM_GSM          ,UIM_GSM},
  {GSDIDIAG_UIM_WCDMA        ,UIM_WCDMA},
  {GSDIDIAG_UIM_UICC         ,UIM_UICC},
  {GSDIDIAG_UIM_MAXIMUM_MODE ,UIM_MAXIMUM_MODE}
};

/*=============================================================================
    Static table containing mapping between user specified protocol
    to UIM task internal values.
=============================================================================*/
static const gsdidiag_gsdi_mc_uim_mc_pair_type gsdidiag_gsdi_mc_to_uim_mc[] =
{
  {GSDIDIAG_UIM_MC_OPEN,  UIM_MC_OPEN },
  {GSDIDIAG_UIM_MC_CLOSE, UIM_MC_CLOSE}
};

/*=============================================================================
    Static table containing mapping between user specified access type
    to UIM task internal values.
=============================================================================*/
static const gsdidiag_gsdi_access_uim_access_pair_type
gsdidiag_gsdi_access_to_uim_access[] =
{
  {GSDIDIAG_UIM_READ_F,  UIM_READ_F},
  {GSDIDIAG_UIM_WRITE_F, UIM_WRITE_F},
  {GSDIDIAG_UIM_INC_F,   UIM_INC_F}
};


/*=============================================================================
    Static table containing mapping between user specified access type
    to UIM task internal values.
=============================================================================*/
static const gsdidiag_gsdi_rec_mode_uim_rec_mode_pair_type
gsdidiag_gsdi_rec_mode_to_uim_rec_mode[] =
{
  {GSDIDIAG_UIM_CURRENT,  UIM_CURRENT},
  {GSDIDIAG_UIM_NEXT,     UIM_NEXT},
  {GSDIDIAG_UIM_PREVIOUS, UIM_PREVIOUS},
  {GSDIDIAG_UIM_ABSOLUTE, UIM_ABSOLUTE}
};

/*=============================================================================
    S T A T I C    F U N C T I O N    P R O T O T Y P E S
=============================================================================*/

static uim_record_mode_type gsdidiag_map_gsdi_rec_mode_to_uim_rec_mode(
  gsdidiag_uim_record_mode_type gsdidiag_rec_mode
);

static uim_access_type gsdidiag_map_gsdi_access_to_uim_access(
  gsdidiag_uim_access_type gsdidiag_access
);

static uim_mc_action_type gsdidiag_map_gsdi_mc_to_uim_mc(
  gsdidiag_uim_mc_action_type gsdidiag_mc
);

static uim_cmd_name_type gsdidiag_map_gsdi_cmd_to_uim_cmd(
  gsdidiag_uim_cmd_type gsdidiag_uim_cmd
);

static gsdidiag_uim_rpt_code_type gsdidiag_map_uim_rpt_to_gsdi_rpt(
  uim_report_code_type uim_rpt_code
);

static uim_protocol_type gsdidiag_map_gsdi_protocol_to_uim_protocol(
  gsdidiag_uim_protocol_type gsdidiag_uim_protocol
);


/*===========================================================================
FUNCTION gsdidiag_map_gsdi_rec_mode_to_uim_rec_mode()

DESCRIPTION
    Search through the mapping table and return the corresponding
    uim_access_type based on the passed in gsdidiag_uim_access_type

DEPENDENCIES
    None

RETURN VALUE
    uim_cmd_name_type

SIDE EFFECTS
    None
===========================================================================*/
static  uim_record_mode_type gsdidiag_map_gsdi_rec_mode_to_uim_rec_mode(
  gsdidiag_uim_record_mode_type gsdidiag_rec_mode)
{
  int i = 0;
  int count = ARR_SIZE(gsdidiag_gsdi_rec_mode_to_uim_rec_mode);

  for(i=0; i<count; i++)
  {
    if (gsdidiag_gsdi_rec_mode_to_uim_rec_mode[i].gsdidiag_rec_mode ==
        gsdidiag_rec_mode)
      return gsdidiag_gsdi_rec_mode_to_uim_rec_mode[i].uim_record_mode;
  }

  return (uim_record_mode_type) gsdidiag_rec_mode;
}



/*===========================================================================
FUNCTION gsdidiag_map_gsdi_access_to_uim_access()

DESCRIPTION
    Search through the mapping table and return the corresponding
    uim_access_type based on the passed in gsdidiag_uim_access_type

DEPENDENCIES
    None

RETURN VALUE
    uim_cmd_name_type

SIDE EFFECTS
    None
===========================================================================*/
static uim_access_type gsdidiag_map_gsdi_access_to_uim_access(
  gsdidiag_uim_access_type gsdidiag_access
)
{
  int i = 0;
  int count = ARR_SIZE(gsdidiag_gsdi_access_to_uim_access);

  for(i=0; i<count; i++)
  {
    if (gsdidiag_gsdi_access_to_uim_access[i].gsdidiag_access ==
        gsdidiag_access)
      return gsdidiag_gsdi_access_to_uim_access[i].uim_access;
  }

  return (uim_access_type) gsdidiag_access;
}



/*===========================================================================
FUNCTION gsdidiag_map_gsdi_mc_to_uim_mc()

DESCRIPTION
    Search through the mapping table and return the corresponding
    uim_cmd_name_type based on the passed in gsdidiag_uim_cmd_type

DEPENDENCIES
    None

RETURN VALUE
    uim_cmd_name_type

SIDE EFFECTS
    None
===========================================================================*/
static uim_mc_action_type gsdidiag_map_gsdi_mc_to_uim_mc(
  gsdidiag_uim_mc_action_type gsdidiag_mc
)
{
  int i = 0;
  int count = ARR_SIZE(gsdidiag_gsdi_mc_to_uim_mc);

  for(i=0; i<count; i++)
  {
    if (gsdidiag_gsdi_mc_to_uim_mc[i].gsdidiag_uim_mc ==
        gsdidiag_mc)
      return gsdidiag_gsdi_mc_to_uim_mc[i].uim_mc;
  }

  return (uim_mc_action_type) gsdidiag_mc;
}


/*===========================================================================
FUNCTION gsdidiag_map_gsdi_cmd_to_uim_cmd()

DESCRIPTION
    Search through the mapping table and return the corresponding
    uim_cmd_name_type based on the passed in gsdidiag_uim_cmd_type

DEPENDENCIES
    None

RETURN VALUE
    uim_cmd_name_type

SIDE EFFECTS
    None
===========================================================================*/
static uim_cmd_name_type gsdidiag_map_gsdi_cmd_to_uim_cmd(
  gsdidiag_uim_cmd_type gsdidiag_uim_cmd
)
{
  int i = 0;
  int count = ARR_SIZE(gsdidiag_gsdi_to_uim_cmd);

  if ( gsdidiag_uim_cmd >= GSDIDIAG_UIM_MAX_CMDS )
    return UIM_NO_SUCH_COMMAND_F;

  for(i=0; i<count; i++)
  {
    if (gsdidiag_gsdi_to_uim_cmd[i].gsdidiag_uim_cmd ==
        gsdidiag_uim_cmd)
      return gsdidiag_gsdi_to_uim_cmd[i].uim_cmd_name;
  }

  return UIM_NO_SUCH_COMMAND_F;
}

/*===========================================================================
FUNCTION gsdidiag_map_uim_rpt_to_gsdi_rpt()

DESCRIPTION
    Search through the mapping table and return the corresponding
    uim_cmd_name_type based on the passed in gsdidiag_uim_cmd_type

DEPENDENCIES
    None

RETURN VALUE
    uim_cmd_name_type

SIDE EFFECTS
    None
===========================================================================*/

static gsdidiag_uim_rpt_code_type gsdidiag_map_uim_rpt_to_gsdi_rpt(
  uim_report_code_type uim_rpt_code)
{
  int i = 0;
  int count = ARR_SIZE(gsdidiag_gsdi_to_uim_rpt);

  if ( uim_rpt_code == UIM_MAX_R )
    return GSDIDIAG_UIM_RPT_UIM_NO_SUCH_REPORT_R;

  for(i=0; i<count; i++)
  {
    if (gsdidiag_gsdi_to_uim_rpt[i].uim_rpt_code ==
        uim_rpt_code)
      return gsdidiag_gsdi_to_uim_rpt[i].gsdidiag_uim_rpt_code;
  }

  return GSDIDIAG_UIM_RPT_UIM_NO_SUCH_REPORT_R;
}


/*===========================================================================
FUNCTION gsdidiag_map_gsdi_protocol_to_uim_protocol()

DESCRIPTION
    Search through the mapping table and return the corresponding
    uim_protocol_type based on the passed in gsdidiag_uim_protocol_type

DEPENDENCIES
    None

RETURN VALUE
    uim_protocol_type

SIDE EFFECTS
    None
===========================================================================*/

static uim_protocol_type gsdidiag_map_gsdi_protocol_to_uim_protocol(
  gsdidiag_uim_protocol_type gsdidiag_uim_protocol
  )
{
  int i = 0;
  int count = ARR_SIZE(gsdidiag_gsdi_protocol_to_uim_protocol);

  for(i=0; i<count; i++)
  {
    if (gsdidiag_gsdi_protocol_to_uim_protocol[i].gsdidiag_uim_protocol ==
        gsdidiag_uim_protocol)
      return gsdidiag_gsdi_protocol_to_uim_protocol[i].uim_protocol;
  }

  return (uim_protocol_type) gsdidiag_uim_protocol;
}

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
uim_items_enum_type gsdidiag_convert_file_to_uim_enum(gsdidiag_elementary_file_enum_type gsdidiag_file)
{
  int i;
  int count = ARR_SIZE(gsdidiag_file_to_uim_table);

  for (i = 0; i< count; i++) {
    if (gsdidiag_file_to_uim_table[i].gsdidiag_file == gsdidiag_file)
      return gsdidiag_file_to_uim_table[i].uim_file;
  }

  return UIM_NO_SUCH_ITEM;
}


gsdidiag_elementary_file_enum_type gsdidiag_convert_uim_file_to_gsdidiag_file(
  uint16                  uim_file)
{
  int i;
  int count = ARR_SIZE(gsdidiag_file_to_uim_table);

  for (i = 0; i< count; i++) {
    if (gsdidiag_file_to_uim_table[i].uim_file == (uim_items_enum_type)uim_file)
      return gsdidiag_file_to_uim_table[i].gsdidiag_file ;
  }
  return GSDIDIAG_MAX_FILE_ENUM;
}

/*===========================================================================
FUNCTION gsdidiag_convert_file_from_mmgsdi_enum()

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
gsdidiag_elementary_file_enum_type gsdidiag_convert_file_from_mmgsdi_enum(
    mmgsdi_file_enum_type mmgsdi_file )
{
  int i;
  int count = ARR_SIZE(gsdidiag_file_to_uim_table);

  for (i = 0; i< count; i++) {
    if (gsdidiag_file_to_uim_table[i].mmgsdi_file == mmgsdi_file)
      return gsdidiag_file_to_uim_table[i].gsdidiag_file ;
  }
  return GSDIDIAG_MAX_FILE_ENUM;
} /* gsdidiag_convert_file_from_mmgsdi_enum */

/*===========================================================================
FUNCTION gsdidiag_convert_file_to_mmgsdi_enum()

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
mmgsdi_file_enum_type gsdidiag_convert_file_to_mmgsdi_enum(
    gsdidiag_elementary_file_enum_type gsdidiag_file)
{
  int i;
  int count = ARR_SIZE(gsdidiag_file_to_uim_table);

  for (i = 0; i< count; i++) {
    if (gsdidiag_file_to_uim_table[i].gsdidiag_file == gsdidiag_file)
      return gsdidiag_file_to_uim_table[i].mmgsdi_file;
  }
  return MMGSDI_NO_FILE_ENUM;
} /* gsdidiag_convert_file_to_mmgsdi_enum */


/*===========================================================================
FUNCTION gsdidiag_get_uim_cmd_payload

DESCRIPTION
  Populates the request structures with the data from the request packet.

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
void gsdidiag_get_uim_cmd_payload(
  gsdidiag_uim_cmd_req_type *uim_req_ptr,
  PACKED void * req,
  uint32 offset
)
{
  if (uim_req_ptr == NULL ||
      req == NULL)
  {
    return;
  }

  /* increment req pointer to obtain position of next data element */
  req = (char*)req + offset;

  /* Extract protocol from req packet and store it
   * in the request pointer
   */
  memcpy( ((uint32*)&(uim_req_ptr->gsdidiag_uim_protocol)),
           (uint32*)req,
           sizeof(uint32));
  req = (char*)req + sizeof(uint32);

  /* Extract gsdidiag_uim_cmd req packet and store it
   * in the request pointer
   */
  memcpy( ((uint32*)&(uim_req_ptr->gsdidiag_uim_cmd)),
           (uint32*)req,
           sizeof(uint32));
  req = (char*)req + sizeof(uint32);

  /* Extract the user data  */
  memcpy( ((uint32*)&(uim_req_ptr->user_data)),
           (uint32*)req,
           sizeof(uint32));
  req = (char*)req + sizeof(uint32);
  
  /* Extract the logical channel number */
  memcpy( ((uint32*)&(uim_req_ptr->logical_channel)),
           (uint32*)req,
           sizeof(uint32));
  req = (char*)req + sizeof(uint32);


  switch(uim_req_ptr->gsdidiag_uim_cmd)
  {
    case GSDIDIAG_UIM_CMD_UIM_MANAGE_CHANNEL_F:
    {
      /* Copy the packed request to our packed structure */
      memcpy((void *)&(uim_req_ptr->params.mng_ch_req), (void*)req,
             sizeof(uim_req_ptr->params.mng_ch_req));

    }
    break;

    case GSDIDIAG_UIM_CMD_UIM_ACCESS_F:
    {
      memcpy((void *)&(uim_req_ptr->params.access_req), (void *)req,
             sizeof(uim_req_ptr->params.access_req));
    }
    break;

    case GSDIDIAG_UIM_CMD_UIM_ISIM_AUTHENTICATE_F:
    {
      /* move past the authentication context... not being used right now
       * Will have to pupulate when necessary
       */
      req = (char*)req + sizeof(uint32);

      /* We need to copy the authenticate command parameters here*/
      memcpy ((void *) &(uim_req_ptr->params.isim_autn_req.num_bytes),
               (void *)req,
              sizeof(uint32));

      req = (char *)req + sizeof(uint32);

      /* allocate enough memory to copy the rest of the
       * rest of the authentication request
       */

      uim_req_ptr->params.isim_autn_req.data =
        mmgsdi_malloc(
                    (size_t) uim_req_ptr->params.isim_autn_req.num_bytes);

      if (uim_req_ptr->params.isim_autn_req.data == NULL)
      {
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        break;
      }

      /* copy the actual data */
      memcpy( (void *)uim_req_ptr->params.isim_autn_req.data,
              (void *)req,
              (size_t) uim_req_ptr->params.isim_autn_req.num_bytes );

    }
    break;

    case GSDIDIAG_UIM_CMD_UIM_READ_BIN_CMD_F:
    {
      memcpy((void *)&(uim_req_ptr->params.read_bin), (void *)req,
             sizeof(uim_req_ptr->params.read_bin));
    }
    break;

    case GSDIDIAG_UIM_CMD_UIM_READ_REC_CMD_F:
    {
      memcpy((void *)&(uim_req_ptr->params.read_rec), (void *)req,
             sizeof(uim_req_ptr->params.read_rec));
    }
    break;

    case GSDIDIAG_UIM_CMD_UIM_WRITE_BIN_CMD_F:
    {
      memcpy((void *)&(uim_req_ptr->params.write_bin), (void *)req,
             sizeof(uim_req_ptr->params.write_bin));

      uim_req_ptr->params.write_bin.data =
        mmgsdi_malloc(
                    (size_t) uim_req_ptr->params.write_bin.len);

      if (uim_req_ptr->params.write_bin.data == NULL)
      {
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        break;
      }

      /* Locate the offset where the data starts */
      offset = sizeof(uim_req_ptr->params.write_bin) -
        sizeof(uim_req_ptr->params.write_bin.data);

      req = (char *)req + offset;

      /* copy the actual data */
      memcpy( (void *)uim_req_ptr->params.write_bin.data,
              (void *)req,
              (size_t) uim_req_ptr->params.write_bin.len);
    }
    break;

    case GSDIDIAG_UIM_CMD_UIM_WRITE_REC_CMD_F:
    {
      memcpy((void *)&(uim_req_ptr->params.write_rec), (void *)req,
             sizeof(uim_req_ptr->params.write_rec));

      uim_req_ptr->params.write_rec.data =
        mmgsdi_malloc(
                    (size_t) uim_req_ptr->params.write_rec.len);

      if (uim_req_ptr->params.write_rec.data == NULL)
      {
        MMGSDI_DEBUG_MSG_ERROR("mmgsdi_malloc failed",0,0,0);
        break;
      }

      /* Locate the offset where the data starts */
      offset = sizeof(uim_req_ptr->params.write_rec) -
        sizeof(uim_req_ptr->params.write_rec.data);

      req = (char *)req + offset;

      /* copy the actual data */
      memcpy( (void *)uim_req_ptr->params.write_rec.data,
              (void *)req,
              (size_t) uim_req_ptr->params.write_rec.len);
    }
    break;

    case GSDIDIAG_UIM_CMD_UIM_INC_CMD_F:
    {
      memcpy((void *)&(uim_req_ptr->params.inc), (void *)req,
             sizeof(uim_req_ptr->params.inc));

      uim_req_ptr->params.inc.data =
        mmgsdi_malloc(
                    (size_t) uim_req_ptr->params.inc.len);

      if (uim_req_ptr->params.inc.data == NULL)
      {
        MMGSDI_DEBUG_MSG_ERROR("mem_malloc failed",0,0,0);
        break;
      }

      /* Locate the offset where the data starts */
      offset = sizeof(uim_req_ptr->params.inc) -
        sizeof(uim_req_ptr->params.inc.data);

      req = (char *)req + offset;

      /* copy the actual data */
      memcpy( (void *)uim_req_ptr->params.inc.data,
              (void *)req,
              (size_t) uim_req_ptr->params.inc.len);
    }
    break;

  default:
    break;
  }
} /*gsdidiag_get_uim_cmd_payload*/

/*===========================================================================
FUNCTION gsdidiag_free_uim_req_mem

DESCRIPTION
  Frees up any memory that might be allocated memory based on the command type

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
void gsdidiag_free_uim_req_mem(
    gsdidiag_uim_cmd_req_type *uim_req_ptr
)
{

  if (uim_req_ptr == NULL)
  {
    MSG_ERROR("Null uim req ptr", 0, 0, 0);
    return;
  }

  switch (uim_req_ptr->gsdidiag_uim_cmd)
  {
    case GSDIDIAG_UIM_CMD_UIM_ISIM_AUTHENTICATE_F:
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_req_ptr->params.isim_autn_req.data);
    }
    break;

    case GSDIDIAG_UIM_CMD_UIM_WRITE_BIN_CMD_F:
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_req_ptr->params.write_bin.data);
    }
    break;

    case GSDIDIAG_UIM_CMD_UIM_WRITE_REC_CMD_F:
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_req_ptr->params.write_rec.data);
    }
    break;

    case GSDIDIAG_UIM_CMD_UIM_INC_CMD_F:
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_req_ptr->params.inc.data);
    }
    break;

    default:
    break;
  }
}


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
)
{
  gsdidiag_uim_cmd_type      gsdidiag_uim_cmd = GSDIDIAG_UIM_CMD_NO_SUCH_UIM_CMD;
  gsdi_returns_T             gsdi_status  = GSDI_SUCCESS;
  gsdidiag_uim_protocol_type gsdidiag_uim_protocol = GSDIDIAG_UIM_NO_SUCH_MODE;

  if (req_ptr  == NULL ||
      data_ptr == NULL)
  {
    return GSDI_ERROR;
  }

  if (uim_cmd_ptr == NULL)
  {
    MSG_ERROR("Null uim cmd ptr", 0, 0, 0);
    return GSDI_ERROR;
  }

  /* Initialize data_ptr to NULL */
  *data_ptr = NULL;

  /* Get the uim command command out from the request */
  gsdidiag_uim_cmd = req_ptr->gsdidiag_uim_cmd;

  /* Populate the UIM command from the translation table */
  uim_cmd_ptr->hdr.command = gsdidiag_map_gsdi_cmd_to_uim_cmd(gsdidiag_uim_cmd);

  /* Populate information from the request */
  uim_cmd_ptr->hdr.slot     = (uim_slot_type)slot_id;

  /* Populate the protocol information */
  gsdidiag_uim_protocol = req_ptr->gsdidiag_uim_protocol;
  uim_cmd_ptr->hdr.protocol =
    gsdidiag_map_gsdi_protocol_to_uim_protocol(gsdidiag_uim_protocol);

  /* We will have to populate the channel information based on
   * whether the channel management is supported or not
   */
   uim_cmd_ptr->hdr.channel = (uim_channel_type)
     int32touint8(req_ptr->logical_channel);

  /*
   * Add code to fill in any additional information based on the command
   * if need be.
   */

  switch(uim_cmd_ptr->hdr.command)
  {
    case UIM_RESET_SWITCH_UIM_PASSIVE_F:
    case UIM_POWER_UP_UIM_PASSIVE_F:
    case UIM_POWER_DOWN_F:
      break;
    case UIM_MANAGE_CHANNEL_F:
      {
        /* Fill in the additional information required */
        uim_cmd_ptr->manage_channel.channel =
          int32touint8(req_ptr->logical_channel);

        uim_cmd_ptr->manage_channel.op_close_channel =
          int32touint8(req_ptr->params.mng_ch_req.op_close_channel);

        uim_cmd_ptr->manage_channel.mc_action =
          gsdidiag_map_gsdi_mc_to_uim_mc(req_ptr->params.mng_ch_req.mc_action);
      }
      break;
    case UIM_ACCESS_F:
      {
        /* Fill in the item that needs to be accessed */
        uim_cmd_ptr->access_uim.item=
          gsdidiag_convert_file_to_uim_enum(req_ptr->params.access_req.item);

        /* Fill in the access type */
        uim_cmd_ptr->access_uim.access =
         gsdidiag_map_gsdi_access_to_uim_access(
           req_ptr->params.access_req.access);

        /* Fill in the record acess mode */
        uim_cmd_ptr->access_uim.rec_mode =
          gsdidiag_map_gsdi_rec_mode_to_uim_rec_mode(
            req_ptr->params.access_req.rec_mode);

        /* Fill in the number of bytes to be read */
        uim_cmd_ptr->access_uim.num_bytes =
          int32touint16(req_ptr->params.access_req.num_bytes);

        if (uim_cmd_ptr->access_uim.num_bytes != 0)
        {
          uim_cmd_ptr->access_uim.data_ptr =
            mmgsdi_malloc(uim_cmd_ptr->access_uim.num_bytes);

          if (uim_cmd_ptr->access_uim.data_ptr != NULL)
          {
            /* Store the allocated memory to the pointer provided*/
            *data_ptr = uim_cmd_ptr->access_uim.data_ptr;
          }
          else
          {
            /* Indicate that something went wrong */
            gsdi_status = GSDI_ERROR;

            break;
          }
        }
        else
        {
          uim_cmd_ptr->access_uim.data_ptr = NULL;
        }

        /* Fill in the offset */
        uim_cmd_ptr->access_uim.offset =
          int32touint16(req_ptr->params.access_req.offset);

        /* Fill in the path */
        memcpy((void *)uim_cmd_ptr->access_uim.path,
               (void *)req_ptr->params.access_req.path,
               sizeof(uim_cmd_ptr->access_uim.path));

        /* Fill in the aid information */

        uim_cmd_ptr->access_uim.aid.aid_length =
          app_id_len;

        if ((app_id_len > 0) && (app_id_len <= UIM_AID_MAX_NAME_LENGTH))
        {
          if (app_id != NULL)
          {
            memcpy((void *)uim_cmd_ptr->access_uim.aid.data,
               (void *)app_id,
               app_id_len);
          }
          else
          {
            MSG_ERROR("Null app id ptr with app id len = 0x%x", app_id_len, 0, 0);
            return GSDI_ERROR;
          }
        }
      }
      break;
    case UIM_READ_BIN_CMD_F:
      {
        /* Fill in the item that needs to be accessed */
        uim_cmd_ptr->read_bin.item =
          gsdidiag_convert_file_to_uim_enum(req_ptr->params.read_bin.item);

        uim_cmd_ptr->read_bin.offset =
          int32touint16(req_ptr->params.read_bin.offset);

        /* Fill in the number of bytes to be read */
        uim_cmd_ptr->read_bin.len =
          int32touint16(req_ptr->params.read_bin.num_bytes);

        /* Fill in the path */
        memcpy((void *)uim_cmd_ptr->read_bin.path,
               (void *)req_ptr->params.read_bin.path,
               sizeof(uim_cmd_ptr->read_bin.path));

        /* Fill in the aid information */

        uim_cmd_ptr->read_bin.aid.aid_length =
          app_id_len;

        if (app_id_len > 0 && app_id_len <= UIM_AID_MAX_NAME_LENGTH)
        {
          if (app_id != NULL)
          {
            memcpy((void *)uim_cmd_ptr->read_bin.aid.data,
               (void *)app_id,
               app_id_len);
          }
          else
          {
            MSG_ERROR("Null app id ptr with app id len = 0x%x", app_id_len, 0, 0);
            return GSDI_ERROR;
          }
        }
      }
      break;
    case UIM_READ_REC_CMD_F:
      {
        /* Fill in the item that needs to be accessed */
        uim_cmd_ptr->read_rec.item =
          gsdidiag_convert_file_to_uim_enum(req_ptr->params.read_rec.item);

        uim_cmd_ptr->read_rec.recmode =
          gsdidiag_map_gsdi_rec_mode_to_uim_rec_mode(req_ptr->params.read_rec.rec_mode);

        /* Fill in the number of bytes to be read */
        uim_cmd_ptr->read_rec.rec_num =
          int32touint8(req_ptr->params.read_rec.rec_num);

        /* Fill in the path */
        memcpy((void *)uim_cmd_ptr->read_rec.path,
               (void *)req_ptr->params.read_rec.path,
               sizeof(uim_cmd_ptr->read_rec.path));

        /* Fill in the aid information */

        uim_cmd_ptr->read_rec.aid.aid_length =
          app_id_len;

        if (app_id_len > 0 && app_id_len <= UIM_AID_MAX_NAME_LENGTH)
        {
          if (app_id != NULL)
          {
            memcpy((void *)uim_cmd_ptr->read_rec.aid.data,
               (void *)app_id,
               app_id_len);
          }
          else
          {
            MSG_ERROR("Null app id ptr with app id len = 0x%x", app_id_len, 0, 0);
            return GSDI_ERROR;
          }
        }
      }
      break;

    case UIM_WRITE_BIN_CMD_F:
      {
        /* Fill in the item that needs to be accessed */
        uim_cmd_ptr->write_bin.item =
          gsdidiag_convert_file_to_uim_enum(req_ptr->params.read_bin.item);

        uim_cmd_ptr->write_bin.offset =
          int32touint16(req_ptr->params.write_bin.offset);


        /* Fill in the path */
        memcpy((void *)uim_cmd_ptr->write_bin.path,
               (void *)req_ptr->params.write_bin.path,
               sizeof(uim_cmd_ptr->write_bin.path));

        /* Fill in the aid information */

        uim_cmd_ptr->read_bin.aid.aid_length =
          app_id_len;

        if (app_id_len > 0 && app_id_len <= UIM_AID_MAX_NAME_LENGTH)
        {
          if (app_id != NULL)
          {
            memcpy((void *)uim_cmd_ptr->read_bin.aid.data,
               (void *)app_id,
               app_id_len);
          }
          else
          {
            MSG_ERROR("Null app id ptr with app id len = 0x%x", app_id_len, 0, 0);
            return GSDI_ERROR;
          }
        }

        /* Fill in the number of bytes to be written */
        uim_cmd_ptr->write_bin.len =
          int32touint16(req_ptr->params.write_bin.len);

        if (uim_cmd_ptr->write_bin.len != 0)
        {
          uim_cmd_ptr->write_bin.data =
            mmgsdi_malloc(uim_cmd_ptr->write_bin.len);

          if (uim_cmd_ptr->write_bin.data != NULL)
          {
            /* Store the allocated memory to the pointer provided*/
            *data_ptr = uim_cmd_ptr->write_bin.data;
          }
          else
          {
            /* Indicate that something went wrong */
            gsdi_status = GSDI_ERROR;

            break;
          }
        }
        else
        {
          uim_cmd_ptr->write_bin.data = NULL;
        }

        /* copy the data to be written */
        memcpy((void *)uim_cmd_ptr->write_bin.data,
               req_ptr->params.write_bin.data,
               uim_cmd_ptr->write_bin.len);

      }
      break;

    case UIM_WRITE_REC_CMD_F:
      {
        /* Fill in the item that needs to be accessed */
        uim_cmd_ptr->write_rec.item =
          gsdidiag_convert_file_to_uim_enum(req_ptr->params.write_rec.item);

        uim_cmd_ptr->write_rec.recmode =
          gsdidiag_map_gsdi_rec_mode_to_uim_rec_mode(req_ptr->params.write_rec.rec_mode);

        /* Fill in the number of bytes to be read */
        uim_cmd_ptr->write_rec.rec_num =
          int32touint8(req_ptr->params.write_rec.rec_num);

        /* Fill in the path */
        memcpy((void *)uim_cmd_ptr->write_rec.path,
               (void *)req_ptr->params.write_rec.path,
               sizeof(uim_cmd_ptr->write_rec.path));

        /* Fill in the aid information */

        uim_cmd_ptr->write_rec.aid.aid_length =
          app_id_len;

        if (app_id_len > 0 && app_id_len <= UIM_AID_MAX_NAME_LENGTH)
        {
          if (app_id != NULL)
          {
            memcpy((void *)uim_cmd_ptr->write_rec.aid.data,
               (void *)app_id,
               app_id_len);
          }
          else
          {
            MSG_ERROR("Null app id ptr with app id len = 0x%x", app_id_len, 0, 0);
            return GSDI_ERROR;
          }
        }

        uim_cmd_ptr->write_rec.len =
          int32touint16(req_ptr->params.write_rec.len);

        if (uim_cmd_ptr->write_rec.len != 0)
        {
          uim_cmd_ptr->write_rec.data =
            mmgsdi_malloc(uim_cmd_ptr->write_rec.len);

          if (uim_cmd_ptr->write_rec.data != NULL)
          {
            /* Store the allocated memory to the pointer provided*/
            *data_ptr = uim_cmd_ptr->write_rec.data;
          }
          else
          {
            /* Indicate that something went wrong */
            gsdi_status = GSDI_ERROR;

            break;
          }
        }
        else
        {
          uim_cmd_ptr->write_rec.data = NULL;

          /* Need not do a memcpy... break out of the switch */
          break;
        }

        /* copy the data to be written */
        memcpy((void *)uim_cmd_ptr->write_rec.data,
               req_ptr->params.write_rec.data,
               uim_cmd_ptr->write_rec.len);

      }
      break;

      case UIM_INC_CMD_F:
      {
        /* Fill in the item that needs to be accessed */
        uim_cmd_ptr->inc.item =
          gsdidiag_convert_file_to_uim_enum(req_ptr->params.inc.item);

        /* Fill in the path */
        memcpy((void *)uim_cmd_ptr->inc.path,
               (void *)req_ptr->params.inc.path,
               sizeof(uim_cmd_ptr->inc.path));

        /* Fill in the aid information */

        uim_cmd_ptr->inc.aid.aid_length =
          app_id_len;

        if (app_id_len > 0 && app_id_len <= UIM_AID_MAX_NAME_LENGTH)
        {
          if (app_id != NULL)
          {
            memcpy((void *)uim_cmd_ptr->inc.aid.data,
               (void *)app_id,
               app_id_len);
          }
          else
          {
            MSG_ERROR("Null app id ptr with app id len = 0x%x", app_id_len, 0, 0);
            return GSDI_ERROR;
          }
        }

        uim_cmd_ptr->inc.len =
          int32touint16(req_ptr->params.inc.len);

        if (uim_cmd_ptr->inc.len != 0)
        {
          uim_cmd_ptr->inc.data =
            mmgsdi_malloc(uim_cmd_ptr->inc.len);

          if (uim_cmd_ptr->inc.data != NULL)
          {
            /* Store the allocated memory to the pointer provided*/
            *data_ptr = uim_cmd_ptr->inc.data;
          }
          else
          {
            /* Indicate that something went wrong */
            gsdi_status = GSDI_ERROR;

            break;
          }
        }
        else
        {
          uim_cmd_ptr->inc.data = NULL;
        }

        /* copy the data to be written */
        memcpy((void *)uim_cmd_ptr->inc.data,
               req_ptr->params.inc.data,
               uim_cmd_ptr->inc.len);

      }
      break;


#ifdef FEATURE_ISIM
    case UIM_ISIM_AUTHENTICATE_F:
      {
        if ( req_ptr->params.isim_autn_req.data == NULL )
        {
          /* memory allocation failure when trying to parse the request*/
          gsdi_status = GSDI_ERROR;
          break;
        }

        if ( req_ptr->params.isim_autn_req.num_bytes > UIM_MAX_CHARS )
        {
          MMGSDI_DEBUG_MSG_ERROR("ISIM Authentication data length high",
                                 req_ptr->params.isim_autn_req.num_bytes,0,0);
          /* Indicate that something went wrong */
          gsdi_status = GSDI_ERROR;

          break;
        }

        /* populate the context with a constant value */

        uim_cmd_ptr->isim_autn.auth_context = UIM_AUTN_ISIM_3G_CONTEXT;

        /* Fill in the aid information */

        uim_cmd_ptr->isim_autn.aid.aid_length =
          app_id_len;

        memcpy((void *)uim_cmd_ptr->isim_autn.aid.data,
               (void *)app_id,
               app_id_len);

        /* Coppy the memory to the uim command pointer */
        uim_cmd_ptr->isim_autn.num_bytes =
          int32touint8(req_ptr->params.isim_autn_req.num_bytes);

        memcpy( (void *)uim_cmd_ptr->isim_autn.data,
                (void *)req_ptr->params.isim_autn_req.data,
                uim_cmd_ptr->isim_autn.num_bytes);
      }
      break;
#endif /*FEATURE_ISIM*/
    default :
      /* Any command other than what we handle */
      gsdi_status = GSDI_ERROR;
      break;
  }

  return gsdi_status;
}



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
void gsdidiag_populate_uim_cmd_rsp(
  gsdidiag_rsp_uim_cmd_type  *uim_rsp_ptr,
  const uim_rpt_type* uim_data,
  const uim_cmd_type* uim_cmd_ptr
)
{
  if ( uim_rsp_ptr == NULL ||
       uim_data == NULL )
  {
    return;
  }

  uim_rsp_ptr->rpt_type   = gsdidiag_map_uim_rpt_to_gsdi_rpt(uim_data->rpt_type);
  uim_rsp_ptr->rpt_status = (int) uim_data->rpt_status;
  uim_rsp_ptr->sw1        = (int) uim_data->sw1;
  uim_rsp_ptr->sw2        = (int) uim_data->sw2;
  uim_rsp_ptr->cmd_transacted =
    (int) uim_data->cmd_transacted;

  /* Fill in the rest of the data only if it is a pass case */
  if (uim_data->rpt_status == UIM_PASS)
  {
    /* Which response are we looking at */
    switch (uim_data->rpt_type)
    {
      case UIM_RESET_SWITCH_UIM_PASSIVE_R:
      case UIM_POWER_UP_UIM_PASSIVE_R:
        {
          uim_rsp_ptr->uim_response.atr_response.num_atr_bytes =
            uim_data->rpt.rst_uim_passive.num_bytes;
          if (uim_data->rpt.rst_uim_passive.num_bytes > 0 )
          {
            if (uim_data->rpt.rst_uim_passive.num_bytes <= UIM_MAX_ATR_CHARS)
            {
              memcpy((uint8 *)uim_rsp_ptr->uim_response.atr_response.atr_bytes,
                     (uint8 *)uim_data->rpt.rst_uim_passive.data,
                     uim_data->rpt.rst_uim_passive.num_bytes);
            }
            else
            {
              MMGSDI_DEBUG_MSG_ERROR("ATR Len %d is greater than expected",
                         uim_data->rpt.rst_uim_passive.num_bytes,0,0);
              memcpy((uint8 *) uim_rsp_ptr->uim_response.atr_response.atr_bytes,
                     (uint8 *) uim_data->rpt.rst_uim_passive.data,
                     UIM_MAX_ATR_CHARS);
            }
          }
          else
          {
            MMGSDI_DEBUG_MSG_ERROR("ATR Len %d is less than 0",
                       uim_data->rpt.rst_uim_passive.num_bytes,0,0);
            memset((uint8 *)uim_rsp_ptr->uim_response.atr_response.atr_bytes,
                   0x0,
                   UIM_MAX_ATR_CHARS);
          }
        }
        break;
      case UIM_MANAGE_CHANNEL_R:
        {
          uim_rsp_ptr->uim_response.manage_channel_response.channel =
            uim_data->rpt.channel.channel_num;
        }
        break;
      case UIM_ACCESS_R:
        {
          if (uim_cmd_ptr == NULL)
          {
            MSG_ERROR("Null uim cmd ptr", 0, 0, 0);
            return;
          }

          uim_rsp_ptr->uim_response.uim_access_rsp.num_response_bytes =
            uim_cmd_ptr->access_uim.num_bytes_rsp;

          memcpy((void *)uim_rsp_ptr->uim_response.uim_access_rsp.data,
                 uim_cmd_ptr->access_uim.data_ptr,
                 uim_cmd_ptr->access_uim.num_bytes_rsp);

          uim_rsp_ptr->uim_response.uim_access_rsp.num_records_rsp =
            uim_cmd_ptr->access_uim.num_records_rsp;

        }
        break;
#ifdef FEATURE_UIM_ISIM
      case UIM_ISIM_AUTHENTICATE_R:
        {
          /* Copy the length of response */
          uim_rsp_ptr->uim_response.uim_isim_autn_rsp.data_length =
            uim_data->rpt.isim_autn.data_length;

          /* Copy the data itself */
          memcpy ( (void *) uim_rsp_ptr->uim_response.uim_isim_autn_rsp.data,
                   (void *)uim_data->rpt.isim_autn.data,
                   (uint32)uim_data->rpt.isim_autn.data_length);
        }
        break;

      case UIM_READ_BIN_CMD_R:
      case UIM_READ_REC_CMD_R:
        {
          if (uim_data->rpt.read.num_bytes_read >
              sizeof(uim_rsp_ptr->uim_response.uim_read_rsp.data))
          {
            MSG_ERROR("TRUNCATING response to fit the response buffer",
                      sizeof(uim_rsp_ptr->uim_response.uim_read_rsp.data),0,0);

            uim_rsp_ptr->uim_response.uim_read_rsp.data_length =
              sizeof(uim_rsp_ptr->uim_response.uim_read_rsp.data);
          }
          else
          {
            uim_rsp_ptr->uim_response.uim_read_rsp.data_length =
              uim_data->rpt.read.num_bytes_read;
          }

          /* copy as much data as we can fit to the response */
          memcpy((void *)uim_rsp_ptr->uim_response.uim_read_rsp.data,
                 (void *)uim_data->rpt.read.data,
                 int32touint32(uim_rsp_ptr->uim_response.uim_read_rsp.data_length));
        }
        break;
      case UIM_WRITE_BIN_CMD_R:
      case UIM_WRITE_REC_CMD_R:
        {
          uim_rsp_ptr->uim_response.uim_write_rsp.bytes_written
            = uim_data->rpt.write.num_bytes_written;
        }
        break;
      case UIM_INC_CMD_R:
      {
        if (uim_data->rpt.inc.inc_rsp_len >
            sizeof(uim_rsp_ptr->uim_response.inc_rsp.data))
        {
          MSG_ERROR("TRUNCATING response to fit the response buffer",
                    sizeof(uim_rsp_ptr->uim_response.inc_rsp.data),0,0);

          uim_rsp_ptr->uim_response.inc_rsp.data_length =
            sizeof(uim_rsp_ptr->uim_response.inc_rsp.data);
        }
        else
        {
          uim_rsp_ptr->uim_response.inc_rsp.data_length =
            uim_data->rpt.inc.inc_rsp_len;
        }

        /* copy as much data as we can fit to the response */
        memcpy((void *)uim_rsp_ptr->uim_response.inc_rsp.data,
               (void *)uim_data->rpt.inc.data,
               int32touint32(uim_rsp_ptr->uim_response.inc_rsp.data_length));
      }
      break;

#endif /*FEATURE_UIM_ISIM*/
      default:
        break;
    }
  }
}

#endif /* FEATURE_GSDI_TEST_AUTOMATION */

