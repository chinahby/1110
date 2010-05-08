#ifndef _DRV_SD_H_
#define _DRV_SD_H_

#define drv_readsector sd_readsector
#define drv_writesector sd_writesector
#define drv_readmultisector sd_readmultisector
#define drv_writemultisector sd_writemultisector
#define drv_getstatus sd_getstatus
#define drv_initfunc sd_initfunc
#define drv_getphy sd_getphy

unsigned char sd_initfunc(void); 
unsigned char sd_getphy(F_PHY *phy); // success:0
unsigned char sd_readsector(void *data, unsigned long sector); //success : 0 or fail
unsigned char sd_writesector(void *data, unsigned long sector); //success:0 or fail
unsigned char sd_readmultisector(void * data,unsigned long sector,unsigned long count);
unsigned char sd_writemultisector(void * data,unsigned long sector,unsigned long count);
unsigned char sd_getstatus(void); //success:0  F_ST_Missing: Changed:

#endif /* _DRV_SD_H_ */
