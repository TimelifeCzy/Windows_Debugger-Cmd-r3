#pragma once
#ifndef _UWINDOWS_H_
#define _UWINDOWS_H_
#include <windows.h>
#include <stdio.h>

/*字符句柄光标*/
void WriteChar(int x, int y, char * szInfo, int color);

/*整形句柄光标*/
void WriteInt(int x, int y, int Info, int color);

/*隐藏光标*/
void HideCursor();

/*加载音乐*/
void Musics_Game();

int SetWindowsSize(char* pszWindowTitle, int nX, int nY);

#endif