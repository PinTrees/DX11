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

public:
	static const int SMapSize = 2048;
	shared_ptr<class ShadowMap> editorShadowMap;

public:
	void Init();
};

