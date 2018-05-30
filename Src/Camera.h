#pragma once

#include "Prerequisites.h"
#include "Vector3.h"

class Camera
{
private:
	Vector3	mPos;
	Vector3	mLookDir;
	Vector3	mUp;
	float	mLookDistance;

public:
	Camera( );

	void SetPosition( const Vector3& pos )
	{
		mPos = pos;
	}

	const Vector3& GetPosition( ) const
	{
		return mPos;
	}

	inline void LookAt( const Vector3& target )
	{
		Vector3 look = target - mPos;
		mLookDistance = look.Magnitude( );
		mLookDir = look.Normalize( );
	}

	Vector3 GetDiretion( )
	{
		return mLookDir;
	}
	float GetLookDistance( )
	{
		return mLookDistance;
	}

	Matrix4	GetViewMatrix( );

	void	Phi( float r );
	void	Theta( float r );
	void	Zoom( float units );
};