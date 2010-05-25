#ifndef MDP_VSYNC_CONFIG_H
#define MDP_VSYNC_CONFIG_H
/*============================================================================

                   MDP VSYNC CONFIGURATION Header File

  DESCRIPTION
  This file contains the definitions needed to configure the MDP Vsync algorithm.

  Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/Display/mdp1/mdp/main/latest/src/mdp_vsync_config.h#03 
 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 09/09/08  rk    Added GPIO settings for ULC
 08/04/08  rk    Added GPIO settings for LCU target
 06/20/08  rk    Added Vsync Parameters for TMD22 QVGA EBI2 LCD
 06/03/08  rk    Initial code
============================================================================*/

//============================================================================
//                        INCLUDE FILES
//============================================================================

/**none so far**/

/////////////////////////////////////////////////////////////////////
//Vsync Config
/////////////////////////////////////////////////////////////////////


#if (defined(T_QSC60X5) && !defined(FEATURE_TMD_20_QVGA)) 
  #define MDP_WRITE_FASTER_VSYNC_OFFSET   124
  #define MDP_WRITE_SLOWER_VSYNC_OFFSET   32
  #define MDP_ROWS_PER_SECOND_OFFSET   50
  #define LCD_REFRESH_EBI   73  // LCD Refresh Rate in Hz on SC2x
  #define LCD_DISPLAY_LINES 225 // QCIF+ LCD 
#elif defined (FEATURE_TMD_22_QVGA)
  #define  MDP_WRITE_FASTER_VSYNC_OFFSET 35  
  #define  MDP_WRITE_SLOWER_VSYNC_OFFSET 45
  #define  MDP_ROWS_PER_SECOND_OFFSET 35
  #define  LCD_DISPLAY_LINES 320 
  #define  LCD_REFRESH_EBI  60
#else //for M6290
  //This offset is used to estimate LCD read location 
  #define  MDP_WRITE_FASTER_VSYNC_OFFSET 70  
  #define  MDP_WRITE_SLOWER_VSYNC_OFFSET 45
  //Used for MDDI rows update
  #define  MDP_ROWS_PER_SECOND_OFFSET 35
  #define LCD_DISPLAY_LINES 320 
#endif /*(defined(T_QSC60X5) && !defined(FEATURE_TMD_20_QVGA))*/

#define VSYNC_RESYNC_TIME_IN_MS   100 /*how often the MDP syncs with the LCD read pointer - 100ms*/


#if (defined (T_QSC1110))
  #define MDP_GPIO_BLANKING_SIGNAL         GPIO_INT_54
  #define MDP_GPIO_BLANKING_SIGNAL_INPUT   GPIO_INPUT_54
  #define MDP_GPIO_ACTIVE_LEVEL            ACTIVE_LOW
#elif (defined(T_QSC60X5))
  #define MDP_GPIO_BLANKING_SIGNAL         GPIO_INT_13
  #define MDP_GPIO_BLANKING_SIGNAL_INPUT   GPIO_INPUT_13
  #define MDP_GPIO_ACTIVE_LEVEL            ACTIVE_HIGH
#elif (defined (T_QSC6270) || defined (T_QSC6240))
#error code not present
#elif (defined FEATURE_TMD_22_QVGA)
#error code not present
#endif


/* Please refer to MDDI driver file for MDDI parameters*/
/* Please refer to EBI- lcd driver file for detailed settings of the LCD*/

/////////////////////////////////////////////////////////////////////
// End of Vsync Config
/////////////////////////////////////////////////////////////////////
#endif /*MDP_VSYNC_CONFIG_H */

