#include "app.h"
#include "math.h"
#include "vector3.h"
#include "matrix4.h"
#include "camera.h"
RenderDevice*	gRenderDevice = nullptr;

class DemoApp : public App
{
private:
	RenderDevice*	mRenderDevice;
	Camera			mCamera;
	Matrix4			mWorldTransform;
	Matrix4			mViewTransform;
	Matrix4			mPerspectTransform;

public:
	DemoApp( int width = 800, int height = 600, LPCWSTR name = L"Demo" )
		: App( width, height, name ) { }

public:
	virtual void OnCreate( );
	virtual void OnRender( );
};

void DemoApp::OnCreate( )
{
	mRenderDevice = GetRenderDevice( );
	mCamera.eye = Vector3( 3.0f, 0.0f, 0.0f );
	mCamera.look = Vector3( 0.0f, 0.0f, 0.0f );
	mCamera.up = Vector3( 0.0f, 0.0f, 1.0f );

	mWorldTransform = Matrix4::identity;
	mViewTransform = Matrix4::View( mCamera.eye, mCamera.look, mCamera.up );
	mPerspectTransform = Matrix4::Perspective( 1.57f, (float) mRenderDevice->GetDeviceWidth( ) / (float) mRenderDevice->GetDeviceHeight( ), 0.001f, 1000.0f );

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
	mRenderDevice->SetClearColor( 0xFF808080 );
	mRenderDevice->Clear( );

	Matrix4 wvp = mWorldTransform * mViewTransform * mPerspectTransform;

	mRenderDevice->FillTriangle( Point( 200, 400 ), Point( 100, 100 ), Point( 300, 300 ), 0xff00ff00 );
}

int main( )
{
	DemoApp app( 800, 600 );
	app.Create( );
	app.Run( );

	return 0;
}