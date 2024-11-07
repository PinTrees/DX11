#pragma once

class ShadowMap;

class RenderManager
{
	SINGLE_HEADER(RenderManager)

public:
	// Common
	shared_ptr<class ShadowMap> BaseShadowMap;
	shared_ptr<class ShadowMap> EditorShadowMap;

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

	// ShaodwTransform -> VPT, �������� clear �ʼ�
	vector<XMMATRIX> ShadowTransformArray[(uint32)LightType::End];
	vector<XMMATRIX> EditorShadowTransformArray[(uint32)LightType::End];
	XMMATRIX LightViewProjection; // shadowMap Render�� ����, Editor and Game �� �� ���

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

