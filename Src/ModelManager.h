#pragma once

#include "Prerequisites.h"

class ModelManager
{
private:
	static std::unique_ptr< ModelManager > mInstance;
	ModelManager( ) { }

public:
	static ModelManager&	Instance( );

	StaticMeshPtr			LoadModel( std::wstring& resname );
	StaticMeshPtr			CreateCube( );
};