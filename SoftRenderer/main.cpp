#include "renderer.h"
RenderDevice*	gRenderDevice = nullptr;

class DemoApp : public App
{
private:
	RenderDevice*	mRenderDevice;
	Camera			mCamera;
	Matrix4			mWorldTransform;
	Matrix4			mViewTransform;
	Matrix4			mPerspectTransform;
	Texture*		mTexture;

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

		// Draw Scanline.
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

				VSOutput* left = top;
				VSOutput* right = bottom;
				uint startx = (uint) left->pos.x;
				uint endx = (uint) left->pos.x;
				for ( uint x = startx; x < endx; x ++ )
				{
					float factor = (float) ( x - startx ) / ( endx - startx );
					VSOutput out;
					out.pos = Vector4::Lerp( left->pos, right->pos, factor );
					out.color = Color::Lerp( left->color, right->color, factor );

					float w = 1.0f / out.pos.w;
					out.color *= w;

					mRenderDevice->DrawPixel( (uint) out.pos.x, (uint) out.pos.y, out.color );
				}
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

				// Process top newmiddle, middle.
				{
					uint starty = (uint) top->pos.y;
					uint endy = (uint) middle->pos.y;

					for ( uint y = starty; y < endy; y ++ )
					{
						float factor = (float) ( y - starty ) / ( endy - starty );
						VSOutput va;
						va.pos = Vector4::Lerp( top->pos, middle->pos, factor );
						va.color = Color::Lerp( top->color, middle->color, factor );
						VSOutput vb;
						vb.pos = Vector4::Lerp( top->pos, newmiddle.pos, factor );
						vb.color = Color::Lerp( top->color, newmiddle.color, factor );

						VSOutput* left = &va;
						VSOutput* right = &vb;
						if ( left->pos.x > right->pos.x )
							Math::Swap( left, right );

						uint startx = (uint) left->pos.x;
						uint endx = (uint) right->pos.x;
						for ( uint x = startx; x < endx; x ++)
						{
							float factor = (float) ( x - startx ) / ( endx - startx );
							VSOutput out;
							out.pos = Vector4::Lerp( left->pos, right->pos, factor );
							out.color = Color::Lerp( left->color, right->color, factor );

							float w = 1.0f / out.pos.w;
							out.color *= w;

							// Pixel Shader.
							mRenderDevice->DrawPixel( (uint) out.pos.x, (uint) out.pos.y, out.color );
						}
					}
				}

				// Process middle, newmiddle, bottom.
				if ( 1 )
				{
					uint starty = (uint) middle->pos.y;
					uint endy = (uint) bottom->pos.y;

					for ( uint y = starty; y < endy; y ++ )
					{
						float factor = (float) ( y - starty ) / ( endy - starty );
						VSOutput va;
						va.pos = Vector4::Lerp( middle->pos, bottom->pos, factor );
						va.color = Color::Lerp( middle->color, bottom->color, factor );
						VSOutput vb;
						vb.pos = Vector4::Lerp( newmiddle.pos, bottom->pos, factor );
						vb.color = Color::Lerp( newmiddle.color, bottom->color, factor );

						VSOutput* left = &va;
						VSOutput* right = &vb;
						if ( left->pos.x > right->pos.x )
							Math::Swap( left, right );

						uint startx = (uint) left->pos.x;
						uint endx = (uint) right->pos.x;
						for ( uint x = startx; x < endx; x ++)
						{
							float factor = (float) ( x - startx ) / ( endx - startx );
							VSOutput out;
							out.pos = Vector4::Lerp( left->pos, right->pos, factor );
							out.color = Color::Lerp( left->color, right->color, factor );

							float w = 1.0f / out.pos.w;
							out.color *= w;

							mRenderDevice->DrawPixel( (uint) out.pos.x, (uint) out.pos.y, out.color );
						}
					}
				}
			}
		}

// 		mRenderDevice->DrawLine( Point( v1.x, v1.y ), Point( v2.x, v2.y ), c1 * v1.z );
// 		mRenderDevice->DrawLine( Point( v1.x, v1.y ), Point( v3.x, v3.y ), c2 * v2.z );
// 		mRenderDevice->DrawLine( Point( v2.x, v2.y ), Point( v3.x, v3.y ), c3 * v3.z );
	}
	// Rasterazer Statge.
// 	for ( uint i = 0; i < 36; i += 3 )
// 	{
// 		Vector4 s1 = vertex[indexs[i]].pos * wvp;
// 		s1 = s1 / s1.w;
// 		Vector4 s2 = vertex[indexs[i+1]].pos * wvp;
// 		s2 = s2 / s2.w;
// 		Vector4 s3 = vertex[indexs[i+2]].pos * wvp;
// 		s3 = s3 / s3.w;
// 
// 		Point point1( (int) ( (s1.x + 1.0f) * 0.5f * mRenderDevice->GetDeviceWidth( ) ), (int) ( (s1.y + 1.0f) * 0.5f * mRenderDevice->GetDeviceHeight( ) ) );
// 		Point point2( (int) ( (s2.x + 1.0f) * 0.5f * mRenderDevice->GetDeviceWidth( ) ), (int) ( (s2.y + 1.0f) * 0.5f * mRenderDevice->GetDeviceHeight( ) ) );
// 		Point point3( (int) ( (s3.x + 1.0f) * 0.5f * mRenderDevice->GetDeviceWidth( ) ), (int) ( (s3.y + 1.0f) * 0.5f * mRenderDevice->GetDeviceHeight( ) ) );
// 
// 		uint color;
// 		if ( i < 10 )
// 			color = 0xffff0000;
// 		else if ( i < 20 )
// 			color = 0xff00ff00;
// 		else
// 			color = 0xff0000ff;
// 
// 		// Pixel Stage.
// 
// 		//mRenderDevice->FillTriangle( point1, point2, point3, 0xff00ff00 );
// 		mRenderDevice->DrawLine( point1, point2, color );
// 		mRenderDevice->DrawLine( point3, point2, color );
// 		mRenderDevice->DrawLine( point1, point3, color );
// 	}
}

int main( )
{
	DemoApp app( 800, 600 );
	app.Create( );
	app.Run( );

	return 0;
}