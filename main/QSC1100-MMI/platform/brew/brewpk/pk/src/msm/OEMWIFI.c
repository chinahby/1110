/*=============================================================================

FILE: OEMWIFI.c

SERVICES: OEM Functions for the 802.11 WiFi Interface.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright c 2005-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
/*=============================================================================

$Header: //source/qcom/qct/platform/brew/brewpk/rel/3.1.5sp/pk/src/msm/OEMWIFI.c#9 $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------
 02/28/08    edc      Fixing channel update in net_list and 
                      Adding option category to identify which file to write
                      Cleaning up internal API and adding header for APIs
                      Fixing dereference to NULL ptr in OEMWIFI_LoadOptions()
 03/03/06     AT      Adding support for new authentication types
 08/18/05     AT      Adding WEP key conversions between text and binary
 04/15/05     AT      Adding new constants for Brew 3.1.4
 03/23/05    JAS      Converting "ANY" band pref between IWIFI and net values.
 03/04/05     AT      Release after code review
 11/04/04     AT      Initial Revision
=============================================================================*/


/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
/* Lint global dont cares */
/*lint -esym(754, IWIFI::pvt) */
/*lint -esym(754, IWIFIOpts::pvt) */
/*lint -save -e740 */
/*lint -save -e611 */

#include "BREWVersion.h"
#include "AEEStdLib.h"

#if MIN_BREW_VERSION(3,0)
#include "OEMFeatures.h"
#include "AEE_OEMDispatch.h"
#else
#include "OEMFeatures.h"
#include "AEE_OEM.h"
#endif // Brew Version


#ifdef FEATURE_WLAN

#ifdef AEE_SIMULATOR

#include "AEEModTable.h"

#else // !AEE_SIMULATOR

#include "err.h"
#include "AEECM.h"

#endif // !AEE_SIMULATOR

#include "AEE_OEM.h"
#include "AEEWIFI.h"
#include "OEMHeap.h"
#include "net.h"
#include "wlan_config.h"

#ifndef AEEWIFI_AUTHENTICATION_PAP
#include "AEEWIFIExt.h"
#endif

#ifdef AEE_SIMULATOR
#define MSG_ERROR( msg, p1, p2, p3 )
#define MSG_HIGH( msg, p1, p2, p3 )
#endif

/* enable to debug IWIFI memory operations */
//#define IWIFI_MEM_DEBUG

#define OEMWIFI_FREEIF(p) if ( NULL != ( p )) { OEMWIFI_Free(( p )); (( p )) = NULL; }
#define OEMWIFI_MALLOC(size) OEMWIFI_Malloc(( size ), __LINE__)
#define IN_RANGE(val, min, max) (( val ) >= ( min ) && ( val ) <= ( max ))

#define MAX_WLAN_FILE_SIZE     sizeof(data_settings_s_type)

#define DEFAULT_SSID          "default"
#define DEFAULT_SSID_SZ       7

#ifndef AEEWIFI_CHANNEL_MIN
#define AEEWIFI_CHANNEL_MIN   1
#endif

#ifndef AEEWIFI_CHANNEL_MAX
#define AEEWIFI_CHANNEL_MAX  14
#endif

#ifndef AEEWIFI_CHANNEL_ANY
#define AEEWIFI_CHANNEL_ANY  0x00FF
#endif

#ifndef AEEWIFI_BAND_MAX
#define AEEWIFI_BAND_MAX     AEEWIFI_BAND_5000MHZ_JAPAN
#endif

/* implementation dependent constants */
#define AEEWIFI_PROFILE_ID_MIN 0x0000
#define AEEWIFI_PROFILE_ID_MAX 0x00FF

#define AEEWIFI_POWERLEVEL_MIN     0x0000
#define AEEWIFI_POWERLEVEL_MAX     0x00C8

#define AEEWIFI_OPT_CATEGORY_NONE         0x00
#define AEEWIFI_OPT_CATEGORY_IP           0x01
#define AEEWIFI_OPT_CATEGORY_WLAN         0x02
#define AEEWIFI_OPT_CATEGORY_NETFILE      0x04

/*===========================================================================

                    LOCAL/STATIC DATA

===========================================================================*/

typedef struct data_settings
{
  wlan_config_wlan_params_type  wlan;
  wlan_config_ip_params_type    ip;
} data_settings_s_type;

/* Core Object, instantiated once */
typedef struct IWIFICore
{
  uint32                  uRefs;        /* Reference count */

  byte                   *pBuffer;      /* Working Buffer for net parser  */
  word                    BufferSize;
  net_file_parser_s_type  Parser;

  data_settings_s_type   *pDataRecord;  /* WLAN Data Settings. */

#ifndef AEE_SIMULATOR
  ICM                    *pICM;         /* Setting updates to CM */
#endif

  AEECallback             cbSysObj;

#ifdef IWIFI_MEM_DEBUG
  int                      malloc_cnt;   /* Used to Debug memory leaks. */
#endif
} IWIFICore;

/* IWIFI object, instantiated every time */
struct IWIFI
{
  AEEVTBL(IWIFI) *pvt;      
  IShell         *pIShell;
  uint32          uRefs; 
  AEECLSID        cls; 
  word            nEnum;     /* enumeration index */
  IWIFICore      *coreObj;   /* pointer to core object */
};

/* IWIFIOpts Object */
struct IWIFIOpts
 {
  AEEVTBL(IWIFIOpts) *pvt;
  IShell             *pIShell;
  uint32              uRefs;  
  AEECLSID            cls;
  IWebOpts           *pIWebOpts;    /* Web Options object containing 
                                       * WLAN preferences for this instance.
                                       */
  uint32              u32_ip_type;
};

/*=========================================================================*/

static IWIFICore *IWIFICoreObj = NULL;

/*===========================================================================

               INTERFACE   FUNCTION   DECLARATIONS   

===========================================================================*/

/* Interface to the WIFI object */
static uint32 OEMWIFI_AddRef( IWIFI *po );
static uint32 OEMWIFI_Release( IWIFI *po );
static int OEMWIFI_QueryInterface( IWIFI *po, AEECLSID clsid, void **ppNew );
static int OEMWIFI_LoadOptions( IWIFI *pMe, uint32 id, IWIFIOpts *options );
static int OEMWIFI_CommitOptions( IWIFI *pMe, uint32 id, IWIFIOpts *options );
static int OEMWIFI_GetStatistics( IWIFI *pMe, AEEWIFIStatistics *statistics );
static int OEMWIFI_EnumProfileInit( IWIFI *pMe );
static int OEMWIFI_EnumNextProfile( IWIFI *pMe, uint32 *id );

/* Interface to the IWIFIObts Object */
static uint32 OEMWIFIOpts_AddRef( IWIFIOpts *pMe );
static uint32 OEMWIFIOpts_Release( IWIFIOpts *pMe );
static int OEMWIFIOpts_QueryInterface( IWIFIOpts *pMe, AEECLSID id, void **ppo );
static int OEMWIFIOpts_AddOpt( IWIFIOpts *pMe, WIFIOpt *pOpt );
static int OEMWIFIOpts_RemoveOpt( IWIFIOpts *pMe, int32 nOptId );
static int OEMWIFIOpts_GetOpt( IWIFIOpts *pMe, int32 nOptId, WIFIOpt *pOpt );

static void *OEMWIFI_Malloc( uint32 size, int line );
static void OEMWIFI_Free( void *p );
static boolean OEMWIFI_WriteFile( uint32 id );
static int OEMWIFI_ReadFile( void );

/* The function supported by WIFI object. */
static const AEEVTBL(IWIFI) gOEMWIFIFuncs = 
{
   OEMWIFI_AddRef,
   OEMWIFI_Release,
   OEMWIFI_QueryInterface,
   OEMWIFI_LoadOptions,
   OEMWIFI_CommitOptions,
   OEMWIFI_GetStatistics,
   OEMWIFI_EnumProfileInit,
   OEMWIFI_EnumNextProfile
};

/* The functions supported by IWIFIOpts */
static const AEEVTBL(IWIFIOpts) gOEMWIFIOptsFuncs =
{
  OEMWIFIOpts_AddRef,
  OEMWIFIOpts_Release,
  OEMWIFIOpts_QueryInterface,
  OEMWIFIOpts_AddOpt,
  OEMWIFIOpts_RemoveOpt,
  OEMWIFIOpts_GetOpt
};

/*===========================================================================

               INTERNAL FUNCTION DECLARATIONS   

===========================================================================*/

/* Internal core function */
static IWIFICore * OEMWIFICore_New( IShell *pIShell, int *error_code );
static uint32 OEMWIFICore_AddRef( void );
static uint32 OEMWIFICore_Release( void );
static void OEMWIFICoreObjCB( void *pUsr );

/* Internal IWIFI function */
static void *OEMWIFI_Malloc( uint32 size, int line );
static void OEMWIFI_Free( void *p );
static boolean OEMWIFI_WriteFile( uint32 profile_id, uint8 category );
static int OEMWIFI_ReadFile( void );
static void OEMWIFI_Get_DefaultSysRec( net_sys_rec_s_type *system_record );
static void OEMWIFI_Get_DefaultProfileRec( net_profile_s_type *profile_record,
                                           uint32 profile_id );
static boolean OEMWIFI_Get_SysProfileRec( uint32 profile_id,
                                          net_sys_rec_s_type *system_record,
                                          net_profile_s_type *profile_record );
static boolean OEMWIFI_Set_OptVal( IWIFIOpts *pMe, int32 option_id,
                                   void *option_value,
                                   net_sys_rec_s_type *system_record,
                                   net_profile_s_type *profile_record );
static void *OEMWIFI_Get_OptVal( IWIFIOpts *pMe, int32 option_id,
                                 net_sys_rec_s_type *system_record,
                                 net_profile_s_type *profile_record );
static int OEMWIFI_Cmp_OptVal( IWIFIOpts *pMe, int32 option_id,
                               void *option_value,
                               net_sys_rec_s_type *system_record,
                               net_profile_s_type *profile_record );
static int OEMWIFI_LoadNestedOptions( net_profile_s_type *profile_record,
                                      IWebOpts *options );
static uint8 OEMWIFI_Get_OptCategory( int32 option_id );

/* Internal utility function */
static int ConvertStrToBinBuf( const char *pszwIn, uint8 *pBuf, uint8 nBufLen );
static void *ConvertBinBufToStr( const uint8 *pBuf, uint8 nLen );
static char *DuplicateStr( char *src, int size );
static char *ConvertIpToStr( uint32 value );
static boolean ConvertStrToIp( const char *in, uint32 *out );
static uint32 GetChannelFromMask( word value );
static word GetMaskFromChannel( uint32 channel );


/*===========================================================================

                   IWIFI SIMULTOR FUNCTION DEFINITIONS   

===========================================================================*/
#ifdef AEE_SIMULATOR

int OEMWIFI_New( IShell *pIShell, AEECLSID cls, void **ppif );
int OEMWIFIOpts_New ( IShell *pIShell, AEECLSID cls, void **ppif );

/* Create an AEEStaticClass array that lists the information about the interface */
const AEEStaticClass gIWIFIExtensionClasses[] =
{
   { AEECLSID_WIFI, ASCF_UPGRADE, 0, 0, OEMWIFI_New }, 
   { AEECLSID_WIFI_OPTS, ASCF_UPGRADE, 0, 0, OEMWIFIOpts_New }, 
   { 0, 0, 0, 0, NULL},
};

// Define this if you are intending to build this as a static extension.
// Otherwise this implementation file can be statically built and linked
// in the target build.
#ifdef _BUILD_AS_STATIC_EXTENSION_
/*===========================================================================

Function:  GetStaticClassEntries()

Description:
	This function is required to be exposed if this interface will be 
	exposed as a static extension DLL. BREW Simulator loads all the DLLs
	that expose this function as a Static Extension DLL.

Prototype:
		const AEEStaticClass* GetStaticClassEntries(void)

Parameters: None	

Return Value:
	return an array of AEEStaticClass

Comments:  None

Side Effects: None

==============================================================================*/
BREWSTATICEXTENSION_API const AEEStaticClass* GetStaticClassEntries( void )
{
	MSG_HIGH( "OEMWIFI GetStaticClassEntries called.", 0, 0, 0 );

	return gIWIFIExtensionClasses;
}
#endif // _BUILD_AS_STATIC_EXTENSION_


//Global Context for MyInterface.
OEMCONTEXT  ghIWIFI = NULL;

OBJECT( CWIFI )
{
   // Standard stuff
   AEEVTBL( IWIFI ) *pvt;
   uint32            nRefs;
};

OEMCONTEXT  ghIWIFIOpts = NULL;

OBJECT( CWIFIOPTS )
{
   // Standard stuff
   AEEVTBL( IWIFIOpts ) *pvt;
   uint32                nRefs;
};

#endif // AEE_SIMULATOR


/*===========================================================================

                   IWIFI FUNCTION DEFINITIONS   

===========================================================================*/
/*=============================================================================
FUNCTION: OEMWIFI_New

DESCRIPTION:  
  Allocates a new IWIFI object, creates a Core object if it is not present,
  stores the App context and initializes object data. A new IWIFI object is
  created for every ISHELL_CreateInstance().

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

RETURN VALUE:
  EBADPARAM    if any of the parameters are NULL
  ENOMEMORY    when out of memory
  EUNSUPPORTED if the class ID is not AEECLSID_WIFI
  AEE_SUCCESS  otherwise

=============================================================================*/
int OEMWIFI_New( IShell *pIShell, AEECLSID cls, void **ppif )
{
#ifdef AEE_SIMULATOR
   extern void setNetShell( IShell *pShell );
#endif
   IWIFI *pNew;
   int    nerr;
  
   // Check parameters.
   if (( NULL == pIShell ) || ( NULL == ppif ))
   {
      MSG_ERROR( "OEMWIFI_New: NULL parameters - pIShell 0x%x, ppif 0x%x", (uint32)pIShell, (uint32)ppif, 0 );
      return EBADPARM;
   }

   if ( cls != AEECLSID_WIFI )
   {
      MSG_ERROR( "OEMWIFI_New: bad class ID 0x%x", cls, 0, 0 );
      return ECLASSNOTSUPPORT;
   }

   // Allocate the object.
   pNew = (IWIFI *)AEE_NewClass( (IBaseVtbl*)&gOEMWIFIFuncs, sizeof( IWIFI )); 
   if ( pNew == NULL )
   {
      MSG_ERROR( "OEMWIFI_New: out of memory", 0, 0, 0 );
      return ENOMEMORY;
   }


   /* Initialize the values. */
   pNew->pIShell = pIShell;
   pNew->cls = cls;
   pNew->uRefs = 1;

#ifdef AEE_SIMULATOR
   setNetShell( pIShell );
#endif

   pNew->coreObj = OEMWIFICore_New( pIShell, &nerr );
   if ( NULL == pNew->coreObj ) 
   {
      (void)OEMWIFI_Release( pNew );
      MSG_ERROR( "OEMWIFI_New: failed to initialize CoreObj", 0, 0, 0 );
      return nerr;
   }

   *ppif = pNew;

   return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMWIFI_AddRef

DESCRIPTION:  
  Increments object reference count and returns the new incremented reference count.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static uint32 OEMWIFI_AddRef( IWIFI *pMe )
{
  return ( ++pMe->uRefs ); 
}

/*=============================================================================
FUNCTION: OEMWIFI_Release

DESCRIPTION:  
  Decreases the number of references to the IWIFI object.  If the
  last reference is removed, it deallocates the object.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static uint32 OEMWIFI_Release( IWIFI *pMe )
{
   if ( --pMe->uRefs != 0 )
   {
      return pMe->uRefs;
   }

   /* Release core */
   if ( NULL != pMe->coreObj )
   {
      (void)OEMWIFICore_Release();
   }

   // Ref count is zero.  Release memory
   FREE( pMe );
   return 0;
}

/*===========================================================================

FUNCTION: OEMWIFI_QueryInterface

DESCRIPTION
  Allows applications to query if IWIFI supports other interfaces and then
  returns a pointer to that new interface supported via IWIFI.  Currently,
  we do not support any other interface.

DEPENDENCIES
  none

RETURN VALUE
  EBADPARM           if any of the parameters are NULL
  ECLASSNOTSUPPORT   if requested class is not supported by this object
  otherwise requested interface pointer

SIDE EFFECTS
===========================================================================*/
static int OEMWIFI_QueryInterface( IWIFI *pMe, AEECLSID id, void **ppNew )
{
   int ret_val = AEE_SUCCESS;

   if ( NULL == ppNew )
   {
      MSG_ERROR( "OEMWIFI_QueryInterface: NULL ppNew", 0, 0, 0 );
      return EBADPARM;
   }

   switch ( id )
   {
      case AEECLSID_WIFI:
      case AEECLSID_WIFI_313:
      case AEECLSID_QUERYINTERFACE:
         if ( 0 != OEMWIFI_AddRef( pMe ))
         {
            *ppNew = pMe;
         } 
         else
         {
            *ppNew = NULL;
            MSG_ERROR( "OEMWIFI_QueryInterface: class 0x%x not supported", id, 0, 0 );
            ret_val = ECLASSNOTSUPPORT;
         }
         break;

      default:
         *ppNew = NULL;
         MSG_ERROR( "OEMWIFI_QueryInterface: calss 0x%x not supported", id, 0, 0 );
         ret_val = ECLASSNOTSUPPORT;
         break;
   }


   return ret_val;
}

/*===========================================================================
FUNCTION ConvertStrToBinBuf

DESCRIPTION
   Converts an ASCII string of hexadecimal characters to a buffer of
   binary data.  Note that, since each character translates to 4 bits, the
   input character buffer must be of even length for the translation to work.

DEPENDENCIES
   None

SIDE EFFECTS
   None

RETURN VALUE
   TRUE if coversion was successful. 
   FALSE otherwise.

COMMENTS
   This routine expects hexadecimal characters only, but it does perform
   all necessary error checking.  Upper- and lower-case hex values are
   both accepted.
===========================================================================*/
static int ConvertStrToBinBuf( const char *pszwIn, uint8 *pBuf, uint8 nBufLen )
{
   int   nIdx = 0;
   int   nStrLen;
   char  cToConvert;
   uint8 nValue = 0;

   if (( !pszwIn ) || ( !pBuf ) || ( nBufLen == 0 ))
   {
      MSG_ERROR( "Invalid input, %x %x %d", pszwIn, pBuf, nBufLen );
      return FALSE;
   }

   nStrLen = (int)STRLEN( pszwIn );

   /* The key data must fill the buffer completely */
   if (( nStrLen & 1 ) || (nBufLen != ( nStrLen >>= 1 )))
   {
      
      MSG_ERROR( "WEP key size missmatch", 0, 0, 0 );
      return FALSE;
   }

   while (( cToConvert = pszwIn[ nIdx++ ] ) != '\0' )
   {
     /* Convert the character to binary value */
     if (( cToConvert >= '0' ) && ( cToConvert <= '9' ))
     {
        nValue += (uint8)( cToConvert - '0' );
     }
     else if (( cToConvert >= 'A' ) && ( cToConvert <= 'F' ))
     {
        nValue += (uint8)( cToConvert - 'A' + 10 );
     }
     else if (( cToConvert >= 'a' ) && ( cToConvert <= 'f' ))
     {
        nValue += (uint8)( cToConvert - 'a' + 10 );
     }
     else
     {
        MSG_ERROR( "Input string has invalid character, %d", (uint16)cToConvert, 0, 0 );
        return FALSE;
     }

     /* Copy the converted value into the appropriate byte */
     if ( nIdx & 1 )
     {
       /* For characters at even index in input string, shift the value over to
        * the upper nibble to make room for the character at odd index 
        * in the lower nibble. */
        nValue <<= 4;
     }
     else
     {
        *pBuf++ = nValue;
        nValue = 0;
     }
   }
   return TRUE;
}

/*===========================================================================
FUNCTION ConvertBinBufToStr

DESCRIPTION
   Converts a buffer of binary data to a NULL terminated string of
   hexadecimal characters. Note that, since each nibble translates to a
   character, the output buffer will be sizeof(char) * 2 * (nLen + 1) bytes .

DEPENDENCIES
   None

SIDE EFFECTS
   None

RETURN VALUE
   Pointer to created string

COMMENTS

===========================================================================*/
static void *ConvertBinBufToStr( const uint8 *pBuf, uint8 nLen)
{
   int    outLen = 0, nIdx = 0;
   uint8  nToConvert, nibble;
   char  *result;

   if (( !pBuf ) || ( nLen == 0 ))
   {
      MSG_ERROR( "Invalid input, %x %d", pBuf, nLen, 0 );
      return NULL;
   }

   if ( NULL == ( result = (char *)MALLOC( sizeof( char ) * 2 *  nLen + 1 )))
   {
      MSG_ERROR( "Out of Memory", 0, 0, 0 );
      return NULL;
   }

   while ( nIdx < nLen )
   {
     /* Convert the nibbles to wide characters */
     nToConvert = pBuf[ nIdx++ ];

     nibble = nToConvert >> 4;
     result[ outLen++ ] = (char)( nibble <= 9 ? nibble + '0' :
                                                nibble - 10 + 'A' );
     nibble = nToConvert & 0x0F;
     result[ outLen++ ] = (char)( nibble <= 9 ? nibble + '0' :
                                                nibble - 10 + 'A' );
   }
   result[ outLen ] = '\0';
   return (void *)result;
}

/*===========================================================================

FUNCTION: DuplicateStr

DESCRIPTION
  Create a copy of first 'size' characters in 'src' string. Note that
  'src' may not be a '\0' terminated string but the returned string is.

DEPENDENCIES
  none

RETURN VALUE
  NULL  when out of memory
  otherwise pointer to the duplicated string

SIDE EFFECTS
===========================================================================*/
static char *DuplicateStr( char *src, int size )
{
   char *result = NULL;

   if ( NULL != ( result = (char *)MALLOC( sizeof( char ) * ( size + 1 ))))
   {
      (void) STRNCPY( result, src, size );
      result[ size ] = '\0';
   }

   return result;
}

/*===========================================================================

FUNCTION: ConvertIpToStr

DESCRIPTION
  Takes an integer value and creates an IP string representing for this entry.

DEPENDENCIES
  none

RETURN VALUE
  NULL  when out of memory
  otherwise pointer to string representing IP address

SIDE EFFECTS
===========================================================================*/
static char *ConvertIpToStr( uint32 value )
{
#define IPSTRSZ 17
  char tmp[ IPSTRSZ ]; /* temp home for IP address "xxx.xxx.xxx.xxx" max size */

  INET_NTOA( value, tmp, sizeof( tmp ));

  return DuplicateStr( tmp, IPSTRSZ );
}

/*===========================================================================

FUNCTION ConvertStrToIp

DESCRIPTION
  This function will take a AECHAR string and convert it into a
  32 bit IP address in Host Order

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if we could convert the string, FALSE if not.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean ConvertStrToIp( const char *in, uint32 *out )
{
  /* Convert it to an unsigned integer. */
  return ( INET_ATON( in, out ));
}

/*===========================================================================

FUNCTION: OEMWIFI_Get_DefaultSysRec

DESCRIPTION
  Fills in default values for net_sys_rec_s_type

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static void OEMWIFI_Get_DefaultSysRec( net_sys_rec_s_type *system_record )
{
   (void)MEMSET( (void *)system_record, 0, sizeof( net_sys_rec_s_type ));
   system_record->len = sizeof( net_sys_rec_s_type );
   system_record->type = NET_REC_TYPE_WLAN;
   system_record->sys_rec.wlan.mode = AEEWIFI_MODE_ANY;
   system_record->sys_rec.wlan.tech = AEEWIFI_TECH_80211_B;
   system_record->sys_rec.wlan.chan = 1;
   (void)STRCPY( (char *)system_record->sys_rec.wlan.ssid.ssid, DEFAULT_SSID );
   system_record->sys_rec.wlan.ssid.size = DEFAULT_SSID_SZ;
}

/*===========================================================================

FUNCTION: OEMWIFI_Get_DefaultProfileRec

DESCRIPTION
  Fills in default values for net_profile_s_type

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static void OEMWIFI_Get_DefaultProfileRec( net_profile_s_type *profile_record, uint32 profile_id )
{
   (void)MEMSET( (void *)profile_record, 0, sizeof( net_profile_s_type ));
   profile_record->len = sizeof( net_profile_s_type );
   profile_record->type = NET_PROFILE_TYPE_WLAN;
   profile_record->profile_id = (byte)profile_id;
}

/*===========================================================================

FUNCTION: GetChannelFromMask

DESCRIPTION
  Converts net channel bitmask representation to Brew channel number

DEPENDENCIES
  none

RETURN VALUE
  Wifi Channel number in the range AEEWIFI_CHANNEL_MIN .. AEEWIFI_CHANNEL_MIN

SIDE EFFECTS
===========================================================================*/
static uint32 GetChannelFromMask( word mask )
{
   uint32 channel;

   for ( channel = 0; 0 != mask; ++channel, mask >>= 1 ) ;

   if ( !IN_RANGE( channel , AEEWIFI_CHANNEL_MIN, AEEWIFI_CHANNEL_MAX ))
   {
      /* return any channel  */
      channel = AEEWIFI_CHANNEL_ANY;
   }

   return channel;
}

/*===========================================================================

FUNCTION: GetMaskFromChannel

DESCRIPTION
  Converts Brew channel number to net channel bitmask representation 

DEPENDENCIES
  none

RETURN VALUE
  net channel bitmask

SIDE EFFECTS
===========================================================================*/
static word GetMaskFromChannel( uint32 channel )
{
  word mask;

  mask = ( AEEWIFI_CHANNEL_ANY == channel-- ) ?
         (word)NET_CHAN_WLAN_2400_ANY : (word)SYS_BM( channel );

  return mask;
}

/*===========================================================================

FUNCTION: OEMWIFI_Get_OptCategory

DESCRIPTION
  Get IWIFI option category based on option ID

DEPENDENCIES
  none

RETURN VALUE
  Option category

SIDE EFFECTS
===========================================================================*/
static uint8 OEMWIFI_Get_OptCategory(int32 option_id)
{
  switch ( option_id )
  {
    case AEEWIFI_OPT_NESTED_AUTH_ID:
    case AEEWIFI_OPT_PROFILE_ID:
    case AEEWIFI_OPT_TECH:
    case AEEWIFI_OPT_BANDPREF:
    case AEEWIFI_OPT_CHANNEL:
    case AEEWIFI_OPT_GROUPS:
    case AEEWIFI_OPT_WLAN_MODE:
    case AEEWIFI_OPT_SSID:
    case AEEWIFI_OPT_MOB_IP_PROFILE_ID:
    case AEEWIFI_OPT_CERTIFICATE_ID:
    case AEEWIFI_OPT_WEP_KEY_ID:
    case AEEWIFI_OPT_WEP_KEY:
    case AEEWIFI_OPT_WEP_64BIT_KEY_ID:
    case AEEWIFI_OPT_WEP_64BIT_KEY_0:
    case AEEWIFI_OPT_WEP_64BIT_KEY_1:
    case AEEWIFI_OPT_WEP_64BIT_KEY_2:
    case AEEWIFI_OPT_WEP_64BIT_KEY_3:
    case AEEWIFI_OPT_WEP_128BIT_KEY_ID:
    case AEEWIFI_OPT_WEP_128BIT_KEY_0:
    case AEEWIFI_OPT_WEP_128BIT_KEY_1:
    case AEEWIFI_OPT_WEP_128BIT_KEY_2:
    case AEEWIFI_OPT_WEP_128BIT_KEY_3:
    case AEEWIFI_OPT_8021X_ID:
    case AEEWIFI_OPT_8021X_PASSWORD:
    case AEEWIFI_OPT_USER_ID:
    case AEEWIFI_OPT_ENCRYPTION_TYPE:
    case AEEWIFI_OPT_AUTHENTICATION_TYPE:
      return AEEWIFI_OPT_CATEGORY_NETFILE;

    case AEEWIFI_OPT_LISTEN_INTERVAL:
    case AEEWIFI_OPT_PREAMBLE_TYPE:
    case AEEWIFI_OPT_RTSTHRESHOLD:
    case AEEWIFI_OPT_SCANTIMING:
    case AEEWIFI_OPT_POWERLEVEL:
    case AEEWIFI_OPT_FRAGMENTATIONTHRESHOLD:
#if MIN_BREW_VERSIONEx(3,1,5)
    case AEEWIFI_OPT_RSSI_THRESHOLD:
#endif
      return AEEWIFI_OPT_CATEGORY_WLAN;

    case AEEWIFI_OPT_DHCP_ENABLED:
//  case AEEWIFI_OPT_IP_TYPE:  /* Local option not related to either category */
    case AEEWIFI_OPT_STATIC_IP:
    case AEEWIFI_OPT_DNS_ADDRESS:
    case AEEWIFI_OPT_SUBNET_MASK:
    case AEEWIFI_OPT_DEFAULT_GATEWAY:
#if MIN_BREW_VERSIONEx(3,1,5)
    case AEEWIFI_OPT_MTU_SIZE:
    case AEEWIFI_OPT_SECONDARY_DNS_ADDRESS:
    case AEEWIFI_OPT_DOMAIN_NAME:
#endif
      return AEEWIFI_OPT_CATEGORY_IP;

    default:
      return AEEWIFI_OPT_CATEGORY_NONE;
  }
}

/*===========================================================================

FUNCTION: OEMWIFI_Cmp_OptVal

DESCRIPTION
  Compare Web Opt style option value with current system/profile records

DEPENDENCIES
  none

RETURN VALUE
  0           option value or option string are identical to current
  non-0       otherwise

SIDE EFFECTS
===========================================================================*/
static int OEMWIFI_Cmp_OptVal( IWIFIOpts *pMe, int32 option_id,
                               void *option_value,
                               net_sys_rec_s_type *system_record,
                               net_profile_s_type *profile_record )
{ 
  WebOpt current;
  int diff = -1;

  current.pVal = OEMWIFI_Get_OptVal( pMe, option_id, system_record, profile_record );

  switch ( option_id )
  {
    /* option value is a value */
    case AEEWIFI_OPT_NESTED_AUTH_ID:
    case AEEWIFI_OPT_PROFILE_ID:
    case AEEWIFI_OPT_TECH:
    case AEEWIFI_OPT_BANDPREF:
    case AEEWIFI_OPT_CHANNEL:
    case AEEWIFI_OPT_GROUPS:
    case AEEWIFI_OPT_WLAN_MODE:
    case AEEWIFI_OPT_MOB_IP_PROFILE_ID:
    case AEEWIFI_OPT_CERTIFICATE_ID:
    case AEEWIFI_OPT_WEP_KEY_ID:
    case AEEWIFI_OPT_WEP_64BIT_KEY_ID:
    case AEEWIFI_OPT_WEP_128BIT_KEY_ID:
    case AEEWIFI_OPT_ENCRYPTION_TYPE:
    case AEEWIFI_OPT_AUTHENTICATION_TYPE:
    case AEEWIFI_OPT_LISTEN_INTERVAL:
    case AEEWIFI_OPT_PREAMBLE_TYPE:
    case AEEWIFI_OPT_RTSTHRESHOLD:
    case AEEWIFI_OPT_SCANTIMING:
    case AEEWIFI_OPT_POWERLEVEL:
    case AEEWIFI_OPT_FRAGMENTATIONTHRESHOLD:
#if MIN_BREW_VERSIONEx(3,1,5)
    case AEEWIFI_OPT_RSSI_THRESHOLD:
    case AEEWIFI_OPT_MTU_SIZE:
#endif
    case AEEWIFI_OPT_DHCP_ENABLED:
    case AEEWIFI_OPT_IP_TYPE:
      diff = ((uint32)option_value) - ((uint32)current.pVal);
      break;

    /* option value is a string pointer */
    case AEEWIFI_OPT_SSID:
    case AEEWIFI_OPT_WEP_KEY:
    case AEEWIFI_OPT_WEP_64BIT_KEY_0:
    case AEEWIFI_OPT_WEP_64BIT_KEY_1:
    case AEEWIFI_OPT_WEP_64BIT_KEY_2:
    case AEEWIFI_OPT_WEP_64BIT_KEY_3:
    case AEEWIFI_OPT_WEP_128BIT_KEY_0:
    case AEEWIFI_OPT_WEP_128BIT_KEY_1:
    case AEEWIFI_OPT_WEP_128BIT_KEY_2:
    case AEEWIFI_OPT_WEP_128BIT_KEY_3:
    case AEEWIFI_OPT_8021X_ID:
    case AEEWIFI_OPT_8021X_PASSWORD:
    case AEEWIFI_OPT_USER_ID:
    case AEEWIFI_OPT_STATIC_IP:
    case AEEWIFI_OPT_DNS_ADDRESS:
    case AEEWIFI_OPT_SUBNET_MASK:
    case AEEWIFI_OPT_DEFAULT_GATEWAY:
#if MIN_BREW_VERSIONEx(3,1,5)
    case AEEWIFI_OPT_SECONDARY_DNS_ADDRESS:
    case AEEWIFI_OPT_DOMAIN_NAME:
#endif
      diff = STRCMP( option_value, current.pVal );
      /* Free string allocated by OEMWIFI_Get_OptVal() */
      FREEIF( current.pVal );
      break;

    default:
      MSG_ERROR( "Compare unsupported option ID: %d", option_id, 0, 0 );
      break;
  }

  return diff;
}

/*===========================================================================

FUNCTION: OEMWIFI_Set_OptVal

DESCRIPTION
  Takes the Web Opt style option value and saves it into the correct profile.
  AEEWIFI_OPT_PROFILE_ID is handled by the caller.

DEPENDENCIES
  none

RETURN VALUE
  FALSE       if any of the parameterd are NULL pointers,
              or net data cannot be retrieved
  TRUE        otherwise

SIDE EFFECTS
===========================================================================*/
static boolean OEMWIFI_Set_OptVal( IWIFIOpts *pMe, int32 option_id,
                                   void *option_value,
                                   net_sys_rec_s_type *system_record,
                                   net_profile_s_type *profile_record )
{ 
  boolean               ret_val = TRUE;
  data_settings_s_type *data_record;
  uint32                tmp_value, size;

  data_record = IWIFICoreObj->pDataRecord;

  switch( option_id )
  {
    case AEEWIFI_OPT_TECH:
      tmp_value = (uint32)option_value;
      system_record->sys_rec.wlan.tech = (byte)tmp_value;
      break;

    case AEEWIFI_OPT_BANDPREF:
      tmp_value = (uint32)option_value;
      if ( AEEWIFI_BAND_ANY == tmp_value )
      {
        /* Convert "any" value from IWIFI to net value */
        tmp_value = (uint32)NET_WLAN_BAND_ANY;
      }
      system_record->sys_rec.wlan.band = (byte)tmp_value;
      break;

    case AEEWIFI_OPT_CHANNEL:
      tmp_value = (uint32)option_value;
      system_record->sys_rec.wlan.chan |= GetMaskFromChannel( tmp_value );
      break;

    case AEEWIFI_OPT_GROUPS:
      tmp_value = (uint32)option_value;
      system_record->sys_rec.wlan.groups = (word)tmp_value;
      break;

    case AEEWIFI_OPT_WLAN_MODE:
      tmp_value = (uint32)option_value;
      system_record->sys_rec.wlan.mode = (byte)tmp_value;
      break;

    case AEEWIFI_OPT_SSID:
      size = STRLEN((char *)option_value);
      if ( 0 == size )
      {
         (void)STRCPY( (char *)system_record->sys_rec.wlan.ssid.ssid, DEFAULT_SSID );
         system_record->sys_rec.wlan.ssid.size = DEFAULT_SSID_SZ;
      }
      else if ( size <= sizeof( system_record->sys_rec.wlan.ssid.ssid ))
      {
         system_record->sys_rec.wlan.ssid.size = (byte)size;
         (void)STRNCPY( (char *) system_record->sys_rec.wlan.ssid.ssid,
                        (char *) option_value, size );
      }
      else
      {
         ret_val = FALSE;
      }
      break;

    case AEEWIFI_OPT_ENCRYPTION_TYPE:
      tmp_value = (uint32)option_value;
      profile_record->profile.wlan.encryp_type = (byte)tmp_value;
      break;

    case AEEWIFI_OPT_AUTHENTICATION_TYPE:
      tmp_value = (uint32)option_value;
      profile_record->profile.wlan.auth_type = (byte)tmp_value;
      break;

    case AEEWIFI_OPT_MOB_IP_PROFILE_ID:
      tmp_value = (uint32)option_value;
      profile_record->profile.wlan.mob_ip_profile_id = (byte)tmp_value;
      break;

    case AEEWIFI_OPT_NESTED_AUTH_ID:
       if ( AEEWIFI_AUTHENTICATION_PEAP == profile_record->profile.wlan.auth_type ||
            AEEWIFI_AUTHENTICATION_TTLS == profile_record->profile.wlan.auth_type )
       {
          net_profile_s_type tmp_profile;
          net_status_e_type  status = NET_STATUS_SUCCESS;

          tmp_value = (uint32)option_value;
          profile_record->profile.wlan.auth_info.peap_ttls.nested.profile_id = (byte)tmp_value;

          OEMWIFI_Get_DefaultProfileRec( &tmp_profile, tmp_value  );

          // get the nested profile record
          if ( NET_STATUS_SUCCESS != net_get_profile( (byte)tmp_value, &tmp_profile, &IWIFICoreObj->Parser ))
          {
             profile_record->profile.wlan.auth_info.peap_ttls.nested.auth = NET_AUTH_TYPE_NO_AUTH;
             status  = net_insert_profile( &tmp_profile, &IWIFICoreObj->Parser );
          }
          else
          {
             profile_record->profile.wlan.auth_info.peap_ttls.nested.auth = tmp_profile.profile.wlan.auth_type;
          }

          // save current profile record and switch to processing the nested one
          if ( NET_STATUS_SUCCESS == net_modify_profile( profile_record, &IWIFICoreObj->Parser ))
          {
             (void)MEMCPY( (void *)profile_record, (void *)&tmp_profile, sizeof( net_profile_s_type ));
          }
          else
          {
             MSG_ERROR( "ERROR %d nested profile record!", status, 0, 0 );
             ret_val = FALSE;
          }
       }
       else
       {
          ret_val = FALSE;
       }
       break;

    case AEEWIFI_OPT_CERTIFICATE_ID:
       if ( AEEWIFI_AUTHENTICATION_8021X_TLS_CERT == profile_record->profile.wlan.auth_type )
       {
          tmp_value = (uint32)option_value;
          profile_record->profile.wlan.auth_info.x_info.tls_cert.cert_id = (byte)tmp_value;
       }
       else if ( AEEWIFI_AUTHENTICATION_PEAP == profile_record->profile.wlan.auth_type ||
                 AEEWIFI_AUTHENTICATION_TTLS == profile_record->profile.wlan.auth_type )
       {
          tmp_value = (uint32)option_value;
          profile_record->profile.wlan.auth_info.peap_ttls.cert_id = (byte)tmp_value;
       }
       else
       {
          ret_val = FALSE;
       }
       break;

    case AEEWIFI_OPT_WEP_KEY_ID:
      tmp_value = (uint32)option_value;
      profile_record->profile.wlan.auth_info.wep_104_bit_info.default_key = (byte)tmp_value;
      break;

    case AEEWIFI_OPT_WEP_KEY:
    {
      byte *key_ptr = NULL;

      if ( AEEWIFI_ENCRYPTION_WEP_128bit == profile_record->profile.wlan.encryp_type )
      {
         switch ( profile_record->profile.wlan.auth_info.wep_104_bit_info.default_key )
         {
            case 1:
               key_ptr = profile_record->profile.wlan.auth_info.wep_104_bit_info.key2;
               break;

            case 2:
               key_ptr = profile_record->profile.wlan.auth_info.wep_104_bit_info.key3;
               break;

            case 3:
               key_ptr = profile_record->profile.wlan.auth_info.wep_104_bit_info.key4;
               break;

            case 0:
            default:
               key_ptr = profile_record->profile.wlan.auth_info.wep_104_bit_info.key1;
               break;
         }        
         ret_val = ConvertStrToBinBuf( (char *)option_value, key_ptr,
                                       sizeof( profile_record->profile.wlan.auth_info.wep_104_bit_info.key1 ));
      }
      else if ( AEEWIFI_ENCRYPTION_WEP_64bit == profile_record->profile.wlan.encryp_type )
      {
         switch ( profile_record->profile.wlan.auth_info.wep_40_bit_info.default_key )
         {
            case 1:
               key_ptr = profile_record->profile.wlan.auth_info.wep_40_bit_info.key2;
               break;

            case 2:
               key_ptr = profile_record->profile.wlan.auth_info.wep_40_bit_info.key3;
               break;

            case 3:
               key_ptr = profile_record->profile.wlan.auth_info.wep_40_bit_info.key4;
               break;

            case 0:
            default:
               key_ptr = profile_record->profile.wlan.auth_info.wep_40_bit_info.key1;
               break;
         }
         ret_val = ConvertStrToBinBuf( (char *)option_value, key_ptr,
                                       sizeof( profile_record->profile.wlan.auth_info.wep_40_bit_info.key1 ));
      }
      else
      {
         ret_val = FALSE;
      }
      break;
    }

    case AEEWIFI_OPT_WEP_64BIT_KEY_ID:
      if ( AEEWIFI_ENCRYPTION_WEP_64bit == profile_record->profile.wlan.encryp_type )
      {
         tmp_value = (uint32)option_value;
         profile_record->profile.wlan.auth_info.wep_40_bit_info.default_key = (byte)tmp_value;
      }
      else
      {
         ret_val = FALSE;
      }
      break;

    case AEEWIFI_OPT_WEP_64BIT_KEY_0:
      if ( AEEWIFI_ENCRYPTION_WEP_64bit == profile_record->profile.wlan.encryp_type )
      {
         ret_val = ConvertStrToBinBuf( (char *)option_value, profile_record->profile.wlan.auth_info.wep_40_bit_info.key1, 
                                       (uint8)sizeof( profile_record->profile.wlan.auth_info.wep_40_bit_info.key1 ));
      }
      else
      {
         ret_val = FALSE;
      }
      break;

    case AEEWIFI_OPT_WEP_64BIT_KEY_1:
      if ( AEEWIFI_ENCRYPTION_WEP_64bit == profile_record->profile.wlan.encryp_type )
      {
         ret_val = ConvertStrToBinBuf( (char *)option_value, profile_record->profile.wlan.auth_info.wep_40_bit_info.key2, 
                                       (uint8)sizeof( profile_record->profile.wlan.auth_info.wep_40_bit_info.key2 ));
      }
      else
      {
         ret_val = FALSE;
      }
      break;

    case AEEWIFI_OPT_WEP_64BIT_KEY_2:
      if ( AEEWIFI_ENCRYPTION_WEP_64bit == profile_record->profile.wlan.encryp_type )
      {
         ret_val = ConvertStrToBinBuf( (char *)option_value, profile_record->profile.wlan.auth_info.wep_40_bit_info.key3, 
                                       (uint8)sizeof( profile_record->profile.wlan.auth_info.wep_40_bit_info.key3 ));
      }
      else
      {
         ret_val = FALSE;
      }
      break;

    case AEEWIFI_OPT_WEP_64BIT_KEY_3:
      if ( AEEWIFI_ENCRYPTION_WEP_64bit == profile_record->profile.wlan.encryp_type )
      {
         ret_val = ConvertStrToBinBuf( (char *)option_value, profile_record->profile.wlan.auth_info.wep_40_bit_info.key4, 
                                       (uint8)sizeof( profile_record->profile.wlan.auth_info.wep_40_bit_info.key4 ));
      }
      else
      {
         ret_val = FALSE;
      }
      break;

    case AEEWIFI_OPT_WEP_128BIT_KEY_ID:
      if ( AEEWIFI_ENCRYPTION_WEP_128bit == profile_record->profile.wlan.encryp_type )
      {
         tmp_value = (uint32)option_value;
         profile_record->profile.wlan.auth_info.wep_104_bit_info.default_key = (byte)tmp_value;
      }
      else
      {
         ret_val = FALSE;
      }
      break;

    case AEEWIFI_OPT_WEP_128BIT_KEY_0:
      if ( AEEWIFI_ENCRYPTION_WEP_128bit == profile_record->profile.wlan.encryp_type )
      {
         ret_val = ConvertStrToBinBuf( (char *)option_value, profile_record->profile.wlan.auth_info.wep_104_bit_info.key1, 
                                       (uint8)sizeof( profile_record->profile.wlan.auth_info.wep_104_bit_info.key1 ));
      }
      else
      {
         ret_val = FALSE;
      }
      break;
    
    case AEEWIFI_OPT_WEP_128BIT_KEY_1:
      if ( AEEWIFI_ENCRYPTION_WEP_128bit == profile_record->profile.wlan.encryp_type )
      {
         ret_val = ConvertStrToBinBuf( (char *)option_value, profile_record->profile.wlan.auth_info.wep_104_bit_info.key2, 
                                       (uint8)sizeof( profile_record->profile.wlan.auth_info.wep_104_bit_info.key2 ));
      }
      else
      {
         ret_val = FALSE;
      }
      break;

    case AEEWIFI_OPT_WEP_128BIT_KEY_2:
      if ( AEEWIFI_ENCRYPTION_WEP_128bit == profile_record->profile.wlan.encryp_type )
      {
         ret_val = ConvertStrToBinBuf( (char *)option_value, profile_record->profile.wlan.auth_info.wep_104_bit_info.key3, 
                                       (uint8)sizeof( profile_record->profile.wlan.auth_info.wep_104_bit_info.key3 ));
      }
      else
      {
         ret_val = FALSE;
      }
      break;

    case AEEWIFI_OPT_WEP_128BIT_KEY_3:
      if ( AEEWIFI_ENCRYPTION_WEP_128bit == profile_record->profile.wlan.encryp_type )
      {
         ret_val = ConvertStrToBinBuf( (char *)option_value, profile_record->profile.wlan.auth_info.wep_104_bit_info.key4, 
                                       (uint8)sizeof( profile_record->profile.wlan.auth_info.wep_104_bit_info.key4 ));
      }
      else
      {
         ret_val = FALSE;
      }
      break;

    case AEEWIFI_OPT_8021X_ID:
    {
      size = STRLEN( (char *)option_value );
      if ( AEEWIFI_AUTHENTICATION_8021X_TLS == (AEEWIFIAuthenticationOption)profile_record->profile.wlan.auth_type ||
           AEEWIFI_AUTHENTICATION_8021X_TLS_RADIUS_WKEY == (AEEWIFIAuthenticationOption)profile_record->profile.wlan.auth_type )
      {
         if ( size <= sizeof( profile_record->profile.wlan.auth_info.x_info.tls.id ))
         {
            (void)STRNCPY( (char *)profile_record->profile.wlan.auth_info.x_info.tls.id,
                           (char *)option_value, size ); 
            profile_record->profile.wlan.auth_info.x_info.tls.id_len = (byte)size;
         }
         else
         {
            ret_val = FALSE;
         }
      }
      else if ( AEEWIFI_AUTHENTICATION_8021X_TLS_CERT == (AEEWIFIAuthenticationOption)profile_record->profile.wlan.auth_type )
      {
         if ( size <= sizeof( profile_record->profile.wlan.auth_info.x_info.tls_cert.id ))
         {
            (void)STRNCPY( (char *)profile_record->profile.wlan.auth_info.x_info.tls_cert.id,
                           (char *)option_value, size ); 
            profile_record->profile.wlan.auth_info.x_info.tls_cert.id_len = (byte)size;
         }
         else
         {
            ret_val = FALSE;
         }
      }
      else if ( AEEWIFI_AUTHENTICATION_8021X_MD5 == profile_record->profile.wlan.auth_type )
      {
         if ( size <= sizeof( profile_record->profile.wlan.auth_info.x_info.md5.id ))
         {
            (void)STRNCPY( (char *)profile_record->profile.wlan.auth_info.x_info.md5.id,
                           (char *)option_value, size );
            profile_record->profile.wlan.auth_info.x_info.md5.id_len = (byte)size;
         }
         else
         {
            ret_val = FALSE;
         }
      }
      else if ( AEEWIFI_AUTHENTICATION_PAP == profile_record->profile.wlan.auth_type ||
                AEEWIFI_AUTHENTICATION_CHAP == profile_record->profile.wlan.auth_type ||
                AEEWIFI_AUTHENTICATION_MSCHAPV2 == profile_record->profile.wlan.auth_type )
      {
         if ( size <= sizeof( profile_record->profile.wlan.auth_info.passwd.id ))
         {
            (void)STRNCPY( (char *)profile_record->profile.wlan.auth_info.passwd.id,
                           (char *)option_value, size );
            profile_record->profile.wlan.auth_info.passwd.id_len = (byte)size;
         }
         else
         {
            ret_val = FALSE;
         }
      }
      else if ( AEEWIFI_AUTHENTICATION_PEAP == profile_record->profile.wlan.auth_type ||
                AEEWIFI_AUTHENTICATION_TTLS == profile_record->profile.wlan.auth_type )
      {
         if ( size <= sizeof( profile_record->profile.wlan.auth_info.peap_ttls.user_id.id ))
         {
            (void)STRNCPY( (char *)profile_record->profile.wlan.auth_info.peap_ttls.user_id.id,
                           (char *)option_value, size );
            profile_record->profile.wlan.auth_info.peap_ttls.user_id.id_len = (byte)size;
         }
         else
         {
            ret_val = FALSE;
         }
      }
      else
      {
         ret_val = FALSE;
      }
      break;
    }

    case AEEWIFI_OPT_USER_ID:
    {
       size = STRLEN( (char *)option_value );

       switch (profile_record->profile.wlan.auth_type )
       {
       case AEEWIFI_AUTHENTICATION_PAP:
       case AEEWIFI_AUTHENTICATION_CHAP:
       case AEEWIFI_AUTHENTICATION_MSCHAPV2:
       case AEEWIFI_AUTHENTICATION_PEAP:
       case AEEWIFI_AUTHENTICATION_TTLS:
          if ( size <= sizeof( profile_record->profile.wlan.auth_info.user_id.id ))
          {
             (void)STRNCPY( (char *)profile_record->profile.wlan.auth_info.user_id.id,
                            (char *)option_value, size ); 
             profile_record->profile.wlan.auth_info.user_id.id_len = (byte)size;
          }
          else
          {
             ret_val = FALSE;
          }
          break;

       default:
          ret_val = FALSE;
          break;
       }
       break;
    }

    case AEEWIFI_OPT_8021X_PASSWORD:
    {
      size = STRLEN( (char *)option_value );
      if ( AEEWIFI_AUTHENTICATION_8021X_PASSWORD == profile_record->profile.wlan.auth_type )
      {
         if ( size <= sizeof( profile_record->profile.wlan.auth_info.x_info.passwd.pass ))
         {
            (void)STRNCPY( (char *)profile_record->profile.wlan.auth_info.x_info.passwd.pass,
                           (char *)option_value, size );
            profile_record->profile.wlan.auth_info.x_info.passwd.pass_len = (byte)size;
         }
         else
         {
            ret_val = FALSE;
         }
      }
      else if ( AEEWIFI_AUTHENTICATION_8021X_MD5 == profile_record->profile.wlan.auth_type )
      {
         if ( size <= sizeof( profile_record->profile.wlan.auth_info.x_info.md5.pass ))
         {
            (void)STRNCPY( (char *)profile_record->profile.wlan.auth_info.x_info.md5.pass,
                           (char *)option_value, size );
            profile_record->profile.wlan.auth_info.x_info.md5.pass_len = (byte)size;
         }
         else
         {
            ret_val = FALSE;
         }
      }
      else if ( AEEWIFI_AUTHENTICATION_PAP == profile_record->profile.wlan.auth_type ||
                AEEWIFI_AUTHENTICATION_CHAP == profile_record->profile.wlan.auth_type ||
                AEEWIFI_AUTHENTICATION_MSCHAPV2 == profile_record->profile.wlan.auth_type )
      {
         if ( size <= sizeof( profile_record->profile.wlan.auth_info.passwd.pass ))
         {
            (void)STRNCPY( (char *)profile_record->profile.wlan.auth_info.passwd.pass,
                           (char *)option_value, size );
            profile_record->profile.wlan.auth_info.passwd.pass_len = (byte)size;
         }
         else
         {
            ret_val = FALSE;
         }
      }
      else
      {
         ret_val = FALSE;
      }
      break;
    }

    case AEEWIFI_OPT_LISTEN_INTERVAL:
      tmp_value = (uint32)option_value;
      data_record->wlan.listen_interval = (word)tmp_value;
      break;

    case AEEWIFI_OPT_PREAMBLE_TYPE:
      tmp_value = (uint32)option_value;
      data_record->wlan.preamble_type = (byte)tmp_value;
      break;

    case AEEWIFI_OPT_RTSTHRESHOLD:
      tmp_value = (uint32)option_value;
      data_record->wlan.rts_threshold = (word)tmp_value;
      break;

    case AEEWIFI_OPT_SCANTIMING:
      tmp_value = (uint32)option_value;
      data_record->wlan.scan_timing = (word)tmp_value;
      break;

    case AEEWIFI_OPT_POWERLEVEL:
      tmp_value = (uint32)option_value;
      data_record->wlan.max_tx_power = (word)tmp_value;
      break;

    case AEEWIFI_OPT_FRAGMENTATIONTHRESHOLD:
      tmp_value = (uint32)option_value;
      data_record->wlan.frag_threshold = (word)tmp_value;
      break;

#if MIN_BREW_VERSIONEx(3,1,5)
    case AEEWIFI_OPT_RSSI_THRESHOLD:
      tmp_value = (uint32)option_value;
      data_record->wlan.rssi_threshold = (word)tmp_value;
      break;

    case AEEWIFI_OPT_MTU_SIZE:
      tmp_value = (uint32)option_value;
      data_record->ip.mtu_size = (word)tmp_value;
      break;
#endif

    case AEEWIFI_OPT_DHCP_ENABLED:
      tmp_value = (uint32)option_value;
      data_record->ip.dhcp_enabled = (boolean)tmp_value;
      break;

    case AEEWIFI_OPT_IP_TYPE:
      /* currently only IPv4 is supported by wlan, store this setting locally */
      pMe->u32_ip_type = (uint32)option_value;
      break;

    case AEEWIFI_OPT_STATIC_IP:
      ret_val = ConvertStrToIp( (char *)option_value, &tmp_value );
      data_record->ip.static_ip_addr = tmp_value;
      break;

    case AEEWIFI_OPT_DNS_ADDRESS:
      ret_val = ConvertStrToIp( (char *)option_value, &tmp_value );
      data_record->ip.pri_dns_addr = tmp_value;
      break;

    case AEEWIFI_OPT_SUBNET_MASK:
      ret_val = ConvertStrToIp( (char *)option_value, &tmp_value );
      data_record->ip.subnet_mask = tmp_value;
      break;

    case AEEWIFI_OPT_DEFAULT_GATEWAY:
      ret_val = ConvertStrToIp( (char *)option_value, &tmp_value );
      data_record->ip.default_gateway = tmp_value;
      break;

#if MIN_BREW_VERSIONEx(3,1,5)
    case AEEWIFI_OPT_SECONDARY_DNS_ADDRESS:
      ret_val = ConvertStrToIp( (char *)option_value, &tmp_value );
      data_record->ip.sec_dns_addr = tmp_value;
      break;
    
    case AEEWIFI_OPT_DOMAIN_NAME:
      if (( WLAN_CONFIG_DOMAIN_NAME_MAX_SIZE  <= STRLEN( (char *)option_value )) ||
          ( (size_t)-1 == STRLCPY( (char *)data_record->ip.domain_name, (char *)option_value, WLAN_CONFIG_DOMAIN_NAME_MAX_SIZE )))
      {
         ret_val = FALSE;
      }
      break;
#endif

    default:
      ret_val = FALSE;
      break;
  }

  return ret_val;
}

/*===========================================================================

FUNCTION: OEMWIFI_Get_OptVal

DESCRIPTION
  Takes the option ID and returns the value which should be saved in the IWEBOPT 
  interface.

DEPENDENCIES
  none

RETURN VALUE
  NULL    if any of the parameterd are NULL pointers,
          or net data cannot be retrieved
  otherwise requested option value

SIDE EFFECTS
===========================================================================*/
static void *OEMWIFI_Get_OptVal( IWIFIOpts *pMe, int32 option_id,
                                 net_sys_rec_s_type *system_record,
                                 net_profile_s_type *profile_record )
{
  void                 *ret_val = (void *)NULL;
  data_settings_s_type *data_record;
  boolean               sysoption = TRUE;

  data_record = IWIFICoreObj->pDataRecord;

  if ( NULL == system_record )
  {
     sysoption = FALSE;
  }
  else
  {
    switch( option_id )
    {
      case AEEWIFI_OPT_PROFILE_ID:
        ret_val = (void *)((uint32)system_record->sys_rec.wlan.profile_id);
        break;

      case AEEWIFI_OPT_TECH:
        ret_val = (void *)((uint32)system_record->sys_rec.wlan.tech);
        break;

      case AEEWIFI_OPT_BANDPREF:
        ret_val = (void *)((uint32)system_record->sys_rec.wlan.band);
        if ( NET_WLAN_BAND_ANY == (uint32)ret_val )
        {
          ret_val = (void *)(uint32)AEEWIFI_BAND_ANY;
        }
        break;

      case AEEWIFI_OPT_CHANNEL:
        ret_val = (void *)GetChannelFromMask( system_record->sys_rec.wlan.chan );
        break;

      case AEEWIFI_OPT_GROUPS:
        ret_val = (void *)((uint32)system_record->sys_rec.wlan.groups);
        break;

      case AEEWIFI_OPT_WLAN_MODE:
        ret_val = (void *)((uint32)system_record->sys_rec.wlan.mode);
        break;

      case AEEWIFI_OPT_SSID:
        if (system_record->sys_rec.wlan.ssid.size == 0)
        {
          (void)MEMCPY(system_record->sys_rec.wlan.ssid.ssid, DEFAULT_SSID, DEFAULT_SSID_SZ );
          system_record->sys_rec.wlan.ssid.size = (byte)DEFAULT_SSID_SZ;
        }

        ret_val = (void *)DuplicateStr( (char *)system_record->sys_rec.wlan.ssid.ssid,
                                        system_record->sys_rec.wlan.ssid.size );
        break;

      default:
         sysoption = FALSE;
         break;
    }
  }

  if ( TRUE == sysoption )
  {
     return ret_val;
  }

  // non system record options
  switch( option_id )
  {
    case AEEWIFI_OPT_ENCRYPTION_TYPE:

      ret_val = (void *)((uint32)profile_record->profile.wlan.encryp_type);
      break;

    case AEEWIFI_OPT_AUTHENTICATION_TYPE:
      ret_val = (void *)((uint32)profile_record->profile.wlan.auth_type );

      if ( AEEWIFI_AUTHENTICATION_PEAP == profile_record->profile.wlan.auth_type ||
           AEEWIFI_AUTHENTICATION_TTLS == profile_record->profile.wlan.auth_type )
      {
          // continue processing the nested profile
          if ( NET_STATUS_SUCCESS != net_get_profile( profile_record->profile.wlan.auth_info.peap_ttls.nested.profile_id, profile_record, &IWIFICoreObj->Parser ))
          {
             OEMWIFI_Get_DefaultProfileRec( profile_record, profile_record->profile.wlan.auth_info.peap_ttls.nested.profile_id );
          }
      }
      break;

    case AEEWIFI_OPT_MOB_IP_PROFILE_ID:
      ret_val = (void *)((uint32)profile_record->profile.wlan.mob_ip_profile_id );
      break;

    case AEEWIFI_OPT_NESTED_AUTH_ID:
       ret_val = (void *)((uint32)profile_record->profile.wlan.auth_info.peap_ttls.nested.profile_id );
       break;

    case AEEWIFI_OPT_CERTIFICATE_ID:
       if ( AEEWIFI_AUTHENTICATION_8021X_TLS_CERT == profile_record->profile.wlan.auth_type )
       {
          ret_val = (void *)((uint32)profile_record->profile.wlan.auth_info.x_info.tls_cert.cert_id );
       }
       else if ( AEEWIFI_AUTHENTICATION_PEAP == profile_record->profile.wlan.auth_type ||
                 AEEWIFI_AUTHENTICATION_TTLS == profile_record->profile.wlan.auth_type )
       {
          ret_val = (void *)((uint32)profile_record->profile.wlan.auth_info.peap_ttls.cert_id );
       }
       break;

    case AEEWIFI_OPT_WEP_KEY_ID:
      ret_val = (void *)((uint32)profile_record->profile.wlan.auth_info.wep_104_bit_info.default_key );
      break;

    case AEEWIFI_OPT_WEP_KEY:
    {
      uint8 size;

      if ( AEEWIFI_ENCRYPTION_WEP_128bit == profile_record->profile.wlan.encryp_type )
      {
         size = (uint8)sizeof( profile_record->profile.wlan.auth_info.wep_104_bit_info.key1 );

         switch ( profile_record->profile.wlan.auth_info.wep_104_bit_info.default_key )
         {
         case 1:
            ret_val = ConvertBinBufToStr( profile_record->profile.wlan.auth_info.wep_104_bit_info.key2, size );
            break;

         case 2:
            ret_val = ConvertBinBufToStr( profile_record->profile.wlan.auth_info.wep_104_bit_info.key3, size );
            break;

         case 3:
            ret_val = ConvertBinBufToStr( profile_record->profile.wlan.auth_info.wep_104_bit_info.key4, size );
            break;

         case 0:
         default:
            ret_val = ConvertBinBufToStr( profile_record->profile.wlan.auth_info.wep_104_bit_info.key1, size );
            break;
         }
      }
      else if ( AEEWIFI_ENCRYPTION_WEP_64bit == profile_record->profile.wlan.encryp_type )
      {
         size = (uint8)sizeof( profile_record->profile.wlan.auth_info.wep_40_bit_info.key1 );

         switch ( profile_record->profile.wlan.auth_info.wep_40_bit_info.default_key )
         {
         case 1:
            ret_val = ConvertBinBufToStr( profile_record->profile.wlan.auth_info.wep_40_bit_info.key2, size );
            break;

         case 2:
            ret_val = ConvertBinBufToStr( profile_record->profile.wlan.auth_info.wep_40_bit_info.key3, size );
            break;

         case 3:
            ret_val = ConvertBinBufToStr( profile_record->profile.wlan.auth_info.wep_40_bit_info.key4, size );
            break;

         case 0:
         default:
            ret_val = ConvertBinBufToStr( profile_record->profile.wlan.auth_info.wep_40_bit_info.key1, size );
            break;
         }
      }
      else
      {
         ret_val = (void *)DuplicateStr( "", 0 );
      }
      break;
    }

    case AEEWIFI_OPT_WEP_64BIT_KEY_ID:
      if ( AEEWIFI_ENCRYPTION_WEP_64bit == profile_record->profile.wlan.encryp_type )
      {
         ret_val = (void *)((uint32)profile_record->profile.wlan.auth_info.wep_40_bit_info.default_key);
      }
      else
      {
         ret_val = (void *)DuplicateStr( "", 0 );
      }
      break;

    case AEEWIFI_OPT_WEP_64BIT_KEY_0:
      if ( AEEWIFI_ENCRYPTION_WEP_64bit == profile_record->profile.wlan.encryp_type )
      {
         ret_val = ConvertBinBufToStr( profile_record->profile.wlan.auth_info.wep_40_bit_info.key1, 
                                       (uint8)sizeof( profile_record->profile.wlan.auth_info.wep_40_bit_info.key1 ));
      }
      else
      {
         ret_val = (void *)DuplicateStr( "", 0 );
      }
      break;

    case AEEWIFI_OPT_WEP_64BIT_KEY_1:
      if ( AEEWIFI_ENCRYPTION_WEP_64bit == profile_record->profile.wlan.encryp_type )
      {
         ret_val = ConvertBinBufToStr( profile_record->profile.wlan.auth_info.wep_40_bit_info.key2, 
                                       (uint8)sizeof( profile_record->profile.wlan.auth_info.wep_40_bit_info.key2 ));
      }
      else
      {
         ret_val = (void *)DuplicateStr( "", 0 );
      }
      break;

    case AEEWIFI_OPT_WEP_64BIT_KEY_2:
      if ( AEEWIFI_ENCRYPTION_WEP_64bit == profile_record->profile.wlan.encryp_type )
      {
         ret_val = ConvertBinBufToStr( profile_record->profile.wlan.auth_info.wep_40_bit_info.key3, 
                                       (uint8)sizeof( profile_record->profile.wlan.auth_info.wep_40_bit_info.key3 ));
      }
      else
      {
         ret_val = (void *)DuplicateStr( "", 0 );
      }
      break;

    case AEEWIFI_OPT_WEP_64BIT_KEY_3:
      if ( AEEWIFI_ENCRYPTION_WEP_64bit == profile_record->profile.wlan.encryp_type )
      {
         ret_val = ConvertBinBufToStr( profile_record->profile.wlan.auth_info.wep_40_bit_info.key4, 
                                       (uint8)sizeof( profile_record->profile.wlan.auth_info.wep_40_bit_info.key4 ));
      }
      else
      {
         ret_val = (void *)DuplicateStr( "", 0 );
      }
      break;

    case AEEWIFI_OPT_WEP_128BIT_KEY_ID:
      if ( AEEWIFI_ENCRYPTION_WEP_128bit == profile_record->profile.wlan.encryp_type )
      {
         ret_val = (void *)((uint32)profile_record->profile.wlan.auth_info.wep_104_bit_info.default_key);
      }
      break;

    case AEEWIFI_OPT_WEP_128BIT_KEY_0:
      if ( AEEWIFI_ENCRYPTION_WEP_128bit == profile_record->profile.wlan.encryp_type )
      {
         ret_val = ConvertBinBufToStr( profile_record->profile.wlan.auth_info.wep_104_bit_info.key1, 
                                       (uint8)sizeof( profile_record->profile.wlan.auth_info.wep_104_bit_info.key1 ));
      }
      else
      {
         ret_val = (void *)DuplicateStr( "", 0 );
      }
      break;

    case AEEWIFI_OPT_WEP_128BIT_KEY_1:
      if ( AEEWIFI_ENCRYPTION_WEP_128bit == profile_record->profile.wlan.encryp_type )
      {
         ret_val = ConvertBinBufToStr( profile_record->profile.wlan.auth_info.wep_104_bit_info.key2, 
                                       (uint8)sizeof( profile_record->profile.wlan.auth_info.wep_104_bit_info.key2 ));
      }
      else
      {
         ret_val = (void *)DuplicateStr( "", 0 );
      }
      break;

    case AEEWIFI_OPT_WEP_128BIT_KEY_2:
      if ( AEEWIFI_ENCRYPTION_WEP_128bit == profile_record->profile.wlan.encryp_type )
      {
         ret_val = ConvertBinBufToStr( profile_record->profile.wlan.auth_info.wep_104_bit_info.key3, 
                                       (uint8)sizeof( profile_record->profile.wlan.auth_info.wep_104_bit_info.key3 ));
      }
      else
      {
         ret_val = (void *)DuplicateStr( "", 0 );
      }
      break;

    case AEEWIFI_OPT_WEP_128BIT_KEY_3:
      if ( AEEWIFI_ENCRYPTION_WEP_128bit == profile_record->profile.wlan.encryp_type )
      {
         ret_val = ConvertBinBufToStr( profile_record->profile.wlan.auth_info.wep_104_bit_info.key4, 
                                       (uint8)sizeof( profile_record->profile.wlan.auth_info.wep_104_bit_info.key4 ));
      }
      else
      {
         ret_val = (void *)DuplicateStr( "", 0 );
      }
      break;

    case AEEWIFI_OPT_8021X_ID:
    {
      if ( AEEWIFI_AUTHENTICATION_8021X_TLS == (AEEWIFIAuthenticationOption)profile_record->profile.wlan.auth_type ||
           AEEWIFI_AUTHENTICATION_8021X_TLS_RADIUS_WKEY == (AEEWIFIAuthenticationOption)profile_record->profile.wlan.auth_type )
      {
         ret_val = (void *)DuplicateStr( (char *) profile_record->profile.wlan.auth_info.x_info.tls.id, 
                                                    profile_record->profile.wlan.auth_info.x_info.tls.id_len ); 
      }
      else if ( AEEWIFI_AUTHENTICATION_8021X_TLS_CERT == (AEEWIFIAuthenticationOption)profile_record->profile.wlan.auth_type )
      {
         ret_val = (void *)DuplicateStr( (char *) profile_record->profile.wlan.auth_info.x_info.tls_cert.id, 
                                                    profile_record->profile.wlan.auth_info.x_info.tls_cert.id_len );
      }
      else if ( AEEWIFI_AUTHENTICATION_8021X_MD5 == profile_record->profile.wlan.auth_type )
      {
         ret_val = (void *)DuplicateStr( (char *) profile_record->profile.wlan.auth_info.x_info.md5.id, 
                                                    profile_record->profile.wlan.auth_info.x_info.md5.id_len );
      }
      else if ( AEEWIFI_AUTHENTICATION_PAP == profile_record->profile.wlan.auth_type ||
                AEEWIFI_AUTHENTICATION_CHAP == profile_record->profile.wlan.auth_type ||
                AEEWIFI_AUTHENTICATION_MSCHAPV2 == profile_record->profile.wlan.auth_type )
      {
         ret_val = (void *)DuplicateStr( (char *) profile_record->profile.wlan.auth_info.passwd.id, 
                                                    profile_record->profile.wlan.auth_info.passwd.id_len );
      }
      else if ( AEEWIFI_AUTHENTICATION_PEAP == profile_record->profile.wlan.auth_type ||
                AEEWIFI_AUTHENTICATION_TTLS == profile_record->profile.wlan.auth_type )
      {
         ret_val = (void *)DuplicateStr( (char *) profile_record->profile.wlan.auth_info.peap_ttls.user_id.id, 
                                                    profile_record->profile.wlan.auth_info.peap_ttls.user_id.id_len );
      }
      else
      {
         ret_val = (void *)DuplicateStr( "", 0 );
      }
      break;
    }

    case AEEWIFI_OPT_USER_ID:
    {
       switch (profile_record->profile.wlan.auth_type )
       {
       case AEEWIFI_AUTHENTICATION_PAP:
       case AEEWIFI_AUTHENTICATION_CHAP:
       case AEEWIFI_AUTHENTICATION_MSCHAPV2:
       case AEEWIFI_AUTHENTICATION_PEAP:
       case AEEWIFI_AUTHENTICATION_TTLS:
          ret_val = (void *)DuplicateStr( (char *) profile_record->profile.wlan.auth_info.user_id.id, 
                                                     profile_record->profile.wlan.auth_info.user_id.id_len );
          break;

       default:
          ret_val = (void *)DuplicateStr( "", 0 );
          break;
       }
       break;
    }

    case AEEWIFI_OPT_8021X_PASSWORD:
    {
      if ( AEEWIFI_AUTHENTICATION_8021X_PASSWORD == profile_record->profile.wlan.auth_type )
      {
         ret_val = (void *)DuplicateStr( (char *) profile_record->profile.wlan.auth_info.x_info.passwd.pass, 
                                                    profile_record->profile.wlan.auth_info.x_info.passwd.pass_len );
      }
      else if ( AEEWIFI_AUTHENTICATION_8021X_MD5 == profile_record->profile.wlan.auth_type )
      {
         ret_val = (void *)DuplicateStr( (char *) profile_record->profile.wlan.auth_info.x_info.md5.pass, 
                                                    profile_record->profile.wlan.auth_info.x_info.md5.pass_len );
      }
      else if ( AEEWIFI_AUTHENTICATION_PAP == profile_record->profile.wlan.auth_type ||
                AEEWIFI_AUTHENTICATION_CHAP == profile_record->profile.wlan.auth_type ||
                AEEWIFI_AUTHENTICATION_MSCHAPV2 == profile_record->profile.wlan.auth_type )
      {
         ret_val = (void *)DuplicateStr( (char *) profile_record->profile.wlan.auth_info.passwd.pass, 
                                                    profile_record->profile.wlan.auth_info.passwd.pass_len );
      }
      else
      {
         ret_val = (void *)DuplicateStr( "", 0 );
      }
      break;
    }
    case AEEWIFI_OPT_LISTEN_INTERVAL:
      ret_val = (void *)((uint32)data_record->wlan.listen_interval);
      break;

    case AEEWIFI_OPT_PREAMBLE_TYPE:
      ret_val = (void *)((uint32)data_record->wlan.preamble_type);
      break;

    case AEEWIFI_OPT_RTSTHRESHOLD:
      ret_val = (void *)((uint32)data_record->wlan.rts_threshold);
      break;

    case AEEWIFI_OPT_SCANTIMING:
      ret_val = (void *)((uint32)data_record->wlan.scan_timing);
      break;

    case AEEWIFI_OPT_POWERLEVEL:
      ret_val = (void *)((uint32)data_record->wlan.max_tx_power);
      break;

    case AEEWIFI_OPT_FRAGMENTATIONTHRESHOLD:
      ret_val = (void *)((uint32)data_record->wlan.frag_threshold);
      break;

#if MIN_BREW_VERSIONEx(3,1,5)
    case AEEWIFI_OPT_RSSI_THRESHOLD:
      ret_val = (void *)((uint32)data_record->wlan.rssi_threshold);
      break;

    case AEEWIFI_OPT_MTU_SIZE:
      ret_val = (void *)((uint32)data_record->ip.mtu_size);
      break;
#endif

    case AEEWIFI_OPT_DHCP_ENABLED:
      ret_val = (void *)((uint32)data_record->ip.dhcp_enabled);
      break;

    case AEEWIFI_OPT_IP_TYPE:
      ret_val = (void *) pMe->u32_ip_type;
      break;

    case AEEWIFI_OPT_STATIC_IP:
      ret_val = (void *)ConvertIpToStr( data_record->ip.static_ip_addr );
      break;

    case AEEWIFI_OPT_DNS_ADDRESS:
      ret_val = (void *)ConvertIpToStr( data_record->ip.pri_dns_addr );
      break;

    case AEEWIFI_OPT_SUBNET_MASK:
      ret_val = (void *)ConvertIpToStr( data_record->ip.subnet_mask );
      break;

    case AEEWIFI_OPT_DEFAULT_GATEWAY:
      ret_val = (void *)ConvertIpToStr( data_record->ip.default_gateway );
      break;

#if MIN_BREW_VERSIONEx(3,1,5)
    case AEEWIFI_OPT_SECONDARY_DNS_ADDRESS:
      ret_val = (void *)ConvertIpToStr( data_record->ip.sec_dns_addr );
      break;

    case AEEWIFI_OPT_DOMAIN_NAME:
      ret_val = (void *)DuplicateStr( (char *)data_record->ip.domain_name, 
                                      STRLEN( (char *)data_record->ip.domain_name ));
      break;
#endif

    default:
      break;
  }

  return ret_val;
}

/*===========================================================================

FUNCTION: OEMWIFI_Get_SysProfileRec

DESCRIPTION
  This function will get current system record and profile record from IWIFICoreObj

DEPENDENCIES
  none

RETURN VALUE
  TRUE if successful
  FALSE if failed

SIDE EFFECTS
  None

===========================================================================*/
static boolean OEMWIFI_Get_SysProfileRec( uint32 profile_id,
                                          net_sys_rec_s_type *system_record,
                                          net_profile_s_type *profile_record )
{
  word              i = 0;
  net_status_e_type Status;

  for ( ;; ) // find system record by profile id.
  {
     Status =  net_get_sys_rec( i++, system_record, &IWIFICoreObj->Parser );
     if ( NET_STATUS_SUCCESS == Status )
     {
        if ( system_record->sys_rec.wlan.profile_id == (byte)profile_id )
        {
           Status  = net_get_profile( (byte)profile_id, profile_record, &IWIFICoreObj->Parser );

           if ( NET_STATUS_SUCCESS !=  Status )
           {
              MSG_ERROR( "net_get_profile: error %d", Status, 0, 0 );
              return FALSE;
           }
           return TRUE;
        }
     }
     else
     {
        MSG_ERROR( "net_get_sys_rec: error %d", Status, 0, 0 );
        return FALSE;
     }
  }

  return FALSE;
}

/*===========================================================================

FUNCTION: OEMWIFI_LoadNestedOptions

DESCRIPTION
  This function will take the given IWIFIOption interface and fill it with
  the current nested options.

DEPENDENCIES
  none

RETURN VALUE
  IWEBOPTS_AddOpt   return values

SIDE EFFECTS
  None

===========================================================================*/
#define SETOPT( s )                                                    \
   tmp[ 0 ].nId = s;                                                   \
   tmp[ 0 ].pVal = OEMWIFI_Get_OptVal( NULL, s, NULL, profile_record );\
   if ( SUCCESS != ( ret = IWEBOPTS_AddOpt( options, tmp )))           \
   {                                                                   \
      MSG_ERROR( "Could not add option 0x%x, Error 0x%x", s, ret, 0 ); \
      return ret;                                                      \
   }

static int OEMWIFI_LoadNestedOptions( net_profile_s_type *profile_record,
                                      IWebOpts *options )
{
   int     ret;       /* Return value. */
   WebOpt  tmp[ 2 ] = { 0, NULL, WEBOPT_END, NULL };

   SETOPT( AEEWIFI_OPT_AUTHENTICATION_TYPE )
   SETOPT( AEEWIFI_OPT_ENCRYPTION_TYPE )
   SETOPT( AEEWIFI_OPT_MOB_IP_PROFILE_ID )

   switch ( profile_record->profile.wlan.auth_type )
   {
   case AEEWIFI_AUTHENTICATION_WEP:
      if ( AEEWIFI_ENCRYPTION_WEP_64bit == profile_record->profile.wlan.encryp_type )
      {
         SETOPT( AEEWIFI_OPT_WEP_64BIT_KEY_ID )

         SETOPT( AEEWIFI_OPT_WEP_64BIT_KEY_0 )
         FREEIF( tmp[ 0 ].pVal );

         SETOPT( AEEWIFI_OPT_WEP_64BIT_KEY_1 )
         FREEIF( tmp[ 0 ].pVal );

         SETOPT( AEEWIFI_OPT_WEP_64BIT_KEY_2 )
         FREEIF( tmp[ 0 ].pVal );

         SETOPT( AEEWIFI_OPT_WEP_64BIT_KEY_3 )
         FREEIF( tmp[ 0 ].pVal );
      }
      else
      {
         SETOPT( AEEWIFI_OPT_WEP_128BIT_KEY_ID )

         SETOPT( AEEWIFI_OPT_WEP_128BIT_KEY_0 )
         FREEIF( tmp[ 0 ].pVal );

         SETOPT( AEEWIFI_OPT_WEP_128BIT_KEY_1 )
         FREEIF( tmp[ 0 ].pVal );

         SETOPT( AEEWIFI_OPT_WEP_128BIT_KEY_2 )
         FREEIF( tmp[ 0 ].pVal );

         SETOPT( AEEWIFI_OPT_WEP_128BIT_KEY_3 )
         FREEIF( tmp[ 0 ].pVal );
      }
      break;

   case AEEWIFI_AUTHENTICATION_8021X_PASSWORD:
      SETOPT( AEEWIFI_OPT_8021X_PASSWORD )
      FREEIF( tmp[ 0 ].pVal );
      break;

   case AEEWIFI_AUTHENTICATION_8021X_TLS:
   case AEEWIFI_AUTHENTICATION_8021X_TLS_RADIUS_WKEY:
      SETOPT( AEEWIFI_OPT_8021X_ID )
      FREEIF( tmp[ 0 ].pVal );
      break;

   case AEEWIFI_AUTHENTICATION_8021X_MD5:
      SETOPT( AEEWIFI_OPT_8021X_ID )
      FREEIF( tmp[ 0 ].pVal );

      SETOPT( AEEWIFI_OPT_8021X_PASSWORD )
      FREEIF( tmp[ 0 ].pVal );
      break;

   case AEEWIFI_AUTHENTICATION_8021X_TLS_CERT:
      SETOPT( AEEWIFI_OPT_8021X_ID )
      FREEIF( tmp[ 0 ].pVal );

      SETOPT( AEEWIFI_OPT_CERTIFICATE_ID )
      break;

     case AEEWIFI_AUTHENTICATION_PAP:
     case AEEWIFI_AUTHENTICATION_CHAP:
     case AEEWIFI_AUTHENTICATION_MSCHAPV2:
      SETOPT( AEEWIFI_OPT_USER_ID )
      FREEIF( tmp[ 0 ].pVal );

      SETOPT( AEEWIFI_OPT_8021X_ID )
      FREEIF( tmp[ 0 ].pVal );

      SETOPT( AEEWIFI_OPT_8021X_PASSWORD )
      FREEIF( tmp[ 0 ].pVal );
      break;

   case AEEWIFI_AUTHENTICATION_PEAP:
   case AEEWIFI_AUTHENTICATION_TTLS:

      SETOPT( AEEWIFI_OPT_USER_ID )
      FREEIF( tmp[ 0 ].pVal );

      SETOPT( AEEWIFI_OPT_8021X_ID )
      FREEIF( tmp[ 0 ].pVal );

      SETOPT( AEEWIFI_OPT_CERTIFICATE_ID ) 
      SETOPT( AEEWIFI_OPT_NESTED_AUTH_ID )

      ret = OEMWIFI_LoadNestedOptions( profile_record, options );
      break;

   case AEEWIFI_AUTHENTICATION_SIM:
   case AEEWIFI_AUTHENTICATION_AKA:
   case AEEWIFI_AUTHENTICATION_NONE:
   default:
      ret = SUCCESS;
      break;
   }

   return ret;
}

/*===========================================================================

FUNCTION: OEMWIFI_LoadOptions

DESCRIPTION
  This function will take the given IWIFIOption interface and fill it with
  the current options.

DEPENDENCIES
  none

RETURN VALUE
  BADPARM           if any of the input pointers are NULL
  IWEBOPTS_AddOpt   return values otherwise

SIDE EFFECTS
  None

===========================================================================*/
static int OEMWIFI_LoadOptions( IWIFI *pMe, uint32 id, IWIFIOpts *options )
{
#if MIN_BREW_VERSIONEx(3,1,5)
#define AUTHINDEX 42
#else
#define AUTHINDEX 38
#endif
  int                   i;         /* Control variable for loop */
  int                   ret;       /* Return value. */
  net_sys_rec_s_type    system_record;
  net_profile_s_type    profile_record;
  IWebOpts             *pTmpOptions;
  WebOpt                tmp[ 2 ] = { 0, NULL, WEBOPT_END, NULL };
  int                   location = 0;
#ifdef MULTIPROF
#ifndef AEE_SIMULATOR
  AEECMPhInfo           phInfo;
  AEECMSSInfo           ssInfo;
#endif
#endif

  static WebOpt values_to_set[] =      /* Values to be put into options. */
  {{AEEWIFI_OPT_NESTED_AUTH_ID,         NULL},  // always first to get, last to set
   {AEEWIFI_OPT_PROFILE_ID,             NULL},
   {AEEWIFI_OPT_TECH,                   NULL},
   {AEEWIFI_OPT_BANDPREF,               NULL},
   {AEEWIFI_OPT_CHANNEL,                NULL},
   {AEEWIFI_OPT_GROUPS,                 NULL}, 
   {AEEWIFI_OPT_WLAN_MODE,              NULL},
   {AEEWIFI_OPT_SSID,                   NULL},
   {AEEWIFI_OPT_MOB_IP_PROFILE_ID,      NULL},
   {AEEWIFI_OPT_CERTIFICATE_ID,         NULL},
   {AEEWIFI_OPT_WEP_KEY_ID,             NULL}, // 10
   {AEEWIFI_OPT_WEP_KEY,                NULL},
   {AEEWIFI_OPT_WEP_64BIT_KEY_ID,       NULL},  
   {AEEWIFI_OPT_WEP_64BIT_KEY_0,        NULL},
   {AEEWIFI_OPT_WEP_64BIT_KEY_1,        NULL},
   {AEEWIFI_OPT_WEP_64BIT_KEY_2,        NULL},
   {AEEWIFI_OPT_WEP_64BIT_KEY_3,        NULL},
   {AEEWIFI_OPT_WEP_128BIT_KEY_ID,      NULL},
   {AEEWIFI_OPT_WEP_128BIT_KEY_0,       NULL},
   {AEEWIFI_OPT_WEP_128BIT_KEY_1,       NULL},
   {AEEWIFI_OPT_WEP_128BIT_KEY_2,       NULL}, // 20
   {AEEWIFI_OPT_WEP_128BIT_KEY_3,       NULL},
   {AEEWIFI_OPT_8021X_ID,               NULL},
   {AEEWIFI_OPT_8021X_PASSWORD,         NULL},
   {AEEWIFI_OPT_USER_ID,                NULL},
   {AEEWIFI_OPT_LISTEN_INTERVAL,        NULL},
   {AEEWIFI_OPT_PREAMBLE_TYPE,          NULL},
   {AEEWIFI_OPT_RTSTHRESHOLD,           NULL},
   {AEEWIFI_OPT_SCANTIMING,             NULL},
   {AEEWIFI_OPT_POWERLEVEL,             NULL},
   {AEEWIFI_OPT_FRAGMENTATIONTHRESHOLD, NULL}, // 30
#if MIN_BREW_VERSIONEx(3,1,5)
   {AEEWIFI_OPT_RSSI_THRESHOLD,         NULL},
   {AEEWIFI_OPT_MTU_SIZE,               NULL},
#endif
   {AEEWIFI_OPT_DHCP_ENABLED,           NULL},
   {AEEWIFI_OPT_IP_TYPE,                NULL},
   {AEEWIFI_OPT_STATIC_IP,              NULL},
   {AEEWIFI_OPT_DNS_ADDRESS,            NULL},
   {AEEWIFI_OPT_SUBNET_MASK,            NULL},
   {AEEWIFI_OPT_DEFAULT_GATEWAY,        NULL},
#if MIN_BREW_VERSIONEx(3,1,5)
   {AEEWIFI_OPT_SECONDARY_DNS_ADDRESS,  NULL}, 
   {AEEWIFI_OPT_DOMAIN_NAME,            NULL}, // 40
#endif
   {AEEWIFI_OPT_ENCRYPTION_TYPE,        NULL},
   {AEEWIFI_OPT_AUTHENTICATION_TYPE,    NULL}, // AUTHINDEX, always last to get, first to set
   {WEBOPT_END,                         NULL}
  };

  MSG_HIGH( "LoadOptions called: profile id %d", id, 0, 0 );
  if ( NULL == options )
  {
     MSG_ERROR( "OEMWIFI_LoadOptions: NULL parameters - options 0x%x", (uint32)options, 0, 0 );
     return EBADPARM;
  }
  else if ( NULL == options->pIWebOpts )
  {
     MSG_ERROR( "OEMWIFI_LoadOptions: NULL parameters - pIWebOpts 0x%x", (uint32)options->pIWebOpts, 0, 0 );
     return EBADPARM;
  }

  if (( NULL == IWIFICoreObj ))
  {
     MSG_ERROR( "OEMWIFI_LoadOptions: NULL IWIFICoreObj", 0, 0, 0 );

     return EFAILED;
  }

  if (( NULL == IWIFICoreObj->pBuffer ) || ( NULL == IWIFICoreObj->pDataRecord ))
  {
     MSG_ERROR( "OEMWIFI_LoadOptions: NULL values - pBuffer 0x%x, pDataRecord 0x%x", (uint32)(IWIFICoreObj->pBuffer), (uint32)(IWIFICoreObj->pDataRecord), 0 );

     return EFAILED;
  }

  (void)MEMSET( (void *)&system_record, 0, sizeof( net_sys_rec_s_type ));
  (void)MEMSET( (void *)&profile_record, 0, sizeof( net_profile_s_type ));

#ifdef MULTIPROF
#ifndef AEE_SIMULATOR
  if ( 0 == id )
  {
     if ( SUCCESS != ICM_GetPhoneInfo(IWIFICoreObj->pICM, &phInfo, sizeof(AEECMPhInfo)) ||
          SUCCESS != ICM_GetSSInfo(IWIFICoreObj->pICM,  &ssInfo, sizeof(AEECMSSInfo)))
     {
        MSG_ERROR( "OEMWIFI_LoadOptions: Cannot read settings", 0, 0, 0 );
        return EFAILED;
     }

     if ( AEECM_SRV_STATUS_NO_SRV == ssInfo.wlan_srv_status ||
          FALSE == ssInfo.is_registered ||
          FALSE == phInfo.is_wlan_subscription_available )
     {
        return SUCCESS; // no current profile
     }

     system_record.len = sizeof( net_sys_rec_s_type );
     system_record.type = NET_REC_TYPE_WLAN;
     system_record.sys_rec.wlan.tech = (byte)ssInfo.wlan_bss_info.tech;

     switch(ssInfo.wlan_bss_info.band)
     {
     case AEECM_SYS_BAND_WLAN_US_2400:
        system_record.sys_rec.wlan.band = (byte)NET_WLAN_BAND_US_2400;
        break;

     case AEECM_SYS_BAND_WLAN_JAPAN_2400:
        system_record.sys_rec.wlan.band = (byte)NET_WLAN_BAND_JAPAN_2400;
        break;

     case AEECM_SYS_BAND_WLAN_EUROPE_2400:
        system_record.sys_rec.wlan.band = (byte)NET_WLAN_BAND_EUROPE_2400;
        break;

     case AEECM_SYS_BAND_WLAN_FRANCE_2400:
        system_record.sys_rec.wlan.band = (byte)NET_WLAN_BAND_FRANCE_2400;
        break;

     case AEECM_SYS_BAND_WLAN_SPAIN_2400:
        system_record.sys_rec.wlan.band = (byte)NET_WLAN_BAND_SPAIN_2400;
        break;

     case AEECM_SYS_BAND_WLAN_US_5000:
        system_record.sys_rec.wlan.band = (byte)NET_WLAN_BAND_US_5000;
        break;

     case AEECM_SYS_BAND_WLAN_JAPAN_5000:
        system_record.sys_rec.wlan.band = (byte)NET_WLAN_BAND_JAPAN_5000;
        break;

     case AEECM_SYS_BAND_WLAN_EUROPE_5000:
        system_record.sys_rec.wlan.band = (byte)NET_WLAN_BAND_EUROPE_5000;
        break;

     case AEECM_SYS_BAND_WLAN_FRANCE_5000:
        system_record.sys_rec.wlan.band = (byte)NET_WLAN_BAND_FRANCE_5000;
        break;

     case AEECM_SYS_BAND_WLAN_SPAIN_5000:
        system_record.sys_rec.wlan.band = (byte)NET_WLAN_BAND_SPAIN_5000;
        break;

     default:
        system_record.sys_rec.wlan.band = (byte)NET_WLAN_BAND_NONE;
        break;
     }

     system_record.sys_rec.wlan.chan = (word)ssInfo.wlan_bss_info.chan;
     system_record.sys_rec.wlan.groups = 0; 
     system_record.sys_rec.wlan.mode = (byte)ssInfo.wlan_bss_info.bss_type;
     system_record.sys_rec.wlan.profile_id = 0;
     (void)WSTRTOSTR( ssInfo.wlan_bss_info.ssid, (char *)system_record.sys_rec.wlan.ssid.ssid, NET_WLAN_SSID_MAX_SIZE );
     system_record.sys_rec.wlan.ssid.size = STRLEN( (char *)system_record.sys_rec.wlan.ssid.ssid );

     profile_record.len = sizeof( net_profile_s_type );
     profile_record.profile_id = 0;
     profile_record.type = NET_PROFILE_TYPE_WLAN;
     profile_record.profile.wlan.encryp_type = phInfo.m_wlan_pref.encrypt;
     profile_record.profile.wlan.auth_type = phInfo.m_wlan_pref.auth;
     profile_record.profile.wlan.mob_ip_profile_id = 0;

     switch ( profile_record.profile.wlan.auth_type )
     {
     case NET_AUTH_TYPE_WEP:
        if ( NET_ENCRY_TYPE_WEP_40_BIT == profile_record.profile.wlan.encryp_type )
        {
           profile_record.profile.wlan.auth_info.wep_40_bit_info.default_key = phInfo.m_wlan_pref.auth_info.wep64_info.default_key;
           (void)MEMCPY( profile_record.profile.wlan.auth_info.wep_40_bit_info.key1, phInfo.m_wlan_pref.auth_info.wep64_info.key1, NET_40_BIT_WEP_KEY_SIZE );
           (void)MEMCPY( profile_record.profile.wlan.auth_info.wep_40_bit_info.key2, phInfo.m_wlan_pref.auth_info.wep64_info.key2, NET_40_BIT_WEP_KEY_SIZE );
           (void)MEMCPY( profile_record.profile.wlan.auth_info.wep_40_bit_info.key3, phInfo.m_wlan_pref.auth_info.wep64_info.key3, NET_40_BIT_WEP_KEY_SIZE );
           (void)MEMCPY( profile_record.profile.wlan.auth_info.wep_40_bit_info.key4, phInfo.m_wlan_pref.auth_info.wep64_info.key4, NET_40_BIT_WEP_KEY_SIZE );

        }
        else
        {
           profile_record.profile.wlan.auth_info.wep_104_bit_info.default_key = phInfo.m_wlan_pref.auth_info.wep128_info.default_key;
           (void)MEMCPY( profile_record.profile.wlan.auth_info.wep_104_bit_info.key1, phInfo.m_wlan_pref.auth_info.wep128_info.key1, NET_104_BIT_WEP_KEY_SIZE );
           (void)MEMCPY( profile_record.profile.wlan.auth_info.wep_104_bit_info.key2, phInfo.m_wlan_pref.auth_info.wep128_info.key2, NET_104_BIT_WEP_KEY_SIZE );
           (void)MEMCPY( profile_record.profile.wlan.auth_info.wep_104_bit_info.key3, phInfo.m_wlan_pref.auth_info.wep128_info.key3, NET_104_BIT_WEP_KEY_SIZE );
           (void)MEMCPY( profile_record.profile.wlan.auth_info.wep_104_bit_info.key4, phInfo.m_wlan_pref.auth_info.wep128_info.key4, NET_104_BIT_WEP_KEY_SIZE ); 
        }
        break;

     case NET_AUTH_TYPE_8021X_PASSWD:
        profile_record.profile.wlan.auth_info.x_info.passwd.pass_len = phInfo.m_wlan_pref.auth_info.passwd_info.pass_len;
        (void)MEMCPY( profile_record.profile.wlan.auth_info.x_info.passwd.pass, phInfo.m_wlan_pref.auth_info.passwd_info.pass, profile_record.profile.wlan.auth_info.x_info_passwd.pass_len );
        break;

#if 0
     case NET_AUTH_TYPE_8021X_TLS_RADIUS:
     case NET_AUTH_TYPE_8021X_TLS_RADIUS_WKEY:
        profile_record.profile.wlan.auth_info.x_info.tls.id_len =
        (void)MEMCPY( profile_record.profile.wlan.auth_info.x_info.tls.id, ,profile_record.profile.wlan.auth_info.x_info.tls.id_len );
        break;
#endif

     case NET_AUTH_TYPE_8021X_MD5_RADIUS:
        profile_record.profile.wlan.auth_info.x_info.md5.id_len = phInfo.m_wlan_pref.auth_info.md5_info.id_len;
        (void)MEMCPY( profile_record.profile.wlan.auth_info.x_info.md5.id, phInfo.m_wlan_pref.auth_info.md5_info.id,profile_record.profile.wlan.auth_info.x_info.md5.id_len );
        profile_record.profile.wlan.auth_info.x_info.md5.pass_len = phInfo.m_wlan_pref.auth_info.md5_info.pass_len;
        (void)MEMCPY( profile_record.profile.wlan.auth_info.x_info.md5.pass, phInfo.m_wlan_pref.auth_info.md5_info.pass, profile_record.profile.wlan.auth_info.x_info.md5.pass_len );
        break;

     case NET_AUTH_TYPE_8021X_TLS_CERT:
        profile_record.profile.wlan.auth_info.x_info.tls_cert.id_len = phInfo.m_wlan_pref.auth_info.tls_cert_info.id_len;
        (void)MEMCPY( profile_record.profile.wlan.auth_info.x_info.tls_cert.id, phInfo.m_wlan_pref.auth_info.tls_cert_info.id,profile_record.profile.wlan.auth_info.x_info.tls_cert.id_len );
        profile_record.profile.wlan.auth_info.x_info.tls_cert.cert_id = phInfo.m_wlan_pref.auth_info.tls_cert_info.cert_id;
        break;

#if 0
     case NET_AUTH_TYPE_PAP:
     case NET_AUTH_TYPE_CHAP:
     case NET_AUTH_TYPE_MsCHAPv2:
        profile_record.profile.wlan.auth_info.user_id.id_len =
        (void)MEMCPY( profile_record.profile.wlan.auth_info.user_id.id, ,profile_record.profile.wlan.auth_info.user_id.id_len );
        profile_record.profile.wlan.auth_info.passwd.id_len =
        (void)MEMCPY( profile_record.profile.wlan.auth_info.passwd.id, ,profile_record.profile.wlan.auth_info.passwd.id_len );
        profile_record.profile.wlan.auth_info.passwd.pass_len =
        (void)MEMCPY( profile_record.profile.wlan.auth_info.passwd.pass, , profile_record.profile.wlan.auth_info.passwd.pass_len );

        break;

     case NET_AUTH_TYPE_PEAP:
     case NET_AUTH_TYPE_TTLS:
        profile_record.profile.wlan.auth_info.user_id.id_len =
        (void)MEMCPY( profile_record.profile.wlan.auth_info.user_id.id, ,profile_record.profile.wlan.auth_info.user_id.id_len );
        profile_record.profile.wlan.auth_info.peap_ttls.nested.auth =
        profile_record.profile.wlan.auth_info.peap_ttls.nested.profile_id =
        profile_record.profile.wlan.auth_info.peap_ttls.cert_id = 
        profile_record.profile.wlan.auth_info.peap_ttls.user_id.id_len = 
        (void)MEMCPY( profile_record.profile.wlan.auth_info.peap_ttls.user_id.id, ,profile_record.profile.wlan.auth_info.peap_ttls.user_id.id_len );
        break;

     case NET_AUTH_TYPE_SIM:
     case NET_AUTH_TYPE_AKA:
#endif
     default:
        break;
     }
     return SUCCESS;
  }
  else 
#endif // AEE_SIMULATOR
#endif //MULTIPROF
  if ( FALSE == OEMWIFI_Get_SysProfileRec( id, &system_record, &profile_record ))
  {
     MSG_ERROR( "OEMWIFI_LoadOptions: Cannot read settings", 0, 0, 0 );
     return EFAILED;
  }

  if ( SUCCESS != ISHELL_CreateInstance( pMe->pIShell, AEECLSID_WEBOPTS, (void **)&pTmpOptions ))
  {
     MSG_ERROR( "OEMWIFI_LoadOptions: Tmp Options", 0, 0, 0 );
   
     return EFAILED;
  }

  /* Run through each of the items and add the value parameter. */
  for ( i = 0; i < (int)ARR_SIZE( values_to_set ); i++ )
  {
     values_to_set[ i ].pVal = OEMWIFI_Get_OptVal( options, values_to_set[ i ].nId, &system_record, &profile_record );
  }

  /* Fill the options parameter with the internal variables. */
  if ( SUCCESS != ( ret = IWEBOPTS_AddOpt( pTmpOptions, values_to_set )))
  {
     MSG_ERROR( "Could not add set of Options. Error 0x%x", ret, 0, 0 );
  }

  /* remove nonapplicable options */
  for ( i = 0; i < (int)ARR_SIZE( values_to_set ); i++ )
  {
     switch ( values_to_set[ i ].nId )
     {
     case AEEWIFI_OPT_SSID:
     case AEEWIFI_OPT_STATIC_IP:
     case AEEWIFI_OPT_DNS_ADDRESS:
     case AEEWIFI_OPT_SUBNET_MASK:
     case AEEWIFI_OPT_DEFAULT_GATEWAY:
#if MIN_BREW_VERSIONEx(3,1,5)
     case AEEWIFI_OPT_SECONDARY_DNS_ADDRESS:
     case AEEWIFI_OPT_DOMAIN_NAME:
#endif
       FREEIF( values_to_set[ i ].pVal ); 
       break;

     case AEEWIFI_OPT_WEP_KEY:
        if ( (void *)AEEWIFI_AUTHENTICATION_WEP != values_to_set[ AUTHINDEX ].pVal )
        {
           (void)IWEBOPTS_RemoveOpt( pTmpOptions, AEEWIFI_OPT_WEP_KEY, 0 ); 
           (void)IWEBOPTS_RemoveOpt( pTmpOptions, AEEWIFI_OPT_WEP_KEY_ID, 0 ); 
           (void)IWEBOPTS_RemoveOpt( pTmpOptions, AEEWIFI_OPT_WEP_64BIT_KEY_ID, 0 ); 
           (void)IWEBOPTS_RemoveOpt( pTmpOptions, AEEWIFI_OPT_WEP_128BIT_KEY_ID, 0 ); 
        }
        else
        {
           (void)IWEBOPTS_RemoveOpt( pTmpOptions, 
                                     ((void *)AEEWIFI_ENCRYPTION_WEP_64bit != values_to_set[ AUTHINDEX - 1 ].pVal ) ?
                                     AEEWIFI_OPT_WEP_64BIT_KEY_ID : AEEWIFI_OPT_WEP_128BIT_KEY_ID, 0 ); 
        }
        FREEIF( values_to_set[ i ].pVal );
        break;

     case AEEWIFI_OPT_WEP_64BIT_KEY_0:
     case AEEWIFI_OPT_WEP_64BIT_KEY_1:
     case AEEWIFI_OPT_WEP_64BIT_KEY_2:
     case AEEWIFI_OPT_WEP_64BIT_KEY_3:
        if ( (void *)AEEWIFI_AUTHENTICATION_WEP != values_to_set[ AUTHINDEX ].pVal ||
             (void *)AEEWIFI_ENCRYPTION_WEP_64bit != values_to_set[ AUTHINDEX - 1 ].pVal )
        {
           (void)IWEBOPTS_RemoveOpt( pTmpOptions, values_to_set[ i ].nId, 0 ); 
        }
        FREEIF( values_to_set[ i ].pVal );
        break;

     case AEEWIFI_OPT_WEP_128BIT_KEY_0:
     case AEEWIFI_OPT_WEP_128BIT_KEY_1:
     case AEEWIFI_OPT_WEP_128BIT_KEY_2:
     case AEEWIFI_OPT_WEP_128BIT_KEY_3:
         if ( (void *)AEEWIFI_AUTHENTICATION_WEP != values_to_set[ AUTHINDEX ].pVal ||
              (void *)AEEWIFI_ENCRYPTION_WEP_128bit != values_to_set[ AUTHINDEX - 1 ].pVal )
        {
           (void)IWEBOPTS_RemoveOpt( pTmpOptions, values_to_set[ i ].nId, 0 ); 
        }
        FREEIF( values_to_set[ i ].pVal );
        break;

     case AEEWIFI_OPT_8021X_ID:
        switch ((uint32)values_to_set[ AUTHINDEX ].pVal)
        {
        case AEEWIFI_AUTHENTICATION_8021X_MD5:
        case AEEWIFI_AUTHENTICATION_8021X_TLS:
        case AEEWIFI_AUTHENTICATION_8021X_TLS_RADIUS_WKEY:
        case AEEWIFI_AUTHENTICATION_8021X_TLS_CERT:
        case AEEWIFI_AUTHENTICATION_PAP:
        case AEEWIFI_AUTHENTICATION_CHAP:
        case AEEWIFI_AUTHENTICATION_MSCHAPV2:
        case AEEWIFI_AUTHENTICATION_PEAP:
        case AEEWIFI_AUTHENTICATION_TTLS:
           break;

        default:
           (void)IWEBOPTS_RemoveOpt( pTmpOptions, AEEWIFI_OPT_8021X_ID, 0 ); 
           break;
        }
        FREEIF( values_to_set[ i ].pVal );
        break;

     case AEEWIFI_OPT_USER_ID:
        switch ((uint32)values_to_set[ AUTHINDEX ].pVal)
        {
        case AEEWIFI_AUTHENTICATION_PAP:
        case AEEWIFI_AUTHENTICATION_CHAP:
        case AEEWIFI_AUTHENTICATION_MSCHAPV2:
        case AEEWIFI_AUTHENTICATION_PEAP:
        case AEEWIFI_AUTHENTICATION_TTLS:
           break;

        default:
           (void)IWEBOPTS_RemoveOpt( pTmpOptions, AEEWIFI_OPT_USER_ID, 0 ); 
           break;
        }
        FREEIF( values_to_set[ i ].pVal );
        break;

     case AEEWIFI_OPT_8021X_PASSWORD:
        switch ((uint32)values_to_set[ AUTHINDEX ].pVal)
        {
        case AEEWIFI_AUTHENTICATION_8021X_MD5:
        case AEEWIFI_AUTHENTICATION_8021X_PASSWORD:
        case AEEWIFI_AUTHENTICATION_PAP:
        case AEEWIFI_AUTHENTICATION_CHAP:
        case AEEWIFI_AUTHENTICATION_MSCHAPV2:
           break;

        default:
           (void)IWEBOPTS_RemoveOpt( pTmpOptions, AEEWIFI_OPT_8021X_PASSWORD, 0 ); 
           break;
        }
        FREEIF( values_to_set[ i ].pVal );
        break;

     case AEEWIFI_OPT_CERTIFICATE_ID:
        switch( (uint32)values_to_set[ AUTHINDEX ].pVal )
        {
        case AEEWIFI_AUTHENTICATION_8021X_TLS_CERT:
        case AEEWIFI_AUTHENTICATION_PEAP:
        case AEEWIFI_AUTHENTICATION_TTLS:
           break;

        default:
           (void)IWEBOPTS_RemoveOpt( pTmpOptions, AEEWIFI_OPT_CERTIFICATE_ID, 0 ); 
           break;
        }
        break;

     default: 
         break;
     }
  }

  if ( SUCCESS == ret )
  {
     if ((void *)AEEWIFI_AUTHENTICATION_PEAP == values_to_set[ AUTHINDEX ].pVal ||
         (void *)AEEWIFI_AUTHENTICATION_TTLS == values_to_set[ AUTHINDEX ].pVal )
     {
        ret = OEMWIFI_LoadNestedOptions( &profile_record, pTmpOptions );
     }
     else
     {
       (void)IWEBOPTS_RemoveOpt( pTmpOptions, AEEWIFI_OPT_NESTED_AUTH_ID, 0 ); 
     }
  }

  /* Clear out the options variable. */
  while ( SUCCESS == OEMWIFIOpts_RemoveOpt( options, WEBOPT_ANY ));

  /* copy new values in reverse to get nested authentication options into correct order */
  location = 0;
  while ( SUCCESS == IWEBOPTS_GetOpt( pTmpOptions, WEBOPT_ANY, location++, tmp ))
  {
     switch( tmp[0].nId )
     {
        case WEBOPT_RESERVED: // WEBOPTS_END
        case WEBOPT_ANY:
        case WEBOPT_REMOVED:
        case WEBOPT_COPYOPTS:
           continue;

        default:
           break;
     }
     if ( SUCCESS != ( ret = IWEBOPTS_AddOpt( options->pIWebOpts, tmp )))
     {
        MSG_ERROR( "Could not finalize set of Options. Error 0x%x", ret, 0, 0 );
     }
  }
  IWEBOPTS_Release( pTmpOptions );

  MSG_HIGH( "LoadOptions returning %d", ret, 0, 0 );
  return ret;
#undef AUTHINDEX
}

/*===========================================================================

FUNCTION: OEMWIFI_CommitOptions 

DESCRIPTION
  Takes the contents of the IWIFIOpts interface and sets the structure
  to these file.  At the completion, it rewrites the file.

DEPENDENCIES
  none

RETURN VALUE
  BADPARM         if any of the input pointers are NULL
  AEE_SUCCESS     when setting options into net interface was successful
  EFAILED         when setting options into net interface failed  

SIDE EFFECTS
===========================================================================*/
static int OEMWIFI_CommitOptions ( IWIFI *pMe, uint32 id, IWIFIOpts *options )
{
#define MAX_PREOPTIONS 4
#define PAUTH      0
#define PENCR      1
#define PKEYID     2
#define PKEY       3

  IWebOpts           *pWebOpts;
  WebOpt              tmp;
  WebOpt              preOptions[MAX_PREOPTIONS];
  int                 i, location = 0;
  word                index = 0;
  net_sys_rec_s_type  system_record;
  net_profile_s_type  profile_record;
  boolean             newProfile = FALSE;
  uint32              tmp_value;
  net_status_e_type   Status;
  byte                skipKey = 4;
  byte                npreop = 0;
  boolean             nestedAuthentication = FALSE;
  uint8               category = AEEWIFI_OPT_CATEGORY_NONE;

  MSG_HIGH( "CommitOptions called: profile id %d", id, 0, 0 );
  if ( NULL == options )
  {
     MSG_ERROR( "OEMWIFI_CommitOptions: NULL options", 0, 0, 0 );
     return EBADPARM;
  }
  
  if ((( pWebOpts = options->pIWebOpts ) == NULL ) || ( IWIFICoreObj == NULL ))
  {
     MSG_ERROR( "OEMWIFI_CommitOptions: NULL fields -  pIWebOpts 0x%x, IWIFICoreObj 0x%x", (uint32)pWebOpts, (uint32)IWIFICoreObj, 0 );
     return EFAILED;
  }

  if (( IWIFICoreObj->pBuffer == NULL ) || ( IWIFICoreObj->pDataRecord == NULL ))
  {
     MSG_ERROR( "OEMWIFI_CommitOptions: NULL buffers - pBuffer 0x%x, pDataRecord 0x%x", (uint32)(IWIFICoreObj->pBuffer), (uint32)(IWIFICoreObj->pDataRecord), 0 );

     return EFAILED;
  }

  (void)MEMSET( (void *)preOptions, 0, sizeof( preOptions ));
    /* don't rely on net interface to initialize system/profile records entirely */
  (void)MEMSET( (void *)&system_record, 0, sizeof( system_record ));
  (void)MEMSET( (void *)&profile_record, 0, sizeof( profile_record ));

  for ( ;; )  //  find system record by profile id.
  {
     if ( NET_STATUS_SUCCESS == net_get_sys_rec( index++, &system_record, &IWIFICoreObj->Parser ))
     {
        if ( system_record.sys_rec.wlan.profile_id == (byte)id )
        {
            Status = net_get_profile( (byte)id, &profile_record, &IWIFICoreObj->Parser );

            if ( NET_STATUS_SUCCESS != Status )
            {
               MSG_ERROR( "net_get_profile: error %d", Status, 0, 0 );
               return EFAILED;
            }
            break;
        }
     }
     else
     {
        newProfile = TRUE;
        OEMWIFI_Get_DefaultSysRec( &system_record ); 
        system_record.sys_rec.wlan.profile_id = (byte)id;
        OEMWIFI_Get_DefaultProfileRec( &profile_record, id );

        break;
     }
  }
  --index;

  // process options that must be set first
  while ( SUCCESS == IWEBOPTS_GetOpt( pWebOpts, WEBOPT_ANY, location++, &tmp ))
  {
     switch ( tmp.nId )
     {
        case AEEWIFI_OPT_ENCRYPTION_TYPE:
           if ( !IN_RANGE( (int32)tmp.pVal, AEEWIFI_ENCRYPTION_NONE, AEEWIFI_ENCRYPTION_AES ))
           {
              MSG_ERROR( "OEMWIFI_CommitOptions: ID 0x%x value 0x%x out of range", tmp.nId, tmp.pVal, 0 );
              return EBADPARM;
           }
           preOptions[ PENCR ].nId = tmp.nId;
           preOptions[ PENCR ].pVal = tmp.pVal;
           ++npreop;
           break;

        case AEEWIFI_OPT_AUTHENTICATION_TYPE:
           if ( !IN_RANGE( (int32)tmp.pVal, AEEWIFI_AUTHENTICATION_NONE, AEEWIFI_AUTHENTICATION_AKA ))
           {
              MSG_ERROR( "OEMWIFI_CommitOptions: ID 0x%x value 0x%x out of range", tmp.nId, tmp.pVal, 0 );
              return EBADPARM;
           }
           preOptions[ PAUTH ].nId = tmp.nId;
           preOptions[ PAUTH ].pVal = tmp.pVal;
           /* if changing authentication type, clear out the old one */
           if ( (void *)profile_record.profile.wlan.auth_type != tmp.pVal )
           {
              (void)MEMSET((void *)&profile_record.profile.wlan.auth_info, 0, sizeof(profile_record.profile.wlan.auth_info));
           }
           ++npreop;
           break;

        case AEEWIFI_OPT_WEP_KEY_ID:
           if ( !IN_RANGE( (int32)tmp.pVal, 0, 3 ))
           {
              MSG_ERROR( "OEMWIFI_CommitOptions: ID 0x%x value 0x%x out of range", tmp.nId, tmp.pVal, 0 );
              return EBADPARM;
           }
           preOptions[ PKEYID ].nId = tmp.nId;
           preOptions[ PKEYID ].pVal = tmp.pVal;
           ++npreop;
           break;

        case AEEWIFI_OPT_WEP_KEY:
           preOptions[ PKEY ].nId = tmp.nId;
           preOptions[ PKEY ].pVal = tmp.pVal;
           ++npreop;
           break;

        default:
           break;
     }
     if ( AEEWIFI_OPT_NESTED_AUTH_ID == tmp.nId )
     {
        break;
     }
  }

  if (( 0 != preOptions[ PKEY ].nId ) &&
      (( AEEWIFI_ENCRYPTION_WEP_64bit == profile_record.profile.wlan.encryp_type ) ||
       ( AEEWIFI_ENCRYPTION_WEP_128bit == profile_record.profile.wlan.encryp_type )))
  {
     // for 3.1.4->3 backward compatibility
     // check if 64/128-bit default key is changed. If so, ignore the other *_KEY_0..3
     uint8  *key_ptr = NULL;
     void   *pStr = NULL;
       
     if ( AEEWIFI_ENCRYPTION_WEP_128bit == profile_record.profile.wlan.encryp_type )
     {
        switch ( profile_record.profile.wlan.auth_info.wep_104_bit_info.default_key )
        {
        case 1:
           key_ptr = profile_record.profile.wlan.auth_info.wep_104_bit_info.key2;
           break;

        case 2:
           key_ptr = profile_record.profile.wlan.auth_info.wep_104_bit_info.key3;
           break;

        case 3:
           key_ptr = profile_record.profile.wlan.auth_info.wep_104_bit_info.key4;
           break;

        case 0:
        default:
           key_ptr = profile_record.profile.wlan.auth_info.wep_104_bit_info.key1;
           break;
        }
        if ( NULL == ( pStr = ConvertBinBufToStr( key_ptr, (uint8)sizeof( profile_record.profile.wlan.auth_info.wep_104_bit_info.key1 ))))
        {
           return ENOMEMORY;
        }
        if (( 0 == STRCMP( (char *)pStr, (char *)preOptions[ PKEY ].pVal )) && 
            ( (uint32)preOptions[ PKEYID ].pVal == (uint32)profile_record.profile.wlan.auth_info.wep_104_bit_info.default_key ))
        {
           npreop -= 2;
        }
     }
     else
     {
        switch ( profile_record.profile.wlan.auth_info.wep_40_bit_info.default_key )
        {
        case 1:
           key_ptr = profile_record.profile.wlan.auth_info.wep_40_bit_info.key2;
           break;

        case 2:
           key_ptr = profile_record.profile.wlan.auth_info.wep_40_bit_info.key3;
           break;

        case 3:
           key_ptr = profile_record.profile.wlan.auth_info.wep_40_bit_info.key4;
           break;

        case 0:
        default:
           key_ptr = profile_record.profile.wlan.auth_info.wep_40_bit_info.key1;
           break;
        }
        if ( NULL == ( pStr = ConvertBinBufToStr( key_ptr, (uint8)sizeof( profile_record.profile.wlan.auth_info.wep_40_bit_info.key1 ))))
        {
           return ENOMEMORY;
        }   
        if (( 0 == STRCMP( (char *)pStr, (char *)preOptions[ PKEY ].pVal )) && 
            ( (uint32)preOptions[ PKEYID ].pVal == (uint32)profile_record.profile.wlan.auth_info.wep_40_bit_info.default_key ))
        {
           npreop -= 2;
        }
     }
     FREE( pStr );
  }

  for (i = 0; i < npreop; ++i)
  {
     if ( 0 != preOptions[ i ].nId )
     {
        if ( PKEY == i ) // 3.1.3 operation
        {
           skipKey = ( AEEWIFI_ENCRYPTION_WEP_128bit == profile_record.profile.wlan.encryp_type ) ?
                     profile_record.profile.wlan.auth_info.wep_104_bit_info.default_key :
                     profile_record.profile.wlan.auth_info.wep_40_bit_info.default_key;
        }
        if ( FALSE == OEMWIFI_Set_OptVal( options, preOptions[ i ].nId, preOptions[ i ].pVal,
                                          &system_record, &profile_record ))
        {
           MSG_ERROR( "Unsupported Option Value: Profile %d,  ID 0x%x,  Value 0x%x", id, tmp.nId, tmp.pVal );
           return EBADPARM;
        }
     }
  }

  // process rest of the options
  location = 0;
  while ( SUCCESS == IWEBOPTS_GetOpt( pWebOpts, WEBOPT_ANY, location++, &tmp ))
  {
     switch ( tmp.nId )
     {
        case AEEWIFI_OPT_WEP_KEY:
        case AEEWIFI_OPT_WEP_KEY_ID:
        case AEEWIFI_OPT_ENCRYPTION_TYPE:
        case AEEWIFI_OPT_AUTHENTICATION_TYPE:
           if ( FALSE == nestedAuthentication )
           {
              // already processed
              continue;
           }
           break;


        case AEEWIFI_OPT_PROFILE_ID:
           if ( !IN_RANGE( (int32)tmp.pVal, AEEWIFI_PROFILE_ID_MIN, AEEWIFI_PROFILE_ID_MAX ))
           {
              MSG_ERROR( "OEMWIFI_CommitOptions: Profile ID 0x%x value 0x%x out of range", tmp.nId, tmp.pVal, 0 );
              return EBADPARM;
           }

           if (( FALSE == newProfile ) && ( id != (uint32)system_record.sys_rec.wlan.profile_id ))
           {
              /* changing profile ID on an existing profile */
              (void)net_del_profile( profile_record.profile_id, &IWIFICoreObj->Parser );
              (void)net_del_sys_rec( index, &IWIFICoreObj->Parser );
              newProfile = TRUE;
           }       
           tmp_value = (uint32)tmp.pVal;
           system_record.sys_rec.wlan.profile_id = (byte)tmp_value;
           profile_record.profile_id = (byte)tmp_value;
           continue;

        case AEEWIFI_OPT_TECH:
           if ( !IN_RANGE( (uint32)tmp.pVal, AEEWIFI_TECH_80211_A, AEEWIFI_TECH_80211_ANY ))
           {
              MSG_ERROR( "OEMWIFI_CommitOptions: ID 0x%x value 0x%x out of range", tmp.nId, tmp.pVal, 0 );
              return EBADPARM;
           }
           break;

        case AEEWIFI_OPT_BANDPREF:
           if (( !IN_RANGE( (int32)tmp.pVal, AEEWIFI_BAND_2400MHZ_US, AEEWIFI_BAND_MAX )) &&
               ( AEEWIFI_BAND_ANY != (uint32)tmp.pVal))
           {
              MSG_ERROR( "OEMWIFI_CommitOptions: ID 0x%x value 0x%x out of range", tmp.nId, tmp.pVal, 0 );
              return EBADPARM;
           }
           break;

        case AEEWIFI_OPT_CHANNEL:
           if (( AEEWIFI_CHANNEL_ANY != (uint32)tmp.pVal ) &&
               !IN_RANGE( (uint32)tmp.pVal, AEEWIFI_CHANNEL_MIN, AEEWIFI_CHANNEL_MAX ))
           {
              MSG_ERROR( "OEMWIFI_CommitOptions: ID 0x%x value 0x%x out of range", tmp.nId, tmp.pVal, 0 );
              return EBADPARM;
           }
           break;

        case AEEWIFI_OPT_WLAN_MODE:
           if ( !IN_RANGE( (uint32)tmp.pVal, AEEWIFI_MODE_ADHOC, AEEWIFI_MODE_ANY ))
           {
              MSG_ERROR( "OEMWIFI_CommitOptions: ID 0x%x value 0x%x out of range", tmp.nId, tmp.pVal, 0 );
              return EBADPARM;
           }
           break;

        case AEEWIFI_OPT_PREAMBLE_TYPE:
           if ( !IN_RANGE( (int32)tmp.pVal, AEEWIFI_PREAMBLE_AUTO, AEEWIFI_PREAMBLE_LONG ))
           {
              MSG_ERROR( "OEMWIFI_CommitOptions: ID 0x%x value 0x%x out of range", tmp.nId, tmp.pVal, 0 );
              return EBADPARM;
           }
           break;

        case AEEWIFI_OPT_DHCP_ENABLED:
           if ( !IN_RANGE( (int32)tmp.pVal, AEEWIFI_DHCP_DISABLED, AEEWIFI_DHCP_ENABLED ))
           {
              MSG_ERROR( "OEMWIFI_CommitOptions: ID 0x%x value 0x%x out of range", tmp.nId, tmp.pVal, 0 );
              return EBADPARM;
           }
           break;

        case AEEWIFI_OPT_IP_TYPE:
           if ( !IN_RANGE( (int32)tmp.pVal, AEEWIFI_IPTYPE_IPV4, AEEWIFI_IPTYPE_IPV6 ))
           {
              MSG_ERROR( "OEMWIFI_CommitOptions: ID 0x%x value 0x%x out of range", tmp.nId, tmp.pVal, 0 );
              return EBADPARM;
           }
           break;

        case AEEWIFI_OPT_POWERLEVEL:
           if ( !IN_RANGE( (int32)tmp.pVal, AEEWIFI_POWERLEVEL_MIN, AEEWIFI_POWERLEVEL_MAX ))
           {
              MSG_ERROR( "OEMWIFI_CommitOptions: Power level 0x%x value 0x%x out of range", tmp.nId, tmp.pVal, 0 );
              return EBADPARM;
           }
           break;

        case AEEWIFI_OPT_WEP_64BIT_KEY_ID: 
        case AEEWIFI_OPT_WEP_128BIT_KEY_ID:
           if ( !IN_RANGE( (int32)tmp.pVal, 0, 3 ))
           {
              MSG_ERROR( "OEMWIFI_CommitOptions: ID 0x%x value 0x%x out of range", tmp.nId, tmp.pVal, 0 );
              return EBADPARM;
           }
           break;

        case AEEWIFI_OPT_WEP_64BIT_KEY_0:
           if (( AEEWIFI_ENCRYPTION_WEP_64bit == profile_record.profile.wlan.encryp_type )  && ( 0 == skipKey ))
           {
              continue;
           }
           break;

        case AEEWIFI_OPT_WEP_64BIT_KEY_1:
           if (( AEEWIFI_ENCRYPTION_WEP_64bit == profile_record.profile.wlan.encryp_type ) && ( 1 == skipKey ))
           {
              continue;
           }
           break;

        case AEEWIFI_OPT_WEP_64BIT_KEY_2:
           if (( AEEWIFI_ENCRYPTION_WEP_64bit == profile_record.profile.wlan.encryp_type ) && ( 2 == skipKey ))
           {
              continue;
           }
           break;

        case AEEWIFI_OPT_WEP_64BIT_KEY_3:
           if (( AEEWIFI_ENCRYPTION_WEP_64bit == profile_record.profile.wlan.encryp_type ) && ( 3 == skipKey ))
           {
              continue;
           }
           break;

        case AEEWIFI_OPT_WEP_128BIT_KEY_0:
           if (( AEEWIFI_ENCRYPTION_WEP_128bit == profile_record.profile.wlan.encryp_type ) && ( 0 == skipKey ))
           {
              continue;
           }
           break;

        case AEEWIFI_OPT_WEP_128BIT_KEY_1:
           if (( AEEWIFI_ENCRYPTION_WEP_128bit == profile_record.profile.wlan.encryp_type ) && ( 1 == skipKey ))
           {
              continue;
           }
           break;

        case AEEWIFI_OPT_WEP_128BIT_KEY_2:
           if (( AEEWIFI_ENCRYPTION_WEP_128bit == profile_record.profile.wlan.encryp_type ) && ( 2 == skipKey ))
           {
              continue;
           }
           break;

        case AEEWIFI_OPT_WEP_128BIT_KEY_3:
           if (( AEEWIFI_ENCRYPTION_WEP_128bit == profile_record.profile.wlan.encryp_type ) && ( 3 == skipKey ))
           {
              continue;
           }
           break;

        case AEEWIFI_OPT_8021X_ID:
           switch (profile_record.profile.wlan.auth_type )
           {
           case AEEWIFI_AUTHENTICATION_8021X_MD5:
           case AEEWIFI_AUTHENTICATION_8021X_TLS:
           case AEEWIFI_AUTHENTICATION_8021X_TLS_RADIUS_WKEY:
           case AEEWIFI_AUTHENTICATION_8021X_TLS_CERT:
           case AEEWIFI_AUTHENTICATION_PAP:
           case AEEWIFI_AUTHENTICATION_CHAP:
           case AEEWIFI_AUTHENTICATION_MSCHAPV2:
           case AEEWIFI_AUTHENTICATION_PEAP:
           case AEEWIFI_AUTHENTICATION_TTLS:
              break;

           default:
              continue; // ignore;
           }
           break;

        case AEEWIFI_OPT_8021X_PASSWORD:
           switch (profile_record.profile.wlan.auth_type )
           {
           case AEEWIFI_AUTHENTICATION_8021X_MD5:
           case AEEWIFI_AUTHENTICATION_8021X_PASSWORD:
           case AEEWIFI_AUTHENTICATION_PAP:
           case AEEWIFI_AUTHENTICATION_CHAP:
           case AEEWIFI_AUTHENTICATION_MSCHAPV2:
              break;

           default:
              continue; // ignore;
           }
           break;

        case AEEWIFI_OPT_USER_ID:
           switch (profile_record.profile.wlan.auth_type )
           {
           case AEEWIFI_AUTHENTICATION_PAP:
           case AEEWIFI_AUTHENTICATION_CHAP:
           case AEEWIFI_AUTHENTICATION_MSCHAPV2:
           case AEEWIFI_AUTHENTICATION_PEAP:
           case AEEWIFI_AUTHENTICATION_TTLS:
              break;

           default:
              continue; // ignore;
           }
           break;

        case AEEWIFI_OPT_NESTED_AUTH_ID:
           nestedAuthentication = TRUE;
           break;

        case AEEWIFI_OPT_GROUPS:
        case AEEWIFI_OPT_SSID:
        case AEEWIFI_OPT_MOB_IP_PROFILE_ID:
        case AEEWIFI_OPT_CERTIFICATE_ID:
        case AEEWIFI_OPT_LISTEN_INTERVAL:
        case AEEWIFI_OPT_RTSTHRESHOLD:
        case AEEWIFI_OPT_SCANTIMING:
        case AEEWIFI_OPT_FRAGMENTATIONTHRESHOLD:
#if MIN_BREW_VERSIONEx(3,1,5)
        case AEEWIFI_OPT_RSSI_THRESHOLD:
        case AEEWIFI_OPT_MTU_SIZE:
#endif
        case AEEWIFI_OPT_STATIC_IP:
        case AEEWIFI_OPT_DNS_ADDRESS:
        case AEEWIFI_OPT_SUBNET_MASK:
        case AEEWIFI_OPT_DEFAULT_GATEWAY:
#if MIN_BREW_VERSIONEx(3,1,5)
        case AEEWIFI_OPT_SECONDARY_DNS_ADDRESS:
        case AEEWIFI_OPT_DOMAIN_NAME:
#endif
           break;

        case WEBOPT_RESERVED:
        case WEBOPT_ANY:
        case WEBOPT_REMOVED:
        case WEBOPT_COPYOPTS:
           continue;

        default:
           MSG_ERROR( "OEMWIFI_CommitOptions: Unknown ID 0x%x - value 0x%x", tmp.nId, tmp.pVal, 0 );
           return EFAILED;

     }

     /* Compare tmp value with what records have and
        only commit it if they are different. */
     if ( 0 != OEMWIFI_Cmp_OptVal( options, tmp.nId, tmp.pVal,
                                   &system_record, &profile_record ) )
     {
       /* Update option category flag to track category change */
       category |= OEMWIFI_Get_OptCategory(tmp.nId);

       /* Take the tmp value and commit it to the structures. */
       if ( FALSE == OEMWIFI_Set_OptVal( options, tmp.nId, tmp.pVal,
                                         &system_record, &profile_record ) )
       {
          MSG_ERROR( "Unsupported Option Value: Profile %d,  ID 0x%x,  Value 0x%x", id, tmp.nId, tmp.pVal );
          return EBADPARM;
       }
     }
  }

  if ( TRUE == newProfile )
  {
     Status = net_insert_sys_rec( 0, &system_record, &IWIFICoreObj->Parser );
     if ( NET_STATUS_SUCCESS != Status )
     {
        MSG_ERROR( "ERROR %d changing system record profile ID!", Status, 0, 0 );
        return EFAILED;
     }
   
     Status  = net_insert_profile( &profile_record, &IWIFICoreObj->Parser );

     if ( NET_STATUS_SUCCESS != Status )
     {
        MSG_ERROR( "ERROR %d changing profile record!", Status, 0, 0 );
        return EFAILED;
     }
  }
  else
  {
     Status =  net_modify_sys_rec( index, &system_record, &IWIFICoreObj->Parser );
     if ( NET_STATUS_SUCCESS != Status )
     {
        MSG_ERROR( "ERROR %d updating system record!", Status, 0, 0 );
        return EFAILED;
     }

     Status = net_modify_profile( &profile_record, &IWIFICoreObj->Parser );

     if ( NET_STATUS_SUCCESS != Status )
     {
        MSG_ERROR( "ERROR %d updating profile record!", Status, 0, 0 );
        return EFAILED;
     }
  }

  return (( TRUE == OEMWIFI_WriteFile( (uint32)system_record.sys_rec.wlan.profile_id,
                                       category ) ) ? AEE_SUCCESS : EFAILED );
}

/*===========================================================================

FUNCTION: OEMWIFI_GetStatistics 

DESCRIPTION
  Gets the WIFI Statistics.  Currently not implemented.

DEPENDENCIES
  none

RETURN VALUE
  EBADPARM     if any of the input pointers are NULL
  AEE_SUCCESS  otherwise

SIDE EFFECTS
===========================================================================*/
static int OEMWIFI_GetStatistics ( IWIFI *pMe, AEEWIFIStatistics *statistics )
{
#ifndef AEE_SIMULATOR
  AEECMSSInfo ssInfo;

  if ( NULL == statistics )
  {
     MSG_ERROR( "OEMWIFI_GetStatistics: NULL statistics", 0, 0, 0 );
     return EBADPARM;
  }
  if (( NULL == IWIFICoreObj ))
  {
     MSG_ERROR( "OEMWIFI_GetStatistics: NULL IWIFICoreObj", 0, 0, 0 );

     return EFAILED;
  }
#endif

  (void)MEMSET( (void *)statistics, 0, sizeof( AEEWIFIStatistics ));
  statistics->dwSize = sizeof( AEEWIFIStatistics );

#ifndef AEE_SIMULATOR
  if ( AEE_SUCCESS == ICM_GetSSInfo( IWIFICoreObj->pICM, &ssInfo, sizeof( ssInfo )))
  {
     statistics->tx_bytes = ssInfo.wlan_stats.total_tx_bytes;
     statistics->rx_bytes = ssInfo.wlan_stats.total_rx_bytes;
  }
#endif

  return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMWIFI_EnumProfileInit()

DESCRIPTION:
    Resets the IWIFI’s internal profile enumeration.

DEPENDENCIES
  none

RETURN VALUE:
    AEE_SUCCESS:  If enumeration of profiles is allowed.
    EFAILED:      Unspecified internal failure.

SIDE EFFECTS:
    None

============================================================================*/
static int OEMWIFI_EnumProfileInit( IWIFI *pMe )
{
   pMe->nEnum = 0;

   if ( NULL == IWIFICoreObj )
   {
      MSG_ERROR( "OEMWIFI_EnumProfiletInit: CoreObj is NULL", 0, 0, 0 );
      return EFAILED;
   }

   return AEE_SUCCESS;
}

/*=======================================================================

FUNCTION: OEMWIFI_EnumNextProfile()

DESCRIPTION:
    Returns next profile ID if any.

DEPENDENCIES
    none

RETURN VALUE:
    AEE_SUCCESS:  Valid profile id was returned.
    EBADPARM:     If any of the parameters are NULL
    EFAILED:      All profile ids have been returned in previous calls
                  or an error occured.
============================================================================*/
static int OEMWIFI_EnumNextProfile ( IWIFI *pMe, uint32 *id )
{ 
   net_sys_rec_s_type system_record;
   net_status_e_type  Status;

   if ( NULL == id )
   {
      MSG_ERROR( "OEMWIFI_EnumNextProfile: NULL id", 0, 0, 0 );
      return EBADPARM;
   }

   if ( NULL == IWIFICoreObj )
   {
      MSG_ERROR( "OEMWIFI_EnumNextProfile: CoreObj is NULL", 0, 0, 0 );
      return EFAILED;
   }

   Status = net_get_sys_rec( pMe->nEnum, &system_record, &IWIFICoreObj->Parser );
   if ( NET_STATUS_SUCCESS == Status )
   {
      *id = (uint32)system_record.sys_rec.wlan.profile_id;
      ++pMe->nEnum;

      return AEE_SUCCESS;
   }

   return EFAILED;
}

/*===========================================================================

                   IWIFIOpts FUNCTION DEFINITIONS   

===========================================================================*/
/*=============================================================================
FUNCTION: OEMWIFIOpts_New

DESCRIPTION:  
  Allocates a new IWIFIOpts object for every ISHELL_CreateInstance().
  An IWebOpts is also created for each IWIFIOpts object.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

RETURN VALUE:
  EBADPARAM    if any of the parameters are NULL
  ENOMEMORY    when out of memory
  EUNSUPPORTED if the class ID is not AEECLSID_WIFIOPTS
  AEE_SUCCESS  otherwise

=============================================================================*/
int OEMWIFIOpts_New ( IShell *pIShell, AEECLSID cls, void **ppif )
{
   IWIFIOpts *pNew;
   IWebOpts  *pWebOpts;
  
   // Check parameters.
   if (( NULL == pIShell ) || ( NULL == ppif ))
   {
      MSG_ERROR( "OEMWIFIOpts_New: NULL parameters - pIShell 0x%x, ppif 0x%x", (uint32)pIShell, ppif, 0 );
      return EBADPARM;
   }

   if ( cls != AEECLSID_WIFI_OPTS )
   {
      MSG_ERROR( "OEMWIFIOpts_New: unsupported class ID 0x%x", cls, 0, 0 );
      return ECLASSNOTSUPPORT;
   }

   // Allocate the object.
   pNew = (IWIFIOpts *) AEE_NewClass( (IBaseVtbl*)&gOEMWIFIOptsFuncs, sizeof( IWIFIOpts )); 
   if ( pNew == NULL )
   {
      MSG_ERROR( "OEMWIFIOpts_New: out of memory", 0, 0, 0 );
      return ENOMEMORY;
   }

   /* Initialize the values. */
   pNew->pIShell = pIShell;
   pNew->cls = cls;
   pNew->uRefs = 1;

   if (( SUCCESS != ISHELL_CreateInstance( pIShell, AEECLSID_WEBOPTS, (void **) &pWebOpts )) ||
       ( pWebOpts == NULL ))
   {
      FREE(pNew);
      MSG_ERROR( "OEMWIFIOpts_New: out of memory", 0, 0, 0 );
      return ENOMEMORY;
   }

   pNew->pIWebOpts = pWebOpts;

   *ppif = (void **)pNew;

   return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMWIFIOpts_AddRef

DESCRIPTION:  
  Indicates that one or more object is now pointing to object.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

RETURN VALUE:
  Current incremented reference count

=============================================================================*/
static uint32 OEMWIFIOpts_AddRef( IWIFIOpts *pMe )
{
   return ( ++pMe->uRefs );
}

/*=============================================================================
FUNCTION: OEMWIFIOpts_Release

DESCRIPTION:  
  Decreases the number of references to the IWIFIOpts object.  If the
  last reference is removed, it deallocates the object.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

RETURN VALUE:
  Current decremented reference count

=============================================================================*/
static uint32 OEMWIFIOpts_Release( IWIFIOpts *pMe )
{
   if ( --pMe->uRefs != 0 )
   {
      return pMe->uRefs;
   }
    
   (void) IWEBOPTS_Release( pMe->pIWebOpts );

   FREE( pMe );

   return 0;
}

/*===========================================================================

FUNCTION: OEMWIFIOpts_QueryInterface

DESCRIPTION
  Allows applications to query if IWIFIOpts supports other interfaces and then
  returns a pointer to that new interface supported via IWIFIOpts.  Currently,
  we do not support any other interface.

DEPENDENCIES
  none

SIDE EFFECTS
  none

RETURN VALUE
  EBADPARM             if any of the parameters is NULL
  ECLASSNOTSUPPORTED   if id != AEECLSID_WIFIOPTS
  AEE_SUCCESS          otherwise

===========================================================================*/
static int OEMWIFIOpts_QueryInterface( IWIFIOpts *pMe, AEECLSID id, void **ppNew )
{
   int ret_val = AEE_SUCCESS;

   if ( NULL == ppNew )
   {
      MSG_ERROR( "OEMWIFIOpts_QueryInterface: NULL ppNew", 0, 0, 0 );
      return EBADPARM;
   }

   switch ( id )
   {
      case AEECLSID_WIFI_OPTS:
      case AEECLSID_QUERYINTERFACE:
         if ( 0 != OEMWIFIOpts_AddRef( pMe ))
         {
            *ppNew = pMe;
         }
         else
         {
            *ppNew = NULL;
            MSG_ERROR( "OEMWIFIOpts_QueryInterface: unsupported class id 0x%x", id, 0, 0 );
            ret_val = ECLASSNOTSUPPORT;
         }
         break;

      default:
         *ppNew = NULL;
         MSG_ERROR( "OEMWIFIOpts_QueryInterface: unsupported class id 0x%x", id, 0, 0 );
         ret_val = ECLASSNOTSUPPORT;
         break;
   }


   return ret_val;
}

/*=============================================================================
FUNCTION: OEMWIFIOpts_AddOpt

DESCRIPTION:  
  Adds an ID-Value pair to IWIFIOpts. Only one value is allowed for each
  ID unlike IWebOpts which maintains a stack of values for each ID.

PROTOTYPE:
  int OEMWIFIOpts_AddOpt(IWIFIOpts *pMe, WIFIOpt *pOpt)

PARAMETERS:
  pMe                [in]: IWIFIOpts interface pointer.
  pOpt               [in]: Call option

RETURN VALUE:
  SUCCESS:   If addition was successful.
  ENOMEMORY: If enough memory couldn't be allocated
  EBADPARM:  If any of the input parameters is NULL.

COMMENTS:
  
SIDE EFFECTS:

SEE ALSO:
  IWEBOPTS_AddOpt()

=============================================================================*/
static int OEMWIFIOpts_AddOpt( IWIFIOpts *pMe, WIFIOpt *pOpt )
{
   WebOpt tmp[ 2 ] = { 0, NULL, WEBOPT_END, NULL };
   char *szTmp = NULL;
   uint32 len;
   int ret;
  
   if (( NULL == pOpt ) || ( NULL == pMe->pIWebOpts ))
   {
      MSG_ERROR( "OEMWIFIOpts_AddOpt: NULL parameters - pIWebOpts 0x%x, pOpt 0x%x", (uint32)pMe->pIWebOpts, (uint32)pOpt, 0 );
      return EBADPARM;
   }

   MSG_HIGH( "AddOpt called: id = 0x%x", pOpt->nId, 0, 0 );
   if ( SUCCESS == IWEBOPTS_RemoveOpt( pMe->pIWebOpts, pOpt->nId, 0 ))
   {
      MSG_HIGH( "Option already found in IWebOpts", 0, 0, 0 );
   }

   /* If option is an AECHAR * string, translate it to char * */
   switch ( pOpt->nId )
   {
      case AEEWIFI_OPT_SSID:
      case AEEWIFI_OPT_WEP_KEY:
      case AEEWIFI_OPT_WEP_64BIT_KEY_0:
      case AEEWIFI_OPT_WEP_64BIT_KEY_1:
      case AEEWIFI_OPT_WEP_64BIT_KEY_2:
      case AEEWIFI_OPT_WEP_64BIT_KEY_3:
      case AEEWIFI_OPT_WEP_128BIT_KEY_0:
      case AEEWIFI_OPT_WEP_128BIT_KEY_1:
      case AEEWIFI_OPT_WEP_128BIT_KEY_2:
      case AEEWIFI_OPT_WEP_128BIT_KEY_3:
      case AEEWIFI_OPT_8021X_ID:
      case AEEWIFI_OPT_8021X_PASSWORD:
      case AEEWIFI_OPT_USER_ID:
      case AEEWIFI_OPT_STATIC_IP:
      case AEEWIFI_OPT_DNS_ADDRESS:
      case AEEWIFI_OPT_SUBNET_MASK:
      case AEEWIFI_OPT_DEFAULT_GATEWAY:
#if MIN_BREW_VERSIONEx(3,1,5)
      case AEEWIFI_OPT_SECONDARY_DNS_ADDRESS:
      case AEEWIFI_OPT_DOMAIN_NAME:
#endif
         len = (uint32) WSTRLEN( (AECHAR *)( pOpt->pVal )) + 1;
         szTmp = (char *)MALLOC( len * sizeof( char ));
         if ( NULL == szTmp )
         {
            MSG_ERROR( "Could not allocate len %d bytes of memory", len, 0, 0 );
            return ENOMEMORY;
         }

         (void)WSTRTOSTR( (const AECHAR *)pOpt->pVal, szTmp, (int)len ); 
         tmp[ 0 ].pVal = szTmp;

         break;
    
       default:
         tmp[ 0 ].pVal = pOpt->pVal;
         break;
   }

   tmp[ 0 ].nId = pOpt->nId;
   if ( SUCCESS != ( ret = IWEBOPTS_AddOpt( pMe->pIWebOpts, tmp )))
   {
       MSG_ERROR("Unable to add option 0x%x with value 0x%x to IWebOpts: error 0x%x", tmp[ 0 ].nId, tmp[ 0 ].pVal, ret );
   }

   FREEIF( szTmp );

   MSG_HIGH( "AddOpt returning %d", ret, 0, 0 );
   return ret;
}

/*=============================================================================
FUNCTION: OEMWIFIOpts_RemoveOpt

DESCRIPTION:  
  Remove the ID-value pair matching the specified ID. 

PROTOTYPE:
  boolean OEMWIFIOpts_RemoveOpt(IWIFIOpts *pMe, int nOptId)

PARAMETERS:
  pMe                [in]: IWIFIOpts interface pointer.
  nOptId            [in]: Option ID.

RETURN VALUE:
  EBADPARM     if any of the parameters is NULL
  SUCCESS      if remove was successful
  EFAILED      if ID was not found

COMMENTS:
  Corresponds to IWEBOPTS_RemoveOpt() with index 0.

SIDE EFFECTS:

SEE ALSO:
  IWEBOPTS_RemoveOpt()
  
=============================================================================*/
static int OEMWIFIOpts_RemoveOpt( IWIFIOpts *pMe, int32 nOptId )
{
  int ret;
  
  if ( NULL == pMe->pIWebOpts )
  {
     MSG_ERROR( "OEMWIFIOpts_RemoveOpt: NULL parameters - pIWebOpts 0x%x", (uint32)(pMe->pIWebOpts), 0, 0 );
     return EBADPARM;
  }

  MSG_HIGH( "RemoveOpt called: id = 0x%x", nOptId, 0, 0 );
  if ( SUCCESS != ( ret = IWEBOPTS_RemoveOpt( pMe->pIWebOpts, nOptId, 0 )))
  {
     MSG_ERROR( "Unable to remove option 0x%x from IWebOpts: error 0x%x", nOptId, ret, 0 );
  } 

  MSG_HIGH( "RemoveOpt returning %d", ret, 0, 0 );
  return ret;
}

/*=============================================================================
FUNCTION: OEMWIFIOpts_GetOpt

DESCRIPTION:  
  Find the ID-value pair matching the specified ID. 

PROTOTYPE:
  int OEMWIFIOpts_GetOpt(ICalOpts *pMe, int32 nOptId, WIFIOpt *pOpt)

PARAMETERS:
  pMe                [in]: IWIFIOpts interface pointer.
  nOptId            [in]: Option ID to search for.
  pOpt          [in/out]: Reference to WIFIOpt where matching call option
                          is copied.

RETURN VALUE:
  EBADPARM     if any of the parameters is NULL
  SUCCESS      if a matching call option is found.
  EFAILED      otherwise

COMMENTS:
  Only the WIFIOpt structure is copied and not the data pointed by pVal field.

SIDE EFFECTS:

SEE ALSO:
  IWEBOPTS_GetOpt()

=============================================================================*/
static int OEMWIFIOpts_GetOpt( IWIFIOpts *pMe, int32 nOptId, WIFIOpt *pOpt )
{
  WebOpt  tmp;
  int     ret = AEE_SUCCESS;
  uint32  size;
  AECHAR *tmp_wstr;
  
  if (( NULL == pOpt ) || ( NULL == pMe->pIWebOpts ))
  {
     MSG_ERROR( "OEMWIFIOpts_GetOpt: NULL parameters - pIWebOpts 0x%x, pOpt 0x%x", (uint32)pMe->pIWebOpts, (uint32)pOpt, 0 );
     return EBADPARM;
  }

  MSG_HIGH( "GetOpt called: id = 0x%x", nOptId, 0, 0 );
  if (( ret = IWEBOPTS_GetOpt( pMe->pIWebOpts, nOptId, 0, &tmp )) != SUCCESS)
  {
     MSG_HIGH( "Unable to obtain option for id 0x%x from IWebOpts: %d", nOptId, ret, 0 );
     return ret;
  }

  /* Convert the strings to AECHAR strings. */
  switch ( nOptId )
  {
    case AEEWIFI_OPT_SSID:
    case AEEWIFI_OPT_WEP_KEY:
    case AEEWIFI_OPT_WEP_64BIT_KEY_0:
    case AEEWIFI_OPT_WEP_64BIT_KEY_1:
    case AEEWIFI_OPT_WEP_64BIT_KEY_2:
    case AEEWIFI_OPT_WEP_64BIT_KEY_3:
    case AEEWIFI_OPT_WEP_128BIT_KEY_0:
    case AEEWIFI_OPT_WEP_128BIT_KEY_1:
    case AEEWIFI_OPT_WEP_128BIT_KEY_2:
    case AEEWIFI_OPT_WEP_128BIT_KEY_3:
    case AEEWIFI_OPT_8021X_ID:
    case AEEWIFI_OPT_8021X_PASSWORD:
    case AEEWIFI_OPT_USER_ID:
    case AEEWIFI_OPT_STATIC_IP:
    case AEEWIFI_OPT_DNS_ADDRESS:
    case AEEWIFI_OPT_SUBNET_MASK:
    case AEEWIFI_OPT_DEFAULT_GATEWAY:
#if MIN_BREW_VERSIONEx(3,1,5)
    case AEEWIFI_OPT_SECONDARY_DNS_ADDRESS:
    case AEEWIFI_OPT_DOMAIN_NAME:
#endif
      if ( tmp.pVal )
      {
        size = STRLEN( (char *)tmp.pVal ) + 1;
        tmp_wstr = (AECHAR *)MALLOC( sizeof( AECHAR ) * size );
        if ( NULL != tmp_wstr )
        {
           (void)STRTOWSTR( tmp.pVal, tmp_wstr, (int)( sizeof( AECHAR ) * size ));
        }
        tmp.pVal = (void *)tmp_wstr;
      }
      break;

    case AEEWIFI_OPT_PROFILE_ID:
    case AEEWIFI_OPT_TECH:
    case AEEWIFI_OPT_BANDPREF:
    case AEEWIFI_OPT_CHANNEL:
    case AEEWIFI_OPT_GROUPS:
    case AEEWIFI_OPT_WLAN_MODE:
    case AEEWIFI_OPT_ENCRYPTION_TYPE:
    case AEEWIFI_OPT_AUTHENTICATION_TYPE:
    case AEEWIFI_OPT_MOB_IP_PROFILE_ID:
    case AEEWIFI_OPT_NESTED_AUTH_ID:
    case AEEWIFI_OPT_CERTIFICATE_ID:
    case AEEWIFI_OPT_WEP_KEY_ID:
    case AEEWIFI_OPT_WEP_64BIT_KEY_ID:
    case AEEWIFI_OPT_WEP_128BIT_KEY_ID:
    case AEEWIFI_OPT_LISTEN_INTERVAL:
    case AEEWIFI_OPT_PREAMBLE_TYPE:
    case AEEWIFI_OPT_RTSTHRESHOLD:
    case AEEWIFI_OPT_SCANTIMING:
    case AEEWIFI_OPT_POWERLEVEL:
    case AEEWIFI_OPT_FRAGMENTATIONTHRESHOLD:
#if MIN_BREW_VERSIONEx(3,1,5)
    case AEEWIFI_OPT_RSSI_THRESHOLD:
    case AEEWIFI_OPT_MTU_SIZE:
#endif
    case AEEWIFI_OPT_DHCP_ENABLED:
    case AEEWIFI_OPT_IP_TYPE:
      break;

    default:
      (void)MEMSET( (void *)&tmp, 0, sizeof(tmp) );
      ret = EFAILED;
      break;
  }
 
  pOpt->nId = tmp.nId;
  pOpt->pVal = tmp.pVal;

  MSG_HIGH( "GetOpt returning %d", ret, 0, 0 );
  return ret;
}

/*=============================================================================
FUNCTION: OEMWIFI_WriteFile

DESCRIPTION:  
  Takes the structure we currently have and writes it to the files
  based on category specified by change flag

PROTOTYPE:
  boolean OEMWIFI_WriteFile(uint32 profile_id, uint8 category);

PARAMETERS:
  profile_id  profile to update
  category    option category change flag

RETURN VALUE:
  TRUE:  If the file was written for category
  FALSE: Otherwise

COMMENTS:
  

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static boolean OEMWIFI_WriteFile( uint32 profile_id, uint8 category )
{
  boolean ret_val = TRUE;
  int32   size;

  MSG_HIGH( "WriteFile called", 0, 0, 0 );

  if ( IWIFICoreObj == NULL )
  {
     MSG_ERROR( "OEMWIFI_WriteFile: CoreObj is NULL", 0, 0, 0 );
     return FALSE;
  }

  if (category & AEEWIFI_OPT_CATEGORY_NETFILE)
  {
    size = (int32)IWIFICoreObj->BufferSize;

    ret_val = ( NET_STATUS_SUCCESS == net_file_write((byte *) IWIFICoreObj->pBuffer, &size )) ? TRUE : FALSE;
    MSG_HIGH( "Net File Write returned %d bytes", size, 0, 0 );
  }

  if (category & AEEWIFI_OPT_CATEGORY_IP)
  {
    if ( 0 != wlan_config_set_ip_params(&IWIFICoreObj->pDataRecord->ip) )
    {
       ret_val = FALSE;
    }
  }

  if (category & AEEWIFI_OPT_CATEGORY_WLAN)
  {
    if ( 0 != wlan_config_set_wlan_params(&IWIFICoreObj->pDataRecord->wlan) )
    {
       ret_val = FALSE;
    }
  }

#ifdef MULTIPROF
#ifndef AEE_SIMULATOR
  if (0 == profile_id && TRUE == ret_val)
  {
     (void)ICM_SetSubscriptionStatus ( IWIFICoreObj->pICM, AEECM_SYS_MODE_WLAN, TRUE );
  }
#endif
#endif

  MSG_HIGH( "WriteFile returning %d", ret_val, 0, 0 );
  return ret_val;
}

/*=============================================================================
FUNCTION: OEMWIFI_ReadFile

DESCRIPTION:  
  Reads the contents of the file and initializes the interface to them.
  If the file doesn't exist, or cannot be read, initialized data to default values. 

PARAMETERS:
  None

RETURN VALUE:
  FALSE: on memory allocation problems
  TRUE:  Otherwise

COMMENTS:
  This method is called only once - during IWIFICoreObj creation.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMWIFI_ReadFile( void )
{
  int32               size;
  net_sys_rec_s_type  system_record;
  net_profile_s_type  profile_record;
  boolean             netRecordExist = TRUE;

  MSG_HIGH( "ReadFile called", 0, 0, 0 );

  if ( NULL == IWIFICoreObj )
  {
     MSG_ERROR( "OEMWIFI_ReadFile: CoreObj is NULL", 0, 0, 0 );
     return EFAILED;
  }

  OEMWIFI_FREEIF( IWIFICoreObj->pBuffer );
  OEMWIFI_FREEIF( IWIFICoreObj->pDataRecord );

  IWIFICoreObj->BufferSize = NET_LIST_FILE_MAX_SIZE;
  IWIFICoreObj->pBuffer = OEMWIFI_MALLOC( NET_LIST_FILE_MAX_SIZE );
  if ( NULL == IWIFICoreObj->pBuffer )
  {
     IWIFICoreObj->BufferSize = 0;
     MSG_ERROR( "OEMWIFI_ReadFile: Out of memory", 0, 0, 0 );
     return ENOMEMORY;
  }

  IWIFICoreObj->pDataRecord = OEMWIFI_MALLOC( MAX_WLAN_FILE_SIZE );
  if ( NULL == IWIFICoreObj->pDataRecord )
  {
    OEMWIFI_FREEIF( IWIFICoreObj->pBuffer );
    IWIFICoreObj->BufferSize = 0;
    MSG_ERROR( "OEMWIFI_ReadFile: Out of memory", 0, 0, 0 );
    return ENOMEMORY;
  }

  (void)MEMSET( (void *)(IWIFICoreObj->pBuffer), 0, NET_LIST_FILE_MAX_SIZE );
  (void)MEMSET( (void *)(IWIFICoreObj->pDataRecord), 0, MAX_WLAN_FILE_SIZE );

  size = (int32)IWIFICoreObj->BufferSize;
  if ( NET_STATUS_SUCCESS != net_file_read ( IWIFICoreObj->pBuffer, &size )) 
  {
     netRecordExist = FALSE;
     size = (word)IWIFICoreObj->BufferSize;
     (void)MEMSET((void *)(IWIFICoreObj->pBuffer), 0, (word)size );
     (void)net_parser_make( &IWIFICoreObj->Parser, IWIFICoreObj->pBuffer, (word)size );
  }

  if ( TRUE == netRecordExist )
  {
     if ( NET_STATUS_SUCCESS != net_parser_init( &IWIFICoreObj->Parser, IWIFICoreObj->pBuffer, (word)size ))
     {
        MSG_ERROR( "Parser Init failed.", 0, 0, 0 ); 

        /* try to initialize to defaults */
        (void)net_parser_make( &IWIFICoreObj->Parser, IWIFICoreObj->pBuffer, (word)size );
        netRecordExist = FALSE;
     }
  }

  /* Create default records if file didn't exist */
  if ( FALSE == netRecordExist )
  {
     OEMWIFI_Get_DefaultSysRec( &system_record );
     (void)net_insert_sys_rec( 0, &system_record, &IWIFICoreObj->Parser );
  
     OEMWIFI_Get_DefaultProfileRec( &profile_record, 0 );
     (void)net_insert_profile( &profile_record, &IWIFICoreObj->Parser );
  }

  if ( 0 != wlan_config_get_wlan_params( &IWIFICoreObj->pDataRecord->wlan ))
  {
      (void)MEMSET( (void *) & (IWIFICoreObj->pDataRecord->wlan), 0, sizeof( IWIFICoreObj->pDataRecord->wlan ));
      
      /* Philips WiFi module defaults */
      IWIFICoreObj->pDataRecord->wlan.listen_interval = 10;
      IWIFICoreObj->pDataRecord->wlan.preamble_type = 0;
      IWIFICoreObj->pDataRecord->wlan.rts_threshold = 2432;
      IWIFICoreObj->pDataRecord->wlan.scan_timing = 120;
      IWIFICoreObj->pDataRecord->wlan.frag_threshold = 2346;
      IWIFICoreObj->pDataRecord->wlan.max_tx_power = 0x00C5;
      IWIFICoreObj->pDataRecord->wlan.rssi_threshold = 0;

      (void)wlan_config_set_wlan_params( &IWIFICoreObj->pDataRecord->wlan );
  }

  if ( 0 != wlan_config_get_ip_params( &IWIFICoreObj->pDataRecord->ip ))
  {
      (void)MEMSET( (void *) & (IWIFICoreObj->pDataRecord->ip), 0, sizeof( IWIFICoreObj->pDataRecord->ip ));
      
      /* Philips WiFi module defaults */
      IWIFICoreObj->pDataRecord->ip.dhcp_enabled = 1;
      IWIFICoreObj->pDataRecord->ip.subnet_mask = 0xFFFFFF00;
      IWIFICoreObj->pDataRecord->ip.mtu_size = 1500;

      (void)wlan_config_set_ip_params( &IWIFICoreObj->pDataRecord->ip );
  }

  MSG_HIGH( "ReadFile returning SUCCESS", 0, 0, 0 );
  return AEE_SUCCESS;
}

/*===========================================================================

                    IWIFICore functions

===========================================================================*/

/* Abnormal exit callback to clean up system memory */
static void OEMWIFICoreObjCB( void *pUsr )
{
   (void)OEMWIFICore_Release();
}

/*=============================================================================
FUNCTION: OEMWIFICore_New

DESCRIPTION:  
  Returns a pointer to new CoreObj or NULL if out of memory

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static IWIFICore *OEMWIFICore_New( IShell *pIShell, int *error_code )
{
  ACONTEXT *pac = NULL;

  *error_code = AEE_SUCCESS;

  if ( NULL != IWIFICoreObj )
  { 
    (void)OEMWIFICore_AddRef();
  } 
  else
  {
     pac = AEE_EnterAppContext( 0 );

     IWIFICoreObj = (IWIFICore *)OEMWIFI_MALLOC( sizeof( IWIFICore ));
     if (( NULL == IWIFICoreObj ) || ( NULL == pIShell ))
     {
        MSG_ERROR( "OEMWIFICore_New: out of memory", 0, 0, 0 );
        *error_code = ENOMEMORY;
        AEE_LeaveAppContext( pac );
        return NULL;
     }

     IWIFICoreObj->uRefs = 1;
     if ( AEE_SUCCESS != ( *error_code = OEMWIFI_ReadFile()))
     {
        MSG_ERROR( "OEMWIFICore_New: Cannot read data file", 0, 0, 0 );
        goto cleanup;
     }

#ifndef AEE_SIMULATOR
     if ( AEE_SUCCESS != ( *error_code = ISHELL_CreateInstance( pIShell, AEECLSID_CM, (void **) &IWIFICoreObj->pICM )))
     {
        MSG_ERROR( "OEMWIFICore_New: cannot create CM object", 0, 0, 0 );
        goto cleanup;
     }
#endif

     CALLBACK_Init( &IWIFICoreObj->cbSysObj, OEMWIFICoreObjCB, IWIFICoreObj );
#if MIN_BREW_VERSION(3,0)
     AEE_LinkSysObject( &IWIFICoreObj->cbSysObj );
#else
     AEE_LinkSysObject( (AEESysObject *)&IWIFICoreObj->cbSysObj );
#endif
     AEE_LeaveAppContext( pac );
  }
  
  return IWIFICoreObj;

cleanup:
  OEMWIFI_FREEIF(IWIFICoreObj->pBuffer);
  OEMWIFI_FREEIF(IWIFICoreObj->pDataRecord);
  OEMWIFI_FREEIF( IWIFICoreObj );
  AEE_LeaveAppContext( pac );
  return NULL;

}

/*=============================================================================
FUNCTION: OEMWIFICore_AddRef

DESCRIPTION:  
  Increments object reference count and returns the new incremented reference count


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static uint32 OEMWIFICore_AddRef()
{
  IWIFICore *pMe = IWIFICoreObj;

  return (( NULL == pMe ) ? 0 : ++pMe->uRefs );
}

/*=============================================================================
FUNCTION: OEMWIFICore_Release

DESCRIPTION:
  Decrements the reference count for an object and returns the current decremented reference count  
  Frees the object when reference count reaches 0 

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static uint32 OEMWIFICore_Release()
{
   IWIFICore *pMe = IWIFICoreObj;

   if ( NULL == pMe )
   {
      return 0;
   }

   if ( --pMe->uRefs != 0 )
   {
      return pMe->uRefs;
   }

   // Ref count is zero.  Release memory
  CALLBACK_Cancel( &pMe->cbSysObj );
#ifndef AEE_SIMULATOR
  ICM_Release(pMe->pICM);
#endif
  OEMWIFI_FREEIF(pMe->pBuffer);
  OEMWIFI_FREEIF(pMe->pDataRecord);
  OEMWIFI_FREEIF(pMe);
  IWIFICoreObj = NULL;

  return 0;
}

/*=============================================================================

                       Other utility functions

=============================================================================*/

/*=============================================================================
FUNCTION: OEMWIFI_Malloc

DESCRIPTION: 
  Allocates a 0 initializes block of memory of at least 'size' bytes 


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void * OEMWIFI_Malloc( uint32 size, int line )
{
  void *p;
  
  if ( 0 == size )
  {
    return NULL;
  }

  if ( NULL == ( p = (void *)OEM_Malloc( size )))
  {
#ifdef IWIFI_MEM_DEBUG
    MSG_ERROR( "Failed to allocate memory %d in line %d, total size %d", 
                size, line, IWIFICoreObj ? IWIFICoreObj->malloc_cnt : 0 );
#endif
    return NULL;
  }

#ifdef IWIFI_MEM_DEBUG
  if ( NULL != IWIFICoreObj )
  {
     IWIFICoreObj->malloc_cnt++;
     MSG_HIGH( "IWIFI: Malloc cnt = %d, size = %d", IWIFICoreObj->malloc_cnt,size, 0 );
  }
#endif

  return p;
}

/*=============================================================================
FUNCTION: OEMWIFI_Free

DESCRIPTION:  
  Frees memory previosly allocated with OEMWIFI_Malloc


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMWIFI_Free( void *p ) 
{
  if (p) {
    (void)OEM_Free( p );

#ifdef IWIFI_MEM_DEBUG
    if ( NULL != IWIFICoreObj )
    {
       IWIFICoreObj->malloc_cnt--;

       MSG_HIGH( "IWIFI: (Free) Malloc cnt = %d", IWIFICoreObj->malloc_cnt, 0, 0 );
    }
#endif
  }
}

/* lint -restore */
#endif // FEATURE_WLAN
