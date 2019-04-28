#pragma once
#ifndef _MEMORYINFODATA_H_
#define _MEMORYINFODATA_H_
#include <windows.h>

class MemoryData
{
public:
	MemoryData();
	virtual ~MemoryData();

public:
	// 保存线程句柄
	static HANDLE hMemThread;
	// 保存进程句柄
	static HANDLE hMemProcess;
	// 默认Eip查看
	void MemoryDatas();
	// 带参数查看
	void MemoryDatas(const DWORD dAddress);
	// 修改内存数据
	void MemoryDatas(const DWORD dAddress, const DWORD ModifyMemData);
	
private:
	// 显示内存数据
	void ShowMemoryDatas();

private:
	// 保存读取的内存数据
	BYTE MemoryArrData[100];
	// 原地址更变使用
	DWORD Address = 0;

};

#endif