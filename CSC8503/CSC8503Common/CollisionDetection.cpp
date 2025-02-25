#include "CollisionDetection.h"
#include "CollisionVolume.h"
#include "AABBVolume.h"
#include "OBBVolume.h"
#include "SphereVolume.h"
#include "../../Common/Vector2.h"
#include "../../Common/Window.h"
#include "../../Common/Maths.h"
#include "Debug.h"

#include <list>

using namespace NCL;

bool CollisionDetection::RayPlaneIntersection(const Ray&r, const Plane&p, RayCollision& collisions) {
	float ln = Vector3::Dot(p.GetNormal(), r.GetDirection());

	if (ln == 0.0f) {
		return false; //direction vectors are perpendicular!
	}
	
	Vector3 planePoint = p.GetPointOnPlane();

	Vector3 pointDir = planePoint - r.GetPosition();

	float d = Vector3::Dot(pointDir, p.GetNormal()) / ln;

	collisions.collidedAt = r.GetPosition() + (r.GetDirection() * d);

	return true;
}

bool CollisionDetection::RayIntersection(const Ray& r,GameObject& object, RayCollision& collision) {
	bool hasCollided = false;

	const Transform& worldTransform = object.GetTransform();
	const CollisionVolume* volume	= object.GetBoundingVolume();

	if (!volume) {
		return false;
	}

switch (volume->type) {
case VolumeType::AABB:		hasCollided = RayAABBIntersection(r, worldTransform, (const AABBVolume&)*volume, collision); break;
case VolumeType::OBB:		hasCollided = RayOBBIntersection(r, worldTransform, (const OBBVolume&)*volume, collision); break;
case VolumeType::Sphere:	hasCollided = RaySphereIntersection(r, worldTransform, (const SphereVolume&)*volume, collision); break;
case VolumeType::Capsule:	hasCollided = RayCapsuleIntersection(r, worldTransform, (const CapsuleVolume&)*volume, collision); break;
}

return hasCollided;
}

bool CollisionDetection::RayBoxIntersection(const Ray& r, const Vector3& boxPos, const Vector3& boxSize, RayCollision& collision)
{
	Vector3 boxMin = boxPos - boxSize;
	Vector3 boxMax = boxPos + boxSize;

	Vector3 rayPos = r.GetPosition();
	Vector3 rayDir = r.GetDirection();

	Vector3 tVals(-1, -1, -1);

	for (int i = 0; i < 3; ++i)// get best 3 intersections
	{
		if (rayDir[i] > 0)
		{
			tVals[i] = (boxMin[i] - rayPos[i]) / rayDir[i];
		}
		else if (rayDir[i] < 0)
		{
			tVals[i] = (boxMax[i] - rayPos[i]) / rayDir[i];
		}
	}
	float bestT = tVals.GetAbsMaxElement();
	if (bestT < 0.0f)
	{
		return false;
	}

	Vector3 intersection = rayPos + (rayDir * bestT);
	const float epsilon = 0.0001f;// an amount of leeway in our calcs

	for (int i = 0; i < 3; ++i)
	{
		if (intersection[i] + epsilon < boxMin[i] || intersection[i] - epsilon > boxMax[i])
		{
			return false; // best intersection doesn �t touch the box !
		}
	}

	collision.collidedAt = intersection;
	collision.rayDistance = bestT;

	return true;
}

bool CollisionDetection::RayAABBIntersection(const Ray& r, const Transform& worldTransform, const AABBVolume& volume, RayCollision& collision)
{
	Vector3 boxPos = worldTransform.GetPosition();
	Vector3 boxSize = volume.GetHalfDimensions();
	return RayBoxIntersection(r, boxPos, boxSize, collision);
}

bool CollisionDetection::RayOBBIntersection(const Ray& r, const Transform& worldTransform, const OBBVolume& volume, RayCollision& collision)
{
	Quaternion orientation = worldTransform.GetOrientation();
	Vector3 position = worldTransform.GetPosition();

	Matrix3 transform = Matrix3(orientation);
	Matrix3 invTransform = Matrix3(orientation.Conjugate());

	Vector3 localRayPos = r.GetPosition() - position;

	Ray tempRay(invTransform * localRayPos, invTransform * r.GetDirection());

	bool collided = RayBoxIntersection(tempRay, Vector3(),
		volume.GetHalfDimensions(), collision);

	if (collided)
	{
		collision.collidedAt = transform * collision.collidedAt + position;
	}

	return collided;
}

bool CollisionDetection::RayCapsuleIntersection(const Ray& r, const Transform& worldTransform, const CapsuleVolume& volume, RayCollision& collision)
{
	Vector3 origin = worldTransform.GetPosition();
	float capsRadius = volume.GetRadius();
	float capsHalfHeight = volume.GetHalfHeight();

	Vector3 up = worldTransform.GetOrientation() * Vector3(0, 1, 0);
	Vector3 planePoint = Vector3::Cross(up, origin - r.GetPosition());

	Plane tempPlane = Plane::PlaneFromTri(origin, origin + up, planePoint);

	bool collided = RayPlaneIntersection(r, tempPlane, collision);

	if (collided)
	{
		Vector3 P = collision.collidedAt;

		Vector3 topShpere = origin + up * (capsHalfHeight - capsRadius);
		Vector3 bottomShpere = origin - up * (capsHalfHeight - capsRadius);
		Vector3 d = origin + up * (Vector3::Dot(P - origin, up));

		float distance = 0.0f;
	
		if (Vector3::Dot(topShpere - P, topShpere - P) < 0)
		{
			distance = (topShpere - P).Length();
		}
		else if ((Vector3::Dot(bottomShpere - P, bottomShpere - P) < 0))
		{
			distance = (bottomShpere - P).Length();
		}
		else 
		{
			distance  = (d - P).Length();
		}

		if (distance < capsRadius)
		{
			Vector3 dir = (origin - r.GetPosition());
			float sphereProj = Vector3::Dot(dir, r.GetDirection());
			Vector3 point = r.GetPosition() + (r.GetDirection() * sphereProj);
			float offset = sqrt((capsRadius * capsRadius) - (distance * distance));
			collision.rayDistance = distance -(offset);
			collision.collidedAt = r.GetPosition() + (r.GetDirection() * collision.rayDistance);
			return true;
		}
	}

	return false;
}

bool CollisionDetection::RaySphereIntersection(const Ray&r, const Transform& worldTransform, const SphereVolume& volume, RayCollision& collision) 
{
	Vector3 spherePos	= worldTransform.GetPosition();
	float sphereRadius	= volume.GetRadius();

	// Get the direction between the ray origin and the sphere origin
	Vector3 dir = (spherePos - r.GetPosition());

	// Then project the sphere�s origin onto our ray direction vector
	float sphereProj = Vector3::Dot(dir, r.GetDirection());

	if (sphereProj < 0.0f)
	{
		return false;
	}

	// Get closest point on ray line to sphere
	Vector3 point = r.GetPosition() + (r.GetDirection() * sphereProj);

	float sphereDist = (point - spherePos).Length();

	if (sphereDist > sphereRadius)
	{
		return false;
	}

	float offset = sqrt((sphereRadius * sphereRadius) - (sphereDist * sphereDist));
	collision.rayDistance = sphereProj - (offset);
	collision.collidedAt = r.GetPosition() + (r.GetDirection() * collision.rayDistance);

	return true;

}

Matrix4 GenerateInverseView(const Camera &c) {
	float pitch = c.GetPitch();
	float yaw	= c.GetYaw();
	Vector3 position = c.GetPosition();

	Matrix4 iview =
		Matrix4::Translation(position) *
		Matrix4::Rotation(-yaw, Vector3(0, -1, 0)) *
		Matrix4::Rotation(-pitch, Vector3(-1, 0, 0));

	return iview;
}

Vector3 CollisionDetection::Unproject(const Vector3& screenPos, const Camera& cam) {
	Vector2 screenSize = Window::GetWindow()->GetScreenSize();

	float aspect	= screenSize.x / screenSize.y;
	float fov		= cam.GetFieldOfVision();
	float nearPlane = cam.GetNearPlane();
	float farPlane  = cam.GetFarPlane();

	//Create our inverted matrix! Note how that to get a correct inverse matrix,
	//the order of matrices used to form it are inverted, too.
	Matrix4 invVP = GenerateInverseView(cam) * GenerateInverseProjection(aspect, fov, nearPlane, farPlane);

	//Our mouse position x and y values are in 0 to screen dimensions range,
	//so we need to turn them into the -1 to 1 axis range of clip space.
	//We can do that by dividing the mouse values by the width and height of the
	//screen (giving us a range of 0.0 to 1.0), multiplying by 2 (0.0 to 2.0)
	//and then subtracting 1 (-1.0 to 1.0).
	Vector4 clipSpace = Vector4(
		(screenPos.x / (float)screenSize.x) * 2.0f - 1.0f,
		(screenPos.y / (float)screenSize.y) * 2.0f - 1.0f,
		(screenPos.z),
		1.0f
	);

	//Then, we multiply our clipspace coordinate by our inverted matrix
	Vector4 transformed = invVP * clipSpace;

	//our transformed w coordinate is now the 'inverse' perspective divide, so
	//we can reconstruct the final world space by dividing x,y,and z by w.
	return Vector3(transformed.x / transformed.w, transformed.y / transformed.w, transformed.z / transformed.w);
}

Ray CollisionDetection::BuildRayFromMouse(const Camera& cam) {
	Vector2 screenMouse = Window::GetMouse()->GetAbsolutePosition();
	Vector2 screenSize	= Window::GetWindow()->GetScreenSize();

	//We remove the y axis mouse position from height as OpenGL is 'upside down',
	//and thinks the bottom left is the origin, instead of the top left!
	Vector3 nearPos = Vector3(screenMouse.x,
		screenSize.y - screenMouse.y,
		-0.99999f
	);

	//We also don't use exactly 1.0 (the normalised 'end' of the far plane) as this
	//causes the unproject function to go a bit weird. 
	Vector3 farPos = Vector3(screenMouse.x,
		screenSize.y - screenMouse.y,
		0.99999f
	);

	Vector3 a = Unproject(nearPos, cam);
	Vector3 b = Unproject(farPos, cam);
	Vector3 c = b - a;

	c.Normalise();

	//std::cout << "Ray Direction:" << c << std::endl;

	return Ray(cam.GetPosition(), c);
}



//http://bookofhook.com/mousepick.pdf
Matrix4 CollisionDetection::GenerateInverseProjection(float aspect, float fov, float nearPlane, float farPlane) {
	Matrix4 m;

	float t = tan(fov*PI_OVER_360);

	float neg_depth = nearPlane - farPlane;

	const float h = 1.0f / t;

	float c = (farPlane + nearPlane) / neg_depth;
	float e = -1.0f;
	float d = 2.0f*(nearPlane*farPlane) / neg_depth;

	m.array[0]  = aspect / h;
	m.array[5]  = tan(fov*PI_OVER_360);

	m.array[10] = 0.0f;
	m.array[11] = 1.0f / d;

	m.array[14] = 1.0f / e;

	m.array[15] = -c / (d*e);

	return m;
}

/*
And here's how we generate an inverse view matrix. It's pretty much
an exact inversion of the BuildViewMatrix function of the Camera class!
*/
Matrix4 CollisionDetection::GenerateInverseView(const Camera &c) {
	float pitch = c.GetPitch();
	float yaw	= c.GetYaw();
	Vector3 position = c.GetPosition();

	Matrix4 iview =
Matrix4::Translation(position) *
Matrix4::Rotation(yaw, Vector3(0, 1, 0)) *
Matrix4::Rotation(pitch, Vector3(1, 0, 0));

return iview;
}


/*
If you've read through the Deferred Rendering tutorial you should have a pretty
good idea what this function does. It takes a 2D position, such as the mouse
position, and 'unprojects' it, to generate a 3D world space position for it.

Just as we turn a world space position into a clip space position by multiplying
it by the model, view, and projection matrices, we can turn a clip space
position back to a 3D position by multiply it by the INVERSE of the
view projection matrix (the model matrix has already been assumed to have
'transformed' the 2D point). As has been mentioned a few times, inverting a
matrix is not a nice operation, either to understand or code. But! We can cheat
the inversion process again, just like we do when we create a view matrix using
the camera.

So, to form the inverted matrix, we need the aspect and fov used to create the
projection matrix of our scene, and the camera used to form the view matrix.

*/
Vector3	CollisionDetection::UnprojectScreenPosition(Vector3 position, float aspect, float fov, const Camera &c) {
	//Create our inverted matrix! Note how that to get a correct inverse matrix,
	//the order of matrices used to form it are inverted, too.
	Matrix4 invVP = GenerateInverseView(c) * GenerateInverseProjection(aspect, fov, c.GetNearPlane(), c.GetFarPlane());

	Vector2 screenSize = Window::GetWindow()->GetScreenSize();

	//Our mouse position x and y values are in 0 to screen dimensions range,
	//so we need to turn them into the -1 to 1 axis range of clip space.
	//We can do that by dividing the mouse values by the width and height of the
	//screen (giving us a range of 0.0 to 1.0), multiplying by 2 (0.0 to 2.0)
	//and then subtracting 1 (-1.0 to 1.0).
	Vector4 clipSpace = Vector4(
		(position.x / (float)screenSize.x) * 2.0f - 1.0f,
		(position.y / (float)screenSize.y) * 2.0f - 1.0f,
		(position.z) - 1.0f,
		1.0f
	);

	//Then, we multiply our clipspace coordinate by our inverted matrix
	Vector4 transformed = invVP * clipSpace;

	//our transformed w coordinate is now the 'inverse' perspective divide, so
	//we can reconstruct the final world space by dividing x,y,and z by w.
	return Vector3(transformed.x / transformed.w, transformed.y / transformed.w, transformed.z / transformed.w);
}

bool CollisionDetection::ObjectIntersection(GameObject* a, GameObject* b, CollisionInfo& collisionInfo) {
	const CollisionVolume* volA = a->GetBoundingVolume();
	const CollisionVolume* volB = b->GetBoundingVolume();

	if (!volA || !volB) {
		return false;
	}

	collisionInfo.a = a;
	collisionInfo.b = b;

	Transform& transformA = a->GetTransform();
	Transform& transformB = b->GetTransform();

	VolumeType pairType = (VolumeType)((int)volA->type | (int)volB->type);

	if (pairType == VolumeType::AABB) {
		return AABBIntersection((AABBVolume&)*volA, transformA, (AABBVolume&)*volB, transformB, collisionInfo);
	}

	if (pairType == VolumeType::Sphere) {
		return SphereIntersection((SphereVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}

	if (pairType == VolumeType::Capsule) {
		return CapsuleIntersection((CapsuleVolume&)*volA, transformA, (CapsuleVolume&)*volB, transformB, collisionInfo);
	}

	if (pairType == VolumeType::OBB) {
		return OBBIntersection((OBBVolume&)*volA, transformA, (OBBVolume&)*volB, transformB, collisionInfo);
	}

	if (volA->type == VolumeType::AABB && volB->type == VolumeType::Sphere) {
		return AABBSphereIntersection((AABBVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}
	if (volA->type == VolumeType::Sphere && volB->type == VolumeType::AABB) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return AABBSphereIntersection((AABBVolume&)*volB, transformB, (SphereVolume&)*volA, transformA, collisionInfo);
	}

	if (volA->type == VolumeType::OBB && volB->type == VolumeType::Sphere) 
	{
		return OBBShpereIntersection((OBBVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}

	if (volA->type == VolumeType::Sphere && volB->type == VolumeType::OBB)
	{
		collisionInfo.a = b;
		collisionInfo.b = a;
		return OBBShpereIntersection((OBBVolume&)*volB, transformB, (SphereVolume&)*volA, transformA, collisionInfo);
	
	}

	if (volA->type == VolumeType::OBB && volB->type == VolumeType::AABB)
	{
		return OBBAABBIntersection((OBBVolume&)*volA, transformA, (AABBVolume&)*volB, transformB, collisionInfo);

	}

	if (volA->type == VolumeType::AABB && volB->type == VolumeType::OBB)
	{
		collisionInfo.a = b;
		collisionInfo.b = a;
		return OBBAABBIntersection((OBBVolume&)*volB, transformB, (AABBVolume&)*volA, transformA, collisionInfo);

	}

	if (volA->type == VolumeType::Capsule && volB->type == VolumeType::Sphere) {
		return SphereCapsuleIntersection((CapsuleVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}

	if (volA->type == VolumeType::Sphere && volB->type == VolumeType::Capsule) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return SphereCapsuleIntersection((CapsuleVolume&)*volB, transformB, (SphereVolume&)*volA, transformA, collisionInfo);
	}

	if (volA->type == VolumeType::Capsule && volB->type == VolumeType::AABB) {
		return CapsuleAABBIntersection((CapsuleVolume&)*volA, transformA, (AABBVolume&)*volB, transformB, collisionInfo);
	}

	if (volA->type == VolumeType::AABB && volB->type == VolumeType::Capsule) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return CapsuleAABBIntersection((CapsuleVolume&)*volB, transformB, (AABBVolume&)*volA, transformA, collisionInfo);
	}

	if (volA->type == VolumeType::Capsule && volB->type == VolumeType::OBB) {
		return CapsuleOBBIntersection((CapsuleVolume&)*volA, transformA, (OBBVolume&)*volB, transformB, collisionInfo);
	}

	if (volA->type == VolumeType::OBB && volB->type == VolumeType::Capsule) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return CapsuleOBBIntersection((CapsuleVolume&)*volB, transformB, (OBBVolume&)*volA, transformA, collisionInfo);
	}

	if (volA->type == VolumeType::Sphere && volB->type == VolumeType::OBB) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return CapsuleOBBIntersection((CapsuleVolume&)*volB, transformB, (OBBVolume&)*volA, transformA, collisionInfo);
	}

	return false;
}

bool CollisionDetection::AABBTest(const Vector3& posA, const Vector3& posB, const Vector3& halfSizeA, const Vector3& halfSizeB) 
{
	Vector3 delta = posB - posA;
	Vector3 totalSize = halfSizeA + halfSizeB;
	
	if (abs(delta.x) < totalSize.x && abs(delta.y) < totalSize.y && abs(delta.z) < totalSize.z) 
	{
		return true;
	}
	return false;
}

//AABB/AABB Collisions
bool CollisionDetection::AABBIntersection(const AABBVolume& volumeA, const Transform& worldTransformA,
	const AABBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo)
{
	Vector3 boxAPos = worldTransformA.GetPosition();
	Vector3 boxBPos = worldTransformB.GetPosition();
	
	Vector3 boxASize = volumeA.GetHalfDimensions();
	Vector3 boxBSize = volumeB.GetHalfDimensions();
	
	bool overlap = AABBTest(boxAPos, boxBPos, boxASize, boxBSize);

	if (overlap) 
	{
		static const Vector3 faces[6] =
		{
			Vector3(-1, 0, 0), Vector3(1, 0, 0),
			Vector3(0, -1, 0), Vector3(0, 1, 0),
			Vector3(0, 0, -1), Vector3(0, 0, 1),
		};
		
		Vector3 maxA = boxAPos + boxASize;
		Vector3 minA = boxAPos - boxASize;
		
		Vector3 maxB = boxBPos + boxBSize;
		Vector3 minB = boxBPos - boxBSize;
		
		float distances[6] =
		{
			(maxB.x - minA.x),// distance of box �b� to �left � of �a �.
			(maxA.x - minB.x),// distance of box �b� to �right � of �a �.
			(maxB.y - minA.y),// distance of box �b� to �bottom � of �a �.
			(maxA.y - minB.y),// distance of box �b� to �top � of �a �.
			(maxB.z - minA.z),// distance of box �b� to �far � of �a �.
			(maxA.z - minB.z) // distance of box �b� to �near � of �a �.
		};
		float penetration = FLT_MAX;
		Vector3 bestAxis;
		
		for (int i = 0; i < 6; i++)
		{
			if (distances[i] < penetration) {
				penetration = distances[i];
				bestAxis = faces[i];
			}
		}
		collisionInfo.AddContactPoint(Vector3(), Vector3(),
		bestAxis, penetration);
		return true;
	}

	return false;
}

//Sphere / Sphere Collision
bool CollisionDetection::SphereIntersection(const SphereVolume& volumeA, const Transform& worldTransformA,
	const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) 
{
	float radii = volumeA.GetRadius() + volumeB.GetRadius();
	Vector3 delta = worldTransformB.GetPosition() - worldTransformA.GetPosition();

	float deltaLength = delta.Length();

	if (deltaLength < radii)
	{
		float penetraion = (radii - deltaLength);
		Vector3 normal = delta.Normalised();

		Vector3 localA = normal * volumeA.GetRadius();
		Vector3 localB = -normal * volumeB.GetRadius();

		collisionInfo.AddContactPoint(localA, localB, normal, penetraion);

		return true;//we're colliding!
	}

	return false;
}

//AABB - Sphere Collision
bool CollisionDetection::AABBSphereIntersection(const AABBVolume& volumeA, const Transform& worldTransformA,
	const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo)
{

	Vector3 boxSize = volumeA.GetHalfDimensions();
	Vector3 delta	= worldTransformB.GetPosition() - worldTransformA.GetPosition();

	Vector3 closestPointOnBox = Maths::Clamp(delta, -boxSize, boxSize);

	Vector3 localPoint = delta - closestPointOnBox;
	float distance = localPoint.Length();
	if (distance < volumeB.GetRadius())
	{//yes we're colliding!
		Vector3 collisionNormal = localPoint.Normalised();
		float penetration = (volumeB.GetRadius() - distance);

		Vector3 localA = Vector3();
		Vector3 localB = -collisionNormal * volumeB.GetRadius();
		
		collisionInfo.AddContactPoint(localA, localB, collisionNormal, penetration);

		return true;
	}

	return false;
}

//OBB/OBB Collisions
bool CollisionDetection::OBBIntersection(
	const OBBVolume& volumeA, const Transform& worldTransformA,
	const OBBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo)
{
	Quaternion orientationA = worldTransformA.GetOrientation();
	Quaternion orientationB = worldTransformB.GetOrientation();

	Vector3 boxAPos = worldTransformA.GetPosition();
	Vector3 boxBPos = worldTransformB.GetPosition();

	Vector3 boxASize = volumeA.GetHalfDimensions();
	Vector3 boxBSize = volumeB.GetHalfDimensions();

	static const Vector3 faces[3] =
	{
		Vector3(1, 0, 0),
		Vector3(0, 1, 0),
		Vector3(0, 0, 1)
	};

	Vector3 Axis[15];

	for (int i = 0; i < 3; i++)
	{
		Axis[i] = orientationA * faces[i];
		Axis[i].Normalise();
	}

	for (int i = 3; i < 6; i++)
	{
		Axis[i] = orientationB * faces[i - 3];
		Axis[i].Normalise();
	}

	Axis[6] = Vector3::Cross(Axis[0], Axis[3]).Normalised();
	Axis[7] = Vector3::Cross(Axis[0], Axis[4]).Normalised();
	Axis[8] = Vector3::Cross(Axis[0], Axis[5]).Normalised();

	Axis[9] = Vector3::Cross(Axis[1], Axis[3]).Normalised();
	Axis[10] = Vector3::Cross(Axis[1], Axis[4]).Normalised();
	Axis[11] = Vector3::Cross(Axis[1], Axis[5]).Normalised();

	Axis[12] = Vector3::Cross(Axis[2], Axis[3]).Normalised();
	Axis[13] = Vector3::Cross(Axis[2], Axis[4]).Normalised();
	Axis[14] = Vector3::Cross(Axis[2], Axis[5]).Normalised();

	Vector3 overLapAxis;
	float penetration = FLT_MAX;
	Vector3 contactA;
	Vector3 contactB;

	for (int i = 0; i < 15; i++)
	{
		Vector3 maxA = volumeA.OBBSupport(worldTransformA, Axis[i]);
		Vector3 minA = volumeA.OBBSupport(worldTransformA, -Axis[i]);

		Vector3 maxB = volumeB.OBBSupport(worldTransformB, Axis[i]);
		Vector3 minB = volumeB.OBBSupport(worldTransformB, -Axis[i]);

		maxA = maxA * boxASize;
		minA = minA * boxASize;

		maxB = maxB * boxBSize;
		minB = minB * boxBSize;

	/*	maxA = worldTransformA.GetOrientation() * maxA;
		minA = worldTransformA.GetOrientation() * minA;

		maxB = worldTransformB.GetOrientation() * maxB;
		minB = worldTransformB.GetOrientation() * minB;*/

		float MaxExtentA = Vector3::Dot(Axis[i], maxA);
		float MinExtentA = Vector3::Dot(Axis[i], minA);
		float MaxExtentB = Vector3::Dot(Axis[i], maxB);
		float MinExtentB = Vector3::Dot(Axis[i], minB);


		if ((MinExtentA < MinExtentB) && (MaxExtentA > MinExtentB)
			|| (MinExtentB < MinExtentA) && (MaxExtentB > MinExtentA))
		{
			//test = true;
			//overlap = true;		
			float tempPenetration = 0;

			if((MinExtentA < MinExtentB) && (MaxExtentA > MinExtentB))
			{
				tempPenetration = abs(MaxExtentA - MinExtentB);
			}

			if ((MinExtentB < MinExtentA) && (MaxExtentB > MinExtentA))
			{
				tempPenetration = abs(MinExtentA - MaxExtentB);
			}

			if (tempPenetration == 0)
			{
				return false;
			}
			else if (penetration > tempPenetration)
			{
				penetration = tempPenetration;
				overLapAxis = Axis[i];

				//temp need to better think this  out

				Vector3 maxAdist = boxAPos - maxA;
				Vector3 minAdist = boxAPos - minA;

				if (maxAdist.Length() > minAdist.Length())
				{
					contactA = maxA;
				}
				else
				{
					contactA = minA;
				}

				Vector3 maxBdist = boxBPos - maxB;
				Vector3 minBdist = boxBPos - minB;

				if (maxBdist.Length() > minBdist.Length())
				{
					contactB =  maxB;
				}
				else
				{
					contactB = minB;
				}
			}
		}
		else
		{
			//axis has a seperation on it so the two volumes cannot be overalping
			return false;
		}	
	}

	collisionInfo.AddContactPoint(contactA, contactB, overLapAxis, penetration);

	return true;

}

//OBB/AABB Collisions
bool CollisionDetection::OBBAABBIntersection(
	const OBBVolume& volumeA, const Transform& worldTransformA,
	const AABBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo)
{
	//Quaternion orientationA = worldTransformA.GetOrientation();

	//Vector3 boxASize = volumeA.GetHalfDimensions();
	//Vector3 boxBSize = volumeB.GetHalfDimensions();

	//static const Vector3 faces[3] =
	//{
	//	Vector3(1, 0, 0),
	//	Vector3(0, 1, 0),
	//	Vector3(0, 0, 1)
	//};

	//Vector3 Axis[15];

	//for (int i = 0; i < 3; i++)
	//{
	//	Axis[i] = orientationA * faces[i];
	//	Axis[i].Normalise();
	//}

	//for (int i = 3; i < 6; i++)
	//{
	//	Axis[i] = faces[i - 3];
	//	Axis[i].Normalise();
	//}

	//Axis[6] = Vector3::Cross(Axis[0], Axis[3]).Normalised();
	//Axis[7] = Vector3::Cross(Axis[0], Axis[4]).Normalised();
	//Axis[8] = Vector3::Cross(Axis[0], Axis[5]).Normalised();

	//Axis[9] = Vector3::Cross(Axis[1], Axis[3]).Normalised();
	//Axis[10] = Vector3::Cross(Axis[1], Axis[4]).Normalised();
	//Axis[11] = Vector3::Cross(Axis[1], Axis[5]).Normalised();

	//Axis[12] = Vector3::Cross(Axis[2], Axis[3]).Normalised();
	//Axis[13] = Vector3::Cross(Axis[2], Axis[4]).Normalised();
	//Axis[14] = Vector3::Cross(Axis[2], Axis[5]).Normalised();
	//Vector3 overLapAxis;
	//float penetration = FLT_MAX;
	//Vector3 contactA;
	//Vector3 contactB;

	//for (int i = 0; i < 15; i++)
	//{
	//	Vector3 maxA = volumeA.OBBSupport(worldTransformA, Axis[i]);
	//	Vector3 minA = volumeA.OBBSupport(worldTransformA, -Axis[i]);

	//	Vector3 maxB = volumeB.AABBSupport(worldTransformB, Axis[i]);
	//	Vector3 minB = volumeB.AABBSupport(worldTransformB, -Axis[i]);

	//	maxA = maxA * boxASize;
	//	minA = minA * boxASize;

	//	maxB = maxB * boxBSize;
	//	minB = minB * boxBSize;

	//	//maxA = worldTransformA.GetOrientation() * maxA;
	//	//minA = worldTransformA.GetOrientation() * minA;

	//	//maxB = worldTransformB.GetOrientation() * maxB;
	//	//minB = worldTransformB.GetOrientation() * minB;

	//	float MaxExtentA = Vector3::Dot(Axis[i], maxA);
	//	float MinExtentA = Vector3::Dot(Axis[i], minA);
	//	float MaxExtentB = Vector3::Dot(Axis[i], maxB);
	//	float MinExtentB = Vector3::Dot(Axis[i], minB);


	//	if ((MinExtentA < MinExtentB) && (MaxExtentA > MinExtentB)
	//		|| (MinExtentB < MinExtentA) && (MaxExtentB > MinExtentA))
	//	{
	//		//test = true;
	//		//overlap = true;		
	//		float tempPenetration = 0;

	//		if ((MinExtentA < MinExtentB) && (MaxExtentA > MinExtentB))
	//		{
	//			tempPenetration = abs(MaxExtentA - MinExtentB);
	//		}

	//		if ((MinExtentB < MinExtentA) && (MaxExtentB > MinExtentA))
	//		{
	//			tempPenetration = abs(MinExtentA - MaxExtentB);
	//		}

	//		if (penetration > tempPenetration)
	//		{
	//			penetration = tempPenetration;
	//			overLapAxis = Axis[i];
	//		}
	//	}
	//	else
	//	{
	//		//axis has a seperation on it so the two volumes cannot be overalping
	//		return false;
	//	}
	//}

	//collisionInfo.AddContactPoint(Vector3(), Vector3(), overLapAxis, penetration);

	return false;
}


//OBB/Sphere Collisions
bool CollisionDetection::OBBShpereIntersection(
	const OBBVolume& volumeA, const Transform& worldTransformA,
	const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo)
{
	Quaternion orientation = worldTransformA.GetOrientation();

	//Matrix3 transform = Matrix3(orientation);
	//Matrix3 invTransform = Matrix3();

	Vector3 localPos =  worldTransformB.GetPosition();
//invTransform *
	Vector3 boxSize = volumeA.GetHalfDimensions();

	Vector3 delta =  localPos - worldTransformA.GetPosition();

	delta = orientation.Conjugate() * delta;

	Vector3 closestPointOnBox = Maths::Clamp( delta, -boxSize, boxSize);

	Vector3 localPoint = delta - closestPointOnBox;
	float distance = localPoint.Length();

	if (distance < volumeB.GetRadius())
	{
		Vector3 collisionNormal = localPoint.Normalised();
		collisionNormal = orientation * collisionNormal;
		collisionNormal.Normalise();

		float penetration = (volumeB.GetRadius() - distance);
		Vector3 localA = orientation * closestPointOnBox;
		Vector3 localB = -collisionNormal * volumeB.GetRadius();

		collisionInfo.AddContactPoint(localA, localB, collisionNormal, penetration);

		return true;
	}
	return false;
}

//capsule/capsule
bool CollisionDetection::CapsuleIntersection(const CapsuleVolume& volumeA, const Transform& worldTransformA,
	const CapsuleVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo)
{
	Vector3 capsPosA = worldTransformA.GetPosition();
	float capsRadiusA = volumeA.GetRadius();
	float capsHalfHeightA = volumeA.GetHalfHeight();
	Vector3 capsUpA = worldTransformA.GetOrientation() * Vector3(0, 1, 0);
	capsUpA.Normalise();

	Vector3 topShpereA = capsPosA + capsUpA * (capsHalfHeightA - capsRadiusA);
	Vector3 bottomShpereA = capsPosA - capsUpA * (capsHalfHeightA - capsRadiusA);

	float radii = volumeA.GetRadius() + volumeB.GetRadius();

	Vector3 capsPosB = worldTransformB.GetPosition();
	float capsRadiusB = volumeB.GetRadius();
	float capsHalfHeightB = volumeB.GetHalfHeight();
	Vector3 capsUpB = worldTransformB.GetOrientation() * Vector3(0, 1, 0);
	capsUpA.Normalise();

	Vector3 topShpereB = capsPosB + capsUpB * (capsHalfHeightB - capsRadiusB);
	Vector3 bottomShpereB = capsPosB - capsUpB * (capsHalfHeightB - capsRadiusB);

	Vector3 dA = capsPosA + capsUpA * (Vector3::Dot(capsPosB - capsPosA, capsUpA));
	Vector3 dB = capsPosB + capsUpB * (Vector3::Dot(capsPosA - capsPosB, capsUpB));

	Vector3 delta = Vector3();

	if (Vector3::Dot(topShpereA - capsPosB, topShpereA - capsPosA) < 0)
	{
		if (Vector3::Dot(topShpereB - topShpereA, topShpereB - capsPosB) < 0)
		{
			delta = topShpereB - topShpereA;
		}
		else if (Vector3::Dot(bottomShpereB - topShpereA, bottomShpereB - capsPosB) < 0)
		{
			delta = bottomShpereB - topShpereA;
		}
		else
		{
			delta = dB - topShpereA;
		}
	}
	else if (Vector3::Dot(bottomShpereA - capsPosB, bottomShpereA - capsPosA) < 0)
	{
		if (Vector3::Dot(topShpereB - bottomShpereA, topShpereB - capsPosB) < 0)
		{
			delta = topShpereB - bottomShpereA;
		}
		else if (Vector3::Dot(bottomShpereB - bottomShpereA, bottomShpereB - capsPosB) < 0)
		{
			delta = bottomShpereB - bottomShpereA;
		}
		else
		{
			delta = dB - bottomShpereA;
		}
	}
	else
	{
		if (Vector3::Dot(topShpereB - dB, topShpereB - capsPosB) < 0)
		{
			delta = topShpereB - dA;
		}
		else if (Vector3::Dot(bottomShpereB - dB, bottomShpereB - capsPosB) < 0)
		{
			delta = bottomShpereB - dA;
		}
		else
		{
			delta = dB - dA;
		}
	}


	float deltaLength = delta.Length();

	if (deltaLength < radii)
	{
		float penetraion = (radii - deltaLength);
		Vector3 normal = delta.Normalised();
		Vector3 localA = normal * volumeA.GetRadius();
		Vector3 localB = -normal * volumeB.GetRadius();
		collisionInfo.AddContactPoint(localA, localB, normal, penetraion);
		return true;
	}

	return false;
}

//Sphere/Capsule Collisions
bool CollisionDetection::SphereCapsuleIntersection(
	const CapsuleVolume& volumeA, const Transform& worldTransformA,
	const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) 
{

	Vector3 capsPos = worldTransformA.GetPosition();
	float capsRadius = volumeA.GetRadius();
	float capsHalfHeight = volumeA.GetHalfHeight();
	Vector3 capsUp = worldTransformA.GetOrientation() * Vector3(0, 1, 0);
	capsUp.Normalise();

	Vector3 spherePos = worldTransformB.GetPosition();
	float sphereRadius = volumeB.GetRadius();

	float radii = volumeA.GetRadius() + volumeB.GetRadius();

	Vector3 topShpere = capsPos + capsUp * (capsHalfHeight - capsRadius);
	Vector3 bottomShpere = capsPos - capsUp * (capsHalfHeight - capsRadius);
	Vector3 d = capsPos + capsUp * (Vector3::Dot(spherePos - capsPos, capsUp));

	Vector3 delta = Vector3();


	if (Vector3::Dot(topShpere - spherePos, topShpere - capsPos) < 0)
	{
		delta = spherePos - topShpere;
	}
	else if (Vector3::Dot(bottomShpere - spherePos, bottomShpere - capsPos) < 0)
	{
		delta = spherePos - bottomShpere;
	}
	else
	{
		delta = spherePos - d;
	}

	float deltaLength = delta.Length();

	if (deltaLength < radii)
	{
		float penetraion = (radii - deltaLength);
		Vector3 normal = delta.Normalised();
		Vector3 localA = normal * volumeA.GetRadius();
		Vector3 localB = -normal * volumeB.GetRadius();
		collisionInfo.AddContactPoint(localA, localB, normal, penetraion);
		return true;
	}

	return false;
}

//Capsule/AABB Collisions
bool CollisionDetection::CapsuleAABBIntersection(
	const CapsuleVolume& volumeA, const Transform& worldTransformA,
	const AABBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) 
{
	Vector3 capsPos = worldTransformA.GetPosition();
	float capsRadius = volumeA.GetRadius();
	float capsHalfHeight = volumeA.GetHalfHeight();
	Vector3 capsUp = worldTransformA.GetOrientation() * Vector3(0, 1, 0);
	capsUp.Normalise();

	Vector3 boxPos = worldTransformB.GetPosition();
	Vector3 boxSize = volumeB.GetHalfDimensions();

	Vector3 topShpere = capsPos + capsUp * (capsHalfHeight - capsRadius);
	Vector3 bottomShpere = capsPos - capsUp * (capsHalfHeight - capsRadius);
	Vector3 d = capsPos + capsUp * (Vector3::Dot(boxPos - capsPos, capsUp));

	Vector3 delta = Vector3();

	if (Vector3::Dot(topShpere - boxPos, topShpere - capsPos) < 0)
	{
		delta = boxPos - topShpere;
	}
	else if (Vector3::Dot(bottomShpere - boxPos, bottomShpere - capsPos) < 0)
	{
		delta = boxPos - bottomShpere;
	}
	else
	{
		delta = boxPos - d;
	}
	//Vector3 delta = boxPos - capsSphereOrigin;

	Vector3 closestPointOnBox = Maths::Clamp(delta, -boxSize, boxSize);
	Vector3 localPoint = delta - closestPointOnBox;
	float distance = localPoint.Length();

	if (distance < capsRadius)
	{
		Vector3 collisionNormal = localPoint.Normalised();
		float penetration = (capsRadius - distance);
		Vector3 localA = collisionNormal * capsRadius;
		Vector3 localB = Vector3();
		collisionInfo.AddContactPoint(localA, localB, collisionNormal, penetration);
		return true;
	}
	return false;

}

//Capsule/OBB Collisions
bool CollisionDetection::CapsuleOBBIntersection(
	const CapsuleVolume& volumeA, const Transform& worldTransformA,
	const OBBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo)
{
	Quaternion orientation = worldTransformB.GetOrientation();
	Matrix3 invTransform = Matrix3(orientation.Conjugate());


	Vector3 capsPos = worldTransformA.GetPosition();
	float capsRadius = volumeA.GetRadius();
	float capsHalfHeight = volumeA.GetHalfHeight();
	Vector3 capsUp = worldTransformA.GetOrientation() * Vector3(0, 1, 0);
	capsUp.Normalise();

	Vector3 boxPos = worldTransformB.GetPosition();
	Vector3 boxSize = volumeB.GetHalfDimensions();

	Vector3 localCaps = invTransform * capsPos;
	//Vector3 localUp = invTransform * capsUp;

	Vector3 topShpere = localCaps + capsUp * (capsHalfHeight - capsRadius);
	Vector3 bottomShpere = localCaps - capsUp * (capsHalfHeight - capsRadius);
	Vector3 d = localCaps + capsUp * (Vector3::Dot(boxPos - localCaps, capsUp));

	Vector3 delta = Vector3();

	if (Vector3::Dot(topShpere - boxPos, topShpere - localCaps) < 0)
	{
		delta = boxPos - topShpere;

	}
	else if (Vector3::Dot(bottomShpere - boxPos, bottomShpere - localCaps) < 0)
	{
		delta = boxPos - bottomShpere;
	}
	else
	{
		delta = boxPos - d;
	}
	//delta = invTransform * delta;

	Vector3 closestPointOnBox = Maths::Clamp(delta, -boxSize, boxSize);
	Vector3 localPoint = delta - closestPointOnBox;
	float distance = localPoint.Length();

	if (distance < capsRadius)
	{
		Vector3 collisionNormal = localPoint.Normalised();
		collisionNormal = orientation * collisionNormal;
		collisionNormal.Normalise();

		float penetration = (capsRadius - distance);
		Vector3 localA = -collisionNormal * capsRadius;
		Vector3 localB = orientation * closestPointOnBox;

		collisionInfo.AddContactPoint(localA, localB, collisionNormal, penetration);

		//Vector3 collisionNormal = localPoint.Normalised();
		//collisionNormal = orientation * collisionNormal;
		//collisionNormal.Normalise();

		//float penetration = (volumeB.GetRadius() - distance);
		//Vector3 localA = orientation * closestPointOnBox;
		//Vector3 localB = -collisionNormal * volumeB.GetRadius();

		//collisionInfo.AddContactPoint(localA, localB, collisionNormal, penetration);

		//return true;
		return true;
	}
	return false;

}