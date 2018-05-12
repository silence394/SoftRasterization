#pragma once

#include "Prerequisites.h"
#include <vector>

class GraphicsBuffer
{
public:
	enum BufferFormat
	{
		BF_A8R8G8B8				= 0x01,
		BF_R32G32_FLOAT			= 0x02,
		BF_R32B32G32_FLOAT		= 0x03,
		BF_R32B32G32A32_FLOAT	= 0x04,
	};

private:
	friend class RenderDevice;

	void*	mBuffer;
	uint	mLength;
	uint	mSize;

	GraphicsBuffer( void* buffer, uint len, uint size ) : mBuffer( buffer ), mLength( len ), mSize( size ) { }

public:
	~GraphicsBuffer( )
	{
		delete[] mBuffer;
	}

	void* GetBuffer( ) const
	{
		return mBuffer;
	}

	uint GetLength( ) const
	{
		return mLength;
	}

	uint GetSize( ) const
	{
		return mSize;
	}
};

struct InputElementDesc
{
	std::string	mName;
	uint		mFormat;
	uint		mOffset;

	InputElementDesc( std::string name, uint format, uint offset ) : mName( name ), mFormat( format ), mOffset( offset ) { }
};

class InputLayout
{
private:
	friend class RenderDevice;

	std::vector<InputElementDesc>	mDescs;

	InputLayout( InputElementDesc const* desc, uint count )
	{
		mDescs.assign( desc, desc + count );
	}

public:
	const std::vector<InputElementDesc>& GetElementDescs( ) const
	{
		return mDescs;
	}
};