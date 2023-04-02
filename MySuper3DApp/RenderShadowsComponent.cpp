#include "RenderShadowsComponent.h"
#include "Game.h"
#include "RenderSystem.h"
#include "RenderShadows.h"
#include "LightComponent.h"
#include "GameObject.h"
#include "CameraComponent.h"

struct alignas(16) CameraLightData
{
	Matrix viewProjection;
	Matrix model;
};

/*
RenderShadowsComponent::RenderShadowsComponent(RenderComponent* currentRenderComponent)
{
	this->currentRenderComponent = currentRenderComponent;
}
*/

RenderShadowsComponent::RenderShadowsComponent()
{

}

void RenderShadowsComponent::Initialize()
{
	Game::GetInstance()->GetRenderShadowsSystem()->renderShadowsComponents.push_back(this);

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

	D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
	ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthStencilStateDesc.DepthEnable = true;
	depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
	Game::GetInstance()->GetRenderSystem()->context->OMSetDepthStencilState(Game::GetInstance()->GetRenderShadowsSystem()->depthStencilState.Get(), 0);

	D3D11_BUFFER_DESC constBufferDesc = {};
	constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufferDesc.MiscFlags = 0;
	constBufferDesc.StructureByteStride = 0;
	constBufferDesc.ByteWidth = sizeof(CameraLightData);
	Game::GetInstance()->GetRenderSystem()->device->CreateBuffer(&constBufferDesc, nullptr, constBuffer.GetAddressOf());
	std::cout << "000" << std::endl;
}

void RenderShadowsComponent::Update(float deltaTime)
{
	const CameraLightData cameraLightData
	{
		Game::GetInstance()->currentLight->GetViewMatrix() * Game::GetInstance()->currentLight->GetProjectionMatrix(),
		gameObject->transformComponent->GetModel()
	};
	D3D11_MAPPED_SUBRESOURCE firstMappedResource;
	Game::GetInstance()->GetRenderSystem()->context->Map(constBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &firstMappedResource);
	memcpy(firstMappedResource.pData, &cameraLightData, sizeof(CameraLightData));
	Game::GetInstance()->GetRenderSystem()->context->Unmap(constBuffer.Get(), 0);
	std::cout << "111" << std::endl;
}

void RenderShadowsComponent::Draw()
{
	Game::GetInstance()->GetRenderSystem()->context->RSSetState(Game::GetInstance()->GetRenderShadowsSystem()->rastState.Get());
	Game::GetInstance()->GetRenderSystem()->context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT strides[] = { 48 };
	UINT offsets[] = { 0 };
	Game::GetInstance()->GetRenderSystem()->context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), strides, offsets); //
	Game::GetInstance()->GetRenderSystem()->context->IASetInputLayout(Game::GetInstance()->GetRenderShadowsSystem()->inputLayout.Get());
	Game::GetInstance()->GetRenderSystem()->context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	Game::GetInstance()->GetRenderSystem()->context->VSSetShader(Game::GetInstance()->GetRenderShadowsSystem()->vertexShader.Get(), nullptr, 0);
	Game::GetInstance()->GetRenderSystem()->context->PSSetShader(Game::GetInstance()->GetRenderShadowsSystem()->pixelShader.Get(), nullptr, 0);
	Game::GetInstance()->GetRenderSystem()->context->GSSetShader(nullptr, nullptr, 0);

	Game::GetInstance()->GetRenderSystem()->context->VSSetConstantBuffers(0, 1, constBuffer.GetAddressOf());
	Game::GetInstance()->GetRenderSystem()->context->PSSetConstantBuffers(0, 1, constBuffer.GetAddressOf());

	Game::GetInstance()->GetRenderSystem()->context->DrawIndexed(indices.size(), 0, 0); //
}

void RenderShadowsComponent::AddGrid(int gridSize, float cellSize, Color color)
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
void RenderShadowsComponent::AddPlane(float radius)
{
	points = {
		Vector4(radius,   radius, 0.0f, 1.0f), Vector4(radius, radius, 0.0f, 0.0f), Vector4(0.0f, 0.0f, 1.0f, 0.0f),
		Vector4(-radius, -radius, 0.0f, 1.0f), Vector4(0.0f,   0.0f,   0.0f, 0.0f), Vector4(0.0f, 0.0f, 1.0f, 0.0f),
		Vector4(radius, -radius, 0.0f, 1.0f), Vector4(radius, 0.0f,   0.0f, 0.0f), Vector4(0.0f, 0.0f, 1.0f, 0.0f),
		Vector4(-radius,   radius, 0.0f, 1.0f), Vector4(0.0f,   radius, 0.0f, 0.0f), Vector4(0.0f, 0.0f, 1.0f, 0.0f)
	};
	indices = { 0, 1, 2, 1, 0, 3 };
}

void RenderShadowsComponent::AddMesh(float scaleRate, std::string objectFileName)
{
	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(objectFileName, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

	if (!pScene) { return; }

	ProcessNode(pScene->mRootNode, pScene, scaleRate);
}
void RenderShadowsComponent::ProcessNode(aiNode* node, const aiScene* scene, float scaleRate)
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
void RenderShadowsComponent::ProcessMesh(aiMesh* mesh, const aiScene* scene, float scaleRate)
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

		points.push_back({ mesh->mVertices[i].x * scaleRate, mesh->mVertices[i].y * scaleRate, mesh->mVertices[i].z * scaleRate, 1.0f });
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