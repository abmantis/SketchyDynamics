/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIGesture.h
+-----------------------------------------------------------------------------+
| Description: Base class for all gestures, shapes and commands. 
| 
| Notes:       
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
#ifndef _CIGESTURE_H
#define _CIGESTURE_H
#pragma warning( push )
#pragma warning( disable : 4100 )

#include "CIScribble.h"
#include "CIFeatures.h"

class CIGesture {

public:
    CIGesture ()          { _sc = NULL; _dom = 0; _prevGesture = NULL; }
    virtual ~CIGesture()          { if (_features) delete _features; }

    virtual char *getName() { return NULL; }
    virtual char *getGestureType() { return NULL; }
    virtual CIGesture* clone(){ return NULL; }
    
    virtual double evalGlobalFeatures(CIScribble* sc) { return 0; }
    virtual double evalLocalFeatures(CIScribble *sc, CIList<CIGesture *>* _shapesList) { return 1; }
    CIScribble* getScribble() { return _sc; }
    double getDom() { return _dom; }
    void resetDom() { _dom = 0; }
    
    void pushAttribs () { _prevGesture = clone(); }
    virtual void popAttribs () {}

protected:
    CIScribble* _sc;
    CIFeatures* _features;
    double _dom;
    CIGesture* _prevGesture;
};
#pragma warning( pop )
#endif

