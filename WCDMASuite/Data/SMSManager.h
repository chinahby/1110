#ifndef _SMSMANAGER_H_
#define _SMSMANAGER_H_

#include "SMSPC.h"
#include "SMSEmbed.h"

class CSMSManager
{
public:
	void AddNewEmbedSMS(const TSMSItem& info);
	void SetEmbedSMSBoxLimit(size_t size);
public:
	CSMSPC& GetSMSPCRecv();
	const CSMSPC& GetSMSPCRecv()const;
	
	CSMSPC& GetSMSPCSend();
	const CSMSPC& GetSMSPCSend()const;
	
	CSMSPC& GetSMSPCNotSend();
	const CSMSPC& GetSMSPCNotSend()const;

	CSMSPC& GetSMSPCDraft();
	const CSMSPC& GetSMSPCDraft()const;

	CSMSPC& GetSMSPCRecyle();
	const CSMSPC& GetSMSPCRecyle()const;

	CSMSEmbed& GetSMSEmbedRecv();
	const CSMSEmbed& GetSMSEmbedRecv()const;

	CSMSEmbed& GetSMSEmbedSend();
	const CSMSEmbed& GetSMSEmbedSend()const;

	CSMSEmbed& GetSMSEmbedDraft();
	const CSMSEmbed& GetSMSEmbedDraft()const;

	bool  IsReadID(int ID)const;
private:
	CSMSPC m_PCRecv;			//PC收件箱
	CSMSPC m_PCSend;			//PC已发送
	CSMSPC m_PCNotSend;			//PC未发送
	CSMSPC m_PCDraft;			//PC草稿箱
	CSMSPC m_PCRecyle;			//PC回收站

	CSMSEmbed m_EmbedRecv;		//嵌入端收件箱
	CSMSEmbed m_EmbedSend;		//嵌入端已发送
	CSMSEmbed m_EmbedDraft;		//嵌入端草稿箱
};

#endif
