#include "pch.h"
#include "Application.h"
#include "App.h"
#include <filesystem>

namespace fs = std::filesystem;

SINGLE_BODY(Application)

bool Application::isPlaying = false;

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

wstring Application::GetDataPath()
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");
    std::string executablePath = std::string(buffer).substr(0, pos);

    // �ַ�� ��θ� ã�� ���� �θ� ���丮�� Ž��
    fs::path currentPath = executablePath;
    while (!currentPath.empty()) {
        for (const auto& entry : fs::directory_iterator(currentPath)) {
            if (entry.path().extension() == ".sln") {
                // �ַ�� ��ΰ� �߰ߵǸ� Assets ��θ� ��ȯ
                fs::path assetsPath = currentPath / "Assets/";
                if (!fs::exists(assetsPath)) {
                    fs::create_directory(assetsPath);
                }
                return assetsPath.wstring();
            }
        }
        currentPath = currentPath.parent_path();
    }

    // �ַ�� ��θ� ã�� ���� ��� ���� ���� ��ο� Assets ���� ����
    fs::path fallbackAssetsPath = fs::path(executablePath) / "Assets/";
    if (!fs::exists(fallbackAssetsPath)) {
        fs::create_directory(fallbackAssetsPath);
    }
    return fallbackAssetsPath.wstring();
}
