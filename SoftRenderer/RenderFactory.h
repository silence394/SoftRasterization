#pragma once

#include "prerequisites.h"

class BaseRenderer
{
private:
	std::unique_ptr<BaseRenderer>	mInstance;

	BaseRenderer( ) { }

public:
	void Render( const StaticMeshPtr& mesh, const RenderTechnique& tech );
};