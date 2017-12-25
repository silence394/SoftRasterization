#include "shaders.h"
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