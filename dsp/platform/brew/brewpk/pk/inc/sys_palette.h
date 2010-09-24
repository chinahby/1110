#include "disp_options.h"

/*
 * This file contains a macro to lookup the index into the system
 * palette that corresponds most closely to the given RGB triplet.
 */
#ifndef SYS_PALETTE_H
#define SYS_PALETTE_H

extern const unsigned long int system_palette[];
extern uint16 gPalette8To16Tbl[256];
#ifdef DISP_DEVICE_16BPP
extern uint8  roundTo5[256];
extern uint8  roundTo6[256];
#elif defined DISP_DEVICE_18BPP
extern uint8  roundTo6[256];
#elif defined DISP_DEVICE_8BPP
extern uint8  roundTo2[256];
extern uint8  roundTo3[256];
#endif /* DISP_DEVICE_8BPP */

#endif /* SYS_PALETTE_H */
