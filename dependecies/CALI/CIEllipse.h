/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIEllipse.h
+-----------------------------------------------------------------------------+
| Description: Represents an ellipse. (solid, dashed or bold)
| 
| Notes:       
|
| Author: Manuel Joao Fonseca
|	  e-mail: mjf@inesc-id.pt
|
| Date: April 98, May 99
+----------------------------------------------------------------------------
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
#ifndef _CIELLIPSE_H
#define _CIELLIPSE_H

#pragma warning( push )
#pragma warning( disable : 4100)

#include "CIShape.h"

class CIEllipse : public CIShape {

public:
    CIEllipse (bool rotated=true);
    CIEllipse (CIScribble* sc, double dom, bool dash = false, bool bold = false);
    ~CIEllipse();

    void setUp(CIScribble* sc);
    void draw(void *ptr) {}
    char *getName() { return ("Ellipse"); }
    CIGesture* clone();

    CIPolygon *_eliPoints;
    CIPoint _points[4];

private:
    CIPolygon* calcEllipse(CIPoint *points);
    CIPolygon* rotate(CIPoint *points, double theta, CIPoint p, double a, double b, int np);
};

#pragma warning( pop )
#endif
