#include "camera.h"
#include "matrix4.h"

Camera::Camera( )
{
	mPos = Vector3( 0.0f, 0.0f, 0.0f );
	mLookAt = Vector3( 0.0f, 1.0f, 0.0f );
	mUp = Vector3( 0.0f, 0.0f, 1.0f );
}

Matrix4 Camera::GetViewMatrix( )
{
	Vector3 zaxis = mLookAt.Normalize( );
	Vector3 xaxis = Vector3::Cross( mUp, zaxis ).Normalize( );
	Vector3 yaxis = Vector3::Cross( zaxis, xaxis );

	float xeye = - Vector3::Dot( xaxis, mPos );
	float yeye = - Vector3::Dot( yaxis, mPos );
	float zeye = - Vector3::Dot( zaxis, mPos );

	return Matrix4( xaxis.x, yaxis.x, zaxis.x, 0.0f,
					xaxis.y, yaxis.y, zaxis.y, 0.0f,
					xaxis.z, yaxis.z, zaxis.z, 0.0f,
					xeye,    yeye,    zeye,	   1.0f );
}

void Camera::Pitch( float uints )
{

}

void Camera::Yaw( float uints )
{

}

void Camera::Roll( float units )
{

}

void Camera::Theta( float r )
{

}

void Camera::Rotate( const Vector3& axis, float r )
{

}