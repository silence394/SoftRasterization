#include "libengine.h"
RenderDevice*	gRenderDevice = nullptr;

class PixelShader : public IPixelShader
{
	virtual void Execute( Vector4* regs, Color& color, float& depth )
	{
		color = Color( regs[1].x, regs[1].y, regs[1].z, regs[1].w );
	}
};

class DemoApp : public App
{
private:
	RenderDevice*	mRenderDevice;
	Camera			mCamera;
	Matrix4			mWorldTransform;
	Matrix4			mViewTransform;
	Matrix4			mPerspectTransform;
	Texture*		mTexture;

	IVertexShader*	mVertexShader;
	IPixelShader*	mPixelShader;

public:
	DemoApp( int width = 800, int height = 600, LPCWSTR name = L"Demo" )
		: App( width, height, name ) { }

public:
	virtual void OnCreate( );
	virtual void OnRender( );
	virtual void OnClose( );
};

void DemoApp::OnCreate( )
{
	mRenderDevice = GetRenderDevice( );
	mCamera.eye = Vector3( 5.0f, 4.0f, 3.0f );
	mCamera.look = Vector3( 0.0f, 0.0f, 0.0f );
	mCamera.up = Vector3( 0.0f, 0.0f, 1.0f );

	mWorldTransform = Matrix4::identity;
	mViewTransform = Matrix4::View( mCamera.eye, mCamera.look, mCamera.up );
	mPerspectTransform = Matrix4::Perspective( 1.57f, (float) mRenderDevice->GetDeviceWidth( ) / (float) mRenderDevice->GetDeviceHeight( ), 0.001f, 1000.0f );

	// Create texture.
	uint width = 256;
	uint height = 256;
	uint length = width * height;
	uint* texbuffer = new uint[ length ];
	uint* base = texbuffer;
	for ( uint j = 0; j < height; j ++ )
	{
		for ( uint i = 0; i < width; i ++ )
		{
			uint color = ( ( i / 32 + j / 32 ) & 1) ? 0xffffffff : 0xff0093dd;
			*base = color;
		}
	}

	mTexture = new Texture( texbuffer, width, height, Texture::TF_ARGB8 );
	delete[] texbuffer;

	mPixelShader = new PixelShader( );
	mRenderDevice->SetPixelShader( mPixelShader );
}

void DemoApp::OnClose( )
{
	delete mTexture;
}

bool CheckInCVV( const Vector4& v )
{
	if ( v.x < -1.0f || v.x > 1.0f || v.y < -1.0f || v.y > 1.0f || v.z < 0 || v.w > 1.0f )
		return false;

	return true;
}

void DemoApp::OnRender( )
{
	mRenderDevice->SetClearColor( 0xFF808080 );
	mRenderDevice->Clear( );

	struct Vertex
	{
		Vector3	pos;
		uint	color;
		Vector2	texcoord;
	};

	Vertex vertex[8] = 
	{
		{ Vector3( -1.0f, -1.0f, -1.0f ), 0xffff0000, Vector2( 0.0f, 0.0f ) },
		{ Vector3( -1.0f, +1.0f, -1.0f ), 0xff0000ff, Vector2( 1.0f, 0.0f ) },
		{ Vector3( +1.0f, +1.0f, -1.0f ), 0xffff0000, Vector2( 1.0f, 1.0f ) },
		{ Vector3( +1.0f, -1.0f, -1.0f ), 0xffff00ff, Vector2( 0.0f, 1.0f ) },
		{ Vector3( -1.0f, -1.0f, +1.0f ), 0xffffff00, Vector2( 1.0f, 1.0f ) },
		{ Vector3( -1.0f, +1.0f, +1.0f ), 0xffff0000, Vector2( 0.0f, 1.0f ) },
		{ Vector3( +1.0f, +1.0f, +1.0f ), 0xff00ff00, Vector2( 0.0f, 0.0f ) },
		{ Vector3( +1.0f, -1.0f, +1.0f ), 0xff22ffff, Vector2( 1.0f, 0.0f ) },
	};

	uint indices[36] =
	{
		0, 1, 2,
		0, 2, 3,
		4, 6, 5,
		4, 7, 6,
		4, 5, 1,
		4, 1, 0,
		3, 2, 6,
		3, 6, 7,
		1, 5, 6,
		1, 6, 2,
		4, 0, 3,
		4, 3, 7,
	};

	Matrix4 wv = mWorldTransform * mViewTransform;
	Matrix4 wvp = wv * mPerspectTransform;
	
	// VS Stage.
	struct VSOutput
	{
		Vector4	pos; // To Pixel shader stage.
		Color	color;
	};

	VSOutput vsoutput[8];
	for ( uint i = 0; i < 8; i ++ )
	{
		// TODO. 可以通过Vector3 * wvp 得到一个空间点， 哪种更适合后面的计算？
		Vector4 hamopos = Vector4( vertex[i].pos, 1.0f );
		Vector4 pos = hamopos * wvp;
		float invw = 1.0f / pos.w;
		pos.x *= invw;
		pos.y *= invw;
		pos.z *= invw;
		pos.w = invw;
		vsoutput[i].pos = pos;

		vsoutput[i].color = vertex[i].color;
		vsoutput[i].color *= invw;
	}

	// TO Screen.
	float width = (float) mRenderDevice->GetDeviceWidth( );
	float height = (float) mRenderDevice->GetDeviceHeight( );
	for ( uint i = 0; i < 8; i ++ )
	{
		vsoutput[i].pos.x = ( vsoutput[i].pos.x + 1.0f ) * 0.5f * width;
		vsoutput[i].pos.y = ( vsoutput[i].pos.y + 1.0f ) * 0.5f * height;
	}

	for ( uint i = 0; i < 36; i += 3 )
	{
		const Vector4& v1 = vsoutput[ indices[i] ].pos;
		const Vector4& v2 = vsoutput[ indices[i + 1] ].pos;
		const Vector4& v3 = vsoutput[ indices[i + 2] ].pos;
		const Color& c1 = vsoutput[ indices[i] ].color;
		const Color& c2 = vsoutput[ indices[i + 1] ].color;
		const Color& c3 = vsoutput[ indices[i + 2] ].color;

//		Move To assembly stage.
// 		if ( !CheckInCVV( v1 ) || !CheckInCVV( v2 ) || !CheckInCVV( v3 ) )
// 			continue;

		// BackCulling.
		if ( ( v3.x - v1.x ) * ( v3.y - v2.y ) - ( v3.y - v1.y ) * ( v3.x - v2.x ) > 0 )
			continue;

		{
			VSOutput* top = &vsoutput[ indices[i] ];
			VSOutput* middle = &vsoutput[ indices[i+1] ];
			VSOutput* bottom = &vsoutput[ indices[i+2] ];
			// top to bottom, value of y is larger.
			if ( top->pos.y > middle->pos.y )
				Math::Swap( top, middle );
			if ( middle->pos.y > bottom->pos.y )
				Math::Swap( middle, bottom );
			if ( top->pos.y > middle->pos.y )
				Math::Swap( top, middle );

			if ( top->pos.y == bottom->pos.y )
			{
				if ( top->pos.x > middle->pos.x )
					Math::Swap( top, middle );
				if ( middle->pos.x > bottom->pos.x )
					Math::Swap( middle, bottom );
				if ( top->pos.x > middle->pos.x )
					Math::Swap( top, middle );

				PSInput topinput = { Vector4(0.0f, 0.0f, 0.0f, 0.0f ) };
				PSInput bottominput = { Vector4(0.0f, 0.0f, 0.0f, 0.0f ) };
				topinput.mShaderRigisters[0] = top->pos;
				topinput.mShaderRigisters[1] = Vector4( top->color.r, top->color.g, top->color.b, top->color.a );
				bottominput.mShaderRigisters[0] = bottom->pos;
				bottominput.mShaderRigisters[1] = Vector4( bottom->color.r, bottom->color.g, bottom->color.b, bottom->color.a );
				mRenderDevice->DrawScanline( &topinput, &bottominput );
			}
			else
			{
				float factor = ( middle->pos.y - top->pos.y ) / ( bottom->pos.y - top->pos.y );
				VSOutput newmiddle;
				// Lerp.
				{
					// w is in linear space, z is not. So doesnot use z when interploate.
					newmiddle.pos = Vector4::Lerp( top->pos, bottom->pos, factor );
					newmiddle.color = Color::Lerp( top->color, bottom->color, factor );
				}

				PSInput topinput = { Vector4(0.0f, 0.0f, 0.0f, 0.0f ) };
				PSInput middleinput = { Vector4(0.0f, 0.0f, 0.0f, 0.0f ) };
				PSInput newmiddleinput = { Vector4(0.0f, 0.0f, 0.0f, 0.0f ) };
				PSInput bottominput = { Vector4(0.0f, 0.0f, 0.0f, 0.0f ) };
				topinput.mShaderRigisters[0] = top->pos;
				topinput.mShaderRigisters[1] = Vector4( top->color.r, top->color.g, top->color.b, top->color.a );
				middleinput.mShaderRigisters[0] = middle->pos;
				middleinput.mShaderRigisters[1] = Vector4( middle->color.r, middle->color.g, middle->color.b, middle->color.a );
				newmiddleinput.mShaderRigisters[0] = newmiddle.pos;
				newmiddleinput.mShaderRigisters[1] = Vector4( newmiddle.color.r, newmiddle.color.g, newmiddle.color.b, newmiddle.color.a );
				bottominput.mShaderRigisters[0] = bottom->pos;
				bottominput.mShaderRigisters[1] = Vector4( bottom->color.r, bottom->color.g, bottom->color.b, bottom->color.a );

				mRenderDevice->DrawStandardTopTriangle( &topinput, &newmiddleinput, &middleinput );
				mRenderDevice->DrawStandardBottomTriangle( &middleinput, &newmiddleinput, &bottominput );
			}
		}
	}
}

int main( )
{
	DemoApp app( 800, 600 );
	app.Create( );
	app.Run( );

	return 0;
}