/**
 * @file A8_win.h
 * @brief Copyright (C) 2002-2008 Alpha Imaging Technology Corp.
 * @n AIT701/703 Series Host API.
 * @n The purpose of AIT701 Host API is for Host CPU to control and communicate AIT701
 *      chip via direct/indirect parallel interface.
 * @note N/A
 * @bug N/A
 */
#if !defined (AIT_WIN_H)
#define AIT_WIN_H

typedef struct
{
    u_short		width;
    u_short		height;
    u_short		colordepth;
    u_int		baseaddr;	
} WINDOW_INFO;

// Registers for Game Palette
//
#define GAME_PALETTE_REG_BASE             (0x5400)
#define GAME_PALETTE_B0_RG                (0x000+GAME_PALETTE_REG_BASE)
#define GAME_PALETTE_G0_RG                (0x200+GAME_PALETTE_REG_BASE)
#define GAME_PALETTE_R0_RG                (0x400+GAME_PALETTE_REG_BASE)

#define MAIN_PALETTE_REG_BASE             (0x5400)
#define MAIN_PALETTE_B0_RG                (0x000+MAIN_PALETTE_REG_BASE)
#define MAIN_PALETTE_G0_RG                (0x200+MAIN_PALETTE_REG_BASE)
#define MAIN_PALETTE_R0_RG                (0x400+MAIN_PALETTE_REG_BASE)

#define PIP_PALETTE_REG_BASE              (0x5400)
#define PIP_PALETTE_B0_RG                 (0x000+PIP_PALETTE_REG_BASE)
#define PIP_PALETTE_G0_RG                 (0x040+PIP_PALETTE_REG_BASE)
#define PIP_PALETTE_R0_RG                 (0x0C0+PIP_PALETTE_REG_BASE)

#define OVERLAY_PALETTE_REG_BASE          (0x5400)
#define OVERLAY_PALETTE_B0_RG             (0x000+OVERLAY_PALETTE_REG_BASE)
#define OVERLAY_PALETTE_G0_RG             (0x200+OVERLAY_PALETTE_REG_BASE)
#define OVERLAY_PALETTE_R0_RG             (0x400+OVERLAY_PALETTE_REG_BASE)


// Registers for Window controller
//
#define DISPLAY_SETTING_REG_BASE          (0x5000)

#define DISP_GENERAL_CONTROL1_RG          (0x040 + DISPLAY_SETTING_REG_BASE)
#define DISP_GENERAL_CONTROL2_RG          (0x041 + DISPLAY_SETTING_REG_BASE)
#define DISP_GENERAL_CONTROL3_RG          (0x042 + DISPLAY_SETTING_REG_BASE)
#define DISP_WIN_PRIORITY_RG              (0x043 + DISPLAY_SETTING_REG_BASE)
#define DISP_PANEL_WIDTH_L_RG             (0x044 + DISPLAY_SETTING_REG_BASE)
#define DISP_PANEL_WIDTH_H_RG             (0x045 + DISPLAY_SETTING_REG_BASE)
#define DISP_PANEL_HEIGHT_L_RG            (0x046 + DISPLAY_SETTING_REG_BASE)
#define DISP_PANEL_HEIGHT_H_RG            (0x047 + DISPLAY_SETTING_REG_BASE)
#define DISP_GENERAL_CONTROL4_RG          (0x04A + DISPLAY_SETTING_REG_BASE)
#define DISP_GENERAL_CONTROL5_RG          (0x04B + DISPLAY_SETTING_REG_BASE)
#define DISP_GENERAL_CONTROL6_RG          (0x04C + DISPLAY_SETTING_REG_BASE)
                                                   
#define LCD_GAME_LAYER_X                  (0x0B6 + DISPLAY_SETTING_REG_BASE)
#define LCD_GAME_LAYER_Y                  (0x0B8 + DISPLAY_SETTING_REG_BASE)
#define LCD_GAME_LAYER_WIDTH              (0x0C0 + DISPLAY_SETTING_REG_BASE)
#define LCD_GAME_LAYER_HEIGHT             (0x0C2 + DISPLAY_SETTING_REG_BASE)
                                                   
#define GAME_WIN_SEMI_WEIGHT_RG           (0x0CA + DISPLAY_SETTING_REG_BASE)
#define BACKGROUND_SEMI_WEIGHT_RG         (0x0EA + DISPLAY_SETTING_REG_BASE)

#define MAIN_WIN_TP_COLOR_B_RG            (0x052 + DISPLAY_SETTING_REG_BASE)
#define MAIN_WIN_TP_COLOR_G_RG            (0x053 + DISPLAY_SETTING_REG_BASE)
#define MAIN_WIN_TP_COLOR_R_RG            (0x054 + DISPLAY_SETTING_REG_BASE)
#define MAIN_WIN_LCD_X_L_RG               (0x056 + DISPLAY_SETTING_REG_BASE)
#define MAIN_WIN_LCD_X_H_RG               (0x057 + DISPLAY_SETTING_REG_BASE)
#define MAIN_WIN_LCD_Y_L_RG               (0x058 + DISPLAY_SETTING_REG_BASE)
#define MAIN_WIN_LCD_Y_H_RG               (0x059 + DISPLAY_SETTING_REG_BASE)
#define MAIN_WIN_PIXEL_OFF_L_RG           (0x05A + DISPLAY_SETTING_REG_BASE)
#define MAIN_WIN_PIXEL_OFF_H_RG           (0x05B + DISPLAY_SETTING_REG_BASE)
#define MAIN_WIN_ROW_OFF_L_RG             (0x05C + DISPLAY_SETTING_REG_BASE)
#define MAIN_WIN_ROW_OFF_H_RG             (0x05D + DISPLAY_SETTING_REG_BASE)
#define MAIN_WIN_WIDTH_L_RG		  (0x05E + DISPLAY_SETTING_REG_BASE)
#define MAIN_WIN_WIDTH_H_RG               (0x05F + DISPLAY_SETTING_REG_BASE)
#define MAIN_WIN_HEIGHT_L_RG              (0x060 + DISPLAY_SETTING_REG_BASE)
#define MAIN_WIN_HEIGHT_H_RG              (0x061 + DISPLAY_SETTING_REG_BASE)
#define MAIN_WIN_TP_SET_RG                (0x062 + DISPLAY_SETTING_REG_BASE)
#define MAIN_WIN_SEMI_WEIGHT_RG           (0x063 + DISPLAY_SETTING_REG_BASE)
#define MAIN_WIN_START_OFF_L_RG           (0x064 + DISPLAY_SETTING_REG_BASE)
#define MAIN_WIN_START_OFF_M_RG           (0x065 + DISPLAY_SETTING_REG_BASE)
#define MAIN_WIN_START_OFF_H_RG           (0x066 + DISPLAY_SETTING_REG_BASE)
#define MAIN_WIN_START_OFF_UH_RG          (0x067 + DISPLAY_SETTING_REG_BASE)
#define MAIN_WIN_START_ADDR_L_RG          (0x068 + DISPLAY_SETTING_REG_BASE)
#define MAIN_WIN_START_ADDR_M_RG          (0x069 + DISPLAY_SETTING_REG_BASE)
#define MAIN_WIN_START_ADDR_H_RG          (0x06A + DISPLAY_SETTING_REG_BASE)
#define MAIN_WIN_START_ADDR_UH_RG         (0x06B + DISPLAY_SETTING_REG_BASE)

#define PIP_WIN_TP_COLOR_B_RG             (0x072 + DISPLAY_SETTING_REG_BASE)
#define PIP_WIN_TP_COLOR_G_RG             (0x073 + DISPLAY_SETTING_REG_BASE)
#define PIP_WIN_TP_COLOR_R_RG             (0x074 + DISPLAY_SETTING_REG_BASE)
#define PIP_WIN_LCD_X_L_RG                (0x076 + DISPLAY_SETTING_REG_BASE)
#define PIP_WIN_LCD_X_H_RG                (0x077 + DISPLAY_SETTING_REG_BASE)
#define PIP_WIN_LCD_Y_L_RG                (0x078 + DISPLAY_SETTING_REG_BASE)
#define PIP_WIN_LCD_Y_H_RG                (0x079 + DISPLAY_SETTING_REG_BASE)
#define PIP_WIN_PIXEL_OFF_L_RG            (0x07A + DISPLAY_SETTING_REG_BASE)
#define PIP_WIN_PIXEL_OFF_H_RG            (0x07B + DISPLAY_SETTING_REG_BASE)
#define PIP_WIN_ROW_OFF_L_RG              (0x07C + DISPLAY_SETTING_REG_BASE)
#define PIP_WIN_ROW_OFF_H_RG              (0x07D + DISPLAY_SETTING_REG_BASE)
#define PIP_WIN_WIDTH_L_RG                (0x07E + DISPLAY_SETTING_REG_BASE)
#define PIP_WIN_WIDTH_H_RG                (0x07F + DISPLAY_SETTING_REG_BASE)
#define PIP_WIN_HEIGHT_L_RG               (0x080 + DISPLAY_SETTING_REG_BASE)
#define PIP_WIN_HEIGHT_H_RG               (0x081 + DISPLAY_SETTING_REG_BASE)
#define PIP_WIN_TP_SET_RG                 (0x082 + DISPLAY_SETTING_REG_BASE)
#define PIP_WIN_SEMI_WEIGHT_RG            (0x083 + DISPLAY_SETTING_REG_BASE)
#define PIP_WIN_START_OFF_L_RG            (0x084 + DISPLAY_SETTING_REG_BASE)
#define PIP_WIN_START_OFF_M_RG            (0x085 + DISPLAY_SETTING_REG_BASE)
#define PIP_WIN_START_OFF_H_RG            (0x086 + DISPLAY_SETTING_REG_BASE)
#define PIP_WIN_START_OFF_UH_RG           (0x087 + DISPLAY_SETTING_REG_BASE)
#define PIP_WIN_START_ADDR_L_RG           (0x088 + DISPLAY_SETTING_REG_BASE)
#define PIP_WIN_START_ADDR_M_RG           (0x089 + DISPLAY_SETTING_REG_BASE)
#define PIP_WIN_START_ADDR_H_RG           (0x08A + DISPLAY_SETTING_REG_BASE)
#define PIP_WIN_START_ADDR_UH_RG          (0x08B + DISPLAY_SETTING_REG_BASE)
#define	PIP_WIN_PIPO_START_ADDR_L_RG	  (0x08C + DISPLAY_SETTING_REG_BASE)
#define	PIP_WIN_PIPO_START_ADDR_M_RG	  (0x08D + DISPLAY_SETTING_REG_BASE)
#define	PIP_WIN_PIPO_START_ADDR_H_RG	  (0x08E + DISPLAY_SETTING_REG_BASE)
#define	PIP_WIN_PIPO_START_ADDR_UH_RG	  (0x08F + DISPLAY_SETTING_REG_BASE)

#define OLAY_WIN_TP_COLOR_B_RG            (0x092 + DISPLAY_SETTING_REG_BASE)
#define OLAY_WIN_TP_COLOR_G_RG            (0x093 + DISPLAY_SETTING_REG_BASE)
#define OLAY_WIN_TP_COLOR_R_RG            (0x094 + DISPLAY_SETTING_REG_BASE)
#define OLAY_WIN_LCD_X_L_RG               (0x096 + DISPLAY_SETTING_REG_BASE)
#define OLAY_WIN_LCD_X_H_RG               (0x097 + DISPLAY_SETTING_REG_BASE)
#define OLAY_WIN_LCD_Y_L_RG               (0x098 + DISPLAY_SETTING_REG_BASE)
#define OLAY_WIN_LCD_Y_H_RG               (0x099 + DISPLAY_SETTING_REG_BASE)
#define OLAY_WIN_PIXEL_OFF_L_RG           (0x09A + DISPLAY_SETTING_REG_BASE)
#define OLAY_WIN_PIXEL_OFF_H_RG           (0x09B + DISPLAY_SETTING_REG_BASE)
#define OLAY_WIN_ROW_OFF_L_RG             (0x09C + DISPLAY_SETTING_REG_BASE)
#define OLAY_WIN_ROW_OFF_H_RG             (0x09D + DISPLAY_SETTING_REG_BASE)
#define OLAY_WIN_WIDTH_L_RG               (0x09E + DISPLAY_SETTING_REG_BASE)
#define OLAY_WIN_WIDTH_H_RG               (0x09F + DISPLAY_SETTING_REG_BASE)
#define OLAY_WIN_HEIGHT_L_RG              (0x0A0 + DISPLAY_SETTING_REG_BASE)
#define OLAY_WIN_HEIGHT_H_RG              (0x0A1 + DISPLAY_SETTING_REG_BASE)
#define OLAY_WIN_TP_SET_RG                (0x0A2 + DISPLAY_SETTING_REG_BASE)
#define OLAY_WIN_SEMI_WEIGHT_RG           (0x0A3 + DISPLAY_SETTING_REG_BASE)
#define OLAY_WIN_START_OFF_L_RG           (0x0A4 + DISPLAY_SETTING_REG_BASE)
#define OLAY_WIN_START_OFF_M_RG           (0x0A5 + DISPLAY_SETTING_REG_BASE)
#define OLAY_WIN_START_OFF_H_RG           (0x0A6 + DISPLAY_SETTING_REG_BASE)
#define OLAY_WIN_START_OFF_UH_RG          (0x0A7 + DISPLAY_SETTING_REG_BASE)
#define OLAY_WIN_START_ADDR_L_RG          (0x0A8 + DISPLAY_SETTING_REG_BASE)
#define OLAY_WIN_START_ADDR_M_RG          (0x0A9 + DISPLAY_SETTING_REG_BASE)
#define OLAY_WIN_START_ADDR_H_RG          (0x0AA + DISPLAY_SETTING_REG_BASE)
#define OLAY_WIN_START_ADDR_UH_RG         (0x0AB + DISPLAY_SETTING_REG_BASE)

#define WIN_BACKGROUND_COLOR_R_RG         (0x0E0 + DISPLAY_SETTING_REG_BASE)
#define WIN_BACKGROUND_COLOR_G_RG         (0x0E1 + DISPLAY_SETTING_REG_BASE)
#define WIN_BACKGROUND_COLOR_B_RG         (0x0E2 + DISPLAY_SETTING_REG_BASE)


#define	FUNX_AVERAGE	0x00
#define	FUNX_AND		0x01
#define	FUNX_OR			0x02
#define	FUNX_INVERSE	0x03

#define	WIN_NO_ERROR	0x00
#define	WIN_PARAS_ERROR		0x01
#define	WIN_PARAS_OUT_RANGE	0x02


u_short	A8L_SetWindowActive(u_short windowID, u_short winActive);
u_short	A8L_SetWindowTransparency(u_short windowID, u_short transpActive, u_int transpColor);
u_short	A8L_SetWindowPaletteActive(u_short windowID, u_short paletteActive);
u_short	A8L_SetWindowPriority(u_short firstPrio, u_short secondPrio, 
			u_short thirdPrio, u_short fourthPrio);
u_short	A8L_BindBufferToWindow(u_short bufferID, u_short windowID);			
u_short	A8L_SetWindowAttribute(u_short windowID, u_short	width, u_short height,
			u_short colordepth, u_int baseAddr);
u_short	A8L_GetWindowAttribute(u_short windowID, WINDOW_INFO *winInfoPtr);
u_short	A8L_SetWindowToDisplay(u_short windowID, u_short startx, u_short starty,
			u_short displayWidth, u_short displayHeight, u_short displayOffsetx, 
			u_short displayOffsety, u_short rotateType);
u_short	A8L_LoadWindowPalette(u_short windowID, u_char *palette, u_short entry);
u_int	A8L_SetWindowBackground(u_int backColor);
u_short A8L_SetWindowSemiTransparency(u_short windowID, u_short SemiTranspActive, 
												u_int semiTransColor, u_short semiFunx, u_short semiWeight);
u_short	A8L_SetIconAttribute( u_int IconAddr, u_short width, u_short height, u_short x0, u_short y0, u_short ColorDepth, u_short LPF );
u_short	A8L_SetIconTransparency( u_short IsActive, u_int TransparentColor );
u_short	A8L_SetIconSemiTransparency( u_short IsActive, u_short IconWeight, u_short SourceWeight );
u_short	A8L_SetIconActive( u_short IsActive, u_short Mode );

void A8L_InitialAlphaBlendingLUT(void);
void A8L_InitialAlphaBlendingLUT_1(void);

s_short A8L_SetWindowAlphaBlendingActive( u_short WinID, u_short IsActive );
#endif
