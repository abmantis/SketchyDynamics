#include "../dependecies/CALI/CIAlpha.h"
#include "../dependecies/CALI/CIVector.h"
#include "../dependecies/CALI/CIFunction.h"

CIAlpha::CIAlpha ()
{
    _features = new CIFeatures (&CIEvaluate::Alq_Ach, 0.85, 0.91, 1, 1
                                ,&CIEvaluate::Plq_Pch, 0.95, 0.96, 1, 1
                                );
}

/*----------------------------------------------------------------------------+
| Description: This method analyse the scribble taking into account local 
|              features of an alpha. An alpha is built up of one line that 
|              selft intersects somewhere at the middle. 
| Input: A scribble and list of all possible gestures.
| Output: degree of membership
+----------------------------------------------------------------------------*/
double CIAlpha::evalLocalFeatures(CIScribble *sc, CIList<CIGesture *>* _shapesList)
{
	CIStroke *strk = (*sc->getStrokes())[0];
	int nPts = strk->getNumPoints();
	CIList<CIPoint *> *pts = strk->getPoints();
	
	// skip 5% from beggin and end of the scribble	
	double ignoreLen = strk->getLen() * 0.07f;
	double currLen = 0;
	int startIndex = -1;
	int endIndex = -1;
	for (int i = 0; i < nPts - 1; i++)
	{
		currLen += CIFunction::distance(*(*pts)[i], *(*pts)[i+1]);
		if(currLen > ignoreLen)
		{
			startIndex = i;
			break;
		}
	}
	currLen = 0;
	for (int i = nPts-1; i > 0; i--)
	{
		currLen += CIFunction::distance(*(*pts)[i], *(*pts)[i-1]);
		if(currLen > ignoreLen)
		{
			endIndex = i;
			break;
		}
	}

	// check for intersection
	for (int i = startIndex; i < endIndex; i++)
	{
		for (int j = i+2; j < endIndex; j++)
		{
			if(intersect(*(*pts)[i], *(*pts)[i+1], *(*pts)[j], *(*pts)[j+1]))
			{
				// Show % indicating the distance between the two strokes that 
				// form the intersection, relative to the whole stroke length
				//double pointDist = 0;
				//for (int x = i; x < j; x++)
				//{
				//	pointDist += CIFunction::distance(*(*pts)[x], *(*pts)[x+1]);
				//}

				//std::cout << (float)(j-i) / (float)nPts << "   " << pointDist/strk->getLen();
				
				float intersectPtsDist = (float)(j-i) / (float)nPts;
				if(intersectPtsDist > 0.2f)
				{
					intersectStroke1_s = *(*pts)[i];
					intersectStroke1_e = *(*pts)[i+1];
					intersectStroke2_s = *(*pts)[j];
					intersectStroke2_e = *(*pts)[j+1];
					
					return 1;
				}
				else
				{
					return 0;
				}				
			}
		}
	}

	return 0;
}


/*----------------------------------------------------------------------------+
| Description: checks if the two lines of the scribble intersects
+----------------------------------------------------------------------------*/
bool CIAlpha::intersect(CIPoint stroke1_s, CIPoint stroke1_e, CIPoint stroke2_s, CIPoint stroke2_e)
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

void CIAlpha::getIntersectionLines( CIPoint &stroke1_s, CIPoint &stroke1_e, CIPoint &stroke2_s, CIPoint &stroke2_e )
{
	stroke1_s = intersectStroke1_s;
	stroke1_e = intersectStroke1_e;
	stroke2_s = intersectStroke2_s;
	stroke2_e = intersectStroke2_e;
}
