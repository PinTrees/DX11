#include "pch.h"
#include "EditorUtility.h"
#include <filesystem>

namespace fs = std::filesystem;

std::wstring EditorUtility::OpenFileDialog()
{
    OPENFILENAME ofn;
    wchar_t szFile[260] = { 0 };   
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);                    // 변경: wchar_t에 맞게 크기 조정
    ofn.lpstrFilter = L"Image Files\0*.png;*.jpg\0All Files\0*.*\0";    // 변경: L"..." 사용
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameW(&ofn) == TRUE) {               // 변경: GetOpenFileNameW 사용
        std::wstring ws(szFile);
        return ws;
    }

    return L"";
}

std::wstring EditorUtility::OpenFileDialog(const std::string& initialPath)
{
    fs::path resourcePath(initialPath);
    fs::path directoryPath = resourcePath.parent_path();
    std::wstring directoryPathW = directoryPath.wstring();

    OPENFILENAME ofn;
    wchar_t szFile[260] = { 0 };
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
    ofn.lpstrFilter = L"Image Files\0*.png;*.jpg\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrInitialDir = directoryPathW.c_str();  // 초기 경로 설정
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameW(&ofn) == TRUE) {
        return std::wstring(ofn.lpstrFile);
    }

    return L"";
}
