/** ----------------------------------------------------------------------------
 *
 *
 * History:
 *
 * when         who            what and why
 * -----------  -----------    -----------------------------
 * 2008-11-3      chunfeng.hou        create
 *
 * -----------------------------------------------------------------------------
 */

#include "recorder_priv.h"


typedef NextFSMActionEnum (*FSMSTATE_HANDLER)( Recorder* pme);

static NextFSMActionEnum state_handler_of_state_none( Recorder* pme);
static NextFSMActionEnum state_handler_of_state_init( Recorder* pme);
static NextFSMActionEnum state_handler_of_state_main( Recorder* pme);
static NextFSMActionEnum state_handler_of_state_record( Recorder* pme);
static NextFSMActionEnum state_handler_of_state_record_list( Recorder* pme);
static NextFSMActionEnum state_handler_of_state_set_as( Recorder* pme);
static NextFSMActionEnum state_handler_of_state_play_msg( Recorder* pme);
static NextFSMActionEnum state_handler_of_state_storage_setup( Recorder* pme);
static NextFSMActionEnum state_handler_of_state_exit( Recorder* pme);
// lizy add for data protect @20140321
#ifdef FEATURE_VERSION_C316
static NextFSMActionEnum Handler_STATE_PWD(Recorder *pMe);
#endif

static const FSMSTATE_HANDLER gFSMStateHandler[] =
{
	state_handler_of_state_none,
	state_handler_of_state_init,
	// lizy add for data protect @20140321
#ifdef FEATURE_VERSION_C316
	Handler_STATE_PWD,
#endif
	state_handler_of_state_main,
	state_handler_of_state_record,
	state_handler_of_state_record_list,
	state_handler_of_state_set_as,
	state_handler_of_state_play_msg,
	state_handler_of_state_storage_setup,
	state_handler_of_state_exit
};


//-------------------------------------------------------------------------------------------------
NextFSMActionEnum Recorder_ProcessState( Recorder* pme)
{

	int size = ARRAY_SIZE( gFSMStateHandler);
	if( (int)pme->m_eCurState < size )
	{
		if( pme->m_eCurState < STATE_NONE || pme->m_eCurState > STATE_EXIT)
		{
			return state_handler_of_state_main( pme);
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

//-------------------------------------------------------------------------------------------------
static NextFSMActionEnum state_handler_of_state_none( Recorder* pme)
{
	MOVE_TO_STATE( STATE_INIT);
	return NFSMACTION_CONTINUE;
}

static NextFSMActionEnum state_handler_of_state_init( Recorder* pme)
{
#ifdef FEATURE_VERSION_C316
	boolean locksel;
#endif
		// liyz add for test @20140321
#ifdef FEATURE_VERSION_C316
	OEM_GetConfig(CFGI_MULTIMEDIA_LOCK_CHECK, &locksel, sizeof( locksel));
	
	DBGPRINTF("Handler_STATE_INIT enter here locksel %d,pwdwright %d",locksel,pme->b_pwdWright);
	if((locksel) && (!pme->b_pwdWright))
	{
		MOVE_TO_STATE(STATE_PWD);
	}
	else
#endif
   {
	MOVE_TO_STATE( STATE_MAIN);
   }
	return NFSMACTION_CONTINUE;
}
// liyz add for test @20140322
#ifdef FEATURE_VERSION_C316
static NextFSMActionEnum Handler_STATE_PWD(Recorder *pme)
{
	MSG_FATAL("Handler_STATE_PWD....",0,0,0);
	switch(pme->m_eDlgRet)
    {
        case DLGRET_CREATE:
			if(SUCCESS != Recorder_ShowDialog(pme, IDD_PWD))
            {
                MOVE_TO_STATE(STATE_EXIT);
                return NFSMACTION_CONTINUE;
            }
            return NFSMACTION_WAIT;

        case DLGRET_CANCELED:
            MOVE_TO_STATE(STATE_EXIT);
            return NFSMACTION_CONTINUE;

        case MGDLGRET_PASS:    
			DBGPRINTF("Handler_STATE_PWD MGDLGRET_PASS");
			MOVE_TO_STATE(STATE_INIT);
            return NFSMACTION_CONTINUE;

        case MGDLGRET_FAILD:    
			MSG_FATAL("MGDLGRET_FAILD.............",0,0,0);
			Recorder_ShowDialog(pme, IDD_PWD_INVAD);
            return NFSMACTION_WAIT;
		
        default:
            MOVE_TO_STATE(STATE_EXIT);
            return NFSMACTION_CONTINUE;
    }
}

#endif

static NextFSMActionEnum state_handler_of_state_main( Recorder* pme)
{

	switch (pme->m_eDlgRet)
	{
		case DLGRET_CREATE:
		{
			pme->m_bNotOverwriteDlgRet = FALSE;
			if( SUCCESS != Recorder_ShowDialog( pme, IDD_MAIN))
			{
				MOVE_TO_STATE( STATE_EXIT);
				return NFSMACTION_CONTINUE;
			}
		}
		return NFSMACTION_WAIT;

		case DLGRET_NEW_RECORD:
			pme->m_Media.m_bRecorder = TRUE;
			MOVE_TO_STATE( STATE_RECORD);
			return NFSMACTION_CONTINUE;

		case DLGRET_RECORD_LIST:
			MOVE_TO_STATE( STATE_RECORD_LIST);
			return NFSMACTION_CONTINUE;

		case DLGRET_CANCELED:
			MOVE_TO_STATE( STATE_EXIT);
			return NFSMACTION_CONTINUE;
		case DLGRET_STORAGE_SETUP:
			MOVE_TO_STATE( STATE_STORAGE_SETUP);
			return NFSMACTION_CONTINUE;
	}

	return NFSMACTION_WAIT;
}

static NextFSMActionEnum state_handler_of_state_record( Recorder* pme)
{

	switch (pme->m_eDlgRet)
	{
		case DLGRET_CREATE:
		{
            int hasCard = IFILEMGR_Test(pme->m_pFileManager, AEEFS_CARD0_DIR);
            if((APP_MEDIA_ALLOW != app_media_scheduler()) || (hasCard != SUCCESS))
            {
                MOVE_TO_STATE(STATE_PLAY_MSG);
                return NFSMACTION_CONTINUE;
            }                    
			pme->m_bNotOverwriteDlgRet = FALSE;
			if( SUCCESS != Recorder_ShowDialog( pme, IDD_RECORD))
			{
				MOVE_TO_STATE( STATE_MAIN);
				return NFSMACTION_CONTINUE;
			}
		}
		return NFSMACTION_WAIT;


		case DLGRET_OK:
		case DLGRET_CANCELED:
			if( pme->m_Media.m_bRecorder)
			{
				MOVE_TO_STATE( STATE_MAIN);
			}
			else
			{
				MOVE_TO_STATE( STATE_RECORD_LIST);
			}
			return NFSMACTION_CONTINUE;
	}

	return NFSMACTION_WAIT;
}

static NextFSMActionEnum state_handler_of_state_record_list( Recorder* pme)
{

	switch (pme->m_eDlgRet)
	{
		case DLGRET_CREATE:
		{			
			pme->m_bNotOverwriteDlgRet = FALSE;
			if( SUCCESS != Recorder_ShowDialog( pme, IDD_RECORD_LIST))
			{
				MOVE_TO_STATE( STATE_MAIN);
				return NFSMACTION_CONTINUE;
			}
		}
		return NFSMACTION_WAIT;


		case DLGRET_CANCELED:
			MOVE_TO_STATE( STATE_MAIN);
			return NFSMACTION_CONTINUE;

		case DLGRET_PLAY:			
			pme->m_Media.m_bRecorder = FALSE;
			MOVE_TO_STATE( STATE_RECORD);
			return NFSMACTION_CONTINUE;

		case DLGRET_SET_AS:
			MOVE_TO_STATE( STATE_SET_AS);
			return NFSMACTION_CONTINUE;
		case DLGRET_PLAY_MSG:
			MOVE_TO_STATE( STATE_PLAY_MSG);
		    return NFSMACTION_WAIT;
            
		}

	return NFSMACTION_WAIT;
}

static NextFSMActionEnum state_handler_of_state_set_as( Recorder* pme)
{

	switch (pme->m_eDlgRet)
	{
		case DLGRET_CREATE:
		{
			pme->m_bNotOverwriteDlgRet = FALSE;
			if( SUCCESS != Recorder_ShowDialog( pme, IDD_SET_AS_RECORD))
			{
				MOVE_TO_STATE( STATE_RECORD_LIST);
				return NFSMACTION_CONTINUE;
			}
		}
		return NFSMACTION_WAIT;


		case DLGRET_CANCELED:
			MOVE_TO_STATE( STATE_RECORD_LIST);
			return NFSMACTION_CONTINUE;
	}

	return NFSMACTION_WAIT;
}

static NextFSMActionEnum state_handler_of_state_play_msg( Recorder* pme)
{
	switch (pme->m_eDlgRet)
	{
        case DLGRET_CREATE:
        Recorder_ShowDialog( pme, IDD_SET_PLAY_MSG);
    	MOVE_TO_STATE( STATE_PLAY_MSG);
        return NFSMACTION_WAIT;
/*
        case DLGRET_OK:
        pme->m_Media.m_bRecorder = FALSE;
		MOVE_TO_STATE( STATE_RECORD);
		return NFSMACTION_CONTINUE;
*/
        case DLGRET_OK:
        MOVE_TO_STATE( STATE_MAIN);
        return NFSMACTION_CONTINUE;   

        case DLGRET_CANCELED:
        MOVE_TO_STATE( STATE_MAIN);
        return NFSMACTION_WAIT;        
	}

    return NFSMACTION_WAIT;
}

static NextFSMActionEnum state_handler_of_state_storage_setup( Recorder* pme)
{
	switch (pme->m_eDlgRet)
	{
		case DLGRET_CREATE:
		{
			pme->m_bNotOverwriteDlgRet = FALSE;
			if( SUCCESS != Recorder_ShowDialog( pme, IDD_STORAGE_SETUP))
			{
				MOVE_TO_STATE( STATE_MAIN);
				return NFSMACTION_CONTINUE;
			}
		}
		return NFSMACTION_WAIT;

		case DLGRET_OK:
		case DLGRET_CANCELED:
			MOVE_TO_STATE( STATE_MAIN);
			return NFSMACTION_CONTINUE;
	}

	return NFSMACTION_WAIT;
}

static NextFSMActionEnum state_handler_of_state_exit( Recorder* pme)
{

	ISHELL_CloseApplet( pme->m_pShell, FALSE);
	return NFSMACTION_WAIT;
}
