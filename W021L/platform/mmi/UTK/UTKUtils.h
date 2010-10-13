#ifndef _UTKUTILS_H
#define _UTKUTILS_H
/*==============================================================================
// 文件：
//        UTKUtils.h
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//        
// 文件说明：
//        
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间       修 改 人     修改内容、位置及原因
-----------      ----------    -----------------------------------------------
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "AEE.h"
#include "AEEStdLib.h"
#ifndef WIN32
#include "uim.h"
#else
#define UIM_K_FIELD          0x0F      /* In T0; Number of historical bytes */
#define UIM_Y_FIELD_SHIFT       4      /* Number of bits to shift to get the
                                           Y field in the lsb                */
#define UIM_TD_PRESENT       0x80      /* TD byte is present in following
                                           bytes                             */
#define UIM_T_FIELD          0x0F      /* In TD; transmission protocol      */

#define UIM_DEFLT_GUARDTIME_BITS 2     /* This is the default number of
                                           bits used for the guardtime.      */
#define UIM_MINIMUM_T_1_GUARDTIME 1    /* This is the minimum guardtime for
                                          the T=1 protocol */

#define UIM_MAX_CHV_DIGITS      8      /* This is the maximum number of digits */
/* Maximum number of chars in one transaction */
#define UIM_MAX_CHARS         512
#define UIM_MAX_TRANSACTION_SIZE 256
#define UIM_MAX_STREAM_ISO7816_APDU_TRANSACTION_SIZE 261
#define UIM_MAX_TRANS_SIZE_OUT   255
#define UIM_MAX_TRANSACTION_P3   0
#define UIM_MAX_PSO_DATA      256
#define NUM_BYTES_APDU_HDR    5
#endif
/*==============================================================================
                                 宏定义和常数
==============================================================================*/
#define  REFRESHMASK_UIMALL         0x0001
#define  REFRESHMASK_UIMSMS         0x0002
#define  REFRESHMASK_UIMADD         0x0004
#define  REFRESHMASK_UIMRESET       0x0008

/*==============================================================================
                                 类型定义
==============================================================================*/
typedef struct _command_describe 
{
    byte    describe_id;
    byte    length;
    byte    command_num;
    byte    command_type;
    byte    command_restricttag;
} command_describe;

typedef struct _device_identity 
{
    byte    speciality_id;
    byte    length;
    byte    source_speciality;
    byte    target_speciality;
} device_identity;

typedef struct _text_string 
{
    byte    text_id;
    byte    length;
    byte    encode_schedule; // 4:oect 0:ASCII 7位编码 8: Unicode 编码
    AECHAR  data[256];
} text_string;

/* Response Length - parsed Value field */
typedef struct _response_len
{
    byte   response_id;
    byte   length;  
    byte   min_len_rsp;
    byte   max_len_rsp;
} response_len;

/* Default Text - parsed Value field */
typedef struct _default_text
{
    byte    text_id;
    byte    length;
    byte    encode_schedule; // 4:oect 0:ASCII 7??±à?? 8: Unicode ±à??
    AECHAR  data[256];
} default_text;

/* Icon Identifier - parsed Value field */
typedef struct _icon_id
{
   byte    icon_id;
   byte    length;    
   byte    qual;
   byte    id;
} icon_id;

typedef struct _alpha_id
{
   byte   alpha_id_id;
   byte   length;
  AECHAR  data[256];
} alpha_id;

typedef struct _tone
{
   byte   tone_id;
   byte   length;
   byte   tone;
} tone;

typedef struct _duration
{
   byte   duration_id;
   byte   length;
   byte   time_unit;
   byte   time_interval;   
} duration;

/* File List - parsed Value field */
typedef struct _file_list
{
   byte   file_list_id;
   byte   length;
   byte   num_files;
   byte   data[UIM_MAX_CHARS];
} file_list;

/* Language - parsed Value field */
typedef struct _lang 
{
  byte   lang_id;
  word   lang;
} lang;

typedef struct _date_time
{
   word  year;
   word  month;
   word  day;
   word  hour;
   word  minute;
   word  second;
   word  zone;
}date_time;

typedef struct _locn_info
{
   // based on CCAT 3GPP2 C.S0035
   word   mcc;
   word   imsi_11_12;
   word   sid;
   word   nid;
   word   base_id; 
   dword  base_lat;
   dword  base_long;
}locn_info;
typedef struct _Display_text 
{
    byte    command_tag;
    byte    length;
    command_describe    cmd_describe;
    device_identity     dev_identity;
    text_string         text;
} Display_text;

typedef struct _Setup_Call 
{
    command_describe    cmd_describe;
    AECHAR              wszAlpha[64];           // 用户证实阶段
    AECHAR              wszAlphaSetupCall[64];  // 呼叫建立阶段
    AECHAR              wszDialNum[32];
} Setup_Call;

typedef struct _Get_input
{
    byte                command_tag;
    byte                length;
    command_describe    cmd_describe;
    device_identity     dev_identity;
    text_string         text;
    response_len        response;
    default_text        defaulttext;
    icon_id             icon;    
} Get_input;

typedef struct _Local_Information
{
    byte                command_tag;
    byte                length;
    command_describe    cmd_describe;
    device_identity     dev_identity;
    byte                command_rsp_tag;
    lang                lang;    
    date_time           date_time;
    byte                access_technology;
    locn_info           locn_info;
} Local_Information;

typedef struct _Play_Tone
{
    byte                command_tag;
    byte                length;
    command_describe    cmd_describe;
    device_identity     dev_identity;
    text_string         text;    
    alpha_id            alpha_id;
    tone                tone;
    duration            duration;
    icon_id             icon;        
} Play_Tone;

typedef struct FileListType 
{
    flg         isValid:1;      // 1 = this structure contains valid data
    
    byte        numFiles;      
    byte        numBytes;       // number of bytes of valid data in the fileData field
    byte        fileData[256];  // raw file data information
} FileListType;

typedef struct _RefreshCmdType {
   byte                 cmd_qualifier;
   FileListType         fileList;
} RefreshCmdType;

/*==============================================================================
                                 函数声明
==============================================================================*/
/*==============================================================================
函数: 
    UTK_parse_proactive_command
       
说明: 
    本函数只分析原语命令数据的命令描述部分，并保存原始数据到文件级全局缓冲区中。
       
参数: 
    cmd_data [in]：指向命令数据的指针。
    cmd_len [in]：命令数据长度。
       
返回值:
    原语命令
       
备注:
    
    
==============================================================================*/
byte UTK_parse_proactive_command(byte *cmd_data, byte cmd_len);

/*==============================================================================
函数: 
    UTK_GetCmddata
       
说明: 
    本函数用来获取指向与命令相关的数据的地址。
       
参数: 
    cmd_type [In]: uim_tk_proactive_cmd_enum_type 。
       
返回值:
    byte *
       
备注:
    注意：返回数据的第一字节本应为主动式RUIM 命令标签，为便于使用，在函数
    parse_UTK_proactive_command 中将其改为 proactive command 值。其实知道
    proactive command 值后，主动式RUIM 命令标签就没多大意义了。

==============================================================================*/
byte * UTK_GetCmddata(byte cmd_type);


/*==============================================================================
函数: 
    UTK_decodetext
       
说明: 
    本函数用来解码按短信编码方式编码的字节序列到 AECHAR 字符串。
       
参数: 
    wszBuf [In/out]: 用于保存解码后文本的缓冲区 。
    nBufSize [In]: 输出缓冲区大小,单位为字节 。
    pdata [In]: 指向待解码字节流 。
    nlen [In]: 待解码字节流大小,单位为字节 。
    encode_schedule [In]: 待解码字节流的编码方式 。
    
       
返回值:
    none
       
备注:
    注意这里字节流的编码方式完全与 uasms_user_data_encoding_e_type 定义的不同。
    0x00: SMS缺省压缩字母7比特的数据编码计划
    0x04: GSM缺省字母8比特数据的数据编码计划
    0x08: 16比特UNICODE字母的数据编码计划
    本函数对不能识别的编码方式，按 0x04 处理
==============================================================================*/
void UTK_decodetext(AECHAR *wszBuf, int nBufSize, byte *pdata, int nlen, byte encode_schedule);

/*==============================================================================
函数: 
    DecodeDisplayTextData
       
说明: 
    本函数用来解码相应于 Display Text 命令的原始数据字节序列到结构 Display_text 中。
       
参数: 
    pdata [In]: 相应于 Display Text 命令的原始数据字节序列(BER-TLV 结构) 。
    pText [In/out]: 指向用于保存解码后数据的 Display_text 结构缓冲区 。
    
       
返回值:
    none
       
备注:

==============================================================================*/
void DecodeDisplayTextData(byte *pdata, Display_text *pText);

/*==============================================================================
函数: 
    DecodeAlphaString
       
说明: 
    本函数解码 Alpha 域字符串。
       
参数: 
    pdata [in]：指向输入数据字节序列。
    nLen [in]：输入数据字节序列长度,单位字节。
    wstrOut [out]： 输出Buffer
    nBufSize [in]:  输出Buffer大小,单位 AECHAR
       
返回值:
    none
       
备注:
       
==============================================================================*/
void DecodeAlphaString(byte *pdata, int nLen, AECHAR *wstrOut, int nBufSize);

/*==============================================================================
函数: 
    UTK_Get_Setup_Call_Buf
       
说明: 
    本函数用来获取存储相应于 Setup Call 命令原始数据字节序列解码后数据的结构 
    Setup_Call 缓冲区地址。
       
参数: 
    none
       
返回值:
    Setup_Call 结构指针
       
备注:
    因为 Setup Call 命令解码信息会跨模块使用，故使用一个全局静态变量来保存。但
    必须通过本函数来访问。
==============================================================================*/
Setup_Call *UTK_Get_Setup_Call_Buf(void);

/*==============================================================================
函数: 
    UTK_DecodeSetupCallCmdData
       
说明: 
    本函数用来解码相应于 Setup Call 命令的原始数据字节序列到结构 Setup_Call 中。
       
参数: 
    pdata [In]: 相应于 Setup Call 命令的原始数据字节序列(BER-TLV 结构) 。
    pstCall [in/out]: 指向用于保存解码后数据的 Setup_Call 结构缓冲区 。
       
返回值:
    SUCCESS : 成功
    EFAILED : 失败
       
备注:

==============================================================================*/
int UTK_DecodeSetupCallCmdData(byte *pdata, Setup_Call *pstCall);

/*==============================================================================
函数: 
    DecodeGetInputData
       
说明: 
    本函数用来解码文本输入命令
       
参数:     
       
返回值:
    none
       
备注:
==============================================================================*/
void DecodeGetInputData(byte *pdata, Get_input *pGetInput);

/*==============================================================================
函数: 
    DecodePlayToneData
       
说明: 
    本函数用来解码音频播放命令
       
参数:  
       
返回值:
    none
       
备注:
==============================================================================*/
void DecodePlayToneData(byte *pdata, Play_Tone *pPlayTone);

/*==============================================================================
函数: 
    DecodeRefreshData
       
说明: 
    本函数用来解码相应于 Refresh 命令的原始数据字节序列到结构 RefreshCmdType 中。
       
参数: 
    pdata [In]: 相应于 Refresh 命令的原始数据字节序列(BER-TLV 结构) 。
    pCmd [In/out]: 指向用于保存解码后数据的 RefreshCmdType 结构缓冲区 。
    
       
返回值:
    none
       
备注:

==============================================================================*/
void DecodeRefreshData(byte *pdata, RefreshCmdType *pCmd);

#endif //_UTKUTILS_H
