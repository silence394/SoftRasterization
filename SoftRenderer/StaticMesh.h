#pragma once

#include "prerequisites.h"

struct StaticMeshResouce
{
	GraphicsBufferPtr	mVertexBuffer;
	GraphicsBufferPtr	mIndexBuffer;
};

class StaticMesh
{
private:
	std::vector<StaticMeshResouce>	mResources;

public:
	StaticMesh( const std::vector<StaticMeshResouce>& resources );
};