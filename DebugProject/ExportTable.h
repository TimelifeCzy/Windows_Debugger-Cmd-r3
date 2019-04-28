#pragma once
#ifndef _EXPORT_TABLE_H_
#define _EXPORT_TABLE_H_
#include <windows.h>

class ExportTab
{
public:
	ExportTab();
	virtual ~ExportTab();

public:
	void puShowExportTable(){ this->ShowExportTable(); }

private:
	// 保存加载基址
	char* m_lpBase;
	// 初始化导出表
	void InitExportTable();
	// 显示导出表
	void ShowExportTable();
	// 保存Nt头
	DWORD pNtheaders = 0;
	// 导出表结构体
	PIMAGE_EXPORT_DIRECTORY pExport;
	// 保存三个表（地址、序号、函数名）FOA
	DWORD* pFunAdd;
	DWORD* pNameAddr;
	DWORD* pOrdinalAddr;
	// 保存导出表
	PIMAGE_DATA_DIRECTORY pData;
};


#endif