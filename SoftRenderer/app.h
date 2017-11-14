#pragma once

#include "windows.h"
#include "renderdevice.h"

class App
{
private:
	HWND			mWindow;
	HDC				mWindowDC;
	HBITMAP			mBITMAP;
	RenderDevice*	mRenderDevice;
	void*			mScreenBuffer;

private:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	void ProcessMessage( UINT msg, WPARAM wparam, LPARAM lparam );

public:
	App(int width = 800, int height = 600, LPCWSTR name = L"SoftRender");
	~App();

	virtual void	OnCreate( ) { };
	virtual void	OnRender( ) { };

public:
	void Run();
	RenderDevice& CreateRenderDevice( );
};