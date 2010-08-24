#ifndef _HSU_AL_SICD_H_
#define _HSU_AL_SICD_H_

#include <uw_device_sicd.h>
#include <jos.h>
#include "fs.h"  /* AMSS file operations */


#define SICD_MAX_STORES 8

#ifdef CONFIG_FD_MTP_APP
typedef struct 
{
    mtp_obj_prop_t           prop_code;
    mtp_obj_prop_desc_t     *desc;
    ptp_prop_value_t        *current_value;
} obj_proper_t;
#endif

typedef struct _sicd_object
{
    ptp_obj_handle_t id;
    ptp_obj_info_t info;
    juint8_t *data;
    juint8_t *sample_data;
    char      file_path[DPS_FILEPATH_SIZE+1];
#ifdef CONFIG_FD_MTP_APP
    obj_proper_t       *obj_props;
    juint32_t           obj_props_count;
    ptp_obj_handle_t   *ref_handles;
    juint32_t           ref_handles_count;
#endif
    struct _sicd_object *next;
} sicd_object_t;

typedef struct _sicd_store
{
    ptp_storage_id_t id;
    ptp_storage_info_t  info;
    sicd_object_t      *objects;
    struct _sicd_store *next;
} sicd_store_t;


jresult_t sicd_store_add(ptp_storage_id_t id, ptp_storage_info_t *info);

jresult_t sicd_store_remove(ptp_storage_id_t id);

jresult_t sicd_store_add_object(ptp_storage_id_t store_id, sicd_object_t *obj);

jresult_t sicd_store_remove_object(ptp_storage_id_t store_id,
    ptp_obj_handle_t obj_id);

jresult_t sicd_store_remove_all_objects(ptp_storage_id_t store_id); /* dk */

sicd_store_t* sicd_find_store(ptp_storage_id_t store_id);

sicd_object_t *sicd_store_find_object(ptp_storage_id_t store_id,
    ptp_obj_handle_t obj_id);

ptp_obj_handle_t sicd_get_new_obj_handle(void);

sicd_store_t* sicd_store_get_first(void);

void sicd_free_object(sicd_store_t *store, sicd_object_t *obj);



/*==============================================================================

                         file op functions

==============================================================================*/

uint32 sicd_fs_file_size(const char * filepath);

/* Returns valid file handle for success */
fs_handle_type sicd_fs_file_open(const char * filepath);

/* Returns FS_OKAY_S for success */
fs_status_type sicd_fs_file_close(fs_handle_type hFile);

/* Returns bytes read */   
uint32 sicd_fs_file_read(fs_handle_type hFile, void * buffer, uint32 nBytes);

/* Returns FS_OKAY_S for success */
fs_status_type sicd_fs_file_seek(fs_handle_type hFile, uint32 nOffset);


    
/* TODO : add more file operation functions write */




#endif /* _HSU_AL_SICD_H_ */

