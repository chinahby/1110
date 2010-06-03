#ifndef _OEM_IMM_HEADER_H_ // {
#define _OEM_IMM_HEADER_H_
/*=============================================================================

FILE: OEMIMM.h

SERVICES: OEM Functions Input Method Manager

GENERAL DESCRIPTION:
         Input Method manager header

        Copyright 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#include "AEE.h"
#include "AEEText.h"

#define AEECLSID_OEMIMMGR    	(0x0101278e)

// System context options
#define IMMCTX_EMAIL          (0x0001)          // Match words for email type applications
#define IMMCTX_NAME           (0x0002)          // Match words for default name setting
#define IMMCTX_SURNAME        (0x0004)          // Match words for surnames, if supported
#define IMMCTX_GIVENNAME      (0x0008)          // Match words for given names, if supported
#define IMMCTX_PUNCTUATION    (0x0010)          // Match punctuation, if supported
#define IMMCTX_COMMON         (0x0020)          // Match only common items, if supported
#define IMMCTX_NODUPE         (0x0040)          // Match only one character per entry, if supported

#define IMMCTX_ALL            (0x007F)          // All Ctx Options
#define IMMCTX_LAST           (IMMCTX_NODUPE)   // Last Ctx Option

// Key press result options
#define IMMKEY_DEFAULT        (0x0000)          // System default
#define IMMKEY_MATCHONE       (0x0001)          // Match Key only
#define IMMKEY_MATCHMANY      (0x0002)          // Match other expected entries not on this key also

#define IMMKEY_LAST           (IMMKEY_MATCHMANY)// Last Key Option

// IMMgr Options
#define IMGRO_MAXCANDIDATE    (0x0001)          // pData = uint16 *, default value = 64
#define IMGRO_SYSTEMCONTEXT   (0x0002)          // pData = uint16 * of IMMCTX_*, default value is system dependant
#define IMGRO_KEYOPTIONS      (0x0003)          // pData = uint16 * of IMMKEY_*, default value is system dependant

typedef struct IIMMgr IIMMgr;

AEEINTERFACE(IIMMgr) 
{   
   INHERIT_IQueryInterface(IIMMgr);
   
   boolean  (*HandleEvent)(IIMMgr * po, AEEEvent evt, uint16 wp, uint32 dwp);
   void     (*Reset)(IIMMgr * po);
   int      (*Initialise)(IIMMgr * po);
   int      (*EnumModeInit)(IIMMgr * po);
   int      (*EnumNextMode)(IIMMgr * po, AEETextInputMode * pmode);
   int      (*SetInputMode)(IIMMgr * po, AEETextInputMode mode);
   int      (*GetInputMode)(IIMMgr * po, AEETextInputMode * pmode);
   int      (*SetOpt)(IIMMgr * po, uint16 wOption, void * pData, uint32 nSize);
   int      (*GetOpt)(IIMMgr * po, uint16 wOption, void * pData, uint32 nSize);
   int      (*GetCompositionCandidates)(IIMMgr * po, AECHAR * pBuff, int nBuffSize, int nFirst, int * pnNumCandidates, int * pnMaxChars);
   int      (*GetCompleteCandidates)(IIMMgr * po, AECHAR * pBuff, int nBuffSize, int nFirst, int * pnNumCandidates, int * pnMaxChars);
   boolean  (*IsCompleteCandidate)(IIMMgr * po, const AECHAR * pBuff);
   int      (*GetData)(IIMMgr * po, AECHAR * pBuff, uint16 * pwBuffSize);
   int      (*SetData)(IIMMgr * po, const AECHAR * pBuff);
   int      (*GetAssociatedCandidates)(IIMMgr * po, AECHAR * pBuff, int nBuffSize, int nFirst, int * pnNumCandidates, int * pnMaxChars);
   int      (*GetCompositionCount)(IIMMgr * po, int * pnElements);
};

#define IIMMGR_AddRef(p)                                 AEEGETPVTBL((p),IIMMgr)->AddRef((p))
#define IIMMGR_Release(p)                                AEEGETPVTBL((p),IIMMgr)->Release((p))
#define IIMMGR_QueryInterface(p,clsid,pp)                AEEGETPVTBL((p),IIMMgr)->QueryInterface((p),(clsid),(pp))
#define IIMMGR_HandleEvent(p,ec,wp,dw)                   AEEGETPVTBL((p),IIMMgr)->HandleEvent((p),(ec),(wp),(dw))
#define IIMMGR_Reset(p)                                  AEEGETPVTBL((p),IIMMgr)->Reset((p))
#define IIMMGR_Initialise(p)                             AEEGETPVTBL((p),IIMMgr)->Initialise((p))
#define IIMMGR_EnumModeInit(p)                           AEEGETPVTBL((p),IIMMgr)->EnumModeInit((p))
#define IIMMGR_EnumNextMode(p,m)                         AEEGETPVTBL((p),IIMMgr)->EnumNextMode((p),(m))
#define IIMMGR_SetInputMode(p,m)                         AEEGETPVTBL((p),IIMMgr)->SetInputMode((p),(m))
#define IIMMGR_GetInputMode(p,m)                         AEEGETPVTBL((p),IIMMgr)->GetInputMode((p),(m))
#define IIMMGR_SetOpt(p,o,d,s)                           AEEGETPVTBL((p),IIMMgr)->SetOpt((p),(o),(d),(s))
#define IIMMGR_GetOpt(p,o,d,s)                           AEEGETPVTBL((p),IIMMgr)->GetOpt((p),(o),(d),(s))
#define IIMMGR_GetCompositionCandidates(p,b,n,f,pn,pm)   AEEGETPVTBL((p),IIMMgr)->GetCompositionCandidates((p),(b),(n),(f),(pn),(pm))
#define IIMMGR_GetCompleteCandidates(p,b,n,f,pn,pm)      AEEGETPVTBL((p),IIMMgr)->GetCompleteCandidates((p),(b),(n),(f),(pn),(pm))
#define IIMMGR_IsCompleteCandidate(p,b)                  AEEGETPVTBL((p),IIMMgr)->IsCompleteCandidate((p),(b))
#define IIMMGR_GetData(p,b,s)                            AEEGETPVTBL((p),IIMMgr)->GetData((p),(b),(s))
#define IIMMGR_SetData(p,b)                              AEEGETPVTBL((p),IIMMgr)->SetData((p),(b))
#define IIMMGR_GetAssociatedCandidates(p,b,n,f,pn,pm)    AEEGETPVTBL((p),IIMMgr)->GetAssociatedCandidates((p),(b),(n),(f),(pn),(pm))
#define IIMMGR_GetCompositionCount(p,n)                  AEEGETPVTBL((p),IIMMgr)->GetCompositionCount((p),(n))

/*============================================================================
   DATA STRUCTURE DOCUMENTATION
==============================================================================
AEE IIMMgr Options

Description:
These are the options defined for IIMMgr Interface .

Definition:
 
IMGRO_MAXCANDIDATE:  This is the maximum count of candidates to return from both 
                     Candidate query functions.
                     pBuff - uint16 * variable
                     Default value is 64 Candidates

Members:
None

Comments:
None

See Also:
IIMMGR_GetOpt()
IIMMGR_SetOpt()

=======================================================================
   INTERFACE DOCUMENTATION
=======================================================================

Interface Name: IIMMGR

Description: 

Functions in this the Input Method Manager interface include:

   IIMMGR_AddRef()~
   IIMMGR_Release()~
   IIMMGR_QueryInterface()~
   IIMMGR_HandleEvent()~
   IIMMGR_Reset()~
   IIMMGR_Initialise()~
   IIMMGR_SetInputMode()~
   IIMMGR_SetOpt()~
   IIMMGR_GetOpt()~
   IIMMGR_GetCompositionCandidates()~
   IIMMGR_GetCompleteCandidates()~
   IIMMGR_GetData()~
   IIMMGR_SetData()
======================================================================= 

IIMMGR_HandleEvent() 

Description: 
    This is the Event handler for the Input Method Manager. The events it
handles will depend on the type of input mode it is in and the underlying 
engine's needs. Typically the EVT_KEY will be handled for all Number faced
keys to enable the engine to offer candidates based on keypad entry. If an 
event is not handled by the interface this function will return FALSE, otherwise
it will return TRUE.

Prototype:
    boolean IIMMGR_HandleEvent(IIMMgr * po, AEEEvent evt, uint16 wp, uint32 dwp)

Parameters:
    po : Pointer to the IIMMgr Interface object
    evt : AEEEvent code of the event to be processed
    wp : 16 bit event specific data, see AEE.h for specifics
    dwp : 32 bit event specific data, see AEE.h for specifics

Return Value:
    TRUE : If event is handled
    FALSE : If event is not handled

Comments: 
    This interface may handle different sets of events depending on the state it is
    in.

Side Effects: 
    This function will not handle any events until it is initialised properly with
    IIMMGR_Initialise().

See Also:
    IIMMGR_GetCompositionCount().

======================================================================= 

IIMMGR_Reset() 

Description: 
    This function will reset the Input Method Manager to an uninitialised default state.
You can use this function to make this interface resemble a newly created instance
of the Input Method Manager.

Prototype:
    void IIMMGR_Reset(IIMMgr * po)

Parameters:
    po : Pointer to the IIMMgr Interface object

Return Value:
    None.

Comments: 
    When starting a new composition you must Reset the IMM and intialise it. Doing this
    enables it to know that it is handling new events for composition rather than appending
    to the current composition state data.

Side Effects: 
    This will empty any internal buffers and bring the Interface to an initial state.
For this interface to handle events after calling reset the IIMMGR_Initialise() must be called.

See Also:
    IIMMGR_Initialise()
    IIMMGR_HandleEvent()
    
======================================================================= 

IIMMGR_Initialise() 

Description: 
    This function will initialise the underlying Input Method Managing engine
if required and do any extra initialisation required to set up context. This function
must be called after creation or resetting of the interface. 

Prototype:
    int IIMMGR_Initialise(IIMMgr * po)

Parameters:
    po : Pointer to the IIMMgr Interface object

Return Value:
    AEE_SUCCESS : If Successful
    OEM Specific error code: If Unsuccessful

Comments: 
    Calling this function after it is already initialised must be successful.

Side Effects: 
    None.

See Also:
    IIMMGR_Reset()
    IIMMGR_HandleEvent()

=======================================================================

IIMMGR_EnumModeInit() 

Description: 
    This function will initialise/reset the enumerator for supported text modes. 

Prototype:
    int IIMMGR_EnumModeInit(IIMMgr * po)

Parameters:
    po : Pointer to the IIMMgr Interface object

Return Value:
    AEE_SUCCESS : If Successful
    OEM Specific error code: If Unsuccessful

Comments: 
    none.

Side Effects: 
    none.

See Also:
    IIMMGR_EnumNextMode().

=======================================================================

IIMMGR_EnumNextMode() 

Description: 
    This function will enumerate the next supported mode. 

Prototype:
    int IIMMGR_EnumNextMode(IIMMgr * po, AEETextInputMode * pmode)

Parameters:
    po : Pointer to the IIMMgr Interface object
    pmode : [OUT] Next AEETextInputMode supported by this object

Return Value:
    AEE_SUCCESS : If Successful
    EFAILED : No more modes
    OEM Specific error code: If Unsuccessful

Comments: 
    IIMMGR_EnumModeInit() must be called first. If the enumeration is completed
    IIMMGR_EnumModeInit() must be called to reset the enumerator.

Side Effects: 
    none.

See Also:
    IIMMGR_EnumModeInit().

=======================================================================

IIMMGR_SetInputMode() 

Description: 
    This function will set the input mode state. This will be used to decide
what kind of language and characters the IIMMGR_GetCompositionCandidates() and 
IIMMGR_GetCompleteCandidates() are returned to the caller. 

Prototype:
    int IIMMGR_SetInputMode(IIMMgr * po, AEETextInputMode mode)

Parameters:
    po : Pointer to the IIMMgr Interface object
    mode : AEETextInputMode to manage

Return Value:
    AEE_SUCCESS : If Successful
    OEM Specific error code: If Unsuccessful

Comments: 
    This function must be called after initialising or resetting the 
interface to have meaningful interactions with the underlying engine.

Side Effects: 
    If the language is set to another mode the composition data will be
    removed. The information of IIMMGR_GetData() will remain. It is usually
    best to call this before any other functionality is used. 
    It will choose the best IMGRO_KEYOPTIONS for the mode it is set in.

See Also:
    IIMMGR_GetData()
    IIMMGR_GetInputMode()

=======================================================================

IIMMGR_GetInputMode() 

Description: 
    This function will get the current input mode state. 

Prototype:
    int IIMMGR_GetInputMode(IIMMgr * po, AEETextInputMode * pmode)

Parameters:
    po : Pointer to the IIMMgr Interface object
    pmode : [OUT] Current AEETextInputMode assigned to the object

Return Value:
    AEE_SUCCESS : If Successful
    EBADPARM : Bad parameter given
    EBADSTATE : No input mode assigned
    OEM Specific error code: If Unsuccessful

Comments: 
    None.

Side Effects: 
    None.

See Also:
    IIMMGR_GetInputMode()

=======================================================================

IIMMGR_SetOpt() 

Description: 
    This function will set various options for the engine's functionality. 

Prototype:
    int IIMMGR_SetOpt(IIMMgr * po, uint16 wOption, void * pData, uint32 nSize)

Parameters:
    po : Pointer to the IIMMgr Interface object
    wOption : Option to set
    pData : Option specific data
    nSize : size of pData

Return Value:
    AEE_SUCCESS : If Successful
    OEM Specific error code: If Unsuccessful

Comments: 
    If a bitflag is specified and not all operations can be supported
    do not alter the present option setting and ensure it returns a failure code.

Side Effects: 
    None.

See Also:
    None.

=======================================================================

IIMMGR_GetCompositionCandidates() 

Description: 
    This function will return the candidates based on what the user has keyed
in. The state of this function will depend largely on what events were handled
in IIMMGR_HandleEvent(). 
For Chinese text and other complex languagees it is used to display candidates that the user
can then use to further narrow down the character or word they were trying to key in.
For Rapid text modes, it will give the candidates based on the keyed items so far. There
is no need to use a further function to get a valid word.

Prototype:
    int IIMMGR_GetCompositionCandidates(IIMMgr * po, AECHAR * pBuff, int nBuffSize, int nFirst, int * pnNumCandidates, int * pnMaxChars)

Parameters:
    po : Pointer to the IIMMgr Interface object
    pBuff : [IN/OUT] IN:  If NULL on input and nBuffSize is 0 this will poll the engine for 
                          values to be returned in other IN/OUT parameters according 
                          to the other parameters values. See other parameters for their affects.
                          Otherwise this should be a valid buffer.
                     OUT: If this item is filled on input the composition candidates are returned
                          in a NULL separated buffer. Note, each candidate may occupy more than one
                          AECHAR use the value returned in pnNumCandidates for iteration.
    nBuffSize : [IN] IN:  If 0 and pBuff is NULL this will poll the engine according to the other
                          parameters. See other parameters for their effects. 
                          Otherwise this should be the size of the buffer (in bytes) supplied 
                          as pBuff.
    nFirst :    [IN] IN:  This is used to return the candidates starting with this number.
                          Common usage would be to manage pages of Candidates by supplying 0
                          for the first page, then when the user wants to see the next page
                          you would increment this by the number of candidates you have displayed.
                          If given, then any candidates preceeding this are ignored in quantity
                          calculations.
    pnNumCandidates : [IN/OUT] IN:  Maximum number of candidates to return. When provided the 
                                    quantity calculations are based on the lesser of this figure
                                    or actual candidates remaining.
                               OUT: Quantity of Candidates matching, or returned in pBuff if supplied.
                               Must not be NULL.
    pnMaxChars : [IN/OUT] IN:  If this parameter is NOT NULL, a calculation will be made for the 
                               maximum number of AECHAR sized buffer that would needed to be supplied
                               to successfully fill the complete request based on the other parameters.
                               A typical usage would be a size calculation with this parameter filled, 
                               then an allocation of a buffer of this size, and finally another call
                               with this parameter as NULL.
                          OUT: If this parameter is supplied the above calculation is performed and 
                               it contains the number of AECHAR required to completely satisfy the request
                               based on the other parameters.

Return Value:
    AEE_SUCCESS : If Successful
    EBADSTATE   : Not ready to return candidates
    OEM Specific error code: If Unsuccessful

Comments: 
    None.

Side Effects: 
    None.

See Also:
    IIMMGR_GetCompleteCandidates().

=======================================================================

IIMMGR_GetCompleteCandidates() 

Description: 
    This function will return the complete candidates based on the contents of the buffer fed to it.
    This allows some candidates to be returned based on the incomplete (or complete) compositional
    elements usually typed by the user in conjunction with IIMMGR_GetCompositionCandidates().

Prototype:
    int IIMMGR_GetCompleteCandidates(IIMMgr * po, AECHAR * pBuff, int nBuffSize, int nFirst, int * pnNumCandidates, int * pnMaxChars)

Parameters:
    po : Pointer to the IIMMgr Interface object
    pBuff : [IN/OUT] IN:  This should be a valid buffer containing the component that
                          will be used to determine completed candidates.
                     OUT: The completed candidates are returned in a NULL separated buffer. 
                          Note, each candidate may occupy more than one AECHAR, so use the 
                          value returned in pnNumCandidates for iteration.
    nBuffSize : [IN] IN:  If 0 this will poll the engine according to the other parameters. 
                          See other parameters for their effects. 
                          Otherwise this should be the size of the buffer (in bytes) supplied 
                          as pBuff. Note, that this is not the length of the string supplied, but
                          the complete buffer. It is best to use a larger buffer than the minimum
                          requirement to meet the input string length.
    nFirst :    [IN] IN:  This is used to return the candidates starting with this number.
                          Common usage would be to manage pages of Candidates by supplying 0
                          for the first page, then when the user wants to see the next page
                          you would increment this by the number of candidates you have displayed.
                          If given, then any candidates preceeding this are ignored in quantity
                          calculations.
    pnNumCandidates : [IN/OUT] IN:  Maximum number of candidates to return. When provided the 
                                    quantity calculations are based on the lesser of this figure
                                    or actual candidates remaining.
                               OUT: Quantity of Candidates matching, or returned in pBuff if supplied.
                               Must not be NULL.
    pnMaxChars : [IN/OUT] IN:  If this parameter is NOT NULL, a calculation will be made for the 
                               maximum number of AECHAR sized buffer that would needed to be supplied
                               to successfully fill the complete request based on the other parameters.
                               A typical usage would be a size calculation with this parameter filled, 
                               then an allocation of a buffer of this size, and finally another call
                               with this parameter as NULL.
                          OUT: If this parameter is supplied the above calculation is performed and 
                               it contains the number of AECHAR required to completely satisfy the request
                               based on the other parameters.

Return Value:
    AEE_SUCCESS : If Successful
    EBADSTATE   : Not ready to return candidates
    OEM Specific error code: If Unsuccessful

Comments: 
    None.

Side Effects: 
    None.

See Also:
    IIMMGR_Reset()
    IIMMGR_HandleEvent()
    IIMMGR_GetCompositionCandidates()

=======================================================================

IIMMGR_IsCompleteCandidate() 

Description: 
    This function will initialise the underlying Input Method Managing engine
if required and do any extra initialisation required to set up context. This function
must be called after creation or resetting of the interface. 

Prototype:
    boolean IIMMGR_IsCompleteCandidate(IIMMgr * po, const AECHAR * pBuff)

Parameters:
    po : Pointer to the IIMMgr Interface object
    pBuff :  This should be a valid buffer containing the character to 
             be analysed.

Return Value:
    TRUE :  If it is a Complete Candidate
    FALSE : If it is a Component, Radical, or partial word or function failed

Comments: 
    None.

Side Effects: 
    None.

See Also:
    None.

=======================================================================

IIMMGR_GetData() 

Description: 
    This function will retrieve the text data that was previously set.

Prototype:
    int IIMMGR_GetData(IIMMgr * po, AECHAR * pBuff, uint16 * pwBuffSize)

Parameters:
    po : Pointer to the IIMMgr Interface object
    pBuff : [IN/OUT] IN:  If this buffer is supplied the contents will be
                          filled with the data that was previously set.
                          If this buffer is not supplied the size of buffer
                          to hold the contents will be returned in pwBuffSize.
                     OUT: The data will be returned if available and will be 
                          NULL terminated always. If the buffer is not large 
                          enough to fit all the text the text will be truncated.
   pwBuffSize: [IN/OUT] IN:  Input contains the size of the pBuff[0 if pBuff is NULL]
                        OUT: Will contain the size of buffer required to satisfy
                             a full copy. If an error occurs, this will be 0.

Return Value:
    AEE_SUCCESS : If Successful
    EBADPARM    : Bad Parameter
    EBADSTATE   : No data to provide
    OEM Specific error code: If Unsuccessful

Comments: 
    None.

Side Effects: 
    None.

See Also:
    IIMMGR_SetData()

=======================================================================

IIMMGR_SetData() 

Description: 
    This function will set or clear data from the Input Method Manager. 

Prototype:
    int IIMMGR_SetData(IIMMgr * po, AECHAR * pBuff)

Parameters:
    po : Pointer to the IIMMgr Interface object
    pBuff : If valid buffer is passed in the data will be
            stored.
            If NULL this will clear the data.
            Maximum length of the string should fit in uint16 [0xFFFF-1]

Return Value:
    AEE_SUCCESS : If Successful
    ENOMEMORY   : Not enough memory to satisfy the request
    EBADPARM    : If a bad parameter is passed in
    OEM Specific error code: If Unsuccessful

Comments: 
    None.

Side Effects: 
    None.

See Also:
    IIMMGR_GetData()

=======================================================================

IIMMGR_GetAssociatedCandidates() 

Description: 
    This function will associate candidates with the word supplied in the input buffer.

Prototype:
    int IIMMGR_GetCompleteCandidates(IIMMgr * po, AECHAR * pBuff, int nBuffSize, int nFirst, int * pnNumCandidates, int * pnMaxChars)

Parameters:
    po : Pointer to the IIMMgr Interface object
    pBuff : [IN/OUT] IN:  This should be a valid buffer containing the word that
                          will be used to associate to generate other candidates.
                     OUT: The completed candidates are returned in a NULL separated buffer. 
                          Note, each candidate may occupy more than one AECHAR, so use the 
                          value returned in pnNumCandidates for iteration.
    nBuffSize : [IN] IN:  If 0 this will poll the engine according to the other parameters. 
                          See other parameters for their effects. 
                          Otherwise this should be the size of the buffer (in bytes) supplied 
                          as pBuff. Note, that this is not the length of the string supplied, but
                          the complete buffer. It is best to use a larger buffer than the minimum
                          requirement to meet the input string length.
    nFirst :    [IN] IN:  This is used to return the candidates starting with this number.
                          Common usage would be to manage pages of Candidates by supplying 0
                          for the first page, then when the user wants to see the next page
                          you would increment this by the number of candidates you have displayed.
                          If given, then any candidates preceeding this are ignored in quantity
                          calculations.
    pnNumCandidates : [IN/OUT] IN:  Maximum number of candidates to return. When provided the 
                                    quantity calculations are based on the lesser of this figure
                                    or actual candidates remaining.
                               OUT: Quantity of Candidates matching, or returned in pBuff if supplied.
                               Must not be NULL.
    pnMaxChars : [IN/OUT] IN:  If this parameter is NOT NULL, a calculation will be made for the 
                               maximum number of AECHAR sized buffer that would needed to be supplied
                               to successfully fill the complete request based on the other parameters.
                               A typical usage would be a size calculation with this parameter filled, 
                               then an allocation of a buffer of this size, and finally another call
                               with this parameter as NULL.
                          OUT: If this parameter is supplied the above calculation is performed and 
                               it contains the number of AECHAR required to completely satisfy the request
                               based on the other parameters.

Return Value:
    AEE_SUCCESS : If Successful
    EBADSTATE   : Not ready to return candidates
    OEM Specific error code: If Unsuccessful

Comments: 
    None.

Side Effects: 
    None.

See Also:
    IIMMGR_Reset()
    IIMMGR_HandleEvent()

=======================================================================

IIMMGR_GetCompositionCount() 

Description: 
    This function will obtain the count of items in the compositional
    buffer. This is useful for obtaining the quantity of characters the user
    has typed and are tracked by this object. An example is for calculating a
    selection region for extra auto-completed characters.

Prototype:
    int IIMMGR_GetCompositionCount(IIMMgr * po, int * pnElements)

Parameters:
    po : Pointer to the IIMMgr Interface object
    pnElements : [IN/OUT] IN:  This should be a valid buffer
                          OUT: The number of stored compositional elements.

Return Value:
    AEE_SUCCESS : If Successful
    EBADPARM    : Bad parameters given
    EFAILED     : General Failure
    OEM Specific error code: If Unsuccessful

Comments: 
    Useful to see if the user is composing, you can hide your UI as you see fit
    if the compositional elements is 0.

Side Effects: 
    None.

See Also:
    None.

=======================================================================*/

#endif // } !_OEM_IMM_HEADER_H_