#ifndef __UXURI_H__
#define __UXURI_H__
/*===========================================================================


								 Q X U R I
             
                            H E A D E R  F I L E

FILE:  UxURI.h

DESCRIPTION

	This header file declares a class UxURI and functions to parse URI,
	and access different components of URI(Uniform Resource Identifier)
	
	
PUBLIC CLASSES 
class UxURI
	
INITIALIZATION AND SEQUENCING REQUIREMENTS
	NA

		  (c) COPYRIGHT <2005> QUALCOMM Incorporated.
                      All Rights Reserved.
                    QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================  

                            EDIT HISTORY FOR FILE
                            
  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/UxURI.h#1 $ 
  $DateTime: 2007/07/17 11:00:30 $ 2005/09/01 14:21:57
  $Author: jmiera $ Nitin Veda  


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/08/06    ws     Prefixed port numbers to avoid compile collisions
01/03/05    ws     Minor changes to build on-target.
10/11/05   Nit	   Initial Version			

===========================================================================*/

/*===========================================================================
                           
               INCLUDE FILES

============================================================================*/
#include <stdio.h>
#include <string.h>	//String library to provide string parsing functionality.
#include <stdlib.h>
#include "UxURIDebug.h"
#include "comdef.h"
#include "IxErrno.h"

//Delimeters needed to parse a URI
#define QX_URI_MAGIC_CHR '#'
#define QX_URI_MAGIC_STR "#"
#define QX_URI_PATH_CHR '/'
#define QX_URI_PATH_STR "/"
#define QX_URI_NULL_CHR '\0'
#define QX_URI_NULL_STR ""
#define QX_URI_COLON_CHR ':'
#define QX_URI_COLON_STR ":"
#define QX_URI_QMARK_CHR '?'
#define QX_URI_QMARK_STR "?"
#define QX_URI_ATTHERATE_CHR '@'
#define QX_URI_ATTHERATE_STR "@"
#define QX_URI_DOUBLEPATH_STR "//"
#define QX_URI_ASSIGNMENT_CHR '='
#define QX_URI_ASSIGNMENT_STR "="

#define PORT_SIZE 5 //Port size can be any integer

//Default port numbers
#define UXURI_DEFAULT_FTP_PORT          21
#define UXURI_DEFAULT_TELNET_PORT       23
#define UXURI_DEFAULT_GOPHER_PORT       70
#define UXURI_DEFAULT_HTTP_PORT         80
#define UXURI_DEFAULT_NNTP_PORT        119
#define UXURI_DEFAULT_WAIS_PORT        210
#define UXURI_DEFAULT_HTTPS_PORT       443
#define UXURI_DEFAULT_RTSP_PORT        554
#define UXURI_DEFAULT_RTSPU_PORT       554
#define UXURI_DEFAULT_PROSPERO_PORT   1525
#define UXURI_DEFAULT_H323_PORT       1720
#define UXURI_DEFAULT_H323RAS_PORT    1719
#define UXURI_DEFAULT_SIP_PORT        5060

#define DEFAULT_SCHEME 0
#define FILE_SCHEME    1

#define FALSE 0
#define TRUE  1

//Structure to store different parameters of a URI
struct SchemeStruct {
  const char *name;
  bool hasUsername;
  bool hasPassword;
  bool hasHostPort;
  bool defaultHostToLocal;
  bool hasQuery;
  bool hasParameters;
  bool hasFragments;
  bool hasPath;
  bool relativeImpliesScheme;
  int32 defaultPort;
};


//Structure to store a scheme and what all parameters(hostname, portname,
//fragments,query, path etc)
//a particular scheme can have.
//  scheme       user   pass   host   defhost query  params frags  path   
//	rel    port
static SchemeStruct const SchemeTable[] = {
  { "http",      FALSE, FALSE, TRUE,  TRUE,   TRUE,  TRUE,  TRUE,  TRUE,  
  TRUE,  UXURI_DEFAULT_HTTP_PORT     }, 
  { "file",      FALSE, FALSE, TRUE,  TRUE,   FALSE, FALSE, FALSE, TRUE,  
  FALSE, 0                     }, 
  { "https",     FALSE, FALSE, TRUE,  TRUE,   TRUE,  TRUE,  TRUE,  TRUE,  
  TRUE,  UXURI_DEFAULT_HTTPS_PORT    },
  { "gopher",    FALSE, FALSE, TRUE,  TRUE,   FALSE, FALSE, FALSE, TRUE,  
  FALSE, UXURI_DEFAULT_GOPHER_PORT   },
  { "wais",      FALSE, FALSE, TRUE,  FALSE,  FALSE, FALSE, FALSE, TRUE,  
  FALSE, UXURI_DEFAULT_WAIS_PORT     },
  { "nntp",      FALSE, FALSE, TRUE,  TRUE,   FALSE, FALSE, FALSE, TRUE,  
  FALSE, UXURI_DEFAULT_NNTP_PORT     },
  { "prospero",  FALSE, FALSE, TRUE,  TRUE,   FALSE, FALSE, FALSE, TRUE,  
  FALSE, UXURI_DEFAULT_PROSPERO_PORT },
  { "rtsp",      FALSE, FALSE, TRUE,  TRUE,   FALSE, FALSE, FALSE, TRUE,  
  FALSE, UXURI_DEFAULT_RTSP_PORT     },
  { "rtspu",     FALSE, FALSE, TRUE,  TRUE,   FALSE, FALSE, FALSE, TRUE,  
  FALSE, UXURI_DEFAULT_RTSPU_PORT    },
  { "ftp",       TRUE,  TRUE,  TRUE,  TRUE,   FALSE, FALSE, FALSE, TRUE,  
  FALSE, UXURI_DEFAULT_FTP_PORT      },
  { "telnet",    TRUE,  TRUE,  TRUE,  TRUE,   FALSE, FALSE, FALSE, FALSE, 
  FALSE, UXURI_DEFAULT_TELNET_PORT   },
  { "mailto",    FALSE, FALSE, FALSE, TRUE,   TRUE,  FALSE, FALSE, TRUE, 
  FALSE, 0                     },
  { "news",      FALSE, FALSE, FALSE, TRUE,   FALSE, FALSE, FALSE, TRUE, 
  FALSE, 0                     },
  { "h323",      TRUE,  FALSE, TRUE,  FALSE,  FALSE, TRUE,  FALSE, FALSE, 
  FALSE, UXURI_DEFAULT_H323_PORT     },
  { "sip",       TRUE,  TRUE,  TRUE,  FALSE,  FALSE, TRUE,  FALSE, FALSE, 
  FALSE, UXURI_DEFAULT_SIP_PORT      },
  { "tel",       FALSE, FALSE, FALSE, FALSE,  FALSE, TRUE,  FALSE, TRUE, 
  FALSE, 0                     },
  { "fax",       FALSE, FALSE, FALSE, FALSE,  FALSE, TRUE,  FALSE, FALSE, 
  FALSE, 0                     },
  { "callto",    FALSE, FALSE, FALSE, FALSE,  FALSE, TRUE,  FALSE, FALSE, 
  FALSE, 0                     },
  { "memory",    FALSE, FALSE, FALSE, FALSE,  FALSE, TRUE,  FALSE, FALSE, 
  FALSE, 0                     },
  {'\0',         TRUE,  TRUE,  TRUE,  TRUE,   TRUE,  TRUE,  TRUE,  TRUE,  
  TRUE,  0                     }
};

/*===========================================================================

                           CLASS DEFINITION

CLASS:
	UxURI


DESCRIPTION:
	Class having different functions to 	


PUBLIC FUNCTIONS & CONSTRUCTORS:

	UxURI(); //Default Constructor

	UxURI (
        const char *texturi
        );

    UxURI(
        const char *scheme,
        const char *host,
        const int32 port,
        const char *path
        );

    UxURI(
        const char *scheme,
        const char *authority,
        const char *path,
        const char *query = NULL,
        const char *fragment = NULL
        );

	~UxURI(); //Destructor

	void Initialize();

	const char *GetScheme ();

    const char *GetAuthority ();

    const char *GetPath ();

    const char *GetQuery ();

    const char *GetFragment ();

    const char *GetHostName ();

    int32 GetPort();

	const char *GetUserName();

	const char *GetPassword();

	const char *GetParent();

    const char *GetRootPath();

	int32 GetLength();

    int32 GetUriString (char **uristring, int32 len);

    UxURIError GetLastError ();

	const char *GetLastErrorString();

	const char *GetQueryValueName(char *name);

    bool SetScheme (const char *scheme);

    bool SetAuthority (const char *authority);

    bool SetPath (const char *path);

    bool SetQuery (const char *query);

	bool SetQueryNameValue(char *name,char *value);

	bool SetFragment (const char *fragment);

    bool SetHostName (char *hostname);

    bool SetPort(int32 hostport);
    
	bool SetUserName(const char *newusername);

	bool SetPassword(const char *newpassword);

	bool AppendPath (char *pathtext);

    bool AppendFragment (char *fragment);

	UxURI *CreateRelative (const char *relativereference);

	UxURI *Duplicate();

    bool HasParent();

	bool IsLocal();

  	void ClearAll();

    bool operator== (UxURI *otheruri);

    void operator= (UxURI *otheruri);

	
PRIVATE FUNCTION
	
	const SchemeStruct *GetSchemeInfo(const char *scheme);

	char *GetSchemeFromText(const char *texturi);

	void ParseURI(const char* texturi);

	void ValidateInputString(const char *texturi);

===========================================================================*/

class UxURI
{

public : // constructors and destructors

/*==============================================================
FUNCTION:  UxURI :: UxURI()

DESCRIPTION:
Default constructor caling Initialise function to initialise 
private members of class

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
None

SIDE EFFECTS:
None.
==============================================================*/
	UxURI(); //Default Constructor

/*==============================================================
FUNCTION:  UxURI :: UxURI()

DESCRIPTION:
Function to parse a string, this is used to maintain client 
interface , incase we want to implement a different approach to
parse a string we can replace ParseInputString function.

INPUT/OUTPUT PARAMETERS:
Input - const char *texturi : Pointer to input string

RETURN VALUE:
None

SIDE EFFECTS:
None.
==============================================================*/
	UxURI (
        const char *texturi
        );

/*==============================================================
FUNCTION:  UxURI :: UxURI()

DESCRIPTION:
This Function takes initial parameters to represent a remote path

INPUT/OUTPUT PARAMETERS:
Input - const char *schemeuri : pointer to character string representing
		scheme of URI
		const char *host		 : pointer to character string representing
		host of URI
		const int32 port		 : pointer to character string representing 
		port of URI
		const char *pathuri   : pointer to path string representing
		path of URI

RETURN VALUE:
None

SIDE EFFECTS:
None.
==============================================================*/
    UxURI(
        const char *scheme,
        const char *host,
        const int32 port,
        const char *path
        );

/*==============================================================
FUNCTION:  UxURI :: UxURI()

DESCRIPTION:
This Function is used to create a UxURI object with default parameters

INPUT/OUTPUT PARAMETERS:
Input - const char *schemeuri : pointer to character string representing
		scheme of URI
		const char *authority	 : pointer to character string representing
		authority of URI
		const int32 *path		 : pointer to character string representing 
		path of URI
		const char *query      : pointer to query string representing
		query of URI
		const char *fragment   : pointer to fragmentstring representing 
		fragment of URI


RETURN VALUE:
None

SIDE EFFECTS:
None.
==============================================================*/
    UxURI(
        const char *scheme,
        const char *authority,
        const char *path,
        const char *query = NULL,
        const char *fragment = NULL
        );

/*==============================================================
FUNCTION:  UxURI :: ~UxURI()

DESCRIPTION:
Destructor - Calling ClearAll to free all variables

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
None

SIDE EFFECTS:
None.
==============================================================*/
	~UxURI(); //Destructor


public:

/*==============================================================
FUNCTION:  UxURI :: GetScheme()

DESCRIPTION:
Get the scheme part of URI

INPUT/OUTPUT PARAMETERS:
None
		
RETURN VALUE:
Pointer to character string representing scheme of URI

SIDE EFFECTS:
None.
==============================================================*/
	const char *GetScheme ();

/*==============================================================
FUNCTION:  UxURI :: GetAuthority()

DESCRIPTION:
Get the authority part of URI

INPUT/OUTPUT PARAMETERS:
None
		
RETURN VALUE:
Pointer to character string representing authority of URI

SIDE EFFECTS:
None.
==============================================================*/
    const char *GetAuthority ();

/*==============================================================
FUNCTION:  UxURI :: GetPath()

DESCRIPTION:
Get the path part of URI

INPUT/OUTPUT PARAMETERS:
None
		
RETURN VALUE:
Pointer to character string representing path of URI

SIDE EFFECTS:
None.
==============================================================*/
    const char *GetPath ();

/*==============================================================
FUNCTION:  UxURI :: GetQuery()

DESCRIPTION:
Get the query part of URI

INPUT/OUTPUT PARAMETERS:
None
		
RETURN VALUE:
Pointer to character string representing query of URI

SIDE EFFECTS:
None.
==============================================================*/
    const char *GetQuery ();

/*==============================================================
FUNCTION:  UxURI :: GetFragment()

DESCRIPTION:
Get the fragment part of URI

INPUT/OUTPUT PARAMETERS:
None
		
RETURN VALUE:
Pointer to character string representing fragment of URI

SIDE EFFECTS:
None.
==============================================================*/
    const char *GetFragment ();

/*==============================================================
FUNCTION:  UxURI :: GetHostName()

DESCRIPTION:
Method returns the host name from the URI

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
Host name of URI if successful, NULL otherwise

SIDE EFFECTS:
None.
==============================================================*/
    const char *GetHostName ();

/*==============================================================
FUNCTION:  UxURI :: GetPort()

DESCRIPTION:
Method returns the host port number from the URI

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
Host port number from the URI if successful, NULL otherwise

SIDE EFFECTS:
None.
==============================================================*/

    int32 GetPort();

/*==============================================================
FUNCTION:  UxURI :: GetUserName()

DESCRIPTION:
Method to extract username from ftp/telnet
 ftp://user:password@host:port/path...
 telnet://user:password@host:port

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
Character pointer to User name

SIDE EFFECTS:
Caller has to free the variable returned
==============================================================*/
	const char *GetUserName();

/*==============================================================
FUNCTION:  UxURI :: GetPassword()

DESCRIPTION:
Method to extract password from ftp/telnet
 ftp://user:password@host:port/path...
 telnet://user:password@host:port


INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
Character pointer to password

SIDE EFFECTS:
User has to free the return character pointer.
==============================================================*/
	const char *GetPassword();

/*==============================================================
FUNCTION:  UxURI :: GetParent()

DESCRIPTION:
Returns the parent path for the URI

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
TRUE if URI represents a valid parent path FALSE otherwise.

SIDE EFFECTS:
None.
==============================================================*/
	const char *GetParent();

/*==============================================================
FUNCTION:  UxURI :: GetRootPath()

DESCRIPTION:
Returns the character string representing the root path for the URI

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
Character string represents the root path for the URI otherwise NULL

SIDE EFFECTS:
None.
==============================================================*/
    const char *GetRootPath();

/*==============================================================
FUNCTION:  UxURI :: GetLength()

DESCRIPTION:
Function returns the length of the URI in bytes

INPUT/OUTPUT PARAMETERS:
None
		
RETURN VALUE:
Int - Length of URI in bytes

SIDE EFFECTS:
None.
==============================================================*/
	int32 GetLength();

/*==============================================================
FUNCTION:  UxURI :: GetUriString()

DESCRIPTION:
Function returns URI as character string

INPUT/OUTPUT PARAMETERS:
char **uristring - User send the empty string
int32 len - Length of string 

RETURN VALUE:
int32 - Return error code if fails else NO_ERROR

SIDE EFFECTS:
None.
==============================================================*/
    int32 GetUriString (char **uristring, int32 len);

/*==============================================================
FUNCTION:  UxURI :: GetLastError()

DESCRIPTION:
Method returns the error type defined

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
IxErrnoType enumeration value representing the various error types

SIDE EFFECTS:
None.
==============================================================*/
    IxErrnoType GetLastError ();

/*==============================================================
FUNCTION:  UxURI :: GetLastErrorString()

DESCRIPTION:
Method gets the last error in a printable string format

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
Character string returned is a valid character string

SIDE EFFECTS:
None.
==============================================================*/
	const char *GetLastErrorString();


/*==============================================================
FUNCTION:  UxURI :: GetQueryValueName()

DESCRIPTION:
Method gets the query value equal to the name passed

INPUT/OUTPUT PARAMETERS:
Input - char *name : Character pointer representing name in 
		Querystring
		
RETURN VALUE:
Return value of name in query string if successful,NULL otherwise

SIDE EFFECTS:
None.
==============================================================*/
	const char *GetQueryValueName(char *name);


/*==============================================================
FUNCTION:  UxURI :: SetScheme()

DESCRIPTION:
Set the scheme for the URI

INPUT/OUTPUT PARAMETERS:
Input - const char *scheme : pointer to character string
		representing scheme of URI
		
RETURN VALUE:
bool- TRUE on success and FALSE on error
	  

SIDE EFFECTS:
None.
==============================================================*/
    bool SetScheme (const char *scheme);

/*==============================================================
FUNCTION:  UxURI :: SetAuthority()

DESCRIPTION:
Set the authority for the URI

INPUT/OUTPUT PARAMETERS:
Input - const char *authority : pointer to character string 
		representing authority of URI
		
RETURN VALUE:
bool- TRUE on success and FALSE on error

SIDE EFFECTS:
None.
==============================================================*/
    bool SetAuthority (const char *authority);

/*==============================================================
FUNCTION:  UxURI :: SetPath()

DESCRIPTION:
Set the path for the URI

INPUT/OUTPUT PARAMETERS:
Input - const char *path : pointer to character string 
		representing path of URI.
		
RETURN VALUE:
bool- TRUE on success and FALSE on error

SIDE EFFECTS:
None.
==============================================================*/
    bool SetPath (const char *path);

/*==============================================================
FUNCTION:  UxURI :: SetQuery()

DESCRIPTION:
Set the query for the URI

INPUT/OUTPUT PARAMETERS:
Input - const char *query : pointer to character string 
		representing query of URI
		
RETURN VALUE:
bool- TRUE on success and FALSE on error

SIDE EFFECTS:
None.
==============================================================*/
    bool SetQuery (const char *query);

/*==============================================================
FUNCTION:  UxURI :: SetQueryNameValue()

DESCRIPTION:
Set name and value for a query string

INPUT/OUTPUT PARAMETERS:
Input - char *name : Character pointer representing name in 
		Querystring
		char *value : Character pointer representing value in 
		Querystring

RETURN VALUE:
Return TRUE if query is updated FALSE otherwise

SIDE EFFECTS:
None.
==============================================================*/
	bool SetQueryNameValue(char *name,char *value);

/*==============================================================
FUNCTION:  UxURI :: SetFragment()

DESCRIPTION:
Set the fragment for the URI

INPUT/OUTPUT PARAMETERS:
Input - const char *fragment : Pointer to character string 
		representing fragment of URI
		
RETURN VALUE:
bool- TRUE on success and FALSE on error

SIDE EFFECTS:
None.
==============================================================*/
	bool SetFragment (const char *fragment);

/*==============================================================
FUNCTION:  UxURI :: SetHostName()

DESCRIPTION:
Method sets the host name for the URI

INPUT/OUTPUT PARAMETERS:
Input - char *hostname : Character string representing host
		name of URI

RETURN VALUE:
TRUE if the hostname is updated successfully ,FALSE otherwise

SIDE EFFECTS:
None.
==============================================================*/
    bool SetHostName (char *hostname);


/*==============================================================
FUNCTION:  UxURI :: SetPort()

DESCRIPTION:
Method sets the port number for the URI

INPUT/OUTPUT PARAMETERS:
Input - int32 hostport : port number for the uri

RETURN VALUE:
TRUE if the hostnumber is updated successfully ,FALSE otherwise

SIDE EFFECTS:
None.
==============================================================*/
    bool SetPort(int32 hostport);

/*==============================================================
FUNCTION:  UxURI :: SetUserName()

DESCRIPTION:
Method to set user name for ftp/telnet
 ftp://user:password@host:port/path...
 telnet://user:password@host:port

INPUT/OUTPUT PARAMETERS:
const char *newusername - New User Name to set

RETURN VALUE:
Return NO_ERROR if User Name is set successfully, else ERROR_VALUE

SIDE EFFECTS:
None.
==============================================================*/
 	bool SetUserName(const char *newusername);

/*==============================================================
FUNCTION:  UxURI :: SetPassword()

DESCRIPTION:
Method to extract username from ftp/telnet
 ftp://user:password@host:port/path...
 telnet://user:password@host:port

INPUT/OUTPUT PARAMETERS:
const char *newpassword - new password to set

RETURN VALUE:
Return NO_ERROR if password is set successfully, else ERROR_VALUE

SIDE EFFECTS:
None.
==============================================================*/
	bool SetPassword(const char *newpassword);


/*==============================================================
FUNCTION:  UxURI :: AppendPath()

DESCRIPTION:
Append path to the existing URI path

INPUT/OUTPUT PARAMETERS:
Input- char *pathtext : path to be appended to the URI path
		
RETURN VALUE:
bool- TRUE on success and FALSE on error

SIDE EFFECTS:
None.
==============================================================*/
	bool AppendPath (char *pathtext);

/*==============================================================
FUNCTION:  UxURI :: AppendFragment()

DESCRIPTION:
Append fragment path to the existing URI fragment section

INPUT/OUTPUT PARAMETERS:
Input- char *pathtext : fragment string to be appended to 
		existing URI fragment
		
RETURN VALUE:
bool- TRUE on success and FALSE on error

SIDE EFFECTS:
None.
==============================================================*/
    bool AppendFragment (char *fragment);

/*==============================================================
FUNCTION:  UxURI :: CreateRelative()

DESCRIPTION:
This Function is used to create a UxURI object relative to 
path specified.

INPUT/OUTPUT PARAMETERS:
Input - const char *relative_reference : reference path from which
		UxURI needs to be created
		
RETURN VALUE:
UxURI - class object

SIDE EFFECTS:
None.
==============================================================*/
	UxURI *CreateRelative (const char *relativereference);

/*==============================================================
FUNCTION:  UxURI :: Duplicate()

DESCRIPTION:
Creates a clone copy of the existing URI

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
pointer to newly created UxURI. Returns NULL if the operation fails

SIDE EFFECTS:
None.
==============================================================*/
	UxURI *Duplicate();


/*==============================================================
FUNCTION:  UxURI :: HasParent()

DESCRIPTION:
Method checks if the URI path has a valid parent path

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
TRUE if URI represents a valid parent path FALSE otherwise.

SIDE EFFECTS:
None.
==============================================================*/
    bool HasParent();

/*==============================================================
FUNCTION:  UxURI :: IsLocal()

DESCRIPTION:
Method checks if the URI represents a local path in the system

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
TRUE if URI represents a local path FALSE otherwise.

SIDE EFFECTS:
None.
==============================================================*/
	bool IsLocal();

/*==============================================================
FUNCTION:  UxURI :: ClearAll()

DESCRIPTION:
Clear the URI details stored in private variable. 

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
None

SIDE EFFECTS:
None.
==============================================================*/
  	void ClearAll();


/*==============================================================
FUNCTION:  UxURI :: operator ==()

DESCRIPTION:
Comparison operator overloading. The URI specified is compared with 
itself.

INPUT/OUTPUT PARAMETERS:
Input - UxURI *otheruri : URI to be compared with

RETURN VALUE:
TRUE if both URI's match ,FALSE otherwise

SIDE EFFECTS:
None.
==============================================================*/
    bool operator == (UxURI &otheruri);

/*==============================================================
FUNCTION:  UxURI :: operator =()

DESCRIPTION:
assignment operator overloading for the URI object. 

INPUT/OUTPUT PARAMETERS:
Input - UxURI *otheruri : URI to be assigned with

RETURN VALUE:
TRUE if URI's updated successfully,FALSE otherwise

SIDE EFFECTS:
None.
==============================================================*/
    void operator= (UxURI &otheruri);

private:
/*==============================================================
FUNCTION:  UxURI :: Initialize()

DESCRIPTION:
Method to initialize variables

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
None

SIDE EFFECTS:
None.
==============================================================*/
	void Initialize();

/*==============================================================
FUNCTION:  UxURI :: GetSchemeInfo()

DESCRIPTION:
Method to populate SchemeTable structure depending on schema value

INPUT/OUTPUT PARAMETERS:
None

RETURN VALUE:
None

SIDE EFFECTS:
None.
==============================================================*/
	const SchemeStruct *GetSchemeInfo(const char *scheme);

/*==============================================================
FUNCTION:  UxURI :: GetSchemeFromText()

DESCRIPTION:
Method to get scheme from given charcater pointer

INPUT/OUTPUT PARAMETERS:
Input :Point const char *texturi - pointer to character string

RETURN VALUE:
character pointer to scheme

SIDE EFFECTS:
None.
==============================================================*/
	char *GetSchemeFromText(const char *texturi);

/*==============================================================
FUNCTION:  UxURI :: ParseURI()

DESCRIPTION:
Method to parse a given URI

INPUT/OUTPUT PARAMETERS:
Input : const char *texturi - pointer to character string

RETURN VALUE:
character pointer to scheme

SIDE EFFECTS:
None.
==============================================================*/
	void ParseURI(const char* texturi);

/*==============================================================
FUNCTION:  UxURI :: ValidateInputString()

DESCRIPTION:
Function to parse given string into scheme,authority,query,path
and fragment

INPUT/OUTPUT PARAMETERS:
Input - const char *texturi : pointer to input string 

RETURN VALUE:
None

SIDE EFFECTS:
None.
==============================================================*/
	void ValidateInputString(const char *texturi);


private:

	SchemeStruct const *sstruct;
	char *scheme;
	char *authority;
	char *path;
	char *query;
	char *fragment;
	IxErrnoType error;
	char *hostname;
	char *username;
	char *password;
	char *parentpath;
	char *rootpath;
	char *queryvalue;
};

#endif//__UXURI_H__
