/*****************************************************************************
**
**  Name:           bta_pbc_api.h
**
**  Description:    This is the public interface file for the Phone Book Access Client
**                  subsystem of BTA, Widcomm's
**                  Bluetooth application layer for mobile phones.
**
**  Copyright (c) 2003-2005, Widcomm Inc., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_PBC_API_H
#define BTA_PBC_API_H

#include "bta_api.h"
#include "btm_api.h"
#include "bta_sys.h"

/*****************************************************************************
**  Constants and data types
*****************************************************************************/

/**************************
**  Common Definitions
***************************/
/* Access response types */
#define BTA_PBC_ACCESS_ALLOW     0   /* Allow the requested operation */
#define BTA_PBC_ACCESS_FORBID    1   /* Disallow the requested operation */

typedef UINT8 tBTA_PBC_ACCESS;

/* Access event operation types */
#define BTA_PBC_OPER_GET         1   /* Request is a GET file */
#define BTA_PBC_OPER_CHG_DIR     2   /* Request is a Change Folder */

typedef UINT8 tBTA_PBC_OPER;

/**************************
**  Client Definitions
***************************/
/* Extra Debug Code */
#ifndef BTA_PBC_DEBUG
#define BTA_PBC_DEBUG           FALSE
#endif


/* Client callback function events */
#define BTA_PBC_ENABLE_EVT      0   /* Phone Book Access client is enabled. */
#define BTA_PBC_OPEN_EVT        1   /* Connection to peer is open. */
#define BTA_PBC_CLOSE_EVT       2   /* Connection to peer closed. */
#define BTA_PBC_AUTH_EVT        3   /* Request for Authentication key and user id */
#define BTA_PBC_LIST_EVT        4   /* Event contains a directory entry (tBTA_PBC_LIST) */
#define BTA_PBC_PROGRESS_EVT    5   /* Number of bytes read or written so far */
#define BTA_PBC_GETFILE_EVT     6   /* Get complete */
#define BTA_PBC_CHDIR_EVT       7   /* Change Directory complete */
#define BTA_PBC_PHONEBOOK_EVT   8   /* Report the Application Parameters for BTA_PbcGetPhoneBook response */


typedef UINT8 tBTA_PBC_EVT;

#define BTA_PBC_OK              0
#define BTA_PBC_FAIL            1
#define BTA_PBC_NO_PERMISSION   2
#define BTA_PBC_NOT_FOUND       3
#define BTA_PBC_FULL            4
#define BTA_PBC_BUSY            5
#define BTA_PBC_ABORTED         6

typedef UINT8 tBTA_PBC_STATUS;

#define BTA_PBC_FLAG_NONE       0
#define BTA_PBC_FLAG_BACKUP     1

typedef UINT8 tBTA_PBC_FLAG;

typedef struct
{
    tBTA_SERVICE_ID service;    /* Connection is open with PBAP */
} tBTA_PBC_OPEN;

#define BTA_PBC_FILTER_VERSION  (1<<0)  /* Version */
#define BTA_PBC_FILTER_FN       (1<<1)  /* Formatted Name */
#define BTA_PBC_FILTER_N        (1<<2)  /* Structured Presentation of Name */
#define BTA_PBC_FILTER_PHOTO    (1<<3)  /* Associated Image or Photo */
#define BTA_PBC_FILTER_BDAY     (1<<4)  /* Birthday */
#define BTA_PBC_FILTER_ADR      (1<<5)  /* Delivery Address */
#define BTA_PBC_FILTER_LABEL    (1<<6)  /* Delivery */
#define BTA_PBC_FILTER_TEL      (1<<7)  /* Telephone Number */
#define BTA_PBC_FILTER_EMAIL    (1<<8)  /* Electronic Mail Address */
#define BTA_PBC_FILTER_MAILER   (1<<9)  /* Electronic Mail */
#define BTA_PBC_FILTER_TZ       (1<<10)  /* Time Zone */
#define BTA_PBC_FILTER_GEO      (1<<11) /* Geographic Position */
#define BTA_PBC_FILTER_TITLE    (1<<12) /* Job */
#define BTA_PBC_FILTER_ROLE     (1<<13) /* Role within the Organization */
#define BTA_PBC_FILTER_LOGO     (1<<14) /* Organization Logo */
#define BTA_PBC_FILTER_AGENT    (1<<15) /* vCard of Person Representing */
#define BTA_PBC_FILTER_ORG      (1<<16) /* Name of Organization */
#define BTA_PBC_FILTER_NOTE     (1<<17) /* Comments */
#define BTA_PBC_FILTER_REV      (1<<18) /* Revision */
#define BTA_PBC_FILTER_SOUND    (1<<19) /* Pronunciation of Name */
#define BTA_PBC_FILTER_URL      (1<<20) /* Uniform Resource Locator */
#define BTA_PBC_FILTER_UID      (1<<21) /* Unique ID */
#define BTA_PBC_FILTER_KEY      (1<<22) /* Public Encryption Key */
#define BTA_PBC_FILTER_ALL      (0)
typedef UINT32 tBTA_PBC_FILTER_MASK;

enum
{
    BTA_PBC_FORMAT_CARD_21, /* vCard format 2.1 */
    BTA_PBC_FORMAT_CARD_30, /* vCard format 3.0 */
    BTA_PBC_FORMAT_MAX
};
typedef UINT8 tBTA_PBC_FORMAT;

typedef struct
{
    UINT16          phone_book_size;
    BOOLEAN         pbs_exist;          /* phone_book_size is present in the response */
    UINT8           new_missed_calls;
    BOOLEAN         nmc_exist;          /* new_missed_calls is present in the response */
} tBTA_PBC_PB_PARAM;

typedef struct
{
    tBTA_PBC_PB_PARAM *p_param;
    UINT8           *data;
    UINT16           len;
    BOOLEAN          final;     /* If TRUE, entry is last of the series */
    tBTA_PBC_STATUS  status;    /* Fields are valid when status is BTA_PBC_OK */
} tBTA_PBC_LIST;

enum
{
    BTA_PBC_ORDER_INDEXED = 0,  /* indexed */
    BTA_PBC_ORDER_ALPHANUM,     /* alphanumeric */
    BTA_PBC_ORDER_PHONETIC,      /* phonetic */
    BTA_PBC_ORDER_MAX
};
typedef UINT8 tBTA_PBC_ORDER;
enum
{
    BTA_PBC_ATTR_NAME = 0,      /* name */
    BTA_PBC_ATTR_NUMBER,        /* number */
    BTA_PBC_ATTR_SOUND,         /* sound */
    BTA_PBC_ATTR_MAX
};
typedef UINT8 tBTA_PBC_ATTR;

typedef struct
{
    UINT32 file_size;   /* Total size of file (BTA_FS_LEN_UNKNOWN if unknown) */
    UINT16 bytes;       /* Number of bytes read or written since last progress event */
} tBTA_PBC_PROGRESS;

typedef struct
{
    UINT8  *p_realm;
    UINT8   realm_len;
    UINT8   realm_charset;
    BOOLEAN userid_required;    /* If TRUE, a user ID must be sent */
} tBTA_PBC_AUTH;


typedef union
{
    tBTA_PBC_STATUS     status;
    tBTA_PBC_OPEN       open;
    tBTA_PBC_LIST       list;
    tBTA_PBC_PROGRESS   prog;
    tBTA_PBC_AUTH       auth;
    tBTA_PBC_PB_PARAM   pb;
} tBTA_PBC;

/* Client callback function */
typedef void tBTA_PBC_CBACK(tBTA_PBC_EVT event, tBTA_PBC *p_data);


/*****************************************************************************
**  External Function Declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/**************************
**  Client Functions
***************************/

/*******************************************************************************
**
** Function         BTA_PbcEnable
**
** Description      Enable the phone book access client.  This function must be
**                  called before any other functions in the PBC API are called.
**                  When the enable operation is complete the callback function
**                  will be called with an BTA_PBC_ENABLE_EVT event.
**                  
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_PbcEnable(tBTA_PBC_CBACK *p_cback, UINT8 app_id);

/*******************************************************************************
**
** Function         BTA_PbcDisable
**
** Description      Disable the phone book access client.  If the client is currently
**                  connected to a peer device the connection will be closed.
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_PbcDisable(void);

/*******************************************************************************
**
** Function         BTA_PbcOpen
**
** Description      Open a connection to an PBAP server.  
**                  
**                  When the connection is open the callback function
**                  will be called with a BTA_PBC_OPEN_EVT.  If the connection
**                  fails or otherwise is closed the callback function will be
**                  called with a BTA_PBC_CLOSE_EVT.
** 
**                  Note: Pbc always enable (BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)  
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_PbcOpen(BD_ADDR bd_addr, tBTA_SEC sec_mask);

/*******************************************************************************
**
** Function         BTA_PbcClose
**
** Description      Close the current connection to the server. Aborts any
**                  active PBAP transfer.
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_PbcClose(void);


/*******************************************************************************
**
** Function         BTA_PbcGetPhoneBook
**
** Description      Retrieve a PhoneBook from the peer device and copy it to the
**                  local file system.
**
**                  This function can only be used when the client is connected
**                  in PBAP mode.
**
** Note:            local file name is specified with a fully qualified path.
**                  Remote file name is absolute path in UTF-8 format
**                  (telecom/pb.vcf or SIM1/telecom/pb.vcf).
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_PbcGetPhoneBook(char *p_local_name, char *p_remote_name,
                         tBTA_PBC_FILTER_MASK filter, tBTA_PBC_FORMAT format,
                         UINT16 max_list_count, UINT16 list_start_offset);

/*******************************************************************************
**
** Function         BTA_PbcGetCard
**
** Description      Retrieve a vCard from the peer device and copy it to the
**                  local file system.
**
**                  This function can only be used when the client is connected
**                  in PBAP mode.
**
** Note:            local file name is specified with a fully qualified path.
**                  Remote file name is relative path in UTF-8 format.
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_PbcGetCard(char *p_local_name, char *p_remote_name,
                    tBTA_PBC_FILTER_MASK filter, tBTA_PBC_FORMAT format);


/*******************************************************************************
**
** Function         BTA_PbcChDir
**
** Description      Change PB path on the peer device.
**
**                  This function can only be used when the client is connected
**                  in PBAP mode.
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_PbcChDir(char *p_dir, tBTA_PBC_FLAG flag);

/*******************************************************************************
**
** Function         BTA_PbcAuthRsp
**
** Description      Sends a response to an OBEX authentication challenge to the
**                  connected OBEX server. Called in response to an BTA_PBC_AUTH_EVT
**                  event.
**
** Note:            If the "userid_required" is TRUE in the BTA_PBC_AUTH_EVT event,
**                  then p_userid is required, otherwise it is optional.
**
**                  p_password  must be less than BTA_PBC_MAX_AUTH_KEY_SIZE (16 bytes)
**                  p_userid    must be less than OBX_MAX_REALM_LEN (defined in target.h)
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_PbcAuthRsp (char *p_password, char *p_userid);

/*******************************************************************************
**
** Function         BTA_PbcListCards
**
** Description      Retrieve a directory listing from the peer device. 
**                  When the operation is complete the callback function will
**                  be called with one or more BTA_PBC_LIST_EVT events
**                  containing directory list information formatted as described
**                  in the PBAP Spec, Version 0.9, section 3.1.6.
**                  This function can only be used when the client is connected
**                  to a peer device.
**
**                  This function can only be used when the client is connected
**                  in PBAP mode.
**
** Parameters       p_dir - Name of directory to retrieve listing of.
**
** Returns          void
**
*******************************************************************************/

BTA_API extern void BTA_PbcListCards(char *p_dir, tBTA_PBC_ORDER order, char *p_value,
                      tBTA_PBC_ATTR attribute, UINT16 max_list_count,
                      UINT16 list_start_offset);


/*******************************************************************************
**
** Function         BTA_PbcAbort
**
** Description      Aborts any active PBC operation.
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_PbcAbort(void);


#ifdef __cplusplus
}
#endif

#endif /* BTA_PBC_API_H */
