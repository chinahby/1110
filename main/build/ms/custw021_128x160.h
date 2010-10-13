
#ifndef CUSTSW021_128X160_H
#define CUSTSW021_128X160_H
/* ========================================================================
FILE: CUSTSBNAALZ

Copyright (c) 2001-2010 by QUALCOMM Incorporated.  All Rights Reserved.        
=========================================================================== */
#ifndef CUST_EDITION
#define CUST_EDITION
#endif

#ifdef CUST_EDITION
#define FEATURE_VERSION_W021_128x160

#define FEATURE_SPN_FROM_BSMCCMNC
#define FEATURE_VERSION_W021
#define FEATURE_LANG_INDONESIAN //add by yangdecai 2010/06/22

#ifdef USES_DS_1536
#define CLKRGM_INCLUDE_TD
#endif

#endif

#include "custw021.h"

#endif /* CUSTSW021_128X160_H */




