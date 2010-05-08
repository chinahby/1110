#ifndef _VIM_SYS_CLOCK_H_
#define _VIM_SYS_CLOCK_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "VIM_COMMON_Typedef.h"

#define V8_REG_TIMER_REG_BASE			0x60010000




//-----------------------------------------------------------------------------
// TIMER CTRL

#define V8_REG_TIMER_TMRCTRL			(V8_REG_TIMER_REG_BASE + 0x0508)
#define V8_REG_TIMER_TMRBASE			(V8_REG_TIMER_REG_BASE + 0x050C)
#define V8_REG_TIMER_TMR1TGT			(V8_REG_TIMER_REG_BASE + 0x0510)
#define V8_REG_TIMER_TMR2TGT			(V8_REG_TIMER_REG_BASE + 0x0514)
#define V8_REG_TIMER_TMR1DOUT		(V8_REG_TIMER_REG_BASE + 0x0518)
#define V8_REG_TIMER_TMR2DOUT		(V8_REG_TIMER_REG_BASE + 0x051C)
#define V8_REG_TIMER_WDOGTGT			(V8_REG_TIMER_REG_BASE + 0x0500)
#define V8_REG_TIMER_WDOGDOUT		(V8_REG_TIMER_REG_BASE + 0x0504)


// 820 MP chip macro
#define V5_REG_SYS_PLLCTRL                                    (V5_REG_SYS_BASE+0x0000)
#define V5_REG_SYS_PLL1CKD                                    (V5_REG_SYS_BASE+0x0004)
#define V5_REG_SYS_PLL2CKD                                    (V5_REG_SYS_BASE+0x0008)
#define V5_REG_SYS_PLLSETV                                    (V5_REG_SYS_BASE+0x000c)
#define V5_REG_SYS_PLLSTATUS                                (V5_REG_SYS_BASE+0x0010)
#define V5_REG_SYS_PLLWAIT                                    (V5_REG_SYS_BASE+0x0014)
//CKD and other CLK control
#define V5_REG_SYS_CLKSWITCH                                (V5_REG_SYS_BASE+0x0100)
#define V5_REG_SYS_CKDENCTRL                                (V5_REG_SYS_BASE+0x0104)
#define V5_REG_SYS_CKDBPCTRL                                (V5_REG_SYS_BASE+0x0108)
#define V5_REG_SYS_PHYWAIT                                   (V5_REG_SYS_BASE+0x010c)
#define V5_REG_SYS_SYSCLKCTRL                               (V5_REG_SYS_BASE+0x0110)

#define V5_REG_SYS_CGTMCTRL                              (V5_REG_SYS_BASE+0x0114)
#define V5_REG_SYS_MODCLKCTRL                               (V5_REG_SYS_BASE+0x0118)
#define V5_REG_SYS_CKDCBCON                               (V5_REG_SYS_BASE+0x011c)
#define V5_REG_SYS_CKDVMCON                               (V5_REG_SYS_BASE+0x0120)

#define V5_REG_SYS_CKDPNCON                               (V5_REG_SYS_BASE+0x0124)
#define V5_REG_SYS_CKDATCON                               (V5_REG_SYS_BASE+0x0128)

#define V5_REG_SYS_CKDLSCON                              (V5_REG_SYS_BASE+0x012c)
#define V5_REG_SYS_CKDPSCON                              (V5_REG_SYS_BASE+0x0130)


#define V5_REG_SYS_CKDBYPASSCON                               (V5_REG_SYS_BASE+0x0134)
#define V5_REG_SYS_CKDSCLKCON                              (V5_REG_SYS_BASE+0x0138)

#define V5_REG_SYS_CKDCODECON                               (V5_REG_SYS_BASE+0x013c)
#define V5_REG_SYS_SDRMCLKDL                              (V5_REG_SYS_BASE+0x0140)

//PMU


#define V5_REG_SYS_PMUCTRL                             (V5_REG_SYS_BASE+0x1200)
#define V5_REG_SYS_WAKEUPCTRL                              (V5_REG_SYS_BASE+0x1204)
#define V5_REG_SYS_WPSOURCE                               (V5_REG_SYS_BASE+0x1208)
#define V5_REG_SYS_CSTATUS                             (V5_REG_SYS_BASE+0x120c)
#define V5_REG_SYS_MOLPWCTRL                               (V5_REG_SYS_BASE+0x1210)
#define V5_REG_SYS_RESETLEN                              (V5_REG_SYS_BASE+0x1214)
//SWRST
#define V5_REG_SYS_SWRST                              (V5_REG_SYS_BASE+0x0300)
//RTC
#define V5_REG_SYS_RTCDIN                              (V5_REG_SYS_BASE+0x1400)
#define V5_REG_SYS_RTCYIN                              (V5_REG_SYS_BASE+0x1404)
#define V5_REG_SYS_RTCDOUT                              (V5_REG_SYS_BASE+0x1408)
#define V5_REG_SYS_RTCYOUT                              (V5_REG_SYS_BASE+0x140c)
#define V5_REG_SYS_RTCMAT                              (V5_REG_SYS_BASE+0x1410)
#define V5_REG_SYS_RTCCTRL                              (V5_REG_SYS_BASE+0x1414)
#define V5_REG_SYS_CKDRTCEXTCON                              (V5_REG_SYS_BASE+0x1418)
//TIM
#define V5_REG_SYS_WDOGTGT                              (V5_REG_SYS_BASE+0x0500)
#define V5_REG_SYS_WDOGDOUT                              (V5_REG_SYS_BASE+0x0504)
#define V5_REG_SYS_TMRCTRL                              (V5_REG_SYS_BASE+0x0508)
#define V5_REG_SYS_TMRBASE                             (V5_REG_SYS_BASE+0x050c)
#define V5_REG_SYS_TMR1TGT                              (V5_REG_SYS_BASE+0x0510)
#define V5_REG_SYS_TMR2TGT                              (V5_REG_SYS_BASE+0x0514)
#define V5_REG_SYS_TMR1DOUT                              (V5_REG_SYS_BASE+0x0518)
#define V5_REG_SYS_TMR2DOUT                              (V5_REG_SYS_BASE+0x051c)
//SYSTEM
#define V5_REG_SYS_DEBUGCTRL                              (V5_REG_SYS_BASE+0x1600)
#define V5_REG_SYS_CHIPID                              (V5_REG_SYS_BASE+0x0604)
#define V5_REG_SYS_VERSIONNO                             (V5_REG_SYS_BASE+0x0608)


//define gating
#define VIM_CLKGT_LED		(25)
#define VIM_CLKGT_DMAC		(23)
#define VIM_CLKGT_SDRC		(22)
#define VIM_CLKGT_STO		(21)
#define VIM_CLKGT_HIF		(20)
#define VIM_CLKGT_SIF		(19)
#define VIM_CLKGT_IPP		(18)
#define VIM_CLKGT_LBUF		(17)
#define VIM_CLKGT_JPEG		(16)
#define VIM_CLKGT_ISP		(15)
#define VIM_CLKGT_GE		(14)
#define VIM_CLKGT_VENC		(13)
#define VIM_CLKGT_VDEC		(12)
#define VIM_CLKGT_NFC		(11)
#define VIM_CLKGT_I2S		(10)
#define VIM_CLKGT_TV		(9)
#define VIM_CLKGT_LCD		(8)
#define VIM_CLKGT_USB		(7)
#define VIM_CLKGT_I2C		(6)
#define VIM_CLKGT_PADC		(5)
#define VIM_CLKGT_SPI		(4)
#define VIM_CLKGT_UART		(3)
#define VIM_CLKGT_KEYP		(2)
#define VIM_CLKGT_TIM		(1)	

#define VIM_GATE_ON		(1)	
#define VIM_GATE_OFF		(0)	



typedef enum  tag_VIM_BUS_FREQ
{
SYS_BUS_80M=80,
SYS_BUS_72M=72,
SYS_BUS_64M=64,
SYS_BUS_60M=60,
SYS_BUS_56M=56,
SYS_BUS_54M=54,
SYS_BUS_48M=48,
SYS_BUS_40M=40,
SYS_BUS_36M=36,
SYS_BUS_32M=32,
SYS_BUS_27M=27,
SYS_BUS_24M=24,
SYS_BUS_18M=18,
SYS_BUS_16M=16,
SYS_BUS_3M=3,
SYS_BUS_RESREVED=0x7fffffff
}VIM_BUS_FREQ,*PVIM_BUS_FREQ;
typedef enum  tag_VIM_CPU_FREQ
{
//must pre_div and 3:1 
SYS_CPU240M=240,//USB
SYS_CPU216M=216,//TV
SYS_CPU192M=192,//USB
SYS_CPU168M=168,
SYS_CPU162M=162,//TV
//must be
SYS_CPU144M=144,//USB
SYS_CPU120M=120,//USB
SYS_CPU108M=108,//TV
SYS_CPU96M=96,//USB
SYS_CPU72M=72,//USB
SYS_CPU54M=54,//TV
SYS_CPU48M=48,//USB
SYS_CPU36M=36,
SYS_CPU27M=27,//tv
SYS_CPU24M=24,//USB
SYS_CPU6M=6,
SYS_CPU_RESREVED=0x7fffffff
}VIM_CPU_FREQ,*PVIM_CPU_FREQ;
typedef enum  tag_VIM_PLL_FREQ
{
//must pre_div and 3:1 
SYS_P_PLL480M=480,//USB
SYS_P_PLL432M=432,//TV
SYS_P_PLL384M=384,//USB
SYS_P_PLL336M=336,
SYS_P_PLL324M=324,//TV
//must be
SYS_PLL288M=288,//USB
SYS_PLL240M=240,//USB
SYS_PLL216M=216,//TV
SYS_PLL192M=192,//USB
SYS_PLL144M=144,//USB
SYS_PLL_RESREVED=0x7fffffff
}VIM_PLL_FREQ,*PVIM_PLL_FREQ;
typedef struct   tag_VIM_MODULE_FREQ_SET
{
unsigned int cpu;
unsigned int bus;
unsigned int vid;
unsigned int mpeg;
unsigned int peri;
unsigned int nand;
unsigned int audio;
unsigned int tv;
unsigned int lcd;
unsigned int snr;
unsigned int phy;
unsigned int lvds;
}VIM_MODULE_FREQ_SET,*PVIM_MODULE_FREQ_SET;
typedef enum  tag_VIM_SYS_FREQ
{
	/*	PLL_CPU216M_BUS_72M=0,    
	PLL_CPU192M_BUS_64M=1,
	PLL_CPU144M_BUS_36M=2,
	PLL_CPU240M_BUS_80M=3,
	PLL_CPU264M_BUS_88M=4,
	PLL_CPU256M_BUS_64M=5,
	PLL_CPU72M_BUS_36M=6,
	PLL_CPU72M_BUS_24M=7,
	PLL_CPU108M_BUS_54M=8,
	PLL_CPU54M_BUS_27M=9,
	PLL_CPU96M_BUS_48M=10,
	PLL_CPU48M_BUS_24M=11,
	PLL_CPU64M_BUS_32M=12,
	PLL_CPU36M_BUS_18M=13,
	PLL_CPU36M_BUS_36M=14,
	PLL_CPU24M_BUS_24M=15,
	PLL_CPU54M_BUS_54M=16,*/
//must be 3:1	
	PLL_CPU240M_BUS_80M_USB=(240<<8)|80,
	PLL_CPU216M_BUS_72M_TV=(216<<8)|72,  
	 //not stable PLL_CPU192M_BUS_96M_USB=(192<<8)|96,
	PLL_CPU192M_BUS_64M_USB=(192<<8)|64,
 	PLL_CPU168M_BUS_56M=(168<<8)|56,
 	PLL_CPU162M_BUS_54M_TV=(162<<8)|54,

	PLL_CPU128M_BUS_32M=(128<<8)|32,

	PLL_CPU108M_BUS_54M_TV=(108<<8)|54,
	PLL_CPU108M_BUS_36M_TV=(108<<8)|36,

	PLL_CPU120M_BUS_60_USB=(120<<8)|60,
	PLL_CPU120M_BUS_40_USB=(120<<8)|40,
	PLL_CPU120M_BUS_30M=(120<<8)|30,
	PLL_CPU90M_BUS_30M=(90<<8)|30,

	PLL_CPU96M_BUS_48M_USB=(96<<8)|48,
	PLL_CPU96M_BUS_32M_USB=(96<<8)|32,
	PLL_CPU96M_BUS_24M_USB=(96<<8)|24,
	PLL_CPU72M_BUS_72M_USB=(72<<8)|72,
	PLL_CPU72M_BUS_36M_USB=(72<<8)|36,
	PLL_CPU72M_BUS_24M_USB=(72<<8)|24,

	PLL_CPU64M_BUS_32M=(64<<8)|32,
	PLL_CPU60M_BUS_30M=(60<<8)|30,

//not stable	PLL_CPU54M_BUS_54M_TV=(54<<8)|54,
	PLL_CPU54M_BUS_27M_TV=(54<<8)|27,
	PLL_CPU54M_BUS_18M_TV=(54<<8)|18,

//not stable	PLL_CPU48M_BUS_48M_USB=(48<<8)|48,
	PLL_CPU48M_BUS_24M_USB=(48<<8)|24,
	PLL_CPU48M_BUS_16M_USB=(48<<8)|16,

	PLL_CPU36M_BUS_36M=(36<<8)|36,
	PLL_CPU32M_BUS_32M=(32<<8)|32,
	PLL_CPU30M_BUS_30M=(30<<8)|30,
	PLL_CPU27M_BUS_27M_TV=(27<<8)|27,
	PLL_CPU24M_BUS_24M=(24<<8)|24,
	PLL_CPU6M_BUS_3M_USB=(6<<8)|3,
	PLL_CPU12M_BUS_12M=(12<<8)|12,
	PLL_CPU6M_BUS_6M=(6<<8)|6,
	PLL_SYS_FREQ_RESERVED=0x7FFFFFFF
}VIM_SYS_CPU_BUS_FREQ,*PVIM_SYS_CPU_BUS_FREQ;
typedef enum tag_VIM_SYS_SENSORCLKTYPE
{
	VIM_SYS_SENSOR_CLKNORMAL=0, /* Sensor normal clk */
	VIM_SYS_SENSOR_HALF,
	VIM_SYS_CLKTYPE_UNKOWNED=0x7FFFFFFF /* Sensor half clk ,yuv is 1/4*/
}VIM_SYS_SENSORCLKTYPE;
typedef struct   _MMD_SYS_FREQ
{
       UINT16 cpu;
       UINT16 bus;
       UINT16 vid;
       UINT16 mpeg;
       UINT16 peri;
       UINT16 nand;
	//MMD_U16 audio; /* the audio clk is contorlled by pll2 */
       UINT16 tv;
       UINT16 lcd;
       UINT16 snr;
       UINT16 phy;
       UINT16 lvds;
}MMD_SYS_FREQ;
/* system clk  */
typedef enum _MMD_SYS_CLK
{
	CLK_CPU261M_BUS87M,
	CLK_CPU240M_BUS80M,
	CLK_CPU216M_BUS72M,
	CLK_CPU192M_BUS64M,
	CLK_CPU168M_BUS56M,
	CLK_CPU162M_BUS54M,
	CLK_CPU144M_BUS48M,
	CLK_CPU120M_BUS120M,
	CLK_CPU120M_BUS60M,
	CLK_CPU120M_BUS40M,
	CLK_CPU120M_BUS30M,
	CLK_CPU108M_BUS108M,
	CLK_CPU108M_BUS54M,
	CLK_CPU108M_BUS36M,
	CLK_CPU96M_BUS96M,
	CLK_CPU96M_BUS48M,
	CLK_CPU96M_BUS32M,
	CLK_CPU96M_BUS24M,
	CLK_CPU90M_BUS90M,
	CLK_CPU90M_BUS45M,
	CLK_CPU90M_BUS30M,
	CLK_CPU81M_BUS81M,
	CLK_CPU72M_BUS72M,
	CLK_CPU72M_BUS36M,
	CLK_CPU72M_BUS24M,
	CLK_CPU60M_BUS60M,
	CLK_CPU60M_BUS30M,
	CLK_CPU54M_BUS54M,
	CLK_CPU54M_BUS27M,
	CLK_CPU54M_BUS18M,
	CLK_CPU48M_BUS48M,
	CLK_CPU48M_BUS24M,
	CLK_CPU48M_BUS16M,
	CLK_CPU36M_BUS36M,
	CLK_CPU30M_BUS30M,
	CLK_CPU27M_BUS27M,
	CLK_CPU24M_BUS24M,
	CLK_CPU12M_BUS12M,
	CLK_CPU6M_BUS6M,
	CLK_CPU6M_BUS3M,
	CLK_NUM
}MMD_SYS_CLK;
typedef enum _MMD_CKD
{
       CKD_NULL0,
	CKD_VID,
	CKD_MPEG,
	CKD_PER,
	CKD_NAND,
	CKD_LCD,
	CKD_USB,
	CKD_AUD,
	CKD_TV,
	CKD_NULL1,
	CKD_CODEC,
	CKD_SCLK,
	CKD_BYPASS,
	CKD_SNR
}MMD_CKD;
typedef enum _MMD_CLKGATE
{
        
	 CLKGATE_NULL0=0,
	 CLKGATE_TIM,
	 CLKGATE_KPD,
	 CLKGATE_UART,
	 CLKGATE_SPI,
	 CLKGATE_PAD,
	 CLKGATE_I2C,
	 CLKGATE_USB,
	 CLKGATE_LCD,
	 CLKGATE_TV,
	 CLKGATE_I2S,
	 CLKGATE_NFC,
	 CLKGATE_VDEC,
	 CLKGATE_VENC,
	 CLKGATE_GE,
	 CLKGATE_ISP,
	 CLKGATE_JPEG,
	 CLKGATE_LBUF,
	 CLKGATE_IPP,
	 CLKGATE_SIF,
	 CLKGATE_NULL,
	 CLKGATE_SDIO,
	 CLKGATE_SDRC,
	 CLKGATE_DMAC,
	 CLKGATE_NULL2,
	 CLKGATE_LED,
        CLKGATE_NULL3,
        CLKGATE_NULL4,
        CLKGATE_NULL5,
        CLKGATE_NULL6,
        CLKGATE_NULL7,
        CLKGATE_NULL8,
	/* system clk gate */
	 CLKGATE_NULL9,
        CLKGATE_NULL10,
        CLKGATE_NULL11,
	 CLKGATE_BIU,
	 CLKGATE_MARB,
	 CLKGATE_AHB,
	 CLKGATE_CPU,
	 CLKGATE_RTC,
	 CLKGATE_XCLK1,
	 CLKGATE_NULL12,
        CLKGATE_NULL13,
        CLKGATE_NULL14,
        CLKGATE_NULL15,
        CLKGATE_NULL16,
        CLKGATE_NULL17,
        CLKGATE_NULL18

}MMD_CLKGATE;
/* Define PMU Power Save Mode */
typedef enum _MMD_PS_MODE
{
        MMD_PS_MODE_NORMAL=0,
	 MMD_PS_MODE_IDLE,
	 MMD_PS_MODE_LITESLEEP,
	 MMD_PS_MODE_DEEPSLEEP
}MMD_PS_MODE;
typedef enum _MMD_REG_POWER
{
         REG_POWER_AUDIO_CODEC=0,
	  REG_POWER_VDAC,
	  REG_POWER_1TSRAM,
	  REG_POWER_USB
} MMD_REG_POWER;

typedef enum _MMD_PMU_MODE
{
         PMU_MODE_NORMAL=0,
	  PMU_MODE_BYPASS,
	  PMU_MODE_NUM
	  
} MMD_PMU_MODE;
typedef enum _CLKGATE_MODE
{
	CLKGATE_MODE_NORMAL,
	CLKGATE_MODE_BYPASS,
	CLKGATE_MODE_CODECONLY
}CLKGATE_MODE;
UINT32 VIM_Sys_Get_current_pll(void);
void VIM_SYS_SetSensorClk(VIM_SYS_SENSORCLKTYPE Type);
void VIM_INT_Init( void);
void VIM_INT_Mask(UINT32 vector);
void VIM_INT_Unmask(UINT32 vector);
void VIM_INT_Acknowledge(UINT32 vector);
void VIM_INT_Disable(UINT32 vector);
void VIM_INT_Enable(UINT32 vector);
UINT32 VIM_INT_Get2ndStatus(UINT32 vector);
UINT32 VIM_INT_Get2ndEnable(UINT32 vector);
void VIM_INT_Set2ndEnable(UINT32 vector, UINT32 val);
UINT32 VIM_INT_Get1stStatus(void);
UINT32 VIM_INT_Get1stEnable(void);
void VIM_INT_Set1stEnable(UINT32 flg);
UINT32 timer_getvalue(UINT32 timer_id);
UINT32 VIM_TIM2_GetValue(void);
#ifdef __cplusplus
}
#endif
#endif





