#include "Matrix4.h"
#include "Vector3.h"
#include "Math.h"

const Matrix4 Matrix4::identity(1.0f, 0.0f, 0.0f, 0.0f,
								0.0f, 1.0f, 0.0f, 0.0f,
								0.0f, 0.0f, 1.0f, 0.0f, 
								0.0f, 0.0f, 0.0f, 1.0f);

Matrix4 Matrix4::Perspective( float fov, float aspect, float znear, float zfar )
{
	float ys = 1.0f / ::tan( fov / 2.0f );
	float xs = ys / aspect;
	float zf = zfar / ( zfar - znear );
	float zn = - znear * zf;

	return Matrix4( xs,	0.0f, 0.0f, 0.0f,
					0.0f, ys, 0.0f, 0.0f,
					0.0f, 0.0f, zf, 1.0f,
					0.0f, 0.0f, zn, 0.0f );
}

Matrix4& Matrix4::RotationX( float r )
{
	float sinvalue = Math::Sin( r ), cosvalue = Math::Cos( r );

	m[0] = 1.0f;	m[1] = 0.0f;		m[2] = 0.0f;		m[3] = 0.0f;
	m[4] = 0.0f;	m[5] = cosvalue;	m[6] = sinvalue;	m[7] = 0.0f;
	m[8] = 0.0f;	m[9] = - sinvalue;	m[10] = cosvalue;	m[11] = 0.0f;
	m[12] = 0.0f;	m[13] = 0.0f;		m[14] = 0.0f;		m[15] = 1.0f;

	return *this;
}

Matrix4& Matrix4::RotationY( float r )
{
	float sinvalue = Math::Sin( r ), cosvalue = Math::Cos( r );

	m[0] = cosvalue;	m[1] = 0.0f;		m[2] = -sinvalue;	m[3] = 0.0f;
	m[4] = 0.0f;		m[5] = 1.0f;		m[6] = 0.0f;		m[7] = 0.0f;
	m[8] = sinvalue;	m[9] = 0.0f;		m[10] = cosvalue;	m[11] = 0.0f;
	m[12] = 0.0f;		m[13] = 0.0f;		m[14] = 0.0f;		m[15] = 1.0f;

	return *this;
}

Matrix4& Matrix4::RotationZ( float r )
{
	float sinvalue = Math::Sin( r ), cosvalue = Math::Cos( r );

	m[0] = cosvalue;	m[1] = sinvalue;	m[2] = 0.0f;	m[3] = 0.0f;
	m[4] = -sinvalue;	m[5] = cosvalue;	m[6] = 0.0f;	m[7] = 0.0f;
	m[8] = 0.0f;		m[9] = 0.0f;		m[10] = 1.0f;	m[11] = 0.0f;
	m[12] = 0.0f;		m[13] = 0.0f;		m[14] = 0.0f;	m[15] = 1.0f;

	return *this;
}

Matrix4& Matrix4::Transpose( )
{
	Matrix4 t(*this);

	m[0] = t.m[0];	m[1] = t.m[4];	m[2] = t.m[8];	m[3] = t.m[12];
	m[4] = t.m[1];	m[5] = t.m[5];	m[6] = t.m[9];	m[7] = t.m[13];
	m[8] = t.m[2];	m[9] = t.m[6];	m[10] = t.m[10];m[11] = t.m[14];
	m[12] = t.m[3];	m[13] = t.m[7];	m[14] = t.m[11];m[15] = t.m[15];

	return *this;
}

Matrix4& Matrix4::SetTrans( const Vector3& v )
{
	m[0] = 1.0f;
	m[1] = 0.0f;
	m[2] = 0.0f;
	m[3] = 0.0f;

	m[4] = 0.0f;
	m[5] = 1.0f;
	m[6] = 0.0f;
	m[7] = 0.0f;

	m[8] = 0.0f;
	m[9] = 0.0f;
	m[10] = 1.0f;
	m[11] = 0.0f;

	m[12] = v.x;
	m[13] = v.y;
	m[14] = v.z;
	m[15] = 1.0f;

	return *this;
}

Matrix4& Matrix4::SetRotation( const Vector3& v, float r )
{
	float sin = Math::Sin( r ), cos = Math::Cos( r );
	float cosreverse = 1 - cos;

	Vector3 n( v );
	n.Normalize( );

	m[0] = cosreverse * n.x * n.x + cos;
	m[1] = cosreverse * n.x * n.y + sin * n.z;
	m[2] = cosreverse * n.x * n.z - sin * n.y;
	m[3] = 0.0f;

	m[4] = cosreverse * n.x * n.y - sin * n.z;
	m[5] = cosreverse * n.y * n.y + cos;
	m[6] = cosreverse * n.y * n.z + sin * n.x;
	m[7] = 0.0f;

	m[8] = cosreverse * n.x * n.z + sin * n.y;
	m[9] = cosreverse * n.y * n.z - sin * n.x;
	m[10] = cosreverse * n.z * n.z + cos;
	m[11] = 0.0f;

	m[12] = 0.0f;
	m[13] = 0.0f;
	m[14] = 0.0f;
	m[15] = 1.0f;

	return *this;
}

Matrix4& Matrix4::SetScaling( float s )
{
	m[0] = s;
	m[1] = 0.0f;
	m[2] = 0.0f;
	m[3] = 0.0f;

	m[4] = 0.0f;
	m[5] = s;
	m[6] = 0.0f;
	m[7] = 0.0f;

	m[8] = 0.0f;
	m[9] = 0.0f;
	m[10] = s;
	m[11] = 0.0f;

	m[12] = 0.0f;
	m[13] = 0.0f;
	m[14] = 0.0f;
	m[15] = 1.0f;

	return *this;
}

Matrix4 Matrix4::operator + ( const Matrix4& mat ) const
{
	Matrix4 ret( *this );
	return ret -= mat;
}

Matrix4& Matrix4::operator += ( const Matrix4& mat )
{
	m[0] += mat[0];
	m[1] += mat[1];
	m[2] += mat[2];
	m[3] += mat[3];
	m[4] += mat[4];
	m[5] += mat[5];
	m[6] += mat[6];
	m[7] += mat[7];
	m[8] += mat[8];
	m[9] += mat[9];
	m[10] += mat[10];
	m[11] += mat[11];
	m[12] += mat[12];
	m[13] += mat[13];
	m[14] += mat[14];
	m[15] += mat[15];

	return *this;
}

Matrix4 Matrix4::operator - ( const Matrix4& mat ) const
{
	Matrix4 ret( *this );
	return ret -= mat;
}

Matrix4& Matrix4::operator -= ( const Matrix4& mat )
{
	return operator += ( - mat );
}

Matrix4 Matrix4::operator * ( const Matrix4& mat ) const
{
	Matrix4 ret( *this );
	return ret *= mat;
}

Matrix4& Matrix4::operator *= (const Matrix4& mat)
{
	Matrix4 t(*this);

	m[0]  = t.m[0]  * mat.m[0] + t.m[1]  * mat.m[4] + t.m[2]  * mat.m[8]  + t.m[3]  * mat.m[12];
	m[1]  = t.m[0]  * mat.m[1] + t.m[1]  * mat.m[5] + t.m[2]  * mat.m[9]  + t.m[3]  * mat.m[13];
	m[2]  = t.m[0]  * mat.m[2] + t.m[1]  * mat.m[6] + t.m[2]  * mat.m[10] + t.m[3]  * mat.m[14];
	m[3]  = t.m[0]  * mat.m[3] + t.m[1]  * mat.m[7] + t.m[2]  * mat.m[11] + t.m[3]  * mat.m[15];
	m[4]  = t.m[4]  * mat.m[0] + t.m[5]  * mat.m[4] + t.m[6]  * mat.m[8]  + t.m[7]  * mat.m[12];
	m[5]  = t.m[4]  * mat.m[1] + t.m[5]  * mat.m[5] + t.m[6]  * mat.m[9]  + t.m[7]  * mat.m[13];
	m[6]  = t.m[4]  * mat.m[2] + t.m[5]  * mat.m[6] + t.m[6]  * mat.m[10] + t.m[7]  * mat.m[14];
	m[7]  = t.m[4]  * mat.m[3] + t.m[5]  * mat.m[7] + t.m[6]  * mat.m[11] + t.m[7]  * mat.m[15];
	m[8]  = t.m[8]  * mat.m[0] + t.m[9]  * mat.m[4] + t.m[10] * mat.m[8]  + t.m[11] * mat.m[12];
	m[9]  = t.m[8]  * mat.m[1] + t.m[9]  * mat.m[5] + t.m[10] * mat.m[9]  + t.m[11] * mat.m[13];
	m[10] = t.m[8]  * mat.m[2] + t.m[9]  * mat.m[6] + t.m[10] * mat.m[10] + t.m[11] * mat.m[14];
	m[11] = t.m[8]  * mat.m[3] + t.m[9]  * mat.m[7] + t.m[10] * mat.m[11] + t.m[11] * mat.m[15];
	m[12] = t.m[12] * mat.m[0] + t.m[13] * mat.m[4] + t.m[14] * mat.m[8]  + t.m[15] * mat.m[12];
	m[13] = t.m[12] * mat.m[1] + t.m[13] * mat.m[5] + t.m[14] * mat.m[9]  + t.m[15] * mat.m[13];
	m[14] = t.m[12] * mat.m[2] + t.m[13] * mat.m[6] + t.m[14] * mat.m[10] + t.m[15] * mat.m[14];
	m[15] = t.m[12] * mat.m[3] + t.m[13] * mat.m[7] + t.m[14] * mat.m[11] + t.m[15] * mat.m[15];

	return *this;
}