#ifndef AEEWEB_H
#define AEEWEB_H
/*======================================================

FILE:  AEEWeb.h

SERVICES:
        IWeb, IWebUtil

GENERAL DESCRIPTION:
        Web stuff.  Retrieval/transaction/utility APIs for anything
           that can be addressed by a URL.

=====================================================

Copyright © 1999-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
#include "AEE.h"
#include "AEESource.h"
#include "AEEError.h"
#include "AEEWebOpts.h"


/* ======================================================================= 
   Type Declarations
   ======================================================================= */

/*
  ||
  || WebStatus 
  || 
  ||  status callbacks come with an integer set to one of these values
  ||
*/
enum {
   WEBS_STARTING = 0,
   WEBS_CANCELLED,
   WEBS_GETHOSTBYNAME,
   WEBS_CONNECT,
   WEBS_SENDREQUEST,
   WEBS_READRESPONSE,
   WEBS_GOTREDIRECT,
   WEBS_CACHEHIT,
   WEBS_PROGRESS
};
#ifdef WIN32
typedef unsigned WebStatus;
#else
typedef int8 WebStatus;
#endif

/*
  || 
  || PFNWEBSTATUS
  ||   called when web state changes can be communicated, 
  ||   are meaningful
  ||
  || Arguments:
  ||
  ||   WebStatus ws:  one of the above enum cases
  ||
  ||   void *pData: status dependent extra data
  ||
*/
typedef void (*PFNWEBSTATUS)(void *pNotifyData, WebStatus ws, void *pData);


/*
  || 
  || PFNWEBHEADER 
  ||    will be called when web headers are received, this is
  ||    orthogonal to headers inline
  ||
  || Arguments:
  ||
  ||   const char *pszName:  parsed out header name (null-terminated)
  ||
  ||   GetLine *pglVal:  header value (rest of line), might be truncated, 
  ||                       continued..
  ||
*/
typedef void (*PFNWEBHEADER)(void *pNotifyData, const char *pszName, 
                             GetLine *pglVal);




/*
  ||
  || WebOpt nOptIds
  ||
  || WebOpt ids fall in ranges that are designed to keep the pVal data opaque 
  ||  while helping IWeb deal with copying the data if necessary.  
  ||
  || During a Web transaction the data in options passed to GetResponse 
  ||  are not copied by default, they must stay valid until the GetResponse 
  ||  callback fires.  Data passed as options to IWeb *are* copied by default,
  ||  so it needn't be kept around, unless you specifically turn off 
  ||  COPYOPTS on the IWeb.
  ||
*/

/* 
   || Following WebOpts are null-terminated strings, valid ids up to 0x01ffff
*/
#define WEBOPT_METHOD         (WEBOPT_SZ+1) /* (char *) defaults to "GET" */
#define WEBOPT_HEADER         (WEBOPT_SZ+2) /* (char *) CRLF-separated, 
                                               name-value pairs like this: 
                                               "Name1: val\r\nName2: val\r\n", 
                                               defaults to NULL */
#define WEBOPT_EXTRAHEADERS   WEBOPT_HEADER /* deprecated, use WEBOPT_HEADER */
#define WEBOPT_PROXYSPEC      (WEBOPT_SZ+3) /* (char *) proxyspec format */
#define WEBOPT_AUTHBASIC      (WEBOPT_SZ+4) /* (char *) send basic auth 
                                               [ unimplemented ] */
#define WEBOPT_USERAGENT      (WEBOPT_SZ+5) /* (char *) user-agent string, if 
                                               unset, IWeb sends a default 
                                               User-Agent header, if set to 
                                               NULL, no user-agent is sent */

/*
  || Following two WebOpts are introduced by BREW 3.1.5, as a way to override
  || startline and Host headers for applications need to use CONNECT with a
  || proxy.  They need to be used in conjuntion with each other and
  || WEBOPT_METHOD specified as "CONNECT".   For other methods, and for CONNECT
  || to an HTTP(S) server, these webopts are not needed as the default value
  || will be provided correctly by the IWEB library.
*/
#define WEBOPT_REQUESTURI     (WEBOPT_SZ+6) /* (char *) request URI string, if 
                                               unset, IWeb sends a default 
                                               URI based on the URL */

#define WEBOPT_REQUESTHOST    (WEBOPT_SZ+7) /* (char *) HOST header string, if 
                                               unset, IWeb sends the default 
                                               parsed from the URL */

/*
  || Following WebOpts are 32-bit quantities, no need to copy, valid ids up to 
  ||  0x02ffff
*/
#define WEBOPT_FLAGS            (WEBOPT_32BIT+1) /* (unsigned int) default 0 */
#define WEBOPT_HANDLERDATA      (WEBOPT_32BIT+2) /* (void *) default NULL */
#define WEBOPT_STATUSHANDLER    (WEBOPT_32BIT+3) /* (PFNWEBSTATUS) default 
                                                    NULL */
#define WEBOPT_HEADERHANDLER    (WEBOPT_32BIT+4) /* (PFNWEBHEADER) default 
                                                    NULL */
#define WEBOPT_CONNECTTIMEOUT   (WEBOPT_32BIT+5) /* (unsigned int) in milli-
                                                    seconds, defaults to 
                                                    AEENet connect() timeout, 
                                                    also failover timeout, 
                                                    0 means use system default,
                                                    -1 means infinite */
#define WEBOPT_CONTENTLENGTH    (WEBOPT_32BIT+6) /* (long) specifies size of
                                                    request (in bytes) of 
                                                    message body */
#define WEBOPT_IDLECONNTIMEOUT  (WEBOPT_32BIT+7) /* (int) time in *seconds* 
                                                    before I close idle web 
                                                    connections, default is 
                                                    INETMgr PPP linger, -1 is 
                                                    infinite */
#define WEBOPT_ACTIVEXACTIONS   (WEBOPT_32BIT+8) /* (unsigned int) scalar, 
                                                    maximum number of "active"
                                                    web transactions allowed.
                                                    IWeb enforces this by 
                                                    queueing transactions 
                                                    started when the maximum
                                                    number are already 
                                                    "active", meaning handed 
                                                    to the correct protocol 
                                                    handler */
#define WEBOPT_NETWORK          (WEBOPT_32BIT+9) /* AEE_NETWORK_*  (see 
                                                    INETMGR_SetNetwork() for
                                                    more details) */


/*
  || Following WebOpts are pointers to interfaces, which are AddRef'd to copy
  || valid ids up to 0x03ffff
*/
#define WEBOPT_BODY           (WEBOPT_INTERFACE+1)

/* CAUTION: Do not use (WEBOPT_INTERFACE+2) as it is already taken by as WEBOPT_DEFAULTS */



/*
  || WebRequest flags: OR'd these together and pass as option with option id
  ||                      WEBOPT_FLAGS to get the below behaviour
*/
#define WEBREQUEST_REDIRECT       0x0001  /* handle redirection invisibly, if 
                                             possible [ unimplemented ] */
#define WEBREQUEST_NOCOOKIES      0x0002  /* do not process cookies; pass 
                                             headers through to caller 
                                             [ unimplemented ] */
#define WEBREQUEST_NOCACHE        0x0004  /* send "Pragma: no-cache" to proxy 
                                             (if proxying) [ unimplemented ] */
#define WEBREQUEST_NOKEEPALIVE    0x0008  /* disable keep-alives; force basic 
                                             HTTP/1.0 */
#define WEBREQUEST_FORCENEWCONN   0x0010  /* force a new connection */
#define WEBREQUEST_NOWAITCONN     0x0020  /* don't wait for reusable 
                                            connection, force new */
#define WEBREQUEST_HTTPBOGUSCRLF  0x0100  /* append an extra CRLF to HTTP request
                                             bodies to work around a CERN webserver bug (see here:
                                             http://httpd.apache.org/docs/misc/known_client_problems.html#trailing-crlf) */
#define WEBREQUEST_FORCEHTTP10	  0x200	   /* Force content length requirement on post; force basic HTTP/1.0*/
/*
  || IWebResponse flags (use IWEBRESP_GetOpt(WEBOPT_FLAGS))
*/
#define WEBRESPONSE_REDIRECTED        0x0001   /* redirection was handled by
                                                  IWeb [ unimplemented ] */
#define WEBRESPONSE_COOKIES           0x0002   /* cookies were sent in the 
                                                  request [ unimplemented ] */
#define WEBRESPONSE_CACHED            0x0004   /* retrieved from local cache 
                                                  (no network activity) 
                                                  [ unimplemented ] */
#define WEBRESPONSE_KEEPALIVE         0x0008   /* connection kept alive */
#define WEBRESPONSE_HTTP09            0x0010   /* got an HTTP0.9 response 
                                                  (no headers; 200 response 
                                                  fabricated) */
#define WEBRESPONSE_LOCAL             0x0100   /* locally-generated content 
                                                  (no network activity) */
#define WEBRESPONSE_PROXIED           0x0200   /* gotten through a proxy */
#define WEBRESPONSE_AUTHBASIC         0x1000   /* basic authentication info 
                                                  was sent [ unimplemented ] */

/* 
   We adopt HTTP1.1's codes for IWeb implementations, these predefined
   constants can be used to interpret a WebRespInfo.nCode. See rfc2626 for
   for more information.
   
   When WEB_ERROR_MAP() evaluates to WEB_ERROR_PROTOCOL, consult these.
*/

/*  Informational 1xx ... */
#define WEBCODE_CONTINUE                        100
#define WEBCODE_SWITCHINGPROTOCOLS              101
/* Successful 2xx ... */
#define WEBCODE_OK                              200
#define WEBCODE_CREATED                         201
#define WEBCODE_ACCEPTED                        202
#define WEBCODE_NONAUTHORITATIVEINFORMATION     203
#define WEBCODE_NOCONTENT                       204
#define WEBCODE_RESETCONTENT                    205
#define WEBCODE_PARTIALCONTENT                  206
/* Redirection 3xx ... */
#define WEBCODE_MULTIPLECHOICES                 300
#define WEBCODE_MOVEDPERMANENTLY                301
#define WEBCODE_FOUND                           302
#define WEBCODE_SEEOTHER                        303
#define WEBCODE_NOTMODIFIED                     304
#define WEBCODE_USEPROXY                        305
#define WEBCODE_UNUSED                          306
#define WEBCODE_TEMPORARYREDIRECT               307
/* Client Error 4xx ... */
#define WEBCODE_BADREQUEST                      400
#define WEBCODE_UNAUTHORIZED                    401
#define WEBCODE_PAYMENTREQUIRED                 402
#define WEBCODE_FORBIDDEN                       403
#define WEBCODE_NOTFOUND                        404
#define WEBCODE_METHODNOTALLOWED                405
#define WEBCODE_NOTACCEPTABLE                   406
#define WEBCODE_PROXYAUTHENTICATIONREQUIRED     407
#define WEBCODE_REQUESTTIMEOUT                  408
#define WEBCODE_CONFLICT                        409
#define WEBCODE_GONE                            410
#define WEBCODE_LENGTHREQUIRED                  411
#define WEBCODE_PRECONDITIONFAILED              412
#define WEBCODE_REQUESTENTITYTOOLARGE           413
#define WEBCODE_REQUESTURITOOLONG               414
#define WEBCODE_UNSUPPORTEDMEDIATYPE            415
#define WEBCODE_REQUESTEDRANGENOTSATISFIABLE    416
#define WEBCODE_EXPECTATIONFAILED               417
/* Server Error 5xx ... */
#define WEBCODE_INTERNALSERVERERROR             500
#define WEBCODE_NOTIMPLEMENTED                  501
#define WEBCODE_BADGATEWAY                      502
#define WEBCODE_SERVICEUNAVAILABLE              503
#define WEBCODE_GATEWAYTIMEOUT                  504
#define WEBCODE_PROTOCOLVERSIONNOTSUPPORTED     505


typedef struct WebRespInfo
{
   int         nCode;           /* either protocol code, or negative 
                                   from AEEError.h */
   ISource    *pisMessage;      /* source interface pointer to the body and 
                                   maybe headers */
   
   /* follows meta info that may increase */
   long        lContentLength;  /* length of *BODY* in bytes, -1 if unknown */
   const char *cpszContentType; /* mime type, NULL if unknown */
   const char *cpszCharset;     /* charset desc, NULL if unknown 
                                   [ unimplemented ], see cpszContentType */
   
   int32       tExpires;        /* doc expiry, -1 if unknown */
   int32       tModified;       /* doc modded, -1 if unknown */
} WebRespInfo;

#define WEBRESPINFO_CTOR(pwri) do { \
                                   (pwri)->nCode           = WEB_ERROR_BASE;\
                                   (pwri)->pisMessage      = 0;\
                                   (pwri)->lContentLength  = -1;\
                                   (pwri)->cpszContentType = 0;\
                                   (pwri)->cpszCharset     = 0;\
                                   (pwri)->tExpires        = -1;\
                                   (pwri)->tModified       = -1;\
                               } while (0)


#define WEB_ERROR_SUCCEEDED(nCode) (((nCode) >= 200) && ((nCode) < 300))
#define WEB_ERROR_MAP(nCode) (((nCode) < 0) ? -(nCode) : (WEB_ERROR_SUCCEEDED((nCode)) ? SUCCESS : WEB_ERROR_PROTOCOL))

/*
  deprecated
 */
#define WEBRESP_Succeeded(nCode) WEB_ERROR_SUCCEEDED(nCode)

/* 
   ||
   || IWebResp encapsulates the response side of an IWeb transaction.
   ||   It assumes responsibilty for resources allocated to express a request.
   ||
   || By default, a response only expresses the body, use SetOpt to get the
   ||   headers (inline and/or via callback)
   ||
*/
typedef struct IWebResp IWebResp;
AEEINTERFACE(IWebResp) {
   INHERIT_IWebOpts(IWebResp);
   WebRespInfo *(*GetInfo)(IWebResp *);
};

#define IWEBRESP_AddRef(p)               AEEGETPVTBL((p),IWebResp)->AddRef((p))
#define IWEBRESP_Release(p)              AEEGETPVTBL((p),IWebResp)->Release((p))
#define IWEBRESP_QueryInterface(p,i,ppo) AEEGETPVTBL((p),IWebResp)->QueryInterface((p),(i),(ppo))
#define IWEBRESP_AddOpt(p,o)             AEEGETPVTBL((p),IWebResp)->AddOpt((p),(o))
#define IWEBRESP_RemoveOpt(p,i,n)        AEEGETPVTBL((p),IWebResp)->RemoveOpt((p),(i),(n))
#define IWEBRESP_GetOpt(p,i,n,o)         AEEGETPVTBL((p),IWebResp)->GetOpt((p),(i),(n),(o))
#define IWEBRESP_GetInfo(p)              AEEGETPVTBL((p),IWebResp)->GetInfo((p))


/*
  ||
  || IWeb 
  || 
  ||  This non-static interface dictates how web transations will be handled,
  ||    and provides a method for kicking off those transacions.
  ||
*/
typedef struct IWeb IWeb;
#define INHERIT_IWeb(iname) \
   INHERIT_IWebOpts(iname);\
   void  (*GetResponse) (iname *po, IWebResp **ppiwresp, \
                         AEECallback *pcb, const char *cpszUrl, ...);\
   void  (*GetResponseV)(iname *po, IWebResp **ppiwresp, \
                         AEECallback *pcb, const char *cpszUrl, \
                         WebOpt *awozList)

AEEINTERFACE(IWeb) {
   INHERIT_IWeb(IWeb);
};

#define IWEB_AddRef(p)                 AEEGETPVTBL((p),IWeb)->AddRef((p))
#define IWEB_Release(p)                AEEGETPVTBL((p),IWeb)->Release((p))
#define IWEB_QueryInterface(p,i,o)     AEEGETPVTBL((p),IWeb)->QueryInterface((p),(i),(o))
#define IWEB_AddOpt(p,o)               AEEGETPVTBL((p),IWeb)->AddOpt((p),(o))
#define IWEB_RemoveOpt(p,i,n)          AEEGETPVTBL((p),IWeb)->RemoveOpt((p),(i),(n))
#define IWEB_GetOpt(p,i,n,o)           AEEGETPVTBL((p),IWeb)->GetOpt((p),(i),(n),(o))
#define IWEB_GetResponse(p,args)       AEEGETPVTBL((p),IWeb)->GetResponse args
#define IWEB_GetResponseV(p,r,cb,u,ol) AEEGETPVTBL((p),IWeb)->GetResponseV((p),(r),(cb),(u),(ol))


/*
 || For URL parsing, UrlParts is a set of const char * pointers
 ||    into a null-terminated URL string.  If a field is present,
 ||    its value is different from the next field's. Some
 ||    fields include delimiters, as documented below.
 ||
 || The UP_ macros below should help make exploring a URL easier.
 || 
 ||  Example 1:
 ||  
 ||    cpszUrl = "http://user:pass@host:port/path/doc.htm?srch#name"
 ||    
 ||    cpcSchm = "http://use...
 ||    cpcAuth = "//user:pas...
 ||    cpcUser = "user:pass@...
 ||    cpcPass = ":pass@host...
 ||    cpcHost = "host:port/...
 ||    cpcPort = ":port/path...
 ||    cpcPath = "/path/doc....
 ||    cpcSrch = "?srch#name...
 ||    cpcName = "#name"
 ||    cpcEnd  = ""
 ||
 ||  Example 2: 
 ||
 ||    cpszUrl = "http://user@host/path#name
 ||    
 ||    cpcSchm = "http://use...
 ||    cpcAuth = "//user@hos...
 ||    cpcUser = "user@host/...
 ||    cpcPass = "host/path#...
 ||    cpcHost = "host/path#...
 ||    cpcPort = "/path#name"
 ||    cpcPath = "/path#name"
 ||    cpcSrch = "#name"
 ||    cpcName = "#name"
 ||    cpcEnd  = ""
 ||
 ||  Example 3: 
 ||
 ||    cpszUrl = "mailto:brewery@qualcomm.com
 ||    
 ||    cpcSchm = "mailto:bre...
 ||    cpcAuth = "brewery@qu...
 ||    cpcUser = "brewery@qu...
 ||    cpcPass = "brewery@qu...
 ||    cpcHost = "brewery@qu...
 ||    cpcPort = "brewery@qu...
 ||    cpcPath = "brewery@qu...
 ||    cpcSrch = ""
 ||    cpcName = ""
 ||    cpcEnd  = ""
 ||
*/
typedef struct UrlParts { 
   const char *cpcSchm; /* always the beginning */
   const char *cpcAuth; /* at "//..." */
   const char *cpcUser; /* at "user:pwd@host:port/..." */
   const char *cpcPass; /* at ":pwd@host:port/..." */
   const char *cpcHost; /* at "host:port/..." */
   const char *cpcPort; /* at ":port/..." */
   const char *cpcPath; /* at '/', if present  */
   const char *cpcSrch; /* at '?', if present */
   const char *cpcName; /* at '#', if present */
   const char *cpcEnd;  /* at '\0' */
} UrlParts;

#define UP_PARTLEN(p) ((&(p))[1] - (p)) /* cute but dangerous */

#define UP_HASSCHM(pup)   ((pup)->cpcAuth != (pup)->cpcSchm)
#define UP_SCHMLEN(pup)   ((pup)->cpcAuth - (pup)->cpcSchm)

#define UP_HASAUTH(pup)   ((pup)->cpcAuth != (pup)->cpcPath)
#define UP_AUTHLEN(pup)   ((pup)->cpcPath - (pup)->cpcAuth)

#define UP_HASHOST(pup)   ((pup)->cpcPort != (pup)->cpcHost)
#define UP_HOSTLEN(pup)   ((pup)->cpcPort - (pup)->cpcHost)

#define UP_HASPORT(pup)   ((pup)->cpcPath != (pup)->cpcPort)
#define UP_PORTLEN(pup)   ((pup)->cpcPath - (pup)->cpcPort)

#define UP_HASUSER(pup)   ((pup)->cpcUser != (pup)->cpcHost)
#define UP_HASPASS(pup)   ((pup)->cpcPass != (pup)->cpcHost)

#define UP_HASPATH(pup)   ((pup)->cpcSrch != (pup)->cpcPath)
#define UP_PATHLEN(pup)   ((pup)->cpcSrch - (pup)->cpcPath)

#define UP_HASSRCH(pup)   ((pup)->cpcName != (pup)->cpcSrch)
#define UP_SRCHLEN(pup)   ((pup)->cpcName - (pup)->cpcSrch)

#define UP_HASNAME(pup)   ((pup)->cpcEnd != (pup)->cpcName)
#define UP_NAMELEN(pup)   ((pup)->cpcEnd - (pup)->cpcName)

#define UP_URLLEN(pup)    ((pup)->cpcEnd - (pup)->cpcSchm)


/*
  || IWEBUTIL_ParseFormFields() output parameter, name value pair
*/
typedef struct WebFormField
{
   char *pcName;
   char *pcValue;

} WebFormField;

/*
  || IWEBUTIL_ParseFormField flags 
*/

#define WEBUTIL_PFFF_NODECODE 0x01  /* don't URL decode individual fields */
#define WEBUTIL_PFFF_NOMUTATE 0x03  /* don't modify input, implies NODECODE */


/*
  ||
  || handy-dandy util parsing stuff (date stuff missing)
  ||
*/
typedef struct IWebUtil IWebUtil;
AEEINTERFACE(IWebUtil) {
   INHERIT_IQueryInterface(IWebUtil);

   int    (*ParseUrl)      (IWebUtil *po, const char *cpszUrl, 
                            UrlParts *pwup);
   int    (*MakeUrl)       (IWebUtil *po, const char *cpszBaseUrl, 
                            const char *cpszRelativeUrl, 
                            char *pResult, int nResultLen);
   char  *(*UrlEncode)     (IWebUtil *po, const char *cpcIn, int *pnInLen,
                            char *pcOut, int *pnOutLen);
   char  *(*UrlDecode)     (IWebUtil *po, const char *cpcIn, int *pnInLen,
                            char *pcOut, int *pnOutLen);
   char  *(*EncodeBase64)  (IWebUtil *po, const char *cpcIn, int nInLen, 
                            char *pszOut);
   int    (*DecodeBase64)  (IWebUtil *po, const char *cpszIn, char *pcOut);
   int    (*ParseFormFields)(IWebUtil *po, char **ppszIn, 
                             WebFormField *awff, int nMaxFields,
                             uint32 uFlags);
};

#define IWEBUTIL_AddRef(p)                  AEEGETPVTBL((p),IWebUtil)->AddRef((p))
#define IWEBUTIL_Release(p)                 AEEGETPVTBL((p),IWebUtil)->Release((p))
#define IWEBUTIL_QueryInterface(p,i,ppo)    AEEGETPVTBL((p),IWebUtil)->QueryInterface((p),(i),(ppo))
#define IWEBUTIL_ParseUrl(p,u,up)           AEEGETPVTBL((p),IWebUtil)->ParseUrl((p),(u),(up))
#define IWEBUTIL_MakeUrl(p,b,r,res,len)     AEEGETPVTBL((p),IWebUtil)->MakeUrl((p),(b),(r),(res),(len))
#define IWEBUTIL_UrlEncode(p,i,il,o,ol)     AEEGETPVTBL((p),IWebUtil)->UrlEncode((p),(i),(il),(o),(ol))
#define IWEBUTIL_UrlDecode(p,i,il,o,ol)     AEEGETPVTBL((p),IWebUtil)->UrlDecode((p),(i),(il),(o),(ol))
#define IWEBUTIL_EncodeBase64(p,i,il,o)     AEEGETPVTBL((p),IWebUtil)->EncodeBase64((p),(i),(il),(o))
#define IWEBUTIL_DecodeBase64(p,i,o)        AEEGETPVTBL((p),IWebUtil)->DecodeBase64((p),(i),(o))
#define IWEBUTIL_ParseFormFields(p,i,f,n,g) AEEGETPVTBL((p),IWebUtil)->ParseFormFields((p),(i),(f),(n),(g))


#define BASE64LEN(x) (((((x) + 2) / 3) * 4) + 1)


/* 
   ||
   || IWebReq encapsulates the request side of an IWeb transaction.  It is here
   ||   to assume responsibilty for resources allocated to express a request.
   ||
*/
typedef struct IWebReq IWebReq;
AEEINTERFACE(IWebReq) {
   INHERIT_IWebOpts(IWebReq);
   const char *(*GetUrl)(IWebReq *po);
};

#define IWEBREQ_AddRef(p)               AEEGETPVTBL((p),IWebReq)->AddRef((p))
#define IWEBREQ_Release(p)              AEEGETPVTBL((p),IWebReq)->Release((p))
#define IWEBREQ_QueryInterface(p,i,ppo) AEEGETPVTBL((p),IWebReq)->QueryInterface((p),(i),(ppo))
#define IWEBREQ_AddOpt(p,o)             AEEGETPVTBL((p),IWebReq)->AddOpt((p),(o))
#define IWEBREQ_RemoveOpt(p,i,n)        AEEGETPVTBL((p),IWebReq)->RemoveOpt((p),(i),(n))
#define IWEBREQ_GetOpt(p,i,n,o)         AEEGETPVTBL((p),IWebReq)->GetOpt((p),(i),(n),(o))
#define IWEBREQ_GetUrl(p)               AEEGETPVTBL((p),IWebReq)->GetUrl((p))



/* 
   ||
   || IWebEng is a BREW extension that registers with the Shell to 
   ||   to handle certain kinds of URLs, keyed by scheme, like "http:"
   ||
*/
typedef struct IWebEng IWebEng;
AEEINTERFACE(IWebEng) {
   INHERIT_IQueryInterface(IWebEng);
   int (*Transaction)(IWebEng *piwe, IWeb *piw, IWebReq *piwreq, 
                      AEECallback *pcb, IWebResp **ppiwresp);
};

#define IWEBENG_AddRef(p)               AEEGETPVTBL((p),IWebEng)->AddRef((p))
#define IWEBENG_Release(p)              AEEGETPVTBL((p),IWebEng)->Release((p))
#define IWEBENG_QueryInterface(p,i,ppo) AEEGETPVTBL((p),IWebEng)->QueryInterface((p),(i),(ppo))
#define IWEBENG_Transaction(p,w,q,cb,a) AEEGETPVTBL((p),IWebEng)->Transaction((p),(w),(q),(cb),(a))



/*
=======================================================================
  DATA STRUCTURES DOCUMENTATION
=======================================================================

WebStatus

Description:
   Enumerated type passed to a WebStatus callback (installed using
   WEBOPT_STATUSHANDLER) to inform the API client of the current
   status of an in-progress Web request.  These are pre-defined
   status events which describe the major phases of a Web request
   transaction.

Definition:
   typedef enum WebStatus
   {
      WEBS_STARTING = 0,
      WEBS_CANCELLED,
      WEBS_GETHOSTBYNAME,
      WEBS_CONNECT,
      WEBS_SENDREQUEST,
      WEBS_READRESPONSE,
      WEBS_GOTREDIRECT,
      WEBS_CACHEHIT,
      WEBS_PROGRESS
   } WebStatus;


Members:
   WEBS_STARTING -- initial state.
   WEBS_CANCELLED -- request has been cancelled by CALLBACK_Cancel.
   WEBS_GETHOSTBYNAME -- looking up hostname in URL.
   WEBS_CONNECT -- attempting to connect to host.
   WEBS_SENDREQUEST -- sending request data to remote server.
   WEBS_READRESPONSE -- receiving response data from remote server.
   WEBS_GOTREDIRECT -- handling HTTP redirect.
   WEBS_CACHEHIT -- requested document has been found in cache.
   WEBS_PROGRESS -- check pData; it is set to a number 
                    between 0 and 100 that indicates the
                    percentage of the transaction that has been
                    completed.

Comments:
   It is not intended that applications use these WebStatus codes
   for any other purpose than displaying a status summary through
   the UI or through debug messages, etc.  Web engines might
   not implement all of these status codes, nor is there any
   requirement that web transactions proceed through the status
   codes in any particular order.  Making programmatic decisions
   based on the WebStatus codes is strongly discouraged.

See Also:
   None

=======================================================================

PFNWEBSTATUS

Description:
   This is a prototype of callback for web request status, passed as WEBOPT_STATUSHANDLER
pVal to either IWEB_AddOpt() or to IWEB_GetResponse().

Definition:
   typedef void (*PFNWEBSTATUS)(void *pNotifyData, WebStatus ws, 
                                void *pData);

Members:

  void *pNotifyData: user-supplied callback context, use WEBOPT_HANDLERDATA 
                     to specify

  WebStatus ws: the status being communicated

  void *pData: extra data, sometimes holds more information about 
                the status of the transaction, see WebStatus type for 
                more information

Comments:
  PFNWEBSTATUS and PFNWEBHEADER types use the same pNotifyData value

See Also:
  None

======================================================================= 
PFNWEBHEADER

Description:
   Prototype of callback for web headers.  If specified this function is 
   called whenever IWeb has parsed a header from the response.  Passed as 
   WEBOPT_HEADERHANDLER pVal to either IWEB_AddOpt() or to 
   IWEB_GetResponse()

Definition:
  typedef void (*PFNWEBHEADER)(void *pNotifyData, const char *pszName, 
                               GetLine *pglVal);


Members:
  pNotifyData: user-supplied callback context, use WEBOPT_HANDLERDATA to specify

  pszName: parsed-out, null-terminated header name without colon, e.g. "Content-Length"
                  
  pglVal: the value of the header, see AEESource.h for definition of GetLine

Comments:
   Because a line may not be able to fit into IWeb's internal buffers, pplVal may
be a truncated line or characters leftover from another line. When this happens,
the header callback is activated for each packet of header received. The first callback
has pszName set to the name of the header; any continuation callbacks have pszName
set to NULL.

See Also:
   GetLine
   WebOpt
   PFNWEBSTATUS

======================================================================= 
WebRespInfo

Description:
   WebRespInfo is designed to simplify coding of the most basic web 
   transactions, and so holds the most salient, commonly important
   pieces of info about how a web transaction went.

Definition:

  typedef struct 
  {
     int         nCode;           
                                  
     ISource    *pisMessage;      
                                  
                                  
     long        lContentLength;  
     const char *cpszContentType;
     const char *cpszCharset; 
                                  
     int32       tExpires;     
     int32       tModified;
  } WebRespInfo;

Members:

  nCode: either a protocol error code or a *negative* WEB_ERROR (see 
         AEEError.h for codes), use WEB_ERROR_MAP(nCode) to map to 
         AEE error, and WEB_ERROR_SUCCEEDED(nCode) to test for success.
         A positive number is one that was returned by the server, 
         negative error codes are system errors.
  
  pisMessage: a stream representing the body of the response
  
  lContentLength: length of the body stream, if known (-1 if unknown.
                  It is unnecessary to limit your reading from pisMessage, 
                  IWeb takes care of EOF being correct
                       
  cpszContentType: mime-type of the message, if known (NULL if unknown)
  
  cpszCharset: character-set of the message, if known (NULL if unknown)
  
  tExpires: document expiry date, if known (-1 if unknown)
        
  tModified: document modification date, if known (-1 if unknown)       

Comments:
   The memory pointed to by cpszContentType and cpszCharset is valid
   only until IWEBRESP_Release() is called. Similarly, pisMessage is
   released when the IWebResp interface is released. If you want to
   read pisMessage after the IWebResp interface is released, you must call
   ISOURCE_AddRef() on pisMessage before calling IWEBRESP_Release() on
   the IWebResp.

See Also:
   WEB_ERROR_SUCCEEDED

======================================================================= 
UrlParts

Description:
   a set of string pointers into a URL string, parsed to the most commonly
   useful parts of a URL.  Constructed by IWEBUTIL_ParseUrl(), only works
   for null-terminated strings.

Definition:
   typedef struct UrlParts { 
      const char *cpcSchm;
      const char *cpcAuth;
      const char *cpcUser;
      const char *cpcPass;
      const char *cpcHost;
      const char *cpcPort;
      const char *cpcPath;
      const char *cpcSrch;
      const char *cpcName;
      const char *cpcEnd; 
   } UrlParts;

Members:
   const char *cpcSchm: always points at the beginning of the string

   const char *cpcAuth: points at "//..." part of URL, if present

   const char *cpcUser: points at at "user:pwd@host:port/...", if present

   const char *cpcPass: points at ":pwd@host:port/..."

   const char *cpcHost: points at "host:port/..."

   const char *cpcPort: points at ":port/..."

   const char *cpcPath: points at '/', if present

   const char *cpcSrch: points at '?', if present

   const char *cpcName: points at '#', if present

   const char *cpcEnd;  points at '\0'

Comments:
   If a field is *not* present, its value is the same as the next member's.

   For example, in the URL:
 
    mailto:brew@qualcomm.com

   since there's no "//", cpcAuth will be set to cpcUser (the next member), 
   and cpcUser will point to "brew@..."
   
   These UP_ macros below should help make exploring a URL easier.

     UP_HASSCHM(pup)   TRUE if the URL specifies a scheme (has a colon before
                        the first slash, but not at position 0
     UP_SCHMLEN(pup)   length of the scheme, including terminating ':'
     
     UP_HASAUTH(pup)   TRUE if the URL has an authority part, which includes
                            user, pass, host and port, if applicable
     UP_AUTHLEN(pup)   length of authority part (including leading "//")
     
     UP_HASHOST(pup)   TRUE if the URL has a host part
     UP_HOSTLEN(pup)   length of the host part
     
     UP_HASPORT(pup)   TRUE if the URL has a port part (on the host)
     UP_PORTLEN(pup)   length of the port part (including ':')
     
     UP_HASUSER(pup)   TRUE if the URL specifies a user 

     UP_HASPASS(pup)   TRUE if the URL specifies a password, a password
                           (includes leading ':' and trailing '@')
     
     UP_HASPATH(pup)   TRUE if the URL has a path part (first '/' after host, 
                            if any)
     UP_PATHLEN(pup)   length of the path part (including the '/')
     
     UP_HASSRCH(pup)   TRUE if the URL has a search part (stuff after '?')
     UP_SRCHLEN(pup)   length of the search part (including the '?')
     
     UP_HASNAME(pup)   TRUE if the URL has a named position
     UP_NAMELEN(pup)   length of named position part (including the '#')
     
     UP_URLLEN(pup)    length of entire URL (not including null-terminator)

   Some additional examples:
   
    Example 1:
    
      cpszUrl = "http://user:pass@host:port/path/doc.htm?srch#name"
      
      cpcSchm = "http://use...
      cpcAuth = "//user:pas...
      cpcUser = "user:pass@...
      cpcPass = ":pass@host...
      cpcHost = "host:port/...
      cpcPort = ":port/path...
      cpcPath = "/path/doc....
      cpcSrch = "?srch#name...
      cpcName = "#name"
      cpcEnd  = ""
   
    Example 2: 
   
      cpszUrl = "http://user@host/path#name
      
      cpcSchm = "http://use...
      cpcAuth = "//user@hos...
      cpcUser = "user@host/...
      cpcPass = "host/path#...
      cpcHost = "host/path#...
      cpcPort = "/path#name"
      cpcPath = "/path#name"
      cpcSrch = "#name"
      cpcName = "#name"
      cpcEnd  = ""
   
    Example 3: 
   
      cpszUrl = "mailto:brewery@qualcomm.com
      
      cpcSchm = "mailto:bre...
      cpcAuth = "brewery@qu...
      cpcUser = "brewery@qu...
      cpcPass = "brewery@qu...
      cpcHost = "brewery@qu...
      cpcPort = "brewery@qu...
      cpcPath = "brewery@qu...
      cpcSrch = ""
      cpcName = ""
      cpcEnd  = ""

See Also:
   None

=======================================================================
WebFormField

Description:
   This data type is typically a NULL terminated strings representing name and
value of form fields.

Definition:
   typedef struct WebFormField
   {
      char  *pcName;
      char  *pcValue;
   } WebFormField;

Members:
   pcName: pointer to the beginning of the name

   pcValue: pointer to the beginning of the value

Comments:
   if WEBUTIL_PFFF_NOMUTATE is passed to IWEBUTIL_ParseFormField, the pointers
   in the resulting WebFormField will point strings that overlap.
   For example: given the string "x=y", pcName will point to "x=y" and 
   pcValue will point to "y".

See Also:
   IWEBUTIL_ParseFormFields()

=======================================================================

*/

/*
=======================================================================
   MACROS DOCUMENTATION
=======================================================================

WEB_ERROR_SUCCEEDED()

Description: 
  determine if a WebRespInfo error code means "success"

Definition:

  WEB_ERROR_SUCCEEDED(nCode) (((nCode) >= 200) && ((nCode) < 300))


Parameters:
   nCode: error code from WebRespInfo

Evaluation Value:
   TRUE if nCode is a successful error value
   FALSE otherwise

Comments:  
   None

Side Effects: 
   None

See Also:
   WebRespInfo

=======================================================================

WEB_ERROR_MAP()

Description: 
  map a WebRespInfo error to the AEEError.h space

Definition:

   WEB_ERROR_MAP(nCode) (((nCode) < 0) ? -(nCode) : (WEB_ERROR_SUCCEEDED((nCode)) ? SUCCESS : WEB_ERROR_PROTOCOL))

Parameters:
   nCode: error code from WebRespInfo

Evaluation Value:
   an error code from AEEError.h

Comments:  
   This kinda tosses information (like the protocol error code), but is 
    useful for APIs returning straight AEE errors to their clients

Side Effects: 
   None

See Also:
   WebRespInfo, WEB_ERROR_SUCCEEDED

======================================================================= 

*/

/*
======================================================================= 
   INTERFACES DOCUMENTATION
======================================================================= 

IWebResp Interface

Description: 

   The IWebResp interface provides the resources for a Web
   transaction, the response side.  This interface represents the
   answer to an IWEB_GetResponse() question.  A new one is created and
   returned (stuffed into a pointer) upon completion of a web
   transaction.  IWebResp runs header callbacks, provides the message
   body, and tells extra information about the web transaction via
   IWebOpts.

Header files:
  AEEWeb.h

See Also:
   IWebOpts, IWeb

======================================================================= 
IWEBRESP_AddRef()

Description:
This function is inherited from IBASE_AddRef()
======================================================================= 
IWEBRESP_Release()

Description:
This function is inherited from IBASE_AddRef.
======================================================================= 
IWEBRESP_QueryInterface()

Description:
This function is inherited from IQI_QueryInterface().
======================================================================= 
IWEBRESP_GetOpt()

Description:
This function is inherited from IWEBOPTS_GetOpt().
======================================================================= 
IWEBRESP_AddOpt()

Description:
This function is inherited from IWEBOPTS_AddOpt().
======================================================================= 
IWEBRESP_RemoveOpt()

Description:
This function is inherited from IWEBOPTS_RemoveOpt().
======================================================================= 

IWEBRESP_GetInfo()

Description: 
   return a pointer to the response's relevant, pre-packaged/parsed info

Prototype:

   WebRespInfo *IWEBRESP_GetInfo(pIWebResp);

Parameters:

  pIWebResp:  the IWebResp interface pointer

Return Value:  
   pointer to a WespRespInfo

Comments:
   Besides the WebRespInfo, important information can also be retrieved
   via IWEBRESP_GetOpt(WEBOPT_FLAGS).  The result of such a call is a 
   logical OR of the below flags:

     WEBRESPONSE_REDIRECTED - redirection was handled by IWeb 
            (Unimplemented by BREW's HTTP protocol engine)

     WEBRESPONSE_COOKIES - cookies were sent in the request 
            (Unimplemented by BREW's HTTP protocol engine)

     WEBRESPONSE_CACHED - retrieved from local cache (no network activity) 

     WEBRESPONSE_KEEPALIVE  - connection kept alive 

     WEBRESPONSE_HTTP09 - got an HTTP0.9 response (so there were no HTTP 
            headers; and the WebRespInfo. nCode == 200 was fabricated

     WEBRESPONSE_LOCAL - locally-generated content (no network activity)
 
     WEBRESPONSE_PROXIED - gotten through a proxy 

     WEBRESPONSE_AUTHBASIC  - basic authentication info was sent.  
            (Unimplemented by BREW's HTTP protocol engine)

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   WebRespInfo

======================================================================= 
   INTERFACES DOCUMENTATION
======================================================================= 

IWeb Interface

Description: 
   The IWeb interface is the context in which web transactions are
   conducted, completed.  An IWeb interface pointer is designed to be
   created at application startup, initialized, and used until
   application shutdown.

   NOTE: Your application must have a privilege level of Web, Network or All
   to invoke the functions in this interface.

   IWeb's most interesting entry point is IWEB_GetResponse(), which
   takes a URL and an AEECallback and kicks off an asynchronous web 
   transaction.  IWeb supervises proxying, connection Keep-Alives, and
   other optional web-related behaviour.

   BREW has several built-in protocol engines for support of "http:",
   "https:", "file:", and "res:" URLs.

   BREW's "http:" engine supports and advertises HTTP version 1.0.
   The engine has support for HTTP Keep-Alives, which it manages
   without bothering the application with details of persistent
   sockets and protocol state.  Applications need only manage individual
   HTTP transactions via the IWEB_GetResponse() method.  This is true
   of all IWeb transactions, not just HTTP.

   BREW's "file:" engine opens files in BREW's application-relative
   file namespace.  Anything the application can pass to
   IFILEMGR_OpenFile() may be passed to IWEB_GetResponse() as a "file:"
   URL.  "file:" URLs that include a host name (i.e. FTP urls like
   "file://ftp.domain.com/foo.txt") generate an HTTP-like redirect
   header and response code.  It is the application's responsibility
   to handle redirects.  "file:" URLs are most properly specified with
   leading slashes, i.e.: "file:/foo.txt", not "file:foo.txt".

   BREW's "res:" engine opens BREW resources and returns them as
   IWebResps.  The syntax for "res:" URLs is:

      res:/<RESFILENAME>?<RESID>["." <TYPE>]

      RESFILENAME: the BREW application resource file in which to 
            find the resource
      RESID: the resource id of the requested resource as a 
            decimal number
      TYPE: the type of the resource requested as a DECIMAL NUMBER, 
            if this field is absent, it defaults to 6 (RESTYPE_IMAGE)

   For example: "res:/appname.bar?100" denotes the image resource with 
   resource id 100, and "res:/appname.bar?100.6" denotes the same.

   IWeb descends from IWebOpts, and uses WebOpt structures in the call
   to IWEB_GetResponse().  During a Web transaction the data in options
   passed to IWEB_GetResponse() are not copied by default, they must
   stay valid until the IWEB_GetResponse() callback fires.  Data
   passed as options to IWeb *are* copied by default, so it needn't be
   kept around, unless you specifically turn off COPYOPTS on the IWeb.
    
    ** Vals are null-terminated strings, valid ids up to 0x01ffff

        WEBOPT_METHOD - (char *) defaults to "GET" 

        WEBOPT_HEADER - (char *) CRLF-separated, name-value pairs like 
                   this: "Name1: v1\r\nName2: v2\r\n", defaults to NULL

        WEBOPT_PROXYSPEC - (char *) proxyspec format 

        WEBOPT_USERAGENT - (char *) user-agent string, if unset, IWeb
                   sends a default User-Agent header of the form:
         "BREW-Applet/<aid> (BREW/<vsn>; DeviceId: <did>; Lang: <lng>)"
                   where <aid> is the application's class id, 
                   <vsn> is a dotted-quad version number (e.g. 3.0.0.16),
                   <did> is dwPlatformID from AEEDeviceInfo and <lng> 
                   is dwLang from AEEDeviceInfo as a string. If this 
                   option is explicitly set to NULL, no user-agent 
                   is sent.
                   NOTE the value for this option should NOT be terminated by
                   a new line as one would be provided by the HTTP engine.
                   An excess newline will cause the HTTP request to become
                   incorrectly formated.

    ** 32-bit range: pVals 32-bit quantities, no need to (actually can't) 
                   copy, valid ids up to 0x02ffff

        WEBOPT_FLAGS - (unsigned int) default 0, specifies or denotes
                   request or response flags

        WEBOPT_HANDLERDATA - (void *) default NULL, user data pointer
                   passed to the functions registered for web status 
                   and web headers

        WEBOPT_STATUSHANDLER - (PFNWEBSTATUS) function to 
                call with transaction status, default NULL

        WEBOPT_HEADERHANDLER - (PFNWEBHEADER) function to call with
                header data, default NULL

        WEBOPT_CONNECTTIMEOUT - (unsigned int) in milli-seconds,
                defaults to AEENet connect timeout, also failover
                timeout, 0 means system default, -1 means infinite

        WEBOPT_CONTENTLENGTH - (long) if request or response,
                specifies size in bytes of message body

        WEBOPT_IDLECONNTIMEOUT - (int) time in *seconds* before IWeb
                closes idle web connections.  The default is INetMgr
                PPP linger.  This option has no effect
                when passed as an option to IWEB_GetResponse(), it
                must be set using IWEB_AddOpt().

        WEBOPT_ACTIVEXACTIONS - (unsigned int) scalar, maximum number
                of "active" web transactions allowed.  IWeb enforces
                this by queueing transactions started when the maximum
                number are already "active", meaning handed to the
                correct protocol handler.  This option has no effect
                when passed as an option to IWEB_GetResponse(), it must be
                set using IWEB_AddOpt().  This limit is examined at
                IWEB_GetResponse() time and anytime a previously queued
                transaction finishes, so changing the value has no
                effect until the next IWEB_GetResponse() or "GotResp"
                callback
                
        WEBOPT_SSL_CLASSID - (unsigned int) specifies the class id to 
                use for "https:" type connections to select the SSL 
                implementation.  By default, IWeb will use AEECLSID_SSL.
                See AEESSL.h for additional information.

        WEBOPT_REQUESTURI - (char *) Introduced as of BREW V3.1.5, this
                option is specifically designed to allow the default
                request URI string to be overridden, useful in case of
                a need to connect to a non-HTTP(S) server via a proxy tunnel
                using the CONNECT method.  Use of this option under other
                scenario is not recommended since it is usually set by
                default by the library by parsing the request URL.                              This option should be used in conjunction with 
                WEBOPT_METHOD and WEBOPT_REQUESTHOST.
                NOTE the value for this option should NOT be terminated by
                a new line as one would be provided by the HTTP engine.
                An excess newline will cause the HTTP request to become
                incorrectly formated.

        WEBOPT_REQUESTHOST - (char *) Introduced as of BREW 3.1.5, this
                option is designed specifically for CONNECT to a non-HTTP(S)
                server via a proxy tunnel, use of this option under other
                scenario is not recommended as the library will provide
                the correct default based on the request URL.  
                This option should be used in conjuntion with the
                WEBOPT_METHOD and WeBOPT_REQUESTURI option to correctly
                write the request headers for the desired connection.
                Note: For CONNECT to a HTTP server via a proxy, simply
                specify the PROXYSPEC option itself and there is no need
                for this option.
                NOTE the value for this option should NOT be terminated by
                a new line as one would be provided by the HTTP engine.
                An excess newline will cause the HTTP request to become
                incorrectly formated.
                
     ** BREW Interface range: pVals are pointers to interfaces, which are 
                AddRef'd to copy valid ids up to 0x03ffff

        WEBOPT_BODY - (ISource *), default NULL, used for POST body 
                in making a web request

        WEBOPT_DEFAULTS - (IWebOpt *), default NULL, used for chaining 
                IWebOpts.

   
Header files:
   AEEWeb.h

See Also: 
   IWebOpts,IWebResp

======================================================================= 
IWEB_AddRef()

Description:
This function is inherited from IBASE_AddRef().
======================================================================= 
IWEB_Release()

Description:
This function is inherited from IBASE_Release().
======================================================================= 
IWEB_QueryInterface()

Description:
This function is inherited from IQI_QueryInterface().
======================================================================= 
IWEB_GetOpt()

Description:
This function is inherited from IWEBOPTS_GetOpt().
======================================================================= 
IWEB_AddOpt()

Description:
This function is inherited from IWEBOPTS_AddOpt().
======================================================================= 
IWEB_RemoveOpt()

Description:
This function is inherited from IWEBOPTS_RemoveOpt().
======================================================================= 

IWEB_GetResponse()

Description: 
   This function begins a web transaction by establishing the
   resources and locations needed for a web response to be received.

Prototype:

   void IWEB_GetResponse(IWeb *pIWeb, IWebResp **ppiwresp, 
                         AEECallback *pcb, const char *cpszUrl, ...);

Parameters:

  pIWeb:  the interface pointer
  ppiwresp:  pointer to IWebResp interface pointer, where to stick
                        answer
  pcb:  where to call when answer is ready
  cpszUrl:  requested document, this pointer must stay valid
               until the above AEECallback fires, cpszUrl is not copied~
  list :  a variable list of WebOpt id/value pairs, terminated with WEBOPT_END

Return Value:  
   None
    
Comments:
   since the prototype is actually a macro that calls through a function 
   pointer, the call has to look like this:
===pre>   
     IWEB_GetResponse(pWeb, (pWeb, &pWResp, pcb, "http://www.qualcomm.com", 
                             WEBOPT_SOMETHING, someval,
                             WEBOPT_SOMETHING2, someval2,
                             WEBOPT_END));
===/pre>
   Note the inner parentheses for the varargs part of the call...

   The options presented to IWEB_GetResponse() (in the vararg list) are 
   IWEBOPTS_AddOpt()ed, in order, to an IWebOpts object that is then handed 
   to the protocol handler.  They are treated as *overrides* to any 
   options that are already set in the IWeb object.

   For example: An IWeb object with WEBOPT_CONNTIMEOUT set to 5000 via 
   IWEB_AddOpt() will do connection timeouts and/or failover at 5 seconds, 
   unless WEBOPT_CONNTIMEOUT is specified in the arguments to 
   IWEB_GetResponse(), which overrides this behaviour.

   An important WEBOPT to be passed to IWEB_GetResponse() is WEBOPT_FLAGS, 
   which influence the behaviour of the underlying protocol engine.  Here
   are the to-date defined WEBREQUEST_* flags (use WEBREQUEST_* flags when
   calling _GetResponse()) logically OR them together as one WebOpt:

       WEBREQUEST_REDIRECT - handle redirection invisibly, if possible
                   (Unimplemented by BREW's HTTP protocol engine)

       WEBREQUEST_NOCOOKIES - do not process cookies; pass headers 
                   through to caller (Unimplemented by BREW's HTTP 
                   protocol engine, but it this behaviour is the default)

       WEBREQUEST_NOCACHE - send "Pragma: no-cache" to proxy (if proxying) 
                   (Unimplemented by BREW's HTTP protocol engine)

       WEBREQUEST_NOKEEPALIVE - disable keep-alives; By default, the
                   HTTP engine sends a "Connection: Keep-Alive"
                   header.  If this flag is set, the header is not
                   sent, nor will the HTTP engine keep the socket open
                   even if the server sends Keep-Alive.  Specifying a
                   "Connection: " header of your own may have
                   unpredictable results, since the HTTP engine does
                   not parse headers passed in the request.

       WEBREQUEST_FORCENEWCONN  - force a new connection 

       WEBREQUEST_FORCEHTTP10  - force IWEB to behave like an HTTP1.0 client.
                  Default version of the library is HTTP1.1 for BREW 3.1.3 or
                  higher if this option is not specified.
                  NOTE: IWEB will continue to send 'Connection: Keep-Alive'
                  http header even in forced 1.0 mode.  This has been the
                  default BREW behavior since BREW 2.1 (See WEBOPT_NOKEEPALIVE
                  on how to disable that behavior).

       WEBREQUEST_NOWAITCONN - don't wait for reusable connection, force new

       WEBREQUEST_HTTPBOGUSCRLF - append an extra CRLF to HTTP request
                    bodies to work around a CERN webserver bug (see here:
                    http://httpd.apache.org/docs/misc/known_client_problems.html#trailing-crlf)

   IWebResp options are completely separate from IWeb options, i.e. 
   IWEBRESP_GetOpt() is not required to defer or chain to the IWeb with 
   which it was created.

   IWEB_GetResponse() options are overrides, supplements to the
   options passed to IWEB_AddOpt().  The options passed to
   IWEB_AddOpt() stay in effect until you call IWEB_RemoveOpt() or you
   release the IWeb pointer.  Options passed to IWEB_GetResponse()
   apply only to the transaction started by IWEB_GetResponse().  Prior
   to BREW 2.0, the above support was incomplete: some
   transaction-oriented options passed to IWEB_AddOpt() were ignored
   by BREW's http implememtation unless passed to IWEB_GetResponse().
   The options that *are* handled correctly by the pre-2.0 BREW http
   implementation are: WEBOPT_HEADER, WEBOPT_USERAGENT, and
   WEBOPT_FLAGS.  These options may be  *either* applied to the IWeb (via
   IWEB_AddOpt(), thereby applying to all transactions) or be passed
   to IWEB_GetResponse().  All other transaction-oriented options need
   to be passed at transaction time (passed to IWEB_GetResponse()).

   *AN IMPORTANT NOTE ABOUT WEBOPT LIFETIMES* 

   The pointers that represent pVals in the above list of WebOpts 
   (like extra header strings) must stay valid until the response callback 
   fires.  They are not copied unless WEBOPT_COPYOPTS is included in the 
   list of options **
   
    For example, DON'T use code similar to the following example:
===pre>    
      {
        char szDateHeader[32];
      
        SPRINTF(szDateHeader, "X-Date: %s\r\n", szToday);
        IWeb_GetResponse(me->piWeb, "http://www.somewhere.com/", &me->cb,
                                    WEBOPT_HEADER, szDateHeader,
                                    0);
      }
===/pre>
   because szHeader is on the stack, won't be copied by IWeb for the
   transaction, and thus will end up invalid when this function returns.

   Instead, you must use code similar to the following example:
===pre>

      typedef struct me {
         IWeb         *piWeb;
         AEECallback   cb;
         char          szDateHeader[32]; // here to hold data while 
                                         // web transaction completes
      } me;

      {
         SPRINTF(me->szDateHeader, "X-Date: %s\r\n", szToday);
         IWeb_GetResponse(me->piWeb, "http://www.somewhere.com/", &me->cb,
                                     WEBOPT_HEADER, me->szDateHeader,
                                     0);
      }

===/pre>
   You may also use code similar to the following example, though it
    will increase the chance the transaction will fail for lack of
    memory:
===pre>
      {
        char szDateHeader[32];
      
        SPRINTF(szDateHeader, "X-Date: %s\r\n", szToday);
        IWeb_GetResponse(me->piWeb, "http://www.somewhere.com/", &me->cb,
                                    WEBOPT_HEADER, szDateHeader,
                                    WEBOPT_COPYOPTS, TRUE,
                                    0);
      }

===/pre>
   In the case of header and status callbacks, you must be particularly 
   careful to keep the callback and the callback data valid while *any*
   web transaction that might use them is still in progress.

   A good way to make sure you don't violate any of these rules is to keep 
   and use a *single* IWeb pointer for the lifetime of your application.  
   This simplifies and thus bulletproofs your interaction with IWeb and its 
   callbacks and allows IWeb to do HTTP Keep-Alives properly.  When a new
   IWeb object is created, it initializes *itself* with WEBOPT_COPYOPTS, so
   options passed to AddOpt() need not live beyond that call, unless you
   RemoveOpt() WEBOPT_COPYOPTS immediately after you create the IWeb pointer.

   The IWebResp return value (pWResp, above) is filled with an IWebResp 
   interface pointer before the callback is fired.  Once the callback 
   fires, the caller of _GetResponse() is responsible for calling 
   _Release() on the IWebResp.

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   None

======================================================================= 

IWEB_GetResponseV()

Description: 

   vector form of GetResponse

Prototype:

   void IWEB_GetResponseV(IWeb *pIWeb, IWebResp **ppiwresp, 
                          AEECallback *pcb, const char *cpszUrl, 
                          WebOpt *awozList);

Parameters:

  pIWeb:  the interface pointer
  ppiwresp:  pointer to IWebResp interface pointer, where to stick
                        answer
  pcb:  where to call when answer is ready
  cpszUrl:  requested document, this pointer must stay valid
                    until the above AEECallback fires, cpszUrl is not copied
  awozList:  an array of WebOpts, terminated with WEBOPT_END

Return Value:
   None
    
Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   IWeb_GetResponse()


======================================================================= 
   INTERFACES DOCUMENTATION
======================================================================= 

IWebUtil Interface

Description: 

   This interface offers utility functions for web transactions,
   mostly stateless APIs.

See Also:
   IWeb

======================================================================= 
IWEBUTIL_AddRef()

Description:
This function is inherited from IBASE_AddRef().
======================================================================= 
IWEBUTIL_Release()

Description:
This function is inherited from IBASE_Release().
======================================================================= 
IWEBUTIL_QueryInterface()

Description:
This function is inherited from IQI_QueryInterface().
======================================================================= 

IWEBUTIL_ParseUrl()

Description: 
   This function parses a null-terminated URL string into its
   constituent parts

Prototype:

   int     IWEBUTIL_ParseUrl(IWebUtil *pIWebUtil, const char *cpszUrl, 
                             UrlParts *pup);

Parameters:

  pIWebUtil:  the interface pointer

  cpszUrl: the null-terminated string to parse
  
  pup: pointer to UrlParts struct to fill out
  

Return Value:  
   SUCCESS: if the URL seemed parseable
   EFAILED: otherwise

Comments:
   no modifications are made to the original string, pointers into it are 
   returned in the UrlParts structure

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   UrlParts

======================================================================= 

IWEBUTIL_MakeUrl()

Description: 
   This function constructs a full URL from a base URL and a relative
   URL, according to RFC2396

Prototype:

   int     IWEBUTIL_MakeUrl(IWebUtil *pIWebUtil, const char *cpszBase, 
                            const char *cpszRel, char *pResult, 
                            int nResultLen);

Parameters:

  pIWebUtil:  the interface pointer

  cpszBase: the null-terminated base URL (may be null)

  cpszRel: the null-terminated relative URL (may be null)
  
  pResult: where to put the full URL (may be null)
  
  nResultLen: size (in bytes) of pResult
  

Return Value:  
   number of bytes written, if pResult is non-null, and nResultLen large 
     enough (basically STRLEN(pResult) + 1)

   number of bytes needed to construct pResult, if either pResult is NULL 
     or if nResultLen is not large enough

Comments:
   if pResult is non-null and the result would not fit, pResult is set to ""

   pResult is always null-terminated

   the number of bytes required to construct the full URL may be larger than
     the  length  of the result, since this function coalesces "." and ".." 
     from hierarchal URLs

   Example code:
===pre>
   {
      int nLen;
      char *pResult;
   
      nLen = IWEBUTIL_MakeUrl(piwu, pszBase, pszRel, 0, 0);

      // now nLen holds size necessary to make the full URL...

      pResult = (char *)MALLOC(nLen);

      nLen = IWEBUTIL_MakeUrl(piwu, pszBase, pszRel, pResult, nLen);
      
      // now nLen holds STRLEN(pResult) + 1
   }
   
===/pre>
Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:

======================================================================= 

IWEBUTIL_UrlEncode()

Description: 
   encode characters in a string for inclusion in a in a URL, 
   according to RFC2936

Prototype:

   char *IWEBUTIL_UrlEncode(IWebUtil *pIWebUtil, const char *cpcIn, int *pnInLen,
                              char *pcOut, int *pnOutLen);

Parameters:
   
  pIWebUtil: the interface pointer

  cpcIn: pointer to string to be encoded

  pnInLen: [in/out] pointer to inLen, set on return to number of bytes
                  successfully encoded

  pcOut: [out] where to write encoded bytes, may be NULL, for counting

  pnOutLen: [in/out] On entry, this provides the maximum number of 
                  bytes to write into pcOut[].  On exit, this holds the 
                  number of bytes written to pcOut[].  When pcOut == NULL, 
                  this input value is ignored and the value on exit will 
                  be the number of bytes required to hold the encoded string.

Return Value:  
   pcOut

Comments:
   this function encodes *every* character that is either illegal in a URL
   or has syntactic meaning in a URL, it is meant not to be used on an entire
   URL, but only on parts of a URL that might have URL-illegal stuff, but
   that should be ignored (like the "query" section of a URL)

   This code can also be used to encode form contents (for the body of an 
   HTTP form POST)

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   None
======================================================================= 

IWEBUTIL_UrlDecode()

Description: 
   decodes characters in a URL-encoded string 

Prototype:

   char *IWEBUTIL_UrlDecode(IWebUtil *pIWebUtil, const char *cpcIn, int *pnInLen,
                              char *pcOut, int *pnOutLen);

Parameters:
   
   pIWebUtil: the interface pointer

   cpcIn: pointer to bytes to decode

   pnInLen: [in/out] sizeof cpcIn, on return set to number of bytes 
                  consumed
   pcOut: where to decode to
   
   pnOutLen: [in/out] On entry, this provides the maximum number of 
                  bytes to write into pcOut[].  On exit, this holds the 
                  number of bytes written to pcOut[].  When pcOut == NULL, 
                  this input value is ignored and the value on exit will 
                  be the number of bytes required to hold the decoded string.

Return Value:  
   pcOut

Comments: 
   UrlDecode() does generate zero-terminators for strings.  The caller must 
     write a zero after the bytes returned by UrlDecode() if a 
     zero-terminated string is required but zero-termination is 
     not already included in the input data.

Side Effects:
   None

See Also:

======================================================================= 

IWEBUTIL_EncodeBase64()

Description: 
   encode a buffer with base64

Prototype:

   char  *IWEBUTIL_EncodeBase64(IWebUtil *pIWebUtil, const char *cpcIn, int nInLen, 
                                char *pszOut);

Parameters:
   
   pIWebUtil: the interface pointer

   cpcIn: input

   nLen: len of cpcIn

   pszOut: output buffer

Return Value:  
   pcOut

Comments:
   pcOut must be allocated to at least BASE64LEN(nLen) where nLen is the
   length of the input data

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   None
======================================================================= 

IWEBUTIL_DecodeBase64()

Description: 
   decode a base64-encoded string into a buffer

Prototype:

   int IWEBUTIL_DecodeBase64(IWebUtil *pIWebUtil, const char *cpszIn, char *pcOut);

Parameters:
   
   pIWebUtil: the interface pointer

   cpszIn: input, base64 encoded, null-terminated

   pcOut: output buffer
   
Return Value:  
   the length of the resultant data, or -1 if the base64 string is bogus

Comments:
   pcOut must be long enough to absorb all the output of decoding
   it may also be the same as cpszIn, and decoding will be done in place

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   None
======================================================================= 

IWEBUTIL_ParseFormFields()

Description: 
   Parse the next nMaxFields form fields out of the x-www-urlencoded string
   *ppszIn, store the pointers to the beginnings of each name and value in 
   sequential elements of awff, and advance ppszIn to the beginning 
   of the next unparsed form field.
===pre>

   x-www-urlencoded form field strings have the form:

     name1=value1&name2=value2&name3=value3& ...

   or:

     name1=value1;name2=value2;name3=value3; ...

===/pre>
   where the name and value parts may not contain '=', ';' or '&', and so 
   may be URL escape encoded according to RFC2936

Prototype:

   int IWEBUTIL_ParseFormFields(IWebUtil *pIWebUtil, char **ppszIn, 
                                WebFormField *awff, int nMaxFields,
                                uint32 uFlags);

Parameters:
   
   pIWebUtil: the interface pointer

   ppszIn: input/output, pointer to the x-www-urlencoded list of
                   name value pairs, on return it is set to the beginning
                   of the next form field. Neither ppszIn nor *ppszIn 
                   may be NULL.

   awff: array of WebFormFields to be filled once parsed, may
                       be NULL

   nMaxFields: number of elements in awff

   uFlags: either 0 or a logical OR of -
===ul>

        WEBUTIL_PFFF_NODECODE - suppress URL decoding of name and
                value, awff will be filled with raw, but
                null-terminated values.  this can be useful if you
                expect binary data in your form fields (since NULLs
                may be encoded as %00)

        WEBUTIL_PFFF_NOMUTATE - suppress URL decoding _and_
                null-termination, awff will be filled with pointers
                into *ppszIn, any further parsing must be undertaken
                by the caller.  ppszIn will be set to the next form
                field, but the memory pointed to by *ppszIn will not
                be modified.
===/ul>   
Return Value:  
   the number of successfully parsed form fields, always less than or
   equal to nMaxFields

Comments:
===pre>
   * name and value are separated by '=', thus a form field may have a 
       zero-length name, value, or both.  For example:

         "x="  has the name "x", and a zero-length value
         "=y"  has a a zero-length name and the value "y"
         "="   has a zero-length name and zero-length value
         "x"   has the name "x" and a zero-length value
         ""    is not a form field, ParseFormField will return 0

   * form fields are separated by '&' or ';' and ParseFormFields() skips 
       over empty form fields (back-to-back separators, or the empty string):

         "x=y&"         contains one form field
         "&x=y"         contains one form field
         "&&&x=y&&&w=z" contains two form fields
         ""             contains zero form fields
         "&&&&&"        contains zero form fields
         ";;;;"         contains zero form fields
      
      This API does not provide a way to distinguish between "x=" and "x"

   * behaviour of ParseFormFields() is undefined when flags other than 
      those given above are passed 

   * if awff is NULL, parsing _still_occurs_, ppszIn is _still_
      _written_to_ (unless WEBUTIL_PFFF_NOMUTATE is passed), and *ppszIn is 
      _still_set_ to the beginning of the next form field.

   * to merely _count_ the number of form fields in a string, call 
      ParseFormFields() thusly:

   IWEBUTIL_ParseFormFields(&pszQuery,             // string to parse
                            0,                     // don't fill anything
                            ((unsigned int)-1)>>1, // no limit (MAXINT)
                            WEBUTIL_PFFF_NOMUTATE);// don't destroy pszQuery
===/pre>
Side Effects:
   None other than those listed above

Version:
   Introduced BREW Client 1.1

See Also:
   IWEBUTIL_UrlDecode(), WebFormField

======================================================================= 
   INTERFACES DOCUMENTATION
======================================================================= 

IWebEng Interface

Description: 

   IWebEng is an abstract base class that allows dynamic BREW 
     modules to extend the functionality of IWeb by registering as a 
     protocol handler either via ISHELL_RegisterHandler(), or by adding 
     an entry in the module's MIF's list of extensions.  It is an 
     abstract base class.

Note: 
   This interface is subject to change.

Header files:
   AEEWeb.h

See Also:
   IWeb, IWebResp

======================================================================= 
IWEBENG_AddRef()

Description:
This function is inherited from IBASE_AddRef().
======================================================================= 
IWEBENG_Release()

Description:
This function is inherited from IBASE_Release().
======================================================================= 
IWEBENG_QueryInterface()

Description:
This function is inherited from IQI_QueryInterface().
======================================================================= 

IWEBENG_Transaction()

Description: 
   Start handling the described IWeb Transaction.  This method is 
     called by IWeb when the URL for IWEB_GetResponse() matches
     the scheme handled by this IWebEng

Prototype:

   int IWEBENG_Transaction(IWebEng *piwe, IWeb *piw, IWebReq *piwreq, 
                           AEECallback *pcb, IWebResp **ppiwresp);


Parameters:

  piwe: an implementation of IWebEng that handles the relevant 
                scheme

  piw: pointer to the IWeb object that's handling the WebRequest

  piwreq: the request, holds the URL and specific
                   information about this transaction (e.g. extra headers)

  pcb: where to call when the transaction is to a point 
                    where an IWebResponse can be used

  ppiwresp: [output] place to put newly created IWebResp.

Return Value:  
  SUCCESS: if the IWebEng was capable of starting the transaction (usually
       if the IWebResp could be allocated)

  failure code from AEEError.h (not negated)

Comments:
   If the IWebEng runs into a real bad problem, returning an AEEError is
     ok, and IWeb will construct an IWebResp on the IWebEng's behalf.

   If the IWebEng returns SUCCESS, it *must* fire the AEECallback with a 
     *valid* IWebResp pointer in *ppiwresp.

   If the IWebEng returns anything other than SUCCESS the IWebEng *must not*
     fire the AEECallback.

   The callback function in pcb must be fired *synchronously* once ppiwresp
      is filled, otherwise a memory leak could result.

   Once IWEBENG_Transaction() is invoked and SUCCESS returned, the IWebEng
       pointer is immediately Release()d by IWeb.

Side Effects:
   None

Version:
   Introduced BREW Client 3.1

See Also:
   IWebReq, IWEB_GetResponse()

=======================================================================  

*/

#endif /* AEEWEB_H */
