#ifndef __VHOSTCTRL_H_
#define __VHOSTCTRL_H_

#include "mmp_rdk.h"


#ifndef ASSERT
#if DRV_ASSERT_ENABLE
#define ASSERT(_x)  {MMD_PRINTF(("ASSERT 0x%x ,Line %d\r\n",_x,__LINE__));}
#else
#define ASSERT(_x)
#endif
#endif

void Vhost_WriteReg(MMD_U32 dAddr, MMD_U32 dData);
MMD_U32 Vhost_ReadReg(MMD_U32 dAddr);
void Vhost_Delay(MMD_U32 cnt);


#endif
