#include "StdAfx.h"
#include "SMS.h"


TSMSItem::TSMSItem()
{
	iSMSState = SMS_UNSEND;
	iSaveDays = 0;
	iID = 0;
}

bool TSMSItem::operator==(const TSMSItem& item)const
{
	return	strPhoneNum == item.strPhoneNum &&
			tmSMSTime == item.tmSMSTime &&
			strText == item.strText;
}

CString TSMSItem::TimeString()const
{
	return tmSMSTime.Format("%Y-%m-%d %H:%M:%S");
}
