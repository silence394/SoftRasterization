#include "window.h"

LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch( msg )
	{
	case WM_KEYDOWN:
		int a = 1;
		break;
	}

	if ( msg == WM_CLOSE )
		PostQuitMessage( 0 );

	return DefWindowProc( hwnd, msg, wparam, lparam );
}

void Window::ProcessMessage(UINT msg, WPARAM wparam, LPARAM lparam)
{

}

Window::Window( )
{

}

Window::Window( int width, int height, LPCWSTR name )
{
	WNDCLASS winclass;
	RECT rect = {0, 0, width, height};

	winclass.style			= CS_BYTEALIGNCLIENT;
	winclass.lpfnWndProc	= WindowProc;
	winclass.cbClsExtra		= 0;
	winclass.cbWndExtra		= 0;
	winclass.hInstance		= GetModuleHandle( NULL );
	winclass.hIcon			= LoadIcon( NULL, IDI_APPLICATION );
	winclass.hCursor		= LoadCursor( NULL, IDC_ARROW );
	winclass.hbrBackground	= (HBRUSH) GetStockObject( BLACK_BRUSH );
	winclass.lpszMenuName	= NULL;
	winclass.lpszClassName	= name;

	if (!RegisterClass(&winclass))
		return;

	int cx = ::GetSystemMetrics( SM_CXSCREEN );
	int cy = ::GetSystemMetrics( SM_CYSCREEN );

	DWORD style = WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
	DWORD exstyle = 0;
	mWindow = CreateWindowEx( exstyle, name, name, style, ( cx - width ) / 2, ( cy - height ) / 2, width, height, NULL, NULL, winclass.hInstance, NULL );

	ShowWindow( mWindow, SW_SHOWNORMAL );

	BITMAPINFO bitinfo;

	bitinfo.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
	bitinfo.bmiHeader.biWidth = width;
	bitinfo.bmiHeader.biHeight = -height;
	bitinfo.bmiHeader.biPlanes = 1;
	bitinfo.bmiHeader.biBitCount = 32;
	bitinfo.bmiHeader.biCompression = BI_RGB;
	bitinfo.bmiHeader.biSizeImage = width * height * 4;
	bitinfo.bmiHeader.biXPelsPerMeter = 0;
	bitinfo.bmiHeader.biYPelsPerMeter = 0;
	bitinfo.bmiHeader.biClrUsed = 0;
	bitinfo.bmiHeader.biClrImportant = 0;

	HDC hdc;
	hdc = GetDC( mWindow );
	mWindowDC = CreateCompatibleDC( hdc );
	ReleaseDC( mWindow, hdc );

	mBITMAP = CreateDIBSection( mWindowDC, &bitinfo, DIB_RGB_COLORS, &mScreenBuffer, 0, 0 );
	if (mBITMAP == NULL)
		return;

	SelectObject( mWindowDC, mBITMAP );

	unsigned char* sb = (unsigned char*) mScreenBuffer;
	memset (sb, 128, width*height*4 );
}

Window::~Window()
{
	DeleteObject( mBITMAP );
	DeleteDC( mWindowDC );
	CloseWindow( mWindow );
}

void Window::Run()
{
	MSG msg = {0};
	while( 1 )
	{
		if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			if ( msg.message == WM_QUIT )
				break;

			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			HDC hdc = GetDC(mWindow);
			BitBlt( hdc, 0, 0, 800, 600, mWindowDC, 0, 0, SRCCOPY );
			ReleaseDC( mWindow, hdc );
			Sleep( 10 );
		}
	}
}
