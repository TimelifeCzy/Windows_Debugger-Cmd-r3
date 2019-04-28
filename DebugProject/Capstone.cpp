#include "Capstone.h"
#include "windows.h"
#include "iostream"

using std::cout; using std::endl;

Capstone::Capstone()
{

}

Capstone::~Capstone()
{

}

// 初始化反汇编引擎
void Capstone::InitCapstone()
{
	// 配置堆空间的回调函数
	OptMem.free = free;
	OptMem.calloc = calloc;
	OptMem.malloc = malloc;
	OptMem.realloc = realloc;
	OptMem.vsnprintf = (cs_vsnprintf_t)vsprintf_s;
	// 注册堆空间管理组函数
	cs_option(NULL, CS_OPT_MEM, (size_t)&OptMem);
	// 打开一个句柄
	cs_open(CS_ARCH_X86, CS_MODE_32, &Handle);
}

// 反汇编信息输出
void Capstone::ShowAssembly(const void* pAddr, int nLen)
{
	this->InitCapstone();
	// 接收OpCode大小 最大16保存机器指令			
	BYTE* pOpCode = (BYTE *)malloc(nLen * 16);
	memset(pOpCode, 0, (sizeof(BYTE) * 16 * nLen) );
	SIZE_T read = 0;			
	// 反汇编指定条数的语句
	// 用来读取指令位置内存的缓冲区信息
	cs_insn* ins = nullptr;

	// 读取指定长度的内存空间
	DWORD dwCount = 0;
	ReadProcessMemory(MasterExceptionHandler::m_hProcess, pAddr, pOpCode, nLen * 16, &dwCount);

	int count = cs_disasm(Handle, (uint8_t*)pOpCode, nLen * 16, (uint64_t)pAddr, 0, &ins);

	for (int i = 0; i < nLen; ++i)
	{
		printf("%08X\t", ins[i].address);
		for (uint16_t j = 0; j < 16; ++j)
		{
			if (j < ins[i].size)
				printf("%02X", ins[i].bytes[j]);
			else
				printf(" ");
		}
		printf("\t");
		printf("%s  ", ins[i].mnemonic);
		cout << ins[i].op_str << endl;  
	}
	printf("\n");
	// 释放动态分配的空间
	delete[] pOpCode;
	cs_free(ins, count);
}