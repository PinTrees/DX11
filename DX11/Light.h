#pragma once
#include "Component.h"
#include "LightHelper.h"

enum class LightType
{
	Directional,
	Point,
	Spot
};

class Light
	: public Component
{
private:
	LightType m_lightType;

	DirectionalLight m_directionalDesc;
	PointLight m_pointDesc;
	SpotLight m_spotDesc;

	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	float m_lightLengthX = 20.0f;
	float m_lightLengthY = 20.0f;
	float m_lightFarZ = 100.0f;

	XMFLOAT4X4 m_lightView;
	XMFLOAT4X4 m_lightProj;

public:
	Light();
	~Light();

	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void FixedUpdate() override;
	virtual void Render() override;
	virtual void OnInspectorGUI() override;

	DirectionalLight GetDirLight() { return m_directionalDesc; }
	PointLight GetPointLight() { return m_pointDesc; }
	SpotLight GetSpotLight() { return m_spotDesc; }

	XMFLOAT4X4 GetLightView() { return m_lightView; }
	XMFLOAT4X4 GetLightProj() { return m_lightProj; }

	float GetLightLengthX() { return m_lightLengthX; }
	float GetLightLengthY() { return m_lightLengthY; }
	float GetLightFarZ() { return m_lightFarZ; }

	void SetLightLengthX(float x) { m_lightLengthX = x; }
	void SetLightLengthY(float y) { m_lightLengthY = y; }
	void SetLightFarZ(float farZ) { m_lightFarZ = farZ; }


	GENERATE_COMPONENT_BODY(Light)
};

REGISTER_COMPONENT(Light)
