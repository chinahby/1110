/* zifile.h 
*/
/*****************************************************************************
* COPYRIGHT ZI AND SUBJECT TO CONFIDENTIALITY RESTRICTIONS                   *
*                                                                            *
* This file is the confidential and proprietary property of Zi Corporation   *
* of Canada, Inc. ("Zi") or one of its affiliates.                           *
**************************************************************************** */
#ifndef  _ZI_FILE_H_
#define  _ZI_FILE_H_

#include "ezicmn.h"


/*****************************************************************************
 * callback function definition
 *****************************************************************************/
 
 
typedef int (* ZiReadDataCallback)(void *, long, char *, int  );

/******************************************************************************
 * structure which need to pass in for engine initialization
 ******************************************************************************/
typedef struct _ZI_FILE_CONTEXT
{
    ZiReadDataCallback pZiCallbackFunction;
    
    PZI8VOID           pZiExtraCallbackInfo;
    
} ZI_FILE_CONTEXT;
typedef struct _ZI_FILE_CONTEXT ZI8RAMPOINTER PZI_FILE_CONTEXT;


/*****************************************************************************
 * the following functions are used internally used by the engine 
 *****************************************************************************/

ZI8FUNCTION PZI8VOID Zi8FileOpen(  PZI8VOID  pLangInfo);

ZI8FUNCTION ZI8USHORT Zi8ReadFile( PZI8VOID  pFileHandle,
                                   PZI8UCHAR pBuffer,
                                   ZI8LONG   lPos,
                                   ZI8USHORT uSize );

ZI8FUNCTION ZI8BOOL Zi8FileClose(  PZI8VOID pFileHandle );

#endif /* #ifndef  _ZI_FILE_H_ */
