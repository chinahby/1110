#ifndef AEEUSRSYSCLOCK_H
#define AEEUSRSYSCLOCK_H
/*======================================================
FILE:  AEEUsrSysClock.h

SERVICES:  Phone user clock functions.

GENERAL DESCRIPTION:
   Base level definitions, typedefs, etc. for IUsrSysClock

   Can be built as a static extension or a dynamic module

        Copyright © 2005 - 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

/*===========================================================================
                        INCLUDES
===========================================================================*/

#include "AEE.h"

#include "AEESysClock.h"
#include "AEEQueryInterface.h"
#include "AEE64structs.h"

#define  AEEIID_USRSYSCLOCK    	0x0103ca10


/**
 * List of supported time zones.
 * This should really not be a list of array indicies. Too much room 
 * for error. The timezone DST rules should be an array of structures that 
 * gets searched for a matching define. These should be refactored. The groups
 * could then exist in ranges of reserved values, allowing for easier updates.
 */

#define tzInvalid           0

#define  tzTimeZoneFirst    1

#define  tzGMTminus1100     1       //  Midway Islands

#define  tzGMTminus1000_1   2       //  Adak
#define  tzGMTminus1000     3       //  Fakaofo

#define  tzGMTminus0930     4       //  Taiohae

#define  tzGMTminus0900_1   5       //  Anchorage
#define  tzGMTminus0900     6       //  Gambier Islands

#define  tzGMTminus0800_1   7       //  Los Angeles
#define  tzGMTminus0800     8       //  Adamstown

#define  tzGMTminus0700_1   9       //  Denver
#define  tzGMTminus0700     10      //  Phoenix

#define  tzGMTminus0600_1   11      //  Easter Island 
#define  tzGMTminus0600_2   12      //  Chicago
#define  tzGMTminus0600_3   13      //  Winnipeg
#define  tzGMTminus0600     14      //  Guatemala

#define  tzGMTminus0500_1   15      //  Nassau
#define  tzGMTminus0500_2   16      //  Havana
#define  tzGMTminus0500_3   17      //  Grand Turk
#define  tzGMTminus0500_4   18      //  New York
#define  tzGMTminus0500     19      //  Bogota

#define  tzGMTminus0400_1   20      //  Asuncion  
#define  tzGMTminus0400_2   21      //  Stanley
#define  tzGMTminus0400_3   22      //  Campo Grande
#define  tzGMTminus0400_4   23      //  Santiago
#define  tzGMTminus0400_5   24      //  Bermuda
#define  tzGMTminus0400     25      //  Caracas

#define  tzGMTminus0330_1   26      //  St John's  

#define  tzGMTminus0300_1   27      //  Rio de Janerio  
#define  tzGMTminus0300_2   28      //  Nuuk
#define  tzGMTminus0300     29      //  Buenos Aires  

#define  tzGMTminus0200     30      //  Fernando de Noronha

#define  tzGMTminus0100_1   31      //  Azores
#define  tzGMTminus0100     32      //  Praia

#define  tzGMTminus0000_1   33      //  London
#define  tzGMTminus0000     34      //  Reykjavik

#define  tzGMTplus0100_1    35      //  Windhoek
#define  tzGMTplus0100_2    36      //  Paris
#define  tzGMTplus0100      37      //  Tunis

#define  tzGMTplus0200_1    38      //  Gaza
#define  tzGMTplus0200_2    39      //  Jerusalem
#define  tzGMTplus0200_3    40      //  Athens
#define  tzGMTplus0200_5    41      //  Amman
#define  tzGMTplus0200_6    42      //  Damascus
#define  tzGMTplus0200_7    43      //  Cairo
#define  tzGMTplus0200      44      //  Tripoli

#define  tzGMTplus0300_1    45      //  Moscow
#define  tzGMTplus0300_2    46      //  Baghdad
#define  tzGMTplus0300_3    47      //  Tbilisi
#define  tzGMTplus0300      48      //  Nairobi

#define  tzGMTplus0330_1    49      //  Tehran

#define  tzGMTplus0400_1    50      //  Baku
#define  tzGMTplus0400      51      //  Port Louis

#define  tzGMTplus0430      52      //  Kabul  

#define  tzGMTplus0500_1    53      //  Aqtobe
#define  tzGMTplus0500_2    54      //  Islamabad, Dushanbe, Male

#define  tzGMTplus0530      55      //  New Delhi

#define  tzGMTplus0545      56      //  Katmandu

#define  tzGMTplus0600_1    57      //  Astana
#define  tzGMTplus0600      58      //  Colombo

#define  tzGMTplus0630      59      //  Rangoon

#define  tzGMTplus0700_1    60      //  Hovd
#define  tzGMTplus0700_2    61      //  Krasnoyarsk
#define  tzGMTplus0700      62      //  Jakarta

#define  tzGMTplus0800_1    63      //  Irktusk
#define  tzGMTplus0800      64      //  Hong Kong

#define  tzGMTplus0900      65      //  Tokyo

#define  tzGMTplus0930_1    66      //  Adelaide
#define  tzGMTplus0930      67      //  Alice Springs

#define  tzGMTplus1000_1    68      //  Hobart
#define  tzGMTplus1000_2    69      //  Sydney
#define  tzGMTplus1000_3    70      //  Vladivostok
#define  tzGMTplus1000      71      //  Brisbane

#define  tzGMTplus1030      72      //  Lord Howe Island

#define  tzGMTplus1100_1    73      //  Magadan
#define  tzGMTplus1100      74      //  Port-Vila

#define  tzGMTplus1130      75      //  Kingston

#define  tzGMTplus1200_1    76      //  Wellington
#define  tzGMTplus1200_2    77      //  Kamchatka
#define  tzGMTplus1200      78      //  Suva

#define  tzGMTplus1245_1    79      //  Chatham Island

#define  tzGMTplus1300_1    80      //  Rawaki

#define  tzGMTplus1400      81      //  Kiritmati

#define  tzTimeZoneLast     81

typedef  uint32 AEETimeZoneType;



/*===========================================================================

                        IUsrSysClock Interface

===========================================================================*/
typedef struct IUsrSysClock IUsrSysClock;

#define INHERIT_IUsrSysClock(iname) \
   INHERIT_IQueryInterface(iname); \
   int (*GetTimeUS)          (iname* me, uint64struct* pqwUS); \
   int (*SetTimeUS)          (iname* me, uint64struct qwUS); \
   int (*GetTimeZone)(iname * po,AEETimeZoneType* peTimeZone);\
   int (*SetTimeZone)(iname * po,AEETimeZoneType eTimeZone);\
   int (*GetTimeS)(iname * po,uint32 * pdwSeconds );\
   int (*SetTimeS)(iname * po,uint32 dwSeconds );\
   int (*GetNetworkTimeZone)(iname * po,boolean * pbIsAvailable,int32 * pdwLocalTimeOffset,boolean * pbDaylightSavings);\
   int (*SetNetworkTimeZone)(iname * po,boolean bIsAvailable,int32 dwLocalTimeOffset,boolean bDaylightSavings);\
   int (*GetTimeOffset)(iname * po,uint32 dwUTCTimeInSeconds,AEETimeZoneType eTimeZone,int32* pOffset,boolean* pbDaylightSavings);\
   uint32 (*ConvertToLocalTime)(iname * po,uint32 dwUTCTimeInSeconds);\
   uint32 (*ConvertToUTCTime)(iname * po,uint32 dwLocalTimeInSeconds);\
   int (*ConvertOffsetToTimeZone)(iname * po,int32 dwOffset,uint32 dwSecsDaylightAdj,boolean bHasDaylightSavings, AEETimeZoneType *peTimeZone);\
   boolean (*IsTimeSet)(iname * po)

AEEINTERFACE(IUsrSysClock)
{
  INHERIT_IUsrSysClock(IUsrSysClock);
};


/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION IUSRSYSCLOCK_AddRef

DESCRIPTION
  Increment reference count for this instance of IUsrSysClock.

DEPENDENCIES
  None

PROTOTYPE
  int IUSRSYSCLOCK_AddRef(IUsrSysClock * po)

PARAMETERS
  po                 [In]- Pointer to IUsrSysClock object

RETURN VALUE
   Updated reference count

COMMENTS
  None

SIDE EFFECTS
  None
===========================================================================*/
static __inline int IUSRSYSCLOCK_AddRef(IUsrSysClock * po)
{
   return AEEGETPVTBL((po),IUsrSysClock)->AddRef(po);
}

/*===========================================================================

FUNCTION IUSRSYSCLOCK_Release

DESCRIPTION
  Release IUsrSysClock object and its members.

DEPENDENCIES
  None

PROTOTYPE
  int IUSRSYSCLOCK_Release(IUsrSysClock * po)

PARAMETERS
  po                 [In]- Pointer to IUsrSysClock object

RETURN VALUE
  Reference count of IUsrSysClock instance remaining from the po pointer.

COMMENTS
  Only frees the data associated with IUsrSysClock if the reference 
  count hits 0.

SIDE EFFECTS
  None
===========================================================================*/
static __inline int IUSRSYSCLOCK_Release(IUsrSysClock * po)
{
   return AEEGETPVTBL((po),IUsrSysClock)->Release(po);
}


/*===========================================================================

FUNCTION IUSRSYSCLOCK_QueryInterface

DESCRIPTION
  Query for an interface of IUsrSysClock.

DEPENDENCIES
  None

PROTOTYPE
  int IUSRSYSCLOCK_QueryInterface(IUsrSysClock *po, AEECLSID id,void **ppo)

PARAMETERS
  po                         [In] - Pointer to IUsrSysClock instance
  id                         [In] - Interface ID
  ppo                        [Out]- Pointer to queried object

RETURN VALUE
  EBADPARM, SUCCESS or ECLASSNOTSUPPORT

COMMENTS
  None

SIDE EFFECTS
  None
===========================================================================*/
static __inline int IUSRSYSCLOCK_QueryInterface(IUsrSysClock *po, 
                                                AEECLSID id,
                                                void **ppo)
{
   return AEEGETPVTBL((po),IUsrSysClock)->QueryInterface(po,id,ppo);
}


/*===========================================================================

FUNCTION IUSRSYSCLOCK_GetTimeUS

DESCRIPTION
  This interface is used to get the time in microseconds since midnight
  Jan 6, 1980 GMT from the phone's clock.

DEPENDENCIES
  None

PROTOTYPE
  int IUSRSYSCLOCK_GetTimeUS(IUsrSysClock * po, uint64struct * pqwUS )

PARAMETERS
  po                  [In] - Pointer to IUsrSysClock object
  pqwUS               [Out]- Pointer to quad word struct for microseconds

RETURN VALUE
  SUCCESS   Successfully got the time
  FAILED    Failed to get the time

COMMENTS
  The phone's clock is automatically set to 1/6/1980 upon first-time
  initialization.  If the return value from this function returns a
  date in the year 1980, it may be because the phone's clock was never
  properly set.

SIDE EFFECTS
  None
===========================================================================*/
static __inline int IUSRSYSCLOCK_GetTimeUS(IUsrSysClock * po, uint64struct * pqwUS )
{
   return AEEGETPVTBL((po),IUsrSysClock)->GetTimeUS(po, pqwUS);
}

/*===========================================================================

FUNCTION IUSRSYSCLOCK_SetTimeUS

DESCRIPTION
  This interface is used to set the time in microseconds since midnight
  Jan 6, 1980 GMT from the phone's clock.

DEPENDENCIES
  None

PROTOTYPE
  int IUSRSYSCLOCK_SetTimeUS(IUsrSysClock * po, uint64struct qwUS)

PARAMETERS
  po                [In]- Pointer to IUsrSysClock object
  qwUS              [In]- UTC time in microseconds to set


RETURN VALUE
  SUCCESS      Successfully set the time
  EUNSUPPORTED Invalid parameters
  EFAILED      Failed to set the clock

COMMENTS
  None

SIDE EFFECTS
  None
===========================================================================*/
static __inline int IUSRSYSCLOCK_SetTimeUS(IUsrSysClock * po, uint64struct qwUS)
{
   return AEEGETPVTBL((po),IUsrSysClock)->SetTimeUS(po, qwUS);
}

/*===========================================================================

FUNCTION IUSRSYSCLOCK_GetTimeS

DESCRIPTION
  This interface is used to get the time in seconds since midnight
  Jan 6, 1980 GMT from the phone's clock.

DEPENDENCIES
  None

PROTOTYPE
  int IUSRSYSCLOCK_GetTimeS(IUsrSysClock * po, uint32 * pdwUS )

PARAMETERS
  po                  [In] - Pointer to IUsrSysClock object
  pdwUS               [Out]- Pointer to uint32 for seconds
                             since 1/6/80 00:00:00 GMT

RETURN VALUE
  SUCCESS or EFAILED

COMMENTS
  The phone's clock is automatically set to 1/6/1980 upon first-time
  initialization.  If the return value from this function returns a
  date in the year 1980, it may be because the phone's clock was never
  properly set.

SIDE EFFECTS
  None
===========================================================================*/
static __inline int IUSRSYSCLOCK_GetTimeS(IUsrSysClock * po, uint32 * pdwSeconds )
{
   return AEEGETPVTBL((po),IUsrSysClock)->GetTimeS(po, pdwSeconds);
}

/*===========================================================================

FUNCTION IUSRSYSCLOCK_SetTimeS

DESCRIPTION
  This interface is used to set the time in seconds since midnight
  Jan 6, 1980 GMT on the phone's clock.

DEPENDENCIES
  None

PROTOTYPE
  int IUSRSYSCLOCK_SetTimeUS(IUsrSysClock * po, uint32 dwSeconds)

PARAMETERS
  po                         [In]- Pointer to IUsrSysClock object
  dwSeconds                  [In]- UTC time in seconds to set

RETURN VALUE
  SUCCESS      Successfully set the time
  EUNSUPPORTED Invalid parameters
  EFAILED      Failed to set the clock

COMMENTS
  None

SIDE EFFECTS
  None
===========================================================================*/
static __inline int IUSRSYSCLOCK_SetTimeS(IUsrSysClock * po, uint32 dwSeconds)
{
   return AEEGETPVTBL((po),IUsrSysClock)->SetTimeS(po, dwSeconds);
}


/*===========================================================================

FUNCTION IUSRSYSCLOCK_GetTimeZone

DESCRIPTION
  This interface is used to get the current system time zone.

DEPENDENCIES
  None

PROTOTYPE
  int IUSRSYSCLOCK_GetTimeZone(IUsrSysClock * po, AEETimeZoneType* peTimeZone)

PARAMETERS
  po                      [In] - Pointer to IUsrSysClock object
  peTimeZone              [Out]- Pointer to current system time zone


RETURN VALUE
  SUCCESS      Successfully got the current system time zone.
  EBADPARM     Invalid parameters

COMMENTS
  None

SIDE EFFECTS
  None
===========================================================================*/
static __inline int IUSRSYSCLOCK_GetTimeZone(IUsrSysClock * po, AEETimeZoneType* peTimeZone)
{
   return AEEGETPVTBL((po),IUsrSysClock)->GetTimeZone(po, peTimeZone);
}


/*===========================================================================

FUNCTION IUSRSYSCLOCK_SetTimeZone

DESCRIPTION
  This interface is used to set the user time zone.

DEPENDENCIES
  None

PROTOTYPE
  int IUSRSYSCLOCK_SetTimeZone(IUsrSysClock * po, AEETimeZoneType peTimeZone)

PARAMETERS  
  po               [In]- Pointer to IUsrSysClock object
  eTimeZone        [In]- TimeZone to set

RETURN VALUE
  SUCCESS      Successfully set the system time zone.
  EBADPARM     Invalid parameters
  EFAILED

COMMENTS
  None 

SIDE EFFECTS
  None
===========================================================================*/
static __inline int IUSRSYSCLOCK_SetTimeZone(IUsrSysClock * po, AEETimeZoneType peTimeZone)
{
   return AEEGETPVTBL((po),IUsrSysClock)->SetTimeZone(po, peTimeZone);
}


/*===========================================================================

FUNCTION IUSRSYSCLOCK_GetNetworkTimeZone

DESCRIPTION
  This interface is used to get the network time zone. This time-zone is distinct from the
  system time zone (set by the user).  When the "auto time" option is enabled, the network
  time zone should be updated from information available in the NITZ message.
DEPENDENCIES
  None

PROTOTYPE
  int IUSRSYSCLOCK_GetNetworkTimeZone(IUsrSysClock * po, 
                                      boolean * pbIsAvailable,
                                      int32 * pdwLocalTimeOffset, 
                                      boolean * pbDaylightSavings)

PARAMETERS
  po                        [In] - Pointer to IUsrSysClock object
  pbIsAvailable             [Out]- Indicates if network time zone information is available.
  pdwLocalTimeOffset        [Out]- Offset in seconds added to UTC to get local time,
                                   includes DST adjustment.
  pbDaylightSavings         [Out]- Indicates if DST is active.

RETURN VALUE
  SUCCESS      Successfully got the network time zone.
  EBADPARM     Invalid parameters
  EFAILED      Network time zone unavailable.

COMMENTS
  None

SIDE EFFECTS
  None
===========================================================================*/
static __inline int IUSRSYSCLOCK_GetNetworkTimeZone(IUsrSysClock * po, 
													boolean * pbIsAvailable,
													int32 * pdwLocalTimeOffset, 
													boolean * pbDaylightSavings)
{
   return AEEGETPVTBL((po),IUsrSysClock)->GetNetworkTimeZone(po, 
	                                                         pbIsAvailable,
	                                                         pdwLocalTimeOffset,
															 pbDaylightSavings);
}


/*===========================================================================

FUNCTION IUSRSYSCLOCK_SetNetworkTimeZone

DESCRIPTION
  This interface is used to set the network time zone. This time-zone is distinct from the
  system time zone (set by the user).  When the "auto time" option is enabled, the network
  time zone should be updated from information available in the NITZ message.  If
  bIsAvailable is FALSE, then the remaining parameters are ignored.

DEPENDENCIES
  None

PROTOTYPE
  int IUSRSYSCLOCK_SetNetworkTimeZone(IUsrSysClock * po, 
                                      boolean bIsAvailable,
                                      int32 dwLocalTimeOffset, 
                                      boolean bDaylightSavings)

PARAMETERS
  po                         [In]- Pointer to IUsrSysClock object
  bIsAvailable               [In]- Indicates if network time zone information is available.
  dwLocalTimeOffset          [In]- Offset in seconds added to UTC to get local time,
                                   includes DST adjustment.
  bDaylightSavings           [In]- Indicates if DST is active.

RETURN VALUE
  SUCCESS      Successfully set the network time zone.
  EBADPARM     Invalid parameters
  EFAILED

COMMENTS
  None

SIDE EFFECTS
  None
===========================================================================*/

static __inline int IUSRSYSCLOCK_SetNetworkTimeZone(IUsrSysClock * po, 
													boolean bIsAvailable,
													int32 dwLocalTimeOffset, 
													boolean bDaylightSavings)
{
   return AEEGETPVTBL((po),IUsrSysClock)->SetNetworkTimeZone(po, 
	                                                         bIsAvailable,
	                                                         dwLocalTimeOffset,
															 bDaylightSavings);
}


/*===========================================================================

FUNCTION IUSRSYSCLOCK_GetTimeOffset

DESCRIPTION
  This interface is used to get the offset for a given time in UTC seconds at a particular time zone. The offset
  is *added* to UTC time to get the local time in that time zone. Optionally returns whether daylight
  savings is active.

DEPENDENCIES
  None

PROTOTYPE
  int IUSRSYSCLOCK_GetTimeOffset(IUsrSysClock * po, 
                                 uint32 dwUTCTimeInSeconds, 
                                 AEETimeZoneType eTimeZone,
                                 int32* pOffset, 
                                 boolean* pbDaylightSavings)

PARAMETERS
  po                        [In] - Pointer to IUsrSysClock object
  dwUTCTimeInSeconds        [In] - UTC time in seconds
  eTimeZone                 [In] - Time zone to use for evaluation
  pOffset                   [Out]- Pointer to offset that is added to UTC to get local time
  pbDaylightSavings         [Out]- Optional pointer to boolean to indicate whether daylight savings is active


RETURN VALUE
  SUCCESS      Successfully got the offset
  EBADPARM     Invalid parameters
  EFAILED

COMMENTS
  pbDaylightSavings is permitted to be NULL.

SIDE EFFECTS
  None
===========================================================================*/

static __inline int IUSRSYSCLOCK_GetTimeOffset(IUsrSysClock * po, 
											   uint32 dwUTCTimeInSeconds, 
											   AEETimeZoneType eTimeZone,
											   int32* pOffset, 
											   boolean* pbDaylightSavings)
{
   return AEEGETPVTBL((po),IUsrSysClock)->GetTimeOffset(po, 
	                                                    dwUTCTimeInSeconds, 
														eTimeZone, 
	                                                    pOffset, 
														pbDaylightSavings);
}


/*===========================================================================

FUNCTION IUSRSYSCLOCK_ConvertToLocalTime

DESCRIPTION
  This interface is used to convert UTC time in seconds to the local time in seconds
  based on the current user time zone.

DEPENDENCIES
  None

PROTOTYPE
  uint32 IUSRSYSCLOCK_ConvertToLocalTime(IUsrSysClock * po, 
                                         uint32 dwUTCTimeInSeconds)
 
PARAMETERS
  po                         [In]- Pointer to IUsrSysClock object
  dwUTCTimeInSeconds         [In]- UTC time in seconds

RETURN VALUE
  Local time in seconds

COMMENTS
  None

SIDE EFFECTS
  None
===========================================================================*/

static __inline uint32 IUSRSYSCLOCK_ConvertToLocalTime(IUsrSysClock * po, 
                                                       uint32 dwUTCTimeInSeconds)
{
   return AEEGETPVTBL((po),IUsrSysClock)->ConvertToLocalTime(po, dwUTCTimeInSeconds);
}


/*===========================================================================

FUNCTION IUSRSYSCLOCK_ConvertToUTCTime

DESCRIPTION
  This interface is used to convert local time in seconds to UTC time in seconds
  based on the current system time zone.

DEPENDENCIES
  None

PROTOTYPE
uint32 IUSRSYSCLOCK_ConvertToUTCTime(IUsrSysClock * po, 
                                     uint32 dwLocalTimeInSeconds)

PARAMETERS
  po                         [In]- Pointer to IUsrSysClock object
  dwLocalTimeInSeconds       [In]- Local time in seconds

RETURN VALUE
  UTC time in seconds

COMMENTS
  If the given local time specifies a time during the missing hour when DST
  begins, then the returned UTC time will be converted to a legal time
  prior to DST starting. Example: DST starts at 2:00 with a shift of 1
  hour, then if the local time is entered such as 2:10 (not a legal time),
  then the returned UTC time will correspond to 1:10 with no DST. The next
  legal time is 3:00, at which point DST is active.

  If the given local time specifies a time during the compounded hour
  when DST ends, then the returned UTC time will correspond to the hour
  *prior* to DST ending.  Example: DST ends at 2:00 with a shift of 1 hour,
  then if the local time is entered anywhere between 1:00 and 1:59 it
  corresponds only with DST being active. If the time entered is 2:00, then
  DST is now inactive.

SIDE EFFECTS
  None
===========================================================================*/

static __inline uint32 IUSRSYSCLOCK_ConvertToUTCTime(IUsrSysClock * po, 
                                                     uint32 dwLocalTimeInSeconds)
{
   return AEEGETPVTBL((po),IUsrSysClock)->ConvertToUTCTime(po, dwLocalTimeInSeconds);
}


/*===========================================================================

FUNCTION IUSRSYSCLOCK_ConvertOffsetToTimeZone

DESCRIPTION
  This interface is used to convert an offset (that includies adjustments for daylight savings time)
  in seconds (local time) to an AEETimeZoneType.  The bHasDaylightSavings parameter indicates
  whether or not the time-zone performs daylight savings adjustments.  The dwSecsDaylightAdj
  parameter indicates the number of seconds used for daylight savings adjustment in the offset.
  Note: if the time-zone supports daylight savings, but there is no offset currently, then the
  dwSecsDaylightAdj should be 0 and bHasDaylightSavings should be TRUE.

  If it is not known whether or not the time-zone uses daylight savings adjmustments, then set
  bHasDaylightSavings to TRUE and dwSecsDaylightAdj to 0.  This function will still return the
  proper time-zone.

  If more than one time-zone correspond to the given offset/daylight-savings pair, then one
  will be picked arbitrarily from the set.

DEPENDENCIES
  None

PROTOTYPE
  int IUSRSYSCLOCK_ConvertOffsetToTimeZone(IUsrSysClock * po, 
                                           int32 dwOffset,
                                           uint32 dwSecsDaylightAdj, 
                                           AEETimeZoneType *peTimeZone)

PARAMETERS
  po                         [In] - Pointer to IUsrSysClock object
  dwOffset                   [In] - Offset added to UTC in seconds to get local time
  dwSecsDaylightAdj          [In] - Daylight savings adjustment in seconds
  bHasDaylightSavings        [In] - Whether or not time-zone performs daylight savings change
  peTimeZone                 [Out]- Time-zone that corresponds with offset

RETURN VALUE
  SUCCESS      Found a time-zone for the offset.
  EFAILED      No time-zone found.

COMMENTS
  None

SIDE EFFECTS
  None
===========================================================================*/

static __inline int IUSRSYSCLOCK_ConvertOffsetToTimeZone(IUsrSysClock * po, 
														 int32 dwOffset,
														 uint32 dwSecsDaylightAdj, 
														 boolean bHasDaylightSavings, 
														 AEETimeZoneType *peTimeZone)
{
   return AEEGETPVTBL((po),IUsrSysClock)->ConvertOffsetToTimeZone(po, 
	                                                              dwOffset,
	                                                              dwSecsDaylightAdj,
																  bHasDaylightSavings,
																  peTimeZone);
}


/*===========================================================================

FUNCTION IUSRSYSCLOCK_IsTimeSet

DESCRIPTION
  Return TRUE if the user clock has been set, FALSE otherwise. Note that is the
  time has never been set, the initial value will be midnight Jan 6, 1980 GMT.

DEPENDENCIES
  None

PROTOTYPE
  boolean IUSRSYSCLOCK_IsTimeSet(IUsrSysClock * po)


PARAMETERS
   po                [In]- Pointer to IUsrSysClock object

RETURN VALUE
  TRUE         Time has been set.
  FALSE        Time has not been set.

COMMENTS
  None

SIDE EFFECTS
  None
===========================================================================*/

static __inline boolean IUSRSYSCLOCK_IsTimeSet(IUsrSysClock * po)
{
   return AEEGETPVTBL((po),IUsrSysClock)->IsTimeSet(po);
}



#endif  // AEEUSRSYSCLOCK_H
