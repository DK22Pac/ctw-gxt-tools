#pragma once

#include <Windows.h>
#include <stdio.h>

std::wstring currentFile;

void setCurrentFile(wchar_t *filename) {
    currentFile.assign(filename);
}

bool show_message(wchar_t *message, ...)
{
    wchar_t text[512];
    va_list ap;
    va_start(ap, message);
    vswprintf(text, 512, message, ap);
    va_end(ap);
    MessageBoxW(NULL, text, currentFile.c_str(), 0);
    return false;
}