#include	"a800_sd.h"
#include	"A8_sd.h"
#include	"A8_common.h"

u_short	A800_CheckSDDevice(void)
{
	return	A8L_CheckSDDevice();
}

u_int	A800_GetSDSize(void)
{
	return	A8L_GetSDSize();
}

u_short	A800_ReadSD(u_int startsect, u_int offset, u_char* buf, u_int read_size)
{
	return	A8L_ReadSD( startsect, offset, buf, read_size);
}

u_short	A800_WriteSD(u_int startsect, u_int offset, u_char* buf, u_int write_size)
{
	return	A8L_WriteSD( startsect, offset, buf, write_size);
}