#pragma once
#ifndef _STACKINFODATA_H_
#define _STACKINFODATA_H_
#include <windows.h>

class StackData
{
public:
	StackData();
	virtual ~StackData();


public:
	// 保存当前进程句柄
	static HANDLE hStackThread;
	// 保存进程句柄
	static HANDLE hStackProcess;
	// 显示栈信息接口
	void puShowStackInfoData() { this->ShowStackInfoData(); }

private:
	// 初始化栈信息
	void InitStackInfoData();
	// 显示栈信息
	void ShowStackInfoData();

private:
	// 保存线程线下信息
	CONTEXT ct;
	// 保存栈中的数据
	BYTE StackDatabuf[521];
	// 保存栈地址
	DWORD stackAddress = 0;
};

#endif