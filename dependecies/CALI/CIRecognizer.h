/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIRecognizer.h
+-----------------------------------------------------------------------------+
| Description: A recognizer that uses fuzzy logic. It can recognize 
|              multistroke and unistroke gestures, invariant in rotation and 
|              scale.
|              It uses a list of all possible shapes and commands, and during
|              the recognition process computes the similarity between the 
|              scribble and the different gestures.
| 
| Notes:       
|
| Author: Manuel Joao Fonseca
|	  e-mail: mjf@inesc-id.pt
|
| Date: Oct 98, May 99
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
#ifndef _CIRECO_H
#define _CIRECO_H

#include "CIGesture.h"
#include "CIScribble.h"
#include "CIEvaluate.h"
#include "CIFuzzySet.h"
#include "Shapes.h"  // include all shapes
#include "Commands.h" // include all commands


class CIRecognizer {
public:
    CIRecognizer (bool rotated = true, double alfaCut = 0);
    ~CIRecognizer ();

    CIList<CIGesture *> *recognize (CIScribble *sc);
	void printFeatures(CIScribble *sc);
protected:
    CIList<CIGesture *>* _shapesList;
    double _alfaCut;
    CIShape *_unknownShape;
};
#endif
