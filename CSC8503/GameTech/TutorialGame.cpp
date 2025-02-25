#include "TutorialGame.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"
#include "../CSC8503Common/PositionConstraint.h"
#include "../CSC8503Common/FloatConstraint.h"
#include "../CSC8503Common/MotorConstraint.h"


using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame()	{
	world		= new GameWorld();
	renderer	= new GameTechRenderer(*world);
	physics		= new PhysicsSystem(*world);

	forceMagnitude	= 10.0f;
	useGravity		= false;
	inSelectionMode = false;
	testStateObject = nullptr;

	tutorial = true;
	singlePlayer = false;
	player = nullptr;
	Debug::SetRenderer(renderer);
	InitialiseAssets();
}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes, 
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/
void TutorialGame::InitialiseAssets() {
	auto loadFunc = [](const string& name, OGLMesh** into) {
		*into = new OGLMesh(name);
		(*into)->SetPrimitiveType(GeometryPrimitive::Triangles);
		(*into)->UploadToGPU();
	};

	loadFunc("cube.msh"		 , &cubeMesh);
	loadFunc("sphere.msh"	 , &sphereMesh);
	loadFunc("Male1.msh"	 , &charMeshA);
	loadFunc("courier.msh"	 , &charMeshB);
	loadFunc("security.msh"	 , &enemyMesh);
	loadFunc("coin.msh"		 , &bonusMesh);
	loadFunc("capsule.msh"	 , &capsuleMesh);

	basicTex	= (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");

	InitCamera();
	//initStartMenu();
	//InitWorld();
	InitSingleCourse();
	//initDoubleCourse();
}

TutorialGame::~TutorialGame()	{
	delete cubeMesh;
	delete sphereMesh;
	delete charMeshA;
	delete charMeshB;
	delete enemyMesh;
	delete bonusMesh;

	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete world;
}

void TutorialGame::UpdateGame(float dt) {
	
	if (!menu)
	{
		if (!inSelectionMode) 
		{
			world->GetMainCamera()->UpdateCamera(dt);
		}

		UpdateKeys();
		if (lockedObject && tutorial) 
		{
			if (lockedObject->IsActive())
				LockedObjectMovement(dt);
		}
		else {
			DebugObjectMovement();
		}

		if (tutorial)
		{
			if (useGravity) {
				Debug::Print("(G)ravity on", Vector2(5, 95));
			}
			else {
				Debug::Print("(G)ravity off", Vector2(5, 95));
			}
	
			SelectObject();
			MoveSelectedObject();
		}

		physics->Update(dt);

		if (lockedObject != nullptr) 
		{
			Vector3 objPos	= lockedObject->GetTransform().GetPosition();
			Vector3 objOrient	= lockedObject->GetTransform().GetOrientation() * lockedOffset;
			Vector3 camPos	= objPos + objOrient;

			Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0,1,0));

			Matrix4 modelMat = temp.Inverse();

			Quaternion q(modelMat);
			Vector3 angles = q.ToEuler(); //nearly there now!

			world->GetMainCamera()->SetPosition(camPos);
			world->GetMainCamera()->SetPitch(-12.0f);
			world->GetMainCamera()->SetYaw(angles.y);
		}

		if (player != nullptr && !tutorial)
		{
			int tempScore = player->getScore();
			renderer->DrawString("Score: " + std::to_string(tempScore), Vector2(5, 10));

		
			if(player->lost)
			{
				//gameOver = true;
				renderer->DrawString("GAME OVER!", Vector2(30, 50),Vector4(1.0f,0.0f,0.0f,0.0f),50.0f);
				renderer->DrawString("Press F1 to retry or F3 to return to menu!", Vector2(15, 65), Vector4(1.0f, 0.0f, 0.0f, 0.0f), 20.0f);
			}
			if (player->won)
			{
				int tempScore = player->getScore();
				renderer->DrawString("YOU WON!", Vector2(35, 50), Vector4(0.0f, 1.0f, 0.0f, 0.0f), 50.0f);
				renderer->DrawString("your final Score: " + std::to_string(tempScore), Vector2(25, 60), Vector4(0.0f, 1.0f, 0.0f, 0.0f), 25.0f);
				renderer->DrawString("Press F1 to retry or F3 to return to menu!", Vector2(15, 65), Vector4(1.0f, 0.0f, 0.0f, 0.0f), 20.0f);
			}
		}
		if (testStateObject) 
		{
			testStateObject -> Update(dt);
		}
	}
	else
	{
		world->GetMainCamera()->SetPosition(Vector3(0, 0, 0));
		renderer->DrawString("Main Menu", Vector2(30, 10), Vector4(1.0f, 0.5f, 0.5f, 0.0f), 50.0f);
		renderer->DrawString("1. Testing ", Vector2(10, 25), Vector4(1.0f, 1.0f, 0.0f, 0.0f), 40.0f);
		renderer->DrawString("2. SinglePlayer", Vector2(10, 45), Vector4(1.0f, 1.0f, 0.0f, 0.0f), 40.0f);
		renderer->DrawString("3. MultiPlayer", Vector2(10, 65), Vector4(1.0f, 1.0f, 0.0f, 0.0f), 40.0f);
		renderer->DrawString("4. Exit", Vector2(10, 85), Vector4(1.0f, 1.0f, 0.0f, 0.0f), 40.0f);
		physics->Update(dt);
	}

	
	world->UpdateWorld(dt);
	renderer->Update(dt);

	Debug::FlushRenderables(dt);
	renderer->Render();
}

void TutorialGame::UpdateKeys() 
{

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2) && tutorial) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G) && tutorial) {
		useGravity = !useGravity; //Toggle gravity!
		physics->UseGravity(useGravity);
	}
	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) {
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) {
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7)) {
		world->ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8)) {
		world->ShuffleObjects(false);
	}

}

void TutorialGame::LockedObjectMovement(float dt) {
	Matrix4 view		= world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld	= view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();

	float force = 5.0f;
	float turnSpeed = 5.0f ;
	float jumpForce = 60.0f ;

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
		lockedObject->GetPhysicsObject()->AddForce(-rightAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
		lockedObject->GetPhysicsObject()->AddForce(rightAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
		lockedObject->GetPhysicsObject()->AddForce(fwdAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
		lockedObject->GetPhysicsObject()->AddForce(-fwdAxis * force);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {
		lockedObject->GetPhysicsObject()->AddForce(Vector3(0, jumpForce, 0));// *jumpForce);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SHIFT)) {
		lockedObject->GetPhysicsObject()->AddForce(Vector3(0, -jumpForce, 0));// *jumpForce);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::E))
	{
		lockedObject->GetPhysicsObject()->AddTorque(-Vector3(0, 1, 0) * turnSpeed);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::Q))
	{
		lockedObject->GetPhysicsObject()->AddTorque(Vector3(0, 1, 0) * turnSpeed);
	}
}

void TutorialGame::DebugObjectMovement() {
//If we've selected an object, we can manipulate it with some key presses
	if (inSelectionMode && selectionObject) {
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM7)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM8)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM5)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
		}
	}

}

void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
	lockedObject = nullptr;
}

void TutorialGame::initStartMenu()
{
	world->ClearAndErase();
	physics->Clear();

	selectionObject = nullptr;

	useGravity = false;
	physics->UseGravity(useGravity);

	menu = true;
	tutorial = false;
	gameOver = false;

	lockedObject = nullptr;

	AddBonusToWorld(Vector3(0, 3, -10));
}

void TutorialGame::InitWorld() 
{
	InitCamera();
	world->ClearAndErase();
	physics->Clear();

	selectionObject = nullptr;

	useGravity = false;
	physics->UseGravity(useGravity);

	gameOver = false;
	tutorial = true;
	menu = false;

	lockedObject = nullptr;
	//InitMixedGridWorld(5, 5, 3.5f, 3.5f);
	InitGameExamples();
	InitDefaultFloor();
	//BridgeConstraintTest();
	//testStateObject = AddStateObjectToWorld(Vector3(0, 10, 0));

}

void TutorialGame::InitSingleCourse()
{
	world->ClearAndErase();
	physics->Clear();

	selectionObject = nullptr;

	useGravity = true;
	physics->UseGravity(useGravity);

	gameOver = false;
	tutorial = false;
	menu = false;

	Vector3 origin = Vector3(0, 0, 0);

	lockedObject = AddPlayerToWorld(origin + Vector3(0, 7, -10));

	bouncyballJump(origin);
	balanceBeam(origin);
	MovingPlatforms(origin);

	GameObject* finishLine = AddCubeToWorld(origin + Vector3(0, 10, -859), Vector3(20, 10, 1), 0.1, 0.5f, Vector4(1, 1, 1, 1), 0);
	finishLine->SetType(finish);
	finishLine->GetRenderObject()->SetColour(Vector4(0.5, 1, 0.5, 0.0f));
}

void TutorialGame::bouncyballJump(const Vector3& position)
{
	Vector3 ballSize = Vector3(5, 5, 5);
	int numBalls = 1;
	float elasticity = 2.0;

	//room layout
	AddCubeToWorld(position + Vector3(-20, 10,-200), Vector3(1, 10, 200),0.1, 0.5f, Vector4(1, 1, 1, 1), 0);
	AddCubeToWorld(position + Vector3(20, 10, -200), Vector3(1, 10, 200), 0.1, 0.5f, Vector4(1, 1, 1, 1), 0);
	AddCubeToWorld(position + Vector3(0, 0, -200), Vector3(20, 2, 200), 0.1, 0.5f, Vector4(1, 1, 1, 1), 0);

	AddCubeToWorld(position + Vector3(0, 10, 1), Vector3(20, 10, 1), 0.1, 0.5f, Vector4(1, 1, 1, 1), 0);

	for (int i = 0; i < numBalls; i++)
	{
		AddSphereToWorld(position + Vector3((rand() % 30) - 15, 7, -((rand() % 380))), 3.0f, elasticity, 0.2);
	}

	//place bonuses
	for (int i = 0; i < 5; i++)
	{
		AddBonusToWorld(position + Vector3((rand() % 30) - 15, 5, -((rand() % 380) + 15) ));
	}
}

void TutorialGame::balanceBeam(const Vector3& position)
{
	//room layout
	AddCubeToWorld(position + Vector3(-20, 10, -500), Vector3(1, 10, 100), 0.1, 0.5f, Vector4(1, 1, 1, 1), 0);
	AddCubeToWorld(position + Vector3(20, 10, -500), Vector3(1, 10, 100), 0.1, 0.5f, Vector4(1, 1, 1, 1), 0);

	AddCubeToWorld(position + Vector3(0, 1, -495), Vector3(3, 1, 95), 0.1, 0.01f, Vector4(0, 1, 1, 1), 0);

	AddCubeToWorld(position + Vector3(0, 1, -595), Vector3(20, 1, 5), 0.1, 0.5f, Vector4(1, 1, 1, 1), 0);

	AddCubeToWorld(position + Vector3(-20, -10, -500), Vector3(1, 10, 100), 0.1, 0.5f, Vector4(1, 1, 1, 1), 0);
	AddCubeToWorld(position + Vector3(20, -10, -500), Vector3(1, 10, 100), 0.1, 0.5f, Vector4(1, 1, 1, 1), 0);
	AddCubeToWorld(position + Vector3(0, -20, -500), Vector3(20, 1, 100), 0.1, 0.5f, Vector4(1, 1, 1, 1), 0);

	AddSpeedBlock(position + Vector3(-16, -19.6f, -406));

	AddCubeToWorld(position + Vector3(0, -10, -399), Vector3(20, 10, 1), 0.1, 0.5f, Vector4(1, 1, 1, 1), 0);

	//obstacles
	float maxDistance = 15; // constraint distance

	for (int i = 0; i < 5; i++)
	{
		GameObject* anchor = AddCubeToWorld(position + Vector3(0, 20, -420 - (i * 40)), Vector3(1, 1, 1), 0.1, 0.5f, Vector4(1, 1, 1, 1), 0);
		GameObject* sphere = AddStateSphereToWorld(position + Vector3(0, 5, -420 - (i * 40)), 3.0f, 0.8f, 0.2f);
		PositionConstraint* constraint = new PositionConstraint(anchor, sphere, maxDistance);
		world->AddConstraint(constraint);
	}
	//place bonuses
	for (int i = 0; i < 5; i++)
	{
		AddBonusToWorld(position + Vector3(0, 5, -400 - (i * 40)));
	}

}

void TutorialGame::MovingPlatforms(const Vector3& position)
{
	//room layout
	AddCubeToWorld(position + Vector3(-50, 0, -600), Vector3(30, 20, 1), 0.1, 0.5f, Vector4(1, 1, 1, 1), 0);
	AddCubeToWorld(position + Vector3(50, 0, -600), Vector3(30, 20, 1), 0.1, 0.5f, Vector4(1, 1, 1, 1), 0);
	AddCubeToWorld(position + Vector3(0, -10, -599), Vector3(20, 10, 1), 0.1, 0.5f, Vector4(1, 1, 1, 1), 0);

	AddSpeedBlock(position + Vector3(0, -19.6f, -606));

	AddCubeToWorld(position + Vector3(-80, 0, -700), Vector3(1, 20, 100), 0.1, 0.5f, Vector4(1, 1, 1, 1), 0);
	AddCubeToWorld(position + Vector3(80, 0, -700), Vector3(1, 20, 100), 0.1, 0.5f, Vector4(1, 1, 1, 1), 0);

	AddCubeToWorld(position + Vector3(0, -20, -700), Vector3(80, 1, 100), 0.1, 0.5f, Vector4(1, 1, 1, 1), 0);

	AddCubeToWorld(position + Vector3(-50, 0, -800), Vector3(30, 20, 1), 0.1, 0.5f, Vector4(1, 1, 1, 1), 0);
	AddCubeToWorld(position + Vector3(50, 0, -800), Vector3(30, 20, 1), 0.1, 0.5f, Vector4(1, 1, 1, 1), 0);
	AddCubeToWorld(position + Vector3(0, -10, -800), Vector3(20, 10, 1), 0.1, 0.5f, Vector4(1, 1, 1, 1), 0);

	AddCubeToWorld(position + Vector3(-20, 10, -829), Vector3(1, 10, 30), 0.1, 0.5f, Vector4(1, 1, 1, 1), 0);
	AddCubeToWorld(position + Vector3(20, 10, -829), Vector3(1, 10, 30), 0.1, 0.5f, Vector4(1, 1, 1, 1), 0);
	AddCubeToWorld(position + Vector3(0, 0, -829), Vector3(20, 1, 30), 0.1, 0.5f, Vector4(1, 1, 1, 1), 0);

	Vector3 bottomLeft = Vector3(-50, 0, -770);
	for (int i = 0; i < 2; i++)
	{
		MovingPlatform* platform = AddMovingPlatform(bottomLeft + Vector3(0+(i*100),0,0+(i*140)));
		FloatConstraint* constraint = new FloatConstraint(platform, 0);
		world->AddConstraint(constraint);
	}

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			if(j == 0 || j == 4 || i == 0 || i == 4)
				AddBonusToWorld(position + Vector3((j - 2) * 30, 5, -700 - ((i -2) * 40)));
		}
	}

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			if (j == 0 || j == 4 || i == 0 || i == 4)
				AddBonusToWorld(position + Vector3((j - 2) * 30, -15, -700 - ((i - 2) * 40)));
		}
	}

}

void TutorialGame::initDoubleCourse()
{
	world->ClearAndErase();
	physics->Clear();

	selectionObject = nullptr;

	useGravity = true;
	physics->UseGravity(useGravity);

	gameOver = false;
	tutorial = false;
	menu = false;

	Vector3 origin = Vector3(0, 0, 0);
	lockedObject = AddPlayerToWorld(origin + Vector3(0, 7, -10));
	
	//place bonuses
	for (int i = 0; i < 10; i++)
	{
		AddBonusToWorld(origin + Vector3((rand() % 30) - 15, 5, -20 - (i * 70)));
	}

	//room layout
	AddCubeToWorld(origin + Vector3(-20, 10, -350), Vector3(1, 10, 350), 0.1, 0.5f, Vector4(1, 1, 1, 1), 0);
	AddCubeToWorld(origin + Vector3(20, 10, -350), Vector3(1, 10, 350), 0.1, 0.5f, Vector4(1, 1, 1, 1), 0);
	AddCubeToWorld(origin + Vector3(0, 0, -350), Vector3(20, 2, 350), 0.1, 0.5f, Vector4(1, 1, 1, 1), 0);
	AddCubeToWorld(origin + Vector3(0, 10, 1), Vector3(20, 10, 1), 0.1, 0.5f, Vector4(1, 1, 1, 1), 0);


	GameObject* finishLine = AddCubeToWorld(origin + Vector3(0, 10, -700), Vector3(20, 10, 1), 0.1, 0.5f, Vector4(0, 1, 1, 1), 0);
	finishLine->SetType(finish);
	
	AddFollowEnemyToWorld(origin + Vector3(10, 7, -2), finishLine);
	AddBehaviourToWorld(origin + Vector3(-10, 7, -2), finishLine);
}


void TutorialGame::BridgeConstraintTest()
{
	Vector3 cubeSize = Vector3(8, 8, 8);

	float invCubeMass = 5; // how heavy the middle pieces are
	int numLinks = 10;
	float maxDistance = 30; // constraint distance
	float cubeDistance = 20; // distance between links
	
	Vector3 startPos = Vector3(0, 0, 0);
	
	GameObject * start = AddCubeToWorld(startPos + Vector3(0, 0, 0)
			, cubeSize,0.1, 0.5f, Vector4(1, 1, 1, 1), 0);
	GameObject* end = AddCubeToWorld(startPos + Vector3(0, 0, -((numLinks + 2) * cubeDistance)), cubeSize, 0.5f, 0.5f, Vector4(1, 1, 1, 1), 0);
	
	GameObject * previous = start;
	
	for (int i = 0; i < numLinks; ++i) 
	{
		GameObject * block = AddCubeToWorld(startPos + Vector3(0, 0,-((i + 1) * cubeDistance)), cubeSize,0.1, 0.5f, Vector4(1, 1, 1, 1),invCubeMass); 
		PositionConstraint * constraint = new PositionConstraint(previous,
			block, maxDistance);
		world -> AddConstraint(constraint);
		previous = block;
	}
	PositionConstraint * constraint = new PositionConstraint(previous,
		end, maxDistance);
	world -> AddConstraint(constraint);

}

/*

A single function to add a large immoveable cube to the bottom of our world

*/
GameObject* TutorialGame::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject();

	Vector3 floorSize	= Vector3(100, 2, 100);
	AABBVolume* volume	= new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(floor);

	return floor;
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple' 
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float elasticity,float inverseMass) {
	GameObject* sphere = new GameObject("Sphere");

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->setElasticity(elasticity);

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();
	sphere->GetPhysicsObject()->SetFriction(0.01f);

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass) {
	GameObject* capsule = new GameObject( "Capsule");

	CapsuleVolume* volume = new CapsuleVolume(halfHeight, radius);
	capsule->SetBoundingVolume((CollisionVolume*)volume);

	capsule->GetTransform()
		.SetScale(Vector3(radius* 2, halfHeight, radius * 2))
		.SetPosition(position);

	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), capsuleMesh, basicTex, basicShader));
	capsule->SetPhysicsObject(new PhysicsObject(&capsule->GetTransform(), capsule->GetBoundingVolume()));

	capsule->GetPhysicsObject()->setElasticity(0.5);

	capsule->GetPhysicsObject()->SetInverseMass(inverseMass);
	capsule->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(capsule);

	return capsule;

}

GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float elasticity, float friction, Vector4 colour, float inverseMass )
{
	GameObject* cube = new GameObject("AABB Cube");

	AABBVolume* volume = new AABBVolume(dimensions);

	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->GetRenderObject()->SetColour(colour);

	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	cube->GetPhysicsObject()->setElasticity(elasticity);
	cube->GetPhysicsObject()->SetFriction(friction);

	world->AddGameObject(cube);

	return cube;
}

GameObject* TutorialGame::AddOBBCubeToWorld(const Vector3& position, Vector3 dimensions, float elasticity, float friction, Vector4 colour, float inverseMass)
{
	GameObject* cube = new GameObject("OBB Cube");

	OBBVolume* volume = new OBBVolume(dimensions);

	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	
	//cube->GetTransform().SetOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), 50.0f));

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	cube->GetPhysicsObject()->setElasticity(elasticity);

	world->AddGameObject(cube);

	return cube;
}

void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld(position, radius, 0.8, 1.0f);
		}
	}
	AddFloorToWorld(Vector3(0, -2, 0));
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 1.0f;
	Vector3 cubeDims = Vector3(1, 1, 1);

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);

			if (rand() % 2) {
				AddCubeToWorld(position, cubeDims,0.5,0.5,  Vector4(1, 1, 1, 1));
			}
			else {
				AddSphereToWorld(position, sphereRadius,0.5, 0.8);
			}
		}
	}
}

void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
	for (int x = 1; x < numCols+1; ++x) {
		for (int z = 1; z < numRows+1; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCubeToWorld(position, cubeDims,0.5, 0.5f, Vector4(1, 1, 1, 1), 1.0);
		}
	}
}

void TutorialGame::InitDefaultFloor() {
	AddFloorToWorld(Vector3(0, -2, 0));
}

void TutorialGame::InitGameExamples() {
	//AddPlayerToWorld(Vector3(0, 5, 0));
	AddEnemyToWorld(Vector3(0, 5, 0));

	AddBonusToWorld(Vector3(5, 3, 0));

	GameObject* cube = AddOBBCubeToWorld(Vector3(10, 10, 0), Vector3(1, 1, 1), 0.5f,0.5f,Vector4(1,1,1,1), 0.3f);
	MotorConstraint* constraint = new MotorConstraint(cube);
	world->AddConstraint(constraint);

	AddOBBCubeToWorld(Vector3(15, 10, 0), Vector3(1, 1, 1), 0.5f, 0.5f, Vector4(1, 1, 1, 1));

	AddCubeToWorld(Vector3(20, 10, 0), Vector3(1, 1, 1), 0.5f,0.5f, Vector4(1, 1, 1, 1));

	AddSphereToWorld(Vector3(25, 10, 0), 2.0f, 0.5f);

	AddCapsuleToWorld(Vector3(30, 10, 0), 3.0f, 1.5f);

	AddCapsuleToWorld(Vector3(35, 10, 0), 3.0f, 1.5f);
}

GameObject* TutorialGame::AddPlayerToWorld(const Vector3& position) {
	float meshSize = 3.0f;
	float inverseMass = 0.7f;

	string name = "Player";

	PlayerGameObject* character = new PlayerGameObject(name);

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.85f, 0.3f) * meshSize);
	//CapsuleVolume* volume = new CapsuleVolume(1.0f * meshSize, 1.0f*meshSize);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	if (rand() % 2) {
		character->SetRenderObject(new RenderObject(&character->GetTransform(), charMeshA, nullptr, basicShader));
	}
	else {
		character->SetRenderObject(new RenderObject(&character->GetTransform(), charMeshB, nullptr, basicShader));
	}

	character->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));

	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	character->SetType(Players);
	character->GetPhysicsObject()->SetFriction(0.1f);

	world->AddGameObject(character);

	//lockedObject = character;
	player = character;

	return character;
}

GameObject* TutorialGame::AddEnemyToWorld(const Vector3& position) {
	float meshSize		= 3.0f;
	float inverseMass	= 0.5f;

	GameObject* character = new GameObject( "Enemy");

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);

	

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), enemyMesh, nullptr, basicShader));
	character->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));

	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	character->GetPhysicsObject()->SetFriction(0.01f);

	character->SetType(enemy);
	world->AddGameObject(character);

	return character;
}

FollowEnemy* TutorialGame::AddFollowEnemyToWorld(const Vector3& position, GameObject* target)
{
	float meshSize = 3.0f;
	float inverseMass = 0.5f;

	FollowEnemy* character = new FollowEnemy(world);

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);



	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), enemyMesh, nullptr, basicShader));
	character->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));

	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	character->setTarget(target);
	character->setPlayer(player);
	character->SetType(Players);
	world->AddGameObject(character);

	return character;
}

BehaviuorTreeEnemy* TutorialGame::AddBehaviourToWorld(const Vector3& position, GameObject* target)
{
	float meshSize = 3.0f;
	float inverseMass = 0.5f;

	BehaviuorTreeEnemy* character = new BehaviuorTreeEnemy(world);

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);



	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), enemyMesh, nullptr, basicShader));
	character->GetRenderObject()->SetColour(Vector4(1, 0, 1, 1));

	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	character->setTarget(target);
	character->SetType(Players);
	world->AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddBonusToWorld(const Vector3& position) {
	BonusGameObject* apple = new BonusGameObject();
	
	SphereVolume* volume = new SphereVolume(0.25f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(0.25, 0.25, 0.25))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), bonusMesh, nullptr, basicShader));
	apple->GetRenderObject()->SetColour(Vector4(1, 1, 0, 1));

	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(0.5f);
	apple->GetPhysicsObject()->InitSphereInertia();

	apple->SetType(bonus);

	FloatConstraint* constraint = new FloatConstraint(apple, position.y);
	world->AddConstraint(constraint);


	world->AddBonus(apple);

	return apple;
}

StateGameObject* TutorialGame::AddStateObjectToWorld(const Vector3& position)
{
	StateGameObject* apple = new StateGameObject();

	SphereVolume* volume = new SphereVolume(0.25f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(0.25, 0.25, 0.25))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), bonusMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(apple);

	return apple;
}

GameObject* TutorialGame::AddStateSphereToWorld(const Vector3& position, float radius, float elasticity, float inverseMass)
{
	StateGameObject* sphere = new StateGameObject();
	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->setElasticity(elasticity);

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	sphere->GetPhysicsObject()->SetFriction(0.1f);

	world->AddGameObject(sphere);

	return sphere;
}

MovingPlatform* TutorialGame::AddMovingPlatform(const Vector3& position)
{
	Vector3 dimensions = Vector3(10, 1, 10);
	float invMass = 0.1f;

	MovingPlatform* platform = new MovingPlatform();
	AABBVolume* volume = new AABBVolume(dimensions);

	platform->SetBoundingVolume((CollisionVolume*)volume);

	platform->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	platform->SetRenderObject(new RenderObject(&platform->GetTransform(), cubeMesh, basicTex, basicShader));
	platform->SetPhysicsObject(new PhysicsObject(&platform->GetTransform(), platform->GetBoundingVolume()));

	platform->GetPhysicsObject()->SetInverseMass(invMass);
	platform->GetPhysicsObject()->InitCubeInertia();

	platform->GetPhysicsObject()->SetFriction(2.5f);
	platform->SetType(platforms);

	world->AddGameObject(platform);

	return platform;
}

SpeedBlock* TutorialGame::AddSpeedBlock(const Vector3& position)
{
	Vector3 dimensions = Vector3(4, 1, 4);
	float invMass = 0.0f;

	SpeedBlock* cube = new SpeedBlock();

	AABBVolume* volume = new AABBVolume(dimensions);

	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetRenderObject()->SetColour(Vector4(0, 0, 1, 1));

	cube->GetPhysicsObject()->SetInverseMass(invMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	cube->GetPhysicsObject()->setElasticity(0.1);
	cube->GetPhysicsObject()->SetFriction(0.0f);


	FloatConstraint* constraint = new FloatConstraint(cube, position.y);
	world->AddConstraint(constraint);

	world->AddGameObject(cube);

	return cube;
}

/*

Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be 
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around. 

*/
bool TutorialGame::SelectObject() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::C)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode) {
		renderer->DrawString("Press C to change to camera mode!", Vector2(5, 85));

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT)) {
			if (selectionObject) {	//set colour to deselected;
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject = nullptr;
				lockedObject	= nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
				selectionObject = (GameObject*)closestCollision.node;

				Debug::DrawLine(world->GetMainCamera()->GetPosition(), closestCollision.collidedAt, Vector4(1, 1, 1, 1), 5.0f);
				selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				return true;
			}
			else {
				return false;
			}
		}
	}
	else {
		renderer->DrawString("Press C to change to select mode!", Vector2(5, 85));
	}

	if (selectionObject)
	{ 
		Vector3 Position = selectionObject->GetTransform().GetPosition();
		renderer->DrawString("Object Name : " + selectionObject->GetName(), Vector2(5, 67), Vector4(1.0f, 0.0f, 0.0f, 0.0f), 15.0f);
		renderer->DrawString("World ID : " + std::to_string(selectionObject->GetWorldID()) , Vector2(5, 70), Vector4(1.0f, 0.0f, 0.0f, 0.0f), 15.0f);
		renderer->DrawString("x:" + std::to_string(Position.x),Vector2(5, 73), Vector4(1.0f, 0.0f, 0.0f, 0.0f), 15.0f);
		renderer->DrawString("y:" + std::to_string(Position.y), Vector2(5, 75), Vector4(1.0f, 0.0f, 0.0f, 0.0f), 15.0f);
		renderer->DrawString("z:" + std::to_string(Position.z), Vector2(5, 77), Vector4(1.0f, 0.0f, 0.0f, 0.0f), 15.0f);
		//renderer->DrawString(, Vector2(5, 85));
	}

	if (lockedObject) {
		renderer->DrawString("Press L to unlock object!", Vector2(5, 80));
	}

	else if(selectionObject){
		renderer->DrawString("Press L to lock selected object object!", Vector2(5, 80));
	}

	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L)) {
		if (selectionObject) {
			if (lockedObject == selectionObject) {
				lockedObject = nullptr;
			}
			else {
				lockedObject = selectionObject;
			}
		}

	}

	return false;
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/
void TutorialGame::MoveSelectedObject() 
{
	renderer -> DrawString("Click Force :" + std::to_string(forceMagnitude),
	Vector2(10, 20)); // Draw debug text at 10 ,20
	forceMagnitude += Window::GetMouse() -> GetWheelMovement() * 100.0f;
	
	if (!selectionObject) 
	{
		return;// we haven �t selected anything !
	}
	// Push the selected object !
	if (Window::GetMouse() -> ButtonPressed(NCL::MouseButtons::RIGHT)) 
	{
		Ray ray = CollisionDetection::BuildRayFromMouse(*world -> GetMainCamera());
		RayCollision closestCollision;
		if (world -> Raycast(ray, closestCollision, true)) 
		{
			if (closestCollision.node == selectionObject) 
			{
				selectionObject -> GetPhysicsObject() ->AddForceAtPosition(ray.GetDirection() * forceMagnitude ,
																			closestCollision.collidedAt);
			}
		}
	}
}