/*******************************************************************************
;*******************************************************************************
;**                                                                           **
;**                    COPYRIGHT 2007 TEGIC COMMUNICATIONS                    **
;**                                                                           **
;**                TEGIC COMMUNICATIONS PROPRIETARY INFORMATION               **
;**                                                                           **
;**     This software is supplied under the terms of a license agreement      **
;**     or non-disclosure agreement with Tegic Communications and may not     **
;**     be copied or disclosed except in accordance with the terms of that    **
;**     agreement.                                                            **
;**                                                                           **
;*******************************************************************************
;**                                                                           **
;**     FileName: t9lang.c                                                    **
;**                                                                           **
;**  Description: File to include T9 language database files and access       **
;**               function                                                    **
;**                                                                           **
;*******************************************************************************
;******* 10 ****** 20 ****** 30 ****** 40 ****** 50 ****** 60 ****** 70 *******/


#include "t9.h"
//#include "custcs01.h"
#include "t9gen.h"       /* Pull in main access function module */
#if defined(T9CHINESE)
#include "t9cgen.c"
#endif //#if defined(T9CHINESE)

#ifdef FEATURE_LANG_ENGLISH
#include "l0109b00.c"     /* English */
#include "l0109b01.c"
#include "l0109b02.c"
#endif //FEATURE_LANG_ENGLISH

#ifdef FEATURE_LANG_FRENCH
#include "l0112b00.h"     /* French */
#include "l0112b01.h"
#include "l0112b02.h"
#include "l0112b03.h" 
#endif //FEATURE_LANG_FRENCH

//Gemsea 20080228 add
#ifdef FEATURE_LANG_THAI
#include "l0130b00.c"     /* Thai */
#include "l0130b01.c"
#include "l0130b02.c"
#include "l0130b03.c"
#endif //FEATURE_LANG_THAI
//Gemsea 20080228 end

#ifdef FEATURE_LANG_SPANISH
#include "l0110b00.c"     /* Spanish */
#include "l0110b01.c"
#include "l0110b02.c"
#include "l0110b03.c"
#endif //FEATURE_LANG_SPANISH

#ifdef FEATURE_LANG_INDONESIAN
#include "l0133b00.c"     /* Indonesian */
#include "l0133b01.c"
#include "l0133b02.c"
#include "l0133b03.c"
#include "l0133b04.c"
#endif //FEATURE_LANG_INDONESIAN

// Gemsea add the code on 080621
#ifdef FEATURE_LANG_PORTUGUESE
#include "l0122b00.c"     /* Portuguese */
#include "l0122b01.c"
#include "l0122b02.c"
#endif //FEATURE_LANG_PORTUGUESE
// Gemsea add end

// Gemsea add the code on 080819
#ifdef FEATURE_LANG_HEBREW
#include "l0113b00.c"     /* Hebrew */
#include "l0113b01.c"
#include "l0113b02.c"
#include "l0113b03.c"
#include "l0113b04.c"
#include "l0113b05.c"
#include "l0113b06.c"
#include "l0113b07.c"
#include "l0113b08.c"
#endif //FEATURE_LANG_HEBREW
// Gemsea add end

// Gemsea add the code on 080822
#ifdef FEATURE_LANG_ARABIC
#include "l0101b00.c"     /* Arabic */
#include "l0101b01.c"
#include "l0101b02.c"
#include "l0101b03.c"
#include "l0101b04.c"
#include "l0101b05.c"
#include "l0101b06.c"
#include "l0101b07.c"
#include "l0101b08.c"
#include "l0101b09.c"
#endif //FEATURE_LANG_ARABIC
// Gemsea add end

#ifdef FEATURE_LANG_TCHINESE
#include "l0104b00.c"     /* Traditional Chinese  */
#include "l0104b01.c"
#include "l0104b08.c"
#include "l0104b09.c"
#include "l0104b10.c"
#include "l0104b17.c"
#include "l0104b18.c"
#include "l0104b25.c"
#include "l0104b26.c"
#include "l0104b27.c"
#include "l0104b37.c"
#include "l0104b41.c"
#include "l0104b42.c"
#include "l0104b43.c"
#include "l0104b46.c"
#include "l0104b47.c"
#include "l0104b48.c"
#include "l0104b49.c"
#include "l0104b59.c"
#include "l0104b60.c"
#include "l0104b64.c"
#endif //FEATURE_LANG_TCHINESE

#ifdef FEATURE_LANG_CHINESE
#include "l0204b00.c"     /* Simplified Chinese  */
#include "l0204b08.c"
#include "l0204b09.c"
#include "l0204b17.c"
#include "l0204b25.c"
#include "l0204b37.c"
#include "l0204b41.c"
#include "l0204b42.c"
#include "l0204b43.c"
#include "l0204b46.c"
#include "l0204b47.c"
#include "l0204b48.c"
#include "l0204b59.c"
#include "l0204b60.c"
#include "l0204b64.c"
#endif //FEATURE_LANG_CHINESE
/*-----------------------------------< eof >----------------------------------*/
