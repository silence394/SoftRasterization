#include "shaders.h"
#include "renderdevice.h"
#include "assert.h"

const Matrix4& ShaderBase::GetMatrix( uint index ) const
{
	assert( index < _RC_VERTEX );
	return mMatrixConstants[ index ];
}

void ShaderBase::SetMatrix( uint index, const Matrix4& mat )
{
	assert( index < _RC_VERTEX );
	mMatrixConstants[ index ] = mat;
}

uint IPixelShader::SampleTexture( uint index, float u, float v )
{
	return RenderDevice::Instance( ).SampleTexture( index, u, v );
}

Color IPixelShader::Texture2D( uint index, float u, float v )
{
	return RenderDevice::Instance( ).Texture2D( index, u, v );
}