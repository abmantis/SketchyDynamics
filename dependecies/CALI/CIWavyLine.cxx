/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIWavyLine.cxx
+-----------------------------------------------------------------------------+
| Description: Implements the class CIWavyLine.
| 
| Notes:       
|
| Author: Manuel Joao Fonseca
|	  e-mail: mjf@inesc-id.pt
|
| Date: May 98, May 99
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

#include "CIWavyLine.h"
#include "CIVector.h"
#include "CIFunction.h"

CIWavyLine::CIWavyLine ()
{
    _features = new CIFeatures (&CIEvaluate::Her_Wer, 0.06, 0.08, 0.4, 0.45, // separate from lines
                               //,&CIEvaluate::Hm_Wbb, 0.9, 0.98, 1.1, 1.1,   // separate from Arrows
                               &CIEvaluate::Tl_Pch, 0.5, 0.55, 1.5, 1.9,
                               &CIEvaluate::Hollowness, 0, 3, BIG, BIG
                               //,&CIEvaluate::Ns, 1, 1, 1, 1
                               );
}

/*----------------------------------------------------------------------------+
| Description: This method analyse the scribble taking into account local 
|              features of an WavyLine. This is needed to separate it from alphas 
| Input: A scribble and list of all possible gestures.
| Output: degree of membership
+----------------------------------------------------------------------------*/
double CIWavyLine::evalLocalFeatures( CIScribble *sc, CIList<CIGesture *>* _shapesList )
{
	CIStroke *strk = (*sc->getStrokes())[0];
	int nPts = strk->getNumPoints();
	CIList<CIPoint *> *pts = strk->getPoints();

	// check for intersection
	for (int i = 0; i < nPts - 1; i++)
	{
		for (int j = i+2; j < nPts - 1; j++)
		{
			if(intersect(*(*pts)[i], *(*pts)[i+1], *(*pts)[j], *(*pts)[j+1]))
			{
				float intersectPtsDist = (float)(j-i) / (float)nPts;
				if(intersectPtsDist > 0.13f)				
				{
					return 0;
				}				
			}
		}
	}

	return 1;
}

/*----------------------------------------------------------------------------+
| Description: checks if the two lines of the scribble intersects
+----------------------------------------------------------------------------*/
bool CIWavyLine::intersect(CIPoint stroke1_s, CIPoint stroke1_e, CIPoint stroke2_s, CIPoint stroke2_e)
{	
	CIVector *v1 = new CIVector (stroke1_s.x, stroke1_s.y, stroke1_e.x, stroke1_e.y);
	CIVector *v2 = new CIVector (stroke2_s.x, stroke2_s.y, stroke2_e.x, stroke2_e.y);
	
	bool left1, left2;
	left1 = CIFunction::left(v1->startp, v1->endp, v2->startp) ^ CIFunction::left(v1->startp, v1->endp, v2->endp);
	left2 = CIFunction::left(v2->startp, v2->endp, v1->startp) ^ CIFunction::left(v2->startp, v2->endp, v1->endp);

	delete v1;
	delete v2;

	return left1 && left2;
}
