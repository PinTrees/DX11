#include "pch.h"
#include "FBXLoader.h"


bool FBXLoader::LoadFBX(
    const std::string& filename,
    vector<Vertex::PosNormalTexTan2>& vertices,
    vector<USHORT>& indices,
    vector<MeshGeometry::Subset>& subsets,
    vector<FbxMaterial>& materials)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        filename, 
        aiProcess_ConvertToLeftHanded |
        aiProcess_Triangulate | 
        aiProcess_GenUVCoords | 
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace 
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        printf("ERROR::ASSIMP:: %s\n", importer.GetErrorString());
        return false;
    }

    // Process materials
    materials.resize(scene->mNumMaterials);
    for (UINT i = 0; i < scene->mNumMaterials; ++i)
    {
        aiMaterial* mat = scene->mMaterials[i];
        aiString name;

        materials[i].ambient = XMFLOAT3(0.2f, 0.2f, 0.2f);
        materials[i].diffuse = XMFLOAT3(0.8f, 0.8f, 0.8f);
        materials[i].specular = XMFLOAT3(1.0f, 1.0f, 1.0f);
        materials[i].reflect = XMFLOAT3(0.0f, 0.0f, 0.0f);
        materials[i].shininess = 0.0f;

        aiColor3D color(0.f, 0.f, 0.f);
        if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_AMBIENT, color))
            materials[i].ambient = XMFLOAT3(color.r, color.g, color.b);
        if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, color))
            materials[i].diffuse = XMFLOAT3(color.r, color.g, color.b);
        if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_SPECULAR, color))
            materials[i].specular = XMFLOAT3(color.r, color.g, color.b);
        if (AI_SUCCESS == mat->Get(AI_MATKEY_SHININESS, materials[i].shininess))
            materials[i].shininess = materials[i].shininess;

        if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            mat->GetTexture(aiTextureType_DIFFUSE, 0, &name);
            materials[i].DiffuseMapName = std::wstring(name.C_Str(), name.C_Str() + name.length);
        }

        if (mat->GetTextureCount(aiTextureType_NORMALS) > 0)
        {
            mat->GetTexture(aiTextureType_NORMALS, 0, &name);
            materials[i].NormalMapName = std::wstring(name.C_Str(), name.C_Str() + name.length);
        }
    }

    // Process nodes (and meshes)
    ProcessNode(scene->mRootNode, scene, vertices, indices, subsets);

    return true;
}

bool FBXLoader::LoadFBX(
    const std::string& filename,
    vector<Vertex::PosNormalTexTanSkinned>& vertices,
    vector<USHORT>& indices,
    vector<MeshGeometry::Subset>& subsets,
    vector<FbxMaterial>& materials,
    SkinnedData& skinInfo)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        printf("ERROR::ASSIMP:: %s\n", importer.GetErrorString());
        return false;
    }

    // Process materials
    materials.resize(scene->mNumMaterials);
    for (UINT i = 0; i < scene->mNumMaterials; ++i)
    {
        aiMaterial* mat = scene->mMaterials[i];
        aiString name;

        materials[i].ambient = XMFLOAT3(0.2f, 0.2f, 0.2f);
        materials[i].diffuse = XMFLOAT3(0.8f, 0.8f, 0.8f);
        materials[i].specular = XMFLOAT3(1.0f, 1.0f, 1.0f);
        materials[i].reflect = XMFLOAT3(0.0f, 0.0f, 0.0f);
        materials[i].shininess = 0.0f;

        aiColor3D color(0.f, 0.f, 0.f);
        if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_AMBIENT, color))
            materials[i].ambient = XMFLOAT3(color.r, color.g, color.b);
        if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, color))
            materials[i].diffuse = XMFLOAT3(color.r, color.g, color.b);
        if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_SPECULAR, color))
            materials[i].specular = XMFLOAT3(color.r, color.g, color.b);
        if (AI_SUCCESS == mat->Get(AI_MATKEY_SHININESS, materials[i].shininess))
            materials[i].shininess = materials[i].shininess;

        if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            mat->GetTexture(aiTextureType_DIFFUSE, 0, &name);
            materials[i].DiffuseMapName = std::wstring(name.C_Str(), name.C_Str() + name.length);
        }

        if (mat->GetTextureCount(aiTextureType_NORMALS) > 0)
        {
            mat->GetTexture(aiTextureType_NORMALS, 0, &name);
            materials[i].NormalMapName = std::wstring(name.C_Str(), name.C_Str() + name.length);
        }
    }

    // Process nodes (and meshes)
    ProcessNodeSkinned(scene->mRootNode, scene, vertices, indices, subsets);

    return true;
}


void FBXLoader::ProcessNode(
    aiNode* node,
    const aiScene* scene, 
    vector<Vertex::PosNormalTexTan2>& vertices, 
    vector<USHORT>& indices,
    vector<MeshGeometry::Subset>& subsets)
{
    for (UINT i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        MeshGeometry::Subset subset; 
        ProcessMesh(mesh, scene, vertices, indices, subset); 
        subsets.push_back(subset); 
    }

    for (UINT i = 0; i < node->mNumChildren; ++i)
    {
        ProcessNode(node->mChildren[i], scene, vertices, indices, subsets);
    }
}

void FBXLoader::ProcessMesh(
    aiMesh* mesh,
    const aiScene* scene,
    vector<Vertex::PosNormalTexTan2>& vertices,
    vector<USHORT>& indices,
    MeshGeometry::Subset& subset)
{
    subset.Name = mesh->mName.C_Str();
    subset.VertexStart = vertices.size();
    subset.FaceStart = indices.size() / 3;
    subset.VertexCount = mesh->mNumVertices;
    subset.FaceCount = mesh->mNumFaces;

    for (UINT i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex::PosNormalTexTan2 vertex;
        vertex.pos = XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.normal = XMFLOAT3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

        // UV
        if (mesh->HasTextureCoords(0))
            ::memcpy(&vertex.tex, &mesh->mTextureCoords[0][i], sizeof(Vec2));

        //vertex.tangentU = XMFLOAT4(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z, 1.0f);

        vertices.push_back(vertex);
    }

    for (UINT i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for (UINT j = 0; j < face.mNumIndices; ++j)
        {
            indices.push_back(face.mIndices[j]);
        }
    }
}

void FBXLoader::ProcessNodeSkinned(
    aiNode* node, 
    const aiScene* scene,
    vector<Vertex::PosNormalTexTanSkinned>& vertices,
    vector<USHORT>& indices, 
    vector<MeshGeometry::Subset>& subsets)
{
    for (UINT i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        MeshGeometry::Subset subset;
        ProcessMeshSkinned(mesh, scene, vertices, indices, subset);
        subsets.push_back(subset);
    }

    for (UINT i = 0; i < node->mNumChildren; ++i)
    {
        ProcessNodeSkinned(node->mChildren[i], scene, vertices, indices, subsets);
    }
}


void FBXLoader::ProcessMeshSkinned(
    aiMesh* mesh, 
    const aiScene* scene, 
    vector<Vertex::PosNormalTexTanSkinned>& vertices, 
    vector<USHORT>& indices,
    MeshGeometry::Subset& subset)
{
    subset.VertexStart = vertices.size();
    subset.FaceStart = indices.size() / 3;
    subset.VertexCount = mesh->mNumVertices;
    subset.FaceCount = mesh->mNumFaces;

    for (UINT i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex::PosNormalTexTanSkinned vertex;
        vertex.pos = XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.normal = XMFLOAT3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        vertex.tex = XMFLOAT2(mesh->mTextureCoords[0] ? mesh->mTextureCoords[0][i].x : 0.0f,
            mesh->mTextureCoords[0] ? mesh->mTextureCoords[0][i].y : 0.0f);
        vertex.tangentU = XMFLOAT4(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z, 1.0f);

        // Initialize bone indices and weights to zero
        memset(vertex.boneIndices, 0, sizeof(vertex.boneIndices));
        vertex.weights = XMFLOAT3(0.0f, 0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    for (UINT i = 0; i < mesh->mNumBones; ++i)
    {
        aiBone* bone = mesh->mBones[i];
        int boneID = i;

        for (UINT j = 0; j < bone->mNumWeights; ++j)
        {
            UINT vertexID = bone->mWeights[j].mVertexId;
            float weight = bone->mWeights[j].mWeight;

            // Assign the bone weight and index to the vertex
            for (int k = 0; k < 4; ++k)
            {
                if (vertices[vertexID].weights.x == 0.0f)
                {
                    vertices[vertexID].weights.x = weight;
                    vertices[vertexID].boneIndices[k] = (BYTE)boneID;
                    break;
                }
                else if (vertices[vertexID].weights.y == 0.0f)
                {
                    vertices[vertexID].weights.y = weight;
                    vertices[vertexID].boneIndices[k] = (BYTE)boneID;
                    break;
                }
                else if (vertices[vertexID].weights.z == 0.0f)
                {
                    vertices[vertexID].weights.z = weight;
                    vertices[vertexID].boneIndices[k] = (BYTE)boneID;
                    break;
                }
            }
        }
    }

    for (UINT i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for (UINT j = 0; j < face.mNumIndices; ++j)
        {
            indices.push_back(face.mIndices[j]);
        }
    }
}