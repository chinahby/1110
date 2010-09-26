/**
 * @file AIT700_EBIbus.h
 * @brief Copyright (C) 2002-2008 Alpha Imaging Technology Corp.
 * @n AIT701/703 Series Host API.
 * @n The purpose of AIT701 Host API is for Host CPU to control and communicate AIT701
 *      chip via direct/indirect parallel interface.
 * @note Data bus interface between Host and AIT chip.
 * @bug N/A
 */


#if !defined (AIT_EBIBUS_H)
#define AIT_EBIBUS_H
#include "A8_common.h"



#if defined (NXP_PLATFORM)
#define AIT800_REG_BASE          (0x80000000)		//  4902 CS0
#define CMD_SIGNAL_PIN				(0x00100000)		//4902 1st pin(A20)
#elif defined (MTK_PLATFORM)
#define AIT800_REG_BASE			(0x12000000)//(LCD_base+0x4000)
#define CMD_SIGNAL_PIN				(0x01000000)
#elif defined (__QSC_TARGET__)
#define AIT800_REG_BASE			EBI2_LCD_BASE//(LCD_base+0x4000)
#define CMD_SIGNAL_PIN			0x100000	//(0x01000000)
#elif defined (AIT701_DEMO_BOARD)
#define AIT800_REG_BASE          (0x02000000)
#define CMD_SIGNAL_PIN           (0x00080000)
#else   //add by yangdecai 09-24
#define AIT800_REG_BASE			(0x12000000)//(LCD_base+0x4000)
#define CMD_SIGNAL_PIN				(0x01000000)
#endif

#define INDIRECT_DATA_PORT       AIT800_REG_BASE 
#define INDIRECT_CMD_PORT        (AIT800_REG_BASE+CMD_SIGNAL_PIN)

#ifdef DEFINE_GSBINDDATAPORT
volatile u_char * gsbIndCmdPort = (u_char *)INDIRECT_CMD_PORT;
volatile u_char * gsbIndDataPortB = (u_char *)INDIRECT_DATA_PORT;
volatile u_short * gsbIndDataPortW = (u_short *)INDIRECT_DATA_PORT;
#else
extern volatile u_char * gsbIndCmdPort;
extern volatile u_char * gsbIndDataPortB;
extern volatile u_short * gsbIndDataPortW;
#endif

#define A8IndCmdP                      (*gsbIndCmdPort)
#define A8IndDataPW(Addr)              (*(gsbIndDataPortW))
#define A8IndDataPB(Addr)              (*(gsbIndDataPortB+(Addr&0x0001)))

//
//  Host Indirect Interface Command Definition
//
#define HII_SET_ADDR_H                    (0)
#define HII_SET_ADDR_L                    (1)
#define HII_RW_REG_B                      (2)
#define HII_RW_REG_B_INC                  (3)
#define HII_RW_REG_W                      (4)
#define HII_RW_REG_W_INC                  (5)
#define HII_RW_MEM_B                      (6)
#define HII_RW_MEM_B_INC                  (7)
#define HII_RW_MEM_W                      (8)
#define HII_RW_MEM_W_INC                  (9)


#define HOST_PARA_INTERFACE               (0)      // Reserve for backward compatible
#define HOST_INDIRECT_INTERFACE           (1)      // Reserve for backward compatible

//
// The Definition for Indirect Host Interface
//

void	SetA8RegW(u_short Addr, u_short Val);
void	SetA8RegB(u_short Addr, u_char Val);
u_short	GetA8RegW(u_short Addr);
u_char	GetA8RegB(u_short Addr);
void	SetA8MemW(u_int Addr, u_short Val);
void	SetA8MemB(u_int Addr, u_char Val);
u_short	GetA8MemW(u_int Addr);
u_char	GetA8MemB(u_int Addr);
void	SetA8MemAddr(u_int Addr);
void	SetA8MemNextW(u_short data);
u_short	GetA8MemNextW(void);

void	CopyMemByteHostToA8(u_int DestAddr, u_char *SrcAddr, u_int Length);
void	CopyMemByteA8ToHost(u_char *DestAddr, u_int SrcAddr, u_int Length);
void CopyMemWordHostToA8(u_int destAddr, u_short *srcAddr, u_int length);
void CopyMemWordA8ToHost(u_short *destAddr, u_int srcAddr, u_int length);

void	OpenFIFO( u_short FIFOAddr );
void	PutWordFIFO( u_short Data );
u_short	GetWordFIFO(void);

s_short	SendA8Cmd(u_short cmd);
s_short	SendA8LongCmd(u_short cmd, u_short arg);
u_short	GetA8DSCStatus(void);
void SetHostBusMode(void);
void	ClearA8Mem(u_int destAddr, u_short Val, u_int length );
u_short SetA8GrpFIFODataW(u_short **pshorttmp);
void A8L_Get16WDataFromFIFO(u_short FIFO_Reg, u_short* DestAddr, u_int Length);

u_short	TimeOutWhile(u_short status);
u_short CheckTimeOut(void);

#define A8_HOST_CMD_RG						(0x6538)
#define A8_HOST_ARG_RG						(0x6540)
#define A8_DSC_STATUS_RG					(0x654E)


//#define SendA8Cmd(cmd)						(*fvSetA8RegW)((A8_HOST_CMD_RG),(cmd))
//#define GetA8DSCStatus()					(*fvGetA8RegW)((A8_DSC_STATUS_RG))

//	AIT800 Host Command Definition
//
#define A8_HOST_CMD_SET_CAM_MODE				(0x01)
#define A8_HOST_CMD_SELECT_SEC_CAM_MODE		(0x02)
#define A8_HOST_CMD_SELECT_SEC_VIEW_MODE		(0x03)
#define A8_HOST_CMD_SET_PREVIEW_MODE			(0x04)
//#define A8_HOST_CMD_STOP_PREVIEW				(0x05)
#define A8_HOST_CMD_INIT_SENSOR					(0x06)
#define A8_HOST_CMD_INIT_LCD					(0x07)
#define A8_HOST_CMD_REQUIRE_ECHO				(0x08)
#define A8_HOST_CMD_BY_PASS_REQ				(0x09)
#define A8_HOST_CMD_RESUME_REQ				(0x0A)
#define A8_HOST_CMD_GET_FW_VER					(0x0B)
#define A8_HOST_CMD_GET_CHIP_VER				(0x0C)
#define A8_HOST_CMD_SELECT_LCD_ACTIVE			(0x0D)
#define A8_HOST_CMD_SET_ACTIVE_LCD				(0x0F)

#define A8_HOST_CMD_TAKE_JPEG					(0x10)
#define A8_HOST_CMD_GET_JPEG_SIZE				(0x11)
#define A8_HOST_CMD_TAKE_RGB					(0x12)
#define A8_HOST_CMD_TAKE_RGB_STICK					(0x13)
#define A8_HOST_CMD_ENCODE_RGB_JPEG				(0x14)
#define A8_HOST_CMD_TAKE_CONTINUE_JPEG				(0x15)
#define A8_HOST_CMD_TAKE_STICK_JPEG						(0x16)

#define A8_HOST_CMD_SET_WB_MODE					(0x20)
#define A8_HOST_CMD_SET_AE_MODE					(0x21)
#define A8_HOST_CMD_SET_EV_VALUE				(0x22)
#define A8_HOST_CMD_SET_NIGHT_MODE				(0x23)
#define A8_HOST_CMD_SET_DEBAND_MODE				(0x24)
#define A8_HOST_CMD_SET_JPEG_QUALITY			(0x25)
#define A8_HOST_CMD_SET_ZOOM_RATE				(0x26)
#define A8_HOST_CMD_SET_IMAGE_EFFECT			(0x27)
#define A8_HOST_CMD_SET_VIDEO_FPS			(0x28)
#define A8_HOST_CMD_SET_SENSOR_FLIP        (0x29)

#define A8_HOST_CMD_START_VIDEO_REC				(0x30)
#define A8_HOST_CMD_STOP_VIDEO_REC				(0x31)
#define A8_HOST_CMD_SET_FRAME_TAB_ADDRLO		(0x32)
#define A8_HOST_CMD_SET_FRAME_TAB_ADDRHI		(0x33)
#define A8_HOST_CMD_SET_FILE_SIZE_LIMIT_LO		(0x34)		// Set file size limit in KB.
#define A8_HOST_CMD_SET_FILE_SIZE_LIMIT_HI		(0x35)		// Set file size limit in KB.
#define A8_HOST_CMD_START_MJPEG_REC						(0x36)
#define A8_HOST_CMD_STOP_MJPEG_REC						(0x37)

#define A8_HOST_CMD_BYPASS_MODE   (0x36)
#define A8_HOST_CMD_ACTIVE_MODE   (0x37)
#define A8_HOST_CMD_AE_DEFAULT  			(0x38)

#define A8_HOST_CMD_FLASH_TRIG_PIN				(0x41)
#define A8_HOST_CMD_SET_FLASH_MODE				(0x42)
#define A8_HOST_CMD_SET_FLASH_ON				(0x43)
#define A8_HOST_CMD_SET_FLASH_OFF				(0x44)	
#define	A8_HOST_CMD_TRIGGER_FLASH				(0x46)	

#define A8_HOST_CMD_SET_SENSOR_REG        (0x50)
#define A8_HOST_CMD_GET_SENSOR_REG        (0x51)
#define A8_HOST_CMD_I2CDEVICERDADDR				(0x52)
#define A8_HOST_CMD_I2CDEVICEWRADDR				(0x53)
#define A8_HOST_CMD_I2C_INIT						(0x54)

#define	A8_HOST_CMD_SET_MATRIX					(0x55) // 0424
#define	A8_HOST_CMD_SET_CENTERWEIGHTED			(0x56)
#define	A8_HOST_CMD_SET_SPOT					(0x57)
#define	A8_HOST_CMD_SET_VIF_SRC					(0x58)

#define A8_HOST_CMD_SET_USB_CHANGE_MODE         (0x70)
#define A8_HOST_CMD_PCCAM_MEMORY                (0x74)
		#define PCCAM_COMPRESS_BUF      (0x0100)
		#define PCCAM_EXT_COMPRESS_BUF  (0x0200)
		#define PCCAM_LINE_BUF          (0x0300)
#define A8_HOST_CMD_MSDC_MEMORY                 (0x75)
#define A8_HOST_CMD_CDC_MEMORY               (0x7A)
		//  #define HANDSHAKE_BUFFER_ADDR   (0x0100) // multi-define
		#define CDC_TX_BUFFER_ADDR     (0x0200)
		#define CDC_RX_BUFFER_ADDR     (0x0300)
#define A8_HOST_CMD_USBBASE_MEMORY              (0x7E)
		#define HANDSHAKE_BUFFER_ADDR (0x0100)
		#define INTERFACE_BUF           (0x0200)
		#define RX_BUF                  (0x0300)
		#define SET_MANUFACTURERNAME	(0x0400)
		#define SET_PRODUCTNAME			(0x0500)

#define A8_HOST_CMD_SET_SENSOR_DIRECTION		(0x90)
#define A8_HOST_CMD_SET_SENSOR_ENVIRONMENT	(0x91)
#define A8_HOST_CMD_SET_SENSOR_FPS				(0x94)

#define A8_HOST_CMD_I2CRead    (0x92)  // 
#define A8_HOST_CMD_I2CWrite    (0x93)  // 

#define A8_HOST_SCMD_START_PREVIEW				(0x01 << 8)
#define A8_HOST_SCMD_STOP_PREVIEW				(0x00 << 8)

#define A8_HOST_SCMD_SUBLCD_ACTIVE				(0x01 << 8)
#define A8_HOST_SCMD_MAINLCD_ACTIVE				(0x00 << 8)

//  A8 DSC Status Definition
//
#define A8_DSC_VIDEO_STOP_ST					(0x01)		// bit 0
#define A8_DSC_PREVIEW_START_ST					(0x02)		// bit 1
#define A8_DSC_READY_FOR_CMD						(0x10)		// bit 4


#define A8_STATE_STILL_PREVIEW		(0x01)
#define A8_STATE_VIDEO_PREVIEW		(0x02)
#define A8_STATE_STILL_PLAYBACK		(0x03)
#define A8_STATE_VIDEO_PLAYBACK		(0x04)
#define A8_STATE_MENU				(0x05)
#define A8_STATE_SUSPEND			(0x06)
#define A8_STATE_OFF				(0x07)
#endif
