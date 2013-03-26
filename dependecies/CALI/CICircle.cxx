/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CICircle.cxx
+-----------------------------------------------------------------------------+
| Description: 
| 
| Notes:       
|
| $Log$
|
| Author: Manuel Joao Fonseca
|	  e-mail: mjf@inesc-id.pt
|
| Date: May 98, May 99
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


#include "CICircle.h"

/*----------------------------------------------------------------------------+
| Description: In this constructor we define all the features that are used 
|              to identify circles. 
| Input: rotated - tells if the shapes are rotated or not.
+----------------------------------------------------------------------------*/
CICircle::CICircle (bool rotated) : CIShape(rotated)
{
    _features = new CIFeatures (&CIEvaluate::Pch2_Ach, 12.5, 12.5, 13.2, 13.5,
                                &CIEvaluate::Hollowness, 0, 0, 0, 0);
}


CICircle::CICircle (CIScribble* sc, double dom, bool dash, bool bold)
{ 
    _sc=sc;
    if(sc)
        setUp(sc);
    _dashed = dash; 
    _bold = bold;
    _open = false;
    _dom = dom;
    _features = NULL;
    _dashFeature = NULL;
}

/*----------------------------------------------------------------------------+
| Description: Computes the center and the radius of the recognized circle
+----------------------------------------------------------------------------*/
void CICircle::setUp(CIScribble* sc)
{
    CIList<CIPoint> *points;
    double d1, d2;
    
    _sc = sc;
    points = sc->boundingBox()->getPoints();
    _points[0] = (*points)[0];
    _points[1] = (*points)[1];
    _points[2] = (*points)[2];
    _points[3] = (*points)[3];
    d1 = sqrt(std::pow(_points[0].x-_points[1].x,2) + std::pow(_points[0].y-_points[1].y,2));
    d2 = sqrt(std::pow(_points[2].x-_points[1].x,2) + std::pow(_points[2].y-_points[1].y,2));
    _radius = (d1+d2)/2/2;
    _center.x = _points[0].x + d2/2;
    _center.y = _points[0].y + d1/2;
}

/*----------------------------------------------------------------------------+
| Description: Makes a clone of the current circle.
+----------------------------------------------------------------------------*/
CIGesture* CICircle::clone()
{
    return new CICircle(_sc, _dom, _dashed, _bold);
}
