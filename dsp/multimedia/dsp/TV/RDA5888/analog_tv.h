
#ifndef _ANALOG_TV_H
#define _ANALOG_TV_H

/*GPIO for analog_tv power interface*/
#define ATV_POWER_PIN		31 // connect to RDA5888 PDN pin

/*GPIO for analog_tv I2C interface*/
#define ATV_I2C_SDA_PIN    	46
#define ATV_I2C_SCL_PIN		45

#define   setup_sda_pin()  	GPIO_ModeSetup(ATV_I2C_SDA_PIN,0)
#define   setup_scl_pin()	GPIO_ModeSetup(ATV_I2C_SCL_PIN,0)

#define ATV_APP_MODULE  	1
#define CAMERA_APP_MODULE   0

#define ATV_STATE  	1
#define FM_STATE    0

// tv area definition
typedef enum {
    TV_AREA_CHINA 				= 0,  
	TV_AREA_CHINA_SHENZHEN,
	TV_AREA_CHINA_TAIWAN, 
	TV_AREA_WESTERNEUROP,
	TV_AREA_AMERICA,
	TV_AREA_CHINA_MACAO,
	TV_AREA_INDIA,
	TV_AREA_PAKISTAN,
	TV_AREA_INDONESIA,
	TV_AREA_CHINA_HONGKONG,
	TV_AREA_YEMEN,  // 10
	TV_AREA_BAHRAIN,
	TV_AREA_ABU_DHABI,
	TV_AREA_KUWAIT,
	TV_AREA_THAI,
	TV_AREA_PORTUGAL,
	TV_AREA_SPAIN,
	TV_AREA_PHILIPINES,
	TV_AREA_MALAYSIA,
	TV_AREA_VIETNAM,
	TV_AREA_BRAZIL,  // 20
	TV_AREA_UK,
	TV_AREA_SOUTH_AFRICA,
	TV_AREA_TURKEY,
	TV_AREA_SINGAPORE,
	TV_AREA_CAMBODIA,
	TV_AREA_LAOS,
	TV_AREA_AFGHANISTA,
	TV_AREA_CANADA,
	TV_AREA_KOREA,
	TV_AREA_MEXICO,   // 30
	TV_AREA_CHILE,
	TV_AREA_VENEZUELA,
	TV_AREA_JAPAN,
	TV_AREA_ARGENTINA,
	TV_AREA_BURMA,	  // 35 
	TV_AREA_RUSSIA,
	
	TV_AREA_FACTORYMODE,
    TLG_VSTD_NUM
} TV_AREA;

typedef enum
{
	ATV_POWER_OFF,
	ATV_POWER_ON
} ATV_POWER_STATE;

/*functions for MMI/ISP/MEDIA*/
extern void 			analogtv_sensor_func_config(void);
extern void 			analogtv_module_power_on(unsigned char on);
extern unsigned char 	analogtv_get_curmodule(void);
extern void 			analogtv_set_curmodule(unsigned char is_atv_app);
extern void		       Autoscan_ATV_Channel(void);
extern 	unsigned char  Get_TLG1100_State(void);
extern 	void  Set_TLG1100_State(unsigned char state);
extern unsigned short analogtv_get_chn_count(void);

#endif /* _ANALOG_TV_H */
