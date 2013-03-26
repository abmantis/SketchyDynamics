/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CICommand.h
+-----------------------------------------------------------------------------+
| Description: Base class for all commands. 
| 
| Notes:       
|
| Author: Manuel Joao Fonseca
|	  e-mail: mjf@inesc-id.pt
|
| Date: April99
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
#ifndef _CICOMMAND_H
#define _CICOMMAND_H

#pragma warning( push )
#pragma warning( disable : 4100 )

#include "CIGesture.h"
#include "CIScribble.h"

class CICommand : public CIGesture {

public:
    CICommand ()          { _sc = NULL; }

    virtual char *getName() { return NULL; }
    char *getGestureType() { "Command"; }
    virtual CIGesture* clone(){ return NULL; }
    void popAttribs ();
    
    double evalGlobalFeatures(CIScribble* sc);
    double evalLocalFeatures(CIScribble *sc, CIList<CIGesture *>* _shapesList) { return 1; }
protected:
};

#pragma warning( pop )
#endif

