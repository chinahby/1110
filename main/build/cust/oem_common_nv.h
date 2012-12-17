

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
#define OEMNV_CALLFORWARD_UNCONDITIONAL_ENABLE      "*71"                       //CFGI_CALLFORWARD_UNCONDITIONAL_ENABLE
#define OEMNV_CALLFORWARD_UNCONDITIONAL_DISABLE     "*710"                      //CFGI_CALLFORWARD_UNCONDITIONAL_DISABLE
#define OEMNV_CALLFORWARD_UNREACHABLE_ENABLE        "0"                       //CFGI_CALLFORWARD_UNREACHABLE_ENABLE
#define OEMNV_CALLFORWARD_UNREACHABLE_DISABLE       "0"                      //CFGI_CALLFORWARD_UNREACHABLE_DISABLE
#define OEMNV_CALLFORWARD_DISABLE_ALL               "0"                      //CFGI_CALLFORWARD_DISABLE_ALL
#define OEMNV_CALLFORWARD_WAIT_ENABLE               "*40"                       //CFGI_CALLFORWARD_WAIT_ENABLE
#define OEMNV_CALLFORWARD_WAIT_DISABLE              "*400"                      //CFGI_CALLFORWARD_WAIT_DISABLE
#define OEMNV_CALLFORWARD_VOICEMAIL_ENABLE          "*738"                         //CFGI_CALLFORWARD_VOICEMAIL_ENABLE 
#define OEMNV_VOICEMAIL_NUMBER                      L"388"                         //NV_SMS_VM_NUMBER_I 
#elif defined(FEATURE_VERSION_M8) || defined(FEATURE_VERSION_M8021)
//Operator parameters
#define OEMNV_EMERGENCYNUM_TABLE_NUM                {{{"112",3},{"113",3},{"118",3},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0}},4} //CFGI_EMERGENCYNUM_TABLE
#define OEMNV_CALLFORWARD_BUSY_ENABLE               "*73"                       //CFGI_CALLFORWARD_BUSY_ENABLE
#define OEMNV_CALLFORWARD_BUSY_DISABLE              "*730"                      //CFGI_CALLFORWARD_BUSY_DISABLE
#define OEMNV_CALLFORWARD_NOANSWER_ENABLE           "*72"                       //CFGI_CALLFORWARD_NOANSWER_ENABLE
#define OEMNV_CALLFORWARD_NOANSWER_DISABLE          "*720"                      //CFGI_CALLFORWARD_NOANSWER_DISABLE
#define OEMNV_CALLFORWARD_UNCONDITIONAL_ENABLE      "*71"                       //CFGI_CALLFORWARD_UNCONDITIONAL_ENABLE
#define OEMNV_CALLFORWARD_UNCONDITIONAL_DISABLE     "*710"                      //CFGI_CALLFORWARD_UNCONDITIONAL_DISABLE
#define OEMNV_CALLFORWARD_UNREACHABLE_ENABLE        "*74"                       //CFGI_CALLFORWARD_UNREACHABLE_ENABLE
#define OEMNV_CALLFORWARD_UNREACHABLE_DISABLE       "*740"                      //CFGI_CALLFORWARD_UNREACHABLE_DISABLE
#define OEMNV_CALLFORWARD_DISABLE_ALL               "*740"                      //CFGI_CALLFORWARD_DISABLE_ALL
#define OEMNV_CALLFORWARD_WAIT_ENABLE               "*40"                       //CFGI_CALLFORWARD_WAIT_ENABLE
#define OEMNV_CALLFORWARD_WAIT_DISABLE              "*400"                      //CFGI_CALLFORWARD_WAIT_DISABLE
#define OEMNV_CALLFORWARD_VOICEMAIL_ENABLE          "*52"                         //CFGI_CALLFORWARD_VOICEMAIL_ENABLE 
#define OEMNV_VOICEMAIL_NUMBER                      L"*88"                         //NV_SMS_VM_NUMBER_I 
#elif defined(FEATURE_VERSION_M8P)
//Operator parameters
#define OEMNV_EMERGENCYNUM_TABLE_NUM                {{{"112",3},{"113",3},{"118",3},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0}},4} //CFGI_EMERGENCYNUM_TABLE
#define OEMNV_CALLFORWARD_BUSY_ENABLE               "*73"                       //CFGI_CALLFORWARD_BUSY_ENABLE
#define OEMNV_CALLFORWARD_BUSY_DISABLE              "*730"                      //CFGI_CALLFORWARD_BUSY_DISABLE
#define OEMNV_CALLFORWARD_NOANSWER_ENABLE           "*72"                       //CFGI_CALLFORWARD_NOANSWER_ENABLE
#define OEMNV_CALLFORWARD_NOANSWER_DISABLE          "*720"                      //CFGI_CALLFORWARD_NOANSWER_DISABLE
#define OEMNV_CALLFORWARD_UNCONDITIONAL_ENABLE      "*71"                       //CFGI_CALLFORWARD_UNCONDITIONAL_ENABLE
#define OEMNV_CALLFORWARD_UNCONDITIONAL_DISABLE     "*710"                      //CFGI_CALLFORWARD_UNCONDITIONAL_DISABLE
#define OEMNV_CALLFORWARD_UNREACHABLE_ENABLE        "*74"                       //CFGI_CALLFORWARD_UNREACHABLE_ENABLE
#define OEMNV_CALLFORWARD_UNREACHABLE_DISABLE       "*740"                      //CFGI_CALLFORWARD_UNREACHABLE_DISABLE
#define OEMNV_CALLFORWARD_DISABLE_ALL               "*740"                      //CFGI_CALLFORWARD_DISABLE_ALL
#define OEMNV_CALLFORWARD_WAIT_ENABLE               "*40"                       //CFGI_CALLFORWARD_WAIT_ENABLE
#define OEMNV_CALLFORWARD_WAIT_DISABLE              "*400"                      //CFGI_CALLFORWARD_WAIT_DISABLE
#define OEMNV_CALLFORWARD_VOICEMAIL_ENABLE          "*52"                         //CFGI_CALLFORWARD_VOICEMAIL_ENABLE 
#define OEMNV_VOICEMAIL_NUMBER                      L"*88"                         //NV_SMS_VM_NUMBER_I 
#elif defined(FEATURE_VERSION_CITYCELL)  //add by yangdecai 2010-10-06
//Operator parameters
#define OEMNV_EMERGENCYNUM_TABLE_NUM                {{{"100",3},{"101",3},{"102",3},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0}},3} //CFGI_EMERGENCYNUM_TABLE
#define OEMNV_CALLFORWARD_BUSY_ENABLE               "*74"                       //CFGI_CALLFORWARD_BUSY_ENABLE
#define OEMNV_CALLFORWARD_BUSY_DISABLE              "*740"                      //CFGI_CALLFORWARD_BUSY_DISABLE
#define OEMNV_CALLFORWARD_NOANSWER_ENABLE           "*71"                       //CFGI_CALLFORWARD_NOANSWER_ENABLE
#define OEMNV_CALLFORWARD_NOANSWER_DISABLE          "*710"                      //CFGI_CALLFORWARD_NOANSWER_DISABLE
#define OEMNV_CALLFORWARD_UNCONDITIONAL_ENABLE      "0"                       //CFGI_CALLFORWARD_UNCONDITIONAL_ENABLE
#define OEMNV_CALLFORWARD_UNCONDITIONAL_DISABLE     "0"                      //CFGI_CALLFORWARD_UNCONDITIONAL_DISABLE
#define OEMNV_CALLFORWARD_UNREACHABLE_ENABLE        "*72"                       //CFGI_CALLFORWARD_UNREACHABLE_ENABLE
#define OEMNV_CALLFORWARD_UNREACHABLE_DISABLE       "*720"                      //CFGI_CALLFORWARD_UNREACHABLE_DISABLE
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
#define OEMNV_CALLFORWARD_UNCONDITIONAL_ENABLE      "0"                       //CFGI_CALLFORWARD_UNCONDITIONAL_ENABLE
#define OEMNV_CALLFORWARD_UNCONDITIONAL_DISABLE     "0"                      //CFGI_CALLFORWARD_UNCONDITIONAL_DISABLE
#define OEMNV_CALLFORWARD_UNREACHABLE_ENABLE        "0"                         //CFGI_CALLFORWARD_UNREACHABLE_ENABLE
#define OEMNV_CALLFORWARD_UNREACHABLE_DISABLE       "0"                         //CFGI_CALLFORWARD_UNREACHABLE_DISABLE
#define OEMNV_CALLFORWARD_DISABLE_ALL               "0"                          //CFGI_CALLFORWARD_DISABLE_ALL
#define OEMNV_CALLFORWARD_WAIT_ENABLE               "*118"                       //CFGI_CALLFORWARD_WAIT_ENABLE
#define OEMNV_CALLFORWARD_WAIT_DISABLE              "*119"                      //CFGI_CALLFORWARD_WAIT_DISABLE
#define OEMNV_CALLFORWARD_VOICEMAIL_ENABLE          "0"                         //CFGI_CALLFORWARD_VOICEMAIL_ENABLE 
#define OEMNV_VOICEMAIL_NUMBER                      L"0"                         //NV_SMS_VM_NUMBER_I 

#elif defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)  //add by yangdecai 2010-10-08
//Operator parameters
#define OEMNV_EMERGENCYNUM_TABLE_NUM                {{{"191",3},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0}},1} //CFGI_EMERGENCYNUM_TABLE
#define OEMNV_CALLFORWARD_BUSY_ENABLE               "*90"                       //CFGI_CALLFORWARD_BUSY_ENABLE
#define OEMNV_CALLFORWARD_BUSY_DISABLE              "*900"                      //CFGI_CALLFORWARD_BUSY_DISABLE
#define OEMNV_CALLFORWARD_NOANSWER_ENABLE           "*92"                       //CFGI_CALLFORWARD_NOANSWER_ENABLE
#define OEMNV_CALLFORWARD_NOANSWER_DISABLE          "*920"                      //CFGI_CALLFORWARD_NOANSWER_DISABLE
#define OEMNV_CALLFORWARD_UNCONDITIONAL_ENABLE      "*68"                       //CFGI_CALLFORWARD_UNCONDITIONAL_ENABLE
#define OEMNV_CALLFORWARD_UNCONDITIONAL_DISABLE     "*680"                      //CFGI_CALLFORWARD_UNCONDITIONAL_DISABLE
#define OEMNV_CALLFORWARD_UNREACHABLE_ENABLE        "*72"                         //CFGI_CALLFORWARD_UNREACHABLE_ENABLE
#define OEMNV_CALLFORWARD_UNREACHABLE_DISABLE       "*720"                         //CFGI_CALLFORWARD_UNREACHABLE_DISABLE
#define OEMNV_CALLFORWARD_DISABLE_ALL               "*720"                          //CFGI_CALLFORWARD_DISABLE_ALL
#define OEMNV_CALLFORWARD_WAIT_ENABLE               "*42"                       //CFGI_CALLFORWARD_WAIT_ENABLE
#define OEMNV_CALLFORWARD_WAIT_DISABLE              "*420"                      //CFGI_CALLFORWARD_WAIT_DISABLE
#define OEMNV_CALLFORWARD_VOICEMAIL_ENABLE          "0"                         //CFGI_CALLFORWARD_VOICEMAIL_ENABLE 
#define OEMNV_VOICEMAIL_NUMBER                      L"91122"                         //NV_SMS_VM_NUMBER_I 

#elif defined(FEATURE_VERSION_W208S)//Add by zzg 2012-03-08
//Operator parameters
#define OEMNV_EMERGENCYNUM_TABLE_NUM                {{{"*1",2},{"911",3},{"*171",4},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0},{"",0}},3} //CFGI_EMERGENCYNUM_TABLE
#define OEMNV_CALLFORWARD_BUSY_ENABLE               "*90"                       //CFGI_CALLFORWARD_BUSY_ENABLE
#define OEMNV_CALLFORWARD_BUSY_DISABLE              "*900"                      //CFGI_CALLFORWARD_BUSY_DISABLE
#define OEMNV_CALLFORWARD_NOANSWER_ENABLE           "*92"                       //CFGI_CALLFORWARD_NOANSWER_ENABLE
#define OEMNV_CALLFORWARD_NOANSWER_DISABLE          "*920"                      //CFGI_CALLFORWARD_NOANSWER_DISABLE
#define OEMNV_CALLFORWARD_UNCONDITIONAL_ENABLE      "*68"                       //CFGI_CALLFORWARD_UNCONDITIONAL_ENABLE
#define OEMNV_CALLFORWARD_UNCONDITIONAL_DISABLE     "*680"                      //CFGI_CALLFORWARD_UNCONDITIONAL_DISABLE
#define OEMNV_CALLFORWARD_UNREACHABLE_ENABLE        "*72"                         //CFGI_CALLFORWARD_UNREACHABLE_ENABLE
#define OEMNV_CALLFORWARD_UNREACHABLE_DISABLE       "*720"                         //CFGI_CALLFORWARD_UNREACHABLE_DISABLE
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
#define OEMNV_CALLFORWARD_UNCONDITIONAL_ENABLE      "*68"                       //CFGI_CALLFORWARD_UNCONDITIONAL_ENABLE
#define OEMNV_CALLFORWARD_UNCONDITIONAL_DISABLE     "*680"                      //CFGI_CALLFORWARD_UNCONDITIONAL_DISABLE
#define OEMNV_CALLFORWARD_UNREACHABLE_ENABLE        "*72"                       //CFGI_CALLFORWARD_UNREACHABLE_ENABLE
#define OEMNV_CALLFORWARD_UNREACHABLE_DISABLE       "*720"                      //CFGI_CALLFORWARD_UNREACHABLE_DISABLE
#define OEMNV_CALLFORWARD_DISABLE_ALL               "*730"                      //CFGI_CALLFORWARD_DISABLE_ALL
#define OEMNV_CALLFORWARD_WAIT_ENABLE               "*74"                       //CFGI_CALLFORWARD_WAIT_ENABLE
#define OEMNV_CALLFORWARD_WAIT_DISABLE              "*740"                      //CFGI_CALLFORWARD_WAIT_DISABLE
#define OEMNV_CALLFORWARD_VOICEMAIL_ENABLE          "0"                         //CFGI_CALLFORWARD_VOICEMAIL_ENABLE 
#ifdef FEATURE_VERSION_W208S
#define OEMNV_VOICEMAIL_NUMBER                      L"*9"                         //NV_SMS_VM_NUMBER_I  
#else
#define OEMNV_VOICEMAIL_NUMBER                      L"0"                         //NV_SMS_VM_NUMBER_I  
#endif
#endif
//Others
#if defined(FEATURE_VERSION_W515V3) || defined(FEATURE_VERSION_S1000T)
#define OEMNV_PHONE_PASSWORD                        11005                       //CFGI_PHONE_PASSWORD
#elif defined (FEATURE_VERSION_W317A)
#define OEMNV_PHONE_PASSWORD                        11234                    
#elif defined (FEATURE_VERSION_W208S)||defined(FEATURE_VERSION_C316)
#define OEMNV_PHONE_PASSWORD                        14321                    //10670    
#else
#define OEMNV_PHONE_PASSWORD                        10670                    //10670                       //CFGI_PHONE_PASSWORD
#endif
#if defined(FEATURE_LANG_DEFAULT_ENGLISH)
#define OEMNV_LANGUAGE_DEFULT                       NV_LANGUAGE_ENGLISH         //NV_LANGUAGE_SELECTION_I
#elif defined(FEATURE_LANG_CHINESE)
#ifdef FEATURE_VERSION_X3
#define OEMNV_LANGUAGE_DEFULT                       NV_LANGUAGE_ENGLISH         //简体中文
#else
#define OEMNV_LANGUAGE_DEFULT                       NV_LANGUAGE_CHINESE         //简体中文
#endif
#elif defined(FEATURE_LANG_TCHINESE)
#define OEMNV_LANGUAGE_DEFULT                       NV_LANGUAGE_TCHINESE        //繁体中文
#elif defined(FEATURE_LANG_ITALIAN)
#define OEMNV_LANGUAGE_DEFULT                       NV_LANGUAGE_ITALIAN         //意大利文
#elif defined(FEATURE_LANG_SPANISH)
#define OEMNV_LANGUAGE_DEFULT                       NV_LANGUAGE_SPANISH         //西班牙文
#elif defined(FEATURE_LANG_PORTUGUESE)
#define OEMNV_LANGUAGE_DEFULT                       NV_LANGUAGE_PORTUGUESE      //葡萄牙文
#elif defined(FEATURE_LANG_INDONESIAN)
#define OEMNV_LANGUAGE_DEFULT                       NV_LANGUAGE_INDONESIAN      //印度尼西亚
#elif defined(FEATURE_LANG_HINDI)
#define OEMNV_LANGUAGE_DEFULT                       NV_LANGUAGE_HINDI           //印度语
#elif defined(FEATURE_LANG_ARABIC)
#ifdef FEATURE_VERSION_N021
#define OEMNV_LANGUAGE_DEFULT                       NV_LANGUAGE_ENGLISH         //NV_LANGUAGE_SELECTION_I
#else
#define OEMNV_LANGUAGE_DEFULT                       NV_LANGUAGE_ARABIC          //阿拉伯语
#endif
#elif defined(FEATURE_LANG_HEBREW)
#define OEMNV_LANGUAGE_DEFULT                       NV_LANGUAGE_HEBREW          //希伯来语
#ifndef FEATURE_VERSION_VG68
#elif defined(FEATURE_LANG_THAI)
#ifdef FEATURE_USES_LOWMEM
#define OEMNV_LANGUAGE_DEFULT                       NV_LANGUAGE_ENGLISH            //泰国语
#else
#define OEMNV_LANGUAGE_DEFULT                       NV_LANGUAGE_THAI            //泰国语
#endif
#endif
#elif defined(FEATURE_LANG_VIETNAMESE)
#define OEMNV_LANGUAGE_DEFULT                       NV_LANGUAGE_VIETNAMESE      //越南语
#elif defined(FEATURE_LANG_RUSSIAN)
#define OEMNV_LANGUAGE_DEFULT                       NV_LANGUAGE_RUSSIAN         //俄文
#elif defined(FEATURE_LANG_FRENCH)
#define OEMNV_LANGUAGE_DEFULT                       NV_LANGUAGE_FRENCH          //法文
#elif defined(FEATURE_LANG_MYANMAR)
#define OEMNV_LANGUAGE_DEFULT                       NV_LANGUAGE_MYANMAR         //缅甸文
#else
#define OEMNV_LANGUAGE_DEFULT                       NV_LANGUAGE_ENGLISH         //NV_LANGUAGE_SELECTION_I
#endif
#define OEMNV_TV_TvSetting                          {0,0,0,0,0,0,0,{{{0},{0}},{{0},{0}}},0,{0}}

