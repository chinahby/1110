#ifndef AEEPOSDET_H
#define AEEPOSDET_H /* #ifndef AEEPOSDET_H */
/*============================================================================

FILE:  AEEPosDet.h

SERVICES:
        Posision determination APIs

GENERAL DESCRIPTION:
        IPosDet Interface provides services for position determination using GPS 
        information or sector information. 

REVISION HISTORY:

============================================================================

               Copyright © 1999-2002 QUALCOMM Incorporated
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

============================================================================*/

#include "AEE.h"
#include "AEENet.h"
#include "AEE64structs.h"

/* Sector Based position determination info */
typedef struct AEESectorInfo
{
   uint16 wSysID;             /* System Identification */
   uint16 wNetID;             /* Network Identification */
   uint16 wBaseID;            /* Base Station Identification */
   uint16 wBaseClass;         /* Base Station Class */
   uint16 wBestPN;            /* Best Pilot */
   uint16 wPacketZoneID;      /* Packet Data Service Zone Identifier */
   uint16 wMobileCountryCode; /* Mobile country code */
   int32 dwBaseLatitude;      /* Base Station Latitude */
   int32 dwBaseLongitude;     /* Base Station Longitude */
} AEESectorInfo;
typedef  uint16      AEEGPSMode;

#define  AEEGPS_MODE_ONE_SHOT             0x0001
#define  AEEGPS_MODE_DLOAD_FIRST          0x0002
#define  AEEGPS_MODE_TRACK_LOCAL          0x0004
#define  AEEGPS_MODE_TRACK_NETWORK        0x0008
#define  AEEGPS_MODE_TRACK_OPTIMAL        0x0009
#define  AEEGPS_MODE_TRACK_STANDALONE     0x000A
#define  AEEGPS_MODE_DEFAULT              AEEGPS_MODE_ONE_SHOT

typedef uint8        AEEGPSOpt;

#define  AEEGPS_OPT_NONE         0x00
#define  AEEGPS_OPT_SPEED        0x01
#define  AEEGPS_OPT_ACCURACY     0x02
#define  AEEGPS_OPT_PAYLOAD      0x03
#define  AEEGPS_OPT_DEFAULT      AEEGPS_OPT_NONE

typedef uint32        AEEGPSSvrType;

#define  AEEGPS_SERVER_DEFAULT   0
#define  AEEGPS_SERVER_IP        0x00000001
#define  AEEGPS_SERVER_DBURST    0x00000002

typedef struct _AEEGPSIPServer {
   INAddr      addr;
   INPort      port;
   uint16      reserved;
} AEEGPSIPServer;

typedef struct _AEEGPSServer{
   AEEGPSSvrType  svrType;
   union {
      AEEGPSIPServer ipsvr;
   } svr;
} AEEGPSServer;

typedef uint8     AEEGPSQos;

typedef struct _AEEGPSConfig {
   AEEGPSMode        mode;      //Operation type requested
   uint16            nFixes;    //Estimated number of fixes
   uint16            nInterval; //Estimated interval between fixes (seconds)
   AEEGPSOpt         optim;     //Optimization (speed/accuracy)
   AEEGPSQos         qos;       //Quality of Service
   AEEGPSServer      server;    //Server type
} AEEGPSConfig;

typedef  uint8    AEEGPSReq;

#define  AEEGPS_GETINFO_NONE       0x00      /* Acceptable with AEEGPS_MODE_DLOAD_FIRST */
#define  AEEGPS_GETINFO_LOCATION   0x01
#define  AEEGPS_GETINFO_VELOCITY   0x02
#define  AEEGPS_GETINFO_ALTITUDE   0x04

#define  AEEGPS_GETINFO_VERSION_MASK  0xE0
#define  AEEGPS_GETINFO_VERSION_0  0x00
#define  AEEGPS_GETINFO_VERSION_1  0x20

typedef  uint8    AEEGPSInfoVersion;

typedef  uint8    AEEGPSAccuracy;

#define  AEEGPS_ACCURACY_LEVEL1    1
#define  AEEGPS_ACCURACY_LEVEL2    2
#define  AEEGPS_ACCURACY_LEVEL3    3
#define  AEEGPS_ACCURACY_LEVEL4    4
#define  AEEGPS_ACCURACY_LEVEL5    5
#define  AEEGPS_ACCURACY_LEVEL6    6
#define  AEEGPS_ACCURACY_LOWEST    AEEGPS_ACCURACY_LEVEL1
#define  AEEGPS_ACCURACY_HIGHEST   AEEGPS_ACCURACY_LEVEL6


#define  AEEGPS_VALID_LAT     0x0001
#define  AEEGPS_VALID_LON     0x0002
#define  AEEGPS_VALID_ALT     0x0004
#define  AEEGPS_VALID_HEAD    0x0008
#define  AEEGPS_VALID_HVEL    0x0010
#define  AEEGPS_VALID_VVEL    0x0020
#define  AEEGPS_VALID_HUNC    0x0040
#define  AEEGPS_VALID_AUNC    0x0080
#define  AEEGPS_VALID_PUNC    0x0100
#define  AEEGPS_VALID_VUNC    0x0200
#define  AEEGPS_VALID_UTCOFFSET 0x0400
#define  AEEGPS_VALID_VERSION 0x0800
#define  AEEGPS_VALID_TIMEMS  0x1000

#define  AEEGPS_VALID_MASK    ( (AEEGPS_VALID_LAT) | (AEEGPS_VALID_LON) \
                              | (AEEGPS_VALID_ALT) | (AEEGPS_VALID_HEAD) \
                              | (AEEGPS_VALID_HVEL) | (AEEGPS_VALID_VVEL) \
                              | (AEEGPS_VALID_HUNC) | (AEEGPS_VALID_AUNC) \
                              | (AEEGPS_VALID_PUNC) | (AEEGPS_VALID_VUNC) \
                              | (AEEGPS_VALID_UTCOFFSET) | (AEEGPS_VALID_VERSION) \
                              | (AEEGPS_VALID_TIMEMS) )

#define  AEEGPS_ERR_NO_ERR              0
#define  AEEGPS_ERR_BASE                0x00100000
#define  AEEGPS_ERR_GENERAL_FAILURE    (AEEGPS_ERR_BASE + 1)
#define  AEEGPS_ERR_TIMEOUT            (AEEGPS_ERR_BASE + 2)
#define  AEEGPS_ERR_ACCURACY_UNAVAIL   (AEEGPS_ERR_BASE + 3)
#define  AEEGPS_ERR_INFO_UNAVAIL       (AEEGPS_ERR_BASE + 4)
#define  AEEGPS_ERR_PRIVACY_REFUSED    (AEEGPS_ERR_BASE + 5)
#define  AEEGPS_ERR_SRV_UNREACHABLE    (AEEGPS_ERR_BASE + 6)
#define  AEEGPS_ERR_LINK_FAILED        (AEEGPS_ERR_BASE + 7)
#define  AEEGPS_ERR_REJECTED           (AEEGPS_ERR_BASE + 8)
#define  AEEGPS_ERR_OUTOF_RESOURCES    (AEEGPS_ERR_BASE + 9)
#define  AEEGPS_ERR_STALE_BS_INFO      (AEEGPS_ERR_BASE + 10)
#define  AEEGPS_ERR_RECEIVER_BUSY      (AEEGPS_ERR_BASE + 11)

#define  AEEGPS_METHOD_LOCAL        0x00000001
#define  AEEGPS_METHOD_NETWORK      0x00000002
#define  AEEGPS_METHOD_ASSISTED     0x00000004
#define  AEEGPS_METHOD_STANDALONE   0x00000008
#define  AEEGPS_METHOD_SATELLITE    0x00000010
#define  AEEGPS_METHOD_CELLID       0x00000020
#define  AEEGPS_METHOD_AFLT         0x00000040

typedef uint32 AEEGPSMethod;

typedef struct _AEEGPSInfo {
   uint32             dwTimeStamp;          // Time, seconds since 1/6/1980
   uint32             status;               // Response status;
   int32              dwLat;                // Lat, 180/2^25 degrees, WGS-84 ellipsoid
   int32              dwLon;                // Lon, 360/2^26 degrees, WGS-84 ellipsoid
   int16              wAltitude;            // Alt, meters, WGS-84 ellipsoid
   uint16             wHeading;             // Heading, 360/2^10 degrees
   uint16             wVelocityHor;         // Horizontal velocity, 0.25 meters/second
   int8               bVelocityVer;         // Vertical velocity, 0.5 meters/second
   AEEGPSAccuracy     accuracy;             // Accuracy of the data
   uint16             fValid;               // Flags indicating valid fields in the struct.
   uint8              bHorUnc;              // Horizontal uncertainity
   uint8              bHorUncAngle;         // Hor. Uncertainity at angle
   uint8              bHorUncPerp;          // Hor. uncertainty perpendicular
   AEEGPSInfoVersion  Version;              // Version number of this structure
   uint16             wVerUnc;              // Vertical uncertainity.
   /* AECLSID_POSDET_312 end */
   int8               GPS_UTCOffset;        // Time difference in seconds between UTC and GPS satellite time
   byte               LocProbability;       // Probability that user's actual position is within 
                                            // the described ellipse, expressed as a percentage 
   AEEGPSMethod       method;               // Bitmask providing information about the positioning 
                                            // method used to calculate the location
   uint32             dwTimeMS;             // To be used with dwTimeStamp. Milliseconds info 
                                            // over the seconds reported by dwTimeStamp
} AEEGPSInfo;

typedef struct _AEEPositionInfoEx {
   uint32               dwSize;
   flg                  fLatitude:1;
   flg                  fLongitude:1;
   flg                  fAltitude:1;
   flg                  fHeading:1;
   flg                  fHorVelocity:1;
   flg                  fVerVelocity:1;
   flg                  fHorUnc:1;           /* valid : HorUncAngle, HorUnc, HorUncPerp */
   flg                  fAltitudeUnc:1;
   flg                  fUTCOffsetValid:1;

   double               Latitude;            /* in degrees */
   double               Longitude;           /* in degrees */
   int                  nAltitude;           /* in meters */
   double               Heading;             /* degress from true North. */
   double               HorVelocity;         /* meters/second. */
   double               VerVelocity;         /* meters/second. */

   double               HorUncAngle;         /* Angle of axis with respect
                                             ** to true North for position
                                             ** uncertainity. */
   double               HorUnc;              /* Uncertainity along the axis
                                             ** specified with the HorUncAngle. */
   double               HorUncPerp;          /* Uncertainity along the axis
                                             ** axis perpendicular to HorUncAngle. */

   double               AltitudeUnc;         /* Uncertainity in altitude */

/* AECLSID_POSDET_312 end */

   int8                 GPS_UTCOffset;       /* Time difference in seconds between UTC and GPS 
                                             ** satellite time */
   byte                 LocProbability;      /* Probability that user's actual position 
                                             ** is within the described ellipse, expressed 
                                             ** as a percentage */
   AEEGPSMethod         method;              /* Bitmask providing information about the positioning 
                                             ** method used to calculate the location */
} AEEPositionInfoEx;

typedef  uint8    AEEOrientationReq;
#define  AEEORIENTATION_REQ_AZIMUTH   0x01

typedef struct _AEEOrientationAzimuthInfo {
  uint16             wSize;           // sizeof( AEEOrientationAzimuthInfo ) in bytes.
  uint16             wAzimuth;        // Angle 0 - 359 degress and 59 arcminutes.
                                      // bits 0-5 contain arcminutes
                                      // bits 6-15 contain degrees.
                                      // This is the heading angle in the local
                                      // horizontal plane measured clockwise from
                                      // true North.
  uint32             dwTimeStamp;     // Time, seconds since 1/6/1980
} AEEOrientationAzimuthInfo;

typedef struct IPosDet IPosDet;

#if (__ARMCC_VERSION >= 200000) && (!defined (__APCS_ADSABI))

typedef struct _AEEPositionInfoEx_ALIGN_32 {
   uint32               dwSize;
   flg                  fLatitude:1;
   flg                  fLongitude:1;
   flg                  fAltitude:1;
   flg                  fHeading:1;
   flg                  fHorVelocity:1;
   flg                  fVerVelocity:1;
   flg                  fHorUnc:1;           
   flg                  fAltitudeUnc:1;
   flg                  fUTCOffsetValid:1;

   doublestruct         Latitude;
   doublestruct         Longitude;
   int                  nAltitude;
   doublestruct         Heading;
   doublestruct         HorVelocity;
   doublestruct         VerVelocity;

   doublestruct         HorUncAngle;
   
   doublestruct         HorUnc;
   
   doublestruct         HorUncPerp;
   
   doublestruct         AltitudeUnc;
   
   int8                 GPS_UTCOffset;
   
   byte                 LocProbability;
   
   AEEGPSMethod         method;
} AEEPositionInfoEx_ALIGN_32;

static __inline int AEEPositionInfoEx_ALIGN_32_to_AEEPositionInfoEx(AEEPositionInfoEx_ALIGN_32 *pAEEPosInfo_align_32, AEEPositionInfoEx *po)
{
   if(po != NULL)
   {
      if(po->dwSize < offsetof(AEEPositionInfoEx,GPS_UTCOffset)) {
         return EBADPARM;
      }
      po->fLatitude = pAEEPosInfo_align_32->fLatitude;
      po->fLongitude = pAEEPosInfo_align_32->fLongitude;
      po->fAltitude = pAEEPosInfo_align_32->fAltitude;
      po->fHeading = pAEEPosInfo_align_32->fHeading;
      po->fHorVelocity = pAEEPosInfo_align_32->fHorVelocity;
      po->fVerVelocity = pAEEPosInfo_align_32->fVerVelocity;
      po->fHorUnc = pAEEPosInfo_align_32->fHorUnc;
      po->fAltitudeUnc = pAEEPosInfo_align_32->fAltitudeUnc;
      po->fUTCOffsetValid = pAEEPosInfo_align_32->fUTCOffsetValid;
      po->nAltitude = pAEEPosInfo_align_32->nAltitude;
      
      po->Latitude = doublestruct_to_double(pAEEPosInfo_align_32->Latitude);
      po->Longitude = doublestruct_to_double(pAEEPosInfo_align_32->Longitude);
      po->Heading = doublestruct_to_double(pAEEPosInfo_align_32->Heading);
      po->HorVelocity = doublestruct_to_double(pAEEPosInfo_align_32->HorVelocity);
      po->VerVelocity = doublestruct_to_double(pAEEPosInfo_align_32->VerVelocity);
      po->HorUnc = doublestruct_to_double(pAEEPosInfo_align_32->HorUnc);
      po->HorUncAngle = doublestruct_to_double(pAEEPosInfo_align_32->HorUncAngle);
      po->HorUncPerp = doublestruct_to_double(pAEEPosInfo_align_32->HorUncPerp);
      po->AltitudeUnc = doublestruct_to_double(pAEEPosInfo_align_32->AltitudeUnc);

      if(po->dwSize == sizeof(AEEPositionInfoEx)) {
         po->GPS_UTCOffset = pAEEPosInfo_align_32->GPS_UTCOffset;
         po->LocProbability = pAEEPosInfo_align_32->LocProbability;
         po->method = pAEEPosInfo_align_32->method;
      }
      return SUCCESS;
   }
   return EBADPARM;
}
#endif

AEEINTERFACE(IPosDet) {
   DECLARE_IQIBASE(IPosDet);
   int (*GetSectorInfo)(IPosDet *, AEESectorInfo *);
   int (*SetGPSConfig)(IPosDet *, AEEGPSConfig *);
   int (*GetGPSConfig)(IPosDet *, AEEGPSConfig *);
   int (*GetGPSInfo)(IPosDet *, AEEGPSReq, AEEGPSAccuracy, AEEGPSInfo *, AEECallback *);
   /* AEECLSID_POSDET_20 end */
   int (*GetOrientation)(IPosDet *, AEEOrientationReq, void *, AEECallback *);
   int (*GetSectorInfoEx)(IPosDet *, AEESectorInfo *, uint16 wSize);
   /* AEECLSID_POSDET_21 end */
   int (*ExtractPositionInfo)(IPosDet *, AEEGPSInfo *, AEEPositionInfoEx *);
};

#define IPOSDET_AddRef(p)                AEEGETPVTBL((p),IPosDet)->AddRef((p))
#define IPOSDET_Release(p)               AEEGETPVTBL((p),IPosDet)->Release((p))
#define IPOSDET_QueryInterface(p,id,pp)  AEEGETPVTBL((p),IPosDet)->QueryInterface((p),(id),(pp))
#define IPOSDET_SetGPSConfig(p,pc)       AEEGETPVTBL((p),IPosDet)->SetGPSConfig((p),(pc))
#define IPOSDET_GetGPSConfig(p,pc)       AEEGETPVTBL((p),IPosDet)->GetGPSConfig((p),(pc))
#define IPOSDET_GetGPSInfo(p,r,a,pi,pcb) AEEGETPVTBL((p),IPosDet)->GetGPSInfo((p),(r),(a),(pi),(pcb))

static __inline int IPOSDET_GetSectorInfo(IPosDet *p, AEESectorInfo *pi)
{
   IPosDet *pNew;
   int     nErr = EUNSUPPORTED;

   if( SUCCESS == IPOSDET_QueryInterface(p, AEECLSID_POSDET, (void **)&pNew) 
      || SUCCESS == IPOSDET_QueryInterface(p, AEECLSID_POSDET_312, (void **)&pNew) 
      || SUCCESS == IPOSDET_QueryInterface(p, AEECLSID_POSDET_302, (void **)&pNew)) {
     nErr = AEEGETPVTBL(pNew,IPosDet)->GetSectorInfoEx(pNew, pi, sizeof(AEESectorInfo));
     IPOSDET_Release(pNew);
   }
   else if( SUCCESS == IPOSDET_QueryInterface(p, AEECLSID_POSDET_20, (void **)&pNew)
            || SUCCESS == IPOSDET_QueryInterface(p, AEECLSID_POSDET_11, (void **)&pNew) ) {
     nErr = AEEGETPVTBL(pNew,IPosDet)->GetSectorInfo(pNew, pi);
     IPOSDET_Release(pNew);
   }
   return nErr;
}

static __inline int IPOSDET_GetOrientation(IPosDet *p, AEEOrientationReq r, void *pi, AEECallback *pcb)
{
   IPosDet *pNew;
   int      nErr = EUNSUPPORTED;

   if (SUCCESS == IPOSDET_QueryInterface(p, AEECLSID_POSDET, (void **)&pNew)
      || SUCCESS == IPOSDET_QueryInterface(p, AEECLSID_POSDET_312, (void **)&pNew) 
      || SUCCESS == IPOSDET_QueryInterface(p, AEECLSID_POSDET_302, (void **)&pNew)) {
      nErr = AEEGETPVTBL(pNew,IPosDet)->GetOrientation(pNew,r,pi,pcb);
      IPOSDET_Release(pNew);
   }
   return nErr;
}

static __inline int IPOSDET_ExtractPositionInfo(IPosDet *p, AEEGPSInfo *pi, AEEPositionInfoEx *po)
{
   IPosDet *pNew;
   int      nErr = EUNSUPPORTED;

   if (SUCCESS == IPOSDET_QueryInterface(p, AEECLSID_POSDET, (void **)&pNew)
      || SUCCESS == IPOSDET_QueryInterface(p, AEECLSID_POSDET_312, (void **)&pNew)) {
#if (__ARMCC_VERSION >= 200000) && (!defined (__APCS_ADSABI))
      AEEPositionInfoEx_ALIGN_32 AEEPosInfo_align_32;
      AEEPosInfo_align_32.dwSize = sizeof(AEEPositionInfoEx_ALIGN_32);
      nErr = AEEGETPVTBL(pNew,IPosDet)->ExtractPositionInfo(pNew,pi,(AEEPositionInfoEx *)&AEEPosInfo_align_32);
      if(nErr == SUCCESS) {
         nErr = AEEPositionInfoEx_ALIGN_32_to_AEEPositionInfoEx(&AEEPosInfo_align_32, po);
      }
#else
      nErr = AEEGETPVTBL(pNew,IPosDet)->ExtractPositionInfo(pNew,pi,po);
#endif
      IPOSDET_Release(pNew);
   }
   return nErr;
}

/*=====================================================================
  DATA STRUCTURES DOCUMENTATION
=======================================================================

AEESectorInfo

Description:
This structure is used to obtain sector based position location information from the system.

Definition:

typedef struct _AEESectorInfo
{
   uint16 wSysID;
   uint16 wNetID;
   uint16 wBaseID;
   uint16 wBaseClass;
   uint16 wBestPN;
   uint16 wPacketZoneID;
   uint16 wMobileCountryCode;
   uint32 dwBaseLatitude;
   uint32 dwBaseLongitude;
}AEESectorInfo;

Members:

   wSysID:               System Identification
   wNetID:               Network Identification
   wBaseStationID:            Base Station Identification
   wBaseStationClass:         Base Station Class
   wBestPN:              Best Pilot
   wPacketZoneID:        Packet Data Service Zone Identifier
   wMobileCountryCode:   Mobile country code
   dwBaseLatitude:       Base Station Latitude in the units of 0.25second, expressed
                         as a two's complement signed number with positive numbers
                         signifying North latitudes.
   dwBaseLongitude:      Base Station Longitude in the units of 0.25second, expressed
                         as a two's complement signed number with positive numbers
                         signifying East longitude.

Comments:
   For dwBaseLatitude and dwBaseLongitude, a floating point division with 14400 transforms
   them into degress.

See Also:
   IPOSDET_GetSectorInfo()

=======================================================================

AEEGPSMode

Description:
   This structure is used to specify the mode of operation of the GPS
   system on the device.

Definition:
   typedef  uint16      AEEGPSMode;
   
Supported values:

   AEEGPS_MODE_ONE_SHOT       : This mode is the default mode. It allows an application to 
                                request a position fix without worrying about which underlying 
                                mode is used by the position determination engine. This mode 
                                may not be optimal for repeated position determination requests
                                and should be used when only a single position determination 
                                request will be placed. If an application needs to have control over
                                the mode, use the AEEGPS_MODE_TRACK_LOCAL or AEEGPS_MODE_TRACK_NETWORK
                                modes, which are described below.~
                                
                                In this mode, the position determination engine uses the 
                                AEEGPSConfig.optim member to determine how to optimize its performance. 
                                See AEEGPSOpt for more information on possible optimizations. Since 
                                this mode indicates only a single position request will be placed, the 
                                AEEGPSConfig.nInterval and AEEGPSConfig.nFixes members are ignored.~

   AEEGPS_MODE_DLOAD_FIRST    : This mode is intended for applications which would like the
                                position results to be computed locally on the mobile device
                                (after the intial assistance data is downloaded from the server), 
                                and are making repeated position determination requests.~ 
                                
                                Applications can use this mode to download the assistance data 
                                (almanac and ephermis) from the location server. Applications can 
                                specify a request to only download the assistance data by using a 
                                AEEGPSReq type of AEE_GPSINFO_NONE when calling IPOSDET_GetGPSInfo().
                                Applications may want to do this as they are starting up (such as while
                                they display their splash screen) in order to reduce the time to 
                                obtain the first fix. Applications may also specify other request 
                                types, in which case the assistance data will be obtained in addition 
                                to the requested position information.~
                                
                                Applications can take advantage of performance optimizations by the
                                device's position determination engine by changing the mode to 
                                AEEGPS_MODE_TRACK_LOCAL for subsequent position location requests.~
                                
                                Note that using AEEGPS_MODE_TRACK_LOCAL mode also downloads the assistance
                                data from the location server. In this mode, the device will automatically 
                                refresh the assistance data if it is unavailable or obsolete when a fix is 
                                requested. The device uses the duration computed by (nFixes * nInterval) to 
                                determine the period for which assistance data is required. Periodically using 
                                AEEGPS_MODE_DLOAD_FIRST mode gives the application some control over when the 
                                almanac and ephermis data is downloaded or refreshed.~

   AEEGPS_MODE_TRACK_LOCAL    : This mode may be used by applications which are making multiple
                                position determination requests and want the position results
                                to be computed on the device. It is suited for applications
                                which intend to perform tracking, and require frequent, fast 
                                location/velocity/altitude information.~ 
                                
                                This mode also minimizes requests to the network for position 
                                determination information. The mobile will compute the 
                                position locally on the device. This mode uses aiding satellite 
                                data (ephemeris data and almanac) downloaded from the location 
                                server. If the aiding data in the device is unavailable or 
                                becomes invalid, the device seeks that information from the 
                                location server automatically (as it does in 
                                AEEGPS_MODE_DLOAD_FIRST mode).~

                                This mode does not implement any persistent tracking session. 
                                To perform tracking, an application must call 
                                IPOSDET_GetGPSInfo() at regular intervals.~

   AEEGPS_MODE_TRACK_NETWORK  : This mode may be used by applications which are making multiple
                                position determination requests. In this mode, the location 
                                server will compute the position using data provided by the 
                                device. The device communicates with the server using TCP/IP.~

                                This mode does not implement a persistent tracking session. 
                                To perform tracking, an application must call 
                                IPOSDET_GetGPSInfo() at regular intervals.~ 

   AEEGPS_MODE_TRACK_OPTIMAL  : This mode allows applications to request a position fix without
                                worrying about which underlying method is used by the position 
                                determination engine. It may be used by applications which are 
                                making multiple position determination requests. This mode 
                                does not implement a persistent tracking session. To perform 
                                tracking, an application must call IPOSDET_GetGPSInfo() at 
                                regular intervals.~ 
                                
                                This mode allows applications to request that the position 
                                determination requests be optimized. The type of optimization 
                                desired is specified by the AEEGPSConfig.optim member, which 
                                is set using IPOSDET_SetGPSConfig(). For more information
                                about possible optimizations, see AEEGPSOpt. AEEGPS_OPT_NONE is 
                                not an acceptable value for this mode and will result in EBADPARM. 
                                This mode also makes use of the AEEGPSConfig.nFixes and 
                                nInterval members. If nFixes=1, this mode is equivalent to 
                                AEEGPS_MODE_ONE_SHOT. If an application needs to have control 
                                over the position determination process, use the 
                                AEEGPS_MODE_TRACK_LOCAL or AEEGPS_MODE_TRACK_OPTIMAL modes.~
 
   AEEGPS_MODE_TRACK_STANDALONE : This mode should be used when the mobile device autonomously 
                                performs position determination, without communicating with an 
                                outside position location server. This mode is suited for
                                applications which intend to perform position fixes even when
                                terrestrial wireless service is unavailable. In this method, 
                                the device obtains GPS satellite ephemeris and almanac data by 
                                decoding the GPS signals (the same way a traditional GPS 
                                receiver does). This mode differs from the 
                                AEEGPS_MODE_TRACK_LOCAL mode because in the 
                                AEEGPS_MODE_TRACK_LOCAL mode, the device seeks the GPS satellite 
                                ephemeris and almanac data from the position location server 
                                using wireless TCP/IP communication.~

   AEEGPS_MODE_DEFAULT        : The default mode of operation, which is set to 
                                AEEGPS_MODE_ONE_SHOT.~

Comments:
   Not all devices with PosDet capability are capable of AEEGPS_MODE_TRACK_LOCAL or
   AEEGPS_MODE_DLOAD_FIRST. Setting the mode on such devices will result in EUNSUPPORTED.

See Also:
   AEEGPSOpt
   AEEGPSConfig
   IPOSDET_SetGPSConfig()
   IPOSDET_GetGPSInfo()

=======================================================================

AEEGPSMethod

Description:
   Describes bit mask explaining positioning method used to compute location.

Definition:
   typedef  uint32      AEEGPSMethod;
   
Supported values:

   AEEGPS_METHOD_LOCAL : Position is computed on the device.
   AEEGPS_METHOD_NETWORK : Position is computed on location server.
   AEEGPS_METHOD_ASSISTED : Network or location server assitance is used.
   AEEGPS_METHOD_STANDALONE : No network assistance is used.
   AEEGPS_METHOD_SATELLITE : GPS satellites are used as reference.
   AEEGPS_METHOD_CELLID : Position is determined from single cell transmitter or sector.
   AEEGPS_METHOD_AFLT : Position is triangulated using multiple cell transmitters as reference

Comments:
   None

See Also:
   AEEGPSInfo
   AEEPositionInfoEx
   IPOSDET_ExtractPositionInfo()
   IPOSDET_GetGPSInfo()

=======================================================================

AEEGPSInfoVersion

Description:
   Describes the version of AEEGPSInfo structure.
 
Definition:
   typedef  uint8   AEEGPSInfoVersion;
   
Supported values:

   AEEGPS_GETINFO_VERSION_0 : No extended data members in the AEEGPSInfo structure.
   AEEGPS_GETINFO_VERSION_1 : Version 1 of the AEEGPSInfo structure, dwTimeMS member added

Comments:
   In the AEEGPSInfo structure, the flag AEEGPS_VALID_VERSION indicates if the version 
   field of the AEEGPSInfo structure is supported by the device. If this flag is not set 
   the device supports AEEGPS_GETINFO_VERSION_0.

See Also:
   AEEGPSInfo
   AEEGPSReq
   IPOSDET_GetGPSInfo()

=======================================================================

AEEGPSAccuracy

Description:
   Describes the accuracy of the position fix. The accuracy levels are implementation 
   specific and may vary between devices. The accuracy levels correspond to an accuracy 
   value in meters.

Definition:
   typedef  uint8      AEEGPSAccuracy;

Supported values:
   AEEGPS_ACCURACY_LEVEL1 : Lowest accuracy
   AEEGPS_ACCURACY_LEVEL2 : 
   AEEGPS_ACCURACY_LEVEL3 : 
   AEEGPS_ACCURACY_LEVEL4 : 
   AEEGPS_ACCURACY_LEVEL5 : 
   AEEGPS_ACCURACY_LEVEL6 : Highest accuracy
   AEEGPS_ACCURACY_LOWEST : Defined as AEEGPS_ACCURACY_LEVEL1
   AEEGPS_ACCURACY_HIGHEST: Defined as AEEGPS_ACCURACY_LEVEL6

Comments:
   As a default value, AEEGPS_ACCURACY_LEVEL1 is typically used. Network operators (carriers) 
   may have their own recommendations on what AEEGPSAccuracy level should be used for
   various types of applications.~ 

   When the mode AEEGPS_MODE_TRACK_OPTIMAL is used, the device's position determination
   engine may try to compute the position fix locally on the device first. If the position 
   cannot be computed locally and meet the requested accuracy level, the position determination
   engine may attempt to compute a more accurate position fix by using the location server.~

See Also:
   AEEGPSInfo
   AEEGPSMode
   IPOSDET_GetGPSInfo()

=======================================================================

AEEGPSReq

Description:
   Describes the type of request the application is placing to the 
   IPOSDET_GetGPSInfo() interface.

Definition:
   typedef  uint8   AEEGPSReq;
   
Supported values:
   The following flags indicate what type of information is being requested:
	AEEGPS_GETINFO_NONE:     No position information is requested
	AEEGPS_GETINFO_LOCATION: Get the location
	AEEGPS_GETINFO_VELOCITY: Get the velocity
	AEEGPS_GETINFO_ALTITUDE: Get the altitude

   The flag AEEGPS_GETINFO_NONE is applicable only when the configured mode is of
   type AEEGPS_MODE_DLOAD_FIRST (AEEGPSConfig.mode = AEEGPS_MODE_DLOAD_FIRST). See 
   the description of AEEGPS_MODE_DLOAD_FIRST mode for more information.

   The following flags indicate what version of the AEEGPSInfo structure is being used. 
         AEEGPS_GETINFO_VERSION_0: No extended data members in the AEEGPSInfo structure
         AEEGPS_GETINFO_VERSION_1: AEEGPS_GETINFO_VERSION_1 members of AEEGPSInfo are
         desired.
         
   Specifying a version greater than AEEGPS_GETINFO_VERSION_0 implies that the 
   AEEGPSInfo structure has been extended and includes additional data members. 
   See AEEGPSGetInfoVersion for details on what members were added in each version.

   The flags (excluding AEEGPS_GETINFO_NONE) can be combined to get more than one type of 
   information. For example, the req specification (AEEGPS_GETINFO_LOCATION|AEEGPS_GETINFO_VERSION_1) 
   indicates to get the (2D) position along with any AEEGPS_GETINFO_VERSION_1 members of AEEGPSInfo.

Comments:
   The three most significant bits of the request, denoted by the mask 
   AEEGPS_GETINFO_VERSION_MASK, represent the version number for AEEGPSInfo buffer. 

See Also:
   AEEGPSMode
   AEEGPSInfoVersion
   IPOSDET_GetGPSInfo()

=======================================================================

AEEGPSOpt

Description:
   This data type is used to specify the optimization desired from the
   position determination subsystem.

Definition:
   typedef  uint8      AEEGPSOpt;

Supported values:
   AEEGPS_OPT_NONE        : Does not apply any optimization methods.
   AEEGPS_OPT_SPEED       : Get a position fix in the fastest manner possible. Based on 
                            the capabilities and current state of the device, the device 
                            will select the best method to obtain a position fix in the 
                            least amount of time. When used in conjunction with 
                            AEEGPS_MODE_TRACK_OPTIMAL or AEEGPS_MODE_ONE_SHOT, this will
                            result in a position fix in the fastest manner possible.
   AEEGPS_OPT_ACCURACY    : Get a position fix in the most accurate manner possible. This
                            This option may require a longer time to compute a position
                            fix and may require network connectivity, or may result in 
                            more data burst messages. When used in conjunction with 
                            AEEGPS_MODE_TRACK_OPTIMAL or AEEGPS_MODE_ONE_SHOT modes, this 
                            will result in the most accurate position fix possible.
   AEEGPS_OPT_PAYLOAD     : Get a position fix using the manner that results in no or 
                            minimal data exchange with the remote location server. 
                            For example, if a device is capable of performing a Standalone 
                            GPS fix, that option may result in the least data being exchanged 
                            with the location server and may be used if AEEGPS_OPT_PAYLOAD
                            is specified.
   AEEGPS_OPT_DEFAULT     : This is set to AEEGPS_OPT_NONE.


Comments:
   Applications can specify the accuracy level desired for a position fix regardless of 
   the optimization that is specified. This is done by setting the AEEGPSAccuracy parameter
   to the desired accuracy level requirement when calling IPOSDET_GetGPSInfo().

See Also:
   AEEGPSAccuracy
   AEEGPSConfig
   AEEGPSMode
   IPOSDET_GetGPSConfig()
   IPOSDET_SetGPSConfig()
   IPOSDET_GetGPSInfo()

=======================================================================

AEEGPSSvrType

Description:
   This data type is used to specify the transport layer by which the device will 
   reach the location server. It is recommended to use the default setting
   AEEGPS_SERVER_DEFAULT so that the device may use the preferred location server.
   
Definition:
   typedef  uint32      AEEGPSSvrType;

Supported values:
   AEEGPS_SERVER_DEFAULT  : Use the location server IP address, port number and other 
                            location server configuration parameters that have been 
                            provisioned into the device by the network operator or OEM. This 
                            setting should be used for commercial versions of applications.~
   AEEGPS_SERVER_IP       : Use a TCP/IP data connection to contact the location server. The 
                            application must specify the IP address and Port number 
                            (see also AEEGPSIPServer). This setting may be useful
                            for testing an application, as it allows the developer to
                            update the location server IP address in a flexible way.~
   AEEGPS_SERVER_DBURST   : Use data burst messages as the transport to/from the
                            location server. (This server type may not be supported in
                            all the mobile networks. Using this server type in such
                            event causes EUNSUPPORTED with IPOSDET_SetGPSConfig.)~

Comments:
   None

See Also:
   AEEGPSConfig
   AEEGPSIPServer
   AEEGPSServer
   IPOSDET_SetGPSConfig
   IPOSDET_GetGPSConfig

=======================================================================

AEEGPSIPServer

Description:
   This structure specifies the IP Server to be used (To be used when
   AEEGPS_SERVER_IP is specified as the server type).

Definition:
   typedef struct _AEEGPSIPServer {
      INAddr      addr;
      INPort      port;
      uint16      reserved;
   } AEEGPSIPServer;

Members:
   addr     :  32-bit IP address of the server in network byte order
   port     :  port to connect. In network byte order.

Comments:
   None

See Also:
   INAddr
   INPort
   AEEGPSConfig
   AEEGPSServer

=======================================================================

AEEGPSServer

Description:
   This structure specifies the Server type and specifications for the
   server to be used for GPS-based position determination.

Definition:
   typedef struct _AEEGPSServer{
      AEEGPSSvrType  svrType;
      union {
         AEEGPSIPServer ipsvr;
      } svr;
   } AEEGPSServer;

Members:

   svrType :  
   svr     :  specifications for the server to be used.

Comments:
   A valid value for ipsvr is required when server type AEEGPS_SERVER_IP is specified.

See Also:
   AEEGPSConfig
   AEEGPSSvrType
   AEEGPSIPServer
   IPOSDET_GetGPSConfig
   IPOSDET_SetGPSConfig

=======================================================================

AEEGPSConfig

Description:
   This structure is used to configure the GPS engine provided by this interface.

Definition:
   typedef struct _AEEGPSConfig {
      AEEGPSMode        mode;
      uint16            nFixes;
      uint16            nInterval;
      AEEGPSOpt         optim;
      AEEGPSQos         qos;
      AEEGPSServer      server;
   } AEEGPSConfig;

Members:
   mode:      This is used to specify which of several operating modes the underlying
              GPS engine should use. Possible values are described under AEEGPSMode.

   nFixes:    Estimated number of repetitive position determination requests that will 
              be made using this interface when using AEEGPS_MODE_TRACK_LOCAL or 
              AEEGPS_MODE_TRACK_NETWORK. This information is passed to 
              the position determination engine so it can use estimates of 
              upcoming position determination requests to optimize its 
              performance in terms of battery life and time to fix.~ 
              
              The application sets nFixes to an estimate of the number of fixes to be 
              placed. For each position determination request, the IPOSDET interface  
              decrements the value of nFixes by one, until the value of nFixes=1. 
              The value of nFixes remains at one (i.e. the IPOSDET interface does not 
              decrement the value of nFixes once it reaches 1) until it is set to a 
              different value using IPOSDET_SetGPSConfig().~ 

              NOTE: nFixes is an estimate of the number of position determination requests 
              the application will be placing. Each position determination request results 
              in only one position fix -- nFixes does NOT cause multiple fixes to be returned 
              for a single call to IPOSDET_GetGPSInfo(). To receive multiple position fixes, 
              the application will have to make multiple calls to IPOSDET_GetGPSInfo().~ 

              The application can change the value of nFixes at any point using  
              IPOSDET_SetGPSConfig(). The value should be set to 1 for a single position  
              determination request. A value of 0 implies an unknown number of position 
              requests.~ 

   nInterval: Estimated interval between consequtive position determination requests (in  
              seconds). This information is passed to the position determination engine so  
              it can use estimates of upcoming position determination requests to optimize its  
              performance in terms of battery life and time to fix.~ 

   optim:     The position determination engine can use information about upcoming 
              requests to optimize its performance. The optim parameter tells the 
              position determination engine how to optimize its performance.  
              Possible values are described by AEEGPSOpt.~ 

   qos:       Quality of Service. The qos parameter practically corresponds to the  
              maximum amount of time (expressed in number of seconds) that the 
              application wants the position determination engine to search for  
              satellites. Note that in blue sky conditions, the search for 
              satellites may take only a few seconds even if the qos is set to a  
              much higher value.~  
              
              Values between 0-255 are valid, 255 providing the highest quality  
              of service. Note: The underlying implementation may round off the  
              maximum value to less than 255. Higher qos values may result 
              in greater accuracy of position fixes, and longer fix times. 
              Lower qos values may result in shorter times to obtain position  
              fixes. It is recommended that applications use  
              AEEGPS_MODE_TRACK_OPTIMAL or AEEGPS_MODE_ONE_SHOT modes if the  
              application goal is to focus on speed or accuracy and you do not  
              want to work with the qos parameter.~ 
              
              Setting qos=0 indicates that no time should be spend searching for  
              satellites.  On some devices, this may result in a pure AFLT fix. 
              (AFLT computes the position using triangulation based on multiple  
              cell transmitters as reference.) Position fixes made without the  
              use of satellite data (such as AFLT fixes) perform no pseudo-range  
              measurements against GPS satellites. Since a fix made with qos=0 relies  
              on the wireless network for information to compute a position fix, a  
              qos value of 0 should not be used when AEEGPS_MODE_TRACK_LOCAL or   
              AEEGPS_MODE_TRACK_STANDALONE are specified.~  

              The accuracy of a fix made with qos=0 is influenced by the location  
              server's base station almanac accuracy, the device calibration, and   
              other possible factors, such as the number of base station pilot   
              channels visible to the device.~  
              
              The qos parameter may be ignored on certain mobile devices.~ 

   server:    Server configuration. This structure specifies the server type 
              and configuration. Possible server types are: 
~
              AEEGPS_SERVER_DEFAULT~
              AEEGPS_SERVER_IP~
              AEEGPS_SERVER_DBURST ~
*
              Effective BREW 3.1, applications using server type other than  
              AEEGPS_SERVER_DEFAULT must have privilege to the group id  
              AEECLSID_LOCATIONSERVER.~ 

              If the server type is AEEGPS_SERVER_IP, the IP Address and port
              of the Position Determination server must be specified. If the port
              number is specified as 0, connection will be attempted on a default
              port for the service.~

Comments:
   None

See Also:
   AEEGPSMode 
   AEEGPSOpt 
   AEEGPSServer 
   IPOSDET_GetGPSConfig()
   IPOSDET_SetGPSConfig()

=======================================================================

AEEGPSInfo

Description:
   This structure is used to obtain GPS based position location information from the
   system. The parameters returned are as per the TIA/EIA IS-801 standard.

Definition:
   typedef struct _AEEGPSInfo {
      uint32             dwTimeStamp;
      uint32             status;
      int32              dwLat;
      int32              dwLon;
      int16              wAltitude;
      uint16             wHeading;
      uint16             wVelocityHor;
      int8               bVelocityVer;
      AEEGPSAccuracy     accuracy;
      uint16             fValid;
      uint8              bHorUnc;
      uint8              bHorUncAngle;
      uint8              bHorUncPerp;
      AEEGPSInfoVersion  Version;
      uint16             wVerUnc;
      int8               GPS_UTCOffset;
      byte               LocProbability;
      AEEGPSMethod       method; 
      uint32             dwTimeMS;
   } AEEGPSInfo;

Members:
   dwTimeStamp:          Time (in seconds since 1/6/1980) when measurement was taken
   status:               Response Status
   dwLat:                Latitude, 180/2^25 degrees, WGS-84 ellipsoid
   dwLon:                Longitude, 360/2^26 degrees, WGS-84 ellipsoid
   wAltitude:            Altitude, meters, WGS-84 ellipsoid
   wHeading:             Heading, 360/2^10 degrees
   wVelocityHor:         Horizontal velocity, 0.25 meters/second
   bVelocityVer:         Vertical velocity, 0.5 meters/second
   accuracy:             Set to be requested accuracy level.
   fValid:               Flags indicating valid fields in the struct.
   bHorUnc:              Horizontal uncertainity
   bHorUncAngle:         Horizontal uncertainity at angle
   bHorUncPerp:          Horizontal uncertainty perpendicular
   Version:              Version number of this structure
   wVerUnc:              Vertical uncertainity
   GPS_UTCOffset:        Time difference in seconds between UTC and GPS satellite time
   LocProbability:       Probability that user's actual position is within 
                         the described ellipse, expressed as a percentage 
   method:               Bitmask providing information about the positioning method
                         used to calculate the location. 
   dwTimeMS:             Milliseconds info over the seconds reported by dwTimeStamp.
                         Supported in AEEGPS_GETINFO_VERSION_1.

Comments:
   fValid indicates the fields set in the AEEGPSInfo structure. The following
   flags are available.
~
         AEEGPS_VALID_LAT   :    Valid latitude  ~
         AEEGPS_VALID_LON   :    Valid longitude  ~
         AEEGPS_VALID_ALT   :    Valid altitude ~
         AEEGPS_VALID_HEAD  :    Valid heading  ~
         AEEGPS_VALID_HVEL  :    Valid horizontal velocity~
         AEEGPS_VALID_VVEL  :    Valid vertical velocity~
         AEEGPS_VALID_HUNC  :    Valid horizontal uncertainity  ~
         AEEGPS_VALID_AUNC  :    Valid Horizontal uncertainity at angle~
         AEEGPS_VALID_PUNC  :    Valid horizontal uncertainity (orthogonal)~
         AEEGPS_VALID_VUNC  :    Valid Vertical uncertainity~
         AEEGPS_VALID_UTCOFFSET : Valid GPS UTC Offset~
         AEEGPS_VALID_VERSION :  Valid Version field, indicates version of
                                 AEEGPSInfo structure supported by device. If
                                 this flag is not set device supports 
                                 AEEGPS_GETINFO_VERSION_0.~
         AEEGPS_VALID_TIMEMS :   Valid dwTimeMS field
*

   accuracy is specified by 6 levels starting from AEEGPS_ACCURACY_LEVEL1 to
   AEEGPS_ACCURACY_LEVEL6. See AEEGPSAccuracy for more information.

   status may contain following information indicating the Success or Failure
   of the GetGpsInfo request :
~
     AEEGPS_ERR_NO_ERR : The fix was successfully returned by the handset GPS engine.

     AEEGPS_ERR_GENERAL_FAILURE : This error indicates a general failure in the system. The
     application may abort the sessions and display an error message to the user.
	 
     AEEGPS_ERR_TIMEOUT : This error indicates the fix could not be obtained within the time 
     range specified by the handset implmentation. This time range is device-dependent and may 
     differ from one device to another. The application may retry position requests on this error. 
     If the application encounters repeated timeouts, the application may abort the sessions and 
     display an error message to the user. Timeouts may occur for many reasons, including inability
     of the device to communicate with the location server (such as if the location server IP 
     address and port are not provisioned or are provisioned incorrectly), inability of the location 
     server to respond to a request, data connection failure, etc.
	 
     AEEGPS_ERR_INFO_UNAVAIL : This error indicates that some or all of the requested information
     is unavailable. (For example, the altitude information may be unavailable, but the 
     latitude/longitude information may be available.)  If there is no infomation available in fValid, 
     it means the device failed to obtain position for one of the following reasons:
     If using AEEGPS_MODE_TRACK_NETWORK, the device failed to see a sufficient number of 
     CDMA pilots or GPS satellites in order to obtain a position.
     If using AEEGPS_MODE_TRACK_LOCAL, the device failed to see a sufficinet number of satellites to 
     compute a position. This may occur if the mobile device is inside a building or a tunnel. 
     If the application repeatedly failes in this mode, it may attempt to use AEEGPS_MODE_TRACK_NETWORK 
     to obtain a position.
	 
     AEEGPS_ERR_ACCURACY_UNAVAIL : This error indicates the fix could not be obtained within the
     accuracy range requested by the accuracy parameter passed to IPOSDET_GetGPSInfo(). The application 
     may implement a retry mechanism to handle this error condition. NOTE:  This error code is not 
     currently returned by the BREW API.
	 
     AEEGPS_ERR_PRIVACY_REFUSED : This error indicates that the request was refused to 
     protect the device's privacy. This occurs if privacy is enforced at the device level 
     and the device rejects the position requests of the application. ISHELL_GetDeviceInfoEx()
     may be used to query if is enabled on the device.
	 
     AEEGPS_ERR_SRV_UNREACHABLE : This error is returned when the device fails to
     connect/communicate with the location server. Possible reasons for this error could 
     be that the IP address (which was set by application using IPOSDET_SetGPSConfig() or 
     is the default value provisioned into the device) is either incorrect or unavailable.
	 
     AEEGPS_ERR_LINK_FAILED : This error is returned when the communication link setup to 
     handle the position request is disrupted. Possible reasons for this error could be 
     that another entity (either user or a high priority program) preempted the session 
     causing the link for current request to fail. Another possible reason is that the wireless 
     link failed. This is likely a temporary failure and the application may re-try the request.
	 
     AEEGPS_ERR_REJECTED : One or more entities in the system rejected the position request.
	 
     AEEGPS_ERR_OUTOF_RESOURCES : This error is returned when the system does not have sufficient 
     resources to handle the position request. Resource contention could be coming from one or 
     more reasons, such as another position session, out of TCP sockets, a voice call or any other 
     traffic call is locking up resources, etc. This error most likely requires user intervention 
     in order for the re-tries to succeed.
	 
     AEEGPS_ERR_RECEIVER_BUSY : This error is returned when device radio receiver is temporarily 
     unavailable for position determination.~
*

See Also:
   AEEGPSAccuracy
   AEEGPSInfo
   AEEGPSInfoVersion
   AEEGSPMethod
   IPOSDET_GetGPSInfo()
   IPOSDET_ExtractPositionInfo()
   ISHELL_GetDeviceInfoEx()
   WGS84_TO_DEGREES()

=======================================================================

AEEPositionInfoEx

Description:
   This structure is used to obtain position location information from the
   GPS based position location AEEGPSInfo.

Definition:
typedef struct _AEEPositionInfoEx {
   uint32               dwSize;
   flg                  fLatitude:1;
   flg                  fLongitude:1;
   flg                  fAltitude:1;
   flg                  fHeading:1;
   flg                  fHorVelocity:1;
   flg                  fVerVelocity:1;
   flg                  fHorUnc:1;
   flg                  fAltitudeUnc:1;
   flg                  fUTCOffsetValid:1;

   double               Latitude;
   double               Longitude;
   int                  nAltitude;
   double               Heading;
   double               HorVelocity;
   double               VerVelocity;
   double               HorUncAngle;
   double               HorUnc;
   double               HorUncPerp;
   double               AltitudeUnc;
   int8                 GPS_UTCOffset;
   byte                 LocProbability;
   AEEGPSMethod         method;  

} AEEPositionInfoEx;

Members:

   dwSize:              The sizeof the AEEPositionInfoEx structure.
   fLatitude:           Latitude member of the structure is valid.
   fLongitude:          Longitude member of the AEEPositionInfoEx is valid.
   fAltitude:           nAltitude member is valid.
   fHeading:            the member Heading is valid.
   fHorVelocity:        the member HorVelocity is valid.
   fVerVelocity:        the member VerVelocity is valid.
   
   fHorUnc:             Horizontal uncertainity members (HorUncAngle,
                                HorUnc and HorUncPerp) are valid.
								
   fAltitudeUnc:        the member AltitudeUnc is valid.
   fUTCOffSetValid:     the member GPS_UTCOffset is valid.
   Latitude:            latitude in degrees w.r.t WGS-84 ellipsoid.
   Longitude:           longitude in degrees w.r.t WGS-84 ellipsoid.
   nAltitude:           altitude in meters w.r.t WGS-84 ellipsoid.
   Heading:             heading direction in degrees from true North.
   HorVelocity:         horizontal velocity in meters/second.
   VerVelocity:         vertical velocity in meters/second.
   
   HorUncAngle:         one of the data members describing uncertainity in position.
                        This describes an axis in horizontal plane, in degrees
                        from true North.
						
   HorUnc:              one of the data members describing uncertainity in position.
                        This describes the uncertainity in meters along the axis(specified
                        bye HorUncAngle) in horizontal plane.
						
   HorUncPerp:          one of the data members describing uncertainity in position.
                        This describes the uncertainity in meters along the axis perpendicular
                        to the one specified by HorUncAngle.
						
   AltitudeUnc:         meters of uncertainity in the altitude.
   
   GPS_UTCOffset:       Time difference in seconds between UTC and GPS satellite time   
   
   LocProbability:      Probability that user's actual position is within 
                        the described ellipse, expressed as a percentage 
						
   method:              Bitmask providing information about the positioning 
                        method used to calculate the location

Comments:

   The uncertainity in position parameters : HorUncAngle, HorUnc and HorUncPerp - describe an 
   ellipse in the horizontal plane.

See Also:
   AEEGPSMethod
   IPOSDET_ExtractPositionInfo()

=============================================================================

AEEOrientationReq

Description:

Definition:
    typedef uint8 AEEOrientationReq;
    #define AEEORIENTATION_REQ_AZIMUTH 0x01

Members:
    None

Comments:
    None

See Also:
    IPOSDET_GetOrientation

=======================================================================

AEEOrientationAzimuthInfo

Description:
   This structure is used to obtain the azimuth of the device. This is the response
   from the device for the request AEEORIENTATION_REQ_AZIMUTH.

Definition:
   typedef struct _AEEOrientationAzimuthInfo {
      uint16             wSize;
      uint16             wAzimuth;
      uint32             dwTimeStamp;
   } AEEOrientationAzimuthInfo;

Members:
   wSize:                size of the data structure AEEOrientationAzimuthInfo.
   
   wAzimuth:             Angle 0 - 359 degress and 59 arcminutes.
                         bits 0-5 contain arcminutes
                         bits 6-15 contain degrees.
                         This is the heading angle in the local horizontal plane
                         measured clockwise from true North
						 
   dwTimeStamp:          Time (in seconds since 1/6/1980) of this measurement

Comments:
   None

See Also:
   IPOSDET_GetOrientation()

=========================================================================
=========================================================================
INTERFACES   DOCUMENTATION
=========================================================================
IPosdet Interface

Description:

This interface provides services for position determination using GPS 
information or sector information. In order to use the GPS based position 
determination methods such as IPOSDET_SetGPSConfig(), IPOSDET_GetGPSConfig(), 
and IPOSDET_GetGPSInfo(), Position Determination Privileges (PL_POS_LOCATION) 
are required. Similarly, to use the sector-based position determination methods 
such as IPOSDET_GetSectorInfo(), SectorInfo privileges (PL_SECTORINFO) are 
required.

IPOSDET_GetGPSInfo() is an asynchronous method which uses AEECallback. Care
must be taken to ensure that the callbacks and information structures passed
to these methods by reference remain in scope until the callback returns.

Developers of Location Based Applications may use the BREW SDK to develop and
test their applications. The BREW SDK provides GPS emulation; users can set 
the GPS emulation in the Tools->GPS Emulation menu to use a pre-recorded NMEA 
file as GPS input, or connect an NMEA-output capable GPS device. An offline 
utility called NMEALogger.exe can be used to record an NMEA file from data coming 
from a GPS device connected to the serial port of the desktop/laptop. This NMEA 
file can be used later as GPS input. See the SDK User's Guide and the SDK 
Utilities Guide for details.

See the IPOSDET Use Case Scenarios for additional information.
===span class=Blue>
===a href="../UseCaseScenarios/UseCaseScenariosList.htm" >Use Case Scenarios List
===/a>
===/span>
The following header file is required:

AEEPosDet.h

=======================================================================

IPOSDET_AddRef()

Description:
   This function increments the reference count of the IPosDet Interface object, allowing
   the object to be shared by multiple callers. The object is freed when the reference
   count reaches 0 (zero).

Prototype:
   uint32 IPOSDET_AddRef(IPosDet * pIPosDet)

Parameters:
   pIPosDet: [in]. Pointer to the IPosDet Interface object.

Return Value:
   Incremented reference count for the object.

Comments:
   A valid object returns a positive reference count.

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   IPOSDET_Release()

=======================================================================

IPOSDET_Release()

Description:
   This function decrements the reference count of the IPosDet Interface object. The
   object is freed from memory and is no longer valid when the reference count reaches
   0 (zero).

Prototype:
   uint32 IPOSDET_Release(IPosDet * pIPosDet)

Parameters:
   pIPosDet: [in]. Pointer to the IPosDet Interface object.

Return Value:
   Decremented reference count for the object.

   0 (zero), If the object has been freed and is no longer valid.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   IPOSDET_AddRef()

=======================================================================

IPOSDET_QueryInterface()

Description:
   This function asks an object for another API contract from the object in question.

Prototype:

   int IPOSDET_QueryInterface
   (
   IPosDet * pIPosDet,
   AEECLSID idReq,
   void ** ppo
   )

Parameters:
   pIPosDet: [in] Pointer to the IPosDet Interface object.
   idReq:    [in] Requested class ID exposed by the object
   ppo:      [out] Returned object. Filled by this method

Return Value:
   SUCCESS     :  Interface found~
   EBADPARM    :  This error is returned if the ppo parameter specified is invalid (NULL).~
   ECLASSNOTSUPPORT:  Requested interface is unsupported~

Comments:
   The pointer in *ppo is set to the new interface (with refcount positive), or NULL if the
   ClassID is not supported by the object. ppo MUST not be NULL.

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   None

=======================================================================

IPOSDET_GetSectorInfo()

Description:
   This function returns information for sector based position
   location. It returns information about the SystemID, NetworkID,
   BaseStationID, BaseStationClass and best Pilot. This information is
   proprietary infomation from each network operator. Inorder to invoke this 
   function, the caller (app) must have the PL_SECTORINFO privilege
   level. Without this privilege level, this function will fail.

Prototype:
   int   IPOSDET_GetSectorInfo(IPosDet *pif, AEESectorInfo *pSecInfo)

Parameters:
   pif      : [in]. Pointer to IPosDet object.
   pSecInfo : [out]. This must be a value ptr to the AEESectorInfo structure.
              On return, the members of this struct contain sector information.

Return Value:

   SUCCESS   : The function succeeded.
   
   EPRIVLEVEL: This error is returned if the calling BREW application does not have
               PL_SECTORINFO privilege set in the Module Information File (MIF).  This 
               privilege is requred to invoke this function. Correct the MIF and try 
               again.
			   
   EBADPARM  : This error is returned if the pSecInfo parameter specified is invalid (NULL).
   
   EUNSUPPORTED: This error is returned if the function is not supported by the device.
   
   EFAILED   : General failure

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   AEESectorInfo

=======================================================================

IPOSDET_SetGPSConfig()

Description:
   This function sets the GPS configuration to be used by the GPS engine.

Prototype:
   int IPOSDET_SetGPSConfig(IPosDet *pIPosDet, AEEGPSConfig *pConfig);

Parameters:
   pIPosDet : [in]. The interface pointer.
   pConfig  : [in]. Pointer to GPS configuration. See AEEGPSConfig for details.


Return Value:
   SUCCESS   : The function succeeded.
   
   EPRIVLEVEL: This error is returned if the calling BREW application does not have
               PL_POS_LOCATION privilege set in the Module Information File (MIF). This 
               privilege is requred to invoke this function. Correct the MIF and try 
               again.

EBADPARM  : This error is returned if the pConfig parameter specified is invalid 
               (NULL) or the data passed in the *pConfig is invalid (checks validity 
               of mode, server.svrType, optim).
			   
   EUNSUPPORTED: This error is returned if the mode specified in the AEEGPSConfig 
                 structure is not supported by the device's GPS engine or if 
                 the server type specified by AEEGPSConfig.server is not supported.

Comments:
   Until this function is called, the position determination engine will be
   configured with default settings. Default settings can be obtained using
   IPOSDET_GetGPSConfig. Only the position determination requests following
   a call to IPOSDET_SetGPSConfig() will use the new configuration.

Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   AEEGPSConfig
   IPOSDET_GetGPSConfig()
   IPOSDET – Placing a single request for GPS position information
   IPOSDET – Placing multiple requests for GPS position information

=======================================================================

IPOSDET_GetGPSConfig()

Description:
    This function gets the current GPS configuration of the GPS engine.

Prototype:
   int IPOSDET_GetGPSConfig(IPosDet *pIPosDet, AEEGPSConfig *pConfig);

Parameters:
   pIPosDet      : [in]. Pointer to IPosDet Object.
   pConfig       : [out]. Pointer to GPS configuration. See AEEGPSConfig for details.


Return Value:

   SUCCESS   : The function succeeded.
   
   EPRIVLEVEL: This error is returned if the calling BREW application does not have
               PL_POS_LOCATION privilege set in the Module Information File (MIF). This 
               privilege is requred to invoke this function. Correct the MIF and try 
               again.
			   
   EBADPARM  : This error is returned if the pConfig parameter specified is invalid (NULL).
   
   EUNSUPPORTED: This error is returned if the function is not supported by the device.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   AEEGPSConfig
   IPOSDET_SetGPSConfig()
   IPOSDET – Placing a single request for GPS position information
   IPOSDET – Placing multiple requests for GPS position information

=======================================================================

IPOSDET_GetGPSInfo()

Description:
   This function returns information for GPS based position location. It returns
   latitude, longitude, altitude information, as well as vector information such as
   horizontal and vertical velocity, heading, and the uncertainity of the horizontal
   information. If the position request is not satisfactorily answered, the status 
   member of the AEEGPSInfo structure that is returned in the response will be non-zero 
   and will be set to one of the AEEGPS_ERR_* values.
   
   This is an asynchronous call which uses AEECallback. The callback specified by pcb 
   is called on completion. Care must be taken to ensure that the callbacks and 
   information structures passed to these methods by reference remain in scope until 
   the callback returns.

   This function enforces the privacy policies recommended by the network operator.
   Privacy implementation on the device may prompt the user using a dialog.
   Applications invoking this function must be prepared to surrender the screen for
   the dialog prompted to user. The application must suspend any paint to the screen 
   upon receipt of the event EVT_DIALOG_START and must redraw upon receipt of the event
   EVT_DIALOG_END.

   See the IPOSDET Use Cases for more information on using this interface, and an
   illustraction of the interaction with the EVT_DIALOG_START.

   IPOSDET – Placing a single request for GPS position information
   IPOSDET – Placing multiple requests for GPS position information

Prototype:
   int IPOSDET_GetGPSInfo(IPosDet *pif, AEEGPSReq req, AEEGPSAccuracy accuracy,
                          AEEGPSInfo *pGPSInfo, AEECallback *pcb );

Parameters:
   pif      : [in]. Pointer to IPosDet object.~

   req      : [in]. Flags which describe the type of request the application is placing 
              to the IPOSDET_GetGPSInfo() interface.

              The flags (excluding AEEGPS_GETINFO_NONE) can be combined to get more
              than one type of information. See AEEGPSReq for possible values and 
              more information.

   accuracy : [in]. Desired level of accuracy for this request. See AEEGPSAccuracy for 
              possible values.

   pGPSInfo : [out]. On input, this must be a valid ptr to the
              AEEGPSInfo structure. On callback, the members of this struct 
              contain the position location information. The caller is 
              notifified asynchronously using AEECallback when a response is ready. 
              For this reason, the caller must ensure that this structure remains valid 
              until the callback specified by pcb is called.

              See AEEGPSInfo and AEEGPSInfoVersion for more information on this structure, 
              and details of extended members added in each version of the structure.

   pcb      : [in]. Callback function which gets called on completion of position
              determination. This structure may be initialized using CALLBACK_Init().


Return Value:

   SUCCESS   : The function succeeded, and the request is being processed. A response
               will be sent asynchronously using the callback specified by pcb. 
			   
   EPRIVLEVEL: This error is returned if the calling BREW application does not have
               PL_POS_LOCATION privilege set in the Module Information File (MIF). This 
               privilege is required to invoke this function. Correct the MIF and try 
               again.
			   
   EBADPARM  : This error is returned if the pGPSInfo or pcb parameter specified are 
               invalid (NULL), or the accuracy level specified is invalid.
			   
   EUNSUPPORTED: This error is returned if either the AEEGPSReq request type or the 
                 AEEGPSAccuracy accuracy level requested is not supported by the 
                 device's GPS engine.
				 
   ENOMEMORY : Unable to process the request due to insufficient memory.
   
   EFAILED   : General failure, the callback function will not be called by BREW.  

Comments:
   SUCCESS means the request was submitted and is being processed. The application will 
   be notified asynchronously using the callback specified by pcb when the response is 
   available. Care must be taken to ensure that the callbacks and information structures 
   passed to these methods by reference remain in scope until the callback returns. 
   Application may invoke CALLBACK_Cancel() on the pcb to cancel the request.

Side Effects:
   None

Version:
   Introduced BREW Client 2.0

See Also:
   AEEGPSReq
   AEEGPSAccuracy
   AEEGPSInfo
   AEEGPSInfoVersion
   AEECallback
   CALLBACK_Init()
   CALLBACK_Cancel()
   IPOSDET – Placing a single request for GPS position information
   IPOSDET – Placing multiple requests for GPS position information

=======================================================================

IPOSDET_GetOrientation()

Description:
   This function returns device's orientation in the horizontal plane.

   This is an asynchronous call which uses AEECallback. The callback specified by pcb 
   is called on completion. Care must be taken to ensure that the callbacks and 
   information structures passed to these methods by reference remain in scope until 
   the callback returns.

Prototype:
   int IPOSDET_GetOrientation( IPosDet *pif, AEEOrientationReq req, void *pOrInfo,
                               AEECallback *pcb );

Parameters:
   pif      : [in]. Pointer to the IPosDet object.
   req      : [in]. The requested information. (Possible values include
              AEEORIENTATION_REQ_AZIMUTH)
   pOrInfo  : [out]. On input, this must be a valid ptr to the
              valid memory with the first two bytes (wSize) indicating
              the space available in bytes. The space should be a minimum
              of that required to place the response corresponding the request.
              On callback, the members of this struct contain Orientation
              information corresponding to the request. The caller must ensure
              that this memory is valid until the callback specified by pcb
              gets called.
   pcb      : [in]. Callback function which gets called on completion of the
              request. This structure may be initialized using CALLBACK_Init().

Return Value:

   SUCCESS   : The function succeeded, and the request is being processed. A response
               will be sent asynchronously using the callback specified by pcb.
		
   EPRIVLEVEL: This error is returned if the calling BREW application does not have
               PL_POS_LOCATION privilege set in the Module Information File (MIF). This 
               privilege is requred to invoke this function. Correct the MIF and try 
               again.
			   
   EBADPARM  : This error is returned if the pOrInfo or pcb parameter specified are 
               invalid (NULL).
			   
   EUNSUPPORTED: This error is returned if this function is not supported by the device.~
   EMEMPTR   : Invalid pCB pointer (such as if the pointer is NULL or points to a block
               of memery that is the wrong size)
			   
   EFAILED   : General failure, the callback function will not be called by BREW.

Comments:
   SUCCESS means the request was submitted and is being processed. The application will 
   be notified asynchronously using the callback specified by pcb when the response is 
   available. Care must be taken to ensure that the callbacks and information structures 
   passed to these methods by reference remain in scope until the callback returns. 
   Application may invoke CALLBACK_Cancel() on the pcb to cancel the request.

Side Effects:
   None

Version:
   Introduced BREW Client 2.1

See Also:
   AEEOrientationReq
   AEEOrientationAzimuthInfo
   AEECallback
   CALLBACK_Init()
   CALLBACK_Cancel()

=======================================================================

IPOSDET_ExtractPositionInfo()

Description:
   Given the position in AEEGPSInfo, this function generates the position
   information in the format specified by AEEPositionInfoEx.

Prototype:
   int IPOSDET_ExtractPositionInfo( IPosDet *pif, AEEGPSInfo *pi, AEEPositionInfoEx *po );

Parameters:
   pif      : [in]. Pointer to the IPosDet object.
   pi       : [in]. Pointer to AEEGPSInfo data.
   po       : [out]. Output buffer for position information.

Return Value:

   SUCCESS   : The function succeeded.
   
   EPRIVLEVEL: This error is returned if the calling BREW application does not have
               PL_POS_LOCATION privilege set in the Module Information File (MIF). This 
               privilege is requred to invoke this function. Correct the MIF and try 
               again.
			   
   EBADPARM  : This error is returned if the pi or po parameter specified are invalid (NULL).
   
   EUNSUPPORTED: This error is returned if this function is not supported by the device.
   
   EFAILED   : General failure
   
Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   AEEGPSInfo
   AEEPositionInfoEx
   IPOSDET – Placing a single request for GPS position information
   IPOSDET – Placing multiple requests for GPS position information

=======================================================================*/

#endif /* #ifndef AEEPOSDET_H */


