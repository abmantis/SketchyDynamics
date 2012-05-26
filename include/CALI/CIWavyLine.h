/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIWavyLine.h
+-----------------------------------------------------------------------------+
| Description: This class represents a WavyLine gesture.
| 
| Notes:       
|
| Author: Manuel Joao Fonseca
|	  e-mail: mjf@inesc-id.pt
|
| Date: April 98, May 99
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
#ifndef _CIWAVYLINE_H
#define _CIWAVYLINE_H

#include "CIScribble.h"
#include "CICommand.h"

class CIWavyLine : public CICommand {

public: 
    CIWavyLine();
    CIWavyLine (CIScribble* sc, double dom) { _sc = sc; _dom = dom; _features = NULL; }

    CIGesture* clone() { return new CIWavyLine(_sc, _dom); }
    char* getName() { return "WavyLine"; }

	double evalLocalFeatures(CIScribble *sc, CIList<CIGesture *>* _shapesList);
private:
	bool intersect(CIPoint stroke1_s, CIPoint stroke1_e, CIPoint stroke2_s, CIPoint stroke2_e);
};

#endif
