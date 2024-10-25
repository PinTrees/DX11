#pragma once
#include "EditorWindow.h"

class GameViewEditorWindow
	: public EditorWindow
{
private:
	// DirectX 11 ���� Ÿ�� �� ����Ʈ ����
	ID3D11Texture2D* renderTargetTexture = nullptr;
	ID3D11RenderTargetView* renderTargetView = nullptr;
	ID3D11ShaderResourceView* shaderResourceView = nullptr;

	ID3D11RenderTargetView* oldRenderTarget;

	// â ũ�� ����
	UINT windowWidth;
	UINT windowHeight;

public:
	GameViewEditorWindow();
	~GameViewEditorWindow();

public:
	virtual void OnRender() override;

private:
	void InitRenderTarget(UINT width, UINT height);
	void CleanUpRenderTarget();
	void RenderScene();
};

