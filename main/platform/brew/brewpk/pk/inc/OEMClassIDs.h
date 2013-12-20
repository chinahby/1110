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
#define AEECLSID_NEMOBREW 			0x01007180
#ifdef FEATURE_FLEXI_STATIC_BREW_APP
#define AEECLSID_NASRANI    	0x0109d6ca
#define AEECLSID_MUSLIM         0x01111010
#define AEECLSID_FCHAT          0x01007262
#define AEECLSID_FBROWSER       0x19882401
#define AEECLSID_FLEXIGURU    	0x0109a9cd  //add for flexi

#elif defined FEATURE_SMARTFREN_STATIC_BREW_APP
#define AEECLSID_SMARTFREN_MNEWS 	0x1008281	//N48C	M8 / SMARTFREN	M-News
#define AEECLSID_SMARTFREN_MSHOP 	0x1008282	//N48C	M8 / SMARTFREN	M-Shop
#define AEECLSID_SMARTFREN_MKARAOKE 0x1008283	//N48C	M8 / SMARTFREN	M-Karaoke
#define AEECLSID_SMARTFREN_SATU 	0x1008284	//N48C	M8 / SMARTFREN	Seribu Satu Kisah
#define AEECLSID_SMARTFREN_FACEBOOK 0x1008288	//N48C	M8 / SMARTFREN	Facebook
#define AEECLSID_SMARTFREN_TWITTER 	0x1008286	//N48C	M8 / SMARTFREN	Twitter
#define AEECLSID_SMARTFREN_SFM 		0x1008287	//N48C	M8 / SMARTFREN	SFM
#define AEECLSID_SMARTFREN_TEST 	0xEC0AD103	//N48C	M8 / SMARTFREN	SFM

#define AEECLSID_SMARTFREN_MDIRECTORY 	0x010bb7d5	//N48C	M8 / SMARTFREN	M-Directory
#define AEECLSID_SMARTFREN_FGEN    	    0x0100829F	//MusicBox


#define AEECLSID_SMARTFREN_BOMB		0x01088652
#define AEECLSID_SMARTFREN_MAGICSUSHI 0x456123B9

#define AEECLSID_SMARTFREN_MSHOP_HITZ233C		0x10082A2	//Hitz 233C	SMARTFREN	M-Shop
#define AEECLSID_SMARTFREN_FACEBOOK_HITZ233C 	0x10082A0	//Hitz 233C	SMARTFREN	Facebook
#define AEECLSID_SMARTFREN_TWITTER_HITZ233C 	0x10082A1	//Hitz 233C	SMARTFREN	Twitter
#define AEECLSID_SMARTFREN_SFM_HITZ233C 		0x10082A3	//Hitz 233C	SMARTFREN	SFM
#define AEECLSID_SMARTFREN_MOKA_HITZ233C 		0x10082A4	//Hitz 233C	SMARTFREN	SFM
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
//#define AEECLSID_WAPBROWSER        0x0104C960               //add by yangdecai 2011-04-11
#if defined(FEATURE_VERSION_M8) || defined(FEATURE_VERSION_M8P) || defined (FEATURE_VERSION_M8021)
#define AEECLSID_FRENDUO           (AEECLSID_OEM + 0x505)   //add by yangdecai
#endif
#if defined(FEATURE_VERSION_C316)
#define AEECLSID_INDEX_ZONE			(AEECLSID_OEM + 0x506)	//Add By zzg for C316 index_zone
#define AEECLSID_SERVICE			(AEECLSID_OEM + 0x507)	//Add By zzg for C316 index_zone
#endif
#define AEECLSID_MULTIMEDIA_LIST   (AEECLSID_OEM + 0x600)   //add by yangdecai
#define AEECLSID_STATIC_APP        (AEECLSID_OEM + 0x601)   //add by yangdecai
#define AEECLSID_TVAPP            (AEECLSID_OEM + 0x602)   //add by pengyuangui  AEECLSID_Tv_APP
#define MMI_CLSID_IMMITV          (AEECLSID_OEM + 0x603)   
#define AEECLSID_TLG_ATV          (AEECLSID_OEM +0x604)
#define AEECLSID_BRW_APP    	    0x2104c960
#define AEECLSID_NF3				0x01007016
#define AEECLSID_UCWEB				0x20068888            //add by pengyuangui 2012/09/06
#define AEECLSID_MiZone				0x20121106            //add by pengyuangui 2012/11/06
#define AEECLSID_MOBILETRACKER      0x20131313            //add by yangdecai 2012/12/29
#if defined(FEATURE_DISP_128X160)&& defined(FEATURE_QQ_APP)
#define AEECLSID_QQ                 0x0109A277 //0x0109A277            //add by pengyuangui 2013/08/07
#elif defined(FEATURE_DISP_128X128)&& defined(FEATURE_QQ_APP)
#define AEECLSID_QQ                 0x0109A277 //0x0109A277            //add by pengyuangui 2013/08/07
#else
#define AEECLSID_QQ                 0x0109EB13 //0x0109A277            //add by pengyuangui 2013/08/07
#endif

#define AEECLSID_BLUETOOTH_APP    	0x01015223
#define AEECLSID_NF3				0x01007016
#define AEECLSID_TETRIS             0x01007239
#define AEECLSID_PINTU              0x01007241
#define AEECLSID_FIVEBALL           0x01007242
#define AEECLSID_GMFIVE             0x01007133

#if defined(FEATURE_VERSION_FLEXI203P)
#define AEECLSID_TWTTER             0x09090000
#define AEECLSID_APPMORA            0x010BB7D5
#define AEECLSID_FRAME              0x11111121
#define AEECLSID_FTP005             0x19882401
#define AEECLSID_FLEXIKOMIK         0x11111114
#define AEECLSID_FLEXIIKLAN         0x10000000
#define AEECLSID_FGEN               0x01009279
#define AEECLSID_EFSBROWSER         0x20000013
#define AEECLSID_CHATTING           0x01007262
#define AEECLSID_FACEBOOK           0x20000010
#endif
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


