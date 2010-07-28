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

static const FSMSTATE_HANDLER gFSMStateHandler[] =
{
	state_handler_of_state_none,
	state_handler_of_state_init,
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

	MOVE_TO_STATE( STATE_MAIN);
	return NFSMACTION_CONTINUE;
}

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

        case DLGRET_OK:
        pme->m_Media.m_bRecorder = FALSE;
		MOVE_TO_STATE( STATE_RECORD);
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
