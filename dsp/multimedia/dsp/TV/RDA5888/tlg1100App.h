#ifndef _TLG1100APP_H_
#define _TLG1100APP_H_
#include "tlg1100api.h"


#ifdef __cplusplus
extern "C" {
#endif

#define xSUPPORT_STRUCT
#ifdef SUPPORT_STRUCT
/** Pointer to a tlg_channel_t.
 * @typedef p_tlg_channel_t
 */
typedef struct tlg_channel_s {

    int     chn;
    uint32  hz;
    int     bw;
    int     chstd;

} tlg_channel_t, *p_tlg_channel_t;
#define CHN_S .chn
#define HZ_S  .hz
#define BW_S  .bw
#define STD_S .chstd
#else
/*
 * moving to no structures since some C compilers don't support them
 */
#define tlg_channel_t(a)    uint32 a[][4]
#define p_tlg_channel_t(a)  uint32 (*a)[4]
#define CHN_S [0]
#define HZ_S  [1]
#define BW_S  [2]
#define STD_S [3]
#endif


#define TLG_NEW_INIT        (0) /*!< indicates first Init of TLG1100        */
#define TLG_INIT_DEEP_SLEEP (1) /*!< indicates re-init from deep sleep      */


/** Initialize the TLG1100 to be ready to perform channel changes.
 *
 * This procedure performs all tasks necessary to initialize the TLG1100
 * chip and have it perform calibration. Upon return from this procedure the
 * TLG1100 is ready to perform channel changes.
 */
TLG1100DLL_API int TLGAPP_Init(uint32 base_addr);


/** Setup channel mode, video standard and bandwidth for channel.
 *
 * This procedure checks the input parameters against the currently 
 * configured values and, if necessary, sets the TLG100 to these values.
 *
 * @param mode          The desired mode. Valid values are:
 *                      - TLG_MODE_ANALOG_TV
 *                      - TLG_MODE_FM_RADIO
 * @param std           The desired video standard. Valid values are:
 *                      - TLG_VSTD_NTSC_M
 *                      - TLG_VSTD_PAL_M 
 *                      - TLG_VSTD_PAL_B
 *                      - TLG_VSTD_PAL_B1
 *                      - TLG_VSTD_PAL_D
 *                      - TLG_VSTD_PAL_D1
 *                      - TLG_VSTD_PAL_G
 *                      - TLG_VSTD_PAL_H
 *                      - TLG_VSTD_PAL_K
 *                      - TLG_VSTD_PAL_N
 *                      - TLG_VSTD_PAL_I
 *                      - TLG_VSTD_PAL_NC
 * @param bw            The new bandwidth setting. Values are:
 *                      - TLG_BW_5
 *                      - TLG_BW_6
 *                      - TLG_BW_7
 *                      - TLG_BW_8
 * 
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLG1100DLL_API int TLGAPP_ChannelSetup(int mode, int std, int bw);

/** Set the channel map for the system along with its default settings.
 *
 * This procedure sets the default channel map for the system as well as
 * checking its mode, video standard and bandwidth settings and updating 
 * the TLG1100 registers if necessary. Note, if the channel map is a FM Radio
 * channel map, TLGAPP_SetChannelMapFM() should be called instead as this
 * will also set the proper slope and offset settings for the system.
 *
 * @param map       Pointer to an array of tlg_channel_t entries. The list
 *                  should be terminated with an entry whose .chn field is set
 *                  to 0.
 * @param mode      the mode for the channel map. See TLGAPP_ChannelSetup() for
 *                  a description of this parameter.
 * @param std       The default video standard for this channel map. See
 *                  TLGAPP_ChannelSetup() for a descrition of this parameter.
 *                  TLGAPP_ChannelSetup() for a descrition of this parameter.
 * @param bw        The default bandwidth for this channel map. See
 * 
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLG1100DLL_API int TLGAPP_SetChannelMap(p_tlg_channel_t(map), int mode, int std, int bw);

TLG1100DLL_API int TLGAPP_SetChannelMapExt(int area);


/** Tune to the specified channel.
 *
 * This procedure will scan the default channel map for the specified channel
 * and then perform a TLG_ScanTVChn() or TLG_ScanFMChn() depending on what
 * mode has been set.
 *
 * @param chn   channel number of the entry in the channel map to change to.
 *
 * @retval  TRUE    channel change succeeded and frequency is locked.
 * @retval  FALSE   channel change failed.
 */
TLG1100DLL_API int TLGAPP_SetChannel(int chn);


/** Scan the default channel map and return the channels that locked.
 *
 * This procedure scans the default channel map and populates the supplied 
 * channel map with all entries that locked. It will terminate the map with
 * an entry with map[i].chn = 0.
 *
 * @param tlg_scan_map  map to fill with locked channels.
 * @param max_scan      total number of channel entries in tlg_scan_map
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 * @sa TLGAPP_ScanChanFound
 */ 
TLG1100DLL_API void TLGAPP_ScanChannels(p_tlg_channel_t(tlg_scan_map), int max_scan);

/** Turn on Test Pattern.
 *
 * This procedure will turn on the test pattern after making sure the TLG1100
 * is properly initialized.
 */
TLG1100DLL_API void TLGAPP_TurnOnTestPattern(void);

/** Turn off Test Pattern.
 *
 * This procedure will turn off the test pattern.
 */
TLG1100DLL_API void TLGAPP_TurnOffTestPattern(void);


extern TLG1100DLL_API p_tlg_channel_t(p_tlg_cur_map);/*!< Current Default Channel Map*/
extern int              tlg_cur_mode;       /*!< current Mode               */
extern int              tlg_cur_std;        /*!< Current Video Std          */
extern int              tlg_init_done;      /*!< Flag Init called           */
extern uint32           tlg_i2c_addr;       /*!< i2c address of TLG1100     */

/*@}*/

#ifdef __cplusplus
};
#endif


#endif /* _TLG1100APP_H_*/
