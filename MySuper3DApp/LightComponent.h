#pragma once

#include "Transform.h"
#include "Component.h"

using namespace DirectX;

class LightComponent : public Component
{
public:

	virtual void Initialize() override;

	XMMATRIX GetViewMatrix();
	XMMATRIX GetProjectionMatrix();

	float viewWidth;
	float viewHeight;
	float nearZ;
	float farZ;
};