#ifndef GSM1XACTVFILE_H
#define GSM1XACTVFILE_H 1

/*======================================================================
FILE: gsm1xactvfile.h

DESCRIPTION:

     Defines interfaces for storing/retrieving information to/from 
     EFS files.  The mehods here encapsulate the interfaction
     with EFS system which allows the application to be independent
     of storage methods used.
     
              (c) COPYRIGHT 2003 QUALCOMM Incorporated.
                         All Rights Reserved.
                      QUALCOMM Proprietary/GTDR
======================================================================*/

/*----------------------------------------------------------------------
 Include files 
----------------------------------------------------------------------*/
/* None.*/



/*----------------------------------------------------------------------
  Typedefs and constants.
----------------------------------------------------------------------*/
#define  GSM1XACTVFILE_SYSTEM_PRL     (0)
#define  GSM1XACTVFILE_USER_PRL       (1)
#define  GSM1XACTVFILE_UIM_ICCID      (2)
#define  GSM1XACTVFILE_PRLGEN_METHOD  (3)

#define  GSM1XACTVFILE_EFS_ITEM_CNT   (4)

#define  GSM1XACTVFILE_FILE_NAME_MAX  (20)



typedef  uint32     gsm1xactv_ValueType;

/*----------------------------------------------------------------------
 Global Constant Declarations
----------------------------------------------------------------------*/
/* None.*/

/*----------------------------------------------------------------------
 Global Data Declarations
----------------------------------------------------------------------*/
/* None.*/

/*----------------------------------------------------------------------
 Function Prototypes
----------------------------------------------------------------------*/
boolean gsm1xactvfile_GetValue
(
    IShell                     *shellPtr,
    gsm1xactv_ValueType         valueType, 
    uint32                      valueMaxLen,
    char                       *valuePtr, 
    uint32                      *valueLen
);

boolean gsm1xactvfile_SetValue
(
    IShell                     *shellPtr,
    gsm1xactv_ValueType         valueType, 
    char                       *valuePtr, 
    uint32                      len
);


#endif /* GSM1XACTVFILE_H */
