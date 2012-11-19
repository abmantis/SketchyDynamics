#pragma once

#ifndef SketchyDynamicsPhysicsEventsListener_h__
#define SketchyDynamicsPhysicsEventsListener_h__

#include "SketchyDynamicsDefinitions.h"

namespace SketchyDynamics
{
	class PhysicsEventsListener
	{
	public:
		PhysicsEventsListener()  {};
		virtual ~PhysicsEventsListener() {};

		/// <summary> Called when the simulation state changes, i.e. it is paused or resumed. </summary>
		/// <param name="paused"> true if paused. </param>
		virtual void simulationStateChanged(bool paused) = 0;

		/// <summary> Called when the simulation state changes, but by an internal mechanism (such as object selection, guesses list waiting for input, etc). </summary>
		/// <param name="paused"> true if paused. </param>
		virtual void simulationInternalStateChanged(bool paused) = 0;

		/// <summary> Called when a body is created. </summary>
		/// <param name="body"> The new body. </param>
		virtual void bodyCreated(PhysicsBody *body) = 0;

		/// <summary> Called when a joint is created. </summary>
		/// <param name="body"> The new joint. </param>
		virtual void jointCreated(PhysicsJoint *joint) = 0;
	};
}

#endif // SketchyDynamicsPhysicsEventsListener_h__