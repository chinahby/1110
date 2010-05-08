 /*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                        RF FREQUENCY UTILITIES

GENERAL DESCRIPTION
   This module contains RF Frequency related utility functions.
   
EXTERNALIZED FUNCTIONS

rf_get_rx_carrier_freq
  This function returns the Rx carrier frequency in KHz for the band and channel
  specified in the parameters.  If the band or channel is not supported 
  then 0 is returned.

rf_get_tx_carrier_freq
  This function returns the Tx carrier frequency for the band and channel
  specified in the parameters.  If the band or channel is not supported 
  then 0 is returned.

rf_get_valid_bc_and_mask
  This function is used to index a table that contains the supported band
  classes and the corresponding system band class masks.

rf_translate_band_class
  This function translates the band class coming 
  in/out of the Rf driver to a different band.   


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 1998,1999,2000 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2001,2002,2003 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2004,2005,2006 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2007           by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*=================================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rf_freq_util.c#4 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------------
04/16/08   vm      Corrected the Tx and Rx frequency sets for BC5 and BC10
03/09/07   bmg     BC15 feature fixes
01/17/06   rmd     Fixed Lint error.
01/16/06   rmd     Implemented BC11 and BC14 code review results. Including:
                   * Mainline BC11 and BC14 code.
                   * Moving Translate code into rf_util_freq.c.
                   * Moving rf_get_valid_bc_and_mask() into rf_util_freq.c.
                   * Replacing ASSERT() with RF_ASSERT() and ERROR_FATAL() with
                     RF_FATAL_ERROR().
11/08/05   rmd     * Added support for BC11 and BC14.
                   * Added support for BC5 block K.
                   * Increased the resolution of the base frequency to KHz.
10/21/05   sar     Fixed Lint warnings.
10/17/05   sar     Initial Revision.
===================================================================================*/
#include "rf1x.h"
#include "sys.h"
#include "rfi.h"

/* -------------------------------------------------------------------------

                        LOCAL CONTANT/MACRO DEFINITIONS

---------------------------------------------------------------------------- */ 
#define RF_T53_BC0_MASK      0x80
#define RF_BC14_TO_BC1_MASK  0x40
#define RF_BC5_TO_BC11_MASK  0x20

/*==============================================================
    This structure holds the parameters required for calculation
    of Rx Carrier Frequency as described below:
        Find the row where the band is matched and chan lies between
        lo and hi limits then 
            Rx Carrier Freq (KHz)= ( base + (spacing * (chan + offset)))

    Note:  The functions utilizing these tables perform the appropriate 
    scaling within the calculations to return the Carrier frequencies in KHz.  
    
================================================================*/    
static const rf_carrier_freq_type rf_rx_freq_tbl [] =
{
  /*  band        low   high  spacing  offset   base (KHz)                   */
  {RF_BC0_BAND,     1,   799,  300,        0,   870000}, /* BC0: Ch 1-799    */
  {RF_BC0_BAND,   991,  1023,  300,    -1023,   870000}, /* BC0: Ch 991-1023 */
  {RF_BC1_BAND,     0,  1199,  500,        0,  1930000}, /* BC1: Ch 0-1199   */
  {RF_BC3_BAND,     1,   799,  125,        0,   860000}, /* BC3: Ch 1-799    */
  {RF_BC3_BAND,   801,  1039,  125,     -800,   843000}, /* BC3: Ch 801-1039 */
  {RF_BC3_BAND,  1041,  1199,  125,    -1040,   832000}, /* BC3: Ch 1041-1199*/
  {RF_BC3_BAND,  1201,  1600,  125,    -1200,   838000}, /* BC3: Ch 1201-1600*/
  {RF_BC4_BAND,     0,   599,  500,        0,  1840000}, /* BC4: Ch 0-599    */
  {RF_BC5_BAND,     1,   400,  250,       -1,   460000}, /* BC5: Ch 1-400    */
  {RF_BC5_BAND,     472, 538,  250,      -472,   420000}, /* BC5: Ch 472-538    */
  {RF_BC5_BAND,   539,   871,  250,     -512,   421000}, /* BC5: Ch 539-871  */
  {RF_BC5_BAND,  1039,  1473,  200,    -1024,   461010}, /* BC5: Ch 1039-1473*/
  {RF_BC5_BAND,  1536,  1715,  250,    -1536,   489000}, /* BC5: Ch 1536-1715*/
  {RF_BC5_BAND,  1792,  2016,  200,    -1792,   489000}, /* BC5: Ch 1792-2016*/
  {RF_BC6_BAND,     0,  1199,  500,        0,  2110000}, /* BC6: Ch 0-1199   */
  {RF_BC10_BAND,    0,   719,  250,        0,   851000},  /* BC10:Ch 0-719    */
  {RF_BC10_BAND,  720,   919,  250,        -720,   935000},  /* BC10:Ch 720-919    */  
  {RF_BC11_BAND,    1,   400,  250,       -1,   460000}, /* BC11:Ch 1-400    */
  {RF_BC11_BAND,  539,   871,  250,     -512,   421000}, /* BC11:Ch 539-871  */
  {RF_BC11_BAND, 1039,  1473,  200,    -1024,   461010}, /* BC11:Ch 1039-1473*/
  {RF_BC11_BAND, 1536,  1715,  250,    -1536,   489000}, /* BC11:Ch 1536-1715*/
  {RF_BC11_BAND, 1792,  2016,  200,    -1792,   489000}, /* BC11:Ch 1792-2016*/
  {RF_BC14_BAND,    0,  1299,  500,        0,  1930000}, /* BC14:Ch 0-1299   */
#ifdef RF_HAS_BC15_NV_TABLE
  {RF_BC15_BAND,    0,   899,  500,        0,  2110000}  /* BC15:Ch 0-899    */
#endif
};

/*==============================================================
    This structure holds the parameters required for calculation
    of Tx Carrier Frequency as described below:
        Find the row where the band is matched and chan lies between
        lo and hi limits then 
            Tx Carrier Freq (KHz)= ( base + (spacing * (chan + offset)))

    Note:  The functions utilizing these tables perform the appropriate 
    scaling within the calculations to return the Carrier frequencies in KHz.  
    
================================================================*/    
static const rf_carrier_freq_type rf_tx_freq_tbl [] =
{
  /*   band        low  high   spacing offset  base (KHz) */
  {RF_BC0_BAND,     1,   799,  300,        0,   825000}, /* BC0: Ch 1-799 */
  {RF_BC0_BAND,   991,  1023,  300,    -1023,   825000}, /* BC0: Ch 991-1023 */
  {RF_BC1_BAND,     0,  1199,  500,        0,  1850000}, /* BC1: Ch 0-1199   */
  {RF_BC3_BAND,     1,   799,  125,        0,   915000}, /* BC3: Ch 1-799    */
  {RF_BC3_BAND,   801,  1039,  125,     -800,   898000}, /* BC3: Ch 801-1039 */
  {RF_BC3_BAND,  1041,  1199,  125,    -1040,   887000}, /* BC3: Ch 1041-1199*/
  {RF_BC3_BAND,  1201,  1600,  125,    -1200,   893000}, /* BC3: Ch 1201-1600*/
  {RF_BC4_BAND,     0,   599,  500,        0,  1750000}, /* BC4: Ch 0-599    */
  {RF_BC5_BAND,     1,   400,  250,       -1,   450000}, /* BC5: Ch 1-400    */
  {RF_BC5_BAND,     472, 538,  250,      -472,   410000}, /* BC5: Ch 472-538    */
  {RF_BC5_BAND,   539,   871,  250,     -512,   411000}, /* BC5: Ch 539-871  */
  {RF_BC5_BAND,  1039,  1473,  200,    -1024,   451010}, /* BC5: Ch 1039-1473*/
  {RF_BC5_BAND,  1536,  1715,  250,    -1536,   479000}, /* BC5: Ch 1536-1715*/
  {RF_BC5_BAND,  1792,  2016,  200,    -1792,   479000}, /* BC5: Ch 1792-2016*/
  {RF_BC6_BAND,     0,  1199,  500,        0,  1920000}, /* BC6: Ch 0-1199   */
  {RF_BC10_BAND,    0,   719,  250,        0,   806000},  /* BC10:Ch 0-719    */
  {RF_BC10_BAND,  720,   919,  250,        -720,   896000},  /* BC10:Ch 720-919    */
  {RF_BC11_BAND,    1,   400,  250,       -1,   450000}, /* BC11:Ch 1-400    */
  {RF_BC11_BAND,  539,   871,  250,     -512,   411000}, /* BC11:Ch 539-871  */
  {RF_BC11_BAND, 1039,  1473,  200,    -1024,   451010}, /* BC11:Ch 1039-1473*/
  {RF_BC11_BAND, 1536,  1715,  250,    -1536,   479000}, /* BC11:Ch 1536-1715*/
  {RF_BC11_BAND, 1792,  2016,  200,    -1792,   479000}, /* BC11:Ch 1792-2016*/
  {RF_BC14_BAND,    0,  1299,  500,        0,  1850000}, /* BC14:Ch 0-1299   */
#ifdef RF_HAS_BC15_NV_TABLE
  {RF_BC15_BAND,    0,   899,  500,        0,  1710000}  /* BC14:Ch 0-1299   */
#endif
};


/* -------------------------------------------------------------------------

                      LOCAL FUNCTION PROTOTYPES

---------------------------------------------------------------------------- */ 
static boolean rf_enable_translate_bc14(void);

static boolean rf_enable_translate_bc11(void);

/* -------------------------------------------------------------------------

                          FUNCTION DEFINITIONS

---------------------------------------------------------------------------- */ 
/*=========================================================================

FUNCTION RF_GET_TX_CARRIER_FREQ

DESCRIPTION
  This function returns the Tx carrier frequency for the band and channel
  specified in the parameters.  If the band or channel is not supported 
  then 0 is returned.

DEPENDENCIES
  None
   

RETURN VALUE
  Tx carrier frequency for valid band and channel in KHz.  

SIDE EFFECTS
  None.

===========================================================================*/
uint32 rf_get_tx_carrier_freq( rf_card_band_type band, word chan )
{
    int32 freq=0; 
    int32  delta = 0;
    int16 max_entries= sizeof(rf_tx_freq_tbl)/sizeof(rf_tx_freq_tbl[0]);
    int16 i = 0;

    while(i < max_entries)
    {
        if((band == rf_tx_freq_tbl[i].band ) && 
           (chan >= rf_tx_freq_tbl[i].lo_chan ) &&
           (chan <= rf_tx_freq_tbl[i].hi_chan ))
            break;
        i++;
    }

    if(i < max_entries)
    {
        /* To avoid doing floating point math:
             1) All calculations are done in Khz.
             2) To avoid losing accuracy, the ch_sp (spacing) parameter in the 
                table is scaled to 10 times the value in KHz. At the end of the 
                delta calculation, the 10 is divided off.
        */           
        delta = (rf_tx_freq_tbl[i].ch_sp * (chan + rf_tx_freq_tbl[i].ch_offst))/10;
        freq = (rf_tx_freq_tbl[i].base_freq) + delta;
    }
    return (uint32) freq;
}
/* Eject  */
/*=========================================================================

FUNCTION RF_GET_RX_CARRIER_FREQ

DESCRIPTION
  This function returns the Rx carrier frequency in KHz for the band and channel
  specified in the parameters.  If the band or channel is not supported 
  then 0 is returned.

DEPENDENCIES
  None
   

RETURN VALUE
  Rx carrier frequency in KHz for valid band and channel.  

SIDE EFFECTS
  None.

===========================================================================*/
uint32 rf_get_rx_carrier_freq( rf_card_band_type band, word chan )
{
    int32 freq=0;
    int32  delta = 0;
    int16 max_entries= sizeof(rf_rx_freq_tbl)/sizeof(rf_rx_freq_tbl[0]);
    int16 i = 0;

    while(i < max_entries)
    {
        if((band == rf_rx_freq_tbl[i].band ) && 
           (chan >= rf_rx_freq_tbl[i].lo_chan ) &&
           (chan <= rf_rx_freq_tbl[i].hi_chan ))
            break;
        i++;
    }

    if(i < max_entries)
    {
        /* To avoid doing floating point math:
             1) All calculations are done in Khz.
             2) To avoid losing accuracy, the ch_sp (spacing) parameter in the 
                table is scaled to 10 times the value in KHz. At the end of the 
                delta calculation, the 10 is divided off.
        */
        delta = (rf_rx_freq_tbl[i].ch_sp * (chan + rf_rx_freq_tbl[i].ch_offst))/10;
        freq = (rf_rx_freq_tbl[i].base_freq) + delta;
    }
    return (uint32) freq;
}

/* Eject  */
/*===========================================================================

FUNCTION rf_get_valid_bc_and_mask                EXTERNAL FUNCTION

DESCRIPTION
    This function is used to index a table that contains the supported band
  classes and the corresponding system band class masks (defined in sys,h).
  This function should be called within a while loop, Example:

  while(rf_get_valid_bc_and_mask(index, &valid_band)
  {
     MSG_HIGH("Valid Band Class %d, Corresponding mask = %d", \
                                                    valid_band.bClass,
                                                    valid_band.bMask,
                                                    0);
  }    
    
PARAMETERS  
  1) Parameter name: index
     - While loop index/counter. 

     - Parameter type: uint8

  2) Parameter name: vband
     - Structure pointer used to return the valid band class and its 
       corresponding system mask.

     - Parameter type: rf_bclass_bmask_type
       Valid Outputs: 
          2.1) Structure member: bClass
               - Return the valid band class. Note: It will return all 1s if
                 index is outside the table boundary.
          
               Type: rf_card_band_type (enum)

          2.1) Structure member: bMask
               - Return the corresponding band mask.Note: It will return 0 if
                 index is outside the table boundary.
          
               Type: sys_band_mask_type (enum)     
       
DEPENDENCIES
  None.
  
RETURN VALUE
  return type: boolean.
  - TRUE  = the index value passed by the user is within the table boundary.
  - FALSE = the index value passed by the user is outside the table boundary.
  
SIDE EFFECTS
  None
==========================================================================*/
boolean rf_get_valid_bc_and_mask(uint8                index, \
                                 rf_bc_and_mask_type* valid_bc_ptr)
{
  const rf_bc_and_mask_type valid_band_table[] =
      {{RF_BC0_BAND, SYS_BAND_MASK_BC0},
       {RF_BC1_BAND, SYS_BAND_MASK_BC1},
       {RF_BC3_BAND, SYS_BAND_MASK_BC3},
       {RF_BC4_BAND, SYS_BAND_MASK_BC4},
       {RF_BC5_BAND, SYS_BAND_MASK_BC5},
       {RF_BC6_BAND, SYS_BAND_MASK_BC6},
       {RF_BC10_BAND, SYS_BAND_MASK_BC10},
       {RF_BC11_BAND,SYS_BAND_MASK_BC11},
       {RF_BC14_BAND,SYS_BAND_MASK_BC14},
#ifdef RF_HAS_BC15_NV_TABLE
       {RF_BC15_BAND,SYS_BAND_MASK_BC15}
#endif
      };

  boolean rtn_value = FALSE;

  /* If we are within the table boundary, return the valid band class and
     its corresponding system mask. */
  if (((sizeof(valid_band_table)/sizeof(valid_band_table[0])) > index)) 
  {
      valid_bc_ptr->bc   = valid_band_table[index].bc;
      valid_bc_ptr->mask = valid_band_table[index].mask;
      rtn_value          = TRUE;
  }
  else
  {
      valid_bc_ptr->bc   = (rf_card_band_type)(-1); /* equal to all 1s. */
      valid_bc_ptr->mask = 0;                       /* Where 0 is not a valid 
                                                        value.*/
      rtn_value          = FALSE;
  }

  return rtn_value;
}

/* Eject  */
/*===========================================================================

FUNCTION rf_translate_band_class                       EXTERNAL FUNCTION

DESCRIPTION
    The purpose of this function is to translate the band class coming 
  in/out of the Rf driver to a different band (for testing). 

    For example: To test BC14 (US PCS + 5 MHz), if the RF driver is told 
  to go into Band Class 1 then this function will tell the RF driver to go 
  to Band class 14 instead. If the RF driver is in band class 14 and a client 
  wants to know what band class is the RF driver on then this function will
  tell the client that we are in band class 1 instead.
   
  NOTE: If no translation is performed then the return value is equal c_band
        (input band).
    
PARAMETERS  
  1) Parameter name: cmd
     - Specifies if the code calling this function is setting or reading 
       a band class to/from the RF driver. 

     - Parameter type: rf_translate_bc_type(enum).
       Valid Input:
           RF_TRANSLATE_BC_IN:  The code calling this function is setting  
                                the RF driver band class.
           RF_TRANSLATE_BC_OUT: The code calling this function is reading  
                                the RF driver current band class and returning
                                it to a client.

  2) Parameter name: c_band
     - Current band class.

     - Parameter Type: rf_card_band_type (enum)    
                      
DEPENDENCIES
  NV most have been read.
  
RETURN VALUE
  1) Returns the new band based on the current band (cBand). If no 
     translation is performed then the return value is equal cBand.

     - Parameter Type: rf_card_band_type (enum)  
  
SIDE EFFECTS
  None
==========================================================================*/
rf_card_band_type rf_translate_band_class(rf_translate_bc_type cmd, \
                                                 rf_card_band_type    c_band)
{
  uint8              rf_config = 0;
  rf_card_band_type  new_band  = (rf_card_band_type)0;

  /* If input parameter is out of range then assert(). */
  RF_ASSERT(cmd < RF_TRANSLATE_BC_INVALID);
  rf_config = rf_get_rf_config();

  /* Set the new band equal the current band. */
  new_band = c_band;

  /* If the corresponding RFNV CONFIG NV item bit is set then 
     translate BC0 to/from BC3. This is done for JCDMA testing. */
  if( (rf_config & RF_T53_BC0_MASK) != 0 )
  {   /* We are writing to the RF driver. */
      if (cmd == RF_TRANSLATE_BC_IN)
      {   /* If we are told to go to BC3 then set the RF driver to BC0. */
          if(c_band == RF_BC3_BAND)
          {    
              new_band = RF_BC0_BAND;
              MSG_HIGH("BC Translate: cmd=%d, old bc=%d, new bc=%d.", \
                                                        cmd,c_band,new_band);
          }
      }
      else /* We are reading from the RF driver. */
      {   /* If we are currently in BC0 then tell the caller that we are in
             BC3. */
          if(c_band == RF_BC0_BAND)
          {    
              new_band = RF_BC3_BAND;
              MSG_HIGH("BC Translate: cmd=%d, old bc=%d, new bc=%d.", \
                                                        cmd,c_band,new_band);
          }
      }
  }


 
  /* If the corresponding NV_RF_CONFIG_I NV item bit is set then 
     translate BC1 to/from BC14. This is done for BC14 (US PCS + 5MHz) 
     testing. */
  if (rf_enable_translate_bc14()) 
  {   /* If we are writing into the RF driver. */
      if (cmd == RF_TRANSLATE_BC_IN)
      {   /* If we are told to go to BC1 then set the RF driver to BC14. */
          if(c_band == RF_BC1_BAND)
          {    
              new_band = RF_BC14_BAND;
              MSG_HIGH("BC Translate: cmd=%d, old bc=%d, new bc=%d.", \
                                                    cmd,c_band,new_band);
          }
      }
      else /* We are reading from the RF driver. */
      {   /* If we are currently in BC14 then tell the caller that we are in
             BC1. */ 
          if(c_band == RF_BC14_BAND)
          {  
              new_band = RF_BC1_BAND;
              MSG_HIGH("BC Translate: cmd=%d, old bc=%d, new bc=%d.", \
                                                    cmd,c_band,new_band);
          }
      }
  }


  /* If the corresponding NV_RF_CONFIG_I NV item bit is set then 
     translate BC5 to/from BC11. This is done for BC11 testing. */
  if (rf_enable_translate_bc11()) 
  {   /* If we are writing into the RF driver. */
      if (cmd == RF_TRANSLATE_BC_IN)
      {   /* If we are told to go to BC5 then set the RF driver to BC11. */
          if(c_band == RF_BC5_BAND)
          {    
              new_band = RF_BC11_BAND;
              MSG_HIGH("BC Translate: cmd=%d, old bc=%d, new bc=%d.", \
                                                    cmd,c_band,new_band);
          }
      }
      else /* We are reading from the RF driver. */
      {   /* If we are currently in BC5 then tell the caller that we are in
             BC11. */ 
          if(c_band == RF_BC11_BAND)
          {  
              new_band = RF_BC5_BAND;
              MSG_HIGH("BC Translate: cmd=%d, old bc=%d, new bc=%d.", \
                                                    cmd,c_band,new_band);
          }
      }
  }
  
  return new_band;
}

/* Eject  */
/*===========================================================================

FUNCTION rf_enable_translate_bc14                 EXTERNAL FUNCTION

DESCRIPTION
    This function returns if we are suppose to translate BC1 to BC14.
  (for testing). 
    
PARAMETERS  
  NONE.
                      
DEPENDENCIES
  NV most have been initialized.
  
RETURN VALUE
  return type: boolean.
  - TRUE  = Translate BC1 to BC14.
  - FALSE = Do not translate BC1 to BC14.
  
SIDE EFFECTS
  None
==========================================================================*/
boolean rf_enable_translate_bc14(void)
{  
    boolean  rtn_val = FALSE;

    /* If the corresponding NV_RF_CONFIG_I NV item bit is set then 
       translate BC1 to/from BC14. This is done for BC14 (US PCS + 5MHz) 
       testing. */
    if (rf_get_rf_config() & RF_BC14_TO_BC1_MASK) 
    {   
        rtn_val = TRUE;
    }

    return rtn_val;
}

/* Eject  */
/*===========================================================================

FUNCTION rf_enable_translate_bc11                 EXTERNAL FUNCTION

DESCRIPTION
    This function returns if we are suppose to translate BC5 to BC11.
  (for testing). 
    
PARAMETERS  
  NONE.
                      
DEPENDENCIES
  NV most have been initialized.
  
RETURN VALUE
  return type: boolean.
  - TRUE  = Translate BC5 to BC11.
  - FALSE = Do not translate BC5 to BC11.
  
SIDE EFFECTS
  None
==========================================================================*/
boolean rf_enable_translate_bc11(void)
{  
    boolean  rtn_val = FALSE;

	return rtn_val;

    /* If the corresponding NV_RF_CONFIG_I NV item bit is set then 
       translate BC5 to/from BC11. This is done for BC11 testing. */
    if (rf_get_rf_config() & RF_BC5_TO_BC11_MASK) 
    {   
        rtn_val = TRUE;
    }

    return rtn_val;
}



