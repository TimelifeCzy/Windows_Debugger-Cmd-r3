#include <iostream>
#include "MasterExceptionHandler.h"

using namespace std;

int main(int argc, char** argv)
{
	cout << "请拖拽可执行程序进行调试" << endl;
	MasterExceptionHandler ExceptionObj;
	ExceptionObj.ResponsetoDragMessages();
	return 0;
}