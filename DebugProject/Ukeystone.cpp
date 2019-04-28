#include "UKeystone.h"
#include "iostream"
#include "XEDParse/XEDParse.h"
#pragma comment(lib, "XEDParse\\x86\\XEDParse_x86.lib")

using std::cout; using std::endl;

KeysTone::KeysTone()
{

}

KeysTone::~KeysTone()
{

}

// 初始化Assembly
void KeysTone::InitAssembly()
{
	// 初始化ks_open
	if (KS_ERR_OK != ks_open(KS_ARCH_X86, KS_MODE_32, &pengine)) {
		cout << "ks_open failuer()";
		return;
	}
}

// 修改汇编指令成源码
BOOL KeysTone::ModifyOpCode(const HANDLE hthread, const DWORD MemoryAddress, const char *ModifyData)
{
	this->InitAssembly();
	nRet = ks_asm(
		pengine,					/* 汇编引擎句柄，通过ks_open函数得到*/
		ModifyData,					/* 要转换的汇编指令*/
		NULL,						/* 汇编指令所在的地址*/
		&opcode,					/* 输出的opcode*/
		&nOpCcodeSize,				/* 输出的opcode的字节数*/
		&stat_count					/* 输出成功汇编的指令的条数*/
		);

	// 1. 返回值等于-1时反汇编错误
	if (nRet == -1)
	{
		// 输出错误信息
		// ks_errno 获得错误码
		// ks_strerror 将错误码转换成字符串，并返回这个字符串
		printf("错误信息：%s\n", ks_strerror(ks_errno(pengine)));
		return FALSE;
	}
	DWORD dwSize = 0;
	DWORD dwAtt = 0;
	// 2. 赋值属性
	SavaMemoryAdd = MemoryAddress;
	// 3. 修改内存地址属性
	VirtualProtectEx(MasterBpHandle, (LPVOID)MemoryAddress, 16, PAGE_READWRITE, &dwAtt);
	// 4. 保存原来的汇编OpCode
	ReadProcessMemory(MasterBpHandle, (LPVOID)MemoryAddress, Oldopcode, 16, &read);
	// 5. 写入新的汇编OpCode
	if (!WriteProcessMemory(MasterBpHandle, (LPVOID)MemoryAddress, opcode, nOpCcodeSize, &dwSize)){
		printf("写入汇编指令失败，内存属性修改，权限不够\n");
		return FALSE;
	}
	// 6. 恢复原来属性
	VirtualProtectEx(MasterBpHandle, (LPVOID)MemoryAddress, 16, dwAtt, &dwAtt);
	// 7. 释放空间
	ks_free(opcode);
	// 8. 关闭句柄
	ks_close(pengine);
	return TRUE;
}

// 恢复原来数据
BOOL KeysTone::AttData()
{
	DWORD dwSize = 0;
	if (!WriteProcessMemory(MasterBpHandle, (LPVOID)SavaMemoryAdd, &Oldopcode, read, &dwSize)){
		printf("写入汇编指令失败， 读写权限已更改，权限不够");
		return FALSE;
	}
	printf("已撤销修改属性");
	return TRUE;
}