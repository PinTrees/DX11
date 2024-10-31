#include "pch.h"
#include "TerrainTool.h"
#include "GameObject.h"
#include "EditorGUI.h"

TerrainTool::TerrainTool() : m_brushSize(10.0f), m_brushStrength(10.0f), m_useHeightmap(false)
{
    m_InspectorTitleName = "TerrainTool";
}

TerrainTool::~TerrainTool()
{
}

void TerrainTool::Awake()
{

}

void TerrainTool::Update()
{
    if (m_useHeightmap)
        ApplyHeightmap();
    else
        PaintTerrain();
}

void TerrainTool::Render()
{

}

void TerrainTool::OnInspectorGUI()
{
    // brush °ª ¼³Á¤
    EditorGUI::LabelHeader("Brush");
    
    if (EditorGUI::FloatField("Brush Size", m_brushSize))
    {
        m_brushSize = max(m_brushSize, 0.001f);
        m_brushSize = min(m_brushSize, 9999);
    }
    if (EditorGUI::FloatField("Brush Strength", m_brushStrength))
    {
        m_brushStrength = max(m_brushStrength, 0.001f);
        m_brushStrength = min(m_brushStrength, 9999);
    }
    
    // heightmap
    EditorGUI::LabelHeader("Heightmap");

    if (EditorGUI::BoolField("Use Heightmap", m_useHeightmap))
    {

    }

}

void TerrainTool::PaintTerrain()
{

}

void TerrainTool::ApplyHeightmap()
{

}

GENERATE_COMPONENT_FUNC_TOJSON(TerrainTool)
{
    json j = {};

    SERIALIZE_TYPE(j, TerrainTool);

    SERIALIZE_FLOAT(j, m_brushSize, "brushSize");
    SERIALIZE_FLOAT(j, m_brushStrength, "brushStrength");
    SERIALIZE_BOOL(j, m_useHeightmap, "m_UseHeightmap");

    return j;
}

GENERATE_COMPONENT_FUNC_FROMJSON(TerrainTool)
{
    DE_SERIALIZE_FLOAT(j, m_brushSize, "brushSize");
    DE_SERIALIZE_FLOAT(j, m_brushStrength, "brushStrength");
    DE_SERIALIZE_BOOL(j, m_useHeightmap, "m_UseHeightmap");
}
