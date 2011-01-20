#ifndef AEEMCPATHS_H
#define AEEMCPATHS_H
/*======================================================
FILE:  AEEMCPaths.h
SERVICES:  AEE Media Content Paths

GENERAL DESCRIPTION:
	This file contains path definitions for accessing various content directories

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEEFile.h"

#define AEE_LOCKED_MYSOUNDS_DIR  "fs:/~0x0102e5fb/"
#define AEE_LOCKED_MYPIX_DIR     "fs:/~0x0102e5fc/" 
#define AEE_LOCKED_MYFLIX_DIR    "fs:/~0x0102e5fd/"
#define AEE_MYFLIX_DIR           "fs:/~0x0102e5fe/"
#define AEE_MYMUSIC_DIR          "fs:/~0x0102382d/"
#define AEE_MYPIX_DIR            "fs:/~0x0102382c/"
#define AEE_MYSOUNDS_DIR         "fs:/~0x0102e5e7/"


#endif //#ifndef AEEMCPATHS_H

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
======================================================================= 

AEEFS_CARD0_DIR

Description:
Specifies the path to the MMC card on the device. This path can be used with
IFileMgr interface for dealing with the directory on the MMC Card.
The privelege AEECLSID_CARD0_ACCESS is required to access the MMC Card.

This path can be used in conjunction with all the functions of the IFileMgr interface
that take a filename

For ex: to create a file called "foo.txt" on the MMC Card, the following sample
can be used:
   IFILEMGR_OpenFile(pfm,AEEFS_CARD0_DIR"foo.txt",_OFM_CREATE);

Definition:
#define AEEFS_CARD0_DIR          "fs:/card0/"

Members: 
None

Comments:
None

See Also:
None

=======================================================================
AEE_LOCKED_MYSOUNDS_DIR

Description:
Specifies the path to a specific directory that stores content that belongs to the
category "Locked MySounds". This path can be used with IFileMgr interface for 
accessing this special directory. 
The privelege AEECLSID_LOCKED_CONTENT_ACCESS  is required to access this directory.

This path can be used in conjunction with all the functions of the IFileMgr interface
that take a filename

For ex: to create a file called "foo.txt" in this directory, the following sample
can be used:
   IFILEMGR_OpenFile(pfm,AEE_LOCKED_MYSOUNDS_DIR"foo.txt",_OFM_CREATE);

Definition:
#define AEE_LOCKED_MYSOUNDS_DIR  "fs:/~0x0102e5fb/"

Members: 
None

Comments:
None

See Also:
None

=======================================================================
AEE_LOCKED_MYPIX_DIR

Description:
Specifies the path to a specific directory that stores content that belongs to the
category "Locked MyPix". This path can be used with IFileMgr interface for 
accessing this special directory. 
The privelege AEECLSID_LOCKED_CONTENT_ACCESS   is required to access this directory.

This path can be used in conjunction with all the functions of the IFileMgr interface
that take a filename


For ex: to create a file called "foo.txt" in this directory, the following sample
can be used:
   IFILEMGR_OpenFile(pfm,AEE_LOCKED_MYPIX_DIR"foo.txt",_OFM_CREATE);

Definition:
#define AEE_LOCKED_MYPIX_DIR  "fs:/~0x0102e5fc/"

Members: 
None

Comments:
None

See Also:
None

=======================================================================
AEE_LOCKED_MYFLIX_DIR

Description:
Specifies the path to a specific directory that stores content that belongs to the
category "Locked MyFlix". This path can be used with IFileMgr interface for 
accessing this special directory. 
The privelege AEECLSID_LOCKED_CONTENT_ACCESS   is required to access this directory.

This path can be used in conjunction with all the functions of the IFileMgr interface
that take a filename

For ex: to create a file called "foo.txt" in this directory, the following sample
can be used:
   IFILEMGR_OpenFile(pfm,AEE_LOCKED_MYFLIX_DIR"foo.txt",_OFM_CREATE);

Definition:
#define AEE_LOCKED_MYFLIX_DIR "fs:/~0x0102e5fd/"

Members: 
None

Comments:
None

See Also:
None

=======================================================================

AEE_MYFLIX_DIR

Description:
Specifies the path to a specific directory that stores content that belongs to the
category "MyFlix". This path can be used with IFileMgr interface for 
accessing this special directory. 
No privelege is required to access this directory.

This path can be used in conjunction with all the functions of the IFileMgr interface
that take a filename

For ex: to create a file called "foo.txt" in this directory, the following sample
can be used:
   IFILEMGR_OpenFile(pfm,AEE_MYFLIX_DIR"foo.txt",_OFM_CREATE);

Definition:
#define AEE_MYFLIX_DIR  "fs:/~0x0102e5fe/"

Members: 
None

Comments:
None

See Also:
None

=======================================================================

AEE_MYMUSIC_DIR

Description:
Specifies the path to a specific directory that stores content that belongs to the
category "MyMusic". This path can be used with IFileMgr interface for 
accessing this special directory. 
The privelege AEEGROUPID_MCF_MUSIC is required to access this directory.

This path can be used in conjunction with all the functions of the IFileMgr interface
that take a filename

For ex: to create a file called "foo.txt" in this directory, the following sample
can be used:
   IFILEMGR_OpenFile(pfm,AEE_MYMUSIC_DIR"foo.txt",_OFM_CREATE);

Definition:
#define AEE_MYMUSIC_DIR  "fs:/~0x0102382d/"

Members: 
None

Comments:
None

See Also:
None

=======================================================================
AEE_MYPIX_DIR

Description:
Specifies the path to a specific directory that stores content that belongs to the
category "MyPix". This path can be used with IFileMgr interface for 
accessing this special directory. 
No privelege is required to access this directory.

This path can be used in conjunction with all the functions of the IFileMgr interface
that take a filename

For ex: to create a file called "foo.txt" in this directory, the following sample
can be used:
   IFILEMGR_OpenFile(pfm,AEE_MYPIX_DIR"foo.txt",_OFM_CREATE);

Definition:
#define AEE_MYPIX_DIR  "fs:/~0x0102382c/"

Members: 
None

Comments:
None

See Also:
None

=======================================================================
AEE_MYSOUNDS_DIR

Description:
Specifies the path to a specific directory that stores content that belongs to the
category "MySounds". This path can be used with IFileMgr interface for 
accessing this special directory. 
No privelege is required to access this directory.

This path can be used in conjunction with all the functions of the IFileMgr interface
that take a filename

For ex: to create a file called "foo.txt" in this directory, the following sample
can be used:
   IFILEMGR_OpenFile(pfm,AEE_MYSOUNDS_DIR"foo.txt",_OFM_CREATE);

Definition:
#define AEE_MYSOUNDS_DIR  "fs:/~0x0102e5e7/"

Members: 
None

Comments:
None

See Also:
None

=====================================================================*/
