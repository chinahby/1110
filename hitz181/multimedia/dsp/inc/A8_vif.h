#ifndef	A8_VIF_H
#define	A8_VIF_H

#define  VIF_REG_BASE 0x7100

#define	VIF_INT_EN_HOST		(VIF_REG_BASE+0x00)
#define	VIF_INT_ST_HOST		(VIF_REG_BASE+0x01)
#define	VIF_INT_EN_CPU		(VIF_REG_BASE+0x02)
#define	VIF_INT_ST_CPU		(VIF_REG_BASE+0x03)
#define	VIF_LINE_INT_NUM	(VIF_REG_BASE+0x04)
#define	VIF_INT_MODE		(VIF_REG_BASE+0x06)

#define	VIF_DATA_IN_EN		(VIF_REG_BASE+0x10)
#define	VIF_DATA_OUT_EN		(VIF_REG_BASE+0x11)
#define	VIF_OPR_UPDATE_CTL	(VIF_REG_BASE+0x14)
#define	VIF_FRAME_SKIP_EN	(VIF_REG_BASE+0x15)
#define	VIF_FRAME_SKIP_NUM	(VIF_REG_BASE+0x16)

#define	VIF_SEN_CLK_CTL		(VIF_REG_BASE+0x20)
#define	VIF_SEN_CLK_DIV		(VIF_REG_BASE+0x21)
#define	VIF_SEN_GEN_CTL		(VIF_REG_BASE+0x22)
#define	VIF_SEN_YUV_CTL		(VIF_REG_BASE+0x24)
#define	VIF_PXL_CLK_DELAY	(VIF_REG_BASE+0x26)

#define	VIF_GRAB_START_X	(VIF_REG_BASE+0x30)
#define	VIF_GRAB_END_X		(VIF_REG_BASE+0x32)
#define	VIF_GRAB_START_Y	(VIF_REG_BASE+0x34)
#define	VIF_GRAB_END_Y		(VIF_REG_BASE+0x36)

#define	VIF_SEN_IF_OUTPUT	(VIF_REG_BASE+0x50)
#define	VIF_SEN_IF_DATA		(VIF_REG_BASE+0x51)

//=============================//
//  VIF_INT_ST_HOST : 0x01     //
//=============================//
#define	VIF_LINE_INT				0x08
#define	VIF_FRAME_START				0x04
#define	VIF_FRAME_END				0x02
#define	VIF_GRAB_END				0x01
//=============================//
//  VIF_DATA_IN_EN : 0x10	   //
//=============================//
#define	VIF_EN_DATA_IN				0x01
//=============================//
//  VIF_DATA_OUT_EN : 0x11	   //
//=============================//
#define	VIF_EN_DATA_OUT				0x01
//=============================//
//  VIF_OPR_UPDATE_CTL : 0x14  //
//=============================//
#define	VIF_UPDATE_OPR_EN			0x02
#define	VIF_UPDATE_OPR_FRAME_SYNC	0x01
//=============================//
//  VIF_SEN_CLK_CTL : 0x20     //
//=============================//
#define	SEN_CLK_PHASE_3				0x30
#define	SEN_CLK_PHASE_2				0x20
#define	SEN_CLK_PHASE_1				0x10
#define	SEN_CLK_PHASE_0				0x00
#define	SEN_CLK_POS_POL				0x02
#define	SEN_CLK_NEG_POL				0x00
#define	SEN_CLK_OUTPUT				0x01
//=============================//
//  VIF_SEN_GEN_CTL : 0x22     //
//=============================//
#define	SEN_LATCH_POS_PHASE			0x80
#define	SEN_LATCH_NEG_PHASE			0x00
#define	SEN_HOR_POS_POL				0x00
#define	SEN_HOR_NEG_POL				0x02
#define	SEN_VER_POS_POL				0x00
#define	SEN_VER_NEG_POL				0x01
//=============================//
//  VIF_SEN_YUV_CTL : 0x24     //
//=============================//
#define	SEN_YCBYCR					0x00
#define	SEN_YCRYCB					0x10
#define	SEN_CBYCRY					0x20
#define	SEN_CRYCBY					0x30
#define	SEN_YUV_EN					0x01

#endif