#include "VertexProcessEngine.h"

std::unique_ptr<VertexProcessEngine> VertexProcessEngine::mInstance = nullptr;

VertexProcessEngine& VertexProcessEngine::Instance( )
{
	if ( mInstance == nullptr )
		mInstance = std::unique_ptr<VertexProcessEngine> ( new VertexProcessEngine );

	return *mInstance;
}

void VertexProcessEngine::Prepare( )
{

}

void VertexProcessEngine::Process( const VertexProcessContext& context )
{

}