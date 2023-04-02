#include "DebugObject.h"
#include "RenderComponent.h"

DebugObject::DebugObject() {}

void DebugObject::CreateGrid(int gridSize, float cellSize, DirectX::SimpleMath::Color color)
{
	renderComponent = new RenderComponent("../Shaders/MyVeryFirstShader.hlsl", D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	AddComponent(renderComponent);
	renderComponent->AddGrid(gridSize, cellSize, color);
}