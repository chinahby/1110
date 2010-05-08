#include "OEMHelperFuncType.h"
#ifdef WIN32
static db_items_value_type DB_CAPTURE_WALLPER_item;
void db_put(db_items_type  item,db_items_value_type  *value_ptr )
{
	if((item == DB_CAPTURE_WALLPER) && (value_ptr))
	{
		DB_CAPTURE_WALLPER_item = *value_ptr;
	}
	return;
}
void db_get
(
  db_items_type         item,               
  db_items_value_type  *value_ptr        
)
{
	if((item == DB_CAPTURE_WALLPER) && (value_ptr))
	{
		*value_ptr = DB_CAPTURE_WALLPER_item;
	}
	return ;
}
boolean OEMRTC_Process_Auto_Power_On(void)
{
	return TRUE;
}
boolean cm_ph_cmd_oprt_mode(

  void *    cmd_cb_func,

  void                   *data_block_ptr,

  int      client_id,

  int    oprt_mode

)
{
	return TRUE;
}
 char ver_modelname[]  = {'a'};

// Product Build Version
 char ver_modelversion[]  = {'b'};
NOTICE_TYPE ver_date[] = {'0'};
NOTICE_TYPE ver_time[] = {'0'};

void mmi_g_sensor_process(g_sensor_cmd_type num)
{
	return;
}
int ReadPedoCounter(void)
{
	return 0;
}
void MMD_LCDRotate(int i)
{
	return;
}
void disp_set_contrast(word contrast)
{
	return;
}
#endif


