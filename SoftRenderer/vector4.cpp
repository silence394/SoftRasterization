#include "vector4.h"
#include "color.h"

Vector4::operator Color( ) const
{
	return Color( x, y, z, w );
}