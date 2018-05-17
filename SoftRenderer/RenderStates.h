#pragma once

enum ESamplerFilter
{
	SF_POINT	= 0,
	SF_Linear	= 1,
};

enum EAddressMode
{
	AM_WRAP		= 0,
	AM_CLAMP	= 1,
};

struct SamplerStateDesc
{
	ESamplerFilter	filter;
	EAddressMode	address;

	SamplerStateDesc( )
	{
		filter = SF_POINT;
		address = AM_CLAMP;
	}
};

class SamplerState
{
	friend class RenderDevice;

private:
	SamplerStateDesc	mDesc;

	SamplerState( const SamplerStateDesc& desc ) : mDesc( desc ) { }
};

enum EFillMode
{
	FM_SOLID		= 0,
	FM_WIREFRAME	= 1,
};

enum ECullMode
{
	ECM_NONE		= 0,
	ECM_FRONT		= 1,
	ECM_BACK		= 2,
};

struct RasterizerDesc
{
	EFillMode	fillMode;
	ECullMode	cullMode;
};

class RasterizerState
{
	friend class RenderDevice;

private:
	RasterizerDesc	mDesc;

	RasterizerState( const RasterizerDesc& desc ) : mDesc( desc ) { }

public:
	void SetFillMode( EFillMode mode )
	{
		mDesc.fillMode = mode;
	}

	EFillMode GetFillMode( ) const
	{
		return mDesc.fillMode;
	}

	void SetCullMode( ECullMode mode )
	{
		mDesc.cullMode = mode;
	}

	ECullMode GetCullMode( ) const
	{
		return mDesc.cullMode;
	}
};