/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   P A C K E T   P R O C E S S I N G

GENERAL DESCRIPTION
  This module handles the DMSS Async Download Protocol to download
  code using simple generic UART/USB services.  This consists of an
  infinite loop of receiving a command packet through the UART/USB,
  acting on it, and returning a response packet.

  Actual block memory operations are not part of this module.
  These functions (named do_write and do_erase) are responsible
  for checking the received fields for validity.
  
  Everything in this file is MSM independent.
  
EXTERNALIZED FUNCTIONS
  process_packets
    Runs the packet protocol (forever).

INITIALIZATION AND SEQUENCING REQUIREMENTS
  All necessary initialization for normal CPU operation must have
  been performed, and the UART/USB must have been initialized, before
  entering this module.  Dispatch table to correct transport (UART or USB)
  must have been filled in.

Copyright (c) 2002-2004, 2006, 2008, 2009 Qualcomm Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*============================================================================
                          EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/tools/hostdl/HOSTDL.14.00/common/ap_packet.c#7 $ $DateTime: 2009/04/23 00:23:19 $ $Author: spuliven $
  Integrated from P402:  //depot/common-sw/armprg/armprg-9.0/common/ap_packet.c#3 
  
 when       who     what, where, why
 --------   ---     --------------------------------------------------------
 04/07/09   sp      Fix for UART1DM CRC errors under RESET_UART_ON_CRC_ERR
 03/19/09   sv      Fix out of bounds access lint error
 12/04/08   rk      Changes from BOOT team to support Download in AMSS
 09/16/08   sv      Modified for QHSUSB support. 
 04/09/08   mm      Added support for programming factory image.
 01/22/08   op      Pass length of the partition table to flash layer 
 05/08/06   rt      Use CRC_16 routines defined in services\utils\crc.h 
 11/19/04   drh     Support Multi-Image boot
 07/12/03   drh     Change two debug messages.
 11/07/03   drh     Transmit to host the number of bytes received from
                    handle_reset()
 10/14/03   drh     Get reply_buffer size from packet layer public header
                    file.
 09/24/03   drh     Add SUPPORTED_FEATURES def for NOR target
 09/25/03   drh     Change define name for extended NAND commands
 09/09/03   drh     Fix address handling on stream_write for boot loader
 07/22/03   drh     Add real command handling for extended NAND commands
 04/08/03   drh     Modified handling of feature bits to allow any number of
                    bytes of feature bits to be passed from host.
 03/25/03   drh     Added debug code to simulate dropping of received packets
                    and/or ACKs back to host.
 03/12/03   drh     Corrected handling of features requested by host.
 02/25/03   drh     Added calls to new extended commands, but in a kludgey
                    way until we can actually get the new commands from the
                    host tool and dispatch them correctly.
                    Added debug print statements on command entry and exit
 12/12/03   drh     Enhance transmit_error() to send out log message as
                    as well as error packet.
 11/13/03   drh     Add fix to flush last packet for USB if length is exact
                    multiple of 64 which is USB frame size
 11/07/02   drh     Remove include of ap_flashi.h
 11/05/02   drh     Included new flash layer header file.
                    Changed the way no valid flash is detected.
                    Changed hardcoded return value to enum type.
 10/30/02   drh     Moved obtaining of flash name and sector information to 
                    flash layer
                    Moved actual read in handle_simple_read to flash layer
                    Moved actual write in handle_simple_write to flash layer
                    but to new function do_simple_write
                    Moved actual stream_write to flash_layer
 10/29/02   drh     Move is_erased[] and it's init to flash layer
                    Move MAXIMUM_SECTORS to ap_flashi.h
                    Get flash device name from external function in flash layer
 10/23/02   drh     Move actual reset code to ap_msm.c as it now is sometimes
                    hardware specific.
 09/24/02   drh     Put in a temporary fix for MSM6100 watchdog bug.  This will
                    come out when hardware is working, so no need to put
                    it in hardware specific file.
 6/10/02    drh     Increased size of is_erased[] array.  It was not big
                    enough to handle the new Mitsubishi 8mb part and
                    code was walking past the end of the array and smashing
                    other data.
                    Added code to catch walking off end of is_erased[] and
                    reply_buffer[] arrays.
 06/07/02   drh     Moved call to flash_device_type() into main_c()
                    Moved reference to id_tbl into main_c()
 03/11/02   drh     Removed extern reference to target_id
                    Added code to copy small portion of release info
                    structure, ensuring that it will appear in the binary.
                    Added use of process_packet_flag to prevent USB from
                    adding data to packet which is currently being processed
 02/11/02   drh     Made macros of CHECK_FOR_DATA, TRANSMIT_BYTE and
                    DRAIN_FIFO so that functions could be dispatched.
 01/16/02   drh     Initial version of armprg to support the software 
                    downloads over UART on a single MSM target built in
                    the DMSS directory using DMSS header files.  Based on
                    code cleanup effort by lnr 11/2001.

============================================================================*/

//--------------------------------------------------------------------------
// Include Files
//--------------------------------------------------------------------------

#include <string.h>
#include "comdef.h"
#include "ap_armprg.h"
#include "ap_flash.h"
#include "crc.h"
#include "ap_msm.h"
#include "ap_packet.h"


//--------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------

#define DEVICE_UNKNOWN  0xFF

/* Maximum data length. */
/* Currently QPST tool supports a maximum packet length of 4096. This 
 * value can be tuned to use bigger packet sizes for better performance 
 * if the QPST tool supports bigger size packets */
#define UNFRAMED_MAX_DATA_LENGTH 4096

/* Maximum data length. */
#define MAX_DATA_LENGTH 1024

/* Maximum packet size.  1 for packet type.  4 for length.  2 for CRC. 
 * 9 for any other overhead */
#define MAX_PACKET_SIZE (UNFRAMED_MAX_DATA_LENGTH + 1 + 4 + 2 + 9)

/* Number of packets.  NUMBER_OF_PACKETS * MAX_DATA_LENGTH will be our
   maximum window size. */
#define NUMBER_OF_PACKETS 100
#define STREAM_DLOAD_MAX_VER         0x04
#define STREAM_DLOAD_MIN_VER         0x02

#define UNFRAMED_DLOAD_MIN_VER       0x04

#define FEATURE_UNCOMPRESSED_DLOAD   0x00000001

/* We only support these on NAND targets */
#if defined(EXTENDED_NAND_CMDS)
#define FEATURE_NAND_PRIMARY_IMAGE      0x00000002
#define FEATURE_NAND_BOOTLOADER_IMAGE   0x00000004
#define FEATURE_NAND_MULTI_IMAGE        0x00000008

/* This version of the downloader does not support the old NAND two
 * image mode, only Multi-Image
 */
#define SUPPORTED_FEATURES \
  (FEATURE_UNCOMPRESSED_DLOAD | FEATURE_NAND_MULTI_IMAGE)
#else
#define SUPPORTED_FEATURES FEATURE_UNCOMPRESSED_DLOAD
#endif  /* EXTENDED_NAND_CMDS */

#define READ_LEN                     7



/* Command/Rsp codes */
#define HELLO_RSP                    0x02
#define READ_RSP                     0x04
#define WRITE_RSP                    0x06
#define STREAM_WRITE_RSP             0x08
#define NOP_RSP                      0x0A
#define RESET_RSP                    0x0C
#define ERROR_RSP                    0x0D
#define CMD_LOG                      0x0E
#define UNLOCK_RSP                   0x10
#define PWRDOWN_RSP                  0x12
#define OPEN_RSP                     0x14
#define CLOSE_RSP                    0x16
#define SECURITY_MODE_RSP            0x18
#define PARTITION_TABLE_RSP          0x1A
#define OPEN_MULTI_IMAGE_RSP         0x1C
/* Gap here for commands implemented in QPST but not phone side */
#define UNFRAMED_STREAM_WRITE_CMD    0x30 /* Needed for command parsing */
#define UNFRAMED_STREAM_WRITE_RSP    0x31
#define DUMMY_RSP                    0x32

/* Only dispatch commands if they fall in the valid command range. */
#define FIRST_COMMAND 0x01
#define LAST_COMMAND  0x31


/* Used by debug only code that simulates dropped ACKS */
#ifdef DROP_ACKS
#define ACK_DROP_MOD  129
#endif

/* Used by debug only code that simulates dropped packets */
#ifdef DROP_PACKETS
#define DROP_MOD  29
#endif


/* Length of buffer for size message back to host.  Increase this
 * value if you change the size of size_msg_fixed.  Used by the
 * function log_size_msg(). This should be >= 1.
 */
#define SIZE_MSG_LEN  64


//--------------------------------------------------------------------------
// Global Data
//--------------------------------------------------------------------------

struct incoming_data
{
  struct incoming_data *next;   /* Chain appropriately. */
  word length;                  /* Number of valid bytes in buffer. */
  byte buffer[MAX_PACKET_SIZE];
};

typedef struct incoming_data *incoming_t;

struct incoming_data packet_data[NUMBER_OF_PACKETS];

/* We need 4 bytes extra for adjustment purposes. */
ALIGN(4) byte aligned_buffer[UNFRAMED_MAX_DATA_LENGTH + 4];

/* Lists. */
incoming_t free_list;           /* Unused buffers. */
incoming_t waiting;             /* Waiting to be processed. */
incoming_t waiting_last;        /* Last waiting packet (for queuing). */
incoming_t current;             /* The current packet for incoming
                                   data. */
/* Set to true after an escape character. */
static byte escape_state = 0;

/* True indicates that we need our hello packet before seeing
   anything. */
static byte need_hello = 1;

/* Reply packet. */
byte reply_buffer[REPLY_BUFFER_SIZE];
word reply_length;

/* Flag to indicate active packet processing, only used by USB */
boolean process_packet_flag = FALSE;

/* Used by debug only code that simulates dropped ACKS */
#ifdef DROP_ACKS
static uint32 ack_drop_count = 0;
#endif

/* Used by debug only code that simulates dropped packets */
#ifdef DROP_PACKETS
static uint32 drop_count = 0;
#endif

#ifdef EXTENDED_NAND_CMDS
/* Variables used in Multi-Image mode extended commands */
static security_mode_rcvd = FALSE;
static int partition_table_rcvd = FALSE;
static open_multi_mode_type open_multi_mode = OPEN_MULTI_MODE_NONE;
static open_mode_type open_mode = OPEN_MODE_NONE;
#endif

/* Global to take status of partition table command from layer below */
parti_tbl_type partition_status;

/* Global for status of open multi from layer below */
byte open_multi_status;

/* Variable to keep track of how many bytes received */
static uint32 bytes_recd = 0;

/* Data to produce size message back to host.  The length
 * of size_msg_fixed plus 9 must not exceed SIZE_MSG_LEN or the code
 * which checks for this will deal with it by sending a fixed message
 * indicating an overrun was detected.
 */
char size_msg[SIZE_MSG_LEN];
char *size_msg_fixed = "Total bytes recd by ARMPRG 0x";
char *size_msg_overrun = "ARMPRG size msg overrun";
const char digs[] = "0123456789ABCDEF";




//--------------------------------------------------------------------------
// Extern Definitions
//--------------------------------------------------------------------------

extern void process_current_packet (void);
#ifdef RESET_UART_ON_CRC_ERR
extern void uartdm_init_receiver(void);
boolean uart_rx_reset = FALSE;
#endif

//--------------------------------------------------------------------------
// Function Prototypes
//--------------------------------------------------------------------------

void transmit_error (dword code, char *message);
void xmit_log_message (char *message);

//--------------------------------------------------------------------------
// Function Declarations
//--------------------------------------------------------------------------

/*===========================================================================

FUNCTION packet_init

DESCRIPTION
  This function initializes the receive packet list.
  It also copies some data from the global identification block so that
  the data in the block will be included by the linker and will be able
  to be seen in the resulting binary.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void
packet_init (void)
{
  int i;

  TPRINTF(2,("packet_init: entry\n"));
  packet_data[0].next = NULL;
  packet_data[0].length = 0;

  /* Initialize free list. */
  for (i = 1; i < NUMBER_OF_PACKETS; i++)
  {
    packet_data[i].next = &packet_data[i - 1];
    packet_data[i].length = 0;
  }

  free_list = &packet_data[NUMBER_OF_PACKETS - 1];
  waiting = 0;
  current = 0;

  escape_state = 0;
  waiting_last = 0;
  need_hello = 1;

  TPRINTF(2,("packet_init: exit\n"));
  return;
}

/*===========================================================================

FUNCTION packet_handle_incoming

DESCRIPTION
  This function stores a character received over the serial link in the
  receive buffer. It handles the escape character 0x7D appropriately.

DEPENDENCIES
  UART must be initialized before this function is invoked.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void
packet_handle_incoming (int ch)
{
  bytes_recd++;

  /* Get us a buffer if needed. */
  if (current == NULL)
  {
    if (free_list == NULL)
    {
      xmit_log_message ("ERR: Window overrun detected...");
      return;
    }

    current = free_list;
    free_list = free_list->next;

    current->length = 0;
    current->next = NULL;       /* Pull it off of the list. */
  }

  switch (ch)
  {
    case 0x7E:
      process_current_packet ();
      escape_state = 0;
      break;

    case 0x7D:
      escape_state = 1;
      break;

    default:
      if (current->length < MAX_PACKET_SIZE)
      {
        if (escape_state)
        {
          ch ^= 0x20;
          escape_state = 0;
        }

        current->buffer[current->length] = ch;  /*lint !e734 */
      }

      /* Increment the count anyway.  We will automatically discard
         packets that are too long. */
      current->length++;
  }
}

/*===========================================================================

FUNCTION packet_handle_incoming_buf

DESCRIPTION
  This function stores characters received over the serial link in the
  receive buffer. It handles the escape character 0x7D appropriately.

DEPENDENCIES
  USB must be initialized before this function is invoked.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void
packet_handle_incoming_buf (byte* buf, int len)
{
   int i;
  bytes_recd+=len;

  /* Get us a buffer if needed. */
  if (current == NULL)
  {
    if (free_list == NULL)
    {
      xmit_log_message ("ERR: Window overrun detected...");
      return;
    }

    current = free_list;
    free_list = free_list->next;

    current->length = 0;
    current->next = NULL;       /* Pull it off of the list. */
  }
  if (buf[0] == UNFRAMED_STREAM_WRITE_CMD)
  {
    current->length = len;
    memcpy(current->buffer, buf, len);
    process_current_packet ();
    return;
  }
  for (i=0; i < len; i++) {
     byte ch = *buf++;
     switch (ch)
     {
       case 0x7E:
         process_current_packet ();
         escape_state = 0;
         break;

       case 0x7D:
         escape_state = 1;
         break;

       default:
         if (current->length < MAX_PACKET_SIZE)
         {
           if (escape_state)
           {
             ch ^= 0x20;
             escape_state = 0;
           }

           current->buffer[current->length] = ch;  /*lint !e734 */
         }

         /* Increment the count anyway.  We will automatically discard
            packets that are too long. */
         current->length++;
     }
  }
}

/*===========================================================================

FUNCTION compute_reply_crc

DESCRIPTION
  This function calculates the CRC for the reply message. It updates the 
  reply length by 2 to account for the 2 bytes of CRC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void
compute_reply_crc ()
{
  word crc = crc_16_l_calc (reply_buffer, reply_length * 8);
  reply_buffer[reply_length] = crc & 0xFF;
  reply_buffer[reply_length + 1] = crc >> 8;

  reply_length += 2;
}


/*===========================================================================

FUNCTION force_xmit_reply

DESCRIPTION
  This function transmits a reply packet that has been filled up previously.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void
force_xmit_reply ()
{
  int i;
  int ch;
#ifndef USE_UART_ONLY
  int j;

  j = 0;
#endif

  CHECK_FOR_DATA ();

  /* Since we don't know how long it's been. */
  TRANSMIT_BYTE (0x7E);
#ifndef USE_UART_ONLY
  j++;
#endif

  for (i = 0; i < reply_length; i++)
  {
    /* we only need to check once every 31 characters, since RX and TX
     * run at about the same speed, and our RX FIFO is 64 characters
     */
    if ((i & 31) == 31)
      CHECK_FOR_DATA ();

    ch = reply_buffer[i];

    if (ch == 0x7E || ch == 0x7D)
    {
      TRANSMIT_BYTE (0x7D);
      TRANSMIT_BYTE (0x20 ^ ch);        /*lint !e734 */
#ifndef USE_UART_ONLY
      j += 2;
#endif
    }
    else
    {
      TRANSMIT_BYTE (ch);       /*lint !e734 */
#ifndef USE_UART_ONLY
      j++;
#endif
    }
  }

  CHECK_FOR_DATA ();
  TRANSMIT_BYTE (0x7E);
#ifndef USE_UART_ONLY
  j++;
#endif

#ifndef USE_UART_ONLY
  /* Hack for USB protocol.  If we have an exact multiple of the USB frame
   * size, then the last frame will not be sent out.  The USB standard says
   * that a "short packet" needs to be sent to flush the data.  Two flag
   * characters can serve as the short packet.  Doing it this way, we only
   * perform this test once on every entire packet from the target, so the 
   * over head is not too much.
   */
  if ((j%64) == 0)
  {
    TRANSMIT_BYTE (0x7E);
    TRANSMIT_BYTE (0x7E);
  }
#endif
}


/*===========================================================================

FUNCTION process_current_packet

DESCRIPTION
  This function processes a received packet. It validates the packet contents
  and stores the packet in the receive list to be processed on a FIFO basis.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void
process_current_packet (void)
{
  /* Begin with some simple sanity checks. */
  if (current->length > MAX_PACKET_SIZE || current->length < 3)
  {

    current->length = 0;

    return;
  }

  /* Queue it up to be processed. */
  if (waiting_last == NULL)
  {
    waiting = waiting_last = current;
    current = NULL;
  }
  else
  {
    waiting_last->next = current;
    waiting_last = current;
    current = NULL;
  }
}


/*===========================================================================

FUNCTION my_strlen

DESCRIPTION
  This function computes the length of the received string.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
int
my_strlen (char *s)
{
  int i = 0;
  while (*s++)
    i++;
  return i;
}



/*===========================================================================

FUNCTION log_size_msg

DESCRIPTION
  This function sends a message back to the host containing the number of
  bytes received.  It embodies a very simple sprintf for hex digits.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void
log_size_msg(void)
{
  int i = 0;
  int shift_count = 28;
  unsigned int nibble;
  int fixed_size;

  fixed_size = my_strlen(size_msg_fixed);

  /* Check if length of size_msg_fixed + 8 is within the max message buffer 
   * length(SIZE_MSG_LEN). 8 is the number of nibbles in the 32 bit data
   * (bytes_recd) used to store the number of bytes received. Each nibble 
   * of bytes_read is stored in size_msg as a character. 
   */
  if ((fixed_size + 8) < SIZE_MSG_LEN)
  {
    memcpy(size_msg, size_msg_fixed, fixed_size);
  
    for (i = fixed_size; i < (fixed_size + 8); i++)
    {
      /* prevent buffer overrun */
      nibble = (bytes_recd >> shift_count) & 0xF;
      size_msg[i] = digs[nibble];
      shift_count -= 4;
    }
    size_msg[i] = 0;
  }
  else
  {
    DPRINTF(("log_size_msg() Overrun\n"));
    fixed_size = my_strlen(size_msg_overrun);
    if (fixed_size < SIZE_MSG_LEN)
    {
      memcpy(size_msg, size_msg_overrun, fixed_size);
      size_msg[fixed_size] = 0;
    }
    else
    {
      /* This case will not occur currently */
      size_msg[0] = 0;
    }

  }
  xmit_log_message(size_msg);
}




/*===========================================================================

FUNCTION transmit_error

DESCRIPTION
  This function transmits an error response with the specified error code and
  specified error message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void
transmit_error (dword code, char *message)
{
  int length = my_strlen (message);
  
  TPRINTF(2,("transmit_error:  %s\n", message));
  
  /* Make sure error message gets in log */
  xmit_log_message (message);

  reply_buffer[0] = ERROR_RSP;
  reply_buffer[1] = code & 0xFF;
  reply_buffer[2] = (code >> 8) & 0xFF;
  reply_buffer[3] = (code >> 16) & 0xFF;
  reply_buffer[4] = (code >> 24) & 0xFF;
  memcpy (&reply_buffer[5], message, length);   /*lint !e732 */

  reply_length = 5 + length;    /*lint !e734 */

  compute_reply_crc ();
  force_xmit_reply ();

  TPRINTF(2,("transmit_error: exit\n"));
  return;
}


/*===========================================================================

FUNCTION xmit_log_message

DESCRIPTION
  This function transmits the specified log message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void
xmit_log_message (char *message)
{
  int length = my_strlen (message);

  reply_buffer[0] = CMD_LOG;
  memcpy (&reply_buffer[1], message, length);   /*lint !e732 */

  reply_length = 1 + length;    /*lint !e734 */

  /* Host tool does not add a CR to the log after the message,
   * so we must
   */
  reply_buffer[reply_length++] = '\n';

  compute_reply_crc ();
  force_xmit_reply ();

  /* If printing is on, also print all messages transmitted to log */
  DPRINTF(("XMIT_ERR:  %s\n", message));
}


/*===========================================================================

FUNCTION handle_invalid

DESCRIPTION
  This function transmits an *Invalid Command* message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
int
handle_invalid (incoming_t buffer)
{
  TPRINTF(2,("handle_invalid: entry\n"));
  transmit_error (0x0005, "Invalid Command");
  TPRINTF(2,("handle_invalid: exit\n"));
  
  return 0;
}                               /*lint !e715 */

/*===========================================================================

FUNCTION handle_hello

DESCRIPTION
  This function handles the *hello command* packet and transmits the 
  appropriate response packet. It sets a flag to indicate that flash driver 
  commands could be processed after validating the hello packet.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
int
handle_hello (incoming_t buffer)
{
  static const byte host_header[] = "QCOM fast download protocol host";
  static const byte target_header[] = "QCOM fast download protocol targ";
  dword wsize;
  int p;
  char *flashname;
  uint8 requested_feature_bits;

  TPRINTF(2,("handle_hello: entry\n"));
  
  if (flash_dev_type != FLASH_DEVICE_FOUND)      /*lint !e650 */
  {
    transmit_error (NAK_WRONG_IID, "Unrecognized flash device");
    return 0;
  }

  if (memcmp (&buffer->buffer[1], host_header, 32) != 0)
  {
    transmit_error (NAK_INVALID_CMD, "Invalid magic number");
    return 0;
  }
  
  TPRINTF(2,("handle_hello: Stream min ver is 0x%x\n", buffer->buffer[34]));
  TPRINTF(2,("handle_hello: Stream max ver is 0x%x\n", buffer->buffer[33]));

  
  /* OK to accept versions higher than we know about */
  if (buffer->buffer[34] < STREAM_DLOAD_MIN_VER)
  {
    transmit_error (NAK_INVALID_CMD, "Invalid protocol version");
    return 0;
  }



  TPRINTF(2,("handle_hello: buffer length %d  feature bits 0x%x\n",
             buffer->length, buffer->buffer[35]));
 
  /* 
   * Allow more than 1 byte of feature bits, even though we do not use
   * more than one byte of feature bits.  This allows the host to send
   * as many bytes as it cares, and we will only deal with the ones
   * we know about.
   */
  if (buffer->length < 36)
  {
    transmit_error (NAK_INVALID_LEN, "Parameter Request Packet Length Error");
    return 0;
  }


  need_hello = FALSE;

  /* Generate reply. */
  reply_buffer[0] = HELLO_RSP;

  memcpy (&reply_buffer[1], target_header, 32);
  reply_buffer[33] = STREAM_DLOAD_MAX_VER;
  reply_buffer[34] = STREAM_DLOAD_MIN_VER;

  /* Preferred block size. */
  reply_buffer[35] = (MAX_DATA_LENGTH) & 0xFF;  /*lint !e778 */
  reply_buffer[36] = (MAX_DATA_LENGTH >> 8) & 0xFF;
  reply_buffer[37] = 0;
  reply_buffer[38] = 0;

  /* Base address of flash  */
  reply_buffer[39] = 0;
  reply_buffer[40] = 0;
  reply_buffer[41] = 0;
  reply_buffer[42] = 0;

  /* Flash id len and string */
  flashname = flash_device_name();
  if (flashname == 0)
  {
    transmit_error (NAK_WRONG_IID, "Unrecognized flash device");
    return 0;
  }

  reply_buffer[43] = my_strlen (flashname);
  memcpy (&reply_buffer[44], flashname, reply_buffer[43]);

  p = 44 + reply_buffer[43];    /* index into WindowSize field */

  /* Window size. */
  wsize = NUMBER_OF_PACKETS;

  reply_buffer[p++] = (wsize) & 0xFF;
  reply_buffer[p++] = (wsize >> 8) & 0xFF;

  p += flash_device_sectors (&reply_buffer[p]);


  /* 
   * Feature bits! Currently, we only deal with the first byte of
   * feature bits, but above, we do not error if more bytes are sent
   * by the host.  If we eventually start recognizing more bytes
   * of feature bits, then these few lines of code need to change
   * to handle each byte on its own.
   */
  requested_feature_bits = buffer->buffer[35];
  reply_buffer[p++] = (requested_feature_bits & SUPPORTED_FEATURES);

  reply_length = p;             /*lint !e734 */


  /*
   *  Check to see if we overran the end of the reply_buffer.  If so,
   *  loop here forever.  The fix is to increase the size of the
   *  reply_buffer, however, this may have consequences with regards
   *  to the host side.  This should be checked thoroughly.  This
   *  probably would have occurred by someone adding a flash part
   *  that is larger than one we have ever used before.
   */
  if (p > sizeof(reply_buffer))
  {
    transmit_error (NAK_FAILED, "Internal error - reply_buffer overrun");
    DPRINTF(("handle_hello: exceeded buffer length\n"));
    for (;;)
    {
      KICK_WATCHDOG();
    }
  }

  compute_reply_crc ();
  force_xmit_reply ();

  TPRINTF(2,("handle_hello: exit\n"));
  
  return 0;                     /* Don't queue. */
}


/*===========================================================================

FUNCTION handle_simple_read

DESCRIPTION
  This function reads the specified number of bytes from the specified 
  location.

DEPENDENCIES
  None.

RETURN VALUE
  0 - success

SIDE EFFECTS
  None.

===========================================================================*/
int
handle_simple_read (incoming_t buffer)
{
  dword addr;
  word length, i = 0;
  byte *data = buffer->buffer;
  response_code_type retVal;

  TPRINTF(2,("handle_simple_read: entry\n"));

  if (buffer->length != READ_LEN)
  {
    transmit_error (NAK_INVALID_LEN, "Invalid packet length");
    return 0;
  }

  addr = (data[1] | (data[2] << 8) | (data[3] << 16) | (data[4] << 24));
  length = (data[5] | (data[6] << 8));

  /* length of the read reply packet(read data length + 5(1 byte for command +
   * 4 byte for address) should not exceed the reply buffer size.
   */
  if ((length + 5) > REPLY_BUFFER_SIZE)
  {
    transmit_error (NAK_INVALID_LEN, "Packet too large");
    return 0;
  }

  reply_buffer[i++] = READ_RSP;
  reply_buffer[i++] = data[1];
  reply_buffer[i++] = data[2];
  reply_buffer[i++] = data[3];
  reply_buffer[i++] = data[4];

  if ((retVal = do_read (&reply_buffer[i], addr, length)) != ACK)
  {
    transmit_error ((dword) retVal, "Read unsuccessful");
    return 0;
  }
  reply_length = i + length;
  
  compute_reply_crc ();
  force_xmit_reply ();

  TPRINTF(2,("handle_simple_read: exit\n"));
  
  return 0;
}


/*===========================================================================

FUNCTION handle_reset

DESCRIPTION
  This function causes a watchdog reset to occur.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
int
handle_reset (incoming_t buffer)
{
  
  TPRINTF(2,("handle_reset: entry\n"));

#ifdef EXTENDED_NAND_CMDS
  /* Do not allow reset after open but before close */
  if ((open_multi_mode != OPEN_MULTI_MODE_NONE) ||
      (open_mode != OPEN_MODE_NONE))
  {
    transmit_error (NAK_CMD_OUT_SEQ, "Cannot reset before close");
    return 0;
  }
#endif  /* EXTENDED_NAND_CMDS */
  
  reply_buffer[0] = RESET_RSP;
  reply_length = 1;

  compute_reply_crc ();
  force_xmit_reply ();

#ifdef LOG_DOWNLOAD_SIZE
  /* Send a message to the host log indicating how many bytes we received */
  log_size_msg();
#endif

  /* Kick dog once to allow enough time for UART to drain.  */
  KICK_WATCHDOG ();

  DRAIN_TRANSMIT_FIFO ();

  TPRINTF(2,("handle_reset: exit\n"));
  
  msm_reset();
  /* does not return */

  return 0;                     /*lint !e527 */
}                               /*lint !e715 */

/*===========================================================================

FUNCTION handle_simple_write

DESCRIPTION
  This function is used to write the specified data at a specified location. It
  can be only be used to change 1's to 0's. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
int
handle_simple_write (incoming_t buffer)
{
  byte *data = buffer->buffer;
  word len = buffer->length - 5;        /* cmd-type : 1, addr : 4 */
  dword addr = data[1] | (data[2] << 8) | (data[3] << 16) | (data[4] << 24);
  response_code_type retVal;
  byte* src_ptr;

  TPRINTF(2,("handle_simple_write: entry\n"));

#ifdef EXTENDED_NAND_CMDS
  /* NAND programming does not support this command */
  transmit_error (NAK_INVALID_CMD, "Simple write not allowed");
  return 0;
#endif  /* EXTENDED_NAND_CMDS */

  src_ptr = &data[5];

  if (((uint32)src_ptr | addr | len) & 0x01)
  {
    byte *dest_ptr, *buffer_ptr;
    uint32 actual_len_to_write;

    actual_len_to_write = len;

    /* Base address is assumed as 0. Refer to the do_simple_write implementation. */
    dest_ptr = (byte*)addr;
    buffer_ptr = &aligned_buffer[0];

    if (((uint32)dest_ptr) & 0x01)
    {
      dest_ptr = (byte*)((uint32)dest_ptr & (~0x01));
      *buffer_ptr++ = *dest_ptr;
      --addr;
      ++actual_len_to_write;
    }

    if (actual_len_to_write & 0x01)
    {
      aligned_buffer[actual_len_to_write] = dest_ptr[actual_len_to_write];
      ++actual_len_to_write;
    }

    memcpy(buffer_ptr, src_ptr, len);

    retVal = do_simple_write (aligned_buffer, addr, actual_len_to_write);
  }
  else
  {        /* Do we really need this else case..??!! */
    retVal = do_simple_write (src_ptr, addr, len);
  }

  if (retVal != ACK)
  {
    transmit_error ((dword) retVal, "Write unsuccessful");
    return 0;
  }

  reply_buffer[0] = WRITE_RSP;
  reply_buffer[1] = addr & 0xFF;
  reply_buffer[2] = (addr >> 8) & 0xFF;
  reply_buffer[3] = (addr >> 16) & 0xFF;
  reply_buffer[4] = (addr >> 24) & 0xFF;
  reply_length = 5;

  compute_reply_crc ();
  force_xmit_reply ();

  TPRINTF(2,("handle_simple_write: exit\n"));
  
  return 0;
}

/*===========================================================================

FUNCTION handle_stream_write

DESCRIPTION
  This function write a stream of bytes at the specified location in flash
  memory. It validates that the command is valid before writing into flash.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

int
handle_stream_write (incoming_t buffer)
{
  byte *data = buffer->buffer;
  word len = buffer->length - 5;
  dword addr = data[1] | (data[2] << 8) | (data[3] << 16) | (data[4] << 24);
  response_code_type retVal;
  byte* src_ptr;

  TPRINTF(4,("handle_stream_write: addr 0x%x  len 0x%x\n", addr, len));

  src_ptr = &data[5];

  if ((addr | (uint32)src_ptr | len) & 0x01)
  {
    byte *dest_ptr, *buffer_ptr;
    uint32 actual_len_to_write;

    actual_len_to_write = len;

    /* Base address is assumed as 0. Refer to the do_stream_write implementation. */
    dest_ptr = (byte*)addr;
    buffer_ptr = &aligned_buffer[0];

    if (((uint32)dest_ptr) & 0x01)
    {
      dest_ptr = (byte*)((uint32)dest_ptr & (~0x01));
      *buffer_ptr++ = *dest_ptr;
      --addr;
      ++actual_len_to_write;
    }
#ifndef FEATURE_DLOAD_IN_AMSS
    if (actual_len_to_write & 0x01)
    {
      aligned_buffer[actual_len_to_write] = dest_ptr[actual_len_to_write];
      ++actual_len_to_write;
    }
#endif /* !FEATURE_DLOAD_IN_AMSS */
    memcpy(buffer_ptr, src_ptr, len);

    retVal = do_stream_write (aligned_buffer, addr, actual_len_to_write);
  }
  else
  {
    retVal = do_stream_write (src_ptr, addr, len);
  }

  if (retVal != ACK)
  {
    transmit_error ((dword) retVal, "Write unsuccessful");
    return 0;
  }

#ifdef DROP_ACKS
  /* Simulate dropping of ACKs to force false retransmit by host */
  ack_drop_count++;
  if (ack_drop_count%ACK_DROP_MOD == 0)
  {
    term_put('a');
    DPRINTF(("drop ACK %d\n", ack_drop_count));
    return 0;
  }
#endif


  reply_buffer[0] = STREAM_WRITE_RSP;
  reply_buffer[1] = addr & 0xFF;
  reply_buffer[2] = (addr >> 8) & 0xFF;
  reply_buffer[3] = (addr >> 16) & 0xFF;
  reply_buffer[4] = (addr >> 24) & 0xFF;
  reply_length = 5;

  compute_reply_crc ();
  force_xmit_reply ();

  TPRINTF(4,("handle_stream_write: exit\n"));
  
  return 0;
}


/*===========================================================================

FUNCTION handle_sync

DESCRIPTION
  This function responds to a NOP command packet with a NOP response packet.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
int
handle_sync (incoming_t buffer)
{
  byte *data = buffer->buffer;
  word len = buffer->length - 1;

  TPRINTF(2,("handle_sync: entry\n"));
  
  reply_buffer[0] = NOP_RSP;
  memcpy (&reply_buffer[1], &data[1], len);
  reply_length = len + 1;

  compute_reply_crc ();
  force_xmit_reply ();

  TPRINTF(2,("handle_sync: exit\n"));
  
  return 0;
}



/*===========================================================================

FUNCTION handle_power_off

DESCRIPTION
  This function sends a message that this command is not supported.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

int
handle_power_off (incoming_t buffer)
{
  reply_buffer[0] = PWRDOWN_RSP;
  reply_length = 1;

  TPRINTF(2,("handle_power_off: entry\n"));

  /* Do not allow power down after open but before close */
  transmit_error (NAK_PWROFF_NOT_SUPP, "Power off not supported");
  return 0;
}





/*===========================================================================

FUNCTION handle_open

DESCRIPTION
  This function is used to open a NAND device for writing.
  Can be used for either Primary Image or BootLoader Image.

DEPENDENCIES
  None.

RETURN VALUE
  0 - success

SIDE EFFECTS
  None.

===========================================================================*/
#ifndef EXTENDED_NAND_CMDS
int
handle_open (incoming_t buffer)
{
  TPRINTF(2,("handle_open: entry\n"));
  
  /* If we were not built as NANDPRG, open is not supported */
  transmit_error (NAK_NAND_NOT_SUPP, "NAND programming not supported");
  
  TPRINTF(2,("handle_open: exit\n"));
  return 0;
}
#else
int
handle_open (incoming_t buffer)
{
  byte mode = buffer->buffer[1];
  response_code_type retVal;
  
  TPRINTF(2,("handle_open: entry\n"));

  if (open_mode != OPEN_MODE_NONE)
  {
    if (open_mode != mode)
    {
      transmit_error (NAK_CMD_OUT_SEQ, "Already opened in different mode");
      return 0;
    }
    else
    {
      /* Not a failure, but needs to get in host side log */
      xmit_log_message("Already opened, ignoring open in same mode");
      return 0;
    }
  }
 
  retVal = do_open(mode);

  if ( retVal != ACK )
  {
    transmit_error (NAK_FAILED, "Open failed");
    TPRINTF(2,("handle_open: exit\n"));
    return 0;
  }

  /* Success, store mode and reply */
  open_mode = (open_mode_type) mode;
  reply_buffer[0] = OPEN_RSP;
  reply_length = 1;

  compute_reply_crc ();
  force_xmit_reply ();
  
  TPRINTF(2,("handle_open: exit\n"));
  
  return 0;
}
#endif  /* !EXTENDED_NAND_CMDS */


/*===========================================================================

FUNCTION handle_close

DESCRIPTION
  This function is used to close a NAND device for writing.
  Can be used for either Primary Image or BootLoader Image.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
#ifndef EXTENDED_NAND_CMDS
int
handle_close (incoming_t buffer)
{
  TPRINTF(2,("handle_close: entry\n"));
  
  /* If we were not built as NANDPRG, open is not supported */
  transmit_error (NAK_NAND_NOT_SUPP, "NAND programming not supported");
  TPRINTF(2,("handle_close: exit\n"));
  return 0;
}
#else
int
handle_close (incoming_t buffer)
{
  response_code_type retVal;

  TPRINTF(2,("handle_close: entry\n"));

  if ((open_multi_mode == OPEN_MULTI_MODE_NONE) &&
      (open_mode == OPEN_MODE_NONE))
  {
    transmit_error (NAK_CMD_OUT_SEQ, "Cannot close when not previously opened");
    return 0;
  }


  retVal = do_close();
  if (retVal != ACK)
  {
    transmit_error (retVal, "Close failed");
    return 0;
  }

  open_multi_mode = OPEN_MULTI_MODE_NONE;
  open_mode = OPEN_MODE_NONE;
  
  reply_buffer[0] = CLOSE_RSP;
  reply_length = 1;

  compute_reply_crc ();
  force_xmit_reply ();

  TPRINTF(2,("handle_close: exit\n"));
  
  return 0;
  }
#endif  /* !EXTENDED_NAND_CMDS */



/*===========================================================================

FUNCTION handle_security_mode

DESCRIPTION
  This function is used to set the security mode to be used for all
  subsequent operations.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
#ifndef EXTENDED_NAND_CMDS
int
handle_security_mode (incoming_t buffer)
{
  TPRINTF(2,("handle_security_mode: entry\n"));
  
  /* If we were not built as NANDPRG, open is not supported */
  transmit_error (NAK_NAND_NOT_SUPP, "NAND programming not supported");
  TPRINTF(2,("handle_security_mode: exit\n"));
  return 0;
}
#else
int
handle_security_mode (incoming_t buffer)
{
  byte mode = buffer->buffer[1];
  response_code_type retVal;
  
  TPRINTF(2,("handle_security_mode: entry\n"));

  retVal = do_security_mode (mode);

  if (retVal != ACK)
  {
    transmit_error (retVal, "Set security mode failed.");
    return 0;
  }

  security_mode_rcvd = TRUE;

  /* Reply to the host as success */
  reply_buffer[0] = SECURITY_MODE_RSP;
  reply_length = 1;

  compute_reply_crc ();
  force_xmit_reply ();

  TPRINTF(2,("handle_security_mode: exit\n"));
  
  return 0;
}
#endif  /* !EXTENDED_NAND_CMDS */


/*===========================================================================

FUNCTION handle_parti_tbl

DESCRIPTION
  This function is used to receive and validate a partition table used
  in all subsequent programming operations

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
#ifndef EXTENDED_NAND_CMDS
int
handle_parti_tbl (incoming_t buffer)
{
  TPRINTF(2,("handle_parti_tbl: entry\n"));
  
  /* If we were not built as NANDPRG, open is not supported */
  transmit_error (NAK_NAND_NOT_SUPP, "NAND programming not supported");
  TPRINTF(2,("handle_parti_tbl: exit\n"));
  return 0;
}
#else
int
handle_parti_tbl (incoming_t buffer)
{
  byte *data = &buffer->buffer[2];
  word len = buffer->length - 2;        /* cmd-type : 1, override : 1 */
  response_code_type retVal;
  byte override = buffer->buffer[1];

  TPRINTF(2,("handle_parti_tbl: entry\n"));

  /* Check for security mode already sent */
  if (security_mode_rcvd != TRUE)
  {
    transmit_error (NAK_CMD_OUT_SEQ, 
                    "No security mode received before partition table");
    TPRINTF(2,("handle_parti_tbl: exit\n"));
    return 0;
  }
  else
  {
    TPRINTF(2,("handle_parti_tbl: OK - security mode already received\n"));
  }

  /* Check for payload length not exceeded */
  if (len > 512)
  {
    transmit_error (NAK_INVALID_LEN, 
                    "Partition table length exceeded");
    TPRINTF(2,("handle_parti_tbl: exit\n"));
    return 0;
  }
  else
  {
    TPRINTF(2,("handle_parti_tbl: OK - length check\n"));
  }

  TPRINTF(2,("handle_parti_tbl: Calling do_partition_table\n"));
  retVal = do_partition_table (data, override, len);
  TPRINTF(2,("handle_parti_tbl: Back from do_partition_table with status 0x%x\n", retVal));
  if (retVal != ACK)
  {
    if (partition_status == PARTI_TBL_UNKNOWN_ERROR)
    {
      /* Some fatal error occurred that host does not care to parse */
      partition_table_rcvd = FALSE;
      transmit_error (NAK_FAILED, 
                      "Unknown error accepting partition table");
      TPRINTF(2,("handle_parti_tbl: exit\n"));
      return 0;
    }
    else
    {
      /* Error occurred that host needs to parse and possibly send
       * partition table again.
       */
      partition_table_rcvd = FALSE;
      reply_buffer[0] = PARTITION_TABLE_RSP;
      reply_buffer[1] = partition_status;    /* status from below */
      reply_length = 2;

      TPRINTF(2,("handle_parti_tbl: Error host needs to parse is 0x%x\n", partition_status));
      compute_reply_crc ();
      force_xmit_reply ();
      TPRINTF(2,("handle_parti_tbl: exit\n"));
      return 0;
    }
  }

  partition_table_rcvd = TRUE;

  reply_buffer[0] = PARTITION_TABLE_RSP;
  reply_buffer[1] = 0x0;                 /* partition table accepted */
  reply_length = 2;

  compute_reply_crc ();
  force_xmit_reply ();
  TPRINTF(2,("handle_parti_tbl: exit\n"));
  
  return 0;
}
#endif  /* !EXTENDED_NAND_CMDS */


/*===========================================================================

FUNCTION handle_open_multi

DESCRIPTION
  This function is used to open for writing a particular image for multi-image
  boot targets

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
#ifndef EXTENDED_NAND_CMDS
int
handle_open_multi (incoming_t buffer)
{
  TPRINTF(2,("handle_open_multi: entry\n"));
  
  /* If we were not built as NANDPRG, open is not supported */
  transmit_error (NAK_NAND_NOT_SUPP, "NAND programming not supported");
  TPRINTF(2,("handle_open_multi: exit\n"));
  return 0;
}
#else
int
handle_open_multi (incoming_t buffer)
{
  byte *data = &buffer->buffer[2];
  byte mode = buffer->buffer[1];
  response_code_type retVal;
  word len = buffer->length - 2;  /* length of optional data */

  
  TPRINTF(2,("handle_open_multi: entry\n"));

  /* Check for security mode already sent */
  if (security_mode_rcvd != TRUE)
  {
    transmit_error (NAK_CMD_OUT_SEQ, 
                    "No security mode received before open multi");
    TPRINTF(2,("handle_open_multi: exit\n"));
    return 0;
  }

  /* Check for partition table received */
  if (partition_table_rcvd != TRUE)
  {
    transmit_error (NAK_CMD_OUT_SEQ, 
                    "No partition table received before open multi");
    TPRINTF(2,("handle_open_multi: exit\n"));
    return 0;
  }


  TPRINTF(2,("handle_open_multi: calling do_open_multi\n"));
  TPRINTF(2,("handle_open_multi: mode value is 0x%x\n", mode));
  retVal = do_open_multi (data, mode, len);
  TPRINTF(2,("handle_open_multi: back from do_open_multi\n"));
  if (retVal != ACK)
  {
    /* This error cannot be dealt with by the host */
    if (open_multi_status == OPEN_MULTI_UNKNOWN_ERROR)
    {
      transmit_error (NAK_FAILED, "Open multi failed, unknown error");
      TPRINTF(2,("handle_open_multi: exit\n"));
      return 0;
    }
    else
    {
      /* These errors can be parsed by the host, treat them differently */
      switch (open_multi_status)
      {
        case OPEN_MULTI_LENGTH_EXCEEDED:
        case OPEN_MULTI_PAYLOAD_NOT_ALLOWED:
        case OPEN_MULTI_PAYLOAD_REQUIRED:
          reply_buffer[0] = OPEN_MULTI_IMAGE_RSP;
          reply_buffer[1] = open_multi_status;
          reply_length = 2;
          break;
        default:
          transmit_error (NAK_FAILED, "Open multi failed, unknown error");
          TPRINTF(2,("handle_open_multi: exit\n"));
          return 0;
      }
    }
  }
  else
  {
    /* Success, store mode and reply */
    open_multi_mode = (open_multi_mode_type) mode;
    reply_buffer[0] = OPEN_MULTI_IMAGE_RSP;
    reply_buffer[1] = OPEN_MULTI_SUCCESS;
    reply_length = 2;
  }
  
  compute_reply_crc ();
  force_xmit_reply ();
  TPRINTF(2,("handle_open_multi: exit\n"));
  
  return 0;
}
#endif  /* !EXTENDED_NAND_CMDS */



/*===========================================================================

  This function is used to handle flashing with packets that are not following
  HDLC encoding.

===========================================================================*/
int handle_unframed_stream_write(incoming_t buffer)
{
  dword unframed_length;
  byte *unframed_data;
  dword addr;
  response_code_type retVal;
  byte* buf;
  TPRINTF(4,("handle_unframed_stream_write: enter\n"));

  /* In unframed packet, byte 0 is command, bytes 1-3 are padding bits for
   * alignment, bytes 4-7 are the address, bytes 8-11 are the length.
   */

  buf = buffer->buffer;

  addr = (dword) (buf[4] | (buf[5] << 8) | (buf[6] << 16) | (buf[7] << 24));

  unframed_length = (dword) (buf[8] | (buf[9] << 8) |
        (buf[10] << 16) | (buf[11] << 24));

  unframed_data = &(buffer->buffer[12]);

  TPRINTF(4,("handle_unframed_stream_write: addr 0x%x  len 0x%x\n",
     addr, unframed_length));

  retVal = do_stream_write(unframed_data, addr,unframed_length);

  if (retVal != ACK)
  {
    transmit_error ((dword) retVal, "Unframed stream write unsuccessful");
    return 0;
  }

  reply_buffer[0] = UNFRAMED_STREAM_WRITE_RSP;
  reply_buffer[1] = addr & 0xFF;
  reply_buffer[2] = (addr >> 8) & 0xFF;
  reply_buffer[3] = (addr >> 16) & 0xFF;
  reply_buffer[4] = (addr >> 24) & 0xFF;
  reply_length = 5;

  compute_reply_crc ();
  force_xmit_reply();

  TPRINTF(4,("handle_unframed_stream_write: exit\n"));

  return 0;
}
/*===========================================================================

FUNCTION packet_process

DESCRIPTION
  This function processes a command packet, validates the same and invokes the
  appropriate command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

int (*packet_handler[]) (incoming_t) =
{
  handle_hello,               /*  0x01 -  hello.                    */
  handle_invalid,             /*  0x02 -    hello reply             */
  handle_simple_read,         /*  0x03 -  simple read.              */
  handle_invalid,             /*  0x04 -    simple read reply.      */
  handle_simple_write,        /*  0x05 -  simple write.             */
  handle_invalid,             /*  0x06 -    simple write reply.     */
  handle_stream_write,        /*  0x07 -  stream write.             */
  handle_invalid,             /*  0x08 -    block written.          */
  handle_sync,                /*  0x09 -  sync.                     */
  handle_invalid,             /*  0x0A -    sync reply.             */
  handle_reset,               /*  0x0B -  reset.                    */
  handle_invalid,             /*  0x0C -    reset reply.            */
  handle_invalid,             /*  0x0D -    error msg.              */
  handle_invalid,             /*  0x0E -    log   msg.              */
  handle_invalid,             /*  0x0F -  invalid (was unlock)      */
  handle_invalid,             /*  0x10 -   unlock reply.            */
  handle_power_off,           /*  0x11 -  pwr-off.                  */
  handle_invalid,             /*  0x12 -    pwr-off reply.          */
  handle_open,                /*  0x13 -  open.                     */
  handle_invalid,             /*  0x14 -    open reply.             */
  handle_close,               /*  0x15 -  close.                    */
  handle_invalid,             /*  0x16 -    close reply.            */
  handle_security_mode,       /*  0x17 -  security mode.            */
  handle_invalid,             /*  0x18 -    security mode reply.    */
  handle_parti_tbl,           /*  0x19 -  partition table.          */
  handle_invalid,             /*  0x1A -    partition table reply.  */
  handle_open_multi,          /*  0x1B -  open multi-image.         */
  handle_invalid,             /*  0x1C -    open multi-image reply. */
  handle_invalid,             /*  0x1D -  Unimplemented command     */
  handle_invalid,             /*  0x1E -  Unimplemented command     */
  handle_invalid,             /*  0x1F -  Unimplemented command     */
  handle_invalid,             /*  0x20 -  Unimplemented command     */
  handle_invalid,             /*  0x21 -  Unimplemented command     */
  handle_invalid,             /*  0x22 -  Unimplemented command     */
  handle_invalid,             /*  0x23 -  Unimplemented command     */
  handle_invalid,             /*  0x24 -  Unimplemented command     */
  handle_invalid,             /*  0x25 -  Unimplemented command     */
  handle_invalid,             /*  0x26 -  Unimplemented command     */
  handle_invalid,             /*  0x27 -  Unimplemented command     */
  handle_invalid,             /*  0x28 -  Unimplemented command     */
  handle_invalid,             /*  0x29 -  Unimplemented command     */
  handle_invalid,             /*  0x2A -  Unimplemented command     */
  handle_invalid,             /*  0x2B -  Unimplemented command     */
  handle_invalid,             /*  0x2C -  Unimplemented command     */
  handle_invalid,             /*  0x2D -  Unimplemented command     */
  handle_invalid,             /*  0x2E -  Unimplemented command     */
  handle_invalid,             /*  0x2F -  Unimplemented command     */
  handle_unframed_stream_write, /*0x30 -  unframed stream write     */
  handle_invalid,             /*  0x31 -    stream written          */
};


void
packet_process (void)
{
  incoming_t this;
  int command, pos, count;
  word crc;

  CHECK_FOR_DATA ();

  while (waiting != NULL)
  {
    /* Enable flag, only used by USB */
    process_packet_flag = TRUE;
    
    /* Pull it out of the waiting queue. */
    this = waiting;
    waiting = waiting->next;
    if (waiting == NULL)
      waiting_last = NULL;
    this->next = NULL;

    /* Need to verify the CRC.  Unfortunately, the time to compute the
       CRC of the buffer when running out of ram is longer than the
       time to fill up the FIFO. */
    crc = CRC_16_L_STEP_SEED;
    pos = 0;
    while (pos < this->length)
    {
      CHECK_FOR_DATA ();
      count = this->length - pos;
      if (count > 128)
        count = 128;
      crc = crc_16_l_step (crc, this->buffer + pos, count); /*lint !e734 */
      pos += count;
    }

#ifdef DROP_PACKETS
    /* Simulate dropping of packets */
    drop_count++;
    if (drop_count%DROP_MOD == 0)
    {
      term_put('p');
      DPRINTF(("drop pkt %d\n", drop_count));
      goto bogus_packet;        /*lint !e801 */
    }
#endif
    
    if (crc != CRC_16_L_OK)
    {
#ifdef RESET_UART_ON_CRC_ERR
      if (!uart_rx_reset)
      {
        /* There can be multiple packets queued up and each will
         * have CRC errors. We only need to reset the uart receiver 
         * once to correct the error condition. */
        uartdm_init_receiver();
        uart_rx_reset = TRUE;
      }
#endif
      xmit_log_message ("ERR: CRC invalid");
      goto bogus_packet;        /*lint !e801 */
    }
    else
    {
#ifdef RESET_UART_ON_CRC_ERR
      uart_rx_reset = FALSE;
#endif
      this->length -= 2;
    }

    command = this->buffer[0];

    if (command >= FIRST_COMMAND && command <= LAST_COMMAND
        && (!need_hello || command == 1))
    {
      if (flash_dev_type != FLASH_DEVICE_FOUND)   /*lint !e650 */
      {
        transmit_error (NAK_WRONG_IID, "Unrecognized flash device");
      }
      else
      {
        (*packet_handler[command - FIRST_COMMAND]) (this);      /*lint !e522 */
      }
    }
    else
    {
      handle_invalid (this);    /*lint !e534 */
    }

  bogus_packet:
    /* Add back to free list. */
    this->next = free_list;
    free_list = this;
  }

  /* Disable flag, processing is complete */
  process_packet_flag = FALSE;

}


/*===========================================================================

FUNCTION packet_loop

DESCRIPTION
  This function continuously polls the UART for input characters, and processes
  them into receive buffers. It kicks the watchdog to keep the system alive.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void
packet_loop (void)
{
  for (;;)
  {
    CHECK_FOR_DATA ();
    KICK_WATCHDOG ();
    packet_process ();
    KICK_WATCHDOG ();
  }
}
