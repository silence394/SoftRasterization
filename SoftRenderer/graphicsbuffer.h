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

public:
	GraphicsBuffer( )
		: mBuffer( nullptr ), mLength( 0 ){ }

	GraphicsBuffer( void* buffer, uint len )
		: mBuffer( buffer ), mLength( ) { }

	~GraphicsBuffer( )
		{ delete[] mBuffer; }

	inline void* GetBuffer( ) const
		{ return mBuffer; }
	inline uint GetLength( ) const
		{ return mLength; }
};

struct InputElementDesc
{
	std::string	mName;
	uint		mFormat;
	uint		mOffset;

	InputElementDesc( std::string name, uint format )
		: mName( name ), mFormat( format ) { }
};

class InputLayout
{
private:
	std::vector<InputElementDesc>	mDescs;

public:
	InputLayout( ) { }

	InputLayout( InputElementDesc const* desc, uint count )
		{ mDescs.assign( desc, desc + count ); }
};