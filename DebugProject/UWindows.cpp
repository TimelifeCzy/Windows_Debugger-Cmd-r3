#include "UWindows.h"
#include <iostream>
#pragma comment(lib, "winmm.lib")

using std::cout;

void WriteChar(int x, int y, char * szInfo, int color)
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos = { y * 2, x };
	SetConsoleCursorPosition(hOut, pos);
	SetConsoleTextAttribute(hOut, color);
	cout << szInfo;
}

void WriteInt(int x, int y, int Info, int color)
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos = { y * 2, x };
	SetConsoleCursorPosition(hOut, pos);
	SetConsoleTextAttribute(hOut, color);
	cout << Info;
}

void HideCursor()
{
	CONSOLE_CURSOR_INFO cci;
	cci.bVisible = FALSE;
	cci.dwSize = sizeof(cci);
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorInfo(handle, &cci);
}

void Musics_Game()
{
	PlaySoundA("7895.wav", NULL, SND_ASYNC | SND_NODEFAULT);
}