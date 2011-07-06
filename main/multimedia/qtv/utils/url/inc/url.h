#ifndef _URL_CLASS_H_
#define _URL_CLASS_H_

/* =======================================================================

                          Url.h

DESCRIPTION
  Header file of the QTV URL class

Copyright 2005, 2006 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/url/main/latest/inc/url.h#2 $
$DateTime: 2010/01/04 23:14:42 $
$Change: 1127650 $ 

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "oscl_types.h"

//Max length of a URN, both EFS file name and "rtsp:" spec
const uint32 QTV_MAX_URN_BYTES = 2048;

//max relative control path length
const uint16 QTV_CONTROL_RELATIVE_URL_LEN = 30;

// maximum size of host name, by RFC 1035 
const uint16 HOSTNAME_STR_SIZE  = 255;

//max uint32 number's size
const uint8 PORT_STRING_SIZE = 10;

//forward declarations
//enum QtvStreamCode;



/* ======================================================================
CLASS:
  URL

DESCRIPTION:
  URL class provides functions required to manipulate URLs it has 
  also IPV6 awareness.
======================================================================*/

class URL 
{
  public:
    //Error Codes
    enum UrlErrCode
    {
      URL_NOT_DEFINED = -1,
      URL_OK = 0,
      URL_BAD_FORMAT,
      URL_EMPTY,
      URL_BUFFER_NULL,
      URL_MAX_LEN_EXCEEDS,
      URL_OUT_OF_MEMORY,
      URL_UNKNOWN_PROTOCOL,
      URL_ERROR
    };

    enum UrlProtocol
    {
      PROTO_HTTP,
      PROTO_RTSP,
      PROTO_RTSPT,
      PROTO_ISDB,
      PROTO_SDP_BUFFER,
      PROTO_UNKNOWN
    };

    static const int DEF_RTSP_PORT = 554;
    static const int  DEF_FASTTRACK_PORT = 9300;

    /*URL Maximum length, configurable through StreamerConfg*/
    static uint32 MaxUrlSize;
    static uint32 MaxUrlSizeWithCtrlPath;

    //Ctors
    URL();
    //Creates a URL object based on an existing URL object.
    URL(const URL &url);
    URL(const char* urlStr);
    URL(uint32 urlSize);

    //Dtor
    ~URL();
    
    //initialization
    bool Init(uint32 len = 0);

    // Assigns a new value to the URL object.
    URL & operator =(const URL & otherUrl);
    URL & operator =(const char *urlStr);
  
    //Checks whether the object and other url/str are identical.
    bool operator == (const URL & otherUrl) const;
    bool operator == (const char *urlstr) const;
  
    //Checks whether the object and other url/str are not identical.
    bool operator != (const URL & otherUrl) const;
    bool operator != (const char *urlstr) const;

    //provide “const char*” access to URL object 
    operator const char*() const;

    //clears the url info
    UrlErrCode Reset();

    //is url object empty
    bool IsEmpty() const;

    //is url buffer null
    bool IsNull() const;

    //find a substring in the URL object
    const char * Find(const char * str) const;

    //get number of allocated bytes for url buffer
    uint32 GetCapacity() const;

    //url length
    uint32 GetUrlLength() const;

    //provides char * access to url, can be manipulated in c-string style
    char *GetUrlBuffer();
    const char *GetUrlBuffer() const;
    
    //set url
    UrlErrCode SetUrl(const char* urlStr, int32 copylen = -1);
    UrlErrCode SetUrl(const URL * urlObj);

    //concatenates a string to the url
    UrlErrCode ConcatUrl(const char * str);
  
    //skip numChars in url 
    uint32 SkipCharsAtStart(uint32 numChars);

    //Returns TRUE if the other-Uri matches with the start of the Url.
    bool StartsWith(const URL & otherUrl) const;
    bool StartsWith(const char * str) const;
    bool EndsWith(const char * str) const;

    //checks a URL whether it meets RFC 2396
    UrlErrCode ValidateURL() const;

    //make a non null url buffer, and resets it
    void MakeEmptyNonNullUrl();

    
    //strips control chars in url
    UrlErrCode StripControlChars();

    //get the protocol
    enum UrlProtocol GetProtocol() const;
  
    //get the ip addr/dns name
    UrlErrCode GetHost(char * host,int maxHostLen) const;

    //returns the port number if present
    UrlErrCode GetPort(uint32 * port) const;

    //returns the clip name
    UrlErrCode GetClipName(char * clip_name, int size) const;
  
    //check whether host is literal address or ip address
    bool IsHostLiteralAddress() const;
  
    //error handling
    //Get the recent error
    UrlErrCode GetErrorCode() const;
    
    //Mpas url error codes to PV error codes
    int32 MapToPvErrorCode(
         UrlErrCode errCode = URL_NOT_DEFINED) const;

    //Ipv6 specific
#if defined(FEATURE_QTV_IPV6) || defined(FEATURE_QTV_MCAST_IPV6)
#error code not present
#endif

  private:
    //url buffer
    char * url;

    //holds allocated number of bytes
    uint32 capacity;

    //holds current url length
    uint32 urlLen;

    //recent error occured
    mutable UrlErrCode errCode;

    //member functions
    //empties url object, deletes buffer as well
    void Clear();

    //reset the error code
    void ResetErrorCode() const;
};

#endif

