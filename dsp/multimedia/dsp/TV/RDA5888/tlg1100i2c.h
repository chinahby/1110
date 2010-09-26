#ifndef  __TLG1100I2C_H_
#define __TLG1100I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

int TLGI2C_WriteReg(unsigned char dab, unsigned short rab, unsigned short data);
int TLGI2C_ReadReg(unsigned char dab, unsigned short rab, unsigned short *data);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __TLG1100I2C_H_

