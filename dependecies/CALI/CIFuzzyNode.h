/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIFuzzyNode.h
+-----------------------------------------------------------------------------+
| Description: This class represents a node which has a fuzzyset and the 
|              feature (a function) associated to it. It provides a method to 
|              compute the degree of membership.
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
#ifndef _CIFUZZYNODE_H
#define _CIFUZZYNODE_H

#include "CIScribble.h"
#include "CIFuzzySet.h"
#include "CIEvaluate.h"

class CIFuzzyNode {
public:
    CIFuzzyNode (CIFuzzySet *fuzzy, double (CIEvaluate::* ptrF) (CIScribble*));
    ~CIFuzzyNode ();

    double dom(CIScribble *sc);
    
protected:
    CIFuzzySet *_fuzzySet;
    double (CIEvaluate::* _ptrFunc) (CIScribble*);
};
#endif
