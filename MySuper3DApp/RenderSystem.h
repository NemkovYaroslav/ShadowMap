#pragma once

#include "includes.h"

using namespace DirectX::SimpleMath;

class DisplayWin32;
class RenderComponent;

class RenderSystem
{
public:

	RenderSystem();

	virtual void Draw();
	virtual void EndFrame();
	virtual void PrepareFrame();

public:

	std::shared_ptr<D3D11_VIEWPORT> viewport;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthView;

	std::vector<RenderComponent*> renderComponents;
};