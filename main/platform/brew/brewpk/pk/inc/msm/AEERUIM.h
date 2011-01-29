#if !defined(AEERUIM_H)
#define AEERUIM_H // #if !defined(AEERUIM_H)
/*============================================================================

FILE:  AEERUIM.h

SERVICES:  
        Removable User Identity Module APIs

GENERAL DESCRIPTION:

REVISION HISTORY: 

============================================================================

               Copyright © 1999-2002 QUALCOMM Incorporated 
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

============================================================================*/

#include "AEE.h"
#include "AEEComdef.h"

typedef struct {
  boolean   chv1_enabled;       // is CHV1 required?
  byte      chv1CountRemain;    // Remaining CHV1 count
  byte      chv1UnblockRemain;  // Remaining Unblock CHV1 count
  byte      chv2CountRemain;    // Remaining CHV2 count
  byte      chv2UnblockRemain;  // Remaining Unblock CHV2 count
} AEECHVStatus;

typedef enum {
   IRUIM_CHV1 = 1,  // Card Holder Verification 1.
   IRUIM_CHV2 = 2  // Card Holder Verification 2.
} AEECHVType;

#ifdef CUST_EDITION	
typedef enum {
  Active_Callforward_Busy = 11,//¼¤»îÃ¦×ª½Ó
  De_Active_Callforward_Busy=13,//È¡ÏûÃ¦×ª½Ó
  Active_Callforward_Default=21,//¼¤»îÎ´½ÓÍ¨×ª½Ó
  De_Active_Callforward_Default=23,//È¡ÏûÎ´½ÓÍ¨×ª½Ó
  Active_Callforward_No_Answer=31,//¼¤»îÎÞÓ¦´ð×ª½Ó
  De_Active_Callforward_No_Answer=33,//È¡ÏûÎÞÓ¦´ð×ª½Ó
  Active_Callforward_Unconditional=41,//¼¤»îÎÞÌõ¼þ×ª½Ó
  De_Active_Callforward_Unconditional=43,//È¡ÏûÎÞÌõ¼þ×ª½Ó
  Active_CallWaiting=45,//¼¤»îºô½ÐµÈ´ý
  De_Active_CallWaiting=47,//È¡Ïûºô½ÐµÈ´ý
#if defined(FEATURE_CARRIER_INDONESIA)
   //Register Section
   REGISTER_CFB                  = 5,
   DE_REGISTER_CFB            = 9,
   
   REGISTER_CFD                  = 15,
   REGISTER_CFD_TO_VM     = 17,
   DE_REGISTER_CFD            = 19,

   REGISTER_CFNA                = 25,
   REGISTER_CFNA_TO_VM   = 27,
   DE_REGISTER_CFNA          = 29,

   REGISTER_CFU                  = 35,
   REGISTER_CFU_TO_VM     = 37,
   DE_REGISTER_CFU            = 39,
#endif  
} OEMRUIM_PreferCallState;
#endif /*CUST_EDITION*/

typedef struct IRUIM IRUIM;

AEEINTERFACE(IRUIM) {
   INHERIT_IQueryInterface(IRUIM);
   boolean (*IsCardConnected)(IRUIM *pIRUIM);
   int (*GetCHVStatus)(IRUIM *pIRUIM, AEECHVStatus *pCHVStatus);
   boolean (*PINCheck)(IRUIM *pIRUIM, AEECHVType chv, const char *pPin,
                                                           boolean bVirtualPIN);
   int (*PINChange)(IRUIM *pIRUIM, AEECHVType chv, const char *pPin);
   boolean (*CHVEnable)(IRUIM *pIRUIM, const char *pPin);
   int (*CHVDisable)(IRUIM *pIRUIM);
   int (*UnblockCHV)(IRUIM *pIRUIM, AEECHVType chv, const char *pUnblockPin,
                                                              const char *pPin);
   int (*GetPrefLang)(IRUIM *pIRUIM, int *pLang, int *pEncoding);
   int (*GetId)(IRUIM *pIRUIM, char *pId, int *pnLen);
#ifdef CUST_EDITION	   
   int (*Get_Feature_Code)(IRUIM *pIRUIM,byte *Buf,int  Lable);
   int (*Read_Svc_P_Name)(IRUIM *pIRUIM,AECHAR *Buf);
#endif /*CUST_EDITION*/   

#ifdef FEATURE_OEMOMH 
   int (*Get_Ecc_Code)(IRUIM *pIRUIM,byte *Buf);
#endif
};

#define IRUIM_AddRef(p)                \
                           AEEGETPVTBL((p),IRUIM)->AddRef((p))     
#define IRUIM_Release(p)               \
                           AEEGETPVTBL((p),IRUIM)->Release((p))    
#define IRUIM_QueryInterface(p,id,pp)  \
                           AEEGETPVTBL((p),IRUIM)->QueryInterface((p),(id),(pp))
#define IRUIM_IsCardConnected(p)       \
                           AEEGETPVTBL((p),IRUIM)->IsCardConnected((p))
#define IRUIM_GetCHVStatus(p,ps)       \
                           AEEGETPVTBL((p),IRUIM)->GetCHVStatus((p),(ps))
#define IRUIM_VirtualPINCheck(p,c,ps)  \
                           AEEGETPVTBL((p),IRUIM)->PINCheck((p),(c),(ps),TRUE)
#define IRUIM_PINCheck(p,c,ps)         \
                           AEEGETPVTBL((p),IRUIM)->PINCheck((p),(c),(ps),FALSE)
#define IRUIM_PINChange(p,c,ps)        \
                           AEEGETPVTBL((p),IRUIM)->PINChange((p),(c),(ps))
#define IRUIM_CHVEnable(p,ps)          \
                           AEEGETPVTBL((p),IRUIM)->CHVEnable((p),(ps))
#define IRUIM_CHVDisable(p)            \
                           AEEGETPVTBL((p),IRUIM)->CHVDisable((p))
#define IRUIM_UnblockCHV(p,c,pu,pp)    \
                           AEEGETPVTBL((p),IRUIM)->UnblockCHV((p),(c),(pu),(pp))
#define IRUIM_GetPrefLang(p,pil,pit)   \
                           AEEGETPVTBL((p),IRUIM)->GetPrefLang((p),(pil),(pit))
#define IRUIM_GetId(p,pi,pn)           \
                           AEEGETPVTBL((p),IRUIM)->GetId((p),(pi),(pn)) 
#ifdef CUST_EDITION							   
#define IRUIM_Get_Feature_Code(p,pi,pn)           \
                           AEEGETPVTBL((p),IRUIM)->Get_Feature_Code((p),(pi),(pn))

#define IRUIM_Read_Svc_P_Name(p,pi)           \
                           AEEGETPVTBL((p),IRUIM)->Read_Svc_P_Name((p),(pi))
#endif /*CUST_EDITION*/		

#ifdef FEATURE_OEMOMH 
#define IRUIM_Get_Ecc_Code(p,pi)           \
                           AEEGETPVTBL((p),IRUIM)->Get_Ecc_Code((p),(pi))
#endif
/* Preferred language encoding */
#define  AEERUIM_LANG_ENCODING_OCTET            0  // Octet, unspecified:
                                                   // 8 bits/char
#define  AEERUIM_LANG_ENCODING_EXTPROTMSG    0x01  // Extended Protocal Message 
#define  AEERUIM_LANG_ENCODING_7BIT          0x02  // 7-bit ASCII: 8 bits/char  
#define  AEERUIM_LANG_ENCODING_IA5           0x03  // IA5: 7 bits/char 
#define  AEERUIM_LANG_ENCODING_UNICODE       0x04  // Unicode: 16 bits/char
#define  AEERUIM_LANG_ENCODING_SHIFT_JIS     0x05  // Shift-JIS: 8 or 16 bits/char
#define  AEERUIM_LANG_ENCODING_KOREAN        0x06  // Korean: 8 or 18 bits/char
#define  AEERUIM_LANG_ENCODING_LATINHEBREW   0x07  // Latin/Hebrew: 8 bits/char
#define  AEERUIM_LANG_ENCODING_LATIN         0x08  // Latin: 8 bits/char

#ifdef CUST_EDITION	
#define UIM_CDMA_HOME_SERVICE_SIZE 35
#endif /*CUST_EDITION*/
/*=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================
Interface Name: IRUIM

Description:
   This interface provides the BREW interface to the R-UIM device.
It provides the following services:

- Retrieving status of the Card and relevent parameters including card ID.
- Locking/Unlocking the card.
- Allowing access to changing the PIN codes.

The IRUIM interface is obtained via the ISHELL_CreateInstance mechanism.

======================================================================= 
Function:  IRUIM_AddRef()

Description: 
   This function increments the reference count of the IRUIM Interface object,
   allowing the object to be shared by multiple callers. The object is freed
   when the reference count reaches 0 (zero).

Prototype:
   uint32 IRUIM_AddRef(IRUIM *pIRUIM)

Parameters:
   pIRUIM:  [in].  Pointer to the IRUIM Interface object.

Return Value:
   Incremented reference count for the object.

Comments:  
   A valid object returns a positive reference count.  Otherwise, 0 (zero) is
   returned.

Side Effects: 
   None

See Also:
   IRUIM_Release()
   
======================================================================= 
Function:  IRUIM_Release()

Description: 
   This function decrements the reference count of the IRUIM Interface object.
   The object is freed from memory and is no longer valid when the reference
   count reaches 0 (zero).

Prototype:
   uint32 IRUIM_Release(IRUIM *pIRUIM)

Parameters:
   pIRUIM:  [in].  Pointer to the IRUIM Interface object.

Return Value: 
   Decremented reference count for the object.
   0 (zero), If the object has been freed and is no longer valid.

Comments: 
   None

Side Effects: 
   None

See Also: 
   IRUIM_AddRef()
   
======================================================================= 
Function:  IRUIM_QueryInterface()

Description: 
   This function retrieves a pointer to an interface conforming to the specified
   class ID.  This can be used to query for extended functionality, like future
   versions or proprietary extensions.

   Upon a successful query, the interface is returned after the AddRef method
   is called.  The caller is responsible for call the Release method at some
   point in the future.

Prototype:
   int IRUIM_QueryInterface (IRUIM *pIRUIM,
                             AEECLSID idReq,
                             void **ppo)

Parameters:
   pIRUIM:  [in].  Pointer to the IRUIM Interface object.
   idReq:  [in].  Requested class ID exposed by the object
   ppo:  [out].  Returned object.  Filled by this method

Return Value:
   AEE_SUCCESS - Interface found
   ENOMEMORY - Insufficient memory
   ECLASSNOTSUPPORT - Requested interface is unsupported
   EFAILED - Any general failure.

Comments:  
   The pointer in *ppo is set to the new interface (with refcount positive), or
   NULL if the ClassID is not supported by the object.
   This function can be called with idReq = AEECLSID_ADDRBOOK to return an
   address book object.  However, CHV1 must have been previously disabled or
   verified before this function is called.
   ppo MUST not be NULL.

Side Effects: 
   None

See Also:
   IRUIM_CHVDisable
   IRUIM_PINCheck
   IAddrBook Interface
   
======================================================================= 
Function:  IRUIM_IsCardConnected
 
Description:
   This function checks to see if the R-UIM card is present.

Prototype:
   boolean IRUIM_IsCardConnected(IRUIM *pIRUIM)

Parameters:
   pIRUIM:  [in].  Pointer to the IRUIM Interface object.
 
Return Value:
   TRUE if a R-UIM card was found, otherwise FALSE.

Comments:
   If pIRUIM is not valid, FALSE is returned.  If the R-UIM card is present
   FALSE will still be returned if the card is not initialized, is powered down,
   or is faulty.

Side Effects:
   None.

See Also:
   None
   
=============================================================================
Function:  IRUIM_GetCHVStatus()
 
Description:
   Returns the current R-UIM status
 
Prototype:
   int IRUIM_GetCHVStatus(IRUIM *pIRUIM, AEECHVStatus *pCHVStatus)

Parameters:
   pIRUIM:  [in].  Pointer to the IRUIM Interface object.
   pCHVStatus:  [out].  Pointer to the returned CHV status.
 
Return Value:
   AEE_SUCCESS if operation succeeded, EFAILED otherwise.
   
Comments:
   None.
 
Side Effects:
   None.
 
See Also:
   None
   
=============================================================================
Function:  IRUIM_VirtualPINCheck()
 
Description:
   If IRUIM_PINCheck() has been previously called, this function can be used to
   re-verify the CHV1 PIN without accessing the R-UIM.  It compares the input
   PIN with the previous PIN value.
 
Prototype:
   boolean IRUIM_VirtualPINCheck(IRUIM *pIRUIM, AEECHVType chv,
                                                               const char *pPin)

Parameters:
   pIRUIM:  [in].  Pointer to the IRUIM Interface object.
   chv:  [in].  PIN to be checked.
   pPin:  [in].  Pointer to the eight digit character string.
 
Return Value:
   TRUE if the user input matches the previously read value of CHV1 on the
   R-UIM, otherwise FALSE.
   FALSE will be returned if pIRUIM or pPin are NULL.
   FALSE will also be returned if the R-UIM is not connected as defined above in
   the description of IRUIM_IsCardConnected().

Comments:
   This function should only be called after IRUIM_PINCheck() has been called
   successfully.

Side Effects:
   None.

See Also:
   IRUIM_PINCheck
   
=============================================================================
Function:  IRUIM_PINCheck()
 
Description:
   This function will compare the designated CHV on the R-UIM with the PIN
   passed in.
 
Prototype:
   boolean IRUIM_PINCheck(IRUIM *pIRUIM, AEECHVType chv, const char *pPin)

Parameters:
   pIRUIM:  [in].  Pointer to the IRUIM Interface object.
   chv:  [in].  PIN to be checked.
   pPin:  [in].  Pointer to an eight digit character string.
 
Return Value:
   TRUE if the user input matches the CHV on the R-UIM, otherwise FALSE.
   FALSE will be returned if pIRUIM or pPin are NULL.
   FALSE will also be returned if the RIUM is not connected as defined above in
   the description of IRUIM_IsCardConnected().

Comments:
   The appropriate CHV must be enabled and not blocked when this function is
   called.  This function performs the VERIFY CHV functionality as described in
   3GPP TS 11.11.  If the operation returns TRUE, the PIN is stored and will be
   used for the next call to IRUIM_VirtualPINCheck().

Side Effects:
   None

See Also:
   IRUIM_VirtualPINCheck
   
=============================================================================
Function:  IRUIM_UnblockCHV()
 
Description:
   This function will unblock a CHV which has been previously blocked using
   the passed in unblock CHV and PIN.  An application must have PL_SYSTEM
   privilege complete this function.
 
Prototype:
   int IRUIM_UnblockCHV(IRUIM *pIRUIM,
                        AEECHVType chv,
                        char *pUnblockPin,
                        char *pPin)

Parameters:
   pIRUIM:  [in].  Pointer to the IRUIM Interface object.
   chv:  [in].  PIN to be unblocked.
   pUnblockPin:  [in].  Pointer to an eight digit character unblock string.
   pPin:  [in].  Pointer to an eight digit character string.
 
Return Value:
   AEE_SUCCESS if the user input was valid.
   EPRIVLEVEL if the calling app. does not have PL_SYSTEM privilege.
   EFAILED for other errors.

Comments:
   CHV1 will be restored to a the pPin value.  This function performs the
   UNBLOCK CHV functionality as described in 3GPP TS 11.11.  If the operation
   returns AEE_SUCCESS, the PIN is stored and will be used for the next call to
   IRUIM_VirtualPINCheck().
   
Side Effects:
   None.

See Also:
   None
   
=============================================================================
Function:  IRUIM_PINChange()
 
Description:
   This function will change the designated CHV on the R-UIM to the PIN
   passed in.  IRUIM_PINCheck() or another command that sets the virtual pin
   must be called before this command.
 
Prototype:
   int IRUIM_PINChange(IRUIM *pIRUIM, AEECHVType chv, const char *pPin)

Parameters:
   pIRUIM:  [in].  Pointer to the IRUIM Interface object.
   chv:  [in].  PIN to be checked.
   pPin:  [in].  Pointer to an eight digit character string to be used as the
                 new PIN.
 
Return Value:
   AEE_SUCCESS if the PIN was changed, EFAILED otherwise.

Comments:
   The appropriate CHV must be enabled and not blocked when this function is
   called.  This function performs the CHANGE CHV functionality as described in
   3GPP TS 11.11.  If the operation returns AEE_SUCCESS, the new PIN will be
   stored and used for the next call to IRUIM_VirtualPINCheck().

Side Effects:
   None.

See Also:
   IRUIM_PINCheck
   IRUIM_VirtualPINCheck
   
=============================================================================
Function:  IRUIM_CHVDisable()
 
Description:
   This function will disable CHV1 if the last call to IRUIM_PINCheck() was
   successful.  
 
Prototype:
   int IRUIM_CHVDisable(IRUIM *pIRUIM)

Parameters:
   pIRUIM:  [in].  Pointer to the IRUIM Interface object.
 
Return Value:
   AEE_SUCCESS if CHV1 was disabled, EFAILED otherwise.

Comments:
  The successful completion of this command allows unprotected access to all
  files protected by CHV1.  Ths function performs the DISABLE CHV functionality
  as described in 3GPP TS 11.11.

Side Effects:
  None.

See Also:
   None
   
=============================================================================
Function:  IRUIM_CHVEnable()
 
Description:
   This function will enable CHV1 if the passed in PIN is correct.
 
Prototype:
   boolean IRUIM_CHVEnable(IRUIM *pIRUIM, const char *pPin)

Parameters:
   pIRUIM:  [in].  Pointer to the IRUIM Interface object.
   pPin:  [in].  Pointer to an eight digit character string.  If NULL, use the
                PIN previously entered with IRUIM_PINCheck().
 
Return Value:
   TRUE, if CHV1 was enabled, FALSE otherwise.

Comments:
   If CHV1 has been disabled, you need to include the CHV1 as the argument
   to this function since the 'ENABLE CHV1' command requires it.  

   If CHV1 is enabled, the user will have already entered CHV1 so you 
   do not need to include it as an argument (pass in NULL).

   This function performs the ENABLE CHV functionality as described in
   3GPP TS 11.11.  If the operation returns TRUE, the PIN is stored and
   will be used for the next call to IRUIM_VirtualPINCheck().

Side Effects:
   None.

See Also:
   IRUIM_PINCheck
   IRUIM_VirtualPINCheck
   
=============================================================================
Function:  IRUIM_GetPrefLang()
 
Description:
  This function returns the highest priority preferred language on the R-UIM
  card. 

  Note that there may be a second, third,...,nth preferred language on the 
  R-UIM.  This function will only return the first preferred langauge.
 
Prototype:
   int IRUIM_GetPrefLang(IRUIM *pIRUIM, int *pLang, int *pEncoding)

Parameters:
   pIRUIM:  [in].  Pointer to the IRUIM Interface object.
   pLang:  [out].  Most preferred langauge returned from the R-UIM.
   pEncoding:  [out].  Encoding of the most preferred language.
 
Return Value:
   None

Comments:
   AEE_SUCCESS if no problems occurred, EFAILED otherwise.

Side Effects:
   None

See Also:
   None
   
=============================================================================
Function:  IRUIM_GetId()
 
Description:
  This function returns the identification number of the R-UIM.

Prototype:
   int IRUIM_GetId(IRUIM *pIRUIM, char *pId, int *pnLen)

Parameters:
   pIRUIM:  [in].  Pointer to the IRUIM Interface object.
   pId:  [in/out].  Card ID.  If set to NULL when called, then pnLen will
                    contain the required size of the ID when the function
                    returns.
   pnLen:  [in/out].  If pId is NULL when this function is called, pnLen will
                      return the number of bytes required to hold the entire Id.
                      Otherwise, pnLen should be set to the number of bytes
                      requested and will return the number of bytes actually
                      provided.
 
Return Value:
   AEE_SUCCESS if no problem occurred, EFAILED otherwise.

Comments:
   None.

Side Effects:
   None.

See Also:
   None
   
=============================================================================*/

#endif // #if !defined(AEERUIM_H)

