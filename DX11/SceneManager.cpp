#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"
#include "LightManager.h"


SINGLE_BODY(SceneManager)

SceneManager::SceneManager()
	: m_pCurrScene(nullptr)
{

}

SceneManager::~SceneManager()
{
	Safe_Delete_Map(m_Scenes); 
}

void SceneManager::Init()
{
}

void SceneManager::LoadScene(wstring scenePath)
{
	Scene* scene = nullptr;

	if (m_Scenes.find(scenePath) != m_Scenes.end())
	{
		scene = m_Scenes[scenePath];
		m_pCurrScene = scene;
	}
	else
	{
		scene = Scene::Load(scenePath);
		if (scene == nullptr)
		{
			CreateScene();
		}
		else
		{
			m_Scenes[scenePath] = scene;
			m_pCurrScene = scene;
		}
	}

	m_pCurrScene->Enter();
	DisplayManager::GetI()->Init(); 
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

void SceneManager::HandleSaveScene()
{
	if (INPUT_KEY_HOLD(KEY::CTRL) && INPUT_KEY_HOLD(KEY::LSHIFT) && INPUT_KEY_DOWN(KEY::S))
	{
		if (m_pCurrScene == nullptr)
			return;

		Scene::SaveNewScene(m_pCurrScene);
	}
	else if (INPUT_KEY_HOLD(KEY::CTRL) && INPUT_KEY_DOWN(KEY::S))
	{
		if (m_pCurrScene == nullptr)
			return;

		if (m_pCurrScene->GetScenePath().empty())
			Scene::SaveNewScene(m_pCurrScene);
		else
			Scene::Save(m_pCurrScene);
	}
}

void SceneManager::HandlePlay()
{
	if (m_pCurrScene == nullptr) 
		return;

	Scene::Save(m_pCurrScene); 
	m_pCurrScene->Enter();
}

void SceneManager::HandleStop()
{
	if (m_pCurrScene == nullptr)
		return;

	wstring scenePath = m_pCurrScene->GetScenePath();

	// Delete Scene
	m_pCurrScene->Exit();
	delete m_pCurrScene;

	m_pCurrScene = nullptr;
	m_Scenes.erase(scenePath); 

	// Load Scene
	SceneManager::LoadScene(scenePath); 
}

void SceneManager::CreateScene()
{
	m_pCurrScene = new Scene;

	// Camera, Light Object 생성, 현재 Demo에서 씬의 크기를 구할 때 게임오브젝트는 1개인데, 메쉬렌더러 오브젝트는 아무것도없어서 씬의 크기가 무한대여서 오류 ㅇㅇ
	GameObject* camera = new GameObject("Camera");
	camera->AddComponent<Camera>();
	m_pCurrScene->AddRootGameObject(camera);

	GameObject* light = new GameObject("Light");
	
	DirectionalLight dir;
	dir.Ambient = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	dir.Diffuse = XMFLOAT4(0.8f, 0.7f, 0.7f, 1.0f);
	dir.Specular = XMFLOAT4(0.6f, 0.6f, 0.7f, 1.0f); 
	dir.Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);
	
	light->AddComponent<Light>();
	light->GetComponent<Light>()->SetDirLight(dir);
	
	m_pCurrScene->AddRootGameObject(light);
}
