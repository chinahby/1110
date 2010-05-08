
#include "bt_ui_int.h"
#include "bt_ui_ht_common.h"
#include "bd.h"


/* functions related to device data base */


/* Load the device db from nv. */
void bt_ui_init_device_db (void)
{
	IFile *pif;
	tBTUI_DEV_DB *p_device_db;

	pif = bt_ui_open_file(BT_DEV_DB_FILE, _OFM_READ);	
	if(NULL == pif)
	{
		bt_ui_nv_init_device_db();
		return ;
	}	
	
	p_device_db = bt_ui_get_device_db();
	if(p_device_db)
	{
		uint32 cbOffset = 0;		
		uint32 read_size =0;
		//cbOffset = BTUI_OffsetOf(tBTUI_DEV_DB, dev_tle);
		cbOffset = sizeof(tBTUI_DEV_DB);
		read_size = (uint32)IFILE_Read(pif, (void *)p_device_db, cbOffset);
		BCM_MSG_MED("bt_ui_init_device_db %d %d",cbOffset,read_size,0);
		if(cbOffset != read_size)
		{      
			MEMSET(p_device_db, 0, cbOffset);
			IFILE_Release(pif);
			pif = NULL;
			bt_ui_remove_file(BT_DEV_DB_FILE);
		}
		//MEMSET(&p_device_db->dev_tle, 0, sizeof(TIMER_LIST_ENT));
	}
	if(pif != NULL)
	{
		IFILE_Release(pif);
	}
}

/* store the device visibility member. */
void bt_ui_store_visibility_setting(BOOLEAN visibility)
{	
	IFile *pif;
	tBTUI_DEV_DB *p_device_db;	
	int32 cbOffset;

	pif = bt_ui_open_file(BT_DEV_DB_FILE, _OFM_READWRITE);	
	if(NULL == pif)
		return ;

	p_device_db = bt_ui_get_device_db();
	if(p_device_db)
	{
		p_device_db->visibility = visibility;

		cbOffset = BTUI_OffsetOf(tBTUI_DEV_DB, visibility);
		if(SUCCESS == IFILE_Seek(pif, _SEEK_START, cbOffset))
			IFILE_Write(pif, (void *)&p_device_db->visibility, sizeof(p_device_db->visibility));
	}

	IFILE_Release(pif);
}

/* store the device enable member. */
void bt_ui_store_bt_enable_setting(BOOLEAN enabled)
{
	IFile *pif;
	tBTUI_DEV_DB *p_device_db;	
// 	int32 cbOffset;

	pif = bt_ui_open_file(BT_DEV_DB_FILE, _OFM_READWRITE);	
	if(NULL == pif)
		return ;

	p_device_db = bt_ui_get_device_db();
	if(p_device_db)
	{
		p_device_db->bcm_enabled = enabled;
// 		cbOffset = BTUI_OffsetOf(tBTUI_DEV_DB, bcm_enabled);
// 		if(SUCCESS == IFILE_Seek(pif, cbOffset, _SEEK_START))
// 			IFILE_Write(pif, (void *)&p_device_db->enabled, sizeof(p_device_db->enabled));
		IFILE_Write(pif, (void *)&p_device_db->bcm_enabled, sizeof(p_device_db->bcm_enabled));
	}

	IFILE_Release(pif);
}

/* store the local name of the bluetooth device. */
void bt_ui_store_local_name(char * p_name)
{
	IFile *pif;
	tBTUI_DEV_DB *p_device_db;
	int32 cbOffset;

	if(NULL == p_name)
		return ;

	pif = bt_ui_open_file(BT_DEV_DB_FILE, _OFM_READWRITE);	
	if(NULL == pif)
		return ;

	p_device_db = bt_ui_get_device_db();
	if(p_device_db)
	{
		bt_ui_str_cpyn(p_device_db->local_device_name, BTUI_DEV_NAME_LENGTH, p_name);

		cbOffset = BTUI_OffsetOf(tBTUI_DEV_DB, local_device_name);
		if(SUCCESS == IFILE_Seek(pif, _SEEK_START, cbOffset))
			IFILE_Write(pif, (void *)p_device_db->local_device_name, STRLEN(p_device_db->local_device_name)+1);
	}

	IFILE_Release(pif);
}

/* store the peer device count member. */
#if 0
void bt_ui_store_device_count(UINT8 count)
{
	IFile *pif;
	tBTUI_DEV_DB *p_device_db;	
	int32 cbOffset;

	pif = bt_ui_open_file(BT_DEV_DB_FILE, _OFM_READWRITE);	
	if(NULL == pif)
		return ;

	p_device_db = bt_ui_get_device_db();
	if(p_device_db)
	{
		p_device_db->count = count;

		cbOffset = BTUI_OffsetOf(tBTUI_DEV_DB, count);
		if(SUCCESS == IFILE_Seek(pif, _SEEK_START, cbOffset))
			IFILE_Write(pif, (void *)&p_device_db->count, sizeof(p_device_db->count));
	}

	IFILE_Release(pif);
}
#endif
/* init the nv item for bluetooth devices. */
void bt_ui_nv_init_device_db(void)
{
	IFile *pif;
	tBTUI_DEV_DB *p_device_db;		

	pif = bt_ui_create_file(BT_DEV_DB_FILE);
	if(pif)
	{
		p_device_db = bt_ui_get_device_db();
		if(p_device_db)
		{
			MEMSET(p_device_db, 0, sizeof(tBTUI_DEV_DB));
			STRCPY(p_device_db->local_device_name, "(C)Anylook Tech");
#ifdef FEATURE_BLUETOOTH_DEVICE_VISIBILITY
			p_device_db->visibility = TRUE;
#endif
			IFILE_Write(pif, (void *)p_device_db, sizeof(tBTUI_DEV_DB));
		}	

		IFILE_Release(pif);
	}	
}

/* store the struct contain all the device info to nv. */
void bt_ui_nv_store_device_db(void)
{
	IFile *pif;
	tBTUI_DEV_DB *p_device_db;		

	pif = bt_ui_open_file(BT_DEV_DB_FILE, _OFM_READWRITE);	
	if(NULL == pif)
		return ;

	p_device_db = bt_ui_get_device_db();
	if(p_device_db)		
		IFILE_Write(pif, (void *)p_device_db, sizeof(tBTUI_DEV_DB));/*rewrite all*/

	IFILE_Release(pif);
}


#if 0
void bt_ui_nv_init_wug_db(void)
{
}

void bt_ui_nv_store_wug_db(void)
{
}
#endif

#if( defined BTA_FM_INCLUDED ) && (BTA_FM_INCLUDED == TRUE)
void bt_ui_nv_store_fm_db(void)
{
}

void bt_ui_nv_init_fm_db(void)
{
}
#endif

/* get the record by bd_addr from inquiry cache db. */
tBTUI_REM_DEVICE * bt_ui_get_inquiry_record(BD_ADDR bd_addr)
{
	tBTUI_REM_DEVICE *p_rem_dev;
	tBTUI_INQ_DB *p_inq_db;
	
	p_inq_db = bt_ui_get_inq_db();
	if(p_inq_db)
	{
		UINT8 i = p_inq_db->rem_index;

		while(i--)
		{
			p_rem_dev = &p_inq_db->remote_device[i];
			if(p_rem_dev->in_use && !bdcmp(p_rem_dev->bd_addr, bd_addr))
				return p_rem_dev;
		}
	}

	return NULL;
}

/* get the record by bd_addr from device cache db. */
tBTUI_REM_DEVICE * bt_ui_get_device_record(BD_ADDR bd_addr)
{
	tBTUI_DEV_DB *p_device_db = bt_ui_get_device_db();

	if(p_device_db)
	{
		tBTUI_REM_DEVICE *p_rem_dev;
		UINT8 i = 0;

		//while(i--)
		for(i=0; i<BTUI_NUM_REM_DEVICE; i++)
		{
			p_rem_dev = &p_device_db->device[i];
			if(p_rem_dev->in_use && !bdcmp(p_rem_dev->bd_addr, bd_addr))
			{
				return p_rem_dev;
			}
		}
	}

	return NULL;
}

/* alloc a record space from device cache db. */
tBTUI_REM_DEVICE * bt_ui_alloc_device_record(BD_ADDR bd_addr)
{	
	tBTUI_DEV_DB *p_device_db = bt_ui_get_device_db();

	if(p_device_db)
	{	
		tBTUI_REM_DEVICE *p_rem_dev;
		UINT8 i = 0;

		//while(i--)
		for(i=0; i<BTUI_NUM_REM_DEVICE; i++)
		{
			p_rem_dev = &p_device_db->device[i];
			if(!p_rem_dev->in_use)
			{
				memset(p_rem_dev, 0, sizeof(tBTUI_REM_DEVICE));
				p_rem_dev->in_use = TRUE;
				bdcpy(p_rem_dev->bd_addr, bd_addr);
				return p_rem_dev;
			}
		}
#if 0
		if(p_device_db->count < BTUI_NUM_REM_DEVICE)
		{
			p_rem_dev = &p_device_db->device[p_device_db->count];
			// Just save the count member
			bt_ui_store_device_count(++p_device_db->count);

			// We just put the device info in the memory, and if we want to save the device info in the file,
			// the function bt_ui_store_device must be called .
			MEMSET(p_rem_dev, 0, sizeof(tBTUI_REM_DEVICE));
			p_rem_dev->in_use = TRUE;
			bdcpy(p_rem_dev->bd_addr, bd_addr);
			return p_rem_dev;
		}
#endif
	}	

	return NULL;
}

/* get the device name by bd_addr from device db. */
//char * bt_ui_get_dev_name(BD_ADDR bd_addr)
//{
//	tBTUI_REM_DEVICE *p_rem_dev = bt_ui_get_device_record(bd_addr);

//	if(p_rem_dev)
//		return p_rem_dev->name;

//	return NULL;
//}

/* store the device record to nv. */
BOOLEAN bt_ui_store_device( tBTUI_REM_DEVICE * p_rem_device)
{
	IFile *pif;
	tBTUI_DEV_DB *p_device_db;
	uint32 cbWrite = 0;
	int32 cbOffset;
	UINT8 i;//, count;
	boolean bSuccess = FALSE;

	pif = bt_ui_open_file(BT_DEV_DB_FILE, _OFM_READWRITE);	
	if(NULL == pif)
	{   
		return FALSE;
	}
	p_device_db = bt_ui_get_device_db();
	if(p_device_db)
	{
        	//count = p_device_db->count;
        	for(i=0; i<BTUI_NUM_REM_DEVICE; i++)
        	{			
        		if(p_device_db->device[i].in_use && p_rem_device == &p_device_db->device[i])
        		{	
        			//memcpy(&p_device_db->device[i], p_rem_device, sizeof(tBTUI_REM_DEVICE));//wuraojin 20090206,no need

        			cbOffset = BTUI_OffsetOf(tBTUI_DEV_DB, device);
        			cbOffset += i * sizeof(tBTUI_REM_DEVICE);
        			if(SUCCESS == IFILE_Seek(pif, _SEEK_START, cbOffset))
        			{
        				cbWrite = IFILE_Write(pif, p_rem_device, sizeof(tBTUI_REM_DEVICE));
        			}
        			bSuccess = (cbWrite == sizeof(tBTUI_REM_DEVICE));

        			IFILE_Release(pif);
        			return bSuccess;
        		}	
        	}
       }
	IFILE_Release(pif);
	return bSuccess;
    
}
#if 0
BOOLEAN bt_ui_store_device_ex( tBTUI_REM_DEVICE * p_rem_device)
{
	IFile *pif;
	tBTUI_DEV_DB *p_device_db;
	uint32 cbWrite = 0;
	int32 cbOffset;
	UINT8 i = 0; 
	UINT8 count;
	boolean bSuccess = FALSE;

	pif = bt_ui_open_file(BT_DEV_DB_FILE, _OFM_READWRITE);	
	if(NULL == pif)
		return FALSE;

	p_device_db = bt_ui_get_device_db();
	if(p_device_db)
	{
		count = p_device_db->count;
		for(i=0; i<count; i++)
		{			
			if(p_device_db->device[i].in_use && 
			!bdcmp(p_device_db->device[i].bd_addr, p_rem_device->bd_addr) )
			{
				goto LCopyAndWrite;
			}
		}

		for(i=0; i<count; i++)
		{
			if(!p_device_db->device[i].in_use)
			{
				p_device_db->device[i].in_use = TRUE;
				goto LCopyAndWrite;
			}	
		}

		if(i >= BTUI_NUM_REM_DEVICE)
		{
			goto LError;
		}

		// Increase the count member and save it.
		p_device_db->count++;
		cbOffset = BTUI_OffsetOf(tBTUI_DEV_DB, count);
		if(SUCCESS == IFILE_Seek(pif, _SEEK_START, cbOffset))
		{			
			cbWrite = sizeof(p_device_db->count);
			if( cbWrite != IFILE_Write(pif, (void *)&p_device_db->count, cbWrite) )
			{
				goto LError;
			}
		}
	}

LCopyAndWrite:

	MEMCPY(&p_device_db->device[i], p_rem_device, sizeof(tBTUI_REM_DEVICE));

	cbOffset = BTUI_OffsetOf(tBTUI_DEV_DB, device);
	cbOffset += i * sizeof(tBTUI_REM_DEVICE);
	if(SUCCESS == IFILE_Seek(pif, _SEEK_START, cbOffset))
	{
		cbWrite = IFILE_Write(pif, p_rem_device, sizeof(tBTUI_REM_DEVICE));
	}
	bSuccess = (cbWrite == sizeof(tBTUI_REM_DEVICE));
LError:

	IFILE_Release(pif);
	return bSuccess;
}
#endif
/* delete the device record in the device db. */
BOOLEAN bt_ui_delete_device(BD_ADDR bd_addr)
{
	IFile *pif;
	tBTUI_DEV_DB *p_device_db;
	boolean succ = FALSE;
	pif = bt_ui_open_file(BT_DEV_DB_FILE, _OFM_READWRITE);	
	if(NULL == pif)
	{   
		return FALSE;
	}
	p_device_db = bt_ui_get_device_db();
	if(p_device_db)
	{		
		//UINT8 count = p_device_db->count;
		UINT8 i;
		int32 cbOffset = 0;
		BCM_FUN_STAR("bt_ui_delete_device %x", p_device_db,0,0);
		cbOffset = BTUI_OffsetOf(tBTUI_DEV_DB, device);
		for(i=0; i<BTUI_NUM_REM_DEVICE; i++)
		{
			tBTUI_REM_DEVICE *p_rem_device = &p_device_db->device[i];
			if(p_rem_device->in_use && !bdcmp(p_rem_device->bd_addr, bd_addr))
			{
				cbOffset += i * sizeof(tBTUI_REM_DEVICE);
				//cbOffset += BTUI_OffsetOf(tBTUI_REM_DEVICE, in_use);
				//p_rem_device->in_use = FALSE;
				memset(p_rem_device,0,sizeof(tBTUI_REM_DEVICE));
				//p_device_db->count--;
				if(SUCCESS == IFILE_Seek(pif, _SEEK_START, cbOffset))
				{
					uint32 mm = 0;
					mm = IFILE_Write(pif, p_rem_device, sizeof(tBTUI_REM_DEVICE));
					BCM_MSG_MED("bt_ui_delete_device_succ %x %d", p_rem_device,mm,0);
					succ =  TRUE;
				}
                		//cbOffset = BTUI_OffsetOf(tBTUI_DEV_DB, count);
                		//if(SUCCESS == IFILE_Seek(pif, _SEEK_START, cbOffset))
                		//	IFILE_Write(pif, (void *)&p_device_db->count, sizeof(p_device_db->count));
			}			
		}
	}

	IFILE_Release(pif);
	return succ;
}

/* delete all the device record in the device db. */
//void bt_ui_delete_all_stored_device(void)
//{	
//	bt_ui_store_device_count(0);
//}


/* set the platform root path for access the files. */
//void bt_ui_platform_set_root_path(char *root_path, uint16 path_len)
//{
//	// copy the root path to root_path
//	STRCPY(root_path, "/brew/");
//}

///////////////////////////////// File End /////////////////////////////////////////