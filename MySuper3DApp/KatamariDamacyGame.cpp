#include "KatamariDamacyGame.h"
#include "GameObject.h"
#include "CameraComponent.h"
#include "CameraController.h"
#include "FPSCameraController.h"
#include "KatamariControllerComponent.h"
#include "CollisionComponent.h"
#include "LightComponent.h"
#include "RenderShadows.h"

KatamariDamacyGame::KatamariDamacyGame(LPCWSTR name, int clientWidth, int clientHeight) : Game(name, clientWidth, clientHeight)
{
	Game::CreateInstance(name, clientWidth, clientHeight);
	Initialize();
}

void KatamariDamacyGame::Initialize()
{
	GameObject* ground = new GameObject();
	ground->CreatePlane(25.0f, "../Textures/moon.jpg");
	ground->transformComponent->SetRotation(Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Right, -DirectX::XM_PIDIV2));

	GameObject* katamari = new GameObject();
	katamari->CreateMesh(50.0f, "../Textures/katamari0.png", "../Models/katamari0.obj");
	katamari->transformComponent->SetPosition(Vector3(0, 1, 0));
	katamari->radius = 1.0f;
	katamari->maxRadius = katamari->radius;
	KatamariControllerComponent* katamariController = new KatamariControllerComponent();
	katamariController->katamariSpeed = 5.0f;
	katamari->AddComponent(katamariController);
	CollisionComponent* katamariCollision = new CollisionComponent();
	katamari->collisionComponent = katamariCollision;
	katamari->transformComponent->SetPosition(Vector3(0.0f, katamari->radius, 15.0f));
	katamari->AddComponent(katamariCollision);

	GameObject* camera = new GameObject();
	CameraComponent* cameraComponent = new CameraComponent();
	CameraArmControllerComponent* armCameraController = new CameraArmControllerComponent();
	camera->AddComponent(cameraComponent);
	armCameraController->aim = katamari->transformComponent;
	camera->AddComponent(armCameraController);
	Game::GetInstance()->currentCamera = cameraComponent;
	katamariController->cameraTransform = camera->transformComponent;

	GameObject* removeLight = new GameObject();
	LightComponent* lightComponent = new LightComponent();
	removeLight->AddComponent(lightComponent);
	Game::GetInstance()->currentLight = lightComponent;
	//removeLight->CreateMesh(0.1f, "../Textures/ground.jpg", "../Models/arrow.obj");
	removeLight->transformComponent->SetRotation(Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Right, DirectX::XM_PIDIV2));
	removeLight->transformComponent->SetPosition(Vector3(0.0f, 10.0f, 0.0f));

	Game::GetInstance()->AddGameObject(ground);      // 0
	Game::GetInstance()->AddGameObject(camera);      // 1
	Game::GetInstance()->AddGameObject(katamari);    // 2
	Game::GetInstance()->AddGameObject(removeLight); // 3

	GameObject* bigBoy = new GameObject();
	bigBoy->CreateMesh(2.0f, "../Textures/SpaceMan.png", "../Models/SpaceMan.fbx");
	bigBoy->radius = 1.0f;
	bigBoy->maxRadius = 1.0f + 0.05f * 20 - 0.001f;
	CollisionComponent* objectCollisionSpaceShip = new CollisionComponent(); bigBoy->collisionComponent = objectCollisionSpaceShip;
	bigBoy->AddComponent(objectCollisionSpaceShip); bigBoy->transformComponent->SetPosition(Vector3(0.0f, 2.8f, 0.0f));
	bigBoy->transformComponent->SetRotation(Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Right, DirectX::XM_PIDIV2) * Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Up, DirectX::XM_PI));
	bigBoy->transformComponent->SetPosition(Vector3(0, bigBoy->radius + 1.1f, 0));
	Game::GetInstance()->AddGameObject(bigBoy);

	GameObject* objectN;
	CollisionComponent* objectCollisionN;
	for (int i = 0; i < 10; i++)
	{
		objectN = new GameObject();
		objectN->CreateMesh(0.5f, "../Textures/spacemem.png", "../Models/spacemem.fbx");  objectN->radius = 0.5f;
		objectN->maxRadius = 0.5f;
		objectCollisionN = new CollisionComponent();
		objectN->collisionComponent = objectCollisionN;
		objectN->AddComponent(objectCollisionN);
		objectN->transformComponent->SetRotation(Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Right, DirectX::XM_PIDIV2) * Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Up, DirectX::XM_PIDIV2));
		objectN->transformComponent->SetPosition(Vector3(objectN->radius * i * 3 + objectN->radius * 5, objectN->radius - 0.5f, 0));
		Game::GetInstance()->AddGameObject(objectN);
	}
	for (int i = 0; i < 10; i++) {
		objectN = new GameObject();
		objectN->CreateMesh(0.5f, "../Textures/spacemem.png", "../Models/spacemem.fbx");
		objectN->radius = 0.5f;
		objectN->maxRadius = 0.5f;
		objectCollisionN = new CollisionComponent();
		objectN->collisionComponent = objectCollisionN;
		objectN->AddComponent(objectCollisionN);
		objectN->transformComponent->SetRotation(Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Right, DirectX::XM_PIDIV2) * Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Down, DirectX::XM_PIDIV2));
		objectN->transformComponent->SetPosition(Vector3(-objectN->radius * i * 3 - objectN->radius * 5, objectN->radius - 0.5f, 0));
		Game::GetInstance()->AddGameObject(objectN);
	}
}

void KatamariDamacyGame::Run()
{
	Game::GetInstance()->Run();
}