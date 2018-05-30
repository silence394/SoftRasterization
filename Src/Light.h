#pragma once

#include "Prerequisites.h"

class Light
{
public:
	Color color;
	Light( ) { }
	Light( const Color& c ) : color( c ) { }
};

class AmbientLight : public Light
{
public:
	AmbientLight( ) { }
	AmbientLight( const Color& c ) : Light( c) { }
};

class SkyLight : public Light
{
public:
	Vector3 direction;
	SkyLight( ) { }
	SkyLight( const Color& c, const Vector3& dir ) : Light( c ), direction( dir ) { }
};