#pragma once

#include "prerequisites.h"
#include "windows.h"

static App*	GStaticWindow = nullptr;

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
	App( int width = 800, int height = 600, LPCWSTR name = L"SoftRender" );
	~App();

	virtual void	OnCreate( ) { };
	virtual void	OnRender( ) { };
	virtual void	OnClose( ) { };

	virtual void	OnMouseMove( int x, int y ) { };

public:
	inline RenderDevice* GetRenderDevice( )
		{ return mRenderDevice; }

	void			Create( );
	void			Run();
};