#ifndef TestListener_h__
#define TestListenerr_h__

#include "PhySketchInputListener.h"
#include "PhySketchLogger.h"
#include "PhySketchUtils.h"
#include "PhySketchVector2.h"
#include "PhySketchPhysicsManager.h"
#include "PhySketchPhysicsEventsListener.h"
#include "PhySketchPolygon.h"
#include "PhySketchMaterialManager.h"
#include "PhySketchRenderer.h"


class TestListener : public PhySketch::InputListener, public PhySketch::PhysicsEventsListener
{
public:
	TestListener() : PhySketch::InputListener()
	{
		_simulationPausedIndicator = nullptr;
		_simulationInternalPausedIndicator = nullptr;
		_simulationPaused = false;
		_simulationInternalPaused = false;
	}

	void keyDown(PhySketch::Key key) {}

	void keyUp(PhySketch::Key key) 
	{
		switch(key)
		{
		case PhySketch::KEY_Space:
			{
				_physicsMgr->toggleSimulation();
				break;
			}
		case PhySketch::KEY_S:
			{
				HWND hwnd = GetConsoleWindow();
				SetForegroundWindow(hwnd);

				std::string file;
				std::cout << "File to save? ";
				std::cin >> file;
				

				file = file + ".xml";
				_physicsMgr->saveToDisk(file);
				break;
			}
		case PhySketch::KEY_A:
			{
				HWND hwnd = GetConsoleWindow();
				SetForegroundWindow(hwnd);

				std::string file;
				std::cout << "File to load (add)? ";
				std::cin >> file;


				file = file + ".xml";
				_physicsMgr->loadFromDisk(file);
				break;
			}
		}
	}

	void mouseDown(PhySketch::MouseButton button, PhySketch::Vector2 position) {}

	void mouseUp(PhySketch::MouseButton button, PhySketch::Vector2 position) {}

	void mouseMoved(PhySketch::Vector2 position) {}

	virtual void initInputListener() 
	{
		PhySketch::Vector2 pauseIndicatorPos(7.0f, 3.5f);
		PhySketch::Vector2 pauseIndicatorScale(1.3f, 1.3f);
		_simulationPausedIndicator = new PhySketch::Polygon(PhySketch::VV_Static, "Tester::simulationPausedIndicator");
		_simulationPausedIndicator->CreateSquareSubPolygon(PhySketch::DM_TRIANGLE_FAN);
		_simulationPausedIndicator->setMaterial(PhySketch::MaterialManager::getSingletonPtr()->createMaterial("pauseMat", "../../../textures/pause.png", false, false));
		_simulationPausedIndicator->setVisible(false);
		_simulationPausedIndicator->setPosition(pauseIndicatorPos);
		_simulationPausedIndicator->setScale(pauseIndicatorScale);
		PhySketch::Renderer::getSingletonPtr()->addPolygon(_simulationPausedIndicator, PhySketch::RQT_UI);

		_simulationInternalPausedIndicator = new PhySketch::Polygon(PhySketch::VV_Static, "Tester::simulationInternalPausedIndicator");
		_simulationInternalPausedIndicator->CreateSquareSubPolygon(PhySketch::DM_TRIANGLE_FAN);
		_simulationInternalPausedIndicator->setMaterial(PhySketch::MaterialManager::getSingletonPtr()->createMaterial("pauseInternalMat", "../../../textures/pause_internal.png", false, false));
		_simulationInternalPausedIndicator->setVisible(false);
		_simulationInternalPausedIndicator->setPosition(pauseIndicatorPos);
		_simulationInternalPausedIndicator->setScale(pauseIndicatorScale);
		PhySketch::Renderer::getSingletonPtr()->addPolygon(_simulationInternalPausedIndicator, PhySketch::RQT_UI);
	}

	virtual void simulationStateChanged( bool paused ) 
	{
		_simulationPaused = paused;
		_simulationPausedIndicator->setVisible(paused);	
		if(_simulationInternalPaused)
		{
			if(paused)
			{				
				_simulationInternalPausedIndicator->setVisible(false);
			}
			else
			{
				_simulationInternalPausedIndicator->setVisible(true);
			}


		}
	}

	virtual void simulationInternalStateChanged( bool paused ) 
	{
		_simulationInternalPaused = paused;		
		if (paused && !_simulationPaused)
		{
			_simulationInternalPausedIndicator->setVisible(true);
		} 
		else
		{
			_simulationInternalPausedIndicator->setVisible(false);
		}
	}

	virtual void bodyCreated( PhySketch::PhysicsBody *body ) {}

	virtual void jointCreated( PhySketch::PhysicsJoint *joint ) {}

	PhySketch::PhysicsManager *_physicsMgr;

private:
	PhySketch::Polygon *_simulationPausedIndicator;
	PhySketch::Polygon *_simulationInternalPausedIndicator;

	bool _simulationPaused;
	bool _simulationInternalPaused;
};
#endif // TestListener_h__