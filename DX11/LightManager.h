#pragma once

class Light;

// Light ������Ʈ ����, ����, ������Ʈ(Light ������Ʈ ���Ե�) ���� or ������ �Ͼ ��
// LightManager�� Light ������Ʈ ���Ե� ������Ʈ �߰�, ����
class LightManager
{
	SINGLE_HEADER(LightManager)
	// Light ������Ʈ�� ���Ե� ������Ʈ��
private:
	vector<shared_ptr<Light>> m_lightList;

	vector<DirectionalLight> m_dirLights;
	vector<PointLight> m_pointLights;
	vector<SpotLight> m_spotLights;
public:
	void Init();

	void SetLight(shared_ptr<Light> light) { m_lightList.push_back(light); }
	void DeleteLight(InstanceID id);

	// Ÿ�Ժ� Lights ��ȯ, ī�޶� ���� �Ÿ��� �ش��ϴ� ��쿡��
	vector<DirectionalLight>& GetDirLights();
	vector<PointLight>& GetPointLights();
	vector<SpotLight>& GetSpotLights();
};

