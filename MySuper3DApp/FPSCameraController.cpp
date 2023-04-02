#include "FPSCameraController.h"
#include "GameObject.h"
#include "Transform.h"

FPSCameraControllerComponent::FPSCameraControllerComponent()
{
	this->cameraSpeed = 3.0f;
}

void FPSCameraControllerComponent::Update(float deltaTime)
{
	TransformComponent* transform = gameObject->transformComponent;
	if (Game::GetInstance()->GetInputDevice()->IsKeyDown(Keys::E))
	{
		std::cout << " X: " << transform->GetPosition().x << " Y: " << transform->GetPosition().y << " Z: " << transform->GetPosition().z << std::endl;
	}
	if (Game::GetInstance()->GetInputDevice()->IsKeyDown(Keys::A))
	{
		transform->SetPosition(transform->GetPosition() + Vector3(cameraSpeed * deltaTime, 0, 0));
	}
	if (Game::GetInstance()->GetInputDevice()->IsKeyDown(Keys::D))
	{
		transform->SetPosition(transform->GetPosition() + Vector3( - cameraSpeed * deltaTime, 0, 0));
	}
	if (Game::GetInstance()->GetInputDevice()->IsKeyDown(Keys::W))
	{
		transform->SetPosition(transform->GetPosition() + Vector3(0, 0, cameraSpeed * deltaTime));
	}
	if (Game::GetInstance()->GetInputDevice()->IsKeyDown(Keys::S))
	{
		transform->SetPosition(transform->GetPosition() + Vector3(0, 0, - cameraSpeed * deltaTime));
	}
	if (Game::GetInstance()->GetInputDevice()->IsKeyDown(Keys::Space))
	{
		transform->SetPosition(transform->GetPosition() + Vector3(0, cameraSpeed * deltaTime, 0));
	}
	if (Game::GetInstance()->GetInputDevice()->IsKeyDown(Keys::LeftShift))
	{
		transform->SetPosition(transform->GetPosition() + Vector3(0, - cameraSpeed * deltaTime, 0));
	}
}

void FPSCameraControllerComponent::MouseEventHandler(const InputDevice::MouseMoveEventArgs& mouseData)
{
}
