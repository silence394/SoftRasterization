#include "Shaders.h"
#include "RenderDevice.h"
#include "assert.h"
#include "Vector2.h"

Color IPixelShader::Texture2D( uint index, float u, float v )
{
	return RenderDevice::Instance( ).Texture2D( index, u, v );
}

Color IPixelShader::Texture2D( uint index, Vector2 uv )
{
	return RenderDevice::Instance( ).Texture2D( index, uv );
}

uint PSInput::mVaryingCount = 0;
const PSInput PSInput::cZero = PSInput( 0.0f );