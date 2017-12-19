#pragma once

#include "prerequisites.h"

class GraphicsBuffer
{
public:
	enum BufferType
	{
		BT_VERTEX,
		BT_INDEX,
	};

private:
	uint	mType;
	void*	mBuffer;

public:
	GraphicsBuffer( ) { }
};