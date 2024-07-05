#include "pch.h"
#include "SelectionManager.h"
#include <filesystem>

namespace fs = std::filesystem;

SINGLE_BODY(SelectionManager)

SelectionType SelectionManager::m_SelectedType = SelectionType::GAMEOBJECT;
SelectionSubType SelectionManager::m_SelectedSubType = SelectionSubType::MATERIAL;

std::string SelectionManager::m_SelectedFilePath;
GameObject* SelectionManager::m_SelectedGameObject = nullptr;

UMaterial* SelectionManager::m_SelectedFile_Material = nullptr;

SelectionManager::SelectionManager()
{

}

SelectionManager::~SelectionManager()
{

}

void SelectionManager::SetSelectedFile(const std::string& filePath)
{
	m_SelectedFilePath = filePath;
	m_SelectedType = SelectionType::FILE;

	fs::path path(filePath);

	if (path.extension() == ".mat")
	{
		if (m_SelectedFile_Material != nullptr)
			delete m_SelectedFile_Material;

		m_SelectedFile_Material = ResourceManager::GetI()->LoadMaterial(filePath);
		m_SelectedSubType == SelectionSubType::MATERIAL;
	}
}

void SelectionManager::SetSelectedGameObject(GameObject* gameObject)
{
	m_SelectedGameObject = gameObject;
	m_SelectedType = SelectionType::GAMEOBJECT;
}
