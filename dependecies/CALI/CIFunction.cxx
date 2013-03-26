/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIFunction.cxx
+-----------------------------------------------------------------------------+
| Description: This class has a set of static methods to compute some geometric
|              functions.
| 
| Notes:       
|
| Author: Manuel Joao Fonseca
|	  e-mail: mjf@inesc-id.pt
|
| Date: April 99
+-----------------------------------------------------------------------------+
|
| Copyright (C) 1998, 1999, 2000 Manuel João da Fonseca
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of the GNU General Public License
| as published by the Free Software Foundation; either version 2
| of the License, or any later version.
| 
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
| GNU General Public License for more details.
| 
| You should have received a copy of the GNU General Public License
| along with this program; if not, write to the Free Software
| Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
| 
+----------------------------------------------------------------------------*/

#include "CIFunction.h"
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <stdio.h>

/*----------------------------------------------------------------------------+
| Description: Computes the area of a triangle
+----------------------------------------------------------------------------*/
double CIFunction::triangleArea(CIPoint& p1, CIPoint& p2, CIPoint& p3)
{
    double area = p1.x * p2.y - p2.x * p1.y;
    area += p2.x * p3.y - p3.x * p2.y;
    area += p3.x * p1.y - p1.x * p3.y;

    return fabs(area/2);
}

/*----------------------------------------------------------------------------+
| Description: Computes the area of a rectangle
+----------------------------------------------------------------------------*/
double CIFunction::quadArea(CIPoint& p1, CIPoint& p2, CIPoint& p3, CIPoint& p4)
{
    double area = p1.x * p2.y - p2.x * p1.y;
    area += p2.x * p3.y - p3.x * p2.y;
    area += p3.x * p4.y - p4.x * p3.y;
    area += p4.x * p1.y - p1.x * p4.y;

    return fabs(area/2);
}

/*----------------------------------------------------------------------------+
| Description: Computes the "angle" made by the line defined by the 2 points 
+----------------------------------------------------------------------------*/
double CIFunction::theta(const CIPoint& p, const CIPoint& q)
{
    double dx = q.x - p.x, ax = fabs(dx),
    dy = q.y - p.y, ay = fabs(dy);

    double t = (ax + ay == 0) ? 0 : (double) dy / (ax + ay);

    if (dx < 0) 
        t = 2 - t;
    else if (dy < 0) 
        t = 4 + t;

    return t*90;
};

/*----------------------------------------------------------------------------+
| Description: Computes the distance between two points
+----------------------------------------------------------------------------*/
double CIFunction::distance(const CIPoint& p, const CIPoint& q)
{
    return sqrt(std::pow(q.x-p.x,2) + std::pow(q.y-p.y,2));
}

bool CIFunction::left(CIPoint a, CIPoint b, CIPoint c)
{
    return (a.x * b.y - a.y * b.x + a.y * c.x - a.x * c.y + b.x * c.y - c.x * b.y) > 0;
}

double CIFunction::angle(const CIVector& a, const CIVector& b)
{
    return atan2(CIFunction::cross(a, b), CIFunction::dot(a, b));
}

double CIFunction::angle(const CIPoint& a, const CIPoint& b)
{
    return atan2(b.y - a.y, b.x - a.x);
}

/*---------------------------------------------------------------------------+
| closest: return point on line (p1, p2) which is closer to p3
+---------------------------------------------------------------------------*/
CIPoint CIFunction::closest(const CIPoint& p1, const CIPoint& p2, const CIPoint& p3)
{
    double d;
	
    if (fabs(p2.x - p1.x) < 1e-10) 
	return CIPoint(p1.x, p3.y);

    if ((p1 == p3) || (p2 == p3))
        return p3;

    if (fabs(p2.y - p1.y) < 1e-10)
         return CIPoint(p3.x, p1.y);

    d = p2.x - p1.x;
    double m = (p2.y - p1.y) / d;

    double b1, b2, x, y;
    b1 = p2.y - m * p2.x;
    b2 = p3.y + 1/m * p3.x;
    x = (b2 - b1) / (m + 1/m);
    y = m * x + b1;

    return CIPoint(x,y);
}

double CIFunction::cross(const CIVector& a, const CIVector& b) {
    double dx1 = a.endp.x - a.startp.x, dx2 = b.endp.x - b.startp.x,
    dy1 = a.endp.y - a.startp.y, dy2 = b.endp.y - b.startp.y;
    return dx1 * dy2 - dy1 * dx2;
}

double CIFunction::dot(const CIVector& a, const CIVector& b) {
    double dx1 = a.endp.x - a.startp.x, dx2 = b.endp.x - b.startp.x,
    dy1 = a.endp.y - a.startp.y, dy2 = b.endp.y - b.startp.y;
    return dx1 * dx2 + dy1 * dy2;
}

