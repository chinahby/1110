

/*---------------------------------------------------------------------------

                    以下为运营商相关的OEM NV的默认值
    
*--------------------------------------------------------------------------*/
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
#define OEMNV_SERVICE_PROVIDER                      {"000","00","0"}            //CFGI_SERVICE_PROVIDER
#define OEMNV_VOICEMAIL_NUMBER                      "0"                         //NV_SMS_VM_NUMBER_I  

//Others
#define OEMNV_PHONE_PASSWORD                        10670                       //CFGI_PHONE_PASSWORD       
#define OEMNV_RTRT_DEFULT_CONFIG                    NV_RTRE_CONFIG_RUIM_ONLY    //NV_RTRE_CONFIG_RUIM_ONLY
#define OEMNV_LANGUAGE_DEFULT                       NV_LANGUAGE_CHINESE         //NV_LANGUAGE_SELECTION_I
#ifdef FEATURE_USB_FUNCTION_SELECT
#define OEMNV_USB_FUNCTION_DEFAULT                  OEMNV_USB_DATA_AND_DIAG     //
#endif //FEATURE_USB_FUNCTION_SELECT
