/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIEllipse.cxx
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

#include "CALI/CIEllipse.h"
#include "CALI/CIFunction.h"

/*----------------------------------------------------------------------------+
| Description: In this constructor we define all the features that are used 
|              to identify ellipses. The set of features are different for 
|              rotated and non ratated ellipses.
| Input: rotated - tells if the shapes are rotated or not.
+----------------------------------------------------------------------------*/
CIEllipse::CIEllipse (bool rotated) : CIShape(rotated)
{
    _eliPoints = NULL;
    if (rotated)
        _features = new CIFeatures (&CIEvaluate::Pch2_Ach, 13.2, 13.5, 19, 30, // separate from bold circles
                                    &CIEvaluate::Alq_Ach, 0.6, 0.65, 0.71, 0.78,
                                    &CIEvaluate::Hollowness, 0, 0, 0, 0);
    else
        _features = new CIFeatures (&CIEvaluate::Alt_Abb, 0, 0, 0.4, 0.45,
                                    &CIEvaluate::Alt_Ach, 0.4, 0.43, 0.47, 0.52,
                                    &CIEvaluate::Ach_Abb, 0.6, 0.7, 0.8, 0.9//,
                                    //&CIEvaluate::scLen_Pch, 0, 0, 1.5, 1.7
                                    );
}

CIEllipse::CIEllipse (CIScribble* sc, double dom, bool dash, bool bold)
{ 
    _eliPoints = NULL;
    _sc=sc;
    if(sc)
        setUp(sc);
    _dashed = dash; 
    _bold = bold;
    _open = false;
    _dom = dom;
    _features = NULL;
    _dashFeature = NULL;
}

/*----------------------------------------------------------------------------+
| Description: Deletes elipses points.
+----------------------------------------------------------------------------*/
CIEllipse::~CIEllipse () 
{
    if(_eliPoints != NULL)
	delete _eliPoints;
}

/*----------------------------------------------------------------------------+
| Description: Makes a clone of the current ellipse.
+----------------------------------------------------------------------------*/
CIGesture* CIEllipse::clone()
{
    return new CIEllipse(_sc, _dom, _dashed, _bold);
}

/*----------------------------------------------------------------------------+
| Description: Computes the points of the recognized ellipse
+----------------------------------------------------------------------------*/
void CIEllipse::setUp(CIScribble* sc)
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
    _eliPoints = calcEllipse(_points);
}

// ----- Private Members -----
CIPolygon *CIEllipse::calcEllipse(CIPoint *points)
{
    double a, b, d1, d2;
    double theta;
    int np=0;
    CIPoint pts[1000];
    CIPoint pt;

    pt.x = points[1].x;
    pt.y = points[1].y;
    theta = CIFunction::angle(points[0], points[3]);

    a = sqrt(std::pow(points[2].x - points[1].x,2) + std::pow(points[2].y - points[1].y,2)) / 2;
    b = sqrt(std::pow(points[0].x - points[1].x,2) + std::pow(points[0].y - points[1].y,2)) / 2;

    int x=0;
    int y=(int)b;

    d1 = std::pow(b,2)-std::pow(a,2)*b+std::pow(a,2)/4;
    pts[np].x = x;
    pts[np].y = y;
    np++;

    while (std::pow(a,2)*(y-1/2) > std::pow(b,2)*(x+1)) {
        if(d1<0) {
	    d1+=std::pow(b,2)*(2*x+3);
	    x++;
        }
	else {
    	    d1+=std::pow(b,2)*(2*x+3)+std::pow(a,2)*(-2*y+2);
	    x++;
	    y--;
        }
        pts[np].x = x;
        pts[np].y = y;
        np++;
    }

    d2=std::pow(b,2)*std::pow((double)x+1/2,2)+std::pow((double)a,2)*std::pow((double)y-1,2)-std::pow((double)a,2)*std::pow((double)b,2);
    while(y>=-1) {
    	if(d2<0) {
	    d2+=std::pow(b,2)*(2*x+2)+std::pow(a,2)*(-2*y+3);
	    x++;
	    y--;
        }
	else {
	    d2+=std::pow(a,2)*(-2*y+3);
	    y--;
        }
        pts[np].x = x;
        pts[np].y = y;
        np++;
    }

    np--;

    _eliPoints = rotate(pts, theta, pt, a, b, np);
    return _eliPoints;
}

CIPolygon *CIEllipse::rotate(CIPoint *points, double theta, CIPoint p, double a, double b, int np)
{
    CIPolygon *pol = new CIPolygon();
    CIPoint *_ellipsePoints = new CIPoint[4*np];

    for (int i=0; i<np; i++) {
        _ellipsePoints[i].x =cos(theta)*(-points[np-i-1].x+a)-sin(theta)*(points[np-i-1].y+b)+p.x;
        _ellipsePoints[i].y =sin(theta)*(-points[np-i-1].x+a)+cos(theta)*(points[np-i-1].y+b)+p.y;

        _ellipsePoints[np+i].x =cos(theta)*(points[i].x+a)-sin(theta)*(points[i].y+b)+p.x;
        _ellipsePoints[np+i].y =sin(theta)*(points[i].x+a)+cos(theta)*(points[i].y+b)+p.y;  

        _ellipsePoints[2*np+i].x =cos(theta)*(points[np-i-1].x+a)-sin(theta)*(-points[np-i-1].y+b)+p.x;
        _ellipsePoints[2*np+i].y =sin(theta)*(points[np-i-1].x+a)+cos(theta)*(-points[np-i-1].y+b)+p.y;
  
        _ellipsePoints[3*np+i].x =cos(theta)*(-points[i].x+a)-sin(theta)*(-points[i].y+b)+p.x;
        _ellipsePoints[3*np+i].y =sin(theta)*(-points[i].x+a)+cos(theta)*(-points[i].y+b)+p.y;
    }

    for (int j=0; j<4*np; j++) 
        pol->addPoint(_ellipsePoints[j]);
    delete []_ellipsePoints;

    return pol;
}
