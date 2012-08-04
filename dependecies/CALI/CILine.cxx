/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CILine.cxx
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


#include "../dependecies/CALI/CILine.h"

/*----------------------------------------------------------------------------+
| Description: In this constructor we define all the features that are used 
|              to identify lines. The set of features are different for 
|              rotated and non ratated lines.
| Input: rotated - tells if the shapes are ratated or not.
+----------------------------------------------------------------------------*/
CILine::CILine (bool rotated) : CIShape(rotated)
{
    delete _normalFeature; // we delete these features because they are different
    delete _dashFeature;   // for lines

    _normalFeature = new CIFeatures (&CIEvaluate::Tl_Pch, 0.4, 0.45, BIG, BIG);
    _dashFeature = new CIFeatures (&CIEvaluate::Tl_Pch, 0, 0, 0.4, 0.45,
                                   &CIEvaluate::Pch_Ns_Tl, 5, 10, BIG, BIG);

    _features = new CIFeatures (&CIEvaluate::Her_Wer, 0, 0, 0.06, 0.08);
}

CILine::CILine (CIScribble* sc, CIPoint a, CIPoint b, double dom, bool dash, bool bold)
{
    _sc=sc;
    _points[0] = a; 
    _points[1] = b; 
    _dashed = dash; 
    _bold = bold;
    _open = false;
    _dom = dom;
    _features = NULL;
    _dashFeature = NULL;
}

/*----------------------------------------------------------------------------+
| Description: Computes the points of the recognized line
+----------------------------------------------------------------------------*/
void CILine::setUp(CIScribble* sc)
{
    CIList<CIPoint> *points;
    
    _sc = sc;
    points = sc->enclosingRect()->getPoints();
    _points[0] = (*points)[0];
    _points[1] = (*points)[2];
}

/*----------------------------------------------------------------------------+
| Description: Makes a clone of the current line.
+----------------------------------------------------------------------------*/
CIGesture* CILine::clone()
{
    return new CILine(_sc, _points[0], _points[1], _dom, _dashed, _bold);
}
