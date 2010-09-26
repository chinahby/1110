#ifndef _TLG1120APP_H_
#define _TLG1120APP_H_
/****************************************************************************/
/*!@file  tlg1120App.h
 *
 * @brief Example application code for using TLG1120 API.
 *
 * This file contains the api definitions for the TLG1120 Example Application
 * procedures. These procedures assume that TLG_EMBEDDED_TIMERS is defined.
 * The simplest application that can be written is (assuming TLGAPP_Init() has
 * been customized correctly):
 * 
  @verbatim
  main()
  {
       TLGAPP_TurnOnTestPattern();
  }
 
  OR
 
  main()
  {
       TLGAPP_Init(TLGI2C_DEFAULT_BASE_ADDR);
       TLGAPP_SetChannelMapNTSC();
       TLGAPP_SetChannel(48);
  }
  @endverbatim
 *
 *
 ****************************************************************************/
/* Revision Information:
 *
 * $Revision: 1.14.2.4 $
 *
 * (c) 2008 Telegent Systems
 ****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
#include "tlgconfig.h"
#if 1//TLG_CHIP_TYPE == TLG_CHIP_TYPE_TLG1120_1
    
/** Example data structure of an entry in a channel map.
 * @typedef tlg_channel_t
 *
 * This datastructure defines an example format for entries in a channel map.
 * A channel map is defined as an array of these structures that is terminated
 * with an entry whose "chn" field is set to "0". For instance:
 *
 * tlg_channel_t small_ntsc_chn_map[] = {
 *
 *  {2, 101000000, 6},
 *
 *  {0, 0, 0}
 *
 * };
 *
 * @param chn   integer value to describe the channel number (e.g. 2,3...).
 * @param hz    center frequency in Hz of the channel.
 * @param bw    bandwidth in MHz of the channel. Values are (5,6,7,8)
 * @param chstd channel standard 
 *               TLG_VSTD_NTSC_M 
 *               TLG_VSTD_PAL_M  
 *               TLG_VSTD_PAL_B  
 *               TLG_VSTD_PAL_B1 
 *               TLG_VSTD_PAL_D  
 *               TLG_VSTD_PAL_D1 
 *               TLG_VSTD_PAL_G  
 *               TLG_VSTD_PAL_H  
 *               TLG_VSTD_PAL_K  
 *               TLG_VSTD_PAL_N  
 *               TLG_VSTD_PAL_I  
 *               TLG_VSTD_PAL_NC 
 *               TLG_VSTD_SECAM_M 
 *               TLG_VSTD_SECAM_B 
 *               TLG_VSTD_SECAM_B1
 *               TLG_VSTD_SECAM_D 
 *               TLG_VSTD_SECAM_D1
 *               TLG_VSTD_SECAM_G 
 *               TLG_VSTD_SECAM_H 
 *               TLG_VSTD_SECAM_K 
 *               TLG_VSTD_SECAM_N 
 *               TLG_VSTD_SECAM_I 
 *               TLG_VSTD_SECAM_NC
 *               TLG_VSTD_SECAM_L 
 */
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

#else  /*! SUPPORT_STRUCT */

/*
 * moving to no structures since some C compilers don't support them
 */
#define tlg_channel_t(a)    uint32 a[][4]
#define p_tlg_channel_t(a)  uint32 (*a)[4]
#define CHN_S [0]
#define HZ_S  [1]
#define BW_S  [2]
#define STD_S [3]

#endif  /* SUPPORT_STRUCT */

#define TLG_NEW_INIT        (0) /*!< indicates first Init of TLG1120        */
#define TLG_INIT_DEEP_SLEEP (1) /*!< indicates re-init from deep sleep      */

/** Restart the TLGAPP API.
 *
 * This procedure resets all the TLGAPP API state variables so that the
 * TLG1120 may be restarted using TLGAPP_Init(). An external chip reset
 * procedure may also be employed to physically reset the chip as well.
 *
 */
TLGDLL_API void TLGAPP_Restart(void);

/** Initialize the TLG1120 to be ready to perform channel changes.
 *
 * This procedure performs all tasks necessary to initialize the TLG1120
 * chip and have it perform calibration. Upon return from this procedure the
 * TLG1120 is ready to perform channel changes.
 */
TLGDLL_API int TLGAPP_Init(uint32 base_addr);

/** USER CUSTOMIZABLE procedure to set up audio and video portions of TLG1120.
 *
 * This procedure is a USER CUSTOMIZABLE procedure where all audio and video
 * initialization should be done. The code resides in tlg1120customize.c
 */
TLGDLL_API void TLGAPP_InitVideoAndAudio(void);

/** Initialize registers to their desired default values.
 *
 * This procedure loads selected registers to their default values as
 * defined in the tlg1120.ini file. It is called by TLGAPP_Init() and should
 * not need to be called by itself.
 *
 * @sa TLGAPP_Init.
 */
TLGDLL_API void TLGAPP_Ini_Reg(int start);

/** Setup channel mode, video standard and bandwidth for channel.
 *
 * This procedure checks the input parameters against the currently 
 * configured values and, if necessary, sets the TLG100 to these values.
 *
 * @param mode          The desired mode. Valid values are:
 *                      -  TLG_MODE_NONE              
 *                      -  TLG_MODE_ANALOG_TV_UNCOMP  
 *                      -  TLG_MODE_ANALOG_TV_COMP    
 *                      - TLG_MODE_FM_RADIO
 *                      -  TLG_MODE_DVB_T             
 *                      -  TLG_MODE_DVB_H             
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
 *                      - TLG_VSTD_SECAM_M 
 *                      - TLG_VSTD_SECAM_B 
 *                      - TLG_VSTD_SECAM_B1
 *                      - TLG_VSTD_SECAM_D 
 *                      - TLG_VSTD_SECAM_D1
 *                      - TLG_VSTD_SECAM_G 
 *                      - TLG_VSTD_SECAM_H 
 *                      - TLG_VSTD_SECAM_K 
 *                      - TLG_VSTD_SECAM_N 
 *                      - TLG_VSTD_SECAM_I 
 *                      - TLG_VSTD_SECAM_NC
 *                      - TLG_VSTD_SECAM_L 
 *                      - TLG_VSTD_PAL_SECAM_M 
 *                      - TLG_VSTD_PAL_SECAM_B 
 *                      - TLG_VSTD_PAL_SECAM_B1
 *                      - TLG_VSTD_PAL_SECAM_D 
 *                      - TLG_VSTD_PAL_SECAM_D1
 *                      - TLG_VSTD_PAL_SECAM_G 
 *                      - TLG_VSTD_PAL_SECAM_H 
 *                      - TLG_VSTD_PAL_SECAM_K 
 *                      - TLG_VSTD_PAL_SECAM_N 
 *                      - TLG_VSTD_PAL_SECAM_I 
 *                      - TLG_VSTD_PAL_SECAM_NC
 * @param bw            The new bandwidth setting. Values are:
 *                      - TLG_BW_5
 *                      - TLG_BW_6
 *                      - TLG_BW_7
 *                      - TLG_BW_8
 *                      - TLG_BW_12
 *                      - TLG_BW_15
 * 
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLGAPP_ChannelSetup(int mode, int std, int bw);

/** Set the channel map for the system along with its default settings.
 *
 * This procedure sets the default channel map for the system as well as
 * checking its mode, video standard and bandwidth settings and updating 
 * the TLG1120 registers if necessary. Note, if the channel map is a FM Radio
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
TLGDLL_API int TLGAPP_SetChannelMap(p_tlg_channel_t(map), int mode, int std, int bw);


/** Set the Channel map, mode, bandwidth, video standard to PAL-D settings.
 *
 * This procedure sets the default channel map to the PAL-D channel map and
 * sets up the mode to TLG_MODE_ANALOG_TV_UNCOMP, the standard to TLG_VSTD_PAL_D
 * and the bandwidth to TLG_BW_8 (if not already set up).
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
#ifdef  SUPPORT_CHANNEL_MAP_CHINA
TLGDLL_API int TLGAPP_SetChannelMapPALD(void);
#endif /*  SUPPORT_CHANNEL_MAP_CHINA */


/** Set the Channel map, mode, bandwidth, video standard to NTSC settings.
 *
 * This procedure sets the default channel map to the NTSC channel map and
 * sets up the mode to TLG_MODE_ANALOG_TV_UNCOMP, the standard to TLG_VSTD_NTSC_M
 * and the bandwidth to TLG_BW_6 (if not already set up).
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
#ifdef SUPPORT_CHANNEL_MAP_NTSC
TLGDLL_API int TLGAPP_SetChannelMapNTSC(void);
#endif /* SUPPORT_CHANNEL_MAP_NTSC */

/** Set the Channel map, mode, bandwidth, video standard to FM Radio settings.
 *
 * This procedure sets the default channel map to the NTSC channel map and
 * sets up the mode to TLG_MODE_FM_RADIO, the standard to TLG_VSTD_NTSC_M
 * and the bandwidth to TLG_BW_6 (if not already set up). It will also
 * perform TLG_InitScanFMChn() to obtain slope and offset settings that will
 * be used in subsequent calls to TLGAPP_SetChannel() and TLGAPP_ScanChannels().
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLGAPP_SetChannelMapFM(void);

/** Set the Channel map, mode, bandwidth, video standard to FM Radio settings
 * in USA.
 *
 * This procedure sets the USA channel map to the NTSC channel map and
 * sets up the mode to TLG_MODE_FM_RADIO, the standard to TLG_VSTD_NTSC_M
 * and the bandwidth to TLG_BW_6 (if not already set up). It will also
 * perform TLG_InitScanFMChn() to obtain slope and offset settings that will
 * be used in subsequent calls to TLGAPP_SetChannel() and TLGAPP_ScanChannels().
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLGAPP_SetChannelMapFM_USA(void);


/** Switch the default channel map to the specified channel map.
 *
 * This procedure sets the default channel map to the specified map. Note that
 * the mode, video standard and bandwidth are not changed so this procedure
 * should only be called to switch maps with the same attributes (e.g.
 * a map retured from TLGAPP_ScanChannels().
 *
 * @param map       The channel map to set the default map to.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLGAPP_SwitchChannelMap(p_tlg_channel_t(map));

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
TLGDLL_API int TLGAPP_SetChannel(int chn);

/** Tune to the specified channel using the "fast" lock detection code.
 *
 * This procedure will scan the default channel map for the specified channel
 * and then perform a TLG_FastScanTVChn() or TLG_ScanFMChn() depending on what
 * mode has been set.
 *
 * @param chn   channel number of the entry in the channel map to change to.
 *
 * @retval  TRUE    channel change succeeded and frequency is locked.
 * @retval  FALSE   channel change failed.
 */
TLGDLL_API int TLGAPP_FastSetChannel(int chn);

/** Setup TLGAPP for scanning a channel map.
 *
 * This procedure sets up TLGAPP to begin scanning the currently
 * set channel map. It also sets the map to be filled with locked
 * channels. This call should be followed by a series of 
 * TLGAPP_ScanNextChannel() ending with a call to TLGAPP_StopChannelScan()
 *
 * @param tlg_scan_map  map to fill with locked channels. If no scan map
 *                      is to be used, this parameter may be set to NULL.
 * @param max_scan      total number of channel entries in tlg_scan_map. If
 *                      no scan map is being used, this may be set to 0.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 * @sa TLGAPP_StopChannelScan
 * @sa TLGAPP_ScanNextChannel
 */ 
TLGDLL_API int TLGAPP_StartChannelScan(p_tlg_channel_t(scan_map), int max_scan);

/** Scan the next channel of the current channel map.
 *
 * This procedure scans the next channel in the currently configured
 * channel map. If it is locked it adds it to the scan map set up in
 * TLGAPP_StartChannelScan(). 
 *
 * @param cur_chn_i     index of the next channel map record to scan
 * @param ret_locked    returned value indicating whether channel was 
 *                      successfully locked or not.
 * @param ret_chan_i    returned value indicating the index of the channel map
 *                      entry that was locked. This is for FM where
 *                      the actual channel locked may depend on 
 *                      previously scanned channels. If the value returned 
 *                      is 0, then the end of the channel map has been reached.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 * @sa TLGAPP_StartChannelScan
 * @sa TLGAPP_StopChannelScan
 */ 
TLGDLL_API int TLGAPP_ScanNextChannel(int cur_chn, int *ret_locked, int *ret_chn);

/** Stop a previously started Scan.
 *
 * This procedure stops the current scan and writes and end record on
 * the scan list supplied in a previous TLGAPP_StartChannelScan() call.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 * @sa TLGAPP_StartChannelScan
 * @sa TLGAPP_ScanNextChannel
 */ 
TLGDLL_API int TLGAPP_StopChannelScan(void);


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
TLGDLL_API void TLGAPP_ScanChannels(p_tlg_channel_t(tlg_scan_map), int max_scan);

/** Scan the default channel map upwards starting from the given channel 
 * and stop at first channel that locked.
 *
 * This procedure seeks the next available channel in the default channel
 * map starting from the supplied channel number. Calling this procedure
 * will automatically tune to the next available channel.
 *
 * @param chn               starting channel number
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 */
TLGDLL_API void TLGAPP_SeekNextChn(int chn);

/** Scan the default channel map downwards starting from the given channel
 * and stop at first channel that locked.
 *
 * This procedure seeks the previous available channel in the default channel
 * map starting from the supplied channel number. Calling this procedure
 * will automatically tune to the previous available channel.
 *
 * @param chn               starting channel number
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 */
TLGDLL_API void TLGAPP_SeekPrevChn(int chn);

/** Turn on Test Pattern.
 *
 * This procedure will turn on the test pattern after making sure the TLG1120
 * is properly initialized.
 */
TLGDLL_API void TLGAPP_TurnOnTestPattern(void);

/** Turn off Test Pattern.
 *
 * This procedure will turn off the test pattern.
 */
TLGDLL_API void TLGAPP_TurnOffTestPattern(void);

/** Turn on Test Tone Mode and set Gain to minimum.
 *
 * This procedure will turn on the Audio Test Tone Mode if the mode is set
 * to a digital mode (TLG_MASTER, TLG_SLAVE). It will also temporarily set
 * the Audio Gain to the mininum (TLG_MIN_AUD_GAIN). Once Test Tone Mode
 * has been enabled the calls TLG_GetAudioTestToneL(), TLG_SetAudioTestToneL()
 * TLG_GetAudioTestToneR(), TLG_SetAudioTestToneR() may be used to manipulate
 * the test tones. When Test Tone Mode is disabled 
 * (TLGAPP_TurnOffTestToneMode()) the gain will be restored.
 * 
 *
 * @sa TLG_GetAudioTestToneL
 * @sa TLG_SetAudioTestToneL
 * @sa TLG_GetAudioTestToneR
 * @sa TLG_SetAudioTestToneR
 * @sa TLGAPP_TurnOffTestToneMode
 */
TLGDLL_API void TLGAPP_TurnOnTestToneMode(void);

/** Turn off Test Tone Mode and restore the Audio Gain.
 * 
 * This procedure turns the Test Tone Mode off and restores the Audio Gain to
 * the value that was set in TLGAPP_Init().
 *
 * @sa TLGAPP_Init
 */
TLGDLL_API void TLGAPP_TurnOffTestToneMode(void);

/** Switch the antenna to the proper LNA.
 *
 * This procedure provides a framework to manage an antenna switch. This may
 * be done through the GPIOs available on the TLG1120, or by some other means.
 * THIS PROCEDURE REQUIRES CUSTIMIZATION TO WORK PROPERLY.
 *
 * @param hz    The frequency, in hertz, of the channel being tuned.
 *
 * @sa TLGAPP_SetChannel
 */
TLGDLL_API void TLGAPP_SetLnaGpio(uint32 hz);

/** Perform custom actions when TLGAPP_ScanChannels() has found a new channel.
 *
 * This procedure is called by TLGAPP_ScanChannels() when it has encounterd a
 * new channel. It is intended to be customized for a given customer's product
 * implementation.
 * THIS PROCEDURE REQUIRES CUSTIMIZATION TO WORK PROPERLY.
 *
 * @param chn   The channel index of the channel map that has passed scan.
 *
 * @sa TLGAPP_ScanChannels
 */
TLGDLL_API void TLGAPP_ScanChanFound(int chn);

/** Perform custom actions before TLGAPP_ScanChannels() starts to scan a new channel.
 *
 * This procedure is called by TLGAPP_ScanChannels() before it scan a
 * new channel. It is intended to be customized for a given customer's product
 * implementation.
 * THIS PROCEDURE REQUIRES CUSTIMIZATION TO WORK PROPERLY.
 *
 * @param chn   The channel index of the channel map that has passed scan.
 *
 * @sa TLGAPP_ScanChannels
 */
/*****************************************************************************
* TLGAPP_PreScanChan
* BEGIN CUSTOM SECTION: This procedure is called before TLGAPP_ScanChannels()
*                       starts to scan a new channel.
* It may be customized for a given build.
*****************************************************************************/

TLGDLL_API void TLGAPP_PreScanChan(int chn);

/** Put the TLG1120 into low power mode.
 *
 * This procedure may be called to put the TLG1120 into low power mode
 * also called "Deep Sleep" mode. To return from this mode
 * TLGAPP_ReInitFromPowerDown() should be called. Note these calls are only
 * supported in chip revision 1.3 and later and the I2C must be configured
 * for Asynchronous mode (GPIO5 tied to ground).
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 * @sa TLG_DeepSleep
 * @sa TLG_ReturnFromDeepSleep
 * @sa TLG_ReInitFromDeepSleep
 */
TLGDLL_API int TLGAPP_PowerDown(void);

/** Return from the Power Down (Deep Sleep) state.
 *
 * This procedure powers up and re-initializes the TLG1120 after it has been
 * put into its low power state with TLGAPP_PowerDown().
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 * @sa TLG_DeepSleep
 * @sa TLG_ReturnFromDeepSleep
 * @sa TLG_ReInitFromDeepSleep
 */
TLGDLL_API int TLGAPP_ReInitFromPowerDown(void);

/** Set the Channel map, mode, bandwidth, video standard to Argentina settings.
 *
 * This procedure sets the default channel map to the PAL-NC channel map and
 * sets up the mode to TLG_MODE_ANALOG_TV_UNCOMP, the standard to TLG_VSTD_PAL_NC
 * and the bandwidth to TLG_BW_6 (if not already set up).
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
#ifdef  SUPPORT_CHANNEL_MAP_ARGENTINA
TLGDLL_API int TLGAPP_SetChannelMapArgentina(void);
#endif /* SUPPORT_CHANNEL_MAP_ARGENTINA */

/** Set the Channel map, mode, bandwidth, video standard to Brazil settings.
 *
 * This procedure sets the default channel map to the PAL-M channel map and
 * sets up the mode to TLG_MODE_ANALOG_TV_UNCOMP, the standard to TLG_VSTD_PAL_M
 * and the bandwidth to TLG_BW_6 (if not already set up).
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
#ifdef SUPPORT_CHANNEL_MAP_BRAZIL
TLGDLL_API int TLGAPP_SetChannelMapBrazil(void);
#endif /* SUPPORT_CHANNEL_MAP_BRAZIL */

/** Set the Channel map, mode, bandwidth, video standard to UK,
 *  Ireland, South Africa, Hong Kong settings.
 *
 * This procedure sets the default channel map to the PAL-B channel map and
 * sets up the mode to TLG_MODE_ANALOG_TV_UNCOMP, the standard to TLG_VSTD_PAL_B
 * and the bandwidth to TLG_BW_7 (if not already set up).
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
#ifdef SUPPORT_CHANNEL_MAP_UK
TLGDLL_API int TLGAPP_SetChannelMapUK(void);
#endif /* SUPPORT_CHANNEL_MAP_UK */

/** Set the Channel map, mode, bandwidth, video standard to Japan settings.
 *
 * This procedure sets the default channel map to the NTSC-M channel map and
 * sets up the mode to TLG_MODE_ANALOG_TV_UNCOMP, the standard to TLG_VSTD_NTSC_M
 * and the bandwidth to TLG_BW_6 (if not already set up).
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
#ifdef SUPPORT_CHANNEL_MAP_JAPAN
TLGDLL_API int TLGAPP_SetChannelMapJapan(void);
#endif /* SUPPORT_CHANNEL_MAP_JAPAN */

#ifdef SUPPORT_CHANNEL_MAP_PALWE
/** Set the Channel map, mode, bandwidth, video standard to Western Europe,  
 *  Turkey, United Arab Emirates, Israel, Afghanistan, 
 *  Nigeria, Kenya, Singapore, Inida, Thailand, Cambodia
 *  Indonesia, Malaysia, Myanmar, Laos settings.
 *
 * This procedure sets the default channel map to the PAL-D channel map and
 * sets up the mode to TLG_MODE_ANALOG_TV_UNCOMP, the standard to TLG_VSTD_PAL_B
 * and the bandwidth to TLG_BW_7 (if not already set up).
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLGAPP_SetChannelMapPALWE(void);
/** Set the Channel map, mode, bandwidth, video standard to Western Europe,  
 *  countries with hybrid PAL/SECAM settings.
 *
 * This procedure sets the default channel map to the PAL-D channel map and
 * sets up the mode to TLG_MODE_ANALOG_TV_UNCOMP, the standard to TLG_VSTD_PAL_B
 * and the bandwidth to TLG_BW_7 (if not already set up).
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLGAPP_SetChannelMapPAL_SECAM_b_g_Hybrid(void);
#endif /* SUPPORT_CHANNEL_MAP_PALWE */

/** Set the Channel map, mode, bandwidth, video standard to settings
 *  for Vietnam.
 *
 * This procedure sets the default channel map to the PAL-D channel map and
 * sets up the mode to TLG_MODE_ANALOG_TV_UNCOMP, the standard to TLG_VSTD_PAL_D
 * and the bandwidth to TLG_BW_6 (if not already set up).
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
#ifdef SUPPORT_CHANNEL_MAP_VIETNAM
TLGDLL_API int TLGAPP_SetChannelMapVietnam(void);
#endif /* SUPPORT_CHANNEL_MAP_VIETNAM */

/** Set the Channel map, mode, bandwidth, video standard to PAL-D settings.
 *
 * This procedure sets the default channel map to the SECAM B G channel map and
 * sets up the mode to TLG_MODE_ANALOG_TV_UNCOMP, the standard to TLG_VSTD_SECAM_B
 * and the bandwidth to TLG_BW_7 (if not already set up).
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
#ifdef SUPPORT_CHANNEL_MAP_SECAM_BG
TLGDLL_API int TLGAPP_SetChannelMap_SECAM_b_g(void);
#endif /* SUPPORT_CHANNEL_MAP_SECAM_BG */

#ifdef SUPPORT_CHANNEL_MAP_SECAM_DK
/** Set the Channel map, mode, bandwidth, video standard to PAL-D settings.
 *
 * This procedure sets the default channel map to the SECAM D/K channel map and
 * sets up the mode to TLG_MODE_ANALOG_TV_UNCOMP, the standard to TLG_VSTD_SECAM_D
 * and the bandwidth to TLG_BW_8 (if not already set up).
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLGAPP_SetChannelMap_SECAM_d_k(void);
/** Set the Channel map, mode, bandwidth, video standard to PAL_SECAM-D settings.
 *
 * This procedure sets the default channel map to the PAL?SECAM D/K channel map and
 * sets up the mode to TLG_MODE_ANALOG_TV_UNCOMP, the standard to TLG_VSTD_SECAM_D
 * and the bandwidth to TLG_BW_8 (if not already set up).
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLGAPP_SetChannelMapPAL_SECAM_d_k_Hybrid(void);
#endif /* SUPPORT_CHANNEL_MAP_SECAM_DK */


TLGDLL_API int TLGAPP_Init_Aux(uint32 base_addr, int start);

/** Start and stop close caption.
 *
 * This procedure starts/stops the close caption data for field 0 and 1
 *
 * @param start_stop        1: enable capturing close caption data
 *                          0: disable capturing close caption data
 *
 * @param drop_nulls        drop null
 * @param pass_parity       pass parity
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 * @sa TLGAPP_CC_GetRawData
 */
TLGDLL_API int TLGAPP_CC_Control   (
    uint16 start_stop,
    uint16 drop_nulls ,
    uint16 pass_parity);

/** Get close caption raw data for each field.
 *
 * This procedure only gets the raw data for the field specified by
 * the field_number parameter. It does not decode the raw data.
 * User should call this function every 1/2 second or more often to get the cc data
 * Before calling this function, this is the normal sequence:
 *                          call to enable cc for field 0 and 1 
 *                      TLG_CC_Control()
 *                          every 1/2 second, or more often, polling the data
 *                      TLG_CC_GetRawData()
 *                          call to disable cc for field 0 and 1 when done
 *                      TLG_CC_Control()
 *
 * @param field_number     0: field 0, 1: field 1
 * @param p_length         return the number of bytes read from the fifo
 * @param p_data           raw data read back from the field
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 * @sa TLGAPP_CC_Control
 */
TLGDLL_API int TLGAPP_CC_GetRawData(
    uint16                  field_number,
    uint16                 *p_length ,
    uint8                  *p_data
);

/** Force the app init status to the new status.
 *
 * This procedure force the app init status to the new state
 * Set the init status to 1 if the application wants to bypass the TLGAPP_Init(),
 *
 * @param init_done         0: needs to call TLGAPP_Init() later, 
 *                          1: do not need to callTLGAPP_Init() 
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 * @sa TLGAPP_Init
 * @sa TLGAPP_ChannelSetup
 * @sa TLGAPP_TurnOnTestPattern
 * @sa TLGAPP_TurnOffTestPattern
 */
TLGDLL_API int TLGAPP_SetAppInitStatus(int init_done);


/** Get the current channel map.
 *
 * This procedure returns the current channel map and the size
 *
 * @param p_chn_map_size     size of the current channel map
 *
 * @retval pointer to the current channel  Call succeeded.
 * @retval 0                Call failed
 *
 * @sa TLGAPP_GetCurChannelMapSize
 */
TLGDLL_API void *TLGAPP_GetCurChannelMap(uint16 *p_chn_map_size);

#ifdef TLG_NEW_CALLBACK
/** Set the call back function for the scan channel function.
 *
 * This procedure sets the callback function for the TLGAPP_ScanChannels
 * While scanning channel, the callback function is called each time the
 * a channel is found to transfer the control back to the application
 *
 * @param scan_num  callback function number
 * @param param     data structure for call back function to pass data 
 *                  back to application
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 * @sa TLGAPP_ScanChannels
 */
/*
    Genric call back param
*/
typedef struct _tlg_cb_param_t
{
    uint32 handle;
    uint32 data [31]; /* 32 dword align */
} tlg_cb_param;

typedef struct _tlg_cb_param_chn_t
{
    uint32 handle;
    uint32 chn  ;
    uint32 hz   ;
    uint32 bw   ;
    uint32 std  ;
} tlg_cb_param_chn_s;

#define CB_ID_NONE          0
#define CB_ID_TEST          1
#define CB_ID_SCAN          2
#define CB_ID_PRE_SCAN      3
typedef int (__stdcall *fp_call_back)(int num, void *param);

/** Set the test call back function.
 *
 * This procedure sets the test call back function to test each checkpoint.
 * NOTE: 
 *      The tlg_cb_param is the generic one, all call back parameter 
 *      structured should have the handle.
 *      Currently there are 2 call back functions supported
 *          CB_ID_TEST      supports register setting test check point
 *          CB_ID_SCAN      supports the ScanChannel call back to update the GUI
 *          CB_ID_PRE_SCAN  supports the ScanChannel call back before scanning
 *                          for example it can be used to program the promax box
 *
 * @param cb_id             call back function ID.  This ID identifies which
 *                          call back function and param are updated with the new 
 *                          call back function and param
 * @param cb_func           call back function pointer.
 * @param cb_param          Test parameter to pass to the call back function.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call 
 *
 */
TLGDLL_API int TLGAPP_SetCallbackFunc(
    uint16              cb_id    ,
    fp_call_back        cb_func  , 
    void                *cb_param     );

#endif /* TLG_NEW_CALLBACK */

/** Set the scan mode.
 *
 * This procedure sets the scan mode to use fast scan or slow scan
 *
 * @param fast_scan_mode    0: slow scan, 1: fast scan
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 * @sa TLGAPP_ScanChannels
 * @sa TLGAPP_SeekNextChn
 * @sa TLGAPP_SeekPrevChn
 */
TLGDLL_API void TLGAPP_SetFastScanMode(uint16 fast_scan_mode);

#ifdef TLG_FM_NEW_INTERFACE
/** Set the FM parameter: step size, min frequency and max frequency
 *
 * This procedure sets the fm step size, min/max frequency and
 * sets up the mode to TLG_MODE_FM_RADIO, the standard to TLG_VSTD_NTSC_M
 * and the bandwidth to TLG_BW_6 (if not already set up). It will also
 * perform TLG_InitScanFMChn() to obtain slope and offset settings that will
 * be used in subsequent calls to TLGAPP_SetChannel() and TLGAPP_ScanChannels().
 * 
 * The parameter has a range - step_size: [400K, 10K], min_hz/max_hz: [108M, 76M]
 * if not in the range, it will set to the default value: step_size=100K, min_hz=76M, max_hz=108M
 *
 * It must be called before FM scan.
 *
 * @param step_size   FM step size between 2 channel
 * @param min_hz   the minimal frequency to be scanned
 * @param max_hz   the maximal frequency to be scanned
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLGAPP_FMSetParam(uint32 step_size, uint32 min_hz, uint32 max_hz);

/** Scan/Tune to the specified Frequency for FM.
 *
 * This procedure will scan the given frequency and tune to it. 
 * If in scan state, it will only scan the given frequency.
 *
 * @param center_freq_hz   frequency to be tuned
 *
 * @retval  TRUE    channel change succeeded and frequency is locked.
 * @retval  FALSE   channel change failed.
 */
TLGDLL_API int TLGAPP_FMSetFrequency(uint32 center_freq_hz);

/** Start FM scan
 *
 * This procedure will set indication that following is fm scan. It must be called when before FM scan.
 *
 */
TLGDLL_API void TLGAPP_FMStartScan(void);

/** Stop FM scan
 *
 * This procedure will set indication that fm scan is end. It must be called after FM scan.
 *
 */
TLGDLL_API void TLGAPP_FMStopScan(void);

/** get current FM SSI value
 *
 * This procedure will get current FM SSI value
 *
 */
TLGDLL_API int TLGAPP_FMGetCurrentSSI(void);
#endif // TLG_FM_NEW_INTERFACE

extern TLGDLL_API p_tlg_channel_t(p_tlg_cur_map);/*!< Current Default Channel Map*/
extern int              tlg_cur_bw;         /*!< Current Bandwidth          */
extern int              tlg_cur_mode;       /*!< current Mode               */
extern int              tlg_cur_std;        /*!< Current Video Std          */
extern int              tlg_slope;          /*!< Current FM Slope           */
extern int              tlg_offset;         /*!< Current FM Offset          */
extern int              tlg_init_done;      /*!< Flag Init called           */
extern int              tlg_cur_lna;        /*!< LNA Setting                */
extern uint16           tlg_cur_gain;       /*!< audio gain                 */
extern uint16           tlg_cur_aud_mode;   /*!< audio mode                 */
extern uint32           tlg_i2c_addr;       /*!< i2c address of TLG1120     */
extern tlg_statep       tlg_power_state;    /*!< state for Deep Sleep       */

/*@}*/

#ifdef __cplusplus
};
#endif
#endif /* TLG_CHIP_TYPE */


#endif /* _TLG1120APP_H_*/
