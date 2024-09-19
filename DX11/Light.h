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
	
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	// DirectionalLight FarZ�� �̷л� ���Ѵ������� ���ҽ� ȿ������ ���� (���� ũ�� + ���� ��ǥ�Ÿ�)�� �ϴ°��� �̻����ϵ� �ϴ�.
	// Point, Spot�� ��� Light����ü�� Range�� FarZ�� ��ü, Directinal�� ���� ũ��� ��ü������ ���߿� �� ������ ������ ����
	float m_lightLengthX = 20.0f;
	float m_lightLengthY = 20.0f;
	float m_lightFarZ = 100.0f;		

	XMFLOAT4X4 m_lightView;
	XMFLOAT4X4 m_lightProj;

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
	virtual void ComponentOnDestroy() override;

	DirectionalLight GetDirLight() { return m_directionalDesc; }
	PointLight GetPointLight() { return m_pointDesc; }
	SpotLight GetSpotLight() { return m_spotDesc; }

	XMFLOAT4X4 GetLightView() { return m_lightView; }
	XMFLOAT4X4 GetLightProj() { return m_lightProj; }

	float GetLightLengthX() { return m_lightLengthX; }
	float GetLightLengthY() { return m_lightLengthY; }
	float GetLightFarZ() { return m_lightFarZ; }

	void SetLightLengthX(float x) { m_lightLengthX = x; ProjUpdate(); }
	void SetLightLengthY(float y) { m_lightLengthY = y; ProjUpdate(); }
	void SetLightFarZ(float farZ) { m_lightFarZ = farZ; ProjUpdate(); }


	GENERATE_COMPONENT_BODY(Light)
};

REGISTER_COMPONENT(Light)
