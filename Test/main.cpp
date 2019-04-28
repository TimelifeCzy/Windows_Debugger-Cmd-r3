#include <iostream>

using std::cout; using std::endl;

// 单步步过测试
void SetpOver()
{
	printf("步过步过");
}

int main(int argc, char** argv)
{
	// 单步步过测试
	SetpOver();
	// 读写断点测试
	static int b = 10;
	b += 10;
	// 软件断点测试（永久性）
	for (int i = 0; i < 10; i++)
	{
		printf("测试断点\n");
	}
	// 内存读写测试
	system("pause");
	return 0;
}