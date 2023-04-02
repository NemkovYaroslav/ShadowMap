#pragma once
#include "Component.h"
#include "SimpleMath.h"

using namespace DirectX::SimpleMath;

class CollisionComponent : public Component
{
public:

	DirectX::BoundingSphere* sphereCollision;
	bool isPickedUp;

	CollisionComponent();
	virtual void Initialize() override;
	virtual void Update(float deltaTime) override;
};

