// Tester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "PhySketchCore.h"
#include "PhySketchApplicationWindow.h"
#include "TestInputListener.h"
#include "PhySketchRenderer.h"
#include "PhySketchBodyPolygon.h"

PhySketch::Core _core;
PhySketch::ApplicationWindow *_window;
PhySketch::Renderer *_renderer;
TestInputListener *_inputListener;


int _tmain(int argc, _TCHAR* argv[])
{	
	_core.initialise("Log.txt", true, PhySketch::Vector2(0, -10));
	_window = _core.createWindow("test", 1280, 720, false);
	_renderer = PhySketch::Renderer::getSingletonPtr();
		
	_inputListener = new TestInputListener();
	_window->addInputListener(_inputListener);
		
	b2BodyDef bodyDef;
	bodyDef.position.Set(0.0f, -4.0f);
	b2Body *body = _core.getPhysicsWorld()->CreateBody(&bodyDef);

	b2PolygonShape groundBox;
	groundBox.SetAsBox(7.0f, 0.3f);
	body->CreateFixture(&groundBox, 0.0f);
	PhySketch::BodyPolygon*p = new PhySketch::BodyPolygon(body, PhySketch::Polygon::DM_LINE_LOOP);
	_renderer->addPolygon(p);
	
	_core.startLoop();

	delete _inputListener; _inputListener = 0;

	return 0;
}

