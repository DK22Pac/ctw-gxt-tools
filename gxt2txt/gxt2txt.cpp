#include "..\shared\gxt.h"
#include "..\shared\translation.h"
#include <conio.h>

int main() {
    SetConsoleTitle(L"GTA Chinatown Wars GXT2TXT Converter");
    wprintf(L"GTA Chinatown Wars GXT2TXT Converter\n    by DK22\n\n");
    readSettingsFiles();
    WIN32_FIND_DATA FindFileData;
    memset(&FindFileData, 0, sizeof(WIN32_FIND_DATA));
    HANDLE hFind = FindFirstFile(L"*.gxt", &FindFileData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                setCurrentFile(FindFileData.cFileName);
                wchar_t outputFileName[MAX_PATH];
                wcscpy(outputFileName, FindFileData.cFileName);
                wsprintf(&outputFileName[wcslen(outputFileName) - 4], L".txt");
                cGXTFile gxtFile;
                wprintf(L"converting %s ... ", FindFileData.cFileName);
                if (gxtFile.read(FindFileData.cFileName)) {
                    gxtFile.writeToTexFile(outputFileName);
                    if (gxtFile.hasWarnings)
                        wprintf(L"\n   ");
                    wprintf(L"done (%d strings)\n", gxtFile.strings.size());
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