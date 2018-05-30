#pragma once

#include "Prerequisites.h"
#include "Windows.h"
#include "Vector4.h"
#include "Shaders.h"
#include <list>

class RenderDevice
{
private:
	enum EMaxCount
	{
		MC_TEXTURE			= 8,
		MC_CONSTANTBUFFER	= 4,
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

	// Default settings.
	SamplerStatePtr				mDefaultSampler;
	RasterizerStatePtr			mDefaultRS;

	VertexShaderPtr				mVertexShader;
	PixelShaderPtr				mPixelShader;
	InputLayoutPtr				mInputLayout;
	GraphicsBufferPtr			mVertexBuffer;
	GraphicsBufferPtr			mIndexBuffer;

	ConstantBufferPtr			mVSConstantBuffer[ MC_CONSTANTBUFFER ];
	ConstantBufferPtr			mPSConstantBuffer[ MC_CONSTANTBUFFER ];

	RasterizerStatePtr			mRasterizerState;

	// Textures and samplers.
	TexturePtr					mTextures[ MC_TEXTURE ];
	SamplerStatePtr				mSamplers[ MC_TEXTURE ];

private:
	RenderDevice( );

	void	PreparePipeline( );

public:
	~RenderDevice( );

	bool	Init( HWND window, uint * framebuffer );

	// 2D.
	void	FillUniqueTriangle( const Point& p1, const Point&p2, const Point& p3, uint color  );
	void	DrawLine( uint x1, uint y1, uint x2, uint y2, uint color );
	void	DrawClipLine( int x1, int y1, int x2, int y2, uint color );
	bool	ClipLine( int& x1, int& y1, int& x2, int& y2 );

	// 3D.
	bool	DepthTestAndWrite( uint x, uint y, float depth );
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

	void				Clear( );

	// 2D.
	void				DrawPixel( uint x, uint y, uint color );
	void				DrawPoint( const Point& p, uint color );
	void				DrawLine( const Point& p1, const Point& p2, uint color );
	void				FillTriangle( const Point& p1, const Point& p2, const Point& p3, uint color );

	InputLayoutPtr		CreateInputLayout( InputElementDesc const * desc, uint count );
	GraphicsBufferPtr	CreateBuffer( void* buffer, uint length, uint size );

	TexturePtr			CreateTexture2D( uint width, uint height, uint format );
	void				SetTexture( uint i, TexturePtr tex );
	Color				Texture2D( uint index, Vector2 uv );
	Color				Texture2D( uint index, float u, float v );

	SamplerStatePtr		CreateSamplerState( const SamplerStateDesc& desc );
	void				SetSamplerState( uint index, SamplerStatePtr sampler );

	ConstantBufferPtr	CreateConstantBuffer( );
	void				VSSetConstantBuffer( uint index, ConstantBufferPtr bufferptr );
	void				PSSetConstantBuffer( uint index, ConstantBufferPtr bufferptr );

	RasterizerStatePtr	CreateRasterizerState( const RasterizerDesc& desc );
	void				SetRasterizerState( RasterizerStatePtr rs );

	void				DrawIndex( uint indexcount, uint startindex, uint startvertex );
};