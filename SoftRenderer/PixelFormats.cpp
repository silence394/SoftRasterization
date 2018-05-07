#include "PixelFormats.h"

PixelFormatInfo GPixelFormats[ PF_MAX ] = 
{
	{ "RGBA8", 4 },
	{ "RGB8", 3 },
};

PixelFormatConvertor::PixelConvertor PixelFormatConvertor::sConvertors[ PF_MAX ];

PixelFormatConvertor::PixelFormatConvertor( )
{
	sConvertors[ EPixelFormat::PF_A8R8G8B8 ] = PixelConvert<RGBA8>;
	sConvertors[ EPixelFormat::PF_R8G8B8 ] = PixelConvert<RGB8>;
}