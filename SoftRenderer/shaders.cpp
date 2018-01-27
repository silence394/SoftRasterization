#include "shaders.h"
#include "assert.h"
#include "renderdevice.h"

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

void ShaderBase::SetDevice( RenderDevice* rd )
{
	mRenderDevice = rd;
}

uint IPixelShader::SampleTexture( uint index, float u, float v )
{
	if ( mRenderDevice == nullptr )
		return 0;

	return mRenderDevice->SampleTexture( index, u, v );
}