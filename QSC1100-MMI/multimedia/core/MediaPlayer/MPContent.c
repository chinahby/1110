/*=============================================================================
  FILE: MPContent.c

  SERVICES: MPContent object and functions
 
  GENERAL DESCRIPTION:

  PUBLIC CLASSES AND STATIC FUNCTIONS:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright ?2005 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

Revision:
  $Header: //depot/asic/msmshared/apps/MediaPlayer/MPContent.c#18 $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------

===========================================================================*/
#include "MPContent.h"
#include "MediaPlayer.h"

#ifdef MP_FEATURE_ICONTENT
#include "MPFileListWin.h"

// parent category of mediaplayer contents
#define MP_CONTENT_PARENT_CAT   "user"



typedef struct stSelectFile
{
  CMediaPlayer*       m_pme;           // point to MediaPlayer
  IContent*           m_pIContent;     // IContent ptr
  void*               m_pObject;       // IMedia object at callback from ICONTENT_GetObject
  char*               m_pszFile;       // content path and name
  uint16              m_nFileId;       // file index

  AEEContentType      m_dwContentType;
  AEEContentType      m_dwObjectType;
  AEECallback         m_cbObject;
  uint32              m_dwObjectSize;  // object size
  AEEContentStatus    m_eStatus;       // ICONTENT_GetObject calling stastus 
  int                 m_nErr;          // error information

  boolean             m_bPlay;         // TRUE for playing selection
}stSelectFile;

typedef struct stDeleteContent
{
  CMediaPlayer*       m_pme;           // point to MediaPlayer
  IContent*           m_pIContent;     // IContent ptr
  AEECallback         m_cbObject;      
  int32               m_nErr;          // error information
}stDeleteContent;

static void MPContentBrowseCat(stContent* pct);
static void MPContentBrowseSubCat(stContent* pct);
static void MPContentBrowseContent(stContent* pct);

static int MPContentGetCatListFromVarbuffer(void *pvBuffer, uint32 *pnNumSubcats, AEECategoryID **ppadwSubcatList);
//static boolean ValidateCatName(const char* pstr);
static void MPAddBrowsingItem(stContent* pct, const char* pwstr, boolean type);
static boolean MPContentLicenseIsType(AEEContentLicenseType enLicense, AEEContentLicenseType enCheckLicense);

static int  MPContentCheckLicense(stContent *pct, const char *pURI, boolean *bValid);

static ICategory* MPContentCat(stContent* pct, const AECHAR* pwszCatName);
static int MPCatSetAttribute(ICategory* piCat, 
                                 const AECHAR* pName,
                                 const AECHAR* pDesc,
                                 const byte*   pIcon,
                                 AEECategoryID* pnSubCatList,
                                 int     nListSize,
                                 boolean bUpdate);

static boolean MPContentAddConetent(stContent* pct, 
                                    const AECHAR* pasCat, 
                                    const char* pszFile, 
                                    const char* pszDesc, 
                                    const char* pstrMimeType);

static void MPContentGetObjectCB(void * pData);
static void MPContentFileUpdateCB(void* pdata);
static void MPContentDeleteCB(void *pData);

static int MPContentCatCount(stContent* pct, AECHAR* pwszCatName, int* nCat);
static int MPContentItemCount(stContent* pct, AECHAR* pwszCatName, int* nCT);
static boolean MPContentHasSubCat(stContent* pct, ICategory* piCat, const AECHAR* pwszCatName);

#ifdef FEATURE_MULTISEQUENCER
static int MPSetSequencerWin(stSelectFile *psf);
#endif

static void MPContentUpdateStructInit(stContentUpdate* ptr);
static void MPContentUpdateStructFree(stContentUpdate* ptr);

static stSelectFile* MPContentSelectFileMake(stContent* pct);
static void MPContentSelectFileFree(stSelectFile* pSF);

static stDeleteContent* MPContentDeleteMake(stContent* pct);
static void MPContentDeleteFree(stDeleteContent* pSF);

// this function creates and initialize stContent object
stContent* MPContentMake(CMediaPlayer* pme)
{
  stContent* pct = (stContent*)MALLOC(sizeof(stContent));
  if(pct)
  {
    pct->m_pOwner       = pme;

    pct->m_pStack       = MPItemStoreMake(pme->a.m_pIShell);
    if(!pct->m_pStack)
      DBGPRINTF_ERROR("MP: failed to create stack object.");

    MPContentUpdateStructInit(&pct->m_CTUpdate);
  }
  else
    DBGPRINTF_ERROR("MP: failed to create stContent.");

  return pct;
}

// Free allocated memories and object handler
// this function must be called in pair with MPContentMake
void MPContentFree(stContent* pct)
{
  if(!pct)
    return;

  if(pct->m_pStack)
  {
    AECHAR* pwStr;
    uint32 i;
    uint32 size = MPItemStoreGetSize(pct->m_pStack);
    for(i = 0; i < size; i++)
    {
      pwStr = MPItemStoreGetAt(pct->m_pStack, i);
      FREEIF(pwStr);
    }
    MPItemStoreFree(pct->m_pStack);
  }

  MPContentUpdateStructFree(&pct->m_CTUpdate);

  FREEIF(pct);
}

static stSelectFile* MPContentSelectFileMake(stContent* pst)
{
  stSelectFile* pSF;
  pSF = (stSelectFile*)MALLOC(sizeof(stSelectFile));
  if(pSF)
  {
    MEMSET(pSF, 0, sizeof(stSelectFile));
    if(pst)
      pSF->m_pme = pst->m_pOwner; 
  }
  return pSF;
}

static void MPContentSelectFileFree(stSelectFile* pSF)
{
  if(pSF)
  {
    RELEASEIF(pSF->m_pIContent);
    FREEIF(pSF->m_pszFile);
    FREEIF(pSF);
  }
}

static stDeleteContent* MPContentDeleteMake(stContent* pct)
{
  stDeleteContent* pSF;
  pSF = (stDeleteContent*)MALLOC(sizeof(stDeleteContent));
  if(pSF)
  {
    MEMSET(pSF, 0, sizeof(stDeleteContent));
	  if(pct)
		  pSF->m_pme = pct->m_pOwner; 
    pSF->m_pIContent = NULL;
  }
  return pSF;
}
static void MPContentDeleteFree(stDeleteContent* pSF)
{
  if(pSF)
  {
    RELEASEIF(pSF->m_pIContent);
    FREEIF(pSF);
  }
}


// this function initializes the struct, which is used when updating content attr 
static void MPContentUpdateStructInit(stContentUpdate* ptr)
{
  if(ptr)
  {
    ptr->m_pIContentMgr = NULL;
    ptr->m_pszURI = NULL;
    ptr->m_pszLocalURL = NULL;
    ptr->m_pszMimeType = NULL;
    ptr->m_pwszName = NULL;
    ptr->m_pwszDesc = NULL;
  }
}
// this function frees allocated memory when updating content attr
static void MPContentUpdateStructFree(stContentUpdate* ptr)
{
  if(ptr)
  {
    FREEIF(ptr->m_pszURI);
    FREEIF(ptr->m_pszLocalURL);
    FREEIF(ptr->m_pszMimeType);
    FREEIF(ptr->m_pwszName);
    FREEIF(ptr->m_pwszDesc);
    RELEASEIF(ptr->m_pIContentMgr);
  }
}

// this function finds all cats and contents at the current level.
void MPContentBrowse(stContent* pct)
{
   uint32 i ;
   // if stack size is zero, browse top level
   i =  MPItemStoreGetSize(pct->m_pStack);
   if(i > 0)
   {
     // if we are not at root, browse sub categories and contents
     MPContentBrowseSubCat(pct);
     MPContentBrowseContent(pct);
     return;
   }
   else
     MPContentBrowseCat(pct);

}
// this function finds all top level cats and fill them into buffer
static void MPContentBrowseCat(stContent* pct)
{
   ICategoryMgr     *pCatMgr = NULL ;
   ICategory       **ppiCatList = NULL ;
   IContentBrowser  *pContentBrowser = NULL ;

   CategoryAttr      wAttr ;
   int               nErr ;
   int               nCount ;
   int               i ;
   AECHAR           *pwszStr = NULL ;
   char              szStr[MP_MAX_FILE_NAME] ;

   uint32           *padwParentcatIDList = NULL ;
   uint32            nNumParentcats = 0;

   if ( ISHELL_CreateInstance ( pct->m_pOwner->a.m_pIShell, AEECLSID_CONTENTBROWSER, (void**)(&pContentBrowser) ) != SUCCESS )
   {
      DBGPRINTF_ERROR ("MP: failed to create content browser instance.") ;
      return;
   }

   if(SUCCESS != ICONTENTBROWSER_GetCategoryMgr ( pContentBrowser, &pCatMgr ))
   {
     DBGPRINTF_ERROR ("MP: call to ICONTENTBROWSER_GetCategoryMgr failed.") ;
     goto cleanup;
   }
   if(EBADSTATE == ICATEGORYMGR_EnumCategoryInit ( pCatMgr ))
   {
     DBGPRINTF_ERROR ("MP: call to ICATEGORYMGR_EnumCategoryInit failed.") ;
     goto cleanup;
   }
   if(SUCCESS != ICATEGORYMGR_EnumCategoryInitSortAttr ( pCatMgr, CATEGORYATTR_CATEGORY_DESCRIPTION ))
   {
     DBGPRINTF_ERROR ("MP: call to ICATEGORYMGR_EnumCategoryInitSortAttr failed.") ;
   }

   // browse 3 cat at a time
   nCount = 3 ;
   if ( NULL == (ppiCatList = (ICategory**) REALLOC ( ppiCatList, sizeof (ICategory*) * nCount ) ) )
   {
     DBGPRINTF_ERROR ("MP: failed to allocate memory.") ;
     goto cleanup;
   }

   // get cat count
   nCount = 0 ;
   if(SUCCESS != ICATEGORYMGR_EnumNextCategory ( pCatMgr, &nCount, ppiCatList ))
   {
      DBGPRINTF_ERROR ("MP: call to ICATEGORYMGR_EnumNextCategory failed.\n") ;
      goto cleanup;
   }
   DBGPRINTF_HIGH ("MP: Number of Cabs = %d\n", nCount);

   // reallocate current data buffer
   if(!CMediaPlayer_ResetExplorerData(pct->m_pOwner, nCount))
     goto cleanup;

   do
   {
      // browse cat 3 at a time
      nCount = 3 ;
      nErr = ICATEGORYMGR_EnumNextCategory ( pCatMgr, &nCount, ppiCatList ) ;
      if(SUCCESS != nErr)
      {
        // end of enumeration
        nErr = SUCCESS;
        goto cleanup;
      }

      DBGPRINTF_HIGH ("ICATEGORYMGR_EnumNextCategory returned %d objects", nCount) ;

      if ( NULL != ppiCatList )
      {
         for ( i = 0; i < nCount; i++ )
         {
           nNumParentcats = 0;
           padwParentcatIDList = NULL;
           if ( SUCCESS == ( nErr = ICATEGORY_GetAttr ( ppiCatList[i], CATEGORYATTR_PARENTCATEGORY_LIST, &wAttr ) ) )
           {
             if ( SUCCESS != ( nErr = MPContentGetCatListFromVarbuffer ( wAttr.pVal, &nNumParentcats, &padwParentcatIDList ) ) )
               nNumParentcats = 0;
           }
           FREEIF ( padwParentcatIDList ) ; 


           // find cats without parent, ICATEGORY_GetParentcatList seems not working now
   //        if(AEECATEGORY_ERR_NO_ATTR == ICATEGORY_GetParentcatList(ppiCatList[i], &ParentCatList))
           if(nNumParentcats == 0)
           {
             // get cat name
             ICATEGORY_GetAttr ( ppiCatList[i], CATEGORYATTR_CATEGORY_NAME, &wAttr ) ;
             pwszStr = (AECHAR *) wAttr.pVal ;
            
             szStr[0] = '\0';
             if ( NULL != pwszStr )
             {
               WSTRTOSTR ( pwszStr, szStr, MP_MAX_FILE_NAME ) ;
            //  if(ValidateCatName(szStr))
               {
                 STRLCPY(pct->m_pOwner->m_FileExplorer.m_FileArray[pct->m_pOwner->m_FileExplorer.m_NumFiles].m_szName, szStr, MP_MAX_FILE_NAME);
                 pct->m_pOwner->m_FileExplorer.m_FileArray[pct->m_pOwner->m_FileExplorer.m_NumFiles].m_bIsFile = FALSE;
                 pct->m_pOwner->m_FileExplorer.m_NumFiles++;
               }
             }
             else
             {
               // failed to get name attribute, try description
               ICATEGORY_GetAttr ( ppiCatList[i], CATEGORYATTR_CATEGORY_DESCRIPTION, &wAttr ) ;
               pwszStr = (AECHAR *) wAttr.pVal ;
               if ( NULL != pwszStr )
               {
                 WSTRTOSTR ( pwszStr, szStr, MP_MAX_FILE_NAME );
                 //  if(ValidateCatName(szStr))
                 {
                   STRLCPY(pct->m_pOwner->m_FileExplorer.m_FileArray[pct->m_pOwner->m_FileExplorer.m_NumFiles].m_szName, szStr, MP_MAX_FILE_NAME);
                   pct->m_pOwner->m_FileExplorer.m_FileArray[pct->m_pOwner->m_FileExplorer.m_NumFiles].m_bIsFile = FALSE;
                   pct->m_pOwner->m_FileExplorer.m_NumFiles++;
                 }
               }
             }
             DBGPRINTF_HIGH ("MP: Found Category %s\n", szStr) ;
           }
           RELEASEIF ( ppiCatList[i] ) ; 
         }
      }
   } while ( 0 != nCount ) ;
   
cleanup:   
   FREEIF ( ppiCatList ) ;
   RELEASEIF ( pCatMgr ) ;
   RELEASEIF(pContentBrowser);
}

// this function finds all current level cats and fill them into buffer
static void MPContentBrowseSubCat(stContent* pct)
{
   ICategoryMgr      *pCatMgr = NULL ;
   ICategory         **ppiCatList = NULL ;
   ICategory         *piCat = NULL ;
   ICategory         *piCatSub = NULL ;
   IContentBrowser   *pContentBrowser = NULL ;

   CategoryAttr      wAttr ;
   int               nErr ;
   int               i ;
   AECHAR           *pwszStr = NULL ;
   char              szStr[MP_MAX_FILE_NAME] ;
   AECHAR*           aeStr; 
   uint32            nNumSubcats ;
   uint32            *padwSubcatIDList = NULL ;
   int               nSubCatCount = 0;
   int               nContentCount = 0;

   if (ISHELL_CreateInstance ( pct->m_pOwner->a.m_pIShell, AEECLSID_CONTENTBROWSER, (void**)(&pContentBrowser) ) != SUCCESS )
   {
      DBGPRINTF_ERROR ("MP: failed to create content browser instance.") ;
      return;
   }

   if(SUCCESS != ICONTENTBROWSER_GetCategoryMgr ( pContentBrowser, &pCatMgr ))
   {
     DBGPRINTF_ERROR ("MP: call to ICONTENTBROWSER_GetCategoryMgr failed.") ;
     goto cleanup;
   }

   // get current cat name from stack
   aeStr = MPItemStoreGetTop(pct->m_pStack);
   if(!aeStr)
   {
     DBGPRINTF_ERROR ("MP: cat stack return NULL ptr.") ;
     goto cleanup;
   }

   // get cats and contents count
   if(SUCCESS != MPContentCatCount(pct, aeStr, &nSubCatCount))
   {
     DBGPRINTF_ERROR ("MP: cannot retrieve sub cat count, or count is 0.") ;
   }
   if(SUCCESS != MPContentItemCount(pct, aeStr, &nContentCount))
   {
     DBGPRINTF_ERROR ("MP: cannot retrieve content count.") ;
   }

   // reallocate explore memory for both cats and contents
   if(!CMediaPlayer_ResetExplorerData(pct->m_pOwner, (nSubCatCount + nContentCount)))
     goto cleanup;

   // search all sub cats by this name
   if ( SUCCESS != ( nErr = ICATEGORYMGR_GetCategoryByName ( pCatMgr, aeStr, &piCat ) ) )
   {
     DBGPRINTF_ERROR ("MP: no cats fount.") ;
     goto cleanup;
   }

   // retrieve sub cat ID list
   if (piCat && SUCCESS == ( nErr = ICATEGORY_GetAttr ( piCat, CATEGORYATTR_SUBCATEGORY_LIST, &wAttr ) ) )
   {
     if (SUCCESS == ( nErr = MPContentGetCatListFromVarbuffer ( wAttr.pVal, &nNumSubcats, &padwSubcatIDList ) ) )
     {
       DBGPRINTF_MED("MP: num sub cats: %d", nNumSubcats);

       for ( i = 0; i < nNumSubcats; i++ )
       {
         // get sub ICategory ptr from ID
         if(SUCCESS == ICATEGORYMGR_GetCategory(pCatMgr, padwSubcatIDList[i], &piCatSub))
         {
           // retrieve name now
           if(SUCCESS == ICATEGORY_GetAttr ( piCatSub, CATEGORYATTR_CATEGORY_NAME, &wAttr ))
           {
             pwszStr = (AECHAR *) wAttr.pVal ;
             WSTRTOSTR ( pwszStr, szStr, MP_MAX_FILE_NAME ) ;
             // add into item list      
             MPAddBrowsingItem(pct, szStr, FALSE);
           }
           RELEASEIF ( piCatSub ) ;
         }
       }
     }
     
     FREEIF ( padwSubcatIDList ) ;
   }

cleanup:   
   FREEIF ( ppiCatList ) ;
   RELEASEIF ( pCatMgr ) ;
   RELEASEIF(pContentBrowser);
}

// this function will find all contents under current cat
static void MPContentBrowseContent(stContent* pct)
{
   IContentInfo    **ppiContentInfoList = NULL ;
   IContentBrowser  *pContentBrowser = NULL ;
   ICategoryMgr     *pCatMgr = NULL ;
   IContent         *piContent = NULL ;
   AEEContentAttrDataType      wAttr ;
   int               i ;
   int               nCount = 1 ;
   int               nErr ;
   //AECHAR          **ppaszCatNames = NULL;
   AECHAR            *ppaszCatNames[1];
   int               nCatCount;

   // The overall filter struct
   CONTENTBROWSER_FilterType  stFilter ;

   // List of attribute filters (just allowing 1 filter for now)
   #define NUM_BROWSE_FILTERS    1
   CONTENTBROWSER_AttrFilterListType*  apstAttrFilterList[NUM_BROWSE_FILTERS] ; // array of pointers to structs

   if ( ISHELL_CreateInstance ( pct->m_pOwner->a.m_pIShell, AEECLSID_CONTENTBROWSER, (void**)(&pContentBrowser) ) != SUCCESS )
   {
      DBGPRINTF_ERROR ("MP: failed to create content browser instance.") ;
      return;
   }

   if(SUCCESS != ICONTENTBROWSER_GetCategoryMgr ( pContentBrowser, &pCatMgr ))
   {
     DBGPRINTF_ERROR ("MP: call to ICONTENTBROWSER_GetCategoryMgr failed.") ;
     goto cleanup;
   }

   if(MPItemStoreGetSize(pct->m_pStack) == 0)
   {
     DBGPRINTF_ERROR ("MP: cat stack return 0 size.") ;
     goto cleanup;
   }
/*
   nCatCount = MPItemStoreGetSize(pct->m_pStack);
   if(!nCatCount)
   {
     DBGPRINTF_ERROR ("MP: MPContentBrowseContent cannot be called from root.") ;
     goto cleanup;
   }

   ppaszCatNames = (AECHAR**) MALLOC ( sizeof (AECHAR*) * nCatCount );
   if(ppaszCatNames == NULL)
   {
      DBGPRINTF_ERROR ("MP: failed to allocate memory.\n") ;
      goto cleanup;
   }

   // searching content which belongs to all categories
   // i.e. A&B&C&D
   for(i = 0; i< nCatCount; i++)
   {
     ppaszCatNames[i] = MPItemStoreGetAt(pct->m_pStack, i) ;
   }
*/

   // only search content under current cat
   ppaszCatNames[0] = MPItemStoreGetTop(pct->m_pStack);
   nCatCount = 1;

   // do not browse hidden file
   // GRF TODO : NEED TO ADD FILTER FOR HIDDEN ATTR = TRUE.

   if ( SUCCESS != ICONTENTBROWSER_EnumInit (pContentBrowser) )
   {
      nErr = EFAILED;
      DBGPRINTF_ERROR ("\n[IContTst] call to ICONTENTBROWSER_EnumInit failed\n") ;
      goto cleanup;

   }

   MEMSET ( (void *) &stFilter, 0, sizeof (stFilter) ) ;

   for ( i = 0; i < NUM_BROWSE_FILTERS; i++ )
   {
      apstAttrFilterList[i] = (CONTENTBROWSER_AttrFilterListType *) MALLOC (sizeof (CONTENTBROWSER_AttrFilterListType) ) ;
   }

   stFilter.nAttrIdSortList = (int *) MALLOC (sizeof(int)) ;
   stFilter.bAttrIdSortOrder = (boolean *) MALLOC (sizeof(boolean)) ;

   stFilter.apszCatNameList = (AECHAR **) ppaszCatNames ;
   stFilter.nNumCat = 1;
   stFilter.nAttrIdSortList[0] = CONTENTATTR_NAME ;
   stFilter.bAttrIdSortOrder[0] = TRUE ;
   stFilter.nSortNum = 1;

   if (SUCCESS != ICONTENTBROWSER_EnumSetFilter(pContentBrowser, &stFilter) )
   {
      nErr = EFAILED;
      DBGPRINTF_ERROR ("\n[IContTst] call to SetFilter failed\n") ;
      goto cleanup;
   }

/* OLD ContentBrowser stuff  
   if(SUCCESS != ICONTENTBROWSER_EnumContentInitByCategory ( pContentBrowser, 
                                (const AECHAR**) ppaszCatNames, nCatCount, TRUE, FALSE, NULL, 0 ) )
   {
     DBGPRINTF_ERROR ("MP: call to ICONTENTBROWSER_EnumContentInit failed.") ;
     goto cleanup;
   }
  
   // sort the result by name
   ICONTENTBROWSER_EnumInitSortAttr ( pContentBrowser, CONTENTATTR_NAME ) ;
*/
   // get the count of contents
   nCount = 0 ;
   if ( SUCCESS != ICONTENTBROWSER_EnumNextContent(pContentBrowser, &nCount, NULL) )
   {
      DBGPRINTF_ERROR ("MP: call to ICONTENTBROWSER_EnumNextContent failed.\n") ;
      goto cleanup;
   }
   DBGPRINTF_HIGH ("MP: Number of contents = %d\n", nCount);
   
   // Now enum actual IContentInfo's, 10 at a time
   nCount = 10 ;
   if (NULL == (ppiContentInfoList = (IContentInfo**)REALLOC(ppiContentInfoList, sizeof (IContentInfo*)*nCount)))
   {
     DBGPRINTF_HIGH ("MP: failed to allocate memory.");
     goto cleanup ;
   }
   
   do
   {
      // reset this count back to 10
      nCount = 10 ;
      nErr = ICONTENTBROWSER_EnumNextContent ( pContentBrowser, &nCount, ppiContentInfoList ) ;
      if ( SUCCESS != nErr )
      {
        // done!
        nErr = SUCCESS;
        goto cleanup ;
      }

      if ( 0 != nCount )
      {
         for ( i = 0; i < nCount; i++ )
         {
#ifdef FEATURE_MULTISEQUENCER
           if(pct->m_pOwner->m_bIsMultiSequencer)
           {
             // to retrieve URL, we need to get IContent object
             if (SUCCESS == (nErr =  ICONTENTINFO_QueryInterface ( ppiContentInfoList[i], AEECLSID_CONTENT, (void**)(&piContent))))
             {
               if (SUCCESS == (nErr = ICONTENT_GetAttr ( piContent, CONTENTATTR_LOCAL_URL, &wAttr)))
               {
                 uint32    wExtType;
                 // verify if the file is supported
                 wExtType = CMediaPlayer_FindHandlerType(pct->m_pOwner, (char*)wAttr.pVal);
                 if(!wExtType)
                  continue;
               }
               RELEASEIF(piContent );
             }
           }
#endif     // FEATURE_MULTISEQUNCER  

           if ( SUCCESS == ICONTENTINFO_GetAttr ( ppiContentInfoList[i], CONTENTATTR_URI, &wAttr ) )
           {
             // add to browsing list
             MPAddBrowsingItem(pct, (char*) wAttr.pVal, TRUE);
           }
           RELEASEIF (ppiContentInfoList[i]);
         }
      }
   } while ( 0 != nCount ) ;

cleanup:   
   for ( i = 0; i < NUM_BROWSE_FILTERS; i++ )
   {
      FREEIF (apstAttrFilterList[i]) ;
   }

   FREEIF (stFilter.nAttrIdSortList) ;
   FREEIF (stFilter.bAttrIdSortOrder) ;

//   FREEIF(ppaszCatNames);
   FREEIF(ppiContentInfoList);
   RELEASEIF(pCatMgr);
   RELEASEIF(pContentBrowser);
}

// this function returns sub cat counts
static int MPContentCatCount(stContent* pct, AECHAR* pwszCatName, int* nCat)
{
  IContentBrowser   *pContentBrowser = NULL ;
  ICategoryMgr      *pCatMgr = NULL ;
  ICategory         *piCat = NULL ;
  CategoryAttr       wAttr ;
  uint32            *padwSubcatIDList = NULL ;
  uint32             nNumSubcats = 0;
  int                bRet = EFAILED;

  if (SUCCESS != ( bRet = ISHELL_CreateInstance ( pct->m_pOwner->a.m_pIShell, AEECLSID_CONTENTBROWSER, (void**)(&pContentBrowser))))
  {
    DBGPRINTF_ERROR ("MP: failed to create content browser instance.") ;
    return bRet;
  }

  if(SUCCESS != (bRet = ICONTENTBROWSER_GetCategoryMgr ( pContentBrowser, &pCatMgr)))
  {
    DBGPRINTF_ERROR ("MP: call to ICONTENTBROWSER_GetCategoryMgr failed.") ;
    goto cleanup;
  }

  // get cat ptr by this name
  if(SUCCESS != ( bRet = ICATEGORYMGR_GetCategoryByName ( pCatMgr, pwszCatName, &piCat)))
  {
    DBGPRINTF_ERROR ("MP: no cats fount.") ;
    goto cleanup;
  }

  // retrieve sub cat ID list
  if(piCat && SUCCESS == ( bRet = ICATEGORY_GetAttr(piCat, CATEGORYATTR_SUBCATEGORY_LIST, &wAttr)))
  {
    if(SUCCESS == ( bRet = MPContentGetCatListFromVarbuffer(wAttr.pVal, &nNumSubcats, &padwSubcatIDList)))
    {
      *nCat = nNumSubcats;      
      FREEIF(padwSubcatIDList ) ;
    }
    else
      DBGPRINTF_ERROR("MP: call to MPContentGetCatListFromVarbuffer failed.");

  }
  else
    DBGPRINTF_ERROR("MP: call to ICATEGORY_GetAttr failed.");
   
cleanup:   
   RELEASEIF(piCat);
   RELEASEIF(pCatMgr);
   RELEASEIF(pContentBrowser);
  return bRet;
}

// retrieve content  count under the given category
// this funcion will not look for any sub cats
static int MPContentItemCount(stContent* pct, AECHAR* pwszCatName, int* nCT)
{
   IContentBrowser  *pContentBrowser = NULL ;
   ICategoryMgr     *pCatMgr = NULL ;
   int               nCount;
   //const AECHAR     *ppwszCatNames[1];
   int               bRet = EFAILED;
   int               i;

   // The overall filter struct
   CONTENTBROWSER_FilterType  stFilter ;

   // List of attribute filters (just allowing 1 filter for now)
   #define NUM_BROWSE_FILTERS    1
   CONTENTBROWSER_AttrFilterListType*  apstAttrFilterList[NUM_BROWSE_FILTERS] ; // array of pointers to structs

   AECHAR*           apwszCatNameList[1] ;    // array of pointers to widestrings

   if(!pwszCatName)
   {
      DBGPRINTF_ERROR ("MP: cat name ptr is NULL.") ;
      return bRet;
   }

   if (SUCCESS != (bRet = ISHELL_CreateInstance ( pct->m_pOwner->a.m_pIShell, AEECLSID_CONTENTBROWSER, (void**)(&pContentBrowser))))
   {
      DBGPRINTF_ERROR ("MP: failed to create content browser instance.") ;
      return bRet;
   }

   if(SUCCESS != (bRet = ICONTENTBROWSER_GetCategoryMgr ( pContentBrowser, &pCatMgr)))
   {
     DBGPRINTF_ERROR ("MP: call to ICONTENTBROWSER_GetCategoryMgr failed.") ;
     goto cleanup;
   }

   apwszCatNameList[0] = pwszCatName;

   // do not browse hidden file
   // GRF TODO : NEED TO ADD FILTER FOR HIDDEN ATTR = TRUE.

   if ( SUCCESS != ICONTENTBROWSER_EnumInit (pContentBrowser) )
   {
      bRet = EFAILED;
      DBGPRINTF_ERROR ("\n[IContTst] call to ICONTENTBROWSER_EnumInit failed\n") ;
      goto cleanup;

   }

   MEMSET ( (void *) &stFilter, 0, sizeof (stFilter) ) ;

   for ( i = 0; i < NUM_BROWSE_FILTERS; i++ )
   {
      apstAttrFilterList[i] = (CONTENTBROWSER_AttrFilterListType *) MALLOC (sizeof (CONTENTBROWSER_AttrFilterListType) ) ;
   }

   stFilter.apszCatNameList = (AECHAR **) apwszCatNameList ;
   stFilter.nNumCat = 1;

   if (SUCCESS != ICONTENTBROWSER_EnumSetFilter(pContentBrowser, &stFilter) )
   {
      bRet = EFAILED;
      DBGPRINTF_ERROR ("\n[IContTst] call to SetFilter failed\n") ;
      goto cleanup;
   }
/* OLD ContentBrowser Stuff
   ppwszCatNames[0] = pwszCatName;
   // browse all contents associate with given cats, no hidden file
   if(SUCCESS != (bRet = ICONTENTBROWSER_EnumContentInitByCategory ( pContentBrowser, ppwszCatNames, 1, TRUE, FALSE, NULL, 0)))
   {
     DBGPRINTF_ERROR ("MP: call to ICONTENTBROWSER_EnumContentInit failed.") ;
     goto cleanup;
   }
*/
   // get the count of the number of pieces of content
   nCount = 0 ;
   if(SUCCESS != (bRet = ICONTENTBROWSER_EnumNextContent(pContentBrowser, &nCount, NULL)))
   {
      DBGPRINTF_ERROR ("MP: call to ICONTENTBROWSER_EnumNextContent failed.\n") ;
      goto cleanup;
   }
   
   *nCT = nCount;

cleanup: 

   for ( i = 0; i < NUM_BROWSE_FILTERS; i++ )
   {
      FREEIF (apstAttrFilterList[i]) ;
   }

   RELEASEIF ( pCatMgr ) ;
   RELEASEIF(pContentBrowser);

   return bRet;
}

// this function add a string into filelist window display 
// filetype,
//    TRUE for file
//    FALSE for category/directory
static void MPAddBrowsingItem(stContent* pct, const char* pstr, boolean filetype)
{
  if ( NULL != pstr )
  {
    STRLCPY(pct->m_pOwner->m_FileExplorer.m_FileArray[pct->m_pOwner->m_FileExplorer.m_NumFiles].m_szName, pstr, MP_MAX_FILE_NAME);
    pct->m_pOwner->m_FileExplorer.m_FileArray[pct->m_pOwner->m_FileExplorer.m_NumFiles].m_bIsFile = filetype;
    pct->m_pOwner->m_FileExplorer.m_NumFiles++;

    DBGPRINTF_HIGH ("MP: add content %s", pstr) ;
  }
}


//   Extract category list from varbuffer, taking care of possible 32 bit alignment issues.
//   This call will malloc memory for the category list. It is the caller's
//   responsibility to free the memory. 
static int MPContentGetCatListFromVarbuffer(void *pvBuffer, uint32* pnNumSubcats, AEECategoryID** ppadwSubcatList)
{
   void           *pvTemp = NULL ;
   AEECategoryID  *padwList = NULL ;
   uint32          nSize ;
   int             nErr = SUCCESS ;
   
   *pnNumSubcats = 0 ;
   
   if ( ( NULL == pvBuffer ) || ( NULL == pnNumSubcats ) || ( NULL == ppadwSubcatList ) )
   {
      return EBADPARM ;
   }
   
   nSize = WEBOPT_GETVARBUFFERLENGTH ( pvBuffer ) ;
   *pnNumSubcats = nSize / sizeof ( AEECategoryID ) ;
   
   padwList = (AEECategoryID *) MALLOC ( nSize ) ; 
   
   if ( NULL == padwList )
   {
      nErr = ENOMEMORY ;
      goto cleanup ;
   }
   
   pvTemp = WEBOPT_GETVARBUFFERDATA ( pvBuffer ) ;
   
   MEMCPY ( (void*) padwList, pvTemp, nSize ) ;
   
   *ppadwSubcatList = padwList ;
   
cleanup:
   return nErr ;
}
/*
static boolean ValidateCatName(const char* pstr)
{
  if(STRSTR(pstr, "video"))
    return TRUE;
  else if(STRSTR(pstr, "audio"))
    return TRUE;
  else if(STRSTR(pstr, "image"))
    return TRUE;
  return FALSE;
}
*/

// this function will find the local URL string from given URI id
int MPContentGetLocalURLfromURI(stContent *pct, const char* szURI, char* pstrOutput, int length)
{
  IContentBrowser   *piContentBrowser = NULL ;
  IContentInfo      *piContentInfo = NULL ;
  IContent          *piContent = NULL ;
  AEEContentAttrDataType       wAttr ;
  int               nErr = EFAILED ;

  // create a content browser so we can get some content
  if(pstrOutput == NULL || SUCCESS != ISHELL_CreateInstance ( 
                     pct->m_pOwner->a.m_pIShell, AEECLSID_CONTENTBROWSER, (void**)(&piContentBrowser)))
  {
    DBGPRINTF_ERROR ("MP: failed to create content browser instance, or pstrOutput is NULL.") ;
    return nErr;
  }

  // see if the specific piece of content exists in the content browser
  if(SUCCESS != (nErr = ICONTENTBROWSER_FindContent ( piContentBrowser, szURI, &piContentInfo)))
  {
    DBGPRINTF_ERROR ("MP: failed to call ICONTENTBROWSER_FindContent.\n");
    goto cleanup;
  }

  if(SUCCESS !=  ICONTENTINFO_QueryInterface(piContentInfo, AEECLSID_CONTENT, (void **)&piContent))
  {
    DBGPRINTF_ERROR ("MP: failed to get IContent instance.\n");
    goto cleanup;
  }

  if(SUCCESS == (nErr = ICONTENT_GetAttr ( piContent, CONTENTATTR_LOCAL_URL, &wAttr)))
  {
    STRLCPY(pstrOutput, (char*) wAttr.pVal, length);
  }

cleanup:   
   FREEIF(piContentInfo);
   RELEASEIF(piContent );
   RELEASEIF(piContentBrowser);

  return nErr;
}

// this function will find the URI string from given URL.  
// the function will scan all contents in DB and compare URL strings, Please use with care.
int MPContentGetURIfromLocalURL(stContent *pct, const char* pszURL, char* pstrOutput, int length)
{
  IContentBrowser   *piContentBrowser = NULL ;
  IContentInfo     **ppiContentInfoList = NULL ;
  IContent          *piContent = NULL ;
  AEEContentAttrDataType        wAttr ;
  int                nErr = EFAILED;
  int                nCount;
  boolean            bFond = FALSE;
  int                i;

   // The overall filter struct
   CONTENTBROWSER_FilterType  stFilter ;

   // List of attribute filters (just allowing 1 filter for now)
   #define NUM_BROWSE_FILTERS    1
   CONTENTBROWSER_AttrFilterListType*  apstAttrFilterList[NUM_BROWSE_FILTERS] ; // array of pointers to structs

  // create a content browser so we can get some content
  if(pstrOutput == NULL || SUCCESS != ISHELL_CreateInstance ( 
                     pct->m_pOwner->a.m_pIShell, AEECLSID_CONTENTBROWSER, (void**)(&piContentBrowser)))
  {
    DBGPRINTF_ERROR ("MP: failed to create content browser instance, or pstrOutput is NULL.") ;
    return nErr;
  }

  // enumerate all contents
   if ( SUCCESS != ICONTENTBROWSER_EnumInit (piContentBrowser) )
   {
      nErr = EFAILED;
      DBGPRINTF_ERROR ("\n[IContTst] call to ICONTENTBROWSER_EnumInit failed\n") ;
      goto cleanup;

   }

   MEMSET ( (void *) &stFilter, 0, sizeof (stFilter) ) ;

   for ( i = 0; i < NUM_BROWSE_FILTERS; i++ )
   {
      apstAttrFilterList[i] = (CONTENTBROWSER_AttrFilterListType *) MALLOC (sizeof (CONTENTBROWSER_AttrFilterListType) ) ;
   }

   if (SUCCESS != ICONTENTBROWSER_EnumSetFilter(piContentBrowser, &stFilter) )
   {
      nErr = EFAILED;
      DBGPRINTF_ERROR ("\n[IContTst] call to SetFilter failed\n") ;
      goto cleanup;
   }

/* OLD ContentBrowser
  if(SUCCESS != (nErr = ICONTENTBROWSER_EnumContentInit ( piContentBrowser, FALSE)))
  {
    DBGPRINTF_ERROR ("MP: failed to call ICONTENTBROWSER_EnumContentInit.\n");
    goto cleanup;
  }
*/
  // allocate memory 10 to hold IContentInfo
  nCount = 10 ;
  if (NULL == (ppiContentInfoList = (IContentInfo**)REALLOC(ppiContentInfoList, sizeof (IContentInfo*)*nCount)))
  {
    DBGPRINTF_HIGH ("MP: failed to allocate memory.");
    goto cleanup ;
  }
   
  do
  {
     // reset this count back to 10
    nCount = 10 ;
    nErr = ICONTENTBROWSER_EnumNextContent ( piContentBrowser, &nCount, ppiContentInfoList ) ;
    if ( SUCCESS != nErr || bFond)
    {
      // done!
      nErr = SUCCESS;
      goto cleanup ;
    }

    if (0 != nCount )
    {
      for (i = 0; i < nCount; i++ )
      {
        // the URL string can only be retrieved through IContent
        if (SUCCESS == (nErr =  ICONTENTINFO_QueryInterface ( ppiContentInfoList[i], AEECLSID_CONTENT, (void**)(&piContent))))
        {
          if (SUCCESS == (nErr = ICONTENT_GetAttr ( piContent, CONTENTATTR_LOCAL_URL, &wAttr)))
          {
            if (STRCMP(wAttr.pVal, pszURL) == 0)
            {
              // this is what we wanted, get URI now
              if (SUCCESS == (nErr = ICONTENTINFO_GetAttr ( ppiContentInfoList[i], CONTENTATTR_URI, &wAttr)))
              {
                (void)STRLCPY(pstrOutput, (char*) wAttr.pVal, length);
                bFond = TRUE;
              }
            }
          }
          RELEASEIF(piContent );
        }
        RELEASEIF (ppiContentInfoList[i]);
      }
    }
  } while ( 0 != nCount ) ;

cleanup:   
   for ( i = 0; i < NUM_BROWSE_FILTERS; i++ )
   {
      FREEIF (apstAttrFilterList[i]) ;
   }

  FREEIF(ppiContentInfoList);
  RELEASEIF(piContentBrowser);

  return nErr;
}

// this function will verify the license and create content(media) object
int MPContentSelectFile(stContent *pct, const char* szURI, uint16 nFileId, boolean bPlay)
{
  IContentBrowser         *piContentBrowser = NULL ;
  IContentInfo            *piContentInfo = NULL ;

  stSelectFile*           psf;
  AEEContentAttrDataType             wAttr ;
  uint32                  dwContentType;
  AEEContentConsumeAction enAction ;
  int                     nErr = EFAILED;
  boolean                 bLicanse;
  int                     nLength;
   
   // check license
   if(SUCCESS == MPContentCheckLicense(pct, szURI, &bLicanse))
   {
     if(bLicanse == FALSE)
     {
       DBGPRINTF_HIGH ("MP: %s license verification failed.", szURI) ;
       MP_ErrorDlg(pct->m_pOwner, IDS_ERR_LICENSE_VERIFICATION_FAILED);
       return nErr;
     }
   }
   else
   {
       DBGPRINTF_HIGH ("MP: cannot verify license for %s.", szURI) ;
       //MP_ErrorDlg(pct->m_pOwner, IDS_ERR_LICENSE_VERIFY);
   }

   if ( SUCCESS != (nErr = ISHELL_CreateInstance ( pct->m_pOwner->a.m_pIShell, AEECLSID_CONTENTBROWSER, (void**)(&piContentBrowser))))
   {
      DBGPRINTF_ERROR ("MP: failed to create content browser instance.") ;
      goto cleanup;
   }

   // see if the specific piece of content exists in the content browser
   if ( SUCCESS != (nErr = ICONTENTBROWSER_FindContent ( piContentBrowser, szURI, &piContentInfo )) )
   {
     DBGPRINTF_ERROR ("MP: call to ICONTENTBROWSER_FindContent failed.") ;
     goto cleanup;
   }

   if(NULL == (psf = MPContentSelectFileMake(pct)))
   {
     DBGPRINTF_ERROR ("MP: call to MPContentSelectFileMake failed.") ;
     goto cleanup;
   }

   // get content interface from content info
   if (SUCCESS != (nErr = ICONTENTINFO_QueryInterface ( piContentInfo, AEECLSID_CONTENT, (void**)(&psf->m_pIContent))))
   {
     DBGPRINTF_ERROR ("MP: failed to create content instance") ;
     goto cleanup;
   }

   nLength = STRLEN(szURI) + 1;
   psf->m_pszFile = (char*)MALLOC(nLength + 1);
   if(psf->m_pszFile)
   {
     (void)MEMSET(psf->m_pszFile, 0, nLength + 1);
     // save URI
     STRLCPY(psf->m_pszFile, szURI, nLength);
   }
   else
     DBGPRINTF_ERROR("MP: failed to alloc memory.");

   // save file id
   psf->m_nFileId = nFileId;
   psf->m_bPlay = bPlay;

   // default to media type
   dwContentType = AEECONTENT_TYPE_MEDIA;
   enAction = AEECONTENT_CONSUME_PLAY;
   if ( SUCCESS == ICONTENT_GetAttr ( psf->m_pIContent, CONTENTATTR_TYPE, &wAttr))
   {
      dwContentType = ( uint32 ) wAttr.pVal ;
      switch ( dwContentType )
      {
         case AEECONTENT_TYPE_IMAGE:
            enAction = AEECONTENT_CONSUME_DISPLAY ;
            break ;
            
         case AEECONTENT_TYPE_APPLET:
            enAction = AEECONTENT_CONSUME_EXECUTE ;
            break ;
            
         case AEECONTENT_TYPE_RAW_BINARY:
            enAction = AEECONTENT_CONSUME_EXECUTE ;
            break ;
            
         case AEECONTENT_TYPE_MEDIA:
            enAction = AEECONTENT_CONSUME_PLAY ;
            break ;
            
         default :
            break ;
      }
   }

   // initialize callback
   CALLBACK_Init(&psf->m_cbObject, MPContentGetObjectCB, psf);

   // now make the call to get back an interface to play/display/etc. depending on what type of media
   // was asked for, you will get back an IMedia, IImage, or ISource interface to use normally.
   if ( SUCCESS != (nErr = ICONTENT_GetObject ( psf->m_pIContent,
                                                TRUE,
                                                enAction,
                                                FALSE,
                                                &psf->m_cbObject, 
                                                &psf->m_dwContentType, 
                                                &psf->m_pObject,
                                                &psf->m_dwObjectSize,
                                                &psf->m_eStatus, 
                                                (int32 *)&psf->m_nErr)))
                               
   {
     CALLBACK_Cancel(&psf->m_cbObject);
     MPContentSelectFileFree(psf);
     DBGPRINTF_ERROR("MP: call to ICONTENT_GetObject failed, returned %d", nErr ) ;
   }


cleanup:   
   RELEASEIF ( piContentInfo ) ;
   RELEASEIF ( piContentBrowser);

   return nErr;
}

// Callback function for ICONTENT_GetObject.
static void MPContentGetObjectCB ( void * pData )
{
   stSelectFile *psf ;
   boolean bRet = FALSE;
   psf = (stSelectFile *) pData ;
           
   if(psf == NULL)
   {
     DBGPRINTF_ERROR ("MP: GetObject() callback returns NULL ptr.");
     return;
   }

   if ( SUCCESS == psf->m_nErr )
   {
      // now see if it's renderable
      if ( AEECONTENT_STATUS_RENDERABLE == psf->m_eStatus )
      {
        switch ( psf->m_dwContentType )
        {
          case AEECONTENT_TYPE_MEDIA:
#ifdef FEATURE_MULTISEQUENCER
            if (psf->m_pme->m_bIsMultiSequencer)
            {
              if (SUCCESS == MPSetSequencerWin(psf))
                bRet = TRUE;
            }
            else
#endif //FEATURE_MULTISEQUENCER
            {            
              if(TRUE == (bRet = MPCreatePlayerWin(psf->m_pme, psf->m_pObject, MPPW_PLAY, psf->m_pszFile)))
              {
                if(psf->m_bPlay)
                {
                  (void)ISHELL_PostEvent(psf->m_pme->a.m_pIShell, AEECLSID_MEDIAPLAYER,
                                         EVT_COMMAND, IDM_PM_PLAY, 0);
                }
              }
            }
            break;
          case AEECONTENT_TYPE_IMAGE:
            bRet = MPCreatePlayerWin(psf->m_pme, psf->m_pObject, MPPW_IMAGE, NULL);
            break;
          default:
            DBGPRINTF_ERROR ("MP: unsupported type.") ;
            break;
        }
      }
      else
      {
         DBGPRINTF_HIGH ("MP: Not renderable, status = %d", psf->m_eStatus);
      }
   }
   else
   {
      DBGPRINTF_HIGH ("MP: Error 0x%08x in retrieving object, Status = 0x%08x\n", psf->m_nErr, psf->m_eStatus);
   }

   if(bRet == FALSE)
     MP_RELEASEIF(psf->m_pObject);
   RELEASEIF(psf->m_pIContent);
   MPContentSelectFileFree(psf);
} 
// this function will verify the license and create multiple content(media) objects 
// for playing in sequencer mode
#ifdef FEATURE_MULTISEQUENCER
int MPContentSequencerSelectFiles(stContent *pct)
{
  int nErr = EFAILED;
  uint16    nIndex;
  CMediaPlayer *pme;
  CSequencePlayerWin *pWin;

  pme = pct->m_pOwner;
  pWin = (CSequencePlayerWin *)pme->m_pWin;

  if (pme->m_MSFiles.m_wNumSequences == 0)
  {
    /*
     * No files were selected in the sequence
     */
    MP_ErrorDlg(pme, IDS_ERR_NOSEQUENCE);
    return nErr;;
  }

  if (!CMediaPlayer_SetWindow(pme, MPW_SEQUENCE_PLAYER, 0))
  {
    return nErr;;
  }

  pWin = (CSequencePlayerWin *)pme->m_pWin;

  for (nIndex = 0; nIndex < pme->m_MSFiles.m_wNumSequences; nIndex++)
  {
    MPContentSelectFile(pct, pme->m_MSFiles.m_szSequenceFiles[nIndex], nIndex, FALSE);
  }

  return nErr;
}

static int MPSetSequencerWin(stSelectFile *psf)
{
  CMediaPlayer* pme = psf->m_pme;    
  CSequencePlayerWin *pWin;
  int nErr = EFAILED;
  uint32 nIdx;

  if(pme)
  {
    uint32    dwCaps;

    pWin = (CSequencePlayerWin *)pme->m_pWin;
    if(!pWin || !psf->m_pObject)
     return nErr;

    nIdx = (uint32)psf->m_nFileId;
    (void)IMEDIA_GetMediaParm(psf->m_pObject, MM_PARM_CAPS, (int32 *) & dwCaps, 0);
    pWin->m_sSequenceFile[nIdx].m_bImage = (dwCaps & MM_CAPS_VIDEO) ? TRUE : FALSE;
   
    (void)IMEDIA_SetRect(psf->m_pObject, &pWin->m_sSequenceFile[nIdx].m_rectImage, NULL);
    
    // enable multi channel play
    if(SUCCESS != (nErr = IMEDIA_EnableChannelShare(psf->m_pObject, TRUE)))
    {
      DBGPRINTF_ERROR("MP: call to IMEDIA_EnableChannelShare failed.");
    }
     
    if (SUCCESS != (nErr = IMEDIA_RegisterNotify(psf->m_pObject, MP_SequencePlayerWin_MediaNotify, pWin->m_pOwner)))
    {
      DBGPRINTF_ERROR("MP: call to IMEDIA_RegisterNotify failed.");
    }
   
    if(SUCCESS == nErr)
      pWin->m_sSequenceFile[nIdx].m_pMedia = psf->m_pObject;
    else
      pWin->m_sSequenceFile[nIdx].m_pMedia = NULL;
  }
  return nErr;
}
#endif //FEATURE_MULTISEQUENCER

// return SUCCESS if content can be verified,
//   bValid is TRUE if license is valid, otherwise bValid is FALSE
static int MPContentCheckLicense(stContent *pct, const char *pURI, boolean *bValid)
{
  IContentBrowser   *piContentBrowser = NULL ;
  IContentInfo      *piContentInfo = NULL ;
  IContent          *piContent = NULL ;
  AEEContentAttrDataType       wAttr ;
  int               nRet;
 
  if (SUCCESS != (nRet = ISHELL_CreateInstance ( pct->m_pOwner->a.m_pIShell, AEECLSID_CONTENTBROWSER, (void**)(&piContentBrowser))))
  {
    DBGPRINTF_ERROR ("MP: failed to create content browser instance.") ;
    return nRet;
  }

  if (SUCCESS != (nRet = ICONTENTBROWSER_FindContent ( piContentBrowser, pURI, &piContentInfo )))
  {
    DBGPRINTF_ERROR ("MP: call to ICONTENTBROWSER_FindContent failed.\n") ;
    goto cleanup;
  }

  if (SUCCESS != (nRet = ICONTENTINFO_GetAttr(piContentInfo, CONTENTATTR_LICENSE_TYPE, &wAttr)))
  {
    DBGPRINTF_ERROR ("MP: call to ICONTENTINFO_GetAttr failed.\n") ;
    goto cleanup;
  }

  *bValid = !MPContentLicenseIsType ( (int32)wAttr.pVal, AEECONTENT_LICENSE_EXPIRED );

cleanup:
  RELEASEIF(piContent ) ;
  RELEASEIF(piContentInfo);
  RELEASEIF(piContentBrowser);
  return nRet;
}

// return TRUE if two license type is equal, otherwise FALSE
static boolean MPContentLicenseIsType ( AEEContentLicenseType enLicense, 
                                        AEEContentLicenseType enCheckLicense )
{
   boolean bRet  = TRUE;
   
   if(AEECONTENT_LICENSE_NON_PROTECTED == enLicense)
   {
     bRet = FALSE;
   }
   else
   {
     if(enCheckLicense == ((uint32) enLicense & enCheckLicense))
       bRet = TRUE;
     else
       bRet = FALSE;
   }
   return bRet;
}

// add an entry in content DB
//   pszCatName, category name
//   pszCatDesc, category description (option)
//   pszFile, file path and name
//   pszFileDesc, file description (option)
//   pstrMimeType, file mine type
void MPContentAdd(stContent* pct, 
                  const char* pszCatName, const char* pszCatDesc,
                  const char* pszFile,    const char* pszFileDesc, 
                  const char* pstrMimeType)
{
  AECHAR* pwstrCatName = NULL;
  AECHAR* pwstrCatDesc = NULL;
  ICategory* pCat = NULL;
  ICategory* pParentCat = NULL;
  AECHAR* pwstrParentCatName = NULL;
  AEECategoryID catID[1];
  int nRet;

  if(pct && pszCatName)
  {
    pwstrCatName = MPWStrDup(pszCatName);
    pwstrCatDesc = MPWStrDup(pszCatDesc);
    if(!pwstrCatName || !pwstrCatDesc)
    {
      DBGPRINTF_ERROR("MP: failed to allocate memory.");
      goto cleanup;
    }

    // create/get cat ptr
    pCat = MPContentCat(pct, pwstrCatName);
    if(pCat != NULL)
    {
      // add new attributes
      (void)MPCatSetAttribute(pCat, pwstrCatName, pwstrCatDesc, NULL, NULL, 0, FALSE);
      // get cat ID
      catID[0] = 0;
      if(SUCCESS == ICATEGORY_GetID(pCat, &catID[0]))
      {
        // we want set all mp cats under user
        pwstrParentCatName = MPWStrDup(MP_CONTENT_PARENT_CAT);
        if(pwstrParentCatName)
        {
          // get/create parent cat ptr
          pParentCat = MPContentCat(pct, pwstrParentCatName);
          if(pParentCat)
          {
            // set cat attributes
            if(!MPContentHasSubCat(pct, pParentCat, pwstrCatName))
            {
              // now set sub cat list
              if(SUCCESS != (nRet = MPCatSetAttribute(pParentCat, NULL, NULL, NULL, catID, 1, TRUE)))
                DBGPRINTF_ERROR("MP: call to MPCatSetAttribute failed, %d.", nRet);
            }
            RELEASEIF(pParentCat);
          }
          FREEIF(pwstrParentCatName);
        }
        else
          DBGPRINTF_ERROR("MP: failed to allocate memory.");
      }
      else
        DBGPRINTF_ERROR("MP: failed to get cat ID.");
    }
    else 
    {
      DBGPRINTF_ERROR("MP: call to MPCatSetAttribute failed, 0x%x.", pCat);
    }
    // release cat now
    RELEASEIF(pCat);

    // add content
    MPContentAddConetent(pct, pwstrCatName, pszFile, pszFileDesc, pstrMimeType);
  }
  else
    DBGPRINTF_ERROR("MP: invalid attribute, pct = 0x%x, pszCatName = 0x%x.",pct, pszCatName);

cleanup:
  FREEIF(pwstrCatName);
  FREEIF(pwstrCatDesc);
}

// called from MPContentAdd, this function will add an entry into content DB
static boolean MPContentAddConetent(stContent* pct, 
                                    const AECHAR* pasCat, 
                                    const char* pszFile, 
                                    const char* pszDesc, 
                                    const char* pstrMimeType)
{
  int nRet;
  AEEContentAttrDataType  wAttr[ 2 ];
  AEEContentWStringList     stCatList ;

  if(pct->m_CTUpdate.m_pIContentMgr || pct->m_CTUpdate.m_pszURI)
  {
    DBGPRINTF_ERROR ("MP: content update is in processing, try again.");
    return FALSE;
  }

  if(SUCCESS != ISHELL_CreateInstance(pct->m_pOwner->a.m_pIShell, AEECLSID_CONTENTMGR,(void**)&pct->m_CTUpdate.m_pIContentMgr))
  {
    DBGPRINTF_ERROR ("MP: failed to create m_pIContentMgr.") ;
    return FALSE;
  }

  if(!pct->m_CTUpdate.m_pszURI)
  {
    wAttr[ 1 ].nId =  CONTENTATTR_END;
    pct->m_CTUpdate.m_pszURI = MPStrDup(MP_GetFileName(pszFile));

    // set file url/location
    pct->m_CTUpdate.m_pszLocalURL = MPStrDup(pszFile);
    wAttr[ 0 ].nId  = CONTENTATTR_LOCAL_URL;
    wAttr[ 0 ].pVal = (void *)pct->m_CTUpdate.m_pszLocalURL;
    nRet = ICONTENTMGR_AddContentAttr ( pct->m_CTUpdate.m_pIContentMgr, &pct->m_CTUpdate.m_pszURI, NULL, wAttr ) ;
    if ( SUCCESS != nRet )
    {
      DBGPRINTF_ERROR("MP: failed to set content attribute, %d", nRet ) ;
    }

    // name 
    pct->m_CTUpdate.m_pwszName = MPWStrDup(MP_GetFileName(pszFile));
    wAttr[ 0 ].nId = CONTENTATTR_NAME ;
    wAttr[ 0 ].pVal = (void *)pct->m_CTUpdate.m_pwszName;
    nRet = ICONTENTMGR_AddContentAttr ( pct->m_CTUpdate.m_pIContentMgr, &pct->m_CTUpdate.m_pszURI, NULL, wAttr ) ;
    if ( SUCCESS != nRet )
    {
      DBGPRINTF_ERROR("MP: failed to set content attribute, %d", nRet ) ;
    }

    // description 
    pct->m_CTUpdate.m_pwszDesc = MPWStrDup(pszDesc);
    wAttr[ 0 ].nId = CONTENTATTR_DESCRIPTION ;
    wAttr[ 0 ].pVal = (void *)pct->m_CTUpdate.m_pwszDesc;
    nRet = ICONTENTMGR_AddContentAttr ( pct->m_CTUpdate.m_pIContentMgr, &pct->m_CTUpdate.m_pszURI, NULL, wAttr ) ;
    if ( SUCCESS != nRet )
    {
      DBGPRINTF_ERROR("MP: failed to set content attribute, %d", nRet ) ;
    }

    // license type
    wAttr[ 0 ].nId = CONTENTATTR_LICENSE_TYPE ;
    wAttr[ 0 ].pVal = (void *) AEECONTENT_LICENSE_NON_PROTECTED ;
    nRet = ICONTENTMGR_AddContentAttr ( pct->m_CTUpdate.m_pIContentMgr, &pct->m_CTUpdate.m_pszURI, NULL, wAttr ) ;
    if ( SUCCESS != nRet )
    {
      DBGPRINTF_ERROR("MP: failed to set content attribute, %d", nRet ) ;
    }

    // Mime type
    pct->m_CTUpdate.m_pszMimeType = MPStrDup(pstrMimeType);
    wAttr[ 0 ].nId = CONTENTATTR_MIME_TYPE ;
    wAttr[ 0 ].pVal = (void *) pct->m_CTUpdate.m_pszMimeType ;
    nRet = ICONTENTMGR_AddContentAttr ( pct->m_CTUpdate.m_pIContentMgr, &pct->m_CTUpdate.m_pszURI, NULL, wAttr ) ;
    if ( SUCCESS != nRet )
    {
      DBGPRINTF_ERROR("MP: failed to set content attribute, %d", nRet ) ;
    }

    // Storage Mime type
    wAttr[ 0 ].nId = CONTENTATTR_STORAGE_MIME_TYPE ;
    wAttr[ 0 ].pVal = (void *) pct->m_CTUpdate.m_pszMimeType ;
    nRet = ICONTENTMGR_AddContentAttr ( pct->m_CTUpdate.m_pIContentMgr, &pct->m_CTUpdate.m_pszURI, NULL, wAttr ) ;
    if ( SUCCESS != nRet )
    {
      DBGPRINTF_ERROR("MP: failed to set content attribute, %d", nRet ) ;
    }

    // content type
    wAttr[ 0 ].nId = CONTENTATTR_TYPE ;
    wAttr[ 0 ].pVal = (void *) AEECONTENT_TYPE_MEDIA ;
    ICONTENTMGR_AddContentAttr ( pct->m_CTUpdate.m_pIContentMgr, &pct->m_CTUpdate.m_pszURI, NULL, wAttr ) ;
    if ( SUCCESS != nRet )
    {
      DBGPRINTF_ERROR("MP: failed to set content attribute, %d", nRet ) ;
    }

    stCatList.apwszStrList = ( AECHAR **) MALLOC ( 5 * sizeof (AECHAR *));
    stCatList.apwszStrList[0] = (AECHAR *) MALLOC (40 * sizeof (AECHAR *));
    if(stCatList.apwszStrList == NULL || stCatList.apwszStrList[0] == NULL)
    {
      DBGPRINTF_ERROR("MP: failed to allocate memory.");
    }

    // add attribute: content category list 
    stCatList.dwNumItems = 1 ;
    WSTRLCPY(stCatList.apwszStrList[0], pasCat, 40 * sizeof (AECHAR *));
    wAttr[ 0 ].nId = CONTENTATTR_CATEGORY_LIST;
    wAttr[ 0 ].pVal = (void *) &stCatList ;
    nRet = ICONTENTMGR_AddContentAttr ( pct->m_CTUpdate.m_pIContentMgr, &pct->m_CTUpdate.m_pszURI, NULL, wAttr ) ;
    if(stCatList.apwszStrList == NULL || stCatList.apwszStrList[0] == NULL)
    {
      DBGPRINTF_ERROR("MP: failed to allocate memory.");
    }

    // callback init
    CALLBACK_Init(&pct->m_CTUpdate.m_cbUpdateCallback, MPContentFileUpdateCB, pct);
    
    // commit all changes
    nRet = ICONTENTMGR_Commit(pct->m_CTUpdate.m_pIContentMgr, &pct->m_CTUpdate.m_cbUpdateCallback, &pct->m_CTUpdate.m_nRetContentUpdate); 
    if( SUCCESS != nRet )
    {
      DBGPRINTF_ERROR("MP: failed to commit content updates %d", nRet ) ;

      CALLBACK_Cancel(&pct->m_CTUpdate.m_cbUpdateCallback);
      MPContentUpdateStructFree(&pct->m_CTUpdate);
    }
    FREEIF(stCatList.apwszStrList[0]);
    FREEIF(stCatList.apwszStrList);
  }
      
  // content mge will be released at callback
  return TRUE;
}

// content file update callback
static void MPContentFileUpdateCB(void* pdata)
{ 
  stContent *pct = (stContent *) pdata ;
  if(pct)
  {
    CALLBACK_Cancel(&pct->m_CTUpdate.m_cbUpdateCallback); 

    if(SUCCESS != pct->m_CTUpdate.m_nRetContentUpdate)
    {
      DBGPRINTF_ERROR("MP: content update return error %d", pct->m_CTUpdate.m_nRetContentUpdate ) ;
    }
    
    MPContentUpdateStructFree(&pct->m_CTUpdate);
  }
  else
    DBGPRINTF_ERROR("MP: content update callback returns NULL ptr.") ;
}

// get or create cat
// caller should free ICategory
static ICategory* MPContentCat(stContent* pct, const AECHAR* pwszCatName)
{
   ICategoryMgr     *pCatMgr = NULL ;
   IContentBrowser  *pContentBrowser = NULL ;
   ICategory        *piCat = NULL ;   

   int               nErr = EFAILED ;
   AEECategoryID     CategoryID;

   if ( ISHELL_CreateInstance ( pct->m_pOwner->a.m_pIShell, AEECLSID_CONTENTBROWSER, (void**)(&pContentBrowser) ) != SUCCESS )
   {
      DBGPRINTF_ERROR ("MP: failed to create content browser instance.") ;
      return NULL;
   }

   if(SUCCESS != ICONTENTBROWSER_GetCategoryMgr ( pContentBrowser, &pCatMgr ))
   {
     DBGPRINTF_ERROR ("MP: call to ICONTENTBROWSER_GetCategoryMgr failed.") ;
     goto cleanup;
   }

  // search all sub cats by this name
   nErr = ICATEGORYMGR_GetCategoryByName ( pCatMgr, pwszCatName, &piCat );
   if ( EINVALIDITEM == nErr )
   {
     // want a new CategoryID
     CategoryID = INVALID_CAT_ID;
     if(SUCCESS != ICATEGORYMGR_NewCategory(pCatMgr, &CategoryID, &piCat))
     {
       DBGPRINTF_ERROR ("MP: failed to create new category.") ;
     }
   }
   
cleanup:   
   RELEASEIF ( pCatMgr ) ;
   RELEASEIF(pContentBrowser);

   return piCat;
}

// set new attributes
static int MPCatSetAttribute(ICategory* piCat, 
                                 const AECHAR* pwszName,
                                 const AECHAR* pwszDesc,
                                 const byte*   pIcon,
                                 AEECategoryID* pnSubCatList,
                                 int     nListSize,
                                 boolean bUpdate)
{
  char* pcSubcatList = NULL;
  int nRet = EBADPARM;
  int i = 0;
  CategoryAttr       catAtt[5] ;

  if(!piCat)
    return nRet;

  if(pwszName)
  {
    catAtt[i].nId = CATEGORYATTR_CATEGORY_NAME ;
    catAtt[i].pVal = (void *) pwszName;
    i++;
  }

  if(pwszDesc)
  {
    catAtt[i].nId = CATEGORYATTR_CATEGORY_DESCRIPTION ;
     catAtt[i].pVal = (void *) pwszDesc;
    i++;
  }

  if(pIcon)
  {
    catAtt[i].nId = CATEGORYATTR_ICON ;
    catAtt[i].pVal = (void *) pIcon ;
    i++;
  }


  if(pnSubCatList && (nListSize > 0))
  {
    pcSubcatList = (char *) MALLOC ( 22 ) ;
    WEBOPT_SETVARBUFFER (pcSubcatList, pnSubCatList, sizeof (uint32) * 1) ;
    catAtt[i].nId = CATEGORYATTR_SUBCATEGORY_LIST ;
    catAtt[i].pVal = (void *) pcSubcatList ;
    i++;
  }

  if(i)
  {
    catAtt[i].nId = CATEGORYATTR_END ;

    nRet = ICATEGORY_NewAttr ( piCat, catAtt ) ;
    if(nRet != SUCCESS)
    {
      //Attribute already exists
      if(nRet == EALREADY && bUpdate) 
        nRet = ICATEGORY_UpdateAttr ( piCat, catAtt ) ;
    }
  }

  FREEIF( pcSubcatList ) ;
  return nRet;
}
// return TRUE if piCat has a pwszCatName subcategory, otherwise return FALSE
static boolean MPContentHasSubCat(stContent* pct, ICategory* piCat, const AECHAR* pwszCatName)
{
   ICategoryMgr     *pCatMgr = NULL ;
   IContentBrowser  *pContentBrowser = NULL ;
   ICategory         *piCatSub = NULL ;
   CategoryAttr      wAttr ;
   int               i ;
   AECHAR           *pwszStr = NULL ;
   uint32            nNumSubcats ;
   uint32            *padwSubcatIDList = NULL ;
   int               nErr;
   boolean           bRet = FALSE;

   if ( ISHELL_CreateInstance ( pct->m_pOwner->a.m_pIShell, AEECLSID_CONTENTBROWSER, (void**)(&pContentBrowser) ) != SUCCESS )
   {
      DBGPRINTF_ERROR ("MP: failed to create content browser instance.") ;
      return bRet;
   }

   if(SUCCESS != ICONTENTBROWSER_GetCategoryMgr ( pContentBrowser, &pCatMgr ))
   {
     DBGPRINTF_ERROR ("MP: call to ICONTENTBROWSER_GetCategoryMgr failed.") ;
     goto cleanup;
   }

   // retrieve sub cat ID list
   if (pCatMgr && piCat && (SUCCESS == (nErr = ICATEGORY_GetAttr ( piCat, CATEGORYATTR_SUBCATEGORY_LIST, &wAttr))))
   {
     if (SUCCESS == (nErr = MPContentGetCatListFromVarbuffer ( wAttr.pVal, &nNumSubcats, &padwSubcatIDList)))
     {
       for (i = 0; i < nNumSubcats; i++ )
       {
         // get sub ICategory ptr from ID
         if (SUCCESS == (nErr = ICATEGORYMGR_GetCategory(pCatMgr, padwSubcatIDList[i], &piCatSub)))
         {
           // retrieve name now
           if (SUCCESS == (nErr = ICATEGORY_GetAttr (piCatSub, CATEGORYATTR_CATEGORY_NAME, &wAttr)))
           {
             pwszStr = (AECHAR *) wAttr.pVal ;
             if (WSTRCMP(pwszStr, pwszCatName) == 0)
               bRet = TRUE;
           }
           else
             DBGPRINTF_ERROR("MP: failed to call ICATEGORY_GetAttr, %d.", nErr);

           RELEASEIF ( piCatSub ) ;
         }
         else
           DBGPRINTF_ERROR("MP: failed to call ICATEGORYMGR_GetCategory, %d.", nErr); 
       }
     }
     else if (nErr != SUCCESS)
     {
       DBGPRINTF_ERROR("MP: failed to call ICATEGORY_GetAttr, %d.", nErr); 
     }
     else
     {
       DBGPRINTF_ERROR("MP: invalid attribute, pCatMgr = 0x%x, piCat = 0x%x.",pCatMgr, piCat);
     }
     
     FREEIF ( padwSubcatIDList ) ;
   }

cleanup:   
   RELEASEIF ( pCatMgr ) ;
   RELEASEIF(pContentBrowser);
  
   DBGPRINTF_MED("MP:  has subcategories returning %d", bRet);
   return bRet;
}


int MPContentDelete(stContent* pct, char *pszURI)
{
  IContentBrowser   *piContentBrowser = NULL ;
  IContentInfo      *piContentInfo = NULL ;
  int                nErr = EFAILED ;
  stDeleteContent   *pDelCt = NULL;

  if(SUCCESS != ISHELL_CreateInstance(pct->m_pOwner->a.m_pIShell, 
                                      AEECLSID_CONTENTBROWSER, (void**)(&piContentBrowser)))
  {
    DBGPRINTF_ERROR ("MP: failed to create content browser instance.") ;
    return nErr;
  }

  if(SUCCESS != (nErr = ICONTENTBROWSER_FindContent ( piContentBrowser, pszURI, &piContentInfo)))
  {
    DBGPRINTF_ERROR ("MP: failed to call ICONTENTBROWSER_FindContent.\n");
    goto cleanup;
  }

  if( NULL == (pDelCt = MPContentDeleteMake(pct)))
  {
     DBGPRINTF_ERROR ("MP: call to MPContentDeleteMake failed.") ;
     goto cleanup;
  }

  if(SUCCESS !=  ICONTENTINFO_QueryInterface(piContentInfo, AEECLSID_CONTENT, (void **)&pDelCt->m_pIContent))
  {
    MPContentDeleteFree(pDelCt);
    pDelCt = NULL;
    DBGPRINTF_ERROR ("MP: failed to get IContent instance.\n");
    goto cleanup;
  }

  CALLBACK_Init(&pDelCt->m_cbObject, MPContentDeleteCB, pDelCt);
  // delete media from ICOntent DB
  if(SUCCESS != (nErr = ICONTENT_Delete(pDelCt->m_pIContent, &pDelCt->m_cbObject, &pDelCt->m_nErr)))
  {
    DBGPRINTF_ERROR ("MP: Couldn't delete media from IContent DB, %s. Error %d", pszURI, nErr);
    MPContentDeleteFree(pDelCt);
    pDelCt = NULL;
  }

cleanup:   
   FREEIF(piContentInfo);
   RELEASEIF(piContentBrowser);

  return nErr;
}

static void MPContentDeleteCB(void *pData)
{
  stDeleteContent *pDelCt = pData;
 
  if(pDelCt == NULL)
  {
    DBGPRINTF_ERROR("MP: ICONTENT_Delete() callback returns NULL ptr.");
    return;
  }

  if(SUCCESS == pDelCt->m_nErr )
    DBGPRINTF_MED("MP: media deleted successfully from IContent DB.");
  else
    DBGPRINTF_ERROR("MP: ICONTENT_Delete() callback return error, %d",pDelCt->m_nErr);

  MPContentDeleteFree(pDelCt);
}
#endif //MP_FEATURE_ICONTENT
