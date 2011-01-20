#ifndef _TLG_INCLUDE_H_
#define _TLG_INCLUDE_H_

 
#include "tlg_bit_definition.h"
#include "tlg_error.h"
#include "tlg_defs.h"
#include "msg.h"
#include "tlgConfig.h"
#ifndef TLG_TOOL
#include "tlgi2c.h"
#else
#include "..\tlg1120dll\tlg1100i2c_tool.h"
#endif

#if TLG_CHIP_TYPE == TLG_CHIP_TYPE_TLG1100_1
    #include "tlg1100Hal.h"
    #include "tlg1100Api.h"
    #include "tlg1100App.h"
#elif TLG_CHIP_TYPE == TLG_CHIP_TYPE_TLG1150_1
    #include "tlg1150Hal.h"
    #include "tlg1150Api.h"
    #include "tlg1150App.h"
#elif TLG_CHIP_TYPE == TLG_CHIP_TYPE_TLG1155_1
    #include "tlg1155Hal.h"
    #include "tlg1155Api.h"
    #include "tlg1155App.h"
#elif TLG_CHIP_TYPE == TLG_CHIP_TYPE_TLG2300_1
    #include "tlg2300Hal.h"
    #include "tlg2300Api.h"
    #include "tlg2300App.h"
#elif TLG_CHIP_TYPE == TLG_CHIP_TYPE_TLG1120_1
    #include "tlg1120Hal.h"
    #include "tlg1120Api.h"
    #include "tlg1120App.h"
#endif

#endif
