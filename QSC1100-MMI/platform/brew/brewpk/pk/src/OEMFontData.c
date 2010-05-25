#include "OEMFontData.h"

//const byte ChineseNormalFontBuf[] =
//{
//#include "FontDB//font.i"
//};
//
//const byte gbWidth[] =
//{
//#include "FontDB//width.i"
//};
//
//const word gbIndex[] = {
//#include "FontDB//index.i"
//};

#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE

const unsigned char gbFont_Arabic_Hebrew[] = {
#include "FontDB//Arabic_Hebrew_14x14p.i"
};

const byte BigNumFontBuf[] = 
{
#include "FontDB//bignum.i"
};

#else  //#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE

const byte BigNumFontBuf[] = 
{
#include "FontDB//bignum.i"
};

#ifdef FEATURE_FONT_12PIX1

const byte ChineseNormalFontBuf12[] =
{
#include "FontDB//font12.i"
};

const word gbIndex12[] = 
{
#include "FontDB//index12.i"
};

const byte gbWidth12[] = {
#include "FontDB//width12.i"
};

#elif defined(FEATURE_CARRIER_CHINA_TELCOM)

const byte ChineseNormalFontBuf12[] = 
{
#include "FontDB//font14_China.i"
};

const word gbIndex12[] = 
{
#include "FontDB//index14_China.i"
};

const byte gbWidth12[] = 
{
#include "FontDB//width14m_China.i"
};

#else

const byte ChineseNormalFontBuf12[] =
{
#include "FontDB//font14.i"
};

const word gbIndex12[] = 
{
#include "FontDB//index14.i"
};

const byte gbWidth12[] = 
{
#include "FontDB//width14m.i"
};

#endif //FEATURE_FONT_12PIX

#endif //#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
