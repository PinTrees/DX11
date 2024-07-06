#include "pch.h"
#include "RenderManager.h"
#include "ShadowMap.h"

SINGLE_BODY(RenderManager)

RenderManager::RenderManager()
{

}

RenderManager::~RenderManager()
{

}

void RenderManager::Init()
{
	auto device = Application::GetI()->GetDevice();
	editorShadowMap = make_shared<ShadowMap>(device, SMapSize, SMapSize); 

    SetEditorViewport(100, 100);
}

void RenderManager::SetEditorViewport(UINT width, UINT height)
{
    EditorViewport.TopLeftX = 0.0f;
    EditorViewport.TopLeftY = 0.0f;
    EditorViewport.Width = static_cast<float>(width); 
    EditorViewport.Height = static_cast<float>(height); 
    EditorViewport.MinDepth = 0.0f;
    EditorViewport.MaxDepth = 1.0f;
}
