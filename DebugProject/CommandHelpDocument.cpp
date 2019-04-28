#include "CommandHelpDocument.h"
#include "UWindows.h"
#include <iostream>
#include <string>

using std::cout; using std::endl;

ComHelpDocument::ComHelpDocument()
{

}

ComHelpDocument::~ComHelpDocument()
{

}

// 命令管理
void ComHelpDocument::ComandManage()
{
	cout << "命令：\t    ";
	cout << "说明：\t\t\t";
	cout << "补充：" << endl;
	string strName;
	strName = "1.  t       ";
	strName = strName + "    单步中断            ";
	strName = strName + "\t    TF位置1";
	CmdManage.push_back(strName);
	strName = "2.  g       ";
	strName = strName + "    运行                ";
	strName = strName + "\t    未补充1";
	CmdManage.push_back(strName);
	strName = "3.  bp      ";
	strName = strName + "    设置断点            ";
	strName = strName + "\t    TFint3 断点 0xCC";
	CmdManage.push_back(strName);
	strName = "4.  md      ";
	strName = strName + "    修改汇编指令        ";
	strName = strName + "\t    address / ModifyData：操作数两个修改的地址 及 修改汇编指令";
	CmdManage.push_back(strName);
	strName = "5.  help    ";
	strName = strName + "    令帮助查询          ";
	strName = strName + "\t    列举当前的指令信息及功能介绍";
	CmdManage.push_back(strName);
	strName = "6.  rmodify ";
	strName = strName + "    修改寄存器          ";
	strName = strName + "\t    格式: rmodify ESP 0x00000000 名称大写";
	CmdManage.push_back(strName);
	strName = "7.  u       ";
	strName = strName + "    查看汇编指令        ";
	strName = strName + "\t    查看当前ECS:EIP指向为起始的机器指令（默认6条）";
	CmdManage.push_back(strName);
	strName = "8.  stack   ";
	strName = strName + "    获取栈信息          ";
	strName = strName + "\t    查看当前ESS:EIP指向为起始的栈内存信息（默认10条）";
	CmdManage.push_back(strName);
	strName = "9.  d       ";
	strName = strName + "    带参数内存数据获取   ";
	strName = strName + "\t    格式:d 0x000000（默认10条）";
	CmdManage.push_back(strName);
	strName = "10. dd      ";
	strName = strName + "    获取默认内存数据     ";
	strName = strName + "\t    查看当前EDS:EIP指向为起始的数据内存信息（默认10条）";
	CmdManage.push_back(strName);
	strName = "11. dmodify ";
	strName = strName + "    修改内存数据         ";
	strName = strName + "\t    修改指定的地址内存数据";
	CmdManage.push_back(strName);
	strName = "12. export  ";
	strName = strName + "    导出表               ";
	strName = strName + "\t    查看当前进程导出表";
	CmdManage.push_back(strName);
	strName = "13. import  ";
	strName = strName + "    导入表               ";
	strName = strName + "\t    查看当前进程导入表";
	CmdManage.push_back(strName);
	strName = "14. reload  ";
	strName = strName + "    重载程序             ";
	strName = strName + "\t    重新加载程序";
	CmdManage.push_back(strName);
	strName = "15. bl      ";
	strName = strName + "    断点查看             ";
	strName = strName + "\t    软、硬、系统断点列表";
	CmdManage.push_back(strName);	
	strName = "16. hwxbp   ";
	strName = strName + "    硬断点（执行）       ";
	strName = strName + "\t    硬件执行断点";
	CmdManage.push_back(strName);
	strName = "17. hwrwbp  ";
	strName = strName + "    硬断点（读写）       ";
	strName = strName + "\t    类型1. 读  类型2. 写 格式：(hwrwbp 0x00000000 Type Len)";
	CmdManage.push_back(strName);
	strName = "18. ml      ";
	strName = strName + "    模块查看             ";
	strName = strName + "\t    查看被调试程序进程模块";
	CmdManage.push_back(strName);
	strName = "19. mu      ";
	strName = strName + "    汇编指令撤销          ";
	strName = strName + "\t    只能撤销上一次的修改指令";
	CmdManage.push_back(strName);
	strName = "20. memorybp";
	strName = strName + "    内存断点             ";
	strName = strName + "\t    类型0. 读	类型1. 写 类型8. 可执行 格式：(memorybp 0x00000000 Type)";
	CmdManage.push_back(strName);

	

}

// 显示命令帮助文档
void ComHelpDocument::ShowComandHelp()
{
	// 初始化命令容器
	ComandManage();
	// 遍历打印名称 作用 说明
	for (size_t i = 0; i < CmdManage.size(); ++i)
	{
		cout << CmdManage[i] << endl;
	}
}