#pragma once

#include "prerequisites.h"
#include "windows.h"
#include "vector4.h"
#include "shaders.h"
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
		_MAX_VERTEXCACHE_COUNT	= 64,
		_MAX_TEXTURE_COUNT		= 8,
	};

	static std::unique_ptr<RenderDevice>	mInstance;

	uint						mWidth;
	uint						mHeight;
	int							mClipXMax;
	int							mClipYMax;
	uint**						mFrameBuffer;
	float**						mDepthBuffer;
	uint						mClearColor;
	IVertexShader*				mVertexShader;
	IPixelShader*				mPixelShader;
	InputLayout*				mInputLayout;
	GraphicsBuffer*				mVertexBuffer;
	GraphicsBuffer*				mIndexBuffer;
	uint						mRenderState;

	std::pair< uint, PSInput* >	mVertexCache[ _MAX_VERTEXCACHE_COUNT ];
	std::vector< PSInput >		mVertexPool;
	std::list<PSInput>			mClippedVertex;
	std::vector< PSInput* >		mPtrClipedVertex;
	std::vector< PSInput* >		mWireFrameVertexs;

	// Textures and samplers.
	TexturePtr					mTextures[ _MAX_TEXTURE_COUNT ];
	SamplerStatePtr				mSamplers[ _MAX_TEXTURE_COUNT ];

	// Default settings.
	SamplerStatePtr				mDefaultSampler;

private:
	RenderDevice( );

public:
	~RenderDevice( );

	bool	Init( HWND window, uint * framebuffer );

	void	FillUniqueTriangle( const Point& p1, const Point&p2, const Point& p3, uint color  );
	void	DrawLine( uint x1, uint y1, uint x2, uint y2, uint color );
	void	DrawClipLine( int x1, int y1, int x2, int y2, uint color );

	PSInput	InterpolatePSInput( const PSInput& input1, const PSInput& input2, float factor );
	void	DrawScanline( PSInput& input1, PSInput& input2 );
	void	DrawStandardTopTriangle( PSInput& top, PSInput& middle, PSInput& bottom );
	void	DrawStandardBottomTriangle( PSInput& top, PSInput& middle, PSInput& bottom );
	bool	DepthTestAndWrite( uint x, uint y, float depth );

	uint	SampleTexture( uint index, float u, float v );

	bool	ClipLine( int& x1, int& y1, int& x2, int& y2 );
	bool	IsFrontFace( const Vector4& v1, const Vector4& v2, const Vector4& v3 );

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

	void SetVertexShader( IVertexShader* vs )
	{
		mVertexShader = vs;
	}
	
	void SetPixelShader( IPixelShader* ps )
	{
		mPixelShader = ps;
	}

	void SetInputLayout( InputLayout* layout )
	{
		mInputLayout = layout;
	}
	void SetVertexBuffer( GraphicsBuffer* buffer )
	{
		mVertexBuffer = buffer;
	}
	void SetIndexBuffer( GraphicsBuffer* buffer )
	{
		mIndexBuffer = buffer;
	}

	inline void SetRenderState( uint state )
	{
		mRenderState = state;
	}

	void Clear( );
	void DrawPixel( uint x, uint y, uint color );
	void DrawPoint( const Point& p, uint color );
	void DrawLine( const Point& p1, const Point& p2, uint color );
	void FillTriangle( const Point& p1, const Point& p2, const Point& p3, uint color );

	InputLayout*	CreateInputLayout( InputElementDesc const * desc, uint count );
	void			ReleaseInputLayout( InputLayout*& layout );

	GraphicsBuffer*	CreateBuffer( void* buffer, uint length, uint size );
	void			Releasebuffer( GraphicsBuffer*& buffer );

	void			BeginScene( );

	void			SetTexture( uint i, TexturePtr tex );

	void			DrawIndex( uint indexcount, uint startindex, uint startvertex );

	TexturePtr		CreateTexture2D( uint width, uint height, uint format );
	SamplerStatePtr	CreateSamplerState( const SamplerStateDesc& desc );
	void			SetSamplerState( uint index, SamplerStatePtr sampler );

	Color			Texture2D( uint index, float u, float v );
};