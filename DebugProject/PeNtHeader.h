#pragma once
#ifndef _PENTHEADER_H_
#define _PENTHEADER_H_
#include "windows.h"

class PeNtHeader
{
public:
	PeNtHeader();
	virtual ~PeNtHeader();

public:
	// 文件句柄
	static HANDLE hFilehand;
	// 保存加载基址
	static HANDLE Imagebase;
	// 获取FOA
	DWORD puRvaofFoa(const DWORD Rvaddress) { 
		return this->RvaofFoa(Rvaddress); 
	}
	// 获取NT头
	DWORD puGetPeNtHeader(){ 
		return this->GetPeNtHeader(); 
	}
	// 加载文件到内存
	void puLoadFile(DWORD lpAddress) {
		this->LoadFile(lpAddress);
	}
	// 设置文件路径接口
	void puSetProcessHeader(HANDLE ProcessBases){ PeNtHeader::hMasterprocess = ProcessBases; }
	// 获取加载基址
	char* puGetLoadFileBaseAddress(){ return PeNtHeader::m_pSbase; }
	
private:
	// 加载文件到内存
	DWORD LoadFile(DWORD lpAddress);
	// 获取Nt头
	BOOL GetPeNtHeader();
	// RVA转FOA
	DWORD RvaofFoa(const DWORD Rvaddress);

private:
	// 获取文件首地址
	static char* m_pSbase;
	// 保存NT头
	static DWORD PeNtHeaders;
	// 保存句柄（机制）
	static HANDLE hMasterprocess;
};


#endif