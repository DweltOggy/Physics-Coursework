#pragma once
#include "CollisionVolume.h"

namespace NCL {
	class SphereVolume : CollisionVolume
	{
	public:
		SphereVolume(float sphereRadius = 1.0f) 
		{
			type	= VolumeType::Sphere;
			radius	= sphereRadius;
		}
		~SphereVolume() {}

		float GetRadius() const 
		{
			return radius;
		}

		Maths::Vector3 sphereSupport(const CSC8503::Transform& worldTransform, Vector3 axis) const
		{
			return worldTransform.GetPosition() + (axis * radius);
		}

	protected:
		float	radius;
	};
}

