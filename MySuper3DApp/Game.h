#pragma once
#include "includes.h"

class DisplayWin32;
class GameObject;
class RenderSystem;
class InputDevice;
class CameraComponent;
class TPSCameraController;
class LightComponent;
class RenderShadows;

class Game
{
private:

	LPCWSTR	name;
	int clientWidth;
	int clientHeight;

	std::shared_ptr<std::chrono::time_point<std::chrono::steady_clock>> startTime;
	std::shared_ptr<std::chrono::time_point<std::chrono::steady_clock>> prevTime;
	HRESULT res;
	float totalTime;
	float deltaTime;
	unsigned int frameCount;

protected:

	Game(LPCWSTR name, int clientWidth, int clientHeight);
	static Game* instance;

	std::shared_ptr<DisplayWin32> display;
	std::shared_ptr<RenderSystem> render;
	std::shared_ptr<RenderShadows> renderShadows;
	std::shared_ptr<InputDevice>  inputDevice;

	virtual void DestroyResources();
	virtual void Draw();
	virtual void Initialize();
	virtual void PrepareResources();
	virtual void Update();
	virtual void UpdateInternal();

public:

	CameraComponent* currentCamera;

	LightComponent* currentLight;

	std::vector<GameObject*> gameObjects;

	Game(const Game&) = delete;
	void operator = (const Game&) = delete;
	virtual ~Game() = default;

	static Game* CreateInstance(LPCWSTR name, int screenWidth, int screenHeight);
	static Game* GetInstance();

	void Run();

	void AddGameObject(GameObject* gameObject);

	std::shared_ptr<RenderSystem>  GetRenderSystem();
	std::shared_ptr<RenderShadows> GetRenderShadowsSystem();
	std::shared_ptr<DisplayWin32>  GetDisplay();
	std::shared_ptr<InputDevice>   GetInputDevice();

	LRESULT MessageHandler(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);