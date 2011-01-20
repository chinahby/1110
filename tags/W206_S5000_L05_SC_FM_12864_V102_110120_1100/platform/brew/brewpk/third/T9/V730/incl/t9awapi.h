/*******************************************************************************
********************************************************************************

                    COPYRIGHT 2003-2005 TEGIC COMMUNICATIONS

                TEGIC COMMUNICATIONS PROPRIETARY INFORMATION

     This software is supplied under the terms of a license agreement
     or non-disclosure agreement with Tegic Communications and may not
     be copied or disclosed except in accordance with the terms of that
     agreement.

********************************************************************************

     File Name:   t9awapi.h

     Description: Header file for T9 alphabetic word module

     Version:	  7.3                                                       
********************************************************************************
********************************************************************************/

#ifndef _T9AWAPI_H
#define _T9AWAPI_H    1

#include "t9.h"

/* don't mangle the function name if compile under C++ */
#if defined(__cplusplus)
extern "C" {
#endif

/*----------------------------------------------------------------------------
 *  Define T9 core version number
 *
 *  String format is defined as "T9AW VMM.mm.bf"
 *  Where
 *      MM   = major version number
 *      mm   = minor version number
 *      bf   = bug fix version number
 *  Update T9AWCOREVER defn. if any component version # goes beyond 2 digits.
 *----------------------------------------------------------------------------*/
#define T9AWMAJORVER   T9MAJORVER
#define T9AWMINORVER   T9MINORVER
#define T9AWPATCHVER   T9PATCHVER
#define T9AWCOREVER   "T9AW V0" T9AWMAJORVER ".0" T9AWMINORVER ".0" T9AWPATCHVER "   "

#define T9AWMAXSELECTIONOBJECT        64  /* Max objects in the selection list */
#define T9MAXHASHENTRIES            32  /* Max # of duplicate-suppressed
                                           sel-list entries */
#ifndef T9AWMAXPUNCTKEYS
#define T9AWMAXPUNCTKEYS 1 /* How many keys can have a punctuation table? */
#endif
#if (T9AWMAXPUNCTKEYS < 1) || (T9AWMAXPUNCTKEYS > T9MAXDBKEYS)
#error Illegal value for T9MAXDBKEYS.
#endif

#define T9AW_NUMSUBCOUNTS                 9

#define T9AW_NUMSELLISTSUBCNTS            4

/*----------------------------------------------------------------------------
 *  Define maximum udb word size
 *  NOTE:  This should not be set to a value less than 32.  In addition, the
 *  T9 stack usage will increase by two times the increase of the max UDB word
 *  size.  For example, doubling the size of the maximum UDB
 *  word will quadruple the amount of stack used for some UDB functions.
 *----------------------------------------------------------------------------*/
#ifndef T9MAXUDBWORDSIZE
#define T9MAXUDBWORDSIZE    64  /* Max udb word length supported by T9 system */
#endif /* !T9MAXUDBWORDSIZE  */

#if (T9MAXUDBWORDSIZE > 64)
#error T9MAXUDBWORDSIZE cannot be bigger than 64.
#endif

/*----------------------------------------------------------------------------
 * Max MDB word length supported by T9 system.
 * Note: the maximum MDB word size does not necessarily equal to the maximum
 *       UDB word size.
 *----------------------------------------------------------------------------*/
#ifndef T9MAXMDBWORDSIZE
#define T9MAXMDBWORDSIZE	T9MAXUDBWORDSIZE
#endif /* !T9MAXMDBWORDSIZE  */

#if (T9MAXUDBWORDSIZE > T9_MAXLDBWORDSIZE)
#if (T9MAXUDBWORDSIZE > T9MAXMDBWORDSIZE)
        /* Max udb word length supported by T9 system */
#define T9AWMAXWORDSIZE   T9MAXUDBWORDSIZE
#else
        /* Max mdb word length supported by T9 system */
#define T9AWMAXWORDSIZE   T9MAXMDBWORDSIZE
#endif
#else
#if (T9_MAXLDBWORDSIZE > T9MAXMDBWORDSIZE)
        /* Max ldb word length supported by T9 system */
#define T9AWMAXWORDSIZE   T9_MAXLDBWORDSIZE
#else
        /* Max mdb word length supported by T9 system */
#define T9AWMAXWORDSIZE   T9MAXMDBWORDSIZE
#endif
#endif

#if (T9AWMAXWORDSIZE > 256) && (T9MINORWIDTH < 2)
#error -- error; T9AWMAXWORDSIZE must be less than 256 if T9MINORWIDTH is less than 2
#endif

/* max multitap sequence size is 32, build buffer size is at
least as big, since multitap uses the build buffer to store symbols */
#define T9AW_MAXMTSEQSIZE 32
#if (T9AWMAXWORDSIZE > T9AW_MAXMTSEQSIZE)
#define T9AW_BUILDBUFSIZE   T9AWMAXWORDSIZE
#else
#define T9AW_BUILDBUFSIZE   T9AW_MAXMTSEQSIZE
#endif

#ifdef T9CDB_IN_BUILD    
/* define max number of predictions in prediction list. */
#ifndef T9MAXCDBMATCHES
#define T9MAXCDBMATCHES 0x06
#endif

#endif

#define T9AWDEFAULTLANGFENCE ((T9U8)2)  /* Default sel list position of backup lang words */
#define T9AWDEFAULTUDBFENCE  ((T9U8)1)  /* Default display list UDB start point   */

#define T9TUDB_MEMMINSIZE 256
#define T9TUDB_RECMINSIZE 21

/*----------------------------------------------------------------------------
 *  Define T9 shift value and mask for the MDB state bit.
 *----------------------------------------------------------------------------*/
#define T9STATEMDBWORDCOMPLT        0
#define T9STATEMDBWORDCOMPLTMASK    ((T9U8)(1L << T9STATEMDBWORDCOMPLT))

/*----------------------------------------------------------------------------
 *  Alpha UDB description.
 *----------------------------------------------------------------------------*/
/* Minimum size requested for UDB in bytes */
#define T9MINUDBDATABYTES 1024

/* The number of keys with separate storage areas. */
#define T9UDBTOTALKEYSTORAGES   10

/* Make sure bDataBytes is aligned on an even boundary T9SYMB is two bytes. */
#if (T9MAXDBKEYS & 1) || (T9SYMBOLWIDTH <= 1)
#define T9AW_ALIGNUDBDATA 0
#else
#define T9AW_ALIGNUDBDATA 1
#endif

typedef struct T9UdbInfo_s {

    T9U16 wDataSize;        /* Total size in bytes of this struct (minimum is 1024) */
    T9U16 wDataCheck;       /* Additive checksum of database */
    T9U16 wUpdateCounter;   /* Count incremented each time user database modified */
    T9U16 wSymbolClass;     /* T9 enum value indicating symbol table mapping for this UDB */
    T9U16 wUdbWordCount;        /* Number of valid udb words currently in user database */
    T9U16 wRdbWordCount;        /* Number of valid rdb words currently in user database */
    T9U16 wRemainingMemory;     /* Free memory remaining in RUDB. */


    T9U16 wLastDelCutOffFreq;   /* Last deletion freq. cut-off by garbage collection */
    T9U16 wSavedOffset;         /* pointer to last accessed position in database */
    T9U32 dwOffsetSaver;        /* identifier for thread that last saved offset. */
    T9U16 wKeyOffset[T9UDBTOTALKEYSTORAGES];       
                                /* pointers to beginning of key storage area. */
    T9U8  pbFirstCharOnKey[T9MAXDBKEYS + 1 + T9AW_ALIGNUDBDATA];

    T9U8  bDataArea[1];         /* Really a variable size data array */
} T9AWUdbInfo;

#ifdef T9CDB_IN_BUILD
/*----------------------------------------------------------------------------
 *  Alpha CDB description.
 *----------------------------------------------------------------------------*/
/* Minimum size requested for CDB in bytes */
#define T9MINCDBDATABYTES 1600
typedef struct T9CdbInfo_s {

    T9U16 wDataSize;        /* Total size in bytes of this struct (minimum is 1024) */
    T9U16 wUpdateCounter;   /* Count incremented each time user database modified */
    T9U16 wSymbolClass;     /* T9 enum value indicating symbol table mapping for this UDB */
    T9U16 wDataEndOffset;      /* Offset to end of context data */
    T9U16 wSavedOffset;         /* pointer to last accessed position in database */
    T9U32 dwOffsetSaver;        /* identifier for thread that last saved offset. */
    T9SYMB  sDataArea[1];          /* Really a variable size data array */
} T9AWCdbInfo;
#endif
/*----------------------------------------------------------------------------
 *  Define T9 MDB request types
 *----------------------------------------------------------------------------*/
typedef enum T9REQMDB_e {
    T9AWMDBGETEXACTWORD = 1,   /* 1 */
    T9AWMDBGETSTEMWORD         /* 2 */
} T9REQMDB;

/*----------------------------------------------------------------------------
 *  Define T9 Manufacture Database (MDB) information structure.
 *---------------------------------------------------------------------------*/
struct T9AWFieldInfo_s;
typedef T9STATUS (T9FARCALL *T9AWMDBCALLBACK)(

    struct T9AWFieldInfo_s *pAWFieldInfo,  /* I   - T9 Field Info structure         */
    T9U8         bMdbRequestType,       /* I   - Mdb request type. Should be one */
                                        /*       of the values defined above.    */
    T9AuxType   *paAuxBuf,              /* I   - Pointer to T9 Aux buffer        */
    T9UINT        nWordLen,              /* I   - Word length                     */
    T9UINT        nMaxWordLen,           /* I   - Maximum word length             */
    T9SYMB      *psBuildTxtBuf,         /* O   - Word to return                  */
    T9UINT       *pnActWordLen,          /* O   - Length of the returned word     */
    T9U32       *pdwWordListIdx         /* I/O - MDB word list index             */
);

typedef struct T9MdbInfo_s {
    T9U8            bStateBits;     /* State bits for MDB                       */
    T9UINT           nMdbFence;      /* Selection list start point for MDB words */
    T9AWMDBCALLBACK   T9ReadMdbData;  /* Pointer to MDB callback function         */
} T9AWMdbInfo;

/*============================================================================
 * NOTE:
 *   The following definitions are private to AOL Mobile and are subject
 *   to change without notice.
 *============================================================================*/

/* Define structure for T9 Private persistent data */
struct T9AW_PrivateNotLdb_s {
    T9U32 dwObjectHashes[T9MAXHASHENTRIES + 1];  /* array of object hashes */
    T9U8 bOTFlyMtapIndex; /* which multitap char was the last one entered */
    T9U8 bHowBuilt;   /* What was the source of info for the last word? */
    T9U8 bObjectType;

    T9U8 bCompletionOn;        /* Is wd completion not temporarily disabled? */
    T9U8 bInflectPosition;     /* pos of punct or explicit (point for build-around or break) */
    T9U8 bFirstObjectNum;      /* the first object shown on entry on key */

    T9U8 bFirstCurSelObjNum;   /* duplicate suppressed first object number */
    T9U8 bTmpBuffer[T9_MAXLDBWORDSIZE];
    T9U8 bSavedKeyLength; 
    T9U8 bSavedLastMaxListLength;

    T9U8 bSavedThisMaxListLength;
    T9U8 bSavedKeysLeft;
    T9U8 bLockLangIndex;  /* Language of word to left of left-most lock point. */
    T9SYMB sOTFlyFirstMT;

    T9SYMB sOTFlyLastMT;
    T9SYMB sBuildTxtBuf[T9AW_BUILDBUFSIZE];
#ifdef T9CDB_IN_BUILD    
    T9SYMB sContextBuf[T9AWMAXWORDSIZE];
    T9U8  bContextWordSize;
#endif

    T9U32 dwSave_Pointers[T9_MAXLDBWORDSIZE];
    T9U32 dwMdbDataIdx;
};  /* end of struct T9AW_PrivateNotLdb_s */

struct T9AW_PrivateLdb_s {
    T9U8      bPreSmartPunct;
    T9U8      bSubCounts[T9AW_NUMSUBCOUNTS];  /* global holding subcounts of totobjcnt */

    T9U8 bSelListSubCnts[T9AW_NUMSELLISTSUBCNTS];  /* 7 elements, for
                         0.explicit FULLMATCH,
                         1.trailing punct from language,
                         2.leading punct from lang
                         3.embedded punct from lang
                         4.trailing explicit from lang,
                         5.leading explicit from lang
                         6.default explicit from lang */
};  /* end of struct T9AW_PrivateLdb_s */

struct T9AWPunctInfo_s {
    T9AuxType aKey;              /* The principal punct key (zero offset) */
    T9SYMB    sEmbedded;         /* The embedded punctuation symbol */
    T9U8      bSequenceSize;     /* Number of punctuation symbols */
    T9SYMB    psSequence[16];    /* The punctuation symbols */
};  /* end of struct T9AWPunctInfo   */

/*============================================================================
 * END OF PRIVATE:
 *============================================================================*/

/*----------------------------------------------------------------------------
 *  Define alphabetic word T9 field information structure.
 *---------------------------------------------------------------------------*/
typedef struct T9AWFieldInfo_s {
    T9FieldInfo G; /* Generic data. */

    T9UINT   nUdbFence;      /* Selection list start point for UDB words  */
    T9AWUdbInfo T9FARUDBPOINTER *pUdbInfo;
                            /* Shared user data base.                    */
#ifdef T9CDB_IN_BUILD
    T9AWCdbInfo T9FARUDBPOINTER *pCdbInfo;
                            /* Shared context data base.                    */
#endif
    T9AWMdbInfo MdbInfo;      /* Manufacture database info                 */

    T9U8    bLeadLangFence;    /* the number of Leading language words to appear before Trailing language words */
    T9U8    bTrailLangFence;    /* the number of Trailing language words to appear before remaining */
    struct T9AWPunctInfo_s sPunct[T9MAXBACKUPLANGS + 1][T9AWMAXPUNCTKEYS]; /* Ldb-specific punct data */
    struct T9AW_PrivateNotLdb_s AWPrivate; /* Our private data */
    struct T9AW_PrivateLdb_s AWPrivateLdb[T9MAXBACKUPLANGS + 1]; /* Ldb-specific private data */
} T9AWFieldInfo;

#define T9AWAUXKEYNUM(aAux)   T9AUXKEYNUM(aAux)
#define T9AWAUXSHIFTED(aAux)  ((aAux) & T9_SHIFTEDMASK)

/*----------------------------------------------------------------------------
 *  Define alphabetic word API functions.
 *---------------------------------------------------------------------------*/
T9STATUS T9FARCALL T9AWActivateWordLeft(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWActivateWordRight(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWCheckWordMatch(T9AWFieldInfo *pAWFieldInfo, T9UINT nWordLen, T9SYMB T9FARDATA *psWordBuf, T9AuxType T9FARDATA *paKeyBuf);
T9STATUS T9FARCALL T9AWClrCapState(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWClrReorderWord(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWClrShfState(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWClrUdbCallBack(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWClrLdbCompletion(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWClrUdbCustWordCompletion(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWDelWord(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWGetCodeVersion(T9U8 *pbCoreVerBuf, T9U16 wBufSize, T9U16 *pwCoreVerLen);
T9STATUS T9FARCALL T9AWGetSLCount(T9AWFieldInfo *pAWFieldInfo, T9UINT *pnResponse);
T9STATUS T9FARCALL T9AWInitialize(T9AWFieldInfo *pAWFieldInfo, T9SYMB *psTxtBuf, T9AuxType *paAuxBuf, T9UINT nBufLenMax, T9UINT nBufLen, T9HandleRequest_f T9HandleRequest, T9ReadLdbData_f T9ReadLdbData);
T9STATUS T9FARCALL T9AWNoteWordDone(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWRegisterMdb(T9AWFieldInfo *pAWFieldInfo, T9UINT nMdbFence, T9U8 bWordComplt, T9AWMDBCALLBACK T9ReadMdbData);
T9STATUS T9FARCALL T9AWResetEmbeddedSmartPunct(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWResetSmartPunctTable(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWClrMultitapMode(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWSetCapState(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWSetEmbeddedSmartPunct(T9AWFieldInfo *pAWFieldInfo, T9SYMB *psBuf, T9MINOR mBufLen);
T9STATUS T9FARCALL T9AWSetLanguage(T9AWFieldInfo *pAWFieldInfo, T9UINT nLdbNum, T9FieldInfo *pPreviousFieldInfo, T9U32 *pdwCSUnsupportVal);
T9STATUS T9FARCALL T9AWAddLanguage(T9AWFieldInfo *pAWFieldInfo, T9UINT nLdbNum,
                                   T9U32 *pdwCSUnsupportVal);
T9STATUS T9FARCALL T9AWRemoveLanguage(T9AWFieldInfo *pAWFieldInfo, T9UINT nLdbNum);
T9STATUS T9FARCALL T9AWSetAutoswitchLang(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWClrAutoswitchLang(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWSwitchLeadLang(T9AWFieldInfo *pAWFieldInfo, T9UINT nLdbNum);
T9STATUS T9FARCALL T9AWSetLangFences(T9AWFieldInfo* pAWFieldInfo, T9U8 bLangFenceLead, T9U8 bLangFenceTrail);
T9STATUS T9FARCALL T9AWSetReorderWord(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWSetShfState(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWSetSmartPunctTable(T9AWFieldInfo *pAWFieldInfo, T9SYMB *psBuf, T9MINOR mBufLen);
T9STATUS T9FARCALL T9AWSetUdbCallBack(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWSetLdbCompletion(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWSetUdbCustWordCompletion(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWSetMultitapMode(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWUdbActivate(T9AWFieldInfo *pAWFieldInfo, T9AWUdbInfo T9FARUDBPOINTER *pUdbInfo, T9U16 wDataSize, T9UINT nUdbFence, T9U8 bSymbolClass);
T9STATUS T9FARCALL T9AWUdbAddCustWord(T9AWFieldInfo *pAWFieldInfo, T9SYMB *psBuf, T9MINOR mBufLen);
T9STATUS T9FARCALL T9AWUdbReset(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWUdbDeleteCustWord(T9AWFieldInfo *pAWFieldInfo, T9SYMB *psBuf, T9MINOR mBufLen);
T9STATUS T9FARCALL T9AWUdbGetCustWordCount(T9AWFieldInfo *pAWFieldInfo, T9UINT *pnValue);
T9STATUS T9FARCALL T9AWUdbGetCustWord(T9AWFieldInfo *pAWFieldInfo, T9SYMB *psBuf, T9MINOR mOldBufLen, T9MINOR *pmNewBufLen, T9MINOR mForward);
T9STATUS T9FARCALL T9AWUdbScanBufForCustWords(T9AWFieldInfo *pAWFieldInfo, T9SYMB *psBuf, T9UINT nBufLen);
T9STATUS T9FARCALL T9AWUdbSetFence(T9AWFieldInfo *pAWFieldInfo, T9UINT nValue);
T9STATUS T9FARCALL T9AWUnregisterMdb(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWTUdbGetSize(T9AWFieldInfo *pAWFieldInfo, T9U32 *pdwSize, T9U32 *pdwTotalWords);
T9STATUS T9FARCALL T9AWTUdbExport(T9AWFieldInfo *pAWFieldInfo, T9FARPOINTER pTUdb, T9U32 dwTUdbSize, T9U32 *pdwExportSize, T9WriteTUdb_f T9WriteTUdb, T9U16 *pwNextRecord, T9U32 *pdwTotalExported);
T9STATUS T9FARCALL T9AWTUdbImport(T9AWFieldInfo *pAWFieldInfo, T9FARPOINTER pTUdb, T9U32 dwTUdbSize, T9ReadTUdb_f T9ReadTUdb, T9U32 *pdwTotalImported, T9U32 *pdwTotalRejected);

#ifdef T9CDB_IN_BUILD
T9STATUS T9FARCALL T9AWCdbActivate(T9AWFieldInfo *pAWFieldInfo, T9AWCdbInfo T9FARUDBPOINTER *pCdbInfo, T9U16 wDataSize, T9U8 bSymbolClass);
T9STATUS T9FARCALL T9AWClrCdbWriteByOEM(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWSetCdbWriteByOEM(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWCdbReset(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWFillContextBuffer(T9AWFieldInfo *pAWFieldInfo, T9SYMB *psBuf, T9UINT nBufLen);
T9STATUS T9FARCALL T9AWBreakCdbContext(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWGetWordPrediction(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWClearBuffer(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWSetCdbCompletion(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWClrCdbCompletion(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWSetCdbPrediction(T9AWFieldInfo *pAWFieldInfo);
T9STATUS T9FARCALL T9AWClrCdbPrediction(T9AWFieldInfo *pAWFieldInfo);
#endif

#ifdef T9CHECKCOMPILE
enum {
    T9AWWRONGDATA_BUILDTXTBUF4 = T9CHECKCOMPILE_NEXT,
    T9AWWRONGDATA_SAVEPOINTER7,
    T9AWCDB_OFF,
    T9WRONGVALUE_MAXUDBWORDSIZE,
    T9WRONGVALUE_MAXMDBWORDSIZE,
    T9WRONGVALUE_MAXCDBMATCHES
};

T9U32 T9FARCALL T9AWCheckCompileParameters(T9AWFieldInfo *pAWFieldInfo,
                                           T9U16  *pwT9AWFieldInfoSize, 
                                           T9U8   *pbyT9U8,
                                           T9U8   *pbyT9U16,
                                           T9U8   *pbyT9U32,
                                           T9U8   *pbyT9SYMB,
                                           T9U8   *pbyT9UINT,
                                           T9U8   *pbyT9INT,
                                           T9U8   *pbyT9MINOR,
                                           T9U8   *pbyT9FARDATA,
                                           T9U8   *pbyT9NEARDATA,
                                           T9U8   *pbyVoidPtr,
                                           T9U8   *pbCoreVer,
                                           T9U8   *pbT9MAXDBKEYS,
                                           T9U8   *pbT9MAXTAPKEYS,
                                           T9U8   *pbT9MAXTREES,
                                           T9U8   *pbT9CACHESIZE,
                                           T9U8   *pbT9MAXUDBWORDSIZE,
                                           T9U8   *pbT9MAXMDBWORDSIZE,
                                           T9U32  *pdwBuildTxtBuf4,
                                           T9U32  *pdwInitOK,
                                           T9U32  *pdwSave_Pointers7
#ifdef T9CDB_IN_BUILD   
                                           ,
                                           T9U8   *pbT9MAXCDBMATCHES
#endif
                                           );
#endif

#if defined(__cplusplus)
}
#endif

#endif /* _T9AWAPI_H */
/*-----------------------------------< eof >----------------------------------*/

