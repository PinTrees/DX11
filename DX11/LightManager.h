#pragma once

class Light;

// Light ������Ʈ ����, ����, ������Ʈ(Light ������Ʈ ���Ե�) ���� or ������ �Ͼ ��
// LightManager�� Light ������Ʈ ���Ե� ������Ʈ �߰�, ����
class LightManager
{
	// Light ������Ʈ�� ���Ե� ������Ʈ��
private:
	vector<GameObject*> m_lightObj;
public:
	void SetLightObject(GameObject* obj) { m_lightObj.push_back(obj); }
	void DeleteLightObject(GameObject* obj) { std::find(m_lightObj.begin(), m_lightObj.end(), obj); }

	Light* GetLights(); // ��Ƶ� ������Ʈ�鿡�� ����Ʈ Ŭ�����鸸 �����ؼ� �ѱ��
};

