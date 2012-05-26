/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIFeatures.h
+-----------------------------------------------------------------------------+
| Description: This class represents the features that will classify
|              a gesture (shape/command).
| 
| Notes: It works as a way to simplify the definition of features for each
|        gesture.
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
#ifndef _CIFEATURES_H
#define _CIFEATURES_H

#include "CIEvaluate.h"
#include "CIScribble.h"
#include "CIList.h"
#include "CIFuzzyNode.h"

class CIFeatures {
public:
    CIFeatures (double (CIEvaluate::* ptrF1) (CIScribble*), double awa, double a, double b, double bwb);

    CIFeatures (double (CIEvaluate::* ptrF1) (CIScribble*), double awa1, double a1, double b1, double bwb1,
                double (CIEvaluate::* ptrF2) (CIScribble*), double awa2, double a2, double b2, double bwb2);

    CIFeatures (double (CIEvaluate::* ptrF1) (CIScribble*), double awa1, double a1, double b1, double bwb1,
                double (CIEvaluate::* ptrF2) (CIScribble*), double awa2, double a2, double b2, double bwb2,
                double (CIEvaluate::* ptrF3) (CIScribble*), double awa3, double a3, double b3, double bwb3);

    CIFeatures (double (CIEvaluate::* ptrF1) (CIScribble*), double awa1, double a1, double b1, double bwb1,
                double (CIEvaluate::* ptrF2) (CIScribble*), double awa2, double a2, double b2, double bwb2,
                double (CIEvaluate::* ptrF3) (CIScribble*), double awa3, double a3, double b3, double bwb3,
                double (CIEvaluate::* ptrF4) (CIScribble*), double awa4, double a4, double b4, double bwb4);

    CIFeatures (double (CIEvaluate::* ptrF1) (CIScribble*), double awa1, double a1, double b1, double bwb1,
                double (CIEvaluate::* ptrF2) (CIScribble*), double awa2, double a2, double b2, double bwb2,
                double (CIEvaluate::* ptrF3) (CIScribble*), double awa3, double a3, double b3, double bwb3,
                double (CIEvaluate::* ptrF4) (CIScribble*), double awa4, double a4, double b4, double bwb4,
                double (CIEvaluate::* ptrF5) (CIScribble*), double awa5, double a5, double b5, double bwb5);
    CIFeatures (double (CIEvaluate::* ptrF1) (CIScribble*), double awa1, double a1, double b1, double bwb1,
                double (CIEvaluate::* ptrF2) (CIScribble*), double awa2, double a2, double b2, double bwb2,
                double (CIEvaluate::* ptrF3) (CIScribble*), double awa3, double a3, double b3, double bwb3,
                double (CIEvaluate::* ptrF4) (CIScribble*), double awa4, double a4, double b4, double bwb4,
                double (CIEvaluate::* ptrF5) (CIScribble*), double awa5, double a5, double b5, double bwb5,
                double (CIEvaluate::* ptrF6) (CIScribble*), double awa6, double a6, double b6, double bwb6);
    ~CIFeatures ();

    double evaluate(CIScribble* sc);

protected:
    CIList<CIFuzzyNode *> *_nodesList;
};
#endif //_CIFEATURES_H

