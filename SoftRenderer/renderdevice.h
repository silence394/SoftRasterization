#pragma once

#include "prerequisites.h"
#include "windows.h"
#include "vector4.h"
#include "shaders.h"
#include <vector>

#define _MAX_VSINPUT_COUNT 4
#define _MAX_PSINPUT_COUNT 4

struct VSInput
{
	Vector4	mShaderRigisters[ _MAX_VSINPUT_COUNT ];
};

struct PSInput
{
	Vector4 mShaderRigisters[ _MAX_PSINPUT_COUNT ];
};

class RenderDevice
{
private:
	enum
	{
		_MAX_VERTEXCACHE_COUNT	= 64,
		_MAX_TEXTURE_COUNT		= 8,
	};

	uint						mWidth;
	uint						mHeight;
	uint**						mFrameBuffer;
	uint						mClearColor;
	IVertexShader*				mVertexShader;
	IPixelShader*				mPixelShader;
	InputLayout*				mInputLayout;
	GraphicsBuffer*				mVertexBuffer;
	GraphicsBuffer*				mIndexBuffer;

	std::pair< uint, PSInput* >	mVertexCache[ _MAX_VERTEXCACHE_COUNT ];
	std::vector< PSInput >		mVertexPool;
	Texture*					mTextures[ _MAX_TEXTURE_COUNT ];

public:
	RenderDevice( HWND window, uint * framebuffer );
	~RenderDevice( );

	void	FillUniqueTriangle( const Point& p1, const Point&p2, const Point& p3, uint color  );
	void	DrawLine( uint x1, uint y1, uint x2, uint y2, uint color );
	PSInput	InterpolatePSInput( const PSInput* input1, const PSInput* input2, float factor );
	void	DrawScanline( const PSInput* input1, const PSInput* input2 );
	void	DrawStandardTopTriangle( const PSInput* top, const PSInput* middle, const PSInput* bottom );
	void	DrawStandardBottomTriangle( const PSInput* top, const PSInput* middle, const PSInput* bottom );

	uint	SampleTexture( uint index, float u, float v );

public:
	inline int GetDeviceWidth( ) const
		{ return mWidth; }
	inline int GetDeviceHeight( ) const
		{ return mHeight; }

	inline void SetClearColor( uint color )
		{ mClearColor = color; }

	inline void SetVertexShader( IVertexShader* vs )
		{ mVertexShader = vs; }
	inline void SetPixelShader( IPixelShader* ps )
		{ mPixelShader = ps; }

	inline void SetInputLayout( InputLayout* layout )
		{ mInputLayout = layout; }
	inline void SetVertexBuffer( GraphicsBuffer* buffer )
		{ mVertexBuffer = buffer; }
	inline void SetIndexBuffer( GraphicsBuffer* buffer )
		{ mIndexBuffer = buffer; }

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

	void			SetTexture( uint i, Texture* tex );

	void			DrawIndex( uint indexcount, uint startindex, uint startvertex );
};