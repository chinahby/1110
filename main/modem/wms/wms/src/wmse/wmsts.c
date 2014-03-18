/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            W I R E L E S S   M E S S A G I N G   S E R V I C E S
            -- Translation Services

GENERAL DESCRIPTION
  The WMS module which implements the User API for SMS. This source file
  implements the translation functions for the client applications to call
  and for internal use.

Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008
              by QUALCOMM INCORPORATED.
  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$PVCSPath:  L:/src/asw/MSM_MM/Services/wms/vcs/wmsts.c_v   1.20   01 Jul 2002 16:41:30   hqu  $
$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/src/WMSE/wmsts.c#3 $ $DateTime: 2008/12/02 13:02:59 $ $Author: pyaramad $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/24/08   PMD     Removed Tabs from code
06/04/08   PMD     Removed support for Auto / Transparent SMS 
05/28/08   PMD     Fixed ascii_to_def_alpha_table Array 
05/19/08   PMD     Fixed CR in wms_ts_decode_gw_cb_dcs()
01/31/08   PMD     Updated Copyright Information to include 2008
01/14/07   PMD     Guard against Buffer Overflow in wms_ts_encode_command()
07/23/07   PMD     Fixed compile warning due to missing endif comment braces
07/20/07   PMD     Moved Feature inside wms_ts_encode/decode_user_data_header
06/29/07   PMD     Fix for more Compile Warnings
06/28/07   PMD     Fixed Compile Warnings 
06/20/07   PMD     Cleaned up file for internal functions - moved to wmstspriv
05/15/07   PMD     Added wms_ts_init() function
03/21/07   HQ      Check for RP_OA/RP_DA being present or not.
03/12/07   PMD     Fixed potential memory corruption in function
                   wms_ts_convert_from_gstk_address()
02/20/07   HQ      Fixed lint warnings.
02/13/07   HQ      Added support for Extended/Long SMS.
01/18/07   HQ      Fixed Lint errors 661 and 662.
01/17/07   HQ      Updated copyright info to include 2007.
01/15/07   NP      For GSM 7 bit encoding, '_' symbol being encoded incorrectly
01/12/07   NP      Fix lint warning 539 introduced by previous change
01/05/07   NP      Extended object decoding depends on order of UDH.  Added
                   new function to check for first segment during udh decoding.
11/29/06   PMD     Fixed Null Pointer deref in wms_ts_decode_user_data_header()
10/31/06   HQ      Fixed wrong comments for wms_ts_unpack_gw_7_bit_chars().
10/25/06   PMD     Fix High Lint Warning due to possible Memory overrun.
10/24/06   PMD     Removed SC Address Validity check in wms_ts_unpack_sim_sms()
                   and handling of zero SC Address Length
09/29/06   NP      Fixed some Lint warnings (e661-Possible access of 
                   out-of-bounds pointer) for GW code.
08/22/06   PMD     Removed check in wms_ts_upack/pack_gw_7_bit_chars().
08/14/06   PMD     Mainlined / Replaced FEATURE_GWSMS_UDH in code
08/14/06   PMD     Mainlined / Replaced FEATURE_SMS_UDH_EMS in code
08/14/06   PMD     Eliminated sts_rpt Global Variable
08/14/06   PMD     Mainlined FEATURE_GWSMS_STATUS_REPORTS in code.
08/14/06   PMD     Mainlined FEATURE_GWSMS_UDH_WAP in code.
08/14/06   PMD     Mainlined FEATURE_CDSMS_UDH_WAP in code.
07/06/06   PMD     Fixed Decoding Var Picture Check
                   Removed check in wms_ts_decode_udh_user_def_sound()
05/18/06   PMD     Changed GSM Default to ASCII Character Conversion for '_'
05/03/06   PMD     Added Function Headers
04/26/06   PMD     Fixed bad mesage being sent out in case of Zero Length Data
04/24/06   HQ      Added additional Null pointer checks.
04/11/06   HQ      Fixes for cleaning up Lint Errors.
03/09/06   PMD     Fixed decode_gw_user_data bug with len = 0
02/22/06   PMD     Fixed wms_ts_decode_udh_rfc822 bug
02/06/06   PMD     Fixed Compile Warnings
01/25/06   VK      Updated Copyright Info
12/21/05   VK      Lint fixes  
01/24/06   PMD     Fix for checking length in wms_ts_unpack_sim_sms()
12/20/05   PMD     Cleaned up FEATURE_SIRIUS from Code Base.
12/16/05   PMD     Fixes for checking Length
11/21/05   VK      Feature define fixes
11/18/05   VK      Cleaned up Lint warnings.
10/20/05   PMD     Added checks in wms_ts_decode/encode_gw_validity
10/20/05   VK      Changed byte = uint8, word = uint16, dword = uint32
10/20/05   VK      Removed Lint Suppressions & Lint error/warning fixes
10/20/05   VK      Updated Copyright info
10/19/05   VK      EMS Featurization fixes
10/18/05   VK      Fixes decoding issue with ASCII & IA5
09/26/05   PMD     Skipped UDL byte while checking UDH Length.
09/16/05   PMD     Commented User Prompt and Extended Object code under 
                   FEATURE_SIRIUS
09/09/05   PMD     Removed Raw DCS mismatch check in encode_dcs
08/05/05   Rex     Code cleanup
08/04/05   PMD     Fixed certain Lint Warnings in code.
07/11/05   PMD     Added support for raw_dcs_data in GW DCS.
06/17/05   PMD     Fixed wms_ts_decode_gw_cb_dcs function
06/07/05   Rex     Fixed wms_ts_decode_deliver for replace sm type 1
03/08/05   PMD     Fixed Compile Warning.
02/25/05   Rex     Added additional VMWI support
02/23/04   VK      Added an extra octet - text color - udh_text_formatting
02/24/05   Rex     Fix UDH length checking
02/14/05   Rex     Length checking for memcpy and fixes for integer underflow/overflow
02/11/05   VK      Fix in decode_udh_text_formating
01/07/05   PMD     Added and used function wms_ts_compute_gw_user_data_length()
                   Added and used function wms_ts_compute_user_data_header_length()
12/15/04   PMD     Moved Memset in GW Encode Functions
12/15/04   Rex     Fixed buffer length for gw address storage
12/03/04   Rex     Preset timezone value when decoding timestamp failed
10/21/04   PMD     Updated the Checking logic in Encode GW User Data
10/06/04   PMD     Changed the Width Parameter in encode and decode var pic udh
09/29/04   PMD     Added User Data Length Checks in Encode and Decode functions
09/22/04   Rex     Fixed wms_ts_encode_udh_eo
09/03/04   Rex     Fixed wms_ts_encode_udh_special_sm
09/03/04   PMD     Fixed GW encoding and 7-bit packing logic
09/01/04   HQ      Update for UDH User Prompt and Extended Object.
07/15/04   PMD     Added check for max headers in wms_ts_decode_user_data_header
07/15/04   PMD     Modified wms_ts_decode_gw_cb_dcs to handle NULL User Data
06/29/04   PMD     Added Fixes for encoding 8-bit user data with headers
06/02/04   HQ      Changed class to msg_class to fix C++ compilation issue.
05/13/04   Rex     Fixed proper bit shift when packing gw 7 bit character
05/03/04   Rex     Fixed proper checking of address length for gw
03/09/04   PMD     Allow Message to go through even if Timestamp Decoding Fails
02/26/04   PMD     Fixed User Data Length in wms_ts_pack_gw_user_data()
02/25/04   PMD     Modified wms_ts_encode_timestamp to handle zero differently
02/14/04   Rex     Remove compile warning in code
02/14/04   PMD     Added wms_ts_encode_gw_cb()
02/04/04   PMD     Cleaned up Lint Warnings in code
02/02/04   PMD     Fixed problem with Validity Format = NONE in wms_ts_decode_submit
                   Added some more checks in wms_ts_pack_gw_user_data()
01/30/04   PMD     Fixed Merging Bug (data) in decode
01/29/04   Rex     Added wms_ts_bcd_to_int and wms_ts_int_to_bcd for checking
                   in wms_ts_decode/encode_timestamp and wms_ts_decode/encode_relative_time.
01/29/04   HQ      Changed WMS_NUMBER_RESERVED_5 to WMS_NUMBER_ALPHANUMERIC.
01/26/04   PMD     Added function wms_ts_get_udh_length()
01/23/04   PMD     Moved Features within wms_ts_encode_dcs and wms_ts_decode_dcs
01/20/04   PMD     More fixes in wms_ts_decode_address
01/19/04   PMD     Added support for Alphanumeric Number Type
12/15/03   PMD     Added 1x Broadcast Support in Multimode Broadcast API
12/02/03   HQ      Resolved some Lint errors.
11/14/03   Rex     Check for max data length when packing/unpacking sim_sms and sim_smsr
11/04/03   AP      Added client id authentication check and missing pieces for
                   FEATURE_SMS_TRANSPARENT_MO.
10/30/03   PMD     Changed Encode and Decode Relative Time fn for 1 day
10/29/03   AP      Added support for FEATURE_SMS_TRANSPARENT_MO.
10/06/03   PMD     Check for valid SC Address during encoding and packing
                   for templates.
09/18/03   HQ      Check SC address length range during unpacking.
08/18/03   HQ      Fixed compile problem.
08/18/03   HQ      Check WMS_ADDRESS_MAX in address encoding/decoding;
                   check failure of address encoding/decoding result;
                   removed some unused code.
08/15/03   PMD     Fixed Display of @ character
08/07/03   Rex     Fixed received dcs value of 12 to interpret as GSM 7-bit.
08/06/03   Rex     Fixed TP-MMS field with correct assignmet of bit-2 value.
08/04/03   Rex     Fixed decoding reserved values in default case to 7-bit.
08/04/03   PMD     Changes to suppress / remove Lint Errors
07/29/03   HQ      Added support for FEATURE_BROADCAST_SMS_MULTIMODE:
                      - New BC_MM commands, events and structures.
                      - New BC_MM API functions.
                      - GSM CB support.
                      - 1x BC SMS is still supported via BC group, not BC_MM.
06/25/03   AP      Fixed decoding user data for 8-bit.
06/17/03   HQ      Allow 0 length UDH data.
06/11/03   SUN     Fixed Style Text decoding
06/08/03   PMD     Check and Support for Dynamic Alpha ID in GW Template
05/29/03   SUN     Fixed Style Text decoding
05/27/03   PMD     Fixed Decoding of User_Def_Sound data header
04/22/03   AP      Updated conversions for to/from gstk_address.
04/21/03   PMD     Added code for proper formatting of Destination Number on
                   screen (bcd to ascii conversion)
04/03/03   PMD     Added check for a valid mask in Status Report
04/02/03   HQ      Set num_headers to 0 if UDH is not defined.
04/01/03   HQ/PMD  Strip top 4 bits in bcd_to_ascii().
03/17/03   HQ      Changed GSTK address data format.
02/28/03   HQ      Added support for CAT.
02/28/03   SUN     Fix Decoding of "other" udh data
02/07/03   SUN     Added Support for Status Reports and Commands
01/30/03   SUN     Set the Data to FFs before packing, added error checking
                   for SC Address
01/20/03   HQ      Changed timezone range to [-48,48] which is [-12,12] in hour.
12/5/02    SUN     Fixed Pack/Unpack for EMS
11/01/02   HQ      Reorganized the feature flags for WAP UDH support.
10/30/02   SUN     Fixed DCS, Handled Non- Supported UDHs properly
10/24/02   SUN     Added support for WAP
10/02/02   HQ      Fixed a compile problem with wms_ts_decode_sc_address.
09/17/02   SUN     Chnaged the timezone range to -47 to +47(12 hrs)
09/12/03   SUN     Fixed Errors for encoding and decoding SC Address
09/09/02   SUN     Added Support for Concatenation
09/03/02   SUN     Fixed decode_relative_time
                   Fixed errors with FEATURE_CDSMS_UDH_EMS
07/31/02   HQ      Added support of the following features:
                     - FEATURE_GWSMS_UDH_EMS
                     - FEATURE_CDSMS_UDH_EMS
                     - FEATURE_GWSMS_UDH
                     - FEATURE_CDSMS_UDH
                   Changed prototypes of wms_ts_encode_user_data_header()
                   and wms_ts_decode_user_data_header().
07/29/02   SUN     Fixed DCS Decoding
07/24/02   SUN     Added EMS Changes,
                   Fixed SC Address Length,
                   Fixed Timestamp Decode/Encode
06/29/02   HQ      Made ascii_to_def_alpha_table static.
06/03/02   SUN     Fixed 0 PDU Length issue
                   Fixed the number of fill bits added in UDH
05/16/02   SUN     Fixed Template Decode/Encode Error
05/14/02   SUN     Initialised num_headers to 0 during decoding
05/07/02   SUN     Fixed SC address decode/encode error
05/07/02   HQ      Added more support for FEATURE_CDSMS.
05/06/02   SUN     Added support for User Data Headers
04/30/02   SUN     Fixed PDU length ,during decoding user data
04/18/02   SUN     Added format type during Sim Unpack function
03/13/02   HQ      Added support of Multimode WMS - added FEATURE_CDSMS.
03/12/02   SUN     Fixed Timezone range to [-96,96]
03/07/02   SUN     Fixed length problem during  unpacking SIM data
02/11/02   HQ      Return the raw ts data len during unpacking SIM data.
01/25/02   HQ      Fixed problem of address conversion.
01/22/01   HQ      Changed order of TPDU encoding/decoding on the first byte.
1/17/01    SUN     Fixed Addr Length and UDL for encoding/decoding and alpha tag
                   length for templates
12/13/01   HQ      Renamed UASMS to WMS;
                   fixed user data packing/unpacking problem.
12/07/01   HQ      Initial version for MSM_MM WMS.
11/26/01   SUN     Fixed encoding problems
11/9/01    HQ      Cleaned up compiler warnings.
08/27/01   HQ      Initial version.
===========================================================================*/


/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/


#include "comdef.h"

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

#include "wms.h"
#include "wmsts.h"
#include "wmspriv.h"
#include "msg.h"
#include "bit.h"
#include "string.h"
#include "err.h"
#include "rex.h"

#ifdef FEATURE_CDSMS
#include "wmstscdma.h"
#endif /* FEATURE_CDSMS */

/*===========================================================================
======================== STATIC DATA ========================================
===========================================================================*/

static const uint8 ascii_to_def_alpha_table[] =
{
0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x0A,0x2E,0x2E,0x0D,0x2E,0x2E,
0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,0x2E,
0x20,0x21,0x22,0x23,0x02,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
0x00,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x28,0x2F,0x29,0x2E,0x11,
0x2F,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x21,0x2F,0x29,0x2D,0x2E
};

static const uint8 def_alpha_to_ascii_table[] =
{
 '@', '#', '$', 'Y', 'e', 'e', 'u', 'i', 'o', 'C',0x0A, 'O', 'o',0x0D, 'A', 'a',
 'D', '_', '?', '?', 'L', 'O', 'P', '?', 'S', 'T', '?', ' ', 'A', 'a', 's', 'E',
 ' ', '!',0x22, '#', '?', '%', '&',0x27, '(', ')', '*', '+', ',', '-', '.', '/',
 '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',
 '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'A', 'O', 'N', 'U', 'S',
 '?', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'a', 'o', 'n', 'u', 'a'
};

typedef struct
{
  uint16   unicode;
  uint8    gsm_def;
} unicode_to_gsm_def;

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_SMS_UDH
enum { WMS_UDH_OCTETS_PORT8                =2};
enum { WMS_UDH_OCTETS_PORT16               =4};
enum { WMS_UDH_OCTETS_CONCAT8              =3};
enum { WMS_UDH_OCTETS_SPECIAL_SM           =2};
enum { WMS_UDH_OCTETS_CONCAT16             =4};
enum { WMS_UDH_OCTETS_PRE_DEF              =2};
enum { WMS_UDH_OCTETS_TEXT_FORMATTING      =3};
enum { WMS_UDH_OCTETS_RFC822               =1};
enum { WMS_UDH_OCTETS_USER_PROMPT          =1};
enum { WMS_UDH_OCTETS_EO_HEADER            =7};
static const wms_udh_s_type *const_header;
#endif

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

rex_crit_sect_type encode_decode_data_crit_sect;
boolean            encode_decode_data_crit_sect_init = FALSE;

#if defined(FEATURE_SMS_UDH) && !defined(FEATURE_SMS_EXTENDED)
static boolean wms_ts_udh_decode_first_seg_check
(
  const uint8               len,                /* user_data_length*/
  const uint8               *data,              /* first byte of user data */
  boolean                   *is_first_segment_ptr /* OUT */
);
#endif /* FEATURE_SMS_UDH && ! FEATURE_SMS_EXTENDED */

/*===========================================================================
============================ FUNCTIONS ======================================
===========================================================================*/


/*=========================================================================
FUNCTION
  wms_ts_bcd_to_int

DESCRIPTION
  Convert a BCD to Integer.
  Valid BCD values altogether are from 0 to 99 and byte arrangement is <MSB, ... ,LSB)

DEPENDENCIES
  None

RETURN VALUE
  Failure if either of the BCD digit is invaild (greater than 9, e.g. 11111010 = 15 and 10)
  Success if both BCD digits are valid

SIDE EFFECTS
  None
=========================================================================*/
boolean wms_ts_bcd_to_int
(
  const uint8 bcd, /*IN*/
  uint8 *result    /*OUT*/
)
{
  if ( (bcd & 0x0F) > 9 || ((bcd & 0xF0) >> 4) > 9)
  {
    MSG_ERROR("Invalid BCD digit!",0,0,0);
    *result = 0;
    return FALSE;
  }
  else
  {
    *result = ( (bcd & 0x0F) + (((bcd & 0xF0) >> 4) * 10) );
    return TRUE;
  }
}


/*=========================================================================
FUNCTION
  wms_ts_int_to_bcd

DESCRIPTION
  Convert an Integer to BCD.
  Valid Integer values are from 0 to 99 and byte arrangement is <MSB, ... ,LSB)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_int_to_bcd
(
  const uint8 i
)
{
  return (uint8)(((i % 10) + ((i / 10) << 4)));
}

/*=========================================================================
FUNCTION
  wms_ts_ascii_to_default

DESCRIPTION
  Convert an ASCII string to a GSM 7-bit Default alphabet string.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_ts_ascii_to_default
(
  const uint8   * asc,
  uint32        len,
  uint8         * def
)
{
  uint32  i, j;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( i = 0; i < len; i ++ )
  {
    j = asc[i];
    def[i] = ascii_to_def_alpha_table[j];
  }

} /* wms_ts_ascii_to_default() */



/*=========================================================================
FUNCTION
  wms_ts_default_to_ascii

DESCRIPTION
  Convert 7-bit GSM Default alphabet string to an ASCII string.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_ts_default_to_ascii
(
  const uint8   * def,
  uint32        len,
  uint8         * asc
)
{
  uint32  i, j;

  for( i = 0; i < len; i ++ )
  {
    j = def[i];
    asc[i] = def_alpha_to_ascii_table[j];
  }

} /* wms_ts_default_to_ascii() */


/*=========================================================================
FUNCTION
  wms_ts_bcd_to_ascii

DESCRIPTION
  Convert BCD digits to an ASCII number string.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_ts_bcd_to_ascii
(
  const uint8         * addr,
  uint8               len,
  uint8               * out
)
{
  uint8   i;

  for( i=0; i<len; i++ )
  {
    uint8   digit; /* to store stripped digit */

    digit = addr[i] & 0x0F;

    switch( digit )
    {
      case 0xA:
        out[i] = '*';
        break;
      case 0xB:
        out[i] = '#';
        break;
      case 0xC:
      case 0xD:
      case 0xE:
        out[i] = (digit - 0xC) + 'a';
        break;
      case 0xF:
        /* Ignore it */
        out[i] = ' ';
        break;
      default:
        out[i] = digit + '0';
        break;
    }
  }

  /* done */
  return;

} /* wms_ts_bcd_to_ascii() */



/*=========================================================================
FUNCTION
  wms_ts_ascii_to_bcd

DESCRIPTION
  Convert an ASCII number string to BCD digits.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_ts_ascii_to_bcd
(
  const uint8     * ascii,
  uint8           len,
  uint8           * out
)
{
  uint8   i;

  for( i=0; i<len; i++ )
  {
    switch( ascii[i] )
    {
      case '*':
        out[i] = 0xA;
        break;
      case '#':
        out[i] = 0xB;
        break;
      default:
        out[i] = ascii[i] - '0';
        break;
    }
  }

  /* done */
  return;

} /* wms_ts_ascii_to_bcd() */


/*=========================================================================
FUNCTION
  wms_ts_encode_address

DESCRIPTION
  Encode address structure into format for SIM storage and for interfacing
  with lower layers.

DEPENDENCIES
  None

RETURN VALUE
  0: Failure
  Otherwise: Number of bytes encoded.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_encode_address
(
  const wms_address_s_type    * addr,
  uint8                       * data
)
{
  uint8   i, pos = 0;
  
  if( addr->number_of_digits > WMS_GW_ADDRESS_MAX )
  {
    MSG_ERROR("Addr len too long: %d", addr->number_of_digits,0,0);
    return 0;
  }

  if (addr->number_type == WMS_NUMBER_ALPHANUMERIC )
  {
    data[pos] = (uint8)((addr->number_of_digits*7 + 3)/4);
  }
  else
  {
    /* Len field: */
    data[pos] = addr->number_of_digits;
  }
  pos ++;

  /* TON & NPI: */
  data[pos] = 0x80;
  data[pos] |= (uint8) ((uint8) addr->number_type << 4);
  data[pos] |= (uint8) addr->number_plan;
  pos ++;

  if (addr->number_type == WMS_NUMBER_ALPHANUMERIC )
  {
    /* Alphanumberic Number Type */
    pos += (uint8)wms_ts_pack_gw_7_bit_chars
              (
                addr->digits,
                addr->number_of_digits,
                0,
                WMS_GW_ADDRESS_MAX,
                &data[pos]
              );
  }
  else
  {
    /* the digits: */
    for( i=0; i<addr->number_of_digits; i++ )
    {
      /* pack two digits each time */
      data[pos]  = (uint8) (addr->digits[i++] & 0x0F);

      if( i == addr->number_of_digits )
      {
        data[pos] |= 0xF0;
      }
      else
      {
        data[pos] |= (uint8)(addr->digits[i] << 4);
      }
      pos ++;
    }
  }
  
  /* done */
  return pos;
} /* wms_ts_encode_address() */

/*=========================================================================
FUNCTION
  wms_ts_encode_sc_address

DESCRIPTION
  Encode address structure into format for SIM storage and for interfacing
  with lower layers.

DEPENDENCIES
  wms_ts_encode_address

RETURN VALUE
  None.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_encode_sc_address
(
  const wms_address_s_type    * addr,
  uint8                       * data
)
{
  uint8  pos =0;
  pos += wms_ts_encode_address( addr, data);

  data[0] = (uint8)(pos-1);

  if( data[0] < 2 || data[0] > 11 )
  {
    MSG_ERROR("Bad SC Addr len: %d", data[0], 0,0);
    return 0;
  }

  return pos;
}
/*=========================================================================
FUNCTION
  wms_ts_decode_address

DESCRIPTION
  Decode address data into a structure.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes decoded.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_decode_address
(
  const uint8               * data,
  wms_address_s_type        * addr
)
{
  uint8   i, pos = 0;

  /* Len field: number of digits */
  i = data[pos];

  if( i > WMS_GW_ADDRESS_MAX )
  {
    /* Address is too long */
    MSG_ERROR("Addr len too long: %d", i,0,0);
    return 0;
  }

  addr->number_of_digits = i;

  pos ++;

  /* TON & NPI: */
  addr->digit_mode  = WMS_DIGIT_MODE_4_BIT;
  addr->number_type = (wms_number_type_e_type) (( data[pos] & 0x70 ) >> 4);
  addr->number_plan = (wms_number_plan_e_type) (data[pos] & 0x0F);
  pos ++;
  
  if (addr->number_type == WMS_NUMBER_ALPHANUMERIC )
  {
    uint8 bytes_increment=0;
    /* Alphanumberic Number Type */
    addr->digit_mode = WMS_DIGIT_MODE_8_BIT;

    /* length = number of BCD digits */
    bytes_increment = (addr->number_of_digits+1)/2;

    addr->number_of_digits = (uint8)(addr->number_of_digits*4/7);

    (void)wms_ts_unpack_gw_7_bit_chars
           (
             &data[pos],
             addr->number_of_digits,
             WMS_GW_ADDRESS_MAX,
             0,
             addr->digits
            );

    pos += bytes_increment;
  }
  else
  {
    /* the digits: */
    for( i=0; i<addr->number_of_digits; i++ )
    {
      /* unpack two digits each time */
      addr->digits[i++] = data[pos] & 0x0F;
      addr->digits[i]   = ( data[pos] & 0xF0 ) >> 4;
      pos ++;
    }
  }
  return pos;
} /* wms_ts_decode_address() */

/*=========================================================================
FUNCTION
  wms_ts_decode_sc_address

DESCRIPTION
  Decode SC address data into a structure.

DEPENDENCIES
  wms_ts_decode_address

RETURN VALUE
0: Failure
Otherwise: bytes decoded

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_decode_sc_address
(
  const uint8 *data,
  wms_address_s_type *addr
)
{
  uint8  buf[(WMS_GW_ADDRESS_MAX/2) + 2];
  int    length;

  if (data == NULL || addr == NULL)
  {
    MSG_ERROR("null pointer in wms_ts_decode_sc_address",0,0,0);
    return 0;
  }

  /*Check valid range for sc address, number type/plan (1 byte) and dest addr (between 1 to 10 bytes)*/
  if( data[0] < 2 || data[0] > 11 )
  {
    MSG_ERROR("Bad SC Addr len: %d", data[0], 0,0);
    return 0;
  }

  /*Get total number of bytes, includes 1 byte of addr_length */
  length = data[0]+1;

  memcpy( buf, (void*)(data), (uint32)length );

  /*Calculate number of digits */
  buf[0] =(uint8)( 2 * (buf[0]-1));

  if( (data[data[0]] & 0xF0) == 0xF0)
  {
    buf[0] -= 1;
  }

  return wms_ts_decode_address( buf, addr );
}

/*=========================================================================
FUNCTION
  wms_ts_pack_gw_7_bit_chars

DESCRIPTION
  Pack GSM 7-bit characters into raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes encoded.

SIDE EFFECTS
  None
=========================================================================*/
uint16 wms_ts_pack_gw_7_bit_chars
(
  const uint8     * in,
  uint16          in_len,      /* Number of 7-bit characters */
  uint16          shift,
  uint16          out_len_max, /* Number of 7-bit characters */
  uint8           * out
)
{
  uint16    i=0;
  uint16    pos = 0;

  if (in == NULL || out == NULL)
  {
    MSG_ERROR("null pointer in wms_ts_pack_gw_7_bit_chars",0,0,0);
    return 0;
  }
  /* pack the ASCII characters */
  shift %= 7;

  if (shift != 0)
  {
    /* Pack the upper bytes of the first character */
    out[pos] |= (uint8) (in[i] << shift);
    shift = (7 - shift) + 1;
    if (shift == 7)
    {
      shift = 0;
      i++;
    }
    pos++;
  }

  for( ; pos < out_len_max && i < in_len;
       pos++, i++ )
  {
    /* pack the low bits */
    out[pos] = in[i] >> shift;

    if( i+1 < in_len )
    {
      /* pack the high bits using the low bits of the next character */
      out[pos] |= (uint8) (in[i+1] << (7-shift));

      shift ++;

      if( shift == 7 )
      {
        shift = 0;
        i ++;
      }
    }
  }

  /* done */
  return pos;

} /* wms_ts_pack_gw_7_bit_chars() */


/*=========================================================================
FUNCTION
  wms_ts_unpack_gw_7_bit_chars

DESCRIPTION
  Unpack raw data to GSM 7-bit characters.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes decoded.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_unpack_gw_7_bit_chars
(
  const uint8       * in,
  uint8             in_len,        /* Number of 7-bit characters */
  uint8             out_len_max,   /* Number of maximum 7-bit characters after unpacking */
  uint16            shift,
  uint8             * out
)
{
  int      i=0;
  uint16   pos=0;

  if (in == NULL || out == NULL)
  {
    MSG_ERROR("null pointer in wms_ts_unpack_gw_7_bit_chars",0,0,0);
    return 0;
  }

  /*If the number of fill bits != 0, then it would cause an additional shift*/
  if (shift != 0)
   pos = pos+1;

  if (shift ==7)
  {
    out[0] = in[0] >> 1; /*7 fillbits*/
    shift =0;            /*First Byte is a special case*/
    i=1;
  }

  for( i=i;
       i < out_len_max && i< in_len;
       i++, pos++ )
  {
    out[i] = ( in[pos] << shift ) & 0x7F;

    if( pos != 0 )
    {
      /* except the first byte, a character contains some bits
      ** from the previous byte.
      */
      out[i] |= in[pos-1] >> (8-shift);
    }

    shift ++;

    if( shift == 7 )
    {
      shift = 0;

      /* a possible extra complete character is available */
      i ++;
      if( i >= out_len_max )
      {
        MSG_ERROR("Not enough output buffer for unpacking!",0,0,0);
        break;
      }
      out[i] = in[pos] >> 1; 
    }
  }

  return(uint8)(pos);

} /* wms_ts_unpack_gw_7_bit_chars() */

/*=========================================================================
FUNCTION
  wms_ts_encode_dcs

DESCRIPTION
  Encode Data Coding Scheme into raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes encoded.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_encode_dcs
(
  const wms_gw_dcs_s_type   *dcs,  /* IN */
  uint8                     *data  /* OUT */
)
{
  uint8 pos = 0;
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
  return pos;
} /* wms_ts_encode_dcs() */


/*=========================================================================
FUNCTION
  wms_ts_encode_timestamp

DESCRIPTION
  Encode a timestamp into raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes encoded.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_encode_timestamp
(
  const wms_timestamp_s_type    *timestamp, /* IN */
  uint8                           *data     /* OUT */
)
{
  sint7     i;
  uint8     pos = 0, j;

  /* year check */
  if ( !wms_ts_bcd_to_int(timestamp->year, &j) )
  {
    return 0;
  }
  data[pos] = ((timestamp->year & 0x0F) << 4) + ((timestamp->year& 0xF0) >> 4);
  pos ++;

  /* month check */
  if ( wms_ts_bcd_to_int(timestamp->month, &j) )
  {
    if (j > 12 || j < 1)
    {
      MSG_ERROR("Month is invalid: %d", j, 0, 0);
      return 0;
    }
  }
  else
  {
    return 0;
  }
  data[pos] = ((timestamp->month & 0x0F) << 4) + ((timestamp->month & 0xF0) >> 4);
  pos ++;

  /* day check */
  if ( wms_ts_bcd_to_int(timestamp->day, &j) )
  {
    if (j > 31 || j < 1)
    {
      MSG_ERROR("Day is invalid: %d", j, 0, 0);
      return 0;
    }
  }
  else
  {
    return 0;
  }
  data[pos] = ((timestamp->day & 0x0F) << 4) + ((timestamp->day & 0xF0) >> 4);
  pos ++;

  /* hour check */
  if ( wms_ts_bcd_to_int(timestamp->hour, &j) )
  {
    if (j > 23)
    {
      MSG_ERROR("Hour is invalid: %d", j, 0, 0);
      return 0;
    }
  }
  else
  {
    return 0;
  }
  data[pos] = ((timestamp->hour & 0x0F) << 4) + ((timestamp->hour & 0xF0) >> 4);
  pos ++;

  /* minute check */
  if ( wms_ts_bcd_to_int(timestamp->minute, &j) )
  {
    if (j > 59)
    {
      MSG_ERROR("Minute is invalid: %d", j, 0, 0);
      return 0;
    }
  }
  else
  {
    return 0;
  }
  data[pos] = ((timestamp->minute & 0x0F) << 4) + ((timestamp->minute & 0xF0) >> 4);
  pos ++;

  /* second check */
  if ( wms_ts_bcd_to_int(timestamp->second, &j) )
  {
    if (j > 59)
    {
      MSG_ERROR("Second is invalid: %d", j, 0, 0);
      return 0;
    }
  }
  else
  {
    return 0;
  }
  data[pos] = ((timestamp->second & 0x0F) << 4) + ((timestamp->second & 0xF0) >> 4);
  pos ++;

  /* timezone check */
  i = (sint7)timestamp->timezone;
  if (i > 48|| i < -48)
  {
    MSG_ERROR("Timezone is out of bound: %d", (uint32) i, 0, 0);
    return 0;
  }

  if (i >= 0)
  {
    data[pos] = (uint8) (((uint8)( i % 10 )) << 4);
    data[pos] |= ( i / 10 );
  }
  else
  {
    i *= (-1);
    data[pos] = (uint8) (((uint8)( i % 10 )) << 4);
    data[pos] |= ( i / 10 );
    data[pos] |= 0x08;
  }
  pos ++;

  return pos;

} /* wms_ts_encode_timestamp() */



#ifdef FEATURE_SMS_UDH
static int wms_ts_encode_udh_concat_8
(
  uint8 *udh
)
{
  int pos =0;

  if ( const_header->u.concat_8.total_sm == 0 ||
       const_header->u.concat_8.seq_num  == 0 ||
       const_header->u.concat_8.seq_num > const_header->u.concat_8.total_sm)

  {
    MSG_ERROR("SMS UDH Header id %d Present with no Data",
               const_header->header_id, 0,
               0 );
    return 0;
  }


  udh[pos++] = (uint8) WMS_UDH_CONCAT_8;
  udh[pos++] = (uint8) WMS_UDH_OCTETS_CONCAT8;
  udh[pos++] = const_header->u.concat_8.msg_ref;
  udh[pos++] = const_header->u.concat_8.total_sm;
  udh[pos++] = const_header->u.concat_8.seq_num;

  return pos;
}/*wms_ts_encode_udh_concat_8*/

static int wms_ts_encode_udh_concat16
(
  uint8 *udh
)
{
  int pos=0;

  if (const_header->u.concat_16.total_sm == 0 ||
      const_header->u.concat_16.seq_num == 0 ||
      const_header->u.concat_16.seq_num > const_header->u.concat_16.total_sm)
  {
    MSG_ERROR("SMS UDH Header id %d Present with no Data",
               const_header->header_id, 0,
               0 );
    return 0;
  }

  udh[pos++] = (uint8) WMS_UDH_CONCAT_16;
  udh[pos++] = (uint8) WMS_UDH_OCTETS_CONCAT16;
  udh[pos++] = (uint8)((const_header->u.concat_16.msg_ref & 0xFF00) >> 8);
  udh[pos++] = (uint8)(const_header->u.concat_16.msg_ref & 0x00FF);
  udh[pos++] = const_header->u.concat_16.total_sm;
  udh[pos++] = const_header->u.concat_16.seq_num;

  return pos;
}/*wms_ts_encode_udh_concat16*/


#if !defined(FEATURE_SMS_EXTENDED)
int wms_ts_encode_udh_special_sm
(
  uint8 *udh
)
{
  int pos=0;
  udh[pos++] = (uint8) WMS_UDH_SPECIAL_SM;
  udh[pos++] = (uint8) WMS_UDH_OCTETS_SPECIAL_SM;

  /* store or discard the message */
  udh[pos] = (const_header->u.special_sm.msg_waiting == WMS_GW_MSG_WAITING_STORE) ? 0x80 : 0;
  udh[pos] |= (uint8) const_header->u.special_sm.msg_waiting_kind;
  pos++;
  udh[pos++] = const_header->u.special_sm.message_count;

  return pos;
}/*wms_ts_encode_udh_special_sm*/

static int wms_ts_encode_udh_port_8
(
  uint8 *udh
)
{
  int pos =0;

  udh[pos++] = (uint8) WMS_UDH_PORT_8;
  udh[pos++] = WMS_UDH_OCTETS_PORT8;
  udh[pos++] = const_header->u.wap_8.dest_port;
  udh[pos++] = const_header->u.wap_8.orig_port;

  return pos;
}/*wms_ts_encode_udh_port_8*/

static int wms_ts_encode_udh_port16
(
  uint8 *udh
)
{
  int pos =0;

  udh[pos++] = (uint8) WMS_UDH_PORT_16;
  udh[pos++] = (uint8) WMS_UDH_OCTETS_PORT16;
  udh[pos++] = (uint8)((const_header->u.wap_16.dest_port & 0xFF00) >> 8);
  udh[pos++] = (uint8)(const_header->u.wap_16.dest_port & 0x00FF);
  udh[pos++] = (uint8)((const_header->u.wap_16.orig_port & 0xFF00) >> 8);
  udh[pos++] = (uint8)(const_header->u.wap_16.orig_port & 0x00FF);

  return pos;

}/*wms_ts_encode_udh_port16*/

int wms_ts_encode_udh_text_formatting
(
  uint8 *udh
)
{
  int pos=0;

  udh[pos++] = (uint8) WMS_UDH_TEXT_FORMATING;

  if(const_header->u.text_formating.is_color_present)
  {
    udh[pos++] = (uint8) WMS_UDH_OCTETS_TEXT_FORMATTING + 1;
  }
  else
  {
    udh[pos++] = (uint8) WMS_UDH_OCTETS_TEXT_FORMATTING;
  }

  udh[pos++] = const_header->u.text_formating.start_position;
  udh[pos++] = const_header->u.text_formating.text_formatting_length;
  udh[pos]   = (uint8) const_header->u.text_formating.alignment_type;
  /*bit 1 and  bit 0*/
  udh[pos]   = (uint8) (((uint8)const_header->u.text_formating.font_size <<2) | udh[pos]);
  /*bit 3 and  bit 2*/
  udh[pos]   = (uint8)(const_header->u.text_formating.style_bold <<4)         | udh[pos];
  /*bit 4 */
  udh[pos]   = (uint8)(const_header->u.text_formating.style_italic <<5)       | udh[pos];
  /*bit 5 */
  udh[pos]   = (uint8)(const_header->u.text_formating.style_underlined<<6)    | udh[pos];
  /*bit 6 */
  udh[pos]   = (uint8)(const_header->u.text_formating.style_strikethrough<<7) | udh[pos];
  pos++;
  /*bit 7 */

  if(const_header->u.text_formating.is_color_present)
  {
    udh[pos] = 0;
    udh[pos] = (uint8) (const_header->u.text_formating.text_color_foreground)| udh[pos];
    /* bit 0-3 */
    udh[pos] = (uint8) ((uint8) const_header->u.text_formating.text_color_background<<4 | udh[pos]);
    /* bit 4-7 */
    pos++;
  }



  return pos;
}/*wms_ts_encode_udh_text_formatting*/

int wms_ts_encode_udh_pre_def_sound
(
  uint8 *udh
)
{
  int pos=0;

  udh[pos++] = (uint8) WMS_UDH_PRE_DEF_SOUND;
  udh[pos++] = (uint8) WMS_UDH_OCTETS_PRE_DEF;
  udh[pos++] = const_header->u.pre_def_sound.position;
  udh[pos++] = const_header->u.pre_def_sound.snd_number;

  return pos;
}/*wms_ts_encode_udh_pre_def_sound*/

int wms_ts_encode_udh_user_def_sound
(
  uint8 *udh
)
{
  int pos=0,j;

  if (const_header->u.user_def_sound.data_length == 0)
  {
     MSG_ERROR("SMS UDH Header id %d Present with no Data",
                const_header->header_id, 0,
                0 );
  }


  udh[pos++] = (uint8) WMS_UDH_USER_DEF_SOUND;
  udh[pos++] = const_header->u.user_def_sound.data_length+1;
  udh[pos++] = const_header->u.user_def_sound.position;

  for (j=0;j<const_header->u.user_def_sound.data_length;j++)
    udh[pos++] = const_header->u.user_def_sound.user_def_sound[j];

  return pos;
}/*wms_ts_encode_udh_user_def_sound*/


int wms_ts_encode_udh_pre_def_anim
(
  uint8 *udh
)
{
  int pos=0;

  udh[pos++] = (uint8) WMS_UDH_PRE_DEF_ANIM;
  udh[pos++] = (uint8) WMS_UDH_OCTETS_PRE_DEF;
  udh[pos++] = const_header->u.pre_def_anim.position;
  udh[pos++] = const_header->u.pre_def_anim.animation_number;

  return pos;
}/*wms_ts_encode_udh_pre_def_anim*/

int wms_ts_encode_udh_large_anim
(
  uint8 *udh
)
{
  int pos=0,j,k;

  udh[pos++] = (uint8) WMS_UDH_LARGE_ANIM;
  udh[pos++] = (uint8) WMS_UDH_LARGE_BITMAP_SIZE * (uint8) WMS_UDH_ANIM_NUM_BITMAPS  + 1;
  udh[pos++] = const_header->u.large_anim.position;

  for (j=0;j<WMS_UDH_ANIM_NUM_BITMAPS;j++)
    for (k=0;k<WMS_UDH_LARGE_BITMAP_SIZE;k++)
      udh[pos++] = const_header->u.large_anim.data[j][k];

  return pos;
}/*wms_ts_encode_udh_large_anim*/


int wms_ts_encode_udh_small_anim
(
  uint8 *udh
)
{
  int pos=0,j,k;

  udh[pos++] = (uint8) WMS_UDH_SMALL_ANIM;
  udh[pos++] = (uint8) (WMS_UDH_SMALL_BITMAP_SIZE * WMS_UDH_ANIM_NUM_BITMAPS) + 1;
  udh[pos++] = const_header->u.small_anim.position;


  for (j=0;j<WMS_UDH_ANIM_NUM_BITMAPS;j++)
    for (k=0;k<WMS_UDH_SMALL_BITMAP_SIZE;k++)
      udh[pos++] = const_header->u.small_anim.data[j][k];

  return pos;
}/*wms_ts_encode_udh_small_anim*/

int wms_ts_encode_udh_large_picture
(
  uint8 *udh
)
{
  int pos=0,j;

  udh[pos++] = (uint8) WMS_UDH_LARGE_PICTURE;
  udh[pos++] = (uint8) WMS_UDH_LARGE_PIC_SIZE +1;
  udh[pos++] = const_header->u.large_picture.position;

  for (j=0;j<WMS_UDH_LARGE_PIC_SIZE;j++)
    udh[pos++] = const_header->u.large_picture.data[j];

  return pos;
}/*wms_ts_encode_udh_large_picture*/


int wms_ts_encode_udh_small_picture
(
  uint8 *udh
)
{
  int pos=0,j;

  udh[pos++] = (uint8) WMS_UDH_SMALL_PICTURE;
  udh[pos++] = (uint8) WMS_UDH_SMALL_PIC_SIZE +1;
  udh[pos++] = const_header->u.small_picture.position;

  for (j=0;j<WMS_UDH_SMALL_PIC_SIZE;j++)
    udh[pos++] = const_header->u.small_picture.data[j];

  return pos;
}/*wms_ts_encode_udh_small_picture*/



int wms_ts_encode_udh_var_picture
(
  uint8 *udh
)
{
  int pos=0,j;

  udh[pos++] = (uint8) WMS_UDH_VAR_PICTURE;
  udh[pos++] = (uint8)((const_header->u.var_picture.height * const_header->u.var_picture.width/8) + 3);
  udh[pos++] = const_header->u.var_picture.position;
  udh[pos++] = const_header->u.var_picture.width/8;
  udh[pos++] = const_header->u.var_picture.height;

  for (j=0;j<(const_header->u.var_picture.height * const_header->u.var_picture.width/8);j++)
    udh[pos++] = const_header->u.var_picture.data[j];


  return pos;
}/*wms_ts_encode_udh_var_picture*/


int wms_ts_encode_udh_user_prompt
(
  uint8 *udh
)
{
  int pos=0;

  udh[pos++] = (uint8) WMS_UDH_USER_PROMPT;
  udh[pos++] = (uint8) WMS_UDH_OCTETS_USER_PROMPT; /* only 1 byte for the udh data */
  udh[pos++] = const_header->u.user_prompt.number_of_objects;

  return pos;
} /* wms_ts_encode_udh_user_prompt() */


int wms_ts_encode_udh_eo
(
  uint8 *udh
)
{
  int    pos = 0;
  uint8  udh_length;


  udh[pos++] = (uint8) WMS_UDH_EXTENDED_OBJECT;

  /* Pack UDH length */
  if( const_header->u.eo.first_segment == TRUE )
  {
    udh_length = WMS_UDH_OCTETS_EO_HEADER + const_header->u.eo.content.length;
  }
  else
  {
    udh_length = const_header->u.eo.content.length;
  }

  if( udh_length > WMS_UDH_EO_DATA_SEGMENT_MAX )
  {
    MSG_ERROR("UDH EO data too long: %d", udh_length,0,0);
    return 0;
  }

  udh[pos++] = udh_length;

  /* Pack EO header for the first segment */
  if( const_header->u.eo.first_segment == TRUE )
  {
    udh[pos++] = const_header->u.eo.reference;
    udh[pos++] = const_header->u.eo.length >> 8;
    udh[pos++] = const_header->u.eo.length & 0xFF;
    udh[pos++] = const_header->u.eo.control;
    udh[pos++] = (uint8) const_header->u.eo.type;
    udh[pos++] = const_header->u.eo.position >> 8;
    udh[pos++] = const_header->u.eo.position & 0xFF;
  }

  /* Pack content data */
  memcpy( udh+pos,
          const_header->u.eo.content.data,
          const_header->u.eo.content.length );
  pos += const_header->u.eo.content.length;

  return pos;

} /* wms_ts_encode_udh_eo() */

int wms_ts_encode_udh_rfc822
(
  uint8 *udh
)
{
  int pos=0;

  udh[pos++] = (uint8) WMS_UDH_RFC822;
  udh[pos++] = (uint8) WMS_UDH_OCTETS_RFC822;
  udh[pos++] = const_header->u.rfc822.header_length;

  return pos;
}/*wms_ts_encode_udh_rfc822*/


int wms_ts_encode_udh_other
(
  uint8 *udh,
  wms_udh_id_e_type header_id
)
{
  int i=0;
  int pos =0;

  udh[pos++] = (uint8) const_header->u.other.header_id;
  udh[pos++] = const_header->u.other.header_length;

  for(i = 0; i< const_header->u.other.header_length;i++)
  {
    udh[pos++] = const_header->u.other.data[i];
  }

  return pos;
}
#endif /* !defined FEATURE_SMS_EXTENDED */

#endif /* FEATURE_SMS_UDH */

/*=========================================================================
FUNCTION
  wms_ts_encode_user_data_header

DESCRIPTION
  Encode User Data structure into raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes encoded.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_encode_user_data_header
(
  uint8                           num_headers, /* IN */
  const wms_udh_s_type            * headers,   /* IN */
  uint8                           *data        /* OUT */
)
{
#ifdef FEATURE_SMS_UDH
   int i,pos=0;

   if (num_headers == 0)
     return 0;

   ++pos; /*Fill the user data header length later*/


   for (i=0;i<WMS_MAX_UD_HEADERS && i< num_headers;i++)
   {
     const_header = &headers[i];
     switch(const_header->header_id)
     {
       case WMS_UDH_CONCAT_8:
         pos+= wms_ts_encode_udh_concat_8(data+pos);
         break;

       case WMS_UDH_CONCAT_16:
         pos+= wms_ts_encode_udh_concat16(data+pos);
         break;

#if !defined(FEATURE_SMS_EXTENDED)
       case WMS_UDH_SPECIAL_SM:
         pos+= wms_ts_encode_udh_special_sm(data+pos);
         break;

       case WMS_UDH_PORT_8:
         pos+= wms_ts_encode_udh_port_8(data+pos);
         break;

       case WMS_UDH_PORT_16:
         pos+= wms_ts_encode_udh_port16(data+pos);
         break;

       case WMS_UDH_TEXT_FORMATING:
         pos+= wms_ts_encode_udh_text_formatting(data+pos);
         break;


       case WMS_UDH_PRE_DEF_SOUND:
         pos+= wms_ts_encode_udh_pre_def_sound(data+pos);
         break;

       case WMS_UDH_USER_DEF_SOUND:
         pos+= wms_ts_encode_udh_user_def_sound(data+pos);
         break;

       case WMS_UDH_PRE_DEF_ANIM:
         pos+= wms_ts_encode_udh_pre_def_anim(data+pos);
         break;

       case WMS_UDH_LARGE_ANIM:
         pos+= wms_ts_encode_udh_large_anim(data+pos);
         break;

       case WMS_UDH_SMALL_ANIM:
         pos+= wms_ts_encode_udh_small_anim(data+pos);
         break;

       case WMS_UDH_LARGE_PICTURE:
         pos+= wms_ts_encode_udh_large_picture(data+pos);
         break;

       case WMS_UDH_SMALL_PICTURE:
         pos+= wms_ts_encode_udh_small_picture(data+pos);
         break;

       case WMS_UDH_VAR_PICTURE:
         pos+= wms_ts_encode_udh_var_picture(data+pos);
         break;

#ifndef FEATURE_SMS_IGNORE_EMS_5_CONTENT
       case WMS_UDH_USER_PROMPT:
         pos+= wms_ts_encode_udh_user_prompt(data+pos);
         break;

       case WMS_UDH_EXTENDED_OBJECT:
         pos+= wms_ts_encode_udh_eo(data+pos);
         break;
#endif /* ! FEATURE_SMS_IGNORE_EMS_5_CONTENT */

       case WMS_UDH_RFC822:
         pos+= wms_ts_encode_udh_rfc822(data+pos);
         break;
#endif /* !defined FEATURE_SMS_EXTENDED */

       default:
#ifdef FEATURE_SMS_EXTENDED
         /* Ignore any other headers */
         MSG_ERROR( "Unsupported header %d not encoded",
                    const_header->header_id, 0, 0);
#else /* FEATURE_SMS_EXTENDED */
         pos+= wms_ts_encode_udh_other(data+pos,const_header->header_id);
#endif /* FEATURE_SMS_EXTENDED */

     }

   }
   data[0] = (uint8)(pos-1); /*User Data Header Length*/
   return ((uint8)(pos-1));
#else
   return 0;
#endif /* FEATURE_SMS_UDH */

} /*wms_ts_encode_user_data_header*/

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /*FEATURE_GWSMS_BROADCAST */

/*=========================================================================
FUNCTION
  wms_ts_encode

DESCRIPTION
  Encode Transport Service structure into raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  status of encoding.

SIDE EFFECTS
  None
=========================================================================*/
wms_status_e_type wms_ts_encode
(
  const wms_client_ts_data_s_type         * client_ts_data_ptr,
  wms_raw_ts_data_s_type                  * raw_ts_data_ptr
)
{
  wms_status_e_type   st = WMS_OK_S;
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

  if( client_ts_data_ptr == NULL || raw_ts_data_ptr == NULL )
  {
    MSG_ERROR("Null pointer!",0,0,0);
    return WMS_NULL_PTR_S;
  }

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

  wms_ts_init();
  rex_enter_crit_sect(&encode_decode_data_crit_sect);

  switch( client_ts_data_ptr->format )
  {

#ifdef FEATURE_CDSMS

    case WMS_FORMAT_CDMA:
    case WMS_FORMAT_ANALOG_AWISMS:
    case WMS_FORMAT_ANALOG_CLI:
    case WMS_FORMAT_ANALOG_VOICE_MAIL:
    case WMS_FORMAT_ANALOG_SMS:
    case WMS_FORMAT_MWI:
      st = wms_ts_encode_bearer_data( & client_ts_data_ptr->u.cdma,
                                      raw_ts_data_ptr );
      break;

#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

      default:
        st = WMS_INVALID_FORMAT_S;
        ERR("Invalid format: %d", client_ts_data_ptr->format, 0,0);
        break;
  }

  raw_ts_data_ptr->format = client_ts_data_ptr->format;

  rex_leave_crit_sect(&encode_decode_data_crit_sect);

  return st;

} /* wms_ts_encode() */

/*=========================================================================
FUNCTION
  wms_ts_decode_dcs

DESCRIPTION
  Decode Data Coding Scheme from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes decoded.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_decode_dcs
(
  const uint8           *data,
  wms_gw_dcs_s_type     *dcs
)
{
  uint8 pos = 0;
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
  return pos;

} /* wms_ts_decode_dcs() */

/*=========================================================================
FUNCTION
  wms_ts_decode_timestamp

DESCRIPTION
  Decode a timestamp from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes decoded.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_decode_timestamp
(
  const uint8             *data,
  wms_timestamp_s_type  *timestamp
)
{
  uint8 pos = 0, i, j;

  if (data == NULL || timestamp == NULL)
  {
    MSG_ERROR("null pointer in wms_ts_decode_timestamp",0,0,0);
    return 0;
  }

  /*year check*/

  /* in GW, swap the order of LSB, MSB */
  i = ((data[pos] & 0x0F) << 4) + ((data[pos] & 0xF0) >> 4);
  if ( !wms_ts_bcd_to_int(i, &j) )
  {
    MSG_ERROR("Invalid BCD Digits in Encoded Timestamp Year : %d",data[pos],0,0);
    i = 0;  /* Modifying it to a Good Value */
  }
  timestamp->year = i;
  pos ++;

  /*month check*/
  i = ((data[pos] & 0x0F) << 4) + ((data[pos] & 0xF0) >> 4);
  if ( wms_ts_bcd_to_int(i, &j) )
  {
    if (j > 12 || j < 1)
    {
      MSG_ERROR("Month is invalid: %d", j, 0, 0);
      i = 1;  /* Modifying it to a Good Value */
    }
  }
  else
  {
    MSG_ERROR("Invalid BCD Digits in Encoded Timestamp Month : %d",data[pos],0,0);
    i = 1;  /* Modifying it to a Good Value */
  }
  timestamp->month = i;
  pos ++;

  /*day check*/
  i = ((data[pos] & 0x0F) << 4) + ((data[pos] & 0xF0) >> 4);
  if ( wms_ts_bcd_to_int(i, &j) )
  {
    if (j > 31 || j < 1)
    {
      MSG_ERROR("Day is invalid: %d", j, 0, 0);
      i = 1;  /* Modifying it to a Good Value */
    }
  }
  else
  {
    MSG_ERROR("Invalid BCD Digits in Encoded Timestamp Day : %d",data[pos],0,0);
    i = 1;  /* Modifying it to a Good Value */
  }
  timestamp->day = i;
  pos ++;

  /*hour check*/
  i = ((data[pos] & 0x0F) << 4) + ((data[pos] & 0xF0) >> 4);
  if ( wms_ts_bcd_to_int(i, &j) )
  {
    if (j > 23)
    {
      MSG_ERROR("Hour is too large: %d", j, 0, 0);
      i = 0;  /* Modifying it to a Good Value */
    }
  }
  else
  {
    MSG_ERROR("Invalid BCD Digits in Encoded Timestamp Hour : %d",data[pos],0,0);
    i = 0;  /* Modifying it to a Good Value */
  }
  timestamp->hour = i;
  pos ++;

  /*minute check*/
  i = ((data[pos] & 0x0F) << 4) + ((data[pos] & 0xF0) >> 4);
  if ( wms_ts_bcd_to_int(i, &j) )
  {
    if (j > 59)
    {
      MSG_ERROR("Minute is too large: %d", j, 0, 0);
      i = 0;  /* Modifying it to a Good Value */
    }
  }
  else
  {
    MSG_ERROR("Invalid BCD Digits in Encoded Timestamp Minute : %d",data[pos],0,0);
    i = 0;  /* Modifying it to a Good Value */
  }
  timestamp->minute = i;
  pos ++;

  /*seconds check*/
  i = ((data[pos] & 0x0F) << 4) + ((data[pos] & 0xF0) >> 4);
  if ( wms_ts_bcd_to_int(i, &j) )
  {
    if (j > 59)
    {
      MSG_ERROR("Second is too large: %d", i, 0, 0);
      i = 0;  /* Modifying it to a Good Value */
    }
  }
  else
  {
    MSG_ERROR("Invalid BCD Digits in Encoded Timestamp Second : %d",data[pos],0,0);
    i = 0;  /* Modifying it to a Good Value */
  }
  timestamp->second = i;
  pos ++;

  /*timezone, special case where timestamp->timezone is an integer value*/
  if (data[pos] & 0x08)
  {
    timestamp->timezone = (data[pos] & 0x07) * 10 + ((data[pos] & 0xF0)>>4);
    timestamp->timezone *= (-1);
  }
  else
  {
    timestamp->timezone = (sint7)((data[pos] & 0x0F) * 10 + ((data[pos] & 0xF0) >> 4 ));
  }

  if (timestamp->timezone > 48 || timestamp->timezone < -48)
  {
    MSG_ERROR("Timezone is out of bound: %d", timestamp->timezone, 0, 0);
    timestamp->timezone = 0;  /* Modifying it to a Good Value */
  }
  pos ++;

  return pos;

} /* wms_ts_decode_timestamp() */


#ifdef FEATURE_SMS_UDH
static uint8 wms_ts_decode_udh_concat_8
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr /* OUT */
)
{
  uint8 pos=0;

  if (udh == NULL || header_ptr == NULL )
  {
    MSG_ERROR("udh is NULL",0,0,0);
    return 0;
  }

  if (udh[pos] < 3)  /*Length of information element*/
  {
    MSG_ERROR("UDH Header Concat 8 Present with invalid data length = %d",
               udh[pos],0,0);
    return 0; /*Return 0*/
  }


  /* if the maximum number of messages is 0     Or*/
  /* if the sequence number of the message is 0 Or*/
  /* if the sequence number of the current message is greater than the max messages*/
  if (udh[pos +2] == 0 ||
      udh[pos +3] == 0 ||
      udh[pos +3] > udh[pos +2])
  {
    MSG_ERROR("UDH Header Contact 8 with out of bound max messages", 0, 0, 0);
    return 0;
  }

  pos++;
  header_ptr->header_id          = WMS_UDH_CONCAT_8;
  header_ptr->u.concat_8.msg_ref = udh[pos++];
  header_ptr->u.concat_8.total_sm= udh[pos++];
  header_ptr->u.concat_8.seq_num = udh[pos++];

  return (udh[0] + 1);

}/*wms_ts_decode_udh_concat_8*/

static uint8 wms_ts_decode_udh_concat16
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0;

  if (udh == NULL || header_ptr == NULL )
  {
    MSG_ERROR("udh is NULL",0,0,0);
    return 0;
  }

  if(udh[pos] < 4) /*Length of information element*/
  {
    MSG_ERROR("SMS UDH Header Concat16 Present with invalid data length = %d",
               udh[pos], 0, 0 );
    return 0;
  }

  /* if the maximum number of messages is 0     Or*/
  /* if the sequence number of the message is 0 Or*/
  /* if the sequence number of the current message is greater than the max messages*/
  if (udh[pos +3] == 0 ||
      udh[pos +4] == 0 ||
      udh[pos +4] > udh[pos +3])
    return 0;

  header_ptr->header_id           = WMS_UDH_CONCAT_16;
  pos++;
  header_ptr->u.concat_16.msg_ref = udh[pos++];
  header_ptr->u.concat_16.msg_ref = (uint16)(header_ptr->u.concat_16.msg_ref << 8) | udh[pos++];
  header_ptr->u.concat_16.total_sm= udh[pos++];
  header_ptr->u.concat_16.seq_num = udh[pos++];

  return (udh[0] + 1);
}/*wms_ts_decode_udh_concat16*/


#if !defined(FEATURE_SMS_EXTENDED)
/*=========================================================================
FUNCTION
  wms_ts_udh_decode_first_seg_check

DESCRIPTION

  Local helper function used during decoding to check if message segment
  is first segment or not. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE depending on if function is successful or not.

SIDE EFFECTS
  Modifies is_first_segment_ptr with result
=========================================================================*/
static boolean wms_ts_udh_decode_first_seg_check
(
  const uint8  len,                /* user_data_length*/
  const uint8  *data,              /* first byte of user data */
  boolean      *is_first_segment_ptr      /* OUT */
)
{
  uint8 pos = 0;
  uint8 num_headers = 0;
  uint8 udhl = 0;
  uint8 iedl = 0;
  uint8 iei = 0;

  // Default to true - it might not have concat header present
  *is_first_segment_ptr = TRUE;
      
  if (data == NULL || data[pos] == 0 || len == 0 )
  {
    MSG_ERROR("null in wms_ts_udh_decode_first_seg_check",0,0,0);
    return FALSE;
  }

  // First byte is the UDH Length
  udhl = data[pos];

  // Move pos to first user data header
  pos++;

  while ((pos < udhl) && (num_headers < WMS_MAX_UD_HEADERS))
  {
      // First byte is UDH, next byte is length
      iei  = data[pos];
      iedl = data[pos+1];

      if ( iei == WMS_UDH_CONCAT_16 )
      {
          // For concat16 element, peek and see seq#
          // -------------------------------------------
          // || IEI | IEDL | Ref# | Ref# |Max# | Seq# ||
          // -------------------------------------------
          if ( data[pos+5] != 1 )
          {
              MSG_HIGH("WMS_UDH_CONCAT_16 not first segment!",0,0,0);
              *is_first_segment_ptr = FALSE;
              return TRUE;
          }
          else
          {
              return TRUE;
          }
      }
      // Not a concat header, so we dont care, skip over it's length
      else 
      {
          num_headers++;
          pos += (2 + iedl); // IEI + IEDL + Actual Data Length
      }
  } // while

  return TRUE;
}


static uint8 wms_ts_decode_udh_special_sm
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0;

  if (udh == NULL || header_ptr == NULL )
  {
    MSG_ERROR("udh is NULL",0,0,0);
    return 0;
  }

  if(udh[pos] < 2) /*Length of information element*/
  {
    MSG_ERROR("SMS UDH Header Special SM Present with invalid data length = %d",
               udh[pos], 0, 0 );
    return 0;
  }

  pos++;
  header_ptr->header_id                             = WMS_UDH_SPECIAL_SM;

  /* Bit 7 of octet 1 indicates if the message shall be stored or not.
     Bit 6..0 of octet 1 indicate which indicator is to be updated.
          000 0000 = Voice Message Waiting
          000 0001 = Fax Message Waiting
          000 0010 = Email Message Waiting
          000 0011 = Other Message Waiting
    Octet 2 represents the number of waiting messages
          The number can range from 0 to 255 with the value of 255 meaning
          that there are 255 or more messages waiting
  */

  header_ptr->u.special_sm.msg_waiting =  (wms_gw_msg_waiting_e_type) ((udh[pos] >> 7 == 0) ? WMS_GW_MSG_WAITING_DISCARD : WMS_GW_MSG_WAITING_STORE);
  header_ptr->u.special_sm.msg_waiting_kind         = (wms_gw_msg_waiting_kind_e_type) (udh[pos++] & 0x7f);
  header_ptr->u.special_sm.message_count            = udh[pos++];

  return (udh[0] + 1);
}/*wms_ts_decode_udh_special_sm*/

static uint8 wms_ts_decode_udh_port_8
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0;

  if (udh == NULL || header_ptr == NULL )
  {
    MSG_ERROR("udh is NULL",0,0,0);
    return 0;
  }


  if (udh[pos] < 2)  /*Length of information element*/
  {
    MSG_ERROR("UDH Header Port 8 Present with invalid data length = %d",
               udh[pos],0,0 );
    return 0; /*Return 0*/
  }


  pos++;
  header_ptr->header_id          = WMS_UDH_PORT_8;
  header_ptr->u.wap_8.dest_port  = udh[pos++];
  header_ptr->u.wap_8.orig_port  = udh[pos++];

  return (udh[0] + 1);
}/*wms_ts_decode_udh_port_8*/

static uint8 wms_ts_decode_udh_port16
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0;

  if (udh == NULL || header_ptr == NULL )
  {
    MSG_ERROR("udh is NULL",0,0,0);
    return 0;
  }

  if(udh[pos] < 4) /*Length of information element*/
  {
    MSG_ERROR("SMS UDH Header Port16 Present with invalid data length = %d",
               udh[pos], 0, 0 );
    return 0;
  }

  header_ptr->header_id           = WMS_UDH_PORT_16;
  pos++;
  header_ptr->u.wap_16.dest_port = udh[pos++];
  header_ptr->u.wap_16.dest_port = (uint16)(header_ptr->u.wap_16.dest_port << 8) | udh[pos++];
  header_ptr->u.wap_16.orig_port = udh[pos++];
  header_ptr->u.wap_16.orig_port = (uint16)(header_ptr->u.wap_16.orig_port << 8) | udh[pos++];

  return (udh[0] + 1);
}/*wms_ts_decode_udh_port16*/


static uint8 wms_ts_decode_udh_text_formatting
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0;

  if (udh == NULL || header_ptr == NULL )
  {
    MSG_ERROR("udh is NULL",0,0,0);
    return 0;
  }

  if(udh[pos] < 3 ) /*Length of information element*/
  {
    MSG_ERROR("SMS UDH Header Text Formatting Present with invalid data length = %d", udh[pos], 0, 0 );
    return 0;
  }

  if(udh[pos] >= 4)
  {
    header_ptr->u.text_formating.is_color_present = TRUE;
  }
  else
  {
    header_ptr->u.text_formating.is_color_present = FALSE;
  }
  pos++;


  header_ptr->header_id                                 = WMS_UDH_TEXT_FORMATING;
  header_ptr->u.text_formating.start_position           = udh[pos++];
  header_ptr->u.text_formating.text_formatting_length   = udh[pos++];
  header_ptr->u.text_formating.alignment_type           = (wms_udh_alignment_e_type) (udh[pos] & 0x03 ); /*bit 0 and  bit 1*/
  header_ptr->u.text_formating.font_size                = (wms_udh_font_size_e_type) ((udh[pos] & 0x0c) >> 2); /*bit 3 and  bit 2*/

  header_ptr->u.text_formating.style_bold               = (udh[pos] & 0x10) >> 4; /*bit 4 */
  header_ptr->u.text_formating.style_italic             = (udh[pos] & 0x20) >> 5; /*bit 5 */
  header_ptr->u.text_formating.style_underlined         = (udh[pos] & 0x40) >> 6; /*bit 6 */
  header_ptr->u.text_formating.style_strikethrough      = (udh[pos] & 0x80) >> 7; /*bit 7 */
  pos++;

  if(header_ptr->u.text_formating.is_color_present)
  {
    header_ptr->u.text_formating.text_color_foreground   = (wms_udh_text_color_e_type) (udh[pos] & 0x0F);  /* bit 0-3 */
    header_ptr->u.text_formating.text_color_background   = (wms_udh_text_color_e_type) ((udh[pos] & 0xF0) >> 4);  /* bit 4-7 */
    pos++;
  }
  return (udh[0] + 1);
}/*wms_ts_decode_udh_text_formatting*/

static uint8 wms_ts_decode_udh_pre_def_sound
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0;

  if (udh == NULL || header_ptr == NULL )
  {
    MSG_ERROR("udh is NULL",0,0,0);
    return 0;
  }

  if(udh[pos] < 2) /*Length of information element*/
  {
    MSG_ERROR("SMS UDH Header Pre Defined Sound Present with invalid data length = %d",
               udh[pos], 0, 0 );
    return 0;
  }

  pos++;
  header_ptr->header_id                                 = WMS_UDH_PRE_DEF_SOUND;
  header_ptr->u.pre_def_sound.position                  = udh[pos++];
  header_ptr->u.pre_def_sound.snd_number                = udh[pos++];

  return (udh[0] + 1);
}/*wms_ts_decode_udh_pre_def_sound*/

static uint8 wms_ts_decode_udh_user_def_sound
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0,j;

  if (udh == NULL || header_ptr == NULL )
  {
    MSG_ERROR("udh is NULL",0,0,0);
    return 0;
  }

  if(udh[pos] == 0) /*Length of information element*/
  {
    MSG_ERROR("SMS UDH Header User Defined Sound Present with no Data",
               0, 0, 0 );
    return 0;
  }

  header_ptr->header_id                                 = WMS_UDH_USER_DEF_SOUND;
  header_ptr->u.user_def_sound.data_length              = udh[pos++]-1;
  header_ptr->u.user_def_sound.position                 = udh[pos++];


  if (header_ptr->u.user_def_sound.data_length > WMS_UDH_MAX_SND_SIZE)
  {
    MSG_ERROR("Max Size Exceed Header id %d ",
                     header_ptr->header_id, 0,
                     0 );
    //data[pos] += data[pos]; /*Skip the bytes*/
    return 0;
  }

  //pos++;
  memset (header_ptr->u.user_def_sound.user_def_sound, 0xff,WMS_UDH_MAX_SND_SIZE);

  for(j=0;j<header_ptr->u.user_def_sound.data_length;j++)
    header_ptr->u.user_def_sound.user_def_sound[j]      = udh[pos++];

  return pos;
}/*wms_ts_decode_udh_user_def_sound*/


static uint8 wms_ts_decode_udh_pre_def_anim
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0;

  if (udh == NULL || header_ptr == NULL )
  {
    MSG_ERROR("udh is NULL",0,0,0);
    return 0;
  }

  if(udh[pos] != 2) /*Length of information element*/
  {
    MSG_ERROR("SMS UDH Header Pre Defined Animation Present with invalid data length = %d",
               udh[pos], 0, 0 );
    return 0;
  }

  pos++;
  header_ptr->header_id                                 = WMS_UDH_PRE_DEF_ANIM;
  header_ptr->u.pre_def_anim.position                   = udh[pos++];
  header_ptr->u.pre_def_anim.animation_number           = udh[pos++];

  return pos;
}/*wms_ts_decode_udh_pre_def_anim*/

static uint8 wms_ts_decode_udh_large_anim
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0,j,k;

  if (udh == NULL || header_ptr == NULL )
  {
    MSG_ERROR("udh is NULL",0,0,0);
    return 0;
  }

  if(udh[pos] != (WMS_UDH_ANIM_NUM_BITMAPS * WMS_UDH_LARGE_BITMAP_SIZE + 1) ) /*Length of information element*/
  {
    MSG_ERROR("SMS UDH Header Large Defined Animation Present with invalid data length = %d",
               udh[pos], 0, 0 );
    return 0;
  }

  header_ptr->header_id                             = WMS_UDH_LARGE_ANIM;
  pos++; /*Skip the Size*/
  header_ptr->u.large_anim.position                 = udh[pos++];

  for(j=0;j<WMS_UDH_ANIM_NUM_BITMAPS;j++)
    for (k=0;k<WMS_UDH_LARGE_BITMAP_SIZE; k++)
      header_ptr->u.large_anim.data[j][k] = udh[pos++];

  return pos;
}/*wms_ts_decode_udh_large_anim*/


static uint8 wms_ts_decode_udh_small_anim
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0,j,k;

  if (udh == NULL || header_ptr == NULL )
  {
    MSG_ERROR("udh is NULL",0,0,0);
    return 0;
  }

  if(udh[pos] != (WMS_UDH_ANIM_NUM_BITMAPS * WMS_UDH_SMALL_BITMAP_SIZE + 1) ) /*Length of information element*/
  {
    MSG_ERROR("SMS UDH Header Large Defined Animation Present with invalid data length = ",
               udh[pos], 0, 0 );
    return 0;
  }

  header_ptr->header_id                             = WMS_UDH_SMALL_ANIM;

  pos++; /*Skip the Size*/
  header_ptr->u.small_anim.position                 = udh[pos++];

  for(j=0;j<WMS_UDH_ANIM_NUM_BITMAPS;j++)
    for (k=0;k<WMS_UDH_SMALL_BITMAP_SIZE; k++)
      header_ptr->u.small_anim.data[j][k] = udh[pos++];

  return pos;
}/*wms_ts_decode_udh_small_anim*/

static uint8 wms_ts_decode_udh_large_picture
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0,j;

  if (udh == NULL || header_ptr == NULL )
  {
    MSG_ERROR("udh is NULL",0,0,0);
    return 0;
  }

  if(udh[pos] != WMS_UDH_LARGE_PIC_SIZE + 1) /*Length of information element*/
  {
    MSG_ERROR("SMS UDH Header Large Picture Present with invalid data length = %d",
               udh[pos], 0, 0 );
    return 0;
  }


  header_ptr->header_id                                 = WMS_UDH_LARGE_PICTURE;
  pos++; /*Skip the Size*/
  header_ptr->u.large_picture.position                  = udh[pos++];

  for(j=0;j<WMS_UDH_LARGE_PIC_SIZE;j++)
    header_ptr->u.large_picture.data[j]    = udh[pos++];

  return pos;
}/*wms_ts_decode_udh_large_picture*/


static uint8 wms_ts_decode_udh_small_picture
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0,j;

  if (udh == NULL || header_ptr == NULL )
  {
    MSG_ERROR("udh is NULL",0,0,0);
    return 0;
  }

  if(udh[pos] != WMS_UDH_SMALL_PIC_SIZE + 1) /*Length of information element*/
  {
    MSG_ERROR("SMS UDH Header Small Picture Present with invalid data legnth = %d",
               udh[pos], 0, 0 );
    return 0;
  }

  header_ptr->header_id                                 = WMS_UDH_SMALL_PICTURE;
  pos++; /*Skip the size*/
  header_ptr->u.small_picture.position                  = udh[pos++];

  for(j=0;j<WMS_UDH_SMALL_PIC_SIZE;j++)
    header_ptr->u.small_picture.data[j]           = udh[pos++];

  return pos;
}/*wms_ts_decode_udh_small_picture*/


static uint8 wms_ts_decode_udh_var_picture
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0,j,pic_size;

  if (udh == NULL || header_ptr == NULL )
  {
    MSG_ERROR("udh is NULL",0,0,0);
    return 0;
  }

  if(udh[pos] > WMS_UDH_VAR_PIC_SIZE + 3) /*Length of information element*/
  {
    MSG_ERROR("SMS UDH Header Var Picture Present with invalid data length = %d",
               udh[pos], 0, 0 );
    return 0;
  }

  if ( (udh[pos] - 3) != (udh[pos+2] * udh[pos+3]) )
  {
    MSG_ERROR("SMS UDH Header Var Picture, pic size value mismatch with heigt and weight",0,0,0);
    return 0;
  }

  pic_size                                          = udh[pos++] -3;
  header_ptr->header_id                                 = WMS_UDH_VAR_PICTURE;
  header_ptr->u.var_picture.position                    = udh[pos++];
  header_ptr->u.var_picture.width                       = (uint8) (udh[pos++] * 8);
  header_ptr->u.var_picture.height                      = udh[pos++];

  for(j=0;j<pic_size && j<WMS_UDH_VAR_PIC_SIZE; j++)
    header_ptr->u.var_picture.data[j]             = udh[pos++];


  return pos;
}/*wms_ts_decode_udh_var_picture*/


static uint8 wms_ts_decode_udh_user_prompt
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0;

  if (udh == NULL || header_ptr == NULL )
  {
    MSG_ERROR("udh is NULL",0,0,0);
    return 0;
  }

  if(udh[pos] < 1) /*Length of information element*/
  {
    MSG_ERROR("SMS UDH User Prompt present with invalid data length = %d",
               udh[pos], 0, 0 );
    return 0;
  }

  pos ++; /* Skip udh length */

  header_ptr->header_id                                 = WMS_UDH_USER_PROMPT;
  header_ptr->u.user_prompt.number_of_objects           = udh[pos++];

  return (udh[0] + 1);

} /* wms_ts_decode_udh_user_prompt() */


/* Decoding UDH Extended Object
*/
static uint8 wms_ts_decode_udh_eo
(
  const uint8 *udh,
  boolean     first_segment,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0, udh_length;

  if (udh == NULL || header_ptr == NULL )
  {
    MSG_ERROR("udh is NULL",0,0,0);
    return 0;
  }

  if(udh[pos] == 0) /*Length of information element*/
  {
    MSG_ERROR("SMS UDH Extended Object present with no Data",
               0, 0, 0 );
    return 0;
  }

  /* Get the length of this UDH */
  udh_length = udh[pos++];

  header_ptr->header_id                  = WMS_UDH_EXTENDED_OBJECT;
  header_ptr->u.eo.first_segment         = first_segment;

  if( first_segment == TRUE )
  {
    /* The following fields in the first segment occupy 7 bytes */
    if( udh_length < WMS_UDH_OCTETS_EO_HEADER )
    {
      return 0;
    }

    header_ptr->u.eo.reference           = udh[pos++];
    header_ptr->u.eo.length              = udh[pos++] << 8;
    header_ptr->u.eo.length              |= udh[pos++];
    header_ptr->u.eo.control             = udh[pos++];
    header_ptr->u.eo.type                = (wms_udh_eo_id_e_type) udh[pos++];
    header_ptr->u.eo.position            = udh[pos++] << 8;
    header_ptr->u.eo.position            |= udh[pos++];
  }

  /* Decode EO content */
  header_ptr->u.eo.content.length = (udh_length - pos) + 1;
  memcpy( header_ptr->u.eo.content.data,
          udh + pos,
          header_ptr->u.eo.content.length );

  pos += header_ptr->u.eo.content.length;

  return pos;

} /* wms_ts_decode_udh_eo() */

static uint8 wms_ts_decode_udh_rfc822
(
  const uint8 *udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0;

  if (udh == NULL || header_ptr == NULL )
  {
    MSG_ERROR("udh is NULL",0,0,0);
    return 0;
  }

  if(udh[pos] <  1) /*Length of information element*/
  {
    MSG_ERROR("SMS UDH Header Rfc822 Present with invalid data length = %d",
               udh[pos], 0, 0 );
    return 0;
  }

  pos++;
  header_ptr->header_id                                 = WMS_UDH_RFC822;
  header_ptr->u.rfc822.header_length                    = udh[pos++];

  return (udh[0] + 1);
}/*wms_ts_decode_udh_rfc822*/

static uint8 wms_ts_decode_udh_other
(
  const uint8* udh,
  wms_udh_s_type *header_ptr
)
{
  uint8 pos=0, i=0;

  if (udh == NULL || header_ptr == NULL )
  {
    MSG_ERROR("udh is NULL",0,0,0);
    return 0;
  }

  if (udh[pos+1] > WMS_UDH_OTHER_SIZE)
  {
    MSG_ERROR("SMS UDH Header Other data length exceeding 226",0,0,0);
    return 0;
  }

  header_ptr->header_id                                 = (wms_udh_id_e_type) udh[pos];
  header_ptr->u.other.header_id                         = (wms_udh_id_e_type) udh[pos++];
  header_ptr->u.other.header_length                     = udh[pos++];

  for(i=0;i<header_ptr->u.other.header_length;i++)
  {
    header_ptr->u.other.data[i] = udh[pos++];
  }

  return pos;
}
#endif /* !defined FEATURE_SMS_EXTENDED */

#endif /* FEATURE_SMS_UDH */

/*=========================================================================
FUNCTION
  wms_ts_decode_user_data_header

DESCRIPTION
  Decode User Data from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes decoded.

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_decode_user_data_header
(
  const uint8               len, /* user_data_length*/
  const uint8               *data, /* first byte of user data */
  uint8                     * num_headers_ptr, /* OUT */
  wms_udh_s_type            * udh_ptr          /* OUT */
)
{
#ifdef FEATURE_SMS_UDH
  uint8 pos =0;
  uint8 header_length =0, num_headers=0;
  uint8 udhl;

#if !defined(FEATURE_SMS_EXTENDED)
  boolean    first_segment = TRUE; /* Used for Extended Object decoding */
#endif /* FEATURE_SMS_EXTENDED */

  if (data == NULL || len == 0 || data[pos] == 0 || num_headers_ptr == NULL || udh_ptr == NULL )
  {
    MSG_ERROR("null pointer in wms_ts_decode_user_data_header",0,0,0);
   return 0;
  }

  udhl = data[pos];

  pos++;

   while ((pos < udhl)&&(num_headers<WMS_MAX_UD_HEADERS))
   {
#ifdef FEATURE_SMS_EXTENDED
     boolean   decoded_udh_concatenation;
     decoded_udh_concatenation = FALSE;
#endif /* FEATURE_SMS_EXTENDED */

     /*first byte - header id*/
     /*second byte - header length*/
     switch(data[pos++])
     {
       case WMS_UDH_CONCAT_8:
         header_length = wms_ts_decode_udh_concat_8(data+pos, &udh_ptr[num_headers]);
#ifdef FEATURE_SMS_EXTENDED
         decoded_udh_concatenation = TRUE;
#endif /* FEATURE_SMS_EXTENDED */
         break;

       case WMS_UDH_CONCAT_16:
         header_length = wms_ts_decode_udh_concat16(data+pos, &udh_ptr[num_headers]);
#ifdef FEATURE_SMS_EXTENDED
         decoded_udh_concatenation = TRUE;
#endif /* FEATURE_SMS_EXTENDED */
         break;

#if !defined(FEATURE_SMS_EXTENDED)
       case WMS_UDH_SPECIAL_SM:
         header_length = wms_ts_decode_udh_special_sm(data+pos, &udh_ptr[num_headers]);
         break;

       case WMS_UDH_PORT_8:
         header_length = wms_ts_decode_udh_port_8(data+pos, &udh_ptr[num_headers]);
         break;

       case WMS_UDH_PORT_16:
         header_length = wms_ts_decode_udh_port16(data+pos, &udh_ptr[num_headers]);
         break;

       case WMS_UDH_TEXT_FORMATING:
         header_length = wms_ts_decode_udh_text_formatting(data+pos, &udh_ptr[num_headers]);
         break;

       case WMS_UDH_PRE_DEF_SOUND:
         header_length = wms_ts_decode_udh_pre_def_sound(data+pos, &udh_ptr[num_headers]);
         break;

       case WMS_UDH_USER_DEF_SOUND:
         header_length = wms_ts_decode_udh_user_def_sound(data+pos, &udh_ptr[num_headers]);
         break;

       case WMS_UDH_PRE_DEF_ANIM:
         header_length = wms_ts_decode_udh_pre_def_anim(data+pos, &udh_ptr[num_headers]);
         break;

       case WMS_UDH_LARGE_ANIM:
         header_length = wms_ts_decode_udh_large_anim(data+pos, &udh_ptr[num_headers]);
         break;

       case WMS_UDH_SMALL_ANIM:
         header_length = wms_ts_decode_udh_small_anim(data+pos, &udh_ptr[num_headers]);
         break;

       case WMS_UDH_LARGE_PICTURE:
         header_length = wms_ts_decode_udh_large_picture(data+pos, &udh_ptr[num_headers]);
         break;

       case WMS_UDH_SMALL_PICTURE:
         header_length = wms_ts_decode_udh_small_picture(data+pos, &udh_ptr[num_headers]);
           break;

       case WMS_UDH_VAR_PICTURE:
         header_length = wms_ts_decode_udh_var_picture(data+pos, &udh_ptr[num_headers]);
         break;

#ifndef FEATURE_SMS_IGNORE_EMS_5_CONTENT
       case WMS_UDH_USER_PROMPT:
         header_length = wms_ts_decode_udh_user_prompt( data+pos, &udh_ptr[num_headers]);
         break;

       case WMS_UDH_EXTENDED_OBJECT:
         if ( wms_ts_udh_decode_first_seg_check ( len, data, &first_segment ) == FALSE )
         {
           MSG_ERROR("wms_ts_udh_decode_first_seg_check failed",0,0,0);
           return 0;
         }
         header_length = wms_ts_decode_udh_eo( data+pos, first_segment, &udh_ptr[num_headers]);
         break;
#endif /* ! FEATURE_SMS_IGNORE_EMS_5_CONTENT */

       case WMS_UDH_RFC822:
         header_length = wms_ts_decode_udh_rfc822(data+pos, &udh_ptr[num_headers]);
         break;
#endif /* !defined FEATURE_SMS_EXTENDED */

       default:
#ifdef FEATURE_SMS_EXTENDED
         /* Ignore unsupported headers */
         /* data[pos] here is the Length field; data[pos-1] is the Id field */
         MSG_ERROR("Ignoring unsupported header %d", data[pos-1], 0, 0);
         header_length = 1 + data[pos]; /* number of bytes for Length+Value */
#else /* FEATURE_SMS_EXTENDED */
         pos --;
         header_length = wms_ts_decode_udh_other(data+pos, &udh_ptr[num_headers]);
#endif /* FEATURE_SMS_EXTENDED */
         break;
     } /* End of Switch */

     if ( (uint16)pos + (uint16)header_length  > WMS_MAX_LEN )
     {
       MSG_ERROR("number of bytes decoded has exceeded UDHL value of %d",udhl,0,0);
       return 0;
     }
     else if(header_length != 0)
     {
       pos += header_length;
#ifdef FEATURE_SMS_EXTENDED
       if( TRUE == decoded_udh_concatenation )
       {
         /* Only increment if we have just decoded a concatenation header */
         num_headers++;
       }
#else /* FEATURE_SMS_EXTENDED */
       num_headers++;
#endif /* FEATURE_SMS_EXTENDED */
     }
     else
     {
       MSG_ERROR("Bad UDH: pos=%d, data[pos]=%d", pos, data[pos], 0 );
       * num_headers_ptr = 0;
       return 0;  /* SHORT-RETURN */
     }
   } /* End of While loop */


   if (num_headers >= WMS_MAX_UD_HEADERS)
   {
     /* Num Headers has Exceeded Max */
     MSG_ERROR("decode_udh: Num Headers has exceeded WMS_MAX_UD_HEADERS", 0, 0, 0);

     /* Placing the correct value */
     pos = udhl+1;
   }

   if (pos!= (udhl+1))
   {
     MSG_ERROR("SMS UDH could not be decoded",
               0, 0, 0 );
     num_headers =0;
     udhl =0;
   }

   if (num_headers >0)
   {
     * num_headers_ptr = num_headers;
   }

   return udhl;
#else
   return 0;
#endif /* FEATURE_SMS_UDH */
}
/*wms_ts_decode_user_data_header*/

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */


#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */



/*=========================================================================
FUNCTION
  wms_ts_decode

DESCRIPTION
  Decode Transport Service structure from raw bytes.

DEPENDENCIES
  None

RETURN VALUE
  status of decoding.

SIDE EFFECTS
  None
=========================================================================*/
wms_status_e_type wms_ts_decode
(
  const wms_raw_ts_data_s_type            * raw_ts_data_ptr,
  wms_client_ts_data_s_type               * client_ts_data_ptr
)
{
  // TBD: GW format only
  wms_status_e_type   st = WMS_OK_S;
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

  if( raw_ts_data_ptr == NULL || client_ts_data_ptr == NULL )  
  {
    MSG_ERROR("Null pointer!",0,0,0);
    return WMS_NULL_PTR_S;
  }

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */


  wms_ts_init();
  rex_enter_crit_sect(&encode_decode_data_crit_sect);

  switch( raw_ts_data_ptr->format )
  {

#ifdef FEATURE_CDSMS

    case WMS_FORMAT_CDMA:
    case WMS_FORMAT_ANALOG_AWISMS:
    case WMS_FORMAT_ANALOG_CLI:
    case WMS_FORMAT_ANALOG_VOICE_MAIL:
    case WMS_FORMAT_ANALOG_SMS:
    case WMS_FORMAT_MWI:
      st = wms_ts_decode_bearer_data( raw_ts_data_ptr,
                                      & client_ts_data_ptr->u.cdma );
      break;

#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

      default:
        st = WMS_INVALID_FORMAT_S;
        ERR("Invalid format: %d", raw_ts_data_ptr->format, 0,0);
        break;
  }

  client_ts_data_ptr->format = raw_ts_data_ptr->format;

  rex_leave_crit_sect(&encode_decode_data_crit_sect);

  return st;

} /* wms_ts_decode() */



#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */



void wms_ts_decode_relative_time
(
  uint8                     v,
  wms_timestamp_s_type    * timestamp
)
{
  uint32                  i;
  MSG_FATAL("v=========wms_ts_decode_relative_time============%d",v,0,0);
  if (timestamp != NULL)
  {
    wms_ts_init();
    rex_enter_crit_sect(&encode_decode_data_crit_sect);

    memset( (void*) timestamp, 0, sizeof(wms_timestamp_s_type) );

    if( v < 144 )
    {
      i = ( v + 1 ) * 5; /* minutes */
      timestamp->hour    = (uint8)wms_ts_int_to_bcd((uint8)(i/60));
      timestamp->minute  = (uint8)wms_ts_int_to_bcd((uint8)(i % 60));
    }
    else if (v < 167)
    {
      i = ( v - 143 ) * 30; /* minutes */
      timestamp->hour    = (uint8)wms_ts_int_to_bcd((uint8)(12 + i/60));
      timestamp->minute  = (uint8)wms_ts_int_to_bcd((uint8)(i % 60));
    }
    else if (v < 197)
    {
      i = v - 166; /* days */
      timestamp->month    = (uint8)wms_ts_int_to_bcd((uint8)(i/30));
      timestamp->day      = (uint8)wms_ts_int_to_bcd((uint8)(i % 30));
    }
    #ifdef FEATURE_FLASH_SMS
    else if(v == 246)
    {
        MSG_FATAL("timestamp->minute======%d",timestamp->minute,0,0);
    	timestamp->minute = 1;
    }
    #endif
    else
    {
      i = ( v - 192 ) * 7; /* days */
      timestamp->year     = (uint8)wms_ts_int_to_bcd((uint8)(i/365));
      timestamp->month    = (uint8)wms_ts_int_to_bcd((uint8)((i%365)/30));
      timestamp->day      = (uint8)wms_ts_int_to_bcd((uint8)(( i % 365 ) % 30));
    }

    rex_leave_crit_sect(&encode_decode_data_crit_sect);
  }
  else
  {
    MSG_ERROR("null pointer in wms_ts_decode_relative_time",0,0,0);
  }
  /* done */
  return;

} /* wms_ts_decode_relative_time() */



uint8 wms_ts_encode_relative_time
(
  const wms_timestamp_s_type  *timestamp
)
{
  uint32    i;
  uint8     v = 0, j;
  /* round up to the next time unit boundary*/
  MSG_FATAL("v=========wms_ts_encode_relative_time============%d",v,0,0);
  MSG_FATAL("timestamp=====minute=%d,hour=%d,day=%d",timestamp->minute,timestamp->hour,timestamp->day);
  if (timestamp != NULL)
  {
    wms_ts_init();
    rex_enter_crit_sect(&encode_decode_data_crit_sect);

    if ( !wms_ts_bcd_to_int(timestamp->year, &j) )
    {
      MSG_ERROR("Year is invalid: %d", j, 0, 0);
    }
    i = j * 365;

    if ( !wms_ts_bcd_to_int(timestamp->month, &j) )
    {
      MSG_ERROR("Month is invalid: %d", j, 0, 0);
    }
    i = i + j * 30;

    if ( !wms_ts_bcd_to_int(timestamp->day, &j) )
    {
      MSG_ERROR("Day is invalid: %d", j, 0, 0);
    }
    i += j;

    if( i > 30 )
    {
      /* 197 - 255: (TP-VP - 192) weeks */
      v = (uint8) ( (i+6) / 7 + 192 );
    }
    else if( i >= 1 )
    {
      /* 168 to 196: (TP-VP - 166 ) days */
      v = (uint8) ( i + 166 );
    }
    else
    {
      if ( !wms_ts_bcd_to_int(timestamp->day, &j) )
      {
        MSG_ERROR("Day is invalid: %d", j, 0, 0);
      }
      i = j * 24 * 60;

      if ( !wms_ts_bcd_to_int(timestamp->hour, &j) )
      {
        MSG_ERROR("Hour is invalid: %d", j, 0, 0);
      }
      i = i + j * 60;

      if ( !wms_ts_bcd_to_int(timestamp->minute, &j) )
      {
        MSG_ERROR("Minute is invalid: %d", j, 0, 0);
      }
      i += j;
      MSG_FATAL("i==================%d",i,0,0);
      if( i > 12 * 60 ) /* greater than 12 hours */
      {
        /* 144 - 167: 12 hours + ( (TP-VP - 143) * 30 minutes ) */
        v = (uint8) ( ( i - ((12 * 60) + 29) ) / 30 + 143 );
      }
      #ifdef FEATURE_FLASH_SMS
      else if(i ==1)
      {
      	v = 246;//246;
      }
      #endif
      else
      {
        /* 0 - 143: (TP-VP + 1) * 5 minutes */
        v = (uint8) ( ( i + 4 ) / 5 - 1 );
      }
    }
    MSG_FATAL("v=========wms_ts_decode_relative_time============%d",v,0,0);
    rex_leave_crit_sect(&encode_decode_data_crit_sect);
  }
  else
  {
    MSG_ERROR("null pointer in wms_ts_encode_relative_time",0,0,0);
  }
  /* done */
  return v;

} /* wms_ts_encode_relative_time() */

/*=========================================================================
FUNCTION
wms_ts_pack_gw_user_data

DESCRIPTION
  Packs only the user data for a GW Message

DEPENDENCIES
  None

RETURN VALUE
  status of the conversion

SIDE EFFECTS
  Modifies raw_ts_data_ptr directly

=========================================================================*/
wms_status_e_type wms_ts_pack_gw_user_data
(
  wms_raw_ts_data_s_type            * raw_ts_data_ptr
)
{
#ifdef FEATURE_GWSMS
#error code not present
#else 
  return WMS_UNSUPPORTED_S;
#endif /* FEATURE_GWSMS */
}

#ifdef FEATURE_GWSMS
#error code not present
#endif/*FEATURE_GWSMS*/
#ifdef CUST_EDITION	



wms_status_e_type wms_ts_convert_cdma_gw_msg_to_auto
(
  const wms_client_message_s_type     *msg_ptr,       /* IN */
  uint8                               email_len,
  wms_auto_message_s_type             *auto_msg_ptr   /* OUT */
)
{
#ifdef FEATURE_SMS_TRANSPARENT_MO
#error code not present
#else
  return WMS_UNSUPPORTED_S;
#endif /* FEATURE_SMS_TRANSPARENT_MO */
}

/*=========================================================================
FUNCTION
  wms_convert_auto_to_cdma_gw_msg

DESCRIPTION
  Converts an automatic formatted message, auto_msg_ptr, to a CDMA or GW msg,
  msg_ptr, based on:
  (1) msg_mode desired, CDMA format or GW format.
  (2) the set auto params declared by client_id.
  NOTE: mem_store and index will not be assigned for the client message hdr,
        the tag will be set to WMS_TAG_NONE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Modifies msg_ptr directly.

=========================================================================*/
wms_status_e_type wms_ts_convert_auto_to_cdma_gw_msg
(
  wms_client_id_type                  client_id,
  wms_message_mode_e_type             msg_mode,
  const wms_auto_message_s_type       *auto_msg_ptr,  /* IN */
  wms_client_message_s_type           *msg_ptr        /* OUT */
)
{
#ifdef FEATURE_SMS_TRANSPARENT_MO
#error code not present
#else
  return WMS_UNSUPPORTED_S;
#endif /* FEATURE_SMS_TRANSPARENT_MO */
}
#endif /*CUST_EDITION*/
#ifdef FEATURE_SMS_UDH
/*=========================================================================
FUNCTION
  wms_ts_get_udh_length

DESCRIPTION
  Allow the client to get the header length for the given user data header.

DEPENDENCIES
  None

RETURN VALUE
  User Data Header Length

SIDE EFFECTS
  None
=========================================================================*/
uint8 wms_ts_get_udh_length
(
  const wms_udh_s_type                    *udh
)
{
  uint8 length = 0;
  if (udh != NULL)
  {
    switch(udh->header_id)
    {
      /* 0x00 */
      case WMS_UDH_CONCAT_8:
        /* Header ID Octet + Header Length Octet + Header Length */
        length = 1 + 1 + WMS_UDH_OCTETS_CONCAT8;
        break;

      /* 0x08 */
      case WMS_UDH_CONCAT_16:
        /* Header ID Octet + Header Length Octet + Header Length */
        length = 1 + 1 + WMS_UDH_OCTETS_CONCAT16;
        break;

#if !defined(FEATURE_SMS_EXTENDED)
      /* 0x01 */
      case WMS_UDH_SPECIAL_SM:
        /* Header ID Octet + Header Length Octet + Header Length */
        length = 1 + 1 + WMS_UDH_OCTETS_SPECIAL_SM;
        break;

      /* 0x02 - 0x03 Reserved */

      /* 0x04 */
      case WMS_UDH_PORT_8:
        /* Header ID Octet + Header Length Octet + Header Length */
        length = 1 + 1 + WMS_UDH_OCTETS_PORT8;
        break;

      /* 0x05 */
      case WMS_UDH_PORT_16:
        /* Header ID Octet + Header Length Octet + Header Length */
        length = 1 + 1 + WMS_UDH_OCTETS_PORT16;
        break;

      /* 0x06 */
      case WMS_UDH_SMSC_CONTROL:
        /* Header ID Octet + Header Length Octet + Header Length */
        length = 1 + 1 + udh->u.other.header_length;
        break;

      /* 0x07 */
      case WMS_UDH_SOURCE:
        /* Header ID Octet + Header Length Octet + Header Length */
        length = 1 + 1 + udh->u.other.header_length;
        break;

      /* 0x09 */
      case WMS_UDH_WCMP:
        /* Header ID Octet + Header Length Octet + Header Length */
        length = 1 + 1 + udh->u.other.header_length;
        break;

      /* 0x0A */
      case WMS_UDH_TEXT_FORMATING:
        /* Header ID Octet + Header Length Octet + Header Length */
        if(!udh->u.text_formating.is_color_present) {
          length = 1 + 1 + WMS_UDH_OCTETS_TEXT_FORMATTING;
        }
        else {
          /* Header ID Octet + Header Length Octet + Header Length + text color octet */
          length = 1 + 1 + WMS_UDH_OCTETS_TEXT_FORMATTING + 1;
        }
        break;

      /* 0x0B */
      case WMS_UDH_PRE_DEF_SOUND:
        /* Header ID Octet + Header Length Octet + Header Length */
        length = 1 + 1 + WMS_UDH_OCTETS_PRE_DEF;
        break;

      /* 0x0C */
      case WMS_UDH_USER_DEF_SOUND:
        /* Header ID Octet + Header Length Octet + Header Length + 1 (for position octet)*/
        length = 1 + 1 + udh->u.user_def_sound.data_length + 1;
        break;

      /* 0x0D */
      case WMS_UDH_PRE_DEF_ANIM:
        /* Header ID Octet + Header Length Octet + Header Length */
        length = 1 + 1 + WMS_UDH_OCTETS_PRE_DEF;
        break;

      /* 0x0E */
      case WMS_UDH_LARGE_ANIM:
        /* Header ID Octet + Header Length Octet + Header Length + 1 (for position octet)*/
        length = 1 + 1 + WMS_UDH_LARGE_BITMAP_SIZE * WMS_UDH_ANIM_NUM_BITMAPS + 1;
        break;

      /* 0x0F */
      case WMS_UDH_SMALL_ANIM:
        /* Header ID Octet + Header Length Octet + Header Length + 1 (for position octet)*/
        length = 1 + 1 + WMS_UDH_SMALL_BITMAP_SIZE * WMS_UDH_ANIM_NUM_BITMAPS + 1;
        break;

      /* 0x10 */
      case WMS_UDH_LARGE_PICTURE:
        /* Header ID Octet + Header Length Octet + Header Length + 1 (for position octet)*/
        length = 1 + 1 + WMS_UDH_LARGE_PIC_SIZE + 1;
        break;

      /* 0x11 */
      case WMS_UDH_SMALL_PICTURE:
        /* Header ID Octet + Header Length Octet + Header Length + 1 (for position octet)*/
        length = 1 + 1 + WMS_UDH_SMALL_PIC_SIZE + 1;
        break;

      /* 0x12 */
      case WMS_UDH_VAR_PICTURE:
        /* Header ID Octet + Header Length Octet + Header Length + 3 (for height, width and position octets)*/
        length = 1 + 1 + (uint8)(udh->u.var_picture.height * udh->u.var_picture.width/8) + 3;
        break;

      /* 0x13 - 0x1F Reserved for future EMS*/

      /* 0x20 */
      case WMS_UDH_RFC822:
        /* Header ID Octet + Header Length Octet + Header Length*/
        length = 1 + 1 + WMS_UDH_OCTETS_RFC822;
        break;

     /*  0x21 - 0x6F Reserved for future use*/
     /*  0x70 - 0x7f Reserved for (U)SIM Toolkit Security Headers */
     /*  0x80 - 0x9F SME to SME specific use*/
     /*  0xA0 - 0xBF Reserved for future use*/
     /*  0xC0 - 0xDF SC specific use*/
     /*  0xE0 - 0xFF Reserved for future use*/

#ifndef FEATURE_SMS_IGNORE_EMS_5_CONTENT
      case WMS_UDH_USER_PROMPT:
        length = 1 + 1 + WMS_UDH_OCTETS_USER_PROMPT;
        break;

      case WMS_UDH_EXTENDED_OBJECT:
        length = 1 + 1 + udh->u.eo.content.length;
        if( udh->u.eo.first_segment == TRUE )
        {
          length += WMS_UDH_OCTETS_EO_HEADER;
        }
        break;
#endif /* ! FEATURE_SMS_IGNORE_EMS_5_CONTENT */

#endif /* ! FEATURE_SMS_EXTENDED */

      default:
#ifdef FEATURE_SMS_EXTENDED
        MSG_ERROR("Unsupported UDH %d", udh->header_id, 0,0);
        length = 0;
#else /* FEATURE_SMS_EXTENDED */
        /* Header ID Octet + Header Length Octet + Header Length */
        length = 1 + 1 + udh->u.other.header_length;
#endif /* FEATURE_SMS_EXTENDED */
        break;

    }
  }
  return length;
}
#endif /* FEATURE_SMS_UDH */

/*=========================================================================
FUNCTION
  wms_ts_compute_user_data_header_length

DESCRIPTION
  Computes the User Data Header Length

DEPENDENCIES
  None

RETURN VALUE
  User Data Header Length in bytes

SIDE EFFECTS
  None
=========================================================================*/
uint32 wms_ts_compute_user_data_header_length
(
  const uint8           num_headers,
  const wms_udh_s_type *headers
)
{
  uint32 length = 0;

#ifdef FEATURE_SMS_UDH
  uint32 i;

  if( headers == NULL )
  {
    MSG_ERROR("Null pointer!",0,0,0);
    return 0;
  }

  if (num_headers > 0)
  {
    length += 1; /* 1 byte for UDHL byte */

    /* User Data Headers Length */
    for ( i=0 ; i<num_headers && i<WMS_MAX_UD_HEADERS ; i++)
    {
      length += (uint32)wms_ts_get_udh_length(&headers[i]);
    }
  }
#endif /* FEATURE_SMS_UDH */

  return length;
}

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

/*=========================================================================
FUNCTION
  wms_ts_init

DESCRIPTION
  Initialize the TS Module

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void wms_ts_init
(
  void
)
{
  if (FALSE == encode_decode_data_crit_sect_init)
  {
    rex_init_crit_sect(&encode_decode_data_crit_sect);
    encode_decode_data_crit_sect_init = TRUE;
  }
}
  
#endif /* FEATURE_CDSMS || FEATURE_GWSMS */
