/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIAlpha.h
+-----------------------------------------------------------------------------+
| Description: 
| 
| Notes:       
|
| $Log$
|
| Author: Manuel Joao Fonseca
|	  e-mail: mjf@inesc-id.pt
|
| Date: Oct 99
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
#ifndef _CIALPHA_H
#define _CIALPHA_H

#include "CIScribble.h"
#include "CICommand.h"

class CIAlpha : public CICommand {

public: 
    CIAlpha();
    CIAlpha (CIScribble* sc, double dom) { _sc = sc; _dom = dom; _features = NULL; }

    CIGesture* clone() { return new CIAlpha(_sc, _dom); }
    char* getName() { return "Alpha"; }
    
    double evalLocalFeatures(CIScribble *sc, CIList<CIGesture *>* _shapesList);

private:
    bool intersect(CIPoint stroke1_s, CIPoint stroke1_e, CIPoint stroke2_s, CIPoint stroke2_e);
};

#endif

