#include "RenderShadows.h"

#include "DisplayWin32.h"
#include "RenderComponent.h"
#include "Game.h"
#include "RenderSystem.h"
#include "RenderShadowsComponent.h"

RenderShadows::RenderShadows()
{
	viewport = std::make_shared<D3D11_VIEWPORT>();

	D3D11_TEXTURE2D_DESC textureDesc = {}; // ok
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = static_cast<float>(1024);
	textureDesc.Height = static_cast<float>(1024);
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	auto result = Game::GetInstance()->GetRenderSystem()->device->CreateTexture2D(&textureDesc, nullptr, lightDepthBufferTexture.GetAddressOf());
	assert(SUCCEEDED(result));

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc; // ok
	shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc; // ok
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	result = Game::GetInstance()->GetRenderSystem()->device->CreateShaderResourceView(lightDepthBufferTexture.Get(), &shaderResourceViewDesc, textureResourceView.GetAddressOf());
	assert(SUCCEEDED(result));
	result = Game::GetInstance()->GetRenderSystem()->device->CreateDepthStencilView(lightDepthBufferTexture.Get(), &depthStencilViewDesc, depthStencilView.GetAddressOf());
	assert(SUCCEEDED(result));

	D3D11_SAMPLER_DESC comparisonSamplerDesc;
	ZeroMemory(&comparisonSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	comparisonSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.BorderColor[0] = 1.0f;
	comparisonSamplerDesc.BorderColor[1] = 1.0f;
	comparisonSamplerDesc.BorderColor[2] = 1.0f;
	comparisonSamplerDesc.BorderColor[3] = 1.0f;
	comparisonSamplerDesc.MinLOD = 0.f;
	comparisonSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	comparisonSamplerDesc.MipLODBias = 0.f;
	comparisonSamplerDesc.MaxAnisotropy = 0;
	comparisonSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	comparisonSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
	Game::GetInstance()->GetRenderSystem()->device->CreateSamplerState(&comparisonSamplerDesc, samplerState.GetAddressOf());

	viewport->Width = static_cast<float>(1024); // ok
	viewport->Height = static_cast<float>(1024);
	viewport->MinDepth = 0.0f;
	viewport->MaxDepth = 1.0f;
	viewport->TopLeftX = 0.0f;
	viewport->TopLeftY = 0.0f;

	Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderByteCode;
	ID3DBlob* errorCode = nullptr;
	auto res = D3DCompileFromFile(
		L"../Shaders/DepthShader.hlsl",
		nullptr,
		nullptr,
		"VSMain",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		vertexShaderByteCode.GetAddressOf(),
		&errorCode
	);
	if (FAILED(res))
	{
		if (errorCode)
		{
			const char* compileErrors = (char*)(errorCode->GetBufferPointer());
			std::cout << compileErrors << std::endl;
		}
		else
		{
			std::cout << "Missing Shader File: " << std::endl;
		}
		return;
	}
	Game::GetInstance()->GetRenderSystem()->device->CreateVertexShader(
		vertexShaderByteCode->GetBufferPointer(),
		vertexShaderByteCode->GetBufferSize(),
		nullptr, vertexShader.GetAddressOf()
	);

	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		D3D11_INPUT_ELEMENT_DESC {
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		}
	};
	Game::GetInstance()->GetRenderSystem()->device->CreateInputLayout(
		inputElements,
		1,
		vertexShaderByteCode->GetBufferPointer(),
		vertexShaderByteCode->GetBufferSize(),
		inputLayout.GetAddressOf()
	);

	Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderByteCode;
	res = D3DCompileFromFile(
		L"../Shaders/DepthShader.hlsl",
		nullptr /*macros*/,
		nullptr /*include*/,
		"PSMain",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		pixelShaderByteCode.GetAddressOf(),
		&errorCode
	);
	Game::GetInstance()->GetRenderSystem()->device->CreatePixelShader(
		pixelShaderByteCode->GetBufferPointer(),
		pixelShaderByteCode->GetBufferSize(),
		nullptr, pixelShader.GetAddressOf()
	);

	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_FRONT; //
	rastDesc.FillMode = D3D11_FILL_SOLID;
	res = Game::GetInstance()->GetRenderSystem()->device->CreateRasterizerState(&rastDesc, rastState.GetAddressOf());
}

void RenderShadows::PrepareFrame()
{
	Game::GetInstance()->GetRenderSystem()->context->RSSetState(rastState.Get());
	Game::GetInstance()->GetRenderSystem()->context->OMSetRenderTargets(0, nullptr, depthStencilView.Get());
	Game::GetInstance()->GetRenderSystem()->context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	Game::GetInstance()->GetRenderSystem()->context->RSSetViewports(1, viewport.get());
}

void RenderShadows::Draw()
{
	for (auto& renderShadowsComponent : renderShadowsComponents)
	{
		renderShadowsComponent->Draw();
	}
}

void RenderShadows::EndFrame()
{
	Game::GetInstance()->GetRenderSystem()->context->OMSetRenderTargets(0, nullptr, nullptr);
}