#include "texture.h"
#include <string>

uint Texture::GetBpp( uint format )
{
	switch ( format )
	{
		case TF_ARGB8:
			return 4;

		default:
			return 0;
	}
}