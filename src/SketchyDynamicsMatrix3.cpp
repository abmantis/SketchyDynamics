/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2009 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#include "SketchyDynamicsMatrix3.h"


// Adapted from Matrix math by Wild Magic http://www.geometrictools.com/

namespace SketchyDynamics
{
    const float Matrix3::EPSILON = 1e-06f;
    const Matrix3 Matrix3::ZERO(0,0,0,0,0,0,0,0,0);
    const Matrix3 Matrix3::IDENTITY(1,0,0,0,1,0,0,0,1);

    //-----------------------------------------------------------------------
    bool Matrix3::operator== (const Matrix3& rkMatrix) const
    {
        for (size_t iRow = 0; iRow < 3; iRow++)
        {
            for (size_t iCol = 0; iCol < 3; iCol++)
            {
                if ( _m[iRow][iCol] != rkMatrix._m[iRow][iCol] )
                    return false;
            }
        }

        return true;
    }
    //-----------------------------------------------------------------------
    Matrix3 Matrix3::operator+ (const Matrix3& rkMatrix) const
    {
        Matrix3 kSum;
        for (size_t iRow = 0; iRow < 3; iRow++)
        {
            for (size_t iCol = 0; iCol < 3; iCol++)
            {
                kSum._m[iRow][iCol] = _m[iRow][iCol] +
                    rkMatrix._m[iRow][iCol];
            }
        }
        return kSum;
    }
    //-----------------------------------------------------------------------
    Matrix3 Matrix3::operator- (const Matrix3& rkMatrix) const
    {
        Matrix3 kDiff;
        for (size_t iRow = 0; iRow < 3; iRow++)
        {
            for (size_t iCol = 0; iCol < 3; iCol++)
            {
                kDiff._m[iRow][iCol] = _m[iRow][iCol] -
                    rkMatrix._m[iRow][iCol];
            }
        }
        return kDiff;
    }
    //-----------------------------------------------------------------------
    Matrix3 Matrix3::operator* (const Matrix3& rkMatrix) const
    {
        Matrix3 kProd;
        for (size_t iRow = 0; iRow < 3; iRow++)
        {
            for (size_t iCol = 0; iCol < 3; iCol++)
            {
                kProd._m[iRow][iCol] =
                    _m[iRow][0]*rkMatrix._m[0][iCol] +
                    _m[iRow][1]*rkMatrix._m[1][iCol] +
                    _m[iRow][2]*rkMatrix._m[2][iCol];
            }
        }
        return kProd;
    }
    
    //-----------------------------------------------------------------------
    Matrix3 Matrix3::operator- () const
    {
        Matrix3 kNeg;
        for (size_t iRow = 0; iRow < 3; iRow++)
        {
            for (size_t iCol = 0; iCol < 3; iCol++)
                kNeg[iRow][iCol] = -_m[iRow][iCol];
        }
        return kNeg;
    }
    //-----------------------------------------------------------------------
    Matrix3 Matrix3::operator* (float fScalar) const
    {
        Matrix3 kProd;
        for (size_t iRow = 0; iRow < 3; iRow++)
        {
            for (size_t iCol = 0; iCol < 3; iCol++)
                kProd[iRow][iCol] = fScalar*_m[iRow][iCol];
        }
        return kProd;
    }

	SketchyDynamics::Vector2 Matrix3::operator*( Vector2 vec ) const
	{
		Vector2 ret;
		float vx = vec.x;
		float vy = vec.y;
		
		float invz = 1.0f / (_m[2][0]*vx + _m[2][1]*vy + _m[2][2]);

		ret.x = (_m[0][0]*vx + _m[0][1]*vy + _m[0][2]) * invz;
		ret.y = (_m[1][0]*vx + _m[1][1]*vy + _m[1][2]) * invz;
		

		return ret;
	}

	//-----------------------------------------------------------------------
    Matrix3 operator* (float fScalar, const Matrix3& rkMatrix)
    {
        Matrix3 kProd;
        for (size_t iRow = 0; iRow < 3; iRow++)
        {
            for (size_t iCol = 0; iCol < 3; iCol++)
                kProd[iRow][iCol] = fScalar*rkMatrix._m[iRow][iCol];
        }
        return kProd;
    }
    //-----------------------------------------------------------------------
    Matrix3 Matrix3::Transpose () const
    {
        Matrix3 kTranspose;
        for (size_t iRow = 0; iRow < 3; iRow++)
        {
            for (size_t iCol = 0; iCol < 3; iCol++)
                kTranspose[iRow][iCol] = _m[iCol][iRow];
        }
        return kTranspose;
    }
    //-----------------------------------------------------------------------
    bool Matrix3::Inverse (Matrix3& rkInverse, float fTolerance) const
    {
        // Invert a 3x3 using cofactors.  This is about 8 times faster than
        // the Numerical Recipes code which uses Gaussian elimination.

        rkInverse[0][0] = _m[1][1]*_m[2][2] -
            _m[1][2]*_m[2][1];
        rkInverse[0][1] = _m[0][2]*_m[2][1] -
            _m[0][1]*_m[2][2];
        rkInverse[0][2] = _m[0][1]*_m[1][2] -
            _m[0][2]*_m[1][1];
        rkInverse[1][0] = _m[1][2]*_m[2][0] -
            _m[1][0]*_m[2][2];
        rkInverse[1][1] = _m[0][0]*_m[2][2] -
            _m[0][2]*_m[2][0];
        rkInverse[1][2] = _m[0][2]*_m[1][0] -
            _m[0][0]*_m[1][2];
        rkInverse[2][0] = _m[1][0]*_m[2][1] -
            _m[1][1]*_m[2][0];
        rkInverse[2][1] = _m[0][1]*_m[2][0] -
            _m[0][0]*_m[2][1];
        rkInverse[2][2] = _m[0][0]*_m[1][1] -
            _m[0][1]*_m[1][0];

        float fDet =
            _m[0][0]*rkInverse[0][0] +
            _m[0][1]*rkInverse[1][0]+
            _m[0][2]*rkInverse[2][0];

        if ( std::fabs(fDet) <= fTolerance )
            return false;

        float fInvDet = 1.0f/fDet;
        for (size_t iRow = 0; iRow < 3; iRow++)
        {
            for (size_t iCol = 0; iCol < 3; iCol++)
                rkInverse[iRow][iCol] *= fInvDet;
        }

        return true;
    }
    //-----------------------------------------------------------------------
    Matrix3 Matrix3::Inverse (float fTolerance) const
    {
        Matrix3 kInverse = Matrix3::ZERO;
        Inverse(kInverse,fTolerance);
        return kInverse;
    }
    //-----------------------------------------------------------------------
    float Matrix3::Determinant () const
    {
        float fCofactor00 = _m[1][1]*_m[2][2] -
            _m[1][2]*_m[2][1];
        float fCofactor10 = _m[1][2]*_m[2][0] -
            _m[1][0]*_m[2][2];
        float fCofactor20 = _m[1][0]*_m[2][1] -
            _m[1][1]*_m[2][0];

        float fDet =
            _m[0][0]*fCofactor00 +
            _m[0][1]*fCofactor10 +
            _m[0][2]*fCofactor20;

        return fDet;
    }

	float* Matrix3::operator[]( size_t iRow ) const
	{
		return (float*)_m[iRow];
	}

	Matrix3& Matrix3::operator=( const Matrix3& rkMatrix )
	{
		memcpy(_m,rkMatrix._m,9*sizeof(float));
		return *this;
	}

	bool Matrix3::operator!=( const Matrix3& rkMatrix ) const
	{
		return !operator==(rkMatrix);
	}

	void Matrix3::fromAngle( float angle )
	{
		float angCos = std::cos(angle);
		float angSin = std::sin(angle);

		_m[0][0] = angCos;
		_m[0][1] =-angSin;
		_m[0][2] = 0;
		_m[1][0] = angSin; 
		_m[1][1] = angCos;
		_m[1][2] = 0;
		_m[2][0] = 0;
		_m[2][1] = 0;
		_m[2][2] = 1;
	}

	void Matrix3::fromPosition( Vector2 pos )
	{
		_m[0][0] = 1;
		_m[0][1] = 0;
		_m[0][2] = pos.x;
		_m[1][0] = 0; 
		_m[1][1] = 1;
		_m[1][2] = pos.y;
		_m[2][0] = 0;
		_m[2][1] = 0;
		_m[2][2] = 1;
	}

	void Matrix3::fromScale( Vector2 scale )
	{
		_m[0][0] = scale.x;
		_m[0][1] = 0;
		_m[0][2] = 0;
		_m[1][0] = 0;
		_m[1][1] = scale.y;
		_m[1][2] = 0;
		_m[2][0] = 0;
		_m[2][1] = 0;
		_m[2][2] = 1;
	}

// 	void Matrix3::setAngle( float angle )
// 	{
// 		// Vector2 scale = getScale();
// 
// 	}
// 
// 	void Matrix3::setPosition( Vector2 pos )
// 	{
// 		_m[0][2] = pos.x;
// 		_m[1][2] = pos.y;
// 	}
// 
// 	float Matrix3::getAngle() const
// 	{
// 		return std::atan(-_m[0][1]/_m[0][0]);
// 	}
// 
// 	SketchyDynamics::Vector2 Matrix3::getPosition() const
// 	{
// 		return Vector2(_m[0][2], _m[1][2]);
// 	}
// 
// 	SketchyDynamics::Vector2 Matrix3::getScale() const
// 	{
// 		return Vector2(sqrt(_m[0][0]*_m[0][0] + _m[0][1]*_m[0][1]), sqrt(_m[1][0]*_m[1][0] + _m[1][1]*_m[1][1]));
// 	}

}
