#if !defined(AEECLIPBOARD_H)
#define AEECLIPBOARD_H
/*======================================================
FILE:  AEEClipboard.h

SERVICES:  AEE IClipboard Interface

GENERAL DESCRIPTION:
	Base level definitions, typedefs, etc. for AEE IClipboard

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEE.h"

#define CLIPBOARD_MAX_DATA    (4096)

typedef struct _IClipboard    IClipboard;

typedef struct _CBItemInfo
{
   char *      pszName;
   int         nNameLen;
   int         nDataSize;
} CBItemInfo;

//********************************************************************************************************************************
//
// IClipboard Interface
//
//********************************************************************************************************************************

QINTERFACE(IClipboard)
{
   DECLARE_IBASE(IClipboard)

   int         (*Empty)(IClipboard* po);
   uint32      (*GetSequence)(IClipboard * po);
   boolean     (*EnumInit)(IClipboard * po);
   boolean     (*EnumNext)(IClipboard * po, CBItemInfo * pci);
   int         (*GetData)(IClipboard * po,const char * pszName,void * pDest, int * pnDestSize);
   int         (*SetData)(IClipboard * po,const char * pszName,void * pSrc, int nSrcSize);
   uint32      (*MaxFreeBlock)(IClipboard * po,uint32 * pdwTotalSize, uint32 * pdwTotalUsed);
};

// Standard IBase Methods

#define ICLIPBOARD_AddRef(p)              GET_PVTBL(p,IClipboard)->AddRef(p)
#define ICLIPBOARD_Release(p)             GET_PVTBL(p,IClipboard)->Release(p)

// IClipboard Methods

#define ICLIPBOARD_Empty(p)               GET_PVTBL(p,IClipboard)->Empty(p)
#define ICLIPBOARD_GetSequence(p)         GET_PVTBL(p,IClipboard)->GetSequence(p)
#define ICLIPBOARD_EnumInit(p)            GET_PVTBL(p,IClipboard)->EnumInit(p)
#define ICLIPBOARD_EnumNext(p,pi)         GET_PVTBL(p,IClipboard)->EnumNext(p,pi)
#define ICLIPBOARD_GetData(p,cf,pd,pu)    GET_PVTBL(p,IClipboard)->GetData(p,cf,pd,pu)
#define ICLIPBOARD_SetData(p,cf,pd,u)     GET_PVTBL(p,IClipboard)->SetData(p,cf,pd,u)
#define ICLIPBOARD_MaxFreeBlock(p,pt,pu)  GET_PVTBL(p,IClipboard)->MaxFreeBlock(p,pt,pu)

/*
======================================================================= 
  INTERFACES   DOCUMENTATION
======================================================================= 

IClipboard Interface

Description:
	This interface implements the cut and paste features that are implemented in the
	AEE.

For associated events, please use : 
~
  EVT_CB_CUT  ~
  EVT_CB_COPY ~
  EVT_CB_PASTE
*

The following header file is required:

AEEClipboard.h

=============================================================================

ICLIPBOARD_AddRef()

Description:
    This function is inherited from IBASE_AddRef(). 

Version:
   Introduced BREW Client 2.0

See Also:
    ICLIPBOARD_Release()    

=======================================================================

ICLIPBOARD_Empty()

Description:
	This function removes all data in all formats from the clipboard. If the last
	app doesn't free the clipboard by calling ICLIPBOARD_Empty, and BREW exits when the
	handset is power-cycled, all the data on the clipboard will be lost.

Prototype:
    int ICLIPBOARD_Empty(IClipboard* pIClipboard)

Parameters:
    pIClipboard :  Pointer to the IClipboard Interface object.

Return Value:
    SUCCESS, if the clipboard is emptied.

    EFAILED, if the clipboard is not emptied.

Comments:
    None

Version:
   Introduced BREW Client 2.0

See Also:
    None

=======================================================================

ICLIPBOARD_EnumInit()

Description: 
	This function initializes the enumeration context for the formats in 
	the clipboard.  

Prototype:
    boolean ICLIPBOARD_EnumInit(IClipboard * pIClipboard)

Parameters:
    pIClipboard :  pointer to the IClipboard Interface object

Return Value:
	TRUE - Clipboard formats are available

    FALSE - No registered formats

Comments:  
    None

Version:
   Introduced BREW Client 2.0

See Also: 
    ICLIPBOARD_EnumNext

=======================================================================

ICLIPBOARD_EnumNext()

Description: 
	This function returns the next available/registered clipboard format.

Prototype:
	boolean ICLIPBOARD_EnumNext(IClipboard * pIClipboard,CBItemInfo * pci);

Parameters:
	pIClipboard: Clipboard interface object
	pci:         Pointer to item information to fill. Before calling this function, the members
	pszName and nNameLen members of the pci parameter (that is, CBItemInfo struct) must be set properly. 
	On return, pszName will contain the name of the format and nDataSize will contain the actual size of the data.
~
	On input:~
		pci->pszName must be a valid pointer capable of holding a string.
		pci->nNameLen specifies the size (in bytes) that the pointer pci->pszName can hold~

    On Return:~
		 pci->pszName contains the format name
		 pci->nDataLen contains the size of the data in the clipboard for this format
*
Return Value:
	TRUE - Format returned

	FALSE - No more items

Comments:  
	None

Version:
   Introduced BREW Client 2.0

See Also: 
	CBItemInfo
	ICLIPBOARD_EnumInit

=======================================================================

ICLIPBOARD_GetData()

Description: 
    This function copies clipboard data for the specified clipboard format to the
	destination buffer. The size copied is returned in the size parameter.
	Note:
     -  Passing a destination of NULL fills the size with the destination size.
     -  Passing a destination size that is too small returns an error.

Prototype:
	int ICLIPBOARD_GetData(IClipboard* pIClipboard, const char * pszName, void* pData, int * pnDest);

Parameters:

	pIClipboard: Clipboard interface object
	pszName:	 Name of the format
	pData:		 Pointer to destination data buffer
	pnDest:		 Input size of destination buffer and output size copied.

Return Value:
	0 (zero)       - Success

	EFAILED  - Format is not supported.

	EBADPARM - Invalid parameter

	ENOMEMORY- Insufficient buffer size passed to function

Comments:  
	None

Version:
   Introduced BREW Client 2.0

See Also: 
	None

=======================================================================

ICLIPBOARD_GetSequence()

Description: 
	This function returns the sequence number of the clipboard. This function is
	provided to allow the caller to determine if the clipboard has been updated. Updates
	to the clipboard include the addition of data. The sequence number is only counted
	up on data addition. Data removal has no effect on the sequence number. The sequence
	number resets when BREW exits; that is, when the handset is power-cycled.

Prototype:
	uint32 ICLIPBOARD_GetSequence(IClipboard* pIClipboard)

Parameters:
	pIClipboard: Clipboard interface object

Return Value:
	Sequence number of the clipboard

Comments:  
	None

Version:
   Introduced BREW Client 2.0

See Also: 
	None

=======================================================================

ICLIPBOARD_MaxFreeBlock()

Description: 
    This function returns the maximum free block size. Fills pointers to the total
	size (in bytes) and total used size (in bytes) of the clipboard.

Prototype:
	uint32 ICLIPBOARD_MaxFreeBlock(IClipboard* pIClipboard, uint32 * pdwTotal, uint32 * pdwUsed);

Parameters:

	pIClipboard: Clipboard interface object
	pdwTotal:    Pointer to fill with total size in bytes of the clipboard memory (if non-NULL)
	pdwUsed:     Pointer to fill with total space used in clipboard (if non-NULL)

Return Value:
	Size of largest free block of memory in clipboard.

Comments:  
	None

Version:
   Introduced BREW Client 2.0

See Also: 
	None

=============================================================================

ICLIPBOARD_Release()

Description:
    This function is inherited from IBASE_Release(). 

See Also:
    ICLIPBOARD_AddRef()    

=============================================================================

ICLIPBOARD_SetData()

Description: 
    This function sets the data for a clipboard format. Because clipboard data can
	live beyond the life of an application, this function allocates new memory and copies
	the contents into a new buffer.
    Note:
     -    Passing a NULL pointer or a size of 0 frees existing clipboard data.
     -    The clipboard can contain only single data for a certain format name
     -    Data already associated with the format is released.

Prototype:
	int ICLIPBOARD_SetData(IClipboard* pIClipboard, const char * pszName, void* pData, int nSize)

Parameters:

	pIClipboard: Clipboard interface object
	pszName:	 Format name
	pData:		 Data pointer
	nSize:		 Size of data object

Return Value:
	0  (zero)         - Data copied

	EFAILED     - Clipboard format not registered

	ENOMEMORY   - Insufficient RAM

Comments:  
	None

Version:
   Introduced BREW Client 2.0

See Also: 
	None

=======================================================================

*/
#endif    // AEECLIPBOARD_H



