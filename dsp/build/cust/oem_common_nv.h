

/*---------------------------------------------------------------------------

                    以下为运营商相关的OEM NV的默认值
    
*--------------------------------------------------------------------------*/


#if defined(FEATURE_VERSION_SMART)
//Operator parameters
#define OEMNV_EMERGENCYNUM_TABLE_NUM                {{{"110",3},{"112",3},{"911",3},{"999",3},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0}},4} //CFGI_EMERGENCYNUM_TABLE
#define OEMNV_CALLFORWARD_BUSY_ENABLE               "*73"                       //CFGI_CALLFORWARD_BUSY_ENABLE
#define OEMNV_CALLFORWARD_BUSY_DISABLE              "*730"                      //CFGI_CALLFORWARD_BUSY_DISABLE
#define OEMNV_CALLFORWARD_NOANSWER_ENABLE           "*74"                       //CFGI_CALLFORWARD_NOANSWER_ENABLE
#define OEMNV_CALLFORWARD_NOANSWER_DISABLE          "*740"                      //CFGI_CALLFORWARD_NOANSWER_DISABLE
#define OEMNV_CALLFORWARD_NOCONNECT_ENABLE          "*71"                       //CFGI_CALLFORWARD_NOCONNECT_ENABLE
#define OEMNV_CALLFORWARD_NOCONNECT_DISABLE         "*710"                      //CFGI_CALLFORWARD_NOCONNECT_DISABLE
#define OEMNV_CALLFORWARD_ANYWAY_ENABLE             "0"                       //CFGI_CALLFORWARD_ANYWAY_ENABLE
#define OEMNV_CALLFORWARD_ANYWAY_DISABLE            "0"                      //CFGI_CALLFORWARD_ANYWAY_DISABLE
#define OEMNV_CALLFORWARD_DISABLE_ALL               "0"                      //CFGI_CALLFORWARD_DISABLE_ALL
#define OEMNV_CALLFORWARD_WAIT_ENABLE               "*40"                       //CFGI_CALLFORWARD_WAIT_ENABLE
#define OEMNV_CALLFORWARD_WAIT_DISABLE              "*400"                      //CFGI_CALLFORWARD_WAIT_DISABLE
#define OEMNV_CALLFORWARD_VOICEMAIL_ENABLE          "*738"                         //CFGI_CALLFORWARD_VOICEMAIL_ENABLE 
#define OEMNV_VOICEMAIL_NUMBER                      L"388"                         //NV_SMS_VM_NUMBER_I 
#elif defined(FEATURE_VERSION_M8)
//Operator parameters
#define OEMNV_EMERGENCYNUM_TABLE_NUM                {{{"110",3},{"112",3},{"911",3},{"999",3},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0}},4} //CFGI_EMERGENCYNUM_TABLE
#define OEMNV_CALLFORWARD_BUSY_ENABLE               "*73"                       //CFGI_CALLFORWARD_BUSY_ENABLE
#define OEMNV_CALLFORWARD_BUSY_DISABLE              "*730"                      //CFGI_CALLFORWARD_BUSY_DISABLE
#define OEMNV_CALLFORWARD_NOANSWER_ENABLE           "*74"                       //CFGI_CALLFORWARD_NOANSWER_ENABLE
#define OEMNV_CALLFORWARD_NOANSWER_DISABLE          "*740"                      //CFGI_CALLFORWARD_NOANSWER_DISABLE
#define OEMNV_CALLFORWARD_NOCONNECT_ENABLE          "*71"                       //CFGI_CALLFORWARD_NOCONNECT_ENABLE
#define OEMNV_CALLFORWARD_NOCONNECT_DISABLE         "*710"                      //CFGI_CALLFORWARD_NOCONNECT_DISABLE
#define OEMNV_CALLFORWARD_ANYWAY_ENABLE             "0"                       //CFGI_CALLFORWARD_ANYWAY_ENABLE
#define OEMNV_CALLFORWARD_ANYWAY_DISABLE            "0"                      //CFGI_CALLFORWARD_ANYWAY_DISABLE
#define OEMNV_CALLFORWARD_DISABLE_ALL               "0"                      //CFGI_CALLFORWARD_DISABLE_ALL
#define OEMNV_CALLFORWARD_WAIT_ENABLE               "*40"                       //CFGI_CALLFORWARD_WAIT_ENABLE
#define OEMNV_CALLFORWARD_WAIT_DISABLE              "*400"                      //CFGI_CALLFORWARD_WAIT_DISABLE
#define OEMNV_CALLFORWARD_VOICEMAIL_ENABLE          "0"                         //CFGI_CALLFORWARD_VOICEMAIL_ENABLE 
#define OEMNV_VOICEMAIL_NUMBER                      L"*88"                         //NV_SMS_VM_NUMBER_I 

#elif defined(FEATURE_VERSION_CITYCELL)  //add by yangdecai 2010-10-06
//Operator parameters
#define OEMNV_EMERGENCYNUM_TABLE_NUM                {{{"100",3},{"101",3},{"102",3},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0}},3} //CFGI_EMERGENCYNUM_TABLE
#define OEMNV_CALLFORWARD_BUSY_ENABLE               "*74"                       //CFGI_CALLFORWARD_BUSY_ENABLE
#define OEMNV_CALLFORWARD_BUSY_DISABLE              "*740"                      //CFGI_CALLFORWARD_BUSY_DISABLE
#define OEMNV_CALLFORWARD_NOANSWER_ENABLE           "*71"                       //CFGI_CALLFORWARD_NOANSWER_ENABLE
#define OEMNV_CALLFORWARD_NOANSWER_DISABLE          "*710"                      //CFGI_CALLFORWARD_NOANSWER_DISABLE
#define OEMNV_CALLFORWARD_NOCONNECT_ENABLE          "0"                       //CFGI_CALLFORWARD_NOCONNECT_ENABLE
#define OEMNV_CALLFORWARD_NOCONNECT_DISABLE         "0"                      //CFGI_CALLFORWARD_NOCONNECT_DISABLE
#define OEMNV_CALLFORWARD_ANYWAY_ENABLE             "*72"                       //CFGI_CALLFORWARD_ANYWAY_ENABLE
#define OEMNV_CALLFORWARD_ANYWAY_DISABLE            "*720"                      //CFGI_CALLFORWARD_ANYWAY_DISABLE
#define OEMNV_CALLFORWARD_DISABLE_ALL               "*730"                      //CFGI_CALLFORWARD_DISABLE_ALL
#define OEMNV_CALLFORWARD_WAIT_ENABLE               "*76"                       //CFGI_CALLFORWARD_WAIT_ENABLE
#define OEMNV_CALLFORWARD_WAIT_DISABLE              "*760"                      //CFGI_CALLFORWARD_WAIT_DISABLE
#define OEMNV_CALLFORWARD_VOICEMAIL_ENABLE          "0"                         //CFGI_CALLFORWARD_VOICEMAIL_ENABLE 
#define OEMNV_VOICEMAIL_NUMBER                      L"112"                         //NV_SMS_VM_NUMBER_I 

#elif defined(FEATURE_VERSION_KARBONN)  //add by yangdecai 2010-10-08
//Operator parameters
#define OEMNV_EMERGENCYNUM_TABLE_NUM                {{{"110",3},{"112",3},{"911",3},{"999",3},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0}},4} //CFGI_EMERGENCYNUM_TABLE
#define OEMNV_CALLFORWARD_BUSY_ENABLE               "*75"                       //CFGI_CALLFORWARD_BUSY_ENABLE
#define OEMNV_CALLFORWARD_BUSY_DISABLE              "*65"                      //CFGI_CALLFORWARD_BUSY_DISABLE
#define OEMNV_CALLFORWARD_NOANSWER_ENABLE           "0"                       //CFGI_CALLFORWARD_NOANSWER_ENABLE
#define OEMNV_CALLFORWARD_NOANSWER_DISABLE          "0"                      //CFGI_CALLFORWARD_NOANSWER_DISABLE
#define OEMNV_CALLFORWARD_NOCONNECT_ENABLE          "0"                       //CFGI_CALLFORWARD_NOCONNECT_ENABLE
#define OEMNV_CALLFORWARD_NOCONNECT_DISABLE         "0"                      //CFGI_CALLFORWARD_NOCONNECT_DISABLE
#define OEMNV_CALLFORWARD_ANYWAY_ENABLE             "0"                         //CFGI_CALLFORWARD_ANYWAY_ENABLE
#define OEMNV_CALLFORWARD_ANYWAY_DISABLE            "0"                         //CFGI_CALLFORWARD_ANYWAY_DISABLE
#define OEMNV_CALLFORWARD_DISABLE_ALL               "0"                          //CFGI_CALLFORWARD_DISABLE_ALL
#define OEMNV_CALLFORWARD_WAIT_ENABLE               "*118"                       //CFGI_CALLFORWARD_WAIT_ENABLE
#define OEMNV_CALLFORWARD_WAIT_DISABLE              "*119"                      //CFGI_CALLFORWARD_WAIT_DISABLE
#define OEMNV_CALLFORWARD_VOICEMAIL_ENABLE          "0"                         //CFGI_CALLFORWARD_VOICEMAIL_ENABLE 
#define OEMNV_VOICEMAIL_NUMBER                      L"0"                         //NV_SMS_VM_NUMBER_I 

#elif defined(FEATURE_VERSION_HITZ181)  //add by yangdecai 2010-10-08
//Operator parameters
#define OEMNV_EMERGENCYNUM_TABLE_NUM                {{{"191",3},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0}},1} //CFGI_EMERGENCYNUM_TABLE
#define OEMNV_CALLFORWARD_BUSY_ENABLE               "*90"                       //CFGI_CALLFORWARD_BUSY_ENABLE
#define OEMNV_CALLFORWARD_BUSY_DISABLE              "*900"                      //CFGI_CALLFORWARD_BUSY_DISABLE
#define OEMNV_CALLFORWARD_NOANSWER_ENABLE           "*92"                       //CFGI_CALLFORWARD_NOANSWER_ENABLE
#define OEMNV_CALLFORWARD_NOANSWER_DISABLE          "*920"                      //CFGI_CALLFORWARD_NOANSWER_DISABLE
#define OEMNV_CALLFORWARD_NOCONNECT_ENABLE          "*68"                       //CFGI_CALLFORWARD_NOCONNECT_ENABLE
#define OEMNV_CALLFORWARD_NOCONNECT_DISABLE         "*680"                      //CFGI_CALLFORWARD_NOCONNECT_DISABLE
#define OEMNV_CALLFORWARD_ANYWAY_ENABLE             "*72"                         //CFGI_CALLFORWARD_ANYWAY_ENABLE
#define OEMNV_CALLFORWARD_ANYWAY_DISABLE            "*720"                         //CFGI_CALLFORWARD_ANYWAY_DISABLE
#define OEMNV_CALLFORWARD_DISABLE_ALL               "*720"                          //CFGI_CALLFORWARD_DISABLE_ALL
#define OEMNV_CALLFORWARD_WAIT_ENABLE               "*42"                       //CFGI_CALLFORWARD_WAIT_ENABLE
#define OEMNV_CALLFORWARD_WAIT_DISABLE              "*420"                      //CFGI_CALLFORWARD_WAIT_DISABLE
#define OEMNV_CALLFORWARD_VOICEMAIL_ENABLE          "0"                         //CFGI_CALLFORWARD_VOICEMAIL_ENABLE 
#define OEMNV_VOICEMAIL_NUMBER                      L"91122"                         //NV_SMS_VM_NUMBER_I 

#else
//Operator parameters
#define OEMNV_EMERGENCYNUM_TABLE_NUM                {{{"110",3},{"112",3},{"911",3},{"999",3},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0}},4} //CFGI_EMERGENCYNUM_TABLE
#define OEMNV_CALLFORWARD_BUSY_ENABLE               "*90"                       //CFGI_CALLFORWARD_BUSY_ENABLE
#define OEMNV_CALLFORWARD_BUSY_DISABLE              "*900"                      //CFGI_CALLFORWARD_BUSY_DISABLE
#define OEMNV_CALLFORWARD_NOANSWER_ENABLE           "*92"                       //CFGI_CALLFORWARD_NOANSWER_ENABLE
#define OEMNV_CALLFORWARD_NOANSWER_DISABLE          "*920"                      //CFGI_CALLFORWARD_NOANSWER_DISABLE
#define OEMNV_CALLFORWARD_NOCONNECT_ENABLE          "*68"                       //CFGI_CALLFORWARD_NOCONNECT_ENABLE
#define OEMNV_CALLFORWARD_NOCONNECT_DISABLE         "*680"                      //CFGI_CALLFORWARD_NOCONNECT_DISABLE
#define OEMNV_CALLFORWARD_ANYWAY_ENABLE             "*72"                       //CFGI_CALLFORWARD_ANYWAY_ENABLE
#define OEMNV_CALLFORWARD_ANYWAY_DISABLE            "*720"                      //CFGI_CALLFORWARD_ANYWAY_DISABLE
#define OEMNV_CALLFORWARD_DISABLE_ALL               "*730"                      //CFGI_CALLFORWARD_DISABLE_ALL
#define OEMNV_CALLFORWARD_WAIT_ENABLE               "*74"                       //CFGI_CALLFORWARD_WAIT_ENABLE
#define OEMNV_CALLFORWARD_WAIT_DISABLE              "*740"                      //CFGI_CALLFORWARD_WAIT_DISABLE
#define OEMNV_CALLFORWARD_VOICEMAIL_ENABLE          "0"                         //CFGI_CALLFORWARD_VOICEMAIL_ENABLE 
#define OEMNV_VOICEMAIL_NUMBER                      L"0"                         //NV_SMS_VM_NUMBER_I  
#endif
//Others
#define OEMNV_PHONE_PASSWORD                        10670                       //CFGI_PHONE_PASSWORD       
#define OEMNV_LANGUAGE_DEFULT                       NV_LANGUAGE_ENGLISH         //NV_LANGUAGE_SELECTION_I

