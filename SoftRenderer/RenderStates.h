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

	SamplerState( ) { }

public:
	SamplerState( const SamplerStateDesc& desc )
	{
		mDesc = desc;
	}
};