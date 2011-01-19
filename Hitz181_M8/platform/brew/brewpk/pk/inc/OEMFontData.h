#ifndef __OEMFONTDATA_H__
#define __OEMFONTDATA_H__

#ifndef AEE_SIMULATOR
#include "comdef.h"
#else
#include "AEEComdef.h"//VC defnitions
#endif

extern const byte ChineseNormalFontBuf[];
extern const word gbIndex[];
extern const byte gbWidth[];


extern const byte BigNumFontBuf[];

//#ifdef FEATURE_FONT_12PIX
extern const byte ChineseNormalFontBuf12[];
extern const word gbIndex12[];
extern const byte gbWidth12[];
//#endif //FEATURE_FONT_12PIX

#define FIXED_WIDTH_START_INDEX 0x0141   // Start index for fixed font width 
#define FIXED_WIDTH_END_INDEX   0xAEEB   //fixed font width
#define FIXED_WIDTH_EXTEND_INDEX 0xAEFA

#define BIG_NUM_ASCENT 29
#define BIG_NUM_DESCENT 3
#define BIG_NUM_HEIGHT 32
#define BIG_NUM_WIDTH  16
#define BIG_NUM_BYTES_PER_CHAR  64
#define BIG_NUM_BYTES_PER_ROW   2
#endif //__OEMFONTDATA_H__
