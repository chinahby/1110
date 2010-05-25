/*=====================================================================
FILE: gsm1xactvprl.c

GENERAL DESCRIPTION:
   Implementation of PRL generation algorithms for GSM1x.
   Includes services for packagin PRL information according
   to IS683A standard.   Those services are mostly taken
   from DMSS code.
   

IMPLEMENTATION-SPECIFIC COMMENTS:
   The routines in this class are called from a single thread
   (BREW UI), so there is not re-entrancy requirement.  Thus,
   the routines provided in this file should be assumed non-reentrant.
    
   The implementation is based on the following documents:
       1. QUALCOMM "GSM1x MS Provisioning & Activation", 80-31492-1 X1.
       2. TIA/EIA/IS-683-A Over-the-Air Provisioning of Mobile Stations
          in Spread Spectrum Systems.
       3. TIA/EIA/IS 2000.5-A-1 Upper Layer (Layer 3) Signaling Standard
          for cdma2000 Spread Spectrum Systems.   
       

              (c) COPYRIGHT 2002, 2003 QUALCOMM Incorporated.
                         All Rights Reserved.
                      QUALCOMM Proprietary/GTDR
======================================================================*/

/*----------------------------------------------------------------------
 Include Files
----------------------------------------------------------------------*/

#include "AEEStdLib.h"                            
                            
#include "AEEGSM1xControl.h"

#include "gsm1xactvprl.h"



/*----------------------------------------------------------------------
 Defines and Typedefs
----------------------------------------------------------------------*/                                     

/* This one is used for debugging only.*/
#if defined(DEBUG_GSM1XPRL)
#define GSM1XACTVPRL_MSG_ERROR(__msg,__va1,__val2,__val3)  \
   DBGPRINTF(__msg,__va1,__val2,__val3)  
#else
#define GSM1XACTVPRL_MSG_ERROR(__msg,__va1,__val2,__val3)
#endif /* DEBUG_GSM1XPRL */




typedef enum
{          
   GSM1XACTVPRL_ACQ_REC_SOURCE_RESERVED = 0,
   GSM1XACTVPRL_ACQ_REC_SOURCE_BUILT_IN,
   GSM1XACTVPRL_ACQ_REC_SOURCE_RUIM,
   GSM1XACTVPRL_ACQ_REC_SOURCE_BREW_APP,
   GSM1XACTVPRL_ACQ_REC_SOURCE_MAX_CNT

} gsm1xactvprl_acqRecSourceType;




#define GSM1XACTVPRL_GSM_ACCOLC_LEN_BYTES            (2)
#define GSM1XACTVPRL_GSM_IMSI_LEN_BYTES              (9)

#define GSM1XACTVPRL_GSM_MSISDN_LEN_BYTES            (32)



#define  GSM1xSID_START      (15872)
#define  GSM1xSID_END        ((GSM1xSID_START) + 32)
#define  POW_10_3            (1000)



/* S_ indicates that these variables are allocated in the global
   App data structure.  This is done in order not to allocate
   these large variables on the stack.*/
#define S_NewPRL       (largeDataBuf->NewPRL)
#define S_SIDNIDPair   (largeDataBuf->SIDNIDPair)
#define S_AcqRecords   (largeDataBuf->AcqRecords)
#define S_SysRecord    (largeDataBuf->SysRecord)
#define S_PLMNEntries  (largeDataBuf->PLMNEntries)
#define S_Header       (largeDataBuf->Header)
#define S_CRC16Table   (largeDataBuf->CRC16Table)
#define S_HomeSID      (largeDataBuf->HomeSID)
#define S_HomeNID      (largeDataBuf->HomeNID)



/* Residual CRC value to compare against return value of crc_16_calc().
** Use crc_16_calc() to calculate a 16 bit CRC, and append it to the buffer.
** When crc_16_calc() is applied to the unchanged result, it returns CRC_16_OK.
*/
#define CRC_16_OK               0xE2F0

/* Mask for CRC-16 polynomial:
**
**      x^16 + x^12 + x^5 + 1
**
** This is more commonly referred to as CCITT-16.
** Note:  the x^16 tap is left off, it's implicit.
*/
#define CRC_16_POLYNOMIAL       0x1021

/* Seed value for CRC register.  The all ones seed is part of CCITT-16, as
** well as allows detection of an entire data stream of zeroes.
*/
#define CRC_16_SEED             0xFFFF

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* Seed value for CRC register.  The all zeroes seed is inverted prior to
** being used in the step-wise CRC CCITT-16.  This behaves as CRC_16_SEED.
*/
#define CRC_16_STEP_SEED        (~((uint16) CRC_16_SEED))


/* The CRC table size is based on how many bits at a time we are going
** to process through the table.  Given that we are processing the data
** 8 bits at a time, this gives us 2^8 (256) entries.
*/
#define CRC_TAB_SIZE          (256)           /* 2^CRC_TAB_BITS      */


/* CRC table for 16 bit CRC, with generator polynomial 0x1021,
** calculated 8 bits at a time, MSB first.
*/

const uint16 gsm1xactvprl_crc16_table[ CRC_TAB_SIZE ] = {
  0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
  0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
  0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
  0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
  0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
  0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
  0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
  0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
  0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
  0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
  0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
  0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
  0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
  0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
  0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
  0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
  0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
  0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
  0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
  0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
  0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
  0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
  0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
  0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
  0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
  0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
  0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
  0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
  0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
  0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
  0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
  0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};


/* --------------------------------------------------
** Masks the number of bits give by length starting
** at the given offset.  Unlike MASK and MASK_AND_SHIFT,
** this macro only creates that mask, it does not
** operate on the data buffer
** -------------------------------------------------- */
#define MASK_B(offset, len) \
  ((0xff >> offset) & (0xff << (8 - (offset + len))))


#define  GSM1XACTVPRL_SYSTEM_PRL_FILE_NAME        "system.prl"
#define  GSM1XACTVPRL_USER_CREATED_PRL_FILE_NAME  "user.prl"




/*===========================================================================

MACRO FSIZ

DESCRIPTION
  This macro computes the size, in bytes, of a specified field
  of a specified structure or union type.

PARAMETERS
  type          type of the structure or union
  field         field in the structure or union to get the size of

DEPENDENCIES
  None

RETURN VALUE
  size in bytes of the 'field' in a structure or union of type 'type'

SIDE EFFECTS
  None

===========================================================================*/

#define FSIZ( type, field ) sizeof( ((type *) 0)->field )


/*----------------------------------------------------------------------
 Global Data Declarations
----------------------------------------------------------------------*/
/* None.*/

/*----------------------------------------------------------------------
 Static Data Declarations
----------------------------------------------------------------------*/
/* None.*/

/*----------------------------------------------------------------------
 Function Prototypes
----------------------------------------------------------------------*/

static boolean provisionPRLFromFile
(
   IGSM1xControl                  *instancePtr,
   char                           *fileName,
   gsm1xactvprl_LargeDataBuffers  *largeDataBuf
);

static boolean provisionPRLFromFileAndPLMN
(
   IGSM1xControl                  *instancePtr,
   char                           *fileName,
   gsm1xactvprl_LargeDataBuffers  *largeDataBuf
);


/* Routine to get acquisition records.
   Usually these records are "built into" software.*/
static boolean getAcqRecords
(
   gsm1xactvprl_acqRecSourceType   source,
   uint32                          recdBufMaxCnt,
   prli_acq_rec_type              *recdBuf,
   uint16                         *pRecdBufCnt
);

/***********************************************************************
  Conversion routines from the format used to store at the identity card
  (SIM or R-UIM) into the format used to store at non-volatile memory(NV.)
*/

static boolean convertPLMNToSIDNID
(
   uint16                             PLMNEntriesCnt, 
   AEEGSM1xControl_PLMNTripletType   *PLMNBuf,
   uint32                             SIDNITMaxCnt,
   gsm1xactvprl_prlSIDNIDType        *SIDNID,
   uint16                            *pSIDNIDCnt,
   gsm1xactvprl_LargeDataBuffers     *largeDataBuf
);

static boolean convertMCCMNCintoSIDNID
(
   uint32  mcc,
   uint32  mnc, 
   uint16 *sid, 
   uint16 *nid
);


     
/***********************************************************************
  Packing routines for PRL fields.*/

static boolean packPRLHdr
(
   prl_hdr_s_type  *pHdr,
   uint32           bufMaxSizeBits,
   uint32           bufOffsetBits,
   byte            *buf, 
   uint32          *pNewOffsetBits
);

static boolean packAcqRec
(
   prli_acq_rec_type          *acqRecord,
   uint32                      bufMaxSizeBits,
   uint32                      bufOffsetBits,
   byte                       *buf, 
   uint32                     *pNewOffsetBits
);
         
static boolean packSysRec
(  
   prli_sys_rec_type          *sysRecord,
   uint32                      bufMaxSizeBits,
   uint32                      bufOffsetBits,
   byte                       *buf, 
   uint32                     *pNewOffsetBits
);

static boolean packCRC16
(
   uint16           CRC16,
   uint32           bufMaxSizeBits,
   uint32           bufOffsetBits,
   byte            *buf, 
   uint32          *pNewOffsetBits
);   

static boolean packByte
( 
   uint32   src, 
   byte    *dst, 
   uint32  *pOffsetBits, 
   uint32   size,
   uint32   bufMaxSizeBits
);

static boolean packWord
( 
   uint32         src, 
   byte          *dst, 
   uint32        *pOffsetBits, 
   uint32         size,
   uint32         bufMaxSizeBits
);


static uint16 crc_16_calc
(
  byte *buf_ptr,
    /* Pointer to bytes containing the data to CRC.  The bit stream starts
    ** in the MS bit of the first byte.
    */

  uint16 len
    /* Number of data bits to calculate the CRC over */
);

static void b_packb(
   byte src, 
   byte dst[], 
   uint16 pos, 
   uint16 len 
);

static void b_packw(
   uint16 src, 
   byte dst[], 
   uint16 pos, 
   uint16 len 
);


/*----------------------------------------------------------------------
 Function Definitions
----------------------------------------------------------------------*/


/*=============================================================================
FUNCTION: gsm1xactvprl_provisionGSM1xPRLToNV 

DESCRIPTION:
   Take GSM IMSI, PLMNsel and FPLMN info from SIM/R-UIM, convert it to CDMA PRL
   and write to NV.    

RETURN VALUE:
   boolean competion status (TRUE=success)

SIDE EFFECTS:
    Can change the contents of some NV fields.
=============================================================================*/
boolean gsm1xactvprl_provisionGSM1xPRLToNV
(
   IGSM1xControl                   *instancePtr,
   gsm1xactvprl_LargeDataBuffers   *largeDataBuf
)
{
#define  GSM1XACTVAPI_PRL_GEN_FROM_PLMN         (0) 
#define  GSM1XACTVAPI_PRL_GEN_FROM_SYSTEM_FILE  (1)
#define  GSM1XACTVAPI_PRL_GEN_FROM_USER_FILE    (2)
    
    
    /* TBD - only one PRL generation mode is supported for now.*/
    int                               PRLGenMode;
        
    boolean                           isValid;

    AEEGSM1xControl_SIDNIDPairType    SIDNIDInfo;

    
    PRLGenMode = GSM1XACTVAPI_PRL_GEN_FROM_PLMN;

    switch( PRLGenMode ){
       
       case GSM1XACTVAPI_PRL_GEN_FROM_PLMN:

          {
             if(! provisionPRLFromFileAndPLMN( instancePtr, GSM1XACTVPRL_SYSTEM_PRL_FILE_NAME, largeDataBuf) ){
                 return FALSE;
             }
             break;
          }
    
       case GSM1XACTVAPI_PRL_GEN_FROM_SYSTEM_FILE:
          {
             if(! provisionPRLFromFile( instancePtr, GSM1XACTVPRL_SYSTEM_PRL_FILE_NAME, largeDataBuf) ){
                 return FALSE;
             }
             break;
          }

       case GSM1XACTVAPI_PRL_GEN_FROM_USER_FILE:
          {
             if(! provisionPRLFromFile( instancePtr, GSM1XACTVPRL_USER_CREATED_PRL_FILE_NAME, largeDataBuf) ){
                return FALSE;
             }
             break;
          }
          
       default:
          {
             return FALSE;
             break;
          }
          
    } /* switch */


    if( AEEGSM1XCONTROL_STATUS_SUCCESS != 
           IGSM1xControl_ValidatePRL
           (
              instancePtr,
              (byte*)(S_NewPRL.roaming_list),
             &isValid
           )
      ){
        /* Don't want to store an invalid PRL tables.*/
        return FALSE;
    }

    if(! isValid){
        /* Don't want to store an invalid PRL tables.*/
        return FALSE;
    }

    if( AEEGSM1XCONTROL_STATUS_SUCCESS != 
           IGSM1xControl_SetGSM1xPRL
           (
              instancePtr,
              (byte*)(&S_NewPRL)
           )
      ){
       GSM1XACTVPRL_MSG_ERROR("Could not update GSM1x PRL.",0,0,0);
       return FALSE;
    }


    SIDNIDInfo.sid = S_HomeSID;
    SIDNIDInfo.nid = S_HomeNID;



    if( AEEGSM1XCONTROL_STATUS_SUCCESS !=
           IGSM1xControl_SetGSM1xSIDNIDPairs
           (
              instancePtr,
              1,
              &SIDNIDInfo,
              0,
              NULL
           )
      ){
        GSM1XACTVPRL_MSG_ERROR("Could not update Home SID/NID.",0,0,0);
        return FALSE;
    }
    
    return TRUE;
}



/*=============================================================================
FUNCTION: provisionPRLFromFile 

DESCRIPTION:
   Read PRL from the specified EFS file. TBD.
   
RETURN VALUE:
   completion status (TRUE==success)   

SIDE EFFECTS:
   None
=============================================================================*/
static boolean provisionPRLFromFile
(
   IGSM1xControl                   *instancePtr,
   char                            *fileName,
   gsm1xactvprl_LargeDataBuffers   *largeDataBuf

)
{
    /* Implementation is TBD.*/
    return TRUE;
}



/*=============================================================================
FUNCTION: provisionPRLFromFileAndPLMN 

DESCRIPTION:
   Read acquisition records from the specified EFS file
   and take PLMN from SIM and generate PRL.
   
RETURN VALUE:
   completion status (TRUE==success)   

SIDE EFFECTS:
   None
=============================================================================*/
static boolean provisionPRLFromFileAndPLMN
(
   IGSM1xControl                  *instancePtr,
   char                           *fileName,
   gsm1xactvprl_LargeDataBuffers  *largeDataBuf
)
{
    uint16         SIDNIDPairCnt;
   
    uint16         acqRecCnt;
   

    uint32         offsetBits;

   
    uint32         acqRecIndex;
    uint32         sysRecIndex;

    uint32         num_reserved_bits;

    uint16         roamingListCRC16;

    uint32         crcOffsetBits;

  
    uint16         PLMNEntriesCnt;
    
   
    /* Obtain acquisition records.*/
    /* TBD - need to take acquisition records from file
       "fileName", not from hard coded stuff.*/
    if( ! getAcqRecords
          (
             GSM1XACTVPRL_ACQ_REC_SOURCE_BUILT_IN,
             sizeof(S_AcqRecords)/sizeof(prli_acq_rec_type),
             S_AcqRecords,
            &acqRecCnt
          )
       ){
        GSM1XACTVPRL_MSG_ERROR("Could not get acquisition records.",0,0,0);
        return FALSE;
    }

    
    if( AEEGSM1XCONTROL_STATUS_SUCCESS != 
           IGSM1xControl_GetPLMN
           (
              instancePtr,
              AEEGSM1XCONTROL_HOME_PLMN	| AEEGSM1XCONTROL_SEL_PLMN | AEEGSM1XCONTROL_FORBIDDEN_PLMN,
              sizeof(S_PLMNEntries)/sizeof(AEEGSM1xControl_PLMNTripletType), 
              S_PLMNEntries,  
             &PLMNEntriesCnt
           )
      ){
       GSM1XACTVPRL_MSG_ERROR("Could not read PLMN from SIM.",0,0,0);
       return FALSE;
    }

    GSM1XACTVPRL_MSG_ERROR("Received %d entries from SIM.",PLMNEntriesCnt,0,0);
       

    GSM1XACTVPRL_MSG_ERROR("VALUE0 =%d %d %d",(int)S_PLMNEntries[0].MCC,
                                (int)S_PLMNEntries[0].MNC,
                                (int)(S_PLMNEntries[0].PLMNtype));


    GSM1XACTVPRL_MSG_ERROR("VALUE1 =%d %d %d",(int)S_PLMNEntries[1].MCC,
                                (int)S_PLMNEntries[1].MNC,
                                (int)(S_PLMNEntries[1].PLMNtype));


    GSM1XACTVPRL_MSG_ERROR("VALUES2 =%d %d %d",(int)S_PLMNEntries[2].MCC,
                                (int)S_PLMNEntries[2].MNC,
                                (int)(S_PLMNEntries[3].PLMNtype));



    if( ! convertPLMNToSIDNID
          (
             PLMNEntriesCnt,
             S_PLMNEntries,  
             sizeof(S_SIDNIDPair)/sizeof(gsm1xactvprl_prlSIDNIDType),
             S_SIDNIDPair,
            &SIDNIDPairCnt,
             largeDataBuf
          )
      ){
        GSM1XACTVPRL_MSG_ERROR("Could not convert MCC:MNC pairs into SID:NID pairs.",0,0,0);
        return FALSE;
    }

    S_NewPRL.prl_version  = 0; /* This is what the 5105 build is using.*/
    
    /* roaming_list is a byte array that contains packed
          header
          acquisition records
          system records
          crc16
    */


    /* The header contains fields  pr_list_size,acq_tbl_bit_offset,
       and sys_tbl_bit_offset that cannot be computed until
       all acquisition records and all system records are packed !
       To overcome this, we first pack "unfinished" header just
       to get the offset correctly, then pack acquisition records
       and system records, then fill out these fields in the header
       and pack it again.*/

    S_Header.is_disabled  = 0;
    S_Header.pr_list_id   = 0;
    S_Header.pref_only    = 0; /* TBD - is this what we want?*/
    S_Header.def_roam_ind = DB_ROAM_OFF; /* actually on.*/
    S_Header.num_acq_recs = acqRecCnt;
    S_Header.num_sys_recs = SIDNIDPairCnt * acqRecCnt;
                             

    offsetBits = 0;

    /* Pack unfinished header just to get offsets correctly.*/
    if( ! packPRLHdr
          (
             &S_Header,
             8 * sizeof(S_NewPRL.roaming_list),
             offsetBits,
             (byte*)(S_NewPRL.roaming_list),
            &offsetBits
          )
      ){
        GSM1XACTVPRL_MSG_ERROR("Could not pack unfinished PRL header into a byte array.",0,0,0);
        return FALSE;
    }
            
    S_Header.acq_tbl_bit_offset = (uint16)offsetBits;
       
    /* Pack acquisition records.*/
    for( acqRecIndex = 0; acqRecIndex < S_Header.num_acq_recs; acqRecIndex++ ){

        if( ! packAcqRec
              (
                  &S_AcqRecords[acqRecIndex],
                  (8 * sizeof(S_NewPRL.roaming_list)) - offsetBits,
                  offsetBits,
                  (byte*)(S_NewPRL.roaming_list),
                 &offsetBits
              )
          ){
            GSM1XACTVPRL_MSG_ERROR("Could not pack acquisition record# %d",(int)acqRecIndex,0,0);
            return FALSE;
        }
    } /* for */

    S_Header.sys_tbl_bit_offset = (uint16)offsetBits;

    /* Generate and pack system records.
       We generate a record for each (SID,NID) pair for each 
       acquisition record. */
    for( sysRecIndex = 0; sysRecIndex < SIDNIDPairCnt; sysRecIndex++ ){
    
        for( acqRecIndex = 0; acqRecIndex < S_Header.num_acq_recs; acqRecIndex++ ){

           

           S_SysRecord.sid       = S_SIDNIDPair[sysRecIndex].SID;
           S_SysRecord.nid_incl  = 1;
           S_SysRecord.nid       = S_SIDNIDPair[sysRecIndex].NID;
           S_SysRecord.neg_pref  = (S_SIDNIDPair[sysRecIndex].isPreferred) ? 
                                     PRL_PREFERRED : PRL_NEGATIVE;
           S_SysRecord.geo       = ( (0==sysRecIndex) && (0==acqRecIndex) ) ? 
                                  PRL_NEW_GEO_REGION : PRL_SAME_GEO_REGION;
           
           /* This field only makes sense for preferred records.
              It should be set to PRL_PRI_HIGHER for all records except
              the last preferred record.*/
           S_SysRecord.pri = PRL_PRI_HIGHER;
           if( 
               ( (uint16)(S_Header.num_acq_recs - 1) == acqRecIndex )
               &&
               (
                  ( (uint16)(SIDNIDPairCnt - 1) == sysRecIndex )
                  ||
                  (! S_SIDNIDPair[sysRecIndex+1].isPreferred )
               )
             ){
              S_SysRecord.pri   = PRL_PRI_SAME;
           }
               
           S_SysRecord.acq_index = (byte)acqRecIndex;
           
           
           /* We disable roaming indication just like GSM does not show
              roaming indication for PLMN entries.*/
           S_SysRecord.roam_ind = 1; /* No roaming indication.*/
                       

           if( ! packSysRec
                 (
                   &S_SysRecord,
                    (8 * sizeof(S_NewPRL.roaming_list)) - offsetBits,
                    offsetBits,
                    (byte*)(S_NewPRL.roaming_list),
                   &offsetBits
                 )
             ){
               GSM1XACTVPRL_MSG_ERROR
               (
                   "Could not pack system rec, SIDNID# %d acqInd=%d",
                   (int)sysRecIndex,
                   (int)acqRecIndex,
                   0
               );
               return FALSE;
           }
        } /* for */
    } /* for */


    /* If the current offset in bits is not a multiple of 8,
       we pad the array with so called reserved bits.*/
    num_reserved_bits = (8 - (offsetBits % 8)) % 8;
    offsetBits += num_reserved_bits;
    
    /* Save the current offset so we can put CRC in there.*/
    crcOffsetBits    = offsetBits;

    offsetBits      += 16; /* size of CRC16.*/

    S_Header.pr_list_size = (uint16)(offsetBits/8); /*This needs to be in bytes*/
    S_Header.is_valid     = TRUE;


    S_NewPRL.valid       = TRUE;
    S_NewPRL.size        = (uint16)offsetBits;
    

    /* Now, that we have filled out all the fields in the header
       we have to pack it again into the beginning of the buffer.*/
    offsetBits = 0;
    
    if( ! packPRLHdr
          (
             &S_Header,
             8 * sizeof(S_NewPRL.roaming_list),
             offsetBits,
             (byte*)(S_NewPRL.roaming_list),
            &offsetBits
          )
      ){
        GSM1XACTVPRL_MSG_ERROR("Could not pack finished PRL header into a byte array.",0,0,0);
        return FALSE;
    }

    
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      Compute the CRC for the table.
    */
    roamingListCRC16 = crc_16_calc
                       (
                           (byte*)(S_NewPRL.roaming_list),
                           (uint16)(8*(S_Header.pr_list_size - sizeof(roamingListCRC16)))
                       );

          
    if( ! packCRC16
          (
             roamingListCRC16,
             8 * sizeof(S_NewPRL.roaming_list),
             crcOffsetBits,
             (byte*)(S_NewPRL.roaming_list),
            &offsetBits
          )
      ){
        GSM1XACTVPRL_MSG_ERROR("Could not pack CRC16 into a byte array.",0,0,0);
        return FALSE;
    }

    GSM1XACTVPRL_MSG_ERROR("NEW NUMBER OF SYS_RECS IS %d",(int)(S_Header.num_sys_recs),0,0);
    
    return TRUE;
}


/*======================================================================
FUNCTION:   getAcqRecords

DESCRIPTION:
   Get the acquisition records from the specified
   source and put them into the specified array.
   
RETURN VALUE:
   boolean 
SIDE EFFECTS:
   None
======================================================================*/
static boolean getAcqRecords
(
   gsm1xactvprl_acqRecSourceType   source,
   uint32                          recdBufMaxCnt,
   prli_acq_rec_type              *recdBuf,
   uint16                         *pRecdBufCnt
)
{
   /* Validate arguments.*/
   if( 
        (NULL == recdBuf)
        ||
        (NULL == pRecdBufCnt)
     ){
       GSM1XACTVPRL_MSG_ERROR("NULL pointer is not allowed here.",0,0,0);
       return FALSE;
   }

   switch(source){
      
      case GSM1XACTVPRL_ACQ_REC_SOURCE_BUILT_IN:
         {

            if( recdBufMaxCnt < 2 ){
                GSM1XACTVPRL_MSG_ERROR("Supplied acq rec buf is too small.",0,0,0);
                return FALSE;
            }
           /*

           *pRecdBufCnt = 2;
           
           recdBuf[0].acq_type = PRL_ACQ_REC_CELLULAR_CDMA_STD;
           recdBuf[0].type.cellular_cdma.a_b     = PRL_AB_SYSTEM_A_OR_B;
           recdBuf[0].type.cellular_cdma.pri_sec = PRL_PRISEC_PRI_OR_SEC;

           recdBuf[1].acq_type = PRL_ACQ_REC_CELLULAR_CDMA_CUST;
           recdBuf[1].type.cellular_cdma_cust.num_chans = 3;
           recdBuf[1].type.cellular_cdma_cust.chan[0]   = 304;
           recdBuf[1].type.cellular_cdma_cust.chan[1]   = 786;
           recdBuf[1].type.cellular_cdma_cust.chan[2]   = 786;
           */

           *pRecdBufCnt = 2;

           recdBuf[0].acq_type = PRL_ACQ_REC_CELLULAR_CDMA_CUST;
           recdBuf[0].type.cellular_cdma_cust.num_chans = 3;
           recdBuf[0].type.cellular_cdma_cust.chan[0]   = 777;
           recdBuf[0].type.cellular_cdma_cust.chan[1]   = 758;
           recdBuf[0].type.cellular_cdma_cust.chan[2]   = 786;

           recdBuf[1].acq_type = PRL_ACQ_REC_CELLULAR_CDMA_STD;
           recdBuf[1].type.cellular_cdma.a_b     = PRL_AB_SYSTEM_A_OR_B;
           recdBuf[1].type.cellular_cdma.pri_sec = PRL_PRISEC_PRI_OR_SEC;
           
           /*
           recdBuf[1].acq_type = PRL_ACQ_REC_CELLULAR_CDMA_CUST;
           recdBuf[1].type.cellular_cdma_cust.num_chans = 1;
           recdBuf[1].type.cellular_cdma_cust.chan[0]   = 999;
           */
          
           
           break;
         }
      
      case GSM1XACTVPRL_ACQ_REC_SOURCE_RUIM:
         {
            GSM1XACTVPRL_MSG_ERROR("Getting acquisition records from R-UIM is not supported.",0,0,0);
            return FALSE;
         }
      
      case GSM1XACTVPRL_ACQ_REC_SOURCE_BREW_APP:
         {
            GSM1XACTVPRL_MSG_ERROR("Getting acquisition records from BREW APP is not supported.",0,0,0);
             return FALSE;
         }

      case GSM1XACTVPRL_ACQ_REC_SOURCE_RESERVED:
      case GSM1XACTVPRL_ACQ_REC_SOURCE_MAX_CNT:
      default:
         {
            GSM1XACTVPRL_MSG_ERROR("Unsupported source %d",(int)source,0,0);
            break;
         }
   } /* switch */

   return TRUE;
}



/*======================================================================
FUNCTION:   convertPLMNToSIDNID

DESCRIPTION:
   The function forms a list of SID, NID and pref/neg flag
   from the home-, selected and forbidden MCC,MNC lists.
   
RETURN VALUE:
   boolean 
SIDE EFFECTS:
   None
======================================================================*/
static boolean convertPLMNToSIDNID
(
   uint16                            PLMNEntriesCnt, 
   AEEGSM1xControl_PLMNTripletType  *PLMNBuf,
   uint32                            SIDNITMaxCnt,
   gsm1xactvprl_prlSIDNIDType       *SIDNID,
   uint16                           *pSIDNIDCnt,
   gsm1xactvprl_LargeDataBuffers    *largeDataBuf
)
{
    uint32    SIDNIDIndex;
    uint32    entryIndex;

    /* Initialize global variables that will be set below.*/
    S_HomeSID = 0;
    S_HomeNID = 0;
    
    
    /*----------- Argument(s) validation.-----------*/    
    if( SIDNITMaxCnt < PLMNEntriesCnt ){
       GSM1XACTVPRL_MSG_ERROR
       (
           "SIDNID buffer is too small (avail=%d needed=%d",
           (int)SIDNITMaxCnt,
           (int)PLMNEntriesCnt,
           0
       );
       return FALSE;
    }

    if( (0 != PLMNEntriesCnt) && (NULL == PLMNBuf) ){
       GSM1XACTVPRL_MSG_ERROR("Invalid input parameter(s).",0,0,0);
       return FALSE;
    }
    
    if(
        (NULL == SIDNID)
        ||
        (NULL == pSIDNIDCnt)
      ){
        GSM1XACTVPRL_MSG_ERROR("Invalid output parameter(s).",0,0,0);
        return FALSE;
    }
    

    GSM1XACTVPRL_MSG_ERROR("PLMNENTRYCNT=%d",(int)PLMNEntriesCnt,0,0);


    for( 
           entryIndex = 0, SIDNIDIndex = 0; 
           entryIndex < PLMNEntriesCnt; 
           entryIndex++, SIDNIDIndex++ 
       ){

        GSM1XACTVPRL_MSG_ERROR("VALUESbef=%d %d %d",(int)PLMNBuf[entryIndex].MCC,
                                (int)PLMNBuf[entryIndex].MNC,
                                (int)(PLMNBuf[entryIndex].PLMNtype));


       
        if( 
            ! convertMCCMNCintoSIDNID
              (
                  PLMNBuf[entryIndex].MCC,
                  PLMNBuf[entryIndex].MNC, 
                 &(SIDNID[SIDNIDIndex].SID), 
                 &(SIDNID[SIDNIDIndex].NID)
              )
          ){
             GSM1XACTVPRL_MSG_ERROR("Could not generate SID, NID for sel PLMN.",0,0,0);
             return FALSE;
        }

        GSM1XACTVPRL_MSG_ERROR("PLMNENTRYCNTINDEX=%d",(int)entryIndex,0,0);
        GSM1XACTVPRL_MSG_ERROR("VALUESaft=%d %d %d",(int)PLMNBuf[entryIndex].MCC,
                                (int)PLMNBuf[entryIndex].MNC,
                                (int)(PLMNBuf[entryIndex].PLMNtype));


        if(AEEGSM1XCONTROL_HOME_PLMN == PLMNBuf[entryIndex].PLMNtype){
           S_HomeSID = SIDNID[entryIndex].SID;
           S_HomeNID = SIDNID[entryIndex].NID;
        }


        switch( PLMNBuf[entryIndex].PLMNtype ){
        
           case AEEGSM1XCONTROL_HOME_PLMN:
           case AEEGSM1XCONTROL_SEL_PLMN:
               {
                  SIDNID[entryIndex].isPreferred = TRUE;
                  break;
               }
        
           case AEEGSM1XCONTROL_FORBIDDEN_PLMN:
               {
                  SIDNID[entryIndex].isPreferred = FALSE;
                  break;
               }

           default:
               {
                  GSM1XACTVPRL_MSG_ERROR("Should never get here.",0,0,0);
                  return FALSE;
               }
        } /* switch */

    } /* for */
    
    *pSIDNIDCnt = PLMNEntriesCnt;
    
    return TRUE;
}


/*======================================================================
FUNCTION:   convertMCCMNCintoSIDNID

DESCRIPTION:
   The function gets as input mcc & mnc and returns as 
   output the coresponding sid & nid
   
RETURN VALUE:
   boolean 
SIDE EFFECTS:
   None
======================================================================*/
static boolean convertMCCMNCintoSIDNID
(
   uint32  mcc,
   uint32  mnc, 
   uint16 *sid, 
   uint16 *nid
)
{
    uint32 mcc_mnc, rem_mcc_mnc;
    
    /* Make sure mcc & mnc are three digit number*/
    if (mcc >= POW_10_3 || mnc >= POW_10_3){
       return FALSE;
    }
    
    mcc_mnc = mcc*POW_10_3 + mnc;
    
    *nid = (uint16)(mcc_mnc & 0xFFFF);
    
    rem_mcc_mnc = mcc_mnc >> 16;
    
    *sid = (uint16)(GSM1xSID_START + rem_mcc_mnc);

    return TRUE;
}


/*======================================================================
FUNCTION:   packPRLHdr

DESCRIPTION:
   The function packs PRL roaming list header into the specified
   byte array.
   
RETURN VALUE:
   boolean (completion status, TRUE=success)
SIDE EFFECTS:
   None
======================================================================*/
static boolean packPRLHdr
(
   prl_hdr_s_type         *pHdr,
   uint32                  bufMaxSizeBits,
   uint32                  bufOffsetBits,
   byte                   *buf, 
   uint32                 *pNewOffsetBits
)
{
  uint32  offsetBits = bufOffsetBits;
  
#define PACK_PRL_HDR_FIELD(__packFunc,__field) \
  if( ! (__packFunc)(           \
            pHdr-> __field,     \
            buf,                \
            &offsetBits,        \
            (uint32)FSIZ(prl_fixed_roaming_list_type, __field), \
            bufMaxSizeBits      \
        )                       \
    ){                          \
      return FALSE;             \
  }
    
  PACK_PRL_HDR_FIELD( packWord, pr_list_size )
  PACK_PRL_HDR_FIELD( packWord, pr_list_id   )
  PACK_PRL_HDR_FIELD( packByte, pref_only    )
  PACK_PRL_HDR_FIELD( packByte, def_roam_ind )
  PACK_PRL_HDR_FIELD( packWord, num_acq_recs )
  PACK_PRL_HDR_FIELD( packWord, num_sys_recs )
    
  *pNewOffsetBits = offsetBits;

  return TRUE;
}


/*======================================================================
FUNCTION:   packAcqRec

DESCRIPTION:
   Pack PRL acquisition record to a specified byte array.
RETURN VALUE:
   boolean completion status (TRUE=success)
SIDE EFFECTS:
   None
======================================================================*/
static boolean packAcqRec
(
   prli_acq_rec_type      *acqRecord,
   uint32                  bufMaxSizeBits,
   uint32                  bufOffsetBits,
   byte                   *buf, 
   uint32                 *pNewOffsetBits
)
{
   uint32  offsetBits = bufOffsetBits;
   int32   chanIndex;
   int32   blockIndex;


#define PACK_ACQ_REG_FIELD(__packFunc,__field,__size) \
  if( ! (__packFunc)(             \
            acqRecord-> __field,  \
            buf,                  \
            &offsetBits,          \
            (__size),             \
            bufMaxSizeBits        \
        )                         \
    ){                            \
      return FALSE;               \
  }


   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
       Pack the ACQ_TYPE field
   */

   PACK_ACQ_REG_FIELD( packWord, acq_type, sizeof(prl_acq_rec_header_type) )
   
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
       The other field depend on the ACQ_TYPE.
   */
   
   switch (acqRecord->acq_type){
        
      case PRL_ACQ_REC_RESERVED :         /* Reserved */
         {
            GSM1XACTVPRL_MSG_ERROR("Reserved acq record type", 0, 0, 0);
            return FALSE;
            break;
         }

      case PRL_ACQ_REC_CELLULAR_ANALOG :  /* Cellular analog */
         {
             
             PACK_ACQ_REG_FIELD( packWord, acq_type, sizeof(prl_acq_rec_header_type) )
             PACK_ACQ_REG_FIELD( packByte, type.cellular_analog.a_b, FSIZ(prl_cellular_analog_type, a_b) ) 
                                              
             break;
         }

#if defined(FEATURE_IS683A_PRL)
      case PRL_ACQ_REC_CELLULAR_CDMA_STD :    /* Cellular CDMA */
         {
             PACK_ACQ_REG_FIELD( packByte, type.cellular_cdma.a_b,     FSIZ(prl_cellular_cdma_type, a_b) )
             PACK_ACQ_REG_FIELD( packByte, type.cellular_cdma.pri_sec, FSIZ(prl_cellular_cdma_type, pri_sec) )
             
             break;
         }

      case PRL_ACQ_REC_CELLULAR_CDMA_CUST :
         {
             PACK_ACQ_REG_FIELD( packByte, type.cellular_cdma_cust.num_chans, FSIZ(prl_cellular_cdma_custom_type, num_chans) )

             for(chanIndex = 0; chanIndex < acqRecord->type.cellular_cdma_cust.num_chans; chanIndex++){
                 PACK_ACQ_REG_FIELD( packWord, type.cellular_cdma_cust.chan[chanIndex], FSIZ(prl_cellular_cdma_custom_type, chan) )
             } /* for */
             break;
         }
#else
      case PRL_ACQ_REC_CELLULAR_CDMA :    /* Cellular CDMA */
         {
             PACK_ACQ_REG_FIELD( packByte, type.cellular_cdma.a_b, FSIZ(prl_cellular_cdma_type, a_b) )
             PACK_ACQ_REG_FIELD( packByte, type.cellular_cdma.pri_sec, FSIZ(prl_cellular_cdma_type, pri_sec) )
             break;
         }

#endif /* FEATURE_IS683A_PRL */

      case PRL_ACQ_REC_CELLULAR_GENERIC : /* Cellular generic */
         {
             PACK_ACQ_REG_FIELD( packByte, type.cellular_generic.a_b, FSIZ(prl_cellular_generic_type, a_b) )
             break;
         }

      case PRL_ACQ_REC_PCS_CDMA_BLOCKS :  /* PCS CDMA using blocks */
         {
             PACK_ACQ_REG_FIELD( packByte, type.pcs_cdma_block.num_blocks, FSIZ(prl_pcs_cdma_using_blocks_type, num_blocks) )
                         
             for (blockIndex = 0; blockIndex < acqRecord->type.pcs_cdma_block.num_blocks; blockIndex++){
                 PACK_ACQ_REG_FIELD( packWord, type.pcs_cdma_block.block[blockIndex], FSIZ(prl_pcs_cdma_using_blocks_type, block) )
             } /*for*/
                 
             break;
         }

      case PRL_ACQ_REC_PCS_CDMA_CHANNELS :/* PCS CDMA using channels */
         {
            PACK_ACQ_REG_FIELD( packByte, type.pcs_cdma_chan.num_chans, FSIZ(prl_pcs_cdma_using_chans_type, num_chans) )
                        
            for (chanIndex = 0; chanIndex < acqRecord->type.pcs_cdma_chan.num_chans; chanIndex++)
            {
               PACK_ACQ_REG_FIELD( packWord, type.pcs_cdma_chan.chan[chanIndex], FSIZ(prl_pcs_cdma_using_chans_type, chan) )              
            }
            break;
         }

        
      default :
         {
            GSM1XACTVPRL_MSG_ERROR("Illegal acq_type %d", acqRecord->acq_type, 0, 0);
            return FALSE;
         }
   }

   *pNewOffsetBits = offsetBits;

   return TRUE;

}    


/*======================================================================
FUNCTION:   packSysRec

DESCRIPTION:
   Pack PRL system record to a specified memory location.
   
RETURN VALUE:
   boolean completion status (TRUE=success) 
SIDE EFFECTS:
   None
======================================================================*/
static boolean packSysRec
(  
   prli_sys_rec_type      *sysRecord,
   uint32                  bufMaxSizeBits,
   uint32                  bufOffsetBits,
   byte                   *buf, 
   uint32                 *pNewOffsetBits
)
{
  
  uint32 offsetBits = bufOffsetBits;

  /* Pack the fields of the system record one by one while incrementing
     the bit offset per each packed field.
  */  

#define PACK_SYS_REG_FIELD(__packFunc,__field,__size) \
  if( ! (__packFunc)(             \
            sysRecord-> __field , \
            buf,                  \
            &offsetBits,          \
            (__size),             \
            bufMaxSizeBits        \
        )                         \
    ){                            \
      return FALSE;               \
  }

  PACK_SYS_REG_FIELD( packWord, sid, FSIZ(prl_sys_record_type, sid) )
  PACK_SYS_REG_FIELD( packByte, nid_incl, FSIZ(prl_sys_record_type, nid_incl) )

  if(PRL_NID_INCL == sysRecord->nid_incl){
     PACK_SYS_REG_FIELD( packWord, nid, FSIZ(prl_sys_record_type, nid) )
  }

  PACK_SYS_REG_FIELD( packByte, neg_pref, FSIZ(prl_sys_record_type, neg_pref) )
  PACK_SYS_REG_FIELD( packByte, geo, FSIZ(prl_sys_record_type, geo) )
  

  if(PRL_PREFERRED == sysRecord->neg_pref){
     PACK_SYS_REG_FIELD( packByte, pri, FSIZ(prl_sys_record_type, pri) )
  }

  PACK_SYS_REG_FIELD( packByte, acq_index, FSIZ(prl_sys_record_type, acq_index) )  
  
  if(PRL_PREFERRED == sysRecord->neg_pref){
     PACK_SYS_REG_FIELD( packByte, roam_ind, FSIZ(prl_sys_record_type, roam_ind) )  
  }   

  *pNewOffsetBits = offsetBits;
  
  return TRUE;
}


/*======================================================================
FUNCTION:   packCRC16

DESCRIPTION:
   Pack crc16 to a specified memory location.
   
RETURN VALUE:
   boolean completion status (TRUE = success)
SIDE EFFECTS:
   None
======================================================================*/
static boolean packCRC16
(
   uint16       CRC16,
   uint32       bufMaxSizeBits,
   uint32       bufOffsetBits,
   byte        *buf, 
   uint32      *pNewOffsetBits
)
{
   uint32 offsetBits = bufOffsetBits;

   if( ! packWord
          (
             CRC16, 
             buf, 
             &offsetBits, 
             8*sizeof(CRC16), 
             bufMaxSizeBits
          )
      ){
        return FALSE;
   }

   *pNewOffsetBits = offsetBits;
  
   return TRUE;
}


/*======================================================================
FUNCTION:   packByte
DESCRIPTION:
   Pack the specified field into the specified array of bits.
RETURN VALUE:
   boolean completion status
SIDE EFFECTS:
   None
======================================================================*/
static boolean packByte
( 
   uint32   src, 
   byte    *dst, 
   uint32  *pOffsetBits, 
   uint32   size,
   uint32   bufMaxSizeBits
)
{
   if( ((*pOffsetBits) + size) > bufMaxSizeBits){
      GSM1XACTVPRL_MSG_ERROR("Could not pack acq record into a byte stream",0,0,0);
      return FALSE;
   }
             
   b_packb((byte)src, dst, (uint16)(*pOffsetBits), (uint16)size);
   (*pOffsetBits) += size;

   return TRUE;
}



/*======================================================================
FUNCTION:   packWord
DESCRIPTION:
   Pack the specified field into the specified array of bits.
RETURN VALUE:
   boolean completion status
SIDE EFFECTS:
   None
======================================================================*/
static boolean packWord
( 
   uint32   src, 
   byte    *dst, 
   uint32  *pOffsetBits, 
   uint32   size,
   uint32   bufMaxSizeBits
)
{
   if( ((*pOffsetBits) + size) > bufMaxSizeBits){
      GSM1XACTVPRL_MSG_ERROR("Could not pack acq record into a byte stream",0,0,0);
      return FALSE;
   }
             
   b_packw((uint16)src, dst, (uint16)(*pOffsetBits), (uint16)size);
   (*pOffsetBits) += size;

   return TRUE;
}



/*============================================================================

FUNCTION B_PACKB

DESCRIPTION
  Packs the given byte into the destination at the given offset for the
  given number of length bits

DEPENDENCIES
  None
   
RETURN VALUE
  None

SIDE EFFECTS
  None
   
============================================================================*/
static void b_packb(
   byte    src, 
   byte    dst[], 
   uint16  pos, 
   uint16  len 
)
{
   uint16   t_pos = pos % 8;
   uint16   bits  = 8 - t_pos;

   dst += (pos+len-1)/8;

   if ( bits >= len )
   {
       *dst &= (byte) ~MASK_B(t_pos, len);
       *dst |= (byte) (MASK_B(t_pos, len) & (src << (bits - len)));
   }
   else /* len > bits */
   {
       dst--;
       *dst &= (byte) ~MASK_B(t_pos, bits);
       *dst |= (byte) (MASK_B(t_pos, bits) & (src >> (len - bits)));

       dst++;
       *dst &= (byte) ~MASK_B(0, (len - bits));
       *dst |= (byte) (MASK_B(0, (len - bits)) & (src << (8 - (len - bits))));
   }
} /* END b_packb */


/*============================================================================

FUNCTION B_PACKW

DESCRIPTION
  Packs the given uint16 into the destination at the given offset for the
  given number of length bits

DEPENDENCIES
  None
   
RETURN VALUE
  None

SIDE EFFECTS
  None
   
============================================================================*/
static void b_packw(
   uint16  src, 
   byte    dst[], 
   uint16  pos, 
   uint16  len 
)
{
   int bits, start;
   byte   mask;
 
   dst += (len+pos-1)/8;        /* point to last byte to be written */
   pos  = (len+pos-1)%8;        /* index of last bit to be written */
   
   if (len > pos)  /* if we are filling all of the left part of the byte */
   {
     start = 0;  
   }
   else            /* There are going to be untouched bits at left of 
                   ** destination byte.                                   */
   {
     start = (pos+1) - len;     
   }
   bits = (pos - start) + 1;    /* # of bits to be written in this byte */
   
   *dst &= (byte) ~MASK_B(start,bits);  /* clear the bits to be written */
   
   *dst |= (byte) (   ( src << (7 - pos) )    /* left-shift src to line up */
                    & MASK_B(start, bits) );  /* only touch desired bits */
   
   dst--;                /* back up one byte */
   src >>= bits;         /* get rid of bits we've consumed already */
   
   if(len > bits)        /* if we need to write into other bytes */
   {
     len -= bits;        /* compute remaining length  */
     
     /* for full bytes, we can just overwrite the old value with the new */
     for ( ; len >= 8 ; len -= 8 ) {
       *dst = (byte)( src );
       dst--;                         /* back up one byte */
       src >>= 8;                     /* get rid of used bits */
     }
 
     if (len > 0)     /* if some bits are leftover... */
     {
       mask = (byte) (0xff << len);   
       *dst &= mask;                  /* clear bits on right side of byte */
       *dst |= ( (byte)( src ) & ~mask);        /* set appropriate bits */
     }
 
   }
} /* END b_packw */



/*===========================================================================

FUNCTION CRC_16_CALC

DESCRIPTION
  This function calculates a 16-bit CRC over a specified number of data
  bits.  It can be used to produce a CRC and to check a CRC.

DEPENDENCIES
  None

RETURN VALUE
  Returns a uint16 holding 16 bits which are the contents of the CRC
  register as calculated over the specified data bits.  If this
  function is being used to check a CRC, then the return value will be
  equal to CRC_16_OK (defined in crc.h) if the CRC checks correctly.

SIDE EFFECTS
  None

===========================================================================*/
static uint16 crc_16_calc
(
  byte *buf_ptr,
    /* Pointer to bytes containing the data to CRC.  The bit stream starts
    ** in the MS bit of the first byte.
    */

  uint16 len
    /* Number of data bits to calculate the CRC over */
)
{
  uint16 data, crc_16;

  
  /* Generate a CRC-16 by looking up the transformation in a table and
  ** XOR-ing it into the CRC, one byte at a time.
  */
  for (crc_16 = CRC_16_SEED ; len >= 8; len -= 8, buf_ptr++) {
    crc_16 = (uint16)(gsm1xactvprl_crc16_table[ (crc_16 >> (16 - 8)) ^ *buf_ptr ] ^ (crc_16 << 8));
  }

  /* Finish calculating the CRC over the trailing data bits
  **
  ** XOR the MS bit of data with the MS bit of the CRC.
  ** Shift the CRC and data left 1 bit.
  ** If the XOR result is 1, XOR the generating polynomial in with the CRC.
  */
  if (len != 0) {

    data = ((uint16) (*buf_ptr)) << (16-8); /* Align data MSB with CRC MSB */

    while (len-- != 0) {
      if ( ((crc_16 ^ data) & ((uint16) 1 << 15)) != 0 ){ /* Is MSB of XOR a 1 */

        crc_16 <<= 1;                   /* Left shift CRC          */
        crc_16 ^= CRC_16_POLYNOMIAL;    /* XOR polynomial into CRC */

      } else {

        crc_16 <<= 1;                   /* Left shift CRC          */

      }

      data <<= 1;                       /* Left shift data         */
    }
  }

  return( ~crc_16 );            /* return the 1's complement of the CRC */

} /* end of crc_16_calc */

