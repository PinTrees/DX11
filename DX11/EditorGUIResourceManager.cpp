#include "pch.h"
#include "EditorGUIResourceManager.h"

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
        else
        {
            return m_FontMap[fontKey];
        }
    }

    m_LoadFontTask[fontKey] = true;
    FontLoadContainer fontContainer;
    fontContainer.FontKey = fontKey;
    fontContainer.FontStyle = style;

    //Task<ImFont*> fontFuture = LoadFontAsync(fontContainer); 
    //ImFont* font = fontFuture.get();  // ��Ʈ �ε尡 �Ϸ�Ǹ� ��Ʈ�� ����
    //
    //if (font) {
    //    m_FontMap[fontKey] = font;
    //    m_LoadFontTask[fontKey] = false;
    //    return m_FontMap[fontKey];
    //}
    //else {
    //}

    return nullptr;  // �۾��� �Ϸ�Ǳ� �������� nullptr ��ȯ
}

void EditorGUIResourceManager::LastUpdate()
{
    // ������ �ε� ����Ʈ�� �ִ� ��� ��Ʈ�� ������ �ε�
    for (const auto& fontContainer : loadedAA)
    {
        const string& fontPath = fontContainer.FontStyle.Bold ? "C:\\Windows\\Fonts\\malgunbd.ttf" : "C:\\Windows\\Fonts\\malgun.ttf";
        const EditorTextStyle& style = fontContainer.FontStyle;

        // ��Ʈ ��ο� ũ�⸦ �����Ͽ� ����ũ Ű ����
        std::string fontKey = std::to_string(style.FontSize) + (style.Bold ? "_bold" : "_regular");

        // ��Ʈ�� �̹� �ε�� ��� �ٷ� ��ȯ
        if (m_FontMap.find(fontKey) != m_FontMap.end())
        {
            continue;
        }

        ImGuiIO& io = ImGui::GetIO();
        ImFont* font = io.Fonts->AddFontFromFileTTF(
            fontPath.c_str(),
            style.FontSize,
            NULL,
            io.Fonts->GetGlyphRangesKorean()
        );

        if (font)
        {
            // �ε�� ��Ʈ�� �ʿ� ����
            m_FontMap[fontKey] = font;
        }
        else
        {
            // �ε� ���н� �α� ���
            fprintf(stderr, "Failed to load font: %s with size: %.1f\n", fontPath.c_str(), style.FontSize);
        }

        io.Fonts->Build();
    }

    // ��� ������ �ε带 ó�������Ƿ� ����Ʈ ���
    loadedAA.clear();

    // ��Ʈ �ؽ�ó ������Ʈ
    ImGui_ImplDX11_InvalidateDeviceObjects();
    ImGui_ImplDX11_CreateDeviceObjects();       // �ؽ�ó ���� �� ���ε�
}

Task<ImFont*> EditorGUIResourceManager::LoadFontAsync(FontLoadContainer container)
{
    const string& fontPath = container.FontStyle.Bold ? "C:\\Windows\\Fonts\\malgunbd.ttf" : "C:\\Windows\\Fonts\\malgun.ttf";
    const EditorTextStyle& style = container.FontStyle;

    std::string fontKey = std::to_string(style.FontSize) + (style.Bold ? "_bold" : "_regular");

    ImGuiIO& io = ImGui::GetIO();
    ImFont* font = io.Fonts->AddFontFromFileTTF(
        fontPath.c_str(),
        style.FontSize,
        NULL,
        io.Fonts->GetGlyphRangesKorean()
    );

    if (font)
    {
        //co_await io.Fonts->GetGlyphRangesKorean(); 
        //co_await TaskSystem.EndFrame(); // �񵿱� ������ ������

        // ��Ʈ �ؽ�ó ������Ʈ
        io.Fonts->Build();
        //ImGui_ImplDX11_InvalidateDeviceObjects();
        //ImGui_ImplDX11_CreateDeviceObjects();      

        co_return font;  // �񵿱������� ��Ʈ ��ȯ
    }
    else
    {
        // �ε� ���н� �α� ���
        fprintf(stderr, "Failed to load font: %s with size: %.1f\n", fontPath.c_str(), style.FontSize);
    }

    co_return nullptr;  // ��Ʈ �ε� ���� �� nullptr ��ȯ
}
