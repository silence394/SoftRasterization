#pragma once

#include "prerequisites.h"
#include "vector3.h"

class Camera
{
private:
	Vector3	mPos;
	Vector3	mLookDir;
	Vector3	mUp;

public:
	Camera( );

	inline void SetPosition( const Vector3& pos )
		{ mPos = pos; }
	inline const Vector3& GetPosition( ) const
		{ return mPos; }

	inline void LookAt( const Vector3& target )
		{ mLookDir = ( target - mPos ).Normalize( ); }

	Matrix4	GetViewMatrix( );

	void	Pitch( float uints );
	void	Yaw( float uints );
	void	Roll( float units );
	void	Phi( float r );
	void	Theta( float r );
	void	Rotate( const Vector3& axis, float r );
};