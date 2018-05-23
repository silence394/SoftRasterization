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
		//Vector2 uv = Vector2( in.varying( 0 ).x, in.varying( 0 ).y );

		//Vector3 L = -cb[0]->GetConstant<Vector3>( "skydir" );

		//Vector3 N( in.varying( 1 ).x, in.varying( 1 ).y, in.varying( 1 ).z );
		//N.Normalize( );
		//float skydiffuse = Math::Clamp( Vector3::Dot( N, L ), 0.0f, 1.0f );

		//Vector3 viewdir( in.varying( 2 ).x, in.varying( 2 ).y, in.varying( 2 ).z );
		//viewdir.Normalize( );
		//Vector3 H = ( viewdir + L ).Normalize( );

		//float specular = Math::Pow( Math::Max( 0.0f, Vector3::Dot( H, N ) ), cb[0]->GetConstant<float>( "shiness" ) );

		//
		//Color skycolor = cb[0]->GetConstant<Color>( "skycolor" );
		//Color ambientcolor = cb[0]->GetConstant<Color>( "ambientcolor" );
		//Color albedo = Texture2D( 0, uv );
		out.color = Color( 1.0f, 1.0f, 1.0f, 1.0f );//(ambientcolor + skycolor * (skydiffuse + specular)) * albedo;
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

	StaticMeshPtr		mCube;

	VertexShaderPtr		mBaseVS;
	PixelShaderPtr		mBasePS;
	InputLayoutPtr		mBaseInputLayout;
	StaticMeshPtr		mBaseBox;

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

	mCamera.SetPosition( Vector3( 8.34752f, 2.22418f, 0.609865f ) );
	mCamera.LookAt( Vector3( 0.0f, 0.0f, 0.0f ) );

	mViewTransform = mCamera.GetViewMatrix( );
	mPerspectTransform = Matrix4::Perspective( 3.141592654f / 4.0f, (float) rd.GetDeviceWidth( ) / (float) rd.GetDeviceHeight( ), 1.0f, 5000.0f );

	mTexture = TextureManager::Instance( ).Load( L"../Media/stone_color.jpg" );
	mNormalTexture = TextureManager::Instance( ).Load( L"../Media/stone_normal.jpg" );

	mBaseBox = ModelManager::Instance( ).LoadModel( std::wstring( L"../Media/OBJ/empty_mat.obj" ) );

	mBaseVS = VertexShaderPtr( new BaseVertexShader( ) );
	mBasePS = PixelShaderPtr( new BasePixelShader( ) );
	mBaseVS->SetVaryingCount( 0 );

	std::vector<InputElementDesc> eledesc;
	eledesc.push_back( InputElementDesc( "POSITION", GraphicsBuffer::BF_R32B32G32_FLOAT, 0 ) );
	mBaseInputLayout = rd.CreateInputLayout( &eledesc[0], eledesc.size( ) );

	SamplerStateDesc desc;
	desc.address = EAddressMode::AM_CLAMP;
	desc.filter = ESamplerFilter::SF_Linear;
	mSampler = rd.CreateSamplerState( desc );

	mVertexShader = VertexShaderPtr( new VertexShader( ) );
	mVertexShader->SetVaryingCount( 3 );

	mPixelShader = PixelShaderPtr( new PixelShader( ) );

	std::vector<InputElementDesc> descs;
	descs.push_back( InputElementDesc( "POSITION", GraphicsBuffer::BF_R32B32G32_FLOAT, 0 ) );
	descs.push_back( InputElementDesc( "NORMAL", GraphicsBuffer::BF_R32B32G32_FLOAT, sizeof( Vector3 ) ) );
	descs.push_back( InputElementDesc( "TEXCOORD0", GraphicsBuffer::BF_R32G32_FLOAT, sizeof( Vector3 ) + sizeof( Vector3 ) ) );

	mInputLayout = rd.CreateInputLayout( &descs[0], descs.size( ) );

	mCube = ModelManager::Instance( ).CreateBox( );

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

	{
		//rd.SetTexture( 0, mTexture );
		//rd.SetSamplerState( 0, mSampler );
		//rd.SetTexture( 1, mNormalTexture );
		//rd.SetSamplerState( 1, mSampler );
		//rd.SetRasterizerState( mRasterState );
		//rd.SetVertexShader( mVertexShader );
		//rd.SetPixelShader( mPixelShader );
		//mWorldTransform = Matrix4( ).SetScaling( 1.0f );
		//mVSContantBuffer->SetConstant( "wvp", mWorldTransform * mViewTransform * mPerspectTransform );
		//mVSContantBuffer->SetConstant( "w", mWorldTransform );
		//mVSContantBuffer->SetConstant( "eye", mCamera.GetPosition( ) );
		//rd.VSSetConstantBuffer( 0, mVSContantBuffer );

		//rd.PSSetConstantBuffer( 0, mPSConstantBuffer );
		//rd.SetInputLayout( mInputLayout );
		//mCube->Draw( );
	}

	{
		rd.SetRasterizerState( mRasterState );
		rd.SetVertexShader( mBaseVS );
		rd.SetPixelShader( mBasePS );
		rd.SetInputLayout( mBaseInputLayout );
		mWorldTransform = Matrix4( ).SetTrans( Vector3( 2.0f, 0.0f, 0.0f ) );
		mVSContantBuffer->SetConstant( "wvp", mWorldTransform * mViewTransform * mPerspectTransform );
		rd.VSSetConstantBuffer( 0, mVSContantBuffer );

		mBaseBox->Draw( );
	}
}

int main( )
{
	DemoApp app( 800, 600 );
	app.Create( );
	app.Run( );

	return 0;
}