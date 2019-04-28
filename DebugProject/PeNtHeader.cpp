#include "PeNtHeader.h"
#include "iostream"

using std::cout; using std::endl;

DWORD PeNtHeader::PeNtHeaders = 0;

char* PeNtHeader::m_pSbase = 0;

HANDLE PeNtHeader::hMasterprocess = 0;

HANDLE PeNtHeader::hFilehand = 0;

HANDLE PeNtHeader::Imagebase = 0;

PeNtHeader::PeNtHeader()
{

}

PeNtHeader::~PeNtHeader()
{

}

//  读内存
DWORD PeNtHeader::LoadFile(DWORD lpAddress)
{
	// 获取文件大小
	DWORD dwSize = GetFileSize(PeNtHeader::hFilehand, NULL);
	DWORD nRead = 0;
	PeNtHeader::m_pSbase = new char[dwSize]{0};
	BOOL ErrorCode =
		ReadProcessMemory(PeNtHeader::hMasterprocess, (LPVOID)PeNtHeader::Imagebase, PeNtHeader::m_pSbase, dwSize, &nRead);
	if (ErrorCode)
		return 1;
	return -1;
 }

// 获取Nt头
BOOL PeNtHeader::GetPeNtHeader()
{
	// 获取Dos
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)PeNtHeader::m_pSbase;
	// 获取Nt
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + PeNtHeader::m_pSbase);
	// 保存Nt头
	PeNtHeader::PeNtHeaders = (DWORD)pNt;
	return PeNtHeader::PeNtHeaders;
}

// RVA转FOA
DWORD PeNtHeader::RvaofFoa(const DWORD Rvaddress)
{
	// 获取Dos
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)m_pSbase;
	// 获取Nt
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + m_pSbase);
	// 获取区段头
	PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNt);
	// 获取区段数量
	DWORD dwCount = pNt->FileHeader.NumberOfSections;
	// 区段转换
	for (DWORD i = 0; i < dwCount; ++i)
	{
		// 符合区段
		if ((pSection->VirtualAddress <= Rvaddress) && ((pSection->VirtualAddress + pSection->SizeOfRawData) > Rvaddress))
		{
			return Rvaddress - pSection->VirtualAddress + pSection->PointerToRawData;
		}
		++pSection;
	}
	return -1;
}