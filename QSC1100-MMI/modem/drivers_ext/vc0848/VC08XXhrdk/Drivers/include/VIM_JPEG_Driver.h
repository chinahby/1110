/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]          : VIM_JPEG_Driver.h
 * [Description]        : 适用于578b.和jpeg相关的头文件  
 *				
 * [Author]                 : angela	
 * [Date Of Creation]  : 2007-03-19 16:01
 * [Platform]               : 平台相关
 * [Note]                 : 
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        			Author     		Modifications
 * ------------------------------------------------------------------------------
 * 2007-03-24 		angela      		 Created
 *******************************************************************************/
/*******************************************************************************
 * This source code has been made available to you by VIMICRO on an
 * AS-IS basis. Anyone receiving this source code is licensed under VIMICRO
 * copyrights to use it in any way he or she deems fit, including copying it,
 * modifying it, compiling it, and redistributing it either with or without
 * modifications. Any person who transfers this source code or any derivative
 * work must include the VIMICRO copyright notice and this paragraph in
 * the transferred software.
 *******************************************************************************/
#include "VIM_COMMON.h"

#ifndef _VIM_5XRDK_JPEG_H_
#define _VIM_5XRDK_JPEG_H_

#ifdef __cplusplus
extern "C" {
#endif


#define VIM_MAX_JPEGSIZE_W	4096
#define VIM_MAX_JPEGSIZE_H		4096

#define GETBYTE(base,offset) ((UINT8)(*((UINT8 *)base+offset)))
#define GETWORD(base,offset) (((UINT16)(*((UINT8 *)base+offset))<< 8) + (UINT16)*((UINT8 *)base+offset+1))
#define GETWORD_INV(base,offset) (((UINT16)(*((UINT8 *)base+offset+1))<< 8) + (UINT16)*((UINT8 *)base+offset))
#define GETDWORD(base,offset) (((UINT32)(*((UINT8 *)base+offset))<< 24) + ((UINT32)(*((UINT8 *)base+offset+1))<<16) + 	\
								((UINT32)(*((UINT8 *)base+offset+2))<< 8) + ((UINT32)(*((UINT8 *)base+offset+3))))
#define GETDWORD_INV(base,offset) (((UINT32)(*((UINT8 *)base+offset+3))<< 24) + ((UINT32)(*((UINT8 *)base+offset+2))<<16) + 	\
								((UINT32)(*((UINT8 *)base+offset+1))<< 8) + ((UINT32)(*((UINT8 *)base+offset))))

#define SETBYTE(base, offset, val)	\
		{												\
			(*((UINT8 *)base+offset)) = (UINT8)val;		\
			offset++;									\
		}

#define SETWORD(base, offset, val)		\
		{								\
			(*((UINT8 *)base+offset)) = (UINT8)((((UINT16)val)>> 8)&0xff);	\
			(*((UINT8 *)base+offset+1)) = (UINT8)val;						\
			offset += 2;													\
		}

#define SETWORD_INV(base, offset, val)		\
		{								\
			(*((UINT8 *)base+offset+1)) = (UINT8)((((UINT16)val)>> 8)&0xff);	\
			(*((UINT8 *)base+offset)) = (UINT8)val;						\
			offset += 2;												\
		}

#define SETDWORD(base, offset, val)		\
		{								\
			(*((UINT8 *)base+offset)) = (UINT8)((((UINT32)val)>> 24)&0xff);	\
			(*((UINT8 *)base+offset+1)) = (UINT8)((((UINT32)val)>> 16)&0xff);	\
			(*((UINT8 *)base+offset+2)) = (UINT8)((((UINT32)val)>> 8)&0xff);	\
			(*((UINT8 *)base+offset+3)) = (UINT8)val;						\
			offset += 4;													\
		}

#define SETDWORD_INV(base, offset, val)		\
		{								\
			(*((UINT8 *)base+offset+3)) = (UINT8)((((UINT32)val)>> 24)&0xff);	\
			(*((UINT8 *)base+offset+2)) = (UINT8)((((UINT32)val)>> 16)&0xff);	\
			(*((UINT8 *)base+offset+1)) = (UINT8)((((UINT32)val)>> 8)&0xff);	\
			(*((UINT8 *)base+offset)) = (UINT8)val;						\
			offset += 4;												\
		}


typedef enum _VIM_JPEG_MODE
{
	VIM_JPEG_MODE_PREVIEW=0,
	VIM_JPEG_MODE_CAPTURE_MJPEG,
	VIM_JPEG_MODE_DISPLAY_JPEG,
	VIM_JPEG_MODE_DISPLAY_YUV,
	VIM_JPEG_MODE_DISPLAY_VIDEO,
	VIM_JPEG_MODE_DECODE_JPEGTOYUV,
	VIM_JPEG_MODE_DECODE_JPEGTORGB,
#if(BOARD_TYPE==VC0820_FPGA)
	VIM_JPEG_MODE_DECODE_YUV,
#endif
	VIM_JPEG_MODE_ENCODE,
	VIM_JPEG_MODE_CAPTURE_MPG4,
	VIM_JPEG_MODE_DISPLAY_MPG4,
	VIM_JPEG_MODE_VIDEO_MPG4,
	VIM_JPEG_MODE_RESERVED=0x7FFFFFFF
}VIM_JPEG_MODE;

typedef enum _VIM_JPEG_YUVMODE	//this enum must same as lbuf format order
{
	JPEG_422 = 0,
	JPEG_420,
	JPEG_411,
	JPEG_400,
	JPEG_444,
	JPEG_UNKNOWN,
	JPEG_RESERVED=0x7FFFFFFF
}VIM_JPEG_YUVMODE;
typedef enum _VIM_JPEG_YUVTYPE	//this enum must same as lbuf format order
{
	VIM_JPEG_Y = 0,
	VIM_JPEG_CB,
	VIM_JPEG_CR,
	VIM_JPEG_UNKOWN=0x7FFFFFFF
}VIM_JPEG_YUVTYPE;
enum THUMBTYPEenum
{
	NO_THUMBNAIL	=	0,
	THUMBNAIL_JFIF	=	1,
	THUMBNAIL_JFXX	=	2,
	THUMBNAIL_EXIF	=	3,
	THUMBNAIL_UNKOWN=0x7FFFFFFF
};

typedef struct tag_TThumbNail
{
	UINT8		thumbtype;
	UINT8		thumbfmt;
	UINT32		thumbWidth;
	UINT32		thumbHeight;
	UINT32		thumblen;
	UINT8*		pthumbbuf;
	UINT8*		ppalette;

	UINT8		byteOrder;

} TThumbNail, *PTThumbNail;

typedef struct tag_TExifIFD1
{	
	UINT32		thumbWidth;					//for uncompressed thumbnail 	
	UINT32		thumbHeight;
	UINT16		compression;				//when thumbnails use JPEG compression, this tag 6 = JPEG compression 1 = uncompressed 	 	
	UINT16		PhotometricInterpretation;	//The pixel composition. 2 = RGB 6 = YCbCr	
	UINT16		YCbCrSubSampling;			//The sampling ratio of chrominance components in relation to the luminance component.
											//[2, 1] = YCbCr4:2:2 [2, 2] = YCbCr4:2:0 
	UINT32		StripOffsets;				//For each strip, the byte offset of that strip. When image format is no compression, this value shows offset to image data
											// = StripsPerImage (when PlanarConfiguration = 1)
											// = SamplesPerPixel * StripsPerImage (when PlanarConfiguration = 2)		
	UINT32		StripByteCounts;			//The total number of bytes in each strip.
											// = StripsPerImage (when PlanarConfiguration = 1)
											// = SamplesPerPixel * StripsPerImage (when PlanarConfiguration = 2)
											//These members are used for compressed thumbnail
	UINT32		JPEGInterchangeFormat;		//The offset to the start byte (SOI) of JPEG compressed thumbnail data. This is not used for primary image JPEG data		
	UINT32		JPEGInterchangeFormatLength;//The number of bytes of JPEG compressed thumbnail data. This is not used for primary image JPEG data.
	
	UINT16		PlanarConfiguration;		//Indicates whether pixel components are recorded in chunky or planar format.
											//If this field does not exist, the TIFF default of 1 (chunky) is assumed.
											//1 = chunky format 2 = planar format		


} TExifIFD1, *PTExifIFD1;


typedef struct tag_VIM_JPEG_Index
{
	UINT8	YUVType; 		//the format of jpeg file,such as YUV422\YUV420\YUV411\YUV400
	UINT8	HTCount;		//the number of Huffman table
	UINT8	QTCount;		//the number of Quantization table
	UINT8	data_precision;	//Specifies the precision in bits for the samples of the components in the frame.
	UINT8	CompCount;		//the number of component
	UINT8	qf;				//Jpeg Q factor

	UINT8	HTLen[4];		//the length of every Huffman table
	UINT8	QTPrecision[4];	//the precision of quantization table 

	UINT16	frmcnt;			//frame count
	UINT16	blkcount;		//the number of block
	UINT16	Comp[4];		//the component 
	
	UINT32	offset;			//the offset from jpeg file begin to 
							//the postion of image data begin
	UINT32	vwc;			//video word count
	UINT32	sop;			//the position of jpeg file begin(begin with FFD8)
	UINT32	eop;			//the position of jpeg file end(end with FFD9)
	BOOL 	frmEnd;			//frame end flag, use for add 0xffd9

	TSize	ImageSize; 		//real image size
	TSize	JpgSize;			//image size after compensation,if the image format is YUV422,
							//then,JpgSize.cx must be multiple of 16,and JpgSize.cy must be multiple of 8.

	UINT8	*HT[4]; 			//the point of Huffman table
	UINT8	*QT[4]; 			//the point of Quantization table

	TThumbNail	thumbinfo;
	
} VIM_JPEG_Index, *PVIM_JPEG_Index;
enum IMAGESTREAMenum
{
	YUV422_UYVY,
	YUV422_VYUY,
	YUV422_YUYV,
	YUV422_YVYU,
	YUV422_YUVY,

	YUV422_YVUY,
	YUV422_UVYY,
	YUV422_VUYY,
	YUV422_YYVU,
	YUV422_YYUV,

	RAW_BG,
	RAW_GB,
	RAW_RG,
	RAW_GR,

	RAW_RGB,
	RAW_RBG,
	RAW_GBR,
	RAW_GRB,
	RAW_BGR,
	RAW_BRG,

	YUV420_UYVY,
	YUV420_VYUY,
	YUV420_YUYV,
	YUV420_YVYU,
	YUV420_YUVY,

	YUV420_YVUY,
	YUV420_UVYY,
	YUV420_VUYY,
	YUV420_YYVU,
	YUV420_YYUV,

	YUV411_YYYYUV,
	YUV411_YYYYVU,
	YUV411_YUYVYY,
	YUV411_YVYUYY,
	YUV411_UVYYYY,
	YUV411_VUYYYY,

	YUV400,
	YUV444,

	RGB1,
	RGB2,
	RGB4,
	RGB8,
	//RGB444,
	//RGB555,
	//RGB565,
	//RGB666,
	RGB24,
	RGB32,

	YUV422M2BLK,
	YUV422BLK,
	YUV420BLK,
	YUV411BLK,
	YUV400BLK,
	YUV444BLK,
	YUV444M2BLK,

	YUV422LBUF,
	YUV420LBUF,
	YUV411LBUF,
	YUV400LBUF,
	YUV444LBUF,

	BMP,
	JPEG422,
	JPEG420,
	JPEG411,
	JPEG400,
	JPEG444,

	RAW_R,
	RAW_G,
	RAW_B,

	RAW_Y,
	RAW_U,
	RAW_V,

	YUV422FRM,
	YUV420FRM,
	YUV411FRM,
	YUV400FRM,
	YUV444FRM,

	MPEG1I_FRM,
	MPEG2IFRM420,
	MPEG2IFRM422,
	MPEG2IFRM444,
	MPEG4I_FRM,

	EDGE_MAP,

	YIQ,
	HSI,
	RAW_YIQ_Y,
	RAW_YIQ_I,
	RAW_YIQ_Q,

	RAW_HSI_I,
	RAW_HSI_H,
	RAW_HSI_S,

	GIF,
	PNG,
	PCX,
	CCIR656,
	CCIR601,

	YCbCr422_YYCbCr,
	YCbCr422_YpCbpCrp,
	YCbCr420_YYYYCbCr,
	YCbCr420_YpCbpCrp,
	RAW_RGB_RpGpBp
};

enum JPEG_MARKERenum {			
/* JPEG marker codes */
  M_SOF0   = 0xc0,
  M_SOF1   = 0xc1,
  M_SOF2   = 0xc2,
  M_SOF3   = 0xc3,

  M_SOF5   = 0xc5,
  M_SOF6   = 0xc6,
  M_SOF7   = 0xc7,

  M_JPG    = 0xc8,
  M_SOF9   = 0xc9,
  M_SOF10  = 0xca,
  M_SOF11  = 0xcb,

  M_SOF13  = 0xcd,
  M_SOF14  = 0xce,
  M_SOF15  = 0xcf,

  M_DHT    = 0xc4,

  M_DAC    = 0xcc,

  M_RST0   = 0xd0,
  M_RST1   = 0xd1,
  M_RST2   = 0xd2,
  M_RST3   = 0xd3,
  M_RST4   = 0xd4,
  M_RST5   = 0xd5,
  M_RST6   = 0xd6,
  M_RST7   = 0xd7,

  M_SOI    = 0xd8,
  M_EOI    = 0xd9,
  M_SOS    = 0xda,
  M_DQT    = 0xdb,
  M_DNL    = 0xdc,
  M_DRI    = 0xdd,
  M_DHP    = 0xde,
  M_EXP    = 0xdf,

  M_APP0   = 0xe0,
  M_APP1   = 0xe1,
  M_APP2   = 0xe2,
  M_APP3   = 0xe3,
  M_APP4   = 0xe4,
  M_APP5   = 0xe5,
  M_APP6   = 0xe6,
  M_APP7   = 0xe7,
  M_APP8   = 0xe8,
  M_APP9   = 0xe9,
  M_APP10  = 0xea,
  M_APP11  = 0xeb,
  M_APP12  = 0xec,
  M_APP13  = 0xed,
  M_APP14  = 0xee,
  M_APP15  = 0xef,

  M_JPG0   = 0xf0,
  M_JPG13  = 0xfd,
  M_COM    = 0xfe
};
enum EXIFTAGenum {
/* EXIF TAG codes */
	EXIF_TAG_INTEROPERABILITY_INDEX		= 0x0001,
	EXIF_TAG_INTEROPERABILITY_VERSION	= 0x0002,
	EXIF_TAG_IMAGE_WIDTH 			= 0x0100,
	EXIF_TAG_IMAGE_LENGTH 			= 0x0101,
	EXIF_TAG_BITS_PER_SAMPLE 		= 0x0102,
	EXIF_TAG_COMPRESSION 			= 0x0103,
	EXIF_TAG_PHOTOMETRIC_INTERPRETATION 	= 0x0106,
	EXIF_TAG_FILL_ORDER 			= 0x010a,
	EXIF_TAG_DOCUMENT_NAME 			= 0x010d,
	EXIF_TAG_IMAGE_DESCRIPTION 		= 0x010e,
	EXIF_TAG_MAKE 				= 0x010f,
	EXIF_TAG_MODEL 				= 0x0110,
	EXIF_TAG_STRIP_OFFSETS 			= 0x0111,
	EXIF_TAG_ORIENTATION 			= 0x0112,
	EXIF_TAG_SAMPLES_PER_PIXEL 		= 0x0115,
	EXIF_TAG_ROWS_PER_STRIP 		= 0x0116,
	EXIF_TAG_STRIP_BYTE_COUNTS		= 0x0117,
	EXIF_TAG_X_RESOLUTION 			= 0x011a,
	EXIF_TAG_Y_RESOLUTION 			= 0x011b,
	EXIF_TAG_PLANAR_CONFIGURATION 		= 0x011c,
	EXIF_TAG_RESOLUTION_UNIT 		= 0x0128,
	EXIF_TAG_TRANSFER_FUNCTION 		= 0x012d,
	EXIF_TAG_SOFTWARE 			= 0x0131,
	EXIF_TAG_DATE_TIME			= 0x0132,
	EXIF_TAG_ARTIST				= 0x013b,
	EXIF_TAG_WHITE_POINT			= 0x013e,
	EXIF_TAG_PRIMARY_CHROMATICITIES		= 0x013f,
	EXIF_TAG_TRANSFER_RANGE			= 0x0156,
	EXIF_TAG_JPEG_PROC			= 0x0200,
	EXIF_TAG_JPEG_INTERCHANGE_FORMAT	= 0x0201,
	EXIF_TAG_JPEG_INTERCHANGE_FORMAT_LENGTH	= 0x0202,
	EXIF_TAG_YCBCR_COEFFICIENTS		= 0x0211,
	EXIF_TAG_YCBCR_SUB_SAMPLING		= 0x0212,
	EXIF_TAG_YCBCR_POSITIONING		= 0x0213,
	EXIF_TAG_REFERENCE_BLACK_WHITE		= 0x0214,
	EXIF_TAG_RELATED_IMAGE_FILE_FORMAT	= 0x1000,
	EXIF_TAG_RELATED_IMAGE_WIDTH		= 0x1001,
	EXIF_TAG_RELATED_IMAGE_LENGTH		= 0x1002,
	EXIF_TAG_CFA_REPEAT_PATTERN_DIM		= 0x828d,
	EXIF_TAG_CFA_PATTERN			= 0x828e,
	EXIF_TAG_BATTERY_LEVEL			= 0x828f,
	EXIF_TAG_COPYRIGHT			= 0x8298,
	EXIF_TAG_EXPOSURE_TIME			= 0x829a,
	EXIF_TAG_FNUMBER			= 0x829d,
	EXIF_TAG_IPTC_NAA			= 0x83bb,
	EXIF_TAG_EXIF_IFD_POINTER		= 0x8769,
	EXIF_TAG_INTER_COLOR_PROFILE		= 0x8773,
	EXIF_TAG_EXPOSURE_PROGRAM		= 0x8822,
	EXIF_TAG_SPECTRAL_SENSITIVITY		= 0x8824,
	EXIF_TAG_GPS_INFO_IFD_POINTER		= 0x8825,
	EXIF_TAG_ISO_SPEED_RATINGS		= 0x8827,
	EXIF_TAG_OECF				= 0x8828,
	EXIF_TAG_EXIF_VERSION			= 0x9000,
	EXIF_TAG_DATE_TIME_ORIGINAL		= 0x9003,
	EXIF_TAG_DATE_TIME_DIGITIZED		= 0x9004,
	EXIF_TAG_COMPONENTS_CONFIGURATION	= 0x9101,
	EXIF_TAG_COMPRESSED_BITS_PER_PIXEL	= 0x9102,
	EXIF_TAG_SHUTTER_SPEED_VALUE		= 0x9201,
	EXIF_TAG_APERTURE_VALUE			= 0x9202,
	EXIF_TAG_BRIGHTNESS_VALUE		= 0x9203,
	EXIF_TAG_EXPOSURE_BIAS_VALUE		= 0x9204,
	EXIF_TAG_MAX_APERTURE_VALUE		= 0x9205,
	EXIF_TAG_SUBJECT_DISTANCE		= 0x9206,
	EXIF_TAG_METERING_MODE			= 0x9207,
	EXIF_TAG_LIGHT_SOURCE			= 0x9208,
	EXIF_TAG_FLASH				= 0x9209,
	EXIF_TAG_FOCAL_LENGTH			= 0x920a,
	EXIF_TAG_SUBJECT_AREA			= 0x9214,
	EXIF_TAG_MAKER_NOTE			= 0x927c,
	EXIF_TAG_USER_COMMENT			= 0x9286,
	EXIF_TAG_SUB_SEC_TIME			= 0x9290,
	EXIF_TAG_SUB_SEC_TIME_ORIGINAL		= 0x9291,
	EXIF_TAG_SUB_SEC_TIME_DIGITIZED		= 0x9292,
	EXIF_TAG_FLASH_PIX_VERSION		= 0xa000,
	EXIF_TAG_COLOR_SPACE			= 0xa001,
	EXIF_TAG_PIXEL_X_DIMENSION		= 0xa002,
	EXIF_TAG_PIXEL_Y_DIMENSION		= 0xa003,
	EXIF_TAG_RELATED_SOUND_FILE		= 0xa004,
	EXIF_TAG_INTEROPERABILITY_IFD_POINTER	= 0xa005,
	EXIF_TAG_FLASH_ENERGY			= 0xa20b,
	EXIF_TAG_SPATIAL_FREQUENCY_RESPONSE	= 0xa20c,
	EXIF_TAG_FOCAL_PLANE_X_RESOLUTION	= 0xa20e,
	EXIF_TAG_FOCAL_PLANE_Y_RESOLUTION	= 0xa20f,
	EXIF_TAG_FOCAL_PLANE_RESOLUTION_UNIT	= 0xa210,
	EXIF_TAG_SUBJECT_LOCATION		= 0xa214,
	EXIF_TAG_EXPOSURE_INDEX			= 0xa215,
	EXIF_TAG_SENSING_METHOD			= 0xa217,
	EXIF_TAG_FILE_SOURCE			= 0xa300,
	EXIF_TAG_SCENE_TYPE			= 0xa301,
	EXIF_TAG_NEW_CFA_PATTERN		= 0xa302,
	EXIF_TAG_CUSTOM_RENDERED		= 0xa401,
	EXIF_TAG_EXPOSURE_MODE			= 0xa402,
	EXIF_TAG_WHITE_BALANCE			= 0xa403,
	EXIF_TAG_DIGITAL_ZOOM_RATIO		= 0xa404,
	EXIF_TAG_FOCAL_LENGTH_IN_35MM_FILM	= 0xa405,
	EXIF_TAG_SCENE_CAPTURE_TYPE		= 0xa406,
	EXIF_TAG_GAIN_CONTROL			= 0xa407,
	EXIF_TAG_CONTRAST			= 0xa408,
	EXIF_TAG_SATURATION			= 0xa409,
	EXIF_TAG_SHARPNESS			= 0xa40a,
	EXIF_TAG_DEVICE_SETTING_DESCRIPTION	= 0xa40b,
	EXIF_TAG_SUBJECT_DISTANCE_RANGE		= 0xa40c,
	EXIF_TAG_IMAGE_UNIQUE_ID		= 0xa420
};

enum EXIFFORMATenum {
/* EXIF Format Type */
	EXIF_FORMAT_BYTE       =  1,
	EXIF_FORMAT_ASCII      =  2,
	EXIF_FORMAT_SHORT      =  3,
	EXIF_FORMAT_LONG       =  4,
	EXIF_FORMAT_RATIONAL   =  5,
	EXIF_FORMAT_SBYTE      =  6,
	EXIF_FORMAT_UNDEFINED  =  7,
	EXIF_FORMAT_SSHORT     =  8,
	EXIF_FORMAT_SLONG      =  9,
	EXIF_FORMAT_SRATIONAL  = 10,
	EXIF_FORMAT_FLOAT      = 11,
	EXIF_FORMAT_DOUBLE     = 12
};

enum EXIFBYTEORDER {
	EXIF_BYTE_ORDER_MOTOROLA = 0x1,
	EXIF_BYTE_ORDER_INTEL = 0x2
};


enum THUMBNAIL_FORMATenum {
	THUMB_FORMAT_JPEG		=	0,
	THUMB_FORMAT_RGB8		=	1,
	THUMB_FORMAT_RGB24		=	2,
	THUMB_FORMAT_YCBCR		=	3
};
typedef enum _VIM_JPEG_INT_TYPE
{
	VIM_INT_JPEG_ENC_DONE		= 0,
	VIM_INT_JPEG_DEC_DONE		= 1,
	VIM_INT_JPEG_DEC_ERR		= 2,
//VIM_INT_JPEG_LINBUF_READ	=3,
	VIM_JPEG_JPEG_INT_ALL		= VIM_JPEG_JPEG_INT_NUM
}VIM_JPEG_INT_TYPE;
typedef enum _VIM_JPEG_INTFLAG
{
	VIM_JPEG_ENCODE_DONE = 0x01,
	VIM_JPEG_DECODE_DONE = 0x02,
	VIM_JPEG_DECODE_ERROR  = 0x04,
	VIM_JPEG_HEADER_DONE   = 0x08,
	VIM_JPEG_ENCODE_ERROR  = 0x10
}VIM_JPEG_INTFLAG;

#if (BOARD_TYPE==VC0820_FPGA)//add by zhouj
typedef enum _VIM_LBUF_INT_TYPE
{
	VIM_INT_LBUF_READ_INT		= 0,
	VIM_INT_LBUF_FRAME_START	= 1,
	VIM_INT_LBUF_INT_ALL		= VIM_JPEG_LBUF_INT_NUM
}VIM_LBUF_INT_TYPE;
#endif
void VIM_JPEG_ToolParseExif(HUGE UINT8* ptr, PTThumbNail pthumbinfo);

VIM_JPEG_YUVMODE VIM_JPEG_GetYUVMode(void);

VIM_RESULT VIM_JPEG_SetSize(VIM_JPEG_YUVMODE bYUVmode,TSize ImageSize);
void VIM_JPEG_SetBitRateControlEn(BOOL bEnable);

UINT8 VIM_JPEG_CheckDecodeOK(void);
UINT16 VIM_JPEG_GetFrameCount(void);
UINT16 VIM_JPEG_GetWidth(void);
UINT32 VIM_JPEG_GetYuvSize(VIM_JPEG_YUVMODE yuvfmt, TSize size);
UINT32 VIM_JPEG_GetVideoWordCount(void);
void VIM_JPEG_SetBrcQF(UINT8 byQFactor);
UINT8 VIM_JPEG_GetQFactor(void);

void VIM_JPEG_ResetState(void);
UINT8 VIM_JPEG_GetStatus(void);
void VIM_JPEG_SetMode(VIM_JPEG_MODE bMode);

void VIM_JPEG_SetUseCustomQTEnable(BOOL bEnable);
void VIM_JPEG_SetUseSameQTEnable(BOOL bEnable);
void VIM_JPEG_FeedHeaderEnable(BOOL bEnable);
void VIM_JPEG_SetTargetWordCount(UINT32 dwCount);
UINT32 VIM_JPEG_GetTargetWordCount(void);
void VIM_JPEG_SetTargetCompressRatio(UINT8 byRatio);
void VIM_JPEG_SetDefaultHT(void);
void VIM_JPEG_SetJpegInfo(PVIM_JPEG_Index pinfo);
void VIM_JPEG_SetLbufLcdRGB565(void);

UINT16  VIM_JPEG_LineGetWidth(void);
void  VIM_JPEG_GetSize(PTSize Size);
void VIM_JPEG_GetJpegSize(VIM_JPEG_YUVMODE fmt, TSize size, TSize *size1);
void VIM_JPEG_AdjustClkBlank(void);
void VIM_JPEG_StartDecode(void);
void VIM_JPEG_ToolJpegParse(HUGE UINT8 *ptr, UINT32 length, PVIM_JPEG_Index pinfo,UINT8 FindEnd);

void VIM_JPEG_RestartDecode(void);
 void _ISR_JpegLbufIntHandle(UINT8 wIntFlag);
 void _ISR_JpegJpegIntHandle(UINT8 wIntFlag);
void VIM_JPEG_RegisterISR(VIM_JPEG_INT_TYPE bySecondLevelInt, PVIM_Isr  Isr);
void VIM_JPEG_SetIntEnable(VIM_JPEG_INT_TYPE bySecondLevelInt,BOOL Eanble);
void VIM_JPEG_Toolyuv422_Rgb565(HUGE UINT8 *src, HUGE UINT8 *dest, UINT16 width, UINT16 height);
void VIM_JPEG_StartEncode(BOOL Enable);
void VIM_JPEG_WriteYUVData(UINT8 byData);
UINT8 VIM_JPEG_GetPixRate(void);
UINT16 VIM_JPEG_GetHbanlk(UINT8 pixelrate);
void VIM_JPEG_SetPixelRate(UINT8 bPixelrate);
void VIM_JPEG_SetLineBlank(UINT16 bHblank);
#if(BOARD_TYPE==VC0820_FPGA)
void VIM_JPEG_SetMotionJpegInfo(PVIM_JPEG_Index pinfo);
void VIM_JPEG_SetLbuf1Line(VIM_JPEG_YUVMODE mode);
void VIM_JPEG_StartEncodeYUV(BOOL Enable);
void VIM_LBUF_RegisterISR(VIM_LBUF_INT_TYPE bySecondLevelInt, PVIM_Isr Isr);
void VIM_LBUF_SetIntEnable(VIM_LBUF_INT_TYPE bySecondLevelInt,BOOL Eanble);
#endif


#ifdef __cplusplus
}
#endif

#endif /* _RDK_HIF_H_ */
