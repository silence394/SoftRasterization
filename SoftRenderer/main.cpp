#include "libengine.h"
#include <string>
#include "stdio.h"
#include "stdlib.h"
class VertexShader : public IVertexShader
{
	virtual void Execute( Vector4* regs )
	{
		regs[0] *= GetMatrix( _CT_WVP_TRANSFORM );
	}
};

// Regs[0] - position, regs[1] = color, reg[2] = UV., reg[3] = 
class PixelShader : public IPixelShader
{
	virtual void Execute( Vector4* regs, Color& color, float& depth )
	{
		color = SampleTexture( 0, regs[2].x, regs[2].y );
	}
};

class DemoApp : public App
{
private:
	RenderDevice*	mRenderDevice;
	Camera			mCamera;
	Matrix4			mWorldTransform;
	Matrix4			mViewTransform;
	Matrix4			mPerspectTransform;
	Texture*		mTexture;

	InputLayout*	mInputLayout;
	IVertexShader*	mVertexShader;
	IPixelShader*	mPixelShader;

	GraphicsBuffer*	mVertexBuffer;
	GraphicsBuffer*	mIndexBuffer;

public:
	DemoApp( int width = 800, int height = 600, LPCWSTR name = L"Demo" )
		: App( width, height, name ) { }

public:
	virtual void OnCreate( );
	virtual void OnRender( );
	virtual void OnClose( );

	virtual void OnMouseMove( int x, int y );
};

void DemoApp::OnCreate( )
{
	mRenderDevice = GetRenderDevice( );
	mCamera.SetPosition( Vector3( 5.0f, 4.0f, -3.0f ) );
	mCamera.LookAt( Vector3( 0.0f, 0.0f, 0.0f ) );

	mWorldTransform = Matrix4::identity;
	mViewTransform = mCamera.GetViewMatrix( );
	mPerspectTransform = Matrix4::Perspective( 1.57f, (float) mRenderDevice->GetDeviceWidth( ) / (float) mRenderDevice->GetDeviceHeight( ), 0.001f, 1000.0f );

	// Create texture.
	uint width = 256;
	uint height = 256;
	uint length = width * height;
	uint* texbuffer = new uint[ length ];
	uint* base = texbuffer;
	for ( uint j = 0; j < height; j ++ )
	{
		for ( uint i = 0; i < width; i ++ )
		{
			uint color = ( ( i / 32 + j / 32 ) & 1) ? 0xffffffff : 0xff0093dd;
			*base ++ = color;
		}
	}

	mTexture = new Texture( texbuffer, width, height, Texture::TF_ARGB8 );

	mPixelShader = new PixelShader( );
	mRenderDevice->SetPixelShader( mPixelShader );

	mVertexShader = new VertexShader( );
	mRenderDevice->SetVertexShader( mVertexShader );

	std::vector<InputElementDesc> descs;
	descs.push_back( InputElementDesc( "POSITION", GraphicsBuffer::BF_R32B32G32_FLOAT ) );
	descs.push_back( InputElementDesc( "COLOR", GraphicsBuffer::BF_A8R8G8B8 ) );
	descs.push_back( InputElementDesc( "TEXCOORD0", GraphicsBuffer::BF_R32G32_FLOAT ) );

	mInputLayout = mRenderDevice->CreateInputLayout( &descs[0], descs.size( ) );

	struct Vertex
	{
		Vector3	pos;
		uint	color;
		Vector2	texcoord;
	};

	Vertex vertex[24] = 
	{
		// X +.
		{ Vector3(  1.0f,  1.0f,  1.0f ), 0xffff0000, Vector2( 0.0f, 0.0f ) },
		{ Vector3(  1.0f,  1.0f, -1.0f ), 0xff0000ff, Vector2( 1.0f, 0.0f ) },
		{ Vector3(  1.0f, -1.0f, -1.0f ), 0xffff00ff, Vector2( 1.0f, 1.0f ) },
		{ Vector3(  1.0f, -1.0f,  1.0f ), 0xffffff00, Vector2( 0.0f, 1.0f ) },

		// X -.
		{ Vector3( -1.0f, -1.0f,  1.0f ), 0xff00ff00, Vector2( 0.0f, 0.0f )},
		{ Vector3( -1.0f, -1.0f, -1.0f ), 0xff22ff88, Vector2( 1.0f, 0.0f ) },
		{ Vector3( -1.0f,  1.0f, -1.0f ), 0xff339977, Vector2( 1.0f, 1.0f ) },
		{ Vector3( -1.0f,  1.0f,  1.0f ), 0xff00ffff, Vector2( 0.0f, 1.0f ) },

		// Y +.
		{ Vector3( -1.0f,  1.0f,  1.0f ), 0xff00ffff, Vector2( 0.0f, 0.0f ) },
		{ Vector3( -1.0f,  1.0f, -1.0f ), 0xff339977, Vector2( 1.0f, 0.0f ) },
		{ Vector3(  1.0f,  1.0f, -1.0f ), 0xff0000ff, Vector2( 1.0f, 1.0f ) },
		{ Vector3(  1.0f,  1.0f,  1.0f ), 0xffff0000, Vector2( 0.0f, 1.0f ) },

		// Y -.
		{ Vector3(  1.0f, -1.0f,  1.0f ), 0xffffff00, Vector2( 0.0f, 0.0f ) },
		{ Vector3(  1.0f, -1.0f, -1.0f ), 0xffff00ff, Vector2( 1.0f, 0.0f ) },
		{ Vector3( -1.0f, -1.0f, -1.0f ), 0xff22ff88, Vector2( 1.0f, 1.0f ) },
		{ Vector3( -1.0f, -1.0f,  1.0f ), 0xff00ff00, Vector2( 0.0f, 1.0f ) },

		// Z +.
		{ Vector3( -1.0f, -1.0f,  1.0f ), 0xff00ff00, Vector2( 0.0f, 0.0f ) },
		{ Vector3( -1.0f,  1.0f,  1.0f ), 0xff00ffff, Vector2( 1.0f, 0.0f ) },
		{ Vector3(  1.0f,  1.0f,  1.0f ), 0xffff0000, Vector2( 1.0f, 1.0f ) },
		{ Vector3(  1.0f, -1.0f,  1.0f ), 0xffffff00, Vector2( 0.0f, 1.0f ) },

		// Z -.
		{ Vector3( -1.0f,  1.0f, -1.0f ), 0xff339977, Vector2( 0.0f, 0.0f ) },
		{ Vector3( -1.0f, -1.0f, -1.0f ), 0xff22ff88, Vector2( 1.0f, 0.0f ) },
		{ Vector3(  1.0f, -1.0f, -1.0f ), 0xffff00ff, Vector2( 1.0f, 1.0f ) },
		{ Vector3(  1.0f,  1.0f, -1.0f ), 0xff0000ff, Vector2( 0.0f, 1.0f ) },
	};

	uint vsize = sizeof( Vertex );
	uint vlen = sizeof( vertex );
	byte* vbuffer = new byte[ vlen ];
	memcpy( vbuffer, vertex, vlen );

	ushort indices[36] =
	{
		0, 2, 1,
		0, 3, 2,
		4, 6, 5,
		4, 7, 6,
		8, 10, 9,
		8, 11, 10,
		12, 14, 13,
		12, 15, 14,
		16, 18, 17,
		16, 19, 18,
		20, 22, 21,
		20, 23, 22,
	};
	
	uint ilen = sizeof( indices );
	byte* ibuffer = new byte[ ilen ];
	memcpy( ibuffer, indices, ilen );

	mVertexBuffer = mRenderDevice->CreateBuffer( vbuffer, vlen, vsize );
	mIndexBuffer = mRenderDevice->CreateBuffer( ibuffer, ilen, sizeof( ushort ) );
}

void DemoApp::OnClose( )
{
	delete mTexture;
	delete mVertexShader;
	delete mPixelShader;
}

void DemoApp::OnMouseMove( int x, int y )
{
	static int lastx = x, lasty = y;

	if ( ( GetKeyState( 32 ) & 0x80 ) != 0 )
	{
		mCamera.Phi( ( x - lastx ) * 0.005f );
		mCamera.Theta( ( y- lasty ) * 0.005f );
		mViewTransform = mCamera.GetViewMatrix( );
	}

	lastx = x; lasty = y;
}

void DemoApp::OnRender( )
{
	mRenderDevice->BeginScene( );
	mRenderDevice->SetClearColor( 0xFF808080 );
	mRenderDevice->Clear( );

	mRenderDevice->SetTexture( 0, mTexture );
	mVertexShader->SetMatrix( ShaderBase::_CT_WVP_TRANSFORM, mWorldTransform * mViewTransform * mPerspectTransform );
	mRenderDevice->SetInputLayout( mInputLayout );
	mRenderDevice->SetVertexBuffer( mVertexBuffer );
	mRenderDevice->SetIndexBuffer( mIndexBuffer );
	mRenderDevice->DrawIndex( mIndexBuffer->GetLength( ) / mIndexBuffer->GetSize( ), 0, 0 );
}

int main( )
{
	DemoApp app( 800, 600 );
	app.Create( );
	app.Run( );

	return 0;
}