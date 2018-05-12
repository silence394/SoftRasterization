#include "shaders.h"
#include "RenderDevice.h"
#include "assert.h"

uint IPixelShader::SampleTexture( uint index, float u, float v )
{
	return RenderDevice::Instance( ).SampleTexture( index, u, v );
}

Color IPixelShader::Texture2D( uint index, float u, float v )
{
	return RenderDevice::Instance( ).Texture2D( index, u, v );
}