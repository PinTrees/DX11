#include "pch.h"
#include "EditorUtility.h"

std::string EditorUtility::OpenFileDialog()
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
        std::string filePath(ws.begin(), ws.end());     // wstring을 string으로 변환
        return filePath;
    }

    return "";
}