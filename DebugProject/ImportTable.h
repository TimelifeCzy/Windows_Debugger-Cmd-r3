#pragma once
#ifndef _IMPORT_TABLE_H_
#define _IMPORT_TABLE_H_
#include <windows.h>

class ImportTab
{
public:
	ImportTab();
	virtual ~ImportTab();

public:
	void puShowTmportTab(){ this->ShowTmportTab(); }
	void puSetProcess(const HANDLE process) { this->m_process = process; }

private:
	// 初始化导入表
	void InitImportTab();
	// 显示导入表
	void ShowTmportTab();
	// NT头(DWORD)
	DWORD pNtheaders;
	// pData
	PIMAGE_DATA_DIRECTORY pData;
	// 文件加载基址
	char* m_lpBase;
	// 导出表结构体
	PIMAGE_IMPORT_DESCRIPTOR pImport;
	// 保存THUNK_DATA32
	PIMAGE_THUNK_DATA32 pTunk;
	// 保存进程
	HANDLE m_process;


};

#endif