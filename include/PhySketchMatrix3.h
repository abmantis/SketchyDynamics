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
#ifndef __Matrix3_H__
#define __Matrix3_H__

#include "PhySketchDefinitions.h"
#include "PhySketchVector2.h"

namespace PhySketch
{
    /// <summary> A 3x3 Matrix for use with 2D transformations. </summary>
    /// <remarks> This is used for 2D transformations, not 3D. </remarks>
    class  Matrix3
    {
    public:

		/// <summary> Default constructor. </summary>
		/// <remarks> It does <b>NOT</b> initialize the matrix for efficiency. </remarks>
		inline Matrix3 () {}
        inline explicit Matrix3 (const float arr[3][3])
		{
			memcpy(_m,arr,9*sizeof(float));
		}
        inline Matrix3 (const Matrix3& rkMatrix)
		{
			memcpy(_m,rkMatrix._m,9*sizeof(float));
		}
        Matrix3 (float fEntry00, float fEntry01, float fEntry02,
                    float fEntry10, float fEntry11, float fEntry12,
                    float fEntry20, float fEntry21, float fEntry22)
		{
			_m[0][0] = fEntry00;
			_m[0][1] = fEntry01;
			_m[0][2] = fEntry02;
			_m[1][0] = fEntry10;
			_m[1][1] = fEntry11;
			_m[1][2] = fEntry12;
			_m[2][0] = fEntry20;
			_m[2][1] = fEntry21;
			_m[2][2] = fEntry22;
		}

		/// <summary> Exchange the contents of this matrix with another. </summary>
		/// <param name="other"> The other matrix. </param>
		inline void swap(Matrix3& other)
		{
			std::swap(_m[0][0], other._m[0][0]);
			std::swap(_m[0][1], other._m[0][1]);
			std::swap(_m[0][2], other._m[0][2]);
			std::swap(_m[1][0], other._m[1][0]);
			std::swap(_m[1][1], other._m[1][1]);
			std::swap(_m[1][2], other._m[1][2]);
			std::swap(_m[2][0], other._m[2][0]);
			std::swap(_m[2][1], other._m[2][1]);
			std::swap(_m[2][2], other._m[2][2]);
		}

		/// <summary>  member access, allows use of construct mat[r][c]. </summary>
        /// <param name="iRow"> Zero-based index of the row. </param>
        /// <returns> The indexed value. </returns>
        float* operator[] (size_t iRow) const;
    
		/// <summary> Assignment operator. </summary>
        Matrix3& operator= (const Matrix3& rkMatrix);

        /// <summary> Equality operator. </summary>
        bool operator== (const Matrix3& rkMatrix) const;

        /// <summary> Inequality operator. </summary>
        bool operator!= (const Matrix3& rkMatrix) const;

        // arithmetic operations
        Matrix3 operator+ (const Matrix3& rkMatrix) const;
        Matrix3 operator- (const Matrix3& rkMatrix) const;
        Matrix3 operator* (const Matrix3& rkMatrix) const;
        Matrix3 operator- () const;
		        
        // matrix * scalar
        Matrix3 operator* (float fScalar) const;

        // scalar * matrix
        friend Matrix3 operator* (float fScalar, const Matrix3& rkMatrix);

		// matrix * vector2 -> transform point
		Vector2 operator* (Vector2 vec) const;

        // utilities
        Matrix3 Transpose () const;
        bool Inverse (Matrix3& rkInverse, float fTolerance = 1e-06) const;
        Matrix3 Inverse (float fTolerance = 1e-06) const;
        float Determinant () const;

		/// <summary> Creates a 2D rotation matrix based on the given angle. </summary>
		/// <param name="angle"> The angle in radians. </param>
		void fromAngle(float angle);
		void fromPosition(Vector2 pos);
		void fromScale(Vector2 scale);

// 		void setAngle(float angle);
// 		void setPosition(Vector2 pos);
// 		void setScale(Vector2 scale);
// 
// 		float getAngle() const;
// 		Vector2 getPosition() const;
// 		Vector2 getScale() const;

		/** Function for writing to a stream.
		*/
		inline friend std::ostream& operator <<
			( std::ostream& o, const Matrix3& mat )
		{
			o << "Matrix3(" << mat[0][0] << ", " << mat[0][1] << ", " << mat[0][2] << ", " 
                            << mat[1][0] << ", " << mat[1][1] << ", " << mat[1][2] << ", " 
                            << mat[2][0] << ", " << mat[2][1] << ", " << mat[2][2] << ")";
			return o;
		}

        static const float EPSILON;
        static const Matrix3 ZERO;
        static const Matrix3 IDENTITY;

    protected:
		float _m[3][3];

    };
	
}
#endif
