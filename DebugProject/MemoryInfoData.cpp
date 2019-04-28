#include "MemoryInfoData.h"
#include <stdio.h>
#include <ctype.h>

HANDLE MemoryData::hMemThread = nullptr;
HANDLE MemoryData::hMemProcess = nullptr;

MemoryData::MemoryData()
{
}

MemoryData::~MemoryData()
{
}

// 默认Eip查看
void MemoryData::MemoryDatas()
{
	//MemoryArrData[0] = 0;
	CONTEXT ct = { 0 };
	DWORD dwSize = 0;
	ct.ContextFlags = CONTEXT_ALL;
	GetThreadContext(MemoryData::hMemThread, &ct);
	Address = ct.Eip;
	auto ErrorCode = ReadProcessMemory(
		MemoryData::hMemProcess,
		(LPVOID)ct.Eip,
		MemoryArrData,
		sizeof(MemoryArrData),
		&dwSize
		);
	ShowMemoryDatas();
}

// 带参数查看
void MemoryData::MemoryDatas(const DWORD dAddress)
{
	// 来数据转成ASCII码：toascii()
	MemoryArrData[0] = 0;
	DWORD dwSize = 0;
	Address = dAddress;
	auto ErrorCode = ReadProcessMemory(
		MemoryData::hMemProcess,
		(LPVOID)dAddress,
		MemoryArrData,
		sizeof(MemoryArrData),
		&dwSize
		);
	ShowMemoryDatas();
}

// 修改内存数据
void MemoryData::MemoryDatas(const DWORD dAddress, const DWORD ModifyMemData)
{
	DWORD dwSize = 0;
	DWORD dwOldAttrubet = 0;
	// 内存可读可写
	// VirtualProtect((LPVOID)dAddress, sizeof(ModifyMemData), PAGE_READWRITE, &dwSize);
	// 写入内存数据
	if (WriteProcessMemory(MemoryData::hMemProcess, (LPVOID)dAddress, &ModifyMemData, sizeof(ModifyMemData), &dwSize))
		printf("修改内存成功\n");
	else
		printf("修改内存失败,访问内存位置无效\n");
}

// 显示内存数据
void MemoryData::ShowMemoryDatas()
{
	int ndCount = 0;
	int naCount = 0;
	BOOL Flag = FALSE;
	while (1)
	{
		// 1. 输出显示地址
		printf("%08X    ", Address);
		// 2. 循环输出数据
		for (int i = 0; i < 16; ++i)
		{
			printf("%02X ", MemoryArrData[ndCount]);
			++ndCount;
			if (96 == ndCount)
			{
				Flag = TRUE;
				break;
			}
		}
		printf("    ");
		// 3. 输出ascci
		for (int i = 0; i < 16; ++i)
		{
			if (MemoryArrData[naCount] >= 0x00 && MemoryArrData[naCount] <= 0x1F) {
				printf(".");
			}
			else if (MemoryArrData[naCount] >= 0x81 && MemoryArrData[naCount] <= 0xFF) {
				printf("?");
			}
			else
				printf("%X", __toascii(MemoryArrData[naCount]));
			++naCount;
			if (96 == naCount)
			{
				Flag = TRUE;
				break;
			}
		}
		printf("\n");
		Address += 16;
		if (Flag)
			break;
	}
}