#ifndef __OEMSERVINGNETWORKS_H__
#define __OEMSERVINGNETWORKS_H__

/*============================================================================
FILE:  OEMServingNetworks.h

SERVICES:  OEM ServingNetworks Interface

GENERAL DESCRIPTION:
   Interface definitions, data structures, etc. to support BREW Telephony

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "AEE.h"
#include "AEEModel.h"
#include "AEEWebOpts.h"
#include "AEETelephone.h"
#include "AEEIServingNetworks.h"
#include "AEEISignal.h"
#include "AEEISignalCtl.h"
#include "AEEISignalBus.h"
#include "AEESignalBus.bid"


typedef struct _OEMServingNetworks OEMServingNetworks;

#ifdef __cplusplus
extern "C" {
#endif

int OEMServingNetworks_GetSrvNetworks(OEMServingNetworks* me, AEETNwInfo *pNwInfo,int nSize, int *pAvailableNw);
int OEMServingNetworks_GetNwInfo(OEMServingNetworks* me, AEETNetworkID nwid, char *pBuff, int buffLen );
int OEMServingNetworks_RegisterSignal(OEMServingNetworks* me, ISignal *piSignal);
void OEMServingNetworks_Close(OEMServingNetworks *me);
int OEMServingNetworks_Open(OEMServingNetworks **ppObject);                             

#ifdef __cplusplus
}
#endif

/*===================================================================== 
 INTERFACES   DOCUMENTATION
=======================================================================
OEMTelephone Interface
=======================================================================

Function: OEMTelephone_Open()

Description:
   Opens up a handle to OEMTelephone.
  
Prototype:
   int OEMTelephone_Open( OEMTelephone **ppme );
      
Parameters:
   ppme: A handle is returned on SUCCESS.

Return Value:
   SUCCESS : 
   ENOMEMORY :

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_AddListener()

Description:
   Adds a listener for asynchronous notifications.
  
Prototype:
   void OEMTelephone_AddListener( OEMTelephone *pme, ModelListener *pl );
      
Parameters:
   pme: Handle to OEMTelephone.
   pl: An initialized listener.

Return Value:
   None
   
Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_Originate()

Description:
   Originate a telephone call of given type. Options are defined in AEETelephone.h
  
Prototype:
   int OEMTelephone_Originate( OEMTelephone *pme, AEETCallType ct, IWebOpts *pOpts, int *cd );
      
Parameters:
   pme : Handle to OEMTelephone.
   ct : Call type
   pOpts : options with origination parameters.
   cd : return a call descriptor.

Return Value:
   SUCCESS : 

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_SendFlash()

Description:
   Send a simple flash or flash with info. Applicable for IS95/IS2000 devices.
  
Prototype:
   int OEMTelephone_SendFlash( OEMTelephone *pme, int cd, const char *pszNumber );
      
Parameters:
   pme : Handle to OEMTelephone.
   cd : call descriptor on which the flash message is to be sent.
   pszNumber : dial string.

Return Value:
   SUCCESS :

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_Answer()

Description:
   Answer the telephone call identified by call descriptor cd.
  
Prototype:
   int OEMTelephone_Answer( OEMTelephone *pme, int cd );
      
Parameters:
   pme : Handle to OEMTelephone.
   cd : call descriptor

Return Value:
   SUCCESS :

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_End()

Description:
   End the telephone call identified by call descriptor cd.
  
Prototype:
   int OEMTelephone_End( OEMTelephone *pme, int cd );
      
Parameters:
   pme : Handle to OEMTelephone.
   cd : call descriptor

Return Value:
   SUCCESS :

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_Reject()

Description:
   Reject the incoming call.
  
Prototype:
   int OEMTelephone_Reject( OEMTelephone *pme, int cd );
      
Parameters:
   pme : Handle to OEMTelephone.
   cd : call descriptor.

Return Value:
   SUCCESS :

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_Activate()

Description:
   Activate the telephone call. Applicable if the call is on hold.
  
Prototype:
   int OEMTelephone_Activate( OEMTelephone *pme, int cd );
      
Parameters:
   pme : Handle to OEMTelephone
   cd : call descriptor

Return Value:
   SUCCESS :

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_Hold()

Description:
   Put the given call on hold.
  
Prototype:
   int OEMTelephone_Hold( OEMTelephone *pme, int cd );
      
Parameters:
   pme : Handle to OEMTelephone.
   cd : call descriptor.

Return Value:
   SUCCESS :

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_Transfer()

Description:
   Perform an explicit call transfer. One of the calls must be active and
   other in hold states.
  
Prototype:
   int OEMTelephone_Transfer( OEMTelephone *pme, int cd1, int cd2 );
      
Parameters:
   pme : Handle to OEMTelephone.
   cd1 : call descriptor.
   cd2 : call descriptor.

Return Value:
   SUCCESS :

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_Redirect()

Description:
   Redirect the incoming (Waiting) call to the given destination number.
  
Prototype:
   int OEMTelephone_Redirect( OEMTelephone *pme, int cd, const char *pszNumber );
      
Parameters:
   pme : Handle to OEMTelephone
   cd : call descriptor (incoming or waiting call)
   pszNumber : dial digits

Return Value:
   SUCCESS :

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_GetCallInfo()

Description:
   Copy the call information of call identified by cd into the given buffer.
  
Prototype:
   int OEMTelephone_GetCallInfo( OEMTelephone *pme, int cd, AEETCallInfo *pInfo, uint32 dwSize );
      
Parameters:
   pme : Handle to OEMTelephone
   cd : call descriptor
   pInfo : buffer to write the call information.
   dwSize : size of the buffer.

Return Value:
   SUCCESS :

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_GetPhoneInfo()

Description:
   Copy the phone information to the given buffer.
  
Prototype:
   int OEMTelephone_GetPhoneInfo( OEMTelephone *pme, AEETPhInfo *pInfo, uint32 dwSize );
      
Parameters:
   pme : Handle to OEMTelephone
   pInfo : buffer to write phone information
   dwSize size of the buffer.

Return Value:
   SUCCESS :

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_GetServingSystemInfo()

Description:
   Get the serving system information.
  
Prototype:
   int OEMTelephone_GetServingSystemInfo( OEMTelephone *pme, AEETSSInfo *pInfo, uint32 dwSize );
      
Parameters:
   pme : Handle to OEMTelephone
   pInfo : buffer to write data.
   dwSize : size of the buffer.

Return Value:
   SUCCESS :

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_GetOperatingMode()

Description:
   Return the current operating mode of the device.
  
Prototype:
   AEETOprtMode OEMTelephone_GetOperatingMode( OEMTelephone *pme );
      
Parameters:
   pme : Handle to OEMTelephone.

Return Value:
   AEETOprtMode : current operating mode.

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_GetSystemMode()

Description:
   Return the current system mode.
  
Prototype:
   AEETSysMode  OEMTelephone_GetSystemMode( OEMTelephone *pme );
      
Parameters:
   pme : Handle to OEMTelephone.

Return Value:
   AEETSysMode : current system mode.

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_EndAllCalls()

Description:
   Terminate all telephone calls. 
  
Prototype:
   int OEMTelephone_EndAllCalls( OEMTelephone *pme );
      
Parameters:
   pme : Handle to OEMTelephone.

Return Value:
   SUCCESS :

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_Close()

Description:
   Release the OEMTelephone Handle.
  
Prototype:
   int OEMTelephone_Close( OEMTelephone *pme );
      
Parameters:
   pme : Handle to OEMTelephone.

Return Value:
   SUCCESS :

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_SetOperatingMode()

Description:
   Set the preferred operating mode.
  
Prototype:
   int OEMTelephone_SetOperatingMode( OEMTelephone *pme, AEETOprtMode mode );
      
Parameters:
   pme : Handle to OEMTelephone.
   mode : operating mode.

Return Value:
   SUCCESS :

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_SetNamSel()

Description:
   Make the given NAM as an active one.
  
Prototype:
   int OEMTelephone_SetNamSel( OEMTelephone *pme, AEETNAM nam );
      
Parameters:
   pme : Handle to OEMTelephone
   nam : identifies the NAM.

Return Value:
   SUCCESS :

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_SetRSSIDeltaThreshold()

Description:
   Program the RSSI delta(or threshold) change for notifications. Device
sends events when RSSI changes by the given delta.
  
Prototype:
   int OEMTelephone_SetRSSIDeltaThreshold( OEMTelephone *pme, byte thr );
      
Parameters:
   pme : Handle to OEMTelephone
   thr : delta threshold

Return Value:
   SUCCESS :

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_WakeupFromStandby()

Description:
   Command phone to wakeup from standby.
  
Prototype:
   int OEMTelephone_WakeupFromStandby( OEMTelephone *pme );
      
Parameters:
   pme : Handle to OEMTelephone.

Return Value:
   SUCCESS :

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_BurstDTMF()

Description:
   Send a burst of DTMF digits with the given on/off duration. The given
call must be active.
  
Prototype:
   int OEMTelephone_BurstDTMF( OEMTelephone *pme, int cd, 
                               AEETDTMFOnLength on, AEETDTMFOffLength off, 
                               const char *pszDigits );
      
Parameters:
   pme : Handle to OEMTelephone
   cd : Call descriptor on which to send the dtmfs.
   on : Enumeration of tone ON duration
   off : Enumeration of tone OFF duration
   pszDigits : null terminated sequence of digits.

Return Value:
   SUCCESS :

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_StartDTMF()

Description:
   Start a DTMF corresponding to the given digit.
  
Prototype:
   int OEMTelephone_StartDTMF( OEMTelephone *pme, int cd, char bDigit );
      
Parameters:
   pme : Handle to OEMTelephone
   cd : call descriptor must be an active call.
   bDigit : a Digit.

Return Value:
   SUCCESS :

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_StopDTMF()

Description:
   Stop the DTMF tone being played on the call. This stops the burst DTMFs.
  
Prototype:
   int OEMTelephone_StopDTMF( OEMTelephone *pme, int cd );
      
Parameters:
   pme : Handle to OEMTelephone
   cd : call descriptor   

Return Value:
   SUCCESS :

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_SetSubscriptionStatus()

Description:
   Inform the availability of subscription on a given system mode. This information
is used by device to activate the protocol stack corresponding to the system mode.
  
Prototype:
   int OEMTelephone_SetSubscriptionStatus( OEMTelephone *pme, AEETSysMode mode, boolean status );
      
Parameters:
   pme : Handle to OEMTelephone
   mode : system mode.
   status : TRUE to indicate subscription available.

Return Value:
   SUCCESS :

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_Command()

Description:
   A generic command interface used by IPhoneCtl_Command to issue commands. 
  
Prototype:
   int OEMTelephone_Command( OEMTelephone *pme, uint32 dwCmd, AEECallback *pcb, void *pInBuffer, 
                       uint32 dwInBufSize, void *pOutBuffer, uint32 dwOutBufSize );
      
Parameters:
   pme : Handle to OEMTelephone
   dwCmd : Command code as described in AEETelephone.h
   pcb : Used by Asynchronous requests.
   pInBuffer : request parameters.
   dwInBufSize : sizeof request parameters buffer.
   pOutBuffer : Response buffer
   dwOutBufSize : Size of response buffer.

Return Value:
   SUCCESS :

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_IsModeCDMA()

Description:
   Helper function to determine a given mode is a IS-95 class.
  
Prototype:
   boolean OEMTelephone_IsModeCDMA( AEETSysMode );
      
Parameters:
   AEETSysMode : the mode.

Return Value:
   TRUE : the given mode is IS-95/IS-2000 category.

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_IsModeGW()

Description:
   Helper function to determine a given mode is a GSM class.
  
Prototype:
   boolean OEMTelephone_IsModeGW( AEETSysMode );
      
Parameters:
   AEETSysMode : the mode.

Return Value:
   TRUE : the given mode is a GSM/UMTS category.

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_IsCallTypeVoice()

Description:
   Helper function to determine a given call type carries voice information.
  
Prototype:
   boolean OEMTelephone_IsCallTypeVoice( AEETCallType );
      
Parameters:
   AEETCallType : call type.

Return Value:
   TRUE : the given call type carriers voice.

Comments:
   None

Side Effects:
   None

See Also:
   None


=======================================================================

Function: OEMTelephone_GetLineInfo()

Description:
   Obtains the alternate line supporting information of the device.
  
Prototype:
   int OEMTelephone_GetLineInfo( OEMTelephone *pme, AEETLineInfo *po, uint32 dwSize );
      
Parameters:
   pme : Handle to OEMTelephone.
   po : Buffer to return data.
   dwSize : size of data.

Return Value:
   SUCCESS : when information is copied.

Comments:
   None

Side Effects:
   None

See Also:
   None


=======================================================================

Function: OEMTelephone_SelectLine()

Description:
   Select a line by id if the device supports alternate lines.
  
Prototype:
   int OEMTelephone_SelectLine( OEMTelephone *pme, uint32 lineid );
      
Parameters:
   pme : Handle to OEMTelephone.
   lineid : line identifier

Return Value:
   SUCCESS when the line id is accepted.

Comments:
   None

Side Effects:
   None

See Also:
   None


=======================================================================

Function: OEMTelephone_EnableLineSwitch()

Description:
   Enable line switching of the device.
  
Prototype:
   int OEMTelephone_EnableLineSwitch( OEMTelephone *pme, boolean bEnable );
      
Parameters:
   pme : Handle to OEMTelephone
   bEnable : TRUE to turn ON line switching, FALSE to turn OFF switching.

Return Value:
   SUCCESS when the command is accepted.

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: OEMTelephone_GetMaxCallDescriptors()

Description:
   Obtains the maximum simultaneous calls allowed by the device.
  
Prototype:
   int OEMTelephone_GetMaxCallDescriptors( OEMTelephone *pme, uint32 *pdwCount );
      
Parameters:
   pme : Handle to OEMTelephone
   pdwCount : buffer to return the maximum count.

Return Value:
   SUCCESS when the command is accepted.

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================*/

#endif /* __OEMTELEPHONE_H__ */
