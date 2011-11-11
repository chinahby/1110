#include "StdAfx.h"
#include "SMSManager.h"


CSMSPC& CSMSManager::GetSMSPCRecv()
{
	return m_PCRecv;
}
const CSMSPC& CSMSManager::GetSMSPCRecv()const
{
	return m_PCRecv;
}

CSMSPC& CSMSManager::GetSMSPCSend()
{
	return m_PCSend;
}
const CSMSPC& CSMSManager::GetSMSPCSend()const
{
	return m_PCSend;
}

CSMSPC& CSMSManager::GetSMSPCNotSend()
{
	return m_PCNotSend;
}
const CSMSPC& CSMSManager::GetSMSPCNotSend()const
{
	return m_PCNotSend;
}

CSMSPC& CSMSManager::GetSMSPCDraft()
{
	return m_PCDraft;
}
const CSMSPC& CSMSManager::GetSMSPCDraft()const
{
	return m_PCDraft;
}

CSMSPC& CSMSManager::GetSMSPCRecyle()
{
	return m_PCRecyle;
}
const CSMSPC& CSMSManager::GetSMSPCRecyle()const
{
	return m_PCRecyle;
}

CSMSEmbed& CSMSManager::GetSMSEmbedRecv()
{
	return m_EmbedRecv;
}
const CSMSEmbed& CSMSManager::GetSMSEmbedRecv()const
{
	return m_EmbedRecv;
}

CSMSEmbed& CSMSManager::GetSMSEmbedSend()
{
	return m_EmbedSend;
}
const CSMSEmbed& CSMSManager::GetSMSEmbedSend()const
{
	return m_EmbedSend;
}

CSMSEmbed& CSMSManager::GetSMSEmbedDraft()
{
	return m_EmbedDraft;
}
const CSMSEmbed& CSMSManager::GetSMSEmbedDraft()const
{
	return m_EmbedDraft;
}

bool  CSMSManager::IsReadID(int ID)const
{
	return m_EmbedSend.FindID(ID) || m_EmbedRecv.FindID(ID) || m_EmbedDraft.FindID(ID);
}

void CSMSManager::AddNewEmbedSMS(const TSMSItem& info)
{
	if(IsReadID(info.iID))
		return;//ÖØ¸´ID²»¼ÇÂ¼

	switch(info.iSMSState)
	{
	case SMS_READ:
	case SMS_UNREAD:
		m_EmbedRecv.AddFront(info);
		break;
	case SMS_SEND:
	case SMS_UNSEND:
		m_EmbedSend.AddFront(info);
		break;
	}
}

void CSMSManager::SetEmbedSMSBoxLimit(size_t size)
{
	this->GetSMSEmbedRecv().SetLimit(size);
	this->GetSMSEmbedSend().SetLimit(size);
}