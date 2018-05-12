#pragma once

#include "Prerequisites.h"
#include "Windows.h"

static App*	GStaticWindow = nullptr;

class App
{
private:
	HWND			mWindow;
	HDC				mWindowDC;
	HBITMAP			mBITMAP;
	void*			mScreenBuffer;

private:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	void ProcessMessage( UINT msg, WPARAM wparam, LPARAM lparam );

public:
	App( int width = 800, int height = 600, LPCWSTR name = L"SoftRender" );
	~App();

	virtual void	OnCreate( ) { };
	virtual void	OnRender( ) { };
	virtual void	OnClose( ) { };

	virtual void	OnMouseMove( int x, int y ) { };
	virtual void	OnMouseWheel( int delta ) { };

public:
	void			Create( );
	void			Run();
};