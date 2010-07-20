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
#include "camsensor.h"
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

int SoftDSP_Start(const camsensor_static_params_type *camsensor_params)
{
    return 0;
}

int SoftDSP_Stop(void)
{
    return 0;
}

int SoftDSP_Preview(CAMSoftDSP_MsgCBType cb)
{
    return 0;
}

int SoftDSP_Capture(CAMSoftDSP_MsgCBType cb)
{
    return 0;
}

int SoftDSP_Ack1(void *pBuff)
{
    return 0;
}

#endif // CAMERA_USES_SOFTDSP

