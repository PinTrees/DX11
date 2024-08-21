#include "pch.h"
#include "Scene.h"

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
    for (auto& gameObject : m_arrGameObjects[0])
    {
        for (auto& component : gameObject->GetComponents())
        {
            component->Render();
        }
    }
}

void Scene::RenderSceneShadow()
{
    for (auto& gameObject : m_arrGameObjects[0])
    {
        MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
        if (meshRenderer == nullptr)
            continue;

        meshRenderer->RenderShadow();
    }
}

void Scene::RenderSceneShadowNormal()
{
    for (auto& gameObject : m_arrGameObjects[0])
    {
        MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
        if (meshRenderer == nullptr)
            continue;

        meshRenderer->RenderShadowNormal();
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
    std::ifstream is(wstring_to_string(scenePath));

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
    std::ofstream os(scene->m_ScenePath);

    if (os)
    {
        os << j.dump(4);
        os.close();

        // 저장됨
        // 마지막 오픈 씬을 현재 씬으로 변경
        EditorSettingManager::SetLadtOpenedScenePath(scene->m_ScenePath);
    }
    else
    {
        // 저장 실패 처리
    }
}

void Scene::SaveNewScene(Scene* scene)
{
    std::wstring filePath = EditorUtility::SaveFileDialog(Application::GetDataPath(), L"Save Scene As", L"scene");
    if (!filePath.empty()) 
    {
        scene->m_ScenePath = filePath;

        json j = *scene;
        std::ofstream os(filePath);

        if (os)
        {
            os << j.dump(4); 
            os.close();

            // 저장됨
            // 마지막 오픈 씬을 현재 씬으로 변경
            EditorSettingManager::SetLadtOpenedScenePath(filePath);
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
        GameObject* gameObject = new GameObject;
        from_json(gameObjectJson, *gameObject); 
        scene.m_RootGameObjects.push_back(gameObject);

        // 루트 오브젝트와 그 자식들을 모두 m_arrGameObjects에 저장
        AddGameObjectAndChildrenToArray(gameObject, scene.m_arrGameObjects[0]);
    }
}



// 재귀적으로 오브젝트와 그 자식들을 m_arrGameObjects에 추가하는 함수
void AddGameObjectAndChildrenToArray(GameObject* gameObject, std::vector<GameObject*>& arrGameObjects)
{
    // 현재 오브젝트를 배열에 추가
    arrGameObjects.push_back(gameObject);

    // 자식 오브젝트들을 가져와 재귀적으로 추가
    for (auto* child : gameObject->GetChildren())
    {
        AddGameObjectAndChildrenToArray(child, arrGameObjects);
    }
}