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
    PhysicsManager::GetI()->Start();
}

void Scene::Exit()
{
}

void Scene::RenderScene()
{
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
