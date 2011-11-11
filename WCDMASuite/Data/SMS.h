#ifndef _SMS_H_
#define _SMS_H_

//////////////////////////////////
//嵌入端短信类型
enum ESMSType
{
	SMS_UNREAD = 0,
	SMS_READ,	
	SMS_UNSEND,
	SMS_SEND,
};

//PC端短信类型
enum PSMSType
{
	SMS_PC_RECV = 0,	//收件箱
	SMS_PC_SEND,		//已发送
	SMS_PC_UNSEND,		//未发送
	SMS_PC_UNREAD		//未读
};

enum SMSMailBoxID
{
	SMS_MAILBOX_RECV = 0,	//收件箱
	SMS_MAILBOX_SEND,		//已发送
	SMS_MAILBOX_UNSEND,		//未发送
	SMS_MAILBOX_DRAFT,		//草稿箱
	SMS_MAILBOX_RECYLE		//回收站
};

struct TSMSItem
{
	CString strPhoneNum;	//电话号码
	CTime tmSMSTime;		//短信时间
	CString strText;		//内容
	ESMSType iSMSState;		//嵌入端短信类型
	PSMSType nPSMSState;	//PC端短信类型
	int iSaveDays;//保存天数 (发件箱使用) -1无效，永远保存
	int iID; //ID,uim卡中的序号，对于pc机的，由sqlite3自动分配
	SMSMailBoxID nMailBoxID;		//信箱ID 0收件箱1已发送2未发送3草稿箱4回收站 
	SMSMailBoxID nOrigMailBoxID;	//原信箱ID,用于回收站恢复操作
	int nIndex;		//在信箱中的索引


	CString TimeString()const;

	bool operator==(const TSMSItem& item)const;

	TSMSItem();
};

struct ISMSObserver
{
	virtual void OnSMSAdd(size_t index, const TSMSItem& sms) = 0;
	virtual void OnSMSSort() = 0;
};

class CSMS
{
public:
	virtual void Clear() = 0;					
	virtual TSMSItem& GetAt(size_t nIndex) = 0;	
	virtual const TSMSItem& GetAt(size_t nIndex)const = 0;	
	virtual void Delete(size_t nID) = 0;	
	virtual bool FindID(int id)const = 0;	
	
	virtual TSMSItem& AddBack(const TSMSItem& item) = 0;							
	virtual TSMSItem& AddFront(const TSMSItem& item) = 0;												

	virtual size_t Size()const =0;						

	virtual void SetLimit(size_t nLimit) = 0;
	virtual size_t GetLimit()const = 0;

	virtual bool Find(const TSMSItem& item)const = 0;

	virtual void AddObserver(ISMSObserver* pObserver) = 0;
	virtual void RemoveObserver(ISMSObserver* pObserver) = 0;

};

#endif