#pragma once
#ifndef _MASTEREXCEPTIONHANDLER_H_
#define _MASTEREXCEPTIONHANDLER_H_
#include <windows.h>
#include <vector>
#include <string>

using std::vector; using std::string;

class MasterExceptionHandler
{
public:
	MasterExceptionHandler();
	virtual ~MasterExceptionHandler();

// public  method interface
public:
	// 响应.exe拖拽消息公开接口
	void ResponsetoDragMessages(){ this->ResponsetoDragMessage(); }
	// 进程句柄
	static HANDLE m_hProcess;

// private function method
private:
	// 保存原数据结构体，Int3还原使用
	typedef struct _BREAKPOINT
	{
		DWORD addRess;
		BYTE oldData;
		char Type[20];
		bool operator ==(_BREAKPOINT obj)
		{
			return obj.addRess == this->addRess;
		}
	}BREAKPOINT, *PBREAKPOINT;


	// 响应.exe拖拽消息
	INT ResponsetoDragMessage();
	// 第一次判定处理异常
	DWORD OneofExceptionDispacth(const EXCEPTION_RECORD* ExceptionRecord);
	// 用户按键信息接收
	void UserInfoInput();
	// 设置单步循环
	void SetBreakPoint();
	// 单步步过
	void StepOverBp();
	// 设置int3中断0XCC
	void SetInt3BreakPoint(DWORD address);
	// 修改汇编代码
	void SetAssemblyCode(const DWORD address, const char* ModifyData);
	// 查看当前断点(软、硬)
	void ShowCurrBreakPoint();
	// 单步步入及单步步过（恢复0xCC）
	void PermanentBreakPoint(DWORD address);
	// 删除永久性断点
	void DeletePermanentBp(DWORD address);
	// 获取系统权限
	void GetSystempermissions();
	// 获取机器码长度
	short OpCodeLen(DWORD address);
	// 处理内存断点
	void MemoryExecuteBp();
	// 内存处理属性
	DWORD MemorydwSize = 0;
	// 初始化符号
	void InitParSymbol();
	// API转地址
	void ApiofAddress(char *ApiName);
	// 地址转API
	void AddressofApi(DWORD ApiAddress);
	// PEB隐藏
	void MasterExceptionHandler::PEBGo();
	// 设置条件断点
	void MasterExceptionHandler::SetCpbp(DWORD address, int Count);

// 数据属性
private:
	// 可执行文件路径信息
	char m_Executable[MAX_PATH];
	// 线程句柄
	HANDLE m_hThread = 0;
	// 线程环境快
	CONTEXT m_conText;
	// 设置异常处理标志结果
	DWORD m_dExceptionHandleFlag = DBG_CONTINUE;
	// 修改寄存器名字保存
	char regName[MAX_PATH];
	// 判断是不是第一次加载程序
	static BOOL IsOneLoadProcess;
	// 描述了一个调试事件
	DEBUG_EVENT DebugEvent;
	// 判断是否系统断点
	static BOOL m_SystemBreakPointFlag;
	// 导入导出表初始化标记
	BOOL IsInitTabFlag = TRUE;
	// 保存文件句柄
	HANDLE Filehandle = 0;
	// 加载基址
	HANDLE Imagebase = 0;
	// 判断是否是OEP
	BOOL OEPflag = TRUE;
	// 装载异常断点容器（int 3软断点）
	vector<BREAKPOINT>  m_SaveBreakPoint, m_SaveHardBp,m_SaverMemoryBp, m_SaveCpBp;
	// 只能下一个内存断点
	BOOL MemoryBpFlag = TRUE;
	// 屏蔽垃圾信息(内存输出)
	BOOL FlgasFlou = FALSE;
	// 启动地址保存
	DWORD m_StartAddr;
	// 条件断点判定变量
	INT nCount = 0;
	// 内存永久性判断
	static BOOL MemFlag;
	// 文件指针
	FILE *Fopen;
	// 保存行数指针
	static int nCountRow;
	// 条件断点属性
	BOOL IsCpBp = FALSE;
	// 实现硬件返回
	BOOL HardBpFlag = FALSE;
	// 创建线程结构体
	PROCESS_INFORMATION pRocessInfo;

};

#endif