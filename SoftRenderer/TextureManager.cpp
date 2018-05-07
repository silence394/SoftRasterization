#include "TextureManager.h"
#include "RenderDevice.h"
#include "texture.h"
#include "Surface.h"
#include "String.h"
#include "FreeImageColor.h"

template <typename T>
void ConvertColor( FIBITMAP* image, Surface* suf )
{
}

Texture* TextureManager::Load( const wstring& resname )
{
	string filename;
	WStr2Str( resname, filename );

	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP *dib( 0 );
	BYTE* bits( 0 );
	uint width, height;
	fif = FreeImage_GetFileType( filename.c_str( ), 0 );
	if(fif == FIF_UNKNOWN) 
		fif = FreeImage_GetFIFFromFilename( filename.c_str( ) );

	dib = FreeImage_Load( fif, filename.c_str( ) );

	dib = FreeImage_ConvertTo32Bits( dib );
	bits = FreeImage_GetBits( dib );
	width = FreeImage_GetWidth( dib );
	height = FreeImage_GetHeight( dib );

	uint format = 1;
	Texture* tex = RenderDevice::Instance( ).CreateTexture2D( width, height, format );

	uint type = FreeImage_GetImageType( dib );
	if ( type == FIT_BITMAP )
	{
		uint colortype = FreeImage_GetColorType( dib );

		uint pitch = FreeImage_GetPitch( dib );
		uint bpp = FreeImage_GetBPP( dib ) >> 3;

		byte* src = bits;

		for ( uint j = 0; j < height; j ++ )
		{
			for ( uint i = 0; i < width; i ++ )
			{
				byte* temp = src + i * bpp;
				FreeImageColor<RGB8> fc( src + i * bpp );
				RGB8 c( fc.r, fc.g, fc.b, fc.a );
				RGB8* temp1 = reinterpret_cast<RGB8*> ( temp );
				*temp1 = c;
			}

			src += pitch;
		}

		if ( colortype == FIC_RGBALPHA )
		{
		}
		else if ( colortype == FIC_RGB )
		{
		}
	}

	return nullptr;
}