#include "pch.h"
#include "Application.h"
#include "App.h"

SINGLE_BODY(Application)

Application::Application()
	: m_pCurrApp(nullptr)
{

}

Application::~Application()
{

}


HWND Application::GetMainHwnd()
{
	return m_pCurrApp->MainWnd();
}

HINSTANCE Application::GetInstance()
{
	return m_pCurrApp->AppInst();
}

ID3D11Device* Application::GetDevice()
{
	return m_pCurrApp->GetDevice();
}

ID3D11DeviceContext* Application::GetDeviceContext()
{
	return m_pCurrApp->GetDeviceContext();
}
