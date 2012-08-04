/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIRectangle.cxx
+-----------------------------------------------------------------------------+
| Description: 
| 
| Notes:       
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

#include "../dependecies/CALI/CIRectangle.h"

/*----------------------------------------------------------------------------+
| Description: In this constructor we define all the features that are used 
|              to identify rectangles. The set of features are different for 
|              rotated and non ratated rectangles.
| Input: rotated - tells if the shapes are rotated or not.
+----------------------------------------------------------------------------*/
CIRectangle::CIRectangle (bool rotated) : CIShape(rotated)
{
    if (rotated)
        _features = new CIFeatures (&CIEvaluate::Ach_Aer, 0.75, 0.85, 1, 1, // separate from diamonds
                                    &CIEvaluate::Alq_Aer, 0.72, 0.78, 1, 1,
                                    &CIEvaluate::Hollowness, 0, 0, 1, 1);
    else
        _features = new CIFeatures (&CIEvaluate::Ach_Abb, 0.8, 0.83, 1, 1,
                                    &CIEvaluate::Pch_Pbb, 0.87, 0.9, 1, 1,
                                    &CIEvaluate::Alt_Abb, 0.45, 0.47, 0.5, 0.52//,
                                    //&CIEvaluate::scLen_Pch, 0, 0, 1.5, 1.7
                                    );
}

CIRectangle::CIRectangle (CIScribble* sc, CIPoint a, CIPoint b, CIPoint c, CIPoint d, double dom, bool dash, bool bold)
{ 
    _sc=sc;
    _points[0] = a; 
    _points[1] = b; 
    _points[2] = c; 
    _points[3] = d;
    _dashed = dash; 
    _bold = bold;
    _open = false;
    _dom = dom;
    _features = NULL;
    _dashFeature = NULL;
}

/*----------------------------------------------------------------------------+
| Description: Makes a clone of the current rectangle.
+----------------------------------------------------------------------------*/
CIGesture* CIRectangle::clone()
{
    return new CIRectangle(_sc, _points[0], _points[1], _points[2], _points[3], _dom, _dashed, _bold);
}

/*----------------------------------------------------------------------------+
| Description: Computes the points of the recognized rectangle
+----------------------------------------------------------------------------*/
void CIRectangle::setUp(CIScribble* sc)
{
    CIList<CIPoint> *points;
    
    _sc = sc;
    points = sc->enclosingRect()->getPoints();
    _points[0] = (*points)[0];
    _points[1] = (*points)[1];
    _points[2] = (*points)[2];
    _points[3] = (*points)[3];
}
