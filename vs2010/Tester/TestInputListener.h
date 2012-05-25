#ifndef TestInputListener_h__
#define TestInputListener_h__

#include "PhySketchInputListener.h"
#include "PhySketchLogger.h"
#include "PhySketchUtils.h"

class TestInputListener : public PhySketch::InputListener
{
public:
	TestInputListener() : PhySketch::InputListener()
	{
	}

	void keyDown(PhySketch::Key key) {}
	void keyUp(PhySketch::Key key) {}

	void mouseDown(PhySketch::MouseButton button, int x, int y)
	{

	}
	void mouseUp(PhySketch::MouseButton button, int x, int y) 
	{

	}
	void mouseMoved(int x, int y) 
	{

	}
};
#endif // TestInputListener_h__