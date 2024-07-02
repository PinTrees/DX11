#pragma once

class RenderManager
{
	SINGLE_HEADER(RenderManager)

public:
	XMMATRIX shadowTransform;

	XMMATRIX cameraViewMatrix;
	XMMATRIX cameraProjectionMatrix;
	XMMATRIX cameraViewProjectionMatrix;

	XMMATRIX directinalLightViewProjection;
};

