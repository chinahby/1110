#ifndef DEC5000_H
#define DEC5000_H

#include "comdef.h"
/*===========================================================================

           D E C O D E R   ( S V D )    H E A D E R    F I L E

DESCRIPTION
  All the declarations and definitions necessary to support interaction
  with the QUALCOMM CDMA Serial Viterbi Decoder chip

REFERENCES
  CDMA Mobile Station ASIC Specification Sheet

Copyright (c) 1990 through 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: O:/src/asw/MSM5100/vcs/dec5000.h_v   1.23   23 Oct 2002 14:25:42   vlechan  $
$Header: //source/qcom/qct/modem/1x/drivers/rel/1h08/inc/dec5000.h#1 $ $DateTime: 2009/02/13 14:29:02 $ $Author: joshuam $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/24/08   jtm     Modified DEC_DCCH_RC_INDEX to perform bounds checking.
07/16/08   jtm     Removed FEATURE_RXCCHAIN_DEBUG. Modified DEC_FCH_RC_INDEX 
                   and DEC_SCH_RC_INDEX to perform bounds checking.
07/01/08   vlc     Merged code changes from old tip archive.
04/10/08   trc     Switch clock mgmt strategy via FEATURE_SRCH_CLKREGIM_RM
03/05/08   vlc     User clock resource manager to manage RX clocks 
                   (FEATURE_CLKREGIM_RM.)
01/03/05   rkc     Added DEC_FCCCH_RAM_BASE.  Decoder Output Buffer offset for FCCCH
                   channel data. Value is offset in the Short RAM in Decoder Output
                   Buffer.
04/21/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.
05/06/04   sr      Moved out DV driver code to separate file
09/03/03   dna     Ported for MSM6500
08/08/03   vlc     Added changes to move output buffer address of DCCH task to 
                   decouple FCH/DCCH rate sets.
07/08/03   vlc     Added DEC_SCH_DUMMY_FULL_OB_ADDR.  This address is used
                   for Turbo SCH dummy task and is beyong any critical 
                   decoder output buffer adresses.
01/21/03   vlc     Put DEC_TURBO_SCH_TASK define under release A feature for
                   clarity sake.
11/26/02   sr/vlc  Defined DEC_TURBO_SCH_TASK for MSM6100.
10/23/02   vlc     Increased NUM_TD_SCH_RATES to account for Turbo 32X rate 
                   (applies to MSM6100 only.)
07/23/02   dlb2    Add EXTENDED_ENERGY_MATRIX support to CDMA2000 Rel. A code.
06/07/02   dlb2    Always use mask macros to write to VD_MODE_WB.
05/30/02   vlc     VP2 changes are now featurized under FEATURE_IS2000_REL_A_SVD
                   and FEATURE_IS2000_P2 per request of development team.
05/21/02   vlc     Remove rate from parameter list to dec_read_dcch_data ().
05/13/02   vlc     Added support for simultaneous DCCH & FCH channel set up 
                   (feature FEATURE_IS2000_REL_A_VP2.)
03/27/02   ems     Featurized dec_read_scch_data for MSMHW_EXTENDED_ENERGY_MATRIX
02/25/02   sr      Merged in Release A common channels from the branch
01/23/02   dlb     Added extended energy matrix feature for MSM6050.
01/22/02   vlc     Added rate_set field to dec_sch_cfg_type, consolidated 
                   dec_sch_cfg_type.
11/16/01   vlc     Added dec_clk_select_type and prototype for function
                   dec_clk_select ().
10/05/01   sr      Changed dec_fch_status_type's ser field from byte to word 
08/08/01   vlc     Changed #include msmhw.h to #include msm.h to stay forward
                   compatible with future MSMs.
08/07/01   hrk     Modified prototype of dec_dtx_thresh_init().
07/03/01   baon    Added and cleaned up support for REALEASE_A / 32X
02/20/01   sr      Added support for 32X
01/10/01   tc      Added decoder CRC and quality bits.
12/13/00   vlc     Added #include msmhw.h to enable MSMHW_TURBO_CODING.
10/20/00   rmd     Merged with MSM5000 code. Moved bits to msm5XXXbits.h
09/06/00   rm      Added new function prototype of dec_read_sch_data
                   for Factory Test Mode.
09/06/00   djd/et  Added ifdef MSMHW_TURBO_CODING for turbo coding software.
07/12/00   rm      Added suuport for for 5ms frames and MULTIFRAME
                   INTERLEAVING. 
05/18/00   et      added prototype for dec_dtx_thresh_init
03/24/00   bgc     Changes energy's to signed values.
02/16/00   eh      Changed DEC_MAX_SCC_NUM to 0x07  
01/28/00   rm      Added support for 1X SCH.
01/26/00   rm      Modified the dec_read_sch_data fumction to read
                   each mux pdu into the buffer pointed by the dsm_item.
11/09/99   rm      Added in support for Turbo Codes.
10/20/99   rm      Added in support for IS95B SCCH.
10/01/99   rm      Externalized dec_sch_ob_params parameter.
09/24/99   rm      Added in support for SCH (RC3 & RC4).
07/31/99   rm      Created file. Interfaces to support Serial Viterbi Decoder

============================================================================*/
#include "dsm.h"
#include "msm.h"        

#define DEC_FCH_TASK        0x03  /* Decoder Task ID for FCH           */
#define DEC_SCCH_SCH_TASK   0x10  /* Decoder Task ID for SCH/SCCH      */
#ifdef FEATURE_IS2000_REL_A
#define DEC_TURBO_SCH_TASK  0x20  /* Turbo Decoder Task ID for SCH    */
#endif /* FEATURE_IS2000_REL_A */
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
#define DEC_DCCH_TASK       0x04  /* Decoder Task ID for DCCH          */
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */


/************************************************************************
                              COMMON CONSTANTS
************************************************************************/

#define VD_INITMODE_NO_INIT_V        (0x00 << 1)
#define VD_INITMODE_SATURATE_STATE_V (0x01 << 1)
#define VD_CONTINUOUS_V 0x00
/* Decoder in Continous Mode fpr SYNC and Paging Channel */
#define VD_PACKET_V 0x01
/* Decoder in Packet Mode for Traffic Channel */


#define DEC_192_BITS      192     /* Bits per  9600 bps rate 1   frame */
#define DEC_96_BITS        96     /* Bits per  9600 bps rate 1/2 frame */
#define DEC_48_BITS        48     /* Bits per  9600 bps rate 1/4 frame */
#define DEC_24_BITS        24     /* Bits per  9600 bps rate 1/8 frame */

#define DEC_54_BITS       54     /* For RC3 and RC4 9600 bps rate 1/4 & 1/8 
                                     have 6 bits CRC */
#define DEC_30_BITS       30  

#define DEC_288_BITS      288     /* Bits per 14400 bps rate 1   frame */
#define DEC_144_BITS      144     /* Bits per 14400 bps rate 1/2 frame */
#define DEC_72_BITS        72     /* Bits per 14400 bps rate 1/4 frame */
#define DEC_36_BITS        36     /* Bits per 14400 bps rate 1/8 frame */

/* CRC + Quality Bit Indicator */
#define DEC_CRC8_M        0x01    /* 1 if CRC ok for  1/8 rate frame   */
#define DEC_CRC4_M        0x01    /* 1 if CRC ok for  1/4 rate frame   */
#define DEC_CRC2_M        0x01    /* 1 if CRC ok for  1/2 rate frame   */
#define DEC_CRC1_M        0x01    /* 1 if CRC ok for full rate frame   */

#define DEC_GOOD8_M       0x02    /* ACS Quality bit  1/8 rate frame   */
#define DEC_GOOD4_M       0x02    /* ACS Quality bit  1/4 rate frame   */
#define DEC_GOOD2_M       0x02    /* ACS Quality bit  1/2 rate frame   */
#define DEC_GOOD1_M       0x02    /* ACS Quality bit full rate frame   */                                               

/*****************************************************************************
                              SYNC CHANNEL 
*****************************************************************************/

/* SYNC Channel Data Buffer size */
#define DEC_SC_BUF_SIZ    (DEC_96_BITS / 8)    

/* SYNC Channel Data Buffer Type */
typedef byte dec_sc_type[ DEC_SC_BUF_SIZ ];

/* Status information for Sync chanel */
typedef struct {             
 byte status;                
 word ser;                   /* Symbol Error Rate */
 int4 energy;                /* Frame Energy based on SER Calculations */
} dec_sc_status_type;   /* For Sync Channel */

/*****************************************************************************
                              PAGING CHANNEL 
*****************************************************************************/


/* Paging Channel data buffer type, the rate is set dynamically.
** The buffer is sized for a 192 bit (1:1) frame.
*/
#define DEC_PC_BUF_SIZ    (DEC_192_BITS / 8)

typedef enum {
  DEC_PRAT_9600=0,   /* Paging Channel Rate 9600 bps */
  DEC_PRAT_4800      /* paging Channel Rate 4800 bps */
} dec_pc_rate_type;

/* Paging Channel Buffer */
typedef byte dec_pc_type[ DEC_PC_BUF_SIZ ];

/* Status information for Paging Channel */
typedef struct {
 byte status;
 word ser;                   /* Symbol Error Count */
 int4 energy;                /* Frame Energy based on SER calculations */
} dec_pc_status_type;   /*  Paging Channel */

typedef enum {
  DEC_20MS_FRAME=0,
  DEC_40MS_FRAME,
  DEC_80MS_FRAME
} dec_frame_type;

/*****************************************************************************
                   Generic Access Methods
*****************************************************************************/
/* Decoder frame status information */
typedef struct {
  boolean crc_passed;  /* TRUE is CRC pass */
  boolean quality_ind; /* TRUE if frame quality indicator is set */

  word sec;            /* Symbol Error Count */
  int4 energy;         /* Frame Energy based on SER calculations */
} dec_frame_status_type;

/*===========================================================================

FUNCTION DEC_READ_DATA

DESCRIPTION
  Reads the decoder output buffer starting at 'offset'.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean indicating success.

SIDE EFFECTS
  None

===========================================================================*/
boolean
dec_read_data
(
  unsigned int ram_offset,
  void *buf_ptr,
  unsigned int num_bytes  /* # of bytes to be read. */
);

/*****************************************************************************
                     MSM REGISTER I/O WRAPPERS 
        (for ease of porting and to avoid #ifdefs in the code)
*****************************************************************************/

#define DECOB_SHORT_BUFFER_ADDR_SET( address ) \
      HWIO_OUT( DECOB_SHORT_BUFFER_ADDR, (address) )
#define DECOB_LONG_BUFFER_ADDR_SET( address ) \
      HWIO_OUT( DECOB_LONG_BUFFER_ADDR, (address) )

#define DECOB_LONG_BUFFER_DATA_GET( data_ptr, num_words) \
      insw_rep( (void *) HWIO_ADDR(DECOB_LONG_BUFFER_DATA), (data_ptr), (num_words) )

/*****************************************************************************
                        FUNDAMENTAL CHANNEL 
*****************************************************************************/

/* RC1 FCH Size for different rates */
#define DEC_FCH_SIZ_FULL_RC1 ((DEC_192_BITS + 7) / 8)
#define DEC_FCH_SIZ_HALF_RC1 ((DEC_96_BITS  + 7) / 8)
#define DEC_FCH_SIZ_QTR_RC1 ((DEC_48_BITS  + 7) / 8) 
#define DEC_FCH_SIZ_8TH_RC1 ((DEC_24_BITS +7) / 8) + 1

#define DEC_FCH_SIZ_ALL_RC1   \
  ( DEC_FCH_SIZ_FULL_RC1 + DEC_FCH_SIZ_HALF_RC1 \
  + DEC_FCH_SIZ_QTR_RC1  + DEC_FCH_SIZ_8TH_RC1 )


/* RC2 FCH Size for different rates */
#define DEC_FCH_SIZ_FULL_RC2 ((DEC_288_BITS + 7) / 8)
#define DEC_FCH_SIZ_HALF_RC2 ((DEC_144_BITS  + 7) / 8)
#define DEC_FCH_SIZ_QTR_RC2 ((DEC_72_BITS  + 7) / 8) + 1
#define DEC_FCH_SIZ_8TH_RC2 ((DEC_36_BITS +7) / 8) + 1

#define DEC_FCH_SIZ_ALL_RC2   \
  ( DEC_FCH_SIZ_FULL_RC2 + DEC_FCH_SIZ_HALF_RC2 \
  + DEC_FCH_SIZ_QTR_RC2  + DEC_FCH_SIZ_8TH_RC2 )


/* RC3 FCH Size for different rates */
#define DEC_FCH_SIZ_FULL_RC3 ((DEC_192_BITS + 7) / 8)
#define DEC_FCH_SIZ_HALF_RC3 ((DEC_96_BITS  + 7) / 8)
#define DEC_FCH_SIZ_QTR_RC3 ((DEC_54_BITS  + 7) / 8) + 1
#define DEC_FCH_SIZ_8TH_RC3 ((DEC_30_BITS +7) / 8) 

#define DEC_FCH_SIZ_ALL_RC3   \
  ( DEC_FCH_SIZ_FULL_RC3 + DEC_FCH_SIZ_HALF_RC3 \
  + DEC_FCH_SIZ_QTR_RC3  + DEC_FCH_SIZ_8TH_RC3 )

/* RC4 FCH Size for different rates */
#define DEC_FCH_SIZ_FULL_RC4 ((DEC_192_BITS + 7) / 8)
#define DEC_FCH_SIZ_HALF_RC4 ((DEC_96_BITS  + 7) / 8)
#define DEC_FCH_SIZ_QTR_RC4 ((DEC_54_BITS  + 7) / 8) + 1
#define DEC_FCH_SIZ_8TH_RC4 ((DEC_30_BITS +7) / 8)

#define DEC_FCH_SIZ_ALL_RC4   \
  ( DEC_FCH_SIZ_FULL_RC4 + DEC_FCH_SIZ_HALF_RC4 \
  + DEC_FCH_SIZ_QTR_RC4  + DEC_FCH_SIZ_8TH_RC4 )


/* RC5 FCH Size for different rates */
#define DEC_FCH_SIZ_FULL_RC5 ((DEC_288_BITS + 7) / 8)
#define DEC_FCH_SIZ_HALF_RC5 ((DEC_144_BITS  + 7) / 8)
#define DEC_FCH_SIZ_QTR_RC5 ((DEC_72_BITS  + 7) / 8) + 1
#define DEC_FCH_SIZ_8TH_RC5 ((DEC_36_BITS +7) / 8) + 1

#define DEC_FCH_SIZ_ALL_RC5   \
  ( DEC_FCH_SIZ_FULL_RC5 + DEC_FCH_SIZ_HALF_RC5 \
  + DEC_FCH_SIZ_QTR_RC5 + DEC_FCH_SIZ_8TH_RC5 )

/* Decoder Output Buffer offset for FCH RC1 for different rates 
 (Full, Half, Quarter & Eighth ) */
 
#define DEC_FCH_FULL_OB_ADDR_RC1   0
#define DEC_FCH_HALF_OB_ADDR_RC1 \
  (DEC_FCH_FULL_OB_ADDR_RC1 + DEC_FCH_SIZ_FULL_RC1 + DEC_FCH_STATUS_SIZE)
#define DEC_FCH_QTR_OB_ADDR_RC1 \
  (DEC_FCH_HALF_OB_ADDR_RC1 + DEC_FCH_SIZ_HALF_RC1 + DEC_FCH_STATUS_SIZE)
#define DEC_FCH_8TH_OB_ADDR_RC1 \
  (DEC_FCH_QTR_OB_ADDR_RC1 + DEC_FCH_SIZ_QTR_RC1 + DEC_FCH_STATUS_SIZE)

/* Decoder Output Buffer offset for FCH RC2 for different rates 
 (Full, Half, Quarter & Eighth ) */

#define DEC_FCH_FULL_OB_ADDR_RC2   0
#define DEC_FCH_HALF_OB_ADDR_RC2 \
  (DEC_FCH_FULL_OB_ADDR_RC2 + DEC_FCH_SIZ_FULL_RC2 + DEC_FCH_STATUS_SIZE)
#define DEC_FCH_QTR_OB_ADDR_RC2 \
  (DEC_FCH_HALF_OB_ADDR_RC2 + DEC_FCH_SIZ_HALF_RC2 + DEC_FCH_STATUS_SIZE)
#define DEC_FCH_8TH_OB_ADDR_RC2 \
  (DEC_FCH_QTR_OB_ADDR_RC2 + DEC_FCH_SIZ_QTR_RC2 + DEC_FCH_STATUS_SIZE)


/* Decoder Output Buffer offset for FCH RC3 for different rates 
 (Full, Half, Quarter & Eighth ) */

#define DEC_FCH_FULL_OB_ADDR_RC3   0
#define DEC_FCH_HALF_OB_ADDR_RC3 \
  (DEC_FCH_FULL_OB_ADDR_RC3 + DEC_FCH_SIZ_FULL_RC3 + DEC_FCH_STATUS_SIZE)
#define DEC_FCH_QTR_OB_ADDR_RC3 \
  (DEC_FCH_HALF_OB_ADDR_RC3 + DEC_FCH_SIZ_HALF_RC3 + DEC_FCH_STATUS_SIZE)
#define DEC_FCH_8TH_OB_ADDR_RC3 \
  (DEC_FCH_QTR_OB_ADDR_RC3 + DEC_FCH_SIZ_QTR_RC3 + DEC_FCH_STATUS_SIZE)
  
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
/* Decoder Output Buffer offset for full rate DCCH RC3 */
#define DEC_DCCH_FULL_OB_ADDR_RC3 \
  (DEC_FCH_8TH_OB_ADDR_RC3 + DEC_FCH_SIZ_8TH_RC3 + DEC_FCH_STATUS_SIZE)
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */
                        

/* Decoder Output Buffer offset for FCH RC4 for different rates 
 (Full, Half, Quarter & Eighth ) */

#define DEC_FCH_FULL_OB_ADDR_RC4   0
#define DEC_FCH_HALF_OB_ADDR_RC4 \
  (DEC_FCH_FULL_OB_ADDR_RC4 + DEC_FCH_SIZ_FULL_RC4 + DEC_FCH_STATUS_SIZE)
#define DEC_FCH_QTR_OB_ADDR_RC4 \
  (DEC_FCH_HALF_OB_ADDR_RC4 + DEC_FCH_SIZ_HALF_RC4 + DEC_FCH_STATUS_SIZE)
#define DEC_FCH_8TH_OB_ADDR_RC4 \
  (DEC_FCH_QTR_OB_ADDR_RC4 + DEC_FCH_SIZ_QTR_RC4 + DEC_FCH_STATUS_SIZE)
  
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
/* Decoder Output Buffer offset for full rate DCCH RC4 */
#define DEC_DCCH_FULL_OB_ADDR_RC4 \
  (DEC_FCH_8TH_OB_ADDR_RC4 + DEC_FCH_SIZ_8TH_RC4 + DEC_FCH_STATUS_SIZE)
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */


/* Decoder Output Buffer offset for FCH RC5 for different rates 
 (Full, Half, Quarter & Eighth ) */

#define DEC_FCH_FULL_OB_ADDR_RC5   0
#define DEC_FCH_HALF_OB_ADDR_RC5 \
  (DEC_FCH_FULL_OB_ADDR_RC5 + DEC_FCH_SIZ_FULL_RC5 + DEC_FCH_STATUS_SIZE)
#define DEC_FCH_QTR_OB_ADDR_RC5 \
  (DEC_FCH_HALF_OB_ADDR_RC5 + DEC_FCH_SIZ_HALF_RC5 + DEC_FCH_STATUS_SIZE)
#define DEC_FCH_8TH_OB_ADDR_RC5 \
  (DEC_FCH_QTR_OB_ADDR_RC5 + DEC_FCH_SIZ_QTR_RC5 + DEC_FCH_STATUS_SIZE)

#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
/* Decoder Output Buffer offset for full rate DCCH RC5 */
#define DEC_DCCH_FULL_OB_ADDR_RC5 \
  (DEC_FCH_8TH_OB_ADDR_RC5 + DEC_FCH_SIZ_8TH_RC5 + DEC_FCH_STATUS_SIZE)
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */


#define DEC_FCH_MAX_SIZ_FULL  DEC_FCH_SIZ_FULL_RC2  
#define DEC_FCH_MAX_SIZ_HALF  DEC_FCH_SIZ_HALF_RC2
#define DEC_FCH_MAX_SIZ_QTR  DEC_FCH_SIZ_QTR_RC2
#define DEC_FCH_MAX_SIZ_8TH  DEC_FCH_SIZ_8TH_RC2

#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
/* Make sure to include DCCH full rate (same as FCH full rate.) */
#define DEC_FCH_MAX_SIZ_ALL  (DEC_FCH_SIZ_ALL_RC2 + DEC_FCH_SIZ_FULL_RC2)
#else
#define DEC_FCH_MAX_SIZ_ALL  DEC_FCH_SIZ_ALL_RC2
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */


#ifdef MSMHW_TURBO_CODING
/* Decoder Output Buffer offset for Turbo SCH dummy task is beyong critical
** addresses in the QS buffer.  This is throw away data.  QL buffer starts at
** DEC_SCH_RAM_BASE, currently set at 160.
*/
#define DEC_SCH_DUMMY_FULL_OB_ADDR 100
#endif /* MSMHW_TURBO_CODING */

/* Size of status area in decoder output buffer in bytes */
#define DEC_FCH_STATUS_SIZE   6

#ifdef FEATURE_IS2000_REL_A_CC
/******************************************************************************
                        BCCH
******************************************************************************/

#define DEC_BCCH_SIZ (768/8)
#define DEC_BCCH_BUF_SIZ DEC_BCCH_SIZ

typedef byte dec_bcch_buf_type[ DEC_BCCH_BUF_SIZ ];

/******************************************************************************
                        CCCH
******************************************************************************/

#define DEC_CCCH_1X_SIZ (192/8)
#define DEC_CCCH_2X_SIZ (2*DEC_CCCH_1X_SIZ)
#define DEC_CCCH_4X_SIZ (4*DEC_CCCH_1X_SIZ)
#define DEC_CCCH_BUF_SIZ DEC_CCCH_4X_SIZ

typedef byte dec_ccch_buf_type[ DEC_CCCH_BUF_SIZ ];
#endif /* FEATURE_IS2000_REL_A_CC */

/******************************************************************************
                        Supplemental Channel 
******************************************************************************/

/* Buffer Size (in bytes) for IS95C  RC3 SCH 1X (9.6 kbps) */
#define DEC_SCH_SIZ_1X_RC3  (192 / 8)
/* Buffer Size (in bytes) for IS95C  RC3 SCH 2X (19.2 kbps) */
#define DEC_SCH_SIZ_2X_RC3  (384 / 8)
/* Buffer Size (in bytes) for IS95C  RC3 SCH 4X (38.4 kbps) */
#define DEC_SCH_SIZ_4X_RC3  (768 / 8)
/* Buffer Size (in bytes) for IS95C  RC3 SCH 8X (76.8 kbps) */
#define DEC_SCH_SIZ_8X_RC3  (1536 / 8)
/* Buffer Size (in bytes) for IS95C  RC3 SCH 16X (153.6 kbps) */
#define DEC_SCH_SIZ_16X_RC3  (3072 / 8)

/* Buffer Size (in bytes) for IS95C  RC4 SCH 1X (9.6 kbps) */
#define DEC_SCH_SIZ_1X_RC4  (192 / 8)
/* Buffer Size (in bytes) for IS95C  RC4 SCH 2X (19.2 kbps) */
#define DEC_SCH_SIZ_2X_RC4  (384 / 8)
/* Buffer Size (in bytes) for IS95C  RC4 SCH 4X (38.4 kbps) */
#define DEC_SCH_SIZ_4X_RC4  (768 / 8)
/* Buffer Size (in bytes) for IS95C  RC4 SCH 8X (76.8 kbps) */
#define DEC_SCH_SIZ_8X_RC4  (1536 / 8)
/* Buffer Size (in bytes) for IS95C  RC4 SCH 16X (153.6 kbps) */
#define DEC_SCH_SIZ_16X_RC4  (3072 / 8)

#ifdef FEATURE_32X_SCH_DATA_RATE
/* Buffer Size (in bytes) for IS95C  RC4 SCH 32X (153.6 kbps) */
#define DEC_SCH_SIZ_32X_RC4  (6144 / 8)
#endif /* FEATURE_32X_SCH_DATA_RATE */ 

/* Buffer Size (in bytes) for IS95C  RC5 SCH 1X (14.4 kbps) */
#define DEC_SCH_SIZ_1X_RC5  (288 / 8)
/* Buffer Size (in bytes) for IS95C  RC5 SCH 2X (28.8 kbps) */
#define DEC_SCH_SIZ_2X_RC5  (576 / 8)
/* Buffer Size (in bytes) for IS95C  RC5 SCH 4X (57.6 kbps) */
#define DEC_SCH_SIZ_4X_RC5  (1152 / 8)
/* Buffer Size (in bytes) for IS95C  RC5 SCH 8X (115.2 kbps) */
#define DEC_SCH_SIZ_8X_RC5  (2304 / 8)
/* Buffer Size (in bytes) for IS95C  RC5 SCH 16X (230.4 kbps) */
#define DEC_SCH_SIZ_16X_RC5  (4608 / 8)

/* Frame Quality Indicator length in bits for 1X (RS 1 & 2) */
#define DEC_SCH_SIZ_1X_CRC  12
/* Frame Quality Indicator length in bits for 2X (RS 1 & 2) */
#define DEC_SCH_SIZ_2X_CRC  16
/* Length of encoder tail bits */
#define DEC_SCH_SIZ_TAIL     8


#define DEC_SCCH_RC1_PAYLOAD_SIZE 11   
/* Payload for SCCH RC1 is (192 - 12 -8 -1) = 171 bits. 12 bits for Frame CRC,
   8 tail bits and 1 bit for MM Indicator . Note Paylod is in words */

#define DEC_SCCH_RC2_PAYLOAD_SIZE 17
/* Payload for SCCH RC2 is (288 - 12 -8 -2) = 171 bits. 12 bits for Frame CRC,
   8 tail bits and 2 bits for MM  & EIB Indicator . Note Paylod is in words */


#define DEC_SCC_SIZE_9600          (DEC_192_BITS / 16)  
/* Bits is RC1 channel */
#define DEC_SCC_SIZE_14400         (DEC_288_BITS / 16)
/* Bits is RC2 channel */

#define DEC_MAX_SCC_NUM    0x07  
/* Maximum Number of SCCh supported */
       
/* Decoder Output Buffer offset for SCH and SCCH channel data. 128 (or 160)
   points to the Large RAM in Decoder Output Buffer */
#define DEC_SCH_RAM_BASE 160

/* Decoder Output Buffer offset for FCCCH channel data. 80
   points to the Short RAM in Decoder Output Buffer */
#define DEC_FCCCH_RAM_BASE 80

/* Decoder Output Buffer offset for 5ms frames (FCH) */

#define DEC_5MS_PHASE1_RAM_OFFSET  0x0055
#define DEC_5MS_PHASE2_RAM_OFFSET  0x005A
#define DEC_5MS_PHASE3_RAM_OFFSET  0x005F
#define DEC_5MS_PHASE4_RAM_OFFSET  0x0050

/* HACK FOR SNM */
typedef enum {
  DEC_RATE_9600    = 0x00, /* Select 9600 bps rate set   */
  DEC_RATE_14400   = 0x01, /* Select 14400 bps rate set  */
  DEC_RATE_INVALID = 0x02  /* Invalid rate set           */
} dec_rate_set_type;


#define DEC_FCH_RC_BASE 0x01

#define DEC_FCH_RC_INDEX(rc)\
    ((((rc) - DEC_FCH_RC_BASE) > 0) ? ((rc) - DEC_FCH_RC_BASE) : 0)

typedef enum {
   DEC_RC1=0x01,
   DEC_RC2,
   DEC_RC3,
   DEC_RC4,
   DEC_RC5
} dec_rc_type;

#define DEC_SCH_RC_BASE 0x03  /* IS95C SCH begins from RC3 */

#define DEC_SCH_RC_INDEX(rc)\
    ((((rc) - DEC_SCH_RC_BASE) > 0) ? ((rc) - DEC_SCH_RC_BASE): 0)
    


/* Decode Rates to read
*/
typedef enum {
  DEC_RATE_ALL = 0,     /* Return all data for decode at all data rates */
  DEC_RATE_FULL,        /* Return data for "full" rate only             */
  DEC_RATE_HALF,        /* Return data for "half" rate only             */
  DEC_RATE_QTR,         /* Return data for "quarter" rate only          */
  DEC_RATE_8TH         /* Return data for "eighth" rate only             */
} dec_fch_rate_type;

/*  Decoder Supplemental Channel Rate */
typedef enum {
  DEC_SCH_RATE_1X,
  DEC_SCH_RATE_2X,
  DEC_SCH_RATE_4X,
  DEC_SCH_RATE_8X,
  DEC_SCH_RATE_16X
#ifdef FEATURE_32X_SCH_DATA_RATE
  ,DEC_SCH_RATE_32X
#endif /* FEATURE_32X_SCH_DATA_RATE */ 

} dec_sch_rate_type;

/* Decoding Type : Convolutional /  TURBO */
typedef enum {
  DEC_CONVOLUTIONAL = 0,
  DEC_TURBO
} dec_coding_type;

#ifndef FEATURE_SRCH_CLKREGIM_RM
/* Decoder clock select:  TCXO (traffic) / Chipx8 (idle) */
typedef enum {
  DEC_CLK_NONE = 0,
  DEC_CLK_TCXO,
  DEC_CLK_CHIPX8

} dec_clk_select_type;
#endif /* !FEATURE_SRCH_CLKREGIM_RM */

/* Fundamental Channel buffer type */
typedef union
{
  struct 
  {
    byte bits1[DEC_FCH_MAX_SIZ_FULL];
    byte bits2[DEC_FCH_MAX_SIZ_HALF];
    byte bits4[DEC_FCH_MAX_SIZ_QTR];
    byte bits8[DEC_FCH_MAX_SIZ_8TH];
#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
    byte bits1dcch[DEC_FCH_MAX_SIZ_FULL];
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */
  } buf;
  byte bits[DEC_FCH_MAX_SIZ_ALL];
} dec_fch_type;

typedef dec_fch_type dec_tc_type;

/* The following structure stores the Output Buffer address for a given RC for 
   all rates */
typedef struct
{
  byte dec_ob_addr_full_rate;
  byte dec_ob_addr_half_rate;
  byte dec_ob_addr_qtr_rate;
  byte dec_ob_addr_8th_rate;
  byte dec_full_rate_siz;
  byte dec_half_rate_siz;
  byte dec_qtr_rate_siz;
  byte dec_8th_rate_siz;
} dec_fch_ob_params_type;


/* Buffer for holding Symbol Error Rates, used for rate determination.
*/
typedef struct {
  byte status[4];            /* CRC and Quality bits for all frame sizes  */
  word ser[4];              /* SER for decode as a full rate frame       */
  int4 energy[4]; 
} dec_fch_status_type;

#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
/* Decoder Output Buffer base for DCCH */
#define DEC_DCCH_FULL_OB_ADDR \
  (DEC_FCH_8TH_OB_ADDR_RC5 + DEC_FCH_SIZ_8TH_RC5 + DEC_FCH_STATUS_SIZE)

/* Index to Output Buffer size table based on rc */
#define DEC_DCCH_RC_BASE        0x03    /* DCCH begins from RC3 */
#define DEC_DCCH_RC_INDEX(rc)\
    ((((rc) - DEC_DCCH_RC_BASE) > 0) ? ((rc) - DEC_DCCH_RC_BASE): 0)
    
/* Decoder Output Buffer size for DCCH for different RCs */
typedef word dec_dcch_ob_params_type;
#define DEC_DCCH_SIZ_FULL_RC3         (192/8)
#define DEC_DCCH_SIZ_FULL_RC4         (192/8)
#define DEC_DCCH_SIZ_FULL_RC5         (288/8)

/* DCCH status read from Decoder */
typedef struct {
  byte status;
  word ser;
  int4 energy;

} dec_dcch_status_type;
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */

/* Supplemental Channel (SCH) Buffer Type. In a RC5 frame, the largest amount of bits is
equal to 230,400 (bits/sec)/50 = 4608, where 50 = 1 sec / 20 msec (20 ms frame) */
// In a RC4 frame, the largest amount of bits is equal to 307,200 (bits/sec)/50 = 6144;
// where 50 = 1 sec / 20 msec (20 ms frame).
//typedef word dec_sch_type[4608/16];
typedef word dec_sch_type[6144/16];

/* Buffer for holding SCH Outut Buffer addresses */
typedef word dec_sch_ob_params_type;

#define NUM_SCH_RC 3       /* for SCH , RC3, RC4 and RC5 are supported */

#ifdef FEATURE_32X_SCH_DATA_RATE
#define NUM_SCH_RATES  6  /* Rates Supported are 1X, 2X, 4X, 8X, 16X and 32X */
#else
#define NUM_SCH_RATES  5  /* Rates Supported are 1X, 2X, 4X, 8X, 16X */
#endif /* FEATURE_32X_SCH_DATA_RATE */ 

#ifdef MSMHW_TURBO_CODING
#ifdef FEATURE_32X_SCH_DATA_RATE
#define NUM_TD_SCH_RATES 5 /* Rates Supported for Turbo Codes 2X,4X,8X,16X & 32X */
#else
#define NUM_TD_SCH_RATES 4 /* Rates Supported for Turbo Codes 2X,4X,8X & 16X */
#endif /* FEATURE_32X_SCH_DATA_RATE */
#endif /* MSMHW_TURBO_CODING */
extern const dec_sch_ob_params_type dec_sch_ob_params[NUM_SCH_RC][NUM_SCH_RATES];

/* Buffer to hold IS95C SCH status */
typedef struct {
  byte status;                /* bit 0 = FRAME CRC, bit 1 = Quality Indicator */
  word ser;                   /* Symbol Error Rate */
  int4 energy;
  word ltu_crc;               /* Valid only for Convolutional Decoding */
}  dec_conv_sch_status_type;   

#ifdef MSMHW_TURBO_CODING
typedef struct {
  word min_llr; /* Log Likelihood Ratio */
  int4 energy;  /* 18 bits of energy metric */
  byte num_iteration; /* Indicates the number of iterations run by the TD */
  boolean crc_pass; 
}  dec_turbo_sch_status_type;  
#endif /* MSMHW_TURBO_CODING */

#ifdef FEATURE_IS2000_REL_A //32X
/* ENUM for MuxPDU Type */
typedef enum {
  DEC_MUX_PDU_TYPE_1_2_4 = 0,      /* '00' for MuxPDU 1, 2, or 4 */
  DEC_MUX_PDU_TYPE_6     = 1,      /* '01' for MuxPDU 6 */
  DEC_MUX_PDU_TYPE_3     = 2,      /* '10' for MuxPDU 3 */
  DEC_MUX_PDU_TYPE_5     = 3       /* '11' for MuxPDU 5 */
} dec_mux_pdu_type_type;
#endif /* FEATURE_IS2000_REL_A */

typedef struct
{
   dec_rc_type rc;       /* Supplemental Channel Radio Configuration */
   dec_rate_set_type sch_rs;    /* rate set in use (RS1 or RS2)      */
   dec_sch_rate_type sch_rate;  /* SCH Rate (2x, 4X, 8X, 16X or 32X  */
   boolean mux_pdu_double;  /*  TRUE if double sized PDU */
   dec_coding_type code_type; /* CONVOLUTIONAL or TURBO */
#ifdef FEATURE_MULTIFRAME_INTERLEAVING
#error code not present
#endif /*  FEATURE_MULTIFRAME_INTERLEAVING  */
#ifdef FEATURE_IS2000_REL_A //32X
   dec_mux_pdu_type_type mux_pdu_type; // 1,2, 3, 5
   uint8             num_ltu;          // Number of LTUs in an SDU
   uint16            ltu_len;          // Length of LTU in bits
#endif /* FEATURE_IS2000_REL_A */
} dec_sch_cfg_type;

typedef union {
  dec_conv_sch_status_type conv_sch_status;
#ifdef MSMHW_TURBO_CODING
  dec_turbo_sch_status_type turbo_sch_status;
#endif /* MSMHW_TURBO_CODING */
} dec_sch_status_type;


extern const dec_fch_ob_params_type dec_fch_ob_params[];


#ifdef FEATURE_IS2000_REL_A_CC
typedef struct
{
  byte status;       /* bit 0 = FRAME CRC, bit 1 = Yamamoto Quality Indicator */
  byte ser;          /* Symbol Error Rate */
  int2 energy;
} dec_ccch_status_type;

typedef enum
{
  DEC_CCCH_FULL_RATE = 0,   // 9600 bps
  DEC_CCCH_2X_RATE,     // 19200 bps
  DEC_CCCH_4X_RATE,     // 38400 bps
  DEC_NUM_CCCH_RATES
} dec_ccch_data_rate_type;
#endif /* FEATURE_IS2000_REL_A_CC */
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*==========================================================================

FUNCTION DEC_INIT

DESCRIPTION
 Initialize the circuit to Idle. Load the polynomials for  1/2, 1/3,
 1/4 code rate.

DEPENDENCIES
  deint_init() calls dec_init()

RETURN VALUE
 None

SIDE EFFECTS
 None

============================================================================*/
extern void dec_init
(
  void
);

/*===========================================================================

FUNCTION DEC_VERSION

DESCRIPTION
  Return the hardware version number for this circuit.

DEPENDENCIES
  None

RETURN VALUE
  Returns the version number of the circuit

SIDE EFFECTS
  None

===========================================================================*/

extern byte dec_version
( 
  void
);

/*===========================================================================

FUNCTION DEC_SC_INIT

DESCRIPTION
  Initialize the circuit for decoding the Sync Channel.

DEPENDENCIES
  Dec_init() should have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  The decoder is put in continous mode.

===========================================================================*/
extern void dec_sc_init (
  void
);

/*===========================================================================

FUNCTION DEC_READ_SC_DATA

DESCRIPTION
  Read data from the Decoder as Sync Channel data.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dec_read_sc_data
(
  dec_sc_type buffer
  /* Buffer Pointer for storing the sync channel data */
);

/*=========================================================================
FUNCTION DEC_READ_SC_STATUS

DESCRIPTION
  This function reads the Status (SER, Energy Metrics) for Sync Channel frames.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
extern void dec_read_sc_status
(
  dec_sc_status_type *sc_buffer
  /* Pointer to the buffer for Sync Channel Status information */
);

/*===========================================================================

FUNCTION DEC_PC_INIT

DESCRIPTION
  Initialize the circuit for decoding the Paging Channel.

DEPENDENCIES
  Dec_init() should have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  The decoder is put in continous mode.

===========================================================================*/
extern void dec_pc_init
(
  dec_pc_rate_type pc_rate
  /* pc_rate =0 indicates Paging Channel Rate of 9600bps, 
    pc_rate =1 indicates 4800 bps */
);

/*===========================================================================

FUNCTION DEC_READ_PC_DATA

DESCRIPTION
  Read data from the Decoder Output Buffer as Paging Channel data.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dec_read_pc_data
(
  dec_pc_type buffer
  /* pointer to the paging channel buffer, the paging channel data will be 
     stored in this buffer */
  
);

/*=========================================================================
FUNCTION DEC_READ_PC_STATUS

DESCRIPTION
  This function reads the Status (SER, Energy Metrics) for Paging Channel frame

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
extern void dec_read_pc_status
(
  dec_pc_status_type *pc_buffer
  /* pointer to the paging channel status buffer */
);

/*===========================================================================

FUNCTION DEC_TC_INIT

DESCRIPTION
  Initialize the circuit for decoding the Traffic Channel.

DEPENDENCIES
  Dec_init() should have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void dec_tc_init
(
  void
);

/*===========================================================================

FUNCTION DEC_READ_FCH_DATA

DESCRIPTION
  Read data from the decoder as Fundamental Channel data.  Note that the data
  for the specified RC is loaded from the beginning of the buffer.  Any
  space at the end of the buffer, which was not requested to be filled,
  is indeterminant.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dec_read_fch_data
(
  dec_fch_type      *buf_ptr,    
  /* Buffer for Traffic Channel data      */
  dec_rc_type       rc,
  /* Radio Configuration */
  dec_fch_rate_type fch_rate     
  /* Rate decode buffer to read           */
);


/*=========================================================================
FUNCTION DEC_READ_FCH_STATUS

DESCRIPTION
  This function reads the Status (CRC, SER, Energy Metrics) for
  Fundamental Channel frames.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
extern void dec_read_fch_status
(
  dec_rc_type         rc, 
  /* Radio Configuration */
  dec_fch_status_type *fch_status
  /* Pointer to the FCH status buffer */
);


#if defined (FEATURE_IS2000_P2) && defined (FEATURE_IS2000_REL_A_SVD)
/*===========================================================================

FUNCTION DEC_READ_DCCH_DATA

DESCRIPTION
  Read data from the Decoder as DCCH data.  

DEPENDENCIES
  Dec_tc_init() should have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/              

extern void dec_read_dcch_data
(
  dec_fch_type      *buf_ptr,     /* Buffer for Traffic Channel data       */
  dec_rc_type       rc            /* Radio configuration                   */                                    
);


/*=========================================================================
FUNCTION DEC_READ_DCCH_STATUS

DESCRIPTION
  This function reads the Status (CRC, SER, Energy Metrics) for 
  DCCH frames.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/

extern void dec_read_dcch_status
(
  dec_rc_type rc, 
    /* Indicates the Radio Configuration for DCCH */
  dec_dcch_status_type *dcch_status
    /* Pointer the buffer where DCCH decoding status will be stored */
);
#endif /* FEATURE_IS2000_P2 && FEATURE_IS2000_REL_A_SVD */


/*===========================================================================

FUNCTION DEC_READ_SCCH_DATA

DESCRIPTION
  Read data from the Decoder as Supplemental(Traffic) Channel data
 (IS95B SCCH).

DEPENDENCIES
  Dec_tc_init() should have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dec_read_scch_data
(
  dec_rc_type       rc,         
  /* RADIO CONFIGURATION */
  dword              *buf_ptr,    
  /* Buffer for Supplemental Code Channel data      */
  uint16            *scch_ser,
  /* pointer to symbol error rate */
  int32             *scch_energy,
  byte              *scch_crc,
  byte              num_sup     
  /* Number of SCCH */
);

/*===========================================================================

FUNCTION DEC_READ_SCH_DATA

DESCRIPTION
  Read data from the Decoder as Supplemental(Traffic) Channel data
 (IS95C SCH). This function reads each mux pdu (single/double) into the 
  buffer pointed to by the dsm_item.

DEPENDENCIES
  Dec_tc_init() should have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_PLT
#error code not present
#else
extern void dec_read_sch_data
(
  dec_sch_cfg_type *sch_rec_ptr,  /* Pointer to rxc_sch record pointer */
  dsm_item_type **dsm_item_ptr, /* array of dsm_item pointers */
  byte *mux_pdu_count      /* return the number of MUX PDU read */
);
#endif  /* FEATURE_PLT */

/*=========================================================================
FUNCTION DEC_READ_SCH_STATUS

DESCRIPTION
  This function reads the Status (SER, Energy Metrics) for SCH Channel frames

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
extern void dec_read_sch_status
(
#ifdef FEATURE_IS2000_REL_A
  dec_mux_pdu_type_type pdu_type,
#endif
  dec_rc_type rc, 
  dec_sch_rate_type sch_rate, 
  dec_sch_status_type *sch_status,
  dec_coding_type code_type   /* Indicates if Convolutional/Turbo coding */
);

/*=========================================================================
FUNCTION DEC_DTX_THRESH_INIT

DESCRIPTION
  This function initializes the dtx threshold value depending on the RC and
  the code type.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Writes into the MSM register DEFAULT_IQ_ACC_WH
=============================================================================*/
void dec_dtx_thresh_init(dec_rc_type rc, dec_coding_type code_type, dec_sch_rate_type
 rate);

#ifdef FEATURE_SRCH_CLKREGIM_RM
/*============================================================================

FUNCTION DEC_SVD_CLK_ENABLE

DESCRIPTION 
  Enable clocks for DEC and SVD blocks for CDMA operations.

DEPENDENCIES
  DEC clock regimes should have been initialized.

RETURN VALUE 
  None

SIDE EFFECTS
  Enable SVD decoder clocks
=============================================================================*/
extern void dec_svd_clk_enable (void);

/*============================================================================

FUNCTION DEC_SVD_CLK_DISABLE

DESCRIPTION 
  Disable clocks for DEC and SVD blocks when not in use.

DEPENDENCIES
  DEC clock regimes should have been initialized.

RETURN VALUE 
  None

SIDE EFFECTS
  Disable SVD decoder clocks
=============================================================================*/
extern void dec_svd_clk_disable (void);

/*============================================================================

FUNCTION DEC_TD_CLK_ENABLE

DESCRIPTION 
  Enable clocks for Turbo Decoder block for CDMA operations.

DEPENDENCIES
  TURBO DEC clock regimes should have been initialized.

RETURN VALUE 
  None

SIDE EFFECTS
  Enable turbo decoder clocks
=============================================================================*/
extern void dec_td_clk_enable (void);

/*============================================================================

FUNCTION DEC_TD_CLK_DISABLE

DESCRIPTION 
  Disable clocks for Turbo Decoder block when not in use.

DEPENDENCIES
  TURBO DEC clock regimes should have been initialized.

RETURN VALUE 
  None

SIDE EFFECTS
  Disable turbo decoder clocks
=============================================================================*/
extern void dec_td_clk_disable (void);

#else /* !FEATURE_SRCH_CLKREGIM_RM */

/*============================================================================

FUNCTION DEC_CLK_SELECT

DESCRIPTION 
  This function selects the appropriate decoder clock for Traffic state (TCXO)
  or Idle state (Chipx8).  This function is implemented to reduce power 
  consumption in Idle state.
  Both Turbo and Viterbi decoders are reset after clock switching.

DEPENDENCIES
  DEC Clock regime should have been enabled.

RETURN VALUE 
  None

SIDE EFFECTS
  Switches decoder clock
=============================================================================*/
extern void dec_clk_select
(
  dec_clk_select_type new_clk_select
);
#endif /* !FEATURE_SRCH_CLKREGIM_RM */


/*============================================================================
=============================================================================*/
#ifdef FEATURE_IS2000_REL_A_CC

boolean dec_read_bcch_status
(
  dec_frame_status_type *status
);

void dec_ccch_init (void);

boolean dec_read_ccch_status
(
  dec_frame_status_type *status,
  dec_ccch_data_rate_type ccch_rate
);

void dec_read_ccch_data
(
  dec_ccch_buf_type ccch_buf,
  dec_ccch_data_rate_type ccch_rate
);

#endif

/*===========================================================================

FUNCTION DEC_READ_MUX_PDU5_SCH_DATA

DESCRIPTION
  Read data from the Decoder as Supplemental(Traffic) Channel data (IS95C SCH).
  This function reads MUX_PDU5 into the buffer pointed
  to by the dsm_item.
DEPENDENCIES
  Dec_tc_init() should have already been called.
  This function should not be called if a frame is bad and there is no LTU
  ( 1X, 2X and turbo case ).

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dec_read_mux_pdu5_sch_data
(
  dec_sch_cfg_type *sch_rec_ptr,  /* Pointer to rxc_sch record pointer */
  dsm_item_type **dsm_item_ptr, /* array of dsm_item pointers */
  byte *mux_pdu_count,      /* return the number of MUX PDU read */
  word ltu_crc,            /* LTU crc from the decoder status */
  byte *good_ltu           /* return value of number of good ltus */
);

#endif /* DEC_5000_H */

