/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIScribble.cxx
+-----------------------------------------------------------------------------+
| Description: Implements the methods of CIScribble.
| 
| Notes:       
|
| Author: Manuel Joao Fonseca
|	  e-mail: mjf@inesc-id.pt
|
| Date: April 98, July 99
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

#include "../dependecies/CALI/CIScribble.h"
#include "../dependecies/CALI/CIFunction.h"
#include "../dependecies/CALI/myValues.h"

#ifndef _USE_MATH_DEFINES
	#define _USE_MATH_DEFINES
	#include <math.h>
	#undef _USE_MATH_DEFINES
#else
	#include <math.h>
#endif // _USE_MATH_DEFINES


#include <stdlib.h>
#include <iostream>
//using namespace std;

CIScribble::CIScribble () 
{
    _len = 0;
    _totalSpeed = 0;
    _strokes = new CIList<CIStroke *> ();

    _boundingBox = NULL;
    _convexHull = NULL;
    _largestTriangleOld = NULL;
    _largestTriangle = NULL;
    _largestQuadOld = NULL;
    _largestQuad = NULL;
    _enclosingRect = NULL;
}

CIScribble::~CIScribble () 
{
    if (_boundingBox) delete _boundingBox;
    if (_convexHull) delete _convexHull;
    if (_largestTriangleOld) delete _largestTriangleOld;
    if (_largestTriangle) delete _largestTriangle;
    if (_largestQuadOld) delete _largestQuadOld;
    if (_largestQuad) delete _largestQuad;
    if (_enclosingRect) delete _enclosingRect;

    if (_strokes) {
	int t = _strokes->getNumItems();
	for (int i=0; i< t; i++) 
	    delete (*_strokes)[i];
	delete _strokes;
    }
}

/*----------------------------------------------------------------------------+
| Description: Creates a copy (a clone) of the current scribble.
| Output: A new scribble like the current one.
+----------------------------------------------------------------------------*/
CIScribble *CIScribble::clone()
{
    int ns, np;
    CIStroke *strk, *stroke;
    CIList<CIPoint *> *pts;
    CIScribble *scribb = new CIScribble();
 
    ns = getNumStrokes();
    for (int s=0; s<ns; s++) {
        strk = (*_strokes)[s];
        stroke = new CIStroke();
        np = strk->getNumPoints();
        pts = strk->getPoints();
        for (int p=0; p<np; p++) {
            stroke->addPoint((*pts)[p]->x, (*pts)[p]->y, (*pts)[p]->getTime());
        }
        scribb->addStroke(stroke);
    }
    return scribb;
}

/*----------------------------------------------------------------------------+
| Description: Adds a new stroke to the end of the list of strokes manipulated
|              by the scribble. It also computes the new length of the scribble
|              and the new drawing speed.
| Input: A stroke
+----------------------------------------------------------------------------*/
void CIScribble::addStroke(CIStroke *stroke)
{
    _len += stroke->getLen();
    _totalSpeed += stroke->getDrawingSpeed();
    _strokes->push(stroke);
}

/*----------------------------------------------------------------------------+
| Description: Pops out the last stroke of the scribble
| Output: The last stroke
+----------------------------------------------------------------------------*/
CIStroke *CIScribble::popStroke()
{
    CIStroke *strk;

    strk = _strokes->pop();
    _len -= strk->getLen();
    _totalSpeed -= strk->getDrawingSpeed();

    if (_boundingBox) delete _boundingBox;
    if (_convexHull) delete _convexHull;
    if (_largestTriangleOld) delete _largestTriangleOld;
    if (_largestTriangle) delete _largestTriangle;
    if (_largestQuadOld) delete _largestQuadOld;
    if (_largestQuad) delete _largestQuad;
    if (_enclosingRect) delete _enclosingRect;

    _boundingBox = NULL;
    _convexHull = NULL;
    _largestTriangle = NULL;
    _largestTriangleOld = NULL;
    _largestQuadOld = NULL;
    _largestQuad = NULL;
    _enclosingRect = NULL;

    return strk;
}

/*----------------------------------------------------------------------------+
| Description: Computes the correct timeout, based on the drawing speed.
| Output: Timeout, in milliseconds
| Notes: Actually it returns a constant value, because the formula used to 
|        the "best" timeout is not very good. We are searching for a better one :-)
+----------------------------------------------------------------------------*/
int CIScribble::getTimeOut()
{
    return 550; // to delete <<<<<<<<<<

    double avs = getAvgSpeed();
    if (avs >= 900)
        return TIMEOUT_BASE + 0;
    else if (avs <= 100)
        return TIMEOUT_BASE + 400;
    else // y=-0.5*x+450
        return (int) (TIMEOUT_BASE + (-0.5*avs + 450));
}

// ------------- Polygons

/*----------------------------------------------------------------------------+
| Description: Computes the convex hull of the scribble, using the Graham's
|              algorithm. After the computation of the convex hull, we perform
|              a simple filtration to eliminate points that are very close.
| Output: A polygon that is the convex hull.
+----------------------------------------------------------------------------*/
CIPolygon* CIScribble::convexHull()
{
    if (_convexHull == NULL) {
        _convexHull = new CIPolygon();
        CIList<CIPoint *> *ordedPoints = new CIList<CIPoint *> (false);
        CIPoint *min;
	CIPoint tmp;
        int np, i;
        CIList<CIPoint> *pts;

        min = findLowest();
        ordedPoints = ordPoints(ordedPoints,min);
        np = ordedPoints->getNumItems();
        _convexHull->push(*(*ordedPoints)[np-1]);
        _convexHull->push(*(*ordedPoints)[0]);
        
        pts = _convexHull->getPoints();
        i=1;
        int nc = _convexHull->getNumPoints();
        while (np>2 && i<np && nc>=2) { // Changed Out 2008 & Jul 2011
            if (CIFunction::left((*pts)[nc-2], (*pts)[nc-1], *(*ordedPoints)[i])) {
                _convexHull->push(*(*ordedPoints)[i]);
                i++;
                nc++;
            }
            else {
                tmp = _convexHull->pop();
                nc--;
            }
        }
        delete ordedPoints; // this list is no longer needed
        //_convexHull = filterConvexHull(); // reduce the number of points
    }
    return _convexHull;
}


/*----------------------------------------------------------------------------+
| Description: Computes the bounding box of the convex hull.
| Output: A polygon that is the bounding box.
+----------------------------------------------------------------------------*/
CIPolygon *CIScribble::boundingBox()
{
    if (_boundingBox == NULL) {
	    convexHull();

        CIList<CIPoint> *pts = _convexHull->getPoints();
        int np = _convexHull->getNumPoints();
	    double x1, x2, y1, y2;

	    x1=x2= (*pts)[0].x;
	    y1=y2=(*pts)[0].y;
  
        for ( int i = 0; i < np ; i++) {
	    if ((*pts)[i].x < x1)
	        x1 = (*pts)[i].x;
	    if ((*pts)[i].x > x2)
	        x2 = (*pts)[i].x;
	    if ((*pts)[i].y < y1)
	        y1 = (*pts)[i].y;
	    if ((*pts)[i].y > y2)
	        y2 = (*pts)[i].y;
        }

	    // Tranfer the points to a polygon
	    _boundingBox = new CIPolygon();
	    _boundingBox->addPoint(CIPoint(x1,y1));
	    _boundingBox->addPoint(CIPoint(x2,y1));
	    _boundingBox->addPoint(CIPoint(x2,y2));
	    _boundingBox->addPoint(CIPoint(x1,y2));
	    _boundingBox->addPoint(CIPoint(x1,y1));
    }
    return _boundingBox;
}  

/*----------------------------------------------------------------------------+
| Description: Computes the largest triangle that fits inside the convex hull
| Output: A polygon that is the largest triangle.
| Notes: We used the algorithm described by J.E. Boyce and D.P. Dobkin.
+----------------------------------------------------------------------------*/
CIPolygon *CIScribble::largestTriangle()
{
    if (_largestTriangle == NULL) {
	    convexHull();
	    
	int ia, ib, ic, i;
        int ripa, ripb, ripc; // indexes of rooted triangle
	double area, triArea;
        int numPts = _convexHull->getNumPoints();
        CIList<CIPoint> *pts = _convexHull->getPoints();

        if (numPts <= 3) {
	    _largestTriangle = new CIPolygon();
	    for (i=0; i<numPts; i++)
		_largestTriangle->addPoint((*pts)[i]);
	    for (i= numPts; i < 4; i++)
		_largestTriangle->addPoint((*pts)[0]);
            return _largestTriangle;
        }

// computes one rooted triangle with root in the first point of the convex hull
        ia = 0;
        area = 0;
        triArea = 0;
        for(ib=1; ib <= numPts-2; ib++) {
            if (ib >= 2)
                ic = ib + 1;
            else
                ic = 2;
            area = compRootedTri (pts, ia, ib, &ic, numPts);
            if (area > triArea) {
                triArea = area;
                ripa = ia;
                ripb = ib;
                ripc = ic;
            }
        } // ripa, ripb and ripc are the indexes of the points of the rooted triangle


// computes other triangles and choose the largest one
        double finalArea = triArea;
        int pf0, pf1, pf2;   // indexes of the final points
        int fipa, fipb, fipc; 
        int ib0;
        pf0 = ripa;
        pf1 = ripb;
        pf2 = ripc;

        for(ia=ripa+1; ia<= ripb; ia++) {
            triArea = 0;
            if (ia == ripb)
                ib0 = ripb + 1;
            else
                ib0 = ripb;
            area = 0;
            for(ib=ib0; ib <= ripc; ib++) {
                if (ib == ripc)
                    ic = ripc + 1;
                else
                    ic = ripc;
                area = compRootedTri (pts, ia, ib, &ic, numPts);
                if (area > triArea) {
                    triArea = area;
                    fipa = ia;
                    fipb = ib;
                    fipc = ic;
                }
            }
            if(triArea > finalArea) {
                finalArea = triArea;
                pf0 = fipa;
                pf1 = fipb;
                pf2 = fipc;
            }
        }

	// Tranfer the points to a polygon
	    _largestTriangle = new CIPolygon();
	    _largestTriangle->addPoint((*pts)[pf0]);
	    _largestTriangle->addPoint((*pts)[pf1]);
	    _largestTriangle->addPoint((*pts)[pf2]);
	    _largestTriangle->addPoint((*pts)[pf0]);
    }
    return _largestTriangle;
}  


/*----------------------------------------------------------------------------+
| Description: Computes the largest quadrilateral that fits inside the convex hull
| Output: A polygon that is the largest quadrilateral.
| Notes: We used the algorithm described by J.E. Boyce and D.P. Dobkin.
+----------------------------------------------------------------------------*/
CIPolygon *CIScribble::largestQuad()
{
    if (_largestQuad == NULL) {
	    convexHull();
	    
	int i, ia, ib, ic, ic0;
        int ripa, ripb, ripc; // indexes for rooted triangle
	double area, triArea;
	int numPts = _convexHull->getNumPoints();
        CIList<CIPoint> *pts = _convexHull->getPoints();

        if (numPts <= 4) {
	    _largestQuad = new CIPolygon();
	    for (i=0; i<numPts; i++)
		_largestQuad->addPoint((*pts)[i]);
	    for (i= numPts; i < 5; i++)
		_largestQuad->addPoint((*pts)[0]);
            return _largestQuad;
	}

// computes one rooted triangle        
        ia = 0;
        area = 0;
        triArea = 0;
        for(ib=1; ib <= numPts-2; ib++) {
            if (ib >= 2)
                ic = ib + 1;
            else
                ic = 2;
            area = compRootedTri (pts, ia, ib, &ic, numPts);
            if (area > triArea) {
                triArea = area;
                ripa = ia;
                ripb = ib;
                ripc = ic;
            }
        }

// computes the rooted quadrilateral based on a rooted triangle
        int fipa, fipb, fipc, fipd; // indexes for final values
        int id, ib0;
        double quadArea;

        quadArea = 0;
        for(ib=ripa+1; ib<= ripb; ib++) {
            if (ib == ripb)
                ic0 = ripb + 1;
            else
                ic0 = ripb;
            for(ic=ic0; ic <= ripc; ic++) {
                if (ic == ripc)
                    id = ripc + 1;
                else
                    id = ripc;
                area = compRootedQuad (pts, ia, ib, ic, &id, numPts);
                if (area > quadArea) {
                    quadArea = area;
                    fipa = ia;
                    fipb = ib;
                    fipc = ic;
                    fipd = id;
                }
            }
        }

// computes other quadrilaterals and choose the largest one
        int pf0, pf1, pf2, pf3;
        double finalArea = quadArea;
        pf0 = fipa;
        pf1 = fipb;
        pf2 = fipc;
        pf3 = fipd;
        ripa = fipa;
        ripb = fipb;
        ripc = fipc;
        int ripd = fipd;

        for(ia=ripa+1; ia<= ripb; ia++) {
            if (ia == ripb)
                ib0 = ripb + 1;
            else
                ib0 = ripb;

            quadArea = 0;
            area = 0;
            for(ib=ib0; ib <= ripc; ib++) {
                if (ib == ripc)
                    ic0 = ripc + 1;
                else
                    ic0 = ripc;
                for (ic=ic0; ic <= ripd; ic++) {
                    if (ic == ripd)
                        id = ripd + 1;
                    else
                        id = ripd;
                    area = compRootedQuad (pts, ia, ib, ic, &id, numPts);
                    if (area > quadArea) {
                        quadArea = area;
                        fipa = ia;
                        fipb = ib;
                        fipc = ic;
                        fipd = id;
                    }
                }
            }
            if(quadArea > finalArea) {
                finalArea = quadArea;
                pf0 = fipa;
                pf1 = fipb;
                pf2 = fipc;
                pf3 = fipd;
            }
        }

   // Tranfer the points to a polygon
	    _largestQuad = new CIPolygon();
	    _largestQuad->addPoint((*pts)[pf0]);
	    _largestQuad->addPoint((*pts)[pf1]);
	    _largestQuad->addPoint((*pts)[pf2]);
	    _largestQuad->addPoint((*pts)[pf3]);
	    _largestQuad->addPoint((*pts)[pf0]);
    }
    return _largestQuad;
}


/*----------------------------------------------------------------------------+
| Description: Computes the enclosing rectangle (rotated) that includes the 
|              convex hull
| Output: A polygon that is a rotated rectangle.
| Notes:
+----------------------------------------------------------------------------*/
CIPolygon *CIScribble::enclosingRect()
{
    if (_enclosingRect == NULL) {
	convexHull();

	int num = _convexHull->getNumPoints();
        CIList<CIPoint> *pts = _convexHull->getPoints();
	double minx,miny,maxx,maxy;
	int minxp,minyp,maxxp,maxyp;
	double ang,dis;
	double xx,yy;
	double area;
	double min_area = 0.0;
	double p1x,p1y,p2x,p2y,p3x,p3y,p4x,p4y;

        if (num < 2) {  // is just a point
	    _enclosingRect = new CIPolygon();
	    _enclosingRect->addPoint((*pts)[0]);
	    _enclosingRect->addPoint((*pts)[0]);
	    _enclosingRect->addPoint((*pts)[0]);
	    _enclosingRect->addPoint((*pts)[0]);
	    _enclosingRect->addPoint((*pts)[0]);
        }
        else if (num < 3) {  // is a line with just two points
	    _enclosingRect = new CIPolygon();
	    _enclosingRect->addPoint((*pts)[0]);
	    _enclosingRect->addPoint((*pts)[1]);
	    _enclosingRect->addPoint((*pts)[1]);
	    _enclosingRect->addPoint((*pts)[0]);
	    _enclosingRect->addPoint((*pts)[0]);
        }
        else {  // ok it's normal :-)
	    for(int i=0; i<num-1; i++) {
		for(int a=0; a<num; a++) {

		    CIVector *v1= new CIVector((*pts)[i],(*pts)[i+1]);
		    CIVector *v2= new CIVector((*pts)[i],(*pts)[a]);
		    ang = CIFunction::angle(*v1,*v2);
	      
		    dis = v2->length();
		    xx=dis*cos(ang);
		    yy=dis*sin(ang);
	      
		    if(a==0) {
			minx=maxx=xx;
			miny=maxy=yy;
			minxp=maxxp=minyp=maxyp=0;
		    }
		    if(xx<minx) {
			minxp=a;
			minx=xx;
		    }
		    if(xx>maxx) {
			maxxp=a;
			maxx=xx;
		    }
		    if(yy<miny) {
			minyp=a;
			miny=yy;
		    }
		    if(yy>maxy) {
			maxyp=a;
			maxy=yy;
		    }
	      
		    delete v1;
		    delete v2;        
		}   
		CIPoint p1 = CIFunction::closest((*pts)[i],(*pts)[i+1],(*pts)[minxp]);
		CIPoint p2 = CIFunction::closest((*pts)[i],(*pts)[i+1],(*pts)[maxxp]);
  
		CIPoint *paux = new CIPoint((*pts)[i].x+100,(*pts)[i].y);
		CIVector *v3= new CIVector((*pts)[i], *paux);
		CIVector *v4= new CIVector((*pts)[i],(*pts)[i+1]);
		ang = CIFunction::angle(*v3,*v4);
		delete paux;
		delete v3;
		delete v4;
  
		CIPoint paux1(p1.x+100*cos(ang+M_PI_2),p1.y+100*sin(ang+M_PI_2));
		CIPoint paux2(p2.x+100*cos(ang+M_PI_2),p2.y+100*sin(ang+M_PI_2));
  
		CIPoint p3 = CIFunction::closest(p2,paux2,(*pts)[maxyp]);
		CIPoint p4 = CIFunction::closest(p1,paux1,(*pts)[maxyp]);
  
		area=CIFunction::quadArea(p1,p2,p3,p4);
  
		if ((i==0)||(area < min_area))
		{
		    min_area = area;
		    p1x=p1.x;p1y=p1.y;p2x=p2.x;p2y=p2.y;p3x=p3.x;p3y=p3.y;p4x=p4.x;p4y=p4.y;
		}

	    }
	    _enclosingRect = new CIPolygon();
	    _enclosingRect->addPoint(CIPoint(p1x,p1y));
	    _enclosingRect->addPoint(CIPoint(p2x,p2y));
	    _enclosingRect->addPoint(CIPoint(p3x,p3y));
	    _enclosingRect->addPoint(CIPoint(p4x,p4y));
	    _enclosingRect->addPoint(CIPoint(p1x,p1y));
        }
    }

    return _enclosingRect;
}

/*----------------------------------------------------------------------------+
| Description: Returns the first point of the scribble.
+----------------------------------------------------------------------------*/
CIPoint CIScribble::startingPoint()
{
    return *(*((*_strokes)[0]->getPoints()))[0];
}

/*----------------------------------------------------------------------------+
| Description: Returns the last point of the scribble.
+----------------------------------------------------------------------------*/
CIPoint CIScribble::endingPoint()
{
    CIStroke *strk = (*_strokes)[getNumStrokes()-1];

    return *(*strk->getPoints())[strk->getNumPoints()-1];
}


/*----------------------------------------------------------------------------+
| Description: Writes all the points of the scribble to a file
+----------------------------------------------------------------------------*/
void CIScribble::writeTo(std::ofstream& of) 
{
    int ns, np;
    CIStroke *strk;
    CIList<CIPoint *> *pts;
 
    ns = getNumStrokes();
    of << ns << std::endl; 
    for (int s=0; s<ns; s++) {
        strk = (*_strokes)[s];
        np = strk->getNumPoints();
        pts = strk->getPoints();
        of << np << std::endl;
        for (int p=0; p<np; p++) {
            of << (*pts)[p]->x << " " << (*pts)[p]->y << " " << (*pts)[p]->getTime() << "\n";
        }
    }
}

/*----------------------------------------------------------------------------+
| Description: Reads all the points of the scribble from a file
+----------------------------------------------------------------------------*/
void CIScribble::readFrom(std::ifstream& ar, bool useTime) 
{
    int ns, np, i, k, x, y;
    double t = 0;
    CIStroke *strk;

    ar >> ns;
    for(i=0; i < ns; i++) {
        ar >> np;
        for(k=0; k < np; k++) {
            if (useTime)
                ar >> x >> y >> t;
            else
                ar >> x >> y;
            if (k==0)
                strk = new CIStroke();
            strk->addPoint(x,y,t);
        }
        addStroke(strk);
    }
}

/*----------------------------------------------------------------------------+
| Description: Computes the number of points inside a small triangle, computed
|              from the largest triangle.
| Output: Number of points inside.
| Notes: Some of the lines commented were used to return a list with the points
|        inside.
+----------------------------------------------------------------------------*/
int CIScribble::ptsInSmallTri ()
{
    //CIPolygon *inP = new CIPolygon();
    CIPolygon *tri = smallTriangle();
    CIList<CIPoint> *points = tri->getPoints();
    CIPoint pt[3];  // points of the triangle
    CIPoint cp; // current point of the scribble
    double m[3];
    double dx, dy, x[3];
    int i, inter;
    int ns, np;
    CIStroke *strk;
    CIList<CIPoint *> *pts;
    int empty = 0; // number of points inside the triangle

    for (i=0; i<3; i++)
        pt[i] = (*points)[i];  // just to be faster!

    for (i=0; i<3; i++) {
        dx = pt[i].x - pt[(i+1)%3].x;
        if (dx == 0) {
            m[i] = BIG;
            continue;
        }
        dy = pt[i].y - pt[(i+1)%3].y;
        m[i] = dy/dx;
    }

// Computation of the number of points of the scribble inside the triangle

    ns = getNumStrokes();
    for (int s=0; s<ns; s++) {
        strk = (*_strokes)[s];
        np = strk->getNumPoints();
        pts = strk->getPoints();
        for (int p=0; p<np; p++) {
            cp = *(*pts)[p];
            inter = 0;
            if (cp.x >= pt[0].x && cp.x >= pt[1].x && cp.x >= pt[2].x)
                continue;
            else if (cp.x <= pt[0].x && cp.x <= pt[1].x && cp.x <= pt[2].x)
                continue;
            else if (cp.y >= pt[0].y && cp.y >= pt[1].y && cp.y >= pt[2].y)
                continue;
            else if (cp.y <= pt[0].y && cp.y <= pt[1].y && cp.y <= pt[2].y)
                continue;
            else {
                for (i=0; i<3; i++) {
                    if (m[i] == 0)
                        continue;
                    else if (m[i] >= BIG) {
                        x[i] = pt[i].x;
                        if (x[i] >= cp.x)
                            inter++;
                    }
                    else {
                        x[i] = (double) (cp.y - pt[i].y + m[i]*pt[i].x)/m[i];
                        if (x[i] >= cp.x && (x[i] < ((pt[i].x > pt[(i+1) %3].x) ? pt[i].x : pt[(i+1) %3].x))
)                           inter++;
                    }
                }
                if (inter % 2)
                    empty++;

                    //inP->addPoint(cp);
            }
        }
    }
    //return inP;
    
    delete tri;
	
    return empty;
}


/*----------------------------------------------------------------------------+
| Description: Return the number of points of the scribble
+----------------------------------------------------------------------------*/
int CIScribble::getNumPoints()
{
    int ns, nPoints;
    
    nPoints = 0;
    ns = getNumStrokes();
    for (int s=0; s<ns; s++)
        nPoints += (*_strokes)[s]->getNumPoints();
    return nPoints;
}

/*----------------------------------------------------------------------------+
| Description: Computes the sum of all the absolute movements in X
+----------------------------------------------------------------------------*/
double CIScribble::hMovement()
{
    int ns, np;
    CIStroke *strk;
    CIList<CIPoint *> *pts;
    double mov;

    mov = 0;
    ns = getNumStrokes();
    for (int s=0; s<ns; s++) {
        strk = (*_strokes)[s];
        np = strk->getNumPoints();
        pts = strk->getPoints();
        for (int p=0; p<np-1; p++) {
            mov += fabs((*pts)[p+1]->x - (*pts)[p]->x);
        }
    }
    return mov;
}

/*----------------------------------------------------------------------------+
| Description: Computes the sum of all the absolute movements in Y
+----------------------------------------------------------------------------*/
double CIScribble::vMovement()
{
    int ns, np;
    CIStroke *strk;
    CIList<CIPoint *> *pts;
    double mov;

    mov = 0;
    ns = getNumStrokes();
    for (int s=0; s<ns; s++) {
        strk = (*_strokes)[s];
        np = strk->getNumPoints();
        pts = strk->getPoints();
        for (int p=0; p<np-1; p++) {
            mov += fabs((*pts)[p+1]->y - (*pts)[p]->y);
        }
    }
    return mov;
}

// ------------------------------------------------------------
// ------ Private Members -------------------------------------

/*----------------------------------------------------------------------------+
| Description: Computes a small triangle that is 60% of the largest triangle.
| Output: A polygon
| Notes:
+----------------------------------------------------------------------------*/
CIPolygon *CIScribble::smallTriangle()
{
    CIPolygon* tri = new CIPolygon();
    CIList<CIPoint> *points;
    CIPoint p1, p2, p3;
    CIPoint m1, m2, m3;
    CIPoint t1, t2, t3;

    if (!_largestTriangle)
        largestTriangle();

	if (_largestTriangle == NULL)
		points = NULL;

    points = _largestTriangle->getPoints();

    p1 = (*points)[0];
    p2 = (*points)[1];
    p3 = (*points)[2];

    m1.x = p3.x + (p1.x - p3.x)/2;
    m1.y = p3.y + (p1.y - p3.y)/2;
    m2.x = p1.x + (p2.x - p1.x)/2;
    m2.y = p1.y + (p2.y - p1.y)/2;
    m3.x = p2.x + (p3.x - p2.x)/2;
    m3.y = p2.y + (p3.y - p2.y)/2;

    t1.x = m3.x + (p1.x - m3.x)*0.6;
    t1.y = m3.y + (p1.y - m3.y)*0.6;
    t2.x = m1.x + (p2.x - m1.x)*0.6;
    t2.y = m1.y + (p2.y - m1.y)*0.6;
    t3.x = m2.x + (p3.x - m2.x)*0.6;
    t3.y = m2.y + (p3.y - m2.y)*0.6;

    tri->addPoint(t1);
    tri->addPoint(t2);
    tri->addPoint(t3);
    tri->addPoint(t1);

    return tri;
}

/*----------------------------------------------------------------------------+
| Description: Reduce the number of points from the convex hull, eliminating 
|              the points that are too close (5 pixels).
+----------------------------------------------------------------------------*/
CIPolygon* CIScribble::filterConvexHull()
{
    CIPolygon* _convexHull2 = new CIPolygon();
    int np, i;
    CIList<CIPoint> *pts;
    CIPoint pt, pti;

    pts = _convexHull->getPoints();
    np = _convexHull->getNumPoints();
    pt = (*pts)[0];
    _convexHull2->push(pt);
    for (i=1; i<np; i++) {
        pti = (*pts)[i];
        if (CIFunction::distance(pt, pti) > 5) {
            _convexHull2->push(pti);
            pt = pti;
        }
        else if (i == np-1)
            _convexHull2->push((*pts)[0]);
    }
    delete _convexHull;
    return _convexHull2;
}


/*----------------------------------------------------------------------------+
| Description: Selects the point with the lowest y
+----------------------------------------------------------------------------*/
CIPoint *CIScribble::findLowest ()
{
    CIPoint *min;
    int ns, np, s, p;
    CIList<CIPoint *> *pts;

    min = (*(*_strokes)[0]->getPoints())[0]; // gets the first point of the first stroke
    ns = getNumStrokes();
    for (s=0; s<ns; s++) {
        np = (*_strokes)[s]->getNumPoints();
        pts = (*_strokes)[s]->getPoints();
        for (p=0; p<np; p++) 
            if ((*pts)[p]->y < min->y)
                min = (*pts)[p];
            else if ((*pts)[p]->y == min->y && (*pts)[p]->x > min->x)
                min = (*pts)[p];
    }
    return min;
}

/*----------------------------------------------------------------------------+
| Description: Order all scribble points by angle.
| Output: A list of points order by angle
| Notes: This method is used during the computation of the convex hull.
+----------------------------------------------------------------------------*/
CIList<CIPoint *> *CIScribble::ordPoints(CIList<CIPoint *> *ordedPoints, CIPoint *min)
{
    int ns, np, s, p;
    CIList<CIPoint *> *pts;
    CIListNode<CIPoint *> *ptr;
    double ang;

    ordedPoints->insertInOrder(min, 0);

    ns = getNumStrokes();
    for (s=0; s<ns; s++) {
        np = (*_strokes)[s]->getNumPoints();
        pts = (*_strokes)[s]->getPoints();
        for (p=0; p<np; p++) {
            ang = CIFunction::theta (*min, *(*pts)[p]);
            ptr = ordedPoints->insertInOrder((*pts)[p], ang);
            if (ptr) { // there is another point with the same angle
                       // so we compute the distance and save the big one.
                if (CIFunction::distance(*min, *(*pts)[p]) > CIFunction::distance (*min, *ptr->getItem()))
                    ptr->setItem((*pts)[p]);
            }
        }
    }
    return ordedPoints;
}


/*----------------------------------------------------------------------------+
| Description: Auxiliar method used during the computation of the largest triangle
+----------------------------------------------------------------------------*/
double CIScribble::compRootedTri(CIList<CIPoint> *pts, int ripa, int ripb, int *ripc, int np)
{
	CIPoint pa,pb,pc;
	int ia, ib, ic;
	double area, _trigArea = 0;

// computes one rooted triangle        
    ia = ripa;
    ib = ripb;
    for(ic=*ripc; ic < np - 1; ic++ ) {
	    pa = (*pts)[ia];
	    pb = (*pts)[ib];
	    pc = (*pts)[ic];
	    if( (area=CIFunction::triangleArea(pa, pb, pc)) > _trigArea ) {
            *ripc = ic;
	        _trigArea = area;
	    }
        else {
            break;
        }
    }
    return _trigArea;
}

/*----------------------------------------------------------------------------+
| Description: Auxiliar method used during the computation of the largest 
|              quadrilateral
+----------------------------------------------------------------------------*/
double CIScribble::compRootedQuad(CIList<CIPoint> *pts, int ripa, int ripb, int ripc, int *ripd, int np)
{
	CIPoint pa,pb,pc,pd;
	int id;

	double area, _trigArea = 0;

// computes one rooted triangle        
	pa = (*pts)[ripa];
	pb = (*pts)[ripb];
	pc = (*pts)[ripc];
    for(id=*ripd; id < np - 1; id++ ) {
        pd = (*pts)[id];
	    if( (area=CIFunction::quadArea(pa, pb, pc, pd)) > _trigArea ) {
                *ripd = id;
	    _trigArea = area;
	    }
        else {
            break;
        }
    }
    return _trigArea;
}


// ------------------------------------------------------------
/* This cicle uses all the points of the scribble
    int ns, np;
    CIStroke *strk;
    CIList<CIPoint *> *pts;

        ns = getNumStrokes();
        for (int s=0; s<ns; s++) {
            strk = (*_strokes)[s];
            np = strk->getNumPoints();
            pts = strk->getPoints();
            for (int p=0; p<np; p++) {

                // do something with the points.
                // (*pts)[p]

            }
        }

  */

//---------- To Delete one day ------------------

CIPolygon *CIScribble::largestTriangleOld()
{
    if (_largestTriangleOld == NULL) {
	convexHull();
	
	CIPoint p1,p2,p3;
	CIPoint _trigPts[3];
	int i,j,k;
	double area, _trigArea = 0;
	int numPts = _convexHull->getNumPoints();
        CIList<CIPoint> *pts = _convexHull->getPoints();
  
	for( i=0; i < numPts; i++ )
	    for( j=0; j< numPts; j++ )
		for( k=0; k<numPts; k++ ) {
		    p1 = (*pts)[i];
		    p2 = (*pts)[j];
		    p3 = (*pts)[k];
		    if( (area=CIFunction::triangleArea(p1, p2, p3)) > _trigArea ) {
			_trigPts[0]=p1;
			_trigPts[1]=p2;
			_trigPts[2]=p3;
			_trigArea = area;
		    }
		}

	// Tranfer the points to a polygon
	_largestTriangleOld = new CIPolygon();
	_largestTriangleOld->addPoint(_trigPts[0]);
	_largestTriangleOld->addPoint(_trigPts[1]);
	_largestTriangleOld->addPoint(_trigPts[2]);
	_largestTriangleOld->addPoint(_trigPts[0]);
    }
    return _largestTriangleOld;
}  


CIPolygon *CIScribble::largestQuadOld()
{
    if (_largestQuadOld == NULL) {
	convexHull();
	
	CIPoint p1,p2,p3,p4;
	CIPoint _trigPts[4];
	int i,j,k,l;
	double area, _quadArea = 0;
	int numPts = _convexHull->getNumPoints();
        CIList<CIPoint> *pts = _convexHull->getPoints();
  
	for( i=0; i < numPts; i++ )
	    for( j=0; j< numPts; j++ )
		for( k=0; k<numPts; k++ )
		    for( l=0; l<numPts; l++ ) {
		    p1 = (*pts)[i];
		    p2 = (*pts)[j];
		    p3 = (*pts)[k];
		    p4 = (*pts)[l];
		    if( (area=CIFunction::quadArea(p1, p2, p3, p4)) > _quadArea ) {
			_trigPts[0]=p1;
			_trigPts[1]=p2;
			_trigPts[2]=p3;
			_trigPts[3]=p4;
			_quadArea = area;
		    }
		}

	// Tranfer the points to a polygon
	_largestQuadOld = new CIPolygon();
	_largestQuadOld->addPoint(_trigPts[0]);
	_largestQuadOld->addPoint(_trigPts[1]);
	_largestQuadOld->addPoint(_trigPts[2]);
	_largestQuadOld->addPoint(_trigPts[3]);
	_largestQuadOld->addPoint(_trigPts[0]);
    }
    return _largestQuadOld;
}
