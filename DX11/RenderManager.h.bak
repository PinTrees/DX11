#pragma once

class ShadowMap;

class RenderManager
{
	SINGLE_HEADER(RenderManager)

public:
	XMMATRIX shadowTransform;

	// Common
	shared_ptr<class ShadowMap> BaseShadowMap;


	// Editor Only
	XMMATRIX EditorCameraViewMatrix;
	XMMATRIX EditorCameraProjectionMatrix;
	XMMATRIX EditorCameraViewProjectionMatrix;
	D3D11_VIEWPORT EditorViewport;

	// Game Only
	XMMATRIX CameraViewMatrix;
	XMMATRIX CameraProjectionMatrix;
	XMMATRIX CameraViewProjectionMatrix;
	D3D11_VIEWPORT Viewport; 

	XMMATRIX DirectinalLightViewProjection;

	// Editor Only
public:
	static const int SMapSize = 2048;

	bool WireFrameMode;
	bool InstancingMode;
public:
	void Init();

	// Editor Only
public:
	void SetEditorViewport(UINT width, UINT height);
	void SetViewport(UINT width, UINT height);
};

