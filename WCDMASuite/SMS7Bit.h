// SMS7Bit.h: interface for the CSMS7Bit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SMS7BIT_H__8AC2170D_81F3_451A_BAD7_F0768CEE1010__INCLUDED_)
#define AFX_SMS7BIT_H__8AC2170D_81F3_451A_BAD7_F0768CEE1010__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

struct T7BitASCII
{
	BYTE b7Bit1;
	BYTE b7Bit2;
	TCHAR bASCII;
};

class CSMS7Bit  
{
public:
	CSMS7Bit();
	virtual ~CSMS7Bit();
public:
	bool Encode(BYTE& b1,BYTE& b2, BYTE bASCII);
	bool Decode(BYTE b1,BYTE b2,TCHAR& bASCII);
private:
	void SetInitValue();
	void ClearValue();
private:
	std::vector<T7BitASCII*> m_7Bit;
};

#endif // !defined(AFX_SMS7BIT_H__8AC2170D_81F3_451A_BAD7_F0768CEE1010__INCLUDED_)
