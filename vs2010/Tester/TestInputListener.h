#ifndef TestInputListener_h__
#define TestInputListener_h__

#include "PhySketchInputListener.h"
#include "PhySketchLogger.h"
#include "PhySketchUtils.h"
#include "PhySketchVector2.h"

class TestInputListener : public PhySketch::InputListener
{
public:
	TestInputListener() : PhySketch::InputListener()
	{
	}

	void keyDown(PhySketch::Key key) {}
	void keyUp(PhySketch::Key key) {}

	void mouseDown(PhySketch::MouseButton button, PhySketch::Vector2 position)
	{

	}
	void mouseUp(PhySketch::MouseButton button, PhySketch::Vector2 position) 
	{

	}
	void mouseMoved(PhySketch::Vector2 position) 
	{

	}
};
#endif // TestInputListener_h__