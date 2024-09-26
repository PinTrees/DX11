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

	// 타입별 Lights 반환, 카메라 렌더 거리에 해당하는 경우에만(적용x)
	vector<DirectionalLight>& GetDirLights();
	vector<PointLight>& GetPointLights();
	vector<SpotLight>& GetSpotLights();
};

