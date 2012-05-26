/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIPoint.h
+-----------------------------------------------------------------------------+
| Description: Class that implements a bi-dimensional point with a temporal
| 	       mark.
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
#ifndef _CIPOINT_H
#define _CIPOINT_H

class CIPoint {
    public:
	double x, y;

	CIPoint () { x=0; y=0; _time=0; }
	CIPoint (double xx, double yy, double time=0) { x=xx; y=yy; _time=time; }
	bool const operator==(const CIPoint& p) const { return x == p.x && y == p.y; };

#ifdef WIN32
	CIPoint& operator= (CIPoint& point);
	inline double getTime() { return _time; }
#else
// For Linux
	CIPoint& operator= (const CIPoint& point);
	inline double getTime() const { return _time; }
#endif

    protected:
	double _time;  // temporal mark of the point in milliseconds
};
#endif
