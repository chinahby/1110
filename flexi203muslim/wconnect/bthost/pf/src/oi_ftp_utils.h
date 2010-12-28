#ifndef _OI_FTP_UTILS_H
#define _OI_FTP_UTILS_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_ftp_utils.h#1 $ 

$Log: $

===========================================================================*/
/** 
  @file  

  File Transfer Profile utilities for parsing folder listing XML

 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_stddefs.h"
#include "oi_obexspec.h"
#include "oi_status.h"


typedef enum {
    OI_FTP_UTF8,      /* Single-byte encoding for ascii */
    OI_FTP_UTF16_BE,  /* Big-endian double-byte encoding for ascii */
    OI_FTP_UTF16_LE   /* Little-endian double-byte encoding for ascii */
} OI_FTP_XML_ENCODING;


typedef struct {
    OI_FTP_XML_ENCODING encoding; /**< Character encoding of stream */
    OI_BYTE *xmlStream;           /**< Pointer to XML stream */
    OI_UINT16 xmlSize;            /**< Size of XML stream */
    OI_UINT16 xmlPos;             /**< Current position in XML stream */
    OI_OBEX_UNICODE name;         /**< Name is always converted to unicode */
    OI_BOOL isFolder;             /**< Is the current node a folder or a file */
    OI_BOOL readOnly;             /**< Readonly permission? */
} OI_FTP_PARSE_INFO;



OI_STATUS OI_FTP_FolderParseInit(OI_FTP_PARSE_INFO *parseInfo);


OI_STATUS OI_FTP_FolderParseNext(OI_FTP_PARSE_INFO *parseInfo);


#endif /* _OI_FTP_UTILS_H */
