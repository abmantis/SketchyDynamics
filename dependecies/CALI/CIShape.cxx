/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIShape.cxx
+-----------------------------------------------------------------------------+
| Description: 
| 
| Notes:       
|
| Author: Manuel Joao Fonseca
|	  e-mail: mjf@inesc-id.pt
|
| Date: May 99
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

#include "../dependecies/CALI/CIShape.h"

CIShape::CIShape (bool rotated)
{
    _sc = NULL; 
    _dashed = false;
    _bold = false;
    _open = false;
    _dom = 0;
    _rotated = rotated;
    _normalFeature = new CIFeatures (&CIEvaluate::Tl_Pch, 0.83, 0.93, BIG, BIG);
    _dashFeature = new CIFeatures (&CIEvaluate::Tl_Pch, 0.2, 0.3, 0.83, 0.93,
                                   &CIEvaluate::Pch_Ns_Tl, 5, 10, BIG, BIG);
    _openFeature = new CIFeatures (&CIEvaluate::Tl_Pch, 0.2, 0.3, 0.83, 0.93);
    _boldFeature =  new CIFeatures (&CIEvaluate::Tl_Pch, 1.5, 3, BIG, BIG);
}


CIShape::~CIShape ()
{
    if (_normalFeature) delete _normalFeature;
    if (_dashFeature) delete _dashFeature;
    if (_openFeature) delete _openFeature;
    if (_boldFeature) delete _boldFeature;
}

/*----------------------------------------------------------------------------+
| Description: Computes the degree of membership for the scribble, taking
|              into account the fuzzysets for the current shape.
|              This evaluation is made based on geometric global features.
| Input: A scribble
| Output: degree of membership
| Notes: This method is the same for all shapes.
+----------------------------------------------------------------------------*/
double CIShape::evalGlobalFeatures(CIScribble* sc)
{
    _dom = _features->evaluate(sc);
    _dashed = false;
    _bold = false;
    _open = false;
    _sc = NULL;
    if (_dom) {
        setUp(sc);
        if (_normalFeature->evaluate(sc)) {
            if (_boldFeature->evaluate(sc))
                _bold = true;
        }
        else {
            _dom *= _dashFeature->evaluate(sc);
            if (_dom)
                _dashed = true;
        }
    }
    return _dom;
}

/*----------------------------------------------------------------------------+
| Description: Recover the previous attributes of the shape
+----------------------------------------------------------------------------*/
void CIShape::popAttribs()
{
    CIShape* shp;
    CIScribble* sc;

    if (_prevGesture) {
        shp = (CIShape *)_prevGesture;
        sc = shp->getScribble();
        if (sc)
            setUp(shp->getScribble());
        _dashed = shp->isDashed();
        _open = shp->isOpen();
        _bold = shp->isBold();
        _dom = shp->getDom();
        delete shp;
        _prevGesture = NULL;
    }
}
