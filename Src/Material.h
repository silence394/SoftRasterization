#pragma once

#include "Prerequisites.h"
#include "Vector3.h"

class Material
{
public:
	Vector3	ambient;
	Vector3	diffuse;
	Vector3	specular;
	float	shiness;
};