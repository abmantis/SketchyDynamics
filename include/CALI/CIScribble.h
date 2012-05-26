/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIScribble.h
+-----------------------------------------------------------------------------+
| Description: This class defines a scribble as a set of strokes. It offers
|              a set of methods to compute some polygons used during the 
|              recognition process.
| 
| Notes:       
|
| Author: Manuel Joao Fonseca
|	  e-mail: mjf@inesc-id.pt
|
| Date: April 98
| Changed: April 99
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
#ifndef _CISCRIBBLE_H
#define _CISCRIBBLE_H

#include <math.h>
#include <fstream>
#include "CIStroke.h"
#include "CIList.h"
#include "CIPolygon.h"

class CIScribble {

public:
    CIScribble ();
    ~CIScribble ();

    CIScribble *clone();

// Stroke methods
    void addStroke (CIStroke *stroke);
    
    inline int getNumStrokes ()		// Number of strokes of the scribble
        { return _strokes->getNumItems(); }
	
    inline CIList<CIStroke *> *getStrokes()  // List of strokes of the scribble
        { return _strokes; }
	
    inline double getLen()	// Scribble length
        { return _len; }

    inline double getAvgSpeed()	// Drawing average speed of the scribble
        { return _totalSpeed/getNumStrokes(); }

    CIStroke *popStroke();	// Excludes the last stroke from the scribble
 
    int getTimeOut();           // Returns the appropriate time-out for the scribble
    int getNumPoints();

// Store and retrieve methods
    void writeTo(std::ofstream& of);
    void readFrom(std::ifstream& ar, bool useTime = false);

// Polygons 
    CIPolygon* boundingBox ();
    CIPolygon* convexHull ();
    CIPolygon* largestTriangle ();
    CIPolygon* largestQuad ();
    CIPolygon* enclosingRect ();

// Methods to delete in the future. They are used just for debug
    CIPolygon* largestTriangleOld ();
    CIPolygon* largestQuadOld ();

// Some methods for other information about the scribble
    CIPoint startingPoint();
    CIPoint endingPoint();
    int ptsInSmallTri ();
    double hMovement();
    double vMovement();

  protected:
    double _len;
    double _totalSpeed;
    CIList<CIStroke *> *_strokes;

    CIPolygon* _boundingBox;    // The points are order
    CIPolygon* _convexHull;
    CIPolygon* _largestTriangleOld;
    CIPolygon* _enclosingRect;
    CIPolygon* _largestQuadOld;
    CIPolygon* _largestTriangle;
    CIPolygon* _largestQuad;

  private:
    CIPoint *findLowest ();
    CIList<CIPoint *> *ordPoints(CIList<CIPoint *> *ordedPoints, CIPoint *min);
    CIPolygon* filterConvexHull();
    CIPolygon* smallTriangle();
    double compRootedTri(CIList<CIPoint> *pts, int ripa, int ripb, int *ripc, int np);
    double compRootedQuad(CIList<CIPoint> *pts, int ripa, int ripb, int ripc, int *ripd, int np);
};

#endif
