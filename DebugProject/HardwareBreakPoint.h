#ifndef _HARDWAREBREAKPOINT_H_
#define _HARDWAREBREAKPOINT_H
#include "windows.h"

// 保存原数据结构体，Int3还原使用
typedef struct _BREAKPOINT
{
	DWORD addRess;
	BYTE oldData;
	char Type[20];
}BREAKPOINT, *PBREAKPOINT;

class HardwareBreakP
{
public:
	HardwareBreakP();
	virtual ~HardwareBreakP();
	// 保存HardwareThread
	static HANDLE hHardThread;

public:
	// 初始化硬件断点
	BREAKPOINT puInitHardwareBreakP(const DWORD uAddress, CONTEXT* m_conText){
		hwBreakPoint = { 0 };
		this->voidHardwareBreakP(uAddress, m_conText);
		return hwBreakPoint;
	}
	// 设置硬件读写断点
	BREAKPOINT puReadWriteHardbp(CONTEXT* m_conText, DWORD uAddress, const WORD type, const WORD dwLen) {
		hwBreakPoint = { 0 };
		this->ReadWriteHardbp(m_conText, uAddress, type, dwLen);
		return hwBreakPoint;
	}

private:
	// 初始化硬件断点
	BOOL voidHardwareBreakP(const DWORD uAddress, CONTEXT* m_conText);
	// 设置硬件读写断点
	BOOL ReadWriteHardbp(CONTEXT* m_conText, DWORD uAddress, const WORD type, const WORD dwLen);
	// 保存硬断点的信息
	BREAKPOINT hwBreakPoint;

private:
	CONTEXT ct;

};

#endif