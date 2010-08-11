/*=============================================================================
FILE:         OEMBTDevDB.c

SERVICES:     OEMBTDevDB implementation

DESCRIPTION:  This file handles the OEM implemenation for the 
              IBTDevDB interface 

===============================================================================
        Copyright ?2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
#include "OEMFeatures.h"

#include "OEMHeap.h"
#include "bt.h"

#include "btsd.h"
#include "msg.h"

#include "OEMOS.h"
#include "OEMBTDevDB.h"
#include "AEE_OEMDispatch.h"

#include "AEEStdLib.h"
#include "AEEHandle.h"
#include "AEE_OEM.h"

//==========================================================================
//   Macro definitions
//==========================================================================

   #define OEM_BT_MAX_DEVICES BT_RM_MAX_DEVICES

   #define MAX_DB_INSTANCE       1
BTHandle gBTHandle;
//==========================================================================
//   Type definitions
//==========================================================================
typedef struct OEMBTDEVDB_REC
{
   BTDBEntry            btDbEntry;
   boolean              valid; // the contents have been writen to EFS
   boolean              available; // this entry is free in database

}OEMBTDEVDB_REC;


typedef struct OEMBTDEVDB_BOND_INFO
{
   bt_bd_addr_type     bdAddr;
   BTResult*           pResult;
   ACONTEXT*           pac;
   boolean             bCmdDoneSuccess;
}OEMBTDEVDB_BOND_INFO;

struct OEMBTDEVDB
{
   // App id that is given by the Bt driver
   bt_app_id_type      appId; 
   AEECallback*        pcbResult;
   uint32              nRefs;

   OEMBTDEVDB_REC*     pDbRecs;
};

//==========================================================================
//   Function prototypes
//==========================================================================
static void OEMBTDEVDB_Cancel( AEECallback* pcb );
static void OEMBTDB_EventCallback(bt_ev_msg_type* bt_ev_msg_ptr);
static void OEMBTDEVDB_initDevices(OEMBTDEVDB_REC* arrDbRecs );
static void OEMBTDEVDB_initDevice( BTDBEntry* pDev );
static void OEMBTDEVDB_FromOEMToCore(bt_device_type* pDev,
                                     BTDBEntry* pDbEntry);
static void OEMBTDEVDB_copyFromCoreToOEM(BTDBEntry* pDbEntry,
                                         bt_device_type* pDevice);
static BTResult OEMBTDEVDB_CheckCmdStatus( bt_cmd_status_type stat );
static int OEMBTDEVDB_CmdStatusToAEEError( bt_cmd_status_type stat );
static int OEMBTDEVDB_loadBTDataBase(OEMBTDEVDB* pMe);
static OEMBTDEVDB_REC* OEMBTDEVDB_searchDeviceRecord(
                                                    const BDAddress* pBDAddr,
                                                    OEMBTDEVDB_REC* arrDbRecs );
static OEMBTDEVDB* OEMBTDevDB_GetObj( BTHandle hBT );
static void OEMBTDB_OnDeviceUpdated(bt_ev_msg_type* bt_ev_msg_ptr);
static boolean OEMBTDEVDB_GetConnectionState(BDAddress* bdAddr);
static boolean OEMBTDevDB_MatchFound(OEMBTDEVDB_REC* pDbRec, OEMBTDEV_ITERATOR* iter );
//==========================================================================
//   Static data
//==========================================================================
static OEMBTDEVDB *gpOEMBTDevDB;
static rex_crit_sect_type    gOEMBTDevDBCritSect; 
static AEEInstance gpOEMBTDEVDBInstances[MAX_DB_INSTANCE];

static AEEInstanceList gOEMBTDEVDB_HandleList 
= { gpOEMBTDEVDBInstances, sizeof(gpOEMBTDEVDBInstances)/
   sizeof(gpOEMBTDEVDBInstances[0]), 0};

/*======================================================================= 
=======================================================================*/
int OEMBTDEVDB_New( BTHandle* phBT )
{
   bt_cmd_status_type stat;
   int result = EFAILED;

   if (gpOEMBTDevDB)
   {
      ++gpOEMBTDevDB->nRefs;
      *phBT = gBTHandle; 
      result = SUCCESS;

   }
   else
   {
      gpOEMBTDevDB = (OEMBTDEVDB*)OEM_Malloc(sizeof(OEMBTDEVDB)
                                             + ( sizeof(OEMBTDEVDB_REC) * OEM_BT_MAX_DEVICES ));


      if ( NULL == gpOEMBTDevDB )
      {
         MSG_ERROR("IBT DB Failed to allocate memory for oem obj ",0, 0, 0);
         return ENOMEMORY;
      }

      gpOEMBTDevDB->pDbRecs = (OEMBTDEVDB_REC*) (gpOEMBTDevDB +1);


      if ( AEEHandle_To( &gOEMBTDEVDB_HandleList,
                         (OEMINSTANCE *)gpOEMBTDevDB, &gBTHandle  ) == FALSE )
      {
         OEM_Free( gpOEMBTDevDB );
         gpOEMBTDevDB = NULL;
         MSG_ERROR("IBT DB Failed to get a handle for database object",0, 0, 0);
         return EFAILED;
      }

      gpOEMBTDevDB->nRefs = 1;

      gpOEMBTDevDB->pcbResult      = NULL; 

      OEMBTDEVDB_initDevices(gpOEMBTDevDB->pDbRecs);

      gpOEMBTDevDB->appId     = bt_cmd_ec_get_application_id(  );


      if ( BT_APP_ID_NULL == gpOEMBTDevDB->appId )
      {
         OEM_Free( gpOEMBTDevDB );
         gpOEMBTDevDB = NULL;

         MSG_ERROR("IBT DB  Cannot reg app with BT ",0, 0, 0); 
         return EFAILED;
      }


      stat = bt_cmd_ec_register_event_set( gpOEMBTDevDB->appId,
                                           OEMBTDB_EventCallback,
                                           BT_EC_ES_RADIO_MANAGER,
                                           BT_EV_NULL,
                                           BT_EV_NULL );

      if ( stat == BT_CS_GN_SUCCESS )
      {

         stat = bt_cmd_ec_register_event_set( gpOEMBTDevDB->appId,
                                              OEMBTDB_EventCallback,
                                              BT_EC_ES_CUSTOM,
                                              BT_EV_GN_CMD_DONE, 
                                              BT_EV_GN_CMD_DONE );

         if ( stat == BT_CS_GN_SUCCESS )
         {

            stat = bt_cmd_ec_register_event_set( gpOEMBTDevDB->appId,
                                                 OEMBTDB_EventCallback,
                                                 BT_EC_ES_CUSTOM,
                                                 BT_EV_RM_BONDED,
                                                 BT_EV_RM_BOND_FAILED );
            if ( stat == BT_CS_GN_SUCCESS )
            {

               stat = bt_cmd_ec_reg_event_set_any_app_id(
                                                        gpOEMBTDevDB->appId,
                                                        OEMBTDB_EventCallback,
                                                        BT_EC_ES_CUSTOM,
                                                        BT_EV_RM_DEVICE_UPDATED,
                                                        BT_EV_RM_DEVICE_UPDATED );

               if ( stat == BT_CS_GN_SUCCESS )
               {
                  result = OEMBTDEVDB_loadBTDataBase(gpOEMBTDevDB);

                  bt_cmd_rm_auto_service_search_disable( gpOEMBTDevDB->appId,
                                                         TRUE
                                                         );
               }

            }

         }

      }

      if ( result != SUCCESS )
      {
         // we were not able to register with BT or load the DB
         OEMBTDevDB_Release(*phBT);
      }

      *phBT = gBTHandle;
   }
   return result;
}


/*======================================================================= 
=======================================================================*/
void OEMBTDevDB_Release( BTHandle hBT )
{
   OEMBTDEVDB*          pMe = OEMBTDevDB_GetObj(hBT);
   uint32 uRefs;

   if ( pMe == NULL )
   {
      return;
   }

   if ( pMe->nRefs == 0 )
   {
      return;
   }

   MSG_MED( "IBT DB Decrementing ref count : ref count = 0x%x ",\
            pMe->nRefs,0,0);

   uRefs = --pMe->nRefs;

   if ( uRefs == 0 )
   {
      // as the global pointer is accessed from BT 
      // task for event callback.
      rex_enter_crit_sect( &gOEMBTDevDBCritSect );

      gpOEMBTDevDB = NULL;

      while ( pMe->pcbResult != NULL )
      {
         CALLBACK_Cancel( pMe->pcbResult);
      }

      // leave critic section
      rex_leave_crit_sect( &gOEMBTDevDBCritSect );

      if ( pMe->appId != BT_APP_ID_NULL )
      {

         bt_cmd_rm_auto_service_search_disable( pMe->appId,
                                                FALSE
                                               );

         bt_cmd_ec_unregister_event_set( pMe->appId,
                                         BT_EC_ES_RADIO_MANAGER,
                                         BT_EV_NULL,
                                         BT_EV_NULL );


         bt_cmd_ec_unregister_event_set( pMe->appId,
                                         BT_EC_ES_CUSTOM,
                                         BT_EV_GN_CMD_DONE,
                                         BT_EV_GN_CMD_DONE );

         bt_cmd_ec_unregister_event_set( pMe->appId,
                                         BT_EC_ES_CUSTOM,
                                         BT_EV_RM_BONDED,
                                         BT_EV_RM_BOND_FAILED );

         bt_cmd_ec_unreg_event_set_any_app_id( pMe->appId,
                                               BT_EC_ES_CUSTOM,
                                               BT_EV_RM_DEVICE_UPDATED,
                                               BT_EV_RM_DEVICE_UPDATED );

         
         bt_cmd_ec_free_application_id( pMe->appId );
      }

      

      
      MSG_MED("IBT DB Releasing OEM DB object ",0, 0, 0);

      OEM_Free( pMe);

      AEEHandle_Clear( &gOEMBTDEVDB_HandleList, hBT );

      gBTHandle = 0xFFFF;
   }
   return;
}

/*======================================================================= 
=======================================================================*/
static void OEMBTDEVDB_initDevices(OEMBTDEVDB_REC* arrDbRecs )
{
   uint8  i;

   //  Initialize the OEM device data base  
   for ( i = 0; i < OEM_BT_MAX_DEVICES; i++ )
   {
      arrDbRecs[ i ].valid        = FALSE;
      arrDbRecs[ i ].available    = TRUE;

      OEMBTDEVDB_initDevice( &arrDbRecs[ i ].btDbEntry );
   }

   MSG_LOW("IBT DB OEM device databse initialisation complete ",0, 0, 0);
}
/*======================================================================= 
=======================================================================*/
static void OEMBTDEVDB_initDevice( BTDBEntry* pDev )
{
   BTDeviceInfo*   pDevInfo = &pDev->deviceInfo;

   pDev->bBonded                    = FALSE;    
   pDev->security                   = AEEBT_SEC_NONE;
   pDev->bConnected                 = FALSE; 

   pDev->u16Value1                  = 0;
   pDev->u16Value2                  = 0;
   pDev->u16Value3                  = 0;
   pDev->u16Value4                  = 0;

   pDevInfo->LMPVersion.VersNum     = 0;
   pDevInfo->LMPVersion.CompID      = 0;
   pDevInfo->LMPVersion.SubVersNum  = 0;

   pDevInfo->COD.COS                = BT_SERVICE_CLASS_UNKNOWN;
   pDevInfo->COD.DevCls             = BT_SD_MAJOR_DEVICE_CLASS_UNCLASSIFIED;
   pDevInfo->COD.SubDevCls          = BT_SD_MINOR_DEVICE_CLASS_UNCLASSIFIED;

   MEMSET( (void*)(&pDevInfo->bdAddr), 0,sizeof( BDAddress) );

   MEMSET( (void*)(&pDevInfo->LMPFeatures), 0,sizeof( BTLMPFeatures ) );

   MEMSET( (void*)(pDevInfo->szName), 0, (AEEBT_MAX_LEN_DEVICE_NAME + 1) );

   MEMSET( (void*)pDev->szNickName, 0, (AEEBT_MAX_LEN_NICK_NAME + 1) );
}

/*======================================================================= 
   This function synchronizes OEMDevDB database with the RM database.
   Function needs to be invoked inside a critical section to avoid 
   database corruption.
=======================================================================*/
static int OEMBTDEVDB_loadBTDataBase(OEMBTDEVDB* pMe)
{
   bt_cmd_status_type       status;
   bt_rm_dev_iterator_type  devIter;
   bt_device_type           device;
   uint16                   devCount;

   status          = BT_CS_GN_SUCCESS;
   devIter.reset   = TRUE;
   devIter.control = (bt_rm_iter_ctrl_enum_type)
                      (BT_RM_IC_ALL | BT_RM_IC_INCLUDE_VOLATILE_B);

   devCount = 0;

   while ( BT_CS_GN_SUCCESS == status)
   {
      status = bt_cmd_rm_device_read_iter( pMe->appId,
                                           &devIter,
                                           &device,
                                           NULL,
                                           NULL);

      if ( (BT_CS_GN_SUCCESS == status ) && (OEM_BT_MAX_DEVICES != devIter.index) )
      {
         pMe->pDbRecs[devCount].valid = device.valid;
         pMe->pDbRecs[devCount].available = FALSE;

         OEMBTDEVDB_copyFromCoreToOEM(&pMe->pDbRecs[ devCount ].btDbEntry,
                                      &device);

         devCount++;
      }
      else
      {
         break;
      }

      devIter.reset = FALSE;
   }

   return OEMBTDEVDB_CmdStatusToAEEError(status);
}

/*======================================================================= 
=======================================================================*/
BTResult OEMBTDevDB_Add( BTHandle hBT,
                         const BTDeviceInfo* pDeviceInfo,
                         IShell* pIShell)
{
   OEMBTDEVDB_REC*     pDev = NULL;
   int i;
   bt_device_type      device;
   bt_cmd_status_type  result;
   int                 status;
   OEMBTDEVDB          *pMe = OEMBTDevDB_GetObj(hBT);
   BTDBEntry           dbEntry;


   if ( (NULL == pMe) || (NULL == pDeviceInfo) ||
        (!AEE_CHECK_PTR("OEMBTDEVDB_Add", pDeviceInfo, 
                        sizeof(BTDeviceInfo), TRUE)) )
   {
      return AEEBT_RESULT_BAD_PARAM;
   }

   rex_enter_crit_sect( &gOEMBTDevDBCritSect );

   pDev = OEMBTDEVDB_searchDeviceRecord(&pDeviceInfo->bdAddr,
                                        pMe->pDbRecs);

   if ( (NULL != pDev) && (TRUE == pDev->valid) )
   {
      // device found.
      MSG_MED("IBT DB Entry already exists in database ",0, 0, 0);
      status = AEEBT_RESULT_ENTRY_EXISTS;

   }
   else
   {
      if ( pDev == NULL )
      {
         // device not found. see if there is space available
         for ( i = 0; i < OEM_BT_MAX_DEVICES; i++ )
         {
            if ( TRUE == pMe->pDbRecs[ i ].available )
            {
               pDev = &pMe->pDbRecs[ i ];
               break;
            }
         }
      }

      if ( NULL == pDev )
      {
         MSG_ERROR("IBT DB Database full ",0, 0, 0);
         status = AEEBT_RESULT_DB_FULL;
      }
      else
      {
         MEMSET( (void*)(&dbEntry), 0, sizeof(BTDBEntry) );
         dbEntry.deviceInfo = *pDeviceInfo;

         OEMBTDEVDB_FromOEMToCore( &device, &dbEntry );
         device.valid   = TRUE;
         result  = bt_cmd_rm_device_add( pMe->appId, &device );

         status = OEMBTDEVDB_CheckCmdStatus( result );

         if ( status == AEEBT_RESULT_SUCCESS )
         {
            //add entry to db 
            pDev->btDbEntry.deviceInfo        = *pDeviceInfo;
            //  data has been added to persistent data base
            pDev->valid                       = TRUE; 
            pDev->available                   = FALSE;
         }
      }

   }
   rex_leave_crit_sect( &gOEMBTDevDBCritSect );

   return status;
}

/*======================================================================= 
=======================================================================*/
BTResult OEMBTDevDB_Read(BTHandle hBT, BTDBEntry* pEntry)
{
   BTResult            stat = AEEBT_RESULT_SUCCESS;
   OEMBTDEVDB_REC*     pDBInfo = NULL;
   uint16              i;
   OEMBTDEVDB*          pMe = OEMBTDevDB_GetObj(hBT);

   if ( (NULL == pMe) || (NULL == pEntry) ||
        (!AEE_CHECK_PTR( "IBTDEVDB_Read", pEntry, 
                         sizeof(BTDBEntry), TRUE)) )
   {
      return AEEBT_RESULT_BAD_PARAM ;
   }

   rex_enter_crit_sect( &gOEMBTDevDBCritSect );

   for ( i = 0; i < OEM_BT_MAX_DEVICES; i++ )
   {
      if ( BT_BD_ADDRS_EQUAL(&pMe->pDbRecs[ i ].btDbEntry.deviceInfo.bdAddr,
                             &pEntry->deviceInfo.bdAddr ) != FALSE )
      {
         if (pMe->pDbRecs[ i ].valid == TRUE )
         {
            pDBInfo = &pMe->pDbRecs[ i ];
            break;
         }
      }
   }

   if ( NULL != pDBInfo )
   {
      *pEntry = pDBInfo->btDbEntry;
      pEntry->bConnected = 
      OEMBTDEVDB_GetConnectionState(&pEntry->deviceInfo.bdAddr);
   }
   else
   {
      MSG_HIGH("IBT DB Entry not found in database ",0, 0, 0);
      stat = AEEBT_RESULT_ENTRY_NOT_FOUND;
   }

   rex_leave_crit_sect( &gOEMBTDevDBCritSect );

   return stat;
}

/*======================================================================= 
=======================================================================*/
BTResult OEMBTDevDB_EnumNext( BTHandle hBT,
                              BTDBEntry*     pEntry,
                              OEMBTDEV_ITERATOR* iter )

{
   OEMBTDEVDB*  pMe = OEMBTDevDB_GetObj(hBT);
   BTResult    result = AEEBT_RESULT_ENTRY_NOT_FOUND;
   int         i;


   if ( (NULL == pMe) || (NULL == pEntry) ||
        (!AEE_CHECK_PTR( "OEMBTDEVDB_EnumNext", pEntry, 
                         sizeof(BTDBEntry), TRUE)) )
   {
      return AEEBT_RESULT_BAD_PARAM;
   }

   if ( ( iter->index == DEV_DB_INVALID_INDX ) ||
        ( iter->index > OEM_BT_MAX_DEVICES )
      )
   {
      return AEEBT_RESULT_NOT_INITIALIZED;
   }

   rex_enter_crit_sect( &gOEMBTDevDBCritSect );

   for ( i = iter->index; i < OEM_BT_MAX_DEVICES; i++ )
   {
      if ( OEMBTDevDB_MatchFound(&pMe->pDbRecs[i], iter) == TRUE )
      {
         break;
      }
   }

   if ( i == OEM_BT_MAX_DEVICES )
   {
      result = AEEBT_RESULT_ENTRY_NOT_FOUND;
      iter->index = DEV_DB_INVALID_INDX;
   }
   else
   {
      *pEntry = pMe->pDbRecs[i].btDbEntry;
      pEntry->bConnected = 
      OEMBTDEVDB_GetConnectionState(&pEntry->deviceInfo.bdAddr);

      iter->index = i+1;
      result = AEEBT_RESULT_SUCCESS;
   }

   rex_leave_crit_sect( &gOEMBTDevDBCritSect );
   return result;
}

/*======================================================================= 
=======================================================================*/
static boolean OEMBTDevDB_MatchFound(OEMBTDEVDB_REC* pRec,
                                     OEMBTDEV_ITERATOR* iter )
{
   BTEnumCriteria matchStatus = 0;
   BTEnumCriteria whatToMatch = iter->devEnum.what2match;
   int i;
   boolean bFlag = pRec->valid;

   if ( bFlag != FALSE )
   {
      if ( (whatToMatch & AEEBT_EC_ALL ) == AEEBT_EC_ALL )
      {
         matchStatus = AEEBT_EC_ALL;
      }

      if ( (whatToMatch & AEEBT_EC_COS ) == AEEBT_EC_COS )
      {
         if (iter->devEnum.cos == pRec->btDbEntry.deviceInfo.COD.COS)
         {
            matchStatus |= AEEBT_EC_COS;
         }
      }

      if ( (whatToMatch & AEEBT_EC_BONDED) == AEEBT_EC_BONDED)
      {
         if (iter->devEnum.bBonded == pRec->btDbEntry.bBonded )
         {
            matchStatus |= AEEBT_EC_BONDED;
         }
      }

      if ( (whatToMatch & AEEBT_EC_BD_ADDR) == AEEBT_EC_BD_ADDR )
      {

         for ( i = 0; i < sizeof( BDAddress ); i++ )
         {
            if ( (iter->devEnum.bdAddrMask.uAddr[ i ] &
                  iter->devEnum.bdAddrValue.uAddr[ i ] ) !=
                 (pRec->btDbEntry.deviceInfo.bdAddr.uAddr[i] &
                  iter->devEnum.bdAddrMask.uAddr[ i ]) )
            {
               break;
            }
         }

         if ( i == sizeof( BDAddress ) )
         {
            matchStatus |= AEEBT_EC_BD_ADDR;
         }
      }

      if (( whatToMatch & AEEBT_EC_VALUE_1) == AEEBT_EC_VALUE_1)
      {
         if (iter->devEnum.u16Value1 == pRec->btDbEntry.u16Value1 )
         {
            matchStatus |= AEEBT_EC_VALUE_1;
         }
      }

      if ( (whatToMatch & AEEBT_EC_VALUE_2 ) == AEEBT_EC_VALUE_2 )
      {
         if ( iter->devEnum.u16Value2 == pRec->btDbEntry.u16Value2 )
         {
            matchStatus |= AEEBT_EC_VALUE_2;
         }
      }

      if ( (whatToMatch & AEEBT_EC_VALUE_3 ) == AEEBT_EC_VALUE_3 )
      {
         if ( iter->devEnum.u16Value3 == pRec->btDbEntry.u16Value3 )
         {
            matchStatus |= AEEBT_EC_VALUE_3;
         }
      }

      if ( (whatToMatch & AEEBT_EC_VALUE_4 ) == AEEBT_EC_VALUE_4 )
      {
         if ( iter->devEnum.u16Value4 == pRec->btDbEntry.u16Value4 )
         {
            matchStatus |= AEEBT_EC_VALUE_4;
         }
      }
   }

   if ( ( pRec->valid == TRUE ) && ( whatToMatch == matchStatus ) )
      return TRUE;
   else
      return FALSE;

}

/*======================================================================= 
=======================================================================*/
static boolean OEMBTDEVDB_GetConnectionState(BDAddress* bdAddr)
{
   bt_rm_link_status_type   linkStat;

   linkStat.bd_addr = *((bt_bd_addr_type*)bdAddr);

   if ( bt_cmd_rm_get_link_status( gpOEMBTDevDB->appId, 
                                   &linkStat ) == BT_CS_GN_SUCCESS )
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}

/*======================================================================= 
=======================================================================*/
OEMBTDEVDB_REC* OEMBTDEVDB_searchDeviceRecord(const BDAddress* pBDAddr,
                                              OEMBTDEVDB_REC* pBTDBEntries )
{
   OEMBTDEVDB_REC*  pDbInfo = NULL;
   uint16 i;

   for ( i = 0; i < OEM_BT_MAX_DEVICES; i++ )
   {
      if ( BT_BD_ADDRS_EQUAL(&pBTDBEntries[ i ].btDbEntry.deviceInfo.bdAddr,
                             pBDAddr ) != FALSE )
      {
         pDbInfo = &pBTDBEntries[ i ];
         break;
      }
   }

   return pDbInfo;
}

/*======================================================================= 
=======================================================================*/
BTResult OEMBTDevDB_Update( BTHandle         hBT,
                            BTModDevField    modField,
                            const BTDBEntry* pEntry,
                            IShell* pIShell

                          )
{
   bt_cmd_status_type  stat = BT_CS_GN_SUCCESS;
   bt_device_type      device;
   OEMBTDEVDB_REC*     pInfo  = NULL;
   OEMBTDEVDB*         pMe = OEMBTDevDB_GetObj(hBT);
   const BDAddress*    pBDAddr = &pEntry->deviceInfo.bdAddr;
   BTResult            retVal = AEEBT_RESULT_SUCCESS;

   if ( (NULL == pMe) || (NULL == pEntry) ||
        (!AEE_CHECK_PTR( "OEMBTDEVDB_Update", pEntry, 
                         sizeof(BTDBEntry), TRUE)) )
   {
      return AEEBT_RESULT_BAD_PARAM;
   }

   if ( ISHELL_CheckPrivLevel( pIShell, PL_SYSTEM, TRUE ) != TRUE )
   {
      return EPRIVLEVEL;
   }

   rex_enter_crit_sect( &gOEMBTDevDBCritSect );

   pInfo = OEMBTDEVDB_searchDeviceRecord( pBDAddr, pMe->pDbRecs );

   if ( (NULL == pInfo) || (FALSE == pInfo->valid) )
   {
      rex_leave_crit_sect( &gOEMBTDevDBCritSect );
      return AEEBT_RESULT_ENTRY_NOT_FOUND;
   }

   switch ( modField )
   {
   case AEEBT_MDF_BONDED_B:
      { 
         BTDBEntry *pDbEntry = &pInfo->btDbEntry;

         bt_rm_dev_update_ctrl_type  update_bitmap = BT_RM_DU_UNBOND_B;

         if ( TRUE == pDbEntry->bBonded )
         {
            device.bd_addr = *((bt_bd_addr_type*)&pEntry->deviceInfo.bdAddr);

            stat = bt_cmd_rm_device_update( pMe->appId,
                                            update_bitmap, &device );
            if ( ( stat == BT_CS_GN_SUCCESS ) ||
                 ( stat == BT_CS_GN_PENDING ))
            {
               pDbEntry->bBonded = FALSE;
            }
         }
         else
         {
            retVal = AEEBT_RESULT_DEV_NOT_BONDED;
         }
         break;
      }
   case AEEBT_MDF_SEC_B:
      {
         BTDBEntry *pDbEntry = &pInfo->btDbEntry;

         if ( pDbEntry->security > BT_SEC_MAX )
         {
            stat = BT_CS_RM_BAD_PARAM;
         }
         else
         {
            stat = bt_cmd_rm_set_device_security( pMe->appId,
                                                  (bt_bd_addr_type*)pBDAddr,
                                                  (bt_security_type)pEntry->security );

            if ( ( stat == BT_CS_GN_SUCCESS ) ||
                 ( stat == BT_CS_GN_PENDING ))
            {
               pDbEntry->security = pEntry->security;
            }
         }

         break;
      }

   case AEEBT_MDF_VALID_B:
      {

         stat = bt_cmd_rm_device_remove( pMe->appId,
                                         (bt_bd_addr_type*)pBDAddr );

         if ( ( stat == BT_CS_GN_SUCCESS ) ||
              ( stat == BT_CS_GN_PENDING ))
         {
            pInfo->valid     = FALSE;
            pInfo->available = TRUE;
         }

         break;
      }

   case AEEBT_MDF_VAL1_B:
      {
         BTDBEntry *pDbEntry = &pInfo->btDbEntry;
         bt_rm_dev_update_ctrl_type  update_bitmap = BT_RM_DU_VALUE_1_B;

         device.bd_addr = *((bt_bd_addr_type*)&pEntry->deviceInfo.bdAddr);
         device.value_1 = pEntry->u16Value1;

         stat = bt_cmd_rm_device_update( pMe->appId,
                                         update_bitmap,
                                         (bt_device_type*)&device);

         if ( ( stat == BT_CS_GN_SUCCESS ) ||
              ( stat == BT_CS_GN_PENDING ) )
         {
            pDbEntry->u16Value1 = pEntry->u16Value1;
         }
         break;
      }
   case AEEBT_MDF_VAL2_B:
      {
         BTDBEntry *pDbEntry = &pInfo->btDbEntry;
         bt_rm_dev_update_ctrl_type  update_bitmap = BT_RM_DU_VALUE_2_B;

         device.bd_addr = *((bt_bd_addr_type*)&pEntry->deviceInfo.bdAddr);
         device.value_2 = pEntry->u16Value2;

         stat = bt_cmd_rm_device_update( pMe->appId,
                                         update_bitmap,
                                         (bt_device_type*)&device);

         if ( ( stat == BT_CS_GN_SUCCESS ) ||
              ( stat == BT_CS_GN_PENDING ) )
         {
            pDbEntry->u16Value2 = pEntry->u16Value2;
         }
         break;
      }
   case AEEBT_MDF_VAL3_B:
      {
         BTDBEntry *pDbEntry = &pInfo->btDbEntry;
         bt_rm_dev_update_ctrl_type  update_bitmap = BT_RM_DU_VALUE_3_B;

         device.bd_addr = *((bt_bd_addr_type*)&pEntry->deviceInfo.bdAddr);
         device.value_3 = pEntry->u16Value3;

         stat = bt_cmd_rm_device_update( pMe->appId,
                                         update_bitmap,
                                         (bt_device_type*)&device);

         if ( ( stat == BT_CS_GN_SUCCESS ) ||
              ( stat == BT_CS_GN_PENDING ))
         {
            pDbEntry->u16Value3 = pEntry->u16Value3;
         }
         break;
      }
   case AEEBT_MDF_VAL4_B:
      {
         BTDBEntry *pDbEntry = &pInfo->btDbEntry;
         bt_rm_dev_update_ctrl_type  update_bitmap = BT_RM_DU_VALUE_4_B;

         device.bd_addr = *((bt_bd_addr_type*)&pEntry->deviceInfo.bdAddr);
         device.value_4 = pEntry->u16Value4;

         stat = bt_cmd_rm_device_update( pMe->appId,
                                         update_bitmap,
                                         (bt_device_type*)&device);

         if ( ( stat == BT_CS_GN_SUCCESS ) ||
              ( stat == BT_CS_GN_PENDING ))
         {
            pDbEntry->u16Value4 = pEntry->u16Value4;
         }
         break;
      }
   case AEEBT_MDF_NICKNAME_B:
      {
         BTDBEntry *pDbEntry = &pInfo->btDbEntry;
         uint8 len;
         bt_rm_dev_update_ctrl_type  update_bitmap = BT_RM_DU_NICK_NAME_B;

         device.bd_addr = *((bt_bd_addr_type*)&pEntry->deviceInfo.bdAddr);

         len = STRLEN((char *)pEntry->szNickName);

         if ( len > BT_MAX_REMOTE_NICK_NAME_LEN )
         {
            len = BT_MAX_REMOTE_NICK_NAME_LEN;
         }

         MEMCPY((void*)&device.nick_name_str[0],(void*)&pEntry->szNickName[0], len );
         device.nick_name_str[len] = 0; 

         stat = bt_cmd_rm_device_update( pMe->appId,
                                         update_bitmap,
                                         (bt_device_type*)&device);

         if ( ( stat == BT_CS_GN_SUCCESS ) ||
              ( stat == BT_CS_GN_PENDING ))
         {

            MEMCPY(pDbEntry->szNickName, pEntry->szNickName,len);
            pDbEntry->szNickName[len] = 0;
         }
         break;
      }
   default :
      stat = BT_CS_RM_BAD_PARAM;
      break;

   }
   rex_leave_crit_sect( &gOEMBTDevDBCritSect );

   if ( retVal != AEEBT_RESULT_DEV_NOT_BONDED )
   {
      retVal = OEMBTDEVDB_CheckCmdStatus( stat );
   }

   return retVal;
}


/*======================================================================= 
=======================================================================*/
static void OEMBTDEVDB_copyFromCoreToOEM(BTDBEntry*      pDbEntry,
                                         bt_device_type* pDevice)
{
   BTDeviceInfo*       pDevInfo = &pDbEntry->deviceInfo;
   uint16              len;

   pDbEntry->bBonded                    = pDevice->bonded;    

   pDbEntry->security                   = (BTSecurityLevel) pDevice->security;

   pDbEntry->u16Value1                  = pDevice->value_1;
   pDbEntry->u16Value2                  = pDevice->value_2;
   pDbEntry->u16Value3                  = pDevice->value_3;
   pDbEntry->u16Value4                  = pDevice->value_4;

   pDevInfo->LMPVersion.VersNum     = pDevice->version.lmp_version;
   pDevInfo->LMPVersion.CompID      = pDevice->version.manufacturer_name;
   pDevInfo->LMPVersion.SubVersNum  = pDevice->version.lmp_subversion;

   bt_sd_parse_class_of_device( &pDevice->class_of_device,
                                (uint16*)&pDevInfo->COD.COS, 
                                (uint16*)&pDevInfo->COD.DevCls,
                                (uint16*)&pDevInfo->COD.SubDevCls );

   pDevInfo->bdAddr = *((BDAddress*)&pDevice->bd_addr);
   pDbEntry->bConnected = OEMBTDEVDB_GetConnectionState(&pDevInfo->bdAddr);


   len = STRLEN((char*)pDevice->name_str);

   if (len > AEEBT_MAX_LEN_DEVICE_NAME)
   {
      len = AEEBT_MAX_LEN_DEVICE_NAME;
   }

   MEMCPY((void*)pDevInfo->szName,
          (void*)pDevice->name_str, len );

   pDevInfo->szName[len] = 0;

   len = STRLEN((char*)pDevice->nick_name_str);

   if (len > AEEBT_MAX_LEN_NICK_NAME)
   {
      len = AEEBT_MAX_LEN_NICK_NAME;
   }

   MEMCPY((void*)pDbEntry->szNickName,
          (void*)pDevice->nick_name_str, len );

   pDbEntry->szNickName[len] = 0;

   return;

}

/*======================================================================= 
=======================================================================*/
static void OEMBTDEVDB_FromOEMToCore(bt_device_type* pDev,
                                     BTDBEntry*      pDbEntry)
{
   uint16              len;
   BTDeviceInfo*       pDevInfo = &pDbEntry->deviceInfo;


   pDev->bonded = pDbEntry->bBonded;
   pDev->security = (bt_security_type)pDbEntry->security;

   pDev->value_1  =  pDbEntry->u16Value1;
   pDev->value_2  =  pDbEntry->u16Value2;
   pDev->value_3  =  pDbEntry->u16Value3;
   pDev->value_4  =  pDbEntry->u16Value4;

   pDev->version.lmp_version       = pDevInfo->LMPVersion.VersNum;
   pDev->version.manufacturer_name = pDevInfo->LMPVersion.CompID;
   pDev->version.lmp_subversion    = pDevInfo->LMPVersion.SubVersNum;


   bt_sd_build_class_of_device(pDevInfo->COD.COS, 
                               pDevInfo->COD.DevCls, 
                               pDevInfo->COD.SubDevCls, 
                               &pDev->class_of_device );

   pDev->bd_addr = *((bt_bd_addr_type*)&pDevInfo->bdAddr);

   len = STRLEN((char*)pDevInfo->szName);


   if (len > BT_SD_MAX_DEVICE_NAME_LEN)
   {
      len = BT_SD_MAX_DEVICE_NAME_LEN;
   }

   MEMCPY((void*)pDev->name_str,
          (void*)pDevInfo->szName, len );

   pDev->name_str[len] = 0;

   len = STRLEN((char*)pDbEntry->szNickName);

   if (len > BT_MAX_REMOTE_NICK_NAME_LEN)
   {
      len = BT_MAX_REMOTE_NICK_NAME_LEN;
   }

   MEMCPY((void*)pDev->nick_name_str,
          (void*)pDbEntry->szNickName, len );

   pDev->nick_name_str[len] = 0;
   return;
}

/*======================================================================= 
=======================================================================*/
int OEMBTDevDB_Bond( BTHandle          hBT,
                     const BDAddress*  pBDAddr,
                     const uint8*      pPin,
                     boolean           bInitiating,
                     BTResult*         pResult,
                     AEECallback*      pResultCb)

{
   bt_cmd_status_type    stat;
   bt_pin_code_type      pin; // note: of len BT_MAX_PIN_CODE_LEN
   uint8                 i;
   boolean               pin_valid = FALSE;
   OEMBTDEVDB*           pMe = OEMBTDevDB_GetObj(hBT);
   OEMBTDEVDB_BOND_INFO* pBondInfo = NULL;



   if ( ( pMe == NULL ) || ( pBDAddr == NULL ) || ( pPin == NULL ) ||
        ( pResult == NULL ) ||
        ( !AEE_CHECK_PTR( "OEMBTDEVDB_Bond", pResult, 
                          sizeof(BTResult), TRUE)) || 
        ( !AEE_CHECK_PTR( "OEMBTDEVDB_Bond", pResultCb, 
                          sizeof( AEECallback ), TRUE )) ||
        ( pResultCb->pfnNotify == NULL ) )
   {
      return EBADPARM;
   }

   pin.length = STRLEN( (char*)pPin );

   if ( pin.length > BT_MAX_PIN_CODE_LEN )
   {
      MSG_ERROR("IBT DB Pin length invalid ",0,0,0);
      return EBADPARM;
   }

   for ( i = 0; i < BT_MAX_PIN_CODE_LEN; i++ )
   {
      pin.pin_code[i] = 0;
   }

   // copy without the \0 
   MEMCPY( (void*)&pin.pin_code,(void*) pPin, pin.length );

   CALLBACK_Cancel(pResultCb);
   // queue callback

   pBondInfo = OEM_Malloc (sizeof ( OEMBTDEVDB_BOND_INFO ) );

   if ( pBondInfo == NULL )
   {
      return ENOMEMORY;
   }
   pBondInfo->bdAddr = *((bt_bd_addr_type*)pBDAddr);

   pBondInfo->pac =  AEE_GetAppContext();
   pBondInfo->pResult = pResult;
   pBondInfo->bCmdDoneSuccess = FALSE;


   pResultCb->pCancelData  = &pMe->pcbResult;
   pResultCb->pfnCancel    = OEMBTDEVDB_Cancel;

   pResultCb->pmc          = (void*)pBondInfo; 
   pResultCb->pNext        = pMe->pcbResult;
   pMe->pcbResult          = pResultCb;

   if ( TRUE == bInitiating )
   {
      stat = bt_cmd_rm_bond( pMe->appId, (bt_bd_addr_type*)pBDAddr, &pin );
   }
   else
   {
      pin_valid = ( pin.length > 0 ) ? TRUE : FALSE;

      stat = bt_cmd_rm_pin_reply( pMe->appId, (bt_bd_addr_type*)pBDAddr, 
                                  pin_valid, &pin );
   }

   return OEMBTDEVDB_CmdStatusToAEEError( stat );
}

static int OEMBTDEVDB_CmdStatusToAEEError( bt_cmd_status_type stat )
{
   switch (stat)
   {
   case BT_CS_GN_SUCCESS:
   case BT_CS_GN_PENDING:
      return SUCCESS;

   case BT_CS_GN_CMD_Q_FULL:
      return ENOMEMORY;

   case BT_CS_RM_BAD_PARAM :
   case BT_CS_RM_UNRECOGNIZED_CMD:
      return EBADPARM;
  
   default:
      return EFAILED;
   }

}
/*======================================================================= 
=======================================================================*/
static BTResult OEMBTDEVDB_CheckCmdStatus( bt_cmd_status_type stat )
{
   switch (stat)
   {
   case BT_CS_GN_SUCCESS:
   case BT_CS_GN_PENDING:
      return AEEBT_RESULT_SUCCESS;

   case BT_CS_GN_CMD_Q_FULL:
      return AEEBT_RESULT_NO_RESOURCE;

   case BT_CS_RM_NO_DEVICE:
      return AEEBT_RESULT_ENTRY_NOT_FOUND;

   case BT_CS_RM_BAD_PARAM :
      return AEEBT_RESULT_BAD_PARAM;

   case BT_CS_RM_UNRECOGNIZED_CMD:
      return AEEBT_RESULT_BAD_PARAM;

      // just to make the code returning this result readable.
   case AEEBT_RESULT_DEV_NOT_BONDED:
      return AEEBT_RESULT_DEV_NOT_BONDED;

   default:
      return AEEBT_RESULT_UNKNOWN_ERR;
   }
}

/*======================================================================= 
 This function synchronizes OEMDevDB database with the RM database when 
  RM database is changed.Function needs to be invoked inside a critical
   section to avoid database corruption.
=======================================================================*/
static void OEMBTDB_OnDeviceUpdated(bt_ev_msg_type* bt_ev_msg_ptr)
{
   OEMBTDEVDB_REC*  pDev = NULL;
   bt_device_type   device;
   uint8            uNumSvc;
   bt_service_type  svcs[ BT_SD_MAX_NUM_OF_SRV_REC ];
   bt_cmd_status_type  cmd_status;
   int i;

   MSG_HIGH( "IBT DB OEMBTDB_OnDeviceUpdated ",  0, 0, 0 );

   device.bd_addr = bt_ev_msg_ptr->ev_msg.ev_rm_devud.bd_addr;

   // see if the device is in the DB.
   pDev = OEMBTDEVDB_searchDeviceRecord(
                 (BDAddress*)&bt_ev_msg_ptr->ev_msg.ev_rm_devud.bd_addr,
                 gpOEMBTDevDB->pDbRecs);

   if ( bt_ev_msg_ptr->ev_msg.ev_rm_devud.update_status
                 & BT_RM_DUS_REMOVED_B ) 
   {
      if ( pDev != NULL )
      {
         // the device was removed. mark it as invalid
         pDev->available = TRUE;
         pDev->valid     = FALSE;
      }// else the device was not in the database so do nothing

   }
   else
   {
      // for all changes other than remove   
      cmd_status = bt_cmd_rm_device_read(gpOEMBTDevDB->appId,
                                         &device,
                                         &uNumSvc,
                                         svcs);

      if ( BT_CS_GN_SUCCESS == cmd_status )
      {

         // check if the device is valid
         if ( device.valid == TRUE )
         {
            MSG_MED( "IBT DB Updating DB ",  0, 0, 0 );

            if ( NULL == pDev )
            {
               // some field has changed but the device is not in OEM database
               // new device added to the database
               // see if there is space available
               for ( i = 0; i < OEM_BT_MAX_DEVICES; i++ )
               {
                  if ( TRUE == gpOEMBTDevDB->pDbRecs[ i ].available )
                  {
                     pDev = &gpOEMBTDevDB->pDbRecs[ i ];
                     break;
                  }
               }

               if ( i == OEM_BT_MAX_DEVICES )
               {
                  // no space and ideally should not be reaching here
                  MSG_HIGH("IBT DB Database Resynch ",0, 0, 0);
                  OEMBTDEVDB_initDevices(gpOEMBTDevDB->pDbRecs);
                  OEMBTDEVDB_loadBTDataBase(gpOEMBTDevDB);
               }
               else
               {
                  // updating the OEM database with BT database
                  pDev->available = FALSE;
                  pDev->valid     = TRUE;
                  OEMBTDEVDB_copyFromCoreToOEM(&pDev->btDbEntry,
                                               &device);
               }
            }
            else
            { 
               // the device is in our database, update it to the RM value
               pDev->available = FALSE;
               pDev->valid     = TRUE;
               OEMBTDEVDB_copyFromCoreToOEM(&pDev->btDbEntry,
                                            &device);
            }
         }
      }
      else
      {
         // we should not be reaching here
         MSG_ERROR( "IBT DB Error reading DB Err = 0x%x ", 
                    cmd_status, 0, 0 );
      }
   }
   return;
}
/*======================================================================= 
=======================================================================*/
static void OEMBTDB_EventCallback(bt_ev_msg_type* bt_ev_msg_ptr)
{
   boolean bCallApp = FALSE; 
   bt_bd_addr_type  bdAddr;
   boolean bCmdDoneSuccess = TRUE;
   AEECallback* ppcb;
   BTResult result = SUCCESS;

   rex_enter_crit_sect( &gOEMBTDevDBCritSect );

   if (gpOEMBTDevDB == NULL )
   {
      rex_leave_crit_sect( &gOEMBTDevDBCritSect );
      return;
   }

   switch (bt_ev_msg_ptr->ev_hdr.ev_type)
   {
   case BT_EV_GN_CMD_DONE:
      {
         bt_ev_gn_cmd_done_type* pm = &bt_ev_msg_ptr->ev_msg.ev_gn_cmd_done;

         switch ( pm->cmd_type )
         {
         case BT_CMD_RM_BOND:
            {
               if ( pm->cmd_status != BT_CS_GN_SUCCESS )
               {
                  result =  OEMBTDEVDB_CheckCmdStatus(pm->cmd_status);
                  bCmdDoneSuccess = FALSE;
                  bCallApp = TRUE;
               }
               ppcb = gpOEMBTDevDB->pcbResult;

               for ( ; ppcb != NULL; ppcb = ppcb->pNext)
               {
                  if (((OEMBTDEVDB_BOND_INFO*)(ppcb->pmc))->bCmdDoneSuccess
                      == FALSE )
                  {
                     ((OEMBTDEVDB_BOND_INFO*)(ppcb->pmc))->bCmdDoneSuccess
                     = bCmdDoneSuccess;

                     break;
                  }
               }

               break;
            } /* end of case BT_CMD_RM_BOND */
         case BT_CMD_RM_DEVICE_ADD:
         case BT_CMD_RM_DEVICE_UPDATE:
         case BT_CMD_RM_DEVICE_REMOVE:
         case BT_CMD_RM_SET_DEVICE_SECURITY:
            {
               if (pm->cmd_status != BT_CS_GN_SUCCESS )
               {
                  MSG_HIGH( "IBT DB Error Updating record in the BT DB "
                            ,0, 0, 0 );
                  MSG_HIGH("IBT DB Database Resynch ",0, 0, 0);
                  OEMBTDEVDB_initDevices(gpOEMBTDevDB->pDbRecs);
                  OEMBTDEVDB_loadBTDataBase(gpOEMBTDevDB);
               }
               break;
            }
         } /* end of switch ( cmd type )*/
         break;
      }/* BT_EV_GN_CMD_DONE */

   case BT_EV_RM_BONDED :
      {
         result =  SUCCESS;

         MSG_MED( "IBT DB Bonding successful", 0, 0, 0 );

         bdAddr = bt_ev_msg_ptr->ev_msg.ev_rm_bond.bd_addr;

         bCallApp = TRUE;
         break;
      }
   case BT_EV_RM_BOND_FAILED :
      {
         result =  AEEBT_RESULT_DEV_NOT_BONDED;

         MSG_ERROR( "IBT DB Bonding Failed r=0x%x", \
                    bt_ev_msg_ptr->ev_msg.ev_rm_bondf.reason, 0, 0 );

         bdAddr = bt_ev_msg_ptr->ev_msg.ev_rm_bondf.bd_addr;

         bCallApp = TRUE;

         break;
      }
   case BT_EV_RM_DEVICE_UPDATED:
      {
         OEMBTDB_OnDeviceUpdated(bt_ev_msg_ptr);
         break;
      }

   } /* end of switch */

   if ( ( TRUE == bCallApp ) &&
        ( NULL != gpOEMBTDevDB) && 
        ( NULL != gpOEMBTDevDB->pcbResult ) )
   {

      AEECallback* pcb = gpOEMBTDevDB->pcbResult; 
      if ( FALSE == bCmdDoneSuccess )
      {
         /* Search the list of AEE Callbacks and find the callback 
            corresponding to current BT_EV_GN_CMD_DONE */
         for ( ; pcb != NULL; pcb = pcb->pNext)
         {
            OEMBTDEVDB_BOND_INFO* pBondInfo = 
            (OEMBTDEVDB_BOND_INFO*)(pcb->pmc);

            if ( pBondInfo->bCmdDoneSuccess == FALSE)
            {
               if ( pBondInfo->pResult != NULL )
               {
                  *pBondInfo->pResult = result;
               }
               AEE_ResumeCallback( pcb,pBondInfo->pac);
               break;
            }
         }
      }
      else
      {
         for ( ; pcb != NULL; pcb = pcb->pNext)
         {
            OEMBTDEVDB_BOND_INFO* pBondInfo = 
            (OEMBTDEVDB_BOND_INFO*)(pcb->pmc);

            if ( BT_BD_ADDRS_EQUAL( &pBondInfo->bdAddr, &bdAddr ) )
            {
               if ( pBondInfo->pResult != NULL )
               {
                  *pBondInfo->pResult = result;
               }
               AEE_ResumeCallback( pcb,pBondInfo->pac);
               break;
            }
         }
      }

   }

   rex_leave_crit_sect( &gOEMBTDevDBCritSect );

   return;
}

/*======================================================================= 
=======================================================================*/
static void OEMBTDEVDB_Cancel( AEECallback* pcb )
{
   AEECallback** ppcb;

   rex_enter_crit_sect( &gOEMBTDevDBCritSect );

   ppcb = pcb->pCancelData;
   // Verify that it is me set as the cancel function.
   // Cross-task cancellations need this check.
   if ( pcb->pfnCancel == OEMBTDEVDB_Cancel )
   {
      pcb->pfnCancel = NULL;
      pcb->pCancelData = NULL;

      if ( pcb->pmc != NULL )
      {
         OEM_Free ( pcb->pmc);
         pcb->pmc = NULL;
      }

      for ( ; *ppcb != NULL; ppcb = &(*ppcb)->pNext)
      {
         if ( pcb == *ppcb )
         {
            *ppcb = pcb->pNext;
            pcb->pNext = NULL;
            break;
         }
      }
   }

   rex_leave_crit_sect( &gOEMBTDevDBCritSect );
}

/*======================================================================= 
=======================================================================*/
static OEMBTDEVDB* OEMBTDevDB_GetObj( BTHandle hBT )
{

   OEMBTDEVDB* pOemDb = NULL;
   AEEHandle_From( &gOEMBTDEVDB_HandleList, hBT, (OEMINSTANCE*)&pOemDb );
   return pOemDb;

}

