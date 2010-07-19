/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  Camera QDSP Interface

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2007 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                       INCLUDE FILES FOR MODULE


===========================================================================*/
#include "camqdsp.h"
#include "camerai.h"

#ifdef CAMERA_USES_SOFTDSP
#include "softdsp.h"

static void SoftDSP_FrameISR(void)
{
}

void SoftDSP_Init(void)
{
}

void SoftDSP_Exit(void)
{
}

int SoftDSP_Start(CAMSoftDSP_MsgCBType cb)
{
}

int SoftDSP_Stop(void)
{
}

int SoftDSP_Preview(void)
{
}

int SoftDSP_Capture(void)
{
}

int SoftDSP_Ack1(void *pBuff)
{
}

#endif // CAMERA_USES_SOFTDSP

