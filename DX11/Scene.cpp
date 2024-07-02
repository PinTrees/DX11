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
}

void Scene::UpdateScene()
{
}

void Scene::AddRootGameObject(GameObject* gameObject)
{
	m_RootGameObjects.push_back(gameObject);
}
