#ifndef AEEMCF_H
#define AEEMCF_H
/*=====================================================================
FILE:  AEEMCF.h

SERVICES:  Media Content File support

DESCRIPTION:  Starting from BREW 3.1, MCF provides a method to store
media files in designated directories, which can only be accessed by
authorized application. This access is controlled by the ACL enforced
by IFileMgr.

PUBLIC CLASSES: Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS: Not Applicable
        
Copyright © 2005-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=====================================================================*/

#include "AEEFile.h"

//---------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------

//---------------------------------------------------------------------
// Access Macros
//---------------------------------------------------------------------


#define AEEFS_MCF_PICTURE_DIR          "fs:/~0x0102382c/"
#define AEEFS_MCF_PICTURE_LOCK_DIR     "fs:/~0x0102e5fc/"

#define AEEFS_MCF_MUSIC_DIR            "fs:/~0x0103201e/"
#define AEEFS_MCF_MUSIC_LOCK_DIR       "fs:/~0x0102382d/"

#define AEEFS_MCF_MOVIE_DIR            "fs:/~0x0102382e/"
#define AEEFS_MCF_MOVIE_LOCK_DIR       "fs:/~0x0102e5fd/"

#define AEEFS_MCF_THEME_DIR            "fs:/~0x0102382f/"
#define AEEFS_MCF_THEME_LOCK_DIR       "fs:/~0x01031fd5/"

#define AEEFS_MCF_TEXT_DIR             "fs:/~0x01023830/"
#define AEEFS_MCF_TEXT_LOCK_DIR        "fs:/~0x01031fd6/"

#define AEEFS_MCF_SOUND_DIR            "fs:/~0x0102e5e7/"
#define AEEFS_MCF_SOUND_LOCK_DIR       "fs:/~0x0102e5fb/"

#define AEEFS_MCF_LICENSE_LOCK_DIR     "fs:/~0x010315fa/"

#define AEEFS_MCF_LINK_DIR             "fs:/~0x0103201b/"
#define AEEFS_MCF_LINK_LOCK_DIR        "fs:/~0x0103201c/"


/*=======================================================================
  INTERFACE DOCUMENTATION
======================================================================= 

Interface Name: Multimedia_Content_File

Description:

Starting with BREW 3.1, instead of storing multimedia files into the BREW
shared directory to be shared among multiple applications, Multimedia
Content File (MCF) is utilized to share, and at the same time, control 
access to the multimedia contents.

MCF utilizes the BREW 3.1 file system's ACL (Access Control List) to
control access to certain multimedia content directories. Each MCF type
is defined as a Group ID and has a separate module directory. For more 
information on the ACL, please refer to the SDK Reference Guide.

There are two types of MCF directories for each multimedia type:

===pre>

  * Unlocked: this is where you store unprotected contents that can be shared
              with all BREW applications with access to the File System.
  * Locked: this is where you store protected contents that can only be
            shared with BREW applications that belong to the appropriate
            group or has the appropriate access privilege level. 

===/pre>

You can use the provided macros to access the MCF directories.

MCF Types:

===pre>

  * Picture: this is where you store still or simple animation pictures
             (e.g. BMP, GIF, PNG, JPEG, etc.).
    - Unlocked:
      = Group ID: 0x0102382c
      = Macro: AEEFS_MCF_PICTURE_DIR
      = Macro Definition: fs:/~0x0102382c/
      = Module ID: 10888
      = Canonical Path Name: fs:/mod/10888
      = Associated BID File: AEEGROUPID_MCF_PICTURE.BID
      = Access Privilege: None
    - Locked:
      = Group ID: 0x0102e5fc
      = Macro: AEEFS_MCF_PICTURE_LOCK_DIR
      = Macro Definition: fs:/~0x0102e5fc/
      = Module ID: 19919
      = Canonical Path Name: fs:/mod/19919
      = Associated BID File: AEEGROUPID_MCF_PICTURE_LOCK.BID
      = Access Privilege: LOCKED_CONTENT_ACCESS (0x0102e82d) or Group ID (0x0102e5fc)

  * Music: this is where you store music files (e.g. MP3, MIDI, WAV, etc.).
    - Unlocked:
      = Group ID: 0x0103201e
      = Macro: AEEFS_MCF_MUSIC_DIR
      = Macro Definition: fs:/~0x0103201e/
      = Module ID: 19920
      = Canonical Path Name: fs:/mod/19920
      = Associated BID File: AEEGROUPID_MCF_MUSIC_UNLOCKED.BID
      = Access Privilege: None
    - Locked:
      = Group ID: 0x0102382d
      = Macro: AEEFS_MCF_MUSIC_LOCK_DIR
      = Macro Definition: fs:/~0x0102382d/
      = Module ID: 10889
      = Canonical Path Name: fs:/mod/10889
      = Associated BID File: AEEGROUPID_MCF_MUSIC.BID
      = Access Privilege: Group ID (0x0102382d)

  * Movie: this is where you store movie files (e.g. MPEG-4, MPEG-2, AVI, etc.).
    - Unlocked:
      = Group ID: 0x0102382e and 0x0102e5fe
      = Macro: AEEFS_MCF_MOVIE_DIR
      = Macro Definition: fs:/~0x0102382e/
      = Module ID: 10890
      = Canonical Path Name: fs:/mod/10890
      = Associated BID File: AEEGROUPID_MCF_MOVIE.BID
      = Access Privilege: None
    - Locked:
      = Group ID: 0x0102e5fd
      = Macro: AEEFS_MCF_MOVIE_LOCK_DIR
      = Macro Definition: fs:/~0x0102e5fd/
      = Module ID: 19922
      = Canonical Path Name: fs:/mod/19922
      = Associated BID File: AEEGROUPID_MCF_MOVIE_LOCK.BID
      = Access Privilege: LOCKED_CONTENT_ACCESS (0x0102e82d) or Group ID (0x0102e5fd)

  * Sound: this is where you store tone or simple sound files.
    - Unlocked:
      = Group ID: 0x0102e5e7
      = Macro: AEEFS_MCF_SOUND_DIR
      = Macro Definition: fs:/~0x0102e5e7/
      = Module ID: 18067
      = Canonical Path Name: fs:/mod/18067
      = Associated BID File: AEEGROUPID_MCF_SOUND.BID
      = Access Privilege: None
    - Locked:
      = Group ID: 0x0102e5fb
      = Macro: AEEFS_MCF_SOUND_LOCK_DIR
      = Macro Definition: fs:/~0x0102e5fb/
      = Module ID: 16225
      = Canonical Path Name: fs:/mod/16225
      = Associated BID File: AEEGROUPID_MCF_SOUND_LOCK.BID
      = Access Privilege: LOCKED_CONTENT_ACCESS (0x0102e82d) or Group ID (0x0102e5fb)
   
  * Theme: this is where you store handset theme files. The file format in this
           directory may be specific to each handset model.
    - Unlocked:
      = Group ID: 0x0102382f
      = Macro: AEEFS_MCF_THEME_DIR
      = Macro Definition: fs:/~0x0102382f/
      = Module ID: 10891
      = Canonical Path Name: fs:/mod/10891
      = Associated BID File: AEEGROUPID_MCF_THEME.BID
      = Access Privilege: None
    - Locked:
      = Group ID: 0x01031fd5
      = Macro: AEEFS_MCF_THEME_LOCK_DIR
      = Macro Definition: fs:/~0x01031fd5/
      = Module ID: 19917
      = Canonical Path Name: fs:/mod/19917
      = Associated BID File: AEEGROUPID_MCF_THEME_LOCK.BID
      = Access Privilege: Group ID (0x01031fd5)
      
  * Text: this is where you store text-based media files (e.g. TXT).
    - Unlocked:
      = Group ID: 0x01023830
      = Macro: AEEFS_MCF_TEXT_DIR
      = Macro Definition: fs:/~0x01023830/
      = Module ID: 10892
      = Canonical Path Name: fs:/mod/10892
      = Associated BID File: AEEGROUPID_MCF_TEXT.BID
      = Access Privilege: None
    - Locked:
      = Group ID: 0x01031fd6
      = Macro: AEEFS_MCF_TEXT_LOCK_DIR
      = Macro Definition: fs:/~0x01031fd6/
      = Module ID: 19918
      = Canonical Path Name: fs:/mod/19918
      = Associated BID File: AEEGROUPID_MCF_TEXT_LOCK.BID
      = Access Privilege: Group ID (0x01031fd6)

  * License: this is where you store license-related information or files.
    - Unlocked: Not Provided
    - Locked:
      = Group ID: 0x010315fa
      = Macro: AEEFS_MCF_LICENSE_LOCK_DIR
      = Macro Definition: fs:/~0x010315fa/
      = Module ID: 19240
      = Canonical Path Name: fs:/mod/19240
      = Associated BID File: AEEGROUPID_MCF_LICENSE_LOCK.BID
      = Access Privilege: LOCKED_LICENSE_ACCESS (0x0103160c) or Group ID (0x010315fa)

  * Link: this is where you store soft-links, bookmarks or URL-related files.
    - Unlocked:
      = Group ID: 0x0103201b
      = Macro: AEEFS_MCF_LINK_DIR
      = Macro Definition: fs:/~0x0103201b/
      = Module ID: 19924
      = Canonical Path Name: fs:/mod/19924
      = Associated BID File: AEEGROUPID_MCF_LINK.BID
      = Access Privilege: None
    - Locked:
      = Group ID: 0x0103201c
      = Macro: AEEFS_MCF_LINK_LOCK_DIR
      = Macro Definition: fs:/~0x0103201c/
      = Module ID: 19923
      = Canonical Path Name: fs:/mod/19923
      = Associated BID File: AEEGROUPID_MCF_LINK_LOCK.BID
      = Access Privilege: LOCKED_CONTENT_ACCESS (0x0102e82d) or Group ID (0x0103201c)

===/pre>
    

Code Example:

===pre>

  IFileMgr * pIFileMgr;
  IFile * pMyFile;
  ...
  ...
  
  pMyFile = IFILEMGR_Open(pIFileMgr, AEEFS_MCF_MUSIC_DIR"myMusic.mp3", _OFM_READ);

===/pre>
  

======================================================================= */

#endif // AEEMCF_H
