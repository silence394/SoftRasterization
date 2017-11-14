#pragma once

#include "windows.h"

class Window
{
private:
	HWND	mWindow;
	HDC		mWindowDC;
	HBITMAP	mBITMAP;
	void*	mScreenBuffer;

private:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	void ProcessMessage( UINT msg, WPARAM wparam, LPARAM lparam );

public:
	Window();
	Window(int width, int height, LPCWSTR name);
	~Window();

public:
	void Run();
};