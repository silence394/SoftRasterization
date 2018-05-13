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

	bool IsValid( ) const
	{
		return mVertexBuffer != nullptr && mIndexBuffer != nullptr && mInputLayout != nullptr && mVertexShader != nullptr;
	}
};

class VertexProcessEngine
{
private:
	enum EMaxCount
	{
		MC_VertexCache	= 64,
	};

	static std::unique_ptr< VertexProcessEngine >	mInstance;
	VertexProcessContext							mContext;
	std::pair< uint, PSInput* >						mVertexCache[ MC_VertexCache ];
	std::vector< PSInput >							mVertexPool;
	std::list< PSInput >							mClippedVertex;

	std::vector< PSInput* >							mRasterizerVertex;

private:
	VertexProcessEngine( ) { }

	void FetchVertex3( PSInput** out, uint prim );

	void Cull( PSInput** in );

public:
	static VertexProcessEngine&	Instance( );

	void						Prepare( const VertexProcessContext& context );

	std::vector< PSInput* >&	Process( );
};