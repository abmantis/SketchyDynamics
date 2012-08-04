/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIStroke.h
+-----------------------------------------------------------------------------+
| Description: Class that represents a stroke as a set of points. Each point of
|              the stroke has a temporal mark, which is used to calculate the
|              drawing speed of the stroke.
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
#ifndef _CISTROKE_H
#define _CISTROKE_H

#include "CIList.h"
#include "CIPoint.h"

class CIStroke {
    public:
        CIStroke ();
        ~CIStroke ();

	void addPoint(double x, double y, double time=0); // time in milliseconds

	inline int getNumPoints()	// Number of points of the stroke
	    { return _points->getNumItems(); }
	    
	inline double getLen()		// Stroke length
	    { return _len; }
	    
	inline CIList<CIPoint *> *getPoints()  // List of points of the stroke
	    { return _points; }
	    
	inline double getDrawingSpeed()	    // Average drawing speed 
	    { return _speed; }

    protected:
        CIList<CIPoint *> *_points;
	double _len;
	double _speed;
	CIPoint _lastPoint;
	double _firstTime;
};
#endif
