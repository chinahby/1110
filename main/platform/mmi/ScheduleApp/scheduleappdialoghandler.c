/** ----------------------------------------------------------------------------
 *
 * History:
 *
 * when         who            what and why
 * -----------  -----------    -----------------------------
 * -----------------------------------------------------------------------------
 */

#include "ScheduleApp_priv.h"
#ifdef FEATURE_USES_ZI
#include "OEMZIText.h" 
#else
#include "OEMText.h" 
#endif
#ifdef FEATURE_APP_MUSICPLAYER
#include "MusicPlayer.h"
#endif 
#include "appscommonimages.brh"

#define CLOSE_DIALOG(DlgRet)                         \
        {                                            \
            pme->m_eDlgRet = DlgRet;                 \
            (void) ISHELL_EndDialog(pme->m_pShell);  \
        }
#define BK_COLOR            MAKE_RGB(214, 223, 247)
#define FRAME_COLOR         MAKE_RGB(127, 157, 185)

#ifdef FEATURE_CARRIER_CHINA_VERTU
#define LUNAR_BG_COLOR  (MAKE_RGB(0x40, 0x30, 0x20)) 
#else
#define LUNAR_BG_COLOR  (MAKE_RGB(0x05, 0x0A, 0x12)) 
#endif

#define LUNAR_FONT_COLOR (RGB_WHITE)

static boolean  dialog_handler_of_state_viewmonth( CScheduleApp* pme,
                                                      AEEEvent       eCode,
                                                      uint16         wParam,
                                                      uint32         dwParam);

static boolean  dialog_handler_of_state_pwd(CScheduleApp* pme,
                                                  AEEEvent eCode,
                                                  uint16   wParam,
                                                  uint32   dwParam);

static boolean  dialog_handler_of_state_msgbox(CScheduleApp* pme,
                                                    AEEEvent eCode,
                                                    uint16    wParam,
                                                    uint32    dwParam);

static void Schedule_DialogTimeout(void *pme);

static boolean  dialog_handler_of_state_option( CScheduleApp* pme,
                                                      AEEEvent       eCode,
                                                      uint16         wParam,
                                                      uint32         dwParam);

static boolean  dialog_handler_of_state_viewday( CScheduleApp* pme,
                                                    AEEEvent       eCode,
                                                    uint16         wParam,
                                                    uint32         dwParam);


static boolean  dialog_handler_of_state_gotodate( CScheduleApp* pme,
                                                    AEEEvent    eCode,
                                                    uint16      wParam,
                                                    uint32      dwParam);

static boolean  dialog_handler_of_state_event_edit( CScheduleApp* pme,
                                                      AEEEvent       eCode,
                                                      uint16         wParam,
                                                      uint32         dwParam);

static boolean  dialog_handler_of_state_inputtext( CScheduleApp* pme,
                                             AEEEvent       eCode,
                                             uint16         wParam,
                                             uint32         dwParam);
static boolean  dialog_handler_of_state_setup( CScheduleApp* pme,
                                            AEEEvent       eCode,
                                            uint16         wParam,
                                            uint32         dwParam);

static boolean  dialog_handler_of_state_showalert( CScheduleApp* pme,
                                              AEEEvent       eCode,
                                              uint16         wParam,
                                              uint32         dwParam);

static boolean  dialog_handler_of_state_viewevent( CScheduleApp* pme,
                                              AEEEvent       eCode,
                                              uint16         wParam,
                                              uint32         dwParam);

static void initMenuItemWhenViewDay(CCalApp *pme,IMenuCtl *pIMenuCtl, int type);
#if FEATURE_ONE_DB
static boolean openDatabaseIf(CCalApp *pme);
#else
static boolean openDatabaseIf(CCalApp *pme, uint16 year, uint16 month);
#endif
static boolean readRecordFields(IDBRecord * prec, CalDBRecFields * pf);
static void updateEvent( CCalApp* pme);
static void saveEvent(CCalApp *pme);
static int saveEventToDB(CCalApp* pme, boolean newEvent);
static int updateDBRecord(CCalApp     * pme,
                                   IDBRecord     *pidb,
                                   uint16         permRecId,
                                   const AECHAR  *pszDescription,
                                   const AECHAR  *pszNote,
                                   CalBaseFields *pb);

static void initRecordField(AEEDBField*         pFields,
                            AEEDBFieldType      type,
                            RecordFieldTypeEnum name,
                            void*               pBuff,
                            uint16              nSize
                        );

static void stopRingerAlert(CCalApp *pme);
static void playRingerAlert(CCalApp *pme);

#if defined(FILECFG)
static int      CScheduleApp_ReadCFG(IFileMgr *pFileMgr, CalCfg *pCFGCache);
static int      CScheduleApp_WriteCFG(IFileMgr *pFileMgr, CalCfg *pCFGCache);
static void     CScheduleApp_CFGCacheInit(CalCfg *pCFGCache);
#endif

static void drawModalDialog( IDisplay* pd, IStatic* pStatic, uint16 stringResId, boolean confirm);
static void ScheduleAppNotifyMP3PlayerAlertEvent(CCalApp *pMe, boolean toStartAlert);


#if FEATURE_DRAW_LUNAR_CALENDAR
//内部函数
//============阴历的处理函数============//
//转换uint16 to uint32
static uint32 CScheduleApp_MakeLong(uint16 m_Low,uint16 m_High);

//取高16位
static uint16 CScheduleApp_HiWord(uint32 m_Num);

//取低16位
static uint16 CScheduleApp_LoWord(uint32 m_Num);

//获取对应阴历日期汉字字符串
void CScheduleApp_GetChineseLunar(CCalendarApp * pme);

//判断iYear是不是闰年
static boolean CScheduleApp_IsLeapYear(uint16 iYear);

//返回阴历iLunarYer年阴历iLunarMonth月的天数，如果iLunarMonth为闰月，
//高字为第二个iLunarMonth月的天数，否则高字为0
// 1901年1月---2050年12月
static uint32 CScheduleApp_LunarMonthDays(CCalendarApp * pme, uint16 iLunarYear, uint16 iLunarMonth);

//返回阴历iLunarYear年的总天数
// 1901年1月---2050年12月
static uint16 CScheduleApp_LunarYearDays(CCalendarApp * pme, uint16 iLunarYear);

//返回阴历iLunarYear年的闰月月份，如没有返回0
// 1901年1月---2050年12月
static uint16 CScheduleApp_GetLeapMonth(CCalendarApp * pme, uint16 iLunarYear);

//计算公历两个日期间相差的天数  1年1月1日 --- 65535年12月31日
static uint32 CScheduleApp_CalcDateDiff(CCalendarApp * pme,uint16 iEndYear, uint16 iEndMonth, uint16 iEndDay,
                             uint16 iStartYear, uint16 iStartMonth, uint16 iStartDay);

//计算公历iYear年iMonth月iDay日对应的阴历日期,
//1901年1月1日---2050年12月31日
void CScheduleApp_GetLunarDate(CCalendarApp * pme,uint16 iYear, uint16 iMonth, uint16 iDay);


void CScheduleApp_GetLunarStr(CScheduleApp *pme);

static void CScheduleApp_DrawLunarStr(CScheduleApp *pme);
#endif


extern boolean Calendar_FormatDateTime( uint32 seconds,
                                        AECHAR* resultString,
                                        int resultStringLength
                                    );

extern boolean Calendar_FormatTime( uint32 seconds, AECHAR* resultString, int resultStringLength);
extern int OEM_GetConfig(AEEConfigItem i, void * pBuff, int nSize);
extern int OEM_SetConfig(AEEConfigItem i, void * pBuff, int nSize);

#ifdef FEATURE_SPORTS_APP
Sport_Appointment pSportAppt;
#endif

/*==============================================================================
函数:
       CScheduleApp_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pme [In]: 指向ScheduleApp Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       SUCCESS: 创建对话框成功
       非SUCCESS: ?唇ǘ曰翱蚴О?

备注:


==============================================================================*/
int CScheduleApp_ShowDialog(CScheduleApp *pme, uint16  dlgResId)
{
    int nRet;
    // At most one dialog open at once
    if (ISHELL_GetActiveDialog(pme->m_pShell) != NULL)
    {
        // Looks like there is one dialog already opened.
        // Flag an error an return without doing anything.
#if defined(AEE_STATIC)
        MOVE_TO_STATE( pme->m_ePreState)
#endif
        return EFAILED;
    }

    nRet = ISHELL_CreateDialog(pme->m_pShell,AEE_SCHEDULEAPP_RES_FILE,dlgResId,NULL);

#if defined(AEE_STATIC)
    if (nRet != SUCCESS)
    {
#if 0
        nv_language_enum_type language;
        extern int OEM_GetConfig( AEEConfigItem i, void * pBuff, int nSize);
        OEM_GetConfig( CFGI_LANGUAGE_SELECTION, &language, sizeof(language));

#endif
        MOVE_TO_STATE( pme->m_ePreState)
    }
#endif
    return nRet;
}


/*==============================================================================
函数:
       CScheduleApp_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pme [In]: 指向ScheduleApp Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean CScheduleApp_RouteDialogEvent( CScheduleApp* pme,
                                       AEEEvent   eCode,
                                       uint16     wParam,
                                       uint32     dwParam)
{
    switch (pme->m_pActiveDlgID)
    {
        
        case IDD_PWD:
            return dialog_handler_of_state_pwd(pme, eCode, wParam, dwParam);

        case IDD_MSGBOX:
            return dialog_handler_of_state_msgbox(pme, eCode, wParam, dwParam);

        case IDD_VIEWMONTH:
            return dialog_handler_of_state_viewmonth(pme,eCode,wParam,dwParam);

        case IDD_OPTION:
            return dialog_handler_of_state_option(pme,eCode,wParam,dwParam);

        case IDD_GOTODATE:
            return dialog_handler_of_state_gotodate(pme, eCode, wParam, dwParam);

        case IDD_VIEWDAY:
            return dialog_handler_of_state_viewday(pme,eCode,wParam,dwParam);

        case IDD_EVENT_EDIT:
            return dialog_handler_of_state_event_edit(pme,eCode,wParam,dwParam);

        case IDD_INPUTTEXT:
            return dialog_handler_of_state_inputtext(pme,eCode,wParam,dwParam);

        case IDD_SETUP:
            return dialog_handler_of_state_setup(pme,eCode,wParam,dwParam);

        case IDD_SHOWALERT:
            return dialog_handler_of_state_showalert(pme,eCode,wParam,dwParam);

        case IDD_VIEWEVENT:
            return dialog_handler_of_state_viewevent(pme,eCode,wParam,dwParam);

        default:
            return FALSE;
    }
}


static int32 dateToJday( int nYear, int nMonth, int nDay)
{
  // check for invalid dates
  if (nYear < 1 ||
      nMonth <= 0 || nMonth > 12 ||
      nDay <= 0 || nDay > 31)
    return 0;

  // Adjust the start of the year to be March 1
  if (nMonth > 2)
    nMonth -= 3;
  else {
    nMonth += 9;
    nYear--;
  }
  return ((nYear / 100) * DAYS_PER_400_YEARS) / 4
          + ((nYear % 100) * DAYS_PER_4_YEARS) / 4
          + (nMonth * DAYS_PER_5_MONTHS + 2) / 5
          + nDay
          - JDAY_OFFSET;
}

static boolean jdayToDate(int32 jday, uint16 *pYear, uint16 *pMonth, uint16 *pDay)
{
  long      lTemp;
  int       nCentury;
  int       nYear;
  int       nMonth;
  int       nDay;
  int       nDayOfYear;    // (1 <= nDayOfYear <= 366

  if(jday < 0 || jday >= ((uint32)0xffffffff - (JDAY_OFFSET * 4)) / 4)
      return(FALSE);

   lTemp = (jday + JDAY_OFFSET) * 4 - 1;
   nCentury = lTemp / DAYS_PER_400_YEARS;

  // Calculate the year and day of year
   lTemp = ((lTemp % DAYS_PER_400_YEARS) / 4) * 4 + 3;
   nYear = (nCentury * 100) + (lTemp / DAYS_PER_4_YEARS);

   if(nYear > 9999)
      return(FALSE);

   nDayOfYear = (lTemp % DAYS_PER_4_YEARS) / 4 + 1;

  // Calculate the month and day of month
   lTemp = nDayOfYear * 5 - 3;                   // shift to Mar based years
   nMonth = lTemp / DAYS_PER_5_MONTHS;
   nDay = (lTemp % DAYS_PER_5_MONTHS) / 5 + 1;

  // Convert back to Jan based years
   if (nMonth < 10)
      nMonth += 3;
   else {
      nMonth -= 9;
      nYear++;
   }
   *pYear = nYear;
   *pMonth = nMonth;
   *pDay = nDay;

   return(TRUE);
}

static uint32 secondsToCompositeTime( uint32 seconds)
{
    JulianType jdate = {0};
    uint32     jday  = 0;

    GETJULIANDATE( seconds, &jdate);
    jday = dateToJday( jdate.wYear, jdate.wMonth, jdate.wDay);
    return MAKECOMPTIME( jday, jdate.wHour * 60 + jdate.wMinute);
}

uint32 getNowCompositeTime( void)
{
    return secondsToCompositeTime( GETTIMESECONDS());
}

static void addEventToList( CalEvent** ppHead, CalEvent* pElement)
{

    CalEvent* pHead = *ppHead;
    //debug( ";-------------------");
    //debug( ";addEventToList, pElement.id = %d", pElement->nPermRecID);

    if( pHead == NULL)
    {
        pHead           = pElement;
        pElement->pNext = pHead;
        pElement->pPrev = pHead;

        *ppHead = pHead;
        //debug( ";addEventToList, as head");
    }
    else
    {

        CalEvent* pEvent        = pHead->pPrev;
        CalEvent* pEventLast    = NULL;
        while( pEventLast != pHead && pEvent->m_b.dwTime > pElement->m_b.dwTime)
        {
            pEventLast  = pEvent;
            pEvent      = pEvent->pPrev;
        }

        if( !pEventLast)
        {
            pEventLast = pHead;
        }
        else if( pEventLast == pHead)
        {
            *ppHead = pElement;
        }

        //debug( ";addEventToList, pHead.id = %d, pEventLast.id = %d", pHead->nPermRecID, pEventLast->nPermRecID);
        pElement->pNext             = pEventLast;
        pElement->pPrev             = pEventLast->pPrev;
        pEventLast->pPrev->pNext    = pElement;
        pEventLast->pPrev           = pElement;

    }

//    {
//
//        CalEvent* pEvent = *ppHead;
//        CalEvent* pFirst = pEvent;
//
//        debug( ";to iterate today events");
//        while( pEvent)
//        {
//            debug( ";event.%d, %02d:%02d", pEvent->nPermRecID, GET_HOUR( GETTIME( pEvent->m_b.dwTime)), GET_MINUTE( GETTIME( pEvent->m_b.dwTime)));
//            pEvent = pEvent->pNext;
//            if( pEvent == pFirst)
//            {
//                break;
//            }
//        }
//        debug( ";-------------------");
//    }

}

static void registerEvent( CCalApp* pme, uint16 permId, int32 expireSecondsFrowNow)
{
    //debug( ";registerEvent, ( %d, %d)", permId, expireSecondsFrowNow);

#if !defined( AEE_SIMULATOR)
    IAlarm_CancelAlarm( pme->m_pIAlarm, AEECLSID_SCHEDULEAPP, permId);
    IAlarm_SetAlarm( pme->m_pIAlarm,
                        AEECLSID_SCHEDULEAPP,
                        permId,
                        expireSecondsFrowNow
                    );
#endif
}

static int registerEventIf( CCalApp* pme,
                             CalDBRecFields* pRecordFields,
                             JulianType* pJulianEvent,
                             uint32*     pAlarmCompositeTime,
                             boolean     postpone
)
{
    uint32 jdayEvent    = 0;
    uint32 timeEvent    = 0;
    int    result       = NO_ALARM;
    MEMSET( pJulianEvent, 0, sizeof( JulianType));
    jdayEvent   = GETDATE( pRecordFields->b.dwTime);
    timeEvent   = GETTIME( pRecordFields->b.dwTime);

    //debug( ";---------------");
    //debug( ";registerEventIf");
    if( jdayToDate( jdayEvent, &pJulianEvent->wYear, &pJulianEvent->wMonth, &pJulianEvent->wDay))
    {
        pJulianEvent->wHour   = (uint16)GET_HOUR( timeEvent);
        pJulianEvent->wMinute = (uint16)GET_MINUTE( timeEvent);
        //debug( ";event start: %d.%02d.%02d %02d:%02d", pJulianEvent->wYear, pJulianEvent->wMonth, pJulianEvent->wDay, pJulianEvent->wHour, pJulianEvent->wMinute);
    }

    if( pRecordFields->b.wAlarmTime < 65535)
    {
        uint32 alarmSeconds          = JULIANTOSECONDS( pJulianEvent) - pRecordFields->b.wAlarmTime * 60;
        int32 expireSecondsFrowNow  = (alarmSeconds - GETTIMESECONDS());
        uint32 nowCompositeTime      = getNowCompositeTime();

        *pAlarmCompositeTime  = secondsToCompositeTime( alarmSeconds);
        //debug( ";now = %d, alarm = %d", nowCompositeTime, *pAlarmCompositeTime);

        if( postpone)
        {
            expireSecondsFrowNow = pme->m_cfg.nSnooze * 60;
            if( expireSecondsFrowNow == 0)
            {
                return EXPIRED;
            }
            registerEvent( pme, pRecordFields->nPermRecID, expireSecondsFrowNow);

            //debug( ";event@%d.%02d.%02d, to postpone it", pJulianEvent->wYear, pJulianEvent->wMonth, pJulianEvent->wDay);
            //debug( ";expireSecondsFrowNow=%d", expireSecondsFrowNow);

            result = EXPIRED;
        }
        else if( nowCompositeTime <= *pAlarmCompositeTime && expireSecondsFrowNow >= 0)
        {
            //debug( ";event@%d.%02d.%02d, register it", pJulianEvent->wYear, pJulianEvent->wMonth, pJulianEvent->wDay);
            //debug( ";composite, now=%d, alarm=%d, ", nowCompositeTime, *pAlarmCompositeTime);
            //debug( ";expireSecondsFrowNow=%d", expireSecondsFrowNow);

            registerEvent( pme, pRecordFields->nPermRecID, expireSecondsFrowNow);
            result = NOT_EXPIRED;
        }

    }
    return result;
}

static void releaseMonthEvents( CCalApp* pme)
{

    int         i       = 0;
    CalEvent*   pHead   = 0;
    CalEvent*   pEvent  = 0;
    CalEvent*   pFree   = 0;
    CalEvent*       pEventFirst  = 0;
    for( i = 0; i < 31; i ++)
    {
        pHead = pme->m_pMonthEvents[i];
        pEvent = pHead;
        if( NULL != pEvent)
        {
            pEventFirst = pEvent;
            while( NULL != pEvent) 
            {
                pFree  = pEvent;
                pEvent = pEvent->pNext;
                FREEIF( pFree);
                if(pEvent == pEventFirst)
                {
                    pEvent = NULL; 
                }                    
            }
        }
        //FREEIF( pHead); Remove by Gemsea
        pme->m_pMonthEvents[i] = NULL;
    }

    MEMSET( &pme->m_pMonthEvents, 0, sizeof( pme->m_pMonthEvents));
}

void closeDatabaseIf( CCalApp *pme)
{
    if( pme->m_pIDatabase)
    {
        //debug( ";closeDatabaseIf, close db firstly");
        IDATABASE_Release( pme->m_pIDatabase);
        pme->m_pIDatabase = 0;
    }
}


static boolean openDatabase( CCalApp *pme, char* dbName)
{
    pme->m_pIDatabase = IDBMGR_OpenDatabase( pme->m_pIDBMgr, dbName, 1);
    if (!pme->m_pIDatabase)
    {
        return FALSE;
    }
    //debug( ";openDatabase, open db %s successful", dbName);
    return TRUE;
}

#if FEATURE_ONE_DB
static boolean openDatabaseIf(CCalApp *pme)
{
    if (!pme->m_pIDatabase)
    {
        return openDatabase( pme, CAL_DATABASE_NAME);
    }
    return TRUE;
}
#else
static boolean openDatabaseIf(CCalApp *pme, uint16 year, uint16 month)
{
    if( !pme->m_pIDatabase)
    {
        char dbName[16] = {0};
        SPRINTF( dbName, "%d.%02d.db", year, month);
        return openDatabase( pme, dbName);
    }
    return TRUE;
}
#endif

static boolean retrieveMonthEventsFromDB( CCalApp* pme)
{

    uint16  i       = 0;
    uint16  year    = pme->m_julianCurrentDay.wYear;
    uint16  month   = pme->m_julianCurrentDay.wMonth;

    int             theLastEventDay         = 1;
    int             theLastDayOfThisMonth   = 0;
    uint32          nowCompositeTime        = 0;
    uint32          alarmCompositeTime      = 0;
    uint32          autoDeleteCompositeTime = 0;
    CalEvent*       pEvent                  = 0;
    CalEvent*       pEventPrevious          = 0;
    IDBRecord*      pRecord                 = 0;
    CalDBRecFields  recordFields            = {0};
    JulianType      julianEvent             = {0};
    releaseMonthEvents( pme);
    //debug(";---------------------------------------------------");
    //debug(";retrieveMonthEvents");

#if FEATURE_ONE_DB
    if( !openDatabaseIf( pme))
#else
    if( !openDatabaseIf( pme, year, month))
#endif
    {
        return FALSE;
    }
    //debug(";open db, ok");

    for( theLastDayOfThisMonth = 31; theLastDayOfThisMonth > 0; theLastDayOfThisMonth --)
    {

        uint16 year1   = 0;
        uint16 month1  = 0;
        uint16 day1    = 0;
        int32  theLastJdayOfThisMonth = dateToJday( year, month, theLastDayOfThisMonth);

        if( jdayToDate( theLastJdayOfThisMonth, &year1, &month1, &day1) &&
            year                    == year1    &&
            month                   == month1   &&
            theLastDayOfThisMonth   == day1
        )
        {
            break;
        }
    }

    if( pme->m_cfg.nAutoDelete > 0 && pme->m_cfg.nAutoDelete != 65535)
    {
        autoDeleteCompositeTime = secondsToCompositeTime( GETTIMESECONDS() - pme->m_cfg.nAutoDelete * 24 * 60 * 60);
        //debug( ";auto delete events: %d day before", pme->m_cfg.nAutoDelete);
    }

#if FEATURE_ONE_DB
    pme->m_nNextPermRecID   = 1;
    pme->m_dwTotalEventsNumber    = IDATABASE_GetRecordCount( pme->m_pIDatabase);
#endif

    pme->m_dwThisMonthEventsNumber = 0;
    pme->m_dwMask                  = 0;
    nowCompositeTime               = getNowCompositeTime();

    IDATABASE_Reset( pme->m_pIDatabase);
    for( i = 0;;)
    {

        pRecord = IDATABASE_GetNextRecord( pme->m_pIDatabase);
        //debug(";read record, i = %d, pRecord = 0x%x", i, pRecord);

        if (pRecord == NULL)
        {
            break;
        }

        pEvent          = 0;
        pEventPrevious  = 0;
        MEMSET( &recordFields, 0, sizeof(CalDBRecFields));

        if( !readRecordFields( pRecord, &recordFields))
        {
            IDBRECORD_Release( pRecord);
            //debug(";read fields failed");
            continue;
        }
        //debug(";id,        %d", recordFields.nPermRecID);
        //debug(";alarmtime, %d", recordFields.b.wAlarmTime);
        if( autoDeleteCompositeTime && recordFields.b.dwTime <= autoDeleteCompositeTime)
        {
            //debug( ";-----------------auto deleted!");
            //debug( ";%d, %d", autoDeleteCompositeTime, recordFields.b.dwTime);
            IDBRECORD_Remove( pRecord);
            continue;
        }

        registerEventIf( pme, &recordFields, &julianEvent, &alarmCompositeTime, FALSE);
        IDBRECORD_Release( pRecord);

#if FEATURE_ONE_DB
        if( recordFields.nPermRecID >= pme->m_nNextPermRecID)
        {
            pme->m_nNextPermRecID = recordFields.nPermRecID + 1;
        }
#endif

        if( julianEvent.wYear   != year     ||
            julianEvent.wMonth  != month    ||
            julianEvent.wDay    < 1         ||
            julianEvent.wDay    > theLastDayOfThisMonth)
        {
            //debug( ";record.day=%d, not this month event", julianEvent.wDay);
            continue;
        }

        if( julianEvent.wDay != theLastEventDay)
        {
            if( julianEvent.wDay <= theLastDayOfThisMonth)
            {
                i               = julianEvent.wDay - 1;
                theLastEventDay = julianEvent.wDay;

                //debug( ";this event is %d day, set theLastEventDay to %d", julianEvent.wDay, theLastEventDay);
            }
            else
            {
                //debug( ";event day %d, theLastDayOfThisMonth %d, not this month", julianEvent.wDay, theLastDayOfThisMonth);
                continue;
            }
        }

        if( (pEvent = (CalEvent*)MALLOC( sizeof( CalEvent))) != NULL)
        {

            pme->m_dwMask |= 1 << i;

            MEMSET( pEvent, 0, sizeof( CalEvent));
            MEMCPY( &pEvent->m_b, &recordFields.b, sizeof( CalBaseFields));
            pEvent->nCurrRecID = recordFields.nCurrRecID;
            pEvent->nPermRecID = recordFields.nPermRecID;

            if( nowCompositeTime < alarmCompositeTime)
            {
                pEvent->m_lAlarmDateTime = alarmCompositeTime;
            }

            //debug( ";add event node to event list[%d]", i);
            addEventToList( &pme->m_pMonthEvents[i], pEvent);
            pme->m_dwThisMonthEventsNumber ++;
        }
    } // for


    return TRUE;
}


static void deleteEventFromDB( CCalApp *pme, uint16 recordId)
{

#if FEATURE_ONE_DB
    if( !openDatabaseIf( pme))
#else
    if( !openDatabaseIf( pme, pme->m_julianCurrentDay.wYear, pme->m_julianCurrentDay.wMonth))
#endif
    {
    }
    else
    {
        IDBRecord* pRecord = IDATABASE_GetRecordByID( pme->m_pIDatabase, recordId);
        if( pRecord != NULL)
        {
            CalDBRecFields fields = {0};
            if( readRecordFields( pRecord, &fields) && fields.b.dwTime >= pme->m_lToday)
            {
                //debug( ";deleteEventFromDB, cancel alarm");
                IAlarm_CancelAlarm( pme->m_pIAlarm, AEECLSID_SCHEDULEAPP, fields.nPermRecID);
            }

            IDBRECORD_Remove( pRecord);
            //debug( ";deleteEventFromDB, recordID %d, ok", recordId);
        }
    }
}

static void deleteEventsOfTodayFromDB( CCalApp *pme, uint16 theDay)
{

    CalEvent* pHead  = pme->m_pMonthEvents[theDay - 1];
    CalEvent* pEvent = pHead;
    //debug( ";  ");
    //debug( ";deleteEventsOfTodayFromDB, day[%d]", theDay);
    while( pEvent)
    {
        deleteEventFromDB( pme, pEvent->nCurrRecID);
        pEvent = pEvent->pNext;
        if( pEvent == pHead)
        {
            break;
        }
    }
    //debug( ";  ");
}

static void deleteEventsOfThisMonthFromDB( CCalApp *pme)
{

    int i = 0;
    //debug( ";------------------------------------------------------------------------");
    //debug( ";deleteEventsOfThisMonthFromDB, %d.%02d", pme->m_julianCurrentDay.wYear, pme->m_julianCurrentDay.wMonth);
    for( i = 1; i < 32; i ++)
    {
        deleteEventsOfTodayFromDB( pme, i);
    }
    //debug( ";------------------------------------------------------------------------");
}

void deleteAllEventsFromDB( CCalApp *pme)
{


    //debug( ";------------------------------------------------------------------------");
    //debug( ";deleteAllEventsFromDB");
    closeDatabaseIf( pme);

#if FEATURE_ONE_DB


    if( IFILEMGR_Remove( pme->m_pFileMgr, CAL_DATABASE_NAME) == SUCCESS)
    {
        pme->m_dwTotalEventsNumber = 0;
    }

#else

    if(IFILEMGR_EnumInit( pme->m_pFileMgr, CAL_DATABASE_PATH, FALSE)  == SUCCESS)
    {
        FileInfo info = {0};
        //int i=0;
        
        while(IFILEMGR_EnumNext( pme->m_pFileMgr, &info))
        {
            if(STRENDS( ".db", info.szName))
            {
                if( IFILEMGR_Remove( pme->m_pFileMgr, info.szName) == SUCCESS)
                {
                }
                else
                {
                }
            }
        }
    }
    else
    {
        MSG_FATAL(";deleteAllEventsFromDB, enumInit failed",0,0,0);
    }

    pme->m_cfg.maxPermId    = 1;
    pme->m_nNextPermRecID   = 1;
    CScheduleApp_WriteCFG( pme->m_pFileMgr, &pme->m_cfg);
#endif
    //debug( ";------------------------------------------------------------------------");
}


//---------------------------------
static void drawModalDialog( IDisplay* pd, IStatic* pStatic, uint16 stringResId, boolean confirm)
{
    PromptMsg_Param_type    parm;
    AECHAR                  text[128];
    MEMSET( &parm, 0, sizeof( parm));
    parm.ePMsgType  = MESSAGE_CONFIRM;
    parm.eBBarType  = confirm ? BTBAR_OK_CANCEL : BTBAR_BACK;

    ISHELL_LoadResString( AEE_GetShell(),
                       AEE_SCHEDULEAPP_RES_FILE,
                       stringResId,
                       text,
                       256
                   );
    parm.pwszMsg = text;
    DrawPromptMessage( pd, pStatic, &parm);
}


static void drawImage( CScheduleApp* pme, char *resFile, int16 resId, int x, int y)
{

    IImage* image   = ISHELL_LoadResImage( pme->m_pShell, resFile, resId);

    if( image != NULL)
    {
        IIMAGE_Draw( image, x, y);
        IIMAGE_Release( image);
    }
}


static void loadDataTimerCB( void* pdata)
{
    CScheduleApp* pme = (CScheduleApp*)pdata;
    loadData();
}
static void drawPrompt( CScheduleApp* pme, uint16 stringResId)
{

    PromptMsg_Param_type  Msg_Param={0};
    AECHAR  wstrText[MSGBOX_MAXTEXTLEN] = {0};
    static IStatic *pStatic = NULL;

    if (NULL == pStatic)
    {
        AEERect rect = {0};
        if (AEE_SUCCESS != ISHELL_CreateInstance(pme->m_pShell,AEECLSID_STATIC,(void **)&pStatic))
    
        {
            return;
        }
        ISTATIC_SetRect(pStatic, &rect);
    }


    // 从资源文件取消息内容
    (void)ISHELL_LoadResString(pme->m_pShell,
                    AEE_SCHEDULEAPP_RES_FILE,
                    stringResId,
                    wstrText,
                    sizeof(wstrText));
          
    Msg_Param.ePMsgType = MESSAGE_WAITING;
    Msg_Param.pwszMsg = wstrText;
    Msg_Param.eBBarType = BTBAR_NONE;
    DrawPromptMessage(pme->m_pDisplay, pStatic, &Msg_Param);

    if(pStatic != NULL)
    {
        ISTATIC_Release(pStatic);
        pStatic = NULL;    
    }
    
#if 0
    AECHAR              text[64]    = {0};
    Theme_Param_type    themeParms  = {0};
    IImage*             imageBg     = 0;

    imageBg = ISHELL_LoadResImage( pme->m_pShell,
                        AEE_APPSCOMMONRES_IMAGESFILE,
                        IDB_BACKGROUND
                    );
    if( imageBg)
    {

        int             x    = 0;
        int             y    = 0;
        AEEImageInfo    ii   = {0};

        IIMAGE_GetInfo( imageBg, &ii);

        while( x < pme->m_rc.dx)
        {

            y = 0;
            while( y < pme->m_rc.dy)
            {
                IIMAGE_Draw( imageBg, x, y);
                y += ii.cy;
            }
            x += ii.cx;
        }

        IIMAGE_Release( imageBg);
    }


    Appscom_GetThemeParameters( &themeParms);
    IDISPLAY_SetColor( pme->m_pDisplay, CLR_USER_TEXT, themeParms.textColor);


    ISHELL_LoadResString( pme->m_pShell,
                          AEE_SCHEDULEAPP_RES_FILE,
                          stringResId,
                          text,
                          128
                      );

    IDISPLAY_DrawText( pme->m_pDisplay,
                AEE_FONT_NORMAL,
                text,
                -1,
                0,
                0,
                0,
                IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE | IDF_TEXT_TRANSPARENT
            );
   IDISPLAY_SetColor( pme->m_pDisplay, CLR_USER_TEXT, 0);
#endif 
}

static void drawLoading( CScheduleApp* pme)
{
    drawPrompt( pme, IDS_INITING);
}

/*==============================================================================
函数:
       dialog_handler_of_state_msgbox

说明:
       弹出菜单显示

参数:


返回值:


备注:

==============================================================================*/

static boolean  dialog_handler_of_state_msgbox(CScheduleApp* pme,
                                                    AEEEvent eCode,
                                                    uint16    wParam,
                                                    uint32    dwParam
)
{
    static IStatic * pStatic = NULL;
    if (NULL == pme)
    {
        return FALSE;
    }

    if (NULL == pStatic)
    {
        AEERect rect = {0};
        if (AEE_SUCCESS != ISHELL_CreateInstance(pme->m_pShell,
                                                 AEECLSID_STATIC,
                                                 (void **)&pStatic))
        
        {
            return FALSE;
        }        
        ISTATIC_SetRect(pStatic, &rect);  
    }
   
    switch(eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            ISHELL_SetTimer(pme->m_pShell,1000,Schedule_DialogTimeout,pme);
            (void) ISHELL_PostEventEx(pme->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_SCHEDULEAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);

            return TRUE;

        case EVT_USER_REDRAW:
            {
                PromptMsg_Param_type  Msg_Param={0};
                AECHAR  wstrText[MSGBOX_MAXTEXTLEN];
                
                // 从资源文件取消息内容
                (void)ISHELL_LoadResString(pme->m_pShell,
                                AEE_SCHEDULEAPP_RES_FILE,
                                pme->m_wMsgResID,
                                wstrText,
                                sizeof(wstrText));
                                
                Msg_Param.ePMsgType = MESSAGE_WARNNING;
                Msg_Param.pwszMsg = wstrText;
                DrawPromptMessage(pme->m_pDisplay, pStatic, &Msg_Param);
            }
            // 更新界面
            IDISPLAY_UpdateEx(pme->m_pDisplay, FALSE);
            
            return TRUE;

        case EVT_DIALOG_END:
            ISTATIC_Release(pStatic);
            pStatic = NULL;
            return TRUE;

        case EVT_DISPLAYDIALOGTIMEOUT:
            CLOSE_DIALOG(DLGRET_MSGBOX_OK)
            return TRUE;
            
        case EVT_KEY:
            
            ISHELL_CancelTimer(pme->m_pShell, NULL,  pme);
            CLOSE_DIALOG(DLGRET_MSGBOX_OK)

            return TRUE;
        

        default:
            break;
    }
    return FALSE;
}


/*==============================================================================
函数:
       dialog_handler_of_state_pwd

说明:
       密码检查对话框

参数:

返回值:

备注:

==============================================================================*/

static boolean dialog_handler_of_state_pwd(CScheduleApp* pme,
                                            AEEEvent    eCode,
                                            uint16      wParam,
                                            uint32      dwParam
)
{
    PARAM_NOT_REF(dwParam)
    //static char   *m_strPhonePWD = NULL;
    AECHAR      wstrDisplay[OEMNV_LOCKCODE_MAXLEN+2] = {0};
    int             nLen = 0;
    char        strDisplay[OEMNV_LOCKCODE_MAXLEN+2] = {0};
    
    if (NULL == pme)
    {
        return FALSE;
    }
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            if(NULL == pme->m_strPhonePWD)
            {
                pme->m_strPhonePWD = (char *)MALLOC((OEMNV_LOCKCODE_MAXLEN + 1)* sizeof(char));
            }
            return TRUE;
            
        case EVT_DIALOG_START:  
            (void) ISHELL_PostEvent(pme->m_pShell,
                                    AEECLSID_SCHEDULEAPP,
                                    EVT_USER_REDRAW,
                                    NULL,
                                    NULL);

            return TRUE;
            
        case EVT_USER_REDRAW:
            // 绘制相关信息
            {
                AECHAR  text[32] = {0};
                RGBVAL nOldFontColor;
                TitleBar_Param_type  TitleBar_Param = {0};
                
                // 先清屏
#ifdef FEATURE_CARRIER_CHINA_VERTU
                {
                    IImage *pImageBg = ISHELL_LoadResImage(pme->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SECURITY_BACKGROUND);
                    
                    Appscommon_ResetBackground(pme->m_pDisplay, pImageBg, APPSCOMMON_BG_COLOR, &pme->m_rc, 0, 0);
                    if(pImageBg != NULL)
                    {
                        IImage_Release(pImageBg);
                    }
                }
#else
                Appscommon_ResetBackgroundEx(pme->m_pDisplay, &pme->m_rc, TRUE);
#endif
                //IDISPLAY_FillRect  (pme->m_pDisplay,&pme->m_rc,RGB_BLACK);

				#if defined (FEATURE_VERSION_C337) || defined (FEATURE_VERSION_W317A)
				(void)ISHELL_LoadResString(pme->m_pShell, 
	                                        AEE_SCHEDULEAPP_RES_FILE,
	                                        IDS_CALENDAR, 
	                                        text,
	                                        sizeof(text));    
				#else
               (void)ISHELL_LoadResString(pme->m_pShell, 
	                                        AEE_SCHEDULEAPP_RES_FILE,
	                                        IDS_APP, 
	                                        text,
	                                        sizeof(text));    
				#endif
				
                // 画标题条
                TitleBar_Param.pwszTitle = text;
                TitleBar_Param.dwAlignFlags = IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
                #if 0
                DrawTitleBar(pme->m_pDisplay, &TitleBar_Param);
				#else
				IANNUNCIATOR_SetFieldText(pme->m_pIAnn,text);
				#endif

               (void)ISHELL_LoadResString(pme->m_pShell, 
                                                AEE_SCHEDULEAPP_RES_FILE,
                                                IDS_PWD_TITLE, 
                                                text,
                                                sizeof(text));
                nOldFontColor = IDISPLAY_SetColor(pme->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
                
                IDISPLAY_DrawText(pme->m_pDisplay, 
                                    AEE_FONT_BOLD, 
                                    text,
                                    -1, 
                                    5, 
                                    MENUITEM_HEIGHT*1/2, 
                                    NULL, 
                                    IDF_TEXT_TRANSPARENT);
                   
                nLen = (pme->m_strPhonePWD == NULL)?(0):(STRLEN(pme->m_strPhonePWD));
                MEMSET(strDisplay, '*', nLen);
                strDisplay[nLen] = '|';
                strDisplay[nLen + 1] = '\0';
                (void) STRTOWSTR(strDisplay, wstrDisplay, sizeof(wstrDisplay));
                IDISPLAY_DrawText(pme->m_pDisplay, 
                                AEE_FONT_BOLD, 
                                wstrDisplay,
                                -1, 
                                10, 
                                MENUITEM_HEIGHT*3/2,
                                NULL, 
                                IDF_TEXT_TRANSPARENT);
                (void)IDISPLAY_SetColor(pme->m_pDisplay, CLR_USER_TEXT, nOldFontColor);
        
                // 绘制底条提示
                if (nLen > 3)
                {// 确定-----删除
                	#ifndef FEATURE_ALL_KEY_PAD
                    drawBottomBar(BTBAR_OK_DELETE)
                    #else
                    drawBottomBar(BTBAR_OK_BACK)
                    #endif
                }
                else if(nLen > 0)
                {
                	#ifndef FEATURE_ALL_KEY_PAD
                    drawBottomBar(BTBAR_DELETE)
                    #else
                    drawBottomBar(BTBAR_BACK)
                    #endif
                }
                else
                {// 确定-----取消
                    drawBottomBar(BTBAR_CANCEL)
                }

                // 更新显示
                IDISPLAY_UpdateEx(pme->m_pDisplay, FALSE); 
        
                return TRUE;
            }
            
        case EVT_DIALOG_END:
            if(!pme->m_bSuspended)
            {
                FREEIF(pme->m_strPhonePWD);
            }
            return TRUE;

        case EVT_KEY:
            {
                char  chEnter = 0;
                int   nLen = 0;
                boolean bRedraw = FALSE;
                
                switch (wParam)
                {
                    case AVK_0:
                    case AVK_1:
                    case AVK_2:
                    case AVK_3:
                    case AVK_4:
                    case AVK_5:
                    case AVK_6:
                    case AVK_7:
                    case AVK_8:
                    case AVK_9:
                        chEnter = '0' + (wParam - AVK_0);
                        break;

                    case AVK_STAR:
                        chEnter = '*';
                        break;
 
                    case AVK_POUND:
                        chEnter = '#';
                        break;
                    //Add By zzg 2012_02_27					
					case AVK_DEL:	 
					{
						chEnter = 0;
						break;
					}
					//Add End	
                    case AVK_CLR:
                        chEnter = 0;       
                        #ifndef FEATURE_ALL_KEY_PAD
                        if (pme->m_strPhonePWD == NULL || STRLEN(pme->m_strPhonePWD) == 0)
                        {
                            CLOSE_DIALOG(DLGRET_CANCELED)
                            return TRUE;
                        }
                        #else
                        if(dwParam == 0)
                        {
                        	CLOSE_DIALOG(DLGRET_CANCELED)
	                        return TRUE;
                        }
                        else
                        {
                        	if (pme->m_strPhonePWD == NULL || STRLEN(pme->m_strPhonePWD) == 0)
	                        {
	                            CLOSE_DIALOG(DLGRET_CANCELED)
	                            return TRUE;
	                        }
                        }
                        #endif
                        break;
                        
                    case AVK_SELECT:
                    case AVK_INFO:
                        if (pme->m_strPhonePWD == NULL || STRLEN(pme->m_strPhonePWD) < 4)
                        {
                            return TRUE;
                        }
                        else
                        //end added
                        {
                            uint16 wPWD=0;

                            OEM_GetConfig(CFGI_PHONE_PASSWORD, &wPWD, sizeof(wPWD));
                        
                            if (wPWD == EncodePWDToUint16(pme->m_strPhonePWD))
                            {// 密码符合
                                CLOSE_DIALOG(DLGRET_PASS)
                            }
                            else
                            {// 密码错误
                                CLOSE_DIALOG(DLGRET_FAILD)
                            }
                        }
                        return TRUE;
                        
                    default:
                        return TRUE;
                }
                nLen = (pme->m_strPhonePWD == NULL)?(0):(STRLEN(pme->m_strPhonePWD));
                if (chEnter == 0)
                {// 删除字符
                    if (nLen > 0)
                    {
                        bRedraw = TRUE;
                        pme->m_strPhonePWD[nLen-1] = chEnter;
                    }
                }
                else if (nLen < OEMNV_LOCKCODE_MAXLEN)
                {
                    pme->m_strPhonePWD[nLen] = chEnter;
                    nLen++;
                    pme->m_strPhonePWD[nLen] = 0;
                    bRedraw = TRUE;
                }
                
                if (bRedraw)
                {
                    (void) ISHELL_PostEvent(pme->m_pShell,
                                            AEECLSID_SCHEDULEAPP,
                                            EVT_USER_REDRAW,
                                            NULL,
                                            NULL);
                }
            }
            return TRUE;
            
        default:
            break;
    }
    
    return FALSE;

}
/*==============================================================================
函数:
       Schedule_DialogTimeout

说明:
       定时器回调函数。主要用于自动关闭消息对话框。

参数:
       pme [in]：void *类型指针，暗指CScheduleApp结构指针。

返回值:
       无。

备注:

==============================================================================*/
static void Schedule_DialogTimeout(void *pMe)
{
    CScheduleApp *pme = (CScheduleApp *)pMe;
    if (NULL == pme)
    {
        return;
    }

    (void) ISHELL_PostEvent(pme->m_pShell,
                            AEECLSID_SCHEDULEAPP,
                            EVT_DISPLAYDIALOGTIMEOUT,
                            0,
                            0);
}



static boolean dialog_handler_of_state_viewmonth( CScheduleApp* pme,
                                                  AEEEvent   eCode,
                                                  uint16     wParam,
                                                  uint32     dwParam
)
{
    static IDateCtl  *pDatePick;
    int pnYear;
    int pnMonth;
    int pnDay;
    AEERect rc;
    static boolean inited   = 0;
    // 1 - deleting    2- done
    static int     subState = 0;
    static boolean bRedrawDone = FALSE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
	if (eCode == EVT_PEN_UP)
	{
		int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
		int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
		AEERect bottomBarRect;
		//int ht;
		int nBarH ;
		AEEDeviceInfo devinfo;
		nBarH = GetBottomBarHeight(pme->m_pDisplay);
		
		MEMSET(&devinfo, 0, sizeof(devinfo));
		ISHELL_GetDeviceInfo(pme->m_pShell, &devinfo);
		SETAEERECT(&bottomBarRect, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
		MSG_FATAL("wXPos=====%d,wYPos=========%d",wXPos,wYPos,0);
		
		if( TOUCH_PT_IN_RECT(wXPos, wYPos, bottomBarRect))
		{
			if(wXPos >= bottomBarRect.x + (bottomBarRect.dx/3)*2 && wXPos < bottomBarRect.x + (bottomBarRect.dx/3)*3 )//右
			{						
				boolean rt =  ISHELL_PostEvent(pme->m_pShell,AEECLSID_SCHEDULEAPP,EVT_USER,AVK_CLR,0);
				return rt;
			}
			else if((wXPos >= bottomBarRect.x) && (wXPos < bottomBarRect.x + (bottomBarRect.dx/3)))//左
			{						
				
				boolean rt =  ISHELL_PostEvent(pme->m_pShell,AEECLSID_SCHEDULEAPP,EVT_USER,AVK_SELECT,0);
				return rt;
			}
			else if((wXPos >= bottomBarRect.x +(bottomBarRect.dx/3)) && (wXPos < bottomBarRect.x + (bottomBarRect.dx/3)*2))
			{
				boolean rt =  ISHELL_PostEvent(pme->m_pShell,AEECLSID_SCHEDULEAPP,EVT_USER,AVK_INFO,0);
				return rt;
			}
		}
	}
#endif

    switch (eCode)
    {

        case EVT_DIALOG_INIT:
            IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
            pDatePick = (IDateCtl*)IDIALOG_GetControl( pme->m_pActiveDlg,IDC_DATE_MONTHVIEW);
            subState = 0;
            return TRUE;

        case EVT_DIALOG_END:
            pDatePick = 0;
            bRedrawDone = FALSE;
            ISHELL_CancelTimer( pme->m_pShell, loadDataTimerCB, pme);
            return TRUE;

        case EVT_DIALOG_START:
        {
            int32 startdate, enddate;

            rc = pme->m_rc;
			#ifdef FEATURE_VERSION_X3
			rc.dy -= (HEIGHT_PROMPT_BAR);
			#else
#if FEATURE_DRAW_LUNAR_CALENDAR
            rc.dy -= (HEIGHT_PROMPT_BAR+14);
#else
            rc.dy -= GetBottomBarHeight( pme->m_pDisplay);
#endif
			#endif
            IDATECTL_SetRect(pDatePick,&rc);

            if( IDATECTL_SetDate( pDatePick,1899,1,1))
            {
                 startdate = IDATECTL_GetJulianDay( pDatePick);
            }
            else
            {
                 return FALSE;
            }

            if( IDATECTL_SetDate( pDatePick,2051,12,31))
            {
                 enddate = IDATECTL_GetJulianDay( pDatePick);
            }
            else
            {
                 return FALSE;
            }

            IDATECTL_SetDateRange(pDatePick, startdate, enddate);
            IDATECTL_EnableDateRange( pDatePick,TRUE);
            if(pme->m_ePreState == STATE_GOTODATE && pme->update == TRUE)
            {
                uint32   year, month, day;
                char     strTep[5] = {0};
                
                (void)WSTRTOSTR(pme->wstrYear, strTep, sizeof(strTep));
                year = STRTOUL(strTep, NULL, 10);
                (void)WSTRTOSTR(pme->wstrMonth, strTep, sizeof(strTep));
                month = STRTOUL(strTep, NULL, 10);
                (void)WSTRTOSTR(pme->wstrDay, strTep, sizeof(strTep));
                day = STRTOUL(strTep, NULL, 10);
                IDATECTL_SetDate( pDatePick, year, month, day);
                inited = TRUE;
                
            }
            else
            {
                IDATECTL_SetJulianDay( pDatePick, (int32)pme->m_CalMgr.m_lCurrentDay);
            }
            IDATECTL_EnableCommand( pDatePick, TRUE, AVK_INFO);
            {
                byte bytData;
                uint32 dwDateFormat;

                OEM_GetConfig(CFGI_DATE_FORMAT, &bytData, sizeof(bytData));
                
                switch(bytData)
                {
                    case  OEMNV_DATEFORM_DMY:
                        dwDateFormat = DFMT_DD_MM_YYYY;
                        break;

                    case OEMNV_DATEFORM_MDY :
                        dwDateFormat = DFMT_MM_DD_YYYY;
                        break;

                    default:
                    case OEMNV_DATEFORM_YMD :
                        dwDateFormat = DFMT_YYYY_MM_DD;
                        break;
                }

                IDATECTL_SetProperties( pDatePick,dwDateFormat);
            }
            IDATECTL_SetToday( pDatePick, (int32)pme->m_CalMgr.m_lToday);

            IDATECTL_GetDate( pDatePick, &pnYear, &pnMonth, &pnDay);
            MEMSET( &pme->m_CalMgr.m_julianCurrentDay, 0, sizeof( JulianType));
            pme->m_CalMgr.m_julianCurrentDay.wYear     = pnYear;
            pme->m_CalMgr.m_julianCurrentDay.wMonth    = pnMonth;
            pme->m_CalMgr.m_julianCurrentDay.wDay      = pnDay;

#if FEATURE_DRAW_LUNAR_CALENDAR
            //suspect 在这里应加年份边界的判断，因为阴历是有范围的
            //方法：从datectl里取出日期，然后再判断是否越界
            //(void)IDATECTL_GetDate(pDatePick,&pnYear,&pnMonth,&pnDay);
            CScheduleApp_GetLunarDate(&pme->m_calendar,
                            (uint16)pnYear,(uint16)pnMonth,(uint16)pnDay);

            //CScheduleApp_GetLunarDate(&pme->m_calendar,2004,10,12);
            CScheduleApp_GetChineseLunar(&pme->m_calendar);

            CScheduleApp_GetLunarStr(pme);
#endif

            if( pme->m_subStateViewMonth == SUBSTATE_NONE &&
                pme->m_ePreState         != STATE_OPTION
            )
            {
                inited = 0;
                ISHELL_SetTimer( pme->m_pShell, 500, loadDataTimerCB, pme);
            }
            else
            {
                inited = 1;
            }
            repaint(pme, TRUE);
        }
        {
            AECHAR WTitle[20] = {0};

			#if defined (FEATURE_VERSION_C337) || defined (FEATURE_VERSION_W317A)
			(void)ISHELL_LoadResString(pme->m_pShell,
							            AEE_SCHEDULEAPP_RES_FILE,                                
							            IDS_CALENDAR,
							            WTitle,
							            sizeof(WTitle));
			#else
            (void)ISHELL_LoadResString(pme->m_pShell,
							            AEE_SCHEDULEAPP_RES_FILE,                                
							            IDS_APP,
							            WTitle,
							            sizeof(WTitle));
			#endif
            IANNUNCIATOR_SetFieldText(pme->m_pIAnn,WTitle);
        }   
        return TRUE;

        case EVT_LOAD_DATA:
            if(pDatePick == NULL)
            {
                return TRUE;
            }
#ifndef WIN32//wlh 临时
            retrieveMonthEventsFromDB( &pme->m_CalMgr);
            pme->m_CalMgr.m_pceCurrent = NULL;
#endif//WIN32
            inited = TRUE;
            repaint(pme, TRUE);
            return TRUE;

        case EVT_USER_REDRAW:
            if(pDatePick == NULL)
            {
                return TRUE;
            }
            
            IDATECTL_SetActiveDayMask( pDatePick, pme->m_CalMgr.m_dwMask);
            (void)IDATECTL_Redraw(pDatePick);
            drawBottomBar(BTBAR_OPTION_TODAY_BACK);
		
#if FEATURE_DRAW_LUNAR_CALENDAR
#if 1
           CScheduleApp_DrawLunarStr(pme);
#else
            rc.x = 0;
            rc.y = pme->m_rc.dy - (HEIGHT_PROMPT_BAR + 14);
            rc.dx = pme->m_rc.dx;
            rc.dy = 14;
            IDISPLAY_EraseRect(pme->m_pDisplay, &rc);
            if (WSTRLEN(pme->m_LunarString) > 4)
            {
                // 绘制农历日期
                (void)IDISPLAY_DrawText(pme->m_pDisplay, AEE_FONT_BOLD,
                            pme->m_LunarString, -1, 0, 0, &rc,
                            IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE | IDF_TEXT_TRANSPARENT);
            }
#endif
#endif

            IDATECTL_SetActive( pDatePick, (pme->m_subStateViewMonth == SUBSTATE_NONE) && (subState == 0));

            if( pme->m_subStateViewMonth != SUBSTATE_NONE)
            {

                if( subState == 1)
                {
                    drawPrompt( pme, IDS_DELETING);
                }
                else if(subState == 2)
                {
                    drawPrompt( pme, IDS_DONE);
                }
                else
                {
                    IStatic* pStatic = (IStatic*)IDIALOG_GetControl(pme->m_pActiveDlg, IDC_VIEWMONTH_STATIC_PROMPT);

                    switch( pme->m_subStateViewMonth)
                    {
                        case SUBSTATE_DELETE_TODAY_CONFIRM:
                            drawModalDialog( pme->m_pDisplay, pStatic, IDS_OPTION_DELETE_TODAY_CONFIRM, TRUE);
                            break;
                        case SUBSTATE_DELETE_MONTH_CONFIRM:
                            drawModalDialog( pme->m_pDisplay, pStatic, IDS_OPTION_DELETE_MONTH_CONFIRM, TRUE);
                            break;
                        case SUBSTATE_DELETE_ALL_CONFIRM:
                            drawModalDialog( pme->m_pDisplay, pStatic, IDS_OPTION_DELETE_ALL_CONFIRM, TRUE);
                            break;
                    }
                }
            }

            if( !inited)
            {
                //drawLoading( pme);
            }
            else
            {
                bRedrawDone = TRUE;
            }

            IDISPLAY_UpdateEx( pme->m_pDisplay, FALSE);
            return TRUE;
		case EVT_KEY:
        {
            if( subState == 1)
            {
                return TRUE;
            }

            if( pme->m_subStateViewMonth != SUBSTATE_NONE)
            {
                if( wParam == AVK_CLR || wParam == AVK_SELECT)
                {
                    if( wParam == AVK_SELECT)
                    {

                        subState = 1;
                        repaint(pme, FALSE);

                        switch( pme->m_subStateViewMonth)
                        {
                            case SUBSTATE_DELETE_ALL_CONFIRM:
                                deleteAllEventsFromDB( &pme->m_CalMgr);
                                break;
                            case SUBSTATE_DELETE_TODAY_CONFIRM:
                                deleteEventsOfTodayFromDB( &pme->m_CalMgr, pme->m_CalMgr.m_julianCurrentDay.wDay);
                                break;
                            case SUBSTATE_DELETE_MONTH_CONFIRM:
                                deleteEventsOfThisMonthFromDB( &pme->m_CalMgr);
                                break;
                        }

                        subState = 2;
                        repaint(pme, FALSE);
                        
                        if( pme->m_CalMgr.m_dwThisMonthEventsNumber > 0)
                        {
                            retrieveMonthEventsFromDB( &pme->m_CalMgr);
                        }
                        OEMOS_Sleep( 200);
                        subState = 0;
                    }
                    pme->m_subStateViewMonth = SUBSTATE_NONE;
                    repaint(pme, TRUE);
                }
                return TRUE;
            }

            switch (wParam)
            {
				case AVK_SELECT:
                {
                    int nLeft ,i;
                    int pnChars;
                    AECHAR *pchar;
                    AEEDeviceInfo  dm;
                    
                    if(pDatePick == NULL)
                    {
                        return TRUE;
                    }
                    
                    pchar = pme->m_pVDTitle;
                    nLeft = sizeof(pme->m_pVDTitle);
                    MEMSET(pme->m_pVDTitle,0,sizeof(pme->m_pVDTitle));

                    ISHELL_GetDeviceInfo(pme->m_pShell, &dm);

                    IDATECTL_GetDate( pDatePick, &pnYear, &pnMonth, &pnDay);
                    MEMSET( &pme->m_CalMgr.m_julianCurrentDay, 0, sizeof( JulianType));
                    pme->m_CalMgr.m_julianCurrentDay.wYear     = pnYear;
                    pme->m_CalMgr.m_julianCurrentDay.wMonth    = pnMonth;
                    pme->m_CalMgr.m_julianCurrentDay.wDay      = pnDay;
                    pme->m_CalMgr.m_lCurrentDay = (uint32)IDATECTL_GetJulianDay(pDatePick);
#ifdef FEATURE_APP_WORLDTIME
                    {
                        extern boolean Calendar_FormatDate2( uint16 year, uint16 month, uint16 day, AECHAR* resultString, int resultStringLength);

                        if( Calendar_FormatDate2( pnYear, pnMonth, pnDay, pchar, nLeft))
                        {
                            pnChars = WSTRLEN( pchar);
                        }
                    }
#endif
                    nLeft = nLeft - pnChars;
                    pchar = pchar + pnChars;
                    for( i = 0; i < 2; i ++)
                    {
                        *pchar = (AECHAR)' ';
                        pchar ++;
                        nLeft --;
                    }
                    (void)IDATECTL_GetDayString( pDatePick,  pchar, nLeft,  &pnChars);
                    if( dm.dwLang == LNG_ENGLISH && pnChars > 3)
                    {
                        pchar[3] = (AECHAR)'\0';
                    }

                    pme->m_subStateOption    = SUBSTATE_OPTION_VIEWMONTH;
                    pme->m_stateToBackOption = STATE_VIEWMONTH;
                    CLOSE_DIALOG(DLGRET_OK)
                }
                return TRUE;

                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
				case AVK_INFO:
					(void)IDATECTL_SetJulianDay(pDatePick,(int32)pme->m_CalMgr.m_lToday);
                    repaint(pme, TRUE);
					return TRUE;
            }
        }
        break;

#if FEATURE_DRAW_LUNAR_CALENDAR
        case EVT_KEY_RELEASE:
        case EVT_CTL_CHANGING:
            if(pDatePick == NULL)
            {
                return TRUE;
            }
            
            if(bRedrawDone && pme->m_subStateViewMonth == SUBSTATE_NONE)
            {
                (void)IDATECTL_GetDate(pDatePick,&pnYear,&pnMonth,&pnDay);
                CScheduleApp_GetLunarDate(&pme->m_calendar,
                                (uint16)pnYear,(uint16)pnMonth,(uint16)pnDay);
                CScheduleApp_GetChineseLunar(&pme->m_calendar);

                CScheduleApp_GetLunarStr(pme);
				//#ifndef FEATURE_VERSION_X3
                CScheduleApp_DrawLunarStr(pme);  //画日历
                //#endif
                IDISPLAY_UpdateEx( pme->m_pDisplay, FALSE);
                //repaint(pme, TRUE);
            }
            return TRUE;
#endif

        case EVT_COMMAND:
        {
            if(pDatePick == NULL)
            {
                return TRUE;
            }
            
            switch( wParam)
            {
                case AVK_INFO:
                {
                    //debug( ";---------go to today");
                    (void)IDATECTL_SetJulianDay(pDatePick,(int32)pme->m_CalMgr.m_lToday);
                    repaint(pme, TRUE);
                    return TRUE;
                }

                case 0xffff:
                {
                    //drawLoading( pme);
                    IDISPLAY_UpdateEx( pme->m_pDisplay, FALSE);
                    OEMOS_Sleep( 100);

                    pme->m_CalMgr.m_lCurrentDay = (uint32)IDATECTL_GetJulianDay(pDatePick);

                    IDATECTL_GetDate( pDatePick, &pnYear, &pnMonth, &pnDay);
                    MEMSET( &pme->m_CalMgr.m_julianCurrentDay, 0, sizeof( JulianType));
                    pme->m_CalMgr.m_julianCurrentDay.wYear     = pnYear;
                    pme->m_CalMgr.m_julianCurrentDay.wMonth    = pnMonth;
                    pme->m_CalMgr.m_julianCurrentDay.wDay      = pnDay;
                    //debug( ";---------month changed to %d.%02d", pnYear, pnMonth, pnDay);

                    closeDatabaseIf( &pme->m_CalMgr);
                    retrieveMonthEventsFromDB( &pme->m_CalMgr);
                    repaint(pme, TRUE);
                    return TRUE;
                }

                case 0xfffe:
                {
                    pme->m_CalMgr.m_lCurrentDay = (uint32)IDATECTL_GetJulianDay(pDatePick);

                    IDATECTL_GetDate( pDatePick, &pnYear, &pnMonth, &pnDay);
                    MEMSET( &pme->m_CalMgr.m_julianCurrentDay, 0, sizeof( JulianType));
                    pme->m_CalMgr.m_julianCurrentDay.wYear     = pnYear;
                    pme->m_CalMgr.m_julianCurrentDay.wMonth    = pnMonth;
                    pme->m_CalMgr.m_julianCurrentDay.wDay      = pnDay;
                }
            }
        }
        break;
    }
    return FALSE;
} // CScheduleApp_HandleViewMonthDlgEvent


static void initDataForEditEventAtOptionState( CScheduleApp* pme, boolean newEvent)
{

    if( newEvent)
    {
        pme->m_CalMgr.m_szEventDes[0]   = 0;
        pme->m_CalMgr.m_szNote[0]       = 0;

        pme->m_CalMgr.m_viewDaySelectedEventID  = pme->m_CalMgr.m_nNextPermRecID;
        pme->m_subStateEventEdit                = SUBSTATE_EVENT_EDIT_NEW;
    }
    else
    {
#if FEATURE_ONE_DB
        if( !openDatabaseIf( &pme->m_CalMgr))
#else
        if( !openDatabaseIf( &pme->m_CalMgr, pme->m_CalMgr.m_julianCurrentDay.wYear, pme->m_CalMgr.m_julianCurrentDay.wMonth))
#endif
        {
            MSG_FATAL(";open db failed",0,0,0);
        }
        else if( pme->m_CalMgr.m_pceCurrent)
        {
            CalDBRecFields recordFields = {0};
            IDBRecord*     pRecord      = IDATABASE_GetRecordByID( pme->m_CalMgr.m_pIDatabase, pme->m_CalMgr.m_pceCurrent->nCurrRecID);

            if( pRecord)
            {
                recordFields.pszDesc  = pme->m_CalMgr.m_szEventDes;
                recordFields.nDescLen = sizeof( pme->m_CalMgr.m_szEventDes) / sizeof(AECHAR);
                recordFields.pszNote  = pme->m_CalMgr.m_szNote;
                recordFields.nNoteLen = sizeof( pme->m_CalMgr.m_szNote) / sizeof(AECHAR);

                readRecordFields( pRecord, &recordFields);
                IDBRECORD_Release( pRecord);
            }
        }

        pme->m_subStateEventEdit = SUBSTATE_EVENT_EDIT_EDIT;
    }
}

/*==============================================================================
函数:
       dialog_handler_of_state_option
说明:
       日程表弹出菜单对话框

参数:
       pme [In]: 指向ScheduleApp Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:
      主界面与view事件界面共用
==============================================================================*/

static boolean  dialog_handler_of_state_option( CScheduleApp* pme,
                                              AEEEvent       eCode,
                                              uint16         wParam,
                                              uint32         dwParam
)
{

    static IMenuCtl* pMenu;

    // 1 - saving 2 - dberror
    static int subState = 0;

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            pMenu = (IMenuCtl*)IDIALOG_GetControl( pme->m_pActiveDlg, IDC_MENU_OPTION);
            subState = 0;
            return TRUE;

        case EVT_DIALOG_START:
        if( pme->m_subStateOption == SUBSTATE_OPTION_VIEWMONTH ||
            pme->m_CalMgr.m_pceCurrent != NULL
        )
        {
            IMENUCTL_AddItem( pMenu,
                    AEE_SCHEDULEAPP_RES_FILE,
                    IDS_OPTION_VIEWMONTH_VIEW_EVENT,
                    IDS_OPTION_VIEWMONTH_VIEW_EVENT,
                    0,
                    0
                );
        }

        if( pme->m_CalMgr.m_pceCurrent != NULL)
        {
            IMENUCTL_AddItem( pMenu,
                    AEE_SCHEDULEAPP_RES_FILE,
                    IDS_OPTION_VIEWDAY_EDIT_EVENT,
                    IDS_OPTION_VIEWDAY_EDIT_EVENT,
                    0,
                    0
                );
        }

        if( pme->m_subStateOption == SUBSTATE_OPTION_VIEWDAY && pme->m_CalMgr.m_pceCurrent == NULL)
        {
            IMENUCTL_AddItem( pMenu,
                AEE_SCHEDULEAPP_RES_FILE,
                IDS_OPTION_NEW_EVENT,
                IDS_OPTION_NEW_EVENT,
                0,
                0
            );
        }

        if( pme->m_subStateOption == SUBSTATE_OPTION_VIEWMONTH)
        {
            IMENUCTL_AddItem( pMenu,
                    AEE_SCHEDULEAPP_RES_FILE,
                    IDS_OPTION_GOTODATE,
                    IDS_OPTION_GOTODATE,
                    0,
                    0
                );
        }

        if( pme->m_CalMgr.m_pceCurrent != NULL)
        {

            IMENUCTL_AddItem( pMenu,
                    AEE_SCHEDULEAPP_RES_FILE,
                    IDS_OPTION_VIEWDAY_DELETE,
                    IDS_OPTION_VIEWDAY_DELETE,
                    0,
                    0
                );
        }

        if( ( pme->m_CalMgr.m_dwMask & (1 << (pme->m_CalMgr.m_julianCurrentDay.wDay - 1)))
                                 && pme->m_subStateOption == SUBSTATE_OPTION_VIEWMONTH)
        {
#if defined(FEATURE_VERSION_C337)  
            IMENUCTL_AddItem( pMenu,
                        AEE_SCHEDULEAPP_RES_FILE,
                        IDS_DELETE,
                        IDS_DELETE,
                        0,
                        0
                    );
#else
            IMENUCTL_AddItem( pMenu,
                        AEE_SCHEDULEAPP_RES_FILE,
                        IDS_OPTION_DELETE_TODAY,
                        IDS_OPTION_DELETE_TODAY,
                        0,
                        0
                    );
#endif
        }
#if 0
        if( ( pme->m_CalMgr.m_dwMask & (~(1 << (pme->m_CalMgr.m_julianCurrentDay.wDay - 1)))))
        {
            IMENUCTL_AddItem( pMenu,
                    AEE_SCHEDULEAPP_RES_FILE,
                    IDS_OPTION_DELETE_MONTH,
                    IDS_OPTION_DELETE_MONTH,
                    0,
                    0
                );
        }
#endif //   去掉了 delete month 菜单

#if FEATURE_ONE_DB
        if( pme->m_CalMgr.m_dwTotalEventsNumber > pme->m_CalMgr.m_dwThisMonthEventsNumber)

#else
        if( (( pme->m_CalMgr.m_dwMask & (1 << (pme->m_CalMgr.m_julianCurrentDay.wDay - 1)))
            &&(pme->m_subStateOption == SUBSTATE_OPTION_VIEWMONTH))
                      || ( pme->m_CalMgr.m_pceCurrent != NULL) )
#endif
        {
            IMENUCTL_AddItem( pMenu,
                    AEE_SCHEDULEAPP_RES_FILE,
                    IDS_OPTION_DELETE_ALL,
                    IDS_OPTION_DELETE_ALL,
                    0,
                    0
                );
        }
        if( pme->m_subStateOption == SUBSTATE_OPTION_VIEWMONTH && !pme->m_sports)
        {
            IMENUCTL_AddItem( pMenu,
                    AEE_SCHEDULEAPP_RES_FILE,
                    IDS_OPTION_SETUP,
                    IDS_OPTION_SETUP,
                    0,
                    0
                );
        }

#if FEATURE_TEST_HOOK
        IMENUCTL_AddItem( pMenu,
                0,
                0,
                9876,
                L"Create test DB",
                0
            );
#endif

        IMENUCTL_SetPopMenuRect( pMenu);
        IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
        IMENUCTL_SetBottomBarType( pMenu, BTBAR_SELECT_BACK);

        repaint(pme, TRUE);
        return TRUE;

        case EVT_DIALOG_END:
            pMenu = 0;
            return TRUE;

        case EVT_USER_REDRAW:
        {
            IMENUCTL_Redraw( pMenu);

#if FEATURE_TEST_HOOK
            IMENUCTL_SetActive( pMenu, subState == 0);

            if( subState == 1)
            {
                drawPrompt( pme, IDS_SAVING);
            }
            else if( subState == 2)
            {

                uint16 resId = IDS_DB_ERROR;
                switch( pme->m_CalMgr.m_dbOperationResult)
                {
                    case -2000:
                    case ENOMEMORY:
                        resId = IDS_DB_ERROR_NOMEMORY;
                        break;
                }
                drawPrompt( pme, resId);
            }

            if( subState > 0)
            {
                IDISPLAY_UpdateEx( pme->m_pDisplay, FALSE);
            }
#endif
        }
        return TRUE;

        case EVT_KEY:
        {
#if FEATURE_TEST_HOOK
            if( subState == 1)
            {
                return TRUE;
            }
            else if( subState == 2)
            {
                if( wParam == AVK_CLR)
                {
                    subState = 0;
                    repaint(pme, TRUE);
                }
                return TRUE;
            }
#endif

            if( wParam == AVK_CLR)
            {
                DLGRetValue backTo[] = { DLGRET_OPTION_BACK_TO_VIEWMONTH,
                                         DLGRET_OPTION_BACK_TO_VIEWDAY
                                       };
                CLOSE_DIALOG( backTo[pme->m_subStateOption - SUBSTATE_OPTION_VIEWMONTH])
            }

#if defined( AEE_SIMULATOR)
            else if( wParam == AVK_SOFT1)
            {
                ISHELL_HandleEvent( pme->m_pShell, EVT_COMMAND, IMENUCTL_GetSel( pMenu), 0);
            }
#endif
        }
        return TRUE;

        case EVT_COMMAND:
        {
            FsmSubState* theSubState = 0;
            FSMState     stateToBack = 0;
            if( pme->m_subStateOption == SUBSTATE_OPTION_VIEWMONTH)
            {
                stateToBack = STATE_VIEWMONTH;
                theSubState        = &pme->m_subStateViewMonth;
            }
            else
            {
                stateToBack = STATE_VIEWDAY;
                theSubState        = &pme->m_subStateViewDay;
            }

            switch( wParam)
            {
                case IDS_OPTION_NEW_EVENT:
                case IDS_OPTION_VIEWDAY_EDIT_EVENT:
                case IDS_SPORTS_CREATE:
                {
                    pme->m_stateToBackEventEdit = stateToBack;
                    initDataForEditEventAtOptionState( pme, wParam == IDS_OPTION_NEW_EVENT || wParam == IDS_SPORTS_CREATE);
                    CLOSE_DIALOG( DLGRET_OPTION_NEW_EVENT)
                }
                break;

                case IDS_SPORTS_JUMP:
                case IDS_OPTION_GOTODATE:
                {
                    CLOSE_DIALOG(DLGRET_GOTODATE)
                }
                break;

                case IDS_SPORTS_VIEW:
                {
                    CLOSE_DIALOG(DLGRET_OPTION_VIEWMONTH_VIEW)
                }
                break;

                case IDS_OPTION_VIEWMONTH_VIEW_EVENT:
                {
                    if( pme->m_subStateOption == SUBSTATE_OPTION_VIEWMONTH)
                    {
                        pme->m_CalMgr.m_viewDaySelectedEventID = 0;
                        CLOSE_DIALOG( DLGRET_OPTION_VIEWMONTH_VIEW)
                    }
                    else if( pme->m_CalMgr.m_pceCurrent)
                    {
                        Appointment* pappointment = (Appointment*)MALLOC( sizeof(Appointment));
                        if( pappointment)
                        {
#if FEATURE_ONE_DB
                            if( !openDatabaseIf( &pme->m_CalMgr))
#else
                            if( !openDatabaseIf( &pme->m_CalMgr, pme->m_CalMgr.m_julianCurrentDay.wYear, pme->m_CalMgr.m_julianCurrentDay.wMonth))
#endif
                            {
                                return FALSE;
                            }
                            else
                            {
                                IDBRecord* precord = IDATABASE_GetRecordByID( pme->m_CalMgr.m_pIDatabase, pme->m_CalMgr.m_pceCurrent->nCurrRecID);
                                if( precord)
                                {
                                    CalDBRecFields fields = {0};

                                    fields.pszDesc  = pappointment->subject;
                                    fields.nDescLen = sizeof(pappointment->subject) / sizeof(AECHAR);
                                    fields.pszNote  = pappointment->location;
                                    fields.nNoteLen = sizeof(pappointment->location) / sizeof(AECHAR);

                                    readRecordFields( precord, &fields);
                                    IDBRECORD_Release(precord);

                                    pappointment->dbId          = fields.nCurrRecID;
                                    pappointment->permId        = fields.nPermRecID;
                                    pappointment->baseFields    = fields.b;
                                    pappointment->julian        = pme->m_CalMgr.m_julianCurrentDay;

                                    //debug( ";-------------------------");
                                    //debug( ";to view event");
                                    //debug( ";dbId = %d", pappointment->dbId);
                                    //debug( ";permId = %d", pappointment->permId);
                                    //debug( ";subject = %S", pappointment->subject);
                                    //debug( ";location = %S", pappointment->location);
                                    //debug( ";date = %d.%02d.%02d %02d:%02d", pappointment->julian.wYear, pappointment->julian.wMonth, pappointment->julian.wDay, pappointment->julian.wHour, pappointment->julian.wSecond);

                                    pme->m_CalMgr.m_appointmentToView = pappointment;
                                    pme->m_stateToBackViewEvent = STATE_VIEWDAY;
                                    CLOSE_DIALOG( DLGRET_OPTION_VIEWDAY_TO_VIEWEVENT)
                                }
                            }
                        }
                    }
                }
                break;

                case IDS_OPTION_VIEWDAY_DELETE:
                {
                    pme->m_subStateViewDay = SUBSTATE_VIEWDAY_DELETE_CONFIRM;
                    CLOSE_DIALOG( DLGRET_OPTION_VIEWDAY_DELETE)
                }
                break;
				
#if defined(FEATURE_VERSION_C337) 
                case IDS_DELETE: 		
#else
                case IDS_OPTION_DELETE_TODAY:
#endif					
                {
                    *theSubState = SUBSTATE_DELETE_TODAY_CONFIRM;
                    CLOSE_DIALOG( DLGRET_OPTION_DELETE_TODAY)
                }
                break;

                case IDS_OPTION_DELETE_MONTH:
                {
                    *theSubState = SUBSTATE_DELETE_MONTH_CONFIRM;
                    CLOSE_DIALOG( DLGRET_OPTION_DELETE_MONTH)
                }
                break;

                case IDS_OPTION_DELETE_ALL:
                {
                    *theSubState = SUBSTATE_DELETE_ALL_CONFIRM;
                    CLOSE_DIALOG( DLGRET_OPTION_DELETE_ALL)
                }
                break;

                case IDS_OPTION_SETUP:
                {
                    pme->m_stateToBackSetup = stateToBack;
                    CLOSE_DIALOG( DLGRET_OPTION_SETUP)
                }
                break;


#if FEATURE_TEST_HOOK
                case 9876: // create test DB
                {
                    CCalApp*    pcal    = &pme->m_CalMgr;
                    uint16      year    = 0;
                    uint16      month   = 0;
                    uint16      day     = 0;

                    subState = 1;
                    repaint(pme, FALSE);

                    deleteAllEventsFromDB( &pme->m_CalMgr);

                    WSTRCPY( pcal->m_szEventDes, L"test");
                    WSTRCPY( pcal->m_szNote, L"test");
                    pcal->m_nNextPermRecID  = 1;
                    pcal->m_lDuration       = 60;
                    pcal->m_lStartTime      = 8 * 60;
                    pcal->m_nAlarmTime      = 0;
                    pcal->m_AlertState      = ALERTSET;
                    pcal->m_lEventDay       = dateToJday( 2007, 1, 1);

                    while( TRUE)
                    {

                        jdayToDate( pcal->m_lEventDay, &year, &month, &day);
                        //debug( ";create test record %d @%d%d.%02d.%02d", pcal->m_nNextPermRecID, year, month, day);
#if !FEATURE_ONE_DB
                        closeDatabaseIf( pcal);
                        openDatabaseIf( pcal, year, month);
#endif

                        pcal->m_dbOperationResult = saveEventToDB( pcal, TRUE);
                        if( pcal->m_dbOperationResult != SUCCESS)
                        {
                            subState = 2;
                            break;
                        }

                        pcal->m_lEventDay       ++;
                        pcal->m_nNextPermRecID  ++;

                        if( pcal->m_nNextPermRecID > 400)
                        {
                            subState = 0;
                            break;
                        }
                    }
                    repaint(pme, TRUE);
                }
                break;
#endif
            } // switch( wparam)
        } //evt_command
        return TRUE;
		
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
		{
			AEEDeviceInfo devinfo;
			int nBarH ;
			AEERect rc;
			int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
			int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
			nBarH = GetBottomBarHeight(pme->m_pDisplay);
			MEMSET(&devinfo, 0, sizeof(devinfo));
			ISHELL_GetDeviceInfo(pme->m_pShell, &devinfo);
			SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, devinfo.cyScreen);
			if ((wXPos>0)&&(wXPos<devinfo.cxScreen/3)&&(wYPos>rc.y)&&(wYPos<devinfo.cyScreen))
			{
				uint16 nEvtCommand	= IMENUCTL_GetSel(pMenu);
				boolean rt =  ISHELL_PostEvent(pme->m_pShell,AEECLSID_SCHEDULEAPP,EVT_USER,AVK_SELECT,nEvtCommand);
				return rt;
			}
			else if ((wXPos>devinfo.cxScreen/3)&&(wXPos<(devinfo.cxScreen/3)*2)&&(wYPos>rc.y)&&(wYPos<devinfo.cyScreen))
			{
				boolean rt =  ISHELL_PostEvent(pme->m_pShell,AEECLSID_SCHEDULEAPP,EVT_USER,AVK_INFO,0);
				return rt;
			}
			else if ((wXPos>(devinfo.cxScreen/3)*2)&&(wXPos<devinfo.cxScreen)&&(wYPos>rc.y)&&(wYPos<devinfo.cyScreen))
			{
				boolean rt =  ISHELL_PostEvent(pme->m_pShell,AEECLSID_SCHEDULEAPP,EVT_USER,AVK_CLR,0);
				return rt;
			}
		}
		return TRUE;
#endif

    }

    return FALSE;
}


/*==============================================================================
函数:
       dialog_handler_of_state_gotodate

说明:
       输入 go to date 的日期 对话框

参数:


返回值:


备注:

==============================================================================*/
static boolean  dialog_handler_of_state_gotodate( CScheduleApp* pme,
                                                    AEEEvent    eCode,
                                                    uint16      wParam,
                                                    uint32      dwParam
)
{
    switch(eCode)
    {
        case EVT_DIALOG_INIT:
            
            return TRUE;

        case EVT_DIALOG_START:
            pme->curSel = DATE_SEL_YEAR;
            pme->update = FALSE;
            (void) ISHELL_PostEvent(pme->m_pShell,
                                    AEECLSID_SCHEDULEAPP,
                                    EVT_USER_REDRAW,
                                    NULL,
                                    NULL);

        case EVT_USER_REDRAW:
        {
            AEEDeviceInfo di;
            AEERect rc;
            uint16  FontHeight;
            RGBVAL nOldFontColor;

            FontHeight = IDISPLAY_GetFontMetrics(pme->m_pDisplay,
                                                      AEE_FONT_NORMAL,
                                                      NULL,
                                                      NULL) + 4;
            ISHELL_GetDeviceInfo(pme->m_pShell, &di);
            if(!dwParam)
            {
                //draw title 
                {
                    AECHAR text[16];
                    TitleBar_Param_type TitleBar = {0};

					#if defined (FEATURE_VERSION_C337) ||defined (FEATURE_VERSION_W317A)
					ISHELL_LoadResString(pme->m_pShell, 
                                            AEE_SCHEDULEAPP_RES_FILE, 
                                            IDS_CALENDAR, 
                                            text, sizeof(text));
					#else
                    ISHELL_LoadResString(pme->m_pShell, 
                                            AEE_SCHEDULEAPP_RES_FILE, 
                                            IDS_APP, 
                                            text, sizeof(text));
					#endif
					
                    TitleBar.pwszTitle = text;
                    TitleBar.dwAlignFlags = IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
					#if 0
                    DrawTitleBar(pme->m_pDisplay, &TitleBar);
					#else
					IANNUNCIATOR_SetFieldText(pme->m_pIAnn,text);
					#endif

                }

                //draw bottombar
                {
                    BottomBar_Param_type BottomBar = {0};

                    BottomBar.eBBarType = BTBAR_OK_BACK;
                    DrawBottomBar(pme->m_pDisplay, &BottomBar);

                }
                //current time
                {
                    JulianType jtdate;
                    AECHAR  wstrFMT[10]={0};
                    
                    GETJULIANDATE(GETTIMESECONDS(), &jtdate);
                    (void)STRTOWSTR("%d", wstrFMT, sizeof(wstrFMT));
                    WSPRINTF(pme->wstrYear, sizeof(pme->wstrYear), wstrFMT, jtdate.wYear);
                    WSPRINTF(pme->wstrMonth, sizeof(pme->wstrMonth), wstrFMT, jtdate.wMonth);
                    WSPRINTF(pme->wstrDay, sizeof(pme->wstrDay), wstrFMT, jtdate.wDay);

                }
            }

            else
            {
                int         nMaxDays,nLen;
                JulianType  jtdate;
                uint32      nRet = 0;
                char        strTep[5] = {0};

                GETJULIANDATE(GETTIMESECONDS(), &jtdate);

                nLen = WSTRLEN(pme->wstrYear);
                (void)WSTRTOSTR(pme->wstrYear, strTep, sizeof(strTep));
                nRet = STRTOUL(strTep, NULL, 10);
                if (nLen == 4 && (nRet>2050 || nRet<1980))
                {
                    MEMSET(pme->wstrYear, 0, sizeof(pme->wstrYear));
                    SPRINTF(strTep, "%d", jtdate.wYear);
                    (void)STRTOWSTR(strTep, pme->wstrYear, sizeof(pme->wstrYear));
                }
                else
                {
                    jtdate.wYear = nRet;
                }

                (void)WSTRTOSTR(pme->wstrMonth, strTep, sizeof(strTep));
                nRet = STRTOUL(strTep, NULL, 10);
                {
                    if (nRet == 2)
                    {
                        nMaxDays =  (((((jtdate.wYear)%4)==0) && (((jtdate.wYear)%100) || (((jtdate.wYear)%400)==0))) ? 29 : 28);
                    }
                    else
                    {
                        nRet = (nRet > 2) ? nRet-3 : nRet+9;
                        nMaxDays = ((nRet % 5) & 1) ? 30 : 31;
                    }
                
                }

                (void)WSTRTOSTR(pme->wstrDay, strTep, sizeof(strTep));
                nRet = STRTOUL(strTep, NULL, 10);

                if (nRet>nMaxDays)
                {
                    MEMSET(pme->wstrDay, 0, sizeof(pme->wstrDay));
                    SPRINTF(strTep, "%d", nMaxDays);
                    (void)STRTOWSTR(strTep, pme->wstrDay, sizeof(pme->wstrDay));
                }
            }            

            //draw text
            {
                AECHAR  dwsz[16];
                
                SETAEERECT(&rc, 0, TITLEBAR_HEIGHT, di.cxScreen, TITLEBAR_HEIGHT);
                ISHELL_LoadResString(pme->m_pShell, 
                                        AEE_SCHEDULEAPP_RES_FILE, 
                                        IDS_DATE, 
                                        dwsz, sizeof(dwsz));
                 nOldFontColor = IDISPLAY_SetColor(pme->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
                IDISPLAY_DrawText(pme->m_pDisplay, 
                                    AEE_FONT_NORMAL, 
                                    dwsz, 
                                    -1, 
                                    rc.x, 
                                    rc.y, 
                                    &rc, 
                                    IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE | IDF_TEXT_TRANSPARENT);
                (void)IDISPLAY_SetColor(pme->m_pDisplay, CLR_USER_TEXT, nOldFontColor);
            }

            //set date rect
            {            
                uint16 x = (di.cxScreen - FontHeight * 6)/2;
                uint16 y = TITLEBAR_HEIGHT * 4;
                RGBVAL  clrFrame = FRAME_COLOR; 
                RGBVAL  clrFill = RGB_WHITE;
                RGBVAL  clrNosel = BK_COLOR;
                uint16 dateFormatType = 0;
				OEM_GetConfig( CFGI_DATE_FORMAT, &dateFormatType, sizeof( byte));
				switch(dateFormatType)
				{
					case OEMNV_DATEFORM_DMY:
						{
	                	//SETAEERECT(&rc_year, x, y, FontHeight*2, FontHeight);
	                	//SETAEERECT(&rc_month, x + FontHeight * 3, y, FontHeight, FontHeight);
	                	//SETAEERECT(&rc_day, x + FontHeight * 5, y, FontHeight, FontHeight);
	                	SETAEERECT(&pme->rc_day, x, y, FontHeight, FontHeight);
	                	SETAEERECT(&pme->rc_month, x + FontHeight * 2, y, FontHeight, FontHeight);
	                	SETAEERECT(&pme->rc_year, x + FontHeight * 4, y, FontHeight*2, FontHeight);    
	                	}
	                	break;
	                case OEMNV_DATEFORM_MDY:
						{
	                	//SETAEERECT(&rc_year, x, y, FontHeight*2, FontHeight);
	                	//SETAEERECT(&rc_month, x + FontHeight * 3, y, FontHeight, FontHeight);
	                	//SETAEERECT(&rc_day, x + FontHeight * 5, y, FontHeight, FontHeight);
	                	SETAEERECT(&pme->rc_month, x, y, FontHeight, FontHeight);
	                	SETAEERECT(&pme->rc_day, x + FontHeight * 2, y, FontHeight, FontHeight);
	                	SETAEERECT(&pme->rc_year, x + FontHeight * 4, y, FontHeight*2, FontHeight);    
	                	}
	                	break;
	                case OEMNV_DATEFORM_YMD:
						{
	                	//SETAEERECT(&rc_year, x, y, FontHeight*2, FontHeight);
	                	//SETAEERECT(&rc_month, x + FontHeight * 3, y, FontHeight, FontHeight);
	                	//SETAEERECT(&rc_day, x + FontHeight * 5, y, FontHeight, FontHeight);
	                	SETAEERECT(&pme->rc_year, x, y, FontHeight*2, FontHeight);
	                	SETAEERECT(&pme->rc_month, x + FontHeight * 3, y, FontHeight, FontHeight);
	                	SETAEERECT(&pme->rc_day, x + FontHeight * 5, y, FontHeight, FontHeight);    
	                	}
	                	break;
	                default:
	                    break;
                }

                //year
                if (pme->curSel == DATE_SEL_YEAR)
                {
                    clrFrame = FRAME_COLOR; 
                    clrFill =  clrNosel;
                }
                else
                {
                    clrFrame = clrNosel; 
                    clrFill =  RGB_WHITE;
                }                
                IDISPLAY_DrawRect(pme->m_pDisplay, &pme->rc_year, clrFrame, clrFill, IDF_RECT_FRAME | IDF_RECT_FILL);
                IDISPLAY_DrawText(pme->m_pDisplay, 
                                    AEE_FONT_NORMAL, 
                                    pme->wstrYear, 
                                    -1, 
                                    pme->rc_year.x, 
                                    pme->rc_year.y, 
                                    &pme->rc_year, 
                                    IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE | IDF_TEXT_TRANSPARENT);

                //month
                if(pme->curSel == DATE_SEL_MONTH)
                {
                    clrFrame = FRAME_COLOR;
                    clrFill  = clrNosel;
                }
                else
                {
                    clrFrame = clrNosel;
                    clrFill  = RGB_WHITE;
                }
                IDISPLAY_DrawRect(pme->m_pDisplay, &pme->rc_month, clrFrame, clrFill, IDF_RECT_FRAME | IDF_RECT_FILL);
                IDISPLAY_DrawText(pme->m_pDisplay,
                                    AEE_FONT_NORMAL,
                                    pme->wstrMonth,
                                    -1,
                                    pme->rc_month.x,
                                    pme->rc_month.y,
                                    &pme->rc_month,
                                    IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE | IDF_TEXT_TRANSPARENT);

                //day
                if(pme->curSel == DATE_SEL_DAY)
                {
                    clrFrame = FRAME_COLOR;
                    clrFill  = clrNosel;
                }
                else
                {
                    clrFrame = clrNosel;
                    clrFill  = RGB_WHITE;
                }
                IDISPLAY_DrawRect(pme->m_pDisplay, &pme->rc_day, clrFrame, clrFill, IDF_RECT_FRAME | IDF_RECT_FILL);
                IDISPLAY_DrawText(pme->m_pDisplay,
                                    AEE_FONT_NORMAL,
                                    pme->wstrDay,
                                    -1,
                                    pme->rc_day.x,
                                    pme->rc_day.y,
                                    &pme->rc_day,
                                    IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE | IDF_TEXT_TRANSPARENT);

            }
            

        }
        IDISPLAY_UpdateEx(pme->m_pDisplay, FALSE);
        return TRUE;

        case EVT_DIALOG_END:
            
            return TRUE;
			
#ifdef FEATURE_LCD_TOUCH_ENABLE//ydc add for LCD touch
		case EVT_PEN_UP:
			{
				int i;
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
				int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
				
				nBarH = GetBottomBarHeight(pme->m_pDisplay);
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pme->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, devinfo.cyScreen);
				if ((wXPos>0)&&(wXPos<devinfo.cxScreen/3)&&(wYPos>rc.y)&&(wYPos<devinfo.cyScreen))
				{
					boolean rt =  ISHELL_PostEvent(pme->m_pShell,AEECLSID_SCHEDULEAPP,EVT_USER,AVK_SELECT,0);
					return rt;
				}
				else if ((wXPos>devinfo.cxScreen/3)&&(wXPos<(devinfo.cxScreen/3)*2)&&(wYPos>rc.y)&&(wYPos<devinfo.cyScreen))
				{
					boolean rt =  ISHELL_PostEvent(pme->m_pShell,AEECLSID_SCHEDULEAPP,EVT_USER,AVK_INFO,0);
					return rt;
				}
				else if ((wXPos>(devinfo.cxScreen/3)*2)&&(wXPos<devinfo.cxScreen)&&(wYPos>rc.y)&&(wYPos<devinfo.cyScreen))
				{
					boolean rt =  ISHELL_PostEvent(pme->m_pShell,AEECLSID_SCHEDULEAPP,EVT_USER,AVK_CLR,0);
					return rt;
				}
				else if(TOUCH_PT_IN_RECT(wXPos, wYPos, pme->rc_year))
				{
					pme->curSel = DATE_SEL_YEAR;
				}
				else if(TOUCH_PT_IN_RECT(wXPos, wYPos, pme->rc_month))
				{
					pme->curSel = DATE_SEL_MONTH;
				}
				else if(TOUCH_PT_IN_RECT(wXPos, wYPos, pme->rc_day))
				{
					pme->curSel = DATE_SEL_DAY;
				}
				MSG_FATAL("wXPos==========%d,wYPos====%d",wXPos,wYPos,0);
				(void) ISHELL_PostEvent(pme->m_pShell,
					AEECLSID_SCHEDULEAPP,
					EVT_USER_REDRAW,
					NULL,
					TRUE);

			}
			return TRUE;
#endif    
        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                case AVK_INFO:
                case AVK_SELECT:
                    if (WSTRLEN(pme->wstrYear) < 4)
                    {
                        CLOSE_DIALOG(DLGRET_TO_SHOWALERT)
                    }
                    else
                    {
                        pme->update = TRUE;
                        CLOSE_DIALOG(DLGRET_OPTION_VIEWMONTH_VIEW)
                    }
                    return TRUE;

                case AVK_LEFT:
                	{
	                    uint16 dateFormatType = 0;
						OEM_GetConfig( CFGI_DATE_FORMAT, &dateFormatType, sizeof( byte));
						switch(dateFormatType)
						{
						case OEMNV_DATEFORM_DMY:
						{
		                    if(pme->curSel == DATE_SEL_YEAR)
		                    {
		                        pme->curSel = DATE_SEL_MONTH;//DATE_SEL_DAY;
		                    }
		                    else if(pme->curSel == DATE_SEL_DAY)
		                    {
		                        pme->curSel = DATE_SEL_YEAR;//DATE_SEL_MONTH;
		                    }
		                    else if(pme->curSel == DATE_SEL_MONTH)
		                    {
		                        pme->curSel = DATE_SEL_DAY;//DATE_SEL_YEAR;
		                    }
	                    }
	                    break;
	                    case OEMNV_DATEFORM_MDY:
						{
		                    if(pme->curSel == DATE_SEL_YEAR)
		                    {
		                        pme->curSel = DATE_SEL_DAY;//DATE_SEL_DAY;
		                    }
		                    else if(pme->curSel == DATE_SEL_DAY)
		                    {
		                        pme->curSel = DATE_SEL_MONTH;//DATE_SEL_MONTH;
		                    }
		                    else if(pme->curSel == DATE_SEL_MONTH)
		                    {
		                        pme->curSel = DATE_SEL_YEAR;//DATE_SEL_YEAR;
		                    }
	                    }
	                    break;
	                    case OEMNV_DATEFORM_YMD:
						{
		                    if(pme->curSel == DATE_SEL_YEAR)
		                    {
		                        pme->curSel = DATE_SEL_DAY;//DATE_SEL_DAY;
		                    }
		                    else if(pme->curSel == DATE_SEL_DAY)
		                    {
		                        pme->curSel = DATE_SEL_MONTH;//DATE_SEL_MONTH;
		                    }
		                    else if(pme->curSel == DATE_SEL_MONTH)
		                    {
		                        pme->curSel = DATE_SEL_YEAR;//DATE_SEL_YEAR;
		                    }
	                    }
	                    break;
	                    default:
	                    break;
	                    }
                    (void) ISHELL_PostEvent(pme->m_pShell,
                                            AEECLSID_SCHEDULEAPP,
                                            EVT_USER_REDRAW,
                                            NULL,
                                            TRUE);
                    }
                    return TRUE;

                case AVK_RIGHT:

                    {
                    	uint16 dateFormatType = 0;
						OEM_GetConfig( CFGI_DATE_FORMAT, &dateFormatType, sizeof( byte));
						switch(dateFormatType)
						{
						case OEMNV_DATEFORM_DMY:
						{
		                    if(pme->curSel == DATE_SEL_DAY)
		                    {
		                        pme->curSel = DATE_SEL_MONTH;//DATE_SEL_YEAR;
		                    }
		                    else if(pme->curSel == DATE_SEL_YEAR)
		                    {
		                        pme->curSel = DATE_SEL_DAY;//DATE_SEL_MONTH;
		                    }
		                    else if(pme->curSel == DATE_SEL_MONTH)
		                    {
		                        pme->curSel = DATE_SEL_YEAR;//DATE_SEL_DAY;
                    		}
	                    }
	                    break;
	                    case OEMNV_DATEFORM_MDY:
						{
		                    if(pme->curSel == DATE_SEL_YEAR)
		                    {
		                        pme->curSel = DATE_SEL_MONTH;//DATE_SEL_DAY;
		                    }
		                    else if(pme->curSel == DATE_SEL_DAY)
		                    {
		                        pme->curSel = DATE_SEL_YEAR;//DATE_SEL_MONTH;
		                    }
		                    else if(pme->curSel == DATE_SEL_MONTH)
		                    {
		                        pme->curSel = DATE_SEL_DAY;//DATE_SEL_YEAR;
		                    }
	                    }
	                    break;
	                    case OEMNV_DATEFORM_YMD:
						{
		                    if(pme->curSel == DATE_SEL_YEAR)
		                    {
		                        pme->curSel = DATE_SEL_MONTH;//DATE_SEL_DAY;
		                    }
		                    else if(pme->curSel == DATE_SEL_DAY)
		                    {
		                        pme->curSel = DATE_SEL_YEAR;//DATE_SEL_MONTH;
		                    }
		                    else if(pme->curSel == DATE_SEL_MONTH)
		                    {
		                        pme->curSel = DATE_SEL_DAY;//DATE_SEL_YEAR;
		                    }
	                    }
	                    break;
	                    default:
	                    break;
	                    }
                    
                    }
                    (void) ISHELL_PostEvent(pme->m_pShell,
                                            AEECLSID_SCHEDULEAPP,
                                            EVT_USER_REDRAW,
                                            NULL,
                                            TRUE);
                    
                    return TRUE;

                case AVK_0:
                case AVK_1:
                case AVK_2:
                case AVK_3:
                case AVK_4:
                case AVK_5:
                case AVK_6:
                case AVK_7:
                case AVK_8:
                case AVK_9:
                {
                    switch(pme->curSel)
                    {
                        case DATE_SEL_YEAR:
                        {
                            int nLen = WSTRLEN(pme->wstrYear);
                            
                            if (nLen >= 4)
                            {
                                MEMSET(pme->wstrYear, 0, sizeof(pme->wstrYear));
                                nLen = 0;
                            }
                            pme->wstrYear[nLen] = (AECHAR)('0'+wParam-AVK_0);
                            nLen++;
                            pme->wstrYear[nLen] = 0;
                        }
                        break;


                        case DATE_SEL_MONTH:
                        {
                            int nLen = WSTRLEN(pme->wstrMonth);
                            
                            if (nLen != 1)
                            {
                                if (wParam == AVK_0)
                                {
                                    return TRUE;
                                }
                                MEMSET(pme->wstrMonth, 0, sizeof(pme->wstrMonth));
                                nLen = 0;
                            }
                            else
                            {
                                if ((pme->wstrMonth[0] != (AECHAR)'1') ||
                                    (wParam>AVK_2))
                                {
                                    nLen = 0;
                                }
                            }
                            pme->wstrMonth[nLen] = (AECHAR)('0'+wParam-AVK_0);
                            pme->wstrMonth[nLen+1] = 0;
                        }
                        break;

                        // 当前输入---日
                        case DATE_SEL_DAY:
                        {
                            int     nLen, wYear=0, wNonth=0, nMaxDays=0;
                            char    strTep[5] = {0};
                            
                            (void)WSTRTOSTR(pme->wstrYear, strTep, sizeof(strTep));
                            wYear = STRTOUL(strTep, NULL, 10);
                            (void)WSTRTOSTR(pme->wstrMonth, strTep, sizeof(strTep));
                            wNonth = STRTOUL(strTep, NULL, 10);
                            //计算每个月最多几天
                            {
                                if (wNonth == 2)
                                {
                                    nMaxDays =  (((((wYear)%4)==0) && (((wYear)%100) || (((wYear)%400)==0))) ? 29 : 28);
                                }
                                else
                                {
                                    wNonth = (wNonth > 2) ? wNonth-3 : wNonth+9;
                                    nMaxDays = ((wNonth % 5) & 1) ? 30 : 31;
                                }

                            }
                            nLen = WSTRLEN(pme->wstrDay);
                            if (nLen < 2)
                            {
                                pme->wstrDay[nLen] = (AECHAR)('0'+wParam-AVK_0);
                                pme->wstrDay[nLen+1] = 0;
                                
                                (void)WSTRTOSTR(pme->wstrDay, strTep, sizeof(strTep));
                                nLen = STRTOUL(strTep, NULL, 10);
                                if (nLen>nMaxDays)
                                {
                                    pme->wstrDay[0] = (AECHAR)('0'+wParam-AVK_0);
                                    pme->wstrDay[1] = 0;
                                }
                            }
                            else if (wParam != AVK_0)
                            {
                                pme->wstrDay[0] = (AECHAR)('0'+wParam-AVK_0);
                                pme->wstrDay[1] = 0;
                            }
                        }
                        break;
                    }
                    (void) ISHELL_PostEvent(pme->m_pShell,
                                            AEECLSID_SCHEDULEAPP,
                                            EVT_USER_REDRAW,
                                            NULL,
                                            TRUE);                     
                }
               
                return TRUE;
            }
            return TRUE;
    }
    return FALSE;

}


static void drawLeftRightArrows( CScheduleApp *pme, int x1, int y1, int x2, int y2)
{
    IImage      *pImageArrowRight = NULL;
    IImage      *pImageArrowLeft = NULL;
    pImageArrowLeft  = ISHELL_LoadResImage(pme->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDB_LEFTARROW);
    pImageArrowRight = ISHELL_LoadResImage(pme->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDB_RIGHTARROW);

    if(pImageArrowLeft != NULL)
    {
        IIMAGE_Draw( pImageArrowLeft, x1, y1);
        IIMAGE_Release(pImageArrowLeft);
    }

    if(pImageArrowRight != NULL)
    {
        IIMAGE_Draw( pImageArrowRight, x2, y2);
        IIMAGE_Release(pImageArrowRight);
    }
}

static void copyTextContentToVariables( CScheduleApp *pme, ITextCtl* pSubject, ITextCtl* pNote)
{
    if( !ITEXTCTL_GetText( pSubject, pme->m_CalMgr.m_szEventDes, /*MAXTEXT*/sizeof(pme->m_CalMgr.m_szEventDes)))
    {
    	MSG_FATAL("pme->m_CalMgr.m_szEventDes............",0,0,0);
        pme->m_CalMgr.m_szEventDes[0] = 0;
    }

    if( !ITEXTCTL_GetText( pNote, pme->m_CalMgr.m_szNote, /*MAXTEXT*/sizeof(pme->m_CalMgr.m_szNote)))
    {
    	MSG_FATAL("pme->m_CalMgr.m_szNote................",0,0,0);
        pme->m_CalMgr.m_szNote[0] = 0;
    }
}


static boolean  dialog_handler_of_state_event_edit( CScheduleApp* pme,
                                                      AEEEvent       eCode,
                                                      uint16         wParam,
                                                      uint32         dwParam)
{

    static IControl*   pControls[12]    = {0};
    static ITextCtl*   pSubject         = 0;
    static ITextCtl*   pNote            = 0;
    static ITimeCtl*   pStart           = 0;
    static ITimeCtl*   pDuration        = 0;
    static IMenuCtl*   pAlarm           = 0;
    static IMenuCtl*   pRingtone        = 0;
    static IMenuCtl*   pMode            = 0;

    static int         titleBarHeight      = 0;
    static int         bottomBarHeight     = 0;
    static int         itemNumberPerPage   = 0;
    static int         currentItem         = 0;
    static int         itemHeight          = 0;
    static int         fontHeight          = 0;
    static int         itemSpace           = 0;
    static boolean     inited              = 0;

    static Theme_Param_type    themeParms   = {0};
    static AEERect             rectWindow   = {0};

    // 1 - saving
    // 2 - db error
    static byte subState = 0;

    static AECHAR   subjectValue[MAXTEXT+1]     = {0};
    static AECHAR   locationValue[MAXTEXT+1]    = {0};
    static uint32   startTimeValue            = 0;
    static uint32   durationValue             = 0;
    static uint16   sportsmodeValue           = 0;
    static uint16   alarmValue                = 0;
    static uint16   ringtoneValue             = 0;
    static boolean  exitByUser                = 0;
    static byte     timeFormatType            = 0;

#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch

		if ((eCode == EVT_PEN_UP) || (eCode == EVT_PEN_DOWN))
		{
			int i;
			int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
			int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
			int xy[][4] = {
				{0,38,20,58},		  // Menu1 LEFT
				{146,38,166,58},	  // Menu1 RIGHT
				{0,84,20,104},		  // Menu2 LEFT
				{146,84,166,104},	  // Menu2 RIGHT
				{0,174,20,194}, 	  // Menu3 LEFT
				{146,174,166,194}	  // Menu3 RIGHT
			};
			for( i = 0; i < 6; i ++)
			{
				if( wXPos >= xy[i][0] &&
					wXPos <= xy[i][2] &&
					wYPos >= xy[i][1] &&
					wYPos <= xy[i][3]
				)
				{
					if (i == 0)
					{
						if (currentItem == 4)
						{
							eCode = EVT_KEY;
							wParam = AVK_LEFT;
						}
					}
					else if (i == 1)
					{
						if (currentItem == 4)
						{
							eCode = EVT_KEY;
							wParam = AVK_RIGHT;
						}
					}
					else if (i == 2)
					{
						if (currentItem == 5)
						{
							eCode = EVT_KEY;
							wParam = AVK_LEFT;
						}
					}
					else if (i == 3)
					{
						if (currentItem == 5)
						{
							eCode = EVT_KEY;
							wParam = AVK_RIGHT;
						}
					}
					else if (i == 4)
					{
						if (currentItem ==3)
						{
							eCode = EVT_KEY;
							wParam = AVK_LEFT;
						}
					}
					else if (i == 5)
					{
						if (currentItem ==3)
						{
							eCode = EVT_KEY;
							wParam = AVK_RIGHT;
						}
							
					}
	
				}
			}
	
		}
	
#endif 

    switch (eCode)
    {
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch

				//case EVT_PEN_DOWN:
				case EVT_PEN_UP:
					{
						int i;
						int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
						int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
						int xy[][4] = {
							{167,20,176,49},	  // Menu1
							{167,50,176,79},	  // Menu2
							{167,80,176,109},	  // Menu3
							{167,110,176,139},	  // Menu4
							{167,140,176,169},	  // Menu5
							{167,170,176,200},	  // Menu6
							{20,55,146,75},       // Menu11
							{20,105,146,125},     // Menu12
							{20,155,146,175},     // Menu13
							{0,182,58,202},
							{59,182,116,202},
							{117,182,176,202},
							{1,105,20,125},        // Menu5 left
							{146,105,166,125},     // Menu5 right
							{1,155,20,175},        // Menu6 left
							{146,155,166,175},     // Menu6 right

						};
						for( i = 0; i < 18; i ++)
						{
							if( wXPos >= xy[i][0] &&
								wXPos <= xy[i][2] &&
								wYPos >= xy[i][1] &&
								wYPos <= xy[i][3]
							)
							{
								if (i == 0)
								{
									currentItem = 0;
									repaint(pme, TRUE);
								}
								else if (i == 1)
								{
									currentItem = 1;
									repaint(pme, TRUE);
								}
								else if (i == 2)
								{
									currentItem = 2;
									repaint(pme, TRUE);
								}
								else if (i == 3)
								{
									currentItem = 3;
									repaint(pme, TRUE);
								}
								else if (i == 4)
								{
									currentItem = 4;
									repaint(pme, TRUE);
								}
								else if (i == 5)
								{
									currentItem  =5;
									repaint(pme, TRUE);
								}
								else if (i == 6)
								{
									if (currentItem >2)
									{
										currentItem = 3;
										repaint(pme, TRUE);
									}
									else
									{
										currentItem = 0;
										repaint(pme, TRUE);

									}
								}
								else if (i == 7)
								{
									if (currentItem >2)
									{
										currentItem = 4;
										repaint(pme, TRUE);
									}
									else
									{
										currentItem = 1;
										repaint(pme, TRUE);
									}
								}
								else if (i == 8)
								{
									if (currentItem >2)
									{
										currentItem = 5;
										repaint(pme, TRUE);
									}
									else
									{
										currentItem = 2;
										repaint(pme, TRUE);
									}
								}
								else if (i == 9)
								{
									return  ISHELL_PostEvent(pme->m_pShell,AEECLSID_SCHEDULEAPP,EVT_USER,AVK_SELECT,NEVT_KEY_PRESSS);
								}
								else if (i == 10)
								{
									repaint(pme, TRUE);
									return TRUE;
								}
								else if (i == 11)
								{
									return  ISHELL_PostEvent(pme->m_pShell,AEECLSID_SCHEDULEAPP,EVT_USER,AVK_CLR,0);
								}
								else if (i == 12)
								{
									if (currentItem>2)
									{
										return  ISHELL_PostEvent(pme->m_pShell,AEECLSID_SCHEDULEAPP,EVT_USER,AVK_LEFT,0);
									}

								}
								else if (i == 13)
								{
									if (currentItem>2)
									{
										return  ISHELL_PostEvent(pme->m_pShell,AEECLSID_SCHEDULEAPP,EVT_USER,AVK_RIGHT,0);
									}
								}
								else if (i == 14)
								{
									if (currentItem>2)
									{
										return  ISHELL_PostEvent(pme->m_pShell,AEECLSID_SCHEDULEAPP,EVT_USER,AVK_LEFT,0);
									}

								}
								else if (i == 15)
								{
									if (currentItem>2)
									{
										return  ISHELL_PostEvent(pme->m_pShell,AEECLSID_SCHEDULEAPP,EVT_USER,AVK_RIGHT,0);
									}
								}
								
							//repaint(pme, TRUE);
							}
						}
		
					}
					return TRUE;
#endif        

        case EVT_DIALOG_END:
        {
            MEMSET( subjectValue, 0, sizeof( subjectValue));
            MEMSET( locationValue, 0, sizeof( locationValue));
            startTimeValue      = 0;
            durationValue       = 0;
            alarmValue          = 0;
            ringtoneValue       = 0;
            if( !pme->m_closeByPowerKey && !exitByUser)
            {
                ITEXTCTL_GetText( pSubject, subjectValue, MAXTEXT - 1+2);
                ITEXTCTL_GetText( pNote, locationValue, MAXTEXT - 1+2);

                startTimeValue  = ITIMECTL_GetTime( pStart);
                if(pme->m_sports)
                {
                    sportsmodeValue = IMENUCTL_GetSel( pMode);
                }
                else
                {
                    durationValue   = ITIMECTL_GetTime( pDuration);
                }
                alarmValue      = IMENUCTL_GetSel( pAlarm);
                ringtoneValue   = IMENUCTL_GetSel( pRingtone);
            }
            exitByUser = 0;
            pSubject   = 0;
        }
        return TRUE;

        case EVT_DIALOG_START:
        {
//            AEERect rect = { 0};
//            SETAEERECT( &rect, 1-pme->m_rc.dx, 1-pme->m_rc.dy, 0, 0);
//            IDISPLAY_SetClipRect( pme->m_pDisplay, &rect);
//
            subState = 0; 
        }
        return TRUE;

        case EVT_DIALOG_INIT:
        {

            // init parms
            Appscom_GetThemeParameters( &themeParms);
            fontHeight          = IDISPLAY_GetFontMetrics( pme->m_pDisplay, AEE_FONT_NORMAL, 0, 0);
            titleBarHeight      = 0;//GetTitleBarHeight( pme->m_pDisplay);
            bottomBarHeight     = GetBottomBarHeight( pme->m_pDisplay);
            itemHeight          = ( fontHeight + 2) * 2 + 2 + 6;
            itemNumberPerPage   = ( pme->m_rc.dy - titleBarHeight - bottomBarHeight) / itemHeight;
            if( pme->m_ePreState != STATE_INPUTTEXT && !pme->m_bInactive)
            {
                currentItem = 0;
            }
            SETAEERECT( &rectWindow, 0, titleBarHeight, pme->m_rc.dx, pme->m_rc.dy - titleBarHeight - bottomBarHeight);
            itemSpace = ( rectWindow.dy - itemHeight * itemNumberPerPage) / ( itemNumberPerPage + 1);

            IDIALOG_SetProperties( pme->m_pActiveDlg, DLG_NOT_SET_FOCUS_AUTO);
            repaint(pme, TRUE);

            inited = 0;
            //ISHELL_SetTimer( pme->m_pShell, 1000, loadDataTimerCB, pme);
        }
        //return TRUE;

        case EVT_LOAD_DATA:
        {

            int             i           = 0;
            AEEMenuColors   color       = {
                                        MC_SEL_TEXT | MC_SEL_BACK | MC_TEXT | MC_BACK,
                                        RGB_WHITE,
                                        0x84848400,
                                        RGB_WHITE,
                                        RGB_BLACK,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0
                                    };

            extern int OEM_GetConfig( AEEConfigItem i, void * pBuff, int nSize);
            OEM_GetConfig( CFGI_TIME_FORMAT, &timeFormatType, sizeof( byte));
#if defined( AEE_SIMULATOR)
            timeFormatType = OEMNV_TIMEFORM_AMPM;
            //timeFormatType = OEMNV_TIMEFORM_24HR;
#endif


            for (i = 0; i < 12; i ++)
            {
                pControls[i] = IDIALOG_GetControl(pme->m_pActiveDlg, IDC_STATIC_SUBJECT + i);
                ICONTROL_SetActive( pControls[i], FALSE);
            }
            if(pme->m_sports)
            {
                pControls[0] = IDIALOG_GetControl(pme->m_pActiveDlg, IDC_STATIC_SPORT_DISTANCE);
                pControls[2] = IDIALOG_GetControl(pme->m_pActiveDlg, IDC_STATIC_SPORT_TIME);
                pControls[6] = IDIALOG_GetControl(pme->m_pActiveDlg, IDC_STATIC_SPORTS_MODE);
                pControls[7] = IDIALOG_GetControl(pme->m_pActiveDlg, IDC_LIST_SPORTS);
                
                ICONTROL_SetActive( pControls[0], FALSE);
                ICONTROL_SetActive( pControls[2], FALSE);
                ICONTROL_SetActive( pControls[6], FALSE);
                ICONTROL_SetActive( pControls[7], FALSE);
            }

            for (i = 0; i < 6; i ++)
            {
                ISTATIC_SetProperties( (IStatic*)pControls[i<<1], ST_MIDDLETEXT|ST_TRANSPARENTBACK);
                ISTATIC_SetFontColor( (IStatic*)pControls[i<<1], RGB_WHITE);
            }

            pSubject    = (ITextCtl*)pControls[1];
            pNote       = (ITextCtl*)pControls[3];
            pStart      = (ITimeCtl*)pControls[5];
            if(pme->m_sports)
            {
                pMode   = (IMenuCtl*)pControls[7];
            }
            else
            {
                pDuration= (ITimeCtl*)pControls[7];
            }
            pAlarm      = (IMenuCtl*)pControls[9];
            pRingtone   = (IMenuCtl*)pControls[11];           

            for( i = 0; i < 11; i ++)
            {
                IMENUCTL_AddItem( pAlarm,
                        AEE_SCHEDULEAPP_RES_FILE,
                        IDS_EVENT_EDIT_ALARM_OPTION_NO_ALARM + i,
                        IDS_EVENT_EDIT_ALARM_OPTION_NO_ALARM + i,
                        0,
                        0
                    );
            }

            if(pme->m_sports)
            {
                for( i = 0; i < 2; i ++)
                {
                    IMENUCTL_AddItem( pMode,
                            AEE_SCHEDULEAPP_RES_FILE,
                            IDS_SPORTS_RUN + i,
                            IDS_SPORTS_RUN + i,
                            0,
                            0
                            );
                }
            }

            if( alarmValue > 0)
            {
                IMENUCTL_SetSel( pAlarm, alarmValue);
            }
            else if( pme->m_subStateEventEdit == SUBSTATE_EVENT_EDIT_EDIT && pme->m_CalMgr.m_pceCurrent)
            {
                static uint16 alarmTime[]   = { 65535, 0, 1, 5, 10, 15, 30, 60, 360, 720, 1440};
                       int    alarmSetting  = 0;

                while( alarmTime[alarmSetting] != pme->m_CalMgr.m_pceCurrent->m_b.wAlarmTime && alarmSetting < 11)
                {
                    alarmSetting ++;
                }

                //debug( ";event edit, alarmTime[%d] == %d", alarmSetting, alarmTime[alarmSetting]);
                IMENUCTL_SetSel( pAlarm, IDS_EVENT_EDIT_ALARM_OPTION_NO_ALARM + alarmSetting);
            }
            else
            {
#if defined(FEATURE_VERSION_C337)    
                IMENUCTL_SetSel( pAlarm, IDS_EVENT_EDIT_ALARM_OPTION_ON_TIME);                		 
#else
                IMENUCTL_SetSel( pAlarm, IDS_EVENT_EDIT_ALARM_OPTION_15_MIN_BEFORE);
#endif
            }

            for( i = 0; i < 2; i ++)
            {
                IMENUCTL_AddItem( pRingtone,
                        AEE_SCHEDULEAPP_RES_FILE,
                        IDS_EVENT_EDIT_RINGTONE_OPTION_ALARM_CLOCK + i,
                        IDS_EVENT_EDIT_RINGTONE_OPTION_ALARM_CLOCK + i,
                        0,
                        0
                    );
            }
            if( ringtoneValue > 0)
            {
                IMENUCTL_SetSel( pRingtone, ringtoneValue);
            }
            else if( pme->m_subStateEventEdit == SUBSTATE_EVENT_EDIT_EDIT &&
                pme->m_CalMgr.m_pceCurrent &&
                pme->m_CalMgr.m_pceCurrent->m_b.alertst == SILENCE
            )
            {
                IMENUCTL_SetSel( pRingtone, IDS_EVENT_EDIT_RINGTONE_OPTION_SILENCE);
            }

            ITIMECTL_SetProperties( pStart, TP_NO_SECONDS | TP_AUTOREDRAW);
            if( ! pme->m_sports)
            {
                ITIMECTL_SetProperties( pDuration, TP_NO_SECONDS | TP_AUTOREDRAW);
            }
            {
                uint32 startTimeByMilliseconds = pme->m_CalMgr.m_cfg.wDayStart*60*1000;
                uint32 durationByMilliseconds  = 60*60*1000;
                if( pme->m_subStateEventEdit == SUBSTATE_EVENT_EDIT_EDIT && pme->m_CalMgr.m_pceCurrent)
                {
                    startTimeByMilliseconds = GETTIME( pme->m_CalMgr.m_pceCurrent->m_b.dwTime)*60*1000;
                    durationByMilliseconds  = pme->m_CalMgr.m_pceCurrent->m_b.wDuration*60*1000;
                    //debug( ";event edit, start time == %d", GETTIME( pme->m_CalMgr.m_pceCurrent->m_b.dwTime));
                }
                else if( pme->m_subStateEventEdit == SUBSTATE_EVENT_EDIT_NEW && pme->m_CalMgr.m_julianCurrentDay.wSecond > 0)
                {
                    startTimeByMilliseconds  = pme->m_CalMgr.m_julianCurrentDay.wHour*60*60*1000;
                    startTimeByMilliseconds += pme->m_CalMgr.m_julianCurrentDay.wMinute*60*1000;
                }

                if( startTimeValue > 0)
                {
                    startTimeByMilliseconds = startTimeValue;
                }
                if( durationValue > 0)
                {
                    durationByMilliseconds = durationValue;
                }
                ITIMECTL_SetTimeEx( pStart, startTimeByMilliseconds, TRUE);
                if( !pme->m_sports)
                {
                    ITIMECTL_SetTimeEx( pDuration, durationByMilliseconds, TRUE);
                }
            }
//          ITEXTCTL_SetInputMode( pSubject, AEE_TM_CURRENT);
//          ITEXTCTL_SetInputMode( pNote, AEE_TM_CURRENT);

            if(!pme->m_sports)
            {
                ITEXTCTL_SetProperties( pSubject, TP_FIXOEM | TP_FIXSETRECT /*| TP_STARKEY_SWITCH*/|TP_FOCUS_NOSEL|TP_MULTILINE);
                ITEXTCTL_SetProperties( pNote, TP_FIXOEM | TP_FIXSETRECT /*| TP_STARKEY_SWITCH*/|TP_MULTILINE);
                #if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
                (void)ITEXTCTL_SetInputMode( pNote, AEE_TM_RAPID);
                (void)ITEXTCTL_SetInputMode( pSubject, AEE_TM_RAPID);
                #elif defined(FEATURE_VERSION_C306)
                {
                	nv_language_enum_type language;
	        	    OEM_GetConfig( CFGI_LANGUAGE_SELECTION,&language,sizeof(language));
	                if(NV_LANGUAGE_ARABIC == language)
	                {
	        	    	(void)ITEXTCTL_SetInputMode( pNote, AEE_TM_ARABIC);
                		(void)ITEXTCTL_SetInputMode( pSubject, AEE_TM_ARABIC);
	        	    }
	        	    else if(NV_LANGUAGE_THAI== language)
	        	    {
	        	    	(void)ITEXTCTL_SetInputMode( pNote, AEE_TM_THAI);
                		(void)ITEXTCTL_SetInputMode( pSubject, AEE_TM_THAI);
	        	    }
	        	    else
	        	    {
	        	    	(void)ITEXTCTL_SetInputMode( pNote, AEE_TM_LETTERS);
                		(void)ITEXTCTL_SetInputMode( pSubject, AEE_TM_LETTERS);
	        	    }
                }
                #else
                (void)ITEXTCTL_SetInputMode( pNote, AEE_TM_LETTERS);
                (void)ITEXTCTL_SetInputMode( pSubject, AEE_TM_LETTERS);
                #endif
                (void)ITEXTCTL_SetMaxSize(pSubject, MAX_INPUT_SUBJECT);
                (void)ITEXTCTL_SetMaxSize(pNote, MAX_INPUT_LOCATION);                
            }
            else
            {
                ITEXTCTL_SetProperties( pSubject, TP_FIXOEM | TP_FIXSETRECT|TP_FOCUS_NOSEL);
                ITEXTCTL_SetProperties( pNote, TP_FIXOEM | TP_FIXSETRECT);
                (void)ITEXTCTL_SetInputMode(pSubject, AEE_TM_NUMBERS);
                (void)ITEXTCTL_SetInputMode(pNote, AEE_TM_NUMBERS);
                (void)ITEXTCTL_SetMaxSize(pSubject, MAX_SPORTS_DISTANCE);
                (void)ITEXTCTL_SetMaxSize(pNote, MAX_SPORTS_TIME);
            }
            
            ITIMECTL_SetOemProperties( pStart, TP_OEM_COUNTDOWNCTL_EDITABLE|((timeFormatType==OEMNV_TIMEFORM_AMPM)?TP_OEM_COUNTDOWNCTL_12_FORMAT:0));
            if(pme->m_sports)
            {
                IMENUCTL_SetOemProperties( pMode, OEMMP_SWITCHNAVIGATIONKEY | OEMMP_IDF_ALIGN_CENTER);
            }
            else
            {
                ITIMECTL_SetOemProperties( pDuration, TP_OEM_COUNTDOWNCTL_EDITABLE);
            }

            if( subjectValue[0] > 0)
            {
                ITEXTCTL_SetText( pSubject, subjectValue, -1);
            }
            else
            {
                ITEXTCTL_SetText( pSubject, pme->m_CalMgr.m_szEventDes, -1);
            }
            if( locationValue[0] > 0)
            {
                ITEXTCTL_SetText( pNote, locationValue, -1);
            }
            else
            {
                ITEXTCTL_SetText( pNote, pme->m_CalMgr.m_szNote, -1);
            }

            IMENUCTL_SetOemProperties( pAlarm, OEMMP_SWITCHNAVIGATIONKEY | OEMMP_IDF_ALIGN_CENTER);
            IMENUCTL_SetOemProperties( pRingtone, OEMMP_SWITCHNAVIGATIONKEY | OEMMP_IDF_ALIGN_CENTER);
            IMENUCTL_SetColors( pAlarm, &color);
            IMENUCTL_SetColors( pRingtone, &color);
            if(pme->m_sports)
            {
                IMENUCTL_SetColors( pMode, &color);
            }

            inited = TRUE;
            //ISHELL_SetTimer( pme->m_pShell, 500, redrawDialog, (void*)pme);
            repaint(pme, TRUE);
        }
        return TRUE;

        case EVT_USER_REDRAW:
        {
            #ifndef min
            #define min(a,b) (((a)<(b)) ? (a) : (b))
            #endif

            int     i               = 0;
            int     y               = 0;
            int     x               = 3;
            int     width           = rectWindow.dx - 6;
            int     height          = fontHeight + 2;
            int     itemStart       = ( currentItem / itemNumberPerPage) * itemNumberPerPage;
            int     itemNumber      = 6;
            int     itemEnd         = min( itemNumber, itemStart + itemNumberPerPage);
            AEERect rc              = { 0};

            TitleBar_Param_type     titleBarParms   = {0};
            BottomBar_Param_type    bottomBarParms  = {0};

            // clear screen
            if( itemNumberPerPage < itemNumber)
            {
                width -= 6;
            }
            IDISPLAY_SetClipRect( pme->m_pDisplay, &pme->m_rc);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            {
                IImage *pImageBg = ISHELL_LoadResImage(pme->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SCHEDULE_BACKGROUND);

                //Appscommon_ResetBackground(pme->m_pDisplay, pImageBg, APPSCOMMON_BG_COLOR, &rectWindow, 0, 0);
                if(pImageBg != NULL)
                {
                    Appscommon_ResetBackground(pme->m_pDisplay, pImageBg, APPSCOMMON_BG_COLOR, &rectWindow, 0, 0);
                    IImage_Release(pImageBg);
                }
            }
#else
            Appscommon_ResetBackgroundEx(pme->m_pDisplay, &rectWindow, TRUE);
#endif
            //IDISPLAY_FillRect( pme->m_pDisplay, &rectWindow, 0);

            // draw title and bottom bar
            titleBarParms.dwAlignFlags  = IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER;
            
            if(pme->m_sports)
            {
                titleBarParms.nTitleResID = IDS_APP;
            }
            else
            {
                titleBarParms.nTitleResID   = IDS_APP;
            }

			#if defined (FEATURE_VERSION_C337) || defined (FEATURE_VERSION_W317A)
			titleBarParms.nTitleResID   = IDS_CALENDAR;
			#endif
			
            STRCPY( titleBarParms.strTitleResFile, AEE_SCHEDULEAPP_RES_FILE);
			#if 0
            DrawTitleBar( pme->m_pDisplay, &titleBarParms);
			#else
			//IANNUNCIATOR_SetFieldText(pme->m_pIAnn,(uint16*)titleBarParms.strTitleResFile);
			#endif

            if( currentItem == 2 && timeFormatType == OEMNV_TIMEFORM_AMPM)
            {
                if( ITIMECTL_GetTime( pStart) >= 12*3600000)
                {
                    bottomBarParms.eBBarType = BTBAR_SAVE_AM_BACK;
                }
                else
                {
                    bottomBarParms.eBBarType = BTBAR_SAVE_PM_BACK;
                }
            }
            else
            {
            	AECHAR *pwsText;
                    
                pwsText = ITEXTCTL_GetTextPtr(pSubject);
                #ifndef FEATURE_ALL_KEY_PAD    //add by yangdecai 
                if( WSTRLEN(pwsText) > 0)
                {
                    bottomBarParms.eBBarType = BTBAR_SAVE_DELETE;
                }
                else
                {
                	bottomBarParms.eBBarType = BTBAR_SAVE_BACK;
                }
                #else
                bottomBarParms.eBBarType = BTBAR_SAVE_BACK;
                #endif
            }

            if(pme->m_sports)
            {
                if( currentItem == 0)
                {
                    AECHAR *pwsText;
                    
                    pwsText = ITEXTCTL_GetTextPtr(pSubject);
                    #ifndef FEATURE_ALL_KEY_PAD    //add by yangdecai 
                    if ( WSTRLEN(pwsText) > 0)
                    {
                        bottomBarParms.eBBarType = BTBAR_SAVE_DELETE;
                    }
                    #else
                    bottomBarParms.eBBarType = BTBAR_SAVE_BACK;
                    #endif
                }

                if( currentItem == 1)
                {
                    AECHAR *pwsText;

                    pwsText = ITEXTCTL_GetTextPtr(pNote);
					#ifndef FEATURE_ALL_KEY_PAD    //add by yangdecai 
                    if( WSTRLEN(pwsText) > 0)
                    {
                        bottomBarParms.eBBarType = BTBAR_SAVE_DELETE;
                    }
                    #else
                    bottomBarParms.eBBarType = BTBAR_SAVE_BACK;
                    #endif
                }
            }
            
            DrawBottomBar( pme->m_pDisplay, &bottomBarParms);

            if( inited)
            {

                SETAEERECT( &rc, 1-pme->m_rc.dx, 1-pme->m_rc.dy, 0, 0);
                IDISPLAY_SetClipRect( pme->m_pDisplay, &rc);

                // hide invalid controls
                for (i = 0; i < 12; i ++)
                {
                    ICONTROL_SetRect( pControls[i], &rc);
                    ICONTROL_SetActive( pControls[i], FALSE);
                }

                IDISPLAY_SetClipRect( pme->m_pDisplay, &rectWindow);
                y = rectWindow.y + (itemSpace + itemHeight) * ( currentItem - itemStart) + itemSpace + 3 + height;
                SETAEERECT( &rc, x - 1, y, width + 2, height + 2);
                IDISPLAY_DrawRect( pme->m_pDisplay, &rc, themeParms.themeColor, 0, IDF_RECT_FRAME);

                // show valid controls
                for( i = itemStart, y = rectWindow.y; i < itemEnd; i ++)
                {
                    y += itemSpace + 3;
                    SETAEERECT( &rc, x, y, width, height);
                    ICONTROL_SetRect( pControls[i<<1], &rc);

                    rc.y += height + 1;
                    if( i == 4 || i == 5)
                    {
                        IDISPLAY_FillRect( pme->m_pDisplay, &rc, RGB_WHITE);
                        drawLeftRightArrows( pme, x, rc.y + (rc.dy - ARROW_WIDTH) / 2, x + width - ARROW_WIDTH, rc.y + (rc.dy - ARROW_WIDTH) / 2);

                        rc.x    += ARROW_WIDTH;
                        rc.dx   -= ARROW_HEIGHT;
                    }
                    if(pme->m_sports)
                    { 
                        if( i == 3)
                        {
                            IDISPLAY_FillRect( pme->m_pDisplay, &rc, RGB_WHITE);
                            drawLeftRightArrows( pme, x, rc.y + (rc.dy - ARROW_WIDTH) / 2, x + width - ARROW_WIDTH, rc.y + (rc.dy - ARROW_WIDTH) / 2);

                            rc.x += ARROW_WIDTH;
                            rc.dx -= ARROW_HEIGHT;
                        }
                        
                    }
                    ICONTROL_SetRect( pControls[(i<<1)+1], &rc);

                    y += height * 2 + 2 + 3;

                    ICONTROL_Redraw( pControls[i<<1]);
                    ICONTROL_Redraw( pControls[(i<<1)+1]);
                }

                // set the focus to the current item
                ITEXTCTL_SetActive( pSubject, FALSE);
                ITEXTCTL_SetActive( pNote, FALSE);
                ICONTROL_SetActive( pControls[(currentItem<<1)+1], TRUE);

                IDISPLAY_SetClipRect( pme->m_pDisplay, &pme->m_rc);

                if(( currentItem == 0 || currentItem == 1)&& !pme->m_sports)
                {
                    drawImage( pme,
                            AEE_APPSCOMMONRES_IMAGESFILE,
                            IDI_NOTE_BOOK,
                            (pme->m_rc.dx - 13) / 2,
                            pme->m_rc.dy - 14 - (bottomBarHeight - 14) / 2
                        );
                }

                // draw scroll bar
                if( itemNumberPerPage < itemNumber)
                {
#ifdef FEATURE_SCROLLBAR_USE_STYLE
                    RGBVAL  ScrollbarClr = MAKE_RGB(0xDE, 0xDE, 0xDE),
                                ScrollbarFillClr = MAKE_RGB(0xFF, 0x70, 0x00);
#endif
                    height  = ( rectWindow.dy / itemNumber) * itemNumber;
                    y       = rectWindow.y + ( rectWindow.dy - height) / 2;
                    
#ifdef FEATURE_SCROLLBAR_USE_STYLE
                    SETAEERECT( &rc, pme->m_rc.dx - (SCROLLBAR_WIDTH - 2), y, (SCROLLBAR_WIDTH - 2), height);
                    IDISPLAY_FillRect(pme->m_pDisplay, &rc, ScrollbarClr);
                    rc.dy  = rc.dy / itemNumber;
                    rc.y  += rc.dy * currentItem;
                    IDISPLAY_FillRect(pme->m_pDisplay, &rc, ScrollbarFillClr);
#else
                    SETAEERECT( &rc, pme->m_rc.dx - 1, y, 1, height);
                    IDISPLAY_FillRect( pme->m_pDisplay, &rc, themeParms.textColor);
                    rc.dy  = rc.dy / itemNumber;
                    rc.y  += rc.dy * currentItem;
                    IDISPLAY_FillRect( pme->m_pDisplay, &rc, themeParms.themeColor);
#endif
                }
            }
            else
            {
                //drawLoading( pme);
            }


            // 1 - saving, 2 - db error
            if( subState == 1)
            {
                drawPrompt( pme, IDS_SAVING);
            }
            else if( subState == 2)
            {

                uint16 resId = IDS_DB_ERROR;
                switch( pme->m_CalMgr.m_dbOperationResult)
                {
                    case -2000:
                    case ENOMEMORY:
                        resId = IDS_DB_ERROR_NOMEMORY;
                        break;
                }
                drawModalDialog( pme->m_pDisplay, (IStatic*)pControls[0], resId, FALSE);
            }

            IDISPLAY_UpdateEx( pme->m_pDisplay, FALSE);
        }
        return TRUE;
		case EVT_KEY_RELEASE:
			{
				switch( wParam)
            	{
					case AVK_0:  //ADD BY yangdecai 2011-04-06
	                case AVK_1:
	                case AVK_2:
	                case AVK_3:
	                case AVK_4:
	                case AVK_5:
	                case AVK_6:
	                case AVK_7:
	                case AVK_8:
	                case AVK_9:
	                	return TRUE;
	                	break;
                }
                return FALSE;
			}
        case EVT_KEY:
        {

            if( !inited || subState == 1)
            {
                return TRUE;
            }

            switch( wParam)
            {
                case AVK_SOFT2:
                case AVK_CLR:
                if(pme->m_sports)
                {
                    exitByUser = TRUE;
                    if(currentItem == 0 || currentItem == 1)
                    {
                    	#ifndef FEATURE_ALL_KEY_PAD
                    	if(dwParam == 1)
                    	#endif
                    	{
                        	AECHAR *pwsText = NULL;
                        	{
                            	if(currentItem == 0)
                            	{
                               	 	pwsText = ITEXTCTL_GetTextPtr(pSubject);
                            	}
                            	else if(currentItem == 1)
                            	{
                            	    pwsText = ITEXTCTL_GetTextPtr(pNote);
                            	}
                        	}
                        	if (WSTRLEN(pwsText)>0)
                        	{
                            	repaint(pme,TRUE);
                        	}
                        	else
                        	{
                            	CLOSE_DIALOG(DLGRET_CANCELED)
                        	}
                        }
                        #ifndef FEATURE_ALL_KEY_PAD
                        else
                        {
                        	CLOSE_DIALOG(DLGRET_CANCELED)
                        }
                        #endif
                        
                    }
                    else
                    {
                        CLOSE_DIALOG(DLGRET_CANCELED)
                    }
                }
                else
                {

                    if( subState == 2)
                    {
                    	#ifndef FEATURE_ALL_KEY_PAD
                        subState = 0;
                        repaint(pme, TRUE);
                        #else
                        CLOSE_DIALOG(DLGRET_CANCELED)
                        #endif
                    }
                    else
                    {
                        exitByUser = TRUE;
                        CLOSE_DIALOG(DLGRET_CANCELED)
                    }
                }
                break;
                case AVK_SELECT:
                case AVK_INFO:
                case AVK_0:  //ADD BY yangdecai 2011-04-06
                case AVK_1:
                case AVK_2:
                case AVK_3:
                case AVK_4:
                case AVK_5:
                case AVK_6:
                case AVK_7:
                case AVK_8:
                case AVK_9:
                	MSG_FATAL("AVK_SELECT AVK_INFO AVK_0 AVK_1 AVK_2 AVK_3",0,0,0);
#if defined( AEE_SIMULATOR)
                if( subState == 0 && wParam == AVK_SELECT && (currentItem == 0 || currentItem == 1))
#else
                if( subState == 0 && (wParam == AVK_INFO ||wParam == AVK_0 ||wParam == AVK_1 ||wParam == AVK_2
                	||wParam == AVK_3 || wParam == AVK_4 ||wParam == AVK_5 ||wParam == AVK_6 ||wParam == AVK_7 
                	||wParam == AVK_8 || wParam == AVK_9) && (currentItem == 0 || currentItem == 1))
#endif
                {
                    if(pme->m_sports)
                    {
                        repaint(pme,TRUE);
                    }
                    else
                    {
                        copyTextContentToVariables( pme, pSubject, pNote);
                        MSG_FATAL("currentItem===============================%d",currentItem,0,0);
                        pme->m_pszEventEditText = currentItem == 0 ? pme->m_CalMgr.m_szEventDes : pme->m_CalMgr.m_szNote;
                        pme->m_eventEditItem    = currentItem;
                        exitByUser = TRUE;
                        CLOSE_DIALOG( DLGRET_EVENT_EDIT_SUBJECT)
                    }
                    return TRUE;
                }
                break;

_scheduleapp_event_edit_save_:
                if( subState == 0)
                {

                    static uint16       alarmTime[] = { 65535, 0, 1, 5, 10, 15, 30, 60, 360, 720, 1440};
                    static AlertStates  ringtone[]  = { ALERTSET, SILENCE};
                    static WalkMode     walkmode[]  = { RUN, WALK};
                           boolean      changed     = FALSE;

                    subState = 1;
                    repaint(pme, FALSE);


                    if ( pme->m_CalMgr.m_pceCurrent)
                    {
                        uint32    duration1 = 0;

                        uint32      startTime1    = ITIMECTL_GetTime( pStart) / 1000 / 60;
                        //uint32      duration1     = ITIMECTL_GetTime( pDuration) / 1000 / 60;
                        uint16      alarmTime1    = alarmTime[IMENUCTL_GetSel( pAlarm) - IDS_EVENT_EDIT_ALARM_OPTION_NO_ALARM];
                        AlertStates alertState1   = ringtone[IMENUCTL_GetSel( pRingtone) - IDS_EVENT_EDIT_RINGTONE_OPTION_ALARM_CLOCK];
                        if(!pme->m_sports)
                        {
                            duration1  = ITIMECTL_GetTime( pDuration) / 1000 / 60;
                        }

                        if( WSTRCMP( ITEXTCTL_GetTextPtr( pSubject), pme->m_CalMgr.m_szEventDes)  ||
                            WSTRCMP( ITEXTCTL_GetTextPtr( pNote), pme->m_CalMgr.m_szNote)         ||
                            duration1 != pme->m_CalMgr.m_pceCurrent->m_b.wDuration
                        )
                        {
                            changed = TRUE;
                        }

                        if( startTime1  != GETTIME( pme->m_CalMgr.m_pceCurrent->m_b.dwTime)        ||
                            alarmTime1  != pme->m_CalMgr.m_pceCurrent->m_b.wAlarmTime              ||
                            alertState1 != pme->m_CalMgr.m_pceCurrent->m_b.alertst
                        )
                        {
                            changed = TRUE;
                            IAlarm_CancelAlarm( pme->m_CalMgr.m_pIAlarm, AEECLSID_SCHEDULEAPP, pme->m_CalMgr.m_pceCurrent->nPermRecID);
                        }
                    }

                    copyTextContentToVariables( pme, pSubject, pNote);

                    pme->m_CalMgr.m_lEventDay   = pme->m_CalMgr.m_lCurrentDay;
                    pme->m_CalMgr.m_lStartTime  = ITIMECTL_GetTime( pStart) / 1000 / 60;
                    if(!pme->m_sports)
                    {
                    pme->m_CalMgr.m_lDuration   = ITIMECTL_GetTime( pDuration) / 1000 / 60;
                    }
                    pme->m_CalMgr.m_nAlarmTime  = alarmTime[IMENUCTL_GetSel( pAlarm) - IDS_EVENT_EDIT_ALARM_OPTION_NO_ALARM];
                    pme->m_CalMgr.m_AlertState  = ringtone[IMENUCTL_GetSel( pRingtone) - IDS_EVENT_EDIT_RINGTONE_OPTION_ALARM_CLOCK];

					MSG_FATAL("***zzg today=%d, m_lEventDay=%d, m_lStartTime=%d***", pme->m_CalMgr.m_lToday, pme->m_CalMgr.m_lEventDay, pme->m_CalMgr.m_lStartTime);

					//Add By zzg 2012_02_01
					if (pme->m_CalMgr.m_lEventDay < pme->m_CalMgr.m_lToday)
					{
						CLOSE_DIALOG(DLGRET_FAILD)
						return TRUE;
					}
					//Add End

                    if(pme->m_sports)
                    {
                        pme->m_CalMgr.m_CurMode     = walkmode[IMENUCTL_GetSel(pMode) - IDS_SPORTS_RUN];
                    }
                    
                    if( pme->m_subStateEventEdit == SUBSTATE_EVENT_EDIT_NEW)
                    {
                        saveEvent( &pme->m_CalMgr);
                    }
                    else
                    {
                        updateEvent( &pme->m_CalMgr);
                    }

                    if(  pme->m_CalMgr.m_dbOperationResult == SUCCESS)
                    {
                        if( pme->m_stateToBackEventEdit == STATE_VIEWDAY &&
                            (pme->m_subStateEventEdit == SUBSTATE_EVENT_EDIT_NEW || changed)
                        )
                        {
                            retrieveMonthEventsFromDB( &pme->m_CalMgr);
                        }
                        OEMOS_Sleep( 200);
                        exitByUser = TRUE;
                        MSG_FATAL("_scheduleapp_event_edit_save_.........",0,0,0);
                        CLOSE_DIALOG( DLGRET_OK)
                    }
                    else
                    {
                        subState = 2;
                        repaint(pme, TRUE);
                    }
                }
                
            }
        }
        return TRUE;

        case EVT_KEY_PRESS:
        {

            if( !inited || subState == 1)
            {
                return TRUE;
            }

            switch( wParam)
            {
                

                case AVK_UP:
                case AVK_DOWN:
                {
                    if( subState == 0)
                    {
                        if( wParam == AVK_UP)
                        {
                            currentItem = (currentItem + 5) % 6;
                        }
                        else
                        {
                            currentItem = (currentItem + 1) % 6;
                        }
                        if(currentItem!=1)
                        {
                        	ITEXTCTL_SetActive( pSubject, FALSE);
                        }
                        else if(currentItem!=2)
                        {
                        	ITEXTCTL_SetActive( pNote, FALSE);
                        }
                        repaint(pme, TRUE);
                    }
                }
                break;
                case AVK_SELECT:
                case AVK_INFO:
                case AVK_0:  //ADD BY yangdecai 2011-04-06
                case AVK_1:
                case AVK_2:
                case AVK_3:
                case AVK_4:
                case AVK_5:
                case AVK_6:
                case AVK_7:
                case AVK_8:
                case AVK_9:
                if(pme->m_sports)
                {
                    if(currentItem == 2 && wParam == AVK_INFO)
                    {
                        repaint(pme, TRUE);
                    }
                    else if(wParam == AVK_SELECT)
                    {
                        repaint(pme, TRUE);
                        goto _scheduleapp_event_edit_save_;
                    }
                }
                else
                {
                    if(currentItem == 2 && wParam == AVK_INFO)
                    {
                        repaint(pme, TRUE);
                    }
                    else if(wParam == AVK_SELECT /*|| currentItem ==3 || currentItem == 4 || currentItem == 5*/)
                    {
                        repaint(pme, TRUE);
                        goto _scheduleapp_event_edit_save_;
                    }

                }
                break;
                
            }
        }
        return TRUE;
    }
    return FALSE;
}

static boolean  dialog_handler_of_state_inputtext( CScheduleApp* pme,
                                               AEEEvent   eCode,
                                               uint16     wParam,
                                               uint32     dwParam
)
{
    AEERect rc = {0};

    static ITextCtl*        pTextControl  = 0;
    static BottomBar_e_Type bottomBarType = 0;

    static AECHAR   theText[MAXTEXT+1]    = {0};
    static boolean  exitByUser          = 0;

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            pTextControl = (ITextCtl*)IDIALOG_GetControl( pme->m_pActiveDlg, IDC_TEXT_INPUT);
            return TRUE;

        case EVT_DIALOG_END:
            MEMSET( theText, 0, sizeof( theText));
            if( pTextControl && !pme->m_closeByPowerKey && !exitByUser)
            {
                if( !ITEXTCTL_GetText( pTextControl, theText, MAXTEXT+1))
                {
                    theText[0] = 0;
                }
            }
            exitByUser   = 0;
            pTextControl = 0;
            return TRUE;

        case EVT_DIALOG_START:
        {
            rc = pme->m_rc;
            rc.dy -= GetBottomBarHeight( pme->m_pDisplay);

            ITEXTCTL_SetRect( pTextControl, &rc);
            ITEXTCTL_SetProperties( pTextControl, TP_MULTILINE|TP_FRAME|TP_FOCUS_NOSEL|
                                     TP_FIXSETRECT|TP_STARKEY_SWITCH|TP_DISPLAY_COUNT|TP_NOUPDATE|TP_GRAPHIC_BG);
            
            if(pme->m_eventEditItem == 0)
            {
                ITEXTCTL_SetMaxSize( pTextControl, MAX_INPUT_SUBJECT);
            }
            else
            {
                ITEXTCTL_SetMaxSize( pTextControl, MAX_INPUT_LOCATION);
            }
            if( theText[0])
            {
                ITEXTCTL_SetText(pTextControl, theText, -1);
            }
            else
            {
                ITEXTCTL_SetText(pTextControl, pme->m_pszEventEditText, -1);
            }

            {
                static uint16 resId[] = { IDS_EVENT_EDIT_TITLE_SUBJECT, IDS_EVENT_EDIT_TITLE_NOTE};
                ITEXTCTL_SetTitle( pTextControl,
                                   AEE_SCHEDULEAPP_RES_FILE,
                                   resId[pme->m_eventEditItem],
                                   0
                               );
            }
            repaint(pme, TRUE);
        }
        return TRUE;

        case EVT_USER_REDRAW:
        {
            if( pTextControl)
            {
            	MSG_FATAL("ITEXTCTL_Redraw......................................,0,0,0",0,0,0);
                ITEXTCTL_Redraw( pTextControl);
            }
        }
        case EVT_KEY_RELEASE:
        {

            if ( pTextControl && ITEXTCTL_GetInputMode(pTextControl,NULL) != AEE_TM_SYMBOLS)
            {

                if( WSTRCMP( ITEXTCTL_GetTextPtr(pTextControl), pme->m_pszEventEditText) == 0)
                {
                	#ifndef FEATURE_ALL_KEY_PAD
                    bottomBarType = WSTRLEN(ITEXTCTL_GetTextPtr(pTextControl)) > 0 ? BTBAR_OK_DELETE : BTBAR_BACK;
                    #else
                    bottomBarType = BTBAR_BACK;
                    #endif
                }
                else
                {
                	#ifndef FEATURE_ALL_KEY_PAD
                    bottomBarType = WSTRLEN(ITEXTCTL_GetTextPtr(pTextControl)) > 0 ? BTBAR_OK_DELETE : BTBAR_OK_BACK;
                    #else
                    bottomBarType = BTBAR_OK_BACK;
                    #endif
                }

                drawBottomBar( bottomBarType);
                IDISPLAY_UpdateEx( pme->m_pDisplay, FALSE);
            }
        }
        return TRUE;

        case EVT_KEY:
        {
            switch (wParam)
            {
                case AVK_SELECT:
                {
                    if( bottomBarType == BTBAR_BACK)
                    {
                        return FALSE;
                    }
                    else if( pTextControl && bottomBarType != BTBAR_BACK_DELETE)
                    {
                    	MSG_FATAL("bottomBarType != BTBAR_BACK_DELETE....................",0,0,0);
                        ITEXTCTL_GetText( pTextControl, pme->m_pszEventEditText, MAXTEXT+1/*WSTRLEN(pme->m_pszEventEditText)*sizeof(AECHAR)*/);
                    }
                    exitByUser = 1;
                    CLOSE_DIALOG(DLGRET_OK)
                }
                return TRUE;

               case AVK_CLR:
               {
                   exitByUser = 1;
                   CLOSE_DIALOG(DLGRET_CANCELED)
               }
               return TRUE;
            }
        }
        break;

    }

    return FALSE;
} // CScheduleApp_HandleSTATE_INPUTTEXT


static boolean  dialog_handler_of_state_setup( CScheduleApp* pme,
                                            AEEEvent   eCode,
                                            uint16     wParam,
                                            uint32     dwParam)
{

    static IControl*   pControls[6]     = {0};
    static ITimeCtl*   pStart           = 0;
    static IMenuCtl*   pAutoDelete      = 0;
    static IMenuCtl*   pSnooze          = 0;

    static int         titleBarHeight      = 0;
    static int         bottomBarHeight     = 0;
    static int         itemNumberPerPage   = 0;
    static int         currentItem         = 0;
    static int         itemHeight          = 0;
    static int         fontHeight          = 0;
    static int         itemSpace           = 0;
    static boolean     inited              = 0;

    static Theme_Param_type    themeParms       = {0};
    static AEERect             rectWindow       = {0};
    static uint16              autoDeleteDay[]  = { 30, 60, 90, 180, 365, 0};
    static uint16              snooze[]         = { 0, 5, 10, 15};

    static uint32   snoozeValue             = 0;
    static uint16   autoDeleteValue         = 0;
    static uint32   startTimeValue          = 0;
    static boolean  exitByUser              = 0;
    static byte     timeFormatType          = 0;
    switch (eCode)
    {

        case EVT_DIALOG_END:
        {
            if( pme->m_closeByPowerKey || exitByUser)
            {
                snoozeValue     = 0;
                autoDeleteValue = 0;
                startTimeValue  = 0;
            }
            else
            {
                snoozeValue     = IMENUCTL_GetSel( pSnooze);
                autoDeleteValue = IMENUCTL_GetSel( pAutoDelete);
                startTimeValue  = ITIMECTL_GetTime( pStart);
            }
            exitByUser = 0;
            pStart     = 0;
        }
        return TRUE;

        case EVT_DIALOG_START:
//        {
//            AEERect rect = { 0};
//            SETAEERECT( &rect, 1-pme->m_rc.dx, 1-pme->m_rc.dy, 0, 0);
//            IDISPLAY_SetClipRect( pme->m_pDisplay, &rect);
//        }
        return TRUE;

        case EVT_DIALOG_INIT:
        {

            // init parms
            Appscom_GetThemeParameters( &themeParms);
            fontHeight          = IDISPLAY_GetFontMetrics( pme->m_pDisplay, AEE_FONT_NORMAL, 0, 0);
            titleBarHeight      = 0;//GetTitleBarHeight( pme->m_pDisplay);
            bottomBarHeight     = GetBottomBarHeight( pme->m_pDisplay);
            itemHeight          = ( fontHeight + 2) * 2 + 2 + 6;
            itemNumberPerPage   = ( pme->m_rc.dy - titleBarHeight - bottomBarHeight) / itemHeight;
            if( !pme->m_bInactive)
            {
                currentItem = 0;
            }
            SETAEERECT( &rectWindow, 0, titleBarHeight, pme->m_rc.dx, pme->m_rc.dy - titleBarHeight - bottomBarHeight);
            itemSpace = ( rectWindow.dy - itemHeight * itemNumberPerPage) / ( itemNumberPerPage + 1);

            IDIALOG_SetProperties( pme->m_pActiveDlg, DLG_NOT_SET_FOCUS_AUTO);
            repaint(pme, TRUE);

            inited = 0;
            //ISHELL_SetTimer( pme->m_pShell, 1000, loadDataTimerCB, pme);
        }
        //return TRUE;

        case EVT_LOAD_DATA:
        {
            int             i           = 0;
            AEEMenuColors   color       = {
                                        MC_SEL_TEXT | MC_SEL_BACK | MC_TEXT | MC_BACK,
                                        RGB_WHITE,
                                        0x84848400,
                                        RGB_WHITE,
                                        RGB_BLACK,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0
                                    };

            extern int OEM_GetConfig( AEEConfigItem i, void * pBuff, int nSize);
            OEM_GetConfig( CFGI_TIME_FORMAT, &timeFormatType, sizeof( byte));
#if defined( AEE_SIMULATOR)
            timeFormatType = OEMNV_TIMEFORM_AMPM;
            //timeFormatType = OEMNV_TIMEFORM_24HR;
#endif

            for (i = 0; i < 6; i ++)
            {
                pControls[i] = IDIALOG_GetControl(pme->m_pActiveDlg, IDC_SETUP_TITLE_REMINDER_SNOOZE + i);
                ICONTROL_SetActive( pControls[i], FALSE);
            }

            for (i = 0; i < 3; i ++)
            {
                ISTATIC_SetProperties( (IStatic*)pControls[i<<1], ST_MIDDLETEXT|ST_TRANSPARENTBACK);
            }

            pSnooze     = (IMenuCtl*)pControls[1];
            pAutoDelete = (IMenuCtl*)pControls[3];
            pStart      = (ITimeCtl*)pControls[5];

            IMENUCTL_SetOemProperties( pSnooze, OEMMP_SWITCHNAVIGATIONKEY | OEMMP_IDF_ALIGN_CENTER);
            IMENUCTL_SetColors( pSnooze, &color);
            IMENUCTL_SetOemProperties( pAutoDelete, OEMMP_SWITCHNAVIGATIONKEY | OEMMP_IDF_ALIGN_CENTER);
            IMENUCTL_SetColors( pAutoDelete, &color);

            for( i = 0; i < 4; i ++) 
            {
                IMENUCTL_AddItem( pSnooze,
                        AEE_SCHEDULEAPP_RES_FILE,
                        IDS_SETUP_SNOOZE_OPTION_OFF + i,
                        IDS_SETUP_SNOOZE_OPTION_OFF + i,
                        0,
                        0
                    );
            }

            for( i = 0; i < 6; i ++)
            {
                IMENUCTL_AddItem( pAutoDelete,
                        AEE_SCHEDULEAPP_RES_FILE,
                        IDS_SETUP_AUTO_DELETE_OPTION_30_DAY + i,
                        IDS_SETUP_AUTO_DELETE_OPTION_30_DAY + i,
                        0,
                        0
                    );
            }

#ifndef FILECFG
            (void) ISHELL_GetPrefs(pme->m_pShell,
                                   AEECLSID_SCHEDULEAPP,
                                   CAL_CFG_VERSION,
                                   &pme->m_CalMgr.m_cfg,
                                   sizeof(CalCfg));
#endif

            if( snoozeValue > 0)
            {
                IMENUCTL_SetSel( pSnooze, snoozeValue);
            }
            else
            {
                for( i = 0; i < 4 && pme->m_CalMgr.m_cfg.nSnooze != snooze[i]; i ++)
                {
                }
                if( i < 4)
                {
                    IMENUCTL_SetSel( pSnooze, IDS_SETUP_SNOOZE_OPTION_OFF + i);
                }
                else
                {
                    IMENUCTL_SetSel( pSnooze, IDS_SETUP_SNOOZE_OPTION_10_MIN);
                }
            }

            if( autoDeleteValue > 0)
            {
                IMENUCTL_SetSel( pAutoDelete, autoDeleteValue);
            }
            else
            {
                for( i = 0; i < 6 && pme->m_CalMgr.m_cfg.nAutoDelete != autoDeleteDay[i]; i ++)
                {
                }
                if( i < 6)
                {
                    IMENUCTL_SetSel( pAutoDelete, IDS_SETUP_AUTO_DELETE_OPTION_30_DAY + i);
                }
                else
                {
                    IMENUCTL_SetSel( pAutoDelete, IDS_SETUP_AUTO_DELETE_OPTION_NEVER);
                }
            }
            ITIMECTL_SetProperties( pStart, TP_NO_SECONDS | TP_AUTOREDRAW);
            ITIMECTL_SetOemProperties( pStart, TP_OEM_COUNTDOWNCTL_EDITABLE|((timeFormatType==OEMNV_TIMEFORM_AMPM)?TP_OEM_COUNTDOWNCTL_12_FORMAT:0));
            ITIMECTL_SetTimeEx( pStart, startTimeValue>0?startTimeValue:(pme->m_CalMgr.m_cfg.wDayStart*60*1000), TRUE);
            IDISPLAY_SetClipRect( pme->m_pDisplay, &pme->m_rc);

            inited = TRUE;
            //ISHELL_SetTimer( pme->m_pShell, 500, redrawDialog, (void*)pme);
            repaint(pme, TRUE);
        }
        return TRUE;

        case EVT_USER_REDRAW:
        {
			
            int     i               = 0;
            int     y               = 0;
            int     x               = 3;
            int     width           = rectWindow.dx - 6;
            int     height          = fontHeight + 2;
            int     itemStart       = ( currentItem / itemNumberPerPage) * itemNumberPerPage;
            int     itemNumber      = 3;
            int     itemEnd         = min( itemNumber, itemStart + itemNumberPerPage);
            AEERect rc              = { 0};

            TitleBar_Param_type     titleBarParms   = {0};
            BottomBar_Param_type    bottomBarParms  = {0};


            if( itemNumberPerPage < itemNumber)
            {
                width -= 6;
            }
            IDISPLAY_SetClipRect( pme->m_pDisplay, &pme->m_rc);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            {
                IImage *pImageBg = ISHELL_LoadResImage(pme->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SCHEDULE_BACKGROUND);

                Appscommon_ResetBackground(pme->m_pDisplay, pImageBg, APPSCOMMON_BG_COLOR, &rectWindow, 0, 0);
                if(pImageBg != NULL)
                {
                    IImage_Release(pImageBg);
                }
            }
#else
            Appscommon_ResetBackgroundEx(pme->m_pDisplay, &rectWindow, TRUE);
#endif
            //IDISPLAY_FillRect( pme->m_pDisplay, &rectWindow, 0);

            // draw title and bottom bar
            titleBarParms.dwAlignFlags  = IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER;
            titleBarParms.nTitleResID   = IDS_SETUP_TITLE;
            STRCPY( titleBarParms.strTitleResFile, AEE_SCHEDULEAPP_RES_FILE);
			#if 0
            DrawTitleBar( pme->m_pDisplay, &titleBarParms);
			#else
			IANNUNCIATOR_SetFieldText(pme->m_pIAnn,(uint16*)titleBarParms.strTitleResFile);
			#endif

            if( currentItem == 2 && timeFormatType == OEMNV_TIMEFORM_AMPM)
            {
                if( ITIMECTL_GetTime( pStart) >= 12*3600000)
                {
                    bottomBarParms.eBBarType = BTBAR_SAVE_AM_BACK;
                }
                else
                {
                    bottomBarParms.eBBarType = BTBAR_SAVE_PM_BACK;
                }
            }
            else
            {
                bottomBarParms.eBBarType = BTBAR_SAVE_BACK;
            }
            DrawBottomBar( pme->m_pDisplay, &bottomBarParms);

            IDISPLAY_SetColor( pme->m_pDisplay, CLR_USER_TEXT, themeParms.textColor);

			if( inited)
            {
                SETAEERECT( &rc, 1-pme->m_rc.dx, 1-pme->m_rc.dy, 0, 0);
                IDISPLAY_SetClipRect( pme->m_pDisplay, &rc);

                // hide invalid controls
                for (i = 0; i < 6; i ++)
                {
                    ICONTROL_SetRect( pControls[i], &rc);
                    ICONTROL_SetActive( pControls[i], FALSE);
                }

                IDISPLAY_SetClipRect( pme->m_pDisplay, &rectWindow);
                y = rectWindow.y + (itemSpace + itemHeight) * ( currentItem - itemStart) + itemSpace + 3 + height;
                SETAEERECT( &rc, x - 1, y, width + 2, height + 2);
                IDISPLAY_DrawRect( pme->m_pDisplay, &rc, themeParms.themeColor, 0, IDF_RECT_FRAME);

				IDISPLAY_SetColor( pme->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);

                // show valid controls
                for( i = itemStart, y = rectWindow.y; i < itemEnd; i ++)
                {
                    y += itemSpace + 3;
                    SETAEERECT( &rc, x, y, width, height);
                    ICONTROL_SetRect( pControls[i<<1], &rc);

                    rc.y += height + 1;
                    if( i < 2)
                    {
                        IDISPLAY_FillRect( pme->m_pDisplay, &rc, RGB_WHITE);
                        drawLeftRightArrows( pme, x, rc.y + (rc.dy - ARROW_WIDTH) / 2, x + width - ARROW_WIDTH, rc.y + (rc.dy - ARROW_WIDTH) / 2);

                        rc.x    += ARROW_WIDTH;
                        rc.dx   -= ARROW_HEIGHT;
                    }
                    ICONTROL_SetRect( pControls[(i<<1)+1], &rc);
                       
                    y += height * 2 + 2 + 3;

                    ICONTROL_Redraw( pControls[i<<1]);
                    ICONTROL_Redraw( pControls[(i<<1)+1]);
                }
                
                IDISPLAY_SetColor( pme->m_pDisplay, CLR_USER_TEXT, themeParms.seltextColor);

                // set the focus to the current item
                ICONTROL_SetActive( pControls[(currentItem<<1)+1], TRUE);

                IDISPLAY_SetClipRect( pme->m_pDisplay, &pme->m_rc);

				
                // draw scroll bar
                if( itemNumberPerPage < itemNumber)
                {
#ifdef FEATURE_SCROLLBAR_USE_STYLE
                    RGBVAL  ScrollbarClr = MAKE_RGB(0xDE, 0xDE, 0xDE),
                                ScrollbarFillClr = MAKE_RGB(0xFF, 0x70, 0x00);
#endif
                    height  = ( rectWindow.dy / itemNumber) * itemNumber;
                    y       = rectWindow.y + ( rectWindow.dy - height) / 2;
                    
#ifdef FEATURE_SCROLLBAR_USE_STYLE
                    SETAEERECT( &rc, pme->m_rc.dx - (SCROLLBAR_WIDTH - 2), y, (SCROLLBAR_WIDTH - 2), height);
                    IDISPLAY_FillRect(pme->m_pDisplay, &rc, ScrollbarClr);
                    rc.dy  = rc.dy / itemNumber;
                    rc.y  += rc.dy * currentItem;
                    IDISPLAY_FillRect(pme->m_pDisplay, &rc, ScrollbarFillClr);
#else
                    SETAEERECT( &rc, pme->m_rc.dx - 1, y, 1, height);
                    IDISPLAY_FillRect( pme->m_pDisplay, &rc, themeParms.textColor);
                    rc.dy  = rc.dy / itemNumber;
                    rc.y  += rc.dy * currentItem;
                    IDISPLAY_FillRect( pme->m_pDisplay, &rc, themeParms.themeColor);
#endif
                }
            }
            else
            {
            }
            IDISPLAY_SetColor( pme->m_pDisplay, CLR_USER_TEXT, 0);

            IDISPLAY_Update( pme->m_pDisplay);			
        }
        return TRUE;

        case EVT_KEY_PRESS:
        {
            if( !inited)
            {
                return TRUE;
            }

            switch( wParam)
            {
                case AVK_SOFT2:
                case AVK_CLR:
                {
                    exitByUser = TRUE;
                    CLOSE_DIALOG(DLGRET_CANCELED)
                }
                break;
                case AVK_SELECT:
                case AVK_INFO:
				{
                    if( wParam == AVK_INFO && currentItem == 2)
                    {
                        repaint(pme, TRUE);
                    }
                    else
                    {                    	
                        pme->m_CalMgr.m_cfg.wDayStart    = ITIMECTL_GetTime( pStart) / 1000 / 60;
                        pme->m_CalMgr.m_cfg.nAutoDelete  = autoDeleteDay[IMENUCTL_GetSel( pAutoDelete) - IDS_SETUP_AUTO_DELETE_OPTION_30_DAY];
                        pme->m_CalMgr.m_cfg.nSnooze      = snooze[IMENUCTL_GetSel( pSnooze) - IDS_SETUP_SNOOZE_OPTION_OFF];
                        CScheduleApp_WriteCFG( pme->m_CalMgr.m_pFileMgr, &pme->m_CalMgr.m_cfg);

                        exitByUser = TRUE;
                        CLOSE_DIALOG( DLGRET_OK)
                    }
                }
                break;

                case AVK_UP:
                case AVK_DOWN:
                {
                    if( wParam == AVK_UP)
                    {
                        currentItem = (currentItem + 2) % 3;
                    }
                    else
                    {
                        currentItem = (currentItem + 1) % 3;
                    }
                    repaint(pme, TRUE);
                }
                break;
            }
        }
        return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//ydc add for LCD touch

				case EVT_PEN_UP:
				{
					
					int i;
					AEEDeviceInfo devinfo;
					int nBarH ;
					AEERect rc;
					int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
					int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
					int xy[][4] = {
							{20,55,156,75},		// Menu1
							{0,38,20,75},		// 左
							{156,55,176,75},	// 右
							{0,102,176,122},	// Menu2
							{0,102,20,122}, 	// 左
							{156,102,176,122},	// 右 
							{0,153,176,173},	// Menu3
							
						};
					nBarH = GetBottomBarHeight(pme->m_pDisplay);
					MEMSET(&devinfo, 0, sizeof(devinfo));
					ISHELL_GetDeviceInfo(pme->m_pShell, &devinfo);
					SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, devinfo.cyScreen);
					if ((wXPos>0)&&(wXPos<devinfo.cxScreen/3)&&(wYPos>rc.y)&&(wYPos<devinfo.cyScreen))
					{
						boolean rt =  ISHELL_PostEvent(pme->m_pShell,AEECLSID_SCHEDULEAPP,EVT_USER,AVK_SELECT,NEVT_KEY_PRESSS);
						return rt;
					}
					else if ((wXPos>(devinfo.cxScreen/3)*2)&&(wXPos<devinfo.cxScreen)&&(wYPos>rc.y)&&(wYPos<devinfo.cyScreen))
					{
						boolean rt =  ISHELL_PostEvent(pme->m_pShell,AEECLSID_SCHEDULEAPP,EVT_USER,AVK_CLR,NEVT_KEY_PRESSS);
						return rt;
					}
					MSG_FATAL("wXPos==========%d,wYPos====%d",wXPos,wYPos,0);
					for( i = 0; i < 7; i ++)
					{
						if( wXPos >= xy[i][0] &&
							wXPos <= xy[i][2] &&
							wYPos >= xy[i][1] &&
							wYPos <= xy[i][3]
						)
						{
							if ( i == 0)
							{
								currentItem = 0;
								
							}
							else if (i == 1) 
							{ 
								int rt = IMENUCTL_HandleEvent(pSnooze,EVT_USER,AVK_LEFT,0);
								return rt;
							}
							else if (i == 2)
							{
								int rt = IMENUCTL_HandleEvent(pSnooze,EVT_USER,AVK_RIGHT,0);
								return rt;
							}
							else if (i == 3)
							{
								currentItem  =1;
							}
							else if (i == 4)
							{
								int rt = IMENUCTL_HandleEvent(pAutoDelete,EVT_USER,AVK_LEFT,0);
								return rt;
							}
							else if (i == 5) 
							{
								int rt = IMENUCTL_HandleEvent(pAutoDelete,EVT_USER,AVK_RIGHT,0);
								return rt;
							}
							else if (i == 6)
							{
								currentItem = 2;
							}
							
						}
					}
					repaint(pme, TRUE);
				}
				return TRUE;
#endif

    }
    return FALSE;
} // dialog_handler_of_state_SETUP


static void initMenuItemWhenViewDay( CCalApp* pme, IMenuCtl* pMenu, int type)
{

    CalEvent*       pEvent      = 0;
    CalEvent*       pEventHead  = 0;
    uint32          nFirstSel   = 0;
    IDBRecord*      pRecord     = 0;
    CalDBRecFields  fields      = {0};
    boolean         haveSeled   = 0;
    boolean         haveEvent   = 0;
    int             i           = 0;
    int             id          = 1;
    int             setupHour   = GET_HOUR( pme->m_cfg.wDayStart);
    int             setupMinute = GET_MINUTE( pme->m_cfg.wDayStart);

    AECHAR          subject[MAXTEXT+1]    = {0};
    AECHAR          location[MAXTEXT+1]   = {0};
    AECHAR          text[160]           = {0};
#if FEATURE_ONE_DB
    if( !openDatabaseIf( pme))
#else
    if( !openDatabaseIf( pme, pme->m_julianCurrentDay.wYear, pme->m_julianCurrentDay.wMonth))
#endif
    {
        return;
    }

    // no current event
    pme->m_pceCurrent = NULL;

    pEvent = pEventHead = pme->m_pMonthEvents[pme->m_julianCurrentDay.wDay - 1];
    nFirstSel = (pEvent == NULL) ? ( GET_HOUR( pme->m_cfg.wDayStart) + 1) :
                                   ( GET_HOUR( GETTIME( pEvent->m_b.dwTime)) + 1);

    for( i = 0; i < 24;)
    {


        if( pEvent && GET_HOUR( GETTIME(pEvent->m_b.dwTime)) == i)
        {
            pRecord     = IDATABASE_GetRecordByID( pme->m_pIDatabase, pEvent->nCurrRecID);
            subject[0]   = 0;
            if( pRecord)
            {

                //把记录保存起来
                MEMSET( &fields, 0, sizeof(CalDBRecFields));
                fields.pszDesc  = subject;
                fields.nDescLen = sizeof(subject) / sizeof(AECHAR);
                fields.pszNote  = location;
                fields.nNoteLen = sizeof(location) / sizeof(AECHAR);

                (void) readRecordFields(pRecord, &fields);
                IDBRECORD_Release(pRecord);
            }
            if((type == 1) || (type == 2))
            {
                CtlAddItem  item        = { 0 };
                uint16      hour        = GET_HOUR( GETTIME(pEvent->m_b.dwTime));
                uint16      minute      = GET_MINUTE( GETTIME(pEvent->m_b.dwTime));

#ifdef FEATURE_APP_WORLDTIME
                Calendar_FormatTime( hour*3600 + minute*60, text, sizeof( text));
#endif
                text[WSTRLEN( text)] = ' ';
                WSTRCAT( text, subject);
                text[WSTRLEN( text)] = ' ';
                WSTRCAT( text, location);
                item.pText          = text;
                item.pszResImage    = AEE_APPSCOMMONRES_IMAGESFILE;
                item.wImage         = IDI_ALARM;
                item.dwData         = (uint32)pEvent;
                item.wItemID        = id ++;
                IMENUCTL_AddItemEx( pMenu, &item);
                debug( ";add item: %S", text);

                if( pEvent->nPermRecID == pme->m_viewDaySelectedEventID ||
                    ( !haveSeled && pme->m_viewDaySelectedEventID == 0)
                )
                {
                    nFirstSel = item.wItemID;
                    haveSeled = TRUE;
                }
            }

            pEvent = pEvent->pNext;
            if( pEvent == pEventHead)
            {
                pEvent = NULL;
            }

            haveEvent = TRUE;
        }
        else if( haveEvent)
        {
            i ++;
            haveEvent = FALSE;
        }
        else
        {
            uint32 seconds = i*3600 + (i==setupHour?setupMinute:0)*60;
#ifdef FEATURE_APP_WORLDTIME			
            Calendar_FormatTime( seconds, subject, sizeof( subject));
#endif
            IMENUCTL_AddItem( pMenu, 0, 0, id ++, subject, 0);
            debug( ";%d, %S, no event", seconds, subject);

            i ++;
        }
    }

    if( !haveSeled && pme->m_julianCurrentDay.wSecond > 0)
    {
        nFirstSel = pme->m_julianCurrentDay.wSecond;
        
        IMENUCTL_SetBottomBarType( pMenu, BTBAR_ADD_BACK);
    }

    IMENUCTL_SetSel( pMenu, nFirstSel);
}

static void initDataForEditEventAtViewDayState( CCalApp *pme, IMenuCtl* pMenu)
{

    uint16      itemId  = IMENUCTL_GetSel( pMenu);
    IMENUCTL_GetItemData( pMenu, itemId, (uint32*)&pme->m_pceCurrent);
    //debug( ";initDataForEditEventAtViewDayState, itemId = %d", itemId);
    //debug( ";initDataForEditEventAtViewDayState, date = 0x%x", pme->m_pceCurrent);

    if( pme->m_pceCurrent)
    {
        pme->m_viewDaySelectedEventID   = pme->m_pceCurrent->nPermRecID;
        pme->m_julianCurrentDay.wSecond = pme->m_pceCurrent->nPermRecID;

        pme->m_julianCurrentDay.wHour   = GET_HOUR( GETTIME( pme->m_pceCurrent->m_b.dwTime));
        pme->m_julianCurrentDay.wMinute = GET_MINUTE( GETTIME( pme->m_pceCurrent->m_b.dwTime));
    }
    else
    {
        CtlAddItem item = {0};

        if( IMENUCTL_GetItem( pMenu, itemId, &item))
        {
            pme->m_julianCurrentDay.wHour   = num( item.pText[0])*10 + num( item.pText[1]);
            pme->m_julianCurrentDay.wMinute = num( item.pText[3])*10 + num( item.pText[4]);

            {
                byte timeFormatType = 0;
#if defined( AEE_SIMULATOR)
                timeFormatType = OEMNV_TIMEFORM_AMPM;
#else
                OEM_GetConfig( CFGI_TIME_FORMAT, &timeFormatType, sizeof( byte));
#endif
                if( timeFormatType == OEMNV_TIMEFORM_AMPM)
                {
                    if( pme->m_julianCurrentDay.wHour == 12)
                    {
                        pme->m_julianCurrentDay.wHour = 0;
                    }
                    if( item.pText[6] == 'P' && item.pText[7] == 'M')
                    {
                        pme->m_julianCurrentDay.wHour += 12;
                    }
                }
            }
        }

        pme->m_julianCurrentDay.wSecond = itemId;
        pme->m_viewDaySelectedEventID   = 0;
    }
}

static boolean dialog_handler_of_state_viewday(CScheduleApp *pme, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    static IMenuCtl*   pMenu;
    static boolean     inited   = 0;
    // 1 - deleting
    static int         subState = 0;
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            pMenu = (IMenuCtl*)IDIALOG_GetControl( pme->m_pActiveDlg, IDC_MENU_SCH);
            subState = 0;
            return TRUE;
        case EVT_DIALOG_END:
            pMenu = 0;
            return TRUE;

        case EVT_DIALOG_START:
        {
            IMENUCTL_SetProperties(pMenu, MP_WRAPSCROLL);
			#if 0
            (void)IMENUCTL_SetTitle(pMenu, 0, 0, pme->m_pVDTitle);
			#else
		    {
				IANNUNCIATOR_SetFieldText(pme->m_pIAnn,pme->m_pVDTitle);
		    }
			#endif

            IMENUCTL_SetBottomBarType( pMenu, BTBAR_OPTION_BACK);
            IMENUCTL_SetRect(pMenu, &pme->m_rc);
            IMENUCTL_SetOemProperties( pMenu, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY | OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SCHEDULE_BACKGROUND);
#endif
            {
                int     i           = 0;
                AECHAR  time[8]     = {0};
                AECHAR  format[]    = {'%','0','2','d',':','%','0','2','d',0};
                int     setupHour   = GET_HOUR( pme->m_CalMgr.m_cfg.wDayStart);
                int     setupMinute = GET_MINUTE( pme->m_CalMgr.m_cfg.wDayStart);

                for( i = 0; i < 24; i ++)
                {
                    WSPRINTF( time, 16, format, i, i == setupHour ? setupMinute : 0);
                    IMENUCTL_AddItem( pMenu, 0, 0, i + 1, time, 0);
                }
            }

            if( pme->m_subStateViewDay == SUBSTATE_NONE)
            {
                inited = 0;
                ISHELL_SetTimer( pme->m_pShell, 500, loadDataTimerCB, pme);
            }
            else
            {
                inited = 1;
            }
            repaint(pme, TRUE);
        }
        return TRUE;

        case EVT_LOAD_DATA:
        {
            int type;
            if(pme->m_sports)
            {
                type = 2;
            }
            else
            {
                type = 1;
            }
            IMENUCTL_DeleteAll( pMenu);
            initMenuItemWhenViewDay( &pme->m_CalMgr, pMenu, type);

            inited = TRUE;
            repaint(pme, TRUE);
        }
        return TRUE;

        case EVT_USER_REDRAW:
        {
            IMENUCTL_SetActive( pMenu, pme->m_subStateViewDay == SUBSTATE_NONE && subState == 0 && inited);
            (void)IMENUCTL_Redraw(pMenu);

            if( pme->m_subStateViewDay != SUBSTATE_NONE)
            {
                if( subState == 1)
                {
                    drawPrompt( pme, IDS_DELETING);
                }
                else if( subState == 2)
                {
                    drawPrompt( pme, IDS_DONE);
                }
                else
                {
                    IStatic* pStatic = (IStatic*)IDIALOG_GetControl(pme->m_pActiveDlg, IDC_VIEWDAY_STATIC_PROMPT);

                    switch( pme->m_subStateViewDay)
                    {
                        case SUBSTATE_DELETE_TODAY_CONFIRM:
                            drawModalDialog( pme->m_pDisplay, pStatic, IDS_OPTION_DELETE_TODAY_CONFIRM, TRUE);
                            break;
                        case SUBSTATE_DELETE_MONTH_CONFIRM:
                            drawModalDialog( pme->m_pDisplay, pStatic, IDS_OPTION_DELETE_MONTH_CONFIRM, TRUE);
                            break;
                        case SUBSTATE_DELETE_ALL_CONFIRM:
                            drawModalDialog( pme->m_pDisplay, pStatic, IDS_OPTION_DELETE_ALL_CONFIRM, TRUE);
                            break;
                        case SUBSTATE_VIEWDAY_DELETE_CONFIRM:
                            drawModalDialog( pme->m_pDisplay, pStatic, IDS_OPTION_VIEWDAY_DELETE_CONFIRM, TRUE);
                            break;
                    }
                }
            }

            if( !inited)
            {
                //drawLoading( pme);
            }

            IDISPLAY_UpdateEx( pme->m_pDisplay, FALSE);
        }
        return TRUE;
        case EVT_KEY:
        {
            if( !inited)
            {
                return TRUE;
            }

            if( subState == 1)
            {
                return TRUE;
            }

            if( pme->m_subStateViewDay != SUBSTATE_NONE)
            {
                if( wParam == AVK_CLR || wParam == AVK_SELECT)
                {
                    if( wParam == AVK_SELECT)
                    {

                        subState = 1;
                        repaint(pme, FALSE);

                        switch( pme->m_subStateViewDay)
                        {
                            case SUBSTATE_DELETE_ALL_CONFIRM:
                                deleteAllEventsFromDB( &pme->m_CalMgr);
                                break;
                            case SUBSTATE_DELETE_TODAY_CONFIRM:
                                deleteEventsOfTodayFromDB( &pme->m_CalMgr, pme->m_CalMgr.m_julianCurrentDay.wDay);
                                break;
                            case SUBSTATE_DELETE_MONTH_CONFIRM:
                                deleteEventsOfThisMonthFromDB( &pme->m_CalMgr);
                                break;
                            case SUBSTATE_VIEWDAY_DELETE_CONFIRM:
                                deleteEventFromDB( &pme->m_CalMgr, pme->m_CalMgr.m_pceCurrent->nCurrRecID);
                                break;
                        }
                        
                        subState = 2;
                        repaint(pme, FALSE);
                        
                        if( pme->m_CalMgr.m_dwThisMonthEventsNumber > 0)
                        {
                            retrieveMonthEventsFromDB( &pme->m_CalMgr);
                        }

                        subState = 0;
                    }
                    pme->m_subStateViewDay = SUBSTATE_NONE;
                    repaint(pme, TRUE);

                    inited = 0;
                    ISHELL_SetTimer( pme->m_pShell, 500, loadDataTimerCB, pme);
                }
                return TRUE;
            }

            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

#if defined( AEE_SIMULATOR)
                case AVK_SOFT1:
                {
                    wParam = IMENUCTL_GetSel( pMenu);
                    goto state_viewday_evt_command;
                }
                return TRUE;
#endif

                case AVK_INFO:
                {
                    pme->m_stateToBackEventEdit = STATE_VIEWDAY;
                    initDataForEditEventAtViewDayState( &pme->m_CalMgr, pMenu);
                    initDataForEditEventAtOptionState( pme, pme->m_CalMgr.m_pceCurrent == 0);
                    CLOSE_DIALOG( DLGRET_VIEWDAY_EDIT_EVENT)
                }
                return TRUE;

            }
        }
        break;

        case EVT_COMMAND:
        {
#if defined( AEE_SIMULATOR)
state_viewday_evt_command:
#endif

            if( !inited)
            {
                return TRUE;
            }
            initDataForEditEventAtViewDayState( &pme->m_CalMgr, pMenu);
            if(pme->m_CalMgr.m_pceCurrent != NULL)
            {
                // 没有标记就直接添加
                pme->m_subStateOption    = SUBSTATE_OPTION_VIEWDAY;
                pme->m_stateToBackOption = STATE_VIEWDAY;
                CLOSE_DIALOG( DLGRET_OK)
            }
            else
            {
                pme->m_stateToBackEventEdit = STATE_VIEWDAY;
                initDataForEditEventAtOptionState( pme, pme->m_CalMgr.m_pceCurrent == 0);
                CLOSE_DIALOG( DLGRET_VIEWDAY_EDIT_EVENT)
            }
        }
        return TRUE;

        case EVT_CTL_SEL_CHANGED:
        {
            if( dwParam == (uint32)pMenu)
            {
                pme->m_CalMgr.m_julianCurrentDay.wSecond = wParam;
                initDataForEditEventAtViewDayState( &pme->m_CalMgr, pMenu);
                if(pme->m_CalMgr.m_pceCurrent == NULL)
                {
                    IMENUCTL_SetBottomBarType( pMenu, BTBAR_ADD_BACK);
                }
                else
                {
                    IMENUCTL_SetBottomBarType( pMenu, BTBAR_OPTION_BACK);
                }
                IMENUCTL_Redraw(pMenu);
            }
        }
        return TRUE;
    }
    return FALSE;
} // CScheduleApp_HandleViewDayDlgEvent


static boolean readRecordFields( IDBRecord * prec, CalDBRecFields * pf)
{
    boolean bValid = TRUE;
    AEEDBFieldType fType;
    AEEDBFieldName dbfname;
    uint16 len;
    pf->nPermRecID = 0;
    pf->nCurrRecID = 0;
    MEMSET(&pf->b, 0, sizeof(CalBaseFields));
    if (pf->pszDesc)
        *pf->pszDesc = 0;

    if (pf->pszNote)
        *pf->pszNote = 0;


    IDBRECORD_Reset(prec);

    for (;;)
    {
        fType = IDBRECORD_NextField(prec, &dbfname, &len);

        if (fType == AEEDB_FT_NONE)
        {
            break;
        }

        switch (((int) dbfname)& 0xff)
        {
            case FLD_ALARM:
                if (fType == AEEDB_FT_BINARY)
                {
                    CalBaseFields *pb = (CalBaseFields *)(void *)IDBRECORD_GetField(prec, &dbfname, &fType, &len);
                    //debug( ";read field, FLD_ALARM, %d, %d, %d, %d", pb->dwTime, pb->wDuration, pb->wAlarmTime, pb->alertst);
                    if (pb && len == sizeof(CalBaseFields))
                    {
                        MEMCPY(&pf->b, pb, sizeof(CalBaseFields));
                    }
                }
                break;

            case FLD_DESC:
                if (pf->pszDesc && fType == AEEDB_FT_STRING)
                {
                    WSTRNCOPY(pf->pszDesc, pf->nDescLen, IDBRECORD_GetFieldString(prec));
                    //debug( ";read field, FLD_DESC, %S", pf->pszDesc);
                }
                break;

            case FLD_NOTE:
                if (pf->pszNote && fType == AEEDB_FT_STRING)
                {
                    WSTRNCOPY(pf->pszNote, pf->nNoteLen, IDBRECORD_GetFieldString(prec));
                    //debug( ";read field, FLD_NOTE, %S", pf->pszNote);
                }
                break;

            case FLD_ID:
                if (!IDBRECORD_GetFieldWord(prec, &pf->nPermRecID))
                {
                    //debug( ";read field FLD_ID failed");
                    bValid = FALSE;
                }
                //debug( ";read field, FLD_ID, %d", pf->nPermRecID);
                break;
        }
    }

    if (!pf->b.dwTime)
    {
        //debug( ";read field failed, start time is %d", pf->b.dwTime);
        (void) IDBRECORD_Remove(prec);
        bValid = FALSE;
    }

    if (0 == pf->nPermRecID)
    {
        //debug( ";read field failed, nPermRecID is 0");
        bValid = FALSE;
    }
    else
    {
        pf->nCurrRecID = IDBRECORD_GetID(prec);
    }
    return bValid;
}

static void updateEvent( CCalApp* pme)
{

   //debug( ";-------------------------------------------------------------------------");
   //debug( ";updateEvent, %02d:%02d", GET_HOUR(GETTIME( pme->m_lStartTime)), GET_MINUTE(GETTIME( pme->m_lStartTime)));

   pme->m_dbOperationResult = saveEventToDB(pme, FALSE);
   //debug( ";-------------------------------------------------------------------------");
}

static void saveEvent( CCalApp* pme)
{

   //debug( ";-------------------------------------------------------------------------");
   //debug( ";saveEvent, %02d:%02d", GET_HOUR(GETTIME( pme->m_lStartTime)), GET_MINUTE(GETTIME( pme->m_lStartTime)));

   pme->m_dbOperationResult = saveEventToDB(pme, TRUE);
   //debug( ";-------------------------------------------------------------------------");
}

static int saveEventToDB( CCalApp* pme, boolean newEvent)
{
    CalBaseFields   baseFields      = {0};
    IDBRecord*      precord         = 0;
    uint16          permId          = pme->m_nNextPermRecID;
    int             result          = 0;

#if FEATURE_ONE_DB
    openDatabaseIf( pme);
#else
    openDatabaseIf( pme, pme->m_julianCurrentDay.wYear, pme->m_julianCurrentDay.wMonth);
    if( newEvent)
    {
        pme->m_nNextPermRecID   = pme->m_cfg.maxPermId;
        permId                  = pme->m_nNextPermRecID;

        pme->m_cfg.maxPermId ++;
        pme->m_nNextPermRecID ++;
        CScheduleApp_WriteCFG( pme->m_pFileMgr, &pme->m_cfg);
    }
#endif

    baseFields.wDuration     = (uint16)pme->m_lDuration;
    baseFields.dwTime        = MAKECOMPTIME(pme->m_lEventDay,(uint32)pme->m_lStartTime);
    baseFields.wAlarmTime    = pme->m_nAlarmTime;
    baseFields.alertst       = pme->m_AlertState;

    if( !newEvent)
    {
        precord = IDATABASE_GetRecordByID( pme->m_pIDatabase, pme->m_pceCurrent->nCurrRecID);

        if( precord == NULL)
        {
            return -1000;
        }

        permId = pme->m_pceCurrent->nPermRecID;
    }

    result = updateDBRecord(pme,
                    precord,
                    permId,
                    pme->m_szEventDes,
                    pme->m_szNote,
                    &baseFields
                );

    return result;
}


static int updateDBRecord( CCalApp     * pme,
                           IDBRecord     *pidb,
                           uint16         permRecId,
                           const AECHAR  *pszDescription,
                           const AECHAR  *pszNote,
                           CalBaseFields *pb
)
{
    AEEDBField  arFields[NUMFLDS]   = {0};
    int         nFields             = 0;

    initRecordField( &arFields[nFields++], AEEDB_FT_WORD, FLD_ID, &permRecId, sizeof(permRecId));
    //debug( ";update record, FLD_ID, %d, len %d", permRecId, sizeof(permRecId));

    initRecordField( &arFields[nFields++], AEEDB_FT_BINARY, FLD_ALARM, (void*)pb, sizeof(CalBaseFields));
    //debug( ";update record, FLD_ALARM, %d(%02d:%02d), %d, %d, %d, len %d", pb->dwTime, GET_HOUR(GETTIME( pb->dwTime)), GET_MINUTE(GETTIME( pb->dwTime)), pb->wDuration, pb->wAlarmTime, pb->alertst, sizeof(CalBaseFields));

    if (pszDescription && *pszDescription)
    { //Description可以为空
        initRecordField( &arFields[nFields++], AEEDB_FT_STRING, FLD_DESC, (void*)pszDescription, WSTRSIZE( pszDescription));
        //debug( ";update record, FLD_DESC, %S", pszDescription);
    }

    if (pszNote && *pszNote)
    { //Note可以为空
        initRecordField( &arFields[nFields++], AEEDB_FT_STRING, FLD_NOTE, (void*)pszNote, WSTRSIZE( pszNote));
        //debug( ";update record, FLD_NOTE, %S", pszNote);
    }

    if ( !pidb)
    { //说明是新增记录
        pidb = IDATABASE_CreateRecord(pme->m_pIDatabase, arFields, nFields);
        if( !pidb)
        {
            //debug( ";create new record failed");
            return -2000;
        }
        //debug( ";create new record ok");

    }
    else
    {
        int result = IDBRECORD_Update(pidb, arFields, nFields);
        if( result != SUCCESS)
        {
            //debug( ";update record failed");
            IDBRECORD_Release(pidb);
            return result;
        }
        //debug( ";update record ok");
    }

    if( pidb)
    {
        IDBRECORD_Release( pidb);
    }
    return SUCCESS;
}

static void initRecordField(AEEDBField*         pFields,
                            AEEDBFieldType      type,
                            RecordFieldTypeEnum name,
                            void*               pBuff,
                            uint16              nSize
)
{

   pFields->fType    = type;
   pFields->fName    = name;
   pFields->wDataLen = nSize;
   pFields->pBuffer  = pBuff;
}


#define SET_EXPIREDAPPOINTMENT_ARRAYSIZE( pme, x)     (uint32)((pme)->m_expiredAppointment[0]) = (uint32)((((uint32)(pme)->m_expiredAppointment[0])&0x0000ffff)|(((x)<<16)&0xffff0000))
#define SET_EXPIREDAPPOINTMENT_NUMBER( pme, x)        (uint32)((pme)->m_expiredAppointment[0]) = (uint32)((((uint32)(pme)->m_expiredAppointment[0])&0xffff0000)|((x)&0x0000ffff))
#define GET_EXPIREDAPPOINTMENT_ARRAYSIZE( pme)        ((uint16)((((uint32)(((pme)->m_expiredAppointment[0])))>>16)&0xffff))
#define GET_EXPIREDAPPOINTMENT_NUMBER( pme)           ((uint16)(((uint32)(((pme)->m_expiredAppointment[0])))&0xffff))

static void releaseExpiredAppointmentArrayIf( CCalApp* pme)
{
    if( pme->m_expiredAppointment != NULL)
    {
        int i   = 0;
        int end = GET_EXPIREDAPPOINTMENT_ARRAYSIZE( pme);

        for( i = 1; i <= end; i ++)
        {
            FREEIF( pme->m_expiredAppointment[i]);
        }

        FREEIF( pme->m_expiredAppointment);
        pme->m_expiredAppointment = NULL;
    }
}

static void creatExpiredAppointmentArray( CCalApp* pme, int num, int offset)
{
    offset ++;

    pme->m_expiredAppointment = (Appointment**)MALLOC( sizeof(Appointment*) * (num + 1));
    if( pme->m_expiredAppointment)
    {

        int i = 0;
        for( i = offset; i <= num; i ++)
        {
            pme->m_expiredAppointment[i] = (Appointment*)MALLOC( sizeof(Appointment));
            if( pme->m_expiredAppointment[i] == NULL)
            {
                releaseExpiredAppointmentArrayIf( pme);
                break;
            }
        }

        if( pme->m_expiredAppointment)
        {
            SET_EXPIREDAPPOINTMENT_ARRAYSIZE( pme, num);
        }
    }
}

static void creatExpiredAppointmentArrayIf( CCalApp* pme)
{
    if( pme->m_expiredAppointment == NULL)
    {
        creatExpiredAppointmentArray( pme, 8, 0);
    }
    else if( GET_EXPIREDAPPOINTMENT_NUMBER( pme) >= GET_EXPIREDAPPOINTMENT_ARRAYSIZE( pme))
    {
        int             i       = 0;
        int             oldSize = GET_EXPIREDAPPOINTMENT_ARRAYSIZE( pme);
        Appointment**   pOld    = pme->m_expiredAppointment;

        creatExpiredAppointmentArray( pme, oldSize + 8, oldSize);
        if( pme->m_expiredAppointment)
        {
            for( i = 1; i <= oldSize; i ++)
            {
                pme->m_expiredAppointment[i] = pOld[i];
            }
            SET_EXPIREDAPPOINTMENT_ARRAYSIZE( pme, oldSize + 8);
            FREEIF( pOld);
        }
        else
        {
            pme->m_expiredAppointment = pOld;
        }
    }
}

static Appointment* getNextAvailableAppointment( CCalApp* pme)
{

    creatExpiredAppointmentArrayIf( pme);

    if( pme->m_expiredAppointment)
    {
        int i = 0;

        i = GET_EXPIREDAPPOINTMENT_NUMBER( pme) + 1;
        SET_EXPIREDAPPOINTMENT_NUMBER( pme, i);
        return pme->m_expiredAppointment[i];
    }
    else
    {
        return 0;
    }
}

static void callbackTheNextAvailableAppointment( CCalApp* pme)
{

    if( pme->m_expiredAppointment)
    {
        MEMSET( pme->m_expiredAppointment[GET_EXPIREDAPPOINTMENT_NUMBER( pme)], 0, sizeof(Appointment));
        SET_EXPIREDAPPOINTMENT_NUMBER( pme, GET_EXPIREDAPPOINTMENT_NUMBER( pme) - 1);
    }
}

static void clearAllDoneAppointment( CCalApp* pme)
{
	MSG_FATAL("pme->m_expiredAppointment===========%d",pme->m_expiredAppointment,0,0);
    if( pme->m_expiredAppointment)
    {
        int         j       = 0;
        int         i       = 0;
        int         end     = GET_EXPIREDAPPOINTMENT_NUMBER( pme);
        int         num     = end;
        IDBRecord*  precord = 0;

        MSG_FATAL("end===========%d",end,0,0);
        for( i = 1; i <= end; i ++)
        {
            if( pme->m_expiredAppointment[i]->status == DONE)
            {
#if !FEATURE_ONE_DB
                closeDatabaseIf( pme);
                openDatabaseIf( pme,
                                pme->m_expiredAppointment[i]->julian.wYear,
                                pme->m_expiredAppointment[i]->julian.wMonth
                            );
#endif

                precord = IDATABASE_GetRecordByID( pme->m_pIDatabase, pme->m_expiredAppointment[i]->dbId);
                if( precord)
                {
                    pme->m_expiredAppointment[i]->baseFields.wAlarmTime = 65535;
                    updateDBRecord( pme,
                                precord,
                                pme->m_expiredAppointment[i]->permId,
                                pme->m_expiredAppointment[i]->subject,
                                pme->m_expiredAppointment[i]->location,
                                &pme->m_expiredAppointment[i]->baseFields
                            );
                }
                MSG_FATAL("IAlarm_CancelAlarm...........................",0,0,0);
                IAlarm_CancelAlarm( pme->m_pIAlarm,
                        AEECLSID_SCHEDULEAPP,
                        pme->m_expiredAppointment[i]->permId
                    );


                FREEIF( pme->m_expiredAppointment[i]);
                pme->m_expiredAppointment[i] = NULL;

                num --;
            }
        }

        SET_EXPIREDAPPOINTMENT_NUMBER( pme, num);
        if( num == end || num == 0)
        {
            return;
        }

        for( i = 1; i <= end; i ++)
        {
            if( pme->m_expiredAppointment[i] != NULL)
            {
                j = i + 1;

                for( ; j <= end; j ++)
                {
                    if( pme->m_expiredAppointment[j] != NULL)
                    {
                        pme->m_expiredAppointment[i] = pme->m_expiredAppointment[j];
                        pme->m_expiredAppointment[j] = NULL;
                    }
                }
            }
        }

    }
}

static int getAppointmentByPermId( CCalApp *pme, uint16 permId, CalDBRecFields* pfields, Appointment** pResult)
{

    Appointment*    pappointment   = 0;
    IDBRecord*      precord        = 0;
    uint16          dbId           = AEE_DB_RECID_NULL;


    *pResult = 0;

    if( pme->m_expiredAppointment && GET_EXPIREDAPPOINTMENT_NUMBER( pme) > 0)
    {

        int i   = 0;
        int end = GET_EXPIREDAPPOINTMENT_NUMBER( pme);
        for( i = 1; i <= end; i ++)
        {
            pappointment = *(pme->m_expiredAppointment + i);

#if 0
            if( (precord = IDATABASE_GetRecordByID( pme->m_pIDatabase, pappointment->dbId)))
            {
                IDBRECORD_Release( precord);
            }
            else
            {
                debug( ";not existed in db, remove it from list");
                FREEIF( pme->m_expiredAppointment[i]);
                for( ; i < end; i ++)
                {
                    pme->m_expiredAppointment[i] = pme->m_expiredAppointment[i+1];
                }
                end --;
                setExpiredAppointmentNumber( pme, end);
                continue;
            }
#endif

            if( pappointment->permId == permId)
            {
                pfields->nPermRecID = permId;
                pfields->b          = pappointment->baseFields;

                *pResult = pappointment;
                return 1;
            }
        }
    }

    IDATABASE_Reset( pme->m_pIDatabase);
    pappointment = getNextAvailableAppointment( pme);
    if( !pappointment)
    {
        return -1;
    }

    while( 1)
    {
        precord = IDATABASE_GetNextRecord( pme->m_pIDatabase);
        if( NULL == precord)
        {
            break;
        }

        MEMSET( pfields, 0, sizeof( CalDBRecFields));
        pfields->pszDesc  = pappointment->subject;
        pfields->nDescLen = sizeof( pappointment->subject);
        pfields->pszNote  = pappointment->location;
        pfields->nNoteLen = sizeof( pappointment->location);

        if( readRecordFields( precord, pfields))
        {
            dbId = IDBRECORD_GetID( precord);
            IDBRECORD_Release( precord);

            if( pfields->nPermRecID == permId)
            {
                pappointment->permId        = permId;
                pappointment->dbId          = dbId;
                pappointment->baseFields    = pfields->b;
                *pResult = pappointment;
                return 2;
            }
        }
    }

    callbackTheNextAvailableAppointment( pme);
    return -2;
}

int16 g_nCurEvtStyle = 0;//是sport 事件还是日程表事件
#ifdef FEATURE_SPORTS_APP
int16 g_tempCurEvtFlag = 0;//如果在alert 界面按clr 键取消的话，直接退到前一个界面
boolean g_gobacktosportflag = FALSE;//直接退到sport main 界面这里是让他直接从main 界面进入到pedometer 的标示位
boolean g_otherappflag = FALSE;//如果在其他模块训练计划响铃clr 不能正常跳回到当前模块的问题
#endif
void Cal_HandleAlarm(CScheduleApp* pme, uint16 permId)
{

    CCalApp*        pCalMgr         = &pme->m_CalMgr;
    uint32          lAlarmTime      = 0;
    CalDBRecFields  fields          = {0};
    int             result          = EFAILED;
    Appointment*    pappointment    = 0;
    boolean         openDB          = TRUE;



    pCalMgr->m_wPendingAlarmPermID = EMPTY_ALARM_ID;

#if FEATURE_ONE_DB
    if( !( openDB = openDatabaseIf( pCalMgr)) ||
        ( result = getAppointmentByPermId( pCalMgr, permId, &fields, &pappointment)) < 0
    )
    {

        if( !openDB || result == -1)
        {
            registerEvent( pCalMgr, permId, 30);
        }
        return;
    }
#else
    {
        int offset[] = { 0, 1, -1};
        int year     = 0;
        int month    = 0;
        int year1    = 0;
        int month1   = 0;
        int i        = 0;

        GETJULIANDATE( GETTIMESECONDS(), &pCalMgr->m_julianCurrentDay);
        year  = pCalMgr->m_julianCurrentDay.wYear;
        month = pCalMgr->m_julianCurrentDay.wMonth;

        while( i < 3)
        {

            year1  = year;
            month1 = month + offset[i];
            if( month1 < 0)
            {
                month1 = 12;
                year1 --;
            }
            else if( month1 > 12)
            {
                month1 = 1;
                year1 ++;
            }
            i ++;

            closeDatabaseIf( pCalMgr);
            if( !( openDB = openDatabaseIf( pCalMgr, year1, month1)) ||
                ( result = getAppointmentByPermId( pCalMgr, permId, &fields, &pappointment)) < 0
            )
            {
                continue;
            }
            else
            {
                break;
            }
        }

        if( !pappointment)
        {

            if( !openDB || result == -1)
            {
                registerEvent( pCalMgr, permId, 30);
            }
            return;
        }
    }
#endif

    result = registerEventIf( &pme->m_CalMgr, &fields, &pappointment->julian, &lAlarmTime, TRUE);
    pappointment->status = result;
    if( result == 2)
    {

        pCalMgr->m_wPendingAlarmPermID = permId;
        if( ISHELL_ActiveApplet(pCalMgr->m_pIShell) == AEECLSID_SCHEDULEAPP)
        {
            if (pme->m_eCurState == STATE_SHOWALERT)
            {
                repaint(pme, TRUE);
            }
            else
            {
                if( pme->m_eCurState == STATE_VIEWEVENT)
                {
                    if( pme->m_stateToBackViewEvent == STATE_VIEWDAY)
                    {
                        pme->m_stateToBackShowAlert = STATE_VIEWDAY;
                    }
                }
                else if( pme->m_eCurState == STATE_OPTION)
                {
                    FSMState backTo[] = { STATE_VIEWMONTH, STATE_VIEWDAY};
                    pme->m_stateToBackShowAlert = backTo[pme->m_subStateOption - SUBSTATE_OPTION_VIEWMONTH];
                }
                else
                {
                    pme->m_stateToBackShowAlert = pme->m_eCurState;
                }
				MSG_FATAL("CLOSE_DIALOG( DLGRET_TO_SHOWALERT)............",0,0,0);
                CLOSE_DIALOG( DLGRET_TO_SHOWALERT)
            }
        }
        else
        {
            if( pme->m_bStartAppletCommandQueued)
            {
            }
            else
            {
                pme->m_bStartAppletCommandQueued = TRUE;
                #ifdef FEATURE_SPORTS_APP
                g_otherappflag = TRUE;
                #endif
                ISHELL_StartApplet(pCalMgr->m_pIShell, AEECLSID_SCHEDULEAPP);
            }
        }
    }

}


#ifdef FEATURE_SPORTS_APP
boolean g_sport_snooze_back_flag = FALSE;//训练弹出框时间到了之后处理表示
#endif
static boolean  dialog_handler_of_state_showalert( CScheduleApp* pme,
                                              AEEEvent   eCode,
                                              uint16     wParam,
                                              uint32     dwParam
)
{

    //static IMenuCtl* pMenu                = 0;
    static byte      keyBeepVolumeSetting = 0;
    static uint16    theSel               = 0;

	MSG_FATAL("***zzg dialog_handler_of_state_showalert eCode=%x, wParam=%x, dwParam=%x***", eCode, wParam, dwParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
        {
/*
            pMenu = (IMenuCtl*)IDIALOG_GetControl(pme->m_pActiveDlg, IDC_SHOWALERT_MENU);
            IMENUCTL_SetOemProperties( pMenu, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY);
            IMENUCTL_SetTitle( pMenu, AEE_SCHEDULEAPP_RES_FILE, IDS_APP, 0);
            IMENUCTL_SetBottomBarType( pMenu, BTBAR_SNOOZE_DONE);
*/

            return TRUE;
        }

        case EVT_DIALOG_START:
        {
#if !defined( AEE_SIMULATOR)
            byte mute = OEMSOUND_MUTE_VOL;
            OEM_GetConfig( CFGI_BEEP_VOL, &keyBeepVolumeSetting, sizeof(byte));
            OEM_SetConfig( CFGI_BEEP_VOL, &mute, sizeof(byte));
#endif

            repaint(pme, TRUE);
        }
        return TRUE;

        case EVT_DIALOG_END:
        {
            MSG_FATAL("***zzg scheduleapp EVT_DIALOG_END***", 0, 0, 0);
            
            pme->m_CalMgr.m_wPendingAlarmPermID = EMPTY_ALARM_ID;  
            if( !pme->m_bshowEventDetail && !pme->m_bSuspended)
            {
                SET_EXPIREDAPPOINTMENT_NUMBER( &pme->m_CalMgr, 0);
                theSel = 0;
            }
            else
            {
                //theSel = IMENUCTL_GetSel( pMenu);
            }
            ISHELL_CancelTimer( pme->m_pShell, 0, &(pme->m_CalMgr));
            ISHELL_SendEvent(pme->m_pShell,
                             AEECLSID_APP_FMRADIO,
                             EVT_ALARM,
                             FALSE,
                             0
                            );
            IALERT_StopRingerAlert(pme->m_CalMgr.m_alert);
            IALERT_StopMp3Alert(pme->m_CalMgr.m_alert);
            ScheduleAppNotifyMP3PlayerAlertEvent(&pme->m_CalMgr,FALSE);
            if(!pme->m_bshowEventDetail)
            {
                #ifdef FEATURE_SPORTS_APP
                if(g_sport_snooze_back_flag == FALSE)
                {
                    if(g_otherappflag == FALSE) 
                    {
                        pme->m_stateToBackShowAlert = g_tempCurEvtFlag;
                        g_gobacktosportflag = FALSE;

                    }
                }
                g_sport_snooze_back_flag = FALSE;
                g_otherappflag = FALSE;
                #endif
				
                //pme->m_eDlgRet = DLGRET_OK;// 闹钟提示完成后关闭对话框	//Del by zzg 2012_03_13

				//Add By zzg 2012_03_13
				if (pme->m_CalMgr.m_bCloseFromTimer == TRUE)
				{
					pme->m_eDlgRet = DLGRET_OK;// 闹钟提示完成后关闭对话框
				}	
				//Add End
            }

#if !defined( AEE_SIMULATOR)
            OEM_SetConfig( CFGI_BEEP_VOL, &keyBeepVolumeSetting, sizeof(byte));
#endif

            //pMenu = 0;
        }
        return TRUE;

        case EVT_USER_REDRAW:
        {
            //{
                int      i              = 0;
                int      end            = GET_EXPIREDAPPOINTMENT_NUMBER( &pme->m_CalMgr);
    //                AECHAR   text[200]      = {0};
    //                AECHAR   date[32]       = {0};
    //                uint32   seconds        = 0;

    //                CtlAddItem   item         = {0};
    //                int          length       = 0;
                Appointment* pAppointment = 0;
                Appointment* theLast      = 0;
                IImage *AlertImage = NULL;
                AECHAR wstrTitle[MAX_INPUT_SUBJECT] = {0};
                MSG_FATAL("EVT_USER_REDRAW...end======%d",end,0,0);

                for( i = 1; i <= end; i ++)
                {

                    pAppointment = pme->m_CalMgr.m_expiredAppointment[i];
                    if( pAppointment->permId == pme->m_CalMgr.m_wPendingAlarmPermID)
                    {
                        theLast = pAppointment;
                    }
                    if( pAppointment->status <= NOT_EXPIRED)
                    {
                        continue;
                    }
                }
                
                AlertImage = ISHELL_LoadResImage(pme->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SCHEDULE);

                Appscommon_ResetBackgroundEx(pme->m_pDisplay, &pme->m_rc, TRUE);
                if(AlertImage != NULL)
                {
                    AEEImageInfo ImageInfo;
                    
                    IIMAGE_GetInfo(AlertImage, &ImageInfo);
                    //IIMAGE_Draw(AlertImage, 0, TITLEBAR_HEIGHT);
                    IIMAGE_Draw(AlertImage, (pme->m_rc.dx - ImageInfo.cx)/2, TITLEBAR_HEIGHT/2 + BOTTOMBAR_HEIGHT/2 + (pme->m_rc.dy - ImageInfo.cy)/2);
                    IIMAGE_Release(AlertImage);
                    AlertImage = NULL;
                }
				
				
                // draw alert text    // if(theLast->baseFields.m_CurEvtType == 2)
                {
                    AECHAR AlertText[64] = {0};
                    AECHAR text[32] = {0};					
					
                    AEERect rc;
                    int length;
                    
                    SETAEERECT(&rc, 0, TITLEBAR_HEIGHT, pme->m_rc.dx, BOTTOMBAR_HEIGHT);
                    IDISPLAY_SetColor(pme->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
                    // schedule alert
                    {
                    	#if defined (FEATURE_VERSION_C337) || defined (FEATURE_VERSION_W317A)
						ISHELL_LoadResString(pme->m_pShell, 
                                                AEE_SCHEDULEAPP_RES_FILE, 
                                                IDS_CALENDAR, 
                                                wstrTitle, 
                                                sizeof(wstrTitle));
						#else
                        ISHELL_LoadResString(pme->m_pShell, 
                                                AEE_SCHEDULEAPP_RES_FILE, 
                                                IDS_APP, 
                                                wstrTitle, 
                                                sizeof(wstrTitle));
						#endif
                                                
                        //SETAEERECT(&rc, 0, TITLEBAR_HEIGHT , pme->m_rc.dx, TITLEBAR_HEIGHT);
                        SETAEERECT(&rc, 0, 0 , pme->m_rc.dx, MENUITEM_HEIGHT);  //Add By zzg 2012_03_08
                        ISHELL_LoadResString(pme->m_pShell,
                                                AEE_SCHEDULEAPP_RES_FILE,
                                                IDS_EVENT_EDIT_TITLE_SUBJECT,
                                                AlertText,
                                                sizeof(AlertText));

                        length = WSTRLEN(AlertText);
                        AlertText[length++] = ':';
                        //AlertText[length++] = '\n';
                        //WSTRNCOPY(d,dlen,s)
                        WSTRCPY(AlertText + length, theLast->subject);
																							
                        IDISPLAY_DrawText(pme->m_pDisplay, AEE_FONT_NORMAL,
                                                            AlertText,
                                                            -1,
                                                            rc.x, rc.y, &rc,
                                                            IDF_TEXT_TRANSPARENT | IDF_ALIGN_LEFT | IDF_ALIGN_MIDDLE);
					
						//rc.y = rc.y + TITLEBAR_HEIGHT;	
						rc.y = rc.y + MENUITEM_HEIGHT;  //Add By zzg 2012_03_08;	
						IDISPLAY_DrawText(pme->m_pDisplay, AEE_FONT_NORMAL,
						                                   theLast->location,
						                                   -1,
						                                   rc.x, rc.y, &rc,
						                                   IDF_TEXT_TRANSPARENT | IDF_ALIGN_LEFT | IDF_ALIGN_MIDDLE);			
                       
                    }
                }				

                //draw titlebar
                {
                    TitleBar_Param_type TitleBar;
                    MEMSET(&TitleBar, 0, sizeof(TitleBar_Param_type));
                    TitleBar.pwszTitle = wstrTitle;
                    TitleBar.dwAlignFlags = IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
					#if 0
                    DrawTitleBar(pme->m_pDisplay, &TitleBar);
					#else
					IANNUNCIATOR_SetFieldText(pme->m_pIAnn,wstrTitle);
					#endif
                }
                //draw bottombar
                {
                    BottomBar_Param_type BottomBar;
                    MEMSET(&BottomBar, 0, sizeof(BottomBar_Param_type));
                    
                    {
                    	if(g_nCurEvtStyle == 0)
                    	{
	                        if(pme->m_CalMgr.m_cfg.nSnooze == 0)
	                        {                 
	                            BottomBar.eBBarType = BTBAR_STOP;
	                        }
	                        else
	                        {
	                            BottomBar.eBBarType = BTBAR_SNOOZE;                        
	                        }
                    	}
						else
						{
							if(pme->m_CalMgr.m_cfg.nSnooze == 0)
	                        {                 
	                            BottomBar.eBBarType = BTBAR_VIEW_STOP;
	                        }
	                        else
	                        {
	                            BottomBar.eBBarType = BTBAR_VIEW_SNOOZE;                        
	                        }
						}
                    }
                    DrawBottomBar(pme->m_pDisplay, &BottomBar);
                }
                    


                if( pme->m_CalMgr.m_wPendingAlarmPermID == EMPTY_ALARM_ID)
                {
                    //IMENUCTL_SetSel( pMenu, theSel);
                }
                else
                {

                    if( theLast->status == EXPIRED && theLast->baseFields.alertst == ALERTSET)
                    {
                        playRingerAlert( &pme->m_CalMgr);		
						pme->m_CalMgr.m_bCloseFromTimer = FALSE; //Add By zzg 2012_03_13
                        ISHELL_SetTimer( pme->m_pShell, 30000, (PFNNOTIFY)stopRingerAlert, &pme->m_CalMgr);
                    }

                    //IMENUCTL_SetSel( pMenu, pme->m_CalMgr.m_wPendingAlarmPermID);
                }
                //IMENUCTL_Redraw( pMenu);
                IDISPLAY_Update(pme->m_pDisplay);
            //}
        }
        return TRUE;

        case EVT_KEY_PRESS:
        {
            MSG_FATAL("***zzg scheduleapp showalert EVT_KEY_PRESS wParam=%x, g_nCurEvtStyle=%x***", wParam, g_nCurEvtStyle, 0);
            if( wParam == AVK_SELECT || wParam == AVK_CLR)
            {            
                if(g_nCurEvtStyle == 0)
                {
                    if(wParam == AVK_SELECT)
                    {
                    
                        char     args[20] = {0};
                        //char      buf[20];
                        
        				
                        //args = STRDUP(buf);
                        /*Cancel timer and StopRingAlert*/
                        ISHELL_CancelTimer( pme->m_pShell, 0, &(pme->m_CalMgr));
                        stopRingerAlert( &pme->m_CalMgr);
                        
                        {
                            Appointment* pAppointment = 0;
                            int          i            = 0;
                            int          end          = GET_EXPIREDAPPOINTMENT_NUMBER( &pme->m_CalMgr);

                            for( i = 1; i <= end; i ++)
                            {
                                pAppointment = pme->m_CalMgr.m_expiredAppointment[i];
                                if( pAppointment->status == EXPIRED)
                                {
                                    pAppointment->status= DONE;
                                }
                            }
                            clearAllDoneAppointment( &pme->m_CalMgr);
                        }
#ifdef FEATURE_SPORTS_APP                        
                        g_sport_snooze_back_flag = TRUE;
                       
                        ISHELL_StartAppletArgs(pme->m_pShell, AEECLSID_SPORTSAPP,args);
#endif                         
                        //FREEIF(args);
                    }

                    if( wParam == AVK_CLR)
                    {
                        Appointment* pAppointment = 0;
                 
                        //int          i            = 0;
                        //int          end          = GET_EXPIREDAPPOINTMENT_NUMBER( &pme->m_CalMgr);
                        if( pme->m_CalMgr.m_expiredAppointment)
    					{
					        int         j       = 0;
					        int         i       = 0;
					        int         end     = GET_EXPIREDAPPOINTMENT_NUMBER(&pme->m_CalMgr);
					        int         num     = end;
					        IDBRecord*  precord = 0;

					        MSG_FATAL("end===========%d",end,0,0);
					        for( i = 1; i <= end; i ++)
					        {
					        	MSG_FATAL("000000000000000000000000000000==%d",pme->m_CalMgr.m_expiredAppointment[i]->status,0,0);
					            if( pme->m_CalMgr.m_expiredAppointment[i]->status == EXPIRED)
					            {
#if !FEATURE_ONE_DB
					                closeDatabaseIf(&pme->m_CalMgr);
					                openDatabaseIf( &pme->m_CalMgr,
					                                pme->m_CalMgr.m_expiredAppointment[i]->julian.wYear,
					                                pme->m_CalMgr.m_expiredAppointment[i]->julian.wMonth
					                            );
#endif
									MSG_FATAL("11111111111111111111111111111111111",0,0,0);
					                precord = IDATABASE_GetRecordByID(pme->m_CalMgr.m_pIDatabase, pme->m_CalMgr.m_expiredAppointment[i]->dbId);
					                if( precord)
					                {
					                    pme->m_CalMgr.m_expiredAppointment[i]->baseFields.wAlarmTime = 65535;
					                    updateDBRecord(&pme->m_CalMgr,
					                                precord,
					                                pme->m_CalMgr.m_expiredAppointment[i]->permId,
					                                pme->m_CalMgr.m_expiredAppointment[i]->subject,
					                                pme->m_CalMgr.m_expiredAppointment[i]->location,
					                                &pme->m_CalMgr.m_expiredAppointment[i]->baseFields
					                            );
					                }
					                MSG_FATAL("IAlarm_CancelAlarm...........................",0,0,0);
					                IAlarm_CancelAlarm( pme->m_CalMgr.m_pIAlarm,
					                        AEECLSID_SCHEDULEAPP,
					                        pme->m_CalMgr.m_expiredAppointment[i]->permId
					                    );


					                FREEIF( pme->m_CalMgr.m_expiredAppointment[i]);
					                pme->m_CalMgr.m_expiredAppointment[i] = NULL;

					                num --;
					            }
					        }

					        SET_EXPIREDAPPOINTMENT_NUMBER(&pme->m_CalMgr, num);
					        if( num == end || num == 0)
					        {
					            return;
					        }

					        for( i = 1; i <= end; i ++)
					        {
					            if( pme->m_CalMgr.m_expiredAppointment[i] != NULL)
					            {
					                j = i + 1;

					                for( ; j <= end; j ++)
					                {
					                    if( pme->m_CalMgr.m_expiredAppointment[j] != NULL)
					                    {
					                        pme->m_CalMgr.m_expiredAppointment[i] = pme->m_CalMgr.m_expiredAppointment[j];
					                        pme->m_CalMgr.m_expiredAppointment[j] = NULL;
					                    }
					                }
					            }
					        }

					    }
						MSG_FATAL("----------------------->ok2", 0, 0, 0); 
						ISHELL_CancelTimer( pme->m_pShell, 0, &(pme->m_CalMgr));
                        stopRingerAlert( &pme->m_CalMgr);
                        //MSG_FATAL("end       =============%d",end,0,0);
#if  0//def //FEATURE_SPORTS_APP     
                        for( i = 1; i <= end; i ++)
                        {
                            pAppointment = pme->m_CalMgr.m_expiredAppointment[i];
                            if( pAppointment->status == EXPIRED)
                            {
                                pAppointment->status= DONE;
                            }
                        }

                       {
                   
                            if(g_otherappflag == FALSE)
                            {
                                pme->m_stateToBackShowAlert = g_tempCurEvtFlag;
                                
                                g_gobacktosportflag = FALSE;

                            }
                      }
                      clearAllDoneAppointment( &pme->m_CalMgr);
#endif                      

                    }
					CLOSE_DIALOG( DLGRET_OK)
                }

                if((g_nCurEvtStyle == 1)&&(wParam == AVK_SELECT))
                {
                    //if(wParam == AVK_SELECT)
                    //{
                    uint16          permId = pme->m_CalMgr.m_wPendingAlarmPermID;
                    CalDBRecFields  fields = {0};
                    
                    if( getAppointmentByPermId( &pme->m_CalMgr, permId, &fields, &pme->m_CalMgr.m_appointmentToView) > 0)
                    {
                        pme->m_stateToBackViewEvent = pme->m_stateToBackShowAlert;
                        pme->m_bshowEventDetail = TRUE;
                        CLOSE_DIALOG( DLGRET_SHOWALERT_TO_VIEWEVENT)
                        return TRUE;
                    }
					CLOSE_DIALOG( DLGRET_OK)
                    //}
                }
                
            }
        }
        return TRUE;

    }//switch (eCode)

    return FALSE;
} // CScheduleApp_HandleShowAlert


static void stopRingerAlert(CCalApp *pme)
{
#if !defined( AEE_SIMULATOR)
    if(pme->m_alert == NULL)
    {
        return;
    }
	pme->m_bCloseFromTimer = TRUE;		//Add By zzg 2012_03_13
    (void) ISHELL_EndDialog(pme->m_pIShell);
#endif
}

static void playRingerAlert(CCalApp *pme)
{

#if !defined( AEE_SIMULATOR)
    if( pme->m_alert == NULL &&
        ISHELL_CreateInstance( pme->m_pIShell, AEECLSID_ALERT, (void**)&pme->m_alert) != SUCCESS
    )
    {
        return;
    }
    else
    {
        uint16  ring_id = 1;
        ringID  ringid[PROFILENUMBER] = {0};
        byte profilenum;
        
        (void) OEM_GetConfig( CFGI_ALARM_RINGER, &ring_id, sizeof(ring_id));

        (void) OEM_GetConfig(CFGI_PROFILE_CUR_NUMBER, &profilenum, sizeof(profilenum));
        (void) OEM_GetConfig(CFGI_PROFILE_ALARM_RINGER, (void*)ringid, sizeof(ringid));
        ScheduleAppNotifyMP3PlayerAlertEvent(pme,TRUE);
        if(ringid[profilenum].ringType == OEMNV_MID_RINGER)
        {
            IALERT_StartRingerAlert(pme->m_alert, ring_id);
        }
        else
        {
            if ((IALERT_StartMp3Alert(pme->m_alert, ringid[profilenum].szMusicname, ALERT_NORMAL_SND) != SUCCESS))
            {
                IALERT_StartRingerAlert(pme->m_alert, (uint32)ring_id);
            }                        
        }
        ISHELL_SendEvent(pme->m_pIShell,
                     AEECLSID_APP_FMRADIO,
                     EVT_ALARM,
                     TRUE,
                     0
                 );
        //IALERT_StartRingerAlert(pme->m_alert, ring_id);
    }
#endif

}


static boolean  dialog_handler_of_state_viewevent( CScheduleApp* pme,
                                              AEEEvent   eCode,
                                              uint16     wParam,
                                              uint32     dwParam
)
{
    static IStatic*     pStatic        = 0;
    static Appointment* pappointment   = 0;
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
        {
            pStatic = (IStatic*)IDIALOG_GetControl( pme->m_pActiveDlg, IDC_VIEWEVENT_STATIC);
            pappointment = pme->m_CalMgr.m_appointmentToView;
            if( !pappointment)
            {
                debug( ";view event, null appointment");
                return FALSE;
            }
            ISTATIC_SetProperties( pStatic, ST_GRAPHIC_BG|ST_NOSCROLL);
            ISTATIC_SetBackGround(pStatic, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND);//modified by yangdecai
            {
                AEERect rect = pme->m_rc;

                rect.dy -= BOTTOMBAR_HEIGHT;
                rect.y   = 0;
                ISTATIC_SetRect( pStatic, &rect);
            }
            ISTATIC_SetActive( pStatic, TRUE);
        }
        return TRUE;

        case EVT_DIALOG_START:
        {
            AECHAR  text[200]    = {0};
            AECHAR  date[32]     = {0};
            AECHAR  alarm[32]    = {0};
            AECHAR  mode[32]     = {0};
            AECHAR  title[5][32] = {{0}};
            uint32  seconds      = 0;
            int     i            = 0;
            int     length       = 0;
            int     hour         = GET_HOUR( pappointment->baseFields.wDuration);
            int     minute       = GET_MINUTE( pappointment->baseFields.wDuration);
            int     alarmSetting = 0;

            static uint16 alarmTime[]   = { 65535, 0, 1, 5, 10, 15, 30, 60, 360, 720, 1440};

            while( alarmTime[alarmSetting] != pappointment->baseFields.wAlarmTime && alarmSetting < 11)
            {
                alarmSetting ++;
            }
            ISHELL_LoadResString( pme->m_pShell,
                        AEE_SCHEDULEAPP_RES_FILE,
                        IDS_EVENT_EDIT_ALARM_OPTION_NO_ALARM + alarmSetting,
                        alarm,
                        64
                    );

            for( i = 0; i < 5; i ++)
            {
                ISHELL_LoadResString( pme->m_pShell,
                            AEE_SCHEDULEAPP_RES_FILE,
                            IDS_EVENT_EDIT_TITLE_SUBJECT + i,
                            title[i],
                            64
                        );
            }

            if(pme->m_sports)
            {
                ISHELL_LoadResString( pme->m_pShell,
                            AEE_SCHEDULEAPP_RES_FILE,
                            IDS_SPORTS_DISTANCE,
                            title[0],
                            64
                            );

                ISHELL_LoadResString( pme->m_pShell,
                            AEE_SCHEDULEAPP_RES_FILE,
                            IDS_SPORTS_TIME,
                            title[1],
                            64
                            );
                
                ISHELL_LoadResString( pme->m_pShell,
                            AEE_SCHEDULEAPP_RES_FILE,
                            IDS_SPORTS_MODE,
                            title[3],
                            64
                            );

                ISHELL_LoadResString( pme->m_pShell,
                            AEE_SCHEDULEAPP_RES_FILE,
                            IDS_SPORTS_RUN + (pappointment->baseFields.walk - WALK),
                            mode,
                            64
                        );
                
                
            }
            
            if( WSTRLEN( pappointment->subject) > 0)
            {
                WSTRCPY( text, title[0]);
                length = WSTRLEN( text);
                text[length ++] = ':';
                text[length ++] = '\n';
                text[length ++] = ' ';
                text[length ++] = ' ';
                WSTRCPY( text + length, pappointment->subject);
                length = WSTRLEN( text);
                text[length ++] = '\n';
            }
            if( WSTRLEN( pappointment->location) > 0)
            {
                WSTRCPY( text + length, title[1]);
                length = WSTRLEN( text);
                text[length ++] = ':';
                text[length ++] = '\n';
                text[length ++] = ' ';
                text[length ++] = ' ';
                WSTRCPY( text + length, pappointment->location);
                length = WSTRLEN( text);
                text[length ++] = '\n';
            }
            WSTRCPY( text + length, title[2]);
            length = WSTRLEN( text);
            text[length ++] = ':';
            text[length ++] = '\n';
            text[length ++] = ' ';
            text[length ++] = ' ';
            seconds = JULIANTOSECONDS( &pappointment->julian);
#ifdef FEATURE_APP_WORLDTIME    			
            Calendar_FormatDateTime( seconds, date, sizeof( date));
#endif
            WSTRCPY( text + length, date);
            length = WSTRLEN( text);
            text[length ++] = '\n';
            WSTRCPY( text + length, title[3]);
            length = WSTRLEN( text);
            text[length ++] = ':';
            text[length ++] = '\n';
            text[length ++] = ' ';
            text[length ++] = ' ';
            
            if(pme->m_sports)
            {
                WSTRCPY( text + length, mode);
            }
            else
            {
                WSPRINTF( text + length, 32, L"%02d:%02d", hour, minute);
            }
            
            length = WSTRLEN( text);
            text[length ++] = '\n';
            WSTRCPY( text + length, title[4]);
            length = WSTRLEN( text);
            text[length ++] = ':';
            text[length ++] = '\n';
            text[length ++] = ' ';
            text[length ++] = ' ';
            WSTRCPY( text + length, alarm);
            length = WSTRLEN( text);
            text[length] = '\n';

            debug( ";view event, text = %S", text);
            ISTATIC_SetText( pStatic, 0, text, 0, 0);
            repaint(pme, TRUE);
        }
        return TRUE;

        case EVT_DIALOG_END:
            if( pme->m_stateToBackViewEvent == STATE_VIEWDAY && !pme->m_bInactive)
            {
                FREEIF( pme->m_CalMgr.m_appointmentToView);
                pme->m_CalMgr.m_appointmentToView = NULL;
            }
            IDISPLAY_SetColor( pme->m_pDisplay, CLR_SYS_SCROLLBAR_FILL, 0);
            pStatic = 0;
            return TRUE;

        case EVT_USER_REDRAW:
        {
            TitleBar_Param_type TitleBar = {0};
            AECHAR                 wstrTitle[MAX_INPUT_SUBJECT+1] = {0};
#ifdef FEATURE_FUNCS_THEME
            Theme_Param_type    themeParms          = {0};
            RGBVAL              scrollbarFillColor  = 0;
#else
            RGBVAL                  nOldFontColor;
#endif
            drawBottomBar(BTBAR_BACK);

            //draw titlebar
            ISHELL_LoadResString(pme->m_pShell, AEE_SCHEDULEAPP_RES_FILE, IDS_VIEW_PLAN_DETAIL_TITLE, wstrTitle, sizeof(wstrTitle));
            TitleBar.pwszTitle = wstrTitle;
            TitleBar.dwAlignFlags = IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
			#if 0
            DrawTitleBar(pme->m_pDisplay, &TitleBar);
			#else
			IANNUNCIATOR_SetFieldText(pme->m_pIAnn,wstrTitle);
			#endif
#ifdef FEATURE_FUNCS_THEME
            Appscom_GetThemeParameters( &themeParms);
            scrollbarFillColor  =   themeParms.themeColor;
            scrollbarFillColor = IDISPLAY_SetColor( pme->m_pDisplay, CLR_SYS_SCROLLBAR_FILL, scrollbarFillColor);
#else
            nOldFontColor = IDISPLAY_SetColor(pme->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
#endif

            ISTATIC_Redraw( pStatic);

#ifdef FEATURE_FUNCS_THEME
            IDISPLAY_SetColor( pme->m_pDisplay, CLR_SYS_SCROLLBAR_FILL, scrollbarFillColor);
#else
            (void)IDISPLAY_SetColor(pme->m_pDisplay, CLR_USER_TEXT, nOldFontColor);
#endif
        }
        IDISPLAY_Update(pme->m_pDisplay);
        return TRUE;
#if 0
        case EVT_CTL_PREREDRAW:
        {
            Theme_Param_type    themeParms  = {0};
            CtlValChange*       pv          = (CtlValChange*)dwParam;

            Appscom_GetThemeParameters( &themeParms);
            IDISPLAY_SetColor( pme->m_pDisplay, CLR_SYS_SCROLLBAR_FILL, themeParms.themeColor);

            IDISPLAY_FillRect( pme->m_pDisplay, &pme->m_rc, 0);
            drawBottomBar(BTBAR_BACK);
            pv->bValid = FALSE;
        }
        return TRUE;
#endif     

#ifdef FEATURE_LCD_TOUCH_ENABLE//ydc ADD FOR LCD TOUCH
		case EVT_PEN_UP:
		{
			int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
			int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
			AEERect bottomBarRect;
			//int ht;
			int nBarH ;
			AEEDeviceInfo devinfo;
			nBarH = GetBottomBarHeight(pme->m_pDisplay);
			
			MEMSET(&devinfo, 0, sizeof(devinfo));
			ISHELL_GetDeviceInfo(pme->m_pShell, &devinfo);
			SETAEERECT(&bottomBarRect, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
			MSG_FATAL("wXPos=====%d,wYPos=========%d",wXPos,wYPos,0);
			if( TOUCH_PT_IN_RECT(wXPos, wYPos, bottomBarRect))
			{
				if(wXPos >= bottomBarRect.x + (bottomBarRect.dx/3)*2 && wXPos < bottomBarRect.x + (bottomBarRect.dx/3)*3 )//右
				{						
					return  ISHELL_PostEvent(pme->m_pShell,AEECLSID_SCHEDULEAPP,EVT_USER,AVK_CLR,0);
				}
			}
		}
#endif

        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    if(pme->m_bshowEventDetail)
                    {
                        Appointment* pAppointment = 0;
                        int          i            = 0;
                        int          end          = GET_EXPIREDAPPOINTMENT_NUMBER( &pme->m_CalMgr);

                        for( i = 1; i <= end; i ++)
                        {
                            pAppointment = pme->m_CalMgr.m_expiredAppointment[i];
                            if( pAppointment->status == EXPIRED)
                            {
                                pAppointment->status= DONE;
                                debug( ";[%d].( %d, %d), done, %S", i, pAppointment->permId, pAppointment->dbId, pAppointment->subject);
                            }
                        }
                        clearAllDoneAppointment( &pme->m_CalMgr);
                    }                    
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
            }
            break;
    }
    return FALSE;
} // CScheduleApp_HandleViewEvent


boolean CalMgr_InitAppData(CCalApp *pme, IShell * pIShell, IDisplay *pIDisplay)
{
    pme->m_pIShell = pIShell;
    pme->m_pIDisplay = pIDisplay;
    pme->m_AlertState = ALERTSET;

       //check
//    (void) ISHELL_AddRef(pme->m_pIShell);
//    (void) IDISPLAY_AddRef(pme->m_pIDisplay);

    pme->m_wPendingAlarmPermID = EMPTY_ALARM_ID;

#if defined( FEATURE_UIALARM) && !defined( AEE_SIMULATOR)
    if (ISHELL_CreateInstance(pIShell,
                              AEECLSID_UIALARM,
                              (void**)&pme->m_pIAlarm))
    {
        MSG_FATAL("ALARM CREAT FAILE",0,0,0);
        return FALSE;
    }
#endif

    if (ISHELL_CreateInstance(pIShell,
                              AEECLSID_DBMGR,
                             (void **)&pme->m_pIDBMgr))
    {
        return FALSE;
    }


#if !defined( AEE_SIMULATOR)
    if (AEE_SUCCESS != ISHELL_CreateInstance(pIShell,
                                             AEECLSID_ALERT,
                                             (void **)&pme->m_alert))
    {
        return FALSE;
    }
#endif

    //读进applet上次所保存的关于日程表设置
    //suspect如果设置成功，是否还要把数据初始化
#if defined(FILECFG)
    // Create IFileMgr instance
    if ( SUCCESS != ISHELL_CreateInstance( pIShell,
                                           AEECLSID_FILEMGR,
                                           (void **)&pme->m_pFileMgr))
    {
        return FALSE;
    }
    else
    {
        // Read the config file
        CScheduleApp_CFGCacheInit(&pme->m_cfg);
        (void)CScheduleApp_ReadCFG(pme->m_pFileMgr, &pme->m_cfg);
    }
#else
    if (ISHELL_GetPrefs(pIShell,
                        AEECLSID_SCHEDULEAPP,
                        CAL_CFG_VERSION,
                        &pme->m_cfg,
                        sizeof(CalCfg)))
    {
        pme->m_cfg.nAutoDelete  = IDS_SETUP_AUTO_DELETE_OPTION_30_DAY;
        pme->m_cfg.wDayStart    = 7 * MINUTES_PER_HOUR;
        pme->m_cfg.nSnooze      = 0;
    }
#endif

   return TRUE;
}

void CalMgr_FreeAppData(CCalApp *pme)
{

    //suspect:
//    if(pme->m_pIShell != NULL)
//    {
//        (void) ISHELL_Release(pme->m_pIShell);
//        pme->m_pIShell = NULL;
//    }
//
//    if(pme->m_pIDisplay != NULL)
//    {
//        (void) IDISPLAY_Release(pme->m_pIDisplay);
//        pme->m_pIDisplay = NULL;
//    }
#ifdef FEATURE_UIALARM
    if (pme->m_pIAlarm)
    {
        //debug( ";CalMgr_FreeAppData, IAlarm_Release");
        IAlarm_Release(pme->m_pIAlarm);
    }
#endif

    closeDatabaseIf( pme);
    //debug( ";********* free, 1*********");

    if (pme->m_alert != NULL)
    {
        IALERT_Release(pme->m_alert);
        pme->m_alert = NULL;
    }


    if (pme->m_pIDBMgr != NULL)
    {
        (void) IDBMGR_Release(pme->m_pIDBMgr);
        pme->m_pIDBMgr = NULL;
    }   

#if defined(FILECFG)
    if(pme->m_pFileMgr != NULL)
    {
        IFILEMGR_Release(pme->m_pFileMgr);
        pme->m_pFileMgr = NULL;
    }
#endif

    releaseMonthEvents( pme);
    //debug( ";********* free, 2*********");
    releaseExpiredAppointmentArrayIf( pme);
    //debug( ";********* free, 3*********");
}


#if defined(FILECFG)
/*==============================================================================
函数:
       CScheduleApp_ReadCFG

说明:
       将配置文件中的内容用读到Cache中

参数:
       pFileMgr [in]：指向IFileMgr对象结构的指针。
       pCFGCache [out]:存储数据的Cache

返回值:
       SUCCESS : 操作成功
       EFAILED : 操作失败
       EBADPARM: 参数错误

备注:

==============================================================================*/
static int CScheduleApp_ReadCFG(IFileMgr *pFileMgr, CalCfg *pCFGCache)
{
    IFile  *pFile;
    uint16  version;
    if(pCFGCache == NULL || pFileMgr == NULL)
    {
        return EBADPARM;
    }

    // 打开配置文件
    pFile = IFILEMGR_OpenFile(pFileMgr, FILE_SCHEDULE_CFG, _OFM_READ);
    if(pFile == NULL)
    {
        // 尚未创建，使用默认值
        return SUCCESS;
    } //else 打开文件继续执行

    // 从文件开始读取数据
    if(SUCCESS != IFILE_Seek(pFile, _SEEK_START, 0))
    {
        (void)IFILE_Release(pFile);
        return EFAILED;
    }

    // 读取版本号
    if(sizeof(version) != IFILE_Read(pFile, &version, sizeof(version)))
    {
        (void)IFILE_Release(pFile);
        return EFAILED;
    }

    // 校验版本号
    if(version != CAL_CFG_VERSION)
    {
        // 版本号不相等，忽略此文件，使用默认值
        (void)IFILE_Release(pFile);
        return SUCCESS;
    }

    // 读取配置信息
    if(sizeof(CalCfg) != IFILE_Read( pFile,
                                     pCFGCache,
                                     sizeof(CalCfg)))
    {
        (void)IFILE_Release(pFile);
        return EFAILED;
    }

    (void)IFILE_Release(pFile);
    return SUCCESS;
} //

/*==============================================================================
函数:
       CScheduleApp_WriteCFG

说明:
       将Cache中的内容写到配置文件中

参数:
       pFileMgr [in]：指向IFileMgr对象结构的指针。
       pCFGCache [out]:存储数据的Cache

返回值:
       SUCCESS : 操作成功
       EFAILED : 操作失败
       EBADPARM: 参数错误

备注:

==============================================================================*/
static int CScheduleApp_WriteCFG(IFileMgr *pFileMgr, CalCfg *pCFGCache)
{
    IFile  *pFile;
    uint16  version = CAL_CFG_VERSION;
    if(pCFGCache == NULL || pFileMgr == NULL)
    {
        return EBADPARM;
    }

    // 打开配置文件
    pFile = IFILEMGR_OpenFile(pFileMgr, FILE_SCHEDULE_CFG, _OFM_READWRITE);
    if(pFile == NULL)
    {
        // 创建配置文件
        pFile = IFILEMGR_OpenFile(pFileMgr, FILE_SCHEDULE_CFG, _OFM_CREATE);

        if(pFile == NULL)
        {
            // 创建配置文件失败
            return EFAILED;
        }//else 打开文件继续执行
    }//else 打开文件继续执行

    // 校验版本号
    if(SUCCESS != IFILE_Seek(pFile, _SEEK_START, 0))
    {
        (void)IFILE_Release(pFile);
        return EFAILED;
    }

    // 写版本号
    if(sizeof(version) != IFILE_Write(pFile, &version, sizeof(version)))
    {
        (void)IFILE_Release(pFile);
        return EFAILED;
    }

    // 写配置信息
    if(sizeof(CalCfg) != IFILE_Write( pFile,
                                      pCFGCache,
                                      sizeof(CalCfg)))
    {
        (void)IFILE_Release(pFile);
        return EFAILED;
    }

    (void)IFILE_Release(pFile);
    return SUCCESS;
} // CContApp_WriteCFG

/*==============================================================================
函数:
       CScheduleApp_CFGCacheInit

说明:
       初始化配置项Cache

参数:
       pCFGCache [int]:存储数据的Cache

返回值:

备注:

==============================================================================*/
static void CScheduleApp_CFGCacheInit(CalCfg *pCFGCache)
{

    pCFGCache->nAutoDelete   = 65535;
    pCFGCache->wDayStart     = 7 * MINUTES_PER_HOUR;;
    pCFGCache->nSnooze       = 15;

#if !FEATURE_ONE_DB
    pCFGCache->maxPermId    = 1;
#endif
}
#endif




#if FEATURE_DRAW_LUNAR_CALENDAR

/*************************************************************************
 内部调用方法
*************************************************************************/
/*************************************************************************
===>以下是处理阴历的程序
*************************************************************************/
//================================================================================//
//阴历的资源初始化
//================================================================================//
boolean Calendar_InitAppData(CCalendarApp * pme, IShell* pIShell)
{
   int i;
   //要增加addreference 函数在这里

   //存入阴历1899年到2051年每年中的月天数信息
   //阴历每月只能是29或30天，一年用12（或13）个二进制位表示，对应位为1表30天，否则为29天
   //测试数据只有1899.1.1 --2051.12.31
   uint32 gLunarMonthDaytemp[]=
   {
                                                                             0X4b68,   //1900
     0X4ae0, 0Xa570, 0X5268, 0Xd260, 0Xd950, 0X6aa8, 0X56a0, 0X9ad0, 0X4ae8, 0X4ae0,   //1910
     0Xa4d8, 0Xa4d0, 0Xd250, 0Xd548, 0Xb550, 0X56a0, 0X96d0, 0X95b0, 0X49b8, 0X49b0,   //1920
     0Xa4b0, 0Xb258, 0X6a50, 0X6d40, 0Xada8, 0X2b60, 0X9570, 0X4978, 0X4970, 0X64b0,   //1930
     0Xd4a0, 0Xea50, 0X6d48, 0X5ad0, 0X2b60, 0X9370, 0X92e0, 0Xc968, 0Xc950, 0Xd4a0,   //1940
     0Xda50, 0Xb550, 0X56a0, 0Xaad8, 0X25d0, 0X92d0, 0Xc958, 0Xa950, 0Xb4a8, 0X6ca0,   //1950
     0Xb550, 0X55a8, 0X4da0, 0Xa5b0, 0X52b8, 0X52b0, 0Xa950, 0Xe950, 0X6aa0, 0Xad50,   //1960
     0Xab50, 0X4b60, 0Xa570, 0Xa570, 0X5260, 0Xe930, 0Xd950, 0X5aa8, 0X56a0, 0X96d0,   //1970
     0X4ae8, 0X4ad0, 0Xa4d0, 0Xd268, 0Xd250, 0Xd528, 0Xb540, 0Xb6a0, 0X96d0, 0X95b0,   //1980
     0X49b0, 0Xa4b8, 0Xa4b0, 0Xb258, 0X6a50, 0X6d40, 0Xada0, 0Xab60, 0X9370, 0X4978,   //1990
     0X4970, 0X64b0, 0X6a50, 0Xea50, 0X6b28, 0X5ac0, 0Xab60, 0X9368, 0X92e0, 0Xc960,   //2000
     0Xd4a8, 0Xd4a0, 0Xda50, 0X5aa8, 0X56a0, 0Xaad8, 0X25d0, 0X92d0, 0Xc958, 0Xa950,   //2010
     0Xb4a0, 0Xb550, 0Xad50, 0X55a8, 0X4ba0, 0Xa5b0, 0X52b8, 0X52b0, 0Xa930, 0X74a8,   //2020
     0X6aa0, 0Xad50, 0X4da8, 0X4b60, 0X9570, 0Xa4e0, 0Xd260, 0Xe930, 0Xd530, 0X5aa0,   //2030
     0X6b50, 0X96d0, 0X4ae8, 0X4ad0, 0Xa4d0, 0Xd258, 0Xd250, 0Xd520, 0Xdaa0, 0Xb5a0,   //2040
     0X56d0, 0X4ad8, 0X49b0, 0Xa4b8, 0Xa4b0, 0Xaa50, 0Xb528, 0X6d20, 0Xada0, 0X55b0,   //2050
     0X9370, //2051
   };

   //存放阴历1899年到2052年闰月的月份，如没有则为0，每字节存两年
   uint8  gLunarMonthtemp[]=
   {
                              0x08,   //1900
      0X00, 0X50, 0X04, 0X00, 0X20,   //1910
      0X60, 0X05, 0X00, 0X20, 0X70,   //1920
      0X05, 0X00, 0X40, 0X02, 0X06,   //1930
      0X00, 0X50, 0X03, 0X07, 0X00,   //1940
      0X60, 0X04, 0X00, 0X20, 0X70,   //1950
      0X05, 0X00, 0X30, 0X80, 0X06,   //1960
      0X00, 0X40, 0X03, 0X07, 0X00,   //1970
      0X50, 0X04, 0X08, 0X00, 0X60,   //1980
      0X04, 0X0a, 0X00, 0X60, 0X05,   //1990
      0X00, 0X30, 0X80, 0X05, 0X00,   //2000
      0X40, 0X02, 0X07, 0X00, 0X50,   //2010
      0X04, 0X09, 0X00, 0X60, 0X04,   //2020
      0X00, 0X20, 0X60, 0X05, 0X00,   //2030
      0X30, 0Xb0, 0X06, 0X00, 0X50,   //2040
      0X02, 0X07, 0X00, 0X50, 0X03,   //2050
      0x08, //2052
   };

   //存放0-11个月总天数
   uint32 monthdaytemp[]={0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
                        //0,  1,  2,  3,   4,   5,   6,   7,   8,   9,  10,  11

   if(pme == NULL)
   {
     return FALSE;
   }

   //check
   pme->c_pIShell = pIShell;
//   (void) ISHELL_AddRef(pme->c_pIShell);

   //suspect 以下的代码有待优化
   //为了提高速度,将数组在程序开始时初始化放入内存,而不用在函数内再去初始化
   for(i=0;i<152;i++)
   {
      pme->gLunarMonthDay[i] = gLunarMonthDaytemp[i];
   }
   for(i=0;i<77;i++)
   {
      pme->gLunarMonth[i] = gLunarMonthtemp[i];
   }
   for(i=0;i<12;i++)
   {
      pme->monthday[i] = monthdaytemp[i];
   }

   MEMSET (pme->mGz_String, 0, sizeof(pme->mGz_String));
   MEMSET (pme->mSx_String, 0, sizeof(pme->mSx_String));
   MEMSET (pme->mLunMonth_String, 0, sizeof(pme->mLunMonth_String));
   MEMSET (pme->mLunLeapMonth_String, 0, sizeof(pme->mLunLeapMonth_String));
   MEMSET (pme->mLunDay_String, 0, sizeof(pme->mLunDay_String));

   (void) ISHELL_LoadResString(pme->c_pIShell,
                               AEE_CALENDARAPP_RES_FILE,
                               IDS_LUNAR_ALL,
                               pme->mGz_String,
                               sizeof(pme->mGz_String));

   (void) ISHELL_LoadResString(pme->c_pIShell,
                               AEE_CALENDARAPP_RES_FILE,
                               IDS_SHENGX_ALL,
                               pme->mSx_String,
                               sizeof(pme->mSx_String));

   (void) ISHELL_LoadResString(pme->c_pIShell,
                               AEE_CALENDARAPP_RES_FILE,
                               IDS_MONTH_ALL,
                               pme->mLunMonth_String,
                               sizeof(pme->mLunMonth_String));

   (void) ISHELL_LoadResString(pme->c_pIShell,
                               AEE_CALENDARAPP_RES_FILE,
                               IDS_LEAPMONTH_ALL,
                               pme->mLunLeapMonth_String,
                               sizeof(pme->mLunLeapMonth_String));

   (void) ISHELL_LoadResString(pme->c_pIShell,
                               AEE_CALENDARAPP_RES_FILE,
                               IDS_DATE_ALL,
                               pme->mLunDay_String,
                               sizeof(pme->mLunDay_String));

   return TRUE;
}

void Calendar_FreeAppData(CCalendarApp * pme)
{
//    if(pme->c_pIShell != NULL)
//    {
//         (void) ISHELL_Release(pme->c_pIShell);
//
//         //pme->c_pIShell只能释放一次
//       pme->c_pIShell = NULL;
//     }
}

//计算公历iYear年iMonth月iDay日对应的阴历日期,
//1900年1月1日---2051年12月31日
void CScheduleApp_GetLunarDate(CCalendarApp * pme,uint16 iYear, uint16 iMonth, uint16 iDay)
{
   uint32 iSpanDays;
   uint32 tmp;
   uint16 LeapMonth=0;
   uint8  rcode=0;
   if(pme == NULL)
   {
     return;
   }

   iSpanDays = CScheduleApp_CalcDateDiff(pme,iYear, iMonth, iDay,1900,1,1);

   //计算从1900年1月1日过iSpanDays天后的阴历日期
   //阳历1900年1月31日为阴历1900年正月初一
   //阳历1900年1月1日到1月31日共有30天
   if(iSpanDays <30)
   {
      pme->m_yYear  = START_YEAR-1;
      pme->m_yMonth = 12;
      pme->m_yDay   = CScheduleApp_LoWord(iSpanDays);
      return;
   }

   //下面从阴历1900年正月初一算起
   iSpanDays -=30;
   pme->m_yYear  = START_YEAR;
   pme->m_yMonth = 1;
   pme->m_yDay   = 1;

   //计算年
   tmp = CScheduleApp_LunarYearDays(pme, pme->m_yYear);
   while(iSpanDays >= tmp)
   {
      iSpanDays -= tmp;
      tmp = CScheduleApp_LunarYearDays(pme, ++pme->m_yYear);
   }

   //计算月
   LeapMonth = CScheduleApp_GetLeapMonth(pme, pme->m_yYear);
   tmp = CScheduleApp_LunarMonthDays(pme, pme->m_yYear, pme->m_yMonth);
   if(iSpanDays >= tmp)
   {
      do{
         if(pme->m_yMonth == LeapMonth)
         {
            tmp = CScheduleApp_LunarMonthDays(pme, pme->m_yYear, pme->m_yMonth);
            iSpanDays -= CScheduleApp_LoWord(tmp);
            tmp = CScheduleApp_HiWord(tmp);
            if(iSpanDays < tmp){rcode = TRUE;break;}
         }
         iSpanDays -= tmp;
         pme->m_yMonth++;
         tmp = CScheduleApp_LunarMonthDays(pme, pme->m_yYear, pme->m_yMonth);
         tmp = CScheduleApp_LoWord(tmp);
      }
      while(iSpanDays >= tmp);
   }

   //计算日
   pme->m_yDay += (uint16)iSpanDays;

   pme->m_LeapMonth = rcode;
   return;
}

//获取对应阴历日期汉字字符串
void CScheduleApp_GetChineseLunar(CCalendarApp * pme)
{
   uint16 GzYear,SxYear;
   if(pme == NULL)
   {
     return ;
   }

  //获取对应阴历年份对应干支纪年
   if((pme->m_yYear - 3)%60)
      GzYear = (pme->m_yYear - 3)%60;
   else
      GzYear = 60;
   pme->m_pResGz = pme->mGz_String;
   pme->m_pResGz += (GzYear - 1)* 2;


  //获取对应阴历年份对应属相
   if((pme->m_yYear - 3)%12)
      SxYear = ((pme->m_yYear - 3)%12);
   else
      SxYear = 12;
   pme->m_pResSx = pme->mSx_String;
   pme->m_pResSx += (SxYear - 1)* 2;


  //获取阴历日期对应字符串
   if(pme->m_LeapMonth)
   {
      pme->m_pResLunM = pme->mLunLeapMonth_String;
      pme->m_pResLunM += (pme->m_yMonth - 1)* 3;
   }
   else
   {
      pme->m_pResLunM = pme->mLunMonth_String;
      pme->m_pResLunM += (pme->m_yMonth - 1)* 2;
   }

   pme->m_pResLunD = pme->mLunDay_String;
   pme->m_pResLunD += (pme->m_yDay - 1)* 2;
}


//返回阴历iLunarYear年的总天数
// 1900年1月---2051年12月
static uint16 CScheduleApp_LunarYearDays(CCalendarApp * pme, uint16 iLunarYear)
{
   uint16 i;
   uint32 tmp;
   uint16 days =0;
   for(i=1; i<=12; i++)
   {
      tmp = CScheduleApp_LunarMonthDays(pme, iLunarYear ,i);
      days += CScheduleApp_HiWord(tmp);
      days += CScheduleApp_LoWord(tmp);
   }
   return days;
}



//计算公历两个日期间相差的天数  1年1月1日 --- 65535年12月31日
static uint32 CScheduleApp_CalcDateDiff(CCalendarApp * pme,uint16 iEndYear, uint16 iEndMonth, uint16 iEndDay,
                              uint16  iStartYear, uint16 iStartMonth, uint16 iStartDay)
{
   uint32 iDiffDays;
   iStartYear = START_YEAR;
   iStartMonth =1;
   iStartDay =1;

   //计算两个年份1月1日之间相差的天数
   iDiffDays =(iEndYear - iStartYear)*365;
   iDiffDays += (iEndYear-1)/4 - (iStartYear-1)/4;      //闰年 +1
   iDiffDays -= ((iEndYear-1)/100 - (iStartYear-1)/100);    //非闰年 -1
   iDiffDays += (iEndYear-1)/400 - (iStartYear-1)/400;    //闰年 +1

   //加上iEndYear年1月1日到iEndMonth月iEndDay日之间的天数
   iDiffDays += pme->monthday[iEndMonth-1] +
               ((CScheduleApp_IsLeapYear(iEndYear)&&(iEndMonth>2))? 1: 0);
   iDiffDays += iEndDay;

   //减去iStartYear年1月1日到iStartMonth月iStartDay日之间的天数
   iDiffDays -= (pme->monthday[iStartMonth-1] +
               (CScheduleApp_IsLeapYear(iStartYear)&&iStartMonth>2 ? 1: 0));
   iDiffDays -= iStartDay;

   iDiffDays =(uint32) ((int32)iDiffDays + pme->adjustDay);

    pme->adjustDay = 0;
    return iDiffDays;
}


//返回阴历iLunarYer年阴历iLunarMonth月的天数，如果iLunarMonth为闰月，
//高字为第二个iLunarMonth月的天数，否则高字为0
//1900年1月---2051年12月
static uint32 CScheduleApp_LunarMonthDays(CCalendarApp * pme, uint16 iLunarYear, uint16 iLunarMonth)
{
   uint16 height;
   uint16 low;
   uint8 iBit;

   if(iLunarYear < START_YEAR)
      return 30;

   height = 0;
   low = 29;
   iBit = 16-(uint8)iLunarMonth;

   if(iLunarMonth > CScheduleApp_GetLeapMonth(pme, iLunarYear) && CScheduleApp_GetLeapMonth(pme, iLunarYear))
      iBit --;

   if(pme->gLunarMonthDay[iLunarYear - START_YEAR] & (1<<iBit))
      low ++;

   if(iLunarMonth == CScheduleApp_GetLeapMonth(pme, iLunarYear))
   {
      if(pme->gLunarMonthDay[iLunarYear - START_YEAR] & (1<< (iBit -1)))
         height =30;
      else
         height =29;
   }
   return CScheduleApp_MakeLong(low, height);
}

//返回阴历iLunarYear年的闰月月份，如没有返回0
// 1900年1月---2051年12月
static uint16 CScheduleApp_GetLeapMonth(CCalendarApp * pme, uint16 iLunarYear)
{
   uint8 flag;

   if(iLunarYear < START_YEAR)
      return 0;

   flag = pme->gLunarMonth[(iLunarYear - START_YEAR_M)/2];
   return  (iLunarYear - START_YEAR_M)%2 ? flag&0x0f : flag>>4;   //monthdata start year 1989
}

static uint32 CScheduleApp_MakeLong(uint16 m_Low,uint16 m_High)
{
  uint32 i,j;
  i = (uint32)(m_Low)&0x0000ffff;
  j = (uint32)(m_High)<<16&0xffff0000;
  return (i+j);
}

//取高16位
static uint16 CScheduleApp_HiWord(uint32 m_Num)
{
   return (uint16)(m_Num>>16&0x0000ffff);
}

//取低16位
static uint16 CScheduleApp_LoWord(uint32 m_Num)
{
  return (uint16)(m_Num&0x0000ffff);
}

//判断iYear是不是闰年
static boolean CScheduleApp_IsLeapYear(uint16 iYear)
{
   return !(iYear%4)&&(iYear%100) || !(iYear%400);
}

//取得阴历字符串
void CScheduleApp_GetLunarStr(CScheduleApp *pme)
{
   AECHAR * strTep;
   int  nCount, nLen;
   if(pme == NULL)
   {
      return ;
   }

   pme->m_LunarString[0]=(AECHAR)'\0';
   strTep = pme->m_LunarString;
   nCount = sizeof(pme->m_LunarString) / sizeof(AECHAR);

   //天支地干
   nLen = WSTRNCOPYN(strTep,nCount, pme->m_calendar.m_pResGz,2);
   nCount -= nLen;
   strTep += nLen;

   //加入生肖字符
   nLen = WSTRNCOPYN(strTep,nCount, pme->m_calendar.m_pResSx,2);
   nCount -= nLen;
   strTep += nLen;

   //suspect 润月字符?允静徽?
   //加入月份字符
   if(pme->m_calendar.m_LeapMonth)
   {
       nLen = WSTRNCOPYN(strTep,nCount, pme->m_calendar.m_pResLunM,3);
   }
   else
   {
       nLen = WSTRNCOPYN(strTep,nCount, pme->m_calendar.m_pResLunM,2);
   }
   nCount -= nLen;
   strTep += nLen;

   //加入日子字符
   (void)WSTRNCOPYN(strTep,nCount, pme->m_calendar.m_pResLunD,2);
}

static void CScheduleApp_DrawLunarStr(CScheduleApp *pme)
{
    AEERect rc;
    SETAEERECT(&rc, 0, pme->m_rc.dy - (HEIGHT_PROMPT_BAR + LUNAR_RECT_HEIGHT), pme->m_rc.dx, LUNAR_RECT_HEIGHT);
    
    IDISPLAY_FillRect(pme->m_pDisplay, &rc, LUNAR_BG_COLOR);
    if (WSTRLEN(pme->m_LunarString) > 4)
    {
        RGBVAL nOldFontColor = IDISPLAY_SetColor(pme->m_pDisplay, CLR_USER_TEXT, LUNAR_FONT_COLOR);
        // 绘制农历日期
        (void)IDISPLAY_DrawText(pme->m_pDisplay, AEE_FONT_BOLD,
                    pme->m_LunarString, -1, 0, 0, &rc,
                    IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE | IDF_TEXT_TRANSPARENT);
        (void)IDISPLAY_SetColor(pme->m_pDisplay, CLR_USER_TEXT, nOldFontColor);
    }
}
/*************************************************************************
===>处理阴历的程序结束
*************************************************************************/
#endif
static void ScheduleAppNotifyMP3PlayerAlertEvent(CCalApp *pMe, boolean toStartAlert)
{
#ifdef FEATURE_APP_MUSICPLAYER
    if(GetMp3PlayerStatus() == MP3STATUS_RUNONBACKGROUND)
    {
    (void) ISHELL_SendEvent(pMe->m_pIShell,
                             AEECLSID_APP_MUSICPLAYER,
                             EVT_ALARM,
                             toStartAlert,
                             0
                         );
    }
#endif
}
