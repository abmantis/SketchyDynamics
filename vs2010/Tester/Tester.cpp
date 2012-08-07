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
PhySketch::MaterialManager *_matMgr;
TestInputListener *_inputListener;

int _tmain(int argc, _TCHAR* argv[])
{	
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	PhySketch::Vector2 worldSize(100.0f,100.0f);

	_core.initialise("Log.txt", true, PhySketch::Vector2(0, -10), worldSize);
	_window = _core.createWindow("test", PhySketch::Vector2::ZERO_XY, PhySketch::Vector2(1280, 720), false);
	_renderer = PhySketch::Renderer::getSingletonPtr();
	_physicsMgr = PhySketch::PhysicsManager::getSingletonPtr();
	_matMgr = PhySketch::MaterialManager::getSingletonPtr();
		
	_inputListener = new TestInputListener();
	_inputListener->_physicsMgr = _physicsMgr;
	_window->addInputListener(_inputListener);

	PhySketch::MainInputListener::getSingletonPtr()->enableGuessesList(true);

	//PhySketch::Material* backgroundMat = _matMgr->createMaterial("backgroundMat", PhySketch::Color(1.0f,1.0f,1.0f,1.0f));
	PhySketch::Material* backgroundMat = _matMgr->createMaterial("backgroundMat", "../../../textures/fzm-notebook.texture-03.jpg");

	PhySketch::Polygon* backPoly = new PhySketch::Polygon(PhySketch::VV_Static, "background");
	backPoly->CreateSquareSubPolygon(PhySketch::DM_TRIANGLE_FAN);
	backPoly->setScale(_renderer->getSceneViewAxisMax() - _renderer->getSceneViewAxisMin());
	backPoly->setMaterial(backgroundMat);	
	_renderer->addPolygon(backPoly, 0);
	

	{
		PhySketch::Material* transparentMat = _matMgr->createMaterial("transparentMat", PhySketch::Color(0.0f, 0.0f, 0.0f, 0.0f));
		b2BodyDef backgroundbodyDef;
		backgroundbodyDef.position.Set(0.0f, 0.0f);
		PhySketch::PhysicsBody *backgroundPhyBody = _physicsMgr->createBody(backgroundbodyDef);

		b2PolygonShape backgroundBox;
		backgroundBox.SetAsBox(worldSize.x*0.5f, worldSize.y*0.5f);
		b2FixtureDef backgroundFixtureDef;
		backgroundFixtureDef.shape = &backgroundBox;
		backgroundFixtureDef.filter.categoryBits = 0x0;
		backgroundPhyBody->getBox2DBody()->CreateFixture(&backgroundFixtureDef);
 		backgroundPhyBody->setFillMaterial(transparentMat);
 		backgroundPhyBody->setLineMaterial(transparentMat);
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
	
	PhySketch::Material* polyFill = _matMgr->createMaterial("polyFill", "../../../textures/whiteSmoke.jpg");
	polyFill->setColor(PhySketch::Color(0.7f, 0.7f, 0.8f, 0.6f));
	_physicsMgr->setDefaultBodyFillMat(polyFill);
	PhySketch::Material* polyLine = _matMgr->createMaterial("polyLine", "../../../textures/laser_w.png");
	_physicsMgr->setDefaultBodyLineMat(polyLine);
	

	_core.startLoop();
	_renderer->removePolygon(backPoly);
	delete backPoly; backPoly = nullptr;
	delete _inputListener; _inputListener = 0;

	return 0;
}

