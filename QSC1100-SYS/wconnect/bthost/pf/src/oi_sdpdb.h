#ifndef _OI_SDPDB_H
#define _OI_SDPDB_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_sdpdb.h#1 $ 

$Log: $
*
*    #3        18 Oct  2006           PR
*  Merged OI Code v3.0.14rev1revQ
*
===========================================================================*/
/**
 * @file  
 *
 *  This file provides the API for management of the SDP database.
 *
 *  For more information see the @ref SDP_docpage section of the 
 *  BLUEmagic 3.0 SDK documentation. 
 */

/** \addtogroup SvcDisc Service Discovery APIs */
/**@{*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_status.h"
#include "oi_bt_spec.h"
#include "oi_dataelem.h"

/**
 * To support multiple languages, text attribute IDs are modified by a language
 * ID. The language ID is defined by an attribute in an SDP service record. The
 * SDP specification also defines a language attribute ID that can always be used to
 * select the default language. See section E.5.1.7 of the Bluetooth specification
 * for information on the language attribute ID.
 */
#define OI_SDP_DEFAULT_LANGUAGE_ID(id) (id | OI_ATTRID_DEFAULT_LanguageBaseOffset)


/**
 * Attribute IDs are 16-bit integers.
 */
typedef OI_UINT16 OI_SDP_ATTRIBUTE_ID;

/**
 * A service record attribute is represented as a structure consisting of 
 * a 16-bit attribute ID and an attribute value represented by a union of type 
 * @ref OI_DATAELEM. A variety of macros defined in oi_dataelem.h can be used to 
 * statically initialize the data element in an OI_SDPDB_ATTRIBUTE. 
 * (See oi_bt_assigned_nos.h for more information on attribute IDs.)
 */
typedef struct {
    OI_SDP_ATTRIBUTE_ID Id;  /**< a 16-bit attribute ID */
    OI_DATAELEM Element;     /**< the data element value for the attribute value */
} OI_SDPDB_ATTRIBUTE;


/**
 * the internationalized strings to use for an SDP service record
 *
 * @param  attrs  the (uuid, string) attribute pairs
 * @param  num    the number of attributes
 */
typedef struct {
    const OI_SDPDB_ATTRIBUTE *attrs;
    OI_UINT16 num;
} OI_SDP_STRINGS;


/**
 * A service record defines the attributes of a service. An SDP service record
 * is an array of attribute ID/value pairs.
 */
typedef struct {
    OI_SDPDB_ATTRIBUTE const *Attributes; /**< A service record is initialized with an attribute list.
                                               This will normally be statically allocated attributes 
                                               that do not change during the lifetime of a service. */
    
    OI_UINT16 NumAttributes;              /**< the number of attributes in the ConstAttributes array */
    
    OI_SDPDB_ATTRIBUTE const *Strings;    /**< name and description strings */
    OI_UINT16 NumStrings;                 /**< the number of strings */

} OI_SDPDB_SERVICE_RECORD;


/**
 * This function requests that a service record be added to the SDP database. 
 * Once added, the service will immediately become visible to SDP clients.
 *
 * @param serviceRecord       A service record defines the attributes of a service. 
 *                            An SDP service record is an array of attribute ID/value pairs.
 *
 * @param serviceRecordHandle This pointer points to the buffer where the service record handle will be written once
 *                            the service record is added. The service record handle is a locally unique handle
 *                            that can be used to reference a service record when accessing attributes within the
 *                            service record. This parameter can be NULL if the caller does not need to store the
 *                            service record handle.
 *
 * @return                    OI_OK if the service record is added; error status otherwise, including if the 
 *                            SDP database is full
 */
OI_STATUS OI_SDPDB_AddServiceRecord(OI_SDPDB_SERVICE_RECORD const *serviceRecord,
                                    OI_UINT32 *serviceRecordHandle);


/**
 * This function removes a service record from the SDP database. Once removed, the service will
 * no longer be visible to SDP clients.
 *
 * @param serviceRecordHandle      service record handle of the service to be removed
 *
 * @return                         OI_OK if the service record was removed; error status otherwise, including 
 *                                 if the service record handle is invalid
 */
OI_STATUS OI_SDPDB_RemoveServiceRecord(OI_UINT32 serviceRecordHandle);


/**
 * An application can use the Service Avaialability attribute to advertise in
 * the service record if a service is available or not and. This function Sets
 * the value of the Service Availability attribute for a given service. Other
 * than 0 (meaning not available) and 255 (meaning fully available) the
 * interpretation of intermediate values is profile/application specific.
 *
 * @param serviceRecordHandle   Identifies the service record to which the
 *                              attribute is to be added.
 *
 * @param availability          0 means not available, 255 means 100% available, 
 *                              128 means 50% available etc.
 *
 * @return                      OI_OK if the attributes was set; error status
 *                              if the service record handle is invalid.
 */
OI_STATUS OI_SDPDB_SetServiceAvailability(OI_UINT32 serviceRecordHandle,
                                          OI_UINT8 availability);

/**
 * Dynamically sets attributes on a service record. Memory allocated for the
 * attributes must not be freed by the caller until the attribute list is
 * removed from the SDP database by calling OI_SDPDB_RemoveAttributeList().
 *
 * @param serviceRecordHandle   identifies the service record to which the
 *                              attribute is to be added
 *
 * @param attributeList         pointer to the list of attributes to add
 *
 * @param numAttributes         number of attributes in list
 *
 * @return                      OI_OK if the attributes were added; error status
 *                              otherwise, including if the service record
 *                              handle is invalid
 */
OI_STATUS OI_SDPDB_SetAttributeList(OI_UINT32 serviceRecordHandle,
                                    OI_SDPDB_ATTRIBUTE const *attributeList,
                                    OI_UINT16 numAttributes);

/**
 * Removes attribute list previously set by OI_SDPDB_SetAttributeList().
 *
 * @param serviceRecordHandle   Identifies the service record that contains the
 *                              attribute list to be removed.
 *
 * @param attributeList         [OUT] returns a pointer to the removed attribute list.
 *                              This is the same pointer that was passed in to
 *                              OI_SDPBP_SetAttributeList(). The caller can now
 *                              free any memory allocated for this attribute
 *                              list. Can be NULL if the caller does not this
 *                              pointer.
 *
 * @param numAttributes         [OUT] returns the length of the removed
 *                              attribute list. Can be NULL if the caller does
 *                              not need this value.
 */
OI_STATUS OI_SDPDB_RemoveAttributeList(OI_UINT32 serviceRecordHandle,
                                       OI_SDPDB_ATTRIBUTE **attributeList,
                                       OI_UINT16 *numAttributes);

/**
 * Copy an attribute list.  This may be used in combination with e.g. 
 * OI_SDPDB_RemoveAttributeList to copy the returned list into a new list
 * containing additional attributes.  OI_SDPDB_FreeAttributeListElements
 * must be called when the destination list is no longer needed to release
 * the memory used.
 *
 * @param toList         The destination to copy the source attributes into.  This
 *                       must point to enough space to include numAttributes attributes.
 * @param fromList       The source attribute list.
 * @param numAttributes  The number of attributes to copy from the source list into
 *                       the destination.
 */
OI_STATUS OI_SDPDB_CloneAttributeList(OI_SDPDB_ATTRIBUTE *toList,
                                      OI_SDPDB_ATTRIBUTE *fromList,
                                      OI_UINT16 numAttributes);

/**
 * Free memory allocated for data elements in an attribute list. Memory for the
 * attribute list itself must be freed by the caller.
 */
void OI_SDPDB_FreeAttributeListElements(OI_SDPDB_ATTRIBUTE *attributeList,
                                        OI_UINT16 numAttributes);

/*****************************************************************************/
#endif /* _OI_SDPDB_H */

/**@}*/
