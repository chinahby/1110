
#ifndef CUSTSW021_220X176_H
#define CUSTSW021_220X176_H
/* ========================================================================
FILE: CUSTSBNAALZ

Copyright (c) 2001-2010 by QUALCOMM Incorporated.  All Rights Reserved.        
=========================================================================== */
#ifndef CUST_EDITION
#define CUST_EDITION
#endif

#ifdef CUST_EDITION
#define FEATURE_VERSION_W021_220X176

#ifdef USES_DS_1536
#define CLKRGM_INCLUDE_TD
#endif

//Add by zzg 2010_10_13
#define FEATURE_KEYPAD_5_BY_5_MATRIX	//KEYPAD_5X5
//Add End

#define FEATURE_LANG_INDONESIAN //add by yangdecai 2010/06/22
#define FEATURE_ALL_KEY_PAD
#endif

#include "custw021.h"


#endif /* CUSTSW021_220X176_H */




