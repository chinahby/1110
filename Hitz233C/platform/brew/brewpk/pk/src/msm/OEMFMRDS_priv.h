#ifndef __OEMFMRDS_PRIV_H__
#define __OEMFMRDS_PRIV_H__
/*=============================================================================

FILE: OEMFMRDS_priv.h

SERVICES:  

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:


INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2007 QUALCOMM Incorporated.
                    All Rights Reserved.

                    QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================

  $Header: //depot/asic/qsc1100/platform/brew/brewpk/pk/src/msm/OEMFMRDS_priv.h#3 $
  $DateTime: 2008/11/25 04:06:35 $
  $Author: bhargavg $
  $Change: 792258 $
                        EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------

=============================================================================*/
//#define IFMRADIO_FEATURE_TUNER // Enable the tuner functionality for testing 
//#define FMRDS_ON_ULC 

//events in the call back
#define FMRDS_EVT_NOTIFY_RDSDATA 1

// This should only be used for function parameters, not for local variables
#define PARAM_NOT_REF(param)         /*lint -esym(715,param) */

#define AEEFMRDS_MSG_LOW(str,a,b,c)      if(fmrds_enable_dbgs) { DBGPRINTF_LOW(str,a,b,c);    }
#define AEEFMRDS_MSG_MEDIUM(str,a,b,c)   if(fmrds_enable_dbgs) { DBGPRINTF_MEDIUM(str,a,b,c); }
#define AEEFMRDS_MSG_HIGH(str,a,b,c)     if(fmrds_enable_dbgs) { DBGPRINTF_HIGH(str,a,b,c);   }
#define AEEFMRDS_MSG_ERROR(str,a,b,c)    if(fmrds_enable_dbgs) { DBGPRINTF_ERROR(str,a,b,c);  }
#define AEEFMRDS_MSG_FATAL(str,a,b,c)    if(fmrds_enable_dbgs) { DBGPRINTF_FATAL(str,a,b,c);  }

#define FMRDS_LAST_GROUPID       AEEFMRDS_INFO_15B

#ifndef BREAKIF
#define BREAKIF(x)   if(AEE_SUCCESS != (x)) {break;}
#endif /* BREAKIF */ 

#define OEMFMRDS_NR_BLOCKS_IN_GROUP 4

#define OEMFMRDS_GROUPID(block_b) (((block_b)&(0xF800))>>11)

#define OEMFMRDS_BLOCK_SIZE_IN_BYTES   2

#ifdef FMRDS_ON_ULC

//this value 50 is taken from drivers code,
//    #define MAX_BUFF_SIZE   100  ( from MAX_APP_RDS_BUFF_SZ in fmrds_si403.h )
//its the amount of blocks that drivers is going to store, before
//passing onto us
#define OEMFMRDS_MAX_BUF_SIZE 100
static AEEFMRDSGroup gRawDataBuffer[OEMFMRDS_MAX_BUF_SIZE];
static uint32 nGroupsRcvd;

#endif /* FMRDS_ON_ULC */

#define LF_START_FREQ            153  
#define LF_FREQ_SPACE            9
#define MF_START_FREQ            531
#define MF_FREQ_SPACE            9
#define VHF_START_FREQ           87600
#define VHF_FREQ_SPACE           100

//to calculate Gruop id
#define FMRDSPARSER_GROUP_ID_MASK		11
#define FMRDSPARSER_GROUP_ID(b)\
                     	            ((b >> FMRDSPARSER_GROUP_ID_MASK) & 0x1F)

//to calculate Program type code 
#define FMRDSPARSER_PTY_CODE_MASK		5
#define FMRDSPARSER_PTY_CODE(b)\
                                    ((b >> FMRDSPARSER_PTY_CODE_MASK) & (0x1F))

//to calculate Traffic program code 
#define FMRDSPARSER_TP_CODE_MASK		   10
#define FMRDSPARSER_TP_CODE(b)\
                              ((b >> FMRDSPARSER_TP_CODE_MASK) & (0x1)) ? 1 : 0

//to calculate music/speech code
#define FMRDSPARSER_MS_CODE_MASK		   3
#define FMRDSPARSER_MS_CODE(b)\
                              ((b & 0x8) >> FMRDSPARSER_MS_CODE_MASK) ? 1 : 0

//to calculate traffic anouncement code
#define FMRDSPARSER_TA_CODE_MASK		   4
#define FMRDSPARSER_TA_CODE(b)\
                              ((b >> FMRDSPARSER_TA_CODE_MASK) & (0x1)) ? 1 : 0

//to calculate PS DI addr. it tell where to place DI segment bit in DI.
#define FMRDSPARSER_PS_DIADDR_CODE(b)  (b & 0x03)

//to calculate DI segment
#define FMRDSPARSER_DI_SEGMENT_MASK    2
#define FMRDSPARSER_DI_SEGMENT(b)\
                     		   ((b >> FMRDSPARSER_DI_SEGMENT_MASK) & 0x1)

//to calculate text segment address and Text AB flag
#define FMRDSPARSER_TSADDR_CODE(b)	   (b & 0xF) 
#define FMRDSPARSER_TEXTABFLAG_CODE(b) ((b >> 4) & 0x1) ? 1 : 0

//Radio text
#define FMRDSPARSER_RT_CODE1(b)        ((b & 0xFF00) >> 8)
#define FMRDSPARSER_RT_CODE2(b)        (b & 0x00FF)


//to calculte Alternate frequency
#define FMRDSPARSER_AF_CODE1_MASK	   8
#define FMRDSPARSER_AF_CODE1(b)\
                  	         ((b & 0xFF00) >> FMRDSPARSER_AF_CODE1_MASK)
#define FMRDSPARSER_AF_CODE2(b)		   (int)(b & 0xFF)


//to calcuate program servcie
#define FMRDSPARSER_PS_CODE1_MASK	   8
#define FMRDSPARSER_PS_CODE1(b)\
                  		      ((b & 0xFF00) >> FMRDSPARSER_AF_CODE1_MASK)
#define FMRDSPARSER_PS_CODE2(b)		   (b & 0xFF)

//Application group type code
#define FMRDSPARSER_AGT_CODE(b)        (b & 0x1F)

//Modified julian day code.
#define FMRDSPARSER_MJD_BLOCKB_CODE(b) (b & 0x03)
#define FMRDSPARSER_MJD_CODE(b,c)      (((uint32)(b >> 1)) | (c << 15))


//loacl hour code(UTC hour)
#define FMRDSPARSER_HOUR_CODE_MASK     12
#define FMRDSPARSER_HOUR_CODE(b, c)\
            c? (((b & 0xF000) >> FMRDSPARSER_HOUR_CODE_MASK) | 0x10) :\
                        ((b & 0xF000) >> FMRDSPARSER_HOUR_CODE_MASK)


//local minute code(UTC minute)
#define FMRDSPARSER_MINUTE_CODE_MASK   6
#define FMRDSPARSER_MINUTE_CODE(b)\
                              (b >> FMRDSPARSER_MINUTE_CODE_MASK) & (0x3F)

//local offset
#define FMRDSPARSER_LOFFSET_CODE(b)    (b & 0x3F)

//Extended country Code
#define FMRDSPARSER_VARIANT_CODE(b)    ((b & 0x7000) >> 12)
#define FMRDSPARSER_EC_CODE(b)         (b & 0xFF)

//PIN  code
#define FMRDSPARSER_PINDAY_CODE(b)     ((b >> 11) & 0x1F)
#define FMRDSPARSER_PINHOUR_CODE(b)    ((b >> 6) & 0x1F)
#define FMRDSPARSER_PINMINUTE_CODE(b)  (b & 0x3F)

// Program type name
#define FMRDSPARSER_PTYN_ADDRESS_CODE(b) (b & 0x1)
#define FMRDSPARSER_PTYN_CODE1(b)\
                              ((b & 0xFF00) >> 8)
#define FMRDSPARSER_PTYN_CODE2(b)      (b & 0xFF)

// PS Segment address
#define FMRDSPARSER_PSSEGMENTADR_CODE(b) (b & 0x1)

#endif /* __OEMFMRDS_PRIV_H__ */

