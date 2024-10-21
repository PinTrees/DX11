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

    return true;
}

bool FBXLoader::LoadModelFbx(const std::string& filename, MeshFile* skinnedModel)
{
    Assimp::Importer importer; 
    //const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
    const aiScene* scene = importer.ReadFile( 
        filename, 
        aiProcess_ConvertToLeftHanded |  
        aiProcess_Triangulate |  
        aiProcess_GenUVCoords |  
        aiProcess_GenNormals |  
        aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType 
    ); 

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        printf("ERROR::ASSIMP:: %s\n", importer.GetErrorString());
        return false;
    }

    if (scene->mRootNode)
    {
        ParsingMeshNode(scene->mRootNode, scene, skinnedModel);
    }

    return true;
}

void FBXLoader::ParsingMeshNode(aiNode* node, const aiScene* scene, MeshFile* model)
{
    if (node->mNumMeshes > 0) 
    {
        if (scene->mMeshes[node->mMeshes[0]]->HasBones())
        {
            SkinnedMesh* mesh = new SkinnedMesh; 
            model->SkinnedMeshs.push_back(mesh); 
            mesh->Name = node->mName.C_Str();   
            mesh->Mat.resize(scene->mNumMaterials);  
            for (UINT i = 0; i < scene->mNumMaterials; ++i) 
            {
                aiMaterial* mat = scene->mMaterials[i]; 
                aiString name; 
            
                mesh->Mat[i].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
                mesh->Mat[i].Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
                mesh->Mat[i].Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
                mesh->Mat[i].Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
            
                aiColor3D color(0.f, 0.f, 0.f);
                if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_AMBIENT, color))
                    mesh->Mat[i].Ambient = XMFLOAT4(color.r, color.g, color.b, 1.0f);
                if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, color))
                    mesh->Mat[i].Diffuse = XMFLOAT4(color.r, color.g, color.b, 1.0f);
                if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_SPECULAR, color))
                    mesh->Mat[i].Specular = XMFLOAT4(color.r, color.g, color.b, 1.0f);
            }

            mesh->Vertices.clear(); 
            for (UINT i = 0; i < node->mNumMeshes; ++i) 
            {
                aiMesh* aiMesh = scene->mMeshes[node->mMeshes[i]];

                MeshGeometry::Subset subset; 
                ProcessMeshSkinned(aiMesh, scene, mesh->Vertices, mesh->Indices, subset); 
                mesh->Subsets.push_back(subset);
            }

            mesh->Setup(); 
        }
        else
        {
            Mesh* mesh = new Mesh;
            model->Meshs.push_back(mesh);
            mesh->Name = node->mName.C_Str();
            mesh->Mat.resize(scene->mNumMaterials);
            for (UINT i = 0; i < scene->mNumMaterials; ++i)
            {
                aiMaterial* mat = scene->mMaterials[i];
                aiString name;

                mesh->Mat[i].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
                mesh->Mat[i].Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
                mesh->Mat[i].Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
                mesh->Mat[i].Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

                aiColor3D color(0.f, 0.f, 0.f);
                if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_AMBIENT, color))
                    mesh->Mat[i].Ambient = XMFLOAT4(color.r, color.g, color.b, 1.0f);
                if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, color))
                    mesh->Mat[i].Diffuse = XMFLOAT4(color.r, color.g, color.b, 1.0f);
                if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_SPECULAR, color))
                    mesh->Mat[i].Specular = XMFLOAT4(color.r, color.g, color.b, 1.0f);
            }

            for (uint32 i = 0; i < node->mNumMeshes; ++i) 
            {
                uint32 index = node->mMeshes[i];
                aiMesh* aiMesh = scene->mMeshes[index]; 

                MeshGeometry::Subset subset;
                ProcessMesh(aiMesh, scene, mesh->Vertices, mesh->Indices, subset);
                mesh->Subsets.push_back(subset);
            }
            mesh->Setup();
        }
    }

    for (UINT i = 0; i < node->mNumChildren; ++i)
    {
        ParsingMeshNode(node->mChildren[i], scene, model);
    }
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
    subset.MaterialIndex = mesh->mMaterialIndex;


    for (UINT i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex::PosNormalTexTan2 vertex;
        vertex.pos = XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        // Normal
        if (mesh->HasNormals())
            ::memcpy(&vertex.normal, &mesh->mNormals[i], sizeof(XMFLOAT3));  

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

void FBXLoader::ProcessMeshSkinned(
    aiMesh* mesh, 
    const aiScene* scene, 
    vector<Vertex::PosNormalTexTanSkinned>& vertices, 
    vector<USHORT>& indices,
    MeshGeometry::Subset& subset)
{
    subset.Name = mesh->mName.C_Str(); 
    subset.VertexStart = vertices.size();
    subset.FaceStart = indices.size() / 3; 
    subset.VertexCount = mesh->mNumVertices; 
    subset.FaceCount = mesh->mNumFaces;
    subset.MaterialIndex = mesh->mMaterialIndex; 

    const uint32 startVertex = vertices.size();

    for (uint32 i = 0; i < mesh->mNumVertices; ++i) 
    {
        Vertex::PosNormalTexTanSkinned vertex;
        vertex.pos = XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        // Normal
        if (mesh->HasNormals())
            ::memcpy(&vertex.normal, &mesh->mNormals[i], sizeof(XMFLOAT3));

        if (mesh->HasTextureCoords(0))
            ::memcpy(&vertex.tex, &mesh->mTextureCoords[0][i], sizeof(Vec2));
        
        //vertex.tangentU = XMFLOAT4(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z, 1.0f);

        memset(vertex.boneIndices, 0, sizeof(vertex.boneIndices));
        vertex.weights = XMFLOAT3(0.0f, 0.0f, 0.0f);
        vertices.push_back(vertex); // 새 버텍스 추가
    }

    for (UINT i = 0; i < mesh->mNumBones; ++i)
    {
        aiBone* bone = mesh->mBones[i];
        int boneID = i;
    
        for (UINT j = 0; j < bone->mNumWeights; ++j)
        {
            UINT vertexID = bone->mWeights[j].mVertexId; // 오프셋을 더해 올바른 버텍스 참조  
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

    // 인덱스 추가
    for (UINT i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for (UINT j = 0; j < face.mNumIndices; ++j)
        {
            indices.push_back(face.mIndices[j]); // 버텍스의 오프셋을 더하여 올바른 인덱스 참조 
        }
    }
}