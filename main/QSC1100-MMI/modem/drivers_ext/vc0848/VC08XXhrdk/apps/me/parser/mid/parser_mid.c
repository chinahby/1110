/*======================================================================
*                                                                       
*                Copyright (C) 2008 Vimicro CO.,LTD     		 
* File Name: 	Parser_mid.c
* User for : 	848
* Creat By : 	Gaosong                                                                   
* CrateDate: 	2008/05/22
* Last modify : Gaosong 2008-xx-xx     
*
* release history:
* 		version 0.1 release by Gaosong
=======================================================================*/
#include "parser_mid.h"
#include "parser_mid_vsf.h"
#include "..\..\..\..\inc\mmd_file_driver.h"
#if ME_KM_PARSER_MIDI_EN

#if ME_KM_OBJ_STACK
static me_parser_t	_g_parser_mid;
#endif

#ifndef _PC_SIM
ME_U32	parser_mid_release(void *fileinfor, me_audio_infor *audio_infor, me_video_infor *video_infor);
#else
ME_U32	parser_mid_release(void *fileinfor);
#endif

unsigned int mid_frame_count;
unsigned int last_frame_flag = 0;
/*******************************************************************************************
Description:
	parser mid initialize
Parameters:
       void *mediafile:		file handle(point)
       fs_callbacks *fs_filefunc:		file operate interface structor point
       void **fileinfor:		file information structor create by parser
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Gaosong			       2008.05.22	   create first version
********************************************************************************************/

static ME_U32 ReadVarLen(ME_U08 *mid_file, ME_U32 *dDeltaTime)
{
	ME_U08 bTemp;
	ME_U08 *pbCurData;
	ME_U32 dLength;
	
	pbCurData = mid_file;
	bTemp = *pbCurData++;
	dLength=1;
	if ( (*dDeltaTime = (ME_U32)bTemp)&0x80 ) 
	{
		*dDeltaTime &= 0x7f;
		do 
		{
			*dDeltaTime = (*dDeltaTime << 7) + ( (bTemp = *pbCurData++)&0x7f );
			dLength++;
		} 
		while(bTemp&0x80);
	}
	return dLength;	
}

static ME_U32 WriteVarLen(ME_U32 value, ME_PU08 writeBuf)
{
	ME_U32 dCurData;
	ME_U32 bTemp;
	ME_U32 writeLens=0;
	dCurData = value;
	bTemp = dCurData & 0x7F;
	
	while ( (dCurData >>= 7) )
	{
		bTemp <<= 8;
		bTemp |= ((dCurData & 0x7F) | 0x80);
	}	
	while (1)
	{
		*writeBuf = bTemp;
		writeBuf++;
		writeLens++;
		if (bTemp & 0x80)
		{			
			bTemp >>= 8;
		}
		else
			break;
	}
	return writeLens;
}

/*******************************************************************************************
Description:
	 accurately access time of the ape file
Parameters:
        void *fileinfor: ape infor stuctor point (include file handle)
        ME_U32 pos: user want time(ms)
	 ME_U32* accesspos: really access time (ms)
Return:	
	ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1		    Gaosong			       2008.05.22		create first version
********************************************************************************************/
static ME_U32 SeekMsg(VM_MIDI_PARSER_INFO *file, ME_U32 dPos)
{
	SEEK_INFO *g_MidiInfo = (SEEK_INFO*)file->vm_input.SeekInfo;
	ME_PU08 pbCurdata = file->mid_Data+file->mid_DataStartPointBackup;
	ME_U32 division = file->mid_Division;

	ME_U08 bTempLen, bMsgID, bMeltaID, bPreMsg, bStatus, bChn, bMsgLen;
	ME_U32 dVarLen, dDeltatime, dPreDeltatime, dLastTempoTicks, dTempoCode, dMeltaLen;
	ME_U32 dCurTime;
	ME_U32 i;

	g_MidiInfo->dTempoMS = 500000;
	
	dLastTempoTicks = 0;
	bTempLen = 0;
	dCurTime = 0;
	dPreDeltatime = 0;
	while( dCurTime <= dPos*1000 )
	{
		dVarLen = ReadVarLen(pbCurdata, &dDeltatime); //dVarLen: the num of bytes which describe the dDeltatime
		
		dDeltatime += dPreDeltatime;
		dPreDeltatime = dDeltatime;
			/*for(i=0; i<=dVarLen; i++)
			{
				pbCurdata++;
			}*/
		    pbCurdata+=dVarLen;
			bMsgID = *pbCurdata;
			
			if (bMsgID == MELTA_EVENT)    
			{
				pbCurdata++;
				bMeltaID = *pbCurdata;
				if (bMeltaID == MELTA_TEMPO)  //tempo
				{
					dTempoCode=0;
					if ( *(pbCurdata+1)==0x03 ) 
					{
						for(i=0; i<3; i++)
						{
							dTempoCode = dTempoCode*0x100 + *(pbCurdata+2+i);
						}
						g_MidiInfo->dTempoMS = dTempoCode;
					}										
				}
				pbCurdata++;
 				dVarLen = ReadVarLen(pbCurdata, &dMeltaLen);
				/*for(i=0; i<=dVarLen; i++)
				{
					pbCurdata++;
				}
				for(i=0; i<dMeltaLen; i++)
				{	
					pbCurdata++;
				}*/
				pbCurdata+=(dVarLen+dMeltaLen);
			}
			else if (bMsgID == SYSEXCLUDE_EVENT) 
			{
				while(*pbCurdata != 0xf7)	    //0xf7 the end signal of sysexclude_event
				{
					pbCurdata++;
				}
				pbCurdata++;				
			}
			else
			{
				if (bMsgID<0x80 || bMsgID>0xEF) 
				{
					bMsgID = bPreMsg;       //midi special function: Running Status
					bTempLen = 1;
				}
				if( (bMsgID&0xf0)==MSG_PROGCHANGE || (bMsgID&0xf0)==MSG_CHNPRESSURE )
					bMsgLen= 2-bTempLen;
				else
					bMsgLen = 3-bTempLen;
				bPreMsg = bMsgID;
				bTempLen = 0;

				bStatus = pbCurdata[0]&0xf0;  //2
				bChn = pbCurdata[0]&0x0f;

				switch(bStatus)
				{
				case MSG_PITCHBEND:
					g_MidiInfo->ChnInfo[bChn].bPitchBendH = pbCurdata[2]&0x7f;
					g_MidiInfo->ChnInfo[bChn].bPitchBendL = pbCurdata[1]&0x7f;
					break;
					
				case MSG_PROGCHANGE:
					g_MidiInfo->ChnInfo[bChn].bTimbre = pbCurdata[1];
					break;
					
				case MSG_CTRLCHANGE:
					
					switch(pbCurdata[1])
					{
					case CTRL_MODULATION:
						g_MidiInfo->ChnInfo[bChn].bModulation = pbCurdata[2];
						break;
						
					case CTRL_VOLUME:
						g_MidiInfo->ChnInfo[bChn].bVolume = pbCurdata[2];
						break;
						
					case CTRL_PANPOT:
						g_MidiInfo->ChnInfo[bChn].bPan= pbCurdata[2];
						break;
						
					case CTRL_EXPRESS:
						g_MidiInfo->ChnInfo[bChn].bExpression = pbCurdata[2];
						break;
						
					case CTRL_HOLD1:
						g_MidiInfo->ChnInfo[bChn].bHold1= pbCurdata[2];
						break;
						
					case CTRL_RESETCTRLS:
						g_MidiInfo->wRPN[bChn] = 0x7f7f;
						//g_MidiInfo.ChnInfo[bChn].volume     	= 0x7f;
						g_MidiInfo->ChnInfo[bChn].bExpression	= 0x7f;
						g_MidiInfo->ChnInfo[bChn].bModulation 	= 0;
						//g_MidiInfo.ChnInfo[bChn].bendrange  	= 0x02;  //bendrange= 0x02*100;
						//g_MidiInfo.ChnInfo[bChn].panpot     	= 0x40;
						g_MidiInfo->ChnInfo[bChn].bHold1      	= 0;
						
						break;
						
					case CTRL_RPNLSB:
						g_MidiInfo->wRPN[bChn] = ((g_MidiInfo->wRPN[bChn] & 0x7F00) | ((ME_U16)pbCurdata[2]));
						break;
						
					case CTRL_RPNMSB:
						g_MidiInfo->wRPN[bChn] = ((g_MidiInfo->wRPN[bChn] & 0x007F) | ((ME_U16)pbCurdata[2])<<8);
						break;
						
					case CTRL_DATAENTRYMSB:
						if (g_MidiInfo->wRPN[bChn] == 0) 
							g_MidiInfo->ChnInfo[bChn].bBendRage= pbCurdata[2];
						break;
						
					default:
						break;
					}
					
					default:
						break;
				}
			    pbCurdata += bMsgLen;
			}
		dCurTime += (dDeltatime-dLastTempoTicks) * (g_MidiInfo->dTempoMS)/division; 
		dLastTempoTicks = dDeltatime;
	}
	 while (1)
         {
                  dVarLen = ReadVarLen(pbCurdata, &dDeltatime);
                  //pbCurdata+=dVarLen;
                  bMsgID = *(pbCurdata+dVarLen);
                  if (bMsgID == MELTA_EVENT) 
                           break;

                  else if (bMsgID == SYSEXCLUDE_EVENT)
                           break;
                  else 
                  {
                           if (bMsgID<0x80 || bMsgID>0xEF) 
                           {
                                    bMsgID = bPreMsg;       //midi special function: Running Status
                                    bTempLen = 1;
                           }
                           else
                                    break;
                           if( (bMsgID&0xf0)==MSG_PROGCHANGE || (bMsgID&0xf0)==MSG_CHNPRESSURE )
                                    bMsgLen= 2-bTempLen;
                           else
                                    bMsgLen = 3-bTempLen;
                           bPreMsg = bMsgID;
                           bTempLen = 0;
                           pbCurdata += bMsgLen+dVarLen;
                  }                          

         }	
	file->mid_DataStartPoint = (ME_U32)pbCurdata - (ME_U32)file->mid_Data;
	g_MidiInfo->LeftLen=(ME_U32)(file->mid_DataLength+file->mid_DataStartPointBackup - file->mid_DataStartPoint);
	return dCurTime/1000;
}
/*
static void AddMsgQue(SEEK_INFO *g_MidiInfo)
{
	ME_U08 bChn;
	VM_UNI_MSG *pUniMsg;

	for(bChn=0; bChn<MAX_MIDI_CHN; bChn++)
	{
		if(g_MidiInfo->ChnInfo[bChn].bTimbre != 0)
		{
			pUniMsg = &g_MidiInfo->MsgQue[g_MidiInfo->bMsgQueNum];
			
			pUniMsg->bChn = bChn;
			pUniMsg->sdDeltaTime = 0;
			pUniMsg->bMsgID = VMD_MSG_PROGRAM_CHANGE;
			pUniMsg->dPara1 = (ME_U32)g_MidiInfo->ChnInfo[bChn].bTimbre ;
			
			g_MidiInfo->bMsgQueNum++;
		}

		if(g_MidiInfo->ChnInfo[bChn].bVolume != 0x7f)
		{
			pUniMsg = &g_MidiInfo->MsgQue[g_MidiInfo->bMsgQueNum];
			
			pUniMsg->bChn = bChn;
			pUniMsg->sdDeltaTime = 0;
			pUniMsg->bMsgID = VMD_MSG_CHANNEL_VOLUME;
			pUniMsg->dPara1 = (ME_U32)g_MidiInfo->ChnInfo[bChn].bVolume ;
			
			g_MidiInfo->bMsgQueNum++;
		}

		if(g_MidiInfo->ChnInfo[bChn].bExpression != 0x7f)
		{
			pUniMsg = &g_MidiInfo->MsgQue[g_MidiInfo->bMsgQueNum];
			
			pUniMsg->bChn = bChn;
			pUniMsg->sdDeltaTime = 0;
			pUniMsg->bMsgID = VMD_MSG_EXPRESSION;
			pUniMsg->dPara1 = (ME_U32)g_MidiInfo->ChnInfo[bChn].bExpression ;
			
			g_MidiInfo->bMsgQueNum++;
		}

		if(g_MidiInfo->ChnInfo[bChn].bModulation != 0)
		{
			pUniMsg = &g_MidiInfo->MsgQue[g_MidiInfo->bMsgQueNum];
			
			pUniMsg->bChn = bChn;
			pUniMsg->sdDeltaTime = 0;
			pUniMsg->bMsgID = VMD_MSG_MODULATION_DEPTH;
			pUniMsg->dPara1 = (ME_U32)g_MidiInfo->ChnInfo[bChn].bModulation ;
			
			g_MidiInfo->bMsgQueNum++;
		}

		if(g_MidiInfo->ChnInfo[bChn].bPitchBendH != 0x40 ||g_MidiInfo->ChnInfo[bChn].bPitchBendL != 0 )
		{
			pUniMsg = &g_MidiInfo->MsgQue[g_MidiInfo->bMsgQueNum];
			
			pUniMsg->bChn = bChn;
			pUniMsg->sdDeltaTime = 0;
			pUniMsg->bMsgID = VMD_MSG_PITCH_BEND;
			pUniMsg->dPara1	= ((ME_U32)(g_MidiInfo->ChnInfo[bChn].bPitchBendH )<<7) +
				(ME_U32)(g_MidiInfo->ChnInfo[bChn].bPitchBendL);
			
			g_MidiInfo->bMsgQueNum++;
		}

		if(g_MidiInfo->ChnInfo[bChn].bBendRage!= 2)
		{
			pUniMsg = &g_MidiInfo->MsgQue[g_MidiInfo->bMsgQueNum];
			
			pUniMsg->bChn = bChn;
			pUniMsg->sdDeltaTime = 0;
			pUniMsg->bMsgID = VMD_MSG_BEND_RANGE;
			pUniMsg->dPara1 = (ME_U32)g_MidiInfo->ChnInfo[bChn].bBendRage ;
			
			g_MidiInfo->bMsgQueNum++;
		}

		if(g_MidiInfo->ChnInfo[bChn].bPan!= 0x40)
		{
			pUniMsg = &g_MidiInfo->MsgQue[g_MidiInfo->bMsgQueNum];
			
			pUniMsg->bChn = bChn;
			pUniMsg->sdDeltaTime = 0;
			pUniMsg->bMsgID = VMD_MSG_PANPOT;
			pUniMsg->dPara1 = (ME_U32)g_MidiInfo->ChnInfo[bChn].bPan ;
			
			g_MidiInfo->bMsgQueNum++;
		}

		if(g_MidiInfo->ChnInfo[bChn].bHold1!= 0)
		{
			pUniMsg = &g_MidiInfo->MsgQue[g_MidiInfo->bMsgQueNum];
			
			pUniMsg->bChn = bChn;
			pUniMsg->sdDeltaTime = 0;
			pUniMsg->bMsgID = VMD_MSG_HOLD1;
			pUniMsg->dPara1 = (ME_U32)g_MidiInfo->ChnInfo[bChn].bHold1 ;
			
			g_MidiInfo->bMsgQueNum++;
		}

		if(bChn==9)
		{
			if(g_MidiInfo->ChnInfo[bChn].bBankID!= 0x80)
			{
				pUniMsg = &g_MidiInfo->MsgQue[g_MidiInfo->bMsgQueNum];
				
				pUniMsg->bChn = bChn;
				pUniMsg->sdDeltaTime = 0;
				pUniMsg->bMsgID = VMD_MSG_BANK_SELECT;
				pUniMsg->dPara1 = (ME_U32)g_MidiInfo->ChnInfo[bChn].bBankID ;
				
				g_MidiInfo->bMsgQueNum++;
			}
		}
		else
		{
			if(g_MidiInfo->ChnInfo[bChn].bBankID!= 0)
			{
				pUniMsg = &g_MidiInfo->MsgQue[g_MidiInfo->bMsgQueNum];
				
				pUniMsg->bChn = bChn;
				pUniMsg->sdDeltaTime = 0;
				pUniMsg->bMsgID = VMD_MSG_BANK_SELECT;
				pUniMsg->dPara1 = (ME_U32)g_MidiInfo->ChnInfo[bChn].bBankID ;
				
				g_MidiInfo->bMsgQueNum++;
			}
		}
	}
}
*/

static changemidiformat1(VM_MIDI_PARSER_INFO *file)
{
	VM_MIDI_INPUT *vm_input=&(file->vm_input);
	ME_PU08 readbuff, writebuff, g_readbuff, g_writebuff, Track_status, pbLastMsgID, pbCurdata;
	ME_U32 *offset, *ticks;
	ME_U32 i;
	ME_U32 Track_datasize, dMeltaLen, dWriteLen, dReadLen_onetrack;
	ME_U32 minticks, minticks_tracknum, preticks, tempticks, dDeltatime, dVarLen, readBufSize;
	ME_U08 bMsgID, bMeltaID, EmptyTrackNum, bPreMsg, bMsgLen, bTempLen, bLastMinticksMsgID, bWriteSignal;
	ME_U32 bPC_Channel, bPC_preset;
	ME_U08 bNO_note, bNO_Channel, bChannel[16];
	ME_U08 bTimbre,bBankID;


	//local malloc memroy 
	g_writebuff=NULL;
	g_readbuff=NULL;
	offset=NULL;
	ticks=NULL;
	Track_status=NULL;
	pbLastMsgID=NULL;	

	readBufSize=file->mid_DataLength;
	g_writebuff=writebuff = (ME_PU08)me_malloc(readBufSize*2);
	if (writebuff==NULL) 
	{
		goto error_exit;
	}
	g_readbuff=readbuff =file->mid_Data+14;
	
	offset = (ME_U32 *)me_malloc(file->mid_TrackNum*sizeof(ME_U32));
	if (offset==NULL) 
	{
		goto error_exit;
	}
	
	ticks = (ME_U32 *)me_malloc(file->mid_TrackNum*sizeof(ME_U32));
	if (ticks==NULL) 
	{
		goto error_exit;
	}
	
	Track_status = (ME_PU08)me_malloc(file->mid_TrackNum);
	if (Track_status==NULL) 
	{
		goto error_exit;
	}
	
	pbLastMsgID = (ME_PU08)me_malloc(file->mid_TrackNum);
	if (pbLastMsgID==NULL) 
	{
		goto error_exit;
	}
	
	me_memset((void*)ticks,0,file->mid_TrackNum*sizeof(ME_U32));
	me_memset((void*)Track_status,0,file->mid_TrackNum);
	
	
	//find each track head
	offset[0] = 8;
	for(i=1; i<file->mid_TrackNum; i++)
	{
		Track_datasize = ((ME_U32)readbuff[4] << 24) + ((ME_U32)readbuff[5] << 16) +
			((ME_U32)readbuff[6] << 8) + (ME_U32)readbuff[7];
		offset[i] = 8+Track_datasize + offset[i-1]; 
		readbuff += 8 + Track_datasize;	
	}
	///////////////////////////////////////////////////////////////
	

	//find the first timestamps in each track (mostly is 0)
	for(i=0; i<file->mid_TrackNum; i++)
	{
		readbuff = g_readbuff;
		readbuff += offset[i];
		ReadVarLen(readbuff, &dDeltatime);
		ticks[i] = dDeltatime;
	}

	dWriteLen = 0;
	EmptyTrackNum = 0;
	preticks = 0;
	bLastMinticksMsgID = 0;
	while (EmptyTrackNum < file->mid_TrackNum) 
	{
		
		//find the least ticks
		bWriteSignal = 0;
		minticks = 0xffffffff;
		for(i=0; i<file->mid_TrackNum; i++) 
		{
			if (Track_status[i]==1) 
				continue;
			if (minticks > ticks[i])
			{
				minticks = ticks[i];
				minticks_tracknum = i;
			}			
		}
        if (minticks == preticks) 
		{
			minticks = 0;
			tempticks = 0;
		}
		else
		{
			tempticks = preticks;
			preticks = minticks;				
		}

		readbuff = g_readbuff;
		readbuff += offset[minticks_tracknum];
		
     	//find the same timestamp events in one track and write to the buffer
		pbCurdata = readbuff;
		dReadLen_onetrack = 0;
		bPreMsg = 0;
		while (1) 
		{
			dVarLen = ReadVarLen(pbCurdata, &dDeltatime); //dVarLen: the num of bytes which describe the dDeltatime
			
			if (dReadLen_onetrack!=0 && dDeltatime!=0) 
			{
				ticks[minticks_tracknum] += dDeltatime;
			//	bCurMsgID = bPreMsg;  //remember the last message id in the same track
				break;
			}
			pbCurdata += dVarLen;
			dReadLen_onetrack += dVarLen;
			bMsgID = *pbCurdata;
			if (bMsgID == MELTA_EVENT)    
			{
				bMeltaID = *(pbCurdata+1);
				if (bMeltaID == MELTA_END) //the end of events in one track
				{
					Track_status[minticks_tracknum] = 1;
					EmptyTrackNum++;
					pbCurdata -= dVarLen;        //1: MeltaEnd_deltatime
					dReadLen_onetrack -= dVarLen;
					break;
				}
				else if ( bMeltaID == MELTA_TEMPO && *(pbCurdata+2)==0x03 )  //tempo
				{
					bWriteSignal = 1;
					if (dDeltatime!=0) 
					{
						dVarLen = WriteVarLen(minticks-tempticks, writebuff);
						dWriteLen += dVarLen;
						writebuff += dVarLen;
					}
					else
					{
						writebuff[0]=0;
						dWriteLen++;
						writebuff++;
					}
					me_memcpy((char*)writebuff, (char*)pbCurdata, 6); //6: size of MELTA_TEMPO
					dWriteLen += 6;
					writebuff += 6;
					pbCurdata += 6;
					dReadLen_onetrack += 6;
				}
				else
				{
					pbCurdata += 2;
					dReadLen_onetrack += 2;
					dVarLen = ReadVarLen(pbCurdata, &dMeltaLen);
					pbCurdata += dVarLen+dMeltaLen;
					dReadLen_onetrack += dVarLen+dMeltaLen;				
				}				
			}
			else if (bMsgID == SYSEXCLUDE_EVENT) 
			{
				while(*pbCurdata != 0xf7)	    //0xf7 the end signal of sysexclude_event
				{
					pbCurdata++;
					dReadLen_onetrack++;
				}
				pbCurdata++;
				dReadLen_onetrack++;
			}
			else
			{
				bWriteSignal = 1;
				if (dDeltatime!=0) 
				{
					dVarLen = WriteVarLen(minticks-tempticks, writebuff);
					dWriteLen += dVarLen;
					writebuff += dVarLen;
				}
				else
				{
					writebuff[0]=0;
					dWriteLen++;
					writebuff++;
				}
				bTempLen = 0;
				if (bMsgID<0x80 || bMsgID>0xEF) 
				{
					if (bPreMsg == 0)
					{
						bMsgID = pbLastMsgID[minticks_tracknum]; //last message id in the same track
						if (bMsgID != bLastMinticksMsgID)      //current message and last message are not in same track
						{
							*writebuff = bMsgID;
							writebuff++;
							dWriteLen++;
						}	
					}
					else
						bMsgID = bPreMsg;       //midi special function: Running Status
					bTempLen = 1;
				}
				bPreMsg = bMsgID;
				if( (bMsgID&0xf0)==MSG_PROGCHANGE || (bMsgID&0xf0)==MSG_CHNPRESSURE )
					bMsgLen= 2-bTempLen;
				else
					bMsgLen = 3-bTempLen;
				if ( (bMsgID&0xf0)==MSG_PROGCHANGE ) 
				{
					bPC_Channel = bMsgID&0x0f;
					bPC_preset = *(pbCurdata+bMsgLen-1);
		
					bChannel[bPC_Channel] = bPC_preset;					
				}
				else if ( (bMsgID&0xf0)==MSG_NOTEON ) 
				{
					bNO_Channel = bMsgID&0x0f;
					bNO_note = *(pbCurdata+bMsgLen-2);			
					bTimbre=bChannel[bNO_Channel];
					
					if (bNO_Channel == 9) 
					{
						bBankID=128;
					}
					else
					{
						bBankID=0;
					}
					parser_mid_get_bandk_header(bBankID,bTimbre,bNO_note,vm_input);
				}
				me_memcpy((char*)writebuff, (char*)pbCurdata, bMsgLen);
				dWriteLen += bMsgLen;
				writebuff += bMsgLen;				
				pbCurdata += bMsgLen;
				dReadLen_onetrack += bMsgLen;
			}				
		}
		
		if (bWriteSignal == 0)
		{
			preticks = tempticks;
		}
		offset[minticks_tracknum] += dReadLen_onetrack;
		bLastMinticksMsgID = bPreMsg;                // remember the last message id write in the buffer
		pbLastMsgID[minticks_tracknum] = bPreMsg;  // remember the last message id in the same track
		if (dReadLen_onetrack != pbCurdata-readbuff) 
		{
			return 0;
		}		
	}
	
	// end of track
	writebuff[0]=0;
	writebuff[1]=255;
	writebuff[2]=47;
	writebuff[3]=0;
	dWriteLen+=4;
	
	//me_free(g_readbuff);
	me_free(offset);
	me_free(ticks);
	me_free(Track_status);
	me_free(pbLastMsgID);
	
	me_free(file->mid_Data);
	file->mid_Data = (ME_PU08)me_malloc(dWriteLen);
	if (file->mid_Data==NULL) 
	{
		//return ME_FAILE;
		//goto error_exit;
		me_free(g_writebuff);
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return ME_ERR_MALLOC_MEMORY_ERROR;		
	}
	//file->g_mid_Data = file->mid_Data;
	me_memcpy((char*)file->mid_Data, (char*)g_writebuff, dWriteLen);

	file->mid_DataLength = dWriteLen;
	
	me_free(g_writebuff);	
	return ME_SUCCESS;
	
error_exit:
	if(g_writebuff) me_free(g_writebuff);
	if(offset) me_free(offset);
	if(ticks) me_free(ticks);
	if(Track_status) me_free(Track_status);
	if(pbLastMsgID) me_free(pbLastMsgID);

	ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
	return ME_ERR_MALLOC_MEMORY_ERROR;
}

#if 0
static changemidiformat(VM_MIDI_PARSER_INFO *file)
{
	VM_MIDI_INPUT *vm_input=&(file->vm_input);
	ME_PU08 readbuff, writebuff, g_readbuff, g_writebuff, Track_status, pbLastMsgID, firstTrackBuff, g_firstTrackBuff;
	ME_U32 minticks, minticks_tracknum, preticks, tempticks, dDeltatime, dVarLen;
	ME_PU08 pbCurdata;
	ME_U32 Track_datasize, dMeltaLen, dWriteLen, dWriteLen_onetrack, readBufSize;
	ME_U32 *offset, *ticks;
	ME_U08 bMsgID, bMeltaID, EmptyTrackNum, bPreMsg, bMsgLen, bTempLen, bLastMinticksMsgID, bCurMsgID;
	ME_U32 bPC_Channel, bPC_preset,dtempLen;
	ME_U08 bNO_note, bNO_Channel, bChannel[16];
//	ME_U32 dTotalTime, dLastTempoTicks, dCurTempoTicks, dTempoCode;
	ME_U08 bTimbre,bBankID;
	//ME_U32 instru_num = file->mid_instrument->instru_num;
	ME_U32 index,i;

	//local malloc memroy 
	g_writebuff=NULL;
	g_readbuff=NULL;
	offset=NULL;
	ticks=NULL;
	Track_status=NULL;
	pbLastMsgID=NULL;
	g_firstTrackBuff=NULL;

	//file->me_seek(file->stream, 0, SEEK_END);
	//readBufSize = ftell(file->stream)-14;
	readBufSize=file->mid_DataLength;
	g_writebuff=writebuff = (ME_PU08)me_malloc(readBufSize*2);
	if (writebuff==NULL) 
	{
		goto error_exit;
		//ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		//return ME_ERR_MALLOC_MEMORY_ERROR;
	}
	
	//g_readbuff=readbuff = (ME_PU08)me_malloc(readBufSize);
	//if (readbuff==NULL) 
	//{
	//	goto error_exit;
		//ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		//return ME_ERR_MALLOC_MEMORY_ERROR;
	//}
	//file->me_seek(file->stream, 14, SEEK_SET);
	//file->me_read(file->stream, readbuff,readBufSize);
	g_readbuff=readbuff =file->mid_Data+14;
	
	offset = (ME_U32 *)me_malloc(file->mid_TrackNum*sizeof(ME_U32));
	if (offset==NULL) 
	{
		goto error_exit;
	}
	
	ticks = (ME_U32 *)me_malloc(file->mid_TrackNum*sizeof(ME_U32));
	if (ticks==NULL) 
	{
		goto error_exit;
	}
	
	Track_status = (ME_PU08)me_malloc(file->mid_TrackNum);
	if (Track_status==NULL) 
	{
		goto error_exit;
	}
	
	pbLastMsgID = (ME_PU08)me_malloc(file->mid_TrackNum);
	if (pbLastMsgID==NULL) 
	{
		goto error_exit;
	}

	me_memset((void*)ticks,0,file->mid_TrackNum*sizeof(ME_U32));
	me_memset((void*)Track_status,0,file->mid_TrackNum);


	//find each track head
	offset[0] = 8;
	for(index=1; index<file->mid_TrackNum; index++)
	{
		Track_datasize = ((ME_U32)readbuff[4] << 24) + ((ME_U32)readbuff[5] << 16) +
			((ME_U32)readbuff[6] << 8) + (ME_U32)readbuff[7];
		offset[index] = 8+Track_datasize + offset[index-1]; 
		readbuff += 8 + Track_datasize;	
	}
	///////////////////////////////////////////////////////////////
	




	// change Track 0 :  only preserve MELTA_TEMPO and MELTA_END ;	
	readbuff = g_readbuff;
	Track_datasize = ((ME_U32)readbuff[4] << 24) + ((ME_U32)readbuff[5] << 16) +
		((ME_U32)readbuff[6] << 8) + (ME_U32)readbuff[7];
	g_firstTrackBuff=firstTrackBuff = (ME_PU08)me_malloc(Track_datasize);
	if (firstTrackBuff==NULL) 
	{
		goto error_exit;
	}
	dtempLen=0;	
	readbuff += offset[0];
	pbCurdata = readbuff;
	preticks = 0;
	tempticks = 0;
	while (1)
	{
		dVarLen = ReadVarLen(pbCurdata, &dDeltatime);
		readbuff = pbCurdata;
		dDeltatime += preticks;
		preticks = dDeltatime;
		/*for(i=0; i<=dVarLen; i++)
		{
			pbCurdata++;
		}*/
		pbCurdata+=dVarLen;
		bMsgID = *pbCurdata;
		if (bMsgID == MELTA_EVENT)    
		{
			pbCurdata++;
			bMeltaID = *pbCurdata;
			if (bMeltaID == MELTA_END) //the end of events in one track
			{
				//dtempLen = dVarLen+1+3;   //3: len of MELTA_END
				readbuff += dVarLen;
				dVarLen = WriteVarLen(dDeltatime-tempticks, firstTrackBuff);
				firstTrackBuff += dVarLen;
				dtempLen = 3;
				me_memcpy((char*)firstTrackBuff, (char*)readbuff, dtempLen);
				firstTrackBuff+=dtempLen;
				break;
			}
			else if (bMeltaID == MELTA_TEMPO)  //tempo
			{
				//dtempLen = dVarLen+1+6;  //6: len of MELTA_TEMPO
				readbuff += dVarLen;
				dVarLen = WriteVarLen(dDeltatime-tempticks, firstTrackBuff);
				firstTrackBuff += dVarLen;
				dtempLen = 6;
				me_memcpy((char*)firstTrackBuff, (char*)readbuff, dtempLen);
				firstTrackBuff += dtempLen;
				pbCurdata += 5;
				tempticks = dDeltatime;
			}
			else 
			{
				pbCurdata++;
				dVarLen = ReadVarLen(pbCurdata, &dMeltaLen);
				/*for(i=0; i<=dVarLen; i++)
				{
					pbCurdata++;
				}
				for(i=0; i<dMeltaLen; i++)
				{	
					pbCurdata++;
				}*/
				pbCurdata+=(dVarLen+dMeltaLen);
			}
		}
		else if (bMsgID == SYSEXCLUDE_EVENT) 
		{
			while(*pbCurdata != 0xf7)	    //0xf7 the end signal of sysexclude_event
			{
				pbCurdata++;
			}
			pbCurdata++;
		}
	}
	dWriteLen = firstTrackBuff-g_firstTrackBuff;
	dtempLen = Track_datasize - dWriteLen;
	offset[0] += dtempLen;
	readbuff = g_readbuff+offset[0];
	me_memcpy((char*)readbuff, (char*)g_firstTrackBuff, dWriteLen);
    ////////////////////////////////////////////////////////////////////////////////////
	
	//find the first timestamps in each track (mostly is 0)
	for(index=0; index<file->mid_TrackNum; index++)
	{
		readbuff = g_readbuff;
		readbuff += offset[index];
		ReadVarLen(readbuff, &dDeltatime);
		ticks[index] = dDeltatime;
	}
	///////////////////////////////////////////////////////////////////////////////




//	dTotalTime = 0;
//	dLastTempoTicks = 0;
//	dCurTempoTicks = 0;
//	dTempoCode = 500000;
	dWriteLen = 0;
	EmptyTrackNum = 0;
	preticks = 0;
	bTempLen = 0;
	bLastMinticksMsgID = 0;
	while (EmptyTrackNum < file->mid_TrackNum) 
	{
		
		//find the least ticks
		minticks = 0xffffffff;
		for(i=0; i<file->mid_TrackNum; i++) 
		{
			if (Track_status[i]==1) 
				continue;
			if (minticks > ticks[i])
			{
				minticks = ticks[i];
				minticks_tracknum = i;
		//		printf("ticks = %d\n",minticks);
			}			
		}

        if (minticks == preticks) 
		{
			minticks = 0;
			tempticks = 0;
		}
		else
		{
			tempticks = preticks;
			preticks = minticks;				
		}
		readbuff = g_readbuff;
		readbuff += offset[minticks_tracknum];
		/////////////////////////////////////////////////
		
		//find the same timestamp events in one track and write to the buffer
		pbCurdata = readbuff;
		dWriteLen_onetrack = 0;
		bPreMsg = 0;
		while (1) 
		{
			dVarLen = ReadVarLen(pbCurdata, &dDeltatime); //dVarLen: the num of bytes which describe the dDeltatime
			if (dWriteLen_onetrack!=0 && dDeltatime!=0) 
			{
				ticks[minticks_tracknum] += dDeltatime;
				if ( *(pbCurdata+dVarLen+1)<0x80 || *(pbCurdata+dVarLen+1)>0xEF )  //Running Status
				{
     				bCurMsgID = bPreMsg;   //remember the last message id in the same track
				}
				break;
			}
			/*for(i=0; i<=dVarLen; i++)
			{
				pbCurdata++;
				dWriteLen_onetrack++;
			}*/
			pbCurdata+=dVarLen;
			dWriteLen_onetrack+=dVarLen;
			
			bMsgID = *pbCurdata;
			
			if (bMsgID == MELTA_EVENT)    
			{
				pbCurdata++;
				dWriteLen_onetrack++;
				bMeltaID = *pbCurdata;
				if (bMeltaID == MELTA_END) //the end of events in one track
				{
					Track_status[minticks_tracknum] = 1;
					EmptyTrackNum++;
					pbCurdata -= 2;        //2: bMsgID_len + MeltaEnd_deltatime
					dWriteLen_onetrack -=2;
					break;
				}
//				else if (bMeltaID == MELTA_TEMPO)  //tempo
//				{
//					if ( *(pbCurdata+1)==0x03 ) 
//					{
//						dCurTempoTicks = ticks[minticks_tracknum];
//						dTotalTime += (dCurTempoTicks-dLastTempoTicks)*dTempoCode/file->mid_Division; 
//						dLastTempoTicks = dCurTempoTicks;
//						dTempoCode = 0;
//						for(i=0; i<3; i++)
//						{
//							dTempoCode = dTempoCode*0x100 + *(pbCurdata+2+i);
//						}						
//					}										
//				}
				pbCurdata++;
				dWriteLen_onetrack++;
				dVarLen = ReadVarLen(pbCurdata, &dMeltaLen);
				/*for(i=0; i<=dVarLen; i++)
				{
					pbCurdata++;
					dWriteLen_onetrack++;
				}
				for(i=0; i<dMeltaLen; i++)
				{	
					pbCurdata++;
					dWriteLen_onetrack++;
				}*/
				pbCurdata+=(dVarLen+dMeltaLen);
				dWriteLen_onetrack+=(dVarLen+dMeltaLen);
			}
			else if (bMsgID == SYSEXCLUDE_EVENT) 
			{
				while(*pbCurdata != 0xf7)	    //0xf7 the end signal of sysexclude_event
				{
					pbCurdata++;
					dWriteLen_onetrack++;
				}
				pbCurdata++;
				dWriteLen_onetrack++;
			}
			else
			{
				bTempLen = 0;

				if (bMsgID<0x80 || bMsgID>0xEF) 
				{
					if (bPreMsg == 0)     //first message which do not have message id
					{
						bMsgID = pbLastMsgID[minticks_tracknum]; //last message id in the same track
					}
					else
						bMsgID = bPreMsg;       //midi special function: Running Status
					bTempLen = 1;
				}
				if( (bMsgID&0xf0)==MSG_PROGCHANGE || (bMsgID&0xf0)==MSG_CHNPRESSURE )
					bMsgLen= 2-bTempLen;
				else
					bMsgLen = 3-bTempLen;

				bPreMsg = bMsgID;
				//bTempLen = 0;
				if ( (bMsgID&0xf0)==MSG_PROGCHANGE ) 
				{
					bPC_Channel = bMsgID&0x0f;
					bPC_preset = *(pbCurdata+bMsgLen-1);
					
					/*if (bPC_Channel != 9)    
					{
						index = 0;
						while (file->mid_instrument->instrumentdata_info[index].preset != EMPTY_SIGNAL) 
						{
							if (file->mid_instrument->instrumentdata_info[index].preset == bPC_preset )
							{							
								break;
							}
							index++;
						}
						bChannel[bPC_Channel] = index;
						if (file->mid_instrument->instrumentdata_info[index].preset == EMPTY_SIGNAL) 
						{							
							file->mid_instrument->instrumentdata_info[index].preset = bPC_preset;
							instru_num++;
						}
					}*/			
					bChannel[bPC_Channel] = bPC_preset;					
				}
				else if ( (bMsgID&0xf0)==MSG_NOTEON ) 
				{
					bNO_Channel = bMsgID&0x0f;
					bNO_note = *(pbCurdata+bMsgLen-2);			
					bTimbre=bChannel[bNO_Channel];
					
					if (bNO_Channel == 9) 
					{
						//file->mid_instrument->drumdata_info[bNO_note] = 1;
						bBankID=128;
					}
					else
					{
						//bNO_index = bChannel[bNO_Channel];					
						//file->mid_instrument->instrumentdata_info[bNO_index].note[bNO_note]=1;
						bBankID=0;
					}
					parser_mid_get_bandk_header(bBankID,bTimbre,bNO_note,vm_input);
				}
				pbCurdata += bMsgLen;
				dWriteLen_onetrack += bMsgLen;
			}				
		}

		offset[minticks_tracknum] += dWriteLen_onetrack;
		dtempLen = WriteVarLen(minticks-tempticks, writebuff);
		dWriteLen += dtempLen;
		writebuff += dtempLen;
		dtempLen = ReadVarLen(readbuff, &dDeltatime);
		//if ( (*(readbuff+dtempLen+1)<0x80 || *(readbuff+dtempLen+1)>0xEF) && *(readbuff+dtempLen+1)!=0xff
		//	&& *(readbuff+dtempLen+1)!=0xf0 )  //midi special function: Running Status
		bMsgID=*(readbuff+dtempLen);
		if(((bMsgID<0x80)||(bMsgID>0xEF)) && (bMsgID!=0xFF) && (bMsgID!=0xF0))
		{
			bMsgID = pbLastMsgID[minticks_tracknum];
			if (bMsgID != bLastMinticksMsgID)    //current message and last message are not in same track
			{
				*writebuff = bMsgID;
				writebuff++;
				dWriteLen++;
			}
		}
		bLastMinticksMsgID = bPreMsg;                // remember the last message id write in the buffer
		pbLastMsgID[minticks_tracknum] = bCurMsgID;  // remember the last message id in the same track
		/*dWriteLen += dWriteLen_onetrack - dtempLen - 1; 
		me_memcpy(writebuff, readbuff+dtempLen+1, dWriteLen_onetrack - dtempLen-1);			
		writebuff += dWriteLen_onetrack - dtempLen-1;*/
		dWriteLen += dWriteLen_onetrack - dtempLen ; 
		me_memcpy((char*)writebuff, (char*)(readbuff+dtempLen), dWriteLen_onetrack - dtempLen);			
		writebuff += dWriteLen_onetrack - dtempLen;

		/*if (dWriteLen_onetrack != pbCurdata-readbuff) 
		{
			return 0;
		}*/
		//assert(dWriteLen_onetrack==pbCurdata-readbuff);
	}
	

	// end of track
	writebuff[0]=0;
	writebuff[1]=255;
	writebuff[2]=47;
	writebuff[3]=0;
	dWriteLen+=4;

	//me_free(g_readbuff);
	me_free(offset);
	me_free(ticks);
	me_free(Track_status);
	me_free(pbLastMsgID);
	me_free(g_firstTrackBuff);

	me_free(file->mid_Data);
	file->mid_Data = (ME_PU08)me_malloc(dWriteLen);
	if (file->mid_Data==NULL) 
	{
		//return ME_FAILE;
		//goto error_exit;
		me_free(g_writebuff);
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return ME_ERR_MALLOC_MEMORY_ERROR;		
	}
	//file->g_mid_Data = file->mid_Data;
	me_memcpy((char*)file->mid_Data, (char*)g_writebuff, dWriteLen);

//	dCurTempoTicks = ticks[minticks_tracknum];
//	dTotalTime += (dCurTempoTicks-dLastTempoTicks)*dTempoCode/file->mid_Division;

//	file->mid_TotalTime = dTotalTime/1000; //ms
	file->mid_DataLength = dWriteLen;
	//file->mid_instrument->instru_num = instru_num;

	me_free(g_writebuff);
	
	return ME_SUCCESS;

error_exit:
	if(g_writebuff) me_free(g_writebuff);
	//if(g_readbuff) me_free(g_readbuff);
	if(offset) me_free(offset);
	if(ticks) me_free(ticks);
	if(Track_status) me_free(Track_status);
	if(pbLastMsgID) me_free(pbLastMsgID);
	if(g_firstTrackBuff) me_free(g_firstTrackBuff);

	ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
	return ME_ERR_MALLOC_MEMORY_ERROR;
}
#endif
static ME_U32 gettrackmessage(VM_MIDI_PARSER_INFO *file)
{
	//midi_instrument_info_t *midi_info = file->mid_instrument;
	VM_MIDI_INPUT *vm_input=&(file->vm_input);

	ME_PU08 mid_file=file->mid_Data+file->mid_DataStartPoint;
	//ME_U32 datasize=file->mid_DataLength; 
	ME_PU08 pbCurData;
	ME_U32 dLen;
	ME_U32 dMeltaLen;
	ME_U08 bMsgID;
	//ME_U32 dTotalsize=0;
	ME_U08 bMsgLen;
	ME_U08 bMeltaNum;
	ME_U32 dDeltaTime;
	ME_U32 dPreDeltaTime=0;
	ME_U08 bPreMsg;
	ME_U32 dTempLen=0;

//	ME_U32 dCurTempoTicks, dTotalTime, dLastTempoTicks, dTempoCode;
	
	//ME_U32 index;       //the num of midi_info.
	//ME_U32 i; 
	//ME_U32 j=1;
	
//	unsigned char bPC_bank;
	ME_U08 bPC_preset;
	ME_U08 bPC_Channel;
	ME_U08 bChannel[16];

	ME_U08 bNO_note;
	ME_U08 bNO_Channel;
	//ME_U08 bNO_index;   //the num of midi_info
	ME_U08 bTimbre,bBankID;

	//ME_U32 instru_num = file->mid_instrument->instru_num;

//	memset(bChannel, 0, 16);


//	dCurTempoTicks = 0;
//	dLastTempoTicks = 0;
//	dTempoCode = 500000;
//	dTotalTime = 0;
	pbCurData = mid_file;
	//while (dTotalsize < datasize) 
	while(1)
	{
		dLen = ReadVarLen(pbCurData, &dDeltaTime);
		dDeltaTime += dPreDeltaTime;
		/*for(i=0; i<=dLen; i++)
		{
			pbCurData++;
			dTotalsize++;
		}*/
		pbCurData+=dLen;
		bMsgID = *pbCurData;
		dPreDeltaTime = dDeltaTime;
		if (bMsgID == MELTA_EVENT) 
		{
			pbCurData++;
			//dTotalsize++;
			bMeltaNum = *pbCurData;
			
//			if (bMeltaNum == MELTA_TEMPO)  //tempo
//			{
//				if ( *(pbCurData+1)==0x03 ) 
//				{
//					dCurTempoTicks = dDeltaTime;
//					dTotalTime += (dCurTempoTicks-dLastTempoTicks)*dTempoCode/file->mid_Division; 
//					dLastTempoTicks = dCurTempoTicks;
//					dTempoCode = 0;
//					for(i=0; i<3; i++)
//					{
//						dTempoCode = dTempoCode*0x100 + *(pbCurData+2+i);
//					}						
//				}										
//			}
			if (bMeltaNum == MELTA_END) //the end of events in one track
			{
				mid_file += 2;        //2: bMsgID_len + MeltaEnd_deltatime
				break;
			}
			
			pbCurData++;
			//dTotalsize++;
			dLen = ReadVarLen(pbCurData, &dMeltaLen);
			/*for(i=0; i<=dLen; i++)
			{
				pbCurData++;
				dTotalsize++;
			}
			for(i=0; i<dMeltaLen; i++)
			{	
				pbCurData++;
				dTotalsize++;
			}*/
			pbCurData+=(dLen+dMeltaLen);
		}
		else if (bMsgID == SYSEXCLUDE_EVENT) 
		{
			while(*pbCurData != 0xf7)	    //0xf7 the end signal of sysexclude_event
			{
				pbCurData++;
			    //dTotalsize++;
			}
			pbCurData++;
			//dTotalsize++;
		}
		else
		{
			dTempLen = 0;
			if (bMsgID<0x80 || bMsgID>0xEF) 
			{
				bMsgID = bPreMsg;       //midi special function: Running Status
				dTempLen = 1;
			}
			if( (bMsgID&0xf0)==MSG_PROGCHANGE || (bMsgID&0xf0)==MSG_CHNPRESSURE )
				bMsgLen= 2-dTempLen;
			else
				bMsgLen = 3-dTempLen;
			bPreMsg = bMsgID;
			//dTempLen = 0;
			if ( (bMsgID&0xf0)==MSG_PROGCHANGE ) 
			{
				bPC_Channel = bMsgID&0x0f;
				bPC_preset = *(pbCurData+bMsgLen-1);
				/*if (bPC_Channel != 9)    
				{
					index = 0;
					while (midi_info->instrumentdata_info[index].preset != EMPTY_SIGNAL) 
					{
						if (midi_info->instrumentdata_info[index].preset == bPC_preset )
						{							
							break;
						}
						index++;
					}
					bChannel[bPC_Channel] = index;
					if (midi_info->instrumentdata_info[index].preset == EMPTY_SIGNAL) 
					{
						//	midi_info->instrumentdata_info[index].bank = bPC_bank;
						midi_info->instrumentdata_info[index].preset = bPC_preset;
						instru_num++;
					}
				}*/
				
				bChannel[bPC_Channel] = bPC_preset;

			}
			else if ( (bMsgID&0xf0)==MSG_NOTEON ) 
			{
				bNO_Channel = bMsgID&0x0f;
				bNO_note = *(pbCurData+bMsgLen-2);
				bTimbre=bChannel[bNO_Channel];

				if (bNO_Channel == 9) 
				{
					//drum instrument
					//midi_info->drumdata_info[bNO_note] = 1;
					bBankID=128;					
				}
				else
				{
					//common instrument
					//bNO_index = bChannel[bNO_Channel];					
					//file->mid_instrument->instrumentdata_info[bNO_index].note[bNO_note]=1;
					bBankID=0;
				}
				parser_mid_get_bandk_header(bBankID,bTimbre,bNO_note,vm_input);
			}
			pbCurData += bMsgLen;
			//dTotalsize += bMsgLen;
		}
	}
//	dCurTempoTicks = dDeltaTime;
//	dTotalTime += (dCurTempoTicks-dLastTempoTicks)*dTempoCode/file->mid_Division;


	//file->mid_instrument->instru_num = instru_num;	
//	file->mid_TotalTime = dTotalTime/1000;

	return ME_SUCCESS;
}
#if 0
static ME_U32  get_mid_time(VM_MIDI_PARSER_INFO *file)
{
	ME_U32 bidx, i;
	ME_PU08 mid_file=file->mid_Data+file->mid_DataStartPoint;
//	ME_U32 dDataReadSize, dOneTrackDataSize; 
	ME_U32 dVarLen;
	ME_U32 dMeltaLen;
	ME_U08 bMsgID, bPreMsg;
	ME_U08 bMeltaID, bTempLen;
	ME_U32 dDeltatime;
	ME_U32 dPreDeltaTime;	
	ME_U32 dLastTempoTicks, dTempoCode;
	ME_U64 dTotalTime,dOneTrackTime;


	dTotalTime = 0;
	bTempLen = 0;
	for(bidx=0; bidx<file->mid_TrackNum; bidx++)
	{
//		dDataReadSize = 0;
//		dOneTrackDataSize = ((ME_U32)mid_file[4] << 24) + ((ME_U32)mid_file[5] << 16) +
//		                    ((ME_U32)mid_file[6] << 8) + (ME_U32)mid_file[7];
		dTempoCode = 500000;
		dLastTempoTicks = 0;
		dOneTrackTime = 0;
		dPreDeltaTime = 0;
		mid_file += 8;
		while (1)
		{
			dVarLen = ReadVarLen(mid_file, &dDeltatime); //dVarLen: the num of bytes which describe the dDeltatime
			dDeltatime += dPreDeltaTime;
			dPreDeltaTime = dDeltatime;
			//bMsgID = *mid_file;   
			/*for(i=0; i<=dVarLen; i++)
			{
				mid_file++;
			}*/
			mid_file+=dVarLen;
			bMsgID = *mid_file;			
			if (bMsgID == MELTA_EVENT)    
			{
				mid_file++;
				bMeltaID = *mid_file;
				if (bMeltaID == MELTA_END) //the end of events in one track
				{
					mid_file += 2;        //2: bMsgID_len + MeltaEnd_deltatime
					break;
				}
				else if (bMeltaID == MELTA_TEMPO)  //tempo
				{
					if ( *(mid_file+1)==0x03 ) 
					{
						//dOneTrackTime += (dDeltatime-dLastTempoTicks)*dTempoCode/file->mid_Division; 
						dOneTrackTime += (ME_U64)(dDeltatime-dLastTempoTicks)*dTempoCode;
						dLastTempoTicks = dDeltatime;
						dTempoCode = 0;
						for(i=0; i<3; i++)
						{
							dTempoCode = dTempoCode*0x100 + *(mid_file+2+i);
						}						
					}										
				}
				mid_file++;
				dVarLen = ReadVarLen(mid_file, &dMeltaLen);
				/*for(i=0; i<=dVarLen; i++)
				{
					mid_file++;
				}
				for(i=0; i<dMeltaLen; i++)
				{	
					mid_file++;
				}*/
				mid_file+=(dVarLen+dMeltaLen);
			}
			else if (bMsgID == SYSEXCLUDE_EVENT) 
			{
				while(*mid_file != 0xf7)	    //0xf7 the end signal of sysexclude_event
				{
					mid_file++;
				}
				mid_file++;
			}
			else
			{
				bTempLen = 0;
				if (bMsgID<0x80 || bMsgID>0xEF) 
				{
					bMsgID = bPreMsg;       //midi special function: Running Status
					bTempLen = 1;
				}
				if( (bMsgID&0xf0)==MSG_PROGCHANGE || (bMsgID&0xf0)==MSG_CHNPRESSURE )
					dVarLen= 2-bTempLen;
				else
					dVarLen = 3-bTempLen;

				bPreMsg = bMsgID;
				//bTempLen = 0;
				mid_file += dVarLen;
			}
		}
		//dOneTrackTime += (dDeltatime-dLastTempoTicks)*dTempoCode/file->mid_Division; 
		dOneTrackTime +=(ME_U64)(dDeltatime-dLastTempoTicks)*dTempoCode;
		if (dTotalTime < dOneTrackTime)
		{
			dTotalTime = dOneTrackTime;
		}
	}
	//file->mid_TotalTime = dTotalTime/1000;  //ms
	file->mid_TotalTime = (ME_S32)(dTotalTime/(1000*file->mid_Division));
	return ME_SUCCESS;
}
#else
static ME_U32  get_mid_time(VM_MIDI_PARSER_INFO *file)
{

         ME_U32 bidx, i;
         ME_PU08 mid_file=file->mid_Data+file->mid_DataStartPoint;
         ME_S08 bSignal;
//       ME_U32 dDataReadSize, dOneTrackDataSize; 

         ME_U32 dVarLen;
         ME_U32 dMeltaLen;
         ME_U08 bMsgID, bPreMsg;
         ME_U08 bMeltaID, bTempLen;
         ME_U32 dDeltatime;
         ME_U32 dPreDeltaTime; 
         ME_U32 dLastTempoTicks, dTempoCode, dLastTrackDeltatime;
         ME_U64 dTotalTime,dOneTrackTime; 

         bSignal=0;
         dTotalTime = 0;
         bTempLen = 0;
         dTempoCode = 500000;
         dLastTrackDeltatime=0; 

         for(bidx=0; bidx<file->mid_TrackNum; bidx++)
         {

//                dDataReadSize = 0;
//                dOneTrackDataSize = ((ME_U32)mid_file[4] << 24) + ((ME_U32)mid_file[5] << 16) +
//                                    ((ME_U32)mid_file[6] << 8) + (ME_U32)mid_file[7];
                  dLastTempoTicks = 0;
                  dOneTrackTime = 0;
                  dPreDeltaTime = 0;
                  mid_file += 8;
                  while (1)
                  {
                           dVarLen = ReadVarLen(mid_file, &dDeltatime); //dVarLen: the num of bytes which describe the dDeltatime
                           dDeltatime += dPreDeltaTime;
                           dPreDeltaTime = dDeltatime;
                           //bMsgID = *mid_file;   
                           /*for(i=0; i<=dVarLen; i++)
                           {
                                    mid_file++;
                           }*/
                           mid_file+=dVarLen;
                           bMsgID = *mid_file;                   

                           if (bMsgID == MELTA_EVENT) 
                           {
                                    mid_file++;
                                    bMeltaID = *mid_file;
                                    if (bMeltaID == MELTA_END) //the end of events in one track
                                    {
                                             mid_file += 2;        //2: bMsgID_len + MeltaEnd_deltatime
                                             break;
                                    }
                                    else if (bMeltaID == MELTA_TEMPO)  //tempo
                                    {
                                             if ( *(mid_file+1)==0x03 ) 
                                             {
                                                      //dOneTrackTime += (dDeltatime-dLastTempoTicks)*dTempoCode/file->mid_Division;
                                                      dOneTrackTime += (ME_U64)(dDeltatime-dLastTempoTicks)*dTempoCode;
                                                      dLastTempoTicks = dDeltatime;
                                                      dTempoCode = 0;
                                                      for(i=0; i<3; i++)
                                                      {
                                                               dTempoCode = dTempoCode*0x100 + *(mid_file+2+i);
                                                      }   
                                             }                                                                     

                                    }
                                    mid_file++;
                                    dVarLen = ReadVarLen(mid_file, &dMeltaLen);
                                    /*for(i=0; i<=dVarLen; i++)
                                    {
                                             mid_file++;
                                    }
                                    for(i=0; i<dMeltaLen; i++)
                                    {   
                                             mid_file++;
                                    }*/
                                    mid_file+=(dVarLen+dMeltaLen);
                           }
                           else if (bMsgID == SYSEXCLUDE_EVENT) 
                           {
                                    while(*mid_file != 0xf7)         //0xf7 the end signal of sysexclude_event
                                    {
                                             mid_file++;
                                    }
                                    mid_file++;
                           }
                           else
                           {
                                    bTempLen = 0;
                                    if (bMsgID<0x80 || bMsgID>0xEF)
                                    {
                                             bMsgID = bPreMsg;       //midi special function: Running Status
                                             bTempLen = 1;
                                    }
                                    if( (bMsgID&0xf0)==MSG_PROGCHANGE || (bMsgID&0xf0)==MSG_CHNPRESSURE )
                                             dVarLen= 2-bTempLen;
                                    else
                                             dVarLen = 3-bTempLen; 

                                    bPreMsg = bMsgID;
                                    //bTempLen = 0;
                                    mid_file += dVarLen;
                           }
                  }
                  if (bSignal==0)
                  {
                           dOneTrackTime +=(ME_U64)(dDeltatime-dLastTempoTicks)*dTempoCode;
                      dTotalTime += dOneTrackTime;
                           dLastTrackDeltatime=dDeltatime;
                           bSignal=1;
                  }
                  if (dLastTrackDeltatime < dDeltatime)
                  {
                           dTotalTime +=(ME_U64)(dDeltatime-dLastTrackDeltatime)*dTempoCode;
                           dLastTrackDeltatime = dDeltatime;
                  }
         }
         //file->mid_TotalTime = dTotalTime/1000;  //ms
         file->mid_TotalTime = (ME_S32)(dTotalTime/(1000*file->mid_Division));
         return ME_SUCCESS;
}
#endif
static ME_U32  get_mid_sound_bank(VM_MIDI_PARSER_INFO *file)
{
	ME_U32 retval;

	file->vm_input_core.total_len = 0;
	file->vm_input_core.total_len += 5 * sizeof(ME_U32);	//	First 5 elements
	file->vm_input_core.pBankheader=(void*)MMD_SYS_Malloc(sizeof(Download_Zone)*MAX_ZONE_LEN + 256);
	
	file->vm_input_core.total_len += sizeof(ME_U32); // pBankheader
	file->vm_input_core.total_len += sizeof(ME_U32);	//	ZoneLen

	
	if(file->vm_input_core.pBankheader==NULL)
	{
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return ME_ERR_MALLOC_MEMORY_ERROR;
	}
	file->vm_input_core.ZoneLen=0;	

	
	file->vm_input.total_len = 0;
	file->vm_input.total_len += 5 * sizeof(ME_U32);	//	First 5 elements
	file->vm_input.pBankheader=me_malloc(sizeof(Download_Zone)*MAX_ZONE_LEN);
	
	file->vm_input.total_len += (sizeof(Download_Zone)*MAX_ZONE_LEN); // pBankheader
	file->vm_input.total_len += sizeof(ME_U32);	//	ZoneLen

	
	if(file->vm_input.pBankheader==NULL)
	{
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return ME_ERR_MALLOC_MEMORY_ERROR;
	}
	file->vm_input.ZoneLen=0;

	
	if (file->mid_Format == 0) 
	{
		me_printf("mid_Format=0\n");
		file->mid_DataStartPoint = 22; //offset of first track data
		retval=gettrackmessage(file);
		if(retval!=ME_SUCCESS)	{
			return ME_FAILE;	
		}
	}
	else
	{
		me_printf("mid_Format=1\n");
		//retval=changemidiformat(file);
       	 retval=changemidiformat1(file);
		if(retval!=ME_SUCCESS)	{
			return ME_FAILE;
		}
		file->mid_DataStartPoint = 0;
	}
	
//: Copy vm_input.pBankheader into vm_input_core.pBankheader
	file->vm_input_core.ZoneLen = file->vm_input.ZoneLen;
	//Vm_WrSram((unsigned int)(file->vm_input_core.pBankheader), (char*)file->vm_input.pBankheader, (sizeof(Download_Zone)*MAX_ZONE_LEN));

	//Get sound bank data
	retval=parser_mid_get_bank_data(file);
	if(retval!=ME_SUCCESS)  return ME_FAILE;
	file->vm_input.Division=file->mid_Division;
	file->vm_input.TrackDataLen=file->mid_DataLength;
	file->mid_SampleRate=file->vm_input.SampleRate;

	file->vm_input_core.Division=file->mid_Division;
	file->vm_input_core.TrackDataLen=file->mid_DataLength;


	//allocate seek infor buffer
	file->vm_input_core.SeekInfo = (void*)MMD_SYS_Malloc(sizeof(SEEK_INFO) + 256);
	if(file->vm_input_core.SeekInfo==NULL)
	{
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return ME_ERR_MALLOC_MEMORY_ERROR;		
	}
	file->vm_input_core.total_len += sizeof(ME_U32); // SEEK_INFO 
	
	//allocate seek infor buffer
	file->vm_input.SeekInfo=me_malloc(sizeof(SEEK_INFO));
	if(file->vm_input.SeekInfo==NULL)
	{
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return ME_ERR_MALLOC_MEMORY_ERROR;		
	}
	file->vm_input.total_len += sizeof(SEEK_INFO); // SEEK_INFO 
	me_memset((ME_PU08) file->vm_input.SeekInfo, 0x0, sizeof(SEEK_INFO));
	file->mid_DataStartPointBackup=file->mid_DataStartPoint;
		
	return ME_SUCCESS;
}

#ifndef _PC_SIM
ME_U32	parser_mid_init(void *mediafile, fs_callbacks *fs_filefunc,void **fileinfor)
#else
ME_U32	parser_mid_init(void *mediafile, fs_callbacks *fs_filefunc,void **fileinfor)
#endif
{
	VM_MIDI_PARSER_INFO *file;
	ME_PU08 readBuf;
	ME_S32 DataLen;
	ME_U32 Format;
	ME_U32 Tracknum;
	ME_U32 Division;
	ME_U32 retval;
	
    #if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_mid_init()"));
    #endif	
	mid_frame_count = 0;
	last_frame_flag = 0;
	me_printf("\nStart parser_mid_init......\n");
	*fileinfor=NULL;
	file = (VM_MIDI_PARSER_INFO *)me_malloc(sizeof(VM_MIDI_PARSER_INFO));
	if(file==NULL){		
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return ME_ERR_MALLOC_MEMORY_ERROR;
	}
	me_memset((ME_PU08)file,0,sizeof(VM_MIDI_PARSER_INFO));
	*fileinfor  = (void *)file;
	
	file->stream	= mediafile;
	
#ifndef _PC_SIM
	file->me_read	= fs_filefunc->read_func;
	file->me_seek	= fs_filefunc->seek_func;
	file->me_write	= fs_filefunc->write_func;
	file->me_tell		= fs_filefunc->tell_func;
#else
	file->me_read	= fs_filefunc->read_func;
	file->me_seek	= fs_filefunc->seek_func;
	file->me_write	= fs_filefunc->write_func;
	file->me_tell		= fs_filefunc->tell_func;
	
	(file->read_block_func) = (fs_filefunc->read_block_func);
	file->write_block_func = fs_filefunc->write_block_func;
	(file->get_offset_sector_func) = (fs_filefunc->get_sector_offset_func);	
#endif
	//load midi file
	file->me_seek(file->stream,0,SEEK_END);
	DataLen=file->me_tell(file->stream);
	file->me_seek(file->stream,0,SEEK_SET);
	file->mid_Data = me_malloc(DataLen);
	file->file_end = 0;
	if (file->mid_Data==NULL) 
	{
#ifndef _PC_SIM
		parser_mid_release(file,NULL,NULL);
#else
        	parser_mid_release(file);
#endif
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return ME_ERR_MALLOC_MEMORY_ERROR;
	}
	file->me_read(file->stream, (char*)file->mid_Data, DataLen);
	
    readBuf=file->mid_Data;
	Format = ((ME_U32)readBuf[8] << 8) + (ME_U32)readBuf[9];
	Tracknum = ((ME_U32)readBuf[10] << 8) + (ME_U32)readBuf[11];
	Division = ((ME_U32)readBuf[12] << 8) + (ME_U32)readBuf[13];
	file->mid_DataStartPoint = 14;//offset of first track 32bit ID "MTrk"
	file->mid_DataLength =	DataLen-14;
	
	file->mid_Channels = 2;
	file->mid_SampleRate = 48000;
	file->mid_BitsPerSample = 16;
	file->mid_Format = Format;
	file->mid_TrackNum = Tracknum;
	file->mid_Division = Division;

	if(Format==0)
	{
		DataLen = ((ME_U32)readBuf[18] << 24) + ((ME_U32)readBuf[19] << 16) +
		          ((ME_U32)readBuf[20] << 8) + (ME_U32)readBuf[21];
		file->mid_DataLength = DataLen;		
	}
	
	retval=get_mid_time(file);
	if(retval!=ME_SUCCESS)	{
#ifndef _PC_SIM
		parser_mid_release(file,NULL,NULL);
#else
        parser_mid_release(file);
#endif
		return ME_FAILE;
	}

	file->pvm_input = NULL;

	#if 0
	me_printf("\nStart get_mid_sound_bank......\n");
    	retval=get_mid_sound_bank(file); 
	
	file->pvm_input = me_malloc(file->vm_input_core.total_len);
	if(retval!=ME_SUCCESS)	{
#ifndef _PC_SIM
		parser_mid_release(file,NULL,NULL);
#else
		parser_mid_release(file);
#endif
		return ME_FAILE;
	}
	#endif

	return ME_SUCCESS;	
}

/*******************************************************************************************
Description:
	parser close and release
Parameters:
      void *fileinfor: file information structor create by parser
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Gaosong			       2008.05.22		create first version
********************************************************************************************/
#ifndef _PC_SIM
ME_U32	parser_mid_release(void *fileinfor, me_audio_infor *audio_infor, me_video_infor *video_infor)
#else
ME_U32	parser_mid_release(void *fileinfor)
#endif
{
	VM_MIDI_PARSER_INFO *file=(VM_MIDI_PARSER_INFO *)fileinfor;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_mid_release()"));
	#endif
	if(file==NULL)  return ME_SUCCESS;
	me_printf("\n###parser_mid_release###\n");
	//if(file->mid_instrument) me_free(file->mid_instrument);
	//these memory must place SRAM of 848
	if(file->vm_input.pBankheader) 
	{
		me_printf("-->release pBankheader=0x%x\n", (MMD_U32)file->vm_input_core.pBankheader);
		me_free(file->vm_input.pBankheader);
		MMD_SYS_Free((MMD_U32)file->vm_input_core.pBankheader);
	}
	if(file->vm_input.pBankData) 
	{
		me_printf("-->release pBankData=0x%x\n", (MMD_U32)file->vm_input_core.pBankData);
		me_free(file->vm_input.pBankData);
		MMD_SYS_Free((MMD_U32)file->vm_input_core.pBankData);
	}
	if(file->vm_input.pAdpcmMapping) 
	{
		me_printf("-->release pAdpcmMapping=0x%x\n", (MMD_U32)file->vm_input_core.pAdpcmMapping);
		me_free(file->vm_input.pAdpcmMapping);
		MMD_SYS_Free((MMD_U32)file->vm_input_core.pAdpcmMapping);
	}
	if(file->vm_input.SeekInfo)  
	{
		me_printf("-->release SeekInfo=0x%x\n", (MMD_U32)file->vm_input_core.SeekInfo);
		me_free(file->vm_input.SeekInfo);
		MMD_SYS_Free((MMD_U32)file->vm_input_core.SeekInfo);
	}
	
	file->vm_input.total_len	 = 0;
	file->vm_input_core.total_len	 = 0;
	if(file->mid_Data) me_free(file->mid_Data);
	if(file->pvm_input) me_free(file->pvm_input);

	
	me_free(file);	
	
	return ME_SUCCESS;
}

/*******************************************************************************************
Description:
	check file is good ape file or not
Parameters:
       void *mediafile:		file handle(point)
       fs_callbacks *fs_filefunc:		file operate interface structor point
Return:	
      ME_TRUE:	 Yes
      ME_FALSE: No
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1		    Gaosong  			   2008.05.22		create first version
********************************************************************************************/
#ifndef _PC_SIM
ME_U32	parser_mid_check_filetype(void *mediafile, fs_callbacks *fs_filefunc)
#else
ME_U32	parser_mid_check_filetype(void *mediafile, me_file_t *fs_filefunc)
#endif
{

	ME_U08 readBuf[8];
	ME_S32 bytesLeft;
	ME_U32 MThd_length;
	char cmpBuf1[4]={0x4D,0x54,0x68,0x64};	//MThd
	int i;

	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_mid_check_filetype()"));
	#endif
	
#ifndef _PC_SIM
	fs_filefunc->seek_func(mediafile,0,SEEK_SET);
	bytesLeft=fs_filefunc->read_func(mediafile,(char*)readBuf,8);
#else
	fs_filefunc->me_seek(mediafile,0,SEEK_SET);
	bytesLeft=fs_filefunc->me_read(mediafile,readBuf,8);
#endif
	
	if(bytesLeft != 8)
		return ME_FALSE;
	
	for(i=0;i<4;i++)
	{
		if(readBuf[i] != cmpBuf1[i])
		{
			return ME_FALSE;
		}
	}
	MThd_length = ((unsigned int)readBuf[4] << 24) + ((unsigned int)readBuf[5] << 16) +
		((unsigned int)readBuf[6] << 8) + (unsigned int)readBuf[7];
	if (MThd_length != 6) 
	{
		return ME_FALSE;	
	}

	//The file is mid
#ifndef _PC_SIM
	fs_filefunc->seek_func(mediafile,0,SEEK_SET);
#else
	fs_filefunc->me_seek(mediafile,0,SEEK_SET);
#endif
	return ME_TRUE;
}

/*******************************************************************************************
Description:
	get ape file audio information
Parameters:
	void *fileinfor: file information structor create by parser
	me_audio_infor *audio_infor: audio information structor point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Gaosong			       2008.05.22		create first version
********************************************************************************************/

ME_U32	parser_mid_get_audio_infor(void *fileinfor, me_audio_infor *audio_infor)
{
	
	VM_MIDI_PARSER_INFO *file;
	ME_U32	ptr = 0;
	ME_U32 i = 0;
	ME_U32	wrsram_len = 0;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_mid_get_audio_infor()"));
	#endif

	
	file = (VM_MIDI_PARSER_INFO *)fileinfor;

	audio_infor->a_isvalid	= 0x01; 
	audio_infor->a_bits 	= file->mid_BitsPerSample;
	audio_infor->a_chn 		= file->mid_Channels;
	audio_infor->a_smplrate   = file->mid_SampleRate;
	audio_infor->a_smpllen 	= audio_infor->a_bits*audio_infor->a_chn/8;  
	audio_infor->a_totaltime   = file->mid_TotalTime; 
	audio_infor->a_codec[0]='m';//'m';
	audio_infor->a_codec[1]='i';//'i';
	audio_infor->a_codec[2]='d';//'d';
	audio_infor->a_codec[3]='.';//'.';

	if(file->pvm_input == NULL)
	{
		me_printf("first parser_mid_get_audio_infor\n");
		return ME_SUCCESS;
	}
	me_printf("second parser_mid_get_audio_infor\n");

	wrsram_len = sizeof(Download_Zone)*MAX_ZONE_LEN;
	if(wrsram_len%4 != 0)	wrsram_len += (4-(wrsram_len%4));
	Vm_WrSram((unsigned int)(file->vm_input_core.pBankheader), (char*)file->vm_input.pBankheader, wrsram_len);	
	wrsram_len = file->vm_input_core.BankDataLen;
	if(wrsram_len%4 != 0)	wrsram_len += (4-(wrsram_len%4));
	Vm_WrSram((unsigned int)(file->vm_input_core.pBankData), (char*)(file->vm_input.pBankData), wrsram_len);
	if(file->vm_input.BankDataType == 1)
	{
		wrsram_len = file->vm_input_core.MappingLen*sizeof(AdpcmWave_InstrumentMap);
		if(wrsram_len%4 != 0)	wrsram_len += (4-(wrsram_len%4));
		Vm_WrSram((unsigned int)(file->vm_input_core.pAdpcmMapping), (char*)(file->vm_input.pAdpcmMapping), wrsram_len);
	}
	wrsram_len = sizeof(SEEK_INFO);
	if(wrsram_len%4 != 0)	wrsram_len += (4-(wrsram_len%4));
	Vm_WrSram((ME_U32)(file->vm_input_core.SeekInfo), (char*)(file->vm_input.SeekInfo), wrsram_len);	
	//audio_infor->a_others=(void *)&(file->vm_input);

//:	Load vm_input into buffer
	me_printf("\nvm_input.total_len=%d\n", file->vm_input_core.total_len);
	me_printf("vm_input.SampleRate=0x%x\n", file->vm_input_core.SampleRate);
	me_printf("vm_input.MaxSythVoice=0x%x\n", file->vm_input_core.MaxSythVoice);
	me_printf("vm_input.TrackDataLen=0x%x\n", file->vm_input_core.TrackDataLen);
	me_printf("vm_input.Division=0x%x\n", file->vm_input_core.Division);

	me_printf("vm_input.ZoneLen=0x%x\n", file->vm_input_core.ZoneLen);
	me_printf("vm_input.pBankheader=0x%x\n", (ME_U32)(file->vm_input_core.pBankheader));
	me_printf("vm_input.BankDataLen=0x%x\n", file->vm_input_core.BankDataLen);
	me_printf("vm_input.pBankData=0x%x\n", (ME_U32)(file->vm_input_core.pBankData));
	me_printf("vm_input.MappingLen=0x%x\n", file->vm_input_core.MappingLen);
	me_printf("vm_input.pAdpcmMapping=0x%x\n", (ME_U32)(file->vm_input_core.pAdpcmMapping));
	me_printf("vm_input.SeekInfo=0x%x\n", (ME_U32)(file->vm_input_core.SeekInfo));
	
	((ME_U32*)(file->pvm_input))[ptr++] = file->vm_input_core.total_len;
	((ME_U32*)(file->pvm_input))[ptr++] = file->vm_input_core.SampleRate;
	((ME_U32*)(file->pvm_input))[ptr++] = file->vm_input_core.MaxSythVoice;
	((ME_U32*)(file->pvm_input))[ptr++] = file->vm_input_core.TrackDataLen;
	((ME_U32*)(file->pvm_input))[ptr++] = file->vm_input_core.Division;
	
	((ME_U32*)(file->pvm_input))[ptr++] = file->vm_input_core.ZoneLen;
	((ME_U32*)(file->pvm_input))[ptr++]  = (ME_U32)(file->vm_input_core.pBankheader);
	
	((ME_U32*)(file->pvm_input))[ptr++]  = file->vm_input_core.BankDataLen;
	((ME_U32*)(file->pvm_input))[ptr++]  = (ME_U32)(file->vm_input_core.pBankData);
	((ME_U32*)(file->pvm_input))[ptr++] = file->vm_input_core.BankDataType;

	((ME_U32*)(file->pvm_input))[ptr++] =  file->vm_input.MappingLen;
	((ME_U32*)(file->pvm_input))[ptr++] = (ME_U32)(file->vm_input_core.pAdpcmMapping);

	((ME_U32*)(file->pvm_input))[ptr] = (ME_U32)(file->vm_input_core.SeekInfo);

	
	me_printf("\npBankheader: \n");
	for(i=0; i<10; i++)
	{
		me_printf("0x%x ", ((ME_U08*)file->vm_input.pBankheader)[i]);
	}
	me_printf("\npBankData: \n");
	for(i=0; i<10; i++)
	{
		me_printf("0x%x ", ((ME_U08*)file->vm_input.pBankData)[i]);
	}	
	me_printf("\npAdpcmMapping: \n");
	for(i=0; i<10; i++)
	{
		me_printf("0x%x ", ((ME_U08*)file->vm_input.pAdpcmMapping)[i]);
	}	
	me_printf("\nSeekInfo: \n");
	for(i=0; i<10; i++)
	{
		me_printf("0x%x ", ((ME_U08*)file->vm_input.SeekInfo)[i]);
	}		
	me_printf(" \n");	

	audio_infor->a_others = (void*)(file->pvm_input);
	audio_infor->a_otherslen = file->vm_input_core.total_len;

	return ME_SUCCESS;
}


/*******************************************************************************************
Description:
	get ape file audio information
Parameters:
	void *fileinfor: file information structor create by parser
	me_audio_infor *audio_infor: audio information structor point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Gaosong			       2008.05.22		create first version
********************************************************************************************/

ME_U32	parser_mid_get_audio_infor_formidi(void *fileinfor, me_audio_infor *audio_infor)
{
	
	ME_U32 retval;
	VM_MIDI_PARSER_INFO *file= (VM_MIDI_PARSER_INFO *)fileinfor;
	
    	retval=get_mid_sound_bank(file); 
	
	file->pvm_input = me_malloc(file->vm_input_core.total_len);
	me_printf("###file->vm_input_core.total_len=0x%x\n", file->vm_input_core.total_len);
	if(retval!=ME_SUCCESS)
	{
		parser_mid_release(file,NULL,NULL);
		return ME_FAILE;
	}

	parser_mid_get_audio_infor(fileinfor, (me_audio_infor*)audio_infor);

	return ME_SUCCESS;
}

/*******************************************************************************************
Description:
	get audio max frame size
Parameters:
	void *fileinfor: file information structor create by parser
	ME_U32 *framesize: max audio frame size
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Gaosong			       2008.05.22		create first version
********************************************************************************************/
ME_U32	parser_mid_get_audio_max_frame_size(void *fileinfor, ME_U32 *framesize, void* audio_info)
{
	//ME_U32 retval;
	//VM_MIDI_PARSER_INFO *file= (VM_MIDI_PARSER_INFO *)fileinfor;
#if 0	
    	retval=get_mid_sound_bank(file); 
	
	file->pvm_input = me_malloc(file->vm_input_core.total_len);
	me_printf("###file->vm_input_core.total_len=0x%x\n", file->vm_input_core.total_len);
	if(retval!=ME_SUCCESS)
	{
		parser_mid_release(file,NULL,NULL);
		return ME_FAILE;
	}

	parser_mid_get_audio_infor(fileinfor, (me_audio_infor*)audio_info);
#endif
	*framesize=1024+256;
	return ME_SUCCESS;
}

/*******************************************************************************************
Description:
	get audio frame duration(ms)
Parameters:
	void *fileinfor: file information structor create by parser
	ME_U32 frame: frame index(from 0)
	ME_U32 * duration: frame duration(ms)
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Gaosong			       2008.05.22	   create first version
********************************************************************************************/

ME_U32	parser_mid_get_audio_duration(void *fileinfor, ME_U32 frame, ME_U32 * duration)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_mid_get_audio_duration()"));
	#endif

	return ME_SUCCESS;
}
/*******************************************************************************************
Description:
	 get the time the audio frame
Parameters:
        file: aac infor stuctor point (include file handle)
	 frame: audio frame id
        *time: audio frame start time
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Gaosong          		2008.05.22		create first version
********************************************************************************************/
#ifdef _PC_SIM
ME_U32	parser_mid_audio_time_of_frame(void *fileinfor, ME_U32  frame, ME_U32  *time)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE( ("->parser_mid_audio_time_of_frame()") );
	#endif	
//	*time = 30;
	return ME_SUCCESS;

}
/*******************************************************************************************
Description:
	 get the frame id of the time(ms)
Parameters:
        file: aac infor stuctor point (include file handle)
	 time: audio frame start time
        *frame: audio frame id
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History: 
	Version			Modifyby				DataTime			modified
	   0.1			Gaosong			       2008.05.22   	create first version
********************************************************************************************/
ME_U32	parser_mid_audio_frame_of_time(void *fileinfor, ME_U32  time, ME_U32  *frame)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE( ("->parser_mid_audio_frame_of_time()") );
	#endif	
//	*frame = 1;
	return ME_SUCCESS;
}
#endif

/*******************************************************************************************
Description:
	read one frame audio data form wav file
Parameters:
	void *fileinfor: file information structor create by parser
	char *audio_buffer: dest buffer point
	ME_U32 *readbytes: really read data length(bytes)
	ME_U32 bufferlength:buffer max leangth(bytes)
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Gaosong		           2008.05.22		create first version
********************************************************************************************/
//just for test currently
ME_U32	parser_mid_read_audio(void *fileinfor, char *audio_buffer, ME_U32 *readbytes,ME_U32 bufferlength)
{
	VM_MIDI_PARSER_INFO *file= fileinfor;
	ME_U32 readLength;
//	ME_U32 readpos;
	ME_U32 total_len=file->mid_DataLength+file->mid_DataStartPointBackup;
	
#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_mid_read_audio()"));
#endif
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_buffer!=NULL));
	
	readLength = 1024;

	if (total_len - file->mid_DataStartPoint < readLength ) 
	{
		readLength = total_len - file->mid_DataStartPoint;		
	}
	me_memcpy((char*)audio_buffer, (char*)(file->mid_Data+file->mid_DataStartPoint), readLength);
	*readbytes = readLength;
	file->mid_DataStartPoint += readLength;
//	readpos = file->mid_DataStartPoint;
	//	me_printf("Test-------read_length = %d,read_pos = %d\n",readbytes,readpos);
	//if(file->mid_DataStartPoint-readLength >= total_len)
	if(file->mid_DataStartPoint>=total_len)
	{
		return ME_FILE_END_OF_AUDIO;
		//return ME_FILE_LAST_OF_AUDIO;
	}
	
	return ME_SUCCESS;	

}




ME_U32	parser_mid_read_audio_frames(void *fileinfor, audio_stream_infor *audio_buffer)
{
	VM_MIDI_PARSER_INFO *file= (VM_MIDI_PARSER_INFO*)fileinfor;
	ME_U32 readLength;
	ME_U32 readpos;
	ME_U32 total_len=file->mid_DataLength+file->mid_DataStartPointBackup;
	ME_U08* tmp_addr;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_mid_read_audio_frames()"));
	#endif
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_buffer!=NULL));
	mid_frame_count++;

	if(file->file_end > 3)
	{
		return ME_FILE_END_OF_AUDIO;
	}

	readLength = 1024;

	tmp_addr = me_malloc(readLength);
	
	if ((total_len - file->mid_DataStartPoint) < readLength ) 
	{
		readLength = total_len - file->mid_DataStartPoint;		
	}
	//me_memcpy((char*)audio_buffer->buffer_address, (char*)(file->mid_Data+file->mid_DataStartPoint), readLength);

	me_memcpy((char*)tmp_addr, (char*)( file->mid_Data + file->mid_DataStartPoint), readLength);

#if 0
	me_printf("\nFrame data: Vm_WrSram addr=0x%x, readLength=0x%x\n", audio_buffer->buffer_address, readLength);
	me_printf("file->mid_DataStartPoint=0x%x\n", file->mid_DataStartPoint);
	for(i=0; i<10; i++)
	{
		me_printf("0x%x ",  tmp_addr[i]);
	}
	me_printf("\n");
#endif
	Vm_WrSram(audio_buffer->buffer_address,  (char*)tmp_addr, readLength);
	
	audio_buffer->data_length		= readLength;
	audio_buffer->isnot_empty	= readLength;

	audio_buffer->cursor_read = audio_buffer->cursor_write;
	audio_buffer->cursor_write += readLength;
	audio_buffer->frame_count = 1;
	
	file->mid_DataStartPoint += readLength;
	readpos = file->mid_DataStartPoint;
	//	me_printf("Test-------read_length = %d,read_pos = %d\n",readbytes,readpos);

	me_free((void*)tmp_addr);
	if(readpos >= total_len)
	{
		last_frame_flag = 1;
		audio_buffer->is_over = 1;
		return ME_FILE_LAST_OF_AUDIO;
	}
	else
	{
		return ME_SUCCESS;	
	}
}

/*******************************************************************************************
Description:
	read one frame audio data form ape file
Parameters:
	void *fileinfor: file information structor create by parser
	char *audio_buffer: dest buffer point
	ME_U32 *readbytes: really read data length(bytes)
	ME_U32 bufferlength:buffer max leangth(bytes)
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			xulei		2007.09.17		create first version
********************************************************************************************/
#ifndef _PC_SIM	//it's new version, midi need to realize this interface
ME_U32	parser_mid_read_audio_frames_reg(void *fileinfor, audio_stream_infor *audio_buffer, ME_U32 reg, ME_U32 bit)
{
	VM_MIDI_PARSER_INFO *file= (VM_MIDI_PARSER_INFO*)fileinfor;
	ME_U32 readLength;
	ME_U32 readpos;
	ME_U32 total_len=file->mid_DataLength+file->mid_DataStartPointBackup;
	//ME_U32 i = 0;
	ME_U08* tmp_addr;
	//ME_U32 audiostatus;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_mid_read_audio_frames()"));
	#endif
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_buffer!=NULL));
	mid_frame_count++;

	if(file->file_end > 3)
	{
		return ME_FILE_END_OF_AUDIO;
	}

	readLength = 1024;

	tmp_addr = me_malloc(readLength);
	
	if ((total_len - file->mid_DataStartPoint) < readLength ) 
	{
		readLength = total_len - file->mid_DataStartPoint;		
	}
	//me_memcpy((char*)audio_buffer->buffer_address, (char*)(file->mid_Data+file->mid_DataStartPoint), readLength);

	me_memcpy((char*)tmp_addr, (char*)( file->mid_Data + file->mid_DataStartPoint), readLength);

#if 0
	me_printf("\nFrame data: Vm_WrSram addr=0x%x, readLength=0x%x\n", audio_buffer->buffer_address, readLength);
	me_printf("file->mid_DataStartPoint=0x%x\n", file->mid_DataStartPoint);
	for(i=0; i<10; i++)
	{
		me_printf("0x%x ",  tmp_addr[i]);
	}
	me_printf("\n");
#endif
	Vm_WrSram(audio_buffer->buffer_address,  (char*)tmp_addr, readLength);
	
	audio_buffer->data_length		= readLength;
	audio_buffer->isnot_empty	= readLength;

	audio_buffer->cursor_read = audio_buffer->cursor_write;
	audio_buffer->cursor_write += readLength;
	audio_buffer->frame_count = 1;
	
	file->mid_DataStartPoint += readLength;
	readpos = file->mid_DataStartPoint;
	//	me_printf("Test-------read_length = %d,read_pos = %d\n",readbytes,readpos);

	me_free((void*)tmp_addr);
	if(readpos >= total_len)
	{
		last_frame_flag = 1;
		audio_buffer->is_over = 1;
		//audio_buffer->data_length	 = 0;
		media_player_audio_fillsrcbuf_smtd(bit, audio_buffer->data_length, 0, audio_buffer->is_over);	

		return ME_FILE_LAST_OF_AUDIO;
	}
	else
	{
		media_player_audio_fillsrcbuf_smtd(bit, audio_buffer->data_length, 0, 0);
		return ME_SUCCESS;	
	}
}
#endif

#ifndef _PC_SIM
ME_U32	parser_mid_seek(void *fileinfor, ME_U32 pos, ME_U32* accesspos, ME_U32 *videoframe)
#else
ME_U32	parser_mid_seek(void *fileinfor, ME_U32 pos, ME_U32* accesspos, ME_U32 *audioframe, ME_U32 *videoframe)
#endif
{
	VM_MIDI_PARSER_INFO *file= fileinfor;
	int bIdx;
	SEEK_INFO* SeekInfo=(SEEK_INFO*)file->vm_input.SeekInfo;
	
#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_mid_seek()"));
#endif

	//init mid_seek info

	for(bIdx=0; bIdx<MAX_MIDI_CHN; bIdx++)
	{
		SeekInfo->ChnInfo[bIdx].bTimbre	  	= 0x00;
		SeekInfo->ChnInfo[bIdx].bVolume     	= 0x7f;
		SeekInfo->ChnInfo[bIdx].bExpression 	= 0x7f;
		SeekInfo->ChnInfo[bIdx].bModulation 	= 0;
		SeekInfo->ChnInfo[bIdx].bPitchBendH 	= 0x40;
		SeekInfo->ChnInfo[bIdx].bPitchBendL 	= 0x00;
		SeekInfo->ChnInfo[bIdx].bBendRage  	= 0x02;  //bendrange= 0x02*100;
		SeekInfo->ChnInfo[bIdx].bPan     		= 0x40;
		SeekInfo->ChnInfo[bIdx].bHold1      	= 0x00;
		SeekInfo->ChnInfo[bIdx].bBankID      	= 0x00;
		if(bIdx==9)
			SeekInfo->ChnInfo[bIdx].bBankID      	= 0x80;

		SeekInfo->wRPN[bIdx] = 0x7f7f;		
	}	
	//SeekInfo->bMsgQueNum	= 0;
	SeekInfo->dTempoMS  = 0;
	
	/////////////////////////////////////////

	if(pos == 0)
	{
		*accesspos = 0;
		file->mid_DataStartPoint = file->mid_DataStartPointBackup;
		SeekInfo->LeftLen = file->mid_DataLength;
		return ME_SUCCESS;
	}
	if(pos >= file->mid_TotalTime)
	{
		*accesspos = file->mid_TotalTime;
		file->mid_DataStartPoint = file->mid_DataLength + file->mid_DataStartPointBackup;
		SeekInfo->LeftLen=0;
		return ME_SUCCESS;
	}

	*accesspos = SeekMsg(file, pos);	
	//AddMsgQue(SeekInfo);
	//g_MidiInfo->dSeekOverTime= g_MidiInfo->dCurTime/1000 - dPos;
	return ME_SUCCESS;
}

/*******************************************************************************************
Description:
	get ape file video information
Parameters:
	void *fileinfor: file information structor create by parser
	me_video_infor *video_infor: video information structor point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Gaosong			       2008.05.25		create first version
********************************************************************************************/
ME_U32	parser_mid_get_video_infor(void *fileinfor, me_video_infor *video_infor)
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_mid_get_video_infor()"));
	#endif

	video_infor->v_isvalid = 0;
	return ME_SUCCESS;
}

//called after filesys has transfered data on SD to 848core.
ME_U32 mid_read_audio_frame_ver3_post(VM_MIDI_PARSER_INFO *file, audio_stream_infor *audio_buffer)
{
	return ME_SUCCESS;
}


ME_U32	parser_mid_read_audio_frames_post(void *fileinfor, audio_stream_infor *audio_buffer)
{
	VM_MIDI_PARSER_INFO *file = (VM_MIDI_PARSER_INFO *)fileinfor;
	ME_U32 result=0;
	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_mid_read_audio_frames_post()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((fileinfor!=NULL));
	ME_ASSERT_PARAM_RETURN((audio_buffer!=NULL));

	result = mid_read_audio_frame_ver3_post(file, (audio_stream_infor *)audio_buffer);
	return result;	
}

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
Description:
	parser mid register
Parameters:
     	No
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Gaosong			      2008.05.25		create first version
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

ME_U32	parser_mid_register()
{
	me_parser_t* pthis;

	#if ME_KM_FUNNAME
	ME_MESSAGE(("->parser_mp3_register()"));
	#endif
	
#if ME_KM_OBJ_STACK
	pthis	= &_g_parser_mid;
#else
	pthis	= (me_parser_t*)MMD_Malloc( sizeof(me_parser_t));
	if(pthis==NULL)
	{	
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
		return	ME_ERR_MALLOC_MEMORY_ERROR;	
	}
#endif
	
	parser_register_initialize(pthis);
	ME_SPRINTF(pthis->id, "mid");
	
	pthis->init					  	= parser_mid_init;
	pthis->release				  	= parser_mid_release;
	pthis->check_file_type			  	= parser_mid_check_filetype;
	pthis->seek				        	= parser_mid_seek;
	
	pthis->get_audio_info			  	= parser_mid_get_audio_infor;
	pthis->get_audio_info_formidi		= parser_mid_get_audio_infor_formidi;
	pthis->get_audio_frame_buffersize	= parser_mid_get_audio_max_frame_size;
	pthis->get_audio_frame_duration		= parser_mid_get_audio_duration;
	pthis->read_audio_oneframe    		= parser_mid_read_audio;
	pthis->read_audio_frames			= parser_mid_read_audio_frames;

	pthis->get_video_info		         	= parser_mid_get_video_infor;

	pthis->read_audio_frames_reg		= parser_mid_read_audio_frames_reg;
	pthis->read_audio_frames_post		= parser_mid_read_audio_frames_post;
	
	return me_parser_register(pthis);	/*×¢²ámid Parser*/

}
#endif
