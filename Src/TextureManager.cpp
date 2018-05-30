#include "TextureManager.h"
#include "RenderDevice.h"
#include "Texture.h"
#include "Surface.h"
#include "String.h"
#include "FreeImageColor.h"

std::unique_ptr<TextureManager> TextureManager::mInstance = nullptr;

TextureManager& TextureManager::Instance( )
{
	if ( mInstance == nullptr )
		mInstance = std::unique_ptr<TextureManager> ( new TextureManager( ) );

	return *mInstance;
}

template <typename T>
void ConvertColor( FIBITMAP* image, SurfacePtr suf )
{
	if ( image == nullptr || suf == nullptr )
		return;

	uint width = FreeImage_GetWidth( image );
	uint height = FreeImage_GetHeight( image );
	byte* bits = FreeImage_GetBits( image );

	uint pitch = FreeImage_GetPitch( image );
	uint bpp = FreeImage_GetBPP( image ) >> 3;

	byte* src = bits;
	for ( uint j = 0; j < height; j ++ )
	{
		for ( uint i = 0; i < width; i ++ )
		{
			byte* temp = src + i * bpp;
			FreeImageColor<T> fc( src + i * bpp );

			T c( fc.r, fc.g, fc.b, fc.a );
			* reinterpret_cast<T*> ( suf->Address( i, j ) ) = c;
		}

		src += pitch;
	}
}

TexturePtr TextureManager::Load( const std::wstring& resname )
{
	std::string filename;
	WStr2Str( resname, filename );

	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP *dib( 0 );
	BYTE* bits( 0 );
	fif = FreeImage_GetFileType( filename.c_str( ), 0 );
	if(fif == FIF_UNKNOWN) 
		fif = FreeImage_GetFIFFromFilename( filename.c_str( ) );

	dib = FreeImage_Load( fif, filename.c_str( ) );

	FREE_IMAGE_TYPE imagetype = FreeImage_GetImageType( dib );
	PixelFormat format = PF_UNKNOWN;
	if ( imagetype == FIT_BITMAP )
	{
		uint bpp = FreeImage_GetBPP( dib );
		uint rmask = FreeImage_GetRedMask( dib );
		uint gmask = FreeImage_GetGreenMask( dib );
		uint bmask = FreeImage_GetBlueMask( dib );
		switch( bpp )
		{
			case 24:
			{
				if ( rmask == 0xff0000 && gmask == 0x00ff00 && bmask == 0x0000ff )
					format = EPixelFormat::PF_R8G8B8;

				break;
			}
			case 32:
			{
				if ( rmask == 0xff0000 && gmask == 0x00ff00 && bmask == 0x0000ff )
					format = EPixelFormat::PF_A8R8G8B8;

				break;
			}
		}
	}

	if ( format == PF_UNKNOWN )
		return nullptr;

	uint width = FreeImage_GetWidth( dib );
	uint height = FreeImage_GetHeight( dib );
	TexturePtr tex = RenderDevice::Instance( ).CreateTexture2D( width, height, format );

	FREE_IMAGE_COLOR_TYPE colortype = FreeImage_GetColorType( dib );
	if ( colortype == FIC_RGBALPHA )
		ConvertColor<RGBA8>( dib, tex->GetSurface( 0 ) );
	else if ( colortype == FIC_RGB )
		ConvertColor<RGB8>( dib, tex->GetSurface( 0 ) );

	return tex;
}