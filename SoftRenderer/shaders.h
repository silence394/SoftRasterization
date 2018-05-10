#pragma once

#include "prerequisites.h"
#include "matrix4.h"
#include "color.h"
#include <unordered_map>

#define _MAX_VSINPUT_COUNT 2
#define _MAX_PSINPUT_COUNT 2

class VSInput
{
private:
	Vector4 mShaderRigisters[ _MAX_VSINPUT_COUNT + 1 ];

public:
	Vector4& attribute( uint index )
		{ return mShaderRigisters[ index ]; }
};

class PSInput
{
private:
	Vector4 mShaderRigisters[ _MAX_PSINPUT_COUNT + 1 ];

public:
	Vector4& position( )
		{ return mShaderRigisters[0]; }

	const Vector4& position( ) const
		{ return mShaderRigisters[0]; }

	Vector4& attribute( uint index )
		{ return mShaderRigisters[ index + 1 ]; }

	const Vector4& attribute( uint index ) const
		{ return mShaderRigisters[ index + 1 ]; }
};

class PSOutput
{
public:
	Color color;
};

class ConstantBuffer
{
private:
	struct ConstantElement
	{
		uint	offset;
		uint	typecode;

		ConstantElement( uint off, uint hashcode ) : offset( off ), typecode( hashcode ) { }
	};

	friend class RenderDevice;
	typedef std::unordered_map<char*, ConstantElement> ConstantMap;
	ConstantMap			mConstantMap;
	std::vector<byte>	mDatas;

	ConstantBuffer( ) { }

public:
	template <typename T>
	void AddConstant( char* str, const T& vec )
	{
		ConstantMap::iterator it = mConstantMap.find( str );
		if ( it != mConstantMap.end( ) )
			return;

		ConstantElement element( mDatas.size( ), typeid( T ).hash_code( ) );
		mConstantMap.insert( std::pair<char*, ConstantElement> ( str, element ) );

		uint offset = mDatas.size( );
		mDatas.resize( mDatas.size( ) + sizeof( T ) );
		*reinterpret_cast<T*> (&mDatas[offset]) = vec;
	}

	template <typename T>
	const T& GetConstant( char* str )
	{
		ConstantMap::iterator it = mConstantMap.find( str );
		assert( it != mConstantMap.end( ) && it->second.typecode == typeid( T ).hash_code( ) );

		return *reinterpret_cast<T*> ( &mDatas[ it->second.offset ] );
	}

	template <typename T>
	void SetConstant( char* str, const T& constant )
	{
		ConstantMap::iterator it = mConstantMap.find( str );
		if ( it != mConstantMap.end( ) )
			*reinterpret_cast<T*> ( &mDatas[ it->second.offset ] ) = constant;
		else
			AddConstant( str, constant );
	}
};

class IVertexShader
{
public:
	virtual void Execute( VSInput& in, PSInput& out, ConstantBufferPtr* cb ) = 0;
};

class IPixelShader
{
public:
	virtual void Execute( PSInput& in, PSOutput& out, float& depth, ConstantBufferPtr* cb ) = 0;
	virtual uint SampleTexture( uint index, float u, float v );
	virtual Color Texture2D( uint index, float u, float v );
};