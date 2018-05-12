#pragma once

#include "prerequisites.h"
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
	void*	mBuffer;
	uint	mLength;
	uint	mSize;

public:
	GraphicsBuffer( ) : mBuffer( nullptr ), mLength( 0 ), mSize( 0 ){ }

	GraphicsBuffer( void* buffer, uint len, uint size ) : mBuffer( buffer ), mLength( len ), mSize( size ) { }

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
	std::vector<InputElementDesc>	mDescs;

public:
	InputLayout( ) { }

	InputLayout( InputElementDesc const* desc, uint count )
	{
		mDescs.assign( desc, desc + count );
	}

	const std::vector<InputElementDesc>& GetElementDescs( ) const
	{
		return mDescs;
	}
};