#ifndef PhySketchPhysicsBody_h__
#define PhySketchPhysicsBody_h__

#include "PhySketchDefinitions.h"
#include "PhySketchMaterial.h"
#include "PhySketchPolygon.h"

class b2Body;

namespace PhySketch
{
	class PhysicsBody : public Polygon
	{	
	protected:
		friend class PhysicsManager;
	
		PhysicsBody(b2Body *body, uint id, Material* fillMaterial, Material* lineMaterial, Material* selectedMaterial);
		virtual ~PhysicsBody();
	
	public:	
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

		virtual void setAngle( float angle );
		virtual void setPosition( const Vector2& position );
		virtual void setScale( const Vector2& scale );

		virtual void translate(const Vector2& amount);
		virtual void rotate( const float& angle );
		virtual void rotateAroundPoint(float angle, const Vector2& rotationPoint);
		virtual void scale(const Vector2& factor);

		// Access the FillMaterial
		Material* getFillMaterial(void) const;
		void setFillMaterial(Material* fillMaterial);
		
		// Access the LineMaterial
		Material* getLineMaterial(void) const;
		void setLineMaterial(Material* lineMaterial);

		// Access the SelectedMaterial
		Material* getSelectedMaterial(void) const;
		void setSelectedMaterial(Material* material);

		virtual void reconstructPolygon();

	protected:
		b2Body* _body;
		ulong _id;
		bool _selectable;
		bool _selected;
	
		Material* _fillMaterial;
		Material* _lineMaterial;
		Material* _selectedMaterial;
	};
}
#endif // PhySketchPhysicsBody_h__