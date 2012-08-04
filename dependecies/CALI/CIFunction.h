/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIFunction.h
+-----------------------------------------------------------------------------+
| Description: Class with a set of static functions used by other classes.
| 
| Notes:       
|
| Author: Manuel Joao da Fonseca
|	      e-mail: mjf@inesc-id.pt
|
| Date: 
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
#ifndef _CIFUNCTION_H
#define _CIFUNCTION_H

#include "CIPoint.h"
#include "CIVector.h"

class CIFunction {
public:
    static double triangleArea(CIPoint& p1, CIPoint& p2, CIPoint& p3);
    static double quadArea(CIPoint& p1, CIPoint& p2, CIPoint& p3, CIPoint& p4);
    static double theta(const CIPoint& p, const CIPoint& q);
    static double distance(const CIPoint& p, const CIPoint& q);
    static bool left(CIPoint a, CIPoint b, CIPoint c);
    static double angle(const CIVector& a, const CIVector& b);
    static double angle(const CIPoint& a, const CIPoint& b);
    static CIPoint closest(const CIPoint& p1, const CIPoint& p2, const CIPoint& p3);
    static double cross(const CIVector& a, const CIVector& b);
    static double dot(const CIVector& a, const CIVector& b);

};
#endif

