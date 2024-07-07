#pragma once

class GameObject;
class UMaterial;

enum class SelectionType
{
    NONE,
    FILE,
    GAMEOBJECT,
};

enum class SelectionSubType
{
    NONE,
    MATERIAL,
};

class SelectionManager
{
	SINGLE_HEADER(SelectionManager)

private:
    static SelectionType m_SelectedType;
    static SelectionSubType m_SelectedSubType;

    static std::wstring m_SelectedFilePath;
    static GameObject* m_SelectedGameObject;

    static UMaterial* m_SelectedFile_Material;

public:
    static void ClearSelection();

    static void SetSelectedFile(const std::wstring& filePath);
    static const std::wstring& GetSelectedFile() { return m_SelectedFilePath; }

    static void SetSelectedGameObject(GameObject* gameObject);
    static GameObject* GetSelectedGameObject() { return m_SelectedGameObject; }

    static SelectionType GetSelectedObjectType() { return m_SelectedType; }
    static SelectionSubType GetSelectedSubType() { return m_SelectedSubType; }

    static UMaterial* GetSelectMaterial() { return m_SelectedFile_Material; }
};

