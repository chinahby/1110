#ifndef DIAGTUNE_H
#define DIAGTUNE_H
/*==========================================================================

                 Diagnostic Target Specific Prototypes

Description
  Prototypes for functions and data members that must be implemented to make
  the core diagnostics service work.
  

Copyright (c) 2001-2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            Edit History

$PVCSPath:  L:/src/asw/MSM5200/common/vcs/diagtune.h_v   1.11   03 Jun 2002 17:06:42   amcintos  $
$Header: //depot/asic/msmshared/services/diag/MSMSHARED_DIAG_1.2/diagtune.h#6 $ $DateTime: 2007/05/22 11:39:58 $ $Author: pratapc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/21/06   as      Changed diag drain timer timeout.
12/05/06   as      Added timeout for diag drain timer 
05/10/06   bt      Added support for DTV.
09/15/05   as      Changed diag SPC timeout for FEATURE_DIAG_SPC_TIMEOUT.
12/06/04   as      Added support for FTM (FEATURE_FACTORY_TESTMODE)
01/29/04   as      Corrected FEATURE, to fix DIAGBUF_SIZE definition
11/12/02   pj      Increased diagbuf size to 30KB for MS-Based
                   (FEATURE_GPSONE_PE).
05/31/02   ATM     Renamed FEATURE_GPLT to GSM_PLT
05/15/02   gsc     Make top level FEATURE symbol (FEATURE_GSM, FEATURE_WCDMA)
                   multi-mode compliant.
03/08/02   kar     Changes for RPC
02/06/02   ATM     Cleanup - renamed log codes to match convention
02/06/02   ATM     GPLT needed 1X codes temporarily till RPC is fixed
01/24/02   ATM     KOVU needs its own version of this tune file -- we don't need
                   other log codes from 1x/wcdma/etc...
11/16/01   gsc     Incorporated review comments from Ajit for changes
                   related to DIAGBUF_SIZE 
11/15/01   gsc     Added support for PLT (FEATURE_WPLT)
06/27/01   lad     Added multiple equip ID logging support.
                   Updated comments to be more useful.
04/17/01   lad     Added constants for event services.
02/23/01   lad     Created file.

===========================================================================*/

#include "log_codes.h" /* To get default # of items for log equip IDs. */

#if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
#endif /* FEATURE_WCDMA || FEATURE_GSM */

#if defined (FEATURE_DTV)
#error code not present
#endif

#ifdef FEATURE_MULTIPROCESSOR
#error code not present
#endif

/* DIAGBUF_SIZE is the size, in bytes, of diag's output buffer.  If this buffer
 * fills up, logs and messages will be dropped.  It is recommended to make this
 * number a power of 2 to allow the compiler to optimize the arithmetic using
 * this number to use shift operations instead of brute force math.
 *
 * Note: This must be larger than DIAG_MAX_TX_PKT_SIZ. */
#ifdef FEATURE_DIAG_SMALL_BUFFER
   #define DIAGBUF_SIZE (2048)
#else
   #define DIAGBUF_SIZE (32768)
#endif

/* The size of the DIAG heap.  Currently, it is only used for response 
   packets and allocating tasks for stress testing.  DIAGBUF_SIZE only 
   impacts MSG and LOG packets. */
#define DIAG_HEAP_SIZE (1024 * 8)

/* Some external devices in the past shared diagpkt.h with the DMSS.  
 * This is the max size they expect, so we don't go larger that 944
 * until those external devices are updated and distributed to handle
 * arbitrary packet lengths.
 *
 * Also, this number must always be <= to DIAGBUF_SIZE - 4, and would still be
 * really unlikely to alloc successfully (unless it is a guaranteed
 * response). */
#define DIAG_MAX_TX_PKT_SIZ 0x800

/* This determines how much RAM is statically allocated for RX'd packets. */
#if defined (FEATURE_WPLT) || defined (FEATURE_GSM_PLT) || \
    defined (FEATURE_DIAG_RPC)
#define DIAG_MAX_RX_PKT_SIZ 0x800
#else
#define DIAG_MAX_RX_PKT_SIZ 1536
#endif

/* By default, all messages and logs leave this much padding free when
 * allocating an outbound packet.  This way, all logs and messages have the
 * same probability of success, as long as all of those packets are smaller
 * than this number.  If a given packet is larger, it will be at a
 * disadvantage.  The basic algorithm is as follows:
 *
 * if (DIAGBUF_BALANCE_PAD < DIAGBUF_FREE) { 
 *   //go ahead and attempt to alloc 
 * }
 *
 * This way, an attempt to alloc 12 bytes will be just as likely to be
 * successful as an attempt to alloc 100 bytes.
 *
 * However, an attempt to alloc more than the PAD will not benefit from this
 * balancing scheme. */
#define DIAGBUF_BALANCE_PAD 0x140

#define DIAG_IDLE_PROC_CNT 10

/* Amount of time to timeout when a bad SPC
 * is received (10 secs = 10000 msecs). */
/* FEATURE_DIAG_SPC_TIMEOUT is defined in Sirius target */
#ifndef FEATURE_DIAG_SPC_TIMEOUT
#define DIAG_TOUT_TIME 10000
#else
#define DIAG_TOUT_TIME 1000
#endif /* FEATURE_DIAG_SPC_TOUT */

/* Search lists, used for the listener API, are allocated in blocks as items
   are inserted.  This constant indicates the increment size of a list. 
   Each node of the list is 8 bytes */
#define DIAG_SEARCHLIST_SIZE_INCR (16)
                                     
/* Event reporting services constants */

/* Time (ms) for event stale timer */
#define EVENT_TIMER_LEN 1000 

/* Time (ms) for diag drain stale timer */
#ifndef DIAG_DRAIN_TIMER_LEN
#define DIAG_DRAIN_TIMER_LEN 200
#endif

/* The max payload data item length */
#define EVENT_MAX_PAYLOAD_LEN 80 

/* Approximate event report packet size - to be used as a threshold - actual
   packet will be slightly larger. */
#define EVENT_RPT_PKT_SIZE 128

/* This specifies the last equipment ID for use in logging services.  This is a
 * 4-bit value, so it cannot exceed 15. */
#define LOG_EQUIP_ID_LAST LOG_EQUIP_ID_LAST_DEFAULT

/* Each equipment ID has a maximum number of items.  Each item takes up a bit
 * in the mask and a byte in the log_nice array.  Therefore, the highest number
 * per equipment ID will cause an allocation of 'n' bytes, where 'n' is defined
 * below ('x' is the last item # for an equipment ID):
 * n(x) = ((x+7) / 8) + x * sizeof(uint16) */
#if (defined (FEATURE_IS2000) || defined (FEATURE_HDR) || \
     defined (FEATURE_GSM_PLT) || defined (FEATURE_BT) || \
     defined (FEATURE_MP4_DECODER) || defined (FEATURE_MULTIPROCESSOR) || \
     defined (FEATURE_FACTORY_TESTMODE)) 
  
  #define LOG_EQUIP_ID_1_LAST_CODE LOG_LAST_C   /* EquipID not supported */

#endif

#ifdef FEATURE_WCDMA
#error code not present
#endif

#ifdef FEATURE_GSM
#error code not present
#endif

#if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
#endif

#ifdef FEATURE_DTV
#error code not present
#endif

#ifdef FEATURE_MULTIPROCESSOR
#error code not present
#endif

#endif /* DIAGTUNE_H */

