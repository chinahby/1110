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
/*
 * Copyright (c) 2004 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Lennart Augustsson (lennart@augustsson.net) and by Charles M. Hannum.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * The driver handles 3 Wire Protocols
 * - Command/Bulk/Interrupt (CBI)
 * - Command/Bulk/Interrupt with Command Completion Interrupt (CBI with CCI)
 * - Mass Storage Bulk-Only (BBB)
 *   (BBB refers Bulk/Bulk/Bulk for Command/Data/Status phases)
 *
 * Over these wire protocols it handles the following command protocols
 * - SCSI
 * - UFI (floppy command set)
 * - 8070i (ATAPI)
 *
 * UFI and 8070i (ATAPI) are transformed versions of the SCSI command set. The
 * sc->transform method is used to convert the commands into the appropriate
 * format (if at all necessary). For example, UFI requires all commands to be
 * 12 bytes in length amongst other things.
 *
 * The source code below is marked and can be split into a number of pieces
 * (in this order):
 *
 * - probe/attach/detach
 * - generic transfer routines
 * - BBB
 * - CBI
 * - CBI_I (in addition to functions from CBI)
 * - CAM (Common Access Method)
 * - SCSI
 * - UFI
 * - 8070i (ATAPI)
 *
 * The protocols are implemented using a state machine, for the transfers as
 * well as for the resets. The state machine is contained in umass_*_state.
 * The state machine is started through either umass_*_transfer or
 * umass_*_reset.
 *
 * The reason for doing this is 
 * a) umass performs a lot better this way and 
 * b) it avoids using tsleep from interrupt context (for example after a failed
 * transfer).
 */

#include <jos.h>
#include <jutil.h>

#ifdef CONFIG_JUMASS_ASYNC
# include "jms_async.h"
#endif

#ifndef SINGLE_THREADED
# define CBI_I
#endif

#define JUSB_UMASS_USE_CBI

#define TEST_UNIT_READY         0x00
#define REQUEST_SENSE           0x03
#define READ_6                  0x08
#define WRITE_6                 0x0a
#define INQUIRY                 0x12
#define MODE_SELECT_6           0x15
#define MODE_SENSE_6            0x1a
#define START_STOP_UNIT         0x1b
#define START_STOP              0x1b
#define RESERVE                 0x16
#define RELEASE                 0x17
#define RECEIVE_DIAGNOSTIC      0x1c
#define SEND_DIAGNOSTIC         0x1d
#define PREVENT_ALLOW           0x1e
#define READ_CAPACITY           0x25
#define READ_10                 0x28
#define WRITE_10                0x2a
#define POSITION_TO_ELEMENT     0x2b
#define VERIFY_10               0x2f
#define SYNCHRONIZE_CACHE       0x35
#define WRITE_BUFFER            0x3b
#define READ_BUFFER             0x3c
#define CHANGE_DEFINITION       0x40
#define LOG_SELECT              0x4c
#define LOG_SENSE               0x4d
#define MODE_SELECT_10          0x55
#define MODE_SENSE_10           0x5A
#define READ_16                 0x88
#define WRITE_16                0x8a
#define SERVICE_ACTION_IN       0x9e
#define REPORT_LUNS             0xA0
#define MOVE_MEDIUM             0xa5
#define READ_12                 0xa8
#define WRITE_12                0xaa
#define READ_ELEMENT_STATUS     0xb8

#define CAM_MAX_CDBLEN          16

#define SSS_START               0x01
#define SHORT_INQUIRY_LENGTH    36
#define REZERO_UNIT             0x01
#define START_STOP_UNIT_SIZE    7

#include <jusb.h>
#include <usbdi.h>
#include <usbdevs.h>

#include "umass.h"
#include "jms_transport.h"

/* Direction for umass_*_transfer */
#define DIR_NONE        0
#define DIR_IN          1
#define DIR_OUT         2

#define UMASS_TIMEOUT   2000 /* msecs */

/* Bulk-Only features */
#define UR_BBB_RESET            0xff            /* Bulk-Only reset */
#define UR_BBB_GET_MAX_LUN      0xfe            /* Get maximum lun */

#define CBWSIGNATURE            0x43425355
#define CBWFLAGS_OUT            0x00
#define CBWFLAGS_IN             0x80
#define CBWCDBLENGTH            16
#define UMASS_BBB_CBW_SIZE      31

/* Command Block Wrapper */
typedef struct {
    uDWord      dCBWSignature;
    uDWord      dCBWTag;
    uDWord      dCBWDataTransferLength;
    uByte       bCBWFlags;
    uByte       bCBWLUN;
    uByte       bCDBLength;
    uByte       CBWCDB[CBWCDBLENGTH];
} umass_bbb_cbw_t;

#define CSWSIGNATURE            0x53425355
#define CSWSIGNATURE_OLYMPUS_C1 0x55425355
#define CSWSTATUS_GOOD          0x0
#define CSWSTATUS_FAILED        0x1
#define CSWSTATUS_PHASE         0x2
#define UMASS_BBB_CSW_SIZE      13

/* Command Status Wrapper */
typedef struct {
    uDWord      dCSWSignature;
    uDWord      dCSWTag;
    uDWord      dCSWDataResidue;
    uByte       bCSWStatus;
} umass_bbb_csw_t;

/* CBI features */
#define UR_CBI_ADSC     0x00

#ifdef JUSB_UMASS_USE_CBI
typedef char umass_cbi_cbl_t[16];       /* Command block */

typedef union {
    struct {
        unsigned char   type;
        #define IDB_TYPE_CCI            0x00
        unsigned char   value;
        #define IDB_VALUE_PASS          0x00
        #define IDB_VALUE_FAIL          0x01
        #define IDB_VALUE_PHASE         0x02
        #define IDB_VALUE_PERSISTENT    0x03
        #define IDB_VALUE_STATUS_MASK   0x03
    } common;

    struct {
        unsigned char   asc;
        unsigned char   ascq;
    } ufi;
} umass_cbi_sbl_t;
#endif

struct umass_softc;             

typedef void (*transfer_cb_f) (struct umass_softc *sc, void *priv,
    juint32_t residue, jint_t status);
typedef usbd_status (*wire_reset_f) (struct umass_softc *sc, jint_t status);
typedef jint_t (*wire_transfer_f) (struct umass_softc *sc, jint_t lun,
    void *cmd, jint_t cmdlen, void *data, juint32_t datalen, jint_t flags, 
    jint_t dir, juint32_t timeout, transfer_cb_f cb, void *priv);
typedef void (*wire_state_f) (usbd_xfer_handle xfer,
    usbd_private_handle priv, usbd_status err);
typedef jint_t (*command_transform_f) (struct umass_softc *sc,
    unsigned char *cmd, jint_t cmdlen, unsigned char **rcmd, jint_t *rcmdlen);


struct umass_devdescr_t {
    juint32_t    vid;
#       define VID_WILDCARD     0xffffffff
#       define VID_EOT          0xfffffffe
    juint32_t    pid;
#       define PID_WILDCARD     0xffffffff
#       define PID_EOT          0xfffffffe
    juint32_t    rid;
#       define RID_WILDCARD     0xffffffff
#       define RID_EOT          0xfffffffe

    /* wire and command protocol */
    juint16_t    proto;
#       define UMASS_PROTO_BBB          0x0001  /* USB wire protocol */
#       define UMASS_PROTO_CBI          0x0002
#       define UMASS_PROTO_CBI_I        0x0004
#       define UMASS_PROTO_WIRE         0x00ff  /* USB wire protocol mask */
#       define UMASS_PROTO_SCSI         0x0100  /* command protocol */
#       define UMASS_PROTO_ATAPI        0x0200
#       define UMASS_PROTO_UFI          0x0400
#       define UMASS_PROTO_RBC          0x0800
#       define UMASS_PROTO_COMMAND      0xff00  /* command protocol mask */

    /* Device specific quirks */
    juint16_t    quirks;
#       define NO_QUIRKS                0x0000
    /* The drive does not support Test Unit Ready. Convert to Start Unit
    */
#       define NO_TEST_UNIT_READY       0x0001
    /* The drive does not reset the Unit Attention state after REQUEST
     * SENSE has been sent. The INQUIRY command does not reset the UA
     * either, and so CAM runs in circles trying to retrieve the initial
     * INQUIRY data.
     */
#       define RS_NO_CLEAR_UA           0x0002
    /* The drive does not support START STOP.  */
#       define NO_START_STOP            0x0004
    /* Don't ask for full inquiry data (255b).  */
#       define FORCE_SHORT_INQUIRY      0x0008
    /* Needs to be initialised the Shuttle way */
#       define SHUTTLE_INIT             0x0010
    /* Drive needs to be switched to alternate iface 1 */
#       define ALT_IFACE_1              0x0020
    /* Drive does not do 1Mb/s, but just floppy speeds (20kb/s) */
#       define FLOPPY_SPEED             0x0040
    /* The device can't count and gets the residue of transfers wrong */
#       define IGNORE_RESIDUE           0x0080
    /* No GetMaxLun call */
#       define NO_GETMAXLUN             0x0100
    /* The device uses a weird CSWSIGNATURE. */
#       define WRONG_CSWSIG             0x0200
    /* Device cannot handle INQUIRY so fake a generic response */
#       define NO_INQUIRY               0x0400
    /* Device cannot handle INQUIRY EVPD, return CHECK CONDITION */
#       define NO_INQUIRY_EVPD          0x0800
    /* The device has Residue endianess in BE */
#       define SWAP_RESIDUE             0x1000
};

#ifndef JUSB_UMASS_DECLARATIONS
static struct umass_devdescr_t umass_devdescrs[] = {
    { USB_VENDOR_ASAHIOPTICAL, PID_WILDCARD, RID_WILDCARD,
        UMASS_PROTO_ATAPI | UMASS_PROTO_CBI_I,
        RS_NO_CLEAR_UA
    },
    { USB_VENDOR_FUJIPHOTO, USB_PRODUCT_FUJIPHOTO_MASS0100, RID_WILDCARD,
        UMASS_PROTO_ATAPI | UMASS_PROTO_CBI_I,
        RS_NO_CLEAR_UA
    },
    { USB_VENDOR_GENESYS,  USB_PRODUCT_GENESYS_GL641USB2IDE, RID_WILDCARD,
        UMASS_PROTO_SCSI | UMASS_PROTO_BBB,
        FORCE_SHORT_INQUIRY | NO_START_STOP | IGNORE_RESIDUE
    },
    { USB_VENDOR_GENESYS,  USB_PRODUCT_GENESYS_GL641USB2IDE_2, RID_WILDCARD,
        UMASS_PROTO_ATAPI | UMASS_PROTO_BBB,
        FORCE_SHORT_INQUIRY | NO_START_STOP | IGNORE_RESIDUE
    },
    { USB_VENDOR_GENESYS,  USB_PRODUCT_GENESYS_GL641USB, RID_WILDCARD,
        UMASS_PROTO_SCSI | UMASS_PROTO_BBB,
        FORCE_SHORT_INQUIRY | NO_START_STOP | IGNORE_RESIDUE
    },
    { USB_VENDOR_HITACHI, USB_PRODUCT_HITACHI_DVDCAM_USB, RID_WILDCARD,
        UMASS_PROTO_ATAPI | UMASS_PROTO_CBI_I,
        NO_INQUIRY
    },
    { USB_VENDOR_HP, USB_PRODUCT_HP_CDW8200, RID_WILDCARD,
        UMASS_PROTO_ATAPI | UMASS_PROTO_CBI_I,
        NO_TEST_UNIT_READY | NO_START_STOP
    },
    { USB_VENDOR_INSYSTEM, USB_PRODUCT_INSYSTEM_USBCABLE, RID_WILDCARD,
        UMASS_PROTO_ATAPI | UMASS_PROTO_CBI,
        NO_TEST_UNIT_READY | NO_START_STOP | ALT_IFACE_1
    },
    { USB_VENDOR_IODATA, USB_PRODUCT_IODATA_IU_CD2, RID_WILDCARD,
        UMASS_PROTO_SCSI | UMASS_PROTO_BBB,
        NO_QUIRKS
    },
    { USB_VENDOR_IODATA, USB_PRODUCT_IODATA_DVR_UEH8, RID_WILDCARD,
        UMASS_PROTO_SCSI | UMASS_PROTO_BBB,
        NO_QUIRKS
    },
    { USB_VENDOR_IOMEGA, USB_PRODUCT_IOMEGA_ZIP100, RID_WILDCARD,
        UMASS_PROTO_SCSI | UMASS_PROTO_BBB,
        NO_TEST_UNIT_READY
    },
    { USB_VENDOR_LOGITEC, USB_PRODUCT_LOGITEC_LDR_H443SU2, RID_WILDCARD,
        UMASS_PROTO_SCSI,
        NO_QUIRKS
    },
    { USB_VENDOR_LOGITEC, USB_PRODUCT_LOGITEC_LDR_H443U2, RID_WILDCARD,
        UMASS_PROTO_SCSI | UMASS_PROTO_BBB,
        NO_QUIRKS
    },
    { USB_VENDOR_MELCO,  USB_PRODUCT_MELCO_DUBPXXG, RID_WILDCARD,
        UMASS_PROTO_SCSI | UMASS_PROTO_BBB,
        FORCE_SHORT_INQUIRY | NO_START_STOP | IGNORE_RESIDUE
    },
    { USB_VENDOR_MICROTECH, USB_PRODUCT_MICROTECH_DPCM, RID_WILDCARD,
        UMASS_PROTO_SCSI | UMASS_PROTO_CBI,
        NO_TEST_UNIT_READY | NO_START_STOP
    },
    { USB_VENDOR_MSYSTEMS, USB_PRODUCT_MSYSTEMS_DISKONKEY, RID_WILDCARD,
        UMASS_PROTO_SCSI | UMASS_PROTO_BBB,
        IGNORE_RESIDUE | NO_GETMAXLUN | RS_NO_CLEAR_UA
    },
    { USB_VENDOR_MSYSTEMS, USB_PRODUCT_MSYSTEMS_DISKONKEY2, RID_WILDCARD,
        UMASS_PROTO_ATAPI | UMASS_PROTO_BBB,
        NO_QUIRKS
    },
    { USB_VENDOR_NEODIO, USB_PRODUCT_NEODIO_ND3260, RID_WILDCARD,
        UMASS_PROTO_SCSI | UMASS_PROTO_BBB,
        FORCE_SHORT_INQUIRY
    },
    { USB_VENDOR_OLYMPUS, USB_PRODUCT_OLYMPUS_C1, RID_WILDCARD,
        UMASS_PROTO_SCSI | UMASS_PROTO_BBB,
        WRONG_CSWSIG
    },
    { USB_VENDOR_ONSPEC, USB_PRODUCT_ONSPEC_UCF100, RID_WILDCARD,
        UMASS_PROTO_ATAPI | UMASS_PROTO_BBB,
        NO_INQUIRY | NO_GETMAXLUN
    },
    { USB_VENDOR_OURS_TECHNOLOGY, USB_PRODUCT_OURS_TECHNOLOGY_SFD_0032,
        RID_WILDCARD,
        UMASS_PROTO_SCSI | UMASS_PROTO_BBB,
        IGNORE_RESIDUE 
    },
    { USB_VENDOR_OURS_TECHNOLOGY, USB_PRODUCT_OURS_TECHNOLOGY_STORAGE_DEVICE,
        RID_WILDCARD,
        UMASS_PROTO_SCSI | UMASS_PROTO_BBB,
        IGNORE_RESIDUE 
    },
    { USB_VENDOR_PANASONIC, USB_PRODUCT_PANASONIC_KXLCB20AN, RID_WILDCARD,
        UMASS_PROTO_SCSI | UMASS_PROTO_BBB,
        NO_QUIRKS
    },
    { USB_VENDOR_PANASONIC, USB_PRODUCT_PANASONIC_KXLCB35AN, RID_WILDCARD,
        UMASS_PROTO_SCSI | UMASS_PROTO_BBB,
        NO_QUIRKS
    },
    { USB_VENDOR_PLEXTOR, USB_PRODUCT_PLEXTOR_40_12_40U, RID_WILDCARD,
        UMASS_PROTO_SCSI | UMASS_PROTO_BBB,
        NO_TEST_UNIT_READY
    },
    { USB_VENDOR_PNY, USB_PRODUCT_PNY_ATTACHE2, RID_WILDCARD,
        UMASS_PROTO_SCSI | UMASS_PROTO_BBB,
        IGNORE_RESIDUE
    },
    { USB_VENDOR_SANDISK, USB_PRODUCT_SANDISK_SDCZ4_256, RID_WILDCARD,
        UMASS_PROTO_SCSI | UMASS_PROTO_BBB,
        SWAP_RESIDUE
    },
    { USB_VENDOR_SANDISK, USB_PRODUCT_SANDISK_SDCZ2_256, RID_WILDCARD,
        UMASS_PROTO_SCSI | UMASS_PROTO_BBB,
        IGNORE_RESIDUE
    },
    { USB_VENDOR_SCANLOGIC, USB_PRODUCT_SCANLOGIC_SL11R, RID_WILDCARD,
        UMASS_PROTO_ATAPI | UMASS_PROTO_BBB,
        NO_INQUIRY
    },
    { USB_VENDOR_SHUTTLE, USB_PRODUCT_SHUTTLE_EUSB, RID_WILDCARD,
        UMASS_PROTO_ATAPI | UMASS_PROTO_CBI_I,
        NO_TEST_UNIT_READY | NO_START_STOP | SHUTTLE_INIT
    },
    { USB_VENDOR_SIGMATEL, USB_PRODUCT_SIGMATEL_I_BEAD100, RID_WILDCARD,
        UMASS_PROTO_SCSI | UMASS_PROTO_BBB,
        SHUTTLE_INIT
    },
    { USB_VENDOR_SIIG, USB_PRODUCT_SIIG_WINTERREADER, RID_WILDCARD,
        UMASS_PROTO_SCSI | UMASS_PROTO_BBB,
        IGNORE_RESIDUE
    },
    { USB_VENDOR_SONY, USB_PRODUCT_SONY_DSC, RID_WILDCARD,
        UMASS_PROTO_RBC | UMASS_PROTO_CBI,
        NO_QUIRKS
    },
    { USB_VENDOR_SONY, USB_PRODUCT_SONY_HANDYCAM, RID_WILDCARD,
        UMASS_PROTO_RBC | UMASS_PROTO_CBI,
        NO_QUIRKS
    },
    { USB_VENDOR_SONY, USB_PRODUCT_SONY_MSC, RID_WILDCARD,
        UMASS_PROTO_RBC | UMASS_PROTO_CBI,
        NO_QUIRKS
    },
    { USB_VENDOR_TREK, USB_PRODUCT_TREK_THUMBDRIVE_8MB, RID_WILDCARD,
        UMASS_PROTO_ATAPI | UMASS_PROTO_BBB,
        IGNORE_RESIDUE
    },
    { USB_VENDOR_WESTERN,  USB_PRODUCT_WESTERN_EXTHDD, RID_WILDCARD,
        UMASS_PROTO_SCSI | UMASS_PROTO_BBB,
        FORCE_SHORT_INQUIRY | NO_START_STOP | IGNORE_RESIDUE
    },
    { USB_VENDOR_YANO,  USB_PRODUCT_YANO_U640MO, RID_WILDCARD,
        UMASS_PROTO_ATAPI | UMASS_PROTO_CBI_I,
        FORCE_SHORT_INQUIRY
    },
    { USB_VENDOR_IRIVER, USB_PRODUCT_IRIVER_T10, RID_WILDCARD,
        UMASS_PROTO_SCSI | UMASS_PROTO_BBB,
        IGNORE_RESIDUE
    },
    { USB_VENDOR_IRIVER, USB_PRODUCT_IRIVER_N11, RID_WILDCARD,
        UMASS_PROTO_SCSI | UMASS_PROTO_BBB,
        IGNORE_RESIDUE
    },
    { USB_VENDOR_IRIVER, USB_PRODUCT_IRIVER_T30, RID_WILDCARD,
        UMASS_PROTO_SCSI | UMASS_PROTO_BBB,
        IGNORE_RESIDUE
    },
    { USB_VENDOR_IRIVER, USB_PRODUCT_IRIVER_U10, RID_WILDCARD,
        UMASS_PROTO_SCSI | UMASS_PROTO_BBB,
        IGNORE_RESIDUE
    },
    { USB_VENDOR_IRIVER, USB_PRODUCT_IRIVER_N12, RID_WILDCARD,
        UMASS_PROTO_SCSI | UMASS_PROTO_BBB,
        IGNORE_RESIDUE
    },
    { USB_VENDOR_IRIVER, USB_PRODUCT_IRIVER_H10, RID_WILDCARD,
        UMASS_PROTO_SCSI | UMASS_PROTO_BBB,
        IGNORE_RESIDUE
    },
    { VID_EOT, PID_EOT, RID_EOT, 0, 0 }
};
#endif

/* the per device structure */
struct umass_softc {
    jdevice_t           sc_dev;         /* base device */
    usbd_device_handle  sc_udev;        /* USB device */

    void                *umass_sim;     /* SCSI Interface Module */

    unsigned char       flags;          /* various device flags */
#       define UMASS_FLAGS_GONE         0x01    /* devices is no more */

    juint16_t            proto;          /* wire and cmd protocol */
    juint16_t            quirks;         /* they got it almost right */

    jint_t               ifaceno;        /* MS iface number */

    juint8_t             bulkin;         /* bulk-in Endpoint Address */
    juint8_t             bulkout;        /* bulk-out Endpoint Address */
    juint8_t             intrin;         /* intr-in Endp. (CBI) */
    usbd_pipe_handle    bulkin_pipe;
    usbd_pipe_handle    bulkout_pipe;
    usbd_pipe_handle    intrin_pipe;

    /* Reset the device in a wire protocol specific way */
    wire_reset_f        reset;

    /* The start of a wire transfer. It prepares the whole transfer (cmd,
     * data, and status stage) and initiates it. It is up to the state
     * machine (below) to handle the various stages and errors in these
     */
    wire_transfer_f     transfer;

    /* The state machine, handling the various states during a transfer */
    wire_state_f        state;

    /* The command transform function is used to conver the SCSI commands
     * into their derivatives, like UFI, ATAPI, and friends.
     */
    command_transform_f transform;      /* command transform */

    /* Bulk specific variables for transfers in progress */
    umass_bbb_cbw_t     cbw;    /* command block wrapper */
    umass_bbb_csw_t     csw;    /* command status wrapper*/
#ifdef JUSB_UMASS_USE_CBI
    /* CBI specific variables for transfers in progress */
    umass_cbi_cbl_t     cbl;    /* command block */
    umass_cbi_sbl_t     sbl;    /* status block */
#endif

    /* generic variables for transfers in progress */
    /* ctrl transfer requests */
    usb_device_request_t request;

    /* xfer handles
     * Most of our operations are initiated from interrupt context, so
     * we need to avoid using the one that is in use. We want to avoid
     * allocating them in the interrupt context as well.
     */
    /* indices into array below */
#       define XFER_BBB_CBW             0       /* Bulk-Only */
#       define XFER_BBB_DATA            1
#       define XFER_BBB_DCLEAR          2
#       define XFER_BBB_CSW1            3
#       define XFER_BBB_CSW2            4
#       define XFER_BBB_SCLEAR          5
#       define XFER_BBB_RESET1          6
#       define XFER_BBB_RESET2          7
#       define XFER_BBB_RESET3          8

#       define XFER_CBI_CB              0       /* CBI */
#       define XFER_CBI_DATA            1
#       define XFER_CBI_STATUS          2
#       define XFER_CBI_DCLEAR          3
#       define XFER_CBI_SCLEAR          4
#       define XFER_CBI_RESET1          5
#       define XFER_CBI_RESET2          6
#       define XFER_CBI_RESET3          7

#       define XFER_NR                  9       /* maximum number */

    usbd_xfer_handle    transfer_xfer[XFER_NR]; /* for ctrl xfers */

    jint_t               transfer_dir;           /* data direction */
    void                *transfer_data;         /* data buffer */
    juint32_t            transfer_datalen;       /* (maximum) length */
    jint_t               transfer_flags;         /* Flags for data transfer */
    juint32_t            transfer_actlen;        /* actual length */
    transfer_cb_f       transfer_cb;            /* callback */
    void                *transfer_priv;         /* for callback */
    jint_t               transfer_status;

    jint_t               transfer_state;
#       define TSTATE_ATTACH                    0       /* in attach */
#       define TSTATE_IDLE                      1
#       define TSTATE_BBB_COMMAND               2       /* CBW transfer */
#       define TSTATE_BBB_DATA                  3       /* Data transfer */
#       define TSTATE_BBB_DCLEAR                4       /* clear endpt stall */
#       define TSTATE_BBB_STATUS1               5       /* clear endpt stall */
#       define TSTATE_BBB_SCLEAR                6       /* clear endpt stall */
#       define TSTATE_BBB_STATUS2               7       /* CSW transfer */
#       define TSTATE_BBB_RESET1                8       /* reset command */
#       define TSTATE_BBB_RESET2                9       /* in clear stall */
#       define TSTATE_BBB_RESET3                10      /* out clear stall */
#       define TSTATE_CBI_COMMAND               11      /* command transfer */
#       define TSTATE_CBI_DATA                  12      /* data transfer */
#       define TSTATE_CBI_STATUS                13      /* status transfer */
#       define TSTATE_CBI_DCLEAR                14      /* clear ep stall */
#       define TSTATE_CBI_SCLEAR                15      /* clear ep stall */
#       define TSTATE_CBI_RESET1                16      /* reset command */
#       define TSTATE_CBI_RESET2                17      /* in clear stall */
#       define TSTATE_CBI_RESET3                18      /* out clear stall */
#       define TSTATE_STATES                    19      /* # of states above */


    /* SCSI/CAM specific variables */
    unsigned char       cam_scsi_command[CAM_MAX_CDBLEN];
    juint32_t            timeout;                /* in msecs */
    juint8_t             maxlun;                 /* maximum LUN number */
};

#ifndef JUSB_UMASS_DECLARATIONS

#ifdef JDEBUG
char *states[TSTATE_STATES+1] = {
    /* should be kept in sync with the list at transfer_state */
    "Attach",
    "Idle",
    "BBB CBW",
    "BBB Data",
    "BBB Data bulk-in/-out clear stall",
    "BBB CSW, 1st attempt",
    "BBB CSW bulk-in clear stall",
    "BBB CSW, 2nd attempt",
    "BBB Reset",
    "BBB bulk-in clear stall",
    "BBB bulk-out clear stall",
    "CBI Command",
    "CBI Data",
    "CBI Status",
    "CBI Data bulk-in/-out clear stall",
    "CBI Status intr-in clear stall",
    "CBI Reset",
    "CBI bulk-in clear stall",
    "CBI bulk-out clear stall",
    NULL
};
#endif

/* USB device probe/attach/detach functions */
static jint_t umass_match_proto (struct umass_softc *sc,
    usbd_interface_handle iface, usbd_device_handle udev);

/* quirk functions */
static void umass_init_shuttle (struct umass_softc *sc);

/* generic transfer functions */
static usbd_status umass_setup_transfer (struct umass_softc *sc,
    usbd_pipe_handle pipe, void *buffer, juint32_t buflen, jint_t flags,
    usbd_xfer_handle xfer);
static usbd_status umass_setup_ctrl_transfer (struct umass_softc *sc,
    usb_device_request_t *req, void *buffer, juint32_t buflen, jint_t flags, 
    usbd_xfer_handle xfer);
static usbd_status umass_clear_endpoint_stall (struct umass_softc *sc,
    juint8_t endpt, usbd_pipe_handle pipe, jint_t state, usbd_xfer_handle xfer);

/* Bulk-Only related functions */
static usbd_status umass_bbb_reset (struct umass_softc *sc, jint_t status);
static jint_t umass_bbb_transfer (struct umass_softc *sc, jint_t lun, void *cmd,
    jint_t cmdlen, void *data, juint32_t datalen, jint_t flags, jint_t dir, 
    juint32_t timeout, transfer_cb_f cb, void *priv);
static void umass_bbb_state (usbd_xfer_handle xfer, usbd_private_handle priv,
    usbd_status err);
static juint8_t umass_bbb_get_max_lun (struct umass_softc *sc, 
    usbd_interface_handle iface);

#ifdef JUSB_UMASS_USE_CBI
/* CBI related functions */
static usbd_status umass_cbi_adsc (struct umass_softc *sc, char *buffer,
    jint_t buflen, usbd_xfer_handle xfer);
static usbd_status umass_cbi_reset (struct umass_softc *sc, jint_t status);
static jint_t umass_cbi_transfer (struct umass_softc *sc, jint_t lun, void *cmd, 
    jint_t cmdlen, void *data, juint32_t datalen, jint_t flags, jint_t dir,
    juint32_t timeout, transfer_cb_f cb, void *priv);
static void umass_cbi_state (usbd_xfer_handle xfer, usbd_private_handle priv,
    usbd_status err);
#endif

/* Attach related functions */
void *umass_attach_up(void *softc);
static jint_t umass_detach_up (struct umass_softc *sc);

/* SCSI specific functions */
static jint_t umass_scsi_transform (struct umass_softc *sc, unsigned char *cmd, 
    jint_t cmdlen, unsigned char **rcmd, jint_t *rcmdlen);

/* UFI specific functions */
#define UFI_COMMAND_LENGTH      12      /* UFI commands are always 12 bytes */
static jint_t umass_ufi_transform (struct umass_softc *sc, unsigned char *cmd,
    jint_t cmdlen, unsigned char **rcmd, jint_t *rcmdlen);

/* ATAPI (8070i) specific functions */
#define ATAPI_COMMAND_LENGTH    12      /* ATAPI commands are always 12 bytes */
static jint_t umass_atapi_transform (struct umass_softc *sc, unsigned char *cmd,
    jint_t cmdlen, unsigned char **rcmd, jint_t *rcmdlen);

/* RBC specific functions */
static jint_t umass_rbc_transform (struct umass_softc *sc, unsigned char *cmd,
    jint_t cmdlen, unsigned char **rcmd, jint_t *rcmdlen);

#ifdef JDEBUG
/* General debugging functions */
static void umass_bbb_dump_cbw (struct umass_softc *sc, umass_bbb_cbw_t *cbw);
static void umass_bbb_dump_csw (struct umass_softc *sc, umass_bbb_csw_t *csw);
#ifdef JUSB_UMASS_USE_CBI
static void umass_cbi_dump_cmd (struct umass_softc *sc, void *cmd, 
    jint_t cmdlen);
#endif
static void umass_dump_buffer (struct umass_softc *sc, juint8_t *buffer,
    juint32_t buflen, juint32_t printlen);
#endif

/*
 * USB device probe/attach/detach
 */

static jresult_t umass_detach(jdevice_t self);

/*
 * Match the device we are seeing with the devices supported. Fill in the
 * description in the softc accordingly. This function is called from both
 * probe and attach.
 */

static jint_t umass_match_proto(struct umass_softc *sc,
    usbd_interface_handle iface, usbd_device_handle udev)
{
    usb_device_descriptor_t *dd;
    usb_interface_descriptor_t *id;
    jint_t i;
    jint_t found = 0;

    sc->sc_udev = udev;
    sc->proto = 0;
    sc->quirks = 0;

    dd = usbd_get_device_descriptor(udev);

    /* An entry specifically for Y-E Data devices as they don't fit in the
     * device description table.
     */
    if (UGETW(dd->idVendor) == USB_VENDOR_YEDATA
        && UGETW(dd->idProduct) == USB_PRODUCT_YEDATA_FLASHBUSTERU) 
    {
        /* Revisions < 1.28 do not handle the interrupt endpoint
         * very well.
         */
        if (UGETW(dd->bcdDevice) < 0x128) 
            sc->proto = UMASS_PROTO_UFI | UMASS_PROTO_CBI;
        else
            sc->proto = UMASS_PROTO_UFI | UMASS_PROTO_CBI_I;

        /*
         * Revisions < 1.28 do not have the TEST UNIT READY command
         * Revisions == 1.28 have a broken TEST UNIT READY
         */
        if (UGETW(dd->bcdDevice) <= 0x128)
            sc->quirks |= NO_TEST_UNIT_READY;

        sc->quirks |= RS_NO_CLEAR_UA | FLOPPY_SPEED;
        return(UMATCH_VENDOR_PRODUCT);
    }

    /* Check the list of supported devices for a match. While looking,
     * check for wildcarded and fully matched. First match wins.
     */
    for (i = 0; umass_devdescrs[i].vid != VID_EOT && !found; i++)
    {
        if (umass_devdescrs[i].vid == VID_WILDCARD &&
            umass_devdescrs[i].pid == PID_WILDCARD &&
            umass_devdescrs[i].rid == RID_WILDCARD) 
        {
            jprintf("umass: ignoring invalid wildcard quirk\n");
            continue;
        }
        if ((umass_devdescrs[i].vid == UGETW(dd->idVendor) ||
            umass_devdescrs[i].vid == VID_WILDCARD)
            && (umass_devdescrs[i].pid == UGETW(dd->idProduct) ||
            umass_devdescrs[i].pid == PID_WILDCARD)) 
        {
            if (umass_devdescrs[i].rid == RID_WILDCARD) 
            {
                sc->proto = umass_devdescrs[i].proto;
                sc->quirks = umass_devdescrs[i].quirks;
                return (UMATCH_VENDOR_PRODUCT);
            } 
            else if (umass_devdescrs[i].rid == UGETW(dd->bcdDevice)) 
            {
                sc->proto = umass_devdescrs[i].proto;
                sc->quirks = umass_devdescrs[i].quirks;
                return (UMATCH_VENDOR_PRODUCT_REV);
            } /* else RID does not match */
        }
    }

    /* Check for a standards compliant device */
    id = usbd_get_interface_descriptor(iface);
    if (id == NULL || id->bInterfaceClass != UICLASS_MASS)
        return(UMATCH_NONE);

    switch (id->bInterfaceSubClass) 
    {
    case UISUBCLASS_SCSI:
        sc->proto |= UMASS_PROTO_SCSI;
        break;
    case UISUBCLASS_UFI:
        sc->proto |= UMASS_PROTO_UFI;
        break;
    case UISUBCLASS_RBC:
        sc->proto |= UMASS_PROTO_RBC;
        break;
    case UISUBCLASS_SFF8020I:
    case UISUBCLASS_SFF8070I:
        sc->proto |= UMASS_PROTO_ATAPI;
        break;
    default:
        DBG_E(DMASS_UMASS, ("%s: Unsupported command protocol %d\n",
            j_device_get_nameunit(sc->sc_dev), 
            id->bInterfaceSubClass));
        return(UMATCH_NONE);
    }

    switch (id->bInterfaceProtocol) 
    {
    case UIPROTO_MASS_CBI:
        sc->proto |= UMASS_PROTO_CBI;
        break;
    case UIPROTO_MASS_CBI_I:
        sc->proto |= UMASS_PROTO_CBI_I;
        break;
    case UIPROTO_MASS_BBB_OLD:
    case UIPROTO_MASS_BBB:
        sc->proto |= UMASS_PROTO_BBB;
        break;
    default:
        DBG_E(DMASS_UMASS, ("%s: Unsupported wire protocol %d\n",
            j_device_get_nameunit(sc->sc_dev), 
            id->bInterfaceProtocol));
        return(UMATCH_NONE);
    }

    return(UMATCH_DEVCLASS_DEVSUBCLASS_DEVPROTO);
}

static jresult_t umass_probe(jdevice_t self)
{
    struct usb_attach_arg *uaa = j_device_get_ivars(self);
    struct umass_softc *sc = j_device_get_softc(self);

    sc->sc_dev = self;

    if (uaa->iface == NULL)
        return(UMATCH_NONE);

    return(umass_match_proto(sc, uaa->iface, uaa->device));
}

static jresult_t umass_attach(jdevice_t self)
{
    struct umass_softc *sc = j_device_get_softc(self);
    struct usb_attach_arg *uaa = j_device_get_ivars(self);

    usb_interface_descriptor_t *id;
    usb_endpoint_descriptor_t *ed;
    static char devinfo[1024];
    jint_t i;
    usbd_status err;

    /*
     * the softc struct is bzero-ed in device_set_driver. We can safely
     * call umass_detach without specifically initialising the struct.
     */

    usbd_devinfo(uaa->device, 0, devinfo, sizeof(devinfo));

    sc->sc_dev = self;
    j_device_set_desc(self, devinfo);
    device_printf(self, ("%s\n", devinfo));

    sc->ifaceno = uaa->ifaceno;

    /* initialise the proto and drive values in the umass_softc (again) */
    (void) umass_match_proto(sc, uaa->iface, uaa->device);

    id = usbd_get_interface_descriptor(uaa->iface);

    /* Get the maximum LUN supported by the device.
    */
    if ((sc->proto & UMASS_PROTO_WIRE) == UMASS_PROTO_BBB)
        sc->maxlun = umass_bbb_get_max_lun(sc, uaa->iface);
    else
        sc->maxlun = 0;

#ifdef JDEBUG
    jprintf("%s: ", j_device_get_nameunit(sc->sc_dev));
    switch (sc->proto&UMASS_PROTO_COMMAND) 
    {
    case UMASS_PROTO_SCSI:
        jprintf("SCSI");
        break;
    case UMASS_PROTO_ATAPI:
        jprintf("8070i (ATAPI)");
        break;
    case UMASS_PROTO_UFI:
        jprintf("UFI");
        break;
    case UMASS_PROTO_RBC:
        jprintf("RBC");
        break;
    default:
        jprintf("(unknown 0x%02x)", sc->proto&UMASS_PROTO_COMMAND);
        break;
    }
    jprintf(" over ");
    switch (sc->proto&UMASS_PROTO_WIRE) 
    {
    case UMASS_PROTO_BBB:
        jprintf("Bulk-Only");
        break;
    case UMASS_PROTO_CBI:               /* uses Comand/Bulk pipes */
        jprintf("CBI");
        break;
    case UMASS_PROTO_CBI_I:             /* uses Comand/Bulk/Interrupt pipes */
        jprintf("CBI with CCI");
#ifndef CBI_I
        jprintf(" (using CBI)");
#endif
        break;
    default:
        jprintf("(unknown 0x%02x)", sc->proto&UMASS_PROTO_WIRE);
    }
    
    jprintf("; Luns = %d ", sc->maxlun + 1);

    jprintf("; quirks = 0x%04x\n", sc->quirks);
#endif

#ifndef CBI_I
    if (sc->proto & UMASS_PROTO_CBI_I) 
        sc->proto = (sc->proto & ~UMASS_PROTO_CBI_I) | UMASS_PROTO_CBI;
#endif

    if (sc->quirks & ALT_IFACE_1)
    {
        err = usbd_set_interface(uaa->iface, 1);
        if (err) 
        {
            DBG_E(DMASS_UMASS, ("%s: could not switch to Alt Interface %d\n",
                j_device_get_nameunit(sc->sc_dev), 1));
            umass_detach(self);
            return JENXIO;
        }
    }

    /*
     * In addition to the Control endpoint the following endpoints
     * are required:
     * a) bulk-in endpoint.
     * b) bulk-out endpoint.
     * and for Control/Bulk/Interrupt with CCI (CBI_I)
     * c) intr-in
     *
     * The endpoint addresses are not fixed, so we have to read them
     * from the device descriptors of the current interface.
     */
    for (i = 0 ; i < id->bNumEndpoints ; i++) 
    {
        ed = usbd_interface2endpoint_descriptor(uaa->iface, (juint8_t)i);
        if (!ed) 
        {
            jprintf("%s: could not read endpoint descriptor\n",
                j_device_get_nameunit(sc->sc_dev));
            umass_detach(self);
            return JENXIO;
        }
        if (UE_GET_DIR(ed->bEndpointAddress) == UE_DIR_IN
            && (ed->bmAttributes & UE_XFERTYPE) == UE_BULK) 
        {
            sc->bulkin = ed->bEndpointAddress;
        } 
        else if (UE_GET_DIR(ed->bEndpointAddress) == UE_DIR_OUT
            && (ed->bmAttributes & UE_XFERTYPE) == UE_BULK) 
        {
            sc->bulkout = ed->bEndpointAddress;
        }
        else if (sc->proto & UMASS_PROTO_CBI_I
            && UE_GET_DIR(ed->bEndpointAddress) == UE_DIR_IN
            && (ed->bmAttributes & UE_XFERTYPE) == UE_INTERRUPT) 
        {
            sc->intrin = ed->bEndpointAddress;
#ifdef JDEBUG
            if (UGETW(ed->wMaxPacketSize) > 2) 
            {
                DBG_W(DMASS_UMASS, ("%s: intr size is %d\n",
                    j_device_get_nameunit(sc->sc_dev),
                    UGETW(ed->wMaxPacketSize)));
            }
#endif
        }
    }

    /* check whether we found all the endpoints we need */
    if (!sc->bulkin || !sc->bulkout
        || (sc->proto & UMASS_PROTO_CBI_I && !sc->intrin) ) 
    {
        DBG_E(DMASS_UMASS, ("%s: endpoint not found %d/%d/%d\n",
            j_device_get_nameunit(sc->sc_dev),
            sc->bulkin, sc->bulkout, sc->intrin));
        umass_detach(self);
        return JENXIO;
    }

    /* Open the bulk-in and -out pipe */
    err = usbd_open_pipe(uaa->iface, sc->bulkout,
        USBD_EXCLUSIVE_USE, &sc->bulkout_pipe);
    if (err) 
    {
        DBG_E(DMASS_UMASS, ("%s: cannot open %d-out pipe (bulk)\n",
            j_device_get_nameunit(sc->sc_dev), sc->bulkout));
        umass_detach(self);
        return JENXIO;
    }
    err = usbd_open_pipe(uaa->iface, sc->bulkin,
        USBD_EXCLUSIVE_USE, &sc->bulkin_pipe);
    if (err) 
    {
        DBG_E(DMASS_UMASS, ("%s: could not open %d-in pipe (bulk)\n",
            j_device_get_nameunit(sc->sc_dev), sc->bulkin));
        umass_detach(self);
        return JENXIO;
    }
    /* Open the intr-in pipe if the protocol is CBI with CCI.
     * Note: early versions of the Zip drive do have an interrupt pipe, but
     * this pipe is unused.
     *
     * We do not open the interrupt pipe as an interrupt pipe, but as a
     * normal bulk endpoint. We send an IN transfer down the wire at the
     * appropriate time, because we know exactly when to expect data on
     * that endpoint. This saves bandwidth, but more important, makes the
     * code for handling the data on that endpoint simpler. No data
     * arriving concurrently.
     */
    if (sc->proto & UMASS_PROTO_CBI_I) 
    {
        err = usbd_open_pipe(uaa->iface, sc->intrin,
            USBD_EXCLUSIVE_USE, &sc->intrin_pipe);
        if (err) 
        {
            DBG_E(DMASS_UMASS, ("%s: couldn't open %d-in (intr)\n",
                j_device_get_nameunit(sc->sc_dev), sc->intrin));
            umass_detach(self);
            return JENXIO;
        }
    }

    /* initialisation of generic part */
    sc->transfer_state = TSTATE_ATTACH;

    /* request a sufficient number of xfer handles */
    for (i = 0; i < XFER_NR; i++) 
    {
        sc->transfer_xfer[i] = usbd_alloc_xfer(uaa->device);
        if (!sc->transfer_xfer[i]) 
        {
            DBG_E(DMASS_UMASS, ("%s: Out of memory\n",
                j_device_get_nameunit(sc->sc_dev)));
            umass_detach(self);
            return JENXIO;
        }
    }

    /* Initialise the wire protocol specific methods */
    if (sc->proto & UMASS_PROTO_BBB) 
    {
        sc->reset = umass_bbb_reset;
        sc->transfer = umass_bbb_transfer;
        sc->state = umass_bbb_state;
#ifdef JUSB_UMASS_USE_CBI
    }
    else if (sc->proto & (UMASS_PROTO_CBI|UMASS_PROTO_CBI_I)) 
    {
        sc->reset = umass_cbi_reset;
        sc->transfer = umass_cbi_transfer;
        sc->state = umass_cbi_state;
#endif
#ifdef JDEBUG
    }
    else 
    {
        j_panic("%s:%d: Unknown proto 0x%02x", __FILE__, __LINE__, sc->proto);
#endif
    }

    if (sc->proto & UMASS_PROTO_SCSI)
        sc->transform = umass_scsi_transform;
    else if (sc->proto & UMASS_PROTO_UFI)
        sc->transform = umass_ufi_transform;
    else if (sc->proto & UMASS_PROTO_ATAPI)
        sc->transform = umass_atapi_transform;
    else if (sc->proto & UMASS_PROTO_RBC)
        sc->transform = umass_rbc_transform;
#ifdef JDEBUG
    else
    {
        j_panic("No transformation defined for command proto 0x%02x",
            sc->proto & UMASS_PROTO_COMMAND);
    }
#endif

    /* From here onwards the device can be used. */

    if (sc->quirks & SHUTTLE_INIT)
        umass_init_shuttle(sc);

    if ((sc->proto & UMASS_PROTO_SCSI) ||
        (sc->proto & UMASS_PROTO_ATAPI) ||
        (sc->proto & UMASS_PROTO_UFI) ||
        (sc->proto & UMASS_PROTO_RBC)) 
    {
        /* register the SIM */
        sc->umass_sim = umass_attach_up(sc);
        if (!sc->umass_sim) 
        {
            umass_detach(self);
            DBG_W(DMASS_UMASS, ("UMASS: No attach !\n"));
            return JENXIO;
        }
        /* scan the new sim */
    } 
    else
    {
        j_panic("%s:%d: Unknown proto 0x%02x", __FILE__, __LINE__, sc->proto);
    }

    sc->transfer_state = TSTATE_IDLE;
    DBG_I(DMASS_UMASS, ("%s: Attach finished\n", 
        j_device_get_nameunit(sc->sc_dev)));

    return 0;
}

static jresult_t umass_detach(jdevice_t self)
{
    struct umass_softc *sc = j_device_get_softc(self);

    jresult_t err = 0;
    jint_t i;

    DBG_I(DMASS_UMASS, ("%s: detached\n", j_device_get_nameunit(sc->sc_dev)));

    sc->flags |= UMASS_FLAGS_GONE;

    if (sc->bulkout_pipe)
        usbd_abort_pipe(sc->bulkout_pipe);
    if (sc->bulkin_pipe)
        usbd_abort_pipe(sc->bulkin_pipe);
    if (sc->intrin_pipe)
        usbd_abort_pipe(sc->intrin_pipe);

    if ((sc->proto & UMASS_PROTO_SCSI) ||
        (sc->proto & UMASS_PROTO_ATAPI) ||
        (sc->proto & UMASS_PROTO_UFI) ||
        (sc->proto & UMASS_PROTO_RBC))
    {
        /* detach the SCSI host controller (SIM) */
        err = umass_detach_up(sc);
    }

    /* Abort & Close all the pipes */
    if (sc->bulkout_pipe)
        usbd_close_pipe(sc->bulkout_pipe);
    if (sc->bulkin_pipe)
        usbd_close_pipe(sc->bulkin_pipe);
    if (sc->intrin_pipe)
        usbd_close_pipe(sc->intrin_pipe);

    /* Free all xfers */
    for (i = 0; i < XFER_NR; i++)
    {
        if (sc->transfer_xfer[i])
            usbd_free_xfer(sc->transfer_xfer[i]);
    }
    return(err);
}

static void umass_init_shuttle(struct umass_softc *sc)
{
    usb_device_request_t req;
    juint8_t status[2];

    req.bmRequestType = UT_READ_VENDOR_DEVICE;
    req.bRequest = 1;   
    USETW(req.wValue, 0);
    USETW(req.wIndex, sc->ifaceno);
    USETW(req.wLength, sizeof status);
    (void)usbd_do_request(sc->sc_udev, &req, &status);

    DBG_V(DMASS_UMASS, ("%s: Shuttle init returned 0x%02x%02x\n",
        j_device_get_nameunit(sc->sc_dev), status[0], status[1]));
}

/*
 * Generic functions to handle transfers
 */

static usbd_status umass_setup_transfer(struct umass_softc *sc,
    usbd_pipe_handle pipe, void *buffer, juint32_t buflen, jint_t flags,
    usbd_xfer_handle xfer)
{
    usbd_status err;

    /* Initialise a USB transfer and then schedule it */

    (void) usbd_setup_xfer(xfer, pipe, (void *) sc, buffer, buflen, 
        (juint16_t)flags, sc->timeout, sc->state);

    err = usbd_transfer(xfer);
    if (err && err != USBD_IN_PROGRESS) 
    {
        DBG_W(DMASS_UMASS, ("%s: failed to setup transfer, %s\n",
            j_device_get_nameunit(sc->sc_dev), usbd_errstr(err)));
        return(err);
    }

    return (USBD_NORMAL_COMPLETION);
}


static usbd_status umass_setup_ctrl_transfer(struct umass_softc *sc,
    usb_device_request_t *req, void *buffer, juint32_t buflen, jint_t flags, 
    usbd_xfer_handle xfer)
{
    usbd_status err;

    /* Initialise a USB control transfer and then schedule it */

    (void) usbd_setup_default_xfer(xfer, sc->sc_udev, (void *)sc,
        sc->timeout, req, buffer, buflen, (juint16_t)flags, sc->state);

    err = usbd_transfer(xfer);
    if (err && err != USBD_IN_PROGRESS) 
    {
        DBG_W(DMASS_UMASS, ("%s: failed to setup ctrl transfer, %s\n",
            j_device_get_nameunit(sc->sc_dev), usbd_errstr(err)));

        /* do not reset, as this would make us loop */
        return(err);
    }

    return (USBD_NORMAL_COMPLETION);
}

static usbd_status umass_clear_endpoint_stall(struct umass_softc *sc,
    juint8_t endpt, usbd_pipe_handle pipe, jint_t state, usbd_xfer_handle xfer)
{
    DBG_I(DMASS_UMASS, ("%s: Clear endpoint 0x%02x stall\n",
        j_device_get_nameunit(sc->sc_dev), endpt));

    sc->transfer_state = state;

    usbd_clear_endpoint_toggle(pipe);

    sc->request.bmRequestType = UT_WRITE_ENDPOINT;
    sc->request.bRequest = UR_CLEAR_FEATURE;
    USETW(sc->request.wValue, UF_ENDPOINT_HALT);
    USETW(sc->request.wIndex, endpt);
    USETW(sc->request.wLength, 0);
    return umass_setup_ctrl_transfer(sc, &sc->request, NULL, 0, 0, 
        xfer);
}

/*
 * Bulk protocol specific functions
 */

static usbd_status umass_bbb_reset(struct umass_softc *sc, jint_t status)
{
    KASSERT(sc->proto & UMASS_PROTO_BBB,
        ("%s: umass_bbb_reset: wrong sc->proto 0x%02x\n",
        j_device_get_nameunit(sc->sc_dev), sc->proto));

    /*
     * Reset recovery (5.3.4 in Universal Serial Bus Mass Storage Class)
     *
     * For Reset Recovery the host shall issue in the following order:
     * a) a Bulk-Only Mass Storage Reset
     * b) a Clear Feature HALT to the Bulk-In endpoint
     * c) a Clear Feature HALT to the Bulk-Out endpoint
     *
     * This is done in 3 steps, states:
     * TSTATE_BBB_RESET1
     * TSTATE_BBB_RESET2
     * TSTATE_BBB_RESET3
     *
     * If the reset doesn't succeed, the device should be port reset.
     */

    DBG_I(DMASS_UMASS, ("%s: Bulk Reset\n", j_device_get_nameunit(sc->sc_dev)));

    sc->transfer_state = TSTATE_BBB_RESET1;
    sc->transfer_status = status;

    /* reset is a class specific interface write */
    sc->request.bmRequestType = UT_WRITE_CLASS_INTERFACE;
    sc->request.bRequest = UR_BBB_RESET;
    USETW(sc->request.wValue, 0);
    USETW(sc->request.wIndex, sc->ifaceno);
    USETW(sc->request.wLength, 0);

    return umass_setup_ctrl_transfer(sc, &sc->request, NULL, 0, 0,
        sc->transfer_xfer[XFER_BBB_RESET1]);
}

static jint_t umass_bbb_transfer(struct umass_softc *sc, jint_t lun, void *cmd, 
    jint_t cmdlen, void *data, juint32_t datalen, jint_t flags, jint_t dir, 
    juint32_t timeout, transfer_cb_f cb, void *priv)
{
    KASSERT(sc->proto & UMASS_PROTO_BBB,
        ("%s: umass_bbb_transfer: wrong sc->proto 0x%02x\n",
        j_device_get_nameunit(sc->sc_dev), sc->proto));

    /* Be a little generous. */
    sc->timeout = timeout + UMASS_TIMEOUT;

    /*
     * Do a Bulk-Only transfer with cmdlen bytes from cmd, possibly
     * a data phase of datalen bytes from/to the device and finally a
     * csw read phase.
     * If the data direction was inbound a maximum of datalen bytes
     * is stored in the buffer pointed to by data.
     *
     * umass_bbb_transfer initialises the transfer and lets the state
     * machine in umass_bbb_state handle the completion. It uses the
     * following states:
     * TSTATE_BBB_COMMAND
     *   -> TSTATE_BBB_DATA
     *   -> TSTATE_BBB_STATUS
     *   -> TSTATE_BBB_STATUS2
     *   -> TSTATE_BBB_IDLE
     *
     * An error in any of those states will invoke
     * umass_bbb_reset.
     */

    /* check the given arguments */
    KASSERT(datalen == 0 || data != NULL,
        ("%s: datalen > 0, but no buffer",j_device_get_nameunit(sc->sc_dev)));
    KASSERT(cmdlen <= CBWCDBLENGTH,
        ("%s: cmdlen exceeds CDB length in CBW (%d > %d)",
        j_device_get_nameunit(sc->sc_dev), cmdlen, CBWCDBLENGTH));
    KASSERT(dir == DIR_NONE || datalen > 0,
        ("%s: datalen == 0 while direction is not NONE\n",
        j_device_get_nameunit(sc->sc_dev)));
    KASSERT(datalen == 0 || dir != DIR_NONE,
        ("%s: direction is NONE while datalen is not zero\n",
        j_device_get_nameunit(sc->sc_dev)));
    /*
     * Determine the direction of the data transfer and the length.
     *
     * dCBWDataTransferLength (datalen) :
     *   This field indicates the number of bytes of data that the host
     *   intends to transfer on the IN or OUT Bulk endpoint(as indicated by
     *   the Direction bit) during the execution of this command. If this
     *   field is set to 0, the device will expect that no data will be
     *   transferred IN or OUT during this command, regardless of the value
     *   of the Direction bit defined in dCBWFlags.
     *
     * dCBWFlags (dir) :
     *   The bits of the Flags field are defined as follows:
     *     Bits 0-6  reserved
     *     Bit  7    Direction - this bit shall be ignored if the
     *                           dCBWDataTransferLength field is zero.
     *               0 = data Out from host to device
     *               1 = data In from device to host
     */

    /* Fill in the Command Block Wrapper
     * We fill in all the fields, so there is no need to bzero it first.
     */
    USETDW(sc->cbw.dCBWSignature, CBWSIGNATURE);
    /* We don't care about the initial value, as long as the values are
     * unique */
    USETDW(sc->cbw.dCBWTag, UGETDW(sc->cbw.dCBWTag) + 1);
    USETDW(sc->cbw.dCBWDataTransferLength, datalen);
    /* DIR_NONE is treated as DIR_OUT (0x00) */
    sc->cbw.bCBWFlags = (dir == DIR_IN? CBWFLAGS_IN:CBWFLAGS_OUT);
    sc->cbw.bCBWLUN = (uByte)lun;
    sc->cbw.bCDBLength = (uByte)cmdlen;
    j_memcpy(sc->cbw.CBWCDB, cmd, cmdlen);

    DBG_RUN(DMASS_UMASS, DL_EX_VERBOSE, umass_bbb_dump_cbw(sc, &sc->cbw));

    /* store the details for the data transfer phase */
    sc->transfer_dir = dir;
    sc->transfer_data = data;
    sc->transfer_datalen = datalen;
    sc->transfer_flags = flags;
    sc->transfer_actlen = 0;
    sc->transfer_cb = cb;
    sc->transfer_priv = priv;
    sc->transfer_status = STATUS_CMD_OK;

    /* move from idle to the command state */
    sc->transfer_state = TSTATE_BBB_COMMAND;

    /* Send the CBW from host to device via bulk-out endpoint. */
    if (umass_setup_transfer(sc, sc->bulkout_pipe, &sc->cbw, 
        UMASS_BBB_CBW_SIZE, 0, sc->transfer_xfer[XFER_BBB_CBW])) 
    {
        return umass_bbb_reset(sc, STATUS_WIRE_FAILED);
    }
    return STATUS_CMD_OK;
}


static void umass_bbb_state(usbd_xfer_handle xfer, usbd_private_handle priv,
    usbd_status err)
{
    struct umass_softc *sc = (struct umass_softc *) priv;
    usbd_xfer_handle next_xfer;
    jint32_t Residue;
    jint_t trans_flags = 0;

    KASSERT(sc, ("UMASS: Invalid handle\n"));
    KASSERT(sc->proto & UMASS_PROTO_BBB,
        ("%s: umass_bbb_state: wrong sc->proto 0x%02x\n",
        j_device_get_nameunit(sc->sc_dev), sc->proto));

    /*
     * State handling for BBB transfers.
     *
     * The subroutine is rather long. It steps through the states given in
     * Annex A of the Bulk-Only specification.
     * Each state first does the error handling of the previous transfer
     * and then prepares the next transfer.
     * Each transfer is done asynchronously so after the request/transfer
     * has been submitted you will find a 'return;'.
     */

    DBG_V(DMASS_UMASS, ("%s: Handling BBB state %d (%s), xfer=%p, %s\n",
        j_device_get_nameunit(sc->sc_dev), sc->transfer_state,
        states[sc->transfer_state], xfer, usbd_errstr(err)));

    switch (sc->transfer_state) 
    {
        /***** Bulk Transfer *****/
    case TSTATE_BBB_COMMAND:
        /* Command transport phase, error handling */
        if (err) 
        {
            DBG_W(DMASS_UMASS, ("%s: failed to send CBW\n",
                j_device_get_nameunit(sc->sc_dev)));
            /* If the device detects that the CBW is invalid, then
             * the device may STALL both bulk endpoints and require
             * a Bulk-Reset
             */
            if (umass_bbb_reset(sc, STATUS_WIRE_FAILED))
                goto Error;

            return;
        }

        /* Prevent data copy for dma'able buffers */
        if (sc->transfer_flags & BUFFER_IS_DMA_ABLE)
            trans_flags = USBD_DMA_BUF;

        /* Data transport phase, setup transfer */
        sc->transfer_state = TSTATE_BBB_DATA;
        if (sc->transfer_dir == DIR_IN) 
        {
            if (umass_setup_transfer(sc, sc->bulkin_pipe,
                sc->transfer_data, sc->transfer_datalen,
                USBD_SHORT_XFER_OK | trans_flags,
                sc->transfer_xfer[XFER_BBB_DATA]))
            {
                if (umass_bbb_reset(sc, STATUS_WIRE_FAILED))
                    goto Error;
            }
            return;
        } 
        else if (sc->transfer_dir == DIR_OUT) 
        {
            if (umass_setup_transfer(sc, sc->bulkout_pipe,
                sc->transfer_data, sc->transfer_datalen,
                trans_flags,    /* fixed length transfer */
                sc->transfer_xfer[XFER_BBB_DATA]))
            {
                if (umass_bbb_reset(sc, STATUS_WIRE_FAILED))
                    goto Error;
            }
            return;
        } 
        else
        {
            DBG_V(DMASS_UMASS, ("%s: no data phase\n",
                j_device_get_nameunit(sc->sc_dev)));
        }

        /* FALLTHROUGH if no data phase, err == 0 */
    case TSTATE_BBB_DATA:
        /* Command transport phase, error handling (ignored if no data
         * phase (fallthrough from previous state)) */
        if (sc->transfer_dir != DIR_NONE) 
        {
            /* retrieve the length of the transfer that was done */
            usbd_get_xfer_status(xfer, NULL, NULL,
                (juint32_t *)&sc->transfer_actlen, NULL);

            if (err) 
            {
                DBG_W(DMASS_UMASS, ("%s: Data-%s %ldb failed, "
                    "%s\n", j_device_get_nameunit(sc->sc_dev),
                    (sc->transfer_dir == DIR_IN?"in":"out"),
                    sc->transfer_datalen,usbd_errstr(err)));

                if (err == USBD_STALLED) 
                {
                    if (umass_clear_endpoint_stall(sc,
                        (sc->transfer_dir == DIR_IN?
                        sc->bulkin:sc->bulkout),
                        (sc->transfer_dir == DIR_IN?
                        sc->bulkin_pipe:sc->bulkout_pipe),
                        TSTATE_BBB_DCLEAR,
                        sc->transfer_xfer[XFER_BBB_DCLEAR]))
                    {
                        goto Error;
                    }

                    return;
                } 
                else
                {
                    /* Unless the error is a pipe stall the
                     * error is fatal.
                     */
                    if (umass_bbb_reset(sc,STATUS_WIRE_FAILED))
                        goto Error;

                    return;
                }
            }
        }

        DBG_RUN(DMASS_UMASS, DL_EX_VERBOSE, if (sc->transfer_dir == DIR_IN)
            umass_dump_buffer(sc, sc->transfer_data,
            sc->transfer_datalen, 48));

        /* FALLTHROUGH, err == 0 (no data phase or successfull) */
    case TSTATE_BBB_DCLEAR:     /* stall clear after data phase */
    case TSTATE_BBB_SCLEAR:     /* stall clear after status phase */
        /* Reading of CSW after bulk stall condition in data phase
         * (TSTATE_BBB_DATA2) or bulk-in stall condition after
         * reading CSW (TSTATE_BBB_SCLEAR).
         * In the case of no data phase or successfull data phase,
         * err == 0 and the following if block is passed.
         */
        if (err) 
        {       /* should not occur */
            /* try the transfer below, even if clear stall failed */
            DBG_W(DMASS_UMASS, ("%s: bulk-%s stall clear failed"
                ", %s\n", j_device_get_nameunit(sc->sc_dev),
                (sc->transfer_dir == DIR_IN? "in":"out"), usbd_errstr(err)));
            if (umass_bbb_reset(sc, STATUS_WIRE_FAILED))
                goto Error;

            return;
        }

        /* Status transport phase, setup transfer */
        if (sc->transfer_state == TSTATE_BBB_COMMAND ||
            sc->transfer_state == TSTATE_BBB_DATA ||
            sc->transfer_state == TSTATE_BBB_DCLEAR) 
        {
            /* After no data phase, successfull data phase and
             * after clearing bulk-in/-out stall condition
             */
            sc->transfer_state = TSTATE_BBB_STATUS1;
            next_xfer = sc->transfer_xfer[XFER_BBB_CSW1];
        }
        else
        {
            /* After first attempt of fetching CSW */
            sc->transfer_state = TSTATE_BBB_STATUS2;
            next_xfer = sc->transfer_xfer[XFER_BBB_CSW2];
        }

        /* Read the Command Status Wrapper via bulk-in endpoint. */
        if (umass_setup_transfer(sc, sc->bulkin_pipe,
            &sc->csw, UMASS_BBB_CSW_SIZE, 0,
            next_xfer)) 
        {
            if (umass_bbb_reset(sc, STATUS_WIRE_FAILED))
                goto Error;

            return;
        }

        return;
    case TSTATE_BBB_STATUS1:    /* first attempt */
    case TSTATE_BBB_STATUS2:    /* second attempt */
        /* Status transfer, error handling */
        if (err) 
        {
            DBG_W(DMASS_UMASS, ("%s: Failed to read CSW, %s%s\n",
                j_device_get_nameunit(sc->sc_dev), usbd_errstr(err),
                (sc->transfer_state == TSTATE_BBB_STATUS1 ? ", retrying":"")));

            /* If this was the first attempt at fetching the CSW
             * retry it, otherwise fail.
             */
            if (sc->transfer_state == TSTATE_BBB_STATUS1)
            {
                if (umass_clear_endpoint_stall(sc, sc->bulkin, sc->bulkin_pipe,
                    TSTATE_BBB_SCLEAR, sc->transfer_xfer[XFER_BBB_SCLEAR]))
                {
                    goto Error;
                }

                return;
            }
            else
            {
                if (umass_bbb_reset(sc, STATUS_WIRE_FAILED))
                    goto Error;

                return;
            }
        }

        DBG_RUN(DMASS_UMASS, DL_EX_VERBOSE, umass_bbb_dump_csw(sc, &sc->csw));

        /* Translate weird command-status signatures. */
        if ((sc->quirks & WRONG_CSWSIG) &&
            UGETDW(sc->csw.dCSWSignature) == CSWSIGNATURE_OLYMPUS_C1)
        {
            USETDW(sc->csw.dCSWSignature, CSWSIGNATURE);
        }

        Residue = UGETDW(sc->csw.dCSWDataResidue);

        if (sc->quirks & IGNORE_RESIDUE)
            Residue = 0;
        else if (sc->quirks & SWAP_RESIDUE)
            Residue = bswap32(Residue);
        else if (Residue != sc->transfer_datalen - sc->transfer_actlen)
        {
            DBG_W(DMASS_UMASS, ("UMASS: Residue and actual transfer "
                "do not agree, residue %ld transfer %ld/%ld [%ld]\n",
                Residue, sc->transfer_actlen, sc->transfer_datalen,
                sc->transfer_datalen - sc->transfer_actlen));
        }

        if (Residue == 0 && sc->transfer_datalen - sc->transfer_actlen != 0)
            Residue = sc->transfer_datalen - sc->transfer_actlen;

        /* Check CSW and handle any error */
        if (UGETDW(sc->csw.dCSWSignature) != CSWSIGNATURE) {
            /* Invalid CSW: Wrong signature or wrong tag might
             * indicate that the device is confused -> reset it.
             */
            DBG_W(DMASS_UMASS, ("%s: Invalid CSW: sig 0x%08lx should be "
                "0x%08lx\n", j_device_get_nameunit(sc->sc_dev),
                UGETDW(sc->csw.dCSWSignature), CSWSIGNATURE));

            if (umass_bbb_reset(sc, STATUS_WIRE_FAILED))
                goto Error;

            return;
        } 
        else if (UGETDW(sc->csw.dCSWTag) != UGETDW(sc->cbw.dCBWTag)) 
        {
            DBG_W(DMASS_UMASS, ("%s: Invalid CSW: tag %ld should be %ld\n",
                j_device_get_nameunit(sc->sc_dev), UGETDW(sc->csw.dCSWTag),
                UGETDW(sc->cbw.dCBWTag)));

            if (umass_bbb_reset(sc, STATUS_WIRE_FAILED))
                goto Error;

            return;

            /* CSW is valid here */
        } 
        else if (sc->csw.bCSWStatus > CSWSTATUS_PHASE) 
        {
            DBG_W(DMASS_UMASS, ("%s: Invalid CSW: status %d > %d\n",
                j_device_get_nameunit(sc->sc_dev), sc->csw.bCSWStatus,
                CSWSTATUS_PHASE));

            if (umass_bbb_reset(sc, STATUS_WIRE_FAILED))
                goto Error;

            return;
        } 
        else if (sc->csw.bCSWStatus == CSWSTATUS_PHASE) 
        {
            DBG_W(DMASS_UMASS, ("%s: Phase Error, residue = %ld\n",
                j_device_get_nameunit(sc->sc_dev), Residue));

            if (umass_bbb_reset(sc, STATUS_WIRE_FAILED))
                goto Error;

            return;

        }
        else if (sc->transfer_actlen > (juint32_t)sc->transfer_datalen) 
        {
            /* Buffer overrun! Don't let this go by unnoticed */
            j_panic("%s: transferred %ldb instead of %ldb",
                j_device_get_nameunit(sc->sc_dev), sc->transfer_actlen, 
                sc->transfer_datalen);

        } 
        else if (sc->csw.bCSWStatus == CSWSTATUS_FAILED) 
        {
            DBG_I(DMASS_UMASS, ("%s: Command Failed, res = %ld\n",
                j_device_get_nameunit(sc->sc_dev), Residue));

            /* SCSI command failed but transfer was succesful */
            sc->transfer_state = TSTATE_IDLE;
            sc->transfer_cb(sc, sc->transfer_priv, Residue, STATUS_CMD_FAILED);
            return;

        } 
        else
        {       /* success */
            sc->transfer_state = TSTATE_IDLE;
            sc->transfer_cb(sc, sc->transfer_priv, Residue, STATUS_CMD_OK);

            return;
        }

        /***** Bulk Reset *****/
    case TSTATE_BBB_RESET1:
        if (err)
        {
            DBG_W(DMASS_UMASS, ("%s: BBB reset failed, %s\n",
                j_device_get_nameunit(sc->sc_dev), usbd_errstr(err)));
        }

        if (umass_clear_endpoint_stall(sc, sc->bulkin, sc->bulkin_pipe,
            TSTATE_BBB_RESET2,  sc->transfer_xfer[XFER_BBB_RESET2]))
        {
            goto Error;
        }


        return;
    case TSTATE_BBB_RESET2:
        if (err) /* should not occur (bull ! what if we detached ? */
        {        
            DBG_W(DMASS_UMASS, ("%s: BBB bulk-in clear stall failed, %s\n",
                j_device_get_nameunit(sc->sc_dev), usbd_errstr(err)));
        }
        /* no error recovery, otherwise we end up in a loop */

        if (umass_clear_endpoint_stall(sc, sc->bulkout, sc->bulkout_pipe, 
            TSTATE_BBB_RESET3, sc->transfer_xfer[XFER_BBB_RESET3]))
        {
            goto Error;
        }

        return;
    case TSTATE_BBB_RESET3:
        if (err)        /* should not occur */
        {
            DBG_W(DMASS_UMASS, ("%s: BBB bulk-out clear stall failed, %s\n",
                j_device_get_nameunit(sc->sc_dev), usbd_errstr(err)));
        }
        /* no error recovery, otherwise we end up in a loop */

        sc->transfer_state = TSTATE_IDLE;
        if (sc->transfer_priv) 
        {
            sc->transfer_cb(sc, sc->transfer_priv, sc->transfer_datalen,
                sc->transfer_status);
        }

        return;

    case TSTATE_IDLE:
        DBG_W(DMASS_UMASS, ("UMASS: Completion callback when in IDLE state "
            "!\n"));
        return;

        /***** Default *****/
    default:
        j_panic("%s: Unknown state %d",
            j_device_get_nameunit(sc->sc_dev), sc->transfer_state);
    }

Error:

    DBG_E(DMASS_UMASS, ("UMASS: Error during state processing\n"));

    /* If reset failed, call CB with wire fail status */
    sc->transfer_state = TSTATE_IDLE;
    sc->transfer_status = STATUS_WIRE_FAILED;
    if (sc->transfer_priv)
    {
        sc->transfer_cb(sc, sc->transfer_priv, sc->transfer_datalen,
            sc->transfer_status);
    }
}

static juint8_t umass_bbb_get_max_lun(struct umass_softc *sc, 
    usbd_interface_handle iface)
{
    usb_device_request_t req;
    usbd_status err;
    usb_interface_descriptor_t *id;
    juint8_t maxlun = 0;
    juint8_t buf = 0;

    id = usbd_get_interface_descriptor(iface);

    /* The Get Max Lun command is a class-specific request. */
    req.bmRequestType = UT_READ_CLASS_INTERFACE;
    req.bRequest = UR_BBB_GET_MAX_LUN;
    USETW(req.wValue, 0);
    USETW(req.wIndex, id->bInterfaceNumber);
    USETW(req.wLength, 1);

    err = usbd_do_request(sc->sc_udev, &req, &buf);
    switch (err) 
    {
    case USBD_NORMAL_COMPLETION:
        maxlun = buf;
        DBG_I(DMASS_UMASS, ("%s: Max Lun is %d\n",
            j_device_get_nameunit(sc->sc_dev), maxlun));
        break;
    case USBD_STALLED:
    case USBD_SHORT_XFER:
    default:
        /* Device doesn't support Get Max Lun request. */
        DBG_I(DMASS_UMASS, ("%s: Get Max Lun not supported (%s)\n",
            j_device_get_nameunit(sc->sc_dev), usbd_errstr(err)));
        break;
    }

    return(maxlun);
}

#ifdef JUSB_UMASS_USE_CBI
/*
 * Command/Bulk/Interrupt (CBI) specific functions
 */

static usbd_status umass_cbi_adsc(struct umass_softc *sc, char *buffer,
    jint_t buflen, usbd_xfer_handle xfer)
{
    KASSERT(sc->proto & (UMASS_PROTO_CBI|UMASS_PROTO_CBI_I),
        ("%s: umass_cbi_adsc: wrong sc->proto 0x%02x\n",
        j_device_get_nameunit(sc->sc_dev), sc->proto));

    sc->request.bmRequestType = UT_WRITE_CLASS_INTERFACE;
    sc->request.bRequest = UR_CBI_ADSC;
    USETW(sc->request.wValue, 0);
    USETW(sc->request.wIndex, sc->ifaceno);
    USETW(sc->request.wLength, buflen);
    return umass_setup_ctrl_transfer(sc, &sc->request, buffer, 
        buflen, 0, xfer);
}


static usbd_status umass_cbi_reset(struct umass_softc *sc, jint_t status)
{
    jint_t i;
#       define SEND_DIAGNOSTIC_CMDLEN   12

    KASSERT(sc->proto & (UMASS_PROTO_CBI|UMASS_PROTO_CBI_I),
        ("%s: umass_cbi_reset: wrong sc->proto 0x%02x\n",
        j_device_get_nameunit(sc->sc_dev), sc->proto));

    /*
     * Command Block Reset Protocol
     *
     * First send a reset request to the device. Then clear
     * any possibly stalled bulk endpoints.
     *
     * This is done in 3 steps, states:
     * TSTATE_CBI_RESET1
     * TSTATE_CBI_RESET2
     * TSTATE_CBI_RESET3
     *
     * If the reset doesn't succeed, the device should be port reset.
     */

    DBG_V(DMASS_UMASS, ("%s: CBI Reset\n",
        j_device_get_nameunit(sc->sc_dev)));

    KASSERT(sizeof(sc->cbl) >= SEND_DIAGNOSTIC_CMDLEN,
        ("%s: CBL struct is too small (%ld < %d)\n",
        j_device_get_nameunit(sc->sc_dev),
        (long)sizeof(sc->cbl), SEND_DIAGNOSTIC_CMDLEN));

    sc->transfer_state = TSTATE_CBI_RESET1;
    sc->transfer_status = status;

    /* The 0x1d code is the SEND DIAGNOSTIC command. To distinguish between
     * the two the last 10 bytes of the cbl is filled with 0xff (section
     * 2.2 of the CBI spec).
     */
    sc->cbl[0] = 0x1d;  /* Command Block Reset */
    sc->cbl[1] = 0x04;
    for (i = 2; i < SEND_DIAGNOSTIC_CMDLEN; i++)
        sc->cbl[i] = 0xff;

    return umass_cbi_adsc(sc, sc->cbl, SEND_DIAGNOSTIC_CMDLEN,
        sc->transfer_xfer[XFER_CBI_RESET1]);
}

static jint_t umass_cbi_transfer(struct umass_softc *sc, jint_t lun, void *cmd,
    jint_t cmdlen, void *data, juint32_t datalen, jint_t flags, jint_t dir, 
    juint32_t timeout, transfer_cb_f cb, void *priv)
{
    KASSERT(sc->proto & (UMASS_PROTO_CBI|UMASS_PROTO_CBI_I),
        ("%s: umass_cbi_transfer: wrong sc->proto 0x%02x\n",
        j_device_get_nameunit(sc->sc_dev), sc->proto));

    /* Be a little generous. */
    sc->timeout = timeout + UMASS_TIMEOUT;

    /*
     * Do a CBI transfer with cmdlen bytes from cmd, possibly
     * a data phase of datalen bytes from/to the device and finally a
     * csw read phase.
     * If the data direction was inbound a maximum of datalen bytes
     * is stored in the buffer pointed to by data.
     *
     * umass_cbi_transfer initialises the transfer and lets the state
     * machine in umass_cbi_state handle the completion. It uses the
     * following states:
     * TSTATE_CBI_COMMAND
     *   -> ... see above
     *
     * An error in any of those states will invoke
     * umass_cbi_reset.
     */

    /* check the given arguments */
    KASSERT(datalen == 0 || data != NULL,
        ("%s: datalen > 0, but no buffer",j_device_get_nameunit(sc->sc_dev)));
    KASSERT(datalen == 0 || dir != DIR_NONE,
        ("%s: direction is NONE while datalen is not zero\n",
        j_device_get_nameunit(sc->sc_dev)));

    /* store the details for the data transfer phase */
    sc->transfer_dir = dir;
    sc->transfer_data = data;
    sc->transfer_datalen = datalen;
    sc->transfer_flags = flags;
    sc->transfer_actlen = 0;
    sc->transfer_cb = cb;
    sc->transfer_priv = priv;
    sc->transfer_status = STATUS_CMD_OK;

    /* move from idle to the command state */
    sc->transfer_state = TSTATE_CBI_COMMAND;

    DBG_RUN(DMASS_UMASS, DL_EX_VERBOSE, umass_cbi_dump_cmd(sc, cmd, cmdlen));

    /* Send the Command Block from host to device via control endpoint. */
    if (umass_cbi_adsc(sc, cmd, cmdlen, sc->transfer_xfer[XFER_CBI_CB]))
        return umass_cbi_reset(sc, STATUS_WIRE_FAILED);

    return STATUS_CMD_OK;
}

static void umass_cbi_state(usbd_xfer_handle xfer, usbd_private_handle priv,
    usbd_status err)
{
    struct umass_softc *sc = (struct umass_softc *) priv;
    jint_t trans_flags = 0;

    KASSERT(sc->proto & (UMASS_PROTO_CBI|UMASS_PROTO_CBI_I),
        ("%s: umass_cbi_state: wrong sc->proto 0x%02x\n",
        j_device_get_nameunit(sc->sc_dev), sc->proto));

    /*
     * State handling for CBI transfers.
     */

    DBG_V(DMASS_UMASS, ("%s: Handling CBI state %d (%s), xfer=%p, %s\n",
        j_device_get_nameunit(sc->sc_dev), sc->transfer_state,
        states[sc->transfer_state], xfer, usbd_errstr(err)));

    switch (sc->transfer_state) 
    {
        /***** CBI Transfer *****/
    case TSTATE_CBI_COMMAND:
        if (err == USBD_STALLED) 
        {
            DBG_W(DMASS_UMASS, ("%s: Command Transport failed\n",
                j_device_get_nameunit(sc->sc_dev)));
            /* Status transport by control pipe (section 2.3.2.1).
             * The command contained in the command block failed.
             *
             * The control pipe has already been unstalled by the
             * USB stack.
             * Section 2.4.3.1.1 states that the bulk in endpoints
             * should not be stalled at this point.
             */

            sc->transfer_state = TSTATE_IDLE;
            sc->transfer_cb(sc, sc->transfer_priv,
                sc->transfer_datalen,
                STATUS_CMD_FAILED);

            return;
        }
        else if (err) 
        {
            DBG_W(DMASS_UMASS, ("%s: failed to send ADSC\n",
                j_device_get_nameunit(sc->sc_dev)));
            if (umass_cbi_reset(sc, STATUS_WIRE_FAILED))
                goto Error;

            return;
        }

        /* Prevent data copy for dma'able buffers */
        if (sc->transfer_flags & BUFFER_IS_DMA_ABLE)
            trans_flags = USBD_DMA_BUF;

        sc->transfer_state = TSTATE_CBI_DATA;
        if (sc->transfer_dir == DIR_IN) 
        {
            if (umass_setup_transfer(sc, sc->bulkin_pipe, sc->transfer_data,
                sc->transfer_datalen, trans_flags | USBD_SHORT_XFER_OK,
                sc->transfer_xfer[XFER_CBI_DATA]))
            {
                if (umass_cbi_reset(sc, STATUS_WIRE_FAILED))
                    goto Error;
            }
        } 
        else if (sc->transfer_dir == DIR_OUT) 
        {
            if (umass_setup_transfer(sc, sc->bulkout_pipe, sc->transfer_data,
                sc->transfer_datalen, trans_flags, /* fixed length transfer */
                sc->transfer_xfer[XFER_CBI_DATA]))
            {
                if (umass_cbi_reset(sc, STATUS_WIRE_FAILED))
                    goto Error;
            }
        }
        else if (sc->proto & UMASS_PROTO_CBI_I) 
        {
            DBG_V(DMASS_UMASS, ("%s: no data phase\n",
                j_device_get_nameunit(sc->sc_dev)));
            sc->transfer_state = TSTATE_CBI_STATUS;
            if (umass_setup_transfer(sc, sc->intrin_pipe, &sc->sbl, 
                sizeof(sc->sbl), 0,     /* fixed length transfer */
                sc->transfer_xfer[XFER_CBI_STATUS]))
            {
                if (umass_cbi_reset(sc, STATUS_WIRE_FAILED))
                    goto Error;
            }
        } 
        else
        {
            DBG_V(DMASS_UMASS, ("%s: no data phase\n",
                j_device_get_nameunit(sc->sc_dev)));
            /* No command completion interrupt. Request
             * sense data.
             */
            sc->transfer_state = TSTATE_IDLE;
            sc->transfer_cb(sc, sc->transfer_priv, 0, STATUS_CMD_UNKNOWN);
        }

        return;

    case TSTATE_CBI_DATA:
        /* retrieve the length of the transfer that was done */
        usbd_get_xfer_status(xfer,NULL,NULL,&sc->transfer_actlen,NULL);

        if (err) 
        {
            DBG_I(DMASS_UMASS, ("%s: Data-%s %ldb failed, "
                "%s\n", j_device_get_nameunit(sc->sc_dev),
                (sc->transfer_dir == DIR_IN?"in":"out"),
                sc->transfer_datalen,usbd_errstr(err)));

            if (err == USBD_STALLED) 
            {
                umass_clear_endpoint_stall(sc, sc->bulkin, sc->bulkin_pipe,
                    TSTATE_CBI_DCLEAR, sc->transfer_xfer[XFER_CBI_DCLEAR]);
            }
            else
            {
                if (umass_cbi_reset(sc, STATUS_WIRE_FAILED))
                    goto Error;
            }
            return;
        }

        DBG_RUN(DMASS_UMASS, DL_EX_VERBOSE, if (sc->transfer_dir == DIR_IN)
            umass_dump_buffer(sc, sc->transfer_data, sc->transfer_actlen, 48));

        if (sc->proto & UMASS_PROTO_CBI_I) 
        {
            sc->transfer_state = TSTATE_CBI_STATUS;
            if (umass_setup_transfer(sc, sc->intrin_pipe, &sc->sbl, 
                sizeof(sc->sbl), 0,     /* fixed length transfer */
                sc->transfer_xfer[XFER_CBI_STATUS]))
            {
                if (umass_cbi_reset(sc, STATUS_WIRE_FAILED))
                    goto Error;
            }
        } 
        else
        {
            /* No command completion interrupt. Request
             * sense to get status of command.
             */
            sc->transfer_state = TSTATE_IDLE;
            sc->transfer_cb(sc, sc->transfer_priv, 
                sc->transfer_datalen - sc->transfer_actlen, STATUS_CMD_UNKNOWN);
        }
        return;

    case TSTATE_CBI_STATUS:
        if (err) 
        {
            DBG_W(DMASS_UMASS, ("%s: Status Transport failed\n",
                j_device_get_nameunit(sc->sc_dev)));
            /* Status transport by interrupt pipe (section 2.3.2.2).
            */

            if (err == USBD_STALLED) 
            {
                umass_clear_endpoint_stall(sc, sc->intrin, sc->intrin_pipe,
                    TSTATE_CBI_SCLEAR, sc->transfer_xfer[XFER_CBI_SCLEAR]);
            } 
            else
            {
                if (umass_cbi_reset(sc, STATUS_WIRE_FAILED))
                    goto Error;
            }
            return;
        }

        /* Dissect the information in the buffer */

        if (sc->proto & UMASS_PROTO_UFI) 
        {
            jint_t status;

            /* Section 3.4.3.1.3 specifies that the UFI command
             * protocol returns an ASC and ASCQ in the interrupt
             * data block.
             */

            DBG_V(DMASS_UMASS, ("%s: UFI CCI, ASC = 0x%02x, ASCQ = 0x%02x\n",
                j_device_get_nameunit(sc->sc_dev), sc->sbl.ufi.asc,
                sc->sbl.ufi.ascq));

            if (sc->sbl.ufi.asc == 0 && sc->sbl.ufi.ascq == 0)
                status = STATUS_CMD_OK;
            else
                status = STATUS_CMD_FAILED;

            sc->transfer_state = TSTATE_IDLE;
            sc->transfer_cb(sc, sc->transfer_priv, 
                sc->transfer_datalen - sc->transfer_actlen, status);
        } 
        else
        {
            /* Command Interrupt Data Block */
            DBG_V(DMASS_UMASS, ("%s: type=0x%02x, value=0x%02x\n",
                j_device_get_nameunit(sc->sc_dev), sc->sbl.common.type,
                sc->sbl.common.value));

            if (sc->sbl.common.type == IDB_TYPE_CCI) 
            {
                jint_t status;

                if ((sc->sbl.common.value&IDB_VALUE_STATUS_MASK) == 
                    IDB_VALUE_PASS) 
                {
                    status = STATUS_CMD_OK;
                }
                else if ((sc->sbl.common.value & IDB_VALUE_STATUS_MASK) == 
                    IDB_VALUE_FAIL || 
                    (sc->sbl.common.value & IDB_VALUE_STATUS_MASK) ==
                    IDB_VALUE_PERSISTENT) 
                {
                    status = STATUS_CMD_FAILED;
                }
                else
                {
                    status = STATUS_WIRE_FAILED;
                }

                sc->transfer_state = TSTATE_IDLE;
                sc->transfer_cb(sc, sc->transfer_priv, 
                    sc->transfer_datalen-sc->transfer_actlen, status);
            }
        }
        return;

    case TSTATE_CBI_DCLEAR:
        if (err)
        {       /* should not occur */
            jprintf("%s: CBI bulk-in/out stall clear failed, %s\n",
                j_device_get_nameunit(sc->sc_dev), usbd_errstr(err));
            if (umass_cbi_reset(sc, STATUS_WIRE_FAILED))
                goto Error;
        }

        sc->transfer_state = TSTATE_IDLE;
        sc->transfer_cb(sc, sc->transfer_priv, sc->transfer_datalen,
            STATUS_CMD_FAILED);
        return;

    case TSTATE_CBI_SCLEAR:
        if (err)
        {   /* should not occur */
            jprintf("%s: CBI intr-in stall clear failed, %s\n",
                j_device_get_nameunit(sc->sc_dev), usbd_errstr(err));
        }

        /* Something really bad is going on. Reset the device */
        if (umass_cbi_reset(sc, STATUS_CMD_FAILED))
            goto Error;
        return;

        /***** CBI Reset *****/
    case TSTATE_CBI_RESET1:
        if (err)
        {
            jprintf("%s: CBI reset failed, %s\n",
                j_device_get_nameunit(sc->sc_dev), usbd_errstr(err));
        }

        umass_clear_endpoint_stall(sc, sc->bulkin, sc->bulkin_pipe,
            TSTATE_CBI_RESET2, sc->transfer_xfer[XFER_CBI_RESET2]);

        return;
    case TSTATE_CBI_RESET2:
        if (err)        /* should not occur */
        {
            jprintf("%s: CBI bulk-in stall clear failed, %s\n",
                j_device_get_nameunit(sc->sc_dev), usbd_errstr(err));
        }
        /* no error recovery, otherwise we end up in a loop */

        umass_clear_endpoint_stall(sc, sc->bulkout, sc->bulkout_pipe,
            TSTATE_CBI_RESET3, sc->transfer_xfer[XFER_CBI_RESET3]);

        return;
    case TSTATE_CBI_RESET3:
        if (err)        /* should not occur */
        {
            jprintf("%s: CBI bulk-out stall clear failed, %s\n",
                j_device_get_nameunit(sc->sc_dev), usbd_errstr(err));
        }
        /* no error recovery, otherwise we end up in a loop */

        sc->transfer_state = TSTATE_IDLE;
        if (sc->transfer_priv) 
        {
            sc->transfer_cb(sc, sc->transfer_priv, sc->transfer_datalen,
                sc->transfer_status);
        }

        return;


        /***** Default *****/
    default:
        j_panic("%s: Unknown state %d",
            j_device_get_nameunit(sc->sc_dev), sc->transfer_state);
    }

Error:
    DBG_E(DMASS_UMASS, ("UMASS: Error during state processing\n"));

    /* If reset failed, call CB with wire fail status */
    sc->transfer_state = TSTATE_IDLE;
    sc->transfer_status = STATUS_WIRE_FAILED;
    if (sc->transfer_priv) 
    {
        sc->transfer_cb(sc, sc->transfer_priv, sc->transfer_datalen,
            sc->transfer_status);
    }
}
#endif

static jint_t umass_detach_up(struct umass_softc *sc)
{
    if (sc->umass_sim) 
    {
        jms_transport_detach((void *)sc->umass_sim);
        sc->umass_sim = NULL;
    }

    return(0);
}

/*
 * SCSI specific functions
 */

static jint_t umass_scsi_transform(struct umass_softc *sc, unsigned char *cmd, 
    jint_t cmdlen, unsigned char **rcmd, jint_t *rcmdlen)
{
    switch (cmd[0]) 
    {
    case TEST_UNIT_READY:
        if (sc->quirks & NO_TEST_UNIT_READY) 
        {
            KASSERT(*rcmdlen >= START_STOP_UNIT_SIZE,
                ("rcmdlen = %d < %ld, buffer too small", *rcmdlen,
                (long)START_STOP_UNIT_SIZE));
            DBG_X(DMASS_UMASS, ("%s: Converted TEST_UNIT_READY "
                "to START_UNIT\n", j_device_get_nameunit(sc->sc_dev)));
            j_memset(*rcmd, 0, *rcmdlen);
            (*rcmd)[0] = START_STOP_UNIT;
            (*rcmd)[4] = SSS_START;
            return 1;
        }
        /* fallthrough */
    case INQUIRY:
        /* some drives wedge when asked for full inquiry information. */
        if (sc->quirks & FORCE_SHORT_INQUIRY) 
        {
            j_memcpy(*rcmd, cmd, cmdlen);
            *rcmdlen = cmdlen;
            (*rcmd)[4] = SHORT_INQUIRY_LENGTH;
            return 1;
        }
        /* fallthrough */
    default:
        *rcmd = cmd;            /* We don't need to copy it */
        *rcmdlen = cmdlen;
    }

    return 1;
}

/* RBC specific functions */
static jint_t umass_rbc_transform(struct umass_softc *sc, unsigned char *cmd,
    jint_t cmdlen, unsigned char **rcmd, jint_t *rcmdlen)
{
    switch (cmd[0])
    {
        /* these commands are defined in RBC: */
    case READ_10:
    case READ_CAPACITY:
    case START_STOP_UNIT:
    case SYNCHRONIZE_CACHE:
    case WRITE_10:
    case VERIFY_10:
    case INQUIRY:
    case MODE_SELECT_10:
    case MODE_SENSE_10:
    case TEST_UNIT_READY:
    case WRITE_BUFFER:
        /* The following commands are not listed in my copy of the RBC specs.
         * CAM however seems to want those, and at least the Sony DSC device
         * appears to support those as well */
    case REQUEST_SENSE:
    case PREVENT_ALLOW:
        *rcmd = cmd;            /* We don't need to copy it */
        *rcmdlen = cmdlen;
        return 1;

        /* All other commands are not legal in RBC 
         * but we try anyway since some devices don't follow spec */
    default:
        DBG_W(DMASS_UMASS, ("%s: Using unsupported RBC command 0x%02x",
            j_device_get_nameunit(sc->sc_dev), cmd[0]));
        *rcmd = cmd;
        *rcmdlen = cmdlen;
        return 1;
    }
}

/*
 * UFI specific functions
 */
static jint_t umass_ufi_transform(struct umass_softc *sc, unsigned char *cmd,
    jint_t cmdlen, unsigned char **rcmd, jint_t *rcmdlen)
{
    /* A UFI command is always 12 bytes in length */
    KASSERT(*rcmdlen >= UFI_COMMAND_LENGTH, 
        ("rcmdlen = %d < %d, buffer too small", *rcmdlen, 
        UFI_COMMAND_LENGTH));

    *rcmdlen = UFI_COMMAND_LENGTH;
    j_memset(*rcmd, 0, UFI_COMMAND_LENGTH);

    switch (cmd[0]) 
    {
        /* Commands of which the format has been verified. They should work.
         * Copy the command into the (zeroed out) destination buffer.
         */
    case TEST_UNIT_READY:
        if (sc->quirks &  NO_TEST_UNIT_READY) 
        {
            /* Some devices do not support this command.
             * Start Stop Unit should give the same results
             */
            DBG_X(DMASS_UMASS, ("%s: Converted TEST_UNIT_READY "
                "to START_UNIT\n", j_device_get_nameunit(sc->sc_dev)));
            (*rcmd)[0] = START_STOP_UNIT;
            (*rcmd)[4] = SSS_START;
        }
        else
        {
            j_memcpy(*rcmd, cmd, cmdlen);
        }
        return 1;

    case REZERO_UNIT:
    case REQUEST_SENSE:
    case INQUIRY:
    case START_STOP_UNIT:
    case SEND_DIAGNOSTIC:
    case PREVENT_ALLOW:
    case READ_CAPACITY:
    case READ_10:
    case WRITE_10:
    case POSITION_TO_ELEMENT:   /* SEEK_10 */
    case MODE_SELECT_10:
    case MODE_SENSE_10:
    case READ_12:
    case WRITE_12:
        j_memcpy(*rcmd, cmd, cmdlen);
        return 1;

        /* Other UFI commands: FORMAT_UNIT, READ_FORMAT_CAPACITY,
         * VERIFY, WRITE_AND_VERIFY.
         * These should be checked whether they somehow can be made to fit.
         */

        /* We allow all commands, since some devices don't follow specs well */
    default:
        DBG_W(DMASS_UMASS, ("%s: Using unsupported UFI command 0x%02x\n",
            j_device_get_nameunit(sc->sc_dev), cmd[0]));
        j_memcpy(*rcmd, cmd, cmdlen);
        return 1;
    }
}

/*
 * 8070i (ATAPI) specific functions
 */
static jint_t umass_atapi_transform(struct umass_softc *sc, unsigned char *cmd,
    jint_t cmdlen, unsigned char **rcmd, jint_t *rcmdlen)
{
    /* An ATAPI command is always 12 bytes in length. */
    KASSERT(*rcmdlen >= ATAPI_COMMAND_LENGTH,
        ("rcmdlen = %d < %d, buffer too small", *rcmdlen,
        ATAPI_COMMAND_LENGTH));

    *rcmdlen = ATAPI_COMMAND_LENGTH;
    j_memset(*rcmd, 0, ATAPI_COMMAND_LENGTH);

    switch (cmd[0]) 
    {
        /* Commands of which the format has been verified. They should work.
         * Copy the command into the (zeroed out) destination buffer.
         */
    case INQUIRY:
        j_memcpy(*rcmd, cmd, cmdlen);
        /* some drives wedge when asked for full inquiry information. */
        if (sc->quirks & FORCE_SHORT_INQUIRY)
            (*rcmd)[4] = SHORT_INQUIRY_LENGTH;
        return 1;

    case TEST_UNIT_READY:
        if (sc->quirks & NO_TEST_UNIT_READY) 
        {
            KASSERT(*rcmdlen >= START_STOP_UNIT_SIZE,
                ("rcmdlen = %d < %ld, buffer too small", *rcmdlen,
                (long)START_STOP_UNIT_SIZE));
            DBG_V(DMASS_UMASS, ("%s: Converted TEST_UNIT_READY "
                "to START_UNIT\n", j_device_get_nameunit(sc->sc_dev)));
            j_memset(*rcmd, 0, *rcmdlen);
            (*rcmd)[0] = START_STOP_UNIT;
            (*rcmd)[4] = SSS_START;
            return 1;
        }
        /* fallthrough */
    case REZERO_UNIT:
    case REQUEST_SENSE:
    case START_STOP_UNIT:
    case SEND_DIAGNOSTIC:
    case PREVENT_ALLOW:
    case READ_CAPACITY:
    case READ_10:
    case WRITE_10:
    case POSITION_TO_ELEMENT:   /* SEEK_10 */
    case SYNCHRONIZE_CACHE:
    case MODE_SELECT_10:
    case MODE_SENSE_10:
    case READ_BUFFER:
    case 0x42: /* READ_SUBCHANNEL */
    case 0x43: /* READ_TOC */
    case 0x44: /* READ_HEADER */
    case 0x51: /* READ_DISK_INFO */
    case 0x52: /* READ_TRACK_INFO */
    case 0x54: /* SEND_OPC */
    case 0x59: /* READ_MASTER_CUE */
    case 0x5b: /* CLOSE_TR_SESSION */
    case 0x5c: /* READ_BUFFER_CAP */
    case 0x5d: /* SEND_CUE_SHEET */
    case 0xa1: /* BLANK */
    case 0xa6: /* EXCHANGE_MEDIUM */
    case 0xad: /* READ_DVD_STRUCTURE */
    case 0xbb: /* SET_CD_SPEED */
    case 0xe5: /* READ_TRACK_INFO_PHILIPS */
        j_memcpy(*rcmd, cmd, cmdlen);
        return 1;

    case READ_12:
    case WRITE_12:

        /* We allow all commands since some devices don't follow spec */
    default:
        DBG_W(DMASS_UMASS, ("%s: Using unsupported ATAPI command 0x%02x\n",
            j_device_get_nameunit(sc->sc_dev), cmd[0]));
        j_memcpy(*rcmd, cmd, cmdlen);
        return 1;
    }
}

#ifdef JDEBUG
static void umass_bbb_dump_cbw(struct umass_softc *sc, umass_bbb_cbw_t *cbw)
{
    jint_t clen = cbw->bCDBLength;
    juint32_t dlen = UGETDW(cbw->dCBWDataTransferLength);
    juint8_t *c = cbw->CBWCDB;
    juint32_t tag = UGETDW(cbw->dCBWTag);
    jint_t flags = cbw->bCBWFlags;

    DBG_E(DMASS_UMASS, ("%s: CBW %ld: cmd = %db (0x%02x%02x%02x%02x%02x%02x%s) "
        ",data = %ldb, dir = %s\n",
        j_device_get_nameunit(sc->sc_dev), tag, clen,
        c[0], c[1], c[2], c[3], c[4], c[5], (clen > 6? "...":""),
        dlen, (flags == CBWFLAGS_IN? "in":
        (flags == CBWFLAGS_OUT? "out":"<invalid>"))));
}

static void umass_bbb_dump_csw(struct umass_softc *sc, umass_bbb_csw_t *csw)
{
    juint32_t sig = UGETDW(csw->dCSWSignature);
    juint32_t tag = UGETW(csw->dCSWTag);
    juint32_t res = UGETDW(csw->dCSWDataResidue);
    jint_t status = csw->bCSWStatus;

    DBG_E(DMASS_UMASS, ("%s: CSW %ld: sig = 0x%08lx (%s), tag = %ld, "
        "res = %ld, status = 0x%02x (%s)\n", j_device_get_nameunit(sc->sc_dev),
        tag, sig, (sig == CSWSIGNATURE?  "valid":"invalid"),
        tag, res, 
        status, (status == CSWSTATUS_GOOD? "good":
        (status == CSWSTATUS_FAILED? "failed":
        (status == CSWSTATUS_PHASE? "phase":"<invalid>")))));
}

#ifdef JUSB_UMASS_USE_CBI
static void umass_cbi_dump_cmd(struct umass_softc *sc, void *cmd, jint_t cmdlen)
{
    juint8_t *c = cmd;
    jint_t dir = sc->transfer_dir;

    DBG_E(DMASS_UMASS, ("%s: cmd = %db "
        "(0x%02x%02x%02x%02x%02x%02x%s), "
        "data = %ldb, dir = %s\n",
        j_device_get_nameunit(sc->sc_dev), cmdlen,
        c[0], c[1], c[2], c[3], c[4], c[5], (cmdlen > 6? "...":""),
        sc->transfer_datalen,
        (dir == DIR_IN? "in":
        (dir == DIR_OUT? "out":
        (dir == DIR_NONE? "no data phase": "<invalid>")))));
}
#endif

static void umass_dump_buffer(struct umass_softc *sc, juint8_t *buffer, 
    juint32_t buflen, juint32_t printlen)
{
    juint32_t i, j;
    char s1[40];
    char s2[40];
    char s3[5];

    s1[0] = '\0';
    s3[0] = '\0';

    j_snprintf(s2, sizeof(s2), " buffer=%p, buflen=%ld", buffer, buflen);
    for (i = 0; i < buflen && i < printlen; i++) 
    {
        j = i % 16;
        if (j == 0 && i != 0) 
        {
            DBG_E(DMASS_UMASS, ("%s: 0x %s%s\n",
                j_device_get_nameunit(sc->sc_dev), s1, s2));
            s2[0] = '\0';
        }
        j_snprintf(&s1[j*2], sizeof(s1) - j*2 - 1, "%02x", buffer[i] & 0xff);
    }
    if (buflen > printlen)
        j_snprintf(s3, sizeof(s3), " ...");
    DBG_E(DMASS_UMASS, ("%s: 0x %s%s%s\n",
        j_device_get_nameunit(sc->sc_dev), s1, s2, s3));
}
#endif

static void umass_complete_cb(struct umass_softc *sc, void *priv, 
    juint32_t residue, jint_t status)
{
    jms_transport_complete(priv, residue, (juint16_t)status);
}

jint_t umass_transfer(void *dev_h, jint_t lun, unsigned char *cmd, jint_t cmdlen,
    jint_t is_read, juint32_t timeout, void *data, juint32_t data_len,
    juint8_t flags, juint32_t *actual_data_len, void *tc)
{
    struct umass_softc *sc = (struct umass_softc *)dev_h;
    unsigned char *rcmd;
    jint_t rcmdlen;
    jint_t direction;
    jint_t rc;

    if (sc->flags & UMASS_FLAGS_GONE)
        return STATUS_WIRE_FAILED;

    rcmd = (unsigned char *)(&sc->cam_scsi_command);
    rcmdlen = sizeof(sc->cam_scsi_command);

    if (!sc->transform(sc, cmd, cmdlen, &rcmd, &rcmdlen)) 
        return STATUS_CMD_UNKNOWN;

    if (data_len)
        direction = is_read ? DIR_IN : DIR_OUT;
    else
        direction = DIR_NONE;

    rc = sc->transfer(sc, lun, rcmd, rcmdlen, data, data_len, flags, direction,
        timeout, umass_complete_cb, tc);
    if (rc)
        return STATUS_WIRE_FAILED;

    return 0;
}

void *umass_attach_up(void *softc)
{
    struct umass_softc *sc = (struct umass_softc *)softc;
    juint8_t subclass;
    void *user_ctx;

    switch (sc->proto & UMASS_PROTO_COMMAND) 
    {
    case UMASS_PROTO_SCSI: 
        subclass = UISUBCLASS_SCSI;
        break;
    case UMASS_PROTO_ATAPI:
        subclass = UISUBCLASS_SFF8070I;
        break;
    case UMASS_PROTO_UFI:
        subclass = UISUBCLASS_UFI;
        break;
    case UMASS_PROTO_RBC:
        subclass = UISUBCLASS_RBC;
        break;
    default:
        DBG_E(DMASS_UMASS, ("UMASS: Unsupported command protocol %ld\n",
            sc->proto&UMASS_PROTO_COMMAND));
        return NULL;
    }

    if (jms_transport_attach((void *)sc, (void *)sc->sc_udev, sc->maxlun, 
        subclass, &user_ctx))
    {
        return NULL;
    }

    return (void *)user_ctx;
}

#endif

static jresult_t umass_suspend(jdevice_t dev)
{
    struct umass_softc *sc = j_device_get_softc(dev);
    
    DBG_I(DMASS_POWER, ("UMASS: Suspending %p\n", dev));
    return jms_transport_suspend((void *)sc->umass_sim);
}

static jresult_t umass_resume(jdevice_t dev)
{
    struct umass_softc *sc = j_device_get_softc(dev);
    
    DBG_I(DMASS_POWER, ("UMASS: Resuming %p\n", dev));
    jms_transport_resume((void *)sc->umass_sim);
    
    return 0;
}

static jresult_t umass_ioctl(jdevice_t self, drv_ioctl_t ioctl, void *arg)
{
    DBG_E(DMASS_UMASS, ("UMASS: Unsupported IOCTL %x\n", ioctl));
    return JEINVAL;
}

static jresult_t umass_custom_init(void)
{
    jresult_t rc = 0;
#ifdef CONFIG_JUMASS_ASYNC
    rc = jms_async_init();
    if (rc)
        return rc;
#endif
    return rc;
}

static jresult_t umass_custom_uninit(void)
{
#ifdef CONFIG_JUMASS_ASYNC
    jms_async_uninit();
#endif
    return 0;
}

jresult_t umass_suspend_self(void *dev_h)
{
    struct umass_softc *sc = (struct umass_softc *)dev_h;
    return j_device_ioctl(j_device_get_parent(sc->sc_dev),
        DRV_IOCTL_PWR_CHILD_SUSPEND, (void *)sc->sc_dev);
}

jresult_t umass_resume_self(void *dev_h)
{
    struct umass_softc *sc = (struct umass_softc *)dev_h;
    return j_device_ioctl(j_device_get_parent(sc->sc_dev),
        DRV_IOCTL_PWR_CHILD_RESUME, (void *)sc->sc_dev);
}

DRIVER_USB_DECLARE_CUSTOM_INIT(umass, umass_custom_init, umass_custom_uninit)

