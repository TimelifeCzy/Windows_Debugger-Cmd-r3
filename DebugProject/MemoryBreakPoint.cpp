#include "MemoryBreakPoint.h"
#include "debugRegisters.h"

HANDLE MemoryBp::ProcessHandle = 0;
DWORD MemoryBp::BpMemoryAddress = 0;
DWORD MemoryBp::m_dwAtt = 0;

MemoryBp::MemoryBp()
{

}

MemoryBp::~MemoryBp()
{

}

// 初始化内存断点
BOOL MemoryBp::InitMemoryBp()
{
	excepRecord = { 0 };
	return 0;
}

// 设置内存断点
BOOL MemoryBp::SetrwxMemoryBp(DWORD Address, BYTE Type)
{
	// 保存的是异常访问时候的数据 共14
	// 0 读异常  1 写异常		8 执行异常 
	// typedef _W64 unsigned long ULONG_PTR, *PULONG_PTR;
	char buf[MAX_PATH] = { 0 };
	DWORD dwSize = 0;
	// 设置异常断点
	VirtualProtectEx(MasterBreakBpHandle, (LPVOID)Address, 4, PAGE_NOACCESS, &m_dwAtt);
	BpMemoryAddress = Address;
	breakPoint.addRess = Address;
	breakPoint.oldData = m_dwAtt;
	strcpy(breakPoint.Type, "内存断点");
	return TRUE;
}

BOOL MemoryBp::RestoreMemoryPropert()
{
	DWORD dwAtter;
	// 恢复当前页面内存属性
	auto ErrorCode = VirtualProtectEx(MasterBreakBpHandle, (LPVOID)BpMemoryAddress, 4, m_dwAtt, &dwAtter);
	if (ErrorCode > 0)
		return TRUE;
	else
		return FALSE;
}

