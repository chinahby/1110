/*******************************************************************************
;*******************************************************************************
;**                                                                           **
;**                  COPYRIGHT 1997-2005 TEGIC COMMUNICATIONS                 **
;**                                                                           **
;**                TEGIC COMMUNICATIONS PROPRIETARY INFORMATION               **
;**                                                                           **
;**     This software is supplied under the terms of a license agreement      **
;**     or non-disclosure agreement with Tegic Communications and may not     **
;**     be copied or disclosed except in accordance with the terms of that    **
;**     agreement.                                                            **
;**                                                                           **
;**     T9 Export Control Classification Number ECCN: EAR99                   **
;**                                                                           **
;*******************************************************************************
;**                                                                           **
;**     FileName: t9.h                                                        **
;**                                                                           **
;**  Description: Generic API Interface Header File.                          **
;**  Version:	  7.3                                                         **
;*******************************************************************************
;******************************************************************************/

#ifndef T9_H
#define T9_H 1

/* don't mangle the function name if compile under C++ */
#if defined(__cplusplus)
extern "C" {
#endif

/*----------------------------------------------------------------------------
 *  Define T9 core version number
 *
 *  String format is defined as "T9 VMM.mm.bf"
 *  Where
 *      MM   = major version number
 *      mm   = minor version number
 *      bf   = bug fix version number
 *  Update T9COREVER defn. if any component version # goes beyond 2 digits.
 *----------------------------------------------------------------------------*/
#define T9MAJORVER   "7"
#define T9MINORVER   "3"
#define T9PATCHVER   "0"
#define T9COREVER   "T9 V0" T9MAJORVER ".0" T9MINORVER ".0" T9PATCHVER "   "


/*----------------------------------------------------------------------------
 *  Define fundamental 8, 16, and 32 bit quantities
 *----------------------------------------------------------------------------*/

#define T9S8      signed char         /* Signed  8 bit quantity               */
#define T9S16     short               /* Signed at least 16 bit quantity      */
#define T9S32     long                /* Signed at least 32 bit quantity      */

#ifndef T9U8
#define T9U8      unsigned char         /* Unsigned  8 bit quantity           */
#endif /* !T9U8 */

#ifndef T9U16
#define T9U16     unsigned T9S16        /* Unsigned 16 bit quantity           */
#endif /* !T9U16 */

#ifndef T9U32
#define T9U32     unsigned T9S32        /* Unsigned 32 bit quantity           */
#endif /* !T9U32 */

#include "xxt9oem.h"

#ifdef T9EVENTAPI
#include "t9names1.h"
#endif

/*----------------------------------------------------------------------------
 *  Default number of bytes in each T9 symbol (aka character) to 2 (Unicode)
 *----------------------------------------------------------------------------*/
#ifndef T9SYMBOLWIDTH
#define T9SYMBOLWIDTH   2           /* Width of T9 Symbols (1 or 2)           */
#endif /* !T9SYMBOLWIDTH */

/*----------------------------------------------------------------------------
 *  Define sizes of T9 types which aren't fixed size.
 *----------------------------------------------------------------------------*/
#ifndef T9MINORWIDTH
#define T9MINORWIDTH    1           /* Word size of target machine.     */
#endif /* !T9MINORWIDTH */

#ifndef T9NUMBERWIDTH
#if T9MINORWIDTH < 2
#define T9NUMBERWIDTH 2             /* Word size, except min of 2 bytes.  */
#else
#define T9NUMBERWIDTH T9MINORWIDTH  /* Word size, except min of 2 bytes.  */
#endif
#endif /* !T9NUMBERWIDTH */

/*----------------------------------------------------------------------------
 *  Define the T9 symbol type based on T9SYMBOLWIDTH
 *----------------------------------------------------------------------------*/

#if (T9SYMBOLWIDTH == 1)              /* Width = 1 --> Symbols are 8 bits     */

#ifndef T9SYMB
#define T9SYMB  T9U8
#endif /* !T9SYMB */

#elif (T9SYMBOLWIDTH == 2)            /* Width = 2 --> Symbols are 16 bits    */

#ifndef T9SYMB
#define T9SYMB  T9U16
#endif /* !T9SYMB */

#else

#error T9SYMBOLWIDTH undefined or invalid

#endif /* T9SYMBOLWIDTH */

/*----------------------------------------------------------------------------
 *  Define the T9 data element for small numbers based on T9MINORWIDTH
 *----------------------------------------------------------------------------*/

#if (T9MINORWIDTH == 1)               /* Width = 1 --> Smalls are 8 bits      */

#ifndef T9MINOR
#define T9MINOR  T9U8
#endif /* !T9MINOR */

#elif (T9MINORWIDTH == 2)             /* Width = 2 --> Smalls are 16 bits     */

#ifndef T9MINOR
#define T9MINOR  T9U16
#endif /* !T9MINOR */

#elif (T9MINORWIDTH == 4)             /* Width = 4 --> Smalls are 32 bits     */

#ifndef T9MINOR
#define T9MINOR  T9U32
#endif /* !T9MINOR */

#else

#error T9MINORWIDTH undefined or invalid

#endif /* T9MINORWIDTH */

/*----------------------------------------------------------------------------
 *  Define the T9 data element for general numbers based on T9NUMBERWIDTH
 *----------------------------------------------------------------------------*/

#if (T9NUMBERWIDTH < T9MINORWIDTH)  /* Ensure T9NUMBERWIDTH >= T9MINORWIDTH  */
#error T9NUMBERWIDTH must be greater than or equal to T9MINORWIDTH
#endif

#if (T9NUMBERWIDTH == 2)              /* Width = 2 --> Numbers are 16 bits    */

#ifndef T9UINT
#define T9UINT   T9U16
#endif /* !T9UINT  */

#ifndef T9INT
#define T9INT  T9S16
#endif /* !T9INT  */

#elif (T9NUMBERWIDTH == 4)            /* Width = 4 --> Numbers are 32 bits    */

#ifndef T9UINT
#define T9UINT   T9U32
#endif /* !T9UINT  */

#ifndef T9INT
#define T9INT  T9S32
#endif /* !T9INT  */

#else

#error T9NUMBERWIDTH undefined or invalid

#endif /* T9NUMBERWIDTH */


/*----------------------------------------------------------------------------
 *  Define function and data prefixes
 *----------------------------------------------------------------------------*/

#ifndef T9FARCALL
#define T9FARCALL                     /* Functions callable from any module.    */
#endif /* !T9FARCALL  */
#ifndef T9FARDATA
#define T9FARDATA                     /* Constant romable data blocks.          */
#endif /* !T9FARDATA  */

#ifndef T9NEARCALL
#define T9NEARCALL /* Function called only locally (also use "static")  */
#endif
#ifndef T9NEARDATA /* Global data used only within one file (also use static). */
#define T9NEARDATA
#endif

typedef T9U8 T9FARDATA * T9FARPOINTER; /* Needed for two levels of indirection. */

/* For releases 7.0 and later, this macro no longer set by OEM.  This is defined to equal T9FARDATA. */
#ifdef T9FARUDBPOINTER
#error T9FARUDBPOINTER MACRO MUST BE SET BY T9.
#endif
#define T9FARUDBPOINTER T9FARDATA /* pointer is set same as T9FARDATA pointer.*/

/*----------------------------------------------------------------------------
 * handle flag for including CDB in build 
 *----------------------------------------------------------------------------*/
#ifndef T9_NO_CDB_IN_BUILD
#define T9CDB_IN_BUILD
#endif

/* setup for Multilanguage ldb reading */
#ifndef T9MAXBACKUPLANGS
#define T9MAXBACKUPLANGS 1
#endif

/*----------------------------------------------------------------------------
 *  Define maximum length of the complete T9 compatibility ID string.
 *----------------------------------------------------------------------------*/
#define T9MAXCOMPATIDSTRLEN 64

/*----------------------------------------------------------------------------
 *  Define T9 compatibility index related macros.
 *----------------------------------------------------------------------------*/
/* Max number of offsets each core can be compatible with. */
#define T9_MAXCOMPATIDXOFFSET    16

/* Convert the compatibility ID offset to the bit mask value. */
#define T9_MASKOFFSET(CompatIdxOffset)  (T9U16)(1 << (CompatIdxOffset - 1))

/*---------------------------------------------------------------------------
 *  NOTE:    The following value must be reviewed and updated as needed on
 *           each release
 *  Example: If the core can take the compatibility ID 1,2, and 3, here is how
 *           the compatibility base and offset should be defined.
 *           #define T9AW_COMPATIDXBASE    1
 *           #define T9AW_COMPATIDXOFFSET  (T9_MASKOFFSET(2 - T9AW_COMPATIDXBASE) | T9_MASKOFFSET(3 - T9AW_COMPATIDXBASE))
 *---------------------------------------------------------------------------*/
#define T9AW_COMPATIDXBASE     3
#define T9AW_COMPATIDXOFFSET   0

#define T9CC_COMPATIDXBASE      3
#define T9CC_COMPATIDXOFFSET    0

#define T9JW_COMPATIDXBASE      2
#define T9JW_COMPATIDXOFFSET    0

#define T9KC_COMPATIDXBASE      3
#define T9KC_COMPATIDXOFFSET    0

 /*----------------------------------------------------------------------------
 *  Define some basic T9 database constants.
 *----------------------------------------------------------------------------*/

#ifndef T9_MAXSYMTOKEYRANGES
#define T9_MAXSYMTOKEYRANGES           8 /* Max # of SymToKey ranges in an LDB. */
#endif

#define T9_NUMSIZECLASSNIBBLERANGES    3
#define T9_NUMVALIDKEYSNIBBLERANGES    3
#define T9_NUMINSTRUCTIONNIBBLERANGES  3
#define T9_NUMSMARTPUNCTTABLERANGES  16  /* Max # of entries use to hold the smart punct table */
#ifndef T9MAXDBKEYS
#define T9MAXDBKEYS                   12
#endif
#if T9MAXDBKEYS > 12
#error Currently, the number of LDB keys must be than or equal to 12.
#endif
#ifndef T9MAXTAPKEYS
#define T9MAXTAPKEYS                  T9MAXDBKEYS
#endif
#if T9MAXTAPKEYS > 16
#error Maximum value for any key is currently 16.
#endif

#ifndef T9MAXTREES
#define T9MAXTREES 5 /* Maximum number of independent trees in the LDB */
#endif

#define T9_MAXLDBWORDSIZE            32  /* Max word length supported by T9 LDB
                                           (not user adjustable) */
#define T9AWMAXLDBOBJECTS  32  /* Max # of LDB words for any key seq. */

/****************************************************************************/

/*----------------------------------------------------------------------------
 *  T9 non-flexible typedefs.
 *----------------------------------------------------------------------------*/

/* Auxilliary buffer member (key information in low nibble; status bits in
   high nibble). Variables of this type should have names starting with "a".
   This is also used for (zero-based) key indices
   and for symbol information returned from T9GetSymbolClass. */
typedef T9U8 T9AuxType;

/*----------------------------------------------------------------------------
 *  Define some basic T9 constants.
 *----------------------------------------------------------------------------*/

#ifndef T9CACHESIZE
#define T9CACHESIZE 32*(T9MAXBACKUPLANGS + 1)  /* Bytes to read ahead. */
#endif

#if !defined(T9NUMCACHE)
#define T9NUMCACHE 2
#endif

#if (T9NUMCACHE < 2)
#error T9NUMCACHE cannot be less than 2.
#endif

#if T9NUMCACHE > 2
#error T9NUMCACHE cannot be greater than 2.
#endif

/****************************************************************************/
/* Some definitions used internally across editions for generic flush function */
#define T9_BASIC_FLUSH_TYPE ((T9MINOR) 0x00)
#define T9_REMOVE_IF_PREDICTION ((T9MINOR) 0x01)

/****************************************************************************/

#define T9_GOODSETUP           0x1428  /* Indicate T9 has been setup properly    */
#define T9TXTFILLSYM    ((T9SYMB)0x20)  /* Text buffer fill value */
#define T9_KEYFILLSYM    ((T9AuxType)0x00)  /* Aux buffer fill value  */

/*----------------------------------------------------------------------------
 *  Define T9 shift values and masks for the state bits.
 *----------------------------------------------------------------------------*/

typedef enum T9STATEBIT_e {
    T9STATESHIFT,               /*  0 */
    T9STATECAPS,                /*  1 */
    T9STATEAMBIG,               /*  2 */
    T9STATEINSERT,              /*  3 */
    T9STATEMULTITAP,            /*  4 */
    T9AWSTATERESERVED5,         /*  5 */
    T9AWSTATERESERVED6,         /*  6 */
    T9STATEEXTMULTITAP,         /*  7 */
    T9AWSTATERESERVED8,         /*  8 */
    T9STATEUDBWRITEBYOEM,       /*  9 */
    T9AWSTATERESERVED10,        /* 10 */
    T9STATEUDBCOMPLETION,       /* 11 */
    T9AWSTATERESERVED12,        /* 12 */
    T9STATEUDBCALLBACK,         /* 13 */
    T9STATECHARCONVERTON,       /* 14 */
    T9STATEREORDERWORD,         /* 15 */
    T9STATELDBCOMPLETION,        /* 16 */
    T9STATEAUTOSWITCHLANG        /* 17 */
#ifdef T9CDB_IN_BUILD
    ,
    T9STATECDBWRITEBYOEM,        /* 18 */
    T9STATECDBCOMPLETION,        /* 19 */
    T9STATECDBPREDICTION        /* 20 */
#endif
} T9STATEBIT;

#define T9STATESHIFTMASK            (1L << T9STATESHIFT)
#define T9STATECAPSMASK             (1L << T9STATECAPS)
#define T9STATEAMBIGMASK            (1L << T9STATEAMBIG)
#define T9STATEINSERTMASK           (1L << T9STATEINSERT)
#define T9STATEMULTITAPMASK            (1L << T9STATEMULTITAP)
#define T9STATEEXTMULTITAPMASK         (1L << T9STATEEXTMULTITAP)
#define T9STATEUDBWRITEBYOEMMASK    (1L << T9STATEUDBWRITEBYOEM)
#define T9STATEUDBCOMPLETIONMASK    (1L << T9STATEUDBCOMPLETION)
#define T9STATEUDBCALLBACKMASK      (1L << T9STATEUDBCALLBACK)
#define T9STATECHARCONVERTONMASK    (1L << T9STATECHARCONVERTON)
#define T9STATEREORDERWORDMASK      (1L << T9STATEREORDERWORD)
#define T9STATELDBCOMPLETIONMASK    (1L << T9STATELDBCOMPLETION)
#ifdef T9CDB_IN_BUILD
#define T9STATECDBCOMPLETIONMASK    (1L << T9STATECDBCOMPLETION)
#define T9STATECDBPREDICTIONMASK    (1L << T9STATECDBPREDICTION)
#define T9STATECDBWRITEBYOEMMASK    (1L << T9STATECDBWRITEBYOEM)
#endif
#define T9STATEAUTOSWITCHLANGMASK    (1L << T9STATEAUTOSWITCHLANG)

/*----------------------------------------------------------------------------
 *  The following definitions make it easier to correctly detect the current
 *  state of the T9 system
 *----------------------------------------------------------------------------*/
#define T9AWRDBON(dwState)          ((dwState) & T9STATEREORDERWORDMASK)
#define T9EXTMULTITAPMODE(dwState)  ((dwState) & T9STATEEXTMULTITAPMASK)

#define T9MULTITAPMODE(dwState)     (((dwState) & (T9STATEAMBIGMASK | T9STATEMULTITAPMASK)) == T9STATEMULTITAPMASK)
#define T9AMBIGMODE(dwState)        (((dwState) & (T9STATEAMBIGMASK | T9STATEMULTITAPMASK)) == T9STATEAMBIGMASK)

#define T9SHIFTMODE(dwState)        ((dwState) & T9STATESHIFTMASK)
#define T9CAPSLOCKMODE(dwState)     ((dwState) & T9STATECAPSMASK)

#define T9AWAUTOSWITCHON(dwState)       ((dwState) & T9STATEAUTOSWITCHLANGMASK)

/* The system is in Insert mode while Multi-Tapping. Once a character has been accepted via time-out
 * or cursor movement, Insert mode is turned off
 */
#define T9INSERTMODE(dwState)    ((dwState & T9STATEINSERTMASK) == T9STATEINSERTMASK)

/*----------------------------------------------------------------------------
 *  Define T9 "Symbol Class", as returned by T9GetSymbolClass.
 *  It shares some bits and the Hungarian prefix code "a", but
 *  some bits are different.
 *----------------------------------------------------------------------------*/
#define T9SYMWHITE 0
#define T9SYMPUNCT 1
#define T9SYMNUMBR 2
#define T9SYMALPHA 3
/*  Define new UNKNOWN symbol class for mixed English/Chinese/Etc. text
    buffers so that T9 Latin core spacing functions can step one character
    at a time over Chinese and other foreign symbols.

    Defined as bit 7 to keep out of the way of the paAuxBuf bits merged
    into the true symbol class value returned by T9GetSymbolClass().
    Those bits are:
        T9_WORDTAG   5
        T9_SHIFTED   6
*/

#define T9SYMUNKN   7

#define T9SYMWHITEMASK ((T9AuxType)((T9AuxType)1 << T9SYMWHITE))
#define T9SYMPUNCTMASK ((T9AuxType)((T9AuxType)1 << T9SYMPUNCT))
#define T9SYMNUMBRMASK ((T9AuxType)((T9AuxType)1 << T9SYMNUMBR))
#define T9SYMALPHAMASK ((T9AuxType)((T9AuxType)1 << T9SYMALPHA))
#define T9SYMUNKNMASK  ((T9AuxType)((T9AuxType)1 << T9SYMUNKN))


/*----------------------------------------------------------------------------
 *  Define T9 user data base information structure.
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *  Define T9 valid key values for T9HandleEvent.
 *----------------------------------------------------------------------------*/

typedef enum T9KEY_e {
    T9KEYNONE,       /* 00 :                       */
    T9KEYAMBIG1,     /* 01 : 1 .-'                 */
    T9KEYAMBIG2,     /* 02 : 2 ABC                 */
    T9KEYAMBIG3,     /* 03 : 3 DEF                 */
    T9KEYAMBIG4,     /* 04 : 4 GHI                 */
    T9KEYAMBIG5,     /* 05 : 5 JKL                 */
    T9KEYAMBIG6,     /* 06 : 6 MNO                 */
    T9KEYAMBIG7,     /* 07 : 7 PQRS                */
    T9KEYAMBIG8,     /* 08 : 8 TUV                 */
    T9KEYAMBIG9,     /* 09 : 9 WXYZ                */
    T9KEYAMBIGA,     /* 10 :                       */
    T9KEYAMBIGB,     /* 11 :                       */
    T9KEYAMBIGC,     /* 12 :                       */
    T9KEYAMBIGD,     /* 13 :                       */
    T9KEYAMBIGE,     /* 14 :                       */
    T9KEYAMBIGF,     /* 15 :                       */
    T9KEYSHIFT,      /* 16 : SHIFT                 */
    T9KEYNEXT,       /* 17 : NEXT                  */
    T9KEYPREV,       /* 18 : PREVIOUS              */
    T9KEYSPACE,      /* 19 : SPACE                 */
    T9KEYCLEAR,      /* 20 : CLR                   */
    T9KEYLEFT,       /* 21 : <--                   */
    T9KEYRIGHT,      /* 22 : -->                   */
    T9KEYMENU,       /* 23 :                       */
    T9KEYOK          /* 24 :                       */
} T9KEY;

/*----------------------------------------------------------------------------
 *  Define T9 cursor adjust types for T9EVTCURSOR
 *----------------------------------------------------------------------------*/
typedef enum T9CA_e {
    T9CA_MOVELEFT,              /* 0: Move left from current position  */
    T9CA_MOVERIGHT,             /* 1: Move right from current position */
    T9CA_FROMBEG,               /* 2: Move left from start of buffer   */
    T9CA_FROMEND                /* 3: Move rigth from end of buffer    */

} T9CA;

/*----------------------------------------------------------------------------
 *  Define T9 error codes returned by T9HandleEvent.
 *----------------------------------------------------------------------------*/

typedef enum T9STATUS_e {
    T9STATNONE,                 /* 00 : No errors encountered.              */
    T9STATERROR,                /* 01 : General error status.               */
    T9STATFULL,                 /* 02 : Buffer full.                        */
    T9STATEMPTY,                /* 03 : Buffer empty.                       */
    T9STATBEGIN,                /* 04 : Begin of buffer .                   */
    T9STATEND,                  /* 05 : End of buffer.                      */
    T9STATMAXWRD,               /* 06 : Maximum word size.                  */
    T9STATNOWRD,                /* 07 : No word match found.                */
    T9STATABORT,                /* 08 : Abort T9 system.                    */
    T9STATNOINIT,               /* 09 : T9 call made before lang setup      */
    T9STATMORE,                 /* 10 : Tegic internal status code          */
    T9STATREADLDBFAIL,          /* 11 : Unable to read LDB                  */
    T9STATLDBVERSIONERR,        /* 12 : LDB Version mismatch                */
    T9STATMAXDBKEYSERR,         /* 13 : Max DB keys exceeded                */
    T9STATLDBPRIMARYIDERR,      /* 14 : Mismatched primary language ID      */
    T9STATLDBSECONDARYIDERR,    /* 15 : Mismatched secondary language ID    */
    T9STATINVALIDLDB,           /* 16 : Invalid/Obsolete LDB layout         */
    T9STAT_LDB_TYPE_ERR,        /* 17 : Wrong LDB type                      */
    T9STAT_MAX_SYMS_ERR,        /* 18 : Wrong max syms per key value        */
    T9STAT_19,                  /* 19 : Reserved                            */
    T9STAT_SYM_SIZE_ERR,        /* 20 : Mismatched Core and LDB Sym size    */
    T9STAT_SYM_CLASS_ERR,       /* 21 : Wrong symbol class value            */
    T9STAT_22,                  /* 22 : Reserved                            */
    T9STAT_23,                  /* 23 : Reserved                            */
    T9STAT_24,                  /* 24 : Reserved                            */
    T9STAT_SPTABLESIZE_ERR,     /* 25 : Smart punct table size is too big   */
    /* Error codes returned in response to API calls.                       */
    T9STAT_INVALID_SYMBOL,      /* 26 : bad symbol value for char           */
    T9STATNOLANG,               /* 27 : T9 has not been set to a valid lang.*/
    T9STAT_28,                  /* 28 : Reserved                            */
    T9STAT_INVALID_STATE,       /* 29 : the API call is invalid at this time*/
    T9STAT_INVALID_TEXT,        /* 30 : text in psTxtBuf is improper        */
    T9STAT_31,                  /* 31 : Reserved                            */
    T9STAT_INVALID_EVENT,       /* 32 : api event not supported/recognized  */
    T9STAT_BAD_PARAM,           /* 33 : invalid parameter for api call      */
    T9STAT_NO_BUFFER,           /* 34 : buffer has not been provided        */
    T9STAT_35,                  /* 35 : Reserved                            */
    T9STAT_NO_UDB,              /* 36 : missing user database               */
    T9STAT_37,                  /* 37 : Reserved                            */
    T9STAT_INVALID_SIZE,        /* 38 : Invalid UDB/CUDB/CAUDB data size    */
    /* Error codes returned for the complete compatibility ID check.        */
    T9STAT_DB_CORE_INCOMP,      /* 39 : The loaded database is not          */
                                /*      compatible with the core.           */
    T9STAT_WRONG_OEMID,         /* 40 : Wrong OEM ID.                       */
    T9STAT_CORRUPT_LDB,         /* 41 : LDB data is corrupted.              */
    T9STAT_MT_STK_INCOMP,       /* 42 : Characters in the multitap table are*/
                                /*      not included in the SymToKey table. */
                                /*      OR Some/one of the collating        */
                                /*      sequences in the LDB is not         */
                                /*      supported by the core.              */
    /* Status codes returned for the T9EVTMDBWORDCHECK event                */
    T9STAT_MDB_MATCH,           /* 43 : The word matches to the keystrokes. */
    T9STAT_MDB_NO_MATCH,        /* 44 : The word doesn't match to the       */
                                /*      keystrokes.                         */
    T9STAT_MATCH_FOUND,         /* 45 : A matching character/word was found */
    T9STAT_NO_CHAR,             /* 46 : Character not found(not displayable)*/
    T9STAT_47,                  /* 47 : Reserved                            */
    T9STAT_48,                  /* 48 : Reserved                            */
    T9STAT_NODATAMODULE,        /* 49 : the API is not supported due to the */
                                /*      absence of a data module in current */
                                /*      LDB                                 */
    T9STAT_UNSUPPORT_LANG,      /* 50 : When Chinese, Japanese or Korean    */
                                /*      core is not compiled in, error on   */
                                /*      trying to init them. This could     */
                                /*      happen when the language number in  */
                                /*      the fieldinfo is set wrong.         */
    T9STAT_MODULEFLAG_ERR,      /* 51 : Error on module flag                */
    T9STAT_NODISPLAYCODE,       /* 52 : Display code module is missing      */
    T9STAT_STROKEDATA_ERR,      /* 53 : Stroke data is invalid.             */
    T9STAT_CONTEXTDATA_ERR,     /* 54 : Context data is invalid.            */
    T9STAT_PHONETICDATA_ERR,    /* 55 : Phonetic data is invalid.           */
    T9STAT_COMPONENTDATA_ERR,   /* 56 : Component data is invalid.          */
    T9STAT_WORDNOTFOUND,        /* 57 : Specified word is not found.        */
    T9STATLOADLDBFAIL,          /* 58 : Unable to load LDB successfully,    */
                                /*      specific to Chinese LDB loading.    */
    T9STATINVALIDKEY,           /* 59 : Invalid key value                   */
    T9STATINVALIDCTRL,          /* 60 : Invalid control value - T9EVTCTRL   */
    T9STATINVALIDUDBCTRL,       /* 61 : Invalid UDB control value - T9EVTUDB*/
    T9STATINVALIDMDBCTRL,       /* 62 : Invalid MDB control value - T9EVTMDB*/
    T9STATINVALIDCAUDBCTRL,     /* 63 : Invalid CAUDB ctrl value - T9EVTCAUDB */
    T9STATINVALIDINDEX,         /* 64 : index parameter is invalid For JPN */
#ifdef T9CDB_IN_BUILD
    T9STAT_NO_CDB,              /* 65 : missing context database            */
    T9STATINVALIDCDBCTRL,       /* 66 : Invalid CDB control value - T9EVTCDB*/
#endif

/** PTI-specific status codes. */
#if (!defined(T9CP_DISABLE_PHONETIC) || !defined(T9CP_DISABLE_STROKE))
    T9CPSTAT_LDB_NOTSUPPORT = 100,    /* 100: LDB is not supported in the Chinese PTI */
    T9CPSTAT_LDB_SEARCH_ERR,          /* 101: general database search error */
    T9CPSTAT_NOT_LONG_ENOUGH,         /* 102: the buffer not long enough */
    T9CPSTAT_NOMORE,                  /* 103: no more entry */
    T9CPSTAT_SPELL_NOTFOUND,          /* 104: specified spelling not found */
    T9CPSTAT_PHRASE_NOTFOUND,         /* 105: specified phrase not found */
    T9CPSTAT_reserve_106,             /* 106: reserved */
    T9CPSTAT_LDB_SPELL_RANGE_ERR,     /* 107: spelling is out of range during the LDB search */
    T9CPSTAT_NOTONE,                  /* 108: no valid tones */
    T9CPSTAT_INVALID_INPUT,           /* 109: invalid tone or delimiter or key input */
    T9CPSTAT_INVALID_APICALL,         /* 110: invalid API call in the current state */
    T9CPSTAT_KEYBUF_EMPTY,            /* 111: key buffer is empty */
    T9CPSTAT_reserve_112,             /* 112: reserved */
    T9CPSTAT_MODE_UNAVAILABLE,        /* 113: the specified mode is not available */
    T9CPSTAT_SELECTED_COMPONENT,      /* 114: the selected item is a component */
    T9CPSTAT_UDB_LENGTH_OVER_LIMIT,   /* 115: UDB phrase length is over the limit */
    T9CPSTAT_RESETKEYS,               /* 116: Caller must reset all keys (then add the keys back one by one if desired) */
#endif

    T9STATLAST = 255                  /* This is just here to have an entry which ends without a comma */
} T9STATUS;

/*----------------------------------------------------------------------------
 *  Define T9 request type handled by T9HandleRequest.
 *----------------------------------------------------------------------------*/

typedef enum T9REQ_e {
    T9REQNONE,           /* 0: nop                                            */
    T9REQTIMEOUT,        /* 1: Request a timer be set.                        */
    T9REQMULTITAPSYM,    /* 2: Request for first/next symbol in mtap seq for  */
                         /*    key                                            */
    T9REQ3,              /* 3  Reserved                                       */
    T9REQ4,              /* 4: Reserved                                       */
    T9REQUDBWRITE,       /* 5: Request OEM write to UDB memory                */
    T9REQDYNAMICCONTEXT, /* 6: Chinese dynamic context - Get pointer to       */
                         /*    external context string                        */
    T9REQ7,              /* 7: Reserved                                       */
    T9REQRELOADCAUDB,    /* 8: reload the t9 caudb according to the different */
                         /*    Chinese languages                              */
    T9REQ9,              /* 9: Reserved                                       */
    T9REQ10,             /* 10: Reserved                                      */
    T9REQUDBWORDENTRY    /* 11: Request to see whethter or not UDB is enabled */
#ifdef T9CDB_IN_BUILD
    ,
    T9REQCDBWRITE       /* 12: Request OEM write to CDB memory                */
#endif

} T9REQ;


 /*----------------------------------------------------------------------------
 *  Define T9 timer types handled by T9HandleRequest.
 *----------------------------------------------------------------------------*/
typedef enum T9TMR_e {
    T9TMRNONE, /* 0: Undefined.                                             */
    T9TMRMULT  /* 1: MultiTap timer.                                        */

} T9TMR;


/*----------------------------------------------------------------------------
 *  Define T9 structures used in calls to T9HandleRequest.
 *----------------------------------------------------------------------------*/

typedef struct T9ReqSetTimeout_s {

    T9TMR eTimerType;    /* Type of timer requested.                        */
    T9MINOR mTimerID;    /* Currently only one timer is active at a time.   */

} T9ReqSetTimeout;

typedef struct T9ReqGetSymbol_s {

    T9KEY       eKeyId;    /* Key being multitapped                           */
    T9SYMB      sSymbol;   /* INPUT: 0 if first call for key being multitapped*/
                           /* INPUT: if subsequent call for same key, symbol  */
                           /*        provided by previous call                */
                           /* OUTPUT: next/first sym for key in MTap sequence */
} T9ReqGetSymbol;

typedef struct T9ReqUDBDataWrite_s {

    void    T9FARDATA*  pTo;          /* pointer within local UDB memory where to write  */
    const   void  T9FARDATA*  pFrom;  /* pointer to data to be placed in UDB memory      */
    T9UINT   nSize;                  /* number of bytes to write                        */

} T9ReqUDBDataWrite;

#ifdef T9CDB_IN_BUILD
typedef struct T9ReqCDBDataWrite_s {

    void    T9FARDATA*  pTo;          /* pointer within local CDB memory where to write  */
    const   void  T9FARDATA*  pFrom;  /* pointer to data to be placed in CDB memory      */
    T9UINT   nSize;                  /* number of bytes to write                        */

} T9ReqCDBDataWrite;
#endif


typedef struct T9ReqUDBWordEntry_s {

    T9SYMB    *psUdbCandidateWord;     /* pointer to buffer holding         */
                                       /* candidate for entry into the udb. */
    T9UINT     nUdbCandidateWordLen;    /* Length of psUdbCandidateWord      */
} T9ReqUDBWordEntry;


typedef struct T9ReqGetChineseDynamicContextString_s {

    T9UINT  nStringNum;   /* Context String to retrieve                        */
                         /* 1 - n = Get 1 - nth string                        */
                         /* 0 = Get next                                      */
    T9SYMB  *psString;   /* OUTPUT: Pointer to the requested dynamic context  */
                         /* string.-  0 if no more entries or invalid number  */
    T9U8    bStrLen;     /* Length of string                                  */

} T9CCReqExtContextString;

typedef struct T9Request_s {

    T9REQ eType;            /* T9RequestType                                     */

    union {
        T9U32               ulReturn; /* any return request value that can be
                                       * fitted within 32-bit quantity           */
        T9ReqSetTimeout     sTimeout;
        T9ReqGetSymbol      sMultitapSymbol;
        T9ReqUDBDataWrite   sUDBWrite;
#ifdef T9CDB_IN_BUILD
        T9ReqCDBDataWrite   sCDBWrite;
#endif
        T9CCReqExtContextString    sChineseDynamicContext;
        T9ReqUDBWordEntry   sUDBCandidate;

    } data;

} T9Request;


/*============================================================================
 * NOTE:
 *   The following definitions are private to AOL Mobile and are subject
 *   to change without notice.
 *============================================================================*/

/*----------------------------------------------------------------------------
 *  Synopsis: Define T9 key buffer information mask bits.
 *----------------------------------------------------------------------------*/

#define T9_LOCKFLAG  4 /* Indicates next-locking point      */
#define T9_WORDTAG   5 /* Word tag indicates end of ambiguous word.            */
#define T9_SHIFTED   6 /* Indicates shift state for this key.                  */
#define T9_EXPLICIT  7 /* This key was input in an explicit mode */

#define T9_LOCKFLAGMASK    ((T9AuxType)(1 << T9_LOCKFLAG))
#define T9_WORDTAGMASK     ((T9AuxType)(1 << T9_WORDTAG))
#define T9_SHIFTEDMASK     ((T9AuxType)(1 << T9_SHIFTED))
#define T9_EXPLICITMASK    (1 << T9_EXPLICIT)

#define T9_AMBIGKEYMASK ((T9AuxType) 0xF)
#define T9AUXKEY(c)      (T9U8)((c) & T9_AMBIGKEYMASK)
#define T9AUXKEYNUM(aAux)   ((T9KEY) (T9AUXKEY(aAux) + 1))

typedef struct T9FieldInfo_s T9FieldInfo;

typedef T9STATUS (T9FARCALL *T9_NullFunc_f)(T9FieldInfo*);
typedef T9STATUS (T9FARCALL *T9_CharFunc_f)(T9FieldInfo*, T9SYMB);
typedef T9STATUS (T9FARCALL *T9_CharToKeyFunc_f)(T9FieldInfo*, T9SYMB, T9AuxType *);
typedef T9STATUS (T9FARCALL *T9_MinorFunc_f)(T9FieldInfo*, T9MINOR);
typedef T9STATUS (T9FARCALL *T9_KeyFunc_f)(T9FieldInfo*, T9KEY);
typedef T9STATUS (T9FARCALL *T9_StrFunc_f)(T9FieldInfo*, T9U8 *);
typedef T9STATUS (T9FARCALL *T9_GetCodeVersion_f)(T9U8 *, T9U16, T9U16 *);

/* For each T9 data cache.  Setting dwNibbleCount to zero invalidates a cache. */
typedef struct T9CacheInfo_s {
    /* LDB cache. */
    T9U8 T9FARDATA *pbPointer; /* Pointer to read position in cache. */
    T9U32 dwStartOffset;      /* Nibble offset of the start of the cache. */
    T9U32 dwNibbleCount;     /* Count of nibbles the cache. */
    T9U32 dwNibblePos;      /* Nibble read position in the cache. */
} T9CacheInfo;

/* Define structure for T9 Private persistent data */
struct T9_PrivateNotLdb_s {
    T9UINT   nTotObjCnt;     /* O   Count of total number of objects for current key seq. */
    T9UINT nObjNum;
#ifdef T9CDB_IN_BUILD
    T9U8  bMovingCursor;
#endif
    T9U16 wInitOK;
    T9KEY eLastKey;

    /* Functions ptrs. to allow generic functions. */
    T9_NullFunc_f pTermFunc;        /* Called when an edition is exited */
    T9_NullFunc_f pTimeOutFunc;     /* Called when a timer expires. */
    T9_CharFunc_f pAddExplicitFunc; /* Called when an explicit char is added */
    T9_CharToKeyFunc_f pGetKeyForSymFunc; /* Called when an explicit char is added */
    T9_KeyFunc_f pAddKeyFunc;       /* Called on a key press */
    T9_StrFunc_f pGetLdbVersionFunc; /* Called for T9GetLdbVersion(). */
    T9_GetCodeVersion_f pGetCodeVersionFunc; /* Called for magic string. */
    T9_NullFunc_f pValidateLdbFunc;  /* Called for T9ValidateLdb(). */
    T9_MinorFunc_f pFlushFunc;       /* Called when any active word is flushed */
    T9_NullFunc_f pFreshStartFunc;  /* Called when restarting w/ no keys */
    T9_NullFunc_f pForgetFunc;      /* Called to forget build info (but keep word active) */
#ifdef T9CDB_IN_BUILD
    T9_NullFunc_f pBreakContextFunc;  /* Called for Breaking Context. */
#endif

};  /* end of struct T9_PrivateNotLdb_s */

struct T9_PrivateLdb_s {
    T9UINT   nTotObjCntLang;    /* O   Count of total number of objects for current key seq. */
    T9UINT   nTotObjInFence;    /* O   Count of total number of objects built inside lang fence. */
    T9U32   dwObjsFromRdbMask;  /* O   a bitmask of which ldb objects have already been retrieved from rdb. */
    T9U32   dwHeaderStart;      /* the starting address of the ldb header.  Should always be set to 0 except in PTI */
    T9U8 bNumValidKeysExplNibbles;
    T9U8 bHasEOW;           /* O   Is end-of-word information encoded in the LDB? */
    T9U8 bHasShift;         /* O   Is alternate case information encoded in the LDB? */
    T9U8 bNumInitPunct;
    T9U8 bNTrees;
    T9U8 bNumContextKeys;
    T9U8 bNSymToKey;
    T9U8 pbNReserved[T9MAXTREES];
    T9U8 sCmapKeyPos[T9MAXDBKEYS + 1];
    T9U8 sCtapKeyLowerPos[T9MAXTAPKEYS + 1];
    T9U8 sCtapKeyUpperPos[T9MAXTAPKEYS + 1];

    T9U16 wLangOK;
    T9U16 wCharMapLowerOffset;
    T9U16 wCharMapUpperOffset;
    T9U16 wMultitapMapLowerOffset;
    T9U16 wMultitapMapUpperOffset;
    T9U16 wCharOnKeyAddr;
    T9U16 wValidKeysAddr;
    T9U16 wInstrAbbrevTableAddr;
    T9U16 wShiftTableAddr;
    T9U16 wSymToKeyTablesAddr;
    T9U16 wValidKeysExplicitIndex;
    T9U16 wInstructExplicitRelOrder;
    T9U16 wSmartPunctMapOffset;
    T9U16 wSmartPunctKeys;
    T9U16 wNumValidKeysNibbles[3];
    T9U16 wNumInstructionNibbles[3];
    T9U16 pwNumSizeNibbles[T9MAXTREES * T9_NUMSIZECLASSNIBBLERANGES];
    T9U16 pwNumInSizeNibbles[T9MAXTREES * T9_NUMSIZECLASSNIBBLERANGES];
    T9U16 pwReserveTableAddr[T9MAXTREES];
    T9U16 pwSizeClassTableAddr[T9MAXTREES];

    T9U32 pdwTrRoots[T9MAXTREES];

    struct {
        T9SYMB sStartSym;
        T9U8 bNSym;
        T9U8 bSCStart;
    } pSymToKey[T9_MAXSYMTOKEYRANGES];

    T9CacheInfo CacheInfo[T9NUMCACHE]; /* Info for all caches. */
};  /* end of struct T9_PrivateLdb_s */

/*============================================================================
 * END OF PRIVATE:
 *============================================================================*/

/*----------------------------------------------------------------------------
 *  Language data base symbol class enumeration
 *----------------------------------------------------------------------------*/
typedef enum T9SymbolClass_e {
    T9SCLatin1        =1,  /* ISO-8859-1 (Latin1) */
    T9SCLatin2        =2,  /* ISO-8859-2 (East & Central Europe) */
    T9SCLatin3        =3,  /* ISO-8859-3 (Maltese, Turkish, ...) */
    T9SCLatin4        =4,  /* ISO-8859-4 (Baltic) */
    T9SCCyrillic      =5,  /* ISO-8859-5 */
    T9SCArabic        =6,  /* ISO-8859-6 */
    T9SCModernGreek  =7,  /* ISO-8859-7 */
    T9SCHebrew        =8,  /* ISO-8859-8 */
    T9SCLatin5        =9,  /* ISO-8859-9 (Latin1 w/ Turkish) */
    T9SCLatin6        =10, /* ISO-8859-10 (Nordic (with indigenous)) */
    T9SCXX11              =11, /* */
    T9SCXX12              =12, /* */
    T9SCXX13              =13, /* */
    T9SCXX14              =14, /* */
    T9SCUnicode           =15, /* UCS-2 or UCS-4 (based on sb field) */
    T9SCXX16              =16, /* */
    T9SCXX17              =17, /* */
    T9SCXX18              =18, /* */
    T9SCJapaneseJISH    =19, /* JIS, Hiragana */
    T9SCJapaneseJISK    =20, /* JIS, Katakana */
    T9SCJapaneseSJISH   =21, /* Shift JIS, Hiragana */
    T9SCJapaneseSJISK   =22, /* Shift JIS, Katakana */
    T9SCXX23              =23, /* */
    T9SCXX24              =24, /* */
    T9SCXX25              =25, /* */
    T9SCXX26              =26, /* */
    T9SCXX27              =27, /* */
    T9SCXX28              =28, /* */
    T9SCXX29              =29, /* */
    T9SCXX30              =30, /* */
    T9SCXX31              =31, /* */
    T9SCXX32              =32, /* */
    T9SCXX33              =33, /* */
    T9SCXX34              =34, /* */
    T9SCXX35              =35, /* */
    T9SCXX36              =36, /* */
    T9SCGSM               =37, /* (7-bit) */
    T9SCArabicWindows    =56, /* Windows code page 1256. */
    T9SCISCII             =70,
    T9SCThaiWindows      =74, /* Windows code page 1274. */
    T9SCBOUNDS
} T9SymbolClass;

/* Call-back functions (for T9 to call the OEM) */
typedef T9STATUS (T9FARCALL *T9HandleRequest_f)(T9FieldInfo*, T9Request*);
typedef T9U32    (T9FARCALL *T9ReadLdbData_f)(T9FieldInfo *pFieldInfo, T9U32 dwOffset, T9FARPOINTER *ppbDst, T9U32 *pdwStart, T9UINT nSize, T9MINOR mCacheIndex);
typedef T9STATUS (T9FARCALL *T9ConvertChar_f)(T9FieldInfo*, T9SYMB*);
typedef T9STATUS (T9FARCALL *T9ReadTUdb_f)(T9FARPOINTER pData, T9U16 wSize, T9FARPOINTER pTUdb, T9U32 dwOffset);
typedef T9STATUS (T9FARCALL *T9WriteTUdb_f)(T9FARPOINTER pData, T9U16 wSize, T9FARPOINTER pTUdb, T9U32 dwOffset);

/* Event-mode compatibility. */
#ifdef T9EVENTAPI
typedef union {
  struct T9CCFieldInfo_s *pAuxChinese;   /* Aux API struct for Chinese */
  struct T9JWFieldInfo_s *pAuxJapanese;  /* Aux API struct for Japanese */
} T9_upAuxAPI;
#endif

/*----------------------------------------------------------------------------
 *  Define generic T9 field information structure.
 *---------------------------------------------------------------------------*/
typedef struct {
    T9U8    bNumDbKeys;     /* Number of ambiguous keys LDB specifies */
    T9U8    bNumTapKeys;    /* Number of multitap keys LDB specifies */
    T9UINT  nLdbNum;        /* Language number for this LDB. */
} T9LdbInfo;  /* end of struct T9LdbInfo  */

struct T9FieldInfo_s {
    T9SYMB *psTxtBuf;       /* Shared text buffer.                       */
    T9AuxType *paAuxBuf;    /* Shared auxillary buffer.                   */
    T9UINT   nBufLenMax;    /* Memory Size of text and key buffers.      */
    T9UINT   nBufLen;       /* Number characters in text and key buffers.*/
    T9UINT   nCursor;       /* Current cursor position.                  */
    T9UINT   nWordLen;      /* Length of active keystrokes.   */
    T9UINT   nComplLen;     /* Length of word beyond active keystrokes.   */
    T9UINT   nLdbNum;       /* LDB number for active language
                               (i.e. pLdb[bActiveLangIndex].nLdbNum). */
    T9U8     bSymbolClass;  /* Symbol class (see T9SymbolClass above). */
                           
    T9UINT   nCurSelObj;    /*
                            * The current selected object in the selection list
                            * when nexting/pre-ing. The valid value
                            * is [1..T9AWMAXSELECTIONOBJECT]
                            */

    T9U32   dwStateBits;    /* Current State bits                     */
                            /* See T9STATEBIT above for legal values. */

    /* For multilingual implementations. */
    T9U8    bNumSupplLangs;   /* Number of supplemental languages (0 by default).   */
    T9U8    bActiveLangIndex; /* Language index for the word currently being built. */
    T9U8    bAutoSwitchLangIndex; /* Which language has been switched to first? */

    T9LdbInfo  pLdb[1 + T9MAXBACKUPLANGS]; /* Public ldb data - 1 for each ldb */

    /* Call-back functions (for T9 to call the OEM) */
    T9HandleRequest_f T9HandleRequest;
    T9ReadLdbData_f T9ReadLdbData;
    T9ConvertChar_f T9ConvertChar;

    /* OEM-controlled fields */
    void   *pOEMPrivate;   /* A general pointer. */
    T9U32   dwReserved;    /* General other info (maybe a thread ID?) */
    T9U8    pbCache[T9CACHESIZE]; /* Space for OEM to use as desired, esp. */
                           /* for LDB read cache. */

    /* Private T9 internals. */
#ifdef T9EVENTAPI
    T9_upAuxAPI uLangData;   /* union of ptrs referencing LDB specific APIs   */
#endif
    struct T9_PrivateNotLdb_s  T9Private;   /* Private T9 data.    */
    struct T9_PrivateLdb_s  T9PrivateLdb[1 + T9MAXBACKUPLANGS]; /* Private per-ldb data */
};


/*----------------------------------------------------------------------------
 The following constants are needed for generic fetching of LDB ID.
 *----------------------------------------------------------------------------*/
#define T9CC_HeaderOffsetLanguageID 36

/*----------------------------------------------------------------------------
 *  Define the minimum number bytes of LDB data that the integration layer
 *  must pass to T9 when calling T9CTRLGETLANGID to get the language ID in the
 *  LDB.  Because the lang ID in Chinese LDB is in a different offset
 *  (T9CC_HeaderOffsetLanguageID) than that of the rest LDBs (T9LANGUAGEID_OFFSET),
 *  T9MINLDBBUFSIZE shall be the bigger value of the two lang ID offsets.
 *----------------------------------------------------------------------------*/
#define T9MINLDBBUFSIZE (0x36 + 2)

/*----------------------------------------------------------------------------
 *  Define T9 collating sequences and bit masks.
 *  Note that these numbers can't be rearranged witout rebuilding all the
 *  databases which use them.
 *----------------------------------------------------------------------------*/
typedef enum T9CollatingSequences_e {
    T9_CSBITGSM                 = 0,
    T9_CSBITLatin1              = 1,
    T9_CSBITLatinA              = 2,
    T9_CSBITLatinB              = 3,
    T9_CSBITDiacritics          = 4,
    T9_CSBITGreek               = 5,
    T9_CSBITGreekAlphabetic     = 6,
    T9_CSBITCyrillic            = 7,
    T9_CSBITCyrillicEven        = 8,
    T9_CSBITHebrew              = 9,
    T9_CSBITArabic              = 10,
    T9_CSBITArabicAlternate     = 11,
    T9_CSBITThai                = 12,
    T9_CSBITThaiHutchison       = 13,
    T9_CSBITKoreanChun          = 14,
    T9_CSBITVietnameseWithTone  = 15,
    T9_CSBITGeneralPunctuation  = 16,
    T9_CSBITCurrencySymbols     = 17,
    T9_CSBITThaiAIS             = 18,
    T9_CSBITDevanagari          = 19,
    T9_CSBITDevanagariAlternate = 20,
    T9_CSBITThaiAlternate       = 21,
    T9_CSBITBengali             = 22,
    T9_CSBITBengaliAlternate    = 23,
    T9_CSBITGurmukhi            = 24,
    T9_CSBITGurmukhiAlternate   = 25,
    T9_CSBITTamil               = 26,
    T9_CSBITTamilAlternate      = 27,
    T9_CSBITGujarati            = 28,
    T9_CSBITGujaratiAlternate   = 29,
    T9_CSBITArabicAlt3          = 30,
    T9_CSBITHebrewAlt3          = 31
} T9CollatingSequences;


#define T9_CSMASKGSM                     (1L << T9_CSBITGSM)
#define T9_CSMASKLatin1              (1L << T9_CSBITLatin1)
#define T9_CSMASKLatinA              (1L << T9_CSBITLatinA)
#define T9_CSMASKLatinB              (1L << T9_CSBITLatinB)
#define T9_CSMASKDiacritics          (1L << T9_CSBITDiacritics)
#define T9_CSMASKGreek                   (1L << T9_CSBITGreek)
#define T9_CSMASKGreekAlphabetic         (1L << T9_CSBITGreekAlphabetic)
#define T9_CSMASKCyrillic                (1L << T9_CSBITCyrillic)
#define T9_CSMASKCyrillicEven            (1L << T9_CSBITCyrillicEven)
#define T9_CSMASKHebrew              (1L << T9_CSBITHebrew)
#define T9_CSMASKHebrewAlt3              (1L << T9_CSBITHebrewAlt3)
#define T9_CSMASKArabic              (1L << T9_CSBITArabic)
#define T9_CSMASKArabicAlternate         (1L << T9_CSBITArabicAlternate)
#define T9_CSMASKArabicAlt3         (1L << T9_CSBITArabicAlt3)
#define T9_CSMASKDevanagari          (1L << T9_CSBITDevanagari)
#define T9_CSMASKDevanagariAlternate     (1L << T9_CSBITDevanagariAlternate)
#define T9_CSMASKBengali          (1L << T9_CSBITBengali)
#define T9_CSMASKBengaliAlternate     (1L << T9_CSBITBengaliAlternate)
#define T9_CSMASKTamil          (1L << T9_CSBITTamil)
#define T9_CSMASKTamilAlternate     (1L << T9_CSBITTamilAlternate)
#define T9_CSMASKThai                    (1L << T9_CSBITThai)
#define T9_CSMASKThaiHutchison       (1L << T9_CSBITThaiHutchison)
#define T9_CSMASKKoreanChun          (1L << T9_CSBITKoreanChun)
#define T9_CSMASKVietnameseWithTone  (1L << T9_CSBITVietnameseWithTone)
#define T9_CSMASKGeneralPunctuation  (1L << T9_CSBITGeneralPunctuation)
#define T9_CSMASKCurrencySymbols         (1L << T9_CSBITCurrencySymbols)
#define T9_CSMASKThaiAIS                 (1L << T9_CSBITThaiAIS)
#define T9_CSMASKThaiAlternate         (1L << T9_CSBITThaiAlternate)


/*----------------------------------------------------------------------------
 *  Primary language data base indentifier enumeration
 *----------------------------------------------------------------------------*/
#define T9PIDMASK         ((T9UINT)0x00FF)

#define T9PIDNone         ((T9UINT)0x0000)
#define T9PIDArabic       ((T9UINT)0x0001)
#define T9PIDBulgarian    ((T9UINT)0x0002)
#define T9PIDCatalan      ((T9UINT)0x0003)
#define T9PIDChinese      ((T9UINT)0x0004)
#define T9PIDCzech        ((T9UINT)0x0005)
#define T9PIDDanish       ((T9UINT)0x0006)
#define T9PIDGerman       ((T9UINT)0x0007)
#define T9PIDGreek        ((T9UINT)0x0008)
#define T9PIDEnglish      ((T9UINT)0x0009)
#define T9PIDSpanish      ((T9UINT)0x000A)
#define T9PIDFinnish      ((T9UINT)0x000B)
#define T9PIDFrench       ((T9UINT)0x000C)
#define T9PIDHebrew       ((T9UINT)0x000D)
#define T9PIDHungarian    ((T9UINT)0x000E)
#define T9PIDIcelandic    ((T9UINT)0x000F)
#define T9PIDItalian      ((T9UINT)0x0010)
#define T9PIDJapanese     ((T9UINT)0x0011)
#define T9PIDKorean       ((T9UINT)0x0012)
#define T9PIDDutch        ((T9UINT)0x0013)
#define T9PIDNorwegian    ((T9UINT)0x0014)
#define T9PIDPolish       ((T9UINT)0x0015)
#define T9PIDPortuguese   ((T9UINT)0x0016)
#define T9PIDRhaetoRomance ((T9UINT)0x0017)
#define T9PIDRomanian     ((T9UINT)0x0018)
#define T9PIDRussian      ((T9UINT)0x0019)
#define T9PIDSerboCroatian ((T9UINT)0x001A)
#define T9PIDSlovak       ((T9UINT)0x001B)
#define T9PIDAlbanian     ((T9UINT)0x001C)
#define T9PIDSwedish      ((T9UINT)0x001D)
#define T9PIDThai         ((T9UINT)0x001E)
#define T9PIDTurkish      ((T9UINT)0x001F)
#define T9PIDUrdu         ((T9UINT)0x0020)
#define T9PIDIndonesian   ((T9UINT)0x0021)
#define T9PIDUkrainian    ((T9UINT)0x0022)
#define T9PIDByelorussian ((T9UINT)0x0023)
#define T9PIDSlovenian    ((T9UINT)0x0024)
#define T9PIDEstonian     ((T9UINT)0x0025)
#define T9PIDLatvian      ((T9UINT)0x0026)
#define T9PIDLithuanian   ((T9UINT)0x0027)
#define T9PIDMaori        ((T9UINT)0x0028)
#define T9PIDFarsi        ((T9UINT)0x0029)
#define T9PIDVietnamese   ((T9UINT)0x002A)
#define T9PIDLaotian      ((T9UINT)0x002B)
#define T9PIDKampuchean   ((T9UINT)0x002C)
#define T9PIDBasque       ((T9UINT)0x002D)
#define T9PIDSorbian      ((T9UINT)0x002E)
#define T9PIDMacedonian   ((T9UINT)0x002F)
#define T9PIDSutu         ((T9UINT)0x0030)
#define T9PIDTsonga       ((T9UINT)0x0031)
#define T9PIDTswana       ((T9UINT)0x0032)
#define T9PIDVenda        ((T9UINT)0x0033)
#define T9PIDXhosa        ((T9UINT)0x0034)
#define T9PIDZulu         ((T9UINT)0x0035)
#define T9PIDAfrikaans    ((T9UINT)0x0036)
#define T9PIDFaeroese     ((T9UINT)0x0038)
#define T9PIDHindi        ((T9UINT)0x0039)
#define T9PIDMaltese      ((T9UINT)0x003A)
#define T9PIDSami         ((T9UINT)0x003B)
#define T9PIDScotsGaelic  ((T9UINT)0x003C)
#define T9PIDMalay        ((T9UINT)0x003E)
#define T9PIDSwahili      ((T9UINT)0x0041)
#define T9PIDAfar         ((T9UINT)0x0042)
#define T9PIDAbkhazian    ((T9UINT)0x0043)
#define T9PIDAmharic      ((T9UINT)0x0044)
#define T9PIDAssamese     ((T9UINT)0x0045)
#define T9PIDAymara       ((T9UINT)0x0046)
#define T9PIDAzerbaijani  ((T9UINT)0x0047)
#define T9PIDBashkir      ((T9UINT)0x0048)
#define T9PIDBihari       ((T9UINT)0x0049)
#define T9PIDBislama      ((T9UINT)0x004a)
#define T9PIDBengali      ((T9UINT)0x004b)
#define T9PIDTibetan      ((T9UINT)0x004c)
#define T9PIDBreton       ((T9UINT)0x004d)
#define T9PIDCorsican     ((T9UINT)0x004e)
#define T9PIDWelsh        ((T9UINT)0x004f)
#define T9PIDBhutani      ((T9UINT)0x0050)
#define T9PIDEsperanto    ((T9UINT)0x0051)
#define T9PIDFiji         ((T9UINT)0x0052)
#define T9PIDFrisian      ((T9UINT)0x0053)
#define T9PIDIrish        ((T9UINT)0x0054)
#define T9PIDGalician     ((T9UINT)0x0055)
#define T9PIDGuarani      ((T9UINT)0x0056)
#define T9PIDGujarati     ((T9UINT)0x0057)
#define T9PIDHausa        ((T9UINT)0x0058)
#define T9PIDCroatian     ((T9UINT)0x0059)
#define T9PIDArmenian     ((T9UINT)0x005a)
#define T9PIDInterlingua  ((T9UINT)0x005b)
#define T9PIDInterlingue  ((T9UINT)0x005c)
#define T9PIDInupiak      ((T9UINT)0x005d)
#define T9PIDInuktitut    ((T9UINT)0x005e)
#define T9PIDJavanese     ((T9UINT)0x005f)
#define T9PIDGeorgian     ((T9UINT)0x0060)
#define T9PIDKazakh       ((T9UINT)0x0061)
#define T9PIDGreenlandic  ((T9UINT)0x0062)
#define T9PIDKannada      ((T9UINT)0x0063)
#define T9PIDKashmiri     ((T9UINT)0x0064)
#define T9PIDKurdish      ((T9UINT)0x0065)
#define T9PIDKirghiz      ((T9UINT)0x0066)
#define T9PIDLatin        ((T9UINT)0x0067)
#define T9PIDLingala      ((T9UINT)0x0068)
#define T9PIDMalagasy     ((T9UINT)0x0069)
#define T9PIDMalayalam    ((T9UINT)0x006a)
#define T9PIDMongolian    ((T9UINT)0x006b)
#define T9PIDMoldavian    ((T9UINT)0x006c)
#define T9PIDMarathi      ((T9UINT)0x006d)
#define T9PIDBurmese      ((T9UINT)0x006e)
#define T9PIDNauru        ((T9UINT)0x006f)
#define T9PIDNepali       ((T9UINT)0x0070)
#define T9PIDOccitan      ((T9UINT)0x0071)
#define T9PIDOromo        ((T9UINT)0x0072)
#define T9PIDOriya        ((T9UINT)0x0073)
#define T9PIDPunjabi     ((T9UINT)0x0074)
#define T9PIDPashto       ((T9UINT)0x0075)
#define T9PIDQuechua      ((T9UINT)0x0076)
#define T9PIDKirundi      ((T9UINT)0x0077)
#define T9PIDKiyarwanda   ((T9UINT)0x0078)
#define T9PIDSanskrit     ((T9UINT)0x0079)
#define T9PIDSindhi       ((T9UINT)0x007a)
#define T9PIDSangho       ((T9UINT)0x007b)
#define T9PIDSinghalese   ((T9UINT)0x007c)
#define T9PIDSamoan       ((T9UINT)0x007d)
#define T9PIDShona        ((T9UINT)0x007e)
#define T9PIDSomali       ((T9UINT)0x007f)
#define T9PIDSerbian      ((T9UINT)0x0080)
#define T9PIDSiswati      ((T9UINT)0x0081)
#define T9PIDSesotho      ((T9UINT)0x0082)
#define T9PIDSudanese     ((T9UINT)0x0083)
#define T9PIDTamil        ((T9UINT)0x0084)
#define T9PIDTelugu       ((T9UINT)0x0085)
#define T9PIDTajik        ((T9UINT)0x0086)
#define T9PIDTigrinya     ((T9UINT)0x0087)
#define T9PIDTurkmen      ((T9UINT)0x0088)
#define T9PIDTagalog      ((T9UINT)0x0089)
#define T9PIDSetswana     ((T9UINT)0x008a)
#define T9PIDTonga        ((T9UINT)0x008b)
#define T9PIDTatar        ((T9UINT)0x008c)
#define T9PIDTwi          ((T9UINT)0x008d)
#define T9PIDUigur        ((T9UINT)0x008e)
#define T9PIDUzbek        ((T9UINT)0x008f)
#define T9PIDVolapuk      ((T9UINT)0x0090)
#define T9PIDWolof        ((T9UINT)0x0091)
#define T9PIDYiddish      ((T9UINT)0x0092)
#define T9PIDYorouba      ((T9UINT)0x0093)
#define T9PIDZhuang       ((T9UINT)0x0094)
#define T9PIDNull         ((T9UINT)0x00FF)

/*----------------------------------------------------------------------------
 *  Secondary language data base indentifier definitions
 *----------------------------------------------------------------------------*/
#define T9SIDMASK                         ((T9UINT)0xFF00)
#define T9SIDNone                         ((T9UINT)0x0000)
#define T9SIDDEFAULT                      ((T9UINT)0x0100)

#define T9SIDChineseTraditional           ((T9UINT)0x0100)
#define T9SIDChineseSimplified            ((T9UINT)0x0200)

#define T9SIDChineseTraditionalPTI        ((T9UINT)0x1100)
#define T9SIDChineseSimplifiedPTI         ((T9UINT)0x1200)

#define T9SIDKoreanSyllable               ((T9UINT)0x0100)
#define T9SIDKoreanChun                   ((T9UINT)0x0300)


/*----------------------------------------------------------------------------
 *  Define T9 API function prototypes
 *----------------------------------------------------------------------------*/
T9STATUS T9FARCALL T9AddExplicitChar(T9FieldInfo *pFieldInfo, T9SYMB sSymID);
T9STATUS T9FARCALL T9HandleKey(T9FieldInfo *pFieldInfo, T9KEY eKeyID);
T9STATUS T9FARCALL T9AddTextString(T9FieldInfo *pFieldInfo, const T9SYMB *psBuf, T9UINT nLen);
T9STATUS T9FARCALL T9GetCodeVersion(T9U8 *pbResponse);
T9STATUS T9FARCALL T9GetMultitapTable(T9FieldInfo *pFieldInfo, T9KEY eKey, T9MINOR wPage, T9MINOR wPageSize, T9SYMB *psPage, T9MINOR *pmResponse);
T9STATUS T9FARCALL T9ClrCharConvert(T9FieldInfo *pFieldInfo);
T9STATUS T9FARCALL T9Cursor(T9FieldInfo *pFieldInfo, T9CA eType, T9UINT nCount);
T9STATUS T9FARCALL T9CursorBeg(T9FieldInfo *pFieldInfo);
T9STATUS T9FARCALL T9CursorEnd(T9FieldInfo *pFieldInfo);
T9STATUS T9FARCALL T9CursorLBWrd(T9FieldInfo *pFieldInfo);
T9STATUS T9FARCALL T9CursorLEWrd(T9FieldInfo *pFieldInfo);
T9STATUS T9FARCALL T9CursorRBWrd(T9FieldInfo *pFieldInfo);
T9STATUS T9FARCALL T9CursorREWrd(T9FieldInfo *pFieldInfo);
T9STATUS T9FARCALL T9GetLdbVersion(T9FieldInfo *pFieldInfo, T9U8 *pbResponse);
T9STATUS T9FARCALL T9GetCoreCompatID(T9U8 *pbResponse);
T9STATUS T9FARCALL T9GetKeyForSym(T9FieldInfo *pFieldInfo, T9SYMB sSymID, T9AuxType *paKey);
T9STATUS T9FARCALL T9GetLdbLanguage(T9U8 *pbLdbData, T9MINOR mBufLen, T9U16 *pwResponse);
T9STATUS T9FARCALL T9ResetTextBuf(T9FieldInfo *pFieldInfo, T9UINT nClearLength);
T9STATUS T9FARCALL T9RestoreState(T9FieldInfo *pFieldInfo, T9U32 dwStateBits);
T9STATUS T9FARCALL T9SaveState(T9FieldInfo *pFieldInfo, T9U32 *pdwStateBits);
T9STATUS T9FARCALL T9SetCharConvert(T9FieldInfo *pFieldInfo, T9ConvertChar_f T9ConvertChar);
T9STATUS T9FARCALL T9Exit(T9FieldInfo *pFieldInfo);
T9STATUS T9FARCALL T9TimeOut(T9FieldInfo *pFieldInfo, T9MINOR mTimerID);
T9STATUS T9FARCALL T9ValidateLdb(T9FieldInfo *pFieldInfo, T9UINT nLdbNum);
T9STATUS T9FARCALL T9MarkUseAndFlush(T9FieldInfo *pFieldInfo);
T9STATUS T9FARCALL T9SetExtMultitapMode(T9FieldInfo *pFieldInfo);
T9STATUS T9FARCALL T9ClrExtMultitapMode(T9FieldInfo *pFieldInfo);
T9STATUS T9FARCALL T9ClrUdbWriteByOEM(T9FieldInfo *pFieldInfo);
T9STATUS T9FARCALL T9SetUdbWriteByOEM(T9FieldInfo *pFieldInfo);
T9AuxType T9FARCALL T9GetSymbolClass(T9FieldInfo *pFieldInfo, T9SYMB sSymID);

#ifdef T9CHECKCOMPILE
enum {
    T9NULL_POINTERS = -1,
    T9WRONGSIZE_T9U8 = 0,
    T9WRONGSIZE_T9U16,
    T9WRONGSIZE_T9U32,
    T9WRONGSIZE_T9SYMB,
    T9WRONGSIZE_T9UINT,
    T9WRONGSIZE_T9INT,
    T9WRONGSIZE_T9MINOR,
    T9WRONGSIZE_T9FARDATA,
    T9WRONGSIZE_T9NEARDATA,
    T9WRONGSIZE_VOIDPOINTER,
    T9WRONGSIZE_FIELDINFO,
    T9WRONG_CORE_VERSION,
    T9WRONGVALUE_MAXDBKEYS,
    T9WRONGVALUE_MAXTAPKEYS,
    T9WRONGVALUE_MAXTREES,
    T9WRONGVALUE_CACHESIZE,
    T9WRONGDATA_INITOK,
    T9CHECKCOMPILE_NEXT

};

#endif

/* End don't mangle the function name if compile under C++ */
#if defined(__cplusplus)
    }
#endif
#endif /* #ifndef T9_H */



/* ----------------------------------< eof >--------------------------------- */

