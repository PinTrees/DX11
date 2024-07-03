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
    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);                    // ����: wchar_t�� �°� ũ�� ����
    ofn.lpstrFilter = L"Image Files\0*.png;*.jpg\0All Files\0*.*\0";    // ����: L"..." ���
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameW(&ofn) == TRUE) {               // ����: GetOpenFileNameW ���
        std::wstring ws(szFile);
        std::string filePath(ws.begin(), ws.end());     // wstring�� string���� ��ȯ
        return filePath;
    }

    return "";
}