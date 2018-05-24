#include "StaticMesh.h"
#include "renderdevice.h"
#include "graphicsbuffer.h"

StaticMesh::StaticMesh( const std::vector<StaticMeshResouce>& resources )
{
	mResources = resources;
}

void StaticMesh::Draw( )
{
	RenderDevice& rd = RenderDevice::Instance( );
	std::vector<StaticMeshResouce>::iterator it = mResources.begin( );
	while( it != mResources.end( ) )
	{
		rd.SetVertexBuffer( it->mVertexBuffer );
		rd.SetIndexBuffer( it->mIndexBuffer );

		//rd.DrawIndex( 6, 138, 0 );
		rd.DrawIndex( it->mIndexBuffer->GetLength( ) / it->mIndexBuffer->GetSize( ), 0, 0 );
		it ++;
	}
}