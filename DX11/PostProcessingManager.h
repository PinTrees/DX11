#pragma once
#include "SSAO.h"

class Camera;
class EditorCamera;

class PostProcessingManager
{
	SINGLE_HEADER(PostProcessingManager)

private:
	std::unique_ptr<Ssao> m_EditorSSAO;
	std::unique_ptr<Ssao> m_GameSSAO;

public:
	void Init();
	void RenderEditorSSAO(EditorCamera* camera);
	//void RenderGameSSAO(ID3D11RenderTargetView* renderTargetView, Camera* camera);

	void SetEditorSSAO(int32 screenWidth, int32 screenHeight, EditorCamera* camera);
	Ssao* GetEditorSSAO() { return m_EditorSSAO.get(); }
};

