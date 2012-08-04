/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIFeatures.cxx
+-----------------------------------------------------------------------------+
| Description: 
| 
| Notes:       
|
| Author: Manuel Joao Fonseca
|	  e-mail: mjf@inesc-id.pt
|
| Date: April 99
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

#include "../dependecies/CALI/CIFeatures.h"

CIFeatures::CIFeatures (double (CIEvaluate::* ptrF) (CIScribble*), double awa, double a, double b, double bwb)
{
    _nodesList = new CIList<CIFuzzyNode *> ();

    _nodesList->insertTail(new CIFuzzyNode(new CIFuzzySet(a, b, a-awa, bwb-b), ptrF));
}

CIFeatures::CIFeatures (double (CIEvaluate::* ptrF1) (CIScribble*), double awa1, double a1, double b1, double bwb1,
            double (CIEvaluate::* ptrF2) (CIScribble*), double awa2, double a2, double b2, double bwb2)
{
    _nodesList = new CIList<CIFuzzyNode *> ();

    _nodesList->insertTail(new CIFuzzyNode(new CIFuzzySet(a1, b1, a1-awa1, bwb1-b1), ptrF1));
    _nodesList->insertTail(new CIFuzzyNode(new CIFuzzySet(a2, b2, a2-awa2, bwb2-b2), ptrF2));
}

CIFeatures::CIFeatures (double (CIEvaluate::* ptrF1) (CIScribble*), double awa1, double a1, double b1, double bwb1,
            double (CIEvaluate::* ptrF2) (CIScribble*), double awa2, double a2, double b2, double bwb2,
            double (CIEvaluate::* ptrF3) (CIScribble*), double awa3, double a3, double b3, double bwb3)
{
    _nodesList = new CIList<CIFuzzyNode *> ();

    _nodesList->insertTail(new CIFuzzyNode(new CIFuzzySet(a1, b1, a1-awa1, bwb1-b1), ptrF1));
    _nodesList->insertTail(new CIFuzzyNode(new CIFuzzySet(a2, b2, a2-awa2, bwb2-b2), ptrF2));
    _nodesList->insertTail(new CIFuzzyNode(new CIFuzzySet(a3, b3, a3-awa3, bwb3-b3), ptrF3));
}

CIFeatures::CIFeatures (double (CIEvaluate::* ptrF1) (CIScribble*), double awa1, double a1, double b1, double bwb1,
            double (CIEvaluate::* ptrF2) (CIScribble*), double awa2, double a2, double b2, double bwb2,
            double (CIEvaluate::* ptrF3) (CIScribble*), double awa3, double a3, double b3, double bwb3,
            double (CIEvaluate::* ptrF4) (CIScribble*), double awa4, double a4, double b4, double bwb4)
{
    _nodesList = new CIList<CIFuzzyNode *> ();

    _nodesList->insertTail(new CIFuzzyNode(new CIFuzzySet(a1, b1, a1-awa1, bwb1-b1), ptrF1));
    _nodesList->insertTail(new CIFuzzyNode(new CIFuzzySet(a2, b2, a2-awa2, bwb2-b2), ptrF2));
    _nodesList->insertTail(new CIFuzzyNode(new CIFuzzySet(a3, b3, a3-awa3, bwb3-b3), ptrF3));
    _nodesList->insertTail(new CIFuzzyNode(new CIFuzzySet(a4, b4, a4-awa4, bwb4-b4), ptrF4));
}

CIFeatures::CIFeatures (double (CIEvaluate::* ptrF1) (CIScribble*), double awa1, double a1, double b1, double bwb1,
            double (CIEvaluate::* ptrF2) (CIScribble*), double awa2, double a2, double b2, double bwb2,
            double (CIEvaluate::* ptrF3) (CIScribble*), double awa3, double a3, double b3, double bwb3,
            double (CIEvaluate::* ptrF4) (CIScribble*), double awa4, double a4, double b4, double bwb4,
            double (CIEvaluate::* ptrF5) (CIScribble*), double awa5, double a5, double b5, double bwb5)
{
    _nodesList = new CIList<CIFuzzyNode *> ();

    _nodesList->insertTail(new CIFuzzyNode(new CIFuzzySet(a1, b1, a1-awa1, bwb1-b1), ptrF1));
    _nodesList->insertTail(new CIFuzzyNode(new CIFuzzySet(a2, b2, a2-awa2, bwb2-b2), ptrF2));
    _nodesList->insertTail(new CIFuzzyNode(new CIFuzzySet(a3, b3, a3-awa3, bwb3-b3), ptrF3));
    _nodesList->insertTail(new CIFuzzyNode(new CIFuzzySet(a4, b4, a4-awa4, bwb4-b4), ptrF4));
    _nodesList->insertTail(new CIFuzzyNode(new CIFuzzySet(a5, b5, a5-awa5, bwb5-b5), ptrF5));
}

CIFeatures::CIFeatures (double (CIEvaluate::* ptrF1) (CIScribble*), double awa1, double a1, double b1, double bwb1,
            double (CIEvaluate::* ptrF2) (CIScribble*), double awa2, double a2, double b2, double bwb2,
            double (CIEvaluate::* ptrF3) (CIScribble*), double awa3, double a3, double b3, double bwb3,
            double (CIEvaluate::* ptrF4) (CIScribble*), double awa4, double a4, double b4, double bwb4,
            double (CIEvaluate::* ptrF5) (CIScribble*), double awa5, double a5, double b5, double bwb5,
            double (CIEvaluate::* ptrF6) (CIScribble*), double awa6, double a6, double b6, double bwb6)
{
    _nodesList = new CIList<CIFuzzyNode *> ();

    _nodesList->insertTail(new CIFuzzyNode(new CIFuzzySet(a1, b1, a1-awa1, bwb1-b1), ptrF1));
    _nodesList->insertTail(new CIFuzzyNode(new CIFuzzySet(a2, b2, a2-awa2, bwb2-b2), ptrF2));
    _nodesList->insertTail(new CIFuzzyNode(new CIFuzzySet(a3, b3, a3-awa3, bwb3-b3), ptrF3));
    _nodesList->insertTail(new CIFuzzyNode(new CIFuzzySet(a4, b4, a4-awa4, bwb4-b4), ptrF4));
    _nodesList->insertTail(new CIFuzzyNode(new CIFuzzySet(a5, b5, a5-awa5, bwb5-b5), ptrF5));
    _nodesList->insertTail(new CIFuzzyNode(new CIFuzzySet(a6, b6, a6-awa6, bwb6-b6), ptrF6));
}

CIFeatures::~CIFeatures ()
{
    if (_nodesList) {
	int t = _nodesList->getNumItems();
	for (int i=0; i< t; i++) 
	    delete (*_nodesList)[i];
	delete _nodesList;
    }
}

double CIFeatures::evaluate(CIScribble* sc)
{
    if (_nodesList) {
        double tmp, dom = 1;
        int nn = _nodesList->getNumItems();

        for (int i=0; i<nn; i++)
        {
            tmp = (*_nodesList)[i]->dom(sc);
            if (tmp < dom)
                dom = tmp;
            if (dom == 0)
                break;
        }
        return dom;
    }
    else
        return 0;
}
