#ifndef _L2CAP_GROUP_H
#define _L2CAP_GROUP_H
/**
@file  

This file provides the interface for L2CAP groups (connectionless data channels). 
*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_l2cap.h"

/**
 * A callback function of this type indicates the success or failure of an 
 * @ref OI_L2CAP_GroupAddMember call.
 *
 * @param cid     the channel ID for the group
 * @param addr    the address of the device being added
 * @param result  success/failure code
 */
typedef void(*OI_L2CAP_GROUP_ADD_MEMBER_CFM)(OI_L2CAP_CID cid,
                                          OI_BD_ADDR *addr,
                                          OI_STATUS result);

/**
 * A callback function of this type indicates that connectionless data was received 
 * on the specified PSM.
 *
 * @param psm      the PSM on which the data was received
 * @param data     the data which that received
 * @param dataLen  the number of bytes received
 */
typedef void(*OI_L2CAP_CL_DATA_RECV)(OI_L2CAP_PSM psm,
                                     OI_BYTE *data,
                                     OI_UINT16 dataLen);

/**
 * A callback indicating that a device has been removed from a group because of a disconnection.
 *
 * @param addr  the address of the removed member
 */
typedef void (*OI_L2CAP_GROUP_REMOVE_MEMBER_CALLBACK)(OI_BD_ADDR *addr);

/***************************************************************

  connectionless reception

**************************************************************/

/**
 * Disable connectionless traffic (CLT) on a specified PSM. The PSM
 * must have been previously enabled using OI_L2CAP_EnableCLT().
 *
 * @param psm  the PSM on which to disable traffic
 */
OI_STATUS OI_L2CAP_DisableCLT(OI_L2CAP_PSM psm);

/**
 * Enable connectionless traffic (CLT) on a specified PSM. 
 * 
 * @param psm     the PSM on which to enable traffic
 * @param readCB  the callback on which to receive data
 */
OI_STATUS OI_L2CAP_EnableCLT(OI_L2CAP_PSM psm,
                             OI_L2CAP_CL_DATA_RECV readCB);

/**
 * Create a new group. A group is used to identify those devices to
 * which connectionless data will be sent.
 * 
 * @param psm  the PSM for outgoing traffic / incoming filter
 * @param cid  the channel ID of the group
 * @param removeCB  this callback is called when a member of the group has been removed because of a disconnection
 * 
 * @return     OI_OK if successful
 */
OI_STATUS OI_L2CAP_GroupCreate(OI_L2CAP_PSM psm, 
                               OI_L2CAP_CID *cid,
                               OI_L2CAP_GROUP_REMOVE_MEMBER_CALLBACK removeCB);

/**
 * Close the specified group.
 * 
 * @param cid  the channel ID of the group
 */
OI_STATUS OI_L2CAP_GroupClose(OI_L2CAP_CID cid);

/**
 * Test the specified CID to determine if it is a Group CID
 * 
 * @param cid  the channel ID to test
 */
OI_BOOL OI_L2CAP_IsGroupCID( OI_L2CAP_CID cid );

/**
 * Add a device to the specified group.
 *
 * @param callback  the callback function to call to confirm that a member has been 
                    added to a group
 * @param cid       the channel ID
 * @param addr      the device to add to the group
 */
OI_STATUS OI_L2CAP_GroupAddMember(OI_L2CAP_GROUP_ADD_MEMBER_CFM callback,
                               OI_L2CAP_CID cid,
                               OI_BD_ADDR *addr);

/**
 * Remove a device from the specified group.
 *
 * @param cid  the channel ID
 * @param addr the device to remove from the group
 */
OI_STATUS OI_L2CAP_GroupRemoveMember(OI_L2CAP_CID cid, OI_BD_ADDR *addr);


/**
 * Get a list of device addresses for the members of a group.
 *
 * @param cid   the channel ID of the channel to free
 * @param list  the list of devices
 * @param num   the number of devices returned
 */
OI_STATUS OI_L2CAP_GetGroupMembership(OI_L2CAP_CID cid,
                                   OI_BD_ADDR *list,
                                   OI_UINT16 *num);

#endif /* _L2CAP_GROUP_H */
