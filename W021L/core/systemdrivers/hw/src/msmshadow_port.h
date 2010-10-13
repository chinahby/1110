#ifndef MSMSHADOW_PORT_H
#define MSMSHADOW_PORT_H
#ifndef _ARM_ASM_

extern word  HWIO_HAPPY_LED_shadow;
extern word  HWIO_HAPPY_LED2_shadow;
extern word  HWIO_FUN_B_shadow;

extern dword HWIO_GPIO1_INT_EN_shadow;
extern dword HWIO_GPIO2_INT_EN_shadow;
extern dword HWIO_GPIO1_INT_POLARITY_shadow;
extern dword HWIO_GPIO2_INT_POLARITY_shadow;
extern dword HWIO_GPIO1_DETECT_CTL_shadow;
extern dword HWIO_GPIO2_DETECT_CTL_shadow;
#ifdef FEATURE_SECOND_UART
extern dword HWIO_UART_SIM_CFG_shadow;
#endif

// CAF 1.27 removed it
extern dword HWIO_RXF_SRCc_DACC_CTL_shadow[2];

extern dword HWIO_RXF_SRCc_BBF_NOTCH_PARAMS_shadow[2];
/* OBJECT libs_xo/rfmsm6500/rflib_adc_samp_freq.o
"..\..\drivers\rf\libs\msm\rflib_adc_samp_freq.c", line 1591: Error: C2456E: undeclared name, inventing 'extern int HWIO_RXF_GPF_COEF_NOTCH1_shadow'
..\..\drivers\rf\libs\msm\rflib_adc_samp_freq.c: 0 warnings, 1 error, 0 serious errors
make[1]: *** [libs_xo/rfmsm6500/rflib_adc_samp_freq.o] Error 1
*/
extern dword HWIO_RXF_GPF_COEF_NOTCH1_shadow;

#endif /*_ARM_ASM_*/

#endif /* MSMSHADOW_PORT_H */
