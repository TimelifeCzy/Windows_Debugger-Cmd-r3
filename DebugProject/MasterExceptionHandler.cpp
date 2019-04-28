#include "MasterExceptionHandler.h"
#include "debugRegisters.h"
#include <iostream>
#include <stdio.h>
#include "Capstone.h"
#include "UKeystone.h"
#include "UWindows.h"
#include "CommandHelpDocument.h"
#include "RegisterInfoData.h"
#include "StackInfoData.h"
#include "MemoryInfoData.h"
#include "PeNtHeader.h"
#include "ExportTable.h"
#include "ImportTable.h"
#include "HardwareBreakPoint.h"
#include "MemoryBreakPoint.h"
#include "ModuleEntry.h"
#include "winternl.h"
#include "DbgHelp.h"

#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "ntdll.lib")

using std::cout; using std::endl; using std::cin; 
using std::wcout; using std::wcin;

BOOL MasterExceptionHandler::IsOneLoadProcess = TRUE;

BOOL MasterExceptionHandler::MemFlag = FALSE;

// 设置系统断点状态（第一次）
BOOL MasterExceptionHandler::m_SystemBreakPointFlag = TRUE;

int MasterExceptionHandler::nCountRow = 0;

#ifdef _DEBUG
// 输出错误DebugInfo
#define ERRORDEBUG_INFO(errorMsg) cout << "%s\n" << __FILE__ << "%s\n"  \
										<< __FUNCTION__ << "%s\n" << "%d\n" \
											<< __LINE__ << "%s\n" << errorMsg << endl;
#else
#define ERRORDEBUG_INFO()
#endif

HANDLE MasterExceptionHandler::m_hProcess = 0;

MasterExceptionHandler::MasterExceptionHandler()
{
}

MasterExceptionHandler::~MasterExceptionHandler()
{

}

// 调试异常主,
INT MasterExceptionHandler::ResponsetoDragMessage()
{
	// 获取权限
	this->GetSystempermissions();
	// 判断是不是第一次加载程序
	if (IsOneLoadProcess == TRUE){
		// 1. 等待Cmd接收路径输入
		cin >> m_Executable;
	}
	// 包含新创建的过程及其主要信息线程。
	pRocessInfo = { 0 };
	// 指定窗口,桌面,主窗口的外观标准处理,创建时间的过程.
	STARTUPINFOA sTartInfo = { sizeof(STARTUPINFOA) };
	// 打开文件
	Fopen = fopen("DUMP.txt", "a+");
	// 2. 打开.exe进程
	INT nRet = CreateProcessA(
		m_Executable,
		NULL,
		NULL,
		NULL,
		FALSE,
		DEBUG_ONLY_THIS_PROCESS | CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&sTartInfo, &pRocessInfo);
	if (0 >= nRet) {
		ERRORDEBUG_INFO("CreateProcessA() failuer!");
		exit(EXIT_FAILURE);
	}
	// pRocessInfo.hProcess = m_hProcess;
	// 3. 描述了一个调试事件
	DebugEvent = { 0 }; 
	// 4. 调试事件结构体
	while (1)
	{
		// 1. 等待
		WaitForDebugEvent(&DebugEvent, INFINITE);
		// 2. 打开进程、线程获取句柄
		m_dExceptionHandleFlag = DBG_CONTINUE;
		m_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, DebugEvent.dwProcessId);
		m_hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, DebugEvent.dwThreadId);
		// 3. 设置线上环境
		m_conText.ContextFlags = CONTEXT_ALL;
		// 4. 检索指定线程的上下文
		if (!GetThreadContext(m_hThread, &m_conText)) {
			ERRORDEBUG_INFO("GetThreadContext() failuer!");
		}
		// 5. 识别的代码调试事件的类型
		switch (DebugEvent.dwDebugEventCode)
		{
			// 5.1 异常事件
		case EXCEPTION_DEBUG_EVENT:
		{
			// 具体的异常记录处理(EXCEPTION_RECORD)
			m_dExceptionHandleFlag = OneofExceptionDispacth(&DebugEvent.u.Exception.ExceptionRecord);
		}
		break;
		case CREATE_THREAD_DEBUG_EVENT: cout << "线程创建事件" << endl; break;			// 线程创建事件
		case EXIT_THREAD_DEBUG_EVENT:   cout << "线程退出事件" << endl; break;			// 退出线程事件
		case CREATE_PROCESS_DEBUG_EVENT:
		{
			// 解析符号
			InitParSymbol();
			// 保存文件创建句柄（首次）
			this->Filehandle = DebugEvent.u.CreateProcessInfo.hFile;
			this->Imagebase = DebugEvent.u.CreateProcessInfo.lpBaseOfImage;
			fprintf(Fopen, "%d %s\n", nCountRow++, "进程创建事件: 保存被调试文件句柄及lpBaseOfImage(Code：124)");
			OEPflag = FALSE;
			// 保存入口点
			m_StartAddr = (DWORD)DebugEvent.u.CreateProcessInfo.lpStartAddress;
			fprintf(Fopen, "%d %s\n", nCountRow++, "进程创建事件: 保存被调试lpStartAddress(Code：129)");
			cout << "进程创建事件" << endl;												// 进程创建事件
			fflush(Fopen);
		}
		break;
		case EXIT_PROCESS_DEBUG_EVENT:	cout << "进程退出事件\n" << endl; break;			// 退出进程事件
		case LOAD_DLL_DEBUG_EVENT:
		{
			char ModuleFile[MAX_PATH] = { 0 };
			// 获取模块名字
			GetModuleFileNameA((HMODULE)DebugEvent.u.LoadDll.lpBaseOfDll, ModuleFile, MAX_PATH);
			// 初始化结构体信息
			SymLoadModule64(
				pRocessInfo.hProcess,
				DebugEvent.u.CreateProcessInfo.hFile,
				ModuleFile,
				NULL,
				(DWORD64)DebugEvent.u.LoadDll.lpBaseOfDll,
				0
				);
			cout << "DLL加载事件" << endl;
		}	
		break;			// 映射DLL事件
		case UNLOAD_DLL_DEBUG_EVENT:    cout << "DLL卸载事件" << endl; break;			// 卸载DLL事件 
		case OUTPUT_DEBUG_STRING_EVENT: cout << "调试字符串输出事件" << endl; break;		// 调试字符串输出事件
		case RIP_EVENT:
		default:
			break;
		}
		// 返回之前设置线程上下文
		if (!SetThreadContext(m_hThread, &m_conText)) {
			ERRORDEBUG_INFO("SetThreadContext() failuer!");
		}
		CloseHandle(m_hProcess);
		CloseHandle(m_hThread);
		// 回复调试子系统
		ContinueDebugEvent(
			DebugEvent.dwProcessId,
			DebugEvent.dwThreadId,
			m_dExceptionHandleFlag);
	}
	// 关闭文件指针
	fclose(Fopen);
	return 0;
}

// 单步步入
void MasterExceptionHandler::SetBreakPoint()
{
	// IF是外部可屏蔽中断，中断不用管IF标志位
	// 1. 单步中断属于 陷阱异常TF = 1
	EFLAGS *pEflage = (EFLAGS *)&m_conText.EFlags;
	pEflage->TF = 1;
}

// 单步步过
void MasterExceptionHandler::StepOverBp()
{
	BYTE OpCodeBuf[16] = { 0 };
	DWORD processAdd = (DWORD)DebugEvent.u.Exception.ExceptionRecord.ExceptionAddress;
	DWORD read = 0;
	ReadProcessMemory(m_hProcess, (LPVOID)processAdd, OpCodeBuf, sizeof(OpCodeBuf), &read);
	
	if ((0xE8 == OpCodeBuf[0]) || (0xF3 == OpCodeBuf[0]))
	{
		m_conText.Eip += OpCodeLen(m_conText.Eip);
		SetInt3BreakPoint(m_conText.Eip);
	}
	else
		SetBreakPoint();
	SetThreadContext(m_hThread, &m_conText);
}

// 获取机器码长度
short MasterExceptionHandler::OpCodeLen(DWORD address)
{
	csh Handle;
	// 打开一个句柄
	cs_open(CS_ARCH_X86, CS_MODE_32, &Handle);
	// 设置缓冲区
	BYTE* OpCode = (BYTE *)malloc(16);
	memset(OpCode, 0, sizeof(BYTE) * 16);
	cs_insn* ins = nullptr;
	DWORD dwSize = 0;
	// 读当前指令数据
	ReadProcessMemory(m_hProcess, (LPCVOID)address, OpCode, 16, &dwSize);
	// int count = cs_disasm(Handle, (uint8_t*)pOpCode, nLen * 16, (uint64_t)pAddr, 0, &ins);
	int count = cs_disasm(Handle, (uint8_t *)OpCode, 16, (uint64_t)m_conText.Eip, 0, &ins);
	// 返回指令长度大小
	return ins->size;
}

// 设置Int3断点
void MasterExceptionHandler::SetInt3BreakPoint(DWORD address)
{
	size_t Flag = 0;
	// 保存原来数据（备份）
	BREAKPOINT BreakPoint = { 0 };
	SIZE_T read = 0;
	int ErrorCode = 0;
	BreakPoint.addRess = address;
	ErrorCode = ReadProcessMemory(m_hProcess, (LPVOID)address, &BreakPoint.oldData, 1, &read);
	if (0 >= ErrorCode) {
		ERRORDEBUG_INFO("Int3 ReadProceeMemory() faliuer!");
	}
	ErrorCode = WriteProcessMemory(m_hProcess, (LPVOID)address, "\xCC", 1, &read);
	if (0 >= ErrorCode) {
		ERRORDEBUG_INFO("Int3 WriteProcessMemory() failuer!");
	}
	if (this->OEPflag == FALSE)
		strcpy(BreakPoint.Type, "OEP");
	else
		strcpy(BreakPoint.Type, "软断点（int 3）");
	if (0 == m_SaveBreakPoint.size())
		m_SaveBreakPoint.push_back(BreakPoint);
	// 做个判断如果当前容器有这个地址则不添加
	for (size_t i = 0; i < m_SaveBreakPoint.size(); ++i)
	{
		if (BreakPoint.addRess != m_SaveBreakPoint[i].addRess)
			Flag += 1;
	}
	if (Flag == m_SaveBreakPoint.size())
	{
		m_SaveBreakPoint.push_back(BreakPoint);
		fprintf(Fopen, "%d %s\n", nCountRow++, "Int 3软断点成功: 成功(Code：237)");
		fflush(Fopen);
	}
}

// 设置汇编代码
void MasterExceptionHandler::SetAssemblyCode(const DWORD address, const char* ModifyData)
{
	// 获取当前地址的OPECODE
	KeysTone obj_Keystone;
	obj_Keystone.puSetBpHandle(m_hProcess);
	if (address == 0)
	{
		obj_Keystone.puAttData();
		printf("恢复汇编指令成功成功!\n");
	}
	else
	{
		BOOL ErrorCode = obj_Keystone.puModifyOpenCode(m_hThread, address, ModifyData);
		if (0 >= ErrorCode) {
			ERRORDEBUG_INFO("Modify Assembly Data failuer!");
			printf("Alter of Assembly Sucess!\n");
			return;
		}
	}

}

// 修改寄存器判断
int SwitchModifyRegister(char *reg)
{	
	if (NULL == reg)
		return 0;
	else if (0 == _stricmp("EAX", reg))
		return 2; 
	else if (0 == _stricmp("ECX", reg))
		return 3;
	else if (0 == _stricmp("EDX", reg))
		return 4; 
	else if (0 == _stricmp("EBX", reg))
		return 5; 
	else if (0 == _stricmp("ESP", reg))
		return 6; 
	else if (0 == _stricmp("EBP", reg))
		return 7;
	else if (0 == _stricmp("ESI", reg))
		return 8; 
	else if (0 == _stricmp("EDI", reg))
		return 9;
	else if (0 == _stricmp("EIP", reg))
		return 10;
	else if (0 == _stricmp("TF", reg))
		return 11;
	else if (0 == _stricmp("IF", reg))
		return 12;
	else if (0 == _stricmp("DF", reg))
		return 13;
	else if (0 == _stricmp("CF", reg))
		return 14;
	else if (0 == _stricmp("ZF", reg))
		return 15;
	else if (0 == _stricmp("PF", reg))
		return 16;
	else if (0 == _stricmp("SF", reg))
		return 17;
	else if (0 == _stricmp("AF", reg))
		return 18;
	else if (0 == _stricmp("OF", reg))
		return 19;
	return 0;
}

// 查看当前断点(软、硬)
void MasterExceptionHandler::ShowCurrBreakPoint()
{
	cout << "断点地址    ";
	cout << "数据项\t\t";
	cout << "断点类型" << endl;
	for (size_t i = 0; i < m_SaveBreakPoint.size(); ++i)
	{
		printf("%08X\t", m_SaveBreakPoint[i].addRess);
		wcout << m_SaveBreakPoint[i].oldData << "\t\t\t";
		wcout << m_SaveBreakPoint[i].Type << endl;
	}
	for (size_t i = 0; i < m_SaveHardBp.size(); ++i)
	{
		printf("%08X\t", m_SaveHardBp[i].addRess);
		wcout << m_SaveHardBp[i].oldData << "\t\t\t";
		wcout << m_SaveHardBp[i].Type << endl;
	}
	for (size_t i = 0; i < m_SaverMemoryBp.size(); ++i)
	{
		printf("%08X\t", m_SaverMemoryBp[i].addRess);
		wcout << m_SaverMemoryBp[i].oldData << "\t\t\t";
		wcout << m_SaverMemoryBp[i].Type << endl;
	}
	for (size_t i = 0; i < m_SaveCpBp.size(); ++i)
	{
		printf("%08X\t", m_SaveCpBp[i].addRess);
		wcout << m_SaveCpBp[i].oldData << "\t\t\t";
		wcout << m_SaveCpBp[i].Type << endl;
	}
}

// 永久性断点设置
void MasterExceptionHandler::PermanentBreakPoint(DWORD address)
{
	for (size_t i = 0; i < m_SaveBreakPoint.size(); ++i)
	{
		// 如果当前地址小于断点地址代表还没有运行到断点处 无须重置
		SetInt3BreakPoint(m_SaveBreakPoint[i].addRess);
	}
	for (size_t i = 0; i < m_SaveHardBp.size(); ++i)
	{
		m_conText.Dr7 = 1;
	}
}

// 删除永久性断点
void MasterExceptionHandler::DeletePermanentBp(DWORD address)
{
	SIZE_T write = 0;
	// 恢复Int3断点
	for (size_t i = 0; i < m_SaveBreakPoint.size(); ++i)
	{
		if (address == m_SaveBreakPoint[i].addRess)
		{
			// 恢复0xCC断点 没有恢复成功
			WriteProcessMemory(m_hProcess, &address, &m_SaveBreakPoint[i].oldData, 1, &write);
			// 删除永久性断点断点
			auto index = find(m_SaveBreakPoint.begin(), m_SaveBreakPoint.end(), m_SaveBreakPoint[i]);
			m_SaveBreakPoint.erase(index);
			cout << "删除永久性断点成功" << endl;
			fprintf(Fopen, "%d %s\n", nCountRow++, "删除永久性断点成功: 成功(Code：374)");
			break;
		}
	}
	// 硬断点
	for (size_t i = 0; i < m_SaveHardBp.size(); ++i)
	{
		if (address == m_SaveHardBp[i].addRess)
		{
			m_conText.Dr7 = 0;
			// 删除永久性断点断点
			auto index = find(m_SaveHardBp.begin(), m_SaveHardBp.end(), m_SaveHardBp[i]);
			m_SaveHardBp.erase(index);
			cout << "删除永久性硬断点成功" << endl;
			fprintf(Fopen, "%d %s\n", nCountRow++, "删除永久性硬断点成功: 成功(Code：388)");
			break;
		}
	}
	// 内存断点
	for (size_t i = 0; i < m_SaverMemoryBp.size(); ++i)
	{
		if (address == m_SaverMemoryBp[i].addRess)
		{
			// 恢复内存页面的原来属性
			WriteProcessMemory(m_hProcess, &address, &m_SaverMemoryBp[i].oldData, 1, &write);
			// 删除永久性断点断点
			auto index = find(m_SaverMemoryBp.begin(), m_SaverMemoryBp.end(), m_SaverMemoryBp[i]);
			m_SaverMemoryBp.erase(index);
			cout << "删除永久性内存断点点成功" << endl;
			fprintf(Fopen, "%d %s\n", nCountRow++, "删除永久性内存断点点成功: 成功(Code：403)");
			break;
		}
	}
	fflush(Fopen);
}

// 用户按键信息接收
void MasterExceptionHandler::UserInfoInput()
{
	// 第一个字节初始化0，初始化数组
	char cmd[10] = { 0 };
	regName[0] = 0;
	// 保存修改的数值
	DWORD NewRegData = 0;
	// 循环接收信息
	while (true)
	{
		cout << "命令输入>> ";
		cin >> cmd;
		// 设置单步且离开循环
		if (0 == _stricmp(cmd, "t")) {
			SetBreakPoint();
			HardBpFlag = TRUE;
			IsCpBp = TRUE;
			FlgasFlou = TRUE;
			break;
		}
		// 单步步过
		else if (0 == _stricmp(cmd, "s")) {
			StepOverBp();
			FlgasFlou = TRUE;
			IsCpBp = TRUE;
			HardBpFlag = TRUE;
			break;
		}
		// 重新加载程序
		else if (0 == _stricmp(cmd, "reload")) {
			CloseHandle(m_hProcess);
			CloseHandle(m_hThread);
			MasterExceptionHandler::IsOneLoadProcess = FALSE;
			MasterExceptionHandler::m_SystemBreakPointFlag = TRUE;
			cout << "重新加载可执行程序......." << endl;
			Sleep(500);
			ResponsetoDragMessage();
			FlgasFlou = TRUE;
			break;
		}
		else if (0 == _stricmp(cmd, "oep")) {
			OEPflag = FALSE;
			SetInt3BreakPoint(m_StartAddr);
			break;
		}
		// 清屏
		else if (0 == _stricmp(cmd, "cls")) {
			system("cls");
		}
		// 设置软断点
		else if (0 == _stricmp(cmd, "bp")) {
			OEPflag = TRUE;
			DWORD addRes = 0;
			scanf_s("%x", &addRes);
			SetInt3BreakPoint(addRes);
		}
		// 可执行硬断点
		else if (0 == _stricmp(cmd, "hwxbp")) {
			IsCpBp = FALSE;
			HardBpFlag = TRUE;
			char str[10] = { 0 };
			cin >> str;
			DWORD hwBpAddress = strtoul(str, NULL, 16);
			HardwareBreakP obj_hardBp;
			HardwareBreakP::hHardThread = this->m_hThread;
			auto BreakPoints = obj_hardBp.puInitHardwareBreakP(hwBpAddress, &m_conText);
			BREAKPOINT Temp = { 0 };
			Temp.addRess = BreakPoints.addRess;
			Temp.oldData = BreakPoints.oldData;
			strcpy(Temp.Type, BreakPoints.Type);
			m_SaveHardBp.push_back(Temp);
		}
		// 读写硬断点
		else if (0 == _stricmp(cmd, "hwrwbp")){
			HardBpFlag = TRUE;
			IsCpBp = FALSE;
			DWORD hwBpAddress = 0;
			WORD hwBpType = 0;
			WORD hwBpLen = 0;
			char str[10] = { 0 };
			cin >> str;
			hwBpAddress = strtoul(str, NULL, 16);
			cin >> hwBpType; cin >> hwBpLen;
			HardwareBreakP obj_hardBp;
			HardwareBreakP::hHardThread = this->m_hThread;
			auto Bpreturn = obj_hardBp.puReadWriteHardbp(&m_conText, hwBpAddress, hwBpType, hwBpLen);
			BREAKPOINT Temp1 = { 0 };
			Temp1.addRess = Bpreturn.addRess;
			Temp1.oldData = Bpreturn.oldData;
			strcpy(Temp1.Type, Bpreturn.Type);
			m_SaveHardBp.push_back(Temp1);
		}
		// 设置内存断点
		else if (0 == _stricmp(cmd, "memorybp")) {
			if (MemoryBpFlag)
			{
				MemoryBp obj_memorybp;
				obj_memorybp.puSetHadnle(this->m_hProcess);
				DWORD Addr = 0;
				scanf_s("%x", &Addr);
				auto Bp = obj_memorybp.puSetMeoryBp(Addr, 0, &MemFlag);
				BREAKPOINT Temp1 = { 0 };
				Temp1.addRess = Bp.addRess;
				Temp1.oldData = Bp.oldData;
				strcpy(Temp1.Type, Bp.Type);
				m_SaverMemoryBp.push_back(Temp1);
				MemoryBpFlag = FALSE;
			}
			else{
				cout << "只能下一个内存断点,请先删除已有内存断点" << endl;
			}
		}
		// 条件断点判断
		else if (0 == _stricmp(cmd, "cdit")) {
			IsCpBp = TRUE;
			// 保证为
			HardBpFlag = FALSE;
			DWORD Address = 0;
			char Temp[10] = { 0 };
			char Reg[10] = { 0 };
			cin >> Temp;
			Address = strtoul(Temp, NULL, 16);
			cin >> Reg;
			cin >> nCount;
			SetCpbp(Address, nCount);
		}
		// 查看当前断点
		else if (0 == _stricmp(cmd, "bl")) {
			ShowCurrBreakPoint();
		}
		// 删除永久性断点
		else if (0 == _stricmp(cmd, "dbp")) {
			DWORD InputAddress = 0;
			scanf("%x", &InputAddress);
			DeletePermanentBp(InputAddress);
		}
		// 运行程序（或者运行至下一个断点）
		else if (0 == _stricmp(cmd, "g")) {
			FlgasFlou = TRUE;
			break;
		}
		// 修改汇编代码
		else if (0 == _stricmp(cmd, "md")) {
			char Address[10];
			char AssemblyData[] = { 0 };
			cin >> Address;
			DWORD dMemoryPath = strtoul(Address, NULL, 16);
			getchar();
			scanf("%[^\n]", AssemblyData);
			SetAssemblyCode(dMemoryPath, AssemblyData);
		}
		// 帮助文档查看
		else if (0 == _stricmp(cmd, "help")) {
			ComHelpDocument obj_Comhelpdct;
			obj_Comhelpdct.puShowComandHelp();
		}
		// 寄存器查看
		else if (0 == _stricmp(cmd, "r")) {
			// 初始化对象
			RegisterData obj_register;
			RegisterData::hReThread = m_hThread;
			obj_register.RegisterInfo();
		}
		// 修改寄存器
		else if (0 == _stricmp(cmd, "rmodify")) {
			cin >> regName;
			scanf("%x", &NewRegData);
			RegisterData obj_register;
			RegisterData::hReThread = m_hThread;
			int nFlag = SwitchModifyRegister(regName);
			if (nFlag > 0)
				obj_register.RegisterInfo(nFlag, NewRegData);
			else
				cout << "请输入正确的寄存器（注意:名称全大写）" << endl;
		}
		// 查看汇编指令
		else if (0 == _stricmp(cmd, "uu")) {
			Capstone obj_capstone;
			obj_capstone.ShowAssembly((void *)m_conText.Eip, 10);
		}
		// 查看内存地址汇编
		else if (0 == _stricmp(cmd, "u")) {
			DWORD Address = 0;
			Capstone obj_capstone;
			scanf("%x", &Address);
			obj_capstone.ShowAssembly((void *)Address, 10);
		}
		// 查看模块数据
		else if (0 == _stricmp(cmd, "ml")) {
			ModuleEntry obj_module;
			obj_module.SetProcessHandle(DebugEvent.dwProcessId);
			obj_module.puShowProcessModule();
		}
		// 显示栈信息
		else if (0 == _stricmp(cmd, "stack")) {
			StackData obj_stack;
			StackData::hStackThread = this->m_hThread;
			StackData::hStackProcess = this->m_hProcess;
			obj_stack.puShowStackInfoData();
		}
		// 显示内存信息
		else if (0 == _stricmp(cmd, "d")) {
			char Address[10] = { 0 };
			cin >> Address;
			DWORD MemoryAddress = strtoul(Address, NULL, 16);
			MemoryData obj_memory;
			MemoryData::hMemThread = this->m_hThread;
			MemoryData::hMemProcess = this->m_hProcess;
			obj_memory.MemoryDatas(MemoryAddress);
		}
		// 显示默认寄存器Eip内存数据
		else if (0 == _stricmp(cmd, "dd")) {
			MemoryData obj_memory;
			MemoryData::hMemThread = this->m_hThread;
			MemoryData::hMemProcess = this->m_hProcess;
			obj_memory.MemoryDatas();
		}
		// 修改内存信息
		else if (0 == _stricmp(cmd, "dmodify")) {
			char Temp1[8] = { 0 };
			char MemoryModifyData[MAX_PATH] = { 0 };
			cin >> Temp1;
			cin >> MemoryModifyData;
			DWORD MemoryAddress = strtoul(Temp1, NULL, 16);
			DWORD ModifyData = strtoul(MemoryModifyData, NULL, 16);
			// DWORD64 MemoryModifyData = strtoul(Temp, NULL, 16);
			MemoryData obj_memory;
			MemoryData::hMemThread = this->m_hThread;
			MemoryData::hMemProcess = this->m_hProcess;
			obj_memory.MemoryDatas(MemoryAddress, ModifyData);
		}
		// 查看导出表与导入表
		else if ((0 == _stricmp(cmd, "export")) || (0 == _stricmp(cmd, "import")))
		{
			if (TRUE == IsInitTabFlag)
			{
				IsInitTabFlag = FALSE;
				PeNtHeader obj_pentheader;
				PeNtHeader::hFilehand = Filehandle;
				PeNtHeader::Imagebase = this->Imagebase;
				obj_pentheader.puSetProcessHeader(MasterExceptionHandler::m_hProcess);
				obj_pentheader.puLoadFile((DWORD)DebugEvent.u.CreateProcessInfo.lpBaseOfImage);
			}
			if (0 == _stricmp(cmd, "export")) {
				ExportTab obj_export;
				obj_export.puShowExportTable();
			}
			if (0 == _stricmp(cmd, "import")) {
				ImportTab obj_import;
				obj_import.puSetProcess(this->m_hProcess);
				obj_import.puShowTmportTab();
			}
		}
		// 符号解析及API查询
		else if (0 == _stricmp(cmd, "findapi")) {
			// InitParSymbol();
			char ApiName[MAX_PATH] = { 0 };
			printf("请输入查询的API：");
			cin >> ApiName;
			ApiofAddress(ApiName);
		}
		// 函数地址
		else if (0 == _stricmp(cmd, "findaddress")) {
			//InitParSymbol();
			DWORD address = 0;
			printf("请输入查询的API：");
			scanf("%x", &address);
			AddressofApi(address);
		}
	}
}

// 第一次异常事件分发
DWORD MasterExceptionHandler::OneofExceptionDispacth(const EXCEPTION_RECORD* ExceptionRecord)
{
	// 条件断点设置回去
	if (IsCpBp)
	{
		DWORD write = 0;
		IsCpBp = FALSE;
		for (auto&i : m_SaveCpBp)
		{
			WriteProcessMemory(m_hProcess, (PVOID)i.addRess,
				"\xCC", 1, &write);
		}
	}
	// 设置永久性断点
	PermanentBreakPoint(m_conText.Eip);

	// 创建对象
	Capstone capstone;
	// 设置断点表
	BOOL BreakPointFlag = FALSE;
	// 1. 设置为异常标记未处理
	DWORD dwRet = DBG_EXCEPTION_NOT_HANDLED;
	// 3. 判断EXCEPTION_DEBUG_EVENT事件具体什么类型异常
	switch (ExceptionRecord->ExceptionCode)
	{
		// 3.1 异常处理（故障/陷阱）
	case EXCEPTION_BREAKPOINT:
	{
		// 过滤掉PEB
		PEBGo();
		if (TRUE == MasterExceptionHandler::m_SystemBreakPointFlag) {
			// 进来之后就不在是第一次
			MasterExceptionHandler::m_SystemBreakPointFlag = FALSE;
			cout << "故障/陷阱：系统断点\n" << endl;
			dwRet = DBG_CONTINUE;
			fprintf(Fopen, "%d %s\n", nCountRow++, "系统断点事件: 成功(Code：673)");
			break;
		}
		for (auto&i : m_SaveBreakPoint)
		{
			// 处理异常
			SIZE_T write = 0;
			if (i.addRess == (DWORD)ExceptionRecord->ExceptionAddress) {
				// 把0xCC修复回去
				if (!WriteProcessMemory(m_hProcess, (void *)i.addRess, &i.oldData, 1, &write)) {
					ERRORDEBUG_INFO("WriteProcessMemory() failuer!");
					fprintf(Fopen, "%d %s\n", nCountRow++, "修复Int 3错误异常: 写入失败(Code：683)");
				}
				// 设置单步断点
				SetBreakPoint();
				// 陷阱异常 
				m_conText.Eip--;
				BreakPointFlag = TRUE;
				dwRet = DBG_CONTINUE;
			}
		}
		// 判断条件断点
		for (auto&i : m_SaveCpBp)
		{
			if (i.addRess = (DWORD)ExceptionRecord->ExceptionAddress)
			{
				// HardBpFlag = TRUE;
				dwRet = DBG_CONTINUE;
				SIZE_T write = 0;
				// 设置单步
				SetBreakPoint();
				IsCpBp = TRUE;
				// 恢复
				WriteProcessMemory(m_hProcess, (PVOID)i.addRess,
					&i.oldData, 1, &write);
				m_conText.Eip--;
				// isDbgBreakPoint = TRUE;
				if (m_conText.Eax == this->nCount)
				{
					BreakPointFlag = DBG_CONTINUE;
					FlgasFlou = TRUE;
					HardBpFlag = FALSE;
					break;
				}
				else
				{
					return DBG_CONTINUE;
				}
			}

		}
		if (!BreakPointFlag) {
			dwRet = DBG_EXCEPTION_NOT_HANDLED;
		}
	}
	break;
	// 处理内存断点
	case EXCEPTION_ACCESS_VIOLATION:
	{
		MemoryExecuteBp();
		dwRet = DBG_CONTINUE;
	}
	break;
	case EXCEPTION_SINGLE_STEP:
		// 恢复硬断点
		dwRet = DBG_CONTINUE;
		for (auto&i : m_SaveHardBp)
		{
			// HardBpFlag = FALSE;
			if (i.addRess == (DWORD)ExceptionRecord->ExceptionAddress) {
				m_conText.Dr7 = 0;
				SetThreadContext(m_hThread, &m_conText);
				// 设置单步断点
				SetBreakPoint();
				HardBpFlag = TRUE;
				break;
			}
		}
		// 硬件读写断点判断
		if (HardBpFlag)
			break;
		// 条件断点判断
		if (!IsCpBp)
			return dwRet;
		else
			break;
	default:
		dwRet = DBG_EXCEPTION_NOT_HANDLED;
		break;
	}
	if (FlgasFlou = TRUE) {
		cout << "异常事件" << endl;
		printf("Eip : %08X\n", m_conText.Eip);
		// 输出反汇编信息
		capstone.ShowAssembly((void *)m_conText.Eip, 6);
	}
	UserInfoInput();
	fflush(Fopen);
	return dwRet;
}

// 获取系统权限
void MasterExceptionHandler::GetSystempermissions()
{
	HANDLE hToken = NULL;
	// 获取伪句柄（该线程）
	HANDLE hProcess = GetCurrentProcess();
	OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	TOKEN_PRIVILEGES tp = { 0 };
	LookupPrivilegeValue(0, SE_SHUTDOWN_NAME, &tp.Privileges[0].Luid);
	// 特权计数
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
}

// 内存断点处理
void MasterExceptionHandler::MemoryExecuteBp()
{
	FlgasFlou = FALSE;
	Capstone capstone;
	MemoryBp obj_Memory;
	// 第二次循环才进入到函数 判断是否命中
	for (size_t i = 0; i < m_SaverMemoryBp.size(); ++i)
	{
		// 读时异常
		if (DebugEvent.u.Exception.ExceptionRecord.ExceptionInformation[0] == 0) {
			if (DebugEvent.u.Exception.ExceptionRecord.ExceptionInformation[1] == m_SaverMemoryBp[i].addRess){
				// 恢复异常断点
				VirtualProtect(&m_SaverMemoryBp[i].addRess, 1, m_SaverMemoryBp[i].oldData, &MemorydwSize);
				// 输出反汇编信息
				capstone.ShowAssembly((void *)m_conText.Eip, 6);
				FlgasFlou = TRUE;
				// 设置单步断点
				SetBreakPoint();
				break;
			}
		}
		// 写时异常
		if (DebugEvent.u.Exception.ExceptionRecord.ExceptionInformation[0] == 1) {
			if (DebugEvent.u.Exception.ExceptionRecord.ExceptionInformation[1] == m_SaverMemoryBp[i].addRess) {
				// 恢复异常断点
				VirtualProtect(&m_SaverMemoryBp[i].addRess, 1, m_SaverMemoryBp[i].oldData, &MemorydwSize);
				// 输出反汇编信息
				capstone.ShowAssembly((void *)m_conText.Eip, 6);
				FlgasFlou = TRUE;
				// 设置单步断点
				SetBreakPoint();
				break;
			}
		}
		// 执行异常
		if (DebugEvent.u.Exception.ExceptionRecord.ExceptionInformation[0] == 8) {
			if (DebugEvent.u.Exception.ExceptionRecord.ExceptionInformation[1] == m_SaverMemoryBp[i].addRess) {
				// 恢复异常断点
				VirtualProtect(&m_SaverMemoryBp[i].addRess, 1, m_SaverMemoryBp[i].oldData, &MemorydwSize);
				// 输出反汇编信息
				capstone.ShowAssembly((void *)m_conText.Eip, 6);
				FlgasFlou = TRUE;
				// 设置单步断点
				SetBreakPoint();
				break;
			}
		}
	}
	// 不论怎样，都别忘了恢复内存页属性！
	obj_Memory.puSetHadnle(m_hProcess);
	obj_Memory.puRestorMempryPro(&MemFlag);
}

/*符号解析*/
// 初始化调试符号
void MasterExceptionHandler::InitParSymbol()
{
	char ModuleFile[MAX_PATH] = { 0 };
	// 初始化符号
	SymInitialize(m_hProcess, NULL, FALSE);
	// GetModuleHandle()
}

// API转地址
void MasterExceptionHandler::ApiofAddress(char *ApiName)
{
	char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
	PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
	pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	pSymbol->MaxNameLen = MAX_SYM_NAME;
	// 根据名字查询
	if (!SymFromName(pRocessInfo.hProcess, ApiName, pSymbol))
		return;
	printf("函数地址： %U\n", pSymbol->Address);
}

// 地址转API
void MasterExceptionHandler::AddressofApi(DWORD ApiAddress)
{
	DWORD64 dwDisplacement = 0;
	char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
	PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
	pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	pSymbol->MaxNameLen = MAX_SYM_NAME;
	// 根据地址获取符号信息
	if (!SymFromAddr(m_hProcess, ApiAddress, &dwDisplacement, pSymbol))
		return;
	printf("函数名称: %s\n", pSymbol->Name);
}

// PEB隐藏
void MasterExceptionHandler::PEBGo()
{
	// 反反调试PEB(获取目标进程各种信息)
	PROCESS_BASIC_INFORMATION hProcessDbug = { 0 };
	HANDLE dwProces = OpenProcess(PROCESS_ALL_ACCESS, FALSE, DebugEvent.dwProcessId);
	// The PebBaseAddress member points to a PEB structure.
	NtQueryInformationProcess(dwProces, ProcessBasicInformation, &hProcessDbug, sizeof(hProcessDbug), NULL);
	// 读取PEB地址中内存信息
	DWORD dwSize = 0;
	PEB peb = { 0 };
	ReadProcessMemory(dwProces, hProcessDbug.PebBaseAddress, &peb, sizeof(PEB), &dwSize);
	// 拿到了PEB地址 把调试状态置位0
	peb.BeingDebugged = 0;
	WriteProcessMemory(dwProces, hProcessDbug.PebBaseAddress, &peb, sizeof(PEB), &dwSize);
	cout << "PEB已经隐藏" << endl;
}

// 设置条件断点int 3
void MasterExceptionHandler::SetCpbp(DWORD address, int Count)
{
	this->nCount = Count;
	size_t Flag = 0;
	// 保存原来数据（备份）
	BREAKPOINT BreakPoint = { 0 };
	SIZE_T read = 0;
	int ErrorCode = 0;
	BreakPoint.addRess = address;
	ErrorCode = ReadProcessMemory(m_hProcess, (LPVOID)address, &BreakPoint.oldData, 1, &read);
	if (0 >= ErrorCode) {
		ERRORDEBUG_INFO("条件断点 ReadProceeMemory() faliuer!");
	}
	ErrorCode = WriteProcessMemory(m_hProcess, (LPVOID)address, "\xCC", 1, &read);
	if (0 >= ErrorCode) {
		ERRORDEBUG_INFO("条件断点 WriteProcessMemory() failuer!");
	}
	strcpy(BreakPoint.Type, "条件断点");
	m_SaveCpBp.push_back(BreakPoint);
	fprintf(Fopen, "%d %s\n", nCountRow++, "条件断点成功: 成功(Code：921)");
	fflush(Fopen);
	IsCpBp = FALSE;
}
