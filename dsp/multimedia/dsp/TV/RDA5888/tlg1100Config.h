#ifndef _TLG1100_CONFIG_H_
#define _TLG1100_CONFIG_H_
/****************************************************************************/
/*!@file  tlg1100Config.h
 *
 * @brief Compile-time configuration declarations for the TLG API.
 *
 * This file contains the compile time declarations for the TLG1100 Ultra Low 
 * Power NTSC/PAL Tuner/Decoder chip support library.
 *
 *
 ****************************************************************************/
/* Revision Information:
 *
 * $Revision: 1.4 $
 *
 * (c) 2007 Telegent Systems
 ****************************************************************************/

#ifdef TLG_USE_DLL
    #ifdef TLG1100DLL_EXPORTS
        #define TLG1100DLL_API __declspec(dllexport)
    #else
        #define TLG1100DLL_API __declspec(dllimport)
    #endif /* TLG1100DLL_EXPORTS */
#else /* !TLG_USE_DLL */
    #define TLG1100DLL_API
#endif /* TLG_USE_DLL */


#ifdef __cplusplus
extern "C"  {
#endif

#ifdef __cplusplus
};
#endif

#endif /* _TLG1100_CONFIG_H_ */
