#include "pch.h"
#include "Scene.h"

Scene::Scene()
	: m_RootGameObjects()
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

void Scene::AddRootGameObject(GameObject* gameObject)
{
	m_RootGameObjects.push_back(gameObject);
	m_arrGameObjects[0].push_back(gameObject);
}
