/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  D O W N L O A D  P A C K E T   P R O C E S S I N G

GENERAL DESCRIPTION
  This module handles the DMSS Async Download Protocol to download
  code using simple generic UART services.  This consists of an
  infinite loop of receiving a command packet through the UART,
  acting on it, and returning a response packet.

EXTERNALIZED FUNCTIONS
  process_packets
    Runs the packet protocol (forever).

INITIALIZATION AND SEQUENCING REQUIREMENTS
  All necessary initialization for normal CPU operation must have
  been performed, and the UART must have been initialized, before
  entering this module.

Copyright (c) 1998-22009 by QUALCOMM Incorporated.
All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM6050/vcs/dloadarm.c_v   1.3   22 Mar 2002 15:39:40   rprenove  $
$Header: //depot/asic/qsc1100/services/dload/dloadarm.c#7 $ $DateTime: 2009/04/09 01:50:15 $ $Author: dkammath $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/08/09   ddk     Fix when MMU was enabled in Boot code for ulc.
06/06/08   dk      Fixed Klockwork errors.
03/17/08   jdt     Fix compiler warnings in dloadarm.c
03/13/08   ng      Set location of DLOAD_BASE to SCL_NPRG_CODE_BASE if it's defined
10/25/07   jdt     Set location of DLOAD_BASE to SCL_MODEM_CODE_BASE if it's defined
06/21/07   yl/ty   Support UART3 for dload if FEATURE_THIRD_UART defined.
11/14/06   tbg     Fixed bug in reset_cmd -- was not working correctly when
                   watchdog was in auto-kick mode.  Requires boot_hw_reset()
                   to be implemented.
10/27/06   tbg     Ported over FEATURE_DLOAD_MEM_DEBUG.
01/12/06   amw     Change location where dload will accept images to be based
                   on memory map symbol.
11/21/05   sd      Added support for secure download and fixed RVCT compiler
                   warnings.
05/13/05   chs     In oemsbl, select dfferent address for BB_RAM_BASE and
                   APP_RAM_BASE based on NAND/NOR boot.
06/07/04   rp      Updated boot_downloader() with global varibale 
                   initialization. 
12/08/03   rp      In process_packets(), moved setting interface from
                   processing of write cmd to processing param req. 
12/04/03   drh     Move data cache disable from go_cmd() to preq_cmd()
                   to ensure active data cache page tables are not corrupted
09/25/03   rp      If USB is not defined will enable USB xtal so flash
                   programming code can access USB registers. 
05/16/03   drh     Moved cache policy change before jumping to armprg from
                   boot_reset_handler.s to here
03/25/03   lad     Added extern decl of chk_security_code_ptr in the boot 
                   block to remove coupling to diagi.h.
03/06/03   pg      Added support for write_32bit_cmd.  
                   Increased PROTOCOL_ID from 4 to 5.
01/28/03   rp      Updated use of active_interface to properly handle data
                   received simultaneously over USB and UART1. 
11/12/02   rp      Updated support to determine active interface. Polling
                   of UART and USB is now initiated in get_next_char().
                   Updated boot_downloader() to not use diag_dload flag.
09/24/02   drh     Add temporary workaround for NAND download due to address
                   modification by QPST that does not work for low RAM
                   addresses.
09/19/02   drh     Fixed packet checksum in finish_building_packet.
                   Change method of jumping to armprg.
03/22/02   rp      In boot_downloader() removed unnecessary call to
                   clk_regime_force_mclk_tcxo().
03/06/02   rp      Moved all USB specific code under FEATURE_USB.
                   Added dloadusb_poll() to recv_packet() to try USB
                   after an error and UART also timing out.
02/26/02   rp      Modified boot_downloader() to only invoke
                   clk_regime_force_mclk_tcxo() when diag_dload is true.
02/20/02   rp      Added static boolean diag_download_over_USB to insure
                   initial value for download flag.
02/19/02   rp      Updated to support dloads over USB.
05/16/01   dsb     Changed clk_regime_force_uP_clk_to_tcxo call to
                   clk_regime_force_mclk_to_tcxo.
05/11/01   dsb     Restricted scope of pkt_buf to process_packets.
04/30/01   dsb     Removed call to clk_regime_set_mclk_tcxo, since that
                   function depends on successful initialization of
                   RAM.  Instead, we switch the uP clock manually back to
                   TCXO.  This can only be done in JTAG, as ICE has a bug
                   removing the ability to do this safely.  Therefore, ICE
                   will not support a code download via the QPST (it doesn't
                   anyways, since the ROM is shadowed in the internal ICE
                   SRAM).
03/21/01   dsb     Added clk_regime_set_mclk_tcxo to boot_downloader.  This
                   replaces the same call in dload_uart_init.
08/27/00   rmd     Replaced KICK_WATCHDOG with BOOTHW_KICK_WATCHDOG.
08/25/00   jcw     Merge for MSM5105
06/30/00   rmd     Added boot_downloader_clk_init to init MSM clk and Memory/IO
                   wait states to old configuration. This done before code is download
                   into the phone to prevent any old software to init incorrectly the MSM.
12/07/99   jc      Minor cleanup to reduce confusion
11/03/99   mk      Transmit Power Amplifier (PA/PA2) will be turned off after
                   hitting an error fatal.
08/27/99   jwh     Merged with the MSM3000 merge with SS baseline.
08/20/99   mk      ARM2.5 compiler update.
03/17/99    ms     Removed bb_ prefix for Virtual Boot Block.
                   Now calling dload_uart_init instead of bb_uart_init.
12/15/98   jc      Merge from et0400
11/05/98   jtg     Shortened line to 80 chars.
10/14/98   hcg     Removed <string.h> and replaced with memory.h for global
                   memcpy() solution.
09/13/98   hcg     General mods for MSM3000 support.  Implemented security
                   feature, changed parameter response message.
08/08/98   hcg     Added #include file - Cleaned up comments/removed unused code.
08/03/98   hcg     Fixed bug for offset into cmd_buf for security feature
07/30/98   hcg     Phone powers down after failure to unlock security feature
07/30/98   hcg     Implemented security feature
07/26/98   hcg     Revised for coding standard, removed unused code
06/01/98   hcg     Ported from Flashprg by ptw

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "memory.h"
#include "comdef.h"
#include "dloadarm.h"
#include "boothw.h"
#include "dloaduart.h"
#include "target.h"
#include "clkregim.h"
#include "mobile.h"
#include "diagi.h"

#include "cache_mmu.h"
#include "uartdm.h"
#ifdef FEATURE_QHSUSB
    #include "qhsusb_al_dload_api.h"
#endif

#ifdef FEATURE_HS_USB_BASIC
  #include "hsu_al_dload.h"
  #include "hsu_boot_dload.h"
#else
  #if (defined(FEATURE_USB) || defined(FEATURE_QHSUSB) )
    #include "dloadusb.h"
  #endif
#endif

#ifdef FEATURE_DLOAD_MEM_DEBUG
  #include "dload_debug.h"
#endif

#ifdef FEATURE_DLOAD_SECURITY
  #ifdef BOOT_SBLDLOAD_H
    #include BOOT_SBLDLOAD_H
  #else
    #include "oemsbl_dload.h"
  #endif
#endif /* FEATURE_DLOAD_SECURITY */



/*===========================================================================

                     EXTERNAL DEFINITIONS FOR MODULE

===========================================================================*/
extern void mmu_invalidate_data_cache(void);
extern void mmu_invalidate_instruction_cache(void);
extern void cache_mmu_disable(void);
extern void mmu_enable_instruction_cache(void);
extern void boot_hw_reset(void);


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* The Download Protocol is enhanced from time to time.  This value defines
   the version number of the protocol supported by this implementation. */
#define  PROTOCOL_ID          6

/* Someday the change might not be backwards compatible.  When that happens,
   this value will define the earliest protocol spec with which this
   implementation is compatible.  So far, all versions are compatible, so
   this value is 1, which indicates that it is compatible with all versions. */
#define  MIN_PROTOCOL_ID      1

/* The protocol allows the implementation to place an arbitrary limit on
   how large a data write packet can be.  This value shouldn't be too
   small, for efficiency reasons, but can't be bigger than the memory
   buffer we can allocate.  Here we pick a nice, arbitrary value of
   4K bytes. */
#define  MAX_WRITE_SIZE       (0x1000)

/* These variables are generated by the linker, which give the locations
   of memory for which the phone code resides.  This serves as a base for which
   we can zero out memory and use as a packet buffer, as well as code
   downloading */

/* The addresses below allow for armprg to be loaded anywhere between 0x0 and
   32MB. The packet buffer will be just below 32MB. */

/* This is the security password located in the boot block. */
extern uint8 const chk_security_code_ptr[];

/* area for download packet, etc. = 8K */
#define  PKT_BUF_SIZE           (0x2000)

/* Start of packet buffer */
#define  PKT_BUFFER_BASE      (0xA000000-PKT_BUF_SIZE)
/* Start of download area */
#define  DLOAD_BASE           0x08000000

/* Limit of download area */
#define  DLOAD_LIMIT          PKT_BUFFER_BASE

/* offset to convert from hex file address to ARM mechanism */
#ifdef  BOOT_MODE_NAND
#define  DLOAD_OFFSET         0
#elif defined(BOOT_MODE_NOR)
#define  DLOAD_OFFSET         (DLOAD_BASE)
#else
#error MUST DEFINE BOOT_MODE_NAND or BOOT_MODE_NOR
#endif


#define PACKED __packed
#define B_PTR( var )  ( (byte *) (void *) &(var) )
#define NOTICE_TYPE    const char


/*-------------------------------------------------------------------------*/

/* We determine what phone model we're running on by checking a magic
   location in the boot block.  Before the boot block was implemented,
   this location was always filled with NOP op-codes. */
#define  MOB_MODEL_NOP_VAL          0x9090      /* Old value: NOP opcodes.
                                                   Model unknown.  */

#define  MOB_MODEL_ERASED_VAL       0xFFFF      /* Value when blank, due to
                                                   the bootblock being
                                                   erased.  Model unknown. */

#define  MOB_MODEL_UNKNOWN_VAL      0x0000      /* Return this value when the
                                                   model is unknown. */

      /* MOB_MODEL_UNKNOWN_VAL is 0x0000 because that is the one and only
         value that can always be attained by writing to the location,
         no matter what is there presently, without erasing the block.
         This makes it possible to recover from corruption of this byte. */

#define  DUMMY                      0x0000

/*-------------------------------------------------------------------------*/


#ifndef FEATURE_HWTC /* 10/23/01 lcl */
/* Flag indicates whether a valid security code has been received yet */
boolean sec_code_unlocked=FALSE;
#endif /* FEATURE_HWTC */

/*-------------------------------------------------------------------------*/

/* Async HDLC achieves data transparency at the byte level by using
   two special values. The first is a flag value which begins and ends
   every packet: */
#define  ASYNC_HDLC_FLAG      0x7e

/* The flag value might appear in the data.  If it does, it is sent as
   a two-byte sequence consisting of a special escape value followed by
   the flag value XORed with 0x20.  This gives a special meaning to the
   escape character, so if it appears in the data it is itself escaped
   in the same way. */
#define  ASYNC_HDLC_ESC       0x7d
#define  ASYNC_HDLC_ESC_MASK  0x20

/*-------------------------------------------------------------------------*/

/* Each packet sent or received in this download protocol begins with a
   one-byte command code chosen from the following list.  Each packet
   type is either received by the phone or transmitted by the phone, but
   not both. */

/*lint -e749  Some values in this local enum are not used in this program. */
/*lint -esym(751,cmd_code_type) and in fact this enum itself is not used. */
typedef PACKED enum
   {
   CMD_WRITE  = 0x01,   /* Write a block of data to memory (received)    */
   CMD_ACK    = 0x02,   /* Acknowledge receiving a packet  (transmitted) */
   CMD_NAK    = 0x03,   /* Acknowledge a bad packet        (transmitted) */
   CMD_ERASE  = 0x04,   /* Erase a block of memory         (received)    */
   CMD_GO     = 0x05,   /* Begin execution at an address   (received)    */
   CMD_NOP    = 0x06,   /* No operation, for debug         (received)    */
   CMD_PREQ   = 0x07,   /* Request implementation info     (received)    */
   CMD_PARAMS = 0x08,   /* Provide implementation info     (transmitted) */
   CMD_DUMP   = 0x09,   /* Debug: dump a block of memory   (received)    */
   CMD_RESET  = 0x0A,   /* Reset the phone                 (received)    */
   CMD_UNLOCK = 0x0B,   /* Unlock access to secured ops    (received)    */
   CMD_VERREQ = 0x0C,   /* Request software version info   (received)    */
   CMD_VERRSP = 0x0D,   /* Provide software version info   (transmitted) */
   CMD_PWROFF = 0x0E,   /* Turn phone power off            (received)    */
   CMD_WRITE_32BIT = 0x0F,  /* Write a block of data to 32-bit memory 
                               address (received)                        */
   CMD_MEM_DEBUG_QUERY = 0x10, /* Memory debug query       (received)    */
   CMD_MEM_DEBUG_INFO  = 0x11, /* Memory debug info        (transmitted) */
   CMD_MEM_READ_REQ    = 0x12, /* Memory read request      (received)    */
   CMD_MEM_READ_RESP   = 0x13, /* Memory read response     (transmitted) */
   CMD_DLOAD_SWITCH = 0x3A
   } cmd_code_type;


/*-------------------------------------------------------------------------*/

/* For the received packets, we need the minimum length of a valid packet
   of that type (except where the packet is only a command code). Note,
   size is in bytes */

#define  WRITE_SIZ   8  /* Minimum size of the Write packet */
#define  ERASE_SIZ   9  /* Total size of the Erase packet   */
#define  GO_SIZ      7  /* Total size of the Go packet      */
#define  UNLOCK_SIZ  9  /* Total size of the unlock packet  */
#define  WRITE_32BIT_SIZ   9  /* Minimum size of the Write_32bit packet */

/*-------------------------------------------------------------------------*/

   /* Maximum size of a received packet. */

#define  MAX_PACKET_LEN    PKT_BUF_SIZE

   /* Minimum size of a received packet. */
#define  MIN_PACKET_LEN    3        /* 2 bytes CRC + 1 byte command code */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * New Method to get data from the underlying driver.
 *-------------------------------------------------------------------------*/
typedef enum 
{
  DLOAD_HDLC_IDLE_STATE,                      /**< Initial State of HDLC state machine */
  DLOAD_HDLC_GOT_FLAG,                        /**< Got Flag                            */
  DLOAD_HDLC_MSG_BODY_STATE,                  /**< State of HDLC when receiving data   */
  MAX_DLOAD_HLDC_STATE_TYPE = 0x7FFFFFFF      /**< To ensure it's 32 bits wide         */
}dload_hdlc_state_type;

struct   incoming_packet 
{
  uint32     length;                   /** Number of bytes in buffer.            */
  uint32     start_location;           /** Location where the valid data starts  */
  uint32     end_location;             /** Location where the valid data ends    */
  uint32     step_crc;                 /** CRC on the incoming data         */
  uint32     packet_ready;             /** Is data in the packet ready      */
  byte       buffer[MAX_PACKET_LEN];   /** Data in the buffer.              */
};

struct incoming_packet packet_data;
dload_hdlc_state_type  hdlc_curr_state   = DLOAD_HDLC_IDLE_STATE;
static uint32          hdlc_escape_state = 0;
/*-------------------------------------------------------------------------
 * New Method to get data from the underlying driver.
 *-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*/

/* These packets are the same every time they are transmitted by the phone,
   so we can use pre-computed "canned" messages.  All byte-stuffing required
   by the protocol for transparency has already been done.  Each packet ends
   with a flag, which is both transmitted and used as the termination
   indicator for these arrays. */

static const byte rsp_ack[] =                   /* ACK */
  {
  CMD_ACK,             /* ACK type */
  0x6a,                /* CRC, MSB */
  0xd3,                /* CRC, LSB */
  ASYNC_HDLC_FLAG
  };

static const byte rsp_nak_invalid_fcs[] =       /* NAK_INVALID_FCS */
  {
  CMD_NAK,             /* NACK type */
  0x00,                /* Reason code, MSB */
  0x01,                /* Reason code, LSB */
  0x21,                /* CRC, MSB */
  0x38,                /* CRC, LSB */
  ASYNC_HDLC_FLAG
  };

static const byte rsp_nak_invalid_dest[] =      /* NAK_INVALID_DEST */
  {
  CMD_NAK,             /* NACK type */
  0x00,                /* Reason code, MSB */
  0x02,                /* Reason code, LSB */
  0xba,                /* CRC, MSB */
  0x0a,                /* CRC, LSB */
  ASYNC_HDLC_FLAG
  };

static const byte rsp_nak_invalid_len[] =       /* NAK_INVALID_LEN */
  {
  CMD_NAK,             /* NACK type */
  0x00,                /* Reason code, MSB */
  0x03,                /* Reason code, LSB */
  0x33,                /* CRC, MSB */
  0x1b,                /* CRC, LSB */
  ASYNC_HDLC_FLAG
  };

static const byte rsp_nak_early_end[] =         /* NAK_EARLY_END */
  {
  CMD_NAK,             /* NACK type */
  0x00,                /* Reason code, MSB */
  0x04,                /* Reason code, LSB */
  0x8c,                /* CRC, MSB */
  0x6f,                /* CRC, LSB */
  ASYNC_HDLC_FLAG
  };

static const byte rsp_nak_too_large[] =         /* NAK_TOO_LARGE */
  {
  CMD_NAK,                      /* NACK type */
  0x00,                         /* Reason code, MSB */
  0x05,                         /* Reason code, LSB */
  0x05,                         /* CRC, MSB */
  ASYNC_HDLC_ESC,
  0x7e ^ ASYNC_HDLC_ESC_MASK,   /* CRC, LSB */
  ASYNC_HDLC_FLAG
  };

static const byte rsp_nak_invalid_cmd[] =       /* NAK_INVALID_CMD */
  {
  CMD_NAK,             /* NACK type */
  0x00,                /* Reason code, MSB */
  0x06,                /* Reason code, LSB */
  0x9e,                /* CRC, MSB */
  0x4c,                /* CRC, LSB */
  ASYNC_HDLC_FLAG
  };

static const byte rsp_nak_failed[] =            /* NAK_FAILED */
  {
  CMD_NAK,             /* NACK type */
  0x00,                /* Reason code, MSB */
  0x07,                /* Reason code, LSB */
  0x17,                /* CRC, MSB */
  0x5d,                /* CRC, LSB */
  ASYNC_HDLC_FLAG
  };

static const byte rsp_nak_wrong_iid[] =         /* NAK_WRONG_IID */
  {
  CMD_NAK,             /* NACK type */
  0x00,                /* Reason code, MSB */
  0x08,                /* Reason code, LSB */
  0xe0,                /* CRC, MSB */
  0xa5,                /* CRC, LSB */
  ASYNC_HDLC_FLAG
  };

static const byte rsp_nak_bad_vpp[] =           /* NAK_BAD_VPP */
  {
  CMD_NAK,             /* NACK type */
  0x00,                /* Reason code, MSB */
  0x09,                /* Reason code, LSB */
  0x69,                /* CRC, MSB */
  0xb4,                /* CRC, LSB */
  ASYNC_HDLC_FLAG
  };

static const byte rsp_nak_verify_failed[] =     /* NAK_VERIFY_FAILED */
  {
  CMD_NAK,             /* NACK type */
  0x00,                /* Reason code, MSB */
  0x0a,                /* Reason code, LSB */
  0xf2,                /* CRC, MSB */
  0x86,                /* CRC, LSB */
  ASYNC_HDLC_FLAG
  };

#ifndef FEATURE_HWTC
static const byte rsp_nak_no_sec_code[] =          /* NAK_NO_SEC_CODE */
  {
  CMD_NAK,    /* NACK type */
  0x000,      /* Reason code, MSB */
  0x00B,      /* Reason code, LSB */
  0x07B,      /* CRC, MSB */
  0x097,      /* CRC, LSB */
  ASYNC_HDLC_FLAG
  };

static const byte rsp_nak_bad_sec_code[] =        /* NAK_BAD_SEC_CODE */
  {
  CMD_NAK,    /* NACK type */
  0x000,      /* Reason code, MSB */
  0x00C,      /* Reason code, LSB */
  0x0C4,      /* CRC, MSB */
  0x0E3,      /* CRC, LSB */
  ASYNC_HDLC_FLAG
  };
#endif /* FEATURE_HWTC */


static const byte rsp_nak_op_not_permitted[] =      /* NAK_OP_NOT_PERMITTED */
  {
  CMD_NAK,    /* NACK type */
  0x000,      /* Reason code, MSB */
  0x00E,      /* Reason code, LSB */
  0x0,      /* CRC, MSB */
  0x0,      /* CRC, LSB */
  ASYNC_HDLC_FLAG
  };
  
#ifdef FEATURE_DLOAD_MEM_DEBUG
static const byte rsp_nak_invalid_addr[] =          /* NAK_INVALID_ADDR */
  {
  CMD_NAK,    /* NACK type */
  0x000,      /* Reason code, MSB */
  0x00F,      /* Reason code, LSB */
  0x0,      /* CRC, MSB */
  0x0,      /* CRC, LSB */
  ASYNC_HDLC_FLAG
  };
#endif

/*-------------------------------------------------------------------------*/

/* This table translates a response code into a pointer to a canned
   response packet.  The table must be in the same order as the
   response_code_type enum. */

static const byte * const response_table[] =
  {
  rsp_ack,
  rsp_nak_invalid_fcs,
  rsp_nak_invalid_dest,
  rsp_nak_invalid_len,
  rsp_nak_early_end,
  rsp_nak_too_large,
  rsp_nak_invalid_cmd,
  rsp_nak_failed,
  rsp_nak_wrong_iid,
  rsp_nak_bad_vpp,
  rsp_nak_op_not_permitted,
#ifdef FEATURE_DLOAD_MEM_DEBUG
  rsp_nak_invalid_addr,
#endif

#ifdef FEATURE_HWTC /* 10/23/01 lcl */
#error code not present
#else
  rsp_nak_verify_failed,
  rsp_nak_no_sec_code,
  rsp_nak_bad_sec_code
#endif /* FEATURE_HWTC */
  };

/* This flag is true until communications established */
static boolean init_boot = TRUE;

#ifdef FEATURE_HS_USB_BASIC
static hsu_al_dload_interface_type interface = INTERFACE_UNKNOWN;
static hsu_al_dload_interface_type active_interface = INTERFACE_UNKNOWN;
#else
#if (defined(FEATURE_USB) || defined(FEATURE_QHSUSB) )
static dloadusb_interface_type interface = INTERFACE_UNKNOWN;
static dloadusb_interface_type active_interface = INTERFACE_UNKNOWN;
#endif
#endif

static const struct
{
  void (*init)(void);
  void (*tx_byte)(byte chr);
  int  (*rx_byte)(void);
  void (*drain)(void);
} uart_fn_tbl = 
{
  /* UART3 takes precedence over UART1 and pretends it is UART1. If the UART
   * features are fixed properly, can consider supporting both UARTs */
#ifdef FEATURE_THIRD_UART
  dload_uart3_init,
  uart3_transmit_byte,
  uart3_receive_byte,
  uart3_drain
#else
  dload_uart_init,
  uart_transmit_byte,
  uart_receive_byte,
  uart_drain
#endif /* FEATURE_THIRD_UART */
};


/*-------------------------------------------------------------------------*/

   /* Other packets must be built up on the fly.  This data type and the
      associated macros support dynamic construction of a packet.  */

/* According to the protocol spec, a version string can be at most 20 bytes */
#define  MAX_VERSTRING_LEN    (20)

#define  ROOM_FOR_CRC   (4)         /* Allow this much room for the CRC,
                                       including worst-case expansion due
                                       to byte-stuffing */
#define  ROOM_FOR_FLAG  (1)         /* Allow this much room for trailing flag */

/* Max length for a memory debug packet is 2048 bytes, added in version 6 */
#define  MAX_MEM_DEBUG_LEN    (2048)

/* With the memory debug feature, the max data length is 2KB, plus 1 byte for
 * for command, 4 bytes for adress and 2 bytes for length of the data
 */
#ifdef FEATURE_DLOAD_MEM_DEBUG
#define  MAX_RESPONSE_LEN  (4096)  // max HDLC packet size
#else
/* The buffer size for response packets.  Based on the longest possible
   packet, the Software Version acknowledgement. */
#define  MAX_RESPONSE_LEN  (1+2+MAX_VERSTRING_LEN+ROOM_FOR_CRC+ROOM_FOR_FLAG)
                           /* 1 = command code
                              2 = fixed fields of packet, version and len */
#endif
typedef struct
{
  uint32      length;                 /* Length of packet so far */
  uint32      broken;                 /* Set if the packet can't be built */
  byte        buf[MAX_RESPONSE_LEN];  /* The packet under construction */
  } pkt_buffer_type;


#if 0
typedef PACKED struct
  {
  word      length;                 /* Length of packet so far */
  boolean   broken;                 /* Set if the packet can't be built */
  byte      buf[MAX_RESPONSE_LEN];  /* The packet under construction */
  } pkt_buffer_type;
#endif

/* Define the data structure in ZI section so we don't overflow the stack */
pkt_buffer_type params_buf;




/*===========================================================================

MACRO START_BUILDING_PACKET

DESCRIPTION
  This macro initializes the process of dynamically building a packet.

PARAMETERS
  pkt     A pkt_buffer_type struct in which the packet will be built.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  pkt is evaluated twice within this macro.
  This macro is not an expression, nor is it a single statement.  It
  must be called with a trailing semicolon.

===========================================================================*/

#define  START_BUILDING_PACKET(pkt)          \
               pkt.length = 0;               \
               pkt.broken = FALSE


/*===========================================================================

MACRO ADD_BYTE_TO_PACKET

DESCRIPTION
  This macro adds a single byte to a packet being built dynamically.

PARAMETERS
  pkt     A pkt_buffer_type struct in which the packet will be built.
  val     The byte to be added to the packet.

DEPENDENCIES
  START_BUILDING_PACKET must have been called on pkt before calling
  this macro.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

#define  ADD_BYTE_TO_PACKET(pkt,val)         \
               add_byte_to_packet(&pkt, val)


/*===========================================================================

MACRO ADD_WORD_TO_PACKET

DESCRIPTION
  This macro adds a word (most significant byte first) to a packet
  being built dynamically.

PARAMETERS
  pkt     A pkt_buffer_type struct in which the packet will be built.
  val     The word to be added to the packet.

DEPENDENCIES
  START_BUILDING_PACKET must have been called on pkt before calling
  this macro.

RETURN VALUE
  None.

SIDE EFFECTS
  Each argument is evaluated twice within this macro.
  This macro is not an expression, not is it a single statement.  It
  must be called with a trailing semicolon.

===========================================================================*/

#define  ADD_WORD_TO_PACKET(pkt,val)         \
  /*lint -e778 val or (val >> 8) may evaluate to zero. */                    \
  add_byte_to_packet(&pkt, (byte)((val >> 8) & 0xFF)); /* high byte */ \
  add_byte_to_packet(&pkt, (byte)(val & 0xFF))         /* low  byte */ \
  /*lint +e778 */

/*===========================================================================

MACRO ADD_DWORD_TO_PACKET

DESCRIPTION
  This macro adds a dword (most significant byte first) to a packet
  being built dynamically.

PARAMETERS
  pkt     A pkt_buffer_type struct in which the packet will be built.
  val     The word to be added to the packet.

DEPENDENCIES
  START_BUILDING_PACKET must have been called on pkt before calling
  this macro.

RETURN VALUE
  None.

SIDE EFFECTS
  Each argument is evaluated twice within this macro.
  This macro is not an expression, not is it a single statement.  It
  must be called with a trailing semicolon.

===========================================================================*/

#define  ADD_DWORD_TO_PACKET(pkt,val)         \
  /*lint -e778 val or (val >> 8) may evaluate to zero. */                  \
  add_byte_to_packet(&pkt, (byte)((val >> 24) & 0xFF)); /* byte 3 */ \
  add_byte_to_packet(&pkt, (byte)((val >> 16) & 0xFF)); /* byte 2 */ \
  add_byte_to_packet(&pkt, (byte)((val >> 8) & 0xFF));  /* byte 1 */ \
  add_byte_to_packet(&pkt, (byte)(val & 0xFF))          /* byte 0 */ \
  /*lint +e778 */

/*===========================================================================

MACRO ADD_TARGET_DWORD_TO_PACKET

DESCRIPTION
  This macro adds a dword (least significant byte first) to a packet
  being built dynamically.

PARAMETERS
  pkt     A pkt_buffer_type struct in which the packet will be built.
  val     The word to be added to the packet.

DEPENDENCIES
  START_BUILDING_PACKET must have been called on pkt before calling
  this macro.

RETURN VALUE
  None.

SIDE EFFECTS
  Each argument is evaluated twice within this macro.
  This macro is not an expression, not is it a single statement.  It
  must be called with a trailing semicolon.

===========================================================================*/

#define  ADD_TARGET_DWORD_TO_PACKET(pkt,val)         \
  /*lint -e778 val or (val >> 8) may evaluate to zero. */                  \
  add_byte_to_packet(&pkt, (byte)(val & 0xFF));         /* byte 0 */ \
  add_byte_to_packet(&pkt, (byte)((val >> 8) & 0xFF));  /* byte 1 */ \
  add_byte_to_packet(&pkt, (byte)((val >> 16) & 0xFF)); /* byte 2 */ \
  add_byte_to_packet(&pkt, (byte)((val >> 24) & 0xFF))  /* byte 3 */ \
  /*lint +e778 */



/*===========================================================================

MACRO ADD_CRC_TO_PACKET

DESCRIPTION
  This macro adds a word (LEAST significant byte first) to a packet
  being built dynamically.  This should only be used for the CRC,
  since other words are supposed to be sent most significant byte
  first.

PARAMETERS
  pkt     A pkt_buffer_type struct in which the packet will be built.
  val     The word to be added to the packet.

DEPENDENCIES
  START_BUILDING_PACKET must have been called on pkt before calling
  this macro.

RETURN VALUE
  None.

SIDE EFFECTS
  Each argument is evaluated twice within this macro.
  This macro is not an expression, not is it a single statement.  It
  must be called with a trailing semicolon.

===========================================================================*/

#define  ADD_CRC_TO_PACKET(pkt,val)         \
  add_byte_to_packet(&pkt, (byte)(val & 0xFF));        /* low  byte */ \
  add_byte_to_packet(&pkt, (byte)((val >> 8) & 0xFF))  /* high byte */


/*===========================================================================

MACRO ADD_STUFF_TO_PACKET

DESCRIPTION
  This macro adds an arbitrary buffer of bytes to a packet being built
  dynamically.

PARAMETERS
  pkt     A pkt_buffer_type struct in which the packet will be built.
  data    A pointer to byte (or array of bytes) containing the data to be added
  len     The number of bytes to be added to the packet.

DEPENDENCIES
  START_BUILDING_PACKET must have been called on pkt before calling
  this macro.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

#define  ADD_STUFF_TO_PACKET(pkt,data,len)   \
               /*lint -e717 Yes, this is a do...while(0). */\
               do {                                         \
               int   stuff_i;                               \
                                                            \
               for (stuff_i=0; stuff_i < len; stuff_i++)    \
                 {                                          \
                 add_byte_to_packet(&pkt, data[stuff_i]);   \
                 }                                          \
               } while (0)                                  \
               /*lint +e717 */


/*===========================================================================

MACRO ADD_STRING_TO_PACKET

DESCRIPTION
  This macro adds a text string (null terminated) to a packet being built
  dynamically.  If the string is not null terminated or is longer than 20
  characters, only the first 20 characters are added to the packet.

PARAMETERS
  pkt     A pkt_buffer_type struct in which the packet will be built.
  str     The string to be added to the packet.

DEPENDENCIES
  START_BUILDING_PACKET must have been called on pkt before calling
  this macro.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

#define  ADD_STRING_TO_PACKET(pkt,str)       \
               /*lint -e717 Yes, this is a do...while(0). */ \
               do {                                          \
               byte count = 0;                               \
               byte stopping_point;                          \
                                                             \
           stopping_point = dload_str_len(str);          \
               while (count < stopping_point)                \
                 add_byte_to_packet(&pkt, str[count++]);     \
               } while (0)                                   \
               /*lint -e717 */


/*===========================================================================

MACRO FINISH_BUILDING_PACKET

DESCRIPTION
  This macro completes the process of building a packet dynamically.
  It just calls a function to do the work.

PARAMETERS
  pkt     A pkt_buffer_type struct in which the packet has been built.

DEPENDENCIES
  START_BUILDING_PACKET must have been called on pkt before calling
  this macro.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

#define  FINISH_BUILDING_PACKET(pkt)         \
               finish_building_packet(&pkt)


/*-------------------------------------------------------------------------*/

/* Mask for CRC-16 polynomial:
**
**      x^16 + x^12 + x^5 + 1
**
** This is more commonly referred to as CCITT-16.
** Note:  the x^16 tap is left off, it's implicit.
*/
#define CRC_16_L_POLYNOMIAL     0x8408

/* Seed value for CRC register.  The all ones seed is part of CCITT-16, as
** well as allows detection of an entire data stream of zeroes.
*/
#define CRC_16_L_SEED           0xFFFF

/* Residual CRC value to compare against return value of a CRC_16_L_STEP().
** Use CRC_16_L_STEP() to calculate a 16 bit CRC, complement the result,
** and append it to the buffer.  When CRC_16_L_STEP() is applied to the
** unchanged entire buffer, and complemented, it returns CRC_16_L_OK.
** That is, it returns CRC_16_L_OK_NEG.
*/
#define CRC_16_L_OK             0x0F47
#define CRC_16_L_OK_NEG         0xF0B8



/*===========================================================================

MACRO CRC_16_L_STEP

DESCRIPTION
  This macro calculates one byte step of an LSB-first 16-bit CRC.
  It can be used to produce a CRC and to check a CRC.

PARAMETERS
  xx_crc  Current value of the CRC calculation, 16-bits
  xx_c    New byte to figure into the CRC, 8-bits

DEPENDENCIES
  None

RETURN VALUE
  The new CRC value, 16-bits.  If this macro is being used to check a
  CRC, and is run over a range of bytes, the return value will be equal
  to CRC_16_L_OK_NEG if the CRC checks correctly according to the DMSS
  Async Download Protocol Spec.

SIDE EFFECTS
  xx_crc is evaluated twice within this macro.

===========================================================================*/

extern word crc_16_l_table[];       /* Extern for macro (global) */

#define CRC_16_L_STEP(xx_crc,xx_c) \
  (((xx_crc) >> 8) ^ crc_16_l_table[((xx_crc) ^ (xx_c)) & 0x00ff])


/*===========================================================================

FUNCTION add_byte_to_packet

DESCRIPTION
  This function adds a single byte to a packet that is being built
  dynamically.  It takes care of byte stuffing and checks for buffer
  overflow.

  This function is a helper function for the packet building macros
  and should not be called directly.

DEPENDENCIES
  The START_BUILDING_PACKET() macro should have been called on the
  packet buffer before calling this function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void add_byte_to_packet
(
  pkt_buffer_type  *pkt,
    /* Structure containing the packet being built */

  const byte        val
    /* The byte to be added to the packet */
)
{

  if (pkt->broken != FALSE)   /* If the packet is broken already, */
    {
    return;                  /* Don't do anything. */
    }

           /* Check if the byte needs escaping for transparency. */
  if (val == ASYNC_HDLC_FLAG || val == ASYNC_HDLC_ESC)
    {
                              /* Check for an impending overflow. */
    if (pkt->length+2+ROOM_FOR_CRC+ROOM_FOR_FLAG >= MAX_RESPONSE_LEN)
      {
      pkt->broken = TRUE;     /* Overflow.  Mark this packet broken. */
      return;
      }

           /* No overflow.  Escape the byte into the buffer. */
    pkt->buf[pkt->length++] = ASYNC_HDLC_ESC;
    pkt->buf[pkt->length++] = val ^ (byte)ASYNC_HDLC_ESC_MASK;
    }

  else     /* Byte doesn't need escaping. */
    {
                              /* Check for an impending overflow. */
    if (pkt->length+1+ROOM_FOR_CRC+ROOM_FOR_FLAG >= MAX_RESPONSE_LEN)
      {
      pkt->broken = TRUE;     /* Overflow.  Mark this packet broken. */
      return;
      }

           /* No overflow.  Place the byte into the buffer. */
    pkt->buf[pkt->length++] = val;
    }

}/* add_byte_to_packet() */


/*===========================================================================

FUNCTION finish_building_packet

DESCRIPTION
  This function completes the process of building a packet dynamically.
  If all is well, it adds the CRC and a trailing flag to the buffer.
  If an error has been encountered in building the packet, it substitutes
  a NAK packet for whatever has been built.

  This function is a helper function for the packet building macros
  and should not be called directly.

DEPENDENCIES
  The START_BUILDING_PACKET() macro should have been called on the
  packet buffer before calling this function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void finish_building_packet
(
  pkt_buffer_type  *pkt
    /* Structure containing the packet being built */
)
{
  word  crc;
    /* Cyclic Redundancy Check for the packet we've built. */

  word  i;
    /* Index for scanning through the packet, computing the CRC. */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (pkt->broken == FALSE)
    {
    /* Compute the CRC for all the bytes in the packet. */
    crc = CRC_16_L_SEED;
    for (i=0; i < pkt->length; i++)
       {
       /* According to the DMSS Download Protocol ICD, the CRC should only
        * be run over the raw data, not the escaped data, so since we
        * escaped the data as we built it, we have to back out any escapes
        * and uncomplement the escaped value back to its original value */
       if (pkt->buf[i] != ASYNC_HDLC_ESC)
         {
         crc = CRC_16_L_STEP(crc, (word) pkt->buf[i]);
         }
       else
         {
         i++;
         crc = CRC_16_L_STEP(crc,
                             (word)(pkt->buf[i] ^ (byte)ASYNC_HDLC_ESC_MASK));
         }
       }
    crc ^= CRC_16_L_SEED;

    ADD_CRC_TO_PACKET(*pkt,crc);             /* Add the CRC to the packet */

    pkt->buf[pkt->length] = ASYNC_HDLC_FLAG;  /* Add a flag to the packet.*/
                                              /* This can't use the regular
                                                 add_byte_to_packet() function
                                                 because it's a flag. */
    }
  else
    {
    (void) memcpy((void*)pkt->buf, rsp_nak_invalid_len,
             sizeof(rsp_nak_invalid_len));    /* Substitute a NAK */
    }

}/* finish_building_packet() */


/*===========================================================================

FUNCTION transmit_byte

DESCRIPTION
  This function transmits a byte through the UART/USB interface.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The watchdog may be reset.

===========================================================================*/
static void transmit_byte
(
  byte data
)
{
#if (defined(FEATURE_USB) || defined(FEATURE_QHSUSB) || defined(FEATURE_HS_USB_BASIC))
  if (active_interface == INTERFACE_USB)
  {
#ifdef FEATURE_HS_USB_BASIC
    hsu_al_dload_transmit_byte(data);
#elif defined(FEATURE_QHSUSB)
    qhsusb_al_dload_transmit_byte(data);
#else
    dloadusb_transmit_byte(data);
#endif
  }
  else
  {
    uart_fn_tbl.tx_byte(data);
  }
#else

  uart_fn_tbl.tx_byte(data);
#endif /* !FEATURE_USB || FEATURE_HS_USB_BASIC */
}


/*===========================================================================

FUNCTION transmit_packet

DESCRIPTION
  This function transmits a packet response through the UART/USB.

DEPENDENCIES
  The packet must end with a flag.

RETURN VALUE
  None.

SIDE EFFECTS
  The watchdog may be reset.

===========================================================================*/

static void transmit_packet
(
  pkt_buffer_type  *pkt
    /* The packet to be transmitted. */
)
{
  /* Pointer into the packet being transmitted */
  const byte *data = (byte*) pkt->buf;

#if (defined(FEATURE_USB) || defined(FEATURE_QHSUSB) || defined(FEATURE_HS_USB_BASIC))
  if (active_interface == INTERFACE_UART1)
  {
    transmit_byte(ASYNC_HDLC_FLAG);      /* Supply the leading flag  */
  }
#else
  transmit_byte(ASYNC_HDLC_FLAG);      /* Supply the leading flag  */
#endif

  do
  {
    transmit_byte(*data);        /* Transmit bytes from the buffer */
  }
  while (*data++ != ASYNC_HDLC_FLAG); /* Until we've transmitted a flag */

#if (defined(FEATURE_USB) || defined(FEATURE_QHSUSB) || defined(FEATURE_HS_USB_BASIC))
  if (interface == INTERFACE_UNKNOWN)
  {
    active_interface = INTERFACE_UNKNOWN;
  }
#endif

} /* transmit_packet() */


/*===========================================================================

FUNCTION transmit_response

DESCRIPTION
  This function transmits a packet response through the UART/USB.
  This function supplies the entire packet, based on the type
  code passed in.

DEPENDENCIES
  Uses the response_table[] to find the packet to transmit.  This
  packet must be terminated by a flag.

RETURN VALUE
  None.

SIDE EFFECTS
  The watchdog may be reset.

===========================================================================*/

static void transmit_response
(
  response_code_type rsp
    /* Type of response to transmit */
)

{
  /* Pointer into the packet being transmitted */
  const byte *pkt;
  pkt = response_table[rsp];             /* Find the packet to transmit */
    
  transmit_byte(ASYNC_HDLC_FLAG);      /* Supply the leading flag  */

  do
  {
    transmit_byte(*pkt);         /* Transmit bytes from the buffer */
  }
  while (*pkt++ != ASYNC_HDLC_FLAG);  /* Until we've transmitted a flag */

#if (defined(FEATURE_USB) || defined(FEATURE_QHSUSB) || defined(FEATURE_HS_USB_BASIC))
  if (interface == INTERFACE_UNKNOWN)
  {
    active_interface = INTERFACE_UNKNOWN;
  }
#endif

} /* transmit_response() */

int get_active_interface (void)
{
    long count;
    int chr;
	
#ifdef FEATURE_HS_USB_BASIC
	hsu_al_dload_interface_type intf = INTERFACE_UNKNOWN;
#else
#if (defined(FEATURE_USB) || defined(FEATURE_QHSUSB))
	dloadusb_interface_type intf = INTERFACE_UNKNOWN;
#endif
#endif

		
  /* loop forever waiting for data */
  for (count = 0;;)
  {
    BOOTHW_KICK_WATCHDOG();                 /* Don't let the watchdog expire */
       
          /* Try USB */
#ifdef FEATURE_HS_USB_BASIC
          chr = hsu_al_dload_receive_byte();

#elif defined(FEATURE_QHSUSB)
          chr = qhsusb_al_dload_receive_byte();
#else
          chr = dloadusb_receive_byte();
#endif
          if (chr == USB_NO_CHAR)
          {
           chr = uartdm_get_interface_status();
		   if ((chr != UART_NO_CHAR) && (chr != UART_RX_ERR))
		   	{
		   	  intf = INTERFACE_UART1;
			  break;
		   	}
            
          }
          else /* USB Char found */
          {
            intf = INTERFACE_USB;
			break;
          }

		/* only loop a limited number of times, ~ 15 seconds, if there is 
       no active initial communications */
    if ((init_boot == TRUE) && (count++ > 400000))
    {
       
      /* Loop count is TBD */
      //break;
    }
       
 }
             
   return intf;
}





/*===========================================================================

FUNCTION get_next_char

DESCRIPTION
  This function receives the next byte using either the generic UART
  service uart_receive_byte or USB service dloadusb_receive_byte.

DEPENDENCIES
  None.

RETURN VALUE
  Word.

SIDE EFFECTS
  The watchdog is reset if polling times out.
===========================================================================*/

static int get_next_char
(
  void
)
{
  int chr = UART_TIMEOUT;
  long count;

  /* Get the next byte on the interface in use, if the interface (UART or 
     USB) has yet to be established then check both */
  
  /* loop forever waiting for data */
  for (count = 0;;)
  {
    BOOTHW_KICK_WATCHDOG();                 /* Don't let the watchdog expire */
#if (defined(FEATURE_USB) || defined(FEATURE_QHSUSB) || defined(FEATURE_HS_USB_BASIC))
    switch (active_interface)
    {
      case INTERFACE_UNKNOWN:
      {
        /* Try UART */
        chr = uart_fn_tbl.rx_byte();

        if ((chr == UART_RX_ERR) || (chr == UART_NO_CHAR))
        {
          /* Try USB */
#ifdef FEATURE_HS_USB_BASIC
          chr = hsu_al_dload_receive_byte();
#elif defined(FEATURE_QHSUSB)
          chr = qhsusb_al_dload_receive_byte();
#else
          chr = dloadusb_receive_byte();
#endif
          if (chr == USB_NO_CHAR)
          {
            chr = UART_NO_CHAR;
          }
          else
          {
            active_interface = INTERFACE_USB;
          }
        }
        else
        {
          active_interface = INTERFACE_UART1;
        }
        break;
      }

      case INTERFACE_UART1:
      {
        chr = uart_fn_tbl.rx_byte();  /* Get next character */
        break;
      }

      case INTERFACE_USB:
      {
#ifdef FEATURE_HS_USB_BASIC
        chr = hsu_al_dload_receive_byte();
#elif defined(FEATURE_QHSUSB)
        chr = qhsusb_al_dload_receive_byte();
#else
        chr = dloadusb_receive_byte();
#endif

        if (chr == USB_NO_CHAR)
        {
          chr = UART_NO_CHAR;
        }
        break;
      }
    }
#else
    chr = uart_fn_tbl.rx_byte();  /* Get next character */
#endif
    if ((chr != UART_RX_ERR) && (chr != UART_NO_CHAR))
    {
      /* Found a byte */
      break;
    }

    /* only loop a limited number of times, ~ 15 seconds, if there is 
       no active initial communications */
    if ((init_boot == TRUE) && (count++ > 400000))
    {
      chr = UART_TIMEOUT;
      /* Loop count is TBD */
      //break;
    }
  }
  return chr;
}


/*===========================================================================

FUNCTION rcv_packet

DESCRIPTION
  This function receives a complete packet.
  It takes care of the async-HDLC state
  machine, enforces a minimum packet length of 1 byte plus CRC, and
  checks the CRC on the fly.

DEPENDENCIES
  Uses the crc table.

RETURN VALUE
  Once a good packet is received, returns its length including
  but not including flags or byte stuffing escapes.

SIDE EFFECTS
  If a NAK-able packet error is detected, this function generates a
  suitable NAK response and does not return until a good packet is
  received.

  The watchdog is reset.
===========================================================================*/

static word rcv_packet
(
  byte *buf
    /* Pointer to the packet buffer for receiving the packet */
)

{

  enum
    {
    HDLC_HUNT_FOR_FLAG,     /* Waiting for a flag to start a packet       */
    HDLC_GOT_FLAG,          /* Have a flag, expecting the packet to start */
    HDLC_GATHER,            /* In the middle of a packet                  */
    HDLC_PACKET_RCVD        /* Now have received a complete packet        */
    } state;
    /* State variable for decoding async HDLC */

  int   chr;
    /* Current character being received */

  word  len=0;
    /* Length of packet collected so far */

  word  crc=0;
    /* Cyclic Redundancy Check, computed as we go. */

  /*lint -esym(644,len,crc) */
  /* Lint can't tell that the state machine guarantees that
     we initialize len and crc before use */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 /* Look at characters and try to find a valid async-HDLC packet of
  ** length at least MIN_PACKET_LEN with a valid CRC.
  ** Keep looking until we find one. */

  for (state = HDLC_HUNT_FOR_FLAG; state != HDLC_PACKET_RCVD; /* nil */)
  {
    BOOTHW_KICK_WATCHDOG();                 /* Don't let the watchdog expire */

    chr = get_next_char();       /* Get next character (wait for it) */

    if (chr == UART_RX_ERR)          /* If it's an error ... */
    {
      state = HDLC_HUNT_FOR_FLAG;             /* Start over. */
      continue;
    }

    /* communication timed out */
    if (chr == UART_TIMEOUT)
    {
      boot_powerdown_entry();
    }

    switch(state)                  /* Process according to which state */
    {
      /*lint -esym(788,HDLC_PACKET_RCVD)  No need to deal with HDLC_PACKET_RCVD
      since we're in a loop that guarantees we're not in that state. */
      case HDLC_HUNT_FOR_FLAG:         /* We're looking for a flag ... */
        if (chr == ASYNC_HDLC_FLAG)    /*   and we got one ...         */
        {
          state = HDLC_GOT_FLAG;       /*   so go on to the next step. */
        }
        break;

      case HDLC_GOT_FLAG:              /* Had a flag, now expect a packet */
        if (chr == ASYNC_HDLC_FLAG)    /* Oops, another flag.  No change. */
        {
          break;
        }
        else
        {                            /* Ah, we can really begin a packet */
          len = 0;                     /* The packet starts out empty      */
          crc = CRC_16_L_SEED;         /* and the CRC in its initial state */
          state = HDLC_GATHER;         /* and we begin to gather a packet  */
          /* Fall through */           /*   (starting with this byte)      */
        }

      case HDLC_GATHER:                   /* We're gathering a packet      */
        if (chr == ASYNC_HDLC_FLAG)       /* We've reached the end         */
        {
          if (len < MIN_PACKET_LEN)       /* Reject any too-short packets  */
          {
            transmit_response(NAK_EARLY_END);            /* Send NAK       */
            state = HDLC_HUNT_FOR_FLAG;                  /* Start over     */
          }
          else if (crc != CRC_16_L_OK_NEG)      /* Reject any with bad CRC */
          {
            transmit_response(NAK_INVALID_FCS);          /* Send NAK       */
            state = HDLC_HUNT_FOR_FLAG;                  /* Start over     */
          }
          else                                 /* Yay, it's a good packet! */
          {
            state = HDLC_PACKET_RCVD;                    /* Done for now   */
            init_boot = FALSE;
          }
          break;           /* However it turned out, this packet is over.  */
        }

        /* It wasn't a flag, so we're still inside the packet. */
        if (chr == ASYNC_HDLC_ESC)               /* If it was an ESC       */
        {
          chr = get_next_char();       /* Get next character (wait for it) */

          if (chr == UART_RX_ERR)                /* If there was an error, */
          {
          state = HDLC_HUNT_FOR_FLAG;          /* Start over             */
          break;
          }

          chr ^= ASYNC_HDLC_ESC_MASK;            /* Otherwise, de-mask it  */
          /* No break; process the de-masked byte normally */
        }

        if (len >= MAX_PACKET_LEN)               /* Make sure there's room */
        {
          transmit_response(NAK_TOO_LARGE);      /* Oops, buffer too full  */
          state = HDLC_HUNT_FOR_FLAG;            /* Start over             */
        }
        else
        {
          buf[len++] = (byte) chr;                   /* Add byte to buffer */
          crc = CRC_16_L_STEP(crc, (word) chr);      /* Update the CRC     */
        }
        break;

      default:       /* Shouldn't happen with an enum, but for safety ...  */
        state = HDLC_HUNT_FOR_FLAG;                  /* Start over         */
        break;
    }/* switch on state */

  }/* for (packet not found) */

  return len;
} /* rcv_packet() */


/*===========================================================================

FUNCTION write_cmd

DESCRIPTION
  This function processes a write command packet.  Pending a valid address
  range, the data is written to RAM.  Note that an offset needs to be added
  to the address to move it to the target RAM location.  The Intel MCS86 hex
  file format does not support the address range for the ARM, therefore, it
  is created with an implicit start address of 0, which then needs to be
  offset into the intended address range.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void write_cmd
(
  byte *cmd_buf,
    /* Pointer to the received command packet */

  word  cmd_len
    /* Number of bytes received in the command packet */
)

{
  dword                addr = 0;    /* destination address for write */
  byte                *w_addr;  /* offset address */
  byte                *start;   /* start location for write - includes offset */
  byte                *data;    /* pointer to bytes of data */
  word                 len = 0 ;     /* number of bytes to write */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cmd_len < WRITE_SIZ)       /* Make sure at least the header arrived  */
    {
    transmit_response(NAK_EARLY_END);        /* Nope, packet ended early   */
    return;
    }

#ifndef FEATURE_HWTC /* 10/23/01 lcl */
  /* Verify that a valid security code was previously received */
  if (!sec_code_unlocked)
    {
    transmit_response(NAK_NO_SEC_CODE);      /* It wasn't, so complain     */
    return;
    }
#endif /* FEATURE_HWTC */

  B_PTR(addr)[3] = 0;          /* Extract destination address from packet  */
  B_PTR(addr)[2] = cmd_buf[1];
  B_PTR(addr)[1] = cmd_buf[2];
  B_PTR(addr)[0] = cmd_buf[3];

  B_PTR(len)[1]  = cmd_buf[4];      /* Extract write length from packet    */
  B_PTR(len)[0]  = cmd_buf[5];

  if (len + WRITE_SIZ != cmd_len)  /* Packet must be exactly the right len */
    {
    transmit_response(NAK_INVALID_LEN);      /* It wasn't, so complain     */
    return;
    }

  /* convert addresses to RAM address range */
  w_addr = (byte *) (addr+DLOAD_OFFSET);
  start = (byte *) (addr+DLOAD_OFFSET);

  /* check address range limits */
  #if (DLOAD_BASE > 0)
  if (w_addr < (byte*)DLOAD_BASE || (w_addr+len) >= (byte*)DLOAD_LIMIT)
  {
     transmit_response(NAK_INVALID_DEST);
     return;
  }
  #else
  if ((w_addr+len) >= (byte*)DLOAD_LIMIT)
  {
     transmit_response(NAK_INVALID_DEST);
     return;
  }
  #endif

  /* start of data portion of packet */
  data = (byte *) (cmd_buf+6);

  /* perform the write to memory */
  while (w_addr <= (start + len)) {
     *w_addr++ = *data++;
  }

  /* everything ok */
  transmit_response(ACK);

} /* write_cmd() */


/*===========================================================================

FUNCTION erase_cmd

DESCRIPTION
  This function processes an erase command packet.  Pending a valid address
  range, the specified area is erased from RAM.  Note that an offset needs to be added
  to the address to move it to the target RAM location.  The Intel MCS86 hex
  file format does not support the address range for the ARM, therefore, it
  is created with an implicit start address of 0, which then needs to be
  offset into the intended address range.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void erase_cmd
(
  byte *cmd_buf,
    /* Pointer to the received command packet */

  word  cmd_len
    /* Number of bytes received in the command packet */
)

{
  dword                addr = 0;     /* destination address */
  dword                len = 0;      /* number of bytes to erase */
  byte                 *e_addr;  /* offset address */
  byte                 *start;   /* start location for erase */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cmd_len != ERASE_SIZ)          /* Make sure the whole packet arrived */
    {
      transmit_response(NAK_EARLY_END);          /* Nope, packet ended early */
      return;
    }

#ifndef FEATURE_HWTC  /* 10/23/01 lcl */
  /* Verify that a valid security code was previously received */
  if (!sec_code_unlocked)
    {
    transmit_response(NAK_NO_SEC_CODE);      /* It wasn't, so complain     */
    return;
    }
#endif /* FEATURE_HWTC */

  B_PTR(addr)[3] = 0;           /* Extract destination address from packet */
  B_PTR(addr)[2] = cmd_buf[1];
  B_PTR(addr)[1] = cmd_buf[2];
  B_PTR(addr)[0] = cmd_buf[3];

  B_PTR(len)[3]  = 0;           /* Extract write length from packet        */
  B_PTR(len)[2]  = cmd_buf[4];
  B_PTR(len)[1]  = cmd_buf[5];
  B_PTR(len)[0]  = cmd_buf[6];

  /* convert addresses to RAM address range */
  e_addr = (byte *) (addr+DLOAD_OFFSET);
  start = (byte *) (addr+DLOAD_OFFSET);

  /* check address range limits */
  #if (DLOAD_BASE > 0)
  if (e_addr < (byte*)DLOAD_BASE || (e_addr+len) >= (byte*)DLOAD_LIMIT)
  {
      transmit_response(NAK_INVALID_DEST);
      return;
  }
  #else
  if ((e_addr+len) >= (byte*)DLOAD_LIMIT)
  {
      transmit_response(NAK_INVALID_DEST);
      return;
  }
  #endif

  /* erase the memory */
  while (e_addr <= (start + len)) {
    *e_addr++ = 0x00;
  }

  /* everything ok */
  transmit_response(ACK);

} /* erase_cmd() */


/*===========================================================================

FUNCTION go_cmd


DESCRIPTION
  This function processes a GO command packet, which requires the
  transfer of execution control to a specified address.

DEPENDENCIES
  None.

RETURN VALUE
  Generally, this function does not return, since the entry point of
  the downloaded module is not supposed to return.  However, if the
  called code does return, this function returns (void).

SIDE EFFECTS
  None.

===========================================================================*/
extern void jump_armprg(uint32 *); // asm function in boot_reset_handler.s

static void go_cmd
(
  byte *cmd_buf,
    /* Pointer to the received command packet */

  word  cmd_len
    /* Number of bytes received in the command packet */
)

{
  void  (* jump_point)(void) = NULL ;  /* address of the routine to run */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure the whole packet arrived. */
  if (cmd_len != GO_SIZ)
    {
    transmit_response(NAK_EARLY_END);  /* Nope, packet ended early         */
    return;
    }

#ifndef FEATURE_HWTC /* 10/23/01 lcl */
  /* Verify that a valid security code was previously received */
  if (!sec_code_unlocked)
    {
    transmit_response(NAK_NO_SEC_CODE);      /* It wasn't, so complain     */
    return;
    }
#endif /* FEATURE_HWTC */

  /* Construct a pointer to the destination address */
  B_PTR(jump_point)[3] = cmd_buf[1];   /* Extract code segment from packet */
  B_PTR(jump_point)[2] = cmd_buf[2];
  B_PTR(jump_point)[1] = cmd_buf[3];   /* Extract code offset from packet  */
  B_PTR(jump_point)[0] = cmd_buf[4];

#ifdef FEATURE_DLOAD_SECURITY
  /* Authenticate the downloaded flash programmer. */
  #ifdef BOOT_SBLDLOAD_H
    jump_point =
      sbl_dload_auth_flash_prog( (mi_boot_image_header_type*) jump_point );
  #else
    jump_point =
      oemsbl_dload_auth_flash_prog( (mi_boot_image_header_type*) jump_point );
  #endif
  if ( jump_point == NULL )
  {
    /* Authentication of the flash programmer failed. */
    transmit_response( NAK_VERIFY_FAILED );

    return;
  }
#endif /* FEATURE_DLOAD_SECURITY */
  
  transmit_response(ACK);          /* Have to send the ACK before GOing. */

  /* Check to make sure that the diag task is communicating to the host on the
     USB port. If so, use the USB interface to drain the transmit buffer. If not,
     we will use the UART interface uart_drain */

#if (defined(FEATURE_USB) || defined(FEATURE_QHSUSB) || defined(FEATURE_HS_USB_BASIC))
  if (active_interface == INTERFACE_USB)
  {
    /*  Wait for host to send IN token to obtain data. */
#ifdef FEATURE_HS_USB_BASIC
    hsu_boot_dload_armprg_transition();
#elif defined(FEATURE_QHSUSB)
    qhsusb_boot_dload_armprg_transition();
#else
    dloadusb_drain();
#endif
  }
  else
  {
    /* De-confiure the USB endpoints so it is obvious to the flash
       programmer that the serial interface is being used */
#ifdef FEATURE_HS_USB_BASIC
    hsu_al_dload_shutdown();
#elif defined(FEATURE_QHSUSB)
    qhsusb_al_dload_shutdown();
#else
    dloadusb_shutdown();
#endif
    
    uart_fn_tbl.drain();              /* Make sure the response gets out */
  }
#else
    uart_fn_tbl.drain();              /* Make sure the response gets out */

  /* Enable USB xtal for flash programming code so it can check USB */
  clk_regime_usb_xtal_on();
#endif

  mmu_invalidate_instruction_cache();
  mmu_invalidate_data_cache();
  cache_mmu_disable();
  mmu_disable();

  /* Call asm function in boot_reset_handler.s which will jump to the 
   * armprg start address.
   * This was done because the ARM9 compiler refuses to create jump 
   * code from the old C code that was here.
   */
  jump_armprg ((uint32 *)jump_point);


  /* It really shouldn't return, but just in case it does we will
     continue with packet processing and hope everything is still
     properly set up for that. */

} /* go_cmd() */

#ifndef FEATURE_HWTC  /* 10/23/01 lcl */

/*============================================================================

 FUNCTION UNLOCK_CMD

 DESCRIPTION
   Process an unlock command.

   If the security code contained in the unlock command does not match
   the security code stored in flash memory, the phone will be
   powered-down.  If the codes do match, then secure download functions
   will be unlocked.

 FORMAL ARGUMENTS
   None

 DEPENDENCIES
   None

 RETURN VALUE
   An error flag is returned: zero indicates no errors occurred,
   one indicates that an error did occur.  However, if an incorrect
   security code is contained in the unlock command, the phone will
   be powered-down and this function will not return.

 SIDE EFFECTS
   None

============================================================================*/
static void unlock_cmd
(
  byte *cmd_buf,
    /* Pointer to the received command packet */

  word  cmd_len
    /* Number of bytes received in the command packet */
)
{
  // extern unsigned long long chk_security_code_ptr; - (included diagi.h and
  // commenting out the extern definition)
  byte *security_code;
  int  i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cmd_len < UNLOCK_SIZ)       /* Make sure at least the header arrived  */
    {
    transmit_response(NAK_EARLY_END);        /* Nope, packet ended early   */
    return;
    }

  /* If secure functions are already unlocked, bypass
     validity check of received security code */
  if (sec_code_unlocked)
    {
      transmit_response(ACK);
      return;
    }

  /* Compare 64-bit security code in the received packet
     with the security code stored in flash */

  /* point to beginning of security code */
  security_code = (byte *) &chk_security_code_ptr;

  /* code is 64 bits long */

  /* scan through security code */

  for (i = 0; i < 8; i++)
    {
      /* received invalid security code */
      /* NAK the command and power-off the phone */
      if (security_code[i] != cmd_buf[i+1])
        {
          sec_code_unlocked = FALSE;
          transmit_response(NAK_BAD_SEC_CODE);

          /* now powerdown the phone */
          boot_hw_powerdown();

          return;
        }
    }

  /* The received security code matches the stored code, so
     set SECURITY_CODE_STATUS to unlock secure operations */
  sec_code_unlocked = TRUE;

} /* unlock_cmd() */
#endif /* FEATURE_HWTC */



/*===========================================================================

FUNCTION nop_cmd

DESCRIPTION
  This function processes a no-op command packet.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void nop_cmd(void)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* (The whole packet must have arrived, since it's only one byte long.) */
  transmit_response(ACK);          /* Nothing can go wrong.  Send an ACK. */

} /* nop_cmd() */


/*===========================================================================

FUNCTION preq_cmd

DESCRIPTION
  This function processes a parameter request command packet.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void preq_cmd(void)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  word dload_mob_model;

  /* Model number of the phone hardware */
  word mob_model;

#ifdef BUILD_BOOT_CHAIN
  mob_model = MOB_MODEL_NOP_VAL;
#else
  extern word boot_mob_model;
  /* Copy the mob_model number out of flash */
  mob_model = boot_mob_model;

#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* (The whole packet must have arrived, since it's only one byte long.) */

  /* If the real value can't be determined, we use the unknown value code */
  if ((mob_model == MOB_MODEL_NOP_VAL) || (mob_model == MOB_MODEL_ERASED_VAL))
    dload_mob_model = MOB_MODEL_UNKNOWN_VAL;
  else 
    dload_mob_model = mob_model;  


  /* Build the packet. */
  START_BUILDING_PACKET(params_buf);
  ADD_BYTE_TO_PACKET(params_buf, CMD_PARAMS);
  ADD_BYTE_TO_PACKET(params_buf, PROTOCOL_ID);
  ADD_BYTE_TO_PACKET(params_buf, MIN_PROTOCOL_ID);
  ADD_WORD_TO_PACKET(params_buf, MAX_WRITE_SIZE);
  ADD_BYTE_TO_PACKET(params_buf, dload_mob_model);
  ADD_BYTE_TO_PACKET(params_buf, DUMMY);
  ADD_BYTE_TO_PACKET(params_buf, DUMMY);
  FINISH_BUILDING_PACKET(params_buf);

  /* Send the requested parameters response */
  transmit_packet(&params_buf);

  /* Call asm functions which will disable data cacheing and enable
   * instruction cache.  Merely turning off data cacheing will not
   * work, we have to disable all cacheing and then turn back on
   * instruction cacheing.
   */
  mmu_invalidate_data_cache();
  mmu_invalidate_instruction_cache();
  cache_mmu_disable();
  mmu_disable();
  mmu_enable_instruction_cache();
  
} /* preq_cmd() */


/*===========================================================================

FUNCTION reset_cmd

DESCRIPTION
  This function processes a reset command packet by resetting the
  phone (after the command is acknowledged).

DEPENDENCIES
  None.

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None.

===========================================================================*/

static void reset_cmd(void)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* (The whole packet must have arrived, since it's only one byte long.) */

  transmit_response(ACK);          /* Nothing can go wrong.  Send an ACK. */


  /* Check to make sure that the diag task is communicating to the host on the
     USB port. If so, use the USB interface to drain the transmit buffer. If not,
       we will use the UART interface uart_drain */

#if (defined(FEATURE_USB) || defined(FEATURE_QHSUSB) || defined(FEATURE_HS_USB_BASIC))
  if (active_interface == INTERFACE_USB)
  {
    /*  Wait for host to send IN token to obtain data. */
#ifdef FEATURE_HS_USB_BASIC
    hsu_al_dload_drain();
#elif defined(FEATURE_QHSUSB)
    qhsusb_al_dload_drain();
#else
    dloadusb_drain();
#endif
  }
  else
  {
    uart_fn_tbl.drain();             /* Make sure the response gets out */
  }
#else
    uart_fn_tbl.drain();             /* Make sure the response gets out */
#endif

  boot_hw_reset();
  
} /* reset_cmd() */


/*===========================================================================

FUNCTION dload_str_len

DESCRIPTION
  This function returns the number of characters in the given string, not
  counting the terminating NULL character.  The returned length is never
  more than 20, to accomodate a limitation of the dload protocol.

DEPENDENCIES
  The string should be NULL terminated.  If it is not, only the first 20
  characters will matter.

RETURN VALUE
  The length of the string, not counting the terminating NULL.  The value
  never exceeds 20.

SIDE EFFECTS
  None.

===========================================================================*/

static byte dload_str_len(const char* str)
{
  byte count = 0;
  char* ptr;

  /* count every non-null character, but don't go beyond 20 characters */
  for (ptr = (char*)str; *ptr != '\0' && count < 20; ptr++)
    count++;

  return count;
}

/*===========================================================================

FUNCTION verreq_cmd

DESCRIPTION
  This function processes a software version request command packet.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void verreq_cmd(void)
{


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* (The whole packet must have arrived, since it's only one byte long.) */

  START_BUILDING_PACKET(params_buf);
  ADD_BYTE_TO_PACKET(params_buf, CMD_VERRSP);

  /* mob_sw_rev is defined in mobile.c */
  ADD_BYTE_TO_PACKET(params_buf, dload_str_len(mob_sw_rev));
  ADD_STRING_TO_PACKET(params_buf, mob_sw_rev);
  FINISH_BUILDING_PACKET(params_buf);

  transmit_packet(&params_buf);
                                /* Send the requested version response */

} /* verreq_cmd() */


/*===========================================================================

FUNCTION pwroff_cmd

DESCRIPTION
  This function processes a Turn Phone Power Off command packet by
  turning off power to the phone (after the command is acknowledged).

DEPENDENCIES
  None.

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None.

===========================================================================*/
void pwroff_cmd(void)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* (The whole packet must have arrived, since it's only one byte long.) */

  transmit_response(ACK);          /* Nothing can go wrong.  Send an ACK. */


  /* Check to make sure that the diag task is communicating to the host on the
     USB port. If so, use the USB interface to drain the transmit buffer. If not,
     we will use the UART interface uart_drain */

#if (defined(FEATURE_USB) || defined(FEATURE_QHSUSB) || defined(FEATURE_HS_USB_BASIC))
  if (active_interface == INTERFACE_USB)
  {
    /*  Wait for host to send IN token to obtain data. */
#ifdef FEATURE_HS_USB_BASIC
    hsu_al_dload_drain();
#elif defined(FEATURE_QHSUSB)
    qhsusb_al_dload_drain();
#else
    dloadusb_drain();
#endif
  }
  else
  {
    uart_fn_tbl.drain();             /* Make sure the response gets out */
  }
#else
    uart_fn_tbl.drain();             /* Make sure the response gets out */
#endif

  /* now powerdown the phone */
  boot_hw_powerdown();

} /* pwroff_cmd() */


/*===========================================================================

FUNCTION write_32bit_cmd

DESCRIPTION
  This function processes a write command packet.  Pending a valid address
  range, the data is written to RAM.  
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void write_32bit_cmd
(
  byte *cmd_buf,
    /* Pointer to the received command packet */

  word  cmd_len
    /* Number of bytes received in the command packet */
)

{
  dword                addr = 0;    /* destination address for write */
  byte                *w_addr = NULL;  /* offset address */
  byte                *start = NULL;   /* start location for write - includes offset */
  byte                *data;    /* pointer to bytes of data */
  word                 len = 0;     /* number of bytes to write */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cmd_len < WRITE_32BIT_SIZ)       /* Make sure at least the header arrived  */
    {
    transmit_response(NAK_EARLY_END);        /* Nope, packet ended early   */
    return;
    }

#ifndef FEATURE_HWTC /* 10/23/01 lcl */
  /* Verify that a valid security code was previously received */
  if (!sec_code_unlocked)
    {
    transmit_response(NAK_NO_SEC_CODE);      /* It wasn't, so complain     */
    return;
    }
#endif /* FEATURE_HWTC */

  B_PTR(addr)[3] = cmd_buf[1];          /* Extract destination address from packet  */
  B_PTR(addr)[2] = cmd_buf[2];
  B_PTR(addr)[1] = cmd_buf[3];
  B_PTR(addr)[0] = cmd_buf[4];

  B_PTR(len)[1]  = cmd_buf[5];      /* Extract write length from packet    */
  B_PTR(len)[0]  = cmd_buf[6];

  if (len + WRITE_32BIT_SIZ != cmd_len)  /* Packet must be exactly the right len */
    {
    transmit_response(NAK_INVALID_LEN);      /* It wasn't, so complain     */
    return;
    }

  /* convert addresses to RAM address range */
  w_addr = (byte *) addr;
  start = (byte *) addr;

  #if (DLOAD_BASE > 0)
  /* check address range limits */
  if (w_addr < (byte*)DLOAD_BASE || (w_addr+len) >= (byte*)DLOAD_LIMIT)
  {
     transmit_response(NAK_INVALID_DEST);
     return;
  }
  #else
  /* check address range limits */
  if ((w_addr+len) >= (byte*)DLOAD_LIMIT)
  {
     transmit_response(NAK_INVALID_DEST);
     return;
  }
  #endif

  /* start of data portion of packet */
  data = (byte *) (cmd_buf+7);

  /* perform the write to memory */
  while (w_addr <= (start + len)) {
     *w_addr++ = *data++;
  }

  /* everything ok */
  transmit_response(ACK);

} /* write_32bit_cmd() */


/*===========================================================================

FUNCTION dload_switch_cmd

DESCRIPTION
  This function processes switch command.

DEPENDENCIES
  None.

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None.

===========================================================================*/
void dload_switch_cmd()
{
  /* Build the packet. */
  START_BUILDING_PACKET(params_buf);
  ADD_BYTE_TO_PACKET(params_buf, 0x3A);
  FINISH_BUILDING_PACKET(params_buf);

  /* Send the requested parameters response */
  transmit_packet(&params_buf);
}

/*===========================================================================

FUNCTION dload_dbg_dump_mem

DESCRIPTION
  This function transmits memory dump response packet specified in version 
  6 of the protocol.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dload_dbg_dump_mem(byte *cmd_buf)
{
#ifndef FEATURE_DLOAD_MEM_DEBUG
  /* if we don't support this feature, then return a NAK */
  transmit_response(NAK_OP_NOT_PERMITTED);
  return;
#else    
  uint32 addr = 0;
  uint16 len = 0;
  uint32 *data, i;

#ifndef FEATURE_HWTC 
  /* Verify that a valid security code was previously received */
  if (!sec_code_unlocked)
  {
    transmit_response(NAK_NO_SEC_CODE);      /* It wasn't, so complain     */
    return;
  }
#endif /* FEATURE_HWTC */
  
  /* Verify feature is supported */ 
  if(!dload_mem_debug_supported())
  {
    transmit_response(NAK_OP_NOT_PERMITTED);
    return;
  }

  B_PTR(addr)[3] = cmd_buf[1];          /* Extract address from packet */
  B_PTR(addr)[2] = cmd_buf[2];
  B_PTR(addr)[1] = cmd_buf[3];
  B_PTR(addr)[0] = cmd_buf[4];

  B_PTR(len)[1]  = cmd_buf[5];          /* Extract write length from packet */
  B_PTR(len)[0]  = cmd_buf[6];

  /* Make sure addr and len are both 32-bit word aligned by dropping lower bits
   */
  addr&=~3;
  len &=~3;

  /* Do an bound check to see if we are dumping valid memory regions */
  if(!dload_mem_debug_verify_addr(addr, (uint32)len))
  {
    transmit_response(NAK_INVALID_ADDR);
    return;
  }

  /* Start building response packet */
  START_BUILDING_PACKET(params_buf);
  ADD_BYTE_TO_PACKET(params_buf, CMD_MEM_READ_RESP);
  ADD_DWORD_TO_PACKET(params_buf, addr);
  ADD_WORD_TO_PACKET(params_buf, len);

  /* Now start reading memory 4 bytes at a time */
  data=(uint32 *)addr;
  for(i=0; i < len; i+=4)
  {
    ADD_TARGET_DWORD_TO_PACKET(params_buf, (*data));
    data++;
  }

  FINISH_BUILDING_PACKET(params_buf);

  /* Send the requested parameters response */
  transmit_packet(&params_buf);
#endif  
}


/*===========================================================================

FUNCTION dload_dbg_info_cmd

DESCRIPTION
  This function transmits the memory regions response packet specified in 
  version 6 of the protocol.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dload_dbg_info_cmd()
{
#ifndef FEATURE_DLOAD_MEM_DEBUG
  /* if we don't support this feature, then return a NAK */
  transmit_response(NAK_OP_NOT_PERMITTED);
  return;
#else
  uint32 i, num;

#ifndef FEATURE_HWTC 
  /* Verify that a valid security code was previously received */
  if (!sec_code_unlocked)
  {
    transmit_response(NAK_NO_SEC_CODE);      /* It wasn't, so complain     */
    return;
  }
#endif /* FEATURE_HWTC */

  /* Verify feature is supported */ 
  if(!dload_mem_debug_supported())
  {
    transmit_response(NAK_OP_NOT_PERMITTED);
    return;
  }
  /* Build the packet. */
  START_BUILDING_PACKET(params_buf);
  ADD_BYTE_TO_PACKET(params_buf, CMD_MEM_DEBUG_INFO);
  /*lint -e666*/
  ADD_WORD_TO_PACKET(params_buf, dload_mem_debug_len());

  num = dload_mem_debug_num_ent();
  for(i=0; i < num; i++)
  {
    ADD_BYTE_TO_PACKET(params_buf, dload_mem_debug_save_pref(i));
    ADD_DWORD_TO_PACKET(params_buf, dload_mem_debug_mem_base(i));
    ADD_DWORD_TO_PACKET(params_buf, dload_mem_debug_mem_length(i));
    ADD_STRING_TO_PACKET(params_buf, dload_mem_debug_desc(i));
    /* Add null-termination character */
    ADD_BYTE_TO_PACKET(params_buf, 0);
    ADD_STRING_TO_PACKET(params_buf, dload_mem_debug_filename(i));
    /* Add null-termination character */
    ADD_BYTE_TO_PACKET(params_buf, 0);
  }

  FINISH_BUILDING_PACKET(params_buf);

  /* Send the requested parameters response */
  transmit_packet(&params_buf);
#endif
}

#ifndef FEATURE_HWTC  /* 10/23/01 lcl */

/*-------------------------------------------------------------------------
 * New Method to get data from the underlying driver.
 *-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION dload_packet_init

DESCRIPTION
  This function Initialize the packet variables.

DEPENDENCIES
  None.

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None.

===========================================================================*/
void dload_packet_init (void)
{
  packet_data.length         = 0;
  packet_data.end_location   = 0;
  packet_data.start_location = 0;
  packet_data.step_crc       = CRC_16_L_SEED;
  packet_data.packet_ready   = FALSE;
}

/*===========================================================================

FUNCTION dload_enable_transmission

DESCRIPTION
  This function enables transmission of packet.

DEPENDENCIES
  None.

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None.

===========================================================================*/
void dload_enable_transmission(void)
{
  packet_data.packet_ready = TRUE;
  packet_data.end_location--;
  //bStor8D((uint8*)&packet_data.buffer[packet_data.length], (uint32)0);
}

/*===========================================================================

FUNCTION dload_enable_transmission

DESCRIPTION
  This function enables transmission of packet.

DEPENDENCIES
  None.

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None.

===========================================================================*/
void dload_packet_handle_incoming_data(boolean escape_char, byte ch)
{

  if (escape_char == FALSE)
  {
    if (packet_data.length < MAX_PACKET_LEN) 
    {
      bStor8D((uint8*)&packet_data.buffer[packet_data.end_location], (uint32)ch);
      packet_data.step_crc= CRC_16_L_STEP(packet_data.step_crc, (word)ch);
      packet_data.end_location++;
    }
  }
  /* if the count becomes greater than the size of the packet it
     will be discarded.
  */
  packet_data.length++;
}

/*===========================================================================

FUNCTION dload_hdlc_init

DESCRIPTION
  Initialize HDLC parameters

DEPENDENCIES
  None.

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None.

===========================================================================*/
void dload_hdlc_init (void)
{
  hdlc_curr_state   = DLOAD_HDLC_IDLE_STATE;
  hdlc_escape_state = 0;
}


/*===========================================================================

FUNCTION dload_hdlc_handle_data

DESCRIPTION
  This function stores a character received over the serial link in the
  receive buffer. It handles the escape character appropriately.

DEPENDENCIES
  None.

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None.

===========================================================================*/
void dload_hdlc_handle_data (byte ch)
{

  switch(ch)
  {
    case ASYNC_HDLC_FLAG:
      /* Found the end flag */
      dload_enable_transmission();
      hdlc_curr_state   = DLOAD_HDLC_IDLE_STATE;
      hdlc_escape_state = 0;
      break;

    case ASYNC_HDLC_ESC:
      hdlc_escape_state = 1;
      dload_packet_handle_incoming_data(TRUE, ch);
      break;

    default:
      if(hdlc_escape_state == 1)
      {
        ch ^= ASYNC_HDLC_ESC_MASK;
        hdlc_escape_state = 0;
      }
      //send this character over to message handler...
      dload_packet_handle_incoming_data(FALSE, ch);
  }

}

/*===========================================================================

FUNCTION dload_hdlc_handle_incoming_byte

DESCRIPTION
 This function stores a character received over the serial link in the
 receive buffer. It handles the escape character appropriately. 
 
DEPENDENCIES
  None.

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None.

===========================================================================*/
void dload_hdlc_handle_incoming_byte (byte ch)
{

  switch(hdlc_curr_state)
  {
    case DLOAD_HDLC_IDLE_STATE:
      /* Once in this state Hunt for ASYNC_HDLC_FLAG */
      if (ch == ASYNC_HDLC_FLAG)
      {
        hdlc_curr_state = DLOAD_HDLC_GOT_FLAG;
        packet_data.length++;
      }
      break;

    case DLOAD_HDLC_GOT_FLAG:
      if (ch == ASYNC_HDLC_FLAG)
      {
        packet_data.length++;
        break;
      }
      else
      {
        packet_data.start_location  = packet_data.length;
        packet_data.end_location    = packet_data.start_location;
        hdlc_curr_state = DLOAD_HDLC_MSG_BODY_STATE;
      }
      
    case DLOAD_HDLC_MSG_BODY_STATE:
      dload_hdlc_handle_data(ch);
      break;
    
    default:
      /* Should never come here */
      break;
  }
}

/*===========================================================================

FUNCTION dload_get_data_from_device

DESCRIPTION
 This function stores a character received over the serial link in the
 receive buffer. It handles the escape character appropriately. 
 
DEPENDENCIES
  None.

RETURN VALUE
  Return length of received bytes

SIDE EFFECTS
  None.

===========================================================================*/
uint32 dload_get_data_from_device()
{
  int    status = UART_TIMEOUT;
  uint32 len;
  /* pointer to buffer for receiving a packet */
  byte   *pkt_buf;

  pkt_buf = (byte *)&packet_data.buffer[packet_data.length];  
  /* loop forever waiting for data */
  for (;;)
  {
    BOOTHW_KICK_WATCHDOG();     /* Don't let the watchdog expire */    
    status = uartdm_receive_byte(pkt_buf, &len);
    if ((status != UART_RX_ERR) && (status != UART_NO_CHAR))
    {
      /* Found a byte */
      break;
    }
  }
  return len;
}

/*===========================================================================

FUNCTION dload_process_incoming_packets

DESCRIPTION
 This function stores a character received over the serial link in the
 receive buffer. It handles the escape character appropriately. 
 
DEPENDENCIES
  None.

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None.

===========================================================================*/
void dload_process_incoming_packets(void)
{
  /* pointer to buffer for receiving a packet */
  byte  *pkt_buf;

  /* packet length */
  uint32 len;

  if (packet_data.packet_ready == FALSE)
    return;

  /* set pointer to base of buffer area */
  pkt_buf = (byte *)&packet_data.buffer[packet_data.start_location];
  len     = (packet_data.end_location - packet_data.start_location) + 1;

  if (len < MIN_PACKET_LEN)       /* Reject any too-short packets  */
  {
    transmit_response(NAK_EARLY_END);            /* Send NAK       */
  }
  else if (packet_data.step_crc != CRC_16_L_OK_NEG)      /* Reject any with bad CRC */
  {
    transmit_response(NAK_INVALID_FCS);          /* Send NAK       */
  }
  else /* Yay, it's a good packet! */
  {
    /* Process the packet according to its command code */
    switch(bLoad8D((uint8*)&pkt_buf[0]))
    {
      case  CMD_WRITE:
        write_cmd(pkt_buf, len);
        break;
  
      case  CMD_ERASE:
        erase_cmd(pkt_buf, len);
        break;

      case  CMD_GO:
        go_cmd(pkt_buf, len);
        break;

      case  CMD_NOP:
        nop_cmd();
        break;

    case  CMD_PREQ:
#if (defined(FEATURE_USB) || defined(FEATURE_HS_USB_BASIC))
        interface = active_interface;
#endif
        preq_cmd();
        break;

      case  CMD_RESET:
        reset_cmd();
        break;
    case  CMD_UNLOCK:
#ifdef FEATURE_HWTC    /* 10/23/01 lcl */
#error code not present
#else
      unlock_cmd(pkt_buf, len);
#endif /* FEATURE_HWTC */
      break;

      case  CMD_VERREQ:
        verreq_cmd();
        break;

      case  CMD_PWROFF:
        pwroff_cmd();
        break;

    case  CMD_WRITE_32BIT:
#if (defined(FEATURE_USB) || defined(FEATURE_HS_USB_BASIC))
      interface = active_interface;
#endif
      write_32bit_cmd(pkt_buf, len);
      break;

    case  CMD_DLOAD_SWITCH:
      dload_switch_cmd();
      break;

    case  CMD_MEM_DEBUG_QUERY:
      dload_dbg_info_cmd();
      break;

    case  CMD_MEM_READ_REQ:
      dload_dbg_dump_mem(pkt_buf);
      break;

      default:
        transmit_response(NAK_INVALID_CMD);
        break;
    }/* switch on packet command code */
  }
  memset( (uint8*)&packet_data.buffer[0], 0x0, packet_data.length+1);
  dload_packet_init();
  uartdm_init_receiver();

}/* dload_process_incoming_packets() */


/*===========================================================================

FUNCTION process_packets_uart

DESCRIPTION
  This function is the main loop implementing the DMSS Async Download
  Protocol.  It loops forever, processing packets as they arrive.

DEPENDENCIES
  All necessary initialization for normal CPU operation must have
  been performed, and the UART must have been initialized, before
  entering this function.

RETURN VALUE
  This function does not return.

SIDE EFFECTS
  None.

===========================================================================*/
void process_packets_uart(void)
{
  uint32 len;  /* Number of bytes received by the driver */
  uint32 i, len_inc;
  
  dload_hdlc_init();
  /* Init the packet to store data */
  dload_packet_init();
  for (;;)
  {
    BOOTHW_KICK_WATCHDOG();
    /* Get data from the device */
    len = dload_get_data_from_device ();
    
    for (i=0, len_inc=packet_data.length; i<len; i++, len_inc++)
    {
      BOOTHW_KICK_WATCHDOG();
      dload_hdlc_handle_incoming_byte(bLoad8D((uint8*)&packet_data.buffer[len_inc]));
    }
    
    dload_process_incoming_packets();

    BOOTHW_KICK_WATCHDOG();
  }
}


#endif/* 10/23/01 lcl */


/*===========================================================================

FUNCTION process_packets

DESCRIPTION
  This function is the main loop implementing the DMSS Async Download
  Protocol.  It loops forever, processing packets as they arrive.

DEPENDENCIES
  All necessary initialization for normal CPU operation must have
  been performed, and the UART must have been initialized, before
  entering this function.

RETURN VALUE
  This function does not return.

SIDE EFFECTS
  None.

===========================================================================*/

void process_packets(void)
{
  byte  *pkt_buf;
    /* pointer to buffer for receiving a packet */

  word  len;
    /* Length of the packet received.  Guaranteed to be at least
       MIN_PACKET_LEN. */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* set pointer to base of buffer area */
  pkt_buf = (byte*)PKT_BUFFER_BASE;

  /* Loop forever processing packets */
  for (;;)
    {
    len = rcv_packet(pkt_buf);         /* Get a packet from the UART */

    /* Process the packet according to its command code */
    switch(pkt_buf[0])
      {
      case  CMD_WRITE:
        write_cmd(pkt_buf, len);
        break;

      case  CMD_ERASE:
        erase_cmd(pkt_buf, len);
        break;

      case  CMD_GO:
        go_cmd(pkt_buf, len);
        break;

      case  CMD_NOP:
        nop_cmd();
        break;

      case  CMD_PREQ:
#if (defined(FEATURE_USB) || defined(FEATURE_QHSUSB) || defined(FEATURE_HS_USB_BASIC))
        interface = active_interface;
#endif
        preq_cmd();
        break;

      case  CMD_RESET:
        reset_cmd();
        break;

      case  CMD_UNLOCK:
#ifdef FEATURE_HWTC    /* 10/23/01 lcl */
#error code not present
#else
        unlock_cmd(pkt_buf, len);
#endif /* FEATURE_HWTC */
        break;

      case  CMD_VERREQ:
        verreq_cmd();
        break;

      case  CMD_PWROFF:
        pwroff_cmd();
        break;

      case  CMD_WRITE_32BIT:
#if (defined(FEATURE_USB) || defined(FEATURE_QHSUSB) || defined(FEATURE_HS_USB_BASIC))
        interface = active_interface;
#endif
        write_32bit_cmd(pkt_buf, len);
        break;

      case  CMD_DLOAD_SWITCH:
        dload_switch_cmd();
        break;

      case  CMD_MEM_DEBUG_QUERY:
        dload_dbg_info_cmd();
        break;

      case  CMD_MEM_READ_REQ:
        dload_dbg_dump_mem(pkt_buf);
        break;

      default:
        transmit_response(NAK_INVALID_CMD);
        break;
      }/* switch on packet command code */

    /* we've processed the command - reset pointer to base of buffer area */
    pkt_buf = (byte*)PKT_BUFFER_BASE;

    }/* for (;;) */

}/* process_packets() */

#ifndef FEATURE_HWTC  /* 10/23/01 lcl */

/*============================================================================

 FUNCTION SECURITY CODE INIT

 DESCRIPTION
   Initialize the phone security code status.  If erased, there is no security
   code, and all operations are unlocked.  If a security code exists, an unlock
   packet with matching security code must be received, prior to performing
   any protected operations (read/write/go).

 FORMAL ARGUMENTS
   None

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   Sets the security code status.  If locked, an unlock command with correct
   security code must be received.
============================================================================*/


void security_code_init()
{
  //  extern unsigned long long chk_security_code_ptr; - (included diagi.h and
  //  commenting out the extern definition)
  byte *security_code;
  int i;

  /* point to beginning of security code */
  security_code = (byte*) &chk_security_code_ptr;

  /* scan through security code */
  for (i = 0; i < 8; i++)
    /* if the security code is not the erased value */
    if (security_code[i] != 0xFF)
      {
    /* then the code was written, and so we're currently locked */
    sec_code_unlocked = FALSE;
    return;
      }

  /* part is erased, so unlock for all protected operations */
  sec_code_unlocked = TRUE;
}
#endif /* FEATURE_HWTC */


/*============================================================================

 FUNCTION BOOT_DOWNLOADER

 DESCRIPTION
   Do a bootstrap download.  Initialize the UART or USB and perform memory
   operations at the direction of an external agent via the UART or USB.

 FORMAL ARGUMENTS
   None

 DEPENDENCIES
   Called from the Diagnostic Task during offline mode, or as a part of
   the Boot modules error recovery.

 RETURN VALUE
   This routine does not return.  It may branch to a new program loaded
   into RAM.

 SIDE EFFECTS
   The watchdog timer is reset, interrupts are disabled.  The timeout
   counter is initialized and timeout checking is enabled/disabled.

============================================================================*/
void
boot_downloader(void)
{

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* This flag is true until communications established */
  init_boot = TRUE;

  BOOTHW_KICK_WATCHDOG();
  
  /* Disable MMU and data cache in DLOAD execution 
  ** since there are not much improvement seen with DLOAD time.  
  ** NOR download happens with 1 min 49 secs and there is some
  ** seen when enabling data cache in the DLOAD context.
  ** Abort is seen in the QHSUSB buffer management.
  */
  mmu_clean_and_invalidate_data_cache();
  cache_mmu_disable();
  mmu_disable();
  mmu_enable_instruction_cache();

#if (defined(FEATURE_USB) || defined(FEATURE_QHSUSB) || defined(FEATURE_HS_USB_BASIC))
  interface = INTERFACE_UNKNOWN;
  active_interface = INTERFACE_UNKNOWN;
#endif


#if 0
  /* Turn of the PAs */
  HWIO_OUT( PA_ON_CTL, 0x0000 );
#endif
  /* Set the clock frequency for the download */
  clk_regime_set_dload_mclk();

#ifdef FEATURE_HS_USB_BASIC
  hsu_al_dload_init();
#elif defined(FEATURE_QHSUSB)
  qhsusb_al_dload_init();
#else
#ifdef FEATURE_USB
  /* Interface maybe UART or USB */
  dloadusb_init();
#endif
#endif

  uartdm_init();        /* Hardware initialization */
  uartdm_drain();


	
#ifndef FEATURE_HWTC /* 10/23/01 lcl */
	security_code_init();	  /* initialize the security code */
#endif /* FEATURE_HWTC */
  
#ifdef FEATURE_DLOAD_MEM_DEBUG
	dload_mem_debug_init();
#endif

  active_interface = get_active_interface();
 
  if (active_interface == INTERFACE_USB )
  {
     process_packets(); /* Detected USB */
     
  }

  else if (active_interface == INTERFACE_UART1)
  {
     process_packets_uart(); /* Detected UART */
  }
 	

}

