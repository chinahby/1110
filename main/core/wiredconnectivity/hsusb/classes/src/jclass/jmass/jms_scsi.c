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
#include "jms_scsi.h"
#include <jutil.h>
#include <jtypes.h>
#include "jms_devmgr.h"
#include "jms_transport.h"

static jresult_t unsafe_scsi_get_sense_data(ms_device_t *device);
static jresult_t unsafe_scsi_inquiry(ms_device_t *device);
static jresult_t unsafe_scsi_mode_sense_10(ms_device_t *device);
static jresult_t unsafe_scsi_mode_sense_6(ms_device_t *device);
static jresult_t unsafe_scsi_read_capacity(ms_device_t *device);
static jresult_t unsafe_scsi_read_partsize(ms_device_t *device);
static jresult_t unsafe_scsi_test_unit_ready(ms_device_t *device);
static jresult_t unsafe_scsi_unit_attention(ms_device_t *device);
static jresult_t unsafe_scsi_send_diagnostic(ms_device_t *device);
static jresult_t unsafe_scsi_read_write(ms_device_t *device,
    juint32_t offset_sector, juint32_t num_sectors, juint8_t *buffer,
    juint8_t flags, juint32_t *transferred, jint_t direction);
static jresult_t unsafe_scsi_request_sense(ms_device_t *device,
    scsi_data_pkt_t *data);
static jresult_t unsafe_scsi_start_stop_unit(ms_device_t *device,
    jint_t start, jint_t eject);
static jresult_t set_part_size(juint32_t capacity, juint32_t *hds, juint32_t *secs, 
    juint32_t *cyls);
#ifdef CONFIG_JUMASS_READ_GEOMETRY
#error code not present
#endif

#define SENSE_SKIP      1
#define SENSE_GET       0
static jresult_t perform_usb_io(ms_device_t *device, scsi_cmd_t *command,
    scsi_data_pkt_t *data, jint_t skip_sense);

/* Handle device error, try restarting the device 
 * Parameters:
 * IN/OUT      device - SCSI Device handle
 * Return Value:
 * 0 on success or one of the error.h error codes for failure */
static jresult_t unsafe_scsi_unit_attention(ms_device_t *device)
{
    juint32_t repeat;
    jresult_t rc = 0;

    DBG_I(DMASS_SCSI, ("SCSI2: [%s] Unit attention %d\n", 
        device->vendor, device->lun));

    if (device->subclass == USBMSC_SUBCLASS_UFI)
    {
        /* UFI floppies seem to require start unit first */
        unsafe_scsi_start_stop_unit(device, TRUE,  FALSE);
    }

    for (repeat = 0; repeat < UNIT_ATTENTION_REPEAT; repeat++)
    {
        rc = unsafe_scsi_test_unit_ready(device);
        if (!rc)
            break;

        /* If the error is irrecoverable or no media is 
         * inserted, exit immediately */
        if (rc == JENXIO || rc == JEINVAL || rc == JENOMEDIUM) 
            break;


        DBG_W(DMASS_SCSI, ("SCSI2: [%s] TEST UNIT READY failed(%d)\n", 
            device->vendor, rc));

        /* On first iteration give the device a chance to 
         * wake up on its own. Only on following iterations
         * try to wake it forecfuly */
        if (repeat)
            unsafe_scsi_start_stop_unit(device, TRUE,  FALSE);

        jdelay_ms(30);
    }

    DBG_X(DMASS_SCSI, ("SCSI2: [%s] Finished unit attention %d\n",
        device->vendor, rc));

    return rc;
}

/* Perform a SCSI2 SPC - TEST_UNIT_READY command (Check if ready for data IO)
 * Parameters:
 * IN/OUT      device - SCSI Device handle
 * Return Value:
 * 0 on success or one of the error.h error codes for failure */
jresult_t unsafe_scsi_test_unit_ready(ms_device_t *device)
{
    scsi_cmd_t  command;
    juint8_t     CDB[MAX_CDB];
    jresult_t rc;

    DBG_I(DMASS_SCSI, ("SCSI2: [%s] Test unit ready %d\n", 
        device->vendor, device->lun));

    command.direction   = DATA_OUT;
    command.timeout     = SCSI_COMMAND_TIMEOUT;
    command.lun         = device->lun;
    command.cmd         = CDB;
    command.size        = device->subclass == USBMSC_SUBCLASS_SCSI ? 
        SCSI_CDB_6 : UFI_CDB;

    j_memset(CDB, 0, sizeof(CDB));
    CDB[0] = SCSI_TEST_UNIT_READY;
    CDB[1] = ((device->lun & 0x7) << 5);

    rc = perform_usb_io(device, &command, NULL, SENSE_GET);
    if (rc)
    {
        DBG_W(DMASS_SCSI, ("SCSI2: [%s] Scsi Test Unit Ready ERROR: %d\n",
            device->vendor, rc));
    }

    DBG_X(DMASS_SCSI, ("SCSI2: [%s] Finished test unit ready %d\n",
        device->vendor, rc));

    /* If test_unit_ready succeeds a medium is present in drive */
    if (!rc && !device->flags.medium_present)
    {
        device->flags.medium_present = TRUE;
        device->flags.medium_changed = TRUE;
    }

    return rc;
}

/* Perform a SCSI2 SPC - REQUEST_SENSE command (Get current status)
 * Parameters:
 * IN/OUT       device  - SCSI Device handle
 * IN/OUT       data    - Descriptor of data packet for the sense data 
 * Return Value:
 * 0 on success or one of the error.h error codes for failure */
jresult_t unsafe_scsi_request_sense(ms_device_t *device, scsi_data_pkt_t *data)
{
    scsi_cmd_t command;
    juint8_t CDB[MAX_CDB];
    jresult_t rc;

    DBG_I(DMASS_SCSI, ("SCSI2: [%s] Request sense\n", 
        device->vendor));

    command.direction   = DATA_IN;
    command.timeout     = SCSI_COMMAND_TIMEOUT;
    command.lun         = device->lun;
    command.cmd         = CDB;
    command.size        = device->subclass == USBMSC_SUBCLASS_SCSI ? 
        SCSI_CDB_6 : UFI_CDB;

    j_memset(CDB, 0, sizeof(CDB));
    CDB[0] = SCSI_REQUEST_SENSE;
    CDB[1] = ((device->lun & 0x7) << 5);
    CDB[4] = (juint8_t)data->transfer_size;

    rc = perform_usb_io(device, &command, data, SENSE_SKIP);

    DBG_X(DMASS_SCSI, ("SCSI2: [%s] Finished request sense %d\n", 
        device->vendor, rc));

    return rc;
}

/* Get sense data and parse it to get device status 
 * Parameters:
 * IN/OUT      device  - SCSI Device handle
 * Return Value:
 * 0 on success or one of the error.h error codes for failure */
jresult_t unsafe_scsi_get_sense_data(ms_device_t *device)
{
    scsi_data_pkt_t     data;
    juint8_t     sense_data[18];
    juint8_t     sense_key, ASC, ASCQ;
    jresult_t    rc;

    DBG_I(DMASS_SCSI, ("SCSI2: [%s] Get sense data %d\n",
        device->vendor, device->lun));

    data.bytes_transfered       = 0;
    data.transfer_size          = sizeof(sense_data);
    data.buffer                 = sense_data;
    data.flags                  = 0;
    j_memset(sense_data, 0, sizeof(sense_data));

    rc = unsafe_scsi_request_sense(device, &data);

    if ((rc && rc != JEAGAIN)
        || data.bytes_transfered < MIN_SENSE_LEN ||
        ((sense_data[0] & 0x7f) != 0x70 &&
        (sense_data[0] & 0x7f) != 0x71))
    {
        if (!rc)
            rc = JEINVAL;

        DBG_W(DMASS_SCSI, ("SCSI2: [%s] Get Sense Data error: %d\n",
            device->vendor, rc));

        return rc;
    }

    sense_key   = sense_data[2] & 0x0f;
    ASC         = sense_data[12];
    ASCQ        = sense_data[13];

    DBG_I(DMASS_SCSI, ("SCSI2: [%s] Sense Key: 0x%x ASC:0x%x ASCQ:0x%x\n",
        device->vendor, sense_key, ASC, ASCQ));

    switch (sense_key) 
    {
    case SENSE_NONE:
    case SENSE_RECOVERED_ERROR:
        rc = 0;
        break;

    case SENSE_NOT_READY:
        switch (ASC)
        {
        case ASC_LUN:
            DBG_W(DMASS_SCSI, ("SCSI2: [%s] SENSE_NOT_READY : ASC_LUN: %d\n", 
                device->vendor, device->lun));
            if (0x02 == ASCQ) 
            {
                /* Initialization Required */
                scsi_start_stop_unit(device, START, FALSE);
            }
            rc = JEAGAIN;
            break;

        case ASC_MEDIUM_NOT_PRESENT:
            DBG_W(DMASS_SCSI, ("SCSI2: [%s] SENSE_NOT_READY: "
                "ASC_MEDIUM_NOT_PRESENT\n",
                device->vendor));

            if (device->flags.medium_present)
                device->flags.medium_changed = TRUE;

            device->flags.medium_present = FALSE;
            device->medium_type         = SCSI_MEDIUM_UNKNOWN;
            device->sector_size         = 0;
            device->sector_count        = 0;
            rc = JENOMEDIUM;
            break;

        default:
            DBG_W(DMASS_SCSI, ("SCSI2: [%s] SENSE_NOT_READY: " 
                "Unhandled ASC:0x%x ASCQ:0x%x\n",
                device->vendor, ASC, ASCQ));

            rc = JEAGAIN;
            break;
        }
        break;

    case SENSE_MEDIUM_ERROR:
        DBG_E(DMASS_SCSI, ("SCSI2: [%s] SENSE_MEDIUM_ERROR\n", device->vendor));
        rc = JEINVAL;
        break;

    case SENSE_HARDWARE_ERROR:
        DBG_E(DMASS_SCSI, ("SCSI2: [%s] SENSE_HARDWARE_ERROR\n", 
            device->vendor));
        rc = JEIO;
        break;

    case SENSE_ILLEGAL_REQUEST:
        DBG_W(DMASS_SCSI, ("SCSI2: [%s] SENSE_ILLEGAL_REQUEST\n", 
            device->vendor));
        rc = JEINVAL;
        break;

    case SENSE_UNIT_ATTENTION:
        switch( ASC ) 
        {
        case ASC_MEDIA_CHANGED:
            DBG_W(DMASS_SCSI, ("SCSI2: [%s] SENSE_UNIT_ATTENTION: "
                "ASC_MEDIA_CHANGED\n", device->vendor));

            device->flags.medium_present        = 1;
            device->flags.medium_changed        = TRUE;
            device->medium_type                 = SCSI_MEDIUM_UNKNOWN;
            device->sector_size                 = 0;
            device->sector_count                = 0;
            rc = JEBUSY;
            break;

        case ASC_RESET:
            DBG_W(DMASS_SCSI, ("SCSI2: [%s] SENSE_UNIT_ATTENTION: ASC_RESET\n",
                device->vendor));
            rc = JENOENT;
            break;

        case ASC_MEDIUM_NOT_PRESENT:
            DBG_W(DMASS_SCSI, ("SCSI2: [%s] SENSE_UNIT_ATTENTION: "
                "MEDIUM NOT PRESENT\n", device->vendor));

            if (device->flags.medium_present)
                device->flags.medium_changed = TRUE;

            device->flags.medium_present        = FALSE;
            device->medium_type                 = SCSI_MEDIUM_UNKNOWN;
            device->sector_size                 = 0;
            device->sector_count                = 0;
            rc = JENOMEDIUM;
            break;

        case ASC_COMMANDS_CLEARED :
        default:
            DBG_E(DMASS_SCSI, ("SCSI2: [%s] SENSE_UNIT_ATTENTION: Unhandled "
                "ASC:0x%x\n", device->vendor, ASC));
            rc = JENOENT;
            break;
        }
        break;

    case SENSE_DATA_PROTECT :
        rc = JEPERM;
        break;

    default:
        rc = JEIO;
        break;
    }

    DBG_X(DMASS_SCSI, ("SCSI2: [%s] Finished get sense data %d\n",
        device->vendor, rc));

    return rc;
}

static void remove_trailing_white(juint8_t *str, jint_t len)
{
    jint_t i = len - 1;
    while (i >= 0 && str[i] == ' ')
    {
        str[i] = 0;
        i--;
    }
}

/* Perform a SCSI2 SPC - INQUIRY command (Get device information)
 * Parameters:
 * IN/OUT      device  - SCSI Device handle
 * Return Value:
 * 0 on success or one of the error.h error codes for failure */
jresult_t unsafe_scsi_inquiry(ms_device_t *device)
{
    scsi_cmd_t          command;
    juint8_t             CDB[MAX_CDB];

    scsi_data_pkt_t     data;
    juint8_t             data_block[MIN_INQUIRY_LEN];
    juint8_t             *dev_info;
    juint8_t perq; /* peripherial qualifier */

    jresult_t rc = 0;

    DBG_I(DMASS_SCSI, ("SCSI2: [%s] Inquiry %d\n",
        device->vendor, device->lun));

    command.direction   = DATA_IN;
    command.timeout     = SCSI_COMMAND_TIMEOUT;
    command.lun         = device->lun;
    command.cmd         = CDB;
    command.size        = device->subclass == USBMSC_SUBCLASS_SCSI ? 
        SCSI_CDB_6 : UFI_CDB;

    j_memset(CDB, 0, sizeof(CDB));
    CDB[0] = SCSI_INQUIRY;
    CDB[1] = ((device->lun & 0x7) << 5);
    CDB[4] = sizeof(data_block);

    j_memset(data_block, 0, sizeof(data_block));
    data.bytes_transfered       = 0;
    data.transfer_size          = sizeof(data_block);
    data.buffer                 = data_block;
    data.flags                  = 0;

    rc = perform_usb_io(device, &command, &data, SENSE_GET);

    if (rc || data.bytes_transfered < MIN_INQUIRY_LEN)
    {
        DBG_W(DMASS_SCSI, ("SCSI2: [%s] Scsi Inquiry ERROR: %d [%ld/%ld]\n",
            device->vendor, rc, data.bytes_transfered, data.transfer_size));
        goto Exit;
    } 

#define PERQ_LUN_CONNECTED      0x0
#define PERQ_LUN_DISCONNECTED   0x1
#define PERQ_LUN_INVALID        0x3

    DBG_V(DMASS_SCSI ,("SCSI2: [%s] Inquiry Data[0] = %x(%x) Size %ld:\n",
        device->vendor, data_block[0], data_block[0] & SCSI_DEVICE_UNKNOWN,
        data.bytes_transfered));

    if (data_block[0] == 0x7F)
    {
        DBG_E(DMASS_SCSI, ("SCSI2: [%s] Invalid parameter in Inquiry, %x\n",
            device->vendor, data_block[0]));
        rc = JEINVAL;
        goto Exit;
    } 

    device->devtype = data_block[0] & SCSI_DEVICE_UNKNOWN;

    perq = (data_block[0] & 0xE0) >> 5;

    if (perq == PERQ_LUN_INVALID || 
        (device->devtype != SCSI_DEVICE_DIRECT_ACCESS &&
        device->devtype != SCSI_DEVICE_CDROM))
    {
        DBG_E(DMASS_SCSI, ("SCSI2: [%s] Unsupported device type %d perq %d!\n",
            device->vendor, device->devtype, perq));
        rc = JEINVAL;
        goto Exit;
    }

    device->flags.removable = (data_block[1] & 0x80) ? 1 : 0;

    dev_info = &data_block[8]; /* Start of ID strings */

    /* ASCI Vendor id string */
    j_memcpy(&device->vendor, dev_info, VENDOR_ID_LEN); 
    device->vendor[VENDOR_ID_LEN] = 0;
    remove_trailing_white(device->vendor, VENDOR_ID_LEN);
    dev_info += VENDOR_ID_LEN;

    /* ASCI Product id string */
    j_memcpy(&device->product, dev_info, PRODUCT_ID_LEN);
    device->product[PRODUCT_ID_LEN] = 0;
    remove_trailing_white(device->product, PRODUCT_ID_LEN);
    dev_info += PRODUCT_ID_LEN;

    /* Four ASCI hex digits describing product release */
    device->release =
        (dev_info[0] - '0') << 12 |
        (dev_info[1] - '0') << 8 |
        (dev_info[2] - '0') << 4 |
        (dev_info[3] - '0');

    DBG_I(DMASS_SCSI, ("SCSI2: Device properties: [%s] from [%s] "
        "release %x type %x %s\n", device->product, device->vendor, 
        device->release, device->devtype,
        device->flags.removable ? "removable" : ""));

Exit:

    DBG_X(DMASS_SCSI, ("SCSI2: [%s] Finished scsi inquiry %d\n",
        device->vendor, rc));

    return rc;
}

/* Perform a SCSI2 SPC - SEND_DIAGNOSTIC command (not supported)
 * Parameters:
 * IN/OUT      device  - SCSI Device handle
 * Return Value:
 * Always -1 */
jresult_t unsafe_scsi_send_diagnostic(ms_device_t *device)
{
    DBG_E(DMASS_SCSI, ("SCSI2: [%s] Send diagnostic is not supported !\n",
        device->vendor));
    return -1;
}
#define MASS_BLOCK_SIZE 128*32

jresult_t unsafe_scsi_read_partsize(ms_device_t *device)
{
    jresult_t rc = 0;
    juint32_t temp_cyl, cylinders = 0;
    juint32_t heads = 0, sectors = 0;
    juint32_t size = device->sector_count;

    {
        /* 3. If not succeed - pick some standart mapping 
         *    with at most 1024 cylinders,
         *    and at most 62 sectors per track - this works up to
         *    7905 MB */
        rc = set_part_size(size, &heads, &sectors, &cylinders);
    }
    /* if something went wrong, then apparently we have to return
       a geometry with more than 1024 cylinders */
    if (rc || heads > 255 || sectors > 63) 
    {
        heads = 64;
        sectors = 32;
        temp_cyl = size / (heads * sectors);
        if (temp_cyl > 65534) 
        {
            heads = 255;
            sectors = 63;
        }
        cylinders = size / (heads * sectors);
    }

    rc = 0;
    device->heads = (juint8_t)heads;
    device->sectors = (juint8_t)sectors;
    device->cylinders = cylinders;
    
    return rc;
}

/* Perform a SCSI2 SPC - READ_CAPACITY command (get media size)
 * Parameters:
 * IN/OUT      device  - SCSI Device handle
 * Return Value:
 * 0 on success or one of the error.h error codes for failure */
jresult_t unsafe_scsi_read_capacity(ms_device_t *device)
{
    scsi_cmd_t  command;
    juint8_t     CDB[MAX_CDB];

    scsi_data_pkt_t data;
    juint8_t     data_block[8];
    juint32_t    size_db = sizeof(data_block);

    jresult_t rc = 0; 

    DBG_I(DMASS_SCSI, ("SCSI2: [%s] Read capacity %d\n",
        device->vendor, device->lun));

    command.direction   = DATA_IN;
    command.timeout     = SCSI_COMMAND_TIMEOUT;
    command.lun         = device->lun;
    command.cmd         = CDB;
    command.size        = device->subclass == USBMSC_SUBCLASS_SCSI ? 
        SCSI_CDB_10 : UFI_CDB; 

    j_memset(CDB, 0, sizeof(CDB));
    CDB[0] = SCSI_READ_CAPACITY;
    CDB[1] = ((device->lun & 0x7) << 5);

    j_memset(data_block, 0, size_db);
    data.bytes_transfered       = 0;
    data.transfer_size          = size_db;
    data.buffer                 = data_block;
    data.flags                  = 0;

    rc = perform_usb_io(device, &command, &data, SENSE_GET);

    if (rc || (data.transfer_size != data.bytes_transfered))
    {
        DBG_W(DMASS_SCSI, ("SCSI2: [%s] Scsi Read Capacity ERROR: %d\n",
            device->vendor, rc));
        rc = JEIO;
    } 
    else 
    {
        device->sector_count    = be32toh_t(&data_block[0]);
        device->sector_size     = be32toh_t(&data_block[4]); 

        DBG_I(DMASS_SCSI, ("SCSI2: [%s] Disk size %ld sectors of %ld bytes "
            "each\n", device->vendor, device->sector_count, 
            device->sector_size));
    }

    DBG_X(DMASS_SCSI, ("SCSI2: [%s] Finished read capacity %d\n",
        device->vendor, rc));

    return rc;
}

/* Perform a SCSI2 SPC - MODE_SENSE_10 command (get device type)
 * Parameters:
 * IN/OUT       device  - SCSI Device handle
 * Return Value:
 * 0 on success or one of the error.h error codes for failure */
jresult_t unsafe_scsi_mode_sense_10(ms_device_t *device)
{
    scsi_data_pkt_t     data;
    scsi_cmd_t          command;
    juint8_t             CDB[MAX_CDB];
    juint8_t             data_block[MAX_SENSE_LEN];
    juint16_t            header_size = 8;                   

    jresult_t rc = 0;

    DBG_I(DMASS_SCSI, ("SCSI2: [%s] Mode sense 10 %d\n",
        device->vendor, device->lun));

    if (device->devtype != SCSI_DEVICE_CDROM &&
        device->devtype != SCSI_DEVICE_DIRECT_ACCESS)
    {
        DBG_E(DMASS_SCSI, ("SCSI2: [%s] Mode sense is supported for CD/HD "
            "only.\n", device->vendor));
        return JEINVAL;
    }

    command.direction   = DATA_IN;
    command.timeout     = SCSI_COMMAND_TIMEOUT;
    command.lun         = device->lun;
    command.cmd         = CDB;
    command.size        = device->subclass == USBMSC_SUBCLASS_SCSI ? 
        SCSI_CDB_10 : UFI_CDB;

    j_memset(CDB, 0, sizeof(CDB));
    CDB[0] = SCSI_MODE_SENSE10;
    CDB[1] = ((device->lun & 0x7) << 5);
    CDB[2] = 0x3f;   /* Get all page codes*/

    switch (device->devtype) 
    {
    case SCSI_DEVICE_DIRECT_ACCESS:
        if (device->subclass == USBMSC_SUBCLASS_UFI)
        {
            CDB[2] = MODE_PAGE_FLEXIBLE_DISK;
            header_size += 32;
        } 
        else 
        {
            header_size = MIN_SENSE_LEN;
        }
        break;

    case SCSI_DEVICE_CDROM:
        CDB[2]  = MODE_PAGE_CDROM;
        header_size += 8;
        break;

    default:
        DBG_E(DMASS_SCSI, ("SCSI2: [%s] Scsi Mode Sense 10: "
            "Unknown DeviceType: 0x%x\n", device->vendor, device->devtype));

        header_size = MIN_SENSE_LEN;
        break;
    }

    CDB[7] = (header_size & 0xFF00) >> 8; /* MSB */
    CDB[8] =  header_size & 0x00FF;       /* LSB */

    j_memset(data_block, 0, sizeof(data_block));

    /* Note: some devices fail with only 8 byte header request length, 
     * but all should recover with extra buffer space. */
    data.bytes_transfered       = 0;
    data.transfer_size          = header_size;
    data.buffer                 = data_block;
    data.flags                  = 0;

    rc = perform_usb_io(device, &command, &data, SENSE_GET);

    if (rc)
    { 
        DBG_W(DMASS_SCSI, ("SCSI2: [%s] Scsi Mode Sense ERROR:%d\n",
            device->vendor, rc));
    } 
    else if (data.bytes_transfered < 8 ) 
    { 
        DBG_W(DMASS_SCSI,
            ("SCSI2: mode sense 10 returned no data - using defaults\n"));
        device->medium_type             = 0;
        device->flags.write_protected   = 0;
    }
    else 
    {
        /* look at the Header */
        DBG_I(DMASS_SCSI, ("SCSI2: [%s] Medium Type:0x%x\n",
            device->vendor, data_block[1]));
        device->medium_type = data_block[1];

        /* bit7 is WP bit */
        DBG_I(DMASS_SCSI, ("SCSI2: [%s] Device Specific:0x%x\n",
            device->vendor, data_block[2]));

        device->flags.write_protected = (data_block[2] & 0x80) ? 1 : 0;
    }

    DBG_X(DMASS_SCSI, ("SCSI2: [%s] Finished mode sense 10 %d\n",
        device->vendor, rc));

    return rc;
}

/* Perform a SCSI1 SPC - MODE_SENSE_6 command (get device type)
 * Parameters:
 * IN/OUT       device  - SCSI Device handle
 * Return Value:
 * 0 on success or one of the error.h error codes for failure */
jresult_t unsafe_scsi_mode_sense_6(ms_device_t *device)
{
    scsi_data_pkt_t     data;
    scsi_cmd_t          command;
    juint8_t             CDB[MAX_CDB];
    juint8_t             data_block[MIN_SENSE_LEN];
    juint16_t            header_size = 8; 
    jresult_t rc = 0;

    DBG_I(DMASS_SCSI, ("SCSI2: [%s] Mode sense 6 %d\n",
        device->vendor, device->lun));

    if (device->devtype != SCSI_DEVICE_CDROM &&
        device->devtype != SCSI_DEVICE_DIRECT_ACCESS)
    {
        DBG_E(DMASS_SCSI, ("SCSI2: [%s] Mode sense is supported for CD/HD "
            "only.\n", device->vendor));
        return JEINVAL;
    }

    command.direction   = DATA_IN;
    command.timeout     = SCSI_COMMAND_TIMEOUT;
    command.lun         = device->lun;
    command.cmd         = CDB;
    command.size        = device->subclass == USBMSC_SUBCLASS_SCSI ? 
        SCSI_CDB_6 : UFI_CDB;

    j_memset(CDB, 0, sizeof(CDB));
    CDB[0] = SCSI_MODE_SENSE6;
    CDB[1] = ((device->lun & 0x7) << 5);
    CDB[2] = 0x3f;

    switch (device->devtype)
    {
    case SCSI_DEVICE_DIRECT_ACCESS:
        if (device->subclass == USBMSC_SUBCLASS_UFI) 
        {
            CDB[2] = MODE_PAGE_FLEXIBLE_DISK;
            header_size += 32;
        }
        else 
        {
            header_size = sizeof(data_block);
        }
        break;
    case SCSI_DEVICE_CDROM:
        CDB[2] = MODE_PAGE_CDROM;
        header_size += 8;
        break;
    default:
        header_size = sizeof(data_block);
        break;

    }

    CDB[4] = MIN(header_size, MAX_CMD_DATA_LEN); 

    j_memset(data_block, 0, sizeof(data_block));

    data.bytes_transfered       = 0;
    data.transfer_size          = header_size;
    data.buffer                 = data_block;
    data.flags                  = 0;

    rc = perform_usb_io(device, &command, &data, SENSE_GET);

    if (rc)
    {
        DBG_W(DMASS_SCSI, ("SCSI2: [%s] Scsi Mode Sense 6: device "
            "failed command %d\n", device->vendor, rc));
    }
    else if (data.bytes_transfered < 8)
    {
        DBG_W(DMASS_SCSI,
            ("SCSI2: mode sense 6 returned no data - using defaults\n"));
        device->medium_type             = 0;
        device->flags.write_protected   = 0;
    }   
    else 
    {
        DBG_I(DMASS_SCSI, ("SCSI2: [%s] Scsi Mode Sense 6: medium type=0x%x\n",
            device->vendor, data_block[2]));

        device->medium_type             = data_block[1];
        device->flags.write_protected   = (data_block[2] & 0x80) ? 1 : 0;
    }

    DBG_X(DMASS_SCSI, ("SCSI2: [%s] Finished mode sense 6 %d\n",
        device->vendor, rc));

    return rc;
}

/* Perform a SCSI2 SPC - START_STOP command (restart/stop device)
 * Parameters:
 * IN/OUT       device  - SCSI Device handle
 * IN           start   - Boolean (True - Start the unit, False - Stop it)
 * IN           eject   - Boolean (True - Try to eject the unit)
 * Return Value:
 * 0 on success or one of the error.h error codes for failure */
jresult_t unsafe_scsi_start_stop_unit(ms_device_t *device, jint_t start,
    jint_t eject)
{
    scsi_cmd_t  command;
    juint8_t     CDB[MAX_CDB];

    jresult_t rc = 0;

    DBG_I(DMASS_SCSI, ("SCSI2: [%s] Start Stop %d\n", device->vendor,
        device->lun));

    command.direction   = DATA_OUT;
    command.timeout     = SCSI_COMMAND_TIMEOUT;
    command.lun         = device->lun;
    command.cmd         = CDB;
    command.size        = device->subclass == USBMSC_SUBCLASS_SCSI ? 
        SCSI_CDB_6 : UFI_CDB;

    j_memset(CDB, 0, sizeof(CDB));
    CDB[0] = SCSI_START_STOP;
    CDB[1] = ((device->lun & 0x7) << 5);
    CDB[4] = (eject & 0x1) << 1;
    CDB[4] |= start & 0x1;

    rc = perform_usb_io(device, &command, NULL, SENSE_GET);
    if (rc )
    {
        DBG_W(DMASS_SCSI, ("SCSI2: [%s] Scsi Start Stop Unit ERROR:%d\n",
            device->vendor, rc));
    }

    DBG_V(DMASS_SCSI, ("SCSI2: [%s] Finished start stop unit %d\n",
        device->vendor, rc));

    return rc;
}

/* Perform a SCSI2 SBC - WRITE10 command (transfer data)
 * Parameters:
 * IN   device  - Device to perform transfer to 
 * IN   offset_sector - Sector to start from
 * IN   num_sectors - Number of sectors to transfer
 * IN   *buffer - Buffer of data to transfer
 * IN   flags   - Transfer specific flags
 * OUT  *transferred - Number of sectors transferred
 * IN   direction - Direction to transfer
 * Return Value:
 * 0 on success or one of the error.h error codes for failure */
jresult_t unsafe_scsi_read_write(ms_device_t *device, juint32_t offset_sector,
    juint32_t num_sectors, juint8_t *buffer, juint8_t flags, juint32_t *transferred,
    jint_t direction)
{
    scsi_cmd_t          command;
    juint8_t             CDB[MAX_CDB];
    scsi_data_pkt_t     data;
    juint32_t            bytes;
    juint8_t             retries = 0;
    jresult_t rc = 0;

    DBG_V(DMASS_SCSI, ("SCSI2: [%s] Transfer from %ld, %ld sector(s). "
        "Direction: %d\n", 
        (device != NULL ? (char *)device->vendor : "NO DEVICE"),
        offset_sector, num_sectors, direction));

    if (!device || !device->sector_size || !buffer || !transferred)
    {
        DBG_E(DMASS_SCSI, ("SCSI2: [%s] Bad parameters in transfer command:\n"
            "   Device     : %p\n"
            "   Sector size: %ld\n"
            "   Buffer     : %p\n"
            "   Buffer size: %ld\n",
            device->vendor, 
            device, (device ? device->sector_size : 0),
            buffer, (transferred ? *transferred : 0)));

        return JEINVAL;
    }

    if (offset_sector + num_sectors > device->sector_count)
    {
        DBG_E(DMASS_SCSI, ("SCSI2: [%s] Trying to %s beyound disk size.\n", 
            device->vendor, direction == DATA_IN ? "read" : "write"));
        return JEINVAL;
    }

    if (device->flags.pending_removal)
    {
        DBG_W(DMASS_SCSI, ("SCSI2: Transfer command, while pending removal\n"));
        return JEPERM;
    }

    if (!*transferred || !num_sectors)
    {
        DBG_E(DMASS_SCSI, ("SCSI2: [%s] ScsiReadWrite: invalid transfer "
            "size:%u, %u\n", device->vendor, *transferred, num_sectors));
        return JEINVAL;
    }

    bytes = num_sectors * device->sector_size;

    /* Check that the buffer is big enough for requested info */
    if (*transferred < bytes)
    {
        DBG_E(DMASS_SCSI, ("SCSI2: [%s] ScsiReadWrite: buffer too small "
            "(%u < %u)\n", device->vendor, *transferred, bytes));
        return JENOMEM;
    }

    command.direction   = direction ? DATA_IN : DATA_OUT;
    command.lun         = device->lun;
    command.cmd         = CDB;
    command.timeout     =  
        (direction ? SCSI_READ_SECTOR_TIMEOUT : SCSI_WRITE_SECTOR_TIMEOUT);
    command.size        = device->subclass == USBMSC_SUBCLASS_SCSI ? 
        SCSI_CDB_10 : UFI_CDB;

    j_memset(CDB, 0, sizeof(CDB));
    CDB[0] = direction ? SCSI_READ10 : SCSI_WRITE10;
    CDB[1] = ((device->lun & 0x7) << 5);

    /* Logical Block Address */
    htobe32_t(&CDB[2], offset_sector);

    /* TransferLength (in sectors) */
    htobe16_t(&CDB[7], (juint16_t)num_sectors);

    data.bytes_transfered       = 0;
    data.transfer_size          = bytes;
    data.buffer                 = buffer;
    data.flags                  = flags;

    DBG_V(DMASS_SCSI, ("SCSI2: [%s] Scsi %s - LBA:%ld Num:%ld TimeOut:%ld\n",
        device->vendor, direction ? "Read" : "Write", offset_sector,
        num_sectors, command.timeout));

    *transferred = 0;

    do 
    {
        rc = perform_usb_io(device, &command, &data, SENSE_GET);
        if (!rc)
        {
            DBG_V(DMASS_SCSI, ("SCSI2: [%s] IO finished %ld bytes\n",
                (device != NULL ? (char *)device->vendor : "NO DEVICE"),
                data.bytes_transfered));

            *transferred = data.bytes_transfered;
            break;
        }

        DBG_W(DMASS_SCSI, ("SCSI2: [%s] Scsi Read Write Sense error:%d\n",
            device->vendor, rc));

        /* Cannot pass cmds to device, recovery impossible */
        if (rc == JENXIO)
            break;

        rc = unsafe_scsi_unit_attention(device);
        if (rc) 
            break;

    } while (++retries < 3);

    DBG_X(DMASS_SCSI, ("SCSI2: [%s] Finished transfer command %d\n",
        device->vendor, rc));

    return rc;
}

/* Perform transfer via lower level layers.
 * Parameters:
 * IN   device  - Device to transfer to
 * IN   command - SCSI command to perfrom
 * IN/OUT *data - Data packet descriptor
 * IN   skip_sense - Do not request sense at completion
 * Return Value:
 * 0 on success or one of the error.h error codes for failure */
static jresult_t perform_usb_io(ms_device_t *device, scsi_cmd_t *command,
    scsi_data_pkt_t *data, jint_t skip_sense)
{
    jresult_t rc = 0;
    jint_t retry;
    juint32_t dummy;

    if (device->flags.pending_removal)
    {
        DBG_W(DMASS_SCSI, ("SCSI: Got command while pending removal %s\n",
            device->vendor));
        return JENXIO;
    }

    DBG_V(DMASS_SCSI, ("SCSI2: [%s] IO Cmd[%ld:%d] Data[%p:%ld]\n",
        device->vendor, command->size, command->direction & DATA_IN,
        data ? data->buffer : NULL,
        data ? data->transfer_size : 0));

    retry = WIRE_FAILURE_RETRIES;
    do
    {
        /* Give the controllers a few msec to sync */
        if (rc == JENXIO)
            jdelay_ms(10);

        rc = jms_transfer(device->transport, command->lun, command->cmd, 
            command->size, command->direction & DATA_IN, command->timeout,
            data ? data->buffer : NULL, 
            data ? data->transfer_size : 0, 
            data ? data->flags : 0,
            data ? &data->bytes_transfered : &dummy);
    } 
    while (rc == JENXIO && retry--);

    DBG_V(DMASS_SCSI, ("SCSI2: [%s] IO Result %d\n", 
        device->vendor, rc));

    /* If this is a mass storage error */
    if (rc == JEIO && !skip_sense)
    {
        rc = unsafe_scsi_get_sense_data(device);
    }
    /* CBI reports we need to get status with get_sense */
    else if (rc == JEAGAIN && !skip_sense)
    {
        retry = CBI_GET_STATUS_RETRIES;
        do
        {
            rc = unsafe_scsi_get_sense_data(device);
            if (rc == JEAGAIN)
                jdelay_ms(1);
        } 
        while (rc == JEAGAIN && retry--);
    }

    return rc;
}

/* "Safe" wrappers for scsi commands. 
 * These assure no concurrent access is made
 * to a single mass storage device */
jresult_t scsi_get_sense_data(ms_device_t *device)
{
    jresult_t rc;
    device_lock(device->devmgr);
    rc = unsafe_scsi_get_sense_data(device);
    device_unlock(device->devmgr);
    return rc;
}

jresult_t scsi_inquiry(ms_device_t *device)
{
    jresult_t rc;
    device_lock(device->devmgr);
    rc = unsafe_scsi_inquiry(device);
    device_unlock(device->devmgr);
    return rc;
}

jresult_t scsi_mode_sense_10(ms_device_t *device)
{
    jresult_t rc;
    device_lock(device->devmgr);
    rc = unsafe_scsi_mode_sense_10(device);
    device_unlock(device->devmgr);
    return rc;
}

jresult_t scsi_mode_sense_6(ms_device_t *device)
{
    jresult_t rc;
    device_lock(device->devmgr);
    rc = unsafe_scsi_mode_sense_6(device);
    device_unlock(device->devmgr);
    return rc;
}

jresult_t scsi_read_capacity(ms_device_t *device)
{
    jresult_t rc;
    device_lock(device->devmgr);
    rc = unsafe_scsi_read_capacity(device);
    device_unlock(device->devmgr);
    return rc;
}

jresult_t scsi_read_partsize(ms_device_t *device)
{
    jresult_t rc;
    device_lock(device->devmgr);
    rc = unsafe_scsi_read_partsize(device);
    device_unlock(device->devmgr);
    return rc;
}

jresult_t scsi_read_write(ms_device_t *device, juint32_t offset_sector,
    juint32_t num_sectors, juint8_t *buffer, juint8_t flags, 
    juint32_t *transferred, jint_t direction)
{
    jresult_t rc;
    device_lock(device->devmgr);
    rc = unsafe_scsi_read_write(device, offset_sector, num_sectors,
        buffer, flags, transferred, direction);
    device_unlock(device->devmgr);
    return rc;
}

jresult_t scsi_request_sense(ms_device_t *device,
    scsi_data_pkt_t *data)
{
    jresult_t rc;
    device_lock(device->devmgr);
    rc = unsafe_scsi_request_sense(device, data);
    device_unlock(device->devmgr);
    return rc;
}

jresult_t scsi_send_diagnostic(ms_device_t *device)
{
    jresult_t rc;
    device_lock(device->devmgr);
    rc = unsafe_scsi_send_diagnostic(device);
    device_unlock(device->devmgr);
    return rc;
}

jresult_t scsi_start_stop_unit(ms_device_t *device, jint_t start, jint_t eject)
{
    jresult_t rc;
    device_lock(device->devmgr);
    rc = unsafe_scsi_start_stop_unit(device, start, eject);
    device_unlock(device->devmgr);
    return rc;
}

jresult_t scsi_test_unit_ready(ms_device_t *device)
{
    jresult_t rc;
    device_lock(device->devmgr);
    rc = unsafe_scsi_test_unit_ready(device);
    device_unlock(device->devmgr);
    return rc;
}

jresult_t scsi_unit_attention(ms_device_t *device)
{
    jresult_t rc;
    device_lock(device->devmgr);
    rc = unsafe_scsi_unit_attention(device);
    device_unlock(device->devmgr);
    return rc;
}

#define MSDOS_MBR 0xAA55
#define MSDOS_MBR_OFFSET 510
#define FAT_OFFSET 0x1BE
#define CYL_MASK 0xc0
#define SECTOR_MASK 0x3f 

#define MAX_CYLINDERS 1024L
#define MAX_SECTORS 62L
static jresult_t set_part_size(juint32_t capacity, juint32_t *hds, juint32_t *secs, 
    juint32_t *cyls)
{
    jresult_t rc = 0;
    juint32_t heads, sectors, cylinders, temp;

    cylinders = MAX_CYLINDERS;          /* Set number of cylinders to max */
    sectors = MAX_SECTORS;              /* Maximize sectors per track */

    temp = cylinders * sectors; /* Compute divisor for heads */
    heads = capacity / temp;    /* Compute value for number of heads */

    /* If no remainder, done! */
    if (capacity % temp) 
    {
        heads++;                  /* Else, increment number of heads */
        temp = cylinders * heads; /* Compute divisor for sectors */
        sectors = capacity / temp;/* Compute value for sectors per
                                     track */
        /* If no remainder, done! */
        if (capacity % temp) 
        {
            sectors++;                  /* Else, increment number of sectors */
            temp = heads * sectors;     /* Compute divisor for cylinders */
            cylinders = capacity / temp;/* Compute number of cylinders */
        }
    }
    if (cylinders == 0)
        rc = JEINVAL;    /* Give error if 0 cylinders */

    *cyls = cylinders;  /* Stuff return values */
    *secs = sectors;
    *hds = heads;
    return (rc);
}
