#include "ImportTable.h"
#include "PeNtHeader.h"
#include "stdio.h"
#include <string>

using std::string;

PeNtHeader obj_Impentheader;

ImportTab::ImportTab()
{
	
}

ImportTab::~ImportTab()
{

}

// 初始化导入表
void ImportTab::InitImportTab()
{
	pImport = { 0 };
	pNtheaders = obj_Impentheader.puGetPeNtHeader();
	m_lpBase = obj_Impentheader.puGetLoadFileBaseAddress();
	// 获取Export头
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)pNtheaders;
	pData = (PIMAGE_DATA_DIRECTORY)&pNt->OptionalHeader.DataDirectory[1];
	if (pData->VirtualAddress == 0)
	{
		printf("该执行文件没有导入表");
		return;
	}
	m_lpBase = obj_Impentheader.puGetLoadFileBaseAddress();
	pImport = (PIMAGE_IMPORT_DESCRIPTOR)(pData->VirtualAddress + m_lpBase);
	// // BUG待修复pImport没有值？？ 可能跟不同对象调用有关
	pTunk = (PIMAGE_THUNK_DATA32)(pImport->FirstThunk + m_lpBase);
}

// 显示导入表
void ImportTab::ShowTmportTab()
{
	InitImportTab();
	if (pData->VirtualAddress == 0)
	{
		printf("该执行文件没有导入表");
		return;
	}
	while (pImport->Name != NULL)
	{
		printf("--------------导入表信息-------------------\n");
		printf("DLL名称: %s\t", (pImport->Name + m_lpBase));
		printf("Name的RVA: %08X\t", pImport->Name);
		printf("导入地址表的RVA: %08X\n", pImport->FirstThunk);
		printf("==============导入表函数具体信息============\n");
		char szGetName[200] = { 0 };
		while (pTunk->u1.Ordinal != 0)
		{
			if (!IMAGE_SNAP_BY_ORDINAL32(pTunk->u1.Ordinal))
			{
				szGetName[0] = 0;
				DWORD read = 0;
				ReadProcessMemory(m_process, (LPVOID)(m_lpBase + pTunk->u1.Ordinal),
					szGetName, 200, &read);
				PIMAGE_IMPORT_BY_NAME pName = (PIMAGE_IMPORT_BY_NAME)szGetName;
				printf("%s\t", pName->Name);
				printf("%08X", pTunk->u1.Ordinal);
			}
			else
			{
				printf("\t\t  %08X", pTunk->u1.Ordinal & 0xFFFF);
			}
			++pTunk;
		}
		++pImport;
	}
}