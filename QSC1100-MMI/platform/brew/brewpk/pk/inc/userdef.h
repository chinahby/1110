/*
//---------------------------------------------------------------------------
// Arphic Bitmap Font API  ver $Name: le_v02150500_20080717 $
//
//  API function prototype
//     (API user should include this header file)
//
// Copyright (C) 2001-2008 Arphic Technology Co., Ltd.
// All rights reserved.
//
//---------------------------------------------------------------------------
// API Version 2.11  (for 16/32 bits compiling)
//---------------------------------------------------------------------------
*/
#ifndef  __USERDEF1__
#define __USERDEF1__




#include "apidef.h"

#ifndef __RBMP_NO_LAYOUTENGINE__
/*ALE*/
#include "aleuser.h"
#endif


#ifdef __cplusplus
extern "C"
 {
#endif

   /* Initialization
      -------------------------------- */
   ALE_INT16 RbmpReset (char *AWorkBuf ,ALE_UINT16 bufSize, void *RbmpGetDataProcInstance,
                          void (*ARbmpGetDataProc)   (void *RbmpInstance, ALE_UINT32 rbmOffs,
                               void *dataBuf, ALE_UINT16 dataLen));
                          /*
                             If your font is in memory,
                             you can and set RbmpGetDataProcInstance to its start address
                             and set ARbmpGetDataProc to NULL.

                             You can use RbmpGetWorkBufferSize () to decide working buffer size.
                           */

         ALE_INT32 RbmpGetWorkBufferSize (void); /* return minimum working buffer size */


   /* Select Font to Use
      -------------------------------- */
   ALE_INT32  RbmpSetFont (struct FONTTYPEFACE ftf0, char *WorkBuf);
   ALE_INT32  RbmpSetFontMore (struct FONTTYPEFACE ftf0, ALE_INT16 index, char *WorkBuf);
   ALE_INT32  RbmpSetFontBySerial (ALE_INT16 index, char *WorkBuf); /* index = 0 means the first font */


   /* Get Font
      -------------------------------- */
   ALE_INT16 RbmpGetFont (ALE_INT16 tcodetype, ALE_UINT16 charCod,
                                char *tFontBuf ,struct FONTMETRICS *fm0, char *WorkBuf);




   /* Get Information about Font Data
      ------------------------------------------------ */
   ALE_INT16 RbmpQueryFont (struct FONTTYPEFACE ftf, char *WorkBuf);
   ALE_INT16 RbmpQueryFontMore (struct FONTTYPEFACE ftf, ALE_INT16 index, char *WorkBuf);

   ALE_INT16 RbmpGetFTypeface (ALE_INT16 fidx ,struct FONTTYPEFACE *ftf0, char *WorkBuf);
   ALE_INT16 RbmpGetFFormat (ALE_INT16 fidx, ALE_UINT16 *FullFontFormat,
                                   ALE_UINT16 *HalfFontFormat, char *WorkBuf);

    /* Checking Current Character Format (Full-Size only)
       ---------------------------------------------------------------- */
    ALE_INT32 Rbmp_UsingProportionalFormat (ALE_INT16 tcodetype, ALE_UINT16 charCod, char *WorkBuf);
    ALE_INT32 Rbmp_UsingRowScanFormat (ALE_INT16 tcodetype, ALE_UINT16 charCod, char *WorkBuf);
    ALE_INT32 Rbmp_UsingColumnScanFormat (ALE_INT16 tcodetype, ALE_UINT16 charCod, char *WorkBuf);
    ALE_INT32 Rbmp_UsingFontWithExtendMetrics (ALE_INT16 tcodetype, ALE_UINT16 charCod, char *WorkBuf);

    ALE_UINT32 RbmpGetWorkingInfo (ALE_INT32 type, char *WorkBuf);

   /* Layout Engine
      -------------------------------- */
   #ifndef __RBMP_NO_LAYOUTENGINE__
   #define RBMP_INNER_BIDI_BUFFER_MAXLEN   ALE_INNER_BIDI_BUFFER_MAXLEN

   ALE_INT16 RbmpGetStringFont (ALE_INT16 tcodetype, ALE_UINT16 *AString,
                                char *tFontBuf,
                                ALE_INT16 ABufferRowByteTotal, ALE_INT16 ABufferColumnByteTotal,
                                struct FONTMETRICS *fm0,
                                char *tCharFontBuf,  ALE_INT32 ACharSpace,
                                ALE_UINT16 *ACharPosition,
                                char *WorkBuf);
                              /*
                                 NOTE: to support Arabic:
                                   1. remember to active BiDirecition Text algorithm
                                       by SetTextDirection ()

                                   2. if your Unicode string will be longer than
                                      RBMP_INNER_BIDI_BUFFER_MAXLEN
                                      please given re-order buffer by
                                      SetBiDiReorderBuffer (),
                                      otherwise ERROR_BIDI_INSUFFICIENT_BUFFER
                                      will be returned.

                                   3. you can use RbmpLasOutputStringClipped ()
                                      to see if your string is too long for the buffer.
                               */

     ALE_INT32 RbmpLastOutputStringClipped (char *WorkBuf);
                              /*
                                   if last string sent to RbmpGetStringFont () is too long for
                                    buffer, it returns the order of character clipped
                                     (1 means displayed 0 chars, cut at 1st char
                                      2 means displayed 1 characters, cut at 2nd char...etc)
                                    otherwise it returns 0
                               */

     void RbmpChangeGetFontFunction
                     (ALE_INT16 (*NewGetFont)
                              (ALE_INT16 tcodetype, ALE_UINT16 charCod,
                               char *tFontBuf ,struct FONTMETRICS *fm0,
                               char *WorkBuf,
                               ALE_INT32 AFullCharWidth, ALE_INT32 AFullCharHeight,
                               ALE_INT32 AHalfCharWidth, ALE_INT32 AHalfCharHeight,
                               ALE_INT32 AFontSerial),
                       char *WorkBuf);
                 /*
                     Give a function (*NewGetFont) () to override original RbmpGetFont ()
                      when executing RbmpGetStringFont (). So you can use your
                      font in RbmpGetStringFont ()'s layout engine.

                       Please define it with extern "C" if you are using C++.

                       (*NewGetFont) ()'s parameters are the same as
                        RbmpGetFont (), except:
                       AFullCharWidth/AFullCharHeight/AHalfCharWidth/AHalfCharHeight
                       is the width and height of current (full and half) font in pixels.

                       AFontSerial is current font's serial (0 means the first font)
                       you can use RbmpGetFTypeface (AFontSerial...) to get the
                       information of font currently used.
                       Please beware of that, tFontBuf may be NULL if caller only
                       want character's metrics.

                      (*NewGetFont) ()'s operation and return value should be
                      the same as RbmpGetFont ().

                     You can call RbmpGetFont () in your (*NewGetFont) ()
                     if you like to use original font of API.

                     Note: this function has no effect on RbmpGetFont ().

                     Send NULL if you do not want to use external function anymore.
                  */


   /* Bi-Direction Support
      -------------------------------- */
   ALE_INT16 RbmpSetTextDirection (ALE_INT16 type, char *WorkBuf);
                              /*
                                  type 0 (default): Always Left-to-right, no right-to-left
                                  type 1: default left-to-right; right-to-left for Arabic
                                  type 2: default right-to-left, left-to-right for English

                                  set type to 1 or 2 if you need Arabic support
                               */

   ALE_UINT16 *RbmpGetLastBiDiReorderBuffer (char *WorkBuf);
                              /*
                                 Get last RbmpGetStringFont ()'s display string
                                   NULL: no re-order occurred or insufficient reorder buffer
                                   non-NULL: the returned buffer contains rendered
                                      string (ended by zero) and then the original
                                      order (1 means originally the first char)
                               */

   void RbmpSetBiDiReorderBuffer (ALE_UINT16 *ABuffer, ALE_INT32 ABufferSize, char *WorkBuf);
                        /*
                            buffer size should be >=
                              (max string length + 1) * 3 * sizeof (ALE_UINT16)
                            give ABuffer = NULL means using internal buffer
                         */

   /* Cursor Operation Support

       Please use these functions immediately after
       RbmpGetStringFont ()
      ------------------------------------------------ */
   ALE_INT32 RbmpGetCharIndex (ALE_INT32 x, ALE_UINT16 *ALastCharPosition, char *WorkBuf);
             /*
                  Get the index of character at horizontal position x for last time you call
                   RbmpGetStringFont ().

                  return value: the order of character where x is on.
                   1 means x is on the first character (i.e. AString [0])
                   2 means x is on the second (i.e. AString [1])
                   ....etc.
                   0 means not on any character.

                  ALastCharacterPosition: returned ACharPos []
                        last time you call RbmpGetStringFont () (cannot be NULL)

                  NOTE: please use this function immediately after calling
                    RbmpGetStringFont ()
              */

   ALE_INT32 RbmpGetDisplayOrder (ALE_INT32 index, char *WorkBuf);
             /*
                 Get the display order of the (index)th character last time
                   RbmpGetStringFont () is called.

                 return: order: 1 means the first, 2 means the second...etc.
                 NOTE: if index is out of bound, 0 is returned.

                 in general, the returned value is index. However,
                 when Arabic or BiDirection text renderred,
                 the returned value may be different.

              */

   ALE_INT32 RbmpGetDisplayDirection (ALE_INT32 index, char *WorkBuf);
              /*
                 Get the display direction of the (index)th character last time
                   RbmpGetStringFont () is called.

                 return:  "Even number" (including 0) means this character
                   is displayed in left-to-right order;
                   "Odd number" means this character is displayed in
                   "right-to-left" order.
                 Note: if index is out of range, current base text direction
                 is used for judge.
               */

   ALE_INT32 RbmpGetDisplayLeft  (ALE_INT32 index, ALE_UINT16 *ALastCharPosition, char *WorkBuf);
   ALE_INT32 RbmpGetDisplayRight  (ALE_INT32 index, ALE_UINT16 *ALastCharPosition, char *WorkBuf);
             /*
                 Get the horizontal display boundary (left/right)
                   of the (index)th character in last time
                   RbmpGetStringFont () is called.
                   i.e. AString [index-1] when calling RbmpGetStringFont ()

                 NOTE
                        "left" boundary is at the first pixel column of this character,
                    while "right" boundary is at the first pixel column of next character.

                 return -1 if out of range.
              */

   ALE_INT32 RbmpGetCursorPrevIndex (ALE_INT32 index, ALE_UINT16 *ALastCharPosition, char *WorkBuf);
   ALE_INT32 RbmpGetCursorNextIndex (ALE_INT32 index, ALE_UINT16 *ALastCharPosition, char *WorkBuf);
             /*
                 when cursor is on the index(th) character,
                  (i.e. AString [index-1] when last time ecalling
                    RbmpGetStringFont ())
                  which character should be skipped to when
                  moving cursor previously or next.

                 return: 1 means the first character,
                   2 means the second...etc

                 you can send returned value to RbmpGetDisplayLeft ()
                 to get new cursor position.
              */


    ALE_INT16 RbmpSetTab (ALE_UINT16 tabval, char *WorkBuf);

    ALE_UINT16 *RbmpBiDiReOrder
                  (ALE_UINT16 *AString, ALE_UINT16 *ABuffer,
                   ALE_INT32 ABaseLevel,  /* -1 means decide by program */
                   ALE_INT32 AMaxStringLength, char *WorkBuf);
    ALE_UINT16 *RbmpBiDiReOrder2
                  (ALE_UINT16 *AString, ALE_UINT16 *ABuffer,
                   ALE_INT32 ABaseLevel,  /* -1 means decide by program */
                   ALE_INT32 AMaxStringLength, char *WorkBuf);



    /*ALE*/
    AleFontDriverInterface *RbmpGetAleInterface (void);

    char *RbmpGetAleWorkBuffer (char *WorkBuf);
    void RbmpSetAleWorkBuffer (char *AleWorkBuf, char *WorkBuf);

    void RBMPMetricsToALE (struct FONTMETRICS *rfm, AleFontMetrics *afm);
    void ALEMetricsToRBMP (AleFontMetrics *afm, struct FONTMETRICS *rfm);

   #endif /* __RBMP_NO_LAYOUTENGINE__ */


#ifdef __cplusplus
  };
#endif


/*=====================================*/

#endif



