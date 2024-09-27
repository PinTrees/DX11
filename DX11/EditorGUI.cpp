#include "pch.h"
#include "EditorGUI.h"
#include "EditorGUIResourceManager.h"

EditorTextStyle EditorGUI::defaultTextStyle = EditorTextStyle();

void EditorGUI::RowSizedBox(float size)
{
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(size, 0));
    ImGui::SameLine();
}

void EditorGUI::Image(wstring path, ImVec2 size)
{
    ComPtr<ID3D11ShaderResourceView> icon = ResourceManager::GetI()->LoadTexture(path);
    if (icon)
    {
        ImGui::Image((void*)icon.Get(), ImVec2(18, 18));
    }
}

void EditorGUI::Checkbox()
{
    bool isChecked = true;
    // üũ�ڽ� ũ�� ����
    ImVec2 checkBoxSize(18, 18);

    // ��Ÿ�� ����
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));         // �⺻ ��� ����
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));  // ȣ�� �� ��� ����
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.30f, 0.30f, 0.30f, 1.0f));   // Ŭ�� �� ��� ����
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));            // �׵θ� ����

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);  // ���� ����
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f); // �׵θ� �β� ����

    // 20x20 ũ���� ��ư�� üũ�ڽ��� ���
    if (ImGui::Button("##Checkbox", checkBoxSize))
    {
        isChecked = !isChecked;  // Ŭ�� �� üũ ���� ����
    }

    // üũ ���¿� ���� üũ ǥ�� �׸���
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 p_min = ImGui::GetItemRectMin();
    ImVec2 p_max = ImGui::GetItemRectMax();

    if (true)
    {
        // üũ ǥ�� (V ǥ�ø� �׸��� �κ�)
        ImVec2 center = ImVec2((p_min.x + p_max.x) * 0.5f, (p_min.y + p_max.y) * 0.5f);
        draw_list->AddLine(ImVec2(p_min.x + 4, center.y), ImVec2(center.x - 2, p_max.y - 4), ImColor(255, 255, 255, 255), 2.0f); // ���� �Ʒ���
        draw_list->AddLine(ImVec2(center.x - 2, p_max.y - 4), ImVec2(p_max.x - 4, p_min.y + 4), ImColor(255, 255, 255, 255), 2.0f); // ������ ����
    }

    // ��Ÿ�� ����
    ImGui::PopStyleVar(2);  // ������ �׵θ� ����
    ImGui::PopStyleColor(4); // ���� ����
}

bool EditorGUI::InputField(string& inputText)
{
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);

    bool changed = ImGui::InputText("##Name", &inputText[0], inputText.capacity() + 1);

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(2);

    return changed;
}

void EditorGUI::Label(string text, EditorTextStyle guiStyle)
{
    ImFont* font = EditorGUIResourceManager::GetI()->LoadFont(guiStyle);

    if (font == nullptr)
    {
        ImGui::Text(text.c_str());
        return;
    }

    ImGui::PushFont(font);
    ImGui::Text(text.c_str());
    ImGui::PopFont();
}

void EditorGUI::LabelHeader(string text)
{
    EditorTextStyle style;
    style.FontSize = 18;
    style.Bold = true;
    ImFont* font = EditorGUIResourceManager::GetI()->LoadFont(style);

    EDITOR_GUI_FIELD_PADDING;

    if (font == nullptr)
    {
        ImGui::Text(text.c_str());
        return;
    }

    ImGui::PushFont(font);
    ImGui::Text(text.c_str());
    ImGui::PopFont();
}

bool EditorGUI::Button(string text, Vec2 size, Color color)
{
    // Unity ��ư ���� �´� ImGui ���� ����
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(color.x, color.y, color.z, color.w));  // (�⺻ ��ư ���)
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.37f, 0.37f, 0.37f, 1.0f));    // (���콺 �÷��� ��)
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.43f, 0.43f, 0.43f, 1.0f));     // (Ŭ������ ��)
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));                // (�ؽ�Ʈ)
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 0));  // �е� (�¿�, ����) 2px ����
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));  // �е� (�¿�, ����) 2px ����
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);         // ���� �� 2 ����
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.4f);       // �׵θ� �β� 1 ����

    // ImGui ��ư ����
    bool clicked = ImGui::Button(text.c_str(), ImVec2(size.x, size.y));

    ImGui::PopStyleVar(4);   // ����� �׵θ� �β� ��Ÿ�� ����
    ImGui::PopStyleColor(5); // �׵θ��� ��ư ���� ��Ÿ�� ���� (�� 5���� ��Ÿ�� ��)

    return clicked;
}

bool EditorGUI::ComponentHeader(string title)
{
    // ȸ�� ���� ����
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.24f, 0.24f, 0.24f, 1.0f));            // �⺻ ����
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.37f, 0.37f, 0.37f, 1.0f));     // ���콺 ���� ��
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.43f, 0.43f, 0.43f, 1.0f));      // Ŭ�� ����

    // �е��� �ձ۱� ����
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 2));   // �е��� �������� �����Ͽ� ���� ����
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

    EditorTextStyle fontStyle;
    fontStyle.Bold = true;
    fontStyle.FontSize = 16;
    ImFont* font = EditorGUIResourceManager::GetI()->LoadFont(fontStyle);

    if (font != nullptr)
        ImGui::PushFont(font);

    bool isOpened = ImGui::CollapsingHeader(title.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

    // ��Ÿ�� ����
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(3);    // ���� ���� (3���� ����)

    if (font != nullptr)
        ImGui::PopFont();

    return isOpened;
}

void EditorGUI::ComponentDivider()
{
    Divider(Color(0, 0, 0, 1.0f));
}

void EditorGUI::Divider(Color color, float height)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float lineHeight = height;
    ImU32 imColor = ImGui::GetColorU32(ImVec4(color.x, color.y, color.z, color.w));

    draw_list->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x + ImGui::GetContentRegionAvail().x, p.y + lineHeight), imColor);

    ImGui::Dummy(ImVec2(0.0f, lineHeight));

    ImGui::PopStyleVar(3);
}

void EditorGUI::Spacing(Vec2 size)
{
    ImGui::Dummy(ImVec2(size.x, size.y));
}

bool EditorGUI::FloatField(string title, float& v, ImVec2 size)
{
    bool isDirty = false;

    if (ImGui::BeginChild((title + "Float Field Group").c_str(), ImVec2(size.x, FIELD_DEFAULT_HEIGHT + 2), true,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
    {
        EditorGUI::Label(title);
        ImGui::SameLine();

        EditorGUI::DropFieldStylePush();
        if (ImGui::BeginChild((title + "Float Field").c_str(), ImVec2(ImGui::GetContentRegionAvail().x, FIELD_DEFAULT_HEIGHT), true,
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
        {
            string id = "##" + title;
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));            // �巡�� �ʵ� ����� �����ϰ� ����
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0, 0, 0, 0));     // ȣ�� �� ������ �����ϰ� ����
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0, 0, 0, 0));      // Ŭ�� �� ������ �����ϰ� ����

            if (ImGui::DragFloat(id.c_str(), &v, 0.1f, -FLT_MAX, FLT_MAX, "%.3f"))
            {
                isDirty = true;
            }
            ImGui::PopStyleColor(3);

            ImGui::PopItemWidth();
        }
        ImGui::EndChild();  // Child ��
        EditorGUI::DropFieldStylePop();
    }
    ImGui::EndChild();  // Child ��

    return isDirty;
}

bool EditorGUI::Vector3Field(string title, Vec3& vec3)
{
    bool isDirty = false;
    float fieldWidth = EDITOR_GUI_FIELD_WIDTH;

    EDITOR_GUI_FIELD_PADDING;
    EditorGUI::Label(title);
    EDITOR_GUI_FIELD_SPACING(fieldWidth - 8);

    EditorGUI::FieldStylePush();

    string id = title + "Vector3 Field";
    if (ImGui::BeginChild(id.c_str(), ImVec2(fieldWidth, FIELD_DEFAULT_HEIGHT), true,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
    {
        float inputWidth = (fieldWidth) / 3;

        if (EditorGUI::FloatField("X", vec3.x, ImVec2(inputWidth, 0)))
            isDirty = true;
        ImGui::SameLine();
        if (EditorGUI::FloatField("Y", vec3.y, ImVec2(inputWidth, 0)))
            isDirty = true;
        ImGui::SameLine();
        if (EditorGUI::FloatField("Z", vec3.z, ImVec2(inputWidth, 0)))
            isDirty = true;
    }
    ImGui::EndChild();

    EditorGUI::FieldStylePop();

    return isDirty;
}

bool EditorGUI::MaterialField(string title, shared_ptr<UMaterial>& material, wstring& materialPath)
{
    bool isDirty = false;
    float fieldWidth = EDITOR_GUI_FIELD_WIDTH;

    EDITOR_GUI_FIELD_PADDING;
    EditorGUI::Label(title);
    EDITOR_GUI_FIELD_SPACING(fieldWidth);

    EditorGUI::DropFieldStylePush();

    if (ImGui::BeginChild("Meterial Field", ImVec2(fieldWidth, FIELD_DEFAULT_HEIGHT), true,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse));
    {
        // Material �̸� �Ǵ� "None" ���
        if (material == nullptr)
        {
            EditorGUI::Label("None");
        }
        else
        {
            EditorGUI::Label(material->GetName());
        }

        ImGui::SameLine();
        ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x - EDITOR_GUI_BUTTON_WIDTH, 0));
        ImGui::SameLine();

        if (EditorGUI::Button("+", Vec2(20, 20), Color(0.20f, 0.20f, 0.20f, 1.0f)))
        {
            wstring filePath = EditorUtility::OpenFileDialog(L"", L"Material", { L"mat" });
            filePath = PathManager::GetI()->GetCutSolutionPath(filePath);

            if (!filePath.empty())
            {
                material = ResourceManager::GetI()->LoadMaterial(wstring_to_string(filePath));
                materialPath = filePath;
                isDirty = true;
            }
        }
    }
    ImGui::EndChild();

    EditorGUI::DropFieldStylePop();

    return isDirty;
}

bool EditorGUI::MeshField(string title, shared_ptr<Mesh>& mesh, int& subsetIndex)
{
    bool isDirty = false;
    float fieldWidth = EDITOR_GUI_FIELD_WIDTH;

    EDITOR_GUI_FIELD_PADDING;
    EditorGUI::Label(title);
    EDITOR_GUI_FIELD_SPACING(fieldWidth - 8);

    EditorGUI::DropFieldStylePush();

    if (ImGui::BeginChild("Mesh Field", ImVec2(fieldWidth, FIELD_DEFAULT_HEIGHT), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
    {
        if (mesh == nullptr)
        {
            EditorGUI::Label("None");
        }
        else
        {
            ImGui::PushItemWidth(fieldWidth - (EDITOR_GUI_BUTTON_WIDTH + 4));

            if (ImGui::BeginCombo("##Subset Dropdown", mesh->Subsets[subsetIndex].Name.c_str()))
            {
                for (int n = 0; n < mesh->Subsets.size(); n++)
                {
                    bool is_selected = (subsetIndex == n);
                    if (ImGui::Selectable(mesh->Subsets[n].Name.c_str(), is_selected))
                    {
                        subsetIndex = n;
                        isDirty = true;
                    }
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::PopItemWidth();
        }

        ImGui::SameLine();
        ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x - EDITOR_GUI_BUTTON_WIDTH, 0));
        ImGui::SameLine();

        if (EditorGUI::Button("+", Vec2(20, 20), Color(0.20f, 0.20f, 0.20f, 1.0f)))
        {
            wstring filePath = EditorUtility::OpenFileDialog(PathManager::GetI()->GetMovePathW(L"Assets\\"), L"Mesh", { L"fbx" });
            filePath = PathManager::GetI()->GetCutSolutionPath(filePath);

            if (!filePath.empty())
            {
                auto newMesh = ResourceManager::GetI()->LoadMesh(filePath);
                if (newMesh)
                {
                    mesh = newMesh;
                    subsetIndex = 0;
                    isDirty = true;
                }
            }
        }
    }
    ImGui::EndChild();

    EditorGUI::DropFieldStylePop();

    return isDirty;
}




void EditorGUI::DropFieldStylePush()
{
    // ���ϵ� â�� ��� ����� �׵θ� ���� ����
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

    // ���ϵ� â�� ������ ���� �е� ����
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 4.0f);         // ���ϵ� â ���� �� 2 ���� 
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.f);      // ���ϵ� â �׵θ� �β� ����
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 0)); // ���ϵ� â ���� �е�
}
void EditorGUI::DropFieldStylePop()
{
    // ������ ��Ÿ�� ���� �� ���� ����
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);
}

void EditorGUI::FieldStylePush()
{
    // ���ϵ� â�� ��� ����� �׵θ� ���� ����
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.f, 0.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.f, 0.f, 0.f, 0.f));

    // ���ϵ� â�� ������ ���� �е� ����
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 4.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 0)); // ���ϵ� â ���� �е�
}

void EditorGUI::FieldStylePop()
{
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);
}

void EditorGUI::ComponentBlockStylePush()
{
    // ���� ȸ�� ��� (#333333 in hex) ����
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

    // �׵θ� �β� ����
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
}

void EditorGUI::ComponentBlockStylePop()
{
    ImGui::PopStyleVar(6);
    ImGui::PopStyleColor(2);
}

void EditorGUI::EditorWindowStylePush()
{
    // ���� ȸ�� ��� ���� (#333333 in hex) ����
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));  // Unity ��Ÿ���� ���� ȸ�� ���
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);  // ��� �𼭸� �ձ۰� (6.0f�� ����)

    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);                   // �׵θ� �β� ����
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));     // �׵θ� ���� (��ο� ȸ��)

    // â �е� ���� (0���� �����Ͽ� ���� ���� ����)
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
}

void EditorGUI::EditorWindowStylePop()
{
    // ��Ÿ�� ����
    ImGui::PopStyleVar(3);  // �׵θ� �β� �� ���� ����
    ImGui::PopStyleColor(2);  // ��� ����� �׵θ� ���� ����
}