#pragma once

class Light;

// Light ������Ʈ ����, ����, ������Ʈ(Light ������Ʈ ���Ե�) ���� or ������ �Ͼ ��
// LightManager�� Light ������Ʈ ���Ե� ������Ʈ �߰�, ����
class LightManager
{
	SINGLE_HEADER(LightManager)
	// Light ������Ʈ�� ���Ե� ������Ʈ��
private:
	vector<Light> m_lightList;
public:
	void Init();

	void SetLight(shared_ptr<Light> light);
	void DeleteLight(InstanceID id);


	// Ÿ�Ժ� Lights ��ȯ, ī�޶� ���� �Ÿ��� �ش��ϴ� ��쿡��
	// ī�޶�� ���� ��ǥ�Ÿ�, ī�޶�� ���� Near, Far���� ����ؾ���
	DirectionalLight GetDirLights();
	PointLight GetPointLights();
	SpotLight GetSpotLights();
};

