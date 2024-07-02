#include "pch.h"
#include "SceneEditorWindow.h"
#include "App.h"

SceneEditorWindow::SceneEditorWindow()
    : EditorWindow("Scene"),
    windowWidth(800),
    windowHeight(600)
{
    InitRenderTarget(windowWidth, windowHeight);
}

SceneEditorWindow::~SceneEditorWindow()
{
}


void SceneEditorWindow::InitRenderTarget(UINT width, UINT height)
{
    CleanUpRenderTarget(); // ���� ���� Ÿ���� ������ ����

    // ���� Ÿ�� �ؽ�ó ����
    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    auto device = Application::GetI()->GetDevice();

    HRESULT hr = device->CreateTexture2D(&textureDesc, nullptr, &renderTargetTexture);
    if (FAILED(hr)) { /* ���� ó�� */ }

    // ���� Ÿ�� �� ����
    hr = device->CreateRenderTargetView(renderTargetTexture, nullptr, &renderTargetView);
    if (FAILED(hr)) { /* ���� ó�� */ }

    // ���̴� ���ҽ� �� ����
    hr = device->CreateShaderResourceView(renderTargetTexture, nullptr, &shaderResourceView);
    if (FAILED(hr)) { /* ���� ó�� */ }
}

void SceneEditorWindow::CleanUpRenderTarget()
{
    if (renderTargetView)
    {
        renderTargetView->Release();
        renderTargetView = nullptr;
    }

    if (shaderResourceView)
    {
        shaderResourceView->Release();
        shaderResourceView = nullptr;
    }

    if (renderTargetTexture)
    {
        renderTargetTexture->Release();
        renderTargetTexture = nullptr;
    }
}


void SceneEditorWindow::Update()
{
    auto context = Application::GetI()->GetDeviceContext();

    // ���� ���� Ÿ�� ���
    context->OMGetRenderTargets(1, &oldRenderTarget, nullptr);

    // �� ���� Ÿ�� ����
    context->OMSetRenderTargets(1, &renderTargetView, nullptr);

    // �� ����
    Application::GetI()->GetApp()->OnRender(renderTargetView);

    if (oldRenderTarget == nullptr)
        return;

    // ���� ���� Ÿ�� ����
    context->OMSetRenderTargets(1, &oldRenderTarget, nullptr);
    if (oldRenderTarget) oldRenderTarget->Release();

    oldRenderTarget = nullptr;
}

void SceneEditorWindow::OnRender()
{
    ImVec2 windowSize = ImGui::GetContentRegionAvail();
    if (windowWidth != windowSize.x || windowHeight != windowSize.y)
    {
        windowWidth = static_cast<UINT>(windowSize.x);
        windowHeight = static_cast<UINT>(windowSize.y);
        InitRenderTarget(windowWidth, windowHeight);
    }

    // ImGui â�� ���� Ÿ�� �ؽ�ó�� ǥ��
    ImGui::Image(reinterpret_cast<void*>(shaderResourceView), windowSize);
}

void SceneEditorWindow::OnRenderExit()
{
}
