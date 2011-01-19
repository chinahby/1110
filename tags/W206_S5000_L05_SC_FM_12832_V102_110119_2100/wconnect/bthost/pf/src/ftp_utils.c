/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/ftp_utils.c#1 $ 

$Log: $
*
*    #5        18 Oct 2006            PR
* Merged OI Code v3.0.14rev1revQ
*
*    #4        04 Nov 2004            JH
* Updated to version 3.0.12 revision 3 baseline
*
===========================================================================*/
/** 
  @file  

  File Transfer Profile utilities for parsing folder listing XML

 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
#define __OI_MODULE__ OI_MODULE_FTP_SRV

#include "oi_ftp_utils.h"
#include "oi_memmgr.h"
#include "oi_unicode.h"

/*
 * The BOM is a Unicode special marker placed at the start of the stream that
 * indicates its encoding. A BOM is mandatory for UTF16 but is optional for
 * UTF8. 
 */

static const OI_BYTE UTF8[]     = { 0xEF, 0xBB, 0xBF };
static const OI_BYTE UTF16_BE[] = { 0xFE, 0xFF };
static const OI_BYTE UTF16_LE[] = { 0xFF, 0xFE };


/*
 * Returns 0 or the length of the matched text.
 */

static OI_UINT16 TestText(OI_CHAR *txt,
                          OI_FTP_PARSE_INFO *parseInfo)
{
    OI_UINT16 pos = parseInfo->xmlPos;

    switch (parseInfo->encoding) {
        case OI_FTP_UTF8:
            while (*txt == parseInfo->xmlStream[pos]) {
                if (++pos >= parseInfo->xmlSize) {
                    return 0;
                }
                ++txt;
            }
            return (*txt == 0) ? (pos - parseInfo->xmlPos) : 0;
        case OI_FTP_UTF16_BE:
            OI_LOG_ERROR(("UTF16 not implemented"));
            break;
        case OI_FTP_UTF16_LE:
            OI_LOG_ERROR(("UTF16 not implemented"));
            break;

    }
    return 0;
}

/*
 * Leaves position at first character after text.
 */
static OI_BOOL ScanToText(OI_CHAR *txt,
                          OI_FTP_PARSE_INFO *parseInfo)
{
    OI_UINT16 match;

    switch (parseInfo->encoding) {
        case OI_FTP_UTF8:
            while (TRUE) {
                while (*txt != parseInfo->xmlStream[parseInfo->xmlPos]) {
                    if (++parseInfo->xmlPos == parseInfo->xmlSize) {
                        return FALSE;
                    }
                }
                if ((match = TestText(txt, parseInfo)) != 0) {
                    parseInfo->xmlPos += match;
                    return TRUE;
                }
                ++parseInfo->xmlPos;
            }
            /* Unreachable */
        case OI_FTP_UTF16_BE:
            OI_LOG_ERROR(("UTF16 not implemented"));
            break;
        case OI_FTP_UTF16_LE:
            OI_LOG_ERROR(("UTF16 not implemented"));
            break;
        
    }
    return FALSE;
}

/*
 * Parse out a quoted string from the XML stream and return it as 16 bit wide
 * characters.
 */
static OI_STATUS ParseQString(OI_FTP_PARSE_INFO *parseInfo,
                              OI_OBEX_UNICODE *qstring)
{
    OI_STATUS status = OI_OK;
    OI_UINT16 start;
    OI_UINT16 end;

    if (!ScanToText("\"", parseInfo)) {
        return OI_STATUS_PARSE_ERROR;
    }
    start = parseInfo->xmlPos;
    if (!ScanToText("\"", parseInfo)) {
        return OI_STATUS_PARSE_ERROR;
    }
    end = parseInfo->xmlPos - 1;

    switch (parseInfo->encoding) {
        case OI_FTP_UTF8:
            qstring->len = 1 + end - start;
            qstring->str = OI_Malloc(qstring->len * sizeof(OI_CHAR16));
            if (qstring->str == NULL) {
                return OI_STATUS_OUT_OF_MEMORY;
            }

            status = OI_Utf8ToUtf16(parseInfo->xmlStream + start, end - start,
                                    qstring->str, qstring->len);
            if (OI_SUCCESS(status)) {
                qstring->len = OI_StrLenUtf16(qstring->str) + 1;
            } else {
                OI_LOG_ERROR(("Failed to convert UTF8 string to UTF16: %!\n", status));
            }
            break;
        case OI_FTP_UTF16_BE:
            OI_LOG_ERROR(("UTF16 not implemented"));
            break;
        case OI_FTP_UTF16_LE:
            OI_LOG_ERROR(("UTF16 not implemented"));
            break;
        
    }
    return status;
}


/*
 * Try and determine the encoding of the XML stream.  We currently support ascii
 * (as UTF8) and UTF16 unicode (as UTF16). Then skip forrward in the stream to
 * the start of the folder listing.  
 */

OI_STATUS OI_FTP_FolderParseInit(OI_FTP_PARSE_INFO *parseInfo)
{
    /*
     * Default encoding is UTF8. In UTF8 ascii characters have their standard
     * representation, in UTF16 ascii characters are represented as 16 bit
     * values.
     */
    parseInfo->encoding = OI_FTP_UTF8;

    switch (parseInfo->xmlStream[0]) {
        case 0xEF:
            if (parseInfo->xmlStream[1] == UTF8[1] && parseInfo->xmlStream[2] == UTF8[2]) {
                parseInfo->encoding = OI_FTP_UTF8;
                parseInfo->xmlPos += 3;
            }
            break;
        case 0xFE:
            if (parseInfo->xmlStream[1] == UTF16_BE[1]) {
                parseInfo->encoding = OI_FTP_UTF16_BE;
                parseInfo->xmlPos += 2;
            }
            break;
        case 0xFF:
            if (parseInfo->xmlStream[1] == UTF16_LE[1]) {
                parseInfo->encoding = OI_FTP_UTF16_LE;
                parseInfo->xmlPos += 2;
            }
            break;
    }
    /*
     * Advance to start of actual folder listing
     */
    if (!ScanToText("<folder-listing", parseInfo)) {
        return OI_STATUS_PARSE_ERROR;
    }
    if (!ScanToText(">", parseInfo)) {
        return OI_STATUS_PARSE_ERROR;
    }
    return OI_OK;
}

/*
 * Find the next file or folder tag in the XML stream. We assume that the XML is
 * well-formed so we do very little syntax checking.
 */
OI_STATUS OI_FTP_FolderParseNext(OI_FTP_PARSE_INFO *parseInfo)
{
    while (TRUE) {
        if (!ScanToText("<", parseInfo)) {
            return OI_STATUS_PARSE_ERROR;
        }
        if (TestText("parent-folder/>", parseInfo)) {
            continue;
        }
        if (TestText("folder ", parseInfo)) {
            if (!ScanToText("name=", parseInfo)) {
                return OI_STATUS_PARSE_ERROR;
            } 
            parseInfo->isFolder = TRUE;
            break;
        }
        if (TestText("file ", parseInfo)) {
            if (!ScanToText("name=", parseInfo)) {
                return OI_STATUS_PARSE_ERROR;
            }
            parseInfo->isFolder = FALSE;
            break;
        }
        if (TestText("/folder-listing>", parseInfo)) {
            /*
             * Set position to end to indicate completion.
             */
            parseInfo->xmlPos = parseInfo->xmlSize;
            return OI_OK;
        }
    }
    return ParseQString(parseInfo, &parseInfo->name);
}

