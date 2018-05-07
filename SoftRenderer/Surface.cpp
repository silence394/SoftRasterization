#include "Surface.h"
#include "PixelFormats.h"

Surface::Surface( uint w, uint h, uint format ) : mWidth( w ), mHeight( h ), mFormat( format )
{
	mBPP = GPixelFormats[ format ].bpp;
	mBuffer.resize( mWidth * mHeight * mBPP );
	mConvertor = PixelFormatConvertor::sConvertors[ format ];
}