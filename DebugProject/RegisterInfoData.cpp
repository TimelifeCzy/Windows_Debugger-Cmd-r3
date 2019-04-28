#include "RegisterInfoData.h"
#include <stdio.h>
#include "debugRegisters.h"


HANDLE RegisterData::hReThread = nullptr;

RegisterData::RegisterData()
{

}

RegisterData::~RegisterData()
{

}

// 显示寄存器信息
void RegisterData::RegisterInfo()
{
	// 获取当前线程寄存器状态
	ct.ContextFlags = CONTEXT_ALL;
	GetThreadContext(RegisterData::hReThread, &ct);
	// 输出寄存器
	printf("寄存器信息输出如下:\n");
	printf("EAX	%08X\t", ct.Eax);		// 1
	printf("ECX	%08X\t", ct.Ecx);		// 2
	printf("EDX	%08X\n", ct.Edx);		// 3
	printf("EBX	%08X\t", ct.Ebx);		// 4
	printf("ESP	%08X\t", ct.Esp);		// 5
	printf("EBP	%08X\n", ct.Ebp);		// 6
	printf("ESI	%08X\t", ct.Esi);		// 7
	printf("EDI	%08X\t", ct.Edi);		// 8
	printf("EIP	%08X\n", ct.Eip);		// 9
	// 标志位状态
	printf("标志寄存器状态如下:\n");
	EFLAGS *pEflage = (EFLAGS *)&ct.EFlags;
	printf("TF：%02X\t", pEflage->TF);	 // 10
	printf("IF：%02X\t", pEflage->IF);	 // 11
	printf("DF：%02X\n", pEflage->DF);	 // 12
	printf("CF：%02X\t", pEflage->CF);	 // 13
	printf("ZF：%02X\t", pEflage->ZF);	 // 14
	printf("PF：%02X\n", pEflage->PF);	 // 15 
	printf("SF：%02X\t", pEflage->SF);	 // 16
	printf("AF：%02X\t", pEflage->AF);	 // 17
	printf("OF：%02X\n", pEflage->OF);	 // 18
	printf("Dr7：%08X\n", ct.Dr7);	     // 22
	printf("Dr0：%08X\t", ct.Dr0);		 // 19
	printf("Dr1：%08X\n", ct.Dr1);	     // 20
	printf("Dr2：%08X\t", ct.Dr2);		 // 21
	printf("Dr3：%08X\n", ct.Dr3);	     // 22
}

// 修改存起信息
void RegisterData::RegisterInfo(int nRegisterFlag, DWORD dRegisterFlagedata)
{
	ct.ContextFlags = CONTEXT_ALL;
	GetThreadContext(RegisterData::hReThread, &ct);
	EFLAGS *pEflage = (EFLAGS *)&ct.EFlags;
	switch (nRegisterFlag)
	{
	case 2:
		ct.Eax = dRegisterFlagedata;
		break;
	case 3:
		ct.Ecx = dRegisterFlagedata;
		break;
	case 4:
		ct.Edx = dRegisterFlagedata;
		break;
	case 5:
		ct.Ebx = dRegisterFlagedata;
		break;
	case 6:
		ct.Esp = dRegisterFlagedata;
		break;
	case 7:
		ct.Ebp = dRegisterFlagedata;
		break;
	case 8:
		ct.Esi = dRegisterFlagedata;
		break;
	case 9:
		ct.Edi = dRegisterFlagedata;
		break;
	case 10:
		ct.Eip = dRegisterFlagedata;
		break;
	case 11:
		pEflage->TF = dRegisterFlagedata;
		break;
	case 12:
		pEflage->IF = dRegisterFlagedata;
		break;
	case 13:
		pEflage->DF = dRegisterFlagedata;
		break;
	case 14:
		pEflage->CF = dRegisterFlagedata;
		break;
	case 15:
		pEflage->ZF = dRegisterFlagedata;
		break;
	case 16:
		pEflage->PF = dRegisterFlagedata;
		break;
	case 17:
		pEflage->SF = dRegisterFlagedata;
		break;
	case 18:
		pEflage->AF = dRegisterFlagedata;
		break;
	case 19:
		pEflage->OF = dRegisterFlagedata;
		break;
	default:
		break;
	}
	// 更新到线上环境
	if (0 >= SetThreadContext(RegisterData::hReThread, &ct))
		printf("修改寄存器失败!\n");
	else
		printf("修改寄存器成功!\n");
}