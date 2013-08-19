/** ----------------------------------------------------------------------------
 *
 * History:
 *
 * when         who            what and why
 * -----------  -----------    -----------------------------
 * -----------------------------------------------------------------------------
 */


#include "ScheduleApp_priv.h"
typedef NextFSMAction (*FSMSTATE_HANDLER)(CScheduleApp *pme);

static NextFSMAction state_handler_of_state_none(CScheduleApp *pme);
static NextFSMAction state_handler_of_state_init(CScheduleApp *pme);
static NextFSMAction state_handler_of_state_pwd(CScheduleApp *pme);
static NextFSMAction state_handler_of_state_viewmonth(CScheduleApp *pme);
static NextFSMAction state_handler_of_state_option(CScheduleApp *pme);
static NextFSMAction state_handler_of_state_gotodate(CScheduleApp *pme);
static NextFSMAction state_handler_of_state_viewday(CScheduleApp *pme);
static NextFSMAction state_handler_of_state_event_edit(CScheduleApp *pme);
static NextFSMAction state_handler_of_state_inputtext(CScheduleApp *pme);
static NextFSMAction state_handler_of_state_setup(CScheduleApp *pme);
static NextFSMAction state_handler_of_state_showalert(CScheduleApp *pme);
static NextFSMAction state_handler_of_state_viewevent(CScheduleApp *pme);
static NextFSMAction state_handler_of_state_exit(CScheduleApp *pme);

extern uint32 getNowCompositeTime( void);


static const FSMSTATE_HANDLER gFSMStateHandler[] =
{
    state_handler_of_state_none,
    state_handler_of_state_init,
    state_handler_of_state_pwd,
    state_handler_of_state_viewmonth,
    state_handler_of_state_option,
    state_handler_of_state_gotodate,
    state_handler_of_state_viewday,
    state_handler_of_state_event_edit,
    state_handler_of_state_inputtext,
    state_handler_of_state_setup,
    state_handler_of_state_showalert,
    state_handler_of_state_viewevent,
    state_handler_of_state_exit
};


NextFSMAction CScheduleApp_ProcessState(CScheduleApp *pme)
{
    int nSize;

    nSize = ARRAY_SIZE(gFSMStateHandler);
    if ((int)pme->m_eCurState < nSize )
    {
        if( pme->m_eCurState < STATE_NONE || pme->m_eCurState > STATE_EXIT)
        {
            return state_handler_of_state_viewmonth( pme);
        }
        else
        {
            return  gFSMStateHandler[pme->m_eCurState](pme);
        }
    }
    else
    {
        return NFSMACTION_WAIT;
    }
}

static NextFSMAction state_handler_of_state_none(CScheduleApp *pme)
{
    MOVE_TO_STATE(STATE_INIT)
    return NFSMACTION_CONTINUE;
}

static NextFSMAction state_handler_of_state_init(CScheduleApp *pme)
{
    boolean locksel;
    
    OEM_GetConfig( CFGI_CALENDAR_LOCK_CHECK, &locksel, sizeof( locksel));

    if(locksel)
    {
        MOVE_TO_STATE(STATE_PWD)
    }
    else
    {
        MOVE_TO_STATE(STATE_VIEWMONTH)
    }
    return NFSMACTION_CONTINUE;
}

static NextFSMAction state_handler_of_state_pwd(CScheduleApp *pme)
{
    switch(pme->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pme->m_bNotOverwriteDlgRet = FALSE;
            if(SUCCESS != CScheduleApp_ShowDialog(pme, IDD_PWD))
            {
                MOVE_TO_STATE(STATE_EXIT);
                return NFSMACTION_CONTINUE;
            }
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(STATE_EXIT);
            return NFSMACTION_CONTINUE;

        case DLGRET_PASS:
            MOVE_TO_STATE(STATE_VIEWMONTH);
            return NFSMACTION_CONTINUE;

        case DLGRET_FAILD:
            pme->m_wMsgResID = IDS_INVALID;
            if(SUCCESS != CScheduleApp_ShowDialog(pme, IDD_MSGBOX))
            {
                MOVE_TO_STATE(STATE_EXIT);
                return NFSMACTION_CONTINUE;
            }
            return NFSMACTION_WAIT;

        case DLGRET_MSGBOX_OK:
            MOVE_TO_STATE(STATE_PWD);
            return NFSMACTION_CONTINUE;

        case DLGRET_MSGBOX_CANCEL:
            MOVE_TO_STATE(STATE_EXIT);
            return NFSMACTION_CONTINUE;

        default:
            MOVE_TO_STATE(STATE_EXIT);
            return NFSMACTION_CONTINUE;
    }
}

static NextFSMAction state_handler_of_state_viewmonth(CScheduleApp *pme)
{

    switch (pme->m_eDlgRet)
    {
        case DLGRET_CREATE:
        {
            pme->m_bNotOverwriteDlgRet = FALSE;
            if(SUCCESS != CScheduleApp_ShowDialog(pme,IDD_VIEWMONTH))
            {
                MOVE_TO_STATE(STATE_EXIT);
                return NFSMACTION_CONTINUE;
            }
        }
        return NFSMACTION_WAIT;

        case DLGRET_OK:
            MOVE_TO_STATE(STATE_OPTION)
            return NFSMACTION_CONTINUE;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(STATE_EXIT)
            return NFSMACTION_CONTINUE;

        case DLGRET_TO_SHOWALERT:
            MOVE_TO_STATE( STATE_SHOWALERT)
            return NFSMACTION_CONTINUE;
    }

    return NFSMACTION_WAIT;
}

static NextFSMAction state_handler_of_state_gotodate(CScheduleApp *pme)
{
    switch(pme->m_eDlgRet)
    {
        case DLGRET_CREATE:
        case DLGRET_MSGBOX_OK:
        {
            pme->m_bNotOverwriteDlgRet = FALSE;
            if( SUCCESS != CScheduleApp_ShowDialog(pme, IDD_GOTODATE))
            {
                MOVE_TO_STATE(STATE_EXIT);
                return NFSMACTION_CONTINUE;
            }
        }
        return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(STATE_VIEWMONTH)
            return NFSMACTION_CONTINUE;

        case DLGRET_OPTION_VIEWMONTH_VIEW:
            MOVE_TO_STATE(STATE_VIEWMONTH)
            return NFSMACTION_CONTINUE;

        case DLGRET_TO_SHOWALERT:
            pme->m_wMsgResID = IDS_INVALID_YEAR;
            if(SUCCESS != CScheduleApp_ShowDialog(pme, IDD_MSGBOX))
            {
                MOVE_TO_STATE(STATE_EXIT)
                return NFSMACTION_CONTINUE;
            }
            return NFSMACTION_WAIT;

    }
    return NFSMACTION_WAIT;
}


static NextFSMAction state_handler_of_state_option(CScheduleApp *pme)
{
    switch (pme->m_eDlgRet)
    {
        case DLGRET_CREATE:
        {
            pme->m_bNotOverwriteDlgRet = FALSE;
            if (SUCCESS != CScheduleApp_ShowDialog(pme, IDD_OPTION))
            {
                MOVE_TO_STATE(STATE_EXIT);
                return NFSMACTION_CONTINUE;
            }
        }
        return NFSMACTION_WAIT;

        case DLGRET_OPTION_BACK_TO_VIEWMONTH:
        case DLGRET_OPTION_BACK_TO_VIEWDAY:
        {
            FSMState moveTo[] = { STATE_VIEWMONTH, STATE_VIEWDAY};
            MOVE_TO_STATE( moveTo[pme->m_eDlgRet - DLGRET_OPTION_BACK_TO_VIEWMONTH])
        }
        return NFSMACTION_CONTINUE;

        case DLGRET_GOTODATE:
            MOVE_TO_STATE(STATE_GOTODATE)
            return NFSMACTION_CONTINUE;
        
        case DLGRET_OPTION_NEW_EVENT:
            MOVE_TO_STATE(STATE_EVENT_EDIT)
            return NFSMACTION_CONTINUE;

        case DLGRET_OPTION_VIEWMONTH_VIEW:
            MOVE_TO_STATE(STATE_VIEWDAY)
            return NFSMACTION_CONTINUE;

        case DLGRET_OPTION_VIEWDAY_DELETE:
        case DLGRET_OPTION_DELETE_TODAY:
        case DLGRET_OPTION_DELETE_MONTH:
        case DLGRET_OPTION_DELETE_ALL:
            MOVE_TO_STATE( pme->m_stateToBackOption)
            return NFSMACTION_CONTINUE;

        case DLGRET_OPTION_SETUP:
            MOVE_TO_STATE(STATE_SETUP)
            return NFSMACTION_CONTINUE;

        case DLGRET_TO_SHOWALERT:
            MOVE_TO_STATE( STATE_SHOWALERT)
            return NFSMACTION_CONTINUE;

        case DLGRET_OPTION_VIEWDAY_TO_VIEWEVENT:
            MOVE_TO_STATE( STATE_VIEWEVENT)
            return NFSMACTION_CONTINUE;
    }

    return NFSMACTION_WAIT;
}

static NextFSMAction state_handler_of_state_viewday(CScheduleApp *pme)
{

    switch (pme->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pme->m_bNotOverwriteDlgRet = FALSE;
            (void)CScheduleApp_ShowDialog(pme,IDD_VIEWDAY);
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(STATE_VIEWMONTH)
            return NFSMACTION_CONTINUE;

        case DLGRET_OK:
            MOVE_TO_STATE(STATE_OPTION)
            return NFSMACTION_CONTINUE;

        case DLGRET_VIEWDAY_EDIT_EVENT:
            MOVE_TO_STATE(STATE_EVENT_EDIT)
            return NFSMACTION_CONTINUE;


        case DLGRET_TO_SHOWALERT:
            MOVE_TO_STATE( STATE_SHOWALERT)
            return NFSMACTION_CONTINUE;
    }

    return NFSMACTION_WAIT;
} // Handler_STATE_VIEWDAY


static NextFSMAction state_handler_of_state_event_edit(CScheduleApp *pme)
{
    switch (pme->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pme->m_bNotOverwriteDlgRet = FALSE;
            (void)CScheduleApp_ShowDialog(pme,IDD_EVENT_EDIT);
            return NFSMACTION_WAIT;

        case DLGRET_EVENT_EDIT_SUBJECT:
            MOVE_TO_STATE( STATE_INPUTTEXT)
            return NFSMACTION_CONTINUE;

		//Add By zzg 2012_02_01
		case DLGRET_FAILD:
			pme->m_wMsgResID = IDS_VALID_DATE;
            if(SUCCESS != CScheduleApp_ShowDialog(pme, IDD_MSGBOX))
            {
                MOVE_TO_STATE(STATE_EXIT);
                return NFSMACTION_CONTINUE;
            }
            return NFSMACTION_WAIT;
		//Add End

        case DLGRET_OK:
            pme->m_wMsgResID = IDS_DONE;
            if(SUCCESS != CScheduleApp_ShowDialog(pme, IDD_MSGBOX))
            {
                MOVE_TO_STATE(STATE_EXIT);
                return NFSMACTION_CONTINUE;
            }
            return NFSMACTION_WAIT;

        case DLGRET_MSGBOX_OK:
        case DLGRET_CANCELED:
            MOVE_TO_STATE( pme->m_stateToBackEventEdit);
            return NFSMACTION_CONTINUE;

        case DLGRET_TO_SHOWALERT:
            MOVE_TO_STATE( STATE_SHOWALERT)
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
}

static NextFSMAction state_handler_of_state_inputtext(CScheduleApp *pme)
{
    switch (pme->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pme->m_bNotOverwriteDlgRet = FALSE;
            (void)CScheduleApp_ShowDialog(pme,IDD_INPUTTEXT);
            return NFSMACTION_WAIT;

        case DLGRET_OK:
        case DLGRET_CANCELED:
            MOVE_TO_STATE( STATE_EVENT_EDIT)
            return NFSMACTION_CONTINUE;

        case DLGRET_TO_SHOWALERT:
            MOVE_TO_STATE( STATE_SHOWALERT)
            return NFSMACTION_CONTINUE;


        default:
            break;
    }

    return NFSMACTION_WAIT;
}

static NextFSMAction state_handler_of_state_setup(CScheduleApp *pme)
{
    switch (pme->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pme->m_bNotOverwriteDlgRet = FALSE;
            if(SUCCESS != CScheduleApp_ShowDialog(pme,IDD_SETUP))
            {
                MOVE_TO_STATE(STATE_EXIT);
                return NFSMACTION_CONTINUE;
            }
            return NFSMACTION_WAIT;

        case DLGRET_OK:
            pme->m_wMsgResID = IDS_DONE;
            if(SUCCESS != CScheduleApp_ShowDialog(pme, IDD_MSGBOX))
            {
                MOVE_TO_STATE(STATE_EXIT);
                return NFSMACTION_CONTINUE;
            }
            return NFSMACTION_WAIT;

        case DLGRET_MSGBOX_OK:
        case DLGRET_CANCELED:
            MOVE_TO_STATE(pme->m_stateToBackSetup)
            return NFSMACTION_CONTINUE;

        case DLGRET_TO_SHOWALERT:
            MOVE_TO_STATE( STATE_SHOWALERT)
            return NFSMACTION_CONTINUE;

    }
    return NFSMACTION_WAIT;
}


static NextFSMAction state_handler_of_state_showalert(CScheduleApp *pme)
{
    switch (pme->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pme->m_bNotOverwriteDlgRet = FALSE;
            (void)CScheduleApp_ShowDialog(pme,IDD_SHOWALERT);
            return NFSMACTION_WAIT;

        case DLGRET_OK:
        case DLGRET_CANCELED:
            //debug( ";state_handler_of_state_showalert");
            if( pme->m_stateToBackShowAlert == STATE_NONE || pme->m_stateToBackShowAlert == STATE_INIT)
            {
                //debug( ";state_none or state_init, to exit");
                MOVE_TO_STATE( STATE_EXIT)
//                MOVE_TO_STATE( STATE_VIEWMONTH)
            }
            else
            {
                //debug( ";back to %d", pme->m_stateToBackShowAlert);
                //MOVE_TO_STATE( pme->m_stateToBackShowAlert);
                MOVE_TO_STATE( STATE_EXIT)
            }
            return NFSMACTION_CONTINUE;

        case DLGRET_SHOWALERT_TO_VIEWEVENT:
            MOVE_TO_STATE( STATE_VIEWEVENT)
            return NFSMACTION_CONTINUE;


        default:
            break;
    }

    return NFSMACTION_WAIT;
}

static NextFSMAction state_handler_of_state_viewevent(CScheduleApp *pme)
{

    switch (pme->m_eDlgRet)
    {
        case DLGRET_CREATE:
            pme->m_bNotOverwriteDlgRet = FALSE;
            (void)CScheduleApp_ShowDialog(pme,IDD_VIEWEVENT);
            return NFSMACTION_WAIT;


        case DLGRET_OK:
        case DLGRET_CANCELED:
            if(pme->m_bshowEventDetail)
            {
                if( pme->m_stateToBackViewEvent == STATE_NONE || pme->m_stateToBackViewEvent == STATE_INIT)
                {
                    MOVE_TO_STATE( STATE_EXIT)                    
                }
                else
                {
                    MOVE_TO_STATE( pme->m_stateToBackShowAlert);                    
                }
                pme->m_bshowEventDetail = FALSE;
            }
            else
            {            
                MOVE_TO_STATE( pme->m_stateToBackViewEvent);      
            }
            return NFSMACTION_CONTINUE;

        default:
            break;
    }

    return NFSMACTION_WAIT;
}


static NextFSMAction state_handler_of_state_exit(CScheduleApp *pme)
{

    //debug( ";**********state_handler_of_state_exit");
    (void) ISHELL_CloseApplet(pme->m_pShell, FALSE);
    //debug( ";after close applet");
    return NFSMACTION_WAIT;
}
