#include "ParsingSymbolTab.h"
#include <DbgHelp.h>
#pragma comment (lib, "Dbghelp.lib")

ParSymbol::ParSymbol()
{
	
}

ParSymbol::~ParSymbol()
{
	 
}

// 初始化调试符号
void ParSymbol::InitParSymbol()
{
	// 初始化符号
	SymInitialize(ObjectProcess, NULL , FALSE);
	// 初始化结构体信息
}


// 显示符号
void ParSymbol::ShowParSymbol()
{

}