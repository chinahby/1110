#ifndef AEEIVPARM_H 
#define AEEIVPARM_H 
/*==============================================================================

File:
   AEEIvParm.h

Services:
   vCard/vCalendar parameter related services.

General Description:
   This file contains common interface for parameters needed by both vCard and
   vCalendar parsers.

Public Classes And Static Functions:
   IvParm

INITIALIZATION AND SEQUENCING REQUIREMENTS:

      (c) COPYRIGHT 2007 QUALCOMM Incorporated.
            All Rights Reserved.
            QUALCOMM Proprietary
==============================================================================*/
#include "AEEIvObject.h"

#define AEEIID_IvParm          0x0105d01e

/*vCard and vCalendar parameters. */
#define AEEVPARM_ALTREP          0x00000001
#define AEEVPARM_CHARSET         0x00000002
#define AEEVPARM_CN              0x00000003
#define AEEVPARM_CUTYPE          0x00000004
#define AEEVPARM_DELEGATED_FROM  0x00000005
#define AEEVPARM_DELEGATED_TO    0x00000006
#define AEEVPARM_DIR             0x00000007
#define AEEVPARM_ENCODING        0x00000008
#define AEEVPARM_EXPECT          0x00000009
#define AEEVPARM_EXTENDED        0x0000000a
#define AEEVPARM_FBTYPE          0x0000000b
#define AEEVPARM_FMTTYPE         0x0000000c
#define AEEVPARM_LANG            0x0000000d
#define AEEVPARM_MEMBER          0x0000000e
#define AEEVPARM_PARTSTAT        0x0000000f
#define AEEVPARM_RANGE           0x00000010
#define AEEVPARM_RELATED         0x00000011
#define AEEVPARM_RELTYPE         0x00000012
#define AEEVPARM_ROLE            0x00000013
#define AEEVPARM_RSVP            0x00000014
#define AEEVPARM_SENTBY          0x00000015
#define AEEVPARM_STATUS          0x00000016
#define AEEVPARM_TYPE            0x00000017
#define AEEVPARM_TZID            0x00000018
#define AEEVPARM_VALUE           0x00000019

typedef uint32 AEEVParameter; 


/* Parameter Value type AEEVValueType */
#define AEEVVALUE_BINARY         0x00000001
#define AEEVVALUE_BOOLEAN        0x00000002
#define AEEVVALUE_CAL_ADDRESS    0x00000003
#define AEEVVALUE_CID            0x00000004
#define AEEVVALUE_DATE           0x00000005
#define AEEVVALUE_DATE_TIME      0x00000006
#define AEEVVALUE_DURATION       0x00000007
#define AEEVVALUE_FLOAT          0x00000008
#define AEEVVALUE_INTEGER        0x00000009
#define AEEVVALUE_PERIOD         0x0000000a
#define AEEVVALUE_RECUR          0x0000000b
#define AEEVVALUE_TEXT           0x0000000c
#define AEEVVALUE_TIME           0x0000000d
#define AEEVVALUE_URI            0x0000000e
#define AEEVVALUE_UTC_OFFSET     0x0000000f
#define AEEVVALUE_URL            0x00000010
#define AEEVVALUE_CARD           0x00000011

typedef uint32 AEEVValueType;


/*Enconding types  AEEVEncoding */
#define AEEVENCODING_BASE64            0x00000001
#define AEEVENCODING_QUOTED_PRINTABLE  0x00000002
#define AEEVENCODING_8_BIT             0x00000003
#define AEEVENCODING_BINARY            0x00000004

typedef uint32 AEEVEncoding;

/* AEEVCharset */
#define AEEVCHARSET_ASCII  0x00000001
#define AEEVCHARSET_UTF8   0x00000002

typedef uint32 AEEVCharset;


/* AEEVRSVP */
#define AEEVRSVP_FALSE  0x00000001
#define AEEVRSVP_TRUE   0x00000002

typedef uint32 AEEVRSVP;

/* AEEVType */
#define AEEVTYPE_DOM      0x00000001
#define AEEVTYPE_INTL     0x00000002  
#define AEEVTYPE_POSTAL   0x00000003
#define AEEVTYPE_PARCEL   0x00000004
#define AEEVTYPE_HOME     0x00000005  
#define AEEVTYPE_PREF     0x00000006
#define AEEVTYPE_WORK     0x00000007
#define AEEVTYPE_VOICE    0x00000008
#define AEEVTYPE_FAX      0x00000009
#define AEEVTYPE_MSG      0x0000000a
#define AEEVTYPE_CELL     0x0000000b
#define AEEVTYPE_PAGER    0x0000000c
#define AEEVTYPE_BBS      0x0000000d
#define AEEVTYPE_MODEM    0x0000000e
#define AEEVTYPE_CAR      0x0000000f  
#define AEEVTYPE_ISDN     0x00000010
#define AEEVTYPE_VIDEO    0x00000011
#define AEEVTYPE_AOL      0x00000012
#define AEEVTYPE_APPLE    0x00000013
#define AEEVTYPE_ATT      0x00000014
#define AEEVTYPE_CIS      0x00000015
#define AEEVTYPE_EWORLD   0x00000016
#define AEEVTYPE_INTERNET 0x00000017
#define AEEVTYPE_IBM      0x00000018
#define AEEVTYPE_MCM      0x00000019
#define AEEVTYPE_PWRSHR   0x0000001a
#define AEEVTYPE_PRODIGY  0x0000001b
#define AEEVTYPE_TLX      0x0000001c
#define AEEVTYPE_X400     0x0000001d
#define AEEVTYPE_WAVE     0x0000001e
#define AEEVTYPE_PCM      0x0000001f
#define AEEVTYPE_AIFF     0x00000020
#define AEEVTYPE_GIF      0x00000021
#define AEEVTYPE_CGM      0x00000022
#define AEEVTYPE_WMF      0x00000023
#define AEEVTYPE_BMP      0x00000024
#define AEEVTYPE_MET      0x00000025
#define AEEVTYPE_PMB      0x00000026
#define AEEVTYPE_DIB      0x00000027
#define AEEVTYPE_PICT     0x00000028
#define AEEVTYPE_TIFF     0x00000029
#define AEEVTYPE_PS       0x0000002a
#define AEEVTYPE_PDF      0x0000002b
#define AEEVTYPE_JPEG     0x0000002c
#define AEEVTYPE_MPEG     0x0000002d
#define AEEVTYPE_MPEG2    0x0000002e
#define AEEVTYPE_AVI      0x0000002f
#define AEEVTYPE_QTIME    0x00000030
#define AEEVTYPE_VCARD    0x00000031

typedef uint32 AEEVType;


/* AEEVRole */
#define AEEVROLE_ATTENDEE           0x00000001
#define AEEVROLE_ORGANIZER          0x00000002
#define AEEVROLE_OWNER              0x00000003
#define AEEVROLE_DELEGATE           0x00000004
#define AEEVROLE_CHAIR              0x00000005
#define AEEVROLE_REQ_PARTICIPANT    0x00000006
#define AEEVROLE_OPT_PARTICIPANT    0x00000007
#define AEEVROLE_NON_PARTICIPANT    0x00000008

typedef uint32 AEEVRole;

#define AEEVSTATUS_ACCEPTED   0x00000000
#define AEEVSTATUS_ACTION     0x00000001
#define AEEVSTATUS_SENT       0x00000002
#define AEEVSTATUS_TENTATIVE  0x00000003
#define AEEVSTATUS_CONFIRMED  0x00000004
#define AEEVSTATUS_DECLINED   0x00000005
#define AEEVSTATUS_COMPLETED  0x00000006
#define AEEVSTATUS_DELEGATED  0x00000007

typedef uint32 AEEVStatus;

/* AEEVExpect */
#define AEEVEXPECT_FYI        0x00000001
#define AEEVEXPECT_REQUIRE    0x00000002
#define AEEVEXPECT_REQUEST    0x00000003
#define AEEVEXPECT_IMMEDIATE  0x00000004

typedef uint32 AEEVExpect;


/* For iCal2.0 AEEVCuType */
#define AEEV_CUTYPE_INDIVIDUAL     0x00000001 
#define AEEV_CUTYPE_GROUP          0x00000002
#define AEEV_CUTYPE_RESOURCE       0x00000003
#define AEEV_CUTYPE_ROOM           0x00000004
#define AEEV_CUTYPE_UNKNOWN        0x00000005

typedef uint32 AEEVCuType;


/*  AEEVFBType */
#define AEEV_FBTYPE_FREE                  0x00000001
#define AEEV_FBTYPE_BUSY                  0x00000002
#define AEEV_FBTYPE_BUSY_UNAVAILABLE      0x00000003
#define AEEV_FBTYPE_BUSY_TENTATIVE        0x00000004

typedef uint32 AEEVFBType;


/*  AEEVPartStat */
#define AEEV_PARTSTAT_NEEDS_ACTION        0x00000001
#define AEEV_PARTSTAT_ACCEPTED            0x00000002
#define AEEV_PARTSTAT_DECLINED            0x00000003
#define AEEV_PARTSTAT_TENTATIVE           0x00000004
#define AEEV_PARTSTAT_DELEGATED           0x00000005
#define AEEV_PARTSTAT_COMPLETED           0x00000006
#define AEEV_PARTSTAT_IN_PROCCESS         0x00000007

typedef uint32 AEEVPartStat;

/*  AEEVRange */
#define AEEV_RANGE_THIS_AND_PRIOR        0x00000001
#define AEEV_RANGE_THIS_AND_FUTURE       0x00000002

typedef uint32 AEEVRange;


/* Alarm trigger relationship*/
#define AEEV_RELATED_START          0x00000001
#define AEEV_RELATED_END            0x00000002

typedef uint32 AEEVRelated;


/*Relationship type */
#define AEEV_RELTYPE_PARENT         0x00000001
#define AEEV_RELTYPE_CHILD          0x00000002
#define AEEV_RELTYPE_SIBLING        0x00000003

typedef uint32 AEEVRelType;


/*=============================================================================
IvParm INTERFACE
==============================================================================*/

#define INHERIT_IvParm(iname)                                                 \
   INHERIT_IvObject(iname);                                                   \
   int (*GetNumberOfValues)(iname *po, int *pnNumVals);                       \
   int (*GetId)(iname *po, AEEVParameter *pnParmId);                          \
   int (*GetParmName)(iname *po,char *pszName,int nBufLen,int *pnBufSizeReq); \
   int (*InitParm)(iname         *po,                                         \
                  AEEVParameter  nParamId,                                    \
                  char           *pszParmName,                                \
                  uint32         dwValue,                                     \
                  AECHAR         *pwszBuf);                                   \
   int (*AddValue)(iname         *po,                                         \
                   uint32        dwParmValue,                                 \
                   AECHAR        *pwszBuf);                                   \
   int (*ParseValue)(iname       *po,                                         \
                     boolean     bParseData,                                  \
                     int         nIndex,                                      \
                     uint32      *pdwValue,                                   \
                     void        *pBuf,                                       \
                     int         nBufSize,                                    \
                     int         *pnBufSizeReq)                              

AEEINTERFACE_DEFINE(IvParm);


static __inline uint32 IvParm_AddRef(IvParm *pIvParm)
{
   return AEEGETPVTBL(pIvParm,IvParm)->AddRef(pIvParm);
}

static __inline uint32 IvParm_Release(IvParm *pIvParm)
{
   return AEEGETPVTBL(pIvParm,IvParm)->Release(pIvParm);
}


static __inline int IvParm_QueryInterface(IvParm    *pIvParm, 
                                          AEEIID    id,
                                          void      **ppo)
{
   return AEEGETPVTBL(pIvParm,IvParm)->QueryInterface(pIvParm, id, ppo);
}

static __inline int IvParm_GetObjectType(IvParm       *pIvParm,
                                         AEECLSID     *pnObjType)
{
   return AEEGETPVTBL(pIvParm, IvParm)->GetObjectType(pIvParm, pnObjType);
}

static __inline int IvParm_GetNumberOfValues(IvParm *pIvParm,
                                             int    *pnNumVals)
{
   return AEEGETPVTBL(pIvParm,IvParm)->GetNumberOfValues(pIvParm,
                                                         pnNumVals);
}

static __inline int IvParm_GetId(IvParm  *pIvParm, AEEVParameter *pnParmId)
{
   return AEEGETPVTBL(pIvParm,IvParm)->GetId(pIvParm, pnParmId);
}

static __inline int IvParm_GetName(IvParm    *pIvParm,
                                   char      *pszName,
                                   int       nBufLen,
                                   int       *pnBufSizeReq)
{
   return AEEGETPVTBL(pIvParm,IvParm)->GetParmName(pIvParm,
                                                   pszName,
                                                   nBufLen,
                                                   pnBufSizeReq);
}

static __inline int IvParm_InitParm(IvParm         *pIvParm,
                                    AEEVParameter  nId,
                                    char           *pszParmName,
                                    uint32         dwValue,
                                    AECHAR         *pwszBuf)
{
   return AEEGETPVTBL(pIvParm,IvParm)->InitParm(pIvParm,
                                                nId,
                                                pszParmName,
                                                dwValue,
                                                pwszBuf);
}

static __inline int IvParm_AddValue(IvParm  *pIvParm, 
                                    uint32  dwParmValue,
                                    AECHAR  *pwszBuf)
{
   return AEEGETPVTBL(pIvParm,IvParm)->AddValue(pIvParm,
                                                dwParmValue,
                                                pwszBuf);
}

static __inline int IvParm_ParseValue(IvParm    *pIvParm,
                                      boolean   bParseData,
                                      int       nIndex,
                                      uint32    *pdwValue,
                                      void      *pBuf,
                                      int       nBufSize,
                                      int       *pnBufSizeReq)
{
   return AEEGETPVTBL(pIvParm,IvParm)->ParseValue(pIvParm,
                                                  bParseData,
                                                  nIndex,
                                                  pdwValue,
                                                  pBuf,
                                                  nBufSize,
                                                  pnBufSizeReq);
}


static __inline IvObject *IvParm_To_IvObject(IvParm *pIvParm)
{
   return (IvObject *)(void *) pIvParm;
}

/*==============================================================================
 DATA STRUCTURE DOCUMENTATION
================================================================================
AEEVParameter

Description:
   Specifies supported parameters.

Definition:
           
   #define AEEVPARM_ALTREP          0x00000001
   #define AEEVPARM_CHARSET         0x00000002
   #define AEEVPARM_CN              0x00000003
   #define AEEVPARM_CUTYPE          0x00000004
   #define AEEVPARM_DELEGATED_FROM  0x00000005
   #define AEEVPARM_DELEGATED_TO    0x00000006
   #define AEEVPARM_DIR             0x00000007
   #define AEEVPARM_ENCODING        0x00000008
   #define AEEVPARM_EXPECT          0x00000009
   #define AEEVPARM_EXTENDED        0x0000000a
   #define AEEVPARM_FBTYPE          0x0000000b
   #define AEEVPARM_FMTTYPE         0x0000000c
   #define AEEVPARM_LANG            0x0000000d
   #define AEEVPARM_MEMBER          0x0000000e
   #define AEEVPARM_PARTSTAT        0x0000000f
   #define AEEVPARM_RANGE           0x00000010
   #define AEEVPARM_RELATED         0x00000011
   #define AEEVPARM_RELTYPE         0x00000012
   #define AEEVPARM_ROLE            0x00000013
   #define AEEVPARM_RSVP            0x00000014
   #define AEEVPARM_SENTBY          0x00000015
   #define AEEVPARM_STATUS          0x00000016
   #define AEEVPARM_TYPE            0x00000017
   #define AEEVPARM_TZID            0x00000018
   #define AEEVPARM_VALUE           0x00000019

   typedef uint32 AEEVParameter;

Members:
   The ids specify parameter types supported in vCalendar and vCard.


   AEEVPARM_ALTREP            :  The parameter specifies a URI that points to
                                 an alternate representation for a textual
                                 property value
   AEEVPARM_CHARSET           :  It is used to identify the default character
                                 set used within the body part.
   AEEVPARM_CN                :  The parameter specifies the common name to be
                                 associated with the calendar user specified by
                                 the property
   AEEVPARM_CUTYPE            :  The parameter identifies the type of calendar
                                 user specified by the property.
   AEEVPARM_DELEGATED_FROM    :  This parameter can be specified on a property
                                 that has a value type of calendar address
   AEEVPARM_DELEGATED_TO      :  This parameter specifies those calendar users
                                 whom have been delegated participation in a
                                 group scheduled event or to-do by the calendar
                                 user specified by the property
   AEEVPARM_DIR               :  The parameter specifies a reference to the
                                 directory entry associated with the calendar
                                 user specified by the property
   AEEVPARM_ENCODING          :  The property parameter identifies the inline
                                 encoding used in a property value
   AEEVPARM_EXPECT            :  To indicate the expectation of the attendee’s
                                 participation by the originator.
   AEEVPARM_EXTENDED          :  The parameter is of a extended type
   AEEVPARM_FBTYPE            :  The parameter specifies the free or busy time
                                 type
   AEEVPARM_FMTTYPE           :  The parameter specifies the content type 
                                 of the referenced object
   AEEVPARM_LANG              :  The parameter identifies the language of the
                                 text in the property or property parameter
                                 value
   AEEVPARM_MEMBER            :  The parameter identifies the groups or list
                                 membership for the calendar user specified by
                                 the property
   AEEVPARM_PARTSTAT          :  The parameter identifies the participation
                                 status for the calendar user specified by the
                                 property value
   AEEVPARM_RANGE             :  The parameter specifies the effective range of
                                 recurrence instances that is specified by the
                                 property
   AEEVPARM_RELATED           :  The parameter specifies whether the alarm will
                                 trigger relative to the start or end of the
                                 calendar component 
   AEEVPARM_RELTYPE           :  The parameter specifies the hierarchical
                                 relationship type of the calendar component
                                 referenced by the property
   AEEVPARM_ROLE              :  The parameter specifies the participation
                                 role for the calendar user specified by the
                                 property in the group schedule calendar
                                 component
   AEEVPARM_RSVP              :  The parameter identifies the expectation of a
                                 reply from the calendar user specified by the
                                 property value
   AEEVPARM_SENTBY            :  The parameter specifies the calendar user 
                                 that is acting on behalf of the calendar user
                                 specified by the property
   AEEVPARM_STATUS            :  Indicates the status of the attendee’s
                                 participation in the event or todo, 
   AEEVPARM_TYPE              :  Specifies the type of the property
   AEEVPARM_TZID              :  To specify the identifier for the time zone
                                 definition for a time component in the
                                 property value
   AEEVPARM_VALUE             :  To explicitly specify the data type format for
                                 a property value


  
Comments:
   None

See Also:
   None

===============================================================================
AEEVValueType

Description:
   Specifies the parameter value types.
  
Definition:
           
   #define AEEVVALUE_BINARY         0x00000001
   #define AEEVVALUE_BOOLEAN        0x00000002
   #define AEEVVALUE_CAL_ADDRESS    0x00000003
   #define AEEVVALUE_CID            0x00000004
   #define AEEVVALUE_DATE           0x00000005
   #define AEEVVALUE_DATE_TIME      0x00000006
   #define AEEVVALUE_DURATION       0x00000007
   #define AEEVVALUE_FLOAT          0x00000008
   #define AEEVVALUE_INTEGER        0x00000009
   #define AEEVVALUE_PERIOD         0x0000000a
   #define AEEVVALUE_RECUR          0x0000000b
   #define AEEVVALUE_TEXT           0x0000000c
   #define AEEVVALUE_TIME           0x0000000d
   #define AEEVVALUE_URI            0x0000000e
   #define AEEVVALUE_UTC_OFFSET     0x0000000f
   #define AEEVVALUE_URL            0x00000010
   #define AEEVVALUE_CARD           0x00000011

   typedef uint32 AEEVValueType;

Members: 
   The ids specify parameter value types supported in vCalendar and vCard

   AEEVVALUE_BINARY        : This value type is used to identify properties
                             that contain a character encoding of inline binary
                             data
   AEEVVALUE_BOOLEAN       : This value type is used to identify properties
                             that contain either a "TRUE" or "FALSE" Boolean
                             value  
   AEEVVALUE_CAL_ADDRESS   : This value type is used to identify properties
                             that contain a calendar user address
   AEEVVALUE_CID           : The property value is the Content-ID for the MIME
                             entity containing the property value
   AEEVVALUE_DATE          : This value type is used to identify values that
                             contain a calendar date
   AEEVVALUE_DATE_TIME     : This value type is used to identify values that
                             specify a precise calendar date and time of day
   AEEVVALUE_DURATION      : This value type is used to identify properties that
                             contain a duration of time
   AEEVVALUE_FLOAT         : This value type is used to identify properties
                             that contain a real number value
   AEEVVALUE_INTEGER       : This value type is used to identify properties
                             that contain a signed integer value
   AEEVVALUE_PERIOD        : This value type is used to identify values that
                             contain a precise period of time
   AEEVVALUE_RECUR         : This value type is used to identify properties
                             that contain a recurrence rule specification
   AEEVVALUE_TEXT          : This value type is used to identify values that
                             contain human readable text
   AEEVVALUE_TIME          : This value type is used to identify values that
                             contain a time of day
   AEEVVALUE_URI           : This value type is used to identify values that
                             contain a uniform resource identifier (URI) type
                             of reference to the property value.
   AEEVVALUE_URIUTC_OFFSET : This value type is used to identify properties
                             that contain an offset from UTC to local time
   AEEVVALUE_URL           : This value type is used to identify values that
                             contain a uniform resource locator (URL) type
                             of reference to the property value.
   AEEVVALUE_CARD          : This value type is used to identify properties
                             that contain nested vCard. Ex.AEEVPROPERTY_AGENT


Comments:
   The definition of each property restricts the value to be one of the
   value data types. The value type for a property will either be specified
   implicitly as the default value type or will be explicitly specified with
   the AEEVPARM_VALUE parameter.

   For example, a AEEVPROPERTY_RDATE property cannot have a combination of DATE-
   TIME and TIME value types.

See Also:
   None

===============================================================================

AEEVEncoding

Description:
   Specifies the encoding types.

Definition:
           
   #define AEEVENCODING_BASE64            0x00000001
   #define AEEVENCODING_QUOTED_PRINTABLE  0x00000002
   #define AEEVENCODING_8_BIT             0x00000003
   #define AEEVENCODING_BINARY            0x00000004

   typedef uint32 AEEVEncoding;

Members: 
   The ids specify encoding types supported in vCalendar and vCard.

   AEEVENCODING_BASE64            : The data is encoded in binary encoding
                                    format, BASE64 
   AEEVENCODING_QUOTED_PRINTABLE  : Indicates the text is Quoted-printable
                                    encoded
   AEEVENCODING_8_BIT             : The text is 8-bit encoded
   AEEVENCODING_BINARY            : The text is of binary encoded format

  

Comments:
   The property parameter identifies the inline encoding used in a property
   value. The default encoding is "8BIT", corresponding to a property value
   consisting of text.
  
See Also:
  None

===============================================================================

AEEVCharset

Description:
   Specifies the character set types.

Definition:
           
   #define AEEVCHARSET_ASCII  0x00000001
   #define AEEVCHARSET_UTF8   0x00000002
  
   typedef uint32 AEEVCharset;

Members: 
   The ids specify character set types supported in vCalendar and vCard

   AEEVCHARSET_ASCII : Text is 7-bit ASCII encoded 
   AEEVCHARSET_UTF8  : Text is UTF-8 encoded


Comments:
   The "charset" Content-Type parameter can be used in MIME transports
   to specify any other IANA registered character set.

See Also:
   None

===============================================================================
AEEVRSVP

Description:
   Specifies the RSVP parameter expectation types.

Definition:
           
   #define AEEVRSVP_FALSE  0x00000001
   #define AEEVRSVP_TRUE   0x00000002
  
   typedef uint32 AEEVRSVP;


Members: 
   The ids specify RSVP types.

   AEEVRSVP_FALSE : Indicates there is no expectation of a reply 
   AEEVRSVP_TRUE  : Indicates there is an expectation of a reply 


Comments:
   The RSVP parameter identifies the expectation of a reply from the calendar
   user specified by the property value. This parameter is used by 
   the "Organizer" to request a participation status reply from an "Attendee" of
   a group scheduled event or to-do.

See Also:
   None

===============================================================================
AEEVType

Description:
   Specifies the value types.

Definition:
           
   #define AEEVTYPE_DOM      0x00000001
   #define AEEVTYPE_INTL     0x00000002  
   #define AEEVTYPE_POSTAL   0x00000003
   #define AEEVTYPE_PARCEL   0x00000004
   #define AEEVTYPE_HOME     0x00000005  
   #define AEEVTYPE_PREF     0x00000006
   #define AEEVTYPE_WORK     0x00000007
   #define AEEVTYPE_VOICE    0x00000008 
   #define AEEVTYPE_FAX      0x00000009 
   #define AEEVTYPE_MSG      0x0000000a
   #define AEEVTYPE_CELL     0x0000000b
   #define AEEVTYPE_PAGER    0x0000000c
   #define AEEVTYPE_BBS      0x0000000d
   #define AEEVTYPE_MODEM    0x0000000e
   #define AEEVTYPE_CAR      0x0000000f  
   #define AEEVTYPE_ISDN     0x00000010
   #define AEEVTYPE_VIDEO    0x00000011
   #define AEEVTYPE_AOL      0x00000012
   #define AEEVTYPE_APPLE    0x00000013
   #define AEEVTYPE_ATT      0x00000014
   #define AEEVTYPE_CIS      0x00000015
   #define AEEVTYPE_EWORLD   0x00000016
   #define AEEVTYPE_INTERNET 0x00000017
   #define AEEVTYPE_IBM      0x00000018
   #define AEEVTYPE_MCM      0x00000019
   #define AEEVTYPE_PWRSHR   0x0000001a
   #define AEEVTYPE_PRODIGY  0x0000001b
   #define AEEVTYPE_TLX      0x0000001c
   #define AEEVTYPE_X400     0x0000001d
   #define AEEVTYPE_WAVE     0x0000001e
   #define AEEVTYPE_PCM      0x0000001f
   #define AEEVTYPE_AIFF     0x00000020
   #define AEEVTYPE_GIF      0x00000021
   #define AEEVTYPE_CGM      0x00000022
   #define AEEVTYPE_WMF      0x00000023
   #define AEEVTYPE_BMP      0x00000024
   #define AEEVTYPE_MET      0x00000025
   #define AEEVTYPE_PMB      0x00000026
   #define AEEVTYPE_DIB      0x00000027
   #define AEEVTYPE_PICT     0x00000028
   #define AEEVTYPE_TIFF     0x00000029
   #define AEEVTYPE_PS       0x0000002a
   #define AEEVTYPE_PDF      0x0000002b
   #define AEEVTYPE_JPEG     0x0000002c
   #define AEEVTYPE_MPEG     0x0000002d
   #define AEEVTYPE_MPEG2    0x0000002e
   #define AEEVTYPE_AVI      0x0000002f
   #define AEEVTYPE_QTIME    0x00000030
   #define AEEVTYPE_VCARD    0x00000031

   typedef uint32 AEEVType;


Members: 
   The ids specify value types.

   AEEVTYPE_DOM      : Indicates a domestic address
   AEEVTYPE_INTL     : Indicates an international address
   AEEVTYPE_POSTAL   : Indicates a postal delivery address
   AEEVTYPE_PARCEL   : Indicates a parcel delivery address
   AEEVTYPE_HOME     : Indicates a home type
   AEEVTYPE_PREF     : Indicates preferred type
   AEEVTYPE_WORK     : Indicates a work type 
   AEEVTYPE_VOICE    : Indicates a voice number 
   AEEVTYPE_FAX      : Indicates a facsimile number
   AEEVTYPE_MSG      : Indicates a messaging service on the number
   AEEVTYPE_CELL     : Indicates a cellular number
   AEEVTYPE_PAGER    : Indicates a pager number
   AEEVTYPE_BBS      : Indicates a bulletin board service number
   AEEVTYPE_MODEM    : Indicates a MODEM number
   AEEVTYPE_CAR      : Indicates a car-phone number
   AEEVTYPE_ISDN     : Indicates an ISDN number
   AEEVTYPE_VIDEO    : Indicates a video-phone number
   AEEVTYPE_AOL      : Indicates America On-Line
   AEEVTYPE_APPLE    : Indicates AppleLink
   AEEVTYPE_ATT      : Indicates AT&T Mail
   AEEVTYPE_CIS      : Indicates CompuServe Information Service
   AEEVTYPE_EWORLD   : Indicates eWorld
   AEEVTYPE_INTERN   : Indicates Internet SMTP
   AEEVTYPE_IBM      : Indicates IBM Mail
   AEEVTYPE_MCM      : Indicates MCI Mail
   AEEVTYPE_PWRSHR   : Indicates PowerShare
   AEEVTYPE_PRODIG   : Indicates Prodigy information service
   AEEVTYPE_TLX      : Indicates Telex number
   AEEVTYPE_X400     : Indicates X.400 service
   AEEVTYPE_WAVE     : Indicates Wave format 
   AEEVTYPE_PCM      : Indicates MIME basic audio type
   AEEVTYPE_AIFF     : Indicates AIFF format
   AEEVTYPE_GIF      : Indicates Graphics Interchange Format
   AEEVTYPE_CGM      : Indicates ISO Computer Graphics Metafile
   AEEVTYPE_WMF      : Indicates MS Windows Metafile
   AEEVTYPE_BMP      : Indicates MS Windows Bitmap
   AEEVTYPE_MET      : Indicates IBM PM Metafile
   AEEVTYPE_PMB      : Indicates IBM PM Bitmap
   AEEVTYPE_DIB      : Indicates MS Windows DIB
   AEEVTYPE_PICT     : Indicates an Apple Picture format
   AEEVTYPE_TIFF     : Indicates a Tagged Image File Format
   AEEVTYPE_PS       : Indicates Adobe PostScript format
   AEEVTYPE_PDF      : Indicates Adobe Page Description Format
   AEEVTYPE_JPEG     : Indicates ISO JPEG format
   AEEVTYPE_MPEG     : Indicates ISO MPEG format
   AEEVTYPE_MPEG2    : Indicates ISO MPEG version 2 format
   AEEVTYPE_AVI      : Indicates Intel AVI format
   AEEVTYPE_QTIME    : Indicates Apple QuickTime format
   AEEVTYPE_VCARD    : Indicates a vCard type


Comments:
   None

See Also:
   None

===============================================================================
AEEVRole

Description:
   Specifies the participation Role for a particular attendee.

Definition:
           
   #define AEEVROLE_ATTENDEE           0x00000001
   #define AEEVROLE_ORGANIZER          0x00000002
   #define AEEVROLE_OWNER              0x00000003
   #define AEEVROLE_DELEGATE           0x00000004
   #define AEEVROLE_CHAIR              0x00000005
   #define AEEVROLE_REQ_PARTICIPANT    0x00000006
   #define AEEVROLE_OPT_PARTICIPANT    0x00000007
   #define AEEVROLE_NON_PARTICIPANT    0x00000008

   typedef uint32 AEEVRole;

Members: 
   The ids specify participation Role.

   AEEVROLE_ATTENDEE          :  Indicates the user as an attendee
   AEEVROLE_ORGANIZER         :  Indicates the user as an organizer
   AEEVROLE_OWNER             :  Indicates the user as an owner
   AEEVROLE_DELEGATE          :  Indicates the user as an delegate
   AEEVROLE_CHAIR             :  Indicates chair of the entity
   AEEVROLE_REQ_PARTICIPANT   :  Indicates the user as an required participant
   AEEVROLE_OPT_PARTICIPANT   :  Indicates the user as an optional participant
   AEEVROLE_NON_PARTICIPANT   :  Indicates the user as an non participant


Comments:
   None

See Also:
   None

===============================================================================
AEEVStatus

Description:
   Specifies the status of the attendee's participation in the event or todo.

Definition:
           
   #define AEEVSTATUS_ACCEPTED   0x00000000
   #define AEEVSTATUS_ACTION     0x00000001
   #define AEEVSTATUS_SENT       0x00000002
   #define AEEVSTATUS_TENTATIVE  0x00000003
   #define AEEVSTATUS_CONFIRMED  0x00000004
   #define AEEVSTATUS_DECLINED   0x00000005
   #define AEEVSTATUS_COMPLETED  0x00000006
   #define AEEVSTATUS_DELEGATED  0x00000007

   typedef uint32 AEEVStatus;

Members: 
   The ids specify attendee participation status.

   AEEVSTATUS_ACCEPTED        :  Indicates todo was accepted by attendee
   AEEVSTATUS_ACTION          :  Indicates event or todo requires action by attendee
   AEEVSTATUS_SENT            :  Indicates event or todo was send out to attendee
   AEEVSTATUS_TENTATIVE       :  Indicates event is tentatively accepted by attendee
   AEEVSTATUS_CONFIRMED       :  Indicates attendee has confirmed their attendance at the event
   AEEVSTATUS_DECLINED        :  Indicates event or todo has been rejected by attendee
   AEEVSTATUS_COMPLETED       :  Indicates todo has been completed by attendee
   AEEVSTATUS_DELEGATED       :  Indicates event or todo has been delegated by the attendee to another

Comments:
   None

See Also:
   None

===============================================================================
AEEVExpect

Description:
   Specifies the participation expectation for a particular attendee.

Definition:
           
   #define AEEVEXPECT_FYI        0x00000001
   #define AEEVEXPECT_REQUIRE    0x00000002
   #define AEEVEXPECT_REQUEST    0x00000003
   #define AEEVEXPECT_IMMEDIATE  0x00000004

   typedef uint32 AEEVExpect;

Members: 
   The ids specify participation expectation.

   AEEVEXPECT_FYI       : Indicates request is for your information
   AEEVEXPECT_REQUIRE   : Indicates presence is definitely required
   AEEVEXPECT_REQUEST   : Indicates presence is being requested 
   AEEVEXPECT_IMMEDIATE : Indicates an immediate response needed


Comments:
   None

See Also:
   None

===============================================================================

AEEVCuType

Description:
   Specifies the Calendar user type.

Definition:
           
   #define AEEV_CUTYPE_INDIVIDUAL     0x00000001 
   #define AEEV_CUTYPE_GROUP          0x00000002
   #define AEEV_CUTYPE_RESOURCE       0x00000003
   #define AEEV_CUTYPE_ROOM           0x00000004
   #define AEEV_CUTYPE_UNKNOWN        0x00000005
     
   typedef uint32 AEEVCuType;

Members: 
   The ids specify Calendar user types. The parameter CUTTYPE identifies the
   type of calendar user specified by the property. If not specified on a
   property that allows this parameter, the default is INDIVIDUAL


   AEEV_CUTYPE_INDIVIDUAL : Indicates an individual
   AEEV_CUTYPE_GROUP      : Indicates a group of individuals
   AEEV_CUTYPE_RESOURCE   : Indicates a physical resource
   AEEV_CUTYPE_ROOM       : Indicates a room resource
   AEEV_CUTYPE_UNKNOWN    : Otherwise not known


Comments:
   None

See Also:
   None

===============================================================================
AEEVFBType

Description:
   Specifies the Free/Busy time type.

Definition:
           
   #define AEEV_FBTYPE_FREE                  0x00000001
   #define AEEV_FBTYPE_BUSY                  0x00000002
   #define AEEV_FBTYPE_BUSY_UNAVAILABLE      0x00000003
   #define AEEV_FBTYPE_BUSY_TENTATIVE        0x00000004
     
   typedef uint32 AEEVFBType;

Members: 
   The ids specify Free/Busy time types.

   AEEV_FBTYPE_FREE              : Indicates the user is free
   AEEV_FBTYPE_BUSY              : Indicates the user is busy
   AEEV_FBTYPE_BUSY_UNAVAILABLE  : Indicates the user is busy and unavailable
   AEEV_FBTYPE_BUSY_TENTATIVE    : Indicates the user is tentatively busy

Comments:
   None

See Also:
   None

===============================================================================
AEEVPartStat

Description:
   Specifies the Participation status.

Definition:
           
   #define AEEV_PARTSTAT_NEEDS_ACTION        0x00000001
   #define AEEV_PARTSTAT_ACCEPTED            0x00000002
   #define AEEV_PARTSTAT_DECLINED            0x00000003
   #define AEEV_PARTSTAT_TENTATIVE           0x00000004
   #define AEEV_PARTSTAT_DELEGATED           0x00000005
   #define AEEV_PARTSTAT_COMPLETED           0x00000006
   #define AEEV_PARTSTAT_IN_PROCCESS         0x00000007

   typedef uint32 AEEVPartStat;

Members: 
   The ids specify specify the participation status for the calendar user
   specified by the property.

   AEEV_PARTSTAT_NEEDS_ACTION : Event needs action
   AEEV_PARTSTAT_ACCEPTED     : Event is accepted
   AEEV_PARTSTAT_DECLINED     : Event is declined
   AEEV_PARTSTAT_TENTATIVE    : Event is tentative
   AEEV_PARTSTAT_DELEGATED    : Event delegated
   AEEV_PARTSTAT_COMPLETED    : Event completed
   AEEV_PARTSTAT_IN_PROCCESS  : Event in process


Comments:
   None

See Also:
   None

===============================================================================
AEEVRange

Description:
   Specifies the Recurrence identifier range.

Definition:
           
   #define AEEV_RANGE_THIS_AND_PRIOR        0x00000001
   #define AEEV_RANGE_THIS_AND_FUTURE       0x00000002

   typedef uint32 AEEVRange;

Members: 
   The ids specify Recurrence identifier range.
   
   AEEV_RANGE_THIS_AND_PRIOR  : To specify all instances prior to the recurrence
                                identifier
   AEEV_RANGE_THIS_AND_FUTURE : To specify the instance specified by the
                                recurrence identifier and all subsequent
                                recurrence instances


Comments:
   None

See Also:
   None

===============================================================================
AEEVRelated

Description:
   Specifies the relationship of the alarm trigger with
   respect to the start or end of the calendar component.

Definition:
           
   #define AEEV_RELATED_START          0x00000001
   #define AEEV_RELATED_END            0x00000002

   typedef uint32 AEEVRelated;

Members: 
   The ids specify alarm trigger relationship.

   AEEV_RELATED_START : Trigger off of start
   AEEV_RELATED_END   : Trigger off of end


Comments:
   None

See Also:
   None

===============================================================================
AEEVRelType

Description:
   Specifies the type of hierarchical relationship associated
   with the calendar component specified by the property.

Definition:
           
   #define AEEV_RELTYPE_PARENT         0x00000001
   #define AEEV_RELTYPE_CHILD          0x00000002
   #define AEEV_RELTYPE_SIBLING        0x00000003
     
   typedef uint32 AEEVRelType;

Members: 
   The ids specify relationship type.

   AEEV_RELTYPE_PARENT    : Parent relationship
   AEEV_RELTYPE_CHILD     : Child relationship
   AEEV_RELTYPE_SIBLING   : Sibling relationship
   
Comments:
   None

See Also:
   None
   
===============================================================================
      INTERFACE DOCUMENTATION
===============================================================================
IvParm Interface

   IvParm interface represents a parameter either in vCard/vCalendar



   Example usage,
===pre>
   Say we have a parameter buffer associated with a property as follows, 

      TYPE=WORK,POSTAL,X-Service

   After parsing we would get a IvParm object pointer pIvParm (say)

   The various operations that can be performed are,

   Note: For our use case we assume the return values from functions are always
         AEE_SUCCESS and pointers passed in or allocated are valid

   void Sample_Parms_Usage1(IvParm *pIvParm)
   {
      int nResult = 0;
      int nNumVals = 0;
      char   *pszBuf = NULL;
      AECHAR *pwszBuf = NULL;
      int nBufSizeReq = 0;
      AEEVParameter nParmId = 0;
      uint32 dwValue = 0;

      nResult = IvParm_GetId(pIvParm, &nParmId);
      //nParmId=AEEVPARM_TYPE

      nResult = IvParm_GetNumberOfValues(pIvParm,&nNumVals);
      //nNumVals=3
      
      //If the user is interested in getting the raw buffer at index 0,
      nResult = IvParm_ParseValue(pIvParm,
                                  FALSE,     //wants raw buffer
                                  0,         //at index 0
                                  &dwValue,
                                  NULL,
                                  0,
                                  &nBufSizeReq);

      //upon return nBufSizeReq=5
      pszBuf = MALLOC(nBufSizeReq);
      if( NULL == pszBuf ){
         return;
      }

      nResult = IvParm_ParseValue(pIvParm,
                                  FALSE,
                                  0,
                                  &dwValue,
                                  (void *)pszBuf,
                                  nBufSizeReq,
                                  &nBufSizeReq);

      //upon return pBuf="WORK"
      
      //if the user wants parsed value data at index 2 (X-Service)
      nResult = IvParm_ParseValue(pIvParm,
                                  TRUE,
                                  2,
                                  &dwValue,
                                  NULL,
                                  0,
                                  &nBufSizeReq);

      //means we have value which isn't of standard type
      if( AEE_SUCCESS == nResult && 0 == dwValue  ){

         pwszBuf = MALLOC(nBufSizeReq);
         if( NULL == pwszBuf ){
            FREE(pszBuf);
            return;
         }

         nResult = IvParm_ParseValue(pIvParm,
                                     TRUE,
                                     2,
                                     &dwValue,
                                     pwszBuf,
                                     nBufSizeReq,
                                     &nBufSizeReq);

         //upon return pwszBuf would be pointing to  wide string of "X-Service"

      }
      
      //if the user is interested in getting the parsed data at index 1
      nResult = IvParm_ParseValue(pIvParm,
                                  TRUE,
                                  1,
                                  &dwValue,
                                  NULL,
                                  0,
                                  &nBufSizeReq);

      //upon return dwValue= AEEVTYPE_POSTAL


      FREE(pszBuf);
      FREE(pwszBuf);

      return;

   }
   
   

   //if the user wants to generate a buffer of the following type in a property
   //then he can do as follows
   //    TYPE=CELL,HOME,X-MyPlace

   void Sample_Parms_Usage2(IShell *ps)
   {
      IvParm *pIvParm = NULL;
      int nResult = AEE_SUCCESS;
      int nNumVals = 0;
      uint32 dwValue = 0;
      int nBufSizeReq = 0;
      char *pszBuf = "X-MyPlace";
      AECHAR *pwszBuf = NULL;

      if( NULL == ps ) {
         return;
      }

   do {
         nResult = ISHELL_CreateInstance(ps, AEECLSID_vParm, (void **)&pIvParm);
         BREAKIF(nResult);

         nResult = IvParm_InitParm(pIvParm,
                                   AEEVPARM_TYPE,
                                   NULL,
                                   AEEVTYPE_CELL,
                                   NULL);
         BREAKIF(nResult);

         nResult = IvParm_AddValue(pIvParm,AEEVTYPE_HOME,NULL);
         BREAKIF(nResult);

         pwszBuf = MALLOC ( (STRLEN(pszBuf) + 1) * 2 );

         (void)STRTOWSTR(pszBuf, pwszBuf, (STRLEN(pszBuf) + 1)*2 );

         nResult = IvParm_AddValue(pIvParm,0,pwszBuf);
         BREAKIF(nResult);

         //upon return this would be, nNumVals =3
         nResult = IvParm_GetNumberOfValues(pIvParm,&nNumVals);
         BREAKIF(nResult);
         
        //parse value of index 1
        //upon return dwValue = AEEVTYPE_HOME
        nResult = IvParm_ParseValue(pIvParm,
                                    TRUE,
                                    1,
                                    &dwValue,
                                    NULL,
                                    0,
                                    &nBufSizeReq);
        BREAKIF(nResult);


   }while(0);

   RELEASEIF(pIvParm);
   FREEIF(pwszBuf);
 }
  
===/pre>
   
===============================================================================
IvParm_AddRef()

Description:
   This function is inherited from IQI_AddRef().

See Also:
   IvParm_Release()

===============================================================================
IvParm_Release()

Description:
   This function is inherited from IQI_Release().

See Also:
    IvParm_AddRef()

===============================================================================
IvParm_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface(). 

================================================================================
IvParm_GetObjectType

Description:
   This function returns the AEEVObject id

Prototype:
   int IvParm_GetObjectType(IvParm       *pIvParm,
                            AEECLSID     *pnObjType);

Parameters:
   pIvParm      : Pointer to the IvObject interface
   pnObjType    : Upon return, tells the object type

Return Value:
   AEE_SUCCESS   : If we are able to get the object type
   AEE_EBADPARM  : If pnObjType is NULL

Comments:
   None

Side Effects:
   None

================================================================================
IvParm_GetNumberOfValues

Description:
   Get the number of values associated with a parameter

Prototype:
   int IvParm_GetNumberOfValues(IvParm  *pIvParm,
                                int     *pnNumVals);

Parameters:
    pIvParm    : Pointer to the IvParm interface object.
    pnNumVals  : Pointer to number of values returned.

Return Value:
   AEE_SUCCESS   : If the interface was able to get the number of values
   AEE_EBADPARM  : If the pnNumVals is NULL.
   AEE_EFAILED   : Otherwise.

Comments:
   None

Side Effects:
   None

================================================================================
IvParm_GetId

Prototype:
   int IvParm_GetId(IvParm *pIvParm, AEEVParameter *pnParmId);
  
Description:
   Get the Parameter Identifier
   
Parameters:
   pIvParm  : Pointer to the IvParm interface object.
   pnParmId : Upon return would contain the parameter id

Return Value:
   Returns the AEE return value

   AEE_SUCCESS  :  If we are able to get the parameterId
   AEE_EBADPARM :  If pnParmId is NULL
   AEE_EFAILED  :  Otherwise

Comments:
   None.

Side Effects:
   None.

================================================================================
IvParm_GetName

Prototype:
   int IvParm_GetName(IvParm     *pIvParm,
                      char       *pszName,
                      int        nBufLen,
                      int        *pnBufSizeReq);

Description:
   Get the parameter name

Parameters:
   pIvParm     : Pointer to the IvParm interface object
   pszName     : To be filled with the name of the parameter
   nBufLen     : Size of the name buffer
   pnBufLenReq : Indicates the size of the buffer

Return Value:
   AEE_SUCCESS    : If we are able to get the buffer name
   AEE_EBADPARM   : If the input parms are invalid

Comments:
   While returning buffers, if user passes in a valid pBuf pointer but not of
   complete size to hold the complete buffer we fill in as much as we can,
   and update the pnBufSizeReq with the actual size required and return the
   value as AEE_SUCCESS
   

================================================================================
IvParm_InitParm

Description:
   Initialize a IvParm object, with specified Id or an extended name

Prototype:
   int IvParm_InitParm(IvParm         *pIvParm,
                       AEEVParameter  nParmId,
                       char           *pszParmName,
                       uint32         dwValue,
                       AECHAR         *pwszBuf);


Parameters:
   pIvParm     : Pointer to the IvParm interface object.
   nParmId     : The parameter Id.
   pszParmName : Name of the extended parameter
   dwValue     : The parameter value, if not valid should be set 0
   pwszBuf     : Buffer for param values which are not listed
                 or which are extended

Return Value:
   AEE_SUCCESS   : If the function was able to fill the buffer completely.
   AEE_ENOMEMORY : If the parser isn't able to allocate memory
   AEE_EBADPARM  : If nParmId-nValue is not a valid pair.
   AEE_EFAILED   : Otherwise.

Comments:
   After creating a IvParm object, user can use this interface to define the
   parameter. If the value is of pre-defined type then the user can give the
   value directly in dwValue, else he has to fill in pwszBuf

===pre>
    E.g., If the user wants to intialise this paramter with TYPE id and give it
    value of HOME, he can say that as, 
    
       nResult = IvParm_InitParm(pIvParm,
                                 AEEVPARM_TYPE,
                                 NULL,
                                 AEEVTYPE_HOME,
                                 NULL);

   If the values isn't predefined, like LANGUAGE, and user wants to assosciate,
   "en:German" as its value, then he can say as,

   pwszBuf points to wide string of "en:German"

   nResult = IvParm_InitParm(pIvParm,
                             AEEVPARM_LANG,
                             NULL,
                             0,
                             pwszBuf);

   If the user wants to intialiase a parameter with an extended value he can so,

   If user wants to assign "GUEST" to role parameter, which isn't listed in
   AEEVRRole types then he can do as follows,

   pwszBuf points to wide string of "X-Guest"

   nResult = IvParm_InitParm(pIvParm,
                             AEEVPARM_ROLE,
                             NULL,
                             0,
                             pwszBuf);
===/pre>
Side Effects:
   None

================================================================================
IvParm_AddValue

Description:
   Add a value to IvParm object.

Prototype:
   int IvParm_AddValue(IvParm      *pIParm,
                       uint32      dwParmValue,
                       AECHAR      *pwszBuf);
   
Parameters:
   pIvParm     : Pointer to the IvParm interface object.
   dwParmValue : The parameter value.
   pwszBuf     : Params which are not listed or which are extended 
                 (required for vCal version 2.0)

Return Value:
   AEE_SUCCESS   : If the parameter was added successfully
   AEE_EBADPARM  : If nParmValue is not valid value
   AEE_ENOMEMORY : If we are not able to allocate memory 
   AEE_EFAILED   : Otherwise

Comments:
   In vCal 1.0/vCard 2.1/vCard 3.0 all the parameter values are predefined.
   But in vCal 2.0 parameter value can have predefined value or some string.
   If we want to add   predefined value we will set parameter value to nValue
   else we pass it as string in pwszBuf.

   All the encodings shall be in UTF-8 format

===pre>
   For example if we want to set parametr value "WORK" to "Type",the call would
   be,

      nResult = IvParm_AddValue(pIvParm,
                                AEEVTYPE_WORK,
                                NULL);

   If the user want's to set "MAILTO:server@host.com" value to "DELEGATED-TO"
   parameter then, 

   pwszBuf points to wide string of "MAILTO:server@host.com"

   nResult = IvParm_AddValue(pIvParm,
                             0,
                             pwszBuf);

   
===/pre>
  
Side Effects:
   None

================================================================================
IvParm_ParseValue

Description:
   Get the value associated with a parameter either raw buffer or parsed value

   If the user asks for raw value, the buffer returned would be of UTF-8

   If the user asks for a parse value and the value isn't of a standard type
   the returned buffer would be AECHAR

Prototype:
   int IvParm_ParseValue(IvParm    *pIvParm,
                         boolean   bParseData,
                         int       nIndex,
                         uint32    *pdwValue,
                         void      *pBuf,
                         int       nBufSize,
                         int       *pnBufSizeReq);

Parameters:
  IvParm       : Pointer to the IvParm interface object.
  bParsedData  : Indicates whether the user wants a raw value of the buffer or
                 a parsed data
  nIndex       : The index of the value that has to be retrieved, the index is
                 zero based
  pdwValue     : Pointer to the value where it will be stored.
                 This is one of the enums.
  pBuf         : Pointer to the param value if param is from unhandled
                 list or param value is EXTENDED
  nBufSize     : size of pBuf.
  pnBufSizeReq : Required pBuf Size.

Return Value:
  AEE_SUCCESS         : If the function was able to fill the value.
  AEE_EBADPARM        : If either pdwValue or pnBufSizeReq is NULL
  AEE_EFAILED         : Otherwise.

Comments:
   While returning buffers, if user passes in a valid pBuf pointer but not of
   complete size to hold the complete buffer we fill in as much as we can,
   and update the pnBufSizeReq with the actual size required and return the
   value as AEE_SUCCESS

Side Effects:
  None

=============================================================================*/
#endif //AEEIVPARM_H 

