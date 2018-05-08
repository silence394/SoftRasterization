#pragma once

#include "prerequisites.h"
#include "color.h"

#if defined(PF_MAX)
#undef PF_MAX
#endif

typedef int PixelFormat;

enum EPixelFormat
{
	PF_A8R8G8B8	= 0,
	PF_R8G8B8	= 1,
	PF_MAX		= 2,
};

struct PixelFormatInfo
{
	char	name[16];
	uint	bpp;
};

extern PixelFormatInfo GPixelFormats[PF_MAX];

template< typename InColorType >
Color PixelConvert( void* in )
{
	return ( reinterpret_cast<InColorType*> (in) )->ToColor( );
}

class PixelFormatConvertor
{
public:
	typedef Color (*PixelConvertor) ( void* in );

	PixelFormatConvertor( );

	static PixelConvertor	sConvertors[ PF_MAX ];
};

static PixelFormatConvertor SPixelConvertor;