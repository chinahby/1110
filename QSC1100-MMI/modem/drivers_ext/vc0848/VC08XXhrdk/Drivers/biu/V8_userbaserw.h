#ifndef _V8_TRM_USERBASERW_H_
#define _V8_TRM_USERBASERW_H_

#ifdef __cplusplus
extern "C" {
#endif

//#include "../../common/mmd_user_depend.h"

#define WriteMemory8(addr, val)         (*((MMD_VU08 *)addr)) = ((MMD_U08)val)
#define ReadMemory8(addr)               (*((MMD_VU08 *)addr))
#define WriteMemory16(addr, val)        (*((MMD_VU16 *)addr)) = ((MMD_U16)val)
#define ReadMemory16(addr)              (*((MMD_VU16 *)addr))

#define HOSTSETADDR8( addr, val )   WriteMemory8(addr, val)
#define HOSTSETVAL8( addr, val )    WriteMemory8(addr, val)
#define HOSTGETVAL8( addr )         ReadMemory8(addr)
#define HOSTSETADDR16( addr, val )  WriteMemory16(addr, val)
#define HOSTSETVAL16( addr, val )   WriteMemory16(addr, val)
#define HOSTGETVAL16( addr )        ReadMemory16(addr)


#ifdef __cplusplus
}
#endif

#endif

