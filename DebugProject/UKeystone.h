#pragma once
#ifndef _UKEYSTONE_H_
#define _UKEYSTONE_H_
#include "keystone\keystone.h"
#include "windows.h"

#pragma comment (lib, "keystone\\x86\\keystone_x86.lib")

class KeysTone
{
public:
	KeysTone();
	virtual ~KeysTone();

public:
	BOOL puModifyOpenCode(const HANDLE hthread, const DWORD MemoryAddress, const char *ModifyData) {
		auto nRet = this->ModifyOpCode(hthread, MemoryAddress, ModifyData);
		return nRet;
	}
	void puSetBpHandle(const HANDLE masterhandle){ this->MasterBpHandle = masterhandle; }
	// 恢复原来断点
	void puAttData() { this->AttData(); }

private:
	ks_engine *pengine = NULL;
	// 初始化汇编引擎
	void InitAssembly();
	// 修改OpenCode
	BOOL ModifyOpCode(const HANDLE hthread, const DWORD MemoryAddress, const char *ModifyData);
	// 恢复原来断点
	BOOL KeysTone::AttData();

private:
	// 汇编得到的opcode的缓冲区首地址
	unsigned char* opcode = NULL;
	// 汇编出来的opcode的字节数
	unsigned int nOpCcodeSize = 0;
	// 保存函数的返回值，用于判断函数是否执行成功
	int nRet = 0;
	// 保存成功汇编的指令的条数
	size_t stat_count = 0;
	// 保存传递来的被调试句柄
	HANDLE MasterBpHandle;
	// 保存原内存地址数据
	unsigned char* Oldopcode = NULL;
	// 保存读了多大的OpCode
	DWORD read = 0;
	// 保存撤销的内存地址
	DWORD SavaMemoryAdd;
};

#endif