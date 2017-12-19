#pragma once

#include "windows.h"
#include "point.h"
#include "vector4.h"
#include "graphicsbuffer.h"

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
	uint	mWidth;
	uint	mHeight;
	uint**	mFrameBuffer;
	uint	mClearColor;
	void*	mVertexShader;
	void*	mPixelShader;

public:
	RenderDevice( HWND window, unsigned int * framebuffer );
	~RenderDevice( );

	void	FillUniqueTriangle( const Point& p1, const Point&p2, const Point& p3, unsigned int color  );
	void	DrawLine( unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color );
	PSInput	InterpolatePSInput( const PSInput* input1, const PSInput* input2, float factor );
	void	DrawScanline( const PSInput* input1, const PSInput* input2 );
	void	DrawStandardTriangle( const PSInput* top, const PSInput* middle, const PSInput* bottom );

public:
	inline int GetDeviceWidth( ) const
		{ return mWidth; }
	inline int GetDeviceHeight( ) const
		{ return mHeight; }

	inline void SetClearColor( unsigned int color )
		{ mClearColor = color; }

	void Clear( );
	void DrawPixel( uint x, uint y, uint color );
	void DrawPoint( const Point& p, uint color );
	void DrawLine( const Point& p1, const Point& p2, unsigned int color );
	void FillTriangle( const Point& p1, const Point& p2, const Point& p3, unsigned int color );

	GraphicsBuffer*	CreateBuffer( uint type, void* buffer, uint length );
	void			Releasebuffer( GraphicsBuffer*& buffer );
};