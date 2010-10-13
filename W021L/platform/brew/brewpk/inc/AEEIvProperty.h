#ifndef AEEIVPROPERTY_H
#define AEEIVPROPERTY_H
/*==============================================================================

File:
   AEEIvProperty.h

Services:
   vCard/vCalendar property Parser related services.

General Description:
   vCard and vCalendar are having a group of properties. This file contains the
   interface required by both the vCard & vCalendar parsers.

Public Classes And Static Functions:
   IvProperty
   

INITIALIZATION AND SEQUENCING REQUIREMENTS:

         (c) COPYRIGHT 2007 QUALCOMM Incorporated.
                  All Rights Reserved.
                 QUALCOMM Proprietary
==============================================================================*/
#include "AEEIvParm.h"
#include "AEE64structs.h"
#include "AEEDateTime.h"

#define AEEIID_IvProperty      0x0105d01d

/*vCalendar & vCard properties. AEEVProperty */
#define AEEVPROPERTY_AALARM         0x00000001
#define AEEVPROPERTY_ACTION         0x00000002
#define AEEVPROPERTY_ATTACH         0x00000003
#define AEEVPROPERTY_ATTENDEE       0x00000004
#define AEEVPROPERTY_CALSCALE       0x00000005
#define AEEVPROPERTY_CATEGORIES     0x00000006
#define AEEVPROPERTY_CLASS          0x00000007
#define AEEVPROPERTY_COMMENT        0x00000008
#define AEEVPROPERTY_CONTACT        0x00000009
#define AEEVPROPERTY_CREATED        0x0000000a
#define AEEVPROPERTY_DALARM         0x0000000b
#define AEEVPROPERTY_DAYLIGHT       0x0000000c
#define AEEVPROPERTY_DCOMPLETED     0x0000000d
#define AEEVPROPERTY_DCREATED       0x0000000e
#define AEEVPROPERTY_DESCRIPTION    0x0000000f
#define AEEVPROPERTY_DTEND          0x00000010
#define AEEVPROPERTY_DTSTAMP        0x00000011
#define AEEVPROPERTY_DTSTART        0x00000012
#define AEEVPROPERTY_DUE            0x00000013
#define AEEVPROPERTY_DURATION       0x00000014
#define AEEVPROPERTY_EXDATE         0x00000015
#define AEEVPROPERTY_EXRULE         0x00000016
#define AEEVPROPERTY_EXTENDED       0x00000017
#define AEEVPROPERTY_FREEBUSY       0x00000018
#define AEEVPROPERTY_GEO            0x00000019
#define AEEVPROPERTY_LASTMODIFIED   0x0000001a   
#define AEEVPROPERTY_LOCATION       0x0000001b
#define AEEVPROPERTY_MALARM         0x0000001c
#define AEEVPROPERTY_METHOD         0x0000001d
#define AEEVPROPERTY_ORGANIZER      0x0000001e
#define AEEVPROPERTY_PALARM         0x0000001f
#define AEEVPROPERTY_PERCENT        0x00000020
#define AEEVPROPERTY_PRIORITY       0x00000021
#define AEEVPROPERTY_PRODID         0x00000022
#define AEEVPROPERTY_RDATE          0x00000023
#define AEEVPROPERTY_RECUR_ID       0x00000024
#define AEEVPROPERTY_RELATED        0x00000025
#define AEEVPROPERTY_REPEAT         0x00000026
#define AEEVPROPERTY_RESOURCES      0x00000027
#define AEEVPROPERTY_RNUM           0x00000028
#define AEEVPROPERTY_RRULE          0x00000029
#define AEEVPROPERTY_RSTATUS        0x0000002a
#define AEEVPROPERTY_SEQUENCE       0x0000002b
#define AEEVPROPERTY_SORTSTRING     0x0000002c
#define AEEVPROPERTY_STATUS         0x0000002d
#define AEEVPROPERTY_SUMMARY        0x0000002e
#define AEEVPROPERTY_TIMEZONE       0x0000002f
#define AEEVPROPERTY_TRANSP         0x00000030
#define AEEVPROPERTY_TRIGGER        0x00000031
#define AEEVPROPERTY_TZID           0x00000032
#define AEEVPROPERTY_TZNAME         0x00000033
#define AEEVPROPERTY_TZOFFSETFROM   0x00000034
#define AEEVPROPERTY_TZOFFSETTO     0x00000035
#define AEEVPROPERTY_TZURL          0x00000036
#define AEEVPROPERTY_UID            0x00000037
#define AEEVPROPERTY_URL            0x00000038
#define AEEVPROPERTY_VERSION        0x00000039
#define AEEVPROPERTY_XRULE          0x0000003a

#define AEEVPROPERTY_ADR            0x0000003b
#define AEEVPROPERTY_AGENT          0x0000003c
#define AEEVPROPERTY_BIRTHDATE      0x0000003d
#define AEEVPROPERTY_EMAIL          0x0000003e
#define AEEVPROPERTY_FN             0x0000003f
#define AEEVPROPERTY_KEY            0x00000040
#define AEEVPROPERTY_LABEL          0x00000041
#define AEEVPROPERTY_LOGO           0x00000042
#define AEEVPROPERTY_MAILER         0x00000043
#define AEEVPROPERTY_NAME           0x00000044
#define AEEVPROPERTY_NICKNAME       0x00000045
#define AEEVPROPERTY_NOTE           0x00000046
#define AEEVPROPERTY_ORG            0x00000047
#define AEEVPROPERTY_PHOTO          0x00000048
#define AEEVPROPERTY_REV            0x00000049
#define AEEVPROPERTY_ROLE           0x0000004a
#define AEEVPROPERTY_SOUND          0x0000004b
#define AEEVPROPERTY_TEL            0x0000004c
#define AEEVPROPERTY_TITLE          0x0000004d


#define AEEVPROPERTY_BREW_START     0x00010000

#define AEEVPROPERTY_OID            AEEVPROPERTY_BREW_START + 1

typedef uint32 AEEVProperty;

typedef struct
{
   AEEVValueType   nValueType;
   uint32          dwDataSize;
   void            *pData;

}AEEVData;


/* Alarm specific */
typedef struct
{
   uint32      nRpt_Cnt;
   char        *pszTrigger;
   char        *pszDuration;
   char        *pszStr1;
   char        *pszStr2;

}AEEVAlarm;  


/* Date, Date-Time or Period type representaion */
typedef struct
{

   AEEVValueType  dwValueType;
   boolean        bIsDuration;
   boolean        bIsPriorDuration;
   boolean        bIsUTC;
   uint32         dwValue1;
   uint32         dwValue2;
   AEEDateTime    sDuration;

}AEEVDateTimePeriod;


/* UTC Date in Julian type */
typedef struct
{
   boolean     bIsUTC; 
   uint32      dwJulianSeconds;

}AEEVUTCDate;

/* Geographical postion */
typedef struct
{
   doublestruct   nLongitude;
   doublestruct   nLatitude;
  
}AEEVGeo;

/* Recurrence Rule value type */
#define AEEVRRULE_FREQUENCY      0x00000001
#define AEEVRRULE_UNTIL          0x00000002
#define AEEVRRULE_COUNT          0x00000003
#define AEEVRRULE_INTERVAL       0x00000004
#define AEEVRRULE_BYSECOND       0x00000005
#define AEEVRRULE_BYMINUTE       0x00000006
#define AEEVRRULE_BYHOUR         0x00000007
#define AEEVRRULE_BYDAY          0x00000008
#define AEEVRRULE_BYMONTHDAY     0x00000009
#define AEEVRRULE_BYYEARDAY      0x0000000a
#define AEEVRRULE_BYWEEKNO       0x0000000b
#define AEEVRRULE_BYMONTH        0x0000000c
#define AEEVRRULE_BYSETPOS       0x0000000d
#define AEEVRRULE_WKST           0x0000000e
#define AEEVRRULE_EXTENDED       0x0000000f

typedef uint32 AEEVRRuleValueType;


/* Recurrence Rule: Frequency rule part values */
#define AEEVFREQUQNCY_INVALID     0x00000001
#define AEEVFREQUENCY_SECONDLY    0x00000002
#define AEEVFREQUENCY_MINUTLELY   0x00000003
#define AEEVFREQUENCY_HOURLY      0x00000004
#define AEEVFREQUENCY_DAILY       0x00000005
#define AEEVFREQUENCY_WEEKLY      0x00000006
#define AEEVFREQUENCY_MONTHLY     0x00000007
#define AEEVFREQUENCY_YEARLY      0x00000008

typedef uint32 AEEVFrequency;


/*Month in the year */
#define AEEVMONTH_JAN      0x00000001
#define AEEVMONTH_FEB      0x00000002
#define AEEVMONTH_MAR      0x00000004
#define AEEVMONTH_APR      0x00000008
#define AEEVMONTH_MAY      0x00000010
#define AEEVMONTH_JUN      0x00000020
#define AEEVMONTH_JUL      0x00000040
#define AEEVMONTH_AUG      0x00000080
#define AEEVMONTH_SEP      0x00000100
#define AEEVMONTH_OCT      0x00000200
#define AEEVMONTH_NOV      0x00000400
#define AEEVMONTH_DEC      0x00000800

typedef uint32 AEEVMonth;


/*Day in week */
#define AEEVDAY_SUN        0x00000001
#define AEEVDAY_MON        0x00000002
#define AEEVDAY_TUE        0x00000004
#define AEEVDAY_WED        0x00000008
#define AEEVDAY_THU        0x00000010
#define AEEVDAY_FRI        0x00000020
#define AEEVDAY_SAT        0x00000040

typedef uint32 AEEVWeekDay;

#define AEEV_NR_WEEKDAYS         7

/* Recurrence rule */
typedef struct
{
   AEEVFrequency   nFrequency;
   uint32          dwCount;
   uint32          dwInterval;  
   AEEVUTCDate     endDate;
   AEEVWeekDay     dwWeekDay;
   AEEVMonth       dwMonthInYear;
   uint32          dwWKST;
   uint32          dwSize;
   uint32          *pnBySecList;
   uint32          *pnByMinList;
   uint32          *pnByHrList;
   int32           *pnWeekDayList[AEEV_NR_WEEKDAYS];
   int32           *pnDayInMonth;
   int32           *pnDayInYear;
   int32           *pnWeekNoList;
   int32           *pnBySpList;

} AEEVRRule;


/* Utility function for MEMSET*/
static __inline int AEEVCalendar_MEMSET(void* p1, char cChar, int length)
{
   unsigned char *cpc1 = (unsigned char *)p1;   

   while (length-- > 0) {
      *cpc1++ = cChar;
   }
   return 0;
}

static __inline void AEEVCalendar_InitRRule(AEEVRRule *pAEEVRRule)
{
   if ( NULL != pAEEVRRule ){
	     (void)AEEVCalendar_MEMSET(pAEEVRRule, (char)0, sizeof(AEEVRRule));      
      pAEEVRRule->nFrequency   = AEEVFREQUQNCY_INVALID;
      pAEEVRRule->dwInterval   = 1;
   }
   
}

/*==============================================================================
     IVPROPERTY INTERFACE
==============================================================================*/

#define INHERIT_IvProperty(iname)                                           \
   INHERIT_IvObject(iname);                                                 \
   int (*AddParm)(iname *po, IvParm *pIvParm);                              \
   int (*RemoveParm)(iname *po, IvParm *pIvParm);                           \
   int (*GetNumberOfParms)(iname *po,AEEVParameter nParmId,int *pnNumParms);\
   int (*GetParmAtIndex)(iname            *po,                              \
                         AEEVParameter    nId,                              \
                         int              nIndex,                           \
                         IvParm           **ppIvParm);                      \
   int (*GetId)(iname  *po, AEEVProperty *pnPropId);                        \
   int (*InitProperty)(iname          *po,                                  \
                       AEEVProperty   nPropId,                              \
                       char           *pszPropName,                         \
                       boolean        bParsedValue,                         \
                       void           *pValue,                              \
                       int            nSize,                                \
                       char           *pszGroup);                           \
   int (*GetPropertyName)(iname          *po,                               \
                          char           *pszBuf,                           \
                          int            nBufSize,                          \
                          int            *pnBufSizeReq);                    \
   int (*GetGroupName)(iname             *po,                               \
                       char              *pszBuf,                           \
                       int               nBufSize,                          \
                       int               *pnBufSizeReq);                    \
   int (*ParseValue)(iname         *po,                                     \
                     boolean       bParseData,                              \
                     void          *pBuf,                                   \
                     int           nBufSize,                                \
                     int           *pnBufSizeReq)

AEEINTERFACE_DEFINE(IvProperty);


static __inline uint32 IvProperty_AddRef(IvProperty *pIvProperty)
{
   return AEEGETPVTBL(pIvProperty,IvProperty)->AddRef(pIvProperty);
}


static __inline uint32 IvProperty_Release(IvProperty *pIvProperty)
{
   return AEEGETPVTBL(pIvProperty,IvProperty)->Release(pIvProperty);
}


static __inline int IvProperty_QueryInterface(IvProperty  *pIvProperty, 
                                              AEEIID      iid,
                                              void        **ppo)
{
   return AEEGETPVTBL(pIvProperty,IvProperty)->QueryInterface(pIvProperty,
                                                              iid,
                                                              ppo);
}


static __inline int IvProperty_GetObjectType(IvProperty *pIvProperty,
                                             AEECLSID   *pnObjType)
{
   return AEEGETPVTBL(pIvProperty, IvProperty)->GetObjectType(pIvProperty,
                                                              pnObjType);
}

static __inline int IvProperty_AddParm(IvProperty   *pIvProperty,
                                       IvParm       *pIvParm)
{
   return AEEGETPVTBL(pIvProperty,IvProperty)->AddParm(pIvProperty,
                                                       pIvParm);
}


static __inline int IvProperty_RemoveParm(IvProperty *pIvProperty,
                                          IvParm     *pIvParm)
{
   return AEEGETPVTBL(pIvProperty, IvProperty)->RemoveParm(pIvProperty,
                                                           pIvParm);
}

static __inline int IvProperty_GetNumberOfParms(IvProperty    *pIvProperty,
                                                AEEVParameter nParmId,
                                                int           *pnNumParms)
{
   return AEEGETPVTBL(pIvProperty,IvProperty)->GetNumberOfParms(pIvProperty,
                                                                 nParmId,
                                                                 pnNumParms);
}

static __inline int IvProperty_GetParmAtIndex(IvProperty    *pIvProperty,
                                              AEEVParameter nParmId,
                                              int           nIndex,
                                              IvParm        **ppIvParm)
{
   return AEEGETPVTBL(pIvProperty, IvProperty)->GetParmAtIndex(pIvProperty,
                                                               nParmId,
                                                               nIndex,
                                                               ppIvParm);
}

static __inline int IvProperty_GetId(IvProperty    *pIvProperty,
                                     AEEVProperty  *pnPropId)
{
   return AEEGETPVTBL(pIvProperty,IvProperty)->GetId(pIvProperty,pnPropId);
}

static __inline int IvProperty_InitProperty(IvProperty    *pIvProperty,
                                            AEEVProperty  nPropId,
                                            char          *pszPropertyName,
                                            boolean       bParsedValue,                         
                                            void          *pValue,
                                            int           nSize,
                                            char          *pszGroup)
{
   return AEEGETPVTBL(pIvProperty,IvProperty)->InitProperty(pIvProperty,
                                                            nPropId,
                                                            pszPropertyName,
                                                            bParsedValue,
                                                            pValue,
                                                            nSize,
                                                            pszGroup);
}

static __inline int IvProperty_GetPropertyName(IvProperty *pIvProperty,
                                               char       *pszBuf,
                                               int        nBufSize,
                                               int        *pnBufSizeReq)
{
   return AEEGETPVTBL(pIvProperty,IvProperty)->GetPropertyName(pIvProperty,
                                                               pszBuf,
                                                               nBufSize,
                                                               pnBufSizeReq);
}
static __inline int IvProperty_GetGroupName(IvProperty  *pIvProperty,
                                            char        *pszBuf,
                                            int         nBufSize,
                                            int         *pnBufSizeReq)
{
   return AEEGETPVTBL(pIvProperty,IvProperty)->GetGroupName(pIvProperty,
                                                            pszBuf,
                                                            nBufSize,
                                                            pnBufSizeReq);
}

static __inline int IvProperty_ParseValue(IvProperty  *pIvProperty,
                                          boolean     bParseData,
                                          void        *pProperty,
                                          int         nSize,
                                          int         *pnSizeReq)
{
   return AEEGETPVTBL(pIvProperty,IvProperty)->ParseValue(pIvProperty,
                                                          bParseData,
                                                          pProperty,
                                                          nSize,
                                                          pnSizeReq);
}

static __inline IvObject *IvProperty_To_IvObject(IvProperty *pIvProperty)
{
   return (IvObject*)(void *) pIvProperty;
}

/*=============================================================================
 DATA STRUCTURE DOCUMENTATION
===============================================================================

AEEVProperty

Description:
   Specifies the property types.

Definition:
           
   #define AEEVPROPERTY_AALARM         0x00000001
   #define AEEVPROPERTY_ACTION         0x00000002
   #define AEEVPROPERTY_ATTACH         0x00000003
   #define AEEVPROPERTY_ATTENDEE       0x00000004
   #define AEEVPROPERTY_CALSCALE       0x00000005
   #define AEEVPROPERTY_CATEGORIES     0x00000006
   #define AEEVPROPERTY_CLASS          0x00000007
   #define AEEVPROPERTY_COMMENT        0x00000008
   #define AEEVPROPERTY_CONTACT        0x00000009
   #define AEEVPROPERTY_CREATED        0x0000000a
   #define AEEVPROPERTY_DALARM         0x0000000b
   #define AEEVPROPERTY_DAYLIGHT       0x0000000c
   #define AEEVPROPERTY_DCOMPLETED     0x0000000d
   #define AEEVPROPERTY_DCREATED       0x0000000e
   #define AEEVPROPERTY_DESCRIPTION    0x0000000f
   #define AEEVPROPERTY_DTEND          0x00000010
   #define AEEVPROPERTY_DTSTAMP        0x00000011
   #define AEEVPROPERTY_DTSTART        0x00000012
   #define AEEVPROPERTY_DUE            0x00000013
   #define AEEVPROPERTY_DURATION       0x00000014
   #define AEEVPROPERTY_EXDATE         0x00000015
   #define AEEVPROPERTY_EXRULE         0x00000016
   #define AEEVPROPERTY_EXTENDED       0x00000017
   #define AEEVPROPERTY_FREEBUSY       0x00000018
   #define AEEVPROPERTY_GEO            0x00000019
   #define AEEVPROPERTY_LASTMODIFIED   0x0000001a   
   #define AEEVPROPERTY_LOCATION       0x0000001b
   #define AEEVPROPERTY_MALARM         0x0000001c
   #define AEEVPROPERTY_METHOD         0x0000001d
   #define AEEVPROPERTY_ORGANIZER      0x0000001e
   #define AEEVPROPERTY_PALARM         0x0000001f
   #define AEEVPROPERTY_PERCENT        0x00000020
   #define AEEVPROPERTY_PRIORITY       0x00000021
   #define AEEVPROPERTY_PRODID         0x00000022
   #define AEEVPROPERTY_RDATE          0x00000023
   #define AEEVPROPERTY_RECUR_ID       0x00000024
   #define AEEVPROPERTY_RELATED        0x00000025
   #define AEEVPROPERTY_REPEAT         0x00000026
   #define AEEVPROPERTY_RESOURCES      0x00000027
   #define AEEVPROPERTY_RNUM           0x00000028
   #define AEEVPROPERTY_RRULE          0x00000029
   #define AEEVPROPERTY_RSTATUS        0x0000002a
   #define AEEVPROPERTY_SEQUENCE       0x0000002b
   #define AEEVPROPERTY_SORTSTRING     0x0000002c
   #define AEEVPROPERTY_STATUS         0x0000002d
   #define AEEVPROPERTY_SUMMARY        0x0000002e
   #define AEEVPROPERTY_TIMEZONE       0x0000002f
   #define AEEVPROPERTY_TRANSP         0x00000030
   #define AEEVPROPERTY_TRIGGER        0x00000031
   #define AEEVPROPERTY_TZID           0x00000032
   #define AEEVPROPERTY_TZNAME         0x00000033
   #define AEEVPROPERTY_TZOFFSETFROM   0x00000034
   #define AEEVPROPERTY_TZOFFSETTO     0x00000035
   #define AEEVPROPERTY_TZURL          0x00000036
   #define AEEVPROPERTY_UID            0x00000037
   #define AEEVPROPERTY_URL            0x00000038
   #define AEEVPROPERTY_VERSION        0x00000039
   #define AEEVPROPERTY_XRULE          0x0000003a

   #define AEEVPROPERTY_ADR            0x0000003b
   #define AEEVPROPERTY_AGENT          0x0000003c
   #define AEEVPROPERTY_BIRTHDATE      0x0000003d
   #define AEEVPROPERTY_EMAIL          0x0000003e
   #define AEEVPROPERTY_FN             0x0000003f
   #define AEEVPROPERTY_KEY            0x00000040
   #define AEEVPROPERTY_LABEL          0x00000041
   #define AEEVPROPERTY_LOGO           0x00000042
   #define AEEVPROPERTY_MAILER         0x00000043
   #define AEEVPROPERTY_NAME           0x00000044
   #define AEEVPROPERTY_NICKNAME       0x00000045
   #define AEEVPROPERTY_NOTE           0x00000046
   #define AEEVPROPERTY_ORG            0x00000047
   #define AEEVPROPERTY_PHOTO          0x00000048
   #define AEEVPROPERTY_REV            0x00000049
   #define AEEVPROPERTY_ROLE           0x0000004a
   #define AEEVPROPERTY_SOUND          0x0000004b
   #define AEEVPROPERTY_TEL            0x0000004c
   #define AEEVPROPERTY_TITLE          0x0000004d

   #define AEEVPROPERTY_OID            AEEVPROPERTY_BREW_START + 1

   typedef uint32 AEEVProperty;


Members: 
   The ids specify property types supported in vCalendar and vCard
   
   AEEVPROPERTY_AALARM          : This property defines an audio reminder for the 
                                  vCalendar entity.
   AEEVPROPERTY_ACTION          : This property defines the action to be invoked
                                  when an alarm is triggered
   AEEVPROPERTY_ATTACH          : The property provides the capability to
                                  associate a document object with a calendar
                                  component
   AEEVPROPERTY_ATTENDEE        : This property defines an "Attendee" within a
                                  calendar component
   AEEVPROPERTY_CALSCALE        : This property defines the calendar scale used
                                  for the calendar information specified in the
                                  iCalendar object
   AEEVPROPERTY_CATEGORIES      : This property defines the categories for a
                                  calendar component
   AEEVPROPERTY_CLASS           : This property defines the access
                                  classification for a calendar component
   AEEVPROPERTY_COMMENT         : This property specifies non-processing
                                  information intended to provide a comment to
                                  the calendar user
   AEEVPROPERTY_CONTACT         : This property is used to represent contact
                                  information or alternately a reference to
                                  contact information associated with the
                                  calendar component
   AEEVPROPERTY_CREATED         : This property specifies the date and time that
                                  the calendar information was created by the
                                  calendar user agent in the calendar store
   AEEVPROPERTY_DALARM          : This property defines a display reminder for
                                  the vCalendar entity.
   AEEVPROPERTY_DAYLIGHT        : This property defines the daylight savings
                                  time rule observed by the “home” calendar
                                  system that created the vCalendar entity
   AEEVPROPERTY_DCOMPLETED      : This property defines the date and time that
                                  the todo was actually completed
   AEEVPROPERTY_DCREATED        : This property specifies the date and time that
                                  the vCalendar entity was created within the
                                  originating calendar system
   AEEVPROPERTY_DESCRIPTION     : This property provides a more complete
                                  description of the calendar component, than
                                  that provided by the "SUMMARY" property
   AEEVPROPERTY_DTEND           : This property specifies the date and time
                                  that a calendar component ends
   AEEVPROPERTY_DTSTAMP         : The property indicates the date/time that the
                                  instance of the iCalendar object was created
   AEEVPROPERTY_DTSTART         : This property specifies when the calendar
                                  component begins
   AEEVPROPERTY_DUE             : This property defines the date and time that a
                                  to-do is expected to be completed
   AEEVPROPERTY_DURATION        : The property specifies a positive duration of
                                  time
   AEEVPROPERTY_EXDATE          : This property defines the list of date/time
                                  exceptions for a recurring calendar component
   AEEVPROPERTY_EXRULE          : This property defines a rule or repeating
                                  pattern for an exception to a recurrence set
   AEEVPROPERTY_EXTENDED        : This property defines a a non-standard
                                  extended property
   AEEVPROPERTY_FREEBUSY        : The property defines one or more free or busy
                                  time intervals
   AEEVPROPERTY_GEO             : This property specifies information related to
                                  the global position for the activity specified
                                  
   AEEVPROPERTY_LASTMODIFIED    : The property specifies the date and time that
                                  the information associated with the calendar
                                  component was last revised in the calendar
                                  store
   AEEVPROPERTY_LOCATION        : The property defines the intended venue for
                                  the activity defined by a calendar component
   AEEVPROPERTY_MALARM          : This property defines an email address that
                                  is to be sent a reminder for the vCalendar
                                  entity.
   AEEVPROPERTY_METHOD          : This property defines the iCalendar object
                                  method associated with the calendar object
   AEEVPROPERTY_ORGANIZER       : The property defines the organizer for a
                                  calendar component
   AEEVPROPERTY_PALARM          : This property defines a procedure reminder for
                                  the vCalendar entity.
   AEEVPROPERTY_PERCENT         : This property is used by an assignee or
                                  delegatee of a to-do to convey the percent
                                  completion of a to-do to the Organizer
   AEEVPROPERTY_PRIORITY        : The property defines the relative priority for
                                  a calendar component
   AEEVPROPERTY_PRODID          : This property specifies the identifier for the
                                  product that created the iCalendar object
   AEEVPROPERTY_RDATE           : This property defines the list of date/times
                                  for a recurrence set
   AEEVPROPERTY_RECUR_ID        : This property is used in conjunction with the
                                  "UID" and "SEQUENCE" property to identify a
                                  specific instance of a recurring "VEVENT",
                                  "VTODO" or "VJOURNAL" calendar component. The
                                  property value is the effective value of the
                                  "DTSTART" property of the recurrence instance
   AEEVPROPERTY_RELATED         : The property is used to represent a
                                  relationship or reference between one calendar
                                  component and another 
   AEEVPROPERTY_REPEAT          : This property defines the number of time the
                                  alarm should be repeated, after the initial
                                  trigger
   AEEVPROPERTY_RESOURCES       : This property defines the equipment or
                                  resources anticipated for an activity
                                  specified by a calendar entity
   AEEVPROPERTY_RNUM            : The property defines the number of times the
                                  calendar entry will reoccur
   AEEVPROPERTY_RRULE           : This property defines a rule or repeating
                                  pattern for recurring events, to-dos, or
                                  time zone definitions
   AEEVPROPERTY_RSTATUS         : This property, REQUEST-STATUS, defines the
                                  status code returned for a scheduling request
   AEEVPROPERTY_SEQUENCE        : This property defines the revision sequence
                                  number of the calendar component within a
                                  sequence of revisions
   AEEVPROPERTY_SORTSTRING      : To specify the family name or given name text
                                  to be used for national-language-specific
                                  sorting of the FN and N types
   AEEVPROPERTY_STATUS          : This property defines the overall status or
                                  confirmation for the calendar component
   AEEVPROPERTY_SUMMARY         : This property defines a short summary or
                                  subject for the calendar component
   AEEVPROPERTY_TIMEZONE        : This property specifies the standard time zone
                                  of the “home” system that created the
                                  vCalendar object
   AEEVPROPERTY_TRANSP          : This property defines whether an event is
                                  transparent or not to busy time searches
   AEEVPROPERTY_TRIGGER         : This property specifies when an alarm will
                                  trigger
   AEEVPROPERTY_TZID            : This property specifies the text value that
                                  uniquely identifies the "VTIMEZONE" calendar
                                  component
   AEEVPROPERTY_TZNAME          : This property specifies the customary
                                  designation for a time zone description
   AEEVPROPERTY_TZOFFSETFROM    : This property specifies the offset which is
                                  in use prior to this time zone observance
   AEEVPROPERTY_TZOFFSETTO      : This property specifies the offset which is
                                  in use in this time zone observance
   AEEVPROPERTY_TZURL           : The TZURL provides a means for a VTIMEZONE
                                  component to point to a network location that
                                  can be used to retrieve an up-to-date version
                                  of itself
   AEEVPROPERTY_UID             : This property defines the persistent, globally
                                  unique identifier for the calendar component
   AEEVPROPERTY_URL             : This property defines a Uniform Resource
                                  Locator (URL) associated with the iCalendar
                                  object
   AEEVPROPERTY_VERSION         : This property specifies the version of the
                                  calendar or vCard object
   AEEVPROPERTY_XRULE           : This property defines a rule or repeating
                                  pattern for an exception to a recurring
                                  vCalendar entity

   AEEVPROPERTY_ADR             : The property value consists of components of
                                  the address specified as positional fields
                                  separated by the Field Delimiter
   AEEVPROPERTY_AGENT           : This property specifies information about
                                  another person who will act on behalf of the
                                  vCard object
   AEEVPROPERTY_BIRTHDATE       : This property specifies the date of birth of
                                  the individual associated with the vCard
   AEEVPROPERTY_EMAIL           : This property specifies the address for
                                  electronic mail communication with the vCard
                                  object
   AEEVPROPERTY_FN              : This property specifies the formatted name
                                  string associated with the vCard object
   AEEVPROPERTY_KEY             : This property specifies the public encryption
                                  key associated with the vCard object
   AEEVPROPERTY_LABEL           : This property specifies the addressing label
                                  for physical delivery to the person/object
                                  associated with the vCard
   AEEVPROPERTY_LOGO            : This property specifies an image or graphic
                                  of the logo of the organization that is
                                  associated with the individual to which the
                                  vCard belongs
   AEEVPROPERTY_MAILER          : This property parameter specifies the type of
                                  electronic mail software that is in use by the
                                  individual associated with the vCard object
   AEEVPROPERTY_NAME            : This property specifies a structured
                                  representation of the name of the person,
                                  place or thing associated with the vCard
                                  object
   AEEVPROPERTY_NICKNAME        : To specify the text corresponding to the
                                  nickname of the object the vCard represents
   AEEVPROPERTY_NOTE            : To specify supplemental information or a
                                  comment that is associated with the vCard
   AEEVPROPERTY_ORG             : To specify the organizational name and units
                                  associated with the vCard
   AEEVPROPERTY_PHOTO           : This property specifies an image or photograph
                                  of the individual associated with the vCard
   AEEVPROPERTY_REV             : This property specifies the combination of
                                  the calendar date and time of day of the last
                                  update to the vCard object
   AEEVPROPERTY_ROLE            : To specify information concerning the role,
                                  occupation,or business category of the object
                                  the vCard represents
   AEEVPROPERTY_SOUND           : To specify a digital sound content information
                                  that annotates some aspect of the vCard
   AEEVPROPERTY_TEL             : To specify the telephone number for telephony
                                  communication with the object the vCard
                                  represents
   AEEVPROPERTY_TITLE           : To specify the job title, functional position
                                  or function of the object the vCard represents
   AEEVPROPERTY_OID             : The object id internally which BREW identifies
                                  each object uniquely

Comments:
   None

See Also:
   None

===============================================================================
AEEVRRuleValueType

Description:
   Specifies the recurrence rule value type.

Definition:
   
   #define AEEVRRULE_FREQUENCY      0x00000001
   #define AEEVRRULE_UNTIL          0x00000002
   #define AEEVRRULE_COUNT          0x00000003
   #define AEEVRRULE_INTERVAL       0x00000004
   #define AEEVRRULE_BYSECOND       0x00000005
   #define AEEVRRULE_BYMINUTE       0x00000006
   #define AEEVRRULE_BYHOUR         0x00000007
   #define AEEVRRULE_BYDAY          0x00000008
   #define AEEVRRULE_BYMONTHDAY     0x00000009
   #define AEEVRRULE_BYYEARDAY      0x0000000a
   #define AEEVRRULE_BYWEEKNO       0x0000000b
   #define AEEVRRULE_BYMONTH        0x0000000c
   #define AEEVRRULE_BYSETPOS       0x0000000d
   #define AEEVRRULE_WKST           0x0000000e
   #define AEEVRRULE_EXTENDED       0x0000000f

   typedef uint32 AEEVRRuleValueType;


Members: 
   The ids specify the recurrence rule value types.

   AEEVRRULE_FREQUENCY : The FREQ rule part identifies the type of recurrence
                         rule. Valid values include SECONDLY,
                         MINUTELY, HOURLY, DAILY, WEEKLY, MONTHLY, YEARLY.

   AEEVRRULE_UNTIL     : The UNTIL rule part specifies a date-time value which
                         bounds the recurrence rule in an inclusive manner.

   AEEVRRULE_COUNT     : The COUNT rule part specifies the number of occurrences
                         at which to range-bound the recurrence.

   AEEVRRULE_INTERVAL  : The INTERVAL rule part contains a positive integer
                         representing how often the recurrence rule repeats.
                         The default value is "1".

   AEEVRRULE_BYSECOND  : The BYSECOND rule part specifies list of seconds
                         within a minute. Valid values are 0 to 59.

   AEEVRRULE_BYMINUTE  : The BYMINUTE rule part specifies list of minutes 
                         within an hour. Valid values are 0 to 59.

   AEEVRRULE_BYHOUR    : The BYHOUR rule part specifies list of hours of 
                         the day. Valid values are 0 to 23.

   AEEVRRULE_BYDAY     : The BYDAY rule part specifies list of days of the week;
                         MO indicates Monday; TU indicates Tuesday;
                         WE indicates Wednesday; TH indicates Thursday; FR 
                         indicates Friday; SA indicates Saturday; SU 
                         indicates Sunday.
                         For example: BYDAY=1TU,-2SU,2SU,7FR

   AEEVRRULE_BYMONTHDAY: The BYMONTHDAY rule part specifies list of days of the
                         month.

   AEEVRRULE_BYYEARDAY : The BYYEARDAY rule part specifies list of days of the
                         year. Valid values are 1 to 366 or -366 to -1.

   AEEVRRULE_BYWEEKNO  : The BYWEEKNO rule part specifies list of weeks of the 
                         year. Valid values are 1 to 53 or -53 to -1.

   AEEVRRULE_BYMONTH   : The BYMONTH rule part specifies list of months of the 
                         year. Valid values are 1 to 12.

   AEEVRRULE_BYSETPOS  : The BYSETPOS rule part specifies list of values which
                         corresponds to the nth occurrence within the set of
                         events specified by the rule.

   AEEVRRULE_WKST      : The WKST rule part specifies the day on which the
                         workweek starts.
                         Valid values are MO, TU, WE, TH, FR, SA and SU.

   AEEVRRULE_EXTENDED  : Extended case. Specifies text.
  

Comments:
   None

See Also:
   None

================================================================================
AEEVFrequency

Description:
   Specifies the frequency of occurence type in recurrence rule.

Definition:
   #define AEEVFREQUQNCY_INVALID     0x00000001
   #define AEEVFREQUENCY_SECONDLY    0x00000002
   #define AEEVFREQUENCY_MINUTLELY   0x00000003
   #define AEEVFREQUENCY_HOURLY      0x00000004
   #define AEEVFREQUENCY_DAILY       0x00000005
   #define AEEVFREQUENCY_WEEKLY      0x00000006
   #define AEEVFREQUENCY_MONTHLY     0x00000007
   #define AEEVFREQUENCY_YEARLY      0x00000008
     
   typedef uint32 AEEVFrequency;


Members: 

   AEEVFREQUQNCY_INVALID   :  Invalid frequency
   AEEVFREQUENCY_SECONDLY  :  To specify repeating events based on an interval
                              of a second
   AEEVFREQUENCY_MINUTLELY :  To specify repeating events based on an interval
                              of a minute
   AEEVFREQUENCY_HOURLY    :  To specify repeating events based on an interval
                              of an hour
   AEEVFREQUENCY_DAILY     :  To specify repeating events based on an interval
                              of a day or more
   AEEVFREQUENCY_WEEKLY    :  To specify repeating events based on an interval
                              of a week or more
   AEEVFREQUENCY_MONTHLY   :  To specify repeating events based on an interval
                              of a month or mode
   AEEVFREQUENCY_YEARLY    :  To specify repeating events based on an interval
                              of a year or more

Comments:
   None

See Also:
   None

===============================================================================
AEEVMonth

Description:
   Specifies the Month in a year.

Definition:
   #define AEEVMONTH_JAN      0x00000001
   #define AEEVMONTH_FEB      0x00000002
   #define AEEVMONTH_MAR      0x00000004
   #define AEEVMONTH_APR      0x00000008
   #define AEEVMONTH_MAY      0x00000010
   #define AEEVMONTH_JUN      0x00000020
   #define AEEVMONTH_JUL      0x00000040
   #define AEEVMONTH_AUG      0x00000080
   #define AEEVMONTH_SEP      0x00000100
   #define AEEVMONTH_OCT      0x00000200
   #define AEEVMONTH_NOV      0x00000400
   #define AEEVMONTH_DEC      0x00000800

   typedef uint32 AEEVMonth;

Members: 

   AEEVMONTH_JAN  : Month of January
   AEEVMONTH_FEB  : Month of February
   AEEVMONTH_MAR  : Month of March
   AEEVMONTH_APR  : Month of April
   AEEVMONTH_MAY  : Month of May
   AEEVMONTH_JUN  : Month of June
   AEEVMONTH_JUL  : Month of July
   AEEVMONTH_AUG  : Month of August
   AEEVMONTH_SEP  : Month of September
   AEEVMONTH_OCT  : Month of October
   AEEVMONTH_NOV  : Month of November
   AEEVMONTH_DEC  : Month of December

Comments:
   None

See Also:
   None

===============================================================================
AEEVWeekDay

Description:
   Specifies the Day in a week.

Definition:
   #define AEEVDAY_SUN        0x00000001
   #define AEEVDAY_MON        0x00000002
   #define AEEVDAY_TUE        0x00000004
   #define AEEVDAY_WED        0x00000008
   #define AEEVDAY_THU        0x00000010
   #define AEEVDAY_FRI        0x00000020
   #define AEEVDAY_SAT        0x00000040
   
   typedef uint32 AEEVWeekDay;

Members: 
   The ids specify the Day in a week.
   
   AEEVDAY_SUN :  Indicates Sunday
   AEEVDAY_MON :  Indicates Monday
   AEEVDAY_TUE :  Indicates Tuesday  
   AEEVDAY_WED :  Indicates Wednesday
   AEEVDAY_THU :  Indicates Thursday
   AEEVDAY_FRI :  Indicates Friday
   AEEVDAY_SAT :  Indicates Saturday


Comments:
   None

See Also:
   None

===============================================================================
AEEV_NR_WEEKDAYS

Description:
   Specifies the number of Weekdays.
   

Definition:
   #define AEEV_NR_WEEKDAYS   7

Members: 
   The id specify the number of week days.
   
   AEEV_NR_WEEKDAYS   : Signifies number of week days in a week

Comments:
   None

See Also:
   None

===============================================================================
AEEVData

Description:
   Specifies the data for Photo/Logo/Sound/Key/Agent.

Definition:
   typedef struct
   {
      AEEVValueType      nValueType;
      uint32             dwDataSize;
      void               *pData;

   }AEEVData;
 
Members:
   nValueType         [in]: AEEVVALUE_BINARY Specifies pData is Binary data.
                            AEEVVALUE_URI Specifies pData is text value.
                            AEEVVALUE_CARD Specifies pData is pointer to the
                            IvCard.
   dwDataSize         [in]: Size of data in number of bytes.
   pData                  : Actual data provided.

Comments: 
===pre>
   Photo : To specify an image or photograph information that annotates some 
           aspect of the object the vCard represents.
           
           Ex: PHOTO;VALUE=uri:http://www.abc.com/pub/photos/jqpublic.gif.
               nValueTpe = AEEVVALUE_URI;
               dwDataSize and pData is filled according to PHOTO value.

           Ex: PHOTO;ENCODING=b;TYPE=JPEG:MIICajCCAdOgAwIBAgICBEUwDQYJKoZIhvcN
               AQEEBQAwdzELMAkGA1UEBhMCVVMxLDAqBgNVBAoTI05ldHNjYXBlIENvbW11bm
               ljYXRpb25zIENvcnBvcmF0aW9uMRwwGgYDVQQLExNJbmZvcm1hdGlvbiBTeXN0
               <...remainder of "B" encoded binary data...>.
               nValueTpe = AEEVVALUE_BINARY; 
               dwDataSize = size of memory required to store the value
               pData = the actual data itself, NULL terminated

   Agent : Ex: AGENT;VALUE=uri:CID:JQPUBLIC.part3.960129T083020.xyzMail@host3.com
               nValueTpe = AEEVVALUE_URI;
               dwDataSize and pData is filled according to AGENT value.

           Ex: AGENT:BEGIN:VCARD\nFN:Susan Thomas\nTEL:+1-919-555-1234\n
               EMAIL\;INTERNET:sthomas@host.com\nEND:VCARD\n
               nValueTpe = AEEVVALUE_CARD;
               pData is filled with IvCard pointer.
               

    Similarly for LOGO, SOUND & KEY, If ENCODING type is binary then nValueTpe
    is filled with AEEVVALUE_BINARY else 0.
 ===/pre>
   If user passes this function for Parse value, he would get the members filled.
   The user should free this memory once he is done.

See Also:
   None

===============================================================================
AEEVAlarm

Description:
   Specifies the Alarm structure.

Definition:
   typedef struct
   {
      uint32      nRpt_Cnt;
      char        *pszTrigger;
      char        *pszDuration;
      char        *pszStr1;
      char        *pszStr2;

   }AEEVAlarm;    

Members:
   nRpt_Cnt       : Specifies the repeat count, the number of times that the
                    reminder is to be repeated.

   pszTrigger     : Specifies the runtime, the date and time that the reminder
                    is to be executed.

   pszDuration    : Specifies the snooze time, the duration of time after the run
                    time that the reminder is to be dormant prior to being
                    repeated
   pszStr1        : Specifies the audio content if alarm is AALARM specific
                    Specifies the display string if alarm is DALARM specific
                    Specifies the email address if alarm is MALARM specific
                    Specifies the procedure name if alarm is PALARM specific

   pszStr1        : Specifies the Note, the textual reminder string that is to
                    be sent to the email address.

Comments:
   None.

See Also:
   None.
===============================================================================

AEEVDateTimePeriod

Description:
   Specifies the date, date-time, Duration or Period type values.

Definition:
   typedef struct
   {
      AEEVValueType  dwValueType;
      boolean        bIsDuration;
      boolean        bIsPriorDuration;
      boolean        bIsUTC;
      uint32         dwValue1;
      uint32         dwValue2;
      AEEDateTime    sDuration;

   }AEEVDateTimePeriod;   

Members:
   dwValueType       : Specifies the value type.
                       AEEVVALUE_DATE specifies Date type
                       AEEVVALUE_DATE_TIME specifies Date-Time type
                       AEEVVALUE_PERIOD specifies Period type.
                       AEEVVALUE_DURATION specifies duration type.

   bIsDuration       : 1 specifies duration else date-time type. This is valid
                       only if Value is period type
                       (dwValueType = AEEVVALUE_DATE_TIME).

   bIsPriorDuration  : Specifies whether the duration value is prior or not.
                       This will be used for Trigger related properties.

   bIsUTC            : It will be set to 1 if the values (dwValue1 & dwValue2)
                       are in UTC format.

   dwValue1          : Specifies the value in Julian Seconds.

   dwValue2          : Specifies the end date-time in Period type values
                       seperated by solidus character from date-start value 
                       (19960403T020000Z/19960403T040000Z) in Julian Seconds.
                       Julian seconds corresponds to end datetime
                       (19960403T040000Z) will be filled.

   sDuration         : Specifies the duration in Period type values seperated by
                       solidus character from date-start value or Trigger
                       related values. This is valid only if 
                       dwValueType =  AEEVVALUE_PERIOD and bIsDuration = 1 or
                       dwValueType = AEEVVALUE_DURATION and bIsDuration = 1;

Comments:

===pre>
-----------------------------------------------------------------------------------------------------------------------------------------
   |  Value Type        		|  dwValue1(JulSecs) 	| dwValue2(JulSecs)  	|   sDuration        		|
-----------------------------------------------------------------------------------------------------------------------------------------
   |AEEVVALUE_DATE  		|  Filled            		|     X    			|   X                		|
   |                    			|                    		|          			|                    		|
   |AEEVVALUE_DATE_TIME 	|  Filled            		|     X              		|   X                		|
   |                    			|                    		|                    		|                    		|
   |AEEVVALUE_PERIOD    	|  Filled            		| Filled if second   	| Filled if second   	|
   |                    			|                    		| value in period is 	| Value in period is 	|
   |                    			|                    		|  Date type         		|  Duration type     	|
   |                    			|                    		|                    		|                    		|
   |AEEVVALUE_DURATION  	|    X               		|    X               		|    Filled          		|
-----------------------------------------------------------------------------------------------------------------------------------------


   If the value is Date type:
      Ex: RDATE:19970406 then :
      dwValueType = AEEVVALUE_DATE, bIsDuration = 0; 
      dwValue1 will be filled with julian  seconds corresponds to RDATE value.

   If the value is Date-Time type:
      Ex: DTSTART:19980118T230000 then :
      dwValuetype = AEEVVALUE_DATE_TIME, bIsDuration = 0;
      dwValue1 will be filled with seconds corresponds to DTSTART value.

      Ex: TRIGGER;VALUE=DATE-TIME:19970317T133000Z
      dwValuetype = AEEVVALUE_DATE_TIME, bIsDuration = 0; bIsUTC = 1;
      dwValue1 will be filled with julian seconds corresponds to TRIGGER value.

   If the value is Duration type:
      Ex: TRIGGER:-PT30M
      dwValuetype = AEEVVALUE_DURATION, bIsDuration = 1;
      bIsPriorDuration = 1;
      m_JD structure will be filled corresponds to TRIGGER duration value.
      
   If the value is Period type:
      Ex; RDATE;VALUE=PERIOD:19960403T020000Z/19960403T040000Z then:
      dwValueType = AEEVVALUE_PERIOD, bIsDuration = 0; bIsUTC = 1;
      dwvalue1 will be filled with julian seconds corresponds to RDATE start
      date-time value.
      dwValue2 will be filled with julian seconds corresponds to RDATE end
      date-time value.

      Ex: RDATE;VALUE=PERIOD:19960404T010000Z/PT3H then:
      dwValueType = AEEVVALUE_PERIOD, bIsDuration = 1;
      dwvalue1 will be filled with seconds corresponds to RDATE start date-time 
      value.
      m_JD structure will be filled corresponds to RDATE duration value.

   dwValue2 will be considered only if dwValueType = AEEVVALUE_PERIOD &
   bIsDuration = 0;
   m_JD will be considered only if
   dwValueType = AEEVVALUE_PERIOD & bIsDuration = 1 or
   dwValuetype = AEEVVALUE_DURATON & bIsDuration = 1;

   It always assumes that dwValue1 and dwValue2 are in same format.
   If the user wants to generate property with UTC time format then bIsUTC 
   should be set to 1 and dwValue1 filled with corresponding seconds.

   Structure To get DTSTART:19970301T200000Z buffer is:
   bIsUTC = 1, dwValue1 filled with seconds all other filelds are zero.
===/pre>
 
See Also:
   None.

===============================================================================
AEEVUTCDate

Description:
   Specifies UTC date in Julian seconds. In ISO-8601 date format is,
   <year><month><day>T<hour><minute><second><typedesignator> 

Definition:
   typedef struct
   {
      boolean      bIsUTC; 
      uint32       dwJulianSeconds;

   }AEEVUTCDate;
    
Members:
   bIsUTC          : 1 Specifies UTC time else local time.
   dwJulianSeconds : Julian seconds
 
Comments:
   None
 
See Also:
   None
 
===============================================================================
AEEVGeo

Description:
   Specifies the geographical locatoin in Lattitudes and Longitudes.

Definition:
   typedef struct
   {
      doublestruct   nLongitude;
      doublestruct   nLatitude;

   }AEEVGeo;
    
Members:
   nLongitude : Specifies the Longitudinal value.
   nLatitude  : Specifies the lattitudinal value.
 
Comments:
   None
 
See Also:
   None
 
===============================================================================
AEEVRRule

Description:
   Specifies the Recurrence rule.
   The structure used for Recurrence rule supports a subset of
   recurrence rules specified in vCalendar 1.0 & vCalendar 2.0.

   AEEVRRule Usage examples
===pre>
   vCalendar 1.0:

   (I)   To specify the associated event occurs every day:
         VCALENDAR_INITRULE(AEEVRule *recRule);
            nFrequency  = AEEVFREQUENCY_DAILY;
            dwCount     = 0;
   
   (II)  To specify the associated event occurs every day for the next five
         days:
            nFrequency  = AEEVFREQUENCY_DAILY;
            dwCount     = 5;

   (III) To specifiy the event occurs Daily for 10 occurrences:
         D1 #10
            nFrequency  = AEEVFREQUENCY_DAILY;
            dwInterval  = 1;
            dwCount     = 10;

   (IV)  To specify the event occurs Daily until 12/24/94:
         D1 19941224T000000Z
            nFrequency  = AEEVFREQUENCY_DAILY;
            dwInterval  = 1;
            enddate     = ..set the date here.

   (V)   To specify the event occurs Every other day - forever:
         D2 #0
            nFrequency  = AEEVFREQUENCY_DAILY;
            dwInterval  = 2;
            dwCount     = 0;

   (VI)  To specify the event occurs Every 10 days, 5 occruuences:
         D10 #5
            nFrequency  = AEEVFREQUENCY_DAILY;
            dwInterval  = 10;
            dwCount     = 5;

   (VII) To specify the event occurs Weekly for 10 occurrences:
         W1 #10
            nFrequency  = AEEVFREQUENCY_WEEKLY;
            dwInterval  = 1;
            dwCount     = 10;

   (VIII)To specify the event occurs Weekly until 12/24/94:
         W1 19941224T000000Z
            nFrequency  = AEEVFREQUENCY_WEEKLY
            dwInterval  = 1;
            enddate     = ..set the date here.

   (IX)  To specify the event occurs Every other week - forever:
         W2 #0
            nFrequency  = AEEVFREQUENCY_WEEKLY;
            dwInteval   = 2;
            dwCount     = 0;

   (X)   To specify the event occurs Weekly on Tuesday and Thursady for 5 Weeks:
         W1 TU TH #5
            nFrequency  = AEEVFREQUENCY_WEEKLY;
            dwInterval  =  1;
            dwWeekDay   = AEEVDAY_TUE | AEEVDAY_THU;
            dwCount     = 5;

   (XI)  To specify the event occurs Monthly on the 1st Friday for ten
         occurrences:MP1 1+ FR #10
            nFrequency  = AEEVFREQUENCY_MONTHLY;
            dwInterval  = 1;
            pnWeekDayList[5] = (int32 *)MALLOC(2*sizeof(int32));
            *pnWeekDayList[5]        = 1;
            *(pnWeekDayList[5] + 1)  = 0;
            dwCount     = 10;

   (XII) To specify the event occurs Every other month on the 1st and last 
         Sunday of the month for 10 occurrences.
         MP2 1+ Su 1- Su #10
            nFrequency  = AEEVFREQUENCY_MONTHLY;
            dwInterval  = 2;
            pnWeekDayList[0] = (int32 *)MALLOC(3*sizeof(int32));
            *pnWeekDayList[0]= 1;
            *(pnWeekDayList[0] + 1) = -1;
            *(pnWeekDayList[0] + 2) = 0;
            dwCount     = 10;

   (XIII)To specify Monthly on the 1st and last day of the month for 10
         occurrences.
         MD1 1 1- #10
            nFrequency          = AEEVFREQUENCY_MONTHLY;
            dwInterval          = 1;
            pnDayInMonth        = (int32 *)MALLOC(3*sizeof(int32));
            *pnDayInMonth       = 1;
            *(pnDayInMonth + 1) = -1;
            *(pnDayInMonth + 2) = 0;
            dwCount             = 10;


   (XIV) To specify this event occurs every week on Monday and Tuesday:
            nFrequency = AEEVFREQUENCY_WEEKLY;
            dwWeekDay = AEEVDAY_MON | AEEVDAY_TUE;

   (XV)  To specify the associated event occurs every third week on Friday:
            nFrequency  = AEEVFREQUENCY_WEEKLY;
            dwInterval  = 3;
            dwWeekDay = AEEVDAY_FRI;


   
   (XVI) To specify the associated event occurs every month on 10th, 11th,
         and 15th.
            nFrequency   = AEEVFREQUENCY_MONTHLY;
            pnDayInMonth = (int32 *)MALLOC(4*sizeof(int32));
            pnDayInMonth[0] = 10;
            pnDayInMonth[1] = 11;
            pnDayInMonth[2] = 15;
            pnDayInMonth[3] = 0; 

   (XVII)To specify a rule a like "W2 MO WE FR #10", you would say,
            nFrequency  =  AEEVFREQUENCY_WEEKLY;
            dwCount     =  10;
            dwInterval  =  2;
            dwWeekDay   =  AEEVDAY_MON | AEEVDAY_WED | AEEVDAY_FRI;

   (XVIII)To specify a rule like "MD1 12 3- #5", you would fill the structure as 
          follows,
            nFrequency = AEEVFREQUENCY_MONTHLY;
            dwCount    = 5;
            dwInterval = 1;
            pnDayInMonth =(int32 *)MALLOC(3*sizeof(int32));
            pnDayInMonth[0] = -3;
            pnDayInMonth[1] = 12;
            pnDayInMonth[3] = 0; 

   vCalendar 2.0:
   (I)   Daily for 10 occurrences. RRULE:FREQ=DAILY;COUNT=10
            nFrequency = AEEVFREQUENCY_DAILY;
            dwCount    = 10;

   (II)  Daily until December 24, 1997: RRULE:FREQ=DAILY;UNTIL=19971224T000000Z
            nFrequency = AEEVFREQUENCY_DAILY;
            endDate    =  .. //set the date here.

   (III) Every other day - forever:RRULE:FREQ=DAILY;INTERVAL=2
            nFrequency = AEEVFREQUENCY_DAILY
            dwInterval = 2;

   (IV)   Weekly on Tuesday and Thursday for 5 weeks:
         RRULE:FREQ=WEEKLY;COUNT=10;WKST=SU;BYDAY=TU,TH
            nFrequency = AEEVFREQUENCY_WEEKLY;
            dwCount    = 10;
            dwWKST     = AEEVDATY_SUN;
            dwWeekDay  = AEEVDAY_TUE | AEEVDAY_THU; 

   (V)   Every other week on Monday, Wednesday and Friday until December 24:
         RRULE:FREQ=WEEKLY;INTERVAL=2;UNTIL=19971224T000000Z;WKST=SU;BYDAY=MO,
         WE,FR
            nFrequency = AEEVFREQUENCY_WEEKLY;
            dwInterval = 2;
            endDate    = ..//Set the date here from UNTIL.
            dwWKST     = AEEVDAY_SUN;
            dwWeekDay  = AEEVDAY_MON | AEEVDAY_WED | AEEVDAY_FRI;

   (VI)  Monthly on the 1st Friday for ten occurrences:
         RRULE:FREQ=MONTHLY;COUNT=10;BYDAY=1FR
            nFrequency = AEEVFREQUENCY_MONTHLY;
            dwCount    = 10;
            pnWeekDayList[5] = (int32 *)MALLOC(2 * sizeof(int32));
            *pnWeekDayList[5] = 1;
            *(pnWeekDayList[5] + 1) = 0;

   (VII) Every other month on the 1st and last Sunday of the month for 10
         occurrences:RRULE:FREQ=MONTHLY;INTERVAL=2;COUNT=10;BYDAY=1SU,-1SU
            nFrequency = AEEVFREQUENCY_MONTHLY;
            dwInterval = 2;
            dwCount    = 10;
            pnWeekDayList[0] = (int32 *)MALLOC(3 * sizeof(int32));
            *pnWeekDayList[0] = 1;
            *(pnWeekDayList[0] + 1) = -1;
            *(pnWeekDayList[0] + 2) = 0;
           

   (VIII)Monthly on the third to the last day of the month, forever:
         RRULE:FREQ=MONTHLY;BYMONTHDAY=-3.
            nFrequency      = AEEVFREQUENCY_MONTHLY;
            pnDayInMonth    = (int32 *)MALLOC(2 * sizeof(int32));
            pnDayInMonth[0] = -3;
            pnDayInMonth[1] = 0; 

   (IX)  Every 18 months on the 10th thru 15th of the month for 10
         occurrences:
         RRULE:FREQ=MONTHLY;INTERVAL=18;COUNT=10;BYMONTHDAY=10,11,12,13,14,15
            nFrequency      = AEEVFREQUENCY_MONTHLY;
            dwInterval      = 18;
            dwCount         = 10;
            pnDayInMonth    = (int32 *)MALLOC(7* sizeof(int32));
            pnDayInMonth[0] = 10;
            pnDayInMonth[1] = 11;
            pnDayInMonth[2] = 12;
            pnDayInMonth[3] = 13;
            pnDayInMonth[4] = 14;
            pnDayInMonth[5] = 15;
            pnDayInMonth[6] = 0;  

   (X)   Everyday in January, for 3 years:
         RRULE:FREQ=YEARLY;UNTIL=20000131T090000Z;BYMONTH=1;BYDAY=SU,MO,TU,WE,
         TH,FR,SA
            nFrequency        = AEEVFREQUENCY_YEARLY;
            endDate           = ..//Set the date here from UNTIL.
            dwMonthInYear     = 1;
            dwWeekDay         = AEEVDAY_SUN | AEEVDAY_MON | AEEVDAY_TUE |
                                AEEVDAY_WED | AEEVDAY_THU | AEEVDAY_FRI |
                                AEEVDAY_SAT;

   (XI)  Yearly in June and July for 10 occurrences:
         RRULE:FREQ=YEARLY;COUNT=10;BYMONTH=6,7
             nFrequency       = AEEVFREQUENCY_YEARLY;
             dwCount          = 10;
             dwMonthInYear    = AEEVMONTH_JUN | AEEVMONTH_JUL;
  ===/pre>
  
Definition:

   typedef struct
   {
      AEEVFrequency   nFrequency;
      uint32          dwCount;
      uint32          dwInterval;  
      AEEVUTCDate     endDate;
      uint32          dwWeekDay;
      uint32          dwMonthInYear;
      uint32          dwWKST;
      uint32          dwSize;
      uint32          *pnBySecList;
      uint32          *pnByMinList;
      uint32          *pnByHrList;
      int32           *pnWeekDayList[AEEV_NR_WEEKDAYS];
      int32           *pnDayInMonth;
      int32           *pnDayInYear;
      int32           *pnWeekNoList;
      int32           *pnBySpList;

   } AEEVRRule;

Members:

   nFrequency    : Specifies the frequency of occurence.
   dwCount       : Specifies the count of occurence.
   dwInterval    : Specifies the interval. 0 when not specified
   endDate       : End date. 0 when not specified. The UNTIL rule part defines
                   a date-time value which bounds the recurrence rule in an
                   inclusive manner.
   dwWeekDay     : Specifies days in Week.
   dwMonthInYear : Represent months of the year (in logical Bitwise OR).
   dwWKST        : Specifies the day on which the workweek starts. Valid values
                   are MO, TU, WE, TH, FR, SA and SU.
   dwSize        : Size of the structure.
   pnBySecList   : Specifies seperated list of seconds within a minute.
                   NULL terminated.
   pnByMinList   : Specifies seperated list of minutes within a hour.
                   NULL terminated.
   pnByHrList    : specifies seperated list of hours of the day.
                   NULL terminated.
   
   pnWeekDayList : Specifies the list of occurrences of speicific day in week
                   or month or year depends on nFrequency. It is specific to
                   BYDAY rule part.
                   pnWeekdayList[0] : Specific to Sunday.
                   pnWeekdayList[1] : Specific to Monday. Similarly 
                   2nd, 3rd, 4th, 5Th, 6th & 7Th arrays are corresponds to 
                   Tuesday, Wednesday, Thursday, Friday & Saturday respectively.
   pnDayInMonth  : Pointer to an array containing Day in Month , NULL 
                   terminated Changed the above Data Type of pDayInMonth
                   from uint32 to int32 as some times, the rule may be
                   something like "MD1 3- #5" which means third to the last
                   day of the month for 5 occurences for every month. Here 
                   3- would be stored as -3 to indicate the third to the last
                   day of the month. valid values are 1 to 31 or -31 to -1.
   pnDayInYear   : Pointer to an array containing Days in Year, Valid values are
                   1 to 366 or -366 to -1. For example -1 represents the last
                   day of the year. NULL terminated.
   pnWeekNoList  : Specifies the weeks of the year. Valid values are 1 to 53 or
                   -53 to -1. Week number one of the calendar year is the first
                   week which contains at least four (4) days in that calendar
                   year. For example, 3 represents the third week of the year.
                   NULL terminated.

   
   pnBySpList    : Pointe to array containg values which corresponds to the nth
                   occurrence within the set of events specified by the rule.
                   Valid values are 1 to 366 or -366 to -1. NULL terminated.
   

Comments:
   User should free the memory allocated for any members once he is done
 
See Also:
  None
 
============================================================================
   INTERFACE DOCUMENTATION
============================================================================
IvProperty Interface

  Interface to parse the data stream containing vCard or vCalendar properties 
  objects.
  IvProperty - Interface to  IvProperty object.



   Example usage, say if we have buffer as follows,
===pre>
   TEL;TYPE=WORK,VOICE,PREF,MSG:+1-213-555-1234

   Once parsing is done we get the IvProperty object as, pIvProperty interface
   pointer. Some of the operations that can be performed

   Note: For our use case we assume the return values are AEE_SUCCESS and
         pointers passed in or allocated are valid

   //TEL;TYPE=WORK,VOICE,PREF,MSG:+1-213-555-1234
   void Sample_Properties_Usage1(IvProperty *pIvProperty)
   {
      int nResult = AEE_SUCCESS;
      int nNumParms = 0;
      IvParm *pIvParm = NULL;
      AEECLSID nObjectId = 0;
      AEEVProperty nPropId = 0;
      int nBufSizeReq = 0;
      char *pszStr = NULL;

      nResult = IvProperty_GetNumberOfParms(pIvProperty,0,&nNumParms);
      //upon return, nNumParms = 1;


      nResult = IvProperty_GetNumberOfParms(pIvProperty,
                                            AEEVPARM_TYPE,
                                            &nNumParms);
      //upon return, nNumParms=1

      nResult = IvProperty_GetParmAtIndex(pIvProperty, 0, 0,&pIvParm);
      //pIvParm would be pointing to a valid object

      nResult = IvParm_GetNumberOfValues(pIvParm,&nNumParms);
      //on return nNumParms=4

      nResult = IvProperty_GetObjectType(pIvProperty, &nObjectId);
      //upon return nObjectId=AEECLSID_vProperty

      nResult = IvProperty_GetId(pIvProperty,&nPropId);
      //on return nPropId = AEEVPROPERTY_TEL

      nResult = IvProperty_GetPropertyName(pIvProperty,NULL,0,&nBufSizeReq);

      pszStr = MALLOC(nBufSizeReq);

      nResult = IvProperty_GetPropertyName(pIvProperty,
                                           pszStr,
                                           nBufSizeReq,
                                           &nBufSizeReq);
      //upon return pszStr="TEL"

      FREE(pszStr);

      //get the raw buffer
      nResult = IvProperty_ParseValue(pIvProperty,
                                      FALSE,
                                      NULL,
                                      0,
                                      &nBufSizeReq);

      pszStr = MALLOC(nBufSizeReq);

      nResult = IvProperty_ParseValue(pIvProperty,
                                      FALSE,
                                      pszStr,
                                      nBufSizeReq,
                                      &nBufSizeReq);

      //upon return pszStr=+1-213-555-1234

      RELEASEIF(pIvParm);
      FREE(pszStr);
   }
   

   void Sample_Properties_Usage2(IShell *ps)
   {
      int nResult = AEE_SUCCESS;
      IvProperty *pIvProperty = NULL;
      AEEVDateTimePeriod time = {0,};

      nResult = ISHELL_CreateInstance(ps,
                                      AEECLSID_vProperty,
                                      (void **)&pIvProperty);

      //DTSTART:to current time in UTC

      time.dwValue1 = GETUTCSECONDS();
      time.bIsUTC = TRUE;
      time.dwValueType = AEEVVALUE_DATE_TIME;

      nResult = IvProperty_InitProperty(pIvProperty,
                                        AEEVPROPERTY_DTSTART,
                                        NULL,
                                        TRUE,
                                        &time,
                                        sizeof(time),
                                        NULL);


      RELEASEIF(pIvProperty);

      nResult = ISHELL_CreateInstance(ps,
                                      AEECLSID_vProperty,
                                      (void **)&pIvProperty);

      //If the user wants to intialise a property of the following format,
      //N:Public;John;Quinlan;Mr.;Esq.

      nResult = IvProperty_InitProperty(pIvProperty,
                                        0,
                                        "N",
                                        FALSE,
                                        "Public;John;Quinlan;Mr.;Esq.",
                                        STRLEN("Public;John;Quinlan;Mr.;Esq."),
                                        NULL);


      RELEASEIF(pIvProperty);

   }
   
===/pre>

===============================================================================
IvProperty_AddRef()

Description:
   This function is inherited from IQI_AddRef().

See Also:
   IVPROPERTy_Release()

===============================================================================
IvProperty_Release()

Description:
   This function is inherited from IQI_Release().

See Also:
    IvProperty_AddRef()

===============================================================================

IvProperty_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface(). 

===============================================================================
IvProperty_GetObjectType

Description:
   This function returns the AEECLSID of the current object type it is

Prototype:
   int IvProperty_GetObjectType(IvProperty *pIvProperty,
                                AEECLSID   *pnObjType);

Parameters:
   pIvProperty  : Pointert to the IvProperty interface
   pnObjType   : Upon return would contain the class id of the object type

Return Value:
   AEE_SUCCESS   :  If successful in getting the object type
   AEE_EBADPARM  :  If pnObjType is NULL

Comments
   None

Side Effects:
   None

================================================================================
IvProperty_AddParm

Description:
   Add a parameter to a IvProperty object.

Prototype:
   int IvProperty_AddParm(IvProperty  *pIvProperty,
                          IvParm      *pIvParm)
  
Parameters:
   pIvProperty : Pointert to the IvProperty interface
   pIvParm     : The paramter to be added to the property.

Return Value:
   AEE_SUCCESS   :  If the object is added
   AEE_EBADPARM  :  If pIvParm is NULL
   AEE_ENOMEMORY :  If we dont have enough memory to add the new object
   AEE_EFAILED   :  Otherwise

Comments:
   None.

Side Effects:
   Upon success the object's referece count is incremented

===============================================================================
IvProperty_RemoveParm

Description:
   Deletes the provided parameter object from IvProperty

Prototype:
   int IvProperty_RemoveParm(IvProperty *pIvProperty,IvParm *pIvParm);

Parameters:
   pIvProperty :  Pointer to the IvProperty interface object
   pIvParm     :  The parameter object that has to be removed

Return Value:
   AEE_SUCCESS    :  If the object is deleted from the IvProperty list
   AEE_EBADPARM   :  If pIvParm is NULL
 
Comments:
   None

Side Effects:
   None
   
===============================================================================
IvProperty_GetNumberOfParms

Description:
   Retrieves the number of parameters based on the parameter id passed in

   Passing in 0(zero) for nParmId retrieves the total number of parameters

Prototype:
   int IvProperty_GetNumberOfParms(IvProperty     *pIvProperty,
                                   AEEVParameter  nParmId,
                                   int            *pnNumParms);
Parameters:
   pIvProperty : Pointer to IvProperty interface object
   nParmId     : Parameter Id for which the objects cnt has to be retrieved
   pnNumParms  : On return, holds the number of IvParm objects with the
                 specified id

Return Value:
   AEE_SUCCESS  :  If the count is retrieved
   AEE_EBADPARM :  If either nParmId is invalid or pnNumParms is NULL

Comments:
   None

Side Effects:
   None

===============================================================================
IvProperty_GetParmAtIndex

Description:
   Retrieves the IvParm object specified by nParmId and index

   Passing in 0(zero) for the nParmId, retrieves the IvParm object at that index

Prototype:
   int IvProperty_GetParmAtIndex(IvProperty    *pIvProperty,
                                 AEEVParameter nParmId,
                                 int           nIndex,
                                 IvParm        **ppIvParm);

Parameters:
   pIvProperty : Pointer to the interface object
   nParmId     : Parameter Id for which the object has to be retrieved
   nIndex      : The index at which the object has to be retrieved, the index
                 is zero based
   ppIvParm    : Holds a pointer to the returned IvParm object

Return Value:
   AEE_SUCCESS    : If we are able to get the object
   AEE_EBADPARM   : If either nParmId or nIndex is invalid or ppIvParm is NULL
   AEE_ENOMEMORY  : If we are unable to allocate the memory
   AEE_EFAILED    : Otherwise

Comments:
   None

Side Effects:
   The returned object reference count is incremented
   
===============================================================================
IvProperty_GetId

Description:
   Get the property Identifier.

Prototype:
   int IvProperty_GetId(IvProperty *pIvProperty, AEEVProperty *pnPropId);

Parameters:
   pIvProperty : Pointert to the IvProperty interface
   pnPropId    : Upon return contains the property Id

Return Value:
   AEE_SUCCESS    : If we can fill in the property id
   AEE_EBADPARM   : If pnPropId is NULL
   AEE_EFAILED    : Otherwise

Comments:
   None.

Side Effects:
   None.

===============================================================================
IvProperty_InitProperty

Description:
   Initialize a property object, with a given Id or name and Value.
   
Prototype:
   int IvProperty_InitProperty(IvProperty    *pIvProperty,
                               AEEVProperty  nPropId,
                               char          *pszPropName,
                               boolean       bParsedValue,
                               void          *pValue,
                               int           nSize,
                               char          *pszGroup);
  
Parameters:
   pIvProperty   : Pointert to the IvProperty interface
   nPropId       : Property Id, with which this object has to be initialised
   pszPropName   : Property name, with which the user wants to initialize
   bParsedValue  : Indicates whether the passed in value pValue is according to
                   the data types mentioned, or a raw buffer, if raw buffer
                   then it has to be encoded in UTF-8 charset
   pValue        : Property value.
   nSize         : Size of the pValue buffer.
   pszGroup      : Null-terminated property group name(optional).

Return Value:
   AEE_SUCCESS       : If the object was initialized successfully.
   AEE_EUNSUPPORTED  : If the nPropId/pszPropName is not supported.
   AEE_EBADPARM      : If the parameters passed are not correct.
   AEE_ENOMEMORY     : If we filed to allocate memory
   AEE_EFAILED       : Otherwise.

Comments:
   If the user wants to initialize the object using property name then he can
   do so by saying nPropId = 0; 

   Or if the user wants to initialize the object using property Id then he can
   say, pszPropName = NULL

   The property name must be in accordance with the vCal/vCard specification

   If bParsedValue is FALSE, means the user is passing in a raw value encoded
   in UTF-8 charset, which parser wont try to interpret and attach the same
   while generating

   This can be useful in cases like, if the application wants to add a UTF-32
   character to SUMMARY property it can't do so by using expected value, as it
   only takes in AECHAR * it can overcome this by passing in bParsedValue as FALSE
   and give the UTF-8 encoded buffer
   
Side Effects:
   None.

===============================================================================
IvProperty_GetPropertyName

Description:
   Get the property name

Prototype:
   int IvProperty_GetPropertyName(IvProperty *pIvProperty,
                                  char       *pszBuf,
                                  int        nBufSize,
                                  int        *pnBufSizeReq);
  
Parameters:
   pIvProperty   : Pointert to the IvProperty interface
   pszBuf        : Pointer to the buffer where the name will be stored.
   nBufSize      : Size of pszBuf.
   pnBufSizeReq  : If pszBuf is NULL, the size required is returned,
                      
Return Value:
   AEE_SUCCESS           : If the name was stored completely in the buffer
   AEE_EBADPARM          : If the pnBufSizeReq is NULL.

Comments:
   Useful for extended properties, when Parser itself does not know about
   the property.

   If user passes in a valid pszBuf pointer but not of complete size to hold
   the entire buffer we fill in as much as we can, and update the pnBufSizeReq
   with the actual size required and return the value as AEE_SUCCESS

Side Effects:
   None.

===============================================================================
IvProperty_GetGroupName

Description:
   Get the property group-name if it belongs to some group.

Prototype:
   int IvProperty_GetGroupName(IvProperty  *pIvProperty,
                               char        *pszBuf,
                               int         nBufSize,
                               int         *pnBufSizeReq);

Parameters:
   pIvProperty   : Pointert to the IvProperty interface
   pszBuf        : Pointer to the buffer where group name will be stored.
   nBufSize      : size of pszBuf.
   pnBufSizeReq  : If pszBuf is NULL, the size required is returned,
                      
Return Value:
   AEE_SUCCESS           : If the name was stored completely in the buffer
   AEE_ENOTYPE           : If there is no group name assosciated
   AEE_EBADPARM          : If the pnBufSizeReq is NULL.

Comments:
   If user passes in a valid pszBuf pointer but not of complete size to hold
   the entire buffer we fill in as much as we can, and update the pnBufSizeReq
   with the actual size required and return the value as AEE_SUCCESS

Side Effects:
  None.
   
================================================================================
IvProperty_ParseValue

Description:
   Parse the value of the property into predefined data types.

   Or give out the raw data buffer to the apps if bParseData is FALSE

Prototype:
   int IvProperty_ParseValue(IvProperty *pIvProperty,
                             boolean    bParseData,
                             void       *pProperty,
                             int        nSize,
                             int        *pnSizeReq);
  
Parameters:
   pIvProperty  : Pointert to the IvProperty interface
   bParseData   : Indicates whether user wants a raw data buffer or parsed
   pProperty    : Pointer to the buffer where parsed property value will be
                  stored.
   nSize        : Size of pProperty.
   pnSizeReq    : If pProperty is NULL, the size required is returned, ignored,
                  otherwise.

Return Value:
   AEE_SUCCESS          : If the Property value was parsed successfully.
   AEE_EBADPARM         : If pSize is NULL
   AEE_EUNSUPPORTED     : If the value can not be parsed 
   AEE_EBUFFERTOOSMALL  : If the passed in buffer is too small to fill with the
                          parsed value
   AEE_EFAILED          : Otherwise

Comments:
   This function parses the property into more usable data-types.
   For Example,
===pre>
   -----------------------------------------------------------------------
   Property Id               =>       Parsed Result
   ------------------------------------------------------------------------

       AEEVPROPERTY_ALARM              	AEEVAlarm structure
       AEEVPROPERTY_ACTION             	AECHAR * 
       AEEVPROPERTY_ATTACH             	AECHAR * 
       AEEVPROPERTY_ATTENDEE           	AECHAR * 
       AEEVPROPERTY_CALSCALE           	AECHAR *, The only supported is,GREGORIAN
       AEEVPROPERTY_CATEGORIES         	AECHAR * 
       AEEVPROPERTY_CLASS              	AECHAR * 
       AEEVPROPERTY_COMMENT            	AECHAR * 
       AEEVPROPERTY_CONTACT            	AECHAR * 
       AEEVPROPERTY_CREATED            	AEEVDateTimePeriod structure
       AEEVPROPERTY_DALARM             	AEEVAlarm structure
       AEEVPROPERTY_DAYLIGHT           	AECHAR *
       AEEVPROPERTY_DCOMPLETED         	AEEVDateTimePeriod structure
       AEEVPROPERTY_DCREATED           	AEEVDateTimePeriod structure
       AEEVPROPERTY_DESCRIPTION        	AECHAR * 
       AEEVPROPERTY_DTEND              	AEEVDateTimePeriod structure
       AEEVPROPERTY_DTSTAMP            	AEEVDateTimePeriod structure
       AEEVPROPERTY_DTSTART            	AEEVDateTimePeriod structure
       AEEVPROPERTY_DUE                		AEEVDateTimePeriod structure
       AEEVPROPERTY_DURATION           	AEEDateTime structure
       AEEVPROPERTY_EXDATE             	List of AEEVDateTimePeriod structures
       AEEVPROPERTY_EXRULE             	AEEVRRule Structure
       AEEVPROPERTY_EXTENDED           	AECHAR * 
       AEEVPROPERTY_FREEBUSY           	List of AEEVDateTimePeriod structures
       AEEVPROPERTY_GEO                		AEEVGeo Structure
       AEEVPROPERTY_LASTMODIFIED       AEEVDateTimePeriod structure
       AEEVPROPERTY_LOCATION           	AECHAR * 
       AEEVPROPERTY_MALARM             	AEEVAlarm structure
       AEEVPROPERTY_METHOD             	AECHAR * 
       AEEVPROPERTY_ORGANIZER          	AECHAR * 
       AEEVPROPERTY_PALARM             	AEEVAlarm structure
       AEEVPROPERTY_PERCENT            	uint32*
       AEEVPROPERTY_PRIORITY           	uint32*
       AEEVPROPERTY_PRODID             	AECHAR * 
       AEEVPROPERTY_RDATE              	List of AEEVDateTimePeriod structures
       AEEVPROPERTY_RECUR_ID           	AEEVDateTimePeriod structure
       AEEVPROPERTY_RELATED            	AECHAR * 
       AEEVPROPERTY_REPEAT            	 uint32*
       AEEVPROPERTY_RESOURCES          	AECHAR * 
       AEEVPROPERTY_RNUM               	uint32*
       AEEVPROPERTY_RRULE              	AEEVRRule Structure
       AEEVPROPERTY_RSTATUS            	AECHAR * 
       AEEVPROPERTY_SEQUENCE           	uint32*
       AEEVPROPERTY_SORTSTRING  	AECHAR * 
       AEEVPROPERTY_STATUS      		AECHAR *
       AEEVPROPERTY_SUMMARY            	AECHAR * 
       AEEVPROPERTY_TIMEZONE           	int32*
       AEEVPROPERTY_TRANSP             	AECHAR *
       AEEVPROPERTY_TRIGGER            	AEEVDateTimePeriod Structure
       AEEVPROPERTY_TZID               		AECHAR * 
       AEEVPROPERTY_TZNAME             	AECHAR * 
       AEEVPROPERTY_TZOFFSETFROM       int32*
       AEEVPROPERTY_TZOFFSETTO         	int32*
       AEEVPROPERTY_TZURL              	AECHAR * 
       AEEVPROPERTY_UID                		AECHAR * 
       AEEVPROPERTY_URL                		AECHAR * 
       AEEVPROPERTY_VERSION            	AECHAR * 
       AEEVPROPERTY_XRULE              	AEEVRRule Structure
       AEEVPROPERTY_ADR                		AECHAR *
       AEEVPROPERTY_AGENT              	AEEVData
       AEEVPROPERTY_BIRTHDATE          	AECHAR *
       AEEVPROPERTY_EMAIL              	AECHAR * 
       AEEVPROPERTY_FN                 		AECHAR * 
       AEEVPROPERTY_KEY                		AEEVData Structure
       AEEVPROPERTY_LABEL              		AECHAR * 
       AEEVPROPERTY_LOGO               	AEEVData
       AEEVPROPERTY_MAILER             	AECHAR * 
       AEEVPROPERTY_NAME               	AECHAR *
       AEEVPROPERTY_NICKNAME           	AECHAR * 
       AEEVPROPERTY_NOTE               	AECHAR * 
       AEEVPROPERTY_ORG                		AECHAR *   
       AEEVPROPERTY_PHOTO              	AEEVData
       AEEVPROPERTY_REV                		AEEVDateTimePeriod Structure
       AEEVPROPERTY_ROLE               		AECHAR * 
       AEEVPROPERTY_SOUND              	AEEVData Structure
       AEEVPROPERTY_TEL                		AECHAR * 
       AEEVPROPERTY_TITLE              		AECHAR * 
       AEEVPROPERTY_OID                		uint32 *
   ----------------------------------------------------------------------
===/pre>
   This function takes care of the encoding schemes, character sets,etc.
   (i.e. All the different parameters associated with the property) and return
   the result in the above mentioned data-types.

   Since the definition is generic, support for parsing more and more properties
   can be added at later point.

Side Effects:
   None.

See Also:
   None

=============================================================================*/
#endif //AEEIVPROPERTY_H
