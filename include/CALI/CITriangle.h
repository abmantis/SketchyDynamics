/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CITriangle.h
+-----------------------------------------------------------------------------+
| Description: Represents a triangle. (solid, dashed or bold)
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
#ifndef _CITRIANGLE_H
#define _CITRIANGLE_H
#pragma warning( push )
#pragma warning( disable : 4100 )

#include "CIShape.h"

class CITriangle : public CIShape {

public:
    CITriangle (bool rotated=true);
    CITriangle (CIScribble* sc, CIPoint a, CIPoint b, CIPoint c, double dom, bool dash = false, bool bold = false);
    ~CITriangle () {};

    void setUp(CIScribble* sc);
    void draw(void *ptr) {}
    char *getName() { return ("Triangle"); }
    CIGesture* clone();

    CIPoint _points[3];
};
#pragma warning( pop )
#endif
