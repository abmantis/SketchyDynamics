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
#include "TestListener.h"
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
TestListener *_testListener;

int _tmain(int argc, _TCHAR* argv[])
{	
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	
	PhySketch::Vector2 worldSize(100.0f,100.0f);

	_core.initialise("Log.txt", true, PhySketch::Vector2(0, -10), worldSize);
	_window = _core.createWindow("test", PhySketch::Vector2::ZERO_XY, PhySketch::Vector2(854, 480), false);
	_window->maximized(true);
	_renderer = PhySketch::Renderer::getSingletonPtr();
	_physicsMgr = PhySketch::PhysicsManager::getSingletonPtr();
	_matMgr = PhySketch::MaterialManager::getSingletonPtr();
	
	_testListener = new TestListener();
	_testListener->_physicsMgr = _physicsMgr;
	_physicsMgr->addEventListener(_testListener);
	_window->addInputListener(_testListener);

	PhySketch::MainInputListener::getSingletonPtr()->enableGuessesList(true);

	// Create background polygon (the background body is bigger than the screen and would stretch the texture)
	PhySketch::Material* backgroundMat = _matMgr->createMaterial("backgroundMat", "../../../textures/fzm-notebook.texture-03.jpg");	
	PhySketch::Polygon* backPoly = new PhySketch::Polygon(PhySketch::VV_Static, "background");
	backPoly->CreateSquareSubPolygon(PhySketch::DM_TRIANGLE_FAN);
	backPoly->setScale(_renderer->getSceneViewAxisMax() - _renderer->getSceneViewAxisMin());
	backPoly->setMaterial(backgroundMat);	
	_renderer->addPolygon(backPoly, 0);
		
	PhySketch::Material* transparentMat = _matMgr->createMaterial("transparentMat", PhySketch::Color(0.0f, 0.0f, 0.0f, 0.0f));
	{
		// Create transparent background body		
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
		backgroundPhyBody->setSaveToDisk(false);
	}
	
		
	{
		PhySketch::Material* rullerMat = _matMgr->createMaterial("rullerMat", "../../../textures/ruler.png", false, false);	
		//PhySketch::Material* rullerBorderMat = _matMgr->createMaterial("rullerBorderMat", PhySketch::Color(1.0f, 0.95f, 0.0f, 1.0f));	

		// Create "ground" body
		b2BodyDef bodyDef;
		bodyDef.position.Set(0.0f, -4.0f);
		b2Body *body = _physicsMgr->getPhysicsWorld()->CreateBody(&bodyDef);

		b2PolygonShape groundBox;
		groundBox.SetAsBox(7.0f, 0.4f);
		body->CreateFixture(&groundBox, 0.0f);
		PhySketch::PhysicsBody *groundPhyBody = _physicsMgr->createBody(body);
		//groundPhyBody->setFillMaterial(rullerMat);
		//groundPhyBody->setLineMaterial(rullerBorderMat);
		groundPhyBody->setFillMaterial(transparentMat);
		groundPhyBody->setLineMaterial(transparentMat);
		groundPhyBody->reconstructPolygon();
		groundPhyBody->recomputeTexCoordsToFit();
		_physicsMgr->setUnselectableBody(groundPhyBody);
		groundPhyBody->setSaveToDisk(false);

		// Create ground polygon (the background body would not work good with the shadow)
		PhySketch::Polygon* grndPoly = new PhySketch::Polygon(PhySketch::VV_Static, "groundPoly");
		grndPoly->CreateSquareSubPolygon(PhySketch::DM_TRIANGLE_FAN);
		grndPoly->setScale(PhySketch::Vector2(14.1f, 0.8f));
		grndPoly->setPosition(PhySketch::Vector2(-0.05f, -4.0f));
		grndPoly->setMaterial(rullerMat);	
		_renderer->addPolygon(grndPoly, 0);
	}
	
	PhySketch::Material* polyFill = _matMgr->createMaterial("polyFill", "../../../textures/whiteSmoke.jpg");
	polyFill->setColor(PhySketch::Color(0.7f, 0.7f, 0.8f, 0.6f));
	_physicsMgr->setDefaultBodyFillMat(polyFill);
	PhySketch::Material* polyLine = _matMgr->createMaterial("polyLine", "../../../textures/laser_blue.png");
	_physicsMgr->setDefaultBodyLineMat(polyLine);
	

	_core.startLoop();
	_renderer->removePolygon(backPoly);
	delete backPoly; backPoly = nullptr;
	delete _testListener; _testListener = 0;

	return 0;
}

