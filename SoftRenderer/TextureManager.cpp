#include "TextureManager.h"
#include "texture.h"
#include "String.h"
#include "FreeImage.h"

Texture* TextureManager::Load( const wstring& resname )
{
	string filename;
	WStr2Str( resname, filename );

	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP *dib(0);
	BYTE* bits(0);
	uint width, height;
	fif = FreeImage_GetFileType(filename.c_str( ), 0);
	if(fif == FIF_UNKNOWN) 
		fif = FreeImage_GetFIFFromFilename(filename.c_str( ));

	dib = FreeImage_Load(fif, filename.c_str( ));

	dib = FreeImage_ConvertTo32Bits(dib);
	bits = FreeImage_GetBits(dib);
	//get the image width and height
	width = FreeImage_GetWidth(dib);
	height = FreeImage_GetHeight(dib);

	return nullptr;
}