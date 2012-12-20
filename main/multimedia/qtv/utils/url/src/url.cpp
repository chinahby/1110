/* =======================================================================

                          Url.cpp

DESCRIPTION
  Implementation of the QTV URL class

Copyright 2005, 2006 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/url/main/latest/src/url.cpp#9 $
$DateTime: 2010/04/23 00:39:40 $
$Change: 1276762 $

========================================================================== */

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qtvsystem.h"
#include "qcplayer_oscl_utils.h"
#include "zrex_string.h"
#include "Url.h"
#include "qtvstreambase.h"
#include "oscl_string_utils.h"


/*definitions for max url length variables*/
uint32 URL::MaxUrlSize = QTV_MAX_URN_BYTES;

/* MaxUrlSizeWithCtrlPath is MaxUrlSize (currently QTV_MAX_URN_BYTES)
plus "/streamid = *(digit)" */
uint32 URL::MaxUrlSizeWithCtrlPath = 
                URL::MaxUrlSize + QTV_CONTROL_RELATIVE_URL_LEN;


/* ======================================================================
FUNCTION:
  URL::URL

DESCRIPTION:
  constructor

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
======================================================================*/
URL::URL()
{
  url = NULL;
  capacity = 0;
  urlLen = 0;
  errCode = URL_OK;
}

/* ======================================================================
FUNCTION:
  URL::~URL

DESCRIPTION:
  Destructor

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
======================================================================*/
URL::~URL()
{
  if (NULL != url)
  {
    QTV_Free(url);
  }
}
 
/* ======================================================================
FUNCTION:
  URL::URL

DESCRIPTION:
  Creates a URL object based on an existing URL object

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
======================================================================*/
URL::URL(const URL &otherUrl)
{
  urlLen = 0;
  capacity = 0;
  errCode = URL_OK;
  url = NULL;
 
  //no buffer or url object empty,
  //clear our url
  if (otherUrl.IsNull() || otherUrl.IsEmpty())
  {
    Clear();
    return;
  }

  url = (char *) QTV_Malloc((otherUrl.urlLen+1) * sizeof(char));
  if (NULL == url)
  {
    errCode = URL_OUT_OF_MEMORY;
  }
  else
  {
    std_strlcpy(url, otherUrl.url, otherUrl.urlLen+1);
    urlLen = otherUrl.urlLen;
    capacity = otherUrl.urlLen + 1;
  }
}

/* ======================================================================
FUNCTION:
  URL::URL

DESCRIPTION:
  Creates a URL object based on an existing string

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
======================================================================*/
URL::URL(const char* urlStr)
{
  uint32 len;
  errCode = URL_OK;

  urlLen = 0;
  capacity = 0;

  if (NULL == urlStr)
  {
    url = NULL;
  }
  else
  {
    len = strlen(urlStr);
    if (len > MaxUrlSize)
    {
      errCode = URL_MAX_LEN_EXCEEDS;
      return;
    }

    url = (char *) QTV_Malloc((len + 1) * sizeof(char));
    if (NULL == url)
    {
      errCode = URL_OUT_OF_MEMORY;
    }
    else
    {
      std_strlcpy(url, urlStr, len+1);
      urlLen = len;
      capacity = len + 1;
    }
  } /*if (NULL == url)*/
}

/* ======================================================================
FUNCTION:
  URL::URL

DESCRIPTION:
  Creates an empty URL object based on the length

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
======================================================================*/
URL::URL(uint32 len)
{
  errCode = URL_OK;
  urlLen = 0;
  capacity = 0;
  url = NULL;

  if (len > MaxUrlSize)
  {
    errCode = URL_MAX_LEN_EXCEEDS;
    return;
  }

  if (len > 0)
  {
    url = (char *) QTV_Malloc((len + 1) * sizeof(char));  
    if (NULL == url)
    {
      errCode = URL_OUT_OF_MEMORY;
    }
    else
    {
      memset(url, 0x00, len);
      capacity = len + 1;
    }
  }
}

/* ======================================================================
FUNCTION:
  URL::URL

DESCRIPTION:
  initializes url object
  useful if one wants to do initialization in critical section

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
======================================================================*/
bool URL::Init(uint32 len)
{
  urlLen = 0;
  capacity = 0;
  url = NULL;
  errCode = URL_OK; 

  if (len > MaxUrlSize)
  {
    errCode = URL_MAX_LEN_EXCEEDS;
    return false;
  }

  if (len > 0)
  {
    url = (char *) QTV_Malloc((len + 1) * sizeof(char));  
    if (NULL == url)
    {
      errCode = URL_OUT_OF_MEMORY;
      return false;
    }
    else
    {
      memset(url, 0x00, len);
      capacity = len + 1;
    }
  }
  return true;
}

/* ======================================================================
FUNCTION:
  URL::operator =

DESCRIPTION:
   Assigns a new value to the URL object

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
======================================================================*/
URL & URL::operator =(const URL & otherUrl)
{
  errCode = URL_OK;

  //no buffer, delete our source url buffer as well
  if (otherUrl.IsNull())
  {
    Clear();
    return *this;
  }

  //url object empty, reset source url object
  if (otherUrl.IsEmpty())
  {
    Reset();
    return *this;
  }

  if (capacity <= otherUrl.urlLen)
  {
    if (NULL != url)
    {
      QTV_Free(url);
    }
    url = (char *) QTV_Malloc((otherUrl.urlLen + 1) * sizeof(char)); 

    //we resized allocated memory, so update length
    capacity = otherUrl.urlLen + 1;
  }

  if (NULL == url)
  {
    Clear();
    errCode = URL_OUT_OF_MEMORY;
  }
  else
  {
    std_strlcpy(url, otherUrl.url, otherUrl.urlLen + 1);
    urlLen = otherUrl.urlLen;
  }

  return *this;
}

/* ======================================================================
FUNCTION:
  URL::operator =

DESCRIPTION:
   Assigns a new value to the URL object

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
======================================================================*/
URL & URL::operator =(const char *urlStr)
{
  uint32 urlSize;
  errCode = URL_OK;

  if (NULL == urlStr)
  {
    Clear();
  }
  else
  {
    urlSize = strlen(urlStr);
    
    if (urlSize > MaxUrlSize)
    {
      Reset();
      errCode = URL_MAX_LEN_EXCEEDS;
      return *this;
    }

    if (urlLen < urlSize)
    {
      if (NULL != url)
      {
        QTV_Free(url);
      }
      url = (char *) QTV_Malloc((urlSize + 1) * sizeof(char)); 
      capacity = urlSize + 1;
    }
    if (NULL == url)
    {
      Clear();
      errCode = URL_OUT_OF_MEMORY;
    }
    else
    {
      std_strlcpy(url, urlStr, urlSize + 1);
      urlLen = urlSize;
    }
  } /*if (NULL == urlStr)*/
  return *this;
}

/*======================================================================
FUNCTION:
  operator == 

DESCRIPTION:
  Checks whether the object and otherUrl are identical.

INPUT/OUTPUT PARAMETERS:
  

RETURN VALUE:
  

SIDE EFFECTS:
  None.
======================================================================*/
bool URL::operator == (const URL & otherUrl) const
{
  if(!IsNull() && !otherUrl.IsNull() &&
     (urlLen == otherUrl.urlLen) && 
     ZUtils::StrncmpI(url, otherUrl.url, urlLen))
  {
    return true;
  }
  return false;
}

/*======================================================================
FUNCTION:
  operator == 

DESCRIPTION:
  Checks whether the object and urlstr are identical.

INPUT/OUTPUT PARAMETERS:
  

RETURN VALUE:
  

SIDE EFFECTS:
  None.
======================================================================*/
bool URL::operator == (const char * urlStr) const
{
  if(!IsNull() && (NULL != urlStr) && 
      (urlLen == strlen(urlStr)) &&
      ZUtils::StrncmpI(url, urlStr, urlLen))
  {
    return true;
  }
  return false;
}


/*======================================================================
FUNCTION:
  operator != 

DESCRIPTION:
  Checks whether the object and otherUrl are not identical.

INPUT/OUTPUT PARAMETERS:
  

RETURN VALUE:
  

SIDE EFFECTS:
  None.
======================================================================*/
bool URL::operator != (const URL & otherUrl) const
{
    if(!(*this == otherUrl))
  {
    return true;
  }
  return false;
}

/*======================================================================
FUNCTION:
  operator != 

DESCRIPTION:
  Checks whether the object and urlstr are not identical.

INPUT/OUTPUT PARAMETERS:
  

RETURN VALUE:
  

SIDE EFFECTS:
  None.
======================================================================*/
bool URL::operator != (const char * urlStr) const
{
  if(!(*this == urlStr))
  {
    return true;
  }
  return false;
}


/*======================================================================
FUNCTION:
  operator const char*

DESCRIPTION:
  provides “const char*” access to URL object

INPUT/OUTPUT PARAMETERS:
  

RETURN VALUE:
  

SIDE EFFECTS:
  None.
======================================================================*/
URL::operator const char*() const
{
  return url;
}

/* ======================================================================
FUNCTION:
  URL::Reset

DESCRIPTION:
   reset url object, don't delete buffer

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
======================================================================*/
URL::UrlErrCode URL::Reset()
{
  ResetErrorCode();

  if (NULL != url)
  {
    memset(url, 0x00, capacity);
  }

  urlLen = 0;

  return errCode;
}
  
/* ======================================================================
FUNCTION:
  URL::IsEmpty

DESCRIPTION:
   finds out whether url object is empty

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
======================================================================*/
bool URL::IsEmpty() const
{
  if ((NULL == url) || (capacity == 0) || (urlLen == 0) || !strlen(url))
  {
    return true;
  }
  return false;
}

/* ======================================================================
FUNCTION:
  URL::IsNull

DESCRIPTION:
   finds out whether url object's url string is NULL or not

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
======================================================================*/
bool URL::IsNull() const
{
  if (NULL == url)
  {
    return true;
  }
  return false;
}

/* ======================================================================
FUNCTION:
  URL::Clear

ACCESS: private 

DESCRIPTION:
   make url buffer empty, and deletes buffer
   if you don't want to delete buffer, use Reset()

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
======================================================================*/
void URL::Clear()
{
  errCode = URL_OK;

  if (NULL != url)
  {
    QTV_Free(url);
  }
  url = NULL;
  urlLen = 0;
  capacity = 0;
}

/* ======================================================================
FUNCTION:
  URL::Find

DESCRIPTION:
   find a substring in the URL object
   uses case-insensive compare

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
======================================================================*/
const char *URL::Find(const char * str) const
{
  if (!IsEmpty() && (NULL != str))
  {
    return zrex_strstri(url, str);
  }
  return NULL;
}

/* ======================================================================
FUNCTION:
  URL::GetCapacity

DESCRIPTION:
   Gets the length of the url

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
======================================================================*/
uint32 URL::GetCapacity() const
{
  return capacity;
}

/* ======================================================================
FUNCTION:
  URL::GetUrlLength

DESCRIPTION:
   Gets the length of the url

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
======================================================================*/
uint32 URL::GetUrlLength() const
{
  return urlLen;
}

/* ======================================================================
FUNCTION:
  URL::GetUrlBuffer

DESCRIPTION:
   Gets the url pointer, useful for manipulations directly on string 
   (c string like modifications)

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
======================================================================*/
char *URL::GetUrlBuffer()
{
  return url;
}

/* ======================================================================
FUNCTION:
  URL::GetUrlBuffer

DESCRIPTION:
   Gets the url pointer.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
======================================================================*/
const char *URL::GetUrlBuffer() const
{
  return (const char *) url;
}

/* ======================================================================
FUNCTION:
  URL::SetUrl

DESCRIPTION:
   sets the url

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:

SIDE EFFECTS:
======================================================================*/
URL::UrlErrCode URL::SetUrl(const char* urlStr, int32 copylen)
{
  uint32 len;
  errCode = URL_OK;

  if (NULL == urlStr)
  {
    Clear();
    return errCode;
  }

  if (copylen > -1)
  {
    len = copylen;
  }
  else
  {
    len = strlen(urlStr);
  }

  //check whether exceeds maximum length limitation
  if (len > MaxUrlSize)
  {
    Reset();
    errCode = URL_MAX_LEN_EXCEEDS;
    return errCode;
  }

  if (capacity <= len)
  {
    if (NULL != url)
    {
      QTV_Free(url);
    }

    url = (char *) QTV_Malloc((len + 1) * sizeof(char));

    //num alloc bytes changed, so update length
    capacity = len + 1;
  }

  if (NULL == url)
  {
    errCode = URL_OUT_OF_MEMORY;
    capacity = 0;
    urlLen = 0;
  }
  else
  {  
    ZUtils::StrcpyN(url, urlStr, len+1);
    urlLen = len;
  }
  
  return errCode;
}

/* ======================================================================
FUNCTION:
  URL::ConcatUrl

DESCRIPTION:
  concatenate str to url

INPUT/OUTPUT PARAMETERS:
  str   - source URL

RETURN VALUE:
  Url error code

SIDE EFFECTS:
======================================================================*/

URL::UrlErrCode URL::ConcatUrl(const char * str)
{
  char *tmpUrl;
  bool capacityAdjusted = false;

  errCode = URL_OK;

  if ((NULL == str) || IsNull())
  {
    //no need to concat
    errCode = URL_BUFFER_NULL;
    return errCode;
  }

  uint32 srcLen  = strlen(str);

  //check whether concatenated string exceeds maximum length limitation
  if ((urlLen + srcLen) > MaxUrlSize)
  {
    //no need to concatenate
    errCode = URL_MAX_LEN_EXCEEDS;
    return errCode;
  }

  tmpUrl = url;

  if ((urlLen + srcLen) > capacity)
  {
    tmpUrl = (char *) QTV_Malloc((urlLen + srcLen + 1 + 1) * sizeof(char)); 
    if (NULL == tmpUrl)
    {
      errCode = URL_OUT_OF_MEMORY;
      //lets leave the original state of URL object as it is, in this case
      return errCode;
    }
    std_strlcpy(tmpUrl, url, urlLen + srcLen + 1 + 1);

    //update capacity (extra +1 is probable "/" that may be 
    //required to add to url
    capacity = urlLen + srcLen + 1 + 1;

    capacityAdjusted = true;
  }
   
  // Offset from the beginning of srcURL, from which starts to copy
  uint8 offset   = 0;

  // Extra len introduced by '/'
  int8 extraLen  = 0;

  // prepare to concatenate
  if ((tmpUrl[urlLen - 1] != '/') && (str[0] != '/'))
  {
    /* If destURL doesn't ends with a '/' and srcURL doesn't begin with a '/',
    * add a '/' at the end
    */
    extraLen = 1;
  }
  else if ((tmpUrl[urlLen - 1] == '/') &&
           (str[0] == '/') )
  {
    /* If both have a '/' use only one of them */
    offset   = 1;
    extraLen = -1;
  }
  else
  {
    /* If only one of them has it('/'), we are fine*/
  }

  uint cpyLen = 0;
  cpyLen = srcLen - offset;
   
  if (cpyLen > 0)
  {
    if (extraLen > 0)
    {
      std_strlcat(tmpUrl, "/", capacity);
    }
    std_strlcat(tmpUrl, str + offset, capacity);
  }

  if ((capacityAdjusted) && (NULL != url))
  {
    QTV_Free(url);
  }

  url = tmpUrl;
  urlLen = urlLen + extraLen + offset + cpyLen;

  return errCode;
}

/*======================================================================
FUNCTION:
  SkipCharsAtStart

DESCRIPTION:
  skips num chars in the url object

INPUT/OUTPUT PARAMETERS:
  

RETURN VALUE:
  Returns TRUE if the otherUrl matches with the start of the Url or else false

SIDE EFFECTS:
  None.
======================================================================*/
uint32 URL::SkipCharsAtStart(uint32 numChars)
{
  if (!IsEmpty() && (numChars <= urlLen))
  {
    std_strlcpy(url, url+numChars, capacity);
    urlLen -= numChars;
    return numChars;
  }
  return 0;
}

/*======================================================================
FUNCTION:
  StartsWith

DESCRIPTION:
  compares (case-insensitive) the url object with start of the url

INPUT/OUTPUT PARAMETERS:
  

RETURN VALUE:
  Returns TRUE if the otherUrl matches with the start of the Url or else false

SIDE EFFECTS:
  None.
======================================================================*/
bool URL::StartsWith(const URL & otherUrl) const
{
  if ((!otherUrl.IsEmpty()) && !IsEmpty() && 
      (0 == ZUtils::Find(url, otherUrl.url)))
  {
    return true;
  }
  return false;
}

/*======================================================================
FUNCTION:
  StartsWith

DESCRIPTION:
  compares the string with start of the url
  uses case-insensive compare

INPUT/OUTPUT PARAMETERS:
  

RETURN VALUE:
  Returns TRUE if the string matches with the start of the Url or else false

SIDE EFFECTS:
  None.
======================================================================*/
bool URL::StartsWith(const char * str) const
{
  if ((NULL != str) && !IsEmpty() && 
      (0 == ZUtils::Find(url, str)))
  {
    return true;
  }
  return false;
}

/*======================================================================
FUNCTION:
  EndsWith

DESCRIPTION:
  compares (case-insensitive) the string with end of the url

INPUT/OUTPUT PARAMETERS:
  

RETURN VALUE:
  Returns TRUE if the string matches with the end of the Url, or else false

SIDE EFFECTS:
  None.
======================================================================*/
bool URL::EndsWith(const char * str) const
{
  if ((NULL != str) && !IsEmpty() && 
      (ZUtils::FindR(url, str) != ZUtils::npos))
  {
    return true;
  }
  return false;
}

/* ======================================================================
FUNCTION:
  ValidateURL

DESCRIPTION:
  validates the url

INPUT/OUTPUT PARAMETERS:
  

RETURN VALUE:
  returns error code

SIDE EFFECTS:
  None.
======================================================================*/
URL::UrlErrCode URL::ValidateURL() const
{
  ResetErrorCode();

  if (NULL == zrex_strstri(url, "//")) 
  {
    errCode = URL_BAD_FORMAT;
  }
  
  return errCode;
}

/* ======================================================================
FUNCTION:
  MakeEmptyNonNullUrl

DESCRIPTION:
  make a non null buffer, and empties it

  right now we are not using this, may be useful in future

INPUT/OUTPUT PARAMETERS:
  

RETURN VALUE:
  returns error code

SIDE EFFECTS:
  None.
======================================================================*/
void URL::MakeEmptyNonNullUrl()
{
  if (NULL == url)
  {
    url = (char *) QTV_Malloc(sizeof(char));
    if (url != NULL)
    {
    urlLen = 0;
    capacity = 1;
    url[0] = '\0';
  }
  }
  else
  {
    memset(url, 0x00, capacity);
    urlLen = 0;
  }
}

/* ======================================================================
FUNCTION:
  StripControlChars

DESCRIPTION:
  strips control chars in url

INPUT/OUTPUT PARAMETERS:
 

RETURN VALUE:
  returns error code

SIDE EFFECTS:
  None.
======================================================================*/
URL::UrlErrCode URL::StripControlChars()
{
  ResetErrorCode();

  if (IsEmpty())
  {
    errCode = URL_EMPTY;
    return errCode;
  }

  int newLength = strip_control_chars(url, urlLen);
  
  if (newLength != urlLen)
  {
    QTV_MSG_PRIO2(QTVDIAG_RTSP, QTVDIAG_PRIO_HIGH,
      "Modified original length from %d to %d", 
      urlLen, newLength);
  }
  urlLen = newLength;

  return errCode;
}

/* ======================================================================
FUNCTION:
  GetProtocol

DESCRIPTION:
  gets the protocol from the url

INPUT/OUTPUT PARAMETERS:
  

RETURN VALUE:
  returns protocol

SIDE EFFECTS:
  None.
======================================================================*/
enum URL::UrlProtocol URL::GetProtocol() const
{
  if (ZUtils::StrncmpI(url, "rtsp", 4))
  {
    return PROTO_RTSP;
  }
  else if (ZUtils::StrncmpI(url, "rtspt", 5))
  {
    return PROTO_RTSPT;
  }
  else if (ZUtils::StrncmpI(url, "http", 4))
  {
    return PROTO_HTTP;
  }
  else if (ZUtils::StrncmpI(url, "isdb", 4))
  {
    return PROTO_ISDB;
  }
  else if (ZUtils::StrncmpI(url, "sdp", 4))
  {
    return PROTO_SDP_BUFFER;
  }
  
  return PROTO_UNKNOWN;
}
  
/* ======================================================================
FUNCTION:
  GetHost

DESCRIPTION:
  gets the host name (DNS name/IP addr) from the url

INPUT/OUTPUT PARAMETERS:
  

RETURN VALUE:
  returns error code

SIDE EFFECTS:
  None.
======================================================================*/
URL::UrlErrCode URL::GetHost(char * host,int maxHostLen) const
{
  char *server_ip_ptr;
  char *server_port_ptr = NULL;
  char *ptr_to_first_slash_in_url = NULL;
  char *url_end;

  ResetErrorCode();

  if (IsEmpty())
  {
    errCode = URL_EMPTY;
    return errCode;
  }

  url_end = url + urlLen;

  server_ip_ptr = zrex_strstri(url, "//");
  if (server_ip_ptr == NULL) 
  {
    errCode = URL_BAD_FORMAT;
    return errCode;
  }
  server_ip_ptr += 2;

#if defined(FEATURE_QTV_IPV6) || defined(FEATURE_QTV_MCAST_IPV6)
#error code not present
#endif
  /* Locate the IP address. */
  {
    server_port_ptr = zrex_strstri(server_ip_ptr, ":");
  }

  ptr_to_first_slash_in_url = zrex_strstri(server_ip_ptr, "/");

  if ((NULL != server_port_ptr) && 
      (NULL != ptr_to_first_slash_in_url) &&
      (ptr_to_first_slash_in_url < server_port_ptr))
  {
    server_port_ptr = ptr_to_first_slash_in_url;
  }

  if (NULL == server_port_ptr)
  {
    //port not found so check for clip name start
    server_port_ptr = zrex_strstri(server_ip_ptr, "/");

    //we didn't find clip start, lets just return what we have
    if ((NULL == server_port_ptr) && (server_ip_ptr < url_end))
    {
      server_port_ptr = url_end;
    }

    if (NULL == server_port_ptr)
    {
      errCode = URL_BAD_FORMAT;
      return errCode;
    }
  }

  if (NULL != host)
  {
#if defined(FEATURE_QTV_IPV6) || defined(FEATURE_QTV_MCAST_IPV6)
#error code not present
#endif
    {
       int len = MIN(maxHostLen-1,server_port_ptr - server_ip_ptr + 1);
       std_strlcpy(host, server_ip_ptr, len);
    }
  }

  return errCode;
}

/* ======================================================================
FUNCTION:
  GetPort

DESCRIPTION:
  gets the port number from the url

INPUT/OUTPUT PARAMETERS:
  

RETURN VALUE:
  returns error code

SIDE EFFECTS:
  None.
======================================================================*/
URL::UrlErrCode URL::GetPort(uint32 * port) const
{
  char *server_ip_ptr, *clip_name;
  char *server_port_ptr = NULL;
  char port_str[PORT_STRING_SIZE];
  char *url_end;

  ResetErrorCode();

  if (NULL == port)
  {
    return errCode;
  }

  if (IsEmpty())
  {
    errCode = URL_EMPTY;
    return errCode;
  }

  url_end = url + urlLen;

  server_ip_ptr = zrex_strstri(url, "//");
  if (server_ip_ptr == NULL) 
  {
    errCode = URL_BAD_FORMAT;
    return URL_BAD_FORMAT;
  }

  server_ip_ptr += 2;

#if defined(FEATURE_QTV_IPV6) || defined(FEATURE_QTV_MCAST_IPV6)
#error code not present
#endif
    server_port_ptr = zrex_strstri(server_ip_ptr, ":");
#if defined(FEATURE_QTV_IPV6) || defined(FEATURE_QTV_MCAST_IPV6)
#error code not present
#endif

  clip_name = zrex_strstri(server_ip_ptr, "/");
  //in case port is present but no clip name start found
  if ((NULL == clip_name) && (server_port_ptr < url_end))
  {
    clip_name = url_end;
  }

  if (NULL == clip_name) 
  {
    errCode = URL_BAD_FORMAT;
    return URL_BAD_FORMAT;
  }

  if ((NULL == server_port_ptr) || (server_port_ptr > clip_name))
  {
    //port not found; return default port
    if (GetProtocol() == PROTO_RTSP || GetProtocol() == PROTO_RTSPT)
	{
	  *port = DEF_RTSP_PORT;
	  return URL_OK;
	}
	else if (GetProtocol() == PROTO_HTTP)
	{
      *port = DEF_FASTTRACK_PORT;
	  return URL_OK;
	}
    errCode = URL_UNKNOWN_PROTOCOL;
	return errCode;
  }

#if defined(FEATURE_QTV_IPV6) || defined(FEATURE_QTV_MCAST_IPV6)
#error code not present
#endif
  {
    server_port_ptr++;
  }

  ZUtils::StrcpyN(port_str, server_port_ptr, MIN(PORT_STRING_SIZE,((clip_name - server_port_ptr) + 1)));
  sscanf(port_str, "%d", port);

  return URL_OK;
}

/* ======================================================================
FUNCTION:
  GetClipName

DESCRIPTION:
  gets the clip name from the url

INPUT/OUTPUT PARAMETERS:
 1) clip_name
 2) sizeof clip_name 

RETURN VALUE:
  returns error code

SIDE EFFECTS:
  None.
======================================================================*/
URL::UrlErrCode URL::GetClipName(char * clip_name, int size) const
{
  char *server_ip_ptr, *clip_fname;
  
  ResetErrorCode();

  if (NULL == clip_name)
  {
    return errCode;
  }

  if (IsEmpty())
  {
    errCode = URL_EMPTY;
    return errCode;
  }

  server_ip_ptr = zrex_strstri(url, "//");
  if (server_ip_ptr == NULL) 
  {
    errCode = URL_BAD_FORMAT;
    return URL_BAD_FORMAT;
  }

  server_ip_ptr += 2;
  
  clip_fname = zrex_strstri(server_ip_ptr, "/");
  if (NULL == clip_fname) 
  {
    errCode = URL_BAD_FORMAT;
    return URL_BAD_FORMAT;
  }

  std_strlcpy(clip_name, clip_fname+1, size);

  return URL_OK;
}
  
/* ======================================================================
FUNCTION:
  IsHostLiteralAddress

DESCRIPTION:
  check whether host is literal address or ip address

INPUT/OUTPUT PARAMETERS:
  

RETURN VALUE:
  returns true or false

SIDE EFFECTS:
  None.
======================================================================*/
bool URL::IsHostLiteralAddress() const
{
  char ip_addr[HOSTNAME_STR_SIZE];

  if ((URL_OK == GetHost(ip_addr,sizeof(ip_addr))) && 
#if defined(FEATURE_QTV_IPV6) || defined(FEATURE_QTV_MCAST_IPV6)
#error code not present
#endif
      (inet_addr(ip_addr) == INADDR_NONE))
  {
    //literal address
    return true;
  }
  return false;
}

/* ======================================================================
FUNCTION:
  GetErrorCode

DESCRIPTION:
  Gets the error code of the recent error occured

INPUT/OUTPUT PARAMETERS:
  

RETURN VALUE:
  returns error code

SIDE EFFECTS:
  None.
======================================================================*/
URL::UrlErrCode URL::GetErrorCode() const
{
  return errCode;
}

/* ======================================================================
FUNCTION:
  ResetErrorCode

ACCESS: private 

DESCRIPTION:
  Reset the error code to Ok

INPUT/OUTPUT PARAMETERS:
  

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
======================================================================*/
void URL::ResetErrorCode() const
{
  errCode = URL_OK;
}

/* ======================================================================
FUNCTION:
  MapToPvErrorCode

DESCRIPTION:
  maps url error to pv error code

INPUT/OUTPUT PARAMETERS:
  

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
======================================================================*/

int32 URL::MapToPvErrorCode(URL::UrlErrCode errcode) const
{
  QtvStreamCode ecode = PV_STREAM_SUCCESS;

  //if arg not available, use member variable
  if (URL_NOT_DEFINED == errcode)
  {
    errcode = errCode;
  }

  switch(errcode)
  {
  case URL_BAD_FORMAT:
  case URL_UNKNOWN_PROTOCOL:
  case URL_MAX_LEN_EXCEEDS:
  case URL_EMPTY:
  case URL_BUFFER_NULL:
    ecode = PV_STREAM_INVALID_URL;
    break;
  case URL_OUT_OF_MEMORY:
    ecode = PV_STREAM_OUT_OF_MEMORY;
    break;
  case URL_ERROR:
    ecode = PV_STREAM_FAIL;
    break;
  case URL_OK:
    ecode = PV_STREAM_SUCCESS;
    break;
  default:
    ecode = PV_STREAM_FAIL;
    break;
  }

  return (int32)ecode;
}

#if defined(FEATURE_QTV_IPV6) || defined(FEATURE_QTV_MCAST_IPV6)
#error code not present
#endif 
