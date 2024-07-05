#include "pch.h"
#include "Scene.h"

Scene::Scene()
	: m_RootGameObjects(),
    m_ScenePath(L"")
{
}

Scene::~Scene()
{
	Safe_Delete_Vec(m_RootGameObjects);
}

void Scene::Enter()
{
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

void Scene::UpdateScene()
{
}

void Scene::Load(string scenePath)
{
}

void Scene::Save(Scene* scene)
{
    if (scene->m_ScenePath.empty())
    {
        return;
    }

    json j = scene->toJson();
    std::ofstream os(scene->m_ScenePath);

    if (os)
    {
        os << j.dump(4);
        os.close();
    }
    else
    {
    }
}

void Scene::SaveNewScene(Scene* scene)
{
    std::wstring filePath = EditorUtility::SaveFileDialog(Application::GetDataPath(), L"Save Scene As", L"scene");
    if (!filePath.empty()) 
    {
        scene->m_ScenePath = filePath;

        json j = scene->toJson();
        std::ofstream os(filePath);

        if (os)
        {
            os << j.dump(4); 
            os.close();
        }
        else 
        {
        }
    }
}

void Scene::AddRootGameObject(GameObject* gameObject)
{
	m_RootGameObjects.push_back(gameObject);
	m_arrGameObjects[0].push_back(gameObject);
}
