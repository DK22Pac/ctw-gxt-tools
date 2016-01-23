#include "..\shared\bin.h"
#include <conio.h>

int main() {
    SetConsoleTitle(L"GTA Chinatown Wars TXT2BIN Converter");
    wprintf(L"GTA Chinatown Wars TXT2BIN Converter\n    by DK22\n\n");
    WIN32_FIND_DATA FindFileData;
    memset(&FindFileData, 0, sizeof(WIN32_FIND_DATA));
    HANDLE hFind = FindFirstFile(L"*.txt", &FindFileData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                wchar_t outputFileName[MAX_PATH];
                wcscpy(outputFileName, FindFileData.cFileName);
                wsprintf(&outputFileName[wcslen(outputFileName) - 4], L".bin");
                cBinFile binFile;
                wprintf(L"converting %s ... ", FindFileData.cFileName);
                if (binFile.readFromTextFile(FindFileData.cFileName)) {
                    binFile.write(outputFileName);
                    wprintf(L"done\n");
                }
                else
                    wprintf(L"failed\n");
            }
        } while (FindNextFile(hFind, &FindFileData));
        FindClose(hFind);
    }
    wprintf(L"\nConversion done. Press any key to exit.");
    _getch();
    return 1;
}