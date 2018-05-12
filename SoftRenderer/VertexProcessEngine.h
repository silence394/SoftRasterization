#pragma once

#include "Prerequisites.h"

struct VertexProcessContext
{
	GraphicsBufferPtr	mVertexBuffer;
	GraphicsBufferPtr	mIndexBuffer;
	InputLayoutPtr		mInputLayout;
	VertexShaderPtr		mVertexShader;
	uint				mIndexCount;
	uint				mIndexStart;
	uint				mVertexStart;

};

class VertexProcessEngine
{
private:
	static std::unique_ptr<VertexProcessEngine>	mInstance;

	VertexProcessEngine( ) { }

public:
	static VertexProcessEngine& Instance( );

	void	Prepare( );
	void	Process( const VertexProcessContext& context );
};