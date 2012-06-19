// Tester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "PhySketchCore.h"
#include "PhySketchApplicationWindow.h"
#include "TestInputListener.h"
#include "PhySketchRenderer.h"
#include "PhySketchPhysicsManager.h"
#include "PhySketchPhysicsBody.h"
#include "PhySketchPolygon.h"

PhySketch::Core _core;
PhySketch::ApplicationWindow *_window;
PhySketch::Renderer *_renderer;
PhySketch::PhysicsManager *_physicsMgr;
TestInputListener *_inputListener;

int _tmain(int argc, _TCHAR* argv[])
{	
	_core.initialise("Log.txt", true, PhySketch::Vector2(0, -10));
	_window = _core.createWindow("test", PhySketch::Vector2::ZERO, PhySketch::Vector2(1280, 720), false);
	_renderer = PhySketch::Renderer::getSingletonPtr();
	_physicsMgr = PhySketch::PhysicsManager::getSingletonPtr();
		
	_inputListener = new TestInputListener();
	_inputListener->_physicsMgr = _physicsMgr;
	_window->addInputListener(_inputListener);

	// create white background
	PhySketch::Polygon *backgroundPoly = PhySketch::Polygon::CreateSquare(PhySketch::Polygon::CS_Scene);
	backgroundPoly->setScale(_renderer->getSceneViewAxisMin() - _renderer->getSceneViewAxisMax());
	PhySketch::Material mat;
	mat.setColor(PhySketch::Color(1.0f, 1.0f, 1.0f, 0.0f));
	backgroundPoly->SetMaterial(mat);
	_renderer->addPolygon(backgroundPoly);
	
		
	b2BodyDef bodyDef;
	bodyDef.position.Set(0.0f, -4.0f);
	b2Body *body = _physicsMgr->getPhysicsWorld()->CreateBody(&bodyDef);

	b2PolygonShape groundBox;
	groundBox.SetAsBox(7.0f, 0.3f);
	body->CreateFixture(&groundBox, 0.0f);
	PhySketch::PhysicsBody *phyBody = new PhySketch::PhysicsBody(body);
	_physicsMgr->AddBody(phyBody);
	
	
	_core.startLoop();

	delete _inputListener; _inputListener = 0;

	return 0;
}

