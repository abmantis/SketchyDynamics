/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIFuzzyNode.cxx
+-----------------------------------------------------------------------------+
| Description: 
| 
| Notes:       
|
| Author: Manuel Joao Fonseca
|	  e-mail: mjf@inesc-id.pt
|
| Date: May 98
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

#include "CALI/CIFuzzyNode.h"

CIFuzzyNode::CIFuzzyNode (CIFuzzySet *fuzzy, double (CIEvaluate::* ptrF) (CIScribble*))
{
    _ptrFunc = ptrF;
    _fuzzySet = fuzzy;
}

CIFuzzyNode::~CIFuzzyNode ()
{
    delete _fuzzySet;
}

/*----------------------------------------------------------------------------+
| Description: Computes the degree of membership for the current scribble, 
|              using the fuzzyset and the function feature of the FuzzyNode.
| Input: a scribble
| Output: the value of the degree of membership
| Notes:
+----------------------------------------------------------------------------*/
double CIFuzzyNode::dom(CIScribble *sc)
{
    if (_ptrFunc) {
	CIEvaluate *ev = new CIEvaluate();
	double tmp = (ev->*_ptrFunc) (sc);

	delete ev;
	if (_fuzzySet)
            return _fuzzySet->degOfMember(tmp);
        else
            return 0;
    }
    else
	    return 0;
}
