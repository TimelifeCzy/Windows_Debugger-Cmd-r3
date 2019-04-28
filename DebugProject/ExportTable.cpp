#include "ExportTable.h"
#include "PeNtHeader.h"
#include "stdio.h"

ExportTab::ExportTab()
{
}

ExportTab::~ExportTab()
{

}

// 获取导出表
PeNtHeader obj_pentheader;

// 初始化导出表
void ExportTab::InitExportTable()
{
	pNtheaders = obj_pentheader.puGetPeNtHeader();
	// 获取Export头
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)pNtheaders;
	pData = (PIMAGE_DATA_DIRECTORY)&pNt->OptionalHeader.DataDirectory[0];
	if (pData->VirtualAddress == 0)
		return;
	m_lpBase = obj_pentheader.puGetLoadFileBaseAddress();
	pExport = (PIMAGE_EXPORT_DIRECTORY)(pData->VirtualAddress + m_lpBase);
	// 1.地址表
	pFunAdd = (DWORD*)(pExport->AddressOfFunctions + m_lpBase);
	// 2.函数名称表
	pNameAddr = (DWORD*)(pExport->AddressOfNames + m_lpBase);
	// 3.序号表
	pOrdinalAddr = (DWORD*)(pExport->AddressOfNameOrdinals + m_lpBase);
}

// 显示导出表
void ExportTab::ShowExportTable()
{
	InitExportTable();
	if (pData->VirtualAddress == 0)
	{
		printf("该执行文件没有导出表\n");
		return;
	}
	printf("--------------导出表信息-------------------\n");
	printf("特征值: %08X\t", pExport->TimeDateStamp);
	printf("序号基址: %08X\t", pExport->Base);
	printf("名称: %08X\n", pExport->Base);
	printf("函数地址(RVA):\t   %08X\n", pExport->AddressOfFunctions);
	printf("函数名地地址(RVA): %08X\n", pExport->AddressOfNames);
	printf("函数序号地址(RVA): %08X\n", pExport->AddressOfNameOrdinals);
	printf("函数数量: %08X\t", pExport->NumberOfFunctions);
	printf("函数名称数量: %08X\n", pExport->NumberOfNames);
	printf("-------------------------------------------\n");
	DWORD dwCount = (DWORD)pExport->NumberOfFunctions;
	DWORD dwCounts = (DWORD)pExport->NumberOfNames;
	// 循环函数信息
	for (DWORD i = 0; i < dwCount; ++i)
	{
		if (0 == pFunAdd[i])
			continue;
		BOOL bFlag = FALSE;
		for (DWORD j = 0; j < dwCounts; j++)
		{
			bFlag = TRUE;
			DWORD dwNameRVA = pNameAddr[j];
			printf("函数序号：%08d\t", pOrdinalAddr[j]);
			printf("函数名：%s\n", (char *)(dwNameRVA + m_lpBase));
		}
		if (!bFlag) {
			printf("函数序号：%08d\t", (i + pExport->Base));
			printf("函数名：NULL\n");
		}
	}
}