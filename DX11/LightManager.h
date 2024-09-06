#pragma once

class Light;

// Light 컴포넌트 생성, 삭제, 오브젝트(Light 컴포넌트 포함된) 복사 or 삭제가 일어날 때
// LightManager에 Light 컴포넌트 포함된 오브젝트 추가, 삭제
class LightManager
{
	// Light 컴포넌트가 포함된 오브젝트들
private:
	vector<GameObject*> m_lightObj;
public:
	void SetLightObject(GameObject* obj) { m_lightObj.push_back(obj); }
	void DeleteLightObject(GameObject* obj) { std::find(m_lightObj.begin(), m_lightObj.end(), obj); }

	Light* GetLights(); // 모아둔 오브젝트들에서 라이트 클래스들만 추출해서 넘기기
};

