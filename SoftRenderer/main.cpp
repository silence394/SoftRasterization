#include "libengine.h"
#include <string>

class VertexShader : public IVertexShader
{
	virtual void Execute( Vector4* regs )
	{
		regs[0] *= GetMatrix( _CT_WVP_TRANSFORM );
	}
};

class PixelShader : public IPixelShader
{
	virtual void Execute( Vector4* regs, Color& color, float& depth )
	{
		color = Color( regs[1].x, regs[1].y, regs[1].z, regs[1].w );
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
};

void DemoApp::OnCreate( )
{
	mRenderDevice = GetRenderDevice( );
	mCamera.eye = Vector3( 5.0f, 4.0f, 3.0f );
	mCamera.look = Vector3( 0.0f, 0.0f, 0.0f );
	mCamera.up = Vector3( 0.0f, 0.0f, 1.0f );

	mWorldTransform = Matrix4::identity;
	mViewTransform = Matrix4::View( mCamera.eye, mCamera.look, mCamera.up );
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

	Vertex vertex[8] = 
	{
		{ Vector3( -1.0f, -1.0f, -1.0f ), 0xffff0000, Vector2( 0.0f, 0.0f ) },
		{ Vector3( -1.0f, +1.0f, -1.0f ), 0xff0000ff, Vector2( 1.0f, 0.0f ) },
		{ Vector3( +1.0f, +1.0f, -1.0f ), 0xffff0000, Vector2( 1.0f, 1.0f ) },
		{ Vector3( +1.0f, -1.0f, -1.0f ), 0xffff00ff, Vector2( 0.0f, 1.0f ) },
		{ Vector3( -1.0f, -1.0f, +1.0f ), 0xffffff00, Vector2( 1.0f, 1.0f ) },
		{ Vector3( -1.0f, +1.0f, +1.0f ), 0xffff0000, Vector2( 0.0f, 1.0f ) },
		{ Vector3( +1.0f, +1.0f, +1.0f ), 0xff00ff00, Vector2( 0.0f, 0.0f ) },
		{ Vector3( +1.0f, -1.0f, +1.0f ), 0xff22ffff, Vector2( 1.0f, 0.0f ) },
	};

	uint vcount = 8;
	uint vsize = sizeof( Vertex );
	uint vlen = vcount * vsize;
	byte* vbuffer = new byte[ vlen ];
	memcpy( vbuffer, vertex, vlen );

	ushort indices[36] =
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

bool CheckInCVV( const Vector4& v )
{
	if ( v.x < -1.0f || v.x > 1.0f || v.y < -1.0f || v.y > 1.0f || v.z < 0 || v.w > 1.0f )
		return false;

	return true;
}

void DemoApp::OnRender( )
{
	mRenderDevice->SetClearColor( 0xFF808080 );
	mRenderDevice->Clear( );

	mVertexShader->SetMatrix( ShaderBase::_CT_WVP_TRANSFORM, mWorldTransform * mViewTransform * mPerspectTransform );
	mRenderDevice->SetInputLayout( mInputLayout );
	mRenderDevice->SetVertexBuffer( mVertexBuffer );
	mRenderDevice->SetIndexBuffer( mIndexBuffer );
	mRenderDevice->DrawIndex( mIndexBuffer->GetLength( ) / mIndexBuffer->GetSize( ), 0, 0 );

	for ( uint j = 0; j < mTexture->GetHeight( ); j ++ )
	{
		for ( uint i = 0; i < mTexture->GetWidth( ); i ++ )
			mRenderDevice->DrawPixel( i, j, mTexture->GetPixel( i, j ) );
	}
}

int main( )
{
	DemoApp app( 800, 600 );
	app.Create( );
	app.Run( );

	return 0;
}