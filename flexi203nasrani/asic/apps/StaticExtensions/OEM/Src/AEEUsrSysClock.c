/*=============================================================================

FILE: AEEUsrSysClock.c

SERVICES: Implementation of IUsrSysClock class.

GENERAL DESCRIPTION:
   Provides services to set the system time, get/set the system time zone,
   and perform conversion calculations between UTC and local time.

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2006 - 2007      QUALCOMM Incorporated.
                               All Rights Reserved.

                               QUALCOMM Proprietary
=============================================================================*/

/*===========================================================================

                    INCLUDE FILES FOR MODULE

===========================================================================*/

#include "BREWVersion.h"
#include "OEMFeatures.h"

#include "AEEComdef.h"
#include "AEEStdLib.h"
#include "AEE_OEM.h"
#include "AEE.h"

#ifdef FEATURE_USRSYSCLOCK

#include "AEESysClock.h"
#include "AEEUsrSysClock.h"
#include "AEESysClock.bid"
#include "USRSYSCLOCK.BID"

#include "clk.h"
#include "clkrtc.h"
#include "OEMHeap.h"
#include "AEEConfig.h"
#include "OEMCFGI.h"

/** The number of milliseconds per second */
#define MILLISEC_PER_SECOND        (1000)
/** The number of milliseconds per minute */
#define MILLISEC_PER_MINUTE       (60000)
/** The number of milliseconds per hour */
#define MILLISEC_PER_HOUR       (3600000)
/** The number of milliseconds per day */
#define MILLISEC_PER_DAY       (86400000)

#define ONE_MINUTE MILLISEC_PER_MINUTE
#define ONE_HOUR MILLISEC_PER_HOUR
#define ONE_DAY MILLISEC_PER_DAY

#define NUM_OF_TIMEZONES (tzTimeZoneLast+1)

/** Macros to help define UTC offsets */
#define PLUS_OFFSET( hours, minutes )       (((hours) * ONE_HOUR) + ((minutes) * ONE_MINUTE))
#define MINUS_OFFSET( hours, minutes )      (-(PLUS_OFFSET( (hours), (minutes) )))

/** Macro used to define a TimeZone rule with no daylight savings. */
#define TZ_NO_DST(id, offset) \
  { \
      (id), \
      FALSE, \
      (offset), \
      FALSE, \
      ONE_HOUR, \
      0, \
      0, \
      0, \
      0, \
      WALL_TIME, \
      0, \
      0, \
      0, \
      0, \
      WALL_TIME, \
      0, \
      DOM_MODE, \
      DOM_MODE \
  }

/** Macro used to define a TimeZone rule with daylight savings, the time transitions
      shall follow the WALL clock. */
#define TZ_SET( id, offset, startMonth, startDay, \
                          startDOW, startTime, endMonth, endDay, \
                          endDOW, endTime ) \
  { \
      (id), \
      FALSE, \
      (offset), \
      TRUE, \
      ONE_HOUR, \
      (startMonth), \
      (startDay), \
      (startDOW), \
      (startTime), \
      WALL_TIME, \
      (endMonth), \
      (endDay), \
      (endDOW), \
      (endTime), \
      WALL_TIME, \
      0, \
      DOM_MODE, \
      DOM_MODE \
  }

/** Macro used to define a TimeZone rule with daylight savings, the time transitions
      shall follow the UTC time. */
#define TZ_SET2( id, offset, startMonth, startDay, \
                          startDOW, startTime, endMonth, endDay, \
                          endDOW, endTime ) \
  { \
      (id), \
      FALSE, \
      (offset), \
      TRUE, \
      ONE_HOUR, \
      (startMonth), \
      (startDay), \
      (startDOW), \
      (startTime), \
      UTC_TIME, \
      (endMonth), \
      (endDay), \
      (endDOW), \
      (endTime), \
      UTC_TIME, \
      0, \
      DOM_MODE, \
      DOM_MODE \
  }

/** Macro used to define a TimeZone rule with daylight savings, the time transitions
      shall follow the STANDARD time. */
#define TZ_SET3( id, offset, startMonth, startDay, \
                          startDOW, startTime, endMonth, endDay, \
                          endDOW, endTime ) \
  { \
      (id), \
      FALSE, \
      (offset), \
      TRUE, \
      ONE_HOUR, \
      (startMonth), \
      (startDay), \
      (startDOW), \
      (startTime), \
      STANDARD_TIME, \
      (endMonth), \
      (endDay), \
      (endDOW), \
      (endTime), \
      STANDARD_TIME, \
      0, \
      DOM_MODE, \
      DOM_MODE \
  }

/** Macro used to define a TimeZone rule with daylight savings shift of only 30 min.
*/
#define TZ_SET_HALFDST( id, offset, startMonth, startDay, \
                          startDOW, startTime, endMonth, endDay, \
                          endDOW, endTime ) \
  { \
      (id), \
      FALSE, \
      (offset), \
      TRUE, \
      30 * ONE_MINUTE, \
      (startMonth), \
      (startDay), \
      (startDOW), \
      (startTime), \
      WALL_TIME, \
      (endMonth), \
      (endDay), \
      (endDOW), \
      (endTime), \
      WALL_TIME, \
      0, \
      DOM_MODE, \
      DOM_MODE \
  }

#define U_FAILURE(x) ((x)> ZERO_ERROR)

#ifndef PARAM_NOT_REF
#define PARAM_NOT_REF(param)         /*lint -esym(715,param) */
#endif
/** 
 * Possible months in a UCalendar. Note: Calendar month is 0-based.
 */
enum UCalendarMonths {
  /** January */
  UCAL_JANUARY = 0,
  /** February */
  UCAL_FEBRUARY,
  /** March */
  UCAL_MARCH,
  /** April */
  UCAL_APRIL,
  /** May */
  UCAL_MAY,
  /** June */
  UCAL_JUNE,
  /** July */
  UCAL_JULY,
  /** August */
  UCAL_AUGUST,
  /** September */
  UCAL_SEPTEMBER,
  /** October */
  UCAL_OCTOBER,
  /** November */
  UCAL_NOVEMBER,
  /** December */
  UCAL_DECEMBER,
  /** Undecimber */
  UCAL_UNDECIMBER
};

/** 
 * Possible days of the week in a UCalendar.
 * Note: Calendar day-of-week is 1-based.
 */
enum UCalendarDaysOfWeek {
  /** Sunday */
  UCAL_SUNDAY = 1,
  /** Monday */
  UCAL_MONDAY,
  /** Tuesday */
  UCAL_TUESDAY,
  /** Wednesday */
  UCAL_WEDNESDAY,
  /** Thursday */
  UCAL_THURSDAY,
  /** Friday */
  UCAL_FRIDAY,
  /** Saturday */
  UCAL_SATURDAY
};

/**
 * Error Codes
 */
typedef enum UErrorCode {
    ZERO_ERROR              =  0,     /**< No error, no warning. */

    ILLEGAL_ARGUMENT_ERROR  =  1,     /**< Start of codes indicating failure */

    STANDARD_ERROR_LIMIT             /**< This must always be the last value to indicate the limit for standard errors */

} ErrorCode;

/** 
  * Specifies how the time should be interpreted.
  */
typedef enum TimeMode {
  WALL_TIME = 0,
  STANDARD_TIME,
  UTC_TIME
} TimeMode;

/** 
  * The method by which a particular time zone rule is analyzed.
  */
typedef enum EMode
{
  DOM_MODE = 1,
  DOW_IN_MONTH_MODE,
  DOW_GE_DOM_MODE,
  DOW_LE_DOM_MODE
} EMode;

/**
 * Structure to hold the rule for a particular time zone.
 */
typedef struct _TimeZoneRule
{
    AEETimeZoneType id;
   
    boolean isDecoded; // TRUE if rule is decoded.

    int32 rawOffset;  // the TimeZone's raw GMT offset

    boolean useDaylight; // flag indicating whether this TimeZone uses DST

    /**
     * A positive value indicating the amount of time saved during DST in ms.
     * Typically one hour; sometimes 30 minutes.
     */
    int32 dstSavings;

    int8 startMonth;   // the month, day, DOW, and time DST starts
    int8 startDay;
    int8 startDayOfWeek;
    int32 startTime;
    TimeMode startTimeMode;
    
    int8 endMonth;    // the month, day, DOW, and time DST ends
    int8 endDay;
    int8 endDayOfWeek; 
    int32 endTime;
    TimeMode endTimeMode; // Mode for startTime, endTime; see TimeMode

    int32 startYear;  // the year these DST rules took effect
    
    EMode startMode;
    EMode endMode;   // flags indicating what kind of rules the DST rules are

} TimeZoneRule;

/**
 * Structure to hold the network time zone information.
 */
typedef struct _NetworkTimeZoneInfo
{
   boolean isAvailable; // TRUE if network time zone is set.
   int32 tzOffset; // Includes adjustment for daylight savings.
   boolean isDST; // Indicates if daylight savings is active.
} NetworkTimeZoneInfo;


typedef struct IUsrSysClock
{
   const AEEVTBL(IUsrSysClock)  *m_pvt;
   AEECallback    m_cbExit;
   void           *m_pAContext;
   IShell *       m_pIShell;
   uint32         m_uRefs;
   AEECLSID       m_nCls;
   ISysClock      *m_pISysClock;  // Pointer to BREW ISysClock
} IUsrSysClock;



static uint32 AEEUsrSysClock_AddRef(IUsrSysClock *pMe);

static uint32 AEEUsrSysClock_Release(IUsrSysClock *pMe);

static int AEEUsrSysClock_QueryInterface(IUsrSysClock *pMe, 
                                         AEECLSID id, void **ppo);

static int AEEUsrSysClock_GetTimeUS(IUsrSysClock *pMe, 
                                    uint64struct* pqwUS);

static int AEEUsrSysClock_SetTimeUS(IUsrSysClock *pMe, 
                                    uint64struct qwUS);

static int AEEUsrSysClock_GetTimeZone(IUsrSysClock *pMe, 
                                      AEETimeZoneType *peTimeZone);

static int AEEUsrSysClock_SetTimeZone(IUsrSysClock *pMe, 
                                      AEETimeZoneType eTimeZone);

static int AEEUsrSysClock_GetTimeS(IUsrSysClock *pMe, 
                                   uint32* pdwSeconds);

static int AEEUsrSysClock_SetTimeS(IUsrSysClock *pMe, 
                                   uint32 dwSeconds);

static int AEEUsrSysClock_GetNetworkTimeZone(IUsrSysClock *pMe, 
                                             boolean *pbIsAvailable, 
                                             int32 *pdwLocalTimeOffset, 
                                             boolean *pbDaylightSavings);

static int AEEUsrSysClock_SetNetworkTimeZone(IUsrSysClock *po, 
                                             boolean bIsAvailable,
                                             int32 dwLocalTimeOffset, 
                                             boolean bDaylightSavings);

static int AEEUsrSysClock_GetTimeOffset(IUsrSysClock *pMe, 
                                        uint32 dwUTCTimeInSeconds, 
                                        AEETimeZoneType eTimeZone,
                                        int32 *pOffset, 
                                        boolean* pbDaylightSavings);

static uint32 AEEUsrSysClock_ConvertToLocalTime(IUsrSysClock * pMe, 
                                                uint32 dwUTCTimeInSeconds);

static uint32 AEEUsrSysClock_ConvertToUTCTime(IUsrSysClock * pMe, 
                                              uint32 dwLocalTimeInSeconds);

static int AEEUsrSysClock_ConvertOffsetToTimeZone(IUsrSysClock * pMe, 
                                                  int32 dwOffset,
                                                  uint32 dwSecsDaylightAdj, 
                                                  boolean bHasDaylightSavings, 
                                                  AEETimeZoneType *peTimeZone);

static boolean AEEUsrSysClock_IsTimeSet(IUsrSysClock * pMe);

// The virtual function table for IUsrSysClock.
static const AEEVTBL(IUsrSysClock) gvtIUsrSysClock =
{
  //  ISYSCLOCK functions
  AEEUsrSysClock_AddRef,
  AEEUsrSysClock_Release,
  AEEUsrSysClock_QueryInterface,
  AEEUsrSysClock_GetTimeUS,
  AEEUsrSysClock_SetTimeUS,
  // IUsrSysClock functions
  AEEUsrSysClock_GetTimeZone,
  AEEUsrSysClock_SetTimeZone,
  AEEUsrSysClock_GetTimeS,
  AEEUsrSysClock_SetTimeS,
  AEEUsrSysClock_GetNetworkTimeZone,
  AEEUsrSysClock_SetNetworkTimeZone,
  AEEUsrSysClock_GetTimeOffset,
  AEEUsrSysClock_ConvertToLocalTime,
  AEEUsrSysClock_ConvertToUTCTime,
  AEEUsrSysClock_ConvertOffsetToTimeZone,
  AEEUsrSysClock_IsTimeSet
};


#ifdef RELEASEIF
#undef RELEASEIF
#endif
#define RELEASEIF(p) if (p) {IQI_Release((IQueryInterface *)(p)); (p) = NULL;}


static IUsrSysClock *gpSysClock;

static AEETimeZoneType gTimeZone;

/**
 * List of month lengths, only used for argument checking. Leap years are handled
 * correctly during run-time.
 */
static const int8 STATICMONTHLENGTH[] = {31,29,31,30,31,30,31,31,30,31,30,31};

static NetworkTimeZoneInfo   gNetworkTimeZone =
{
   FALSE, // isAvailable
   0,     // tzOffset
   FALSE  // isDST
};

/**
 * Used for translating between Julian day format and UCAL day format.
 */
static const uint8 kJulianDay_To_UCalDay[] =
{
   UCAL_MONDAY,
   UCAL_TUESDAY,
   UCAL_WEDNESDAY,
   UCAL_THURSDAY,
   UCAL_FRIDAY,
   UCAL_SATURDAY,
   UCAL_SUNDAY
};

/**
 * The timezone rules.
 * As they stand, the timezone rules are accessed by array indicies.  This 
 * presents room for error. The timezone DST rules should be searched 
 * for a matching define instead. The accesses to this table should be refactored.
 */
static TimeZoneRule gTimeZoneRules[NUM_OF_TIMEZONES] =
{
   // Indicates that no time zone is currently set. Default to UTC time.
   TZ_NO_DST(tzInvalid, 0),

    // Pacific/Apia W Samoa(WS) -11:00  -   WST # W Samoa Time
   TZ_NO_DST(tzGMTminus1100, MINUS_OFFSET(11, 0)),

    // Rule US  1967    max -   Oct lastSun 2:00    0   S
    // Rule US  1987    max -   Apr Sun>=1  2:00    1:00    D
    // Rule US  2007    max -   Nov Sun>=1  2:00    0   S
    // Rule US  2007    max -   Mar Sun>=8  2:00    1:00    D
    // America/Adak Alaska(US)  -10:00  US  HA%sT
   TZ_SET(tzGMTminus1000_1, MINUS_OFFSET(10, 0),
         UCAL_MARCH, 8, -UCAL_SUNDAY, 2*ONE_HOUR,
         UCAL_NOVEMBER, 1, -UCAL_SUNDAY, 2*ONE_HOUR),

    // Pacific/Fakaofo  Tokelau Is(TK)  -10:00  -   TKT # Tokelau Time
   TZ_NO_DST(tzGMTminus1000, MINUS_OFFSET(10, 0)),

   // Taiohae, could not find rule.
   TZ_NO_DST(tzGMTminus0930, MINUS_OFFSET(9, 30)),

    // Rule US  1967    max -   Oct lastSun 2:00    0   S
    // Rule US  1987    max -   Apr Sun>=1  2:00    1:00    D
    // Rule US  2007    max -   Nov Sun>=1  2:00    0   S
    // Rule US  2007    max -   Mar Sun>=8  2:00    1:00    D
    // America/Anchorage    Alaska(US)  -9:00   US  AK%sT
   TZ_SET(tzGMTminus0900_1, MINUS_OFFSET(9, 0),
         UCAL_MARCH, 8, -UCAL_SUNDAY, 2*ONE_HOUR,
         UCAL_NOVEMBER, 1, -UCAL_SUNDAY, 2*ONE_HOUR),

   //Zone    Pacific/Gambier    -9:00  -       GAMT    # Gambier Time
   TZ_NO_DST(tzGMTminus0900, MINUS_OFFSET(9, 0)),

    // Rule US  1967    max -   Oct lastSun 2:00    0   S
    // Rule US  1987    max -   Apr Sun>=1  2:00    1:00    D
    // Rule US  2007    max -   Nov Sun>=1  2:00    0   S
    // Rule US  2007    max -   Mar Sun>=8  2:00    1:00    D
    // America/Los_Angeles  US Pacific time, represented by Los Angeles(US) -8:00   US  P%sT
   TZ_SET(tzGMTminus0800_1, MINUS_OFFSET(8, 0),
         UCAL_MARCH, 8, -UCAL_SUNDAY, 2*ONE_HOUR,
         UCAL_NOVEMBER, 1, -UCAL_SUNDAY, 2*ONE_HOUR),

   // Zone Pacific/Pitcairn -8:00   -       PST     # Pitcairn Standard Time
   TZ_NO_DST(tzGMTminus0800, MINUS_OFFSET(8, 0)),

    // Rule US  1967    max -   Oct lastSun 2:00    0   S
    // Rule US  1987    max -   Apr Sun>=1  2:00    1:00    D
    // Rule US  2007    max -   Nov Sun>=1  2:00    0   S
    // Rule US  2007    max -   Mar Sun>=8  2:00    1:00    D
    // America/Denver   US Mountain time, represented by Denver(US) -7:00   US  M%sT
   TZ_SET(tzGMTminus0700_1, MINUS_OFFSET(7, 0),
         UCAL_MARCH, 8, -UCAL_SUNDAY, 2*ONE_HOUR,
         UCAL_NOVEMBER, 1, -UCAL_SUNDAY, 2*ONE_HOUR),

    // America/Phoenix  ?(US)   -7:00   -   MST
   TZ_NO_DST(tzGMTminus0700, MINUS_OFFSET(7, 0)),

    // Rule    Chile   1999    max     -       Oct     Sun>=9  4:00u   1:00    S
    // Rule    Chile   2000    max     -       Mar     Sun>=9  3:00u   0       -
    // Pacific/Easter   Chile(CL)   -6:00   Chile   EAS%sT
   TZ_SET2(tzGMTminus0600_1, MINUS_OFFSET(6, 0),
         UCAL_OCTOBER, 9, -UCAL_SUNDAY, 4*ONE_HOUR,
         UCAL_MARCH, 9, -UCAL_SUNDAY, 3*ONE_HOUR),

    // Rule US  1967    max -   Oct lastSun 2:00    0   S
    // Rule US  1987    max -   Apr Sun>=1  2:00    1:00    D
    // Rule US  2007    max -   Nov Sun>=1  2:00    0   S
    // Rule US  2007    max -   Mar Sun>=8  2:00    1:00    D
    // America/Chicago  US Central time, represented by Chicago(US) -6:00   US  C%sT
   TZ_SET(tzGMTminus0600_2, MINUS_OFFSET(6, 0),
         UCAL_MARCH, 8, -UCAL_SUNDAY, 2*ONE_HOUR,
         UCAL_NOVEMBER, 1, -UCAL_SUNDAY, 2*ONE_HOUR),

   // Rule    Winn    1987    max     -       Oct     lastSun 2:00s   0       S
   // Rule    Winn    1987    max     -       Apr     Sun>=1  2:00    1:00    D
   // America/Winnipeg Manitoba(CA)    -6:00   Winn    C%sT
   TZ_SET(tzGMTminus0600_3, MINUS_OFFSET(6, 0),
            UCAL_APRIL, 1, -UCAL_SUNDAY, 2*ONE_HOUR,
            UCAL_OCTOBER, -1, UCAL_SUNDAY, 3*ONE_HOUR),

    // America/Guatemala    Guatemala(GT)   -6:00   -   C%sT
   TZ_NO_DST(tzGMTminus0600, MINUS_OFFSET(6, 0)),

    // Rule Bahamas 1964    max -   Oct lastSun 2:00    0   S
    // Rule Bahamas 1987    max -   Apr Sun>=1  2:00    1:00    D
    // America/Nassau   Bahamas(BS) -5:00   Bahamas 
   TZ_SET(tzGMTminus0500_1, MINUS_OFFSET(5, 0),
            UCAL_APRIL, 1, -UCAL_SUNDAY, 2*ONE_HOUR,
            UCAL_OCTOBER, -1, UCAL_SUNDAY, 2*ONE_HOUR),

    // Rule    Cuba    2000    max     -       Apr     Sun>=1  0:00s   1:00    D
    // Rule    Cuba    2005    max     -       Oct     lastSun 0:00s   0       S
    // America/Havana   Cuba(CU)    -5:00   Cuba    C%sT
   TZ_SET3(tzGMTminus0500_2, MINUS_OFFSET(5, 0),
            UCAL_APRIL, 1, -UCAL_SUNDAY, 0*ONE_HOUR,
            UCAL_OCTOBER, -1, UCAL_SUNDAY, 0*ONE_HOUR),

    // Rule TC  1979    max -   Oct lastSun 0:00    0   S
    // Rule TC  1987    max -   Apr Sun>=1  0:00    1:00    D
    // America/Grand_Turk   Turks and Caicos(TC)    -5:00   TC  E%sT
   TZ_SET(tzGMTminus0500_3, MINUS_OFFSET(5, 0),
            UCAL_APRIL, 1, -UCAL_SUNDAY, 0*ONE_HOUR,
            UCAL_OCTOBER, -1, UCAL_SUNDAY, 0*ONE_HOUR),

    // Rule US  1967    max -   Oct lastSun 2:00    0   S
    // Rule US  1987    max -   Apr Sun>=1  2:00    1:00    D
    // Rule US  2007    max -   Nov Sun>=1  2:00    0   S
    // Rule US  2007    max -   Mar Sun>=8  2:00    1:00    D
    // America/New York  US Eastern time, represented by New York City(US) -5:00   US  E%sT
   TZ_SET(tzGMTminus0500_4, MINUS_OFFSET(6, 0),
         UCAL_MARCH, 8, -UCAL_SUNDAY, 2*ONE_HOUR,
         UCAL_NOVEMBER, 1, -UCAL_SUNDAY, 2*ONE_HOUR),

    // America/Bogota   Colombia(CO)    -5:00   -   CO%sT   # Colombia Time
   TZ_NO_DST(tzGMTminus0500, MINUS_OFFSET(5, 0)),

    // Rule    Para    2002    max     -       Apr     Sun>=1  0:00    0       -
    // Rule    Para    2002    max     -       Sep     Sun>=1  0:00    1:00    S
    // Zone America/Asuncion        -4:00   Para    PY%sT
   TZ_SET(tzGMTminus0400_1, MINUS_OFFSET(4, 0),
            UCAL_SEPTEMBER, 1, -UCAL_SUNDAY, 0*ONE_HOUR,
            UCAL_APRIL, 1, -UCAL_SUNDAY, 0*ONE_HOUR),

   // Rule    Falk    2001    max     -       Apr     Sun>=15 2:00    0       -
   // Rule    Falk    2001    max     -       Sep     Sun>=1  2:00    1:00    S
   // Atlantic/Stanley Falklands(FK)   -4:00   Falk    FK%sT
   TZ_SET(tzGMTminus0400_2, MINUS_OFFSET(4, 0),
            UCAL_SEPTEMBER, 1, -UCAL_SUNDAY, 2*ONE_HOUR,
            UCAL_APRIL, 15, -UCAL_SUNDAY, 2*ONE_HOUR),

   //Zone America/Campo_Grande -4:00   Brazil  AM%sT
   TZ_NO_DST(tzGMTminus0400_3, MINUS_OFFSET(4, 0)),

   // Rule    Chile   1999    max     -       Oct     Sun>=9  4:00u   1:00    S
   // Rule    Chile   2000    max     -       Mar     Sun>=9  3:00u   0       -
   // Zone America/Santiago   -4:00   Chile   CL%sT
   TZ_SET2(tzGMTminus0400_4, MINUS_OFFSET(4, 0),
         UCAL_OCTOBER, 9, -UCAL_SUNDAY, 4*ONE_HOUR,
         UCAL_MARCH, 9, -UCAL_SUNDAY, 3*ONE_HOUR),

   // Rule    Bahamas 1964    max     -       Oct     lastSun 2:00    0       S
   // Rule    Bahamas 1987    max     -       Apr     Sun>=1  2:00    1:00    D
   // Zone Atlantic/Bermuda  -4:00   Bahamas A%sT
   TZ_SET(tzGMTminus0400_5, MINUS_OFFSET(4, 0),
         UCAL_APRIL, 1, -UCAL_SUNDAY, 2*ONE_HOUR,
         UCAL_OCTOBER, -1, UCAL_SUNDAY, 2*ONE_HOUR),

   // Zone    America/Caracas  -4:00   -       VET
   TZ_NO_DST(tzGMTminus0400, MINUS_OFFSET(4, 0)),

   // Rule    Brazil  2001    max     -       Feb     Sun>=15  0:00   0       -
   // Rule    Brazil  2005    max     -       Oct     Sun>=15  0:00   1:00    S
   // Zone America/Sao_Paulo  -3:00   Brazil  BR%sT
   TZ_SET(tzGMTminus0300_1, MINUS_OFFSET(3, 0),
         UCAL_OCTOBER, 15, -UCAL_SUNDAY, 0*ONE_HOUR,
         UCAL_FEBRUARY, 15, -UCAL_SUNDAY, 0*ONE_HOUR),

   // Rule    EU      1981    max     -       Mar     lastSun  1:00u  1:00    S
   // Rule    EU      1996    max     -       Oct     lastSun  1:00u  0       -
   // Zone America/Godthab    -3:00   EU      WG%sT
   TZ_SET2(tzGMTminus0300_2, MINUS_OFFSET(3, 0),
         UCAL_MARCH, -1, UCAL_SUNDAY, 1*ONE_HOUR,
         UCAL_OCTOBER, -1, UCAL_SUNDAY, 1*ONE_HOUR),

   // Zone America/Argentina/Buenos_Aires -3:00   -       ART
   TZ_NO_DST(tzGMTminus0300, MINUS_OFFSET(3, 0)),

   // Rule    StJohns 1987    max     -       Oct     lastSun 0:01    0       S
   // Rule    StJohns 1989    max     -       Apr     Sun>=1  0:01    1:00    D
   // Zone America/St_Johns  -3:30   StJohns N%sT
   TZ_SET(tzGMTminus0330_1, MINUS_OFFSET(3, 30),
         UCAL_APRIL, 1, -UCAL_SUNDAY, 1*ONE_MINUTE,
         UCAL_OCTOBER, -1, UCAL_SUNDAY, 1*ONE_MINUTE),

   // Zone America/Noronha  -2:00   -       FNT
   TZ_NO_DST(tzGMTminus0200, MINUS_OFFSET(2, 0)),

   // Rule    EU      1981    max     -       Mar     lastSun  1:00u  1:00    S
   // Rule    EU      1996    max     -       Oct     lastSun  1:00u  0       -
   // Zone Atlantic/Azores    -1:00   EU      AZO%sT
   TZ_SET2(tzGMTminus0100_1, MINUS_OFFSET(1, 0),
         UCAL_MARCH, -1, UCAL_SUNDAY, 1*ONE_HOUR,
         UCAL_OCTOBER, -1, UCAL_SUNDAY, 1*ONE_HOUR),

   //Zone Atlantic/Cape_Verde  -1:00   -       CVT
   TZ_NO_DST(tzGMTminus0100, MINUS_OFFSET(1, 0)),

   // Rule    EU      1981    max     -       Mar     lastSun  1:00u  1:00    S
   // Rule    EU      1996    max     -       Oct     lastSun  1:00u  0       -
   // Zone    Europe/London   0:00   EU      GMT/BST
   TZ_SET2(tzGMTminus0000_1, MINUS_OFFSET(0, 0),
         UCAL_MARCH, -1, UCAL_SUNDAY, 1*ONE_HOUR,
         UCAL_OCTOBER, -1, UCAL_SUNDAY, 1*ONE_HOUR),

   // Zone Atlantic/Reykjavik  0:00   -       GMT
   TZ_NO_DST(tzGMTminus0000, MINUS_OFFSET(0, 0)),

   // Rule    Namibia 1994    max     -       Sep     Sun>=1  2:00    1:00    S
   // Rule    Namibia 1995    max     -       Apr     Sun>=1  2:00    0       -
   // Zone    Africa/Windhoek  1:00    Namibia WA%sT
   TZ_SET(tzGMTplus0100_1, PLUS_OFFSET(1, 0),
         UCAL_SEPTEMBER, 1, -UCAL_SUNDAY, 2*ONE_HOUR,
         UCAL_APRIL, 1, -UCAL_SUNDAY, 2*ONE_HOUR),

   // Rule    EU      1981    max     -       Mar     lastSun  1:00u  1:00    S
   // Rule    EU      1996    max     -       Oct     lastSun  1:00u  0       -
   // Zone    Europe/Paris   1:00    EU      CE%sT
   TZ_SET2(tzGMTplus0100_2, PLUS_OFFSET(1, 0),
         UCAL_MARCH, -1, UCAL_SUNDAY, 1*ONE_HOUR,
         UCAL_OCTOBER, -1, UCAL_SUNDAY, 1*ONE_HOUR),

   // Zone    Africa/Tunis   1:00    Tunisia CE%sT
   TZ_NO_DST(tzGMTplus0100, PLUS_OFFSET(1, 0)),

   // Rule Palestine  1999    max     -       Apr     Fri>=15 0:00    1:00    S
   // Rule Palestine  1999    max     -       Oct     Fri>=15 0:00    0       -
   // Zone    Asia/Gaza  2:00 Palestine  EE%sT
   TZ_SET(tzGMTplus0200_1, PLUS_OFFSET(2, 0),
         UCAL_APRIL, 15, -UCAL_FRIDAY, 0*ONE_HOUR,
         UCAL_OCTOBER, 15, -UCAL_FRIDAY, 0*ONE_HOUR),

   // Rule    Zion    2005    max     -       Apr      1      1:00    1:00    D
   // Rule    Zion    2005    max     -       Oct      1      1:00    0       S
   // Zone    Asia/Jerusalem  2:00    Zion    I%sT
   TZ_SET(tzGMTplus0200_2, PLUS_OFFSET(2, 0),
         UCAL_APRIL, 1, 0, 1*ONE_HOUR,
         UCAL_OCTOBER, 1, 0, 1*ONE_HOUR),

   // Rule    EU      1981    max     -       Mar     lastSun  1:00u  1:00    S
   // Rule    EU      1996    max     -       Oct     lastSun  1:00u  0       -
   // Zone    Europe/Athens  2:00    EU      EE%sT
   TZ_SET2(tzGMTplus0200_3, PLUS_OFFSET(2, 0),
         UCAL_MARCH, -1, UCAL_SUNDAY, 1*ONE_HOUR,
         UCAL_OCTOBER, -1, UCAL_SUNDAY, 1*ONE_HOUR),

   // Rule    Jordan  1999    max     -       Sep     lastThu 0:00s   0       -
   // Rule    Jordan  2000    max     -       Mar     lastThu 0:00s   1:00    S
   // Zone    Asia/Amman   2:00    Jordan  EE%sT
   TZ_SET3(tzGMTplus0200_5, PLUS_OFFSET(2, 0),
      UCAL_MARCH, -1, UCAL_THURSDAY, 0*ONE_HOUR,
      UCAL_SEPTEMBER, -1, UCAL_THURSDAY, 0*ONE_HOUR),

   // Rule    Syria   1999    max     -       Apr      1      0:00    1:00    S
   // Rule    Syria   1994    max     -       Oct      1      0:00    0       -
   // Zone    Asia/Damascus  2:00    Syria   EE%sT
   TZ_SET(tzGMTplus0200_6, PLUS_OFFSET(2, 0),
       UCAL_APRIL, 1, 0, 0*ONE_HOUR,
       UCAL_OCTOBER, 1, 0, 0*ONE_HOUR),

   // Rule    Egypt   1995    max     -       Apr     lastFri  0:00s  1:00    S
   // Rule    Egypt   1995    max     -       Sep     lastThu 23:00s  0       -
   // Zone    Africa/Cairo   2:00    Egypt   EE%sT
   TZ_SET3(tzGMTplus0200_7, PLUS_OFFSET(2, 0),
       UCAL_APRIL, -1, UCAL_FRIDAY, 0*ONE_HOUR,
       UCAL_SEPTEMBER, -1, UCAL_THURSDAY, 23*ONE_HOUR),
      
   // Zone    Africa/Tripoli  2:00    -       EET
   TZ_NO_DST(tzGMTplus0200, PLUS_OFFSET(2, 0)),

   // Rule    Russia  1993    max     -       Mar     lastSun  2:00s  1:00    S
   // Rule    Russia  1996    max     -       Oct     lastSun  2:00s  0       -
   // Zone Europe/Moscow     3:00   Russia  MSK/MSD
   TZ_SET3(tzGMTplus0300_1, PLUS_OFFSET(3, 0),
       UCAL_MARCH, -1, UCAL_SUNDAY, 2*ONE_HOUR,
       UCAL_OCTOBER, -1, UCAL_SUNDAY, 2*ONE_HOUR),

   // Rule    Iraq    1991    max     -       Apr      1      3:00s   1:00    D
   // Rule    Iraq    1991    max     -       Oct      1      3:00s   0       S
   // Zone    Asia/Baghdad  3:00    Iraq    A%sT
   TZ_SET3(tzGMTplus0300_2, PLUS_OFFSET(3, 0),
       UCAL_APRIL, 1, 0, 3*ONE_HOUR,
       UCAL_OCTOBER, 1, 0, 3*ONE_HOUR),

   // Rule RussiaAsia 1993    max     -       Mar     lastSun  2:00s  1:00    S
   // Rule RussiaAsia 1996    max     -       Oct     lastSun  2:00s  0       -
   // Zone    Asia/Tbilisi     3:00 RussiaAsia GE%sT
   TZ_SET3(tzGMTplus0300_3, PLUS_OFFSET(3, 0),
        UCAL_MARCH, -1, UCAL_SUNDAY, 2*ONE_HOUR,
        UCAL_OCTOBER, -1, UCAL_SUNDAY, 2*ONE_HOUR),

   // Zone    Africa/Nairobi  3:00    -       EAT
   TZ_NO_DST(tzGMTplus0300, PLUS_OFFSET(3, 0)),

   // Zone    Asia/Tehran  3:30    Iran    IR%sT
   // Rule    Iran    2005    2007    -       Mar     22      0:00    1:00    D
   // Rule    Iran    2005    2007    -       Sep     22      0:00    0       S
   // Rule    Iran    2008    only    -       Mar     21      0:00    1:00    D
   // Rule    Iran    2008    only    -       Sep     21      0:00    0       S
   // Rule    Iran    2009    2011    -       Mar     22      0:00    1:00    D
   // Rule    Iran    2009    2011    -       Sep     22      0:00    0       S
   // Rule    Iran    2012    only    -       Mar     21      0:00    1:00    D
   // Rule    Iran    2012    only    -       Sep     21      0:00    0       S
   // Rule    Iran    2013    2015    -       Mar     22      0:00    1:00    D
   // Rule    Iran    2013    2015    -       Sep     22      0:00    0       S
   // Rule    Iran    2016    only    -       Mar     21      0:00    1:00    D
   // Rule    Iran    2016    only    -       Sep     21      0:00    0       S
   // Rule    Iran    2017    2019    -       Mar     22      0:00    1:00    D
   // Rule    Iran    2017    2019    -       Sep     22      0:00    0       S
   // #       The official time of the country will should move forward one hour
   // #       at the 24[:00] hours of the first day of Farvardin and should return
   // #       to its previous state at the 24[:00] hours of the 30th day of
   // #       Shahrivar.
   //
   // For now it's hard-coded for 2005-2007.
   TZ_SET(tzGMTplus0330_1, PLUS_OFFSET(3, 30),
       UCAL_MARCH, 22, 0, 0*ONE_HOUR,
       UCAL_SEPTEMBER, 22, 0, 0*ONE_HOUR),

   // Rule    Azer    1997    max     -       Mar     lastSun  1:00   1:00    S
   // Rule    Azer    1997    max     -       Oct     lastSun  1:00   0       -
   // Zone    Asia/Baku     4:00    Azer    AZ%sT
   TZ_SET(tzGMTplus0400_1, PLUS_OFFSET(4, 0),
        UCAL_MARCH, -1, UCAL_SUNDAY, 1*ONE_HOUR,
        UCAL_OCTOBER, -1, UCAL_SUNDAY, 1*ONE_HOUR),

   // Zone Indian/Mauritius  4:00    -       MUT     # Mauritius Time
   TZ_NO_DST(tzGMTplus0400, PLUS_OFFSET(4, 0)),

   // Zone    Asia/Kabul    4:30    -       AFT
   TZ_NO_DST(tzGMTplus0430, PLUS_OFFSET(4, 30)),

   // Rule RussiaAsia 1993    max     -       Mar     lastSun  2:00s  1:00    S
   // Rule RussiaAsia 1996    max     -       Oct     lastSun  2:00s  0       -
   // Zone    Asia/Aqtobe     5:00 RussiaAsia AQT%sT  # Aqtobe Time
   TZ_SET3(tzGMTplus0500_1, PLUS_OFFSET(5, 0),
        UCAL_MARCH, -1, UCAL_SUNDAY, 2*ONE_HOUR,
        UCAL_OCTOBER, -1, UCAL_SUNDAY, 2*ONE_HOUR),

   // Zone    Asia/Dushanbe  5:00    -       TJT                 # Tajikistan Time
   TZ_NO_DST(tzGMTplus0500_2, PLUS_OFFSET(5, 0)),

   // New Delhi
   TZ_NO_DST(tzGMTplus0530, PLUS_OFFSET(5, 30)),

   // Zone    Asia/Katmandu   5:45    -       NPT     # Nepal Time
   TZ_NO_DST(tzGMTplus0545, PLUS_OFFSET(5, 45)),

   // Rule    Russia  1993    max     -       Mar     lastSun  2:00s  1:00    S
   // Rule    Russia  1996    max     -       Oct     lastSun  2:00s  0       -
   // Zone Asia/Novosibirsk  6:00   Russia  NOV%sT
   TZ_SET3(tzGMTplus0600_1, PLUS_OFFSET(6, 0),
       UCAL_MARCH, -1, UCAL_SUNDAY, 2*ONE_HOUR,
       UCAL_OCTOBER, -1, UCAL_SUNDAY, 2*ONE_HOUR),

   // Zone    Asia/Colombo    6:00    -       LKT
   TZ_NO_DST(tzGMTplus0600, PLUS_OFFSET(6, 0)),

   // Zone    Asia/Rangoon  6:30    -       MMT                # Myanmar Time
   TZ_NO_DST(tzGMTplus0630, PLUS_OFFSET(6, 30)),

   // Rule    Mongol  2001    max     -       Sep     lastSat 2:00    0       -
   // Rule    Mongol  2002    max     -       Mar     lastSat 2:00    1:00    S
   // Zone    Asia/Hovd    7:00    Mongol  HOV%sT
   TZ_SET(tzGMTplus0700_1, PLUS_OFFSET(7, 0),
        UCAL_MARCH, -1, UCAL_SATURDAY, 2*ONE_HOUR,
        UCAL_SEPTEMBER, -1, UCAL_SATURDAY, 2*ONE_HOUR),

   // Rule    Russia  1993    max     -       Mar     lastSun  2:00s  1:00    S
   // Rule    Russia  1996    max     -       Oct     lastSun  2:00s  0       -
   // Zone Asia/Krasnoyarsk   7:00   Russia  KRA%sT
   TZ_SET3(tzGMTplus0700_2, PLUS_OFFSET(7, 0),
       UCAL_MARCH, -1, UCAL_SUNDAY, 2*ONE_HOUR,
       UCAL_OCTOBER, -1, UCAL_SUNDAY, 2*ONE_HOUR),

   // Zone Asia/Jakarta      7:00    -       WIT
   TZ_NO_DST(tzGMTplus0700, PLUS_OFFSET(7, 0)),

   // Rule    Russia  1993    max     -       Mar     lastSun  2:00s  1:00    S
   // Rule    Russia  1996    max     -       Oct     lastSun  2:00s  0       -
   // Zone Asia/Irkutsk  8:00   Russia  IRK%sT
   TZ_SET3(tzGMTplus0800_1, PLUS_OFFSET(8, 0),
       UCAL_MARCH, -1, UCAL_SUNDAY, 2*ONE_HOUR,
       UCAL_OCTOBER, -1, UCAL_SUNDAY, 2*ONE_HOUR),

   // Zone    Asia/Hong_Kong    8:00    HK      HK%sT
   TZ_NO_DST(tzGMTplus0800, PLUS_OFFSET(8, 0)),

   // Zone    Asia/Tokyo  9:00    -       JST
   TZ_NO_DST(tzGMTplus0900, PLUS_OFFSET(9, 0)),

   // Rule    AS      1987    max     -       Oct     lastSun 2:00s   1:00    -
   // Rule    AS      1995    max     -       Mar     lastSun 2:00s   0       -
   // Zone Australia/Adelaide  9:30    AS      CST
   TZ_SET3(tzGMTplus0930_1, PLUS_OFFSET(9, 30),
       UCAL_OCTOBER, -1, UCAL_SUNDAY, 2*ONE_HOUR,
       UCAL_MARCH, -1, UCAL_SUNDAY, 2*ONE_HOUR),

   // Could not find rule.
   TZ_NO_DST(tzGMTplus0930, PLUS_OFFSET(9, 30)),

   // Rule    AT      1991    max     -       Mar     lastSun 2:00s   0       -
   // Rule    AT      2001    max     -       Oct     Sun>=1  2:00s   1:00    -
   // Zone Australia/Hobart   10:00   AT      EST
   TZ_SET3(tzGMTplus1000_1, PLUS_OFFSET(10, 0),
        UCAL_OCTOBER, 1, -UCAL_SUNDAY,  2*ONE_HOUR,
        UCAL_MARCH, -1, UCAL_SUNDAY, 2*ONE_HOUR),

   // Rule    AN      1996    max     -       Mar     lastSun 2:00s   0       -
   // Rule    AN      2001    max     -       Oct     lastSun 2:00s   1:00    -
   // Zone Australia/Sydney 10:00   AN      EST
   TZ_SET3(tzGMTplus1000_2, PLUS_OFFSET(10, 0),
        UCAL_OCTOBER, -1, UCAL_SUNDAY, 2*ONE_HOUR,
        UCAL_MARCH, -1, UCAL_SUNDAY, 2*ONE_HOUR),

   // Rule    Russia  1993    max     -       Mar     lastSun  2:00s  1:00    S
   // Rule    Russia  1996    max     -       Oct     lastSun  2:00s  0       -
   // Zone Asia/Vladivostok   10:00   Russia  VLA%sT
   TZ_SET3(tzGMTplus1000_3, PLUS_OFFSET(10, 0),
       UCAL_MARCH, -1, UCAL_SUNDAY, 2*ONE_HOUR,
       UCAL_OCTOBER, -1, UCAL_SUNDAY, 2*ONE_HOUR),

   // Zone Australia/Brisbane   10:00   AQ      EST
   TZ_NO_DST(tzGMTplus1000, PLUS_OFFSET(10, 0)),

   // Rule    LH      1996    max     -       Mar     lastSun 2:00    0       -
   // Rule    LH      2001    max     -       Oct     lastSun 2:00    0:30    -
   // Zone Australia/Lord_Howe   10:30   LH      LHST
   TZ_SET_HALFDST(tzGMTplus1030, PLUS_OFFSET(10, 30),
        UCAL_OCTOBER, -1, UCAL_SUNDAY, 2*ONE_HOUR,
        UCAL_MARCH, -1, UCAL_SUNDAY, 2*ONE_HOUR),

   // Rule    Russia  1993    max     -       Mar     lastSun  2:00s  1:00    S
   // Rule    Russia  1996    max     -       Oct     lastSun  2:00s  0       -
   // Zone Asia/Magadan   11:00   Russia  MAG%sT
   TZ_SET3(tzGMTplus1100_1, PLUS_OFFSET(11, 0),
       UCAL_MARCH, -1, UCAL_SUNDAY, 2*ONE_HOUR,
       UCAL_OCTOBER, -1, UCAL_SUNDAY, 2*ONE_HOUR),

   // Zone    Pacific/Efate   11:00   Vanuatu VU%sT   # Vanuatu Time
   TZ_NO_DST(tzGMTplus1100, PLUS_OFFSET(11, 0)),

   // Zone    Pacific/Norfolk   11:30   -       NFT             # Norfolk Time
   TZ_NO_DST(tzGMTplus1130, PLUS_OFFSET(11, 30)),

   // Rule    NZ      1990    max     -       Oct     Sun>=1  2:00s   1:00    D
   // Rule    NZ      1990    max     -       Mar     Sun>=15 2:00s   0       S
   // Zone Pacific/Auckland   12:00   NZ      NZ%sT
   TZ_SET3(tzGMTplus1200_1, PLUS_OFFSET(12, 0),
        UCAL_OCTOBER, 1, -UCAL_SUNDAY, 2*ONE_HOUR,
        UCAL_MARCH, 15, -UCAL_SUNDAY, 2*ONE_HOUR),

   // Rule    Russia  1993    max     -       Mar     lastSun  2:00s  1:00    S
   // Rule    Russia  1996    max     -       Oct     lastSun  2:00s  0       -
   // Zone Asia/Kamchatka  12:00   Russia  PET%sT
   TZ_SET3(tzGMTplus1200_2, PLUS_OFFSET(12, 0),
        UCAL_MARCH, -1, UCAL_SUNDAY, 2*ONE_HOUR,
        UCAL_OCTOBER, -1, UCAL_SUNDAY, 2*ONE_HOUR),

   // Zone    Pacific/Fiji   12:00   Fiji    FJ%sT   # Fiji Time
   TZ_NO_DST(tzGMTplus1200, PLUS_OFFSET(12, 0)),

   // Rule    Chatham 1990    max     -       Oct     Sun>=1  2:45s   1:00    D
   // Rule    Chatham 1990    max     -       Mar     Sun>=15 2:45s   0       S
   // Zone Pacific/Auckland  12:45   Chatham CHA%sT
   TZ_SET3(tzGMTplus1245_1, PLUS_OFFSET(12, 45),
        UCAL_OCTOBER, 1, -UCAL_SUNDAY, (2*ONE_HOUR) + (45*ONE_MINUTE),
        UCAL_MARCH, 15, -UCAL_SUNDAY, (2*ONE_HOUR) + (45*ONE_MINUTE)),

   // Zone Pacific/Tongatapu  13:00   Tonga   TO%sT
   TZ_NO_DST(tzGMTplus1300_1, PLUS_OFFSET(13, 0)),

   // Zone Pacific/Kiritimati 14:00  -       LINT
   TZ_NO_DST(tzGMTplus1400, PLUS_OFFSET(14, 0))
   
};


static int32 getOffset(TimeZoneRule *tz, int32 year, int32 month, int32 day,
                          uint8 dayOfWeek, int32 millis, TimeMode input_type, 
                          boolean *bDstActive, ErrorCode* status);

static int32 compareToRule(int8 month, int8 monthLen, int8 prevMonthLen,
                              int8 dayOfMonth,
                              int8 dayOfWeek, int32 millis, int32 millisDelta,
                              EMode ruleMode, int8 ruleMonth, int8 ruleDayOfWeek,
                              int8 ruleDay, int32 ruleMillis);

static void decodeStartRule(TimeZoneRule *tz, ErrorCode* status) ;
static void decodeEndRule(TimeZoneRule *tz, ErrorCode* status);
static int getDaysInMonth(int month, int year);
static void decodeRules(TimeZoneRule *tz, ErrorCode* status);



/*===========================================================================

                    FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION: AEEUsrSysClock_Free

DESCRIPTION:
  Free data associated with an IUsrSysClock. Assumes reference count already
  hit 0.

DEPENDENCIES
  None
  
PROTOTYPE
  static void AEEUsrSysClock_Free
  (
    IUsrSysClock*               pMe
  )

PARAMETERS
  pMe                        [In]- Pointer to IUsrSysClock data structure

RETURN VALUE
  None

COMMENTS 
  None

SIDE EFFECTS
  None
===========================================================================*/
static void AEEUsrSysClock_Free(IUsrSysClock *pMe)
{
   if (pMe == NULL)
   {
     return;
   }

   CALLBACK_Cancel(&pMe->m_cbExit);

   if (pMe->m_pIShell) 
   {
     RELEASEIF(pMe->m_pIShell);     
   }

   if (pMe) 
   {
     if (pMe->m_pISysClock)
     {
       RELEASEIF(pMe->m_pISysClock);
     }
     FREEIF(pMe);
   }
   gpSysClock = NULL;
}

/*===========================================================================
FUNCTION: AEEUsrSysClock_SysObjectCB

DESCRIPTION:
   Registered callback that will be called when the application goes away.  This
   will trigger freeing of the UsrSysClock

DEPENDENCIES
  None
  
PROTOTYPE
  static void AEEUsrSysClock_SysObjectCB
  (
    void *   pUsr
  )


PARAMETERS
  void * pUsr          [in] pointer to the user data

RETURN VALUE
  None

COMMENTS 
  None

SIDE EFFECTS
  None
===========================================================================*/
static void AEEUsrSysClock_SysObjectCB(void *pUsr)
{
  IUsrSysClock *pMe = (IUsrSysClock *)pUsr;

  if (pMe == NULL) 
  {
    return;
  }
  AEEUsrSysClock_Free((IUsrSysClock*)pUsr);
}

/*===========================================================================
FUNCTION: AEEUsrSysClock_New

DESCRIPTION:
  Create an instance of IUsrSysClock.

DEPENDENCIES
  None
  
PROTOTYPE
  int AEEUsrSysClock_New
  (
    IShell*                  pIShell,
    AEECLSID                 ClsId,
    void**                   ppObj
  )

PARAMETERS
  pIShell                    [In] - Pointer to IShell instance
  ClsId                      [In] - Class ID to create
  ppObj                      [Out]-Pointer to resulting IUsrSysClock instance

RETURN VALUE
  SUCCESS      Successfully created an IUsrSysClock instance
  EUNSUPPORTED Invalid parameters
  ENOMEMORY    Insufficient memory

COMMENTS 
  None

SIDE EFFECTS
  None
===========================================================================*/
int AEEUsrSysClock_New(IShell *pIShell, AEECLSID ClsId, void **ppObj)
{
   IUsrSysClock    *pMe = NULL;

   if (ClsId != AEECLSID_USRSYSCLOCK)
   {
     return EUNSUPPORTED;
   }

   if (ppObj == NULL)
   {
     return EBADPARM;
   }

   if (pIShell == NULL)
   {
     return EBADPARM;
   }

   if (NULL == gpSysClock)
   {
     int   nStatus;

     pMe = (IUsrSysClock *)MALLOC(sizeof(IUsrSysClock));

     if (NULL == pMe) {
         return ENOMEMORY;
     }

     MEMSET(pMe, 0, sizeof(IUsrSysClock));
     pMe->m_pvt = &gvtIUsrSysClock;
     pMe->m_uRefs = 1;
     pMe->m_pIShell = pIShell;
     pMe->m_nCls = AEECLSID_USRSYSCLOCK;
     (void)ISHELL_AddRef(pIShell);
#if MIN_BREW_VERSION(4,0)
     nStatus = ISHELL_CreateInstance(pIShell, AEECLSID_SysClock, (void**)&pMe->m_pISysClock);
#else
     nStatus = ISHELL_CreateInstance(pIShell, AEECLSID_SYSCLOCK, (void**)&pMe->m_pISysClock);
#endif
     if ((nStatus != SUCCESS) || (pMe->m_pISysClock == NULL))
     {
       RELEASEIF(pMe->m_pIShell);
       FREEIF(pMe);
       gpSysClock  = NULL;
       *ppObj = NULL;
       return nStatus;
     }

     gpSysClock = pMe;

     CALLBACK_Init(&gpSysClock->m_cbExit, (PFNNOTIFY)AEEUsrSysClock_SysObjectCB, (void *)pMe);
     ISHELL_OnExit(pIShell, &gpSysClock->m_cbExit);
  }
  else
  {
    (void)AEEUsrSysClock_AddRef(gpSysClock);
  }

  *ppObj = gpSysClock;
  return SUCCESS;
}

/*===========================================================================
FUNCTION: AEEUsrSysClock_AddRef

DESCRIPTION:
  Increment reference count for this instance of IUsrSysClock.

DEPENDENCIES
  None
  
PROTOTYPE
  static uint32 AEEUsrSysClock_AddRef
  (
     IUsrSysClock *   pMe
  )

PARAMETERS
  pMe                        [In]- Pointer to IUsrSysClock instance

RETURN VALUE
  Updated reference count.

COMMENTS 
  None

SIDE EFFECTS
  None
===========================================================================*/
static uint32 AEEUsrSysClock_AddRef(IUsrSysClock *pMe)
{
  if (pMe) 
  {
#if MIN_BREW_VERSION(4,0)
    (void)ISysClock_AddRef(pMe->m_pISysClock);
#else
    (void)ISYSCLOCK_AddRef(pMe->m_pISysClock);
#endif /* MIN_BREW_VERSION(4,0) */
    return (++pMe->m_uRefs);
  }

  return 0;
}

/*===========================================================================
FUNCTION: AEEUsrSysClock_Release

DESCRIPTION:
  Release IUsrSysClock object and its members.

DEPENDENCIES
  None
  
PROTOTYPE
  static uint32 AEEUsrSysClock_Release
  (
    IUsrSysClock *   pMe
  )


PARAMETERS
  pMe                        [In]- Pointer to IUsrSysClock instance

RETURN VALUE
  Reference count of IUsrSysClock instance remaining from the pMe pointer.

COMMENTS 
  Only frees the data associated with IUsrSysClock if the reference count
  hits 0.

SIDE EFFECTS
  None
===========================================================================*/
static uint32 AEEUsrSysClock_Release(IUsrSysClock *pMe)
{
   if (!pMe) 
   {
     return 0;
   }
   if (pMe->m_uRefs > 0) 
   { 
     if (--pMe->m_uRefs )
     {
       return pMe->m_uRefs;
     }
   }

   // Ref count is zero.  Release memory
   AEEUsrSysClock_Free(pMe);

   return 0;
}

/*===========================================================================
FUNCTION: AEEUsrSysClock_QueryInterface

DESCRIPTION:
  Query for an interface of IUsrSysClock.

DEPENDENCIES
  None
  
PROTOTYPE
  static int AEEUsrSysClock_QueryInterface
  (
    IUsrSysClock*            pMe,
    AEECLSID                 iid, 
    void**                   ppo
  )

PARAMETERS
  pMe                        [In] - Pointer to IUsrSysClock instance
  iid                        [In] - Interface ID
  ppo                        [Out]-Pointer to queried object

RETURN VALUE
  EBADPARM, SUCCESS or ECLASSNOTSUPPORT

COMMENTS 
  None

SIDE EFFECTS
  None
===========================================================================*/
static int AEEUsrSysClock_QueryInterface(IUsrSysClock *pMe,
                                         AEECLSID id,
                                         void **ppo)
{
  if ((ppo == NULL) || (pMe == NULL)) 
  {
    return EBADPARM;
  }
  if (NULL != IQI_SELF(pMe, id, ppo, AEEIID_USRSYSCLOCK))
  {
    *ppo = pMe;
    (void) AEEUsrSysClock_AddRef(*ppo);
    return SUCCESS;
  } 
  else 
  {
    *ppo = NULL;
    return ECLASSNOTSUPPORT;
  }
}

/*===========================================================================

FUNCTION AEEUsrSysClock_GetTimeUS()

DESCRIPTION
  This interface is used to get the time in microseconds since midnight
  Jan 6, 1980 GMT from the phone's clock.

DEPENDENCIES
  None

PROTOTYPE
  int AEEUsrSysClock_GetTimeUS
  (
    IUsrSysClock *   pMe,
    uint64struct *   pqwUS
  )

PARAMETERS
  pMe                 [In] - Pointer to IUsrSysClock object
  pqwUS               [Out]- Pointer to quad word struct for microseconds
   
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
static int AEEUsrSysClock_GetTimeUS(IUsrSysClock * pMe, uint64struct * pqwUS )
{
  int  iResult;
  uint64  qwTimeInMicroseconds;

   if ((!pMe) || (pMe->m_nCls != AEECLSID_USRSYSCLOCK))
   {
     return EFAILED;
   }

#if MIN_BREW_VERSION(4,0)
   iResult = ISysClock_GetTimeUS(pMe->m_pISysClock, &qwTimeInMicroseconds);
#else
   iResult = ISYSCLOCK_GetTimeUS(pMe->m_pISysClock, &qwTimeInMicroseconds);
#endif /* MIN_BREW_VERSION(4,0) */
   if (iResult != SUCCESS )
   {
     return iResult;
   }

   *pqwUS = uint64struct_from_uint64((uint64)qwTimeInMicroseconds);
   return SUCCESS;
}

/*===========================================================================

FUNCTION AEEUsrSysClock_SetTimeUS()

DESCRIPTION
  This interface is used to set the phone's user time.

DEPENDENCIES
  None
  
PROTOTYPE
  int AEEUsrSysClock_SetTimeUS
  (
    IUsrSysClock *          pMe,
    uint64struct            qwUS
  )

PARAMETERS
  pMe                [In]- Pointer to IUsrSysClock object
  qwUS               [In]- UTC time in microseconds to set

RETURN VALUE
  SUCCESS      Successfully set the time
  EUNSUPPORTED Invalid parameters
  EFAILED      Failed to set the clock

COMMENTS 
  None

SIDE EFFECTS
  None
===========================================================================*/
static int AEEUsrSysClock_SetTimeUS(IUsrSysClock * pMe, uint64struct qwUS)
{
  int  iResult;

  if (pMe) 
  {
    if (pMe->m_nCls != AEECLSID_USRSYSCLOCK)
    {
      return EUNSUPPORTED;
    }

#if MIN_BREW_VERSION(4,0)
    iResult = ISysClock_SetTimeUS(pMe->m_pISysClock, uint64struct_to_uint64(qwUS));
#else
    iResult = ISYSCLOCK_SetTimeUS(pMe->m_pISysClock, uint64struct_to_uint64(qwUS));
#endif /* MIN_BREW_VERSION(4,0) */
    if (iResult != SUCCESS )
    {
      return iResult;
    }
    return SUCCESS;
  }
  return EFAILED;
}



/*===========================================================================

FUNCTION AEEUsrSysClock_GetTimeS()

DESCRIPTION
  This interface is used to get the time in seconds since midnight
  Jan 6, 1980 GMT from the phone's clock.

DEPENDENCIES
  None

PROTOTYPE
  uint32 AEEUsrSysClock_GetTimeS
  (
    IUsrSysClock *         pMe,
    uint32 *               pdwSeconds
  )

PARAMETERS
  pMe                 [In] - Pointer to IUsrSysClock object
  pdwSeconds          [Out]- Pointer to uint32 for seconds
                              since 1/6/80 00:00:00 GMT

RETURN VALUE
  SUCCESS, EBADSTATE, EFAILED

COMMENTS 
  The phone's clock is automatically set to 1/6/1980 upon first-time
  initialization.  If the return value from this function returns a
  date in the year 1980, it may be because the phone's clock was never
  properly set.

SIDE EFFECTS
  None
===========================================================================*/
static int AEEUsrSysClock_GetTimeS(IUsrSysClock * pMe, uint32 * pdwSeconds )
{
   int  iResult;

   if ((!pMe) || (pMe->m_nCls != AEECLSID_USRSYSCLOCK))
   {
     return EFAILED;
   }

#if MIN_BREW_VERSION(4,0)
   iResult = ISysClock_GetTimeS(pMe->m_pISysClock, pdwSeconds );
#else
   iResult = ISYSCLOCK_GetTimeS(pMe->m_pISysClock, pdwSeconds );
#endif /* MIN_BREW_VERSION(4,0) */
   if (iResult != SUCCESS )
   {
     return iResult;
   }
   return SUCCESS;
}

/*===========================================================================

FUNCTION AEEUsrSysClock_SetTimeS()

DESCRIPTION
  This interface is used to set the time in seconds since midnight
  Jan 6, 1980 GMT on the phone's clock.

DEPENDENCIES
  None
  
PROTOTYPE
  int AEEUsrSysClock_SetTimeS
  (
    IUsrSysClock *           pMe,
    uint32                   dwSeconds
  )

PARAMETERS
  pMe                        [In]- Pointer to IUsrSysClock object
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
static int AEEUsrSysClock_SetTimeS(IUsrSysClock * pMe, uint32 dwSeconds)
{
  int  iResult;

  if (pMe) 
  {
    if (pMe->m_nCls != AEECLSID_USRSYSCLOCK)
    {
      return EUNSUPPORTED;
    }
#if MIN_BREW_VERSION(4,0)
    iResult = ISysClock_SetTimeS(pMe->m_pISysClock, dwSeconds);
#else
    iResult = ISYSCLOCK_SetTimeS(pMe->m_pISysClock, dwSeconds);
#endif /* MIN_BREW_VERSION(4,0) */
    if (iResult != SUCCESS )
    {
      return iResult;
    }
    return SUCCESS;
  }
  return EFAILED;
}



/*===========================================================================

                     AEEUsrSysClock FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION AEEUsrSysClock_GetTimeZone()

DESCRIPTION
  This interface is used to get the current system time zone.

DEPENDENCIES
  None

PROTOTYPE  
  int AEEUsrSysClock_GetTimeZone
  (
    IUsrSysClock * pMe, 
    AEETimeZoneType * peTimeZone
  )

PARAMETERS
  pMe,                [In] - Pointer to IUsrSysClock object
  peTimeZone          [Out]- Pointer to current system time zone

RETURN VALUE
  SUCCESS      Successfully got the current system time zone.
  EBADPARM     Invalid parameters

COMMENTS 
  None
  
SIDE EFFECTS
  None
===========================================================================*/
int AEEUsrSysClock_GetTimeZone(IUsrSysClock *pMe, AEETimeZoneType* peTimeZone)
{
  if (peTimeZone == NULL) 
  {
    return EBADPARM;
  }
  *peTimeZone =  gTimeZone;

  return SUCCESS;
}

/*===========================================================================

FUNCTION AEEUsrSysClock_SetTimeZone()

DESCRIPTION
  This interface is used to set the user time zone.

DEPENDENCIES
  None

PROTOTYPE  
  int AEEUsrSysClock_SetTimeZone
  (
    IUsrSysClock * pMe, 
    AEETimeZoneType eTimeZone
  )

PARAMETERS
  pMe              [In]- Pointer to IUsrSysClock object
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
int AEEUsrSysClock_SetTimeZone(IUsrSysClock *pMe, AEETimeZoneType eTimeZone)
{
   int retVal = SUCCESS;

   if (eTimeZone > tzTimeZoneLast) {
      return EBADPARM;
   }
   gTimeZone = eTimeZone;

   return retVal;
}

/*===========================================================================

FUNCTION AEEUsrSysClock_GetNetworkTimeZone

DESCRIPTION
  This interface is used to get the network time zone. This time-zone is distinct from the
  system time zone (set by the user).  When the "auto time" option is enabled, the network
  time zone should be updated from information available in the NITZ message.

DEPENDENCIES
  None

PROTOTYPE  
  int AEEUsrSysClock_GetNetworkTimeZone
  ( 
    IUsrSysClock * pMe, 
    boolean * pbIsAvailable,
    int32 * pdwLocalTimeOffset,
    boolean * pbDaylightSavings
  )

PARAMETERS
  pMe                       [In] - Pointer to IUsrSysClock object
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
int AEEUsrSysClock_GetNetworkTimeZone(IUsrSysClock *pMe, boolean * pbIsAvailable,
   int32 * pdwLocalTimeOffset, boolean * pbDaylightSavings)
{
   if (!pbIsAvailable || !pdwLocalTimeOffset || !pbDaylightSavings) {
      return EBADPARM;
   }

   *pbIsAvailable = gNetworkTimeZone.isAvailable;
   *pdwLocalTimeOffset = gNetworkTimeZone.tzOffset;
   *pbDaylightSavings = gNetworkTimeZone.isDST;

   return SUCCESS;
}

/*===========================================================================

FUNCTION AEEUsrSysClock_SetNetworkTimeZone

DESCRIPTION
  This interface is used to set the network time zone. This time-zone is distinct from the
  system time zone (set by the user).  When the "auto time" option is enabled, the network
  time zone should be updated from information available in the NITZ message.  If
  bIsAvailable is FALSE, then the remaining parameters are ignored.

DEPENDENCIES
  None

PROTOTYPE
  int AEEUsrSysClock_SetNetworkTimeZone
  (
    IUsrSysClock *pMe, 
    boolean bIsAvailable,
    int32 dwLocalTimeOffset, 
    boolean bDaylightSavings
  )

PARAMETERS
  pMe                        [In]- Pointer to IUsrSysClock object
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
int AEEUsrSysClock_SetNetworkTimeZone(IUsrSysClock *pMe, boolean bIsAvailable,
   int32 dwLocalTimeOffset, boolean bDaylightSavings)
{
   gNetworkTimeZone.isAvailable = bIsAvailable;
   gNetworkTimeZone.tzOffset = dwLocalTimeOffset;
   gNetworkTimeZone.isDST = bDaylightSavings;

   return SUCCESS;
}

/*===========================================================================

FUNCTION AEEUsrSysClock_GetTimeOffset()

DESCRIPTION
  This interface is used to get the offset for a given time in UTC seconds at a particular time zone. The offset
  is *added* to UTC time to get the local time in that time zone. Optionally returns whether daylight
  savings is active.

DEPENDENCIES
  None

PARAMETERS
  pMe                       [In] - Pointer to IUsrSysClock object
  dwUTCTimeInSeconds        [In] - UTC time in seconds
  eTimeZone                 [In] - Time zone to use for evaluation
  pOffset                   [Out]- Pointer to offset that is added to UTC to get local time
  pbDaylightSavings         [Out]- Optional pointer to boolean to indicate whether daylight 
                                   savings is active

RETURN VALUE
  SUCCESS      Successfully got the offset
  EBADPARM     Invalid parameters
  EFAILED

COMMENTS 
  pbDaylightSavings is permitted to be NULL.
  This function is not static because it is used directly by OEMOS.c
  
SIDE EFFECTS
  None
===========================================================================*/
int AEEUsrSysClock_GetTimeOffset(IUsrSysClock *pMe, uint32 dwUTCTimeInSeconds, AEETimeZoneType eTimeZone,
      int32* pOffset, boolean* pbDaylightSavings)
{
   JulianType julian_time;
   int32 millis;
   int32 offsetVal;
   boolean daylightSavings;
   ErrorCode status;

   if ((NULL == pOffset) ||
       (eTimeZone > tzTimeZoneLast)) {
      return EBADPARM;
   }

   // Convert from seconds format to Julian format.
   GETJULIANDATE( dwUTCTimeInSeconds, &julian_time);

   // Convert to milliseconds.
   millis = (julian_time.wHour * MILLISEC_PER_HOUR) + 
      (julian_time.wMinute * MILLISEC_PER_MINUTE) +
      (julian_time.wSecond * MILLISEC_PER_SECOND);

   // Get the offset to local time from UTC including daylight savings shift.
   offsetVal = getOffset(&gTimeZoneRules[eTimeZone],
         julian_time.wYear,
         julian_time.wMonth - 1,   //  must be 0 based
         julian_time.wDay,
         kJulianDay_To_UCalDay[julian_time.wWeekDay],
         millis,
         UTC_TIME, // Input time is given as UTC time.
         &daylightSavings,
         &status
     );

   if (U_FAILURE(status)) {
      return EFAILED;
   }

   // The caller wants the arguments in seconds not milliseconds.
   *pOffset = offsetVal / MILLISEC_PER_SECOND;

   // If the caller wants the daylight savings indication, return it.
   if (NULL != pbDaylightSavings) {
      *pbDaylightSavings = (boolean) daylightSavings;
   }

   return SUCCESS;
}

/*===========================================================================

FUNCTION AEEUsrSysClock_ConvertToLocalTime()

DESCRIPTION
  This interface is used to convert UTC time in seconds to the local time in seconds
  based on the current user time zone.

DEPENDENCIES
  None

PROTOTYPE
  uint32 AEEUsrSysClock_ConvertToLocalTime
  (
    IUsrSysClock * pMe, 
    uint32 dwUTCTimeInSeconds
  )

PARAMETERS
  pMe                        [In]- Pointer to IUsrSysClock object
  dwUTCTimeInSeconds         [In]- UTC time in seconds

RETURN VALUE
  Local time in seconds

COMMENTS 
  None

SIDE EFFECTS
  None
===========================================================================*/
uint32 AEEUsrSysClock_ConvertToLocalTime(IUsrSysClock *pMe, uint32 dwUTCTimeInSeconds)
{
   int32 utcOffset = 0;
   AEETimeZoneType eTimeZone;
   boolean bAutoTime;
   boolean bDaylight = FALSE;
   boolean bTZAvail;
   IConfig  *pIConfig = NULL;
   IShell   *pIShell = NULL;

   pIShell = AEE_GetShell();
   if (!pIShell)  return EFAILED;

   if (ISHELL_CreateInstance(pIShell, AEECLSID_CONFIG, (void **)&pIConfig)
      != SUCCESS)   return EFAILED;

   if (pIConfig == NULL) return EFAILED;

   if (ICONFIG_GetItem(pIConfig, CFGI_AUTO_TIME_ENABLE, (void *)&bAutoTime,
      sizeof(bAutoTime)) != SUCCESS) {
      bAutoTime = FALSE;
   }

   do
   {
      if (TRUE == bAutoTime) {
         if (AEEUsrSysClock_GetNetworkTimeZone(pMe, &bTZAvail, &utcOffset, &bDaylight)
            == SUCCESS) {

            if (TRUE == bTZAvail) {
               /* Use the values from network time zone. */
               break;
            }
         }
         // Fall through to attempt the user specified time-zone.
      }

      if (AEEUsrSysClock_GetTimeZone(pMe, &eTimeZone) != SUCCESS) {
         return dwUTCTimeInSeconds;
      }

      if ( AEEUsrSysClock_GetTimeOffset(pMe, dwUTCTimeInSeconds, eTimeZone, &utcOffset, NULL) !=
         SUCCESS) {
         return dwUTCTimeInSeconds;
      }
   }
   while(FALSE);

   return (uint32) ((int32)dwUTCTimeInSeconds + utcOffset);
}

/*===========================================================================

FUNCTION AEEUsrSysClock_ConvertToUTCTime()

DESCRIPTION
  This interface is used to convert local time in seconds to UTC time in seconds
  based on the current system time zone.

DEPENDENCIES
  None

PROTOTYPE
  uint32 AEEUsrSysClock_ConvertToUTCTime
  (
    IUsrSysClock *pMe, 
    uint32 dwLocalTimeInSeconds
  )

PARAMETERS
  pMe                        [In]- Pointer to IUsrSysClock object
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
uint32 AEEUsrSysClock_ConvertToUTCTime(IUsrSysClock *pMe, uint32 dwLocalTimeInSeconds)
{
   int32 utcOffset = 0;
   AEETimeZoneType eTimeZone;
   JulianType  julian_time;
   int32 millis;
   int32 offsetVal;
   boolean daylightSavings;
   ErrorCode status;
   boolean bAutoTime;
   boolean bDaylight = FALSE;
   boolean bTZAvail;
   IConfig  *pIConfig = NULL;
   IShell   *pIShell = NULL;

   pIShell = AEE_GetShell();
   if (!pIShell)  return EFAILED;

   if (ISHELL_CreateInstance(pIShell, AEECLSID_CONFIG, (void **)&pIConfig)
      != SUCCESS)   return EFAILED;

   if (pIConfig == NULL) return EFAILED;

   if (ICONFIG_GetItem(pIConfig, CFGI_AUTO_TIME_ENABLE, (void *)&bAutoTime,
      sizeof(bAutoTime)) != SUCCESS) {
      bAutoTime = FALSE;
   }

   do
   {
      if (TRUE == bAutoTime) {
         if (AEEUsrSysClock_GetNetworkTimeZone(pMe, &bTZAvail, &utcOffset, &bDaylight)
            == SUCCESS) {

            if (TRUE == bTZAvail) {
               /* Use the values from network time zone. */
               break;
            }
         }
         // Fall through to attempt the user specified time-zone.
      }

      if (AEEUsrSysClock_GetTimeZone(pMe, &eTimeZone) != SUCCESS) {
         return dwLocalTimeInSeconds;
      }

      // Convert from seconds format to Julian format.
      GETJULIANDATE(dwLocalTimeInSeconds, &julian_time);

      // Convert to milliseconds.
      millis = (julian_time.wHour * MILLISEC_PER_HOUR) + 
         (julian_time.wMinute * MILLISEC_PER_MINUTE) +
         (julian_time.wSecond * MILLISEC_PER_SECOND);

      // Get the offset to local time from UTC including daylight savings shift.
      offsetVal = getOffset(&gTimeZoneRules[eTimeZone],
            julian_time.wYear,
            julian_time.wMonth-1, // Must be 0-based
            julian_time.wDay,
            kJulianDay_To_UCalDay[julian_time.wWeekDay],
            millis,
            WALL_TIME, // Input time is given as the local wall clock time.
            &daylightSavings,
            &status
        );

      if (U_FAILURE(status)) {
         return dwLocalTimeInSeconds;
      }

      // The caller wants the arguments in seconds not milliseconds.
      utcOffset = offsetVal / MILLISEC_PER_SECOND;
   }
   while(FALSE);

   return (uint32) ((int32)dwLocalTimeInSeconds - utcOffset);
}

/*===========================================================================

FUNCTION AEEUsrSysClock_ConvertOffsetToTimeZone()

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
  int AEEUsrSysClock_ConvertOffsetToTimeZone
  (
    IUsrSysClock * pMe, 
    int32 dwOffset,
    uint32 dwSecsDaylightAdj, 
    boolean bHasDaylightSavings, 
    AEETimeZoneType * peTimeZone
  )

PARAMETERS
  pMe                        [In] - Pointer to IUsrSysClock object
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
int AEEUsrSysClock_ConvertOffsetToTimeZone(IUsrSysClock *pMe, int32 dwOffset,
   uint32 dwSecsDaylightAdj, boolean bHasDaylightSavings, AEETimeZoneType *peTimeZone)
{
   int i;
   int32 utc_rawOffset;

   if (!peTimeZone) {
      return EBADPARM;
   }

   if (TRUE == bHasDaylightSavings) {
      utc_rawOffset = (dwOffset - dwSecsDaylightAdj) * MILLISEC_PER_SECOND;
   }
   else {
      utc_rawOffset = dwOffset * MILLISEC_PER_SECOND;
   }

   for (i = tzTimeZoneFirst; i < NUM_OF_TIMEZONES; i++) {
      if ((gTimeZoneRules[i].rawOffset == utc_rawOffset) &&
          (gTimeZoneRules[i].useDaylight == bHasDaylightSavings)) {
         *peTimeZone = gTimeZoneRules[i].id;
         return SUCCESS;
      }
   }

   /* Run a second pass in the event that the caller of the function may not
    * know whether or not DST really is used for that time-zone. */

   if ((TRUE == bHasDaylightSavings) && (0 == dwSecsDaylightAdj)) {
      for (i = tzTimeZoneFirst; i < NUM_OF_TIMEZONES; i++) {
         if ((gTimeZoneRules[i].rawOffset == utc_rawOffset) &&
             (FALSE == gTimeZoneRules[i].useDaylight)) {
            *peTimeZone = gTimeZoneRules[i].id;
            return SUCCESS;
         }
      }
   }
   return EFAILED;
}

/*===========================================================================

FUNCTION AEEUsrSysClock_IsTimeSet()

DESCRIPTION
  Return TRUE if the user clock has been set, FALSE otherwise. Note that is the
  time has never been set, the initial value will be midnight Jan 6, 1980 GMT.

DEPENDENCIES
  None

PROTOTYPE
  boolean AEEUsrSysClock_IsTimeSet
  (
    IUsrSysClock *pMe
  )

PARAMETERS
  pMe               [In]- Pointer to IUsrSysClock object

RETURN VALUE
  TRUE         Time has been set.
  FALSE        Time has not been set.

COMMENTS 
  None

SIDE EFFECTS
  None
===========================================================================*/

boolean AEEUsrSysClock_IsTimeSet(IUsrSysClock *pMe)
{
  int  result;
  uint32 dwSeconds;

  #define SECS_PER_DAY (60l * 60l * 24l)

  result = AEEGETPVTBL((pMe),IUsrSysClock)->GetTimeS(pMe, &dwSeconds);

  if ((result != SUCCESS) || (dwSeconds < ((uint32)365 * SECS_PER_DAY)))
  {
     return FALSE;
  }
  return TRUE;
}

/*===========================================================================

LOCAL FUNCTION getOffset

DESCRIPTION
  Gets offset, for current date, modified in case of
  daylight savings. This is the offset to add *to* UTC to get local time.
  Gets the time zone offset, for current date, modified in case of daylight
  savings. This is the offset to add *to* UTC to get local time. Assume
  that the start and end month are distinct.

DEPENDENCIES
  None

PROTOTYPE
  static int32 getOffset
  (
    TimeZoneRule * tz,
    int32 year,
    int32 month,
    int32 day,
    uint8 dayOfWeek,
    int32 millis,
    TimeMode input_type,
    boolean * bDstActive,
    ErrorCode * status
  )

PARAMETERS
  tz                      [In] - Pointer to time zone rule to use during calculation
  year                    [In] - The year in the given date.
  month                   [In] - The month in the given date. Month is 0-based. e.g., 0 for January.
  day                     [In] - The day-in-month of the given date.
  dayOfWeek               [In] - The day-of-week of the given date. Sunday is 1 ... Saturday is 7.
  millis                  [In] - The milliseconds in day in UTC time.
  input_type              [In] - The format of the given time (e.g. Wall clock or UTC)
  bDstActive              [Out]- TRUE if daylight savings is active, FALSE otherwise.
  status                  [Out]- ZERO_ERROR if no error during calculation.

RETURN VALUE
  The offset to add to UTC to get local time.

COMMENTS 
  None

SIDE EFFECTS
  None
===========================================================================*/
static int32 getOffset(TimeZoneRule *tz, int32 year, int32 month, int32 day,
                       uint8 dayOfWeek, int32 millis, TimeMode input_type, 
                       boolean *bDstActive, ErrorCode* status)
{
   int32 startCompare;
   int32 endCompare;
   boolean southern;
   int32 monthLength;
   int32 prevMonthLength;
   int32 millisDelta;
   int32 result = tz->rawOffset;

   // Set up default return values.
   *bDstActive = FALSE;
   *status = ZERO_ERROR;

   if (month < UCAL_JANUARY ||
      month > UCAL_DECEMBER) {
      *status = ILLEGAL_ARGUMENT_ERROR;
      return 0;
   }

   monthLength = getDaysInMonth(month, year);
   prevMonthLength = (month >= 1) ? getDaysInMonth(month-1, year) : 31;

   if (day < 1
       || day > monthLength
       || dayOfWeek < UCAL_SUNDAY
       || dayOfWeek > UCAL_SATURDAY
       || millis < 0
       || millis >= MILLISEC_PER_DAY
       || monthLength < 28
       || monthLength > 31
       || prevMonthLength < 28
       || prevMonthLength > 31) 
   {
     *status = ILLEGAL_ARGUMENT_ERROR;
     return 0;
   }

   if (!tz->isDecoded) {
      decodeRules(tz, status);
      tz->isDecoded = TRUE;

      if(U_FAILURE(*status)) {
         // Failed decoding timezone rule. Default to 0 for all future requests.

         tz->rawOffset = 0;
         tz->useDaylight = FALSE;
         return 0;
      }
    }

    // Bail out if we are before the onset of daylight savings time
    if(!tz->useDaylight || year < tz->startYear) 
      return result;

    // Check for southern hemisphere.  We assume that the start and end
    // month are different.
    southern = (tz->startMonth > tz->endMonth);

    // Depending on the format of the time given as the input parameter, we need to
    // shift it to match the format of time given in the time zone rule (start time mode).
    switch(input_type) {
      case WALL_TIME:
      case STANDARD_TIME:
         millisDelta = tz->startTimeMode == UTC_TIME ? -tz->rawOffset : 0;
         break;
      case UTC_TIME:
      default:
         millisDelta = tz->startTimeMode == UTC_TIME ? 0 : tz->rawOffset;
         break;
    }

    // Compare the date to the starting and ending rules.+1 = date>rule, -1
    // = date<rule, 0 = date==rule.
    startCompare = compareToRule((int8)month, (int8)monthLength, (int8)prevMonthLength,
                                         (int8)day, (int8)dayOfWeek, millis,
                                         millisDelta,
                                         tz->startMode, (int8)tz->startMonth, (int8)tz->startDayOfWeek,
                                         (int8)tz->startDay, tz->startTime);
    endCompare = 0;

    /* We don't always have to compute endCompare.  For many instances,
     * startCompare is enough to determine if we are in DST or not.  In the
     * northern hemisphere, if we are before the start rule, we can't have
     * DST.  In the southern hemisphere, if we are after the start rule, we
     * must have DST.  This is reflected in the way the next if statement
     * (not the one immediately following) short circuits. */
    if(southern != (startCompare >= 0)) {

        // Depending on the format of the time given as the input parameter, we need to
        // shift it to match the format of time given in the time zone rule (end time mode).
        switch(input_type) {
          case WALL_TIME:
             millisDelta = tz->endTimeMode == WALL_TIME ? 0 :
                                    (tz->endTimeMode == UTC_TIME ? -tz->rawOffset - tz->dstSavings : -tz->dstSavings);
             break;
          case STANDARD_TIME:
             millisDelta = tz->endTimeMode == WALL_TIME ? tz->dstSavings :
                                    (tz->endTimeMode == UTC_TIME ? -tz->rawOffset : 0);
             break;
          case UTC_TIME:
          default:
             millisDelta = tz->endTimeMode == WALL_TIME ? tz->rawOffset + tz->dstSavings :
                                    (tz->endTimeMode == UTC_TIME ? 0 : tz->rawOffset);
             break;
        }

        endCompare = compareToRule((int8)month, (int8)monthLength, (int8)prevMonthLength,
                                   (int8)day, (int8)dayOfWeek, millis,
                                   millisDelta,
                                   tz->endMode, (int8)tz->endMonth, (int8)tz->endDayOfWeek,
                                   (int8)tz->endDay, tz->endTime);
    }

    // Check for both the northern and southern hemisphere cases.  We
    // assume that in the northern hemisphere, the start rule is before the
    // end rule within the calendar year, and vice versa for the southern
    // hemisphere.
    if ((!southern && (startCompare >= 0 && endCompare < 0)) ||
        (southern && (startCompare >= 0 || endCompare < 0))) {
        result += tz->dstSavings;
        *bDstActive = TRUE;
    }
    return result;
}

/*===========================================================================

LOCAL FUNCTION compareToRule

DESCRIPTION
  Compare a given date in the year to a rule. Return 1, 0, or -1, depending
  on whether the date is after, equal to, or before the rule date. The
  millis are compared directly against the ruleMillis, so any
  standard-daylight adjustments must be handled by the caller.

DEPENDENCIES
  None

PROTOTYPE
  static int32 compareToRule
  (
    int8 month,
    int8 monthLen,
    int8 prevMonthLen,
    int8 dayOfMonth,
    int8 dayOfWeek,
    int32 millis,
    int32 millisDelta,
    EMode ruleMode,
    int8 ruleMonth,
    int8 ruleDayOfWeek,
    int8 ruleDay,
    int32 ruleMillis
  )

PARAMETERS
    month               [In]- Month to compare against rule. Month is 0-based, 0 for January.
    monthLen            [In]- Length of month in number of days.
    prevMonthLen        [In]- Length of previous month in number of days.
    dayOfMonth          [In]- Day of the month.
    dayOfWeek           [In]- Day of the week. Day is 1-based, 1 for Sunday, 7 for Saturday.
    millis              [In]- Milliseconds in UTC time of the time to compare
    millisDelta         [In]- Offset to add to milliseconds to adjust for the rule time mode
                              (WALL, STD, UTC).
    ruleMode            [In]- Decoded rule type.
    ruleMonth           [In]- Month of the rule.
    ruleDayOfWeek       [In]- Day of week of the rule. 1-based.
    ruleDay             [In]- Day of the rule.
    ruleMillis          [In]- Milliseconds in WALL, STD, or UTC time when rule goes into effect.

RETURN VALUE
  1 if the date is after the rule date

 -1 if the date is before the rule date,
 
  0 if the date is equal to the rule date.

COMMENTS 
  None

SIDE EFFECTS
  None
===========================================================================*/
static int32 compareToRule(int8 month, 
                           int8 monthLen, 
                           int8 prevMonthLen,
                           int8 dayOfMonth,
                           int8 dayOfWeek, 
                           int32 millis, 
                           int32 millisDelta,
                           EMode ruleMode, 
                           int8 ruleMonth, 
                           int8 ruleDayOfWeek,
                           int8 ruleDay,
                           int32 ruleMillis)
{
    int32 ruleDayOfMonth;

    // Make adjustments for startTimeMode and endTimeMode
    millis += millisDelta;
    while (millis >= MILLISEC_PER_DAY) {
        millis -= MILLISEC_PER_DAY;
        ++dayOfMonth;
        dayOfWeek = (int8)(1 + (dayOfWeek % 7)); // dayOfWeek is one-based
        if (dayOfMonth > monthLen) {
            dayOfMonth = 1;
            /* When incrementing the month, it is desirible to overflow
             * from DECEMBER to DECEMBER+1, since we use the result to
             * compare against a real month. Wraparound of the value
             * leads to bug 4173604. */
            ++month;
        }
    }
    while (millis < 0) {
        millis += MILLISEC_PER_DAY;
        --dayOfMonth;
        dayOfWeek = (int8)(1 + ((dayOfWeek+5) % 7)); // dayOfWeek is one-based
        if (dayOfMonth < 1) {
            dayOfMonth = prevMonthLen;
            --month;
        }
    }

    // first compare months.  If they're different, we don't have to worry about days
    // and times
    if (month < ruleMonth) return -1;
    else if (month > ruleMonth) return 1;

    // calculate the actual day of month for the rule
    ruleDayOfMonth = 0;
    switch (ruleMode)
    {
    // if the mode is day-of-month, the day of month is given
    case DOM_MODE:
        ruleDayOfMonth = ruleDay;
        break;

    // if the mode is day-of-week-in-month, calculate the day-of-month from it
    case DOW_IN_MONTH_MODE:
        // In this case ruleDay is the day-of-week-in-month (this code is using
        // the dayOfWeek and dayOfMonth parameters to figure out the day-of-week
        // of the first day of the month, so it's trusting that they're really
        // consistent with each other)
        if (ruleDay > 0)
            ruleDayOfMonth = 1 + (ruleDay - 1) * 7 +
                (7 + ruleDayOfWeek - (dayOfWeek - dayOfMonth + 1)) % 7;
        
        // if ruleDay is negative (we assume it's not zero here), we have to do
        // the same calculation figuring backward from the last day of the month.
        // (STATICMONTHLENGTH gives us that last day.  We don't take leap years
        // into account, so this may not work right for February.)
        else
        {
             // (again, this code is trusting that dayOfMonth and dayOfMonth are
            // consistent with each other here, since we're using them to figure
            // the day of week of the first of the month)
            ruleDayOfMonth = monthLen + (ruleDay + 1) * 7 -
                (7 + (dayOfWeek + monthLen - dayOfMonth) - ruleDayOfWeek) % 7;
        }
        break;

    case DOW_GE_DOM_MODE:
        ruleDayOfMonth = ruleDay +
            (49 + ruleDayOfWeek - ruleDay - dayOfWeek + dayOfMonth) % 7;
        break;

    case DOW_LE_DOM_MODE:
        ruleDayOfMonth = ruleDay -
            (49 - ruleDayOfWeek + ruleDay + dayOfWeek - dayOfMonth) % 7;
        // Note at this point ruleDayOfMonth may be <1, although it will
        // be >=1 for well-formed rules.
        break;
    }

    // now that we have a real day-in-month for the rule, we can compare days...
    if (dayOfMonth < ruleDayOfMonth) return -1;
    else if (dayOfMonth > ruleDayOfMonth) return 1;

    // ...and if they're equal, we compare times
    if (millis < ruleMillis) return -1;
    else if (millis > ruleMillis) return 1;
    else return 0;
}

/*===========================================================================

LOCAL FUNCTION getDaysInMonth

DESCRIPTION
  Get the number of days in a month accounting for leap years.

DEPENDENCIES
  None

PROTOTYPE
  static int getDaysInMonth
  (
    int month,
    int year
  )

PARAMETERS
  month            [In]- The month to find number of days. 
                         Month is 0-based, 0 for January.
  year             [In]- The year in the given date.

RETURN VALUE
  Number of days in the month for the given year.

COMMENTS 
  None

SIDE EFFECTS
  None
===========================================================================*/
static int getDaysInMonth(int month, int year)
{
  if (month == UCAL_FEBRUARY){
   if (( (year&3) == 0) && (( (year%100) != 0) || ( (year%400) == 0)))
      return 29;
    else 
      return 28;
  } else if (month < UCAL_AUGUST)
    return 31 - (month & 1);
  else
    return 30 + (month & 1);
}

/*===========================================================================

LOCAL FUNCTION decodeRules

DESCRIPTION
 Given a set of encoded rules in startDay and startDayOfMonth, decode
 them and set the startMode appropriately.  Do the same for endDay and
 endDayOfMonth.  Upon entry, the day of week variables may be zero or
 negative, in order to indicate special modes.  The day of month
 variables may also be negative.  Upon exit, the mode variables will be
 set, and the day of week and day of month variables will be positive.
 This method also recognizes a startDay or endDay of zero as indicating
 no DST.

DEPENDENCIES
  None

PROTOTYPE
  static void decodeRules
  (
    TimeZoneRule * tz
    ErrorCode * status
  )

PARAMETERS
  tz                          [In] - The time zone rule to decode.
  status                      [Out]- The status of the decoding operation.

RETURN VALUE
  None

COMMENTS 
       Rule representation

       We represent the following flavors of rules:
             5        the fifth of the month
             lastSun  the last Sunday in the month
             lastMon  the last Monday in the month
             Sun>=8   first Sunday on or after the eighth
             Sun<=25  last Sunday on or before the 25th

       INTERNAL REPRESENTATION
       This is the format time zone rule structures take after decoding is complete.
       Rules are represented directly, using an
       unencoded format.  We will discuss the start rule only below; the end
       rule is analogous.
         startMode      Takes on enumerated values DAY_OF_MONTH,
                        DOW_IN_MONTH, DOW_AFTER_DOM, or DOW_BEFORE_DOM.
         startDay       The day of the month, or for DOW_IN_MONTH mode, a
                        value indicating which DOW, such as +1 for first,
                        +2 for second, -1 for last, etc.
         startDayOfWeek The day of the week.  Ignored for DAY_OF_MONTH.

       ENCODED REPRESENTATION
         MODE              startMonth    startDay    startDayOfWeek
         DOW_IN_MONTH_MODE >=0           !=0         >0
         DOM_MODE          >=0           >0          ==0
         DOW_GE_DOM_MODE   >=0           >0          <0
         DOW_LE_DOM_MODE   >=0           <0          <0
         (no DST)          don't care    ==0         don't care

SIDE EFFECTS
  None
===========================================================================*/
static void decodeRules(TimeZoneRule *tz, ErrorCode* status)
{
    decodeStartRule(tz, status);
    decodeEndRule(tz, status);
}

/*===========================================================================

LOCAL FUNCTION decodeStartRule

DESCRIPTION
  Decode the start rule and validate the parameters.  The parameters are
  expected to be in encoded form, which represents the various rule modes
  by negating or zeroing certain values.  Representation formats are:

             DOW_IN_MONTH  DOM    DOW>=DOM  DOW<=DOM  no DST
             ------------  -----  --------  --------  ----------
  month       0..11        same    same      same     don't care
  day        -5..5         1..31   1..31    -1..-31   0
  dayOfWeek   1..7         0      -1..-7    -1..-7    don't care
  time        0..ONEDAY    same    same      same     don't care

  The range for month does not include UNDECIMBER since this class is
  really specific to GregorianCalendar, which does not use that month.
  The range for time includes ONEDAY (vs. ending at ONEDAY-1) because the
  end rule is an exclusive limit point.  That is, the range of times that
  are in DST include those >= the start and < the end.  For this reason,
  it should be possible to specify an end of ONEDAY in order to include the
  entire day.  Although this is equivalent to time 0 of the following day,
  it's not always possible to specify that, for example, on December 31.
  While arguably the start range should still be 0..ONEDAY-1, we keep
  the start and end ranges the same for consistency.

DEPENDENCIES
  None

PROTOTYPE
  static void decodeStartRule
  (
    TimeZoneRule * tz
    ErrorCode * status
  )

PARAMETERS
  tz                            [In] - The time zone rule to decode.
  status                        [Out]- The status of the decoding operation.

RETURN VALUE
  None

COMMENTS 
  None

SIDE EFFECTS
  None
===========================================================================*/
static void decodeStartRule(TimeZoneRule *tz, ErrorCode* status) 
{
    if(U_FAILURE(*status)) return;

    tz->useDaylight = (boolean)((tz->startDay != 0) && (tz->endDay != 0) ? TRUE : FALSE);
    if (tz->useDaylight && tz->dstSavings == 0) {
        tz->dstSavings = MILLISEC_PER_HOUR;
    }
    if (tz->startDay != 0) {
        if (tz->startMonth < UCAL_JANUARY || tz->startMonth > UCAL_DECEMBER) {
            *status = ILLEGAL_ARGUMENT_ERROR;
            return;
        }
        if ((tz->startTime < 0) || (tz->startTime > MILLISEC_PER_DAY) ||
            ((tz->startTimeMode != WALL_TIME) && 
             (tz->startTimeMode != STANDARD_TIME) && 
             (tz->startTimeMode != UTC_TIME))) {
            *status = ILLEGAL_ARGUMENT_ERROR;
            return;
        }
        if (tz->startDayOfWeek == 0) {
            tz->startMode = DOM_MODE;
        } else {
            if (tz->startDayOfWeek > 0) {
                tz->startMode = DOW_IN_MONTH_MODE;
            } else {
                tz->startDayOfWeek = (int8)-tz->startDayOfWeek;
                if (tz->startDay > 0) {
                    tz->startMode = DOW_GE_DOM_MODE;
                } else {
                    tz->startDay = (int8)-tz->startDay;
                    tz->startMode = DOW_LE_DOM_MODE;
                }
            }
            if (tz->startDayOfWeek > UCAL_SATURDAY) {
                *status = ILLEGAL_ARGUMENT_ERROR;
                return;
            }
        }
        if (tz->startMode == DOW_IN_MONTH_MODE) {
            if (tz->startDay < -5 || tz->startDay > 5) {
                *status = ILLEGAL_ARGUMENT_ERROR;
                return;
            }
        } else if (tz->startDay > STATICMONTHLENGTH[tz->startMonth]) {
            *status = ILLEGAL_ARGUMENT_ERROR;
            return;
        }
    }
}

/*===========================================================================

LOCAL FUNCTION decodeEndRule

DESCRIPTION
  Decode the end rule and validate the parameters.  This method is exactly
  analogous to decodeStartRule().
  See decodeStartRule

DEPENDENCIES
  None

PROTOTYPE
  static void decodeEndRule
  (
    TimeZoneRule * tz
    ErrorCode * status
  )

PARAMETERS
  tz                            [In] - The time zone rule to decode.
  status                        [Out]- The status of the decoding operation.

RETURN VALUE
  None

COMMENTS 
  None

SIDE EFFECTS
  None
===========================================================================*/
static void decodeEndRule(TimeZoneRule *tz, ErrorCode* status) 
{
    if(U_FAILURE(*status)) return;

    tz->useDaylight = (boolean)((tz->startDay != 0) && (tz->endDay != 0) ? TRUE : FALSE);
    if (tz->useDaylight && tz->dstSavings == 0) {
        tz->dstSavings = MILLISEC_PER_HOUR;
    }
    if (tz->endDay != 0) {
        if (tz->endMonth < UCAL_JANUARY || tz->endMonth > UCAL_DECEMBER) {
            *status = ILLEGAL_ARGUMENT_ERROR;
            return;
        }
        if (tz->endTime < 0 || tz->endTime > MILLISEC_PER_DAY ||
            ((tz->startTimeMode != WALL_TIME) && 
             (tz->startTimeMode != STANDARD_TIME) && 
             (tz->startTimeMode != UTC_TIME))) {
            *status = ILLEGAL_ARGUMENT_ERROR;
            return;
        }
        if (tz->endDayOfWeek == 0) {
            tz->endMode = DOM_MODE;
        } else {
            if (tz->endDayOfWeek > 0) {
                tz->endMode = DOW_IN_MONTH_MODE;
            } else {
                tz->endDayOfWeek = (int8)-tz->endDayOfWeek;
                if (tz->endDay > 0) {
                    tz->endMode = DOW_GE_DOM_MODE;
                } else {
                    tz->endDay = (int8)-tz->endDay;
                    tz->endMode = DOW_LE_DOM_MODE;
                }
            }
            if (tz->endDayOfWeek > UCAL_SATURDAY) {
                *status = ILLEGAL_ARGUMENT_ERROR;
                return;
            }
        }
        if (tz->endMode == DOW_IN_MONTH_MODE) {
            if (tz->endDay < -5 || tz->endDay > 5) {
                *status = ILLEGAL_ARGUMENT_ERROR;
                return;
            }
        } else if (tz->endDay > STATICMONTHLENGTH[tz->endMonth]) {
            *status = ILLEGAL_ARGUMENT_ERROR;
            return;
        }
    }
}

#endif  // FEATURE_USRSYSCLOCK
