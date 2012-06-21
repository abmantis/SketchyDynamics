#pragma once
#ifndef PhySketchPhysicsQueryCallback_h__
#define PhySketchPhysicsQueryCallback_h__

#include "PhySketchVector2.h"
#include "PhySketchPhysicsBody.h"
#include "Box2D\Dynamics\b2Fixture.h"

namespace PhySketch
{
	class PhysicsQueryCallback : public b2QueryCallback
	{
	public:
		PhysicsQueryCallback(const Vector2& point, bool orderByID) :
		  _orderByID(orderByID)
		{
			_point = point.tob2Vec2();
		}

		bool ReportFixture(b2Fixture* fixture)
		{			
			bool inside = fixture->TestPoint(_point);
			if (inside)
			{
				PhysicsBody* body = static_cast<PhysicsBody*>(fixture->GetBody()->GetUserData());

				if(_orderByID)
				{
					ulong bodyId = body->getId();
					ulong currID;

					std::list<PhysicsBody*>::iterator it = _bodies.begin();
					std::list<PhysicsBody*>::iterator it_end = _bodies.end();
					// find where to insert the body
					for ( ; it != it_end; ++it)
					{
						currID = (*it)->getId();
						if(bodyId == currID)
						{
							// this body is already on the list							
							return true; // Continue the query.
						}
						if(bodyId < currID)
						{
							break;
						}
					}
					_bodies.insert(it, body);	
					return true; // Continue the query.
				}
				else
				{
					_bodies.push_back(body);
					return true; // Continue the query.
				}
			}
			return true; // Continue the query.
		}

		const bool _orderByID;
		b2Vec2 _point;
		std::list<PhysicsBody*> _bodies;
	};
}
#endif // PhySketchPhysicsQueryCallback_h__
