#include "pch.h"
#include "TerrainTool.h"
#include "GameObject.h"
#include "EditorGUI.h"

TerrainTool::TerrainTool() : m_brushSize(10.0f), m_brushStrength(10.0f), m_useHeightmap(false)
{
    m_InspectorTitleName = "TerrainTool";
}

TerrainTool::TerrainTool(const TerrainTool& other) : m_brushSize(10.0f), m_brushStrength(10.0f), m_useHeightmap(false)
{
}

TerrainTool::~TerrainTool()
{
}

bool TerrainTool::Initialize(ID3D11Device* device, int terrainWidth, int terrainHeight)
{
    // ������ �ʺ�� ���̸� ����
    m_terrainWidth = terrainWidth;
    m_terrainHeight = terrainHeight;

    // ������ ���� ���� ��Ʈ���� �����ϴ� ���� �� �ε��� ���۸� �ʱ�ȭ
    return InitializeBuffers(device);
}

// heightmap
bool TerrainTool::Initialize(ID3D11Device* device, const char* heightmapFilename)
{
    // ������ ���� ���� �ε�
    if (!LoadHeightmap(heightmapFilename))
        return false;

    // ���� ���� ���̸� ǥ��ȭ
    NormalizeHeightmap();

    // ������ ���� ���� ��Ʈ���� �����ϴ� ���� �� �ε��� ���۸� �ʱ�ȭ
    return InitializeBuffers(device);
}

void TerrainTool::TerrainRender(ID3D11DeviceContext* deviceContext)
{
    // ���������ο� ������ �ε��� ���� ����
    RenderBuffers(deviceContext);
}

void TerrainTool::Shutdown()
{
    ShutdownBuffers();
    ShutdownHeightmap();
}

int TerrainTool::GetIndexCount()
{
    return m_indexCount;
}

bool TerrainTool::LoadHeightmap(const char* filename)
{
    // ���̳ʸ� ���� ����Ʈ�� ����
    FILE* filePtr = nullptr;
    if (fopen_s(&filePtr, filename, "rb") != 0)
        return false;

    // ���� ��� ����
    BITMAPFILEHEADER bitmapFileHeader;
    if (fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr) != 1)
        return false;

    // ��Ʈ�� ���� ��� ����
    BITMAPINFOHEADER bitmapInfoHeader;
    if (fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr) != 1)
        return false;

    // ������ ũ�⸦ ����
    m_terrainWidth = bitmapInfoHeader.biWidth;
    m_terrainHeight = bitmapInfoHeader.biHeight;

    // ��Ʈ�� �̹��� �������� ũ�⸦ ���
    int imageSize = m_terrainWidth * m_terrainHeight * 3;

    // ��Ʈ�� �̹��� �����Ϳ� �޸𸮸� �Ҵ�
    unsigned char* bitmapImage = new unsigned char[imageSize];
    if (!bitmapImage)
    {
        return false;
    }

    // ��Ʈ�� �������� ���� �κ����� �̵�
    fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

    // ��Ʈ�� �̹��� ������ ����
    if (fread(bitmapImage, 1, imageSize, filePtr) != imageSize)
        return false;

    // ������ �ݽ��ϴ�.
    if (fclose(filePtr) != 0)
        return false;

    // ���� �� �����͸� ������ ����ü
    m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
    if (!m_heightMap)
        return false;

    // ���� ��ġ �ʱ�ȭ
    int k = 0;

    // ����Ʈ�� �ε�
    for (int j = 0; j < m_terrainHeight; j++)
    {
        for (int i = 0; i < m_terrainWidth; i++)
        {
            unsigned char height = bitmapImage[k];

            int index = (m_terrainHeight * j) + i;

            m_heightMap[index].x = (float)i;
            m_heightMap[index].y = (float)height;
            m_heightMap[index].z = (float)j;

            k += 3;
        }
    }

    // �迭 ����
    delete[] bitmapImage;
    bitmapImage = 0;

    return true;
}

void TerrainTool::NormalizeHeightmap()
{
    for (int j = 0; j < m_terrainHeight; j++)
    {
        for (int i = 0; i < m_terrainWidth; i++)
        {
            m_heightMap[(m_terrainHeight * j) + i].y /= 15.0f;
        }
    }
}

void TerrainTool::ShutdownHeightmap()
{
    if (m_heightMap)
    {
        delete[] m_heightMap;
        m_heightMap = 0;
    }
}

bool TerrainTool::InitializeBuffers(ID3D11Device* device)
{
    // ���� �޽��� ���� ���� ���
    m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 8;

    // �ε��� ���� ������ ���� ���� ����
    m_indexCount = m_vertexCount;

    // ���� �迭
    VertexType* vertices = new VertexType[m_vertexCount];
    if (!vertices)
        return false;

    // �ε��� �迭
    unsigned long* indices = new unsigned long[m_indexCount];
    if (!indices)
        return false;

    // ���� �迭�� ���� �ε����� �ʱ�ȭ
    int index = 0;

    // ���� �����ͷ� ���� �� �ε��� �迭�� �ε�
    for (int j = 0; j < (m_terrainHeight - 1); j++)
    {
        for (int i = 0; i < (m_terrainWidth - 1); i++)
        {
            // LINE 1
            // ���� ��
            float positionX = (float)i;
            float positionZ = (float)(j + 1);

            vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
            vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            indices[index] = index;
            index++;

            // ������ ��
            positionX = (float)(i + 1);
            positionZ = (float)(j + 1);

            vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
            vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            indices[index] = index;
            index++;

            // LINE 2
            // ������ ��
            positionX = (float)(i + 1);
            positionZ = (float)(j + 1);

            vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
            vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            indices[index] = index;
            index++;

            // ������ �Ʒ�
            positionX = (float)(i + 1);
            positionZ = (float)j;

            vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
            vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            indices[index] = index;
            index++;

            // LINE 3
            // ������ �Ʒ�
            positionX = (float)(i + 1);
            positionZ = (float)j;

            vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
            vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            indices[index] = index;
            index++;

            // ���� �Ʒ�
            positionX = (float)i;
            positionZ = (float)j;

            vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
            vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            indices[index] = index;
            index++;

            // LINE 4
            // ���� �Ʒ�
            positionX = (float)i;
            positionZ = (float)j;

            vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
            vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            indices[index] = index;
            index++;

            // ���� ��
            positionX = (float)i;
            positionZ = (float)(j + 1);

            vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);
            vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            indices[index] = index;
            index++;
        }
    }

    // ���� ���� ������ ����ü�� ����
    D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // subresource ������ ���� �����Ϳ� ���� �����͸� ����
    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // ���� ����
    // if (CHECK(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
    //    return false;

    // ���� �ε��� ������ ����ü�� ����
    D3D11_BUFFER_DESC indexBufferDesc;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // ���� ���ҽ� ������ �ε��� �����Ϳ� ���� �����͸� ����
    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // �ε��� ����
    if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
        return false;

    // �迭 ����
    delete[] vertices;
    vertices = 0;

    delete[] indices;
    indices = 0;

    return true;
}

bool TerrainTool::InitializeHeightmapBuffers(ID3D11Device* device)
{
    // ���� �޽��� ���� ���� ���
    m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 12;

    // �ε��� ���� ������ ���� ���� ����
    m_indexCount = m_vertexCount;

    // ���� �迭
    VertexType* vertices = new VertexType[m_vertexCount];
    if (!vertices)
    {
        return false;
    }

    // �ε��� �迭
    unsigned long* indices = new unsigned long[m_indexCount];
    if (!indices)
    {
        return false;
    }

    // ���� �迭�� ���� �ε����� �ʱ�ȭ
    int index = 0;

    // ���� �����ͷ� ���� �� �ε��� �迭�� �ε�
    for (int j = 0; j < (m_terrainHeight - 1); j++)
    {
        for (int i = 0; i < (m_terrainWidth - 1); i++)
        {
            int index1 = (m_terrainHeight * j) + i;          // ���� �Ʒ�
            int index2 = (m_terrainHeight * j) + (i + 1);      // ������ �Ʒ�
            int index3 = (m_terrainHeight * (j + 1)) + i;      // ���� ��
            int index4 = (m_terrainHeight * (j + 1)) + (i + 1);  // ������ ��

            // ���� ��
            vertices[index].position = XMFLOAT3(m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z);
            vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            indices[index] = index;
            index++;

            // ������ ��
            vertices[index].position = XMFLOAT3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
            vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            indices[index] = index;
            index++;

            // ������ ��
            vertices[index].position = XMFLOAT3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
            vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            indices[index] = index;
            index++;

            // ���� �Ʒ�
            vertices[index].position = XMFLOAT3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
            vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            indices[index] = index;
            index++;

            // ���� �Ʒ�
            vertices[index].position = XMFLOAT3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
            vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            indices[index] = index;
            index++;

            // ���� ��
            vertices[index].position = XMFLOAT3(m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z);
            vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            indices[index] = index;
            index++;

            // ���� �Ʒ�
            vertices[index].position = XMFLOAT3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
            vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            indices[index] = index;
            index++;

            // ������ ��
            vertices[index].position = XMFLOAT3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
            vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            indices[index] = index;
            index++;

            // ������ ��
            vertices[index].position = XMFLOAT3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
            vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            indices[index] = index;
            index++;

            // ������ �Ʒ�
            vertices[index].position = XMFLOAT3(m_heightMap[index2].x, m_heightMap[index2].y, m_heightMap[index2].z);
            vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            indices[index] = index;
            index++;

            // ������ �Ʒ�
            vertices[index].position = XMFLOAT3(m_heightMap[index2].x, m_heightMap[index2].y, m_heightMap[index2].z);
            vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            indices[index] = index;
            index++;

            // ���� �Ʒ�
            vertices[index].position = XMFLOAT3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
            vertices[index].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            indices[index] = index;
            index++;
        }
    }

    // ���� ���� ������ ����ü�� ����
    D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // subresource ������ ���� �����Ϳ� ���� �����͸� ����
    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // ���� ����
    //if (CHECK(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
    //    return false;

    // ���� �ε��� ������ ����ü�� ����
    D3D11_BUFFER_DESC indexBufferDesc;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // ���� ���ҽ� ������ �ε��� �����Ϳ� ���� �����͸� ����
    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // �ε��� ����
    //if (CHECK(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
    //    return false;

    // �迭 ����
    delete[] vertices;
    vertices = 0;

    delete[] indices;
    indices = 0;

    return true;
}

void TerrainTool::ShutdownBuffers()
{
    // �ε��� ���۸� ����
    if (m_indexBuffer)
    {
        m_indexBuffer->Release();
        m_indexBuffer = 0;
    }

    // ���ؽ� ���۸� ����
    if (m_vertexBuffer)
    {
        m_vertexBuffer->Release();
        m_vertexBuffer = 0;
    }
}

void TerrainTool::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    // ���� ���� ���� �� �������� ����
    unsigned int stride = sizeof(VertexType);
    unsigned int offset = 0;

    // ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ ����
    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ ����
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // ���θ���Ʈ
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

void TerrainTool::Awake()
{

}

void TerrainTool::Update()
{
    //if (m_useHeightmap)
    //    ApplyHeightmap();
    //else
    //    PaintTerrain();
}

void TerrainTool::Render()
{
    TerrainRender(Application::GetI()->GetDeviceContext());
}

void TerrainTool::OnInspectorGUI()
{
    // brush �� ����
    EditorGUI::LabelHeader("Brush");
    
    if (EditorGUI::FloatField("Brush Size", m_brushSize))
    {
        m_brushSize = max(m_brushSize, 0.001f);
        m_brushSize = min(m_brushSize, 999);
    }
    if (EditorGUI::FloatField("Brush Strength", m_brushStrength))
    {
        m_brushStrength = max(m_brushStrength, 0.001f);
        m_brushStrength = min(m_brushStrength, 999);
    }
    
    // heightmap
    EditorGUI::LabelHeader("Heightmap");

    if (EditorGUI::BoolField("Use Heightmap", m_useHeightmap))
    {

    }

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
