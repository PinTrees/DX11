#pragma once

class App;
class GameObject;

class Application
{
	SINGLE_HEADER(Application)

private:
	App* m_pCurrApp;

public:
	void SetApp(App* app) { m_pCurrApp = app; }
	App* GetApp() { return m_pCurrApp; }

public:
	HWND GetMainHwnd();
	HINSTANCE GetInstance();
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

public:
	static wstring GetDataPath();
};

