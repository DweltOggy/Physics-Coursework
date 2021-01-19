#pragma once
#include "CollisionVolume.h"
#include "../../Common/Vector3.h"
#include "Transform.h"
namespace NCL {
	class AABBVolume : CollisionVolume
	{
	public:
		AABBVolume(const Vector3& halfDims) {
			type		= VolumeType::AABB;
			halfSizes	= halfDims;
		}
		~AABBVolume() {

		}

		Vector3 GetHalfDimensions() const 
		{
			return halfSizes;
		}

		Maths::Vector3 AABBSupport(const CSC8503::Transform& worldTransform, Vector3 worldDir) const
		{
			Vector3 Vertex;
			Vertex.x = worldDir.x < 0 ? -0.5f : 0.5f;
			Vertex.y = worldDir.y < 0 ? -0.5f : 0.5f;
			Vertex.z = worldDir.z < 0 ? -0.5f : 0.5f;

			return worldTransform.GetMatrix() * Vertex;
		}

	protected:
		Vector3 halfSizes;
	};
}
