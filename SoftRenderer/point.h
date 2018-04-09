#pragma once

#include "prerequisites.h"

class Point
{
public:
	int x, y;

public:
	Point( ) : x ( 0 ), y( 0 )
		{ }
	Point( int px, int py ) : x( px ), y( py )
		{ }
};