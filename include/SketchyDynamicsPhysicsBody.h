#ifndef SketchyDynamicsPhysicsBody_h__
#define SketchyDynamicsPhysicsBody_h__

#include "SketchyDynamicsDefinitions.h"
#include "SketchyDynamicsMaterial.h"
#include "SketchyDynamicsPolygon.h"

class b2Body;

namespace SketchyDynamics
{
	enum PhysicsBodyType
	{
		PBT_Rectangle,
		PBT_Triangle,
		PBT_Circle,
		PBT_Freeform,
		PBT_Unknown
	};

	class PhysicsBody : public Polygon
	{	
	protected:
		friend class PhysicsManager;
	
		PhysicsBody(b2Body *body, uint id, Material* fillMaterial, Material* lineMaterial, Material* selectedMaterial);
		virtual ~PhysicsBody();
	
	public:	
		/// <summary> Updates . </summary>
		/// <remarks> This is normally not called by the "user". It is called automatically by SketchyDynamics. </remarks>
		virtual void update(ulong timeSinceLastFrame);
		
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
		virtual void scaleAroundPoint(const Vector2& factor, const Vector2& point);

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

		// Access the Type
		virtual const PhysicsBodyType getType(void) const;
		virtual void setType(PhysicsBodyType type);

		/// <summary> Wheather or not this body should be saved to disk when
		/// 	saving. </summary>
		/// <remarks> This does not actually save it, it only specifies that it
		/// 	would be included in the saving process. </remarks>
		/// <param name="save"> true to indicate that it should be saved. </param>
		virtual void setSaveToDisk(bool save);

		/// <summary> Returns wheather or not this body is going to be saved to
		/// 	disk when saving. </summary>
		/// <returns> true if the body is going to be saved. </returns>
		virtual bool getSaveToDisk() const;


	protected:
		b2Body* _body;
		ulong _id;
		bool _selectable;
		bool _selected;
		PhysicsBodyType _type;
	
		Material* _fillMaterial;
		Material* _lineMaterial;
		Material* _selectedMaterial;
		bool _saveToDisk;
	};
}
#endif // SketchyDynamicsPhysicsBody_h__