#ifndef AEECORENOTIFIER_H
#define AEECORENOTIFIER_H
/*=====================================================================
FILE:  AEECoreNotifier.h

SERVICES:  AEE Core Notifier Interfaces

GENERAL DESCRIPTION:
   Base level definitions, typedefs, etc. for AEE Core Notifier

        Copyright © 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
======================================================================*/

//*********************************************************************
//
// ICoreNotifier Interface
//
//*********************************************************************
#include "AEE.h"

typedef struct ICoreNotifier         ICoreNotifier;

AEEINTERFACE(ICoreNotifier)
{
   INHERIT_INotifier(ICoreNotifier);
};

// Notification types
typedef enum
{
   CAI_NONE,
   CAI_APP_CHECK_RECALL,
   CAI_APP_RECALL,
   CAI_AUTO_INSTALL,
   CAI_REG_APPS,
   CAI_DL_STATUS
} CoreAppUIInfoType;

typedef enum
{
   CAI_STATUS_BEGIN,
   CAI_STATUS_END
} CoreAppUIStatusType;

// Structure pointed to by dwParam of the EVT_NOTIFY event
typedef struct
{
   CoreAppUIInfoType    InfoType;
   CoreAppUIStatusType  Status;
   int                  nErr;    //Status code
   uint32               nExtra;  //Extra information
   union {
      AECHAR* AppName;  // Used only by CAI_APP_RECALL, CAI_DL_STATUS
   } data;
} CoreAppUIInfo;

#define NMASK_CORE_APP_RECALL    0x00000001
#define NMASK_CORE_AUTO_INSTALL  0x00000002
#define NMASK_CORE_REG_APPS      0x00000004
#define NMASK_CORE_DL_STATUS     0x00000008

/*=====================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

ICoreNotifier Interface

Description:
  ICoreNotifier is the notifier class with which applications can register
  for core notifications in following ways:

  1. Using application MIF
  2. Using ISHELL_RegisterNotify()

Following are the notification masks supported by ICoreNotifier:
===pre>

#define NMASK_CORE_APP_RECALL    0x00000001
#define NMASK_CORE_AUTO_INSTALL  0x00000002
#define NMASK_CORE_REG_APPS      0x00000004
#define NMASK_CORE_DL_STATUS     0x00000008

===/pre>
The following header file is required:~
AEECoreNotifier.h
=======================================================================

NMASK_CORE_APP_RECALL

Description:

   This is a notification mask that allows applications to register for
   app recalls. 

   The application is notified through the EVT_NOTIFY event. 
   The dwParam of this event is of type CoreAppUIInfo.

Definition:
   #define NMASK_CORE_APP_RECALL       0x0001

Members:
   None

Comments:
   None

See Also:
   None

=======================================================================

NMASK_CORE_AUTO_INSTALL

Description:

   This is a notification mask that allows applications to register for
   information about auto installs. 

   The application is notified through the EVT_NOTIFY event. 
   The dwParam of this event is of type CoreAppUIInfo.

Definition:
   #define NMASK_CORE_AUTO_INSTALL       0x0002

Members:
   None

Comments:
   None

See Also:
   None

=======================================================================

NMASK_CORE_REG_APPS

Description:

   This is a notification mask that allows applications to register for
   the state of registration (sending ACKS)

   The application is notified through the EVT_NOTIFY event. 
   The dwParam of this event is of type CoreAppUIInfo.

Definition:
   #define NMASK_CORE_REG_APPS       0x0004

Members:
   None

Comments:
   None

See Also:
   None

=======================================================================

NMASK_CORE_DL_STATUS

Description:

   This is a notification mask that allows applications to register for
   download status of delayed ACKs

   The application is notified through the EVT_NOTIFY event. 
   The dwParam of this event is of type CoreAppUIInfo.

Definition:
   #define NMASK_CORE_DL_STATUS       0x00000008

Members:
   None

Comments:
   None

See Also:
   None

=======================================================================



=======================================================================*/

#endif   // AEECORENOTIFIER_H

