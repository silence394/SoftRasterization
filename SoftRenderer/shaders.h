#pragma once

#include "prerequisites.h"
#include "matrix4.h"

class ShaderBase
{
protected:
	RenderDevice*	mRenderDevice;

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

	Matrix4	mMatrixConstants[ _RC_VERTEX ];

public:
	ShaderBase( ) : mRenderDevice( nullptr )
		{ }

	const Matrix4&	GetMatrix( uint index ) const;
	void			SetMatrix( uint index, const Matrix4& mat );

	void			SetDevice( RenderDevice* rd );
};

class IVertexShader : public ShaderBase
{
public:
	virtual void Execute( Vector4* regs ) = 0;
};

class IPixelShader : public ShaderBase
{
public:
	virtual void Execute( const Vector4* regs, Color& color, float& depth ) = 0;
	virtual uint SampleTexture( uint index, float u, float v );
};