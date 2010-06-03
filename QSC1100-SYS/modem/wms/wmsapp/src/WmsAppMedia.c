/*=============================================================================

FILE: WMSAppMedia.C

    U S E R   I N T E R F A C E   S M S   D I S P L A Y   S E R V I C E

GENERAL DESCRIPTION
  Contains the types and items required for controlling and displaying
  SMS menus and screens.



(c) COPYRIGHT 2002, 2003, 2004, 2005, 2006,2007 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/wms/wmsapp/main/latest/src/WmsAppMedia.c#12 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/29/08   HN      Fix Klocwork warnings by updating code to use new WMSAPPU_ITOA()
07/14/08   HN      Added check for 'offset' buffer overflow in WMSAPPMD_DisplayTemplate()
06/27/08   PMD     Cleaned up Feature Flags in Code
06/23/08   HN      Fixed Lint & Klocwork warnings
06/04/08   PMD     Removed support for Auto / Transparent SMS 
04/10/08   PMD     Fixed compile error
04/09/08   SM/PMD  Fixed Potential Buffer overruns; Incorrect imageBuf Memset
02/07/08   PMD     Replaced Deprecated Function Calls with newer equivalents
01/31/08   PMD     Updated Copyright Information
11/14/07   PMD     Fixed Viewer / Composer size params for Higher Resolution
10/16/07   PMD     Allocate memory on heap for msg in WMSAPPMD_GWDisplay()
10/02/07   PMD     Fixed incorrect computation in WMSAPPMD_DisplayText()
03/07/07   PMD     Fixed compile warning for GSM / WCDMA
02/15/07   HQ      Fixed compiler warnings related to SMS_UDH.
02/08/07   PMD     Fixed Incorrect Status Report Usage Display
01/17/07   HQ      Mainlined 637A.
11/20/06   PMD     Appended 1 to length for Null Character in wms_ts_unpack_ascii
11/17/06   PMD     Initialized other in client_bd structure to 0
10/31/06   PMD     Fixed Compile Warnings on 7k Targets
04/21/06   HQ      Added support for CDMA Read Ack message.
02/09/06   PMD     Fixed Include File wmsapp.brh case
02/06/06   PMD     Fixed Compile Warnings
01/25/06   VK      Updated Copyright Info
01/25/06   VK      Lint fixes
12/20/05   PMD     Added support for WmsApp.brh File
11/20/05   PMD     Fixed potential memory corruption in GotoNextLine()
10/31/05   PMD     Fix to check dataLength before Displaying Message.
10/20/05   PMD     Mainlined FEATURE_SMS_TEST.
10/19/05   VK      EMS Featurization fixes
09/13/05   PMD     Fixed display of Memory Usage for Increased Memory.
05/10/05   VK      Compiling Error
05/09/05   VK      Deleting EMS + text fix
04/19/05   PMD     Fixed Double Freeing of ImageBuf
04/12/05   VK      Fix in Displaying Large Picture
04/11/05   Rex     Fixed cdma unicode display
03/10/05   PMD     Added Check for DispInfo.curLine to prevent Memory Corruption
02/20/05   PMD     Added 1x Broadcast Support in Multimode Broadcast
02/15/05   Rex     Fixed crticial lint warning
01/17/05   PMD     Replaced FREEIF calls with WMSAPPU_FREE
01/11/05   PMP     Fixed Display Issue and Compile Warning.
01/07/05   PMD     Added and used WMSAPPMD_CheckCMXPlayAllowed() function.
11/11/04   VK      Added the unicode functionality
10/16/04   PMD     Removed Compiler Warning.
10/16/04   PMD     Using WMSAPPU_LoadImageFromBuffer function rather than File.
10/13/04   PMD     Changed all filenames to Lower Case. Check if File Present.
                   Fixed Freeing of Static Buffer
                   Released IIMage Pointer after use.
10/13/04   PMD     Fixed a compile error with CDSMS Feature
10/12/04   Rex     Remove function WMSAPPMD_DisplayExtendedObject 
10/12/04   PMD     Fixed Memory Problems in WMSAPPMD_DisplayTemplate().
09/29/04   PMD     Short Return if IFILEMGR Create Instance Fails
09/29/04   PMD     Replaced FREE with FREEIF to ensure we do not free Null Pointers
09/01/04   Rex     Added WMSAPPMD_DisplayExtendedObject
07/07/04   PMD     Changed Function Prototype - WMSAPPMD_ConvertPicToEMS()
06/22/04   Rex     Clean up potential occurrences of memory leaks
04/08/04   PMD     Fix for early termination of CB Messages on detection of 2 '\r'
04/05/04   Rex     Fixed end of text junk character display for GW CB normal msg
03/31/04   Rex     Updating the Annunciator on Reading a Broadcast Message
03/16/04   AP      Added support for the user app
02/14/04   PMD     Marked the Broadcast Message as Read while displaying it.
02/02/04   ak      packing/unpacking problem fixed.
01/26/04   PMD     Added Concat Info in CDMA and GW Message Info Display
01/26/04   PMD     Added Timestamp in GW Message Info Display.
01/12/04   ak      Copy the unpacked message back to pMe data structures
12/22/03   PMD     Fixed Extra Lines in CDMAMessageInfo
12/22/03   PMD     Featurized Status Report Usage Display
12/03/03   PMD     Lint related changes for Saber
12/03/03   PMD     Fixed Display of three digits in MWI
11/21/03   PMD     Added GSM Cell Broadcast Support
11/13/03   PMD     Fixed Display of Message with GW Email
11/08/03   PMD     Changed width to screenSize in WMSAPPMD_DisplayText
11/04/03   PMD     Fixed Display of CDMA Message Info
10/22/03   PMD     Replaced wms_ts_* by IWMS_Ts* (Transport Service Functions)
10/02/03   PMD     Fixed Incorrect Display of Voicemail in both Message
                   Info Display and Message Display for GW
10/01/03   PMD     Changes to Support more than 10 predefined Animations
09/23/03   PMD     Changed User Data in IWMS functions to be a pointer to pMe
08/15/03   PMD     Added Display of Status Report Usage
07/29/03   AP      Modified included files to generate the correct dependencies.
07/14/03   PMD     Changes to remove / suppress Lint Errors
07/08/03   AP      Fixed feature-ization to display CDMA messages correctly.
06/26/03   SUN     Added support for 160 characters
06/11/03   AP      Fixed compile warnings.
06/10/03   PMD     Added sanity check in DisplayTextFormatting
06/05/03   SUN     Added support for FEATURE_GWSMS_STATUS_REPORTS
05/27/03   PMD     Memory Checks in PlayPreDefSound
                   Added functions PlayUserDefSound, PlayMelody,
                   ConvertUserMelodyToEMS and DisplayPreDefAnimation
05/22/03   SUN     Fixed Display with Style Text
05/20/03   SUN     Changed structs to pointers in pMe
05/19/03   PMD     Added Melody Icon on Screen on IMelody Header
05/14/03   PMD     Added support for IMelody Sound Number to range from 0-255.
                   Changed Imelody to get position from pre_def structure
05/14/03   PMD     Fixed File Number in PlayPreDefSound
05/12/03   PMD     Fixed Test Menu Message Information
05/10/03   HQ      Made pText array static;
                   use WMSAPP_OTHER_INFO_TEXT_LENGTH when defining pText.
05/09/03   SUN     Added support to display the class summary,
                   fixed display of style text, fixed erase
                   code cleanup, code review changes
05/02/03   AP      Fixed display of other info for CDMA outbox options.
05/01/03   AP      Display message text correctly if encoding is Unicode.
04/08/03   SUN     Fixed Template Index Display
04/03/03   SUN/AP  Clean up.
03/17/03   AP      Fixed compile warnings.
03/10/03   sun     Checked for the User data Mask before displaying
                   Added support for Memory Usage
03/04/03   AP      Fix displays.
02/27/03   AP      Clean up.
10/31/02   sun     Initial Version
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
#include "msg.h"
#include "wms.h"
#include "AEEImage.h"

#include "WmsappMedia.h"
#include "WmsApp.h"
#include "WmsAppUtils.h"
#include "wmsapp.brh"
#include "bit.h"

#ifdef FEATURE_IMELODY
#include "cmx.h"
#endif /*FEATURE_IMELODY*/

typedef struct mod_ems
{
  int pos;
  int index;
  struct mod_ems *next;
}ems;




typedef struct
{

  uint16    bfType;      /*Specifies the file type, must be BM. */
  uint16    bfSize;      /*Specifies the size, in bytes, of the bitmap file. */
  uint16    bfSize2;     /*Reserved; must be zero. */
  uint16    bfReserved1; /*Reserved; must be zero. */
  uint16    bfReserved2;
  uint16    bfOffBits;   /*Specifies the offset, in bytes, from the beginning of the */
  uint16    bfOffbits2;  /*  BITMAPFILEHEADER structure to the bitmap bits. */

} BITMAPFILEHEADER;



typedef struct tagBITMAPINFOHEADER
{

  dword  biSize;          /*Number of bytes required by the structure. */
  dword  biWidth;         /*Width of the bitmap, in pixels. */
  int32  biHeight;        /*Height of the bitmap, in pixels. If biHeight is positive, the bitmap is a bottom-up DIB and its origin is the lower-left corner. If biHeight is negative, the bitmap is a top-down DIB and its origin is the upper-left corner.
                            If biHeight is negative, indicating a top-down DIB, */
  word   biPlanes;        /*number of planes for the target device. This value must be set to 1. */
  word   biBitCount;      /*bits-per-pixel. */
  dword  biCompression;   /*Type of compression for a compressed bottom-up bitmap (top-down DIBs cannot be compressed)*/
  dword  biSizeImage;     /*size, in bytes, of the image*/
  dword  biXPelsPerMeter; /*horizontal resolution*/
  dword  biYPelsPerMeter; /*Vertical Resolution*/
  dword  biClrUsed;       /*Number of color indexes in the color table that are actually used by the bitmap*/
  dword  biClrImportant;  /*Number of color indexes that are required for displaying the bitmap*/
} BITMAPINFOHEADER;


typedef struct tagRGBQUAD
{
  byte    rgbBlue;
  byte    rgbGreen;
  byte    rgbRed;
  byte    rgbReserved;
} RGBQUAD;


typedef struct tagBITMAPINFO
{
  BITMAPINFOHEADER    bmiHeader;
  RGBQUAD             bmiColors[2];
} BITMAPINFO;

#define IMAGE_SIZE 1000
#define LINE_SPACING 30
#define FRAME_WIDTH 20
#define SMALL_PIC_SIZE 16
#define LARGE_PIC_SIZE 32
#define SMALL_ANIM_SIZE 8
#define LARGE_ANIM_SIZE 16

boolean WMSAPPMD_CDMADisplay(CWMSAPP *pMe);

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
#ifdef FEATURE_SMS_UDH

/*===========================================================================
FUNCTION WMSAPPMD_SwitchHeaders

DESCRIPTION
  Provides linked list handling. The ems Headers are sorted based on their
  position in the user data

RETURN
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  Modifies the linked list directly.
===========================================================================*/
void WMSAPPMD_SwitchHeaders
(
int pos,
int index,
ems **ems_head
)
{

  ems *ems_ptr;
  ems *ems_trav;


  if(*ems_head ==NULL) /*First Index into the arrays*/
  {
    ems_ptr = (ems*)MALLOC(sizeof(ems));

    if(ems_ptr == NULL)
    {
      MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
      return;
    }

    ems_ptr->pos    = pos;
    ems_ptr->index  = index;
    ems_ptr->next   = NULL;
    *ems_head = ems_ptr; /*Make it the head of the linked list*/
    return;
  }
  /*Change the head of the linked list*/
  if(pos < (*ems_head)->pos)
  {
    ems_ptr = (ems*)MALLOC(sizeof(ems));
    if(ems_ptr == NULL)
    {
      MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
      return;
    }

    ems_ptr->next = *ems_head;
    ems_ptr->pos  = pos;
    ems_ptr->index= index;

    *ems_head = ems_ptr;
    return;
  }

  ems_trav =*ems_head;
  /*End of the list*/
  while(ems_trav->next != NULL)
  {
    if((pos > ems_trav->pos)
       && (pos < ems_trav->next->pos))
    {
      ems_ptr = (ems*)MALLOC(sizeof(ems));

      if(ems_ptr == NULL)
      {
        MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
        return;
      }

      ems_ptr->pos   = pos;
      ems_ptr->index = index;

      ems_ptr->next = ems_trav->next;
      ems_trav->next = ems_ptr;
      return;
    }
    else if(pos == ems_trav->pos)
    {
      ems_ptr = (ems*)MALLOC(sizeof(ems));
      if(ems_ptr == NULL)
      {
        MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
        return;
      }

      ems_ptr->pos   = pos;
      ems_ptr->index = index;
      ems_ptr->next  = ems_trav->next;

      ems_trav->next = ems_ptr;
      return;
    }
    else
    {
      ems_trav = ems_trav->next;
    }
  }

  /*End of the Linked List*/
  if(pos >= ems_trav->pos && ems_trav->next==NULL)
  {
    ems_ptr = (ems*)MALLOC(sizeof(ems));

    if(ems_ptr == NULL)
    {
      MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
      return;
    }

    ems_ptr->pos   = pos;
    ems_ptr->index = index;
    ems_ptr->next  = NULL;

    ems_trav->next = ems_ptr;
    return;
  }
}

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
/*===========================================================================
FUNCTION WMSAPPMD_SortCDMAHeaders

DESCRIPTION
  Headers are sorted in the following order
  1. Email
  2. EMS Headers
  3. Special SM
  EMS Headers are sorted on the basis of the position that they hold in the
  user data.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void WMSAPPMD_SortCDMAHeaders
(
 wms_cdma_user_data_s_type *user_data,
 ems **emsHead
)
{
  int i;
  int index_non_ems_headers = 0;

  /* Traverse through all of the headers */
  for(i=0;i<user_data->num_headers;i++)
  {
    switch(user_data->headers[i].header_id)
    {
      case WMS_UDH_TEXT_FORMATING:
        WMSAPPMD_SwitchHeaders(user_data->headers[i].u.text_formating.start_position,
                               i,&(*emsHead));
        break;

      case WMS_UDH_PRE_DEF_SOUND:
        WMSAPPMD_SwitchHeaders(user_data->headers[i].u.pre_def_sound.position,
                               i,&(*emsHead));
        break;

      case WMS_UDH_USER_DEF_SOUND:
        WMSAPPMD_SwitchHeaders(user_data->headers[i].u.user_def_sound.position,
                               i,&(*emsHead));
        break;

      case WMS_UDH_PRE_DEF_ANIM:
        WMSAPPMD_SwitchHeaders(user_data->headers[i].u.pre_def_anim.position,
                               i,&(*emsHead));
        break;

      case WMS_UDH_LARGE_ANIM:
        WMSAPPMD_SwitchHeaders(user_data->headers[i].u.large_anim.position,
                               i,&(*emsHead));
        break;

      case WMS_UDH_SMALL_ANIM:
        WMSAPPMD_SwitchHeaders(user_data->headers[i].u.small_anim.position,
                               i,&(*emsHead));
        break;

      case WMS_UDH_LARGE_PICTURE:
        WMSAPPMD_SwitchHeaders(user_data->headers[i].u.large_picture.position,
                               i,&(*emsHead));
        break;

      case WMS_UDH_SMALL_PICTURE:
        WMSAPPMD_SwitchHeaders(user_data->headers[i].u.small_picture.position,
                               i,&(*emsHead));
        break;

      case WMS_UDH_VAR_PICTURE:
        WMSAPPMD_SwitchHeaders(user_data->headers[i].u.var_picture.position,
                               i,&(*emsHead));
        break;

      default:
        WMSAPPMD_SwitchHeaders(index_non_ems_headers--, i, &(*emsHead));
        break;
    }
  }
}
#endif /* FEATURE_CDSMS */

#endif /*FEATURE_SMS_UDH */

/*===========================================================================
FUNCTION GoToPreviousLine

DESCRIPTION
  Moves the cursor to previous line

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void GoToPreviousLine
(
 CWMSAPP *pMe
)
{
  if (pMe->m_dispInfo.curLine ==0 ) // First Line
  {
    pMe->m_dispInfo.curX =START_POSITION;
  }
  else
  {
    /*Go to previous line*/
    pMe->m_dispInfo.curLine--;

    /*The new X is the previous maxX */
    pMe->m_dispInfo.curX = pMe->m_dispInfo.maxX[pMe->m_dispInfo.curLine];

    /*The new Y is the previous maxY */
    pMe->m_dispInfo.curY = pMe->m_dispInfo.maxY[pMe->m_dispInfo.curLine];
  }

  return;
}

/*===========================================================================
FUNCTION GoToNextLine

DESCRIPTION
  Moves the cursor to the next line

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void GotoNextLine
(
CWMSAPP *pMe
)
{
  if(pMe == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return;
  }

  /*Set the Max X for the Current Line*/
  pMe->m_dispInfo.maxX[pMe->m_dispInfo.curLine] = pMe->m_dispInfo.curX;

  /*Set the current X*/
  pMe->m_dispInfo.curX  = START_POSITION;

  /*if the max Y is greater than the previous line + line spacing, then we know the exact Y for the next line*/
  if(pMe->m_dispInfo.maxY[pMe->m_dispInfo.curLine] > (pMe->m_dispInfo.curY+LINE_SPACING))
  {
     pMe->m_dispInfo.curY = pMe->m_dispInfo.maxY[pMe->m_dispInfo.curLine];
  }
  else /*Cur Y + Line spacing is grater than Max Y*/
  {
    pMe->m_dispInfo.curY +=  LINE_SPACING;
  }

  if (pMe->m_dispInfo.curLine < 20)
  {
    /*Increment the Line*/
    pMe->m_dispInfo.curLine++;
  }
  else
  {
    MSG_ERROR("DispInfo Curline exceeds 20, Memory Corruption", 0, 0, 0);
  }

  if (pMe->m_dispInfo.curLine < 20)
  {
    /*The new Max Y will be the same as the Current Y*/
    pMe->m_dispInfo.maxY[pMe->m_dispInfo.curLine] = pMe->m_dispInfo.curY;
  }
  else
  {
    MSG_ERROR("DispInfo Curline exceeds 20, Memory Corruption", 0, 0, 0);
  } 

  return;
}
/*===========================================================================
FUNCTION WMSAPPMD_DisplayText

DESCRIPTION
  Formats and displays data to the screen in the font provided.

RETURN
  Returns SUCCESS, EFAILED or EUNSUPPORTED.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean WMSAPPMD_DisplayText
(
CWMSAPP *pMe,
uint8 *data,
int len,
uint32 dwFlags,
AEEFont font,
AEERect* rect
)
{
  int rv= SUCCESS;
  int nChars[1];
  AECHAR *psContents;
  int nPixels;
  int pos=0;
  int width=0;
  int screenSize = pMe->m_cxScreen -FRAME_WIDTH;

  if(pMe == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return FALSE;
  }

  if(len <= 0)
    return TRUE;

  psContents = MALLOC((dword)((uint32)sizeof(AECHAR) * (uint32)(len+1)));
  if(psContents == NULL)
  {
    MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
    return FALSE;
  }

  if(pMe->m_modeControl == WMSAPP_MODE_GW)
  {
#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/
  }
  else
  {
    (void)STRTOWSTR((char *)data, psContents, ((int)sizeof(AECHAR) * (len+1)));
  }

  width = screenSize - pMe->m_dispInfo.curX;

  /* Only Left Aligned is Supported */

  for(;;)
  {

    /* Find out the Number of Chars that can be drawn in the given "width" = nChars[0]*/
    nPixels = IDISPLAY_MeasureTextEx(pMe->a.m_pIDisplay,
                                     font,
                                     psContents+pos,
                                     len,
                                     width,
                                     nChars);

    //Skip the White Spaces in the beginning of the line. This does the left aligning ofthe text!
    while(GETCHTYPE(psContents[pos]) == SC_WHITESPACE)
    {
      pos++;
      len--;
    }

    /*There are more characters to be displayed after this line. find out if a word is being broken in the middle*/
    if(nChars[0] != len)
    {
       /*if the next character after nChars[0] is not a space and the last character nChars[0] is
        also not a space, then it means that the word is being broken. Go to the beginning of the word*/
      while((GETCHTYPE(psContents[pos+nChars[0]]) != SC_WHITESPACE) &&
            (GETCHTYPE(psContents[pos+nChars[0]-1]) != SC_WHITESPACE) &&
            nChars[0] >0
           )
      {
        nChars[0] --;
      }


      if(nChars[0] <= 0)
      {
        /*could not find the beginning of the word or the word cannot fit on one line.
          display as it is . find nchars again*/
        nPixels = IDISPLAY_MeasureTextEx(pMe->a.m_pIDisplay,
                                         font,
                                         psContents+pos,
                                         len,
                                         width,
                                         nChars);
      }
    }

    // Draw the Text on the screen
    rv = IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                           font,
                           psContents+pos,
                           nChars[0],
                           pMe->m_dispInfo.curX,
                           pMe->m_dispInfo.curY,
                           NULL,
                           dwFlags);
    /*Find the number of characters left to be displayed*/
    len -= nChars[0];

    /*Update CurX*/
    pMe->m_dispInfo.curX += nPixels;

    IDISPLAY_Update(pMe->a.m_pIDisplay);

    /*If CurX is lamost at the end, go to the next line*/
    if(pMe->m_dispInfo.curX >= (screenSize-2*START_POSITION))
    {
      GotoNextLine(pMe);
      nPixels =0;
      width = screenSize;
    }

    /*No more characters left to display, exit*/
    if(len <= 0)
    {
      WMSAPPU_FREE(psContents);
      if(rv == SUCCESS)
        return TRUE;
      else
        return FALSE;
    }

    pos += nChars[0];

    dwFlags &= ~(IDF_RECT_MASK);   // already drawn

    /*Update the width*/
    width = width - nPixels;
  }
}


/*===========================================================================
FUNCTION WMSAPPMD_CalcEraseTextPixels

DESCRIPTION
  Calculates the number of pixels that need to be erased

RETURN
  Returns number of pixels to be erased

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static int WMSAPPMD_CalcEraseTextPixels
(
  CWMSAPP *pMe
)
{
  int nPixels;
  int dataLength = 0;
  AECHAR eraseData[2] = {0, 0};

  if(pMe == NULL)
  {
    MSG_HIGH("pMe = NULL",0,0,0);
    return FALSE;
  }

  /*find the total number of characters*/
  if (pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS
    dataLength = pMe->m_pClient_ts_mo->u.cdma.user_data.number_of_digits;
#endif /*FEATURE_CDSMS*/
  }
  else
  {
#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/
  }

  /*Nothing to erase*/
  if (dataLength == 0)
    return 0;


  /*One character at a time needs to be deleted*/
  if (pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
    if(pMe->m_bTestMode)
    {
#ifdef FEATURE_CDSMS
      (void)STRTOWSTR((char*)&pMe->m_pClient_ts_mo->u.cdma.user_data.data[dataLength-1],eraseData, sizeof(AECHAR) * (2));
#endif /*FEATURE_CDSMS*/
    }
  }
  else
  {
    if(pMe->m_bTestMode)
    {
#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/
    }
  }
  /*Calculate the number of pixels used by that character*/
  nPixels = IDISPLAY_MeasureText(pMe->a.m_pIDisplay,
                                   pMe->m_curFont,
                                   eraseData);
  /*Reduce the number length by 1*/
  if (pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS
    pMe->m_pClient_ts_mo->u.cdma.user_data.number_of_digits -= 1;
#endif /*FEATURE_CDSMS*/
  }
  else
  {
#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/
  }

  return nPixels;
}

/*===========================================================================
FUNCTION WMSAPPMD_EraseRegion

DESCRIPTION
  Erases Text/Image on the Compose Screen

RETURN
  NONE

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void WMSAPPMD_EraseRegion
(
  CWMSAPP *pMe,
  int nPixels,
  int height
)
{

  if(pMe == NULL)
  {
    MSG_HIGH("pMe = NULL",0,0,0);
    return;
  }

  if (pMe->m_dispInfo.curX <=START_POSITION && pMe->m_dispInfo.curLine == 0)
    return; // Nothing to Erase

  pMe->m_dispInfo.curX -= nPixels;

  /*Go to the end of the previous line*/
  if(pMe->m_dispInfo.curX < START_POSITION)
  {
    GoToPreviousLine(pMe);
    pMe->m_dispInfo.curX -= nPixels;
    height = pMe->m_dispInfo.curY;
  }
  IDISPLAY_EraseRgn(pMe->a.m_pIDisplay,
                    (int16)pMe->m_dispInfo.curX,
                    (int16)pMe->m_dispInfo.curY,
                    (uint16)nPixels,
                    (uint16)height);

  IDISPLAY_Update(pMe->a.m_pIDisplay);
  
}

/*===========================================================================
FUNCTION WMSAPPMD_DisplayWhileComposing

DESCRIPTION
  Display chars to the screen while composing.

RETURN
  SUCCESS, FAILED or EUNSUPPORTED

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMD_DisplayWhileComposing
(
  CWMSAPP *pMe,
  AECHAR *data,
  boolean newCharSet
)
{
  /*Here a char set implies => {a,b,c,1} {d,e,f,2} ...*/
  int rv= SUCCESS;
  int nChars[1];
  int nPixels;
  int screenSize = pMe->m_cxScreen -FRAME_WIDTH;

  if(pMe == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return FALSE;
  }

  /*The text could change. For eg, if we press "1" key 3 times, the text has to switch from a -> b->c-> 1
    if it switches a->b,a has to be erased and then b has to be displayed*/
  if (newCharSet == FALSE)
  {
    /*Find out how many pixels can be erased*/
    nPixels = WMSAPPMD_CalcEraseTextPixels(pMe);

    /*Erase the character*/
    if (nPixels != 0)
    {
      WMSAPPMD_EraseRegion(pMe, nPixels,pMe->m_dispInfo.maxY[pMe->m_dispInfo.curLine]);
    }
  }

  /*Display the new character*/
  nPixels = IDISPLAY_MeasureTextEx(pMe->a.m_pIDisplay,
                                   pMe->m_curFont,
                                   data,
                                   1,
                                   screenSize,
                                   nChars);

  rv = IDISPLAY_DrawText(pMe->a.m_pIDisplay,
                         pMe->m_curFont,
                         data,
                         nChars[0],
                         pMe->m_dispInfo.curX,
                         pMe->m_dispInfo.curY,
                         NULL,
                         pMe->m_curFlags);

  pMe->m_dispInfo.curX += nPixels;

  IDISPLAY_Update(pMe->a.m_pIDisplay);

  if(pMe->m_dispInfo.curX >= screenSize)
  {
    GotoNextLine(pMe);
    nPixels =0;
  }

  if (pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS
    (void)WSTRTOSTR(data,
                    (char *)&pMe->m_pClient_ts_mo->u.cdma.user_data.data[pMe->m_pClient_ts_mo->u.cdma.user_data.number_of_digits],
                    2);

    pMe->m_pClient_ts_mo->u.cdma.user_data.number_of_digits += 1;
#endif /*FEATURE_CDSMS*/
  }
  else
  {
#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/

  }
  if(rv == SUCCESS)
    return TRUE;
  else
    return FALSE;
}


#ifdef FEATURE_SMS_UDH 
/*===========================================================================
FUNCTION WMSAPPMD_CalcNPixelsToEraseEMSElement

DESCRIPTION
  Calculates the number of pixels to be erased for an EMS Content

RETURN
  number of pixels

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean WMSAPPMD_CalcNPixelsToEraseEMSElement
(
CWMSAPP *pMe,
int *nPixels,
int *height,
int position
)
{
  int nHeaders = 0;
  wms_udh_s_type *pUdh = NULL;

  if(pMe == NULL || nPixels == NULL || height == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return FALSE;
  }

  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS
    nHeaders = pMe->m_pClient_ts_mo->u.cdma.user_data.num_headers;

    /*No Header*/
    if(nHeaders== 0)
    {
      return FALSE;
    }

    pUdh = &pMe->m_pClient_ts_mo->u.cdma.user_data.headers[nHeaders-1];
#endif /*FEATURE_CDSMS*/
  }
  else
  {
#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/
  }

  switch(pUdh->header_id)
  {
    case WMS_UDH_SMALL_PICTURE:
      if(pUdh->u.small_picture.position != position)
        return FALSE;

      nPixels[0] = SMALL_PIC_SIZE+2;
      height[0] = SMALL_PIC_SIZE+2;
      break;

    case WMS_UDH_LARGE_PICTURE:
      if(pUdh->u.large_picture.position != position)
        return FALSE;

      nPixels[0] = LARGE_PIC_SIZE+2;
      height[0] = LARGE_PIC_SIZE+2;
      break;

    case WMS_UDH_VAR_PICTURE:
      if(pUdh->u.var_picture.position != position)
        return FALSE;

      nPixels[0] = pUdh->u.var_picture.width+2;
      height[0] = pUdh->u.var_picture.height;
      break;

    case WMS_UDH_SMALL_ANIM:
      if(pUdh->u.small_anim.position != position)
        return FALSE;

      nPixels[0] = SMALL_ANIM_SIZE+2;
      height[0] = SMALL_ANIM_SIZE+2;
      break;

    case WMS_UDH_LARGE_ANIM:
      if(pUdh->u.large_anim.position != position)
        return FALSE;

      nPixels[0] = LARGE_ANIM_SIZE+2;
      height[0] = LARGE_ANIM_SIZE+2;
      break;

    case WMS_UDH_TEXT_FORMATING:
      /*We are in the middle of formatting, the length has not been filled in yet!*/
      if(pUdh->u.text_formating.text_formatting_length == 0)
      {
        /* Middle of Text formatting */
        if(pUdh->u.text_formating.start_position != position)
        {
          // Let the Calling function handle this
          pUdh->u.text_formating.text_formatting_length--;
          return FALSE;
        }
        else
        {  /* The Calling Function has to handle this as well */
          if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
          {
#ifdef FEATURE_CDSMS
            pMe->m_pClient_ts_mo->u.cdma.user_data.num_headers -=1;
            pMe->m_pClient_ts->u.cdma.user_data.num_headers -=1;

            if(pMe->m_pClient_ts_mo->u.cdma.user_data.num_headers == 0)
            {
              pMe->m_pClient_ts_mo->u.cdma.message_id.udh_present = FALSE;
              pMe->m_pClient_ts->u.cdma.message_id.udh_present = FALSE;
            }
#endif /*FEATURE_CDSMS*/
          }
          else
          {
#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/
          }
          return FALSE;
        }
      }
      if((pUdh->u.text_formating.text_formatting_length +
          pUdh->u.text_formating.start_position)
         == position)
      {
        pUdh->u.text_formating.text_formatting_length--;

        if(pUdh->u.text_formating.style_bold)
          pMe->m_curFont = AEE_FONT_BOLD;

        if(pUdh->u.text_formating.style_italic)
          pMe->m_curFlags |= IDF_TEXT_INVERTED;

        if(pUdh->u.text_formating.style_underlined)
          pMe->m_curFlags |= IDF_TEXT_UNDERLINE;

        if(pUdh->u.text_formating.font_size & WMS_UDH_FONT_LARGE)
          pMe->m_curFont = AEE_FONT_LARGE;

        return FALSE;
      }
      return FALSE;

    default:
      break;
  }
  
  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS
    pMe->m_pClient_ts_mo->u.cdma.user_data.num_headers -=1;
    pMe->m_pClient_ts->u.cdma.user_data.num_headers -=1;
    if(pMe->m_pClient_ts_mo->u.cdma.user_data.num_headers == 0)
    {
      pMe->m_pClient_ts_mo->u.cdma.message_id.udh_present = FALSE;
      pMe->m_pClient_ts->u.cdma.message_id.udh_present = FALSE;
    }
#endif /*FEATURE_CDSMS*/
  }
  else
  {
#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/
  }
  return TRUE;
}
#endif /*FEATURE_SMS_UDH */
/*===========================================================================
FUNCTION WMSAPPMD_EraseDisplay

DESCRIPTION
  Erases character/elements from the Screen

RETURN
  SUCCESS, FAILED or EUNSUPPORTED

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int WMSAPPMD_EraseDisplay
(
CWMSAPP *pMe,
int position,
int len
)
{
#if (defined(FEATURE_GWSMS) || defined(FEATURE_CDSMS) || defined (FEATURE_SMS_UDH))
  int dataLength = 0;
  int nHeaders =0;
#endif /* defined(FEATURE_GWSMS) || defined(FEATURE_CDSMS) || defined (FEATURE_SMS_UDH) */
  int height = pMe->m_dispInfo.maxY[pMe->m_dispInfo.curLine];
  int nPixels;

  if(pMe == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return FALSE;
  }


  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS
    nHeaders = pMe->m_pClient_ts_mo->u.cdma.user_data.num_headers;
    dataLength = pMe->m_pClient_ts_mo->u.cdma.user_data.number_of_digits;
#endif /*FEATURE_CDSMS*/
  }
  else
  {
#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/
  }

#ifdef FEATURE_SMS_UDH
  if(nHeaders !=0  &&  WMSAPPMD_CalcNPixelsToEraseEMSElement(pMe,&nPixels, &height, dataLength))
  {
    (void)WMSAPPMD_CDMADisplay(pMe);
    return TRUE;
  }
  else
#endif /* FEATURE_SMS_UDH */
    nPixels = WMSAPPMD_CalcEraseTextPixels(pMe);
  WMSAPPMD_EraseRegion(pMe, nPixels,height);
  return TRUE;
}

#if defined(FEATURE_SMS_UDH) && !defined(FEATURE_SMS_EXTENDED)
/*===========================================================================
FUNCTION WMSAPPMD_DisplayPreText

DESCRIPTION
  Displays Text between EMS Headers

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void WMSAPPMD_DisplayPreText
(
CWMSAPP *pMe,
uint8 *data,
int len
)
{
  if(pMe == NULL || data == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return;
  }

  (void)WMSAPPMD_DisplayText(pMe,
                       data,
                       len,
                       IDF_TEXT_TRANSPARENT,
                       AEE_FONT_NORMAL,
                       NULL);
}


/*===========================================================================
FUNCTION WMSAPPMD_DisplayTextFormatting

DESCRIPTION
  Format and displays text with the appropriate style and font.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean WMSAPPMD_DisplayTextFormatting
(
CWMSAPP *pMe,
uint8 *data,
int *position,
wms_udh_s_type *header
)
{
  //default alignment
  uint32 dwFlags;
  uint8 offset;
  AEEFont font = AEE_FONT_NORMAL;

  if(pMe == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return FALSE;
  }

  if (header->u.text_formating.start_position < *position)
  {
    // Text is already being displayed
    return TRUE;
  }
  offset = (uint8)(header->u.text_formating.start_position - *position);

  /*Display the additional characters between start postion and current
  position*/

  if(offset > 0)
  {
    WMSAPPMD_DisplayPreText(pMe,data,
                            offset);
    *position = header->u.text_formating.start_position;
  }

  switch(header->u.text_formating.alignment_type)
  {
    case WMS_UDH_LEFT_ALIGNMENT:
      dwFlags = IDF_ALIGN_LEFT;
      break;

    case WMS_UDH_CENTER_ALIGNMENT:
      dwFlags = IDF_ALIGN_CENTER;
      break;

    case WMS_UDH_RIGHT_ALIGNMENT:
      dwFlags = IDF_ALIGN_RIGHT;
      break;

    case WMS_UDH_DEFAULT_ALIGNMENT:
    default:
      dwFlags = IDF_ALIGN_NONE;
      break;
  }


  switch(header->u.text_formating.font_size)
  {
    case WMS_UDH_FONT_LARGE:
      font = AEE_FONT_LARGE;
      break;

    case WMS_UDH_FONT_SMALL:
      //No Support
      break;

    case WMS_UDH_FONT_NORMAL:
    default:
      font = AEE_FONT_NORMAL;
      break;


  }


  if(header->u.text_formating.style_bold)
  {
    font = AEE_FONT_BOLD;
  }

  if(header->u.text_formating.style_italic)
  {
    // No Support , use Inverted
    dwFlags |= IDF_TEXT_INVERTED;
  }

  if(header->u.text_formating.style_underlined)
  {
    dwFlags |= IDF_TEXT_UNDERLINE;
  }

  if(header->u.text_formating.style_strikethrough)
  {
    // No Support
  }

  if (header->u.text_formating.text_formatting_length == 0)
  {
    if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
    {
#ifdef FEATURE_CDSMS
      header->u.text_formating.text_formatting_length
      = pMe->m_pClient_ts_mo->u.cdma.user_data.number_of_digits -
          header->u.text_formating.start_position;
#endif /*FEATURE_CDSMS*/
    }
    else
    {
#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/
    }
  }

  /*Display characters only after offset*/

  if((WMSAPPMD_DisplayText(pMe,data + offset,
                           header->u.text_formating.text_formatting_length,
                           dwFlags,
                           font,
                           NULL)))
  {
    *position += header->u.text_formating.text_formatting_length;
    return TRUE;
  }
  return FALSE;
}

#ifdef FEATURE_IMELODY
/*===========================================================================
FUNCTION WMSAPPMD_PlayEventCb

DESCRIPTION
  Callback to handle CMX events.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void WMSAPPMD_PlayEventCb
(
cmx_status_type status,
const void            *client_data,
cmx_af_server_data_type *server_data
)
{
  /* Check Status TBD */
}

/*===========================================================================
FUNCTION WMSAPPMD_CheckCMXPlayAllowed

DESCRIPTION
  THis Function checks if the CMX Play is allowed in current operational mode
  i.e. CMX should not be played in a Voice call / Video Phone Call.

RETURN VALUE
  True / False based on whether CMX can be played or not.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean WMSAPPMD_CheckCMXPlayAllowed
(
  CWMSAPP *pMe
)
{
  boolean inCall = FALSE;

  if(pMe == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return FALSE;
  }

  /* Check for In Call for Sounding Alert */
#ifdef FEATURE_ICM
  inCall = AEECM_IS_VOICECALL_PRESENT(pMe->m_pICM);
#endif /* FEATURE_ICM */

#ifdef FEATURE_QVPHONE
  if (!inCall)
  {
    inCall = AEEVP_IS_VIDEOCALL_CONNECTED(pMe->m_pIVideoPhone);
  }
#endif /* FEATURE_QVPHONE */

  return (!inCall);
}

/*===========================================================================
FUNCTION WMSAPPMD_PlayPreDefSound

DESCRIPTION
  Initiates CMX session to play defined sound.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean WMSAPPMD_PlayPreDefSound
(
  CWMSAPP *pMe,
  uint8 *data,
  int *position,
  wms_udh_s_type *header
)
{
  IFileMgr *pIFileMgr;
  IFile  *pIFile;
  char* fileName;
  char *buf;
  char *fileBuffer;
  int length = 0;
  int size =IMAGE_SIZE;
  FileInfo fileInfo;
  int soundNumber;
  int pos;
  char *tempBuffer;
  IImage *pImage = NULL;
  AEEImageInfo pii;
  
  if(pMe == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return FALSE;
  }

  if(*position < header->u.pre_def_sound.position)
  {
    WMSAPPMD_DisplayPreText(pMe,data,
                            header->u.pre_def_sound.position - *position);
    *position = header->u.pre_def_sound.position;
  }

  fileName = MALLOC(MAX_FILE_NAME);

  if(fileName == NULL)
  {
    MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
    return FALSE;
  }

  fileBuffer = MALLOC(IMAGE_SIZE);

  if(fileBuffer == NULL)
  {
    MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
    WMSAPPU_FREE(fileName);
    return FALSE;
  }

  buf = "ems/melody/imelody";
  length = (int)STRLEN(buf);

  (void)MEMCPY(fileName,buf,(uint16)length);

  // To support PreDefined Sound Numbers from 0..255
  soundNumber = header->u.pre_def_sound.snd_number;

  // Since atmost tempBuffer can be 3 characters
  tempBuffer = MALLOC(4);

  if (tempBuffer == NULL)
  {
    MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
    WMSAPPU_FREE(fileBuffer);
    WMSAPPU_FREE(fileName);
    return FALSE;
  }

  pos = WMSAPPU_ConvertUINT8ToASCII((uint8)soundNumber, tempBuffer);

  // pos indicates the number of digits stored in tempBuffer

  (void)MEMCPY(fileName+length, tempBuffer, (uint16)pos);

  WMSAPPU_FREE (tempBuffer);

  length += pos;

  buf = ".imy";
  (void)MEMCPY(fileName+length, buf, 4);
  length += 4;

  if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR,
                           (void **) &pIFileMgr) != SUCCESS)
  {
    WMSAPPU_FREE(fileName);
    WMSAPPU_FREE(fileBuffer);
    return FALSE;
  }

  pIFile = IFILEMGR_OpenFile(pIFileMgr, fileName,_OFM_READ);

  if(pIFile == NULL)
  {
    (void)IFILEMGR_Release(pIFileMgr);
    WMSAPPU_FREE(fileName);
    WMSAPPU_FREE(fileBuffer);
    return FALSE;
  }

  size = IFILE_Read(pIFile, fileBuffer,(uint32)size);
  (void)IFILE_GetInfo(pIFile, &fileInfo);

  (void)IFILE_Release(pIFile);

  if (WMSAPPMD_CheckCMXPlayAllowed(pMe))
  {
    cmx_handle_type cmx_handle;

    /* Play the Tone */
    cmx_handle.efs.source_type = CMX_SOURCE_MEM;

    cmx_handle.mem.len = fileInfo.dwSize;

    cmx_handle.mem.buf = (uint8*)fileBuffer;


    cmx_handle.mem.source_type = CMX_SOURCE_MEM;


    cmx_audfmt_play(&cmx_handle, (cmx_af_callback_enum_type)0,
                    (cmx_audfmt_start_enum_type)CMX_AUDFMT_START_PATH_FORWARD,
                    (cmx_audfmt_play_cb_func_ptr_type) WMSAPPMD_PlayEventCb,
                    (void *)&pMe->m_melody_trans_id);
  }

  /* Code added for Icon Display */
  (void)STRLCPY(fileName, "ems/melody/melody.bmp", MAX_FILE_NAME);

  pImage = ISHELL_LoadImage(pMe->a.m_pIShell, fileName);

  (void)IFILEMGR_Release(pIFileMgr);

  if(pImage != NULL)
  {
    IIMAGE_GetInfo(pImage, &pii);
    IIMAGE_Start(pImage,  pMe->m_dispInfo.curX, pMe->m_dispInfo.curY);

    pMe->m_pCurImage[pMe->m_numofImages++] = pImage;

    pMe->m_dispInfo.curX += pii.cx+2;
    pMe->m_dispInfo.curY += pii.cx+2;

    pMe->m_dispInfo.maxY[pMe->m_dispInfo.curLine] +=  pMe->m_dispInfo.curY;
    pMe->m_dispInfo.maxX[pMe->m_dispInfo.curLine] +=  pMe->m_dispInfo.curX;

    IDISPLAY_Update(pMe->a.m_pIDisplay);
  }

  WMSAPPU_FREE(fileName);
  WMSAPPU_FREE(fileBuffer);
  return TRUE ;

}/*WMSAPPMD_PlayPreDefSound*/

/*===========================================================================
FUNCTION WMSAPPMD_PlayUserDefSound

DESCRIPTION
  Plays USer Defined Sound

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean WMSAPPMD_PlayUserDefSound
(
  CWMSAPP *pMe,
  uint8 *data,
  int *position,
  wms_udh_s_type *header
)
{
  char *fileName;
  IImage *pImage = NULL;
  AEEImageInfo pii;
  
  if(pMe == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return FALSE;
  }

  if(*position < header->u.user_def_sound.position)
  {
    WMSAPPMD_DisplayPreText(pMe, data,
                            header->u.user_def_sound.position - *position);
    *position = header->u.user_def_sound.position;
  }

  if (WMSAPPMD_CheckCMXPlayAllowed(pMe))
  {
    cmx_handle_type cmx_handle;

    /* Play the Tone */
    cmx_handle.efs.source_type = CMX_SOURCE_MEM;

    cmx_handle.mem.len = header->u.user_def_sound.data_length;

    cmx_handle.mem.buf = (uint8*)header->u.user_def_sound.user_def_sound;

    cmx_handle.mem.source_type = CMX_SOURCE_MEM;

    cmx_audfmt_play(&cmx_handle, (cmx_af_callback_enum_type)0,
                    (cmx_audfmt_start_enum_type)CMX_AUDFMT_START_PATH_FORWARD,
                    (cmx_audfmt_play_cb_func_ptr_type) WMSAPPMD_PlayEventCb,
                    (void *)&pMe->m_melody_trans_id);
  }

  /* Code added for Icon Display */
  fileName = "ems/melody/melody.bmp";

  pImage = ISHELL_LoadImage(pMe->a.m_pIShell, fileName);

  if(pImage != NULL)
  {
    IIMAGE_GetInfo(pImage, &pii);
    IIMAGE_Start(pImage,  pMe->m_dispInfo.curX, pMe->m_dispInfo.curY);

    pMe->m_pCurImage[pMe->m_numofImages++] = pImage;

    pMe->m_dispInfo.curX += pii.cx+2;
    pMe->m_dispInfo.curY += pii.cx+2;

    pMe->m_dispInfo.maxY[pMe->m_dispInfo.curLine] +=  pMe->m_dispInfo.curY;
    pMe->m_dispInfo.maxX[pMe->m_dispInfo.curLine] +=  pMe->m_dispInfo.curX;

    IDISPLAY_Update(pMe->a.m_pIDisplay);
  }

  return TRUE ;

}/*WMSAPPMD_PlayUserDefSound*/

/*===========================================================================
FUNCTION WMSAPPMD_PlayMelody

DESCRIPTION
  Initiates CMX session to Play Melody

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMD_PlayMelody
(
  CWMSAPP *pMe,
  uint16 wDlgID,
  uint16 selection
)
{
  IFileMgr *pIFileMgr;
  IFile  *pIFile;
  char *fileName, *dir;
  char *buf;
  char *fileBuffer;
  int length = 0, i = 0;
  int size =IMAGE_SIZE;
  FileInfo fileInfo;
  int pos;
  char *tempBuffer;

  if(pMe == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return FALSE;
  }

  fileName = MALLOC(MAX_FILE_NAME);

  if(fileName == NULL)
  {
    MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
    return FALSE;
  }

  fileBuffer = MALLOC(IMAGE_SIZE);

  if(fileBuffer == NULL)
  {
    MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
    WMSAPPU_FREE(fileName);
    return FALSE;
  }

  if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR,
                           (void **) &pIFileMgr) != SUCCESS)
  {
    WMSAPPU_FREE (fileName);
    WMSAPPU_FREE (fileBuffer);
    return FALSE;
  }

  if (wDlgID == IDD_LIST_MELODY)
  {
    // Predefined Melody
    buf = "ems/melody/imelody";
    length = (int)STRLEN(buf);

    (void)MEMCPY(fileName,buf,(uint16)length);

    // Since atmost tempBuffer can be 3 characters
    tempBuffer = MALLOC(4);

    pos = WMSAPPU_ConvertUINT8ToASCII((uint8)selection, tempBuffer);

    // pos indicates the number of digits stored in tempBuffer

    (void)MEMCPY(fileName+length, tempBuffer, (uint16)pos);

    WMSAPPU_FREE (tempBuffer);

    length += pos;

    buf = ".imy";
    (void)MEMCPY(fileName+length, buf, 4);
    length += 4;
  }
  else
  {
    // UserDefined Melody
    // Enumerate all instances till selelction Matches

    dir = "ems/usermelody\0";

    if (IFILEMGR_EnumInit(pIFileMgr, dir, FALSE)  == EFAILED)
    {
      WMSAPPU_FREE (fileName);
      WMSAPPU_FREE (fileBuffer);
      (void)IFILEMGR_Release(pIFileMgr);
      return FALSE;
    }

    i=0;
    while(IFILEMGR_EnumNext(pIFileMgr, &fileInfo))
    {
      if (i == selection)
      {
        (void)STRLCPY (fileName, fileInfo.szName, MAX_FILE_NAME);
        break;
      }
      else
        i++;
    }

  }

  pIFile = IFILEMGR_OpenFile(pIFileMgr, fileName,_OFM_READ);

  if(pIFile == NULL)
  {
    (void)IFILEMGR_Release(pIFileMgr);
    WMSAPPU_FREE(fileName);
    WMSAPPU_FREE(fileBuffer);
    return FALSE;
  }

  size = IFILE_Read(pIFile, fileBuffer,(uint32)size);
  (void)IFILE_GetInfo(pIFile, &fileInfo);

  (void)IFILE_Release(pIFile);

  if (WMSAPPMD_CheckCMXPlayAllowed(pMe))
  {
    cmx_handle_type cmx_handle;

    /* Play the Tone */
    cmx_handle.efs.source_type = CMX_SOURCE_MEM;
    /* (void)MEMCPY(cmx_handle.efs.filename, fileInfo.szName, STRLEN(fileInfo.szName)); */

    cmx_handle.mem.len = fileInfo.dwSize;

    cmx_handle.mem.buf = (uint8*)fileBuffer;


    cmx_handle.mem.source_type = CMX_SOURCE_MEM;


    cmx_audfmt_play(&cmx_handle,(cmx_af_callback_enum_type) 0,
                    (cmx_audfmt_start_enum_type)CMX_AUDFMT_START_PATH_FORWARD,
                    (cmx_audfmt_play_cb_func_ptr_type) WMSAPPMD_PlayEventCb,
                    (void *)&pMe->m_melody_trans_id);
  }

  (void)IFILEMGR_Release(pIFileMgr);

  WMSAPPU_FREE(fileName);
  WMSAPPU_FREE(fileBuffer);
  return TRUE ;

}/*WMSAPPMD_PlayMelody*/

#endif /*FEATURE_IMELODY*/

/*===========================================================================
FUNCTION WMSAPPMD_ConvertEmsAnimationToBmp

DESCRIPTION
  Converts EMS animation to bitmap to be viewed.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void WMSAPPMD_ConvertEmsAnimationToBmp
(
byte *imageBuf,
BITMAPFILEHEADER **hdr,
BITMAPINFO *bmpInfo,
int totalSize
)
{
  int width;
  int factor;

  *hdr = (BITMAPFILEHEADER*)MALLOC(sizeof(BITMAPFILEHEADER));
  if(*hdr == NULL)
  {
    MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
    return;
  }


  (*hdr)->bfType = (uint16)('M' << 8 | 'B');
  (*hdr)->bfOffBits = (uint16)(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO));


  bmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmpInfo->bmiHeader.biPlanes = 1;
  bmpInfo->bmiHeader.biBitCount = 1;//Number of bits per pixel
  bmpInfo->bmiHeader.biCompression = 0;

  bmpInfo->bmiHeader.biXPelsPerMeter = 0;//3780;
  bmpInfo->bmiHeader.biYPelsPerMeter = 0;//3780;
  bmpInfo->bmiHeader.biClrUsed = 0;
  bmpInfo->bmiHeader.biClrImportant = 0;

  bmpInfo->bmiColors[0].rgbBlue = 0xFF;
  bmpInfo->bmiColors[0].rgbGreen= 0xFF;
  bmpInfo->bmiColors[0].rgbRed  = 0xFF;

  bmpInfo->bmiColors[1].rgbBlue = 0x00;
  bmpInfo->bmiColors[1].rgbGreen= 0x00;
  bmpInfo->bmiColors[1].rgbRed  = 0x00;


  width = (int)bmpInfo->bmiHeader.biWidth /8;
  if(bmpInfo->bmiHeader.biWidth %8 > 0)
  {
    width++;
  }

  factor = width%4;

  if(factor!=0)
  {
    factor = 4-factor;
  }

  //Since the width is not a double word, ,multiple width by factor
  bmpInfo->bmiHeader.biSizeImage = (dword)(factor+width) * bmpInfo->bmiHeader.biHeight;
  (*hdr)->bfSize = (uint16)((uint16)(*hdr)->bfOffBits + (uint16)bmpInfo->bmiHeader.biSizeImage);


  (void)MEMSET(imageBuf,0x00,IMAGE_SIZE);

  (void)MEMCPY(imageBuf,
         *hdr,
         sizeof(BITMAPFILEHEADER));

  (void)MEMCPY(imageBuf+sizeof(BITMAPFILEHEADER),
         bmpInfo,
         sizeof(BITMAPINFO));


}

/*===========================================================================
FUNCTION WMSAPPMD_ConvertEmsPicToBmp

DESCRIPTION
  Converts EMS picture to bitmap form to be displayed.

DEPENDENCIES
  None

SIDE EFFECTS
  Modifies data in imageBuf directly.
===========================================================================*/
void WMSAPPMD_ConvertEmsPicToBmp
(
byte *imageBuf,
BITMAPFILEHEADER **hdr,
BITMAPINFO *bmpInfo,
uint8 *data,
int totalSize
)
{
  int width;
  int factor;
  int j,k,l,height;

  *hdr = (BITMAPFILEHEADER*)MALLOC(sizeof(BITMAPFILEHEADER));

  if(*hdr == NULL)
  {
    MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
    return;
  }

  (*hdr)->bfType = (uint16)('M' << 8 | 'B');

  (*hdr)->bfOffBits = (uint16)(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO));


  bmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmpInfo->bmiHeader.biPlanes = 1;
  bmpInfo->bmiHeader.biBitCount = 1;//Number of bits per pixel
  bmpInfo->bmiHeader.biCompression = 0;

  bmpInfo->bmiHeader.biXPelsPerMeter = 0;//3780;
  bmpInfo->bmiHeader.biYPelsPerMeter = 0;//3780;
  bmpInfo->bmiHeader.biClrUsed = 0;
  bmpInfo->bmiHeader.biClrImportant = 0;

  bmpInfo->bmiColors[0].rgbBlue = 0xFF;
  bmpInfo->bmiColors[0].rgbGreen= 0xFF;
  bmpInfo->bmiColors[0].rgbRed  = 0xFF;

  bmpInfo->bmiColors[1].rgbBlue = 0x00;
  bmpInfo->bmiColors[1].rgbGreen= 0x00;
  bmpInfo->bmiColors[1].rgbRed  = 0x00;

  width = (int)bmpInfo->bmiHeader.biWidth /8;
  if(bmpInfo->bmiHeader.biWidth %8 > 0)
  {
    width++;
  }

  factor = width%4;

  if(factor!=0)
  {
    factor = 4-factor;
  }

  //Since the width is not a double word, ,multiple width by factor
  bmpInfo->bmiHeader.biSizeImage = (dword)(factor+width) * bmpInfo->bmiHeader.biHeight;
  (*hdr)->bfSize = (uint16)((*hdr)->bfOffBits + bmpInfo->bmiHeader.biSizeImage);


  (void)MEMSET(imageBuf,0x00,IMAGE_SIZE);

  (void)MEMCPY(imageBuf,
         *hdr,
         sizeof(BITMAPFILEHEADER));

  (void)MEMCPY(imageBuf+sizeof(BITMAPFILEHEADER),
         bmpInfo,
         sizeof(BITMAPINFO));

  j = (*hdr)->bfOffBits;
  k=0;
  l=((int)(bmpInfo->bmiHeader.biWidth/8) * (int)(bmpInfo->bmiHeader.biHeight))- width;

  //The bmp starts at the lower left hand corner
  for(height=(int)(bmpInfo->bmiHeader.biHeight-1);height>=0;height--)
  {
    for(k=0;k<width;k++)
    {
      imageBuf[j++]= data[l++];
    }
    l=l-(2*width);
    j +=factor;
  }
}

/*===========================================================================
FUNCTION WMSAPPMD_DisplayPreDefAnimation

DESCRIPTION
  Displays Pre Defined Animation
  equivalent to small animation

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean WMSAPPMD_DisplayPreDefAnimation
(
  CWMSAPP *pMe,
  uint8 *data,
  int *pos,
  wms_udh_s_type *header
)
{
  uint8 animNumber;
  char *fileName;
  char *tempBuffer;
  IImage *pImage = NULL;

  if(pMe == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return FALSE;
  }

  if(header->u.small_anim.position > *pos)
  {
    WMSAPPMD_DisplayPreText(pMe,
                            data,
                            header->u.small_anim.position - *pos);

    *pos = header->u.small_anim.position;
  }

  fileName = MALLOC(MAX_FILE_NAME);

  if(fileName == NULL)
  {
    MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
    return FALSE;
  }

  (void)STRLCPY(fileName,"ems/predefanimations/anim", MAX_FILE_NAME);

  // To support PreDefined Animation Numbers from 0..255
  animNumber = header->u.pre_def_anim.animation_number;

  // Since atmost tempBuffer can be 3 characters
  tempBuffer = MALLOC(5);

  if(tempBuffer == NULL)
  {
    MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
    WMSAPPU_FREE (fileName);
    return FALSE;
  }

  // pos indicates the number of digits stored in tempBuffer
  if (animNumber < 10)
  {
    tempBuffer[0] = '0';
    (void)WMSAPPU_ConvertUINT8ToASCII(animNumber, tempBuffer+1);
  }
  else
  {
    (void)WMSAPPU_ConvertUINT8ToASCII(animNumber, tempBuffer);
  }

  (void)STRLCAT(fileName, tempBuffer, MAX_FILE_NAME);

  (void)STRLCPY(tempBuffer,".bmp", 5);
  (void)STRLCAT(fileName, tempBuffer, MAX_FILE_NAME);

  WMSAPPU_FREE (tempBuffer);

  pImage = ISHELL_LoadImage(pMe->a.m_pIShell,fileName);

  WMSAPPU_FREE (fileName);

  if(pImage == NULL)
    return FALSE;

  IIMAGE_SetFrameCount(pImage, WMS_UDH_ANIM_NUM_BITMAPS);
  IIMAGE_SetAnimationRate(pImage, 250);
  IIMAGE_Start(pImage,pMe->m_dispInfo.curX,pMe->m_dispInfo.curY);

  pMe->m_pCurImage[pMe->m_numofImages++] = pImage;

  IDISPLAY_Update(pMe->a.m_pIDisplay);

  pMe->m_dispInfo.curX += SMALL_ANIM_SIZE + 2;
  pMe->m_dispInfo.maxX[pMe->m_dispInfo.curLine] += pMe->m_dispInfo.curX;

  pMe->m_dispInfo.curY += SMALL_ANIM_SIZE + 2;
  pMe->m_dispInfo.maxY[pMe->m_dispInfo.curLine] += SMALL_ANIM_SIZE + 2;

  return TRUE;

}/*WMSAPPMD_DisplayPreDefAnimation*/

/*===========================================================================
FUNCTION WMSAPPMD_DisplayLargeAnimation

DESCRIPTION
  Format and displays text with the appropriate style and font.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean WMSAPPMD_DisplayLargeAnimation
(
  CWMSAPP *pMe,
  uint8 *data,
  int *pos,
  wms_udh_s_type *header
)
{
  int j,l,height;
  int bitmap;

  BITMAPFILEHEADER *hdr;
  BITMAPINFO       *bmpInfo;
  byte             *imageBuf;
  IImage           *pImage = NULL;

  if(pMe == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return FALSE;
  }

  imageBuf = MALLOC(IMAGE_SIZE);
  if(imageBuf == NULL)
  {
    return FALSE;
  }

  if(header->u.large_anim.position > *pos)
  {
    WMSAPPMD_DisplayPreText(pMe,
                            data,
                            header->u.large_anim.position - *pos);

    *pos = header->u.large_anim.position;
  }

  bmpInfo = MALLOC(sizeof(BITMAPINFO));
  if(bmpInfo == NULL)
  {
    MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
    WMSAPPU_FREE(imageBuf);
    return FALSE;
  }

  bmpInfo->bmiHeader.biWidth = LARGE_ANIM_SIZE*WMS_UDH_ANIM_NUM_BITMAPS;
  bmpInfo->bmiHeader.biHeight = LARGE_ANIM_SIZE;

  WMSAPPMD_ConvertEmsAnimationToBmp(imageBuf,
                                    &hdr,
                                    bmpInfo,
                                    WMS_UDH_ANIM_NUM_BITMAPS*
                                    WMS_UDH_LARGE_BITMAP_SIZE);

  j = hdr->bfOffBits;
  l=((int)((int)(bmpInfo->bmiHeader.biWidth*bmpInfo->bmiHeader.biHeight)/8) - 8)/4;

  //The bmp starts at the lower left hand corner
  for(height=(int)(bmpInfo->bmiHeader.biHeight-1);height>=0;height--)
  {
    for(bitmap=0;bitmap<4;bitmap++)
    {
      imageBuf[j++]= header->u.large_anim.data[bitmap][l];
      imageBuf[j++]= header->u.large_anim.data[bitmap][l+1];
    }

    l -= 2;
  }

  pImage = WMSAPPU_LoadImageFromBuffer (pMe, (void*)imageBuf, (uint32)hdr->bfSize);
  /* imageBuf will be freed by the above function call */

  WMSAPPU_FREE(bmpInfo);
  WMSAPPU_FREE(hdr);

  if(pImage == NULL)
    return FALSE;

  IIMAGE_SetFrameCount(pImage, WMS_UDH_ANIM_NUM_BITMAPS);
  IIMAGE_SetAnimationRate(pImage, 250);
  IIMAGE_Start(pImage,pMe->m_dispInfo.curX,pMe->m_dispInfo.curY);

  pMe->m_pCurImage[pMe->m_numofImages++] = pImage;

  IDISPLAY_Update(pMe->a.m_pIDisplay);

  pMe->m_dispInfo.curX += LARGE_ANIM_SIZE + 2;
  pMe->m_dispInfo.maxX[pMe->m_dispInfo.curLine] =pMe->m_dispInfo.curX;

  pMe->m_dispInfo.curY += LARGE_ANIM_SIZE + 2;
  pMe->m_dispInfo.maxY[pMe->m_dispInfo.curLine] = pMe->m_dispInfo.curY;
  return TRUE;

}/*WMSAPPMD_DisplayLargeAnimation*/

/*===========================================================================
FUNCTION WMSAPPMD_DisplaySmallAnimation

DESCRIPTION
  Displays an EMS small animation.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean WMSAPPMD_DisplaySmallAnimation
(
  CWMSAPP *pMe,
  uint8 *data,
  int *pos,
  wms_udh_s_type *header
)
{
  int j,l,height;
  int bitmap;

  BITMAPFILEHEADER *hdr;
  BITMAPINFO       *bmpInfo;
  byte             *imageBuf;
  IImage           *pImage = NULL;

  if(pMe == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return FALSE;
  }

  if(header->u.small_anim.position > *pos)
  {
    WMSAPPMD_DisplayPreText(pMe,
                            data,
                            header->u.small_anim.position - *pos);

    *pos = header->u.small_anim.position;
  }

  bmpInfo = MALLOC(sizeof(BITMAPINFO));
  if(bmpInfo == NULL)
  {
    MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
    return FALSE;
  }

  bmpInfo->bmiHeader.biWidth = SMALL_ANIM_SIZE*WMS_UDH_ANIM_NUM_BITMAPS;
  bmpInfo->bmiHeader.biHeight = SMALL_ANIM_SIZE;

  imageBuf = MALLOC(IMAGE_SIZE);
  if(imageBuf == NULL)
  {
    MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
    WMSAPPU_FREE(bmpInfo);
    return FALSE;
  }

  WMSAPPMD_ConvertEmsAnimationToBmp(imageBuf,
                                    &hdr,
                                    bmpInfo,
                                    WMS_UDH_ANIM_NUM_BITMAPS*
                                    WMS_UDH_SMALL_BITMAP_SIZE);

  j = hdr->bfOffBits;
  l=((int)((bmpInfo->bmiHeader.biWidth*bmpInfo->bmiHeader.biHeight)/8)/4 - 1);


  //The bmp starts at the lower left hand corner
  for(height=(int)(bmpInfo->bmiHeader.biHeight-1);height>=0;height--)
  {
    for(bitmap=0;bitmap<4;bitmap++)
    {
      imageBuf[j++]= header->u.small_anim.data[bitmap][l];
    }
    l -= 1;
  }

  pImage = WMSAPPU_LoadImageFromBuffer (pMe, (void*)imageBuf, (uint32)hdr->bfSize);
  /* imageBuf will be freed by the above function call */

  WMSAPPU_FREE(bmpInfo);
  WMSAPPU_FREE(hdr);

  if(pImage == NULL)
  {
    return FALSE;
  }

  IIMAGE_SetFrameCount(pImage, WMS_UDH_ANIM_NUM_BITMAPS);
  IIMAGE_SetAnimationRate(pImage, 250);
  IIMAGE_Start(pImage,pMe->m_dispInfo.curX,pMe->m_dispInfo.curY);

  pMe->m_pCurImage[pMe->m_numofImages++] = pImage;

  IDISPLAY_Update(pMe->a.m_pIDisplay);

  pMe->m_dispInfo.curX += SMALL_ANIM_SIZE + 2;
  pMe->m_dispInfo.maxX[pMe->m_dispInfo.curLine] += pMe->m_dispInfo.curX;

  pMe->m_dispInfo.curY += SMALL_ANIM_SIZE + 2;
  pMe->m_dispInfo.maxY[pMe->m_dispInfo.curLine] += SMALL_ANIM_SIZE + 2;

  return TRUE;

}/*WMSAPPMD_DisplaySmallAnimation*/

/*===========================================================================
FUNCTION WMSAPPMD_DisplayLargePicture

DESCRIPTION
  Displays EMS large picture.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean WMSAPPMD_DisplayLargePicture
(
  CWMSAPP *pMe,
  uint8 *data,
  int *pos,
  wms_udh_s_type *header
)
{
  BITMAPFILEHEADER *hdr;
  BITMAPINFO       *bmpInfo;
  byte             *imageBuf;
  IImage           *pImage = NULL;
  AEEImageInfo      pii;
  
  if(pMe == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return FALSE;
  }

  if(header->u.large_picture.position > *pos)
  {
    WMSAPPMD_DisplayPreText(pMe,
                            data,
                            header->u.large_picture.position - *pos);

    *pos = header->u.large_picture.position;
  }

  bmpInfo = MALLOC(sizeof(BITMAPINFO));
  if(bmpInfo == NULL)
  {
    MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
    return FALSE;
  }

  bmpInfo->bmiHeader.biWidth = LARGE_PIC_SIZE;
  bmpInfo->bmiHeader.biHeight = LARGE_PIC_SIZE;

  imageBuf = MALLOC(IMAGE_SIZE);
  if(imageBuf == NULL)
  {
    MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
    WMSAPPU_FREE(bmpInfo);
    return FALSE;
  }

  WMSAPPMD_ConvertEmsPicToBmp(imageBuf,
                              &hdr,
                              bmpInfo,
                              header->u.large_picture.data,
                              WMS_UDH_LARGE_PIC_SIZE);

  pImage = WMSAPPU_LoadImageFromBuffer (pMe, (void*)imageBuf, (uint32)hdr->bfSize);
  /* imageBuf will be freed by the above function call */
  
  WMSAPPU_FREE(hdr);
  WMSAPPU_FREE(bmpInfo);
  
  if (pImage ==NULL)
  {
    return FALSE;
  }

  IIMAGE_GetInfo(pImage,&pii);
  IIMAGE_Start(pImage,  pMe->m_dispInfo.curX, pMe->m_dispInfo.curY);
  pMe->m_pCurImage[pMe->m_numofImages++] = pImage;

  pMe->m_dispInfo.curX += pii.cx+2;
  pMe->m_dispInfo.curY += pii.cy+2;

  pMe->m_dispInfo.maxX[pMe->m_dispInfo.curLine] +=  pii.cx +2;
  pMe->m_dispInfo.maxY[pMe->m_dispInfo.curLine] +=  pii.cy +2;

  IDISPLAY_Update(pMe->a.m_pIDisplay);

  return TRUE;

}/*WMSAPPMD_DisplayLargePicture*/

/*===========================================================================
FUNCTION WMSAPPMD_DisplaySmallPicture

DESCRIPTION
  Displays EMS Small picture.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean WMSAPPMD_DisplaySmallPicture
(
  CWMSAPP *pMe,
  uint8 *data,
  int *pos,
  wms_udh_s_type *header
)
{
  BITMAPFILEHEADER *hdr;
  BITMAPINFO       *bmpInfo;
  byte             *imageBuf;
  IImage           *pImage = NULL;
  AEEImageInfo      pii;
  
  if(pMe == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return FALSE;
  }

  if(header->u.small_picture.position > *pos)
  {
    WMSAPPMD_DisplayPreText(pMe,
                            data,
                            header->u.small_picture.position - *pos);

    *pos = header->u.small_picture.position;
  }

  bmpInfo = MALLOC(sizeof(BITMAPINFO));

  if(bmpInfo == NULL)
  {
    MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
    return FALSE;
  }

  bmpInfo->bmiHeader.biWidth = SMALL_PIC_SIZE;
  bmpInfo->bmiHeader.biHeight = SMALL_PIC_SIZE;

  imageBuf = MALLOC(IMAGE_SIZE);

  if(imageBuf == NULL)
  {
    MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
    WMSAPPU_FREE(bmpInfo);
    return FALSE;
  }

  WMSAPPMD_ConvertEmsPicToBmp(imageBuf,
                              &hdr,
                              bmpInfo,
                              header->u.small_picture.data,
                              WMS_UDH_SMALL_PIC_SIZE);

  pImage = WMSAPPU_LoadImageFromBuffer (pMe, (void*)imageBuf, (uint32)hdr->bfSize);
  /* imageBuf will be freed by the above function call */

  WMSAPPU_FREE(hdr);
  WMSAPPU_FREE(bmpInfo);

  if(pImage == NULL)
  {
    return FALSE;
  }

  IIMAGE_GetInfo(pImage,&pii);
  IIMAGE_Start(pImage,  pMe->m_dispInfo.curX, pMe->m_dispInfo.curY);

  pMe->m_pCurImage[pMe->m_numofImages++] = pImage;

  pMe->m_dispInfo.curX += pii.cx+2;
  pMe->m_dispInfo.curY += pii.cx+2;

  pMe->m_dispInfo.maxY[pMe->m_dispInfo.curLine] +=  pMe->m_dispInfo.curY;
  pMe->m_dispInfo.maxX[pMe->m_dispInfo.curLine] +=  pMe->m_dispInfo.curX;

  IDISPLAY_Update(pMe->a.m_pIDisplay);

  return TRUE;

}/*WMSAPPMD_DisplaySmallPicture*/

/*===========================================================================
FUNCTION WMSAPPMD_DisplayVarPicture

DESCRIPTION
  Displays EMS Variable  picture.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean WMSAPPMD_DisplayVarPicture
(
  CWMSAPP *pMe,
  uint8 *data,
  int *pos,
  wms_udh_s_type *header
)
{
  BITMAPFILEHEADER *hdr;
  BITMAPINFO       *bmpInfo;
  byte             *imageBuf;
  IImage           *pImage = NULL;
  AEEImageInfo      pii;
  
  if(pMe == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return FALSE;
  }
  if(header->u.var_picture.position > *pos)
  {
    WMSAPPMD_DisplayPreText(pMe,
                            data,
                            header->u.var_picture.position - *pos);

    *pos = header->u.var_picture.position;
  }
  
  bmpInfo = MALLOC(sizeof(BITMAPINFO));
  if(bmpInfo == NULL)
  {
    MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
    return FALSE;
  }

  bmpInfo->bmiHeader.biWidth = header->u.var_picture.width;
  bmpInfo->bmiHeader.biHeight = header->u.var_picture.height;

  imageBuf = MALLOC(IMAGE_SIZE);

  if(imageBuf == NULL)
  {
    MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
    WMSAPPU_FREE(bmpInfo);
    return FALSE;
  }

  WMSAPPMD_ConvertEmsPicToBmp(imageBuf,
                              &hdr,
                              bmpInfo,
                              header->u.var_picture.data,
                              WMS_UDH_VAR_PIC_SIZE);

  pImage = WMSAPPU_LoadImageFromBuffer (pMe, (void*)imageBuf, (uint32)hdr->bfSize);
   /* imageBuf will be freed by the above function call */

  WMSAPPU_FREE(hdr);
  WMSAPPU_FREE(bmpInfo);
  
  if (pImage == NULL)
  {
    return FALSE;
  }

  IIMAGE_GetInfo(pImage,&pii);
  IIMAGE_Start(pImage,  pMe->m_dispInfo.curX, pMe->m_dispInfo.curY);

  pMe->m_pCurImage[pMe->m_numofImages++] = pImage;

  pMe->m_dispInfo.curX += pii.cx+2;
  pMe->m_dispInfo.curY += pii.cy+2;

  pMe->m_dispInfo.maxX[pMe->m_dispInfo.curLine] += pMe->m_dispInfo.curX;
  pMe->m_dispInfo.maxY[pMe->m_dispInfo.curLine] += pMe->m_dispInfo.curY;

  return TRUE;

}/*WMSAPPMD_DisplayVarPicture*/

/*===========================================================================
FUNCTION WMSAPPMD_DisplayRfc822Header

DESCRIPTION
  Display Email Address

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean WMSAPPMD_DisplayRfc822Header
(
CWMSAPP *pMe,
uint8 *data,
wms_udh_s_type *header
)
{

  char *email = " EMAIL ADDRESS: ";

  if(pMe == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return FALSE;
  }

  (void)WMSAPPMD_DisplayText(pMe,
                       (uint8*)email,
                       (int)STRLEN(email),
                       IDF_TEXT_TRANSPARENT,
                       AEE_FONT_BOLD,
                       NULL);

  (void)WMSAPPMD_DisplayText(pMe,
                       data,
                       header->u.rfc822.header_length,
                       IDF_TEXT_TRANSPARENT,
                       AEE_FONT_NORMAL,
                       NULL);

  GotoNextLine(pMe);
  /* return(header->u.rfc822.header_length);*/
  return TRUE;
}

/*===========================================================================
FUNCTION WMSAPPMD_DisplaySpecialSM

DESCRIPTION
  Displays Voice Mail if present in the Special Headers

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static int WMSAPPMD_DisplaySpecialSm
(
  CWMSAPP *pMe,
  wms_udh_s_type *header
)
{
  if(pMe == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return FALSE;
  }

  if (header->u.special_sm.msg_waiting_kind == WMS_GW_MSG_WAITING_VOICEMAIL)
  {
    char *vmail = " Voicemail: ";
    char tempBuf[5];

    (void)WMSAPPMD_DisplayText(pMe,
                        (uint8*)vmail,
                        (int)STRLEN(vmail),
                        IDF_TEXT_TRANSPARENT,
                        AEE_FONT_BOLD,
                        NULL);

    (void)WMSAPPU_ConvertUINT8ToASCII(header->u.special_sm.message_count, tempBuf);

    (void)WMSAPPMD_DisplayText(pMe,
                         (uint8*)tempBuf,
                         (int)STRLEN(tempBuf),
                         IDF_TEXT_TRANSPARENT,
                         AEE_FONT_NORMAL,
                         NULL);

    GotoNextLine(pMe);

  }

  return 0;
}
#endif /*FEATURE_SMS_UDH && ! FEATURE_SMS_EXTENDED */

/*===========================================================================
FUNCTION WMSAPPMD_SetScreen

DESCRIPTION
  Sets the Background Screen

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void WMSAPPMD_SetScreen
(
CWMSAPP *pMe
)
{
  AEERect Rect;
#ifdef FEATURE_SMS_UDH
  uint8 i=0;
#endif /* FEATURE_SMS_UDH */

  Rect.x = 0;
  Rect.y = 0;
  Rect.dx = (int16)pMe->m_cxScreen;
  Rect.dy = (int16)pMe->m_cyScreen-FRAME_WIDTH;

  if(pMe == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return;
  }

  if(pMe->m_bEditMsg)
  {
    (void)IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND,
                      MAKE_RGB(255, 255, 255));

  }
  else
  {
    (void)IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND,
                      MAKE_RGB(255, 255, 204));
  }
  IDISPLAY_FrameSolidRect(pMe->a.m_pIDisplay, &Rect);
  
  pMe->m_dispInfo.curLine =0;

  pMe->m_dispInfo.curY = LINE_HEIGHT;
  pMe->m_dispInfo.curX = START_POSITION;

  pMe->m_dispInfo.maxX[pMe->m_dispInfo.curLine] = pMe->m_dispInfo.curX;
  pMe->m_dispInfo.maxY[pMe->m_dispInfo.curLine] = pMe->m_dispInfo.curY;

#ifdef FEATURE_SMS_UDH 
  for(i=0;i< pMe->m_numofImages ;i++)
  {
    (void)IIMAGE_Stop(pMe->m_pCurImage[i]);
    (void)IIMAGE_Release(pMe->m_pCurImage[i]);
    pMe->m_pCurImage[i] = NULL;
  }
  pMe->m_numofImages =0;
#endif /* FEATURE_SMS_UDH */
  IDISPLAY_UpdateEx(pMe->a.m_pIDisplay,TRUE);
  pMe->m_curFlags = 0;
  pMe->m_styleMask =0;

  return;
}

/*===========================================================================
FUNCTION WMSAPPMD_CDMADISPLAY

DESCRIPTION
  Finds the kind of headers in the UDH and displays the header tag at the
  appropriate location

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMD_CDMADisplay
(
  CWMSAPP *pMe
)
{

#ifdef FEATURE_CDSMS
  int position =0;

  wms_cdma_user_data_s_type *user_data ;
  wms_client_ts_data_s_type msg;
  uint8   temp_data[ WMS_CDMA_USER_DATA_MAX ];
  int dataLength;

#ifdef FEATURE_SMS_UDH
  int i =0;
  wms_udh_s_type *header;
  boolean retval =TRUE;
  ems *emsHead=NULL;
  ems *emsTrav;
#endif /* FEATURE_SMS_UDH */

  if(pMe == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return FALSE;
  }

  (void)MEMSET(temp_data, 0, WMS_CDMA_USER_DATA_MAX);
  (void)MEMCPY(&msg, pMe->m_pClient_ts,sizeof(wms_client_ts_data_s_type));
  WMSAPPMD_SetScreen(pMe);
  user_data = &(msg.u.cdma.user_data);

  if(msg.u.cdma.user_data.encoding == WMS_ENCODING_UNICODE)
  {
    /* convert from unicode to display and assign correct length */
    WMSAPPU_ConvertFromUnicode(&msg.u.cdma);
    dataLength = msg.u.cdma.user_data.data_len;
  }
  else
  {
    dataLength = pMe->m_pClient_ts->u.cdma.user_data.number_of_digits;
  }

  if(msg.u.cdma.mask & WMS_MASK_BD_USER_DATA)
  {
    if(msg.u.cdma.user_data.encoding == WMS_ENCODING_IA5 ||
       msg.u.cdma.user_data.encoding == WMS_ENCODING_ASCII)
    {
      msg.u.cdma.user_data.data_len =
      IWMS_TsUnpackAscii( pMe->m_pwms,
                          &msg.u.cdma.user_data,
                          msg.u.cdma.user_data.number_of_digits+1,
                          (byte *)temp_data);

      (void)MEMCPY(msg.u.cdma.user_data.data,
             temp_data,
             msg.u.cdma.user_data.data_len);
    }

    user_data = &(msg.u.cdma.user_data);

    /*ems_head is the head of the linked list. The linked list has the
      sorted list of the headers All the Non-EMS headers are placed before
      all the EMS headers
    */
#ifdef FEATURE_SMS_UDH
    WMSAPPMD_SortCDMAHeaders(user_data,&emsHead);

    emsTrav = emsHead;
    while(i<user_data->num_headers && emsTrav!= NULL && retval)
    {
      header = &user_data->headers[emsTrav->index];
      switch(header->header_id)
      {
        case WMS_UDH_CONCAT_8:
          break;

        case WMS_UDH_CONCAT_16:
          break;


#ifndef FEATURE_SMS_EXTENDED
        case WMS_UDH_SPECIAL_SM:
          (void)WMSAPPMD_DisplaySpecialSm(pMe,header);
          break;

        case WMS_UDH_PORT_8:
          break;

        case WMS_UDH_PORT_16:
          break;

        case WMS_UDH_SMSC_CONTROL:
           break;

        case WMS_UDH_SOURCE:
           break;

        case WMS_UDH_WCMP:
          break;

        case WMS_UDH_TEXT_FORMATING:
          retval = WMSAPPMD_DisplayTextFormatting(pMe,
                                                  user_data->data+position,
                                                  &position,
                                                  header);
          break;


        case WMS_UDH_PRE_DEF_SOUND:
#ifdef FEATURE_IMELODY
          retval = WMSAPPMD_PlayPreDefSound(pMe,
                                            user_data->data+position,
                                            &position,
                                            header);
#endif /*FEATURE_IMELODY*/
          break;

        case WMS_UDH_USER_DEF_SOUND:
#ifdef FEATURE_IMELODY
          retval = WMSAPPMD_PlayUserDefSound(pMe,
                                          user_data->data+position,
                                          &position,
                                          header);
#endif /* FEATURE_IMELODY */
          break;

        case WMS_UDH_PRE_DEF_ANIM:
          retval = WMSAPPMD_DisplayPreDefAnimation(pMe,
                                                 user_data->data+position,
                                                 &position,
                                                 header);
          break;

        case WMS_UDH_LARGE_ANIM:
          retval = WMSAPPMD_DisplayLargeAnimation(pMe,
                                                  user_data->data+position,
                                                  &position,
                                                  header);
          break;

        case WMS_UDH_SMALL_ANIM:
          retval = WMSAPPMD_DisplaySmallAnimation(pMe,
                                                  user_data->data+position,
                                                  &position,
                                                  header);
          break;

        case WMS_UDH_LARGE_PICTURE:
          retval = WMSAPPMD_DisplayLargePicture(pMe,
                                                user_data->data+position,
                                                &position,
                                                header);
          break;

        case WMS_UDH_SMALL_PICTURE:
          retval = WMSAPPMD_DisplaySmallPicture(pMe,
                                                user_data->data+position,
                                                &position,
                                                header);
          break;

        case WMS_UDH_VAR_PICTURE:
          retval = WMSAPPMD_DisplayVarPicture(pMe,
                                              user_data->data+position,
                                              &position,
                                              header);
          break;


        case WMS_UDH_RFC822:
          retval = WMSAPPMD_DisplayRfc822Header(pMe,
                                                user_data->data+position,
                                                header);
          break;
#endif /*!FEATURE_SMS_EXTENDED*/

        default:
          break;
      }
      if(retval == FALSE)
      {
        MSG_ERROR("Header could not be displayed",0,0,0);
        break;
      }
      emsTrav = emsTrav->next;
      i++;
    }

    emsTrav = emsHead;

    for(i=0;i<user_data->num_headers && emsTrav != NULL;i++)
    {
      emsHead = emsTrav->next;
      WMSAPPU_FREE (emsTrav );
      emsTrav = emsHead;
    }
#endif /* FEATURE_SMS_UDH */
    if(WMSAPPU_IsNonStdEmail(TRUE,
                             (uint8*)pMe->m_pClient_msg->u.cdma_message.address.digits,
                             (uint8)pMe->m_pClient_msg->u.cdma_message.address.number_of_digits))
    {
      wmsapp_email_address_s_type email_address;

      position =  WMSAPPU_GetNonStdEmail(temp_data,
                                       (uint8*)email_address.address);
      /* take into account the space */
      position++;
    }
  }
  else
  {
    user_data->data_len = 17;
    (void)MEMCPY(user_data->data,"No Text Available", 17);
    dataLength =17;
  }
  if((dataLength-position) > 0)
  {
    (void)WMSAPPMD_DisplayText(pMe,
                         user_data->data+position,
                         dataLength-position,
                         IDF_TEXT_TRANSPARENT,
                         AEE_FONT_NORMAL,
                         NULL);
  }
#endif /*FEATURE_CDSMS*/

  return TRUE;
}
/*WMSAPPMD_CDMADISPLAY*/



/*===========================================================================
FUNCTION WMSAPPMD_GWDISPLAY

DESCRIPTION
  Finds the kind of headers in the UDH and displays the header tag at the
  appropriate location

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

boolean WMSAPPMD_GWDisplay
(
  CWMSAPP *pMe
)
{
#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/
  return TRUE;
}
/*WMSAPPMD_GWDISPLAY*/

/*===========================================================================
FUNCTION WMSAPPMD_DISPLAY

DESCRIPTION
  Finds the kind of headers in the UDH and displays the header tag at the
  appropriate location

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

boolean WMSAPPMD_Display
(
CWMSAPP *pMe
)
{

  if(pMe == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return FALSE;
  }
  if ( pMe->m_msgInfo.memStore != WMS_MEMORY_STORE_NONE &&
       pMe->m_msgInfo.boxType  == WMSAPP_INBOX )
  {
    (void)IWMS_MsgModifyTag(pMe->m_pwms,
                      pMe->m_clientId,
                      &pMe->m_callback,
                      (void*)pMe,
                      pMe->m_msgInfo.memStore,
                      pMe->m_msgInfo.smsIndex,
                      WMS_TAG_MT_READ);
  }

  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
     return(WMSAPPMD_CDMADisplay(pMe));
  }
  else
  {
     return(WMSAPPMD_GWDisplay(pMe));
  }
}

/*===========================================================================
FUNCTION WMSAPPMD_DisplayCDMAMessageInfo

DESCRIPTION
  Displays Message Info

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMD_DisplayCDMAMessageInfo
(
  CWMSAPP *pMe
)
{
#ifdef FEATURE_CDSMS
  char *pText; /* max number of characters */
  int offset =0;
  char *buf ="";
  int length;
  wms_address_s_type            *addr_ptr;

#ifdef FEATURE_SMS_UDH
  uint8 i=0;
#endif /* FEATURE_SMS_UDH */

  /* Variables needed for non-std e-mail */
  static uint8 email_buf[WMS_CDMA_USER_DATA_MAX];
  wmsapp_email_address_s_type email_address;

  if(pMe == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return FALSE;
  }

  pText = MALLOC(TEXT_BUFFER);

  if(pText == NULL)
  {
    MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
    return FALSE;
  }

  /* Initialize other to 0 */
  (void)MEMSET( &pMe->m_pClient_ts->u.cdma.other, 0, sizeof(wms_other_parm_s_type) );

  if(IWMS_TsDecode(pMe->m_pwms, &pMe->m_pClient_msg->u.cdma_message.raw_ts,
                   pMe->m_pClient_ts) != SUCCESS)
  {
    MSG_HIGH("WMS App decode ts failed!",0,0,0);
    WMSAPPU_FREE(pText);
    return FALSE;
  }

  offset =0;

  /* Display Priority */
  if (pMe->m_pClient_ts->u.cdma.mask & WMS_MASK_BD_PRIORITY)
  {
    offset  += WMSAPPU_FormatPriority(pMe->m_pClient_ts->u.cdma.priority,
                                      (uint8*)pText +offset);

    (void)MEMCPY(pText+offset, " ", 1);
    offset += 1;

  }

  /* Display type of message based on Teleservice this will be displayed after
  ** priority if available.
  */
  if (((pMe->m_pClient_msg->u.cdma_message.teleservice == WMS_TELESERVICE_VMN_95) ||
       (pMe->m_pClient_msg->u.cdma_message.teleservice == WMS_TELESERVICE_IS91_VOICE_MAIL) ||
       (pMe->m_pClient_msg->u.cdma_message.teleservice == WMS_TELESERVICE_MWI)) &&
      ( pMe->m_pClient_ts->u.cdma.num_messages != 0 ))
  {
    buf = "VMAIL ";
    length = (int)STRLEN(buf);

    (void)MEMCPY(pText+offset, buf, (uint16)length);

    /* Display the number of voice mails */
    if (pMe->m_pClient_ts->u.cdma.num_messages >= 100)
    {
      pText[length+offset]=(byte)pMe->m_pClient_ts->u.cdma.num_messages/100+'0';
      offset++;
    }
    pText[length+offset]=(byte)((pMe->m_pClient_ts->u.cdma.num_messages%100)/10)+'0';
    offset++;
    pText[length+offset]=(byte)pMe->m_pClient_ts->u.cdma.num_messages%10+'0';
    offset++;
  }
  else if (pMe->m_pClient_msg->u.cdma_message.teleservice == WMS_TELESERVICE_IS91_PAGE ||
           pMe->m_pClient_msg->u.cdma_message.teleservice == WMS_TELESERVICE_CPT_95)
  {
    buf = "PAGE";
    length = (int)STRLEN (buf);
    (void)MEMCPY(pText+offset, buf, (uint16)length);
  }
  else
  {
    buf = "MESSAGE";
    length = (int)STRLEN (buf);

    (void)MEMCPY(pText+offset,buf,(uint16)length);
  }

  (void)WMSAPPMD_DisplayText(pMe, (uint8 *)pText, length+offset, IDF_TEXT_TRANSPARENT,
                     AEE_FONT_BOLD, NULL);

  GotoNextLine(pMe);
  offset =0;

#if 0
  /* Display alert mode if it is an available parameter */
  if (pMe->m_pClient_ts->u.cdma.mask & WMS_MASK_BD_ALERT)
  {
    offset += WMSAPPU_FormatAlertMode(pMe->m_pClient_ts->u.cdma.alert_mode,
                                      (uint8*)pText);

    (void)WMSAPPMD_DisplayText(pMe, (uint8 *)pText, offset, IDF_TEXT_TRANSPARENT,
                       AEE_FONT_BOLD, NULL);
    offset =0;
    GotoNextLine(pMe);
  }

  /* Display display mode if it is an available parameter */
  if (pMe->m_pClient_ts->u.cdma.mask & WMS_MASK_BD_DISPLAY_MODE)
  {
    offset += WMSAPPU_FormatDisplayMode(pMe->m_pClient_ts->u.cdma.display_mode,
                                       (uint8*)pText);
    (void)WMSAPPMD_DisplayText(pMe, (uint8 *)pText, offset, IDF_TEXT_TRANSPARENT,
                       AEE_FONT_BOLD, NULL);
    offset =0;
    GotoNextLine(pMe);
  }
#endif /* 0 */

  (void)MEMSET(pText,0x20,TEXT_BUFFER);

  /* Display Message ID */
  buf = "MSG #:";
  length = (int)STRLEN(buf);

  (void)MEMCPY(pText +offset, buf, (uint16)length);
  offset += length;
  (void)WMSAPPMD_DisplayText(pMe, (uint8 *)pText, offset, IDF_TEXT_TRANSPARENT,
                       AEE_FONT_BOLD, NULL);

  pText[++offset] = (byte)(pMe->m_pClient_ts->u.cdma.message_id.id_number / 10000) + '0';
  pText[++offset]= (byte)(pMe->m_pClient_ts->u.cdma.message_id.id_number/1000 % 10) + '0';
  pText[++offset]= (byte)(pMe->m_pClient_ts->u.cdma.message_id.id_number/100 % 10) + '0';
  pText[++offset]= (byte)(pMe->m_pClient_ts->u.cdma.message_id.id_number/10 % 10) + '0';
  pText[++offset]= (byte)pMe->m_pClient_ts->u.cdma.message_id.id_number % 10 + '0';

  offset ++;

  (void)WMSAPPMD_DisplayText(pMe, (uint8 *)pText+length, offset-length, IDF_TEXT_TRANSPARENT,
                     AEE_FONT_NORMAL, NULL);

  GotoNextLine(pMe);
  offset =0;
  (void)MEMSET(pText,' ',TEXT_BUFFER);

  /* If msg is MO the address in the message is the destination address.
  ** If msg is MT the address in the message is the origination address.
  */
  addr_ptr = & pMe->m_pClient_msg->u.cdma_message.address;

  if (WMS_IS_MO((int)pMe->m_pClient_msg->msg_hdr.tag))
  {
    buf = "Message To: ";

  }
  else
  {
    buf = "Message From: ";

  }

  length = (int)STRLEN(buf);

  (void)MEMCPY(pText,buf,(uint16)length);
  offset += length;

  /* Check if this is a non-std email formatted address before displaying */
  if ((addr_ptr->digit_mode == WMS_DIGIT_MODE_4_BIT) &&
      (WMSAPPU_IsNonStdEmail(TRUE, (uint8*)addr_ptr->digits,
                        addr_ptr->number_of_digits)))
  {
    /* Unpack to get the user data */
    (void)IWMS_TsUnpackAscii( pMe->m_pwms,
                        &pMe->m_pClient_ts->u.cdma.user_data,
                        pMe->m_pClient_ts->u.cdma.user_data.number_of_digits+1,
                        email_buf);

    /* Get the email address to display */
    email_address.len =
    WMSAPPU_GetNonStdEmail(email_buf,
                          (uint8*)email_address.address);

    (void)WMSAPPMD_DisplayText(pMe, (uint8 *)pText, length, IDF_TEXT_TRANSPARENT,
                     AEE_FONT_BOLD, NULL);

    (void)WMSAPPMD_DisplayText(pMe, (uint8 *)email_address.address, email_address.len,
                       IDF_TEXT_TRANSPARENT, AEE_FONT_NORMAL, NULL);
    offset = 0;
    (void)MEMSET(pText,' ',TEXT_BUFFER);
    GotoNextLine(pMe);
  }
  else
  {
    offset  += WMSAPPU_FormatAddress( pMe,
                                     TRUE,
                                     &pMe->m_pClient_msg->u.cdma_message.address,
                                     (uint8*)pText +offset,
                                      TEXT_BUFFER - offset);

    (void)WMSAPPMD_DisplayText(pMe, (uint8 *)pText, length, IDF_TEXT_TRANSPARENT,
                     AEE_FONT_BOLD, NULL);

    (void)WMSAPPMD_DisplayText(pMe, (uint8 *)pText+length, offset -length,
                     IDF_TEXT_TRANSPARENT, AEE_FONT_NORMAL, NULL);

    offset = 0;
    (void)MEMSET(pText,' ',TEXT_BUFFER);
    GotoNextLine(pMe);
  }

  offset = 0;

  /* Display callback if available */
  if (pMe->m_pClient_ts->u.cdma.mask & WMS_MASK_BD_CALLBACK)
  {
    buf = "CALLBACK:  ";
    length = (int)STRLEN(buf);

    (void)MEMCPY(pText+offset,buf, (uint16)length);

    offset += length;

    offset  += WMSAPPU_FormatAddress(pMe,
                                    TRUE,
                                    &pMe->m_pClient_ts->u.cdma.callback,
                                    (uint8*)pText + offset,
                                    TEXT_BUFFER - offset);
    pText[offset] = 0;

    (void)WMSAPPMD_DisplayText(pMe, (uint8 *)pText, length, IDF_TEXT_TRANSPARENT,
                       AEE_FONT_BOLD, NULL);

    (void)WMSAPPMD_DisplayText(pMe, (uint8 *)pText+length, offset - length,
                       IDF_TEXT_TRANSPARENT, AEE_FONT_NORMAL, NULL);

    GotoNextLine(pMe);
    offset =0;
    (void)MEMSET(pText,' ',TEXT_BUFFER);
  }

  /* Display MC timestamp if available */
  if (pMe->m_pClient_ts->u.cdma.mask & WMS_MASK_BD_MC_TIME)
  {
    buf = "Time:";
    length = (int)STRLEN(buf);

    (void)MEMCPY(pText+offset,buf, (uint16)length);

    offset += length;

    offset += WMSAPPU_FormatTimestamp( pMe,& pMe->m_pClient_ts->u.cdma.mc_time,
                                      (uint8 *)pText+offset, TEXT_BUFFER - offset);
    pText[offset] = 0;

    (void)WMSAPPMD_DisplayText(pMe, (uint8 *)pText, length, IDF_TEXT_TRANSPARENT,
                       AEE_FONT_BOLD, NULL);

    (void)WMSAPPMD_DisplayText(pMe, (uint8 *)pText+length, offset-length,
                       IDF_TEXT_TRANSPARENT, AEE_FONT_NORMAL, NULL);

    GotoNextLine(pMe);
    offset =0;
    (void)MEMSET(pText,' ',TEXT_BUFFER);
  }

  /* Display message type if not reserved */
  if (pMe->m_pClient_ts->u.cdma.mask & WMS_MASK_BD_MSG_ID)
  {
    if (pMe->m_pClient_ts->u.cdma.message_id.type != WMS_BD_TYPE_RESERVED_0)
    {
      offset = WMSAPPU_FormatBDType(pMe->m_pClient_ts->u.cdma.message_id.type,
                                   (uint8*)pText+offset);

      (void)WMSAPPMD_DisplayText(pMe, (uint8 *)pText, offset, IDF_TEXT_TRANSPARENT,
                       AEE_FONT_BOLD, NULL);
      GotoNextLine(pMe);
      offset =0;
      (void)MEMSET(pText,' ',TEXT_BUFFER);
    }
  }

  if((pMe->m_pClient_ts->u.cdma.mask & WMS_MASK_BD_REPLY_OPTION) &&
     (pMe->m_pClient_ts->u.cdma.reply_option.user_ack_requested))
  {
    buf = "Reply requested";
    length = (int)STRLEN(buf);

    (void)MEMCPY(pText+offset,buf, (uint16)length);
    offset += length;

    (void)WMSAPPMD_DisplayText(pMe, (uint8 *)pText, length, IDF_TEXT_TRANSPARENT,
                       AEE_FONT_NORMAL, NULL);

    GotoNextLine(pMe);
    offset =0;
    (void)MEMSET(pText,' ',TEXT_BUFFER);
  }

#ifdef FEATURE_SMS_UDH
  /* Concat Header Display */
  for (i=0; i<pMe->m_pClient_ts->u.cdma.user_data.num_headers; i++)
  {
    if (pMe->m_pClient_ts->u.cdma.user_data.headers[i].header_id == WMS_UDH_CONCAT_8)
    {
      buf = "Segment: ";
      length = (int)STRLEN(buf);
      (void)MEMCPY(pText +offset, buf, (uint16)length);
      offset += length;

      offset += WMSAPPU_ITOA(pMe->m_pClient_ts->u.cdma.user_data.headers[i].u.concat_8.seq_num, pText+offset, TEXT_BUFFER - offset, 10);
      pText[offset++] = '/';
      offset += WMSAPPU_ITOA(pMe->m_pClient_ts->u.cdma.user_data.headers[i].u.concat_8.total_sm, pText+offset, TEXT_BUFFER - offset, 10);
      pText[offset++] = '(';
      pText[offset++] = '0';
      pText[offset++] = 'x';
      pText[offset++] = WMSAPPU_ConvertHexToAscii((byte)((pMe->m_pClient_ts->u.cdma.user_data.headers[i].u.concat_8.msg_ref & 0xF0)>>4));
      pText[offset++] = WMSAPPU_ConvertHexToAscii((byte)(pMe->m_pClient_ts->u.cdma.user_data.headers[i].u.concat_8.msg_ref & 0x0F));
      pText[offset++] = ')';
      /* Null Terminate */
      pText[offset] = 0;
      break;
    }
    else if(pMe->m_pClient_ts->u.cdma.user_data.headers[i].header_id == WMS_UDH_CONCAT_16)
    {
      buf = "Segment: ";
      length = (int)STRLEN(buf);
      (void)MEMCPY(pText +offset, buf, (uint16)length);
      offset += length;

      offset += WMSAPPU_ITOA(pMe->m_pClient_ts->u.cdma.user_data.headers[i].u.concat_16.seq_num, pText+offset, TEXT_BUFFER - offset, 10);
      pText[offset++] = '/';
      offset += WMSAPPU_ITOA(pMe->m_pClient_ts->u.cdma.user_data.headers[i].u.concat_16.total_sm, pText+offset, TEXT_BUFFER - offset, 10);
      pText[offset++] = '(';
      pText[offset++] = '0';
      pText[offset++] = 'x';
      pText[offset++] = WMSAPPU_ConvertHexToAscii((byte)((pMe->m_pClient_ts->u.cdma.user_data.headers[i].u.concat_16.msg_ref & 0xF000)>>12));
      pText[offset++] = WMSAPPU_ConvertHexToAscii((byte)((pMe->m_pClient_ts->u.cdma.user_data.headers[i].u.concat_16.msg_ref & 0x0F00)>>8));
      pText[offset++] = WMSAPPU_ConvertHexToAscii((byte)((pMe->m_pClient_ts->u.cdma.user_data.headers[i].u.concat_16.msg_ref & 0x00F0)>>4));
      pText[offset++] = WMSAPPU_ConvertHexToAscii((byte)(pMe->m_pClient_ts->u.cdma.user_data.headers[i].u.concat_16.msg_ref & 0x000F));
      pText[offset++] = ')';
      /* Null Terminate */
      pText[offset] = 0;
      break;
    }
  }
#endif /* FEATURE_SMS_UDH */
  if (offset > 0)
  {
    (void)WMSAPPMD_DisplayText(pMe, (uint8 *)pText, length, IDF_TEXT_TRANSPARENT,
                       AEE_FONT_BOLD, NULL);
    (void)WMSAPPMD_DisplayText(pMe, (uint8 *)pText+length, offset-length, IDF_TEXT_TRANSPARENT,
                    AEE_FONT_NORMAL, NULL);

    GotoNextLine(pMe);
    offset =0;
    (void)MEMSET(pText,' ',TEXT_BUFFER);
  }

  WMSAPPU_FREE(pText);
#endif /*FEATURE_CDSMS*/
  return TRUE;

}


/*===========================================================================
FUNCTION WMSAPPMD_DisplayGWMessageInfo

DESCRIPTION
  Displays Message Info.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMD_DisplayGWMessageInfo
(
  CWMSAPP *pMe
)
{
#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/

  return TRUE;

}


/*===========================================================================
FUNCTION WMSAPPMD_DisplayMessageInfo

DESCRIPTION
  Displays Message Info.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMD_DisplayMessageInfo
(
  CWMSAPP *pMe
)
{
  char *pText; /* max number of characters */
  int offset =0;
  char *buf ="";
  int length;

  if(pMe == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return FALSE;
  }

  pText = MALLOC(TEXT_BUFFER);

  if(pText == NULL)
  {
    MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
    return FALSE;
  }


  /* Clear the display */
  WMSAPPMD_SetScreen(pMe);

  /* Display the message tag */
  switch (pMe->m_pClient_msg->msg_hdr.tag)
  {
    case WMS_TAG_MT_READ:
      buf = "OLD MSG";
      break;

    case WMS_TAG_MT_NOT_READ:
      buf = "NEW MSG";
      break;

    case WMS_TAG_MO_SENT:
      buf = "SENT MSG";
      break;

    case WMS_TAG_MO_NOT_SENT:
      buf = "UNSENT MSG";
      break;

    default:
      WMSAPPU_FREE(pText);
      MSG_ERROR("Invalid tag %d", pMe->m_pClient_msg->msg_hdr.tag, 0,0);
      return FALSE;
  }

  length = (int)STRLEN(buf);
  (void)MEMCPY(pText, buf, (uint16)length);
  offset =length;

  (void)WMSAPPMD_DisplayText(pMe, (uint8 *)pText, offset, IDF_TEXT_TRANSPARENT,
                     AEE_FONT_BOLD, NULL);

  GotoNextLine(pMe);
  offset =0;
  length =0;
  (void)MEMSET(pText,0x20,TEXT_BUFFER);

  /* Display Memory and Slot # */
  if (pMe->m_pClient_msg->msg_hdr.mem_store == WMS_MEMORY_STORE_RAM_CDMA ||
      pMe->m_pClient_msg->msg_hdr.mem_store == WMS_MEMORY_STORE_RAM_GW )
  {
    buf = "RAM slot: ";
  }
  else if (pMe->m_pClient_msg->msg_hdr.mem_store == WMS_MEMORY_STORE_NV_CDMA ||
           pMe->m_pClient_msg->msg_hdr.mem_store == WMS_MEMORY_STORE_NV_GW)
  {
    buf = "NV slot:  ";
  }
  else if (pMe->m_pClient_msg->msg_hdr.mem_store == WMS_MEMORY_STORE_RUIM)
  {
    buf = "RUIM slot:";
  }
  else if (pMe->m_pClient_msg->msg_hdr.mem_store == WMS_MEMORY_STORE_SIM)
  {
    buf = "SIM slot:";
  }
  else
  {
    buf = "Unstored Msg";
  }

  length = (int)STRLEN(buf);

  if (length !=0)
  {

    (void)MEMCPY( pText+offset ,buf , (uint16)length);
    offset +=length;
    pText [++offset]  = (byte)pMe->m_pClient_msg->msg_hdr.index / 10 + '0';
    pText [++offset] = (byte)pMe->m_pClient_msg->msg_hdr.index % 10 + '0';

    (void)WMSAPPMD_DisplayText(pMe, (uint8 *)pText, length, IDF_TEXT_TRANSPARENT,
                       AEE_FONT_BOLD, NULL);

    if (pMe->m_pClient_msg->msg_hdr.mem_store != WMS_MEMORY_STORE_NONE)
      (void)WMSAPPMD_DisplayText(pMe, (uint8 *)pText+length+1, offset-length,
                         IDF_TEXT_TRANSPARENT, AEE_FONT_NORMAL, NULL);

    GotoNextLine(pMe);
  }

  WMSAPPU_FREE(pText);

  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
    return WMSAPPMD_DisplayCDMAMessageInfo(pMe);
  else
    return WMSAPPMD_DisplayGWMessageInfo(pMe);
}

#if defined(FEATURE_SMS_UDH ) && !defined(FEATURE_SMS_EXTENDED)
/*===========================================================================
FUNCTION WMSAPPMD_ConvertPicToEMS

DESCRIPTION
  Converts BMP format to EMS Format.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMD_ConvertPicToEMS
(
  CWMSAPP        *pMe,      /* INPUT */
  char           *fileName, /* INPUT */
  wms_udh_s_type *pUdh      /* OUTPUT */
)
{
  int i,j;
  BITMAPFILEHEADER *hdr;
  BITMAPINFO *bmpInfo;
  byte *imageBuf;
  int size = IMAGE_SIZE;


  IFileMgr *pIFilemgr;
  IFile *pIFile;
  int index;
  int width;
  char str[3];

  if(pMe == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return FALSE;
  }

  if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR,(void **) &pIFilemgr) != SUCCESS)
  {
    return FALSE;
  }

  pIFile = IFILEMGR_OpenFile(pIFilemgr, fileName,_OFM_READ);

  if(pIFile == NULL)
  {
    (void)IFILEMGR_Release(pIFilemgr);
    return FALSE;
  }

  imageBuf = MALLOC(IMAGE_SIZE);

  if(imageBuf == NULL)
  {
    MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
    (void)IFILEMGR_Release(pIFilemgr);
    return FALSE;
  }

  size = IFILE_Read(pIFile, imageBuf, (uint32)size);
  (void)IFILE_Release(pIFile);

  hdr = MALLOC(sizeof(BITMAPFILEHEADER));

  if(hdr == NULL)
  {
    MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
    WMSAPPU_FREE(imageBuf);
    (void)IFILEMGR_Release(pIFilemgr);
    return FALSE;
  }

  /*Get the Header Info*/
  (void)MEMCPY(hdr,imageBuf,sizeof(BITMAPFILEHEADER));


  str[0] = imageBuf[0];
  str[1] = imageBuf[1];
  str[2] = '\0';

  /*Is it a BMP?*/
  if(STRCMP(str,"BM") == 0)
  {
    bmpInfo = MALLOC(sizeof(BITMAPINFO));

    if(bmpInfo == NULL)
    {
      MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
      WMSAPPU_FREE(hdr);
      WMSAPPU_FREE(imageBuf);
      (void)IFILEMGR_Release(pIFilemgr);
      return FALSE;
    }

    (void)MEMCPY(bmpInfo,imageBuf+sizeof(BITMAPFILEHEADER),sizeof(BITMAPINFO));

    /*Number of bytes that fit on one line*/
    width = (int)(bmpInfo->bmiHeader.biWidth/8);

    /*index of the output buffer*/
    index = width-1;
    /*BMP to EMS Conversion, bytes in a bmp are organised the following way
      30 31 28 29 26 27......2 3 0 1 */

    switch(pUdh->header_id)
    {
      case WMS_UDH_SMALL_PICTURE:
        for(i=(int)bmpInfo->bmiHeader.biSizeImage-1;i>=0;)
        {
          i -= 4-width; // Word Boundary
          for(j=0;j<width;j++)
          {
            pUdh->u.small_picture.data[index--] = imageBuf[hdr->bfOffBits + i--];
          }
          index += 2*width;
        }
        break;

      case WMS_UDH_LARGE_PICTURE:
        for(i=(int)bmpInfo->bmiHeader.biSizeImage-1;i>=0;)
        {
          i -= 4-width;
          for(j=0;j<width;j++)
          {
            pUdh->u.large_picture.data[index--] = imageBuf[hdr->bfOffBits + i--];
          }
          index += 2*width;
        }
        break;

      case WMS_UDH_VAR_PICTURE:
        for(i=(int)bmpInfo->bmiHeader.biSizeImage-1;i>=0;)
        {
          i -= (4-width%4);
          for(j=0;j<width;j++)
          {
            pUdh->u.var_picture.data[index--] = imageBuf[hdr->bfOffBits + i--];
          }
          index += 2*width;
        }
        /* Set the Height and Width of the Variable Picture */
        
        if (bmpInfo->bmiHeader.biHeight >= 0)
        {
          pUdh->u.var_picture.height = (uint8)(bmpInfo->bmiHeader.biHeight);
        }
        else
        {
          pUdh->u.var_picture.height = (uint8)(bmpInfo->bmiHeader.biHeight * -1);
        }

        pUdh->u.var_picture.width  = (uint8)(bmpInfo->bmiHeader.biWidth);
        break;

      default:
        WMSAPPU_FREE(bmpInfo);
        (void)IFILEMGR_Release(pIFilemgr);
        WMSAPPU_FREE(hdr);
        WMSAPPU_FREE(imageBuf);
        return FALSE;
    }
    WMSAPPU_FREE(bmpInfo);
  }

  (void)IFILEMGR_Release(pIFilemgr);
  WMSAPPU_FREE(hdr);
  WMSAPPU_FREE(imageBuf);
  return TRUE;
}


/*===========================================================================
FUNCTION WMSAPPMD_ConvertSmallAnimtoEMS

DESCRIPTION
  Converts Small Anim BMP to EMS Format

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int WMSAPPMD_ConvertSmallAnimToEMS
(
CWMSAPP *pMe,
uint8 data[4][8],
char* fileName
)
{
  int i;//,j;
  BITMAPFILEHEADER *hdr;
  BITMAPINFO *bmpInfo;
  byte *imageBuf;
  int size =IMAGE_SIZE;

  IFileMgr *pIFilemgr;
  IFile *pIFile;
  int index;
  char str[3];

  if(pMe == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return FALSE;
  }

  if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFilemgr) != SUCCESS)
  {
    return FALSE;
  }

  pIFile = IFILEMGR_OpenFile(pIFilemgr, fileName,_OFM_READ);

  if(pIFile == NULL)
  {
    (void)IFILEMGR_Release(pIFilemgr);
    return FALSE;
  }

  imageBuf = MALLOC((dword)size);

  if(imageBuf == NULL)
  {
    MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
    (void)IFILEMGR_Release(pIFilemgr);
    (void)IFILE_Release(pIFile);
    return FALSE;
  }

  size = IFILE_Read(pIFile, imageBuf, (uint32)size);
  (void)IFILE_Release(pIFile);

  hdr = MALLOC(sizeof(BITMAPFILEHEADER));

  if(hdr == NULL)
  {
    MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
    WMSAPPU_FREE(imageBuf);
    (void)IFILEMGR_Release(pIFilemgr);
    return FALSE;
  }

  (void)MEMCPY(hdr,imageBuf,sizeof(BITMAPFILEHEADER));

  str[0] = imageBuf[0];
  str[1] = imageBuf[1];
  str[2] = '\0';

  if(STRCMP(str,"BM") == 0)
  {
    bmpInfo = MALLOC(sizeof(BITMAPINFO));
    if(bmpInfo == NULL)
    {
      MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
      WMSAPPU_FREE(imageBuf);
      WMSAPPU_FREE(hdr);
      (void)IFILEMGR_Release(pIFilemgr);
      (void)IFILE_Release(pIFile);
      return FALSE;
    }
    (void)MEMCPY(bmpInfo,imageBuf+sizeof(BITMAPFILEHEADER),sizeof(BITMAPINFO));

    index = 0;
    
    //BMP to EMS Conversion, bytes in a bmp are organised the following way
    //1.30 1.31 2.30 2.31 3.30 3.31 1.28 1.29....1.0 1.1 2.0 2.1 3.0 3.1 4.0 4.1

    for(i=(int)bmpInfo->bmiHeader.biSizeImage-1;i>=0;i--)
    {
      int bitmap =0;

      for(bitmap=3;bitmap>=0;bitmap--)
      {
        data[bitmap][index] = imageBuf[hdr->bfOffBits + i--];
      }

      index += 1;
    }
    WMSAPPU_FREE(bmpInfo);
  }

  (void)IFILEMGR_Release(pIFilemgr);
  WMSAPPU_FREE(imageBuf);
  WMSAPPU_FREE(hdr);
  return TRUE;
}

/*===========================================================================
FUNCTION WMSAPPMD_ConvertLargeAnimtoEMS

DESCRIPTION
  Converts LArge Anim BMP to EMS

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMD_ConvertLargeAnimToEMS
(
CWMSAPP *pMe,
uint8 data[4][32],
char* fileName
)
{
  int i;//,j;
  BITMAPFILEHEADER *hdr;
  BITMAPINFO *bmpInfo = NULL;
  byte *imageBuf;
  int size =IMAGE_SIZE;

  IFileMgr *pIFilemgr;
  IFile *pIFile;
  int index;
  char str[3];

  if(pMe == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return FALSE;
  }

  if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFilemgr) != SUCCESS)
  {
    return FALSE;
  }

  pIFile = IFILEMGR_OpenFile(pIFilemgr, fileName,_OFM_READ);

  if(pIFile == NULL)
  {
    (void)IFILEMGR_Release(pIFilemgr);
    return FALSE;
  }

  imageBuf = MALLOC((dword)size);

  if(imageBuf == NULL)
  {
    MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
    (void)IFILEMGR_Release(pIFilemgr);
    (void)IFILE_Release(pIFile);
    return FALSE;
  }

  size = IFILE_Read(pIFile, imageBuf,(uint32)size);
  (void)IFILE_Release(pIFile);

  hdr = MALLOC(sizeof(BITMAPFILEHEADER));

  if(hdr == NULL)
  {
    MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
    WMSAPPU_FREE(imageBuf);
    (void)IFILEMGR_Release(pIFilemgr);
    return FALSE;
  }

  (void)MEMCPY(hdr,imageBuf,sizeof(BITMAPFILEHEADER));




  str[0] = imageBuf[0];
  str[1] = imageBuf[1];
  str[2] = '\0';

  if(STRCMP(str,"BM") == 0)
  {
    bmpInfo = MALLOC(sizeof(BITMAPINFO));

    if(bmpInfo == NULL)
    {
      MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
      WMSAPPU_FREE(imageBuf);
      WMSAPPU_FREE(hdr);
      (void)IFILEMGR_Release(pIFilemgr);
      return FALSE;
    }

    (void)MEMCPY(bmpInfo,imageBuf+sizeof(BITMAPFILEHEADER),sizeof(BITMAPINFO));

    index = 0;
    
    //BMP to EMS Conversion, bytes in a bmp are organised the following way
    //1.30 1.31 2.30 2.31 3.30 3.31 1.28 1.29....1.0 1.1 2.0 2.1 3.0 3.1 4.0 4.1
    for(i=(int)bmpInfo->bmiHeader.biSizeImage-1;i>=0;)
    {
      int bitmap =0;

      for(bitmap=3;bitmap>=0;bitmap--)
      {
        data[bitmap][index+1] = imageBuf[hdr->bfOffBits + i--];
        data[bitmap][index]   = imageBuf[hdr->bfOffBits + i--];

      }

      index += 2;
    }
  }

  WMSAPPU_FREE(bmpInfo);
  (void)IFILEMGR_Release(pIFilemgr);
  WMSAPPU_FREE(hdr);
  return TRUE;
}

/*===========================================================================
FUNCTION WMSAPPMD_ConvertUserMelodyToEMS

DESCRIPTION
  Converts User Melody to EMS
  Returns the File Size

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int WMSAPPMD_ConvertUserMelodyToEMS
(
CWMSAPP *pMe,
uint8 *data,
char* fileName
)
{
  int size = MELODY_SIZE;

  IFileMgr *pIFilemgr;
  IFile *pIFile;

  if(pMe == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return FALSE;
  }

  if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void **)&pIFilemgr) != SUCCESS)
  {
    return FALSE;
  }

  pIFile = IFILEMGR_OpenFile(pIFilemgr, fileName,_OFM_READ);

  if(pIFile == NULL)
  {
    (void)IFILEMGR_Release(pIFilemgr);
    return FALSE;
  }
  size = IFILE_Read(pIFile, data, (uint32)size);

  (void)IFILE_Release(pIFile);
  (void)IFILEMGR_Release(pIFilemgr);

  return size;
} /* WMSAPPMN_ConvertUserMelodyToEMS */

#endif/*FEATURE_SMS_UDH && !FEATURE_SMS_EXTENDED*/

/*===========================================================================
FUNCTION WMSAPPMD_DisplayOtherInfo

DESCRIPTION
  Displays Message Parameters.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void WMSAPPMD_DisplayOtherInfo
(
CWMSAPP *pMe,
AECHAR *psContents
)
{

  static char pText[WMSAPP_OTHER_INFO_TEXT_LENGTH];

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
  wms_client_bd_s_type        *cdma_data = & pMe->m_pClient_ts->u.cdma;
#endif /* FEATURE_CDSMS */
  int offset = 0;

  if(pMe == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return;
  }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(pMe->m_modeControl == WMSAPP_MODE_GW)
  {
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */
  }
  else
  {
#ifdef FEATURE_CDSMS
    /* Initialize other to 0 */
    (void)MEMSET( &pMe->m_pClient_ts->u.cdma.other, 0, sizeof(wms_other_parm_s_type) );

    if((pMe->m_msgInfo.boxType == WMSAPP_INBOX) &&
       (IWMS_TsDecode(pMe->m_pwms, &pMe->m_pClient_msg->u.cdma_message.raw_ts,
                       pMe->m_pClient_ts ) != SUCCESS))
    {
      MSG_ERROR("Decoding failed!",0,0,0);
      return;
    }

    offset = WMSAPPU_FormatTeleservice(pMe->m_pClient_msg->u.cdma_message.teleservice,
                                       (uint8*)pText+offset);

    pText[offset] = '\n';

    offset ++;

    offset += WMSAPPU_FormatCDMADcs(cdma_data->user_data.encoding,
                                    (uint8*)pText+offset);

    pText[offset] = '\n';

    offset ++;

    /* display validities if available */
    if(pMe->m_pClient_ts->u.cdma.mask & WMS_MASK_BD_VALID_REL)
    {
      char *str1 = "RELATIVE VALIDITY:";
      (void)STRLCPY(pText + offset, str1, WMSAPP_OTHER_INFO_TEXT_LENGTH-offset);

      offset += (int)STRLEN(str1);

      offset += WMSAPPU_FormatTimestamp( pMe, & cdma_data->validity_relative,
                                         (uint8*)pText+offset, WMSAPP_OTHER_INFO_TEXT_LENGTH - offset);
      pText[offset] = '\n';

      offset ++;

    }

    if(pMe->m_pClient_ts->u.cdma.mask & WMS_MASK_BD_VALID_ABS)
    {
      char *str1 = "ABSOLUTE VALIDITY:";
      (void)STRLCPY(pText + offset, str1, WMSAPP_OTHER_INFO_TEXT_LENGTH-offset);

      offset += (int)STRLEN(str1);

      offset += WMSAPPU_FormatTimestamp( pMe, & cdma_data->validity_absolute,
                                         (uint8*)pText+offset,WMSAPP_OTHER_INFO_TEXT_LENGTH - offset);
      pText[offset] = '\n';

      offset ++;

    }

    if(pMe->m_pClient_ts->u.cdma.mask & WMS_MASK_BD_DEFER_REL)
    {
      char *str1 = "DEFFERED RELATIVE:";
      (void)STRLCPY(pText + offset, str1, WMSAPP_OTHER_INFO_TEXT_LENGTH-offset);

      offset += (int)STRLEN(str1);

      offset += WMSAPPU_FormatTimestamp( pMe, & cdma_data->deferred_relative,
                                         (uint8*)pText+offset, WMSAPP_OTHER_INFO_TEXT_LENGTH - offset );
      pText[offset] = '\n';

      offset ++;

    }

    if(pMe->m_pClient_ts->u.cdma.mask & WMS_MASK_BD_DEFER_ABS)
    {
      char *str1 = "DEFFERED ABSOLUTE:";
      (void)STRLCPY(pText + offset, str1, WMSAPP_OTHER_INFO_TEXT_LENGTH-offset);

      offset += (int)STRLEN(str1);

      offset += WMSAPPU_FormatTimestamp( pMe, & cdma_data->deferred_absolute,
                                         (uint8*)pText+offset, WMSAPP_OTHER_INFO_TEXT_LENGTH - offset );
      pText[offset] = '\n';

      offset ++;

    }

    /* display privacy */
    if(pMe->m_pClient_ts->u.cdma.mask & WMS_MASK_BD_PRIVACY)
    {
      offset += WMSAPPU_FormatPrivacy(cdma_data->privacy,
                                      (uint8*)pText+offset);
      pText[offset] = '\n';

      offset ++;

    }

    if (pMe->m_pClient_ts->u.cdma.mask & WMS_MASK_BD_ALERT)
    {
      offset += WMSAPPU_FormatAlertMode(pMe->m_pClient_ts->u.cdma.alert_mode,
                                        (uint8*)pText+offset);
    }

    if (pMe->m_pClient_ts->u.cdma.mask & WMS_MASK_BD_DISPLAY_MODE)
    {
      if(pMe->m_pClient_ts->u.cdma.mask & WMS_MASK_BD_ALERT)
      {
        (void)MEMCPY(pText+offset, " ", 1);
        offset += 1;
      }

      offset += WMSAPPU_FormatDisplayMode(pMe->m_pClient_ts->u.cdma.display_mode,
                                          (uint8*)pText+offset);
      //pText[offset++] = '\n';
    }

    if((pMe->m_pClient_ts->u.cdma.mask & WMS_MASK_BD_DISPLAY_MODE)
       || (pMe->m_pClient_ts->u.cdma.mask & WMS_MASK_BD_ALERT)
      )
    {
      pText[offset++] = '\n';
    }

    offset += WMSAPPU_FormatAckInfo(pMe->m_pClient_ts->u.cdma.mask,
                                    pMe->m_pClient_msg->u.cdma_message.is_tl_ack_requested,
                                    cdma_data->reply_option.delivery_ack_requested,
                                    cdma_data->reply_option.user_ack_requested,
                                    cdma_data->reply_option.read_ack_requested,
                                    (byte *)pText+offset);

    pText[offset] = '\n';

    offset ++;

    /* display language */
    if(pMe->m_pClient_ts->u.cdma.mask & WMS_MASK_BD_LANGUAGE)
    {
      (void)WMSAPPU_FormatLanguage(cdma_data->language,
                             (uint8 *)pText+offset);
      pText[offset] = '\n';

      offset ++;

    }
#endif /* FEATURE_CDSMS */
  }

/* --- DONE with text formatting --- */

  pText[offset++] = '\0';
  (void)STRTOWSTR(pText,psContents,(int)((int)sizeof(AECHAR) * (int)offset));

} /* WMSAPPMD_DisplayOtherInfo() */


/*===========================================================================
FUNCTION WMSAPPMD_DisplayTemplate

DESCRIPTION
  Displays Template Parameters.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void WMSAPPMD_DisplayTemplate
(
  CWMSAPP *pMe,
  AECHAR *psContents,
  wms_client_message_s_type  *pMsg
)
{
#ifdef FEATURE_CDSMS
  wms_cdma_template_s_type    *cd_temp = & pMsg->u.cdma_template;
#endif /*FEATURE_CDSMS*/
#ifdef FEATURE_GWSMS
#error code not present
#endif
  int offset = 0;
  char *pText;
  char tempChar;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(pMe == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return;
  }

  pText = MALLOC(TEXT_BUFFER);

  if(pText == NULL)
  {
    MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
    return;
  }

  (void)MEMSET(pText,'\0',TEXT_BUFFER);

  /* Initialize text display line length and */
  /* call back number length.                */
  pMe->m_msgInfo.memStore  = pMsg->msg_hdr.mem_store;
  pMe->m_msgInfo.smsIndex  = (byte)pMsg->msg_hdr.index;

  /* show tag */
  switch ( pMsg->msg_hdr.mem_store )
  {
    case WMS_MEMORY_STORE_NV_CDMA:
  case WMS_MEMORY_STORE_NV_GW:
      WMSAPPU_WriteToBuffer(pText, (int)(TEXT_BUFFER), "NV Config", (int)STRLEN("NV Config"), &offset);
      break;

    case WMS_MEMORY_STORE_RUIM:
    case WMS_MEMORY_STORE_SIM:
      WMSAPPU_WriteToBuffer(pText, (int)(TEXT_BUFFER), "TEMPLATE #", (int)STRLEN("TEMPLATE #"), &offset);
      tempChar = (byte)pMsg->msg_hdr.index/10 + '0';
      WMSAPPU_WriteToBuffer(pText, (int)(TEXT_BUFFER), &tempChar, 1, &offset);
      tempChar = (byte)pMsg->msg_hdr.index%10 + '0';
      WMSAPPU_WriteToBuffer(pText, (int)(TEXT_BUFFER), &tempChar, 1, &offset);      
      break;

    default:
      MSG_ERROR("Invalid mem_store %d", pMsg->msg_hdr.mem_store, 0,0);
      break;
  }

  WMSAPPU_WriteToBuffer(pText, (int)(TEXT_BUFFER), "\n\n", 2, &offset);  

  if(pMe->m_modeControl == WMSAPP_MODE_CDMA)
  {
#ifdef FEATURE_CDSMS
    offset += WMSAPPU_FormatTeleservice(cd_temp->teleservice,
                                        (uint8*)pText + offset);

    /* display TL ack bool */
    (void)STRLCPY(pText+offset,"       TL ACK: ",TEXT_BUFFER-offset);
    offset += (int)STRLEN("       TL ACK: ");

    if ( cd_temp->mask & WMS_CDMA_TEMPLATE_MASK_BEARER_REPLY_OPTION )
    {
      pText[offset++] = 'T';
    }
    else
    {
      pText[offset++] = 'F';
    }

    pText[offset++] = '\n';

    if (cd_temp->mask & WMS_CDMA_TEMPLATE_MASK_DEST_ADDR)
    {
      (void)STRLCPY(pText+offset,"DEST ADDR: ",TEXT_BUFFER-offset);
      offset += (int)STRLEN("DEST ADDR: ");

      /* Check if this is a non-std email formatted address before displaying */
      if((cd_temp->address.digit_mode == WMS_DIGIT_MODE_4_BIT) &&
         (WMSAPPU_IsNonStdEmail(TRUE, (uint8*)cd_temp->address.digits,
                               cd_temp->address.number_of_digits)))
      {
        /* Set the flag to acknowledge a non-std email address format is used */
        pMe->m_emailAddr.isNonStdEmail = TRUE;

        /* Unpack to get the user data */
        (void)IWMS_TsUnpackAscii(pMe->m_pwms, &cd_temp->client_bd.user_data,
                            cd_temp->client_bd.user_data.number_of_digits+1,
                            (uint8*)pMe->m_buf);

        /* Get the email address to display */
        offset += WMSAPPU_GetNonStdEmail((uint8*)pMe->m_buf,
                                 (uint8*)pText+offset);

      }
      else
      {
        offset += WMSAPPU_FormatAddress(pMe,TRUE,
                                        &cd_temp->address,
                                        (uint8*)pText+offset,
                                        TEXT_BUFFER - offset );
      }
      pText[offset++] = '\n';
    }

    if (cd_temp->mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA)
    {
#if 0
      if (cd_temp->client_bd.mask & WMS_MASK_BD_REPLY_OPTION)
      {
        /* display delivery ack bool */
        (void)STRLCPY(pText+offset,"DEL ACK:",TEXT_BUFFER-offset);
        offset += (int)STRLEN("DEL ACK:");

        if ( cd_temp->client_bd.reply_option.delivery_ack_requested )
        {
          pText[offset++] = 'T';
        }
        else
        {
          pText[offset++] = 'F';
        }

        pText[offset++] = '\n';

        /* display user ack bool */
        (void)STRLCPY(pText+offset,"USER ACK:",TEXT_BUFFER-offset);
        offset += (int)STRLEN("USER ACK:");


        if ( cd_temp->client_bd.reply_option.user_ack_requested )
        {
          pText[offset++] = 'T';
        }
        else
        {
          pText[offset++] = 'F';
        }

        pText[offset++] = '\n';
      }
#endif

      if (cd_temp->client_bd.mask & WMS_MASK_BD_CALLBACK)
      {
        (void)STRLCPY(pText+offset,"CALLBACK: ",TEXT_BUFFER-offset);
        offset += (int)STRLEN("CALLBACK: ");

        offset += WMSAPPU_FormatAddress(pMe, TRUE, &cd_temp->client_bd.callback,
                                        (uint8*)pText+offset,
                                        TEXT_BUFFER - offset);
        pText[offset++] = '\n';
      }

      if (cd_temp->client_bd.mask & WMS_MASK_BD_PRIORITY)
      {
        offset += WMSAPPU_FormatPriority(cd_temp->client_bd.priority,
                                         (uint8*)pText+offset);
      }

      if (cd_temp->client_bd.mask & WMS_MASK_BD_PRIVACY)
      {
        if(cd_temp->client_bd.mask & WMS_MASK_BD_PRIORITY)
        {
          (void)MEMCPY(pText+offset, "   ", 3);
          offset += 3;
        }
        offset += WMSAPPU_FormatPrivacy(cd_temp->client_bd.privacy,
                                        (uint8*)pText+offset);
      }

      if((cd_temp->client_bd.mask & WMS_MASK_BD_PRIORITY) ||
         (cd_temp->client_bd.mask & WMS_MASK_BD_PRIVACY))
      {
        pText[offset++] = '\n';
      }

      if (cd_temp->client_bd.mask & WMS_MASK_BD_ALERT)
      {
        offset += WMSAPPU_FormatAlertMode(cd_temp->client_bd.alert_mode,
                                          (uint8*)pText+offset);
        //pText[offset++] = '\n';
      }

      if (cd_temp->client_bd.mask & WMS_MASK_BD_DISPLAY_MODE)
      {
        if(cd_temp->client_bd.mask & WMS_MASK_BD_ALERT)
        {
          (void)MEMCPY(pText+offset, " ", 1);
          offset += 1;
        }

        offset += WMSAPPU_FormatDisplayMode(cd_temp->client_bd.display_mode,
                                            (uint8*)pText+offset);
        //pText[offset++] = '\n';
      }

      if((cd_temp->client_bd.mask & WMS_MASK_BD_DISPLAY_MODE)
         || (cd_temp->client_bd.mask & WMS_MASK_BD_ALERT)
        )
      {
        pText[offset++] = '\n';
      }

      if (cd_temp->client_bd.mask & WMS_MASK_BD_LANGUAGE)
      {
        /* display language */
        offset += WMSAPPU_FormatLanguage(cd_temp->client_bd.language,
                                         (uint8*)pText+offset);
        pText[offset++] = '\n';
      }
    }

    if (cd_temp->mask & WMS_CDMA_TEMPLATE_MASK_DCS)
    {
      offset += WMSAPPU_FormatCDMADcs(cd_temp->dcs,
                                      (uint8*)pText+offset);
      pText[offset++] = '\n';
    }
    else if((cd_temp->mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA) &&
            (cd_temp->client_bd.mask & WMS_MASK_BD_USER_DATA))
    {
      offset += WMSAPPU_FormatCDMADcs(cd_temp->client_bd.user_data.encoding,
                                      (uint8*)pText+offset);
      pText[offset++] = '\n';
    }

    if (cd_temp->mask & WMS_CDMA_TEMPLATE_MASK_BEARER_DATA)
    {
      /* Don't display user data if there is no data to display */
      if ((cd_temp->client_bd.mask & WMS_MASK_BD_USER_DATA) &&
          (cd_temp->client_bd.user_data.data_len > 0))
      {
        /* Display user data iff no email or using email and text is available*/
        if(!(pMe->m_emailAddr.isNonStdEmail) ||
           (pMe->m_emailAddr.isNonStdEmail &&
            (cd_temp->client_bd.user_data.data_len > pMe->m_emailAddr.len+1)))
        {
          (void)STRLCPY(pText+offset,"USER DATA: ",TEXT_BUFFER-offset);
          offset += (int)STRLEN("USER DATA: ");
        }

        /* Display user data */
        /* TBD: Unicode & GSM 7-bit */

        if ( cd_temp->client_bd.user_data.encoding == WMS_ENCODING_ASCII ||
             cd_temp->client_bd.user_data.encoding == WMS_ENCODING_IA5 )
        {
          byte *tempBuf = MALLOC(WMS_CDMA_USER_DATA_MAX);

          if(tempBuf == NULL)
          {
            MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
            WMSAPPU_FREE(pText);
            return;
          }

          cd_temp->client_bd.user_data.data_len =
            IWMS_TsUnpackAscii( pMe->m_pwms, &cd_temp->client_bd.user_data,
                                (byte)cd_temp->client_bd.user_data.number_of_digits+1,
                                tempBuf );
          if(cd_temp->client_bd.user_data.data_len > 15)
          {
            tempBuf[13] = '.'; tempBuf[14] = '.'; tempBuf[15] = '.';
            (void)MEMCPY(pText+offset, tempBuf, 16);
            offset += 16;
          }
          else
          {
            (void)MEMCPY(pText+offset, tempBuf, (uint16)cd_temp->client_bd.user_data.data_len);
            offset += cd_temp->client_bd.user_data.data_len;
          }
          WMSAPPU_FREE(tempBuf);
        }
        else
        {
          if(cd_temp->client_bd.user_data.data_len > 15)
          {
            (void)MEMCPY(pText+offset, cd_temp->client_bd.user_data.data, 13);
            (void)MEMCPY(pText+offset+13, "...", 3);
            offset += 16;
          }
          else
          {
            (void)MEMCPY(pText+offset, cd_temp->client_bd.user_data.data,
                   cd_temp->client_bd.user_data.data_len);
            offset += cd_temp->client_bd.user_data.data_len;
          }
        }
      }
    }
#endif /*FEATURE_CDSMS*/
  }
  else
  {
#ifdef FEATURE_GWSMS
#error code not present
#endif /*FEATURE_GWSMS*/
  }  
  
  WMSAPPU_WriteToBuffer(pText, (int)(TEXT_BUFFER), "\0", 1, &offset);  

  (void)STRTOWSTR(pText,psContents,(int)((int)sizeof(AECHAR) * (int)offset));

  WMSAPPU_FREE(pText);

} /* WMSAPPMD_DisplayTemplate*/

/*===========================================================================
FUNCTION WMSAPPMD_DisplayMemoryUsage

DESCRIPTION
  Displays Memory Usage for each memory Store.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMD_DisplayMemoryUsage
( CWMSAPP *pMe,
  AECHAR *psContents
)
{
  char *pText = MALLOC(TEXT_BUFFER);
  char *buf ="";
  int offset =0, length =0;
  
  if(pMe == NULL)
  {
    MSG_ERROR("pMe = NULL",0,0,0);
    return FALSE;
  }

  if(pText == NULL)
  {
    MSG_ERROR("Buffer Could Not be Allocated",0,0,0);
    return FALSE;
  }

  switch(pMe->m_msgInfo.memStore)
  {
    case WMS_MEMORY_STORE_RAM_CDMA:
    case WMS_MEMORY_STORE_RAM_GW:
      buf = "TEMP MEMORY";
      break;

    case WMS_MEMORY_STORE_SIM:
    case WMS_MEMORY_STORE_RUIM:
      buf = "CARD MEMORY";
      break;

    case WMS_MEMORY_STORE_NV_CDMA:
    case WMS_MEMORY_STORE_NV_GW:
      buf = "PHONE MEMORY";
      break;

    default:
      break;
  }

  length = (int)STRLEN(buf);
  (void)MEMCPY(pText,buf,(uint16)length);
  offset = length;

  pText[offset++] = '\n';
  pText[offset++] = '\n';

  buf = "No.of Slots used:";
  length = (int)STRLEN(buf);
  (void)MEMCPY(pText+offset,buf,(uint16)length);
  offset += length;

  /* Used Slots */
  if (WMS_MEMORY_STORE_MAX <= pMe->m_msgInfo.memStore) {
     return FALSE;
  }
  offset += WMSAPPU_ITOA( pMe->m_pMemStatuses[pMe->m_msgInfo.memStore]->max_slots - pMe->m_pMemStatuses[pMe->m_msgInfo.memStore]->free_slots, pText+offset, TEXT_BUFFER - offset, 10);
  pText[offset++] = '\n';


  buf = "No.of Slots available:";
  length = (int)STRLEN(buf);
  (void)MEMCPY(pText+offset,buf,(uint16)length);
  offset += length;

  /* Free Slots */
  offset += WMSAPPU_ITOA( pMe->m_pMemStatuses[pMe->m_msgInfo.memStore]->free_slots, pText+offset, TEXT_BUFFER - offset, 10);
  pText[offset++] = '\n';

  buf = "Total number of Slots:";
  length = (int)STRLEN(buf);
  (void)MEMCPY(pText+offset,buf,(uint16)length);
  offset += length;

  /* Max Slots */
  offset += WMSAPPU_ITOA( pMe->m_pMemStatuses[pMe->m_msgInfo.memStore]->max_slots, pText+offset, TEXT_BUFFER - offset, 10);
  pText[offset++] = '\n';

  /* Special Handling for Status Report Usage Display
     if memStore = WMS_MEMORY_STORE_SIM */
  /* Also Dont display Status Report usage if Status Report
     is not supported i.e. max slots = 0 */

#ifdef FEATURE_GWSMS_STATUS_REPORTS
#error code not present
#endif /* FEATURE_GWSMS_STATUS_REPORTS */

  pText[offset++] = '\0';
  (void)STRTOWSTR(pText,psContents,(int)((int)sizeof(AECHAR) * (int)offset));
  WMSAPPU_FREE(pText);

  return TRUE;
}

/*===========================================================================
FUNCTION WMSAPPMD_DisplayClassSummary

DESCRIPTION
  Displays the summary of all the class Settings.
  It is applicable only to GW

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean WMSAPPMD_DisplayClassSummary
( CWMSAPP *pMe,
  AECHAR *psContents
)
{
#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

  return TRUE;
}

#ifdef FEATURE_GWSMS_STATUS_REPORTS
#error code not present
#endif /*FEATURE_GWSMS_STATUS_REPORTS*/

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

#endif
/*========================End of File ================================================*/

