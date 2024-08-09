#include "pch.h"
#include "Scene.h"
#include "InstancingBuffer.h"

Scene::Scene()
	: m_RootGameObjects(),
    m_arrGameObjects{},
    m_ScenePath(L"")
{
}

Scene::~Scene()
{
	Safe_Delete_Vec(m_RootGameObjects);
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
    /*
    for (auto& gameObject : m_arrGameObjects[0])
    {
        for (auto& component : gameObject->GetComponents())
        {
            component->Render();
        }

        //MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
        //if (meshRenderer == nullptr)
        //    continue;
        //
        //meshRenderer->Render();
    }
    */

    if (false) // if���� Editor���� �ν��Ͻ��� ����� �������� ���� bool�� ������ �ν��Ͻ� ��뿩�� �Ǵ�
    {
        map<InstanceID, vector<shared_ptr<GameObject>>> cache;

        for (const auto& gameObject : m_arrGameObjects[0])
        {
            // �ٸ� ������Ʈ�鵵 ��ȸ�ϸ鼭 �����ؾ���(��������)
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
            const vector<shared_ptr<GameObject>>& vec = pair.second;
            shared_ptr<InstancingBuffer> buffer; // worldMatrix ���� ������ �ִ� �ν��Ͻ� ����, ������ �Ѱܾ���
            {
                //const InstanceID instanceId = pair.first;

                for (int32 i = 0; i < vec.size(); i++)
                {
                    const shared_ptr<GameObject>& gameObject = vec[i];
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
    for (auto& gameObject : m_arrGameObjects[0])
    {
        if (false)
        {
            map<InstanceID, vector<shared_ptr<GameObject>>> cache;

            for (const auto& gameObject : m_arrGameObjects[0])
            {
                // �ٸ� ������Ʈ�鵵 ��ȸ�ϸ鼭 �����ؾ���(��������)
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
                const vector<shared_ptr<GameObject>>& vec = pair.second;
                shared_ptr<InstancingBuffer> buffer; // worldMatrix ���� ������ �ִ� �ν��Ͻ� ����, ������ �Ѱܾ���
                {
                    //const InstanceID instanceId = pair.first;

                    for (int32 i = 0; i < vec.size(); i++)
                    {
                        const shared_ptr<GameObject>& gameObject = vec[i];
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
            MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
            if (meshRenderer == nullptr || (!meshRenderer->GetUseSsao()))
                continue;

            meshRenderer->RenderShadow();
        }
    }
}

void Scene::RenderSceneShadowNormal()
{
    for (auto& gameObject : m_arrGameObjects[0])
    {
        if (false)
        {
            map<InstanceID, vector<shared_ptr<GameObject>>> cache;

            for (const auto& gameObject : m_arrGameObjects[0])
            {
                // �ٸ� ������Ʈ�鵵 ��ȸ�ϸ鼭 �����ؾ���(��������)
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
                const vector<shared_ptr<GameObject>>& vec = pair.second;
                shared_ptr<InstancingBuffer> buffer; // worldMatrix ���� ������ �ִ� �ν��Ͻ� ����, ������ �Ѱܾ���
                {
                    //const InstanceID instanceId = pair.first;

                    for (int32 i = 0; i < vec.size(); i++)
                    {
                        const shared_ptr<GameObject>& gameObject = vec[i];
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
            MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
            if (meshRenderer == nullptr || (!meshRenderer->GetUseSsao()))
                continue;

            meshRenderer->RenderShadowNormal();
        }
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
        gameObject->ApplyPendingComponents();
    }
}

void Scene::UpdateScene()
{
}

Scene* Scene::Load(wstring scenePath)
{
    //std::ifstream is(wstring_to_string(scenePath));
    std::ifstream is(wstring_to_string(PathManager::GetI()->GetMovePath(scenePath)));

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
    std::ofstream os(PathManager::GetI()->GetMovePath(scene->m_ScenePath));

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
    std::wstring filePath = EditorUtility::SaveFileDialog(PathManager::GetI()->GetMovePath(L"Assets\\"), L"Save Scene As", L"scene");
    
    filePath = PathManager::GetI()->GetCutSolutionPath(filePath);

    if (!filePath.empty()) 
    {
        scene->m_ScenePath = filePath;

        json j = *scene;
        std::ofstream os(PathManager::GetI()->GetMovePath(filePath));

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

void Scene::AddRootGameObject(GameObject* gameObject)
{
    if (gameObject == nullptr)
        return;

	m_RootGameObjects.push_back(gameObject);
	m_arrGameObjects[0].push_back(gameObject);
}

void Scene::RemoveRootGameObjects(GameObject* gameObject)
{
    auto it = std::find(m_RootGameObjects.begin(), m_RootGameObjects.end(), gameObject);  
    if (it != m_RootGameObjects.end())  
    { 
        m_RootGameObjects.erase(it); 
    }
}

void to_json(json& j, const Scene& scene)
{
    j = json
    {
        { "rootGameObjects", json::array() }
    };
    for (const GameObject* gameObject : scene.m_RootGameObjects)
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
        scene.m_RootGameObjects.push_back(gameObject);
        scene.m_arrGameObjects[0].push_back(gameObject);
    }
}
