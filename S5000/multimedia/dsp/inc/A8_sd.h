#ifndef	a810h_sd_h
#define	a810h_sd_h

#define	SD_REG_BASE		0x6400

#define	SD_CONTROL_I		(SD_REG_BASE + 0x00)
#define	SD_CONTROL_II		(SD_REG_BASE + 0x01)
#define	SD_CPU_INT_STAT		(SD_REG_BASE + 0x02)
#define	SD_CPU_INT_EN		(SD_REG_BASE + 0x03)
#define	SD_BUS_STATUS		(SD_REG_BASE + 0x04)

#define	SD_COMMAND_ARG_L	(SD_REG_BASE + 0x08)	
#define	SD_COMMAND_ARG_H	(SD_REG_BASE + 0x0A)	
#define	SD_DMA_START_ADDR_L	(SD_REG_BASE + 0x0C)	
#define	SD_DMA_START_ADDR_H	(SD_REG_BASE + 0x0E)	

#define	SD_BLOCK_NUM		(SD_REG_BASE + 0x10)
#define	SD_BLOCK_LEN		(SD_REG_BASE + 0x12)

#define	SD_RESPONSE_TYPE	(SD_REG_BASE + 0x14)
#define	SD_COMMAND_TYPE		(SD_REG_BASE + 0x15)
#define	SD_CLOCK_CTL		(SD_REG_BASE + 0x16)
#define	SD_TIMEOUT			(SD_REG_BASE + 0x18)

#define	SD_HOST_INT_STAT	(SD_REG_BASE + 0x22)
#define	SD_HOST_INT_EN		(SD_REG_BASE + 0x23)

#define	SD_PARAMETER_BUF	(SD_REG_BASE + 0x30)
#define	SD_PARAMETER_BUF_VI_B   (SD_REG_BASE + 0x35)
#define	SD_PARAMETER_BUF_VII_W  (SD_REG_BASE + 0x36)
#define	SD_PARAMETER_BUF_IX_W   (SD_REG_BASE + 0x38)
#define	SD_PARAMETER_BUF_XI_W   (SD_REG_BASE + 0x3A)
#define	SD_PARAMETER_BUF_XV_W   (SD_REG_BASE + 0x3E)
#define	SD_PARAMETER_BUF_XVI_B  (SD_REG_BASE + 0x3F)

/*================
  SD_CONTROL_I  
================*/
#define	SD_4_WIRE_MODE_EN	0x80
#define	SD_1_WIRE_MODE_EN	0x00
#define	MMC_MODE_EN			0x40
#define	PUSH_PULL_TYPE		0x20
#define	MULTIPLE_BLOCK_MODE	0x04
#define	SINGLE_BLOCK_MODE	0x02

/*=================
  SD_CONTROL_II  
=================*/
#define	SD_TRANSFER_START	0x80

/*===================================
  SD_CPU_INT_STAT, SD_CPU_INT_EN   
  SD_HOST_INT_STAT, SD_HOST_INT_EN 
===================================*/
#define	DATA_BLOCK_TRANSFER_DONE	0x01
#define	DATA_RESPONSE_TIMEOUT		0x02
#define	COMMAND_RESPONSE_DONE		0x04
#define	CPU_PATH_DONE				0x08
#define	CRC7_ERROR					0x10
#define	CRC16_ERROR					0x20
#define	BUS_STATUS_ERROR			0x40
#define	COMMAND_RESPONSE_TIMEOUT	0x80

/*===================
  SD_BUS_STATUS_B  
===================*/
#define	DATA0_STATUS_HIGH	0x10
#define	DATA1_STATUS_HIGH	0x20
#define	DATA2_STATUS_HIGH	0x40
#define	DATA3_STATUS_HIGH	0x80

/*================
  SD_CLOCK_CTL  
================*/
#define	SD_CLOCK_DIV_2		0x0001
#define	SD_CLOCK_DIV_4		0x0002
#define	SD_CLOCK_DIV_8		0x0004
#define	SD_CLOCK_DIV_16		0x0008
#define	SD_CLOCK_DIV_32		0x0010
#define	SD_CLOCK_DIV_64		0x0020
#define	SD_CLOCK_DIV_128	0x0040
#define	SD_CLOCK_DIV_256	0x0080

#define	SD_CLOCK_DIV_3		0x0100

/*===============================================*/
#define	SD_NO_ERROR		0x00
#define	SD_RESET_ERROR	0x01
#define	SD_TIME_OUT		0x02
#define	SD_COMMAND_FAILED	0x03
#define	SD_PARAM_ERROR	0x04

#define SUPPORT_VOLTAGE	0x003C0000

#define ENA_RESP_TYPE_PROG      0x80
#define DIS_RESP_WAIT_STATE     0x40
#define OTHER_RESP              (0x10|ENA_RESP_TYPE_PROG)
#define R3_RESP                 (0x80|ENA_RESP_TYPE_PROG)
#define R2_RESP                 (0x40|ENA_RESP_TYPE_PROG)
#define R1B_RESP                (0x02|ENA_RESP_TYPE_PROG)
#define R1_RESP                 (0x01|ENA_RESP_TYPE_PROG)

/* MMC/SD Command Index*/
/* Basic command (class 0) */
#define GO_IDLE_STATE		0 
#define	SEND_OP_COND		1	/* reserved for SD*/
#define	ALL_SEND_CID		2	
#define	SET_RELATIVE_ADDR	3
#define	SEND_RELATIVE_ADDR	3
#define	SET_DSR				4
#define	SELECT_CARD			7	
#define	DESELECT_CARD		7	
#define	SEND_IF_COND		8
#define	SEND_CSD			9	
#define	SEND_CID			10	
#define	READ_DAT_UTIL_STOP	11	/* reserved for SD*/
#define	STOP_TRANSMISSION	12
#define	SEND_STATUS			13
#define	GO_INACTIVE_STATE	15

/* Block oriented read commands (class 2)*/
#define	SET_BLOCKLEN		16
#define	READ_SINGLE_BLOCK	17
#define	READ_MULTIPLE_BLOCK	18

/* Block oriented write commands (class 4)*/
#define	WRITE_BLOCK			24
#define	WRITE_MULTIPLE_BLOCK	25
#define	PROGRAM_CSD			27

/* Erase commands*/
#define	ERASE_WR_BLK_START	32
#define	ERASE_WR_BLK_END	33
#define	ERASE				38

/* Application specific commands (class 8)*/
#define	APP_CMD				55
#define	GEN_CMD				56

/* SD Application command Index*/
#define	SET_BUS_WIDTH			6
#define	SD_STATUS				13
#define	SEND_NUM_WR_BLOCKS		22
#define	SET_WR_BLK_ERASE_COUNT	23
#define	SD_APP_OP_COND			41
#define	SET_CLR_CARD_DETECT		42
#define	SEND_SCR				51

extern u_char* A8L_GetSDCSDValue(void);
extern u_short	A8L_InitSDIF(void);
extern u_short	A8L_ResetSD(void);
extern u_short	A8L_ResetMMC(void);
extern u_int	A8L_GetSDSize(void);
extern u_int	A8L_GetSDCardAddr(void);
extern u_short	A8L_SendSDCommand(u_char command, u_int argument);
extern u_short	A8L_ReadSDSector(u_int dmastartaddr, u_int startsect, u_short sectcount);
extern u_short	A8L_WriteSDSector(u_int dmastartaddr,  u_int startsect, u_short sectcount);
extern u_short	A8L_EnableStorageClock(u_short enable);

extern u_short	A8L_ReadSD(u_int startsect, u_int offset, u_char* buf, u_int read_size);
extern u_short	A8L_WriteSD(u_int startsect, u_int offset, u_char* buf, u_int write_size);
extern u_short A8L_CheckSDDevice(void);
extern u_short A8L_EraseDevice(void);
extern u_short	A8L_SetStorageDmaAddr(u_int dmastartaddr, u_int dmasize);

extern u_short	A8L_CheckSDCardIn(void);
extern u_short	A8L_CheckSDCardWP(void);
extern u_short A8L_InfoSDCardOut(void);/* AIT_SUNNY_20051010 */
#endif		
