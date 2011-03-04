/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*



                            FTM RF ENC  

GENERAL DESCRIPTION
  This is the header file for the embedded FTM RF encoder values 

Copyright (c) 1994, 1995, 1996 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1997, 1998, 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2000, 2001       by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/ftm/ftm_rf_enc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/25/05   ra      DORA merge.
03-21-02   rjr	   clean up and feturize #defines
03-01-02   rjr	   created
===========================================================================*/
#ifndef FTM_RF_ENC_H__
#define FTM_RF_ENC_H__

#if defined(FEATURE_FACTORY_TESTMODE)


    #define MOD_MISC_CTL_IS95A_MODE                 (0x1 << 15)  
    #define MOD_MISC_CTL_IS95C_MODE                 (0x0 << 15)
    #define MOD_MISC_CTL_RETRANS_ENA                (0x1 << 12)
    #define MOD_MISC_CTL_RETRANS_DIS                    (0x0 << 12)
    #define MOD_MISC_CTL_FCH_EIGHTH_GATE_ENA        (0x1 << 11)
    #define MOD_MISC_CTL_FCH_EIGHTH_GATE_DIS    (0x0 << 11)
    #define MOD_MISC_CTL_PCH_GATING_RATE_FULL   (0x00 << 9)
    #define MOD_MISC_CTL_PCH_GATING_RATE_HALF       (0x01 << 9)
    #define MOD_MISC_CTL_PCH_GATING_RATE_QUARTER    (0x02 << 9)

#if defined(T_MSM6700) || defined(T_MSM6800)
    #define MOD_MISC_CTL_PICH_GATING_RATE_FULL   (0x00 << 9)
    #define MOD_MISC_CTL_PICH_GATING_RATE_HALF       (0x01 << 9)
    #define MOD_MISC_CTL_PICH_GATING_RATE_QUARTER    (0x02 << 9)
#endif    
    
      
#if !defined( T_MSM6000 ) 
    #define ENC_IS95C_PCBIT_M      0x0080          /* Test IS95 C PC bits transmitted */
    #define ENC_IS95C_PCBIT_V      0x0080          /*   yes */
    #define ENC_NO_IS95C_PCBIT_V   0x0000          /*   no  */
    #define ENC_ZERO_PCBIT_V       0x0000          /*   no  */
#endif /* not  T_MSM6000  */
    #define ENC_PA_OFF_V      0x00 
    #define ENC_INIT_5MS_BIN0_V     1
    #define ENC_INIT_5MS_BIN3_V     8
    #define ENC_INIT_PCG_0_V          0
    #define ENC_INIT_PCG_1_V        1
    #define ENC_INIT_PCG_3_V        3
    #define ENC_INIT_PCG_T_OFF      15      /* Power control group time offset */
    #define ENC_INIT_WS_T_OFF       0       /* Walsh symbol time offset        */
    #define ENC_FCH_RC1_V                   (0x01 << 2)
    #define ENC_FCH_20MS_FRAME_V            0x02
    
    #define PA_ON_CTL_WB__PA_ON_WRTIE_MASK                                   0x00000002
    #define MOD_FCH_ENC_CTL_WB__FCH_RADIO_CFG_MASK                           0x0000001c
    #define MOD_FCH_ENC_CTL_WB__FCH_FRAME_SIZE_MASK                          0x00000003
    #define MOD_FCH_CTL_WH__FCH_CRC_LENGTH_MASK                              0x00001f00


#endif //  #if defined(FEATURE_FACTORY_TESTMODE)
#endif	
