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
    //ImFont* font = fontFuture.get();  // 폰트 로드가 완료되면 폰트를 얻음
    //
    //if (font) {
    //    m_FontMap[fontKey] = font;
    //    m_LoadFontTask[fontKey] = false;
    //    return m_FontMap[fontKey];
    //}
    //else {
    //}

    return nullptr;  // 작업이 완료되기 전까지는 nullptr 반환
}

void EditorGUIResourceManager::LastUpdate()
{
    // 지연된 로드 리스트에 있는 모든 폰트를 실제로 로드
    for (const auto& fontContainer : loadedAA)
    {
        const string& fontPath = fontContainer.FontStyle.Bold ? "C:\\Windows\\Fonts\\malgunbd.ttf" : "C:\\Windows\\Fonts\\malgun.ttf";
        const EditorTextStyle& style = fontContainer.FontStyle;

        // 폰트 경로와 크기를 결합하여 유니크 키 생성
        std::string fontKey = std::to_string(style.FontSize) + (style.Bold ? "_bold" : "_regular");

        // 폰트가 이미 로드된 경우 바로 반환
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
            // 로드된 폰트를 맵에 저장
            m_FontMap[fontKey] = font;
        }
        else
        {
            // 로드 실패시 로그 출력
            fprintf(stderr, "Failed to load font: %s with size: %.1f\n", fontPath.c_str(), style.FontSize);
        }

        io.Fonts->Build();
    }

    // 모든 지연된 로드를 처리했으므로 리스트 비움
    loadedAA.clear();

    // 폰트 텍스처 업데이트
    ImGui_ImplDX11_InvalidateDeviceObjects();
    ImGui_ImplDX11_CreateDeviceObjects();       // 텍스처 생성 및 업로드
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
        //co_await TaskSystem.EndFrame(); // 비동기 프레임 끝내기

        // 폰트 텍스처 업데이트
        io.Fonts->Build();
        //ImGui_ImplDX11_InvalidateDeviceObjects();
        //ImGui_ImplDX11_CreateDeviceObjects();      

        co_return font;  // 비동기적으로 폰트 반환
    }
    else
    {
        // 로드 실패시 로그 출력
        fprintf(stderr, "Failed to load font: %s with size: %.1f\n", fontPath.c_str(), style.FontSize);
    }

    co_return nullptr;  // 폰트 로드 실패 시 nullptr 반환
}
