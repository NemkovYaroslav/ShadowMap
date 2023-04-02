#pragma once
#include "includes.h"

class TransformComponent;
class RenderComponent;
class RenderShadowsComponent;
class CollisionComponent;
class Component;

using namespace DirectX::SimpleMath;

class GameObject
{
public:

	TransformComponent* transformComponent;
	RenderComponent* renderComponent;
	RenderShadowsComponent* renderShadowsComponent;
	CollisionComponent* collisionComponent;

	std::vector<Component*> components;

	float radius;
	float maxRadius = 0;

	GameObject(GameObject* parent = nullptr);
	~GameObject();

	virtual void Update(float deltaTime);
	virtual void Initialize();

	void AddComponent(Component* component);

	void CreatePlane(float radius, std::string textureFileName);
	void CreateMesh(float scaleRate, std::string textureFileName, std::string objectFileName);
};