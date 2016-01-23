#pragma once

#include <Windows.h>
#include <stdio.h>
#include <vector>
#include "fileop.h"
#include <io.h>
#include <fcntl.h>

struct bin_symbol {
    unsigned short width;
    unsigned short unknown1;
    unsigned short unknown2;
};

struct bin_file {
    unsigned short numSymbolsInFont;
    unsigned short fontHeight;
    // symbols...
    // bin_symbol symbols[numSymbolsInFont];
};

class cBinFile {
public:
    bool loaded;
    bin_file header;
    std::vector<bin_symbol> symbolsInfo;

    cBinFile() {
        loaded = false;
        header.fontHeight = 10;
        header.numSymbolsInFont = 0;
    }

    cBinFile(wchar_t *filepath) {
        read(filepath);
    }

    bool read(wchar_t *filepath) {
        FILE *file = _wfopen(filepath, L"rb");
        if (file) {
            fseek(file, 0, SEEK_END);
            unsigned int size = ftell(file);
            if (size < 4) {
                fclose(file);
                loaded = false;
                return false;
            }
            fseek(file, 0, SEEK_SET);
            fread(&header, 4, 1, file);
            symbolsInfo.resize(header.numSymbolsInFont);
            for (int i = 0; i < header.numSymbolsInFont; i++)
                fread(&symbolsInfo[i], 6, 1, file);
            return true;
        }
        loaded = false;
        return false;
    }

    bool readFromTextFile(wchar_t *filepath) {
        FILE *file = NULL;
        bool result = false;
        file = _wfopen(filepath, L"r");
        _setmode(_fileno(file), _O_WTEXT);
        skipBom(file);
        wchar_t line[128];
        unsigned int value[3];
        if (readline(line, 128, file) && !wcsncmp(line, L"BIN", 3)) {
            if (readline(line, 128, file) && swscanf(line, L"%*s %d", &value[0]) == 1) {
                header.fontHeight = value[0];
                if (readline(line, 128, file) && swscanf(line, L"%*s %d", &value[0]) == 1) {
                    header.numSymbolsInFont = value[0];
                    symbolsInfo.resize(header.numSymbolsInFont);
                    result = true;
                    for (int i = 0; i < header.numSymbolsInFont; i++) {
                        if (result && readline(line, 128, file) && swscanf(line, L"%d %d %d", &value[0], &value[1], &value[2]) == 3) {
                            symbolsInfo[i].width = value[0];
                            symbolsInfo[i].unknown1 = value[1];
                            symbolsInfo[i].unknown2 = value[2];
                        }
                        else
                            result = false;
                    }
                }
            }
        }
        fclose(file);
        if (!result) {
            header.numSymbolsInFont = 0;
            if (symbolsInfo.size() > 0)
                symbolsInfo.clear();
            return false;
        }
        return true;
    }

    void write(wchar_t *outputFilePath) {
        FILE *file = _wfopen(outputFilePath, L"wb");
        fwrite(&header, 4, 1, file);
        for (unsigned int i = 0; i < symbolsInfo.size(); i++)
            fwrite(&symbolsInfo[i], 6, 1, file);
        fclose(file);
    }

    void writeToTexFile(wchar_t *outputFilePath) {
        FILE *file = _wfopen(outputFilePath, L"wb");
        fwrite("\xFF\xFE", 2, 1, file);
        fwprintf(file, L"BIN\nFONT_HEIGHT  %d\nFONT_SYMBOLS %d\n;width x   y\n", header.fontHeight, header.numSymbolsInFont);
        for (unsigned int i = 0; i < symbolsInfo.size(); i++) {
            wchar_t tstr[2];
            tstr[0] = i + 32;
            tstr[1] = 0;
            fwprintf(file, L"   %-3d %-3d %-3d    ;  '%s'  (0x%X)", symbolsInfo[i].width, symbolsInfo[i].unknown1, symbolsInfo[i].unknown2, tstr, i + 32);
            if (i != (symbolsInfo.size() - 1))
                fputws(L"\r\n", file);
        }
        fclose(file);
    }
};