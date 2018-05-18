#pragma once

#include "prerequisites.h"

struct StaticMeshResouce
{
	GraphicsBufferPtr	mVertexBuffer;
	GraphicsBufferPtr	mIndexBuffer;
};

class StaticMesh
{
public:
	enum EVertexFormat
	{
		VF_POSITION		= 0x01,
		VF_NORMAL		= 0x02,
		VF_TEXCOORD0	= 0x04,
	};

private:
	std::vector<StaticMeshResouce>	mResources;

public:
	StaticMesh( const std::vector<StaticMeshResouce>& resources );
};