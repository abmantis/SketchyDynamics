#include "PhySketchBodyPolygon.h"

namespace PhySketch
{

BodyPolygon::BodyPolygon(DrawingMode dm /*= DM_LINES*/) 
	: Polygon(dm, CS_Scene)
{
}

BodyPolygon::~BodyPolygon()
{
}

void BodyPolygon::update()
{
	
}

}

