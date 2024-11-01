#pragma once
#include "Component.h"
#include "LightHelper.h"

class Light
	: public Component
{
private:
	LightType m_lightType = LightType::Directional;

	// 각도를 가지고 있는 구조체 정보는 Transform이랑 연동해야할까?
	DirectionalLight m_directionalDesc;
	PointLight m_pointDesc;
	SpotLight m_spotDesc;

	vector<XMMATRIX> m_LightView;
	XMMATRIX m_LightProj;

	vector<XMMATRIX> m_editorLightView;
	XMMATRIX m_editorLightProj;

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

	virtual void OnDestroy() override;

	virtual void OnInspectorGUI() override;
	virtual void OnDrawGizmos() override;

	void SetDirLight(DirectionalLight light);
	void SetPointLight(PointLight light);
	void SetSpotLight(SpotLight light);

	DirectionalLight GetDirLight() { return m_directionalDesc; }
	PointLight GetPointLight() { return m_pointDesc; }
	SpotLight GetSpotLight() { return m_spotDesc; }

	float GetRange();

	vector<XMMATRIX> GetEditorLightViewArray() { return m_editorLightView; }
	XMMATRIX GetEditorLightView() { return m_editorLightView[0]; }
	XMMATRIX GetEditorLightProjection() { return m_editorLightProj; }
	XMMATRIX GetEditorLightViewProjection(int index) { return m_editorLightView[index] * m_editorLightProj; }

	vector<XMMATRIX> GetLightViewArray() { return m_LightView; }
	XMMATRIX GetLightView() { return m_LightView[0]; }
	XMMATRIX GetLightProjection() { return m_LightProj; }
	XMMATRIX GetLightViewProjection(int index) { return m_LightView[index] * m_LightProj; }

	// LightV * LightP * toTexSpace
	LightType GetLightType() { return m_lightType; }

	GENERATE_COMPONENT_BODY(Light)
};

REGISTER_COMPONENT(Light)
