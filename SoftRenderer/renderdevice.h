#pragma once
#include "windows.h"

#include "Point.h"
#include "math.h"
#include "common.h"

class GraphicsBuffer;
class RenderDevice
{
private:
	unsigned int	mWidth;
	unsigned int	mHeight;
	unsigned int**	mFrameBuffer;
	uint			mClearColor;

public:
	RenderDevice( HWND window, unsigned int * framebuffer );
	~RenderDevice( );

	void FillUniqueTriangle( const Point& p1, const Point&p2, const Point& p3, unsigned int color  );
	void DrawLine( unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color );

public:
	inline int GetDeviceWidth( ) const
		{ return mWidth; }
	inline int GetDeviceHeight( ) const
		{ return mHeight; }

	inline void SetClearColor( unsigned int color )
		{ mClearColor = color; }

	void Clear( );
	void DrawPoint( const Point& p, unsigned int color );
	void DrawLine( const Point& p1, const Point& p2, unsigned int color );
	void FillTriangle( const Point& p1, const Point& p2, const Point& p3, unsigned int color );

	GraphicsBuffer*	CreateBuffer( uint type, void* buffer, uint length );
	void			Releasebuffer( GraphicsBuffer*& buffer );
};