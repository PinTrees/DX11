#pragma once

// ���߿� ��Ÿ���������� ���� �� ������Ʈ �������� ����, ī�޶� �������� ������ �� �ִ� �ִ� ������
// ���ĵ� ���� ����Ʈ�鿡�� �ִ� ����������� �ɷ��� ��
#define LIGHT_SIZE 4 // Light Size�� �� �������� ��������
#define LIGHT_MULTYPLE 6
#define LIGHT_MAX_SIZE (LIGHT_SIZE * LIGHT_MULTYPLE)

class Light;

class LightManager
{
	SINGLE_HEADER(LightManager)

private:
	vector<shared_ptr<Light>> m_lights; // ���� ��� ����

	int m_SortedLightSize = 0;
	int m_SortedEditorLightSize = 0;

	// Frustum Culling, �Ÿ� ��� ����, Type�� ������ �Ϸ�� Light����
	vector<shared_ptr<Light>> m_SortedEditorLights;
	vector<shared_ptr<Light>> m_SortedLights;

	// Frustum Culling, �Ÿ� ��� ���� �� Type���� ��Ƶ� Light����, RenderEffect�� �ѱ�� �뵵
	vector<DirectionalLight> m_EditorDirLights;
	vector<PointLight> m_EditorPointLights;
	vector<SpotLight> m_EditorSpotLights;

	vector<DirectionalLight> m_DirLights;
	vector<PointLight> m_PointLights;
	vector<SpotLight> m_SpotLights;
public:
	void Init();

	void SetLight(shared_ptr<Light> light) { m_lights.push_back(light); }
	void DeleteLight(InstanceID id);

	// Camera Ŭ�������� Frustum �ø� �� ����Ʈ�鸸 ����
	void SortingLights(vector<shared_ptr<Light>> cullingLights, Vec3 cameraPos);
	void SortingEditorLights(vector<shared_ptr<Light>> cullingLights, Vec3 cameraPos);

	int GetSortedLightSize() { return m_SortedLightSize; }
	int GetSortedEditorLightSize() { return m_SortedEditorLightSize; }

	vector<shared_ptr<Light>> GetLights() { return m_lights; }
	vector<shared_ptr<Light>> GetSortedLights() { return m_SortedLights; }
	vector<shared_ptr<Light>> GetSortedEditorLights() { return m_SortedEditorLights; }

	// Ÿ�Ժ� Lights ��ȯ
	vector<DirectionalLight> GetDirLights() { return m_DirLights; }
	vector<PointLight> GetPointLights() { return m_PointLights; }
	vector<SpotLight> GetSpotLights() { return m_SpotLights; }

	vector<DirectionalLight> GetEditorDirLights() { return m_EditorDirLights; }
	vector<PointLight> GetEditorPointLights() { return m_EditorPointLights; }
	vector<SpotLight> GetEditorSpotLights() { return m_EditorSpotLights; }
};

