/*===========================================================================

FILE: OEMKeysMapping.c
===========================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */

#include "AEEShell.h"           // Shell interface definitions
#include "AEEStdLib.h"
#include "AEEClassIDs.h"
#include "AEEQueryInterface.h"
#include "AEEKeysMapping.h"
#include "AEECLSID_KEYSMAPPING.bid"
#include "AEEFile.h"
#include "AEESource.h"
#include "AEEModGen.h"
#include "AEE_OEM.h"
#include "OEMFeatures.h"

#ifdef _WIN32
#include "BREWSimFeatures.h"
#endif

#if defined (FEATURE_EXTENDED_KEYS)

#if defined (FEATURE_KEYS_CONFIG)
#include "AEEKeysConfig.h"
#include "AEEBase.h"
#endif //FEATURE_KEYS_CONFIG

#ifdef _WIN32
extern const char *AEESIM_GetDevicePackDir();
extern boolean AEESIM_GetSimRootDir(char *pBuff, int *nSize);
extern boolean AEESIM_CopyFile(char *from, char *to);
#define SIM_MAPPINGS_FILE "map.csv"
#endif //_WIN32



#define FILE_PATH_PREFIX_LEN 18 //Length of "fs:/~0x01800002/"
#define MAPPINGS_FILE "map.csv"
#define LINE_LEN 100 //Max string length of one line in maps.csv



typedef struct _KeysTriad {
	uint16 avk;
	uint32 modifiers;
	AECHAR mapping;
}KeysTriad;

#if defined (FEATURE_KEYS_CONFIG)
struct IKeysConfig
{
   AEEBASE_INHERIT(IKeysConfig, IBase);

};
#endif 

typedef struct _CKeysMapping {
  AEEVTBL(IKeysMapping) *pvt;
   IShell *pIShell;
   uint32 nRefs;
   KeysTriad *mapTable;
   int numEntries;
#if defined (FEATURE_KEYS_CONFIG)
   IKeysConfig config;
#endif
} CKeysMapping;




/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/

static int IKeysMapping_CreateInstance(AEECLSID clsid, IShell *pIShell, void **ppobj);
static int ReadIntoMemory(CKeysMapping *pme, IFile *pf);
static uint32 IKeysMapping_AddRef(IKeysMapping *pme);
static uint32 IKeysMapping_Release(IKeysMapping *pme);
static int IKeysMapping_QueryInterface(IKeysMapping *pme, AEECLSID cls, void **p);
static int IKeysMapping_GetMapping(IKeysMapping *pme, uint16 avk, uint32 modifiers, AECHAR *mapping);
int AEEKeysMapping_New(IShell *ps, AEECLSID cls, void **ppif);

#if defined (FEATURE_KEYS_CONFIG)
static int IKeysConfig_GetStickyKeys(IKeysConfig *pme, uint32 *pdwKeys);
static int IKeysConfig_GetKeyState(IKeysConfig *pme, uint32 dwKey, boolean *pbState);
static int IKeysConfig_SetKeyState(IKeysConfig *pme, uint32 dwKey, boolean bState);
#endif //FEATURE_KEYS_CONFIG

static AEEVTBL(IKeysMapping) gvtKeysMapping = {
    IKeysMapping_AddRef,
    IKeysMapping_Release,
    IKeysMapping_QueryInterface,
    IKeysMapping_GetMapping
};

#if defined (FEATURE_KEYS_CONFIG)
static AEEVTBL(IKeysConfig) gvtKeysConfig = {
   AEEBASE_AddRef(IKeysConfig),
   AEEBASE_Release(IKeysConfig),
   AEEBASE_QueryInterface(IKeysConfig),
   IKeysConfig_GetStickyKeys,
   IKeysConfig_GetKeyState,
   IKeysConfig_SetKeyState
};
#endif //FEATURE_KEYS_CONFIG


/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

/*===========================================================================
FUNCTION: AEEClsCreateInstance

DESCRIPTION


PROTOTYPE:
   int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)

PARAMETERS:
	clsID: [in]: Specifies the ClassID of the module which is being loaded

	pIShell: [in]: Contains pointer to the IShell object. 

	pIModule: pin]: Contains pointer to the IModule object to the current module to which
	this app belongs

	ppObj: [out]

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: When the module is successfully created
  EBADCLASS: Called with wrong class ID
  EFAILED: Other failures - such as missing file

SIDE EFFECTS
  none
===========================================================================*/

int AEEKeysMapping_New(IShell *ps, AEECLSID cls, void **ppif)
{
	*ppif = NULL;

	if( cls == AEECLSID_KEYSMAPPING)
	{
		return IKeysMapping_CreateInstance(cls, ps, ppif);
		
    }

	return(EBADCLASS);
}


static int IKeysMapping_CreateInstance(AEECLSID clsid, IShell *pIShell, void **ppobj)
{
	CKeysMapping *pme;

	int nErr;
	IFileMgr *pfm;
	IFile *pf;
	char *pszFilePath = NULL;
	PACONTEXT pacRestore;

	pme = MALLOCREC(CKeysMapping);

	if( NULL == pme) return ENOMEMORY;

	*ppobj = pme;

	ZEROAT(pme);

	//Now initialize the data members


    pme->pvt = &gvtKeysMapping;
	pme->nRefs = 1;
	pme->pIShell = pIShell;

	ISHELL_AddRef(pIShell);



	//Open the mappings file

	nErr = AEE_SUCCESS;

#ifdef _WIN32
	{
		int nSize;
		char *szDPKDir, *szRootDir;
		boolean bDone;

		//Query for size of DPK directory
        if( !AEESIM_GetDevicePackDir(NULL, &nSize) )
		{
			nErr = EFAILED;
			goto error;
		}
		

		//Length = size of DPK DIR + 1 + "." + length of mappings file string

		szDPKDir = MALLOC(nSize + 1 + STRLEN(SIM_MAPPINGS_FILE));

		if( !szDPKDir )
		{
			nErr = ENOMEMORY;
			goto error;
		}

		if ( !AEESIM_GetDevicePackDir(szDPKDir, &nSize) )
		{
			nErr = EFAILED;
			goto error;
		}

		STRCAT(szDPKDir, SIM_MAPPINGS_FILE);

		//Now get the simulator root directory

		if( !AEESIM_GetSimRootDir(NULL, &nSize) )
		{
			nErr = EFAILED;
			FREEIF(szDPKDir);
			goto error;
		}

		szRootDir = MALLOC(nSize + 1 + STRLEN(SIM_MAPPINGS_FILE));

		if( !szRootDir )
		{
			nErr = ENOMEMORY;
			FREEIF(szDPKDir);
			goto error;
		}

		if( !AEESIM_GetSimRootDir(szRootDir, &nSize) )
		{
			nErr = EFAILED;
			FREEIF(szDPKDir);
			FREEIF(szRootDir);
			goto error;
		}

        STRCAT(szRootDir, SIM_MAPPINGS_FILE);

		bDone = AEESIM_CopyFile( szDPKDir, szRootDir);

		FREEIF(szDPKDir);
		FREEIF(szRootDir);

		if( !bDone )
		{
			nErr = EFAILED;
			goto error;
		}


		pszFilePath = MALLOC(STRLEN(SIM_MAPPINGS_FILE) + 1);

		if(!pszFilePath)
		{
			nErr = ENOMEMORY;
			goto error;
		}

		STRCPY(pszFilePath, SIM_MAPPINGS_FILE);

	}
#else
    pszFilePath = MALLOC(STRLEN(MAPPINGS_FILE) + 1);

	if(!pszFilePath)
	{
		nErr = ENOMEMORY;
		goto error;
	}
	STRNCPY(pszFilePath, MAPPINGS_FILE, STRLEN(MAPPINGS_FILE));

#endif

	pacRestore = AEE_EnterAppContext(0);

	if( 0 !=  (nErr = ISHELL_CreateInstance(pme->pIShell, AEECLSID_FILEMGR, (void **)&pfm)) )
	{
		AEE_LeaveAppContext(pacRestore);
		goto error;
	}
	pf = IFILEMGR_OpenFile(pfm, pszFilePath, _OFM_READ);
	if( !pf )
    {
			DBGPRINTF("Could not open map.csv, err: %d",IFILEMGR_GetLastError(pfm)); 
   		    IFILEMGR_Release(pfm);
			AEE_LeaveAppContext(pacRestore);
       	    nErr = EFAILED;		    
            goto error;
   	}

	nErr = ReadIntoMemory(pme, pf);
	
	IFILE_Release(pf);
	IFILEMGR_Release(pfm);

	AEE_LeaveAppContext(pacRestore);

error:
	FREEIF(pszFilePath);
	if(nErr)
	{
		FREEIF(pme);
		ISHELL_Release(pIShell);
	}


	return nErr;
}



static int ReadIntoMemory(CKeysMapping *pme, IFile *pf)
{
	int nErr = AEE_SUCCESS;
	char *szReadBuf;
	int numExpMappings=0, numActualMappings=0;
	KeysTriad *k;
	ISourceUtil * pisu;
	ISource * pISource;
	IGetLine * piGetLine;
	GetLine gl;
	int rv;
	char *pos1, *pos2;

	
	if (ISHELL_CreateInstance(pme->pIShell,AEECLSID_SOURCEUTIL,(void **)&pisu))
		return EFAILED;


	
	if (ISOURCEUTIL_SourceFromAStream(pisu,(IAStream*)pf, &pISource))
	{
		ISOURCEUTIL_Release(pisu);
		return EFAILED;
	}


	if (ISOURCEUTIL_GetLineFromSource(pisu, pISource, LINE_LEN, &piGetLine))
	{
		ISOURCEUTIL_Release(pisu);
		return EFAILED;
	}

	
	rv = IGETLINE_GetLine(piGetLine, &gl, IGETLINE_LF);

	if ( IGETLINE_Exhausted(rv)  || !STRBEGINS("Count=",gl.psz) )  {
		nErr = EFAILED;
  	    goto error;		
	}


	numExpMappings = ATOI( (char *)( (byte *)gl.psz + STRLEN("Count=") ) );

	if( 0 == numExpMappings ) //Don't need this interface if no mappings!
	{
		nErr = EFAILED;
		goto error;
	}


	pme->mapTable = (KeysTriad *)MALLOC(sizeof(KeysTriad) * numExpMappings);
	pme->numEntries = numExpMappings;

	if( !pme->mapTable )
	{
		nErr = ENOMEMORY;
		goto error;
	}

	pme->numEntries = numExpMappings;


	k = pme->mapTable;

	while( 1 )
	{

		rv = IGETLINE_GetLine(piGetLine, &gl, IGETLINE_LF);

		if( IGETLINE_Exhausted(rv) )
			break;


		szReadBuf = gl.psz;
		pos1 = STRCHR(szReadBuf, ',');
		*pos1 = '\0';
		++pos1;
		pos2= STRCHR(pos1, ',');
		*pos2 = '\0';
		++pos2;

		k->avk = ATOI(szReadBuf);
		k->modifiers = ATOI(pos1);
		k->mapping = ATOI(pos2);


		++numActualMappings;
		++k;
	}

	//If the mappings do not match, there has to be a problem
	//This is because maps.csv is created by the tool, not manually
	if(numActualMappings != numExpMappings)
	{
		DBGPRINTF("Error: Expected %d mappings: Got %d mappings", numExpMappings, numActualMappings);
		nErr = EFAILED;
	}

error:
	  ISOURCEUTIL_Release(pisu);
	  ISOURCE_Release(pISource);
	  IGETLINE_Release(piGetLine);
      if(nErr)
		FREEIF(pme->mapTable);
		


    return nErr;

}


static uint32 IKeysMapping_AddRef(IKeysMapping *po)
{
	CKeysMapping * pme = (CKeysMapping *)po;
	return ( ++pme->nRefs );
}

static uint32 IKeysMapping_Release(IKeysMapping *po)
{
   CKeysMapping *pme = (CKeysMapping *)po;

   if(pme->nRefs){
      if(--pme->nRefs)
         return(pme->nRefs);

	  FREEIF(pme->mapTable);
	  ISHELL_Release(pme->pIShell);
      FREE(pme);
   }

   return 0;
}

static int IKeysMapping_QueryInterface(IKeysMapping *po, AEECLSID cls, void **p)
{
    IBase * pb = 0;
	CKeysMapping *pMe = (CKeysMapping *)po;

	if( !p )
		return EBADPARM;

	switch(cls)
	{

		case AEEIID_KEYSMAPPING:
		case AEECLSID_QUERYINTERFACE:
			 pb = (IBase *)pMe;
			 break;

#if defined (FEATURE_KEYS_CONFIG)
		case AEEIID_KEYSCONFIG:
            pb = (IBase*)&pMe->config;
			break;
#endif //FEATURE_KEYS_CONFIG

		default:
			break;

   }

   *p = pb;

   if(pb)
   {
	   IBASE_AddRef(pb);
	   return SUCCESS;
   }

   return ECLASSNOTSUPPORT;

}

static int IKeysMapping_GetMapping(IKeysMapping *po, uint16 avk, uint32 modifiers, AECHAR *mapping)
{
	int i;
	CKeysMapping *pme = (CKeysMapping *)po;

	if( !mapping )
		return EBADPARM;


	/* These modifiers can be added by BREW
	 *  But they are not accounted for by the mappings file
	 *  Hence, remove them before checking the map table
     */
    modifiers &= ~KB_AUTOREPEAT;
    modifiers &= ~KB_EXTERNAL;

	for(i=0; i<pme->numEntries ; ++i)
	{
		if( (pme->mapTable[i].avk == avk) && (pme->mapTable[i].modifiers == modifiers) )
		{
			*mapping = pme->mapTable[i].mapping;
			return AEE_SUCCESS;
		}
	}

	return EFAILED;				
}

#if defined (FEATURE_KEYS_CONFIG)

static int IKeysConfig_GetStickyKeys(IKeysConfig *pme, uint32 *pdwKeys)
{

	if( pdwKeys == NULL)
		return EBADPARM;

#error OEM must implement this function
#error Return a bit mask of all sticky keys present on the handset
#error For example if you have Caps Lock and Num Lock, 
#error set *pdwKeys to KB_CAPSLOCK | KB_NUMLOCK

	return EFAILED;

}


static int IKeysConfig_GetKeyState(IKeysConfig *pme, uint32 dwKey, boolean *pbState)
{

    if( pbState == NULL)
		return EBADPARM;

#error OEM must implement this function
#error Return TRUE in *pbState if sticky key "dwKey" (ex. KB_CAPSLOCK) is currently active
#error Return FALSE otherwise
	return EFAILED:

}


static int IKeysConfig_SetKeyState(IKeysConfig *pme, uint32 dwKey, boolean bState)
{
#error OEM must implement this function
#error Call code to set the key state of "dwKey" (ex. KB_CAPSLOCK) to the required state:
#error bState == TRUE: Set the key state to ON
#error bState == FALSE: Set the key state to OFF
	return EFAILED;

}

#endif //FEATURE_KEYS_CONFIG



#endif //FEATURE_EXTENDED_KEYS
