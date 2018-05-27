#include "RenderDevice.h"
#include "Point.h"
#include "Vector4.h"
#include "Color.h"
#include "GraphicsBuffer.h"
#include "Vector2.h"
#include "Texture.h"
#include <algorithm>
#include "RenderStates.h"
#include "VertexProcessEngine.h"

std::unique_ptr<RenderDevice> RenderDevice::mInstance = nullptr;
RenderDevice::RenderDevice( ) : mClearColor( 0 ), mWidth( 0 ), mHeight( 0 ), mClipXMax( 0 ), mClipYMax( 0 ), mVertexShader( nullptr ), mPixelShader( nullptr ), mVertexBuffer( nullptr ), mIndexBuffer( nullptr )
{
	mDefaultSampler = SamplerStatePtr( new SamplerState( SamplerStateDesc( ) ) );
	
	RasterizerDesc rsdesc;
	rsdesc.cullMode = ECullMode::ECM_BACK;
	rsdesc.fillMode = EFillMode::FM_SOLID;
	mDefaultRS = RasterizerStatePtr( new RasterizerState( rsdesc ) );
}

void RenderDevice::PreparePipeline( )
{
	if ( mRasterizerState == nullptr )
		mRasterizerState = mDefaultRS;

	for ( uint i = 0; i < _MAX_TEXTURE_COUNT; i ++ )
	{
		if ( mTextures[i] != nullptr && mSamplers[i] == nullptr )
			mSamplers[i] = mDefaultSampler;
	}

	PSInput::SetVaryingCount( mVertexShader != nullptr ? mVertexShader->GetVaryingCount( ) : 0 );
}

RenderDevice::~RenderDevice( )
{
	delete[] mFrameBuffer;

	for ( uint i = 0; i < mHeight; i ++ )
		delete[] mDepthBuffer[i];

	delete[] mDepthBuffer;
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

void RenderDevice::RasterizeTriangle( const PSInput* v1, const PSInput* v2, const PSInput* v3 )
{
	// top to bottom, value of y is larger.
	if ( v1->position( ).y > v2->position( ).y )
		Math::Swap( v1, v2 );
	if ( v2->position( ).y > v3->position( ).y )
		Math::Swap( v2, v3 );
	if ( v1->position( ).y > v2->position( ).y )
		Math::Swap( v1, v2 );

	float y31 = v3->position( ).y - v1->position( ).y;
	float factor = Math::Abs( y31 ) < Math::cEpsilon ? 0.0f : ( v2->position( ).y - v1->position( ).y ) / y31;

	PSInput mid;
	PSInput::Lerp( mid, *v1, *v3, factor );

	const PSInput* leftmid;
	const PSInput* rightmid;

	if ( v2->position( ).x < mid.position( ).x )
	{
		leftmid = v2;
		rightmid = &mid;
	}
	else
	{
		leftmid = &mid;
		rightmid = v2;
	}

	// Draw top triangle.
	RasterizerScanline topscanline;
	topscanline.left = *v1;
	topscanline.right = *v1;

	float yt = leftmid->position( ).y - v1->position( ).y;
	float invyt = Math::Abs( yt ) < Math::cEpsilon ? 0.0f : 1.0f / yt;
	topscanline.leftstep = ( *leftmid - *v1 ) * invyt;
	topscanline.rightstep = ( *rightmid - *v1 ) * invyt;

	topscanline.ymin = v1->position( ).y;
	topscanline.ymax = leftmid->position( ).y;

	FillScanline( topscanline );

	// Draw bottom triangle.
	RasterizerScanline bottomscanline;
	bottomscanline.left = *leftmid;
	bottomscanline.right = *rightmid;

	float yb = v3->position( ).y - leftmid->position( ).y;
	float invyb = Math::Abs( yb ) < Math::cEpsilon ? 0.0f : 1.0f / yb;
	bottomscanline.leftstep = ( *v3 - *leftmid ) * invyb;
	bottomscanline.rightstep = ( *v3 - *rightmid ) * invyb;

	bottomscanline.ymin = leftmid->position( ).y;
	bottomscanline.ymax = v3->position( ).y;

	FillScanline( bottomscanline );
}

void RenderDevice::FillScanline( RasterizerScanline& scanline )
{
	PSInput& left = scanline.left;
	PSInput& right = scanline.right;

	const PSInput& leftstep = scanline.leftstep;
	const PSInput& rightstep = scanline.rightstep;
	float ymin = scanline.ymin;
	float ymax = scanline.ymax;

	// Pop-left fill convention, top part.
	int ys = (int) Math::Ceil( ymin );
	int ye = (int) Math::Ceil( ymax ) - 1;

	if ( ys < 0 || ye > mClipYMax )
		return;

	if ( ys < 0 )
	{
		left += leftstep * -ymin;
		right += rightstep * -ymin;
		ys = 0;
		ymin = 0.0f;
	}

	if ( ye > mClipYMax )
		ye = mClipYMax;

	if ( ys > ye )
		return;

	float subpixel = (float) ys - ymin;
	left += leftstep * subpixel;
	right += rightstep * subpixel;

	for ( int y = ys; y <= ye; y ++ )
	{
		DrawScanline( left, right, y );

		left += leftstep;
		right += rightstep;
	}
}

void RenderDevice::DrawScanline( const PSInput& left, const PSInput& right, int y )
{
	float xmin = left.position( ).x;
	float xmax = right.position( ).x;
	int xs = (int) Math::Ceil( xmin );
	int xe = (int) Math::Ceil( xmax ) - 1;

	if ( xe < 0 || xs > mClipXMax )
		return;

	PSInput step = xs == xe ? PSInput::cZero : ( right - left ) / ( xmax - xmin );
	PSInput iterator = left;

	if ( xs < 0 )
	{
		xs = 0;
		iterator += step * - xmin;
	}

	if ( xe > mClipXMax )
		xe = mClipXMax;

	float subpixel = (float) xs - xmin;
	iterator += step * subpixel;

	float depth;
	for ( int x = xs; x <= xe; x ++ )
	{
		PSInput raster = iterator;
		raster.InHomogen( );

		PSOutput psout;
		mPixelShader->Execute( raster, psout, depth, mPSConstantBuffer );

		if ( DepthTestAndWrite( x, y, raster.position( ).w ) )
			DrawPixel( x, y, psout.color );

		iterator += step;
	}
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

InputLayoutPtr RenderDevice::CreateInputLayout( InputElementDesc const * desc, uint count )
{
	return InputLayoutPtr( new InputLayout( desc, count ) );
}

GraphicsBufferPtr RenderDevice::CreateBuffer( void* buffer, uint length, uint size )
{
	return GraphicsBufferPtr( new GraphicsBuffer( buffer, length, size ) );
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
	PreparePipeline( );

	VertexProcessContext ctx;
	ctx.mVertexBuffer		= mVertexBuffer;
	ctx.mIndexBuffer		= mIndexBuffer;
	ctx.mInputLayout		= mInputLayout;
	ctx.mVertexShader		= mVertexShader;
	ctx.mVSConstantBuffer	= mVSConstantBuffer;
	ctx.mRasterizerState	= mRasterizerState;
	ctx.mIndexCount			= indexcount;
	ctx.mIndexStart			= startindex;
	ctx.mVertexStart		= startvertex;

	VertexProcessEngine& vpengine = VertexProcessEngine::Instance( );
	vpengine.Prepare( ctx );
	std::vector<PSInput*>& rasterverts = vpengine.Process( );
	if ( rasterverts.size( ) == 0 )
		return;

	std::vector<PSInput*> sorts;
	sorts.insert( sorts.begin( ), rasterverts.begin( ), rasterverts.end( ) );
	std::sort(sorts.begin(), sorts.end());
	sorts.erase( std::unique( sorts.begin( ), sorts.end () ), sorts.end( ) );

	// Viewport Transformation.
	for ( uint i = 0; i < sorts.size( ); i ++ )
	{
		sorts[i]->Homogen( );
				
		// Viewport transformation.
		Vector4& pos = sorts[i]->position( );
		pos.x = ( 1.0f + pos.x ) * 0.5f * mWidth;
		pos.y = ( 1.0f - pos.y ) * 0.5f * mHeight;
	}

	// Rasterization.
	EFillMode fillmode = mRasterizerState->GetFillMode( );
	if ( fillmode == EFillMode::FM_SOLID )
	{
		for ( uint i = 0; i < rasterverts.size( ); i += 3 )
			RasterizeTriangle( rasterverts[i], rasterverts[i + 1], rasterverts[i + 2] );
	}
	else if ( fillmode == EFillMode::FM_WIREFRAME )
	{
		for ( uint i = 0; i < rasterverts.size( ); i += 2 )
		{
			const Vector4& v1 = rasterverts[i]->position( );
			const Vector4& v2 = rasterverts[ i + 1 ]->position( );
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

Color RenderDevice::Texture2D( uint index, Vector2 uv )
{
	return Texture2D( index, uv.x, uv.y );
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

	SamplerStateDesc& desc = mSamplers[index]->mDesc;
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

RasterizerStatePtr RenderDevice::CreateRasterizerState( const RasterizerDesc& desc )
{
	return RasterizerStatePtr( new RasterizerState( desc ) );
}

void RenderDevice::SetRasterizerState( RasterizerStatePtr rs )
{
	mRasterizerState = rs;
}