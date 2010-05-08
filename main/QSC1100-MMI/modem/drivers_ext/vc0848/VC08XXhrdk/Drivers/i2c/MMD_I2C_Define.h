 /*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]            : MMD_I2C_Define.h
 * [Description]          : I2C Driver Internal Definition
 * [Author]                 : An Xinfang
 * [Date Of Creation]  : 2007-04-11 
 * [Platform]              : ARM
 * [Note]                   :  None   
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date                      Author         Modifications
 * ------------------------------------------------------------------------------
 * 2007-04-11         An Xinfang       Created
 *******************************************************************************/
 /*******************************************************************************
 * This source code has been made available to you by VIMICRO on an
 * AS-IS basis. Anyone receiving this source code is licensed under VIMICRO
 * copyrights to use it in any way he or she deems fit, including copying it,
 * modifying it, compiling it, and redistributing it either with or without
 * modifications. Any person who transfers this source code or any derivative
 * work must include the VIMICRO copyright notice and this paragraph in
 * the transferred software.
 *******************************************************************************/
 
#ifndef _MMD_I2C_DEFINE_H_
#define _MMD_I2C_DEFINE_H_

#ifdef _cplusplus
extern "C" {
#endif



/*Define I2C Register Address*/
#define I2C_CTRL 		0x60124000
#define I2C_Command 	0x60124004
#define I2C_TXR 		       0x60124008
#define I2C_RXR  		       0x6012400c
#define I2C_Status   	       0x60124010


/*Define Key Word of I2C_Control Register*/
#define I2C_EN 			0x08000000          /*enable I2C core*/
#define I2C_IEN			0x04000000          /*enable I2C Interrupt*/
#define I2C_SCL5F 		0x00ffffff           /*set I2C speed*/


/*Define Key Word of I2C_Command Register*/
#define I2C_STA    		0x00000020          /*generate start signal*/
#define I2C_STO   		0x00000010          /* generate stop signal*/
#define I2C_WR     		0x00000008          /* write to slave*/
#define I2C_RD      	       0x00000004          /* read from slave*/
#define I2C_ACK    		0x00000002          /*when a receiver,send ACK(Low) or NACK(high)*/
#define I2C_IACK   	       0x00000001          /* interrupt acknowledges,when set,clear a pending interrupt*/


/*Define Key Word of I2C_Status Register*/
#define I2C_IF                   0x00000010          /*interrupt flag,is set when one byte transfer has been completed*/
#define I2C_AL                   0x00000008         /*Arbitration lost flag,is set when arbitration lost*/
#define I2C_RxACK   	       0x00000004          /* received ACK form slave,low active*/
#define I2C_Busy               0x00000002          /*I2C bus busy,high active*/
#define I2C_TIP                 0x00000001          /*transfer in progress,high active*/
 #ifdef _cplusplus
	}
#endif

#endif/*_MMD_I2C_DEFINE_H_*/
