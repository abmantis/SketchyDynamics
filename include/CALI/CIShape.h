/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIShape.h
+-----------------------------------------------------------------------------+
| Description: Base class for all shapes. 
| 
| Notes:       
|
| Author: Manuel Joao Fonseca
|	  e-mail: mjf@inesc-id.pt
|
| Date: April 98, April99
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
#ifndef _CISHAPE_H
#define _CISHAPE_H

#pragma warning( push )
#pragma warning( disable : 4100)


#include "CIGesture.h"
#include "CIScribble.h"
#include "CIFuzzyNode.h"

class CIShape : public CIGesture {

public:
    CIShape (bool rotated = true);
    ~CIShape ();

    virtual void setUp(CIScribble* sc){}

    virtual void draw(void *ptr) {}
    virtual char *getName() { return NULL; }
    virtual CIGesture* clone(){ return NULL; }
    void popAttribs ();
    
    char *getGestureType() { return "Shape"; }
    double evalGlobalFeatures(CIScribble* sc);
    double evalLocalFeatures(CIScribble *sc, CIList<CIGesture *>* _shapesList) { return 1; }
    bool isDashed()        { return _dashed; }
    bool isBold()          { return _bold; }
    bool isOpen()          { return _open; }

protected:
        CIFeatures* _dashFeature; 
        CIFeatures* _normalFeature;
        CIFeatures* _openFeature;
        CIFeatures* _boldFeature;
        bool _dashed;
        bool _bold;
        bool _open;
        bool _rotated;
};

#pragma warning( pop )
#endif
