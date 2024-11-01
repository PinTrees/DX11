#include "pch.h"
#include "Scene.h"
#include "InstancingBuffer.h"
#include "MathHelper.h"

Scene::Scene()
	: m_vecRootGameObjects(),
    m_arrGameObjects{},
    m_ScenePath(L"")
{
}

Scene::~Scene()
{
    for (const auto& g : m_vecRootGameObjects)
        g->OnDestroy(); 

	Safe_Delete_Vec(m_vecRootGameObjects);
    m_arrGameObjects[0].clear();
}

void Scene::Enter()
{
    for (auto& gameObject : m_arrGameObjects[0])
    {
        for (auto& component : gameObject->GetComponents())
        {
            component->Awake();
        }
    }

    PhysicsManager::GetI()->Start();
}

void Scene::Exit()
{
    PhysicsManager::GetI()->Exit(); 
}

void Scene::RenderScene()
{
    if (RenderManager::GetI()->InstancingMode) // if���� Editor���� �ν��Ͻ��� ����� �������� ���� bool�� ������ �ν��Ͻ� ��뿩�� �Ǵ�
    {
        map<InstanceID, vector<GameObject*>> cache;

        for (const auto& gameObject : m_arrGameObjects[0])
        {
            for (auto& component : gameObject->GetComponents())
            {

                // ������Ʈ ���� MeshRenderer���� üũ
                shared_ptr<MeshRenderer> meshRenderer = dynamic_pointer_cast<MeshRenderer>(component);
                //MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
                
                if (meshRenderer)
                {
                    // map<�ν��Ͻ� ID,gameObject> ������ �߰�
                    const InstanceID instanceId = meshRenderer->GetInstanceID();
                    cache[instanceId].emplace_back(gameObject);
                }
                else 
                {
                    component->Render();
                }
            }
        }

        // ���� ������Ʈ�� ���� world�迭�� ���� �� world�迭�� ���ڰ����� �ѱ�鼭 �ν��Ͻ� ����
        for (auto& pair : cache)
        {
            const vector<GameObject*>& vec = pair.second;
            shared_ptr<InstancingBuffer> buffer = make_shared<InstancingBuffer>(); // worldMatrix ���� ������ �ִ� �ν��Ͻ� ����, ������ �Ѱܾ���
            {
                //const InstanceID instanceId = pair.first;

                for (int32 i = 0; i < vec.size(); i++)
                {
                    GameObject* gameObject = vec[i];
                    InstancingData data;
                    data.world = gameObject->GetTransform()->GetWorldMatrix();

                    // Mesh�� Material�� ���� ���̵� �迭�� data ���Ϳ� data(������ǥ��)����
                    buffer->AddData(data);
                    //AddData(instanceId, data);
                }

                vec[0]->GetComponent<MeshRenderer>()->RenderInstancing(buffer);
            }
        }
    }
    else
    {
        for (auto& gameObject : m_arrGameObjects[0])
        {
            for (auto& component : gameObject->GetComponents())
            {
                component->Render(); 
            }
        }
    }
}

void Scene::RenderSceneShadow()
{
    if (RenderManager::GetI()->InstancingMode)
    {
        map<InstanceID, vector<GameObject*>> cache;

        for (const auto& gameObject : m_arrGameObjects[0])
        {
            auto meshRenderer = gameObject->GetComponent<MeshRenderer>();
            if (meshRenderer == nullptr)
                continue;

            // map<�ν��Ͻ� ID,gameObject> ������ �߰�
            const InstanceID instanceId = meshRenderer->GetInstanceID();
            cache[instanceId].emplace_back(gameObject);
        }

        // ���� ������Ʈ�� ���� world�迭�� ���� �� world�迭�� ���ڰ����� �ѱ�鼭 �ν��Ͻ� ����
        for (auto& pair : cache)
        {
            const vector<GameObject*>& vec = pair.second;
            shared_ptr<InstancingBuffer> buffer = make_shared<InstancingBuffer>(); // worldMatrix ���� ������ �ִ� �ν��Ͻ� ����, ������ �Ѱܾ���
            {
                //const InstanceID instanceId = pair.first;

                for (int32 i = 0; i < vec.size(); i++)
                {
                    GameObject* gameObject = vec[i];
                    InstancingData data;
                    data.world = gameObject->GetTransform()->GetWorldMatrix();

                    // Mesh�� Material�� ���� ���̵� �迭�� data ���Ϳ� data(������ǥ��)����
                    buffer->AddData(data);
                    //AddData(instanceId, data);
                }

                vec[0]->GetComponent<MeshRenderer>()->RenderShadowInstancing(buffer);
            }
        }

    }
    else
    {
        for (auto& gameObject : m_arrGameObjects[0])
        {
            MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
            if (meshRenderer) meshRenderer->RenderShadow();

            SkinnedMeshRenderer* skinnedMeshRenderer = gameObject->GetComponent<SkinnedMeshRenderer>();
            if (skinnedMeshRenderer) skinnedMeshRenderer->RenderShadow();
        }
    }
}

void Scene::RenderSceneShadowNormal()
{
    if (RenderManager::GetI()->InstancingMode)
    {
        map<InstanceID, vector<GameObject*>> cache;

        for (const auto& gameObject : m_arrGameObjects[0])
        {
            auto meshRenderer = gameObject->GetComponent<MeshRenderer>();
            if (meshRenderer == nullptr)
                continue;

            // map<�ν��Ͻ� ID,gameObject> ������ �߰�
            const InstanceID instanceId = meshRenderer->GetInstanceID();
            cache[instanceId].emplace_back(gameObject);
        }

        // ���� ������Ʈ�� ���� world�迭�� ���� �� world�迭�� ���ڰ����� �ѱ�鼭 �ν��Ͻ� ����
        for (auto& pair : cache)
        {
            const vector<GameObject*>& vec = pair.second;
            shared_ptr<InstancingBuffer> buffer = make_shared<InstancingBuffer>(); // worldMatrix ���� ������ �ִ� �ν��Ͻ� ����, ������ �Ѱܾ���
            {
                //const InstanceID instanceId = pair.first;

                for (int32 i = 0; i < vec.size(); i++)
                {
                    GameObject* gameObject = vec[i];
                    InstancingData data;
                    data.world = gameObject->GetTransform()->GetWorldMatrix();

                    // Mesh�� Material�� ���� ���̵� �迭�� data ���Ϳ� data(������ǥ��)����
                    buffer->AddData(data);
                    //AddData(instanceId, data);
                }

                vec[0]->GetComponent<MeshRenderer>()->RenderShadowNormalInstancing(buffer);
            }
        }
    }
    else
    {
        for (const auto& gameObject : m_arrGameObjects[0])
        {
            MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
            if (meshRenderer) meshRenderer->RenderShadowNormal();

            SkinnedMeshRenderer* skinnedMeshRenderer = gameObject->GetComponent<SkinnedMeshRenderer>();
            if (skinnedMeshRenderer) skinnedMeshRenderer->RenderShadowNormal();
        }
    }
}

void Scene::_Editor_RenderScene()
{
    for (auto& gameObject : m_arrGameObjects[0]) 
    {
        MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
        if (meshRenderer) meshRenderer->_Editor_Render();

        SkinnedMeshRenderer* skinnedMeshRenderer = gameObject->GetComponent<SkinnedMeshRenderer>();
        if (skinnedMeshRenderer) skinnedMeshRenderer->_Editor_Render();
    }
}

void Scene::_Editor_RenderSceneShadowNormal()
{
    for (const auto& gameObject : m_arrGameObjects[0])
    {
        MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>(); 
        if (meshRenderer) meshRenderer->_Editor_RenderShadowNormal();  

        SkinnedMeshRenderer* skinnedMeshRenderer = gameObject->GetComponent<SkinnedMeshRenderer>();  
        if (skinnedMeshRenderer) skinnedMeshRenderer->_Editor_RenderShadowNormal();
    } 
}

void Scene::RenderSceneGizmos()
{
    if (SelectionManager::GetSelectedObjectType() == SelectionType::GAMEOBJECT)
    {
        GameObject* gameObject = SelectionManager::GetSelectedGameObject();

        if (gameObject)
        {
            Gizmo::DrawTransformHandler(gameObject->GetTransform());
        }
    }

    for (auto& gameObject : m_arrGameObjects[0])
    {
        for (auto& component : gameObject->GetComponents())
        {
            component->OnDrawGizmos();
        }
    }

    PhysicsManager::GetI()->DebugRender();
}

void Scene::LastFramUpdate()
{
    for (auto& gameObject : m_arrGameObjects[0])
    {
        gameObject->LastUpdate(); 
        gameObject->ApplyPendingComponents(); 
    }
}

void Scene::UpdateScene()
{
    for (auto& gameObject : m_arrGameObjects[0])
    {
        for (auto& component : gameObject->GetComponents())
        {
            component->Update();
        }
    }

    for (auto& gameObject : m_arrGameObjects[0])
    {
        for (auto& component : gameObject->GetComponents())
        {
            component->LateUpdate();
        }
    }
}

Scene* Scene::Load(wstring scenePath)
{
    std::ifstream is(wstring_to_string(PathManager::GetI()->GetMovePathW(scenePath)));

    if (!is)
    {
        return nullptr;
    }

    json j;
    is >> j;

    Scene* scene = new Scene();
    scene->m_ScenePath = scenePath;
    from_json(j, *scene);

    return scene;
}

void Scene::Save(Scene* scene)
{
    if (scene->m_ScenePath.empty())
    {
        return;
    }

    json j = *scene;
    //std::ofstream os(scene->m_ScenePath);
    std::ofstream os(PathManager::GetI()->GetMovePathW(scene->m_ScenePath));

    if (os)
    {
        os << j.dump(4);
        os.close();

        // �����
        // ������ ���� ���� ���� ������ ����
        EditorSettingManager::SetLastOpenedScenePath(scene->m_ScenePath);
    }
    else
    {
        // ���� ���� ó��
    }
}

void Scene::SaveNewScene(Scene* scene)
{
    //std::wstring filePath = EditorUtility::SaveFileDialog(Application::GetDataPath(), L"Save Scene As", L"scene");
    std::wstring filePath = EditorUtility::SaveFileDialog(PathManager::GetI()->GetMovePathW(L"Assets\\"), L"Save Scene As", L"scene");
    
    filePath = PathManager::GetI()->GetCutSolutionPath(filePath);

    if (!filePath.empty()) 
    {
        scene->m_ScenePath = filePath;

        json j = *scene;
        std::ofstream os(PathManager::GetI()->GetMovePathW(filePath));

        if (os)
        {
            os << j.dump(4); 
            os.close();

            // �����
            // ������ ���� ���� ���� ������ ����
            EditorSettingManager::SetLastOpenedScenePath(filePath);
        }
        else 
        {
            // ���� ���� ó��
        }
    }
}

void Scene::DestroyComponent(Component* component)
{
    if (component->GetType() == "Transform")
    {
        // �����
        return;
    }

    component->OnDestroy(); 
    auto& components = component->GetGameObject()->GetComponents();

    bool isDeleteComponent = false;
    for (int i = 0; i < components.size(); ++i) 
    {
        auto& c = components[i]; 
        if (c.get() == component)  
        {
            c.reset();
            components.erase(components.begin() + i);
            isDeleteComponent = true;
            break;
        }
    }
    
    if (!isDeleteComponent && component != nullptr)
        delete component; 
}

void Scene::DestroyGameObject(GameObject* gameobject)
{
    auto it1 = std::find(m_vecRootGameObjects.begin(), m_vecRootGameObjects.end(), gameobject);  
    auto it2 = std::find(m_arrGameObjects[0].begin(), m_arrGameObjects[0].end(), gameobject); 
    
    gameobject->OnDestroy();
    if (it1 != m_vecRootGameObjects.end())  m_vecRootGameObjects.erase(it1); 
    if (it2 != m_arrGameObjects[0].end())  m_arrGameObjects[0].erase(it2);  
}

void Scene::AddRootGameObject(GameObject* gameObject)
{
    if (gameObject == nullptr)
        return;

	m_vecRootGameObjects.push_back(gameObject);
	m_arrGameObjects[0].push_back(gameObject);
}

void Scene::RemoveRootGameObjects(GameObject* gameObject)
{
    auto it = std::find(m_vecRootGameObjects.begin(), m_vecRootGameObjects.end(), gameObject);  
    if (it != m_vecRootGameObjects.end())  
    { 
        m_vecRootGameObjects.erase(it); 
    }
}

void to_json(json& j, const Scene& scene)
{
    j = json
    {
        { "rootGameObjects", json::array() }
    };
    for (const GameObject* gameObject : scene.m_vecRootGameObjects)
    {
        j["rootGameObjects"].push_back(*gameObject);
    }
}

void from_json(const json& j, Scene& scene)
{
    for (const auto& gameObjectJson : j.at("rootGameObjects"))
    {
        GameObject* gameObject = new GameObject();
        from_json(gameObjectJson, *gameObject); 
        scene.m_vecRootGameObjects.push_back(gameObject);
        scene.m_arrGameObjects[0].push_back(gameObject);
    }
}
