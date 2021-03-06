#include "Camera.h"
#include "Matrix4.h"

Camera::Camera( )
{
	mPos = Vector3( 0.0f, 0.0f, 0.0f );
	mLookDir = Vector3( 0.0f, 1.0f, 0.0f );
	mUp = Vector3( 0.0f, 1.0f, 0.0f );
}

Matrix4 Camera::GetViewMatrix( )
{
	Vector3 zaxis = mLookDir.Normalize( );
	Vector3 xaxis = Vector3::Cross( mUp, zaxis ).Normalize( );
	Vector3 yaxis = Vector3::Cross( zaxis, xaxis );

	float xeye = - Vector3::Dot( xaxis, mPos );
	float yeye = - Vector3::Dot( yaxis, mPos );
	float zeye = - Vector3::Dot( zaxis, mPos );

	return Matrix4( xaxis.x, yaxis.x, zaxis.x, 0.0f,
					xaxis.y, yaxis.y, zaxis.y, 0.0f,
					xaxis.z, yaxis.z, zaxis.z, 0.0f,
					xeye,	 yeye,	  zeye,	   1.0f );
}

void Camera::Phi( float r )
{
	Vector3 lookat = mPos + mLookDir * mLookDistance;
	mPos *=   Matrix4( ).SetTrans( - lookat ) * Matrix4( ).SetRotation( mUp, r ) * Matrix4( ).SetTrans( lookat ); 
	lookat -= mPos;
	mLookDistance = lookat.Magnitude( );
	mLookDir = lookat.Normalize( );
}

void Camera::Theta( float r )
{
	Vector3 right = Vector3::Cross( mUp, mLookDir ).Normalize( );
	Vector3 v1 = Vector3::Cross( -mLookDir, right );

	Vector3 look = mPos + mLookDir * mLookDistance;
	mPos *= Matrix4( ).SetTrans( - look ) * Matrix4( ).SetRotation( right, r ) * Matrix4( ).SetTrans( look );

	look -= mPos;
	mLookDistance = look.Magnitude( );
	mLookDir = look.Normalize( );

	Vector3 v2 = Vector3::Cross( -mLookDir, right );
	if ( Vector3::Dot( v1, mUp ) * Vector3::Dot( v2, mUp ) < 0.0f )
		mUp = - mUp;
}

void Camera::Zoom( float units )
{
	Vector3 look = mPos + mLookDir * mLookDistance;
	Vector3 d = -mLookDir;
	Vector3 v = mPos + d.Normalize( ) * units;

	if ( v == look )
		return;

	if ( Vector3::Dot( mLookDir, look - v ) > 0.0f )
		mPos = v;

	look -= mPos;
	mLookDistance = look.Magnitude( );
	mLookDir = look.Normalize( );
}