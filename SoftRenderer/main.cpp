#include "libengine.h"
#include <string>
#include "stdio.h"
#include "stdlib.h"
#include <memory>
#include <iostream>

class BaseVertexShader : public IVertexShader
{
	virtual void Execute( VSInput& in, PSInput& out, ConstantBufferPtr* cb )
	{
		out.position( ) = in.attribute( 0 ) * cb[0]->GetConstant<Matrix4>( "wvp" );
	}

	virtual uint GetVaryingCount( ) const
	{
		return 0;
	}
};

class BasePixelShader : public IPixelShader
{
	virtual void Execute( PSInput& in, PSOutput& out, float& depth, ConstantBufferPtr* cb )
	{
		out.color = Color( 1.0f, 1.0f, 1.0f, 1.0f );
	}
};

class BaseLightVSNoneTex : public IVertexShader
{
	virtual void Execute( VSInput& in, PSInput& out, ConstantBufferPtr* cb )
	{
		out.position( ) = in.attribute( 0 ) * cb[0]->GetConstant<Matrix4>( "wvp" );

		// Normal.
		Vector4 normal = in.attribute( 1 );
		normal.w = 0.0f;
		out.varying( 0 ) = normal * cb[0]->GetConstant<Matrix4>( "w" );

		// Vertex position in world space.
		out.varying( 1 ) = in.attribute( 0 ) * cb[0]->GetConstant<Matrix4>( "w" );
	}

	virtual uint GetVaryingCount( ) const
	{
		return 2;
	}
};

class BaseLightPSNoneTex : public IPixelShader
{
	virtual void Execute( PSInput& in, PSOutput& out, float& depth, ConstantBufferPtr* cb )
	{
		Vector3 L = -cb[0]->GetConstant<Vector3>( "skydir" );

		Vector3 N( in.varying( 0 ).x, in.varying( 0 ).y, in.varying( 0 ).z );
		N.Normalize( );

		float skydiffuse = Math::Clamp( Vector3::Dot( N, L ), 0.0f, 1.0f );

		Vector3 worldpos( in.varying( 1 ).x, in.varying( 1 ).y, in.varying( 1 ).z );
		Vector3 viewdir = cb[0]->GetConstant<Vector3>( "eye" ) - worldpos;
		viewdir.Normalize( );
		Vector3 H = ( viewdir + L ).Normalize( );

		float specular = Math::Pow( Math::Clamp( Vector3::Dot( H, N ), 0.0f, 1.0f ), cb[0]->GetConstant<float>( "shiness" ) );

		Color skycolor = cb[0]->GetConstant<Color>( "skycolor" );
		Color ambientcolor = cb[0]->GetConstant<Color>( "ambientcolor" );
		out.color = (ambientcolor + skycolor * (skydiffuse + specular));
	}
};

class DemoApp : public App
{
private:
	Camera				mCamera;
	Matrix4				mWorldTransform;
	Matrix4				mViewTransform;
	Matrix4				mPerspectTransform;

	SamplerStatePtr		mSampler;
	RasterizerStatePtr	mRasterState;

	ConstantBufferPtr	mVSContantBuffer;
	ConstantBufferPtr	mPSConstantBuffer;

	AmbientLight		mAmbientLight;
	SkyLight			mSkyLight;
	Material			mMaterial;

	InputLayoutPtr		mStandardInputLayout;
	InputLayoutPtr		mStandardInputLayoutNoneUV;

	StaticMeshPtr		mCowMesh;
	VertexShaderPtr		mLightNoneTexVS;
	PixelShaderPtr		mLightNoneTexPS;

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

	mCamera.SetPosition( Vector3( 8.0f, 2.0f, 0.0f ) );
	mCamera.LookAt( Vector3( 0.0f, 0.0f, 0.0f ) );

	mViewTransform = mCamera.GetViewMatrix( );
	mPerspectTransform = Matrix4::Perspective( 3.141592654f / 4.0f, (float) rd.GetDeviceWidth( ) / (float) rd.GetDeviceHeight( ), 1.0f, 5000.0f );

	SamplerStateDesc desc;
	desc.address = EAddressMode::AM_CLAMP;
	desc.filter = ESamplerFilter::SF_Linear;
	mSampler = rd.CreateSamplerState( desc );

	mVSContantBuffer = rd.CreateConstantBuffer( );
	mPSConstantBuffer = rd.CreateConstantBuffer( );

	mAmbientLight.color = Color( 0.3f, 0.3f, 0.3f, 1.0f );
	mSkyLight.color = Color( 0.5f, 0.5f, 0.5f, 1.0f );
	mSkyLight.direction = Vector3( -3.0f, -4.0f, 5.0f );

	mMaterial.shiness = 15.0f;

	RasterizerDesc rsdesc;
	rsdesc.fillMode = EFillMode::FM_SOLID;
	rsdesc.cullMode = ECullMode::ECM_BACK;
	mRasterState = rd.CreateRasterizerState( rsdesc );

	std::vector<InputElementDesc> descs;
	descs.push_back( InputElementDesc( "POSITION", GraphicsBuffer::BF_R32B32G32_FLOAT, 0 ) );
	descs.push_back( InputElementDesc( "NORMAL", GraphicsBuffer::BF_R32B32G32_FLOAT, sizeof( Vector3 ) ) );

	mStandardInputLayoutNoneUV = rd.CreateInputLayout( &descs[0], descs.size( ) );

	descs.push_back( InputElementDesc( "TEXCOORD0", GraphicsBuffer::BF_R32G32_FLOAT, sizeof( Vector3 ) + sizeof( Vector3 ) ) );

	mStandardInputLayout = rd.CreateInputLayout( &descs[0], descs.size( ) );

	mCowMesh = ModelManager::Instance( ).LoadModel( std::wstring( L"../Media/OBJ/WusonOBJ.obj" ) );
	mLightNoneTexVS = VertexShaderPtr( new BaseLightVSNoneTex( ) );
	mLightNoneTexPS = PixelShaderPtr( new BaseLightPSNoneTex( ) );
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
	else if ( key == 32 )
		cout << "eye : " << mCamera.GetPosition( ).x << ", " << mCamera.GetPosition( ).y << ", " << mCamera.GetPosition( ).z << ", " << endl;
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

	rd.SetSamplerState( 0, mSampler );
	rd.SetRasterizerState( mRasterState );
	rd.SetVertexShader( mLightNoneTexVS );
	rd.SetPixelShader( mLightNoneTexPS );
	mWorldTransform = Matrix4( ).SetTrans( Vector3( 0.0f, 0.0f, -3.0f ) );
	mVSContantBuffer->SetConstant( "wvp", mWorldTransform * mViewTransform * mPerspectTransform );
	mVSContantBuffer->SetConstant( "w", mWorldTransform );

	rd.VSSetConstantBuffer( 0, mVSContantBuffer );

	mPSConstantBuffer->SetConstant( "ambientcolor", mAmbientLight.color );
	mPSConstantBuffer->SetConstant( "skycolor", mSkyLight.color );
	mPSConstantBuffer->SetConstant( "skydir", mSkyLight.direction.Normalize( ) );
	mPSConstantBuffer->SetConstant( "eye", mCamera.GetPosition( ) );
	mPSConstantBuffer->SetConstant( "shiness", mMaterial.shiness );
	rd.PSSetConstantBuffer( 0, mPSConstantBuffer );
	rd.SetInputLayout( mStandardInputLayoutNoneUV );
	mCowMesh->Draw( );
}

int main( )
{
	DemoApp app( 800, 600 );
	app.Create( );
	app.Run( );

	return 0;
}