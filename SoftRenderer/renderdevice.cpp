#include "renderdevice.h"
#include "point.h"
#include "vector4.h"
#include "color.h"
#include "graphicsbuffer.h"
#include "vector2.h"
#include "texture.h"
#include <algorithm>

RenderDevice::RenderDevice( HWND window, uint* framebuffer ) : mClearColor( 0 ), mVertexShader( nullptr ), mPixelShader( nullptr ), mVertexBuffer( nullptr ), mIndexBuffer( nullptr ), mRenderState( 0 )
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

	memset( mTextures, 0, sizeof( mTextures ) );
}

RenderDevice::~RenderDevice( )
{
	delete[] mFrameBuffer;
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
	uint y = (uint) left->mShaderRigisters[0].y;
	for ( uint x = startx; x < endx; x ++)
	{
		float factor = (float) ( x - startx ) / ( endx - startx );
		PSInput psinput = InterpolatePSInput( left, right, factor );

		float invw = 1.0f / psinput.mShaderRigisters[0].w;
		for ( uint i = 1; i < _MAX_PSINPUT_COUNT; i ++ )
			psinput.mShaderRigisters[i] *= invw;

		// Pixel Shadering.
		Color color;
		float depth = 0.0f;
		mPixelShader->Execute( psinput.mShaderRigisters, color, depth );
		DrawPixel( x, y, color );
		//mFrameBuffer[y][x] = color;
	}
}

// top.y < middle.y = bottom.y
void RenderDevice::DrawStandardTopTriangle( const PSInput* top, const PSInput* middle, const PSInput* bottom )
{
	float x1 = Math::Ceil( top->mShaderRigisters[0].x );
	float x2 = Math::Ceil( middle->mShaderRigisters[0].x );
	float x3 = Math::Ceil( bottom->mShaderRigisters[0].x );
	float ymin, ymax;
	ymin = Math::Ceil( top->mShaderRigisters[0].y );
	ymax = Math::Ceil( bottom->mShaderRigisters[0].y );
	float invheight = 1.0f / (ymax - ymin);

	int ys = (int) ymin;
	int ye = (int) ymax;

	float xs = x1;
	float xe = xs;
	float k1 = (x2 - x1) * invheight;
	float k2 = (x3 - x1) * invheight;
	if ( ymin < 0.0 )
	{
		float factor = -ymin * invheight ;
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

			DrawScanline( &input1, &input2 );
		}
	}
	else
	{
		int xleft, xright;
		for ( int y = ys; y <= ye; y++ )
		{
			float factor = (float) (y - ymin) * invheight;
			PSInput input1 = InterpolatePSInput( top, middle, factor );
			PSInput input2 = InterpolatePSInput( top, bottom, factor );
			xleft = (int) xs;
			xright = (int) xe;

			if ( ( xleft < 0 && xright < 0 ) || ( xleft > mClipXMax && xright > mClipXMax ) == false )
			{
				xleft = Math::Clamp( xleft, 0, mClipXMax );
				xright = Math::Clamp( xright, 0, mClipXMax );

				float invwidth = 1.0f / ( xe - xs );
				PSInput draw1 = InterpolatePSInput( &input1, &input2, ( (float) xleft - xs ) * invwidth );
				PSInput draw2 = InterpolatePSInput( &input1, &input2, ( (float) xright - xs ) * invwidth );

				DrawScanline( &draw1, &draw2 );
			}

			xs += k1;
			xe += k2;
		}
	}
}

// top.y = middle.y < bottom.y
void RenderDevice::DrawStandardBottomTriangle( const PSInput* top, const PSInput* middle, const PSInput* bottom )
{
	float ymin = Math::Ceil( top->mShaderRigisters[0].y );
	float ymax = Math::Ceil( bottom->mShaderRigisters[0].y );

	float x1 = Math::Ceil( top->mShaderRigisters[0].x );
	float x2 = Math::Ceil( middle->mShaderRigisters[0].x );
	float x3 = Math::Ceil( bottom->mShaderRigisters[0].x );

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
		xe += -ymax * k2;
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

			DrawScanline( &input1, &input2 );
		}
	}
	else
	{
		int xleft, xright;
		for ( int y = ys; y <= ye; y++ )
		{
			float factor = (float) (y - ymin) * invheight;
			PSInput input1 = InterpolatePSInput( top, bottom, factor );
			PSInput input2 = InterpolatePSInput( middle, bottom, factor );
			xleft = (int) xs;
			xright = (int) xe;

			if ( ( xleft < 0 && xright < 0 ) || ( xleft > mClipXMax && xright > mClipXMax ) == false )
			{
				xleft = Math::Clamp( xleft, 0, mClipXMax );
				xright = Math::Clamp( xright, 0, mClipXMax );

				float invwidth = 1.0f / ( xe - xs );
				PSInput draw1 = InterpolatePSInput( &input1, &input2, ( (float) xleft - xs ) * invwidth );
				PSInput draw2 = InterpolatePSInput( &input1, &input2, ( (float) xright - xs ) * invwidth );

				DrawScanline( &draw1, &draw2 );
			}

			xs += k1;
			xe += k2;
		}
	}
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

void RenderDevice::Clear( )
{
	for ( uint i = 0; i < mHeight; i ++ )
		memset( mFrameBuffer[i], mClearColor, mWidth * 4 );
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
	if ( mVertexShader != nullptr )
		mVertexShader->SetDevice( this );
	if ( mPixelShader != nullptr )
		mPixelShader->SetDevice( this );
}

void RenderDevice::SetTexture( uint index, Texture* tex )
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

	byte* vb = (byte*) mVertexBuffer->GetBuffer( );
	ushort* ib = (ushort*) mIndexBuffer->GetBuffer( );
	
	uint count = 0;

	indexcount = indexcount - indexcount % 3;
	ushort* ibegin = ib;
	ushort* iend = ib + indexcount;
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
				PSInput& input = mVertexPool[ index ];
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
							input.mShaderRigisters[i] = Vector4( vec3.x, vec3.y, vec3.z, 1.0f );
							vbase += 12;
						}
						else if ( format == GraphicsBuffer::BF_A8R8G8B8 )
						{
							Color c = *(uint*) vbase;
							input.mShaderRigisters[i] = Vector4( c.r, c.g, c.b, c.a );
							vbase += 4;
						}
						else if ( format == GraphicsBuffer::BF_R32G32_FLOAT )
						{
							Vector2& vec2 = *(Vector2*) vbase;
							input.mShaderRigisters[i] = Vector4( vec2.x, vec2.y, 0.0f, 0.0f );
							vbase += 8;
						}
					}

					while ( i < _MAX_VSINPUT_COUNT )
					{
						input.mShaderRigisters[i] = Vector4( 0.0f, 0.0f, 0.0f, 0.0f );
						i ++;
					}
				}

				mVertexShader->Execute( input.mShaderRigisters );

				cache = std::make_pair( index, &input );
				psinputs[k] = &input;
			}
		}

		// Do cull.

		bool infrustum = true;
		for ( uint i = 0; i < 3; i ++ )
		{
			if ( psinputs[i]->mShaderRigisters[0].z < 0 ) //|| psinputs[i]->mShaderRigisters[0].z > psinputs[i]->mShaderRigisters[0].w
			{
				infrustum = false;
				break;
			}
		}

		if ( infrustum )
		{
			// frontface;
			Vector2 v1( psinputs[0]->mShaderRigisters[0].x, psinputs[0]->mShaderRigisters[0].y );
			v1 /= psinputs[0]->mShaderRigisters[0].w;
			Vector2 v2( psinputs[1]->mShaderRigisters[0].x, psinputs[1]->mShaderRigisters[0].y );
			v2 /= psinputs[1]->mShaderRigisters[0].w;
			Vector2 v3( psinputs[2]->mShaderRigisters[0].x, psinputs[2]->mShaderRigisters[0].y );
			v3 /= psinputs[2]->mShaderRigisters[0].w;

			if ( ( v3.x - v1.x ) * ( v3.y - v2.y ) - ( v3.y - v1.y ) * ( v3.x - v2.x ) < 0 )
			{
				mPtrClipedVertex.push_back( psinputs[0] );
				mPtrClipedVertex.push_back( psinputs[1] );
				mPtrClipedVertex.push_back( psinputs[2] );
			}
		}
		else
		{
			PSInput* srcvertexs[3];
			srcvertexs[0] = psinputs[0];
			srcvertexs[1] = psinputs[1];
			srcvertexs[2] = psinputs[2];

			PSInput* clippedvertexs[5];

			float neartest1 = psinputs[0]->mShaderRigisters[0].z;
			uint clipnum = 0;

			for ( uint i = 0, j = 1; i < 3; i ++, j ++ )
			{
				j %= 3;
				if ( neartest1 >= 0.0 )
				{
					clippedvertexs[ clipnum ++ ] = srcvertexs[i];

					if ( srcvertexs[j]->mShaderRigisters[0].z < 0 )
					{
						mClippedVertex.resize( mClippedVertex.size( ) + 1 );
						PSInput* input = &mClippedVertex.back( );

						float factor = neartest1 / ( neartest1 - srcvertexs[j]->mShaderRigisters[0].z );
						*input = InterpolatePSInput( srcvertexs[i], srcvertexs[j], factor );

						clippedvertexs[ clipnum ++ ] = input;
					}
				}
				else
				{
					if ( srcvertexs[j]->mShaderRigisters[0].z >= 0 )
					{
						mClippedVertex.resize( mClippedVertex.size( ) + 1 );
						PSInput* input = &mClippedVertex.back( );

						float factor = srcvertexs[j]->mShaderRigisters[0].z / ( srcvertexs[j]->mShaderRigisters[0].z - neartest1 );
						*input = InterpolatePSInput( srcvertexs[j], srcvertexs[i], factor );

						clippedvertexs[ clipnum ++ ] = input;
					}
				}

				neartest1 = srcvertexs[j]->mShaderRigisters[0].z; 
			}

			if ( clipnum >= 3 )
			{
				for ( uint i = 1; i < clipnum - 1; i ++ )
				{
					mPtrClipedVertex.push_back( clippedvertexs[0] );
					mPtrClipedVertex.push_back( clippedvertexs[ i ] );
					mPtrClipedVertex.push_back( clippedvertexs[ i + 1] );
				}
			}
		}

		// vertexpool.alloc == PsInput input; mVertexPool.push_back(input); vector<psinput*> = mVertexpool.Tail();
		// 被cull的pass，被clip的ClippedVertexs.pushback();没有被处理的也ClippedVertexs.pushback().
		// Fetchvertex 是一个pool, clip也是一个pool,clippedvector里存的都是pool里的指针，所以都是经过vertexshader运算过的。
	}

	vector<PSInput*> sorts;
	sorts.insert( sorts.begin( ), mPtrClipedVertex.begin( ), mPtrClipedVertex.end( ) );
	std::sort(sorts.begin(), sorts.end());
	sorts.erase( std::unique( sorts.begin( ), sorts.end () ), sorts.end( ) );

	for ( uint i = 0; i < sorts.size( ); i ++ )
	{
		PSInput& input = *sorts[i];
		float invw = 1.0f / input.mShaderRigisters[0].w;
		input.mShaderRigisters[0].x *= invw;
		input.mShaderRigisters[0].y *= invw;
		input.mShaderRigisters[0].z *= invw;
		input.mShaderRigisters[0].w = invw;

		for ( uint j = 1; j < _MAX_VSINPUT_COUNT; j ++ )
			input.mShaderRigisters[j] *= invw;
				
		// ToScreen.
		input.mShaderRigisters[0].x = ( 1.0f + input.mShaderRigisters[0].x ) * 0.5f * mWidth;
		input.mShaderRigisters[0].y = ( 1.0f - input.mShaderRigisters[0].y ) * 0.5f * mHeight;
	}

	for ( uint i = 0; i < mPtrClipedVertex.size( ); i += 3 )
		{
			PSInput* top = mPtrClipedVertex[i];
			PSInput* middle = mPtrClipedVertex[i + 1];
			PSInput* bottom = mPtrClipedVertex[i + 2];
	
			const Vector4& v1 = top->mShaderRigisters[0];
			const Vector4& v2 = middle->mShaderRigisters[0];
			const Vector4& v3 = bottom->mShaderRigisters[0];
	
			// BackCulling.
			if ( ( v3.x - v1.x ) * ( v3.y - v2.y ) - ( v3.y - v1.y ) * ( v3.x - v2.x ) < 0 )
				continue;
			
			// top to bottom, value of y is larger.
			if ( top->mShaderRigisters[0].y > middle->mShaderRigisters[0].y )
				Math::Swap( top, middle );
			if ( middle->mShaderRigisters[0].y > bottom->mShaderRigisters[0].y )
				Math::Swap( middle, bottom );
			if ( top->mShaderRigisters[0].y > middle->mShaderRigisters[0].y )
				Math::Swap( top, middle );
	
			if ( mRenderState == _RENDER_SOLID )
			{
				if ( top->mShaderRigisters[0].y == bottom->mShaderRigisters[0].y )
				{
					if ( top->mShaderRigisters[0].x > middle->mShaderRigisters[0].x )
						Math::Swap( top, middle );
					if ( middle->mShaderRigisters[0].x > bottom->mShaderRigisters[0].x )
						Math::Swap( middle, bottom );
					if ( top->mShaderRigisters[0].x > middle->mShaderRigisters[0].x )
						Math::Swap( top, middle );
	
					DrawScanline( top, bottom );
				}
				else
				{
					float factor = ( middle->mShaderRigisters[0].y - top->mShaderRigisters[0].y ) / ( bottom->mShaderRigisters[0].y - top->mShaderRigisters[0].y );
					PSInput newmiddle = InterpolatePSInput( top, bottom, factor );
	
					DrawStandardTopTriangle( top, &newmiddle, middle );
					DrawStandardBottomTriangle( middle, &newmiddle, bottom );
				}
			}
			else if ( mRenderState == _RENDER_WIREFRAME )
			{
				// TODO. linecolor
				DrawLine( Point( (int) top->mShaderRigisters[0].x, (int) top->mShaderRigisters[0].y ), Point( (int) middle->mShaderRigisters[0].x, (int) middle->mShaderRigisters[0].y ), 0xff00ff00 );
				DrawLine( Point( (int) top->mShaderRigisters[0].x, (int) top->mShaderRigisters[0].y ), Point( (int) bottom->mShaderRigisters[0].x, (int) bottom->mShaderRigisters[0].y ), 0xff00ff00 );
				DrawLine( Point( (int) bottom->mShaderRigisters[0].x, (int) bottom->mShaderRigisters[0].y ), Point( (int) middle->mShaderRigisters[0].x, (int) middle->mShaderRigisters[0].y ), 0xff00ff00 );
			}
		}
}