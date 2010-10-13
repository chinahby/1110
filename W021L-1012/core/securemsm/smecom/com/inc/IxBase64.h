#ifndef __QXBASE64_H__
#define __QXBASE64_H__

/*===========================================================================
*
*                              Q X  B A S E 6 4
*             
*                            H E A D E R  F I L E
*
* FILE:  IxBase64.h
*
* DESCRIPTION :
*
* This header file defines the interface to IxBase64
* IxBase64 defines an interface to Encode and Decode the data 
* using of the base64 encoding and decoding mechanism.
*
* PUBLIC METHODS:
*     EncodeBase64()  : This function encodes the given the string.
*     DecodeBase64()  : This function decodes the given encoded string.
*
* Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.  
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
                            
  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/IxBase64.h#1 $ 
  $DateTime: 2007/07/17 11:00:30 $ 2005/10/06 
  $Author: jmiera $ Madhav Chinthala  


when        who         what, where, why
--------    ---         ----------------------------------------------------
12/20/05    MC          Updated function headers with error info
12/09/05    MC          Updated header preprocessor directive
10/24/05    MC          Updated with 2 space indentation
10/07/05    MC          Updates with review comments
2005/10/06  Madhav C    Initial Version
            

===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "comdef.h"

// Define constants 
//#define QXBASE64_ERR_BASE     200
#define QXBASE64_SUCCESS        0 
#define QXBASE64_INVALID_LEN    -1 
#define QXBASE64_BOGUS_STRING   -2 
#define QXBASE64_INVALID_PAR    -3 

#define XX 127
#define BASE64LEN(x) (((((x) + 2) / 3) * 4) + 1)

#ifndef __cplusplus
//Definitions for "C" files

typedef struct
{
   uint32 dummy;
} IxBase64;

IxBase64*       Call_IxBase64_New(void);
void            Call_IxBase64_Delete(IxBase64 *obj);
int32           Call_IxBase64_DecodeBase64(IxBase64 *obj, const char *cpcIn, char *pcOut);
char*           Call_IxBase64_EncodeBase64(IxBase64 *obj, const char *cpcIn, int32  nInLen, char *pcOut);

#else //__cplusplus
// Definitions for "C++" files

/*========================================================================
* CLASS:      IxBase64
*
* DESCRIPTION:
*     This class is used to encode/decode data using base64 encoding 
*     mechanism.
*
* PUBLIC METHODS & CONSTRUCTORS:
*     IxBase64()      : IxBase64 Constructor
*     ~IxBase64()     : IxBase64 Destructor
*     EncodeBase64()  : This function encodes the given the string.
*     DecodeBase64()  : This function decodes the given encoded string.
*
* PRIVATE METHODS:
*     None
*/

class IxBase64 {

  public: // constructor and destructor

    /** Constructor for IxBase64
    * This function creates a IxBase64 object.
    * 
    * @param 
    * 
    * @return
    */

  IxBase64();

  /** Desctructor for IxBase64
    * This function deletes the IxBase64 object and aslo reclaims any of the 
    * underlying buffers of the IxBase64.
    *
    * @param 
    * 
    * @return
    */
    
  ~IxBase64();
   
  public: // Methods

  /** DecodeBase64
    * This function is used to Decode the printable test string to binary
    * data using base64 content encoding transfer mechanism.
    * 
    * @param   cpcIn     - Encoded base64 string
    * @param   cpcOut    - Output string to place the decoded string
    * 
    * @return  IxInt     - Return the number of bytes decoded, 
    *                      if any error returns the Error.
    *                      - QXBASE64_INVALID_PAR : Invalid parameters
    *                      - QXBASE64_INVALID_LEN : Invalid input string lengh
    *                      - QXBASE64_BOGUS_STRING : Invalid chars in string
    */

  int32 DecodeBase64(const char *cpcIn, char *pcOut);

  /** EncodeBase64
    * This function is used to Encode the given string.
    * 
    * @param   cpcIn     - Input string to be Encoded
      * @param   nInLen    - Length of the Input String
      * @param   pcOut     - Output string to place the encoded string
    * 
    * @return  IxChar    - Return a pointer to the output string OR
    *                      NULL if output string pointer is not valid.
      *
    */

  char  *EncodeBase64(const char *cpcIn, int32  nInLen, char *pcOut);
   
  private: // data members
   
};
#endif //_cplusplus

#endif // __QXBASe64_H__
