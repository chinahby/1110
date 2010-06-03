/*===========================================================================

FILE:
    QcamcorderDiag.h

Description:
    This file provides the required defintions for the Qcamcorder Diag
    interface for automation.

$Header: //source/qcom/qct/multimedia/camcorder/apps/6k/main/latest/inc/qcamcorderdiag.h#6 $

 when        who     Description
 --------    -----   ----------------------------------------------------------
 11/24/08   ps       Removed compiler warnings 
 10/23/08   ps       Removed compiler Superfluous warnings 
 08/13/08   sanal    Added support for polling while doing video edit and cut merge
 07/04/08   ps      Removed RVCT compiler warnings
 05/26/08   yogi     Merging changes from qcamcorderdiag.h inorder to fix the 
                     macro mismatch between 6k and 7k 
 02/11/08   sanal    Adding Idiag support for frames dropped in a recorded clip
 12/17/07   yogi     Qcamcorder diag message classification. 
                     Fixed a bug in FILE_MERGE/RAW_ENCODE IDIAG code.
 12/11/07   yogi     Added IDIAG for FILE MERGE and max zoom supported 
 11/29/07   yogi     Added IDIAG for SKM file format and fixed a bug for 
                     audio only diag support 
 11/27/07   yogi     Fixed the fragile code while extracting the raw file name
 07/24/07   yogi     Idiag support for audio only recording.               
 07/19/07   dhaval   Idiag support for max bitrate, dynamic bitrate and FTL
 02/22/07   Gopi     Idiag Support for setting bit rate values.
 09/22/06   Pavan    Added support for new options in transcoder 
 02/03/06   Nagesh   Added support to retreive properties of recorded clips
 01/31/06    Gopi    Added Support for raw mode encoding.
 12/21/05    cl      Rearranged Qcamcorder_DiagEditOperationType enums to sync 
                     with ASIA
 10/28/05    pavan   Added support for Delete Recorded Clip feature
 09/01/05    dw      Added support for antibanding and transcoder brand feature 
 08/11/05    dw      Added timer fix for Telop text recording 
 08/02/05    dw      Modified IDIAG support for Cut and Merge feature
 06/01/05    dw      Rearranged encoding frame size enums to sync with ASIA
 05/17/05    dw      Added support for new video format menu
 29 Apr  05  dw      Ported Cut and Merge IDIAG support from msmshared branch.
 19 July 04  sbhide  Initial version of the file
 02 Sept 04  sbhide  changed the comment stypes as per ali's review comments.
============================================================================*/
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
