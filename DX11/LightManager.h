#pragma once

class Light;

// Light 컴포넌트 생성, 삭제, 오브젝트(Light 컴포넌트 포함된) 복사 or 삭제가 일어날 때
// LightManager에 Light 컴포넌트 포함된 오브젝트 추가, 삭제
class LightManager
{
	SINGLE_HEADER(LightManager)
	// Light 컴포넌트가 포함된 오브젝트들
private:
	vector<Light> m_lightList;
public:
	void Init();

	void SetLight(shared_ptr<Light> light);
	void DeleteLight(InstanceID id);


	// 타입별 Lights 반환, 카메라 렌더 거리에 해당하는 경우에만
	// 카메라와 빛의 좌표거리, 카메라와 빛의 Near, Far까지 고려해야함
	DirectionalLight GetDirLights();
	PointLight GetPointLights();
	SpotLight GetSpotLights();
};

