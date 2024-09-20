#pragma once
#include "Component.h"
#include "LightHelper.h"

enum class LightType
{
	Directional,
	Point,
	Spot,
	End
};

class Light
	: public Component
	, std::enable_shared_from_this<Light>
{
private:
	LightType m_lightType = LightType::Directional;

	DirectionalLight m_directionalDesc;
	PointLight m_pointDesc;
	SpotLight m_spotDesc;

	XMFLOAT2 m_dirLightLen = XMFLOAT2(20.f,20.f);
	XMFLOAT2 m_spotLightLen = XMFLOAT2(20.f, 20.f);

	XMFLOAT4X4 m_lightView;
	XMFLOAT4X4 m_lightProj;

	// shadowTransform => world * (lightV * lightP * toTexSpace)

	void ProjUpdate();
	string GetStringLightType(LightType type);

public:
	Light();
	~Light();

	InstanceID GetInstanceID()
	{
		return make_tuple((uint64)&m_directionalDesc, (uint64)&m_pointDesc, (uint64)&m_spotDesc);
	}

	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void FixedUpdate() override;
	virtual void Render() override;
	virtual void OnInspectorGUI() override;
	virtual void OnDestroy() override;

	DirectionalLight GetDirLight() { return m_directionalDesc; }
	PointLight GetPointLight() { return m_pointDesc; }
	SpotLight GetSpotLight() { return m_spotDesc; }

	XMFLOAT4X4 GetLightView() { return m_lightView; }
	XMFLOAT4X4 GetLightProj() { return m_lightProj; }

	LightType GetLightType() { return m_lightType; }

	GENERATE_COMPONENT_BODY(Light)
};

REGISTER_COMPONENT(Light)
