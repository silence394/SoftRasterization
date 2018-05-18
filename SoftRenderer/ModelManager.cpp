#include "ModelManager.h"
#include "String.h"
#include "StaticMesh.h"
#include "Assimp/importer.hpp"
#include "Assimp/scene.h"
#include "Assimp/postprocess.h"
#include "renderdevice.h"
#include "Vector2.h"
#include "Vector3.h"

std::unique_ptr< ModelManager > ModelManager::mInstance = nullptr;
ModelManager& ModelManager::Instance( )
{
	if ( mInstance == nullptr )
		mInstance = std::unique_ptr< ModelManager > ( new ModelManager );

	return *mInstance;
}

StaticMeshPtr ModelManager::LoadModel( std::wstring& resname )
{
	std::string filename;
	WStr2Str( resname, filename );

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile( filename, aiProcess_GenNormals| aiProcess_GenUVCoords | aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType );
	if ( scene == nullptr || scene->mNumMeshes == 0 )
		return nullptr;

	RenderDevice& rd = RenderDevice::Instance( );
	std::vector<StaticMeshResouce> resources;

	bool hasnormal = scene->mMeshes[0]->HasNormals( );
	bool hasuv = scene->mMeshes[0]->HasTextureCoords( 0 );

	uint vsize = sizeof( Vector3 );
	if ( hasnormal )
		vsize += sizeof( Vector3 );
	if ( hasuv )
		vsize += sizeof( Vector2 );

	for ( uint i = 0; i < scene->mNumMeshes; i ++ )
	{
		std::vector<byte> vertexs;
		std::vector<Vector3> positions;
		std::vector<Vector3> normals;
		std::vector<Vector2> texcoords;
		std::vector<ushort> indices;

		const aiMesh* mesh = scene->mMeshes[i];

		vertexs.resize( mesh->mNumVertices * vsize );

		byte* vbuffer = vertexs.data( );

		byte* vptr = vbuffer;
		for ( uint j = 0; j < mesh->mNumVertices; j ++ )
		{
			Vector3 pos;
			pos.x = mesh->mVertices[j].x;
			pos.y = mesh->mVertices[j].y;
			pos.z = mesh->mVertices[j].z;

			* reinterpret_cast<Vector3*> ( vptr + j * vsize ) = pos;
		}

		vptr = vbuffer + sizeof( Vector3 );
		if ( mesh->HasNormals( ) )
		{
			for ( uint j = 0; j < mesh->mNumVertices; j ++ )
			{
				Vector3 nor;
				nor.x = mesh->mNormals[j].x;
				nor.y = mesh->mNormals[j].y;
				nor.z = mesh->mNormals[j].z;

				* reinterpret_cast<Vector3*> ( vptr + j * vsize ) = nor;
			}
		}

		vptr = vbuffer + sizeof( Vector3 ) + sizeof( Vector3 );
		if ( mesh->HasTextureCoords( 0 ) )
		{
			for ( uint j = 0; j < mesh->mNumVertices; j ++ )
			{
				Vector2 vec;
				vec.x = mesh->mTextureCoords[0][j].x;
				vec.x = mesh->mTextureCoords[0][j].x;

				* reinterpret_cast<Vector2*> ( vptr + j * vsize ) = vec;
			}
		}

		for ( uint j = 0; j < mesh->mNumFaces; j ++ )
		{
			indices.push_back( mesh->mFaces[j].mIndices[0] );
			indices.push_back( mesh->mFaces[j].mIndices[1] );
			indices.push_back( mesh->mFaces[j].mIndices[2] );
		}

		GraphicsBufferPtr indexbuffer = rd.CreateBuffer( indices.data( ), indices.size( ) * sizeof( ushort ), sizeof( ushort ) );
		GraphicsBufferPtr vertexbuffer = rd.CreateBuffer( vertexs.data( ), vertexs.size( ), vsize );
	}

	return StaticMeshPtr( new StaticMesh( resources ) );
}