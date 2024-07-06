#include "pch.h"
#include "Gizmo.h"
#include "Effects.h"
#include "MathHelper.h"
#include "SceneEditorWindow.h"

void Gizmo::DrawCube(const XMMATRIX& worldMatrix, const Vec3& size)
{
    auto context = Application::GetI()->GetDeviceContext();

    // �ڽ��� 8���� ������ ����
    XMFLOAT3 halfSize = { size.x * 0.5f, size.y * 0.5f, size.z * 0.5f };
    XMFLOAT3 vertices[8] = {
        { -halfSize.x, -halfSize.y, -halfSize.z },
        { -halfSize.x,  halfSize.y, -halfSize.z },
        {  halfSize.x,  halfSize.y, -halfSize.z },
        {  halfSize.x, -halfSize.y, -halfSize.z },
        { -halfSize.x, -halfSize.y,  halfSize.z },
        { -halfSize.x,  halfSize.y,  halfSize.z },
        {  halfSize.x,  halfSize.y,  halfSize.z },
        {  halfSize.x, -halfSize.y,  halfSize.z }
    };

    // ī�޶��� �� �������� ��� ��������
    XMMATRIX viewProj = RenderManager::GetI()->cameraViewProjectionMatrix;
    XMMATRIX worldViewProj = worldMatrix * viewProj;

    // ����Ʈ ũ�� ��������
    D3D11_VIEWPORT viewport;
    UINT numViewports = 1;
    context->RSGetViewports(&numViewports, &viewport); 

    // �������� ��ũ�� ��ǥ�� ��ȯ
    ImVec2 screenVertices[8];
    for (int i = 0; i < 8; ++i)
    {
        XMVECTOR pos = XMVector3TransformCoord(XMLoadFloat3(&vertices[i]), worldViewProj);

        // NDC to screen space conversion
        float x = (XMVectorGetX(pos) / XMVectorGetW(pos)) * 0.5f + 0.5f;
        float y = (XMVectorGetY(pos) / XMVectorGetW(pos)) * -0.5f + 0.5f;
        screenVertices[i] = ImVec2(x * viewport.Width + viewport.TopLeftX, y * viewport.Height + viewport.TopLeftY);
    }

    // ImGui�� ����Ͽ� �ڽ��� ������ �׸�
    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    const ImU32 color = IM_COL32(0, 255, 0, 255);
    const float thickness = 1.0f;

    // �ڽ��� 12���� ���� �׸���
    const int edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };

    for (int i = 0; i < 12; ++i)
    {
        drawList->AddLine(screenVertices[edges[i][0]], screenVertices[edges[i][1]], color, thickness);
    }
}
