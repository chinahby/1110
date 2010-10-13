#ifndef CONTAPP_VCARD_H
#define CONTAPP_VCARD_H

#ifdef FEATURE_SUPPORT_VCARD

/*==============================================================================
// 文件：
//        ContApp_vCard.h
//        2009-03-02 twq草拟版本(Draft Version)
//        版权所有(c) 2009 Anylook Tech CO.,LTD.
//        
// 文件说明：
//        
// 作者：2009-03-02
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时    间      修   改   人     修改内容、位置及原因
--------      -----------     -----------------------------------------------
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
//#include "contApp_priv.h"

/*!
@brief vCard specific definitions.
*/
#define     VCARD_BEGIN     ("BEGIN:VCARD")
#define     VCARD_VERSION21 ("VERSION:2.1")
#define     VCARD_VERSION30 ("VERSION:3.0")
#define     VCARD_NAME      ("N:")
#define     VCARD_FNAME     ("FN:")
#define     VCARD_TITLE     ("TITLE:")
#define     VCARD_BIRTH     ("BDAY:")
#define     VCARD_ADDRESS   ("ADR;HOME:")
#define     VCARD_ORG       ("ORG:")
#define     VCARD_URL       ("URL:")
#define     VCARD_HOME      ("TEL;HOME:")/*("TEL;HOME:VOICE")  */
#define     VCARD_OFFICE    ("TEL;WORK:")
#define     VCARD_MOBILE    ("TEL;CELL:")
#define     VCARD_FAX       ("TEL;FAX:")
#define     VCARD_EMAIL     ("EMAIL;INTERNET:")
#define     VCARD_NOTE     ("NOTE:")
#define     VCARD_TIMEZONE  ("TZ:")
#define     VCARD_REV       ("REV:")
#define     VCARD_END       ("END:VCARD")
#define     VCARD_DELIMTER  ("\r\n")
#define     VCARD_DELIMTER_EDIT ("\r\n")
#define     VCARD_STREND    ("\0")
#define     VCARD_TEL_HOME  ("HOME")
#define     VCARD_TEL_WORK  ("WORK")
#define     VCARD_TEL_CELL  ("CELL")
#define     VCARD_EMAIL_INTERNET  ("INTERNET")
#define     VCARD_ADD_HOME  ("HOME")
#define     VCARD_ADD_POSTAL  ("POSTAL")
#define     VCARD_ADD_PARCEL  ("PARCEL")
#define     VCARD_URL_BASE    ("http://")
#define     OPP_VCARD_FLAG    ("VCARD")

#define     VCARD_P_INDEX_BEGIN     ('1')
#define     VCARD_P_INDEX_END       ('2')
#define     VCARD_P_INDEX_VERSION   ('3')
#define     VCARD_P_INDEX_N         ('4')
#define     VCARD_P_INDEX_FN        ('5')
#define     VCARD_P_INDEX_TITLE     ('6')
#define     VCARD_P_INDEX_ADR       ('7')
#define     VCARD_P_INDEX_ORG       ('8')
#define     VCARD_P_INDEX_URL       ('9')
#define     VCARD_P_INDEX_TEL       ('A')
#define     VCARD_T_TEL_HOME        ('B')
#define     VCARD_T_TEL_OFFICE      ('C')
#define     VCARD_T_TEL_VOICE       ('D')
#define     VCARD_T_TEL_MOBILE      ('E')
#define     VCARD_T_TEL_FAX         ('F')
#define     VCARD_P_INDEX_EMAIL     ('G')
#define     VCARD_T_EMAIL_INTERNET  ('H')
#define     VCARD_P_INDEX_NOTE      ('I')
#define     VCARD_P_T_INVALID       ('\0')

#define VCARD_MAX_VALUE_OF_N            (128)
#define VCARD_MAX_VALUE_OF_FN           (128)
#define VCARD_MAX_VALUE_OF_TITLE        (16)/* 这里name 就是title*/     
#define VCARD_MAX_VALUE_OF_ADR          (128)
#define VCARD_MAX_VALUE_OF_ORG          (40)
#define VCARD_MAX_VALUE_OF_URL          (128)
#define VCARD_MAX_VALUE_OF_TEL          (32)
#define VCARD_MAX_VALUE_OF_EMAIL        (64)
#define VCARD_MAX_VALUE_OF_NOTE         (128)
#define VCARD_MAX_PROPERTY              (1024)
#define VCARD_MAX_FILE_SZIE             (VCARD_MAX_PROPERTY)
#define VCARD_TYPE_STACK                (2)   /*include the root layer*/
#define VCARD_TYPE_STACK_RESET          (0)
#define VCARD_BASE_URL_LEN              (7)/* http:// */
#define VCARD_SPECIALCHAR_PROPERTY_END      ('\r')
#define VCARD_SPECIALCHAR_PROPERTY_END2     ('\n')
#define VCARD_SPECIALCHAR_GROUP             ('.')
#define VCARD_SPECIALCHAR_TYPE              (';')
#define VCARD_SPECIALCHAR_PROPERTY          (':')
#define VCARD_SPECIALCHAR_VALUE             VCARD_SPECIALCHAR_PROPERTY_END
#define VCARD_SPECIALCHAR_IGNORE            (' ')
#define VCARD_CR                            ('\r')
#define VCARD_LF                            ('\n')
#define VCARD_NUL                           ('\0')

typedef struct vcardentry_t
{
    char* entry;
    char property_id;
}vcardentry_t;

/*
    Simple VCard record consists of entries, each entry has one line, which includes ID and its value.
    1.1. ID is ended up with ':'
    1.2. value is endep up with '\r';
    1.3. the content before ':' is used as ID field, the space should be discarded.
    1.4. the content between the first ':' and '\r' is used as ID's value.
    1.5. the ID may has extended meaning depended. which is delimited by ';' in ID field.
    1.6. both of them are case sensitive.
    1.7. unsupported group property.
*/
typedef struct property_id_map
{
    char* pid;
    char p_index;
    int length;
}property_id_map;

typedef struct tel_type_map
{
    char* tid;
    char t_index;
    int length;
}tel_type_map;

typedef struct email_type_map
{
    char* mid;
    char m_index;
    int length;
}email_type_map;


typedef enum _Vcard_last_error
{
    VCARD_BEGIN_DUMMY=0,
    VCARD_FAIL,
    VCARD_SUCCESS,
    VCARD_SUCCESS_GROUP,
    VCARD_SUCCESS_PROPERTY,
    VCARD_SUCCESS_TYPE,
    VCARD_SUCCESS_VALUE,
    VCARD_UNSUPPORTED_FEATURES,
    VCARD_UNSUPPORTED_GROUP,
    VCARD_UNSUPPORTED_PROPERTY,
    VCARD_UNSUPPORTED_TYPE,
    VCARD_ERROR_FORMAT,
    VCARD_PROPERTY_TOO_LONG,
    VCARD_PROPERTY_WRONG,
    VCARD_SAVE_RECORD_FAID,
    VCARD_END_DUMMY
}vcard_last_error;

typedef enum _Address_type
{
    ADD_FIRST=0,
    POST_OFFICE_BOX,
    EXTENDED_ADDRESS,
    STREET_ADDRESS,
    LOCALITY,
    REGION,
    POSTAL_CODE,
    COUNTRY_NAME,
    ADD_LAST
}address_type;

typedef enum _Voice_type
{
    VOICE_FIRST=0,
    VOICE_HOME,
    VOICE_WORK,
    VOICE_CELL,
    VOICE_LAST
}voice_type;

typedef struct add_type_map
{
    char* value;
    address_type type;
}add_type_map;

//unsigned int VCardGetEntryNumber(void);
vcard_last_error VCardInsertEntry(char* value,char property_id);
vcard_last_error VCardDeleteEntry(char property_id);
vcard_last_error VCardDeleteAllEntry(void);
vcard_last_error VCardEncode(void);
//vcard_last_error VCardDecode(void);
//void SetVCardRecord(char rec);
//char** GetVCardRecord(void);
//boolean VCardGenAtiCommand(unsigned int index,char** cmd);
//boolean VCardGenAtuCommand(boolean* complete,char* current,char** cmd,char** next);

vcard_last_error ParseVcfFile(CContApp *pMe, char* vcf, int* cnt);
vcard_last_error CheckProperty(char* src, int* len, char** current, char** next);
vcard_last_error ParseVcfLine(char* line, char** vptr,int* vlen, char* vresult);
vcard_last_error CheckCompleteLine(char* src, int *endCrLfLen);
vcard_last_error VCardStrCmp(char* src,char* tar,int length);
vcard_last_error VCardStrCpy(char* tar,char* src, int length);
vcard_last_error VCardSizeOf(char* src,int* rel_length);
vcard_last_error GetCompleteLine(char* src,char** cur, char** next);
/////////////////////////////////////////////////////////////////////////////
// vcard21 dialog
/*
struct _ContApp_Vcard
{
    char* m_vcard_n;
    char* m_vcard_email;
    char* m_vcard_tz;
    char* m_vcard_adr;
    char* m_vcard_fn;
    char* m_vcard_title;
    char* m_vcard_bday;
    char* m_vcard_org;
    char* m_vcard_url;
    char* m_vcard_phone;
    char* m_vcard_mobile;
    char* m_vcard_fax;
    char* m_vcard_result;

};
*/
boolean Vcard_CreateVcfFile(CContApp *pMe, int contId, char* fileName); 

boolean Parse_address(char* vptr, char** temp, int *vlen);

boolean Vcard_CreateVcfFileEx(CContApp *pMe, AEEAddrField *pFields, int nFldCnt, char* fileName);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif
#endif // CONTAPP_VCARD_H

