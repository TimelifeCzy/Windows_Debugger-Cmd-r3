#include "StackInfoData.h"
#include <stdio.h>

HANDLE StackData::hStackThread = nullptr;

HANDLE StackData::hStackProcess = nullptr;

StackData::StackData()
{

}

StackData::~StackData()
{

}

// 初始化栈信息
void StackData::InitStackInfoData()
{
	// 初始化信息数据
	ct = { 0 };
	StackDatabuf[0] = 0;
	ct.ContextFlags = CONTEXT_ALL;
	DWORD dwsize = 0;
	GetThreadContext(StackData::hStackThread, &ct);
	ReadProcessMemory(StackData::hStackProcess, (LPVOID)ct.Esp, StackDatabuf, sizeof(StackDatabuf), &dwsize);
	stackAddress = ct.Esp;
}

// 显示栈信息
void StackData::ShowStackInfoData()
{
	InitStackInfoData();
	printf(" 栈地址\t");
	printf("\t  数据\n");
	for (int i = 0; i < 10; ++i)
	{
		printf("%08X\t", stackAddress);
		// 打印栈内容
		printf("%08X\n", ((DWORD *)StackDatabuf)[i]);
		stackAddress += 4;
	}
}