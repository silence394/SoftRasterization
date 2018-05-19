#pragma once

#include "prerequisites.h"

class RenderTechnique
{
public:
	InputLayoutPtr		mInputLayout;
	VertexShaderPtr		mVertexShader;
	PixelShaderPtr		mPixelShader;

	ConstantBufferPtr	mVSConstantBuffer;
	ConstantBufferPtr	mPSConstantBuffer;

	SamplerStatePtr		mSamplerState;
	RasterizerStatePtr	mRasterState;
/*
public:
	void SetInputLayout( InputLayoutPtr layout )
	{
		mInputLayout = layout;
	}

	InputLayoutPtr GetInputLayout( ) const
	{
		return mInputLayout;
	}

	void SetVertexShader( VertexShaderPtr vs )
	{
		mVertexShader = vs;
	}

	VertexShaderPtr GetVertexShader( ) const
	{
		return mVertexShader;
	}

	void SetPixelShader( PixelShaderPtr ps )
	{
		mPixelShader = ps;
	}

	PixelShaderPtr GetPixelShader( ) const
	{
		return mPixelShader;
	}

	void SetVSConstantBuffer( ConstantBufferPtr cb )
	{
		mVSConstantBuffer = cb;
	}

	ConstantBufferPtr GetVSConstantBuffer( ) const
	{
		return mVSConstantBuffer;
	}

	void SetPSConstantBuffer( ConstantBufferPtr cb )
	{
		mPSConstantBuffer = cb;
	}

	ConstantBufferPtr GetPSConstantBuffer( ) const
	{
		return mPSConstantBuffer;
	}

	void SetSamplerState( SamplerStatePtr samplerstate )
	{
		mSamplerState = samplerstate;
	}

	SamplerStatePtr GetSamplerState( ) const
	{
		return mSamplerState;
	}

	void SetRasterizerState( RasterizerStatePtr rasterstate )
	{
		mRasterState = rasterstate;
	}

	RasterizerStatePtr GetRasterizerState( ) const
	{
		return mRasterState;
	}*/
};