#pragma once

#include <Windows.h>
#include <stdio.h>
#include <vector>

bool readline(wchar_t *outputline, int maxcount, FILE *file) {
    while (fgetws(outputline, 128, file)) {
        if (*outputline != ';')
            return true;
    }
    return false;
}

unsigned short symbol_to_value(wchar_t *str) {
    if (wcslen(str) > 2) {
        if (str[0] == '0' && str[1] == 'x') {
            unsigned int res;
            swscanf(&str[2], L"%X", &res);
            return res;
        }
    }
    return str[0];
}

void writevalue(FILE *file, unsigned short value){
    fwrite(&value, 2, 1, file);
}

void skipBom(FILE *file) {
    fseek(file, 0, SEEK_END);
    unsigned int size = ftell(file);
    if (size > 1) {
        fseek(file, 0, SEEK_SET);
        unsigned short dummy;
        fread(&dummy, 2, 1, file);
        if (dummy != 0xFEFF)
            fseek(file, 0, SEEK_SET);
    }
}

bool read_wholeline(std::wstring &output, FILE *file) {
    output.clear();
    if (feof(file))
        return false;
    wint_t wc = fgetwc(file);
    while (wc != WEOF && wc != '\r' && wc != '\n') {
        output.push_back(wc);
        wc = fgetwc(file);
    }
    if (wc == '\r')
        fgetwc(file);
    if (output.size() > 2 && output[0] == ';' && output[1] == ';' && output[2] == ';')
        return read_wholeline(output, file);
    return true;
}

void read_wholefile(std::wstring &output, FILE *file) {
    output.clear();
    wint_t wc = fgetwc(file);
    while (wc != WEOF) {
        output.push_back(wc);
        wc = fgetwc(file);
    }
}

void replaceAll(std::wstring& str, const std::wstring& from, const std::wstring& to) {
    if (from.empty())
        return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::wstring::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); 
    }
}