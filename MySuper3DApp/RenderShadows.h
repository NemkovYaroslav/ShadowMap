#pragma once

#include "LightComponent.h"

class RenderShadowsComponent;

class RenderShadows
{
public:

	RenderShadows();
	void PrepareFrame();
	void Draw();
	void EndFrame();

	std::shared_ptr<D3D11_VIEWPORT> viewport;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> lightDepthBufferTexture; //
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> lightDepthBufferRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureResourceView; //

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView; //
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer; //

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState; //

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rastState;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	std::vector<RenderShadowsComponent*> renderShadowsComponents;
};