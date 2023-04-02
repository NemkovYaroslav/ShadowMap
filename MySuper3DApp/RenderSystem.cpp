#include "RenderSystem.h"

#include "DisplayWin32.h"
#include "RenderComponent.h"
#include "Game.h"
#include "RenderShadows.h"

RenderSystem::RenderSystem()
{
	viewport = std::make_shared<D3D11_VIEWPORT>();
	viewport->TopLeftX = 0;
	viewport->TopLeftY = 0;
	viewport->Width = static_cast<float>(Game::GetInstance()->GetDisplay()->GetClientWidth());
	viewport->Height = static_cast<float>(Game::GetInstance()->GetDisplay()->GetClientHeight());
	viewport->MinDepth = 0;
	viewport->MaxDepth = 1.0f;

	D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 };

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Width = Game::GetInstance()->GetDisplay()->GetClientWidth();
	swapChainDesc.BufferDesc.Height = Game::GetInstance()->GetDisplay()->GetClientHeight();
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = Game::GetInstance()->GetDisplay()->GetHWnd();
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		featureLevel,
		1,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		swapChain.GetAddressOf(),
		device.GetAddressOf(),
		nullptr,
		context.GetAddressOf()
	);

	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	device->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderView);

	D3D11_TEXTURE2D_DESC depthTexDesc = {};
	depthTexDesc.ArraySize = 1;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.Width = Game::GetInstance()->GetDisplay()->GetClientWidth();
	depthTexDesc.Height = Game::GetInstance()->GetDisplay()->GetClientHeight();
	depthTexDesc.SampleDesc = { 1, 0 };

	device->CreateTexture2D(&depthTexDesc, nullptr, depthBuffer.GetAddressOf());
	device->CreateDepthStencilView(depthBuffer.Get(), nullptr, &depthView);
}

void RenderSystem::PrepareFrame()
{
	context->ClearState();
	context->OMSetRenderTargets(1, renderView.GetAddressOf(), depthView.Get());
	context->RSSetViewports(1, viewport.get());
	float backgroundColor[] = { 0.2f, 0.2f, 0.2f };
	context->ClearRenderTargetView(renderView.Get(), backgroundColor);
	context->ClearDepthStencilView(depthView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void RenderSystem::Draw()
{
	for (auto& renderComponent : renderComponents)
	{
		renderComponent->Draw();
	}
}

void RenderSystem::EndFrame()
{
	context->OMSetRenderTargets(0, nullptr, nullptr);
	swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
}