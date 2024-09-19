#pragma once

class Light;

// Light ������Ʈ ����, ����, ������Ʈ(Light ������Ʈ ���Ե�) ���� or ������ �Ͼ ��
// LightManager�� Light ������Ʈ ���Ե� ������Ʈ �߰�, ����
class LightManager
{
	// Light ������Ʈ�� ���Ե� ������Ʈ��
private:
	vector<shared_ptr<Light>> m_lightList;
public:
	void Init();

	// Ÿ�Ժ� Lights ��ȯ, ī�޶� ���� �Ÿ��� �ش��ϴ� ��쿡��
	// ī�޶�� ���� ��ǥ�Ÿ�, ī�޶�� ���� Near, Far���� ����ؾ���
	DirectionalLight GetDirLights();
	PointLight GetPointLights();
	SpotLight GetSpotLights();
};

