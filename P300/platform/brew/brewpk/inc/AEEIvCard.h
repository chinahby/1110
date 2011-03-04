#ifndef AEEIVCARD_H
#define AEEIVCARD_H
/*==============================================================================
FILE: 
   AEEIvCard.h

SERVICES: 
   vCard Parser related services.

GENERAL DESCRIPTION:
   The IvCard interface provides generic reader/writer for vCard datastream.
   This interface supports vCard 3.0 specification for parsing with backward 
   compatibility for 2.1. Generator support is only for version3.0
   You can find the specification here....
   http://www.imc.org/pdi/vcard-21.doc
   http://www.ietf.org/rfc/rfc2426.txt


PUBLIC CLASSES AND STATIC FUNCTIONS:
   IvCard

INITIALIZATION AND SEQUENCING REQUIREMENTS:

     COPYRIGHT © 2007 QUALCOMM Incorporated.
                   All Rights Reserved.
                   QUALCOMM Proprietary
==============================================================================*/
#include "AEEIvProperty.h"

#define AEEIID_IvCard         0x0105ccd6

/*==============================================================================
IvCard INTERFACE
==============================================================================*/

#define INHERIT_IvCard(iname)                                                 \
   INHERIT_IvObject(iname);                                                   \
   int (*AddProperty)(iname *po, IvProperty *pIvProperty);                    \
   int (*RemoveProperty)(iname *po, IvProperty *pIvProperty);                 \
   int (*GetNumberOfProperties)(iname         *po,                            \
                                AEEVProperty  nPropId,                        \
                                int           *pnNumProperties);              \
   int (*GetPropertyAtIndex)(iname           *po,                             \
                             AEEVProperty    nPropId,                         \
                             int             nIndex,                          \
                             IvProperty      **ppIvProperty)

AEEINTERFACE_DEFINE(IvCard);

static __inline uint32 IvCard_AddRef(IvCard *pIvCard)
{
   return AEEGETPVTBL(pIvCard, IvCard)->AddRef(pIvCard);
}

static __inline uint32 IvCard_Release(IvCard *pIvCard)
{
   return AEEGETPVTBL(pIvCard, IvCard)->Release(pIvCard);
}

static __inline int IvCard_QueryInterface(IvCard    *pIvCard, 
                                          AEEIID    iid,
                                          void      **ppo)
{
   return AEEGETPVTBL(pIvCard, IvCard)->QueryInterface(pIvCard, iid, ppo);
}

static __inline int IvCard_GetObjectType(IvCard     *pIvCard,
                                         AEECLSID  *pnObjType)
{
   return AEEGETPVTBL(pIvCard, IvCard)->GetObjectType(pIvCard,pnObjType);
}

static __inline int IvCard_AddProperty(IvCard *pIvCard,IvProperty *pIvProperty)
{
   return AEEGETPVTBL(pIvCard, IvCard)->AddProperty(pIvCard,pIvProperty);
}

static __inline int IvCard_RemoveProperty(IvCard *pIvCard,IvProperty *pIvProperty)
{
   return AEEGETPVTBL(pIvCard, IvCard)->RemoveProperty(pIvCard,pIvProperty);
}

static __inline int IvCard_GetNumberOfProperties(IvCard        *pIvCard,
                                                 AEEVProperty  nPropId,
                                                 int           *pnNumProps)
{
   return AEEGETPVTBL(pIvCard, IvCard)->GetNumberOfProperties(pIvCard,
                                                              nPropId,
                                                              pnNumProps);
                                                        
}

static __inline int IvCard_GetPropertyAtIndex(IvCard         *pIvCard,
                                              AEEVProperty   nPropId,
                                              int            nIndex,
                                              IvProperty     **ppIvProperty)
{
   return AEEGETPVTBL(pIvCard, IvCard)->GetPropertyAtIndex(pIvCard,
                                                           nPropId,
                                                           nIndex,
                                                           ppIvProperty);
}

static __inline IvObject *IvCard_To_IvObject(IvCard *pIvCard) 
{
   return (IvObject *) (void *)pIvCard;
}

/*=============================================================================
INTERFACE   DOCUMENTATION
===============================================================================
IvCard Interface

Description:
   Interface to parse the buffer containing vCard objects.
   IvCard - Interface to vCard objects

   The vCard definitions can be identified and parsed within the buffer. 
   The vCard is intended to be used for exchanging information about people 
   and resources. A vCard is a collection of one or more properties. 
   A property is a uniquely named value. A set of properties can be grouped 
   within a vCard. A set of parameters can be associated to a property.
   Parameter is again a name-value pair. VERSION property is mandatory & should
   be added to object while creating vCard. The parser will fail to parse any
   vCard object with out VERSION property

   If we have simple vCard of vCard spec author, (this vCard is from rfc 2426)
===pre>
   BEGIN:vCard
   VERSION:3.0
   FN:Frank Dawson
   ORG:Lotus Development Corporation
   EMAIL;TYPE=INTERNET,PREF:Frank_Dawson@Lotus.com
   EMAIL;TYPE=INTERNET:fdawson@earthlink.net
   URL:http://home.earthlink.net/~fdawson
   END:vCard

   void Sample_vCard_Usage1(IvCard *pIvCard)
   {
      int nResult = 0;
      IvProperty *pIvProperty = NULL;
      int nNumProps = 0;
      int nNumParms = 0;

      nResult = IvCard_GetNumberOfProperties(pIvCard,0,&nNumProps);
      //on return nNumProps=6

      nResult = IvCard_GetNumberOfProperties(pIvCard,AEEVPROPERTY_EMAIL,&nNumProps);
      //on return nNumProps=2;

      nResult = IvCard_GetPropertyAtIndex(pIvCard,AEEVPROPERTY_EMAIL,0,&pIvProperty);
      //pIvPropety would be pointg to the first EMAIL property in the above buffer

      nResult = IvProperty_GetNumberOfParms(pIvProperty,0,&nNumParms);
      //on return nNumParms=2

      RELEASEIF(pIvProperty);

   }
   
   
===/pre>

===============================================================================
IvCard_AddRef()

Description:
   This function is inherited from IQI_AddRef().

See Also:
   IvCard_Release()

===============================================================================
IvCard_Release()

Description:
   This function is inherited from IQI_Release().

See Also:
   IvCard_AddRef()

===============================================================================

IvCard_QueryInterface()

Description:
   This function is inherited from IQI_QueryInterface(). 

=============================================================================
IvCard_GetObjectType

Description:
   This function returns the AEECLSID of the object type

Prototype:
   int IvCard_GetObjectType(IvCard *pIvCard,AEECLSID *pnObjType);

Parameters:
   pIvCard      : Pointer to IvCard interface object
   pnObjType : Pointer to object type

Return Values:
   AEE_SUCCESS  : If we can get the ObjType
   AEE_EBADPARM : If pnObjType is NULL

Comments:
   None

Side Effects:
   None

=============================================================================
IvCard_AddProperty

Description: 
   Add a Property to the vCard.

Prototype:
   int IvCard_AddProperty(IvCard       *pIvCard,
                          IvProperty   *pIvProperty);

Parameters:
   pIvCard      : Pointer to the IvCard object
   pIvProperty  : The property to be added.

Return Value: 
   AEE_SUCCESS   : If the property was added successfully to the card
   AEE_EBADPARM  : If pIvProperty is NULL
   AEE_ENOMEMORY : If we couldn't allocate memory for adding the object
   AEE_EFAILED   : Otherwise

Comments:
   Upon success the object's reference count is incremented

Side Effects:
   None

=============================================================================
IvCard_RemoveProperty

DESCRIPTION:
   Remove's the specified property.

Prototype:
   int IvCard_RemoveProperty(IvCard *pIvCard, IvProperty *pIvProperty);

Parameters:
   pIvCard     : Pointer to the IvCard object.
   pIvProperty : Pointer IvProperty object that has to be removed.

Return Value:
   AEE_SUCCESS  :  If the function was able to delete the IvProperty object.
   AEE_EBADPARM :  If the pIvCard or pProperty is NULL
   AEE_EFAILED  :  Otherwise.

Comments:
   Upon success the object's reference count is decremented

Side Effects:
   None

=============================================================================
IvCard_GetNumberOfProperties

Description:
   Returns the number of recurring properties for a particular property id

   Passing 0 for nPropId will return the total number of properties

Prototype:
   int IvCard_GetNumberOfProperties(IvCard        *pIvCard,
                                    AEEVProperty  nPropId,
                                    int           *pnNumProps);

Parameters:
   pIvCard     :  Pointer to the IvCard interface object
   nPropId     :  Property Id, for which we want to check the number
                  of occurrences
   pnNumProps  :  Upon successful return contains the number of
                  properties for that particular property id

Return Value:
   AEE_SUCCESS   :  If we are able to successfully return the count
   AEE_EBADPARM  :  If either nPropId is invalid or pnNumProps is NULL

Comments:
   In card we may have some properties repeat more than once. This
   function is useful in knowing such number of properties.
  
Side Effects:
   None

=============================================================================
IvCard_GetPropertyAtIndex

Description: 
   Get the property based on Id.

Prototype:
   int IvCard_GetPropertyAtIndex(IvCard        *pIvCard, 
                                 AEEVProperty  nPropId,
                                 int           nIndex,
                                 IvProperty    **ppIvProperty);
Parameters:
   pIvCard        : Pointer to the IvCard object.
   nPropId        : The id of the property.
   nIndex         : The index at which the object has to be retrieved, the index
                    is zero based
   ppIvProperty   : pointer to pointer to IvProperty object.

Return Value: 
   AEE_SUCCESS   : If the function was able to get the IvProperty object.
   AEE_EBADPARM  : If the ppIvProperty is null/ Invalid Prop Id
   AEE_ENOMEMORY : If we are unable to allocate more memory
   AEE_EFAILED   : Otherwise

Comments:
   Passing nPropId=0, will retrieve the IvProperty at the given index


Side Effects:
   Upon AEE_SUCCESS, the reference count of the object is incremented

=============================================================================*/
#endif  //AEEIVCARD_H
