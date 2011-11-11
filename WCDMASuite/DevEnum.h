#ifndef DEVENUM_D8C3E302E2634388AE23F2C0B6526E9F
#define DEVENUM_D8C3E302E2634388AE23F2C0B6526E9F

#include <vector>

struct TModemInfo
{
	CString strHardwareID;
	CString strPortAttached;
	CString strFriendlyName;

	bool operator==(const TModemInfo& another)const;
};

class CModemList
{
public:
	CModemList();
	size_t Count()const;
	const TModemInfo& GetAt(size_t index)const;
	void Remove(const TModemInfo& info);
private:
	void Load();
private:
	std::vector<TModemInfo> m_Modems;
};


struct TPortInfo
{
	CString strHardwareID;
	CString strCOM;
	CString strFriendlyName;

	bool operator==(const TPortInfo& another)const;
};

class CPortList
{
public:
	CPortList();
	size_t Count()const;
	const TPortInfo& GetAt(size_t index)const;
private:
	void Load();
private:
	std::vector<TPortInfo> m_Ports;
};

#endif
