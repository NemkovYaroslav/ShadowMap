#include "CollisionComponent.h"
#include "GameObject.h"
#include "Transform.h"

CollisionComponent::CollisionComponent()
{
	this->sphereCollision = nullptr;
	this->isPickedUp = false;
}

void CollisionComponent::Initialize()
{
	this->sphereCollision = new DirectX::BoundingSphere();
	this->sphereCollision->Center = gameObject->transformComponent->GetPosition();
	this->sphereCollision->Radius = gameObject->radius;
}

void CollisionComponent::Update(float deltaTime)
{
	if ((Game::GetInstance()->gameObjects.at(2)->collisionComponent->sphereCollision->Intersects(*(gameObject->collisionComponent->sphereCollision))
		&& (Game::GetInstance()->gameObjects.at(2) != gameObject)) && (!gameObject->collisionComponent->isPickedUp)
		&& (Game::GetInstance()->gameObjects.at(2)->collisionComponent->sphereCollision->Radius >= gameObject->maxRadius))
	{
		Vector3 pos = gameObject->transformComponent->GetPosition();
		Quaternion rot = gameObject->transformComponent->GetRotation();
		gameObject->transformComponent->parent = Game::GetInstance()->gameObjects.at(2)->transformComponent;
		gameObject->transformComponent->SetPosition(pos);
		gameObject->transformComponent->SetRotation(rot);
		Game::GetInstance()->gameObjects.at(2)->collisionComponent->sphereCollision->Radius += 0.05f;
		gameObject->collisionComponent->isPickedUp = true;
	}
	sphereCollision->Center = gameObject->transformComponent->GetPosition();
}