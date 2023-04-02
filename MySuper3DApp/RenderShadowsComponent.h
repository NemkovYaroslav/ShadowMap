#pragma once
#include "Component.h"
#include "RenderComponent.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace DirectX::SimpleMath;

class RenderShadowsComponent : public Component
{
public:

    D3D_PRIMITIVE_TOPOLOGY topology;
    std::string textureFileName;

    //RenderComponent* currentRenderComponent;

    Microsoft::WRL::ComPtr<ID3D11Buffer> constBuffer;

    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
    std::vector<Vector4> points;
    
    std::vector<int> indices;

    void AddGrid(int gridSize, float cellSize, Color color);
    void AddPlane(float radius);
    void AddMesh(float scaleRate, std::string objectFileName);

    void ProcessNode(aiNode* node, const aiScene* scene, float scaleRate);
    void ProcessMesh(aiMesh* mesh, const aiScene* scene, float scaleRate);

    //RenderShadowsComponent(RenderComponent* currentRenderComponent);
    RenderShadowsComponent();

    virtual void Initialize() override;
    virtual void Update(float deltaTime) override;

    void Draw();

    //
};

