#pragma once

class Light;

class LightManager
{
	SINGLE_HEADER(LightManager)

private:
	vector<shared_ptr<Light>> m_lightList;

	vector<DirectionalLight> m_dirLights;
	vector<PointLight> m_pointLights;
	vector<SpotLight> m_spotLights;
public:
	void Init();

	void SetLight(shared_ptr<Light> light) { m_lightList.push_back(light); }
	void DeleteLight(InstanceID id);

	vector<shared_ptr<Light>>& GetLights() { return m_lightList; }

	// Ÿ�Ժ� Lights ��ȯ, ī�޶� ���� �Ÿ��� �ش��ϴ� ��쿡��(����x)
	vector<DirectionalLight>& GetDirLights();
	vector<PointLight>& GetPointLights();
	vector<SpotLight>& GetSpotLights();
};

