#pragma once
#ifndef _COMMANDHELPDOCUMENT_H_
#define _COMMANDHELPDOCUMENT_H_
#include <vector>
#include <string>


using std::vector;	using std::string;

class ComHelpDocument
{
public:
	ComHelpDocument();
	virtual ~ComHelpDocument();

public:
	// 命令帮助文档显示
	void puShowComandHelp() { this->ShowComandHelp(); }

private:
	// 命令管理
	void ComandManage();
	// 命令帮助文档显示
	void ShowComandHelp();
	// 命令预留接口(增删改查)

private:
	vector<string> CmdManage;
};

#endif