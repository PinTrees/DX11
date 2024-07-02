#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"

SINGLE_BODY(SceneManager)

SceneManager::SceneManager()
	: m_pCurrScene(nullptr)
{

}

SceneManager::~SceneManager()
{

}

void SceneManager::Init()
{
}

void SceneManager::LoadScene(string sceneName)
{
	CreateScene();
	m_pCurrScene->Enter();
}

void SceneManager::UpdateScene()
{
	if (m_pCurrScene == nullptr)
		return;

	m_pCurrScene->UpdateScene();
}

void SceneManager::RenderScene()
{
	if (m_pCurrScene == nullptr)
		return;

	m_pCurrScene->RenderScene();
}

void SceneManager::SaveScene()
{
}

void SceneManager::CreateScene()
{
	m_pCurrScene = new Scene;
}
