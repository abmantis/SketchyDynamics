#ifndef TestListener_h__
#define TestListenerr_h__

#include "SketchyDynamicsInputListener.h"
#include "SketchyDynamicsLogger.h"
#include "SketchyDynamicsUtils.h"
#include "SketchyDynamicsVector2.h"
#include "SketchyDynamicsPhysicsManager.h"
#include "SketchyDynamicsPhysicsEventsListener.h"
#include "SketchyDynamicsPolygon.h"
#include "SketchyDynamicsMaterialManager.h"
#include "SketchyDynamicsRenderer.h"


class TestListener : public SketchyDynamics::InputListener, public SketchyDynamics::PhysicsEventsListener
{
public:
	TestListener() : SketchyDynamics::InputListener()
	{
		_simulationPausedIndicator = nullptr;
		_simulationInternalPausedIndicator = nullptr;
		_simulationPaused = false;
		_simulationInternalPaused = false;
	}

	void keyDown(SketchyDynamics::Key key) {}

	void keyUp(SketchyDynamics::Key key) 
	{
		switch(key)
		{
		case SketchyDynamics::KEY_Space:
			{
				_physicsMgr->toggleSimulation();
				break;
			}
		case SketchyDynamics::KEY_S:
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
		case SketchyDynamics::KEY_A:
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

	void mouseDown(SketchyDynamics::MouseButton button, SketchyDynamics::Vector2 position) {}

	void mouseUp(SketchyDynamics::MouseButton button, SketchyDynamics::Vector2 position) {}

	void mouseMoved(SketchyDynamics::Vector2 position) {}

	virtual void initInputListener() 
	{
		SketchyDynamics::Vector2 pauseIndicatorPos(7.0f, 3.5f);
		SketchyDynamics::Vector2 pauseIndicatorScale(1.3f, 1.3f);
		_simulationPausedIndicator = new SketchyDynamics::Polygon(SketchyDynamics::VV_Static, "Tester::simulationPausedIndicator");
		_simulationPausedIndicator->CreateSquareSubPolygon(SketchyDynamics::DM_TRIANGLE_FAN);
		_simulationPausedIndicator->setMaterial(SketchyDynamics::MaterialManager::getSingletonPtr()->createMaterial("pauseMat", "../../../textures/pause.png", false, false));
		_simulationPausedIndicator->setVisible(false);
		_simulationPausedIndicator->setPosition(pauseIndicatorPos);
		_simulationPausedIndicator->setScale(pauseIndicatorScale);
		SketchyDynamics::Renderer::getSingletonPtr()->addPolygon(_simulationPausedIndicator, SketchyDynamics::RQT_UI);

		_simulationInternalPausedIndicator = new SketchyDynamics::Polygon(SketchyDynamics::VV_Static, "Tester::simulationInternalPausedIndicator");
		_simulationInternalPausedIndicator->CreateSquareSubPolygon(SketchyDynamics::DM_TRIANGLE_FAN);
		_simulationInternalPausedIndicator->setMaterial(SketchyDynamics::MaterialManager::getSingletonPtr()->createMaterial("pauseInternalMat", "../../../textures/pause_internal.png", false, false));
		_simulationInternalPausedIndicator->setVisible(false);
		_simulationInternalPausedIndicator->setPosition(pauseIndicatorPos);
		_simulationInternalPausedIndicator->setScale(pauseIndicatorScale);
		SketchyDynamics::Renderer::getSingletonPtr()->addPolygon(_simulationInternalPausedIndicator, SketchyDynamics::RQT_UI);
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

	virtual void bodyCreated( SketchyDynamics::PhysicsBody *body ) {}

	virtual void jointCreated( SketchyDynamics::PhysicsJoint *joint ) {}

	SketchyDynamics::PhysicsManager *_physicsMgr;

private:
	SketchyDynamics::Polygon *_simulationPausedIndicator;
	SketchyDynamics::Polygon *_simulationInternalPausedIndicator;

	bool _simulationPaused;
	bool _simulationInternalPaused;
};
#endif // TestListener_h__