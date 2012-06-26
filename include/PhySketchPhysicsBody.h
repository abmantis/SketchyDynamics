#ifndef PhySketchPhysicsBody_h__
#define PhySketchPhysicsBody_h__

#include "PhySketchDefinitions.h"
#include "Box2D/Dynamics/b2Body.h"
#include "PhySketchMaterial.h"

namespace PhySketch
{
	class PhysicsBody 
	{	
		friend class PhysicsManager;
	public:
		PhysicsBody();
		PhysicsBody(b2Body *body);
		virtual ~PhysicsBody();
		

		/// <summary> Updates . </summary>
		/// <remarks> This is normally not called by the "user". It is called automatically by PhySketch. </remarks>
		virtual void update();
		
		/// <summary> Gets the box2D body. </summary>
		/// <returns> The box2D body. </returns>
		virtual b2Body* getBox2DBody();

		/// <summary> Sets the box2D body. </summary>
		/// <param name="body"> The box2D body. </param>
		virtual void setBox2DBody(b2Body* body);

		virtual ulong getId() const { return _id; }

		virtual bool isSelected() const;
		virtual bool isSelectable() const;

		virtual Vector2 translate(Vector2 translation);
		virtual void rotateAroundPoint(float angle, Vector2 rotationPoint);

		// Access the FillMaterial
		const Material& getFillMaterial(void) const;
		void setFillMaterial(const Material& fillMaterial);
		
		// Access the LineMaterial
		const Material& getLineMaterial(void) const;
		void setLineMaterial(const Material& lineMaterial);

		// Access the SelectedMaterial
		const Material& getSelectedMaterial(void) const;
		void setSelectedMaterial(const Material& material);

		virtual void reconstructPolygons();

	protected:
		bool _needsPolygonUpdate;
		b2Body* _body;
		Polygon * _fillPolygon;
		Polygon * _linePolygon;
		std::vector<Polygon*> _oldPolygons;
		ulong _id;
		bool _selectable;
		bool _selected;
	
		Material _fillMaterial;
		Material _lineMaterial;
		Material _selectedMaterial;
	};
}
#endif // PhySketchPhysicsBody_h__