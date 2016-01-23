#include "..\shared\bin.h"
#include <conio.h>

int main() {
    SetConsoleTitle(L"GTA Chinatown Wars BIN2TXT Converter");
    wprintf(L"GTA Chinatown Wars BIN2TXT Converter\n    by DK22\n\n");
    WIN32_FIND_DATA FindFileData;
    memset(&FindFileData, 0, sizeof(WIN32_FIND_DATA));
    HANDLE hFind = FindFirstFile(L"*.bin", &FindFileData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                wchar_t outputFileName[MAX_PATH];
                wcscpy(outputFileName, FindFileData.cFileName);
                wsprintf(&outputFileName[wcslen(outputFileName) - 4], L".txt");
                cBinFile binFile(FindFileData.cFileName);
                wprintf(L"converting %s ... ", FindFileData.cFileName);
                binFile.writeToTexFile(outputFileName);
                wprintf(L"done\n");
            }
        } while (FindNextFile(hFind, &FindFileData));
        FindClose(hFind);
    }
    wprintf(L"\nConversion done. Press any key to exit.");
    _getch();
    return 1;
}