#pragma once

#include "prerequisites.h"
#include "matrix4.h"
#include "color.h"

#define _MAX_VSINPUT_COUNT 2
#define _MAX_PSINPUT_COUNT 2

class VSInput
{
private:
	Vector4 mShaderRigisters[ _MAX_VSINPUT_COUNT + 1 ];

public:
	Vector4& attribute( uint index )
		{ return mShaderRigisters[ index ]; }
};

class PSInput
{
private:
	Vector4 mShaderRigisters[ _MAX_PSINPUT_COUNT + 1 ];

public:
	Vector4& position( )
		{ return mShaderRigisters[0]; }

	const Vector4& position( ) const
		{ return mShaderRigisters[0]; }

	Vector4& attribute( uint index )
		{ return mShaderRigisters[ index + 1 ]; }

	const Vector4& attribute( uint index ) const
		{ return mShaderRigisters[ index + 1 ]; }
};

class PSOutput
{
public:
	Color color;
};

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

	Matrix4	mMatrixConstants[ _RC_VERTEX ];

public:
	const Matrix4&	GetMatrix( uint index ) const;
	void			SetMatrix( uint index, const Matrix4& mat );
};

class IVertexShader : public ShaderBase
{
public:
	virtual void Execute( VSInput& in, PSInput& out ) = 0;
};

class IPixelShader : public ShaderBase
{
public:
	virtual void Execute( PSInput& in, PSOutput& out, float& depth ) = 0;
	virtual uint SampleTexture( uint index, float u, float v );
	virtual Color Texture2D( uint index, float u, float v );
};