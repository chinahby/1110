/*----------------------------------------------------------------
        本文件包含对于特定机型的通用校准参数，在文件系统重建
        时可以自动写入到NV中。
        
        请根据机型的具体情况改写本文件中的列表。
        
        一般的修改的修改方法：
        1.取10台工作良好的手机，计算Calibration Nv的均值
        2.通过QPST的工具设置好需要的NV值，保存为QCN文件。
        3.使用QCN View转为txt文件。
        4.将其中的值复制到本表中。
----------------------------------------------------------------*/
#define AUTO_NV_RF_VERSION      0x21

//Fixed rf nv, it'may adjust for certain target.
const nv_auto_set_type nv_auto_set_rf_fixed_list[] =
{ 
    {NV_RF_CONFIG_I,                        {0x01}}, // Any
    {NV_RF_HW_CONFIG_I,                     {0x26}}, // For 1100 and 1110
    {NV_RF_BC_CONFIG_I,                     {0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, //BC0 Only
    {NV_FTM_MODE_I,                         {0x00}}, //FTM Mode,
    {NV_CDMA_DYNAMIC_RANGE_I,               {0x00, 0x04}},    
    {NV_CDMA_MIN_RX_RSSI_I,                 {0x82, 0xFB}},
    {NV_BC0_ENC_BTF_I,                      {0x9f, 0x0c}},
    
    // RX
    {NV_C0_BC0_RX_CAL_CHAN_I,               {0xfa, 0x03, 0x2e, 0x00, 0x62, 0x00, 0x96, 0x00, 0xca, 0x00, 0xfe, 0x00, 0x32, 0x01, 0x66, 0x01,
                                             0x9a, 0x01, 0xce, 0x01, 0x02, 0x02, 0x36, 0x02, 0x6a, 0x02, 0x9e, 0x02, 0xd2, 0x02, 0x06, 0x03}},
    {NV_C0_BC0_RX_CAL_CHAN_LRU_I,           {0x10, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01}},
    {NV_C0_BC0_LNA_SWITCHPOINTS_LPM_1_I,    {0xc3, 0xd6, 0x23, 0x7f, 0xbb, 0xce, 0x19, 0x7f}},
    {NV_C0_BC0_INTELLICEIVER_DET_THRESH_I,  {0x00, 0xc4, 0xc4, 0xc4, 0xc4, 0x00, 0xe2, 0xe2, 0xe2, 0xe2}},
    {NV_C0_BC0_LNA_1_RISE_I,                {0xc3}},
    {NV_C0_BC0_LNA_2_RISE_I,                {0xbb}},
    {NV_C0_BC0_LNA_3_RISE_I,                {0x2a}},
    {NV_C0_BC0_LNA_4_RISE_I,                {0x7f}},
    {NV_C0_BC0_LNA_1_FALL_I,                {0xbb}},
    {NV_C0_BC0_LNA_2_FALL_I,                {0xd3}},
    {NV_C0_BC0_LNA_3_FALL_I,                {0x20}},
    {NV_C0_BC0_LNA_4_FALL_I,                {0x7f}},
    {NV_C0_BC0_IM_LEVEL1_I,                 {0xc3}},
    {NV_C0_BC0_IM_LEVEL2_I,                 {0xdd}},
    {NV_C0_BC0_IM_LEVEL3_I,                 {0x2a}},
    {NV_C0_BC0_IM_LEVEL4_I,                 {0x7f}},
    
    // TX
    {NV_BC0_PA_R_MAP_I,                     {0xfe}},
    {NV_C0_BC0_TX_CAL_CHAN_I,               {0xfa, 0x03, 0x2e, 0x00, 0x62, 0x00, 0x96, 0x00, 0xca, 0x00, 0xfe, 0x00, 0x32, 0x01, 0x66, 0x01,
                                             0x9a, 0x01, 0xce, 0x01, 0x02, 0x02, 0x36, 0x02, 0x6a, 0x02, 0x9e, 0x02, 0xd2, 0x02, 0x06, 0x03}},
    {NV_BC0_PA_R1_RISE_I,                   {0xb0}},
    {NV_BC0_PA_R2_RISE_I,                   {0xff}},
    {NV_BC0_PA_R3_RISE_I,                   {0xff}},
    {NV_BC0_PA_R1_FALL_I,                   {0xab}},
    {NV_BC0_PA_R2_FALL_I,                   {0xff}},
    {NV_BC0_PA_R3_FALL_I,                   {0xff}}
};

//Cal rf nv which must adjust for every Model.
//---------------------------------------------------
//NOTICE:Please get at least 10 calibration Value 
//for 1 Model then set it's average value here.
//---------------------------------------------------
const nv_auto_set_type nv_auto_set_rf_cal_list[] =
{    
    {NV_VBATT_I,                            {0x85, 0xab}},
    {NV_THERM_BINS_I,                       {0xa1, 0x06, 0x01, 0x07, 0x60, 0x07, 0xc0, 0x07, 0x1f, 0x08, 0x7f, 0x08, 0xdf, 0x08, 0x3e, 0x09}},
    
    // RX
    {NV_C0_BC0_VGA_GAIN_OFFSET_VS_FREQ_I,   {0xdf, 0xdb, 0xdd, 0xe0, 0xe4, 0xe7, 0xe6, 0xe2, 0xdc, 0xd7, 0xd4, 0xd1, 0xd1, 0xd4, 0xd9, 0xde,
                                             0x29, 0x26, 0x28, 0x2a, 0x2d, 0x2f, 0x2c, 0x29, 0x25, 0x21, 0x1e, 0x1d, 0x1c, 0x1f, 0x24, 0x26}},
    {NV_C0_BC0_VGA_GAIN_OFFSET_I,           {0xac, 0xff}},
    {NV_C0_BC0_LNA_1_OFFSET_VS_FREQ_I,      {0xf8, 0xf6, 0xf5, 0xf6, 0xf9, 0xfd, 0xff, 0xff, 0xfe, 0xfd, 0xfb, 0xf9, 0xf8, 0xf9, 0xf9, 0xfb,
                                             0x05, 0x03, 0x03, 0x03, 0x04, 0x07, 0x08, 0x09, 0x09, 0x0a, 0x08, 0x08, 0x06, 0x06, 0x06, 0x06}},
    {NV_C0_BC0_LNA_2_OFFSET_VS_FREQ_I,      {0xf7, 0xf3, 0xf5, 0xf8, 0xfd, 0x00, 0x02, 0x00, 0xfc, 0xfa, 0xf7, 0xf2, 0xf3, 0xf4, 0xf6, 0xfc,
                                             0x07, 0x04, 0x05, 0x07, 0x0a, 0x0e, 0x0d, 0x0c, 0x0a, 0x09, 0x06, 0x04, 0x02, 0x04, 0x07, 0x09}},
    {NV_C0_BC0_LNA_3_OFFSET_VS_FREQ_I,      {0xf8, 0xf4, 0xf5, 0xf9, 0xfc, 0x00, 0x01, 0xfe, 0xfa, 0xf8, 0xf4, 0xf2, 0xf1, 0xf3, 0xf5, 0xfb,
                                             0x08, 0x06, 0x06, 0x09, 0x0b, 0x0e, 0x0d, 0x0c, 0x0a, 0x09, 0x06, 0x04, 0x03, 0x05, 0x07, 0x09}},
    {NV_C0_BC0_LNA_4_OFFSET_VS_FREQ_I,      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {NV_C0_BC0_LNA_1_OFFSET_I,              {0x72, 0x00}},
    {NV_C0_BC0_LNA_2_OFFSET_I,              {0xca, 0x00}},
    {NV_C0_BC0_LNA_3_OFFSET_I,              {0x57, 0x01}},
    {NV_C0_BC0_LNA_4_OFFSET_I,              {0x00, 0x00}},
    {NV_C0_BC0_INTELLICEIVER_CAL_I,         {0x66, 0x01, 0xdc, 0x00, 0x08, 0x00, 0xaa, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00}},
    
    // TX
    {NV_BC0_TX_PDM_0_I,                     {0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 0x00, 0x05, 0x00, 0x06, 0x00, 0x07, 0x00,
                                             0x08, 0x00, 0x09, 0x00, 0x0a, 0x00, 0x0b, 0x00, 0x0c, 0x00, 0x0d, 0x00, 0x0e, 0x00, 0x0f, 0x00,
                                             0x10, 0x00, 0x11, 0x00, 0x12, 0x00, 0x13, 0x00, 0x14, 0x00, 0x15, 0x00, 0x16, 0x00, 0x17, 0x00,
                                             0x18, 0x00, 0x19, 0x00, 0x1a, 0x00, 0x1b, 0x00, 0x1c, 0x00, 0x1d, 0x00, 0x1e, 0x00, 0x1f, 0x00,
                                             0x20, 0x00, 0x21, 0x00, 0x22, 0x00, 0x23, 0x00, 0x24, 0x00, 0x25, 0x00, 0x26, 0x00, 0x27, 0x00,
                                             0x28, 0x00, 0x29, 0x00, 0x2a, 0x00, 0x2b, 0x00, 0x2c, 0x00, 0x2d, 0x00, 0x2e, 0x00, 0x2f, 0x00,
                                             0x30, 0x00, 0x31, 0x00, 0x32, 0x00, 0x33, 0x00, 0x34, 0x00, 0x35, 0x00, 0x36, 0x00, 0x37, 0x00,
                                             0x38, 0x00, 0x39, 0x00, 0x3a, 0x00, 0x3b, 0x00, 0xff, 0x7f, 0xff, 0x7f, 0xff, 0x7f, 0xff, 0x7f}},
    {NV_BC0_TX_PDM_1_I,                     {0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 0x00, 0x05, 0x00, 0x06, 0x00, 0x07, 0x00,
                                             0x08, 0x00, 0x09, 0x00, 0x0a, 0x00, 0x0b, 0x00, 0x0c, 0x00, 0x0d, 0x00, 0x0e, 0x00, 0x0f, 0x00,
                                             0x10, 0x00, 0x11, 0x00, 0x12, 0x00, 0x13, 0x00, 0x14, 0x00, 0x15, 0x00, 0x16, 0x00, 0x17, 0x00,
                                             0x18, 0x00, 0x19, 0x00, 0x1a, 0x00, 0x1b, 0x00, 0x1c, 0x00, 0x1d, 0x00, 0x1e, 0x00, 0x1f, 0x00,
                                             0x20, 0x00, 0x21, 0x00, 0x22, 0x00, 0x23, 0x00, 0x24, 0x00, 0x25, 0x00, 0x26, 0x00, 0x27, 0x00,
                                             0x28, 0x00, 0x29, 0x00, 0x2a, 0x00, 0x2b, 0x00, 0x2c, 0x00, 0x2d, 0x00, 0x2e, 0x00, 0x2f, 0x00,
                                             0x30, 0x00, 0x31, 0x00, 0x32, 0x00, 0x33, 0x00, 0x34, 0x00, 0x35, 0x00, 0x36, 0x00, 0x37, 0x00,
                                             0x38, 0x00, 0x39, 0x00, 0x3a, 0x00, 0x3b, 0x00, 0x3c, 0x00, 0x3d, 0x00, 0x3e, 0x00, 0x3f, 0x00}},
    {NV_BC0_TX_LIN_0_I,                     {0x3c, 0x00, 0x34, 0x00, 0x29, 0x00, 0x20, 0x00, 0x15, 0x00, 0x11, 0x00, 0x06, 0x00, 0xfb, 0xff,
                                             0xf3, 0xff, 0xe6, 0xff, 0xdb, 0xff, 0xcf, 0xff, 0xc3, 0xff, 0xb6, 0xff, 0xab, 0xff, 0x9f, 0xff,
                                             0x92, 0xff, 0x87, 0xff, 0x7b, 0xff, 0x6d, 0xff, 0x61, 0xff, 0x56, 0xff, 0x4a, 0xff, 0x3e, 0xff,
                                             0x31, 0xff, 0x26, 0xff, 0x19, 0xff, 0x0b, 0xff, 0x00, 0xff, 0xf4, 0xfe, 0xe8, 0xfe, 0xdb, 0xfe,
                                             0xcf, 0xfe, 0xc3, 0xfe, 0xb7, 0xfe, 0xab, 0xfe, 0xa1, 0xfe, 0x94, 0xfe, 0x89, 0xfe, 0x7e, 0xfe,
                                             0x73, 0xfe, 0x65, 0xfe, 0x59, 0xfe, 0x4d, 0xfe, 0x44, 0xfe, 0x38, 0xfe, 0x2a, 0xfe, 0x1f, 0xfe,
                                             0x14, 0xfe, 0x06, 0xfe, 0xfb, 0xfd, 0xee, 0xfd, 0xe1, 0xfd, 0xd5, 0xfd, 0xd0, 0xfd, 0xc7, 0xfd,
                                             0xbb, 0xfd, 0xae, 0xfd, 0xa3, 0xfd, 0xa2, 0xfd, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80}},
    {NV_BC0_TX_LIN_1_I,                     {0x27, 0x01, 0x22, 0x01, 0x20, 0x01, 0x1c, 0x01, 0x16, 0x01, 0x10, 0x01, 0x0c, 0x01, 0x06, 0x01,
                                             0xff, 0x00, 0xfa, 0x00, 0xf1, 0x00, 0xe9, 0x00, 0xe1, 0x00, 0xdb, 0x00, 0xd2, 0x00, 0xc9, 0x00,
                                             0xc0, 0x00, 0xbc, 0x00, 0xb3, 0x00, 0xac, 0x00, 0xa5, 0x00, 0x9c, 0x00, 0x94, 0x00, 0x8b, 0x00,
                                             0x85, 0x00, 0x7b, 0x00, 0x73, 0x00, 0x69, 0x00, 0x61, 0x00, 0x59, 0x00, 0x52, 0x00, 0x47, 0x00,
                                             0x3f, 0x00, 0x36, 0x00, 0x2c, 0x00, 0x26, 0x00, 0x1d, 0x00, 0x15, 0x00, 0x0d, 0x00, 0x03, 0x00,
                                             0xfc, 0xff, 0xf3, 0xff, 0xe9, 0xff, 0xe2, 0xff, 0xda, 0xff, 0xd2, 0xff, 0xca, 0xff, 0xc1, 0xff,
                                             0xb8, 0xff, 0xb1, 0xff, 0xa9, 0xff, 0x9f, 0xff, 0x97, 0xff, 0x8d, 0xff, 0x85, 0xff, 0x7c, 0xff,
                                             0x74, 0xff, 0x6b, 0xff, 0x65, 0xff, 0x5c, 0xff, 0x53, 0xff, 0x49, 0xff, 0x43, 0xff, 0x3a, 0xff}},
    {NV_BC0_TX_PWR_COMP0_I,                 {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                             0xfc, 0xfe, 0x00, 0x02, 0x04, 0x04, 0x01, 0x00, 0xfd, 0xfb, 0xfd, 0xfb, 0xfb, 0xfd, 0xfd, 0xfd}},
    {NV_BC0_TX_PWR_COMP1_I,                 {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                             0x06, 0x0a, 0x0c, 0x0e, 0x0c, 0x07, 0x02, 0x00, 0xfe, 0xfe, 0x03, 0x05, 0x0a, 0x0c, 0x0b, 0x0a}},
    {NV_BC0_TX_LIM_VS_FREQ_I,               {0x01, 0xfe, 0xfc, 0xfa, 0xf9, 0xfa, 0xfe, 0x00, 0x01, 0x01, 0xfe, 0xfd, 0xfa, 0xfb, 0xfa, 0xfa}},
    {NV_BC0_EXP_HDET_VS_AGC_V2_I,           {0x8b, 0x02, 0x35, 0x03, 0x15, 0x04, 0x71, 0x05, 0x3c, 0x07, 0xfb, 0x09, 0xe6, 0x0d, 0xff, 0x0f,
                                             0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f, 0xff, 0x0f}},
    {NV_BC0_HDET_OFF_V2_I,                  {0x4f, 0x04}},
    {NV_BC0_HDET_SPN_V2_I,                  {0xb0, 0x0b}}
};

