/*

       Arphic Layout Engine    ver $Name: le_v02150500_20080717 $
           Copyright (C) 2001-2008 Arphic Technology Co., Ltd. All rights reserved.

         include file for user

 */

#ifndef  __ARPHIC_ALEUSER_H__
#define  __ARPHIC_ALEUSER_H__

#define LeVersion "$Name: le_v02150500_20080717 $"

/* .............................Below code is controlled by source controller, do not modify */
/* @@_ALE_AUTO_SOURCE LANGUAGE */

/* partial */

/* Hindi */



/* Bengali */



/* Tamil */



/* Sinhala */



/* Gurmukhi */



/* Gujarati */



/* Malayalam */



/* Telugu */



/* Kannada */



/* Tibetan */



/* Thai */



/* Myanmar */



 /* Arabic */



/* Arabic extend */



/* Hebrew */




 /* @@_ALE_AUTO_SOURCE - */
/* .............................Above code is controlled by source controller, do not modify */


   /* Language Control
      -------------------------------------------------------- */

                /* To make layout engine compatible with old customers,
                    we support original macros
                    ---------------------------------------------------------------- */
                  #ifdef __ARPHIC_BITMAPAPI_ALLLANGUAGE__
                    #ifndef __ARPHIC_LAYOUTENGINE_ALLLANGUAGE__
                    #define __ARPHIC_LAYOUTENGINE_ALLLANGUAGE__
                    #endif
                  #endif

                  #ifdef __ARPHIC_BITMAPAPI_PARTIALLANGUAGE__
                    #ifndef __ARPHIC_LAYOUTENGINE_PARTIALLANGUAGE__
                    #define __ARPHIC_LAYOUTENGINE_PARTIALLANGUAGE__
                    #endif
                  #endif

                  #ifdef __ARPHIC_BITMAPAPI_HINDI__
                    #ifndef __ARPHIC_LAYOUTENGINE_HINDI__
                    #define __ARPHIC_LAYOUTENGINE_HINDI__
                    #endif
                  #endif

                  #ifdef __ARPHIC_BITMAPAPI_BENGALI__
                    #ifndef __ARPHIC_LAYOUTENGINE_BENGALI__
                    #define __ARPHIC_LAYOUTENGINE_BENGALI__
                    #endif
                  #endif

                  #ifdef __ARPHIC_BITMAPAPI_TAMIL__
                    #ifndef __ARPHIC_LAYOUTENGINE_TAMIL__
                    #define __ARPHIC_LAYOUTENGINE_TAMIL__
                    #endif
                  #endif

                  #ifdef __ARPHIC_BITMAPAPI_SINHALA__
                    #ifndef __ARPHIC_LAYOUTENGINE_SINHALA__
                    #define __ARPHIC_LAYOUTENGINE_SINHALA__
                    #endif
                  #endif

                  #ifdef __ARPHIC_BITMAPAPI_GURMUKHI__
                    #ifndef __ARPHIC_LAYOUTENGINE_GURMUKHI__
                    #define __ARPHIC_LAYOUTENGINE_GURMUKHI__
                    #endif
                  #endif

                  #ifdef __ARPHIC_BITMAPAPI_GUJARATI__
                    #ifndef __ARPHIC_LAYOUTENGINE_GUJARATI__
                    #define __ARPHIC_LAYOUTENGINE_GUJARATI__
                    #endif
                  #endif

                  #ifdef __ARPHIC_BITMAPAPI_MALAYALAM__
                    #ifndef __ARPHIC_LAYOUTENGINE_MALAYALAM__
                    #define __ARPHIC_LAYOUTENGINE_MALAYALAM__
                    #endif
                  #endif

                  #ifdef __ARPHIC_BITMAPAPI_TELUGU__
                    #ifndef __ARPHIC_LAYOUTENGINE_TELUGU__
                    #define __ARPHIC_LAYOUTENGINE_TELUGU__
                    #endif
                  #endif

                  #ifdef __ARPHIC_BITMAPAPI_KANNADA__
                    #ifndef __ARPHIC_LAYOUTENGINE_KANNADA__
                    #define __ARPHIC_LAYOUTENGINE_KANNADA__
                    #endif
                  #endif

                  #ifdef __ARPHIC_BITMAPAPI_TIBETAN__
                    #ifndef __ARPHIC_LAYOUTENGINE_TIBETAN__
                    #define __ARPHIC_LAYOUTENGINE_TIBETAN__
                    #endif
                  #endif

                  #ifdef __ARPHIC_BITMAPAPI_THAI__
                    #ifndef __ARPHIC_LAYOUTENGINE_THAI__
                    #define __ARPHIC_LAYOUTENGINE_THAI__
                    #endif
                  #endif

                  #ifdef __ARPHIC_BITMAPAPI_MYANMAR__
                    #ifndef __ARPHIC_LAYOUTENGINE_MYANMAR__
                    #define __ARPHIC_LAYOUTENGINE_MYANMAR__
                    #endif
                  #endif

                  #ifdef __ARPHIC_BITMAPAPI_ARABIC__
                    #ifndef __ARPHIC_LAYOUTENGINE_ARABIC__
                    #define __ARPHIC_LAYOUTENGINE_ARABIC__
                    #endif
                  #endif

                  #ifdef __ARPHIC_BITMAPAPI_ARABIC_EXTEND__
                    #ifndef __ARPHIC_LAYOUTENGINE_ARABIC_EXTEND__
                    #define __ARPHIC_LAYOUTENGINE_ARABIC_EXTEND__
                    #endif
                  #endif

       /* Support Henry's Macro
          ---------------------------------------- */
       #ifdef __ARPHIC_LAYOUTENGINE_BENGALI__
           #if ( !defined (FOR_BENGALI_001) && !defined (FOR_BENGALI_002_E300))
              #define  FOR_BENGALI_001
              #define  FOR_BENGALI_002_E300
           #endif
       #else
           #if ( defined (FOR_BENGALI_001) || defined (FOR_BENGALI_002_E300))
             #ifndef   __ARPHIC_LAYOUTENGINE_BENGALI__
             #define   __ARPHIC_LAYOUTENGINE_BENGALI__
             #endif
           #endif
       #endif

       /* For convenience, if "All language" not defined, and any
           language is defined, automatically define "partial language"
          ------------------------------------------------------------------------ */
      #if  (!defined ( __ARPHIC_LAYOUTENGINE_ALLLANGUAGE__) &&  !defined ( __ARPHIC_LAYOUTENGINE_PARTIALLANGUAGE__) )
         #if   (  defined (__ARPHIC_LAYOUTENGINE_HINDI__) || defined (__ARPHIC_LAYOUTENGINE_THAI__) || defined (__ARPHIC_LAYOUTENGINE_ARABIC__) ||  defined (__ARPHIC_LAYOUTENGINE_BENGALI__)  )
            #define __ARPHIC_LAYOUTENGINE_PARTIALLANGUAGE__
         #else
            #if   (   defined (__ARPHIC_LAYOUTENGINE_TAMIL__) || defined (__ARPHIC_LAYOUTENGINE_HEBREW__) || defined (__ARPHIC_LAYOUTENGINE_SINHALA__) || defined (__ARPHIC_LAYOUTENGINE_MYANMAR__) )
              #define __ARPHIC_LAYOUTENGINE_PARTIALLANGUAGE__
            #else
              #if (   defined (__ARPHIC_LAYOUTENGINE_GURMUKHI__) || defined (__ARPHIC_LAYOUTENGINE_GUJARATI__)  ||  defined (__ARPHIC_LAYOUTENGINE_MALAYALAM__) ||  defined (__ARPHIC_LAYOUTENGINE_TIBETAN__)  )
              #define __ARPHIC_LAYOUTENGINE_PARTIALLANGUAGE__
              #else
                #if (defined (__ARPHIC_LAYOUTENGINE_TELUGU__) ||  defined (__ARPHIC_LAYOUTENGINE_KANNADA__) )
                #define __ARPHIC_LAYOUTENGINE_PARTIALLANGUAGE__
                #endif
              #endif
            #endif
         #endif
      #endif

      /* if no langauge is defined at all,
          enable all languages
         -------------------------------------------------------------- */
      #ifndef __ARPHIC_LAYOUTENGINE_PARTIALLANGUAGE__
      #ifndef __ARPHIC_LAYOUTENGINE_ALLLANGUAGE__
      #define __ARPHIC_LAYOUTENGINE_ALLLANGUAGE__
      #endif
      #endif

      #ifdef  __ARPHIC_LAYOUTENGINE_ALLLANGUAGE__
        #ifndef  __ARPHIC_LAYOUTENGINE_HINDI__
        #define  __ARPHIC_LAYOUTENGINE_HINDI__
        #endif
        #ifndef  __ARPHIC_LAYOUTENGINE_THAI__
        #define  __ARPHIC_LAYOUTENGINE_THAI__
        #endif
        #ifndef  __ARPHIC_LAYOUTENGINE_MYANMAR__
        #define  __ARPHIC_LAYOUTENGINE_MYANMAR__
        #endif
        #ifndef  __ARPHIC_LAYOUTENGINE_ARABIC__
        #define  __ARPHIC_LAYOUTENGINE_ARABIC__
        #endif
        #ifndef  __ARPHIC_LAYOUTENGINE_ARABIC_EXTEND__
        #define  __ARPHIC_LAYOUTENGINE_ARABIC_EXTEND__
        #endif
        #ifndef  __ARPHIC_LAYOUTENGINE_BENGALI__
        #define  __ARPHIC_LAYOUTENGINE_BENGALI__
            /* enable Henry's macro to avoid external font collision */
            #ifndef FOR_BENGALI_002_E300
            #define FOR_BENGALI_002_E300
            #endif
        #endif

        #ifndef  __ARPHIC_LAYOUTENGINE_TAMIL__
        #define  __ARPHIC_LAYOUTENGINE_TAMIL__
        #endif

        #ifndef  __ARPHIC_LAYOUTENGINE_SINHALA__
        #define  __ARPHIC_LAYOUTENGINE_SINHALA__
        #endif

        #ifndef  __ARPHIC_LAYOUTENGINE_GURMUKHI__
        #define  __ARPHIC_LAYOUTENGINE_GURMUKHI__
        #endif

        #ifndef  __ARPHIC_LAYOUTENGINE_GUJARATI__
        #define  __ARPHIC_LAYOUTENGINE_GUJARATI__
        #endif

        #ifndef  __ARPHIC_LAYOUTENGINE_MALAYALAM__
        #define  __ARPHIC_LAYOUTENGINE_MALAYALAM__
        #endif

        #ifndef  __ARPHIC_LAYOUTENGINE_TELUGU__
        #define  __ARPHIC_LAYOUTENGINE_TELUGU__
        #endif

        #ifndef  __ARPHIC_LAYOUTENGINE_KANNADA__
        #define  __ARPHIC_LAYOUTENGINE_KANNADA__
        #endif

        #ifndef  __ARPHIC_LAYOUTENGINE_TIBETAN__
        #define  __ARPHIC_LAYOUTENGINE_TIBETAN__
        #endif

        #ifndef  __ARPHIC_LAYOUTENGINE_HEBREW__
        #define  __ARPHIC_LAYOUTENGINE_HEBREW__
        #endif

        /* For Old Customers */
        #ifndef  __ARPHIC_BITMAPAPI_HINDI__
        #define  __ARPHIC_BITMAPAPI_HINDI__
        #endif
        #ifndef  __ARPHIC_BITMAPAPI_THAI__
        #define  __ARPHIC_BITMAPAPI_THAI__
        #endif
        #ifndef  __ARPHIC_BITMAPAPI_ARABIC__
        #define  __ARPHIC_BITMAPAPI_ARABIC__
        #endif

      #endif

      #ifdef __ARPHIC_NOARABIC__
        #ifdef  __ARPHIC_LAYOUTENGINE_ARABIC__
        #undef __ARPHIC_LAYOUTENGINE_ARABIC__
        #endif
      #endif

      #ifdef __ARPHIC_NOHINDI__
        #ifdef  __ARPHIC_LAYOUTENGINE_HINDI__
        #undef __ARPHIC_LAYOUTENGINE_HINDI__
        #endif
      #endif

      #ifdef __ARPHIC_NOTAMIL__
        #ifdef  __ARPHIC_LAYOUTENGINE_TAMIL__
        #undef __ARPHIC_LAYOUTENGINE_TAMIL__
        #endif
      #endif

      #ifdef __ARPHIC_NOSINHALA__
        #ifdef  __ARPHIC_LAYOUTENGINE_SINHALA__
        #undef __ARPHIC_LAYOUTENGINE_SINHALA__
        #endif
      #endif

      #ifdef __ARPHIC_NOGURMUKHI__
        #ifdef  __ARPHIC_LAYOUTENGINE_GURMUKHI__
        #undef __ARPHIC_LAYOUTENGINE_GURMUKHI__
        #endif
      #endif

      #ifdef __ARPHIC_NOGUJARATI__
        #ifdef  __ARPHIC_LAYOUTENGINE_GUJARATI__
        #undef __ARPHIC_LAYOUTENGINE_GUJARATI__
        #endif
      #endif

      #ifdef __ARPHIC_NOMALAYALAM__
        #ifdef  __ARPHIC_LAYOUTENGINE_MALAYALAM__
        #undef __ARPHIC_LAYOUTENGINE_MALAYALAM__
        #endif
      #endif

      #ifdef __ARPHIC_NOTELUGU__
        #ifdef  __ARPHIC_LAYOUTENGINE_TELUGU__
        #undef __ARPHIC_LAYOUTENGINE_TELUGU__
        #endif
      #endif

      #ifdef __ARPHIC_NOKANNADA__
        #ifdef  __ARPHIC_LAYOUTENGINE_KANNADA__
        #undef __ARPHIC_LAYOUTENGINE_KANNADA__
        #endif
      #endif

      #ifdef __ARPHIC_NOTIBETAN__
        #ifdef  __ARPHIC_LAYOUTENGINE_TIBETAN__
        #undef __ARPHIC_LAYOUTENGINE_TIBETAN__
        #endif
      #endif

      /* derived macros
         -------------------------------- */
      #if  (  defined ( __ARPHIC_LAYOUTENGINE_ARABIC__) || defined (__ARPHIC_LAYOUTENGINE_HEBREW__) )
      #ifndef  __ARPHIC_BIDI_SUPPORT__
      #define  __ARPHIC_BIDI_SUPPORT__
      #endif
      #endif


      #if   (  defined (__ARPHIC_LAYOUTENGINE_HINDI__) ||  defined (__ARPHIC_LAYOUTENGINE_BENGALI__)  || defined (__ARPHIC_LAYOUTENGINE_TAMIL__) || defined (__ARPHIC_LAYOUTENGINE_SINHALA__)  )
        #ifndef  __ARPHIC_LAYOUTENGINE_INDIC__
        #define  __ARPHIC_LAYOUTENGINE_INDIC__
        #endif
      #endif
      #if   (  defined (__ARPHIC_LAYOUTENGINE_GURMUKHI__) || defined (__ARPHIC_LAYOUTENGINE_GUJARATI__) || defined (__ARPHIC_LAYOUTENGINE_MALAYALAM__) || defined (__ARPHIC_LAYOUTENGINE_TIBETAN__)  )
        #ifndef  __ARPHIC_LAYOUTENGINE_INDIC__
        #define  __ARPHIC_LAYOUTENGINE_INDIC__
        #endif
      #endif
      #if   (  defined (__ARPHIC_LAYOUTENGINE_MYANMAR__) || defined (__ARPHIC_LAYOUTENGINE_TELUGU__) || defined (__ARPHIC_LAYOUTENGINE_KANNADA__)   )
        #ifndef  __ARPHIC_LAYOUTENGINE_INDIC__
        #define  __ARPHIC_LAYOUTENGINE_INDIC__
        #endif
      #endif



   /* Type. NOTE: modify with apidef.h, bitmap font system
      -------------------------------------------------------- */
   #ifndef ALE_DATATYPE
   #define ALE_DATATYPE
   #define ALE_CHAR    signed char
   #define ALE_UBYTE   unsigned char
   #define ALE_INT16   signed short
   #define ALE_UINT16  unsigned short
   #define ALE_INT32   signed long
   #define ALE_UINT32  unsigned long
   #endif



   /* Constants and Message Codes
      -------------------------------------------------------- */
    #define ALE_INNER_BIDI_BUFFER_MAXLEN 84

        /*
            NOTE: to make layout engine compatible with old customers,
             change constant with RBMP API (apidef.h)
         */
    #ifndef  ALE_SUCCESS_GETFONT
    #define ALE_SUCCESS_GETFONT    111
    #endif

    #define ALE_ERROR_NOFONT       -201
    #define ALE_ERROR_CODETYPE     -202
    #define ALE_ERROR_CODE         -203
    #define ALE_ERROR_FONTSIZE     -204
    #define ALE_ERROR_WORKBUFFER   -205
    #define ALE_ERROR_MAPTABLE     -206

    #define ALE_ERROR_BIDI_INSUFFICIENT_BUFFER -209
    #define ALE_ERROR_BIDI_TYPE_NUMBER  -210
    #define ALE_ERROR_BIDI_UNSUPPORTED -211
    #define ALE_ERROR_DATATYPE  -212
    #define ALE_ERROR_WORKBUFFER_32BITALIGN -215


    /* keep old error message code available*/
    #ifndef  SUCCESS_GETFONT
    #define SUCCESS_GETFONT    ALE_SUCCESS_GETFONT
    #endif
    #ifndef ERROR_BIDI_INSUFFICIENT_BUFFER
    #define ERROR_BIDI_INSUFFICIENT_BUFFER ALE_ERROR_BIDI_INSUFFICIENT_BUFFER
    #endif


      /* new error code start from -230 */
    #define ALE_ERROR_GETFONT  -230
    #define ALE_NOT_SUPPORTED   -231
    #define ALE_ILLEGAL_START   -232
    #define ALE_ILLEGAL_BUFFERSIZE   -233
    #define ALE_MISSING_CHAR  -234



   /* Data Structure
      -------------------------------------------------------- */
     typedef struct _AleFontMetrics
     {
         ALE_INT16   outBufWidth;
         ALE_INT16   outBufHeight;
         ALE_INT16   Ascender;
         ALE_INT16   OriginX;
         ALE_INT16   horiAdvance;

         /* for Hindi or other languages */
         ALE_INT16 Extend1;
         ALE_INT16 Extend2;

         ALE_INT16 Padding; /* make structure is 32-bit alignment */

     } AleFontMetrics;



                typedef struct _AleKernPairIndexType
                 {
                    ALE_UINT16  PrevChar, CurrChar;
                         /* WARNING: you may need to reverse byte order  */
                 } AleKernPairIndexType;

                typedef struct _AleKerningHeaderType
                 {
                   /* CAUTION:
                       1.Kerning Data may be burned in ROM,
                         so do not try to modify its contents.

                       2.Remember to check byte order yourself,
                         You may need to reverse byte order!
                    */

                   ALE_UINT32 CheckCode;
                   ALE_UINT32 TotalSize; /* total size of kerning data, including this header */
                   ALE_UINT32 Flag;
                   ALE_INT32 KernTotal;

                    /* Followed by IndexTable & XTable.

                        AleKernPairIndexType IndexTable [KernTotal];
                        ALE_CHAR XTable [KernTotal];

                     */

                 } AleKerningHeaderType;




#ifdef __cplusplus
extern "C"
 {
#endif

         /* Font Driver Interface
            -------------------------------------------------------- */
         typedef struct _AleFontDriverInterface
          {
             ALE_INT32 (*AleGetGlyphIndex) (ALE_UINT16 ucode, char *WorkBuf);
                                   /*
                                       Given an Unicode ucode, get its glyph index;
                                       return -1 if glyph not available
                                    */

             ALE_INT16 (*AleGetFont) (ALE_INT16 tcodetype, ALE_UINT16 charCod,
                                         char *tFontBuf, AleFontMetrics *fm0, char *WorkBuf);
                                    /*
                                        tcodetype = 1 means unicode
                                        charCode: Unicode of Char
                                        tFontBuf: font buffer, may be NULL if only metrics needed
                                        fm0: structure to receive metrics of this font
                                     */


             void (*AleCalculateBufferSizeInPixel) (ALE_INT16 ABufferRowByteTotal, ALE_INT16 ABufferColumnByteTotal,
                                           ALE_INT32 *x, ALE_INT32 *y, char *WorkBuf);
                                    /*
                                         given buffer of ABufferRowByteTotal * ABufferColumnByteTotal (bytes)
                                         get its size in pixels and put in *x & *y
                                     */

             ALE_INT32 (*AleGetFontWidthBBox) (char *WorkBuf);
                                    /* Get Width Bounding Box of current font */

             ALE_INT32 (*AleGetFontHeightBBox) (char *WorkBuf);
                                    /* Get Height Bounding Box of current font */


             ALE_UBYTE* (*AleGetCharFontCache) (char *WorkBuf);
                                   /*
                                        return a cache for character font access. You can provide NULL if none.
                                        If you provide, it must be able to contain any characters of current size and
                                        grayscale.

                                        When layout engine is rendering, it will send this cache to (*AleGetFont) ()
                                           (parameter tFontBuf) when retrieve char's metrics, and then, it will send
                                           this cache later to following (*AleDrawCharOnBuffer) () (parameter tCharFontBuf)

                                         In general, font driver prepare character's font and metrics together.

                                         Since Llayout engine needs font driver to calculate character's metrics first, and
                                         then layout engine request font driver to draw this character on some place
                                         in the buffer, if you provide this cache, the font driver may reduce some
                                         effort.
                                     */


             ALE_INT32 (*AleDrawCharOnBuffer) (char *tFontBuf, ALE_INT16 ABufferRowByteTotal, ALE_INT16 ABufferColumnByteTotal,
                                          ALE_INT16 tcodetype, ALE_UINT16 charCod,
                                          char *tCharFontBuf, AleFontMetrics *charfm,
                                          ALE_INT32 xstart, ALE_INT32 ystart, ALE_INT32 xlimit, ALE_INT32 ylimit,
                                          ALE_INT32 need_or, ALE_INT32 rightcut, char *WorkBuf);
                                     /*
                                         Draw a character on a give buffer.

                                         tFontBuf: buffer to be drawn
                                         ABufferRowByteTotal, ABufferColumnByteTotal: row/column size of buffer in bytes

                                         tcodeType, charCod: code of character, if tcodetype = 1, charCod is char's unicode,
                                                         see AleGetFont ()
                                         tCharFontBuf: if not NULL, it contains the font data of this character obtained
                                                      via AleGetFont ().
                                              tCharFontBuf may be 3 values:
                                                 1. NULL: you have to prepare this character's font yourself
                                                 2. the return value of (*AleGetCharFontCache)()
                                                 3. if user call AleGetStringFont () with tCharFontBuf != NULL,
                                                     tCharFontBuf here  will be user's tCharFontBuf
                                              In case 2 & 3, tCharFonBuf will contain the returned font by
                                                 (*AleGetFont)(), so you may use this for rendering directly instead
                                                 of retrieving font again.

                                          charfm: metrics of this char
                                          xstart, ystart:  up-left corner of this character
                                          xlimit, ylimit: size of tFontBuf in pixels
                                          need_or: always use "OR" for drawning this font, because some character
                                                   may overlap other characters.

                                          rightcut: the char may not be totally displayed; right part may be clipped

                                         return: 1-success, 0-failed, -1:right part clipped
                                      */


             ALE_INT32 (*AleGetFontMetricsExt1) (ALE_UINT16 ucode, char *WorkBuf);
                                   /*
                                      Get metrcs Ext1 for char with Unicode = ucode
                                      (for alebenga.c)
                                    */

             AleKerningHeaderType * (*AleGetDefaultKerningData) (char *ABuffer, ALE_INT32 *ASize, char *WorkBuf);
                                   /*
                                       Return the default kerning data of current font.

                                       If no such kerning data, or if it is not in memory, NULL is returned.

                                       When return, *ASize will be the size of kerning data,
                                         whether it is returned or not.

                                       If ABuffer is given, it is used to load the kerning data.
                                       *ASize will be set to the entire data size of this data. If ABuffer
                                         is not NULL, you should set *ASize to the size of ABuffer []
                                         when calling this function.
                                    */

             ALE_INT32 (*AleKerningToRealOffset) (ALE_INT32 x, char *WorkBuf);
                                  /*
                                     Convert offset in XTable [] of kerning data (in parameter x) to
                                     real offset on screen (in pixels) and return the real offset.
                                  */

             #define ALE_DRVINFO_SUPPORT_HORIADV_CLIP  1
             #define ALE_DRVINFO_SUPPORT_PARTIAL_CHAR  2
             #define ALE_DRVINFO_DEFAULT_RIGHTCLIP_MODE  3
             #define ALE_DRVINFO_ZEROADV_LOOSECLIP   4
             #define ALE_DRVINFO_DEFAULT_COMPACT_HINDI   5

             ALE_INT32 (*AleQueryDriverInfo) (ALE_INT32 AType, ALE_INT32 APara, char *WorkBuf);
                       /*
                          AType
                            ALE_DRIVERINFO_SUPPORT_HORIADV_CLIP
                               return 1-support display character in a buffer of width == char's horizontal advance
                                 0-do not support
                            ALE_DRIVERINFO_SUPPORT_PARTIAL_CHAR
                                 return 1-support display character in a buffer of width < char's horizontal advance
                                 0-do not support
                            ALE_DRVINFO_DEFAULT_RIGHTCLIP_MODE
                                return default right-clipping mode
                            ALE_DRVINFO_ZEROADV_LOOSECLIP
                                return 1 to display characters with zero advance and
                                            is just out of boundary (the next column after right-most column)
                                   0-otherwise
                            ALE_DRVINFO_DEFAULT_COMPACT_HINDI
                                return 1 if ALE_RNDO_CompactHindi should be started by default
                        */


              /* NOTE: if you want to add more functions, add
                 default function at AleSetFontDriver () to
                 avoid old-version users do not provide
                 new interfaces.
               */

          } AleFontDriverInterface;




   /* Initialization
      -------------------------------------------------------- */
   ALE_INT32 AleGetWorkBufferSize (void);  /* return work buffer size*/

   ALE_INT16 AleReset (char *AWorkBuf, ALE_UINT16 bufSize,
                                  AleFontDriverInterface *AFontDriver,
                                  char *AFontDriverWorkBuf);

   ALE_INT16 AleSetFontDriver (AleFontDriverInterface *AFontDriver,
                                  char *AFontDriverWorkBuf, char *WorkBuf);
   ALE_INT16 AleSetFontDriverWorkBuffer (char *AFontDriverWorkBuf, char *WorkBuf);
                /*
                    NOTE: if you start layout engine NOT via RBMP API RbmpReset (),
                     and you want to use old RBMP's layout engine API
                        (such as RbmpGetStringFont ())
                     Call RbmpSetAleWorkBuffer  (curent layout engine's working buffer)
                        so they (old API) can access correct working buffer.
                 */

   ALE_INT16 AleGetFontDriver (AleFontDriverInterface **AFontDriver,
                                  char **AFontDriverWorkBuf, char *WorkBuf);

   char *GetFontDriverWorkBuffer (char *WorkBuf);

   /* Font Operations
      -------------------------------------------------------- */



       /*
         Provide layout engine info to retrieve font of an entire string
         via this structure.

         To make your code compatible with future versions, we suggest
         you to reset the entire structure to zero by

           memset (&YourStructure, 0, sizeof (YourStructure));

         before setting any member.
        */
       typedef struct _AleGetStringFontInfo
        {
           /* Input
              ------------------------ */
           ALE_INT16 CodeType;
                       /* code type, 0-current language, 1-Unicode, 2-GlyphIndex
                           NOTE: for some languages, only Unicode is supported
                        */

           ALE_INT16 CharSpace;
                          /*
                            space (# of pixels) between each character.
                            Only support English and CJK;
                            do not support Indic/Arabic script or other complex ligature.
                            generally, set it to zero.

                             (Move this field after CodeType for 32-bit padding problem)
                           */

           ALE_UINT16 *String;
                       /* string to display, must add one element of zero at end */

           char *FontBuffer;
                         /* bitmap buffer to receive font. if you send NULL, font will
                            not be returned, only metrics is returned.
                          */

           ALE_INT16 BufferRowByteTotal;
                          /* # of bytes of a line(row) in above tFontBuf */

           ALE_INT16 BufferColumnByteTotal;
                          /* # of bytes of a column in above tFontBuf */

           AleFontMetrics *FontMetrics;
                          /* point to a AleFontMetrics structure to receive metrics
                             of this string */

           char *CharFontBuffer;
                          /*
                             buffer for retreiving font of a single character
                             when rendering by layout engine.
                             generally this is not needed, set this to NULL
                           */

           ALE_UINT16 *CharPosition;
                           /*
                             if you need to know the position of each character
                              when renderred, send an array here.
                             otherwise, just give NULL.
                            */

           ALE_INT16 DoNotClearBuffer;
                            /*
                               set to zero if you want layout engine to clear
                               buffer (tFontBuf) before rendering;
                               set to 1 if you do not want this.
                             */

           ALE_INT16 StartX, StartY;
                           /*
                              Position of upper-left corner of the rectangle area where
                              the string is renderred in tFontBuf.
                              generally, set both to zero.
                              NOTE: you can not give negative values.
                            */

           /* Output
              ------------------------ */
            ALE_INT16 HorizontalClipped;
                            /*
                                If not zero, renderred string is horizontally clipped because
                                   of insufficient buffer width. Zero if no such clipping occurred.
                                The returned value is # of character first clipped.
                                Ex: return 5 means the 5th character (and the remaining string)
                                  is clipped. So only 4 characters are rendered (i.e. AString[0]...AString [3])
                                This value is the same as the returned value of
                                  AleLasOutputStringClipped ().
                             */

            ALE_INT16 VerticalClipped;
                            /*
                                If not zero, renderred string is vertically clipped because
                                   of insufficient buffer height.  Zero if no such clipping occurred.
                             */

            ALE_INT16 Padding; /* make structure is 32-bit alignment */

        } AleGetStringFontInfo;

   ALE_INT16 AleGetStringFont (AleGetStringFontInfo *AInfo, char *WorkBuf);
                              /*
                                 AInfo: all information of retrieving font of an entire string.

                                 return:
                                   SUCCESS_GETFONT: ok
                                         (note: check clipping yourself)
                                   ERROR_BIDI_INSUFFICIENT_BUFFER
                                         bi-directional buffer insufficient
                                   ALE_ILLEGAL_START
                                         positon of startx/y is out of buffer area


                                 NOTE: to support Arabic:
                                   1. remember to active BiDirecition Text algorithm
                                       by AleSetTextDirection ()

                                   2. if your Unicode string will be longer than
                                      ALE_INNER_BIDI_BUFFER_MAXLEN
                                      please given re-order buffer by
                                      SetBiDiReorderBuffer (),
                                      otherwise ERROR_BIDI_INSUFFICIENT_BUFFER
                                      will be returned.

                                   3. you can use AleLasOutputStringClipped ()
                                      to see if your string is too ALE_INT32 for the buffer.
                               */

         /* convenient version for above function (with DoNotClearBuffer = StartX = StartY = 0) */
         ALE_INT16 AleGetStringFont0 (ALE_INT16 tcodetype, ALE_UINT16 *AString,
                                      char *tFontBuf,
                                      ALE_INT16 ABufferRowByteTotal, ALE_INT16 ABufferColumnByteTotal,
                                      AleFontMetrics *fm0,
                                      char *tCharFontBuf,  ALE_INT32 ACharSpace,
                                      ALE_UINT16 *ACharPosition,
                                      char *WorkBuf);



     ALE_INT32 AleLastOutputStringClipped (char *WorkBuf);
                              /*
                                   if last string sent to AleGetStringFont () is too long for
                                    buffer, it returns the order of character clipped
                                     (1 means displayed 0 chars, cut at 1st char
                                      2 means displayed 1 characters, cut at 2nd char...etc)
                                    otherwise it returns 0
                               */

     ALE_INT16 AleGetFont (ALE_INT16 tcodetype, ALE_UINT16 charCod,
                                         char *tFontBuf, AleFontMetrics *fm0, char *WorkBuf);

   /* Bi-Direction Support
      -------------------------------- */
   #define ALE_TEXTDIR_DEFAULT  0
   #define ALE_TEXTDIR_ENGLISH_MAJOR 1
   #define ALE_TEXTDIR_ARABIC_MAJOR 2
   #define ALE_TEXTDIR_ARABIC_SMART 3
   #define ALE_TEXTDIR_ARABIC_FIRSTCHAR 4

   ALE_INT16 AleSetTextDirection (ALE_INT16 type, char *WorkBuf);
                              /*
                                  type:
                                     ALE_TEXTDIR_DEFAULT (0, default): Always Left-to-right, no right-to-left
                                     ALE_TEXTDIR_ENGLISH_MAJOR (1): default left-to-right; right-to-left for Arabic
                                     ALE_TEXTDIR_ARABIC_MAJOR (2): default right-to-left, left-to-right for English
                                     ALE_TEXTDIR_ARABIC_SMART (3): default right-to-left, left-to-right for English;
                                         If no right-to-left language in text, all line is left-to-right.

                                  set type to 1-3 if you use right-to-left text
                               */
   ALE_INT16 AleGetTextDirection (char *WorkBuf); /* return current setting of text direction */
   ALE_INT16 AleIsUsingArabicTextDirection (char *WorkBuf);
                /*  return 1 if text direction is ALE_TEXTDIR_ARABIC_MAJOR or
                    ALE_TEXTDIR_ARABIC_SMART; 0 otherwise
                  */

   ALE_UINT16 *AleGetLastBiDiReorderBuffer (char *WorkBuf);
                              /*
                                 Get last AleGetStringFont ()'s display string
                                   NULL: no re-order occurred or insufficient reorder buffer
                                   non-NULL: the returned buffer contains rendered
                                      string (ended by zero) and then the original
                                      order (1 means originally the first char)
                               */

   void AleSetBiDiReorderBuffer (ALE_UINT16 *ABuffer, ALE_INT32 ABufferSize, char *WorkBuf);
                        /*
                            buffer size should be >=
                              (max string length + 1) * 3 * sizeof (ALE_UINT16)
                            give ABuffer = NULL means using internal buffer
                         */

   /* Cursor Operation Support

       Please use these functions immediately after
       AleGetStringFont ()
      ------------------------------------------------ */
   ALE_INT32 AleGetCharIndex (ALE_INT32 x, ALE_UINT16 *ALastCharPosition, char *WorkBuf);
             /*
                  Get the index of character at horizontal position x for last time you call
                   AleGetStringFont ().

                  return value: the order of character where x is on.
                   1 means x is on the first character (i.e. AString [0])
                   2 means x is on the second (i.e. AString [1])
                   ....etc.
                   0 means not on any character.

                  ALastCharacterPosition: returned ACharPos []
                        last time you call AleGetStringFont () (cannot be NULL)

                  NOTE: please use this function immediately after calling
                    AleGetStringFont ()
              */

   ALE_INT32 AleGetDisplayOrder (ALE_INT32 index, char *WorkBuf);
             /*
                 Get the display order of the (index)th character last time
                   AleGetStringFont () is called.

                 return: order: 1 means the first, 2 means the second...etc.
                 NOTE: if index is out of bound, 0 is returned.

                 in general, the returned value is index. However,
                 when Arabic or BiDirection text renderred,
                 the returned value may be different.

              */

   ALE_INT32 AleGetDisplayDirection (ALE_INT32 index, char *WorkBuf);
              /*
                 Get the display direction of the (index)th character last time
                   AleGetStringFont () is called.

                 return:  "Even number" (including 0) means this character
                   is displayed in left-to-right order;
                   "Odd number" means this character is displayed in
                   "right-to-left" order.
                 Note: if index is out of range, current base text direction
                 is used for judge.
               */

   ALE_INT32 AleGetDisplayLeft  (ALE_INT32 index, ALE_UINT16 *ALastCharPosition, char *WorkBuf);
   ALE_INT32 AleGetDisplayRight  (ALE_INT32 index, ALE_UINT16 *ALastCharPosition, char *WorkBuf);
             /*
                 Get the horizontal display boundary (left/right)
                   of the (index)th character in last time
                   AleGetStringFont () is called.
                   i.e. AString [index-1] when calling AleGetStringFont ()

                 NOTE
                        "left" boundary is at the first pixel column of this character,
                    while "right" boundary is at the first pixel column of next character.

                 return -1 if out of range.
              */

   ALE_INT32 AleGetCursorPrevIndex (ALE_INT32 index, ALE_UINT16 *ALastCharPosition, char *WorkBuf);
   ALE_INT32 AleGetCursorNextIndex (ALE_INT32 index, ALE_UINT16 *ALastCharPosition, char *WorkBuf);
             /*
                 when cursor is on the index(th) character,
                  (i.e. AString [index-1] when last time ecalling
                    AleGetStringFont ())
                  which character should be skipped to when
                  moving cursor previously or next.

                 return: 1 means the first character,
                   2 means the second...etc

                 you can send returned value to AleGetDisplayLeft ()
                 to get new cursor position.
              */

    ALE_INT32 AleSetLastRenderedStringLength (ALE_INT32 ANewLength, char *WorkBuf);
            /*
                Set LastRenderedStringLength record in working buffer.

                In general, you send the char position array generated by AleGetStringFont ()
                directly to above API functions (parameter ACharPosition []). However, if you
                want to use ALastCharPosition [] array calculated by yourself
                 on above API functions, set related string length by this function before calling them.

                For example, you may display a string of 20 characters in different sizes
                by calling AleGetStringFont () several times for different segments,
                and then you calculated a char position array of 20 characters
                yourself. To use this array for above API functions, you have to
                use AleSetLastRenderedStringLength () in advance like this:

                AleSetLastRenderedStringLength (20, <working buffer>);
                index = AleGetCharIndex (x, <char position array calculated by yourself>, <working buffer>);

                return: 0 on success; non-zero if failed.

                NOTE: if you use bi-directional text such as Arabic, be sure the
                Bi-direcitonal text reordering buffer matchs the char position
                array generated by yourself.
            */

    ALE_INT32 AleGetLastRenderedStringLength (char *WorkBuf);
            /* return string length of last renderred string */

    /* Other Display Control
       -------------------------------- */
    ALE_INT16 AleSetTab (ALE_UINT16 tabval, char *WorkBuf);
              /*
                   Set TAB width = tabval,
                   0 means do not support TAB (this is the default setting)
               */


    #define ALE_DEFAULTCHAR_NONE 0
    #define ALE_DEFAULTCHAR_SPACE  0xFFFF
    #define ALE_DEFAULTCHAR_STOP   0xFFFE

    ALE_INT32 AleSetDefaultChar (ALE_UINT16 charcode, char *WorkBuf);
                  /*
                       Set default character code
                          (character displayed by AleGetStringFont () when request character
                            is not available, i.e. missing character)

                       The code may be in Unicode or glyph index, depends on code type given to
                          AleGetStringFont ().

                       Three special codes can be given:
                         ALE_DEFAULTCHAR_NONE   ignore the missing char, do not display anything
                                                            (this is the default setting)
                         ALE_DEFAULTCHAR_SPACE  left a proper space for missing char
                         ALE_DEFAULTCHAR_STOP   just stop to display and return, ignore all
                                                          remaining characters.
                                                          AleGetStringFont () will return
                                                          ALE_MISSING_CHAR in this condition.

                         return 0 on success.
                   */


      #define ALE_RIGHTCLIP_SAFE   0
      #define ALE_RIGHTCLIP_HORIADV    1
      #define ALE_RIGHTCLIP_DISPLAYALL  2

      ALE_INT16 AleSetRightClippingMode (ALE_INT16 AMode, char *WorkBuf);
                       /*
                           Set Right-Clipping Mode: How Layout Engine stop rendering
                           when bitmap buffer's width is insufficient for the whole string.

                           ALE_RIGHTCLIP_SAFE
                             Stop rendering when remaining width < maximum possible
                             character width (ex: 16 for 16x16 bitmap font). This mode
                             guarantees all displayed characters are not clipped at all, but
                             some extra space will be left at buffer's right side.

                           ALE_RIGHTCLIP_HORIADV
                             Stop rendering when remaining width < character's
                             horizontal advance. This mode is better in buffer usage.
                             However, some special characters in some languages
                             may have glyph beyond its horizontal advance, and
                             these parts may be clipped at buffer's right side.
                             NOTE: some font engine may not support this mode.

                           ALE_RIGHTCLIP_DISPLAYALL
                             Stop rendering when remaining width is zero, i.e.
                             try to display characters as many as possible, even
                             if they can only be partially displayed.
                             NOTE: some font engine may not support this mode.

                           If you want to display Arabic right-aligned, you
                           should try ALE_RIGHTCLIP_HORIADV or
                           ALE_RIGHTCLIP_DISPLAYALL.

                           Return:
                             1-success
                             0-illegal mode, or mode not supported by font engine
                        */
      ALE_INT16 AleGetRightClippingMode (char *WorkBuf);


      /* Bi-Directional Text Control
         ------------------------------------------------ */
      ALE_UINT16 *AleBiDiReOrder
             (ALE_UINT16 *AString, ALE_UINT16 *ABuffer,
              ALE_INT32 ABaseLevel,  /*  -1 means decide by program */
              ALE_INT32 AReOrderOption,
              ALE_INT32 AMaxStringLength,
              char *WorkBuf);

      ALE_UINT16 *AleBiDiReOrder2
                  (ALE_UINT16 *AString, ALE_UINT16 *ABuffer,
                   ALE_INT32 ABaseLevel,  /* -1 means decide by program */
                   ALE_INT32 AMaxStringLength, char *WorkBuf);

       #define ALE_REORDER_ONLY_NECESSARY 1
       #define ALE_REORDER_MARK_BEFORE_CHAR  2
          /*
               Input:
                 *AString: the original Unicode string
                 ABaseLevel:  0:default text direction is left-to-right (majorly English)
                   1:default is right-to-left (majorly Arabic)
                   -1: decide by first English or Arabic character (if none, left-to-right is selected)
                   (NOTE: this is different from AleSetTextDirection ()'s parameter "type")

                 AReOrderOption: bitmask combination of following flags.
                       (For AleBiDiReOrder2, there is no such parameter, so 0 is assumed,
                       i.e. the following flags are not given)
                    ALE_REORDER_ONLY_NECESSARY
                       if set and no right-to-left text exists, re-order will not be performed
                       even if baselevel is right-to-left. (For AleBiDiReOrder2, there is no
                       such parameter, so reordering is always performed)
                    ALE_REORDER_MARK_BEFORE_CHAR
                       Put Arabic's combination marks (ex: U+0651) BEFORE the character
                       it modified. (Otherwise, this function will put marks AFTER the
                       character it modified.) Only use this flag if you want to display
                       reordered string yourself instead of calling AleGetStringFont (),
                       and you do know the difference.

                 AMaxStringLength: max possible length of *AString.
                 ABuffer []: a buffer to contain the result
                   NOTE: size of ABuffer[] must >= (AMaxStringLength+1) * 3

               Output:
                 *ABuffer: the reorderred string with Arabic characters replaced by
                  proper characters in Arabic presentation form A or B.
                  (NOTE: If no Arabic in AString, do not use data inside *ABuffer)

               Return value:
                 *AString itself: no re-ordering is necessary (and *ABuffer will be set to null-string!)
                 *ABuffer: the reordered string
                  NULL: error: the string needs to be reordered,
                            but the reorder buffer (*ABuffer) is insufficient: a larger buffer is necessary.


           */


      void AleSetBiDiBorderJoinInArabic (ALE_UINT16 CharBeforeBegin, ALE_UINT16 CharAfterEnd, char *WorkBuf);
                    /*
                       NOTE: AleLineClippingCheck () and AleGetStringFontInBlock ()
                         will use it to change setting.
                     */

      void AleSetRTLLigatureToPostCharPos (ALE_UINT16 *ACharPosition, char *WorkBuf);
      ALE_INT32 AleGetLigatureHeadDisplayPos (ALE_INT32 displaystart, ALE_UINT16 *ACharPosition, char *WorkBuf);

      ALE_INT16 AleGetTextDirectionByFirstChar (ALE_UINT16 *AString,  char *WorkBuf);
                     /*
                         Decide text direction by first "strong type character",
                            i.e. character with obvious direction, such as English or Arabic.
                         Return:  1-Left to Right, 2-Right to Left.
                         If no strong type character is found, current TextDirection
                          will be used to make decision:
                            If TextDirection <= 1: return 1.
                            otherwise: return 2.
                      */

   /* Language-Specific Support
      ------------------------------------------------ */
   ALE_INT16 AleEnableThaiCharGroup (ALE_INT32 enabled, char *WorkBuf);
                         /*
                             Enable/Disable Thai's Character Group in AleGetStringFont ().
                               (default is disabled).

                             If this is enabled, AleGetStringFont () will make revise for
                             Thai in CharPosition [] array so that consonants and vowels
                             consisting multiple Unicode characters will be treated
                             as one ligature.
                             This setting has no effect on renderred string, but
                             will make cursor operation function more correct
                             for Thai.

                             enabled:1 means enabled, 0 means disabled

                             return: 0-set success
                             ALE_NOT_SUPPORTED: Thai not supported
                          */

   ALE_UINT16 *TryToFindNextThaiCharGroup (ALE_UINT16 *start, char *WordBuf);
                          /*
                             Try to find the first character group from string
                              pointed by *start.

                             return:
                                non-NULL: address of the character immediately
                                  follow the first character group of start
                                  i.e. where next character group starts.
                                NULL: start is not pointed to a Thai character,
                                   or Thai not supported.
                           */



                         /* Rendering Flags for following function
                            ------------------------------------------------ */
                         #define ALE_RNDO_AdvanceCharGroupEnabled         1
                         #define ALE_RNDO_SinhalaAutoCombine           2
                         #define ALE_RNDO_FirstCharNoZeroWidth         4
                         #define ALE_RNDO_CompactHindi                 8
                         #define ALE_RNDO_CharSpaceBiasEnabled       0x10
                         #define ALE_RNDO_DisableKerningFromFont     0x20
                         #define ALE_RNDO_DisableCommonCaseOptimize         0x40
                         #define ALE_RNDO_DisableCharGroupDictionary         0x80
                         #define ALE_RNDO_EnableArabicLigature  0x100
                         #define ALE_RNDO_CharGroupDictionaryFirst   0x200

                         /* for compatibility of old code */
                         #define ALE_RNDO_ThaiCharGroupEnabled         ALE_RNDO_AdvanceCharGroupEnabled

   ALE_INT16 AleSetRenderOption (ALE_UINT32 option, ALE_INT32 enabled, char *WorkBuf);
                          /*
                             Set Rendering Options.

                             option:
                               ALE_RNDO_AdvancedCharGroupEnabled
                                  enable Thai & Myanmar character group detection.
                                  (setting this option is the same as calling
                                   AleEnableThaiCharGroup ())
                                  default: disabled

                               ALE_RNDO_SinhalaAutoCombine
                                  Generally, you need to add ZWJ to Unicode string to
                                  make characters combined into ligatures or change
                                  forms (such as Ra or Ya).  If this is enabled,
                                  ZWJ will not be necessary. However, you can still
                                  disable the combination by inserting ZWNJ.
                                  default: disabled

                               ALE_RNDO_FirstCharNoZeroWidth
                                  If this is enabled, layout engine will always check the
                                  first character of the rendering string. If its horizontal
                                  advance is zero, layout engine will automatically increase
                                  its horizontal advance to avoid this character being
                                  overlapped by following character.
                                  NOTE: only effective when using Bitmap Font.
                                  default: disabled

                               ALE_RNDO_CompactHindi
                                  If this is enabled, layout engine will try to overlap
                                  Hindi characters by metrics Extend2 to make words
                                  more compact.
                                  default: bitmapfont-disabled, MobileFont-enabled

                               ALE_RNDO_CharSpaceBiasEnabled
                                  This option is for internal use.

                               ALE_RNDO_DisableKerningFromFont
                                  If this is enabled, kerning setting inside font data
                                  will not be enabled automatically. This is used
                                  when you want to provide kerning data yourself.

                               ALE_RNDO_EnableArabicLigature
                                  enable Arabic Form-A ligatures

                             enabled:1 means enabled, 0 means disabled
                                (all options' default settings are disabled)

                             return: 0-set success
                             ALE_NOT_SUPPORTED: option not supported

                           */

   ALE_UINT32 AleGetRenderOption (ALE_UINT32 mask, char *WorkBuf);
                          /*
                             Get current render option.
                               mask:0- get OR combination of all options
                                  non-zero: get only option indicated by mask
                           */


   /* Kerning
      ------------------------------------------------ */
   ALE_INT32 AleEnableKerningData (char *ABuffer, ALE_INT32 *ALoadBufferSize, char *WorkBuf);
                      /*
                          Enable kerning data in following cases:

                            case I: You have your own kerning data,
                              set ABuffer = the address of your kerning data &
                              set ALoadBufferSize = NULL.
                              NOTE: it is suggested to set render option
                                ALE_RNDO_DisableKerningFromFont to keep
                                your kerning data from being overriden by the setting
                                of font data.

                            case II: You want to enable the kerning data inside
                              font data, and you already load whole font data
                              into memory:
                              set ABuffer = NULL & ALoadBufferSize = NULL.

                              If kerning data is not available or if it is not in
                              memory, ALE_NOT_SUPPORTED will be returned,
                              (and kerning will be disabled).

                            case III: You want to query the size of kerning data
                              inside font data, so you can allocate memory to load it.
                              set ABuffer = NULL,
                              set ALoadBufferSize = address of an integer to
                                   receive kerning data size.
                              when return, *ALoadBufferSize will be set
                               to the size of kerning data in bytes.
                               return ALE_SUCCESS_GETFONT if kern data available,
                                ALE_NOT_SUPPORTED if not

                            case IV: You provide a buffer to load kerning data
                                   into memory and enable it. If your font data
                                   is not inside memory, you should use this function
                                   in this way.
                                   (If you do not know the size of kering data,
                                    you can query for it. see case III.)

                                 set ABuffer [] = your buffer,
                                 set *ALoadBufferSize = size of your buffer in bytes.

                                When return, *ALoadBufferSize will be set to the
                                size of kerning data, and ABuffer [] will contain
                                the kerning data.
                                If size of ABuffer[] is insufficient,
                                  ALE_ILLEGAL_BUFFERSIZE will be returned;
                                If no kerning data is available,
                                  ALE_NOT_SUPPORTED will be returned (and kerning will be disabled).

                          return:
                            ALE_SUCCESS_GETFONT  ok
                            ALE_ILLEGAL_START  data in ABuffer [] is error (and kerning will be disabled).
                            ALE_ILLEGAL_BUFFERSIZE size of ABuffer [] is insufficient

                            ALE_NOT_SUPPORTED  no kerning data available.
                                NOTE: in case II/IV kerning will be disabled in
                                 following rendering.

                         WARNING: If there are more than one font in your font data,
                          the setting in case I, II, IV may be void if you select another
                          font. Unless your font is in memory and you let the kerning
                          data be started automatically, you will need to set kerning
                          every time when you select a new font.

                       */
   void AleDisableKerningData (char *WorkBuf);
                     /*  Stop using current kerning data */

   ALE_INT32 AleGetKerningX (ALE_UINT16 APrevChar, ALE_UINT16 ACurrChar, char *WorkBuf);
                      /*
                          Get X Offset (in pixels) of character pair [APrevChar, ANextChar]
                             positive: enlarge char space; negative: shrink char space
                       */


#ifdef __cplusplus
 };
#endif



#endif

