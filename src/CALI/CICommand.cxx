/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CICommand.cxx
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

#include "CALI/CICommand.h"

/*----------------------------------------------------------------------------+
| Description: Computes the degree of membership for the scribble, taking
|              into account the fuzzysets for the current command.
|              This evaluation is made based on global features.
| Input: A scribble
| Output: degree of membership
| Notes: This method is the same for all commands.
+----------------------------------------------------------------------------*/
double CICommand::evalGlobalFeatures(CIScribble* sc)
{
        _dom = _features->evaluate(sc);
        if (_dom > 0)
            _sc = sc;
        else 
            _sc = NULL;
        return _dom;
}

/*----------------------------------------------------------------------------+
| Description: Recovers the previous atributes of the command
+----------------------------------------------------------------------------*/
void CICommand::popAttribs()
{
    if (_prevGesture) {
        _sc = _prevGesture->getScribble();
        _dom = _prevGesture->getDom();
        delete _prevGesture;
        _prevGesture = NULL;
    }
}

