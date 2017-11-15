#include "app.h"
#include "math.h"
#include "vector3.h"
#include "vector4.h"
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
	mCamera.eye = Vector3( 3.0f, 2.0f, 1.0f );
	mCamera.look = Vector3( 0.0f, 0.0f, 0.0f );
	mCamera.up = Vector3( 0.0f, 0.0f, 1.0f );

	mWorldTransform = Matrix4::identity;
	mViewTransform = Matrix4::View( mCamera.eye, mCamera.look, mCamera.up );
	mPerspectTransform = Matrix4::Perspective( 1.57f, (float) mRenderDevice->GetDeviceWidth( ) / (float) mRenderDevice->GetDeviceHeight( ), 0.001f, 1000.0f );


}

void DemoApp::OnRender( )
{
	mRenderDevice->SetClearColor( 0xFF808080 );
	mRenderDevice->Clear( );

	struct Vertex
	{
		Vector4	pos;
		uint	color;
	};

	Vertex vertex[8] = 
	{
		{ Vector4( -0.5f, -0.5f, -0.5f, 1.0f ), 0xffff0000 },
		{ Vector4( -0.5f, +0.5f, -0.5f, 1.0f ), 0xffff0000 },
		{ Vector4( +0.5f, +0.5f, -0.5f, 1.0f ), 0xffff0000 },
		{ Vector4( +0.5f, -0.5f, -0.5f, 1.0f ), 0xffff0000 },
		{ Vector4( -0.5f, -0.5f, +0.5f, 1.0f ), 0xffff0000 },
		{ Vector4( -0.5f, +0.5f, +0.5f, 1.0f ), 0xffff0000 },
		{ Vector4( +0.5f, +0.5f, +0.5f, 1.0f ), 0xffff0000 },
		{ Vector4( +0.5f, -0.5f, +0.5f, 1.0f ), 0xffff0000 },
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

	Matrix4 wvp = mWorldTransform * mViewTransform * mPerspectTransform;

	for ( uint i = 0; i < 36; i += 3 )
	{
		Vector4 s1 = vertex[indexs[i]].pos * wvp;
		s1 = s1 / s1.w;
		Vector4 s2 = vertex[indexs[i+1]].pos * wvp;
		s2 = s2 / s2.w;
		Vector4 s3 = vertex[indexs[i+2]].pos * wvp;
		s3 = s3 / s3.w;

		Point point1( (int) ( (s1.x + 1.0f) * 0.5f * mRenderDevice->GetDeviceWidth( ) ), (int) ( (s1.y + 1.0f) * 0.5f * mRenderDevice->GetDeviceHeight( ) ) );
		Point point2( (int) ( (s2.x + 1.0f) * 0.5f * mRenderDevice->GetDeviceWidth( ) ), (int) ( (s2.y + 1.0f) * 0.5f * mRenderDevice->GetDeviceHeight( ) ) );
		Point point3( (int) ( (s3.x + 1.0f) * 0.5f * mRenderDevice->GetDeviceWidth( ) ), (int) ( (s3.y + 1.0f) * 0.5f * mRenderDevice->GetDeviceHeight( ) ) );

		uint color;
		if ( i < 10 )
			color = 0xffff0000;
		else if ( i < 20 )
			color = 0xff00ff00;
		else
			color = 0xff0000ff;

		//mRenderDevice->FillTriangle( point1, point2, point3, 0xff00ff00 );
		mRenderDevice->DrawLine( point1, point2, color );
		mRenderDevice->DrawLine( point3, point2, color );
		mRenderDevice->DrawLine( point1, point3, color );
	}
}

int main( )
{
	DemoApp app( 800, 600 );
	app.Create( );
	app.Run( );

	return 0;
}