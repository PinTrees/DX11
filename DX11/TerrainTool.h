#pragma once
#include "Component.h"

class TerrainTool : public Component
{
private:
    float m_brushSize;
    float m_brushStrength;
    bool m_useHeightmap;

public:
    TerrainTool();
    ~TerrainTool();

    virtual void Awake() override;
    virtual void Update() override;
    virtual void Render() override;

    virtual void OnInspectorGUI() override;

public:
    void PaintTerrain();
    void ApplyHeightmap();

public:
    GENERATE_COMPONENT_BODY(TerrainTool)
};

REGISTER_COMPONENT(TerrainTool)
