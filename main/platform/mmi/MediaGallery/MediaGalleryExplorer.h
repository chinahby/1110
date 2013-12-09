#ifndef     MEDIAGALLERYEXPLORER_H
#define    MEDIAGALLERYEXPLORER_H
/**--------------------------------------------------------------------------
 * MediaGalleryExplorer.h -- define structure,enumerate,macro,and typedef for
 *                           browse files in file system.
 *
 * Copyright(c) 2008 Anylook
 *
 * History:
 *
 * when         who            what and why
 * -----------  -----------    -----------------------------
 * 2008.10.23       Create.
 *
 **--------------------------------------------------------------------------
 */
#include "MediaGallery.h"
#include "AEEMenu.h"
#include "AEEDB.h"
#include "MGMediaInfo.h"

#define MG_EXPLORER_ITEMBASE     8000
#define MG_EXPLORER_PARENTDIR    MG_EXPLORER_ITEMBASE
#define MG_EXPLORER_GENERALITEM  (MG_EXPLORER_ITEMBASE + 1)

#define MG_CURDEPTH_NONE      0
#define MG_CURDEPTH_INIT      1

#define MG_CURINDEX_NONE      0
#define MG_CURINDEX_INIT      1

/* image thumbnail size */
#define MG_GRID_THUMBDX       30
#define MG_GRID_THUMBDY       30

#define MG_STR_DOT            "."
#define MG_STR_DOUBLEDOT      ".."

#define FEATURE_MGFILEENUM_TREE

//
#define MG_DIRTYPE_SYSIMG      0x0001
#define MG_DIRTYPE_SYSSND      0x0002
#define MG_DIRTYPE_SYSVIDEO    0X0003
#define MG_DIRTYPE_SYSCALLREC  0X0004
#define MG_DIRTYPE_SYSSNDREC   0x0005
#define MG_DIRTYPE_NORMAL      0x0006 //General directory
#define MG_DIRTYPE_NORMALRECUR 0x0007

/*node type in file system*/
typedef enum _MGDocType{
   MG_DOCTYPE_DIR = 0, /*Item is a directory*/
   MG_DOCTYPE_FILE,    /*Item is a file */
   MG_DOCTYPE_MAX
}MGDocType;

/*mask to indicate which to enumerate*/
#define MG_ENUM_FILEMASK  0x01
#define MG_ENUM_DIRMASK   0x02
#define MG_ENUM_BOTHMASK  0x03

/*enumerate directory change type, */
typedef enum _MGDirChange{
   MG_DIR_GOUP, /*go one level up */
   MG_DIR_GODOWN,/* go one directory down*/
   MG_DIR_INVALID
}MGDirChange;

/*view type: list, or grid*/
typedef enum _MGView{
   MG_VIEW_LIST = 0,/*default type*/
   MG_VIEW_GRID,
   MG_VIEW_DEFAULT = 0,
}MGView;

/*sort order when build media menu list*/
typedef enum _MGSortOrder{
   MG_SORT_NONE = 0x0001,
   MG_SORT_NAME,
   MG_SORT_DATE,
   MG_SORT_SIZE,
   MG_SORT_TYPE,
   MG_SORT_MAX,
}MGSortOrder;

/*when add item to menu control, if specific pImage field in struct
 * CtlAddItem, menu control just add reference count, please refer to function
 * Menu_ResolveImage. So if we add 100 music files, it does not need to load
 * mp3 icon 100 times, just give the mp3 icon.*/
typedef struct _CFileIcons{
   IImage *pFolderIcon;
   IImage *pUnkownIcon;
   IImage *pImgIcon;
   IImage *p3GPIcon;
   IImage *pMp4Icon;
   IImage *pMIDIIcon;
   IImage *pMp3Icon;
   IImage *pVoiceRecIcon;
}CFileIcons;

typedef struct _MGFolderInfo{
    char   cAttrib;
    uint16 nFilesNum;
    uint16 nSubDirsNum;
    uint32 dwCreationDate;
    uint64 lDirSize;
}MGFolderInfo;

typedef struct _NodeName{
   char m_szNodeName[MG_MAX_FILE_NAME];
}NodeName;

/*struct for enumerate file & directory from file system*/
typedef struct _CFSExplorer{
   MGFileInfo     m_RootNode;
   MGFileInfo     m_CurFolder;
   MGFileInfo     *m_pCurNode;
   uint16       m_nPreItemID[MG_MAX_SUBDIR_LEVEL]; //Media menu previous select item id.
   uint8        m_nCurDepth;  // current directory depth level (from root)
   //uint8        m_nEnumMask;  // which need to enum: file, dir, or both
   uint32       m_nMaxSize;   //maximum size of file that requery to list.
   MGSortOrder  m_eSortOrder; //sort order type,  ascend , do not do descend now
   MGMimeType   m_eMime;      //current folder mime type, useful for some folders
   MGView       m_eView;      //type for viewer
   MGDocType    m_eType;      /*use for enumrate file & folder list, which now enmu, file or dir*/
}CFSExplorer;


/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_GetCurrentNode
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static __inline MGFileInfo *MGExplorer_GetCurrentNode(CFSExplorer *pExp)
{
   if(NULL == pExp)
      return NULL;

   return pExp->m_pCurNode;
}//MGExplorer_GetCurrentNode


/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_SetCurrentNode
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static __inline boolean MGExplorer_SetCurrentNode(CFSExplorer *pExp,
                                                  MGFileInfo  *pNode)
{
   if(NULL == pExp)
      return FALSE;

   pExp->m_pCurNode = pNode;
   return TRUE;
}//MGExplorer_SetCurrentNode


/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_GetCurrentNodeName
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static __inline char *MGExplorer_GetCurrentNodeName(CFSExplorer *pExp)
{
   if(NULL == pExp || NULL == pExp->m_pCurNode)
      return NULL;

   return pExp->m_pCurNode->szName;
}


/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_SetMime
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static __inline boolean MGExplorer_SetMime(CFSExplorer *pExp, uint16 nMime)
{
   if(NULL == pExp)
      return FALSE;

   pExp->m_eMime = nMime;
   return TRUE;
}//MGExplorer_SetMime


/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_GetMime
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static __inline uint16 MGExplorer_GetMime(CFSExplorer *pExp)
{
   if(NULL == pExp)
      return MG_MIME_UNKNOWN;

   return pExp->m_eMime;
}//MGExplorer_GetMime

/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_GetCurrentDepth
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static __inline uint8 MGExplorer_GetCurrentDepth(CFSExplorer *pExp)
{
   if(NULL == pExp)
      return MG_CURINDEX_INIT;

   return pExp->m_nCurDepth;
}//MGExplorer_GetCurrentDepth


/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_SetCurrentDepth
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static __inline boolean MGExplorer_SetCurrentDepth(CFSExplorer *pExp,
                                                   uint8 nDepth)
{
   if(NULL == pExp)
      return FALSE;

   pExp->m_nCurDepth = nDepth;
   return TRUE;
}//MGExplorer_SetCurrentDepth


/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_GetCurrentFolder
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static __inline MGFileInfo *MGExplorer_GetCurrentFolder(CFSExplorer *pExp)
{
   if(NULL == pExp)
      return NULL;

   return &pExp->m_CurFolder;
}//MGExplorer_GetCurrentFolder

/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_GetCurrentFolderPath
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static __inline char *MGExplorer_GetCurrentFolderPath(CFSExplorer *pExp)
{
   if(NULL == pExp)
      return NULL;

   return pExp->m_CurFolder.szName;
}//MGExplorer_GetCurrentFolderPath


/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_UpdateCurrentFolder
 * DESCRIPTION  :  更新当前目录
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static __inline boolean MGExplorer_UpdateCurrentFolder(CFSExplorer *pExp,
                                                       MGFileInfo *pCurNode,
                                                       size_t nSize)
{
   if(NULL == pExp || NULL == pCurNode)
      return FALSE;

   MEMCPY(&pExp->m_CurFolder, pCurNode, nSize);

   return TRUE;
}//MGExplorer_UpdateCurrentFolder


/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_Init
 * DESCRIPTION  :  初始化CFSExplorer结构体，为枚举文件作准备
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
boolean MGExplorer_Init(CFSExplorer *pFSExplorer,
                        IFileMgr *pFileMgr,
                        const char *cpszPath,
                        uint16 eMimeType);


/*===========================================================================
 * FUNCTION: MGExplorer_ResetPreItemID
 *
 * DESCRIPTION: set element in array m_nPreItemID of CFSExplorer to
 * MG_MENUITEM_NULL.
 *
 * PARAMETERS: bAll : TRUE , reset all element
 *                    FALSE, only reset current element.
 *
 * RETURN:
 * ==========================================================================
 */
boolean MGExplorer_ResetPreItemID(CFSExplorer *pFSExplorer, boolean bAll);


/*===========================================================================
 * FUNCTION:MGExplorer_FreeVectorElements
 *
 * DESCRIPTION:  Free vector interface, for we have memory space allocate
 * in heap attched to it, and need free them by ourself.
 *
 * PARAMETERS:
 *
 * RETURN:
 * ==========================================================================
 */
int MGExplorer_FreeVectorElements(IVector* pList);

/*===========================================================================
 * FUNCTION:MGExplorer_FreeMediaMenuItem
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURN:
 * ==========================================================================
 */
int MGExplorer_FreeMediaMenuItem(IMenuCtl *pMediaMenu);

/*===========================================================================
 * FUNCTION: MGExplorer_CheckFolderExist
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURN: TRUE  --- folder have already exist, or create successfully.
 *         FALSE --- folder not exist, or failed to create.
 * ==========================================================================
 */
boolean MGExplorer_CheckFolderExist(IFileMgr* pFileMgr,
                                    const char** ppFolders,
                                    uint32 nNum,
                                    boolean bMkdir);

/*===========================================================================
 * FUNCTION:MGExplorer_CheckMediaMenuEmpty
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURN:
 * ==========================================================================
 */
boolean MGExplorer_CheckMediaMenuEmpty(IMenuCtl *pMenuCtl);

/*===========================================================================
 * FUNCTION:MGExplorer_ResIsExist
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
boolean MGExplorer_ResIsExist(IFileMgr* pIFileMgr, const char* cpszFilePath);


/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_IsSysDirInMassStorage
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
boolean MGExplorer_IsSysDirInMassStorage(const char *pszFile,
                                          uint8 nStoreMedium);

/*===========================================================================
 * FUNCTION:MGExplorer_EnumFoldersList
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * RETURN:
 * ==========================================================================
 */
int MGExplorer_EnumFoldersList(IFileMgr* pIFileMgr,
                               const char* cpszFilePath,
                               IVector* pFolderList,
                               boolean bRecursive);


/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_BuildSubfolderMenu
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int MGExplorer_BuildSubfolderMenu(CFSExplorer *pFSExplorer,
                                  IFileMgr *pIFileMgr,
                                  IMenuCtl *pMenuCtl,
                                  CFileIcons *pIcons);
/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_GetMediaMenuSelectItem
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
void MGExplorer_GetMediaMenuSelectItem(CFSExplorer* pFSExplorer,
                                       IMenuCtl* pMenuCtl);

/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_SetMediaMenuSelItem
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
boolean MGExplorer_SetMediaMenuSelItem(CFSExplorer* pFSExplorer,
                                       IMenuCtl* pMenuCtl);

/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_GetSelItemIndex
 * DESCRIPTION  :  获取当前选中项的索引值
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
__inline boolean MGExplorer_GetSelItemIndex(IMenuCtl *pm, int *pnIdx);


/*===========================================================================
 * FUNCTION:MGExplorer_ChangeCurDir
 *
 * DESCRIPTION: 改变当前目录
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
int MGExplorer_ChangeCurDir(CFSExplorer* pFSExplorer,
                            MGDirChange eType,
                            const char *pszFilePath);

/*===========================================================================
 * FUNCTION:MGAppUtil_SpaceUnitConvert
 *
 * DESCRIPTION: 把存储空间的大小从字节转化为B/KB/MB/GB（根据大小）
 *
 * PARAMETERS:
 *
 * RETURN:
 * ==========================================================================
 */
boolean MGAppUtil_SpaceUnitConvert(IShell *pShell,
                                   uint32 MemByte,
                                   AECHAR *pszUnit,
                                   size_t nUnitLen);


boolean MGAppUtil_SpaceUnitConvertEx(IShell *pShell,
                                   uint64 MemByte,
                                   AECHAR *pszUnit,
                                   size_t nUnitLen);


/*===========================================================================
 * FUNCTION:MGAppUtil_FileAttribConvert
 *
 * DESCRIPTION: 把文件属性转化为字符串。
 *
 * PARAMETERS:
 *
 * RETURN:
 * ==========================================================================
 */
boolean MGAppUtil_FileAttribConvert(IShell *pShell,
                                    char cAttrib,
                                    AECHAR *pszAttrib,
                                    size_t nAttribLen);

/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_InitFileIcon
 * DESCRIPTION  :  初始化文件图标的各个域，即把各个域赋值为NULL。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
void MGExplorer_InitFileIcon(CFileIcons **ppFileIcons, boolean bAllocate);

/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_ReleaseFileIcon
 * DESCRIPTION  :  释放文件图标的IIMAGE接口。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
void MGExplorer_ReleaseFileIcon(CFileIcons *pFileIcons);

#endif//MEDIAGALLERYEXPLORER_H
