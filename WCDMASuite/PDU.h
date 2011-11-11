#pragma once

#include <string>
#include <vector>


using std::string;
using std::vector;


namespace PDU
{
	
	struct Timestamp
	{
		WORD year;
		BYTE month;
		BYTE day;
		BYTE hour;
		BYTE minute;
		BYTE second;
	};

	class PDUIn
	{
		string m_SMSCenterAddress;
		string m_Number;
		std::tstring m_Text;
		Timestamp m_Timestamp;
	public:
		PDUIn(const BYTE* PDUBytes, size_t PDUBytesCount,size_t smsType);
		const string& CenterAddress()const;
		const string& Number()const;
		const std::tstring& Text()const;
		const Timestamp& Time()const;
	private:
		void Extract(const BYTE* PDUBytes, size_t PDUBytesCount,size_t smsType);
	};

	class PDUOut
	{
		string m_Number;
		string m_SMSCenterAddress;
		string m_Text;
		int m_PDUDataLen;
	public:
		PDUOut(const string& number, const string& SMSCenter, const string& text);
		vector<BYTE> Encode(BYTE id);

		int DataLength()const{return m_PDUDataLen;}
	};
}