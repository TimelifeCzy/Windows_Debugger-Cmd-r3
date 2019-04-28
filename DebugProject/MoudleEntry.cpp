#include "ModuleEntry.h"
#include <TlHelp32.h>
#include <stdio.h>
#include <Psapi.h>
#include <iostream>

#pragma comment (lib, "Version.lib")

using std::wcout; using std::endl; using std::cout;

ModuleEntry::ModuleEntry()
{

}

ModuleEntry::~ModuleEntry()
{

}

BOOL ModuleEntry::ShowProcessModule()
{
	LPVOID VersionInfo = 0;
	HANDLE Moduhandle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, this->SaveMasterProessId);
	HANDLE processhandle = OpenProcess(PROCESS_ALL_ACCESS, GENERIC_READ, SaveMasterProessId);
	MODULEENTRY32 module32 = { sizeof(MODULEENTRY32) };
	Module32First(Moduhandle, &module32);
	// 获取模块入口
	MODULEINFO mi = { 0 };
	DWORD dwSize = 0;
	cout << "基址\t\t";	cout << "大小\t\t";	cout << "入口\t\t";
	cout << "名称\t\t\t";	// cout << "文件版本\t\t";	
	cout << "文件路径" << endl;
	// 支持宽字符中文输出
	_wsetlocale(LC_ALL, L"CHS");
	do{
		auto hModle = GetModuleHandle(module32.szExePath);
		// auto hModles = GetModuleHandle(module32.szModule);
		GetModuleInformation(processhandle, hModle, &mi, sizeof(MODULEINFO));
		// 基址
		printf("%08X\t", module32.modBaseAddr);
		// 大小
		printf("%08X\t", mi.SizeOfImage);
		// 入口
		printf("%08X\t", mi.lpBaseOfDll);
		// 名称
		wprintf(L"%s", module32.szModule);
		printf("\t\t   ");
		// 文件版本
		// LPVOID size = 0;
		// UINT nSize = 0;
		// dwSize = 0;
		// VersionInfobuf[0] = 0;
		// GetFileVersionInfo(module32.szExePath, NULL, 1024, VersionInfo);
		// strSubBlock.Format("\\StringFileInfo\\%s\\ProductVersion ", strTranslation);
		// VerQueryValue(pBlock, strSubBlock.GetBufferSetLength(256), (LPVOID*)&pVerValue, &nSize);
		// VerQueryValue(VersionInfo, L"\\StringFileInfo\\%s\\ProductVersion ", (LPVOID*)&VersionInfobuf, &nSize);
		//if (VersionInfo != 0)
		//	printf("%s\t", VersionInfobuf);
		// 路径
		wprintf(L"%s", module32.szExePath);
		printf("\n");
	} while (Module32Next(Moduhandle, &module32));
	CloseHandle(Moduhandle);
	return TRUE;
}