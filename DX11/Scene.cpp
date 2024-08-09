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

    if (false) // if문에 Editor에서 인스턴싱을 사용할 것인지에 대한 bool형 변수로 인스턴싱 사용여부 판단
    {
        map<InstanceID, vector<shared_ptr<GameObject>>> cache;

        for (const auto& gameObject : m_arrGameObjects[0])
        {
            // 다른 컴포넌트들도 순회하면서 렌더해야함(수정사항)
            auto meshRenderer = gameObject->GetComponent<MeshRenderer>();
            if (meshRenderer == nullptr)
                continue;

            // map<인스턴싱 ID,gameObject> 변수에 추가
            const InstanceID instanceId = meshRenderer->GetInstanceID();
            cache[instanceId].emplace_back(gameObject);
        }

        // 같은 오브젝트들 끼리 world배열에 저장 후 world배열을 인자값으로 넘기면서 인스턴싱 렌더
        for (auto& pair : cache)
        {
            const vector<shared_ptr<GameObject>>& vec = pair.second;
            shared_ptr<InstancingBuffer> buffer; // worldMatrix 값을 가지고 있는 인스턴싱 버퍼, 렌더로 넘겨야함
            {
                //const InstanceID instanceId = pair.first;

                for (int32 i = 0; i < vec.size(); i++)
                {
                    const shared_ptr<GameObject>& gameObject = vec[i];
                    InstancingData data;
                    data.world = gameObject->GetTransform()->GetWorldMatrix();

                    // Mesh와 Material의 조합 아이디 배열의 data 벡터에 data(월드좌표들)저장
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
                // 다른 컴포넌트들도 순회하면서 렌더해야함(수정사항)
                auto meshRenderer = gameObject->GetComponent<MeshRenderer>();
                if (meshRenderer == nullptr)
                    continue;

                // map<인스턴싱 ID,gameObject> 변수에 추가
                const InstanceID instanceId = meshRenderer->GetInstanceID();
                cache[instanceId].emplace_back(gameObject);
            }

            // 같은 오브젝트들 끼리 world배열에 저장 후 world배열을 인자값으로 넘기면서 인스턴싱 렌더
            for (auto& pair : cache)
            {
                const vector<shared_ptr<GameObject>>& vec = pair.second;
                shared_ptr<InstancingBuffer> buffer; // worldMatrix 값을 가지고 있는 인스턴싱 버퍼, 렌더로 넘겨야함
                {
                    //const InstanceID instanceId = pair.first;

                    for (int32 i = 0; i < vec.size(); i++)
                    {
                        const shared_ptr<GameObject>& gameObject = vec[i];
                        InstancingData data;
                        data.world = gameObject->GetTransform()->GetWorldMatrix();

                        // Mesh와 Material의 조합 아이디 배열의 data 벡터에 data(월드좌표들)저장
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
                // 다른 컴포넌트들도 순회하면서 렌더해야함(수정사항)
                auto meshRenderer = gameObject->GetComponent<MeshRenderer>();
                if (meshRenderer == nullptr)
                    continue;

                // map<인스턴싱 ID,gameObject> 변수에 추가
                const InstanceID instanceId = meshRenderer->GetInstanceID();
                cache[instanceId].emplace_back(gameObject);
            }

            // 같은 오브젝트들 끼리 world배열에 저장 후 world배열을 인자값으로 넘기면서 인스턴싱 렌더
            for (auto& pair : cache)
            {
                const vector<shared_ptr<GameObject>>& vec = pair.second;
                shared_ptr<InstancingBuffer> buffer; // worldMatrix 값을 가지고 있는 인스턴싱 버퍼, 렌더로 넘겨야함
                {
                    //const InstanceID instanceId = pair.first;

                    for (int32 i = 0; i < vec.size(); i++)
                    {
                        const shared_ptr<GameObject>& gameObject = vec[i];
                        InstancingData data;
                        data.world = gameObject->GetTransform()->GetWorldMatrix();

                        // Mesh와 Material의 조합 아이디 배열의 data 벡터에 data(월드좌표들)저장
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

        // 저장됨
        // 마지막 오픈 씬을 현재 씬으로 변경
        EditorSettingManager::SetLastOpenedScenePath(scene->m_ScenePath);
    }
    else
    {
        // 저장 실패 처리
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

            // 저장됨
            // 마지막 오픈 씬을 현재 씬으로 변경
            EditorSettingManager::SetLastOpenedScenePath(filePath);
        }
        else 
        {
            // 저장 실패 처리
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
