#include "pch.h"
#include "FBXLoader.h"
struct VertexKey
{
    XMFLOAT3 pos;       // Position
    XMFLOAT3 normal;    // Normal
    XMFLOAT2 tex;       // Texture coordinates
    XMFLOAT4 tangent;   // Tangent
    BYTE color[4];      // Color (R, G, B, A)

    bool operator==(const VertexKey& other) const
    {
        return pos.x == other.pos.x && pos.y == other.pos.y && pos.z == other.pos.z &&
            normal.x == other.normal.x && normal.y == other.normal.y && normal.z == other.normal.z &&
            tex.x == other.tex.x && tex.y == other.tex.y &&
            tangent.x == other.tangent.x && tangent.y == other.tangent.y &&
            tangent.z == other.tangent.z && tangent.w == other.tangent.w &&
            color[0] == other.color[0] && color[1] == other.color[1] &&
            color[2] == other.color[2] && color[3] == other.color[3];
    }
};

struct VertexKeyHasher
{
    std::size_t operator()(const VertexKey& key) const
    {
        std::size_t h1 = std::hash<float>()(key.pos.x) ^ std::hash<float>()(key.pos.y) ^ std::hash<float>()(key.pos.z);
        std::size_t h2 = std::hash<float>()(key.normal.x) ^ std::hash<float>()(key.normal.y) ^ std::hash<float>()(key.normal.z);
        std::size_t h3 = std::hash<float>()(key.tex.x) ^ std::hash<float>()(key.tex.y);
        std::size_t h4 = std::hash<float>()(key.tangent.x) ^ std::hash<float>()(key.tangent.y) ^ std::hash<float>()(key.tangent.z) ^ std::hash<float>()(key.tangent.w);
        std::size_t h5 = std::hash<BYTE>()(key.color[0]) ^ std::hash<BYTE>()(key.color[1]) ^ std::hash<BYTE>()(key.color[2]) ^ std::hash<BYTE>()(key.color[3]);

        return h1 ^ h2 ^ h3 ^ h4 ^ h5;
    }
};


// �ߺ��� ���� �����ϴ� �Լ�
void OptimizeVertices(std::vector<Vertex::PosNormalTexTanSkinned>& vertices, std::vector<USHORT>& indices)
{
    // ���� �ߺ� üũ�� ���� �� (VertexKey, UINT)
    std::unordered_map<VertexKey, UINT, VertexKeyHasher> uniqueVertices;
    std::vector<Vertex::PosNormalTexTanSkinned> optimizedVertices;
    std::vector<USHORT> optimizedIndices;

    for (const auto& index : indices)
    {
        // ���� �ε����� ���� ��������
        Vertex::PosNormalTexTanSkinned& vertex = vertices[index];

        // VertexKey ���� - �񱳸� ���� �ֿ� �ʵ� ����
        VertexKey key;
        key.pos = vertex.pos;
        key.normal = vertex.normal;
        key.tex = vertex.tex;
        key.tangent = vertex.tangentU;
        //std::memcpy(key.color, vertex.color, sizeof(BYTE) * 4);  // color ����

        // ������ ã�ų�, ã�� ���ϸ� ���� �߰�
        auto it = uniqueVertices.find(key);
        if (it == uniqueVertices.end())
        {
            // ������ ���ٸ� �߰��ϰ�, ���ο� �ε����� �ο�
            UINT newIndex = static_cast<UINT>(optimizedVertices.size());
            uniqueVertices[key] = newIndex;
            optimizedVertices.push_back(vertex);
            optimizedIndices.push_back(newIndex);
        }
        else
        {
            // �̹� �ִ� �����̶�� �ش� �ε��� ���
            optimizedIndices.push_back(it->second);
        }
    }

    // ����ȭ�� ������ �ε��� ����Ʈ�� ��ü
    vertices = optimizedVertices;
    indices = optimizedIndices;
}


bool FBXLoader::LoadModelFbx(const std::string& filename, MeshFile* model)
{
    Assimp::Importer importer; 
    //const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
    const aiScene* scene = importer.ReadFile( 
        filename
        , aiProcess_ConvertToLeftHanded
        | aiProcessPreset_TargetRealtime_MaxQuality 
        | 0
    ); 

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        printf("ERROR::ASSIMP:: %s\n", importer.GetErrorString());
        return false;
    }

    if (scene->mRootNode)
    {
        ParsingMeshNode(scene->mRootNode, scene, model);
    }
    ParseAnimations(scene, model->SkinnedData); 
    return true;
}

bool FBXLoader::LoadAnimation(const std::string& filename, SkinnedData& skinnedData)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        filename,
        aiProcessPreset_TargetRealtime_Fast | 
        aiProcess_LimitBoneWeights |
        aiProcess_ConvertToLeftHanded |
        aiProcess_Triangulate
    );

    if (!scene || !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->HasAnimations())
    {
        printf("ERROR::ASSIMP:: %s\n", importer.GetErrorString());
        return false;
    }

    map<string, int> boneMapping;
    ParseBonesFromNodes(scene->mRootNode, boneMapping); 

    skinnedData.BoneHierarchy.resize(boneMapping.size(), -1); 
    ParseBoneHierarchy(scene->mRootNode, boneMapping, skinnedData.BoneHierarchy, -1); 
    ParseBoneOffsets(scene, boneMapping, skinnedData.BoneOffsets);

    // �ִϸ��̼��� �Ľ�
    for (UINT animIndex = 0; animIndex < scene->mNumAnimations; ++animIndex)
    {
        aiAnimation* animation = scene->mAnimations[animIndex];
        AnimationClip animationClip;
        animationClip.Name = animation->mName.C_Str(); 
        
        for (UINT channelIndex = 0; channelIndex < animation->mNumChannels; ++channelIndex)
        {
            aiNodeAnim* nodeAnim = animation->mChannels[channelIndex];
            string boneName = nodeAnim->mNodeName.C_Str();

            BoneAnimation boneAnim;

            // Ű�������� �Ľ�
            for (UINT keyIndex = 0; keyIndex < nodeAnim->mNumPositionKeys; ++keyIndex)
            {
                Keyframe keyframe;
                keyframe.TimePos = (float)nodeAnim->mPositionKeys[keyIndex].mTime;

                keyframe.Translation = XMFLOAT3(
                    nodeAnim->mPositionKeys[keyIndex].mValue.x,
                    nodeAnim->mPositionKeys[keyIndex].mValue.y,
                    nodeAnim->mPositionKeys[keyIndex].mValue.z
                );

                // ���� ������� Scale �� Rotation Ű������ �߰�
                keyframe.Scale = XMFLOAT3(
                    nodeAnim->mScalingKeys[keyIndex].mValue.x,
                    nodeAnim->mScalingKeys[keyIndex].mValue.y,
                    nodeAnim->mScalingKeys[keyIndex].mValue.z
                );

                keyframe.RotationQuat = XMFLOAT4(
                    nodeAnim->mRotationKeys[keyIndex].mValue.x,
                    nodeAnim->mRotationKeys[keyIndex].mValue.y,
                    nodeAnim->mRotationKeys[keyIndex].mValue.z,
                    nodeAnim->mRotationKeys[keyIndex].mValue.w
                );

                boneAnim.Keyframes.push_back(keyframe);
            }

            // �� �ִϸ��̼��� �ִϸ��̼� Ŭ���� �߰�
            animationClip.BoneAnimations.push_back(boneAnim);
        }
        //animationClip.Duration = static_cast<float>(animation->mDuration);
        //animationClip.TicksPerSecond = static_cast<float>(animation->mTicksPerSecond ? animation->mTicksPerSecond : 25.0);

        // SkinnedData�� �ִϸ��̼� Ŭ�� �߰�
        skinnedData.AnimationClips.push_back(animationClip);
    }

    return false;
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
            //OptimizeVertices(mesh->Vertices, mesh->Indices); 
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

        if (mesh->HasTangentsAndBitangents()) 
            vertex.tangentU = XMFLOAT4( 
                mesh->mTangents[i].x, 
                mesh->mTangents[i].y, 
                mesh->mTangents[i].z, 
                1.0f 
            );

        if (mesh->HasTextureCoords(0))
            ::memcpy(&vertex.tex, &mesh->mTextureCoords[0][i], sizeof(Vec2));
        
        // Color Parsing (�÷� ������ �Ľ�)
        //if (mesh->HasVertexColors(0))
        //{
        //    aiColor4D color = mesh->mColors[0][i];
        //    vertex.color[0] = static_cast<BYTE>(color.r * 255.0f); // Red
        //    vertex.color[1] = static_cast<BYTE>(color.g * 255.0f); // Green
        //    vertex.color[2] = static_cast<BYTE>(color.b * 255.0f); // Blue
        //    vertex.color[3] = static_cast<BYTE>(color.a * 255.0f); // Alpha
        //}
        //else
        //{
        //    vertex.color[0] = 255; // Red
        //    vertex.color[1] = 255; // Green
        //    vertex.color[2] = 255; // Blue
        //    vertex.color[3] = 255; // Alpha 
        //}

        memset(vertex.boneIndices, 0, sizeof(vertex.boneIndices));
        vertex.weights = XMFLOAT3(0.0f, 0.0f, 0.0f);
        vertices.push_back(vertex); // �� ���ؽ� �߰�
    }

    for (UINT i = 0; i < mesh->mNumBones; ++i)
    {
        aiBone* bone = mesh->mBones[i];
        int boneID = i;
    
        for (UINT j = 0; j < bone->mNumWeights; ++j)
        {
            UINT vertexID = bone->mWeights[j].mVertexId; // �������� ���� �ùٸ� ���ؽ� ����  
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

    // �ε��� �߰�
    for (UINT i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for (UINT j = 0; j < face.mNumIndices; ++j)
        {
            indices.push_back(face.mIndices[j]); // ���ؽ��� �������� ���Ͽ� �ùٸ� �ε��� ���� 
        }
    }
}

void FBXLoader::ParseBonesFromNodes(aiNode* node, std::map<std::string, int>& boneMapping)
{
    std::string nodeName(node->mName.C_Str());

    if (boneMapping.find(nodeName) == boneMapping.end())
    {
        int boneIndex = static_cast<int>(boneMapping.size());  
        boneMapping[nodeName] = boneIndex;
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        ParseBonesFromNodes(node->mChildren[i], boneMapping); 
    }
}

void FBXLoader::ParseBoneOffsets(const aiScene* scene, const std::map<std::string, int>& boneMapping, std::vector<XMFLOAT4X4>& boneOffsets)
{
    boneOffsets.resize(boneMapping.size(), XMFLOAT4X4()); // �⺻ ���� ��ķ� �ʱ�ȭ

    // ��� ���� ���� ������ ��� ����
    for (const auto& [boneName, boneIndex] : boneMapping)
    {
        aiNode* boneNode = scene->mRootNode->FindNode(boneName.c_str());
        if (boneNode)
        {
            aiMatrix4x4 offsetMatrix = boneNode->mTransformation;
            XMMATRIX mat = XMLoadFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&offsetMatrix));

            // ��� ��ȯ �� ����
            XMStoreFloat4x4(&boneOffsets[boneIndex], XMMatrixTranspose(mat));
        }
        else
        {
            // �� ��尡 ���ٸ� �⺻ ���� ��� ���
            XMStoreFloat4x4(&boneOffsets[boneIndex], XMMatrixIdentity());
        }
    }
}

void FBXLoader::ParseBoneHierarchy(aiNode* node, std::map<std::string, int>& boneMapping, std::vector<int>& boneHierarchy, int parentIndex)
{
    // ���� ��尡 ������ Ȯ�� 
    auto it = boneMapping.find(node->mName.C_Str()); 
    int currentIndex = it != boneMapping.end() ? it->second : -1; 

    // ���� Ȯ�εǸ� �θ�-�ڽ� ���� ����
    if (currentIndex != -1) 
    {
        boneHierarchy[currentIndex] = parentIndex; 
    }

    // �ڽ� ��忡 ���� ��������� ȣ��
    for (unsigned int i = 0; i < node->mNumChildren; ++i) 
    {
        ParseBoneHierarchy(node->mChildren[i], boneMapping, boneHierarchy, currentIndex); 
    }
}

void FBXLoader::ParseAnimations(const aiScene* scene, SkinnedData& skinnedData)
{
    // ������ �ִϸ��̼� Ŭ���� ��ȸ�մϴ�.
    for (UINT animIndex = 0; animIndex < scene->mNumAnimations; ++animIndex)
    {
        aiAnimation* aiAnimation = scene->mAnimations[animIndex];
        std::string animName = aiAnimation->mName.C_Str();

        // �ִϸ��̼� �̸����� Ŭ���� ����ϴ�.
        AnimationClip clip;  
        clip.Name = animName; 
        //clip.duration = static_cast<float>(aiAnimation->mDuration); 
        //clip.ticksPerSecond = static_cast<float>(aiAnimation->mTicksPerSecond); 
        //
        //// �� �ִϸ��̼� Ŭ���� ���� �� �ִϸ��̼� �����͸� �Ľ��մϴ�. 
        ReadAnimationNode(aiAnimation, skinnedData);  
        //
        //// Ŭ���� SkinnedData�� ����
        skinnedData.AnimationClips.push_back(clip); 
    }
}

void FBXLoader::ReadAnimationNode(const aiAnimation* aiAnimation, SkinnedData& skinnedData)
{
    // aiAnimation�� �ִϸ��̼� Ŭ���� ��Ÿ���ϴ�.
    //for (UINT channelIndex = 0; channelIndex < aiAnimation->mNumChannels; ++channelIndex)
    //{
    //    aiNodeAnim* aiChannel = aiAnimation->mChannels[channelIndex];
    //    std::string boneName = aiChannel->mNodeName.C_Str();
    //
    //    BoneAnimation boneAnim;
    //
    //    // 1. Position KeyFrames (��ġ Ű������)
    //    for (UINT i = 0; i < aiChannel->mNumPositionKeys; ++i)
    //    {
    //        aiVectorKey positionKey = aiChannel->mPositionKeys[i];
    //        BoneKeyframe keyframe;
    //        keyframe.timePos = static_cast<float>(positionKey.mTime);
    //        keyframe.translation = XMFLOAT3(positionKey.mValue.x, positionKey.mValue.y, positionKey.mValue.z);
    //        boneAnim.keyframes.push_back(keyframe);
    //    }
    //
    //    // 2. Rotation KeyFrames (ȸ�� Ű������)
    //    for (UINT i = 0; i < aiChannel->mNumRotationKeys; ++i)
    //    {
    //        aiQuatKey rotationKey = aiChannel->mRotationKeys[i];
    //        BoneKeyframe keyframe;
    //        keyframe.timePos = static_cast<float>(rotationKey.mTime);
    //        keyframe.rotationQuat = XMFLOAT4(rotationKey.mValue.x, rotationKey.mValue.y, rotationKey.mValue.z, rotationKey.mValue.w);
    //        boneAnim.keyframes.push_back(keyframe);
    //    }
    //
    //    // 3. Scale KeyFrames (������ Ű������)
    //    for (UINT i = 0; i < aiChannel->mNumScalingKeys; ++i)
    //    {
    //        aiVectorKey scalingKey = aiChannel->mScalingKeys[i];
    //        BoneKeyframe keyframe;
    //        keyframe.timePos = static_cast<float>(scalingKey.mTime);
    //        keyframe.scale = XMFLOAT3(scalingKey.mValue.x, scalingKey.mValue.y, scalingKey.mValue.z);
    //        boneAnim.keyframes.push_back(keyframe);
    //    }
    //
    //    // ���������� ������ boneAnim�� skinnedData�� ����
    //    skinnedData._animations[aiChannel->mNodeName.C_Str()] = boneAnim;
    //}
}
