/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIPolygon.h
+-----------------------------------------------------------------------------+
| Description: Class that implements a polygon. It is built up of a set of 
|			   points. We can compute the area and perimeter of the polygon, 
|			   get the number of points or remove the last point added.
| 
| Notes:       
|
| Author: Manuel Joao Fonseca
|	  e-mail: mjf@inesc-id.pt
|
| Date: April 98
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
#ifndef _CIPOLYGON_H
#define _CIPOLYGON_H

#include "CIList.h"
#include "CIPoint.h"

class CIPolygon {
  public:
    CIPolygon ();
    ~CIPolygon ()			    { delete _points; }

// methods    
    double area ();
    double perimeter ();
    void   addPoint (CIPoint pt)	     { _points->insertTail(pt); }
    void   addPoint (double x, double y)	     { _points->insertTail(*new CIPoint(x,y)); }

    CIList<CIPoint> *getPoints () { return _points; }
    inline int getNumPoints()	// Number of points of the polygon
        { return _points->getNumItems(); }
    
    void   push (CIPoint pt)                 { _points->insertTail(pt); }
    CIPoint pop ()                            { return _points->pop(); }

  protected:
    double _area;
    double _perim;
    CIList<CIPoint> *_points;
};
#endif
