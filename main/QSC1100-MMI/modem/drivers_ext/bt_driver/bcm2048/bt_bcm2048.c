/*===========================================================================

                    BT Driver for BCM Serial Chipset

DESCRIPTION
  All the declarations and definitions necessary to support interaction
  with the G-Sensor functions for the MSM60x0.

REFERENCES

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

2008/11/21   Tonny Tang Modified.
===========================================================================*/

#include "bt_bcm.h"
#include "nv.h"
#if (defined(FEATURE_PM1000_RTC) || defined(FEATURE_PMIC_RTC))
#include "Pm.h"
//#include "pmapp_rtc.h"
#endif
#define BT_BCM_HOST_WAKE_UP_INT         GPIO_INT_21

#ifdef P_Q1
#define BT_BCM_POWER_ENABLE_CTRL        BIO_GPIO_22_REG
#define BT_BCM_POWER_ENABLE_M           BIO_GPIO_22_M
#else
#define BT_BCM_POWER_ENABLE_CTRL        BIO_GPIO_8_REG
#define BT_BCM_POWER_ENABLE_M           BIO_GPIO_8_M
#endif
#define BT_BCM_POWER_ENABLE_ENA         BIO_OUTPUT_SET_V
#define BT_BCM_POWER_ENABLE_DIS         BIO_OUTPUT_CLR_V

#define BT_BCM_RESET_CTRL        BIO_GPIO_48_REG
#define BT_BCM_RESET_M           BIO_GPIO_48_M
#define BT_BCM_RESET_ENA         BIO_OUTPUT_SET_V
#define BT_BCM_RESET_DIS         BIO_OUTPUT_CLR_V

#define BT_BCM_EXT_LDO_ENABLE_CTRL      BIO_GPIO_32_REG
#define BT_BCM_EXT_LDO_ENABLE_M         BIO_GPIO_32_M
#define BT_BCM_EXT_LDO_ENABLE_ENA       BIO_OUTPUT_SET_V
#define BT_BCM_EXT_LDO_ENABLE_DIS       BIO_OUTPUT_CLR_V

#define BT_BCM_WAKEUP_DEVICE_CTRL       BIO_GPIO_53_REG
#define BT_BCM_WAKEUP_DEVICE_M          BIO_GPIO_53_M
#define BT_BCM_WAKEUP_DEVICE_ENA        BIO_OUTPUT_SET_V
#define BT_BCM_WAKEUP_DEVICE_DIS        BIO_OUTPUT_CLR_V


#define BRCM_BT_WAIT_NV_SIG         0x00020000

/*=========================================================================*/
extern void siors_uart_client_isr(void);
extern void siors_uart_enable_warmup(boolean);

void hs_bt_host_wakup_int_isr( void );


/*===========================================================================

FUNCTION hs_bt_host_wakup_int_isr

DESCRIPTION
  This function handle the host wake up interrupt.
  
PARAMETERS
  None.
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void hs_bt_host_sleep_int_isr( void )
{
    MSG_FATAL("hs_bt_host_sleep_int_isr", 0, 0, 0);

    gpio_int_set_handler(BT_BCM_HOST_WAKE_UP_INT, ACTIVE_HIGH, NULL);

    //set RTS as GPIO, pull high RTS of UART
    gpio_tlmm_config(GPIO_OUT_56);
    BIO_OUT( BIO_GPIO_56_REG, BIO_GPIO_56_M, BIO_GPIO_56_HI_V );
    BIO_TRISTATE(BIO_GPIO_56_REG, BIO_GPIO_56_M, BIO_GPIO_56_HI_V);

/*
    gpio_tlmm_config(GPIO_OUT_3);
    BIO_TRISTATE(BIO_GPIO_3_REG, BIO_GPIO_3_M, BIO_GPIO_3_LO_V);
    gpio_tlmm_config(GPIO_OUT_4);
    BIO_TRISTATE(BIO_GPIO_4_REG, BIO_GPIO_4_M, BIO_GPIO_4_LO_V);
    gpio_tlmm_config(GPIO_OUT_5);
    BIO_TRISTATE(BIO_GPIO_5_REG, BIO_GPIO_5_M, BIO_GPIO_5_LO_V);
    gpio_tlmm_config(GPIO_OUT_3);
    BIO_TRISTATE(BIO_GPIO_6_REG, BIO_GPIO_6_M, BIO_GPIO_6_LO_V);
*/

    siors_uart_enable_warmup(FALSE);
    
    SLEEP_ASSERT_OKTS( SLEEP_BT_OKTS_SIG );

    gpio_int_set_handler(BT_BCM_HOST_WAKE_UP_INT, ACTIVE_LOW, hs_bt_host_wakup_int_isr); 
}

/*===========================================================================

FUNCTION hs_bt_host_wakup_int_isr

DESCRIPTION
  This function handle the host wake up interrupt.
  
PARAMETERS
  None.
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void hs_bt_host_wakup_int_isr( void )
{ 
    MSG_FATAL("hs_bt_host_wakup_int_isr", 0, 0, 0);
    
    gpio_int_set_handler(BT_BCM_HOST_WAKE_UP_INT, ACTIVE_LOW, NULL);

    SLEEP_NEGATE_OKTS( SLEEP_BT_OKTS_SIG );

    siors_uart_enable_warmup(TRUE);

    // Set GPIO56 as RTS. 
    gpio_tlmm_config(UART1_RFR_N);

    /*
    gpio_tlmm_un_config(
    gpio_tlmm_config(AUX_PCM_SYNC);
    BIO_TRISTATE(BIO_GPIO_4_REG, BIO_GPIO_4_M, BIO_GPIO_4_LO_V);
    gpio_tlmm_config(AUX_PCM_DIN);
    BIO_TRISTATE(BIO_GPIO_5_REG, BIO_GPIO_5_M, BIO_GPIO_5_LO_V);
    gpio_tlmm_config(AUX_PCM_DOUT);
    BIO_TRISTATE(BIO_GPIO_6_REG, BIO_GPIO_6_M, BIO_GPIO_6_LO_V);
    gpio_tlmm_config(AUX_PCM_CLK);
    */
   
    gpio_int_set_handler(BT_BCM_HOST_WAKE_UP_INT, ACTIVE_HIGH, hs_bt_host_sleep_int_isr);
}

/*===========================================================================

FUNCTION hs_host_wakeup_int_enable

DESCRIPTION
  This function control the GPIO to open/close power supply to BT device
  
PARAMETERS
  boolean bEnable [IN]: TRUE = Enable, FALSE = Disable.
  
DEPENDENCIES
  hs_init_bt_host_ctrl must be called before.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void hs_host_wakeup_int_enable(boolean bEnable)
{
       MSG_FATAL("hs_host_wakeup_int_enable: %d", bEnable, 0, 0);
       
       if(bEnable)
       {
             //Set the Host interrupt detect pad and isr    
            gpio_int_set_detect  ( BT_BCM_HOST_WAKE_UP_INT, DETECT_LEVEL );
            gpio_int_set_handler ( BT_BCM_HOST_WAKE_UP_INT, ACTIVE_HIGH, hs_bt_host_sleep_int_isr);
       }
       else
       {
            gpio_int_set_handler(BT_BCM_HOST_WAKE_UP_INT, ACTIVE_HIGH, NULL);
            gpio_int_set_handler(BT_BCM_HOST_WAKE_UP_INT, ACTIVE_LOW, NULL);
       }
}


/*===========================================================================

FUNCTION hs_init_bt_host_ctrl

DESCRIPTION
  This function init the GPIO pads which use to control BT device
  
PARAMETERS
  None.
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void hs_init_bt_host_ctrl(void)
{
     //Make the power enable signal to disable and enable output of GPIO pad
    BIO_OUT( BT_BCM_POWER_ENABLE_CTRL, BT_BCM_POWER_ENABLE_M, BT_BCM_POWER_ENABLE_DIS);  
    BIO_TRISTATE( BT_BCM_POWER_ENABLE_CTRL, BT_BCM_POWER_ENABLE_M, BIO_OUTPUT_ENA_V);

    //Make the reset signal to disable and enable output of GPIO pad
    BIO_OUT( BT_BCM_RESET_CTRL, BT_BCM_RESET_M, BT_BCM_RESET_DIS);  
    BIO_TRISTATE( BT_BCM_RESET_CTRL, BT_BCM_RESET_M, BIO_OUTPUT_ENA_V);
    
    //Make the extern LDO enable signal to disable and enable output of GPIO pad
    BIO_OUT( BT_BCM_EXT_LDO_ENABLE_CTRL, BT_BCM_EXT_LDO_ENABLE_M, BT_BCM_EXT_LDO_ENABLE_DIS);  
    BIO_TRISTATE( BT_BCM_EXT_LDO_ENABLE_CTRL, BT_BCM_EXT_LDO_ENABLE_M, BIO_OUTPUT_ENA_V);    

    //Make the wake up signal to disable and enable output of GPIO pad
    BIO_OUT( BT_BCM_WAKEUP_DEVICE_CTRL, BT_BCM_WAKEUP_DEVICE_M, BT_BCM_WAKEUP_DEVICE_DIS);  
    BIO_TRISTATE( BT_BCM_WAKEUP_DEVICE_CTRL, BT_BCM_WAKEUP_DEVICE_M, BIO_OUTPUT_ENA_V);
}

/*===========================================================================

FUNCTION hs_open_bt_device

DESCRIPTION
  This function control the GPIO to open/close power supply to BT device
  
PARAMETERS
  boolean bIsOpen [IN]: TRUE = Open, FALSE = CLOSE.
  
DEPENDENCIES
  hs_init_bt_host_ctrl must be called before.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void hs_open_bt_device(boolean bIsOpen)
{
    if(bIsOpen)
    {
        BIO_TRISTATE( BT_BCM_EXT_LDO_ENABLE_CTRL, BT_BCM_EXT_LDO_ENABLE_M, BIO_OUTPUT_ENA_V);    
        BIO_OUT( BT_BCM_EXT_LDO_ENABLE_CTRL, BT_BCM_EXT_LDO_ENABLE_M, BT_BCM_EXT_LDO_ENABLE_ENA);
        
        BIO_TRISTATE( BT_BCM_POWER_ENABLE_CTRL, BT_BCM_POWER_ENABLE_M, BIO_OUTPUT_ENA_V);
        BIO_OUT( BT_BCM_POWER_ENABLE_CTRL, BT_BCM_POWER_ENABLE_M, BT_BCM_POWER_ENABLE_ENA);

        //dog_kick();
        rex_sleep(200);
        
        BIO_TRISTATE( BT_BCM_RESET_CTRL, BT_BCM_RESET_M, BIO_OUTPUT_ENA_V);
        BIO_OUT( BT_BCM_RESET_CTRL, BT_BCM_RESET_M, BT_BCM_RESET_DIS);
        rex_sleep(20);
        BIO_OUT( BT_BCM_RESET_CTRL, BT_BCM_RESET_M, BT_BCM_RESET_ENA);
    }
    else
    {
        BIO_TRISTATE( BT_BCM_POWER_ENABLE_CTRL, BT_BCM_POWER_ENABLE_M, BIO_OUTPUT_ENA_V);
        BIO_OUT( BT_BCM_POWER_ENABLE_CTRL, BT_BCM_POWER_ENABLE_M, BT_BCM_POWER_ENABLE_DIS);  
        
        BIO_TRISTATE( BT_BCM_RESET_CTRL, BT_BCM_RESET_M, BIO_OUTPUT_ENA_V);
        BIO_OUT( BT_BCM_RESET_CTRL, BT_BCM_RESET_M, BT_BCM_RESET_DIS);
        
        BIO_TRISTATE( BT_BCM_EXT_LDO_ENABLE_CTRL, BT_BCM_EXT_LDO_ENABLE_M, BIO_OUTPUT_ENA_V);  
        BIO_OUT( BT_BCM_EXT_LDO_ENABLE_CTRL, BT_BCM_EXT_LDO_ENABLE_M, BT_BCM_EXT_LDO_ENABLE_DIS);  
    }
}

/*===========================================================================

FUNCTION hs_bt_wakeup

DESCRIPTION
  This function control the GPIO to open/close power supply to BT device
  
PARAMETERS
  boolean bIsWakeup [IN]: TRUE = Wake Up Device, FALSE = Make Device Sleep.
  
DEPENDENCIES
  hs_init_bt_host_ctrl must be called before.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void hs_bt_wakeup(boolean bIsWakeup)
{
    if(bIsWakeup)
    {
        MSG_FATAL("hs_bt_wakeup: wakeup", 0, 0, 0);        
        //BIO_TRISTATE(BT_BCM_WAKEUP_DEVICE_CTRL, BT_BCM_WAKEUP_DEVICE_M ,BT_BCM_WAKEUP_DEVICE_ENA);
        BIO_OUT( BT_BCM_WAKEUP_DEVICE_CTRL, BT_BCM_WAKEUP_DEVICE_M, BT_BCM_WAKEUP_DEVICE_DIS);
    }
    else
    {
        MSG_FATAL("hs_bt_wakeup: sleep", 0, 0, 0);
        //BIO_TRISTATE(BT_BCM_WAKEUP_DEVICE_CTRL, BT_BCM_WAKEUP_DEVICE_M, BT_BCM_WAKEUP_DEVICE_ENA);
        BIO_OUT( BT_BCM_WAKEUP_DEVICE_CTRL, BT_BCM_WAKEUP_DEVICE_M, BT_BCM_WAKEUP_DEVICE_ENA);
     }
}
#if 0
/*===========================================================================

FUNCTION AscII_to_bt_addr

DESCRIPTION
  This function transfer a asii to a bt address format.
  
PARAMETERS
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static void AscII_to_bt_addr(char *str, uint16 str_len, char * out_buffer) 
{
    uint8 value[2];
    int i=0;
    int k=0;

    for(i=0; i<str_len; i+=2)
    {
        for(k=0; k<2; k++)
        {
            if((*(str+i+k))>='0' && (*(str+i+k))<='9')
            {
                value[k] = (*(str+i+k)) - 0x30;
            }
            else if((*(str+i+k))>='a' && (*(str+i+k))<='f')
            {
                value[k] = (*(str+i+k)) - 'a' + 10;
            }
            else if((*(str+i+k))>='A' && (*(str+i+k))<='F')
            {
                value[k] = (*(str+i+k)) - 'A' + 10;
            }
            else
            {
                return;
            }
        }
        out_buffer[i>>1] = (char)(0x10*value[0] + value[1]);
    }
}
#endif
/*===========================================================================

FUNCTION btui_get_bt_addr

DESCRIPTION
  This function get the BT address from Host EFS
  
PARAMETERS
  byte* my_bd_addr [IO]: ptr to address buffer
  int   bd_size    [IN]: buffer size 
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
boolean btui_get_bt_addr(byte *my_bd_addr, int bd_size)
{
    nv_item_type nvi;
    //char bt_addr_char[14] = { 0 };
    //char bt_addr_new[NV_BD_ADDR_SIZE+1] = {0};
    nv_cmd_type   bd_addr_nv_cmd;
    
    /* Read MY_BD_ADDR from NV */
    bd_addr_nv_cmd.item       = NV_BD_ADDR_I;
    bd_addr_nv_cmd.cmd        = NV_READ_F;
    bd_addr_nv_cmd.data_ptr   = &nvi;
    bd_addr_nv_cmd.tcb_ptr    = rex_self();
    bd_addr_nv_cmd.sigs       = BRCM_BT_WAIT_NV_SIG;
    bd_addr_nv_cmd.done_q_ptr = NULL;
    
    (void) rex_clr_sigs( rex_self(), BRCM_BT_WAIT_NV_SIG );
    nv_cmd( &bd_addr_nv_cmd );
    (void) rex_wait( BRCM_BT_WAIT_NV_SIG );
    (void) rex_clr_sigs( rex_self(), BRCM_BT_WAIT_NV_SIG );     

    if (bd_addr_nv_cmd.status == NV_DONE_S)
    {
        //int len = strlen((char *)nvi.bd_addr.bd_addr);
        
        //len = (len<=13) ? len : 13;        
        //memcpy (bt_addr_new, (void *) nvi.bd_addr.bd_addr, NV_BD_ADDR_SIZE);
        //AscII_to_bt_addr(bt_addr_char, len, bt_addr_new);

        memcpy(my_bd_addr, (void *) nvi.bd_addr.bd_addr,  bd_size);
        return TRUE;
    }
    else
    {
        byte bd_addr[NV_BD_ADDR_SIZE+1] = { 0x34, 0x12, 0x78, 0x56, 0xbc, 0x9a };
#if (defined(FEATURE_PM1000_RTC) || defined(FEATURE_PMIC_RTC))
        pm_rtc_julian_type time = {0};
        pm_rtc_rw_cmd(PM_RTC_GET_CMD, &time);
        bd_addr[0] = (byte)time.second&0x00FF;
        bd_addr[1] = (byte)time.minute&0x00FF;
        bd_addr[2] = (byte)time.hour&0x00FF;
        bd_addr[3] = (byte)time.day&0x00FF;
        bd_addr[4] = (byte)time.month&0x00FF;
        bd_addr[5] = (byte)time.year&0x00FF;
#endif
        memcpy(my_bd_addr, bd_addr,  bd_size);
    }
    return FALSE;
}

