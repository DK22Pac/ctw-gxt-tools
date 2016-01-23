#pragma once

#include <fstream>
#include <sstream>
#include "gxt.h"
#pragma comment(lib, "urlmon")

class translation_api {
public:
    static wchar_t translatingApiKey[256];
    static wchar_t translatingLang[32];

    static std::wstring translate(std::wstring &str) {
        //replaceAll(str, L" ", L"+");
        return translate_plain_text((wchar_t *)str.c_str());
    }

    static std::wstring translate_plain_text(wchar_t *text) {
        std::wstring output;
        std::wstring address;
        address.append(L"https://translate.yandex.net/api/v1.5/tr/translate?key=");
        address.append(translatingApiKey);
        address.append(L"&text=");
        address.append(text);
        if (translatingLang[0]) {
            address.append(L"&lang=");
            address.append(translatingLang);
        }
        address.append(L"&format=plain");
        if(SUCCEEDED(URLDownloadToFile(NULL, address.c_str(), L"temp_translation.tmp", 0, NULL))){
            FILE *file = NULL;
            _wfopen_s(&file, L"temp_translation.tmp", L"r, ccs=UTF-8");
            if (file) {
                std::wstring buf_str;
                read_wholefile(buf_str, file);
                size_t start = buf_str.find(L"<text>") + 5;
                size_t end = buf_str.find(L"</text>");
                if(start != std::wstring::npos && end != std::wstring::npos && end > start)
                    output = buf_str.substr(start + 1, end - start - 1);
                fclose(file);
            }
            DeleteFile(L"temp_translation.tmp");
        }
        return output;
    }
};

wchar_t translation_api::translatingApiKey[256];
wchar_t translation_api::translatingLang[32];