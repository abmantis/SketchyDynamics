// Tester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// Memory leak debug
#ifndef NDEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
#define new DEBUG_CLIENTBLOCK
#endif


#include "PhySketchCore.h"
#include "PhySketchApplicationWindow.h"
#include "TestInputListener.h"
#include "PhySketchRenderer.h"
#include "PhySketchPhysicsManager.h"
#include "PhySketchPhysicsBody.h"
#include "PhySketchPolygon.h"
#include "PhySketchMaterialManager.h"

PhySketch::Core _core;
PhySketch::ApplicationWindow *_window;
PhySketch::Renderer *_renderer;
PhySketch::PhysicsManager *_physicsMgr;
PhySketch::MaterialManager *_materialMgr;
TestInputListener *_inputListener;

int _tmain(int argc, _TCHAR* argv[])
{	
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	PhySketch::Vector2 worldSize(80.0f, 80.0f);

	_core.initialise("Log.txt", true, PhySketch::Vector2(0, -10), worldSize);
	_window = _core.createWindow("test", PhySketch::Vector2::ZERO_XY, PhySketch::Vector2(1280, 720), false);
	_renderer = PhySketch::Renderer::getSingletonPtr();
	_physicsMgr = PhySketch::PhysicsManager::getSingletonPtr();
	_materialMgr = PhySketch::MaterialManager::getSingletonPtr();
		
	_inputListener = new TestInputListener();
	_inputListener->_physicsMgr = _physicsMgr;
	_window->addInputListener(_inputListener);

	{
		PhySketch::Material* backgroundMat = _materialMgr->createMaterial("backgroundMaterial", PhySketch::Color(1.0f,1.0f,1.0f,1.0f));
		b2BodyDef backgroundbodyDef;
		backgroundbodyDef.position.Set(0.0f, 0.0f);
		PhySketch::PhysicsBody *backgroundPhyBody = _physicsMgr->createBody(backgroundbodyDef);

		b2PolygonShape backgroundBox;
		backgroundBox.SetAsBox(worldSize.x*0.5f,worldSize.y*0.5f);
		b2FixtureDef backgroundFixtureDef;
		backgroundFixtureDef.shape = &backgroundBox;
		backgroundFixtureDef.filter.categoryBits = 0x0;
		backgroundPhyBody->getBox2DBody()->CreateFixture(&backgroundFixtureDef);
		backgroundPhyBody->setFillMaterial(backgroundMat);
		backgroundPhyBody->setLineMaterial(backgroundMat);
		backgroundPhyBody->reconstructPolygon();		
		_physicsMgr->setUnselectableBody(backgroundPhyBody);
	}
	
		
	{
		b2BodyDef bodyDef;
		bodyDef.position.Set(0.0f, -4.0f);
		b2Body *body = _physicsMgr->getPhysicsWorld()->CreateBody(&bodyDef);

		b2PolygonShape groundBox;
		groundBox.SetAsBox(7.0f, 0.3f);
		body->CreateFixture(&groundBox, 0.0f);
		PhySketch::PhysicsBody *phyBody = _physicsMgr->createBody(body);
		_physicsMgr->setUnselectableBody(phyBody);
	}
	
	
	_core.startLoop();

	delete _inputListener; _inputListener = 0;

	return 0;
}

