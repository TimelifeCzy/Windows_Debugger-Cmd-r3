#ifndef _MEMORYBREAKPOINT_H_
#define _MEMORYBREAKPOINT_H_
#include <windows.h>
#include "stdio.h"


class MemoryBp
{
public:
	MemoryBp();
	virtual ~MemoryBp();
	// 保存原数据结构体，Int3还原使用
	typedef struct _BREAKPOINT
	{
		DWORD addRess;
		BYTE oldData;
		char Type[20];
	}BREAKPOINT, *PBREAKPOINT;


public:
	// 保存句柄信息
	static HANDLE ProcessHandle;
	BREAKPOINT puSetMeoryBp(DWORD Address, BYTE Type, BOOL *MemFlag) {
		*MemFlag = this->SetrwxMemoryBp(Address, Type);
		return breakPoint;
	}
	// 设置句柄
	void puSetHadnle(HANDLE MasterHandler) { 
		this->MasterBreakBpHandle = MasterHandler; 
	}
	// 恢复句柄
	void puRestorMempryPro(BOOL *MemFlag){
		if (!this->RestoreMemoryPropert()) {
			printf("老铁恢复失败了！");
			*MemFlag = TRUE;
		}
		else
			*MemFlag = FALSE;
	}

private:
	// 结构体
	EXCEPTION_RECORD excepRecord;
	// 初始化内存断点
	BOOL InitMemoryBp();
	// 设置内存读写断点
	BOOL SetrwxMemoryBp(DWORD Address, BYTE Type);
	// 恢复内存属性
	BOOL RestoreMemoryPropert();
	// 保存断点数据
	BREAKPOINT breakPoint;
	// 保存句柄
	HANDLE MasterBreakBpHandle;
	// 保存内存断点地址及属性上
	static DWORD BpMemoryAddress;
	static DWORD m_dwAtt;

};


#endif