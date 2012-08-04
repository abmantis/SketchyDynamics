/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIEvaluate.cxx
+-----------------------------------------------------------------------------+
| Description: Definition of all function features used to classify gestures
| 
| Notes:       
|
| Author: Manuel Joao Fonseca
|	  e-mail: mjf@inesc-id.pt
|
| Date: May 98, April 99
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

#include "../dependecies/CALI/CIEvaluate.h"
#include "../dependecies/CALI/CIFunction.h"
#include <math.h>

// Special ratios
double CIEvaluate::Tl_Pch (CIScribble *sc)
{
    return sc->getLen() / sc->convexHull()->perimeter();
}

double CIEvaluate::Pch2_Ach (CIScribble *sc)
{
    return std::pow(sc->convexHull()->perimeter(),2) / sc->convexHull()->area();
}

double CIEvaluate::Pch_Ns_Tl (CIScribble *sc)
{
    return sc->convexHull()->perimeter()/(sc->getLen()/sc->getNumStrokes());
}

double CIEvaluate::Hollowness(CIScribble *sc)
{
    return (double)sc->ptsInSmallTri();
}

double CIEvaluate::Ns(CIScribble *sc)
{
    return sc->getNumStrokes();
}

double CIEvaluate::Hm_Wbb(CIScribble *sc)
{
    CIList<CIPoint> *pbb = sc->boundingBox()->getPoints();

    return fabs((double)((*pbb)[0].x - (*pbb)[1].x) / sc->hMovement());
}

double CIEvaluate::Vm_Hbb(CIScribble *sc)
{
    CIList<CIPoint> *pbb = sc->boundingBox()->getPoints();

    return fabs((double)((*pbb)[2].y - (*pbb)[1].y) / sc->vMovement());
}

double CIEvaluate::Hbb_Wbb(CIScribble *sc)
{
    CIList<CIPoint> *pbb = sc->boundingBox()->getPoints();

    double dw, dh;

    dw = (*pbb)[1].x - (*pbb)[0].x;
    dh = (*pbb)[2].y - (*pbb)[1].y;

    if (dw == 0 || dh == 0)
        return 0;

    double tmp = fabs((double)dh / dw);
    if (tmp > 1)
        tmp = 1 / tmp;
    return tmp;
}

double CIEvaluate::Diag_er(CIScribble *sc)
{
    CIList<CIPoint> *pbb = sc->enclosingRect()->getPoints();

    double dw, dh;

    dw = CIFunction::distance((*pbb)[2], (*pbb)[1]);
    dh = CIFunction::distance((*pbb)[1], (*pbb)[0]);

    return sqrt(dw*dw + dh*dh);
}

double CIEvaluate::Her_Wer(CIScribble *sc)
{
    CIList<CIPoint> *pbb = sc->enclosingRect()->getPoints();

    double dw, dh;

    dw = CIFunction::distance((*pbb)[2], (*pbb)[1]);
    dh = CIFunction::distance((*pbb)[1], (*pbb)[0]);

    if (dw == 0 || dh == 0)
        return 0;

    double tmp = dh / dw;
    if (tmp > 1)
        tmp = 1 / tmp;
    return tmp;
}

double CIEvaluate::pch_psc(CIScribble *sc)
{
    return sc->convexHull()->getNumPoints() / (double)sc->getNumPoints();
}

// Area ratios
double CIEvaluate::Alt_Ach (CIScribble *sc)
{
    return sc->largestTriangle()->area() / sc->convexHull()->area();
}

double CIEvaluate::Ach_Aer (CIScribble *sc)
{
    return sc->convexHull()->area() / sc->enclosingRect()->area();
}

double CIEvaluate::Alt_Aer (CIScribble *sc)
{
    return sc->largestTriangle()->area() / sc->enclosingRect()->area();
}

double CIEvaluate::Ach_Abb (CIScribble *sc)
{
    return sc->convexHull()->area() / sc->boundingBox()->area();
}

double CIEvaluate::Alq_Abb (CIScribble *sc)
{
    return sc->largestQuad()->area() / sc->boundingBox()->area();
}

double CIEvaluate::Aer_Abb (CIScribble *sc)
{
    return sc->enclosingRect()->area() / sc->boundingBox()->area();
}

double CIEvaluate::Alt_Abb (CIScribble *sc)
{
    return sc->largestTriangle()->area() / sc->boundingBox()->area();
}

double CIEvaluate::Alq_Ach (CIScribble *sc)
{
    return sc->largestQuad()->area() / sc->convexHull()->area();
}

double CIEvaluate::Alq_Aer (CIScribble *sc)
{
    return sc->largestQuad()->area() / sc->enclosingRect()->area();
}

double CIEvaluate::Alt_Alq (CIScribble *sc)
{
    return sc->largestTriangle()->area() / sc->largestQuad()->area();
}

// Perimeter ratios
double CIEvaluate::Plt_Pch (CIScribble *sc)
{
    return sc->largestTriangle()->perimeter() / sc->convexHull()->perimeter();
}

double CIEvaluate::Pch_Per (CIScribble *sc)
{
    return sc->convexHull()->perimeter()/sc->enclosingRect()->perimeter();
}

double CIEvaluate::Plt_Per (CIScribble *sc)
{
    return sc->largestTriangle()->perimeter()/sc->enclosingRect()->perimeter();
}

double CIEvaluate::Pch_Pbb (CIScribble *sc)
{
    return sc->convexHull()->perimeter() / sc->boundingBox()->perimeter();
}

double CIEvaluate::Plt_Pbb (CIScribble *sc)
{
    return sc->largestTriangle()->perimeter() / sc->boundingBox()->perimeter();
}

double CIEvaluate::Plq_Pch (CIScribble *sc)
{
    return sc->largestQuad()->perimeter() / sc->convexHull()->perimeter();
}

double CIEvaluate::Plq_Per (CIScribble *sc)
{
    return sc->largestQuad()->perimeter() / sc->enclosingRect()->perimeter();
}

double CIEvaluate::Plt_Plq (CIScribble *sc)
{
    return sc->largestTriangle()->perimeter() / sc->largestQuad()->perimeter();
}

double CIEvaluate::Plq_Pbb (CIScribble *sc)
{
    return sc->largestQuad()->perimeter() / sc->boundingBox()->perimeter();
}

double CIEvaluate::Per_Pbb (CIScribble *sc)
{
    return sc->enclosingRect()->perimeter() / sc->boundingBox()->perimeter();
}
