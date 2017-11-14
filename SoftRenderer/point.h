#pragma once

class Point
{
public:
	unsigned int x, y;

public:
	Point( ) : x ( 0 ), y( 0 )
		{ }
	Point( int px, int py ) : x( px ), y( py )
		{ }
};