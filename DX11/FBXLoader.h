#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Vertex.h"
#include "MeshGeometry.h"
#include "SkinnedData.h"

class MeshFile;

struct FbxMaterial
{
    Material Mat;

    XMFLOAT3 ambient;
    XMFLOAT3 diffuse;
    XMFLOAT3 specular;
    XMFLOAT3 reflect;
    float shininess;

    wstring DiffuseMapName;
    wstring NormalMapName;
};

struct Subset
{
    UINT id;
    UINT vertexStart;
    UINT vertexCount;
    UINT faceStart;
    UINT faceCount;
};

class FBXLoader
{
public:
    bool LoadFBX(const std::string& filename,
        std::vector<Vertex::PosNormalTexTan2>& vertices,
        std::vector<USHORT>& indices,
        std::vector<MeshGeometry::Subset>& subsets,
        std::vector<FbxMaterial>& mats);

    bool LoadFBX(
        const std::string& filename,
        vector<Vertex::PosNormalTexTanSkinned>& vertices,
        vector<USHORT>& indices,
        vector<MeshGeometry::Subset>& subsets,
        vector<FbxMaterial>& materials,
        SkinnedData& skinInfo);

    bool LoadModelFbx( 
        const std::string& filename, 
        MeshFile* skinnedModel);

private:
    void ParsingMeshNode(
        aiNode* node,
        const aiScene* scene,
        MeshFile* model); 

    void ProcessNode(
        aiNode* node,
        const aiScene* scene,
        vector<Vertex::PosNormalTexTan2>& vertices,
        vector<USHORT>& indices, vector<MeshGeometry::Subset>& subsets);

    void ProcessMesh(
        aiMesh* mesh,
        const aiScene* scene,
        vector<Vertex::PosNormalTexTan2>& vertices,
        vector<USHORT>& indices, MeshGeometry::Subset& subset);

    void ProcessMeshSkinned(
        aiMesh* mesh,
        const aiScene* scene,
        vector<Vertex::PosNormalTexTanSkinned>& vertices, 
        vector<USHORT>& indices, MeshGeometry::Subset& subset);
};