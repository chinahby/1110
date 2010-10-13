/*
  ========================================================================

  FILE:       ResObj.c

  SERVICES:   ResFile implementation / replacement for ISHELL_LoadResXxx

  GENERAL DESCRIPTION:

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#include "AEE.h"
#include "AEEFile.h"
#include "AEEStdLib.h"
#include "AEESource.h"
#include "ResObj.h"
#include "FileSource.h"
#include "wutil.h"
#include "ResDecoderCache.h"

#include "GenericViewer.bid"
#include "IForceFeed.h"
#include "IImageDecoder.h"
#include "GenericViewer.bid"
#include "BMPDecoder.bid"


// define the following to test const file handling 
// (ROM-based, memory mapped files)
//#define RESFILE_TEST


#if defined(RESFILE_TEST)
static int MappedFileMgr_New(IFileMgr **ppo, IShell *piShell);
#endif


/////////////////////////////////////////////////////////////////////////////
// 

#define RM_FILE_TYPE                (0x11)
#define RM_FILE_CODE_VERSION        (0x0001)
#define RM_FILE_OLDEST_FILE_VERSION (0x0001)

#define PEEK_BUF_SIZE               (4096)


typedef struct ResHeader {
   byte        byFileType;    // File type, must be 0x11 for this resource format
   byte        byReserved;    // Should be zero
   uint16      nFileVers;     // Version of resource format in this file
   uint16      nOldestCode;   // Oldest version of code to read format in file
   uint16      nMapEntries;   // Number of ResMap entries in file
   uint32      dwMapOffset;   // Offset to beginning of resource map in file
   uint32      dwMapLength;   // Length in bytes of resource map
   uint32      dwOffsetOffset;// Offset to beginning of offsets table
   uint32      dwOffsetCount; // Count of offsets in table minus 1
   uint32      dwDataOffset;  // Offset to beginning of resource data
   uint32      dwDataLength;  // Length of resource data in bytes
} ResHeader;



typedef struct ResMap {
   uint16   nType;           // Resource type for this entry
   uint16   nStartID;        // Starting resource ID for this entry
   uint16   nAdditionalIDs;  // Number of ADDITIONAL IDs covered besides nStartID
   uint16   nStartOffsetIdx; // Starting offsets table index
} ResMap;


struct ResObj {
   IShell *       piShell;    // shell required for CI and GetHandler
   IFile *        piFile;     // our open resource file
   ISourceUtil *  piSourceUtil;  // ISourceUtil - created only if needed

   // string caching
   char *         pBuffer;    // string resource buffer caching
   int            cbBuf;      // allocated size of 'pBuffer'
   uint32         cbRawSize;  // sizeof resource data in 'pBuffer'
   uint16         nLastResID; // resource ID that corresponds with 'pBuffer'
                              // (RESTYPE_STRING implied)

   ResHeader      hdr;        // resource header
   void *         pMapped;    // if mapped, pointer to start of file in memory, else 0
   uint32 *       pOffsets;   // pointer to start of offset table
   ResMap *       pMap;       // pointer to start of resource map
   void *         pUser;      // user pointer

   ICharsetConv * piConv;     // pointer to charset converter class object
   const char *   pszConvTo;  // "convert-to" string

   // size is 32-bit alignment copacetic
   // - overallocated - 
   // map data follows
   // then offset table
};



/////////////////////////////////////////////////////////////////////////////
// Decoding functions
// Note they all have the same function signature, namely:

typedef int (*PFNDECODE)      (AECHAR *pwcDest, int cwDest, const char *pcSrc, int cbSrc);

// function signature for counting output size
typedef int (*PFNCOUNTDECODE) (const char *pcSrc, int cbSrc);



/////////////////////////////////////////////////////////////////////////////

#ifndef CHARISIN
#define CHARISIN(ch, cBtm, cTop)   \
   ( (unsigned char) ((ch) - (cBtm)) <= ((unsigned char) (cTop) - (cBtm)) )
#endif   

// Is c the first byte of a double-byte character?
//
#define EUC_DOUBLE(c)      (((c) & 0x80) != 0)

// Is c the first byte of a double-byte character?
//
#define SJIS_DOUBLE(c)     (CHARISIN((c), 0x81, 0x9F) || CHARISIN((c), 0xE0, 0xFC))


static int DecodeEUC(AECHAR *pwcDest, int cwDest, const char *pcSrc, int cbSrc)
{
   AECHAR *pwc = pwcDest;
   byte *  pby = (byte *) pcSrc;
   AECHAR *pwcMax = pwc + cwDest;
   byte *  pbyMax = pby + cbSrc;
   byte by;

   while (pwc < pwcMax && pby < pbyMax) {
      by = pby[0];
      if (EUC_DOUBLE(by)) {
         ((byte*)pwc)[0] = by;
         ((byte*)pwc)[1] = pby[1];
         ++pwc;
         pby += 2;
      } else {
         *pwc++ = by;
         ++pby;
      }
   }

   if (pby > pbyMax) {
      // read one past the end: improperly formatted input
      // discard last character of output
      pby -= 2;
      --pwc;
   }

   if (pwc == pwcMax) {
      pwc--;
   }
   *pwc++ = (AECHAR)0;

   return (pwc - pwcDest) * (int)sizeof(AECHAR);
}


// returns required buffer size (in bytes) to store decoded string
static int CountDecodeEUC(const char *pcSrc, int cbSrc)
{
   int cw = 0;
   byte *  pby = (byte *) pcSrc;
   byte *  pbyMax = pby + cbSrc;

   while (pby < pbyMax) {
      if (EUC_DOUBLE(*pby)) {
         pby += 2;
      } else {
         pby++;
      }
      cw++;
   }

   if (pby > pbyMax) {
      // read one past the end: improperly formatted input
      // discard last character of output
      cw--;
   }

   return (cw + 1) * (int)sizeof(AECHAR);
}


static int DecodeSJIS(AECHAR *pwcDest, int cwDest, const char *pcSrc, int cbSrc)
{
   AECHAR *pwc = pwcDest;
   byte *  pby = (byte *) pcSrc;
   AECHAR *pwcMax = pwc + cwDest;
   byte *  pbyMax = pby + cbSrc;

   while (pwc < pwcMax && pby < pbyMax) {

      byte by = pby[0];

      if (SJIS_DOUBLE(by)) {
         ((byte*)pwc)[0] = by;
         ((byte*)pwc)[1] = pby[1];
         ++pwc;
         pby += 2;
      } else {
         *pwc++ = by;
         ++pby;
      }
   }

   if (pby > pbyMax) {
      // read one past the end: improperly formatted input
      // discard last character of output
      pby -= 2;
      --pwc;
   }

   if (pwc == pwcMax) {
      pwc--;
   }
   *pwc++ = (AECHAR)0;

   return (pwc - pwcDest) * (int)sizeof(AECHAR);
}


// returns required buffer size (in bytes) to store decoded string
static int CountDecodeSJIS(const char *pcSrc, int cbSrc)
{
   int cw = 0;
   byte *  pby = (byte *) pcSrc;
   byte *  pbyMax = pby + cbSrc;

   while (pby < pbyMax) {
      if (SJIS_DOUBLE(*pby)) {
         pby += 2;
      } else {
         pby++;
      }
      cw++;
   }

   if (pby > pbyMax) {
      // read one past the end: improperly formatted input
      // discard last character of output
      cw--;
   }
      
   return (cw + 1) * (int)sizeof(AECHAR);
}


static int DecodeUTF8(AECHAR *pwcDest, int cwDest, const char *pcSrc, int cbSrc)
{
   AECHAR *pwc = pwcDest;
   byte *  pby = (byte *) pcSrc;
   AECHAR *pwcMax = pwc + cwDest;
   byte *  pbyMax = pby + cbSrc;

   while (pwc < pwcMax && pby < pbyMax) {
      
      uint32 w = *pby++;

      if (w >= 0xC0) {
         int nExtra;
         
         if ( (w & 0x20) == 0) {

            // 2 bytes (11 bits)
            
            nExtra = 1;
            w = (w & 0x1f) << 6;
            w |= pby[0] & 0x3F;

         } else if ( (w & 0x10) == 0) {

            // 3 bytes (16 bits)
            
            nExtra = 2;
            w = (w & 0x0f) << 12;
            w |= (pby[0] & 0x3F) << 6;
            w |= (pby[1] & 0x3F);
            
         } else if ( (w & 0x08) == 0) {

            // 4 bytes (21 bits)

            nExtra = 3;
            w = (w & 0x07) << 18;
            w |= (pby[0] & 0x3F) << 12;
            w |= (pby[1] & 0x3F) << 6;
            w |= (pby[2] & 0x3F);

         } else {

            nExtra = 0;
         }

         pby += nExtra;

         if (pby > pbyMax) {
			// don't consume first half of a character
            pby -= nExtra+1;
            break; 
         }

         if (w >= 0x10000) {
            // Output high-order surrgate character & compute low-order character
            //   High Surrogates: U+D800..U+DBFF
            //   Low Surrogates:  U+DC00..U+DFFF
            //
            // Note that 4-byte UTF-8 represents 21 bits, while 2-word UTF-16
            // supports just over a 20 bit range (00000 ... 10FFFF).

            *pwc++ = (AECHAR) (0xD7c0 + ((w >> 10) & 0x3FF));
            w = (w & 0x3FF) | 0xDC00;

            if (pwc == pwcMax) {
               // result won't fit
               pby -= (nExtra + 1);
               --pwc;
               break;
            }
         }
      }
      *pwc++ = (AECHAR) w;
   }

   // zero terminate
   if (pwc == pwcMax) {
      pwc--;
   }
   *pwc++ = (AECHAR)0;

   return (pwc - pwcDest) * (int)sizeof(AECHAR);
}


// returns required buffer size (in bytes) to store decoded string
static int CountDecodeUTF8(const char *pcSrc, int cbSrc)
{
   int cw = 0;
   byte *  pby = (byte *) pcSrc;
   byte *  pbyMax = pby + cbSrc;

   while (pby < pbyMax) {
      
      uint32 w = *pby++;

      if (w >= 0xC0) {
         int nExtra;
         
         if ( (w & 0x20) == 0) {

            // 2 bytes (11 bits)
            
            nExtra = 1;

         } else if ( (w & 0x10) == 0) {

            // 3 bytes (16 bits)
            
            nExtra = 2;
            
         } else if ( (w & 0x08) == 0) {

            // 4 bytes (21 bits)

            nExtra = 3;
            w = (w & 0x07) << 18;

         } else {

            nExtra = 0;
         }

         pby += nExtra;

         if (pby > pbyMax) {
            break; 
         }

         if (w >= 0x10000) {
            // Output high-order surrgate character & compute low-order character
            //   High Surrogates: U+D800..U+DBFF
            //   Low Surrogates:  U+DC00..U+DFFF
            //
            // Note that 4-byte UTF-8 represents 21 bits, while 2-word UTF-16
            // supports just over a 20 bit range (00000 ... 10FFFF).
            cw++;
         }
      }
      cw++;
   }

   return (cw + 1) * (int)sizeof(AECHAR);
}


static int Decode8859_1(AECHAR *pwcDest, int cwDest, const char *pcSrc, int cbSrc)
{
   AECHAR *pwc = pwcDest;
   AECHAR *pwcMax = pwcDest + cwDest;

   byte *  pby = (byte *) pcSrc;
   byte *  pbyMax = pby + cbSrc;

   while (pwc < pwcMax && pby < pbyMax) {
      uint16 w = *pby++;
      *pwc++ = w;
   }

   if (pwc == pwcMax) {
      pwc--;
   }
   *pwc++ = (AECHAR)0;

   return (pwc - pwcDest) * (int)sizeof(AECHAR);
}



// returns required buffer size (in bytes) to store decoded string
static int CountDecode8859_1(const char *pcSrc, int cbSrc)
{  
   (void) pcSrc;
   return (cbSrc + 1) * (int)sizeof(AECHAR);
}



static int DecodeUni(AECHAR *pwcDest, int cwDest, const char *pcSrc, int cbSrc)
{
   int cbMax = CONSTRAIN(cwDest*(int)sizeof(AECHAR), 0, cbSrc);

   MEMMOVE(pwcDest, pcSrc, (uint32)cbMax);
   *(pwcDest + (cbMax/(int)sizeof(AECHAR))) = 0;

   return cbMax + (int)sizeof(AECHAR);
}


// returns required buffer size (in bytes) to store decoded string
static int CountDecodeUni(const char *pcSrc, int cbSrc)
{
   (void) pcSrc;
   return cbSrc + (int)sizeof(AECHAR);
}



/////////////////////////////////////////////////////////////////////////////


static int ResObj_DecodeString(ResObj *me, AECHAR *pwcDest, int cwDest, 
                               const char *pcSrc, int cbSrc, uint32 *pnDecoded)
{
   int nErr = 0;
   byte nType;
   byte nType2;

   nType = (byte) *pcSrc;
   nType2 = (byte) *(pcSrc+1);

   /* Check the string type:

      AEE_ENC_UNICODE - (0xfffe)  0xfeff is not supported.
      AEE_ENC_KSC5601 (Korean)      "
      AEE_ENC_EUC_CN (Chinese)      "
      AEE_ENC_S_JIS (Japanese)      "
      AEE_ENC_UTF8 - 
      AEE_END_ISOLATIN1 - If it's set or was not set but positive, we assume it was latin1
   */
   if (nType < AEE_ENC_UTF8) {
      nErr = EFAILED;

   } else if (nType == AEE_ENC_UNICODE || nType == AEE_ENC_KSC5601 ||
              nType == AEE_ENC_EUC_CN  || nType == AEE_ENC_S_JIS) {

      if (nType2 != AEE_ENC_UNICODE2) {
         nErr = EFAILED;
      }

   } else if (nType > AEE_ENC_UTF8) {
      nType = AEE_ENC_ISOLATIN1;
   }

   // skip over the header byte(s)
   if (nType == AEE_ENC_UTF8 || nType == AEE_ENC_ISOLATIN1) {
      pcSrc++;
      cbSrc--;
   } else {
      pcSrc += 2;
      cbSrc -= 2;
   }

   // use ICharsetConv converter if we have one and the 
   // From/To combination is supported
   if (!nErr && me->piConv && me->pszConvTo) {
      const char *pcszFrom = 0;
      if (AEE_ENC_UNICODE == nType) {
         pcszFrom = "UTF-16LE";
      } else if (AEE_ENC_UTF8 == nType) {
         pcszFrom = "UTF-8";
      } else if (AEE_ENC_KSC5601 == nType) {
         pcszFrom = "KSC-5601-1992-16LE";
      } else {
         nErr = EFAILED;
      }
      // see if the ICharsetConv can handle the conversion
      if (!nErr) {
         nErr = ICharsetConv_Initialize(me->piConv, pcszFrom, me->pszConvTo, ' ');
      }
      // do the conversion
      if (!nErr) {
         byte *pSrcBuf = CAST(byte*, pcSrc);
         byte *pDstBuf = CAST(byte*, pwcDest);
         int nSrcSize = cbSrc; 
         int nDstSize = cwDest * (int)sizeof(AECHAR);
         void *pFree = 0;

         if (!pDstBuf || 0 == nDstSize) {
            nDstSize = nSrcSize * 6;
            pFree = MALLOC((dword)nDstSize);
            pDstBuf = CAST(byte*, pFree);
            if (!pFree) {
               nErr = ENOMEMORY;
            }
         }
         if (!nErr && pDstBuf) {
            int nConverted = 0;
            byte *pDstStart = pDstBuf;
            nErr = ICharsetConv_CharsetConvert(me->piConv, &pSrcBuf, &nSrcSize,
                                               &pDstBuf, &nDstSize, &nConverted);
            *pnDecoded = (uint32)(pDstBuf - pDstStart);
         }
         FREEIF(pFree);
      }

      return nErr;
   }


   if (!nErr) {

      PFNDECODE      pfnDecode = DecodeUni;
      PFNCOUNTDECODE pfnCount  = CountDecodeUni;

      if (nType == AEE_ENC_UTF8) {
         pfnDecode = DecodeUTF8;
         pfnCount  = CountDecodeUTF8;

      } else if (nType == AEE_ENC_ISOLATIN1) {
         pfnDecode = Decode8859_1;
         pfnCount  = CountDecode8859_1;

      } else if (nType == AEE_ENC_S_JIS) {
         pfnDecode = DecodeSJIS;
         pfnCount  = CountDecodeSJIS;

      } else if (nType == AEE_ENC_KSC5601 || nType == AEE_ENC_EUC_CN) {
         pfnDecode = DecodeEUC;
         pfnCount  = CountDecodeEUC;

      } else if (nType == AEE_ENC_UNICODE || nType == OEM_RAW_TEXT) {
         pfnDecode = DecodeUni;
         pfnCount  = CountDecodeUni;
      }

      if (!pwcDest || !cwDest) {
         *pnDecoded = (uint32) pfnCount(pcSrc, cbSrc);
      } else {
         *pnDecoded = (uint32) pfnDecode(pwcDest, cwDest, pcSrc, cbSrc);
      }

   }

   return nErr;
}


/////////////////////////////////////////////////////////////////////////////


static __inline int IFILE_ReadBlock(IFile *me, void *pDest, uint32 dwSize) {
   int32 cbBytes = IFILE_Read(me, pDest, dwSize);
   return (cbBytes == (int32)dwSize) ? SUCCESS : EFAILED;
}


static int IFILE_SeekReadBlock(IFile *me, uint32 dwOffset, void *pDest, uint32 dwSize)
{
   int nErr = IFILE_Seek(me, _SEEK_START, (int32)dwOffset);
   if (!nErr) {
      nErr = IFILE_ReadBlock(me, pDest, dwSize);
   }
   return nErr;
}


static int ResObj_SeekReadBlock(ResObj *me, uint32 dwOffset, void *pDest, uint32 dwSize)
{
   if (me->pMapped) {
      MEMCPY(pDest, (const void*) ((byte*)me->pMapped + dwOffset), dwSize);
      return SUCCESS;
   } else {
      return IFILE_SeekReadBlock(me->piFile, dwOffset, pDest, dwSize);
   }
}


static boolean VersionCheck(
   unsigned short inMyVersion,  // My current version
   unsigned short inOldestVersionICanHandle, // Oldest version of it I understand
   unsigned short inItsVersion, // Its current version
   unsigned short inItsOldestVersionItCanHandle) // Its oldest version of me that it can handle
{

  if (inMyVersion >= inItsVersion) {
    return inItsVersion >= inOldestVersionICanHandle ? (boolean) TRUE : (boolean) FALSE;

  } else {
    return inMyVersion >= inItsOldestVersionItCanHandle ? (boolean) TRUE : (boolean) FALSE;
  }

}

#if 0
static boolean IsMIF(const char* cpszPath)
{
#define AEE_MIF_EXT        ".mif"
#define AEE_MIF_MASK_EXT   ".mi~"

   const char *cpszDot;

   cpszDot = STRRCHR(BASENAME(cpszPath),'.');

   if ((char *)0 == cpszDot) {
      return FALSE;
   }

   // the extension matches AEE_MIF_EXT
   return (!STRICMP(cpszDot, AEE_MIF_EXT) || 
           !STRICMP(cpszDot, AEE_MIF_MASK_EXT));
}
#endif


/* IFILE_CheckValidResFile
Description:
Prototype:
Return Value:
*/
static int IFILE_CheckValidResFile(IFile *me, ResHeader *pHdr)
{
   int nErr = 0;

   nErr = IFILE_SeekReadBlock(me, 0, pHdr, sizeof(ResHeader));

   if (!nErr) {

      // endian conversion
      LETOHS_D(pHdr->nFileVers);
      LETOHS_D(pHdr->nOldestCode);
      LETOHS_D(pHdr->nMapEntries);
      LETOHL_D(pHdr->dwMapOffset);
      LETOHL_D(pHdr->dwMapLength);
      LETOHL_D(pHdr->dwOffsetOffset);
      LETOHL_D(pHdr->dwOffsetCount);
      LETOHL_D(pHdr->dwDataOffset);
      LETOHL_D(pHdr->dwDataLength);

      (void)pHdr->byReserved;
      (void)pHdr->dwDataLength;

      // Check file type and versions
      if (pHdr->byFileType != RM_FILE_TYPE ||
          !VersionCheck(RM_FILE_CODE_VERSION, RM_FILE_OLDEST_FILE_VERSION,
                        pHdr->nFileVers, pHdr->nOldestCode)) {
         nErr = EBADITEM;
      }
   }

   // We think we have a compatible file format version, so perform some
   // obvious checks to see if it looks good.
   if (!nErr && 
       !(pHdr->dwMapOffset >= sizeof(ResHeader)
         && pHdr->dwOffsetOffset >= pHdr->dwMapOffset
         && pHdr->dwDataOffset >= pHdr->dwOffsetOffset)) {
      nErr = EBADITEM;
   }

   // Check some redundancy in the header
   if (!nErr && 
       !(pHdr->dwMapLength == pHdr->nMapEntries * sizeof(ResMap))) {
      nErr = EBADITEM;
   }

   // More offset checks
   if (!nErr && 
       !(pHdr->dwOffsetOffset >= (pHdr->dwMapOffset + pHdr->dwMapLength)
         && pHdr->dwDataOffset >= (pHdr->dwOffsetOffset + 4*(1+pHdr->dwOffsetCount)))) {
      nErr = EBADITEM;
   }

   return nErr;
}

  
/* IFILEMGR_OpenResFile
Description:
Prototype:
Return Value:
*/
int IFILEMGR_OpenResFile(IFileMgr *me, const char *pszResFile, IFile **ppo, ResHeader *pHdr)
{
   IFile *piFile = 0;
   int nErr = SUCCESS;

   // open the resfile for reading
   piFile = IFILEMGR_OpenFile(me, pszResFile, _OFM_READ);
   if (!piFile) {
      nErr = IFILEMGR_GetLastError(me);
   }

   // set cache size and check if file is valid
   if (!nErr && piFile != NULL) {
      (void) IFILE_SetCacheSize(piFile, SCS_BEST);
      nErr = IFILE_CheckValidResFile(piFile, pHdr);
   }

#if 0 // disabled for now
   // special handling for MIF files running on the simulator
#if defined(WIN32)
   if (nErr == EBADITEM && IsMIF(pszResFile)) {
      IFile_Munge(piFile, FALSE);
      // If the unmunged MIF is still invalid, it's likely pirated...
      nErr = IFILE_CheckValidResFile(piFile, pHdr);
   }
#endif
#endif

   if (!nErr && piFile != NULL) {
      *ppo = piFile;
      (void) IFILE_AddRef(piFile);
   }

   RELEASEIF(piFile);

   return nErr;
}

/* ResObj_Find
|| 
|| Description:
||    Find the correct entry in the offset table given a type and id.
|| 
|| Prototype:
||    int ResObj_Find(
||       ResObj * me, 
||       uint16   nResID,        // ID of resource to find
||       uint16   nType,         // type of resource
||       uint32 * pdwOffset,     // address of uint32 to be filled with the file offset of resource
||       uint32 * pdwSize        // address of uint32 to be filled with the resource size
||    );
|| 
|| 
|| Return Value:
||    SUCCESS              resource was found
||    ERESOURCENOTFOUND    resource was not found
|| 
||    On SUCCESS, the memory locations pointed to by pdwOffset and pdwSize are filled 
||    with the file index and size of the found resource, respectively.
*/
static int ResObj_Find(ResObj *me, uint16 nResID, uint16 nType, uint32 *pdwOffset, uint32 *pdwSize)
{
   if (me->pMap && me->pOffsets) {

      ResMap * pMap;
      int i;

      pMap = me->pMap;

      for (i=me->hdr.nMapEntries; i; --i, ++pMap) {

         if (pMap->nType == nType 
               && pMap->nStartID <= nResID
               && nResID <= (pMap->nStartID + pMap->nAdditionalIDs)) {

            // we found the relevant map entry, compute the index
            uint32 index = pMap->nStartOffsetIdx + (nResID - pMap->nStartID);

            // compute the resource size
            *pdwSize = me->pOffsets[index+1] - me->pOffsets[index];
            // return the offset
            *pdwOffset = me->pOffsets[index];
            
            return SUCCESS;
         }
      }
   }

   return ERESOURCENOTFOUND;
}


/* ResObj_Setup
|| 
|| Description:
||    Load the resfile map and offsets table into memory
|| 
|| Prototype:
||    int ResObj_Setup(ResObj *me);
|| 
|| Return Value:
||    SUCCESS
||    EFAILED
*/
static int ResObj_Setup(ResObj *me)
{
   int nErr;

   // Load the map
   nErr = IFILE_SeekReadBlock(me->piFile, me->hdr.dwMapOffset, me->pMap, me->hdr.dwMapLength);

   // load the offset table
   if (!nErr) {
      uint32 nOffsetSize = sizeof(uint32) * (me->hdr.dwOffsetCount + 1);
      nErr = IFILE_SeekReadBlock(me->piFile, me->hdr.dwOffsetOffset, me->pOffsets, nOffsetSize);
   }

// NOTE: we do not support big endian and memory mapped files simultaneously
//       this function is never actually called if memory mapped
#if defined(FEATURE_BIG_ENDIAN)
   if (!nErr) {
      int i;
      for (i=0; i < me->hdr.dwMapLength/sizeof(ResMap); i++) {
         LETOHS_D(me->pMap[i].nType );
         LETOHS_D(me->pMap[i].nStartID );
         LETOHS_D(me->pMap[i].nAdditionalIDs );
         LETOHS_D(me->pMap[i].nStartOffsetIdx );
      }
      for(i=0; i < 1 + me->hdr.dwOffsetCount; i++) {
         LETOHL_D(me->pOffsets[i]);
      }
   }
#endif

   return nErr;
}


/* ResObj_LoadSource
*/
int ResObj_LoadSource(ResObj *me, uint16 nResID, uint16 nType, ISource **ppo)
{
   int nErr;
   uint32 dwOffset, dwSize;

   nErr = ResObj_Find(me, nResID, nType, &dwOffset, &dwSize);

   if (me->pMapped) {

      if (!nErr && !me->piSourceUtil) {
         nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_SOURCEUTIL, (void**)&me->piSourceUtil);
      }

      if (!nErr) {
         nErr = ISOURCEUTIL_SourceFromMemory(me->piSourceUtil, (byte*)me->pMapped + dwOffset,
                                             (int32)dwSize, NULL, 0, ppo);
      }

   } else {

      if (!nErr) {
         nErr = IFILE_Seek(me->piFile, _SEEK_START, (int32)dwOffset);
      }
   
      // wrap the IFile with an object that provides sub-file streams
      if (!nErr) {
         nErr = FileSource_New(ppo, me->piFile, dwOffset, dwSize);
      }
   }

   return nErr;
}


/* ResObj_GetStream
*/
static int ResObj_GetStream(ResObj *me, uint16 nResID, uint16 nType, IAStream **ppo)
{
   int nErr;
   ISource *piSource = 0;

   nErr = ResObj_LoadSource(me, nResID, nType, &piSource);

   if (!nErr && !me->piSourceUtil) {
      nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_SOURCEUTIL, (void**)&me->piSourceUtil);
   }

   if (!nErr) {
      nErr = ISOURCEUTIL_AStreamFromSource(me->piSourceUtil, piSource, ppo);
   }

   RELEASEIF(piSource);

   return nErr;
}


/* ResObj_GetHandler
*/
static __inline int ResObj_GetHandler(ResObj *me, AEECLSID clsidBase, const char *pszIn, AEECLSID *ppoClsid) {
   AEECLSID clsid = ISHELL_GetHandler(me->piShell, clsidBase, pszIn);
   if (ppoClsid) {
      *ppoClsid = clsid;
   }
   return clsid ? SUCCESS : EFAILED;
}

static int ResObj_GetStreamMimeType(ResObj *me, uint16 nResID, uint16 nType,
                                IAStream **ppo, char *szMimeType, byte nMimeLen) 
{
   int nErr = ResObj_GetStream(me, nResID, nType, ppo);
   if (!nErr && nType == RESTYPE_IMAGE) {
      byte nActualLen = 0;
      if (1 == IASTREAM_Read(*ppo, &nActualLen, sizeof(nActualLen))
          && nActualLen > 2
          && nActualLen <= nMimeLen) {
            (void) IASTREAM_Read(*ppo, szMimeType, nActualLen-1);

      }
      else {
         nErr = EFAILED;
      }
   }
   return nErr;
}



///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

#ifdef FULL_FUNCTIONALITY


/* ISOURCEUTIL_PeekFromAStream
*/
static int ISOURCEUTIL_PeekFromAStream(ISourceUtil *me, IAStream *pias, IPeek **ppo)
{
   int nErr;
   ISource * piSource = 0;

   nErr = ISOURCEUTIL_SourceFromAStream(me, pias, &piSource);
   if (!nErr) {
      nErr = ISOURCEUTIL_PeekFromSource(me, piSource, PEEK_BUF_SIZE, ppo);
   }

   RELEASEIF(piSource);
   return nErr;
}


/* IFORCEFEED_FeedFromPeek
*/
static int IFORCEFEED_FeedFromPeek(IForceFeed *me, IPeek *piPeek)
{
   int32 nCode = IPEEK_FULL;
   int nErr = 0;

   while (!nErr && nCode == IPEEK_FULL) {
      char * pcBuf;
      int32 nAvail;

      pcBuf = IPEEK_Peek(piPeek, IPEEK_PEEKMAX, &nAvail, &nCode);
      if (!nAvail) {
         break;
      }
      if (SUCCESS == (nErr = IFORCEFEED_Write(me, pcBuf, nAvail))) {
         IPEEK_Advance(piPeek, nAvail);
      }
   }

   if (!nErr) {
      if (nCode == IPEEK_END) {
         nErr = IFORCEFEED_Write(me, NULL, 0);

      } else if (nCode == IPEEK_ERROR) {
         nErr = IPEEK_ERROR;
      }
   }

   return nErr;
}


/* ResObj_InitializeFromStream
*/
static int ResObj_InitializeFromStream(ResObj *me, IQueryInterface *piqi, IAStream *pias)
{
   int nErr;
   IForceFeed *piForceFeed = 0;
   IPeek *piPeek = 0;

   nErr = IQI_QueryInterface(piqi, AEEIID_FORCEFEED, (void**)&piForceFeed);

   // we're gonna need an ISourceUtil
   if (!nErr && !me->piSourceUtil) {
      nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_SOURCEUTIL, (void**)&me->piSourceUtil);
   }
   // convert IAStream to an IPeek
   if (!nErr) {
      ISOURCEUTIL_PeekFromAStream(me->piSourceUtil, pias, &piPeek);
   }
   // pass IPeek to the IForceFeed so it can graze
   if (!nErr) {
      IFORCEFEED_Reset(piForceFeed);
      nErr = IFORCEFEED_FeedFromPeek(piForceFeed, piPeek);
   }

   RELEASEIF(piForceFeed);
   RELEASEIF(piPeek);

   return nErr;
}

#endif    // FULL_FUNCTIONALITY



/* ResObj_LoadObject
*/
int ResObj_LoadObject(ResObj *me, uint16 nResID, AEECLSID clsidBase, IBase **ppo, 
                      ResDecoderCache *pResDecoderCache) 
{
   int nErr;
   IBase *piBase = 0;
   IBitmap *pib = 0;
   IAStream *pias = 0;
   IImageDecoder *piDecoder = 0;
   AEECLSID clsId = 0;
   char szMimeType[32];
   AEECLSID viewerClsId = 0;

   nErr = ResObj_GetStreamMimeType(me, nResID, RESTYPE_IMAGE, &pias, szMimeType, sizeof(szMimeType));
   if(!nErr) {
      nErr = ResDecoderCache_GetDecoder(pResDecoderCache, szMimeType+1, CAST(IBase**, &piDecoder), &clsId);
      if(!nErr) {
         viewerClsId = (clsId ==AEECLSID_BMPDECODER)? AEECLSID_GENERICVIEWER_CONVERT: AEECLSID_GENERICVIEWER;      
         nErr = ISHELL_CreateInstance(me->piShell, viewerClsId, (void**) &piBase);
         IIMAGE_SetParm(CAST(IImage*,piBase), IPARM_DECODER, (int)piDecoder, 0);
      }
      else {
         nErr = ResObj_GetHandler(me, clsidBase, szMimeType+1, &clsId);
         if(!nErr && clsId) {
            nErr = ISHELL_CreateInstance(me->piShell, clsId, (void **)&piBase);
         }
      }
   }
   

   if (!nErr && piBase != NULL) {
      if (clsidBase == HTYPE_VIEWER ||
          clsidBase == AEECLSID_VIEW) {

         IIMAGE_SetStream(CAST(IImage*, piBase), pias);

      } else if (clsidBase == HTYPE_SOUND ||
                 clsidBase == AEECLSID_SOUNDPLAYER) {

         ISOUNDPLAYER_SetStream(CAST(ISoundPlayer*, piBase), pias);

#ifdef FULL_FUNCTIONALITY
      } else if (0x03010200 <= GETAEEVERSION(0,0,0)) {

         IQueryInterface *piqi = (IQueryInterface*)piBase;
         nErr = ResObj_InitializeFromStream(me, piqi, pias);

         // special case for DIBs
         if (!nErr && (AEECLSID_DIB == clsid)) {
            IImageDecoder *piid = 0;
            nErr = IQI_QueryInterface(piqi, AEEIID_IMAGEDECODER, (void**)&piid);
            if (!nErr) {
               nErr = IIMAGEDECODER_GetBitmap(piid, &pib);
            }
            RELEASEIF(piid);
         }
#endif // FULL_FUNCTIONALITY
      }
   }

   if (!nErr) {
      *ppo = pib ? CAST(IBase*, pib) : piBase;
      IBASE_AddRef(*ppo);
   }
   RELEASEIF(piDecoder);
   RELEASEIF(piBase);
   RELEASEIF(pib);
   RELEASEIF(pias);

   return nErr;
}



///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

/* ResObj_BufferLoadDataEx
|| 
|| Description:
||    Load part of a resource from the resource file into buffer. 
||    The load operation starts at an offset from the beginning of the resource.
|| 
|| Prototype:
||    int ResObj_BufferLoadDataEx(
||       ResObj * me, 
||       uint16   nResID,        // ID of the resource to load
||       uint16   nType,         // type of resource to load
||       void *   pBuffer,       // destination buffer to fill with resource data 
||       uint32   dwBufSize,     // size of buffer pointed to by pBuffer in bytes 
||       uint32 * pdwResSize     // size in bytes of remaining resource data (after offsetting)
||       uint32   cbOffset       // offset in bytes into resource to load
||    );
|| 
|| Return Value:
||    SUCCESS              resource was successfully loaded into memory at pBuffer
||    ERESOURCENOTFOUND    resource not found, invalid ID or type
|| 
|| Note:
||    To determine the size of a resource without attempting to load it, pass 0 for dwBufSize. 
||    The buffer size required to load the entire resource will be returned in *pdwResSize.
|| 
*/
int ResObj_BufferLoadDataEx(ResObj *me, uint16 nResID, uint16 nType, void *pBuffer, 
                            uint32 dwBufSize, uint32 *pdwResSize, uint32 cbOffset)
{
   int nErr;
   uint32 dwOffset;
   uint32 dwSize;

   nErr = ResObj_Find(me, nResID, nType, &dwOffset, &dwSize);
   if (!nErr) {

      uint32 dwRequest = dwSize - cbOffset;

      // return the resource size (size remaining after offsetting)
      *pdwResSize = dwRequest;

      // check if buffer is big enough
      if (dwBufSize < dwRequest) {
         dwRequest = dwBufSize;
      }
      // we can still load a partial resource if buffer size is non-zero
      if (dwRequest) {
         nErr = ResObj_SeekReadBlock(me, dwOffset + cbOffset, pBuffer, dwRequest); 
      }
   }

   return nErr;
}




/* ResObj_BufferLoadData
|| 
|| Description:
||    Load a resource from the resource file into buffer. 
|| 
|| Prototype:
||    int ResObj_BufferLoadData(
||       ResObj * me, 
||       uint16   nResID,        // ID of the resource to load                     
||       uint16  nType,         // type of resource to load                      
||       void *   pBuffer,       // destination buffer to fill with resource data 
||       uint32   dwBufSize,     // size of buffer pointed to by pBuffer in bytes 
||       uint32 * pdwResSize     // size of resource in bytes (if found)                   
||    );
|| 
|| Return Value:
||    SUCCESS              resource was successfully loaded into memory at pBuffer
||    ERESOURCENOTFOUND    resource not found, invalid ID or type
|| 
|| Note:
||    To determine the size of a resource without attempting to load it, pass 0 for dwBufSize. 
||    The buffer size required to load the entire resource will be returned in *pdwResSize.
|| 
*/
int ResObj_BufferLoadData(ResObj *me, uint16 nResID, uint16 nType, void *pBuffer, uint32 dwBufSize, uint32 *pdwResSize)
{
   return ResObj_BufferLoadDataEx(me, nResID, nType, pBuffer, dwBufSize, pdwResSize, 0);
}



/* ResObj_LoadString
|| 
|| Description: 
||    Loads a string resource into a provided buffer
|| 
|| Prototype:
||    int ResObj_LoadString(
||       ResObj * me, 
||       uint16   nResID,        // ID of the string resource to load
||       AECHAR * pwchBuffer,    // pointer to AECHAR array buffer to be filled with resource string
||       uint32   dwBufSize      // sizeof buffer pointed to by pwchBuffer
||       uint32 * pdwResSize     // required sizeof buffer to load resource
||    );
||    
|| 
|| Return Value:  
||    SUCCESS              resource was successfully loaded into memory at pwchBuffer
||    ERESOURCENOTFOUND    resource not found, invalid ID or type
||    ENOMEMORY            out of memory error occurred
|| 
*/
int ResObj_LoadString(ResObj *me, uint16 nResID, AECHAR *pwchBuffer, uint32 dwBufSize, uint32 *pdwResSize)
{
   int nErr = 0;

   // if not already in our buffer
   if (!me->pBuffer || (me->nLastResID != nResID)) {

      uint32 dwOffset;

      // save resource ID
      me->nLastResID = nResID;

      // find offset of resource
      nErr = ResObj_Find(me, nResID, RESTYPE_STRING, &dwOffset, &me->cbRawSize);

      // expand buffer if necessary
      if (!nErr) {
         nErr = MemGrowEx((void**)&me->pBuffer, &me->cbBuf, 0, (int)me->cbRawSize, 40);
      }

      // seek and read raw resource data
      if (!nErr) {
         nErr = ResObj_SeekReadBlock(me, dwOffset, me->pBuffer, me->cbRawSize); 
      }
   }

   if (!nErr) {
      nErr = ResObj_DecodeString(me, pwchBuffer, (int)(dwBufSize / sizeof(AECHAR)), me->pBuffer,
                                 (int)me->cbRawSize, pdwResSize);
   }

   return nErr;
}


int ResObj_GetMappedData(ResObj *me, uint16 nResID, uint16 nType, uint32 *pdwResSize, void **ppo) 
{ 
   int nErr;
   uint32 dwOffset;

   *ppo = 0;

   if (!me->pMapped) {
      return EFAILED;
   }

   nErr = ResObj_Find(me, nResID, nType, &dwOffset, pdwResSize);
   if (!nErr) {
      *ppo = (byte*)me->pMapped + dwOffset;
   }

   return nErr; 
}


void ResObj_SetICharsetConv(ResObj *me, ICharsetConv *piConv)
{
   RELEASEIF(me->piConv);
   me->piConv = piConv;
   ADDREFIF(me->piConv);
}


void ResObj_SetCharsetString(ResObj *me, const char *pszTo)
{
   FREEIF(me->pszConvTo);
   if (pszTo) {
      me->pszConvTo = STRDUP(pszTo);
   }
}


int ResObj_LoadImage(ResObj *me, uint16 nResID, IImage *ppo)
{
   (void) me; (void) nResID; (void) ppo;
   return EFAILED;
}



void *ResObj_GetUserPtr(ResObj *me)
{
   return me->pUser;
}


void  ResObj_SetUserPtr(ResObj *me, void *pUser)
{
   me->pUser = pUser;
}


boolean ResObj_IsMapped(ResObj *me)
{
   return (boolean) (me->pMapped != NULL);
}


int ResObj_GetSize(ResObj *me, uint16 nResID, uint16 nType, uint32 *pdwResSize)
{
   uint32 dwOffset;
   return ResObj_Find(me, nResID, nType, &dwOffset, pdwResSize);
}


/////////////////////////////////////////////////////////////////////////////
// New/Delete


/* ResObj_Delete
*/
void ResObj_Delete(ResObj *me)
{
   RELEASEIF(me->piFile);
   RELEASEIF(me->piShell); 
   RELEASEIF(me->piSourceUtil); 
   RELEASEIF(me->piConv);
   FREEIF(me->pszConvTo);
   FREEIF(me->pBuffer);
   FREE(me);
}


/* ResObj_New
*/
int ResObj_New(ResObj **ppo, IShell *piShell, const char *pszResFile)
{
   ResObj *me = 0;
   IFile *piFile = 0;
   IFileMgr *piFileMgr = 0;
   ResHeader hdr;
   uint32 nOffsetSize = 0;
   int nErr = 0;
   boolean bTryToMap = 0;
   void *pMapped = 0;

   (void) bTryToMap;

// NOTE: we don't support big endian and mapped files simultaneously
#if !defined(FEATURE_BIG_ENDIAN)
   bTryToMap = (0x03000000 <= GETAEEVERSION(0,0,0));
#endif

#if defined(RESFILE_TEST)
   nErr = MappedFileMgr_New(&piFileMgr, piShell);
#else
   nErr = ISHELL_CreateInstance(piShell, AEECLSID_FILEMGR, (void**)&piFileMgr);
#endif

   if (!nErr) {
      nErr = IFILEMGR_OpenResFile(piFileMgr, pszResFile, &piFile, &hdr);
      nOffsetSize = sizeof(uint32) * (hdr.dwOffsetCount+1);
   }

#ifdef IFILE_Map
   if (!nErr && piFile != NULL && bTryToMap) {
      pMapped = IFILE_Map(piFile, 0,0,0,AEE_FMAP_SHARED,0);
      // Unaligned data access is unpredictable on ARM implementations.  If the file
      // has been mapped onto a non-dword aligned address, abort the map attempt
      // and load the file into heap instead.
      if (pMapped && ((int) pMapped & 0x3)) {
         pMapped = NULL; // Pretend that we couldn't map the file
      }
   }
#endif

   if (!nErr) {
      uint32 cbExtra = pMapped ? 0 : (nOffsetSize + hdr.dwMapLength);
      me = MALLOCREC_EX(ResObj, cbExtra);
      if (!me) {
         nErr = ENOMEMORY;
      }
   }

   if (!nErr && me != NULL) {

      // store pMapped
      me->pMapped = pMapped;

      // store the IFile
      me->piFile = piFile;
      IFILE_AddRef(me->piFile);

      me->piShell = piShell;
      ISHELL_AddRef(me->piShell);

      // store the ResHeader info
      MEMCPY(&me->hdr, &hdr, sizeof(ResHeader));

      // setup pOffsets and pMap
      if (me->pMapped) {
         me->pOffsets = (uint32*) ((byte*)me->pMapped + me->hdr.dwOffsetOffset);
         me->pMap = (ResMap*) ((byte*)me->pMapped + me->hdr.dwMapOffset);
      } else {
         me->pOffsets = CAST(uint32*, me + 1);
         me->pMap = (ResMap*) ((byte*)me->pOffsets + nOffsetSize);

         nErr = ResObj_Setup(me);
      }
   }

   if (!nErr) {
      *ppo = me;
   } else if (me) {
      ResObj_Delete(me);
   }

   RELEASEIF(piFile);
   RELEASEIF(piFileMgr);

   return nErr;
}


//##########################################################################
//##########################################################################
#if defined(RESFILE_TEST)
//##########################################################################
//##########################################################################
//### Testing code ###
//
// Create an implementation of a IFileMgr/IFile that simulates a const
// file (ROM-based file), for testing RefFile and ResObj's handling of
// IFILE_Map().


typedef struct MappedFileMgr  MappedFileMgr;
typedef struct MappedFile     MappedFile;

struct MappedFileMgr {
   AEEVTBL(IFileMgr) *  pvt;        // vtbl
   uint32               nRefs;
   IShell  *            piShell;
   IFileMgr *           piFileMgr;  // the real IFileMgr
};


struct MappedFile {
   AEEVTBL(IFile) *     pvt;        // vtbl
   uint32               nRefs;
   byte *               pbData;     // memory-mapped file data
   uint32               dwDataSize;
   uint32               dwFilePos;  // current file position (Seek,Read)
   IFile *              piFile;     // IFile used for some operations
};


/////////////////////////////////////////////////////////////////////////////

#undef MAKE_ME
#define MAKE_ME      MappedFile *me = (MappedFile*)po


static void MappedFile_Dtor(MappedFile *me)
{
   FREEIF(me->pbData);
   RELEASEIF(me->piFile);
}


static uint32 MappedFile_AddRef(IFile *po)
{
   MAKE_ME;
   return ++me->nRefs;
}

static uint32 MappedFile_Release(IFile *po)
{
   MAKE_ME;

   if (me->nRefs == 1) {
      MappedFile_Dtor(me);
      FREE(me);
      return 0;
   }
   return --me->nRefs;
}

static int32 MappedFile_Seek(IFile *po, FileSeekType seek, int32 position)
{
   MAKE_ME;
   return IFILE_Seek(me->piFile, seek, position);
}

static int32 MappedFile_Read(IFile *po, void *pDest, uint32 nWant)
{
   MAKE_ME;
   return IFILE_Read(me->piFile, pDest, nWant);
}


static int32 MappedFile_SetCacheSize(IFile *po, int nSize)
{
   MAKE_ME;
   return IFILE_SetCacheSize(me->piFile, nSize);
}


void *MappedFile_Map(IFile *po, void *pStart, uint32 dwSize, int protections, int flags, uint32 dwOffset)
{
   MAKE_ME;

   if (!me->pbData) {
      return NULL;
   }

   if (pStart) {
      return NULL;   // We only support returning the address...
   }

   if (dwOffset + dwSize > me->dwDataSize) {
      return NULL;   // out of bounds...
   }

   if (protections & AEE_FMAP_PROT_WRITE) {
      return NULL;   // Read/Exec protections only
   }

   if (flags & (AEE_FMAP_FIXED | AEE_FMAP_PRIVATE)) {
      return NULL;   // Not supported
   }

   if (0 == (flags & AEE_FMAP_SHARED)) {
      return NULL;   // Same base address returned for any requestor...
   }

   return (void*) (me->pbData + dwOffset);
}


void MappedFile_Ctor(MappedFile *me, AEEVTBL(IFile) *pvt, IFile *piFile)
{
   me->nRefs = 1;

   me->piFile = piFile;
   IFILE_AddRef(piFile);

   me->pvt = pvt;
   pvt->AddRef       = MappedFile_AddRef;
   pvt->Release      = MappedFile_Release;
   pvt->Seek         = MappedFile_Seek;
   pvt->Read         = MappedFile_Read;
   pvt->SetCacheSize = MappedFile_SetCacheSize;
   pvt->Map          = MappedFile_Map;
   // WARNING: Other IFile methods must not be called without
   // adding their methods to this vtbl
   // Remainder of vtbl is left uninitialized because:
   // 1) This is just test code
   // 2) We know exactly how the IFile gets used within this module

}

static IFile *MappedFile_New(IFile *piFile)
{
   MappedFile *me = 0;
   int nErr = 0;
   FileInfo fi;

   if (!piFile) {
      return NULL;
   }

   me = MALLOCREC_VTBL(MappedFile, IFile);
   if (!me) {
      nErr = ENOMEMORY;
   }

   if (!nErr) {
      MappedFile_Ctor(me, GETVTBL(me, IFile), piFile);
      nErr = IFILE_GetInfo(piFile, &fi);
   }

   if (!nErr) {
      me->dwDataSize = fi.dwSize;
      me->pbData = MALLOC(me->dwDataSize);
      if (!me) {
         nErr = ENOMEMORY;
      }
   }

   if (!nErr) {
      int32 cb = IFILE_Read(piFile, me->pbData, me->dwDataSize);
      if ((uint32)cb != fi.dwSize) {
         nErr = EFAILED;
      }
   }

   if (nErr && me) {
      MappedFile_Dtor(me);
      FREE(me);
      me = 0;
   }

   RELEASEIF(piFile);

   return (IFile*) me;
}

/////////////////////////////////////////////////////////////////////////////


#undef MAKE_ME
#define MAKE_ME      MappedFileMgr *me = (MappedFileMgr*)po


static void MappedFileMgr_Dtor(MappedFileMgr *me)
{
   RELEASEIF(me->piShell);
   RELEASEIF(me->piFileMgr);
}

static uint32 MappedFileMgr_AddRef(IFileMgr *po)
{
   MAKE_ME;
   return ++me->nRefs;
}

static uint32 MappedFileMgr_Release(IFileMgr *po)
{
   MAKE_ME;

   if (me->nRefs == 1) {
      MappedFileMgr_Dtor(me);
      FREE(me);
      return 0;
   }
   return --me->nRefs;
}


static IFile *MappedFileMgr_OpenFile(IFileMgr *po, const char *pszFile, OpenFileMode mode)
{
   MAKE_ME;

   IFile *piFile = IFILEMGR_OpenFile(me->piFileMgr, pszFile, mode);
   return MappedFile_New(piFile);
}


static int MappedFileMgr_GetLastError(IFileMgr *po)
{
   MAKE_ME;
   return IFILEMGR_GetLastError(me->piFileMgr);
}


static int MappedFileMgr_Construct(MappedFileMgr *me, AEEVTBL(IFileMgr) *pvt, IShell *piShell)
{
   me->piShell = piShell;
   ISHELL_AddRef(piShell);

   me->nRefs = 1;
   me->pvt = pvt;

   pvt->AddRef       = MappedFileMgr_AddRef;
   pvt->Release      = MappedFileMgr_Release;
   pvt->OpenFile     = MappedFileMgr_OpenFile;
   pvt->GetLastError = MappedFileMgr_GetLastError;

   // WARNING: Other IFileMgr methods must not be called without
   // adding their methods to this vtbl
   // Remainder of vtbl is left uninitialized because:
   // 1) This is just test code
   // 2) We know exactly how the IFileMgr gets used within this module

   return ISHELL_CreateInstance(me->piShell, AEECLSID_FILEMGR, (void**)&me->piFileMgr);
}


static int MappedFileMgr_New(IFileMgr **ppo, IShell *piShell)
{
   int nErr;
   MappedFileMgr *me = MALLOCREC_VTBL(MappedFileMgr, IFileMgr);

   *ppo = 0;

   if (!me) {
      return ENOMEMORY;
   }

   nErr = MappedFileMgr_Construct(me, GETVTBL(me, IFileMgr), piShell);

   if (!nErr) {
      *ppo = (IFileMgr*)me;
   } else {
      MappedFileMgr_Dtor(me);
      FREE(me);
   }

   return nErr;
}


#endif   // #if defined(RESFILE_TEST)

