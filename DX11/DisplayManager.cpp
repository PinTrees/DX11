#include "pch.h"
#include "DisplayManager.h"

SINGLE_BODY(DisplayManager)

DisplayManager::DisplayManager() 
{
}
DisplayManager::~DisplayManager()
{
}

void DisplayManager::Init()
{
	auto scene = SceneManager::GetI()->GetCurrentScene();

	for (auto gameobject : scene->GetAllGameObjects())
	{
		auto camera = gameobject->GetComponent_SP<Camera>();
		if (camera != nullptr)
			m_AllCameraComponents.push_back(camera);
	}
}

void DisplayManager::RegisterCameraComponent(const shared_ptr<Camera>& camera)
{
}

void DisplayManager::DeleteCameraComponent(const shared_ptr<Camera>& camera)
{

}

shared_ptr<Camera> DisplayManager::GetActiveCamera()
{
	if (m_AllCameraComponents.size() <= 0)
		return nullptr; 

	return m_AllCameraComponents[0];
}
