#include "PhySketchGuessesList.h"
#include "PhySketchPolygon.h"
#include "PhySketchMaterialManager.h"
#include "PhySketchRenderer.h"

namespace PhySketch
{
	GuessesList::GuessesList()
	{		
		_basePosition = Vector2(-9.8f, 4.0f);

		Renderer *renderer = Renderer::getSingletonPtr();
		_mainMaterial = MaterialManager::getSingletonPtr()->createMaterial("PS_guesseslist", "../../../textures/GuessesList.png", false, false);
		_hoverMaterial = MaterialManager::getSingletonPtr()->createMaterial("PS_guesseslist_hover", "../../../textures/GuessesList_hover.png", false, false);

		_initialTranslation = 2.0f;
		_lastGuessPosition = _initialTranslation;

		_rectangleGuess	= new AnimatedPolygon(VV_Static, "PS_guesseslist_rectangleGuess");		
		_triangleGuess	= new AnimatedPolygon(VV_Static, "PS_guesseslist_triangleGuess");
		_circleGuess	= new AnimatedPolygon(VV_Static, "PS_guesseslist_circleGuess");
		_weldGuess		= new AnimatedPolygon(VV_Static, "PS_guesseslist_weldGuess");
		_revoluteGuess	= new AnimatedPolygon(VV_Static, "PS_guesseslist_revoluteGuess");
		_ropeGuess		= new AnimatedPolygon(VV_Static, "PS_guesseslist_ropeGuess");
		_springGuess	= new AnimatedPolygon(VV_Static, "PS_guesseslist_springGuess");
		_freeformGuess	= new AnimatedPolygon(VV_Static, "PS_guesseslist_freeformGuess");
		_cancelGuess	= new AnimatedPolygon(VV_Static, "PS_guesseslist_cancelGuess");

		setupGuessPolygon(_rectangleGuess,	0);
		setupGuessPolygon(_triangleGuess,	1);
		setupGuessPolygon(_circleGuess,		2);
		setupGuessPolygon(_weldGuess,		3);
		setupGuessPolygon(_revoluteGuess,	4);
		setupGuessPolygon(_ropeGuess,		5);
		setupGuessPolygon(_springGuess,		6);
		setupGuessPolygon(_freeformGuess,	7);
		setupGuessPolygon(_cancelGuess,		8);

		_rectangleGuess	->setMaterial(_mainMaterial);
		_triangleGuess	->setMaterial(_mainMaterial);
		_circleGuess	->setMaterial(_mainMaterial);
		_weldGuess		->setMaterial(_mainMaterial);
		_revoluteGuess	->setMaterial(_mainMaterial);
		_ropeGuess		->setMaterial(_mainMaterial);
		_springGuess	->setMaterial(_mainMaterial);
		_freeformGuess	->setMaterial(_mainMaterial);
		_cancelGuess	->setMaterial(_mainMaterial);

		_rectangleGuess	->setUserType(PHYSKETCH_POLYGON_UTYPE_GUESSESLIST);
		_triangleGuess	->setUserType(PHYSKETCH_POLYGON_UTYPE_GUESSESLIST);
		_circleGuess	->setUserType(PHYSKETCH_POLYGON_UTYPE_GUESSESLIST);
		_weldGuess		->setUserType(PHYSKETCH_POLYGON_UTYPE_GUESSESLIST);
		_revoluteGuess	->setUserType(PHYSKETCH_POLYGON_UTYPE_GUESSESLIST);
		_ropeGuess		->setUserType(PHYSKETCH_POLYGON_UTYPE_GUESSESLIST);
		_springGuess	->setUserType(PHYSKETCH_POLYGON_UTYPE_GUESSESLIST);
		_freeformGuess	->setUserType(PHYSKETCH_POLYGON_UTYPE_GUESSESLIST);
		_cancelGuess	->setUserType(PHYSKETCH_POLYGON_UTYPE_GUESSESLIST);

		_rectangleGuess	->Polygon::setPosition(_basePosition);
		_triangleGuess	->Polygon::setPosition(_basePosition);
		_circleGuess	->Polygon::setPosition(_basePosition);
		_weldGuess		->Polygon::setPosition(_basePosition);
		_revoluteGuess	->Polygon::setPosition(_basePosition);
		_ropeGuess		->Polygon::setPosition(_basePosition);
		_springGuess	->Polygon::setPosition(_basePosition);
		_freeformGuess	->Polygon::setPosition(_basePosition);
		_cancelGuess	->Polygon::setPosition(_basePosition);

		renderer->addPolygon(_rectangleGuess, RQT_UI);
		renderer->addPolygon(_triangleGuess	, RQT_UI);
		renderer->addPolygon(_circleGuess	, RQT_UI);
		renderer->addPolygon(_weldGuess		, RQT_UI);
		renderer->addPolygon(_revoluteGuess	, RQT_UI);
		renderer->addPolygon(_ropeGuess		, RQT_UI);
		renderer->addPolygon(_springGuess	, RQT_UI);
		renderer->addPolygon(_freeformGuess	, RQT_UI);
		renderer->addPolygon(_cancelGuess	, RQT_UI);

		_rectangleGuessVisible	= false;
		_triangleGuessVisible	= false;
		_circleGuessVisible		= false;
		_freeformGuessVisible	= false;
		_weldGuessVisible		= false;
		_revoluteGuessVisible	= false;
		_ropeGuessVisible		= false;
		_springGuessVisible		= false;
		_cancelGuessVisible		= false;

		_hoveredGuess = nullptr;
				
	}

	GuessesList::~GuessesList()
	{
		Renderer *renderer = Renderer::getSingletonPtr();
		renderer->removePolygon(_rectangleGuess , RQT_UI);
		renderer->removePolygon(_triangleGuess	, RQT_UI);
		renderer->removePolygon(_circleGuess	, RQT_UI);
		renderer->removePolygon(_weldGuess		, RQT_UI);
		renderer->removePolygon(_revoluteGuess	, RQT_UI);
		renderer->removePolygon(_ropeGuess		, RQT_UI);
		renderer->removePolygon(_springGuess	, RQT_UI);
		renderer->removePolygon(_freeformGuess	, RQT_UI);
		renderer->removePolygon(_cancelGuess	, RQT_UI);

		delete _rectangleGuess;
		delete _triangleGuess;
		delete _circleGuess;
		delete _weldGuess;	
		delete _revoluteGuess;
		delete _ropeGuess;
		delete _springGuess;
		delete _freeformGuess;
		delete _cancelGuess;

		_rectangleGuess	= nullptr;
		_triangleGuess	= nullptr;
		_circleGuess	= nullptr;
		_weldGuess		= nullptr;
		_revoluteGuess	= nullptr;
		_ropeGuess		= nullptr;
		_springGuess	= nullptr;
		_freeformGuess	= nullptr;
		_cancelGuess	= nullptr;

		_hoveredGuess = nullptr;

	}

	SubPolygon* GuessesList::setupGuessPolygon( Polygon *guessPoly, int textureOrder )
	{
		float texLeftCoord	= ((float)textureOrder) / 9.0f;
		float texRightCoord	= ((float)textureOrder+1) / 9.0f;
		SubPolygon *subPoly	= guessPoly->createSubPolygon(DM_TRIANGLE_FAN);
		subPoly->addVertex(Vector2(0.0f, 0.0f), Vector2(texLeftCoord,  1.0f));
		subPoly->addVertex(Vector2(0.0f,-1.0f), Vector2(texLeftCoord,  0.0f));
		subPoly->addVertex(Vector2(1.0f,-1.0f), Vector2(texRightCoord, 0.0f));
		subPoly->addVertex(Vector2(1.0f, 0.0f), Vector2(texRightCoord, 1.0f));

		return subPoly;
	}

	void GuessesList::showGuess( GuessesListGuesses guess, bool highlight )
	{
		AnimatedPolygon *guessPoly_ptr = nullptr;
		bool *guessVisibilityFlag_ptr = nullptr;

		switch(guess)
		{
		case GLG_RECTANGLE:
			{
				guessPoly_ptr = _rectangleGuess;
				guessVisibilityFlag_ptr = &_rectangleGuessVisible;
				break;
			}
		case GLG_TRIANGLE:
			{
				guessPoly_ptr = _triangleGuess;
				guessVisibilityFlag_ptr = &_triangleGuessVisible;
				break;
			}
		case GLG_CIRCLE:
			{
				guessPoly_ptr = _circleGuess;
				guessVisibilityFlag_ptr = &_circleGuessVisible;
				break;
			}
		case GLG_FREEFORM:
			{
				guessPoly_ptr = _freeformGuess;
				guessVisibilityFlag_ptr = &_freeformGuessVisible;
				break;
			}
		case GLG_WELD:
			{
				guessPoly_ptr = _weldGuess;
				guessVisibilityFlag_ptr = &_weldGuessVisible;
				break;
			}
		case GLG_REVOLUTE:
			{
				guessPoly_ptr = _revoluteGuess;
				guessVisibilityFlag_ptr = &_revoluteGuessVisible;
				break;
			}
		case GLG_ROPE:
			{
				guessPoly_ptr = _ropeGuess;
				guessVisibilityFlag_ptr = &_ropeGuessVisible;
				break;
			}
		case GLG_SPRING:
			{
				guessPoly_ptr = _springGuess;
				guessVisibilityFlag_ptr = &_springGuessVisible;
				break;
			}
		case GLG_CANCEL:
			{
				guessPoly_ptr = _cancelGuess;
				guessVisibilityFlag_ptr = &_cancelGuessVisible;
				break;
			}
		}

		if((*guessVisibilityFlag_ptr) == false)
		{
			(*guessVisibilityFlag_ptr) = true;
			
			float translationUnitsPerSec = 20.0f;
			float scale;
			if(highlight)
			{
				scale = 1.8f;
			}
			else
			{
				scale = 1.2f;
			}

			guessPoly_ptr->setScale(Vector2(scale, scale));
			guessPoly_ptr->Polygon::setPosition(_basePosition);
			guessPoly_ptr->translate(Vector2(_lastGuessPosition, 0.0f), translationUnitsPerSec);
			_lastGuessPosition += scale;
		}
	}

	void GuessesList::hideGuessesList()
	{
		_lastGuessPosition = _initialTranslation;
		float translationUnitsPerSec = 20.0f;

		_rectangleGuess	->setPosition(_basePosition, translationUnitsPerSec);
		_triangleGuess	->setPosition(_basePosition, translationUnitsPerSec);
		_circleGuess	->setPosition(_basePosition, translationUnitsPerSec);
		_weldGuess		->setPosition(_basePosition, translationUnitsPerSec);
		_revoluteGuess	->setPosition(_basePosition, translationUnitsPerSec);
		_ropeGuess		->setPosition(_basePosition, translationUnitsPerSec);
		_springGuess	->setPosition(_basePosition, translationUnitsPerSec);
		_freeformGuess	->setPosition(_basePosition, translationUnitsPerSec);
		_cancelGuess	->setPosition(_basePosition, translationUnitsPerSec);


		_rectangleGuessVisible	= false;
		_triangleGuessVisible	= false;
		_circleGuessVisible		= false;
		_freeformGuessVisible	= false;
		_weldGuessVisible		= false;
		_revoluteGuessVisible	= false;
		_ropeGuessVisible		= false;
		_springGuessVisible		= false;
		_cancelGuessVisible		= false;
	}

	PhySketch::GuessesListGuesses GuessesList::getGuessType( Polygon *guessPoly )
	{
		if(guessPoly == _rectangleGuess)
		{
			return GLG_RECTANGLE ;
		}
		else if(guessPoly == _triangleGuess)
		{
			return GLG_TRIANGLE ;
		}
		else if(guessPoly == _circleGuess)
		{
			return GLG_CIRCLE ;
		}
		else if(guessPoly == _freeformGuess)
		{
			return GLG_FREEFORM ;
		}
		else if(guessPoly == _weldGuess)
		{
			return GLG_WELD ;
		}
		else if(guessPoly == _revoluteGuess)
		{
			return GLG_REVOLUTE ;
		}
		else if(guessPoly == _ropeGuess)
		{
			return GLG_ROPE ;
		}
		else if(guessPoly == _springGuess)
		{
			return GLG_SPRING ;
		}
		else if(guessPoly == _cancelGuess)
		{
			return GLG_CANCEL ;
		}

		return GLG_INVALID;
	}

	void GuessesList::mouseHoverGuess( Polygon *guessPoly )
	{
		Polygon *previousHoveredGuess = _hoveredGuess;

		if(guessPoly == _rectangleGuess)
		{
			_hoveredGuess = _rectangleGuess;
		}
		else if(guessPoly == _triangleGuess)
		{
			_hoveredGuess = _triangleGuess;
		}
		else if(guessPoly == _circleGuess)
		{
			_hoveredGuess = _circleGuess;
		}
		else if(guessPoly == _freeformGuess)
		{
			_hoveredGuess = _freeformGuess;
		}
		else if(guessPoly == _weldGuess)
		{
			_hoveredGuess = _weldGuess;
		}
		else if(guessPoly == _revoluteGuess)
		{
			_hoveredGuess = _revoluteGuess;
		}
		else if(guessPoly == _ropeGuess)
		{
			_hoveredGuess = _ropeGuess;
		}
		else if(guessPoly == _springGuess)
		{
			_hoveredGuess = _springGuess;
		}
		else if(guessPoly == _cancelGuess)
		{
			_hoveredGuess = _cancelGuess;
		}
		else
		{
			_hoveredGuess->setMaterial(_mainMaterial);
			_hoveredGuess = nullptr;
			return;
		}

		if(previousHoveredGuess != _hoveredGuess)
		{
			if(previousHoveredGuess)
			{
				previousHoveredGuess->setMaterial(_mainMaterial);
			}
			_hoveredGuess->setMaterial(_hoverMaterial);
		}
		
	}

	void GuessesList::stopMouseHover()
	{
		if(_hoveredGuess)
		{
			_hoveredGuess->setMaterial(_mainMaterial);
			_hoveredGuess = nullptr;
		}
	}

		

} // namespace PhySketch