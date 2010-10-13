#ifndef __DISP_ST7735R_128x128_H__
#define __DISP_ST7735R_128x128_H__




#define ST7735R_128x128_DISPLAY_ON_C  0x29
#define ST7735R_128x128_DISPLAY_OFF_C  0x28
#define ST7735R_128x128_SET_START_ADDRESS_C   0x2a
#define ST7735R_128x128_SET_END_ADDRESS_C   0x2b
#define ST7735R_128x128_RAM_WRITE_C 0x2c
#define ST7735R_128x128_SOFT_RESET_C 0x01

#define PEGS_TM_EBI2_ID  0xDC01
#define LCD_PRIMARY 0
#define PEGS_TM_MAX_DISPLAYS   2


#define LCD_CMD_WH    EBI2_LCD_BASE           // command register for primary LCD port
#define LCD_DATA_WH   EBI2_LCD_BASE+0x100000  // data register for primary LCD port

typedef enum {
   EBI2_WIDTH_8,                       // 8 bits
   EBI2_WIDTH_16,
   EBI2_WIDTH_18,
   EBI2_WIDTH_2_x_12,                  // 24 bits in 2 12 bit writes.
   EBI2_WIDTH_24,
   EBI2_WIDTH_2_PLUS_16,               // 18 bits in 2 writes (2 bits then 16)
   EBI2_WIDTH_16_PLUS_2,               // 18 bits in 2 writes (16 bits then 2)
   EBI2_WIDTH_2_x_9,                   // 18 bits in 2 writes (9 bits then 9)
   EBI2_WIDTH_INVALID
} lcd_ebi2_width_type;

typedef enum
{
   pixel_format_rgb565,
   pixel_format_rgb666,
   pixel_format_rgb888,
   pixel_format_rgb666_unpacked32, // Least-sig 18 bits of uint32 stores one pixel
   pixel_format_count
} pixel_format_type;



#endif










