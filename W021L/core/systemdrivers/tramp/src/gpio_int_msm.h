#ifndef _GPIO_INT_MSM_H_
#define _GPIO_INT_MSM_H_
/*===========================================================================

          G R O U P   I N T E R R U P T   S E R V I C E S
               MSM specific header file

DESCRIPTION
  All the declarations and definitions necessary to support interaction
  with the QUALCOMM CDMA ASIC GROUP interrupt controllers.

REFERENCES
  CDMA Mobile Station ASIC Specification Sheet

Copyright (c) 2006, 2007 by QUALCOMM, Incorporated.  All Rights
Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/tramp/gpio_int_msm.h#3 $  $DateTime: 2008/03/01 14:10:48 $ $Author: vphan $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/20/07   vph     Created from QSC6055.

===========================================================================*/


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/* GPIO table magic */
typedef const void * const hwio_gpio_int_table_t;
extern hwio_gpio_int_table_t hwio_gpio_int_table;

#define HWIO_GPIO_INT_STATUS_ADDR(n) (((gpio_int_info_type*)hwio_gpio_int_table)[n].status_reg)
#define HWIO_GPIO_INT_STATUS_RMSK(n) (((gpio_int_info_type*)hwio_gpio_int_table)[n].status_rmsk)
#define HWIO_GPIO_INT_STATUS_SHFT(n) 0x0
#define HWIO_GPIO_INT_STATUS_INI(n) (in_dword(HWIO_GPIO_INT_STATUS_ADDR(n)))
#define HWIO_GPIO_INT_STATUS_INMI(n,m) (in_dword_masked(HWIO_GPIO_INT_STATUS_ADDR(n),m))
#define HWIO_GPIO_INT_STATUS_OUTI(n,v) out_dword(HWIO_GPIO_INT_STATUS_ADDR(n),v)
#define HWIO_GPIO_INT_STATUS_OUTMI(n,m,v) \
  INTLOCK(); \
  out_dword_masked(HWIO_GPIO_INT_STATUS_ADDR(n),m,v,HWIO_GPIO_INT_STATUS_IN(n)); \
  INTFREE()

#define HWIO_GPIO_INT_DETECT_CTL_ADDR(n) (((gpio_int_info_type*)hwio_gpio_int_table)[n].ctl_reg)
#define HWIO_GPIO_INT_DETECT_CTL_RMSK(n) (((gpio_int_info_type*)hwio_gpio_int_table)[n].ctl_rmsk)
#define HWIO_GPIO_INT_DETECT_CTL_SHFT(n) 0x0
#define HWIO_GPIO_INT_DETECT_CTL_INI(n) (in_dword(HWIO_GPIO_INT_DETECT_CTL_ADDR(n)))
#define HWIO_GPIO_INT_DETECT_CTL_INMI(n,m) (in_dword_masked(HWIO_GPIO_INT_DETECT_CTL_ADDR(n),m))
#define HWIO_GPIO_INT_DETECT_CTL_OUTI(n,v) out_dword(HWIO_GPIO_INT_DETECT_CTL_ADDR(n),v)
#define HWIO_GPIO_INT_DETECT_CTL_OUTMI(n,m,v) \
  INTLOCK(); \
  out_dword_masked_ns(HWIO_GPIO_INT_DETECT_CTL_ADDR(n),m,v,HWIO_GPIO_INT_DETECT_CTL_INI(n)); \
  INTFREE()

#define HWIO_GPIO_INT_POLARITY_ADDR(n) (((gpio_int_info_type*)hwio_gpio_int_table)[n].pol_reg)
#define HWIO_GPIO_INT_POLARITY_RMSK(n) (((gpio_int_info_type*)hwio_gpio_int_table)[n].pol_rmsk)
#define HWIO_GPIO_INT_POLARITY_SHFT(n) 0x0
#define HWIO_GPIO_INT_POLARITY_INI(n) (in_dword(HWIO_GPIO_INT_POLARITY_ADDR(n)))
#define HWIO_GPIO_INT_POLARITY_INMI(n,m) (in_dword_masked(HWIO_GPIO_INT_POLARITY_ADDR(n),m))
#define HWIO_GPIO_INT_POLARITY_OUTI(n,v) out_dword(HWIO_GPIO_INT_POLARITY_ADDR(n),v)
#define HWIO_GPIO_INT_POLARITY_OUTMI(n,m,v) \
  INTLOCK(); \
  out_dword_masked_ns(HWIO_GPIO_INT_POLARITY_ADDR(n),m,v,HWIO_GPIO_INT_POLARITY_INI(n)); \
  INTFREE()

#define HWIO_GPIO_INT_EN_ADDR(n) (((gpio_int_info_type*)hwio_gpio_int_table)[n].mask_reg)
#define HWIO_GPIO_INT_EN_RMSK(n) (((gpio_int_info_type*)hwio_gpio_int_table)[n].mask_rmsk)
#define HWIO_GPIO_INT_EN_SHFT(n) 0x0
#define HWIO_GPIO_INT_EN_INI(n) (in_dword(HWIO_GPIO_INT_EN_ADDR(n)))
#define HWIO_GPIO_INT_EN_INMI(n,m) (in_dword_masked(HWIO_GPIO_INT_EN_ADDR(n),m))
#define HWIO_GPIO_INT_EN_OUTI(n,v) out_dword(HWIO_GPIO_INT_EN_ADDR(n),v)
#define HWIO_GPIO_INT_EN_OUTMI(n,m,v) \
  INTLOCK(); \
  out_dword_masked_ns(HWIO_GPIO_INT_EN_ADDR(n),m,v,HWIO_GPIO_INT_EN_INI(n)); \
  INTFREE()

#define HWIO_GPIO_INT_CLEAR_ADDR(n) (((gpio_int_info_type*)hwio_gpio_int_table)[n].clear_reg)
#define HWIO_GPIO_INT_CLEAR_RMSK(n) (((gpio_int_info_type*)hwio_gpio_int_table)[n].clear_rmsk)
#define HWIO_GPIO_INT_CLEAR_SHFT(n) 0x0
#define HWIO_GPIO_INT_CLEAR_OUTI(n,v) out_dword(HWIO_GPIO_INT_CLEAR_ADDR(n),v)

#define NUM_GROUP_INT_REGISTERS   11

typedef enum 
{
  /* GPIO2 GROUP A */
  GPIO_INT_0 = 0,
  GPIO_INT_1,
  GPIO_INT_2,
  GPIO_INT_3,
  GPIO_INT_4,
  GPIO_INT_5,
  GPIO_INT_6,
  GPIO_INT_7,
  GPIO_INT_8,
  GPIO_INT_9,
  GPIO_INT_10,
  GPIO_INT_11,
  
  /* GPIO1 GROUP 0A */
  GPIO_INT_12,

  /* GPIO2 GROUP B*/
  GPIO_INT_13,

  /* GPIO1 GROUP 0B */
  GPIO_INT_14,
  GPIO_INT_15,
  GPIO_INT_16,
  GPIO_INT_17,
  GPIO_INT_18,
  GPIO_INT_19,
  GPIO_INT_20,
  GPIO_INT_21,
  GPIO_INT_22,
  GPIO_INT_23,
  GPIO_INT_24,
  GPIO_INT_25,
  GPIO_INT_26,
  GPIO_INT_27,
  GPIO_INT_28,
  GPIO_INT_29,
  GPIO_INT_30,
  
  /* GPIO2 GROUP C */
  GPIO_INT_31,
  GPIO_INT_32,
  GPIO_INT_33,

  /* GPIO1 GROUP 0C */
  GPIO_INT_34,
  GPIO_INT_35,
  GPIO_INT_36,
  GPIO_INT_37,
  GPIO_INT_38,
  GPIO_INT_39,
  GPIO_INT_40,
  GPIO_INT_41,
  
  /* GPIO2 GROUP D */
  GPIO_INT_42,
  GPIO_INT_43,
  GPIO_INT_44,
  GPIO_INT_45,
  GPIO_INT_46,
  GPIO_INT_47,
  GPIO_INT_48,
  GPIO_INT_49,
  GPIO_INT_50,
  GPIO_INT_51,
  GPIO_INT_52,
  
  /* GPIO1 GROUP 0D */
  GPIO_INT_53,

  /* GPIO2 GROUP E */
  GPIO_INT_54,

  /* GPIO1 GROUP 0E */
  GPIO_INT_55,
  GPIO_INT_56,
  GPIO_INT_57,
  GPIO_INT_58,
  GPIO_INT_59,

  /* GPIO1 GROUP 1A */
  GPIO_INT_60,
  GPIO_INT_61,
  GPIO_INT_62,
  NUM_GPIO_INTS    
} gpio_int_type;


#endif /* _GPIO_INT_MSM_H_ */
