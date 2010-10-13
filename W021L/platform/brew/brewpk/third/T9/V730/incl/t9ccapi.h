/*******************************************************************************
********************************************************************************

                    COPYRIGHT 2003-2005 TEGIC COMMUNICATIONS

                TEGIC COMMUNICATIONS PROPRIETARY INFORMATION

     This software is supplied under the terms of a license agreement
     or non-disclosure agreement with Tegic Communications and may not
     be copied or disclosed except in accordance with the terms of that
     agreement.

********************************************************************************

     File Name:   t9ccapi.h

     Description: Header file for T9 chinese character module

     Version:	  7.3                                                       
********************************************************************************
********************************************************************************/

#ifndef _T9CCAPI_H
#define _T9CCAPI_H    1

#include "t9.h"

/* don't mangle the function name if compile under C++ */
#if defined(__cplusplus)
extern "C" {
#endif

/*----------------------------------------------------------------------------
 *  Define T9 core version number
 *
 *  String format is defined as "T9CC VMM.mm.bf"
 *  Where
 *      MM   = major version number
 *      mm   = minor version number
 *      bf   = bug fix version number
 *  Update T9CCCOREVER defn. if any component version # goes beyond 2 digits.
 *----------------------------------------------------------------------------*/
#define T9CCMAJORVER   T9MAJORVER
#define T9CCMINORVER   T9MINORVER
#define T9CCPATCHVER   T9PATCHVER
#define T9CCCOREVER   "T9CC V0" T9CCMAJORVER ".0" T9CCMINORVER ".0" T9CCPATCHVER "   " 

/* Language extension define constants - Chinese */
#if !defined(T9CCSPELLBUFMAXSIZE)
#define T9CCSPELLBUFMAXSIZE         128 /* Size of Chinese spell buffer if used */
#endif
#if !defined(T9CCKEYBUFMAXSIZE)
#define T9CCKEYBUFMAXSIZE           55  /* Size of Chinese key buffer           */
#endif
#if !defined(T9CCSELECTPAGEMAXSIZE)
#define T9CCSELECTPAGEMAXSIZE       6   /* Size of Chinese selection page       */
#endif

#define T9CCDYNCTXTSEARCHMAXDEPTH   8 /* CUDB context search depth */
#define T9CCDEFAULTPHRASECOMPFENCE  3 /* default value of phrase completion fence */

#define T9CCUDBSEARCHACRONYM    0x00 /* search CUDB by acronym match */
#define T9CCUDBSEARCHPHRASE     0x01 /* search CUDB by phrase match  */
#define T9CCUDBSEARCHSTROKE     0x02 /* search CUDB by stroke match  */

/*----------------------------------------------------------------------------
 *  Chinese UDB headers and parameters.
 *----------------------------------------------------------------------------*/
/* Minimum size requested for CUDB in bytes */
#define T9CCUDBMINSIZE          512
#define T9CCUDBPHRASEMAXSIZE    32 /* CUDB phrase maximum length */
#define T9CCUDBACRONYMMAXSIZE   32 /* CUDB acronym maximum length */
#define T9CCUDBFORCEADD         0x01 /* force add by removing older entries    */
#define T9CCUDBNORMALADD        0x00 /* normal add, fails when not enough room */

typedef struct T9CudbInfo_s {

    T9U16 wDataSize;        /* Total size in bytes of this struct (minimum is 256) */
    T9U16 wDataCheck;       /* Additive checksum of database */
    T9U16 wEntryCount;      /* Number of entries currently in user database */
    T9U16 wFreeBytes;       /* Amount of free space in bytes */

    T9SYMB sDataArea[1];    /* Really a variable size data array */

} T9CCUdbInfo;

typedef struct T9CAUdbInfo_s {

    T9U16  wDataSize;        /* Total size in bytes of this struct (minimum is 20) */
    T9U16  wDataCheck;       /* Additive checksum of database */
    T9U16  wHeadLoc;         /* the current location of the head of the list */
    T9U16  wSymbolClass;     /* T9 enum value indicating symbol table mapping for this UDB */
    T9U16  wWordCount;       /* Number of valid words currently in user database */
    T9SYMB sDataArea[1];     /* Really a variable size data array */

} T9CCAudbInfo;

/*----------------------------------------------------------------------------
 *  Chinese state bits
 *----------------------------------------------------------------------------*/
#define T9CCSTATEMORE            (T9U16)0x0001      /* More characters available          */
#define T9CCSTATESTROKE          (T9U16)0x0002      /* Stroke input mode active           */
#define T9CCSTATEPHAMBIG         (T9U16)0x0004      /* Ambiguous phonetic mode active     */
#define T9CCSTATEPHMULTI         (T9U16)0x0008      /* Multi-tap phonetic input mode      */
#define T9CCSTATEAUTOSPELL       (T9U16)0x0010      /* Auto-spell (in ambiguous phonetic) */
#define T9CCSTATEPHMTUPDATE      (T9U16)0x0020      /* Always update Select Page in Multi */
#define T9CCSTATECOMPONENT       (T9U16)0x0040   /* Components enabled for strokes     */
#define T9CCSTATESEARCHUDBONLY   (T9U16)0x0080   /* only cudb search enabled */

#define T9CCSTATEDISABLEAUDBUPDATE   (T9U16)0x0100  /* When set, CAUDB is not changed. */

/* The highest nibble in Chinese state bits are used to store the preferences.
 * Must be persistent unless OEM issue API calls to change them. */
#define T9CCSTATEFUZZYPINYIN     (T9U16)0x8000      /* Fuzzy Pinyin enabled for strokes     */

                                        /* Field mask for perference state bits   */
#define T9CCPREFMASK         (T9U16)0xF000

                                        /* Field mask for phonetic mode bits  */
#define T9CCSTATEPHBITS      (T9CCSTATEPHAMBIG | T9CCSTATEPHMULTI)

                                        /* Field mask for possible mode bits  */
#define T9CCSTATEMODEBITS    (T9CCSTATESTROKE | T9CCSTATEPHBITS | T9CCSTATEAUTOSPELL | T9CCSTATEPHMTUPDATE | T9CCSTATECOMPONENT)


/*----------------------------------------------------------------------------
 *  Chinese T9 language extension structures
 *----------------------------------------------------------------------------*/

#define T9CC_LDBLAYOUTVERSION    0x01
#define T9CC_LDBTYPE             0x08
#define T9CC_DATAVALID           0x1234    /*  Data valid indicator        */
#define T9_COPYRIGHTLENGTH             32    /*  Length of Copyright statement */
#define T9CCLDBVERSIONLENGTH      58    /*  Length of Chinese database version string */

/* Defines the possible total number of data blocks in Chinese Database          */
/* !! Note: this number must be updated if the number of possible blocks changes */
#define T9CCLDBBLOCKCOUNT     66          /*  Number of data pointers     */

/* Define bit masks for Chinese database modules */
#define T9CCLDBSTROKE         0x20            /*  Stroke module               */
#define T9CCLDBCOMPONENT      0x10            /*  Component module            */
#define T9CCLDBPINYIN         0x08            /*  Pinyin module               */
#define T9CCLDBBPMF           0x04            /*  BoPoMoFo module             */
#define T9CCLDBCONTEXT40      0x02            /*  Context module (40K Bytes)  */
#define T9CCLDBCONTEXT20      0x01            /*  Context module (20K Bytes)  */

/*
 * Macro to detect if a module exists in the Chinese LDB.
 * Note: bLdbModuleFlag can be found at pFieldInfo->uLangData.pAuxChinese->bLdbModuleFlag.
 *       bModuleMask is any of Chinese module bit masks defined above.
 */
#define T9CCLDBHASMODULE(bLdbModuleFlag, bModuleMask) (bLdbModuleFlag & bModuleMask)

/* defining the offset of the copyright.  The length is already defined by T9_COPYRIGHTLENGTH */
#define T9CC_HeaderOffsetCopyright      0

/* the definition of offset and length of DBLayoutVersion */
#define T9CC_HeaderOffsetLDBLayoutVer        (T9CC_HeaderOffsetCopyright + T9_COPYRIGHTLENGTH)                /* offset 32 */
#define T9CC_HeaderLengthLDBLayoutVer        1

/* offset and length of DBType */
#define T9CC_HeaderOffsetLDBType         (T9CC_HeaderLengthLDBLayoutVer + T9CC_HeaderOffsetLDBLayoutVer)   /* 33 */
#define T9CC_HeaderLengthLDBType         1

#define T9CC_HeaderOffsetDataValid     (T9CC_HeaderLengthLDBType + T9CC_HeaderOffsetLDBType)                  /* 34 */
#define T9CC_HeaderLengthDataValid     2

#if T9CC_HeaderOffsetLanguageID != (T9CC_HeaderLengthDataValid + T9CC_HeaderOffsetDataValid)    /* 36 */
#error T9CC_HeaderOffsetLanguageID must be (T9CC_HeaderLengthDataValid + T9CC_HeaderOffsetDataValid)    /* 36 */
#endif
#define T9CC_HeaderLengthLanguageID        2

#define T9CC_HeaderOffsetCompatID          (T9CC_HeaderLengthLanguageID + T9CC_HeaderOffsetLanguageID)          /* 38 */
#define T9CC_HeaderLengthCompatID          2

#define T9CC_HeaderOffsetOEMID             (T9CC_HeaderLengthCompatID + T9CC_HeaderOffsetCompatID)    /* 40 */
#define T9CC_HeaderLengthOEMID             2

#define T9CC_HeaderOffsetCheckSum          (T9CC_HeaderLengthOEMID + T9CC_HeaderOffsetOEMID)                    /* 42 */
#define T9CC_HeaderLengthCheckSum          2

#define T9CC_HeaderOffsetTotalIdCodes      (T9CC_HeaderLengthCheckSum + T9CC_HeaderOffsetCheckSum)              /* 44 */
#define T9CC_HeaderLengthTotalIdCodes        2

#define T9CC_HeaderOffsetLDBVersion         (T9CC_HeaderLengthTotalIdCodes + T9CC_HeaderOffsetTotalIdCodes)      /* 46 */
#define T9CC_HeaderLengthLDBVersion         1

#define T9CC_HeaderOffsetLDBEncoding        (T9CC_HeaderLengthLDBVersion + T9CC_HeaderOffsetLDBVersion)            /* 47 */
#define T9CC_HeaderLengthLDBEncoding        1

#define T9CC_HeaderOffsetCustomerID        (T9CC_HeaderLengthLDBEncoding + T9CC_HeaderOffsetLDBEncoding)          /* 48 */
#define T9CC_HeaderLengthCustomerID        1

#define T9CC_HeaderOffsetLDBContent         (T9CC_HeaderLengthCustomerID + T9CC_HeaderOffsetCustomerID)          /* 49 */
#define T9CC_HeaderLengthLDBContent         1

#define T9CC_HeaderOffsetModuleContent     (T9CC_HeaderLengthLDBContent + T9CC_HeaderOffsetLDBContent)            /* 50 */
#define T9CC_HeaderLengthModuleContent     1

#define T9CC_HeaderOffsetStrokeVersion     (T9CC_HeaderLengthModuleContent + T9CC_HeaderOffsetModuleContent)    /* 51 */
#define T9CC_HeaderLengthStrokeVersion     1

#define T9CC_HeaderOffsetPhoneticVer       (T9CC_HeaderLengthStrokeVersion + T9CC_HeaderOffsetStrokeVersion)    /* 52 */
#define T9CC_HeaderLengthPhoneticVer       1

#define T9CC_HeaderOffsetContextVersion    (T9CC_HeaderLengthPhoneticVer + T9CC_HeaderOffsetPhoneticVer)   /* 53 */
#define T9CC_HeaderLengthContextVersion    1

#define T9CC_HeaderOffsetComponentVer      (T9CC_HeaderLengthContextVersion + T9CC_HeaderOffsetContextVersion)  /* 54 */
#define T9CC_HeaderLengthComponentVer      1

#define T9CC_HeaderOffsetStrokeSystem      (T9CC_HeaderLengthComponentVer + T9CC_HeaderOffsetComponentVer)  /* 55 */
#define T9CC_HeaderLengthStrokeSystem      1

#define T9CC_HeaderOffsetPhoneticKey       (T9CC_HeaderLengthStrokeSystem + T9CC_HeaderOffsetStrokeSystem)      /* 56 */
#define T9CC_HeaderLengthPhoneticKey       1

#define T9CC_HeaderOffsetContextLimit      (T9CC_HeaderLengthPhoneticKey + T9CC_HeaderOffsetPhoneticKey)        /* 57 */
#define T9CC_HeaderLengthContextLimit      1

#define T9CC_HeaderOffsetDBBuildDay        (T9CC_HeaderLengthContextLimit + T9CC_HeaderOffsetContextLimit)      /* 58 */
#define T9CC_HeaderLengthDBBuildDay        1

#define T9CC_HeaderOffsetDBBldMonthYr      (T9CC_HeaderLengthDBBuildDay + T9CC_HeaderOffsetDBBuildDay)         /* 59 */
#define T9CC_HeaderLengthDBBldMonthYr      1

#define T9CC_HeaderOffsetBlockSize         (T9CC_HeaderLengthDBBldMonthYr + T9CC_HeaderOffsetDBBldMonthYr)  /* 60 or 0x3B */
#define T9CC_HeaderLengthBlockSize         (T9CCLDBBLOCKCOUNT * (sizeof(T9U16)))

#define T9CCHeaderOffsetBlockOffset       (T9CC_HeaderOffsetBlockSize + T9CCLDBBLOCKCOUNT * (sizeof(T9U16))) /* 192 or 0xC0 */
#define T9CC_HeaderLengthBlockOffset       (T9CCLDBBLOCKCOUNT * (sizeof(T9U8 const T9FARDATA *)))

/* Define the Chinese LDB header size in bytes. */
#define T9CCLdbHeaderSize    T9CCHeaderOffsetBlockOffset
/*
 * This structure defines the Chinese database as seen from the system
 * integrator's viewpoint.  For external data systems, the pUserView element
 * of the T9CCFieldInfo must be initialized to point to a valid instance
 * of this structure before initializing T9 for Chinese language support.
 */
typedef struct  T9ChineseData_s {
    /* the database structure is not hidden in the T9U8 array to solve the
     * header alignment issue
     */
    T9U8    bHeader[T9CCLdbHeaderSize];

    /*  The following array defines the pointers to the various parts of the
        Chinese database.  For external binary Chinese data delivery, the binary
        data file starts with this structure with the dwOffset entries containing
        absolute offsets into further sections of the binary data file.

        The system integration logic is responsible for fixing up these offsets
        into pointers which point to the actual run time locations of the data
        blocks before initializing T9 for Chinese language support.

        It is most efficient to do this fixup process when the database is
        stored into system PROM, but it can also be done at run time by
        copying this structure into a RAM copy and performing a simple
        address operation on each element.

        Note that dwOffset entries equal to zero indicate empty data blocks
        and these must be left equal to zero during the pointer fixup operation.
    */
    /* getting rid of the T9U32 offset type from the union because some OEM may use 3-byte pointer */
    T9U8 const T9FARDATA *pBlockPointers[T9CCLDBBLOCKCOUNT];

} T9ChineseData;


/*============================================================================
 * NOTE:
 *   The following definitions are private to AOL and are subject
 *   to change without notice.
 *============================================================================*/
/* Define structure T9 Chinese Private persistent data. */
struct T9CC_Private_s {
    T9U16 wCCInitOK; /* Has the Chinese module initialization happened? */

    /* T9U8 fields */
    T9U8 bSavedDisplaySet;          /* Saved display code set for display code lookup */
    T9U8 bPartialBuffNext;          /* Next entry from partial match buffer */
    T9U8 bPartialBuffTotal;         /* Total entries in partial match buffer */
    T9U8 bLastKey;                  /* Last key argument for Chinese multitap */
    T9U8 bDoFillCSM;                /* Flag to update selection page in post process */
    T9U8 bCMagicStringOn;           /* Flag that magic string state is on */
    T9U8 bCMagicState;              /* The Chinese magic string state:
                                     * 0 = not in magic string state,
                                     * 1 = give Chinese LDB version string,
                                     * 2 = give T9 core version string */
    T9U8 bOrigKeyBufLen;            /* Original key buffer length (without Fuzzy Pinyin info) */
    T9U8 pbOrigKeyBuf[6];           /* Original key buffer content.
                                     * Should hold keys for longest pinyin syllable */

    T9U8 bCaudbThresholdPageNum;    /* Five CAUDB entry creation threshholds */
    T9U8 bCaudbThresholdKeyCountWithPage;
    T9U8 bCaudbThresholdPageNumWithKey;
    T9U8 bCaudbThresholdKeyCount;
    T9U8 bCaudbThresholdCheckTone;

    T9U8 bCaudbCacheCount;          /* Entry count in CAUDB cache */
    T9U8 bCaudbCacheCurrentPos;     /* Current position in CAUDB cache */
    T9U8 bSetup2ndBin;              /* Flag whether to search 2nd bin set for compressed bin */
    T9U8 pbAuxSelectBuf[T9CCSELECTPAGEMAXSIZE]; /* Auxiliary buffer of the selection page */
    T9U8 bUdbPhraseComp;            /* 1 if Phrase completion comes from Udb */
    T9U8 bFoundMatchSize;           /* Length of phrase in found phrase */

    /* T9U16 fields */
    T9U16 wSavedDisplayCode;        /* Saved display code for display code lookup */
    T9U16 wSavedDisplayIndex;       /* Saved display index for display code lookup */
    T9U16 wStrokeData[12];          /* array of STROKES_ARR_SIZE == 12 elements for search */
    T9U16 wStrokeMask[12];          /* array of STROKES_ARR_SIZE == 12 elements for search */
    T9U16 wPartialMatchBuff[16];    /* Partial matches found during exact match scan */
    T9U16 wMatchBuffer[64];         /* Display codes for most recently found matches */
    T9U16 wMatchIdBuffer[64];       /* Tegic ID for most recently found matches */
    T9U16 wCaudbCache[10];          /* Cache for filtered CAUDB matches */
	T9U16 wExactRecordBlk;          /* Current search block for exact context record */
	T9U16 wExactRecordOff;			/* Current search offset for exact context record */
	T9U16 wPartialRecordBlk;        /* Current search block for partial context record */
	T9U16 wPartialRecordOff;		/* Current search offset for partial context record */
	T9U16 wPhoneticBlk;				/* Phonetic block 1 */
	T9U16 wPhoneticBlk2;			/* Phonetic block 2 */
    T9U16 wFoundMatch;              /* Context Character of phrase found in search */
    T9U16 pwAuxAudbBuf[T9CCSELECTPAGEMAXSIZE]; /* Auxilliary buffer for AUDB update */

    /* T9UINT fields */
    T9UINT nMatchBufferCount;        /* Number of entries in the match buffer */
    T9UINT nMatchBufferStart;        /* Number of first entry in match buffer  */
    T9UINT nStrokeCount;             /* Number of strokes to search with */
    T9UINT nStrokeWords;             /* Number of stroke data words to check */
    T9UINT nTotalNibbles;            /* Nibbles in current context record */
    T9UINT nExactNibble;             /* Current nibble in exact context search */
    T9UINT nPartialNibble;           /* Current nibble in partial context search */
    T9UINT nNextExactId;             /* ID code of next exact match to check */
    T9UINT nExactMatchCount;         /* Count of exact stroke matches so far */
    T9UINT nNextPartialId;           /* ID code of next partial match to check */
    T9UINT nPartialBinCount;         /* Number of partial match phonetic bins */
    T9UINT nPartialBinMaxLen;        /* Length of largest partial phonetic bin */
    T9UINT nPartialBinOffset;        /* Offset of current partial phonetic bin */
    T9UINT nNextPartialBin;          /* Next partial bin number to check */
    T9UINT nNextExactId2;            /* ID code of next exact match to check in 2nd bin set */
    T9UINT nNextPartialId2;          /* ID code of next partial match to check in 2nd bin set */
    T9UINT nPartialBinCount2;        /* Number of partial match phonetic bins in 2nd bin set */
    T9UINT nPartialBinMaxLen2;       /* Length of largest partial phonetic bin in 2nd bin set */
    T9UINT nPartialBinOffset2;       /* Current partial check point in 2nd bin set */
    T9UINT nNextPartialBin2;         /* Next partial bin number to check in 2nd bin set */
    T9UINT nDymCtxtSelection;        /* The selection from dynamic context */
    T9UINT nCMagicCursorStart;       /* Cursor position where the magic string should start */

    /* pointer fields */
    T9U8 *pbActiveSpell;                        /* Pointer to active spell */
    const T9U8 T9FARDATA *pbExactRecord;          /* Pointer to exact search context record */
    const T9U8 T9FARDATA *pbPartialRecord;        /* Pointer to partial search context record */
    const T9U16 T9FARDATA *pwRelatedComponents;   /* Pointer to list of components related to selected component */
    const T9U16 T9FARDATA *pwPhoneticMCFBinRef;   /* Phonetic data tbl ptr for 1st bin set */
    const T9U16 T9FARDATA *pwTone5Flags;          /* Phonetic data tbl ptr for 1st bin set */
    const T9U16 T9FARDATA *pwTone2Bits;           /* Phonetic data tbl ptr for 1st bin set */
    const T9U16 T9FARDATA *pwPhoneticCharIdxList; /* Phonetic data tbl ptr for 1st bin set */
    const T9U16 T9FARDATA *pwPhoneticMCFBinRef2;  /* Phonetic data tbl ptr for 2nd bin set */
    const T9U16 T9FARDATA *pwTone5Flags2;         /* Phonetic data tbl ptr for 2nd bin set */
    const T9U16 T9FARDATA *pwTone2Bits2;          /* Phonetic data tbl ptr for 2nd bin set */
    const T9U16 T9FARDATA *pwPhoneticCharIdxList2;/* Phonetic data tbl ptr for 2nd bin set */

	/* Chinese LDB header */
    T9ChineseData    T9CC_LdbUserView;

    /* The following pointers need macro access because they point to structs */
    void T9FARDATA *pPrivatePTR[4];
};  /* end of struct T9CC_Private_s */

/*============================================================================
 * END OF PRIVATE:
 *============================================================================*/


typedef struct T9CCUdbObj_s {
    T9U8 bPhraseSize;                       /* size of phrase  */
    T9U8 bAcronymSize;                      /* size of acronym */
    T9SYMB psPhrase[T9CCUDBPHRASEMAXSIZE];  /* phrase buffer   */
    T9U8 pbAcronym[T9CCUDBACRONYMMAXSIZE];  /* acronym buffer  */
} T9CCUdbObj;


/*----------------------------------------------------------------------------
 *  Chinese aux information structure
 *----------------------------------------------------------------------------*/
typedef struct T9CCFieldInfo_s T9CCFieldInfo;
typedef T9STATUS (T9FARCALL *T9CCLoadLdb_f)(T9CCFieldInfo*, T9ChineseData T9FARDATA *);

struct T9CCFieldInfo_s {
    T9FieldInfo G; /* Generic data. */

    T9U8  pbKeyBuf[T9CCKEYBUFMAXSIZE];       /* Buffer of keystrokes */
    T9UINT nKeyBufLen;                       /* Number of keystrokes currently in the buffer */
    T9U16 pwSelectPage[T9CCSELECTPAGEMAXSIZE];   /* Buffer of the selection page */
    T9UINT nSelectPageMax;                   /* Current max size of selection page */
    T9UINT nSelectPageLen;                   /* Current number of valid entries in selection page */
    T9UINT nSelectPageNum;                   /* 0-based current page number of selection page */
    T9U8  bToneCode;                         /* Filter value to provide only 1 tone for keys */
    T9U8  bSpellCode;                        /* Filter value to provide only 1 phonetic form for keys */
    T9U16 wChineseState;                     /* Current state bits for Chinese */
    T9U8  pbSpellBuf[T9CCSPELLBUFMAXSIZE];   /* Buffer for Pinyin or BoPoMoFo spellings */
    T9U16 wComponent;                        /* Selected component (0 if none) */
    T9U8  bNumCompStrokes;                   /* Number of strokes in current component */
    T9U16 wFirstComponent;                   /* First available component code  (0 if no components available)*/
    T9U16 wLastComponent;                    /* Last available component code */
    T9UINT nDymCtxtFence;                    /* the fence of Dynamic context */
    T9UINT nDymCtxtSearchDepth;              /* Dynamic context Search Depth */
    T9U8  bPhraseCompFence;                  /* Phrase completion fence */
    T9U8  bPhraseCompLen;                    /* Length of completion part of phrase completion */
    T9U16 wPhraseCompCursor;                 /* Original cursor before phrase completion occurs */
    T9CCUdbInfo  T9FARUDBPOINTER *pCudbInfo; /*Pointer to Chinese UDB */
    T9CCUdbObj   UdbCacheObj;                /* cache copy of the current UDB object */
    T9CCAudbInfo   T9FARUDBPOINTER *pCAUdbInfo;        /* Pointer to Chinese Auto User database */
    T9UINT       nCAUdbFence;                        /* AUDB fence */
    T9U8  bLdbModuleFlag;             /* Flags to indicate which modules are available in current LDB */
    T9U8  bSpellMatchFromLdb;         /* record how many matched spells are from ldb */
    T9U8  bSpellMatchFromCudb;        /* record how many matched spells are from cudb*/
    struct T9CC_Private_s T9CPrivate; /* Persistent memory for T9 Chinese */
    void       *pOEMPrivate;          /* pointer for OEM use for callbacks           */

	T9CCLoadLdb_f T9CCLoadLdb;
};


T9STATUS T9FARCALL T9CCAcceptPhraseComp(T9CCFieldInfo *pCCFieldInfo);
T9STATUS T9FARCALL T9CCAddExplicitSpellSym(T9CCFieldInfo *pCCFieldInfo, T9U8 bChar);
T9STATUS T9FARCALL T9CCAudbAddObj(T9CCFieldInfo *pCCFieldInfo, T9SYMB *psBuf);
T9STATUS T9FARCALL T9CCAudbSetFence(T9CCFieldInfo *pCCFieldInfo, T9UINT nValue);
T9STATUS T9FARCALL T9CCClrAutoSpell(T9CCFieldInfo *pCCFieldInfo);
T9STATUS T9FARCALL T9CCClrComponents(T9CCFieldInfo *pCCFieldInfo);
T9STATUS T9FARCALL T9CCClrFuzzyPinyin(T9CCFieldInfo *pCCFieldInfo);
T9STATUS T9FARCALL T9CCClrMultitapSelListUpdate(T9CCFieldInfo *pCCFieldInfo);
T9STATUS T9FARCALL T9CCClrUdbSearchOnly(T9CCFieldInfo *pCCFieldInfo);
T9STATUS T9FARCALL T9CCDelAllKeys(T9CCFieldInfo *pCCFieldInfo);
T9STATUS T9FARCALL T9CCAudbGetThresholds(T9CCFieldInfo *pCCFieldInfo, T9U8 *pbPageNum, T9U8 *pbKeyCountWithPage, T9U8 *pbPageNumWithKey, T9U8 *pbKeyCount, T9U8 *pbCheckTone);
T9STATUS T9FARCALL T9CCGetCharSpell(T9CCFieldInfo *pCCFieldInfo, T9SYMB *psBuf, T9MINOR mIndex);
T9STATUS T9FARCALL T9CCGetCharStrokes(T9CCFieldInfo *pCCFieldInfo, T9SYMB *psBuf, T9MINOR mIndex);
T9STATUS T9FARCALL T9CCGetCodeVersion(T9U8 *pbCoreVerBuf, T9U16 wBufSize, T9U16 *pwCoreVerLen);
T9STATUS T9FARCALL T9CCGetSpell(T9CCFieldInfo *pCCFieldInfo);
T9STATUS T9FARCALL T9CCAudbGetObj(T9CCFieldInfo *pCCFieldInfo, T9UINT nIndex, T9SYMB *psBuf);
T9STATUS T9FARCALL T9CCGetToneOptions(T9CCFieldInfo *pCCFieldInfo, T9UINT *pnResponse);
T9STATUS T9FARCALL T9CCAudbActivate(T9CCFieldInfo *pCCFieldInfo, T9CCAudbInfo T9FARUDBPOINTER *pCAUdbInfo, T9U16 wDataSize, T9UINT nCAUdbFence);
T9STATUS T9FARCALL T9CCInitialize(T9CCFieldInfo *pCCFieldInfo, T9SYMB *psTxtBuf, T9AuxType *paAuxBuf, T9UINT nBufLenMax, T9UINT nBufLen, T9HandleRequest_f T9HandleRequest, T9UINT nSelectPageMax, T9CCLoadLdb_f T9CCLoadLdb);
T9STATUS T9FARCALL T9CCMatchStrokExtDynCtxt(T9CCFieldInfo *pCCFieldInfo, T9UINT nMatches, T9UINT nStart, T9UINT *pnResponse);
T9STATUS T9FARCALL T9CCRejectPhraseComp(T9CCFieldInfo *pCCFieldInfo);
T9STATUS T9FARCALL T9CCAudbReset(T9CCFieldInfo *pCCFieldInfo);
T9STATUS T9FARCALL T9CCUdbReset(T9CCFieldInfo *pCCFieldInfo);
T9STATUS T9FARCALL T9CCRestoreState(T9CCFieldInfo *pCCFieldInfo, T9U16 wChineseState);
T9STATUS T9FARCALL T9CCSaveState(T9CCFieldInfo *pCCFieldInfo, T9U16 *pwChineseState);
T9STATUS T9FARCALL T9CCSetAmbigSpellMode(T9CCFieldInfo *pCCFieldInfo);
T9STATUS T9FARCALL T9CCSetMultitapSpellMode(T9CCFieldInfo *pCCFieldInfo);
T9STATUS T9FARCALL T9CCSetPhraseCompFence(T9CCFieldInfo *pCCFieldInfo, T9UINT nValue);
T9STATUS T9FARCALL T9CCAudbSetThresholds(T9CCFieldInfo *pCCFieldInfo, T9U8 bPageNum, T9U8 bKeyCountWithPage, T9U8 bPageNumWithKey, T9U8 bKeyCount, T9U8 bCheckTone);
T9STATUS T9FARCALL T9CCSetAutoSpell(T9CCFieldInfo *pCCFieldInfo);
T9STATUS T9FARCALL T9CCSetComponents(T9CCFieldInfo *pCCFieldInfo);
T9STATUS T9FARCALL T9CCSetDynCtxtSearchDepth(T9CCFieldInfo *pCCFieldInfo, T9UINT nValue);
T9STATUS T9FARCALL T9CCSetFuzzyPinyin(T9CCFieldInfo *pCCFieldInfo);
T9STATUS T9FARCALL T9CCSetLanguage(T9CCFieldInfo *pCCFieldInfo, T9UINT nLdbNum,
                                 T9FieldInfo *pPreviousFieldInfo,
                                 T9CCAudbInfo T9FARUDBPOINTER *pCAUdbInfo);
T9STATUS T9FARCALL T9CCSetMultitapSelListUpdate(T9CCFieldInfo *pCCFieldInfo);
T9STATUS T9FARCALL T9CCSetDynCtxtFence(T9CCFieldInfo *pCCFieldInfo, T9UINT nValue);
T9STATUS T9FARCALL T9CCSetTone(T9CCFieldInfo *pCCFieldInfo, T9KEY eKeyID);
T9STATUS T9FARCALL T9CCSpecifySpell(T9CCFieldInfo *pCCFieldInfo, T9UINT nValue);
T9STATUS T9FARCALL T9CCSetStrokeMode(T9CCFieldInfo *pCCFieldInfo);
T9STATUS T9FARCALL T9CCSetUdbSearchOnly(T9CCFieldInfo *pCCFieldInfo);
T9STATUS T9FARCALL T9CCUdbActivate(T9CCFieldInfo *pCCFieldInfo, T9CCUdbInfo T9FARUDBPOINTER *pCudbInfo, T9U16 wDataSize);
T9STATUS T9FARCALL T9CCUdbAddObj(T9CCFieldInfo *pCCFieldInfo, T9CCUdbObj *pTargetObj, T9U8 bOption);
T9STATUS T9FARCALL T9CCUdbDelObj(T9CCFieldInfo *pCCFieldInfo, T9CCUdbObj *pTargetObj);
T9STATUS T9FARCALL T9CCUdbEditObj(T9CCFieldInfo *pCCFieldInfo, T9CCUdbObj *pTargetObj, T9CCUdbObj *pResultObj, T9U8 bOption);
T9STATUS T9FARCALL T9CCUdbGetDefaultAcronym(T9CCFieldInfo *pCCFieldInfo, T9CCUdbObj *pTargetObj);
T9STATUS T9FARCALL T9CCUdbGetNextObj(T9CCFieldInfo *pCCFieldInfo, T9CCUdbObj *pTargetObj, T9CCUdbObj *pResultObj, const T9U8 *pbBuf, T9U8 bBufBytes, T9U8 bOption);
T9STATUS T9FARCALL T9CCUdbGetPrevObj(T9CCFieldInfo *pCCFieldInfo, T9CCUdbObj *pTargetObj, T9CCUdbObj *pResultObj, const T9U8 *pbBuf, T9U8 bBufBytes, T9U8 bOption);
T9STATUS T9FARCALL T9CCUdbPopulate(T9CCFieldInfo *pCCFieldInfo, T9SYMB *psBuf);
T9STATUS T9FARCALL T9CCUdbUseObj(T9CCFieldInfo *pCCFieldInfo, T9CCUdbObj *pTargetObj);
T9STATUS T9FARCALL T9CCAudbEnableUpdate(T9CCFieldInfo *pCCFieldInfo);
T9STATUS T9FARCALL T9CCAudbDisableUpdate(T9CCFieldInfo *pCCFieldInfo);

#ifdef T9CHECKCOMPILE
enum {
    T9CCWRONGDATA_ORIGKEYBUF4 = T9CHECKCOMPILE_NEXT,
    T9CCWRONGDATA_MATCHBUFFER9,
    T9CCWRONGDATA_DYMCTXTSELECTION,
    T9CCWRONGVALUE_SPELLBUFMAXSIZE,
    T9CCWRONGVALUE_KEYBUFMAXSIZE,
    T9CCWRONGVALUE_SELECTPAGEMAXSIZE

};


T9U32 T9FARCALL T9CCCheckCompileParameters(T9CCFieldInfo *pCCFieldInfo,
                                           T9U16  *pwT9CCFieldInfoSize, 
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
                                           T9U8   *pbT9CCSPELLBUFMAXSIZE,
                                           T9U8   *pbT9CCKEYBUFMAXSIZE,
                                           T9U8   *pbT9CCSELECTPAGEMAXSIZE,
                                           T9U32  *pdwOrigKeyBuf4,
                                           T9U32  *pdwMatchBuffer9,
                                           T9U32  *pdwDymCtxtSelection,
                                           T9U32  *pdwInitOK);

#endif

#if defined(__cplusplus)
}
#endif

#endif /* _T9CCAPI_H */
/*-----------------------------------< eof >----------------------------------*/

