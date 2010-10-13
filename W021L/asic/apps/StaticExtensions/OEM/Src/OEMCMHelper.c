/*=============================================================================

FILE: OEMCMHelper.c

GENERAL DESCRIPTION: Helper functions used only by OEMCM.c.

        Copyright c 2003-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
/*=============================================================================

$Header: //depot/asic/msmshared/apps/StaticExtensions/OEM/Src/OEMCMHelper.c#3 $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------
11/08/07     alb     Remove CM_API_MBMS ifdefs
11/02/07     alb     Add MBMS helpers.
08/06/07     alb     Created.
=============================================================================*/


/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "OEMCMHelper.h"
#include "err.h"

#ifdef FEATURE_MBMS
#include "sys_gw.h"
#include "cm.h"
#endif

typedef struct 
{
   prl_acq_rec_type SD;
   AEECMPrlAcqRecordType AEECM;
}acqTypeConv;

//This array provides the conversion between 
// prl_acq_rec_type(prl.h) and AEECMPrlAcqRecordType(AEECM.h)
static acqTypeConv acqTypeConvTbl[] =
{
  {PRL_ACQ_REC_RESERVED,                AEECM_PRL_ACQ_REC_RESERVED},
  {PRL_ACQ_REC_CELLULAR_ANALOG,         AEECM_PRL_ACQ_REC_CELLULAR_ANALOG},
  {PRL_ACQ_REC_CELLULAR_CDMA_STD,       AEECM_PRL_ACQ_REC_CELLULAR_CDMA_STD},
  {PRL_ACQ_REC_CELLULAR_CDMA_CUST,      AEECM_PRL_ACQ_REC_CELLULAR_CDMA_CUST},
  {PRL_ACQ_REC_CELLULAR_GENERIC,        AEECM_PRL_ACQ_REC_CELLULAR_GENERIC},
  {PRL_ACQ_REC_PCS_CDMA_BLOCKS,         AEECM_PRL_ACQ_REC_PCS_CDMA_BLOCKS},
  {PRL_ACQ_REC_PCS_CDMA_CHANNELS,       AEECM_PRL_ACQ_REC_PCS_CDMA_CHANNELS},
  {PRL_ACQ_REC_JTACS_CDMA_STD,          AEECM_PRL_ACQ_REC_JTACS_CDMA_STD},
  {PRL_ACQ_REC_JTACS_CDMA_CUST,         AEECM_PRL_ACQ_REC_JTACS_CDMA_CUST},
  {PRL_ACQ_REC_CDMA_2GHZ_CHANS,         AEECM_PRL_ACQ_REC_CDMA_2GHZ_CHANS},
  {PRL_ACQ_REC_CDMA_GENERIC,            AEECM_PRL_ACQ_REC_CDMA_GENERIC},
  {PRL_ACQ_REC_HDR_GENERIC,             AEECM_PRL_ACQ_REC_HDR_GENERIC},
  {PRL_ACQ_REC_GSM_GENERIC,             AEECM_PRL_ACQ_REC_GSM_GENERIC},
  {PRL_ACQ_REC_WCDMA_GENERIC,           AEECM_PRL_ACQ_REC_WCDMA_GENERIC},
  {PRL_ACQ_REC_GW_PREFERRED,            AEECM_PRL_ACQ_REC_GW_PREFERRED},
  {PRL_ACQ_REC_NONE,                    AEECM_PRL_ACQ_REC_NONE}
};

typedef struct 
{
   prl_ab_type SD;
   AEECMPrlABType AEECM;
}prlABConv;

//This array provides the conversion between 
//prl_ab_type(prl.h) and AEECMPrlABType(AEECM.h)
static prlABConv prlABConvTbl[] =
{
   {PRL_AB_SYSTEM_A,      AEECM_PRL_AB_SYSTEM_A},
   {PRL_AB_SYSTEM_B,      AEECM_PRL_AB_SYSTEM_B},
   {PRL_AB_RESERVED,      AEECM_PRL_AB_RESERVED},
   {PRL_AB_SYSTEM_A_OR_B, AEECM_PRL_AB_SYSTEM_A_OR_B}
};

typedef struct 
{
   prl_pri_sec_type SD;
   AEECMPrlPriSecType AEECM;
}prlPriSecConv;

//This array provides the conversion between 
//prl_pri_sec_type(prl.h) and AEECMPrlPriSecType(AEECM.h) 
static prlPriSecConv prlPriSecConvTbl [] =
{
   {PRL_PRISEC_RESERVED,  AEECM_PRL_PRISEC_RESERVED},
   {PRL_PRISEC_PRIMARY,   AEECM_PRL_PRISEC_PRIMARY},
   {PRL_PRISEC_SECONDARY, AEECM_PRL_PRISEC_SECONDARY},
   {PRL_PRISEC_PRI_OR_SEC,AEECM_PRL_PRISEC_PRI_OR_SEC}
};

typedef struct
{
   prl_uarfcn_incl_e_type SD;
   AEECMPrlUarfcnInclType AEECM;
}prlUarfcnConv;

//This array provides the conversion between 
//prl_uarfcn_incl_e_type(prl.h) and AEECMPrlUarfcnInclType(AEECM.h) 
static prlUarfcnConv prlUarfcnConvTbl[] =
{
   {PRL_UARFCN_NOT_INCL,      AEECM_PRL_UARFCN_NOT_INCL},
   {PRL_UARFCN_INCL,          AEECM_PRL_UARFCN_INCL},
   {PRL_UARFCN_INCL_RESERVED, AEECM_PRL_UARFCN_INCL_RESERVED}
};

typedef struct 
{
   prl_gw_pref_sys_ind_e_type SD;
   AEECMPrlGwPrefSysIndType AEECM;
}prlGwPrefSysIndConv;

//This array provides the conversion between 
//prl_gw_pref_sys_ind_e_type(prl.h) and AEECMPrlGwPrefSysIndType(AEECM.h) 
static prlGwPrefSysIndConv prlGwPrefSysIndConvTbl[] = 
{
  {PRL_GW_PREF_SYS_IND_GSM,      AEECM_PRL_GW_PREF_SYS_IND_GSM},
  {PRL_GW_PREF_SYS_IND_WCDMA,    AEECM_PRL_GW_PREF_SYS_IND_WCDMA},
  {PRL_GW_PREF_SYS_IND_RESERVED, AEECM_PRL_GW_PREF_SYS_IND_RESERVED}
};

typedef struct
{
   sd_net_select_mode_e_type SD;
   AEECMSdNetSelectMode AEECM;   
}sdNetSelectModeConv;

//This array provides the conversion between 
//sd_net_select_mode_e_type(sd.h) and AEECMSdNetSelectMode(AEECM.h) 
static sdNetSelectModeConv sdNetSelectModeConvTbl[] = 
{
   {SD_NET_SELECT_MODE_LIMITED,        AEECM_SD_NET_SELECT_MODE_LIMITED},
   {SD_NET_SELECT_MODE_AUTO,           AEECM_SD_NET_SELECT_MODE_AUTO},
   {SD_NET_SELECT_MODE_HPLMN,          AEECM_SD_NET_SELECT_MODE_HPLMN},
   {SD_NET_SELECT_MODE_PRL_SYS,        AEECM_SD_NET_SELECT_MODE_PRL_SYS},
   {SD_NET_SELECT_MODE_MANUAL_SYS,     AEECM_SD_NET_SELECT_MODE_MANUAL_SYS},
   {SD_NET_SELECT_MODE_PRIV_SYS,       AEECM_SD_NET_SELECT_MODE_PRIV_SYS},
   {SD_NET_SELECT_MODE_MANUAL_BSSID,   AEECM_SD_NET_SELECT_MODE_MANUAL_BSS_ID},
   {SD_NET_SELECT_MODE_MAX,            AEECM_SD_NET_SELECT_MODE_MAX}
};
    
/*=============================================================================
FUNCTION: convToPrlAcqType

DESCRIPTION:
   Convert a value from AEECMPrlAcqRecordType to prl_acq_rec_type.
   Should be the same value, as the the enums matched value for value
   at the time of the writing of this function.
=============================================================================*/
static prl_acq_rec_type convToPrlAcqType(AEECMPrlAcqRecordType acqType)
{
   uint32 i;

   for (i=0; i < ARR_SIZE(acqTypeConvTbl); i++)
   {
      if( acqTypeConvTbl[i].AEECM == acqType)
      {
         return (acqTypeConvTbl[i].SD);
      }
   }

   ERR("convToPrlAcqType: %d not found",acqType,0,0);

   return PRL_ACQ_REC_NONE;
}

/*=============================================================================
FUNCTION: convFromPrlAcqType

DESCRIPTION:
   Convert a value from prl_acq_rec_type to AEECMPrlAcqRecordType.
   Should be the same value, as the the enums matched value for value
   at the time of the writing of this function.
=============================================================================*/
static AEECMPrlAcqRecordType convFromPrlAcqType(prl_acq_rec_type src)
{
   uint32 i;

   for (i=0; i < ARR_SIZE(acqTypeConvTbl); i++)
   {
      if( acqTypeConvTbl[i].SD == src)
      {
         return (acqTypeConvTbl[i].AEECM);
      }
   }

   ERR("convToPrlAcqType: %d not found",src,0,0);

   return AEECM_PRL_ACQ_REC_NONE;
}

/*=============================================================================
FUNCTION: convToPrlAB

DESCRIPTION:
   Convert a value from AEECMPrlABType to prl_ab_type.
   Should be the same value, as the the enums matched value for value
   at the time of the writing of this function.
=============================================================================*/
static prl_ab_type convToPrlAB(AEECMPrlABType srcPrlAB)
{
   uint32 i;

   for (i=0; i < ARR_SIZE(prlABConvTbl); i++)
   {
      if( prlABConvTbl[i].AEECM == srcPrlAB)
      {
         return (prlABConvTbl[i].SD);
      }
   }

   ERR("convToPrlAB: %d not found",srcPrlAB,0,0);

   return PRL_AB_SYSTEM_A;
}

/*=============================================================================
FUNCTION: convFromPrlAB

DESCRIPTION:
   Convert a value from prl_ab_type to AEECMPrlABType.
   Should be the same value, as the the enums matched value for value
   at the time of the writing of this function.
=============================================================================*/
static AEECMPrlABType convFromPrlAB(prl_ab_type src)
{
   uint32 i;

   for (i=0; i < ARR_SIZE(prlABConvTbl); i++)
   {
      if( prlABConvTbl[i].SD == src)
      {
         return (prlABConvTbl[i].AEECM);
      }
   }

   ERR("convToPrlAB: %d not found",src,0,0);

   return AEECM_PRL_AB_SYSTEM_A;
}

/*=============================================================================
FUNCTION: convToPrlPriSec

DESCRIPTION:
   Convert a value from AEECMPrlPriSecType to prl_pri_sec_type.
   Should be the same value, as the the enums matched value for value
   at the time of the writing of this function.
=============================================================================*/
static prl_pri_sec_type convToPrlPriSec(AEECMPrlPriSecType src)
{
   uint32 i;

   for (i=0; i < ARR_SIZE(prlPriSecConvTbl); i++)
   {
      if( prlPriSecConvTbl[i].AEECM == src)
      {
         return (prlPriSecConvTbl[i].SD);
      }
   }

   ERR("convToPrlPriSec: %d not found",src,0,0);

   return PRL_PRISEC_RESERVED;
}

/*=============================================================================
FUNCTION: convFromPrlPriSec

DESCRIPTION:
   Convert a value from prl_pri_sec_type to AEECMPrlPriSecType.
   Should be the same value, as the the enums matched value for value
   at the time of the writing of this function.
=============================================================================*/
static AEECMPrlPriSecType convFromPrlPriSec(prl_pri_sec_type src)
{
   uint32 i;

   for (i=0; i < ARR_SIZE(prlPriSecConvTbl); i++)
   {
      if( prlPriSecConvTbl[i].SD == src)
      {
         return (prlPriSecConvTbl[i].AEECM);
      }
   }

   ERR("convFromPrlPriSec: %d not found",src,0,0);

   return AEECM_PRL_PRISEC_RESERVED;
}

/*=============================================================================
FUNCTION: convToPrlGwPrefSysInd

DESCRIPTION:
   Convert a value from AEECMPrlGwPrefSysIndType to prl_gw_pref_sys_ind_e_type.
   Should be the same value, as the the enums matched value for value
   at the time of the writing of this function.
=============================================================================*/
static prl_gw_pref_sys_ind_e_type convToPrlGwPrefSysInd(AEECMPrlGwPrefSysIndType src)
{
   uint32 i;

   for (i=0; i < ARR_SIZE(prlGwPrefSysIndConvTbl); i++)
   {
      if( prlGwPrefSysIndConvTbl[i].AEECM == src)
      {
         return (prlGwPrefSysIndConvTbl[i].SD);
      }
   }

   ERR("convToPrlGwPrefSysInd: %d not found",src,0,0);

   return PRL_GW_PREF_SYS_IND_GSM;
}

/*=============================================================================
FUNCTION: convToPrlGwPrefSysInd

DESCRIPTION:
   Convert a value from prl_gw_pref_sys_ind_e_type to AEECMPrlGwPrefSysIndType.
   Should be the same value, as the the enums matched value for value
   at the time of the writing of this function.
=============================================================================*/
static AEECMPrlGwPrefSysIndType convFromPrlGwPrefSysInd(prl_gw_pref_sys_ind_e_type src)
{
   uint32 i;

   for (i=0; i < ARR_SIZE(prlGwPrefSysIndConvTbl); i++)
   {
      if( prlGwPrefSysIndConvTbl[i].SD == src)
      {
         return (prlGwPrefSysIndConvTbl[i].AEECM);
      }
   }

   ERR("convFromPrlGwPrefSysInd: %d not found",src,0,0);

   return AEECM_PRL_GW_PREF_SYS_IND_GSM;
}

/*=============================================================================
FUNCTION: copyToCellularCdma

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyToCellularCdma(prli_cellular_cdma_type *pDest, 
                               const AEECMPrlCellularCdmaType *pSrc)
{
   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   pDest->a_b = convToPrlAB(pSrc->a_b);
   pDest->pri_sec = convToPrlPriSec(pSrc->pri_sec);
}

/*=============================================================================
FUNCTION: copyFromCellularCdma

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyFromCellularCdma(AEECMPrlCellularCdmaType *pDest,
                                 const prli_cellular_cdma_type *pSrc)
{
   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   pDest->a_b = convFromPrlAB(pSrc->a_b);
   pDest->pri_sec = convFromPrlPriSec(pSrc->pri_sec);
}

/*=============================================================================
FUNCTION: copyToCellularCdmaCust

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyToCellularCdmaCust(prli_cellular_cdma_custom_type *pDest,
                                   const AEECMPrlCellularCdmaCustomType *pSrc)
                       
{
   uint32 i;

   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   pDest->num_chans = pSrc->num_chans;
   if (ARR_SIZE(pDest->chan) != ARR_SIZE(pSrc->chan))
   {
      ERR("copyToCellularCdmaCust: chan array mismatch",0,0,0);
      return;
   }

   for(i=0;i<ARR_SIZE(pDest->chan);i++)
   {
      pDest->chan[i] = pSrc->chan[i];
   }
}

/*=============================================================================
FUNCTION: copyFromCellularCdmaCust

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyFromCellularCdmaCust(AEECMPrlCellularCdmaCustomType *pDest,
                                     const prli_cellular_cdma_custom_type *pSrc)
                       
{
   uint32 i;

   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   pDest->num_chans = pSrc->num_chans;
   if (ARR_SIZE(pDest->chan) != ARR_SIZE(pSrc->chan))
   {
      ERR("copyFromCellularCdmaCust: chan array mismatch",0,0,0);
      return;
   }

   for(i=0;i<ARR_SIZE(pSrc->chan);i++)
   {
      pDest->chan[i] = pSrc->chan[i];
   }
}

/*=============================================================================
FUNCTION: copyToCellularGen

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyToCellularGen(prli_cellular_generic_type *pDest, 
                              const AEECMPrlCellularGenericType *pSrc)
{
   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   pDest->a_b = convToPrlAB(pSrc->a_b);
}

/*=============================================================================
FUNCTION: copyFromCellularGen

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyFromCellularGen(AEECMPrlCellularGenericType *pDest, 
                                const prli_cellular_generic_type *pSrc)
{
   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   pDest->a_b = convFromPrlAB(pSrc->a_b);
}

/*=============================================================================
FUNCTION: copyToPcsCdmaBlocks

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyToPcsCdmaBlocks(prli_pcs_cdma_block_type *pDest,
                                const AEECMPrlPcsCdmaBlockType *pSrc)
{
   uint32 i;

   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   pDest->num_blocks = pSrc->num_blocks;

   if (ARR_SIZE(pDest->block) != ARR_SIZE(pSrc->block))
   {
      ERR("copyToPcsCdmaBlocks: block array mismatch",0,0,0);
      return;
   }

   for(i=0;i < ARR_SIZE(pDest->block); i++)
   {
      pDest->block[i] = pSrc->block[i];
   }
}

/*=============================================================================
FUNCTION: copyFromPcsCdmaBlocks

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyFromPcsCdmaBlocks(AEECMPrlPcsCdmaBlockType *pDest,
                                  const prli_pcs_cdma_block_type *pSrc)
{
   uint32 i;

   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   pDest->num_blocks = pSrc->num_blocks;

   if (ARR_SIZE(pDest->block) != ARR_SIZE(pSrc->block))
   {
      ERR("copyFromPcsCdmaBlocks: block array mismatch",0,0,0);
      return;
   }

   for(i=0;i < ARR_SIZE(pDest->block); i++)
   {
      pDest->block[i] = pSrc->block[i];
   }
}

/*=============================================================================
FUNCTION: copyToPcsCdmaChannels

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyToPcsCdmaChannels(prli_pcs_cdma_chan_type *pDest, 
                                  const AEECMPrlCdmaChanType *pSrc)
{
   uint32 i;

   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   pDest->num_chans = pSrc->num_chans;

   if (ARR_SIZE(pDest->chan) != ARR_SIZE(pSrc->chan))
   {
      ERR("copyToPcsCdmaChannels: chan array mismatch",0,0,0);
      return;
   }

   for(i=0;i < ARR_SIZE(pDest->chan); i++)
   {
      pDest->chan[i] = pSrc->chan[i];
   }

}

/*=============================================================================
FUNCTION: copyFromPcsCdmaChannels

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyFromPcsCdmaChannels(AEECMPrlCdmaChanType *pDest,
                                    const prli_pcs_cdma_chan_type *pSrc)
{
   uint32 i;

   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   pDest->num_chans = pSrc->num_chans;

   if (ARR_SIZE(pDest->chan) != ARR_SIZE(pSrc->chan))
   {
      ERR("copyFromPcsCdmaChannels: chan array mismatch",0,0,0);
      return;
   }

   for(i=0;i < ARR_SIZE(pDest->chan); i++)
   {
      pDest->chan[i] = pSrc->chan[i];
   }

}

/*=============================================================================
FUNCTION: copyToJtacsCdmaStd

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyToJtacsCdmaStd(prli_jtacs_cdma_std_type *pDest,
                               const AEECMPrlJtacsCdmaStdType *pSrc)
{
   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   pDest->a_b = convToPrlAB(pSrc->a_b);
   pDest->pri_sec = convToPrlPriSec(pSrc->pri_sec);
}

/*=============================================================================
FUNCTION: copyFromJtacsCdmaStd

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyFromJtacsCdmaStd(AEECMPrlJtacsCdmaStdType *pDest,
                                 const  prli_jtacs_cdma_std_type *pSrc)
{
   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   pDest->a_b = convFromPrlAB(pSrc->a_b);
   pDest->pri_sec = convFromPrlPriSec(pSrc->pri_sec);
}

/*=============================================================================
FUNCTION: copyToJtacsCdmaCust

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyToJtacsCdmaCust(prli_jtacs_cdma_cust_type *pDest,
                                const AEECMPrlJtacsCdmaCustType *pSrc)
{
   uint32 i;

   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   pDest->num_chans = pSrc->num_chans;

   if (ARR_SIZE(pDest->chan) != ARR_SIZE(pSrc->chan))
   {
      ERR("copyToJtacsCdmaCust: chan array mismatch",0,0,0);
      return;
   }

   for(i=0;i < ARR_SIZE(pDest->chan); i++)
   {
      pDest->chan[i] = pSrc->chan[i];
   }
}

/*=============================================================================
FUNCTION: copyFromJtacsCdmaCust

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyFromJtacsCdmaCust(AEECMPrlJtacsCdmaCustType *pDest,
                                  const prli_jtacs_cdma_cust_type *pSrc)
{
   uint32 i;

   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   pDest->num_chans = pSrc->num_chans;

   if (ARR_SIZE(pDest->chan) != ARR_SIZE(pSrc->chan))
   {
      ERR("copyFromJtacsCdmaCust: chan array mismatch",0,0,0);
      return;
   }

   for(i=0;i < ARR_SIZE(pDest->chan); i++)
   {
      pDest->chan[i] = pSrc->chan[i];
   }
}

/*=============================================================================
FUNCTION: copyToCdma2GhzChans

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyToCdma2GhzChans(prli_cdma_2ghz_chans_type *pDest,
                                const AEECMPrlCdma2ghzChansType *pSrc)
{
   uint32 i;

   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   pDest->num_chans = pSrc->num_chans;

   if (ARR_SIZE(pDest->chan) != ARR_SIZE(pSrc->chan))
   {
      ERR("copyToCdma2GhzChans: chan array mismatch",0,0,0);
      return;
   }

   for(i=0;i < ARR_SIZE(pDest->chan); i++)
   {
      pDest->chan[i] = pSrc->chan[i];
   }
}

/*=============================================================================
FUNCTION: copyFromCdma2GhzChans

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyFromCdma2GhzChans(AEECMPrlCdma2ghzChansType *pDest,
                                  const prli_cdma_2ghz_chans_type *pSrc)
{
   uint32 i;

   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   pDest->num_chans = pSrc->num_chans;

   if (ARR_SIZE(pDest->chan) != ARR_SIZE(pSrc->chan))
   {
      ERR("copyFromCdma2GhzChans: chan array mismatch",0,0,0);
      return;
   }

   for(i=0;i < ARR_SIZE(pDest->chan); i++)
   {
      pDest->chan[i] = pSrc->chan[i];
   }
}

/*=============================================================================
FUNCTION: copyToCdmaGen

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyToCdmaGen(prli_cdma_generic_type *pDest, 
                          const AEECMPrlCdmaGenericType *pSrc)
{
   uint32 i;

   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   pDest->num_recs = pSrc->num_recs;

   if (ARR_SIZE(pDest->rec) != ARR_SIZE(pSrc->rec))
   {
      ERR("copyToCdmaGen: chan array mismatch",0,0,0);
      return;
   }

   for(i=0;i < ARR_SIZE(pDest->rec); i++)
   {
      pDest->rec[i] = pSrc->rec[i];
   }
   
}

/*=============================================================================
FUNCTION: copyFromCdmaGen

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyFromCdmaGen(AEECMPrlCdmaGenericType *pDest, 
                            prli_cdma_generic_type const *pSrc)
{
   uint32 i;

   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   pDest->num_recs = pSrc->num_recs;

   if (ARR_SIZE(pDest->rec) != ARR_SIZE(pSrc->rec))
   {
      ERR("copyFromCdmaGen: chan array mismatch",0,0,0);
      return;
   }

   for(i=0;i < ARR_SIZE(pDest->rec); i++)
   {
      pDest->rec[i] = pSrc->rec[i];
   }
   
}

/*=============================================================================
FUNCTION: copyToHdrGen

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyToHdrGen(prli_hdr_generic_type *pDest,
                         const AEECMPrlHdrGenericType *pSrc)
{
   uint32 i;

   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   pDest->num_recs = pSrc->num_recs;

   if (ARR_SIZE(pDest->rec) != ARR_SIZE(pSrc->rec))
   {
      ERR("copyToHdrGen: chan array mismatch",0,0,0);
      return;
   }

   for(i=0;i < ARR_SIZE(pDest->rec); i++)
   {
      pDest->rec[i] = pSrc->rec[i];
   }
}

/*=============================================================================
FUNCTION: copyFromHdrGen

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyFromHdrGen(AEECMPrlHdrGenericType *pDest,
                           const prli_hdr_generic_type *pSrc)
{
   uint32 i;

   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   pDest->num_recs = pSrc->num_recs;

   if (ARR_SIZE(pDest->rec) != ARR_SIZE(pSrc->rec))
   {
      ERR("copyFromHdrGen: chan array mismatch",0,0,0);
      return;
   }

   for(i=0;i < ARR_SIZE(pDest->rec); i++)
   {
      pDest->rec[i] = pSrc->rec[i];
   }
}

/*=============================================================================
FUNCTION: copyToGsmGen

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyToGsmGen(prli_gsm_generic_type *pDest, 
                         const AEECMPrlGsmGenericType *pSrc)
{
   uint32 i;

   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   pDest->num_bands = pSrc->num_bands;

   if (ARR_SIZE(pDest->band) != ARR_SIZE(pSrc->band))
   {
      ERR("copyToGsmGen: band array mismatch",0,0,0);
      return;
   }

   for(i=0;i < ARR_SIZE(pDest->band); i++)
   {
      pDest->band[i] = pSrc->band[i];
   }
}

/*=============================================================================
FUNCTION: copyFromGsmGen

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyFromGsmGen(AEECMPrlGsmGenericType *pDest, 
                           const prli_gsm_generic_type *pSrc)
{
   uint32 i;

   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   pDest->num_bands = pSrc->num_bands;

   if (ARR_SIZE(pDest->band) != ARR_SIZE(pSrc->band))
   {
      ERR("copyFromGsmGen: band array mismatch",0,0,0);
      return;
   }

   for(i=0;i < ARR_SIZE(pDest->band); i++)
   {
      pDest->band[i] = pSrc->band[i];
   }
}

/*=============================================================================
FUNCTION: convToPrlUarfcnIncl

DESCRIPTION:
   Convert a value from AEECMPrlUarfcnInclType to prl_uarfcn_incl_e_type.
   Should be the same value, as the the enums matched value for value
   at the time of the writing of this function.
=============================================================================*/
static prl_uarfcn_incl_e_type convToPrlUarfcnIncl(AEECMPrlUarfcnInclType src)
{
   uint32 i;

   for (i=0; i < ARR_SIZE(prlUarfcnConvTbl); i++)
   {
      if( prlUarfcnConvTbl[i].AEECM == src)
      {
         return (prlUarfcnConvTbl[i].SD);
      }
   }

   ERR("convToPrlUarfcnIncl: %d not found",src,0,0);

   return PRL_UARFCN_NOT_INCL;
}

/*=============================================================================
FUNCTION: convFromPrlUarfcnIncl

DESCRIPTION:
   Convert a value from AEECMPrlUarfcnInclType to prl_uarfcn_incl_e_type.
   Should be the same value, as the the enums matched value for value
   at the time of the writing of this function.
=============================================================================*/
static AEECMPrlUarfcnInclType convFromPrlUarfcnIncl(prl_uarfcn_incl_e_type src)
{
   uint32 i;

   for (i=0; i < ARR_SIZE(prlUarfcnConvTbl); i++)
   {
      if( prlUarfcnConvTbl[i].SD == src)
      {
         return (prlUarfcnConvTbl[i].AEECM);
      }
   }

   ERR("convFromPrlUarfcnIncl: %d not found",src,0,0);

   return AEECM_PRL_UARFCN_NOT_INCL;
}

/*=============================================================================
FUNCTION: copyToWdmaGenRec

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyToWdmaGenRec(prli_wcdma_generic_rec_type *pDest,
                             const AEECMPrlWcdmaGenericRecType *pSrc)
{
   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   pDest->uarfcn_incl = convToPrlUarfcnIncl(pSrc->uarfcn_incl);
   pDest->band = pSrc->band;
   pDest->uarfcn = pSrc->uarfcn;
}

/*=============================================================================
FUNCTION: copyFromWdmaGenRec

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyFromWdmaGenRec(AEECMPrlWcdmaGenericRecType *pDest,
                               const prli_wcdma_generic_rec_type *pSrc)
{
   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   pDest->uarfcn_incl = convFromPrlUarfcnIncl(pSrc->uarfcn_incl);
   pDest->band = pSrc->band;
   pDest->uarfcn = pSrc->uarfcn;
}

/*=============================================================================
FUNCTION: copyToWcdmaGen

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyToWcdmaGen(prli_wcdma_generic_type *pDest,
                           const AEECMPrlWcdmaGenericType *pSrc)
{
   uint32 i;

   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   pDest->num_bands = pSrc->num_bands;

   if (ARR_SIZE(pDest->rec) != ARR_SIZE(pSrc->rec))
   {
      ERR("copyToWcdmaGen: rec array mismatch",0,0,0);
      return;
   }

   for(i=0;i < ARR_SIZE(pDest->rec); i++)
   {
      copyToWdmaGenRec(&pDest->rec[i],&pSrc->rec[i]);
   }
}

/*=============================================================================
FUNCTION: copyFromWcdmaGen

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyFromWcdmaGen(AEECMPrlWcdmaGenericType *pDest,
                             const prli_wcdma_generic_type *pSrc)
{
   uint32 i;

   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   pDest->num_bands = pSrc->num_bands;

   if (ARR_SIZE(pDest->rec) != ARR_SIZE(pSrc->rec))
   {
      ERR("copyFromWcdmaGen: rec array mismatch",0,0,0);
      return;
   }

   for(i=0;i < ARR_SIZE(pDest->rec); i++)
   {
      copyFromWdmaGenRec(&pDest->rec[i],&pSrc->rec[i]);
   }
}


/*=============================================================================
FUNCTION: copyToGwPreferred

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyToGwPreferred(prli_gw_pref_type *pDest,
                              const AEECMPrlGwPrefType *pSrc)
{
   uint32 i;

   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   pDest->pref_sys_ind = convToPrlGwPrefSysInd(pSrc->pref_sys_ind);
   pDest->num_gsm_bands = pSrc->num_gsm_bands;
   pDest->num_wcdma_bands = pSrc->num_wcdma_bands;

   if ( ARR_SIZE(pDest->gsm_band) != ARR_SIZE(pSrc->gsm_band) )
   {
      ERR("copyToGwPreferred: gsm_band array mismatch",0,0,0);
      return;
   }

   for (i=0; i<ARR_SIZE(pDest->gsm_band); i++)
   {
      pDest->gsm_band[i] = pSrc->gsm_band[i];
   }

   if ( ARR_SIZE(pDest->wcdma_rec) != ARR_SIZE(pSrc->wcdma_rec) )
   {
      ERR("copyToGwPreferred: wcdma_rec array mismatch",0,0,0);
      return;
   }

   for (i=0; i<ARR_SIZE(pDest->wcdma_rec); i++)
   {
      copyToWdmaGenRec(&pDest->wcdma_rec[i], &pSrc->wcdma_rec[i]);
   }
}

/*=============================================================================
FUNCTION: copyFromGwPreferred

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyFromGwPreferred(AEECMPrlGwPrefType *pDest,
                                const prli_gw_pref_type *pSrc)
{
   uint32 i;

   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   pDest->pref_sys_ind = convFromPrlGwPrefSysInd(pSrc->pref_sys_ind);
   pDest->num_gsm_bands = pSrc->num_gsm_bands;
   pDest->num_wcdma_bands = pSrc->num_wcdma_bands;

   if ( ARR_SIZE(pDest->gsm_band) != ARR_SIZE(pSrc->gsm_band) )
   {
      ERR("copyFromGwPreferred: gsm_band array mismatch",0,0,0);
      return;
   }

   for (i=0; i<ARR_SIZE(pDest->gsm_band); i++)
   {
      pDest->gsm_band[i] = pSrc->gsm_band[i];
   }

   if ( ARR_SIZE(pDest->wcdma_rec) != ARR_SIZE(pSrc->wcdma_rec) )
   {
      ERR("copyFromGwPreferred: wcdma_rec array mismatch",0,0,0);
      return;
   }

   for (i=0; i<ARR_SIZE(pDest->wcdma_rec); i++)
   {
      copyFromWdmaGenRec(&pDest->wcdma_rec[i], &pSrc->wcdma_rec[i]);
   }
}

/*=============================================================================
FUNCTION: convToNetSelectMode

DESCRIPTION:
   Convert a value from AEECMSdNetSelectMode to sd_net_select_mode_e_type.
   Should be the same value, as the the enums matched value for value
   at the time of the writing of this function.
=============================================================================*/
static sd_net_select_mode_e_type convToNetSelectMode(AEECMSdNetSelectMode src)
{
   uint32 i;

   for (i=0; i < ARR_SIZE(sdNetSelectModeConvTbl); i++)
   {
      if( sdNetSelectModeConvTbl[i].AEECM == src)
      {
         return (sdNetSelectModeConvTbl[i].SD);
      }
   }

   ERR("convToNetSelectMode: %d not found",src,0,0);

   return SD_NET_SELECT_MODE_LIMITED;
}

/*=============================================================================
FUNCTION: convFromNetSelectMode

DESCRIPTION:
   Convert a value from sd_net_select_mode_e_type to AEECMSdNetSelectMode.
   Should be the same value, as the the enums matched value for value
   at the time of the writing of this function.
=============================================================================*/
static AEECMSdNetSelectMode convFromNetSelectMode(sd_net_select_mode_e_type src)
{
   uint32 i;

   for (i=0; i < ARR_SIZE(sdNetSelectModeConvTbl); i++)
   {
      if( sdNetSelectModeConvTbl[i].SD == src)
      {
         return (sdNetSelectModeConvTbl[i].AEECM);
      }
   }

   ERR("convFromNetSelectMode: %d not found",src,0,0);

   return AEECM_SD_NET_SELECT_MODE_LIMITED;
}

/*=============================================================================
FUNCTION: copyToPlmnSys

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyToPlmnSys(prli_plmn_sys_type *pDest,
                          const AEECMPrlPlmnSysType *pSrc)
{
   uint32 i;

   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   for (i=0;i<ARR_SIZE(pDest->plmn_id.identity);i++)
   {
      pDest->plmn_id.identity[i] = pSrc->plmn_id.identity[i];
   }

   pDest->lac = pSrc->lac;
   pDest->net_select_mode = convToNetSelectMode(pSrc->net_select_mode);

   pDest->num_lac = pSrc->num_lac;

   if ( ARR_SIZE(pDest->lac_a) != ARR_SIZE(pSrc->lac_a))
   {
      ERR("copyToPlmnSys: lac_a array mismatch",0,0,0);
      return;
   }

   for(i=0;i<ARR_SIZE(pDest->lac_a);i++ )
   {
      pDest->lac_a[i] = pSrc->lac_a[i];
   }
}

/*=============================================================================
FUNCTION: copyFromPlmnSys

DESCRIPTION:
   Copy struct from pSrc to pDest, value for value using conversion 
   tables when necessary.
=============================================================================*/
static void copyFromPlmnSys(AEECMPrlPlmnSysType *pDest,
                            const prli_plmn_sys_type *pSrc)
{
   uint32 i;

   ERR_IF_NULL(pDest)
   ERR_IF_NULL(pSrc)

   if (ARR_SIZE(pDest->plmn_id.identity) != ARR_SIZE(pDest->plmn_id.identity))
   {
      ERR("copyFromPlmnSys: identity array mismatch",0,0,0);
      return;
   }

   for (i=0;i<ARR_SIZE(pDest->plmn_id.identity);i++)
   {
      pDest->plmn_id.identity[i] = pSrc->plmn_id.identity[i];
   }

   pDest->lac = pSrc->lac;
   pDest->net_select_mode = convFromNetSelectMode(pSrc->net_select_mode);
   pDest->num_lac = pSrc->num_lac;

   if ( ARR_SIZE(pDest->lac_a) != ARR_SIZE(pSrc->lac_a))
   {
      ERR("copyFromPlmnSys: lac_a array mismatch",0,0,0);
      return;
   }

   for(i=0;i<ARR_SIZE(pDest->lac_a);i++ )
   {
      pDest->lac_a[i] = pSrc->lac_a[i];
   }
}

/*=============================================================================
FUNCTION: OEMCMHelper_CopyToPrlAcqRec

DESCRIPTION:
   Copies the AEECMPrlAcqRecord struct to its prl.h equivalent, 
   prli_acq_rec_type.  

PARAMETERS:
   pDest                [out]: The structure from prl.h to copy to.
   pSrc                 [in]:  The AEECM structure to copy from.

=============================================================================*/
void OEMCMHelper_CopyToPrlAcqRec(prli_acq_rec_type *pDest,
                                 const AEECMPrlAcqRecord *pSrc)
{
   pDest->acq_type = convToPrlAcqType(pSrc->acq_type);

   pDest->is_valid = pSrc->is_valid;

   switch (pSrc->acq_type)
   {
   case AEECM_PRL_ACQ_REC_RESERVED:
      break;

   case AEECM_PRL_ACQ_REC_CELLULAR_ANALOG:
      pDest->type.cellular_analog.a_b = convToPrlAB(pSrc->type.cellular_analog.a_b);
      break;

   case AEECM_PRL_ACQ_REC_CELLULAR_CDMA_STD:
      copyToCellularCdma(&pDest->type.cellular_cdma ,&pSrc->type.cellular_cdma);
      break;

   case AEECM_PRL_ACQ_REC_CELLULAR_CDMA_CUST:
      copyToCellularCdmaCust(&pDest->type.cellular_cdma_cust, &pSrc->type.cellular_cdma_cust);
      break;

   case AEECM_PRL_ACQ_REC_CELLULAR_GENERIC:
      copyToCellularGen(&pDest->type.cellular_generic, &pSrc->type.cellular_generic);
      break;

   case AEECM_PRL_ACQ_REC_PCS_CDMA_BLOCKS:
      copyToPcsCdmaBlocks(&pDest->type.pcs_cdma_block, &pSrc->type.pcs_cdma_block);
      break;

   case AEECM_PRL_ACQ_REC_PCS_CDMA_CHANNELS:
      copyToPcsCdmaChannels(&pDest->type.pcs_cdma_chan, &pSrc->type.pcs_cdma_chan);
      break;

   case AEECM_PRL_ACQ_REC_JTACS_CDMA_STD:
      copyToJtacsCdmaStd(&pDest->type.jtacs_cdma_std, &pSrc->type.jtacs_cdma_std);
      break;

   case AEECM_PRL_ACQ_REC_JTACS_CDMA_CUST:
      copyToJtacsCdmaCust(&pDest->type.jtacs_cdma_cust, &pSrc->type.jtacs_cdma_cust);
      break;

   case AEECM_PRL_ACQ_REC_CDMA_2GHZ_CHANS:
      copyToCdma2GhzChans(&pDest->type.cdma_2ghz_chans, &pSrc->type.cdma_2ghz_chans);
      break;

   case AEECM_PRL_ACQ_REC_CDMA_GENERIC:
      copyToCdmaGen(&pDest->type.cdma_generic, &pSrc->type.cdma_generic);
      break;

   case AEECM_PRL_ACQ_REC_HDR_GENERIC:
      copyToHdrGen(&pDest->type.hdr_generic, &pSrc->type.hdr_generic);
      break;

   case AEECM_PRL_ACQ_REC_GSM_GENERIC:
      copyToGsmGen(&pDest->type.gsm_generic, &pSrc->type.gsm_generic);
      break;

   case AEECM_PRL_ACQ_REC_WCDMA_GENERIC:
      copyToWcdmaGen(&pDest->type.wcdma_generic, &pSrc->type.wcdma_generic);
      break;

   case AEECM_PRL_ACQ_REC_GW_PREFERRED:
      copyToGwPreferred(&pDest->type.gw_pref, &pSrc->type.gw_pref);
      break;

   default:
      ERR("copyToPrlAcqRec: acq_type not found %d",pDest->acq_type,0,0);
      break;
   }

   copyToPlmnSys(&pDest->plmn, &pSrc->plmn);
}

/*=============================================================================
FUNCTION: OEMCMHelper_CopyFromPrlAcqRec

DESCRIPTION:
   Copies to the AEECMPrlAcqRecord struct from its prl.h equivalent, 
   prli_acq_rec_type.  

PARAMETERS:
   pDest                [out]: The AEECM structure to copy to.
   pSrc                 [in]:  The prl.h structure to copy from.

=============================================================================*/
void OEMCMHelper_CopyFromPrlAcqRec(AEECMPrlAcqRecord *pDest,
                                   const prli_acq_rec_type *pSrc)
{
   pDest->acq_type = convFromPrlAcqType(pSrc->acq_type);

   pDest->is_valid = pSrc->is_valid;

   switch (pSrc->acq_type)
   {
   case PRL_ACQ_REC_RESERVED:
      break;

   case PRL_ACQ_REC_CELLULAR_ANALOG:
      pDest->type.cellular_analog.a_b = convFromPrlAB(pSrc->type.cellular_analog.a_b);
      break;

   case PRL_ACQ_REC_CELLULAR_CDMA_STD:
      copyFromCellularCdma(&pDest->type.cellular_cdma ,&pSrc->type.cellular_cdma);
      break;

   case PRL_ACQ_REC_CELLULAR_CDMA_CUST:
      copyFromCellularCdmaCust(&pDest->type.cellular_cdma_cust, &pSrc->type.cellular_cdma_cust);
      break;

   case PRL_ACQ_REC_CELLULAR_GENERIC:
      copyFromCellularGen(&pDest->type.cellular_generic, &pSrc->type.cellular_generic);
      break;

   case PRL_ACQ_REC_PCS_CDMA_BLOCKS:
      copyFromPcsCdmaBlocks(&pDest->type.pcs_cdma_block, &pSrc->type.pcs_cdma_block);
      break;

   case PRL_ACQ_REC_PCS_CDMA_CHANNELS:
      copyFromPcsCdmaChannels(&pDest->type.pcs_cdma_chan, &pSrc->type.pcs_cdma_chan);
      break;

   case PRL_ACQ_REC_JTACS_CDMA_STD:
      copyFromJtacsCdmaStd(&pDest->type.jtacs_cdma_std, &pSrc->type.jtacs_cdma_std);
      break;

   case PRL_ACQ_REC_JTACS_CDMA_CUST:
      copyFromJtacsCdmaCust(&pDest->type.jtacs_cdma_cust, &pSrc->type.jtacs_cdma_cust);
      break;

   case PRL_ACQ_REC_CDMA_2GHZ_CHANS:
      copyFromCdma2GhzChans(&pDest->type.cdma_2ghz_chans, &pSrc->type.cdma_2ghz_chans);
      break;

   case PRL_ACQ_REC_CDMA_GENERIC:
      copyFromCdmaGen(&pDest->type.cdma_generic, &pSrc->type.cdma_generic);
      break;

   case PRL_ACQ_REC_HDR_GENERIC:
      copyFromHdrGen(&pDest->type.hdr_generic, &pSrc->type.hdr_generic);
      break;

   case PRL_ACQ_REC_GSM_GENERIC:
      copyFromGsmGen(&pDest->type.gsm_generic, &pSrc->type.gsm_generic);
      break;

   case PRL_ACQ_REC_WCDMA_GENERIC:
      copyFromWcdmaGen(&pDest->type.wcdma_generic, &pSrc->type.wcdma_generic);
      break;

   case PRL_ACQ_REC_GW_PREFERRED:
      copyFromGwPreferred(&pDest->type.gw_pref, &pSrc->type.gw_pref);
      break;

   default:
      ERR("copyToPrlAcqRec: acq_type not found %d",pDest->acq_type,0,0);
      break;
   }

   copyFromPlmnSys(&pDest->plmn, &pSrc->plmn);

}

/*****************************************************************************
                              MBMS HELPERS
*****************************************************************************/

#ifdef FEATURE_MBMS
/*=============================================================================
FUNCTION: copyTmgiFromSys

DESCRIPTION:
   Copys from the sys_gw_mbms_tmgi_uint_s_type structure to the AEECM
   equivalent structure, AEECMMBMSTmgi.

   This is a file scope only utility function.

DEPENDENCIES:
  sys_gw_mbms_tmgi_uint_s_type in sys.h

=============================================================================*/
static void copyTmgiFromsys(AEECMMBMSTmgi *pAEECMTmgi,
                            const sys_gw_mbms_tmgi_uint_s_type *pSysTmgi)
{
   if (NULL == pAEECMTmgi)
   {
      ERR("copyTmgiFromsys: NULL param",0,0,0);
      return;
   }

   if (NULL == pSysTmgi)
   {
      ERR("copyTmgiFromsys: NULL param",0,0,0);
      return;
   }

   pAEECMTmgi->present = pSysTmgi->present;
   pAEECMTmgi->tmgi = pSysTmgi->tmgi;
}

/*=============================================================================
FUNCTION: copyTmgiToSys

DESCRIPTION:
   Copys from the AEECMMBMSTmgi structure to the sys
   equivalent structure, sys_gw_mbms_tmgi_uint_s_type.

   This is a file scope only utility function.

DEPENDENCIES:
  sys_gw_mbms_tmgi_uint_s_type in sys.h

=============================================================================*/
static void copyTmgiTosys(sys_gw_mbms_tmgi_uint_s_type *pSysTmgi,
                          const AEECMMBMSTmgi *pAEECMTmgi)
{
   if (NULL == pAEECMTmgi)
   {
      ERR("copyTmgiTosys: NULL param",0,0,0);
      return;
   }

   if (NULL == pSysTmgi)
   {
      ERR("copyTmgiTosys: NULL param",0,0,0);
      return;
   }

   pSysTmgi->present = pAEECMTmgi->present;
   pSysTmgi->tmgi = pAEECMTmgi->tmgi;
   
}

/*=============================================================================
FUNCTION: copyIpAddrFromSys

DESCRIPTION:
   Copys from the sys_ip_address_s_type structure to the AEECM
   equivalent structure, AEECMIpAddress.

   This is a file scope only utility function.

DEPENDENCIES:
  sys_ip_address_s_type in sys.h

=============================================================================*/
static void copyIpAddrFromSys(AEECMIpAddress *pAEECMIpAddr,
                              const sys_ip_address_s_type *pSysIpAddr)
{

   if (NULL == pAEECMIpAddr)
   {
      ERR("copyIpAddrFromSys: NULL param",0,0,0);
      return;
   }

   if (NULL == pSysIpAddr)
   {
      ERR("copyIpAddrFromSys: NULL param",0,0,0);
      return;
   }

   pAEECMIpAddr->type = (AEECMIpAddrType)pSysIpAddr->type;
   pAEECMIpAddr->addr.v4 = pSysIpAddr->addr.v4;

   if (AEECM_IP_ADDR_V6_LENGTH == ARR_SIZE(pSysIpAddr->addr.v6))
   {
      pAEECMIpAddr->addr.v6[0] = pSysIpAddr->addr.v6[0];
      pAEECMIpAddr->addr.v6[1] = pSysIpAddr->addr.v6[1];
   }
   else
   {
      ERR("copyIpAddrFromSys: sys ip v6 size invalid",0,0,0);
   }

}

/*=============================================================================
FUNCTION: copyIpAddrToSys

DESCRIPTION:
   Copys from the AEECMIpAddress structure to the sys
   equivalent structure, sys_ip_address_s_type.

   This is a file scope only utility function.

DEPENDENCIES:
  sys_ip_address_s_type in sys.h

=============================================================================*/
static void copyIpAddrToSys(sys_ip_address_s_type *pSysIpAddr,
                            const AEECMIpAddress *pAEECMIpAddr)
{

   if (NULL == pSysIpAddr)
   {
      ERR("copyIpAddrFromSys: NULL param",0,0,0);
      return;
   }

   if (NULL == pAEECMIpAddr)
   {
      ERR("copyIpAddrFromSys: NULL param",0,0,0);
      return;
   }

   pSysIpAddr->type = (sys_ip_addr_enum_type)pAEECMIpAddr->type;
   pSysIpAddr->addr.v4 = pAEECMIpAddr->addr.v4;

   if (AEECM_IP_ADDR_V6_LENGTH == ARR_SIZE(pSysIpAddr->addr.v6))
   {
      pSysIpAddr->addr.v6[0] = pAEECMIpAddr->addr.v6[0];
      pSysIpAddr->addr.v6[1] = pAEECMIpAddr->addr.v6[1];
   }
   else
   {
      ERR("copyIpAddrToSys: sys ip v6 size invalid",0,0,0);
   }

}


/* MBMS rpt copy functions */

/*=============================================================================
FUNCTION: copyMBMSActivateCnf

DESCRIPTION:
   Copys from the sys_gw_mbms_resp_abort_cnf_s_type structure to the AEECM
   equivalent structure, AEECMMBMSRptActivateCnf.

   This is a file scope only utility function.

DEPENDENCIES:
  sys_gw_mbms_resp_activate_cnf_s_type in sys_gw.h

=============================================================================*/
static void copyMBMSActivateCnf(AEECMMBMSRptActivateCnf *pAEECMActCnf,
                                const sys_gw_mbms_resp_activate_cnf_s_type *pSysActCnf)
{
   uint32 i;

   if (NULL == pAEECMActCnf)
   {
      ERR("copyMBMSActivateCnf: NULL param",0,0,0);
      return;
   }

   if (NULL == pSysActCnf)
   {
      ERR("copyMBMSActivateCnf: NULL param",0,0,0);
      return;
   }
   
   if (pSysActCnf->num_services > AEECM_MBMS_NUM_MAX_SRVS_PER_REQ)
   {
      MSG_ERROR("copyMBMSActivateCnf: num_services out of range %d",pSysActCnf->num_services,0,0);
      pAEECMActCnf->num_services = AEECM_MBMS_NUM_MAX_SRVS_PER_REQ;
   }
   else
   {
      pAEECMActCnf->num_services = pSysActCnf->num_services;
   }

   for(i=0;i<pAEECMActCnf->num_services;i++)
   {
      copyTmgiFromsys(&pAEECMActCnf->services[i].tmgi, &pSysActCnf->services[i].tmgi);
      copyIpAddrFromSys(&pAEECMActCnf->services[i].mip,&pSysActCnf->services[i].mip);
      pAEECMActCnf->services[i].port = pSysActCnf->services[i].port;
      pAEECMActCnf->services[i].cnf_status = pSysActCnf->services[i].cnf_status;
      pAEECMActCnf->services[i].srv_status = (AEECMMBMSSrvStatus)pSysActCnf->services[i].srv_status;
      pAEECMActCnf->services[i].log_chan_id.present = pSysActCnf->services[i].log_chan_id.present;
      pAEECMActCnf->services[i].log_chan_id.log_chan_id = pSysActCnf->services[i].log_chan_id.log_chan_id;
      pAEECMActCnf->services[i].mbms_parm = pSysActCnf->services[i].mbms_parm;
   }

}

/*=============================================================================
FUNCTION: copyMBMSDeactivateCnf

DESCRIPTION:
   Copys from the sys_gw_mbms_resp_deactivate_cnf_s_type structure to the AEECM
   equivalent structure, AEECMMBMSRptDeactivateCnf.

   This is a file scope only utility function.

DEPENDENCIES:
  sys_gw_mbms_resp_deactivate_cnf_s_type in sys.h

=============================================================================*/
static void copyMBMSDeactivateCnf(AEECMMBMSRptDeactivateCnf *pAEECMDeactCnf,
                                  const sys_gw_mbms_resp_deactivate_cnf_s_type *pSysDeactCnf)
{
   uint32 i;

   if (NULL == pAEECMDeactCnf)
   {
      ERR("copyMBMSDeactivateCnf: NULL PARAM",0,0,0);
      return;
   }

   if (NULL == pSysDeactCnf)
   {
      ERR("copyMBMSDeactivateCnf: NULL PARAM",0,0,0);
      return;
   }

   if (pSysDeactCnf->num_services > AEECM_MBMS_NUM_MAX_SRVS_PER_REQ)
   {
      MSG_ERROR("copyMBMSDeactivateCnf: num_services out of range %d",pSysDeactCnf->num_services,0,0);
      pAEECMDeactCnf->num_services = AEECM_MBMS_NUM_MAX_SRVS_PER_REQ;
   }
   else
   {
      pAEECMDeactCnf->num_services = pSysDeactCnf->num_services;
   }

   for(i=0;i<pAEECMDeactCnf->num_services;i++)
   {
      copyTmgiFromsys(&pAEECMDeactCnf->services[i].tmgi,&pSysDeactCnf->services[i].tmgi);
      copyIpAddrFromSys(&pAEECMDeactCnf->services[i].mip,&pSysDeactCnf->services[i].mip);
      pAEECMDeactCnf->services[i].port = pSysDeactCnf->services[i].port;
      pAEECMDeactCnf->services[i].cnf_status = pSysDeactCnf->services[i].cnf_status;
      pAEECMDeactCnf->services[i].srv_status = (AEECMMBMSSrvStatus)pSysDeactCnf->services[i].srv_status;
      pAEECMDeactCnf->services[i].log_chan_id.present = pSysDeactCnf->services[i].log_chan_id.present;
      pAEECMDeactCnf->services[i].log_chan_id.log_chan_id = pSysDeactCnf->services[i].log_chan_id.log_chan_id;
      pAEECMDeactCnf->services[i].mbms_parm = pSysDeactCnf->services[i].mbms_parm;
   }
   
}

/*=============================================================================
FUNCTION: copyMBMSServiceInd

DESCRIPTION:
   Copys from the sys_gw_mbms_resp_service_ind_s_type structure to the AEECM
   equivalent structure, AEECMMBMSRptServiceInd.

   This is a file scope only utility function.

DEPENDENCIES:
  sys_gw_mbms_resp_service_ind_s_type in sys.h

=============================================================================*/
static void copyMBMSServiceInd(AEECMMBMSRptServiceInd *pAEECMSrvInd,
                               const sys_gw_mbms_resp_service_ind_s_type *pSysSrvInd)
{
   uint32 i;

   if (NULL == pAEECMSrvInd)
   {
      ERR("copyMBMSServiceInd: NULL PARAM",0,0,0);
      return;
   }

   if (NULL == pSysSrvInd)
   {
      ERR("copyMBMSServiceInd: NULL PARAM",0,0,0);
      return;
   }
   
   if (pSysSrvInd->num_services > AEECM_MBMS_NUM_MAX_SRVS_PER_REQ)
   {
      MSG_ERROR("copyMBMSServiceInd: num_services out of range %d",pSysSrvInd->num_services,0,0);
      pAEECMSrvInd->num_services = AEECM_MBMS_NUM_MAX_SRVS_PER_REQ;
   }
   else
   {
      pAEECMSrvInd->num_services = pSysSrvInd->num_services;
   }

   for(i=0;i<pAEECMSrvInd->num_services;i++)
   {
      copyTmgiFromsys(&pAEECMSrvInd->services[i].tmgi, &pSysSrvInd->services[i].tmgi);
      copyIpAddrFromSys(&pAEECMSrvInd->services[i].mip,&pSysSrvInd->services[i].mip);
      pAEECMSrvInd->services[i].port = pSysSrvInd->services[i].port;
      pAEECMSrvInd->services[i].log_chan_id.present = pSysSrvInd->services[i].log_chan_id.present;
      pAEECMSrvInd->services[i].log_chan_id.log_chan_id = pSysSrvInd->services[i].log_chan_id.log_chan_id;
      pAEECMSrvInd->services[i].srv_status = (AEECMMBMSSrvStatus)pSysSrvInd->services[i].srv_status;
      pAEECMSrvInd->services[i].mbms_parm = pSysSrvInd->services[i].mbms_parm;
   }

}

/*=============================================================================
FUNCTION: copyMBMSCapabilityInd

DESCRIPTION:
   Copys from the sys_gw_mbms_resp_capability_ind_s_type structure to the AEECM
   equivalent structure, AEECMMBMSRptCapabilityInd.

   This is a file scope only utility function.

DEPENDENCIES:
  sys_gw_mbms_resp_capability_ind_s_type in sys.h

=============================================================================*/
static void copyMBMSCapabilityInd(AEECMMBMSRptCapabilityInd *pAEECMCapInd,
                                  const sys_gw_mbms_resp_capability_ind_s_type *pSysCapInd)
{

   if (NULL == pAEECMCapInd)
   {
      ERR("copyMBMSCapabilityInd: NULL PARAM",0,0,0);
      return;
   }

   if (NULL == pSysCapInd)
   {
      ERR("copyMBMSCapabilityInd: NULL PARAM",0,0,0);
      return;
   }

   pAEECMCapInd->network_capability = pSysCapInd->network_capability;
   pAEECMCapInd->cell_capability = pSysCapInd->cell_capability;
   pAEECMCapInd->mbms_parm = pSysCapInd->mbms_parm;
}

/*=============================================================================
FUNCTION: copyMBMSContextActivatedInd

DESCRIPTION:
   Copys from the sys_gw_mbms_resp_context_activated_ind_s_type structure to the AEECM
   equivalent structure, AEECMMBMSRptContextActivatedInd.

   This is a file scope only utility function.

DEPENDENCIES:
  sys_gw_mbms_resp_context_activated_ind_s_type in sys_gw.h

=============================================================================*/
static void copyMBMSContextActivatedInd(AEECMMBMSRptContextActivatedInd *pAEECMContextActivatedInd,
                                        const sys_gw_mbms_resp_context_activated_ind_s_type *pSysContextActivatedInd)
{
   if (NULL == pAEECMContextActivatedInd)
   {
      ERR("copyMBMSContextActivatedInd: NULL PARAM",0,0,0);
      return;
   }

   if (NULL == pSysContextActivatedInd)
   {
      ERR("copyMBMSContextActivatedInd: NULL PARAM",0,0,0);
      return;
   }

   copyTmgiFromsys(&pAEECMContextActivatedInd->tmgi, &pSysContextActivatedInd->tmgi);
   copyIpAddrFromSys(&pAEECMContextActivatedInd->mip,&pSysContextActivatedInd->mip);
   pAEECMContextActivatedInd->port = pSysContextActivatedInd->port;
   pAEECMContextActivatedInd->act_status = pSysContextActivatedInd->act_status;
   pAEECMContextActivatedInd->context_failure_reason = (AEECMMBMSContextFailureReason)pSysContextActivatedInd->context_failure_reason;
      
}

/*=============================================================================
FUNCTION: copyMBMSContextDeactivatedInd

DESCRIPTION:
   Copys from the sys_gw_mbms_resp_context_deactivated_ind_s_type structure to the AEECM
   equivalent structure, AEECMMBMSRptContextDeactivatedInd.

   This is a file scope only utility function.

DEPENDENCIES:
  sys_gw_mbms_resp_context_deactivated_ind_s_type in sys_gw.h

=============================================================================*/
static void copyMBMSContextDeactivatedInd(AEECMMBMSRptContextDeactivatedInd *pAEECMContextDeactInd,
                                          const sys_gw_mbms_resp_context_deactivated_ind_s_type *pSysDeactInd)
{

   if (NULL == pAEECMContextDeactInd)
   {
      ERR("copyMBMSContextDeactivatedInd: NULL PARAM",0,0,0);
      return;
   }

   if (NULL == pSysDeactInd)
   {
      ERR("copyMBMSContextDeactivatedInd: NULL PARAM",0,0,0);
      return;
   }

   copyTmgiFromsys(&pAEECMContextDeactInd->tmgi, &pSysDeactInd->tmgi);
   copyIpAddrFromSys(&pAEECMContextDeactInd->mip,&pSysDeactInd->mip);
   pAEECMContextDeactInd->port = pSysDeactInd->port;
   pAEECMContextDeactInd->deact_status = pSysDeactInd->deact_status;
   pAEECMContextDeactInd->context_failure_reason = (AEECMMBMSContextFailureReason)pSysDeactInd->context_failure_reason;
}

/*=============================================================================
FUNCTION: copyMBMSAbortCnf

DESCRIPTION:
   Copys from the sys_gw_mbms_resp_abort_cnf_s_type structure to the AEECM
   equivalent structure, AEECMMBMSRptAbortCnf.

   This is a file scope only utility function.

DEPENDENCIES:
  sys_gw_mbms_resp_abort_cnf_s_type in sys_gw.h

=============================================================================*/
static void copyMBMSAbortCnf(AEECMMBMSRptAbortCnf *pAEECMAbortCnf,
                             const sys_gw_mbms_resp_abort_cnf_s_type *pSysAbortCnf)
{
   uint32 i;

   if (NULL == pAEECMAbortCnf)
   {
      ERR("copyMBMSAbortCnf: NULL PARAM",0,0,0);
      return;
   }

   if (NULL == pSysAbortCnf)
   {
      ERR("copyMBMSAbortCnf: NULL PARAM",0,0,0);
      return;
   }

   if (pSysAbortCnf->num_services > AEECM_MBMS_NUM_MAX_SRVS_PER_REQ)
   {
      MSG_ERROR("copyMBMSAbortCnf: num_services out of range %d",pSysAbortCnf->num_services,0,0);
      pAEECMAbortCnf->num_services = AEECM_MBMS_NUM_MAX_SRVS_PER_REQ;
   }
   else
   {
      pAEECMAbortCnf->num_services = pSysAbortCnf->num_services;
   }

   for(i=0;i<pAEECMAbortCnf->num_services;i++)
   {
      copyTmgiFromsys(&pAEECMAbortCnf->services[i].tmgi, &pSysAbortCnf->services[i].tmgi_uint);
      copyIpAddrFromSys(&pAEECMAbortCnf->services[i].mip,&pSysAbortCnf->services[i].mip);
      pAEECMAbortCnf->services[i].port = pSysAbortCnf->services[i].port;
      pAEECMAbortCnf->services[i].cnf_status = pSysAbortCnf->services[i].cnf_status;
   }
}

/*=============================================================================
FUNCTION: copyMBMSSuspendCnf

DESCRIPTION:
   Copys from the sys_gw_mbms_resp_suspend_cnf_s_type structure to the AEECM
   equivalent structure, AEECMMBMSRptSuspendCnf.

   This is a file scope only utility function.

DEPENDENCIES:
  sys_gw_mbms_resp_suspend_cnf_s_type in sys_gw.h

=============================================================================*/
static void copyMBMSSuspendCnf(AEECMMBMSRptSuspendCnf *pAEECMSuspendCnf,
                               const sys_gw_mbms_resp_suspend_cnf_s_type *pSysSuspendCnf)
{
   uint32 i;

   if (NULL == pAEECMSuspendCnf)
   {
      ERR("copyMBMSSuspendCnf: NULL param",0,0,0);
      return;
   }

   if (NULL == pSysSuspendCnf)
   {
      ERR("copyMBMSSuspendCnf: NULL param",0,0,0);
      return;
   }
  
   if (pSysSuspendCnf->num_services > AEECM_MBMS_NUM_MAX_SRVS_PER_REQ)
   {
      MSG_ERROR("copyMBMSSuspendCnf: num_services out of range %d",pSysSuspendCnf->num_services,0,0);
      pAEECMSuspendCnf->num_services = AEECM_MBMS_NUM_MAX_SRVS_PER_REQ;
   }
   else
   {
      pAEECMSuspendCnf->num_services = pSysSuspendCnf->num_services;
   }

   for (i=0;i<pAEECMSuspendCnf->num_services;i++)
   {
      copyTmgiFromsys(&pAEECMSuspendCnf->services[i].tmgi, &pSysSuspendCnf->services[i].tmgi);
      pAEECMSuspendCnf->services[i].cnf_status = pSysSuspendCnf->services[i].cnf_status;
      pAEECMSuspendCnf->services[i].mbms_parm = pSysSuspendCnf->services[i].mbms_parm;
   }
}

/*=============================================================================
FUNCTION: copyMBMSResumeCnf

DESCRIPTION:
   Copys from the sys_gw_mbms_resp_resume_cnf_s_type structure to the AEECM
   equivalent structure, AEECMMBMSRptResumeCnf.

   This is a file scope only utility function.

DEPENDENCIES:
  sys_gw_mbms_resp_resume_cnf_s_type in sys_gw.h

=============================================================================*/
static void copyMBMSResumeCnf(AEECMMBMSRptResumeCnf *pAEECMResumeCnf,
                               const sys_gw_mbms_resp_resume_cnf_s_type *pSysResumeCnf)
{
   uint32 i;

   if (NULL == pAEECMResumeCnf)
   {
      ERR("copyMBMSResumeCnf: NULL param",0,0,0);
      return;
   }

   if (NULL == pSysResumeCnf)
   {
      ERR("copyMBMSResumeCnf: NULL param",0,0,0);
      return;
   }

   if (pSysResumeCnf->num_services > AEECM_MBMS_NUM_MAX_SRVS_PER_REQ)
   {
      MSG_ERROR("copyMBMSResumeCnf: num_services out of range %d",pSysResumeCnf->num_services,0,0);
      pAEECMResumeCnf->num_services = AEECM_MBMS_NUM_MAX_SRVS_PER_REQ;
   }
   else
   {
      pAEECMResumeCnf->num_services = pSysResumeCnf->num_services;
   }

   for (i=0;i<pAEECMResumeCnf->num_services;i++)
   {
      copyTmgiFromsys(&pAEECMResumeCnf->services[i].tmgi, &pSysResumeCnf->services[i].tmgi);
      pAEECMResumeCnf->services[i].cnf_status = pSysResumeCnf->services[i].cnf_status;
      pAEECMResumeCnf->services[i].srv_status = (AEECMMBMSSrvStatus)pSysResumeCnf->services[i].srv_status;
      pAEECMResumeCnf->services[i].mbms_parm = pSysResumeCnf->services[i].mbms_parm;
   }
}

/*=============================================================================
FUNCTION: copyMBMSAvailServicesListCnf

DESCRIPTION:
   Copys from the sys_gw_mbms_resp_avail_srv_list_cnf_s_type structure to the AEECM
   equivalent structure, AEECMMBMSRptAvailSrvListCnf.

   This is a file scope only utility function.

DEPENDENCIES:
  sys_gw_mbms_resp_avail_srv_list_cnf_s_type in sys.h

=============================================================================*/
static void copyMBMSAvailServicesListCnf(AEECMMBMSRptAvailSrvListCnf *pAEECMAvailSrvList,
                                         const sys_gw_mbms_resp_avail_srv_list_cnf_s_type *pSysAvaliSrvList)
{
   uint32 i;

   if (NULL == pAEECMAvailSrvList)
   {
      ERR("copyMBMSAvailServicesListCnf: NULL param",0,0,0);
      return;
   }

   if (NULL == pSysAvaliSrvList)
   {
      ERR("copyMBMSAvailServicesListCnf: NULL param",0,0,0);
      return;
   }
   
   pAEECMAvailSrvList->cnf_status = pSysAvaliSrvList->cnf_status;
   pAEECMAvailSrvList->mbms_parm = pSysAvaliSrvList->mbms_parm;

   for(i=0;i<AEECM_MBMS_MAX_SERVICES;i++)
   {
      pAEECMAvailSrvList->service_id[i] = pSysAvaliSrvList->service_id[i];
   }
}

/*=============================================================================
FUNCTION: copyMBMSLLReadyInd

DESCRIPTION:
   Copys from the sys_gw_mbms_resp_ll_ready_ind_s_type structure to the AEECM
   equivalent structure, AEECMMBMSRptLLReadyInd.

   This is a file scope only utility function.

DEPENDENCIES:
  sys_gw_mbms_resp_ll_ready_ind_s_type in sys_gw.h

=============================================================================*/
static void copyMBMSLLReadyInd(AEECMMBMSRptLLReadyInd *pAEECMLLReadyInd,
                               const sys_gw_mbms_resp_ll_ready_ind_s_type *pSysLLReadyInd)
{
   if (NULL == pAEECMLLReadyInd)
   {
      ERR("copyMBMSLLReadyInd: NULL Param",0,0,0);
      return;
   }

   if (NULL == pSysLLReadyInd)
   {
      ERR("copyMBMSLLReadyInd: NULL Param",0,0,0);
      return;
   }

   pAEECMLLReadyInd->mbms_parm1 = pSysLLReadyInd->mbms_parm1;

}


/*=============================================================================
FUNCTION: copyMBMSSetAttrRpt

DESCRIPTION:
   Copys from the sys_gw_mbms_resp_set_attr_s_type structure to the AEECM
   equivalent structure, AEECMMBMSRptSetAttr.

   This is a file scope only utility function.

DEPENDENCIES:
  sys_gw_mbms_resp_set_attr_s_type in sys_gw.h

=============================================================================*/
static void copyMBMSSetAttrRpt(AEECMMBMSRptSetAttr *pAEECMSetAttr,
                               const sys_gw_mbms_resp_set_attr_s_type *pSysSetAttr)
{
   pAEECMSetAttr->mbms_parm1 = pSysSetAttr->mbms_parm1;
   pAEECMSetAttr->mbms_parm2 = pSysSetAttr->mbms_parm2;
   pAEECMSetAttr->mbms_parm3 = pSysSetAttr->mbms_parm3;
   pAEECMSetAttr->mbms_parm4 = pSysSetAttr->mbms_parm4;
   pAEECMSetAttr->mbms_parm5 = pSysSetAttr->mbms_parm5;
   pAEECMSetAttr->mbms_parm6 = pSysSetAttr->mbms_parm6;
}


/* MBMS cmd copy functions */


/*=============================================================================
FUNCTION: copyFromMBMSActivateReq

DESCRIPTION:
   Copies data in a sys_gw_mbms_activate_req_s_type struct to the AEECM equivalent,
   AEECMMBMSCmdActivateReqInfo.
  
PARAMETERS:
   pAEECMActivateReq  [out]:  Struct to copy TO.
   pSysActivateReq    [in]:   Struct to copy FROM.
  
=============================================================================*/
static void copyFromMBMSActivateReq(AEECMMBMSCmdActivateReqInfo *pAEECMActivateReq,
                                const sys_gw_mbms_activate_req_s_type *pSysActivateReq)
{
   uint32 i;

   if (NULL == pSysActivateReq)
   {
      ERR("copyFromMBMSActivateReq: NULL param",0,0,0);
      return;
   }

   if (NULL == pAEECMActivateReq)
   {
      ERR("copyFromMBMSActivateReq: NULL param",0,0,0);
      return;
   }
   
   if (pSysActivateReq->num_services > AEECM_MBMS_NUM_MAX_SRVS_PER_REQ)
   {
      MSG_ERROR("copyFromMBMSActivateReq: num_services out of range %d",pSysActivateReq->num_services,0,0);
      pAEECMActivateReq->num_services = AEECM_MBMS_NUM_MAX_SRVS_PER_REQ;
   }
   else
   {
      pAEECMActivateReq->num_services = pSysActivateReq->num_services;
   }

   for (i=0;i<pAEECMActivateReq->num_services;i++)
   {
      copyTmgiFromsys(&pAEECMActivateReq->services[i].tmgi, &pSysActivateReq->services[i].tmgi);
      copyIpAddrFromSys(&pAEECMActivateReq->services[i].mip, &pSysActivateReq->services[i].mip);
      pAEECMActivateReq->services[i].port = pSysActivateReq->services[i].port;
      pAEECMActivateReq->services[i].priority.present = pSysActivateReq->services[i].priority.present;
      pAEECMActivateReq->services[i].priority.priority = pSysActivateReq->services[i].priority.priority;
      pAEECMActivateReq->services[i].srv_mode = (AEECMMBMSSrvMode)pSysActivateReq->services[i].srv_mode;
      pAEECMActivateReq->services[i].srv_type = (AEECMMBMSSrvType)pSysActivateReq->services[i].srv_type;
      pAEECMActivateReq->services[i].mbms_parm = pSysActivateReq->services[i].mbms_parm;
   }
      
}

/*=============================================================================
FUNCTION: copyFromMBMSDeactivateReq

DESCRIPTION:
   Copies data in a AEECMMBMSCmdDeactivateReqInfo struct to the CM equivalent,
   sys_gw_mbms_deactivate_req_s_type.
  
PARAMETERS:
  pSysDeactivateReq    [out]: Struct that is passed to CM through cm_mbms_cmd_generic.
  pAEECMDeactivateReq  [in]:  Struct that is filled by the APP.

=============================================================================*/
static void copyFromMBMSDeactivateReq(AEECMMBMSCmdDeactivateReqInfo *pAEECMDeactivateReq,
                                      const sys_gw_mbms_deactivate_req_s_type *pSysDeactivateReq)
{
   uint32 i;

   if (NULL == pAEECMDeactivateReq)
   {
      ERR("copyFromMBMSDeactivateReq: NULL param",0,0,0);
      return;
   }

   if (NULL == pSysDeactivateReq)
   {
      ERR("copyFromMBMSDeactivateReq: NULL param",0,0,0);
      return;
   }
      
   if (pSysDeactivateReq->num_services > AEECM_MBMS_NUM_MAX_SRVS_PER_REQ)
   {
      MSG_ERROR("copyFromMBMSDeactivateReq: num_services out of range %d",pSysDeactivateReq->num_services,0,0);
      pAEECMDeactivateReq->num_services = AEECM_MBMS_NUM_MAX_SRVS_PER_REQ;
   }
   else
   {
      pAEECMDeactivateReq->num_services = pSysDeactivateReq->num_services;
   }

   for (i=0;i<pAEECMDeactivateReq->num_services;i++)
   {
      copyTmgiFromsys(&pAEECMDeactivateReq->services[i].tmgi,&pSysDeactivateReq->services[i].tmgi);
      pAEECMDeactivateReq->services[i].mbms_parm = pSysDeactivateReq->services[i].mbms_parm;
   }

}

/*=============================================================================
FUNCTION: copyFromMBMSSessionCompleteInd

DESCRIPTION:
   Copies data in a sys_gw_mbms_session_complete_ind_s_type struct to the AEECM equivalent,
   AEECMMBMSCmdSessionCompleteIndInfo.
  
PARAMETERS:
  pSysSessionCompleteInd    [out]: Struct that is passed to CM through cm_mbms_cmd_generic.
  pAEECMSessionCompleteInd  [in]:  Struct that is filled by the APP.

=============================================================================*/
static void copyFromMBMSSessionCompleteInd(AEECMMBMSCmdSessionCompleteIndInfo *pAEECMSessionCompleteInd,
                                           const sys_gw_mbms_session_complete_ind_s_type *pSysSessionCompleteInd)
{
   if (NULL == pAEECMSessionCompleteInd)
   {
      ERR("copyFromMBMSSessionCompleteInd: NULL param",0,0,0);
      return;
   }

   if (NULL == pSysSessionCompleteInd)
   {
      ERR("copyFromMBMSSessionCompleteInd: NULL param",0,0,0);
      return;
   }

   copyTmgiFromsys(&pAEECMSessionCompleteInd->tmgi, &pSysSessionCompleteInd->tmgi);
   pAEECMSessionCompleteInd->session_id.present = pSysSessionCompleteInd->session_id.present;
   pAEECMSessionCompleteInd->session_id.session_id = pSysSessionCompleteInd->session_id.session_id;
   pAEECMSessionCompleteInd->mbms_parm = pSysSessionCompleteInd->mbms_parm;
}

/*=============================================================================
FUNCTION: copyFromMBMSAbortReq

DESCRIPTION:
   Copies data in a sys_gw_mbms_abort_req_s_type struct to the AEECM equivalent,
   AEECMMBMSCmdAbortReqInfo.
  
PARAMETERS:
   pAEECMAbortReq   [out]: Struct to copy to.
   pSysAbortReq     [in]:  Struct to copy from.

=============================================================================*/
static void copyFromMBMSAbortReq(AEECMMBMSCmdAbortReqInfo *pAEECMAbortReq,
                                 const sys_gw_mbms_abort_req_s_type *pSysAbortReq)
{
   uint32 i;

   if (NULL == pAEECMAbortReq)
   {
      ERR("copyFromMBMSAbortReq: NULL param",0,0,0);
      return;
   }

   if (NULL == pSysAbortReq)
   {
      ERR("copyFromMBMSAbortReq: NULL param",0,0,0);
      return;
   }
    
   if (pSysAbortReq->num_services > AEECM_MBMS_NUM_MAX_SRVS_PER_REQ)
   {
      MSG_ERROR("copyFromMBMSAbortReq: num_services out of range %d",pSysAbortReq->num_services,0,0);
      pAEECMAbortReq->num_services = AEECM_MBMS_NUM_MAX_SRVS_PER_REQ;
   }
   else
   {
      pAEECMAbortReq->num_services = pSysAbortReq->num_services;
   }

   for (i=0;i<pAEECMAbortReq->num_services;i++)
   {
      copyTmgiFromsys(&pAEECMAbortReq->services[i].tmgi, &pSysAbortReq->services[i].tmgi);
      copyIpAddrFromSys(&pAEECMAbortReq->services[i].mip, &pSysAbortReq->services[i].mip);
      pAEECMAbortReq->services[i].port = pSysAbortReq->services[i].port;
      pAEECMAbortReq->services[i].abort_type = (AEECMMBMSAbortType)pSysAbortReq->services[i].abort_type;
      pAEECMAbortReq->services[i].mbms_parm = pSysAbortReq->services[i].mbms_parm;
   }
   
}

/*=============================================================================
FUNCTION: copyFromMBMSSuspendReq

DESCRIPTION:
   Copies data in a sys_gw_mbms_suspend_req_s_type struct to the AEECM equivalent,
   AEECMMBMSCmdSuspendReqInfo.
  
PARAMETERS:
  pAEECMSuspendReq    [out]: Struct to copy to.
  pSysSuspendReq      [in]:  Struct to copy from.

=============================================================================*/
static void copyFromMBMSSuspendReq(AEECMMBMSCmdSuspendReqInfo *pAEECMSuspendReq,
                                   const sys_gw_mbms_suspend_req_s_type *pSysSuspendReq)
{
   uint32 i;

   if (NULL == pAEECMSuspendReq)
   {
      ERR("copyFromMBMSSuspendReq: NULL param",0,0,0);
      return;
   }

   if (NULL == pSysSuspendReq)
   {
      ERR("copyFromMBMSSuspendReq: NULL param",0,0,0);
      return;
   }
   
   if (pSysSuspendReq->num_services > AEECM_MBMS_NUM_MAX_SRVS_PER_REQ)
   {
      MSG_ERROR("copyFromMBMSSuspendReq: num_services out of range %d",pSysSuspendReq->num_services,0,0);
      pAEECMSuspendReq->num_services = AEECM_MBMS_NUM_MAX_SRVS_PER_REQ;
   }
   else
   {
      pAEECMSuspendReq->num_services = pSysSuspendReq->num_services;
   }

   for(i=0;i<pAEECMSuspendReq->num_services;i++)
   {
      copyTmgiFromsys(&pAEECMSuspendReq->services[i].tmgi, &pSysSuspendReq->services[i].tmgi);
      pAEECMSuspendReq->services[i].mbms_parm = pSysSuspendReq->services[i].mbms_parm;
   }
   
}

/*=============================================================================
FUNCTION: copyFromMBMSResumeReq

DESCRIPTION:
   Copies data in a sys_gw_mbms_resume_req_s_type struct to the AEECM equivalent,
   AEECMMBMSCmdResumeReqInfo.
  
PARAMETERS:
  pAEECMResumeReq    [out]: Struct to copy to.
  pSysResumeReq      [in]:  Struct to copy from.

=============================================================================*/
static void copyFromMBMSResumeReq(AEECMMBMSCmdResumeReqInfo *pAEECMResumeReq,
                                  const sys_gw_mbms_resume_req_s_type *pSysResumeReq)
{
   uint32 i;

   if (NULL == pAEECMResumeReq)
   {
      ERR("copyFromMBMSResumeReq: NULL param",0,0,0);
      return;
   }

   if (NULL == pSysResumeReq)
   {
      ERR("copyFromMBMSResumeReq: NULL param",0,0,0);
      return;
   }
   
   if (pSysResumeReq->num_services > AEECM_MBMS_NUM_MAX_SRVS_PER_REQ)
   {
      MSG_ERROR("copyFromMBMSResumeReq: num_services out of range %d",pSysResumeReq->num_services,0,0);
      pAEECMResumeReq->num_services = AEECM_MBMS_NUM_MAX_SRVS_PER_REQ;
   }
   else
   {
      pAEECMResumeReq->num_services = pSysResumeReq->num_services;
   }

   for(i=0;i<pAEECMResumeReq->num_services;i++)
   {
      copyTmgiFromsys(&pAEECMResumeReq->services[i].tmgi, &pSysResumeReq->services[i].tmgi);
      pAEECMResumeReq->services[i].mbms_parm = pSysResumeReq->services[i].mbms_parm;
   }

}

/*=============================================================================
FUNCTION: copyFromMBMSAvailServicesListReq

DESCRIPTION:
   Copies data in a sys_gw_mbms_avail_srv_list_req_s_type struct to the AEECM equivalent,
   AEECMMBMSCmdAvailSrvListReqInfo.
  
PARAMETERS:
   pAEECMAvailSrvListReq   [out]: Struct to copy to.
   pSysAvailSrvListReq     [in]:  Struct to copy from.

=============================================================================*/
static void copyFromMBMSAvailServicesListReq(AEECMMBMSCmdAvailSrvListReqInfo *pAEECMAvailSrvListReq,
                                             const sys_gw_mbms_avail_srv_list_req_s_type *pSysAvailSrvListReq)
{
   if (NULL == pAEECMAvailSrvListReq)
   {
      ERR("copyFromMBMSAvailServicesListReq: NULL param",0,0,0);
      return;
   }

   if (NULL == pSysAvailSrvListReq)
   {
      ERR("copyFromMBMSAvailServicesListReq: NULL param",0,0,0);
      return;
   }

   pAEECMAvailSrvListReq->mbms_parm = pSysAvailSrvListReq->mbms_parm;
   
}

/*=============================================================================
FUNCTION: copyFromMBMSGetCapability

DESCRIPTION:
   Copies data in a sys_gw_mbms_capability_req_s_type struct to the AEECM equivalent,
   AEECMMBMSCmdCapabilityReqInfo.
  
PARAMETERS:
  pAEECMMBMSCapabilityReq  [out]: Struct to copy to.
  pSysCapabilityReq        [in]:  Struct to copy from.

=============================================================================*/
static void copyFromMBMSGetCapability(AEECMMBMSCmdCapabilityReqInfo *pAEECMMBMSCapabilityReq,
                                      const sys_gw_mbms_capability_req_s_type *pSysCapabilityReq)
{
   if (NULL == pAEECMMBMSCapabilityReq)
   {
      ERR("copyFromMBMSGetCapability: NULL param",0,0,0);
      return;
   }

   if (NULL == pSysCapabilityReq)
   {
      ERR("copyFromMBMSGetCapability: NULL param",0,0,0);
      return;
   }

   pAEECMMBMSCapabilityReq->mbms_parm = pSysCapabilityReq->mbms_parm;

}

/*=============================================================================
FUNCTION: copyFromMBMSSetAttr

DESCRIPTION:
   Copies data in a sys_gw_mbms_set_attr_req_s_type struct to the AEECM equivalent,
   AEECMMBMSCmdSetAttrInfo.
  
PARAMETERS:
  pAEECMSetAttr   [out]: Struct to copy to.
  pSysSetAttr     [in]:  Struct to copy from.

=============================================================================*/
static void copyFromMBMSSetAttr(AEECMMBMSCmdSetAttrInfo *pAEECMSetAttr,
                                const sys_gw_mbms_set_attr_req_s_type *pSysSetAttr)
{
   if (NULL == pAEECMSetAttr)
   {
      ERR("copyFromMBMSSetAttr: NULL param",0,0,0);
      return;
   }

   if (NULL == pSysSetAttr)
   {
      ERR("copyFromMBMSSetAttr: NULL param",0,0,0);
      return;
   }
   
   pAEECMSetAttr->mbms_parm1 = pSysSetAttr->mbms_parm1;
   pAEECMSetAttr->mbms_parm2 = pSysSetAttr->mbms_parm2;
   pAEECMSetAttr->mbms_parm3 = pSysSetAttr->mbms_parm3;
   pAEECMSetAttr->mbms_parm4 = pSysSetAttr->mbms_parm4;
   pAEECMSetAttr->mbms_parm5 = pSysSetAttr->mbms_parm5;
   pAEECMSetAttr->mbms_parm6 = pSysSetAttr->mbms_parm6;
}

/*=============================================================================
FUNCTION: copyMBMSRpt

DESCRIPTION:
   Copys from the cm_mbms_info_s_type structure to the AEECM
   equivalent structure, AEECMMBMSEventData.

   This is a file scope only utility function.

DEPENDENCIES:
  cm_mbms_info_s_type in cm.h

=============================================================================*/
static void copyMBMSRpt(AEECMMBMSGenericRpt *pAEECMMBMSRpt,
                        const cm_mbms_generic_rpt_s_type *pCMMBMSRpt)
{

   if (NULL == pAEECMMBMSRpt)
   {
      ERR("copyMBMSRpt: NULL param",0,0,0);
      return;
   }

   if (NULL == pCMMBMSRpt)
   {
      ERR("copyMBMSRpt: NULL param",0,0,0);
      return;
   }

   if (AEECM_MBMS_RPT_MAX != SYS_GW_MBMS_RESP_MAX)
   {
      ERR("copyMBMSRpt: Enums mismatch!",0,0,0);
   }

   MSG_ERROR("copyMBMSRpt: cm rpt_type %d",pCMMBMSRpt->rpt_type,0,0);

   pAEECMMBMSRpt->rpt_type = (AEECMMBMSRptType)pCMMBMSRpt->rpt_type;

   switch(pAEECMMBMSRpt->rpt_type)
   {
      case AEECM_MBMS_RPT_ACTIVATE_CNF:
      copyMBMSActivateCnf(&pAEECMMBMSRpt->rpt.activate_cnf,
                          &pCMMBMSRpt->rpt.activate_cnf);
      break;
     
      case AEECM_MBMS_RPT_DEACTIVATE_CNF:
      copyMBMSDeactivateCnf(&pAEECMMBMSRpt->rpt.deactivate_cnf,
                            &pCMMBMSRpt->rpt.deactivate_cnf);
      break;
     
      case AEECM_MBMS_RPT_SERVICE_IND:
      copyMBMSServiceInd(&pAEECMMBMSRpt->rpt.service_ind,
                         &pCMMBMSRpt->rpt.service_ind);
      break;
     
      case AEECM_MBMS_RPT_CAPABILITY_IND:
      copyMBMSCapabilityInd(&pAEECMMBMSRpt->rpt.capability_ind,
                     &pCMMBMSRpt->rpt.capability_ind);
      break;
     
      case AEECM_MBMS_RPT_CONTEXT_ACTIVATED_IND:
      copyMBMSContextActivatedInd(&pAEECMMBMSRpt->rpt.context_activated_ind,
                                  &pCMMBMSRpt->rpt.context_activated_ind);
      break;
     
      case AEECM_MBMS_RPT_CONTEXT_DEACTIVATED_IND:
      copyMBMSContextDeactivatedInd(&pAEECMMBMSRpt->rpt.context_deactivated_ind,
                                    &pCMMBMSRpt->rpt.context_deactivated_ind);
      break;
     
      case AEECM_MBMS_RPT_ABORT_CNF:
      copyMBMSAbortCnf(&pAEECMMBMSRpt->rpt.abort_cnf,
                       &pCMMBMSRpt->rpt.abort_cnf);
      break;
     
      case AEECM_MBMS_RPT_SUSPEND_CNF:
      copyMBMSSuspendCnf(&pAEECMMBMSRpt->rpt.suspend_cnf,
                         &pCMMBMSRpt->rpt.suspend_cnf);
      break;
     
      case AEECM_MBMS_RPT_RESUME_CNF:
      copyMBMSResumeCnf(&pAEECMMBMSRpt->rpt.resume_cnf,
                        &pCMMBMSRpt->rpt.resume_cnf);
      break;
     
      case AEECM_MBMS_RPT_AVAIL_SERVICES_LIST_CNF:
      copyMBMSAvailServicesListCnf(&pAEECMMBMSRpt->rpt.avail_srv_list_cnf,
                                   &pCMMBMSRpt->rpt.avail_srv_list_cnf);
      break;

      case AEECM_MBMS_RPT_LL_READY_IND:
      copyMBMSLLReadyInd(&pAEECMMBMSRpt->rpt.ll_ready_ind,
                         &pCMMBMSRpt->rpt.ll_ready_ind);

      case AEECM_MBMS_RPT_SET_ATTR:
      copyMBMSSetAttrRpt(&pAEECMMBMSRpt->rpt.set_attr,
                         &pCMMBMSRpt->rpt.set_attr);
      break;

      default:
      MSG_ERROR("copyMBMSRpt: Invalid rpt_type %d",pAEECMMBMSRpt->rpt_type,0,0);
      break;
   }
}

/*=============================================================================
FUNCTION: copyFromMBMSCmd

DESCRIPTION:
   Copys from the cm_mbms_info_s_type structure to the AEECM
   equivalent structure, AEECMMBMSEventData.

   This is a file scope only utility function.

DEPENDENCIES:
  cm_mbms_info_s_type in cm.h

=============================================================================*/
static void copyFromMBMSCmd(AEECMMBMSCmd *pAEECMMBMSCmd,
                        const cm_mbms_generic_cmd_s_type *pCMMBMSCmd)
{

   if (NULL == pAEECMMBMSCmd)
   {
      ERR("copyFromMBMSCmd: NULL param",0,0,0);
      return;
   }

   if (NULL == pCMMBMSCmd)
   {
      ERR("copyFromMBMSCmd: NULL param",0,0,0);
      return;
   }

   pAEECMMBMSCmd->cmd_type = (AEECMMBMSCmdType)pCMMBMSCmd->cmd_type;

   switch(pAEECMMBMSCmd->cmd_type)
   {
      case AEECM_MBMS_CMD_ACTIVATE_REQ:
      copyFromMBMSActivateReq(&pAEECMMBMSCmd->cmd.activate_req,
                              &pCMMBMSCmd->cmd.activate_req);
      break;

      case AEECM_MBMS_CMD_DEACTIVATE_REQ:
      copyFromMBMSDeactivateReq(&pAEECMMBMSCmd->cmd.deactivate_req,
                                &pCMMBMSCmd->cmd.deactivate_req);
      break;
  
      case AEECM_MBMS_CMD_SESSION_COMPLETE_IND:
      copyFromMBMSSessionCompleteInd(&pAEECMMBMSCmd->cmd.session_complete_ind,
                                     &pCMMBMSCmd->cmd.session_complete_ind);
      break;

      case AEECM_MBMS_CMD_ABORT_REQ:
      copyFromMBMSAbortReq(&pAEECMMBMSCmd->cmd.abort_req,
                           &pCMMBMSCmd->cmd.abort_req);
      break;

      case AEECM_MBMS_CMD_SUSPEND_REQ:
      copyFromMBMSSuspendReq(&pAEECMMBMSCmd->cmd.suspend_req,
                             &pCMMBMSCmd->cmd.suspend_req);
      break;

      case AEECM_MBMS_CMD_RESUME_REQ:
      copyFromMBMSResumeReq(&pAEECMMBMSCmd->cmd.resume_req,
                            &pCMMBMSCmd->cmd.resume_req);
      break;

      case AEECM_MBMS_CMD_AVAIL_SERVICES_LIST_REQ:
      copyFromMBMSAvailServicesListReq(&pAEECMMBMSCmd->cmd.avail_srv_list_req,
                                       &pCMMBMSCmd->cmd.avail_srv_list_req);
      break;

      case AEECM_MBMS_CMD_GET_CAPABILITY:
      copyFromMBMSGetCapability(&pAEECMMBMSCmd->cmd.capability_req,
                                &pCMMBMSCmd->cmd.capability_req);
      break;

      case AEECM_MBMS_CMD_SET_ATTR:
      copyFromMBMSSetAttr(&pAEECMMBMSCmd->cmd.set_attr,
                          &pCMMBMSCmd->cmd.set_attr);
      break;

      default:
      MSG_ERROR("copyFromMBMSCmd: Invalid cmd_type %d",pAEECMMBMSCmd->cmd_type,0,0);
      break;
   }

}

/*=============================================================================
FUNCTION: OEMCMHelper_CopyMBMSInfo

DESCRIPTION:
   Copies from cm_mbms_info_s_type to AEECMMBMSEventData.
   This is a file scope only utility function.

DEPENDENCIES:
  cm_mbms_info_s_type in cm.h
=============================================================================*/
void OEMCMHelper_CopyMBMSInfo(AEECMMBMSEventData *pAEECMMBMSInfo,
                              const cm_mbms_info_s_type *pCMMBMSInfo)
{

   if (NULL == pAEECMMBMSInfo)
   {
      ERR("copyMBMSInfo: NULL param",0,0,0);
      return;
   }

   if (NULL == pCMMBMSInfo)
   {
      ERR("copyMBMSInfo: NULL param",0,0,0);
      return;
   }

   copyMBMSRpt(&pAEECMMBMSInfo->mbms_generic_rpt,
               &pCMMBMSInfo->mbms_generic_rpt);

   copyFromMBMSCmd(&pAEECMMBMSInfo->mbms_generic_cmd,
                   &pCMMBMSInfo->mbms_generic_cmd);

   pAEECMMBMSInfo->requesting_client_id = pCMMBMSInfo->requesting_client_id;

}

/*=============================================================================
FUNCTION: copyToMBMSActivateReq

DESCRIPTION:
   Copies data in a AEECMMBMSCmdActivateReqInfo struct to the CM equivalent,
   sys_gw_mbms_activate_req_s_type.
  
PARAMETERS:
  pSysActivateReq    [out]: Struct that is passed to CM through cm_mbms_cmd_generic.
  pAEECMActivateReq  [in]:  Struct that is filled by the APP.

=============================================================================*/
static void copyToMBMSActivateReq(sys_gw_mbms_activate_req_s_type *pSysActivateReq,
                                  const AEECMMBMSCmdActivateReqInfo *pAEECMActivateReq)
{
   uint32 i;

   if (NULL == pSysActivateReq)
   {
      ERR("copyToMBMSActivateReq: NULL param",0,0,0);
      return;
   }

   if (NULL == pAEECMActivateReq)
   {
      ERR("copyToMBMSActivateReq: NULL param",0,0,0);
      return;
   }
   
   if (pAEECMActivateReq->num_services > SYS_MBMS_NUM_MAX_SRVS_PER_REQ)
   {
      MSG_ERROR("copyToMBMSActivateReq: num_services out of range %d",pAEECMActivateReq->num_services,0,0);
      pSysActivateReq->num_services = SYS_MBMS_NUM_MAX_SRVS_PER_REQ;
   }
   else
   {
      pSysActivateReq->num_services = pAEECMActivateReq->num_services;
   }

   for (i=0;i<pSysActivateReq->num_services;i++)
   {
      copyTmgiTosys(&pSysActivateReq->services[i].tmgi, &pAEECMActivateReq->services[i].tmgi);
      copyIpAddrToSys(&pSysActivateReq->services[i].mip, &pAEECMActivateReq->services[i].mip);
      pSysActivateReq->services[i].port = pAEECMActivateReq->services[i].port;
      pSysActivateReq->services[i].priority.present = pAEECMActivateReq->services[i].priority.present;
      pSysActivateReq->services[i].priority.priority = pAEECMActivateReq->services[i].priority.priority;
      pSysActivateReq->services[i].srv_mode = (sys_gw_mbms_srv_mode_e_type)pAEECMActivateReq->services[i].srv_mode;
      pSysActivateReq->services[i].srv_type = (sys_gw_mbms_srv_type_e_type)pAEECMActivateReq->services[i].srv_type;
      pSysActivateReq->services[i].mbms_parm = pAEECMActivateReq->services[i].mbms_parm;
   }
      
}

/*=============================================================================
FUNCTION: copyToMBMSDeactivateReq

DESCRIPTION:
   Copies data in a AEECMMBMSCmdDeactivateReqInfo struct to the CM equivalent,
   sys_gw_mbms_deactivate_req_s_type.
  
PARAMETERS:
  pSysDeactivateReq    [out]: Struct that is passed to CM through cm_mbms_cmd_generic.
  pAEECMDeactivateReq  [in]:  Struct that is filled by the APP.

=============================================================================*/
static void copyToMBMSDeactivateReq(sys_gw_mbms_deactivate_req_s_type *pSysDeactivateReq,
                                    const AEECMMBMSCmdDeactivateReqInfo *pAEECMDeactivateReq)
{
   uint32 i;

   if (NULL == pSysDeactivateReq)
   {
      ERR("copyToMBMSDeactivateReq: NULL param",0,0,0);
      return;
   }

   if (NULL == pAEECMDeactivateReq)
   {
      ERR("copyToMBMSDeactivateReq: NULL param",0,0,0);
      return;
   }
   
   if (pAEECMDeactivateReq->num_services > SYS_MBMS_NUM_MAX_SRVS_PER_REQ)
   {
      MSG_ERROR("copyToMBMSDeactivateReq: num_services out of range %d",pAEECMDeactivateReq->num_services,0,0);
      pSysDeactivateReq->num_services = SYS_MBMS_NUM_MAX_SRVS_PER_REQ;
   }
   else
   {
      pSysDeactivateReq->num_services = pAEECMDeactivateReq->num_services;
   }

   for (i=0;i<pSysDeactivateReq->num_services;i++)
   {
      copyTmgiTosys(&pSysDeactivateReq->services[i].tmgi, &pAEECMDeactivateReq->services[i].tmgi);
      pSysDeactivateReq->services[i].mbms_parm = pAEECMDeactivateReq->services[i].mbms_parm;
   }

}

/*=============================================================================
FUNCTION: copyToMBMSSessionCompleteInd

DESCRIPTION:
   Copies data in a AEECMMBMSCmdSessionCompleteIndInfo struct to the CM equivalent,
   sys_gw_mbms_session_complete_ind_s_type.
  
PARAMETERS:
  pSysSessionCompleteInd    [out]: Struct that is passed to CM through cm_mbms_cmd_generic.
  pAEECMSessionCompleteInd  [in]:  Struct that is filled by the APP.

=============================================================================*/
static void copyToMBMSSessionCompleteInd(sys_gw_mbms_session_complete_ind_s_type *pSysSessionCompleteInd,
                                         const AEECMMBMSCmdSessionCompleteIndInfo *pAEECMSessionCompleteInd)
{

   if (NULL == pSysSessionCompleteInd)
   {
      ERR("copyToMBMSSessionCompleteInd: NULL param",0,0,0);
      return;
   }

   if (NULL == pAEECMSessionCompleteInd)
   {
      ERR("copyToMBMSSessionCompleteInd: NULL param",0,0,0);
      return;
   }

   copyTmgiTosys(&pSysSessionCompleteInd->tmgi,  &pAEECMSessionCompleteInd->tmgi);
   pSysSessionCompleteInd->session_id.present = pAEECMSessionCompleteInd->session_id.present;
   pSysSessionCompleteInd->session_id.session_id = pAEECMSessionCompleteInd->session_id.session_id;
   pSysSessionCompleteInd->mbms_parm = pAEECMSessionCompleteInd->mbms_parm;
}

/*=============================================================================
FUNCTION: copyToMBMSAbortReq

DESCRIPTION:
   Copies data in a AEECMMBMSCmdAbortReqInfo struct to the CM equivalent,
   sys_gw_mbms_abort_req_s_type.
  
PARAMETERS:
  pSysAbortReq    [out]: Struct that is passed to CM through cm_mbms_cmd_generic.
  pAEECMAbortReq  [in]:  Struct that is filled by the APP.

=============================================================================*/
static void copyToMBMSAbortReq(sys_gw_mbms_abort_req_s_type *pSysAbortReq,
                               const AEECMMBMSCmdAbortReqInfo *pAEECMAbortReq)
{
   uint32 i;

   if (NULL == pSysAbortReq)
   {
      ERR("copyToMBMSAbortReq: NULL param",0,0,0);
      return;
   }

   if (NULL == pAEECMAbortReq)
   {
      ERR("copyToMBMSAbortReq: NULL param",0,0,0);
      return;
   }
   
   if (pAEECMAbortReq->num_services > SYS_MBMS_NUM_MAX_SRVS_PER_REQ)
   {
      MSG_ERROR("copyToMBMSAbortReq: num_services out of range %d",pAEECMAbortReq->num_services,0,0);
      pSysAbortReq->num_services = SYS_MBMS_NUM_MAX_SRVS_PER_REQ;
   }
   else
   {
      pSysAbortReq->num_services = pAEECMAbortReq->num_services;
   }

   for (i=0;i<pSysAbortReq->num_services;i++)
   {
      copyTmgiTosys(&pSysAbortReq->services[i].tmgi, &pAEECMAbortReq->services[i].tmgi);
      copyIpAddrToSys(&pSysAbortReq->services[i].mip, &pAEECMAbortReq->services[i].mip);
      pSysAbortReq->services[i].port = pAEECMAbortReq->services[i].port;
      pSysAbortReq->services[i].abort_type = (sys_gw_mbms_abort_type_e_type)pAEECMAbortReq->services[i].abort_type;
      pSysAbortReq->services[i].mbms_parm = pAEECMAbortReq->services[i].mbms_parm;
   }
   
}

/*=============================================================================
FUNCTION: copyToMBMSSuspendReq

DESCRIPTION:
   Copies data in a AEECMMBMSCmdSuspendReqInfo struct to the CM equivalent,
   sys_gw_mbms_suspend_req_s_type.
  
PARAMETERS:
  pSysSuspendReq    [out]: Struct that is passed to CM through cm_mbms_cmd_generic.
  pAEECMSuspendReq  [in]:  Struct that is filled by the APP.

=============================================================================*/
static void copyToMBMSSuspendReq(sys_gw_mbms_suspend_req_s_type *pSysSuspendReq,
                                 const AEECMMBMSCmdSuspendReqInfo *pAEECMSuspendReq)
{
   uint32 i;

   if (NULL == pSysSuspendReq)
   {
      ERR("copyToMBMSSuspendReq: NULL param",0,0,0);
      return;
   }

   if (NULL == pAEECMSuspendReq)
   {
      ERR("copyToMBMSSuspendReq: NULL param",0,0,0);
      return;
   }
   
   if (pAEECMSuspendReq->num_services > SYS_MBMS_NUM_MAX_SRVS_PER_REQ)
   {
      MSG_ERROR("copyToMBMSSuspendReq: num_services out of range %d",pAEECMSuspendReq->num_services,0,0);
      pSysSuspendReq->num_services = SYS_MBMS_NUM_MAX_SRVS_PER_REQ;
   }
   else
   {
      pSysSuspendReq->num_services = pAEECMSuspendReq->num_services;
   }

   for(i=0;i<pSysSuspendReq->num_services;i++)
   {
      copyTmgiTosys(&pSysSuspendReq->services[i].tmgi, &pAEECMSuspendReq->services[i].tmgi);
      pSysSuspendReq->services[i].mbms_parm = pAEECMSuspendReq->services[i].mbms_parm;
   }
   
}

/*=============================================================================
FUNCTION: copyToMBMSResumeReq

DESCRIPTION:
   Copies data in a AEECMMBMSCmdResumeReqInfo struct to the CM equivalent,
   sys_gw_mbms_resume_req_s_type.
  
PARAMETERS:
  pSysResumeReq    [out]: Struct that is passed to CM through cm_mbms_cmd_generic.
  pAEECMResumeReq  [in]:  Struct that is filled by the APP.

=============================================================================*/
static void copyToMBMSResumeReq(sys_gw_mbms_resume_req_s_type *pSysResumeReq,
                                const AEECMMBMSCmdResumeReqInfo *pAEECMResumeReq)
{
   uint32 i;

   if (NULL == pSysResumeReq)
   {
      ERR("copyToMBMSResumeReq: NULL param",0,0,0);
      return;
   }

   if (NULL == pAEECMResumeReq)
   {
      ERR("copyToMBMSResumeReq: NULL param",0,0,0);
      return;
   }
   
   if (pAEECMResumeReq->num_services > SYS_MBMS_NUM_MAX_SRVS_PER_REQ)
   {
      MSG_ERROR("copyToMBMSResumeReq: num_services out of range %d",pAEECMResumeReq->num_services,0,0);
      pSysResumeReq->num_services = SYS_MBMS_NUM_MAX_SRVS_PER_REQ;
   }
   else
   {
      pSysResumeReq->num_services = pAEECMResumeReq->num_services;
   }

   for(i=0;i<pSysResumeReq->num_services;i++)
   {
      copyTmgiTosys(&pSysResumeReq->services[i].tmgi, &pAEECMResumeReq->services[i].tmgi);
      pSysResumeReq->services[i].mbms_parm = pAEECMResumeReq->services[i].mbms_parm;
   }

}

/*=============================================================================
FUNCTION: copyToMBMSAvailServicesListReq

DESCRIPTION:
   Copies data in a AEECMMBMSCmdAvailSrvListReqInfo struct to the CM equivalent,
   sys_gw_mbms_avail_srv_list_req_s_type.
  
PARAMETERS:
  pSysAvailSrvListReq    [out]: Struct that is passed to CM through cm_mbms_cmd_generic.
  pAEECMAvailSrvListReq  [in]:  Struct that is filled by the APP.

=============================================================================*/
static void copyToMBMSAvailServicesListReq(sys_gw_mbms_avail_srv_list_req_s_type *pSysAvailSrvListReq,
                                           const AEECMMBMSCmdAvailSrvListReqInfo *pAEECMAvailSrvListReq)
{
   if (NULL == pSysAvailSrvListReq)
   {
      ERR("copyToMBMSAvailServicesListReq: NULL param",0,0,0);
      return;
   }

   if (NULL == pAEECMAvailSrvListReq)
   {
      ERR("copyToMBMSAvailServicesListReq: NULL param",0,0,0);
      return;
   }

   pSysAvailSrvListReq->mbms_parm = pAEECMAvailSrvListReq->mbms_parm;
   
}

/*=============================================================================
FUNCTION: copyToMBMSGetCapability

DESCRIPTION:
   Copies data in a AEECMMBMSCmdCapabilityReqInfo struct to the CM equivalent,
   sys_gw_mbms_capability_req_s_type.
  
PARAMETERS:
  pSysCapabilityReq        [out]: Struct that is passed to CM through cm_mbms_cmd_generic.
  pAEECMMBMSCapabilityReq  [in]:  Struct that is filled by the APP.

=============================================================================*/
static void copyToMBMSGetCapability(sys_gw_mbms_capability_req_s_type *pSysCapabilityReq,
                                    const AEECMMBMSCmdCapabilityReqInfo *pAEECMMBMSCapabilityReq)
{
   if (NULL == pSysCapabilityReq)
   {
      ERR("copyToMBMSGetCapability: NULL param",0,0,0);
      return;
   }

   if (NULL == pAEECMMBMSCapabilityReq)
   {
      ERR("copyToMBMSGetCapability: NULL param",0,0,0);
      return;
   }

   pSysCapabilityReq->mbms_parm = pAEECMMBMSCapabilityReq->mbms_parm;

}

/*=============================================================================
FUNCTION: copyToMBMSSetAttr

DESCRIPTION:
   Copies data in a AEECMMBMSCmdSetAttrInfo struct to the CM equivalent,
   sys_gw_mbms_set_attr_req_s_type.
  
PARAMETERS:
  pSysSetAttr    [out]: Struct that is passed to CM through cm_mbms_cmd_generic.                        
  pAEECMSetAttr  [in]:  Struct that is filled by the APP.

=============================================================================*/
static void copyToMBMSSetAttr(sys_gw_mbms_set_attr_req_s_type *pSysSetAttr,
                              const AEECMMBMSCmdSetAttrInfo *pAEECMSetAttr)
{
   if (NULL == pSysSetAttr)
   {
      ERR("copyToMBMSSetAttr: NULL param",0,0,0);
      return;
   }

   if (NULL == pAEECMSetAttr)
   {
      ERR("copyToMBMSSetAttr: NULL param",0,0,0);
      return;
   }
   
   pSysSetAttr->mbms_parm1 = pAEECMSetAttr->mbms_parm1;
   pSysSetAttr->mbms_parm2 = pAEECMSetAttr->mbms_parm2;
   pSysSetAttr->mbms_parm3 = pAEECMSetAttr->mbms_parm3;
   pSysSetAttr->mbms_parm4 = pAEECMSetAttr->mbms_parm4;
   pSysSetAttr->mbms_parm5 = pAEECMSetAttr->mbms_parm5;
   pSysSetAttr->mbms_parm6 = pAEECMSetAttr->mbms_parm6;
}

/*=============================================================================
FUNCTION: copyMBMSCmdFromAEECM

DESCRIPTION:
   Copies the data from the AEECMMBMSCmd struct to the CM equivalent struct,
   cm_mbms_generic_cmd_s_type, so that the command data can be passed on to 
   the CM function cm_mbms_cmd_generic.  The AEECMMBMSCmd struct should be
   filled out by the app when calling ICM_SendMBMSCommand.
  
PARAMETERS:
  pMbmsGenericCmd    [out]: Struct that is passed to CM 
  pMbmsCmd           [in]:  Struct that is filled by the APP.

=============================================================================*/
void OEMCMHelper_CopyMBMSCmdFromAEECM(cm_mbms_generic_cmd_s_type *pMbmsGenericCmd,
                                      const AEECMMBMSCmd *pMbmsCmd)
{
   if (NULL == pMbmsGenericCmd)
   {
      ERR("copyMBMSCmdFromAEECM: NULL param",0,0,0);
      return;
   }

   if (NULL == pMbmsCmd)
   {
      ERR("copyMBMSCmdFromAEECM: NULL param",0,0,0);
      return;
   }

   if (AEECM_MBMS_CMD_MAX != SYS_GW_MBMS_REQ_MAX)
   {
      //enums don't match anymore!
      ERR("copyMBMSCmdFromAEECM: Enum mismatch",0,0,0);
   }

   pMbmsGenericCmd->cmd_type = (sys_gw_mbms_req_e_type)pMbmsCmd->cmd_type;

   switch (pMbmsCmd->cmd_type)
   {
      case AEECM_MBMS_CMD_ACTIVATE_REQ:
         copyToMBMSActivateReq(&pMbmsGenericCmd->cmd.activate_req, &pMbmsCmd->cmd.activate_req);
         break;
  
      case AEECM_MBMS_CMD_DEACTIVATE_REQ:
         copyToMBMSDeactivateReq(&pMbmsGenericCmd->cmd.deactivate_req, &pMbmsCmd->cmd.deactivate_req);
         break;
  
      case AEECM_MBMS_CMD_SESSION_COMPLETE_IND:
         copyToMBMSSessionCompleteInd(&pMbmsGenericCmd->cmd.session_complete_ind, &pMbmsCmd->cmd.session_complete_ind);
         break;
  
      case AEECM_MBMS_CMD_ABORT_REQ:
         copyToMBMSAbortReq(&pMbmsGenericCmd->cmd.abort_req, &pMbmsCmd->cmd.abort_req);
         break;
  
      case AEECM_MBMS_CMD_SUSPEND_REQ:
         copyToMBMSSuspendReq(&pMbmsGenericCmd->cmd.suspend_req, &pMbmsCmd->cmd.suspend_req);
         break;
  
      case AEECM_MBMS_CMD_RESUME_REQ:
         copyToMBMSResumeReq(&pMbmsGenericCmd->cmd.resume_req, &pMbmsCmd->cmd.resume_req);
         break;
  
      case AEECM_MBMS_CMD_AVAIL_SERVICES_LIST_REQ:
         copyToMBMSAvailServicesListReq(&pMbmsGenericCmd->cmd.avail_srv_list_req, &pMbmsCmd->cmd.avail_srv_list_req);
         break;

      case AEECM_MBMS_CMD_GET_CAPABILITY:
         copyToMBMSGetCapability(&pMbmsGenericCmd->cmd.capability_req, &pMbmsCmd->cmd.capability_req);
         break;
  
      case AEECM_MBMS_CMD_SET_ATTR:
         copyToMBMSSetAttr(&pMbmsGenericCmd->cmd.set_attr, &pMbmsCmd->cmd.set_attr);
         break;

      default:
         MSG_ERROR("OEMCM_CopyMBMSCmdFromAEECM: invalid command %d",pMbmsCmd->cmd_type,0,0);
         break;
   }

}
#endif //#ifdef FEATURE_MBMS
