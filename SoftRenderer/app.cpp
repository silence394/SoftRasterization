#include "App.h"
#include "RenderDevice.h"

LRESULT CALLBACK App::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch( msg )
	{
		case WM_KEYDOWN:
		{
			break;
		}
			
		case WM_MOUSEMOVE:
		{
			static uint last = 0;
			if ( last != lparam )
			{
				last = lparam;
				int x = (int) LOWORD (lparam);
				int y = (int) HIWORD (lparam);
				GStaticWindow->OnMouseMove(x, y);
			}

			break;
		}

		case WM_MOUSEWHEEL:
			{
				int x = (int) LOWORD (lparam);
				int y = (int) HIWORD (lparam);
				int delta = (short) HIWORD (wparam);

				GStaticWindow->OnMouseWheel( delta );

				break;
			}
	}

	if ( msg == WM_CLOSE )
		PostQuitMessage( 0 );

	return DefWindowProc( hwnd, msg, wparam, lparam );
}

void App::ProcessMessage(UINT msg, WPARAM wparam, LPARAM lparam)
{

}

App::App( int width, int height, LPCWSTR name )
{
	WNDCLASS winclass;

	winclass.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
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

	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	DWORD exstyle = 0;
	mWindow = CreateWindowEx( exstyle, name, name, style, 0, 0, width, height, NULL, NULL, winclass.hInstance, NULL );

	GStaticWindow = this;

	RECT rect = { 0 };
	GetClientRect( mWindow, &rect );
	int dw = rect.right;
	int dh = rect.bottom;

	BITMAPINFO bitinfo;

	bitinfo.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
	bitinfo.bmiHeader.biWidth = dw;
	bitinfo.bmiHeader.biHeight = -dh;
	bitinfo.bmiHeader.biPlanes = 1;
	bitinfo.bmiHeader.biBitCount = 32;
	bitinfo.bmiHeader.biCompression = BI_RGB;
	bitinfo.bmiHeader.biSizeImage = dw * dh * 4;
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

	ShowWindow( mWindow, SW_SHOWNORMAL );
}

App::~App()
{
	OnClose( );
	DeleteObject( mBITMAP );
	DeleteDC( mWindowDC );
	CloseWindow( mWindow );
}

void App::Create()
{
	RECT rect = { 0 };
	GetClientRect( mWindow, &rect );
	RenderDevice::Instance( ).Init( mWindow, (uint *) mScreenBuffer );

	OnCreate( );
}

void App::Run()
{
	MSG msg = {0};
	while( true )
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
			OnRender( );

			//  Present to screen.
			HDC hdc = GetDC( mWindow );
			RECT rect = { 0 };
			GetClientRect( mWindow, &rect );
			BitBlt( hdc, 0, 0, rect.right, rect.bottom, mWindowDC, 0, 0, SRCCOPY );
			ReleaseDC( mWindow, hdc );
			Sleep( 10 );
		}
	}
}