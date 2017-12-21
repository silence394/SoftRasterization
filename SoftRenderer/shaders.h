#pragma once

#include "prerequisites.h"

class ShaderBase
{
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