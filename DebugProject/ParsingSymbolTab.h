#pragma once
#ifndef _PARSINGSYSBOLTAB_H_
#define _PARSINGSYSBOLTAB_H_
#include <windows.h>

class ParSymbol
{
public:
	ParSymbol();
	virtual ~ParSymbol();

public:
	void puSetProcessHandle(HANDLE process){ this->ObjectProcess = process; }

private:
	// 初始化调试符号
	void InitParSymbol();
	// 显示符号
	void ShowParSymbol();

private:
	// 保存有效进程句柄
	HANDLE ObjectProcess;
};

#endif 