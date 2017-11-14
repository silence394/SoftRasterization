#include "graphicsbuffer.h"
#include "math.h"
#include "common.h"
#include "renderdevice.h"

RenderDevice::RenderDevice( HWND window, unsigned int * framebuffer ) : mClearColor( 0 )
{
	RECT rect = { 0 };
	GetClientRect( window, &rect  );
	
	mWidth = rect.right;
	mHeight = rect.bottom;

	mFrameBuffer = new unsigned int* [ mHeight ];
	for ( unsigned int i = 0; i < mHeight; i ++ )
		mFrameBuffer[i] = (unsigned int*) ( framebuffer + mWidth * i );
}

RenderDevice::~RenderDevice( )
{
	delete[] mFrameBuffer;
}

void RenderDevice::FillUniqueTriangle( const Point& p1, const Point&p2, const Point& p3, unsigned int color )
{
	float k1 = (float) (int) ( p3.x - p1.x ) / (float) (int) ( p3.y - p1.y );
	float k2 = (float) (int) ( p3.x - p2.x ) / (float) (int) ( p3.y - p2.y );
	float xs = (float) p1.x + 0.5f;
	float xe = (float) p2.x + 0.5f;
	int dy = 1;
	if ( p1.y > p3.y )
	{
		dy = -1;
		k1 = -k1;
		k2 = -k2;
	}

	for ( unsigned int y = p1.y; y != p3.y; y += dy )
	{
		DrawLine( (unsigned int) xs, y, (unsigned int) xe, y, color );
		xs += k1;
		xe += k2;
	}

	mFrameBuffer[p3.y][ (unsigned int) xs ] = color;
}

void RenderDevice::DrawLine( unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color )
{
// 	if ( x1 == x2 )
// 	{
// 		int dy = y1 <= y2 ? 1 : -1;
// 		if ( y1 == y2 )
// 			dy = 0;
// 		do
// 		{
// 			mFrameBuffer[y1][x1] = color;
// 			y1 += dy;
// 		}
// 		while( y1 != y2 );
// 	}
	if ( y1 == y2 )
	{
		int dx = x1 <= x2 ? 1 : -1;
		if ( x1 == x2 )
			dx = 0;
		do
		{
			mFrameBuffer[y1][x1] = color;
			x1 += dx;
		}
		while( x1 != x2 );
	}
	else 
	{
		int stepx = x1 <= x2 ? 1 : -1;
		int stepy = y1 <= y2 ? 1 : -1;
		int dx = stepx == 1 ? x2 - x1 : x1 - x2;
		int dy = stepy == 1 ? y2 - y1 : y1 - y2;
		int eps = 0;
		int t;

		if( dx >= dy )
		{
			t = y1;
			for ( int x = x1; x != x2; x += stepx )
			{
				mFrameBuffer[t][x] = color;
				eps += dy;
				if ( ( eps << 1 ) >= dx )
				{
					eps -= dx;
					t += stepy;
				}
			}
		}
		else
		{
			t = x1;
			for ( int y = y1; y != y2; y += stepy )
			{
				mFrameBuffer[y][t] = color;
				eps += dx;
				if ( ( eps << 1 ) >= dy )
				{
					eps -= dy;
					t += stepx;
				}
			}
		}
	}
}

void RenderDevice::Clear( )
{
	for ( unsigned int i = 0; i < mHeight; i ++ )
		memset( mFrameBuffer[i], mClearColor, mWidth * 4 );
}

void RenderDevice::DrawPoint( const Point& p, unsigned int color )
{
	if ( p.x < mWidth && p.y < mHeight )
		mFrameBuffer[p.y][p.x] = color;
}

void RenderDevice::DrawLine( const Point& p1, const Point& p2, unsigned int color )
{
	DrawLine( p1.x, p1.y, p2.x, p2.y, color );
}

void RenderDevice::FillTriangle( const Point& p1, const Point& p2, const Point& p3, unsigned int color )
{
	if ( p1.y == p2.y )
	{
		FillUniqueTriangle( p1, p2, p3, color );
	}
	else if ( p1.y == p3.y )
	{
		FillUniqueTriangle( p1, p3, p2, color );
	}
	else if ( p2.y == p3.y )
	{
		FillUniqueTriangle( p2, p3, p1, color );
	}
	else
	{
		// Middle Point.
		const Point* top = &p1;
		const Point* middle1 = &p2;
		const Point* bottom = &p3;
		if ( top->y < middle1->y )
			Math::Swap( top, middle1 );
		if ( middle1->y < bottom->y )
			Math::Swap( middle1, bottom );
		if ( top->y < middle1->y )
			Math::Swap( top, middle1 );

		Point middle2;
		middle2.y = middle1->y;
		float k = (float) ( top->x - bottom->x ) / (float) ( top->y - bottom->y );
		middle2.x =  (int) ( ( (float) (int) (middle1->y - top->y) ) * k ) + top->x ;

		FillUniqueTriangle( *middle1, middle2, *top, color );
		FillUniqueTriangle( *middle1, middle2, *bottom, color );
	}
}

GraphicsBuffer* RenderDevice::CreateBuffer( uint type, void* buffer, uint length )
{

	return new GraphicsBuffer;
}

void RenderDevice::Releasebuffer( GraphicsBuffer*& buffer )
{
	delete buffer;
	buffer = nullptr;
}