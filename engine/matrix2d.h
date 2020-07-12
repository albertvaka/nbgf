#pragma once
//Based on the work of: Mat Buckland (fup@ai-junkie.com)

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>

#include "vec.h"

struct Matrix
{
	Matrix()
	{
		//initialize the matrix to an identity matrix
		Identity();
	}

	//create a transformation matrix
	inline void Translate(float x, float y);

	//create a scale matrix
	inline void Scale(float xScale, float yScale);

	//create a rotation matrix
	inline void  Rotate(float rotation);

	//create a rotation matrix from a fwd and side 2D vector
	inline void  Rotate(const vec &fwd, const vec &side);

	//applys a transformation matrix to a std::vector of points
	inline void TransformVecs(std::vector<vec> &vPoints);

	//applys a transformation matrix to a point
	inline void TransformVec(vec &vPoint);

	inline void Zero() {
		_11 = 0.0; _12 = 0.0; _13 = 0.0;
		_21 = 0.0; _22 = 0.0; _23 = 0.0;
		_31 = 0.0; _32 = 0.0; _33 = 0.0;
	}

	//create an identity matrix
	inline void Identity() {
		_11 = 1; _12 = 0; _13 = 0;
		_21 = 0; _22 = 1; _23 = 0;
		_31 = 0; _32 = 0; _33 = 1;
	}

	float _11, _12, _13;
	float _21, _22, _23;
	float _31, _32, _33;

	//multiplies with mIn
	inline void  MatrixMultiply(Matrix& mIn);
};

//multiply two matrices together
inline void Matrix::MatrixMultiply(Matrix &mIn)
{
	Matrix aux = *this;
  
	//first row
	_11 = (aux._11*mIn._11) + (aux._12*mIn._21) + (aux._13*mIn._31);
	_12 = (aux._11*mIn._12) + (aux._12*mIn._22) + (aux._13*mIn._32);
	_13 = (aux._11*mIn._13) + (aux._12*mIn._23) + (aux._13*mIn._33);

	//second
	_21 = (aux._21*mIn._11) + (aux._22*mIn._21) + (aux._23*mIn._31);
	_22 = (aux._21*mIn._12) + (aux._22*mIn._22) + (aux._23*mIn._32);
	_23 = (aux._21*mIn._13) + (aux._22*mIn._23) + (aux._23*mIn._33);

	//third
	_31 = (aux._31*mIn._11) + (aux._32*mIn._21) + (aux._33*mIn._31);
	_32 = (aux._31*mIn._12) + (aux._32*mIn._22) + (aux._33*mIn._32);
	_33 = (aux._31*mIn._13) + (aux._32*mIn._23) + (aux._33*mIn._33);
}

//applies a 2D transformation matrix to a std::vector of vecs
inline void Matrix::TransformVecs(std::vector<vec> &vPoint)
{
	for (unsigned int i=0; i<vPoint.size(); ++i)
	{
		float tempX =(_11*vPoint[i].x) + (_21*vPoint[i].y) + (_31);
		float tempY = (_12*vPoint[i].x) + (_22*vPoint[i].y) + (_32);
  
		vPoint[i].x = tempX;
		vPoint[i].y = tempY;
	}
}

//applies a 2D transformation matrix to a single vec
inline void Matrix::TransformVec(vec &vPoint)
{
	float tempX =(_11*vPoint.x) + (_21*vPoint.y) + (_31);
	float tempY = (_12*vPoint.x) + (_22*vPoint.y) + (_32);
  
	vPoint.x = tempX;
	vPoint.y = tempY;
}

//create a transformation matrix
inline void Matrix::Translate(float x, float y)
{
	Matrix mat;
  
	mat._11 = 1; mat._12 = 0; mat._13 = 0;
	mat._21 = 0; mat._22 = 1; mat._23 = 0;
	mat._31 = x; mat._32 = y; mat._33 = 1;
  
	//and multiply
	MatrixMultiply(mat);
}

//create a scale matrix
inline void Matrix::Scale(float xScale, float yScale)
{
  Matrix mat;
  
  mat._11 = xScale; mat._12 = 0;      mat._13 = 0;
  mat._21 = 0;      mat._22 = yScale; mat._23 = 0;
  mat._31 = 0;      mat._32 = 0;      mat._33 = 1;
  
  //and multiply
  MatrixMultiply(mat);
}

//create a rotation matrix
inline void Matrix::Rotate(float rot)
{
  Matrix mat;

  float Sin = sin(rot);
  float Cos = cos(rot);
  
  mat._11 = Cos;  mat._12 = Sin; mat._13 = 0;
  mat._21 = -Sin; mat._22 = Cos; mat._23 = 0;
  mat._31 = 0; mat._32 = 0;mat._33 = 1;
  
  //and multiply
  MatrixMultiply(mat);
}

//create a rotation matrix from a 2D vector
inline void Matrix::Rotate(const vec &fwd, const vec &side)
{
	Matrix mat;

	mat._11 = fwd.x;  mat._12 = fwd.y;  mat._13 = 0;
	mat._21 = side.x; mat._22 = side.y; mat._23 = 0;
	mat._31 = 0;      mat._32 = 0;      mat._33 = 1;

	//and multiply
	MatrixMultiply(mat);
}


//--------------------- PointToWorldSpace --------------------------------
//
//  Transforms a point from the agent's local space into world space
//------------------------------------------------------------------------
inline vec PointToWorldSpace(const vec &point, const vec &AgentHeading, const vec &AgentPosition)
{
	
	vec AgentSide = AgentHeading.Perp();

	//make a copy of the point
	vec TransPoint = point;

	//create a transformation matrix
	Matrix matTransform;

	//rotate
	matTransform.Rotate(AgentHeading, AgentSide);

	//and translate
	matTransform.Translate(AgentPosition.x, AgentPosition.y);

	//now transform the vertices
	matTransform.TransformVec(TransPoint);

	return TransPoint;
}

//--------------------- VectorToWorldSpace --------------------------------
//
//  Transforms a vector from the agent's local space into world space
//------------------------------------------------------------------------
inline vec VectorToWorldSpace(const vec &point, const vec &AgentHeading)
{
	vec AgentSide = AgentHeading.Perp();

	//make a copy of the point
	vec TransVec = point;

	//create a transformation matrix
	Matrix matTransform;

	//rotate
	matTransform.Rotate(AgentHeading, AgentSide);

	//now transform the vertices
	matTransform.TransformVec(TransVec);

	return TransVec;
}

//--------------------- PointToLocalSpace --------------------------------
//
//------------------------------------------------------------------------
inline vec PointToLocalSpace(const vec &point, const vec &AgentHeading, const vec &AgentPosition)
{
	vec AgentSide = AgentHeading.Perp();

	//make a copy of the point
	vec TransPoint = point;

	//create a transformation matrix
	Matrix matTransform;

	float Tx = -AgentPosition.Dot(AgentHeading);
	float Ty = -AgentPosition.Dot(AgentSide);

	//create the transformation matrix
	matTransform._11 = AgentHeading.x; matTransform._12 = AgentSide.x;
	matTransform._21 = AgentHeading.y; matTransform._22 = AgentSide.y;
	matTransform._31 = Tx;           matTransform._32 = Ty;

	//now transform the vertices
	matTransform.TransformVec(TransPoint);

	return TransPoint;
}

//--------------------- VectorToLocalSpace --------------------------------
//
//------------------------------------------------------------------------
inline vec VectorToLocalSpace(const vec & point, const vec &AgentHeading, const vec &AgentSide)
{ 
	//make a copy of the point
	vec TransPoint = point;

	//create a transformation matrix
	Matrix matTransform;

	//create the transformation matrix
	matTransform._11 = AgentHeading.x; matTransform._12 = AgentSide.x;
	matTransform._21 = AgentHeading.y; matTransform._22 = AgentSide.y;

	//now transform the vertices
	matTransform.TransformVec(TransPoint);

	return TransPoint;
}
