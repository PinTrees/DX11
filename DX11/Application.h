#pragma once

class App;
class GameObject;

class Application
{
	SINGLE_HEADER(Application)

private:
	App* m_pCurrApp;
	GameObject* m_pCurSelectGameObject;

public:
	void SetApp(App* app) { m_pCurrApp = app; }
	App* GetApp() { return m_pCurrApp; }

	void SetCurSelectGameObject(GameObject* gameObject) { m_pCurSelectGameObject = gameObject; }
	GameObject* GetCurSelectGameObject() { return m_pCurSelectGameObject; }

public:
	HWND GetMainHwnd();
	HINSTANCE GetInstance();
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
};

