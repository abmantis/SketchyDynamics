/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIRecognizer.cxx
+-----------------------------------------------------------------------------+
| Description: Implements the class CIRecognizer
| 
| Notes:       
|
| Author: Manuel Joao Fonseca
|	  e-mail: mjf@inesc-id.pt
|
| Date: Oct 98, April 99
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

#include "CALI/CIRecognizer.h"
#include <string.h>

CIRecognizer::CIRecognizer (bool rotated, double alfaCut)
{
    _alfaCut = alfaCut;

    _shapesList = new CIList<CIGesture *> ();
    _unknownShape = new CIUnknown();

// Gestures (Shapes and Commands) identified by the recognizer. 
// The next lines create a list with all gestures identified by the recognizer
// If you want to add a new gesture to the recognizer, just add it to the list.

	_shapesList->insertTail(new CIAlpha());  
    _shapesList->insertTail(new CIWavyLine());  
	_shapesList->insertTail(new CILine(rotated));
    _shapesList->insertTail(new CITriangle(rotated));
    _shapesList->insertTail(new CIRectangle(rotated));
    _shapesList->insertTail(new CICircle(rotated));
    _shapesList->insertTail(new CIEllipse(rotated));
    _shapesList->insertTail(new CIDiamond(rotated));
}

CIRecognizer::~CIRecognizer ()
{ 
    if (_unknownShape) delete _unknownShape; 
    if (_shapesList) {
	int t = _shapesList->getNumItems();
	for (int i=0; i< t; i++) 
	    delete (*_shapesList)[i];
	delete _shapesList; 
    }
}

/*----------------------------------------------------------------------------+
| Description: Identifies shapes based on a scribble. It starts by looking
|              for global geometric features and then for local features
| Input: sc - A scribble
| Output: A list of plausible shapes ordered by degree of certainty.
|
| Notes: If the application wants to manipulate the gestures returned as new 
|        entities, it must clone them, because the gestures return by the
|        recognizer are always the same. (The gestures returned are the ones
|        created in the recognizer constructor)
+----------------------------------------------------------------------------*/
CIList<CIGesture *> *CIRecognizer::recognize(CIScribble *sc)
{
    double val, val2;
    int i;
    int nshapes = _shapesList->getNumItems();
    CIList<CIGesture *>* _shapes2Return;
    _shapes2Return = new CIList<CIGesture *> ();

	//printFeatures(sc);

    for (i=0; i<nshapes; i++) {     // set doms of all gestures to zero
        (*_shapesList)[i]->resetDom();
    }


    for (i=0; i<nshapes; i++) {
        val = (*_shapesList)[i]->evalGlobalFeatures(sc);
        if (val > _alfaCut) {
            val2 = (*_shapesList)[i]->evalLocalFeatures(sc, _shapesList);
            if (val2 < val)
                val = val2;
            if (val > _alfaCut) {
                _shapes2Return->insertInOrder((*_shapesList)[i], 1 - val); 
                // (1-val) is used because the method insertInOrder creates an
                // ascendant list, and we want a descendant one.
            }
        }
    }
	

    if (_shapes2Return->getNumItems() == 0) {
		    _unknownShape->setUp(sc);
		    _shapes2Return->insertInOrder(_unknownShape, 1 - 0);
    }

    return _shapes2Return;
}

void CIRecognizer::printFeatures(CIScribble *sc)
{
	CIEvaluate *ev = new CIEvaluate();
	
	std::cout << "******************" << std::endl;
	std::cout << "* Special ratios *" << std::endl;
	std::cout << "Tl_Pch     " << ev->Tl_Pch    (sc) << std::endl;
	std::cout << "Pch2_Ach   " << ev->Pch2_Ach  (sc) << std::endl;
	std::cout << "Pch_Ns_Tl  " << ev->Pch_Ns_Tl (sc) << std::endl;
	std::cout << "Hollowness " << ev->Hollowness(sc) << std::endl;
	std::cout << "Ns         " << ev->Ns        (sc) << std::endl;

	std::cout << "* Other special ones *" << std::endl;
	std::cout << "Hm_Wbb     " << ev->Hm_Wbb  (sc) << std::endl;
	std::cout << "Vm_Hbb     " << ev->Vm_Hbb  (sc) << std::endl;
	std::cout << "Hbb_Wbb    " << ev->Hbb_Wbb (sc) << std::endl;
	std::cout << "Her_Wer    " << ev->Her_Wer (sc) << std::endl;
	std::cout << "Diag_er    " << ev->Diag_er (sc) << std::endl;
	std::cout << "pch_psc    " << ev->pch_psc (sc) << std::endl;

	std::cout << "* Area ratios *" << std::endl;
	std::cout << "Alt_Ach    " << ev->Alt_Ach (sc) << std::endl;
	std::cout << "Ach_Aer    " << ev->Ach_Aer (sc) << std::endl;
	std::cout << "Alt_Aer    " << ev->Alt_Aer (sc) << std::endl;
	std::cout << "Ach_Abb    " << ev->Ach_Abb (sc) << std::endl;
	std::cout << "Alt_Abb    " << ev->Alt_Abb (sc) << std::endl;
	std::cout << "Alq_Ach    " << ev->Alq_Ach (sc) << std::endl;
	std::cout << "Alq_Aer    " << ev->Alq_Aer (sc) << std::endl;
	std::cout << "Alt_Alq    " << ev->Alt_Alq (sc) << std::endl;
	std::cout << "Aer_Abb    " << ev->Aer_Abb (sc) << std::endl;
	std::cout << "Alq_Abb    " << ev->Alq_Abb (sc) << std::endl;

	std::cout << "* Perimeter ratios *" << std::endl;
	std::cout << "Plt_Pch    " << ev->Plt_Pch (sc) << std::endl;
	std::cout << "Pch_Per    " << ev->Pch_Per (sc) << std::endl;
	std::cout << "Plt_Per    " << ev->Plt_Per (sc) << std::endl;
	std::cout << "Pch_Pbb    " << ev->Pch_Pbb (sc) << std::endl;
	std::cout << "Plt_Pbb    " << ev->Plt_Pbb (sc) << std::endl;
	std::cout << "Plq_Pch    " << ev->Plq_Pch (sc) << std::endl;
	std::cout << "Plq_Per    " << ev->Plq_Per (sc) << std::endl;
	std::cout << "Plt_Plq    " << ev->Plt_Plq (sc) << std::endl;
	std::cout << "Per_Pbb    " << ev->Per_Pbb (sc) << std::endl;
	std::cout << "Plq_Pbb    " << ev->Plq_Pbb (sc) << std::endl;
}