#pragma once

class Texture
{
public:
	enum TextureFormat
	{
		TF_ARGB8 = 1,
	};

private:
	void*	mBuffer;
	uint	mWidth;
	uint	mHeight;
	uint	mFormat;

public:
	Texture( )
		: mBuffer( nullptr ), mWidth( 0 ), mHeight( 0 ), mFormat( 0 ) { }

	Texture( void* buffer, uint width, uint height, uint format );

	~Texture( );

	static uint Texture::GetBpp( uint format );

	uint GetWidth( ) const
		{ return mWidth; }
	uint GetHeight( ) const
		{ return mHeight; }
};