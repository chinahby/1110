#ifndef ACM_DIAG_H
#define ACM_DIAG_H
/** 
\file **************************************************************************
*
*                                       A C M    D I A G   H E A D E R    F I L E
*
*DESCRIPTION
* This header file contains all the definitions necessary for ATP protocol 
* layer to initialize  ACM diag dispatcher.
*  
*Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*******************************************************************************
*/
/* <EJECT> */
/**
\file ***************************************************************************
*
*                      EDIT HISTORY FOR FILE
*
*  This section contains comments describing changes made to this file.
*  Notice that changes are listed in reverse chronological order.
*   
*  $Header: //source/qcom/qct/multimedia/audio/6k/common/audiocal/QSC1100/main/latest/src/acm_diag.h#1 $ $DateTime: 2008/06/30 02:34:23 $ $Author: prashant $
*
*when         who     what, where, why<br>
*--------   ---     ----------------------------------------------------------<br>
*01/25/08   mas    created  file<br>
********************************************************************************
*/

/*
      --------------------
      |include files                |
      --------------------
 */
#include "comdef.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "hsdiag.h"
#include "audtp.h"

/*
      --------------------
      |Macros                      |
      --------------------
 */
/*ACM subsystem command ID Range (inclusive)*/

/** 
*ACM subsystem command code range start ID
*/

#define ACM_CMD_ID_START 2000 
/**
*ACM subsystem command code range End ID
*/
#define ACM_CMD_ID_END 2100 

/*
      --------------------
      |External functions      |
      --------------------
 */
/**
* FUNCTION : acm_diag_init

* DESCRIPTION :
*Registers the subsystem diag dispathcer with diag dispathcer.<br>
* Initializes the acm session.
*
* DEPENDENCIES : acm_diag_table[] should be initialized.<br>
*And acm_diag_table should have proper sub system command.code range and 
pointer to the dispatcher function.
*
* RETURN VALUE : None

* SIDE EFFECTS : None
 */
extern void acm_diag_init(void);
/**
* FUNCTION : avsACM_diag_cmd
*
* DESCRIPTION :
*This is the entry point to ACM when seen from PC.
*and is the sub system diag dispathcer for Audio Calibration manager
*Receives a diag packet ment for ACM and passes to protocol layer and returns 
the response diag packet.
*
* DEPENDENCIES :
*
* RETURN VALUE : returns response diag packet.
*In case of error, returns diag packet with error code DIAG_BAD_CMD_F
*
* SIDE EFFECTS : None
 */
extern PACKED void *  avsACM_diag_cmd 
( 
  PACKED void *request,
  word length
);

#endif
