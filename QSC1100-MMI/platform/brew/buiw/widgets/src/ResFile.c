/*
  ========================================================================

  FILE:  ResFile.c
  
  SERVICES: 

  GENERAL DESCRIPTION: IResFile Implementation

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/


#include "ResFile.h"
#include "wutil.h"
#include "AEEStdLib.h"
#include "AEEFile.h"


/////////////////////////////////////////////////////////////////////////////
// Dictionary

#define SEP_CHAR_SEMI         ';'
#define ID_TOP_DICT           0
#define DICT_REQ_VERSION      0x00000001

typedef struct {
   uint32   dwVersion;
   uint32   dwNumAssocs;
} DictionaryHeader;

#define DICT_HEADER_SIZE      sizeof(DictionaryHeader)
#define DICT_VERSION(p)       ((DictionaryHeader*)(p))->dwVersion
#define DICT_NRASSOCS(p)      ((DictionaryHeader*)(p))->dwNumAssocs
#define DICT_ASSOCS(d)        ((Assoc*)((char*)(d) + DICT_HEADER_SIZE))
#define DICT_STRINGS(d)       ((char*)DICT_ASSOCS(d) + DICT_NRASSOCS(d) * sizeof(Assoc))

// creation and extraction of ID and index values
#define FILEINDEX(id)         (uint16)((id) >> 16)
#define MAKEID(ndx,id)        (((uint32)(ndx) << 16) | ((id) & 0xFFFF))



// Assoc data struct 
typedef struct {
   uint16 key;
   uint16 type; 
   uint32 id;
} Assoc;



// Dictionary object
typedef struct Dictionary Dictionary;
struct Dictionary {
   char *      pStrings;         // Res strings
   Assoc *     pAssocs;          // Res associations
   uint32      nrAssocs;         // No. of assocs
   uint16 *    aOffsets;         // index to string offsets table
   int         nNumStrings;      // number of strings in pStrings
   // overallocated -- dictionary data follows...
};


/////////////////////////////////////////////////////////////////////////////
// Dictionary methods

//#define DEBUG_DICTIONARY
#ifdef DEBUG_DICTIONARY

static const char *Dictionary_GetAssocName(Dictionary *me, Assoc *pAssoc);

// recursive function
static void Dictionary_Dump(Dictionary *me, Assoc *pAssoc, char *pBuffer, char *pAppend, int cbRem)
{
   // Analyze parent and count children
   for (; pAssoc->type != 0; pAssoc++) {

      const char *pszName = Dictionary_GetAssocName(me, pAssoc);
      int nNameLen = (int)STRLEN(pszName);

      STRLCPY(pAppend, pszName, (unsigned)cbRem);
      DBGPRINTF("%s type=%d, id=%d", pBuffer, (int)pAssoc->type, (int)pAssoc->id);

      // if node has children...descend using recursion
      if (pAssoc->type == RESTYPE_DICT && pAssoc->id < me->nrAssocs) {
         // ... as long as there is remaining space in buffer
         if (cbRem - nNameLen - 1 > 0) {
            char *pszDot = ".";
            STRLCPY(pAppend+nNameLen, pszDot, (unsigned)(cbRem-nNameLen));
            Dictionary_Dump(me, me->pAssocs + pAssoc->id, pBuffer, pAppend+nNameLen+1, 
                            cbRem-nNameLen-1);
         }
      }
   }
}

static void Dictionary_DebugDump(Dictionary *me)
{
#define BUFSIZE   4096

   if (me && me->pAssocs) {
      char *psz = MALLOC(BUFSIZE);
      if (psz) {
         *psz = 0;
         Dictionary_Dump(me, me->pAssocs, psz, psz, BUFSIZE-1);
         FREE(psz);
      }
   }
}

#endif // DEBUG_DICTIONARY


static const char *Dictionary_GetAssocName(Dictionary *me, Assoc *pAssoc)
{
   if (pAssoc->key < me->nNumStrings) {
      return me->pStrings + me->aOffsets[pAssoc->key];
   }
   return "";  // handle this error somewhat gracefully
}


/* Dictionary_Lookup
*/
int Dictionary_Lookup(Dictionary *me, const char *pszName, Assoc **ppAssoc)
{
   Assoc *pa = me->pAssocs;
   Assoc *paSave = pa;

   if (!pszName || !*pszName) {
      return EFAILED;
   }

   while (*pszName && pa && pa->type) {

      const char *pszAssoc = Dictionary_GetAssocName(me, pa);

      // check if strings are equal
      if (0 == STRCMP(pszAssoc, pszName)) {
 
         // match! - save assoc pointer
         paSave = pa;

         // then prepare to descend
         if (pa->type == RESTYPE_DICT && pa->id < me->nrAssocs) {
            pa = me->pAssocs + pa->id;
         }

         // index to next name
         pszName += STRLEN(pszName) + 1;

      } else {
         // index to next sibling node
         pa++;
      }
   }

   // success when we've exhaustively matched the entire string
   if (!*pszName && paSave) {
      *ppAssoc = paSave;
      return SUCCESS;
   }

   return ERESOURCENOTFOUND;
}


/* Dictionary_CalcStringTable
|| 
|| Build an array of uint16 offset values that map string indexes
|| to offsets from the start of the me->pStrings table.  Used for 
|| quick lookup of strings in the Assoc tree.
*/
static int Dictionary_CalcStringTable(Dictionary *me, const char *pDictDataEnd)
{
   int cbAlloc = 0;
   int cbUsed = 0;
   int nErr = 0;
   char *p = me->pStrings;

#define NOMSTRINGSIZE   10

   // guesstimate the required size of the string table
   int nInitialAlloc = ((pDictDataEnd - p) / NOMSTRINGSIZE) * (int)sizeof(uint16);

   // perform initial allocation
   nErr = MemGrow((void**)&me->aOffsets, &cbAlloc, 0, nInitialAlloc);

   // fill the string table (and grow as required)
   while (!nErr && p < pDictDataEnd) {
      uint16 nOffset = (uint16) (p - me->pStrings);
      nErr = MemAppendEx((void**)&me->aOffsets, &cbAlloc, &cbUsed, 
                         &nOffset, sizeof(nOffset), (nInitialAlloc/2));
      p += STRLEN(p) + 1;
      me->nNumStrings++;
   }

   return nErr;
}


/* Dictionary_BuildChildList
|| 
|| Description:
||    Build an array of name strings of children found in parent
|| 
|| Prototype:
||    int Dictionary_BuildChildList(
||       Dictionary * me,
||       Assoc *      pParent,      // Pointer to parent dictionary node
||       char ***     pppszNames,   // address of pointer to allocated char* array
||       int *        pcbAlloc,     // pointer to sizeof char* array
||       int *        pnChildren    // pointer to where 'number of children' will be stored
||    );
|| 
|| Return Value:
||    SUCCESS        No error occurred (but may have found no children)
||    EFAILED        Bad parent or state
||    ENOMEMORY      Memory allocation error
|| 
|| Note:   
||    Since this function is designed to be called iteratively,
||    *pnChildren MUST be initialized to zero on first call to 
||    get a valid count
||    
||    This function MALLOCs and REALLOCs an array of char*, and returns the
||    pointer to the array in pppszNames
|| 
||    pppszNames is the address of 'ppszNames' which is a pointer
||    to an array of char pointers
|| 
||    pppszNames -> ppszNames --> +----------+
||                                | pszName1 | --> points into string table
||                                +----------+
||                                | pszName2 | -->   "      "    "      "
||                                +----------+
||                                | pszName3 |
||                                +----------+
|| 
*/ 
int Dictionary_BuildChildList(Dictionary *me, Assoc *pParent, char ***pppszNames,
                              int *pcbAlloc, int *pnChildren)
{
   Assoc *pAssoc = 0;
   int nErr = 0;

#undef ALLOCCHUNK 
#define ALLOCCHUNK   (10 * sizeof(char*))    // allocate multiple array entries at a time

   // Validate passed in parent and parent type
   if (!pParent || pParent->type != RESTYPE_DICT || pParent->id >= me->nrAssocs) {
      return EFAILED;
   }

   // Analyze parent and count children
   for (pAssoc = me->pAssocs + pParent->id; !nErr && pAssoc->type != 0; pAssoc++) {

      int nChildren;

      // Find child name
      boolean bDuplicate = 0;
      const char *pszName = Dictionary_GetAssocName(me, pAssoc);

      // no name? skip it
      if (!*pszName) {
         continue;
      }

      // get our current count
      nChildren = *pnChildren;

      // protect against initial non-allocated name array
      if (*pppszNames) {
         int i;
         // check array for duplicates
         for (i=0; i < nChildren; i++) {
            // we have to do a full STRCMP here instead of
            // just comparing pointer values, because the two names 
            // may be in different Dictionaries, and thus come from 
            // different string tables
            if (0 == STRCMP(pszName, (*pppszNames)[i])) {
               bDuplicate = 1;   // found, don't add it again
               break;
            }
         }
      }

      // append pszName to array if it's not a duplicate
      if (!bDuplicate) {
         int cbUsed = nChildren * (int)sizeof(char*);
         // add the value of the pointer to the pointer array
         nErr = MemAppendEx((void**)pppszNames, pcbAlloc, &cbUsed, (void*)&pszName, 
                            sizeof(pszName), ALLOCCHUNK);
         (*pnChildren)++;  // increment count
      }
   }

   return nErr;
}


void Dictionary_Delete(Dictionary *me)
{
   FREEIF(me->aOffsets);
   FREE(me);
}


/* Dictionary_New
|| 
|| Create a dictionary object if a dictionary is present in the 
|| resource file.
|| 
|| Return value:
||    SUCCESS        Dictionary object successfully created/loaded.
||    EUNABLETOLOAD  Resource file does not have a valid dictionary.
||    ENOMEMORY      Out of memory error occurred.
*/
int Dictionary_New(Dictionary **ppo, ResObj *pResObj)
{
   int nErr;
   uint32 dwDictSize;      // no dict jokes, please...
   uint32 dwActual;
   Dictionary *me = 0;
   void *pDictData = 0;
   boolean bMapped = ResObj_IsMapped(pResObj);

   // get the dictionary resource size
   nErr = ResObj_GetSize(pResObj, ID_TOP_DICT, RESTYPE_DICT, &dwDictSize);

   if (nErr == ERESOURCENOTFOUND) {
      nErr = EUNABLETOLOAD;
   }

   // if size is less than header size - unable to load
   if (!nErr && dwDictSize <= DICT_HEADER_SIZE) {
      nErr = EUNABLETOLOAD;
   }

   // allocate me and read in the dictionary data
   if (!nErr) {
      if (bMapped) {
         if (NULL == (me = MALLOCREC(Dictionary))) {
            nErr = ENOMEMORY;
         }
         else {
            nErr = ResObj_GetMappedData(pResObj, ID_TOP_DICT, RESTYPE_DICT, &dwActual, &pDictData);
         }
      } else {
         if (NULL == (pDictData = MALLOC(sizeof(Dictionary) + dwDictSize))) {
            nErr = ENOMEMORY;
         }
         else {
            // this looks funky, but fixes lint error 662, where the pointer
            // arithmetic happening later is incorrectly value tracked, and
            // lint thinks pDictData's going off the deep end
            me = pDictData;
            pDictData = (char*)pDictData + sizeof(Dictionary);
            nErr = ResObj_BufferLoadData(pResObj, ID_TOP_DICT, RESTYPE_DICT, pDictData, 
                                         dwDictSize, &dwActual);
         }
      }
   }

   if (!nErr && me != NULL && pDictData != NULL) {
      uint32 cbAssoc;
      
      me->nrAssocs = DICT_NRASSOCS(pDictData);
      me->pStrings = DICT_STRINGS(pDictData);
      me->pAssocs  = DICT_ASSOCS(pDictData);

      cbAssoc = (uint32)(me->pStrings - (char*)pDictData) - DICT_HEADER_SIZE;

      if (DICT_VERSION(pDictData) != DICT_REQ_VERSION 
            || me->nrAssocs * sizeof(Assoc) != cbAssoc) {
         nErr = EUNABLETOLOAD;
      }
   }

   if (!nErr && me != NULL && pDictData != NULL) {
      nErr = Dictionary_CalcStringTable(me, (char*)pDictData + dwDictSize);
   }

#ifdef DEBUG_DICTIONARY
   if (!nErr) {
      DBGPRINTF("===== Dumping dictionary =====");
      Dictionary_DebugDump(me);
   }
#endif

   if (!nErr) {
      *ppo = me;
   } else {
      FREEIF(me);
   }

   return nErr;
}


/////////////////////////////////////////////////////////////////////////////
// ResObj shim for Get API (pBuf == -1)


/* ResObj_GetInfernal (sic)
*/
static int ResObj_GetInfernal(ResObj *me, uint16 nResID, uint16 nType, void *pBuf, uint32 *pLen)
{
   int nErr;
   uint32 dwBufSize;

   // Check length parameter
   if (!pLen) {
      return EBADPARM;
   }

   dwBufSize = *pLen;

   // pBuf == -1 means they just want the resource size
   // (busted API)
   if (pBuf == (void*)-1) {
      pBuf = 0;
      dwBufSize = 0;
   }

   if (nType == RESTYPE_STRING) {
      nErr = ResObj_LoadString(me, nResID, (AECHAR*)pBuf, dwBufSize, pLen);
   } else {
      nErr = ResObj_BufferLoadData(me, nResID, nType, pBuf, dwBufSize, pLen);
   }

   return nErr;
}


/////////////////////////////////////////////////////////////////////////////
// ResFile


static __inline Dictionary *ResObj_GetDictionary(ResObj *me) {
   return (Dictionary*) ResObj_GetUserPtr(me);
}


static void ResFile_Cleanup(ResFile *me)
{
   // Cleanup our resource files   
   if (me->ppResObj) {

      ResObj **pp = me->ppResObj;
      int nn = me->nFileCount;

      // delete all ResObj's
      for (; nn--; pp++) {
         Dictionary *pDict = ResObj_GetDictionary(*pp);
         if (pDict) {
            Dictionary_Delete(pDict);
         }
         ResObj_Delete(*pp);
      }

      FREE(me->ppResObj);
      me->ppResObj = 0;
   }

   me->nFileCount = 0;
}


static int ResFile_FindAndOpenFiles(ResFile *me, const char *pszNames)
{
   char *pResNames = 0;
   char *pMax;
   char *psz;
   int cbAlloc = 0;
   int nErr = 0;

#define RESOBJCHUNK  (sizeof(ResObj*) * 4)

   pResNames = STRDUP(pszNames);
   if (!pResNames) {
      return ENOMEMORY;
   }
   // find limit for scanning filename list
   pMax = pResNames + STRLEN(pResNames);

   // replace ';' with '\0'
   for (psz=pResNames; *psz; psz++) if (*psz == SEP_CHAR_SEMI) *psz = 0;

   // try to create a ResObj for each filename
   for (psz=pResNames; *psz && psz < pMax;)  {

      ResObj *pResObj = 0;

      nErr = ResObj_New(&pResObj, me->piShell, psz);

      // set ICharConv on ResObj
      if (!nErr && pResObj && me->piConv) {
         ResObj_SetICharsetConv(pResObj, me->piConv);
         ResObj_SetCharsetString(pResObj, me->pszConvTo);
      }

      // grow our ResObj array
      if (!nErr) {
         int cbUsed = me->nFileCount * (int)sizeof(ResObj*);
         nErr = MemAppendEx((void**)&me->ppResObj, &cbAlloc, &cbUsed, &pResObj, sizeof(ResObj*), RESOBJCHUNK);
      }

      // increment file count and try to create dictionary
      if (!nErr) {
         Dictionary *pDict;

         me->nFileCount++;
         nErr = Dictionary_New(&pDict, pResObj);
         if (!nErr) {
            ResObj_SetUserPtr(pResObj, pDict);
         } else if (nErr == EUNABLETOLOAD) {
            nErr = 0;
         }
      }

      // fatal error
      if (nErr == ENOMEMORY) {
         break;
      }

      // skip to next file name
      psz += STRLEN(psz) + 1;
   }

   FREE(pResNames);

   // fix up error code if one or more files did not load
   if (nErr == EFILENOEXISTS && me->nFileCount) {
      nErr = SUCCESS;
   }

   return nErr;
}


int ResFile_Open(IResFile *po, const char *pszNames)
{
   ResFile *me = CAST(ResFile *, po);
   int nErr;

   // already open, must close first
   if (me->ppResObj) {
      return EFILEOPEN;
   }

   nErr = ResFile_FindAndOpenFiles(me, pszNames);

   if (!nErr && !me->nFileCount) {
      nErr = EUNABLETOLOAD;
   }

   if (nErr) {
      ResFile_Cleanup(me);
   }

   return nErr;
}


void ResFile_Close(IResFile *po)
{
   ResFile *me = CAST(ResFile *, po);
   ResFile_Cleanup(me);
}





///////////////////////////////////////////////////////////////////////////////
// utility inlines for BuildName


// check if 'ch' is a separation char
static __inline boolean IsSepChar(char ch) {
   return ('.' == ch || '/' == ch);
}

// check if 'psz' is a root specifier
// (a string containing a single separation char)
static __inline boolean IsRootSpec(char *psz) {
   return (boolean) (IsSepChar(*psz) && (0 == *(psz+1)));
}

#define CBEXPAND  64

static __inline int MemAppendString(char** pp, int *pcb, int *pcbUsed,
                                    char *psz, int nLen) {
   return MemAppendEx((void**)pp, pcb, pcbUsed, psz, nLen, CBEXPAND);
}

static __inline int MemAppendChar(char** pp, int *pcb, int *pcbUsed, char ch) {
   return MemAppendEx((void**)pp, pcb, pcbUsed, &ch, 1, CBEXPAND);
}


/* ResFile_BuildName
||
|| Builds a double-zero terminated string of names used 
|| for search into dictionary.
|| 
|| Example:  
|| input:  "System", "Dialog.Softkeys"
|| output: "System\0Dialog\0Softkeys\0"
*/
static int ResFile_BuildName(ResFile *me, VaListPtrType pArgs)
{
   char *pName = 0;
   int cbUsed = 0;
   int nErr = 0;
   va_list args;

   VaListPtr_To_va_list(pArgs, &args);

   // build name strings from args
   while (!nErr && NULL != (pName = va_arg(args, char *))) {

      int nLen = (int) STRLEN(pName);
      // skip leading sep char, unless length is 1
      if (nLen > 1 && (0 == cbUsed) && IsSepChar(*pName)) {
         nLen--;
         pName++;
      }
      // append string to me->pszLookup
      if (nLen) {
         // append sep char
         if (cbUsed > 0) {
            nErr = MemAppendChar(&me->pszLookup, &me->cbAlloc, &cbUsed, '.');
         }
         if (!nErr) {
            nErr = MemAppendString(&me->pszLookup, &me->cbAlloc, &cbUsed, pName, nLen);
         }
      }
   }

   // double-z terminate
   if (!nErr) {
      nErr = MemAppendString(&me->pszLookup, &me->cbAlloc, &cbUsed, "\0", sizeof("\0"));
   }

   // validate the completed lookup string --
   // make sure there are no double separation chars
   if (!nErr) {
      char *p = 0;
      for (p=me->pszLookup; *p; p++) {
         if (IsSepChar(*p) && IsSepChar(*(p+1))) {
            *me->pszLookup = 0;
            return EFAILED;
         }
      }
      // if not root specifier, change dots & slashes to zeros 
      if (!IsRootSpec(me->pszLookup)) {
         for (p=me->pszLookup; *p; p++) {
            if (IsSepChar(*p)) {
               *p = 0;
            }
         }
      }
   }

   return nErr;
}


static int ResFile_FindResource(ResFile *me, int ndxStart, uint16 nResID, uint16 nType, ResObj **ppo)
{
   int nn = ndxStart;

   // cycle through resource files, looking for id,type
   for (; nn < me->nFileCount; nn++) {
      uint32 dwSize;
      ResObj *pResObj = me->ppResObj[nn];
      if (SUCCESS == ResObj_GetSize(pResObj, nResID, nType, &dwSize)) {
         *ppo = pResObj;
         return SUCCESS;
      }
   }

   return ERESOURCENOTFOUND;
}


static int ResFile_FindNamedResourceV(ResFile *me, uint16 nType, ResObj **ppo, 
                                      Assoc **ppAssoc, int *pnIndex, VaListPtrType args)
{
   ResObj **pp = me->ppResObj;
   int nn;
   int nErr;

   // collate args into string list
   if (SUCCESS != (nErr = ResFile_BuildName(me, args))) {
      return nErr;
   }

   // cycle through resource files, looking for named entry
   for (nn=0; nn < me->nFileCount; nn++, pp++) {
      Assoc *paFound = 0;
      Dictionary *pDict = ResObj_GetDictionary(*pp);
      nErr = 0;

      if (!pDict) {
         continue;
      }

      nErr = Dictionary_Lookup(pDict, me->pszLookup, &paFound);
      if (!nErr && ((0 == nType) || (paFound->type == nType))) {
         if (ppo) {
            *ppo = *pp;
         }
         if (ppAssoc) {
            *ppAssoc = paFound;
         }
         if (pnIndex) {
            *pnIndex = nn;
         }
         return SUCCESS;
      }
   }

   return ERESOURCENOTFOUND;
}


int ResFile_Get(IResFile *po, uint32 nResID, uint16 nType, void *pBuf, uint32 *pLen)
{
   ResFile *me = CAST(ResFile *, po);

   int nErr;
   ResObj *pResObj = 0;

   nErr = ResFile_FindResource(me, FILEINDEX(nResID), (uint16)nResID, nType, &pResObj);
   if (!nErr) {
      nErr = ResObj_GetInfernal(pResObj, (uint16)nResID, nType, pBuf, pLen);
   }

   return nErr;
}


int ResFile_GetSource(IResFile *po, uint32 nResID, uint16 nType, ISource **ppo)
{
   ResFile *me = CAST(ResFile *, po);

   int nErr;
   ResObj *pResObj = 0;

   nErr = ResFile_FindResource(me, FILEINDEX(nResID), (uint16)nResID, nType, &pResObj);
   if (!nErr) {
      nErr = ResObj_LoadSource(pResObj, (uint16)nResID, nType, ppo);
   }

   return nErr;
}


int ResFile_GetObject(IResFile *po, uint32 nResID, AEECLSID clsid, void **ppo)
{
   ResFile *me = CAST(ResFile *, po);

   ResObj *pResObj = 0;
   int nErr;

   nErr = ResFile_FindResource(me, FILEINDEX(nResID), (uint16)nResID, RESTYPE_IMAGE, &pResObj);
   if (!nErr) {
      nErr = ResObj_LoadObject(pResObj, (uint16)nResID, clsid, (IBase**)ppo, me->pResDecoderCache);
   }

   return nErr;
}

int ResFile_GetNamedV(IResFile *po, uint16 nType, void *pBuf, uint32 *pLen, VaListPtrType args)
{
   ResFile *me = CAST(ResFile *, po);
   int nErr;
   Assoc *pAssoc = 0;
   ResObj *pResObj = 0;

   nErr = ResFile_FindNamedResourceV(me, nType, &pResObj, &pAssoc, NULL, args);
   if (!nErr) {
      nErr = ResObj_GetInfernal(pResObj, (uint16)pAssoc->id, pAssoc->type, pBuf, pLen);
   }

   return nErr;
}


int ResFile_GetNamedObjectV(IResFile *po, AEECLSID clsid, void **ppo, VaListPtrType args)
{
   ResFile *me = CAST(ResFile *, po);
   int nErr;
   uint16 type;
   Assoc *pAssoc = 0;
   ResObj *pResObj = 0;

   if (clsid == HTYPE_VIEWER || clsid == AEECLSID_VIEW) {
      type = RESTYPE_IMAGE;
   } else {
      type = RESTYPE_BINARY;
   }

   nErr = ResFile_FindNamedResourceV(me, type, &pResObj, &pAssoc, NULL, args);
   if (!nErr) {
      nErr = ResObj_LoadObject(pResObj, (uint16)pAssoc->id, clsid, (IBase**)ppo, me->pResDecoderCache);
   }

   return nErr;
}


int ResFile_GetNamedSourceV(IResFile *po, uint16 nType, ISource **ppo, VaListPtrType args)
{
   ResFile *me = CAST(ResFile *, po);
   int nErr;
   Assoc *pAssoc = 0;
   ResObj *pResObj = 0;

   nErr = ResFile_FindNamedResourceV(me, nType, &pResObj, &pAssoc, NULL, args);
   if (!nErr) {
      nErr = ResObj_LoadSource(pResObj, (uint16)pAssoc->id, nType, ppo);
   }
   return nErr;
}

int ResFile_GetNamedTypeV(IResFile *po, uint16* pnType, VaListPtrType args)
{
   ResFile *me = CAST(ResFile *, po);
   int nErr;
   Assoc *pAssoc = 0;

   nErr = ResFile_FindNamedResourceV(me, 0, NULL, &pAssoc, NULL, args);
   if (!nErr) {
      *pnType = pAssoc->type;
   }
   return nErr;
}


// Iterate through all resource files, trying to find the ID matching the provided full name and type
int ResFile_GetNamedIdV(IResFile *po, uint16 nType, uint32 *pnResID, VaListPtrType args)
{
   ResFile *me = CAST(ResFile *, po);
   int nErr;
   int index;
   Assoc *pAssoc = 0;

   nErr = ResFile_FindNamedResourceV(me, nType, NULL, &pAssoc, &index, args);
   if (!nErr) {
      *pnResID = MAKEID(index, pAssoc->id);
   }
   return nErr;
}

int ResFile_GetNamedChildrenV(IResFile *po, void *pBuf, uint32 *pLen, uint32 *pnChildren, VaListPtrType args)
{
   ResFile *me = CAST(ResFile *, po);
   int nErr = 0;
   uint32 nTotalSize = 0;                 // total memory required
   uint32 nTotalBuf = (pLen) ? *pLen : 0; // provided buffer length
   boolean bParentFound = 0;           // was the parent found in any resource file?
   boolean bRoot = 0;
   char ** ppszNames = 0;  // array of name strings (allocated by Dictionary_BuildChildList)
   int cbAlloc = 0;        // sizeof ppszNames array in bytes
   int nChildren = 0;      // number of children found
   char *pDest = pBuf;

   ResObj **pp;            // ResObj iterator 
   int nn;                 // ResObj iterator

   // Validate arguments and state
   if (!me->ppResObj || !me->nFileCount) {
      return EBADSTATE;
   }

   // collate args into string list
   if (SUCCESS != (nErr = ResFile_BuildName(me, args))) {
      return nErr;
   }

   // 'Root' request ?
   if (IsRootSpec(me->pszLookup)) {
      bRoot = 1;
   }

   pp = me->ppResObj;
   nn = me->nFileCount;

   // iterate thru all of our owned ResObj's
   for (; !nErr && nn--; pp++) {

      Assoc *pAssoc;
      Dictionary *pDict = ResObj_GetDictionary(*pp);

      // if the ResObj has a Dictionary...
      if (pDict) {
         if (bRoot) {
            // if requested 'Root' as parent, use a dummy Assoc as the
            // parent for the BuildChildList operation
            Assoc asc;
            asc.key  = 0;
            asc.id   = 0;
            asc.type = RESTYPE_DICT;
            // add Root's children to the ppszNames array
            nErr = Dictionary_BuildChildList(pDict, &asc, &ppszNames, &cbAlloc, &nChildren);
            bParentFound = 1;    // we found a parent (root)
         } else {
            // if we can find the named resource, add its children to the ppszNames array
            if (SUCCESS == Dictionary_Lookup(pDict, me->pszLookup, &pAssoc)) {
               nErr = Dictionary_BuildChildList(pDict, pAssoc, &ppszNames, &cbAlloc, &nChildren);
               bParentFound = 1;    // we found a parent
            }
         }
      }
   }

      // If we couldn't find the named parent in any of the files, let the caller know
   if (!nErr && !bParentFound) {
      nErr = EUNABLETOLOAD;
   }

   // Copy data if applicable
   if (!nErr && ppszNames) {  

      int i;
      // calc buffer size and copy strings
      for (i=0; i < nChildren; i++) {
         if (ppszNames[i]) {
            uint32 nSize = STRLEN(ppszNames[i]) + 1;
            if (pDest && nTotalBuf != 0) {
               STRLCPY(pDest, ppszNames[i], nTotalBuf);
               nTotalBuf = (nTotalBuf > nSize) ? (nTotalBuf - nSize) : 0;
               pDest += nSize;
            }
            nTotalSize += nSize;
         }
      }

      // terminate with extreme prejudice
      nTotalSize++;
      if (pDest && nTotalBuf != 0) {
         *pDest = 0;
      }

      // return length and children, if required
      if (pLen) {
         *pLen = nTotalSize;
      }

      if (pnChildren) {
         *pnChildren = (uint32) nChildren;
      }
   }

   // Free names array
   FREEIF(ppszNames);

   return nErr;
}


////////////////////////////////////////////////////////////////////////////////
// ICharsetConv stuff

static void ResFile_SetICharsetConv(ResFile *me)
{
   ResObj **pp = me->ppResObj;
   int nn = me->nFileCount;

   // iterate thru all of our owned ResObj's
   while (nn--) {
      ResObj_SetICharsetConv(*pp, me->piConv);
      pp++;
   }
}

static void ResFile_SetCharsetString(ResFile *me)
{
   ResObj **pp = me->ppResObj;
   int nn = me->nFileCount;

   // iterate thru all of our owned ResObj's
   while (nn--) {
      ResObj_SetCharsetString(*pp, me->pszConvTo);
      pp++;
   }
}


////////////////////////////////////////////////////////////////////////////////
// IParameters methods

static int ResFile_GetParam(IParameters1 *po, int nId, void *pParam, 
                            int nParamBufLen, int *pnParamLen)
{
   (void)po, (void)nId, (void)pParam, (void)nParamBufLen, (void)pnParamLen;
   return EUNSUPPORTED;
}


static int ResFile_SetParam(IParameters1 *po, int nId, const void *pParam, int nParamLen)
{
   ResFile *me = CAST(ResFile*, po->pMe);

   (void)nParamLen;

   // set the ICharsetConv interface that ResObj will use for conversions
   if (RFPID_CHARSETCONVERTER == nId) {
      RELEASEIF(me->piConv);
      me->piConv = CAST(ICharsetConv*,pParam);
      ADDREFIF(me->piConv);
      ResFile_SetICharsetConv(me);
      return SUCCESS;
   }

   // set the 'to-string' that the ICharsetConv object will be initialized
   // with whenever a conversion needs to be performed
   if (RFPID_CONVERTTOSTRING == nId) {
      FREEIF(me->pszConvTo);
      if (pParam) {
         me->pszConvTo = STRDUP(pParam);
      }
      ResFile_SetCharsetString(me);
      return SUCCESS;
   }

   return EUNSUPPORTED;
}


////////////////////////////////////////////////////////////////////////////////
// IQueryInterface methods

uint32 ResFile_AddRef(IResFile *po)
{
   ResFile *me = CAST(ResFile *, po);
   return ++me->nRefs;
}

uint32 ResFile_Release(IResFile *po)
{
   ResFile *me = CAST(ResFile *, po);
   uint32 nRefs = --me->nRefs;

   if (nRefs == 0) {
      ResFile_Dtor(me);
      FREE(me);
   }

   return nRefs;
}

int ResFile_QueryInterface(IResFile *po, AEECLSID clsid, void **ppo)
{
   ResFile *me = CAST(ResFile *, po);

   if (AEEIID_RESFILE == clsid || 
         AEEIID_RESFILE_1 == clsid || 
         AEECLSID_QUERYINTERFACE == clsid) {

      *ppo = me;
      (void) ResFile_AddRef(po);
      return SUCCESS;
   }

   if (AEEIID_IParameters1 == clsid) {
      *ppo = &me->param;
      (void) ResFile_AddRef(po);
      return SUCCESS;
   }

   *ppo = 0;
   return ECLASSNOTSUPPORT;
}


void ResFile_Dtor(ResFile *me)
{
   ResFile_Cleanup(me);
   ResDecoderCache_Dtor(me->pResDecoderCache);
   me->pResDecoderCache = NULL;
   FREEIF(me->pszLookup);
   FREEIF(me->pszConvTo);
   RELEASEIF(me->piConv);
   RELEASEIF(me->piShell);
   RELEASEIF(me->piModule);
}


int ResFile_Construct(ResFile *me, AEEVTBL(IResFile) *pvt, IModule *piModule, IShell *piShell)
{
   me->piModule = piModule;
   ADDREFIF(piModule);

   me->piShell = piShell;
   ISHELL_AddRef(piShell);

   (void) ResDecoderCache_Ctor(&me->pResDecoderCache, piShell);

   me->nRefs = 1;
   me->pvt = pvt;

   pvt->AddRef             = ResFile_AddRef;
   pvt->Release            = ResFile_Release;
   pvt->QueryInterface     = ResFile_QueryInterface;
   pvt->Open               = ResFile_Open;
   pvt->Close              = ResFile_Close;
   pvt->Get                = ResFile_Get;
   pvt->GetObject          = ResFile_GetObject;
   pvt->GetSource          = ResFile_GetSource;
   pvt->GetNamedV          = ResFile_GetNamedV;
   pvt->GetNamedObjectV    = ResFile_GetNamedObjectV;
   pvt->GetNamedSourceV    = ResFile_GetNamedSourceV;
   pvt->GetNamedIdV        = ResFile_GetNamedIdV;
   pvt->GetNamedChildrenV  = ResFile_GetNamedChildrenV;
   pvt->GetNamedTypeV      = ResFile_GetNamedTypeV;

   AEEBASE_INIT(me, param, &me->vtParameters);
   me->vtParameters.AddRef          = AEEBASE_AddRef(IParameters1);
   me->vtParameters.Release         = AEEBASE_Release(IParameters1);
   me->vtParameters.QueryInterface  = AEEBASE_QueryInterface(IParameters1);
   me->vtParameters.GetParam        = ResFile_GetParam;
   me->vtParameters.SetParam        = ResFile_SetParam;

   me->nFileCount          = 0;

   return SUCCESS;
}

int ResFile_New(IResFile **ppo, IModule *piModule, IShell *piShell) 
{
   ResFile *me = MALLOCREC_VTBL(ResFile, IResFile);

   *ppo = CAST(IResFile *, me);

   if (!me) {
      return ENOMEMORY;
   }

   return ResFile_Construct(me, GETVTBL(me, IResFile), piModule, piShell);
}


