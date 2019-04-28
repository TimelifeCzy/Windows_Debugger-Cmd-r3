#ifndef _MODULEENTRY_H_
#define _MODULEENTRY_H_
#include <Windows.h>
#include <atlstr.h>

class ModuleEntry
{
public:
	ModuleEntry();
	virtual ~ModuleEntry();

public:
	void puShowProcessModule(){ this->ShowProcessModule(); }
	void SetProcessHandle(const DWORD processid){ this->SaveMasterProessId = processid; }

private:
	// 显示进程模块
	BOOL ShowProcessModule();
	// 保存进程句柄
	DWORD SaveMasterProessId = 0;
	// 保存文件版本信息
	char VersionInfobuf[MAX_PATH];
	// 保存文件版本信息
	char* pVerValue = NULL;
	// 定义数据
	CString strSubBlock, strTranslation, strTemp;
};


#endif