#ifndef IDiagMSM_H
#define IDiagMSM_H
/*============================================================================

FILE:  IDiagMSM.h

SERVICES: Qualcomm's MSM diagnostic service software access and buffering of
 data generated from that service.
 
GENERAL DESCRIPTION:
  Static extension providing access to and and buffering from software
  diagnostics in Qualcomm MSM chipsets.

REVISION HISTORY: 

============================================================================

               Copyright © 2003 QUALCOMM Incorporated 
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

============================================================================*/

#include "AEE.h"
#include "AEEError.h"
#include "IDiagMSM.bid"

#define IDIAGMSM_MODULE_VERSION           "1.1.31"

typedef struct _IDiagMSM IDiagMSM;

typedef uint32 IDiagMSMLHandleType;

#define IDIAGMSM_LHANDLE_NULL ((IDiagMSMLHandleType) 0xFFFFFFFF)

#define IDIAGMSM_VARLEN_ARRAY_DUMMY_SIZE (256)

#ifdef AEE_SIMULATOR
#pragma pack( push, 1 )
#endif
typedef PACKED struct
{
  uint32 qw[2];

} IDiagMSMQw;
#ifdef AEE_SIMULATOR
#pragma pack( pop )
#endif

typedef enum
{
  IDIAGMSM_ID_LOG = 0,
  IDIAGMSM_ID_EVENT,
  IDIAGMSM_ID_COUNT

} IDiagMSMRecordEnumType;

#ifdef AEE_SIMULATOR
#pragma pack( push, 1 )     // VC++ PACKED
#endif
typedef PACKED struct
{
  uint32 dwLogId;

  uint32 dwLength;
  byte aLog[IDIAGMSM_VARLEN_ARRAY_DUMMY_SIZE];

} IDiagMSMLogType;

typedef PACKED struct
{
  uint32 dwEventId;

  IDiagMSMQw qwTs;

  uint8 bLength;
  uint8 aPayload[IDIAGMSM_VARLEN_ARRAY_DUMMY_SIZE];

} IDiagMSMEventType;

typedef PACKED struct
{
  IDiagMSMLHandleType mHandle;
  
  uint32 dwSeqNum;

  uint8 cState;
  uint8 eType; //IDiagMSMRecordEnumType
  
  PACKED union
  {
    IDiagMSMLogType mLog;
    IDiagMSMEventType mEvent;
  
  } uRec;

} IDiagMSMRecordType;
#ifdef AEE_SIMULATOR
#pragma pack( pop )
#endif

typedef enum
{
  IDIAGMSM_OF_DROP = 0,
  IDIAGMSM_OF_CLOBBER,
  IDIAGMSM_OF_COUNT

} IDiagMSMOFlowOptType;

typedef struct
{
  int32 dwThreshold;
  AEECallback *pCb;
} IDiagMSMThresholdType;

typedef struct
{
  void *pUserData;
  
  uint32 dwSize;
  uint32 dwId;
  
  uint32 dwOFlowBufSize;

  uint8 cOflowStateThreshold;
  uint8 eType; //IDiagMSMRecordEnumType
  uint8 eOFlowOpt; //IDiagMSMOFlowOptType
  
  IDiagMSMThresholdType mListenerThresh;
  
  int32 dwLimit;
  
} IDiagMSMListenerType;

typedef boolean (*PFNIDIAGMSM_LSVC)
  (IDiagMSMRecordType *pRec, uint32 dwSize, void *pLUd, void *pUd);

typedef void (*PFNIDIAGMSM_LCB) 
  (IDiagMSMLHandleType mHandle, void *pLUd, void *pUd);

typedef struct
{
  uint32 dwLen;
  uint8  aPkt[IDIAGMSM_VARLEN_ARRAY_DUMMY_SIZE];

} IDiagMSMPktType;

typedef struct
{
  void *pUserData;
  AEECallback *pCb;
  IDiagMSMPktType mReq;

} IDiagMSMReqType;

typedef struct
{
  void *pUserData;
  IDiagMSMPktType mRsp;

} IDiagMSMRspType;

/*-------------------------------------------------------------------
File Interface definitions
-------------------------------------------------------------------*/
typedef int IDiagMSMFile;    

#define IDIAGMSM_FILE_ERROR      -1
#define IDIAGMSM_FS_SUCCESS       0
#define IDIAGMSM_FS_ERROR        -1

// These definitions are taken from fs_sys_types.h   
/* Type declarations. */
typedef int32  fs_ssize_t;
typedef uint32 fs_size_t;
typedef uint16 fs_dev_t;
typedef uint16 fs_mode_t;
typedef int32  fs_off_t;
typedef uint32 fs_time_t;
typedef uint32 fs_fsblkcnt_t;
typedef uint32 fs_fsfilcnt_t;
typedef uint32 fs_inode_t;
typedef uint8  fs_entry_type_t;
typedef uint8  fs_nlink_t;
typedef uint32 fs_devspecial_t;

/* Maximum length of the 'name' component of a file.  The name is the
 * longest string of characters delimited by beginning of string, the '/'
 * character or the end of the string.  This is the maximum length for a
 * filename or directory name.
 *
 * Currently, this cannot be increased above 128 without breaking the
 * B-tree code used to store the names. */
#define IDIAGMSM_FS_NAME_MAX 128

typedef struct _IDiagMSMFileStat {
  fs_dev_t   st_dev;      /* Device ID                  */
  fs_inode_t st_ino;      /* Inode number               */
  fs_mode_t  st_mode;     /* File mode                  */
  fs_nlink_t st_nlink;    /* Number of links.           */
  fs_size_t  st_size;     /* File size in bytes         */
  unsigned long st_blksize;
  unsigned long st_blocks;
  fs_time_t  st_atime;    /* Time of last access        */
  fs_time_t  st_mtime;    /* Time of last modification  */
  fs_time_t  st_ctime;    /* Time of last status change */
  fs_devspecial_t st_rdev; /* Major & Minor device number */
} IDiagMSMFileStat;

/* This is the strict definition.  Other fields may convey more
 * information, but should be looked up using stat. */
typedef struct _IDiagMSMDirEnt {
  char           d_name[IDIAGMSM_FS_NAME_MAX+1];
} IDiagMSMDirEnt;

struct IDiagMSMDirData;
typedef struct IDiagMSMDirData IDiagMSM_EFSDIR;

// These defines are taken from fs_fcntl.h
// Permission flag defines
#define IDIAGMSM_FILE_RDONLY             00
#define IDIAGMSM_FILE_WRONLY             01
#define IDIAGMSM_FILE_RDWR               02
#define IDIAGMSM_FILE_CREAT            0100
#define IDIAGMSM_FILE_EXCL             0200
#define IDIAGMSM_FILE_NOCTTY           0400
#define IDIAGMSM_FILE_TRUNC           01000
#define IDIAGMSM_FILE_APPEND          02000

// These defines are taken from fs.h
/* File SEEK operation origin */
typedef enum {
  IDIAGMSM_FILE_SEEK_SET,            /* Start of file position  */
  IDIAGMSM_FILE_SEEK_CURRENT,        /* Current file position   */
  IDIAGMSM_FILE_SEEK_EOF             /* End of file position    */
} DiagMSM_File_Seek_Origin_Type;

// These defines are taken from fs_sys_types.h
/* Posix permission bits.  Simple posix file permissions use a 3-level ACL,
 * with three privilege bits at each level.  The bits are read, write, and
 * execute.  Read and write have their obvious meaning for regular files.
 * Execute is currently not used by EFS2 (but could be checked).  For
 * directories, the meaning of the bits is a little different:
 *   - Read - Grants permission to open files in the directory.
 *   - Write - Grants permission to modify the "directory" (create, remove,
 *             or rename files)
 *   - Execute - Grants permission to search the directory (opendir).
 *
 * The three levels are "User", "Group", and "Other".  Currently, only
 * 'user' is defined in EFS2.  (Actually, none of these are checked).
 *
 * The values for these permissions are traditionally represented by octal
 * constants.  These defined values are a mixture of Posix and BSD
 * definitions.
 */

/* "User" permissions. */
#define IDIAGMSM_FILE_IRUSR           0400
#define IDIAGMSM_FILE_IWUSR           0200
#define IDIAGMSM_FILE_IXUSR           0100

/* "Group" permissions. */
#define IDIAGMSM_FILE_IRGRP           0040
#define IDIAGMSM_FILE_IWGRP           0020
#define IDIAGMSM_FILE_IXGRP           0010

/* "Other" permissions. */
#define IDIAGMSM_FILE_IROTH           0004
#define IDIAGMSM_FILE_IWOTH           0002
#define IDIAGMSM_FILE_IXOTH           0001

/* BSD definitions. */
#define IDIAGMSM_FILE_IREAD           IDIAGMSM_FILE_IRUSR
#define IDIAGMSM_FILE_IWRITE          IDIAGMSM_FILE_IWUSR
#define IDIAGMSM_FILE_IEXEC           IDIAGMSM_FILE_IXUSR

/* Common combinations of bits. */
#define IDIAGMSM_FILE_ACCESSPERMS     0777
#define IDIAGMSM_FILE_ALLPERMS        0777
#define IDIAGMSM_FILE_DEFFILEMODE     0666

// Event used by to communicate with the calling applications
#define EVT_IDIAGMSM                 ((EVT_USER) + (0x246))

// IDiagMSM privilege levels
// IMPORTANT: Update string table in OEMDiagMSM.c when a privilege level is 
// added to/removed from enum IDiagMSM_PrivLevels
typedef enum {
  DIAGMSM_PRIV_ALL,
  DIAGMSM_PRIV_EFS,
  DIAGMSM_PRIV_LOG,
  DIAGMSM_PRIV_LAST

} IDiagMSM_PrivLevels;

/* File Error codes */
/* These error codes will be returned by IDiagMSM_File_GetLastError() in case of 
   a failure. They can be used to determine errors more accurately */
#define IDIAGMSM_FILE_EPERM            1      /* Operation not permitted */
#define IDIAGMSM_FILE_ENOENT           2      /* No such file or directory */
#define IDIAGMSM_FILE_EEXIST           6      /* File exists */
#define IDIAGMSM_FILE_EBADF            9      /* Bad file descriptor */
#define IDIAGMSM_FILE_ENOMEM          12      /* Out of memory. */
#define IDIAGMSM_FILE_EACCES          13      /* Permission denied. */
#define IDIAGMSM_FILE_EBUSY           16      /* Device or resource is busy */
#define IDIAGMSM_FILE_EXDEV           18      /* Attempt to cross device. */
#define IDIAGMSM_FILE_ENODEV          19      /* No such device. */
#define IDIAGMSM_FILE_ENOTDIR         20      /* Not a directory */
#define IDIAGMSM_FILE_EISDIR          21      /* Is a directory */
#define IDIAGMSM_FILE_EINVAL          22      /* Invalid argument */
#define IDIAGMSM_FILE_EMFILE          24      /* Too many open files */
#define IDIAGMSM_FILE_ETXTBSY         26      /* File or dir already open */
#define IDIAGMSM_FILE_ENOSPC          28      /* No space left on device */
#define IDIAGMSM_FILE_ESPIPE          29      /* Seek is not permitted */
#define IDIAGMSM_FILE_FS_ERANGE       34      /* Parameter out of range. */
#define IDIAGMSM_FILE_ENAMETOOLONG    36      /* File name too long */
#define IDIAGMSM_FILE_ENOTEMPTY       39      /* Directory not empty */
#define IDIAGMSM_FILE_ELOOP           40      /* Too many symbolic links */
#define IDIAGMSM_FILE_EDQUOT         122      /* Attempt to write beyond quota*/

/* Non-posix errnos used within EFS. */
#define IDIAGMSM_FILE_ENOCARD        301      /* No Media present */
#define IDIAGMSM_FILE_EBADFMT        302      /* Bad Formated Media */
#define IDIAGMSM_FILE_ENOTITM        303      /* Not an EFS2 item file. */

/* IDiagMSM Errors */
#define IDIAGMSM_FILE_EFAILED        501      /* General Error */
#define IDIAGMSM_FILE_BADPARM        502      /* Bad Parameter */
#define IDIAGMSM_FILE_BADPATH        503      /* Bad Path, includes (.), (../)
                                                 or (..\) */

typedef void (*PFNIDIAGMSM_DLV_RSP) (IDiagMSMRspType *pRsp, void *pUserData);

#define IDIAGMSM_FILE_BADPATH_NV            "/nvm"
#define IDIAGMSM_FILE_BADPATH_NV_B          "\\nvm"
#define IDIAGMSM_FILE_BADPATH_SYSTEM        "/system"
#define IDIAGMSM_FILE_BADPATH_SYSTEM_B      "\\system"
#define IDIAGMSM_FILE_BADPATH_SYS           "/sys"
#define IDIAGMSM_FILE_BADPATH_SYS_B         "\\sys"
#define IDIAGMSM_FILE_BADPATH_DMU           "/dmu"
#define IDIAGMSM_FILE_BADPATH_DMU_B         "\\dmu"

#define IDIAGMSM_FILE_BADPATH_BREW_SYS      "/brew/sys"
#define IDIAGMSM_FILE_BADPATH_BREW_SYS_B    "\\brew\\sys"

/*-------------------------------------------------------------------
List of features supported by IDiagMSM module.
Future feature definitions will be added using the following pattern:
e.g.
List of current features supported by IDiagMSM module.  New features
can be added with value of last defined feature incremented by 1. 
e.g.
  #define IDIAGMSM_FEATURE_VER      2
  #define IDIAGMSM_FEATURE_NEW_1    3
  #define IDIAGMSM_FEATURE_NEW_2    4
  ...
-------------------------------------------------------------------*/
#define IDIAGMSM_FEATURE_SEC           0
#define IDIAGMSM_FEATURE_EFS           1
#define IDIAGMSM_FEATURE_VER           2

/*-------------------------------------------------------------------
List of error codes returned by IDiagMSM_GetVersion()
-------------------------------------------------------------------*/
#define IDIAGMSM_VER_EFAILED           601    /* General Error              */ 
#define IDIAGMSM_VER_EINVALIDLEN       602    /* Invalid buffer length      */
#define IDIAGMSM_VER_EINSBUFMEM        603    /* Insufficient buffer memory */

AEEINTERFACE(IDiagMSM)
{
  INHERIT_IQueryInterface (IDiagMSM);
  
  uint32 (*SendReq)(IDiagMSM *pDiagMSM, IDiagMSMReqType *pReq, void *pUserData);
  uint32 (*QueryRsp)(IDiagMSM *pDiagMSM, PFNIDIAGMSM_DLV_RSP pfnPktDeliver, 
                     void *pUd);
  
  uint32 (*InitListener)(IDiagMSM *pDiagMSM, IDiagMSMListenerType *pListener);
  IDiagMSMLHandleType (*AddListener)(IDiagMSM *pDiagMSM, IDiagMSMListenerType 
                                     *pListener);
  uint32 (*TraverseListeners)(IDiagMSM *pDiagMSM, PFNIDIAGMSM_LCB pfnDeliver, 
                              void *pUd);
  uint32 (*RemoveListener)(IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle);

  uint32 (*ServiceListener)(IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle, 
                            PFNIDIAGMSM_LSVC pfnSvc, void *pUd);
  uint32 (*ServiceOFlowBuf)(IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle, 
                            PFNIDIAGMSM_LSVC pfnSvc, void *pUd);
  IDiagMSMRecordType * (*RecAlloc)(IDiagMSM *pDiagMSM, IDiagMSMLHandleType 
                                   mHandle, uint32 dwSize);
  uint32 (*RecCommit)(IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle, 
                      IDiagMSMRecordType *pRec);
  uint32 (*RecShorten)(IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle, 
                       IDiagMSMRecordType *pRec, uint32 dwSize);
  
  uint32 (*GetListenerInfo)(IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle, 
                            IDiagMSMListenerType *pDestLCopy);
  int32 (*BufUsage)(IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle);
  
  uint32 (*GetOFlowOpt)(IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle, 
                        uint8 *pOpt);
  uint32 (*GetLThreshold)(IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle, 
                          int32 *pNotifyThresh);
  uint32 (*GetLimit)(IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle, 
                     int32 *pLimit);
  uint32 (*GetSize)(IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle, 
                    uint32 *pSize);
  uint32 (*SetOFlowOpt)(IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle, 
                        uint8 eOpt);
  uint32 (*SetLThreshold)(IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle, 
                          int32 dwNotifyThresh);
  uint32 (*SetLimit)(IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle, 
                     int32 dwLimit);
  uint32 (*CreateHMAC) (IDiagMSM *pMe, byte *pMsg, uint16 nMsgLen,
                        byte *pKey, uint16 nKeyLen, byte *pDigest, 
                        int *nDigestSz);
  boolean (*Verify)(IDiagMSM *pMe, const void *pCert, int nCertSz, 
                    AEECallback *pcb, uint32 *poutRes, uint16 wRand, 
                    uint8 bPrivLevel);
  uint16  (*Unlock)(IDiagMSM *pMe, uint32 dwRand);
  IDiagMSMFile (*File_Open)(IDiagMSM *pDiagMSM, 
                            const char * pPath, 
                            int oFlag, uint16 nMode);
  uint32 (*File_Close)(IDiagMSM *pDiagMSM, IDiagMSMFile handle);
  int32 (*File_Read)(IDiagMSM *pDiagMSM, 
                     IDiagMSMFile handle, 
                     void * pBuf, 
                     int32 nBufSz);
  int32 (*File_Seek)(IDiagMSM *pDiagMSM, 
                     IDiagMSMFile handle, int32 nOffset,
                     int nWhence); 
  uint32 (*File_Truncate)(IDiagMSM *pDiagMSM, 
                          const char * pPath, 
                          int32 nOffset);
  int32 (*File_Write)(IDiagMSM *pDiagMSM, 
                      IDiagMSMFile handle, 
                      const void * pBuf,
                      int32 nBufSz);
  uint32 (*File_MkDir)(IDiagMSM *pDiagMSM, 
                       const char * pPath, 
                       uint16 nMode);
  uint32 (*File_Rename)(IDiagMSM *pDiagMSM, 
                        const char * pOldPath, 
                        const char * pNewPath);
  uint32 (*File_RmDir)(IDiagMSM *pDiagMSM, const char * pPath);
  uint32 (*File_Stat)(IDiagMSM *pDiagMSM, 
                      const char *pPath, 
                      IDiagMSMFileStat * pBuf);
  uint32 (*File_Remove)(IDiagMSM *pDiagMSM, const char *pPath);
  IDiagMSM_EFSDIR * (*File_OpenDir)(IDiagMSM *pDiagMSM, 
                                    const char *pPath);
  IDiagMSMDirEnt * (*File_ReadDir)(IDiagMSM *pDiagMSM, 
                                   const IDiagMSM_EFSDIR *pDir);
  uint32 (*File_CloseDir)(IDiagMSM *pDiagMSM, 
                          const IDiagMSM_EFSDIR *pDir);
  int (*File_GetLastError)(IDiagMSM *pDiagMSM);
  uint32 (*GetVersion)(IDiagMSM *pDiagMSM, 
                       AECHAR *pBuf, uint32 dwBufLen);
  const byte * (*GetFeatures)(IDiagMSM *pDiagMSM, uint32 *pBufLen);
};

#define IDiagMSM_AddRef(p)                   AEEGETPVTBL((p),IDiagMSM)->AddRef((p))     
#define IDiagMSM_Release(p)                  AEEGETPVTBL((p),IDiagMSM)->Release((p))    
#define IDiagMSM_SendReq(p,pr,pud)           AEEGETPVTBL((p),IDiagMSM)->SendReq((p),(pr),(pud))
#define IDiagMSM_QueryInterface(p,clsid,pp)  AEEGETPVTBL((p),IDiagMSM)->QueryInterface((p),(clsid),(pp))
#define IDiagMSM_QueryRsp(p,pfn,pud)         AEEGETPVTBL((p),IDiagMSM)->QueryRsp((p),(pfn),(pud))
#define IDiagMSM_InitListener(p,pl)          AEEGETPVTBL((p),IDiagMSM)->InitListener((p),(pl))
#define IDiagMSM_AddListener(p,pl)           AEEGETPVTBL((p),IDiagMSM)->AddListener((p),(pl))
#define IDiagMSM_TraverseListeners(p,pfn,pu) AEEGETPVTBL((p),IDiagMSM)->TraverseListeners((p),(pfn),(pu))
#define IDiagMSM_RemoveListener(p,h)         AEEGETPVTBL((p),IDiagMSM)->RemoveListener((p),(h))
#define IDiagMSM_ServiceListener(p,h,pfn,pu) AEEGETPVTBL((p),IDiagMSM)->ServiceListener((p),(h),(pfn),(pu))
#define IDiagMSM_ServiceOFlowBuf(p,h,pfn,pu) AEEGETPVTBL((p),IDiagMSM)->ServiceOFlowBuf((p),(h),(pfn),(pu))
#define IDiagMSM_RecAlloc(p,h,s)             AEEGETPVTBL((p),IDiagMSM)->RecAlloc((p),(h),(s))
#define IDiagMSM_RecCommit(p,h,pr)           AEEGETPVTBL((p),IDiagMSM)->RecCommit((p),(h),(pr))
#define IDiagMSM_RecShorten(p,h,pr,l)        AEEGETPVTBL((p),IDiagMSM)->RecShorten((p),(h),(pr),(l))
#define IDiagMSM_GetListenerInfo(p,h,plc)    AEEGETPVTBL((p),IDiagMSM)->GetListenerInfo((p),(h),(plc))
#define IDiagMSM_BufUsage(p,h)               AEEGETPVTBL((p),IDiagMSM)->BufUsage((p),(h))
#define IDiagMSM_GetOFlowOpt(p,h,po)         AEEGETPVTBL((p),IDiagMSM)->GetOFlowOpt((p),(h),(po))
#define IDiagMSM_GetLThreshold(p,h,pt)       AEEGETPVTBL((p),IDiagMSM)->GetLThreshold((p),(h),(pt))
#define IDiagMSM_GetLimit(p,h,pl)            AEEGETPVTBL((p),IDiagMSM)->GetLimit((p),(h),(pl))
#define IDiagMSM_GetSize(p,h,ps)             AEEGETPVTBL((p),IDiagMSM)->GetSize((p),(h),(ps))
#define IDiagMSM_SetOFlowOpt(p,h,o)          AEEGETPVTBL((p),IDiagMSM)->SetOFlowOpt((p),(h),(o))
#define IDiagMSM_SetLThreshold(p,h,t)        AEEGETPVTBL((p),IDiagMSM)->SetLThreshold((p),(h),(t))
#define IDiagMSM_SetLimit(p,h,l)             AEEGETPVTBL((p),IDiagMSM)->SetLimit((p),(h),(l))
#define IDiagMSM_CreateHMAC(p,pm,ml,pk,kl,d,ds) AEEGETPVTBL((p),IDiagMSM)->CreateHMAC((p),(pm),(ml),(pk),(kl),(d),(ds))
#define IDiagMSM_Verify(p,pc,nc,pcb,pr,w,b)  AEEGETPVTBL((p),IDiagMSM)->Verify((p),(pc),(nc),(pcb),(pr),(w),(b))
#define IDiagMSM_Unlock(p,dw)                AEEGETPVTBL((p),IDiagMSM)->Unlock((p),(dw))
#define IDiagMSM_File_Open(p,c,f,m)          AEEGETPVTBL((p),IDiagMSM)->File_Open((p),(c),(f),(m))
#define IDiagMSM_File_Close(p, h)            AEEGETPVTBL((p),IDiagMSM)->File_Close((p),(h))
#define IDiagMSM_File_Read(p, h, b, s)       AEEGETPVTBL((p),IDiagMSM)->File_Read((p),(h),(b),(s))
#define IDiagMSM_File_Seek(p, h, o, w)       AEEGETPVTBL((p),IDiagMSM)->File_Seek((p),(h),(o),(w))
#define IDiagMSM_File_Truncate(p, h, o)      AEEGETPVTBL((p),IDiagMSM)->File_Truncate((p),(h),(o))
#define IDiagMSM_File_Write(p, h, b, s)      AEEGETPVTBL((p),IDiagMSM)->File_Write((p),(h),(b),(s))
#define IDiagMSM_File_MkDir(p, pp, m)        AEEGETPVTBL((p),IDiagMSM)->File_MkDir((p),(pp),(m))
#define IDiagMSM_File_Rename(p, pp, np)      AEEGETPVTBL((p),IDiagMSM)->File_Rename((p),(pp),(np))
#define IDiagMSM_File_RmDir(p, pp)           AEEGETPVTBL((p),IDiagMSM)->File_RmDir((p),(pp))
#define IDiagMSM_File_Stat(p, pp, b)         AEEGETPVTBL((p),IDiagMSM)->File_Stat((p),(pp),(b))
#define IDiagMSM_File_Remove(p, pp)          AEEGETPVTBL((p),IDiagMSM)->File_Remove((p),(pp))
#define IDiagMSM_File_OpenDir(p,pp)          AEEGETPVTBL((p),IDiagMSM)->File_OpenDir((p),(pp))
#define IDiagMSM_File_ReadDir(p,pd)          AEEGETPVTBL((p),IDiagMSM)->File_ReadDir((p),(pd)) 
#define IDiagMSM_File_CloseDir(p, pd)        AEEGETPVTBL((p),IDiagMSM)->File_CloseDir((p),(pd)) 
#define IDiagMSM_File_GetLastError(p)        AEEGETPVTBL((p),IDiagMSM)->File_GetLastError((p))
#define IDiagMSM_GetVersion(p, pb, bl)       AEEGETPVTBL((p),IDiagMSM)->GetVersion((p),(pb),(bl))
#define IDiagMSM_GetFeatures(p, pd)          AEEGETPVTBL((p),IDiagMSM)->GetFeatures((p),(pd))
/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
======================================================================= 

IDiagMSMLHandleType

Description:
  A handle to a registered listener in IDiagMSM.

Definition:
  typedef uint32 IDiagMSMLHandleType;

Values:
  IDIAGMSM_LHANDLE_NULL - A null (unused) handle.
  != IDIAGMSM_LHANDLE_NULL - Valid handle.

Comments:
  This is not a pointer to IDiagMSMListenerType.  This handle should 
  not be dereferenced or modified by the caller.  If the listener 
  is removed by the caller, all operations on that listener produce
  unknown results as the handle is now invalid.

See Also:
  IDiagMSMRecordType
  IDiagMSMListenerType
  PFNIDIAGMSM_LCB
  IDiagMSM_AddListener()
  IDiagMSM_TraverseListeners()
  IDiagMSM_RemoveListener()
  IDiagMSM_ServiceListener()
  IDiagMSM_ServiceOFlowBuf()
  IDiagMSM_RecAlloc()
  IDiagMSM_RecCommit()
  IDiagMSM_RecShorten()
  IDiagMSM_GetListenerInfo()
  IDiagMSM_BufUsage()
  IDiagMSM_GetOFlowUsage()
  IDiagMSM_GetLThreshold()
  IDiagMSM_GetLimit()
  IDiagMSM_GetSize()
  IDiagMSM_SetOFlowOpt()
  IDiagMSM_SetLThreshold()
  IDiagMSM_SetLimit()

=======================================================================

IDIAGMSM_VARLEN_ARRAY_DUMMY_SIZE

Description:
  This constant is used as a dummy array size for variable length 
  arrays in this API to work around compiler quirks for debugging 
  purposes.  
  
  When declaring an array, some compilers will accept [0], such as
  the Microsoft VC++ 6.0 compiler.  Other compilers, such as the 
  ARM ADS 1.2 compiler,  require a non-zero array size.
  
  Using [1] works.  However, this results in debuggers assuming only 1
  element in the array, making it difficult to view the contents.
  
  This value is benign to the operation of this extension.  It is set 
  to a "useful" value for debugging purposes.

=======================================================================

IDiagMSMQw

Description:
  Structure used to represent 64-bit types.

Definition:
  typedef struct
  {
    uint32 qw[2];

  } IDiagMSMQw;

Members:
  qw : array of 2 uint32s.  [0] is the least significant member

Comments:
  This is used to work around the fact that BREW does not support native
  64-bit types.

See Also:
  None

=======================================================================

IDiagMSMListenerEnumType

Description:
  Enumerated type, used in conjunction with ID, to identify records via
  IDiagMSMListenerType.

Definition:
  typedef enum
  {
    IDIAGMSM_ID_LOG,
    IDIAGMSM_ID_EVENT,
    IDIAGMSM_ID_COUNT

  } IDiagMSMListenerEnumType;

Members:
  IDIAGMSM_ID_LOG :   The ID is directed toward a logging service.
  IDIAGMSM_ID_EVENT : The ID is directed toward an event reporting service.
  IDIAGMSM_ID_COUNT : Internal purposes only - not valid for API.

Comments:
  In terms of delivery mechanism, there is no difference between a 
  log and event type.  It merely specifies which DMSS diagnostic service
  to use.
   
See Also:
  IDiagMSMListenerType
  IDiagMSMRecordType

=======================================================================

IDiagMSM_PrivLevels

Description:
  Enumerated type, used in conjunction with RSA key to determine the privilege
  of a calling app while unlocking IDiagMSM

Definition:
  typedef enum {
    DIAGMSM_PRIV_ALL,
    DIAGMSM_PRIV_EFS,
    DIAGMSM_PRIV_LOG,
    DIAGMSM_PRIV_LAST
  
  } IDiagMSM_PrivLevels;

Members:
  DIAGMSM_PRIV_ALL :  Privilege to use all interfaces will be given
  DIAGMSM_PRIV_LOG :  Privilege to use only EFS access interfaces will be 
                      given (all of the IDiagMSM_File_...() interfaces)
  DIAGMSM_PRIV_LOG :  Privileges to use all of the logging interfaces will
                      be given (all of teh IDiagMSM_...() interfaces).

Comments:
   
See Also:
  IDiagMSM_Unlock()

=======================================================================

IDiagMSMLogType

Description:
  This structure outlines a generic, variable length format used to 
  deliver log records from the DMSS diagnostic service.

Definition:
  typedef struct
  {
    uint32 dwId;
    uint32 dwLength;
    byte aLog[IDIAGMSM_VARLEN_ARRAY_DUMMY_SIZE];

  } IDiagMSMLogType;


Members:
  dwId :     The identifier for this log record.
  dwLength : The length of the following log payload field.
  aLog :     The payload of the log record.

Comments:
  The format of a log is specific DMSS diagnostic service.
   
See Also:
  IDiagMSMRecordType

=======================================================================

IDiagMSMEventType

Description:
  This structure outlines a generic, variable length format used to 
  deliver events and their payload from the DMSS diagnostic service.

Definition:
  typedef struct
  {
    unsigned int dwEventId;
   
    IDiagMSMQw qwTs;
   
    uint8 bLength;
    uint8 aPayload[IDIAGMSM_VARLEN_ARRAY_DUMMY_SIZE];
   
  } IDiagMSMEventType;

Members:
  dwEventId : Event ID from the modem's event service.
  qwTs :      64-bit time stamp.
  bLength :   The length of the following event payload field.
  aPayload :  The payload of the event record.

Comments:
  The format of a event is specific to DMSS's event service format.
   
See Also:
  IDiagMSMRecordType

=======================================================================

IDiagMSMRecordType

Description:
  This structure outlines a generic, variable length format used to 
  deliver records and their payload from the DMSS diagnostic service.

Definition:
  typedef struct
  {
    IDiagMSMLHandleType mHandle;
    
    uint32 dwSeqNum;
   
    uint8 cState;
    uint8 eType;
   
    union
    {
      IDiagMSMLogType mLog;
      IDiagMSMEventType mEvent;
      
    } uRec;
   
  } IDiagMSMRecordType;

Members:
  mHandle :     Handle to the listener used to enable this record.
  dwSeqNum :    Sequence number for this record, to detect duplicates.
  cState :      State of the record.  Used for multi-state record buffering.
                The caller handles state processing and updating of this value.
  eType :       Record type (event, log, etc).
                Use IDiagMSMListenerEnumType.
  uRec :        The contents of the record being delivered.
  uRec.mLog :   If eType specifies a log type, this union member 
                describes the format of the record.
  uRec.mEvent : If eType specifies an event type, this union member 
                describes the format of the record.

Comments:
  None.

See Also:
  IDiagMSMEventType
  IDiagMSMLogType
  PFNIDIAGMSM_LSVC
  IDiagMSM_ServiceListener()
  IDiagMSM_ServiceOFlowBuf()

=======================================================================

IDiagMSMOFlowOptType

Description:
  This option determines the behavior of the listener when the inbound
  flow of records exceeds the specified buffer space or listener's limit.

Definition:
  typedef enum
  {
    IDIAGMSM_OF_DROP,
    IDIAGMSM_OF_CLOBBER,
    IDIAGMSM_OF_COUNT
   
  } IDiagMSMOFlowOptType;

Members:
  IDIAGMSM_OF_DROP :    Discard most recent inbound record.
  IDIAGMSM_OF_CLOBBER : Discard oldest inbound record(s) until space for
                        the most recent record is available.
 
Comments:
  The "clobber" option is ideal for collecting a history of records
  prior to a collection trigger.

See Also:
  IDiagMSMListenerType

=======================================================================

IDiagMSMThresholdType

Description:
  This structure is used to define a notification threshold associated
  with the buffer or a particular listener.

Definition:
  typedef struct
  {
    int32 dwThreshold;
    AEECallback *pCb;
  
  } IDiagMSMThresholdType;

Members:
  dwThreshold : Threshold value to trigger this threshold.  
                -1 -> disabled.
                0 -> notify immediately when a record arrives.
                >0 -> notify when size exceeds threshold.
  pCb :         Callback structure used to notify the caller when the
                threshold is triggered.
 
Comments:
  None.

See Also:
  IDiagMSMListenerType

======================================================================= 

IDiagMSMListenerType

Description:
  IDiagMSMListenerType represents a "listener" used to listen for 
  inbound records matching the specified criteria.

  The listener performs the bulk of the funtionality of this extension.
  
  Each listener allocates it's own memory, making operation independent of
  other listeners.

Definition:
  typedef struct
  {
    void *pUserData;
    
    uint32 dwSize;
    uint32 dwId;
  
    uint32 dwOFlowBufSize;

    uint8 cOflowStateThreshold;
    uint8 eType;
    uint8 eOFlowOpt;
    
    IDiagMSMThresholdType mListenerThresh;
    
    int32 dwLimit;
    
  } IDiagMSMListenerType;


Members:
  pUserData :       Callers object pointer that is delivered unmodified when
                    the listener is serviced.
  dwSize :          The size of the buffer to be allocated for this listener.
  dwId :            ID of the desired record (i.e. log code)
  dwOFlowBufSize :  Size of the overflow buffer, if any, for this listener.
  cOflowStateThreshold : 
                    The final state for this listener.  Once a record reaches
                    this state, it can be discarded to make room for a new
                    record in clobber mode.  If the first record in the queue
                    has not yet reached this state, inbound records will move
                    to the overflow queue.  The caller performs state 
                    processing and is responsible for incrementing the state
                    variable when doing so.
  eType :           Type of record indicated by dwID.
                    Use IDiagMSMRecordEnumType.
  eOFlowOpt :       Overflow option.  Use IDiagMSMOFlowOptType.
  mListenerThresh : Notification threshold for the listener's queue.
  dwLimit :         Maximum buffer usage for this listener.  When exceeded, 
                    overflow occurs.
                    '-1' Indicates no limit; overflow occurs only when 
                    extension's buffer is exhaustsed.
                    '0' forces all inbound record to overflow, effectively
                    disabling the listener.
 
Comments:
  Many of these settings may be altered by accessor functions provided 
  in this class.
   
See Also:
  IDiagMSM_AddListener()
  IDiagMSM_InitListener()
  IDiagMSM_GetListenerInfo()

=======================================================================

PFNIDIAGMSM_LSVC

Description:
  This function type is used to deliver a record from the ready 
  queue of a listener via a call to IDiagMSM_ServiceListener().
  
  Upon return from this routine, the record is discarded.
  
Definition:
  typedef void (*PFNIDIAGMSM_LSVC)
    (IDiagMSMRecordType *pRec, uint32 dwSize, void *pLUd, void *pUd);

Arguments:
  pRec :   Pointer to the record from the ready Q.
  dwSize : Size of pRec structure, in bytes.
  pLUd :   Listener user data provided by caller of IDiagMSM_AddListener().
  pUd :    Users object data pointer provided by the caller of
           IDiagMSM_ServiceListener().

Return Value:
  None.

Comments:
  None.

See Also:
  IDiagMSM_ServiceListener()
  IDiagMSM_ServiceOFlowBuf()

=======================================================================

PFNIDIAGMSM_LCB

Description:
  This function type is used to deliver a handle of each listener 
  when the caller traverses the list of registered listeners.

Definition:
  typedef void (*PFNIDIAGMSM_LCB) 
    (IDiagMSMLHandleType mHandle, void *pLUd, void *pUd);

Arguments:
  mHandle : Valid listener handle.
  pLUd :    User's object data pointer specified when the listener 
            was added.
  pUd :     User's object data provided by the caller of 
            IDiagMSM_TraverseListeners() 

Comments:
  This can be used to find and remove listeners.

See Also:
  IDiagMSM_TraverseListeners()

=======================================================================

IDiagMSMPktType

Description:
  This structure is used to denote a packet (command/response) sent to
  or from the underlying modem software's diagnostic service.

Definition:
  typedef struct
  {
     uint32 dwLen;
     uint8  aPkt[IDIAGMSM_VARLEN_ARRAY_DUMMY_SIZE];
  
  } IDiagMSMPktType;

Members:
  dwLen : Length, in bytes, of aPkt.
  aPkt :  The entire packet.  Format is dependent upon underlying
          modem software's format.
 
Comments:
  No encoding is expected.  For example, Qualcomm's DMSS uses 
  HDLC-like encoding over a physical cable connection.  No such
  encoding is used via this class.

See Also:
  IDiagMSMReqType
  IDiagMSMRspType

=======================================================================

IDiagMSMReqType

Description:
  This structure is used to send a request packet to the underlying 
  modem software's diagnostic service.  

Definition:
  typedef struct
  {
    void *pUd;
    AEECallback *pCb;
    IDiagMSMPktType mReq;
  
  } IDiagMSMReqType;

Members:
  pUd :  User's object data pointer to be delivered unmodified with
         the corresponding response packet.
  pCb :  Callback structure used to notify the caller when the 
         corresponging response packet is ready.
  mReq : Request packet.
 
Comments:
  Request/response packets are asynchronous since a context switch is 
  necessary for many, if not all, modem diagnostic services.

See Also:
  IDiagMSMRspType
  IDiagMSM_SendReq()  

=======================================================================

IDiagMSMRspType

Description:
  This structure is used to deliver a response packet from the 
  underlying modem software's diagnostic service.

Definition:
  typedef struct
  {
    void *pUd;
    IDiagMSMPktType mRsp;
  
  } IDiagMSMRspType;

Members:
  pUd :  User's object data pointer specified in the corresponging 
         request packet.
  mRsp : Response packet.
 
Comments:
  Request/response packets are asynchronous since a context switch is 
  necessary for many, if not all, modem diagnostic services.

See Also:
  IDiagMSMReqType
  IDiagMSM_QueryRsp()  

=======================================================================

PFNIDIAGMSM_DLV_RSP

Description:
  This function type is used to deliver a response packet from the 
  response queue.  Order relative to request packets is not guaranteed,
  though is likely.

Definition:
  typedef void (*PFNIDIAGMSM_DLV_RSP) (IDiagMSMRspType *pRsp, void *pUd);

Arguments:
  pRsp : Response packet.
  pUd :  User's object data provided by the caller of IDiagMSM_QueryRsp()

Comments:
  None.

See Also:
  IDiagMSMRspType
  IDiagMSM_QueryRsp()

=======================================================================

IDiagMSMFileStat

Description:
  This structure is used to deliver information about the file. 

Definition:
  typedef struct _IDiagMSMFileStat {
    fs_dev_t   st_dev;      
    fs_inode_t st_ino;      
    fs_mode_t  st_mode;     
    fs_nlink_t st_nlink;    
    fs_size_t  st_size;     
    unsigned long st_blksize;
    unsigned long st_blocks;
    fs_time_t  st_atime;    
    fs_time_t  st_mtime;    
    fs_time_t  st_ctime;    
    fs_devspecial_t st_rdev;
  } IDiagMSMFileStat;

Members:
  st_dev :     [uint16] Device ID
  st_ino :     [uint32] Inode number
  st_mode :    [uint16] File Mode
  st_nlink :   [uint8] Number of links
  st_size :    [uint32] File size in bytes
  st_blksize : [uint32] Block size
  st_blocks :  [uint32] Number of blocks
  st_atime :   [uint32] Time of last access
  st_mtime :   [uint32] Time of last modification
  st_ctime :   [uint32] Time of last status change
  st_rdev :    [uint32] Major & Minor device numbers

Comments:
  None.

See Also:
  IDiagMSM_File_Stat()

=======================================================================

======================================================================= 
INTERFACES   DOCUMENTATION
======================================================================= 

Interface Name: IDiagMSM

Description: 
  This module is the MSM software diagnostics access extension.
  IDiagMSM is used to access diagnostics information from and send 
  commands to DMSS diagnostic services.


This extension allows the user to:
  1.  Send commands to the underlying diagnostic service
  2.  Configure the extension to listen for specific DMSS-generated data
  3.  Perforom state changes on the records queued in a listener.
  4.  Set and alter buffering options
  5.  Set and alter notification options using thresholds

To use an IDiagMSM extension:
  1.  Call ISHELL_CreateInstance() to create an instance of an IDiagMSM,
      specifying the ClassID of this extension.
  2.  Send commands or add various listeners using IDiagMSM_SendReq() or
      IDiagMSM_AddListener(), as needed for your needs.
  3.  When you no longer need the DIAGMSM instance, call 
      IDiagMSM_Release() to release it.
 
==============================================================================

Function: IDiagMSM_AddRef()

Description:
  This function increments the reference count for the IDiagMSM Interface 
  object.

Prototype:
  uint32 IDiagMSM_AddRef (IDiagMSM * pDiagMSM)

Parameters:
  pDiagMSM : Pointer to IDiagMSM Interface object

Return Value:
  Updated reference count

Comments: 
  None

Side Effects: 
  None

See Also: 
  IDiagMSM_Release()
    
==============================================================================

Function: IDiagMSM_Release()

Description:
  This function decerements the reference count for the IDiagMSM 
  Interface object and, if ref count becomes 0, releases resource in use
  by this object.

Prototype:
  uint32 IDiagMSM_Release (IDiagMSM * pDiagMSM)

Parameters:
  pDiagMSM : Pointer to IDiagMSM Interface object

Return Value:
  Updated reference count

Comments: 
  None

Side Effects: 
  None

See Also: 
  IDiagMSM_AddRef()
    
==============================================================================

Function: IDiagMSM_SendReq()

Description:
  This function send a request packet to the DMSS diagnostic service.
   
Prototype:
  uint32 IDiagMSM_SendReq (IDiagMSM *pDiagMSM, IDiagMSMReqType *pReq,void *pUd);
   
Parameters:
  pDiagMSM : Pointer to IDiagMSM Interface object
  pReq :     Request packet.
  pUd :      User's object data pointer to be passed unmodified with 
             the response packet.

Return Value:
  AEEError codes

Comments: 
  Request/response packets are asynchrnonous since DMSS diagnostic 
  services require a context switch to execute a command. 

Side Effects: 
  None

See Also: 
  IDiagMSMReqType
  IDiagMSM_QueryRsp()
    
==============================================================================

Function: IDiagMSM_QueryRsp()

Description:
  This function delivers a diagnostic service response packet from the
  response queue.
   
Prototype:
  uint32 IDiagMSM_QueryRsp (IDiagMSM *pDiagMSM, PFNIDIAGMSM_DLV_RSP 
                            pfnPktDeliver, void *pUd);
   
Parameters:
  pDiagMSM :      Pointer to IDiagMSM Interface object
  pfnPktDeliver : Function pointer to which a single response (if queued) 
                  is delivered.
  pUd :           User's object data pointer specified in the corresponging
                  call to IDiagMSM_SendReq().

Return Value:
  AEEError codes

Comments: 
  Request/response packets are asynchrnonous since DMSS diagnostic 
  services require a context switch to execute a command. 

Side Effects: 
  None

See Also: 
  PFNIDIAGMSM_DLV_RSP
  IDiagMSM_SendReq()
    
==============================================================================

Function: IDiagMSM_InitListener()

Description:
  This function initializes the listener config structure.  Using this 
  routine will ensure that unset fields in the listener structure are 
  set to benign values.

Prototype:
  uint32 (*InitListener)(IDiagMSM *pDiagMSM, IDiagMSMListenerType *pListener);

Parameters:
  pDiagMSM :  Pointer to IDiagMSM Interface object
  pListener : Listener config structure pointer.

Return Value:
  AEEError codes

Comments: 
  This is intended to initialize the listener config structure in case 
  fields are added in future versions.  All fields are initialized to
  benign values, where possible.

Side Effects: 
  None

See Also: 
  IDiagMSMListenerType
    
==============================================================================

Function: IDiagMSM_AddListener()

Description:
  This function adds (registers) a listener to the listener list in this
  extension.

Prototype:
  IDiagMSMLHandleType IDiagMSM_AddListener (IDiagMSM *pDiagMSM, 
                                            IDiagMSMListenerType *pListener);

Parameters:
  pDiagMSM :  Pointer to IDiagMSM Interface object
  pListener : Listener config structure pointer.

Return Value:
  Listener handle

Comments: 
  It is recommended to user IDiagMSM_InitListener() prior to assigning values
  to pListener.
   
  Duplicate entries are supported.  Each are assigned unique handles.

Side Effects: 
  None

See Also: 
  IDiagMSMListenerType
  IDiagMSM_InitListener()
  IDiagMSM_TraverseListeners()
  IDiagMSM_ServiceListener()
  IDiagMSM_ServiceOFlowBuf()
  IDiagMSM_RemoveListener()
  IDiagMSM_GetListenerInfo()
    
==============================================================================

Function: IDiagMSM_TraverseListeners()

Description:
  This function traverses the list of registered listeners in the extension.
  The specified callback is called with a handle for each listener.
   
Prototype:
  uint32 IDiagMSM_TraverseListeners (IDiagMSM *pDiagMSM, 
                                     PFNIDIAGMSM_LCB pfnDeliver, void *pUd);

Parameters:
  pDiagMSM :   Pointer to IDiagMSM Interface object
  pfnDeliver : Function pointer to be called with each listener handle
  pUd :        User data to be passed unmodified to pfnRecHandler

Return Value:
  AEEError codes

Comments: 
  This is intended to eliminate the need for the user to maintain a list of 
  handles.  It can also be used to "reset" the extension by making the
  callback remove each listener.

Side Effects: 
  None

See Also: 
  PFNIDIAGMSM_LCB
    
==============================================================================

Function: IDiagMSM_RemoveListener()

Description:
  This function removes (unregisters) the specified listener from the
  listener list, disabling collection of specified data.
   
Prototype:
  uint32 IDiagMSM_RemoveListener (IDiagMSM *pDiagMSM, 
                                  IDiagMSMLHandleType mHandle);

Parameters:
  pDiagMSM : Pointer to IDiagMSM Interface object
  mHandle :  Handle to a valid listener

Return Value:
  AEEError codes

Comments: 
  All data stored in the listener's queue will be discarded.  All 
  notification callbacks are cancelled.
  
  It is safe to do this while traversing the listener list.

Side Effects: 
  None

See Also: 
  IDiagMSM_AddListener()
  IDiagMSM_TraverseListeners()
    
==============================================================================

Function: IDiagMSM_ServiceListener()

Description:
  This function services the given listener by delivering 1 item from the
  front of the ready queue to the specified function pointer.  The return
  value indicates the existance and delivery of a record in the queue.
  
  Upon completion, this routine discard the record.
   
Prototype:
  uint32 IDiagMSM_ServiceListener (IDiagMSM *pDiagMSM, 
    IDiagMSMLHandleType mHandle, PFNIDIAGMSM_LSVC pfnSvc, void *pUd);

Parameters:
  pDiagMSM : Pointer to IDiagMSM Interface object
  mHandle :  Handle to a valid listener
  pfnSvc :   Pointer to function to which the record is sent
  pUd :      Pointer to be returned to caller via pfnDeliver

Return Value:
  AEEError codes
  
  AEE_SUCCESS:       A record was found in the queue and delivered to the 
                     function pointer.
  ERESOURCENOTFOUND: The listener queue was empty.
  EMEMPTR:           Given pointer(s) were invalid.

Comments: 
  If pfnSvc is NULL, the record is freed.
   
  The function pointer is used to avoid memory leaks.  Once pfnSvc
  returns, the record will be freed.

Side Effects: 
  None

See Also: 
  PFNIDIAGMSM_LSVC
    
==============================================================================

Function: IDiagMSM_RecAlloc()

Description:
  This function allocates a record buffer for the specified listener.  It is
  used to perform state processing by placing a post-state processing at the 
  end of the listener Q.  The caller is responsible for maintaining the state
  variable.  
  
  Allocating a record also places the record in the listener Q.  Each record 
  must be committed or shortened to '0' for the Q to remain operational.
  
  This function will not allocate unless called from PFNIDIAGMSM_LSVC.
     
Prototype:
  IDiagMSMRecordType * (*RecAlloc)(IDiagMSM *pDiagMSM, 
                                   IDiagMSMLHandleType mHandle, 
                                   uint32 dwSizedwSize);

Parameters:
  pDiagMSM : Pointer to IDiagMSM Interface object.
  mHandle :  Handle to listener into which the record is to be allocated.
  dwSize :   Size of destination record to be allocated.

Return Value:
  NULL:  Unsuccessful attempt to allocate.
  !NULL: Pointer to destination record.

Comments: 
  To avoid memory leaks, this may only be called from PFNIDIAGMSM_LSVC.

Side Effects: 
  None
    
==============================================================================

Function: IDiagMSM_RecCommit()

Description:
  This function commits a buffer that was allocated using IDiagMSM_RecAlloc().
  This function must be called for each allocated record, unless it is 
  discarded by calling IDiagMSM_RecShorten (0).
   
     
Prototype:
  uint32 (*RecCommit)(IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle, 
                      IDiagMSMRecordType *pRec);

Parameters:
  pDiagMSM : Pointer to IDiagMSM Interface object.
  mHandle :  Handle to listener into which the record is to be allocated.
  pRec :     Pointer to a previously allocated record.

Return Value:
  AEEError codes.

Side Effects: 
  None
    
==============================================================================

Function: IDiagMSM_RecShorten()

Description:
  This function shortens a previously allocated record, resulting in a hole at
  the end of the orignial buffer.
  
  If the record is shortened to a size of 0, the record is deleted.   
     
Prototype:
  uint32 (*RecShorten)(IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle, 
                       IDiagMSMRecordType *pRec, uint32 dwSize);

Parameters:
  pDiagMSM : Pointer to IDiagMSM Interface object.
  mHandle :  Handle to listener into which the record is to be allocated.
  pRec :     Pointer to a previously allocated record.
  dwSize :   New size of the record.  Must be smaller than the original size.

Return Value:
  AEEError codes.

Side Effects: 
  None
    
==============================================================================

Function: IDiagMSM_GetListenerInfo()

Description:
  This function retrieves listener config information associated with the
  specified handle.
   
Prototype:
  uint32 IDiagMSM_GetListenerInfo (IDiagMSM *pDiagMSM, 
                                   IDiagMSMLHandleType mHandle, 
                                   IDiagMSMListenerType *pDestLCopy);

Parameters:
  pDiagMSM :  Pointer to IDiagMSM Interface object
  mHandle :   Handle to a valid listener
  pDestLCopy: Destination listener structure into which the listener config 
              info will be copied.

Return Value:
  AEEError codes

Comments: 
  None.

Side Effects: 
  None

See Also: 
  IDiagMSMListenerType
    
==============================================================================

Function: IDiagMSM_BufUsage()

Description:
  This function retrieves the current buffer usage for a given listener, 
  including overhead.
   
Prototype:
  int32 IDiagMSM_BufUsage (IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle);

Parameters:
  pDiagMSM :  Pointer to IDiagMSM Interface object
  mHandle :   Handle to a valid listener

Return Value:
  Current usage value.  
  -1 if not valid.

Comments: 
  None.

Side Effects: 
  None

==============================================================================

Function: IDiagMSM_GetOFlowOpt()

Description:
  This function retrieves the current overflow option for the specified
  listener.
   
Prototype:
  uint32 IDiagMSM_GetOFlowOpt (IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle,
    uint8 *pOpt);

Parameters:
  pDiagMSM : Pointer to IDiagMSM Interface object
  mHandle :  Handle to a listener
  pOpt :     Pointer to which the overflow option value will be copied.
             Use IDiagMSMOFlowOptType

Return Value:
  AEEError codes

Comments: 
  None

Side Effects: 
  None

See Also: 
  IDiagMSMListenerType
   
==============================================================================

Function: IDiagMSM_GetLThreshold()

Description:
  This function retrieves the threshold for the specified listener.
   
Prototype:        
  uint32 IDiagMSM_GetLThreshold (IDiagMSM *pDiagMSM, 
    IDiagMSMLHandleType mHandle, int32 *pNotifyThresh);

Parameters:
  pDiagMSM :    Pointer to IDiagMSM Interface object
  mHandle :     Handle to a listener
  pThreshold :  Pointer to which the threshold value will be copied

Return Value:
  AEEError codes

Comments: 
  None

Side Effects: 
  None

See Also: 
  IDiagMSMListenerType
    
==============================================================================

Function: IDiagMSM_GetLimit()

Description:
  This function retrieves the usage limit for the specified listener.
   
Prototype:
  uint32 IDiagMSM_SetLimit (IDiagMSM *pDiagMSM, 
    IDiagMSMLHandleType mHandle, int32 pLimit);

Parameters:
  pDiagMSM : Pointer to IDiagMSM Interface object
  mHandle :  Handle to a listener
  pLimit :   Pointer to which the limit value will be copied

Return Value:
  AEEError codes

Comments: 
  None

Side Effects: 
  None

See Also: 
  IDiagMSMListenerType
    
==============================================================================

Function: IDiagMSM_GetSize()

Description:
  This function retrieves the size of the given listener buffer.
   
Prototype:
  uint32 IDiagMSM_GetSize (IDiagMSM *pDiagMSM, 
    IDiagMSMLHandleType mHandle, uint32 *pSize);

Parameters:
  pDiagMSM : Pointer to IDiagMSM Interface object
  mHandle :  Handle to a listener
  pSize :    Pointer to which the size value will be copied

Return Value:
  AEEError codes

Comments: 
  None

Side Effects: 
  None

See Also: 
  IDiagMSMListenerType
    
==============================================================================

Function: IDiagMSM_SetOFlowOpt()

Description:
  This function alters the overflow option associates with the specified
  listener.
   
Prototype:
  uint32 IDiagMSM_SetOFlowOpt (IDiagMSM *pDiagMSM, IDiagMSMLHandleType mHandle,
    uint8 eOpt);

Parameters:
  pDiagMSM : Pointer to IDiagMSM Interface object
  mHandle :  Handle to a listener
  eOpt :     New overflow option for this listener.
             Use IDiagMSMOFlowOptType.

Return Value:
  AEEError codes

Comments: 
  None

Side Effects: 
  None

See Also: 
  IDiagMSMListenerType
   
==============================================================================

Function: IDiagMSM_SetLThreshold()

Description:
  This function alters the related notification threshold for the specified\
  listener.
   
Prototype:
  uint32 IDiagMSM_SetLThreshold (IDiagMSM *pDiagMSM, 
    IDiagMSMLHandleType mHandle, int32 dwThreshold);

Parameters:
  pDiagMSM :    Pointer to IDiagMSM Interface object
  mHandle :     Handle to a listener
  dwThreshold : New realted notification threshold for this listener

Return Value:
  AEEError codes

Comments: 
  None

Side Effects: 
  None

See Also: 
  IDiagMSMListenerType
    
==============================================================================

Function: IDiagMSM_Unlock()

Description: 
  This function can be used by an application to unlock this instance of 
  IDiagMSM

Prototype:
  boolean IDiagMSM_Unlock(IDiagMSM *pMe, const byte *pMsg, int32 nMsgSz, 
                         byte *pCode,
                         int32 nCodeSz, AEECallback *pCallback, 
                         uint32 *poutRes, int nPadding, uint8 bPrivLevel)


Parameters:
  pBnmRm :    Pointer to IBnmRm Interface object
  pMsg :      Pointer to the clear text authentication message
  nMsgSz :    Size of clear text authentication message
  pCode :     Pointer to encoded authenticaiton message
  nCodeSz :   Size of the encoded authentication message
  pCallback : Pointer to callback that is going to be called upon completion
              of the Unlock process
  poutRes :   Pointer to the variable where the result of the Unlock process
              will be stored
  nPadding :  Type of padding that was used while encoding the authentication
              message
  bPrivLevel :Requested privilege level        

Return Value:
  AEEError codes

Comments:
  
Side Effects:
  Variable pointed to by poutRes will be modified  

See Also:
  None
    
==============================================================================
==============================================================================

Function: IDiagMSM_CreateHMAC()

Description:
  This function provides interface for creating SHA1 HMAC hashes.
   
Prototype:
  uint32 IDiagMSM_CreateHMAC (DiagMSM *pMe, byte *pMsg, uint16 nMsgLen,
                              byte *pKey, uint16 nKeyLen, byte *pDigest,
                              int *nDigestSz);

Parameters:
  pDiagMSM :  Pointer to IDiagMSM Interface object
  pMsg :      Pointer to message that needs to be hashed
  nMsgLen :   Lenght of the message to hash
  pKey :      Pointer to HMAC key
  nKeyLen :   Lenght of the HMAC key
  pDigest :   [out] Pointer to the buffer with created HMAC digest
  nDigestSz : [in/out] In - lenght of the buffer allocated for digest,
                       Out - Lenght of created digest

Return Value:
  Handle to the opened file

Comments: 
  None

Side Effects: 
  None

See Also: 
    
==============================================================================
==============================================================================

Function: DiagMSM_File_Open()

Description:
  This function provides interface for opening files in EFS
   
Prototype:
  uint32 IDiagMSM_File_Close (IDiagMSM *pDiagMSM, IDiagMSMFile handle);

Parameters:
  pDiagMSM :  Pointer to IDiagMSM Interface object
  handle :    Handle to the file to close
                                                                
Return Value:
  AEEError codes

Comments: 
  None

Side Effects: 
  None

See Also: 
    
==============================================================================
==============================================================================

Function: IDiagMSM_File_Read()

Description:
  This function provides interface reading files from EFS
   
Prototype:
  int32 IDiagMSM_File_Read(IDiagMSM *pDiagMSM, 
                           IDiagMSMFile handle, 
                           void * pBuf, 
                           uint32 nBufSz);

Parameters:
  pDiagMSM :  Pointer to IDiagMSM Interface object
  handle :    Handle to the file to read
  pBuf :      Pointer to the buffer in which read data will be recorded
  nBufSz :    Size of the buffer

Return Value:
  Number of successfully read bytes, -1 for failure
  0 is a valid result. 

Comments: 
  None

Side Effects: 
  None

See Also: 
    
==============================================================================
==============================================================================

Function: IDiagMSM_File_Seek()

Description:
  This function provides interface for seeking the offset from the file 
  descriptor
   
Prototype:
  int32 IDiagMSM_File_Seek(IDiagMSM *pDiagMSM, 
                           IDiagMSMFile handle, int32 nOffset,
                           int nWhence);

Parameters:
  pDiagMSM :  Pointer to IDiagMSM Interface object
  handle :    Handle to the file to read
  nOffset :   An offset for the opened file descriptor
  nWhence :   indicates how the new offset is computed:
                IDIAGMSM_FILE_SEEK_SET - Set to 'offset'.
                IDIAGMSM_FILE_SEEK_CURRENT - Set to 'offset' + current position.
                IDIAGMSM_FILE_SEEK_EOF - Set to 'offset' + file size.

Return Value:
  Resulting offset, -1 for failure

Comments: 
  None

Side Effects: 
  None

See Also: 
    
==============================================================================
==============================================================================

Function: IDiagMSM_File_Truncate()

Description:
  This function provides interface for truncating files. The file will be extended, or
  shrunk to the newly specified size.  On extension, the filesystem does
  not actually allocate space to the file, until data is written. When
  shortening a file, the data past the new end is lost.
   
Prototype:
  uint32 IDiagMSM_File_Truncate(IDiagMSM *pDiagMSM, 
                                const char * pPath, 
                                int32 nOffset);

Parameters:
  pDiagMSM :  Pointer to IDiagMSM Interface object
  pPath :     Path to a file to truncate
  nOffset :   A size to which the file will be truncated
  

Return Value:
  AEEError codes

Comments: 
  None

Side Effects: 
  None

See Also: 
    
==============================================================================
==============================================================================

Function: IDiagMSM_File_Write()

Description:
  This function provides interface for writing to the files in EFS.
   
Prototype:
  int32 IDiagMSM_File_Write(IDiagMSM *pDiagMSM, 
                            IDiagMSMFile handle, 
                            const void * pBuf,
                            uint32 nBufSz);

Parameters:
  pDiagMSM :  Pointer to IDiagMSM Interface object    
  handle :    Handle to the file to write
  pBuf :      Buffer with data to write
  nBufSz :    Size of the data
  

Return Value:
  Number of bytes successfully written, -1 for failure

Comments: 
  None

Side Effects: 
  None

See Also: 
    
==============================================================================
==============================================================================

Function: IDiagMSM_File_MkDir()

Description:
  This function provides interface for creating directories in EFS.
   
Prototype:
  uint32 IDiagMSM_File_MkDir(IDiagMSM *pDiagMSM, 
                             const char * pPath, 
                             uint16 nMode);

Parameters:
  pDiagMSM :  Pointer to IDiagMSM Interface object    
  pPath :     Path to a directory to create
  nMode :     Premission bits for a new directory. Please see "Postfix 
              permission bits" for more information
  

Return Value:
  AEEError codes

Comments: 
  None

Side Effects: 
  None

See Also: 
    
==============================================================================
==============================================================================

Function: IDiagMSM_File_Rename()

Description:
  This function provides interface for renaming the files. Old path and 
  new path do not have to be in the same directory. If rename fails, the
  original file will be left with the same name in its original location
   
Prototype:
  uint32 IDiagMSM_File_Rename(IDiagMSM *pDiagMSM, 
                              const char * pOldPath, 
                              const char * pNewPath);

Parameters:
  pDiagMSM :  Pointer to IDiagMSM Interface object    
  pOldPath :  Path to the file to rename
  pNewPath :  New path to the file
  

Return Value:
  AEEError codes

Comments: 
  None

Side Effects: 
  None

See Also: 
    
==============================================================================
==============================================================================

Function: IDiagMSM_File_RmDir()

Description:
  Removes a directory, only empty directories can be removed
   
Prototype:
  uint32 IDiagMSM_File_RmDir(IDiagMSM *pDiagMSM, const char * pPath);

Parameters:
  pDiagMSM :  Pointer to IDiagMSM Interface object    
  pPath :     Path to the direcotry to be removed

Return Value:
  AEEError codes

Comments: 
  None

Side Effects: 
  None

See Also: 
    
==============================================================================
==============================================================================

Function: IDiagMSM_File_Stat()

Description:
  Returns an information about the named file/directory
     
Prototype:
  uint32 IDiagMSM_File_Stat(IDiagMSM *pDiagMSM, 
                            const char *pPath, 
                            IDiagMSMFileStat * pBuf);

Parameters:
  pDiagMSM :  Pointer to IDiagMSM Interface object    
  pPath :     Path to the file/directory  to stat  
  pBuf :      Buffer which will be filled with information

Return Value:
  AEEError codes

Comments: 
  None

Side Effects: 
  None

See Also: 
    
==============================================================================
==============================================================================

Function: IDiagMSM_File_Remove()

Description:
  Removes a name from the system. If this name is the only link to the file
  the file itself will be removed as well. 
     
Prototype:
  uint32 IDiagMSM_File_Remove(IDiagMSM *pDiagMSM, const char *pPath);

Parameters:
  pDiagMSM :  Pointer to IDiagMSM Interface object    
  pPath :     Path to a file to remove  

Return Value:
  AEEError codes

Comments: 
  None

Side Effects: 
  None

See Also: 
    
==============================================================================
==============================================================================

Function: IDiagMSM_File_OpenDir()

Description:
  Open a directory for iteration.  The pointer returned is an opaque
  handle that can be used to iterate through the entries of the directory.
  The entries should be read with IDiagMSM_File_ReadDir,
  and IDiagMSM_File_CloseDir called at the end. 
     
Prototype:
  IDiagMSMDirEnt * IDiagMSM_File_OpenDir(IDiagMSM *pDiagMSM, const char *pPath);

Parameters:
  pDiagMSM :  Pointer to IDiagMSM Interface object    
  pPath :     Path to a directory to open 

Return Value:
  Pointer to opened directory, NULL for failure

Comments: 
  None

Side Effects: 
  None

See Also: 
    
==============================================================================
==============================================================================

Function: IDiagMSM_File_ReadDir()

Description:
  Read the next entry from the opened directory.  On success, returns a
  pointer to a IDiagMSMDirEnt, which contains a field d_name which is
  tne name of this directory component.   
     
Prototype:
  IDiagMSMDirEnt * IDiagMSM_File_ReadDir(IDiagMSM *pDiagMSM, 
                                         const IDiagMSM_EFSDIR *pDir)

Parameters:
  pDiagMSM :  Pointer to IDiagMSM Interface object    
  pDir :      Pointer to an opened directory

Return Value:
  Pointer to a structure that contains a name of the directory component, 
  NULL for failure

Comments: 
  None

Side Effects: 
  None

See Also: 
  
==============================================================================
==============================================================================

Function: IDiagMSM_File_CloseDir()

Description:
  Close a directory iterator   
     
Prototype:
  IDiagMSMDirEnt * IDiagMSM_File_CloseDir(IDiagMSM *pDiagMSM, 
                                          const IDiagMSM_EFSDIR *pDir)

Parameters:
  pDiagMSM :  Pointer to IDiagMSM Interface object    
  pDir :      Pointer to an opened directory

Return Value:
  AEEError codes
  
Comments: 
  None

Side Effects: 
  None

See Also: 
    
==============================================================================
==============================================================================

Function: IDiagMSM_File_GetLastError()

Description:
  Returns last file error. Returns success if no error happened during the last
  call. Refer to "File Error Codes" section for return values
     
Prototype:
  static int IDiagMSM_File_GetLastError(BnmRm *pMe)

Parameters:
  pDiagMSM :  Pointer to IDiagMSM Interface object    

Return Value:
  IDiagMSM File Error codes (defined in IDiagMSM.h)
  
Comments: 
  None

Side Effects: 
  None

See Also: 
   File Error codes 
==============================================================================

==============================================================================

Function: IDiagMSM_GetVersion()

Description: 
  This function can be used by an application to obtain the version of 
  IDiagMSM

Prototype:
  uint32 IDiagMSM_GetVersion (IDiagMSM *pDiagMSM, AECHAR *pBuf, uint32 dwBufLen)

Parameters:
  pDiagMSM :  Pointer to IDiagMSM Interface object
  pBuf :      Pointer to AECHAR buffer in which the version string will be 
              copied 
  dwBufLen :  Size of the buffer in bytes

Return Value:
  AEE_SUCCESS              :  Get version operation is successful
  IDIAGMSM_VER_EFAILED     :  General Error
  IDIAGMSM_VER_EINVALIDLEN :  Invalid buffer length
  IDIAGMSM_VER_EINSBUFMEM  :  Insufficient buffer memory

Comments:
  If dwBufLen is smaller than the length of the version string, the 
  version string will not be returned.

Side Effects:
  Memory pointed to by pBuf will be modified  

See Also:
  None

==============================================================================

==============================================================================

Function: IDiagMSM_GetFeatures()

Description: 
  This is an interface function intended to be used by applications to obtain
  features supported by IDiagMSM module.

Prototype:
  const byte * IDiagMSM_GetFeatures (IDiagMSM *pDiagMSM, uint32 *pBufLen)

Parameters:
  pIDiagMSM : Pointer to IDiagMSM Interface object
  pBufLen   : IDiagMSM returns size in bytes of feature bit mask buffer

Return Value:
  pointer to feature bit mask buffer of bytes. Each bit denotes a feature
  supported by IDiagMSM module.
  The pointer will be set to NULL if an error is encountered.

Comments:
  The feature bit mask buffer has the following structure:
  
    byte 0   byte 1  ....  byte n
  |76543210|76543210|....|76543210|

   11111111 11111111 .... 00000011  

  - A bit value of 1 specifies an IDiagMSM feature is supported.  A bit
    value of 0 denotes a feature not supported or not yet defined.    
  - IDiagMSM features will be mapped in the following order:
    - Bit 0 of byte 0 represents earliest feature supported.
    - Bit 1 of byte n represents latest feature supported.
  - The following algorithms specify how to look up supported features 
    (in c syntax) using the feature bit mask buffer returned by this API
    function:

        // Find the bit for IDIAGMSM_FEATURE_XX within the feature_bit_mask
        // array.
        array_index = IDIAGMSM_FEATURE_XX/8; 
        byte_mask  = 0x01 << (IDIAGMSM_FEATURE_XX%8);
        
        // Check if IDIAGMSM_FEATURE_XX is supported.
        if (feature_bit_mask[array_index] & byte_mask)
          feature IDIAGMSM_FEATURE_XX supported
        else
          feature IDIAGMSM_FEATURED_XX not supported

Side Effects:
  None.  

See Also:
  None.
==============================================================================
=======================================================================*/


#endif /* #ifndef IDiagMSM_H */



