#pragma once
#ifndef _REGISTER_H_
#define _REGISTER_H_
#include <windows.h>

class RegisterData
{
public:
	RegisterData();
	~RegisterData();

public:
	// 保存线程句柄
	static HANDLE hReThread;
	// 显示寄存器信息
	void RegisterInfo();
	// 修改存起信息
	void RegisterInfo(int nRegisterFlag, DWORD dRegisterFlagedata);

private:
	// 保存寄存器信息
	CONTEXT ct;
};

#endif