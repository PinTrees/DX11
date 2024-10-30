#include "pch.h"
#include "EditorGUIResourceManager.h"
#include "TaskSystem.h"

SINGLE_BODY(EditorGUIResourceManager);

EditorGUIResourceManager::EditorGUIResourceManager()
{

}

EditorGUIResourceManager::~EditorGUIResourceManager()
{
    for (auto& entry : m_FontMap)
    {
        ImFont* font = entry.second;
        if (font != nullptr)
        {
        }
    }

    m_FontMap.clear();
}

void EditorGUIResourceManager::Init()
{
}


ImFont* EditorGUIResourceManager::LoadFont(EditorTextStyle style)
{
    std::string fontKey = std::to_string(style.FontSize) + (style.Bold ? "_bold" : "_regular");

    if (m_FontMap.find(fontKey) != m_FontMap.end())
    {
        return m_FontMap[fontKey];
    }

    if (m_LoadFontTask.find(fontKey) != m_LoadFontTask.end())
    {
        if (!m_LoadFontTask[fontKey])
        {
            return nullptr;
        }
        return nullptr;
    }

    m_LoadFontTask[fontKey] = true;
    FontLoadContainer fontContainer;
    fontContainer.FontKey = fontKey;
    fontContainer.FontStyle = style;

    LoadFontAsync(fontContainer);  
    //ImFont* font = fontFuture.get();  // ��Ʈ �ε尡 �Ϸ�Ǹ� ��Ʈ�� ���� 
    
    return nullptr;  // �۾��� �Ϸ�Ǳ� �������� nullptr ��ȯ
}

void EditorGUIResourceManager::LoadFontAsync(FontLoadContainer container)
{
    const string& fontPath = container.FontStyle.Bold ? "C:\\Windows\\Fonts\\malgunbd.ttf" : "C:\\Windows\\Fonts\\malgun.ttf";
    const EditorTextStyle& style = container.FontStyle;

    string fontKey = to_string(style.FontSize) + (style.Bold ? "_bold" : "_regular");

    // �񵿱� �۾��� ����� ������ promise ��ü
    auto fontPromise = make_shared<promise<ImFont*>>(); 
    future<ImFont*> fontFuture = fontPromise->get_future();

    TaskSystem::mainThreadTasks.push([fontKey, fontPath, style, fontPromise, this]()
    {
        ImFontConfig config;
        config.MergeMode = true; // ���� ��Ʈ�� ���� ��� 
        config.PixelSnapH = true;
        static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 }; // FontAwesome ����   

        ImGuiIO& io = ImGui::GetIO(); 
        ImFont* font = io.Fonts->AddFontFromFileTTF( 
            fontPath.c_str(), 
            style.FontSize, 
            NULL, 
            io.Fonts->GetGlyphRangesKorean() 
        );
        // Font Awesome ��Ʈ �߰�
        string fa_path = PathManager::GetI()->GetContentPathS() + "ProjectSetting\\fonts\\fa-solid-900.ttf";
        io.Fonts->AddFontFromFileTTF( 
            fa_path.c_str(),
            style.FontSize - 2,
            &config, icons_ranges);  
         
        if (font)
        { 
            io.Fonts->Build(); 
            ImGui_ImplDX11_InvalidateDeviceObjects();
            ImGui_ImplDX11_CreateDeviceObjects();

            m_LoadFontTask[fontKey] = false;
            this->m_FontMap[fontKey] = font; 
            // �۾� �Ϸ� �� promise�� ��� ����
            fontPromise->set_value(font);
        }
        else
        {
            fontPromise->set_value(nullptr);
        }
    });

    // �񵿱������� ��Ʈ�� ��ȯ (�۾��� ���� ������ ��ٸ���)
    return; 
}
