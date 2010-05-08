#ifndef _ME_PARSER_MID_
#define _ME_PARSER_MID_

#ifdef __cplusplus
extern "C" {
#endif
	
#include "..\..\me_comhead.h"

#define MAX_PCM_SIZE		0x20000
#define MAX_MIDI_CHN            16
#define MAX_MIDI_TRACK			64
#define MAX_MSG_NUM		    	128	//The max number of message in the message que
#define TEMPO_CODE				500000		// 120 bpm
#define Max_instrumet  128
#define	EMPTY_SIGNAL 129
	
#define MELTA_TEMPO				0x51
	
#define MELTA_EVENT				0xff
#define SYSEXCLUDE_EVENT        0xf0
#define MELTA_END               0x2f
	
#define MSG_KEYON				0x90
#define MSG_KEYOFF				0x80
#define MSG_PROGCHANGE			0xc0
#define MSG_SYSEXCLUDE			0xF0
#define MSG_PITCHBEND			0xe0
#define MSG_CTRLCHANGE			0xb0
#define MSG_CHNPRESSURE			0xd0
	
#define CTRL_BANK_MSB			0x00
#define CTRL_MODULATION			0x01
#define CTRL_DATAENTRYMSB		0x06
#define CTRL_DATAENTRYLSB		0x26
#define CTRL_VOLUME				0x07
#define CTRL_PANPOT				0x0a
#define CTRL_EXPRESS			0x0b
#define CTRL_BANK_LSB			0x20	
#define CTRL_HOLD1				0x40
#define CTRL_RESETCTRLS			0x79
#define CTRL_ALLNOTEOFF		    0x7b
#define CTRL_RPNLSB				0x64
#define CTRL_RPNMSB				0x65

#define MSG_NOTEON 0x90
	

#define VMD_MSG_NOTE_ON					(0)
#define VMD_MSG_NOTE_OFF				(1)
#define VMD_MSG_PROGRAM_CHANGE			(2)
#define VMD_MSG_MODULATION_DEPTH		(3)
#define VMD_MSG_CHANNEL_VOLUME			(4)
#define VMD_MSG_PANPOT					(5)
#define VMD_MSG_EXPRESSION				(6)
#define VMD_MSG_HOLD1					(7)
#define VMD_MSG_PITCH_BEND				(8)
#define VMD_MSG_BEND_RANGE				(9)
#define VMD_MSG_CHANNEL_PRESSURE		(10)
#define VMD_MSG_RESET_ALL_CONTROLLERS	(11)
#define VMD_MSG_ALL_NOTE_OFF			(12)
#define VMD_MSG_ALL_SOUND_OFF			(13)
#define VMD_MSG_STREAM_ON				(14)
#define VMD_MSG_STREAM_OFF				(15)
#define VMD_MSG_STREAM_PANPOT			(16)
#define VMD_MSG_MASTER_VOLUME			(17)
#define VMD_MSG_BANK_SELECT				(18)
#define VMD_MSG_REVERVB_ENABLE			(19)
#define VMD_MSG_REVERVB_ON				(20)
#define VMD_MSG_REVERVB_OFF				(21)
#define VMD_MSG_EOS_EVENT				(22)
#define VMD_MSG_NOP						(23)
#define VMD_MSG_SET_TIMER				(24)
#define VMD_MSG_SET_REG_PAGE			(25)
#define VMD_MSG_USER_EVENT				(26)
#define VMD_MSG_UDSE_ON					(27)
#define VMD_MSG_MP3_ON					(28)
#define VMD_MSG_MP3_OFF					(29)
#define VMD_MSG_SOFT_INT				(30)
#define VMD_MSG_INT_ENABLE				(31)
#define VMD_MSG_INT_DISABLE				(32)
#define VMD_MSG_LED_ON					(33)
#define VMD_MSG_LED_OFF					(34)
#define VMD_MSG_MOTOR_ON				(35)
#define VMD_MSG_MOTOR_OFF				(36)
#define VMD_MSG_RESET             (37)
#define VMD_MSG_CONTROL           (38)
#define VMD_MSG_SPEC_EVENT        (39)
#define VMD_MSG_BANK_MSB				(40)
#define VMD_MSG_BANK_LSB				(41)



/*typedef struct 
{
	ME_U08 preset;   //0 - 127
	ME_U08 note[128];
}instrumentdata_info_t;*/

//bank  instrument or drum
//preset  instrument num
//lownote  lowest note in this track
//highnote highest note in this track

/*typedef struct 
{
	ME_U32 instru_num;
	instrumentdata_info_t instrumentdata_info[Max_instrumet];
	ME_U08 drumdata_info[128];
}midi_instrument_info_t;*/

//drumdata_info: each drum has its unique number, the sum of drums is 128.

typedef struct
{
	ME_U08	bTimbre;
	ME_U08	bPan;
	ME_U08	bModulation;
	ME_U08	bPitchBendH;
	ME_U08	bPitchBendL;
	ME_U08	bPressure;
	ME_U08	bExpression;
	ME_U08	bVolume;
	ME_U08	bHold1;
	ME_U08	bBendRage;
	//ME_U08	bBankID;
	ME_U16  bBankID;
} VM_MIDI_CHN;     //channel info

//typedef	struct
//{
//	ME_U08 bMsgID;			/* message_ID*/
//	int	sdDeltaTime ;	/* in ms. <0: immediate event */
//	ME_U08	bChn; 			/* channel */
//	unsigned int dPara1;			/* parameters for this message */
//	unsigned int	dPara2;
//	unsigned int	dPara3;
//} VM_UNI_MSG;

typedef struct
{
	VM_MIDI_CHN 	ChnInfo[MAX_MIDI_CHN];
//	VM_UNI_MSG		MsgQue[MAX_MSG_NUM]; 
	unsigned short	wRPN[MAX_MIDI_CHN];
//	ME_U08			bMsgQueNum;
    ME_U32			dTempoMS;
	ME_U32          LeftLen;
} SEEK_INFO;	

typedef struct  _VM_MIDI_INPUT{
	//parser supply
	ME_U32 total_len;
	ME_U32 SampleRate;
	ME_U32 MaxSythVoice;
	ME_U32 TrackDataLen;//track 0 data len for format 0
	ME_U32 Division;//dTicksPerQN

	ME_U32 ZoneLen;  //zone number	
	void*  pBankheader;
	
	ME_U32 BankDataLen;  //byte
	void*  pBankData;

	ME_U32 BankDataType; //0: 16bit pcm, 1:  4bit ima-adpcm

	ME_U32  MappingLen;//mapping number
	void* pAdpcmMapping;

	void  *SeekInfo;
}VM_MIDI_INPUT,*PVM_MIDI_INPUT; 	

typedef struct
{//file operate
	void *stream;	//file point
	ME_S32 (*me_read)(void *file, char* buffer, int readbytes);
	ME_S32 (*me_write)(void *file, char* buffer, int writebytes);
	ME_S32 (*me_seek)(void *file, int i_offset, int mode);
	ME_S32 (*me_tell)(void *file);

	ME_S32		(*read_block_func)(void *file, char* buffer, ME_S32 readbytes, ME_U32 reg, ME_U32 bit);
	ME_S32		(*write_block_func)(void *file, char* buffer, ME_S32 writebytes, ME_U32 reg, ME_U32 bit);
	ME_S32		(*get_offset_sector_func)(void *file, ME_U32 *sectorsize);
	
			
	ME_S32 mid_Channels;
	ME_S32 mid_SampleRate;
	ME_S32 mid_BitsPerSample;
	ME_S32 mid_TotalTime; 			//ms
	
	ME_S64 mid_DataLength;         
	ME_U32 mid_DataStartPoint;	
	ME_U32 mid_DataStartPointBackup; //seek use
	ME_U16 mid_Format;
	ME_U16 mid_Division;
	ME_U16 mid_TrackNum;

	//midi_instrument_info_t *mid_instrument;
	ME_PU08 mid_Data;
	//ME_PU08 g_mid_Data;

	VM_MIDI_INPUT vm_input;			//	Base Band vm_input
	VM_MIDI_INPUT vm_input_core;	// Core vm_input
	ME_U08*	pvm_input;
	ME_U32	file_end;
}VM_MIDI_PARSER_INFO;

	ME_U32	parser_mid_register(void);
		
#ifdef __cplusplus
	}
#endif
	
#endif
	
	
