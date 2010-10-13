/*=============================================================================

FILE: WMSAppUtils.C

    U S E R   I N T E R F A C E   S M S   D I S P L A Y   S E R V I C E

GENERAL DESCRIPTION
  Contains utlity functions for formatting the data displayed and used by the
  WMS application.



(c) COPYRIGHT 2003, 2004, 2005, 2006, 2007, 2008 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/wms/wmsapp/main/latest/src/WmsAppUtils.c#12 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/29/08   HN      Added check for buffer overflow in WMSAPPU_ITOA()
07/14/08   HN      Added check for buffer overflow in WMSAPPU_FormatNumberType(),
                   WMSAPPU_FormatNumberPlan(), WMSAPPU_FormatAddress(),
                   WMSAPPU_FormatPID() and WMSAPPU_FormatDCS().                        
06/27/08   PMD     Cleaned up Feature Flags in Code
06/23/08   PMD     Mainlined FEATURE_CDSMS_IS637B in code
06/12/08   PMD     Removed wms_bc_*** APIs for CDMA Broadcast
06/04/08   PMD     Removed support for Auto / Transparent SMS 
05/19/08   PMD     Fixed Lint Warnings
04/10/08   SM      Fixed Compile Warnings
04/10/08   SJ      Change variable name from memStore to mem_store
04/09/08   SM/PMD  Fixed Potential Buffer overruns
01/31/08   PMD     Updated Copyright Information
10/02/07   PMD     Fixed Lint Errors
09/26/07   PMD     Fixed Lint Error in WMSAPPU_DeleteAllBroadcastMessage
06/06/07   PMD     Fixed Lint Errors
02/22/07   JAS     Featurize Annunciator references for uiOne HDK builds
01/17/07   HQ      Mainlined 637A.
12/08/06   HQ      Cleaned up Critical lint errors: 661, 662.
11/20/06   PMD     Appended 1 to length for Null Character in wms_ts_unpack_ascii
10/31/06   PMD     Fixed Compile Warning due to comparison of uint8 with -1
04/21/06   HQ      Added support for CDMA Read Ack message.
03/06/06   PMD     Added More Teleservice ID Strings
01/26/06   PMD     Fixed Compile Errors by making pMe as const
01/25/06   VK      Updated Copyright Info
01/25/06   VK      Lint fixes
11/20/05   PMD     Fixed WMSAPPU_EventCbSmsNotify()
10/19/05   VK      Featurization fixes
07/26/05   Rex     Featurization fixes
07/25/05   PMD     Removed DCS check for Update Message.
05/25/05   PMD     Added Null Pointer Checks for Broadcast Messages.
05/24/05   PMD     Replaced FREE by WMSAPPU_FREE call in DeleteBroadcastMessage
05/04/05   PMD     Added support for Any Update Number is Change
04/12/05   PMD     Fixed WMSAPPU_CheckIfUpdate Logic
03/25/05   PMD     Fix to Display Immediate Display Messages on Returning to Same Cell
03/24/05   PMD     Updated WMSAPPU_CheckIfUpdate to check for GeoScope and Exact Match
03/10/05   PMD     Used IWMS_BcMmMsgDeleteAllIndication for Deleting CB Cache
02/25/05   Rex     Fix for cphs and als feature
02/20/05   PMD     Added 1x Broadcast Support in Multimode Broadcast
02/01/05   PMD     Cleaned up Featurization around SetIdleMenuTextInBkgrnd.
01/17/05   PMD     Replaced FREEIF calls with WMSAPPU_FREE
01/07/05   PMD     Support for WMSAPPU_MEMCPY, WMSAPPU_FREE and WMSAPPU_MALLOC Functions.
01/07/05   PMD     Added WMSAPPU_MEMCPY, WMSAPPU_FREE and WMSAPPU_MALLOC Functions.
11/17/04   VK      Added the GW_ConvertToUnicode and GW_ConvertFromUnicode Function
11/16/04   PMD     Added Support for WEMT and GSM 7-bit Display.
10/16/04   PMD     Added WMSAPPU_LoadImageFromBuffer function
10/16/04   PMD     Added Logic to Deactivate and Activate on Delete All CB Msgs
10/16/04   PMD     Change Prototype for WMSAPPU_DeleteBroadcastMessage
09/29/04   PMD     Release Instance of IFILEMGR to avoid Memory Leak
09/29/04   PMD     Replaced FREE with FREEIF to ensure we do not free Null Pointers
09/23/04   Rex     Featurize SetIdleMenuTextInBkgrnd for compilation issues
09/21/04   Rex     Added support for CPHS
08/18/04   Rex     Fixed "@" and "í" character mapping btw gsm 7-bit and ASCII table 
08/06/04   Rex     Check for invalid index in WMSAPPU_ReadBroadcastMessage
08/05/04   Rex     Featurize IWMSDIAG 
08/04/04   PMD     Removed GetTable from AddCBService Utility.
07/16/04   PMD     Using SetIdleMenuTextInBkgrnd() API to not go back to root screen
07/15/04   PMD     Changed Comment
07/15/04   PMD     Added Support for Indicating CB Page Deletion to WMS
06/22/04   Rex     Clean up potential occurrences of memory leaks
06/10/04   PMD     Fixed Featurization Issue
06/08/04   PMD     Added Support for Clearing Immediate Display on Deletion.
06/02/04   Rex     Changed class to msg_class to fix C++ compilation issue.
05/14/04   Rex     Added support for WMSAPPU_EventCbSmsNotify and WMSAPPU_GetCbMsgIndex
04/21/04   PMD     Fix for storing Page Numbers in EFS Cache.
04/21/04   PMD     Implemented Circular buffer in RAM and EFS for Broadcast Msgs.
04/08/04   PMD     Added support for Delete All GW CB Services Function
                   Changes Add Services function to Get Table as well
03/31/04   PMD/Rex Moved WMSAPPU_IsMsgActive and WMSAPPU_UpdateIcons.
03/18/04   Rex     Update Icons after marking BC msg for annunciator refresh
03/22/04   PMD     Added Function WMSAPPU_DeleteAllBroadcastMessage
03/22/04   PMD     Added Function WMSAPPU_CheckIfUpdate()
03/16/04   AP      Added support for the user app
03/15/04   Rex     Fixed a bug in WMSAPPU_SplitUserData
02/14/04   PMD     Fixed many bugs in GSM Cell Broadcast Utility Functions
02/03/04   PMD     Cleaned up Lint Warnings in code
02/03/04   PMD     Removed FEATURE_SMS_SEGMENT
01/29/04   Rex     Changed WMSAPPU_FormatTimestamp to convert timestamp
                   from BCD to ASCII value.
                   Added function ConvertBCDToInt
01/28/04   PMD     Added support for Alphanumeric Number Type
01/20/04   PMD     Added WMSAPPU_ITOA()
01/14/04   PMD     Added Function to Segment / Concat Large Messages
01/06/04   Rex     Cause Code Display changes
12/22/03   PMD     Fixed Extra Lines in CDMA Message Info
12/03/03   PMD     Lint related changes for Saber
11/21/03   PMD     Added GSM Cell Broadcast Support
11/06/03   Rex     Status Report Display Changes
10/30/03   PMD     Fixed Display of Negative Timezone in Absolute Validity
10/22/03   PMD     Replaced wms_ts_* by IWMS_Ts* (Transport Service Functions)
10/03/03   AP      Fixed handling of voicemail MT messages: 0 voicemails and
                   voicemail alert display.
09/08/03   PMD     Timestamp now being displayed correctly with Date, Month
                   and Year in CDMA mode.
08/15/03   PMD     Now Supports display of @ character
07/29/03   AP      Modified included files to generate the correct dependencies.
07/07/03   PMD     Changes to remove / suppress LINT errors
06/26/03   SUN     Added support for GSM 160 characters
06/05/03   SUN     Added support for FEATURE_GWSMS_STATUS_REPORTS
05/27/03   PMD     Fixed Null Termination in convertUint8ToAscii
05/22/03   SUN     Fixed few of the GSM Characters Display
05/20/03   SUN     Fixed Validity Display
05/17/03   PMD     Added function ConvertUINT8ToAscii
05/14/03   HQ      Fixed compiler errors on WMSAPPU_FormatClassRoutes() and
                   WMSAPPU_FormatClassMemStore().
05/09/03   SUN     Added support for gsm character set, added support
                   for class summary
05/02/03   AP      Added unicode for cdma dcs.
04/03/03   SUN/AP  Clean up.
03/04/03   AP      Made HexChar available for both modes.
02/27/03   AP      Clean up.
02/14/03   sun     Initial Version
===========================================================================*/

/*===============================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "comdef.h"
#include "AEEAppGen.h"        // AEEApplet structure and Applet services

#ifdef FEATURE_WMS_APP
// AEE Services used by app
#include "AEEStdLib.h"        // AEE Stb Lib Services
#include "AEEMenu.h"          // AEE Menu Services
#include "AEEFile.h"          // AEE File Manager Services
#include "AEEText.h"
#include "AEEBitmap.h"
#include "AEETransform.h"
#include "WmsAppUtils.h"
#include "WmsApp.h"
#include "msg.h"
#include "wms.h"
#include "bit.h"
#include "OEMCFGI.h"
#ifdef FEATURE_IMELODY
  #include "cmx.h"
#endif /*FEATURE_IMELODY*/

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

             Common Utility Functions used for CDMA and GW

===========================================================================*/

/*===========================================================================
FUNCTION WMSAPPU_WriteToBuffer

DESCRIPTION
  Helper function to write to a bounded-buffer. If write leads to overflow the
  buffer is left untouched.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPU_WriteToBuffer
( char* to_buf,
  int to_buf_size,
  char* from_buf,
  int from_buf_size,
  int *offset
)
{
  if (from_buf_size < (to_buf_size - (*offset))) {
    (void)MEMCPY(to_buf + (*offset), from_buf, (uint16)from_buf_size);
    (*offset) += from_buf_size;
    return TRUE;
  }

  return FALSE;
}


/*===========================================================================
FUNCTION WMSAPPU_MAPINDEX

DESCRIPTION
  Maps the scroll line of the desired message to the index of the
  message list.

RETURN
  Index value of the selected message.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
wms_message_index_type WMSAPPU_MapIndex
(
  const CWMSAPP *pMe,
  wms_memory_store_e_type memStore,
  int scrLine
)
{
  wms_message_index_type  i;
  wms_message_index_type  count;

  /* Traverse through the appropriate message list and return early when
  ** scrLine == count.
  */
  if(pMe->m_bTestMode)
  {
    if( memStore > WMS_MEMORY_STORE_MAX)
    {
       return (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX; /* not found */
    }
    for(i=0, count=0; i<WMS_MESSAGE_LIST_MAX; i++)
    {
      /* Inbox processing */
      if(pMe->m_msgInfo.boxType == WMSAPP_INBOX &&
        WMS_IS_MT( pMe->m_pSmsLists[memStore]->tags[i] ))
      {
        if(memStore == WMS_MEMORY_STORE_NV_CDMA)
        {
          if(pMe->m_pSmsLists[memStore]->indices[i] ==
            pMe->m_pSmsLists[memStore]->voice_mail_index)
          {
            continue;
          }
        }
        else if(memStore == WMS_MEMORY_STORE_SIM)
        {
          if(pMe->m_pSmsLists[memStore]->indices[i] ==
            pMe->m_pSmsLists[memStore]->voice_mail_index)
          {
            continue;
          }
        }
        count++;
      }

      /* Outbox processing */
      if(pMe->m_msgInfo.boxType  == WMSAPP_OUTBOX &&
        WMS_IS_MO( (int)pMe->m_pSmsLists[memStore]->tags[i] ))
      {
        count++;
      }

      /* Is this the desired message? */
      if((int)count == scrLine)
      {
        /* found it */
        return i;
      }
    } /* for */
  }

  return (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX; /* not found */

} /* WMSAPPU_MapIndex() */

/* <EJECT> */
/*===========================================================================
FUNCTION WMSAPPU_FORMATNUMBERTYPE

DESCRIPTION
  Formats number type of address.

RETURN
  i, the number of characters to display the number type.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifes buf directly.
===========================================================================*/
uint8 WMSAPPU_FormatNumberType
(
  const wms_address_s_type     * addr,
  uint8                  * buf,
  int                 buf_size
)
{
  if (buf_size > 0) {
    char       * ptr;
    int        i=0;

    switch(addr->number_type)
    {
      case WMS_NUMBER_INTERNATIONAL:
        ptr = "Int'l";
        break;

      case WMS_NUMBER_NATIONAL:
        ptr = "National";
        break;

      case WMS_NUMBER_NETWORK:
        ptr = "Network";
        break;

      case WMS_NUMBER_SUBSCRIBER:
        ptr = "Subscrib";
        break;

      case WMS_NUMBER_ABBREVIATED:
        ptr = "Abbrev";
        break;

      case WMS_NUMBER_ALPHANUMERIC:
        ptr = "AlphaNumeric";
        break;

      default:
        ptr = "Unknown";
        break;
    }

    WMSAPPU_WriteToBuffer((char*)buf, (int)(buf_size), ptr, (int)STRLEN(ptr), &i);   

    return i;
  }
  else
  {
    return 0;
  }
} /* WMSAPPU_FormatNumberType() */

/* <EJECT> */
/*===========================================================================
FUNCTION WMSAPPU_FORMATNUMBERPLAN

DESCRIPTION
  Formats number plan of address.

RETURN
  i, the number of lines needed to display the number plan.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifes buf directly.
===========================================================================*/
uint8 WMSAPPU_FormatNumberPlan
(
  const wms_address_s_type        * addr,
  uint8                     * buf,
  int                 buf_size
)
{
  if (buf_size > 0) {
    char       * ptr;
    int        i = 0;

    switch(addr->number_plan)
    {
      case WMS_NUMBER_PLAN_TELEPHONY:
        ptr = "Telephony";
        break;

      case WMS_NUMBER_PLAN_DATA:
        ptr = "Data";
        break;

      case WMS_NUMBER_PLAN_TELEX:
        ptr = "Telex";
        break;

      case WMS_NUMBER_PLAN_PRIVATE:
        ptr = "Private";
        break;

      case WMS_NUMBER_PLAN_UNKNOWN:
      default:
        ptr = "Unknown";
        break;
    }

    WMSAPPU_WriteToBuffer((char*)buf, (int)(buf_size), ptr, (int)STRLEN(ptr), &i);

    return i;
  }
  else
  {
    return 0;
  }
} /* WMSAPPU_FormatNumberPlan() */

/* <EJECT> */
/*===========================================================================
FUNCTION WMSAPPU_FORMATADDRESS

DESCRIPTION
  Formats address field of message.

RETURN
  i, the number of characters needed to display an address and its parameters

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifes buf directly.
===========================================================================*/
uint8 WMSAPPU_FormatAddress
(
  const CWMSAPP *pMe,
  boolean              digits_only,
  const wms_address_s_type   * addr,
  uint8                * buf,
  int buf_size
)
{  
  if (buf_size > 0) {      
    int   i = 0;

    /* Display the number type and number plan if needed */
    if(digits_only == FALSE)
    {
      i += WMSAPPU_FormatNumberType( addr, buf, buf_size - i);      
      WMSAPPU_WriteToBuffer((char*)buf, (int)(buf_size), "\n", 1, &i);      
      i += WMSAPPU_FormatNumberPlan( addr, buf, buf_size - i);
      WMSAPPU_WriteToBuffer((char*)buf, (int)(buf_size), "\n", 1, &i);      
    }

    /* Check the mode to convert and display the address */
    if ( pMe->m_modeControl == WMSAPP_MODE_GW )
    {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
    }
    else
    {
#ifdef FEATURE_CDSMS
      if ( addr->number_of_digits )
      {
        if(addr->digit_mode == WMS_DIGIT_MODE_4_BIT)
        {
          (void)IWMS_TsDtmfToAscii( pMe->m_pwms, (byte)addr->number_of_digits,
                                (byte *)addr->digits,
                                (byte *)buf + i);
        }
        else
        {
          (void)MEMCPY(buf+i,addr->digits,addr->number_of_digits);
        }
        i+= addr->number_of_digits;
      }
#endif/*FEATURE_CDSMS*/
    }

    return i;
  }
  else
  {
    return 0;
  }
} /* WMSAPPU_FormatAddress */

/* <EJECT> */
/*===========================================================================
FUNCTION WMSAPPU_HEXCHAR

DESCRIPTION
  Converts decimal to ascii char representation.

RETURN
  table[n]: the ascii value.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 WMSAPPU_HexChar
(
  uint8 n
)
{
  static uint8 table[16] =
  { '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

  return table[n];
} /* WMSAPPU_HexChar() */

/*===========================================================================
FUNCTION ConvertNumToASCII

DESCRIPTION
  Convert a number to ASCII
  Returns pointer to character after last character written

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
char *ConvertNumToASCII
(
  word num,                     /* number to convert */
  char *txt                     /* ascii conversion for up to 2 numbers */
)
{
  *txt++ = (byte) (num/10 + '0');
  *txt++ = (byte) (num%10 + '0');
  return( txt );
} /* ConvertNumToASCII() */

/*===========================================================================
FUNCTION ConvertBCDToASCII

DESCRIPTION
  Convert 2 BCD numbers to 2 ASCII values.
  Returns pointer to character after last character written

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
char * ConvertBCDToASCII
(
  char bcd_num,                 /* 2 BCD numbers to convert */
  char *txt                     /* ascii conversion for up to 2 numbers */
)
{
  byte i;
  /* Loop index counter */
  byte pos = 0;
  /* Starting position of BCD number */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Convert 2 BCD numbers to 2 ASCII numbers */
  for(i=0; i < 2; i++)
  {
    /* Move BCD number to byte array */
    *txt = b_unpackb( (byte *) &bcd_num, pos, 4 );

    /* If BCD number = 10, then translate to
    ** ASCII 0.
    */
    if(*txt == 10)
    {
      *txt = 0x30;
    }
    /* Otherwise, add ASCII offset to translate. */
    else
    {
      *txt += 0x30;
    }
    /* Move to next BCD number and ASCII byte */
    pos += 4;
    txt++;
  }
  return( txt );
} /* ConvertBCDToASCII() */


/*===========================================================================
FUNCTION WMSAPPU_ConvertUINT8ToASCII

DESCRIPTION
  Converts a Uint8 integer (0-255) to appropriate ASCII Text
  Used for retrival of Files given uint8 number
  Returns Number of Characters in the New ASCII Text

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int WMSAPPU_ConvertUINT8ToASCII
(
  uint8 number,                 /* uint8 number to convert */
  char *txt                     /* Ascii output string */
)
{
  int pos = 0;

  if (number / 100)
  {
    // This code is executed if number >= 100

    // For the 100's place digit
    txt[pos] = (char)((number/100) + '0');
    number = number % 100;
    pos++;

    // For the 10's place digit
    txt[pos] = (char)((number/10) + '0');
    number = number % 10;
    pos++;

    // For the 1's place digit
    txt[pos] = (char)(number + '0');
    pos++;
  }
  else if (number / 10)
  {
    // This code is executed if 99 >= number >= 10

    // For the 10's place digit
    txt[pos] = (char)((number/10) + '0');
    number = number % 10;
    pos++;

    // For the 1's place digit
    txt[pos] = (char)(number + '0');
    pos++;
  }
  else
  {
    // This code is executed if 9 >= number >= 0

    // For the 1's place digit
    txt[pos] = (char)(number + '0');
    pos++;
  }

  // To Null Terminate String
  txt[pos] = '\0';

  // Return the Number of Characters
  return (pos);
}

/*=========================================================================
FUNCTION
  WMSAPPU_ConvertBCDToInt

DESCRIPTION
  Convert a BCD to Integer.
  Valid BCD values altogether are from 0 to 99 and byte arrangement is <MSB, ... ,LSB)

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None
=========================================================================*/
uint8 WMSAPPU_ConvertBCDToInt
(
  const uint8 bcd
)
{
  return ( (bcd & 0x0F) + (((bcd & 0xF0) >> 4) * 10) );
}


/*===========================================================================
FUNCTION WMSAPPU_ITOA

DESCRIPTION
  Converts any Integer to appropriate ASCII Text
  Returns Number of Characters in the New ASCII Text

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint8 WMSAPPU_ITOA
(
  int number,
  char *string,
  int size,
  int base
)
{
  if (base == 10)
  {
    (void) SNPRINTF(string, size, "%d", number);
  }
  else if (base == 16)
  {
    (void) SNPRINTF(string, size, "%x", number);
  }
  else if (base == 8)
  {
    (void) SNPRINTF(string, size, "%o", number);
  }
  /* Returns number of characters */
  return ((uint8)(STRLEN(string)));
}


/*===========================================================================
FUNCTION GetJulianData
SMS UTC time

DESCRIPTION
  Converts Time from BCD to Julian

DEPENDENCIES
  None

RETURNS

SIDE EFFECTS
  None

===========================================================================*/
void GetJulianTime
(
  wms_timestamp_s_type    mc_time,
  JulianType              *julian
)
{
  byte text_ptr[2] ;

  /* Hours are given as 24 hour clock time */
  /* Translate BCD hours to ascii format */

  (void) ConvertBCDToASCII(mc_time.year, (char *) &text_ptr[0]);

  julian->wYear = ((text_ptr[0] - 0x30) * 10) +  (text_ptr[1] - 0x30);
  if(julian->wYear < 80)
    julian->wYear = julian->wYear + 2000 ;
  else
    julian->wYear = julian->wYear + 1900 ;

  (void) ConvertBCDToASCII (mc_time.month, (char *) &text_ptr[0]);

  julian->wMonth = ((text_ptr[0] - 0x30) * 10) + (text_ptr[1] - 0x30);

  (void) ConvertBCDToASCII (mc_time.day, (char *) &text_ptr[0]);
  julian->wDay = ((text_ptr[0] - 0x30) * 10) + (text_ptr[1] - 0x30);


  (void) ConvertBCDToASCII (mc_time.hour, (char *) &text_ptr[0]);

  julian->wHour = ((text_ptr[0] - 0x30) * 10) +  (text_ptr[1] - 0x30);

  (void) ConvertBCDToASCII (mc_time.minute, (char *) &text_ptr[0]);

  julian->wMinute = ((text_ptr[0] - 0x30) * 10) +  (text_ptr[1] - 0x30);

  (void) ConvertBCDToASCII (mc_time.second, (char *) &text_ptr[0]);

  julian->wSecond = ((text_ptr[0] - 0x30) * 10) +  (text_ptr[1] - 0x30);

  // TBD: timezone

} /* GetJulianTime () */

/*===========================================================================
FUNCTION WMSAPPU_FORMATTIMESTAMP

DESCRIPTION
  Formats timestamp value of the message.

RETURN
  i, the number of lines needed to display the timestamp.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifes buf directly.
===========================================================================*/
uint8 WMSAPPU_FormatTimestamp
(
  const CWMSAPP               *pMe,
  wms_timestamp_s_type  * pTime,
  byte                  * disp,
  int                disp_size
)
{
  if (disp_size > 0) {
    int i = 0;
    char tempChar;
    {
      if ((disp_size - i) > 2) {
        (void) ConvertBCDToASCII (pTime->month, (char *) &disp[i]);
        i += 2;
      }
      WMSAPPU_WriteToBuffer((char*)disp, (int)(disp_size), "/", 1, &i);

      if ((disp_size - i) > 2) {
        (void) ConvertBCDToASCII (pTime->day, (char *) &disp[i]);
        i += 2;
      }
      WMSAPPU_WriteToBuffer((char*)disp, (int)(disp_size), "/", 1, &i);

      if ((disp_size - i) > 2) {
        (void) ConvertBCDToASCII (pTime->year, (char *) &disp[i]);
        i += 2;
      }
      WMSAPPU_WriteToBuffer((char*)disp, (int)(disp_size), "/", 1, &i);

      if ((disp_size - i) > 2) {
        (void) ConvertBCDToASCII (pTime->hour, (char *) &disp[i]);
        i += 2;
      }
      WMSAPPU_WriteToBuffer((char*)disp, (int)(disp_size), ":", 1, &i);

      if ((disp_size - i) > 2) {
        (void) ConvertBCDToASCII (pTime->minute, (char *) &disp[i]);
        i += 2;
      }
      WMSAPPU_WriteToBuffer((char*)disp, (int)(disp_size), ":", 1, &i);

      if ((disp_size - i) > 2) {
        (void) ConvertBCDToASCII (pTime->second, (char *) &disp[i]);
        i += 2;
      }
      WMSAPPU_WriteToBuffer((char*)disp, (int)(disp_size), " ", 1, &i);

      /* Only if Mode = GW, display the time zone */
      if(pMe->m_modeControl == WMSAPP_MODE_GW)
      {
        WMSAPPU_WriteToBuffer((char*)disp, (int)(disp_size), "GMT", 3, &i);

        if (pTime->timezone >= 0)
        {
          WMSAPPU_WriteToBuffer((char*)disp, (int)(disp_size), "+", 1, &i);
        }
        else
        {
          WMSAPPU_WriteToBuffer((char*)disp, (int)(disp_size), "-", 1, &i);
          pTime->timezone *= -1;
        }
        tempChar = (pTime->timezone / 10) + '0';
        WMSAPPU_WriteToBuffer((char*)disp, (int)(disp_size), &tempChar, 1, &i);
        tempChar = (pTime->timezone % 10) + '0';
        WMSAPPU_WriteToBuffer((char*)disp, (int)(disp_size), &tempChar, 1, &i);
      }

      disp[i] = 0;

      return i;
    }
  }
  else
  {
    return 0;
  }
} /* WMSAPPU_FormatTimestamp() */

/* <EJECT> */
/*===========================================================================
FUNCTION WMSAPPU_GETMSGCOUNT

DESCRIPTION
  Gets the message count for the outbox or inbox.

RETURN
  count, the number of messages in the particular box.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int WMSAPPU_GetMsgCount
(
  const CWMSAPP *pMe,
  wmsapp_box_type box_type,
  wms_memory_store_e_type mem_store
)
{
  uint8 i=0;
  int count=0;

#ifdef FEATURE_GWSMS_STATUS_REPORTS
#error code not present
#endif/*FEATURE_GWSMS_STATUS_REPORTS*/

    /* Traverse through the message list */
  if(pMe->m_bTestMode)
  {
    if( mem_store >= WMS_MEMORY_STORE_MAX+1 )
    {
       return (int)WMS_DUMMY_MESSAGE_INDEX; /* not found */
    }
    /* Check if the message list is empty */
    if(pMe->m_pSmsLists[mem_store]->len == 0)
    {
      return count;
    }

    for(i=0; i < pMe->m_pSmsLists[mem_store]->len; i++)
    {
      /* Check for the desired box type, added check in box type for meaningful
      ** tag and increase count.
      */
      switch(box_type)
      {
        case WMSAPP_OUTBOX:
          if(WMS_IS_MO((int)pMe->m_pSmsLists[mem_store]->tags[i]))
          {
            count ++;
          }
          break;

        case WMSAPP_INBOX:
          if(WMS_IS_MT(pMe->m_pSmsLists[mem_store]->tags[i]))
          {
            if(mem_store == WMS_MEMORY_STORE_NV_CDMA)
            {
              if(pMe->m_pSmsLists[mem_store]->indices[i] ==
                 pMe->m_pSmsLists[mem_store]->voice_mail_index)
              {
                continue;
              }
            }
            else if(mem_store == WMS_MEMORY_STORE_SIM)
            {
              if(pMe->m_pSmsLists[mem_store]->indices[i] == 
                 pMe->m_pSmsLists[mem_store]->voice_mail_index)
              {
                continue;
              }
            }
            count ++;
          }
          break;

        default:
          MSG_ERROR("Wrong box type to get a message count",0,0,0);
          break;
      }
    }
  }

  return count;
} /* WMSAPPU_GetMsgCount() */

#ifdef FEATURE_CDSMS
/*===========================================================================

                      Utility Functions used for CDMA

===========================================================================*/
/* <EJECT> */
/*===========================================================================
FUNCTION WMSAPPU_FORMATTELESERVICE

DESCRIPTION
  Formats teleservice param of the message.

RETURN
  i, the number of characters needed to display the teleservice.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifes buf directly.
===========================================================================*/
uint8 WMSAPPU_FormatTeleservice
(
  wms_teleservice_e_type  teleservice,
  uint8                    *buf
)
{
  char *ptr;
  uint8 i=0;

  char *header = "TELESERVICE: ";

  (void)STRCPY((char*)buf, header);
  i += (uint8)STRLEN(header);

  switch(teleservice)
  {
    case WMS_TELESERVICE_CMT_91:
      ptr = "CMT 91";
      break;

    case WMS_TELESERVICE_CPT_95:
      ptr = "CPT 95";
      break;

    case WMS_TELESERVICE_CMT_95:
      ptr = "CMT 95";
      break;

    case WMS_TELESERVICE_VMN_95:
      ptr = "VMN 95";
      break;

    case WMS_TELESERVICE_WEMT:
      ptr = "WEMT";
      break;

    case WMS_TELESERVICE_WAP:
    case WMS_TELESERVICE_SCPT:
    case WMS_TELESERVICE_CATPT:
      ptr = "TBD";
      break;

    case WMS_TELESERVICE_IS91_PAGE:
      ptr = "IS91 Page";
      break;

    case WMS_TELESERVICE_IS91_VOICE_MAIL:
      ptr = "IS91 Voicemail";
      break;

    case WMS_TELESERVICE_IS91_SHORT_MESSAGE:
      ptr = "IS91 Short Message";
      break;

    case WMS_TELESERVICE_MWI:
      ptr = "Message Waiting Ind.";
      break;

    case WMS_TELESERVICE_BROADCAST:
      ptr = "Broadcast";
      break;

    default:
      ptr = "?";
      break;
  }

  (void)STRCPY((char*)buf+i, ptr);
  i += (uint8)STRLEN(ptr);

  return i;
} /* WMSAPPU_FormatTeleservice() */

/* <EJECT> */
/*===========================================================================
FUNCTION WMSAPPU_FORMATCDMADCS

DESCRIPTION
  Formats DCS value of the message.

RETURN
  i, the number of characters needed to display the DCS.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifes buf directly.
===========================================================================*/
uint8 WMSAPPU_FormatCDMADcs
(
  wms_user_data_encoding_e_type  dcs,
  uint8                          *buf
)
{
  uint8 i = 0;
  char *ptr;
  char *header = "DCS: ";

  (void)STRCPY((char*)buf, header);
  i += (uint8)STRLEN(header);

  switch(dcs)
  {
    case WMS_ENCODING_OCTET:
      ptr = "OCTET";
      break;

    case WMS_ENCODING_IS91EP:
      ptr = "IS91EP";
      break;

    case WMS_ENCODING_ASCII:
      ptr = "ASCII";
      break;

    case WMS_ENCODING_IA5:
      ptr = "IA5";
      break;

    case WMS_ENCODING_UNICODE:
      ptr = "UNICODE";
      break;

    case WMS_ENCODING_GSM_7_BIT_DEFAULT:
      ptr = "GSM 7-BIT";
      break;

    default:
      ptr = "NA";
      break;
  }

  (void)STRCPY((char*)buf+i, ptr);
  i+= (uint8)STRLEN(ptr);
  return i;
} /* WMSAPPU_FormatCDMADcs() */

/*===========================================================================
FUNCTION WMSAPPU_FORMATPRIORITY

DESCRIPTION
  Formats priority value of the message.

RETURN
  i, the number of characters needed to display the priority.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifes buf directly.
===========================================================================*/
uint8 WMSAPPU_FormatPriority
(
  wms_priority_e_type  priority,
  uint8                *buf
)
{
  char *ptr;
  uint8 i = 0;

  switch(priority)
  {
    case WMS_PRIORITY_URGENT:
      ptr = "<URGENT>";
      break;

    case WMS_PRIORITY_EMERGENCY:
      ptr = "<EMERGENCY>";
      break;

    case WMS_PRIORITY_INTERACTIVE:
      ptr = "<INTERACTIV>";
      break;

    default:
      ptr = "<NORMAL>";
      break;
  }

  (void)MEMCPY((char*)buf, ptr, STRLEN(ptr));
  i+= (uint8)STRLEN(ptr);

  return i;
} /* WMSAPPU_FormatPriority() */

/*===========================================================================
FUNCTION WMSAPPU_FORMATPRIVACY

DESCRIPTION
  Formats privacy value of the message.

RETURN
  i, the number of characters needed to display the privacy.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifes buf directly.
===========================================================================*/
uint8 WMSAPPU_FormatPrivacy
(
  wms_privacy_e_type  privacy,
  uint8               *buf
)
{

  char *ptr;
  uint8 i =0;
  char *header ="<PRIV: ";

  (void)STRCPY((char *) buf,header);
  i = (uint8)STRLEN(header);

  switch(privacy)
  {
    case WMS_PRIVACY_RESTRICTED:
      ptr = "restricted>";
      break;

    case WMS_PRIVACY_CONFIDENTIAL:
      ptr = "confidential>";
      break;

    case WMS_PRIVACY_SECRET:
      ptr = "secret>";
      break;

    default:
      ptr = "normal>";
      break;
  }
  (void)STRCPY((char *)(buf+i), ptr);
  i+= (uint8)STRLEN(ptr);

  return i;
} /* WMSAPPU_FormatPrivacy() */

/*===========================================================================
FUNCTION WMSAPPU_FORMATACKINFO

DESCRIPTION
  Formats ack info value(s) of the message.

RETURN
  i, the number of characters needed to display the ack info.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifes buf directly.
===========================================================================*/
uint8 WMSAPPU_FormatAckInfo
(
  uint32  mask,
  boolean tl_ack,
  boolean del_ack,
  boolean user_ack,
  boolean read_ack,
  uint8   *buf
)
{
  uint8 i;
  char *header = "(TL ACK:";

  (void)STRCPY((char *) buf,header);
  i = (uint8)STRLEN(header);

  /* Display TL_Ack */
  if(tl_ack)
  {
    buf[i] ='T';
  }
  else
  {
    buf[i] ='F';
  }
  i+=1;
  buf[i++]=')';

  /* Display Del ack and user ack if required */
  if(mask & WMS_MASK_BD_REPLY_OPTION)
  {
    buf[i] = ' ';
    i++;
    header = "(DEL ACK:";

    (void)STRCPY((char *) buf+i,header);
    i+= (uint8)STRLEN(header);

    if(del_ack)
    {
      buf[i] = 'T';
    }
    else
    {
      buf[i] = 'F';
    }
    i++;
    buf[i++]=')';


    buf[i] = ' ';
    i++;
    header = "(USER ACK:";
    (void)STRCPY((char *)buf+i,header);
    i+= (uint8)STRLEN(header);

    if(user_ack)
    {
      buf[i] ='T';
    }
    else
    {
      buf[i] ='F';
    }
    i++;
    buf[i++]=')';

    /* Read Ack request */
    buf[i] = ' ';
    i++;
    header = "(READ ACK:";
    (void)STRCPY((char *)buf+i,header);
    i+= (uint8)STRLEN(header);

    if(read_ack)
    {
      buf[i] ='T';
    }
    else
    {
      buf[i] ='F';
    }
    i++;
    buf[i++]=')';

  }

  return i;
} /* WMSAPPU_FormatAckInfo() */

/*===========================================================================
FUNCTION WMSAPPU_FORMATLANGUAGE

DESCRIPTION
  Formats language value of the message.

RETURN
  i, the number of characters needed to display the language.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifes buf directly.
===========================================================================*/
uint8 WMSAPPU_FormatLanguage
(
  wms_language_e_type  language,
  uint8                *buf
)
{
  uint8 i = 0;
  char  *ptr;
  char *header ="LANGUAGE: ";

  (void)STRCPY((char *) buf,header);
  i+= (uint8)STRLEN(header);

  switch(language)
  {
    case WMS_LANGUAGE_ENGLISH:
      ptr = "English";
      break;

    case WMS_LANGUAGE_FRENCH:
      ptr = "French";
      break;

    case WMS_LANGUAGE_SPANISH:
      ptr = "Spanish";
      break;

    case WMS_LANGUAGE_JAPANESE:
      ptr = "Japanese";
      break;

    case WMS_LANGUAGE_KOREAN:
      ptr = "Korean";
      break;

    case WMS_LANGUAGE_CHINESE:
      ptr = "Chinese";
      break;

    case WMS_LANGUAGE_HEBREW:
      ptr = "Hebrew";
      break;

    default:
      ptr = "Unspecified";
      break;
  }
  (void)STRCPY((char *) buf+i, ptr);
  i += (uint8)STRLEN(ptr);
  return i;
} /* WMSAPPU_FormatLanguage() */

/* <EJECT> */
/*===========================================================================
FUNCTION WMSAPPU_FORMATDISPLAYMODE

DESCRIPTION
  Formats display mode value of the message.

RETURN
  i, the number of characters needed to display the display mode.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifes buf directly.
===========================================================================*/
uint8 WMSAPPU_FormatDisplayMode
(
  wms_display_mode_e_type  display_mode,
  uint8                    *buf
)
{
  uint8 i = 0;
  char  *ptr;

  char *header = "<DISP: ";
  (void)STRCPY((char *) buf,header);
  i += (uint8)STRLEN(header);

  switch(display_mode)
  {
    case WMS_DISPLAY_MODE_IMMEDIATE:
      ptr = "immediate>";
      break;

    case WMS_DISPLAY_MODE_USER_INVOKE:
      ptr = "user-invoke>";
      break;

    case WMS_DISPLAY_MODE_RESERVED:
      ptr = "reserved>";
      break;

    default:
      ptr = "default>";
      break;
  }
  (void)STRCPY((char *) buf+i, ptr);
  i+= (uint8)STRLEN(ptr);
  return i;
} /* WMSAPPU_FormatDisplayMode() */

/* <EJECT> */
/*===========================================================================
FUNCTION WMSAPPU_FORMATBDTYPE

DESCRIPTION
  Formats bearer data message type value of the message.

RETURN
  i, the number of characters needed to display the BD type.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifes buf directly.
===========================================================================*/
uint8 WMSAPPU_FormatBDType
(
  wms_bd_message_type_e_type  bd_type,
  uint8                    *buf
)
{
  char *ptr;
  uint8 i =0;
  switch(bd_type)
  {
    case WMS_BD_TYPE_DELIVER:
      ptr =  "<DELIVER>";
      break;

    case WMS_BD_TYPE_SUBMIT:
      ptr =  "<SUBMIT>";
      break;

    case WMS_BD_TYPE_CANCELLATION:
      ptr =  "<CANCEL>";
      break;

    case WMS_BD_TYPE_DELIVERY_ACK:
      ptr = "<DEL ACK>";
      break;

    case WMS_BD_TYPE_USER_ACK:
      ptr = "<USER ACK>";
      break;

    default:
      ptr = "<UNKNOWN>";
      break;
  }

  (void)STRCPY((char *)buf+i, ptr);
  i+= (uint8)STRLEN(ptr);
  return i;

} /* WMSAPPU_FormatBDType() */

/*===========================================================================
FUNCTION WMSAPPU_ConvertToUnicode

DESCRIPTION
  Converts the user data to unicode.

DEPENDENCIES
  None

SIDE EFFECTS
  Modifies user data and data length.
===========================================================================*/
void WMSAPPU_ConvertToUnicode
(
  wms_client_bd_s_type *bd_data
)
{
  uint8 i;

  /* Add 0x00 as the high byte */
  for( i=bd_data->user_data.data_len; i>0; i-- )
  {
    bd_data->user_data.data[i*2-1] = bd_data->user_data.data[i-1];
    bd_data->user_data.data[i*2-2] = 0x00;
  }

  /* Double the value of user data length for unicode */
  bd_data->user_data.data_len = (uint8)(bd_data->user_data.data_len*2);
} /* WMSAPPU_ConvertToUnicode() */

/*===========================================================================
FUNCTION WMSAPPU_ConvertFromUnicode

DESCRIPTION
  Converts the user data from unicode.

DEPENDENCIES
  None

SIDE EFFECTS
  Modifies user data and data length.
===========================================================================*/
void WMSAPPU_ConvertFromUnicode
(
  wms_client_bd_s_type *bd_data
)
{
  uint8 i;

  /* Decrease the data length by half */
  bd_data->user_data.data_len = bd_data->user_data.data_len/2;

  /* Extract the high byte from the user data.  This will be equivalent to
  ** 8-bit Ascii */
  for( i=0; i<bd_data->user_data.data_len; i++ )
  {
    bd_data->user_data.data[i] = bd_data->user_data.data[i*2+1];
  }
} /* WMSAPPU_ConvertFromUnicode() */

/* <EJECT> */
/*===========================================================================
FUNCTION WMSAPPU_FORMATALERTMODE

DESCRIPTION
  Formats alert mode value of the message.

RETURN
  i, the number of characters needed to display the alert mode.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifes buf directly.
===========================================================================*/
uint8 WMSAPPU_FormatAlertMode
(
  wms_alert_mode_e_type  alert_mode,
  uint8                  *buf
)
{
  uint8 i = 0;
  char  *ptr;
  char *header = "<ALERT: ";

  (void)STRCPY((char *) buf,header);
  i = (uint8)STRLEN(header);

  switch(alert_mode)
  {
    case WMS_ALERT_MODE_LOW_PRIORITY:
      ptr = "low>";
      break;

    case WMS_ALERT_MODE_MEDIUM_PRIORITY:
      ptr = "med>";
      break;

    case WMS_ALERT_MODE_HIGH_PRIORITY:
      ptr = "HIGH>";
      break;

    default:
      ptr = "default>";
      break;
  }

  (void)STRCPY((char *)buf+i, ptr);
  i+= (uint8)STRLEN(ptr);
  return i;

} /* WMSAPPU_FormatAlertMode */


/*===========================================================================
FUNCTION IS_NON_STD_EMAIL

DESCRIPTION
  Checks whether the address is using the non-stanard e-mail address format.
  Returns TRUE if so.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPU_IsNonStdEmail
(
  boolean             is_dtmf,
  const uint8         *n_buf,
  const uint8         n_length
)
{
  int i;
  /* 4-bit address to detect a non-std email address */
  byte num_to_compare[5] = "6245";

  /* Temporary ascii variable to hold converted dtmf number */
  uint8 ascii_num = '0';

  /* Check the length first for a possible match */
  if(n_length != 4)
  {
    return FALSE;
  }

  for(i=0; i<4; i++)
  {
    if(is_dtmf)
    {
      /* Convert to ascii */
      /* No need to convert other possible dtmf values ie. 0,# because if
      ** they are contained in the address then it will obviously not be
      ** non-std email
      */
      ascii_num = n_buf[i] + 0x30;
    }
    else
    {
      ascii_num = n_buf[i];
    }

    if(ascii_num == num_to_compare[i])
    {
      /* Good so far */
      continue;
    }
    return FALSE;
  }

  /* This is using a non-std email address format */
  return TRUE;
} /* WMSAPPU_IsNonStdEmail() */

/*===========================================================================
FUNCTION WMSAPPU_GetNonStdEmail

DESCRIPTION
  Gets the value of the non standard e-mail address. Returns length.

DEPENDENCIES
  None

SIDE EFFECTS
  Changes the value of email_addr directly.
===========================================================================*/
uint8 WMSAPPU_GetNonStdEmail
(
  const uint8 *user_data,
  uint8 *email_addr
)
{
  uint8 i;

  for(i=0; i<(uint8)(STRLEN((char*)user_data)); i++)
  {
    /* Check if we've reached the end of the e-mail address */
    if(user_data[i] != ' ')
    {
      /* Copy address */
      email_addr[i] = user_data[i];
      continue;
    }
    break;
  }
  return i;
} /* WMSAPPU_GetNonStdEmail() */

/* <EJECT> */
/*===========================================================================
FUNCTION WMSAPPU_PUTNONSTDEMAIL

DESCRIPTION
  Puts the non-std email address in the user data porition of the message.

RETURN
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifies userData directly.
===========================================================================*/
void WMSAPPU_PutNonStdEmail
(
  const uint8 *emailAddr,
  CWMSAPP *pMe,
  wms_cdma_user_data_s_type *userData
)
{
  uint8 length;

  /* Clear the buffer */
  (void)MEMSET(pMe->m_buf, 0, sizeof(pMe->m_buf));

  /* Unpack the data if we have to */
  if((userData->encoding == WMS_ENCODING_ASCII) ||
     (userData->encoding == WMS_ENCODING_IA5))
  {
    userData->data_len =
      IWMS_TsUnpackAscii( pMe->m_pwms, userData, userData->number_of_digits+1, (byte *)pMe->m_buf);
  }
  else
  {
    /* First put the current contents of the user data in the buffer */
    (void)MEMCPY(pMe->m_buf, userData->data, userData->data_len);
  }

  /* Assign new encoding */
  userData->encoding = WMS_ENCODING_ASCII;

  /* Clear the userData->data */
  (void)MEMSET(userData->data, 0, (uint16)WMS_MAX_LEN);

  /* Calculate the length of the email address add 1 to include the space */
  length = pMe->m_emailAddr.len;

  /* store in the user data */
  (void)MEMCPY((uint8 *)userData->data, emailAddr, length);

  /* Add the space */
  userData->data[length] = ' ';

  /* Add the rest of the text */
  (void)MEMCPY((uint8 *)userData->data+length+1, pMe->m_buf, userData->data_len);

  /* Assign the new length */
  userData->data_len += length+1;

  /* Add the null character */
  userData->data[userData->data_len] = '\0';
} /* WMSAPPU_PutNonStdEmail() */

/* <EJECT> */
/*===========================================================================
FUNCTION WMSAPPU_REMOVENONSTDEMAIL

DESCRIPTION
  Removes the non-std email from the user data of the message.

RETURN
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifies the user data and possibly the mask of clBd.
===========================================================================*/
void WMSAPPU_RemoveNonStdEmail
(
  const CWMSAPP *pMe,
  wms_client_bd_s_type  *clBd
)
{
  uint8 length;
  uint8 *udData;
  uint8 *buf;

  udData = MALLOC(WMS_MAX_LEN);
  if (udData == NULL)
  {
    return;
  }

  /* unpack the ascii encoded ud */
  clBd->user_data.data_len =
    IWMS_TsUnpackAscii( pMe->m_pwms, &clBd->user_data, clBd->user_data.number_of_digits+1,
                        (byte *)udData);

  buf = MALLOC(WMS_MAX_LEN);
  if (buf == NULL)
  {
    WMSAPPU_FREE (udData);
    return;
  }

  /* get the length of the email address */
  length = WMSAPPU_GetNonStdEmail(udData,
                                  buf);

  /* if the length is equal to ud length then there is no other text msg */
  if(length == clBd->user_data.data_len)
  {
    clBd->mask &= ~WMS_MASK_BD_USER_DATA;
  }
  else
  {
    clBd->user_data.data_len = length - clBd->user_data.data_len;

    /* Clear the data */
    (void)MEMSET(clBd->user_data.data, 0, sizeof(clBd->user_data.data));

    /* copy the user data(no email) into the user data */
    (void)MEMCPY(buf, udData+length, clBd->user_data.data_len);

    /* repack ascii */
    clBd->user_data.number_of_digits =
      IWMS_TsPackAscii( pMe->m_pwms, (char *)buf, (byte *)clBd->user_data.data,
                        (byte *)&clBd->user_data.data_len,
                        (byte *)&clBd->user_data.padding_bits);
  }

  WMSAPPU_FREE(buf);
  WMSAPPU_FREE(udData);
} /* WMSAPPU_RemoveNonStdEmail() */

#ifdef FEATURE_CDSMS_BROADCAST
/* <EJECT> */
/*===========================================================================
FUNCTION WMSAPPU_BCSORTSVCS

DESCRIPTION
  Sorts the Broadcast SMS services into their respective languages and assigns
  status to a particular languge if all services are selected, partially
  selected or all services are not selected.

RETURN
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifies pMe directly.
===========================================================================*/
void WMSAPPU_BCSortSvcs
(
  CWMSAPP *pMe
)
{
  uint16 i;
  uint16 tempIndex=0;

  /* Array to get status */
  int selectedSize[WMSAPP_MAX_LANGUAGES];

  (void)MEMSET(selectedSize, 0, sizeof(selectedSize));

  /* Traverse through all of the services in the BC table and put the index
  ** in the BC sorted list per assigned language.
  */
  for(i=0; i<pMe->m_bcInfo.tableSize; i++)
  {
    /* Check for unspecified language */
    if(pMe->m_bcInfo.entries[i].srv_id.bc_srv_id.language == WMS_LANGUAGE_UNSPECIFIED)
    {
      /* Assign the index of the BC table to the next available open slot for
      ** this particular language.
      */
      tempIndex = pMe->m_bcSortedSvcs[WMS_LANGUAGE_UNSPECIFIED].size;
      pMe->m_bcSortedSvcs[WMS_LANGUAGE_UNSPECIFIED].entryIndex[tempIndex] = i;

      /* Increase the size */
      pMe->m_bcSortedSvcs[WMS_LANGUAGE_UNSPECIFIED].size++;

      /* Bookkeeping for status of selected services for this language,
      ** used later.
      */
      if(pMe->m_bcInfo.entries[i].selected)
      {
        selectedSize[WMS_LANGUAGE_UNSPECIFIED]++;
      }
    }
    else
      /* Check for English */
    if(pMe->m_bcInfo.entries[i].srv_id.bc_srv_id.language == WMS_LANGUAGE_ENGLISH)
    {
      tempIndex = pMe->m_bcSortedSvcs[WMS_LANGUAGE_ENGLISH].size;
      pMe->m_bcSortedSvcs[WMS_LANGUAGE_ENGLISH].entryIndex[tempIndex] = i;

      pMe->m_bcSortedSvcs[WMS_LANGUAGE_ENGLISH].size++;
      if(pMe->m_bcInfo.entries[i].selected)
      {
        selectedSize[WMS_LANGUAGE_ENGLISH]++;
      }
    }
    else
      /* Check for French */
    if(pMe->m_bcInfo.entries[i].srv_id.bc_srv_id.language == WMS_LANGUAGE_FRENCH)
    {
      tempIndex = pMe->m_bcSortedSvcs[WMS_LANGUAGE_FRENCH].size;
      pMe->m_bcSortedSvcs[WMS_LANGUAGE_FRENCH].entryIndex[tempIndex] = i;

      pMe->m_bcSortedSvcs[WMS_LANGUAGE_FRENCH].size++;
      if(pMe->m_bcInfo.entries[i].selected)
      {
        selectedSize[WMS_LANGUAGE_FRENCH]++;
      }
    }
    else
      /* Check for Spanish */
    if(pMe->m_bcInfo.entries[i].srv_id.bc_srv_id.language == WMS_LANGUAGE_SPANISH)
    {
      tempIndex = pMe->m_bcSortedSvcs[WMS_LANGUAGE_SPANISH].size;
      pMe->m_bcSortedSvcs[WMS_LANGUAGE_SPANISH].entryIndex[tempIndex] = i;

      pMe->m_bcSortedSvcs[WMS_LANGUAGE_SPANISH].size++;
      if(pMe->m_bcInfo.entries[i].selected)
      {
        selectedSize[WMS_LANGUAGE_SPANISH]++;
      }
    }
    else
      /* Check for Japanese */
    if(pMe->m_bcInfo.entries[i].srv_id.bc_srv_id.language == WMS_LANGUAGE_JAPANESE)
    {
      tempIndex = pMe->m_bcSortedSvcs[WMS_LANGUAGE_JAPANESE].size;
      pMe->m_bcSortedSvcs[WMS_LANGUAGE_JAPANESE].entryIndex[tempIndex] = i;

      pMe->m_bcSortedSvcs[WMS_LANGUAGE_JAPANESE].size++;
      if(pMe->m_bcInfo.entries[i].selected)
      {
        selectedSize[WMS_LANGUAGE_JAPANESE]++;
      }
    }
    else
      /* Check for Korean */
    if(pMe->m_bcInfo.entries[i].srv_id.bc_srv_id.language == WMS_LANGUAGE_KOREAN)
    {
      tempIndex = pMe->m_bcSortedSvcs[WMS_LANGUAGE_KOREAN].size;
      pMe->m_bcSortedSvcs[WMS_LANGUAGE_KOREAN].entryIndex[tempIndex] = i;

      pMe->m_bcSortedSvcs[WMS_LANGUAGE_KOREAN].size++;
      if(pMe->m_bcInfo.entries[i].selected)
      {
        selectedSize[WMS_LANGUAGE_KOREAN]++;
      }
    }
    else
      /* Check for Chinese */
    if(pMe->m_bcInfo.entries[i].srv_id.bc_srv_id.language == WMS_LANGUAGE_CHINESE)
    {
      tempIndex = pMe->m_bcSortedSvcs[WMS_LANGUAGE_CHINESE].size;
      pMe->m_bcSortedSvcs[WMS_LANGUAGE_CHINESE].entryIndex[tempIndex] = i;

      pMe->m_bcSortedSvcs[WMS_LANGUAGE_CHINESE].size++;
      if(pMe->m_bcInfo.entries[i].selected)
      {
        selectedSize[WMS_LANGUAGE_CHINESE]++;
      }
    }
    else
      /* Check for Hebrew */
    if(pMe->m_bcInfo.entries[i].srv_id.bc_srv_id.language == WMS_LANGUAGE_HEBREW)
    {
      tempIndex = pMe->m_bcSortedSvcs[WMS_LANGUAGE_HEBREW].size;
      pMe->m_bcSortedSvcs[WMS_LANGUAGE_HEBREW].entryIndex[tempIndex] = i;

      pMe->m_bcSortedSvcs[WMS_LANGUAGE_HEBREW].size++;
      if(pMe->m_bcInfo.entries[i].selected)
      {
        selectedSize[WMS_LANGUAGE_HEBREW]++;
      }
    }
  }

  /* Compare size and selected for status */
  for(i=0; i<WMSAPP_MAX_LANGUAGES; i++)
  {
      /* None of the services are selected for this language */
    if((pMe->m_bcSortedSvcs[i].size == 0) || (selectedSize[i] == 0))
    {
      pMe->m_bcSortedSvcs[i].status = WMSAPP_BC_NONE_ACTIVE;
    }
      /* Some but not all of the services are selected for this language */
    else if(pMe->m_bcSortedSvcs[i].size > selectedSize[i])
    {
      pMe->m_bcSortedSvcs[i].status = WMSAPP_BC_SOME_ACTIVE;
    }
      /* All services are active for this language */
    else
    {
      pMe->m_bcSortedSvcs[i].status = WMSAPP_BC_ALL_ACTIVE;
    }
  }
} /* WMSAPPU_BCSortSvcs() */

#endif /* FEATURE_CDSMS_BROADCAST */
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

/*=========================================================================
FUNCTION
  WMSAPPU_FormatClassRoutes

DESCRIPTION
  Displays the class routes.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/

uint8 WMSAPPU_FormatClassRoutes
(
  const wms_route_e_type route,
  uint8             *buf
)
{
  char   *ptr;
  char *header = "Route: ";
  uint8 i = (uint8)STRLEN(header);

  (void)STRCPY((char*)buf,header);

  switch(route)
  {
    case WMS_ROUTE_DISCARD:
      ptr = "DISCARD";
      break;

    case WMS_ROUTE_STORE_AND_NOTIFY:
      ptr = "STORE";
      break;

    case WMS_ROUTE_TRANSFER_AND_ACK:
      ptr = "TRANSFER and ACK";
      break;

    case WMS_ROUTE_TRANSFER_ONLY:
      ptr = "TRANSFER_ONLY";
      break;

    default:
      ptr ="NONE";
      break;
  }

  (void)STRCPY((char*)buf+i, ptr);
  i+= (uint8)STRLEN(ptr);
  return i;
}

/*=========================================================================
FUNCTION
  WMSAPPU_FormatClassMemStore

DESCRIPTION
  Displays the memory store

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/

uint8 WMSAPPU_FormatClassMemStore
(
  const wms_memory_store_e_type memStore,
  uint8             *buf
)
{
  char   * ptr;
  char *header = "Memory Store: ";
  uint8 i = (uint8)STRLEN(header);

  (void)STRCPY((char*)buf,header);

  switch(memStore)
  {
    case WMS_MEMORY_STORE_RAM_GW:
      ptr = "Temp Memory";
      break;

    case WMS_MEMORY_STORE_SIM:
      ptr = "Card Memory";
      break;

    case WMS_MEMORY_STORE_NV_GW:
      ptr = "Phone Memory";
      break;

    case WMS_MEMORY_STORE_NONE:
      ptr = "None";
      break;

    default:
      ptr ="Invalid Memory Store";
      break;
  }

  (void)STRCPY((char*)buf+i,ptr);
  i+= (uint8)STRLEN(ptr);
  return i;
}

/*=========================================================================
FUNCTION
  WMSAPPU_ConvertHexToAscii

DESCRIPTION
  Converts the Given Hexadecimal Number to its Ascii Value

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
byte WMSAPPU_ConvertHexToAscii
(
 byte num                 /* Hexadecimal Number to convert */
)
{
  if (num <= 9)
    return (num + '0');
  else
    return ((num-10) + 'A');
}



/*=========================================================================
FUNCTION
  WMSAPPU_TpStatusEnumToString

DESCRIPTION
  Converts the TP Status enum type to string for display

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
char* WMSAPPU_TpStatusEnumToString
(
 wms_tp_status_e_type tp_status
)
{
  char *ptr;

  switch(tp_status)
  {
    case WMS_TP_STATUS_RECEIVED_OK:
      ptr = " RECEIVED_OK ";
      break;

    case WMS_TP_STATUS_UNABLE_TO_CONFIRM_DELIVERY:
      ptr = " UNABLE_TO_CONFIRM_DELIVERY ";
      break;

    case WMS_TP_STATUS_REPLACED:
      ptr = " REPLACED ";
      break;

    case WMS_TP_STATUS_TRYING_CONGESTION:
      ptr = " TRYING_CONGESTION ";
      break;

    case WMS_TP_STATUS_TRYING_SME_BUSY:
      ptr = " TRYING_SME_BUSY ";
      break;

    case WMS_TP_STATUS_TRYING_NO_RESPONSE_FROM_SME:
      ptr = " TRYING_NO_RESPONSE_FROM_SME ";
      break;

    case WMS_TP_STATUS_TRYING_SERVICE_REJECTED:
      ptr = " TRYING_SERVICE_REJECTED ";
      break;

    case WMS_TP_STATUS_TRYING_QOS_NOT_AVAILABLE:
      ptr = " TRYING_QOS_NOT_AVAILABLE ";
      break;

    case WMS_TP_STATUS_TRYING_SME_ERROR:
      ptr = " TRYING_SME_ERROR ";
      break;

    case WMS_TP_STATUS_PERM_REMOTE_PROCEDURE_ERROR:
      ptr = " PERM_REMOTE_PROCEDURE_ERROR ";
      break;

    case WMS_TP_STATUS_PERM_INCOMPATIBLE_DEST:
      ptr = " PERM_INCOMPATIBLE_DEST ";
      break;

    case WMS_TP_STATUS_PERM_REJECTED_BY_SME:
      ptr = " PERM_REJECTED_BY_SME ";
      break;

    case WMS_TP_STATUS_PERM_NOT_OBTAINABLE:
      ptr = " PERM_NOT_OBTAINABLE ";
      break;

    case WMS_TP_STATUS_PERM_QOS_NOT_AVAILABLE:
      ptr = " PERM_QOS_NOT_AVAILABLE ";
      break;

    case WMS_TP_STATUS_PERM_NO_INTERWORKING:
      ptr = " PERM_NO_INTERWORKING ";
      break;

    case WMS_TP_STATUS_PERM_VP_EXPIRED:
      ptr = " PERM_VP_EXPIRED ";
      break;

    case WMS_TP_STATUS_PERM_DELETED_BY_ORIG_SME:
      ptr = " PERM_DELETED_BY_ORIG_SME ";
      break;

    case WMS_TP_STATUS_PERM_DELETED_BY_SC_ADMIN:
      ptr = " PERM_DELETED_BY_SC_ADMIN ";
      break;

    case WMS_TP_STATUS_PERM_SM_NO_EXISTING:
      ptr = " PERM_SM_NO_EXISTING ";
      break;

    case WMS_TP_STATUS_TEMP_CONGESTION:
      ptr = " TEMP_CONGESTION ";
      break;

    case WMS_TP_STATUS_TEMP_SME_BUSY:
      ptr = " TEMP_SME_BUSY ";
      break;

    case WMS_TP_STATUS_TEMP_NO_RESPONSE_FROM_SME:
      ptr = " TEMP_NO_RESPONSE_FROM_SME ";
      break;

    case WMS_TP_STATUS_TEMP_SERVICE_REJECTED:
      ptr = " TEMP_SERVICE_REJECTED ";
      break;

    case WMS_TP_STATUS_TEMP_QOS_NOT_AVAILABLE:
      ptr = " TEMP_QOS_NOT_AVAILABLE ";
      break;

    case WMS_TP_STATUS_TEMP_SME_ERROR:
      ptr = " TEMP_SME_ERROR ";
      break;

    case WMS_TP_STATUS_LAST:
      ptr = " LAST ";
      break;

    default:
      ptr = " UNRECOGNIZE STATUS ";
      break;
  }

  return ptr;
}


/*===========================================================================
FUNCTION WMSAPPU_CauseCodeEnumToString

DESCRIPTION
  Processes the cause code information received from WMS

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/
char* WMSAPPU_CauseCodeEnumToString
(
  const wms_submit_report_info_s_type *pRptInfo
)
{
  char *CauseDisplay=NULL;

  /* cause code display for GW */
  if (pRptInfo->message_mode == WMS_MESSAGE_MODE_GW)
  {
    switch(pRptInfo->cause_info.cause_value)
    {
      case WMS_RP_CAUSE_UNASSIGNED_NUMBER:
        CauseDisplay = " Unassigned Number (1) ";
        break;

      case WMS_RP_CAUSE_OPERATOR_DETERMINED_BARRING:
        CauseDisplay = " Operator Barring (8) ";
        break;

      case WMS_RP_CAUSE_CALL_BARRED:
        CauseDisplay = " Call Barred (10) ";
        break;

      case WMS_RP_CAUSE_RESERVED_11:
        CauseDisplay = " Reserved (11) ";
        break;

      case WMS_RP_CAUSE_NETWORK_FAILURE:
        CauseDisplay = " Network Failure (17) ";
        break;

      case WMS_RP_CAUSE_SM_REJECTED:
        CauseDisplay = " Short Message Rejected (21) ";
        break;

      case WMS_RP_CAUSE_MEMORY_EXCEEDED:
        CauseDisplay = " Memory Exceeded (22) ";
        break;

      case WMS_RP_CAUSE_DESTINATION_OUT_OF_ORDER:
        CauseDisplay = " Destination Out of Order (27) ";
        break;

      case WMS_RP_CAUSE_UNIDENTIFIED_SUBSCRIBER:
        CauseDisplay = " Unidentified Subscriber (28) ";
        break;

      case WMS_RP_CAUSE_FACILITY_REJECTED:
        CauseDisplay = " Facility Rejected (29) ";
        break;

      case WMS_RP_CAUSE_UNKNOWN_SUBSCRIBER:
        CauseDisplay = " Unknown Subscriber (30) ";
        break;

      case WMS_RP_CAUSE_NETWORK_OUT_OF_ORDER:
        CauseDisplay = " Network Out of Order (38) ";
        break;

      case WMS_RP_CAUSE_TEMPORARY_FAILURE:
        CauseDisplay = " Temporary Failure (41) ";
        break;

      case WMS_RP_CAUSE_CONGESTION:
        CauseDisplay = " Congestion (42) ";
        break;

      case WMS_RP_CAUSE_RESOURCES_UNAVAILABLE_UNSPECIFIED:
        CauseDisplay = " Resource Unavailable, Unspecified (47) ";
        break;

      case WMS_RP_CAUSE_REQUESTED_FACILITY_NOT_SUBSCRIBED:
        CauseDisplay = " Requested Facility Not Subscribed (50) ";
        break;

      case WMS_RP_CAUSE_REQUESTED_FACILITY_NOT_IMPLEMENTED:
        CauseDisplay = " Requested Facility Not Implemented (69) ";
        break;

      case WMS_RP_CAUSE_INVALID_SM_TRANSFER_REF_VALUE:
        CauseDisplay = " Invalid Short Message Transfer Ref Value (81) ";
        break;

      case WMS_RP_CAUSE_SEMANTICALLY_INCORRECT_MESSAGE:
        CauseDisplay = " Semantically Incorrect Message (95) ";
        break;

      case WMS_RP_CAUSE_INVALID_MANDATORY_INFORMATION:
        CauseDisplay = " Invalid Mandatory Information (96) ";
        break;

      case WMS_RP_CAUSE_MESSAGE_TYPE_NON_EXISTENT:
        CauseDisplay = " Message Type Non Existent (97) ";
        break;

      case WMS_RP_CAUSE_MESSAGE_TYPE_NOT_COMPATIBLE:
        CauseDisplay = " Message Type Not Compatible (98) ";
        break;

      case WMS_RP_CAUSE_IE_NON_EXISTENT_OR_NOT_IMPLEMENTED:
        CauseDisplay = " Info Element Not Existent or Not Implemented (99) ";
        break;

      case WMS_RP_CAUSE_PROTOCOL_ERROR_UNSPECIFIED:
        CauseDisplay = " Protocol Error, Unspecified (111) ";
        break;

      case WMS_RP_CAUSE_INTERWORKING_UNSPECIFIED:
        CauseDisplay = " Interworking, Unspecified (127) ";
        break;

      default:
        CauseDisplay = " Unknown Reason ";
        break;
    }
  }

  /* cause code display for CDMA */
  if (pRptInfo->message_mode == WMS_MESSAGE_MODE_CDMA)
  {
    switch(pRptInfo->cause_info.tl_status)
    {
      case WMS_TL_ADDRESS_VACANT_S:
        CauseDisplay = " Address Vacant (0) ";
        break;

      case WMS_TL_ADDRESS_TRANSLATION_FAILURE_S:
        CauseDisplay = " Address Translation Failure (1) ";
        break;

      case WMS_TL_NETWORK_RESOURCE_SHORTAGE_S:
        CauseDisplay = " Network Resource Storage (2) ";
        break;

      case WMS_TL_NETWORK_FAILURE_S:
        CauseDisplay = " Network Failure (3) ";
        break;

      case WMS_TL_INVALID_TELESERVICE_ID_S:
        CauseDisplay = " Invalid Teleservice ID (4) ";
        break;

      case WMS_TL_OTHER_NETWORK_PROBLEM_S:
        CauseDisplay = " Other Network Problem (5) ";
        break;

      case WMS_TL_OTHER_NETWORK_PROBLEM_MORE_FIRST_S:
        CauseDisplay = " Other Network Problem More First (6) ";
        break;

      case WMS_TL_OTHER_NETWORK_PROBLEM_MORE_LAST_S:
        CauseDisplay = " Other Network Problem More Last (31) ";
        break;

      case WMS_TL_NO_PAGE_RESPONSE_S:
        CauseDisplay = " No Page Response (32) ";
        break;

      case WMS_TL_DESTINATION_BUSY_S:
        CauseDisplay = " Destination Busy (33) ";
        break;

      case WMS_TL_NO_ACK_S:
        CauseDisplay = " No ACK (34) ";
        break;

      case WMS_TL_DESTINATION_RESOURCE_SHORTAGE_S:
        CauseDisplay = " Destination Resource Storage (35> ";
        break;

      case WMS_TL_SMS_DELIVERY_POSTPONED_S:
        CauseDisplay = " Delivery Postponed (36) ";
        break;

      case WMS_TL_DESTINATION_OUT_OF_SERVICE_S:
        CauseDisplay = " Destination Out of Service (37) ";
        break;

      case WMS_TL_DESTINATION_NO_LONGER_AT_THIS_ADDRESS_S:
        CauseDisplay = " Destination No Longer at this Address (38) ";
        break;

      case WMS_TL_OTHER_TERMINAL_PROBLEM_S:
        CauseDisplay = " Other Terminal Problem (39) ";
        break;

      case WMS_TL_OTHER_TERMINAL_PROBLEM_MORE_FIRST_S:
        CauseDisplay = " Other Terminal Problem More First (40) ";
        break;

      case WMS_TL_OTHER_TERMINAL_PROBLEM_MORE_LAST_S:
        CauseDisplay = " Other Terminal Problem more Last (47) ";
        break;

      case WMS_TL_SMS_DELIVERY_POSTPONED_MORE_FIRST_S:
        CauseDisplay = " SMS Delivery Postponed More First (48) ";
        break;

      case WMS_TL_SMS_DELIVERY_POSTPONED_MORE_LAST_S:
        CauseDisplay = " SMS Delivery Postponed More Last (63) ";
        break;

      case WMS_TL_RADIO_IF_RESOURCE_SHORTAGE_S:
        CauseDisplay = " Radio IF Resource Storage (64) ";
        break;

      case WMS_TL_RADIO_IF_INCOMPATIBLE_S:
        CauseDisplay = " Radio IF Incompatible (65) ";
        break;

      case WMS_TL_OTHER_RADIO_IF_PROBLEM_S:
        CauseDisplay = " Other Radio IF Problem (66) ";
        break;

      case WMS_TL_OTHER_RADIO_IF_PROBLEM_MORE_FIRST_S:
        CauseDisplay = " Other Radio IF Problem More First (67) ";
        break;

      case WMS_TL_OTHER_RADIO_IF_PROBLEM_MORE_LAST_S:
        CauseDisplay = " Other Radio IF Problem More Last (95) ";
        break;

      case WMS_TL_UNEXPECTED_PARM_SIZE_S:
        CauseDisplay = " Unexpected Parameter Size (96) ";
        break;

      case WMS_TL_SMS_ORIGINATION_DENIED_S:
        CauseDisplay = " SMS Origination Denied (97) ";
        break;

      case WMS_TL_SMS_TERMINATION_DENIED_S:
        CauseDisplay = " SMS Termination Denied (98) ";
        break;

      case WMS_TL_SUPPL_SERVICE_NOT_SUPPORTED:
        CauseDisplay = " Supplementary Service Not Supported (99) ";
        break;

      case WMS_TL_SMS_NOT_SUPPORTED_S:
        CauseDisplay = " SMS Not Supported (100) ";
        break;

      case WMS_TL_RESERVED_101_S:
        CauseDisplay = " Reserved (101) ";
        break;

      case WMS_TL_MISSING_EXPECTED_PARM_S:
        CauseDisplay = " Missing Expected Parameter (102) ";
        break;

      case WMS_TL_MISSING_MANDATORY_PARM_S:
        CauseDisplay = " Missing Mandatory Parameter (103) ";
        break;

      case WMS_TL_UNRECOGNIZED_PARM_VALUE_S:
        CauseDisplay = " Unrecognized Parameter Value (104) ";
        break;

      case WMS_TL_UNEXPECTED_PARM_VALUE_S:
        CauseDisplay = " Unexpected Parameter Value (105) ";
        break;

      case WMS_TL_USER_DATA_SIZE_ERROR_S:
        CauseDisplay = " User Data Size Error (106) ";
        break;

      case WMS_TL_OTHER_GENERAL_PROBLEMS_S:
        CauseDisplay = " Other General Problems (107) ";
        break;

      case WMS_TL_OTHER_GENERAL_PROBLEMS_MORE_FIRST_S:
        CauseDisplay = " Other General Problems Most First (108) ";
        break;

      case WMS_TL_OTHER_GENERAL_PROBLEMS_MORE_LAST_S:
        CauseDisplay = " Other General Problems More Last (255) ";
        break;

      default:
        CauseDisplay = " Unknown Reason ";
        break;
    }
  }

  return CauseDisplay;
}

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /*FEATURE_GWSMS_BROADCAST*/

/*===========================================================================
FUNCTION WMSAPPU_CharToByteCount

DESCRIPTION
  Converts the number of bytes to number of characters based on the encoding used.

DEPENDENCIES
  None

SIDE EFFECTS
  None

RETURN VALUE
  Number of Characters
===========================================================================*/
uint16 WMSAPPU_CharToByteCount
(
  uint16                 charCount,
  wmsapp_encoding_e_type encoding
)
{
  uint16 byteCount=0;
  switch(encoding)
  {
    case WMSAPP_ENCODING_7_BIT:
      byteCount = (uint16)((charCount*7 + 7)/8);
      break;
    case WMSAPP_ENCODING_8_BIT:
      byteCount = charCount;
      break;
    case WMSAPP_ENCODING_16_BIT:
      byteCount = (uint16)(charCount*2);
      break;
    default:
      break;
  }
  return byteCount;
}

/*===========================================================================
FUNCTION WMSAPPU_ByteToCharCount

DESCRIPTION
  Converts the number of characters to number of bytes based on the encoding used.

DEPENDENCIES
  None

SIDE EFFECTS
  None

RETURN VALUE
  Number of Bytes
===========================================================================*/
uint16 WMSAPPU_ByteToCharCount
(
  uint16                 byteCount,
  wmsapp_encoding_e_type encoding
)
{
  uint16 charCount=0;
  switch(encoding)
  {
    case WMSAPP_ENCODING_7_BIT:
      charCount = (uint16)((byteCount*8)/7);
      break;
    case WMSAPP_ENCODING_8_BIT:
      charCount = byteCount;
      break;
    case WMSAPP_ENCODING_16_BIT:
      charCount = (uint16)(byteCount/2);
      break;
    default:
      break;
  }
  return charCount;
}


/*===========================================================================
FUNCTION WMSAPPU_SplitUserData

DESCRIPTION
  Splits the Long User Data into Multiple segments.
  ** This function allocates memory for the Split Message.
     It is upto the Calling function to FREE the Memory.

DEPENDENCIES
  None

SIDE EFFECTS
  None

RETURN VALUE
  TRUE: Successful Segmentation
  FALSE: Segmentation Failed
===========================================================================*/
boolean WMSAPPU_SplitUserData
(
  const CWMSAPP                   * pMe, /* INPUT */
  const wmsapp_user_data_s_type   * origUserData, /* INPUT */
  uint8                     * numSegments, /* OUTPUT */
  wmsapp_user_data_s_type   ** pSplitUserDataPtr /* OUTPUT */
)
{
  boolean retVal = TRUE;

#ifdef FEATURE_SMS_UDH 
  uint8 i,j,k,segments=0;  /* Number of segments the original message is being split */

  int origUserDataLength=0; /* Number of bytes of original message; used to check if message fits in 1 segment */

  uint16 currentByteCount=0;
  uint16 maxByteCount=0;

  uint16 headerByteCount;
  uint16 dataByteCount;

  int      startPosition = 0;
  int      origUserDataChars;
  int      tempChar=0;

  boolean isRFC822HeaderPresent = FALSE;
  uint8 RFC822HeaderIndex=0;

  int   closestPosition = -1;
  uint8 closestHeaderIndex=0;

  uint16 MsgRef=0;

  uint8 initialOffset=0;
  uint8 headerOffset=0;

  /* Array for Number of headers in each segment */
  uint8 *numHeaders;
  int   *FromPosition;
  int   *ToPosition;

  /* Array for storing which header of origUserData goes in which segment*/
  int *origHeaderSegment;
  int *origHeaderPosition;

  wmsapp_user_data_s_type  *splitUserDataPtr;   /* Allocated by Function;
                                                   To be Freed by Calling function */

  /* Check if whole message fits in One Segment */
  /* Add the Header Length Byte */
  if (origUserData->num_headers > 0)
  {
    origUserDataLength += 1;
  }

  /* Add the Header Length */
  for (i = 0; i < origUserData->num_headers; i++)
  {
    origUserDataLength += (int)IWMS_TsGetHeaderLength(pMe->m_pwms, &origUserData->headers[i]);
  }

  /* Add the Data Length */
  origUserDataLength += WMSAPPU_CharToByteCount(origUserData->num_chars, origUserData->encoding);

  if (origUserDataLength <= WMSAPP_MAX_USER_DATA_LENGTH)
  {
    /* Fits in 1 segment; No Segmentation Needed */
    *numSegments = 1;

    splitUserDataPtr = MALLOC(sizeof(wmsapp_user_data_s_type));
    (void) MEMCPY(splitUserDataPtr, origUserData, sizeof(wmsapp_user_data_s_type));

    if (origUserData->headers)
    {
      splitUserDataPtr->headers = MALLOC(sizeof(wms_udh_s_type)*origUserData->num_headers);
      (void) MEMCPY(splitUserDataPtr->headers, origUserData->headers, sizeof(wms_udh_s_type)*origUserData->num_headers);
    }

    if (origUserData->data)
    {
      if (origUserData->encoding == WMSAPP_ENCODING_16_BIT)
      {
        splitUserDataPtr->data = MALLOC(origUserData->num_chars * 2);
        (void) MEMCPY(splitUserDataPtr->data, origUserData->data, origUserData->num_chars * 2);
      }
      else
      {
        splitUserDataPtr->data = MALLOC(origUserData->num_chars);
        (void) MEMCPY(splitUserDataPtr->data, origUserData->data, origUserData->num_chars);
      }
    }
    *pSplitUserDataPtr = splitUserDataPtr;
    return TRUE;
  }

  switch(pMe->m_segmentSetting)
  {
    case  WMSAPP_NO_SEGMENT:
      return FALSE;

    case WMSAPP_USE_CONCAT_8:
      maxByteCount = WMSAPP_MAX_USER_DATA_LENGTH - (5 + 1); /* 5 = Concat 8 Header Length */
      break;                                              /* 1 = User Data Header Length */

    case WMSAPP_USE_CONCAT_16:
      maxByteCount = WMSAPP_MAX_USER_DATA_LENGTH - (6 + 1); /* 6 = Concat 16 Header Length */
      break;                                              /* 1 = User Data Header Length */
  }

  /* The message is definitely greater than the max and has to be SEGMENTED */
  origHeaderSegment  = MALLOC(origUserData->num_headers * sizeof(int));
  origHeaderPosition = MALLOC(origUserData->num_headers * sizeof(int));

  currentByteCount = 0;
  origUserDataChars = origUserData->num_chars;

  numHeaders = MALLOC(WMSAPP_MAX_SEGMENTS);
  FromPosition = MALLOC(WMSAPP_MAX_SEGMENTS * sizeof(int));
  ToPosition = MALLOC(WMSAPP_MAX_SEGMENTS * sizeof(int));

  /* Initialize the arrays to predefined values */
  for (i=0; i<WMSAPP_MAX_SEGMENTS; i++)
  {
    numHeaders[i] = 0;
    FromPosition[i] = 0;
    ToPosition[i] = 0;
  }

  for (i=0; i<origUserData->num_headers; i++)
  {
    origHeaderSegment[i] = -1;
    origHeaderPosition[i] = -1;
  }

  /* RFC822 Handling */
  for (i=0; i<origUserData->num_headers; i++)
  {
    if (origUserData->headers[i].header_id == WMS_UDH_RFC822)
    {
      isRFC822HeaderPresent = TRUE;
      initialOffset = origUserData->headers[i].u.rfc822.header_length;
      RFC822HeaderIndex = i;
      dataByteCount = WMSAPPU_CharToByteCount(initialOffset, origUserData->encoding)
                    + IWMS_TsGetHeaderLength(pMe->m_pwms, &origUserData->headers[RFC822HeaderIndex]);

      maxByteCount -= (uint16)dataByteCount;
      if (maxByteCount < 1)
      {
        MSG_ERROR("RFC822 Header %d in Original Data does not allow any other data",i,0,0);
        retVal = FALSE;
      }
      break;
    }
  }

  /* Check for position independent headers; If any, pack them first */
  for (i=0; i<origUserData->num_headers; i++)
  {
    if (retVal == TRUE)
    {
      switch(origUserData->headers[i].header_id)
      {
        case WMS_UDH_CONCAT_8:
        case WMS_UDH_CONCAT_16:
          /* Ignore these Headers */
          break;

        /* Position Independent headers */
        case WMS_UDH_SPECIAL_SM:
        case WMS_UDH_PORT_8:
        case WMS_UDH_PORT_16:
        case WMS_UDH_SMSC_CONTROL:
        case WMS_UDH_SOURCE:
        case WMS_UDH_WCMP:
          headerByteCount = IWMS_TsGetHeaderLength(pMe->m_pwms, &origUserData->headers[i]);
          if (currentByteCount + headerByteCount <= maxByteCount)
          {
            origHeaderSegment[i] = segments;
            currentByteCount += headerByteCount;
            numHeaders[segments]++;
          }
          else
          {
            segments+=1;
            origHeaderSegment[i] = segments;
            currentByteCount = headerByteCount;
            if (currentByteCount > maxByteCount)
            {
              MSG_ERROR("Header %d in Original Data is not self contained",i,0,0);
              retVal = FALSE;
            }
            numHeaders[segments]++;
          }
          break;

        /* Position Dependent headers */
        case WMS_UDH_TEXT_FORMATING:
          origHeaderPosition[i] = origUserData->headers[i].u.text_formating.start_position;
          break;

        case WMS_UDH_PRE_DEF_SOUND:
          origHeaderPosition[i] = origUserData->headers[i].u.pre_def_sound.position;
          break;

        case WMS_UDH_USER_DEF_SOUND:
          origHeaderPosition[i] = origUserData->headers[i].u.user_def_sound.position;
          break;

        case WMS_UDH_PRE_DEF_ANIM:
          origHeaderPosition[i] = origUserData->headers[i].u.pre_def_anim.position;
          break;

        case WMS_UDH_LARGE_ANIM:
          origHeaderPosition[i] = origUserData->headers[i].u.large_anim.position;
          break;

        case WMS_UDH_SMALL_ANIM:
          origHeaderPosition[i] = origUserData->headers[i].u.small_anim.position;
          break;

        case WMS_UDH_LARGE_PICTURE:
          origHeaderPosition[i] = origUserData->headers[i].u.large_picture.position;
          break;

        case WMS_UDH_SMALL_PICTURE:
          origHeaderPosition[i] = origUserData->headers[i].u.small_picture.position;
          break;

        case WMS_UDH_VAR_PICTURE:
          origHeaderPosition[i] = origUserData->headers[i].u.var_picture.position;
          break;

        default:
          break;
      }
    }
    else
    {
      break; /* From FOR loop */
    }
  }

  /* Ignored Headers have origHeaderPosition = -1 & origHeaderSegment = -1 */
  /* Position Independent Headers are assigned origHeaderSegment */
  /* Position Dependent Headers are assigned origHeaderPosition */

  /* currentByteCount will give us bytes in current segment */


  /* Now check for Position Dependent Segments */

  while (startPosition <= origUserDataChars)
  {
    if (retVal == TRUE)
    {
      closestPosition = -1;

      for (i=0; i<origUserData->num_headers; i++)
      {
        if (origHeaderSegment[i] != -1)
        {
          /* This header is already being stored in a segment */
          continue;
        }

        if (origHeaderPosition[i] == -1)
        {
          /* Ignore this header; It should have already been
            stored and passed the first check */
          continue;
        }
        if (closestPosition == -1)
        {
          closestPosition = origHeaderPosition[i];
          closestHeaderIndex = i;
        }
        else if (closestPosition > origHeaderPosition[i])
        {
          closestPosition = origHeaderPosition[i];
          closestHeaderIndex = i;
        }
        else if (closestPosition == origHeaderPosition[i])
        {
          /* Special Handling for Text Formatting Header */
          if (origUserData->headers[closestHeaderIndex].header_id == WMS_UDH_TEXT_FORMATING)
          {
            /* Handle Text Formatting Header last in the same position */
            closestHeaderIndex = i;
          }
        }
      }

      if (closestPosition == -1)
      {
        /* No more User Data Headers Present */
        /* Hence Append the Remaining Characters */
        dataByteCount = WMSAPPU_CharToByteCount((uint16)(origUserDataChars-startPosition), origUserData->encoding);

        while (currentByteCount + dataByteCount > maxByteCount)
        {
          tempChar = WMSAPPU_ByteToCharCount((maxByteCount-currentByteCount), origUserData->encoding);
          ToPosition[segments] += tempChar;
          segments+=1;
          dataByteCount = dataByteCount-(maxByteCount-currentByteCount);
          currentByteCount = 0;
          FromPosition[segments] = ToPosition[segments] = ToPosition[segments-1];
        }
        currentByteCount += dataByteCount;
        ToPosition[segments] = origUserDataChars;
        startPosition = origUserDataChars;
        break; /* Break out of while loop */
      }

      if (closestPosition > startPosition)
      {
        /* Copy the Data First */
        dataByteCount = WMSAPPU_CharToByteCount((uint16)(closestPosition-startPosition), origUserData->encoding);

        while (currentByteCount + dataByteCount > maxByteCount)
        {
          tempChar = WMSAPPU_ByteToCharCount((maxByteCount-currentByteCount), origUserData->encoding);
          ToPosition[segments] += tempChar;
          segments+=1;
          dataByteCount = dataByteCount-(maxByteCount-currentByteCount);
          currentByteCount = 0;
          FromPosition[segments] = ToPosition[segments] = ToPosition[segments-1];
        }
        currentByteCount += dataByteCount;
        ToPosition[segments] = closestPosition;
        startPosition = closestPosition;
      }

      /* Copy the closest Header */
      dataByteCount = IWMS_TsGetHeaderLength(pMe->m_pwms, &origUserData->headers[closestHeaderIndex]);

      /* Special Handling for Text Formatting Header */
      if (origUserData->headers[closestHeaderIndex].header_id == WMS_UDH_TEXT_FORMATING)
      {
        dataByteCount += WMSAPPU_CharToByteCount(origUserData->headers[closestHeaderIndex].u.text_formating.text_formatting_length, origUserData->encoding);
      }

      if (currentByteCount + dataByteCount <= maxByteCount)
      {
        currentByteCount += dataByteCount;
        origHeaderSegment[closestHeaderIndex] = segments;
        numHeaders[segments]++;
      }
      else
      {
        segments+=1;
        origHeaderSegment[closestHeaderIndex] = segments;
        currentByteCount = dataByteCount;
        if (currentByteCount > maxByteCount)
        {
          MSG_ERROR("Header %d in Original Data is not self contained",closestHeaderIndex,0,0);
          retVal = FALSE;
        }
        numHeaders[segments]++;
        FromPosition[segments] = ToPosition[segments] = ToPosition[segments-1];
      }
      /* Special Handling for Text Formatting Header */
      if (origUserData->headers[closestHeaderIndex].header_id == WMS_UDH_TEXT_FORMATING)
      {
        ToPosition[segments] += origUserData->headers[closestHeaderIndex].u.text_formating.text_formatting_length;
        startPosition = ToPosition[segments];
      }
    }
    else
    {
      break;
    }
  } /* while */

  if (retVal == TRUE)
  {
    /* Place data into Segments */
    * numSegments = segments+1;

    /* Now Allocate the SplitUserDataPtr */
    splitUserDataPtr = MALLOC(sizeof(wmsapp_user_data_s_type) * (segments+1));

    /* Get Message Reference */
    (void)ICONFIG_GetItem(pMe->m_pIConfig, CFGI_SMS_SEGMENT_REF_NUM, &MsgRef, 2);

    /* GETRAND((byte*)&MsgRef, 2); */

    /* Assign the User Data to the Split Messages */
    for (j = 0; j < (segments+1); j++)
    {
      if (isRFC822HeaderPresent)
      {
        /* Extra one for Concat Header; Extra one for RFC822 Header */
        splitUserDataPtr[j].num_headers = numHeaders[j] + 2;
        splitUserDataPtr[j].headers = MALLOC(sizeof(wms_udh_s_type) * (numHeaders[j]+2));
      }
      else
      {
        /* Extra one for Concat Header */
        splitUserDataPtr[j].num_headers = numHeaders[j] + 1;
        splitUserDataPtr[j].headers = MALLOC(sizeof(wms_udh_s_type) * (numHeaders[j]+1));
      }

      /* Add Concat Header as the first one */
      if (pMe->m_segmentSetting == WMSAPP_USE_CONCAT_8)
      {
        splitUserDataPtr[j].headers[0].header_id = WMS_UDH_CONCAT_8;
        splitUserDataPtr[j].headers[0].u.concat_8.msg_ref = (uint8)(MsgRef & 0x00FF); /* Lower Byte */
        splitUserDataPtr[j].headers[0].u.concat_8.seq_num = j+1;
        splitUserDataPtr[j].headers[0].u.concat_8.total_sm = segments+1;
      }
      else
      {
        splitUserDataPtr[j].headers[0].header_id = WMS_UDH_CONCAT_16;
        splitUserDataPtr[j].headers[0].u.concat_16.msg_ref = MsgRef;
        splitUserDataPtr[j].headers[0].u.concat_16.seq_num = j+1;
        splitUserDataPtr[j].headers[0].u.concat_16.total_sm = segments+1;
      }

      if (isRFC822HeaderPresent)
      {
        splitUserDataPtr[j].headers[1] = origUserData->headers[RFC822HeaderIndex];
        headerOffset = 2;
      }
      else
      {
        headerOffset = 1;
      }

      k=0;

      /* Assign Headers */
      for (i = 0; i < numHeaders[j]; i++)
      {
        while(k < origUserData->num_headers)
        {
          /* Belongs to this segment */
          if (origHeaderSegment[k] == j)
          {
            splitUserDataPtr[j].headers[i+headerOffset] = origUserData->headers[k];

            /* Update position */
            switch(splitUserDataPtr[j].headers[i+headerOffset].header_id)
            {
              /* Position Dependent headers */
              case WMS_UDH_TEXT_FORMATING:
                splitUserDataPtr[j].headers[i+headerOffset].u.text_formating.start_position -= (uint8)(FromPosition[j]);
                break;

              case WMS_UDH_PRE_DEF_SOUND:
                splitUserDataPtr[j].headers[i+headerOffset].u.pre_def_sound.position -= (uint8)(FromPosition[j]);
                break;

              case WMS_UDH_USER_DEF_SOUND:
                splitUserDataPtr[j].headers[i+headerOffset].u.user_def_sound.position -= (uint8)(FromPosition[j]);
                break;

              case WMS_UDH_PRE_DEF_ANIM:
                splitUserDataPtr[j].headers[i+headerOffset].u.pre_def_anim.position -= (uint8)(FromPosition[j]);
                break;

              case WMS_UDH_LARGE_ANIM:
                splitUserDataPtr[j].headers[i+headerOffset].u.large_anim.position -= (uint8)(FromPosition[j]);
                break;

              case WMS_UDH_SMALL_ANIM:
                splitUserDataPtr[j].headers[i+headerOffset].u.small_anim.position -= (uint8)(FromPosition[j]);
                break;

              case WMS_UDH_LARGE_PICTURE:
                splitUserDataPtr[j].headers[i+headerOffset].u.large_picture.position -= (uint8)(FromPosition[j]);
                break;

              case WMS_UDH_SMALL_PICTURE:
                splitUserDataPtr[j].headers[i+headerOffset].u.small_picture.position -= (uint8)(FromPosition[j]);
                break;

              case WMS_UDH_VAR_PICTURE:
                splitUserDataPtr[j].headers[i+headerOffset].u.var_picture.position -= (uint8)(FromPosition[j]);
                break;

              default:
                break;
            }
            break;
          }
          k++;
        }
      }

      splitUserDataPtr[j].num_chars = (uint16)(ToPosition[j] - (FromPosition[j] + initialOffset));
      splitUserDataPtr[j].encoding = origUserData->encoding;

      /* Assign Data if Any */
      if (splitUserDataPtr[j].num_chars > 0)
      {
        switch(origUserData->encoding)
        {
          case WMSAPP_ENCODING_7_BIT:
          case WMSAPP_ENCODING_8_BIT:
            splitUserDataPtr[j].data = MALLOC(splitUserDataPtr[j].num_chars);
            if (isRFC822HeaderPresent)
            {
              (void) MEMCPY(splitUserDataPtr[j].data, origUserData->data, initialOffset);
            }
            (void) MEMCPY(splitUserDataPtr[j].data + initialOffset, origUserData->data + initialOffset + FromPosition[j], splitUserDataPtr[j].num_chars - initialOffset);
            break;

          case WMSAPP_ENCODING_16_BIT:
            splitUserDataPtr[j].data = MALLOC(splitUserDataPtr[j].num_chars*2);
            if (isRFC822HeaderPresent)
            {
              (void) MEMCPY(splitUserDataPtr[j].data, origUserData->data, initialOffset*2);
            }
            (void) MEMCPY(splitUserDataPtr[j].data + 2*initialOffset, origUserData->data + 2*(initialOffset + FromPosition[j]), 2*(splitUserDataPtr[j].num_chars - initialOffset));
            break;
        }
      }
    }
    /* Update Message Reference */
    MsgRef++;
    (void)ICONFIG_SetItem(pMe->m_pIConfig, CFGI_SMS_SEGMENT_REF_NUM, &MsgRef, 2);

    *pSplitUserDataPtr = splitUserDataPtr;
  }

  /* FREE UP used Memory */
  WMSAPPU_FREE(origHeaderSegment);
  WMSAPPU_FREE(origHeaderPosition);

  WMSAPPU_FREE(numHeaders);
  WMSAPPU_FREE(FromPosition);
  WMSAPPU_FREE(ToPosition);

#endif /* FEATURE_SMS_UDH */
  /* Return Value; TRUE or FALSE */
  return retVal;
}

/*===========================================================================
FUNCTION WMSAPPU_ConcatUserData

DESCRIPTION
  Concatenates the Segmented User Data to a single User Data.
  ** This function allocates memory for the HugeMessage.
     It is upto the Calling function to FREE the Memory.

ASSUMPTIONS:
  splitUserDataPtr has all the segments contiguously sorted from
  segment #1 to the last segment
  All segments have same encoding.

DEPENDENCIES
  None

SIDE EFFECTS
  None

RETURN VALUE
  TRUE: Successful Concatenation
  FALSE: Concatenation Failed
===========================================================================*/
boolean WMSAPPU_ConcatUserData
(
  const CWMSAPP                   * pMe,              /* INPUT */
  const uint8                       numSegments,      /* INPUT */
  const wmsapp_user_data_s_type   * splitUserDataPtr, /* INPUT */
  wmsapp_user_data_s_type   ** pHugeUserDataPtr /* OUTPUT */
)
{
  boolean retVal = TRUE;

#ifdef FEATURE_SMS_UDH 
  uint8 i,j;

  uint16 messageRef=0;
  boolean use8BitSegmentation = TRUE;
  wmsapp_encoding_e_type encoding;

  uint8 numHeaders=0;
  uint8 curHeader=0;

  uint16 numChars=0;
  uint16 charPosition=0;

  boolean isRFC822HeaderPresent = FALSE;
  boolean foundConcat = FALSE;
  boolean foundRFC822 = FALSE;

  uint8 RFC822HeaderIndex=0;
  uint8 RFC822Offset=0;

  wmsapp_user_data_s_type *HugeUserDataPtr;

  if (numSegments <= 1)
  {
    MSG_ERROR("Concatentation: Invalid numSegments",0,0,0);
    return FALSE;
  }

  for (i=0 ; i<splitUserDataPtr[0].num_headers ; i++)
  {
    /* Extracting Data from First Segment */
    if (splitUserDataPtr[0].headers[i].header_id == WMS_UDH_CONCAT_8)
    {
      foundConcat = TRUE;
      use8BitSegmentation = TRUE;
      messageRef = splitUserDataPtr[0].headers[i].u.concat_8.msg_ref;
    }
    else if (splitUserDataPtr[0].headers[i].header_id == WMS_UDH_CONCAT_16)
    {
      foundConcat = TRUE;
      use8BitSegmentation = FALSE;
      messageRef = splitUserDataPtr[0].headers[i].u.concat_16.msg_ref;
    }
    else if(splitUserDataPtr[0].headers[i].header_id == WMS_UDH_RFC822)
    {
      isRFC822HeaderPresent = TRUE;
      RFC822HeaderIndex = i;
      RFC822Offset = splitUserDataPtr[0].headers[i].u.rfc822.header_length;
    }
  }

  if (foundConcat == FALSE)
  {
    MSG_ERROR("Concatentation Invalid Format: Concat Header not present",0,0,0);
    return FALSE;
  }

  /* User Data Encoding */
  encoding = splitUserDataPtr[0].encoding;

  /* Do some validation based on First Segment*/
  for (i = 0; i < numSegments ; i++)
  {

    foundConcat = FALSE;
    foundRFC822 = FALSE;

    for (j=0 ; j<splitUserDataPtr[0].num_headers ; j++)
    {
      if (use8BitSegmentation == TRUE)
      {
        /* 8-bit Segmentation */
        if (splitUserDataPtr[i].headers[j].header_id == WMS_UDH_CONCAT_8)
        {
          foundConcat = TRUE;

          /* Check for Message Reference */
          if (splitUserDataPtr[i].headers[j].u.concat_8.msg_ref != messageRef)
          {
            MSG_ERROR("Concatentation Invalid Format: Message Reference not same",0,0,0);
            return FALSE;
          }

          /* Check for Message Number */
          if (splitUserDataPtr[i].headers[j].u.concat_8.seq_num != (i+1))
          {
            MSG_ERROR("Concatentation Invalid Format: Message Sequence not same",0,0,0);
            return FALSE;
          }

          /* Check for NumSegments */
          if (splitUserDataPtr[i].headers[j].u.concat_8.total_sm != numSegments)
          {
            MSG_ERROR("Concatentation Invalid Format: Total Segments not same",0,0,0);
            return FALSE;
          }
        }
      }
      else
      {
        /* 16-bit Segmentation */
        if (splitUserDataPtr[i].headers[j].header_id == WMS_UDH_CONCAT_16)
        {
          foundConcat = TRUE;

          /* Check for Message Reference */
          if (splitUserDataPtr[i].headers[j].u.concat_16.msg_ref != messageRef)
          {
            MSG_ERROR("Concatentation Invalid Format: Message Reference not same",0,0,0);
            return FALSE;
          }

          /* Check for Message Number */
          if (splitUserDataPtr[i].headers[j].u.concat_16.seq_num != (i+1))
          {
            MSG_ERROR("Concatentation Invalid Format: Message Sequence not same",0,0,0);
            return FALSE;
          }

          /* Check for NumSegments */
          if (splitUserDataPtr[i].headers[j].u.concat_16.total_sm != numSegments)
          {
            MSG_ERROR("Concatentation Invalid Format: Total Segments not same",0,0,0);
            return FALSE;
          }
        }
      }

      /* RFC822 header */
      if (splitUserDataPtr[i].headers[j].header_id == WMS_UDH_RFC822)
      {
        foundRFC822 = TRUE;
        if (isRFC822HeaderPresent)
        {
          if (RFC822Offset != splitUserDataPtr[i].headers[j].u.rfc822.header_length)
          {
            MSG_ERROR("Concatentation Invalid Format: RFC Length not consistent",0,0,0);
            return FALSE;
          }
        }
        else
        {
          MSG_ERROR("Concatentation Invalid Format: RFC Header not present in all segments",0,0,0);
          return FALSE;
        }
      }
    }
    if (foundConcat == FALSE)
    {
      MSG_ERROR("Concatentation Invalid Format: Concat Header not present",0,0,0);
      return FALSE;
    }

    if (isRFC822HeaderPresent)
    {
      if (foundRFC822 == FALSE)
      {
        MSG_ERROR("Concatentation Invalid Format: RFC Header not present in all segments",0,0,0);
        return FALSE;
      }
    }

    /* Check for encoding */
    if (splitUserDataPtr[i].encoding != encoding)
    {
      MSG_ERROR("Concatentation Invalid Format: Encoding not same",0,0,0);
      return FALSE;
    }

    /* Update numHeaders and numChars */
    numHeaders += splitUserDataPtr[i].num_headers - 1; /* -1 to remove Concat Header */
    numChars   += splitUserDataPtr[i].num_chars;

    if (foundRFC822)
    {
      numHeaders -= 1; /* -1 to remove RFC822 Header */
      numChars -= RFC822Offset;
    }
  }

  if (isRFC822HeaderPresent)
  {
    /* Adding RFC822 Data only once for all segments */
    numHeaders += 1;
    numChars += RFC822Offset;
  }

  /* Segments have valid Data */

  /* Allocate Memory for HugeDataPtr */
  HugeUserDataPtr = MALLOC(sizeof(wmsapp_user_data_s_type));
  HugeUserDataPtr->encoding = encoding;

  switch (encoding)
  {
    case WMSAPP_ENCODING_7_BIT:
    case WMSAPP_ENCODING_8_BIT:
      HugeUserDataPtr->data = MALLOC(numChars);
      break;

    case WMSAPP_ENCODING_16_BIT:
      HugeUserDataPtr->data = MALLOC(numChars*2);
      break;
  }

  HugeUserDataPtr->num_chars = numChars;
  HugeUserDataPtr->num_headers = numHeaders;
  HugeUserDataPtr->headers = MALLOC(sizeof(wms_udh_s_type) * numHeaders);

  if (isRFC822HeaderPresent)
  {
    HugeUserDataPtr->headers[curHeader] = splitUserDataPtr[0].headers[RFC822HeaderIndex];
    curHeader++;
    switch(encoding)
    {
      case WMSAPP_ENCODING_7_BIT:
      case WMSAPP_ENCODING_8_BIT:
        (void) MEMCPY(HugeUserDataPtr->data, splitUserDataPtr[0].data, RFC822Offset);
        break;

      case WMSAPP_ENCODING_16_BIT:
        (void) MEMCPY(HugeUserDataPtr->data, splitUserDataPtr[i].data, RFC822Offset*2);
        break;
    }
  }

  /* Combine all the segments */
  for (i = 0; i < numSegments ; i++)
  {
    /* Copy the Headers */
    for (j=0; j < splitUserDataPtr[i].num_headers; j++)
    {
      /* Adjust for position */
      switch(splitUserDataPtr[i].headers[j].header_id)
      {
        case WMS_UDH_SPECIAL_SM:
        case WMS_UDH_PORT_8:
        case WMS_UDH_PORT_16:
        case WMS_UDH_SMSC_CONTROL:
        case WMS_UDH_SOURCE:
        case WMS_UDH_WCMP:
          HugeUserDataPtr->headers[curHeader] = splitUserDataPtr[i].headers[j];
          curHeader++;
          break;

        /* Position Dependent headers */
        case WMS_UDH_TEXT_FORMATING:
          HugeUserDataPtr->headers[curHeader] = splitUserDataPtr[i].headers[j];
          HugeUserDataPtr->headers[curHeader].u.text_formating.start_position += (uint8)(charPosition);
          curHeader++;
          break;

        case WMS_UDH_PRE_DEF_SOUND:
          HugeUserDataPtr->headers[curHeader] = splitUserDataPtr[i].headers[j];
          HugeUserDataPtr->headers[curHeader].u.pre_def_sound.position += (uint8)(charPosition);
          curHeader++;
          break;

        case WMS_UDH_USER_DEF_SOUND:
          HugeUserDataPtr->headers[curHeader] = splitUserDataPtr[i].headers[j];
          HugeUserDataPtr->headers[curHeader].u.user_def_sound.position += (uint8)(charPosition);
          curHeader++;
          break;

        case WMS_UDH_PRE_DEF_ANIM:
          HugeUserDataPtr->headers[curHeader] = splitUserDataPtr[i].headers[j];
          HugeUserDataPtr->headers[curHeader].u.pre_def_anim.position += (uint8)(charPosition);
          curHeader++;
          break;

        case WMS_UDH_LARGE_ANIM:
          HugeUserDataPtr->headers[curHeader] = splitUserDataPtr[i].headers[j];
          HugeUserDataPtr->headers[curHeader].u.large_anim.position += (uint8)(charPosition);
          curHeader++;
          break;

        case WMS_UDH_SMALL_ANIM:
          HugeUserDataPtr->headers[curHeader] = splitUserDataPtr[i].headers[j];
          HugeUserDataPtr->headers[curHeader].u.small_anim.position += (uint8)(charPosition);
          curHeader++;
          break;

        case WMS_UDH_LARGE_PICTURE:
          HugeUserDataPtr->headers[curHeader] = splitUserDataPtr[i].headers[j];
          HugeUserDataPtr->headers[curHeader].u.large_picture.position += (uint8)(charPosition);
          curHeader++;
          break;

        case WMS_UDH_SMALL_PICTURE:
          HugeUserDataPtr->headers[curHeader] = splitUserDataPtr[i].headers[j];
          HugeUserDataPtr->headers[curHeader].u.small_picture.position += (uint8)(charPosition);
          curHeader++;
          break;

        case WMS_UDH_VAR_PICTURE:
          HugeUserDataPtr->headers[curHeader] = splitUserDataPtr[i].headers[j];
          HugeUserDataPtr->headers[curHeader].u.var_picture.position += (uint8)(charPosition);
          curHeader++;
          break;

        default:
          break;
      }
    }

    /* Copy the Data */
    switch(encoding)
    {
      case WMSAPP_ENCODING_7_BIT:
      case WMSAPP_ENCODING_8_BIT:
        (void) MEMCPY(HugeUserDataPtr->data + charPosition +RFC822Offset, splitUserDataPtr[i].data + RFC822Offset, splitUserDataPtr[i].num_chars - RFC822Offset);
        break;

      case WMSAPP_ENCODING_16_BIT:
        (void) MEMCPY(HugeUserDataPtr->data + (charPosition+RFC822Offset)*2, splitUserDataPtr[i].data + RFC822Offset*2, (splitUserDataPtr[i].num_chars-RFC822Offset)*2);
        break;
    }

    /* Update charPosition */
    charPosition += splitUserDataPtr[i].num_chars - RFC822Offset;

  }
  *pHugeUserDataPtr = HugeUserDataPtr;
#endif /* FEATURE_SMS_UDH */
  return retVal;
}

/*===========================================================================

FUNCTION: WMSAPPU_IsMsgActive

DESCRIPTION
   Checks if there are any new messages

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPU_IsMsgActive
(
  const CWMSAPP *pMe
)
{
  uint8 i, j;

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

  for( i=0; i<(uint8) WMS_MEMORY_STORE_MAX; i++ )
  {
    for( j=0; j<(uint8)pMe->m_pSmsLists[i]->len; j++ )
    {
      if( pMe->m_pSmsLists[i]->tags[j] == WMS_TAG_MT_NOT_READ )
      {
        if( i == (uint8)WMS_MEMORY_STORE_SIM &&
            pMe->m_pSmsLists[i]->indices[j] == pMe->m_pSmsLists[i]->voice_mail_index )
        {
          /* if the msg is a voice mail, don't display msg icon for it */
          continue;
        }
        else if( i == (uint8) WMS_MEMORY_STORE_NV_CDMA &&
                 pMe->m_pSmsLists[i]->indices[j] ==
                 pMe->m_pSmsLists[i]->voice_mail_index )
        {
          /* if the msg is a voice mail, don't display msg icon for it */
          continue;
        }
        else
        {
          return TRUE;
        }
      }
    }
  }

  return FALSE;

} /* WMSAPPU_IsMsgActive() */

/* <EJECT> */
/*===========================================================================
FUNCTION WMSAPPU_UPDATEICONS

DESCRIPTION
  Updates the envelope icon to reflect the current status.
  For text messages : New urgent messages are reflected with a blinking
  envelope icon.  New non urgent messages are reflected with a solid on
  envelope icon.  No new messages is reflected by turning off the envelope
  icon.

  For Voice Mail : Urgent voice mail is reflected with a blinking envelope  icon.
  Non urgent voice mail is reflected with a solid on envelope icon. No voice
  mail ( no message or a count of 0 ) is reflected by turning the envelope icon
  off.

DEPENDENCIES
  pMe->m_smslist must be current
  WMSAPPU_IsMsgActive

SIDE EFFECTS
 None

===========================================================================*/
void WMSAPPU_UpdateIcons
(
  const CWMSAPP *pMe
)
{
  if (pMe->m_pSmsLists[WMS_MEMORY_STORE_SIM]->voice_mail_active ||
      pMe->m_pSmsLists[WMS_MEMORY_STORE_NV_CDMA]->voice_mail_active   
#ifdef FEATURE_GWSMS      
#error code not present
#endif /* FEATURE_GWSMS */      
  )
#ifdef FEATURE_ANNUNCIATOR
  {
    (void)IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_MSG, ANNUN_STATE_ON);
  }
  else if ( WMSAPPU_IsMsgActive(pMe) )
  {
    /* Turn on envelope icon */
    (void)IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_MSG, ANNUN_STATE_ON);
  }
  else
  {
    /* Turn off envelope icon */
    (void)IANNUNCIATOR_SetField(pMe->m_pIAnn, ANNUN_FIELD_MSG, ANNUN_STATE_OFF);
  }
#endif /* FEATURE_ANNUNCIATOR */

  /* done */
  return;
} /* WMSAPPU_UpdateIcons() */

#if defined(FEATURE_GWSMS_BROADCAST) && defined(FEATURE_IWMSDIAG)
#error code not present
#endif /* defined(FEATURE_GWSMS_BROADCAST) && defined(FEATURE_IWMSDIAG) */

/*===========================================================================
FUNCTION: WMSAPPU_LoadImageFromBuffer

DESCRIPTION
   Loads the IImage Pointer from an Image Buffer Provided

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
IImage *WMSAPPU_LoadImageFromBuffer
(
  const CWMSAPP *pMe,
  void    *pBuff, 
  uint32   dwSize
)
{
  IImage      *pIImage = NULL;
  IMemAStream *pMemStream = NULL;
  AEECLSID     cls;

  /* Check for Null Input Parameters */
  if ((pMe == NULL) || (pBuff == NULL))
  {
    return NULL;
  }

  /* To get the Class ID of IImage */
  cls = ISHELL_GetHandler(pMe->a.m_pIShell, HTYPE_VIEWER, "image/bmp"); 

  if (cls)
  {
    (void)ISHELL_CreateInstance(pMe->a.m_pIShell, cls, (void **)(&pIImage));
    
    if (pIImage == NULL)
    {
      /* Failed! */
      MSG_ERROR("Failed to Created IImage", 0, 0, 0);
      return NULL;
    }
  }
  else 
  {
    MSG_ERROR("Cannot find IImage Class ID", 0, 0, 0);
    return NULL;
  }

  /* Create MEMSTREAM and use it as stream for IImage */
  (void) ISHELL_CreateInstance (pMe->a.m_pIShell, AEECLSID_MEMASTREAM, (void**)(&pMemStream));
  
  if (pMemStream == NULL)
  {
    /* Failed! */
    MSG_ERROR("Failed to Created MEMASTREAM", 0, 0, 0);
    (void)IIMAGE_Release( pIImage );
    return NULL;
  }

  IMEMASTREAM_Set(pMemStream, pBuff, dwSize, 0, 0);
  IIMAGE_SetStream(pIImage, (IAStream*)pMemStream);
  (void)IMEMASTREAM_Release(pMemStream);
  
  return pIImage;
}

/*===========================================================================
FUNCTION: WMSAPPU_MEMCPY

DESCRIPTION
   Copies Memory from Input to Output

DEPENDENCIES
  none

RETURN VALUE
  TRUE / FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPU_MEMCPY
(
  const void *input, 
  uint32 input_size, 
  void *output,
  uint32 output_size
)
{
  if ((input == NULL)||(output == NULL))
  {
    MSG_ERROR("Null Pointer!!!",0,0,0);
    return FALSE;
  }

  if (input_size > output_size)
  {
    MSG_ERROR("MEMCPY unsuccessful, input buffer size greater than output size",0,0,0);
    return FALSE;
  }
  else
  {
    (void) MEMCPY(output, input, input_size);
    return TRUE;
  }
}

/*===========================================================================
FUNCTION: WMSAPPU_MALLOC

DESCRIPTION
   Allocates Memory

DEPENDENCIES
  none

RETURN VALUE
  TRUE / FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPU_MALLOC
(
  void **ptr, 
  uint32 size
)
{
  boolean retVal = FALSE;
  if ((ptr != NULL) && (*ptr == NULL))
  {
    if ((*ptr = MALLOC(size)) != NULL)
    {
      retVal = TRUE;
    }
  }
  else
  {
    MSG_ERROR("Cannot Allocate Memory since Pointer is not Null", 0, 0, 0);
  }
  return retVal;
}


#endif /*FEATURE_WMS_APP*/

/*========================End of File ================================================*/
