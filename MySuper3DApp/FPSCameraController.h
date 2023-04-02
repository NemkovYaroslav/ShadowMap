#pragma once
#include "includes.h"
#include "InputDevice.h"
#include "Component.h"

using namespace DirectX::SimpleMath;

class FPSCameraControllerComponent : public Component
{
public:

	float cameraSpeed;

	FPSCameraControllerComponent();
	virtual void Update(float deltaTime) override;

	void MouseEventHandler(const InputDevice::MouseMoveEventArgs& mouseData);
};
