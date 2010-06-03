#ifndef AEEMIF_H
#define AEEMIF_H
/*===========================================================================

FILE:  AEEMIF.h

SERVICES:  MIF types: resource IDs of MIF resources. These are collected here
            to avoid collisions in this small (16-bit) namespace.  The below are
            partially conventions, partially requirements.

GENERAL DESCRIPTION:
   Definitions, typedefs, etc. for Module Information Files

Copyright © 2003-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

//******************************************************************************
//
// A MIF is a special resource file.  The following IDs are the fixed IDs for
// MIF resources...
//
//******************************************************************************

#define IDB_MIF_HEADER        0 // MIF Header
#define IDB_MIF_CLASSES       1 // Non-App Classes, List of AEECLSID
#define IDB_MIF_APPLETS       2 // Applets, List of AEEAppInfo
#define IDB_MIF_EXTCLASSES    3 // External classes, List of AEECLSID
#define IDB_MIF_NOTIFY        4 // Notify Structure, List of MIFNotify
#define IDB_MIF_MIMETYPE      5 // MIME Registration structure, List of MIFMIMEType

#define IDS_MIF_COMPANY       6 // Company Name
#define IDS_MIF_COPYRIGHT     7 // Copyright Date
#define IDS_MIF_VERSION       8 // Module version

#define IDB_MIF_FSLIMIT       9  // Max: Number of Files and file space limitation per module

#define IDS_MIF_APPFRAME_21   10 // Display settings passed to IAppFrame (BREW 2.1 only)

#define IDB_MIF_FS_ACLS       11 // Indicates a list of files/directories exported by this module
#define IDB_MIF_FIFO_ACLS     12 // Indicates a list of fifos exported by this module


/*
=======================================================================
CONSTANTS DOCUMENTATION
=======================================================================

IDB_MIF_HEADER

Description:
  IDB_MIF_HEADER is the ID of the binary resource type that BREW uses to denote
  a MIF header.

Definition:

   #define IDB_MIF_HEADER        0

Comments:
   The MIF header format is not publicly documented.  The resource
    ID is documented to avoid collisions with other binary resources.

=======================================================================

IDB_MIF_CLASSES

Description:
   IDB_MIF_CLASSES is the ID of the binary resource that BREW uses to associate
    classes with a module.  The contents of this resource is expected to be
    a zero-terminated list of AEECLSIDs, in little-endian form.

Definition:

   #define IDB_MIF_CLASSES       1

Comments:
   This resource is used for various things:

     - finding a module that will satisfy a CreateInstance() request
     - resolving ~<classid> paths

See Also:
   IModInfo_QueryClass()

=======================================================================

IDB_MIF_APPLETS

Description:
   IDB_MIF_APPLETS is the ID of the binary resource that holds
   the BREW Applet information.

Definition:

   #define IDB_MIF_APPLETS       2

Comments:
   The MIF applet format is not publicly documented.  The resource
    ID is documented to avoid collisions with other binary resources.

=======================================================================

IDB_MIF_EXTCLASSES

Description:
   IDB_MIF_EXTCLASSES is the ID of the binary resource that holds
   the BREW "dependency" information.
   The contents of this resource is expected to be
   a zero-terminated list of AEECLSIDs, in little-endian form.

Definition:

   #define IDB_MIF_EXTCLASSES    3

Comments:
   This resource is used in BREW to denote the list of privileges
     an Applet has while running.  It also denotes external, dynamic
     class dependencies.

=======================================================================

IDB_MIF_NOTIFY

Description:
   IDB_MIF_NOTIFY is the ID of the binary resource that holds
   the BREW "notifier" information.

Definition:

   #define IDB_MIF_NOTIFY        4

Comments:
   The format of this resource is not publicly documented.  The resource
    ID is documented to avoid collisions with other binary resources.

=======================================================================

IDB_MIF_MIMETYPE

Description:
   IDB_MIF_MIMETYPE is the ID of the binary resource that holds
   the BREW "handler" information.
   The resource is formatted as a packed list of structures,
   each comprised of 2 uint32s followed by a null-terminated string.

Definition:

   #define IDB_MIF_MIMETYPE      5

=======================================================================

IDS_MIF_COMPANY

Description:
   IDS_MIF_COMPANY is the ID of the string resource that holds
   the BREW "company" information for the module.

Definition:

   #define IDS_MIF_COMPANY       6


=======================================================================

IDS_MIF_COPYRIGHT

Description:
   IDS_MIF_COPYRIGHT  is the ID of the string resource that holds
   the BREW "copyright" information for the module.

Definition:

   #define IDS_MIF_COPYRIGHT     7

=======================================================================

IDS_MIF_VERSION

Description:
   IDS_MIF_VERSION is the ID of the string resource that holds
   the BREW "version" information for the module.

Definition:

   #define IDS_MIF_VERSION       8

=======================================================================

IDB_MIF_FSLIMIT

Description:
   IDB_MIF_FSLIMIT is the ID of the binary resource that holds
   the BREW "fs limit" information.

Definition:

   #define IDB_MIF_FSLIMIT       9

Comments:
   The format of this resource is not publicly documented.  The resource
    ID is documented to avoid collisions with other binary resources.

=======================================================================

IDS_MIF_APPFRAME_21

Description:
   IDS_MIF_APPFRAME_21 is the ID of the string resource that holds
   the BREW "appframe" information.

Definition:

   #define IDS_MIF_APPFRAME_21   10

Comments:
   The format of this resource is not publicly documented.  The resource
    ID is documented to avoid collisions with other binary resources.

=======================================================================

IDB_MIF_FS_ACLS

Description:
    IDB_MIF_FS_ACLS is the ID of the string resource that holds
    the BREW "fs acl" information.

Definition:

   #define IDB_MIF_FS_ACLS       11

Comments:
   The format of this resource is not publicly documented.  The resource
    ID is documented to avoid collisions with other binary resources.

=======================================================================

IDB_MIF_FIFO_ACLS

Description:
   IDB_MIF_FIFO_ACLS is the ID of the string resource that holds the
   BREW "fifo acl" information.

Definition:

   #define IDB_MIF_FIFO_ACLS     12

Comments:
   The format of this resource is not publicly documented.  The resource
    ID is documented to avoid collisions with other binary resources.


=======================================================================
*/
#endif /* #ifndef AEEMIF_H */

