#ifndef OEMCLASSIDS_H
#define OEMCLASSIDS_H
/*======================================================
FILE:  AEEOEMClassIDs.h

SERVICES:  OEM specific class IDs

GENERAL DESCRIPTION:
   Base level definitions, typedefs, etc. for AEE

        Copyright ?1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEEClassIDs.h"

#define AEECLSID_LAUNCHER          (AEECLSID_OEM_APP)
#define AEECLSID_UPBROWSER         (AEECLSID_BROWSER)
#define AEECLSID_UP4BROWSER        (AEECLSID_BROWSER)

#define AEECLSID_PROVISION_PRL     (AEECLSID_OEM + 0x100)             
#define AEECLSID_PROVISION_NAM     (AEECLSID_OEM + 0x101)
#define AEECLSID_PROVISION_CLOGO   (AEECLSID_OEM + 0x102)
#define AEECLSID_OEMBRIDGE         (AEECLSID_OEM + 0x103)
#define AEECLSID_DOWNSHIFT         (AEECLSID_OEM + 0x104)

#define AEECLSID_OEMADDRBOOK       (AEECLSID_OEM + 0x105)
#define AEECLSID_OEMRUIMADDRBOOK   (AEECLSID_OEM + 0x106)

#ifdef CUST_EDITION	
#define AEECLSID_CORE_APP          (AEECLSID_OEM + 0x107) // 0x01006107
#define AEECLSID_APP_SETTINGMENU   (AEECLSID_OEM + 0x108)
#define AEECLSID_APP_SOUNDMENU     (AEECLSID_OEM + 0x109)
#define AEECLSID_APP_SECURITYMENU  (AEECLSID_OEM + 0x10a)
#define AEECLSID_APP_DISPLAYMENU   (AEECLSID_OEM + 0x10b)
#define AEECLSID_DIALER            (AEECLSID_OEM + 0x10c)
#define AEECLSID_STOPWATCH         (AEECLSID_OEM + 0x10d)
#define AEECLSID_CALCAPP           (AEECLSID_OEM + 0x10e)
#define AEECLSID_MAIN_MENU         (AEECLSID_OEM + 0x10f)
#define AEECLSID_GAME_TETRIS	   (AEECLSID_OEM + 0x110)
#define AEECLSID_QUICKTEST         (AEECLSID_OEM + 0x111)
#define AEECLSID_EXTRAMENU         (AEECLSID_OEM + 0x112)
#define AEECLSID_APP_RECENTCALL    (AEECLSID_OEM + 0x113)
#define AEECLSID_APP_UTK           (AEECLSID_OEM + 0x114)
#define AEECLSID_CALLLIST          (AEECLSID_OEM + 0x115)
#define AEECLSID_WMSAPP            (AEECLSID_OEM + 0x116) // 0x01006116 
#define AEECLSID_WORLDTIME         (AEECLSID_OEM + 0x117)
#define AEECLSID_ALARMCLOCK        (AEECLSID_OEM + 0x118)
#define AEECLSID_APP_CONTACT       (AEECLSID_OEM + 0x11a)

#ifdef FEATURE_FLEXI_STATIC_BREW_APP
#define AEECLSID_NASRANI    	0x0109d6ca
#endif

#include "ADDRBOOK_RUIM.BID"
//#define AEECLSID_ADDRBOOK_RUIM     (AEECLSID_OEM + 0x11b) 
#define AEECLSID_FIELDDEBUGAPP     (AEECLSID_OEM + 0x11c)
#define AEECLSID_SVCPRGAPP         (AEECLSID_OEM + 0x11d)
#define AEECLSID_SCHEDULEAPP     (AEECLSID_OEM + 0x11e)
// dont use 0x01008000, it belongs to app manager
#define AEECLSID_MSHOP				0x01010ea7
#define AEECLSID_APP_FMRADIO        (AEECLSID_OEM + 0x11f)
#define AEECLSID_BRICK              (AEECLSID_OEM + 0x120)
#define AEECLSID_BLACKJACK          (AEECLSID_OEM + 0x121)
#define AEECLSID_UIALARM         (AEECLSID_OEM + 0x122)
#define AEECLSID_APP_OTK            (AEECLSID_OEM + 0x123)

#define AEECLSID_MEDIAGALLERY       (AEECLSID_OEM + 0x124)//0x01006124
#define AEECLSID_APP_CAMERA         (AEECLSID_OEM + 0x125)
#define AEECLSID_APP_MUSICPLAYER    (AEECLSID_OEM + 0x126)
#define AEECLSID_VIDEOPLAYER        (AEECLSID_OEM + 0x127)
#define AEECLSID_APPTIMER           (AEECLSID_OEM + 0x128)
#define AEECLSID_CONVERTER          (AEECLSID_OEM + 0x129)
#define AEECLSID_BTUIAPP            (AEECLSID_OEM + 0x12a)
#define AEECLSID_SPORTSAPP          (AEECLSID_OEM + 0x12b)
#if defined( FEATURE_JEWISH_CALENDAR)
#define AEECLSID_JEWISH_CALENDAR    (AEECLSID_OEM + 0x12c)
#endif

#define AEECLSID_MORSE       (AEECLSID_OEM + 0x12d)
#define AEECLSID_RECORDER           (AEECLSID_OEM + 0x12e)
#define AEECLSID_GAME_LKV			(AEECLSID_OEM + 0x12f)

#define AEECLSID_NUMBERMANAGER      (AEECLSID_OEM + 0x130)

#define AEECLSID_GAME_LKVFGTT       (AEECLSID_OEM + 0x131)
#define AEECLSID_EDIT_EXTENSION      (AEECLSID_OEM + 0x132)    //  for Edit Extension.
#define AEECLSID_OEMEXT_FOR3PART   (AEECLSID_OEM + 0x133)    //  for WAP Extension. 
#define AEECLSID_OEMMMSEXT               (AEECLSID_OEM + 0x134)    //  for MMS Extension.
#define AEECLSID_TEXT_APPLET             (AEECLSID_OEM_APP + 0x101)    //  for Edit Applet
#define AEECLSID_ANICTL			(AEECLSID_OEM + 0x135)
#define AEECLSID_CUSTSERVICE     (AEECLSID_OEM + 0X136)   
#define AEECLSID_ESURFING        (AEECLSID_OEM + 0X137)   
#define AEECLSID_NOTEBOOK        (AEECLSID_OEM + 0X138)  
#define AEECLSID_ADJUSTPENAPP      (AEECLSID_OEM + 0x500) //wlh 20090407 add
#define AEECLSID_OEMITSIM          (AEECLSID_OEM + 0x501)   //   //add by ydc
#define AEECLSID_VKEY_CONTROL      (AEECLSID_OEM + 0x502)   //0x01006131
#define AEECLSID_APPLICATION       (AEECLSID_OEM + 0x503)   //add by yangdecai
#define AEECLSID_GAME              (AEECLSID_OEM + 0x504)   //add by yangdecai
#define AEECLSID_FRENDUO           (AEECLSID_OEM + 0x505)   //add by yangdecai


#endif /*CUST_EDITION*/
/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
AEEOEMClassIDs

Description:


Definition:

#define AEECLSID_LAUNCHER          (AEECLSID_OEM_APP)
#define AEECLSID_UPBROWSER         (AEECLSID_BROWSER)
#define AEECLSID_UP4BROWSER        (AEECLSID_BROWSER)

#define AEECLSID_PROVISION_PRL     (AEECLSID_OEM + 0x100)             
#define AEECLSID_PROVISION_NAM     (AEECLSID_OEM + 0x101)
#define AEECLSID_PROVISION_CLOGO   (AEECLSID_OEM + 0x102)
#define AEECLSID_OEMBRIDGE         (AEECLSID_OEM + 0x103)
#define AEECLSID_DOWNSHIFT         (AEECLSID_OEM + 0x104)

#define AEECLSID_OEMADDRBOOK       (AEECLSID_OEM + 0x105)
#define AEECLSID_OEMRUIMADDRBOOK   (AEECLSID_OEM + 0x106)

Members:
None

See Also:
None

======================================================================*/

#endif   // OEMCLASSIDS_H


