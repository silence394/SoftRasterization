#include "Surface.h"

Surface::Surface( uint w, uint h, uint format ) : mWidth( w ), mHeight( h ), mFormat( format )
{
	mBPP = 0;
	mBuffer.resize( mWidth * mHeight * mBPP );

}