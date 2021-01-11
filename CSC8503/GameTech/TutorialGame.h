#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "StateGameObject.h"
#include "BonusGameObject.h"
#include "PlayerGameObject.h"
#include "MovingGameObject.h"
#include "MovingPlatform.h"
#include "FollowEnemy.h"
#include "SpeedBlock.h"

namespace NCL {
	namespace CSC8503 {
		enum type {bonus, Players, enemy, obstacle, finish};

		class TutorialGame		{
		public:
			TutorialGame();
			~TutorialGame();

			virtual void UpdateGame(float dt);

		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			void InitWorld();

			void InitGameExamples();

			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);
			void InitDefaultFloor();
			void BridgeConstraintTest();

			void initStartMenu();
			void InitReplayMenu();

			void InitSingleCourse();
			void bouncyballJump(const Vector3& position);
			void balanceBeam(const Vector3& position);
			void MovingPlatforms(const Vector3& position);

			void initDoubleCourse();
			void raceWay(const Vector3& position);
			void obstacleWay(const Vector3& position);
	
			bool SelectObject();
			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();

			GameObject* AddFloorToWorld(const Vector3& position);
			GameObject* AddSphereToWorld(const Vector3& position, float radius,float elasticity, float inverseMass = 10.0f);
			GameObject* AddStateSphereToWorld(const Vector3& position, float radius, float elasticity, float inverseMass = 10.0f);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float elasticity, float inverseMass = 10.0f);

			GameObject* AddOBBCubeToWorld(const Vector3& position, Vector3 dimensions, float elasticity, float inverseMass = 10.0f);
			
			GameObject* AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass = 10.0f);

			GameObject* AddPlayerToWorld(const Vector3& position);
			GameObject* AddEnemyToWorld(const Vector3& position);

			FollowEnemy* AddFollowEnemyToWorld(const Vector3& position);

			GameObject* AddBonusToWorld(const Vector3& position);

			StateGameObject* AddStateObjectToWorld(const Vector3 & position);
			
			StateGameObject* testStateObject;
			
			MovingPlatform* AddMovingPlatform(const Vector3& position);
			SpeedBlock* AddSpeedBlock(const Vector3& position);


			GameTechRenderer*	renderer;
			PhysicsSystem*		physics;
			GameWorld*			world;

			bool useGravity;
			bool inSelectionMode;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;

			OGLMesh*	capsuleMesh = nullptr;
			OGLMesh*	cubeMesh	= nullptr;
			OGLMesh*	sphereMesh	= nullptr;
			OGLTexture* basicTex	= nullptr;
			OGLShader*	basicShader = nullptr;

			//Coursework Meshes
			OGLMesh*	charMeshA	= nullptr;
			OGLMesh*	charMeshB	= nullptr;
			OGLMesh*	enemyMesh	= nullptr;
			OGLMesh*	bonusMesh	= nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject	= nullptr;
			Vector3 lockedOffset		= Vector3(0, 5, 12);

			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}

			//Coursework Additions
			
			bool gameOver = false;
			bool win = false;
			float time = 0;

			bool tutorial = false;
			bool singlePlayer = true;
			bool twoPlayer = false;
			
			Vector3 courseOffset = Vector3(0, 0, 0);

			PlayerGameObject* player = nullptr;
		
		};
	}
}

