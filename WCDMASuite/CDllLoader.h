#pragma once

class CDLLLoader
{
	HMODULE m_Handle;
private:
	CDLLLoader(const CDLLLoader&);
	CDLLLoader& operator=(const CDLLLoader&);
public:
	CDLLLoader(const TCHAR* dllFileName)
	{
		m_Handle = ::LoadLibrary(dllFileName);
	}

	~CDLLLoader()
	{
		if(m_Handle)
		{
			::FreeLibrary(m_Handle);
			m_Handle = NULL;
		}
	}

	void* GetFunction(const char* funcName)
	{
		return m_Handle ? ::GetProcAddress(m_Handle, funcName) : NULL;
	}
};