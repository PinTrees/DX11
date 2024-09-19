#pragma once

class ShadowMap;

class RenderManager
{
	SINGLE_HEADER(RenderManager)

public:
	XMMATRIX shadowTransform;

	XMMATRIX cameraViewMatrix;
	XMMATRIX cameraProjectionMatrix;
	XMMATRIX cameraViewProjectionMatrix;

	XMMATRIX directinalLightViewProjection;

	// Editor Only
public:
	static const int SMapSize = 2048;
	shared_ptr<class ShadowMap> editorShadowMap;

	D3D11_VIEWPORT EditorViewport;
	bool WireFrameMode;
	bool InstancingMode;
public:
	void Init();

	// Editor Only
public:
	void SetEditorViewport(UINT width, UINT height);
};

