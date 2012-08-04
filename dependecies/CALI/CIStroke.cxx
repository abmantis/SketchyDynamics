/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIStroke.cxx
+-----------------------------------------------------------------------------+
| Description: Implementation of the CIStroke class.
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

#include "../dependecies/CALI/CIList.h"
#include "../dependecies/CALI/CIPoint.h"
#include "../dependecies/CALI/CIStroke.h"
#include "../dependecies/CALI/myValues.h"

CIStroke::CIStroke ()
{
    _points = new CIList<CIPoint *> ();
    _len = 0;
    _speed = 0;
    _firstTime = 0;
}

CIStroke::~CIStroke ()
{
    int t = _points->getNumItems();
    for (int i=0; i< t; i++) 
	delete (*_points)[i];

    delete _points;
}

/*----------------------------------------------------------------------------+
| Description: Adds a point to the stroke. Inserts the point at the end of 
|              the list of points, computes the new length of the stroke and 
|              the drawing speed.
|
| Input: Point coordenates (x,y) and the temporal mark in milliseconds.
+----------------------------------------------------------------------------*/
void CIStroke::addPoint (double x, double y, double time)
{
    _points->push(new CIPoint(x,y,time));
    if (getNumPoints() > 1) {
        _len += sqrt(std::pow(_lastPoint.x - x,2) + std::pow(_lastPoint.y - y,2));
        if (time == _firstTime)
            _speed = BIG;
        else
	    _speed = (double) _len / (time - _firstTime);
    } 
    else
	_firstTime = time;

    _lastPoint.x = x;
    _lastPoint.y = y;
}
