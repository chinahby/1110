#define TLG_1120
#include "tlgPlatform.h"
#ifdef TLG_1100
#include "tlg1100Hal.h"
#include "tlg1100Api.h"
#include "tlg1100App.h"
#if TLG_VERSION_MAJOR == 1 && TLG_VERSION_MINOR == 9 && TLG_VERSION_PATCH == 3
#include "tlg1100i2c.h"
#elif TLG_VERSION_MAJOR == 1 && TLG_VERSION_MINOR == 10 && TLG_VERSION_PATCH == 0
#include "tlgi2c.h"
#else
// Unkown version
#endif


#else
 
#ifdef TLG3100
#include "tlg3100Config.h"
#include "tlg3100i2c.h"
#include "tlg3100Hal.h"
#include "tlg3100Api.h"
#include "tlg3100App.h"
#else
#ifdef TLG_1120 
#include "tlg_bit_definition.h"
#include "tlg_error.h"
#include "tlg_defs.h"
#include "tlgConfig.h"
#include "tlgi2c.h"
#include "tlg1120Hal.h"
#include "tlg1120Api.h"
#include "tlg1120App.h"
#endif	//TLG_1120
#endif	//TLG_3100

#endif	//TLG1100


