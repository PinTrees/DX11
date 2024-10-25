#include "pch.h"
#include "GameViewEditorWindow.h"
#include "MathHelper.h"
#include "App.h"

GameViewEditorWindow::GameViewEditorWindow()
	: EditorWindow("Game"),
    windowWidth(800),
    windowHeight(600)
{
    InitRenderTarget(windowWidth, windowHeight); 
}

GameViewEditorWindow::~GameViewEditorWindow()
{
}


void GameViewEditorWindow::InitRenderTarget(UINT width, UINT height)
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

    auto camera = DisplayManager::GetI()->GetActiveCamera();
    if (camera) 
    {
        float aspectRatio = static_cast<float>(width) / height;
        camera->SetAspect(aspectRatio); 
        PostProcessingManager::GetI()->SetSSAO(width, height, camera.get()); 
    }
    
    RenderManager::GetI()->SetViewport(width, height); 
}

void GameViewEditorWindow::CleanUpRenderTarget()
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

void GameViewEditorWindow::RenderScene()
{
    auto camera = DisplayManager::GetI()->GetActiveCamera();  

    if (camera == nullptr)
        return;

    auto context = Application::GetI()->GetDeviceContext();

    // ���� ���� Ÿ�� ���
    context->OMGetRenderTargets(1, &oldRenderTarget, nullptr);

    // �� ���� Ÿ�� ����
    context->OMSetRenderTargets(1, &renderTargetView, nullptr);

    // �� ����
    camera->LateUpdate();
    XMMATRIX view = camera->View(); 
    XMMATRIX proj = camera->Proj(); 
    XMMATRIX viewProj = camera->ViewProj(); 
    RenderManager::GetI()->CameraViewProjectionMatrix = view * proj; 

    Application::GetI()->GetApp()->OnSceneRender(renderTargetView, camera.get()); 

    if (oldRenderTarget == nullptr)
        return;

    // ���� ���� Ÿ�� ����
    context->OMSetRenderTargets(1, &oldRenderTarget, nullptr);
    if (oldRenderTarget) oldRenderTarget->Release();

    oldRenderTarget = nullptr;
}

void GameViewEditorWindow::OnRender()
{
    ImVec2 windowSize = ImGui::GetContentRegionAvail();

    // ������ ���� ����
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::BeginChild("MainArea", ImVec2(windowSize.x, windowSize.y), false, ImGuiWindowFlags_NoDecoration);
    ImGui::PopStyleVar();

    windowSize = ImGui::GetContentRegionAvail();
    if (windowWidth != windowSize.x || windowHeight != windowSize.y)
    {
        windowWidth = static_cast<UINT>(windowSize.x);
        windowHeight = static_cast<UINT>(windowSize.y);
        InitRenderTarget(windowWidth, windowHeight);
    }

    RenderScene();

    // ImGui â�� ���� Ÿ�� �ؽ�ó�� ǥ��
    ImGui::Image(reinterpret_cast<void*>(shaderResourceView), windowSize);
    ImGui::EndChild(); // MainArea ����
}
