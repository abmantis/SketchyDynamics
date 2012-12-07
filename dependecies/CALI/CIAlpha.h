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

	void getIntersectionLines(CIPoint &stroke1_s, CIPoint &stroke1_e, CIPoint &stroke2_s, CIPoint &stroke2_e);

private:
    bool intersect(CIPoint stroke1_s, CIPoint stroke1_e, CIPoint stroke2_s, CIPoint stroke2_e);

	CIPoint intersectStroke1_s;
	CIPoint intersectStroke1_e;
	CIPoint intersectStroke2_s;
	CIPoint intersectStroke2_e;
};

#endif

