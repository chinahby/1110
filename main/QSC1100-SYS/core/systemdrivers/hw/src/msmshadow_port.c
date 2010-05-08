#include "comdef.h"


word  HWIO_HAPPY_LED_shadow=0;
word  HWIO_HAPPY_LED2_shadow=0;
word  HWIO_FUN_B_shadow=0;
dword HWIO_GPIO1_INT_EN_shadow=0;
dword HWIO_GPIO2_INT_EN_shadow=0;
dword HWIO_GPIO1_INT_POLARITY_shadow=0;
dword HWIO_GPIO2_INT_POLARITY_shadow=0;
dword HWIO_GPIO1_DETECT_CTL_shadow=0;
dword HWIO_GPIO2_DETECT_CTL_shadow=0;
#ifdef FEATURE_SECOND_UART
dword HWIO_UART_SIM_CFG_shadow=0;
#endif

// CAF 1.27 removed it
dword HWIO_RXF_SRCc_DACC_CTL_shadow[2];

dword HWIO_RXF_SRCc_BBF_NOTCH_PARAMS_shadow[2];

dword HWIO_RXF_GPF_COEF_NOTCH1_shadow=0;

