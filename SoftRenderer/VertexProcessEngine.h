#pragma once

#include "prerequisites.h"

class VertexProcessEngine
{
private:
	static std::unique_ptr<VertexProcessEngine>	mInstance;

	VertexProcessEngine( ) { }

public:
	static VertexProcessEngine& Instance( );
};