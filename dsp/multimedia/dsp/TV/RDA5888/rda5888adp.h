#ifndef __RDA5888ADP_H__
#define __RDA5888ADP_H__
#include "tlg1100api.h"

#define RDA_CHIP_VER        (4)

#define RDA5888_FM_ENABLE            // enable fm function
#define RDA5888_SOFTBLENDING_ENABLE  // enable soft blending function
//#define RDA5888_AUTOBLACK_ENABLE   // enable auto black function
#define RDA5888_SYNC_CHECK_ENABLE    // check lock failure by sync signal
//#define RDA5888_HISR_CHECK_ENABLE  // active vsync signal by manual
//#define RDA5888_OUTPUT_SP1_ENABLE    // 搜台过程输出雪花点处理
#define RDA5888_VIDENHANCE_ENABLE      // 降噪处理
//#define RDA5888_INCHFILTER_ENABLE    // 过滤假台的处理

// example: RDA5888-2.0-090814-1.1.35-SPRD
typedef struct tag_version
{
	unsigned char	ProductCode[9]; // product code
	unsigned char	Cx; 			// product major version number
	unsigned char	Cy; 			// product minor version number
	unsigned char	DateTime[7];	// sw release datatime
	unsigned char	Sx; 			// sw major version number
	unsigned char	Sy; 			// sw minor version number
	unsigned char	Sz; 			// sw fix version number
	unsigned char	Remark[8];		// remark for other inforamtion
} rda_version_t;

typedef struct 
{
	uint16 Lim_Low;
	uint16 Lim_UP;
	uint8 var;
} BlendStruct;

#ifndef int16
typedef signed short       int16;
#endif

#ifndef int8
typedef signed char        int8;
#endif

extern uint16 g_nIsTPMode;
extern uint16 g_nTPRegBak;
extern uint8 g_iSignalLevel;
extern uint8 g_nIspHisrCnt;
extern uint8 g_nRdamtvTimer;

#ifdef RDA5888_SOFTBLENDING_ENABLE
extern uint8 g_nSoftBlendingSet;
#endif

#ifdef RDA5888_VIDENHANCE_ENABLE
extern uint8 g_nVideoEnhanceEnable;
extern uint8 g_nVideoEnSet;
extern uint8 g_nDiscardFrameNum;
extern uint32 g_nFrameBufSize;
extern uint8 g_nFirstBufferFlag;
#endif

int RDA_DCDCInit(uint32 base_addr);
short RDA_GetRSSI(void);
uint8 RDA_GetSignalLevel(void);
void RDA_ATVCheckStatus(void);
void RDA_RxOff(uint32 base_addr);
void RDA_RxOn(uint32 base_addr);
void RDA_DigitalInit(uint32 base_addr);
void RDA_Enable(uint32 base_addr);
void RDA_SysReset(void);
void RDA_SetNotchFilter(uint16 mode, uint32 center_freq_khz);
int RDA_SetChnStandard(uint32 base_addr, uint16 mode);
int RDA_ScanTVChn(uint32 base_addr, uint32 center_freq_hz);
void RDA_SetMute(bool bMute);
void RDA_VideoEnhance(unsigned int buffer1, unsigned int buffer2, unsigned int size);
const rda_version_t *RDA_GetVersion(void);

void FMDrv_SetFreq(signed short curf);

#endif
