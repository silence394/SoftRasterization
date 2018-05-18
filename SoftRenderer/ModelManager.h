#pragma once

#include "Prerequisites.h"

class ModelManager
{
private:
	static std::unique_ptr< ModelManager > mInstance;
	ModelManager( ) { }

public:
	ModelManager& Instance( );

	StaticMeshPtr	LoadModel( std::wstring& resname );
};