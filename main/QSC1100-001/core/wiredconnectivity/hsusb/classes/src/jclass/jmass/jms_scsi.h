/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.











************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#ifndef _JMS_SCSI_H_
#define _JMS_SCSI_H_

#include <jos.h>

/* Timeout values */
#define SCSI_READ_SECTOR_TIMEOUT        5000
#define SCSI_WRITE_SECTOR_TIMEOUT       5000
#define SCSI_COMMAND_TIMEOUT            2000

#define UNIT_ATTENTION_REPEAT           10
#define WIRE_FAILURE_RETRIES            3
#define CBI_GET_STATUS_RETRIES          10

#define USBMSC_INTERFACE_CLASS      0x08
#define USBMSC_SUBCLASS_RBC         0x01
#define USBMSC_SUBCLASS_SFF8020i    0x02
#define USBMSC_SUBCLASS_QIC157      0x03
#define USBMSC_SUBCLASS_UFI         0x04
#define USBMSC_SUBCLASS_SFF8070i    0x05
#define USBMSC_SUBCLASS_SCSI        0x06

#define MAX_LUN     0x7
#define DATA_OUT 0x00000000
#define DATA_IN  0x00000001

/* Default sizes */             
#define FAT_SECTOR_SIZE                 512
#define CDSECTOR_SIZE                   2048
#define MAX_SECTOR_SIZE                 CDSECTOR_SIZE
#define SG_BUFF_SIZE                    65536
#define SCSI_CDB_6                      6
#define SCSI_CDB_10                     10
#define UFI_CDB                         12
#define ATAPI_CDB                       12
#define MAX_CDB    UFI_CDB

/* SCSI-2 device types */
#define SCSI_DEVICE_DIRECT_ACCESS       0x00
#define SCSI_DEVICE_SEQUENTIAL_ACCESS   0x01
#define SCSI_DEVICE_PRINTER             0x02
#define SCSI_DEVICE_PROCESSOR           0x03
#define SCSI_DEVICE_WRITE_ONCE          0x04
#define    SCSI_DEVICE_CDROM            0x05
#define    SCSI_DEVICE_SCANNER          0x06
#define SCSI_DEVICE_OPTICAL             0x07
#define SCSI_DEVICE_MEDIUM_CHANGER      0x08
#define SCSI_DEVICE_COMM                0x09
#define SCSI_DEVICE_UNKNOWN             0x1F

/* SCSI-2 medium types */
#define SCSI_MEDIUM_UNKNOWN             0xFF

/* CD-ROM medium types */
#define MEDIUM_CD_ROM_UNKNOWN           0x00
#define MEDIUM_CD_ROM_120               0x01
#define MEDIUM_CD_DA_120                0x02
#define    MEDIUM_CD_MIXED_120          0x03
#define    MEDIUM_CD_HYBRID_120         0x04
#define    MEDIUM_CD_ROM_80             0x05
#define    MEDIUM_CD_DA_80              0x06
#define MEDIUM_CD_MIXED_80              0x07
#define MEDIUM_CD_HYBRID_80             0x08
#define MEDIUM_CDR_ROM_UNKNOWN          0x10
#define MEDIUM_CDR_ROM_120              0x11
#define MEDIUM_CDR_DA_120               0x12
#define MEDIUM_CDR_MIXED_120            0x13
#define MEDIUM_CDR_HYBRID_120           0x14
#define MEDIUM_CDR_ROM_80               0x15
#define MEDIUM_CDR_DA_80                0x16
#define MEDIUM_CDR_MIXED_80             0x17

/* Mandatory SCSI-2 commands */
#define SCSI_TEST_UNIT_READY            0x00
#define SCSI_REQUEST_SENSE              0x03
#define SCSI_INQUIRY                    0x12
#define SCSI_SEND_DIAGNOSTIC            0x1D

/* Mandatory block SCSI-2 commands */
#define SCSI_READ10                     0x28
#define SCSI_READ_CAPACITY              0x25

/* Optional block SCSI-2 commands */
#define    SCSI_MODE_SELECT6            0x15
#define SCSI_MODE_SENSE6                0x1A
#define SCSI_START_STOP                 0x1B
#define SCSI_WRITE10                    0x2A
#define SCSI_MODE_SELECT10              0x55
#define SCSI_MODE_SENSE10               0x5A

/* ATAPI (CD-ROM) commands */
#define SCSI_CD_READ_TOC                0x43
#define SCSI_CD_PLAY10                  0x45
#define SCSI_CD_PLAY_MSF                0x47
#define SCSI_CD_PAUSE_RESUME            0x4B
#define SCSI_CD_STOP                    0x4E

/* Mode pages */
#define MODE_PAGE_FLEXIBLE_DISK         0x05
#define MODE_PAGE_CDROM                 0x0D
#define MODE_PAGE_CDROM_AUDIO           0x0E
#define MODE_PAGE_CDROM_CAPS            0x2A

/* SCSI-2 sense keys */
#define SENSE_NONE                      0x00
#define SENSE_RECOVERED_ERROR           0x01
#define SENSE_NOT_READY                 0x02
#define SENSE_MEDIUM_ERROR              0x03
#define SENSE_HARDWARE_ERROR            0x04
#define SENSE_ILLEGAL_REQUEST           0x05
#define SENSE_UNIT_ATTENTION            0x06
#define SENSE_DATA_PROTECT              0x07
#define SENSE_BLANK_CHECK               0x08

/* SCSI-2 ASC */
#define ASC_LUN                         0x04
#define ASC_INVALID_COMMAND_FIELD       0x24
#define ASC_MEDIA_CHANGED               0x28
#define ASC_RESET                       0x29
#define ASC_COMMANDS_CLEARED            0x2F
#define ASC_MEDIUM_NOT_PRESENT          0x3A

#define START        TRUE
#define STOP        FALSE

#define VENDOR_ID_LEN                   8
#define PRODUCT_ID_LEN                  16
#define MIN_INQUIRY_LEN                 36
#define MIN_SENSE_LEN                   13
#define MAX_SENSE_LEN                   72
#define MAX_CMD_DATA_LEN                72

typedef struct {
    juint_t removable                  : 1;
    juint_t write_protected            : 1;
    juint_t open                       : 1;
    juint_t pending_removal            : 1; 
    juint_t suspended                  : 1;
    juint_t busy                       : 1;
    juint_t medium_present             : 1;
    juint_t medium_changed             : 1;
    juint_t prev_medium_status         : 1; 
    juint_t media_polling_sched        : 1;
    juint_t media_polling_running      : 1;
    juint_t media_notify_done          : 1;
    juint_t media_inserted             : 1;
} dev_flags_t;

typedef __PACKED_PRE__ struct
{
    unsigned char boot_ind;    /* 0x80 - active */
    unsigned char head;        /* starting head */
    unsigned char sector;    /* starting sector */
    unsigned char cyl;        /* starting cylinder */
    unsigned char sys_ind;    /* What partition type */
    unsigned char end_head;    /* end head */
    unsigned char end_sector;    /* end sector */
    unsigned char end_cyl;    /* end cylinder */
    juint32_t start_sect;    /* starting sector counting from 0 */
    juint32_t nr_sects;        /* nr of sectors in partition */
}__PACKED_POST__ ms_partition_t;

typedef struct ms_device_t
{
    juint8_t    subclass;
    juint8_t    devtype;
    juint8_t    lun;

    juint8_t    medium_type;
    juint8_t    opencount;
    juint8_t    remove_lock; 

    juint32_t   sector_count;
    juint32_t   sector_size;

    juint8_t    heads;
    juint8_t    sectors;
    juint32_t   cylinders;

    dev_flags_t    flags;

    juint8_t    vendor[VENDOR_ID_LEN + 1]; /* ASCI vendor ID */
    juint8_t    product[PRODUCT_ID_LEN + 1]; /* ASCI product ID */
    juint16_t   release;

    void        *usbd_dev;
    void        *devmgr;
    void        *dev_specific;
    void        *transport;
    void        *upper_context;
    jresult_t    (*attach)(struct ms_device_t *);
    void        (*detach)(struct ms_device_t *);

    jtask_h     media_poll_task;
} ms_device_t;

/* Used if SCSI command needs to read/write data */
typedef struct {
    juint32_t    transfer_size;
    juint32_t    bytes_transfered;
    juint8_t    *buffer;
    juint8_t     flags;
} scsi_data_pkt_t;

typedef struct {
    juint32_t    timeout;
    juint32_t    size;
    juint8_t    direction;  
    juint8_t    lun;
    juint8_t    *cmd;
} scsi_cmd_t;

jresult_t scsi_get_sense_data(ms_device_t *device);
jresult_t scsi_inquiry(ms_device_t *device);
jresult_t scsi_mode_sense_10(ms_device_t *device);
jresult_t scsi_mode_sense_6(ms_device_t *device);
jresult_t scsi_read_capacity(ms_device_t *device);
jresult_t scsi_read_partsize(ms_device_t *device);
jresult_t scsi_read_write(ms_device_t *device, juint32_t offset_sector,
    juint32_t num_sectors, juint8_t *buffer, juint8_t flags, 
    juint32_t *transferred, jint_t direction);
jresult_t scsi_request_sense(ms_device_t *device, scsi_data_pkt_t *data);
jresult_t scsi_send_diagnostic(ms_device_t *device);
jresult_t scsi_start_stop_unit(ms_device_t *device, jint_t start, jint_t eject);
jresult_t scsi_test_unit_ready(ms_device_t *device);
jresult_t scsi_unit_attention(ms_device_t *device);

/*
   int scsi_CD_read(ms_device_t *device, PCDROM_READ pSgReq,
   juint32_t *transferred);

   int scsi_CD_audio(ms_device_t *device, int ioctl, 
   juint8_t *in_buf,  juint32_t in_buf_size,
   juint8_t *out_buf, juint32_t out_but_size,
   juint32_t *bytes_transferred);
   */
#endif

