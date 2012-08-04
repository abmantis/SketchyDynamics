/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CICircle.h
+-----------------------------------------------------------------------------+
| Description: Represents a circle. (solid, dashed or bold)
|
| Notes:       
|
| Author: Manuel Joao Fonseca
|	  e-mail: mjf@inesc-id.pt
|
| Date: April 98, April 99
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
#ifndef _CICIRCLE_H
#define _CICIRCLE_H

#pragma warning( push )
#pragma warning( disable : 4100)

#include "CIShape.h"

class CICircle : public CIShape {

public:
    CICircle (bool rotated=true);
    CICircle (CIScribble* sc, double dom, bool dash = false, bool bold = false);
	~CICircle () {};

    void setUp(CIScribble* sc);
    void draw(void *ptr) {}
    char *getName() { return ("Circle"); }
    CIGesture* clone();

    double _radius;
    CIPoint _center;
    CIPoint _points[4];
};

#pragma warning( pop )

#endif
