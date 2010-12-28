/*==============================================================================
// 文件：
//        ContApp_vCard.c
//        2009-03-02 twq草拟版本(Draft Version)
//        版权所有(c) 2009 Anylook Tech CO.,LTD.
//        
// 文件说明：
// 由于时间问题，只限于电话本使用，不完全支持VCARD定义的所有字段，不过有框架在，扩展相对容易。       
// 作者：2009-03-02
// 创建日期：
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
#include "ContApp_priv.h"
#include "ContAppCommon.h"
#include "ContApp_vCard.h"

#define RELEASEIF(x) if (x) { IBASE_Release((IBase *)(x)); (x) = 0; }

#ifdef FEATURE_SUPPORT_VCARD

const property_id_map VCARD_PROPERTY_ID[]=
{
    {"BEGIN",   VCARD_P_INDEX_BEGIN,    5},
    {"VERSION", VCARD_P_INDEX_VERSION,  10},
    {"N",       VCARD_P_INDEX_N,        VCARD_MAX_VALUE_OF_N},
    {"FN",      VCARD_P_INDEX_FN,       VCARD_MAX_VALUE_OF_FN},
    {"TITLE",   VCARD_P_INDEX_TITLE,    VCARD_MAX_VALUE_OF_TITLE},
    {"ADR",     VCARD_P_INDEX_ADR,      VCARD_MAX_VALUE_OF_ADR},
    {"ORG",     VCARD_P_INDEX_ORG,      VCARD_MAX_VALUE_OF_ORG},
    {"URL",     VCARD_P_INDEX_URL,      VCARD_MAX_VALUE_OF_URL},
    {"TEL",     VCARD_P_INDEX_TEL,      VCARD_MAX_VALUE_OF_TEL},
    {"EMAIL",   VCARD_T_EMAIL_INTERNET, VCARD_MAX_VALUE_OF_EMAIL},
    {"NOTE",    VCARD_P_INDEX_NOTE,     VCARD_MAX_VALUE_OF_NOTE},
    {"END",     VCARD_P_INDEX_END,      5}
};

const tel_type_map TEL_TYPE_ID[]=
{
    {"VOICE",VCARD_T_TEL_VOICE,VCARD_MAX_VALUE_OF_TEL},
    {"CELL",VCARD_T_TEL_MOBILE,VCARD_MAX_VALUE_OF_TEL},
    {"HOME",VCARD_T_TEL_HOME,VCARD_MAX_VALUE_OF_TEL},
    {"WORK",VCARD_T_TEL_OFFICE,VCARD_MAX_VALUE_OF_TEL},
    {"FAX",VCARD_T_TEL_FAX,VCARD_MAX_VALUE_OF_TEL}

};

const email_type_map EMAIL_TYPE_ID[]=
{
    {"INTERNET",VCARD_T_EMAIL_INTERNET,VCARD_MAX_VALUE_OF_EMAIL}
};

//static const char* id_vcard_version={"2.1"};
//static const char* opp_vcard_flag={"VCARD"};

const char property_table[]=
{
    VCARD_P_INDEX_N,VCARD_P_INDEX_FN,VCARD_P_INDEX_TITLE,
    VCARD_P_INDEX_ADR,VCARD_P_INDEX_ORG,VCARD_P_INDEX_URL,
    VCARD_T_TEL_VOICE,VCARD_T_TEL_MOBILE,VCARD_T_TEL_FAX,
    VCARD_T_EMAIL_INTERNET
};

#define VCARD_SIZEOF_P                  sizeof(VCARD_PROPERTY_ID)/sizeof(VCARD_PROPERTY_ID[0])
#define VCARD_SIZEOF_T_TEL              sizeof(TEL_TYPE_ID)/sizeof(TEL_TYPE_ID[0])
#define VCARD_SIZEOF_T_EMAIL            sizeof(EMAIL_TYPE_ID)/sizeof(EMAIL_TYPE_ID[0])

static vcardentry_t current_entry[]=
{
    {NULL,VCARD_P_T_INVALID},{NULL,VCARD_P_T_INVALID},
    {NULL,VCARD_P_T_INVALID},{NULL,VCARD_P_T_INVALID},
    {NULL,VCARD_P_T_INVALID},{NULL,VCARD_P_T_INVALID},
    {NULL,VCARD_P_T_INVALID},{NULL,VCARD_P_T_INVALID},
    {NULL,VCARD_P_T_INVALID},{NULL,VCARD_P_T_INVALID},
    {NULL,VCARD_P_T_INVALID},{NULL,VCARD_P_T_INVALID}
};

static add_type_map add_type_table[]=
{
    {NULL,ADD_LAST},{NULL,ADD_LAST},
    {NULL,ADD_LAST},{NULL,ADD_LAST},
    {NULL,ADD_LAST},{NULL,ADD_LAST},
    {NULL,ADD_LAST}

};


#define MAX_ENTRIES         (sizeof(current_entry)/sizeof(vcardentry_t))

static char* vcard_record = NULL;
static voice_type v_type;//add
static const char* opp_vcard_flag={"VCARD"};

static char GetPidFromIndex(int index);
//static int GetIndexFromPid(char pid);
//static char GetMidFromIndex(int index);
//static int GetIndexFromMid(char mid);
//static char GetTidFromIndex(int index);
//static int GetIndexFromTid(char tid);
/*
-----------------------------------------------------------------------------------------------------
*/
/*
-----------------------------------------------------------------------------------------------------
*/
/////////////////////////////////////////////////////////////////////////////
/*
unsigned int VCardGetEntryNumber(void)
{
    return (unsigned int)MAX_ENTRIES;
}

////////////////////////////////////////////////////////////////////////////
boolean VCardGenAtiCommand(int index, char** cmd)
{
    if(index<VCardGetEntryNumber())
    {
        (*cmd) = "";
        (*cmd)+="ATI0";
        (*cmd)+=property_table[index];
        return TRUE;
    }
    return FALSE;
}

/////////////////////////////////////////////////////////////////////////
bool VCardGenAtuCommand(bool* complete,char* current,CString* cmd,char** next)
{
    char *now;
    char *vptr;
    char vresult;
    int vlen;
    *complete=FALSE;
    if(GetCompleteLine(current,&now, next)==VCARD_SUCCESS)
    {
        if(ParseVcfLine(now, &vptr,&vlen, &vresult)==VCARD_SUCCESS)
        {
            switch(vresult)
            {
                case VCARD_P_INDEX_BEGIN:
                case VCARD_P_INDEX_VERSION:
                case VCARD_P_INDEX_TEL:     
                case VCARD_P_INDEX_EMAIL:   
                    (*cmd)=_T("");
                    return TRUE;
                    break;
                case VCARD_P_INDEX_END:
                {
                    *complete=TRUE;
                    return TRUE;
                }break;
                case VCARD_P_INDEX_N: 
                case VCARD_P_INDEX_FN:      
                case VCARD_P_INDEX_TITLE:   
                case VCARD_P_INDEX_ADR:     
                case VCARD_P_INDEX_ORG:     
                case VCARD_P_INDEX_URL:     
                case VCARD_T_TEL_VOICE:     
                case VCARD_T_TEL_MOBILE:    
                case VCARD_T_TEL_FAX:       
                case VCARD_T_EMAIL_INTERNET:
                {
                    char value[VCARD_MAX_PROPERTY];
                    memset(value,0,VCARD_MAX_PROPERTY);
                    for(int i=0;i<vlen;i++)
                    {
                        *(value+i)=*(vptr+i);
                    }
                    (*cmd)=_T("");
                    (*cmd)+="ATU0";
                    (*cmd)+=vresult;
                    (*cmd)+=value;
                    return TRUE;
                }break;
                default:
                    return FALSE;

            }
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
    return FALSE;
}
*/
vcard_last_error VCardInsertEntry(char* value, char property_id)
{
    int i;
    
    for(i=0;i<MAX_ENTRIES;i++)
    {
        if(current_entry[i].property_id==VCARD_P_T_INVALID)
        {
            current_entry[i].entry=value;
            current_entry[i].property_id=property_id;
            return VCARD_SUCCESS;
        }
    }
    return VCARD_FAIL;
}
/*
-----------------------------------------------------------------------------------------------------
*/
vcard_last_error VCardDeleteEntry(char property_id)
{
    unsigned int i;
    for(i=0;i<MAX_ENTRIES;i++)
    {
        if(current_entry[i].property_id==property_id)
        {
            //*current_entry[i].entry="";
            FREEIF(current_entry[i].entry);
            current_entry[i].property_id=VCARD_P_T_INVALID;
            return VCARD_SUCCESS;
        }
    }
    return VCARD_FAIL;
}
/*
-----------------------------------------------------------------------------------------------------
*/
vcard_last_error VCardDeleteAllEntry(void)
{
    unsigned int i;
    for(i=0;i<MAX_ENTRIES;i++)
    {
        FREEIF(current_entry[i].entry);
        current_entry[i].property_id=VCARD_P_T_INVALID;
    }
    return VCARD_SUCCESS;
}
/*
-----------------------------------------------------------------------------------------------------
*/
vcard_last_error VCardEncode(void)
{
    unsigned int i;
    char* vcardEncode = NULL;

    if((vcardEncode = (char*)MALLOC(VCARD_MAX_PROPERTY)) == NULL)
    {
        return VCARD_FAIL;
    }

    STRCAT(vcardEncode, VCARD_BEGIN);
    STRCAT(vcardEncode, VCARD_DELIMTER);
    STRCAT(vcardEncode, VCARD_VERSION21);
    STRCAT(vcardEncode, VCARD_DELIMTER);
        
    for(i=0;i<MAX_ENTRIES;i++)
    {
        if(current_entry[i].property_id==VCARD_P_T_INVALID||(current_entry[i].entry==NULL))
        {
            continue;
        }
        
        switch(current_entry[i].property_id)
        {
            //N:
            case VCARD_P_INDEX_N:
                STRCAT(vcardEncode, VCARD_NAME);
                STRCAT(vcardEncode, current_entry[i].entry);
                STRCAT(vcardEncode, "\0");
                STRCAT(vcardEncode, VCARD_DELIMTER_EDIT);
                //vcardencode+=(VCARD_NAME+(*current_entry[i].entry)+VCARD_DELIMTER_EDIT);
                break;

            // FN:
            case VCARD_P_INDEX_FN:
                STRCAT(vcardEncode, VCARD_FNAME);
                STRCAT(vcardEncode, current_entry[i].entry);
                STRCAT(vcardEncode, VCARD_STREND);
                STRCAT(vcardEncode, VCARD_DELIMTER_EDIT);
                //vcardencode+=(VCARD_FNAME+(*current_entry[i].entry)+VCARD_DELIMTER_EDIT);
                break;

            //TITLE:
            case VCARD_P_INDEX_TITLE:
                STRCAT(vcardEncode, VCARD_TITLE);
                STRCAT(vcardEncode, current_entry[i].entry);
                STRCAT(vcardEncode, VCARD_STREND);
                STRCAT(vcardEncode, VCARD_DELIMTER_EDIT);
                //vcardencode+=(VCARD_TITLE+(*current_entry[i].entry)+VCARD_DELIMTER_EDIT);
                break;

            //ADR:
            case VCARD_P_INDEX_ADR:
            {
                STRCAT(vcardEncode, VCARD_ADDRESS);
                STRCAT(vcardEncode, current_entry[i].entry);
                STRCAT(vcardEncode, VCARD_STREND);
                STRCAT(vcardEncode, VCARD_DELIMTER_EDIT);
                //vcardencode+=(VCARD_ADDRESS+(*current_entry[i].entry)+VCARD_DELIMTER_EDIT);
            }
                break;

            //ORG:
            case VCARD_P_INDEX_ORG:
                STRCAT(vcardEncode, VCARD_ORG);
                STRCAT(vcardEncode, current_entry[i].entry);
                STRCAT(vcardEncode, VCARD_STREND);
                STRCAT(vcardEncode, VCARD_DELIMTER_EDIT);
                //vcardencode+=(VCARD_ORG+(*current_entry[i].entry)+VCARD_DELIMTER_EDIT);
                break;

            //URL:
            case VCARD_P_INDEX_URL:
                STRCAT(vcardEncode, VCARD_URL);
                STRCAT(vcardEncode, current_entry[i].entry);
                STRCAT(vcardEncode, VCARD_STREND);
                STRCAT(vcardEncode, VCARD_DELIMTER_EDIT);
                //vcardencode+=(VCARD_URL+(*current_entry[i].entry)+VCARD_DELIMTER_EDIT);
                break;

            //TEL:
            case VCARD_T_TEL_HOME:
                STRCAT(vcardEncode, VCARD_HOME);//HOME
                STRCAT(vcardEncode, current_entry[i].entry);
                STRCAT(vcardEncode, VCARD_STREND);
                STRCAT(vcardEncode, VCARD_DELIMTER_EDIT);
                //vcardencode+=(VCARD_TEL+(*current_entry[i].entry)+VCARD_DELIMTER_EDIT);
                break;

            //TEL:
            case VCARD_T_TEL_OFFICE:
                STRCAT(vcardEncode, VCARD_OFFICE);//OFFICE
                STRCAT(vcardEncode, current_entry[i].entry);
                STRCAT(vcardEncode, VCARD_STREND);
                STRCAT(vcardEncode, VCARD_DELIMTER_EDIT);
                //vcardencode+=(VCARD_TEL+(*current_entry[i].entry)+VCARD_DELIMTER_EDIT);
                break;
                
            //TEL
            case VCARD_T_TEL_MOBILE:
                STRCAT(vcardEncode, VCARD_MOBILE);//MOBILE
                STRCAT(vcardEncode, current_entry[i].entry);
                STRCAT(vcardEncode, VCARD_STREND);
                STRCAT(vcardEncode, VCARD_DELIMTER_EDIT);
                //vcardencode+=(VCARD_MOBILE+(*current_entry[i].entry)+VCARD_DELIMTER_EDIT);
                break;

            //TEL
            case VCARD_T_TEL_FAX:
                STRCAT(vcardEncode, VCARD_FAX);//FAX
                STRCAT(vcardEncode, current_entry[i].entry);
                STRCAT(vcardEncode, VCARD_STREND);
                STRCAT(vcardEncode, VCARD_DELIMTER_EDIT);
                //vcardencode+=(VCARD_FAX+(*current_entry[i].entry)+VCARD_DELIMTER_EDIT);
                break;
                
            case VCARD_T_EMAIL_INTERNET:
                STRCAT(vcardEncode, VCARD_EMAIL);
                STRCAT(vcardEncode, current_entry[i].entry);
                STRCAT(vcardEncode, VCARD_STREND);
                STRCAT(vcardEncode, VCARD_DELIMTER_EDIT);
                //vcardencode+=(VCARD_EMAIL+(*current_entry[i].entry)+VCARD_DELIMTER_EDIT);
                break;
                
            case VCARD_P_INDEX_NOTE:
                STRCAT(vcardEncode, VCARD_NOTE);
                STRCAT(vcardEncode, current_entry[i].entry);
                STRCAT(vcardEncode, VCARD_STREND);
                STRCAT(vcardEncode, VCARD_DELIMTER_EDIT);
                //vcardencode+=(VCARD_EMAIL+(*current_entry[i].entry)+VCARD_DELIMTER_EDIT);
                break;     
            default:
                break;
        }
    }

    STRCAT(vcardEncode, VCARD_END);
    STRCAT(vcardEncode, VCARD_DELIMTER_EDIT);
    
    vcard_record=vcardEncode;
    return VCARD_SUCCESS;
}

/*
void SetVCardRecord(CString rec)
{
    vcard_record=rec;
}

CString* GetVCardRecord(void)
{
    return &vcard_record;
}*/
/*
-----------------------------------------------------------------------------------------------------
*/
static char GetPidFromIndex(int index)
{
    if(index>=(VCARD_SIZEOF_P))
        return VCARD_P_T_INVALID;
    return VCARD_PROPERTY_ID[index].p_index;
}
/*
-----------------------------------------------------------------------------------------------------

static int GetIndexFromPid(char pid)
{
    int index;
    for(index=0;index<(VCARD_SIZEOF_P);index++)
    {
        if(VCARD_PROPERTY_ID[index].p_index==pid)
            return index;
    }
    return 0;//check INvalid
}*/
/*
-----------------------------------------------------------------------------------------------------

static char GetTidFromIndex(int index)
{
    if(index>=(VCARD_SIZEOF_T_TEL))
        return VCARD_P_T_INVALID;
    return TEL_TYPE_ID[index].t_index;
}*/
/*
-----------------------------------------------------------------------------------------------------

static int GetIndexFromTid(char tid)
{
    int index;
    for(index=0;index<(VCARD_SIZEOF_T_TEL);index++)
    {
        if(TEL_TYPE_ID[index].t_index==tid)
            return index;
    }
    return 0;//check INvalid
}*/
/*
-----------------------------------------------------------------------------------------------------

static char GetMidFromIndex(int index)
{
    if(index>=(VCARD_SIZEOF_T_EMAIL))
        return VCARD_P_T_INVALID;
    return EMAIL_TYPE_ID[index].m_index;
}*/
/*
-----------------------------------------------------------------------------------------------------
static int GetIndexFromMid(char mid)
{
    int index;
    for(index=0;index<(VCARD_SIZEOF_T_EMAIL);index++)
    {
        if(EMAIL_TYPE_ID[index].m_index==mid)
            return index;
    }
    return 0;//check INvalid
}
*/
/*
-----------------------------------------------------------------------------------------------------
*/
vcard_last_error ParseVcfLine(char* line, char** vptr,int* vlen, char* vresult)
{
    int stack_size=VCARD_TYPE_STACK_RESET;
    /*include root layer, that is atleast one stack*/ 
    char* cur;
    int cur_len;
    char* next;
    vcard_last_error err=CheckProperty(line,&cur_len,&cur,&next);
    stack_size++;
    if(err==VCARD_SUCCESS_PROPERTY)
    {
        int index;
        char pid;
        if(line!=cur){return VCARD_FAIL; }
        for(index=0;index<(VCARD_SIZEOF_P);index++)
        {
            if(VCardStrCmp((char*)VCARD_PROPERTY_ID[index].pid,cur,cur_len)==VCARD_SUCCESS)
            {
                pid=GetPidFromIndex(index);
                /*"begin","end"*/
                if(pid==VCARD_P_INDEX_BEGIN||pid==VCARD_P_INDEX_END)
                {
                    if(VCardSizeOf(next,vlen)==VCARD_SUCCESS)
                    {
                        if(*vlen>VCARD_PROPERTY_ID[index].length) { return VCARD_FAIL;}
                        if(VCardStrCmp((char*)opp_vcard_flag,next,sizeof(opp_vcard_flag))==VCARD_SUCCESS)
                        {  
                            *vptr=next;*vresult=pid; 
                            return VCARD_SUCCESS;
                        }
                    }
                    return VCARD_FAIL;
                }
                /*other property's value*/
                if(VCardSizeOf(next,vlen)==VCARD_SUCCESS)
                {
                    if(*vlen>VCARD_PROPERTY_ID[index].length) 
                    {
                        return VCARD_PROPERTY_TOO_LONG;
                    }
                    else
                    {
                        *vptr=next;*vresult=pid; 
                        return VCARD_SUCCESS;
                    }
                }
                return VCARD_FAIL;
            }
        }
        if(index>=(VCARD_SIZEOF_P))
        {   return VCARD_UNSUPPORTED_FEATURES;  }
        return VCARD_FAIL;
    };
    
    if(err==VCARD_SUCCESS_TYPE)
    {
        int index;
        char pid;
        if(line!=cur)
        {
            return VCARD_FAIL; 
        }
        
        for(index=0;index<(VCARD_SIZEOF_P);index++)
        {
            // 判断是什么类型，TEL? ORG? ADR? EMAIL? N? FN? ...
            if(VCardStrCmp((char*)VCARD_PROPERTY_ID[index].pid,cur,cur_len)==VCARD_SUCCESS)
            {
                pid=GetPidFromIndex(index);
                switch(pid)
                {
                    case VCARD_P_INDEX_TEL:
                    {
                        line=next;
                        err=CheckProperty(line,&cur_len,&cur,&next);
                        //  扩展一下VOICE, 其实FAX也有HOME和WORK之分
                        while(err != VCARD_SUCCESS_PROPERTY)
                        {
                            if(err == VCARD_SUCCESS_TYPE)
                            {
                                if(VCARD_SUCCESS==VCardStrCmp((char*)VCARD_TEL_HOME,cur,cur_len))
                                {
                                    v_type=VOICE_HOME;
                                }
                                else if(VCARD_SUCCESS==VCardStrCmp((char*)VCARD_TEL_WORK,cur,cur_len))
                                {
                                    v_type=VOICE_WORK;
                                }
                                else if(VCARD_SUCCESS==VCardStrCmp((char*)VCARD_TEL_CELL,cur,cur_len))
                                {
                                    v_type=VOICE_CELL;
                                }
                            }
                            
                            stack_size++;
                            line = next;
                            err=CheckProperty(line,&cur_len,&cur,&next);
                        }
                        // end
                        if(err==VCARD_SUCCESS_PROPERTY)
                        {
                            for(index=0;index<(VCARD_SIZEOF_T_TEL);index++)
                            {
                                if(VCARD_SUCCESS==VCardStrCmp((char*)TEL_TYPE_ID[index].tid,cur,cur_len))
                                {
                                    if(VCardSizeOf(next,vlen)==VCARD_SUCCESS)
                                    {
                                        if(*vlen>TEL_TYPE_ID[index].length)
                                        { 
                                            return VCARD_FAIL;
                                        }
                                        
                                        *vptr=next;*vresult=TEL_TYPE_ID[index].t_index; 
                                        return VCARD_SUCCESS;
                                    }
                                }
                            }
                            if(index>=(VCARD_SIZEOF_T_TEL))
                            {   return VCARD_UNSUPPORTED_FEATURES;}
                            return VCARD_FAIL;
                        }
                        /*
                        if(err==VCARD_SUCCESS_TYPE)
                        {
                            //stack is deeper
                            return VCARD_UNSUPPORTED_FEATURES;
                        }
                        */
                        return VCARD_FAIL;
                    }

                    case VCARD_P_INDEX_EMAIL:
                    case VCARD_T_EMAIL_INTERNET:
                    {
                        line=next;
                        err=CheckProperty(line,&cur_len,&cur,&next);
                        while(err != VCARD_SUCCESS_PROPERTY)
                        {
                            if(VCARD_SUCCESS==VCardStrCmp((char*)VCARD_EMAIL_INTERNET,cur,cur_len))
                            {
                                //isHome=TRUE;
                                // TO MARK FOR email internet
                            }

                            line=next;
                            err=CheckProperty(line,&cur_len,&cur,&next);
                            stack_size++;
                        }
                        
                        if(err==VCARD_SUCCESS_PROPERTY)
                        {
                            for(index=0;index<(VCARD_SIZEOF_T_EMAIL);index++)
                            {
                                if(VCARD_SUCCESS==VCardStrCmp((char*)EMAIL_TYPE_ID[index].mid,cur,cur_len))
                                {
                                    if(VCardSizeOf(next,vlen)==VCARD_SUCCESS)
                                    {
                                        if(*vlen>EMAIL_TYPE_ID[index].length) { return VCARD_FAIL;}
                                        *vptr=next;*vresult=EMAIL_TYPE_ID[index].m_index; 
                                        return VCARD_SUCCESS;
                                    }
                                }
                                // 这里暂时这样，支持标准不是很好
                                else
                                {
                                    if(VCardSizeOf(next,vlen)==VCARD_SUCCESS)
                                    {
                                        if(*vlen>EMAIL_TYPE_ID[index].length) { return VCARD_FAIL;}
                                        *vptr=next;*vresult=EMAIL_TYPE_ID[index].m_index; 
                                        return VCARD_SUCCESS;
                                    }
                                }
                                // 支持标准不是很好
                            }
                            if(index>=(VCARD_SIZEOF_T_EMAIL))
                            {   return VCARD_UNSUPPORTED_FEATURES;}
                            return VCARD_FAIL;
                        }
                        //if(err==VCARD_SUCCESS_TYPE)
                            /*stack is deeper*/
                        //    return VCARD_UNSUPPORTED_FEATURES;
                        return VCARD_FAIL;                 
                        
                    }

                    case VCARD_P_INDEX_ADR:
                        // to code
                        line=next;
                        err=CheckProperty(line,&cur_len,&cur,&next);
                        while(err != VCARD_SUCCESS_PROPERTY)
                        {
                            if(VCARD_SUCCESS==VCardStrCmp((char*)VCARD_ADD_HOME,cur,cur_len))
                            {
                                //isHome=TRUE; 
                                // 丢弃地址类型信息
                                // TO MARK FOR email internet
                            }

                            line=next;
                            err=CheckProperty(line,&cur_len,&cur,&next);
                            stack_size++;
                        }

                       
                        if(err==VCARD_SUCCESS_PROPERTY)
                        {
                            if(VCardSizeOf(next,vlen)==VCARD_SUCCESS)
                            {
                                if(*vlen>VCARD_MAX_VALUE_OF_ADR) { return VCARD_FAIL;}
                                *vptr=next;*vresult=VCARD_P_INDEX_ADR; 
                                return VCARD_SUCCESS;
                            }
                        }
                        
                        return VCARD_SUCCESS;

                    default: 
                        return VCARD_SUCCESS;
                }
                return VCARD_FAIL;
            }
        }
        if(index>=(VCARD_SIZEOF_P))
        {   return VCARD_UNSUPPORTED_FEATURES;  }
        return VCARD_FAIL;
    };
    
    {return VCARD_PROPERTY_WRONG;}
}
/*
-----------------------------------------------------------------------------------------------------
*/
vcard_last_error ParseVcfFile(CContApp *pMe, char* vcf, int *cnt)
{
    char *src=NULL;
    char *cur=NULL;
    char *next=NULL;
    char *vptr = NULL;
    char vresult=NULL;
    int vlen=0;
    boolean begin,end;
    int whilei = 0;
    int err = 0;
    char temp[128] = {0};
    int tmpCnt = 0;
    
    if(STRLEN(vcf)==0)
    {
        return VCARD_FAIL;
    }
    

    CContApp_FreeBuffer(pMe);
    src=vcf;
    begin=FALSE;
    end=FALSE;
    while(GetCompleteLine(src,&cur, &next)==VCARD_SUCCESS)
    {
        whilei++;
        if(ParseVcfLine(cur,&vptr,&vlen, &vresult)==VCARD_SUCCESS)
        {
            switch(vresult)
            {
                case VCARD_P_INDEX_BEGIN:
                {
                    if(begin==FALSE&&end==FALSE) 
                    {
                        begin=TRUE;
                    }
                    else
                    {
                        err = 1;
                        goto __ParseVcfFile_exit;
                    }
                }
                break;
                
                case VCARD_P_INDEX_END:
                {
                    if(begin==TRUE&&end==FALSE)
                    {
                        end=TRUE;
                    }
                    else
                    {
                        err = 2;
                        goto __ParseVcfFile_exit;
                    }
                }
                break;
                
                case VCARD_P_INDEX_VERSION:
                case VCARD_P_INDEX_TITLE:
                case VCARD_P_INDEX_ORG:   
                case VCARD_P_INDEX_TEL:    
                    err = 3;
                    break;
                    
                case VCARD_P_INDEX_N: 
                case VCARD_P_INDEX_FN:      
                {
                    //FREEIF(pMe->m_pAddNewName);
                    pMe->m_pAddNewName = (AECHAR*)MALLOC((vlen+1)*sizeof(AECHAR));
                    if(pMe->m_pAddNewName == NULL)
                    {
                        err = 4;
                        goto __ParseVcfFile_exit;
                    }
                    /*
                    STRNCPY(temp, vptr, vlen);
                    STRTOWSTR(temp, pMe->m_pAddNewName, (vlen+1)*sizeof(AECHAR));
                    
                    temp[vlen] = '\0';
                    */
                    
                    UTF8TOWSTR((byte*)vptr, vlen, pMe->m_pAddNewName, (vlen+ 1)*sizeof(AECHAR));
                }
                break;
                    
                case VCARD_P_INDEX_ADR:
                {
                    //FREEIF(pMe->m_pAddNewAddress);
                    char* tmp=NULL;
                    int len=0;
                    
                    if(TRUE==Parse_address(vptr, &tmp, &len))
                    {
                        pMe->m_pAddNewAddress= (AECHAR*)MALLOC((len + 1)*sizeof(AECHAR));
                        if(pMe->m_pAddNewAddress == NULL)
                        {
                            err = 5;
                            goto __ParseVcfFile_exit;
                        }
                        //STRTOWSTR(tmp, pMe->m_pAddNewAddress, (len+1)*sizeof(AECHAR));
                        UTF8TOWSTR((byte*)tmp, len, pMe->m_pAddNewAddress, (len + 1)*sizeof(AECHAR));
                    }
                    //STRNCPY(temp, vptr, vlen);
                    //temp[vlen] = '\0';
                }
                break;
                                    
                case VCARD_P_INDEX_URL:     
                {
                    //FREEIF(pMe->m_pAddNewURL);
                    if(vlen > VCARD_BASE_URL_LEN)/* http:// */
                    {
                        pMe->m_pAddNewURL= (AECHAR*)MALLOC((vlen+1 - VCARD_BASE_URL_LEN)*sizeof(AECHAR));
                        if(pMe->m_pAddNewURL == NULL)
                        {
                            err = 4;
                            goto __ParseVcfFile_exit;
                        }
                        
                        UTF8TOWSTR((byte*)vptr +VCARD_BASE_URL_LEN, vlen-VCARD_BASE_URL_LEN, pMe->m_pAddNewURL, (vlen+1-VCARD_BASE_URL_LEN)*sizeof(AECHAR));
                    }
                }
                break;
                                    
                case VCARD_P_INDEX_EMAIL:  
                case VCARD_T_EMAIL_INTERNET:
                {
                    //FREEIF(pMe->m_pAddNewEMAIL);
                    pMe->m_pAddNewEMAIL= (AECHAR*)MALLOC((vlen+1)*sizeof(AECHAR));
                    if(pMe->m_pAddNewEMAIL == NULL)
                    {
                        err = 7;
                        goto __ParseVcfFile_exit;
                    }
                    UTF8TOWSTR((byte*)vptr, vlen, pMe->m_pAddNewEMAIL, (vlen+ 1)*sizeof(AECHAR));
                }
                break;
                        
                case VCARD_T_TEL_VOICE:
                {
                    if(v_type==VOICE_HOME)
                    {
                        v_type=VOICE_FIRST;
                        //FREEIF(pMe->m_pAddNewHome);
                        pMe->m_pAddNewHome= (AECHAR*)MALLOC((vlen+1)*sizeof(AECHAR));
                        if(pMe->m_pAddNewHome == NULL)
                        {
                            err = 8;
                            goto __ParseVcfFile_exit;
                        }
                        STRNCPY(temp, vptr, vlen);
                        temp[vlen] = '\0';
                        STRTOWSTR(temp, pMe->m_pAddNewHome, (vlen+1)*sizeof(AECHAR));

                    }
                    else if(v_type==VOICE_WORK)
                    {
                        v_type=VOICE_FIRST;
                        //FREEIF(pMe->m_pAddNewOffice);
                        pMe->m_pAddNewOffice= (AECHAR*)MALLOC((vlen+1)*sizeof(AECHAR));
                        if(pMe->m_pAddNewOffice == NULL)
                        {
                            err = 9;
                            goto __ParseVcfFile_exit;
                        }
                        STRNCPY(temp, vptr, vlen);
                        temp[vlen] = '\0';
                        STRTOWSTR(temp, pMe->m_pAddNewOffice, (vlen+1)*sizeof(AECHAR));
                    }
                    else if(v_type==VOICE_CELL)
                    {
                        v_type=VOICE_FIRST;
                        //FREEIF(pMe->m_pAddNewOffice);
                        pMe->m_pAddNewMobile= (AECHAR*)MALLOC((vlen+1)*sizeof(AECHAR));
                        if(pMe->m_pAddNewMobile== NULL)
                        {
                            err = 10;
                            goto __ParseVcfFile_exit;
                        }
                        STRNCPY(temp, vptr, vlen);
                        temp[vlen] = '\0';
                        STRTOWSTR(temp, pMe->m_pAddNewMobile, (vlen+1)*sizeof(AECHAR));
                    }
                    else
                    {
                        goto __ParseVcfFile_exit;
                    }

                }
                break;

                case VCARD_T_TEL_MOBILE:
                {
                    //FREEIF(pMe->m_pAddNewMobile);
                    pMe->m_pAddNewMobile= (AECHAR*)MALLOC((vlen+1)*sizeof(AECHAR));
                    if(pMe->m_pAddNewMobile == NULL)
                    {
                        err = 11;
                        goto __ParseVcfFile_exit;
                    }
                    STRNCPY(temp, vptr, vlen);
                    temp[vlen] = '\0';
                    STRTOWSTR(temp, pMe->m_pAddNewMobile, (vlen+1)*sizeof(AECHAR));
                }
                break;

                case VCARD_T_TEL_HOME:
                {
                    //FREEIF(pMe->m_pAddNewMobile); 
                    pMe->m_pAddNewHome= (AECHAR*)MALLOC((vlen+1)*sizeof(AECHAR));
                    if(pMe->m_pAddNewHome== NULL)
                    {
                        err = 12;
                        goto __ParseVcfFile_exit;
                    }
                    STRNCPY(temp, vptr, vlen);
                    temp[vlen] = '\0';
                    STRTOWSTR(temp, pMe->m_pAddNewHome, (vlen+1)*sizeof(AECHAR));
                }
                break;

                case VCARD_T_TEL_OFFICE:
                {
                    //FREEIF(pMe->m_pAddNewMobile);
                    pMe->m_pAddNewOffice= (AECHAR*)MALLOC((vlen+1)*sizeof(AECHAR));
                    if(pMe->m_pAddNewOffice== NULL)
                    {
                        err = 13;
                        goto __ParseVcfFile_exit;
                    }
                    STRNCPY(temp, vptr, vlen);
                    temp[vlen] = '\0';
                    STRTOWSTR(temp, pMe->m_pAddNewOffice, (vlen+1)*sizeof(AECHAR));
                }
                break;

                case VCARD_T_TEL_FAX:       
                {
                    //FREEIF(pMe->m_pAddNewFax);
                    pMe->m_pAddNewFax= (AECHAR*)MALLOC((vlen+1)*sizeof(AECHAR));
                    if(pMe->m_pAddNewFax == NULL)
                    {
                        err = 14;
                        goto __ParseVcfFile_exit;
                    }
                    STRNCPY(temp, vptr, vlen);
                    temp[vlen] = '\0';
                    STRTOWSTR(temp, pMe->m_pAddNewFax, (vlen+1)*sizeof(AECHAR));
                }
                break;

                case VCARD_P_INDEX_NOTE:       
                {                    
                    pMe->m_pAddNewRemark= (AECHAR*)MALLOC((vlen + 1)*sizeof(AECHAR));
                    if(pMe->m_pAddNewRemark== NULL)
                    {
                        err = 5;
                        goto __ParseVcfFile_exit;
                    }
                    //STRTOWSTR(tmp, pMe->m_pAddNewAddress, (len+1)*sizeof(AECHAR));
                    UTF8TOWSTR((byte*)vptr, vlen, pMe->m_pAddNewRemark, (vlen + 1)*sizeof(AECHAR));
                }
                break;
                
                default:
                    return VCARD_UNSUPPORTED_FEATURES;
            }

            if(vresult>VCARD_P_INDEX_END && vresult<=VCARD_P_INDEX_NOTE)
            {
                if(begin==FALSE||end==TRUE)
                {
                    err = 16;
                    goto __ParseVcfFile_exit;
                }
            }
            else if(begin==TRUE&&end==TRUE&&vresult==VCARD_P_INDEX_END)
            {
                char * tmpNext = NULL;
                int result = 0;
                // 如果电话本满?
                if(SUCCESS != CContApp_CreateCont( pMe, FALSE))
                {
                    CContApp_FreeBuffer(pMe);
                    return VCARD_SAVE_RECORD_FAID;
                }
                tmpCnt++;
                
                CContApp_FreeBuffer(pMe);

                src=next;
                result= GetCompleteLine(src,&cur, &tmpNext);
                
                result = ParseVcfLine(cur,&vptr,&vlen, &vresult);
                // continue to parse another record
                if((*src!=VCARD_NUL && GetCompleteLine(src,&cur, &tmpNext)==VCARD_SUCCESS 
                    && ParseVcfLine(cur,&vptr,&vlen, &vresult)==VCARD_SUCCESS
                    && VCARD_P_INDEX_BEGIN == vresult))
                {
                    begin = FALSE;
                    end = FALSE;
                }
                else
                {
                    *cnt = tmpCnt;
                    return VCARD_SUCCESS;
                }
            }
            src=next;
        }
        else
        {
            err = 17;
            goto __ParseVcfFile_exit;
        }
    }// end while

__ParseVcfFile_exit:
    
    CContApp_FreeBuffer(pMe);
    return VCARD_FAIL;
}
/*
-----------------------------------------------------------------------------------------------------
Exist one fatal error when string is not ended with the special character.
*/
vcard_last_error VCardSizeOf(char* src,int* rel_length)
{
    int length=0;

    while(1)
    {
        if(*src==VCARD_CR||*src==VCARD_LF||*src==VCARD_NUL) 
        {
            *rel_length=length;
            return VCARD_SUCCESS;
        }
        length++;
        src++;
        if(length>VCARD_MAX_PROPERTY)
        {
            return VCARD_PROPERTY_TOO_LONG;
        }
    }
}
/*
-----------------------------------------------------------------------------------------------------
*/
vcard_last_error VCardStrCpy(char* tar,char* src, int length)
{
    int i;
    for(i=0;i<length;i++)
    {
        *(tar+i)=*(src+i);
    }
    return VCARD_SUCCESS;
}
/*
-----------------------------------------------------------------------------------------------------
*/
vcard_last_error VCardStrCmp(char* src, char* tar,int length)
{
    int length_src;
    int length_tar;
    if((length==0)) return VCARD_FAIL;
    if((VCardSizeOf(src,&length_src)!=VCARD_SUCCESS))   return VCARD_FAIL;
    if((VCardSizeOf(tar,&length_tar)!=VCARD_SUCCESS))   return VCARD_FAIL;
    if((length_src<length)||(length_tar<length))   return VCARD_FAIL;
    for(length_src=0;length_src<length;length_src++)
    {
        if((*(src+length_src))!=(*(tar+length_src)))
            return VCARD_FAIL;
    }
    return VCARD_SUCCESS;
}
/*
-----------------------------------------------------------------------------------------------------
*/
vcard_last_error GetCompleteLine(char* src,char** cur, char** next)
{
    int length=0;
    int endCrLfLen=0;
    
    if(VCardSizeOf(src,&length)==VCARD_SUCCESS)    
    {
        if(CheckCompleteLine(src, &endCrLfLen)==VCARD_SUCCESS)
        {
            *next=src+length+endCrLfLen;
            *cur=src;
            return VCARD_SUCCESS;
        }
    }
    return VCARD_FAIL;
}
/*
-----------------------------------------------------------------------------------------------------
*/
vcard_last_error CheckCompleteLine(char* src, int* CrLfLen)
{
    int length=0;
    if(VCardSizeOf(src,&length)!=VCARD_SUCCESS||length==0)
    {
        return VCARD_FAIL;
    }
    /*if(*(src+length)!=(char)VCARD_SPECIALCHAR_PROPERTY_END)
    {
        return VCARD_FAIL;
    }
    
    length++;
    if(*(src+length)!=(char)VCARD_SPECIALCHAR_PROPERTY_END2)
    {
        return VCARD_FAIL;
    }*/
    if((*(src+length)==(char)VCARD_SPECIALCHAR_PROPERTY_END2))
    {
        *CrLfLen=1;
        return VCARD_SUCCESS;
    }

    if((*(src+length)==(char)VCARD_SPECIALCHAR_PROPERTY_END) 
            && (*(src+length +1)==(char)VCARD_SPECIALCHAR_PROPERTY_END2))
    {
        *CrLfLen=2;
        return VCARD_SUCCESS;
    }

    return VCARD_FAIL;    
    //return VCARD_SUCCESS;
}
/*
-----------------------------------------------------------------------------------------------------
*/
/*
    Property may include GROUP or PROPERTY or TYPE
*/
vcard_last_error CheckProperty(char* src, int* len, char** current, char** next)
{
    int format=0;
    int ov;
    char* startpos=src;
    int count=0;
    int tmplen=0;
    if(VCardSizeOf(src,&ov)!=VCARD_SUCCESS)
    {
        return VCARD_PROPERTY_WRONG; 
    }

    tmplen=ov;
    while(1)
    {
        if(ov==0)
        {
            *len=tmplen;
            return VCARD_PROPERTY_WRONG;
        }
        switch(format)
        {
            case 0: /*start of line*/
            {
                switch(*src)
                {
                    case VCARD_SPECIALCHAR_GROUP:
                        *len=count;*current=startpos;src++;*next=src;return VCARD_SUCCESS_GROUP;
                        
                    case VCARD_SPECIALCHAR_TYPE:
                        *len=count;*current=startpos;src++;*next=src;return VCARD_SUCCESS_TYPE;
                        
                    case VCARD_SPECIALCHAR_PROPERTY:
                        *len=count;*current=startpos;src++;*next=src;return VCARD_SUCCESS_PROPERTY;
                        
                    case VCARD_SPECIALCHAR_IGNORE:
                        src++;break;
                        
                    default:
                        format=1;startpos=src;src++;count++;break;
                }
            }
            break;
            
            case 1: /*normal case*/
            {
                switch(*src)
                {
                    case VCARD_SPECIALCHAR_GROUP:
                        *len=count;*current=startpos;src++;*next=src;return VCARD_SUCCESS_GROUP;
                        
                    case VCARD_SPECIALCHAR_TYPE:
                        *len=count;*current=startpos;src++;*next=src;return VCARD_SUCCESS_TYPE;
                        
                    case VCARD_SPECIALCHAR_PROPERTY:
                        *len=count;*current=startpos;src++;*next=src;return VCARD_SUCCESS_PROPERTY;
                        
                    case VCARD_SPECIALCHAR_IGNORE:
                        src++;format=2;break;
                        
                    default:
                        src++;count++;break;
                }
            }
            break; 
            
            case 2:
            {
                switch(*src)
                {
                    case VCARD_SPECIALCHAR_GROUP:
                        *len=count;*current=startpos;src++;*next=src;return VCARD_SUCCESS_GROUP;
                    case VCARD_SPECIALCHAR_TYPE:
                        *len=count;*current=startpos;src++;*next=src;return VCARD_SUCCESS_TYPE;
                    case VCARD_SPECIALCHAR_PROPERTY:
                        *len=count;*current=startpos;src++;*next=src;return VCARD_SUCCESS_PROPERTY;
                    case VCARD_SPECIALCHAR_IGNORE:
                        src++;break;
                    default:
                        return VCARD_PROPERTY_WRONG;
                }
            }
            break;
            
            default:
            {
                return VCARD_PROPERTY_WRONG;
            }break;
        }
        ov--;
    }
    
}
/*
-----------------------------------------------------------------------------------------------------
*/
/////////////////////////////////////////////////////////////////////////////
// vcard21 dialog

/*
vcard21::vcard21(CWnd* pParent =NULL)
    : CDialog(vcard21::IDD, pParent)
{
    //{{AFX_DATA_INIT(vcard21)
    m_vcard_n = _T("");
    m_vcard_email = _T("");
    m_vcard_tz = _T("");
    m_vcard_adr = _T("");
    m_vcard_fn = _T("");
    m_vcard_title = _T("");
    m_vcard_bday = _T("");
    m_vcard_org = _T("");
    m_vcard_url = _T("");
    m_vcard_phone = _T("");
    m_vcard_mobile = _T("");
    m_vcard_fax = _T("");
    //}}AFX_DATA_INIT
    m_vcard_result=_T("");
}*/
/*
-----------------------------------------------------------------------------------------------------
*/
/*
void vcard21::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(vcard21)
    DDX_Text(pDX, IDC_EDIT1_NAME, m_vcard_n);
    DDV_MaxChars(pDX, m_vcard_n, VCARD_MAX_VALUE_OF_N);
    DDX_Text(pDX, IDC_EDIT10_EMAIL, m_vcard_email);
    DDV_MaxChars(pDX, m_vcard_email, VCARD_MAX_VALUE_OF_EMAIL);
    DDX_Text(pDX, IDC_EDIT11_TZ, m_vcard_tz);
    DDV_MaxChars(pDX, m_vcard_tz, 1);
    DDX_Text(pDX, IDC_EDIT12_ADR, m_vcard_adr);
    DDV_MaxChars(pDX, m_vcard_adr, VCARD_MAX_VALUE_OF_ADR);
    DDX_Text(pDX, IDC_EDIT2_FN, m_vcard_fn);
    DDV_MaxChars(pDX, m_vcard_fn, VCARD_MAX_VALUE_OF_FN);
    DDX_Text(pDX, IDC_EDIT3_TITLE, m_vcard_title);
    DDV_MaxChars(pDX, m_vcard_title, VCARD_MAX_VALUE_OF_TITLE);
    DDX_Text(pDX, IDC_EDIT4_BDAY, m_vcard_bday);
    DDV_MaxChars(pDX, m_vcard_bday, 1);
    DDX_Text(pDX, IDC_EDIT5_ORG, m_vcard_org);
    DDV_MaxChars(pDX, m_vcard_org, VCARD_MAX_VALUE_OF_ORG);
    DDX_Text(pDX, IDC_EDIT6_URL, m_vcard_url);
    DDV_MaxChars(pDX, m_vcard_url, VCARD_MAX_VALUE_OF_URL);
    DDX_Text(pDX, IDC_EDIT7_PHONE, m_vcard_phone);
    DDV_MaxChars(pDX, m_vcard_phone, VCARD_MAX_VALUE_OF_TEL);
    DDX_Text(pDX, IDC_EDIT8_MOBILE, m_vcard_mobile);
    DDV_MaxChars(pDX, m_vcard_mobile, VCARD_MAX_VALUE_OF_TEL);
    DDX_Text(pDX, IDC_EDIT9_FAX, m_vcard_fax);
    DDV_MaxChars(pDX, m_vcard_fax, VCARD_MAX_VALUE_OF_TEL);
    //}}AFX_DATA_MAP
}

*/
boolean Vcard_CreateVcfFile(CContApp *pMe, int contId, char* filename) 
{
    IFile* pIFile = NULL;
    IFileMgr *pIFileMgr = NULL;
    char* temp=NULL;
    boolean rnt = FALSE;
    int err = 0;
    char* utf8=NULL;
    int len=0;
    char fileName[MAX_FILE_NAME]={0};// UTF8 max name len is 16
    
    VCardDeleteAllEntry();
    //name
    //VCardInsertEntry(NULL,VCARD_P_INDEX_N);
    if(contId != 0)
    {
        CContApp_GetRecByID(pMe, contId);    
    }
    
    //full name
    if(pMe->m_pAddNewName != NULL)
    {
        utf8 = NULL;
        len=WSTRLEN(pMe->m_pAddNewName);
        
        //if((temp=(char*)MALLOC(VCARD_MAX_VALUE_OF_N)) == NULL)
        if((utf8=(char*)MALLOC(1+len*4)) == NULL)
        {
            err = 2;
            goto OnUpdateVcfFile_exit;
        }
        else
        {
            /*
            WSTRTOSTR(pMe->m_pAddNewName, temp, VCARD_MAX_VALUE_OF_N);
            VCardInsertEntry(temp,VCARD_P_INDEX_N);
            */
            // Note: 
            // UTF8 encoding can occupy anything from 0.5x to 4x more space
            // than Unicode encoding of the same string.   We take the
            // paranoid (and wasteful!) approach and always assume it will
            // take 4x the space
            if(TRUE != WSTRTOUTF8((AECHAR *) pMe->m_pAddNewName,(int)len, (byte *)utf8, (int)(len*4)))
            {
                err = 14;
                goto OnUpdateVcfFile_exit;
            }
            
            VCardInsertEntry(utf8,VCARD_P_INDEX_N);
        }
    }
    
    //title
    /*if(pMe->m_pAddNewName != NULL)
    {
        temp = NULL;
        if((temp=(char*)MALLOC(VCARD_MAX_VALUE_OF_TITLE)) == NULL)
        {
            goto OnUpdateVcfFile_exit;
        }
        else
        {
            WSTRTOSTR(pMe->m_pAddNewName, temp, VCARD_MAX_VALUE_OF_TITLE);
            VCardInsertEntry(temp,VCARD_P_INDEX_TITLE);
        }
    }*/

    //address
    if(pMe->m_pAddNewAddress != NULL)
    {
        utf8 = NULL;
        len=WSTRLEN(pMe->m_pAddNewAddress);
        
        if((utf8=(char*)MALLOC(1+len*4)) == NULL)
        {
            err = 3;
            goto OnUpdateVcfFile_exit;
        }
        else
        {
            utf8[0]=VCARD_SPECIALCHAR_TYPE;
            utf8[1]=VCARD_SPECIALCHAR_TYPE;
            // Note: 
            // UTF8 encoding can occupy anything from 0.5x to 4x more space
            // than Unicode encoding of the same string.   We take the
            // paranoid (and wasteful!) approach and always assume it will
            // take 4x the space
            if(TRUE != WSTRTOUTF8((AECHAR *) pMe->m_pAddNewAddress,(int)len, (byte *)&utf8[2], (int)(len*4)))
            {
                err = 13;
                goto OnUpdateVcfFile_exit;
            }
            
            VCardInsertEntry(utf8,VCARD_P_INDEX_ADR);
        }
    }
    
    //organization contacts 没有公司的域
    //temp = NULL;
    //VCardInsertEntry(temp,VCARD_P_INDEX_ORG);
    
    //web page
    if(pMe->m_pAddNewURL != NULL)
    {
        utf8 = NULL;
        len=WSTRLEN(pMe->m_pAddNewURL);
        
        if((utf8=(char*)MALLOC(1+len*4)) == NULL)
        {
            err = 3;
            goto OnUpdateVcfFile_exit;
        }
        else
        {
            STRCAT(utf8, VCARD_URL_BASE);
            // Note: 
            // UTF8 encoding can occupy anything from 0.5x to 4x more space
            // than Unicode encoding of the same string.   We take the
            // paranoid (and wasteful!) approach and always assume it will
            // take 4x the space
            if(TRUE != WSTRTOUTF8((AECHAR *) pMe->m_pAddNewURL,(int)len, (byte *)&utf8[VCARD_BASE_URL_LEN], (int)(len*4)))
            {
                err = 14;
                goto OnUpdateVcfFile_exit;
            }
            
            VCardInsertEntry(utf8,VCARD_P_INDEX_URL);
        }
    }

    //NOTE
    if(pMe->m_pAddNewRemark!= NULL)
    {
        utf8 = NULL;
        len=WSTRLEN(pMe->m_pAddNewRemark);

        // Note: 
        // UTF8 encoding can occupy anything from 0.5x to 4x more space
        // than Unicode encoding of the same string.   We take the
        // paranoid (and wasteful!) approach and always assume it will
        // take 4x the space
        if((utf8=(char*)MALLOC(1+len*4)) == NULL)
        {
            err = 4;
            goto OnUpdateVcfFile_exit;
        }
        else
        {
            //WSTRTOSTR(pMe->m_pAddNewRemark, temp, VCARD_MAX_VALUE_OF_NOTE);
            //VCardInsertEntry(temp,VCARD_P_INDEX_NOTE);
            if(TRUE != WSTRTOUTF8((AECHAR *) pMe->m_pAddNewRemark,(int)len, (byte *)utf8, (int)(len*4)))
            {
                err = 13;
                goto OnUpdateVcfFile_exit;
            }
            
            VCardInsertEntry(utf8,VCARD_P_INDEX_NOTE);
        }
    }

    //email
    if(pMe->m_pAddNewEMAIL != NULL)
    {
        utf8 = NULL;
        len=WSTRLEN(pMe->m_pAddNewEMAIL);
        
        if((utf8=(char*)MALLOC(1+len*4)) == NULL)
        {
            err = 3;
            goto OnUpdateVcfFile_exit;
        }
        else
        {
            if(TRUE != WSTRTOUTF8((AECHAR *) pMe->m_pAddNewEMAIL,(int)len, (byte *)utf8, (int)(len*4)))
            {
                err = 13;
                goto OnUpdateVcfFile_exit;
            }
            
            VCardInsertEntry(utf8,VCARD_T_EMAIL_INTERNET);

        }
    }
            
    //home
    if(pMe->m_pAddNewHome != NULL)
    {
        temp = NULL;
        if((temp=(char*)MALLOC(VCARD_MAX_VALUE_OF_TEL)) == NULL)
        {
            err = 6;
            goto OnUpdateVcfFile_exit;
        }
        else
        {
            WSTRTOSTR(pMe->m_pAddNewHome, temp, VCARD_MAX_VALUE_OF_TEL);
            VCardInsertEntry(temp,VCARD_T_TEL_HOME);
        }
    }

    //office
    if(pMe->m_pAddNewOffice != NULL)
    {
        temp = NULL;
        if((temp=(char*)MALLOC(VCARD_MAX_VALUE_OF_TEL)) == NULL)
        {
            err = 7;
            goto OnUpdateVcfFile_exit;
        }
        else
        {
            WSTRTOSTR(pMe->m_pAddNewOffice, temp, VCARD_MAX_VALUE_OF_TEL);
            VCardInsertEntry(temp,VCARD_T_TEL_OFFICE);
        }
    }
    
    //mobile
    if(pMe->m_pAddNewMobile != NULL)
    {
        temp = NULL;
        if((temp=(char*)MALLOC(VCARD_MAX_VALUE_OF_TEL)) == NULL)
        {
            err = 8;
            goto OnUpdateVcfFile_exit;
        }
        else
        {
            WSTRTOSTR(pMe->m_pAddNewMobile, temp, VCARD_MAX_VALUE_OF_TEL);
            VCardInsertEntry(temp,VCARD_T_TEL_MOBILE);
        }
    }
    
    //fax
    if(pMe->m_pAddNewFax != NULL)
    {
        temp = NULL;
        if((temp=(char*)MALLOC(VCARD_MAX_VALUE_OF_TEL)) == NULL)
        {
            err = 9;
            goto OnUpdateVcfFile_exit;
        }
        else
        {
            WSTRTOSTR(pMe->m_pAddNewFax, temp, VCARD_MAX_VALUE_OF_TEL);
            VCardInsertEntry(temp,VCARD_T_TEL_FAX);
        }
    }
    
    if(VCARD_SUCCESS != VCardEncode())
    {
        err = 10;
        goto OnUpdateVcfFile_exit;
    }
    //m_vcard_result=*GetVCardRecord();

    {
        
        if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                              AEECLSID_FILEMGR,
                                              (void **)&pIFileMgr))
        {
            err = 11;
            goto OnUpdateVcfFile_exit;
        }

        STRCAT(fileName, VCARD_PATH);
        WSTRTOSTR(pMe->m_pAddNewName, fileName+VCARD_PATH_LEN, MAX_FILE_NAME - VCARD_PATH_LEN);
        STRCAT(fileName, VCARD_EXTEND);
        if ( SUCCESS == IFILEMGR_Test( pIFileMgr, fileName ) )
        {
            if(SUCCESS != IFILEMGR_Remove(pIFileMgr, fileName))
            {
                err = 12;
                goto OnUpdateVcfFile_exit;
            }
        }
        
        pIFile = IFILEMGR_OpenFile( pIFileMgr, fileName, 
                                    _OFM_CREATE );

        if ( pIFile != NULL )
        {
            IFILE_Write( pIFile, vcard_record, STRLEN( vcard_record ) );
            
            if(filename != NULL)
            {
                STRCPY(filename, fileName);
            }
            //IFILE_Write( pIFile, vcard_record, STRLEN( vcard_record ) );
            rnt=TRUE;
        }
    }    
OnUpdateVcfFile_exit:
    CContApp_FreeBuffer(pMe);
    VCardDeleteAllEntry();
    FREEIF(vcard_record);
    RELEASEIF( pIFile );
    RELEASEIF(pIFileMgr);

    return rnt;
}
/*
CString* const vcard21::GetVcardResult(void)
{
    return &m_vcard_result;
}
*/
/*
void OnButton1TempTest() 
{
    // TODO: Add your control notification handler code here
    // TODO: Add your control notification handler code here

    char* dst;
    char* next;
    int len;
    UpdateData(true);
    vcard_last_error error;*/
    /*
    error=CheckProperty(m_vcard_n.GetBuffer(0),&len,&dst,&next );
    if(error!=VCARD_PROPERTY_WRONG)
    {
       char* display = new char [20];
        memset(display,0,20);
        for(int i=0;i<len;i++)
        {
            *(display+i)=*(next+i);
        }
        if(error==VCARD_SUCCESS_GROUP)
            MessageBox(display,".GROUP");
        if(error==VCARD_SUCCESS_PROPERTY)
            MessageBox(display,":PROPERTY");
        if(error==VCARD_SUCCESS_TYPE)
            MessageBox(display,";TYPE");
        delete display;
    }
    *//*
    error=CheckProperty(m_vcard_n.GetBuffer(0),&len,&dst,&next );
    if(error==VCARD_SUCCESS_PROPERTY)
    {
    if(VCardSizeOf((char*)VCARD_PROPERTY_ID[0].pid,&len)==VCARD_SUCCESS)
    {
        if(VCardStrCmp((char*)VCARD_PROPERTY_ID[0].pid,dst,5)==VCARD_SUCCESS)
        {
            CString msg;
            msg.Format("The same %s,%d",(char*)VCARD_PROPERTY_ID[0].pid,len);
            MessageBox(msg);
        }

        if(CheckCompleteLine((char*)VCARD_PROPERTY_ID[0].pid)==VCARD_SUCCESS)
        {
            CString msg;
            msg.Format("%s,%d",(char*)VCARD_PROPERTY_ID[0].pid,len);
            MessageBox(msg);
        }
        else
        {
            MessageBox("not a complete line");
        }
    }
    }
    else
    {
        MessageBox("not a property");
    }

}*/

boolean Parse_address(char* line, char** temp, int *vlen)
{
    int len = 0;
    char *cur;
    char *next;
    vcard_last_error err=VCARD_BEGIN_DUMMY;
    byte address_index = 0;

    err=CheckProperty(line, &len, &cur, &next);
    while(1)
    {
        // 暂时不支持地址分类，但用这个全局数组可以扩展
        add_type_table[address_index].type=address_index+1;
        add_type_table[address_index].value=cur;// 这里应该分配控件

        // 狭隘地获取了一个地址就退出了
        if(len>0)
        {
            //STRNCPY(*temp, cur, len);
            *temp=next;
            *vlen=len;
            return TRUE;
        }
        address_index++;
        line=next;
        err=CheckProperty(line, &len, &cur, &next);
    }
    
    return FALSE;


}


/*==============================================================================
函数：
       CreateVcfFileEx
说明：
       createVcffield 的扩展函数，主要供外部使用
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/

boolean Vcard_CreateVcfFileEx(CContApp *pMe, AEEAddrField *pFields, int nFldCnt, char* fileName) 
{
    int i = 0;
    AEEAddrField *fld = NULL;

    if(!pMe || !pFields)
    return FALSE;

    if(nFldCnt == 0)
    return FALSE;

    fld = pFields;
    
    while(i<nFldCnt)
    {
        if(NULL == fld)
            return FALSE;
        
        switch(pFields[i].fID)
        {
            case MASK_ADDRFIELD_NAME:
                pMe->m_pAddNewName= WSTRDUP((AECHAR *)fld->pBuffer);
                break;

            case AEE_ADDRFIELD_PHONE_GENERIC:
                // Get mobile number
                //FREEIF(pMe->m_pAddNewMobile);
                pMe->m_pAddNewMobile = WSTRDUP((AECHAR *)fld->pBuffer);
                //pMe->m_wFieldMask |= MASK_ADDRFIELD_PHONE_GENERIC;                        
                break;
                
            case AEE_ADDRFIELD_PHONE_HOME:
                //FREEIF(pMe->m_pAddNewHome);
                pMe->m_pAddNewHome = WSTRDUP((AECHAR *)fld->pBuffer);
                //pMe->m_wFieldMask |= MASK_ADDRFIELD_PHONE_HOME;                        
                break;
                
            case AEE_ADDRFIELD_PHONE_WORK:
                //FREEIF(pMe->m_pAddNewOffice);
                pMe->m_pAddNewOffice = WSTRDUP((AECHAR *)fld->pBuffer);
                //pMe->m_wFieldMask |= MASK_ADDRFIELD_PHONE_WORK;                        
                break;
                
            case AEE_ADDRFIELD_PHONE_FAX:
                //FREEIF(pMe->m_pAddNewFax);
                pMe->m_pAddNewFax = WSTRDUP((AECHAR *)fld->pBuffer);
                //pMe->m_wFieldMask |= MASK_ADDRFIELD_PHONE_FAX;                        
                break;
                
            case AEE_ADDRFIELD_EMAIL:
                //FREEIF(pMe->m_pAddNewEMAIL);
                pMe->m_pAddNewEMAIL = WSTRDUP((AECHAR *)fld->pBuffer);
                /*pMe->m_wFieldMask |= MASK_ADDRFIELD_EMAIL;   
                
                if ((pMe->m_wSelFldType == MULTI_SELECT_NUMBER) ||(pMe->m_wSelFldType == SINGLE_SELECT_NUMBER))
                {
                    pMe->m_wFieldMask &= (~MASK_ADDRFIELD_EMAIL);   
                    --pMe->m_wFieldCount;
                }*/
               break;
                
            case AEE_ADDRFIELD_ADDRESS:
                //FREEIF(pMe->m_pAddNewAddress);
                pMe->m_pAddNewAddress = WSTRDUP((AECHAR *)fld->pBuffer);
                /*pMe->m_wFieldMask |= MASK_ADDRFIELD_ADDRESS;
                
                if ((pMe->m_wSelFldType == MULTI_SELECT_NUMBER) ||(pMe->m_wSelFldType == SINGLE_SELECT_NUMBER))
                {
                    pMe->m_wFieldMask &= (~MASK_ADDRFIELD_ADDRESS);
                    --pMe->m_wFieldCount;
                }*/
                break;

            case AEE_ADDRFIELD_URL:
                //FREEIF(pMe->m_pAddNewURL);
                pMe->m_pAddNewURL = WSTRDUP((AECHAR *)fld->pBuffer);
               /* pMe->m_wFieldMask |= MASK_ADDRFIELD_URL;
                
                if ((pMe->m_wSelFldType == MULTI_SELECT_NUMBER) ||(pMe->m_wSelFldType == SINGLE_SELECT_NUMBER))
                {
                    pMe->m_wFieldMask &= (~MASK_ADDRFIELD_URL);
                    --pMe->m_wFieldCount;
                }*/
                break;
                
            case AEE_ADDRFIELD_NOTES:
                //FREEIF(pMe->m_pAddNewRemark);
                pMe->m_pAddNewRemark = WSTRDUP((AECHAR *)fld->pBuffer);
                /*pMe->m_wFieldMask |= MASK_ADDRFIELD_NOTES;
                
                if ((pMe->m_wSelFldType == MULTI_SELECT_NUMBER) ||(pMe->m_wSelFldType == SINGLE_SELECT_NUMBER))
                {
                    pMe->m_wFieldMask &= (~MASK_ADDRFIELD_NOTES);
                    --pMe->m_wFieldCount;
                }*/                        
                break;

            default:
                break;
        }
        fld = &(pFields[++i]);
        
    }

    
     return Vcard_CreateVcfFile(pMe, 0, fileName);

}
#endif