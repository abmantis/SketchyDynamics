/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIDiamond.cxx
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

#include "CIDiamond.h"

/*----------------------------------------------------------------------------+
| Description: In this constructor we define all the features that are used 
|              to identify diamonds. The set of features are different for 
|              rotated and non ratated diamonds.
| Input: rotated - tells if the shapes are rotated or not.
+----------------------------------------------------------------------------*/
CIDiamond::CIDiamond (bool rotated) : CIShape (rotated)
{
    if (rotated)
        _features = new CIFeatures (&CIEvaluate::Alq_Ach, 0.78, 0.85, 1, 1, // separate from ellipses
                                    &CIEvaluate::Pch2_Ach, 14.5, 15.5, 21.5, 26, // Separate from bold lines
                                    &CIEvaluate::Alq_Aer, 0.52, 0.56, 0.72, 0.78, // Separate from rectangles
                                    &CIEvaluate::Alt_Alq, 0.5, 0.53, 0.62, 0.7,
                                    &CIEvaluate::Hollowness, 0, 0, 1, 1);
    else
        _features = new CIFeatures (&CIEvaluate::Alt_Abb, 0, 0, 0.4, 0.45);
}

CIDiamond::CIDiamond (CIScribble* sc, CIPoint a, CIPoint b, CIPoint c, CIPoint d, double dom, bool dash, bool bold)
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
| Description: Makes a clone of the current diamond.
+----------------------------------------------------------------------------*/
CIGesture* CIDiamond::clone()
{
    return new CIDiamond(_sc, _points[0], _points[1], _points[2], _points[3], _dom, _dashed, _bold);
}

/*----------------------------------------------------------------------------+
| Description: Computes the points of the recognized diamond
+----------------------------------------------------------------------------*/
void CIDiamond::setUp(CIScribble* sc)
{
    CIList<CIPoint> *points;
    
    _sc = sc;
    if (_rotated)
        points = sc->enclosingRect()->getPoints();
    else
        points = sc->boundingBox()->getPoints();

    _points[0] = (*points)[0];
    _points[1] = (*points)[1];
    _points[2] = (*points)[2];
    _points[3] = (*points)[3];

// This is not the correct code to compute the points of the diamond.
}

