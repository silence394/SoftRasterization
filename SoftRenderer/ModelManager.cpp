#include "ModelManager.h"
#include "String.h"
#include "StaticMesh.h"

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

	return StaticMeshPtr( new StaticMesh( ) );
}