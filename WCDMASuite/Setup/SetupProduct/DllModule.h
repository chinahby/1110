#ifndef DLLMODULE_HEADER_
#define DLLMODULE_HEADER_

class CDllModule
{
public:
	CDllModule(const CString& strDllName)
	{
		m_hLib = LoadLibrary(strDllName);
	}
	~CDllModule()
	{
		if(m_hLib)
		{
			::FreeLibrary(m_hLib);
			m_hLib = NULL;
		}
	}

	void* GetFuncAdd(const CString& strFuncName)
	{
		FARPROC pFunc = NULL;
		if(m_hLib)pFunc = ::GetProcAddress(m_hLib, strFuncName);
		
		return pFunc;
	}
private:
	CDllModule(const CDllModule&);
	CDllModule& operator=(const CDllModule&);

	HMODULE m_hLib;
};


#endif