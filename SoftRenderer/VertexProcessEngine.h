#pragma once

#include "Prerequisites.h"
#include <list>
#include "Shaders.h"

struct VertexProcessContext
{
	GraphicsBufferPtr	mVertexBuffer;
	GraphicsBufferPtr	mIndexBuffer;
	InputLayoutPtr		mInputLayout;
	VertexShaderPtr		mVertexShader;
	ConstantBufferPtr*	mVSConstantBuffer;
	RasterizerStatePtr	mRasterizerState;
	uint				mIndexCount;
	uint				mIndexStart;
	uint				mVertexStart;
};

class VertexProcessEngine
{
private:
	enum EMaxCount
	{
		MC_VertexCache	= 64,
	};

	GraphicsBufferPtr								mVertexBuffer;
	GraphicsBufferPtr								mIndexBuffer;
	InputLayoutPtr									mInputLayout;
	VertexShaderPtr									mVertexShader;
	ConstantBufferPtr*								mVSConstantBuffer;
	RasterizerStatePtr								mRasterizerState;
	uint											mIndexCount;
	uint											mIndexStart;
	uint											mVertexStart;

	static std::unique_ptr< VertexProcessEngine >	mInstance;
	std::pair< uint, PSInput* >						mVertexCache[ MC_VertexCache ];
	std::vector< PSInput >							mVertexPool;
	std::list< PSInput >							mClippedVertex;

	std::vector< PSInput* >							mRasterizerVertex;

private:
	VertexProcessEngine( ) { }

	bool IsContextValid( ) const
	{
		return mVertexBuffer != nullptr && mIndexBuffer != nullptr && mInputLayout != nullptr && mVertexShader != nullptr;
	}

	void FetchVertex3( PSInput** out, uint prim );

	void Cull( PSInput** in );

public:
	static VertexProcessEngine&	Instance( );

	void						Prepare( const VertexProcessContext& context );

	std::vector< PSInput* >&	Process( );
};