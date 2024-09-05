#pragma once
#include "Component.h"
#include "LightHelper.h"

class Light
	: public Component
{
private:
	DirectionalLight m_directionalDesc;
	//PointLight _pointDesc;
	//SpotLight _spotDesc;

	float m_lightLengthX;
	float m_lightLengthY;

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

	GENERATE_COMPONENT_BODY(Light)
};

REGISTER_COMPONENT(Light)
