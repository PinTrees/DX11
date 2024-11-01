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

class Light : public Component
{
private:
	LightType m_lightType = LightType::Directional;

	// 각도를 가지고 있는 구조체 정보는 Transform이랑 연동해야할까?
	DirectionalLight m_directionalDesc;
	PointLight m_pointDesc;
	SpotLight m_spotDesc;

	XMFLOAT2 m_dirLightLen = XMFLOAT2(20.f, 20.f);
	XMFLOAT2 m_spotLightLen = XMFLOAT2(20.f, 20.f);

	XMMATRIX m_lightView;
	XMMATRIX m_lightProj;

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

	virtual void Awake() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void FixedUpdate() override;

	virtual void LastUpdate() override;

	virtual void Render() override;
	virtual void OnInspectorGUI() override;
	virtual void OnDestroy() override;
	virtual void OnDrawGizmos() override;

	void SetDirLight(DirectionalLight light);
	void SetPointLight(PointLight light);
	void SetSpotLight(SpotLight light);

	DirectionalLight GetDirLight() { return m_directionalDesc; }
	PointLight GetPointLight() { return m_pointDesc; }
	SpotLight GetSpotLight() { return m_spotDesc; }

	XMMATRIX GetLightView() { return m_lightView; }
	XMMATRIX GetLightProj() { return m_lightProj; }
	XMMATRIX GetLightViewProj() { return m_lightView * m_lightProj; }

	// LightV * LightP * toTexSpace
	LightType GetLightType() { return m_lightType; }

	GENERATE_COMPONENT_BODY(Light)
};

REGISTER_COMPONENT(Light)
