#pragma once
#include "GameObject.h"

class DebugObject : public GameObject
{
public:

    DebugObject();
    void CreateGrid(int gridSize, float cellSize, DirectX::SimpleMath::Color color);
};
