#pragma once
#ifndef SketchyDynamicsGuessesList_h__
#define SketchyDynamicsGuessesList_h__

#include "SketchyDynamicsDefinitions.h"
#include "SketchyDynamicsVector2.h"

namespace SketchyDynamics
{
	enum GuessesListGuesses
	{
		GLG_INVALID = 0,
		GLG_RECTANGLE,
		GLG_TRIANGLE,
		GLG_CIRCLE,
		GLG_FREEFORM,
		GLG_WELD,
		GLG_REVOLUTE,
		GLG_ROPE,
		GLG_SPRING,
		GLG_CANCEL,
	};

	class GuessesList
	{
	public:
		GuessesList();
		virtual ~GuessesList();

		virtual void showGuess(GuessesListGuesses guess, bool highlight);
		virtual void hideGuessesList();
		virtual GuessesListGuesses getGuessType(Polygon *guessPoly);
		virtual void mouseHoverGuess(Polygon *guessPoly);
		virtual inline void stopMouseHover();

	protected:
		virtual SubPolygon* setupGuessPolygon(Polygon *guessPoly, int textureOrder);

	protected:
		Material *_mainMaterial;
		Material *_hoverMaterial;

		AnimatedPolygon *_rectangleGuess;
		AnimatedPolygon *_triangleGuess;
		AnimatedPolygon *_circleGuess;
		AnimatedPolygon *_freeformGuess;
		AnimatedPolygon *_weldGuess;
		AnimatedPolygon *_revoluteGuess;
		AnimatedPolygon *_ropeGuess;
		AnimatedPolygon *_springGuess;
		AnimatedPolygon *_cancelGuess;
		bool _rectangleGuessVisible;
		bool _triangleGuessVisible;
		bool _circleGuessVisible;
		bool _freeformGuessVisible;
		bool _weldGuessVisible;
		bool _revoluteGuessVisible;
		bool _ropeGuessVisible;
		bool _springGuessVisible;
		bool _cancelGuessVisible;

		Vector2 _basePosition;
		float _initialTranslation;
		float _lastGuessPosition;

		Polygon *_hoveredGuess;

	};
}
#endif // SketchyDynamicsGuessesList_h__
