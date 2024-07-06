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
}
