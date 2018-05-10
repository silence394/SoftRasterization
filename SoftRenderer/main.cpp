#include "libengine.h"
#include <string>
#include "stdio.h"
#include "stdlib.h"
#include <memory>

class VertexShader : public IVertexShader
{
	virtual void Execute( VSInput& in, PSInput& out, ConstantBufferPtr* cb )
	{
		out.position( ) = in.attribute( 0 ) * cb[0]->GetConstant<Matrix4>( "wvp" );
		out.attribute( 0 ) = in.attribute( 1 );
		out.attribute( 1 ) = in.attribute( 2 );
	}
};

class PixelShader : public IPixelShader
{
	virtual void Execute( PSInput& in, PSOutput& out, float& depth )
	{
		out.color = Texture2D( 0, in.attribute( 1 ).x, in.attribute( 1 ).y );
	}
};

class DemoApp : public App
{
private:
	Camera			mCamera;
	Matrix4			mWorldTransform;
	Matrix4			mViewTransform;
	Matrix4			mPerspectTransform;
	TexturePtr		mTexture;
	SamplerStatePtr	mSampler;

	InputLayout*	mInputLayout;
	IVertexShader*	mVertexShader;
	IPixelShader*	mPixelShader;

	GraphicsBuffer*	mVertexBuffer;
	GraphicsBuffer*	mIndexBuffer;

	ConstantBufferPtr	mVSContantBuffer;
	ConstantBufferPtr	mPSConstantBuffer;

public:
	DemoApp( int width = 800, int height = 600, LPCWSTR name = L"Demo" )
		: App( width, height, name ) { }

public:
	virtual void OnCreate( );
	virtual void OnRender( );
	virtual void OnClose( );

	virtual void OnMouseMove( int x, int y );
	virtual void OnMouseWheel( int delta );
};

void DemoApp::OnCreate( )
{
	RenderDevice& rd = RenderDevice::Instance( );

	mCamera.SetPosition( Vector3( 4, 4, 4 ) );
	mCamera.LookAt( Vector3( 0.0f, 0.0f, 0.0f ) );

	mViewTransform = mCamera.GetViewMatrix( );
	mPerspectTransform = Matrix4::Perspective( 3.141592654f / 4.0f, (float) rd.GetDeviceWidth( ) / (float) rd.GetDeviceHeight( ), 1.0f, 5000.0f );

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

	mTexture = TextureManager::Load( L"../Media/stone_color.jpg" );

	SamplerStateDesc desc;
	desc.address = EAddressMode::AM_CLAMP;
	desc.filter = ESamplerFilter::SF_Linear;
	mSampler = rd.CreateSamplerState( desc );

	mPixelShader = new PixelShader( );
	rd.SetPixelShader( mPixelShader );

	mVertexShader = new VertexShader( );
	rd.SetVertexShader( mVertexShader );

	std::vector<InputElementDesc> descs;
	descs.push_back( InputElementDesc( "POSITION", GraphicsBuffer::BF_R32B32G32_FLOAT ) );
	descs.push_back( InputElementDesc( "COLOR", GraphicsBuffer::BF_A8R8G8B8 ) );
	descs.push_back( InputElementDesc( "TEXCOORD0", GraphicsBuffer::BF_R32G32_FLOAT ) );

	mInputLayout = rd.CreateInputLayout( &descs[0], descs.size( ) );

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

	mVertexBuffer = rd.CreateBuffer( vbuffer, vlen, vsize );
	mIndexBuffer = rd.CreateBuffer( ibuffer, ilen, sizeof( ushort ) );

	mVSContantBuffer = rd.CreateConstantBuffer( );
	mPSConstantBuffer = rd.CreateConstantBuffer( );
}

void DemoApp::OnClose( )
{
	delete mVertexShader;
	delete mPixelShader;
}

void DemoApp::OnMouseMove( int x, int y )
{
	static int lastx = x, lasty = y;

	if ( ( GetKeyState( 0x01 ) & 0x80 ) != 0 )
	{
		mCamera.Phi( ( x - lastx ) * -0.005f );
		mCamera.Theta( ( y- lasty ) * -0.005f );
		mViewTransform = mCamera.GetViewMatrix( );
	}

	lastx = x; lasty = y;
}

void DemoApp::OnMouseWheel( int delta )
{
	mCamera.Zoom( - mCamera.GetLookDistance( ) / 10.0f * delta / 120.0f );
	mViewTransform = mCamera.GetViewMatrix( );
}

void DemoApp::OnRender( )
{
	RenderDevice& rd = RenderDevice::Instance( );
	rd.BeginScene( );
	rd.SetClearColor( 0xFF808080 );
	rd.Clear( );

	mWorldTransform = Matrix4( ).SetScaling( 1.3f );
	rd.SetTexture( 0, mTexture );
	rd.SetSamplerState( 0, mSampler );
	mVertexShader->SetMatrix( ShaderBase::_CT_WVP_TRANSFORM, mWorldTransform * mViewTransform * mPerspectTransform );
	mVSContantBuffer->SetConstant( "wvp", mWorldTransform * mViewTransform * mPerspectTransform );
	rd.VSSetConstantBuffer( 0, mVSContantBuffer );
	rd.SetInputLayout( mInputLayout );
	rd.SetVertexBuffer( mVertexBuffer );
	rd.SetIndexBuffer( mIndexBuffer );
	rd.DrawIndex( mIndexBuffer->GetLength( ) / mIndexBuffer->GetSize( ), 0, 0 );

	mWorldTransform = Matrix4( ).SetTrans( Vector3( 0.0f, -1.0f, 0.0f ) );
	rd.SetTexture( 0, mTexture );
	mVertexShader->SetMatrix( ShaderBase::_CT_WVP_TRANSFORM, mWorldTransform * mViewTransform * mPerspectTransform );
	mVSContantBuffer->SetConstant( "wvp", mWorldTransform * mViewTransform * mPerspectTransform );
	rd.VSSetConstantBuffer( 0, mVSContantBuffer );
	rd.SetInputLayout( mInputLayout );
	rd.SetVertexBuffer( mVertexBuffer );
	rd.SetIndexBuffer( mIndexBuffer );
	rd.DrawIndex( mIndexBuffer->GetLength( ) / mIndexBuffer->GetSize( ), 0, 0 );
}

int main( )
{
	DemoApp app( 800, 600 );
	app.Create( );
	app.Run( );

	return 0;
}