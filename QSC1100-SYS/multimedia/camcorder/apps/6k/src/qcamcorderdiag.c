/*===========================================================================

FILE:
    QcamcorderDiag.c
    
Description:
    This file contains the functions used by the DIAG automation
    interface. otice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/camcorder/apps/6k/main/latest/src/qcamcorderdiag.c#12 $

 when       who     Description
 --------   -----   ----------------------------------------------------------
 02/20/09   girip   Code change to accomodate the change of unit of measurement of "m_dwTotalTime" in structure "Qcamcorder" 
                    from seconds to milliseconds. 
 11/03/08   sanal   Workaround for handling slow arrival AEE Callbacks
 10/17/08   sanal   Transcoder Target brand 3g2 frag added
 08/27/08   ps      Replaced GETTIMESECONDS API calls with GETUPTIMEMS as it doesnt need network to be latched for correct time.
 08/13/08    sanal   Added support for polling while doing video edit and cut merge
 07/28/08   ps      Sent Delayed Response to the automation client for Transcoding. 
 07/22/08   ps      Error reporting when ".info" file is not present for raw mode encoding.
 07/04/08   ps      Removed RVCT compiler warnings
 07/03/08   sanal   Featurized transcoder and audiodub refernces
 06/30/08   yogi    Poll status check is now a function of qcamcorder app state.
 02/11/08   sanal   Adding Idiag support for frames dropped in a recorded clip
 12/27/07   yogi    Fixed a bug in FILE MERGE & RAW_ENCODE IDIAG code. 
 12/11/07   yogi     Added IDIAG for FILE MERGE and max zoom supported 
 11/29/07   yogi     Added IDIAG for SKM file format and fixed a bug for 
                     audio only diag support 
 11/27/07   yogi    Fixed the fragile code while extracting the raw file name
 11/19/07   dhaval  Removed compilation warnings
 07/24/07   yogi    Idiag support for Audio only Recording.               
 07/19/07   dhaval  Idiag support for max bitrate, dynamic bitrate and FTL
 03/06/07   jdas     Removed Banned API
 02/22/07   Gopi    Idiag Support for setting bit rate values.
 12/14/06   Nag     Added fixes for setting K3g brand
 11/27/06   Nag     Added fixes for Smooth Zoom,Setting of target video format and 
                    Running transcoder without an reset of target
 10/17/06   Nag     Modified code to fix Idiag support for Play_Pause
 10/13/06   Nag     Removed an event that was being treated as an error in Automation Script
                    while starting Transcoding 
 10/06/06   Nag     Added code to send diag response in case of QCAMCORDER_DIAG_EDIT_PAUSE  
 09/22/06   Pavan   Added support for new options in transcoder 
 09/15/06   Nagesh  Added support to change context from Album to preview 
                    when the app is in Edit screen 
 09/01/06   kadhir  Removed unwanted Cross Compilation Dependency 
                    When qcamcorder Feature Disabled 
 06/24/06   dhaval  Fixed support for delete all clips from FLASH and MMC
 05/31/06   kusuma  Changed the prototype of Qcamcorder_Transcode to support IDIAG 
                    start transcode to send response when it is completed
 05/23/06   kusuma  Added support to smooth zoom
 02/03/06   Nagesh  Added support to retreive properties of recorded clips
 01/31/06   Gopi    Added Support for Raw Mode Encoding
 12/21/05   cl      Fixed support for play all and delete clip IDIAG features. 
 10/28/05   pavan   Added support for deleting recorded clips
 10/27/05   dhvl    Added support to play all clips
 09/30/05   dw      Fixes to enable MMC/FLASH memory usage payload
 09/14/05   dw      Added support for retrieving MMC/FLASH memory usage
 09/01/05   dw      Added support for antibanding and transcoder brand feature 
 09/01/05   gm      Fix to enable playback of files through Automation
 08/11/05   dw      Added timer fix for Telop text recording
 08/02/05   dw      Modified IDIAG support for Cut and Merge feature 
 05/17/05   dw      Added support for new video format menu 
 19 July04  sbhide  Initial version of the file
 14 Aug04   sbhide  added record and replay options
 1  Sept04  sbhide  Added edit, transcode and gps operations
============================================================================*/
#include "AEEstd.h"
#include "qcamcorderdiag.h"
#include "qcamcorder.h"
#include "Qcamcorder.bid"
#ifdef FEATURE_APP_QCAMCORDER

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
#endif /* FEATURE_APP_QCAMCORDER */
