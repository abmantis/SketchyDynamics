/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CITriangle.cxx
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

#include "CALI/CITriangle.h"

/*----------------------------------------------------------------------------+
| Description: In this constructor we define all the features that are used 
|              to identify triangles. The set of features are different for 
|              rotated and non ratated triangles.
| Input: rotated - tells if the shapes are ratated or not.
+----------------------------------------------------------------------------*/
CITriangle::CITriangle (bool rotated) : CIShape(rotated)
{
    _features = new CIFeatures (&CIEvaluate::Alt_Ach, 0.67, 0.77, 1, 1
                                ,&CIEvaluate::Plt_Pch, 0.95, 0.98, 1, 1
                                ,&CIEvaluate::Hollowness, 0, 0, 1, 1
								,&CIEvaluate::Pch_Per, 0.78, 0.8, 0.89, 0.945 // Not Lines Dashed
                               // ,&CIEvaluate::Alt_Alq, 0.81, 0.87, 1, 1       // Not Copy
                                ); 
}


CITriangle::CITriangle (CIScribble* sc, CIPoint a, CIPoint b, CIPoint c, double dom, bool dash, bool bold)
{ 
    _sc=sc;
    _points[0] = a; 
    _points[1] = b; 
    _points[2] = c; 
    _dashed = dash; 
    _bold = bold;
    _dom = dom;
    _features = NULL;
    _dashFeature = NULL;
}


/*----------------------------------------------------------------------------+
| Description: Computes the points of the recognized triangle
+----------------------------------------------------------------------------*/
void CITriangle::setUp(CIScribble* sc)
{
    CIList<CIPoint> *points;
    
    _sc = sc;
    points = sc->largestTriangle()->getPoints();
    _points[0] = (*points)[0];
    _points[1] = (*points)[1];
    _points[2] = (*points)[2];
}

/*----------------------------------------------------------------------------+
| Description: Makes a clone of the current triangle.
+----------------------------------------------------------------------------*/
CIGesture* CITriangle::clone()
{
    return new CITriangle(_sc, _points[0], _points[1], _points[2], _dom, _dashed);
}
