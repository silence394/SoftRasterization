#include "renderdevice.h"
#include "point.h"
#include "vector4.h"
#include "color.h"
#include "graphicsbuffer.h"
#include "vector2.h"
#include "texture.h"
#include <algorithm>
#include "RenderStates.h"

std::unique_ptr<RenderDevice> RenderDevice::mInstance = nullptr;
RenderDevice::RenderDevice( ) : mClearColor( 0 ), mWidth( 0 ), mHeight( 0 ), mClipXMax( 0 ), mClipYMax( 0 ), mVertexShader( nullptr ), mPixelShader( nullptr ), mVertexBuffer( nullptr ), mIndexBuffer( nullptr ), mRenderState( 0 )
{
	mDefaultSampler = SamplerStatePtr( new SamplerState( SamplerStateDesc( ) ) );
}

RenderDevice::~RenderDevice( )
{
	delete[] mFrameBuffer;

	for ( uint i = 0; i < mHeight; i ++ )
		delete[] mDepthBuffer[i];
}

bool RenderDevice::Init( HWND window, uint * framebuffer )
{
	RECT rect = { 0 };
	GetClientRect( window, &rect  );

	mWidth = rect.right;
	mHeight = rect.bottom;
	mClipXMax = mWidth - 1;
	mClipYMax = mHeight - 1;

	mFrameBuffer = new uint* [ mHeight ];
	for ( uint i = 0; i < mHeight; i ++ )
		mFrameBuffer[i] = (uint*) ( framebuffer + mWidth * i );

	mDepthBuffer = new float* [ mHeight ];
	for ( uint i = 0; i < mHeight; i ++ )
		mDepthBuffer[i] = new float[ mWidth ];

	return true;
}

void RenderDevice::FillUniqueTriangle( const Point& p1, const Point&p2, const Point& p3, uint color )
{
	float xs, xe, ymin, ymax;
	float k1 = (float) ( p3.x - p1.x ) / (float) ( p3.y - p1.y );
	float k2 = (float) ( p3.x - p2.x ) / (float) ( p3.y - p2.y );
	if ( p1.y < p3.y )
	{
		ymin = (float) p1.y;
		ymax = (float) p3.y;
		xs = (float) p1.x;
		xe = (float) p2.x;
	}
	else
	{
		ymin = (float) p3.y;
		ymax = (float) p1.y;
		xs = (float) p3.x;
		xe = xs;
	}

	int ys = (int) ymin;
	int ye = (int) ymax;
	if ( ymin < 0 )
	{
		xs += k1 * -ymin;
		xe += k2 * -ymin;

		ys = 0;
	}

	if ( ymax > mClipYMax )
		ye = mClipYMax;

	if ( ye < ys )
		return;

	if ( p1.x >=0 && p1.x <= mClipXMax && p2.x >=0 && p2.x <= mClipXMax && p3.x >= 0 && p3.x < mClipXMax )
	{
		for ( int y = ys; y <= ye; y += 1 )
		{
			DrawLine( (uint) xs, y, (uint) xe, y, color );
			xs += k1;
			xe += k2;
		}
	}
	else
	{
		int xleft;
		int xright;
		for ( int y = ys; y <= ye; y += 1 )
		{
			xleft = (int) xs;
			xright = (int) xe;

			xs += k1;
			xe += k2;

			if ( ( xleft < 0 && xright < 0 ) || ( xleft > mClipXMax && xright > mClipXMax ) )
				continue;

			xleft = Math::Clamp( xleft, 0, mClipXMax );
			xright = Math::Clamp( xright, 0, mClipXMax );

			DrawLine( (uint) xleft, y, (uint) xright, y, color );
		}
	}
}

void RenderDevice::DrawLine( uint x1, uint y1, uint x2, uint y2, uint color )
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

void RenderDevice::DrawClipLine( int x1, int y1, int x2, int y2, uint color )
{
	if ( ClipLine( x1, y1, x2, y2 ) )
		DrawLine( x1, y1, x2, y2, color );
}

PSInput RenderDevice::InterpolatePSInput( const PSInput& input1, const PSInput& input2, float factor )
{
	PSInput input;
	input.position( ) = Vector4::Lerp( input1.position( ), input2.position( ), factor );
	for ( uint i = 0; i < _MAX_PSINPUT_COUNT; i ++ )
		input.attribute( i ) = Vector4::Lerp( input1.attribute( i ), input2.attribute( i ), factor );

	return input;
}

void RenderDevice::DrawScanline( PSInput& input1, PSInput& input2 )
{
	PSInput* left = nullptr;
	PSInput* right = nullptr;
	if ( input1.position( ).x < input2.position( ).x )
	{
		left = &input1;
		right = &input2;
	}
	else
	{
		left = &input2;
		right = &input1;
	}

	float xleft = Math::Ceil( left->position( ).x );
	float xright = Math::Ceil( right->position( ).x );
	uint startx = (uint) xleft;
	uint endx = (uint) xright;
	uint y = (uint) Math::Ceil( left->position( ).y );

	Color color;
	float depth = 0.0f;

	if ( startx == endx )
	{
		PSOutput psout;
		mPixelShader->Execute( *left, psout, depth );

		if ( DepthTestAndWrite( startx, y, left->position( ).w ) )
			DrawPixel( startx, y, psout.color );
	}
	else
	{
		float invwidth = 1.0f / ( xright - xleft );
		for ( uint x = startx; x < endx; x ++)
		{
			float factor = ( (float) x - xleft ) * invwidth;
			PSInput psinput = InterpolatePSInput( *left, *right, factor );

			float invw = 1.0f / psinput.position( ).w;
			for ( uint i = 0; i < _MAX_PSINPUT_COUNT; i ++ )
				psinput.attribute( i ) *= invw;

			PSOutput psout;
			mPixelShader->Execute( psinput, psout, depth );

			if ( DepthTestAndWrite( x, y, psinput.position( ).w ) )
				DrawPixel( x, y, psout.color );
		}
	}
}

// top.y < middle.y = bottom.y
void RenderDevice::DrawStandardTopTriangle( PSInput& top, PSInput& middle, PSInput& bottom )
{
	float ymin, ymax;
	ymin = Math::Ceil( top.position( ).y );
	ymax = Math::Ceil( bottom.position( ).y );

	if ( ymin == ymax )
	{
		PSInput* left = &top;
		PSInput* center = &middle;
		PSInput* right = &bottom;
		if ( left->position( ).x > center->position( ).x )
			Math::Swap( left, center );
		if ( center->position( ).x > right->position( ).x )
			Math::Swap( center, right );
		if ( left->position( ).x > middle.position( ).x )
			Math::Swap( left, center );

		DrawScanline( *left, *center );
	}
	else
	{
		float x1 = Math::Ceil( top.position( ).x );
		float x2 = Math::Ceil( middle.position( ).x );
		float x3 = Math::Ceil( bottom.position( ).x );
		float invheight = 1.0f / (ymax - ymin);

		int ys = (int) ymin;
		int ye = (int) ymax;

		float xs = x1;
		float xe = xs;
		float k1 = (x2 - x1) * invheight;
		float k2 = (x3 - x1) * invheight;
		if ( ymin < 0.0 )
		{
			xs += -ymin * k1;
			xe += -ymin * k2;
			ys = 0;
		}

		if ( ymax > mClipYMax )
			ye = mClipYMax;

		if ( ye < ys )
			return;

		if ( x1 >= 0 && x1 <= mClipXMax && x2 >= 0 && x2 <= mClipXMax && x3 >= 0 && x3 <= mClipXMax )
		{
			for ( int y = ys; y <= ye; y ++ )
			{
				float factor = (float) (y - ymin) * invheight;
				PSInput input1 = InterpolatePSInput( top, middle, factor );;
				PSInput input2 = InterpolatePSInput( top, bottom, factor );;

				DrawScanline( input1, input2 );
			}
		}
		else
		{
			for ( int y = ys; y <= ye; y++ )
			{
				float factor = (float) (y - ymin) * invheight;
				PSInput input1 = InterpolatePSInput( top, middle, factor );
				PSInput input2 = InterpolatePSInput( top, bottom, factor );
				
				int xleftint = (int) xs;
				int xrightint = (int) xe;

				if ( ( xleftint < 0 && xrightint < 0 ) || ( xleftint > mClipXMax && xrightint > mClipXMax ) == false )
				{
					float xleft = Math::Clamp( xs, 0.0f, (float) mClipXMax );
					float xright = Math::Clamp( xe, 0.0f, (float) mClipXMax );

					float invwidth = 1.0f / ( xe - xs );
					PSInput draw1 = InterpolatePSInput( input1, input2, ( xleft - xs ) * invwidth );
					PSInput draw2 = InterpolatePSInput( input1, input2, ( xright - xs ) * invwidth );

					DrawScanline( draw1, draw2 );
				}

				xs += k1;
				xe += k2;
			}
		}
	}
}

// top.y = middle.y < bottom.y
void RenderDevice::DrawStandardBottomTriangle( PSInput& top, PSInput& middle, PSInput& bottom )
{
	float ymin = Math::Ceil( top.position( ).y );
	float ymax = Math::Ceil( bottom.position( ).y );

	if ( ymin == ymax )
	{
		PSInput* left = &top;
		PSInput* center = &middle;
		PSInput* right = &bottom;
		if ( left->position( ).x > center->position( ).x )
			Math::Swap( left, center );
		if ( center->position( ).x > right->position( ).x )
			Math::Swap( center, right );
		if ( left->position( ).x > middle.position( ).x )
			Math::Swap( left, center );

		DrawScanline( *left, *center );
	}
	else
	{
		float x1 = Math::Ceil( top.position( ).x );
		float x2 = Math::Ceil( middle.position( ).x );
		float x3 = Math::Ceil( bottom.position( ).x );

		float invheight = 1.0f / ( ymax - ymin );

		float xs = x1;
		float xe = x2;
		float k1 = (x3 - x1) * invheight;
		float k2 = (x3 - x2) * invheight;
		int ys = (int) ymin;
		int ye = (int) ymax;

		if ( ymin < 0.0f )
		{
			xs += -ymin * k1;
			xe += -ymin * k2;
			ys = 0;
		}

		if ( ymax > mClipYMax )
			ye = mClipYMax;

		if ( ye < ys )
			return;

		if ( x1 >= 0 && x1 <= mClipXMax && x2 >= 0 && x2 <= mClipXMax && x3 >= 0 && x3 <= mClipXMax )
		{
			for ( int y = ys; y <= ye; y ++ )
			{
				float factor = (float) (y - ymin) * invheight;
				PSInput input1 = InterpolatePSInput( top, bottom, factor );;
				PSInput input2 = InterpolatePSInput( middle, bottom, factor );;

				DrawScanline( input1, input2 );
			}
		}
		else
		{
			for ( int y = ys; y <= ye; y++ )
			{
				float factor = (float) (y - ymin) * invheight;
				PSInput input1 = InterpolatePSInput( top, bottom, factor );
				PSInput input2 = InterpolatePSInput( middle, bottom, factor );

				int xleftint = (int) xs;
				int xrightint = (int) xe;

				if ( ( xleftint < 0 && xrightint < 0 ) || ( xleftint > mClipXMax && xrightint > mClipXMax ) == false )
				{
					float xleft = Math::Clamp( xs, 0.0f, (float) mClipXMax );
					float xright = Math::Clamp( xe, 0.0f, (float) mClipXMax );

					float invwidth = 1.0f / ( xe - xs );
					PSInput draw1 = InterpolatePSInput( input1, input2, ( xleft - xs ) * invwidth );
					PSInput draw2 = InterpolatePSInput( input1, input2, ( xright - xs ) * invwidth );

					DrawScanline( draw1, draw2 );
				}

				xs += k1;
				xe += k2;
			}
		}
	}
}

bool RenderDevice::DepthTestAndWrite( uint x, uint y, float depth )
{
	if ( mDepthBuffer[ y ][ x ] >= depth )
		return false;

	// DepthWrite
	mDepthBuffer[y][x] = depth;

	return true;
}

uint RenderDevice::SampleTexture( uint index, float u, float v )
{
	if ( index < _MAX_TEXTURE_COUNT && mTextures[ index ] != nullptr )
		return mTextures[index]->GetPixelbyUV( u, v );

	return 0;
}

// Liang-Barsky
bool RenderDevice::ClipLine( int& x1, int& y1, int& x2, int& y2 )
{
	auto ClipTest =[]( float p, float q, float& t1, float& t2 )
	{
		if ( p < 0.0f )
		{
			// 计算从外到内的t.
			float t = q / p;
			if ( t > t2 )
			{
				return false;
			}
			else if ( t > t1 )
			{
				t1 = t;
				return true;
			}
		}
		else if ( p > 0.0f )
		{
			// 计算从内到外的t.
			float t = q / p;
			if ( t < t1 )
			{
				return false;
			}
			else if ( t < t2 )
			{
				t2 = t;
				return true;
			}
			
		}
		else if ( q < 0.0f )
		{
			return false;
		}
		
		return true;
	};

	float dx = float( x2 - x1 );
	float dy = float( y2 - y1 );

	float parray[4];
	float qarray[4];
	parray[0] = -dx;
	parray[1] = dx;
	parray[2] = -dy;
	parray[3] = dy;
	qarray[0] = (float) x1;
	qarray[1] = float( mClipXMax - x1 );
	qarray[2] = (float) y1;
	qarray[3] = float( mClipYMax - y1 );
	float t1 = 0.0f;
	float t2 = 1.0f;
	for ( uint i = 0; i < 4; i ++ )
	{
		if ( ClipTest( parray[i], qarray[i], t1, t2 ) == false )
			return false;
	}
	
	int tx1 = x1, ty1 = y1, tx2 = x2, ty2 = y2;
	x1 = int( tx1 + t1 * dx );
	y1 = int( ty1 + t1 * dy );
	x2 = int( tx1 + t2 * dx );
	y2 = int( ty1 + t2 * dy );

	return true;
}

bool RenderDevice::IsFrontFace( const Vector4& v1, const Vector4& v2, const Vector4& v3 )
{
	return ( v3.x - v1.x ) * ( v3.y - v2.y ) - ( v3.y - v1.y ) * ( v3.x - v2.x ) <= 0;
}

RenderDevice& RenderDevice::Instance( )
{
	if ( mInstance == nullptr )
		mInstance = unique_ptr<RenderDevice>( new RenderDevice( ) );

	return *mInstance;
}

void RenderDevice::Clear( )
{
	for ( uint i = 0; i < mHeight; i ++ )
		memset( mFrameBuffer[i], mClearColor, mWidth * 4 );

	for ( uint j = 0; j < mHeight; j ++ )
	{
		for ( uint i = 0; i < mWidth; i ++ )
			mDepthBuffer[j][i] = 0.0f;
	}
		
}

void RenderDevice::DrawPixel( uint x, uint y, uint color )
{
	if ( x >= 0 && x < mWidth && y >=0 && y < mHeight )
		mFrameBuffer[y][x] = color;
}

void RenderDevice::DrawPoint( const Point& p, uint color )
{
	if ( (uint) p.x < mWidth && (uint) p.y < mHeight )
		mFrameBuffer[p.y][p.x] = color;
}

void RenderDevice::DrawLine( const Point& p1, const Point& p2, uint color )
{
	DrawClipLine( p1.x, p1.y, p2.x, p2.y, color );
}

void RenderDevice::FillTriangle( const Point& p1, const Point& p2, const Point& p3, uint color )
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

	if ( top->y < 0 || bottom->y > mClipYMax || ( top->x < 0 && middle1->x < 0 && bottom->x < 0 ) || ( top->x > mClipXMax && middle1->x > mClipXMax && bottom->x > mClipXMax ) )
		 return;

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
		Point middle2;
		middle2.y = middle1->y;
		float k = (float) ( top->x - bottom->x ) / (float) ( top->y - bottom->y );
		middle2.x =  (int) ( ( (float) (int) (middle1->y - top->y) ) * k ) + top->x ;

		FillUniqueTriangle( *middle1, middle2, *top, color );
		FillUniqueTriangle( *middle1, middle2, *bottom, color );
	}
}

InputLayout* RenderDevice::CreateInputLayout( InputElementDesc const * desc, uint count )
{
	return new InputLayout( desc, count );
}

void RenderDevice::ReleaseInputLayout( InputLayout*& layout )
{
	delete layout;
	layout = nullptr;
}

GraphicsBuffer* RenderDevice::CreateBuffer( void* buffer, uint length, uint size )
{
	return new GraphicsBuffer( buffer, length, size );
}

void RenderDevice::Releasebuffer( GraphicsBuffer*& buffer )
{
	delete buffer;
	buffer = nullptr;
}

void RenderDevice::BeginScene( )
{
}

void RenderDevice::SetTexture( uint index, TexturePtr tex )
{
	if ( index < _MAX_TEXTURE_COUNT )
		mTextures[index] = tex;
}

void RenderDevice::DrawIndex( uint indexcount, uint startindex, uint startvertex )
{
	if ( mVertexBuffer == nullptr || mIndexBuffer == nullptr )
		return;

	// Prepare vertexpool;
	for ( uint i = 0; i < _MAX_VERTEXCACHE_COUNT; i ++ )
		mVertexCache[i] = std::make_pair( -1, nullptr );

	uint vlen = mVertexBuffer->GetLength( );
	uint vsize = mVertexBuffer->GetSize( );
	uint vcount = vlen / vsize;
	mVertexPool.resize( vcount );

	mClippedVertex.clear( );
	mPtrClipedVertex.clear( );
	mWireFrameVertexs.clear( );

	byte* vb = (byte*) mVertexBuffer->GetBuffer( );
	ushort* ib = (ushort*) mIndexBuffer->GetBuffer( );
	
	uint count = 0;

	indexcount = indexcount - indexcount % 3;
	ushort* ibegin = ib;
	ushort* iend = ib + indexcount;

	uint temcout = 0;
	for ( ; ibegin != iend; ibegin += 3 )
	{
		PSInput* psinputs[3];
		for ( uint k = 0; k < 3; k ++ )
		{
			uint index = *( ibegin + k );
			uint key = index % _MAX_VERTEXCACHE_COUNT;
			auto& cache = mVertexCache[ key ];
			if ( cache.first == index )
			{
				psinputs[k] = cache.second;
			}
			else
			{
				// Execute vertex shader.
				VSInput vsinput;
				count ++;
				// Fetch vertex.
				{
					auto& descs = mInputLayout->GetElementDescs( );
					auto iterbegin = descs.begin( );
					auto iterend = descs.end( );
					byte* vbase = vb + index * vsize;
					uint i = 0;
					for ( ; iterbegin != iterend; iterbegin ++, i ++ )
					{
						uint format = iterbegin->mFormat;
						if ( format == GraphicsBuffer::BF_R32B32G32_FLOAT )
						{
							Vector3& vec3 = *(Vector3*) vbase;
							vsinput.attribute( i ) = Vector4( vec3.x, vec3.y, vec3.z, 1.0f );
							vbase += 12;
						}
						else if ( format == GraphicsBuffer::BF_A8R8G8B8 )
						{
							Color c = *(uint*) vbase;
							vsinput.attribute( i ) = Vector4( c.r, c.g, c.b, c.a );
							vbase += 4;
						}
						else if ( format == GraphicsBuffer::BF_R32G32_FLOAT )
						{
							Vector2& vec2 = *(Vector2*) vbase;
							vsinput.attribute( i ) = Vector4( vec2.x, vec2.y, 0.0f, 0.0f );
							vbase += 8;
						}
					}

					while ( i < _MAX_VSINPUT_COUNT )
					{
						vsinput.attribute( i ) = Vector4( 0.0f, 0.0f, 0.0f, 0.0f );
						i ++;
					}
				}

				PSInput& psinput = mVertexPool[ index ];
				mVertexShader->Execute( vsinput, psinput, mVSConstantBuffer );

				cache = std::make_pair( index, &psinput );
				psinputs[k] = &psinput;
			}
		}

		bool infrustum = true;
		for ( uint i = 0; i < 3; i ++ )
		{
			if ( psinputs[i]->position( ).z < 0 )
			{
				infrustum = false;
				break;
			}
		}

		auto IsFrontFace = []( PSInput** inputs )
		{
			Vector2 pv_2d[3];
			for ( uint i = 0; i < 3; i ++ )
			{
				float invw = 1.0f / inputs[i]->position( ).w;
				pv_2d[i].x = inputs[i]->position( ).x * invw;
				pv_2d[i].y = inputs[i]->position( ).y * invw;
			}

			return ( pv_2d[2].x - pv_2d[0].x ) * ( pv_2d[2].y - pv_2d[1].y ) - ( pv_2d[2].y - pv_2d[0].y ) * ( pv_2d[2].x - pv_2d[1].x ) <= 0;
		};

		if ( infrustum )
		{
			if ( IsFrontFace( psinputs ) )
			{
				if ( mRenderState == _RENDER_SOLID )
				{
					mPtrClipedVertex.push_back( psinputs[0] );
					mPtrClipedVertex.push_back( psinputs[1] );
					mPtrClipedVertex.push_back( psinputs[2] );
				}
				else if ( mRenderState == _RENDER_WIREFRAME )
				{
					mWireFrameVertexs.push_back( psinputs[0] );
					mWireFrameVertexs.push_back( psinputs[1] );
					mWireFrameVertexs.push_back( psinputs[1] );
					mWireFrameVertexs.push_back( psinputs[2] );
					mWireFrameVertexs.push_back( psinputs[2] );
					mWireFrameVertexs.push_back( psinputs[0] );
				}
			}
		}
		else
		{
			PSInput* clippedvertexs[5];

			float neartest1 = psinputs[0]->position( ).z;
			uint clipnum = 0;

			for ( uint i = 0, j = 1; i < 3; i ++, j ++ )
			{
				j %= 3;
				if ( neartest1 >= 0.0 )
				{
					clippedvertexs[ clipnum ++ ] = psinputs[i];

					if ( psinputs[j]->position( ).z < 0 )
					{
						mClippedVertex.resize( mClippedVertex.size( ) + 1 );
						PSInput* input = &mClippedVertex.back( );

						float factor = neartest1 / ( neartest1 - psinputs[j]->position( ).z );
						*input = InterpolatePSInput( *psinputs[i], *psinputs[j], factor );

						clippedvertexs[ clipnum ++ ] = input;
					}
				}
				else
				{
					if ( psinputs[j]->position( ).z >= 0 )
					{
						mClippedVertex.resize( mClippedVertex.size( ) + 1 );
						PSInput* input = &mClippedVertex.back( );

						float factor = psinputs[j]->position( ).z / ( psinputs[j]->position( ).z - neartest1 );
						*input = InterpolatePSInput( *psinputs[j], *psinputs[i], factor );

						clippedvertexs[ clipnum ++ ] = input;
					}
				}

				neartest1 = psinputs[j]->position( ).z; 
			}

			if ( clipnum >= 3 )
			{
				if ( IsFrontFace( clippedvertexs ) )
				{
					if ( mRenderState == _RENDER_SOLID )
					{
						for ( uint i = 1; i < clipnum - 1; i ++ )
						{
							mPtrClipedVertex.push_back( clippedvertexs[0] );
							mPtrClipedVertex.push_back( clippedvertexs[ i ] );
							mPtrClipedVertex.push_back( clippedvertexs[ i + 1] );
						}
					}
					else
					{
						for ( uint i = 0; i < clipnum - 1; i ++ )
						{
							mWireFrameVertexs.push_back( clippedvertexs[ i ] );
							mWireFrameVertexs.push_back( clippedvertexs[ i + 1] );
						}

						mWireFrameVertexs.push_back( clippedvertexs[ clipnum - 1 ] );
						mWireFrameVertexs.push_back( clippedvertexs[ 0 ] );
					}
				}
			}
		}
	}

	vector<PSInput*> sorts;
	if ( mRenderState == _RENDER_SOLID )
		sorts.insert( sorts.begin( ), mPtrClipedVertex.begin( ), mPtrClipedVertex.end( ) );
	else if ( mRenderState == _RENDER_WIREFRAME )
		sorts.insert( sorts.begin( ), mWireFrameVertexs.begin( ), mWireFrameVertexs.end( ) );

	std::sort(sorts.begin(), sorts.end());
	sorts.erase( std::unique( sorts.begin( ), sorts.end () ), sorts.end( ) );

	for ( uint i = 0; i < sorts.size( ); i ++ )
	{
		PSInput& input = *sorts[i];
		Vector4& pos = input.position( );
		float invw = 1.0f / pos.w;
		pos.x *= invw;
		pos.y *= invw;
		pos.z *= invw;
		pos.w = invw;

		for ( uint j = 1; j < _MAX_VSINPUT_COUNT; j ++ )
			input.attribute( j ) *= invw;
				
		// ToScreen.
		pos.x = ( 1.0f + input.position( ).x ) * 0.5f * mWidth;
		pos.y = ( 1.0f - input.position( ).y ) * 0.5f * mHeight;
	}

	if ( mRenderState == _RENDER_SOLID )
	{
		for ( uint i = 0; i < mPtrClipedVertex.size( ); i += 3 )
		{
			PSInput* top = mPtrClipedVertex[i];
			PSInput* middle = mPtrClipedVertex[i + 1];
			PSInput* bottom = mPtrClipedVertex[i + 2];

			// top to bottom, value of y is larger.
			if ( top->position( ).y > middle->position( ).y )
				Math::Swap( top, middle );
			if ( middle->position( ).y > bottom->position( ).y )
				Math::Swap( middle, bottom );
			if ( top->position( ).y > middle->position( ).y )
				Math::Swap( top, middle );

			float factor = ( middle->position( ).y - top->position( ).y ) / ( bottom->position( ).y - top->position( ).y );
			PSInput newmiddle = InterpolatePSInput( *top, *bottom, factor );

			DrawStandardTopTriangle( *top, newmiddle, *middle );
			DrawStandardBottomTriangle( *middle, newmiddle, *bottom );
		}
	}
	else if ( mRenderState == _RENDER_WIREFRAME )
	{
		for ( uint i = 0; i < mWireFrameVertexs.size( ); i += 2 )
		{
			const Vector4& v1 = mWireFrameVertexs[i]->position( );
			const Vector4& v2 = mWireFrameVertexs[ i + 1 ]->position( );
			DrawLine( Point( (int) v1.x, (int) v1.y ), Point( (int) v2.x, (int) v2.y ), 0xff00ff00 );
		}
	}
}

TexturePtr RenderDevice::CreateTexture2D( uint width, uint height, uint format )
{
	return TexturePtr( new Texture( width, height, format ) );
}

SamplerStatePtr RenderDevice::CreateSamplerState( const SamplerStateDesc& desc )
{
	return SamplerStatePtr( new SamplerState( desc ) );
}

void RenderDevice::SetSamplerState( uint index, SamplerStatePtr sampler )
{
	assert( index < _MAX_TEXTURE_COUNT );

	mSamplers[index] = sampler;
}

Color RenderDevice::Texture2D( uint index, float u, float v )
{
	assert( index < _MAX_TEXTURE_COUNT && mTextures[index] != nullptr );

	// Address.
	auto Address = [] ( EAddressMode mode, float& texelu, float& texelv )
	{
		if ( mode == EAddressMode::AM_CLAMP )
		{
			texelu = Math::Clamp( texelu, 0.0f, 1.0f );
			texelv = Math::Clamp( texelv, 0.0f, 1.0f );
		}
		else if ( mode == EAddressMode::AM_WRAP )
		{
			texelu = Math::FMod( Math::FMod( texelu, 1.0f ) + 1.0f, 1.0f );
			texelv = Math::FMod( Math::FMod( texelv, 1.0f ) + 1.0f, 1.0f );
		}
	};

	SamplerStateDesc& desc = mSamplers[index] != nullptr ? mSamplers[index]->mDesc : mDefaultSampler->mDesc;
	Address( desc.address, u, v );

	SurfacePtr suf = mTextures[index]->GetSurface( 0 );
	uint sufw = suf->Width( );
	uint sufh = suf->Height( );
	uint mulw = sufw - 1;
	uint mulh = sufh - 1;
	u *= mulw;
	v *= mulh;

	Color samplecolor;

	if ( desc.filter == ESamplerFilter::SF_POINT )
	{
		samplecolor = suf->Sample( (uint) u, (uint) v );
	}
	else if ( desc.filter == ESamplerFilter::SF_Linear )
	{
		int pos0x, pos0y;
		float du, dv, invdu, invdv;
		du = (float) Math::Modf( (double) u, pos0x );
		dv = (float) Math::Modf( (double) v, pos0y );
		invdu = 1.0f - du;
		invdv = 1.0f - dv;

		int pos1x = ( pos0x + 1 ) % sufw;
		int pos1y = ( pos0y + 1 ) % sufh;

		Color c0 = suf->Sample( pos0x, pos0y );
		Color c1 = suf->Sample( pos0x + 1, pos0y );
		Color c2 = suf->Sample( pos0x, pos0y + 1 );
		Color c3 = suf->Sample( pos0x + 1, pos0y + 1 );
		float w0 = invdu * invdv;
		float w1 = du * invdv;
		float w2 = invdu * dv;
		float w3 = du * dv;
		samplecolor = c0 * w0  + c1 * w1 + c2 * w2  + c3 * w3; 
	}

	return samplecolor;
}

ConstantBufferPtr RenderDevice::CreateConstantBuffer( )
{
	return ConstantBufferPtr( new ConstantBuffer( ) );
}

void RenderDevice::VSSetConstantBuffer( uint index, ConstantBufferPtr bufferptr )
{
	assert( index < _MAX_CONSTANTBUFFER_COUNT );

	mVSConstantBuffer[index] = bufferptr;
}

void RenderDevice::PSSetConstantBuffer( uint index, ConstantBufferPtr bufferptr )
{
	assert( index < _MAX_CONSTANTBUFFER_COUNT );

	mPSConstantBuffer[index] = bufferptr;
}