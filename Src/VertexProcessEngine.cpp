#include "VertexProcessEngine.h"
#include "Shaders.h"
#include "GraphicsBuffer.h"
#include "Vector2.h"
#include "RenderStates.h"
#include <algorithm>
#include "RenderDevice.h"

std::unique_ptr<VertexProcessEngine> VertexProcessEngine::mInstance = nullptr;
VertexProcessEngine& VertexProcessEngine::Instance( )
{
	if ( mInstance == nullptr )
		mInstance = std::unique_ptr<VertexProcessEngine> ( new VertexProcessEngine );

	return *mInstance; 
}

void VertexProcessEngine::Prepare( const VertexProcessContext& ctx  )
{
	mContext = ctx;
}

void VertexProcessEngine::FetchVertex3( PSInput** out, uint prim )
{
	GraphicsBufferPtr vertexbuffer = mContext.mVertexBuffer;
	uint vsize = vertexbuffer->GetSize( );
	byte* vb = (byte*) vertexbuffer->GetBuffer( ) + mContext.mVertexStart * vsize;
	ushort* ib = (ushort*) mContext.mIndexBuffer->GetBuffer( ) + mContext.mIndexStart + prim * 3;

	for ( uint i = 0; i < 3; i ++ )
	{
		uint index = *( ib + i );
		uint key = index % MC_VertexCache;
		auto& cache = mVertexCache[ key ];
		if ( cache.first == index )
		{
			out[i] = cache.second;
		}
		else
		{
			VSInput vsinput;
			{
				auto& descs = mContext.mInputLayout->GetElementDescs( );
				auto it = descs.begin( );
				byte* vbase = vb + index * vsize;

				uint attrindex = 0;
				while ( it != descs.end( ) )
				{
					uint format = it->mFormat;
					if ( format == GraphicsBuffer::BF_R32B32G32_FLOAT )
					{
						Vector3& vec3 = *(Vector3*) ( vbase + it->mOffset );
						vsinput.attribute( attrindex ) = Vector4( vec3.x, vec3.y, vec3.z, 1.0f );
					}
					else if ( format == GraphicsBuffer::BF_A8R8G8B8 )
					{
						Color c = *(uint*) ( vbase + it->mOffset );
						vsinput.attribute( attrindex ) = Vector4( c.r, c.g, c.b, c.a );
					}
					else if ( format == GraphicsBuffer::BF_R32G32_FLOAT )
					{
						Vector2& vec2 = *(Vector2*) ( vbase + it->mOffset );
						vsinput.attribute( attrindex ) = Vector4( vec2.x, vec2.y, 0.0f, 0.0f );
					}

					it ++;
					attrindex ++;
				}
			}

			PSInput& psinput = mVertexPool[ index ];
			mContext.mVertexShader->Execute( vsinput, psinput, mContext.mVSConstantBuffer );

			cache = std::make_pair( index, &psinput );
			out[i] = &psinput;
		}
	}
}

void VertexProcessEngine::Cull( PSInput** in )
{
	bool infrustum = true;
	for ( uint i = 0; i < 3; i ++ )
	{
		if ( in[i]->position( ).z < 0 )
		{
			infrustum = false;
			break;
		}
	}

	auto culltest = [&] ( PSInput** in )
	{
		ECullMode cullmode = mContext.mRasterizerState->GetCullMode( );
		if ( cullmode == ECullMode::ECM_NONE )
			return true;

		Vector2 pri[3];
		for ( uint i = 0; i < 3; i ++ )
		{
			float invw = 1.0f / in[i]->position( ).w;
			pri[i].x = in[i]->position( ).x * invw;
			pri[i].y = in[i]->position( ).y * invw;
		}

		float cross = ( pri[2].x - pri[0].x ) * ( pri[2].y - pri[1].y ) - ( pri[2].y - pri[0].y ) * ( pri[2].x - pri[1].x );
		return cullmode == ECullMode::ECM_BACK ? cross <= 0.0f : cross > 0.0f;
	};

	EFillMode fillmode = mContext.mRasterizerState->GetFillMode( );
	uint varyingcount = mContext.mVertexShader->GetVaryingCount( );
	if ( infrustum )
	{
		if ( culltest( in ) )
		{
			if ( fillmode == EFillMode::FM_SOLID )
			{
				mRasterizerVertex.push_back( in[0] );
				mRasterizerVertex.push_back( in[1] );
				mRasterizerVertex.push_back( in[2] );
			}
			else
			{
				for ( uint i = 0; i < 3; i ++ )
				{
					mRasterizerVertex.push_back( in[ i ] );
					mRasterizerVertex.push_back( in[ ( i + 1 ) % 3 ] );
				}
			}
		}
	}
	else
	{
		PSInput* clippedvertexs[5];
		float neartest1 = in[0]->position( ).z;
		uint clipnum = 0;

		for ( uint i = 0, j = 1; i < 3; i ++, j ++ )
		{
			j %= 3;
			if ( neartest1 >= 0.0 )
			{
				clippedvertexs[ clipnum ++ ] = in[i];

				if ( in[j]->position( ).z < 0 )
				{
					mClippedVertex.resize( mClippedVertex.size( ) + 1 );
					PSInput* input = &mClippedVertex.back( );

					float factor = neartest1 / ( neartest1 - in[j]->position( ).z );
					PSInput::Lerp( *input, *in[i], *in[j], factor );

					clippedvertexs[ clipnum ++ ] = input;
				}
			}
			else
			{
				if ( in[j]->position( ).z >= 0 )
				{
					mClippedVertex.resize( mClippedVertex.size( ) + 1 );
					PSInput* input = &mClippedVertex.back( );

					float factor = in[j]->position( ).z / ( in[j]->position( ).z - neartest1 );
					PSInput::Lerp( *input,  *in[j], *in[i], factor );

					clippedvertexs[ clipnum ++ ] = input;
				}
			}

			neartest1 = in[j]->position( ).z; 
		}

		if ( clipnum >= 3 )
		{
			if ( culltest( clippedvertexs ) )
			{
				if ( fillmode == EFillMode::FM_SOLID )
				{
					for ( uint i = 1; i < clipnum - 1; i ++ )
					{
						mRasterizerVertex.push_back( clippedvertexs[0] );
						mRasterizerVertex.push_back( clippedvertexs[ i ] );
						mRasterizerVertex.push_back( clippedvertexs[ i + 1] );
					}
				}
				else
				{
					for ( uint i = 0; i < clipnum; i ++ )
					{
						mRasterizerVertex.push_back( clippedvertexs[ i ] );
						mRasterizerVertex.push_back( clippedvertexs[ ( i + 1 ) % clipnum ] );
					}
				}
			}
		}
	}
}

std::vector<PSInput*>& VertexProcessEngine::Process(  )
{
	mRasterizerVertex.clear( );
	if ( mContext.IsValid( ) == false )
		return mRasterizerVertex;

	for ( uint i = 0; i < MC_VertexCache; i ++ )
		mVertexCache[i] = std::make_pair( -1, nullptr );

	uint vsize = mContext.mVertexBuffer->GetSize( );
	uint vlen = mContext.mVertexBuffer->GetLength( );
	mVertexPool.resize( vlen / vsize );

	GraphicsBufferPtr indexbuffer =mContext. mIndexBuffer;
	uint isize = mContext.mIndexBuffer->GetSize( );
	uint indexcount = mContext.mIndexCount;
	indexcount = Math::Clamp( indexcount, (uint) 0, indexbuffer->GetLength( ) / isize );
	
	uint primcount = indexcount / 3;
	for ( uint i = 0; i < primcount; i ++ )
	{
		PSInput* psinputs[3];
		FetchVertex3( psinputs, i );
		Cull( psinputs );
	}

	return mRasterizerVertex;
}