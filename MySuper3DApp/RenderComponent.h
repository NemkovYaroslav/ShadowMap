#pragma once
#include "Component.h"

#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace DirectX::SimpleMath;

class RenderComponent : public Component
{
public:

    RenderComponent(std::string shaderFileName, D3D_PRIMITIVE_TOPOLOGY topology);
    RenderComponent(std::string shaderFileName, std::string textureFileName, D3D_PRIMITIVE_TOPOLOGY topology);
    RenderComponent() = delete;

    virtual void Initialize() override;
    virtual void Update(float deltaTime) override;

    void Draw();
    void AddGrid(int gridSize, float cellSize, Color color);
    void AddPlane(float radius);
    void AddMesh(float scaleRate, std::string objectFileName);

    void ProcessNode(aiNode* node, const aiScene* scene, float scaleRate);
    void ProcessMesh(aiMesh* mesh, const aiScene* scene, float scaleRate);

    std::string shaderFileName;
    D3D_PRIMITIVE_TOPOLOGY topology;
    std::vector<Vector4> points;
    std::vector<int> indices;

    // вынести в render system
    Microsoft::WRL::ComPtr<ID3D11InputLayout>     inputLayout;   // с шейдером
    Microsoft::WRL::ComPtr<ID3D11Buffer>          vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>          indexBuffer;
    Microsoft::WRL::ComPtr<ID3D11VertexShader>    vertexShader;  // с шейдером
    Microsoft::WRL::ComPtr<ID3D11PixelShader>     pixelShader;   // с шейдером
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rastState;     // с шейдером
    Microsoft::WRL::ComPtr<ID3D11SamplerState>    samplerState;  // с шейдером
    // вынести в render system

    ID3D11Buffer** constBuffer;

    std::string textureFileName;
    Microsoft::WRL::ComPtr<ID3D11Resource> texture; // должна быть здесь и должно быть points и indices
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView; // это тоже
};
