#pragma once

#include "prerequisites.h"
#include "matrix4.h"

class ShaderBase
{
public:
	enum _REGISTER_COUNT
	{
		_RC_VERTEX	= 4,
	};

	enum _CONSTANT_TYPE
	{
		_CT_WORLD_TRANSFORM			= 0,
		_CT_VIEW_TRANSFORM			= 1,
		_CT_PERSPECTIVE_TRANSFORM	= 2,
		_CT_WVP_TRANSFORM			= 3,
	};

	const Matrix4&	GetMatrix( uint index ) const;
	void			SetMatrix( uint index, const Matrix4& mat );

public:
	Matrix4	mMatrixConstants[ _RC_VERTEX ];
};

class IVertexShader : public ShaderBase
{
public:
	virtual void Execute( Vector4* regs ) = 0;
};

class IPixelShader : public ShaderBase
{
public:
	virtual void Execute( Vector4* regs, Color& color, float& depth ) = 0;
};