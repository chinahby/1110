#include "rda5888adp.h"
#include "analog_tv.h"

#include "TimerEvents.h"
#include "visualhisr.h"
#include "kal_release.h"
#include "Lcd_if.h"

#define CHECK_SEEK_READY_TIMES          6
#define CHECK_SYNC_FAIL_TIMES           5

uint16 g_nIsTPMode = 0; // 1: output test pattern data, 0: normal data
uint16 g_nTPRegBak = 0;
uint8 g_iSignalLevel = 0;
uint32 m_nFreqOffset;
unsigned char m_bIs625Lines = 1;
uint8 g_nIspHisrCnt = 0;
uint8 g_nRdamtvTimer = 0;

#ifdef RDA5888_SOFTBLENDING_ENABLE
uint8 g_nSoftBlendingSet = 1;
#endif

// example: RDA5888-2.0-090814-1.1.35-SPRD
const rda_version_t g_stRdaVersion = 
{	
	"RDA5888", // product code
	4,		   // product major version number
	0,		   // product minor version number
	"100109",  // sw release datetime(yy-mm-dd)
	2,		   // sw major version number
	8,		   // sw minor version number
	8,		   // sw fix version number
	"MTK"	   // remark for other inforamtion
};

// Revision
// wuhp@2009-11-02, 1) stop timer after power off atv module.
// wuhp@2009-11-12, 1) modified freq_offset for SECAM_B standard. 
//                  2) add PAL_SECAM hybrid standard.
//                  3) add switch to enable/disable softblending and autoblack function.
//                  4) add a function to mute/unmute RDA5888.
// wuhp@2009-11-13, 1) fixed bug that will making noise when entry ATV module first time.
// wuhp@2009-11-16, 1) deleted inactive code.

#ifdef RDA5888_SOFTBLENDING_ENABLE
#define RATE 6
BlendStruct BlendTable[11]={{0  , 80 ,0x2},
							      {80,200,0x2},
							      {200,280,0x0},
							      {280,360,0x0},
							      {360,440,0xE},
							      {440,520,0xE},
							      {520,600,0xC},
							      {600,680,0xC},
							      {680,800,0xA},
							      {800,920,0x9},
							      {920,32768,0x8}};


static bool get_mid(uint16 DataIn,uint16* mid_var)
{
	static char count=1;
	static uint16 Mid_buff[3];
	uint16 max_var,min_var,i;
	Mid_buff[2]=Mid_buff[1];
	Mid_buff[1]=Mid_buff[0];
	Mid_buff[0]=DataIn;
	if(count<3)
	{
		count++;
		return 0;
	}
      * mid_var=(Mid_buff[0] < Mid_buff[1] ? 
      (Mid_buff[1] < Mid_buff[2] ? Mid_buff[1] :Mid_buff[0] < Mid_buff[2] ? Mid_buff[2] : Mid_buff[0]) :
      (Mid_buff[1] > Mid_buff[2] ? Mid_buff[1] : Mid_buff[0] > Mid_buff[2]  ? Mid_buff[2] :Mid_buff[0]));  
	// kal_prompt_trace(0," GET MID START----------\n");  
	//for(i=0;i<3;i++)
     	//	 kal_prompt_trace(0," Mid_buff[%x]=%x\n",i,Mid_buff[i]);
	 //kal_prompt_trace(0," *****mid_var=%x\n",* mid_var);
	// kal_prompt_trace(0," GET MID end----------\n"); 
	return 1;
}

static bool average(uint16 DataIn,uint16* AveVar)
{
	static char count=0;
	//static uint16 avebuff[32];
	static uint32 sum=0;
	
	sum+=DataIn;
	count++;
	if(count<RATE)
	{
		return 0;
	}

	//kal_prompt_trace(0," average32 START----------\n");

	*AveVar=sum*8/RATE;
	//kal_prompt_trace(0," average=%d ----------\n",*AveVar);
	//kal_prompt_trace(0," average32 END----------\n");
	count=0;
	sum=0;
	return 1;
}

static uint8 search_blend_table(uint16 var , BlendStruct* BlendTable,signed short len,uint8* p_blendVar )
{
	int i;
	static int LastIndex=-1;
	uint16 Lim_UP,Lim_Low;
	
	if(LastIndex!=-1)
	{
		for(i=LastIndex-1;(BlendTable[LastIndex].var==BlendTable[i].var)&&(i>=0);i--);
		Lim_Low= (BlendTable[i+1].Lim_Low-3*8) <BlendTable[i+1].Lim_Low ? BlendTable[i+1].Lim_Low-3*8:BlendTable[i+1].Lim_Low;
		for(i=LastIndex+1;(BlendTable[LastIndex].var==BlendTable[i].var)&&(i<len);i++);
		Lim_UP= (BlendTable[i-1].Lim_UP+3*8)>BlendTable[i-1].Lim_UP ? BlendTable[i-1].Lim_UP+3*8:BlendTable[i-1].Lim_UP;
		if(var>=Lim_Low&&var<=Lim_UP)	
			return 0;
	}
	
	//kal_prompt_trace(0," var=%d ----------\n",var);
	for(i=0;i<len;i++)
	{
		if(var>BlendTable[i].Lim_UP)
		{
			//kal_prompt_trace(0," search table........BlendTable[%d].Lim_UP=%x----------\n", i,BlendTable[i].Lim_UP);;
		}else{
			//kal_prompt_trace(0," BlendTable[%d]=%x----------\n", i,BlendTable[i].var);
			LastIndex=i;
			*p_blendVar=BlendTable[i].var;
			return  1;
		}
	}
	//kal_prompt_trace(0," BlendTable[len-1]=%x----------\n",BlendTable[i].var);
	*p_blendVar= BlendTable[len-1].var;
	return 1;
}

static void soft_blending(void)
{
	uint16 reg,var,midvar,AveVar;
	uint8 blendVar;
	TLG_WriteReg(0x62, 0x12f, 0x0075);
	TLG_ReadReg(0x62,0x117,&reg);

	//kal_prompt_trace(0," 117=%x----------\n",reg);
	
	TLGHAL_GET(var,reg,0x03,0xfff8);
	
	if(!get_mid(var,&midvar))
	{
		return;
	}

	if(!average(midvar,&AveVar))
	{
		return;
	}

	if(!search_blend_table(AveVar , BlendTable,11,&blendVar))
		return;

	TLG_ReadReg(0x62,0x10b,&reg);
	TLGHAL_SET(reg,blendVar,4,0xf0);
	TLG_WriteReg(0x62, 0x10b, reg);
	TLG_ReadReg(0x62,0x10b,&reg);
	//kal_prompt_trace(0," 10b=%x----------\n", reg);	
}
#endif

static void check_hsync_state(void)
{
	uint16 reg, p1, p2;
	uint16 level = 10;
	
	TLG_ReadReg(0x62,0x117,&reg);
	TLGHAL_GET(p1,reg,2,0x7ffc);
	TLG_Delay(20);
	TLG_ReadReg(0x62,0x117,&reg);
	TLGHAL_GET(p2,reg,2,0x7ffc);

	if (p1 != 0)
		p1 = 8192 - p1;
	
	if (p2 != 0)
		p2 = 8192 - p2;

	p1 = (p1 > p2 ? p1 : p2);

	if (p1 < 10)
	{
		kal_prompt_trace(0,"reset_hsync_state\n");
		TLG_WriteReg(0x62, 0x11e, 0x61a2);
		//TLG_Delay(10);
		TLG_WriteReg(0x62, 0x11e, 0x6182);
	}
}


static void dsp_system_reset(uint8 mode)
{
    switch (mode)
    {
        case 0:
			TLG_WriteReg(0x62, 0x130, 0x0014);
			TLG_Delay((uint16)5);
			TLG_WriteReg(0x62, 0x130, 0x0814);			
			break;

        case 1:
			TLG_WriteReg(0x62, 0x130, 0x0015);
			TLG_Delay((uint16)5);
			TLG_WriteReg(0x62, 0x130, 0x0815);	
			break;

        case 2:
			TLG_WriteReg(0x62, 0x130, 0x0016);
			TLG_Delay((uint16)5);
			TLG_WriteReg(0x62, 0x130, 0x0816);				
			break;

        default:		
			break;				
    }
}

static void calc_freq_offset(uint16 mode)
{
	switch (mode)
	{
		case TLG_VSTD_PAL_G:
		case TLG_VSTD_PAL_I:
		case TLG_VSTD_SECAM_G:
		case TLG_VSTD_PAL_SECAM_G:			
			m_nFreqOffset = 625;
			m_bIs625Lines = 1;
			break;
			
		case TLG_VSTD_PAL_D:
		case TLG_VSTD_PAL_D1:
		case TLG_VSTD_PAL_K:	
		case TLG_VSTD_PAL_B:
		case TLG_VSTD_PAL_B1:
		case TLG_VSTD_SECAM_B:
		case TLG_VSTD_SECAM_B1:
		case TLG_VSTD_PAL_SECAM_B:
		case TLG_VSTD_PAL_SECAM_B1:
   		case TLG_VSTD_SECAM_D:
		case TLG_VSTD_SECAM_D1:
		case TLG_VSTD_SECAM_K:
		case TLG_VSTD_PAL_SECAM_D:
		case TLG_VSTD_PAL_SECAM_D1:
		case TLG_VSTD_PAL_SECAM_K:			
			m_nFreqOffset = 125;
			m_bIs625Lines = 1;
			break;

		case TLG_VSTD_PAL_N:
		case TLG_VSTD_PAL_NC:
			m_nFreqOffset = 25;
			m_bIs625Lines = 1;			
			break;
			
        case TLG_VSTD_NTSC_B_G:
		case TLG_VSTD_NTSC_I:
			m_nFreqOffset = 625;
			m_bIs625Lines = 0;			
			break;
			
        case TLG_VSTD_NTSC_D_K:
			m_nFreqOffset = 125;
			m_bIs625Lines = 0;			
		    break;

		case TLG_VSTD_NTSC_M:
		case TLG_VSTD_PAL_M:
			m_nFreqOffset = 25;
			m_bIs625Lines = 0;
			break;
			
		default:
			m_nFreqOffset = 0;
			m_bIs625Lines = 0;			
			break;
	}

    kal_prompt_trace(0, "[rdamtv]calc_freq_offset, m_nFreqOffset = %d\n", m_nFreqOffset);	
}

#ifdef RDA5888_INCHFILTER_ENABLE
#define CHANNEL_FILTER_SAMP_NUM      5
#define CHANNEL_FILTER_THRESHOLD   145
static int InvalidChannelFilter(void)
{
    int var[CHANNEL_FILTER_SAMP_NUM], i, j, temp, exchange;
	int rt = 1;
	uint16 reg;

	TLG_Delay(100);

    TLG_WriteReg(0x62, 0x12f, 0x0076);
	for (i = 0; i < CHANNEL_FILTER_SAMP_NUM; i++)
	{
	    TLG_Delay(10);
		TLG_ReadReg(0x62, 0x117, &reg);
		TLGHAL_GET(var[i], reg, 3, 0x7FF8);
	}

	for (i = 0; i < CHANNEL_FILTER_SAMP_NUM; i++)
	{
		exchange = 0;
		for (j = CHANNEL_FILTER_SAMP_NUM-1; j > i; j--)
		{
		    if (var[j] > var[j-1])
		    {
				exchange = 1;
		        temp = var[j];
				var[j] = var[j-1];
				var[j-1] = temp;
		    }
		}
		if (!exchange)
			break;
	}
	
	temp = var[CHANNEL_FILTER_SAMP_NUM/2];
	
	kal_prompt_trace(0,"[rdamtv]InvalidChannel, temp = %d, var = %d, %d, %d, %d, %d\n", temp, var[0],var[1],var[2],var[3],var[4]);
	
	if (temp > CHANNEL_FILTER_THRESHOLD)
	{
		rt = 0;
		kal_prompt_trace(0,"[rdamtv]InvalidChannel done!!\n");
	}
	else if (temp == 0)
	{
	    TLG_WriteReg(0x62, 0x12f, 0x0050);
		TLG_ReadReg(0x62, 0x117, &reg);
		TLGHAL_GET(var[i], reg, 7, 0x0780);
		
		if ((i != 8)&&(i != 9)) // sync lock fail
		{
			rt = 0;
			kal_prompt_trace(0,"[rdamtv]InvalidChannel done!!!!\n");
		}
	}

	return rt;
}
#endif

int RDA_DCDCInit(uint32 base_addr)
{
	unsigned short reg;

	kal_prompt_trace(0,"[rdamtv] RDA_DCDCInit, Ver = %d\n", RDA_CHIP_VER);

    base_addr = 0x62;

	TLG_ReadReg(base_addr,  0x0,&reg);
	kal_prompt_trace(0,"[rdamtv]chipid: %x\n", reg); // 0x31E2

#if (RDA_CHIP_VER == 2)
	TLG_WriteReg(base_addr, 0x001, 0x0000);//9'0x01,16'h0000;
	TLG_WriteReg(base_addr, 0x001, 0x0115);//9'h01,16'h0115;soft reset
	TLG_Delay((uint16)100);

	TLG_WriteReg(base_addr, 0x001, 0x091B);//9'h001,16'h091B; agc_en_b
	TLG_WriteReg(base_addr, 0x053, 0x0e50);//9'h053,16'h0e50; lna current
	TLG_WriteReg(base_addr, 0x05a, 0x0100);//9'h05a,16'h0100; lna_th0
	TLG_WriteReg(base_addr, 0x05b, 0x0280);//9'h05b,16'h0280; lna_th1
	TLG_WriteReg(base_addr, 0x062, 0x053f);//9'h62,16'h053f; vco high low th 705M
	TLG_WriteReg(base_addr, 0x066, 0x0470);//9'h66,16'h0470; cfc th0
	TLG_WriteReg(base_addr, 0x067, 0x0538);//9'h67,16'h0538; cfc th1
	TLG_WriteReg(base_addr, 0x068, 0x0648);//9'h68,16'h0648; cfc th2
	TLG_WriteReg(base_addr, 0x06a, 0xcccc);//9'h6a,16'hcccc; rfpll_gain0~3
    TLG_WriteReg(base_addr, 0x072, 0xaaaa);//9'h072,16'hAAAA; vco ibit 1010
	TLG_WriteReg(base_addr, 0x08e, 0x3400);//9'h8e,16'h3400; int_dec_sel 00
	TLG_WriteReg(base_addr, 0x086, 0x1045);//9'h86,16'h1045; mdll_test 1
	TLG_WriteReg(base_addr, 0x080, 0x004d);//9'h80,16'h004d; adc_sel_clk 1
	TLG_WriteReg(base_addr, 0x088, 0x4824);//9'h088,16'h4324; xtal cap
	TLG_WriteReg(base_addr, 0x098, 0x0400);//9'h098,16'h0400; os_en, dc_i
	TLG_WriteReg(base_addr, 0x099, 0x0000);//9'h099,16'h0000; dc_q

	TLG_WriteReg(base_addr, 0x042, 0x0924);//9'h42,16'h0FF4; analog gain
	TLG_WriteReg(base_addr, 0x043, 0x0FFC);//9'0x43,16'0x0FFF;
	TLG_WriteReg(base_addr, 0x044, 0x0000);//9'0x44,16'0x1000;
	TLG_WriteReg(base_addr, 0x045, 0xF700);//9'0x45,16'0xFF73;
	
	TLG_WriteReg(base_addr, 0x046, 0x0000);//9'0x46,16'0x0000;
	TLG_WriteReg(base_addr, 0x047, 0x0000);//9'0x47,16'0x0000;
	TLG_WriteReg(base_addr, 0x048, 0x0000);//9'0x48,16'0x0000;
	TLG_WriteReg(base_addr, 0x049, 0x0000);//9'0x49,16'0x0000;
	TLG_WriteReg(base_addr, 0x04A, 0x0000);//9'0x4A,16'0x0000;
	TLG_WriteReg(base_addr, 0x04B, 0x0000);//9'0x4B,16'0x0000;
	TLG_WriteReg(base_addr, 0x04C, 0x28FF);//9'H04C,16'H28F3; // inc adac gain 0x28F6 0x28FF
	TLG_WriteReg(base_addr, 0x04D, 0x00F0);//9'0x4D,16'0x00F0;
	TLG_WriteReg(base_addr, 0x04E, 0x0000);//9'0x4E,16'0x0000;
	TLG_WriteReg(base_addr, 0x04F, 0xC000);//9'0x4F,16'0xC000;
	TLG_WriteReg(base_addr, 0x050, 0x0000);//9'0x50,16'0x0000;
	TLG_WriteReg(base_addr, 0x051, 0x0000);//9'0x51,16'0x0000;
	TLG_WriteReg(base_addr, 0x052, 0x0000);//9'0x52,16'0x0000;

    //TLG_WriteReg(base_addr, 0x0a0, 0x0054);
	TLG_WriteReg(base_addr, 0x0a1, 0x0085);//9'h0a1,16'h008B; bit_8: dsp_pd_mode 0x018b 0x018A
#elif (RDA_CHIP_VER == 4)
	TLG_WriteReg(base_addr, 0x001, 0x0000);//9'0x01,16'h0000;
	TLG_WriteReg(base_addr, 0x001, 0x0115);//9'h01,16'h0115;soft reset
	TLG_Delay((uint16)10);

	TLG_WriteReg(base_addr, 0x001, 0x091B);//9'h001,16'h091B; agc_en_b
	TLG_WriteReg(base_addr, 0x053, 0x0e50);//9'h053,16'h0e50; lna current
	TLG_WriteReg(base_addr, 0x05a, 0x0100);//9'h05a,16'h0100; lna_th0
	TLG_WriteReg(base_addr, 0x05b, 0x0280);//9'h05b,16'h0280; lna_th1
	TLG_WriteReg(base_addr, 0x062, 0x053f);//9'h62,16'h053f; vco high low th 705M
	TLG_WriteReg(base_addr, 0x066, 0x0470);//9'h66,16'h0470; cfc th0
	TLG_WriteReg(base_addr, 0x067, 0x0538);//9'h67,16'h0538; cfc th1
	TLG_WriteReg(base_addr, 0x068, 0x0648);//9'h68,16'h0648; cfc th2
	TLG_WriteReg(base_addr, 0x06a, 0xcccc);//9'h6a,16'hcccc; rfpll_gain0~3
	TLG_WriteReg(base_addr, 0x072, 0xaaaa);//9'h072,16'hAAAA; vco ibit 1010
	TLG_WriteReg(base_addr, 0x084, 0x000f);//9'h084,16'h000F; Analog Filter
	TLG_WriteReg(base_addr, 0x08e, 0x3400);//9'h8e,16'h3400; int_dec_sel 00
	TLG_WriteReg(base_addr, 0x086, 0x1045);//9'h86,16'h1045; mdll_test 1
	TLG_WriteReg(base_addr, 0x087, 0x0c12);//9'h087,16'h0c12;
	TLG_WriteReg(base_addr, 0x080, 0x004d);//9'h80,16'h004d; adc_sel_clk 1
	TLG_WriteReg(base_addr, 0x088, 0x4824);//9'h088,16'h4324; xtal cap
	TLG_WriteReg(base_addr, 0x098, 0x0400);//9'h098,16'h0400; os_en, dc_i
	TLG_WriteReg(base_addr, 0x099, 0x0000);//9'h099,16'h0000; dc_q
	TLG_WriteReg(base_addr, 0x0b0, 0x0140);//9'h0b0,16'h0140; pllbb

	TLG_WriteReg(base_addr, 0x042, 0x0924);//9'h42,16'h0FF4; analog gain
	TLG_WriteReg(base_addr, 0x043, 0x0FFC);//9'0x43,16'0x0FFF;
	TLG_WriteReg(base_addr, 0x044, 0x0000);//9'0x44,16'0x1000;
	TLG_WriteReg(base_addr, 0x045, 0xF700);//9'0x45,16'0xFF73;

	TLG_WriteReg(base_addr, 0x046, 0x0000);//9'0x46,16'0x0000;
	TLG_WriteReg(base_addr, 0x047, 0x0000);//9'0x47,16'0x0000;
	TLG_WriteReg(base_addr, 0x048, 0x0000);//9'0x48,16'0x0000;
	TLG_WriteReg(base_addr, 0x049, 0x0000);//9'0x49,16'0x0000;
	TLG_WriteReg(base_addr, 0x04A, 0x0000);//9'0x4A,16'0x0000;
	TLG_WriteReg(base_addr, 0x04B, 0x0000);//9'0x4B,16'0x0000;
	TLG_WriteReg(base_addr, 0x04C, 0x28FF);//9'H04C,16'H28F3; // inc adac gain 0x28F6 0x28FF
	TLG_WriteReg(base_addr, 0x04D, 0x00F0);//9'0x4D,16'0x00F0;
	TLG_WriteReg(base_addr, 0x04E, 0x0000);//9'0x4E,16'0x0000;
	TLG_WriteReg(base_addr, 0x04F, 0xC000);//9'0x4F,16'0xC000;
	TLG_WriteReg(base_addr, 0x050, 0x0000);//9'0x50,16'0x0000;
	TLG_WriteReg(base_addr, 0x051, 0x0000);//9'0x51,16'0x0000;
	TLG_WriteReg(base_addr, 0x052, 0x0000);//9'0x52,16'0x0000;

	TLG_WriteReg(base_addr, 0x0a1, 0x008b);//9'h0a1,16'h008B; bit_8: dsp_pd_mode 0x0087
#endif
	
	kal_prompt_trace(0,"[rdamtv]-------------init end--------------\n");
	
    return(TLG_ERR_SUCCESS);
}

short RDA_GetRSSI(void)
{
    short rssi = 0;
	uint16 reg;
	
#ifdef RDA5888_VIDENHANCE_ENABLE	
	static uint8 sig4cnt = 0;
#endif

	TLG_ReadReg(0x62, 0x0185, &reg);
	rssi = ((short)(reg&0x03ff))/8 - 110;

	if (rssi <= -83)
	{
		g_iSignalLevel = 0;
	}
	else if ((rssi >= -82)&&(rssi <= -78))
	{
		g_iSignalLevel = 1;
	}
	else if ((rssi >= -77)&&(rssi <= -73))
	{
		g_iSignalLevel = 2;
	}
	else if ((rssi >= -72)&&(rssi <= -63))
	{
		g_iSignalLevel = 3;
	}
	else if (rssi >= -62)
	{
		g_iSignalLevel = 4;
	}

#ifdef RDA5888_VIDENHANCE_ENABLE
    if (1 == g_nVideoEnhanceEnable)
    {
		if (g_iSignalLevel < 4)
		{
		    g_nVideoEnSet = 1;
			sig4cnt = 0;
			if (g_nFrameBufSize < 102400)
			{
			    g_nDiscardFrameNum = 2;
			}
			else
			{
			    g_nDiscardFrameNum = 3;
			}
		}
		else // g_iSignalLevel >= 4
		{
		    sig4cnt++;
			if (sig4cnt > 6)
			{
				g_nVideoEnSet = 0;
				g_nDiscardFrameNum = 2;
				g_nFrameBufSize = 0;
			}
		}
    }
	else
	{
	    g_nVideoEnSet = 0;
		g_nDiscardFrameNum = 2;
	}
#endif

#ifdef RDA5888_VIDENHANCE_ENABLE
	kal_prompt_trace(0,"[rdamtv]rssi = %d, sig_level = %d, video_en = %d, dis_fra = %d, fra_size = %d\n", 
	               rssi, g_iSignalLevel, g_nVideoEnSet, g_nDiscardFrameNum, g_nFrameBufSize);
#else
	kal_prompt_trace(0,"[rdamtv]rssi = %d, sig_level = %d\n", rssi, g_iSignalLevel);
#endif

	return rssi;
}

uint8 RDA_GetSignalLevel(void)
{
    return g_iSignalLevel;
}

void RDA_ATVCheckStatus(void)
{
	uint16 var, reg = 0;
	short rssi = 0;
	static uint8 check_time = 0;
	static uint8 sync_fail_time = 0;
	static uint8 ait_stop_flag = 0;
	
    check_time++;
    if (check_time > 5)
    {
        check_time = 1;
		rssi = RDA_GetRSSI();
		
		TLG_ReadReg(0x62, 0x1a6, &reg);
		TLGHAL_GET(var, reg, 0, 0x0001);
		if (var != 1)
		{
		    TLG_WriteReg(0x62, 0x11e, 0x61a2);
		    TLG_WriteReg(0x62, 0x11e, 0x6182);
		}

#if (RDA_CHIP_VER == 2)
        // hsync state process.
		if (rssi > -80)
		{
		    check_hsync_state();
		}
#endif
    }

#ifdef RDA5888_SOFTBLENDING_ENABLE	
    if (g_nSoftBlendingSet)
    {
	    soft_blending(); // soft blending process.
    }
#endif

#ifdef RDA5888_SYNC_CHECK_ENABLE
    TLG_WriteReg(0x62, 0x12f, 0x0050);
    TLG_ReadReg(0x62, 0x117, &reg);
    TLGHAL_GET(var, reg, 0x07, 0x0780);
	
	//kal_prompt_trace(0,"[rdamtv]var = %d, g_nIsTPMode = %d\n", var, g_nIsTPMode);
	if ((var == 8)||(var == 9)) // sync locked
	{
	    // AIT_TV start data rx process
	   // AIT701_cam_VIF_output_ctl(1); //need to patch
		if (ait_stop_flag)
		{
			ait_stop_flag = 0;
			///////////////
			
		}
		
		sync_fail_time = 0;
	}
	else
	{
		// AIT_TV stop data rx process
		//AIT701_cam_VIF_output_ctl(0);//need to patch
		if (!ait_stop_flag)
		{
			ait_stop_flag = 1;
			////////////////
			
		}
		
	    sync_fail_time++;
	}
	
	if (sync_fail_time > CHECK_SYNC_FAIL_TIMES)
	{
	    if (!g_nIsTPMode)
	    {
		    TLG_ReadReg(0x62, 0x130, &g_nTPRegBak);
			TLG_WriteReg(0x62, 0x130, 0x803);
#ifdef RDA5888_OUTPUT_SP1_ENABLE			
			TLG_WriteReg(0x62, 0x12f, 0x0231); // 0231 -> 0x007e
#endif	
			g_nIsTPMode = 1;
	    }
	}
	else if (g_nIsTPMode)
	{
	    TLG_WriteReg(0x62, 0x130, g_nTPRegBak);
#ifdef RDA5888_OUTPUT_SP1_ENABLE
		TLG_WriteReg(0x62, 0x12f, 0x007e); // 0231 -> 0x007e
#endif
		g_nIsTPMode = 0;
	}
#else
	//get bit_done : reg( 0x100[1])
	TLG_ReadReg(0x62,0x0100,&reg);
	TLGHAL_GET(var,reg,0x02,0x04);

	if (var == 0) // seek_done fail
	{
	    if (!g_nIsTPMode)
	    {
		    TLG_ReadReg(0x62, 0x130, &g_nTPRegBak);
			TLG_WriteReg(0x62, 0x130, 0x803);
#ifdef RDA5888_OUTPUT_SP1_ENABLE			
			TLG_WriteReg(0x62, 0x12f, 0x0231); // 0231 -> 0x007e
#endif
			g_nIsTPMode = 1;
	    }
	}
	else if (g_nIsTPMode)
	{
	    TLG_WriteReg(0x62, 0x130, g_nTPRegBak);
#ifdef RDA5888_OUTPUT_SP1_ENABLE
		TLG_WriteReg(0x62, 0x12f, 0x007e); // 0231 -> 0x007e
#endif
		g_nIsTPMode = 0;
	}
#endif

#ifdef RDA5888_HISR_CHECK_ENABLE
    if (g_nIspHisrCnt++ > 1)
    {
        visual_active_hisr(VISUAL_CAMERA_HISR_ID);
		kal_set_eg_events(lcd_event_id, LCD_CMD_COMPLETE_EVENT, KAL_OR);
		kal_prompt_trace(0,"[rdamtv]hisr\n");
		//RDA_SysReset();
    }
#endif

	GPTI_StopItem(g_nRdamtvTimer);
	GPTI_StartItem(g_nRdamtvTimer,kal_milli_secs_to_ticks(100),RDA_ATVCheckStatus,0);		
}

void RDA_RxOff(uint32 base_addr)
{
	uint16		reg;

	TLG_ReadReg(base_addr,0x09e,&reg);
	TLGHAL_SET(reg,0x0,REG_BIT0_SHIFT,REG_BIT0_MASK); //reg:0x09e[0]->0
	TLG_WriteReg(base_addr, 0x09e, reg);

    TLG_Delay((uint16)10);
}

void RDA_RxOn(uint32 base_addr)
{
    TLG_WriteReg(base_addr, 0x09e, 0x0000);
	TLG_WriteReg(base_addr, 0x09e, 0x0001);
}

#if 0
uint8 nChangeColorCount = 0;
void RDA_UpdateColorOrder(void)
{
	uint16 nWriteValue = 0x088B;

    kal_prompt_trace(0,"[rdamtv]RDA_UpdateColorOrder\n");
    kal_prompt_trace(0,"[rdamtv]nChangeColorCount=(%d)\n", nChangeColorCount);

	switch(nChangeColorCount)
	{
		default:
		case 0:
			nWriteValue = 0x084B;
			break;
		case 1:
			nWriteValue = 0x08CB;
			break;
#if 0
		case 2:
			nWriteValue = 0x084B;
			break;
		case 3:
			nWriteValue = 0x08CB;
			break;
		case 4:
			nWriteValue = 0x088B;
			break;
		case 5:
			nWriteValue = 0x08AB;
			break;
		case 6:
			nWriteValue = 0x08EB;
			break;
#endif
	}
    kal_prompt_trace(0,"[rdamtv]Write(%x), Addr=(0x129)\n", nWriteValue);
	TLG_WriteReg(0x62, 0x129,nWriteValue);

	nChangeColorCount = (nChangeColorCount+1)%2;
}
#endif


void RDA_DigitalInit(uint32 base_addr)
{
    kal_prompt_trace(0,"[rdamtv]RDA_DigitalInit, Ver = %d\n", RDA_CHIP_VER);
	
#if (RDA_CHIP_VER == 2)
x
	TLG_WriteReg(base_addr, 0x100,0x0000); //reset dig registers
	TLG_WriteReg(base_addr, 0x100,0x0001);
	TLG_WriteReg(base_addr, 0x130,0x0010);//9'h130,16'h0000; reset dsp
	
	TLG_WriteReg(base_addr, 0x107,0x00ff); //sk_rssi_rate, sk_tmr_afc
	TLG_WriteReg(base_addr, 0x108,0x00a0); //sk_rssi_th
	TLG_WriteReg(base_addr, 0x10A,0x08af); //9'h10A,16'h09ef; 0x09ef 0x0907  
	TLG_WriteReg(base_addr, 0x10B,0x4668); //cbcr_swap 0x4668
	TLG_WriteReg(base_addr, 0x10c,0xf047); //9'h10c,16'hf04f;
	TLG_WriteReg(base_addr, 0x111,0x5413); //9'h111,16'h5413; V2.3
	TLG_WriteReg(base_addr, 0x112,0x1000);
	TLG_WriteReg(base_addr, 0x113,0x8058); //9'h113,16'h809b;  0x409b
	TLG_WriteReg(base_addr, 0x115,0x048a); // 9'h115,16'h048a;
	TLG_WriteReg(base_addr, 0x118,0x5fdf); //9'h118,16'h5fdf; auto black 0x5fdf 0xbfbf
	TLG_WriteReg(base_addr, 0x11d,0x60ff); //ha_index_var_thd & ha_value_var_thd
    TLG_WriteReg(base_addr, 0x120,0x4832); //9'h120,16'h4832;
	TLG_WriteReg(base_addr, 0x125,0x0870); //9'h125,16'h1070; gainct_fm 0x1070 0x0470

	TLG_WriteReg(base_addr, 0x129,0x08CB); //hsync_delay_sel & clk_inv

	TLG_WriteReg(base_addr, 0x133,0x0186);// 9'h133,16'h0186;
	TLG_WriteReg(base_addr, 0x135,0x03cc); //h_blank_end, cell without shell

	TLG_WriteReg(base_addr, 0x140,0x402D); //line625_flag
	TLG_WriteReg(base_addr, 0x141,0x0001);
	TLG_WriteReg(base_addr, 0x142,0x078f); //notch coefficients
	TLG_WriteReg(base_addr, 0x143,0x1f4b);// notch coefficients
	TLG_WriteReg(base_addr, 0x144,0x071e); //notch coefficients
	TLG_WriteReg(base_addr, 0x145,0x0000);
	TLG_WriteReg(base_addr, 0x146,0x0000);
	TLG_WriteReg(base_addr, 0x147,0x0000);
	TLG_WriteReg(base_addr, 0x148,0x0000);
	TLG_WriteReg(base_addr, 0x149,0x0000);
	TLG_WriteReg(base_addr, 0x14a,0x0000);
	TLG_WriteReg(base_addr, 0x14b,0x0000);
	TLG_WriteReg(base_addr, 0x14c,0x0000);
	TLG_WriteReg(base_addr, 0x14d,0x0000);
	TLG_WriteReg(base_addr, 0x14e,0x0000);
	TLG_WriteReg(base_addr, 0x14f,0x0000);
	TLG_WriteReg(base_addr, 0x150,0x0000);
	TLG_WriteReg(base_addr, 0x151,0x0000);
	TLG_WriteReg(base_addr, 0x152,0x0000);
	TLG_WriteReg(base_addr, 0x153,0x0000);
	TLG_WriteReg(base_addr, 0x154,0x0000);
	TLG_WriteReg(base_addr, 0x155,0x0000);
	TLG_WriteReg(base_addr, 0x156,0x0000);
	TLG_WriteReg(base_addr, 0x157,0x0000);
	TLG_WriteReg(base_addr, 0x158,0x0000);
	TLG_WriteReg(base_addr, 0x159,0x0000);
	TLG_WriteReg(base_addr, 0x15a,0x0000);
	TLG_WriteReg(base_addr, 0x15b,0x0000);
	TLG_WriteReg(base_addr, 0x15c,0x0000);
	TLG_WriteReg(base_addr, 0x15d,0x0000);
	TLG_WriteReg(base_addr, 0x15e,0x0000);
	TLG_WriteReg(base_addr, 0x15f,0x0000);
	TLG_WriteReg(base_addr, 0x160,0x0000);
	TLG_WriteReg(base_addr, 0x161,0x0000);
	TLG_WriteReg(base_addr, 0x162,0x0000);
	TLG_WriteReg(base_addr, 0x163,0x0000);
	TLG_WriteReg(base_addr, 0x164,0x0000);
	TLG_WriteReg(base_addr, 0x165,0x0000);
	TLG_WriteReg(base_addr, 0x166,0x0000);
	TLG_WriteReg(base_addr, 0x167,0x0000);
	TLG_WriteReg(base_addr, 0x168,0x0000);
	TLG_WriteReg(base_addr, 0x169,0x0000);
	TLG_WriteReg(base_addr, 0x16a,0x0000);
	TLG_WriteReg(base_addr, 0x16b,0x0000);
	TLG_WriteReg(base_addr, 0x16c,0x0000);
	TLG_WriteReg(base_addr, 0x16d,0x0000);
	TLG_WriteReg(base_addr, 0x16e,0x0000);
	TLG_WriteReg(base_addr, 0x16f,0x0000);
	TLG_WriteReg(base_addr, 0x170,0x00bf);
	TLG_WriteReg(base_addr, 0x171,0x0006);
	TLG_WriteReg(base_addr, 0x172,0x4c1d);
	TLG_WriteReg(base_addr, 0x173,0x4c1d);
	TLG_WriteReg(base_addr, 0x174,0x008c);
	TLG_WriteReg(base_addr, 0x175,0x1020);
	TLG_WriteReg(base_addr, 0x176,0x0030);
	TLG_WriteReg(base_addr, 0x177,0x01cf); // 9'h177,16'h01cf;
	TLG_WriteReg(base_addr, 0x178,0x236d);
	TLG_WriteReg(base_addr, 0x179,0x0080);
	TLG_WriteReg(base_addr, 0x17a,0x1440);
	TLG_WriteReg(base_addr, 0x17b,0x0000);
	TLG_WriteReg(base_addr, 0x17c,0x0000);
	TLG_WriteReg(base_addr, 0x17d,0x0000);
	TLG_WriteReg(base_addr, 0x17e,0x0000);
	TLG_WriteReg(base_addr, 0x17f,0x0000);
	TLG_WriteReg(base_addr, 0x180,0x0004);
	TLG_WriteReg(base_addr, 0x181,0x2040);
	TLG_WriteReg(base_addr, 0x182,0x0138);
	TLG_WriteReg(base_addr, 0x183,0x9005); //afc_disable_fm 0x9037 0x9005
	TLG_WriteReg(base_addr, 0x184,0x0924); // 0x0924 0x0FFF
	TLG_WriteReg(base_addr, 0x185,0x0000);
	TLG_WriteReg(base_addr, 0x186,0x0000);
	TLG_WriteReg(base_addr, 0x187,0x0000);
	TLG_WriteReg(base_addr, 0x188,0x0000);
	TLG_WriteReg(base_addr, 0x189,0x0000);
	TLG_WriteReg(base_addr, 0x18a,0x0000);
	TLG_WriteReg(base_addr, 0x18b,0x0000);
	TLG_WriteReg(base_addr, 0x18c,0x0000);
	TLG_WriteReg(base_addr, 0x18d,0x0000);
	TLG_WriteReg(base_addr, 0x18e,0x0000);
	TLG_WriteReg(base_addr, 0x18f,0x051f);// tmr_agc1
	TLG_WriteReg(base_addr, 0x190,0x0008);

	TLG_WriteReg(base_addr, 0x191,0x19be);//16'b00_0110111_0111011); adv_th7=55, adv_th6=59
	TLG_WriteReg(base_addr, 0x192,0x275d);//16'b00_1000001_1000111); adv_th5=65, adv_th4=71
	TLG_WriteReg(base_addr, 0x193,0x3fff);//16'b00_1001100_1010000); adv_th3=76, adv_th2=80
	TLG_WriteReg(base_addr, 0x194,0x817f);//16'b100000010_1010110); adv_th1=86
	TLG_WriteReg(base_addr, 0x195,0x050a);//16'b00_0001101_0010011); ana_gain_index0=13,ana_gain_index1=19
	TLG_WriteReg(base_addr, 0x196,0x850a);//16'b10_0011001_0011110); ana_gain_index2=25, ana_gain_index3=30
	TLG_WriteReg(base_addr, 0x197,0x051c);//16'b00_0100011_0101000); ana_gain_index4=35, ana_gain_index5=40
	TLG_WriteReg(base_addr, 0x198,0x1637);//16'b00_0101111_0110100); ana_gain_index6=47, ana_gain_index7=52
	TLG_WriteReg(base_addr, 0x199,0x6000);//9'h199,16'b0_1010000_0_0000000; tagt_pwr=112 0x5000

	TLG_WriteReg(base_addr, 0x19a,0x0014);
	TLG_WriteReg(base_addr, 0x19B,0x0D09);// bypass rate_conv & enable notch & sk_mode
	TLG_WriteReg(base_addr, 0x19C,0x00ff);// tmr_rssi1
	TLG_WriteReg(base_addr, 0x19d,0x14FF);// tmr_rssi3
	TLG_WriteReg(base_addr, 0x19e,0x0000);
	TLG_WriteReg(base_addr, 0x19f,0x0000);
	
	TLG_WriteReg(base_addr, 0x1a0,0x1f39); // notch filter for audio
	TLG_WriteReg(base_addr, 0x1a1,0x071e);
	TLG_WriteReg(base_addr, 0x1a2,0x0070);
	
	TLG_WriteReg(base_addr, 0x1a3,0x0002);
	TLG_WriteReg(base_addr, 0x1a4,0x0000);
	TLG_WriteReg(base_addr, 0x1a5,0x0080);
	TLG_WriteReg(base_addr, 0x1a6,0x0000);
	TLG_WriteReg(base_addr, 0x1a7,0x0000);
	TLG_WriteReg(base_addr, 0x1a8,0x0000);
	TLG_WriteReg(base_addr, 0x1a9,0x0000);
	TLG_WriteReg(base_addr, 0x1aa,0x0000);
	TLG_WriteReg(base_addr, 0x1ab,0x0007); // adc_clk_edge, dcdc_div_ct
	TLG_WriteReg(base_addr, 0x1ac,0x0000);
	TLG_WriteReg(base_addr, 0x1ad,0x3000); //seek_en
	TLG_WriteReg(base_addr, 0x1ae,0x0000);
	TLG_WriteReg(base_addr, 0x1af,0x0000);

	TLG_WriteReg(base_addr, 0x114,0xefe0); //9'h114,16'hefe0;
	TLG_WriteReg(base_addr, 0x116,0x1201); //9'h116,16'h1201;
	TLG_WriteReg(base_addr, 0x12f,0x007e); //9'h12f,16'h0076;
	
	//TLG_Delay(5);

	TLG_WriteReg(base_addr, 0x130,0x0810);
#elif (RDA_CHIP_VER == 4)
	TLG_WriteReg(base_addr, 0x100,0x0000); //reset dig registers
	TLG_WriteReg(base_addr, 0x100,0x0001);
	TLG_WriteReg(base_addr, 0x130,0x0010);//9'h130,16'h0000; reset dsp
	
	TLG_WriteReg(base_addr, 0x104,0x31c7);//9'h104,16'h31c7; Pal D/k angle_in_if

	TLG_WriteReg(base_addr, 0x107,0x00ff); //sk_rssi_rate, sk_tmr_afc
	TLG_WriteReg(base_addr, 0x108,0x00a0); //sk_rssi_th

    TLG_WriteReg(base_addr, 0x10A,0x089f); //9'h10A,16'h089f;
    TLG_WriteReg(base_addr, 0x10B,0x4229); //9'h10b,16'h4229;
    TLG_WriteReg(base_addr, 0x10c,0xf047); //9'h10c,16'hf047;

    TLG_WriteReg(base_addr, 0x111,0x5413); //9'h111,16'h5413;
    TLG_WriteReg(base_addr, 0x112,0x3000); //9'h112,16'h3000; ECO2
    TLG_WriteReg(base_addr, 0x113,0xfd58); //9'h113,16'hfd58;
    TLG_WriteReg(base_addr, 0x114,0xefe0); //9'h114,16'hefe0;
    TLG_WriteReg(base_addr, 0x115,0x048a); //9'h115,16'h048a;
    TLG_WriteReg(base_addr, 0x116,0x1201); //9'h116,16'h1201;
    
    #ifdef RDA5888_AUTOBLACK_ENABLE
	    TLG_WriteReg(base_addr, 0x118,0xbfbf); //9'h118,16'hBFBF;
	#else
        TLG_WriteReg(base_addr, 0x118,0x3fbf); //9'h118,16'hBFBF; auto black(0xbfbf), close ab(0x3fbf)
    #endif

	TLG_WriteReg(base_addr, 0x11d,0x60ff); //ha_index_var_thd & ha_value_var_thd
	TLG_WriteReg(base_addr, 0x11f,0x082a); //9'h11f,16'h082a;
	TLG_WriteReg(base_addr, 0x120,0x4832); //9'h120,16'h4832;
	TLG_WriteReg(base_addr, 0x125,0x0870); //9'h125,16'h1070; gainct_fm 0x1070 0x0470

	//TLG_WriteReg(base_addr, 0x129,0x088B); //2010/02/01 RDA Wu_Leo test;
	//TLG_WriteReg(base_addr, 0x129,0x08CB); //hsync_delay_sel & clk_inv & 
	TLG_WriteReg(base_addr, 0x129,0x08DB); //A8_SENSOR_YCBYCR

	TLG_WriteReg(base_addr, 0x133,0x0186);// 9'h133,16'h0186;
	//TLG_WriteReg(base_addr, 0x133,0x0106); //2010/02/01 RDA Wu_Leo test;
	TLG_WriteReg(base_addr, 0x134,0x02C1); //2010/02/01 RDA Wu_Leo test;
	TLG_WriteReg(base_addr, 0x135,0x03cc); //h_blank_end, cell without shell

	TLG_WriteReg(base_addr, 0x140,0x402D); //9'h140,16'h402D; line625_flag
	TLG_WriteReg(base_addr, 0x141,0x0001);
	TLG_WriteReg(base_addr, 0x142,0x078f); //notch coefficients
	TLG_WriteReg(base_addr, 0x143,0x1f4b);// notch coefficients
	TLG_WriteReg(base_addr, 0x144,0x071e); //notch coefficients
	TLG_WriteReg(base_addr, 0x145,0x0000);
	TLG_WriteReg(base_addr, 0x146,0x0000);
	TLG_WriteReg(base_addr, 0x147,0x0000);
	TLG_WriteReg(base_addr, 0x148,0x0000);
	TLG_WriteReg(base_addr, 0x149,0x0000);
	TLG_WriteReg(base_addr, 0x14a,0x0000);
	TLG_WriteReg(base_addr, 0x14b,0x0000);
	TLG_WriteReg(base_addr, 0x14c,0x0000);
	TLG_WriteReg(base_addr, 0x14d,0x0000);
	TLG_WriteReg(base_addr, 0x14e,0x0000);
	TLG_WriteReg(base_addr, 0x14f,0x0000);
	TLG_WriteReg(base_addr, 0x150,0x0000);
	TLG_WriteReg(base_addr, 0x151,0x0000);
	TLG_WriteReg(base_addr, 0x152,0x0000);
	TLG_WriteReg(base_addr, 0x153,0x0000);
	TLG_WriteReg(base_addr, 0x154,0x0000);
	TLG_WriteReg(base_addr, 0x155,0x0000);
	TLG_WriteReg(base_addr, 0x156,0x0000);
	TLG_WriteReg(base_addr, 0x157,0x0000);
	TLG_WriteReg(base_addr, 0x158,0x0000);
	TLG_WriteReg(base_addr, 0x159,0x0000);
	TLG_WriteReg(base_addr, 0x15a,0x0000);
	TLG_WriteReg(base_addr, 0x15b,0x0000);
	TLG_WriteReg(base_addr, 0x15c,0x0000);
	TLG_WriteReg(base_addr, 0x15d,0x0000);
	TLG_WriteReg(base_addr, 0x15e,0x0000);
	TLG_WriteReg(base_addr, 0x15f,0x0000);
	TLG_WriteReg(base_addr, 0x160,0x0000);
	TLG_WriteReg(base_addr, 0x161,0x0000);
	TLG_WriteReg(base_addr, 0x162,0x0000);
	TLG_WriteReg(base_addr, 0x163,0x0000);
	TLG_WriteReg(base_addr, 0x164,0x0000);
	TLG_WriteReg(base_addr, 0x165,0x0000);
	TLG_WriteReg(base_addr, 0x166,0x0000);
	TLG_WriteReg(base_addr, 0x167,0x0000);
	TLG_WriteReg(base_addr, 0x168,0x0000);
	TLG_WriteReg(base_addr, 0x169,0x0000);
	TLG_WriteReg(base_addr, 0x16a,0x0000);
	TLG_WriteReg(base_addr, 0x16b,0x0000);
	TLG_WriteReg(base_addr, 0x16c,0x0000);
	TLG_WriteReg(base_addr, 0x16d,0x0000);
	TLG_WriteReg(base_addr, 0x16e,0x0000);
	TLG_WriteReg(base_addr, 0x16f,0x0000);
	TLG_WriteReg(base_addr, 0x170,0x00bf);
	TLG_WriteReg(base_addr, 0x171,0x0006);
	TLG_WriteReg(base_addr, 0x172,0x4c1d);
	TLG_WriteReg(base_addr, 0x173,0x4c1d);
	TLG_WriteReg(base_addr, 0x174,0x008c);
	TLG_WriteReg(base_addr, 0x175,0x1020);
	TLG_WriteReg(base_addr, 0x176,0x0030);
	TLG_WriteReg(base_addr, 0x177,0x01cf); // 9'h177,16'h01cf;
	TLG_WriteReg(base_addr, 0x178,0x236d);
	TLG_WriteReg(base_addr, 0x179,0x0080);
	TLG_WriteReg(base_addr, 0x17a,0x1440);
	TLG_WriteReg(base_addr, 0x17b,0x0000);
	TLG_WriteReg(base_addr, 0x17c,0x0000);
	TLG_WriteReg(base_addr, 0x17d,0x0000);
	TLG_WriteReg(base_addr, 0x17e,0x0000);
	TLG_WriteReg(base_addr, 0x17f,0x0000);
	TLG_WriteReg(base_addr, 0x180,0x0004);
	TLG_WriteReg(base_addr, 0x181,0x2040);
	TLG_WriteReg(base_addr, 0x182,0x0138);
	TLG_WriteReg(base_addr, 0x183,0x9005); //afc_disable_fm 0x9037 0x9005
	TLG_WriteReg(base_addr, 0x184,0x0924); // 0x0924 0x0FFF
	TLG_WriteReg(base_addr, 0x185,0x0000);
	TLG_WriteReg(base_addr, 0x186,0x0000);
	TLG_WriteReg(base_addr, 0x187,0x0000);
	TLG_WriteReg(base_addr, 0x188,0x0000);
	TLG_WriteReg(base_addr, 0x189,0x0000);
	TLG_WriteReg(base_addr, 0x18a,0x0000);
	TLG_WriteReg(base_addr, 0x18b,0x0000);
	TLG_WriteReg(base_addr, 0x18c,0x0000);
	TLG_WriteReg(base_addr, 0x18d,0x0000);
	TLG_WriteReg(base_addr, 0x18e,0x0000);
	TLG_WriteReg(base_addr, 0x18f,0x051f);// tmr_agc1
	TLG_WriteReg(base_addr, 0x190,0x0008);

	TLG_WriteReg(base_addr, 0x191,0x19be);//16'b00_0110111_0111011); adv_th7=55, adv_th6=59
	TLG_WriteReg(base_addr, 0x192,0x275d);//16'b00_1000001_1000111); adv_th5=65, adv_th4=71
	TLG_WriteReg(base_addr, 0x193,0x3fff);//16'b00_1001100_1010000); adv_th3=76, adv_th2=80
	TLG_WriteReg(base_addr, 0x194,0x817f);//16'b100000010_1010110); adv_th1=86
	TLG_WriteReg(base_addr, 0x195,0x050a);//16'b00_0001101_0010011); ana_gain_index0=13,ana_gain_index1=19
	TLG_WriteReg(base_addr, 0x196,0x850a);//16'b10_0011001_0011110); ana_gain_index2=25, ana_gain_index3=30
	TLG_WriteReg(base_addr, 0x197,0x051c);//16'b00_0100011_0101000); ana_gain_index4=35, ana_gain_index5=40
	TLG_WriteReg(base_addr, 0x198,0x1637);//16'b00_0101111_0110100); ana_gain_index6=47, ana_gain_index7=52
	TLG_WriteReg(base_addr, 0x199,0x6000);//9'h199,16'b0_1010000_0_0000000; tagt_pwr=112 0x5000

	TLG_WriteReg(base_addr, 0x19a,0x0014); // 0x0015
	TLG_WriteReg(base_addr, 0x19B,0x0D09);// bypass rate_conv & enable notch & sk_mode
	TLG_WriteReg(base_addr, 0x19C,0x00ff);// tmr_rssi1
	TLG_WriteReg(base_addr, 0x19d,0x14FF);// tmr_rssi3
	TLG_WriteReg(base_addr, 0x19e,0x0000);
	TLG_WriteReg(base_addr, 0x19f,0x0000);

	TLG_WriteReg(base_addr, 0x1a0,0x1f39); // notch filter for audio
	TLG_WriteReg(base_addr, 0x1a1,0x071e);
	TLG_WriteReg(base_addr, 0x1a2,0x0070);

	TLG_WriteReg(base_addr, 0x1a3,0x0002);
	TLG_WriteReg(base_addr, 0x1a4,0x0000);
	TLG_WriteReg(base_addr, 0x1a5,0x0080);
	TLG_WriteReg(base_addr, 0x1a6,0x0000);
	TLG_WriteReg(base_addr, 0x1a7,0x0000);
	TLG_WriteReg(base_addr, 0x1a8,0x0801); //9'h1a8,16'h0801;ECO2
	TLG_WriteReg(base_addr, 0x1a9,0x0000);
	TLG_WriteReg(base_addr, 0x1aa,0x0000);
	TLG_WriteReg(base_addr, 0x1ab,0x0007); // adc_clk_edge, dcdc_div_ct
	TLG_WriteReg(base_addr, 0x1ac,0x0000);
	TLG_WriteReg(base_addr, 0x1ad,0x3000); //seek_en
	TLG_WriteReg(base_addr, 0x1ae,0x7000); //9'h1ae,16'h7000;ECO2
	TLG_WriteReg(base_addr, 0x1af,0x0000);

	TLG_WriteReg(base_addr, 0x12f,0x007e); //9'h12f,16'h0076;

	//TLG_Delay(5);

	TLG_WriteReg(base_addr, 0x130,0x0814); //9'h130,16'h0814;
#endif

	kal_prompt_trace(0,"[rdamtv]-------------init end--------------\n");
}

void RDA_Enable(uint32 base_addr)
{
    TLG_WriteReg(base_addr, 0x01, 0x0911);
	TLG_WriteReg(base_addr, 0x01, 0x0919);
}

void RDA_SysReset(void)
{
	uint16 reg;
	
	TLG_ReadReg(0x62, 0x130, &reg);
	TLGHAL_SET(reg, 0x0, REG_BIT11_SHIFT, REG_BIT11_MASK);
	TLG_WriteReg(0x62, 0x130, reg);
	
	TLG_Delay((uint16)5);
	
	TLG_ReadReg(0x62, 0x130, &reg);
	TLGHAL_SET(reg, 0x1, REG_BIT11_SHIFT, REG_BIT11_MASK);
	TLG_WriteReg(0x62, 0x130, reg);
}

void RDA_SetNotchFilter(uint16 mode, uint32 center_freq_khz)
{
	uint16 nFreqOffset = 0;
	
	switch (mode)
	{
		case TLG_VSTD_NTSC_M:
		case TLG_VSTD_PAL_M:
		case TLG_VSTD_PAL_N:
		case TLG_VSTD_PAL_NC:			
			nFreqOffset = m_nFreqOffset + 1725;
			break;
			
		case TLG_VSTD_PAL_B:
		case TLG_VSTD_PAL_B1: //
		case TLG_VSTD_PAL_G:
		case TLG_VSTD_PAL_I:
		case TLG_VSTD_SECAM_B:
		case TLG_VSTD_SECAM_B1: //
		case TLG_VSTD_SECAM_G:
		case TLG_VSTD_PAL_SECAM_B:
		case TLG_VSTD_PAL_SECAM_B1:
		case TLG_VSTD_PAL_SECAM_G:
		case TLG_VSTD_NTSC_B_G:
		case TLG_VSTD_NTSC_I:
			nFreqOffset = m_nFreqOffset + 2125;
			break;
			
		case TLG_VSTD_PAL_D:	
		case TLG_VSTD_PAL_K:
		case TLG_VSTD_PAL_D1: //
		case TLG_VSTD_PAL_H: //
   		case TLG_VSTD_SECAM_D:
		case TLG_VSTD_SECAM_D1: //
		case TLG_VSTD_SECAM_K:
		case TLG_VSTD_PAL_SECAM_D:
		case TLG_VSTD_PAL_SECAM_D1:	
		case TLG_VSTD_PAL_SECAM_K:
		case TLG_VSTD_NTSC_D_K:
			nFreqOffset = m_nFreqOffset + 2625;
			break;
			
    	case TLG_VSTD_SECAM_L:
			break;
			
		default:
			break;
	}

    center_freq_khz -= nFreqOffset; // calc. vision carrier freq.
    
	kal_prompt_trace(0, "v_freq = %d KHZ\n", center_freq_khz);
	
	switch (center_freq_khz)
	{
	    case 49750: // 49.75 MHZ
	        TLG_WriteReg(0x62, 0x1A0, 0x1748);
			TLG_WriteReg(0x62, 0x1A1, 0x7DE);
			TLG_WriteReg(0x62, 0x1A2, 0x10);
	        break;

#if 1
	    case 55250: // 55.25 MHZ
	        TLG_WriteReg(0x62, 0x1A0, 0x10CC);
			TLG_WriteReg(0x62, 0x1A1, 0x7DE);
			TLG_WriteReg(0x62, 0x1A2, 0x10);
	        break;
#endif

	    case 57750: // 57.75 MHZ
	        TLG_WriteReg(0x62, 0x1A0, 0x15CD);
			TLG_WriteReg(0x62, 0x1A1, 0x7DE);
			TLG_WriteReg(0x62, 0x1A2, 0x10);
	        break;

	    case 61250: // 61.25 MHZ
	        TLG_WriteReg(0x62, 0x1A0, 0x1E29);
			TLG_WriteReg(0x62, 0x1A1, 0x7DE);
			TLG_WriteReg(0x62, 0x1A2, 0x10);
	        break;

	    case 62250: // 62.25 MHZ
	        TLG_WriteReg(0x62, 0x1A0, 0x1A90);
			TLG_WriteReg(0x62, 0x1A1, 0x7E8);
			TLG_WriteReg(0x62, 0x1A2, 0xB);
	        break;

	    case 65750: // 65.75 MHZ
	        TLG_WriteReg(0x62, 0x1A0, 0x116D);
			TLG_WriteReg(0x62, 0x1A1, 0x7DE);
			TLG_WriteReg(0x62, 0x1A2, 0x10);
	        break;

	    case 77250: // 77.25 MHZ
	        TLG_WriteReg(0x62, 0x1A0, 0x15CD);
			TLG_WriteReg(0x62, 0x1A1, 0x7DE);
			TLG_WriteReg(0x62, 0x1A2, 0x10);
	        break;

	    case 83250: // 83.25 MHZ
	        TLG_WriteReg(0x62, 0x1A0, 0x124E);
			TLG_WriteReg(0x62, 0x1A1, 0x7D0);
			TLG_WriteReg(0x62, 0x1A2, 0x17);
	        break;	
			
	    case 85250: // 85.25 MHZ
	        TLG_WriteReg(0x62, 0x1A0, 0x1748);
			TLG_WriteReg(0x62, 0x1A1, 0x7DE);
			TLG_WriteReg(0x62, 0x1A2, 0x10);
	        break;

	    case 184250: // 184.25 MHZ
	        TLG_WriteReg(0x62, 0x1A0, 0x18E1);
			TLG_WriteReg(0x62, 0x1A1, 0x7DE);
			TLG_WriteReg(0x62, 0x1A2, 0x10);
	        break;

	    case 187250: // 187.25 MHZ
	        TLG_WriteReg(0x62, 0x1A0, 0x116D);
			TLG_WriteReg(0x62, 0x1A1, 0x7DE);
			TLG_WriteReg(0x62, 0x1A2, 0x10);
	        break;

	    case 192250: // 192.25 MHZ
	        TLG_WriteReg(0x62, 0x1A0, 0x1475);
			TLG_WriteReg(0x62, 0x1A1, 0x7DE);
			TLG_WriteReg(0x62, 0x1A2, 0x10);
	        break;

	    case 210250: // 210.25 MHZ
	        TLG_WriteReg(0x62, 0x1A0, 0x1C54);
			TLG_WriteReg(0x62, 0x1A1, 0x7ED);
			TLG_WriteReg(0x62, 0x1A2, 0x9);
	        break;

	    case 211250: // 211.25 MHZ
	        TLG_WriteReg(0x62, 0x1A0, 0x18E1);
			TLG_WriteReg(0x62, 0x1A1, 0x7DE);
			TLG_WriteReg(0x62, 0x1A2, 0x10);
	        break;

	    case 217250: // 217.25 MHZ
	        TLG_WriteReg(0x62, 0x1A0, 0x10C3);
			TLG_WriteReg(0x62, 0x1A1, 0x7E8);
			TLG_WriteReg(0x62, 0x1A2, 0xB);
	        break;
			
		default:
			//TLG_WriteReg(0x62, 0x1a0, 0x1f4c);
			//TLG_WriteReg(0x62, 0x1a1, 0x071f);
			//TLG_WriteReg(0x62, 0x1a2, 0x0071);
			return; // return before sys reset.
			//break;
	}
	RDA_SysReset();
}

int RDA_SetChnStandard(uint32 base_addr, uint16 mode)
{
    kal_prompt_trace(0, "TLG_SetChnStandard, mode = %d\n", mode);

    //TLG_BOUNDS_CHECK(mode, TLG_VSTD_NONE);
	switch(mode)
	{
		case TLG_VSTD_PAL_B:
		case TLG_VSTD_PAL_B1:
		case TLG_VSTD_PAL_G:
			TLG_WriteReg( base_addr, 0x140, 0x4009);//9'h140,16'h4009; angle_mode_if, angle_mode_fm 001
			TLG_WriteReg( base_addr, 0x143, 0x1bd5);//9'h143,16'h1bd5; notch coefficients
			TLG_WriteReg( base_addr, 0x104, 0x284c);//9'h104,16'h284c; angle_in_if
            TLG_WriteReg( base_addr, 0x111, 0x5413);//9'h111,16'h5413; V2.3
			TLG_WriteReg( base_addr, 0x1a0, 0x1bc3);//9'h1a0,16'h1bc3;Notch filter for audio
			TLG_WriteReg( base_addr, 0x1a1, 0x071e);//9'h1a1,16'h071e;
			TLG_WriteReg( base_addr, 0x1a2, 0x0070);//9'h1a2,16'h0070;			

			dsp_system_reset(0);
			break;
			
		case TLG_VSTD_PAL_D:
		case TLG_VSTD_PAL_D1:
		case TLG_VSTD_PAL_K:		
			TLG_WriteReg( base_addr, 0x140, 0x402d);//9'h140,16'h402d; angle_mode_if, angle_mode_fm 101
			TLG_WriteReg( base_addr, 0x143, 0x1f4b);//9'h143,16'h1f4b; notch coefficients

			TLG_WriteReg( base_addr, 0x104, 0x31c7);// 9'h104,16'h31c7; angle_in_if
			TLG_WriteReg( base_addr, 0x111, 0x5413);//9'h111,16'h5413; V2.3
			
			TLG_WriteReg( base_addr, 0x1a0, 0x1f39);//9'h1a0,16'h1f39;Notch filter for audio
			TLG_WriteReg( base_addr, 0x1a1, 0x071e);//9'h1a1,16'h071e;
			TLG_WriteReg( base_addr, 0x1a2, 0x0070);//9'h1a2,16'h0070;			

			dsp_system_reset(0);
			break;

		case TLG_VSTD_PAL_I:
			TLG_WriteReg( base_addr, 0x140, 0x4024);//9'h140,16'h4024; angle_mode_if, angle_mode_fm 100
			TLG_WriteReg( base_addr, 0x143, 0x1d8c);//9'h143,16'h1d8c; notch coefficients		
			TLG_WriteReg( base_addr, 0x104, 0x284c);//9'h104,16'h284c; angle_in_if
			TLG_WriteReg( base_addr, 0x111, 0x5413);//9'h111,16'h5413; V2.3
			
			TLG_WriteReg( base_addr, 0x1a0, 0x1d7a);//9'h1a0,16'h1d7a;Notch filter for audio
			TLG_WriteReg( base_addr, 0x1a1, 0x071e);//9'h1a1,16'h071e;
			TLG_WriteReg( base_addr, 0x1a2, 0x0070);//9'h1a2,16'h0070;			

			dsp_system_reset(0);
			break;

		case TLG_VSTD_PAL_M:
			TLG_WriteReg( base_addr, 0x140, 0x0040);//9'h140,16'h0040; angle_mode_if, angle_mode_fm 000, angle_mode_sub 001, line625_flag 0
			TLG_WriteReg( base_addr, 0x143, 0x1897);//9'h143,16'h1897; notch coefficients
			TLG_WriteReg( base_addr, 0x104, 0x20b6);//9'h104,16'h20b6; angle_in_if
			TLG_WriteReg( base_addr, 0x111, 0x43ce);//9'h111,16'h5413; V2.3
			
			TLG_WriteReg( base_addr, 0x1a0, 0x1872);//9'h1a0,16'h1872;Notch filter for audio
			TLG_WriteReg( base_addr, 0x1a1, 0x0749);//9'h1a1,16'h0749;
			TLG_WriteReg( base_addr, 0x1a2, 0x005b);//9'h1a2,16'h005b;			
			
			dsp_system_reset(0);
			break;

		case TLG_VSTD_PAL_H:		
			break;

		case TLG_VSTD_PAL_N:
		case TLG_VSTD_PAL_NC:
			TLG_WriteReg( base_addr, 0x140, 0x4100);//9'h140,16'h4100; angle_mode_if, angle_mode_fm 000, angle_mode_sub 100, line625_flag 1
			TLG_WriteReg( base_addr, 0x143, 0x1897);//9'h143,16'h1897; notch coefficients
		
			TLG_WriteReg( base_addr, 0x104, 0x20b6);//9'h104,16'h20b6; angle_in_if
			TLG_WriteReg( base_addr, 0x111, 0x43ed);//9'h111,16'h5413; V2.3
			
			TLG_WriteReg( base_addr, 0x1a0, 0x1872);//9'h1a0,16'h1872;Notch filter for audio
			TLG_WriteReg( base_addr, 0x1a1, 0x0749);//9'h1a1,16'h0749;
			TLG_WriteReg( base_addr, 0x1a2, 0x005b);//9'h1a2,16'h005b;			
			
			dsp_system_reset(0);		
			break;
			
		case TLG_VSTD_SECAM_B:
		case TLG_VSTD_SECAM_B1:
		case TLG_VSTD_PAL_SECAM_B:
		case TLG_VSTD_PAL_SECAM_B1:
		case TLG_VSTD_SECAM_G:
		case TLG_VSTD_PAL_SECAM_G:			
			TLG_WriteReg( base_addr, 0x140,0x40c9);//angle_mode_if, angle_mode_fm 
			TLG_WriteReg( base_addr, 0x143,0x1bd5);//notch coefficients

            TLG_WriteReg( base_addr, 0x104,0x284c);//9'h104,16'h284c; angle_in_if
			TLG_WriteReg( base_addr, 0x111, 0x5142);//9'h111,16'h5413; V2.3

			TLG_WriteReg( base_addr, 0x116,0x008c);//u_cb_dis, k1a
			TLG_WriteReg( base_addr, 0x117,0x0fd2);//k1b
			TLG_WriteReg( base_addr, 0x118,0x3fbf);//auto black			
			TLG_WriteReg( base_addr, 0x1a0, 0x1bc3);//9'h1a0,16'h1bc3;Notch filter for audio
			TLG_WriteReg( base_addr, 0x1a1, 0x071e);//9'h1a1,16'h071e;
			TLG_WriteReg( base_addr, 0x1a2, 0x0070);//9'h1a2,16'h0070;			
			
			dsp_system_reset(2);//system_mode 10, reset dsp
			break;
			
   		case TLG_VSTD_SECAM_D:
		case TLG_VSTD_SECAM_D1:
		case TLG_VSTD_SECAM_K:
		case TLG_VSTD_PAL_SECAM_D:
		case TLG_VSTD_PAL_SECAM_D1:
		case TLG_VSTD_PAL_SECAM_K:
			TLG_WriteReg( base_addr, 0x140,0x40ed);//angle_mode_if, angle_mode_fm 
			TLG_WriteReg( base_addr, 0x143,0x1f4b);//notch coefficients

			TLG_WriteReg( base_addr, 0x104,0x31c7);//9'h104,16'h31c7; angle_in_if
			TLG_WriteReg( base_addr, 0x111, 0x5142);//9'h111,16'h5413; V2.3

			TLG_WriteReg( base_addr, 0x116,0x008c);//u_cb_dis, k1a
			TLG_WriteReg( base_addr, 0x117,0x0fd2);//k1b
			TLG_WriteReg( base_addr, 0x118,0x3fbf);//auto black
			
			TLG_WriteReg( base_addr, 0x1a0, 0x1f39);//9'h1a0,16'h1f39;Notch filter for audio
			TLG_WriteReg( base_addr, 0x1a1, 0x071e);//9'h1a1,16'h071e;
			TLG_WriteReg( base_addr, 0x1a2, 0x0070);//9'h1a2,16'h0070;			
			
			dsp_system_reset(2);//system_mode 10, reset dsp
			break;
			
    	case TLG_VSTD_SECAM_L:
			break;
			
        case TLG_VSTD_NTSC_B_G:
			TLG_WriteReg( base_addr, 0x140,0x0009);//angle_mode_if, angle_mode_fm 
			TLG_WriteReg( base_addr, 0x143,0x1bd5);//notch coefficients

			TLG_WriteReg( base_addr, 0x104, 0x284c);//9'h111,16'h5413; V2.3
			TLG_WriteReg( base_addr, 0x111, 0x5413);//9'h111,16'h5413; V2.3

			TLG_WriteReg( base_addr, 0x137,0x034c);//h_sync_win_end
			TLG_WriteReg( base_addr, 0x139,0x03ac);//h_blank_win_end
			
			TLG_WriteReg( base_addr, 0x1a0, 0x1bc3);//9'h1a0,16'h1bc3;Notch filter for audio
			TLG_WriteReg( base_addr, 0x1a1, 0x071e);//9'h1a1,16'h071e;
			TLG_WriteReg( base_addr, 0x1a2, 0x0070);//9'h1a2,16'h0070;			

			dsp_system_reset(1);//system_mode 01, reset dsp		
			break;
			
        case TLG_VSTD_NTSC_D_K:
			TLG_WriteReg( base_addr, 0x140,0x002d);//angle_mode_if, angle_mode_fm 
			TLG_WriteReg( base_addr, 0x143,0x1f4b);//notch coefficients

			TLG_WriteReg( base_addr, 0x104, 0x31c7);//9'h104,16'h31c7; angle_in_if
			TLG_WriteReg( base_addr, 0x111, 0x5413);//9'h111,16'h5413; V2.3
			
			TLG_WriteReg( base_addr, 0x137,0x034c);//h_sync_win_end
			TLG_WriteReg( base_addr, 0x139,0x03ac);//h_blank_win_end
			
			TLG_WriteReg( base_addr, 0x1a0, 0x1f39);//9'h1a0,16'h1f39;Notch filter for audio
			TLG_WriteReg( base_addr, 0x1a1, 0x071e);//9'h1a1,16'h071e;
			TLG_WriteReg( base_addr, 0x1a2, 0x0070);//9'h1a2,16'h0070;			

			dsp_system_reset(1);//system_mode 01, reset dsp		
		    break;
			
        case TLG_VSTD_NTSC_I:
			TLG_WriteReg( base_addr, 0x140,0x0024);//angle_mode_if, angle_mode_fm 
			TLG_WriteReg( base_addr, 0x143,0x1d8c);//notch coefficients

			TLG_WriteReg( base_addr, 0x104,0x284c);//9'h104,16'h284c; angle_in_if
			TLG_WriteReg( base_addr, 0x111,0x5413);//9'h111,16'h5413; V2.3
			
			TLG_WriteReg( base_addr, 0x137,0x034c);//h_sync_win_end
			TLG_WriteReg( base_addr, 0x139,0x03ac);//h_blank_win_end
			
			TLG_WriteReg( base_addr, 0x1a0, 0x1d7a);//9'h1a0,16'h1f39;Notch filter for audio
			TLG_WriteReg( base_addr, 0x1a1, 0x071e);//9'h1a1,16'h071e;
			TLG_WriteReg( base_addr, 0x1a2, 0x0070);//9'h1a2,16'h0070;			

			dsp_system_reset(1);//system_mode 01, reset dsp		
			break;

		case TLG_VSTD_NTSC_M:
			TLG_WriteReg( base_addr, 0x140, 0x0080);//9'h140,16'h0040; angle_mode_if, angle_mode_fm 
			TLG_WriteReg( base_addr, 0x143, 0x1897);//9'h143,16'h1897; notch coefficients

			TLG_WriteReg( base_addr, 0x104, 0x20b6);//9'h104,16'h20b6; angle_in_if
			TLG_WriteReg( base_addr, 0x111, 0x43e1);//9'h111,16'h5413; V2.3
			
			TLG_WriteReg( base_addr, 0x137, 0x034c);//9'h137,16'h034c; h_sync_win_end
			TLG_WriteReg( base_addr, 0x139, 0x03ac);//9'h139,16'h03ac; h_blank_win_end

			TLG_WriteReg( base_addr, 0x1a0, 0x1872);//9'h1a0,16'h1872;Notch filter for audio
			TLG_WriteReg( base_addr, 0x1a1, 0x0749);//9'h1a1,16'h0749;
			TLG_WriteReg( base_addr, 0x1a2, 0x005b);//9'h1a2,16'h005b;			

			TLG_WriteReg( base_addr, 0x1ac, 0x0030);
			
			dsp_system_reset(1);//system_mode 01, reset dsp
			break;
			
		default:
			break;
	}

    return(TLG_ERR_SUCCESS);
}

extern int tlg_cur_std;
extern int tlg_init_done;
int RDA_ScanTVChn(uint32 base_addr, uint32 center_freq_hz)
{
	int res = 0;
	int ms = 100;
	uint16 var,reg,regb,rega,bit1,bit0,reg130;
	int cnt;
	uint32 center_freq_khz = center_freq_hz/1000;

	//DISABLE_VIEW_FINDER_MODE;

    // DCDC Init.
    if (!tlg_init_done)
    {
	    TLGAPP_Init(0x62);
		TLG_Delay((uint16)10);
    }

	calc_freq_offset((uint16)tlg_cur_std);
	
	center_freq_hz = center_freq_khz - m_nFreqOffset;

	kal_prompt_trace(0,"[rdamtv]TLG_ScanTVChn, c_freq = %d, freq_offset = %d\n", center_freq_hz, m_nFreqOffset);

#if (1)
	//set center freq
	regb = (uint16)(((unsigned long long)center_freq_hz<<8)/67500);
	rega = (uint16)((((unsigned long long)((center_freq_hz<<8) - (regb * 67500))) << 16) /67500);
#elif (0)
	//set center freq
	regb=(center_freq_hz&0xffff0000)>>16;
	rega=center_freq_hz&0xffff;
#else
	rega = 0x999a;
	regb = 0x07d9;
#endif

    // Set Frequency
	TLG_WriteReg( base_addr, 0x0a, rega);
	TLG_WriteReg( base_addr, 0x0b, regb);

	RDA_Enable(base_addr);
	//RDA_RxOff(base_addr);
	RDA_RxOn(base_addr);
	TLG_Delay((uint16)20);

	//dig inital
	RDA_DigitalInit(base_addr);

	TLG_SetChnStandard(0x62, (uint16)tlg_cur_std);

	RDA_SetNotchFilter((uint16)tlg_cur_std, center_freq_khz);

	TLG_Delay((uint16)30);

	reg = 0;
	cnt = 0;
	do
	{
		TLG_Delay((uint16)10);
		//get bit_done : reg( 0x100[1])
		TLG_ReadReg(base_addr,0x0100,&reg);
		TLGHAL_GET(var,reg,0x01,0x02); //reg100[1] 
		cnt++;
	 } while ((var == 0 ) && (cnt < CHECK_SEEK_READY_TIMES));

	if (cnt == CHECK_SEEK_READY_TIMES)
	    kal_prompt_trace(0,"[rdamtv]TLG_ScanTVChn, timeout\n");	
	
	//get bit_succ :reg(0x0100[2])   :1 succ :0 fail
	TLG_ReadReg(base_addr,0x0100,&reg);
	TLGHAL_GET(var,reg,0x02,0x04);
	res = var ? 1 : 0;

#ifdef RDA5888_INCHFILTER_ENABLE
	if (res)
	{
	    res = InvalidChannelFilter();
	}
#endif

	TLG_WriteReg(base_addr, 0x108,0x03ff);
	TLG_WriteReg(base_addr, 0x1ad,0x0);
	
	RDA_SysReset();

#if 0
	TLG_WriteReg(base_addr, 0x130,0x0803);
#endif

#ifdef RDA5888_VIDENHANCE_ENABLE
	g_nFirstBufferFlag = 5;
#endif

    //ENABLE_VIEW_FINDER_MODE;

	kal_prompt_trace(0,"[rdamtv]TLG_ScanTVChn end, seek_done = %d\n", res);

	return res;
}

// ---------------------------------------------------------------------------
// Set RDA5888 mute/unmute.
//
// Params:
//	bMute: set TRUE to mute RDA5888, else unmute RDA5888.
//
// Return: none
// --------------------------------------------------------------------------- 
void RDA_SetMute(bool bMute)
{
    if (bMute)
    {
        TLG_WriteReg(0x62,0x10,0xc500);  // close adac
    }
	else
	{
	    TLG_WriteReg(0x62,0x10,0x8500);  // open adac
	}
}

// ---------------------------------------------------------------------------
// Get RDA5888 Driver Version.
//
// Params:
//	None.
//
// Return: g_stRdaVersion
// ---------------------------------------------------------------------------
const rda_version_t *RDA_GetVersion(void)
{
	kal_prompt_trace(0, "[rdamtv]SoftVer: %s-%d.%d-%s-%d.%d.%d-%s", g_stRdaVersion.ProductCode, 
		g_stRdaVersion.Cx, g_stRdaVersion.Cy, g_stRdaVersion.DateTime, g_stRdaVersion.Sx,
		g_stRdaVersion.Sy, g_stRdaVersion.Sz, g_stRdaVersion.Remark);
	
	return &g_stRdaVersion;
}

#ifdef RDA5888_VIDENHANCE_ENABLE
// --------------------------------------------------------------------------------
// wuhp@2009-12-19, add video enhance process for rda5888.
// --------------------------------------------------------------------------------
#define R_MASK         0xF800F800
#define G_MASK         0x07E007E0
#define B_MASK         0x001F001F
#define R_SHIFT        11
#define G_SHIFT        5
#define RDA_INT_MEM_SIZE        (192) // 128 256 512

//#define RDA5888_INTMEM_ENABLE
//#define RDA5888_DMACOPY_ENABLE

uint8 g_nVideoEnhanceEnable = 1; // enable/disable video enhance function.
uint8 g_nVideoEnSet = 0;  // video enhance state(close/open)
uint8 g_nDiscardFrameNum = 2; // actual discard frame: g_nDiscardFrameNum - 1
uint32 g_nFrameBufSize = 0;
uint8 g_nFirstBufferFlag = 5;  // 1: first frame buffer, 0: other frame buffer.

void RDA_VideoEnhance(unsigned int buffer1, unsigned int buffer2, unsigned int size)
{
#ifdef RDA5888_INTMEM_ENABLE
    unsigned char buf1[RDA_INT_MEM_SIZE], buf2[RDA_INT_MEM_SIZE];
	unsigned short *pIntBuf1 = (unsigned short *)buf1;
	unsigned short *pIntBuf2 = (unsigned short *)buf2;
	unsigned short *pBuf1 = (unsigned short *)buffer1;
	unsigned short *pBuf2 = (unsigned short *)buffer2;
	unsigned short pixel_r, pixel_g, pixel_b;
	unsigned int i, j;

	g_nFrameBufSize = size;

	if (g_nFirstBufferFlag)
	{
		g_nFirstBufferFlag--;
        return;
	}

    kal_prompt_trace(0, "RDA_VideoEnhance_0, size = %d\n", size);

	for (i = 0; i < size; i+=RDA_INT_MEM_SIZE)
	{
    #ifdef RDA5888_DMACOPY_ENABLE
		DMA_memcpy((void *)pBuf1, (void *)buf1, RDA_INT_MEM_SIZE);
		DMA_memcpy((void *)pBuf2, (void *)buf2, RDA_INT_MEM_SIZE);
    #else
		memcpy((void *)buf1, (void *)pBuf1, RDA_INT_MEM_SIZE);
		memcpy((void *)buf2, (void *)pBuf2, RDA_INT_MEM_SIZE);
    #endif
		for (j = 0; j < RDA_INT_MEM_SIZE; j+=2)
		{
			pixel_r = (((*pIntBuf1 & R_MASK) >> R_SHIFT) + ((*pIntBuf2 & R_MASK) >> R_SHIFT)) >> 1;
			pixel_g = (((*pIntBuf1 & G_MASK) >> G_SHIFT) + ((*pIntBuf2 & G_MASK) >> G_SHIFT)) >> 1;
			pixel_b = ((*pIntBuf1 & B_MASK) + (*pIntBuf2 & B_MASK)) >> 1;
			
			*pIntBuf1 = (pixel_r << R_SHIFT) | (pixel_g << G_SHIFT) | (pixel_b);

			pIntBuf1++;
			pIntBuf2++;
		}
		pIntBuf1 = (unsigned short *)buf1;
	    pIntBuf2 = (unsigned short *)buf2;
    #ifdef RDA5888_DMACOPY_ENABLE		
		DMA_memcpy((void *)buf1, (void *)pBuf1, RDA_INT_MEM_SIZE);
		DMA_memcpy((void *)buf2, (void *)pBuf2, RDA_INT_MEM_SIZE);
    #else
		memcpy((void *)pBuf1, (void *)buf1, RDA_INT_MEM_SIZE);
		memcpy((void *)pBuf2, (void *)buf2, RDA_INT_MEM_SIZE);
    #endif
		pBuf1 += (RDA_INT_MEM_SIZE >> 1);
		pBuf2 += (RDA_INT_MEM_SIZE >> 1);
	}
#else
	unsigned int *pBuf1 = (unsigned int *)buffer1;
	unsigned int *pBuf2 = (unsigned int *)buffer2;
	unsigned int i;

	g_nFrameBufSize = size;

	if (g_nFirstBufferFlag)
	{
		g_nFirstBufferFlag--;
        return;
	}

    kal_prompt_trace(0, "[rdamtv]RDA_VideoEnhance_1, size = %d\n", size);

	size = size >> 3;

	for (i = size; i != 0; i--)
	{
		*pBuf1 = (((*pBuf1 & 0xF000F000) >> 1) + ((*pBuf2 & 0xF000F000) >> 1))
				 | (((*pBuf1 & 0x07C007C0) >> 1) + ((*pBuf2 & 0x07C007C0) >> 1))
				 | (((*pBuf1 & 0x001E001E) >> 1) + ((*pBuf2 & 0x001E001E) >> 1));
		pBuf1++;
		pBuf2++;

		*pBuf1 = (((*pBuf1 & 0xF000F000) >> 1) + ((*pBuf2 & 0xF000F000) >> 1))
				 | (((*pBuf1 & 0x07C007C0) >> 1) + ((*pBuf2 & 0x07C007C0) >> 1))
				 | (((*pBuf1 & 0x001E001E) >> 1) + ((*pBuf2 & 0x001E001E) >> 1));
		pBuf1++;
		pBuf2++;
	}
#endif
}
#endif // end RDA5888_VIDENHANCE_ENABLE

// -------------------------------------------------------------------------------------------------
// RDA5888 FM DRIVER
// -------------------------------------------------------------------------------------------------
#ifdef RDA5888_FM_ENABLE
#define FM_RSSI_OFFSET 	      57
#define FM_SEEK_TIMER         20
#define FM_SEEK_THRESHOLD     5

uint16 RDA_GetFMRSSI(void)
{
    uint16 reg, i, data1, data2, data3, rssi[3];

	//kal_prompt_trace(0, "[rdamtv]RDA_GetFMRSSI");

    for (i = 0; i < 3; i++)
	{
	    //将寄存器12FH写成000ah，读取117H的14~8bit的值，记为D1，再将寄存器12FH写成0019h，读取117H的14~8bit的值，记为D2；
	    TLG_WriteReg(0x62,0x12F,0x000a);
	    TLG_ReadReg(0x62,0x117,&reg);
		TLGHAL_GET(data1,reg,0x08,0x7f00);
		
	    TLG_WriteReg(0x62,0x12F,0x0019);
	    TLG_ReadReg(0x62,0x117,&reg);
		TLGHAL_GET(data2,reg,0x08,0x7f00);

		//读取96H的2~0bit的值，此值即为档位值，根据此值获得档值对应的D3：
	    TLG_ReadReg(0x62,0x96,&reg);
		TLGHAL_GET(data3,reg,0x0,0x0007);
		switch (data3)
		{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
				data3 = 10;
				break;
				
			case 5:
				data3 = 28;
			    break;
				
			case 6:
				data3 = 44;
				break;
				
			case 7:
				data3 = 55;
				break;

			default:
				kal_prompt_trace(0, "RDA_GetFMRSSI, error\n");
				break;
		}

		rssi[i]= data1 - data2 - data3 - FM_RSSI_OFFSET + 107;
		
		//kal_prompt_trace(0, "RDA_GetFMRSSI, d1=%d, d2=%d, d3=%d, rssi=%d\n", data1, data2, data3, rssi[i]);
	}

	return ((rssi[0]+rssi[1]+rssi[2])/3);
}

void FMDrv_PowerOnReset(void)
{
    uint16 reg;

	kal_prompt_trace(0, "[rdamtv]FMDrv_PowerOnReset");
	
    // hardware initial
    GPIO_ModeSetup(ATV_I2C_SDA_PIN,0);
	GPIO_ModeSetup(ATV_I2C_SCL_PIN,0);
	GPIO_ModeSetup(ATV_POWER_PIN,0);
	GPIO_InitIO(1, ATV_I2C_SDA_PIN);
	GPIO_InitIO(1, ATV_I2C_SCL_PIN);
	GPIO_InitIO(1, ATV_POWER_PIN);

	// power on
    GPIO_WriteIO(1, ATV_POWER_PIN);

	TLG_Delay(100);

    RDA_DCDCInit(0x62);
	FMDrv_SetFreq(870);
	TLG_Delay((uint16)20);
	RDA_DigitalInit(0x62);

	//将寄存器183H的3bit置为1，寄存器105H的15bit置为1，向寄存器121H写入0020h；
	TLG_ReadReg(0x62,0x183,&reg);
	TLGHAL_SET(reg,0x1,REG_BIT3_SHIFT,REG_BIT3_MASK); //reg:0x183[3]->1
	TLG_WriteReg(0x62,0x183,reg);
	TLG_ReadReg(0x62,0x105,&reg);
	TLGHAL_SET(reg,0x1,REG_BIT15_SHIFT,REG_BIT15_MASK); //reg:0x105[15]->1
	TLG_WriteReg(0x62,0x105,reg);
	TLG_WriteReg(0x62,0x121,0x0020);
}

void FMDrv_PowerOffProc(void)
{
    kal_prompt_trace(0, "[rdamtv]FMDrv_PowerOffProc");
	
    // hardware initial
    GPIO_ModeSetup(ATV_I2C_SDA_PIN,0);
	GPIO_ModeSetup(ATV_I2C_SCL_PIN,0);
	GPIO_ModeSetup(ATV_POWER_PIN,0);
	GPIO_InitIO(1, ATV_I2C_SDA_PIN);
	GPIO_InitIO(1, ATV_I2C_SCL_PIN);
	GPIO_InitIO(1, ATV_POWER_PIN);

	// power off and set iic pin state.
    GPIO_WriteIO(0, ATV_POWER_PIN);
    GPIO_WriteIO(1, ATV_I2C_SDA_PIN);
    GPIO_WriteIO(1, ATV_I2C_SCL_PIN);
}

// 99MHZ, 0x0a = 0xc537, 0x0b = 0x0168
void FMDrv_SetFreq(signed short curf)
{
    uint16 rega, regb;
	uint32 c_freq = curf;
	
    c_freq *= 100; // convert to khz.
    //kal_prompt_trace(0, "[rdamtv]FMDrv_SetFreq, freq = %d", c_freq);
	c_freq -= 3875; // couvert to center frequency.

	//set center freq
	regb = (uint16)(((unsigned long)c_freq<<8)/67500);
	rega = (uint16)((((unsigned long)((c_freq<<8) - (regb * 67500))) << 16) /67500);

    //kal_prompt_trace(0, "[rdamtv]FMDrv_SetFreq, rega = %x, regb = %x", rega, regb);
	
    // Set Frequency
	TLG_WriteReg(0x62, 0x0a, rega);
	TLG_WriteReg(0x62, 0x0b, regb);

	RDA_RxOn(0x62);
}

uint16 FMDrv_GetSigLvl(int16 curf)
{
    uint16 rssi = 0;
	
    if (875 == curf)
    {
		rssi = 1;
    }
	else
	{
        rssi = RDA_GetFMRSSI();
        //rssi = 30;
	}

	//kal_prompt_trace(0, "[rdamtv]FMDrv_GetSigLvl, rssi = %d", rssi);

	return rssi;
}

uint8 FMDrv_ValidStop(int16 freq, int8 signalvl, bool is_step_up)
{
    uint16 rssi_1, rssi_2, rssi_3;

    //kal_prompt_trace(0, "[rdamtv]FMDrv_ValidStop");
	
	// get rssi_1
    FMDrv_SetFreq(freq-1);
	TLG_Delay(FM_SEEK_TIMER);
	rssi_1 = RDA_GetFMRSSI();

	// get rssi_2
    FMDrv_SetFreq(freq);
	TLG_Delay(FM_SEEK_TIMER);
	rssi_2 = RDA_GetFMRSSI();

	// get rssi_3
    FMDrv_SetFreq(freq+1);
	TLG_Delay(FM_SEEK_TIMER);
	rssi_3 = RDA_GetFMRSSI();

	// remove 96.0MHZ channel
	if (freq-1 == 960)
	{
	    rssi_1 = 0;
	}
	else if (freq == 960)
	{
	    rssi_2 = 0;
	}
	else if (freq+1 == 960)
	{
	    rssi_3 = 0;
	}

	//kal_prompt_trace(0, "[rdamtv]FMDrv_ValidStop, rssi: %d, %d, %d", rssi_1, rssi_2, rssi_3);

	if (((rssi_2 - rssi_1) >= FM_SEEK_THRESHOLD)
		&&((rssi_2 - rssi_3) >= FM_SEEK_THRESHOLD))
	{
	    kal_prompt_trace(0, "[rdamtv]FMDrv_ValidStop, freq = %d, rssi = %d!!!", freq, rssi_2);
	    return 1;
	}
	else
	{
	    //kal_prompt_trace(0, "[rdamtv]FMDrv_ValidStop, fail!!!");
	    return 0;
	}
}

void FMDrv_Mute(uint8 mute)
{
    kal_prompt_trace(0, "[rdamtv]FMDrv_Mute");
}

void FMDrv_ChipInit(void)
{
	kal_prompt_trace(0, "[rdamtv]FMDrv_ChipInit");
}

bool FMDrv_IsChipValid(void)
{
    uint16 reg;

	kal_prompt_trace(0, "[rdamtv]FMDrv_IsChipValid");
	
	TLG_ReadReg(0x62,  0x0,&reg);
	if (0x31E2 == reg)
	{
	    return true;
	}
	else
	{
	    return false;
	}
}
#endif // END RDA888_FM_ENABLE
