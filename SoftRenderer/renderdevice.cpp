#include "windows.h"
#include "stdlib.h"
#include "common.h"
#include "math.h"
#include "vector4.h"
#include "graphicsbuffer.h"
#include "Point.h"
#include "renderdevice.h"

RenderDevice::RenderDevice( HWND window, unsigned int * framebuffer ) : mClearColor( 0 ), mVertexShader( nullptr ), mPixelShader( nullptr )
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

PSInput RenderDevice::InterpolatePSInput( const PSInput* input1, const PSInput* input2, float factor )
{
	PSInput input;
	for ( uint i = 0; i < _MAX_PSINPUT_COUNT; i ++ )
		input.mShaderRigisters[i] = Vector4::Lerp( input1->mShaderRigisters[i], input2->mShaderRigisters[i], factor );

	return input;
}

void RenderDevice::DrawScanline( const PSInput* input1, const PSInput* input2 )
{
	const PSInput* left = nullptr;
	const PSInput* right = nullptr;
	if ( input1->mShaderRigisters[0].x < input2->mShaderRigisters[0].x )
	{
		left = input1;
		right = input2;
	}
	else
	{
		left = input2;
		right = input1;
	}

	uint startx = (uint) left->mShaderRigisters[0].x;
	uint endx = (uint) right->mShaderRigisters[0].x;
	for ( uint x = startx; x < endx; x ++)
	{
		float factor = (float) ( x - startx ) / ( endx - startx );
		PSInput psinput = InterpolatePSInput( left, right, factor );

		float invw = psinput.mShaderRigisters[0].w;
		for ( uint i = 1; i < _MAX_PSINPUT_COUNT; i ++ )
			psinput.mShaderRigisters[i] *= invw;

		// Pixel Shadering.
	}
}

void RenderDevice::DrawStandardTriangle( const PSInput* top, const PSInput* middle, const PSInput* bottom )
{
	float factor = ( middle->mShaderRigisters[0].y - top->mShaderRigisters[0].y ) / ( bottom->mShaderRigisters[0].y - top->mShaderRigisters[0].y );

	const Vector4* topreg = top->mShaderRigisters;
	const Vector4* midreg = middle->mShaderRigisters;
	const Vector4* btmreg = bottom->mShaderRigisters;

	uint starty = (uint) topreg[0].y;
	uint endy = (uint) midreg[0].y;
	for ( uint y = starty; y < endy; y ++ )
	{
		float factor = (float) ( y - starty ) / ( endy - starty );
		PSInput input1 = InterpolatePSInput( top, middle, factor );
		PSInput input2 = InterpolatePSInput( top, bottom, factor );

		DrawScanline( &input1, &input2 );
	}
}

void RenderDevice::Clear( )
{
	for ( unsigned int i = 0; i < mHeight; i ++ )
		memset( mFrameBuffer[i], mClearColor, mWidth * 4 );
}

void RenderDevice::DrawPixel( uint x, uint y, uint color )
{
	if ( x >= 0 && x < mWidth && y >=0 && y < mHeight )
		mFrameBuffer[y][x] = color;
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