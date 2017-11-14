
#include "app.h"
#include "math.h"
#include "vector3.h"
RenderDevice*	gRenderDevice = nullptr;

class DemoApp : public App
{
public:
	DemoApp( int width = 800, int height = 600, LPCWSTR name = L"Demo" )
		: App( width, height, name ) { }

public:
	virtual void OnCreate( );
	virtual void OnRender( );
};

void DemoApp::OnCreate( )
{
	struct Vertex
	{
		Vector3	pos;
		uint	color;
	};

	Vertex vertex[8] = 
	{
		{ Vector3( -0.5f, -0.5f, -0.5f ), 0xffff0000 },
		{ Vector3( -0.5f, +0.5f, -0.5f ), 0xffff0000 },
		{ Vector3( +0.5f, +0.5f, -0.5f ), 0xffff0000 },
		{ Vector3( +0.5f, -0.5f, -0.5f ), 0xffff0000 },
		{ Vector3( -0.5f, -0.5f, +0.5f ), 0xffff0000 },
		{ Vector3( -0.5f, +0.5f, +0.5f ), 0xffff0000 },
		{ Vector3( +0.5f, +0.5f, +0.5f ), 0xffff0000 },
		{ Vector3( +0.5f, -0.5f, +0.5f ), 0xffff0000 },
	};

	uint indexs[36] =
	{
		0, 1, 2,
		0, 2, 3,
		4, 6, 5,
		4, 7, 6,
		4, 5, 1,
		4, 1, 0,
		3, 2, 6,
		3, 6, 7,
		1, 5, 6,
		1, 6, 2,
		4, 0, 3,
		4, 3, 7,
	};
}

void DemoApp::OnRender( )
{
	gRenderDevice->SetClearColor( 0xFF808080 );
	gRenderDevice->Clear( );

// 	for ( int i = 0; i < 5; i ++ )
// 	{
// 		int x1 = rand( ) % gRenderDevice->GetDeviceWidth( );
// 		int x2 = rand( ) % gRenderDevice->GetDeviceWidth( );
// 		int y1 =  rand( ) % gRenderDevice->GetDeviceHeight( );
// 		int y2 =  rand( ) % gRenderDevice->GetDeviceHeight( );
// 		Point p1( x1, y1 );
// 		Point p2( x2, y2 );
// 		gRenderDevice->DrawLine( p1, p2, 0xff00ff00 );
// 	}

// 	gRenderDevice->FillTriangle( Point( 300, 100 ), Point( 200, 200 ), Point( 100, 100 ), 0xff00ff00 );
// 	gRenderDevice->DrawPoint( Point( 500, 200 ), 0xffff0000 );
	//gRenderDevice->FillTriangle( Point( 300, 400 ), Point( 200, 400 ), Point( 300, 300 ), 0xff00ff00 );
	gRenderDevice->FillTriangle( Point( 200, 400 ), Point( 100, 100 ), Point( 300, 300 ), 0xff00ff00 );
}

int main( )
{
	DemoApp window( 800, 600 );
	gRenderDevice = &window.CreateRenderDevice( );
	window.Run( );

	return 0;
}