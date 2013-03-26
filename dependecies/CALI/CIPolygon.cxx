/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIPolygon
+-----------------------------------------------------------------------------+
| Description: Implementation of the class CIPolygon
| 
| Notes:       
|
| Author: Manuel Joao Fonseca
|	  e-mail: mjf@inesc-id.pt
|
| Date: May 98
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

#include "CIPolygon.h"
#include "CIFunction.h"
#include "myValues.h"
#include <math.h>

CIPolygon::CIPolygon ()
{
    _points = new CIList<CIPoint>(); 
    _area = 0;
    _perim = 0;
}

/*----------------------------------------------------------------------------+
| Description: Computes the area of the polygon, using a general algorithm.
| Output: the area
+----------------------------------------------------------------------------*/
double CIPolygon::area()
{
    if (_area == 0) {
	int numPoints = _points->getNumItems();
    if (numPoints < 3) {
        _area = ZERO;
        return _area;
    }

	for (int i = 0; i < numPoints - 1; i++) {
	    _area += (*_points)[i].x * (*_points)[i+1].y - (*_points)[i+1].x * (*_points)[i].y;
	}
	_area /= 2;
	if (_area == 0)
	    _area = ZERO;
    }
    return fabs(_area);
}

/*----------------------------------------------------------------------------+
| Description: Computes the perimeter of the polygon, using a general algorithm.
| Output: the perimeter
+----------------------------------------------------------------------------*/
double CIPolygon::perimeter()
{
    if (_perim == 0) {
	int numPoints = _points->getNumItems();

	for (int i = 0; i < numPoints - 1; i++) {
        _perim += CIFunction::distance((*_points)[i], (*_points)[i+1]);
	}
	if (_perim == 0)
	    _perim = ZERO;

        if (numPoints < 3)
            _perim *= 2;
   }
    return _perim;
}
