/**
 * @file A8_graphics.c
 * @brief Copyright (C) 2002-2008 Alpha Imaging Technology Corp.
 * @n AIT701/703 Series Host API.
 * @n The purpose of AIT701 Host API is for Host CPU to control and communicate AIT701
 *      chip via direct/indirect parallel interface.
 * @note N/A
 * @bug N/A
 */
#include "AIT700_ebibus.h"
#include "A8_common.h"
#include "A8_win.h"
#include "A8_graphics.h"
#include "A8_panel.h"
#include "A8_dscapi.h"

u_short			gbTransActive;
StructBUF		gsBuffer[MAX_BUFFER];

/**
 @fn		u_short	SetGraphicsTransparent(u_short transpActive)
 @brief	Set the gbTransActive for the future operation.
 @param	transpActive -Set transparent Attribute.
 @return	u_short - the privious gbTransActive is returned.
 @note	.
 @bug	N/A.
*/
u_short	SetGraphicsTransparent(u_short transpActive)
{
	u_short	oldactive;
	
	oldactive = gbTransActive;
	gbTransActive = transpActive;
	
	return oldactive;
}	


/**
 @fn		u_int	SetGraphicsBackground(u_int backColor)
 @brief	Set the background color for the future operation.
 @param	backColor -Background Color.
 @return	u_short - the privious background color is returned.
 @note	.
 @bug	N/A.
*/
u_int	SetGraphicsBackground(u_int backColor)
{
	u_int	oldcolor;

	oldcolor = GetA8RegB(GRH_BACK_COLOR_0);
	oldcolor += (GetA8RegW(GRH_BACK_COLOR_1) << 8);
	
	SetA8RegB(GRH_BACK_COLOR_0, (u_char)backColor);
	SetA8RegW(GRH_BACK_COLOR_1, (u_short)(backColor >> 8));

	return oldcolor;
}	


/**
 @fn		u_int	SetGraphicsForeground(u_int foreColor)
 @brief	Set the foreground color for the future operation.
 @param	foreColor -Foreground Color	.
 @return	u_short - the privious foreground color is returned.
 @note	.
 @bug	N/A.
*/
u_int	SetGraphicsForeground(u_int foreColor)
{
	u_int	oldcolor;

	oldcolor = GetA8RegW(GRH_FORE_COLOR_0);
	oldcolor += (GetA8RegB(GRH_FORE_COLOR_2) << 16);
	
	SetA8RegW(GRH_FORE_COLOR_0, (u_short)foreColor);
	SetA8RegB(GRH_FORE_COLOR_2, (u_char)(foreColor >> 16));
 
	return oldcolor;
}


/**
 @fn		u_short	A8L_CopyImageFIFOtoBuf(u_short *srcAddr, u_short width, u_short height, u_short colorDepth, u_short bufferID, u_short tgtx, u_short tgty, u_short ropCode)
 @brief	Copy Host image from FIFO to AIT memory buffer.
 @param	srcAddr - the address of the source image (on Host).
 @param	width - the width of image size.
 @param	height - the height of image size.
 @param	colorDepth - the data format of the source image.
 @param	bufferID - the ID of the dest. buffer.
 @param	tgtx - the X position in the dest. buffer for copy.
 @param	tgty - the Y position in the dest. buffer for copy.
 @param	ropCode - the raster operation code.
 @return	u_short - 0:success,  others:error code.
 @note	.
 @bug	N/A.
*/
u_short	A8L_CopyImageFIFOtoBuf(u_short *srcAddr, u_short width, u_short height, 
			u_short colorDepth, u_short bufferID, u_short tgtx, u_short tgty, 
			u_short ropCode)
{
	u_int	longtmp;
	u_short	*pshorttmp;
	u_short	fifocount, transfercount;
	u_short	cut_width, cut_height;
	u_short	cut_width_short=0, width_short=0;
	u_short	j;
	s_int		total_count;
	
	if (width > MAX_WIDTH || height > MAX_HEIGHT || 
			bufferID > (MAX_BUFFER-1) ||
			tgtx >= gsBuffer[bufferID].width || tgty >= gsBuffer[bufferID].height ||
			!width || !height)  {
		return GRH_PARAS_OUT_RANGE;
	}	

	if (colorDepth != A8_DEPTH_8B && colorDepth != A8_DEPTH_16B && colorDepth != A8_DEPTH_24B )
		return GRH_PARAS_ERROR;
	
	if (colorDepth != (gsBuffer[bufferID].colordepth)) {
		if (colorDepth != A8_DEPTH_8B || gsBuffer[bufferID].colordepth != A8_DEPTH_24B) {
			return GRH_FUNX_ERROR;
		}	
	}			

	if ((width + tgtx) > gsBuffer[bufferID].width)
		cut_width = gsBuffer[bufferID].width - tgtx;
	else	
		cut_width = width;

	if ((height + tgty) > gsBuffer[bufferID].height)
		cut_height = gsBuffer[bufferID].height - tgty;
	else	
		cut_height = height;

	/*Some Reset Process */
	SetA8RegB( 0x690A, 0x12 );	
	SetA8RegB( 0x690A, 0x00 );	
	SetA8RegB( 0x6B24, 0x02);
	SetA8RegB( 0x6B24, 0x00);

	// set the graphics control parameter	
	SetA8RegW(GRH_FIFO_WR_TH, FIFO_RD_WR_THRESHOLD);
	SetA8RegW(GRH_BLT_WIDTH_0, cut_width);
	SetA8RegW(GRH_BLT_HEIGHT_0, cut_height);
	SetA8RegB(GRH_ROP, ropCode & GRH_ROP_MASK);
	SetA8RegB(GRH_COLOR_FORMAT, ((gsBuffer[bufferID].colordepth) << 2)|(colorDepth));
	SetA8RegB(GRH_BITBLT_CTL, GRH_OP_DO_ROP | GRH_HOST_2_MEM | GRH_LEFT_TOP_RIGHT);

	if (gbTransActive) 
		SetA8RegB(GRH_BITBLT_CTL, GetA8RegB(GRH_BITBLT_CTL) | GRH_OP_DO_TRANSP);

	// Deal Destination Buffer
	longtmp = (gsBuffer[bufferID].startaddr)
				+ ((gsBuffer[bufferID].lineoffset) * (tgty))
				+ ((tgtx) * (gsBuffer[bufferID].colordepth + 1));

	SetA8RegW(GRH_DST_ADDR_0, (u_short)(longtmp & 0xFFFF));
	SetA8RegW(GRH_DST_ADDR_2, (u_short)(longtmp >> 16));

	longtmp = gsBuffer[bufferID].lineoffset;	

	SetA8RegW(GRH_DST_OFFSET_0, (u_short)(longtmp & 0xFFFF));
	SetA8RegW(GRH_DST_OFFSET_2, (u_short)(longtmp >> 16));

	SetA8RegW(GRH_SRC_ADDR_0, 0);
	SetA8RegW(GRH_SRC_ADDR_2, 0);

	pshorttmp = srcAddr;

	switch (colorDepth) {	// source format
		case A8_DEPTH_8B:
			if (!(cut_width & 0x0001))
				cut_width_short = cut_width >> 1;
			else 	
				cut_width_short = (cut_width+1) >> 1;
			if (!(width & 0x0001))
				width_short = width >> 1;
			else 	
				width_short = (width+1) >> 1;
			break;			
		case A8_DEPTH_16B:
			cut_width_short = cut_width;
			width_short = width;
			break;
		case A8_DEPTH_24B:
			if (!(cut_width & 0x0001))
				cut_width_short = (cut_width*3) >> 1;
			else 	
				cut_width_short = (cut_width*3+1) >> 1;

			if (!(width & 0x0001))
				width_short = (width*3) >> 1;
			else 	
				width_short = (width*3+1) >> 1;
			break;
	}
	
	total_count = (cut_width_short * cut_height);
	j = 0;

	// need to reset the FIFO at first to ensure that there are no
	// data in the FIFO, or check the FIFO empty status
	//SetA8RegB(GRH_ENABLE_CTL, GRH_FIFO_RESET);
	//printf("1xxxxx 0x6B26 = 0x%x\n", GetA8RegB(GRH_FIFO_UNWR_WORD));
	if (GetA8RegB(GRH_FIFO_UNWR_WORD) != FIFO_DEPTH)
		return GRH_HW_ERROR;

	SetA8RegB(GRH_ENABLE_CTL, GRH_ENABLE);

	do {
		//while (!(GetA8RegB(GRH_FIFO_STATUS) & FIFO_WR_UNDER_THRES));
		//fifocount = GetA8RegB(GRH_FIFO_UNWR_WORD) & 0x1f;

		while(!(GetA8RegB(GRH_FIFO_STATUS) & FIFO_EMPTY));
		fifocount = 16;

		if (fifocount < total_count)
			transfercount = fifocount;
		else
			transfercount = total_count;
		
/*	
		for ( ; transfercount > 0; transfercount--, total_count--) {
			if (j == (cut_width_short -1)) {
				SetA8RegW(GRH_FIFO_DAT_PORT_0, *(pshorttmp++)); 	
				pshorttmp += (width_short - cut_width_short);
				j = 0;
			}
			else {	
				SetA8RegW(GRH_FIFO_DAT_PORT_0, *(pshorttmp++)); 	
				j++;
			}	
		}	
*/		
		A8IndCmdP = HII_SET_ADDR_L;
		A8IndDataPW(0) = 0x6B28;
		A8IndCmdP = HII_RW_REG_W;
		for ( j=0; j<transfercount; j++ )
			A8IndDataPW(j) = *pshorttmp++;
		total_count -= transfercount;
	} while (total_count > 0);

	// wait for the FIFO to be empty and graphic engine to stop
	while (GetA8RegB(GRH_ENABLE_CTL) & GRH_ENABLE);
	return GRH_NO_ERROR;
}


/**
 @fn		u_short	A8L_CopyImageBuftoFIFO(u_short *dstAddr, u_short width, u_short height, u_short bufferID, u_short srcx, u_short srcy)
 @brief	Copy image from AIT memory buffer to FIFO (to Host).
 @param	dstAddr - the address of the source image(in Host).
 @param	width - the width of image size.
 @param	height - the height of image size.
 @param	bufferID - the ID of the source buffer.
 @param	srcx - the X position in the source buffer for copy.
 @param	srcy - the Y position in the source buffer for copy.
 @return	u_short - 0:success,  others:error code.
 @note	.
 @bug	N/A.
*/
u_short	A8L_CopyImageBuftoFIFO(u_short *dstAddr, u_short width, u_short height, 
			u_short bufferID, u_short srcx, u_short srcy)
{
	u_int	longtmp;
	u_short	*pshorttmp;
	u_short	fifocount;
	u_short	cut_width, cut_height;
	u_short	cut_width_short = 0;
	u_short	j, k;
	s_int		total_count;
	u_int	timeout = 0;
	/*u_short	err = GRH_NO_ERROR;*/

	if (width > MAX_WIDTH || height > MAX_HEIGHT || 
			bufferID > (MAX_BUFFER-1) ||
			srcx >= gsBuffer[bufferID].width || srcy >= gsBuffer[bufferID].height ||
			!width || !height) {
		return GRH_PARAS_OUT_RANGE;
	}	

	if ((width + srcx) > gsBuffer[bufferID].width)
		cut_width = gsBuffer[bufferID].width - srcx;
	else	
		cut_width = width;

	if ((height + srcy) > gsBuffer[bufferID].height)
		cut_height = gsBuffer[bufferID].height - srcy;
	else	
		cut_height = height;
	
	// set the graphics control parameter	
	SetA8RegW(GRH_FIFO_WR_TH, FIFO_RD_WR_THRESHOLD);
	SetA8RegW(GRH_BLT_WIDTH_0, cut_width);
	SetA8RegW(GRH_BLT_HEIGHT_0, cut_height);
	SetA8RegB(GRH_COLOR_FORMAT, (((gsBuffer[bufferID].colordepth)) << 2)
									| ((gsBuffer[bufferID].colordepth)));
	SetA8RegB(GRH_BITBLT_CTL, GRH_MEM_2_HOST | GRH_LEFT_TOP_RIGHT);

	// Deal Source Buffer	
	longtmp = (gsBuffer[bufferID].startaddr)
				+ ((gsBuffer[bufferID].lineoffset) * (srcy))
				+ ((srcx) * (gsBuffer[bufferID].colordepth + 1));

	SetA8RegW(GRH_SRC_ADDR_0, (u_short)(longtmp & 0xFFFF));
	SetA8RegW(GRH_SRC_ADDR_2, (u_short)(longtmp >> 16));

	longtmp = gsBuffer[bufferID].lineoffset;	

	SetA8RegW(GRH_SRC_OFFSET_0, (u_short)(longtmp & 0xFFFF));
	SetA8RegW(GRH_SRC_OFFSET_2, (u_short)(longtmp >> 16));

	SetA8RegW(GRH_DST_ADDR_0, 0);
	SetA8RegW(GRH_DST_ADDR_2, 0);

	pshorttmp = dstAddr;
	
	switch (gsBuffer[bufferID].colordepth) {	// source format
		case A8_DEPTH_8B:
			if (!(cut_width & 0x0001))
				cut_width_short = cut_width >> 1;
			else 	
				cut_width_short = (cut_width+1) >> 1;
			break;			
		case A8_DEPTH_16B:
			cut_width_short = cut_width;
			break;
		case A8_DEPTH_24B:
			if (!(cut_width & 0x0001))
				cut_width_short = (cut_width*3) >> 1;
			else 	
				cut_width_short = (cut_width*3+1) >> 1;
			break;
	}

	total_count = (cut_width_short * cut_height);
	j = 0;
	k = 0;

	// need to reset the FIFO at first to ensure that there are no
	// data in the FIFO, or check the FIFO empty status
	SetA8RegB(GRH_ENABLE_CTL, GRH_FIFO_RESET);
	if (GetA8RegB(GRH_FIFO_UNRD_WORD) != 0)
		return GRH_HW_ERROR;

	SetA8RegB(GRH_ENABLE_CTL, GRH_ENABLE);

	if (total_count > FIFO_RD_THRESHOLD) {	
		do {
			// wait for FIFO to be over threshold
			while (!(GetA8RegB(GRH_FIFO_STATUS) & FIFO_RD_OVER_THRES));
			fifocount = GetA8RegB(GRH_FIFO_UNRD_WORD) & 0x1f;

			A8IndCmdP = HII_SET_ADDR_L;
			A8IndDataPW(0) = GRH_FIFO_DAT_PORT_0;
			A8IndCmdP = HII_RW_REG_W;
			for ( ; fifocount > 0; fifocount--, total_count--) 
			{
				//*(pshorttmp++) = GetA8RegW(GRH_FIFO_DAT_PORT_0);
				*(pshorttmp++) = A8IndDataPW(0);
			}	
			//if (total_count < 0)
				//AIT_Message_P1("total_count = %d\r\n", total_count);
		} while (total_count > FIFO_RD_THRESHOLD);
	}
	// wait for the FIFO to be empty and graphic engine to stop
	timeout=0;
	while( (GetA8RegB(GRH_ENABLE_CTL) & GRH_ENABLE) && (timeout<0x50000) )
	{
		timeout++;
	}
	if(timeout>=0x50000)
	{
		//AIT_Message_P0("CopyImageFifotoBUf timeout\n");
		return GRH_HW_ERROR;
	}
	
	fifocount = GetA8RegB(GRH_FIFO_UNRD_WORD) & 0x1f;

	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPW(0) = GRH_FIFO_DAT_PORT_0;
	A8IndCmdP = HII_RW_REG_W;
	while (fifocount > 0) 
	{
		
		*(pshorttmp++) = A8IndDataPW(0);
		fifocount--;
	}	

	return GRH_NO_ERROR;
}


/**
 @fn		u_short	A8L_CopyImageBuftoBuf(u_short width, u_short height, u_short srcBufferID, u_short srcx, u_short srcy, u_short tgtBufferID, u_short tgtx, u_short tgty, u_short ropCode)
 @brief	Copy image from AIT memory buffer to AIT memory buffer.
 @param	width - the width of image size.
 @param	height - the height of image size.
 @param	srcBufferID - the ID of the source buffer.
 @param	srcx - the X position in the source buffer for copy.
 @param	srcy - the Y position in the source buffer for copy.
 @param	tgtBufferID - the ID of the dest. buffer.
 @param	tgtx - the X position in the dest. buffer for copy.
 @param	tgty - the Y position in the dest. buffer for copy.
 @param	ropCode - the raster operation code.
 @return	u_short - 0:success,  others:error code.
 @note	.
 @bug	N/A.
*/
u_short	A8L_CopyImageBuftoBuf(u_short width, u_short height, 
			u_short srcBufferID, u_short srcx, u_short srcy, 
			u_short tgtBufferID, u_short tgtx, u_short tgty, u_short ropCode)
{
	u_int	longtmp;
	u_short	cut_width, cut_height;
	u_short	raster_dir;
	
	if (width > MAX_WIDTH || height > MAX_HEIGHT || 
			srcBufferID > (MAX_BUFFER-1) || tgtBufferID > (MAX_BUFFER-1) ||
			srcx >= gsBuffer[srcBufferID].width || srcy >= gsBuffer[srcBufferID].height ||
			tgtx >= gsBuffer[tgtBufferID].width || tgty >= gsBuffer[tgtBufferID].height ||
			!width || !height) {
		return GRH_PARAS_OUT_RANGE;
	}	

	if (gsBuffer[srcBufferID].colordepth != gsBuffer[tgtBufferID].colordepth) {
		if (gsBuffer[srcBufferID].colordepth != A8_DEPTH_8B || 
				gsBuffer[tgtBufferID].colordepth != A8_DEPTH_24B) {
			return GRH_PARAS_ERROR;
		}	
	}			
	
	if ((width + srcx) > gsBuffer[srcBufferID].width)
		cut_width = gsBuffer[srcBufferID].width - srcx;
	else	
		cut_width = width;

	if ((height + srcy) > gsBuffer[srcBufferID].height)
		cut_height = gsBuffer[srcBufferID].height - srcy;
	else	
		cut_height = height;
			
	if ((cut_width + tgtx) > gsBuffer[tgtBufferID].width)
		cut_width = gsBuffer[tgtBufferID].width - tgtx;

	if ((cut_height + tgty) > gsBuffer[tgtBufferID].height)
		cut_height = gsBuffer[tgtBufferID].height - tgty;
	
	raster_dir = GRH_LEFT_TOP_RIGHT;

	if (srcBufferID == tgtBufferID) {
		if (srcx == tgtx) {
			if (srcy > tgty) {
				raster_dir = GRH_LEFT_TOP_RIGHT;	// checked
			}
			else {	// include (srcx == tgtx) and (srcy == tgty)
				raster_dir = GRH_LEFT_BOT_RIGHT;	// checked
			}	
		}	
		else if (srcy == tgty) {
			if (srcx > tgtx) {
				raster_dir = GRH_LEFT_TOP_RIGHT;	// checked
			}	
			else {		
				raster_dir = GRH_RIGHT_BOT_LEFT;	// checked
			}
		}
		// Detect Overlap	
		else if (srcx > tgtx) {
			if ((tgtx+width) > srcx) {
				//(S:down-right d:upper-left)	
				if (srcy > tgty) {
					raster_dir = GRH_LEFT_TOP_RIGHT;	// checked
				}
				//(S:upper-right d:down-left)
				else if (srcy < tgty) {
					raster_dir = GRH_LEFT_BOT_RIGHT;	// checked
				}		
			}	
		}
		else if (srcx < tgtx) {
			if ((srcx+width) > tgtx) {
				//(S:down-left d:upper-right)
				if (srcy > tgty) {
					raster_dir = GRH_LEFT_TOP_RIGHT;	// checked
				}
				//(S:upper-left d:down-right)
				else if (srcy < tgty) {	
					raster_dir = GRH_LEFT_BOT_RIGHT;	// checked
				}
			}	
		}	
	}

	switch (raster_dir) {
		case GRH_LEFT_TOP_RIGHT:
			SetA8RegB(GRH_BITBLT_CTL, GRH_LEFT_TOP_RIGHT);

			// Deal Source Buffer
			longtmp = (gsBuffer[srcBufferID].startaddr)
					+ ((gsBuffer[srcBufferID].lineoffset) * (srcy)) 
					+ ((srcx) * (gsBuffer[srcBufferID].colordepth + 1));

			SetA8RegW(GRH_SRC_ADDR_0, (u_short)(longtmp & 0xFFFF));
			SetA8RegW(GRH_SRC_ADDR_2, (u_short)(longtmp >> 16));

			// Deal Destination Buffer
			longtmp = (gsBuffer[tgtBufferID].startaddr)
					+ ((gsBuffer[tgtBufferID].lineoffset) * (tgty)) 
					+ ((tgtx) * (gsBuffer[tgtBufferID].colordepth + 1));

			SetA8RegW(GRH_DST_ADDR_0, (u_short)(longtmp & 0xFFFF));
			SetA8RegW(GRH_DST_ADDR_2, (u_short)(longtmp >> 16));
			break;
		
		case GRH_RIGHT_BOT_LEFT:
			SetA8RegB(GRH_BITBLT_CTL, GRH_RIGHT_BOT_LEFT);

			// Deal Source Buffer
			longtmp = (gsBuffer[srcBufferID].startaddr)
				+ ((gsBuffer[srcBufferID].lineoffset) * (srcy + cut_height - 1)) 
				+ ((srcx + cut_width - 1) * (gsBuffer[srcBufferID].colordepth + 1));
	
			SetA8RegW(GRH_SRC_ADDR_0, (u_short)(longtmp & 0xFFFF));
			SetA8RegW(GRH_SRC_ADDR_2, (u_short)(longtmp >> 16));

			// Deal Destination Buffer
			longtmp = (gsBuffer[tgtBufferID].startaddr)
				+ ((gsBuffer[tgtBufferID].lineoffset) * (tgty + cut_height - 1)) 
				+ ((tgtx + cut_width - 1) * (gsBuffer[tgtBufferID].colordepth + 1));

			SetA8RegW(GRH_DST_ADDR_0, (u_short)(longtmp & 0xFFFF));
			SetA8RegW(GRH_DST_ADDR_2, (u_short)(longtmp >> 16));
			break;
		
		case GRH_LEFT_BOT_RIGHT:
			SetA8RegB(GRH_BITBLT_CTL, GRH_LEFT_BOT_RIGHT);

			// Deal Source Buffer
			longtmp = (gsBuffer[srcBufferID].startaddr)
				+ ((gsBuffer[srcBufferID].lineoffset) * (srcy + cut_height - 1)) 
				+ ((srcx) * (gsBuffer[srcBufferID].colordepth + 1));

			SetA8RegW(GRH_SRC_ADDR_0, (u_short)(longtmp & 0xFFFF));
			SetA8RegW(GRH_SRC_ADDR_2, (u_short)(longtmp >> 16));

			// Deal Destination Buffer
			longtmp = (gsBuffer[tgtBufferID].startaddr)
				+ ((gsBuffer[tgtBufferID].lineoffset) * (tgty + cut_height - 1)) 
				+ ((tgtx) * (gsBuffer[tgtBufferID].colordepth + 1));
			
			SetA8RegW(GRH_DST_ADDR_0, (u_short)(longtmp & 0xFFFF));
			SetA8RegW(GRH_DST_ADDR_2, (u_short)(longtmp >> 16));
			break;

		case GRH_RIGHT_TOP_LEFT:	
			SetA8RegB(GRH_BITBLT_CTL, GRH_RIGHT_TOP_LEFT);
			
			// Deal Source Buffer
			longtmp = (gsBuffer[srcBufferID].startaddr)
				+ ((gsBuffer[srcBufferID].lineoffset) * (srcy)) 
				+ ((srcx + cut_width - 1) * (gsBuffer[srcBufferID].colordepth + 1));
	
			SetA8RegW(GRH_SRC_ADDR_0, (u_short)(longtmp & 0xFFFF));
			SetA8RegW(GRH_SRC_ADDR_2, (u_short)(longtmp >> 16));

			// Deal Destination Buffer
			longtmp = (gsBuffer[tgtBufferID].startaddr)
				+ ((gsBuffer[tgtBufferID].lineoffset) * (tgty - 1)) 
				+ ((tgtx + cut_width - 1) * (gsBuffer[tgtBufferID].colordepth + 1));
	
			SetA8RegW(GRH_DST_ADDR_0, (u_short)(longtmp & 0xFFFF));
			SetA8RegW(GRH_DST_ADDR_2, (u_short)(longtmp >> 16));
			break;
	}	

	SetA8RegW(GRH_BLT_WIDTH_0, cut_width);
	SetA8RegW(GRH_BLT_HEIGHT_0, cut_height);
	SetA8RegB(GRH_COLOR_FORMAT, ((gsBuffer[tgtBufferID].colordepth) << 2)	// dst
									| (gsBuffer[srcBufferID].colordepth));
	SetA8RegB(GRH_ROP, ropCode & GRH_ROP_MASK);
	SetA8RegB(GRH_BITBLT_CTL, GetA8RegB(GRH_BITBLT_CTL) | GRH_OP_DO_ROP | GRH_MEM_2_MEM);

	if (gbTransActive) 
		SetA8RegB(GRH_BITBLT_CTL, GetA8RegB(GRH_BITBLT_CTL) | GRH_OP_DO_TRANSP);

	// Deal Source Buffer
	longtmp = gsBuffer[srcBufferID].lineoffset;

	SetA8RegW(GRH_SRC_OFFSET_0, (u_short)(longtmp & 0xFFFF));
	SetA8RegW(GRH_SRC_OFFSET_2, (u_short)(longtmp >> 16));

	// Deal Destination Buffer
	longtmp = gsBuffer[tgtBufferID].lineoffset;	

	SetA8RegW(GRH_DST_OFFSET_0, (u_short)(longtmp & 0xFFFF));
	SetA8RegW(GRH_DST_OFFSET_2, (u_short)(longtmp >> 16));

	SetA8RegB(GRH_ENABLE_CTL, GRH_ENABLE);
       sys_IF_ait_delay1ms(10);
       
	while( GetA8RegB(GRH_ENABLE_CTL)&GRH_ENABLE )
        {
        sys_IF_ait_delay1ms(2);
        }
    
	return GRH_NO_ERROR;
}


/**
 @fn		u_short	A8L_SetBufferAttribute(u_short bufferID, u_short width, u_short height, u_short lineOffset, u_short colorDepth, u_int bufferAddr) 
 @brief	Setup the characteristics of Memory Buffer.
 @param	bufferID: the ID of the buffer for setting.
 @param	width - the width of image size.
 @param	height - the height of image size.
 @param	lineOffset: line offset based on byte count.
 @param	colorDepth: bits per pixel on the buffer	.
 @param	bufferAddr: the memory address for the buffer.
 @return	u_short - 0:success,  others:error code.
 @note	.
 @bug	N/A.
*/
u_short	A8L_SetBufferAttribute(u_short bufferID, u_short width, u_short height, 
			u_short lineOffset, u_short colorDepth, u_int bufferAddr) 
{
	
	if (bufferID > (MAX_BUFFER-1) || (colorDepth != A8_DEPTH_8B && colorDepth != A8_DEPTH_16B && colorDepth != A8_DEPTH_24B
		&& colorDepth != A8_YUV_422 && colorDepth != A8_DEPTH_4I&& colorDepth != A8_DEPTH_8I))
	{
		return GRH_PARAS_ERROR;
	}	

    if (width > MAX_WIDTH || height > MAX_HEIGHT)
		return GRH_PARAS_OUT_RANGE;
	
	if (bufferAddr > 0x30000 || bufferAddr & 0x7) 
		return GRH_PARAS_OUT_RANGE;
#if 1	//added by ming 071102	
	if(colorDepth == A8_DEPTH_8B)
		lineOffset = width;
	else if(colorDepth == A8_DEPTH_16B)
		lineOffset = width << 1;
	else if(colorDepth == A8_YUV_422)
		lineOffset = width << 1;
	else if(colorDepth == A8_DEPTH_24B)
		lineOffset = (width*3)/2;
#endif						
    
    gsBuffer[bufferID].lineoffset = lineOffset;
    gsBuffer[bufferID].width = width;
    gsBuffer[bufferID].height = height;
    gsBuffer[bufferID].startaddr = bufferAddr;
    gsBuffer[bufferID].colordepth = colorDepth;
	return GRH_NO_ERROR;
}


u_short	A8L_SetBufferAttribute1(u_short bufferID, u_short width, u_short height, 
			u_short lineOffset, u_short colorDepth, u_int bufferAddr) 
{
	
	if (bufferID > (MAX_BUFFER-1) || (colorDepth != A8_DEPTH_8B && colorDepth != A8_DEPTH_16B && colorDepth != A8_DEPTH_24B
		&& colorDepth != A8_YUV_422 && colorDepth != A8_DEPTH_4I&& colorDepth != A8_DEPTH_8I))
	{
		return GRH_PARAS_ERROR;
	}	

    if (width > MAX_WIDTH || height > MAX_HEIGHT)
		return GRH_PARAS_OUT_RANGE;
	
	if (bufferAddr > 0x30000 || bufferAddr & 0x7) 
		return GRH_PARAS_OUT_RANGE;
    
    gsBuffer[bufferID].lineoffset = lineOffset;
    gsBuffer[bufferID].width = width;
    gsBuffer[bufferID].height = height;
    gsBuffer[bufferID].startaddr = bufferAddr;
    gsBuffer[bufferID].colordepth = colorDepth;
	return GRH_NO_ERROR;
}

/**
 @fn		u_short	A8L_GetBufferAttribute(u_short bufferID, u_short *width, u_short *height, u_short *lineOffset, u_short *colorDepth, u_int *bufferAddr) 
 @brief	Get the characteristics of Memory Buffer.
 @param	bufferID: the ID of the buffer for getting infor.
 @param	width - the width of buffer image size.
 @param	height - the height of buffer image size.
 @param	lineOffset: line offset based on byte count.
 @param	colorDepth: bits per pixel on the buffer	.
 @param	bufferAddr: the memory address for the buffer.
 @return	u_short - 0:success,  others:error code.
 @note	.
 @bug	N/A.
*/
u_short	A8L_GetBufferAttribute(u_short bufferID, u_short *width, u_short *height, 
			u_short *lineOffset, u_short *colorDepth, u_int *bufferAddr) 
{
	if (bufferID > (MAX_BUFFER-1)) 
		return GRH_PARAS_OUT_RANGE;

    *width = gsBuffer[bufferID].width;
    *height = gsBuffer[bufferID].height;
    *lineOffset = gsBuffer[bufferID].lineoffset;
    *bufferAddr = gsBuffer[bufferID].startaddr;
    *colorDepth = gsBuffer[bufferID].colordepth;
	return GRH_NO_ERROR;
}


/**
 @fn		u_short	A8L_DrawRectangletoBuf(u_short width, u_short height, u_short bufferID, u_short tgtx, u_short tgty, u_short style, u_short ropCode)
 @brief	Draw solid rectangle on the assigned buffer.
 @param	width - the width of the rectangle size.
 @param	height - the height of the rectangle size.
 @param	bufferID: the ID of the buffer for getting infor.
 @param	tgtx - the X position of the rectangle respect to the upper-left corner of the buffer.
 @param	tgty - the Y position of the rectangle respect to the upper-left corner of the buffer.
 @param	style - 0: FRAME_STYLE 1: SOLID_STYLE.
 @param	ropCode - the raster operation code.
 @return	u_short - 0:success,  others:error code.
 @note	.
 @bug	N/A.
*/
u_short	A8L_DrawRectangletoBuf(u_short width, u_short height, u_short bufferID, 
			u_short tgtx, u_short tgty, u_short style, u_short ropCode)
{
	u_int	longtmp;
	u_short	cut_width, cut_height;
	
	if (width > MAX_WIDTH || height > MAX_HEIGHT || 
			bufferID > (MAX_BUFFER-1) ||
			tgtx >= gsBuffer[bufferID].width || tgty >= gsBuffer[bufferID].height ||
			!width || !height) {
		return GRH_PARAS_OUT_RANGE;
	}	
	
	if ((width + tgtx) > gsBuffer[bufferID].width)
		cut_width = gsBuffer[bufferID].width - tgtx;
	else	
		cut_width = width;

	if ((height + tgty) > gsBuffer[bufferID].height)
		cut_height = gsBuffer[bufferID].height - tgty;
	else	
		cut_height = height;
	
//	GraphicsEngineReset();
	
	if (style == SOLID_STYLE) {	
		SetA8RegW(GRH_BLT_WIDTH_0, cut_width);
		SetA8RegW(GRH_BLT_HEIGHT_0, cut_height);
		SetA8RegB(GRH_COLOR_FORMAT, ((gsBuffer[bufferID].colordepth) << 2));
		SetA8RegB(GRH_ROP, ropCode & GRH_ROP_MASK);
		SetA8RegB(GRH_BITBLT_CTL, GRH_OP_DO_ROP | GRH_SOLID_FILL | GRH_LEFT_TOP_RIGHT);

		// Deal Destination Buffer
		longtmp = (gsBuffer[bufferID].startaddr)
					+ ((gsBuffer[bufferID].lineoffset) * (tgty))
					+ ((tgtx) * (gsBuffer[bufferID].colordepth + 1));

		SetA8RegW(GRH_DST_ADDR_0, (u_short)(longtmp));
		SetA8RegW(GRH_DST_ADDR_2, (u_short)(longtmp >> 16));

		longtmp = gsBuffer[bufferID].lineoffset;	

		SetA8RegW(GRH_DST_OFFSET_0, (u_short)(longtmp & 0xFFFF));
		SetA8RegW(GRH_DST_OFFSET_2, (u_short)(longtmp >> 16));

		SetA8RegB(GRH_ENABLE_CTL, GRH_ENABLE);
		while(GetA8RegB(GRH_ENABLE_CTL) & GRH_ENABLE);
	
		return GRH_NO_ERROR;	
	}
	else return GRH_PARAS_ERROR;
}


/**
 @fn		u_short	A8L_RotateImageBuftoBuf(u_short srcWidth, u_short srcHeight, u_short srcBufferID, u_short srcx, u_short srcy, u_short tgtBufferID, u_short tgtx, u_short tgty, u_short ropCode, u_short rotateType)
 @brief	Rotate image from AIT memory buffer to AIT memory buffer.
 @param	srcWidth - the width of image size.
 @param	srcHeight - the height of image size.
 @param	srcBufferID - the ID of the source buffer.
 @param	srcx - the X position in the source buffer for copy.
 @param	srcy - the Y position in the source buffer for copy.
 @param	tgtBufferID - the ID of the dest. buffer.
 @param	tgtx - the X position in the dest. buffer for copy.
 @param	tgty - the Y position in the dest. buffer for copy.
 @param	ropCode - the raster operation code.
 @param	rotateType - the rotate type.
 @return	u_short - 0:success,  others:error code.
 @note	.
 @bug	N/A.
*/
u_short	A8L_RotateImageBuftoBuf(u_short srcWidth, u_short srcHeight, 
				u_short srcBufferID, u_short srcx, u_short srcy, 
				u_short tgtBufferID, u_short tgtx, u_short tgty, 
				u_short ropCode, u_short rotateType)
{
	u_int	longtmp;
	u_short	cut_width, cut_height;
	u_short	src_startx, src_starty;
	
	if (srcBufferID == tgtBufferID ||
		srcWidth > MAX_WIDTH || srcHeight > MAX_HEIGHT || 
			srcBufferID > (MAX_BUFFER-1) || tgtBufferID > (MAX_BUFFER-1) ||
			srcx >= gsBuffer[srcBufferID].width || srcy >= gsBuffer[srcBufferID].height ||
			tgtx >= gsBuffer[tgtBufferID].width || tgty >= gsBuffer[tgtBufferID].height ||
			!srcWidth || !srcHeight || (rotateType & 0xf8)) {
		return GRH_PARAS_OUT_RANGE;
	}	

	if (gsBuffer[srcBufferID].colordepth != gsBuffer[tgtBufferID].colordepth) {
		if (gsBuffer[srcBufferID].colordepth != A8_DEPTH_8B || 
				gsBuffer[tgtBufferID].colordepth != A8_DEPTH_24B) {
			return GRH_PARAS_ERROR;
		}	
	}			
	
	if ((srcWidth + srcx) > gsBuffer[srcBufferID].width)
		cut_width = gsBuffer[srcBufferID].width - srcx;
	else	
		cut_width = srcWidth;

	if ((srcHeight + srcy) > gsBuffer[srcBufferID].height)
		cut_height = gsBuffer[srcBufferID].height - srcy;
	else	
		cut_height = srcHeight;

	src_startx = 0;
	src_starty = 0;

	switch (rotateType & 0x03) {
		case A8_RT_NORMAL:
			if ((tgtx + cut_width) > gsBuffer[tgtBufferID].width) {
				if (rotateType & 0x04) {
					src_startx = cut_width + tgtx - gsBuffer[tgtBufferID].width;
				}	
				cut_width = gsBuffer[tgtBufferID].width - tgtx;
			}	
			if ((tgty + cut_height) > gsBuffer[tgtBufferID].height)
				cut_height = gsBuffer[tgtBufferID].height - tgty;
			break;
		case A8_RT_RIGHT_90:
			if ((tgtx + cut_height) > gsBuffer[tgtBufferID].width) {
				src_starty = cut_height + tgtx - gsBuffer[tgtBufferID].width;
				cut_height = gsBuffer[tgtBufferID].width - tgtx;
			}
			if ((tgty + cut_width) > gsBuffer[tgtBufferID].height) {
				if (rotateType & 0x04) {
					src_startx = cut_width + tgty - gsBuffer[tgtBufferID].height;		
				}	
				cut_width = gsBuffer[tgtBufferID].height - tgty;
			}	
			break;
		case A8_RT_RIGHT_180:
			if ((tgtx + cut_width) > gsBuffer[tgtBufferID].width) {
				if (!(rotateType & 0x04)) {
					src_startx = cut_width + tgtx - gsBuffer[tgtBufferID].width;
				}	
				cut_width = gsBuffer[tgtBufferID].width - tgtx;
			}
			if ((tgty + cut_height) > gsBuffer[tgtBufferID].height) {
				src_starty = cut_height + tgty - gsBuffer[tgtBufferID].height;
				cut_height = gsBuffer[tgtBufferID].height - tgty;
			}	
			break;
		case A8_RT_RIGHT_270:
			if ((tgtx + cut_height) > gsBuffer[tgtBufferID].width)
				cut_height = gsBuffer[tgtBufferID].width - tgtx;
			if ((tgty + cut_width) > gsBuffer[tgtBufferID].height) {
				if (!(rotateType & 0x04)) {
					src_startx = cut_width + tgty - gsBuffer[tgtBufferID].height;
				}	
				cut_width = gsBuffer[tgtBufferID].height - tgty;
			}
			break;
		default:
			return GRH_PARAS_ERROR;
	}
			
	// Deal Source Buffer	
	longtmp = (gsBuffer[srcBufferID].startaddr)
		+ ((gsBuffer[srcBufferID].lineoffset) * (srcy + src_starty)) 
		+ ((srcx + src_startx) * (gsBuffer[srcBufferID].colordepth + 1));

	SetA8RegW(GRH_SRC_ADDR_0, (u_short)(longtmp&0xFFFF));
	SetA8RegW(GRH_SRC_ADDR_2, (u_short)(longtmp >> 16));
	longtmp = gsBuffer[srcBufferID].lineoffset;	

	SetA8RegW(GRH_SRC_OFFSET_0, (u_short)(longtmp&0xFFFF));
	SetA8RegW(GRH_SRC_OFFSET_2, (u_short)(longtmp >> 16));
	
	switch (rotateType & 0x03) {
		case A8_RT_NORMAL:
			// Deal Line offset
			longtmp = gsBuffer[tgtBufferID].lineoffset;
			SetA8RegW(GRH_DST_OFFSET_0, (u_short)(longtmp&0xFFFF));
			SetA8RegW(GRH_DST_OFFSET_2, (u_short)(longtmp >> 16));
			SetA8RegB(GRH_ROTATE_CTL, POS_LINE_OFFSET);

			if (rotateType & 0x4) {
				// Deal Pixel offset
				longtmp = 1 * (gsBuffer[tgtBufferID].colordepth + 1);	
				SetA8RegW(GRH_DST_PIXEL_OFFSET_0, NEG_PIXEL_OFFSET | 
												(u_short)longtmp);
				// Deal Destination Buffer	
				longtmp = (gsBuffer[tgtBufferID].startaddr)
					+ ((gsBuffer[tgtBufferID].lineoffset) * (tgty)) 
					+ ((tgtx + cut_width - 1) * (gsBuffer[tgtBufferID].colordepth + 1));
			}	
			else {
				// Deal Pixel offset
				longtmp = 1 * (gsBuffer[tgtBufferID].colordepth + 1);	
				SetA8RegW(GRH_DST_PIXEL_OFFSET_0, POS_PIXEL_OFFSET | 
												(u_short)longtmp);
				// Deal Destination Buffer	
				longtmp = (gsBuffer[tgtBufferID].startaddr)
					+ ((gsBuffer[tgtBufferID].lineoffset) * (tgty)) 
					+ ((tgtx) * (gsBuffer[tgtBufferID].colordepth + 1));
			}
			break;
		case A8_RT_RIGHT_90:
			// Deal Line offset
			longtmp = 1 * (gsBuffer[tgtBufferID].colordepth + 1);	
			SetA8RegW(GRH_DST_OFFSET_0, (u_short)(longtmp&0xFFFF));
			SetA8RegW(GRH_DST_OFFSET_2, (u_short)(longtmp >> 16));
			SetA8RegB(GRH_ROTATE_CTL, NEG_LINE_OFFSET);

			if (rotateType & 0x4) {
				// Deal Pixel offset
				longtmp = gsBuffer[tgtBufferID].lineoffset;
				SetA8RegW(GRH_DST_PIXEL_OFFSET_0, NEG_PIXEL_OFFSET | 
												(u_short)longtmp);
				// Deal Destination Buffer	
				longtmp = (gsBuffer[tgtBufferID].startaddr)
					+ ((gsBuffer[tgtBufferID].lineoffset) * (tgty + cut_width - 1)) 
					+ ((tgtx + cut_height - 1) * (gsBuffer[tgtBufferID].colordepth + 1));
			}
			else {	
				// Deal Pixel offset
				longtmp = gsBuffer[tgtBufferID].lineoffset;
				SetA8RegW(GRH_DST_PIXEL_OFFSET_0, POS_PIXEL_OFFSET | 
												(u_short)longtmp);
				// Deal Destination Buffer	
				longtmp = (gsBuffer[tgtBufferID].startaddr)
					+ ((gsBuffer[tgtBufferID].lineoffset) * (tgty)) 
					+ ((tgtx + cut_height - 1) * (gsBuffer[tgtBufferID].colordepth + 1));
			}									
    		break;
    	case A8_RT_RIGHT_180:
			// Deal Line offset
			longtmp = gsBuffer[tgtBufferID].lineoffset;
			SetA8RegW(GRH_DST_OFFSET_0, (u_short)(longtmp&0xFFFF));
			SetA8RegW(GRH_DST_OFFSET_2, (u_short)(longtmp >> 16));
			SetA8RegB(GRH_ROTATE_CTL, NEG_LINE_OFFSET);
			
			if (rotateType & 0x04) {
				// Deal Pixel offset
				longtmp = 1 * (gsBuffer[tgtBufferID].colordepth + 1);	
				SetA8RegW(GRH_DST_PIXEL_OFFSET_0, POS_PIXEL_OFFSET | 
												(u_short)longtmp);
	    		// Deal Destination Buffer	
				longtmp = (gsBuffer[tgtBufferID].startaddr)
					+ ((gsBuffer[tgtBufferID].lineoffset) * (tgty + cut_height - 1)) 
					+ ((tgtx) * (gsBuffer[tgtBufferID].colordepth + 1));
			}
			else {
				// Deal Pixel offset
				longtmp = 1 * (gsBuffer[tgtBufferID].colordepth + 1);	
				SetA8RegW(GRH_DST_PIXEL_OFFSET_0, NEG_PIXEL_OFFSET | 
												(u_short)longtmp);
	    		// Deal Destination Buffer	
				longtmp = (gsBuffer[tgtBufferID].startaddr)
					+ ((gsBuffer[tgtBufferID].lineoffset) * (tgty + cut_height - 1)) 
					+ ((tgtx + cut_width - 1) * (gsBuffer[tgtBufferID].colordepth + 1));
			}										
    		break;
		case A8_RT_RIGHT_270:
			// Deal Line offset
			longtmp = 1 * (gsBuffer[tgtBufferID].colordepth + 1);	
			SetA8RegW(GRH_DST_OFFSET_0, (u_short)(longtmp&0xFFFF));
			SetA8RegW(GRH_DST_OFFSET_2, (u_short)(longtmp >> 16));
			SetA8RegB(GRH_ROTATE_CTL, POS_LINE_OFFSET);

			if (rotateType & 0x04) {
				// Deal Pixel offset
				longtmp = gsBuffer[tgtBufferID].lineoffset;
				SetA8RegW(GRH_DST_PIXEL_OFFSET_0, POS_PIXEL_OFFSET | 
												(u_short)longtmp);
				// Deal Destination Buffer	
				longtmp = (gsBuffer[tgtBufferID].startaddr)
					+ ((gsBuffer[tgtBufferID].lineoffset) * (tgty)) 
					+ ((tgtx) * (gsBuffer[tgtBufferID].colordepth + 1));
			}	
			else {
				// Deal Pixel offset
				longtmp = gsBuffer[tgtBufferID].lineoffset;
				SetA8RegW(GRH_DST_PIXEL_OFFSET_0, NEG_PIXEL_OFFSET | 
												(u_short)longtmp);
				// Deal Destination Buffer	
				longtmp = (gsBuffer[tgtBufferID].startaddr)
					+ ((gsBuffer[tgtBufferID].lineoffset) * (tgty + cut_width - 1)) 
					+ ((tgtx) * (gsBuffer[tgtBufferID].colordepth + 1));
			}
			break;
	}

	// Deal Destination Buffer	
	SetA8RegW(GRH_DST_ADDR_0, (u_short)(longtmp&0xFFFF));
	SetA8RegW(GRH_DST_ADDR_2, (u_short)(longtmp >> 16));
	
	SetA8RegW(GRH_BLT_WIDTH_0, cut_width);
	SetA8RegW(GRH_BLT_HEIGHT_0, cut_height);
	SetA8RegB(GRH_COLOR_FORMAT, ((gsBuffer[tgtBufferID].colordepth) << 2)
									| (gsBuffer[srcBufferID].colordepth));
	SetA8RegB(GRH_ROP, ropCode & GRH_ROP_MASK);
	SetA8RegB(GRH_BITBLT_CTL, GRH_OP_DO_ROP | GRH_MEM_2_MEM | GRH_LEFT_TOP_RIGHT);

	if (gbTransActive) 
		SetA8RegB(GRH_BITBLT_CTL, GetA8RegB(GRH_BITBLT_CTL) | GRH_OP_DO_TRANSP);

	SetA8RegB(GRH_ROTATE_CTL, GetA8RegB(GRH_ROTATE_CTL) | ROTATE_MODE);

	SetA8RegB(GRH_ENABLE_CTL, GRH_ENABLE);
	while (GetA8RegB(GRH_ENABLE_CTL) & GRH_ENABLE);

	SetA8RegB(GRH_ROTATE_CTL, 0);

	return GRH_NO_ERROR;	
}


/**
 @fn		u_short	ExpandMonoBitmapFIFOtoBuf(u_char *bmpAddr, u_short bmpWidth, u_short bmpHeight, u_short bufferID, u_short tgtx, u_short tgty)
 @brief	Expand (Copy) Mono Bitmap from FIFO to buffer.
 @param	bmpAddr - address of bitmap buffer in host.
 @param	bmpWidth - the width of the mono bitmap.
 @param	bmpHeight - the height of the mono bitmap.
 @param	bufferID - the ID of the buffer to drawing.
 @param	tgtx - the X position in the dest. buffer.
 @param	tgty - the Y position in the dest. buffer.
 @return	u_short - 0:success,  others:error code.
 @note	.
 @bug	N/A.
*/
u_short	ExpandMonoBitmapFIFOtoBuf(u_char *bmpAddr, u_short bmpWidth, u_short bmpHeight, u_short bufferID, u_short tgtx, u_short tgty)
{
	u_int	longtmp;
	u_short	cut_width, cut_height;
	u_short	shorttmp;
	u_short	fifocount, transfercount, j;
	u_short	cut_width_short, cut_width_byte;
	u_short	width_byte;
	u_char	*pchartmp;
	s_int		total_count;
	
	if (bmpWidth > MAX_WIDTH || bmpHeight > MAX_HEIGHT || 
			bufferID > (MAX_BUFFER-1) ||
			tgtx >= gsBuffer[bufferID].width || tgty >= gsBuffer[bufferID].height ||
			!bmpWidth || !bmpHeight) {
		return GRH_PARAS_OUT_RANGE;
	}	
	
	if ((bmpWidth + tgtx) > gsBuffer[bufferID].width)
		cut_width = gsBuffer[bufferID].width - tgtx;
	else	
		cut_width = bmpWidth;

	if ((bmpHeight + tgty) > gsBuffer[bufferID].height)
		cut_height = gsBuffer[bufferID].height - tgty;
	else	
		cut_height = bmpHeight;

	// set the graphics control parameter	
	SetA8RegW(GRH_FIFO_WR_TH, FIFO_RD_WR_THRESHOLD);
	SetA8RegW(GRH_BLT_WIDTH_0, cut_width);
	SetA8RegW(GRH_BLT_HEIGHT_0, cut_height);
	SetA8RegB(GRH_COLOR_FORMAT, GRH_BIT_DAT_L_2_M
									|((gsBuffer[bufferID].colordepth) << 2));
	// Expand All of the Data in FIFO Bitmap FIFO 									
	SetA8RegB(GRH_ROP, GRH_ROP_WHITENESS/*0x0f*/);
	SetA8RegB(GRH_BITBLT_CTL, GRH_COLOR_EXPANSION | GRH_HOST_2_MEM | GRH_LEFT_TOP_RIGHT);

	if (gbTransActive) 
		SetA8RegB(GRH_BITBLT_CTL, GetA8RegB(GRH_BITBLT_CTL) | GRH_OP_DO_TRANSP);
	
	// Deal Destination Buffer
	longtmp = (gsBuffer[bufferID].startaddr)
				+ ((gsBuffer[bufferID].lineoffset) * (tgty))
				+ ((tgtx) * (gsBuffer[bufferID].colordepth + 1));

	SetA8RegW(GRH_DST_ADDR_0, (u_short)(longtmp & 0xFFFF));
	SetA8RegW(GRH_DST_ADDR_2, (u_short)(longtmp >> 16));

	longtmp = gsBuffer[bufferID].lineoffset;	

	SetA8RegW(GRH_DST_OFFSET_0, (u_short)(longtmp & 0xFFFF));
	SetA8RegW(GRH_DST_OFFSET_2, (u_short)(longtmp >> 16));
		
	pchartmp = (u_char *)bmpAddr;
		
	if (!(cut_width & 0x0007))
		cut_width_byte = cut_width >> 3;
	else 	
		cut_width_byte = (cut_width >> 3) + 1;
	
	if (!(cut_width_byte & 0x01))
		cut_width_short = cut_width_byte >> 1;
	else 	
		cut_width_short = (cut_width_byte >> 1) + 1;

	if (!(bmpWidth & 0x0007))
		width_byte = bmpWidth >> 3;
	else 	
		width_byte = (bmpWidth >> 3) + 1;

	total_count = (cut_width_short * cut_height);
	j = 0;	

	// need to reset the FIFO at first to ensure that there are no
	// data in the FIFO, or check the FIFO empty status
	SetA8RegB(GRH_ENABLE_CTL, GRH_FIFO_RESET);
	if (GetA8RegB(GRH_FIFO_UNWR_WORD) != FIFO_DEPTH)
		return GRH_HW_ERROR;

	SetA8RegB(GRH_ENABLE_CTL, GRH_ENABLE);

	do 
	{
		// wait for FIFO to be under threshold
		while (!(GetA8RegB(GRH_FIFO_STATUS) & FIFO_WR_UNDER_THRES));
		fifocount = GetA8RegB(GRH_FIFO_UNWR_WORD) & 0x1f;

		if (fifocount < total_count)
			transfercount = fifocount;
		else
			transfercount = total_count;
	
		for (; transfercount > 0; transfercount--, total_count--) 
		{
			if (j == (cut_width_short -1)) 
			{
				if (cut_width_byte & 0x01) 
				{
					SetA8RegW(GRH_FIFO_DAT_PORT_0, (u_short)(*(pchartmp++)));
				}
				else 
				{
					shorttmp = *(pchartmp++);
					shorttmp |= (*(pchartmp++) << 8);
					SetA8RegW(GRH_FIFO_DAT_PORT_0, shorttmp);
				}	
				j = 0;
				pchartmp += (width_byte - cut_width_byte);
			}	
			else 
			{
				shorttmp = *(pchartmp++);
				shorttmp |= (*(pchartmp++) << 8);
				SetA8RegW(GRH_FIFO_DAT_PORT_0, shorttmp);
				j += 1;
			}	
		}	
	} while (total_count > 0);

	// wait for the FIFO to be empty and graphic engine to stop
	while (GetA8RegB(GRH_ENABLE_CTL) & GRH_ENABLE);

	return GRH_NO_ERROR;
}

u_short	A8L_CopyGrapFIFOtoBuf(u_short *srcAddr, u_short width, u_short height, 
			u_short colorDepth, u_short bufferID, u_short tgtx, u_short tgty, 
			u_short ropCode)
{
	u_int	longtmp;
	u_short	*pshorttmp;
	u_short	fifocount, transfercount;
	u_short	cut_width, cut_height;
	u_short	cut_width_short, width_short;
	u_short	j;
//	u_short	temp,temp1,temp2;
	s_int		total_count;
	
	if (width > MAX_WIDTH || height > MAX_HEIGHT || 
			bufferID > (MAX_BUFFER-1) ||
			tgtx >= gsBuffer[bufferID].width || tgty >= gsBuffer[bufferID].height ||
			!width || !height)  {
		return GRH_PARAS_OUT_RANGE;
	}	

	if (colorDepth != A8_DEPTH_8B && colorDepth != A8_DEPTH_16B && colorDepth != A8_DEPTH_24B)
		return GRH_PARAS_ERROR;
	
	if (colorDepth != (gsBuffer[bufferID].colordepth)) {
		if (colorDepth != A8_DEPTH_8B || gsBuffer[bufferID].colordepth != A8_DEPTH_24B) {
			return GRH_FUNX_ERROR;
		}	
	}			

	if ((width + tgtx) > gsBuffer[bufferID].width)
		cut_width = gsBuffer[bufferID].width - tgtx;
	else	
		cut_width = width;

	if ((height + tgty) > gsBuffer[bufferID].height)
		cut_height = gsBuffer[bufferID].height - tgty;
	else	
		cut_height = height;

	// set the graphics control parameter	
	SetA8RegW(GRH_FIFO_WR_TH, FIFO_RD_WR_THRESHOLD);
	SetA8RegW(GRH_BLT_WIDTH_0, cut_width);
	SetA8RegW(GRH_BLT_HEIGHT_0, cut_height);
	SetA8RegB(GRH_ROP, ropCode & GRH_ROP_MASK);
	SetA8RegB(GRH_COLOR_FORMAT, ((gsBuffer[bufferID].colordepth) << 2)|(colorDepth));
	SetA8RegB(GRH_BITBLT_CTL, GRH_OP_DO_ROP | GRH_HOST_2_MEM | GRH_LEFT_TOP_RIGHT);

	if (gbTransActive) 
		SetA8RegB(GRH_BITBLT_CTL, GetA8RegB(GRH_BITBLT_CTL) | GRH_OP_DO_TRANSP);

	// Deal Destination Buffer
	longtmp = (gsBuffer[bufferID].startaddr)
				+ ((gsBuffer[bufferID].lineoffset) * (tgty))
				+ ((tgtx) * (gsBuffer[bufferID].colordepth + 1));

	SetA8RegW(GRH_DST_ADDR_0, (u_short)(longtmp & 0xFFFF));
	SetA8RegW(GRH_DST_ADDR_2, (u_short)(longtmp >> 16));

	longtmp = gsBuffer[bufferID].lineoffset;	

	SetA8RegW(GRH_DST_OFFSET_0, (u_short)(longtmp & 0xFFFF));
	SetA8RegW(GRH_DST_OFFSET_2, (u_short)(longtmp >> 16));

	SetA8RegW(GRH_SRC_ADDR_0, 0);
	SetA8RegW(GRH_SRC_ADDR_2, 0);

	pshorttmp = srcAddr;

		SetA8RegB(0x6b1e, 0x0);
		/* begin of AITVES */
		SetA8RegB(0x5040, GetA8RegB(0x5040) & 0xAE);	//0: LUT4 is used instead of bypassed
		/* end of AITVES */
		SetA8RegW(0x505a, 0x1);
		SetA8RegW(0x505c, 0xb0);

		SetA8RegW(0x504a, 0x0);
		SetA8RegW(0x509a, 0x1);
        if ( bufferID == 3 )
		SetA8RegW(0x509c, 0x8A);
        else
		SetA8RegW(0x509c, 0xB0);

		SetA8RegW(0x6b5c, 0xB0);
		cut_width_short = (cut_width+1) >> 1;		
		width_short = (width+1) >>1;

	total_count = (cut_width_short * cut_height);
	j = 0;



	// need to reset the FIFO at first to ensure that there are no
	// data in the FIFO, or check the FIFO empty status
	SetA8RegB(GRH_ENABLE_CTL, GRH_FIFO_RESET);
	if (GetA8RegB(GRH_FIFO_UNWR_WORD) != FIFO_DEPTH)
		return GRH_HW_ERROR;

	SetA8RegB(GRH_ENABLE_CTL, GRH_ENABLE);

	do {
		while (!(GetA8RegB(GRH_FIFO_STATUS) & FIFO_WR_UNDER_THRES));
		fifocount = GetA8RegB(GRH_FIFO_UNWR_WORD) & 0x1f;

		if (fifocount < total_count)
			transfercount = fifocount;
		else
			transfercount = total_count;
	
		for ( ; transfercount > 0; transfercount--, total_count--) {
			if (j == (cut_width_short -1)) {
		
				SetA8GrpFIFODataW(&pshorttmp);
				
				pshorttmp += (width_short - cut_width_short);
				j = 0;
			}
			else {
			
				SetA8GrpFIFODataW(&pshorttmp);	
				j+=2;

			}	
		}	
	} while (total_count > 0);

	// wait for the FIFO to be empty and graphic engine to stop
	while (GetA8RegB(GRH_ENABLE_CTL) & GRH_ENABLE);
	return GRH_NO_ERROR;
}


u_short	A8L_CopyGrapFullFIFOtoBuf(u_short *srcAddr, u_short width, u_short height, 
			u_short colorDepth, u_short bufferID, u_short tgtx, u_short tgty, 
			u_short ropCode)
{
	u_int	longtmp;
	u_short	*pshorttmp;
	u_short	fifocount, transfercount;
	u_short	cut_width, cut_height;
	u_short	cut_width_short, width_short;
	u_short	j;
//	u_short	temp,temp1,temp2;
	s_int		total_count;
	u_short *pTmpAddr;
	//u_int	nLoop;
	u_int	nTimeout=0;
	
	
	if ((width + tgtx) > gsBuffer[bufferID].width+1000)
		cut_width = gsBuffer[bufferID].width - tgtx;
	else	
		cut_width = 138;

	if ((height + tgty) > gsBuffer[bufferID].height+1000)
		cut_height = gsBuffer[bufferID].height - tgty;
	else	
		cut_height = 40;

	width=cut_width;
	height=cut_height;

	#if 0
	A8L_SetBufferAttribute(3,176,180,176<<1,0, 0x14410);
	A8L_BindBufferToWindow(3, A8_MAIN_WINDOW);
	A8L_SetWindowToDisplay(A8_MAIN_WINDOW, 0,0,176,180,0,40,A8_RT_NORMAL);
	A8L_SetBufferAttribute(2,138,40,138<<1,0, 0x12E80);	
	A8L_BindBufferToWindow(2, A8_OVERLAY_WINDOW);
	A8L_SetWindowToDisplay(A8_OVERLAY_WINDOW, 0,0,138,40,0,0,A8_RT_NORMAL);
	A8L_SetWindowTransparency(A8_MAIN_WINDOW, 1, ((0xF81F&0xe000)>>8)|((0xF81F&0x700)>>6)|((0xF81F&0x18)>>3)); 
	A8L_SetWindowTransparency(A8_OVERLAY_WINDOW, 1, ((0xF81F&0xe000)>>8)|((0xF81F&0x700)>>6)|((0xF81F&0x18)>>3)); 
	#endif
	
	// set the graphics control parameter	
	SetA8RegW(GRH_FIFO_WR_TH, FIFO_RD_WR_THRESHOLD);
	SetA8RegW(GRH_BLT_WIDTH_0, cut_width);
	SetA8RegW(GRH_BLT_HEIGHT_0, cut_height);
	SetA8RegB(GRH_ROP, ropCode & GRH_ROP_MASK);
	SetA8RegB(GRH_COLOR_FORMAT, ((gsBuffer[bufferID].colordepth) << 2)|(colorDepth));
	SetA8RegB(GRH_BITBLT_CTL, GRH_OP_DO_ROP | GRH_HOST_2_MEM | GRH_LEFT_TOP_RIGHT);

	if (gbTransActive) 
		SetA8RegB(GRH_BITBLT_CTL, GetA8RegB(GRH_BITBLT_CTL) | GRH_OP_DO_TRANSP);

	// Deal Destination Buffer
	longtmp = (gsBuffer[bufferID].startaddr)
				+ ((gsBuffer[bufferID].lineoffset) * (tgty))
				+ ((tgtx) * (gsBuffer[bufferID].colordepth + 1));

	SetA8RegW(GRH_DST_ADDR_0, (u_short)(longtmp & 0xFFFF));
	SetA8RegW(GRH_DST_ADDR_2, (u_short)(longtmp >> 16));

	longtmp = gsBuffer[bufferID].lineoffset;	

	SetA8RegW(GRH_DST_OFFSET_0, (u_short)(longtmp & 0xFFFF));
	SetA8RegW(GRH_DST_OFFSET_2, (u_short)(longtmp >> 16));

	SetA8RegW(GRH_SRC_ADDR_0, 0);
	SetA8RegW(GRH_SRC_ADDR_2, 0);

	pshorttmp = srcAddr;
	pTmpAddr = srcAddr;
		SetA8RegB(0x6b1e, 0x0);	//format control
		
		/* begin of AITVES */
		SetA8RegB(0x5040, GetA8RegB(0x5040) & 0xAE);	//0: LUT4 is used instead of bypassed
		/* end of AITVES */
		
		SetA8RegW(0x509a, 0x1);	//Low byte of overlay window pixel offset
		SetA8RegW(0x509c, width);	//Low byte of overlay window row offset
		SetA8RegW(0x6b5c, width);	//BitBLT Destination offset address
		cut_width_short = (cut_width+1) >> 1;		
		width_short = (width+1) >>1;

	total_count = (cut_width_short * cut_height);
	j = 0;

	// need to reset the FIFO at first to ensure that there are no
	// data in the FIFO, or check the FIFO empty status
	SetA8RegB(GRH_ENABLE_CTL, GRH_FIFO_RESET);
	if (GetA8RegB(GRH_FIFO_UNWR_WORD) != FIFO_DEPTH)
		return GRH_HW_ERROR;

	SetA8RegB(GRH_ENABLE_CTL, GRH_ENABLE);

	do 
	{
		while (!(GetA8RegB(GRH_FIFO_STATUS) & FIFO_WR_UNDER_THRES));
		fifocount = GetA8RegB(GRH_FIFO_UNWR_WORD) & 0x1f;

		if (fifocount < total_count)
			transfercount = fifocount;
		else
			transfercount = total_count;
	
		for ( ; transfercount > 0; transfercount--, total_count--)
		{
			if (j >= (cut_width_short -1)) 
			{
				//printf("Short Addr:0x%x \r\n",pshorttmp);						
				SetA8GrpFIFODataW(&pshorttmp);
				//for(nLoop=cut_width_short;nLoop<(176/2);++nLoop)

				pTmpAddr=pTmpAddr+(176);
				//printf("Short Addr:0x%x ,TmpAddr:0x%x, j=0x%x, cut_width:%x\r\n",pshorttmp,pTmpAddr,j,cut_width_short);				
				pshorttmp=pTmpAddr;
				//pshorttmp+=(176-2*width_short);

				//pshorttmp += (width_short - cut_width_short);
				j = 0;
			}
			else 
			{
				SetA8GrpFIFODataW(&pshorttmp);	
				j++;
				//j+=2;

			}	
		}	
	} while (total_count > 0);
	
	// wait for the FIFO to be empty and graphic engine to stop
	while ((GetA8RegB(GRH_ENABLE_CTL) & GRH_ENABLE) && nTimeout<=0x500000)
	{
		++nTimeout;
	}		

	A8L_CopyGrapFIFOtoBuf(srcAddr+176*40,176,180,0,3,0,0,0xc);
	return GRH_NO_ERROR;
}

u_short	A8L_SetGrapMainTransparency( u_short IsActive, u_int TransparentColor )
{
	u_short Temp;
	
	Temp = GetA8RegB( 0x5062 );
	if ( IsActive != 0 )
		SetA8RegB( 0x5062, Temp | 0x01 );
	else
		SetA8RegB( 0x5062, Temp & 0xFE );
	Temp = ((TransparentColor&0xe000)>>8)|((TransparentColor&0x700)>>6)|((TransparentColor&0x18)>>3);
	SetA8RegW( 0x5052, Temp );
	return A8_NO_ERROR;
}
u_short	A8L_SetGrapIconTransparency( u_short IsActive, u_int TransparentColor )
{
	u_short Temp;
	
	Temp = GetA8RegB( 0x6C1C );
	if ( IsActive != 0 )
		SetA8RegB( 0x6C1C, Temp | 0x01 );
	else
		SetA8RegB( 0x6C1C, Temp & 0xFE );
	Temp = ((TransparentColor&0xe000)>>8)|((TransparentColor&0x700)>>6)|((TransparentColor&0x18)>>3);
	SetA8RegW( 0x6C10, Temp );
	return A8_NO_ERROR;
}
