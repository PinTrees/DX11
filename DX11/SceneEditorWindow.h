#pragma once
#include "EditorWindow.h"

class SceneEditorWindow
	: public EditorWindow
{
private:
	// DirectX 11 렌더 타겟 및 뷰포트 설정
	ID3D11Texture2D* renderTargetTexture = nullptr;
	ID3D11RenderTargetView* renderTargetView = nullptr;
	ID3D11ShaderResourceView* shaderResourceView = nullptr;

	ID3D11RenderTargetView* oldRenderTarget;

	// 창 크기 저장
	UINT windowWidth;
	UINT windowHeight;

public:
	SceneEditorWindow();
	~SceneEditorWindow();

public:
	virtual void Update() override;

protected:

	virtual void OnRender() override;
	virtual void OnRenderExit() override;

private:
	void InitRenderTarget(UINT width, UINT height);
	void CleanUpRenderTarget();
};

