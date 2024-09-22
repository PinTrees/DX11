#include "pch.h"
#include "PostProcessingManager.h"
#include "EditorCamera.h"

SINGLE_BODY(PostProcessingManager)

PostProcessingManager::PostProcessingManager()
{

}

PostProcessingManager::~PostProcessingManager()
{

}

void PostProcessingManager::Init()
{
	//m_GameSSAO = std::make_unique<Ssao>(m_Device, m_Context);
}

void PostProcessingManager::RenderEditorSSAO(EditorCamera* camera)
{
	m_EditorSSAO->ComputeSsao(*camera);
	m_EditorSSAO->BlurAmbientMap(4); 
}

void PostProcessingManager::SetEditorSSAO(int32 screenWidth, int32 screenHeight, EditorCamera* camera)
{
	m_EditorSSAO = std::make_unique<Ssao>();
	m_EditorSSAO->Init(screenWidth, screenHeight, camera->GetFovY(), camera->GetFarZ());
}
