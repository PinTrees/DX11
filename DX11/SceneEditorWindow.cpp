#include "pch.h"
#include "SceneEditorWindow.h"
#include "App.h"
#include "EditorCamera.h"
#include "MathHelper.h"

SceneEditorWindow::SceneEditorWindow()
    : EditorWindow("Scene"),
    windowWidth(800),
    windowHeight(600)
{
    m_Camera = new EditorCamera;
    InitRenderTarget(windowWidth, windowHeight);
    SceneViewManager::GetI()->m_LastActiveSceneEditorWindow = this;
}

SceneEditorWindow::~SceneEditorWindow()
{
    delete m_Camera;
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

    float aspectRatio = static_cast<float>(width) / height;
    m_Camera->SetLens(0.25f * MathHelper::Pi, aspectRatio, 1.0f, 1000.0f);
    PostProcessingManager::GetI()->SetEditorSSAO(width, height, m_Camera);
    
    RenderManager::GetI()->SetEditorViewport(width, height);
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
    XMMATRIX view = m_Camera->View();
    XMMATRIX proj = m_Camera->Proj();
    XMMATRIX viewProj = m_Camera->ViewProj();
    RenderManager::GetI()->cameraViewProjectionMatrix = view * proj;

    auto context = Application::GetI()->GetDeviceContext();

    // ���� ���� Ÿ�� ���
    context->OMGetRenderTargets(1, &oldRenderTarget, nullptr);

    // �� ���� Ÿ�� ����
    context->OMSetRenderTargets(1, &renderTargetView, nullptr);

    // �� ����
    Application::GetI()->GetApp()->OnEditorSceneRender(renderTargetView, m_Camera);

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

    // ���ʿ� ���η� �� �޴� ��ġ
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::BeginChild("LeftPanel", ImVec2(28, windowSize.y), false, ImGuiWindowFlags_NoDecoration); // �׵θ� ����
    ImGui::PopStyleVar(); // �е� ���� ����

    bool isWireframeMode = RenderManager::GetI()->WireFrameMode;

    if (isWireframeMode)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered)); // ���õ� ��ư ����
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    if (ImGui::Button("W", ImVec2(28, 28)))
    {
        RenderManager::GetI()->WireFrameMode = !RenderManager::GetI()->WireFrameMode;
    }
    ImGui::PopStyleVar();

    if (isWireframeMode)
    {
        ImGui::PopStyleColor(); // ����� ���� ����
    }

    // �ٸ� ��ư�� �߰� ����
    ImGui::EndChild();

    ImGui::SameLine();

    windowSize = ImGui::GetContentRegionAvail();
    if (windowWidth != windowSize.x || windowHeight != windowSize.y)
    {
        windowWidth = static_cast<UINT>(windowSize.x);
        windowHeight = static_cast<UINT>(windowSize.y);
        InitRenderTarget(windowWidth, windowHeight);
    }

    // ImGui â�� ���� Ÿ�� �ؽ�ó�� ǥ��
    ImGui::Image(reinterpret_cast<void*>(shaderResourceView), windowSize);

    if (SelectionManager::GetSelectedObjectType() == SelectionType::GAMEOBJECT)
    {
        GameObject* gameObject = SelectionManager::GetSelectedGameObject();

        if (gameObject)
        {
            Gizmo::DrawTransformHandler(gameObject->GetTransform());
        }
    }

    SceneManager::GetI()->GetCurrentScene()->RenderSceneGizmos(); 
}

void SceneEditorWindow::OnRenderExit()
{
    
}
