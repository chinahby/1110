#ifndef	A800_SD_H
#define	A800_SD_H

#include	"A8_common.h"

extern u_short A800_CheckSDDevice(void);
extern u_int	A800_GetSDSize(void);
extern u_short A800_ReadSD(u_int startsect, u_int offset, u_char* buf, u_int read_size);
extern u_short A800_WriteSD(u_int startsect, u_int offset, u_char* buf, u_int write_size);
#endif