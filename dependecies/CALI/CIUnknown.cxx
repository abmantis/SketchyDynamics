/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIUnknown.cxx
+-----------------------------------------------------------------------------+
| Description: Implements class CIUnknown
| 
| Notes:       
|
| Author: Manuel Joao da Fonseca
|	      e-mail: mjf@inesc-id.pt
|
| Date: August 99
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

#include "CIUnknown.h"

CIUnknown::CIUnknown() : CIShape()
{ 
    _features = NULL; 
}

/*----------------------------------------------------------------------------+
| Description: Computes some features of the unknown gesture, like if it is 
|              solid, dashed or bold.
| Input: A scribble.
+----------------------------------------------------------------------------*/
void CIUnknown::setUp(CIScribble *sc)
{
    _sc = sc;
    _dashed = false;
    _bold = false;
    _open = false;
    _dom = 1;

    if (_dashFeature->evaluate(sc))
        _dashed = true;
    else if (_openFeature)
        if (_openFeature->evaluate(sc))
            _open = true;
        else
            if (_boldFeature->evaluate(sc))
                _bold = true;

}

