#include "libengine.h"
#include <string>
#include "stdio.h"
#include "stdlib.h"
#include <memory>
#include <iostream>
#include "Assimp/Importer.hpp"
#include "Assimp/scene.h"
#include "Assimp/postprocess.h"

class BaseVertexShader : public IVertexShader
{
	virtual void Execute( VSInput& in, PSInput& out, ConstantBufferPtr* cb )
	{
		out.position( ) = in.attribute( 0 ) * cb[0]->GetConstant<Matrix4>( "wvp" );
	}
};

class BasePixelShader : public IPixelShader
{
	virtual void Execute( PSInput& in, PSOutput& out, float& depth, ConstantBufferPtr* cb )
	{
		out.color = Color( 1.0f, 1.0f, 1.0f, 1.0f );
	}
};

class VertexShader : public IVertexShader
{
	virtual void Execute( VSInput& in, PSInput& out, ConstantBufferPtr* cb )
	{
		out.position( ) = in.attribute( 0 ) * cb[0]->GetConstant<Matrix4>( "wvp" );
		out.varying( 0 ) = in.attribute( 2 );
		out.varying( 1 ) = in.attribute( 1 ) * cb[0]->GetConstant<Matrix4>( "w" );
		Vector4 worldpos = in.attribute( 0 ) * cb[0]->GetConstant<Matrix4>( "w" );
		out.varying( 2 ) =  Vector4( cb[0]->GetConstant<Vector3>( "eye" ), 1.0f ) - worldpos;
	}
};

class PixelShader : public IPixelShader
{
	virtual void Execute( PSInput& in, PSOutput& out, float& depth, ConstantBufferPtr* cb )
	{
		Vector2 uv = Vector2( in.varying( 0 ).x, in.varying( 0 ).y );

		Vector3 L = -cb[0]->GetConstant<Vector3>( "skydir" );

		Vector3 N( in.varying( 1 ).x, in.varying( 1 ).y, in.varying( 1 ).z );
		N.Normalize( );
		float skydiffuse = Math::Clamp( Vector3::Dot( N, L ), 0.0f, 1.0f );

		Vector3 viewdir( in.varying( 2 ).x, in.varying( 2 ).y, in.varying( 2 ).z );
		viewdir.Normalize( );
		Vector3 H = ( viewdir + L ).Normalize( );

		float specular = Math::Pow( Math::Max( 0.0f, Vector3::Dot( H, N ) ), cb[0]->GetConstant<float>( "shiness" ) );

		
		Color skycolor = cb[0]->GetConstant<Color>( "skycolor" );
		Color ambientcolor = cb[0]->GetConstant<Color>( "ambientcolor" );
		Color albedo = Texture2D( 0, uv );
		out.color = (ambientcolor + skycolor * (skydiffuse + specular)) * albedo;
	}
};

class DemoApp : public App
{
private:
	Camera				mCamera;
	Matrix4				mWorldTransform;
	Matrix4				mViewTransform;
	Matrix4				mPerspectTransform;
	TexturePtr			mTexture;
	TexturePtr			mNormalTexture;
	SamplerStatePtr		mSampler;
	RasterizerStatePtr	mRasterState;

	InputLayoutPtr		mInputLayout;
	VertexShaderPtr		mVertexShader;
	PixelShaderPtr		mPixelShader;

	VertexShaderPtr		mBaseVS;
	PixelShaderPtr		mBasePS;

	GraphicsBufferPtr	mVertexBuffer;
	GraphicsBufferPtr	mIndexBuffer;

	ConstantBufferPtr	mVSContantBuffer;
	ConstantBufferPtr	mPSConstantBuffer;

	AmbientLight		mAmbientLight;
	SkyLight			mSkyLight;
	Material			mMaterial; 

public:
	DemoApp( int width = 800, int height = 600, LPCWSTR name = L"Demo" )
		: App( width, height, name ) { }

public:
	virtual void OnCreate( );
	virtual void OnRender( );
	virtual void OnClose( );

	virtual void OnKeyDown( uint key );
	virtual void OnMouseMove( int x, int y );
	virtual void OnMouseWheel( int delta );
};

void DemoApp::OnCreate( )
{
	RenderDevice& rd = RenderDevice::Instance( );

	mCamera.SetPosition( Vector3( 5.0f, 5.0f, 5.0f ) );
	mCamera.LookAt( Vector3( 0.0f, 0.0f, 0.0f ) );

	mViewTransform = mCamera.GetViewMatrix( );
	mPerspectTransform = Matrix4::Perspective( 3.141592654f / 4.0f, (float) rd.GetDeviceWidth( ) / (float) rd.GetDeviceHeight( ), 1.0f, 5000.0f );

	mTexture = TextureManager::Instance( ).Load( L"../Media/stone_color.jpg" );
	mNormalTexture = TextureManager::Instance( ).Load( L"../Media/stone_normal.jpg" );

	ModelManager::Instance( ).LoadModel( std::wstring( L"../Media/OBJ/cube.obj" ) );

	mBaseVS = VertexShaderPtr( new BaseVertexShader( ) );
	mBasePS = PixelShaderPtr( new BasePixelShader( ) );

	SamplerStateDesc desc;
	desc.address = EAddressMode::AM_CLAMP;
	desc.filter = ESamplerFilter::SF_Linear;
	mSampler = rd.CreateSamplerState( desc );

	mPixelShader = PixelShaderPtr( new PixelShader( ) );
	rd.SetPixelShader( mPixelShader );

	mVertexShader = VertexShaderPtr( new VertexShader( ) );

	std::vector<InputElementDesc> descs;
	descs.push_back( InputElementDesc( "POSITION", GraphicsBuffer::BF_R32B32G32_FLOAT, 0 ) );
	descs.push_back( InputElementDesc( "NORMAL", GraphicsBuffer::BF_R32B32G32_FLOAT, sizeof( Vector3 ) ) );
	descs.push_back( InputElementDesc( "TEXCOORD0", GraphicsBuffer::BF_R32G32_FLOAT, sizeof( Vector3 ) + sizeof( Vector3 ) ) );

	mInputLayout = rd.CreateInputLayout( &descs[0], descs.size( ) );

	struct Vertex
	{
		Vector3	pos;
		Vector3	normal;
		Vector2	texcoord;
	};

	Vertex vertex[24] = 
	{
		// X +.
		{ Vector3(  1.0f,  1.0f,  1.0f ), Vector3( 1.0f, 0.0f, 0.0f ), Vector2( 0.0f, 0.0f ) },
		{ Vector3(  1.0f,  1.0f, -1.0f ), Vector3( 1.0f, 0.0f, 0.0f ), Vector2( 1.0f, 0.0f ) },
		{ Vector3(  1.0f, -1.0f, -1.0f ), Vector3( 1.0f, 0.0f, 0.0f ), Vector2( 1.0f, 1.0f ) },
		{ Vector3(  1.0f, -1.0f,  1.0f ), Vector3( 1.0f, 0.0f, 0.0f ), Vector2( 0.0f, 1.0f ) },

		// X -.
		{ Vector3( -1.0f, -1.0f,  1.0f ), Vector3( -1.0f, 0.0f, 0.0f ), Vector2( 0.0f, 0.0f )},
		{ Vector3( -1.0f, -1.0f, -1.0f ), Vector3( -1.0f, 0.0f, 0.0f ), Vector2( 1.0f, 0.0f ) },
		{ Vector3( -1.0f,  1.0f, -1.0f ), Vector3( -1.0f, 0.0f, 0.0f ), Vector2( 1.0f, 1.0f ) },
		{ Vector3( -1.0f,  1.0f,  1.0f ), Vector3( -1.0f, 0.0f, 0.0f ), Vector2( 0.0f, 1.0f ) },

		// Y +.
		{ Vector3( -1.0f,  1.0f,  1.0f ), Vector3( 0.0f, 1.0f, 0.0f ), Vector2( 0.0f, 0.0f ) },
		{ Vector3( -1.0f,  1.0f, -1.0f ), Vector3( 0.0f, 1.0f, 0.0f ), Vector2( 1.0f, 0.0f ) },
		{ Vector3(  1.0f,  1.0f, -1.0f ), Vector3( 0.0f, 1.0f, 0.0f ), Vector2( 1.0f, 1.0f ) },
		{ Vector3(  1.0f,  1.0f,  1.0f ), Vector3( 0.0f, 1.0f, 0.0f ), Vector2( 0.0f, 1.0f ) },

		// Y -.
		{ Vector3(  1.0f, -1.0f,  1.0f ), Vector3( 0.0f, -1.0f, 0.0f ), Vector2( 0.0f, 0.0f ) },
		{ Vector3(  1.0f, -1.0f, -1.0f ), Vector3( 0.0f, -1.0f, 0.0f ), Vector2( 1.0f, 0.0f ) },
		{ Vector3( -1.0f, -1.0f, -1.0f ), Vector3( 0.0f, -1.0f, 0.0f ), Vector2( 1.0f, 1.0f ) },
		{ Vector3( -1.0f, -1.0f,  1.0f ), Vector3( 0.0f, -1.0f, 0.0f ), Vector2( 0.0f, 1.0f ) },

		// Z +.
		{ Vector3( -1.0f, -1.0f,  1.0f ), Vector3( 0.0f, 0.0f, 1.0f ), Vector2( 0.0f, 0.0f ) },
		{ Vector3( -1.0f,  1.0f,  1.0f ), Vector3( 0.0f, 0.0f, 1.0f ), Vector2( 1.0f, 0.0f ) },
		{ Vector3(  1.0f,  1.0f,  1.0f ), Vector3( 0.0f, 0.0f, 1.0f ), Vector2( 1.0f, 1.0f ) },
		{ Vector3(  1.0f, -1.0f,  1.0f ), Vector3( 0.0f, 0.0f, 1.0f ), Vector2( 0.0f, 1.0f ) },

		// Z -.
		{ Vector3( -1.0f,  1.0f, -1.0f ), Vector3( 0.0f, 0.0f, -1.0f ), Vector2( 0.0f, 0.0f ) },
		{ Vector3( -1.0f, -1.0f, -1.0f ), Vector3( 0.0f, 0.0f, -1.0f ), Vector2( 1.0f, 0.0f ) },
		{ Vector3(  1.0f, -1.0f, -1.0f ), Vector3( 0.0f, 0.0f, -1.0f ), Vector2( 1.0f, 1.0f ) },
		{ Vector3(  1.0f,  1.0f, -1.0f ), Vector3( 0.0f, 0.0f, -1.0f ), Vector2( 0.0f, 1.0f ) },
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

	mAmbientLight.color = Color( 0.2f, 0.2f, 0.2f, 1.0f );
	mSkyLight.color = Color( 0.0f, 1.0f, 0.0f, 1.0f );
	mSkyLight.direction = Vector3( -3.0f, -4.0f, -5.0f );

	mPSConstantBuffer->AddConstant( "ambientcolor", mAmbientLight.color );
	mPSConstantBuffer->AddConstant( "skycolor", mSkyLight.color );
	mPSConstantBuffer->AddConstant( "skydir", mSkyLight.direction.Normalize( ) );

	mMaterial.shiness = 1.0f;

	mPSConstantBuffer->AddConstant( "shiness", mMaterial.shiness );

	RasterizerDesc rsdesc;
	rsdesc.fillMode = EFillMode::FM_SOLID;
	rsdesc.cullMode = ECullMode::ECM_BACK;
	mRasterState = rd.CreateRasterizerState( rsdesc );
}

void DemoApp::OnClose( )
{
}

void DemoApp::OnKeyDown( uint key )
{
	if ( key == 'Q' )
		mRasterState->SetFillMode( EFillMode::FM_WIREFRAME );
	else if ( key == 'A' )
		mRasterState->SetFillMode( EFillMode::FM_SOLID );
	else if ( key == 'W' )
		mRasterState->SetCullMode( ECullMode::ECM_FRONT );
	else if ( key == 'S' )
		mRasterState->SetCullMode( ECullMode::ECM_BACK );
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

	rd.SetTexture( 0, mTexture );
	rd.SetSamplerState( 0, mSampler );
	rd.SetTexture( 1, mNormalTexture );
	rd.SetSamplerState( 1, mSampler );
	rd.SetRasterizerState( mRasterState );
	rd.SetVertexShader( mVertexShader );
	rd.SetPixelShader( mPixelShader );
	rd.SetShaderVaryingCount( 3 );
	mWorldTransform = Matrix4( ).SetScaling( 1.0f );
	mVSContantBuffer->SetConstant( "wvp", mWorldTransform * mViewTransform * mPerspectTransform );
	mVSContantBuffer->SetConstant( "w", mWorldTransform );
	mVSContantBuffer->SetConstant( "eye", mCamera.GetPosition( ) );
	rd.VSSetConstantBuffer( 0, mVSContantBuffer );
	
	rd.PSSetConstantBuffer( 0, mPSConstantBuffer );
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