#pragma once

#include "Prerequisites.h"
#include "Windows.h"
#include "Vector4.h"
#include "Shaders.h"
#include <vector>
#include <list>
#include <memory>
using namespace std;

class RenderDevice
{
public:
	enum
	{
		_RENDER_SOLID		= 0,
		_RENDER_WIREFRAME	= 1,
	};

private:
	enum
	{
		_MAX_VERTEXCACHE_COUNT		= 64,
		_MAX_TEXTURE_COUNT			= 8,
		_MAX_CONSTANTBUFFER_COUNT	= 4,
	};

	struct RasterizerScanline
	{
		PSInput	left;
		PSInput	right;
		PSInput	leftstep;
		PSInput	rightstep;
		float	ymin;
		float	ymax;
	};

	static std::unique_ptr<RenderDevice>	mInstance;

	uint						mWidth;
	uint						mHeight;
	int							mClipXMax;
	int							mClipYMax;
	uint**						mFrameBuffer;
	float**						mDepthBuffer;
	uint						mClearColor;
	VertexShaderPtr				mVertexShader;
	PixelShaderPtr				mPixelShader;
	InputLayoutPtr				mInputLayout;
	GraphicsBufferPtr			mVertexBuffer;
	GraphicsBufferPtr			mIndexBuffer;

	RasterizerStatePtr			mRasterizerState;

	uint						mVaryingCount;

	// Textures and samplers.
	TexturePtr					mTextures[ _MAX_TEXTURE_COUNT ];
	SamplerStatePtr				mSamplers[ _MAX_TEXTURE_COUNT ];

	// Default settings.
	SamplerStatePtr				mDefaultSampler;
	RasterizerStatePtr			mDefaultRS;

	ConstantBufferPtr			mVSConstantBuffer[ _MAX_CONSTANTBUFFER_COUNT ];
	ConstantBufferPtr			mPSConstantBuffer[ _MAX_CONSTANTBUFFER_COUNT ];

private:
	RenderDevice( );

	void	PreparePipeline( );

public:
	~RenderDevice( );

	bool	Init( HWND window, uint * framebuffer );

	void	FillUniqueTriangle( const Point& p1, const Point&p2, const Point& p3, uint color  );
	void	DrawLine( uint x1, uint y1, uint x2, uint y2, uint color );
	void	DrawClipLine( int x1, int y1, int x2, int y2, uint color );

	bool	DepthTestAndWrite( uint x, uint y, float depth );

	uint	SampleTexture( uint index, float u, float v );

	bool	ClipLine( int& x1, int& y1, int& x2, int& y2 );
	bool	IsFrontFace( const Vector4& v1, const Vector4& v2, const Vector4& v3 );

	void	RasterizeTriangle( const PSInput* v1, const PSInput* v2, const PSInput* v3 );
	void	FillScanline( RasterizerScanline& scanline );
	void	DrawScanline( const PSInput& left, const PSInput& right, int y );

public:
	static RenderDevice& Instance( );

	int GetDeviceWidth( ) const
	{
		return mWidth;
	}
	
	int GetDeviceHeight( ) const
	{
		return mHeight;
	}

	void SetClearColor( uint color )
	{
		mClearColor = color;
	}

	void SetVertexShader( VertexShaderPtr vs )
	{
		mVertexShader = vs;
	}
	
	void SetPixelShader( PixelShaderPtr ps )
	{
		mPixelShader = ps;
	}

	void SetInputLayout( InputLayoutPtr layout )
	{
		mInputLayout = layout;
	}
	void SetVertexBuffer( GraphicsBufferPtr buffer )
	{
		mVertexBuffer = buffer;
	}
	void SetIndexBuffer( GraphicsBufferPtr buffer )
	{
		mIndexBuffer = buffer;
	}

	void Clear( );
	void DrawPixel( uint x, uint y, uint color );
	void DrawPoint( const Point& p, uint color );
	void DrawLine( const Point& p1, const Point& p2, uint color );
	void FillTriangle( const Point& p1, const Point& p2, const Point& p3, uint color );

	InputLayoutPtr		CreateInputLayout( InputElementDesc const * desc, uint count );

	GraphicsBufferPtr	CreateBuffer( void* buffer, uint length, uint size );

	void				BeginScene( );

	void				SetTexture( uint i, TexturePtr tex );

	void				DrawIndex( uint indexcount, uint startindex, uint startvertex );

	TexturePtr			CreateTexture2D( uint width, uint height, uint format );
	SamplerStatePtr		CreateSamplerState( const SamplerStateDesc& desc );
	void				SetSamplerState( uint index, SamplerStatePtr sampler );

	Color				Texture2D( uint index, Vector2 uv );
	Color				Texture2D( uint index, float u, float v );

	ConstantBufferPtr	CreateConstantBuffer( );
	void				VSSetConstantBuffer( uint index, ConstantBufferPtr bufferptr );
	void				PSSetConstantBuffer( uint index, ConstantBufferPtr bufferptr );

	RasterizerStatePtr	CreateRasterizerState( const RasterizerDesc& desc );
	void				SetRasterizerState( RasterizerStatePtr rs );
};