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
    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);                    // ����: wchar_t�� �°� ũ�� ����
    ofn.lpstrFilter = L"Image Files\0*.png;*.jpg\0All Files\0*.*\0";    // ����: L"..." ���
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameW(&ofn) == TRUE) {               // ����: GetOpenFileNameW ���
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
    ofn.lpstrInitialDir = directoryPathW.c_str();  // �ʱ� ��� ����
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameW(&ofn) == TRUE) {
        return std::wstring(ofn.lpstrFile);
    }

    return L"";
}
