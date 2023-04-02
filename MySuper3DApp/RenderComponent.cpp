#include "RenderComponent.h"

#include <WICTextureLoader.h>
#include "Game.h"
#include "DisplayWin32.h"
#include "CameraComponent.h"
#include "RenderSystem.h"
#include "GameObject.h"
#include "RenderShadows.h"

RenderComponent::RenderComponent(std::string shaderFileName, D3D_PRIMITIVE_TOPOLOGY topology) : Component()
{
	this->shaderFileName = shaderFileName;
	this->topology = topology;
}

RenderComponent::RenderComponent(std::string shaderFileName, std::string textureFileName, D3D_PRIMITIVE_TOPOLOGY topology) : Component()
{
	this->textureFileName = textureFileName;
	this->shaderFileName = shaderFileName;
	this->topology = topology;
}
struct alignas(16) CameraData
{
	Matrix viewProjection;
	Matrix model;
	Vector3 cameraPosition;
};
struct RemLightData
{
	Vector4 Direction { Vector3(0, -0.5, 0) };
	Vector4 Ambient   { Vector3(0.5f, 0.5f, 0.5f) };
	Vector4 Diffuse   { Vector3(0.2f, 0.5f, 0.9f) };
	Vector4 Specular  { Vector3(1.0f, 1.0f, 1.0f) };
};
struct alignas(16) LightData
{
	RemLightData RemLight;
};

struct alignas(16) ShadowData
{
	Matrix lightViewProjection;
	Vector3 lightCameraPosition;
};

void RenderComponent::Initialize()
{

	constBuffer = new ID3D11Buffer* [3];

	Game::GetInstance()->GetRenderSystem()->renderComponents.push_back(this);

	std::wstring fileName(shaderFileName.begin(), shaderFileName.end());

	ID3DBlob* errorCode = nullptr;

	Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderByteCode;
	auto res = D3DCompileFromFile(
		fileName.c_str(),
		nullptr /*macros*/,
		nullptr /*include*/,
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
			std::cout << "Missing Shader File: " << shaderFileName << std::endl;
		}
		return;
	}	
	Game::GetInstance()->GetRenderSystem()->device->CreateVertexShader(
		vertexShaderByteCode->GetBufferPointer(),
		vertexShaderByteCode->GetBufferSize(),
		nullptr, vertexShader.GetAddressOf()
	);

	Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderByteCode;
	res = D3DCompileFromFile(
		fileName.c_str(),
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

	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		D3D11_INPUT_ELEMENT_DESC {
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
		D3D11_INPUT_ELEMENT_DESC {
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
		D3D11_INPUT_ELEMENT_DESC {
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		}
	};
	Game::GetInstance()->GetRenderSystem()->device->CreateInputLayout(
		inputElements,
		3,
		vertexShaderByteCode->GetBufferPointer(),
		vertexShaderByteCode->GetBufferSize(),
		inputLayout.GetAddressOf()
	);

	D3D11_BUFFER_DESC vertexBufDesc = {};
	vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.CPUAccessFlags = 0;
	vertexBufDesc.MiscFlags = 0;
	vertexBufDesc.StructureByteStride = 0;
	vertexBufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * std::size(points);
	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = points.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	Game::GetInstance()->GetRenderSystem()->device->CreateBuffer(&vertexBufDesc, &vertexData, vertexBuffer.GetAddressOf());

	D3D11_BUFFER_DESC indexBufDesc = {};
	indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufDesc.CPUAccessFlags = 0;
	indexBufDesc.MiscFlags = 0;
	indexBufDesc.StructureByteStride = 0;
	indexBufDesc.ByteWidth = sizeof(int) * std::size(indices);
	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	Game::GetInstance()->GetRenderSystem()->device->CreateBuffer(&indexBufDesc, &indexData, indexBuffer.GetAddressOf());

	std::wstring textureName(textureFileName.begin(), textureFileName.end());

	res = DirectX::CreateWICTextureFromFile(
		Game::GetInstance()->GetRenderSystem()->device.Get(),
		Game::GetInstance()->GetRenderSystem()->context.Get(),
		textureName.c_str(),
		texture.GetAddressOf(),
		textureView.GetAddressOf()
	);

	D3D11_SAMPLER_DESC samplerStateDesc = {};
	samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	res = Game::GetInstance()->GetRenderSystem()->device->CreateSamplerState(&samplerStateDesc, samplerState.GetAddressOf());

	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	Game::GetInstance()->GetRenderSystem()->device->CreateRasterizerState(&rastDesc, rastState.GetAddressOf());

	D3D11_BUFFER_DESC firstConstBufferDesc = {};
	firstConstBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	firstConstBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	firstConstBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	firstConstBufferDesc.MiscFlags = 0;
	firstConstBufferDesc.StructureByteStride = 0;
	firstConstBufferDesc.ByteWidth = sizeof(CameraData);
	Game::GetInstance()->GetRenderSystem()->device->CreateBuffer(&firstConstBufferDesc, nullptr, &constBuffer[0]);

	D3D11_BUFFER_DESC secondConstBufferDesc = {};
	secondConstBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	secondConstBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	secondConstBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	secondConstBufferDesc.MiscFlags = 0;
	secondConstBufferDesc.StructureByteStride = 0;
	secondConstBufferDesc.ByteWidth = sizeof(LightData);
	Game::GetInstance()->GetRenderSystem()->device->CreateBuffer(&secondConstBufferDesc, nullptr, &constBuffer[1]);
		
	D3D11_BUFFER_DESC thirdConstBufferDesc = {};
	thirdConstBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	thirdConstBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	thirdConstBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	thirdConstBufferDesc.MiscFlags = 0;
	thirdConstBufferDesc.StructureByteStride = 0;
	thirdConstBufferDesc.ByteWidth = sizeof(ShadowData);
	Game::GetInstance()->GetRenderSystem()->device->CreateBuffer(&thirdConstBufferDesc, nullptr, &constBuffer[2]);
}

void RenderComponent::Update(float deltaTime)
{	
	const CameraData cameraData
	{
		Game::GetInstance()->currentCamera->gameObject->transformComponent->GetView() * Game::GetInstance()->currentCamera->GetProjection(),
		gameObject->transformComponent->GetModel(),
		Game::GetInstance()->currentCamera->gameObject->transformComponent->GetPosition()
	};
	D3D11_MAPPED_SUBRESOURCE firstMappedResource;
	Game::GetInstance()->GetRenderSystem()->context->Map(constBuffer[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &firstMappedResource);
	memcpy(firstMappedResource.pData, &cameraData, sizeof(CameraData));
	Game::GetInstance()->GetRenderSystem()->context->Unmap(constBuffer[0], 0);

	const LightData lightData
	{
		RemLightData {}
	};
	D3D11_MAPPED_SUBRESOURCE secondMappedResource;
	Game::GetInstance()->GetRenderSystem()->context->Map(constBuffer[1], 0, D3D11_MAP_WRITE_DISCARD, 0, &secondMappedResource);
	memcpy(secondMappedResource.pData, &lightData, sizeof(LightData));
	Game::GetInstance()->GetRenderSystem()->context->Unmap(constBuffer[1], 0);

	const ShadowData lightShadowData
	{
		Game::GetInstance()->currentLight->GetViewMatrix() * Game::GetInstance()->currentLight->GetProjectionMatrix(),
		Game::GetInstance()->currentLight->gameObject->transformComponent->GetPosition()
	};
	D3D11_MAPPED_SUBRESOURCE thirdMappedResource;
	Game::GetInstance()->GetRenderSystem()->context->Map(constBuffer[2], 0, D3D11_MAP_WRITE_DISCARD, 0, &thirdMappedResource);
	memcpy(thirdMappedResource.pData, &lightShadowData, sizeof(ShadowData));
	Game::GetInstance()->GetRenderSystem()->context->Unmap(constBuffer[2], 0);
}

void RenderComponent::Draw()
{
	Game::GetInstance()->GetRenderSystem()->context->PSSetShaderResources(0, 1, textureView.GetAddressOf());
	Game::GetInstance()->GetRenderSystem()->context->PSSetSamplers(0, 1, samplerState.GetAddressOf());

	// передаем шадоу мапу
	Game::GetInstance()->GetRenderSystem()->context->PSSetShaderResources(1, 1, Game::GetInstance()->GetRenderShadowsSystem()->textureResourceView.GetAddressOf());
	Game::GetInstance()->GetRenderSystem()->context->PSSetSamplers(1, 1, Game::GetInstance()->GetRenderShadowsSystem()->samplerState.GetAddressOf());

	Game::GetInstance()->GetRenderSystem()->context->RSSetState(rastState.Get());
	Game::GetInstance()->GetRenderSystem()->context->IASetInputLayout(inputLayout.Get());
	Game::GetInstance()->GetRenderSystem()->context->IASetPrimitiveTopology(topology);
	Game::GetInstance()->GetRenderSystem()->context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	UINT strides[] { 48 };
	UINT offsets[] { 0 };
	Game::GetInstance()->GetRenderSystem()->context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), strides, offsets);
	Game::GetInstance()->GetRenderSystem()->context->VSSetShader(vertexShader.Get(), nullptr, 0);
	Game::GetInstance()->GetRenderSystem()->context->PSSetShader(pixelShader.Get(), nullptr, 0);
	
	Game::GetInstance()->GetRenderSystem()->context->VSSetConstantBuffers(0, 3, constBuffer);
	Game::GetInstance()->GetRenderSystem()->context->PSSetConstantBuffers(0, 3, constBuffer);
	
	Game::GetInstance()->GetRenderSystem()->context->DrawIndexed(indices.size(), 0, 0);
}

void RenderComponent::AddGrid(int gridSize, float cellSize, Color color)
{
	int firstPointIndex = points.size() / 2;
	int nPoints = gridSize * 2 + 1;
	float offset = -(nPoints / 2) * cellSize;
	for (int i = 0; i < nPoints; i++)
	{
		for (int j = 0; j < nPoints; j++)
		{
			points.push_back(Vector4(cellSize * i + offset, 0, cellSize * j + offset, 1));
			points.push_back(color);

			if (i == nPoints / 2 && j == nPoints / 2)
			{
				continue;
			}
			if (j < nPoints - 1)
			{
				indices.push_back(firstPointIndex + i * nPoints + j);
				indices.push_back(firstPointIndex + i * nPoints + j + 1);
			}
			if (i < nPoints - 1)
			{
				indices.push_back(firstPointIndex + i * nPoints + j);
				indices.push_back(firstPointIndex + i * nPoints + j + nPoints);
			}
		}
	}
}
void RenderComponent::AddPlane(float radius)
{
	points = {
		Vector4(   radius,   radius, 0.0f, 1.0f), Vector4(radius, radius, 0.0f, 0.0f), Vector4(0.0f, 0.0f, 1.0f, 0.0f),
		Vector4( - radius, - radius, 0.0f, 1.0f), Vector4(0.0f,   0.0f,   0.0f, 0.0f), Vector4(0.0f, 0.0f, 1.0f, 0.0f),
		Vector4(   radius, - radius, 0.0f, 1.0f), Vector4(radius, 0.0f,   0.0f, 0.0f), Vector4(0.0f, 0.0f, 1.0f, 0.0f),
		Vector4( - radius,   radius, 0.0f, 1.0f), Vector4(0.0f,   radius, 0.0f, 0.0f), Vector4(0.0f, 0.0f, 1.0f, 0.0f)
	};
	indices = { 0, 1, 2, 1, 0, 3 };
}

void RenderComponent::AddMesh(float scaleRate, std::string objectFileName)
{
	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(objectFileName, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

	if (!pScene) { return; }

	ProcessNode(pScene->mRootNode, pScene, scaleRate);
}
void RenderComponent::ProcessNode(aiNode* node, const aiScene* scene, float scaleRate)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		ProcessMesh(mesh, scene, scaleRate);
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, scaleRate);
	}
}
void RenderComponent::ProcessMesh(aiMesh* mesh, const aiScene* scene, float scaleRate)
{
	for (UINT i = 0; i < mesh->mNumVertices; i++) {
		DirectX::SimpleMath::Vector4 textureCoordinate = {};

		if (mesh->mTextureCoords[0])
		{
			textureCoordinate.x = (float)mesh->mTextureCoords[0][i].x;
			textureCoordinate.y = (float)mesh->mTextureCoords[0][i].y;
			textureCoordinate.z = 0;
			textureCoordinate.w = 0;
		}

		DirectX::XMFLOAT4 normal;
		normal.x = mesh->mNormals[i].x;
		normal.y = mesh->mNormals[i].y;
		normal.z = mesh->mNormals[i].z;
		normal.w = 0;

		points.push_back({ mesh->mVertices[i].x * scaleRate, mesh->mVertices[i].y * scaleRate, mesh->mVertices[i].z * scaleRate, 1.0f});
		points.push_back(textureCoordinate);
		points.push_back(normal);
	}

	for (UINT i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}
}