#ifndef PCMCIA_HELP_HEADER
#define PCMCIA_HELP_HEADER

#include "thread.h"
//监视硬件安装向导窗口，若有，则关闭它
class CWizardMonitor: public CThread
{
protected:
	virtual void Go();
};

//修改OXser.inf创建一个临时文件
CString CreateTempInf(const CString& strDstPath, const CString& strHWID);
void DeleteTempInf(const CString& strDstPath);

#endif